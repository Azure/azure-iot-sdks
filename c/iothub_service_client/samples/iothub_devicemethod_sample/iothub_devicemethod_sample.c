// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/threadapi.h"
#include "azure_c_shared_utility/map.h"
#include "azure_c_shared_utility/lock.h"
#include "azure_c_shared_utility/strings.h"
#include "azure_c_shared_utility/string_tokenizer.h"
#include "azure_c_shared_utility/consolelogger.h"
#include "azure_c_shared_utility/xlogging.h"

#include "connection_string_parser.h"
#include "iothub_service_client_auth.h"
#include "iothub_devicemethod.h"
#include "iothub_devicemethod_sample.h"

/* String containing Hostname, SharedAccessKeyName and SharedAccessKey in the format:                       */
/* "HostName=<host_name>;SharedAccessKeyName=<shared_access_key_name>;SharedAccessKey=<shared_access_key>" */
static const char* connectionString = "";
static const char* deviceId = "";
static const char* methodName = "[Method Name]";
static const char* methodPayload = "[Method Payload]";
static unsigned int timeout = 60;

void iothub_devicemethod_sample_run(void)
{
    (void)printf("Calling IoTHubServiceClientAuth_CreateFromConnectionString with connectionString\n");
    IOTHUB_SERVICE_CLIENT_AUTH_HANDLE iotHubServiceClientHandle = IoTHubServiceClientAuth_CreateFromConnectionString(connectionString);
    if (iotHubServiceClientHandle == NULL)
    {
        (void)printf("IoTHubServiceClientAuth_CreateFromConnectionString failed\n");
    }
    else
    {
        (void)printf("iotHubServiceClientDeviceMethodHandle has been created successfully\n");

        (void)printf("Creating serviceClientDeviceMethodHandle...\n");
        IOTHUB_SERVICE_CLIENT_DEVICE_METHOD_HANDLE serviceClientDeviceMethodHandle = IoTHubDeviceMethod_Create(iotHubServiceClientHandle);
        if (serviceClientDeviceMethodHandle == NULL)
        {
            (void)printf("IoTHubDeviceMethod_Create failed\n");
        }
        else
        {
            (void)printf("serviceClientDeviceMethodHandle has been created successfully\n");

            (void)printf("Invoking method on device...\n");

            int responseStatus;
            unsigned char* responsePayload;
            size_t responsePayloadSize;
            IOTHUB_DEVICE_METHOD_RESULT invokeResult = IoTHubDeviceMethod_Invoke(serviceClientDeviceMethodHandle, deviceId, methodName, methodPayload, timeout, &responseStatus, &responsePayload, &responsePayloadSize);
            if (invokeResult == IOTHUB_DEVICE_METHOD_OK)
            {
                printf("\r\nDevice Method called\r\n");
                printf("Device Method name:    %s\r\n", methodName);
                printf("Device Method payload: %s\r\n", methodPayload);

                printf("\r\nResponse status: %d\r\n", responseStatus);
                printf("Response payload: %.*s\r\n", (int)responsePayloadSize, (const char*)responsePayload);

                free(responsePayload);
            }
            else
            {
                (void)printf("IoTHubDeviceMethod_Invoke failed with result: %d\n", invokeResult);
            }

            (void)printf("Calling IoTHubDeviceMethod_Destroy...\n");
            IoTHubDeviceMethod_Destroy(serviceClientDeviceMethodHandle);
        }

        (void)printf("Calling IoTHubServiceClientAuth_Destroy...\n");
        IoTHubServiceClientAuth_Destroy(iotHubServiceClientHandle);
    }
}