// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "serializer.h"
#include "iothub_client.h"

#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/platform.h"
#include "azure_c_shared_utility/threadapi.h"

#include "iothubtransportmqtt.h"

#include "iothub_client_sample_mqtt_dm.h"

/* enum values are in lower case per design */
#define FIRMWARE_UPDATE_STATUS_VALUES \
        waiting, \
        downloading, \
        downloadFailed, \
        downloadComplete, \
        applying, \
        applyFailed, \
        applyComplete
DEFINE_ENUM(FIRMWARE_UPDATE_STATUS, FIRMWARE_UPDATE_STATUS_VALUES)
DEFINE_ENUM_STRINGS(FIRMWARE_UPDATE_STATUS, FIRMWARE_UPDATE_STATUS_VALUES)


BEGIN_NAMESPACE(Contoso);

DECLARE_STRUCT(firmwareUpdate_t,
    ascii_char_ptr, status
);

DECLARE_MODEL(iothubDM_t,
    WITH_REPORTED_PROPERTY(ascii_char_ptr, firmwareVersion),
    WITH_REPORTED_PROPERTY(firmwareUpdate_t, firmwareUpdate)
);

DECLARE_MODEL(thingie_t,
    WITH_REPORTED_PROPERTY(iothubDM_t, iothubDM)
);

END_NAMESPACE(Contoso);

typedef struct PHYSICAL_DEVICE_TAG
{
    thingie_t   *iot_device;
    LOCK_HANDLE  status_lock;
    char *new_firmware_URI;
    FIRMWARE_UPDATE_STATUS status;
} PHYSICAL_DEVICE;

static bool set_physical_device_fwupdate_status(PHYSICAL_DEVICE *physical_device, FIRMWARE_UPDATE_STATUS newStatus)
{
    bool retValue;
    if (Lock(physical_device->status_lock) != LOCK_OK)
    {
        LogError("failed to acquire lock");
        retValue = false;
    }
    else
    {
        physical_device->status = newStatus;
        retValue = true;
        if (Unlock(physical_device->status_lock) != LOCK_OK)
        {
            LogError("failed to release lock");
        }
    }
    return retValue;
}

static FIRMWARE_UPDATE_STATUS get_physical_device_fwupdate_status(const PHYSICAL_DEVICE *physical_device)
{
    FIRMWARE_UPDATE_STATUS retValue;

    if (Lock(physical_device->status_lock) != LOCK_OK)
    {
        LogError("failed to acquire lock");
        retValue = waiting;
    }
    else
    {
        retValue = physical_device->status;
        if (Unlock(physical_device->status_lock) != LOCK_OK)
        {
            LogError("failed to release lock");
        }
    }
    return retValue;
}

static int do_firmware_update(void *param)
{
    int   retValue;
    PHYSICAL_DEVICE *physical_device = (PHYSICAL_DEVICE *) param;
    
    LogInfo("do_firmware_update('%s')", physical_device->new_firmware_URI);
    if (!set_physical_device_fwupdate_status(physical_device, downloading))
    {
        LogError("failed to update device status");
        retValue = -1;
    }
    else
    {
        bool result = device_download_firmware(physical_device->new_firmware_URI);
        if (result)
        {
            if (!set_physical_device_fwupdate_status(physical_device, downloadComplete))
            {
                LogError("failed to update device status");
                retValue = -1;
            }
            else
            {
                LogInfo("starting fw update");
                if (!set_physical_device_fwupdate_status(physical_device, applying))
                {
                    LogError("failed to update device status");
                    retValue = -1;
                }
                else
                {
                    result = device_update_firmware();
                    if (result)
                    {
                        device_reboot();
                        retValue = 0;
                    }
                    else
                    {
                        if (!set_physical_device_fwupdate_status(physical_device, applyFailed))
                        {
                            LogError("failed to update device status");
                        }
                        LogError("firmware update failed during apply");
                        retValue = -1;
                    }
                }
            }
        }
        else
        {
            LogError("failed to download new firmware image from '%s'", physical_device->new_firmware_URI);
            if (!set_physical_device_fwupdate_status(physical_device, downloadFailed))
            {
                LogError("failed to update device status");
            }
            retValue = -1;
        }
    }
    return retValue;
}

static void DeviceTwinCallback(int status_code, void* userContextCallback)
{
    (void)(userContextCallback);
    LogInfo("DeviceTwin CallBack: Status_code = %u", status_code);
}

#define FIRMWARE_UPDATE_METHOD_NAME "firmwareUpdate"

#define SERVER_ERROR 500
#define NOT_IMPLEMENTED 501
#define NOT_VALID 400
#define SERVER_SUCCESS 200


static int DeviceMethodCallback(const char* method_name, const unsigned char* payload, size_t size, unsigned char** response, size_t* resp_size, void* userContextCallback)
{
    PHYSICAL_DEVICE *physical_device = (PHYSICAL_DEVICE *) userContextCallback;

    int retValue;
    if (method_name == NULL)
    {
        LogError("invalid method name");
        retValue = NOT_VALID;
    }
    else if ((response == NULL) || (resp_size == NULL))
    {
        LogError("invalid response parameters");
        retValue = NOT_VALID;
    }
    else if (physical_device == NULL)
    {
        LogError("invalid user context callback data");
        retValue = NOT_VALID;
    }
    else if (strcmp(method_name, FIRMWARE_UPDATE_METHOD_NAME) == 0)
    {
        if (get_physical_device_fwupdate_status(physical_device) != waiting)
        {
            LogError("attempting to initiate a firmware update out of order");
            retValue = NOT_VALID;
        }
        else
        {
            if (payload == NULL)
            {
                LogError("passing invalid parameters payload");
                retValue = NOT_VALID;
            }
            else if (size == 0)
            {
                LogError("passing invalid parameters size");
                retValue = NOT_VALID;
            }
            else
            {
                physical_device->new_firmware_URI = malloc(size - 1);
                if (physical_device->new_firmware_URI == NULL)
                {
                    LogError("failed to allocate memory for URI parameter");
                    retValue = SERVER_ERROR;
                }
                else
                {
                    /* remove the quotation around the argument */
                    memcpy(physical_device->new_firmware_URI, payload + 1, size - 2);
                    physical_device->new_firmware_URI[size - 2] = '\0';

                    THREAD_HANDLE thread_apply;
                    THREADAPI_RESULT t_result = ThreadAPI_Create(&thread_apply, do_firmware_update, physical_device);
                    if (t_result == THREADAPI_OK)
                    {
                        retValue = SERVER_SUCCESS;
                        *response = NULL;
                        *resp_size = 0;
                    }
                    else
                    {
                        LogError("failed to start firmware update thread");
                        retValue = SERVER_ERROR;
                    }
                }
            }
        }
    }
    else
    {
        LogError("invalid method '%s'", method_name);
        retValue = NOT_VALID;
    }
 
    return retValue;
}

static bool send_reported(const PHYSICAL_DEVICE *physical_device, IOTHUB_CLIENT_HANDLE iotHubClientHandle)
{
    unsigned char *buffer;
    size_t         bufferSize;
    bool           retValue;

    thingie_t *iot_device = physical_device->iot_device;
    iot_device->iothubDM.firmwareVersion = device_get_firmware_version();
    if (iot_device->iothubDM.firmwareVersion == NULL)
    {
        retValue = false;
        LogError("Failed to retrieve the firmware version for device.");
        retValue = false;
    }
    else
    {
        iot_device->iothubDM.firmwareUpdate.status = (char *) ENUM_TO_STRING(FIRMWARE_UPDATE_STATUS, get_physical_device_fwupdate_status(physical_device));

        /*serialize the model using SERIALIZE_REPORTED_PROPERTIES */
        if (SERIALIZE_REPORTED_PROPERTIES(&buffer, &bufferSize, *iot_device) != CODEFIRST_OK)
        {
            retValue = false;
            LogError("Failed serializing reported state.");
            retValue = false;
        }
        else
        {
            /* send the data up stream*/
            IOTHUB_CLIENT_RESULT result = IoTHubClient_SendReportedState(iotHubClientHandle, buffer, bufferSize, DeviceTwinCallback, NULL);
            if (result != IOTHUB_CLIENT_OK)
            {
                retValue = false;
                LogError("Failure sending data!!!");
                retValue = false;
            }
            else
            {
                retValue = true;
            }
            free(buffer);
        }
        free(iot_device->iothubDM.firmwareVersion);
    }
    return retValue;
}

static PHYSICAL_DEVICE* physical_device_new(thingie_t *iot_device)
{
    PHYSICAL_DEVICE *retValue = malloc(sizeof(PHYSICAL_DEVICE));
    if (retValue == NULL)
    {
        LogError("failed to allocate memory for physical device structure");
    }
    else
    {
        retValue->status_lock = Lock_Init();
        if (retValue->status_lock == NULL)
        {
            LogError("failed to create a lock");
            free(retValue);
            retValue = NULL;
        }
        else
        {
            retValue->iot_device = iot_device;
            retValue->status = waiting;
            retValue->new_firmware_URI = NULL;
        }
    }

    return retValue;
}

static void physical_device_delete(PHYSICAL_DEVICE *physical_device)
{
    if (physical_device->new_firmware_URI != NULL)
    {
        free(physical_device->new_firmware_URI);
    }
    if (Lock_Deinit(physical_device->status_lock) == LOCK_ERROR)
    {
        LogError("failed to release lock handle");
    }
    free(physical_device);
}

static int iothub_client_sample_mqtt_dm_run(const char *connectionString, bool traceOn)
{
    LogInfo("Initialize Platform");

    int retValue;
    if (platform_init() != 0)
    {
        LogError("Failed to initialize the platform.");
        retValue = -4;
    }
    else
    {
        if (serializer_init(NULL) != SERIALIZER_OK)
        {
            LogError("Failed in serializer_init.");
            retValue = -5;
        }
        else
        {
            LogInfo("Instantiate the device.");
            thingie_t *iot_device = CREATE_MODEL_INSTANCE(Contoso, thingie_t);
            if (iot_device == NULL)
            {
                LogError("Failed on CREATE_MODEL_INSTANCE.");
                retValue = -6;
            }

            else
            {
                LogInfo("Initialize From Connection String.");
                IOTHUB_CLIENT_HANDLE iotHubClientHandle = IoTHubClient_CreateFromConnectionString(connectionString, MQTT_Protocol);
                if (iotHubClientHandle == NULL)
                {
                    LogError("iotHubClientHandle is NULL!");
                    retValue = -7;
                }
                else
                {
                    LogInfo("Device successfully connected.");
                    if (IoTHubClient_SetOption(iotHubClientHandle, "logtrace", &traceOn) != IOTHUB_CLIENT_OK)
                    {
                        LogError("failed to set logtrace option");
                    }

                    PHYSICAL_DEVICE *physical_device = physical_device_new(iot_device);
                    if (physical_device == NULL)
                    {
                        LogError("failed to make an iot device callback structure");
                        retValue = -8;
                    }
                    else
                    {
                        if (IoTHubClient_SetDeviceMethodCallback(iotHubClientHandle, DeviceMethodCallback, physical_device) != IOTHUB_CLIENT_OK)
                        {
                            LogError("failed to associate a callback for device methods");
                            retValue = -9;
                        }
                        else
                        {
                            bool keepRunning = send_reported(physical_device, iotHubClientHandle);
                            if (!keepRunning)
                            {
                                LogError("Failed to send initia device reported");
                                retValue = -10;
                            }
                            else
                            {
                                FIRMWARE_UPDATE_STATUS oldStatus = get_physical_device_fwupdate_status(physical_device);
                                while (keepRunning)
                                {
                                    FIRMWARE_UPDATE_STATUS newStatus = get_physical_device_fwupdate_status(physical_device);

                                    /* send reported only if the status changes */
                                    if (newStatus != oldStatus)
                                    {
                                        oldStatus = newStatus;
                                        keepRunning = send_reported(physical_device, iotHubClientHandle);
                                    }
                                    ThreadAPI_Sleep(1000);
                                }
                                retValue = 0;
                            }
                        }
                        physical_device_delete(physical_device);
                    }
                    IoTHubClient_Destroy(iotHubClientHandle);
                }
                DESTROY_MODEL_INSTANCE(iot_device);
            }
            serializer_deinit();
        }
        platform_deinit();
    }

    return retValue;
}

int main(int argc, char *argv[])
{
    int   exitCode = 0;
    bool  isService = true;
    bool  traceOn = false;
    char *connectionString = NULL;

    for (int ii = 1; ii < argc; ++ii)
    {
        if (0 == strcmp(argv[ii], "-console"))
        {
            isService = false;
        }
        else if (0 == strcmp(argv[ii], "-cs"))
        {
            ++ii;
            if (ii < argc)
            {
                if (mallocAndStrcpy_s(&connectionString, argv[ii]) != 0)
                {
                    LogError("failed to allocate memory for connection string");
                    exitCode = -1;
                }
            }
        }
        else if (0 == strcmp(argv[ii], "-logging"))
        {
            traceOn = true;
        }
    }

    if (exitCode == 0)
    {
        if (connectionString == NULL)
        {
            connectionString = device_get_connection_string();
        }

        if (connectionString == NULL)
        {
            LogError("connection string is NULL");
            exitCode = -2;
        }
        else
        {
            if (isService)
            {
                traceOn = false;
                if (device_run_service() == false)
                {
                    LogError("Failed to run as a service.");
                    exitCode = -3;
                }
            }

            if (exitCode == 0)
            {
                exitCode = iothub_client_sample_mqtt_dm_run(connectionString, traceOn);
            }

            free(connectionString);
        }
    }

    LogInfo("Exit Code: %d", exitCode);
    return exitCode;
}
