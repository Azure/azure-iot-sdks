// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdio.h>
#include <stdlib.h>

/* This sample uses the _LL APIs of iothub_client for example purposes.
   That does not mean that HTTP only works with the _LL APIs.
   Simply changing the using the convenience layer (functions not having _LL)
   and removing calls to _DoWork will yield the same results. */

#ifdef ARDUINO
#include "AzureIoT.h"
#else
#include "iothub_client.h"
#include "iothub_message.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/platform.h"
#include "iothubtransporthttp.h"
#endif

#ifdef MBED_BUILD_TIMESTAMP
#include "certs.h"
#endif // MBED_BUILD_TIMESTAMP

// It is NOT a good practice to put device credentials in the code as shown below.
// This is done in this sample for simplicity purposes.
// Good practices such as the use of TPM or other secure storage on the device are recommended to store device credentials.

static const char* hubName = "[IoT Hub Name]";
static const char* hubSuffix = "[IoT Hub Suffix]";
static const char* deviceId1 = "[device id 1]";
static const char* deviceId2 = "[device id 2]";
static const char* deviceKey1 = "[device key 1]";
static const char* deviceKey2 = "[device key 2]";

static int callbackCounter;


typedef struct EVENT_INSTANCE_TAG
{
    IOTHUB_MESSAGE_HANDLE messageHandle;
    int messageTrackingId;  // For tracking the messages within the user callback.
} EVENT_INSTANCE;

static IOTHUBMESSAGE_DISPOSITION_RESULT ReceiveMessageCallback(IOTHUB_MESSAGE_HANDLE message, void* userContextCallback)
{
    int* counter = (int*)userContextCallback;
    const char* buffer;
    size_t size;
    MAP_HANDLE mapProperties;

    if (IoTHubMessage_GetByteArray(message, (const unsigned char**)&buffer, &size) != IOTHUB_MESSAGE_OK)
    {
        printf("unable to retrieve the message data\r\n");
    }
    else
    {
        (void)printf("Received Message [%d] with Data: <<<%.*s>>> & Size=%d\r\n", *counter, (int)size, buffer, (int)size);
    }

    // Retrieve properties from the message
    mapProperties = IoTHubMessage_Properties(message);
    if (mapProperties != NULL)
    {
        const char*const* keys;
        const char*const* values;
        size_t propertyCount = 0;
        if (Map_GetInternals(mapProperties, &keys, &values, &propertyCount) == MAP_OK)
        {
            if (propertyCount > 0)
            {
                size_t index;

                printf("Message Properties:\r\n");
                for (index = 0; index < propertyCount; index++)
                {
                    printf("\tKey: %s Value: %s\r\n", keys[index], values[index]);
                }
                printf("\r\n");
            }
        }
    }

    /* Some device specific action code goes here... */
    (*counter)++;
    return IOTHUBMESSAGE_ACCEPTED;
}

static void SendConfirmationCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void* userContextCallback)
{
    EVENT_INSTANCE* eventInstance = (EVENT_INSTANCE*)userContextCallback;
    (void)printf("Confirmation[%d] received for message tracking id = %d with result = %s\r\n", callbackCounter, eventInstance->messageTrackingId, ENUM_TO_STRING(IOTHUB_CLIENT_CONFIRMATION_RESULT, result));
    /* Some device specific action code goes here... */
    callbackCounter++;
    IoTHubMessage_Destroy(eventInstance->messageHandle);
}

static char msgText[1024];
static char propText[1024];
#define MESSAGE_COUNT 5

void iothub_client_sample_http_setoptions(IOTHUB_CLIENT_HANDLE iothubClient, unsigned int timeout, unsigned int minimumPollingTime)
{
    if (IoTHubClient_SetOption(iothubClient, "timeout", &timeout) != IOTHUB_CLIENT_OK)
    {
        printf("failure to set option \"timeout\" on [%p]\r\n", iothubClient);
    }

    if (IoTHubClient_SetOption(iothubClient, "MinimumPollingTime", &minimumPollingTime) != IOTHUB_CLIENT_OK)
    {
        printf("failure to set option \"MinimumPollingTime\"on [%p]\r\n", iothubClient);
    }
}

void iothub_client_sample_http_send_one_msg(IOTHUB_CLIENT_HANDLE iothubClient, EVENT_INSTANCE *message, int messageNumber)
{
    if ((message->messageHandle = IoTHubMessage_CreateFromByteArray((const unsigned char*)msgText, strlen(msgText))) == NULL)
    {
        (void)printf("ERROR: iotHubMessageHandle is NULL!\r\n");
    }
    else
    {
        MAP_HANDLE propMap;

        message->messageTrackingId = messageNumber;

        propMap = IoTHubMessage_Properties(message->messageHandle);
        sprintf_s(propText, sizeof(propText), "PropMsg_%d", messageNumber);
        if (Map_AddOrUpdate(propMap, "PropName", propText) != MAP_OK)
        {
            (void)printf("ERROR: Map_AddOrUpdate Failed!\r\n");
        }

        if (IoTHubClient_SendEventAsync(iothubClient, message->messageHandle, SendConfirmationCallback, message) != IOTHUB_CLIENT_OK)
        {
            (void)printf("ERROR: IoTHubClient_LL_SendEventAsync to device [%p]..........FAILED!\r\n", iothubClient);
        }
        else
        {
            (void)printf("IoTHubClient_LL_SendEventAsync accepted message [%d] for transmission to IoT Hub, device [%p].\r\n", messageNumber, iothubClient);
        }
    }
}

void iothub_client_sample_http_run(void)
{
    TRANSPORT_HANDLE httpTransport;
    IOTHUB_CLIENT_HANDLE iothubClient1;
    IOTHUB_CLIENT_HANDLE iothubClient2;


    EVENT_INSTANCE messages1[MESSAGE_COUNT];
    EVENT_INSTANCE messages2[MESSAGE_COUNT];
    double avgWindSpeed = 10.0;
    int receiveContext1 = 0;
    int receiveContext2 = 0;

    srand((unsigned int)time(NULL));

    callbackCounter = 0;

    if (platform_init() != 0)
    {
        printf("Failed to initialize the platform.\r\n");
    }
    else
    {
        (void)printf("Starting the IoTHub client sample HTTP with Shared connection...\r\n");


        if ((httpTransport = IoTHubTransport_Create(HTTP_Protocol, hubName, hubSuffix)) == NULL)
        {
            (void)printf("ERROR: httpTransport is NULL\r\n");
        }
        else
        {
            IOTHUB_CLIENT_CONFIG config1;
            config1.deviceId = deviceId1;
            config1.deviceKey = deviceKey1;
            config1.protocol = HTTP_Protocol;
            config1.iotHubName = NULL;
            config1.iotHubSuffix = NULL;
            config1.deviceSasToken = NULL;
            if ((iothubClient1 = IoTHubClient_CreateWithTransport(httpTransport, &config1)) == NULL)
            {
                (void)printf("ERROR: 1st device handle is NULL\r\n");
            }
            else
            {
                IOTHUB_CLIENT_CONFIG config2;
                config2.deviceId = deviceId2;
                config2.deviceKey = deviceKey2;
                config2.protocol = HTTP_Protocol;
                config2.iotHubName = NULL;
                config2.iotHubSuffix = NULL;
                config2.deviceSasToken = NULL;
                if ((iothubClient2 = IoTHubClient_CreateWithTransport(httpTransport, &config2)) == NULL)
                {
                    (void)printf("ERROR: 2nd device handle is NULL\r\n");
                }
                else
                {
                    unsigned int timeout = 241000;
                    // Because it can poll "after 9 seconds" polls will happen effectively // at ~10 seconds.
                    // Note that for scalabilty, the default value of minimumPollingTime
                    // is 25 minutes. For more information, see:
                    // https://azure.microsoft.com/documentation/articles/iot-hub-devguide/#messaging
                    unsigned int minimumPollingTime = 9;
                    iothub_client_sample_http_setoptions(iothubClient1, timeout, minimumPollingTime);
                    iothub_client_sample_http_setoptions(iothubClient2, timeout, minimumPollingTime);

                    // Setting message callbacks so we can receive commands
                    if (IoTHubClient_SetMessageCallback(iothubClient1, ReceiveMessageCallback, &receiveContext1) != IOTHUB_CLIENT_OK)
                    {
                        (void)printf("ERROR: IoTHubClient_LL_SetMessageCallback 1st device..........FAILED!\r\n");
                    }
                    else
                    {
                        if (IoTHubClient_SetMessageCallback(iothubClient2, ReceiveMessageCallback, &receiveContext2) != IOTHUB_CLIENT_OK)
                        {
                            (void)printf("ERROR: IoTHubClient_LL_SetMessageCallback 2nd device..........FAILED!\r\n");
                        }
                        else
                        {
                            // Now that we are ready to receive commands, let's send some messages
                            int i;

                            (void)printf("IoTHubClient_LL_SetMessageCallback...successful.\r\n");

                            for (i = 0; i < MESSAGE_COUNT; i++)
                            {
                                sprintf_s(msgText, sizeof(msgText), "{\"deviceId\": \"%s\",\"windSpeed\": %.2f}", deviceId1, avgWindSpeed + (rand() % 4 + 2));
                                iothub_client_sample_http_send_one_msg(iothubClient1, &messages1[i], i);

                                sprintf_s(msgText, sizeof(msgText), "{\"deviceId\": \"%s\",\"windSpeed\": %.2f}", deviceId2, avgWindSpeed + (rand() % 4 + 2));
                                iothub_client_sample_http_send_one_msg(iothubClient2, &messages2[i], i);
                            }
                            // Wait for any incoming messages
                            (void)printf("Press any key to exit the application. \r\n");
                            (void)getchar();
                        }
                    }
                    IoTHubClient_Destroy(iothubClient2);
                }
                IoTHubClient_Destroy(iothubClient1);
            }
            IoTHubTransport_Destroy(httpTransport);
        }
        platform_deinit();
    }
}
