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
#include "iothub_registrymanager.h"

/* String containing Hostname, SharedAccessKeyName and SharedAccessKey in the format:                       */
/* "HostName=<host_name>;SharedAccessKeyName=<shared_access_key_name>;SharedAccessKey=<shared_access_key>" */
static const char* connectionString = "[IoTHub Connection String]";
static const char* deviceId = "[Device Id]";

void printDeviceInfo(IOTHUB_DEVICE* device, int orderNum)
{
    if ((device != NULL) && (device->deviceId != NULL))
    {
        if (orderNum >= 0)
        {
            (void)printf("Device(%d)\n", orderNum);
        }
        else
        {
            (void)printf("Device\n");
        }
        (void)printf("    deviceId                    : %s\n", device->deviceId);
        (void)printf("    primaryKey                  : %s\n", device->primaryKey);
        (void)printf("    secondaryKey                : %s\n", device->secondaryKey);
        (void)printf("    generationId                : %s\n", device->generationId);
        (void)printf("    eTag                        : %s\n", device->eTag);
        if (device->connectionState == IOTHUB_DEVICE_CONNECTION_STATE_CONNECTED)
        {
            (void)printf("    connectionState             : Connected\n");
        }
        else
        {
            (void)printf("    connectionState             : Disconnected\n");
        }
        (void)printf("    connectionStateUpdatedTime  : %s\n", device->eTag);
        if (device->status == IOTHUB_DEVICE_STATUS_ENABLED)
        {
            (void)printf("    status                      : Enabled\n");
        }
        else
        {
            (void)printf("    status                      : Disabled\n");
        }
        (void)printf("    statusReason                : %s\n", device->statusReason);
        (void)printf("    statusUpdatedTime           : %s\n", device->statusUpdatedTime);
        (void)printf("    lastActivityTime            : %s\n", device->lastActivityTime);
        (void)printf("    cloudToDeviceMessageCount   : %zu\n", device->cloudToDeviceMessageCount);
    }
}

void iothub_service_client_sample_run(void)
{
    IOTHUB_REGISTRYMANAGER_RESULT result;

    IOTHUB_REGISTRY_DEVICE_CREATE deviceCreateInfo;
    IOTHUB_REGISTRY_DEVICE_UPDATE deviceUpdateInfo;

    (void)printf("Calling IoTHubServiceClientAuth_CreateFromConnectionString with the connection string\n");
    IOTHUB_SERVICE_CLIENT_AUTH_HANDLE iotHubServiceClientHandle = IoTHubServiceClientAuth_CreateFromConnectionString(connectionString);
    if (iotHubServiceClientHandle == NULL)
    {
        (void)printf("IoTHubServiceClientAuth_CreateFromConnectionString failed\n");
    }
    else
    {
        (void)printf("iotHubServiceClientHandle has been created successfully\n");

        (void)printf("Creating RegistryManager...\n");
        IOTHUB_REGISTRYMANAGER_HANDLE iotHubRegistryManagerHandle = IoTHubRegistryManager_Create(iotHubServiceClientHandle);
        (void)printf("RegistryManager has been created successfully\n");

        deviceCreateInfo.deviceId = deviceId;
        deviceCreateInfo.primaryKey = "";
        deviceCreateInfo.secondaryKey = "";

        IOTHUB_DEVICE deviceInfo;

        // Create device
        result = IoTHubRegistryManager_CreateDevice(iotHubRegistryManagerHandle, &deviceCreateInfo, &deviceInfo);
        if (result == IOTHUB_REGISTRYMANAGER_OK)
        {
            (void)printf("IoTHubRegistryManager_CreateDevice: Device has been created successfully: deviceId=%s\n", deviceInfo.deviceId);
        }
        else if (result == IOTHUB_REGISTRYMANAGER_DEVICE_EXIST)
        {
            (void)printf("IoTHubRegistryManager_CreateDevice: Device already exists\n");
        }
        else if (result == IOTHUB_REGISTRYMANAGER_ERROR)
        {
            (void)printf("IoTHubRegistryManager_CreateDevice failed\n");
        }

        // Update device
        deviceUpdateInfo.deviceId = deviceId;
        deviceUpdateInfo.primaryKey = "aaabbbcccdddeeefffggghhhiiijjjkkklllmmmnnnoo";
        deviceUpdateInfo.secondaryKey = "111222333444555666777888999000aaabbbcccdddee";
        deviceUpdateInfo.status = IOTHUB_DEVICE_STATUS_DISABLED;
        result = IoTHubRegistryManager_UpdateDevice(iotHubRegistryManagerHandle, &deviceUpdateInfo);
        if (result == IOTHUB_REGISTRYMANAGER_OK)
        {
            (void)printf("IoTHubRegistryManager_UpdateDevice: Device has been updated successfully: deviceId=%s\n", deviceUpdateInfo.deviceId);
        }
        else if (result == IOTHUB_REGISTRYMANAGER_ERROR)
        {
            (void)printf("IoTHubRegistryManager_UpdateDevice failed\n");
        }

        // Get device
        (void)memset(&deviceInfo, 0, sizeof(IOTHUB_DEVICE));
        result = IoTHubRegistryManager_GetDevice(iotHubRegistryManagerHandle, deviceCreateInfo.deviceId, &deviceInfo);
        if (result == IOTHUB_REGISTRYMANAGER_OK)
        {
            (void)printf("IoTHubRegistryManager_GetDevice: Successfully got device info: deviceId=%s\n", deviceInfo.deviceId);
            printDeviceInfo(&deviceInfo, -1);
        }
        else if (result == IOTHUB_REGISTRYMANAGER_ERROR)
        {
            (void)printf("IoTHubRegistryManager_GetDevice failed\n");
        }

        // Delete device
        result = IoTHubRegistryManager_DeleteDevice(iotHubRegistryManagerHandle, deviceCreateInfo.deviceId);
        if (result == IOTHUB_REGISTRYMANAGER_OK)
        {
            (void)printf("IoTHubRegistryManager_DeleteDevice: Device has been deleted successfully: deviceId=%s\n", deviceCreateInfo.deviceId);
        }
        else if (result == IOTHUB_REGISTRYMANAGER_ERROR)
        {
            (void)printf("IoTHubRegistryManager_DeleteDevice: Delete device failed\n");
        }

        // Get device list
        SINGLYLINKEDLIST_HANDLE deviceList = singlylinkedlist_create();

        result = IoTHubRegistryManager_GetDeviceList(iotHubRegistryManagerHandle, 3, deviceList);
        if (result == IOTHUB_REGISTRYMANAGER_OK)
        {
            (void)printf("IoTHubRegistryManager_GetDeviceList: Successfully got device list\n");
            LIST_ITEM_HANDLE next_device = singlylinkedlist_get_head_item(deviceList);
            int i = 0;
            while (next_device != NULL)
            {
                IOTHUB_DEVICE* device = (IOTHUB_DEVICE*)singlylinkedlist_item_get_value(next_device);
                printDeviceInfo(device, i++);
                next_device = singlylinkedlist_get_next_item(next_device);
            }
        }
        else if (result == IOTHUB_REGISTRYMANAGER_ERROR)
        {
            (void)printf("IoTHubRegistryManager_GetDeviceList failed\n");
        }

        // Get statistics
        IOTHUB_REGISTRY_STATISTICS registryStatistics;
        result = IoTHubRegistryManager_GetStatistics(iotHubRegistryManagerHandle, &registryStatistics);
        if (result == IOTHUB_REGISTRYMANAGER_OK)
        {
            (void)printf("IoTHubRegistryManager_GetStatistics: Successfully got registry statistics\n");
            (void)printf("Total device count: %zu\n", registryStatistics.totalDeviceCount);
            (void)printf("Enabled device count: %zu\n", registryStatistics.enabledDeviceCount);
            (void)printf("Disabled device count: %zu\n", registryStatistics.disabledDeviceCount);
        }
        else if (result == IOTHUB_REGISTRYMANAGER_ERROR)
        {
            (void)printf("IoTHubRegistryManager_GetStatistics failed\n");
        }

        singlylinkedlist_destroy(deviceList);

        free((char*)deviceInfo.deviceId);
        free((char*)deviceInfo.primaryKey);
        free((char*)deviceInfo.secondaryKey);
        free((char*)deviceInfo.generationId);
        free((char*)deviceInfo.eTag);
        free((char*)deviceInfo.connectionStateUpdatedTime);
        free((char*)deviceInfo.statusReason);
        free((char*)deviceInfo.statusUpdatedTime);
        free((char*)deviceInfo.lastActivityTime);
        free((char*)deviceInfo.configuration);
        free((char*)deviceInfo.deviceProperties);
        free((char*)deviceInfo.serviceProperties);


        (void)printf("Calling IoTHubRegistryManager_Destroy...\n");
        IoTHubRegistryManager_Destroy(iotHubRegistryManagerHandle);

        (void)printf("Calling IoTHubServiceClientAuth_Destroy...\n");
        IoTHubServiceClientAuth_Destroy(iotHubServiceClientHandle);
    }
}