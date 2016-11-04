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
#include "azure_c_shared_utility/uniqueid.h"

#include "iothub_devicetwin_sample.h"

#include "connection_string_parser.h"
#include "iothub_service_client_auth.h"
#include "iothub_devicetwin.h"

/* String containing Hostname, SharedAccessKeyName and SharedAccessKey in the format:                       */
/* "HostName=<host_name>;SharedAccessKeyName=<shared_access_key_name>;SharedAccessKey=<shared_access_key>" */
static const char* connectionString = "[IoTHub Connection String]";
static const char* deviceId = "[Device Id]";

void iothub_devicetwin_sample_run(void)
{
    //IOTHUB_TWIN_REQUEST_GET               GET      {iot hub}/twins/{device id}                     // Get device twin  
    //IOTHUB_TWIN_REQUEST_UPDATE            PATCH    {iot hub}/twins/{device id}                     // Partally update device twin
    //IOTHUB_TWIN_REQUEST_REPLACE_TAGS      PUT      {iot hub}/twins/{device id}/tags                // Replace update tags
    //IOTHUB_TWIN_REQUEST_REPLACE_DESIRED   PUT      {iot hub}/twins/{device id}/properties/desired  // Replace update desired properties
    //IOTHUB_TWIN_REQUEST_UPDATE_DESIRED    PATCH    {iot hub}/twins/{device id}/properties/desired  // Partially update desired properties

    xlogging_set_log_function(consolelogger_log);

    (void)printf("Calling IoTHubServiceClientAuth_CreateFromConnectionString with the connection string\r\n");
    IOTHUB_SERVICE_CLIENT_AUTH_HANDLE iotHubServiceClientHandle = IoTHubServiceClientAuth_CreateFromConnectionString(connectionString);
    if (iotHubServiceClientHandle == NULL)
    {
        (void)printf("IoTHubServiceClientAuth_CreateFromConnectionString failed\r\n");
    }
    else
    {
        (void)printf("iotHubServiceClientHandle has been created successfully\r\n");

        IOTHUB_SERVICE_CLIENT_DEVICE_TWIN_HANDLE serviceClientDeviceTwinHandle = IoTHubDeviceTwin_Create(iotHubServiceClientHandle);
        if (serviceClientDeviceTwinHandle == NULL)
        {
            (void)printf("IoTHubDeviceTwin_Create failed\r\n");
        }
        else
        {
            (void)printf("Getting DeviceTwin...\r\n");

            char* deviceTwinJson;

            if ((deviceTwinJson = IoTHubDeviceTwin_GetTwin(serviceClientDeviceTwinHandle, deviceId)) == NULL)
            {
                (void)printf("IoTHubDeviceTwin_GetTwin failed\r\n");
            }
            else
            {
                (void)printf("\r\nDeviceTwin:\r\n");
                printf("%s\r\n", deviceTwinJson);

                const char* updateJson = "{\"properties\":{\"desired\":{\"telemetryInterval\":30}}}";
                char* updatedDeviceTwinJson;

                if ((updatedDeviceTwinJson = IoTHubDeviceTwin_UpdateTwin(serviceClientDeviceTwinHandle, deviceId, updateJson)) == NULL)
                {
                    (void)printf("IoTHubDeviceTwin_UpdateTwin failed\r\n");
                }
                else
                {
                    (void)printf("\r\nDeviceTwin has been successfully updated (partial update):\r\n");
                    printf("%s\r\n", updatedDeviceTwinJson);

                    free(updatedDeviceTwinJson);
                }

                free(deviceTwinJson);
            }
            IoTHubDeviceTwin_Destroy(serviceClientDeviceTwinHandle);
        }
        IoTHubServiceClientAuth_Destroy(iotHubServiceClientHandle);
    }
}