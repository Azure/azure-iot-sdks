// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "iothubtransportamqp.h"
#include "schemalib.h"
#include "iothub_client.h"
#include "serializer.h"
#include "schemaserializer.h"
#include "threadapi.h"

#ifdef MBED_BUILD_TIMESTAMP
#include "certs.h"
#endif // MBED_BUILD_TIMESTAMP

static const char* deviceId = "[Device Id]";
static const char* deviceKey = "[Device Key]";
static const char* hubName = "[IoTHub Name]";
static const char* hubSuffix = "[IoTHub Suffix, i.e. azure-devices.net]";

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
	WITH_DATA(double, Temperature),
	WITH_DATA(double, ExternalTemperature),
	WITH_DATA(double, Humidity),
	WITH_DATA(ascii_char_ptr, DeviceId),

	/* Device Info - This is command metadata + some extra fields */
	WITH_DATA(ascii_char_ptr, ObjectType),
	WITH_DATA(_Bool, IsSimulatedDevice),
	WITH_DATA(ascii_char_ptr, Version),
	WITH_DATA(DeviceProperties, DeviceProperties),
    WITH_DATA(ascii_char_ptr_no_quotes, Commands),

    /* Commands implemented by the device */
    WITH_ACTION(SetTemperature, double, temperature),
	WITH_ACTION(SetHumidity, double, humidity)
);

END_NAMESPACE(Contoso);

EXECUTE_COMMAND_RESULT SetTemperature(Thermostat* thermostat, double temperature)
{
    (void)printf("Received temperature %.02fs\r\n", temperature);
	thermostat->Temperature = temperature;
    return EXECUTE_COMMAND_SUCCESS;
}

EXECUTE_COMMAND_RESULT SetHumidity(Thermostat* thermostat, double humidity)
{
	(void)printf("Received humidity %.02fs\r\n", humidity);
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
            memcpy(temp, buffer, size);
            temp[size] = '\0';
            EXECUTE_COMMAND_RESULT executeCommandResult = EXECUTE_COMMAND(userContextCallback, temp);
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
    if (serializer_init(NULL) != SERIALIZER_OK)
    {
        printf("Failed on serializer_init\r\n");
    }
    else
    {
		IOTHUB_CLIENT_CONFIG config;

		config.deviceId = deviceId;
		config.deviceKey = deviceKey;
		config.iotHubName = hubName;
		config.iotHubSuffix = hubSuffix;
		config.protocol = AMQP_Protocol;

		IOTHUB_CLIENT_HANDLE iotHubClientHandle = IoTHubClient_Create(&config);
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
                            if (SERIALIZE(&buffer, &bufferSize, thermostat->ObjectType, thermostat->Version, thermostat->IsSimulatedDevice, thermostat->DeviceProperties, thermostat->Commands) != IOT_AGENT_OK)
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

					thermostat->Temperature = 50.0;
					thermostat->ExternalTemperature = 55.0;
					thermostat->Humidity = 50.0;
					thermostat->DeviceId = (char*)deviceId;

                    while (1)
                    {
                        unsigned char*buffer;
                        size_t bufferSize;

						(void)printf("Sending sensor value Temperature = %02f, Humidity = %02f\r\n", thermostat->Temperature, thermostat->Humidity);

                        if (SERIALIZE(&buffer, &bufferSize, thermostat->Temperature, thermostat->ExternalTemperature, thermostat->Humidity, thermostat->DeviceId) != IOT_AGENT_OK)
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
}
