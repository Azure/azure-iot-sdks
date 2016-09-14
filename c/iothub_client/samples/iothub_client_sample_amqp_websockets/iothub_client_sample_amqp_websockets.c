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
#include "../../../certs/certs.h"

static const char* connectionString = "[device connection string]";
static int callbackCounter;


typedef struct EVENT_INSTANCE_TAG
{
    IOTHUB_MESSAGE_HANDLE messageHandle;
    int messageTrackingId;  // For tracking the messages within the user callback.
} EVENT_INSTANCE;

static IOTHUBMESSAGE_DISPOSITION_RESULT ReceiveMessageCallback(IOTHUB_MESSAGE_HANDLE message, void* userContextCallback)
{
	const unsigned char* buffer = NULL;
	size_t size = 0;
	
	if (userContextCallback == NULL)
	{
		(void)printf("Failed getting the context of the message received.\r\n");
	}
	else
	{
		int* counter = (int*)userContextCallback;
			
		IOTHUBMESSAGE_CONTENT_TYPE contentType = IoTHubMessage_GetContentType(message);

		if (contentType == IOTHUBMESSAGE_BYTEARRAY)
		{
			if (IoTHubMessage_GetByteArray(message, &buffer, &size) == IOTHUB_MESSAGE_OK)
			{
				(void)printf("Received Message [%d] with BINARY Data: <<<%.*s>>> & Size=%d\r\n", *counter, (int)size, buffer, (int)size);
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
				(void)printf("Received Message [%d] with STRING Data: <<<%.*s>>> & Size=%d\r\n", *counter, (int)size, buffer, (int)size);
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

		(*counter)++;
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

    /* Some device specific action code goes here... */
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

void iothub_client_sample_amqp_websockets_run(void)
{
    IOTHUB_CLIENT_HANDLE iotHubClientHandle;

    EVENT_INSTANCE messages[MESSAGE_COUNT];

    srand((unsigned int)time(NULL));
    double avgWindSpeed = 10.0;

    callbackCounter = 0;
    int receiveContext = 0;

    (void)printf("Starting the IoTHub client sample AMQP over WebSockets...\r\n");
	
	if (platform_init() != 0)
	{
		(void)printf("ERROR: failed initializing the platform.\r\n");
	}
    else if ((iotHubClientHandle = IoTHubClient_CreateFromConnectionString(connectionString, AMQP_Protocol_over_WebSocketsTls)) == NULL)
    {
        (void)printf("ERROR: iotHubClientHandle is NULL!\r\n");
        platform_deinit();
    }
    else
    {
        // For mbed add the certificate information
        if (IoTHubClient_SetOption(iotHubClientHandle, "TrustedCerts", certificates) != IOTHUB_CLIENT_OK)
        {
            printf("failure to set option \"TrustedCerts\"\r\n");
        }

        /* Setting Message call back, so we can receive Commands. */
        if (IoTHubClient_SetMessageCallback(iotHubClientHandle, ReceiveMessageCallback, &receiveContext) != IOTHUB_CLIENT_OK)
        {
            (void)printf("ERROR: IoTHubClient_SetMessageCallback..........FAILED!\r\n");
        }
        else
        {
            (void)printf("IoTHubClient_SetMessageCallback...successful.\r\n");

            /* Now that we are ready to receive commands, let's send some messages */
            for (size_t i = 0; i < MESSAGE_COUNT; i++)
            {
                sprintf_s(msgText, sizeof(msgText), "{\"deviceId\":\"myFirstDevice\",\"windSpeed\":%.2f}", avgWindSpeed+(rand()%4+2) );
                if ((messages[i].messageHandle = IoTHubMessage_CreateFromByteArray((const unsigned char*)msgText, strlen(msgText))) == NULL)
                {
                    (void)printf("ERROR: iotHubMessageHandle is NULL!\r\n");
                }
                else
                {
                    messages[i].messageTrackingId = (int)i;
                    
                    MAP_HANDLE propMap = IoTHubMessage_Properties(messages[i].messageHandle);
                    (void)sprintf_s(propText, sizeof(propText), "PropMsg_%d", (int)i);
                    if (Map_AddOrUpdate(propMap, "PropName", propText) != MAP_OK)
                    {
                        (void)printf("ERROR: Map_AddOrUpdate Failed!\r\n");
                    }

                    if (IoTHubClient_SendEventAsync(iotHubClientHandle, messages[i].messageHandle, SendConfirmationCallback, &messages[i]) != IOTHUB_CLIENT_OK)
                    {
                        (void)printf("ERROR: IoTHubClient_SendEventAsync..........FAILED!\r\n");
                    }
                    else
                    {
                        (void)printf("IoTHubClient_SendEventAsync accepted data for transmission to IoT Hub.\r\n");
                    }
                }
            }

            /* Wait for Commands. */
            (void)printf("Press any key to exit the application. \r\n");
            (void)getchar();
        }
        
        IoTHubClient_Destroy(iotHubClientHandle);
        platform_deinit();
    }
}
