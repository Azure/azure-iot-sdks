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


static int do_firmware_update(void *param)
{
    int   retValue;
    char *uri = param;
    
    LogInfo("do_firmware_update('%s')", uri);
    bool result = device_download_firmware(uri);
    free(uri);
    if (result)
    {
        LogInfo("starting fw update");
        result = device_update_firmware();
        if (result)
        {
            retValue = 0;
        }
        else
        {
            LogError("fiRmware update failed during apply");
            retValue = -1;
        }
    }
    else
    {
        LogError("failed to download new firmware image from", uri);
        retValue = -1;
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


/**
 * @brief     This function will be called by the framework when a method call is received from the service.
 *
 *
 * @param
 * @param
 *
 *
 *
 *
 *
 *
 *
 * @return
 *

    Description:
            

    Parameters:
        <param>method_name</param>, <param>response</param> and <param>resp_size</param> are guaranteed by the frmaework to be valid.
        <param>payload</param> and <param>size</param> will be valid based on the method (given in method_name). If a method does not
            require any parameters, payload and size maybe NULL and zero respectively.

    Return:
        an HTTP return code as appropriate.
        contents of <param>response</param> and <param>resp_size</param> are application dependent and can be NULL and zero respectively.
*/
static int DeviceMethodCallback(const char* method_name, const unsigned char* payload, size_t size, unsigned char** response, size_t* resp_size, void* userContextCallback)
{
    (void)userContextCallback;

    int retValue;
    if (strcmp(method_name, FIRMWARE_UPDATE_METHOD_NAME) == 0)
    {
        if (device_get_firmware_update_status() != waiting)
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
                char *uri = malloc(size - 1);
                if (uri == NULL)
                {
                    LogError("failed to allocate memory for URI parameter");
                    retValue = SERVER_ERROR;
                }
                else
                {
                    /* remove the quotation around the argument */
                    memcpy(uri, payload + 1, size - 2);
                    uri[size - 2] = '\0';

                    THREAD_HANDLE thread_apply;
                    THREADAPI_RESULT t_result = ThreadAPI_Create(&thread_apply, do_firmware_update, uri);
                    if (t_result == THREADAPI_OK)
                    {
                        retValue = SERVER_SUCCESS;
                        *response = NULL;
                        *resp_size = 0;
                    }
                    else
                    {
                        LogError("failed to start firmware update thread");
                        free(uri);
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

static bool send_reported(thingie_t *iot_device, IOTHUB_CLIENT_HANDLE iotHubClientHandle, int *exitCode)
{
    unsigned char *buffer;
    size_t         bufferSize;
    bool           retValue;

    iot_device->iothubDM.firmwareVersion = device_get_firmware_version();
    if (iot_device->iothubDM.firmwareVersion == NULL)
    {
        retValue = false;
        LogError("Failed to retrieve the firmware version for device.");
        *exitCode = -6;
    }
    else
    {
        iot_device->iothubDM.firmwareUpdate.status = (char *) ENUM_TO_STRING(FIRMWARE_UPDATE_STATUS, device_get_firmware_update_status());

        /*serialize the model using SERIALIZE_REPORTED_PROPERTIES */
        if (SERIALIZE_REPORTED_PROPERTIES(&buffer, &bufferSize, *iot_device) != CODEFIRST_OK)
        {
            retValue = false;
            LogError("Failed serializing reported state.");
            *exitCode = -7;
        }
        else
        {
            /* send the data up stream*/
            IOTHUB_CLIENT_RESULT result = IoTHubClient_SendReportedState(iotHubClientHandle, buffer, bufferSize, DeviceTwinCallback, NULL);
            if (result != IOTHUB_CLIENT_OK)
            {
                retValue = false;
                LogError("Failure sending data!!!");
                *exitCode = -15;
            }
            else
            {
                exitCode = 0;
                retValue = true;
            }
            free(buffer);
        }
        free(iot_device->iothubDM.firmwareVersion);
    }
    return retValue;
}

static int iothub_client_sample_mqtt_dm_run(const char *connectionString, bool traceOn)
{
    LogInfo("Initialize Platform");

    int exitCode;
    if (platform_init() != 0)
    {
        LogError("Failed to initialize the platform.");
        exitCode = -1;
    }
    else
    {
        if (serializer_init(NULL) != SERIALIZER_OK)
        {
            LogError("Failed in serializer_init.");
            exitCode = -2;
        }
        else
        {
            LogInfo("Instantiate the device.");
            thingie_t *iot_device = CREATE_MODEL_INSTANCE(Contoso, thingie_t);
            if (iot_device == NULL)
            {
                LogError("Failed on CREATE_MODEL_INSTANCE.");
                exitCode = -3;
            }

            else
            {
                {
                    LogInfo("Initialize From Connection String.");
                    IOTHUB_CLIENT_HANDLE iotHubClientHandle = IoTHubClient_CreateFromConnectionString(connectionString, MQTT_Protocol);
                    if (iotHubClientHandle == NULL)
                    {
                        LogError("iotHubClientHandle is NULL!");
                        exitCode = -5;
                    }
                    else
                    {
                        LogInfo("Device successfully connected.");
                        if (IoTHubClient_SetOption(iotHubClientHandle, "logtrace", &traceOn) != IOTHUB_CLIENT_OK)
                        {
                            LogError("failed to set logtrace option");
                        }

                        if (IoTHubClient_SetDeviceMethodCallback(iotHubClientHandle, DeviceMethodCallback, iot_device) != IOTHUB_CLIENT_OK)
                        {
                            LogError("failed to associate a callback for device methods");
                            exitCode = -6;
                        }
                        else
                        {
                            bool keepRunning = send_reported(iot_device, iotHubClientHandle, &exitCode);
                            while (keepRunning)
                            {
                                if (device_get_firmware_update_status() != waiting)
                                {
                                    keepRunning = send_reported(iot_device, iotHubClientHandle, &exitCode);
                                }
                                ThreadAPI_Sleep(1000);
                            }
                        }
                        IoTHubClient_Destroy(iotHubClientHandle);
                    }
                }
                DESTROY_MODEL_INSTANCE(iot_device);
            }
            serializer_deinit();
        }
        platform_deinit();
    }
    LogInfo("Exiting.");
    return exitCode;
}

int main(int argc, char *argv[])
{
    int   exitCode;
    bool  isService = true;
    bool  traceOn = false;
    char *connectionString;

    exitCode = 0;
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
                    exitCode = -12;
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
            exitCode = -14;
        }
        else
        {
            exitCode = 0;
            if (isService)
            {
                traceOn = false;
                if (device_run_service() == false)
                {
                    LogError("Failed to run as a service.");
                    exitCode = -4;
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
