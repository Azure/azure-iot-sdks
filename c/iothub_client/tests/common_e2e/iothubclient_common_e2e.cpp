// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <cstdlib>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "testrunnerswitcher.h"

#include "iothub_client.h"
#include "iothub_client_options.h"
#include "iothub_message.h"

#include "iothub_account.h"
#include "iothubtest.h"

#include "azure_c_shared_utility/buffer_.h"
#include "azure_c_shared_utility/threadapi.h"
#include "azure_c_shared_utility/platform.h"
#include "../../../certs/certs.h"

static bool g_callbackRecv = false;

const char* TEST_EVENT_DATA_FMT = "{\"data\":\"%.24s\",\"id\":\"%d\"}";
const char* TEST_MESSAGE_DATA_FMT = "{\"notifyData\":\"%.24s\",\"id\":\"%d\"}";

static size_t g_iotHubTestId = 0;
static IOTHUB_ACCOUNT_INFO_HANDLE g_iothubAcctInfo = NULL;

#define IOTHUB_COUNTER_MAX           10
#define IOTHUB_TIMEOUT_SEC           1000
#define MAX_CLOUD_TRAVEL_TIME        60.0

TEST_DEFINE_ENUM_TYPE(IOTHUB_TEST_CLIENT_RESULT, IOTHUB_TEST_CLIENT_RESULT_VALUES);
TEST_DEFINE_ENUM_TYPE(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_RESULT_VALUES);

typedef struct EXPECTED_SEND_DATA_TAG
{
    const char* expectedString;
    bool wasFound;
    bool dataWasRecv;
    LOCK_HANDLE lock;
} EXPECTED_SEND_DATA;

typedef struct EXPECTED_RECEIVE_DATA_TAG
{
    const char* toBeSend;
    size_t toBeSendSize;
    const char* data;
    size_t dataSize;
    bool wasFound;
    LOCK_HANDLE lock; /*needed to protect this structure*/
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
            const char* messageId;
            const char* correlationId;

            if ((messageId = IoTHubMessage_GetMessageId(msg)) == NULL)
            {
                messageId = "<null>";
            }

            if ((correlationId = IoTHubMessage_GetCorrelationId(msg)) == NULL)
            {
                correlationId = "<null>";
            }

            (void)printf("Received new message from IoT Hub (message-id: %s, correlation-id: %s)", messageId, correlationId);

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
            size_t string_length;
            string_length = sprintf(temp, TEST_MESSAGE_DATA_FMT, ctime(&t), g_iotHubTestId);
            if ((string_length < 0) ||
                ((tempString = (char*)malloc(string_length + 1)) == NULL))
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
                result->toBeSend = (const char*)tempString;
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

        free(data);
    }
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
            size_t string_length;
            string_length = sprintf(temp, TEST_EVENT_DATA_FMT, ctime(&t), g_iotHubTestId);
            if ((string_length < 0) ||
                ((tempString = (char*)malloc(string_length + 1)) == NULL))
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

extern "C" void e2e_init(void)
{
    int result = platform_init();
    ASSERT_ARE_EQUAL_WITH_MSG(int, 0, result, "Platform init failed");
    g_iothubAcctInfo = IoTHubAccount_Init(true);
    ASSERT_IS_NOT_NULL_WITH_MSG(g_iothubAcctInfo, "Could not initialize IoTHubAccount");
    platform_init();
}

extern "C" void e2e_deinit(void)
{
    IoTHubAccount_deinit(g_iothubAcctInfo);
    // Need a double deinit
    platform_deinit();
    platform_deinit();
}

extern "C" void e2e_send_event_test(IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol)
{
    // arrange
    IOTHUB_CLIENT_CONFIG iotHubConfig = { 0 };
    IOTHUB_CLIENT_HANDLE iotHubClientHandle;
    IOTHUB_MESSAGE_HANDLE msgHandle;

    iotHubConfig.iotHubName = IoTHubAccount_GetIoTHubName(g_iothubAcctInfo);
    iotHubConfig.iotHubSuffix = IoTHubAccount_GetIoTHubSuffix(g_iothubAcctInfo);
    iotHubConfig.deviceId = IoTHubAccount_GetDeviceId(g_iothubAcctInfo);
    iotHubConfig.deviceKey = IoTHubAccount_GetDeviceKey(g_iothubAcctInfo);
    iotHubConfig.protocol = protocol;

    EXPECTED_SEND_DATA* sendData = EventData_Create();
    ASSERT_IS_NOT_NULL_WITH_MSG(sendData, "Could not create the EventData associated with the event to be sent");

    // Send the Event
    {
        // Create the IoT Hub Data
        IOTHUB_CLIENT_RESULT result;
        iotHubClientHandle = IoTHubClient_Create(&iotHubConfig);
        ASSERT_IS_NOT_NULL_WITH_MSG(iotHubClientHandle, "Could not create IoTHubClient");

        // Turn on Log 
        bool trace = true;
        (void)IoTHubClient_SetOption(iotHubClientHandle, OPTION_LOG_TRACE, &trace);
        (void)IoTHubClient_SetOption(iotHubClientHandle, "TrustedCerts", certificates);

        msgHandle = IoTHubMessage_CreateFromByteArray((const unsigned char*)sendData->expectedString, strlen(sendData->expectedString));
        ASSERT_IS_NOT_NULL_WITH_MSG(msgHandle, "Could not create the D2C message to be sent");

        // act
        result = IoTHubClient_SendEventAsync(iotHubClientHandle, msgHandle, ReceiveConfirmationCallback, sendData);
        ASSERT_ARE_EQUAL_WITH_MSG(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK, result, "SendEventAsync failed");
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
        ASSERT_IS_TRUE_WITH_MSG(sendData->dataWasRecv, "Failure sending data to IotHub"); // was received by the callback...
        (void)Unlock(sendData->lock);
    }

    IoTHubClient_Destroy(iotHubClientHandle);

    /* guess who */
    (void)platform_init();

    {
        IOTHUB_TEST_HANDLE iotHubTestHandle = IoTHubTest_Initialize(IoTHubAccount_GetEventHubConnectionString(g_iothubAcctInfo), IoTHubAccount_GetIoTHubConnString(g_iothubAcctInfo), IoTHubAccount_GetDeviceId(g_iothubAcctInfo), IoTHubAccount_GetDeviceKey(g_iothubAcctInfo), IoTHubAccount_GetEventhubListenName(g_iothubAcctInfo), IoTHubAccount_GetEventhubAccessKey(g_iothubAcctInfo), IoTHubAccount_GetSharedAccessSignature(g_iothubAcctInfo), IoTHubAccount_GetEventhubConsumerGroup(g_iothubAcctInfo));
        ASSERT_IS_NOT_NULL_WITH_MSG(iotHubTestHandle, "Could not initialize IoTHubTest in order to listen for events");

        IOTHUB_TEST_CLIENT_RESULT result = IoTHubTest_ListenForEventForMaxDrainTime(iotHubTestHandle, IoTHubCallback, IoTHubAccount_GetIoTHubPartitionCount(g_iothubAcctInfo), sendData);
        ASSERT_ARE_EQUAL_WITH_MSG(IOTHUB_TEST_CLIENT_RESULT, IOTHUB_TEST_CLIENT_OK, result, "Listening for the event failed");

        IoTHubTest_Deinit(iotHubTestHandle);
    }

    // assert
    ASSERT_IS_TRUE_WITH_MSG(sendData->wasFound, "Failure retrieving data that was sent to eventhub"); // was found is written by the callback...

    // cleanup
    IoTHubMessage_Destroy(msgHandle);
    EventData_Destroy(sendData);
}

extern "C" void e2e_recv_message_test(IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol)
{
    // arrange
    IOTHUB_CLIENT_CONFIG iotHubConfig = { 0 };
    IOTHUB_CLIENT_HANDLE iotHubClientHandle;

    EXPECTED_RECEIVE_DATA* notifyData = MessageData_Create();
    ASSERT_IS_NOT_NULL_WITH_MSG(notifyData, "Could not create the C2D message to be sent to the device");

    // act
    iotHubConfig.iotHubName = IoTHubAccount_GetIoTHubName(g_iothubAcctInfo);
    iotHubConfig.iotHubSuffix = IoTHubAccount_GetIoTHubSuffix(g_iothubAcctInfo);
    iotHubConfig.deviceId = IoTHubAccount_GetDeviceId(g_iothubAcctInfo);
    iotHubConfig.deviceKey = IoTHubAccount_GetDeviceKey(g_iothubAcctInfo);
    iotHubConfig.protocol = protocol;

    IOTHUB_TEST_HANDLE iotHubTestHandle = IoTHubTest_Initialize(IoTHubAccount_GetEventHubConnectionString(g_iothubAcctInfo), IoTHubAccount_GetIoTHubConnString(g_iothubAcctInfo), IoTHubAccount_GetDeviceId(g_iothubAcctInfo), IoTHubAccount_GetDeviceKey(g_iothubAcctInfo), IoTHubAccount_GetEventhubListenName(g_iothubAcctInfo), IoTHubAccount_GetEventhubAccessKey(g_iothubAcctInfo), IoTHubAccount_GetSharedAccessSignature(g_iothubAcctInfo), IoTHubAccount_GetEventhubConsumerGroup(g_iothubAcctInfo));
    ASSERT_IS_NOT_NULL_WITH_MSG(iotHubTestHandle, "Could not initialize IoTHubTest to send C2D messages to the device");

    IOTHUB_TEST_CLIENT_RESULT testResult = IoTHubTest_SendMessage(iotHubTestHandle, (const unsigned char*)notifyData->toBeSend, notifyData->toBeSendSize);
    ASSERT_ARE_EQUAL_WITH_MSG(IOTHUB_TEST_CLIENT_RESULT, IOTHUB_TEST_CLIENT_OK, testResult, "Sending C2D message to device failed");

    IoTHubTest_Deinit(iotHubTestHandle);

    iotHubClientHandle = IoTHubClient_Create(&iotHubConfig);
    ASSERT_IS_NOT_NULL_WITH_MSG(iotHubClientHandle, "Could not create IoTHubClient");

    // Turn on Log 
    bool trace = true;
    (void)IoTHubClient_SetOption(iotHubClientHandle, OPTION_LOG_TRACE, &trace);
    (void)IoTHubClient_SetOption(iotHubClientHandle, "TrustedCerts", certificates);

    IOTHUB_CLIENT_RESULT result = IoTHubClient_SetMessageCallback(iotHubClientHandle, ReceiveMessageCallback, notifyData);
    ASSERT_ARE_EQUAL_WITH_MSG(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK, result, "Setting message callback failed");

    time_t beginOperation, nowTime;
    beginOperation = time(NULL);
    while (
        (nowTime = time(NULL)), (difftime(nowTime, beginOperation) < MAX_CLOUD_TRAVEL_TIME) //time box
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
    ASSERT_IS_TRUE_WITH_MSG(notifyData->wasFound, "Failure retrieving data from C2D"); // was found is written by the callback...

    // cleanup
    MessageData_Destroy(notifyData);
    IoTHubClient_Destroy(iotHubClientHandle);
}

