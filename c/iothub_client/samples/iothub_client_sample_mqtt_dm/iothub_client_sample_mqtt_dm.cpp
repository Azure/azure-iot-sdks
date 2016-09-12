// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "iothub_client_sample_mqtt_dm.h"
DEFINE_ENUM_STRINGS(FIRMWARE_UPDATE_STATE, FIRMWARE_UPDATE_STATE_VALUES)
/* ENUM_TO_STRING(FIRMWARE_UPDATE_STATE, fw_update_state) */


/*String containing Hostname, Device Id & Device Key in the format:                         */
/*  "HostName=<host_name>;DeviceId=<device_id>;SharedAccessKey=<device_key>"                */
/*  "HostName=<host_name>;DeviceId=<device_id>;SharedAccessSignature=<device_sas_token>"    */
static char *connectionString = NULL;

/*Flag marking the run mode. if true, the client will terminate and stay resident.*/
static bool asService = true;

/*trace option is given at the command line.*/
static bool traceOn = false;


void deviceTwinCallback(int status_code, void* userContextCallback)
{
    (void)(userContextCallback);
    LogInfo("DT CallBack: Status_code = %u", status_code);
}

void initGlobalProperties(int argc, char *argv[])
{
    for (int ii = 1; ii < argc; ++ii)
    {
        if (0 == strcmp(argv[ii], "-console"))
        {
            asService = false;
        }
        else if (0 == strcmp(argv[ii], "-cs"))
        {
			connectionString = strdup(argv[++ii]);
			LogInfo("   -cs = '%s'", connectionString);
        }
        else if (0 == strcmp(argv[ii], "-logging"))
        {
            traceOn = true;
        }
    }
}

void iothub_client_sample_mqtt_dm_run(void)
{
    LogInfo("Initialize Platform\n");

    if (platform_init() != 0)
    {
        LogError("Failed to initialize the platform.");
    }
    else
    {
        if (serializer_init(NULL) != SERIALIZER_OK)
        {
            LogError("Failed in serializer_init.");
        }
        else
        {
            LogInfo("Initialize From Connection String.");

            if (asService)
            {
                device_run_service();
            }

            IOTHUB_CLIENT_HANDLE iotHubClientHandle = IoTHubClient_CreateFromConnectionString(connectionString, MQTT_Protocol);
            if (iotHubClientHandle == NULL)
            {
                LogError("iotHubClientHandle is NULL!");
            }
            else
            {
                IoTHubClient_SetOption(iotHubClientHandle, "logtrace", &traceOn);
                if (serializer_init(NULL) != SERIALIZER_OK)
                {
                    LogError("Failed on serializer_init.");
                }
                else
                {
                    bool keepRunning = true;
                    unsigned char *buffer;
                    size_t         bufferSize;

                    while (keepRunning)
                    {
                        /*TODO*/
                        /*Implement DM calls and serialize the model using SERIALIZE_REPORTED_PROPERTIES*/

                        /* send the data up stream*/
                        if (IoTHubClient_SendReportedState(iotHubClientHandle, buffer, bufferSize, deviceTwinCallback, NULL) != IOTHUB_CLIENT_OK)
                        {
                            keepRunning = false;
                            LogError("Failure sending data!!!");
                        }

						else
						{
							ThreadAPI_Sleep(1000);
						}
                    }
                }
            }
        }
        platform_deinit();
    }
    LogInfo("Exiting.");
}

int main(int argc, char *argv[])
{
    initGlobalProperties(argc, argv);
    iothub_client_sample_mqtt_dm_run();

    return 0;
}
