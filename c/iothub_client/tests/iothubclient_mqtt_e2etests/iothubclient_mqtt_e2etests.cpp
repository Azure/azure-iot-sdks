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
#include "iothubtransportmqtt.h"

#include "iothub_account.h"
#include "iothubtest.h"

#include "buffer_.h"
#include "threadapi.h"
#include "iot_logging.h"
#include "platform.h"

#if _WIN32
#include "tlsio_schannel.h"
#else
#ifdef MBED_BUILD_TIMESTAMP
#include "tlsio_wolfssl.h"
#else
#include "tlsio_openssl.h"
#endif
#endif

static MICROMOCK_GLOBAL_SEMAPHORE_HANDLE g_dllByDll;
static bool g_callbackRecv = false;

const char* TEST_EVENT_DATA_FMT = "{\"data\":\"%.24s\",\"id\":\"%d\"}";
const char* TEST_MESSAGE_DATA_FMT = "{\"notifyData\":\"%.24s\",\"id\":\"%d\"}";

static size_t g_iotHubTestId = 0;

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
} EXPECTED_SEND_DATA;

typedef struct EXPECTED_RECEIVE_DATA_TAG
{
    const char* toBeSend;
    size_t toBeSendSize;
    const char* data;
    size_t dataSize;
    bool wasFound;
} EXPECTED_RECEIVE_DATA;


BEGIN_TEST_SUITE(iothubclient_mqtt_e2etests)

    static const XIO_HANDLE CreateXioConnection(const char* hostname, int portNum)
    {
#if _WIN32
        TLSIO_SCHANNEL_CONFIG tls_io_config = { hostname, portNum };
        const IO_INTERFACE_DESCRIPTION* tlsio_interface = tlsio_schannel_get_interface_description();
#else
#ifdef MBED_BUILD_TIMESTAMP
        TLSIO_WOLFSSL_CONFIG tls_io_config = { hostname, portNum };
        const IO_INTERFACE_DESCRIPTION* tlsio_interface = tlsio_wolfssl_get_interface_description();
#else
        TLSIO_OPENSSL_CONFIG tls_io_config = { hostname, portNum };
        const IO_INTERFACE_DESCRIPTION* tlsio_interface = tlsio_openssl_get_interface_description();
#endif
#endif
        return xio_create(tlsio_interface, &tls_io_config, NULL);
    }

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
            char temp[TEMP_BUFFER_SIZE];
            char* tempString;
            time_t t = time(NULL);
            (void)snprintf(temp, TEMP_BUFFER_SIZE, TEST_MESSAGE_DATA_FMT, ctime(&t), g_iotHubTestId);
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
                result->toBeSend = tempString;
                result->toBeSendSize = strlen(result->toBeSend);
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
            char temp[TEMP_BUFFER_SIZE];
            char* tempString;
            time_t t = time(NULL);
            (void)snprintf(temp, TEMP_BUFFER_SIZE, TEST_EVENT_DATA_FMT, ctime(&t), g_iotHubTestId);
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
    }

    TEST_SUITE_CLEANUP(TestClassCleanup)
    {
        DEINITIALIZE_MEMORY_DEBUG(g_dllByDll);
        platform_deinit();
    }

    TEST_FUNCTION_INITIALIZE(TestMethodInitialize)
    {
        g_iotHubTestId++;
    }

    TEST_FUNCTION_CLEANUP(TestMethodCleanup)
    {
    }

    // Will enable as soon as it's ready
    TEST_FUNCTION(IoTHub_MQTT_SendEvent_E2ETests)
    {
        // arrange
        /*IOTHUB_CLIENT_CONFIG iotHubConfig;
        IOTHUB_CLIENT_HANDLE iotHubClientHandle;
        IOTHUB_MESSAGE_HANDLE msgHandle;

        iotHubConfig.iotHubName = IoTHubAccount_GetIoTHubName();
        iotHubConfig.iotHubSuffix = IoTHubAccount_GetIoTHubSuffix();
        iotHubConfig.deviceId = IoTHubAccount_GetDeviceId();
        iotHubConfig.deviceKey = IoTHubAccount_GetDeviceKey();
        iotHubConfig.protocol = MQTT_Protocol;
        iotHubConfig.io_transport_provider_callback = CreateXioConnection;

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
            IoTHubClient_SetOption(iotHubClientHandle, "logtrace", &trace);

            msgHandle = IoTHubMessage_CreateFromByteArray((const unsigned char*)sendData->expectedString, strlen(sendData->expectedString));
            ASSERT_IS_NOT_NULL_WITH_MSG(msgHandle, "Error Creating IoTHubMEssage From Byte Array.");

            // act
            result = IoTHubClient_SendEventAsync(iotHubClientHandle, msgHandle, ReceiveConfirmationCallback, sendData);
            ASSERT_ARE_EQUAL(int, IOTHUB_CLIENT_OK, result);
        }

        time_t beginOperation, nowTime;
        beginOperation = time(NULL);
        while ( (
                (nowTime = time(NULL)),
                (difftime(nowTime, beginOperation) < MAX_CLOUD_TRAVEL_TIME) // time box
                ) && (!sendData->dataWasRecv) // Condition box
              )
        {
            // Just go on here
        }
        ASSERT_IS_TRUE(sendData->dataWasRecv); // was found is written by the callback...
        IoTHubClient_Destroy(iotHubClientHandle);

        {
            IOTHUB_TEST_HANDLE iotHubTestHandle = IoTHubTest_Initialize(IoTHubAccount_GetEventHubConnectionString(), IoTHubAccount_GetIoTHubConnString(), IoTHubAccount_GetDeviceId(), IoTHubAccount_GetDeviceKey(), IoTHubAccount_GetEventhubListenName(), IoTHubAccount_GetEventhubAccessKey(), IoTHubAccount_GetSharedAccessSignature(), IoTHubAccount_GetEventhubConsumerGroup());
            ASSERT_IS_NOT_NULL_WITH_MSG(iotHubTestHandle, "Problem Initializing IoTHub Test.");

            IOTHUB_TEST_CLIENT_RESULT result = IoTHubTest_ListenForEventForMaxDrainTime(iotHubTestHandle, IoTHubCallback, IoTHubAccount_GetIoTHubPartitionCount(), sendData);
            ASSERT_ARE_EQUAL(IOTHUB_TEST_CLIENT_RESULT, IOTHUB_TEST_CLIENT_OK, result);

            IoTHubTest_Deinit(iotHubTestHandle);
        }

        // assert
        ASSERT_IS_TRUE(sendData->wasFound); // was found is written by the callback...

        // cleanup
        IoTHubMessage_Destroy(msgHandle);
        EventData_Destroy(sendData);*/
    }

    TEST_FUNCTION(IoTHub_MQTT_RecvMessage_E2ETest)
    {
        // arrange
        /*IOTHUB_CLIENT_CONFIG iotHubConfig;
        IOTHUB_CLIENT_HANDLE iotHubClientHandle;

        EXPECTED_RECEIVE_DATA* notifyData = MessageData_Create();
        ASSERT_IS_NOT_NULL_WITH_MSG(notifyData, "Error creating Notify Data.");

        // act
        iotHubConfig.iotHubName = IoTHubAccount_GetIoTHubName();
        iotHubConfig.iotHubSuffix = IoTHubAccount_GetIoTHubSuffix();
        iotHubConfig.deviceId = IoTHubAccount_GetDeviceId();
        iotHubConfig.deviceKey = IoTHubAccount_GetDeviceKey();
        iotHubConfig.protocol = MQTT_Protocol;
        iotHubConfig.io_transport_provider_callback = CreateXioConnection;

        iotHubClientHandle = IoTHubClient_Create(&iotHubConfig);
        ASSERT_IS_NOT_NULL_WITH_MSG(iotHubClientHandle, "Error creating IoTHubClient.");

        // Turn on Log 
        bool trace = true;
        IoTHubClient_SetOption(iotHubClientHandle, "logtrace", &trace);

        IOTHUB_CLIENT_RESULT result = IoTHubClient_SetMessageCallback(iotHubClientHandle, ReceiveMessageCallback, notifyData);
        ASSERT_ARE_EQUAL(int, IOTHUB_CLIENT_OK, result);

        IOTHUB_TEST_HANDLE iotHubTestHandle = IoTHubTest_Initialize(IoTHubAccount_GetEventHubConnectionString(), IoTHubAccount_GetIoTHubConnString(), IoTHubAccount_GetDeviceId(), IoTHubAccount_GetDeviceKey(), IoTHubAccount_GetEventhubListenName(), IoTHubAccount_GetEventhubAccessKey(), IoTHubAccount_GetSharedAccessSignature(), IoTHubAccount_GetEventhubConsumerGroup());
        ASSERT_IS_NOT_NULL_WITH_MSG(iotHubTestHandle, "Error Creating IotHubTest.");

        IOTHUB_TEST_CLIENT_RESULT testResult = IoTHubTest_SendMessage(iotHubTestHandle, notifyData->toBeSend, notifyData->toBeSendSize);
        ASSERT_ARE_EQUAL(IOTHUB_TEST_CLIENT_RESULT, IOTHUB_TEST_CLIENT_OK, testResult);

        IoTHubTest_Deinit(iotHubTestHandle);

        time_t beginOperation, nowTime;
        beginOperation = time(NULL);
        while ( (
                (nowTime = time(NULL)),
                (difftime(nowTime, beginOperation) < MAX_CLOUD_TRAVEL_TIME) //time box
                ) && (!notifyData->wasFound) //condition box
              )
        {
            //just go on;
        }

        // assert
        ASSERT_IS_TRUE(notifyData->wasFound); // was found is written by the callback...

        // cleanup
        MessageData_Destroy(notifyData);
        IoTHubClient_Destroy(iotHubClientHandle);*/
    }
END_TEST_SUITE(iothubclient_mqtt_e2etests)
