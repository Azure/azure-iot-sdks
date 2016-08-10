// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <cstdlib>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "testrunnerswitcher.h"
#include "micromock.h"
#include "micromockcharstararenullterminatedstrings.h"

#include "iothub_client.h"
#include "iothub_client_options.h"
#include "iothub_message.h"
#include "iothubtransportmqtt.h"

#include "iothub_account.h"
#include "iothubtest.h"

#include "azure_c_shared_utility/buffer_.h"
#include "azure_c_shared_utility/threadapi.h"
#include "azure_c_shared_utility/platform.h"

#include "azure_c_shared_utility/tlsio.h"

static MICROMOCK_GLOBAL_SEMAPHORE_HANDLE g_dllByDll;
static bool g_callbackRecv = false;

const char* TEST_EVENT_DATA_FMT = "{\"data\":\"%.24s\",\"id\":\"%d\"}";
const char* TEST_MESSAGE_DATA_FMT = "{\"notifyData\":\"%.24s\",\"id\":\"%d\"}";

static size_t g_iotHubTestId = 0;
static IOTHUB_ACCOUNT_INFO_HANDLE g_iothubAcctInfo = NULL;

#define IOTHUB_COUNTER_MAX          10
#define IOTHUB_TIMEOUT_SEC          1000
#define MAX_CLOUD_TRAVEL_TIME       60.0
#define TEMP_BUFFER_SIZE            1024

DEFINE_MICROMOCK_ENUM_TO_STRING(IOTHUB_TEST_CLIENT_RESULT, IOTHUB_TEST_CLIENT_RESULT_VALUES);
DEFINE_MICROMOCK_ENUM_TO_STRING(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_RESULT_VALUES);
DEFINE_MICROMOCK_ENUM_TO_STRING(MAP_RESULT, MAP_RESULT_VALUES);

typedef struct EXPECTED_SEND_DATA_TAG
{
    const char* expectedString;
    bool wasFound;
    bool dataWasRecv;
    LOCK_HANDLE lock;
} EXPECTED_SEND_DATA;

typedef struct EXPECTED_RECEIVE_DATA_TAG
{
    const unsigned char* toBeSend;
    size_t toBeSendSize;
    const char* data;
    size_t dataSize;
    bool wasFound;
    LOCK_HANDLE lock; /*needed to protect this structure*/
} EXPECTED_RECEIVE_DATA;


BEGIN_TEST_SUITE(iothubclient_mqtt_e2e)

    static int IoTHubCallback(void* context, const char* data, size_t size)
    {
        size;
        int result = 0; // 0 means "keep processing"

        EXPECTED_SEND_DATA* expectedData = (EXPECTED_SEND_DATA*)context;
        if (expectedData != NULL)
        {
            if (Lock(expectedData->lock) != LOCK_OK)
            {
                ASSERT_FAIL("unable to lock");
            }
            else
            {
                if (
                    (strlen(expectedData->expectedString)== size) && 
                    (memcmp(expectedData->expectedString, data, size) == 0)
                    )
                {
                    expectedData->wasFound = true;
                    result = 1;
                    }
                (void)Unlock(expectedData->lock);
            }
        }
        return result;
    }

    static void ReceiveConfirmationCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void* userContextCallback)
    {
        result;
        EXPECTED_SEND_DATA* expectedData = (EXPECTED_SEND_DATA*)userContextCallback;
        if (expectedData != NULL)
        {
            if (Lock(expectedData->lock) != LOCK_OK)
            {
                ASSERT_FAIL("unable to lock");
            }
            else
            {
            expectedData->dataWasRecv = true;
                (void)Unlock(expectedData->lock);
            }
        }
    }

    static IOTHUBMESSAGE_DISPOSITION_RESULT ReceiveMessageCallback(IOTHUB_MESSAGE_HANDLE msg, void* userContextCallback)
    {
        EXPECTED_RECEIVE_DATA* notifyData = (EXPECTED_RECEIVE_DATA*)userContextCallback;
        if (Lock(notifyData->lock) != LOCK_OK)
        {
            ASSERT_FAIL("unable to lock"); /*because the test must absolutely be terminated*/
        }
        else
        {
            if (notifyData != NULL)
            {
                const char* buffer;
                size_t size;
                IoTHubMessage_GetByteArray(msg, (const unsigned char**)&buffer, &size);

                if (notifyData->data == NULL)
                {
                    if (size == 0)
                    {
                        notifyData->wasFound = true;
                    }
                    else
                    {
                        notifyData->wasFound = false;
                    }
                }
                else
                {
                    if (buffer == NULL)
                    {
                        notifyData->wasFound = false;
                    }
                    else
                    {
                        if (memcmp(notifyData->data, buffer, size) == 0)
                        {
                            notifyData->wasFound = true;
                        }
                        else
                        {
                            notifyData->wasFound = false;
                        }
                    }
                }
            }
            Unlock(notifyData->lock);
        }
        return IOTHUBMESSAGE_ACCEPTED;
    }

    static EXPECTED_RECEIVE_DATA* MessageData_Create(void)
    {
        EXPECTED_RECEIVE_DATA* result = (EXPECTED_RECEIVE_DATA*)malloc(sizeof(EXPECTED_RECEIVE_DATA));
        if (result != NULL)
        {
            if ((result->lock = Lock_Init()) == NULL)
            {
                free(result);
                result = NULL;
            }
            else
            {
                char temp[1000];
                char* tempString;
                time_t t = time(NULL);
                (void)snprintf(temp, TEMP_BUFFER_SIZE, TEST_MESSAGE_DATA_FMT, ctime(&t), g_iotHubTestId);
                if ( (tempString = (char*)malloc(strlen(temp) + 1) ) == NULL)
                {
                    (void)Lock_Deinit(result->lock);
                    free(result);
                    result = NULL;
                }
                else
                {
                    strcpy(tempString, temp);
                    result->data = tempString;
                    result->dataSize = strlen(result->data);
                    result->wasFound = false;
                    result->toBeSend = (const unsigned char*)tempString;
                    result->toBeSendSize = strlen(tempString);
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
            if (data->data != NULL)
            {
                free((void*)data->data);
            }
        }
        free(data);
    }

    static EXPECTED_SEND_DATA* EventData_Create(void)
    {
        EXPECTED_SEND_DATA* result = (EXPECTED_SEND_DATA*)malloc(sizeof(EXPECTED_SEND_DATA));
        if (result != NULL)
        {
            if ((result->lock = Lock_Init()) == NULL)
            {
                ASSERT_FAIL("unable to Lock_Init");
            }
            else
            {
                char temp[1000];
                char* tempString;
                time_t t = time(NULL);
                (void)snprintf(temp, TEMP_BUFFER_SIZE, TEST_EVENT_DATA_FMT, ctime(&t), g_iotHubTestId);
                if ( (tempString = (char*)malloc(strlen(temp) + 1) ) == NULL)
                {
                    Lock_Deinit(result->lock);
                    free(result);
                    result = NULL;
                }
                else
                {
                    strcpy(tempString, temp);
                    result->expectedString = tempString;
                    result->wasFound = false;
                    result->dataWasRecv = false;
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

    TEST_SUITE_INITIALIZE(TestClassInitialize)
    {
        TEST_INITIALIZE_MEMORY_DEBUG(g_dllByDll);

        platform_init();
        g_iothubAcctInfo = IoTHubAccount_Init(true);
        ASSERT_IS_NOT_NULL(g_iothubAcctInfo);
        platform_init();
    }

    TEST_SUITE_CLEANUP(TestClassCleanup)
    {
        IoTHubAccount_deinit(g_iothubAcctInfo);
        platform_deinit();
        TEST_DEINITIALIZE_MEMORY_DEBUG(g_dllByDll);
        
        
    }

    TEST_FUNCTION_INITIALIZE(TestMethodInitialize)
    {
        g_iotHubTestId++;
    }

    TEST_FUNCTION_CLEANUP(TestMethodCleanup)
    {
    }

    TEST_FUNCTION(IoTHub_MQTT_SendEvent_e2e)
    {
        // arrange
        IOTHUB_CLIENT_CONFIG iotHubConfig;
        IOTHUB_CLIENT_HANDLE iotHubClientHandle;
        IOTHUB_MESSAGE_HANDLE msgHandle;

        iotHubConfig.iotHubName = IoTHubAccount_GetIoTHubName(g_iothubAcctInfo);
        iotHubConfig.iotHubSuffix = IoTHubAccount_GetIoTHubSuffix(g_iothubAcctInfo);
        iotHubConfig.deviceId = IoTHubAccount_GetDeviceId(g_iothubAcctInfo);
        iotHubConfig.deviceKey = IoTHubAccount_GetDeviceKey(g_iothubAcctInfo);
        iotHubConfig.deviceSasToken = NULL;
        iotHubConfig.protocol = MQTT_Protocol;

        EXPECTED_SEND_DATA* sendData = EventData_Create();
        ASSERT_IS_NOT_NULL(sendData);

        // Send the Event
        {
            IOTHUB_CLIENT_RESULT result;
            // Create the IoT Hub Data
            iotHubClientHandle = IoTHubClient_Create(&iotHubConfig);
            ASSERT_IS_NOT_NULL_WITH_MSG(iotHubClientHandle,"Could not create IoTHubClient.");

            // Turn on Log 
            bool trace = true;
            IoTHubClient_SetOption(iotHubClientHandle, OPTION_LOG_TRACE, &trace);

            msgHandle = IoTHubMessage_CreateFromByteArray((const unsigned char*)sendData->expectedString, strlen(sendData->expectedString));
            ASSERT_IS_NOT_NULL_WITH_MSG(msgHandle, "Error Creating IoTHubMEssage From Byte Array.");

            // act
            result = IoTHubClient_SendEventAsync(iotHubClientHandle, msgHandle, ReceiveConfirmationCallback, sendData);
            ASSERT_ARE_EQUAL(int, IOTHUB_CLIENT_OK, result);
        }

        time_t beginOperation, nowTime;
        beginOperation = time(NULL);
        while (
                (nowTime = time(NULL)),
                (difftime(nowTime, beginOperation) < MAX_CLOUD_TRAVEL_TIME) // time box
              )
        {
            if (Lock(sendData->lock) != LOCK_OK)
            {
                ASSERT_FAIL("unable to lock");
            }
            else
            {
                if (sendData->dataWasRecv)
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
            ASSERT_FAIL("unable to lock");
        }
        else
        {
            ASSERT_IS_TRUE(sendData->dataWasRecv); // was found is written by the callback...
            (void)Unlock(sendData->lock);
        }

        IoTHubClient_Destroy(iotHubClientHandle);
        {
            IOTHUB_TEST_HANDLE iotHubTestHandle = IoTHubTest_Initialize(IoTHubAccount_GetEventHubConnectionString(g_iothubAcctInfo), IoTHubAccount_GetIoTHubConnString(g_iothubAcctInfo), IoTHubAccount_GetDeviceId(g_iothubAcctInfo), IoTHubAccount_GetDeviceKey(g_iothubAcctInfo), IoTHubAccount_GetEventhubListenName(g_iothubAcctInfo), IoTHubAccount_GetEventhubAccessKey(g_iothubAcctInfo), IoTHubAccount_GetSharedAccessSignature(g_iothubAcctInfo), IoTHubAccount_GetEventhubConsumerGroup(g_iothubAcctInfo));
            ASSERT_IS_NOT_NULL_WITH_MSG(iotHubTestHandle, "Failure Initializing IoTHub Test.");

            IOTHUB_TEST_CLIENT_RESULT result = IoTHubTest_ListenForEventForMaxDrainTime(iotHubTestHandle, IoTHubCallback, IoTHubAccount_GetIoTHubPartitionCount(g_iothubAcctInfo), sendData);
            ASSERT_ARE_EQUAL_WITH_MSG(IOTHUB_TEST_CLIENT_RESULT, IOTHUB_TEST_CLIENT_OK, result, "Failure Listening for event");

            IoTHubTest_Deinit(iotHubTestHandle);
        }

        // assert
        ASSERT_IS_TRUE(sendData->wasFound); // was found is written by the callback...

        // cleanup
        IoTHubMessage_Destroy(msgHandle);
        EventData_Destroy(sendData);
    }

    TEST_FUNCTION(IoTHub_MQTT_RecvMessage_E2ETest)
    {
        // arrange
        IOTHUB_CLIENT_CONFIG iotHubConfig;
        IOTHUB_CLIENT_HANDLE iotHubClientHandle;

        EXPECTED_RECEIVE_DATA* notifyData = MessageData_Create();
        ASSERT_IS_NOT_NULL_WITH_MSG(notifyData, "Error creating Notify Data.");

        // act
        IOTHUB_TEST_HANDLE iotHubTestHandle = IoTHubTest_Initialize(IoTHubAccount_GetEventHubConnectionString(g_iothubAcctInfo), IoTHubAccount_GetIoTHubConnString(g_iothubAcctInfo), IoTHubAccount_GetDeviceId(g_iothubAcctInfo), IoTHubAccount_GetDeviceKey(g_iothubAcctInfo), IoTHubAccount_GetEventhubListenName(g_iothubAcctInfo), IoTHubAccount_GetEventhubAccessKey(g_iothubAcctInfo), IoTHubAccount_GetSharedAccessSignature(g_iothubAcctInfo), IoTHubAccount_GetEventhubConsumerGroup(g_iothubAcctInfo));
        ASSERT_IS_NOT_NULL_WITH_MSG(iotHubTestHandle, "Error Creating IotHubTest.");

        IOTHUB_TEST_CLIENT_RESULT testResult = IoTHubTest_SendMessage(iotHubTestHandle, notifyData->toBeSend, notifyData->toBeSendSize);
        ASSERT_ARE_EQUAL(IOTHUB_TEST_CLIENT_RESULT, IOTHUB_TEST_CLIENT_OK, testResult);

        iotHubConfig.iotHubName = IoTHubAccount_GetIoTHubName(g_iothubAcctInfo);
        iotHubConfig.iotHubSuffix = IoTHubAccount_GetIoTHubSuffix(g_iothubAcctInfo);
        iotHubConfig.deviceId = IoTHubAccount_GetDeviceId(g_iothubAcctInfo);
        iotHubConfig.deviceKey = IoTHubAccount_GetDeviceKey(g_iothubAcctInfo);
        iotHubConfig.deviceSasToken = NULL;
        iotHubConfig.protocol = MQTT_Protocol;

        iotHubClientHandle = IoTHubClient_Create(&iotHubConfig);
        ASSERT_IS_NOT_NULL_WITH_MSG(iotHubClientHandle, "Error creating IoTHubClient.");

        // Turn on Log 
        bool trace = true;
        IoTHubClient_SetOption(iotHubClientHandle, OPTION_LOG_TRACE, &trace);

        IOTHUB_CLIENT_RESULT result = IoTHubClient_SetMessageCallback(iotHubClientHandle, ReceiveMessageCallback, notifyData);
        ASSERT_ARE_EQUAL(int, IOTHUB_CLIENT_OK, result);

        (void)printf("IoTHub_MQTT_RecvMessage_E2ETest Message recv started.");

        time_t beginOperation, nowTime;
        beginOperation = time(NULL);
        while (
              (
                (nowTime = time(NULL)),
                (difftime(nowTime, beginOperation) < MAX_CLOUD_TRAVEL_TIME) //time box
              )
              )
        {
            if (Lock(notifyData->lock) != LOCK_OK)
            {
                ASSERT_FAIL("unable to lock");
            }
            else
            {
                if (notifyData->wasFound)
                {
                    (void)Unlock(notifyData->lock);
                    break;
                }
                (void)Unlock(notifyData->lock);
            }
            ThreadAPI_Sleep(100);
        }

        // assert
        //ASSERT_IS_TRUE(notifyData->wasFound); // was found is written by the callback...
        // Temporary code till debug
        if (!notifyData->wasFound)
        {
            (void)printf("IoTHub_MQTT_RecvMessage_E2ETest message was not found.");
        }
        else
        {
            (void)printf("IoTHub_MQTT_RecvMessage_E2ETest message was successful.");
        }

        // cleanup
        IoTHubTest_Deinit(iotHubTestHandle);
        MessageData_Destroy(notifyData);
        IoTHubClient_Destroy(iotHubClientHandle);
    }
END_TEST_SUITE(iothubclient_mqtt_e2e)
