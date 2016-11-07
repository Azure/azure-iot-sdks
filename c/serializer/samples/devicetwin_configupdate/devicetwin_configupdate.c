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

DECLARE_MODEL(Thermostat,
    WITH_DATA(uint8_t, temperature),
    WITH_DATA(uint8_t, humidity)
);

DECLARE_MODEL(TelemetryConfig,
    WITH_REPORTED_PROPERTY(int32_t, sendFrequency)
);

DECLARE_DEVICETWIN_MODEL(Configuration,
    WITH_REPORTED_PROPERTY(TelemetryConfig, telemetryConfig)
);

END_NAMESPACE(Contoso);


typedef struct DEVICE_TYPE_TAG
{
    Thermostat    *data;
    Configuration *config;

    LOCK_HANDLE    lock;
} DEVICE_TYPE;

static bool sendMessage(IOTHUB_CLIENT_HANDLE iotHubClientHandle, Thermostat *device)
{
    unsigned char *buffer;
    size_t         bufferSize;

    bool retValue;
    if (SERIALIZE(&buffer, &bufferSize, device->humidity, device->temperature) != CODEFIRST_OK)
    {
        (void) printf("Failed serializing\n");
        retValue = false;
    }
    else
    {
        IOTHUB_MESSAGE_HANDLE messageHandle = IoTHubMessage_CreateFromByteArray(buffer, bufferSize);
        free((void*)buffer);
        if (messageHandle == NULL)
        {
            (void) printf("unable to create a new IoTHubMessage\n");
            retValue = false;
        }
        else
        {
            if (IoTHubClient_SendEventAsync(iotHubClientHandle, messageHandle, NULL, NULL) != IOTHUB_CLIENT_OK)
            {
                (void) printf("failed to hand over the message to IoTHubClient\n");
                retValue = false;
            }
            else
            {
                (void) printf("IoTHubClient accepted the message for delivery\n");
                retValue = true;
            }
            IoTHubMessage_Destroy(messageHandle);
        }
    }
    return retValue;
}

static void reportedCallback(int status_code, void* userContextCallback)
{
    (void) (userContextCallback);
    (void) printf("IoTHub: reported properties delivered with status_code = %u\n", status_code);
}

static DEVICE_TYPE * makeDevice(Thermostat *data, Configuration *config)
{
    DEVICE_TYPE *retValue = malloc(sizeof(DEVICE_TYPE));

    if (retValue == NULL)
    {
        (void) printf("faile in malloc\n");
    }
    else
    {
        if ((retValue->lock = Lock_Init()) == NULL)
        {
            (void) printf("faile to initialize lock\n");
            free((void *) retValue);
            retValue = NULL;
        }
        else
        {
            retValue->config = config;
            retValue->data = data;
        }
    }
    return retValue;
}

static void destroyDevice(DEVICE_TYPE *device)
{
    if (device != NULL)
    {
        Unlock(device->lock);
        Lock_Deinit(device->lock);

        free((void *) device);
    }
}

static char * malloc_and_copy_unsigned_char(const unsigned char* payload, size_t size)
{
    char *retValue;
    if (payload == NULL)
    {
        (void) printf("invalid parameter payload\n");
        retValue = NULL;
    }
    else if (size < 1)
    {
        (void) printf("invalid parameter size\n");
        retValue = NULL;
    }
    else
    {
        char *temp = (char *)malloc(size + 1);
        if (temp == NULL)
        {
            (void) printf("malloc failed\n");
            retValue = NULL;
        }
        else
        {
            retValue = (char *) memcpy(temp, payload, size);
            temp[size] = '\0';
        }
    }
    return retValue;
}

static void deviceTwinCallback(DEVICE_TWIN_UPDATE_STATE update_state, const unsigned char* payload, size_t size, void* userContextCallback)
{
    DEVICE_TYPE *device = (DEVICE_TYPE *) userContextCallback;
    if (Lock(device->lock) == LOCK_ERROR)
    {
        (void) printf("Lock failed\n");
    }
    else
    {
        char *cb_payload = malloc_and_copy_unsigned_char(payload, size);
        JSON_Value  *root_value = json_parse_string(cb_payload);
        JSON_Object *root_object = json_value_get_object(root_value);
        int32_t temp;
        switch (update_state)
        {
            case DEVICE_TWIN_UPDATE_COMPLETE:
                temp = (int) json_object_dotget_number(root_object, "desired.telemetryConfig.sendFrequency");
                break;
            case DEVICE_TWIN_UPDATE_PARTIAL:
                temp = (int) json_object_dotget_number(root_object, "telemetryConfig.sendFrequency");
                break;
            default: // invalid update state
                temp = 0;
                (void) printf("Invalid update_state reported\n");
                break;
        }
        json_value_free(root_value);
        free((void *) cb_payload);
        (void) Unlock(device->lock);

        if (temp != 0)
        {
            device->config->telemetryConfig.sendFrequency = temp;
        }
    }
}

static void sendReported(IOTHUB_CLIENT_HANDLE iotHubClientHandle, Configuration *config)
{
    unsigned char *buffer;
    size_t         bufferSize;
    if (SERIALIZE_REPORTED_PROPERTIES(&buffer, &bufferSize, *config) != CODEFIRST_OK)
    {
        (void)printf("Failed serializing reported state\n");
    }
    else
    {
        /*sending the serialized reported properties to IoTHub*/
        if (IoTHubClient_SendReportedState(iotHubClientHandle, buffer, bufferSize, reportedCallback, NULL) != IOTHUB_CLIENT_OK)
        {
            (void)printf("Failure sending data\n");
        }
        else
        {
            (void)printf("reported state has been delivered to IoTHub\n");
        }
        free((void *)buffer);
    }
}

#if defined(WIN32)
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include <windows.h>
#include <reason.h>

static int reboot_async(void *arg)
{
    (void) arg;
    ThreadAPI_Sleep(2000);
    (void) ExitWindowsEx(EWX_REBOOT | EWX_FORCEIFHUNG, SHTDN_REASON_MINOR_INSTALLATION);
    return 0;
}

#else

#include <sys/reboot.h>

static int reboot_async(void *arg)
{
    (void) arg;
    ThreadAPI_Sleep(2000);
    setuid(0);
    sync();
    reboot(RB_AUTOBOOT);
    return 0;
}
#endif


#define SERVER_ERROR 500
#define NOT_IMPLEMENTED 501
#define NOT_VALID 400
#define SERVER_SUCCESS 200

static int deviceMethodCallback(const char* method_name, const unsigned char* payload, size_t size, unsigned char** response, size_t* resp_size, void* userContextCallback)
{
    (void)userContextCallback;
    (void)payload;
    (void)size;

    int retValue;
    if (method_name == NULL)
    {
        (void) printf("method name is NULL\n");
        retValue = NOT_VALID;
    }
    else if ((response == NULL) || (resp_size == NULL))
    {
        (void) printf("response parameters are NULL\n");
        retValue = NOT_VALID;
    }
    else if (strcmp(method_name, "reboot") == 0)
    {
        *response = NULL;
        *resp_size = 0;

        (void) printf("reboot device\n");

        THREAD_HANDLE th;
        if (ThreadAPI_Create(&th, reboot_async, NULL) == THREADAPI_ERROR)
        {
            (void) printf("ThreadAPI_Create failed\n");
            retValue = SERVER_ERROR;
        }
        else
        {
            retValue = SERVER_SUCCESS;
        }
    }
    else
    {
        (void) printf("Invalid method call\n");
        retValue = NOT_VALID;
    }
    return retValue;
}

static void device_twin_config_update_run(void)
{
    /*prepare the platform*/
    if (platform_init() != 0)
    {
        (void) printf("Failed to initialize the platform\n");
    }
    else
    {
        if (SERIALIZER_REGISTER_NAMESPACE(Contoso) == NULL)
        {
            (void) printf("unable to SERIALIZER_REGISTER_NAMESPACE\n");
        }
        else
        {
            /*create an IoTHub client*/
            IOTHUB_CLIENT_HANDLE iotHubClientHandle = IoTHubClient_CreateFromConnectionString(connectionString, MQTT_Protocol);
            if (iotHubClientHandle == NULL)
            {
                (void) printf("Failure creating IoTHubClient handle\n");
            }
            else
            {
                // Turn on Log 
                bool trace = true;
                (void) IoTHubClient_SetOption(iotHubClientHandle, "logtrace", &trace);

                Thermostat    *data = CREATE_MODEL_INSTANCE(Contoso, Thermostat);
                if (data == NULL)
                {
                    (void) printf("Failure in CREATE_MODEL_INSTANCE\n");
                }
                else
                {
                    Configuration *config = IoTHubDeviceTwin_CreateConfiguration(iotHubClientHandle);
                    if (config == NULL)
                    {
                        (void) printf("Failure in IoTHubDeviceTwin_CreateConfiguration\n");
                    }
                    else
                    {
                        DEVICE_TYPE *device = makeDevice(data, config);
                        if (device == NULL)
                        {
                            (void) printf("failed in making the device intance\n");
                        }
                        else
                        {
                            config->telemetryConfig.sendFrequency = 3000;
                            if (IoTHubClient_SetDeviceTwinCallback(iotHubClientHandle, deviceTwinCallback, device) != IOTHUB_CLIENT_OK)
                            {
                                (void) printf("IoTHubClient_SetDeviceTwinCallback failed\n");
                            }
                            else
                            {
                                if (IoTHubClient_SetDeviceMethodCallback(iotHubClientHandle, deviceMethodCallback, device) != IOTHUB_CLIENT_OK)
                                {
                                    (void) printf("IoTHubClient_SetDeviceTwinCallback failed\n");
                                }

                                srand((unsigned int) time(NULL));
                                while (true)
                                {
                                    sendReported(iotHubClientHandle, config);

                                    /*setting values for reported properties*/
                                    data->humidity = (uint8_t) (rand() % 100);
                                    data->temperature = (uint8_t) (rand() % 100);

                                    if (!sendMessage(iotHubClientHandle, data))
                                    {
                                        break;
                                    }

                                    if (Lock(device->lock) == LOCK_ERROR)
                                    {
                                        (void) printf("failed to lock device data\n");
                                        break;
                                    }
                                    
                                    else
                                    {
                                        int32_t to = device->config->telemetryConfig.sendFrequency;
                                        (void) Unlock(device->lock);
                                        ThreadAPI_Sleep(to);
                                    }
                                }
                            }
                        }
                        destroyDevice(device);
                    }
                    IoTHubDeviceTwin_DestroyConfiguration(config);
                    DESTROY_MODEL_INSTANCE(data);
                }
                IoTHubClient_Destroy(iotHubClientHandle);
            }
        }
        platform_deinit();
    }
}

int main(void)
{
    device_twin_config_update_run();
    return 0;
}

