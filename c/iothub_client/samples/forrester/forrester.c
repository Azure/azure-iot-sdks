// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef WINCE
#include "iothubtransportmqtt.h"
#else
#include "iothubtransporthttp.h"
#endif
#include "schemalib.h"
#include "iothub_client.h"
#include "serializer.h"
#include "schemaserializer.h"
#include "azure_c_shared_utility/threadapi.h"
#include "azure_c_shared_utility/platform.h"

#include "forrester.h"

#ifdef MBED_BUILD_TIMESTAMP
#include "certs.h"
#endif // MBED_BUILD_TIMESTAMP

#define DEVICE_ID "hshami_pi"
#define FIRMWARE_VERSION "4.4.11.v7+"
static const char* deviceId = DEVICE_ID;
static const char* connectionString = "HostName=dm-preview1-1.private.azure-devices-int.net;DeviceId=" DEVICE_ID ";SharedAccessKey=brmfvvTUsFqAgQ8cfhSO1kj1+ziWhvTo/XXWg7fgfj0=";

// Define the Model
BEGIN_NAMESPACE(Contoso);

DECLARE_STRUCT(SystemProperties,
ascii_char_ptr, DeviceID,
_Bool, Enabled
);

DECLARE_STRUCT(DeviceProperties,
ascii_char_ptr, DeviceID,
_Bool, HubEnabledState
);

DECLARE_MODEL(Thermostat,

/* Event data (temperature, external temperature and humidity) */
WITH_DATA(int, Temperature),
WITH_DATA(int, ExternalTemperature),
WITH_DATA(int, Humidity),
WITH_DATA(ascii_char_ptr, DeviceId),

/* Device Info - This is command metadata + some extra fields */
WITH_DATA(ascii_char_ptr, ObjectType),
WITH_DATA(_Bool, IsSimulatedDevice),
WITH_DATA(ascii_char_ptr, Version),
WITH_DATA(DeviceProperties, DeviceProperties),
WITH_DATA(ascii_char_ptr_no_quotes, Commands),
WITH_DATA(ascii_char_ptr, firmware_version),

/* Commands implemented by the device */
WITH_ACTION(SetTemperature, int, temperature),
WITH_ACTION(SetHumidity, int, humidity),
WITH_ACTION(firmwareupdate, ascii_char_ptr, URI) /*    {"Name":"firmwareupdate", "Parameters": {"URI":"GRAB IT FROM HERE"}} <=== put this in device explorer*/
);

END_NAMESPACE(Contoso);

EXECUTE_COMMAND_RESULT SetTemperature(Thermostat* thermostat, int temperature)
{
    (void)printf("Received temperature %d\r\n", temperature);
    thermostat->Temperature = temperature;
    return EXECUTE_COMMAND_SUCCESS;
}

EXECUTE_COMMAND_RESULT firmwareupdate(Thermostat* thermostat, ascii_char_ptr URI)
{
    (void)thermostat;
    (void)printf("Received firmware URI %s\r\n", URI);
    bool device_update_firmware(void);

    bool result = device_download_firmware(URI);
    if (result)
    {
        result = device_update_firmware();
        if (!result)
        {
            LogError("failed to apply new firmware image");
        }
    }
    else
    {
        LogError("failed to download new firmware image");
    }

    return EXECUTE_COMMAND_SUCCESS;
}

EXECUTE_COMMAND_RESULT SetHumidity(Thermostat* thermostat, int humidity)
{
    (void)printf("Received humidity %d\r\n", humidity);
    thermostat->Humidity = humidity;
    return EXECUTE_COMMAND_SUCCESS;
}

static void sendMessage(IOTHUB_CLIENT_HANDLE iotHubClientHandle, const unsigned char* buffer, size_t size)
{
    IOTHUB_MESSAGE_HANDLE messageHandle = IoTHubMessage_CreateFromByteArray(buffer, size);
    if (messageHandle == NULL)
    {
        printf("unable to create a new IoTHubMessage\r\n");
    }
    else
    {
        if (IoTHubClient_SendEventAsync(iotHubClientHandle, messageHandle, NULL, NULL) != IOTHUB_CLIENT_OK)
        {
            printf("failed to hand over the message to IoTHubClient");
        }
        else
        {
            printf("IoTHubClient accepted the message for delivery\r\n");
        }

        IoTHubMessage_Destroy(messageHandle);
    }
    free((void*)buffer);
}

/*this function "links" IoTHub to the serialization library*/
static IOTHUBMESSAGE_DISPOSITION_RESULT IoTHubMessage(IOTHUB_MESSAGE_HANDLE message, void* userContextCallback)
{
    IOTHUBMESSAGE_DISPOSITION_RESULT result;
    const unsigned char* buffer;
    size_t size;
    if (IoTHubMessage_GetByteArray(message, &buffer, &size) != IOTHUB_MESSAGE_OK)
    {
        printf("unable to IoTHubMessage_GetByteArray\r\n");
        result = EXECUTE_COMMAND_ERROR;
    }
    else
    {
        /*buffer is not zero terminated*/
        char* temp = malloc(size + 1);
        if (temp == NULL)
        {
            printf("failed to malloc\r\n");
            result = EXECUTE_COMMAND_ERROR;
        }
        else
        {
            EXECUTE_COMMAND_RESULT executeCommandResult;

            memcpy(temp, buffer, size);
            temp[size] = '\0';
            executeCommandResult = EXECUTE_COMMAND(userContextCallback, temp);
            result =
                (executeCommandResult == EXECUTE_COMMAND_ERROR) ? IOTHUBMESSAGE_ABANDONED :
                (executeCommandResult == EXECUTE_COMMAND_SUCCESS) ? IOTHUBMESSAGE_ACCEPTED :
                IOTHUBMESSAGE_REJECTED;
            free(temp);
        }
    }
    return result;
}

void remote_monitoring_run(void)
{
    if (platform_init() != 0)
    {
        printf("Failed to initialize the platform.\r\n");
    }
    else
    {
        if (serializer_init(NULL) != SERIALIZER_OK)
        {
            printf("Failed on serializer_init\r\n");
        }
        else
        {
            IOTHUB_CLIENT_HANDLE iotHubClientHandle;

            iotHubClientHandle = IoTHubClient_CreateFromConnectionString(connectionString, MQTT_Protocol);
            if (iotHubClientHandle == NULL)
            {
                (void)printf("Failed on IoTHubClient_CreateFromConnectionString\r\n");
            }
            else
            {
#ifdef MBED_BUILD_TIMESTAMP
                // For mbed add the certificate information
                if (IoTHubClient_SetOption(iotHubClientHandle, "TrustedCerts", certificates) != IOTHUB_CLIENT_OK)
                {
                    printf("failure to set option \"TrustedCerts\"\r\n");
                }
#endif // MBED_BUILD_TIMESTAMP

                Thermostat* thermostat = CREATE_MODEL_INSTANCE(Contoso, Thermostat);
                if (thermostat == NULL)
                {
                    (void)printf("Failed on CREATE_MODEL_INSTANCE\r\n");
                }
                else
                {
                    STRING_HANDLE commandsMetadata;

                    if (IoTHubClient_SetMessageCallback(iotHubClientHandle, IoTHubMessage, thermostat) != IOTHUB_CLIENT_OK)
                    {
                        printf("unable to IoTHubClient_SetMessageCallback\r\n");
                    }
                    else
                    {

                        /* send the device info upon startup so that the cloud app knows
                        what commands are available and the fact that the device is up */
                        thermostat->ObjectType = "DeviceInfo";
                        thermostat->IsSimulatedDevice = false;
                        thermostat->Version = "1.0";
                        thermostat->DeviceProperties.HubEnabledState = true;
                        thermostat->DeviceProperties.DeviceID = (char*)deviceId;

                        commandsMetadata = STRING_new();
                        if (commandsMetadata == NULL)
                        {
                            (void)printf("Failed on creating string for commands metadata\r\n");
                        }
                        else
                        {
                            /* Serialize the commands metadata as a JSON string before sending */
                            if (SchemaSerializer_SerializeCommandMetadata(GET_MODEL_HANDLE(Contoso, Thermostat), commandsMetadata) != SCHEMA_SERIALIZER_OK)
                            {
                                (void)printf("Failed serializing commands metadata\r\n");
                            }
                            else
                            {
                                unsigned char* buffer;
                                size_t bufferSize;
                                thermostat->Commands = (char*)STRING_c_str(commandsMetadata);

                                

                                /* Here is the actual send of the Device Info */
                                if (SERIALIZE(&buffer, &bufferSize, thermostat->ObjectType, thermostat->Version, thermostat->IsSimulatedDevice, thermostat->DeviceProperties, thermostat->Commands) != CODEFIRST_OK)
                                {
                                    (void)printf("Failed serializing\r\n");
                                }
                                else
                                {
                                    sendMessage(iotHubClientHandle, buffer, bufferSize);
                                }

                            }

                            STRING_delete(commandsMetadata);
                        }

                        thermostat->Temperature = 50;
                        thermostat->ExternalTemperature = 55;
                        thermostat->Humidity = 50;
                        thermostat->DeviceId = (char*)deviceId;
                        thermostat->firmware_version = FIRMWARE_VERSION;

                        while (1)
                        {
                            unsigned char*buffer;
                            size_t bufferSize;

                            (void)printf("Sending sensor value Temperature = %d, Humidity = %d Firmware Version = %s\r\n", thermostat->Temperature, thermostat->Humidity, thermostat->firmware_version);

                            if (SERIALIZE(&buffer, &bufferSize, thermostat->DeviceId, thermostat->Temperature, thermostat->Humidity, thermostat->ExternalTemperature, thermostat->firmware_version) != CODEFIRST_OK)
                            {
                                (void)printf("Failed sending sensor value\r\n");
                            }
                            else
                            {
                                sendMessage(iotHubClientHandle, buffer, bufferSize);
                            }

                            ThreadAPI_Sleep(1000);
                        }
                    }

                    DESTROY_MODEL_INSTANCE(thermostat);
                }
                IoTHubClient_Destroy(iotHubClientHandle);
            }
            serializer_deinit();
        }
        platform_deinit();
    }
}

int main(void)
{
    if(!device_run_service())
    {
        LogError("failed to run in service mode");
    }

    remote_monitoring_run();
    return 0;
}

