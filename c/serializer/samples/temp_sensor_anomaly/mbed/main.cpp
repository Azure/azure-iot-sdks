// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include "iothubtransporthttp.h"
#include "serializer.h"
#include "schemaserializer.h"
#include "threadapi.h"
#include "C12832.h"
#include "LM75B.h"
#include "platform.h"

#ifdef MBED_BUILD_TIMESTAMP
#include "certs.h"
#endif // MBED_BUILD_TIMESTAMP

C12832 lcd(D11, D13, D12, D7, D10);
LM75B sensor(D14, D15);
DigitalIn Fire(D4);
DigitalOut red_led(D5);
DigitalOut blue_led(D8);
DigitalOut green_led(D9);
PwmOut spkr(D6);
Serial pc(USBTX, USBRX);

static const char* connectionString = "[device connection string]";
static const char* deviceId = "[deviceName]"; /*must match the one on connectionString*/

static Timer led_timer;
static unsigned char alarm_type;
static unsigned char led_on;
static unsigned int last_alarm_time;
static unsigned int last_edge_time;
static unsigned int blink_interval;
static float temp;
static char* sensorId = NULL;

#define ALARM_NONE 0
#define ALARM_ANOMALY 1
#define ALARM_THRESHOLD 2

#define BLINK_TIME 5000     /* ms */
#define BLINK_INTERVAL_ANOMALY 250  /* ms */
#define BLINK_INTERVAL_THRESHOLD 100  /* ms */

// Define the Model
BEGIN_NAMESPACE(Contoso);

DECLARE_STRUCT(SystemProperties,
ascii_char_ptr, DeviceID,
_Bool, Enabled
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

static int LED_Update_Thread(void* threadArgument)
{
    unsigned char display_counter = 0;
    led_timer.start();

    last_alarm_time = led_timer.read_ms() - BLINK_TIME;
    while (1)
    {
        unsigned int current_ms = led_timer.read_ms();
        float new_temp_value;

        if (alarm_type != ALARM_NONE)
        {
            if (current_ms - last_alarm_time > BLINK_TIME)
            {
                /* no more alarm */
                alarm_type = ALARM_NONE;
                free(sensorId);
                sensorId = NULL;
                led_on = 0;

                /* reset LED and clear display and speaker */
                red_led = 1;

                lcd.cls();
                spkr = 0.0;
            }
            else
            {
                if (current_ms - last_edge_time > blink_interval)
                {
                    led_on = 1 - led_on;
                    last_edge_time = current_ms;
                }

                if (led_on)
                {
                    red_led = 0;
                    spkr.period(1.0 / 2000.0);
                    spkr = 0.5;
                }
                else
                {
                    red_led = 1;
                    spkr = 0.0;
                }
            }
        }
        else
        {
            /* alarm off, do nothing */
        }

        new_temp_value = (sensor.temp() * 9 / 5) + 32;
        temp = temp + (new_temp_value - temp) / 2;

        display_counter++;
        if (display_counter == 80)
        {
            display_counter = 0;
            lcd.locate(0, 3);
            lcd.printf("Temp = %.1f\n", temp);
            if (sensorId != NULL)
            {
                lcd.locate(0, 15);
                lcd.printf("%s : %s", (alarm_type == ALARM_ANOMALY) ? "Anomaly" : "Thrshld", sensorId);
            }
        }

        ThreadAPI_Sleep(10);
    }

    led_timer.stop();

    return 0;
}

EXECUTE_COMMAND_RESULT AlarmAnomaly(FrdmDevice* frdmDevice, ascii_char_ptr SensorId)
{
    size_t length;

    last_alarm_time = led_timer.read_ms();
    if (alarm_type != ALARM_THRESHOLD)
    {
        if (alarm_type == ALARM_NONE)
        {
            last_edge_time = last_alarm_time;
        }
        blink_interval = BLINK_INTERVAL_ANOMALY;
    }

    alarm_type = ALARM_ANOMALY;

    /* clear screen */
    lcd.cls();

    length = strlen(SensorId);
    free(sensorId);
    sensorId = (char*)malloc(length + 1);
    strcpy(sensorId, SensorId);
    return EXECUTE_COMMAND_SUCCESS;
}

EXECUTE_COMMAND_RESULT AlarmThreshold(FrdmDevice* frdmDevice, ascii_char_ptr SensorId)
{
    size_t length;

    last_alarm_time = led_timer.read_ms();
    if (alarm_type != ALARM_THRESHOLD)
    {
        last_edge_time = last_alarm_time;
    }
    blink_interval = BLINK_INTERVAL_THRESHOLD;
    alarm_type = ALARM_THRESHOLD;

    /* clear screen */
    lcd.cls();

    /* print ALARM */
    length = strlen(SensorId);
    free(sensorId);
    sensorId = (char*)malloc(length + 1);
    strcpy(sensorId, SensorId);
    return EXECUTE_COMMAND_SUCCESS;
}

/*this function "links" IoTHub to the serialization library*/
static IOTHUBMESSAGE_DISPOSITION_RESULT IoTHubMessage(IOTHUB_MESSAGE_HANDLE message, void* userContextCallback)
{
    const unsigned char* buffer;
    size_t size;
    if (IoTHubMessage_GetByteArray(message, &buffer, &size) != IOTHUB_MESSAGE_OK)
    {
        (void)printf("unable to IoTHubMessage_GetByteArray\r\n");
    }
    else
    {
        /*buffer is not zero terminated*/
        STRING_HANDLE temp = STRING_construct_n((char*)buffer, size);
        if (temp == NULL)
        {
            (void)printf("unable to STRING_construct_n\r\n");
        }
        else
        {
            EXECUTE_COMMAND(userContextCallback, STRING_c_str(temp));
            STRING_delete(temp);
        }
    }
    return IOTHUBMESSAGE_ACCEPTED;
}

static void sendMessage(IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle, const unsigned char* buffer, size_t size)
{
    IOTHUB_MESSAGE_HANDLE messageHandle = IoTHubMessage_CreateFromByteArray(buffer, size);
    if (messageHandle == NULL)
    {
        (void)printf("unable to create a new IoTHubMessage\r\n");
    }
    else
    {
        if (IoTHubClient_LL_SendEventAsync(iotHubClientHandle, messageHandle, NULL, NULL) != IOTHUB_CLIENT_OK)
        {
            (void)printf("failed to hand over the message to IoTHubClient");
        }
        else
        {
            (void)printf("IoTHubClient accepted the message for delivery\r\n");
        }
        IoTHubMessage_Destroy(messageHandle);
    }
}

int main(void)
{
	int result;

	if ((result = platform_init()) != 0)
	{
		(void)printf("Error initializing the platform: %d\r\n",result);
		result = -1;
	}
    else
    {

        THREAD_HANDLE ThreadHandle;

        /* clear the LED light upon startup */
        red_led = 1;
        blue_led = 1;
        green_led = 1;

        alarm_type = ALARM_NONE;
        led_on = 0;

        /* clear the screen */
        lcd.cls();

        if (ThreadAPI_Create(&ThreadHandle, LED_Update_Thread, NULL) != THREADAPI_OK)
        {
            (void)printf("Error spinning LED update thread.\r\n");
            result = -1;
        }
        else
        {
            /* initialize the IoTHubClient */
            if (serializer_init(NULL) != SERIALIZER_OK)
            {
                (void)printf("Failed on serializer_init\r\n");
                result = -1;
            }
            else
            {
                /* Setup IoTHub client configuration */

                IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle = IoTHubClient_LL_CreateFromConnectionString(connectionString, HTTP_Protocol);

                if (iotHubClientHandle == NULL)
                {
                    (void)printf("Failed on IoTHubClient_Create\r\n");
                    result = -1;
                }
                else
                {
#ifdef MBED_BUILD_TIMESTAMP
                    // For mbed add the certificate information
                    if (IoTHubClient_LL_SetOption(iotHubClientHandle, "TrustedCerts", certificates) != IOTHUB_CLIENT_OK)
                    {
                        printf("failure to set option \"TrustedCerts\"\r\n");
                    }
#endif // MBED_BUILD_TIMESTAMP

                    // Because it can poll "after 9 seconds" polls will happen 
                    // effectively at ~10 seconds.
                    // Note that for scalabilty, the default value of minimumPollingTime
                    // is 25 minutes. For more information, see:
                    // https://azure.microsoft.com/documentation/articles/iot-hub-devguide/#messaging
                    unsigned int minimumPollingTime = 9;
                    if (IoTHubClient_LL_SetOption(iotHubClientHandle, "MinimumPollingTime", &minimumPollingTime) != IOTHUB_CLIENT_OK)
                    {
                        printf("failure to set option \"MinimumPollingTime\"\r\n");
                    }

                    FrdmDevice* frdmDevice = CREATE_MODEL_INSTANCE(Contoso, FrdmDevice, true);
                    if (frdmDevice == NULL)
                    {
                        (void)printf("Failed on CREATE_MODEL_INSTANCE\r\n");
                        result = -1;
                    }
                    else
                    {
                        IOTHUB_CLIENT_RESULT setMessageResult = IoTHubClient_LL_SetMessageCallback(iotHubClientHandle, IoTHubMessage, frdmDevice);
                        if (setMessageResult != IOTHUB_CLIENT_OK)
                        {
                            (void)printf("unable to IoTHubClient_SetMessageCallback\r\n");
                            result = -1;
                        }
                        else
                        {
                            STRING_HANDLE commandsMetadata;

                            temp = (sensor.temp() * 9 / 5) + 32;

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
                                result = -1;
                            }
                            else
                            {
                                /* Serialize the commands metadata as a JSON string before sending */
                                if (SchemaSerializer_SerializeCommandMetadata(GET_MODEL_HANDLE(Contoso, FrdmDevice), commandsMetadata) != SCHEMA_SERIALIZER_OK)
                                {
                                    (void)printf("Failed serializing commands metadata\r\n");
                                    result = -1;
                                }
                                else
                                {
                                    frdmDevice->Commands = (char*)STRING_c_str(commandsMetadata);

                                    /* Send the device information and commands metadata to the cloud */
                                    {
                                        unsigned char* destination;
                                        size_t destinationSize;
                                        if (SERIALIZE(&destination, &destinationSize, frdmDevice->ObjectName, frdmDevice->ObjectType, frdmDevice->SystemProperties, frdmDevice->Version, frdmDevice->Commands) != IOT_AGENT_OK)
                                        {
                                            (void)printf("Failed to serialize\r\n");
                                            result = -1;
                                        }
                                        else
                                        {
                                            sendMessage(iotHubClientHandle, destination, destinationSize);
                                            free(destination);
                                        }
                                    }
                                }

                                STRING_delete(commandsMetadata);
                            }

                            frdmDevice->ObjectName = (ascii_char_ptr)deviceId;
                            frdmDevice->ObjectType = "SensorTagEvent";
                            frdmDevice->Version = "1.0";
                            frdmDevice->TargetAlarmDevice = (ascii_char_ptr)deviceId;
                            result = 0;

                            while (1)
                            {
                                unsigned char* destination;
                                size_t destinationSize;

                                (void)printf("Sending %.02f\r\n", temp);
                                frdmDevice->temp = temp;

                                if (SERIALIZE(&destination, &destinationSize, frdmDevice->ObjectName, frdmDevice->ObjectType, frdmDevice->Version, frdmDevice->TargetAlarmDevice, frdmDevice->temp) != IOT_AGENT_OK)
                                {
                                    (void)printf("Failed to serialize\r\n");
                                }
                                else
                                {
                                    sendMessage(iotHubClientHandle, destination, destinationSize);
                                    free(destination);
                                }

                                /* schedule IoTHubClient to send events/receive commands */
                                IoTHubClient_LL_DoWork(iotHubClientHandle);
                            }
                        }
                        DESTROY_MODEL_INSTANCE(frdmDevice);
                    }
                    IoTHubClient_LL_Destroy(iotHubClientHandle);
                }
                serializer_deinit();
            }
        }
        platform_deinit();
    }
    return result;
}
