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
#include "iothub_client_ll.h"
#include "iothub_client_options.h"
#include "iothub_message.h"
#include "azure_c_shared_utility/threadapi.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/platform.h"

#ifdef USE_MQTT
#include "iothubtransportmqtt.h"
#endif
#ifdef USE_HTTP
#include "iothubtransporthttp.h"
#endif
#ifdef USE_AMQP
#include "iothubtransportamqp.h"
#endif
#endif

#ifdef MBED_BUILD_TIMESTAMP
#include "certs.h"
#endif // MBED_BUILD_TIMESTAMP

/*String containing Hostname, Device Id in the format:                         */
/*  "HostName=<host_name>;DeviceId=<device_id>;x509=true"                      */
static const char* connectionString = "[device connection string]";

static const char* x509certificate =
"-----BEGIN CERTIFICATE-----""\n"
"MIICpDCCAYwCCQCfIjBnPxs5TzANBgkqhkiG9w0BAQsFADAUMRIwEAYDVQQDDAls""\n"
"b2NhbGhvc3QwHhcNMTYwNjIyMjM0MzI3WhcNMTYwNjIzMjM0MzI3WjAUMRIwEAYD""\n"
"...""\n"
"+s88wBF907s1dcY45vsG0ldE3f7Y6anGF60nUwYao/fN/eb5FT5EHANVMmnK8zZ2""\n"
"tjWUt5TFnAveFoQWIoIbtzlTbOxUFwMrQFzFXOrZoDJmHNWc2u6FmVAkowoOSHiE""\n"
"dkyVdoGPCXc=""\n"
"-----END CERTIFICATE-----";

static const char* x509privatekey =
"-----BEGIN RSA PRIVATE KEY-----""\n"
"MIIEpQIBAAKCAQEA0zKK+Uu5I0nXq2V6+2gbdCsBXZ6j1uAgU/clsCohEAek1T8v""\n"
"qj2tR9Mz9iy9RtXPMHwzcQ7aXDaz7RbHdw7tYXqSw8iq0Mxq2s3p4mo6gd5vEOiN""\n"
"...""\n"
"EyePNmkCgYEAng+12qvs0de7OhkTjX9FLxluLWxfN2vbtQCWXslLCG+Es/ZzGlNF""\n"
"SaqVID4EAUgUqFDw0UO6SKLT+HyFjOr5qdHkfAmRzwE/0RBN69g2qLDN3Km1Px/k""\n"
"xyJyxc700uV1eKiCdRLRuCbUeecOSZreh8YRIQQXoG8uotO5IttdVRc=""\n"
"-----END RSA PRIVATE KEY-----";

static int callbackCounter;
static bool g_continueRunning;
static char msgText[1024];
static char propText[1024];
#define MESSAGE_COUNT       5
#define DOWORK_LOOP_NUM     3

typedef struct EVENT_INSTANCE_TAG
{
    IOTHUB_MESSAGE_HANDLE messageHandle;
    size_t messageTrackingId;  // For tracking the messages within the user callback.
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
        (void)printf("Received Message [%d] with Data: <<<%.*s>>> & Size=%zu\r\n", *counter, (int)size, buffer, size);
		if (size == (strlen("quit") * sizeof(char)) && memcmp(buffer, "quit", size) == 0)
        {
            g_continueRunning = false;
        }
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
    (void)printf("Confirmation[%d] received for message tracking id = %zu with result = %s\r\n", callbackCounter, eventInstance->messageTrackingId, ENUM_TO_STRING(IOTHUB_CLIENT_CONFIRMATION_RESULT, result));
    /* Some device specific action code goes here... */
    callbackCounter++;
    IoTHubMessage_Destroy(eventInstance->messageHandle);
}

int main(void)
{
    IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle;

    EVENT_INSTANCE messages[MESSAGE_COUNT];
    double avgWindSpeed = 10.0;
    int receiveContext = 0;
    g_continueRunning = true;

    srand((unsigned int)time(NULL));

    IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol;
#ifdef USE_HTTP
    protocol = HTTP_Protocol;
#endif
#ifdef USE_MQTT
    protocol = MQTT_Protocol;
#endif
#ifdef USE_AMQP
    protocol = AMQP_Protocol;
#endif

    callbackCounter = 0;

    if (platform_init() != 0)
    {
        printf("Failed to initialize the platform.\r\n");
    }
    else
    {
        (void)printf("Starting the IoTHub client sample x509...\r\n");

        if ((iotHubClientHandle = IoTHubClient_LL_CreateFromConnectionString(connectionString, protocol)) == NULL)
        {
            (void)printf("ERROR: iotHubClientHandle is NULL!\r\n");
        }
        else
        {
#ifdef USE_HTTP
            if (protocol == HTTP_Protocol)
            {
                unsigned int timeout = 241000;
                // Because it can poll "after 9 seconds" polls will happen effectively // at ~10 seconds.
                // Note that for scalabilty, the default value of minimumPollingTime
                // is 25 minutes. For more information, see:
                // https://azure.microsoft.com/documentation/articles/iot-hub-devguide/#messaging
                unsigned int minimumPollingTime = 9;
                if (IoTHubClient_LL_SetOption(iotHubClientHandle, "timeout", &timeout) != IOTHUB_CLIENT_OK)
                {
                    printf("failure to set option \"timeout\"\r\n");
                }

                if (IoTHubClient_LL_SetOption(iotHubClientHandle, OPTION_MIN_POLLING_TIME, &minimumPollingTime) != IOTHUB_CLIENT_OK)
                {
                    printf("failure to set option \"MinimumPollingTime\"\r\n");
                }
            }
            else
#endif
            {
                bool trace = true;
                (void)IoTHubClient_LL_SetOption(iotHubClientHandle, OPTION_LOG_TRACE, &trace);
            }

            /*this brings in x509 privateKey and certificate*/
            if (
                (IoTHubClient_LL_SetOption(iotHubClientHandle, OPTION_X509_CERT, x509certificate) != IOTHUB_CLIENT_OK) ||
                (IoTHubClient_LL_SetOption(iotHubClientHandle, OPTION_X509_PRIVATE_KEY, x509privatekey) != IOTHUB_CLIENT_OK)
                )
            {
                printf("failure to set options for x509, aborting\r\n");
            }
            else
            {
#ifdef MBED_BUILD_TIMESTAMP
                // For mbed add the certificate information
                if (IoTHubClient_LL_SetOption(iotHubClientHandle, "TrustedCerts", certificates) != IOTHUB_CLIENT_OK)
                {
                    printf("failure to set option \"TrustedCerts\"\r\n");
                }
#endif // MBED_BUILD_TIMESTAMP

                /* Setting Message call back, so we can receive Commands. */
                if (IoTHubClient_LL_SetMessageCallback(iotHubClientHandle, ReceiveMessageCallback, &receiveContext) != IOTHUB_CLIENT_OK)
                {
                    (void)printf("ERROR: IoTHubClient_LL_SetMessageCallback..........FAILED!\r\n");
                }
                else
                {
                    (void)printf("IoTHubClient_LL_SetMessageCallback...successful.\r\n");

                    /* Now that we are ready to receive commands, let's send some messages */
                    size_t iterator = 0;
                    do
                    {
                        if (iterator < MESSAGE_COUNT)
                        {
                            sprintf_s(msgText, sizeof(msgText), "{\"deviceId\": \"myFirstDevice\",\"windSpeed\": %.2f}", avgWindSpeed + (rand() % 4 + 2));
                            if ((messages[iterator].messageHandle = IoTHubMessage_CreateFromByteArray((const unsigned char*)msgText, strlen(msgText))) == NULL)
                            {
                                (void)printf("ERROR: iotHubMessageHandle is NULL!\r\n");
                            }
                            else
                            {
                                MAP_HANDLE propMap;

                                messages[iterator].messageTrackingId = iterator;

                                propMap = IoTHubMessage_Properties(messages[iterator].messageHandle);
                                (void)sprintf_s(propText, sizeof(propText), "PropMsg_%zu", iterator);
                                if (Map_AddOrUpdate(propMap, "PropName", propText) != MAP_OK)
                                {
                                    (void)printf("ERROR: Map_AddOrUpdate Failed!\r\n");
                                }

                                if (IoTHubClient_LL_SendEventAsync(iotHubClientHandle, messages[iterator].messageHandle, SendConfirmationCallback, &messages[iterator]) != IOTHUB_CLIENT_OK)
                                {
                                    (void)printf("ERROR: IoTHubClient_LL_SendEventAsync..........FAILED!\r\n");
                                }
                                else
                                {
                                    (void)printf("IoTHubClient_LL_SendEventAsync accepted message [%zu] for transmission to IoT Hub.\r\n", iterator);
                                }

                            }
                        }
                        IoTHubClient_LL_DoWork(iotHubClientHandle);
                        ThreadAPI_Sleep(1);

                        iterator++;
                    } while (g_continueRunning);

                    (void)printf("iothub_client_sample_x509 has gotten quit message, call DoWork %d more time to complete final sending...\r\n", DOWORK_LOOP_NUM);
                    for (size_t index = 0; index < DOWORK_LOOP_NUM; index++)
                    {
                        IoTHubClient_LL_DoWork(iotHubClientHandle);
                        ThreadAPI_Sleep(1);
                    }
                }
            }
            IoTHubClient_LL_Destroy(iotHubClientHandle);
        }
        platform_deinit();
    }
}
