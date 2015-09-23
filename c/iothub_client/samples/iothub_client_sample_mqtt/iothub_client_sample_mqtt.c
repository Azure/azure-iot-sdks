/*
Microsoft Azure IoT Device Libraries

Copyright (c) Microsoft Corporation
All rights reserved.

MIT License

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the Software), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED *AS IS*, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <stdio.h>
#include <stdlib.h>

#include "iothub_client.h"
#include "iothub_message.h"
#include "threadapi.h"
#include "crt_abstractions.h"
#include "iothubtransportmqtt.h"

static const char* connectionString = "[device connection string]";
static int callbackCounter;

DEFINE_ENUM_STRINGS(IOTHUB_CLIENT_CONFIRMATION_RESULT, IOTHUB_CLIENT_CONFIRMATION_RESULT_VALUES);

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
    if (IoTHubMessage_GetByteArray(message, (const unsigned char**)&buffer, &size) != IOTHUB_MESSAGE_OK)
    {
        (void)printf("unable to retrieve the message data\r\n");
    }
    else
    {
        (void)printf("Received Message [%d] with Data: <<<%.*s>>> & Size=%d\r\n", *counter, (int)size, buffer, (int)size);
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
                (void)printf("Message Properties:\r\n");
                for (size_t index = 0; index < propertyCount; index++)
                {
                    (void)printf("\tKey: %s Value: %s\r\n", keys[index], values[index]);
                }
                (void)printf("\r\n");
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

void iothub_client_sample_mqtt_run(void)
{
    IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle;

    EVENT_INSTANCE messages[MESSAGE_COUNT];

    srand((unsigned int)time(NULL));
    double avgWindSpeed = 10.0;
    
    callbackCounter = 0;
    int receiveContext = 0;

    (void)printf("Starting the IoTHub client sample MQTT...\r\n");

    if ((iotHubClientHandle = IoTHubClient_LL_CreateFromConnectionString(connectionString, MQTT_Protocol)) == NULL)
    {
        (void)printf("ERROR: iotHubClientHandle is NULL!\r\n");
    }
    else
    {
        unsigned int timeout = 241000;
        if (IoTHubClient_LL_SetOption(iotHubClientHandle, "timeout", &timeout) != IOTHUB_CLIENT_OK)
        {
            (void)printf("failure to set option \"timeout\"\r\n");
        }

        /* Setting Message call back, so we can receive Commands. */
        if (IoTHubClient_LL_SetMessageCallback(iotHubClientHandle, ReceiveMessageCallback, &receiveContext) != IOTHUB_CLIENT_OK)
        {
            (void)printf("ERROR: IoTHubClient_LL_SetMessageCallback..........FAILED!\r\n");
        }
        else
        {
            (void)printf("IoTHubClient_LL_SetMessageCallback...successful.\r\n");

            /* Now that we are ready to receive commands, let's send some messages */
            for (int i = 0; i < MESSAGE_COUNT; i++)
            {
                sprintf_s(msgText, sizeof(msgText), "{\"deviceId\":\"myFirstDevice\",\"windSpeed\":%.2f}", avgWindSpeed + (rand() % 4 + 2));
                if ((messages[i].messageHandle = IoTHubMessage_CreateFromByteArray((const unsigned char*)msgText, strlen(msgText))) == NULL)
                {
                    (void)printf("ERROR: iotHubMessageHandle is NULL!\r\n");
                }
                else
                {
                    messages[i].messageTrackingId = i;

                    MAP_HANDLE propMap = IoTHubMessage_Properties(messages[i].messageHandle);
                    sprintf_s(propText, sizeof(propText), "PropMsg_%d", i);
                    if (Map_AddOrUpdate(propMap, "PropName", propText) != MAP_OK)
                    {
                        (void)printf("ERROR: Map_AddOrUpdate Failed!\r\n");
                    }

                    if (IoTHubClient_LL_SendEventAsync(iotHubClientHandle, messages[i].messageHandle, SendConfirmationCallback, &messages[i]) != IOTHUB_CLIENT_OK)
                    {
                        (void)printf("ERROR: IoTHubClient_LL_SendEventAsync..........FAILED!\r\n");
                    }
                    else
                    {
                        (void)printf("IoTHubClient_LL_SendEventAsync accepted message [%d] for transmission to IoT Hub.\r\n", i);
                    }

                }
            }
        }

        /* Wait for Commands. */
        while (1)
        {
            IoTHubClient_LL_DoWork(iotHubClientHandle);
            ThreadAPI_Sleep(1);
        }

        IoTHubClient_LL_Destroy(iotHubClientHandle);
    }
}
