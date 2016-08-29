// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "serializer.h"
#include "schemaserializer.h"

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

/* Commands implemented by the device */
WITH_ACTION(SetTemperature, int, temperature),
WITH_ACTION(SetHumidity, int, humidity),

WITH_REPORTED_PROPERTY(int, RRREPORTED)
);

END_NAMESPACE(Contoso);

EXECUTE_COMMAND_RESULT SetTemperature(Thermostat* thermostat, int temperature)
{
    (void)printf("Received temperature %d\r\n", temperature);
    thermostat->Temperature = temperature;
    return EXECUTE_COMMAND_SUCCESS;
}

EXECUTE_COMMAND_RESULT SetHumidity(Thermostat* thermostat, int humidity)
{
    (void)printf("Received humidity %d\r\n", humidity);
    thermostat->Humidity = humidity;
    return EXECUTE_COMMAND_SUCCESS;
}


void simple_sample_with_strings_run(void)
{
    if (serializer_init(NULL) != SERIALIZER_OK)
    {
        printf("Failed on serializer_init\r\n");
    }
    else
    {
        Thermostat* thermostat = CREATE_MODEL_INSTANCE(Contoso, Thermostat);
        if (thermostat == NULL)
        {
            (void)printf("Failed on CREATE_MODEL_INSTANCE\r\n");
        }
        else
        {

            unsigned char*buffer;
            size_t bufferSize;

            thermostat->DeviceId = "anpoDevice";
            thermostat->Temperature = 28;
            thermostat->Humidity = 90;
            thermostat->ExternalTemperature = 35;

            (void)printf("Sending sensor value Temperature = %d, Humidity = %d\r\n", thermostat->Temperature, thermostat->Humidity);

            if (SERIALIZE(&buffer, &bufferSize, thermostat->DeviceId, thermostat->Temperature, thermostat->Humidity, thermostat->ExternalTemperature) != CODEFIRST_OK)
            {
                (void)printf("Failed serializing sensor values\r\n");
            }
            else
            {
                printf("Serialized = %*.*s\n", (int)bufferSize, (int)bufferSize, buffer);
                free(buffer);
            }

            if (SERIALIZE_REPORTED_PROPERTIES(&buffer, &bufferSize, thermostat->RRREPORTED) != CODEFIRST_OK)
            {
                (void)printf("Failed serializing reported state\r\n");
            }
            else
            {
                printf("Serialized = %*.*s\n", (int)bufferSize, (int)bufferSize, buffer);
                free(buffer);
            }

            DESTROY_MODEL_INSTANCE(thermostat);
        }
        serializer_deinit();
    }
}

int main(void)
{
    simple_sample_with_strings_run();
    return 0;
}

