// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdio.h>

#include "serializer.h"
#include "iothub_client.h"
#include "iothubtransportmqtt.h"
#include "platform.h"
#include "azure_c_shared_utility/threadapi.h"
#include "parson.h"

/*String containing Hostname, Device Id & Device Key in the format:             */
/*  "HostName=<host_name>;DeviceId=<device_id>;SharedAccessKey=<device_key>"    */
static const char* connectionString = "<<device connection string>>";

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
    WITH_DESIRED_PROPERTY(uint8_t, desired_maxSpeed),
    WITH_DESIRED_PROPERTY(Geo, location)
);

DECLARE_MODEL(Car,
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

void onDesiredProperties(DEVICE_TWIN_UPDATE_STATE update_state, const unsigned char* payLoad, size_t size, void* userContextCallback)
{
    Car* car = userContextCallback;
    char* copyOfPayload = (char*)malloc(size + 1);
    if (copyOfPayload == NULL)
    {
        printf("unable to malloc\n");
    }
    else
    {
        memcpy(copyOfPayload, payLoad, size);
        copyOfPayload[size] = '\0';
        JSON_Value* allJSON = json_parse_string(copyOfPayload);
        if (allJSON == NULL)
        {
            printf(" failure in json_parse_string");
        }
        else
        {
            JSON_Object *allObject = json_value_get_object(allJSON);
            if (allObject == NULL)
            {
                printf("failure in json_value_get_object");
            }
            else
            {
                switch (update_state)
                {
                    case DEVICE_TWIN_UPDATE_COMPLETE:
                    {
                        JSON_Object* desired = json_object_get_object(allObject, "desired");
                        if (desired == NULL)
                        {
                            printf("failure in json_object_get_object");
                        }
                        else
                        {
                            (void)json_object_remove(desired, "$version"); //it might not exist
                            JSON_Value* desiredAfterRemove = json_object_get_value(allObject, "desired");
                            if (desiredAfterRemove != NULL)
                            {

                                char* pretty = json_serialize_to_string(desiredAfterRemove);
                                if (pretty == NULL)
                                {
                                    printf("failure in json_serialize_to_string\n");
                                }
                                else
                                {
                                    if (INGEST_DESIRED_PROPERTIES(car, pretty) != CODEFIRST_OK)
                                    {
                                        printf("failure ingesting desired properties\n");
                                    }
                                    else
                                    {
                                        printf(
                                            "car->settings.desired_maxSpeed = %d\n" \
                                            "car->settings.location.latitude = %f\n" \
                                            "car->settings.location.longitude = %f\n" \
                                            , (int)car->settings.desired_maxSpeed, car->settings.location.latitude, car->settings.location.longitude);
                                    }
                                    free(pretty);
                                }
                            }
                        }
                        break;
                    }
                    case DEVICE_TWIN_UPDATE_PARTIAL:
                    {
                        (void)json_object_remove(allObject, "$version");
                        char* pretty = json_serialize_to_string(allJSON);
                        if (pretty == NULL)
                        {
                            printf("failure in json_serialize_to_string\n");
                        }
                        else
                        {
                            if (INGEST_DESIRED_PROPERTIES(car, pretty) != CODEFIRST_OK)
                            {
                                printf("failure ingesting desired properties\n");
                            }
                            else
                            {
                                printf(
                                    "car->settings.desired_maxSpeed = %d\n" \
                                    "car->settings.location.latitude = %f\n" \
                                    "car->settings.location.longitude = %f\n" \
                                    , (int)car->settings.desired_maxSpeed, car->settings.location.latitude, car->settings.location.longitude);
                            }
                            free(pretty);
                        }
                        break;
                    }
                    default:
                    {
                        printf("INTERNAL ERROR: unexpected value for update_state=%d\n", (int)update_state);
                    }
                }
            }
            json_value_free(allJSON);
        }
        free(copyOfPayload);
    }
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
        /*prepare initializer*/
        if (serializer_init(NULL) != SERIALIZER_OK)
        {
            printf("Failed on serializer_init\n");
        }
        else
        {
            /*create a model instance*/
            Car* car = CREATE_MODEL_INSTANCE(Contoso, Car);
            if (car == NULL)
            {
                (void)printf("Failed on CREATE_MODEL_INSTANCE\n");
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

                    /*setting the desired properties callback*/
                    if (IoTHubClient_SetDeviceTwinCallback(iotHubClientHandle, onDesiredProperties, car) != IOTHUB_CLIENT_OK)
                    {
                        printf("unable to set device twin callback");
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
                    }
                    IoTHubClient_Destroy(iotHubClientHandle);
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
    device_twin_simple_sample_run();
    return 0;
}

