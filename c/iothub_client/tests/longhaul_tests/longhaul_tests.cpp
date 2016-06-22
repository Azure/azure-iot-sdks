// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.


#include <cstdlib>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include <limits.h>

#include "testrunnerswitcher.h"
#include "micromock.h"
#include "micromockcharstararenullterminatedstrings.h"

#include "iothub_client.h"
#include "iothub_message.h"
#include "iothubtransportamqp.h"

#include "iothub_account.h"
#include "iothubtest.h"

#include "azure_c_shared_utility/buffer_.h"
#include "azure_c_shared_utility/threadapi.h"
#include "azure_c_shared_utility/platform.h"
#include "azure_c_shared_utility/xlogging.h"


#ifdef MBED_BUILD_TIMESTAMP
#include "certs.h"
#endif

static MICROMOCK_GLOBAL_SEMAPHORE_HANDLE g_dllByDll;
static bool g_callbackRecv = false;

static const char* TEST_EVENT_DATA_FMT = "{\"data\":\"%.24s\",\"id\":\"%d\"}";
static const char* TEST_MESSAGE_DATA_FMT = "{\"notifyData\":\"%.24s\",\"id\":\"%d\"}";

static const int TEST_MAX_TIME_IN_SECONDS = 12 * 60 * 60; // 12 hours

static size_t g_iotHubTestId = 0;
static IOTHUB_ACCOUNT_INFO_HANDLE g_iothubAcctInfo = NULL;

#define IOTHUB_COUNTER_MAX           10
#define IOTHUB_TIMEOUT_SEC           1000
#define MAX_CLOUD_TRAVEL_TIME        60.0
#define INDEFINITE_TIME ((time_t)(-1))

typedef struct LONGHAUL_RECEIVE_TEST_PROFILE_TAG
{
	// Name of the profile or test to be run.
	char* name;
	// Total duration of the test, in seconds.
	int totalRunTimeInSeconds;
	// Number of elements in messageFrequencyInSecs.
	int numberOfMessageFrequencyVariations;
	// A list of delay intervals to be used between messages sent to the client on the long haul test. The sum of these intervals must be equal to <totalRunTimeInSeconds>
	int* messageFrequencyInSecs;
} LONGHAUL_RECEIVE_TEST_PROFILE;

typedef struct LONGHAUL_SEND_TEST_PROFILE_TAG
{
	// Name of the profile or test to be run.
	char* name;
	// Total duration of the test, in seconds.
	int totalRunTimeInSeconds;
	// Number of elements in eventFrequencyInSecs.
	int numberOfEventFrequencyVariations;
	// A list of delay intervals to be used between events sent by the long haul test. The sum of these intervals must be equal to <totalRunTimeInSeconds>
	int* eventFrequencyInSecs;
} LONGHAUL_SEND_TEST_PROFILE;

typedef struct LONGHAUL_RECEIVE_STATISTICS_TAG
{
	unsigned long numberOfMessagesReceivedByClient;
	double minMessageTravelTime;
	double maxMessageTravelTime;
	double avgMessageTravelTime;
} LONGHAUL_RECEIVE_STATISTICS;

typedef struct LONGHAUL_SEND_STATISTICS_TAG
{
	unsigned long numberOfEventsReceivedByHub;
	double minEventTravelTime;
	double maxEventTravelTime;
	double avgEventTravelTime;
} LONGHAUL_SEND_STATISTICS;

typedef struct LONGHAUL_SEND_TEST_STATE_TAG
{
	const LONGHAUL_SEND_TEST_PROFILE* profile;
	int sendFrequencyIndex;
	double timeUntilNextSendEventInSeconds;
	LONGHAUL_SEND_STATISTICS statistics;
} LONGHAUL_SEND_TEST_STATE;

typedef struct LONGHAUL_RECEIVE_TEST_STATE_TAG
{
	const LONGHAUL_RECEIVE_TEST_PROFILE* profile;
	int receiveFrequencyIndex;
	double timeUntilNextReceiveMessageInSeconds;
	LONGHAUL_RECEIVE_STATISTICS statistics;
} LONGHAUL_RECEIVE_TEST_STATE;

typedef struct EXPECTED_SEND_DATA_TAG
{
	const char* expectedString;
	bool wasFoundInHub;
	bool dataWasSent;
	time_t timeSent;
	time_t timeReceived;
	LOCK_HANDLE lock;
} EXPECTED_SEND_DATA;

typedef struct EXPECTED_RECEIVE_DATA_TAG
{
	const char* data;
	size_t dataSize;
	bool receivedByClient;
	time_t timeSent;
	time_t timeReceived;
	LOCK_HANDLE lock;
} EXPECTED_RECEIVE_DATA;

static int IoTHubCallback(void* context, const char* data, size_t size)
{
	size;
	int result = 0; // 0 means "keep processing"

	EXPECTED_SEND_DATA* expectedData = (EXPECTED_SEND_DATA*)context;
	if (expectedData != NULL)
	{
		if (Lock(expectedData->lock) != LOCK_OK)
		{
			LogError("Unable to lock on IoTHubCallback()");
		}
		else
		{
			if (
				(strlen(expectedData->expectedString) == size) &&
				(memcmp(expectedData->expectedString, data, size) == 0)
				)
			{
				expectedData->wasFoundInHub = true;

				if ((expectedData->timeReceived = time(NULL)) == INDEFINITE_TIME)
				{
					LogError("Failed setting expectedData->timeReceived (time(NULL) failed)");
				}

				result = 1;
			}
			(void)Unlock(expectedData->lock);
		}
	}
	return result;
}

static void SendConfirmationCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void* userContextCallback)
{
	(void)result;

	EXPECTED_SEND_DATA* expectedData = (EXPECTED_SEND_DATA*)userContextCallback;
	if (expectedData != NULL)
	{
		if (Lock(expectedData->lock) != LOCK_OK)
		{
			LogError("Failed locking expectedData to flag message reception.");
		}
		else
		{
			if (result != IOTHUB_CLIENT_CONFIRMATION_OK)
			{
				LogError("Failed to send event \"%s\" (client result: %d)", expectedData->expectedString, result);
				expectedData->dataWasSent = false;
			}
			else
			{
				expectedData->dataWasSent = true;

				if ((expectedData->timeSent = time(NULL)) == INDEFINITE_TIME)
				{
					LogError("Failed setting the time the event was sent (time(NULL) failed)");
				}
			}

			(void)Unlock(expectedData->lock);
		}
	}
}

static EXPECTED_RECEIVE_DATA* MessageData_Create(void)
{
	EXPECTED_RECEIVE_DATA* result;
	time_t t;

	if ((t = time(NULL)) == INDEFINITE_TIME)
	{
		LogError("Failed creating data for EXPECTED_RECEIVE_DATA (time(NULL) failed)");
		result = NULL;
	}
	else
	{
		if ((result = (EXPECTED_RECEIVE_DATA*)malloc(sizeof(EXPECTED_RECEIVE_DATA))) == NULL)
		{
			LogError("MessageData_Create failed (malloc failure)");
		}
		else
		{
			if ((result->lock = Lock_Init()) == NULL)
			{
				LogError("Failed initializing lock on MessageData_Create().");
				free(result);
				result = NULL;
			}
			else
			{
				char temp[1000];
				char* tempString;

				sprintf(temp, TEST_MESSAGE_DATA_FMT, ctime(&t), g_iotHubTestId);
				if ((tempString = (char*)malloc(strlen(temp) + 1)) == NULL)
				{
					LogError("Failed allocating memory to create EXPECTED_RECEIVE_DATA");
					(void)Lock_Deinit(result->lock);
					free(result);
					result = NULL;
				}
				else
				{
					strcpy(tempString, temp);
					result->data = tempString;
					result->dataSize = strlen(result->data);
					result->receivedByClient = false;
					result->data = (const char*)tempString;
					result->dataSize = strlen(tempString);
				}
			}
		}
	}

	return result;
}

static void MessageData_Destroy(EXPECTED_RECEIVE_DATA* data)
{
	if (data != NULL)
	{
		(void)Lock_Deinit(data->lock);
		free((void*)data->data);
	}
	free(data);
}

static EXPECTED_SEND_DATA* EventData_Create(void)
{
	EXPECTED_SEND_DATA* result;
	time_t t;

	if ((t = time(NULL)) == INDEFINITE_TIME)
	{
		LogError("Failed creating data for EXPECTED_SEND_DATA (time(NULL) failed)");
		result = NULL;
	}
	else
	{
		if ((result = (EXPECTED_SEND_DATA*)malloc(sizeof(EXPECTED_SEND_DATA))) == NULL)
		{
			LogError("EventData_Create failed (malloc failure)");
		}
		else
		{
			if ((result->lock = Lock_Init()) == NULL)
			{
				LogError("Unable to Lock_Init");
			}
			else
			{
				char temp[1000];
				char* tempString;

				sprintf(temp, TEST_EVENT_DATA_FMT, ctime(&t), g_iotHubTestId);
				if ((tempString = (char*)malloc(strlen(temp) + 1)) == NULL)
				{
					Lock_Deinit(result->lock);
					free(result);
					result = NULL;
				}
				else
				{
					strcpy(tempString, temp);
					result->expectedString = tempString;
					result->wasFoundInHub = false;
					result->dataWasSent = false;
					result->timeSent = 0;
					result->timeReceived = 0;
				}
			}
		}
	}

	return result;
}

static void EventData_Destroy(EXPECTED_SEND_DATA* data)
{
	if (data != NULL)
	{
		(void)Lock_Deinit(data->lock);
		if (data->expectedString != NULL)
		{
			free((void*)data->expectedString);
		}
		free(data);
	}
}


static void initializeSendStatistics(LONGHAUL_SEND_STATISTICS* sendStats)
{
	if (sendStats != NULL)
	{
		sendStats->numberOfEventsReceivedByHub = 0;
		sendStats->minEventTravelTime = LONG_MAX;
		sendStats->maxEventTravelTime = 0;
		sendStats->avgEventTravelTime = 0;
	}
}

static void initializeReceiveStatistics(LONGHAUL_RECEIVE_STATISTICS* receiveStats)
{
	if (receiveStats != NULL)
	{
		receiveStats->numberOfMessagesReceivedByClient = 0;
		receiveStats->minMessageTravelTime = LONG_MAX;
		receiveStats->maxMessageTravelTime = 0;
		receiveStats->avgMessageTravelTime = 0;
	}
}

static void computeSendStatistics(LONGHAUL_SEND_STATISTICS* stats, EXPECTED_SEND_DATA* sendEventData)
{
	if (sendEventData != NULL)
	{
		if (sendEventData->dataWasSent && sendEventData->wasFoundInHub)
		{
			double eventTravelTime = difftime(sendEventData->timeReceived, sendEventData->timeSent);

			if (eventTravelTime > stats->maxEventTravelTime)
			{
				stats->maxEventTravelTime = eventTravelTime;
			}

			if (eventTravelTime < stats->minEventTravelTime)
			{
				stats->minEventTravelTime = eventTravelTime;
			}

			stats->avgEventTravelTime = (stats->avgEventTravelTime * stats->numberOfEventsReceivedByHub + eventTravelTime) / (stats->numberOfEventsReceivedByHub + 1);

			stats->numberOfEventsReceivedByHub++;
		}
	}
}

static void computeReceiveStatistics(LONGHAUL_RECEIVE_STATISTICS* stats, EXPECTED_RECEIVE_DATA* receiveMessageData)
{
	if (receiveMessageData != NULL)
	{
		if (receiveMessageData->receivedByClient)
		{
			double messageTravelTime = difftime(receiveMessageData->timeReceived, receiveMessageData->timeSent);

			if (messageTravelTime > stats->maxMessageTravelTime)
			{
				stats->maxMessageTravelTime = messageTravelTime;
			}

			if (messageTravelTime < stats->minMessageTravelTime)
			{
				stats->minMessageTravelTime = messageTravelTime;
			}

			stats->avgMessageTravelTime = (stats->avgMessageTravelTime * stats->numberOfMessagesReceivedByClient + messageTravelTime) / (stats->numberOfMessagesReceivedByClient + 1);

			stats->numberOfMessagesReceivedByClient++;
		}
	}
}

static void printSendStatistics(LONGHAUL_SEND_STATISTICS* stats)
{
	LogInfo("Number of Events: Sent=%lu, Received=%lu; Travel Time (secs): Min=%f, Max=%f, Average=%f",
		stats->numberOfEventsReceivedByHub, stats->numberOfEventsReceivedByHub, stats->minEventTravelTime, stats->maxEventTravelTime, stats->avgEventTravelTime);
}

static void printReceiveStatistics(LONGHAUL_RECEIVE_STATISTICS* stats)
{
	LogInfo("Number of Messages: Sent=%lu, Received=%lu; Travel Time (secs): Min=%f, Max=%f, Average=%f",
		stats->numberOfMessagesReceivedByClient, stats->numberOfMessagesReceivedByClient, stats->minMessageTravelTime, stats->maxMessageTravelTime, stats->avgMessageTravelTime);
}

// Converts a lowercase characters in a string to uppercase.
static void strToUpperCase(char* string)
{
	if (string != NULL)
	{
		size_t length = strlen(string);
		size_t i;
		for (i = 0; i < length; i++)
		{
			// ASCII lowercase characters a (97) to z (122)
			if (*(string + i) >= 97 && *(string + i) <= 122)
			{
				// Shift 32 positions below on the ASCII table, where the equivalent uppercase letters are.
				*(string + i) -= 32;
			}
		}
	}
}

static int getLonghaulTestDurationInSeconds(char *testName, int defaultDurationInSeconds)
{
	int testDuration;
	char* envVarName;

	if ((envVarName = (char*)malloc(sizeof(char) * (strlen(testName) + 17))) == NULL)
	{
		LogError("Failed computing name of ENV VAR to read test duration (malloc failed)");
	}
	else if (sprintf(envVarName, "%s_DURATION_IN_SEC", testName) <= 0)
	{
		LogError("Failed computing name of ENV VAR to read test duration (sprintf failed)");
		free(envVarName);
	}
	else
	{
		strToUpperCase(envVarName);

#ifndef MBED_BUILD_TIMESTAMP
		char *envVarValue = getenv(envVarName);
#else
		char *envVarValue = getMbedParameter(envVarName);
#endif

		if (envVarValue != NULL)
		{
			testDuration = atoi(envVarValue);

			if (testDuration <= 0)
			{
				testDuration = defaultDurationInSeconds;
			}
		}
		else
		{
			testDuration = 0;
		}

		free(envVarName);
	}

	return testDuration;
}

#ifdef MBED_BUILD_TIMESTAMP
static int verifyEventReceivedByHub(const EXPECTED_SEND_DATA* sendData)
{
	int result;
	int response = -1;

	(void)printf("VerifyMessageReceived[%s] sent on [%s]\r\n", sendData->expectedString, ctime(&sendData->timeSent));

	if (scanf("%d", &response) <= 0)
	{
		LogError("Failed reading response from verification of message received.");
		result = __LINE__;
	}
	else
	{
		if (response == 0)
		{
			sendData->wasFoundInHub = response;
			LogError("Event not received by IoT hub within expected time.");
			result = __LINE__;
		}
		else if (response == 1)
		{
			sendData->wasFoundInHub = response;
			result = 0;
		}
		else
		{
			LogError("Failed getting result of verification of Events received by hub.");
			result = __LINE__;
		}
	}

	return result;
}
#else
static int verifyEventReceivedByHub(EXPECTED_SEND_DATA* sendData, IOTHUB_TEST_HANDLE iotHubTestHandle)
{
	int result;

	if ((IoTHubTest_ListenForEventForMaxDrainTime(iotHubTestHandle, IoTHubCallback, IoTHubAccount_GetIoTHubPartitionCount(g_iothubAcctInfo), sendData)) != IOTHUB_TEST_CLIENT_OK)
	{
		LogError("IoTHubTest_ListenForEventForMaxDrainTime failed.");
		result = __LINE__;
	}
	else 
	{
		if (Lock(sendData->lock) != LOCK_OK)
		{
			LogError("Unable to lock on verifyEventReceivedByHub()");
			result = __LINE__;
		}
		else
		{
			if (sendData->wasFoundInHub != true)
			{
				LogError("Failure verifying if data was received by Event Hub.");
				result = __LINE__;
			}
			else
			{
				result = 0;
			}
	
			(void)Unlock(sendData->lock);
		}
	}

	return result;
}
#endif

static IOTHUBMESSAGE_DISPOSITION_RESULT ReceiveMessageCallback(IOTHUB_MESSAGE_HANDLE msg, void* userContextCallback)
{
	if (userContextCallback != NULL)
	{
		EXPECTED_RECEIVE_DATA* notifyData = (EXPECTED_RECEIVE_DATA*)userContextCallback;

		if (Lock(notifyData->lock) != LOCK_OK)
		{
			LogError("Unable to lock on ReceiveMessageCallback"); /*because the test must absolutely be terminated*/
		}
		else
		{
			const char* messageId;
			const char* correlationId;
			
			const char* buffer;
			size_t size;

			if ((messageId = IoTHubMessage_GetMessageId(msg)) == NULL)
			{
				messageId = "<null>";
			}

			if ((correlationId = IoTHubMessage_GetCorrelationId(msg)) == NULL)
			{
				correlationId = "<null>";
			}

			LogInfo("Received new message from IoT Hub (message-id: %s, correlation-id: %s)", messageId, correlationId);

			if (IoTHubMessage_GetByteArray(msg, (const unsigned char**)&buffer, &size) != IOTHUB_CLIENT_OK)
			{
				LogInfo("Failed calling IoTHubMessage_GetByteArray() for new message received.");
			}
			else
			{
				if (notifyData->data == NULL)
				{
					if (size == 0)
					{
						notifyData->receivedByClient = true;

						if ((notifyData->timeReceived = time(NULL)) == INDEFINITE_TIME)
						{
							LogError("Failed setting notifyData->timeReceived (time(NULL) failed)");
						}
					}
					else
					{
						notifyData->receivedByClient = false;
					}
				}
				else
				{
					if (buffer == NULL)
					{
						notifyData->receivedByClient = false;
					}
					else
					{
						if ((size == notifyData->dataSize) && (memcmp(notifyData->data, buffer, size) == 0))
						{
							notifyData->receivedByClient = true;

							if ((notifyData->timeReceived = time(NULL)) == INDEFINITE_TIME)
							{
								LogError("Failed setting notifyData->timeReceived (time(NULL) failed)");
							}
						}
						else
						{
							notifyData->receivedByClient = false;
						}
					}
				}
			}

			Unlock(notifyData->lock);
		}
	}

	return IOTHUBMESSAGE_ACCEPTED;
}

static int sendEventLoop(IOTHUB_CLIENT_HANDLE iotHubClientHandle, LONGHAUL_SEND_TEST_STATE* test_state)
{
	int result = 0;

#ifndef MBED_BUILD_TIMESTAMP
	IOTHUB_TEST_HANDLE iotHubTestHandle;

	if ((iotHubTestHandle = IoTHubTest_Initialize(IoTHubAccount_GetEventHubConnectionString(g_iothubAcctInfo), IoTHubAccount_GetIoTHubConnString(g_iothubAcctInfo), IoTHubAccount_GetDeviceId(g_iothubAcctInfo), IoTHubAccount_GetDeviceKey(g_iothubAcctInfo), IoTHubAccount_GetEventhubListenName(g_iothubAcctInfo), IoTHubAccount_GetEventhubAccessKey(g_iothubAcctInfo), IoTHubAccount_GetSharedAccessSignature(g_iothubAcctInfo), IoTHubAccount_GetEventhubConsumerGroup(g_iothubAcctInfo))) == NULL)
	{
		LogError("Failed initializing the Event Hub test client.");
		result = __LINE__;
	}
	else
	{
		initializeSendStatistics(&test_state->statistics);
#endif
		time_t testInitialTime;

		if ((testInitialTime = time(NULL)) == INDEFINITE_TIME)
		{
			LogError("Failed setting the initial time of the test (time(NULL) failed).");
			result = __LINE__;
		}
		else
		{
			time_t loopIterationStartTimeInSeconds, loopIterationEndTimeInSeconds;
			double loopIterationTotalTime;
			time_t testCurrentTime;

			while (result == 0)
			{
				if ((testCurrentTime = time(NULL)) == INDEFINITE_TIME)
				{
					LogError("Failed setting the current time of the test (time(NULL) failed)");
					result = __LINE__;
					break;
				}
				else if (difftime(testCurrentTime, testInitialTime) > test_state->profile->totalRunTimeInSeconds)
				{
					LogInfo("Test run for the expected duration (%d seconds)", test_state->profile->totalRunTimeInSeconds);
					break;
				}

				if ((loopIterationStartTimeInSeconds = time(NULL)) == INDEFINITE_TIME)
				{
					LogError("Failed setting the initial time of the send/receive loop (time(NULL) failed)");
					result = __LINE__;
					break;
				}

				if (test_state->timeUntilNextSendEventInSeconds <= 0.0)
				{
					EXPECTED_SEND_DATA* sendData;
					IOTHUB_MESSAGE_HANDLE msgHandle;

					if ((sendData = EventData_Create()) == NULL)
					{
						LogError("Failed creating EXPECTED_SEND_DATA.");
						result = __LINE__;
					}
					else
					{
						if ((msgHandle = IoTHubMessage_CreateFromByteArray((const unsigned char*)sendData->expectedString, strlen(sendData->expectedString))) == NULL)
						{
							LogError("Failed creating IOTHUB_MESSAGE_HANDLE.");
							result = __LINE__;
						}
						else
						{
							if (IoTHubClient_SendEventAsync(iotHubClientHandle, msgHandle, SendConfirmationCallback, sendData) != IOTHUB_CLIENT_OK)
							{
								LogError("Call to IoTHubClient_SendEventAsync failed.");
								result = __LINE__;
							}
							else
							{
								bool dataWasSent = false;
								time_t beginOperation, nowTime;

								if ((beginOperation = time(NULL)) == INDEFINITE_TIME)
								{
									LogError("Failed setting beginOperation (time(NULL) failed).");
									result = __LINE__;
								}
								else
								{
									do
									{
										if (Lock(sendData->lock) != LOCK_OK)
										{
											LogError("Unable to lock to flag event sent.");
											break;
										}
										else
										{
											if (sendData->dataWasSent)
											{
												dataWasSent = true;
												Unlock(sendData->lock);
												break;
											}
											Unlock(sendData->lock);
										}
										ThreadAPI_Sleep(100);

										if ((nowTime = time(NULL)) == INDEFINITE_TIME)
										{
											LogError("Failed setting nowTime (time(NULL) failed).");
											result = __LINE__;
											break;
										}
									} while (difftime(nowTime, beginOperation) < MAX_CLOUD_TRAVEL_TIME);

									if (!dataWasSent)
									{
										LogError("Failure sending data to IotHub");
										result = __LINE__;
									}
									else
									{
#ifdef MBED_BUILD_TIMESTAMP
										if (verifyEventReceivedByHub(sendData) != 0)
										{
											result = __LINE__;
										}
										else
										{
#else
										if (verifyEventReceivedByHub(sendData, iotHubTestHandle) != 0)
										{
											result = __LINE__;
										}
										else
										{
											computeSendStatistics(&test_state->statistics, sendData);
#endif
											test_state->timeUntilNextSendEventInSeconds = test_state->profile->eventFrequencyInSecs[test_state->sendFrequencyIndex];

											if ((test_state->sendFrequencyIndex + 1) < test_state->profile->numberOfEventFrequencyVariations) test_state->sendFrequencyIndex++;
										}
									}
								}
							}

							IoTHubMessage_Destroy(msgHandle);
						}

						EventData_Destroy(sendData);
					}
				}

				ThreadAPI_Sleep(500);

				if ((loopIterationEndTimeInSeconds = time(NULL)) == INDEFINITE_TIME)
				{
					LogError("Failed setting the end time of the send loop iteration (time(NULL) failed)");
					result = __LINE__;
				}
				else
				{
					loopIterationTotalTime = difftime(loopIterationEndTimeInSeconds, loopIterationStartTimeInSeconds);
					test_state->timeUntilNextSendEventInSeconds -= loopIterationTotalTime;
				}
			} // While loop
		}
			
#ifndef MBED_BUILD_TIMESTAMP
		printSendStatistics(&test_state->statistics);

		IoTHubTest_Deinit(iotHubTestHandle);
	}
#endif

	return result;
}

static int receiveMessageLoop(IOTHUB_CLIENT_HANDLE iotHubClientHandle, LONGHAUL_RECEIVE_TEST_STATE* test_state)
{
	int result = 0;

#ifndef MBED_BUILD_TIMESTAMP
	IOTHUB_TEST_HANDLE iotHubTestHandle;

	if ((iotHubTestHandle = IoTHubTest_Initialize(IoTHubAccount_GetEventHubConnectionString(g_iothubAcctInfo), IoTHubAccount_GetIoTHubConnString(g_iothubAcctInfo), IoTHubAccount_GetDeviceId(g_iothubAcctInfo), IoTHubAccount_GetDeviceKey(g_iothubAcctInfo), IoTHubAccount_GetEventhubListenName(g_iothubAcctInfo), IoTHubAccount_GetEventhubAccessKey(g_iothubAcctInfo), IoTHubAccount_GetSharedAccessSignature(g_iothubAcctInfo), IoTHubAccount_GetEventhubConsumerGroup(g_iothubAcctInfo))) == NULL)
	{
		LogError("Failed initializing the Event Hub test client.");
		result = __LINE__;
	}
	else
	{
		initializeReceiveStatistics(&test_state->statistics);
#endif
		time_t testInitialTime;

		if ((testInitialTime = time(NULL)) == INDEFINITE_TIME)
		{
			LogError("Failed setting the initial time of the test (time(NULL) failed).");
			result = __LINE__;
		}
		else
		{
			time_t loopIterationStartTimeInSeconds, loopIterationEndTimeInSeconds;
			double loopIterationTotalTime;
			time_t testCurrentTime;

			while (result == 0)
			{
				if ((testCurrentTime = time(NULL)) == INDEFINITE_TIME)
				{
					LogError("Failed setting the current time of the test (time(NULL) failed)");
					result = __LINE__;
					break;
				}
				else if (difftime(testCurrentTime, testInitialTime) > test_state->profile->totalRunTimeInSeconds)
				{
					LogInfo("Test run for the expected duration (%d seconds)", test_state->profile->totalRunTimeInSeconds);
					break;
				}

				if ((loopIterationStartTimeInSeconds = time(NULL)) == INDEFINITE_TIME)
				{
					LogError("Failed setting the initial time of the receive loop iteration (time(NULL) failed)");
					result = __LINE__;
					break;
				}

				if (test_state->timeUntilNextReceiveMessageInSeconds <= 0.0)
				{
					EXPECTED_RECEIVE_DATA* receiveData;

					if ((receiveData = MessageData_Create()) == NULL)
					{
						LogError("Failed creating EXPECTED_RECEIVE_DATA.");
						result = __LINE__;
					}
					else
					{
						IOTHUB_TEST_CLIENT_RESULT sendResult;

						if (IoTHubClient_SetMessageCallback(iotHubClientHandle, ReceiveMessageCallback, receiveData) != IOTHUB_CLIENT_OK)
						{
							LogError("Call to IoTHubClient_SetMessageCallback failed.");
							result = __LINE__;
						}
						else if ((sendResult = IoTHubTest_SendMessage(iotHubTestHandle, (const unsigned char*)receiveData->data, receiveData->dataSize)) != IOTHUB_TEST_CLIENT_OK)
						{
							LogError("Call to IoTHubTest_SendMessage failed (%i).", sendResult);
							result = __LINE__;
						}
						else
						{
							if ((receiveData->timeSent = time(NULL)) == INDEFINITE_TIME)
							{
								LogError("Failed setting receiveData->timeSent (time(NULL) failed)");
							}

							time_t beginOperation, nowTime;
							
							if ((beginOperation = time(NULL)) == INDEFINITE_TIME)
							{
								LogError("Failed setting beginOperation (time(NULL) failed).");
								result = __LINE__;
							}
							else
							{
								do
								{
									if (Lock(receiveData->lock) != LOCK_OK)
									{
										LogError("Unable to lock to verify if C2D message has been received.");
										result = __LINE__;
										break;
									}
									else
									{
										if (receiveData->receivedByClient)
										{
											(void)Unlock(receiveData->lock);
											break;
										}
										(void)Unlock(receiveData->lock);
									}
									ThreadAPI_Sleep(100);

									if ((nowTime = time(NULL)) == INDEFINITE_TIME)
									{
										LogError("Failed setting nowTime (time(NULL) failed).");
										result = __LINE__;
										break;
									}
								} while (difftime(nowTime, beginOperation) < MAX_CLOUD_TRAVEL_TIME);

								if (result == 0)
								{
									if (!receiveData->receivedByClient)
									{
										LogError("Failure retrieving data from C2D");
										result = __LINE__;
									}
									else
									{
#ifndef MBED_BUILD_TIMESTAMP
										computeReceiveStatistics(&test_state->statistics, receiveData);
#endif
										test_state->timeUntilNextReceiveMessageInSeconds = test_state->profile->messageFrequencyInSecs[test_state->receiveFrequencyIndex];

										if ((test_state->receiveFrequencyIndex + 1) < test_state->profile->numberOfMessageFrequencyVariations) test_state->receiveFrequencyIndex++;
									}
								}
							}
						}

						MessageData_Destroy(receiveData);
					}
				}

				ThreadAPI_Sleep(500);

				if ((loopIterationEndTimeInSeconds = time(NULL)) == INDEFINITE_TIME)
				{
					LogError("Failed setting the end time of the receive loop iteration (time(NULL) failed)");
					result = __LINE__;
				}
				else
				{
					loopIterationTotalTime = difftime(loopIterationEndTimeInSeconds, loopIterationStartTimeInSeconds);
					test_state->timeUntilNextReceiveMessageInSeconds -= loopIterationTotalTime;
				}
			} // While loop
		}

#ifndef MBED_BUILD_TIMESTAMP
		printReceiveStatistics(&test_state->statistics);

		IoTHubTest_Deinit(iotHubTestHandle);
		}
#endif

	return result;
}


static int runLongHaulSendTest(const LONGHAUL_SEND_TEST_PROFILE* testProfile)
{
	int result;

	LONGHAUL_SEND_TEST_STATE test_state;
	test_state.profile = testProfile;
	test_state.sendFrequencyIndex = 0;
	test_state.timeUntilNextSendEventInSeconds = 0;
	
	LogInfo("Starting Long Haul send test \"%s\" (totalRunTimeInSeconds=%d)", test_state.profile->name, test_state.profile->totalRunTimeInSeconds);

	IOTHUB_CLIENT_CONFIG iotHubConfig;
	IOTHUB_CLIENT_HANDLE iotHubClientHandle;
#ifdef MBED_BUILD_TIMESTAMP
	IOTHUB_CLIENT_RESULT client_result;
#endif

	iotHubConfig.iotHubName = IoTHubAccount_GetIoTHubName(g_iothubAcctInfo);
	iotHubConfig.iotHubSuffix = IoTHubAccount_GetIoTHubSuffix(g_iothubAcctInfo);
	iotHubConfig.deviceId = IoTHubAccount_GetDeviceId(g_iothubAcctInfo);
	iotHubConfig.deviceKey = IoTHubAccount_GetDeviceKey(g_iothubAcctInfo);
	iotHubConfig.protocol = AMQP_Protocol;
	iotHubConfig.deviceSasToken = NULL;
	iotHubConfig.protocolGatewayHostName = NULL;

	if ((iotHubClientHandle = IoTHubClient_Create(&iotHubConfig)) == NULL)
	{
		LogError("Failed creating the IoT Hub Client.");
		result = __LINE__;
	}
#ifdef MBED_BUILD_TIMESTAMP
	else if ((client_result = IoTHubClient_SetOption(iotHubClientHandle, "TrustedCerts", certificates)) != IOTHUB_CLIENT_OK)
	{
		LogError("Failed setting certificates on IoT Hub client [%i].", client_result);
		result = __LINE__;
	}
#endif
	else 
	{
		result = sendEventLoop(iotHubClientHandle, &test_state);
	}
		
	if (iotHubClientHandle != NULL)
	{
		IoTHubClient_Destroy(iotHubClientHandle);
	}

	LogInfo("Long Haul send test \"%s\" completed (result=%d)", test_state.profile->name, result);

	return result;
}

static int runLongHaulReceiveTest(const LONGHAUL_RECEIVE_TEST_PROFILE* testProfile)
{
	int result;

	LONGHAUL_RECEIVE_TEST_STATE test_state;
	test_state.profile = testProfile;
	test_state.receiveFrequencyIndex = 0;
	test_state.timeUntilNextReceiveMessageInSeconds = 0;

	LogInfo("Starting Long Haul receive test \"%s\" (totalRunTimeInSeconds=%d)", test_state.profile->name, test_state.profile->totalRunTimeInSeconds);

	IOTHUB_CLIENT_CONFIG iotHubConfig;
	IOTHUB_CLIENT_HANDLE iotHubClientHandle;
#ifdef MBED_BUILD_TIMESTAMP
	IOTHUB_CLIENT_RESULT client_result;
#endif

	iotHubConfig.iotHubName = IoTHubAccount_GetIoTHubName(g_iothubAcctInfo);
	iotHubConfig.iotHubSuffix = IoTHubAccount_GetIoTHubSuffix(g_iothubAcctInfo);
	iotHubConfig.deviceId = IoTHubAccount_GetDeviceId(g_iothubAcctInfo);
	iotHubConfig.deviceKey = IoTHubAccount_GetDeviceKey(g_iothubAcctInfo);
	iotHubConfig.protocol = AMQP_Protocol;
	iotHubConfig.deviceSasToken = NULL;
	iotHubConfig.protocolGatewayHostName = NULL;

	if ((iotHubClientHandle = IoTHubClient_Create(&iotHubConfig)) == NULL)
	{
		LogError("Failed creating the IoT Hub Client.");
		result = __LINE__;
	}
#ifdef MBED_BUILD_TIMESTAMP
	else if ((client_result = IoTHubClient_SetOption(iotHubClientHandle, "TrustedCerts", certificates)) != IOTHUB_CLIENT_OK)
	{
		LogError("Failed setting certificates on IoT Hub client [%i].", client_result);
		result = __LINE__;
	}
#endif
	else
	{
		result = receiveMessageLoop(iotHubClientHandle, &test_state);
	}

	if (iotHubClientHandle != NULL)
	{
		IoTHubClient_Destroy(iotHubClientHandle);
	}

	LogInfo("Long Haul receive test \"%s\" completed (result=%d)", test_state.profile->name, result);

	return result;
}


BEGIN_TEST_SUITE(longhaul_tests)

	TEST_SUITE_INITIALIZE(TestClassInitialize)
	{
                TEST_INITIALIZE_MEMORY_DEBUG(g_dllByDll);

		if (platform_init() != 0)
		{
			ASSERT_FAIL("Failed initializing uAMQP platform (1st call).");
		}
		else if (platform_init() != 0)
		{
			ASSERT_FAIL("Failed initializing uAMQP platform (2nd call).");
		}
		else if (platform_init() != 0)
		{
			ASSERT_FAIL("Failed initializing uAMQP platform (3rd call).");
		}
		else if ((g_iothubAcctInfo = IoTHubAccount_Init(true, "longhaul_tests")) == NULL)
		{
			ASSERT_FAIL("Failed initializing IoTHubAccount.");
		}
		else if (platform_init() != 0)
		{
			ASSERT_FAIL("Failed initializing uAMQP platform (4th call).");
		}
	}

	TEST_SUITE_CLEANUP(TestClassCleanup)
	{
		IoTHubAccount_deinit(g_iothubAcctInfo);
		platform_deinit();
		TEST_DEINITIALIZE_MEMORY_DEBUG(g_dllByDll);
		platform_deinit();
	}

	TEST_FUNCTION_INITIALIZE(TestMethodInitialize)
	{
		g_iotHubTestId++;
	}

	TEST_FUNCTION_CLEANUP(TestMethodCleanup)
	{
	}

	TEST_FUNCTION(IoTHubClient_LongHaul_12h_Run_1_Event_Per_Sec)
	{
		int result;
		int eventFrequencyInSecs[1] = { 1 };

		LONGHAUL_SEND_TEST_PROFILE testProfile;
		testProfile.name = "IoTHubClient_LongHaul_12h_Run_1_Event_Per_Sec";
		testProfile.totalRunTimeInSeconds = getLonghaulTestDurationInSeconds(testProfile.name, TEST_MAX_TIME_IN_SECONDS);
		testProfile.numberOfEventFrequencyVariations = 1;
		testProfile.eventFrequencyInSecs = eventFrequencyInSecs;
	
		if (testProfile.totalRunTimeInSeconds > 0)
		{
			result = runLongHaulSendTest(&testProfile);
		}
		else
		{
			LogInfo("Test %s skipped (test duration was set to zero by user).", testProfile.name);
			result = 0;
		}

		ASSERT_ARE_EQUAL_WITH_MSG(int, result, 0, "Test IoTHubClient_LongHaul_12h_Run_1_Event_Per_Sec failed");
	}

	TEST_FUNCTION(IoTHubClient_LongHaul_12h_Run_1_Message_Per_Sec)
	{
		int result;
		int messageFrequencyInSecs[1] = { 1 };

		LONGHAUL_RECEIVE_TEST_PROFILE testProfile;
		testProfile.name = "IoTHubClient_LongHaul_12h_Run_1_Message_Per_Sec";
		testProfile.totalRunTimeInSeconds = getLonghaulTestDurationInSeconds(testProfile.name, TEST_MAX_TIME_IN_SECONDS);
		testProfile.numberOfMessageFrequencyVariations = 1;
		testProfile.messageFrequencyInSecs = messageFrequencyInSecs;

		if (testProfile.totalRunTimeInSeconds > 0)
		{
			result = runLongHaulReceiveTest(&testProfile);
		}
		else
		{
			LogInfo("Test %s skipped (test duration was set to zero by user).", testProfile.name);
			result = 0;
		}

		ASSERT_ARE_EQUAL_WITH_MSG(int, result, 0, "Test IoTHubClient_LongHaul_12h_Run_1_Message_Per_Sec failed");
	}

	TEST_FUNCTION(IoTHubClient_LongHaul_12h_Run_Send_Events_Expon_Delay)
	{
		int result;

		LONGHAUL_SEND_TEST_PROFILE testProfile;
		testProfile.name = "IoTHubClient_LongHaul_12h_Run_Send_Events_Expon_Delay";
		testProfile.totalRunTimeInSeconds = getLonghaulTestDurationInSeconds(testProfile.name, TEST_MAX_TIME_IN_SECONDS);;
		testProfile.numberOfEventFrequencyVariations = 14;

		if (testProfile.totalRunTimeInSeconds > 0)
		{
			int eventFrequencyInSecs[14] = { 10, 20, 40, 80, 160, 320, 640, 1280, 2560, 5120, 10240, 20480, 40960, 4400 };
			testProfile.eventFrequencyInSecs = eventFrequencyInSecs;

			result = runLongHaulSendTest(&testProfile);
		}
		else
		{
			LogInfo("Test %s skipped (test duration was set to zero by user).", testProfile.name);
			result = 0;
		}

		ASSERT_ARE_EQUAL_WITH_MSG(int, result, 0, "Test IoTHubClient_LongHaul_12h_Run_Send_Events_Expon_Delay failed");
	}

END_TEST_SUITE(longhaul_tests)