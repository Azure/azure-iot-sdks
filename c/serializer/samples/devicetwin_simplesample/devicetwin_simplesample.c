// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdio.h>
#include <inttypes.h>

#include "serializer_devicetwin.h"
#include "iothub_client.h"
#include "iothubtransportmqtt.h"
#include "platform.h"
#include "azure_c_shared_utility/threadapi.h"
#include "parson.h"

/*String containing Hostname, Device Id & Device Key in the format:             */
/*  "HostName=<host_name>;DeviceId=<device_id>;SharedAccessKey=<device_key>"    */
static const char* connectionString = "HostName=....";

// Define the Model - it is a car.
BEGIN_NAMESPACE(Contoso);

DECLARE_STRUCT(Maker,
    ascii_char_ptr, makerName, /*Fabrikam, Contoso ... */
    ascii_char_ptr, style, /*sedan, minivan ...*/
    int, year
);

DECLARE_STRUCT(Geo,
    double, longitude,
    double, latitude
);

DECLARE_MODEL(CarState,
    WITH_REPORTED_PROPERTY(int32_t, softwareVersion),
    WITH_REPORTED_PROPERTY(uint8_t, reported_maxSpeed),
    WITH_REPORTED_PROPERTY(ascii_char_ptr, vanityPlate)
);

DECLARE_MODEL(CarSettings,
    WITH_DESIRED_PROPERTY(uint8_t, desired_maxSpeed, onDesiredMaxSpeed),
    WITH_DESIRED_PROPERTY(Geo, location)
);

DECLARE_DEVICETWIN_MODEL(Car,
    WITH_REPORTED_PROPERTY(ascii_char_ptr, lastOilChangeDate), /*this is a simple reported property*/
    WITH_DESIRED_PROPERTY(ascii_char_ptr, changeOilReminder),
    
    WITH_REPORTED_PROPERTY(Maker, maker), /*this is a structured reported property*/
    WITH_REPORTED_PROPERTY(CarState, state), /*this is a model in model*/
    WITH_DESIRED_PROPERTY(CarSettings, settings) /*this is a model in model*/
);

END_NAMESPACE(Contoso);

DEFINE_ENUM_STRINGS(DEVICE_TWIN_UPDATE_STATE, DEVICE_TWIN_UPDATE_STATE_VALUES);

void deviceTwinCallback(int status_code, void* userContextCallback)
{
    (void)(userContextCallback);
    printf("IoTHub: reported properties delivered with status_code = %u\n", status_code);
}


void onDesiredMaxSpeed(void* argument)
{
    /*by convention 'argument' is of the type of the MODEL encompassing the desired property*/
    /*in this case, it is 'CarSettings'*/

    CarSettings* car = argument;
    printf("received a new desired_maxSpeed = %" PRIu8 "\n", car->desired_maxSpeed);

}

void device_twin_simple_sample_run(void)
{
    /*prepare the platform*/
    if (platform_init() != 0)
    {
        printf("Failed to initialize the platform.\n");
    }
    else
    {
        if (SERIALIZER_REGISTER_NAMESPACE(Contoso) == NULL)
        {
            LogError("unable to SERIALIZER_REGISTER_NAMESPACE");
        }
        else
        {
            /*create an IoTHub client*/
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

                Car* car = IoTHubDeviceTwin_CreateCar(iotHubClientHandle);
                if (car == NULL)
                {
                    printf("Failure in IoTHubDeviceTwin_CreateCar");
                }
                else
                {
                    unsigned char*buffer;
                    size_t bufferSize;

                    /*setting values for reported properties*/
                    car->lastOilChangeDate = "2016";
                    car->maker.makerName = "Fabrikam";
                    car->maker.style = "sedan";
                    car->maker.year = 2014;
                    car->state.reported_maxSpeed = 100;
                    car->state.softwareVersion = 1;
                    car->state.vanityPlate = "1I1";

                    /*getting the serialized form*/
                    if (SERIALIZE_REPORTED_PROPERTIES(&buffer, &bufferSize, *car) != CODEFIRST_OK)
                    {
                        (void)printf("Failed serializing reported state\n");
                    }
                    else
                    {
                        /*sending the serialized reported properties to IoTHub*/
                        if (IoTHubClient_SendReportedState(iotHubClientHandle, buffer, bufferSize, deviceTwinCallback, NULL) != IOTHUB_CLIENT_OK)
                        {
                            printf("Failure sending data\n");
                        }
                        else
                        {
                            printf("reported state has been delivered to IoTHub\n");
                        }
                        free(buffer);

                        printf("press ENTER to end the sample\n");
                        (void)getchar();
                    }
                    IoTHubDeviceTwin_DestroyCar(car);
                }
                IoTHubClient_Destroy(iotHubClientHandle);
            }
        }
        platform_deinit();
    }
}

int main(void)
{
    device_twin_simple_sample_run();
    return 0;
}

