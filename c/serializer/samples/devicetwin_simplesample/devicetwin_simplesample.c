// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdio.h>

#include "serializer.h"
#include "iothub_client.h"
#include "iothubtransportmqtt.h"
#include "platform.h"
#include "azure_c_shared_utility/threadapi.h"

/*String containing Hostname, Device Id & Device Key in the format:             */
/*  "HostName=<host_name>;DeviceId=<device_id>;SharedAccessKey=<device_key>"    */
static const char* connectionString = "[device connection string]";

// Define the Model - it is a car.
BEGIN_NAMESPACE(Contoso);

DECLARE_STRUCT(Maker,
ascii_char_ptr, makerName, /*Fabrikam, Contoso ... */
ascii_char_ptr, style, /*sedan, minivan ...*/
int, year
);

DECLARE_MODEL(CarData,
    WITH_REPORTED_PROPERTY(int32_t, softwareVersion),
    WITH_REPORTED_PROPERTY(uint8_t, maxSpeed),
    WITH_REPORTED_PROPERTY(ascii_char_ptr, vanityPlate)
);

DECLARE_MODEL(Car,
    WITH_REPORTED_PROPERTY(int32_t, price), /*this is a simple reported property*/
    WITH_REPORTED_PROPERTY(Maker, maker), /*this is a structured reported property*/
    WITH_REPORTED_PROPERTY(CarData, options) /*this is a model in model reported property*/
);

END_NAMESPACE(Contoso);

static int wascallbackcalled = 0;

void deviceTwinCallback(int status_code, void* userContextCallback)
{
    (void)(userContextCallback);
    printf("status_code = %u\n", status_code);
    wascallbackcalled = 1;
}

void simple_sample_with_strings_run(void)
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
            Car* car = CREATE_MODEL_INSTANCE(Contoso, Car);
            if (car == NULL)
            {
                (void)printf("Failed on CREATE_MODEL_INSTANCE\r\n");
            }
            else
            {

                unsigned char*buffer;
                size_t bufferSize;

                car->price = 7999;
                car->options.softwareVersion = 21;
                car->options.vanityPlate = "";
                car->options.maxSpeed = 100;
                car->maker.makerName = "Fabrikam";

                car->maker.style = "sedan";
                car->maker.year = 2016;

                if (SERIALIZE_REPORTED_PROPERTIES(&buffer, &bufferSize, *car) != CODEFIRST_OK)
                {
                    (void)printf("Failed serializing reported state\r\n");
                }
                else
                {
                    printf("Serialized = %*.*s\n", (int)bufferSize, (int)bufferSize, buffer);

                    IOTHUB_CLIENT_HANDLE iotHubClientHandle = IoTHubClient_CreateFromConnectionString(connectionString, MQTT_Protocol);
                    if (iotHubClientHandle == NULL)
                    {
                        printf("Failure creating IoTHubClient handle");
                    }
                    else
                    {
                        // Turn on Log 
                        bool trace = true;
                        (void)IoTHubClient_SetOption(iotHubClientHandle, "logtrace", &trace);

                        if (IoTHubClient_SendReportedState(iotHubClientHandle, buffer, bufferSize, deviceTwinCallback, NULL) != IOTHUB_CLIENT_OK)
                        {
                            printf("failure sending data!!!\n");
                        }
                        else
                        {
                            printf("waiting for callback to be called\n");
                            /*do nothing until callback is called*/
                            while (wascallbackcalled == 0)
                            {
                                ThreadAPI_Sleep(10);
                            }
                        }
                    }

                    free(buffer);
                }

                DESTROY_MODEL_INSTANCE(car);
            }
            serializer_deinit();
        }
        platform_deinit();
    }
}

int main(void)
{
    simple_sample_with_strings_run();
    return 0;
}

