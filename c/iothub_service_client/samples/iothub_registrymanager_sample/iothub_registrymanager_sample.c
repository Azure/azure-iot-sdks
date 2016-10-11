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

#include "iothub_registrymanager_sample.h"

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
            (void)printf("Device(%d)\r\n", orderNum);
        }
        else
        {
            (void)printf("Device\r\n");
        }
        (void)printf("    deviceId                    : %s\r\n", device->deviceId);
        (void)printf("    primaryKey                  : %s\r\n", device->primaryKey);
        (void)printf("    secondaryKey                : %s\r\n", device->secondaryKey);
        (void)printf("    generationId                : %s\r\n", device->generationId);
        (void)printf("    eTag                        : %s\r\n", device->eTag);
        if (device->connectionState == IOTHUB_DEVICE_CONNECTION_STATE_CONNECTED)
        {
            (void)printf("    connectionState             : Connected\r\n");
        }
        else
        {
            (void)printf("    connectionState             : Disconnected\r\n");
        }
        (void)printf("    connectionStateUpdatedTime  : %s\r\n", device->eTag);
        if (device->status == IOTHUB_DEVICE_STATUS_ENABLED)
        {
            (void)printf("    status                      : Enabled\r\n");
        }
        else
        {
            (void)printf("    status                      : Disabled\r\n");
        }
        (void)printf("    statusReason                : %s\r\n", device->statusReason);
        (void)printf("    statusUpdatedTime           : %s\r\n", device->statusUpdatedTime);
        (void)printf("    lastActivityTime            : %s\r\n", device->lastActivityTime);
        (void)printf("    cloudToDeviceMessageCount   : %zu\r\n", device->cloudToDeviceMessageCount);
    }
}

void iothub_registrymanager_sample_run(void)
{
    IOTHUB_REGISTRYMANAGER_RESULT result;

    IOTHUB_REGISTRY_DEVICE_CREATE deviceCreateInfo;
    IOTHUB_REGISTRY_DEVICE_UPDATE deviceUpdateInfo;

    xlogging_set_log_function(consolelogger_log);

    (void)printf("Calling IoTHubServiceClientAuth_CreateFromConnectionString with the connection string\r\n");
    IOTHUB_SERVICE_CLIENT_AUTH_HANDLE iotHubServiceClientHandle = IoTHubServiceClientAuth_CreateFromConnectionString(connectionString);
    if (iotHubServiceClientHandle == NULL)
    {
        (void)printf("IoTHubServiceClientAuth_CreateFromConnectionString failed\r\n");
    }
    else
    {
        IOTHUB_REGISTRYMANAGER_HANDLE iotHubRegistryManagerHandle = NULL;
        IOTHUB_DEVICE deviceInfo;
        SINGLYLINKEDLIST_HANDLE deviceList = NULL;
        IOTHUB_REGISTRY_STATISTICS registryStatistics;

        (void)printf("iotHubServiceClientHandle has been created successfully\r\n");
        (void)printf("Creating RegistryManager...\r\n");

        iotHubRegistryManagerHandle = IoTHubRegistryManager_Create(iotHubServiceClientHandle);

        (void)printf("RegistryManager has been created successfully\r\n");

        deviceCreateInfo.deviceId = deviceId;
        deviceCreateInfo.primaryKey = "";
        deviceCreateInfo.secondaryKey = "";

        // Create device
        result = IoTHubRegistryManager_CreateDevice(iotHubRegistryManagerHandle, &deviceCreateInfo, &deviceInfo);
        switch (result)
        {
        case IOTHUB_REGISTRYMANAGER_OK:
            (void)printf("IoTHubRegistryManager_CreateDevice: Device has been created successfully: deviceId=%s\r\n", deviceInfo.deviceId);
            break;
        case IOTHUB_REGISTRYMANAGER_DEVICE_EXIST:
            (void)printf("IoTHubRegistryManager_CreateDevice: Device already exists\r\n");
            break;
        case IOTHUB_REGISTRYMANAGER_ERROR:
            (void)printf("IoTHubRegistryManager_CreateDevice failed\r\n");
            break;
        default:
            (void)printf("IoTHubRegistryManager_CreateDevice failed with unknown error\r\n");
            break;
        }

        // Update device
        deviceUpdateInfo.deviceId = deviceId;
        deviceUpdateInfo.primaryKey = "aaabbbcccdddeeefffggghhhiiijjjkkklllmmmnnnoo";
        deviceUpdateInfo.secondaryKey = "111222333444555666777888999000aaabbbcccdddee";
        deviceUpdateInfo.status = IOTHUB_DEVICE_STATUS_DISABLED;
        result = IoTHubRegistryManager_UpdateDevice(iotHubRegistryManagerHandle, &deviceUpdateInfo);
        switch (result)
        {
        case IOTHUB_REGISTRYMANAGER_OK:
            (void)printf("IoTHubRegistryManager_UpdateDevice: Device has been updated successfully: deviceId=%s\r\n", deviceUpdateInfo.deviceId);
            break;
        case IOTHUB_REGISTRYMANAGER_ERROR:
            (void)printf("IoTHubRegistryManager_UpdateDevice failed\r\n");
            break;
        default:
            (void)printf("IoTHubRegistryManager_CreateDevice failed with unknown error\r\n");
            break;
        }

        // Get device
        deviceInfo.deviceId = "";
        deviceInfo.primaryKey = "";
        deviceInfo.secondaryKey = "";
        deviceInfo.generationId = "";
        deviceInfo.eTag = "";
        deviceInfo.connectionState = IOTHUB_DEVICE_CONNECTION_STATE_DISCONNECTED;
        deviceInfo.connectionStateUpdatedTime = "";
        deviceInfo.status = IOTHUB_DEVICE_STATUS_DISABLED;
        deviceInfo.statusReason = "";
        deviceInfo.statusUpdatedTime = "";
        deviceInfo.lastActivityTime = "";
        deviceInfo.cloudToDeviceMessageCount = 0;
        deviceInfo.isManaged = false;
        deviceInfo.configuration = "";
        deviceInfo.deviceProperties = "";
        deviceInfo.serviceProperties = "";

        result = IoTHubRegistryManager_GetDevice(iotHubRegistryManagerHandle, deviceCreateInfo.deviceId, &deviceInfo);
        switch (result)
        {
        case IOTHUB_REGISTRYMANAGER_OK:
            (void)printf("IoTHubRegistryManager_GetDevice: Successfully got device info: deviceId=%s\r\n", deviceInfo.deviceId);
            printDeviceInfo(&deviceInfo, -1);
            break;
        case IOTHUB_REGISTRYMANAGER_ERROR:
            (void)printf("IoTHubRegistryManager_GetDevice failed\r\n");
            break;
        default:
            (void)printf("IoTHubRegistryManager_GetDevice failed with unknown error\r\n");
            break;
        }

        // Delete device
        result = IoTHubRegistryManager_DeleteDevice(iotHubRegistryManagerHandle, deviceCreateInfo.deviceId);
        switch (result)
        {
        case IOTHUB_REGISTRYMANAGER_OK:
            (void)printf("IoTHubRegistryManager_DeleteDevice: Device has been deleted successfully: deviceId=%s\r\n", deviceCreateInfo.deviceId);
            break;
        case IOTHUB_REGISTRYMANAGER_ERROR:
            (void)printf("IoTHubRegistryManager_DeleteDevice: Delete device failed\r\n");
            break;
        default:
            (void)printf("IoTHubRegistryManager_DeleteDevice failed with unknown error\r\n");
            break;
        }

        // Get device list
        deviceList = singlylinkedlist_create();

        result = IoTHubRegistryManager_GetDeviceList(iotHubRegistryManagerHandle, 3, deviceList);
        switch (result)
        {
        case IOTHUB_REGISTRYMANAGER_OK:
            (void)printf("IoTHubRegistryManager_GetDeviceList: Successfully got device list\r\n");
            LIST_ITEM_HANDLE next_device = singlylinkedlist_get_head_item(deviceList);
            int i = 0;
            while (next_device != NULL)
            {
                IOTHUB_DEVICE* device = (IOTHUB_DEVICE*)singlylinkedlist_item_get_value(next_device);
                printDeviceInfo(device, i++);
                next_device = singlylinkedlist_get_next_item(next_device);
            }
            break;
        case IOTHUB_REGISTRYMANAGER_ERROR:
            (void)printf("IoTHubRegistryManager_GetDeviceList failed\r\n");
            break;
        default:
            (void)printf("IoTHubRegistryManager_GetDeviceList failed with unknown error\r\n");
            break;
        }

        // Get statistics
        result = IoTHubRegistryManager_GetStatistics(iotHubRegistryManagerHandle, &registryStatistics);
        switch (result)
        {
        case IOTHUB_REGISTRYMANAGER_OK:
            (void)printf("IoTHubRegistryManager_GetStatistics: Successfully got registry statistics\r\n");
            (void)printf("Total device count: %zu\r\n", registryStatistics.totalDeviceCount);
            (void)printf("Enabled device count: %zu\r\n", registryStatistics.enabledDeviceCount);
            (void)printf("Disabled device count: %zu\r\n", registryStatistics.disabledDeviceCount);
            break;
        case IOTHUB_REGISTRYMANAGER_ERROR:
            (void)printf("IoTHubRegistryManager_GetStatistics failed\r\n");
            break;
        default:
            (void)printf("IoTHubRegistryManager_GetStatistics failed with unknown error\r\n");
            break;
        }

        (void)printf("Calling IoTHubRegistryManager_Destroy...\r\n");
        IoTHubRegistryManager_Destroy(iotHubRegistryManagerHandle);

        (void)printf("Calling IoTHubServiceClientAuth_Destroy...\r\n");
        IoTHubServiceClientAuth_Destroy(iotHubServiceClientHandle);
    }
}