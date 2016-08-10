// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include "azure_c_shared_utility/threadapi.h"
#include "azure_c_shared_utility/platform.h"
#include "iothubtransporthttp.h"
#include "schemalib.h"
#include "serializer.h"
#include "schemaserializer.h"
#include "iothub_client_options.h"
#include "iothub_client_ll.h"

static const char* connectionString = "[device connection string]";
static const char* deviceId = "[deviceName]"; /*must match the one on connectionString*/

// Define the Model
BEGIN_NAMESPACE(Contoso);

DECLARE_STRUCT(SystemProperties,
ascii_char_ptr, DeviceID,
bool, Enabled
);

DECLARE_MODEL(FrdmDevice,

/* Device Info - This is command metadata + some extra fields */
WITH_DATA(ascii_char_ptr, ObjectName),
WITH_DATA(ascii_char_ptr, ObjectType),
WITH_DATA(ascii_char_ptr, Version),
WITH_DATA(ascii_char_ptr, TargetAlarmDevice),
WITH_DATA(EDM_DATE_TIME_OFFSET, Time),
WITH_DATA(float, temp),
WITH_DATA(SystemProperties, SystemProperties),
WITH_DATA(ascii_char_ptr_no_quotes, Commands),

/* Commands implemented by the device */
WITH_ACTION(AlarmAnomaly, ascii_char_ptr, SensorId),
WITH_ACTION(AlarmThreshold, ascii_char_ptr, SensorId)
);

END_NAMESPACE(Contoso);

EXECUTE_COMMAND_RESULT AlarmAnomaly(FrdmDevice* frdmDevice, ascii_char_ptr SensorId)
{
    (void)frdmDevice;
    (void)printf("Anomaly ALARM from: %s\r\n", SensorId);
    return EXECUTE_COMMAND_SUCCESS;
}

EXECUTE_COMMAND_RESULT AlarmThreshold(FrdmDevice* frdmDevice, ascii_char_ptr SensorId)
{
    (void)frdmDevice;
    (void)printf("Threshold ALARM from: %s\r\n", SensorId);
    return EXECUTE_COMMAND_SUCCESS;
}

static void sendMessage(IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle, const unsigned char* buffer, size_t size)
{
    static int messageTrackingId;
    IOTHUB_MESSAGE_HANDLE messageHandle = IoTHubMessage_CreateFromByteArray(buffer, size);
    if (messageHandle == NULL)
    {
        printf("unable to create a new IoTHubMessage\r\n");
    }
    else
    {
        if (IoTHubClient_LL_SendEventAsync(iotHubClientHandle, messageHandle, NULL, NULL) != IOTHUB_CLIENT_OK)
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
    messageTrackingId++;
}

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

int main(void)
{
    if (platform_init() != 0)
    {
        printf("Failed to initialize the platfrom.\r\n");
    }
    else
    {
        /* initialize the IoTHubClient */
        if (serializer_init(NULL) != SERIALIZER_OK)
        {
            (void)printf("Failed on serializer_init\r\n");
        }
        else
        {
            IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle = IoTHubClient_LL_CreateFromConnectionString(connectionString, HTTP_Protocol);

            if (iotHubClientHandle == NULL)
            {
                (void)printf("Failed on IoTHubClient_Create\r\n");
            }
            else
            {
                FrdmDevice* frdmDevice;
                // Because it can poll "after 9 seconds" polls will happen 
                // effectively at ~10 seconds.
                // Note that for scalabilty, the default value of minimumPollingTime
                // is 25 minutes. For more information, see:
                // https://azure.microsoft.com/documentation/articles/iot-hub-devguide/#messaging
                unsigned int minimumPollingTime = 9;
                if (IoTHubClient_LL_SetOption(iotHubClientHandle, OPTION_MIN_POLLING_TIME, &minimumPollingTime) != IOTHUB_CLIENT_OK)
                {
                    printf("failure to set option \"MinimumPollingTime\"\r\n");
                }

                frdmDevice = CREATE_MODEL_INSTANCE(Contoso, FrdmDevice, true);
                if (frdmDevice == NULL)
                {
                    (void)printf("Failed on CREATE_MODEL_INSTANCE\r\n");
                }
                else
                {
                    if (IoTHubClient_LL_SetMessageCallback(iotHubClientHandle, IoTHubMessage, frdmDevice) != IOTHUB_CLIENT_OK)
                    {
                        printf("unable to IoTHubClient_SetMessageCallback\r\n");
                    }
                    else
                    {
                        STRING_HANDLE commandsMetadata;

                        /* send the device info upon startup so that the cloud app knows
                        what commands are available and the fact that the device is up */
                        frdmDevice->ObjectType = "DeviceInfo-HW";
                        frdmDevice->ObjectName = "An ALARM device";
                        frdmDevice->Version = "1.0";
                        frdmDevice->SystemProperties.DeviceID = (char*)deviceId;
                        frdmDevice->SystemProperties.Enabled = true;

                        /* build the description of the commands on the device */
                        commandsMetadata = STRING_new();
                        if (commandsMetadata == NULL)
                        {
                            (void)printf("Failed on creating string for commands metadata\r\n");
                        }
                        else
                        {
                            /* Serialize the commands metadata as a JSON string before sending */
                            if (SchemaSerializer_SerializeCommandMetadata(GET_MODEL_HANDLE(Contoso, FrdmDevice), commandsMetadata) != SCHEMA_SERIALIZER_OK)
                            {
                                (void)printf("Failed serializing commands metadata\r\n");
                            }
                            else
                            {
                                unsigned char* destination;
                                size_t size;
                                frdmDevice->Commands = (char*)STRING_c_str(commandsMetadata);

                                /* Send the device information and commands metadata to the cloud */
                                if (SERIALIZE(&destination, &size, frdmDevice->ObjectName, frdmDevice->ObjectType, frdmDevice->SystemProperties, frdmDevice->Version, frdmDevice->Commands) != IOT_AGENT_OK)
                                {
                                    (void)printf("Failed serializing commands metadata\r\n");
                                }
                                else
                                {
                                    sendMessage(iotHubClientHandle, destination, size);
                                }
                            }

                            STRING_delete(commandsMetadata);
                        }

                        frdmDevice->ObjectName = (ascii_char_ptr)deviceId;
                        frdmDevice->ObjectType = "SensorTagEvent";
                        frdmDevice->Version = "1.0";
                        frdmDevice->TargetAlarmDevice = (ascii_char_ptr)deviceId;

                        while (1)
                        {
                            unsigned int x = 0;

                            unsigned char* destination;
                            size_t size;

                            while (x < 2)
                            {
                                /* schedule IoTHubClient to send events/receive commands */
                                IoTHubClient_LL_DoWork(iotHubClientHandle);
                                x++;
                            }

                            frdmDevice->temp = ((float)38.0 * 9 / 5) + 32;

                            if (SERIALIZE(&destination, &size, frdmDevice->ObjectName, frdmDevice->ObjectType, frdmDevice->Version, frdmDevice->TargetAlarmDevice, frdmDevice->temp) != IOT_AGENT_OK)
                            {
                                (void)printf("Failed serializing commands metadata\r\n");
                            }
                            else
                            {
                                sendMessage(iotHubClientHandle, destination, size);
                            }
                        }
                    }

                    DESTROY_MODEL_INSTANCE(frdmDevice);
                }
                IoTHubClient_LL_Destroy(iotHubClientHandle);
            }
            serializer_deinit();
        }
        platform_deinit();
    }
    return 0;
}
