// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <cstdlib>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "testrunnerswitcher.h"
#include "micromock.h"
#include "micromockcharstararenullterminatedstrings.h"
#include "iot_logging.h"

#include "iothub_client.h"
#include "iothub_message.h"
#include "iothubtransportamqp.h"

#include "iothub_account.h"
#include "iothubtest.h"

#include "buffer_.h"
#include "threadapi.h"

#ifdef MBED_BUILD_TIMESTAMP
#include "certs.h"
#endif

static MICROMOCK_GLOBAL_SEMAPHORE_HANDLE g_dllByDll;
static bool g_callbackRecv = false;

const char* TEST_EVENT_DATA_FMT = "{\"data\":\"%.24s\",\"id\":\"%d\"}";
const char* TEST_MESSAGE_DATA_FMT = "{\"notifyData\":\"%.24s\",\"id\":\"%d\"}";

#define IOTHUB_COUNTER_MAX           10
#define IOTHUB_TIMEOUT_SEC           1000
#define MAX_CLOUD_TRAVEL_TIME           60.0

DEFINE_MICROMOCK_ENUM_TO_STRING(IOTHUB_TEST_CLIENT_RESULT, IOTHUB_TEST_CLIENT_RESULT_VALUES);

#define IOTHUBCLIENT_TEST_CATEGORY_LONGHAUL "IOTHUBCLIENT_TESTS_LONGHAUL"

int keepVerifyingEventReceived = 0;

typedef struct EVENT_DATA_TAG
{
    int id;
    const char* message;
    bool sent;
    bool received;
} EVENT_DATA;

typedef struct MESSAGE_DATA_TAG
{
    const char* toBeSend;
    size_t toBeSendSize;
    const char* data;
    size_t dataSize;
    bool received;
} MESSAGE_DATA;

BEGIN_TEST_SUITE(longhaul_tests)

    static int IoTHubCallback(void* context, const char* data, size_t size)
    {
        int result = 0; // 0 means "keep processing"
        (void)data, size;
        EVENT_DATA* expectedData = (EVENT_DATA*)context;
        if (expectedData != NULL)
        {
            if (strcmp(expectedData->message, data) == 0)
            {
                expectedData->received = true;
                result = 1;
            }
        }
        return result;
    }

    static void SendEventCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void* userContextCallback)
    {
        EVENT_DATA* data = (EVENT_DATA*)userContextCallback;
        (void)result;
        if (data != NULL)
        {
            data->sent = true;
        }
    }

    static int ReceiveMessageCallback(IOTHUB_MESSAGE_HANDLE msg, void* userContextCallback)
    {
        MESSAGE_DATA* notifyData = (MESSAGE_DATA*)userContextCallback;
        if (notifyData != NULL)
        {
            const char* buffer;
            size_t size;
            IoTHubMessage_GetByteArray(msg, (const unsigned char**)&buffer, &size);

            if (notifyData->data == NULL)
            {
                if (buffer == NULL)
                {
                    if (size == 0)
                    {
                        notifyData->received = true;
                    }
                    else
                    {
                        notifyData->received = false;
                    }
                }
                else
                {
                    notifyData->received = false;
                }
            }
            else
            {
                if (buffer == NULL)
                {
                    notifyData->received = false;
                }
                else
                {
                    if (memcmp(notifyData->data, buffer, size) == 0)
                    {
                        notifyData->received = true;
                    }
                    else
                    {
                        notifyData->received = false;
                    }
                }
            }
        }
        return 0;
    }

    static MESSAGE_DATA* MessageData_Create(size_t dataId)
    {
        MESSAGE_DATA* result = (MESSAGE_DATA*)malloc(sizeof(MESSAGE_DATA));
        if (result != NULL)
        {
            char temp[1000];
            char* tempString;
            time_t t = time(NULL);
            sprintf(temp, TEST_MESSAGE_DATA_FMT, ctime(&t), dataId);
            if ((tempString = (char*)malloc(strlen(temp) + 1)) == NULL)
            {
                free(result);
                result = NULL;
            }
            else
            {
                strcpy(tempString, temp);
                result->data = tempString;
                result->dataSize = strlen(result->data);
                result->received = false;
                result->toBeSend = tempString;
                result->toBeSendSize = strlen(result->toBeSend);
            }
        }
        return result;
    }
    static MESSAGE_DATA* NullMessageData_Create(void)
    {
        MESSAGE_DATA* result = (MESSAGE_DATA*)malloc(sizeof(MESSAGE_DATA));
        if (result != NULL)
        {
            result->data = NULL;
            result->dataSize = 0;
            result->received = false;
            result->toBeSend = NULL;
            result->toBeSendSize = 0;
        }
        return result;
    }

    static void MessageData_Destroy(MESSAGE_DATA* data)
    {
        if (data != NULL)
        {
            if (data->data != NULL)
            {
                free((void*)data->data);
            }
        }
        free(data);
    }

    static EVENT_DATA* EventData_CreateWithId(size_t dataId)
    {
        EVENT_DATA* result = (EVENT_DATA*)malloc(sizeof(EVENT_DATA));
        if (result != NULL)
        {
            char temp[1000];
            char* tempString;
            time_t t = time(NULL);
            sprintf(temp, TEST_EVENT_DATA_FMT, ctime(&t), dataId);
            if ((tempString = (char*)malloc(strlen(temp) + 1)) == NULL)
            {
                free(result);
                result = NULL;
            }
            else
            {
                result->id = dataId;
                strcpy(tempString, temp);
                result->message = tempString;
                result->sent = false;
                result->received = false;
            }
        }

        return result;
    }

    static EVENT_DATA* EventData_Create()
    {
        return EventData_CreateWithId(clock());
    }

    static void EventData_Destroy(EVENT_DATA* data)
    {
        if (data != NULL)
        {
            if (data->message != NULL)
            {
                free((void*)data->message);
            }

            free(data);
        }
    }

    // Description:
    //     This function blocks the current thread for the necessary ammount of time to complete 1 cycle of the expectedFrequencyInHz.
    //     If (currentTimeInMilliseconds - initialTimeInMilliseconds) is longer than expectedFrequencyInHz, the function exits without blocking. 
    // Parameters:
    //     expectedFrequencyInHz: value (in Hertz, i.e. number of cycles per second) used to calculate the duration of 1 cycle of the expected frequency (min value = 1). 
    //     initialTimeInMilliseconds: initial time of the cycle in milliseconds (as returned by 'clock() * 1000 / CLOCKS_PER_SEC').
    // Returns:
    //     Nothing.
    static void WaitForFrequencyMatch(size_t expectedFrequencyInHz, size_t initialTimeInMilliseconds)
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

    static int GetLonghaulTestDurationInSeconds(int defaultDurationInSeconds)
    {
        int testDuration = 0;

        char *envVar = getenv("IOTHUB_LONGHAUL_TEST_DURATION_IN_SECONDS");

        if (envVar != NULL)
        {
            testDuration = atoi(envVar);
        }

        if (testDuration <= 0)
        {
            testDuration = defaultDurationInSeconds;
        }

        return testDuration;
    }

    void RunLongHaulTest(int totalRunTimeInSeconds, int eventFrequencyInHz)
    {
        LogInfo("Starting Long Haul tests (totalRunTimeInSeconds=%d, eventFrequencyInHz=%d)\r\n", totalRunTimeInSeconds, eventFrequencyInHz);

        // arrange
#ifndef MBED_BUILD_TIMESTAMP
        IOTHUB_TEST_HANDLE devhubTestHandle;
#endif
        IOTHUB_CLIENT_CONFIG iotHubConfig;
        IOTHUB_CLIENT_HANDLE iotHubClientHandle;
        IOTHUB_TEST_CLIENT_RESULT clientResult;

        iotHubConfig.iotHubName = IoTHubAccount_GetIoTHubName();
        iotHubConfig.iotHubSuffix = IoTHubAccount_GetIoTHubSuffix();
        iotHubConfig.deviceId = IoTHubAccount_GetDeviceId();
        iotHubConfig.deviceKey = IoTHubAccount_GetDeviceKey();
        iotHubConfig.protocol = AMQP_Protocol;

#ifndef MBED_BUILD_TIMESTAMP
        devhubTestHandle = IoTHubTest_Initialize(IoTHubAccount_GetEventHubConnectionString(), IoTHubAccount_GetIoTHubConnString(), IoTHubAccount_GetDeviceId(), IoTHubAccount_GetDeviceKey(), IoTHubAccount_GetEventhubListenName(), IoTHubAccount_GetEventhubAccessKey(), IoTHubAccount_GetSharedAccessSignature(), IoTHubAccount_GetEventhubConsumerGroup());
        ASSERT_IS_NOT_NULL(devhubTestHandle);
#endif

        iotHubClientHandle = IoTHubClient_Create(&iotHubConfig);
        ASSERT_IS_NOT_NULL(iotHubClientHandle);
		
#ifdef MBED_BUILD_TIMESTAMP
        // For mbed add the certificate information
        if (IoTHubClient_SetOption(iotHubClientHandle, "TrustedCerts", certificates) != IOTHUB_CLIENT_OK)
        {
            LogError("failure to set option \"TrustedCerts\"\r\n");
        }
#endif

        time_t testInitialTime = time(NULL);

        // act
        while (difftime(time(NULL), testInitialTime) <= totalRunTimeInSeconds)
        {
            // arrange
            size_t loopInitialTimeInMilliseconds;
            EVENT_DATA* sendData;
            IOTHUB_MESSAGE_HANDLE msgHandle;
            IOTHUB_CLIENT_RESULT dhresult;
            time_t initialTime;

            loopInitialTimeInMilliseconds = (clock() * 1000) / CLOCKS_PER_SEC;

            sendData = EventData_Create();
            ASSERT_IS_NOT_NULL(sendData);

            msgHandle = IoTHubMessage_CreateFromByteArray((const unsigned char*)sendData->message, strlen(sendData->message));
            ASSERT_IS_NOT_NULL(msgHandle);

            // act
            dhresult = IoTHubClient_SendEventAsync(iotHubClientHandle, msgHandle, SendEventCallback, sendData);
            ASSERT_ARE_EQUAL(int, IOTHUB_CLIENT_OK, dhresult);

            initialTime = time(NULL);

            while ((difftime(time(NULL), initialTime) < MAX_CLOUD_TRAVEL_TIME) && (!sendData->sent))
            {
                // Just go on here
            }

            // assert
            ASSERT_IS_TRUE(sendData->sent); // Callback was invoked (event was sent by IoTHubClient).

#ifdef MBED_BUILD_TIMESTAMP
            (void)LogInfo("VerifyMessageReceived[%s]\r\n", sendData->message);

            int response = -1;
            scanf("%d", &response);

            if (response == 0 || response == 1)
            {
                sendData->received = response;
            }
            else
            {
                LogError("Could not verify if event hub received the message (response=%d).", response);
            }
#else
            clientResult = IoTHubTest_ListenForEventForMaxDrainTime(devhubTestHandle, IoTHubCallback, 16, sendData);
            ASSERT_ARE_EQUAL(IOTHUB_TEST_CLIENT_RESULT, IOTHUB_TEST_CLIENT_OK, clientResult);
#endif

            ASSERT_IS_TRUE(sendData->received); // Callback was invoked (event was received by IoTHub).

            IoTHubMessage_Destroy(msgHandle);
            EventData_Destroy(sendData);

            WaitForFrequencyMatch(eventFrequencyInHz, loopInitialTimeInMilliseconds);
        }

        LogInfo("1 tests ran, 0 failed, 1 succeeded.\r\n");

        // cleanup
        IoTHubClient_Destroy(iotHubClientHandle);

#ifndef MBED_BUILD_TIMESTAMP
        IoTHubTest_Deinit(devhubTestHandle);
#endif

        LogInfo("Long Haul tests completed\r\n");
    }

    TEST_SUITE_INITIALIZE(TestClassInitialize)
    {
        INITIALIZE_MEMORY_DEBUG(g_dllByDll);
    }

    TEST_SUITE_CLEANUP(TestClassCleanup)
    {
        DEINITIALIZE_MEMORY_DEBUG(g_dllByDll);
    }

    TEST_FUNCTION_INITIALIZE(TestMethodInitialize)
    {
    }

    TEST_FUNCTION_CLEANUP(TestMethodCleanup)
    {
    }

    TEST_FUNCTION(IoTHubClient_LongHaul_12h_Run_1_Event_Per_Sec)
    {
        const int TEST_MAX_TIME_IN_SECONDS = 12 * 60 * 60;
        const int EVENT_FREQUENCY_IN_HZ = 1;

        int testDuration = GetLonghaulTestDurationInSeconds(TEST_MAX_TIME_IN_SECONDS);

        RunLongHaulTest(testDuration, EVENT_FREQUENCY_IN_HZ);
    }

END_TEST_SUITE(longhaul_tests)
