// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "iothub_client.h"
#include "iothub_message.h"
#include "iothubtransportamqp.h"

#include "iothub_account.h"
#include "iothubtest.h"

#include "azure_c_shared_utility/buffer_.h"
#include "azure_c_shared_utility/threadapi.h"
#include "azure_c_shared_utility/platform.h"
#include "azure_c_shared_utility/iot_logging.h"

#ifdef MBED_BUILD_TIMESTAMP
#include "certs.h"
#endif

static bool g_callbackRecv = false;

const char* TEST_EVENT_DATA_FMT = "{\"data\":\"%.24s\",\"id\":\"%d\"}";
const char* TEST_MESSAGE_DATA_FMT = "{\"notifyData\":\"%.24s\",\"id\":\"%d\"}";

static size_t g_iotHubTestId = 0;
static IOTHUB_ACCOUNT_INFO_HANDLE g_iothubAcctInfo = NULL;

#define IOTHUB_COUNTER_MAX           10
#define IOTHUB_TIMEOUT_SEC           1000
#define MAX_CLOUD_TRAVEL_TIME        60.0

typedef struct EXPECTED_SEND_DATA_TAG
{
	const char* expectedString;
	bool wasFoundInHub;
	bool dataWasSent;
	time_t timeSent;
	time_t timeReceived;
	LOCK_HANDLE lock;
} EXPECTED_SEND_DATA;


static int IoTHubCallback(void* context, const char* data, size_t size)
{
	size;
	int result = 0; // 0 means "keep processing"

	EXPECTED_SEND_DATA* expectedData = (EXPECTED_SEND_DATA*)context;
	if (expectedData != NULL)
	{
		if (Lock(expectedData->lock) != LOCK_OK)
		{
			// ASSERT_FAIL("unable to lock");
		}
		else
		{
			if (
				(strlen(expectedData->expectedString) == size) &&
				(memcmp(expectedData->expectedString, data, size) == 0)
				)
			{
				expectedData->wasFoundInHub = true;
				expectedData->timeReceived = time(NULL);
				result = 1;
			}
			(void)Unlock(expectedData->lock);
		}
	}
	return result;
}

static void SendConfirmationCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void* userContextCallback)
{
	result;
	EXPECTED_SEND_DATA* expectedData = (EXPECTED_SEND_DATA*)userContextCallback;
	if (expectedData != NULL)
	{
		if (Lock(expectedData->lock) != LOCK_OK)
		{
			LogError("Failed locking expectedData to flag message reception.");
		}
		else
		{
			expectedData->dataWasSent = true;
			expectedData->timeSent = time(NULL);
			(void)Unlock(expectedData->lock);
		}
	}
}

static EXPECTED_SEND_DATA* EventData_Create(void)
{
	EXPECTED_SEND_DATA* result = (EXPECTED_SEND_DATA*)malloc(sizeof(EXPECTED_SEND_DATA));
	if (result != NULL)
	{
		if ((result->lock = Lock_Init()) == NULL)
		{
			LogError("Unable to Lock_Init");
		}
		else
		{
			char temp[1000];
			char* tempString;
			time_t t = time(NULL);
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

typedef struct LONGHAUL_STATISTICS_TAG
{
	double numberOfEventsReceivedByHub;
	double minEventTravelTime;
	double maxEventTravelTime;
	double avgEventTravelTime;
} LONGHAUL_STATISTICS;

static LONGHAUL_STATISTICS* initializeStatistics()
{
	LONGHAUL_STATISTICS* stats = (LONGHAUL_STATISTICS*)malloc(sizeof(LONGHAUL_STATISTICS));

	if (stats != NULL)
	{
		stats->numberOfEventsReceivedByHub = 0;
		stats->minEventTravelTime = LONG_MAX;
		stats->maxEventTravelTime = 0;
		stats->avgEventTravelTime = 0;
	}

	return stats;
}

static void computeStatistics(LONGHAUL_STATISTICS* stats, EXPECTED_SEND_DATA* data)
{
	if (data->dataWasSent && data->wasFoundInHub)
	{
		double eventTravelTime = difftime(data->timeReceived, data->timeSent);

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

static void printStatistics(LONGHAUL_STATISTICS* stats)
{
	LogInfo("Number of Events: Sent=%f, Received=%f; Travel Time (secs): Min=%f, Max=%f, Average=%f",
		stats->numberOfEventsReceivedByHub, stats->numberOfEventsReceivedByHub, stats->minEventTravelTime, stats->maxEventTravelTime, stats->avgEventTravelTime);
}

static void destroyStatistics(LONGHAUL_STATISTICS* stats)
{
	free(stats);
}

static int getLonghaulTestDurationInSeconds(int defaultDurationInSeconds)
{
	int testDuration = 0;

#ifndef MBED_BUILD_TIMESTAMP
	char *envVar = getenv("IOTHUB_LONGHAUL_TEST_DURATION_IN_SECONDS");

	if (envVar != NULL)
	{
		testDuration = atoi(envVar);
	}
#endif

	if (testDuration <= 0)
	{
		testDuration = defaultDurationInSeconds;
	}

	return testDuration;
}

// Description:
//     This function blocks the current thread for the necessary ammount of time to complete 1 cycle of the expectedFrequencyInHz.
//     If (currentTimeInMilliseconds - initialTimeInMilliseconds) is longer than expectedFrequencyInHz, the function exits without blocking. 
// Parameters:
//     expectedFrequencyInHz: value (in Hertz, i.e. number of cycles per second) used to calculate the duration of 1 cycle of the expected frequency (min value = 1). 
//     initialTimeInMilliseconds: initial time of the cycle in milliseconds (as returned by 'clock() * 1000 / CLOCKS_PER_SEC').
// Returns:
//     Nothing.
static void waitForFrequencyMatch(size_t expectedFrequencyInHz, size_t initialTimeInMilliseconds)
{
	if (expectedFrequencyInHz == 0) expectedFrequencyInHz = 1;

	size_t cycleExpectedTimeInMilliseconds = 1000 / expectedFrequencyInHz;

	size_t currentTimeInMilliseconds = (clock() * 1000) / CLOCKS_PER_SEC;

	if ((currentTimeInMilliseconds - initialTimeInMilliseconds) < cycleExpectedTimeInMilliseconds)
	{
		size_t cycleRemainingTimeInMilliseconds = cycleExpectedTimeInMilliseconds - (currentTimeInMilliseconds - initialTimeInMilliseconds);

		ThreadAPI_Sleep(cycleRemainingTimeInMilliseconds);
	}
}

#ifdef MBED_BUILD_TIMESTAMP
static int verifyEventReceivedByHub(EXPECTED_SEND_DATA* sendData)
{
	int result;

	(void)printf("VerifyMessageReceived[%s] sent on [%s]\r\n", sendData->expectedString, ctime(&sendData->timeSent));

	int response = -1;
	scanf("%d", &response);

	if (response == 0 || response == 1)
	{
		sendData->wasFoundInHub = response;

		if (response == 0)
		{
			LogError("Event not received by IoT hub within expected time.");
			result = __LINE__;
		}
		else
		{
			result = 0;
		}
	}
	else
	{
		LogError("Failed getting result of verification of Events received by hub.");
		result = __LINE__;
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
	else if (sendData->wasFoundInHub != true)
	{
		LogError("Failure verifying if data was received by Event Hub.");
		result = __LINE__;
	}
	else
	{
		result = 0;
	}

	return result;
}
#endif


static int testSuiteInitialize()
{
	int result;

	if (platform_init() != 0)
	{
		LogError("Failed initializing uAMQP platform (1st call).");
		result = __LINE__;
	}
	else if (platform_init() != 0)
	{
		LogError("Failed initializing uAMQP platform (2nd call).");
		result = __LINE__;
	}
	else if (platform_init() != 0)
	{
		LogError("Failed initializing uAMQP platform (3rd call).");
		result = __LINE__;
	}
	else if ((g_iothubAcctInfo = IoTHubAccount_Init(true, "longhaul_tests")) == NULL)
	{
		LogError("Failed initializing IoTHubAccount.");
		result = __LINE__;
	}
	else if (platform_init() != 0)
	{
		LogError("Failed initializing uAMQP platform (4th call).");
		result = __LINE__;
	}
	else
	{
		result = 0;
	}

	return result;
}

static void testSuiteCleanup()
{
	IoTHubAccount_deinit(g_iothubAcctInfo);
	// Need a double deinit
	platform_deinit();
	platform_deinit();
}

static int testFunctionInitialize(int totalRunTimeInSeconds, int eventFrequencyInHz)
{
	int result;

	LogInfo("Starting Long Haul tests (totalRunTimeInSeconds=%d, eventFrequencyInHz=%d)\r\n", totalRunTimeInSeconds, eventFrequencyInHz);

	g_iotHubTestId++;
	result = 0;

	return result;
}

static int sendEventLoop(IOTHUB_CLIENT_HANDLE iotHubClientHandle, int totalRunTimeInSeconds, int eventFrequencyInHz)
{
	int result;

#ifndef MBED_BUILD_TIMESTAMP
	IOTHUB_TEST_HANDLE iotHubTestHandle;
	LONGHAUL_STATISTICS *stats;

	if ((iotHubTestHandle = IoTHubTest_Initialize(IoTHubAccount_GetEventHubConnectionString(g_iothubAcctInfo), IoTHubAccount_GetIoTHubConnString(g_iothubAcctInfo), IoTHubAccount_GetDeviceId(g_iothubAcctInfo), IoTHubAccount_GetDeviceKey(g_iothubAcctInfo), IoTHubAccount_GetEventhubListenName(g_iothubAcctInfo), IoTHubAccount_GetEventhubAccessKey(g_iothubAcctInfo), IoTHubAccount_GetSharedAccessSignature(g_iothubAcctInfo), IoTHubAccount_GetEventhubConsumerGroup(g_iothubAcctInfo))) == NULL)
	{
		LogError("Failed initializing the Event Hub listener.");
		result = __LINE__;
	}
	else
	{
		if ((stats = initializeStatistics()) == NULL)
		{
			LogError("Failed creating the container to track statistics.");
			result = __LINE__;
		}
		else
		{
#endif
			result = 0;
						
			time_t testInitialTime = time(NULL);

			while (result == 0 &&
				difftime(time(NULL), testInitialTime) <= totalRunTimeInSeconds)
			{
				size_t loopInitialTimeInMilliseconds;
				EXPECTED_SEND_DATA* sendData;
				IOTHUB_MESSAGE_HANDLE msgHandle;

				loopInitialTimeInMilliseconds = (clock() * 1000) / CLOCKS_PER_SEC;

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
						if ((result = IoTHubClient_SendEventAsync(iotHubClientHandle, msgHandle, SendConfirmationCallback, sendData)) != IOTHUB_CLIENT_OK)
						{
							LogError("Call to IoTHubClient_SendEventAsync failed.");
							result = __LINE__;
						}
						else
						{

							time_t beginOperation, nowTime;
							beginOperation = time(NULL);
							while (
								(nowTime = time(NULL)),
								(difftime(nowTime, beginOperation) < MAX_CLOUD_TRAVEL_TIME) // time box
								)
							{
								if (Lock(sendData->lock) != LOCK_OK)
								{
									LogError("Unable to lock to flag event sent.");
								}
								else
								{
									if (sendData->dataWasSent)
									{
										Unlock(sendData->lock);
										break;
									}
									Unlock(sendData->lock);
								}
								ThreadAPI_Sleep(100);
							}

							if (Lock(sendData->lock) != LOCK_OK)
							{
								LogError("Unable to lock to verify if event was sent.");
								result = __LINE__;
							}
							else
							{
								if (!sendData->dataWasSent)
								{
									LogError("Failure sending data to IotHub");
									result = __LINE__;
								}
								(void)Unlock(sendData->lock);
							}

							if (result == 0)
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
									computeStatistics(stats, sendData);
#endif
									waitForFrequencyMatch(eventFrequencyInHz, loopInitialTimeInMilliseconds);
								}
							}
						}
				
						IoTHubMessage_Destroy(msgHandle);
					}
				
					EventData_Destroy(sendData);
				}
			} // While loop
			
#ifndef MBED_BUILD_TIMESTAMP
			printStatistics(stats);
			destroyStatistics(stats);
		}

		IoTHubTest_Deinit(iotHubTestHandle);
	}
#endif

	return result;
}


static int runLongHaulTest(int totalRunTimeInSeconds, int eventFrequencyInHz)
{
	int result;
	
	if (testSuiteInitialize() != 0 || testFunctionInitialize(totalRunTimeInSeconds, eventFrequencyInHz) != 0)
	{
		result = __LINE__;
	}
	else
	{
		IOTHUB_CLIENT_CONFIG iotHubConfig = { 0 };
		IOTHUB_CLIENT_HANDLE iotHubClientHandle;
		IOTHUB_CLIENT_RESULT client_result;

		iotHubConfig.iotHubName = IoTHubAccount_GetIoTHubName(g_iothubAcctInfo);
		iotHubConfig.iotHubSuffix = IoTHubAccount_GetIoTHubSuffix(g_iothubAcctInfo);
		iotHubConfig.deviceId = IoTHubAccount_GetDeviceId(g_iothubAcctInfo);
		iotHubConfig.deviceKey = IoTHubAccount_GetDeviceKey(g_iothubAcctInfo);
		iotHubConfig.protocol = AMQP_Protocol;

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
			result = sendEventLoop(iotHubClientHandle, totalRunTimeInSeconds, eventFrequencyInHz);
		}
		
		IoTHubClient_Destroy(iotHubClientHandle);
	}

	LogInfo("Long Haul tests completed\r\n");

	testSuiteCleanup();
	
	return result;
}


int IoTHubClient_LongHaul_12h_Run_1_Event_Per_Sec(void)
{
	const int TEST_MAX_TIME_IN_SECONDS = 12 * 60 * 60;
	const int EVENT_FREQUENCY_IN_HZ = 1;

	int testDuration = getLonghaulTestDurationInSeconds(TEST_MAX_TIME_IN_SECONDS);
	
	return runLongHaulTest(testDuration, EVENT_FREQUENCY_IN_HZ);
}

