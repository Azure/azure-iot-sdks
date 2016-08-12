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
#include "iothubtransporthttp.h"
#include "iothubtransport.h"

#include "iothub_account.h"
#include "iothubtest.h"

#include "azure_c_shared_utility/buffer_.h"
#include "azure_c_shared_utility/threadapi.h"
#include "azure_c_shared_utility/platform.h"

static MICROMOCK_GLOBAL_SEMAPHORE_HANDLE g_dllByDll;
static bool g_callbackRecv = false;

const char* TEST_EVENT_DATA_FMT = "{\"data\":\"%.24s\",\"id\":\"%d\"}";
const char* TEST_MESSAGE_DATA_FMT = "{\"notifyData\":\"%.24s\",\"id\":\"%d\"}";

static size_t g_iotHubTestId = 0;
static IOTHUB_ACCOUNT_INFO_HANDLE g_iothubAcctInfo1 = NULL;
static IOTHUB_ACCOUNT_INFO_HANDLE g_iothubAcctInfo2 = NULL;
static IOTHUB_ACCOUNT_INFO_HANDLE g_iothubAcctInfo3 = NULL;

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

BEGIN_TEST_SUITE(iothubclient_http_e2e)

static int IoTHubCallback(void* context, const char* data, size_t size)
{
    EXPECTED_SEND_DATA* expectedData = (EXPECTED_SEND_DATA*)context;
    int result = 0; // 0 means "keep processing"

    if (expectedData != NULL)
    {
        if (Lock(expectedData->lock) != LOCK_OK)
        {
            ASSERT_FAIL("unable to lock");
        }
        else
        {
            if (
                (strlen(expectedData->expectedString) == size) &&
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

static int IoTHubCallbackMultipleEvents(void* context, const char* data, size_t size)
{
    EXPECTED_SEND_DATA** expectedData = (EXPECTED_SEND_DATA**)context;
    int result = 0; // 0 means "keep processing"
    bool allDataFound = false;

    if (expectedData != NULL)
    {
        allDataFound = true;
        for (size_t i = 0; i < 2; i++)
        {
            EXPECTED_SEND_DATA* expected = expectedData[i];
            if (Lock(expected->lock) != LOCK_OK)
            {
                allDataFound = false;
                ASSERT_FAIL("unable to lock");
            }
            else
            {
                if (
                    (strlen(expected->expectedString) == size) &&
                    (memcmp(expected->expectedString, data, size) == 0)
                    )
                {
                    expected->wasFound = true;
                }

                allDataFound &= expected->wasFound;
                (void)Unlock(expected->lock);
            }
        }
    }

    if (allDataFound == true)
    {
        result = 1;
    }

    return result;
}

static void ReceiveConfirmationCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void* userContextCallback)
{
    EXPECTED_SEND_DATA* expectedData = (EXPECTED_SEND_DATA*)userContextCallback;
    (void)result;
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
            sprintf(temp, TEST_MESSAGE_DATA_FMT, ctime(&t), g_iotHubTestId);
            if ((tempString = (char*)malloc(strlen(temp) + 1)) == NULL)
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
    g_iothubAcctInfo1 = IoTHubAccount_Init(true);
    ASSERT_IS_NOT_NULL(g_iothubAcctInfo1);
    g_iothubAcctInfo2 = IoTHubAccount_Init(true);
    ASSERT_IS_NOT_NULL_WITH_MSG(g_iothubAcctInfo2, "Failure to init 2nd IoTHubAccount information.");
    g_iothubAcctInfo3 = IoTHubAccount_Init(true);
    ASSERT_IS_NOT_NULL_WITH_MSG(g_iothubAcctInfo3, "Failure to init 3rd IoTHubAccount information.");
    platform_init();
}

TEST_SUITE_CLEANUP(TestClassCleanup)
{
    IoTHubAccount_deinit(g_iothubAcctInfo3);
    IoTHubAccount_deinit(g_iothubAcctInfo2);
    IoTHubAccount_deinit(g_iothubAcctInfo1);
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


TEST_FUNCTION(IoTHub_HTTP_SendEvent_e2e)
{
    // arrange
    IOTHUB_CLIENT_CONFIG iotHubConfig = { 0 };
    IOTHUB_CLIENT_HANDLE iotHubClientHandle;
    IOTHUB_MESSAGE_HANDLE msgHandle;

    iotHubConfig.iotHubName = IoTHubAccount_GetIoTHubName(g_iothubAcctInfo1);
    iotHubConfig.iotHubSuffix = IoTHubAccount_GetIoTHubSuffix(g_iothubAcctInfo1);
    iotHubConfig.deviceId = IoTHubAccount_GetDeviceId(g_iothubAcctInfo1);
    iotHubConfig.deviceKey = IoTHubAccount_GetDeviceKey(g_iothubAcctInfo1);
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
        ASSERT_ARE_EQUAL_WITH_MSG(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK, result, "Failure calling IoTHubClient_SendEventAsync");
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
        ASSERT_IS_TRUE_WITH_MSG(sendData->dataWasRecv, "Failure sending data to IotHub"); // was found is written by the callback...
        (void)Unlock(sendData->lock);
    }

    {
        IOTHUB_TEST_HANDLE iotHubTestHandle = IoTHubTest_Initialize(IoTHubAccount_GetEventHubConnectionString(g_iothubAcctInfo1), IoTHubAccount_GetIoTHubConnString(g_iothubAcctInfo1), IoTHubAccount_GetDeviceId(g_iothubAcctInfo1), IoTHubAccount_GetDeviceKey(g_iothubAcctInfo1), IoTHubAccount_GetEventhubListenName(g_iothubAcctInfo1), IoTHubAccount_GetEventhubAccessKey(g_iothubAcctInfo1), IoTHubAccount_GetSharedAccessSignature(g_iothubAcctInfo1), IoTHubAccount_GetEventhubConsumerGroup(g_iothubAcctInfo1));
        ASSERT_IS_NOT_NULL(iotHubTestHandle);

        IOTHUB_TEST_CLIENT_RESULT result = IoTHubTest_ListenForEventForMaxDrainTime(iotHubTestHandle, IoTHubCallback, IoTHubAccount_GetIoTHubPartitionCount(g_iothubAcctInfo1), sendData);
        ASSERT_ARE_EQUAL(IOTHUB_TEST_CLIENT_RESULT, IOTHUB_TEST_CLIENT_OK, result);

        IoTHubTest_Deinit(iotHubTestHandle);
    }

    // assert
    ASSERT_IS_TRUE_WITH_MSG(sendData->wasFound, "Failure receiving data from eventhub"); // was found is written by the callback...*/

                                                                                         // cleanup
    IoTHubMessage_Destroy(msgHandle);
    
    IoTHubClient_Destroy(iotHubClientHandle);
    EventData_Destroy(sendData);
}

TEST_FUNCTION(IoTHub_HTTP_SendEvent_Shared_e2e)
{
    // arrange

    TRANSPORT_HANDLE     transportHandle;


    IOTHUB_CLIENT_CONFIG iotHubConfig1 = { 0 };
    IOTHUB_CLIENT_CONFIG iotHubConfig2 = { 0 };
    IOTHUB_CLIENT_HANDLE iotHubClientHandle1;
    IOTHUB_CLIENT_HANDLE iotHubClientHandle2;
    IOTHUB_MESSAGE_HANDLE msgHandle1;
    IOTHUB_MESSAGE_HANDLE msgHandle2;

    iotHubConfig1.iotHubName = IoTHubAccount_GetIoTHubName(g_iothubAcctInfo3);
    iotHubConfig1.iotHubSuffix = IoTHubAccount_GetIoTHubSuffix(g_iothubAcctInfo3);
    iotHubConfig1.deviceId = IoTHubAccount_GetDeviceId(g_iothubAcctInfo3);
    iotHubConfig1.deviceKey = IoTHubAccount_GetDeviceKey(g_iothubAcctInfo3);
    iotHubConfig1.protocol = HTTP_Protocol;

    iotHubConfig2.iotHubName = IoTHubAccount_GetIoTHubName(g_iothubAcctInfo2);
    iotHubConfig2.iotHubSuffix = IoTHubAccount_GetIoTHubSuffix(g_iothubAcctInfo2);
    iotHubConfig2.deviceId = IoTHubAccount_GetDeviceId(g_iothubAcctInfo2);
    iotHubConfig2.deviceKey = IoTHubAccount_GetDeviceKey(g_iothubAcctInfo2);
    iotHubConfig2.protocol = HTTP_Protocol;

    EXPECTED_SEND_DATA* sendData1 = EventData_Create();
    EXPECTED_SEND_DATA* sendData2 = EventData_Create();
    ASSERT_IS_NOT_NULL_WITH_MSG(sendData1, "Failure creating set 1 data to be sent");
    ASSERT_IS_NOT_NULL_WITH_MSG(sendData2, "Failure creating set 2 data to be sent");

    // Create the transport
    {
        transportHandle = IoTHubTransport_Create(HTTP_Protocol, IoTHubAccount_GetIoTHubName(g_iothubAcctInfo3), IoTHubAccount_GetIoTHubSuffix(g_iothubAcctInfo3));
        ASSERT_IS_NOT_NULL_WITH_MSG(transportHandle, "Failure creating transport handle.");
    }

    // Send the Event device 1
    {
        IOTHUB_CLIENT_RESULT result;
        // Create the IoT Hub Data
        iotHubClientHandle1 = IoTHubClient_CreateWithTransport(transportHandle, &iotHubConfig1);
        ASSERT_IS_NOT_NULL_WITH_MSG(iotHubClientHandle1, "Failure creating IothubClient handle device 1");

        msgHandle1 = IoTHubMessage_CreateFromByteArray((const unsigned char*)sendData1->expectedString, strlen(sendData1->expectedString));
        ASSERT_IS_NOT_NULL_WITH_MSG(msgHandle1, "Failure to create message handle");

        // act
        result = IoTHubClient_SendEventAsync(iotHubClientHandle1, msgHandle1, ReceiveConfirmationCallback, sendData1);
        ASSERT_ARE_EQUAL_WITH_MSG(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK, result, "Failure calling IoTHubClient_SendEventAsync");
    }

    // Send the Event device 2
    {
        IOTHUB_CLIENT_RESULT result;
        // Create the IoT Hub Data
        iotHubClientHandle2 = IoTHubClient_CreateWithTransport(transportHandle, &iotHubConfig2);
        ASSERT_IS_NOT_NULL_WITH_MSG(iotHubClientHandle2, "Failure creating IothubClient handle device 2");

        msgHandle2 = IoTHubMessage_CreateFromByteArray((const unsigned char*)sendData2->expectedString, strlen(sendData2->expectedString));
        ASSERT_IS_NOT_NULL_WITH_MSG(msgHandle2, "Failure to create message handle");

        // act
        result = IoTHubClient_SendEventAsync(iotHubClientHandle2, msgHandle2, ReceiveConfirmationCallback, sendData2);
        ASSERT_ARE_EQUAL_WITH_MSG(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK, result, "Failure calling IoTHubClient_SendEventAsync");
    }

    time_t beginOperation, nowTime;
    beginOperation = time(NULL);
    bool dataWasRecv1 = false;
    bool dataWasRecv2 = false;

    while (
        (nowTime = time(NULL)),
        (difftime(nowTime, beginOperation) < MAX_CLOUD_TRAVEL_TIME) // time box
        )
    {
        if (Lock(sendData1->lock) != LOCK_OK)
        {
            ASSERT_FAIL("unable to lock send data 1");
        }
        else
        {
            dataWasRecv1 = sendData1->dataWasRecv; // was found is written by the callback...
            Unlock(sendData1->lock);
        }
        if (Lock(sendData2->lock) != LOCK_OK)
        {
            ASSERT_FAIL("unable to lock send data 2");
        }
        else
        {
            dataWasRecv2 = sendData2->dataWasRecv; // was found is written by the callback...
            Unlock(sendData2->lock);
        }

        if (dataWasRecv1 && dataWasRecv2)
        {
            break;
        }
        ThreadAPI_Sleep(100);
    }

    ASSERT_IS_TRUE_WITH_MSG(dataWasRecv1, "Failure sending data to IotHub, device 1");
    ASSERT_IS_TRUE_WITH_MSG(dataWasRecv2, "Failure sending data to IotHub, device 2");


    {
        EXPECTED_SEND_DATA* sendList[2];
        sendList[0] = sendData1;
        sendList[1] = sendData2;
        IOTHUB_TEST_HANDLE iotHubTestHandle1 = IoTHubTest_Initialize(IoTHubAccount_GetEventHubConnectionString(g_iothubAcctInfo3), IoTHubAccount_GetIoTHubConnString(g_iothubAcctInfo3), IoTHubAccount_GetDeviceId(g_iothubAcctInfo3), IoTHubAccount_GetDeviceKey(g_iothubAcctInfo3), IoTHubAccount_GetEventhubListenName(g_iothubAcctInfo1), IoTHubAccount_GetEventhubAccessKey(g_iothubAcctInfo1), IoTHubAccount_GetSharedAccessSignature(g_iothubAcctInfo1), IoTHubAccount_GetEventhubConsumerGroup(g_iothubAcctInfo1));
        ASSERT_IS_NOT_NULL(iotHubTestHandle1);

        IOTHUB_TEST_CLIENT_RESULT result = IoTHubTest_ListenForEventForMaxDrainTime(iotHubTestHandle1, IoTHubCallbackMultipleEvents, IoTHubAccount_GetIoTHubPartitionCount(g_iothubAcctInfo1), sendList);
        ASSERT_ARE_EQUAL(IOTHUB_TEST_CLIENT_RESULT, IOTHUB_TEST_CLIENT_OK, result);

        IoTHubTest_Deinit(iotHubTestHandle1);
    }

    // assert
    ASSERT_IS_TRUE_WITH_MSG(sendData1->wasFound, "Failure receiving client 1 data from eventhub"); // was found is written by the callback...*/
    ASSERT_IS_TRUE_WITH_MSG(sendData2->wasFound, "Failure receiving client 2 data from eventhub"); // was found is written by the callback...*/

    // cleanup
    IoTHubMessage_Destroy(msgHandle2);
    IoTHubMessage_Destroy(msgHandle1);

    IoTHubClient_Destroy(iotHubClientHandle2);
    IoTHubClient_Destroy(iotHubClientHandle1);
    IoTHubTransport_Destroy(transportHandle);
    EventData_Destroy(sendData2);
    EventData_Destroy(sendData1);

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

    for (size_t i = 0; i < 2000; i++)
    {
        IOTHUB_MESSAGE_HANDLE messageHandle = IoTHubMessage_CreateFromString("a");
        ASSERT_IS_NOT_NULL(messageHandle);
        auto res2 = IoTHubClient_LL_SendEventAsync(iotHubClientLLHandle, messageHandle, IoTHub_HTTP_LL_CanSend_2000_smallest_messages_batched_Message, NULL);
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK, res2);
        IoTHubMessage_Destroy(messageHandle);
    }

    for (size_t i = 0; i < 300; i++)
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

    for (size_t i = 0; i < 2000; i++)
    {
        IOTHUB_MESSAGE_HANDLE messageHandle = IoTHubMessage_CreateFromString("a");
        ASSERT_IS_NOT_NULL(messageHandle);

        auto res4 = Map_AddOrUpdate(IoTHubMessage_Properties(messageHandle), "b", "c");
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_OK, res4);

        auto res2 = IoTHubClient_LL_SendEventAsync(iotHubClientLLHandle, messageHandle, IoTHub_HTTP_LL_CanSend_2000_smallest_messages_batched_with_properties_Message, NULL);
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK, res2);
        IoTHubMessage_Destroy(messageHandle);
    }

    for (size_t i = 0; i < 300; i++)
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
    // arrange
    IOTHUB_CLIENT_CONFIG iotHubConfig = { 0 };
    IOTHUB_CLIENT_HANDLE iotHubClientHandle;

    EXPECTED_RECEIVE_DATA* notifyData = MessageData_Create();
    ASSERT_IS_NOT_NULL(notifyData);

    // act
    iotHubConfig.iotHubName = IoTHubAccount_GetIoTHubName(g_iothubAcctInfo1);
    iotHubConfig.iotHubSuffix = IoTHubAccount_GetIoTHubSuffix(g_iothubAcctInfo1);
    iotHubConfig.deviceId = IoTHubAccount_GetDeviceId(g_iothubAcctInfo1);
    iotHubConfig.deviceKey = IoTHubAccount_GetDeviceKey(g_iothubAcctInfo1);
    iotHubConfig.protocol = HTTP_Protocol;

    platform_init();

    IOTHUB_TEST_HANDLE iotHubTestHandle = IoTHubTest_Initialize(IoTHubAccount_GetEventHubConnectionString(g_iothubAcctInfo1), IoTHubAccount_GetIoTHubConnString(g_iothubAcctInfo1), IoTHubAccount_GetDeviceId(g_iothubAcctInfo1), IoTHubAccount_GetDeviceKey(g_iothubAcctInfo1), IoTHubAccount_GetEventhubListenName(g_iothubAcctInfo1), IoTHubAccount_GetEventhubAccessKey(g_iothubAcctInfo1), IoTHubAccount_GetSharedAccessSignature(g_iothubAcctInfo1), IoTHubAccount_GetEventhubConsumerGroup(g_iothubAcctInfo1));
    ASSERT_IS_NOT_NULL_WITH_MSG(iotHubTestHandle, "IoThubTest Failure Initializing IothubTest Item");

    IOTHUB_TEST_CLIENT_RESULT testResult = IoTHubTest_SendMessage(iotHubTestHandle, notifyData->toBeSend, notifyData->toBeSendSize);
    ASSERT_ARE_EQUAL_WITH_MSG(IOTHUB_TEST_CLIENT_RESULT, IOTHUB_TEST_CLIENT_OK, testResult, "IoThubTest Failure sending message");

    IoTHubTest_Deinit(iotHubTestHandle);

    iotHubClientHandle = IoTHubClient_Create(&iotHubConfig);
    ASSERT_IS_NOT_NULL_WITH_MSG(iotHubClientHandle, "Failure creating Iothub Client");

    IOTHUB_CLIENT_RESULT result = IoTHubClient_SetMessageCallback(iotHubClientHandle, ReceiveMessageCallback, notifyData);
    ASSERT_ARE_EQUAL_WITH_MSG(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK, result, "Failure setting message callback");

    unsigned int minimumPollingTime = 1; /*because it should not wait*/
    if (IoTHubClient_SetOption(iotHubClientHandle, OPTION_MIN_POLLING_TIME, &minimumPollingTime) != IOTHUB_CLIENT_OK)
    {
        printf("failure to set option \"MinimumPollingTime\"\r\n");
    }



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
            ASSERT_FAIL("unable ot lock");
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
    ASSERT_IS_TRUE_WITH_MSG(notifyData->wasFound, "Failure retrieving message that was sent to IotHub."); // was found is written by the callback...

                                                                                                          // cleanup
    MessageData_Destroy(notifyData);
    IoTHubClient_Destroy(iotHubClientHandle);
}

TEST_FUNCTION(IoTHub_HTTP_RecvMessage_shared_E2ETest)
{
    // arrange

    TRANSPORT_HANDLE     transportHandle;
    IOTHUB_CLIENT_CONFIG iotHubConfig1 = { 0 };
    IOTHUB_CLIENT_CONFIG iotHubConfig2 = { 0 };
    IOTHUB_CLIENT_HANDLE iotHubClientHandle1;
    IOTHUB_CLIENT_HANDLE iotHubClientHandle2;

    EXPECTED_RECEIVE_DATA* notifyData1 = MessageData_Create();
    EXPECTED_RECEIVE_DATA* notifyData2 = MessageData_Create();
    ASSERT_IS_NOT_NULL(notifyData1);
    ASSERT_IS_NOT_NULL(notifyData2);

    // act
    iotHubConfig1.iotHubName = IoTHubAccount_GetIoTHubName(g_iothubAcctInfo3);
    iotHubConfig1.iotHubSuffix = IoTHubAccount_GetIoTHubSuffix(g_iothubAcctInfo3);
    iotHubConfig1.deviceId = IoTHubAccount_GetDeviceId(g_iothubAcctInfo3);
    iotHubConfig1.deviceKey = IoTHubAccount_GetDeviceKey(g_iothubAcctInfo3);
    iotHubConfig1.protocol = HTTP_Protocol;

    iotHubConfig2.iotHubName = IoTHubAccount_GetIoTHubName(g_iothubAcctInfo2);
    iotHubConfig2.iotHubSuffix = IoTHubAccount_GetIoTHubSuffix(g_iothubAcctInfo2);
    iotHubConfig2.deviceId = IoTHubAccount_GetDeviceId(g_iothubAcctInfo2);
    iotHubConfig2.deviceKey = IoTHubAccount_GetDeviceKey(g_iothubAcctInfo2);
    iotHubConfig2.protocol = HTTP_Protocol;

    platform_init();

    // Create the transport
    {
        transportHandle = IoTHubTransport_Create(HTTP_Protocol, IoTHubAccount_GetIoTHubName(g_iothubAcctInfo3), IoTHubAccount_GetIoTHubSuffix(g_iothubAcctInfo3));
        ASSERT_IS_NOT_NULL_WITH_MSG(transportHandle, "Failure creating transport handle.");
    }


    IOTHUB_TEST_HANDLE iotHubTestHandle1 = IoTHubTest_Initialize(IoTHubAccount_GetEventHubConnectionString(g_iothubAcctInfo3), IoTHubAccount_GetIoTHubConnString(g_iothubAcctInfo3), IoTHubAccount_GetDeviceId(g_iothubAcctInfo3), IoTHubAccount_GetDeviceKey(g_iothubAcctInfo3), IoTHubAccount_GetEventhubListenName(g_iothubAcctInfo3), IoTHubAccount_GetEventhubAccessKey(g_iothubAcctInfo3), IoTHubAccount_GetSharedAccessSignature(g_iothubAcctInfo3), IoTHubAccount_GetEventhubConsumerGroup(g_iothubAcctInfo3));
    ASSERT_IS_NOT_NULL_WITH_MSG(iotHubTestHandle1, "IoThubTest Failure Initializing IothubTest Item, device 1");

    IOTHUB_TEST_CLIENT_RESULT testResult1 = IoTHubTest_SendMessage(iotHubTestHandle1, notifyData1->toBeSend, notifyData1->toBeSendSize);
    ASSERT_ARE_EQUAL_WITH_MSG(IOTHUB_TEST_CLIENT_RESULT, IOTHUB_TEST_CLIENT_OK, testResult1, "IoThubTest Failure sending message, device 1");

    IoTHubTest_Deinit(iotHubTestHandle1);

    IOTHUB_TEST_HANDLE iotHubTestHandle2 = IoTHubTest_Initialize(IoTHubAccount_GetEventHubConnectionString(g_iothubAcctInfo2), IoTHubAccount_GetIoTHubConnString(g_iothubAcctInfo2), IoTHubAccount_GetDeviceId(g_iothubAcctInfo2), IoTHubAccount_GetDeviceKey(g_iothubAcctInfo2), IoTHubAccount_GetEventhubListenName(g_iothubAcctInfo2), IoTHubAccount_GetEventhubAccessKey(g_iothubAcctInfo2), IoTHubAccount_GetSharedAccessSignature(g_iothubAcctInfo2), IoTHubAccount_GetEventhubConsumerGroup(g_iothubAcctInfo2));
    ASSERT_IS_NOT_NULL_WITH_MSG(iotHubTestHandle2, "IoThubTest Failure Initializing IothubTest Item, device 2");

    IOTHUB_TEST_CLIENT_RESULT testResult2 = IoTHubTest_SendMessage(iotHubTestHandle2, notifyData2->toBeSend, notifyData2->toBeSendSize);
    ASSERT_ARE_EQUAL_WITH_MSG(IOTHUB_TEST_CLIENT_RESULT, IOTHUB_TEST_CLIENT_OK, testResult2, "IoThubTest Failure sending message, device 2");

    IoTHubTest_Deinit(iotHubTestHandle2);

    iotHubClientHandle1 = IoTHubClient_CreateWithTransport(transportHandle, &iotHubConfig1);
    ASSERT_IS_NOT_NULL_WITH_MSG(iotHubClientHandle1, "Failure creating Iothub Client, device 1");
    iotHubClientHandle2 = IoTHubClient_CreateWithTransport(transportHandle, &iotHubConfig2);
    ASSERT_IS_NOT_NULL_WITH_MSG(iotHubClientHandle2, "Failure creating Iothub Client, device 2");

    IOTHUB_CLIENT_RESULT result1 = IoTHubClient_SetMessageCallback(iotHubClientHandle1, ReceiveMessageCallback, notifyData1);
    ASSERT_ARE_EQUAL_WITH_MSG(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK, result1, "Failure setting message callback, device 1");
    IOTHUB_CLIENT_RESULT result2 = IoTHubClient_SetMessageCallback(iotHubClientHandle2, ReceiveMessageCallback, notifyData2);
    ASSERT_ARE_EQUAL_WITH_MSG(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK, result2, "Failure setting message callback device 2");


    unsigned int minimumPollingTime = 1; /*because it should not wait*/
    if (IoTHubClient_SetOption(iotHubClientHandle1, OPTION_MIN_POLLING_TIME, &minimumPollingTime) != IOTHUB_CLIENT_OK)
    {
        printf("failure to set option \"MinimumPollingTime\"\r\n");
    }
    if (IoTHubClient_SetOption(iotHubClientHandle2, OPTION_MIN_POLLING_TIME, &minimumPollingTime) != IOTHUB_CLIENT_OK)
    {
        printf("failure to set option \"MinimumPollingTime\"\r\n");
    }


    time_t beginOperation, nowTime;
    bool wasFound1 = false;
    bool wasFound2 = false;
    beginOperation = time(NULL);
    while (
        (
            (nowTime = time(NULL)),
            (difftime(nowTime, beginOperation) < MAX_CLOUD_TRAVEL_TIME) //time box
            )
        )
    {
        if (Lock(notifyData1->lock) != LOCK_OK)
        {
            ASSERT_FAIL("unable to lock");
        }
        else
        {
            wasFound1 = notifyData1->wasFound; // was found is written by the callback...
            (void)Unlock(notifyData1->lock);
        }
        if (Lock(notifyData2->lock) != LOCK_OK)
        {
            ASSERT_FAIL("unable to lock");
        }
        else
        {
            wasFound2 = notifyData2->wasFound; // was found is written by the callback...
            (void)Unlock(notifyData2->lock);
        }

        if (wasFound1 && wasFound2)
        {
            break;
        }
        ThreadAPI_Sleep(100);
    }

    // assert
    ASSERT_IS_TRUE_WITH_MSG(wasFound1, "Failure retrieving message from client 1 that was sent to IotHub.");
    ASSERT_IS_TRUE_WITH_MSG(wasFound2, "Failure retrieving message from client 2 that was sent to IotHub.");

    // cleanup
    MessageData_Destroy(notifyData2);
    MessageData_Destroy(notifyData1);
    IoTHubClient_Destroy(iotHubClientHandle2);
    IoTHubClient_Destroy(iotHubClientHandle1);
    IoTHubTransport_Destroy(transportHandle);

}

END_TEST_SUITE(iothubclient_http_e2e)
