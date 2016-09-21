// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "serializer.h"
#include "iothub_client.h"

#include "azure_c_shared_utility/threadapi.h"
#include "azure_c_shared_utility/platform.h"

#include "iothubtransportmqtt.h"

#include "iothub_client_sample_mqtt_dm.h"
DEFINE_ENUM_STRINGS(FIRMWARE_UPDATE_STATE, FIRMWARE_UPDATE_STATE_VALUES)

BEGIN_NAMESPACE(Contoso);

DECLARE_STRUCT(dm_error_t,
    ascii_char_ptr, Code,
    ascii_char_ptr, Message
);

DECLARE_STRUCT(reboot_t,
    ascii_char_ptr, description,
    ascii_char_ptr, version
);

DECLARE_STRUCT(factoryreset_t,
    ascii_char_ptr, description,
    ascii_char_ptr, version
);

DECLARE_STRUCT(device_t,
    ascii_char_ptr, time,
    ascii_char_ptr, time_zone,
    ascii_char_ptr, fw_version,
    ascii_char_ptr, name
);

DECLARE_STRUCT(firmwareUpdate_t,
    ascii_char_ptr, fwPackageUri,
    int,            downloadCompletedTime,
    ascii_char_ptr, status,
    int,            lastFirmwareUpdate,
    ascii_char_ptr, version
);

DECLARE_MODEL(iothubDM_t,
    WITH_REPORTED_PROPERTY(reboot_t, reboot),
    WITH_REPORTED_PROPERTY(factoryreset_t, factoryreset),
    WITH_REPORTED_PROPERTY(firmwareUpdate_t, firmwareUpdate),
    WITH_REPORTED_PROPERTY(device_t, device)
);

DECLARE_MODEL(thingie_t,
    WITH_REPORTED_PROPERTY(iothubDM_t, iothubDM)
);

DECLARE_MODEL(root_t,
    WITH_REPORTED_PROPERTY(thingie_t, desired),
    WITH_REPORTED_PROPERTY(thingie_t, reported)
);

END_NAMESPACE(Contoso);


/*String containing Hostname, Device Id & Device Key in the format:                         */
/*  "HostName=<host_name>;DeviceId=<device_id>;SharedAccessKey=<device_key>"                */
/*  "HostName=<host_name>;DeviceId=<device_id>;SharedAccessSignature=<device_sas_token>"    */
static char *connectionString = NULL;

/*Flag marking the run mode. if true, the client will terminate and stay resident.*/
static bool asService = true;

/*trace option is given at the command line.*/
static bool traceOn = false;

/*keep track of run state.*/
static int exitCode = 0;

/*reported*/
static bool reported = false;


static void deviceTwinCallback(int status_code, void* userContextCallback)
{
    (void)(userContextCallback);
    LogInfo("DT CallBack: Status_code = %u", status_code);
	reported = true;
}

static void initGlobalProperties(int argc, char *argv[])
{
    for (int ii = 1; ii < argc; ++ii)
    {
        if (0 == strcmp(argv[ii], "-console"))
        {
            asService = false;
        }
        else if (0 == strcmp(argv[ii], "-cs"))
        {
            ++ii;
            if (ii < argc)
            {
                connectionString = _strdup(argv[ii]);
            }
            LogInfo("connectionString = '%s'", connectionString);
        }
        else if (0 == strcmp(argv[ii], "-logging"))
        {
            traceOn = true;
        }
    }
}

static void iothub_client_sample_mqtt_dm_run(void)
{
    LogInfo("Initialize Platform");

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
            root_t *root = CREATE_MODEL_INSTANCE(Contoso, root_t);
            if (root == NULL)
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
                    free(connectionString);
                    connectionString = NULL;
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
                            IoTHubClient_SetOption(iotHubClientHandle, "logtrace", &traceOn);
                        }
                    
                        unsigned char *buffer = NULL;
                        size_t         bufferSize = 0;

                        bool keepRunning = true;
                        /*TODO: Implement DM calls*/
                        root->reported.iothubDM.device.time = (char *) "Time Is Money";
                        root->reported.iothubDM.device.time_zone = (char *) "anywhere";
                        root->reported.iothubDM.device.fw_version = (char *) "OneTwo12";
                        root->reported.iothubDM.device.name = (char *) "HSEdison";
                        
                        while (keepRunning)
                        {
                            /*serialize the model using SERIALIZE_REPORTED_PROPERTIES */
                            if (SERIALIZE_REPORTED_PROPERTIES(&buffer, &bufferSize, root->reported.iothubDM.device) != CODEFIRST_OK)
                            {
                                keepRunning = false;
                                LogError("Failed serializing reported state.");
                                exitCode = -6;
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
                                    LogInfo("SendReportedState returned '%d'", result);
									while (!reported)
									{
										ThreadAPI_Sleep(6000);
									}
                                }
                                free(buffer);
                                buffer = NULL;
                            }
                        }
                        DESTROY_MODEL_INSTANCE(root);
                        IoTHubClient_Destroy(iotHubClientHandle);
                    }
                }
            }
            serializer_deinit();
        }
        platform_deinit();
    }
    LogInfo("Exiting.");
}

int main(int argc, char *argv[])
{
    initGlobalProperties(argc, argv);
    iothub_client_sample_mqtt_dm_run();

    LogInfo("Exit Code: %d", exitCode);
    return exitCode;
}
