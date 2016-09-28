// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


#include "serializer.h"
#include "iothub_client.h"

#include "azure_c_shared_utility/threadapi.h"
#include "azure_c_shared_utility/platform.h"

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


#define CONNECTION_STRING_FILE_NAME ".device_connection_string"

static void deviceTwinCallback(int status_code, void* userContextCallback)
{
    (void)(userContextCallback);
    LogInfo("DT CallBack: Status_code = %u", status_code);
}

static bool initGlobalProperties(int argc, char *argv[], char **connectionString, bool *isService, bool *traceOn)
{
    bool retValue;
    for (int ii = 1; ii < argc; ++ii)
    {
        if (0 == strcmp(argv[ii], "-console"))
        {
            *isService = false;
        }
        else if (0 == strcmp(argv[ii], "-cs"))
        {
            ++ii;
            if (ii < argc)
            {
                size_t length = strlen(argv[ii]) + 1;
                *connectionString = malloc(length);
                if (*connectionString == NULL)
                {
                    LogError("failed to allocate memory for connection string");
                    retValue = false;
                }
                else
                {
                    memcpy(*connectionString, argv[ii], length);
                }
            }
        }
        else if (0 == strcmp(argv[ii], "-logging"))
        {
            *traceOn = true;
        }
    }
    if (*isService)
    {
        if (*connectionString != NULL)
        {
            LogError("running as a service may not specify the -cs argument");
            free(*connectionString);
            *connectionString = NULL;
            retValue = false;
        }
        else
        {
            *connectionString = device_read_string_from_file(CONNECTION_STRING_FILE_NAME);
            if (*connectionString == NULL)
            {
                LogError("failed to fetch the connection string for the service");
                retValue = false;
            }
            else
            {
                retValue = true;
            }
        }
    }
    else
    {
        if (*connectionString == NULL)
        {
            LogError("missing parameter connection string");
            retValue = false;
        }
        else
        {
            retValue = true;
        }
    }
    return retValue;
}

static int iothub_client_sample_mqtt_dm_run(const char *connectionString, bool asService, bool traceOn)
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
                if (asService && (device_run_service() == false))
                {
                    LogError("Failed to run as a service.");
                    exitCode = -4;
                }
                else
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
                        if (!asService)
                        {
                            if (IoTHubClient_SetOption(iotHubClientHandle, "logtrace", &traceOn) != IOTHUB_CLIENT_OK)
                            {
                                LogError("failed to set logtrace option");
                            }
                        }
                    
                        unsigned char *buffer;
                        size_t         bufferSize;

                        bool keepRunning = true;
                        while (keepRunning)
                        {
                            iot_device->iothubDM.firmwareVersion = device_get_firmware_version();
                            if (iot_device->iothubDM.firmwareVersion == NULL)
                            {
                                keepRunning = false;
                                LogError("Failed to retrieve the firmware version for device.");
                                exitCode = -6;
                            }
                            else
                            {
                                iot_device->iothubDM.firmwareUpdate.status = (char *) ENUM_TO_STRING(FIRMWARE_UPDATE_STATUS, device_get_firmware_update_status());

                                /*serialize the model using SERIALIZE_REPORTED_PROPERTIES */
                                if (SERIALIZE_REPORTED_PROPERTIES(&buffer, &bufferSize, *iot_device) != CODEFIRST_OK)
                                {
                                    keepRunning = false;
                                    LogError("Failed serializing reported state.");
                                    exitCode = -7;
                                }
                                else
                                {
                                    LogInfo("Serialized = %*.*s", (int)bufferSize, (int)bufferSize, buffer);

                                    /* send the data up stream*/
                                    IOTHUB_CLIENT_RESULT result = IoTHubClient_SendReportedState(iotHubClientHandle, buffer, bufferSize, deviceTwinCallback, NULL);
                                    if (result != IOTHUB_CLIENT_OK)
                                    {
                                        keepRunning = false;
                                        LogError("Failure sending data!!!");
                                        exitCode = -15;
                                    }

                                    else
                                    {
                                        ThreadAPI_Sleep(6000 /* six seconds */);
                                    }
                                    free(buffer);
                                }
                                free(iot_device->iothubDM.firmwareVersion);
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
    bool  isService = false;
    bool  traceOn = false;
    char *connectionString;
    if (initGlobalProperties(argc, argv, &connectionString, &isService, &traceOn))
    {
        exitCode = iothub_client_sample_mqtt_dm_run(connectionString, isService, traceOn);
        free(connectionString);
    }
    else
    {
        exitCode = -14;
    }

    LogInfo("Exit Code: %d", exitCode);
    return exitCode;
}
