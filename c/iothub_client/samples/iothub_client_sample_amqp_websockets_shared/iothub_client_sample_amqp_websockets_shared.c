// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdio.h>
#include <stdlib.h>

#include "azure_c_shared_utility/platform.h"
#include "azure_c_shared_utility/threadapi.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "iothub_client.h"
#include "iothub_message.h"
#include "iothubtransportamqp_websockets.h"

#ifdef MBED_BUILD_TIMESTAMP
#include "certs.h"
#endif // MBED_BUILD_TIMESTAMP

static const char* hubName = "[IoT Hub Name]";
static const char* hubSuffix = "[IoT Hub Suffix]";
static const char* deviceId1 = "[device id 1]";
static const char* deviceId2 = "[device id 2]";
static const char* deviceKey1 = "[device key 1]";
static const char* deviceKey2 = "[device key 2]";


//static int callbackCounter;
static bool g_continueRunning;
static char msgText[1024];
static char propText[1024];
#define MESSAGE_COUNT       5
#define DOWORK_LOOP_NUM     3


typedef struct EVENT_INSTANCE_TAG
{
    IOTHUB_MESSAGE_HANDLE messageHandle;
	const char* deviceId;
    size_t messageTrackingId;  // For tracking the messages within the user callback.
} EVENT_INSTANCE;

static IOTHUBMESSAGE_DISPOSITION_RESULT ReceiveMessageCallback(IOTHUB_MESSAGE_HANDLE message, void* userContextCallback)
{
    int* counter = (int*)userContextCallback;
    const unsigned char* buffer = NULL;
    size_t size = 0;
    const char* messageId;
    const char* correlationId;

    // AMQP message properties
    if ((messageId = IoTHubMessage_GetMessageId(message)) == NULL)
    {
        messageId = "<null>";
    }

    if ((correlationId = IoTHubMessage_GetCorrelationId(message)) == NULL)
    {
        correlationId = "<null>";
    }

    // AMQP message content.
    IOTHUBMESSAGE_CONTENT_TYPE contentType = IoTHubMessage_GetContentType(message);

    if (contentType == IOTHUBMESSAGE_BYTEARRAY)
    {
        if (IoTHubMessage_GetByteArray(message, &buffer, &size) == IOTHUB_MESSAGE_OK)
        {
            (void)printf("Received Message [%d] (message-id: %s, correlation-id: %s) with BINARY Data: <<<%.*s>>> & Size=%d\r\n", *counter, messageId, correlationId,(int)size, buffer, (int)size);
        }
        else
        {
            (void)printf("Failed getting the BINARY body of the message received.\r\n");
        }
    }
    else if (contentType == IOTHUBMESSAGE_STRING)
    {
        if ((buffer = (const unsigned char*)IoTHubMessage_GetString(message)) != NULL && (size = strlen((const char*)buffer)) > 0)
        {
            (void)printf("Received Message [%d] (message-id: %s, correlation-id: %s) with STRING Data: <<<%.*s>>> & Size=%d\r\n", *counter, messageId, correlationId, (int)size, buffer, (int)size);

            // If we receive the work 'quit' then we stop running
        }
        else
        {
            (void)printf("Failed getting the STRING body of the message received.\r\n");
        }
    }
    else
    {
        (void)printf("Failed getting the body of the message received (type %i).\r\n", contentType);
    }

    // Retrieve properties from the message
    MAP_HANDLE mapProperties = IoTHubMessage_Properties(message);
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

    if (memcmp(buffer, "quit", size) == 0)
    {
        g_continueRunning = false;
    }

    /* Some device specific action code goes here... */
    (*counter)++;
    return IOTHUBMESSAGE_ACCEPTED;
}

static void SendConfirmationCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void* userContextCallback)
{
    EVENT_INSTANCE* eventInstance = (EVENT_INSTANCE*)userContextCallback;
	(void)printf("Confirmation received for message %zu from device %s with result = %s\r\n", eventInstance->messageTrackingId, eventInstance->deviceId, ENUM_TO_STRING(IOTHUB_CLIENT_CONFIRMATION_RESULT, result));
    /* Some device specific action code goes here... */
}

static int create_events(EVENT_INSTANCE* events, const char* deviceId)
{
	int result = 0;

	srand((unsigned int)time(NULL));
	double avgWindSpeed = 10.0;

	int i;
	for (i = 0; i < MESSAGE_COUNT; i++)
	{
		if (sprintf_s(msgText, sizeof(msgText), "{\"deviceId\":\"%s\",\"windSpeed\":%.2f}", deviceId, avgWindSpeed + (rand() % 4 + 2)) == 0)
		{
			(void)printf("ERROR: failed creating event message for device %s\r\n", deviceId);
			result = __LINE__;
		}
		else if ((events[i].messageHandle = IoTHubMessage_CreateFromByteArray((const unsigned char*)msgText, strlen(msgText))) == NULL)
		{
			(void)printf("ERROR: failed creating the IOTHUB_MESSAGE_HANDLE for device %s\r\n", deviceId);
			result = __LINE__;
		}
		else
		{
			MAP_HANDLE propMap;

			if ((propMap = IoTHubMessage_Properties(events[i].messageHandle)) == NULL) 
			{
				(void)printf("ERROR: failed getting device %s's message property map\r\n", deviceId);
				result = __LINE__;
			}
			else if (sprintf_s(propText, sizeof(propText), "PropMsg_%d", i) == 0) 
			{
				(void)printf("ERROR: sprintf_s failed for device %s's message property\r\n", deviceId);
				result = __LINE__;
			}
			else if (Map_AddOrUpdate(propMap, "PropName", propText) != MAP_OK)
			{
				(void)printf("ERROR: Map_AddOrUpdate failed for device %s\r\n", deviceId);
				result = __LINE__;
			}
			else
			{
				events[i].deviceId = deviceId;
				events[i].messageTrackingId = i;
				result = 0;
			}
		}
	}

	return result;
}

void iothub_client_sample_amqp_websockets_shared_run(void)
{
	TRANSPORT_HANDLE transport_handle;
	IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle1;
	IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle2;

	EVENT_INSTANCE messages_device1[MESSAGE_COUNT];
	EVENT_INSTANCE messages_device2[MESSAGE_COUNT];

    g_continueRunning = true;

    //callbackCounter = 0;
	int receiveContext1 = 0;
	int receiveContext2 = 0;

    (void)printf("Starting the IoTHub client sample AMQP...\r\n");

    if (platform_init() != 0)
    {
        printf("Failed to initialize the platform.\r\n");
    }
	else if ((transport_handle = IoTHubTransport_Create(AMQP_Protocol_over_WebSocketsTls, hubName, hubSuffix)) == NULL)
	{
		printf("Failed to creating the protocol handle.\r\n");
	}
    else
    {
		IOTHUB_CLIENT_DEVICE_CONFIG config1;
		config1.deviceId = deviceId1;
		config1.deviceKey = deviceKey1;
		config1.deviceSasToken = NULL;
		config1.protocol = AMQP_Protocol_over_WebSocketsTls;
		config1.transportHandle = IoTHubTransport_GetLLTransport(transport_handle);

		IOTHUB_CLIENT_DEVICE_CONFIG config2;
		config2.deviceId = deviceId2;
		config2.deviceKey = deviceKey2;
		config2.deviceSasToken = NULL;
		config2.protocol = AMQP_Protocol_over_WebSocketsTls;
		config2.transportHandle = IoTHubTransport_GetLLTransport(transport_handle);

        if ((iotHubClientHandle1 = IoTHubClient_LL_CreateWithTransport(&config1)) == NULL)
        {
            (void)printf("ERROR: iotHubClientHandle1 is NULL!\r\n");
        }
		else if ((iotHubClientHandle2 = IoTHubClient_LL_CreateWithTransport(&config2)) == NULL)
		{
			(void)printf("ERROR: iotHubClientHandle1 is NULL!\r\n");
		}
        else
        {
            bool traceOn = true;
            IoTHubClient_LL_SetOption(iotHubClientHandle1, "logtrace", &traceOn);

#ifdef MBED_BUILD_TIMESTAMP
            // For mbed add the certificate information
            if (IoTHubClient_LL_SetOption(iotHubClientHandle1, "TrustedCerts", certificates) != IOTHUB_CLIENT_OK)
            {
                printf("failure to set option \"TrustedCerts\"\r\n");
            }
#endif // MBED_BUILD_TIMESTAMP

			if (create_events(messages_device1, config1.deviceId) != 0 || create_events(messages_device2, config2.deviceId) != 0)
			{
				(void)printf("ERROR: failed creating events for the devices..........FAILED!\r\n");
			}
            /* Setting Message call back, so we can receive Commands. */
            else if (IoTHubClient_LL_SetMessageCallback(iotHubClientHandle1, ReceiveMessageCallback, &receiveContext1) != IOTHUB_CLIENT_OK)
            {
                (void)printf("ERROR: IoTHubClient_SetMessageCallback for device 1..........FAILED!\r\n");
            }
			else if (IoTHubClient_LL_SetMessageCallback(iotHubClientHandle2, ReceiveMessageCallback, &receiveContext2) != IOTHUB_CLIENT_OK)
			{
				(void)printf("ERROR: IoTHubClient_SetMessageCallback for device 2..........FAILED!\r\n");
			}
            else
            {
                (void)printf("IoTHubClient_SetMessageCallback...successful.\r\n");

                /* Now that we are ready to receive commands, let's send some messages */
                size_t iterator = 0;
                do
                {
                    if (iterator < MESSAGE_COUNT)
                    {
                        if (IoTHubClient_LL_SendEventAsync(iotHubClientHandle1, messages_device1[iterator].messageHandle, SendConfirmationCallback, &messages_device1[iterator]) != IOTHUB_CLIENT_OK)
                        {
                            (void)printf("ERROR: IoTHubClient_SendEventAsync for device 1..........FAILED!\r\n");
                        }
						else if (IoTHubClient_LL_SendEventAsync(iotHubClientHandle2, messages_device2[iterator].messageHandle, SendConfirmationCallback, &messages_device2[iterator]) != IOTHUB_CLIENT_OK)
						{
							(void)printf("ERROR: IoTHubClient_SendEventAsync for device 2..........FAILED!\r\n");
						}
                        else
                        {
                            (void)printf("IoTHubClient_SendEventAsync accepted data for transmission to IoT Hub.\r\n");
                        }
                    }

					IoTHubClient_LL_DoWork(iotHubClientHandle1);
					IoTHubClient_LL_DoWork(iotHubClientHandle2);
                    ThreadAPI_Sleep(1);

                    iterator++;
                } while (g_continueRunning);

                (void)printf("iothub_client_sample_mqtt has gotten quit message, call DoWork %d more time to complete final sending...\r\n", DOWORK_LOOP_NUM);
                for (size_t index = 0; index < DOWORK_LOOP_NUM; index++)
                {
                    IoTHubClient_LL_DoWork(iotHubClientHandle1);
                    ThreadAPI_Sleep(1);
                }
            }
			IoTHubClient_LL_Destroy(iotHubClientHandle1);
			IoTHubClient_LL_Destroy(iotHubClientHandle2);
        }
        IoTHubTransport_Destroy(transport_handle);
        platform_deinit();
    }
}

int main(void)
{
    iothub_client_sample_amqp_websockets_shared_run();
    return 0;
}
