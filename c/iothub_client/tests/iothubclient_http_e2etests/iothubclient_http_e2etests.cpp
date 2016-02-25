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
#include "iothub_message.h"
#include "iothubtransporthttp.h"

#include "iothub_account.h"
#include "iothubtest.h"

#include "buffer_.h"
#include "threadapi.h"
#include "platform.h"

static MICROMOCK_GLOBAL_SEMAPHORE_HANDLE g_dllByDll;
static bool g_callbackRecv = false;

const char* TEST_EVENT_DATA_FMT = "{\"data\":\"%.24s\",\"id\":\"%d\"}";
const char* TEST_MESSAGE_DATA_FMT = "{\"notifyData\":\"%.24s\",\"id\":\"%d\"}";

static size_t g_iotHubTestId = 0;
static IOTHUB_ACCOUNT_INFO_HANDLE g_iothubAcctInfo = NULL;

#define IOTHUB_COUNTER_MAX           10
#define IOTHUB_TIMEOUT_SEC           1000
#define MAX_CLOUD_TRAVEL_TIME        60.0

DEFINE_MICROMOCK_ENUM_TO_STRING(IOTHUB_TEST_CLIENT_RESULT, IOTHUB_TEST_CLIENT_RESULT_VALUES);
DEFINE_MICROMOCK_ENUM_TO_STRING(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_RESULT_VALUES);
DEFINE_MICROMOCK_ENUM_TO_STRING(MAP_RESULT, MAP_RESULT_VALUES);

typedef struct EXPECTED_SEND_DATA_TAG
{
    const char* expectedString;
    bool wasFound;
    bool dataWasRecv;
} EXPECTED_SEND_DATA;

typedef struct EXPECTED_RECEIVE_DATA_TAG
{
    const unsigned char* toBeSend;
    size_t toBeSendSize;
    const char* data;
    size_t dataSize;
    bool wasFound;
} EXPECTED_RECEIVE_DATA;

static size_t IoTHub_HTTP_LL_CanSend_2000_smallest_messages_batched_nCalls;

static void IoTHub_HTTP_LL_CanSend_2000_smallest_messages_batched_Message(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void* userContextCallback)
{
    (void)(result, userContextCallback);
    IoTHub_HTTP_LL_CanSend_2000_smallest_messages_batched_nCalls++;
}

static size_t IoTHub_HTTP_LL_CanSend_2000_smallest_messages_batched_with_properties_nCalls;

static void IoTHub_HTTP_LL_CanSend_2000_smallest_messages_batched_with_properties_Message(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void* userContextCallback)
{
    (void)(result, userContextCallback);
    IoTHub_HTTP_LL_CanSend_2000_smallest_messages_batched_with_properties_nCalls++;
}

BEGIN_TEST_SUITE(iothubclient_http_e2etests)

    static int IoTHubCallback(void* context, const char* data, size_t size)
    {
        size;
        int result = 0; // 0 means "keep processing"

        EXPECTED_SEND_DATA* expectedData = (EXPECTED_SEND_DATA*)context;
        if (expectedData != NULL)
        {
            if (
                (strlen(expectedData->expectedString)== size) && 
                (memcmp(expectedData->expectedString, data, size) == 0)
                )
            {
                expectedData->wasFound = true;
                result = 1;
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
            expectedData->dataWasRecv = true;
        }
    }

    static IOTHUBMESSAGE_DISPOSITION_RESULT ReceiveMessageCallback(IOTHUB_MESSAGE_HANDLE msg, void* userContextCallback)
    {
        EXPECTED_RECEIVE_DATA* notifyData = (EXPECTED_RECEIVE_DATA*)userContextCallback;
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
        return IOTHUBMESSAGE_ACCEPTED;
    }

    static EXPECTED_RECEIVE_DATA* MessageData_Create(void)
    {
        EXPECTED_RECEIVE_DATA* result = (EXPECTED_RECEIVE_DATA*)malloc(sizeof(EXPECTED_RECEIVE_DATA));
        if (result != NULL)
        {
            char temp[1000];
            char* tempString;
            time_t t = time(NULL);
            sprintf(temp, TEST_MESSAGE_DATA_FMT, ctime(&t), g_iotHubTestId);
            if ( (tempString = (char*)malloc(strlen(temp) + 1) ) == NULL)
            {
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
        return result;
    }
    static EXPECTED_RECEIVE_DATA* NullMessageData_Create(void)
    {
        EXPECTED_RECEIVE_DATA* result = (EXPECTED_RECEIVE_DATA*)malloc(sizeof(EXPECTED_RECEIVE_DATA));
        if (result != NULL)
        {
            result->data = NULL;
            result->dataSize = 0;
            result->wasFound = false;
            result->toBeSend = NULL;
            result->toBeSendSize = 0;
        }
        return result;
    }

    static void MessageData_Destroy(EXPECTED_RECEIVE_DATA* data)
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

    static EXPECTED_SEND_DATA* EventData_Create(void)
    {
        EXPECTED_SEND_DATA* result = (EXPECTED_SEND_DATA*)malloc(sizeof(EXPECTED_SEND_DATA));
        if (result != NULL)
        {
            char temp[1000];
            char* tempString;
            time_t t = time(NULL);
            sprintf(temp, TEST_EVENT_DATA_FMT, ctime(&t), g_iotHubTestId);
            if ( (tempString = (char*)malloc(strlen(temp) + 1) ) == NULL)
            {
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
        return result;
    }

    static void EventData_Destroy(EXPECTED_SEND_DATA* data)
    {
        if (data != NULL)
        {
            if (data->expectedString != NULL)
            {
                free((void*)data->expectedString);
            }
            free(data);
        }
    }

    TEST_SUITE_INITIALIZE(TestClassInitialize)
    {
        INITIALIZE_MEMORY_DEBUG(g_dllByDll);
        platform_init();
        g_iothubAcctInfo = IoTHubAccount_Init(true);
        ASSERT_IS_NOT_NULL(g_iothubAcctInfo);
    }

    TEST_SUITE_CLEANUP(TestClassCleanup)
    {
        IoTHubAccount_deinit(g_iothubAcctInfo);
        platform_deinit();
        DEINITIALIZE_MEMORY_DEBUG(g_dllByDll);
    }

    TEST_FUNCTION_INITIALIZE(TestMethodInitialize)
    {
        g_iotHubTestId++;
    }

    TEST_FUNCTION_CLEANUP(TestMethodCleanup)
    {
    }

    TEST_FUNCTION(IoTHub_HTTP_SendEvent_E2ETests)
    {
#if 0
        // arrange
        IOTHUB_CLIENT_CONFIG iotHubConfig = { 0 };
        IOTHUB_CLIENT_HANDLE iotHubClientHandle;
        IOTHUB_MESSAGE_HANDLE msgHandle;

        iotHubConfig.iotHubName = IoTHubAccount_GetIoTHubName(g_iothubAcctInfo);
        iotHubConfig.iotHubSuffix = IoTHubAccount_GetIoTHubSuffix(g_iothubAcctInfo);
        iotHubConfig.deviceId = IoTHubAccount_GetDeviceId(g_iothubAcctInfo);
        iotHubConfig.deviceKey = IoTHubAccount_GetDeviceKey(g_iothubAcctInfo);
        iotHubConfig.protocol = HTTP_Protocol;

        EXPECTED_SEND_DATA* sendData = EventData_Create();
        ASSERT_IS_NOT_NULL_WITH_MSG(sendData, "Failure creating data to be sent");

        // Send the Event
        {
            IOTHUB_CLIENT_RESULT result;
            // Create the IoT Hub Data
            iotHubClientHandle = IoTHubClient_Create(&iotHubConfig);
            ASSERT_IS_NOT_NULL_WITH_MSG(iotHubClientHandle, "Failure creating IothubClient handle");

            msgHandle = IoTHubMessage_CreateFromByteArray((const unsigned char*)sendData->expectedString, strlen(sendData->expectedString));
            ASSERT_IS_NOT_NULL_WITH_MSG(msgHandle, "Failure to create message handle");

            // act
            result = IoTHubClient_SendEventAsync(iotHubClientHandle, msgHandle, ReceiveConfirmationCallback, sendData);
            ASSERT_ARE_EQUAL_WITH_MSG(int, IOTHUB_CLIENT_OK, result, "Failure calling IoTHubClient_SendEventAsync");
        }

        time_t beginOperation, nowTime;
        beginOperation = time(NULL);
        while (
              (
                (nowTime = time(NULL) ),
                (difftime(nowTime, beginOperation) < MAX_CLOUD_TRAVEL_TIME) // time box
              ) &&
                (!sendData->dataWasRecv) // Condition box
              )
        {
            // Just go on here
        }
        ASSERT_IS_TRUE_WITH_MSG(sendData->dataWasRecv, "Failure sending data to IotHub"); // was found is written by the callback...

        {
            IOTHUB_TEST_HANDLE iotHubTestHandle = IoTHubTest_Initialize(IoTHubAccount_GetEventHubConnectionString(g_iothubAcctInfo), IoTHubAccount_GetIoTHubConnString(g_iothubAcctInfo), IoTHubAccount_GetDeviceId(g_iothubAcctInfo), IoTHubAccount_GetDeviceKey(g_iothubAcctInfo), IoTHubAccount_GetEventhubListenName(g_iothubAcctInfo), IoTHubAccount_GetEventhubAccessKey(g_iothubAcctInfo), IoTHubAccount_GetSharedAccessSignature(g_iothubAcctInfo), IoTHubAccount_GetEventhubConsumerGroup(g_iothubAcctInfo) );
            ASSERT_IS_NOT_NULL(iotHubTestHandle);

            IOTHUB_TEST_CLIENT_RESULT result = IoTHubTest_ListenForEventForMaxDrainTime(iotHubTestHandle, IoTHubCallback, IoTHubAccount_GetIoTHubPartitionCount(g_iothubAcctInfo), sendData);
            //ASSERT_ARE_EQUAL(IOTHUB_TEST_CLIENT_RESULT, IOTHUB_TEST_CLIENT_OK, result);

            IoTHubTest_Deinit(iotHubTestHandle);
        }

        // assert
        ASSERT_IS_TRUE_WITH_MSG(sendData->wasFound, "Failure receiving data from eventhub"); // was found is written by the callback...*/

        // cleanup
        IoTHubMessage_Destroy(msgHandle);
        EventData_Destroy(sendData);

        IoTHubClient_Destroy(iotHubClientHandle);
#endif
    }

    #if 0
    TEST_FUNCTION(IoTHub_HTTP_LL_CanSend_2000_smallest_messages_batched)
    {
        IoTHub_HTTP_LL_CanSend_2000_smallest_messages_batched_nCalls = 0;

        IOTHUB_CLIENT_CONFIG iotHubConfig = { 0 };
        iotHubConfig.iotHubName = IoTHubAccount_GetIoTHubName();
        iotHubConfig.iotHubSuffix = IoTHubAccount_GetIoTHubSuffix();
        iotHubConfig.deviceId = IoTHubAccount_GetDeviceId();
        iotHubConfig.deviceKey = IoTHubAccount_GetDeviceKey();
        iotHubConfig.protocol = HTTP_Protocol;
        IOTHUB_CLIENT_LL_HANDLE iotHubClientLLHandle = IoTHubClient_LL_Create(&iotHubConfig);
        ASSERT_IS_NOT_NULL(iotHubClientLLHandle);

        bool thisIsTrue = true;
        auto res1 = IoTHubClient_LL_SetOption(iotHubClientLLHandle, "Batching", &thisIsTrue);
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK, res1);

        for (size_t i = 0;i < 2000; i++)
        {
            IOTHUB_MESSAGE_HANDLE messageHandle = IoTHubMessage_CreateFromString("a");
            ASSERT_IS_NOT_NULL(messageHandle);
            auto res2 = IoTHubClient_LL_SendEventAsync(iotHubClientLLHandle, messageHandle, IoTHub_HTTP_LL_CanSend_2000_smallest_messages_batched_Message, NULL);
            ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK, res2);
            IoTHubMessage_Destroy(messageHandle);
        }

        for (size_t i = 0;i < 300;i++)
        {
            IoTHubClient_LL_DoWork(iotHubClientLLHandle);

            IOTHUB_CLIENT_STATUS status;
            auto res5 = IoTHubClient_LL_GetSendStatus(iotHubClientLLHandle, &status);
            ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK, res5);

            ThreadAPI_Sleep(5000); //seems the service throttles @ 4 seconds

            if (status == IOTHUB_CLIENT_SEND_STATUS_IDLE)
            {
                break;
            }
        }

        ASSERT_ARE_EQUAL(size_t, 2000, IoTHub_HTTP_LL_CanSend_2000_smallest_messages_batched_nCalls);

        IoTHubClient_LL_Destroy(iotHubClientLLHandle);
    }

    TEST_FUNCTION(IoTHub_HTTP_LL_CanSend_2000_smallest_messages_batched_with_properties)
    {
        IoTHub_HTTP_LL_CanSend_2000_smallest_messages_batched_with_properties_nCalls = 0;

        IOTHUB_CLIENT_CONFIG iotHubConfig = { 0 };
        iotHubConfig.iotHubName = IoTHubAccount_GetIoTHubName();
        iotHubConfig.iotHubSuffix = IoTHubAccount_GetIoTHubSuffix();
        iotHubConfig.deviceId = IoTHubAccount_GetDeviceId();
        iotHubConfig.deviceKey = IoTHubAccount_GetDeviceKey();
        iotHubConfig.protocol = HTTP_Protocol;
        IOTHUB_CLIENT_LL_HANDLE iotHubClientLLHandle = IoTHubClient_LL_Create(&iotHubConfig);
        ASSERT_IS_NOT_NULL(iotHubClientLLHandle);

        bool thisIsTrue = true;
        auto res1 = IoTHubClient_LL_SetOption(iotHubClientLLHandle, "Batching", &thisIsTrue);
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK, res1);

        for (size_t i = 0;i < 2000; i++)
        {
            IOTHUB_MESSAGE_HANDLE messageHandle = IoTHubMessage_CreateFromString("a");
            ASSERT_IS_NOT_NULL(messageHandle);

            auto res4 = Map_AddOrUpdate(IoTHubMessage_Properties(messageHandle), "b", "c");
            ASSERT_ARE_EQUAL(MAP_RESULT, MAP_OK, res4);
            
            auto res2 = IoTHubClient_LL_SendEventAsync(iotHubClientLLHandle, messageHandle, IoTHub_HTTP_LL_CanSend_2000_smallest_messages_batched_with_properties_Message, NULL);
            ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK, res2);
            IoTHubMessage_Destroy(messageHandle);
        }

        for (size_t i = 0;i < 300;i++)
        {
            IoTHubClient_LL_DoWork(iotHubClientLLHandle);

            IOTHUB_CLIENT_STATUS status;
            auto res5 = IoTHubClient_LL_GetSendStatus(iotHubClientLLHandle, &status);
            ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK, res5);

            ThreadAPI_Sleep(5000); //seems the service throttles @ 4 seconds

            if (status == IOTHUB_CLIENT_SEND_STATUS_IDLE)
            {
                break;
            }
        }

        ASSERT_ARE_EQUAL(size_t, 2000, IoTHub_HTTP_LL_CanSend_2000_smallest_messages_batched_with_properties_nCalls);

        IoTHubClient_LL_Destroy(iotHubClientLLHandle);
    }
#endif

    TEST_FUNCTION(IoTHub_HTTP_RecvMessage_E2ETest)
    {
#if 0
        // arrange
        IOTHUB_CLIENT_CONFIG iotHubConfig = { 0 };
        IOTHUB_CLIENT_HANDLE iotHubClientHandle;

        EXPECTED_RECEIVE_DATA* notifyData = MessageData_Create();
        ASSERT_IS_NOT_NULL(notifyData);

        // act
        iotHubConfig.iotHubName = IoTHubAccount_GetIoTHubName(g_iothubAcctInfo);
        iotHubConfig.iotHubSuffix = IoTHubAccount_GetIoTHubSuffix(g_iothubAcctInfo);
        iotHubConfig.deviceId = IoTHubAccount_GetDeviceId(g_iothubAcctInfo);
        iotHubConfig.deviceKey = IoTHubAccount_GetDeviceKey(g_iothubAcctInfo);
        iotHubConfig.protocol = HTTP_Protocol;

        IOTHUB_TEST_HANDLE iotHubTestHandle = IoTHubTest_Initialize(IoTHubAccount_GetEventHubConnectionString(g_iothubAcctInfo), IoTHubAccount_GetIoTHubConnString(g_iothubAcctInfo), IoTHubAccount_GetDeviceId(g_iothubAcctInfo), IoTHubAccount_GetDeviceKey(g_iothubAcctInfo), IoTHubAccount_GetEventhubListenName(g_iothubAcctInfo), IoTHubAccount_GetEventhubAccessKey(g_iothubAcctInfo), IoTHubAccount_GetSharedAccessSignature(g_iothubAcctInfo), IoTHubAccount_GetEventhubConsumerGroup(g_iothubAcctInfo) );
        ASSERT_IS_NOT_NULL_WITH_MSG(iotHubTestHandle, "IoThubTest Failure Initializing IothubTest Item");

        IOTHUB_TEST_CLIENT_RESULT testResult = IoTHubTest_SendMessage(iotHubTestHandle, notifyData->toBeSend, notifyData->toBeSendSize);
        ASSERT_ARE_EQUAL_WITH_MSG(IOTHUB_TEST_CLIENT_RESULT, IOTHUB_TEST_CLIENT_OK, testResult, "IoThubTest Failure sending message");

        IoTHubTest_Deinit(iotHubTestHandle);

        iotHubClientHandle = IoTHubClient_Create(&iotHubConfig);
        ASSERT_IS_NOT_NULL_WITH_MSG(iotHubClientHandle, "Failure creating Iothub Client");

        IOTHUB_CLIENT_RESULT result = IoTHubClient_SetMessageCallback(iotHubClientHandle, ReceiveMessageCallback, notifyData);
        ASSERT_ARE_EQUAL_WITH_MSG(int, IOTHUB_CLIENT_OK, result, "Failure setting message callback");

        unsigned int minimumPollingTime = 0; /*because it should not wait*/
        if (IoTHubClient_SetOption(iotHubClientHandle, "MinimumPollingTime", &minimumPollingTime) != IOTHUB_CLIENT_OK)
        {
            printf("failure to set option \"MinimumPollingTime\"\r\n");
        }

        time_t beginOperation, nowTime;
        beginOperation = time(NULL);
        while (
              (
                (nowTime = time(NULL)),
                (difftime(nowTime, beginOperation) < MAX_CLOUD_TRAVEL_TIME) //time box
              ) &&
                (!notifyData->wasFound) //condition box
              )
        {
            //just go on;
        }

        // assert
        ASSERT_IS_TRUE_WITH_MSG(notifyData->wasFound, "Failure retrieving message that was sent to IotHub." ); // was found is written by the callback...

        // cleanup
        MessageData_Destroy(notifyData);
        IoTHubClient_Destroy(iotHubClientHandle);
#endif
    }

END_TEST_SUITE(iothubclient_http_e2etests)
