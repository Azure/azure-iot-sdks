// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

//
// APPLICATION TEMPLATE FOR IoTHub LWM2M client code
//

#include "iotdm_client.h"
#include "azure_c_shared_utility/platform.h"

#include "azure_c_shared_utility/threadapi.h"
#include "device_object.h"
#include "firmwareupdate_object.h"
#include "config_object.h"
#include <stdlib.h>

IOTHUB_CLIENT_RESULT start_simulated_firmware_download(object_firmwareupdate *obj);
IOTHUB_CLIENT_RESULT start_simulated_firmware_update(object_firmwareupdate *obj);
IOTHUB_CLIENT_RESULT do_reboot(object_device *obj);
IOTHUB_CLIENT_RESULT do_factory_reset(object_device *obj);
IOTHUB_CLIENT_RESULT do_apply_config(object_config *obj);

static const char *connectionString = "[device connection string]";


// LWM2M Update State -- for /5/0/3 resource
typedef enum FIRMWARE_UPDATE_STATE_TAG
{
    LWM2M_UPDATE_STATE_NONE = 1,
    LWM2M_UPDATE_STATE_DOWNLOADING_IMAGE = 2,
    LWM2M_UPDATE_STATE_IMAGE_DOWNLOADED = 3
} LWM2M_FIRMWARE_UPDATE_STATE;

// LWM2M Update Result -- for /5/0/5 resource
typedef enum FIRMWARE_UPDATE_RESULT
{
    LWM2M_UPDATE_RESULT_DEFAULT = 0,
    LWM2M_UPDATE_RESULT_UPDATE_SUCCESSFUL = 1,
    LWM2M_UPDATE_RESULT_NOT_ENOUGH_STORAGE = 2,
    LWM2M_UPDATE_RESULT_OUT_OF_MEMORY = 3,
    LWM2M_UPDATE_RESULT_CONNECTION_LOST = 4,
    LWM2M_UPDATE_RESULT_CRC_FAILURE = 5,
    LWM2M_UPDATE_RESULT_UNSUPPORTED_PACKAGE = 6,
    LWM2M_UPDATE_RESULT_INVALID_URI = 7
} LWM2M_FIRMWARE_UPDATE_RESULT;

// App state.  This is our own construct and encapsulates update state and update result.
#define APP_UPDATE_STATE_VALUES \
    APP_UPDATE_STATE_IDLE = 0, \
    APP_UPDATE_STATE_URL_RECEIVED = 1, \
    APP_UPDATE_STATE_DOWNLOAD_IN_PROGRESS = 2, \
    APP_UPDATE_STATE_DOWNLOAD_COMPLETE = 3, \
    APP_UPDATE_STATE_UPDATE_COMMAND_RECEIVED = 4, \
    APP_UPDATE_STATE_UPDATE_IN_PROGRESS = 5, \
    APP_UPDATE_STATE_UPDATE_SUCCESSFUL = 6 
DEFINE_ENUM(APP_UPDATE_STATE, APP_UPDATE_STATE_VALUES);
DEFINE_ENUM_STRINGS(APP_UPDATE_STATE, APP_UPDATE_STATE_VALUES);

// Structure to hold the state of our simulated device
typedef struct _tagSimulatedDeviceState
{
    time_t LastBatteryUpdateTime;
    time_t DownloadStartTime;
    time_t UpdateStartTime;
    
    int BatteryLevel;
    APP_UPDATE_STATE AppUpdateState;
} SimulatedDeviceState;

SimulatedDeviceState *g_sds = NULL;

// time interval (in seconds) for how frequently to drop the battery level
#define BATTERY_DRAIN_INTERVAL_S 30

// time (in seconds) that simulated download takes
#define DOWNLOAD_TIME_S   20

// time (in seconds) that simulated flash takes
#define FLASH_TIME_S 20

// Forwards declarations
IOTHUB_CLIENT_RESULT create_update_thread(IOTHUB_CHANNEL_HANDLE iotHubChannel);

const char target_version[] = "3.0";
void set_initial_firmware_version()
{
    // randomly select either v1 or v2; target version after update will always be v3
    srand((unsigned int)time(NULL));
    (void)set_device_firmwareversion(0, rand() % 2 ? "1.0" : "2.0");
}


void set_initial_property_state()
{
    set_firmwareupdate_state(0, LWM2M_UPDATE_STATE_NONE);
    set_firmwareupdate_updateresult(0, LWM2M_UPDATE_RESULT_DEFAULT);
}


void set_callbacks()
{
    object_firmwareupdate *obj = get_firmwareupdate_object(0);
    obj->firmwareupdate_packageuri_write_callback = start_simulated_firmware_download;
    obj->firmwareupdate_update_execute_callback = start_simulated_firmware_update;

    object_device *device = get_device_object(0);
    device->device_factoryreset_execute_callback = do_factory_reset;
    device->device_reboot_execute_callback = do_reboot;

    object_config *config = get_config_object(0);
    config->config_apply_execute_callback = do_apply_config;
}


typedef struct time_out_thread_data_tag
{
    IOTHUB_CHANNEL_HANDLE hc;
    int                   to;
} time_out_thread_data;


int timeout_thread(void *data)
{
    time_out_thread_data *td = (time_out_thread_data *) data;

    ThreadAPI_Sleep(td->to * 1000);
    IoTHubClient_DM_Close(td->hc);

    return 0;
}


/**********************************************************************************
 * MAIN LOOP
 *
 **********************************************************************************/
int main(int argc, char *argv[])
{
    char *cs = (char *) connectionString;
    int   to = -1;

    if (argc == 2) // use argv[1] as CS
    {
        cs = argv[1];
    }

    else
    {
        for (int ii = 1; ii < argc; ++ii)
        {
            if (0 == strncmp(argv[ii], "-cs", 3))
            {
                ++ii;
                cs = argv[ii];
            }

            else if (0 == strncmp(argv[ii], "-to", 3))
            {
                ++ii;
                to = atol(argv[ii]);
            }

            else if (0 == strncmp(argv[ii], "-u", 2))
            {
                printf("usage: %s -cs <connection_string> [-to <time_out>\n", argv[0]);
                printf("     connection_string is the device connection string\n");
                printf("     time_out specifies the length of the run as follows:\n");
                printf("         -1 means run indefinitly.\n");
                printf("         a positive value means run for the specified number of seconds.\n");

                return 0;
            }
        }
    }

    if (0 != platform_init())
    {
        LogError("failed to initialize the platform");

        return -1;
    }

    IOTHUB_CHANNEL_HANDLE IoTHubChannel = IoTHubClient_DM_Open(cs, COAP_TCPIP);
    if (NULL == IoTHubChannel)
    {
        LogError("IoTHubClientHandle is NULL!");

        return -2;
    }

    LogInfo("IoTHubClientHandle: %p", IoTHubChannel);

    set_initial_property_state();
    set_callbacks();

    // override default version for this sample
    set_initial_firmware_version();

    if (IOTHUB_CLIENT_OK != create_update_thread(IoTHubChannel))
    {
        LogError("failure to create the udpate thread");

        IoTHubClient_DM_Close(IoTHubChannel);
        return -3;
    }

    if (to > 0)
    {
        THREAD_HANDLE        th = NULL;
        time_out_thread_data data = { IoTHubChannel, to };

        THREADAPI_RESULT tr = ThreadAPI_Create(&th, &timeout_thread, &data);
        if (tr != THREADAPI_OK)
        {
            LogError("failed to create time out thread, error=%d", tr);
        }
    }

    IoTHubClient_DM_Start(IoTHubChannel);

    /* Disconnect and cleanup */
    platform_deinit();

    return 0;
}


void update_battery_level(SimulatedDeviceState *sds)
{
    if (sds->LastBatteryUpdateTime == 0)
    {
        sds->BatteryLevel = 100;
    }

    else if ((time(NULL) - sds->LastBatteryUpdateTime) >= BATTERY_DRAIN_INTERVAL_S)
    {
        sds->BatteryLevel--;
        if (sds->BatteryLevel <= 0)
        {
            sds->BatteryLevel = 100;
        }

		LogInfo("** New Battery Level: %d\r\n", sds->BatteryLevel);
    }

	sds->LastBatteryUpdateTime = time(NULL);
	set_device_batterylevel(0, sds->BatteryLevel);
}


void update_firmware_udpate_progress(SimulatedDeviceState *sds)
{
    switch (sds->AppUpdateState)
    {
        case APP_UPDATE_STATE_IDLE:
            // nothing to do
            break;

        case APP_UPDATE_STATE_URL_RECEIVED:
            LogInfo("** firmware download started\r\n");
            sds->DownloadStartTime = time(NULL);
            LogInfo("** %s - > APP_UPDATE_STATE_DOWNLOAD_IN_PROGRESS\r\n", ENUM_TO_STRING(APP_UPDATE_STATE, sds->AppUpdateState));
            sds->AppUpdateState = APP_UPDATE_STATE_DOWNLOAD_IN_PROGRESS;
            set_firmwareupdate_state(0, LWM2M_UPDATE_STATE_DOWNLOADING_IMAGE);
            set_firmwareupdate_updateresult(0, LWM2M_UPDATE_RESULT_DEFAULT);
            break;

        case APP_UPDATE_STATE_DOWNLOAD_IN_PROGRESS:
            if ((time(NULL) - sds->DownloadStartTime) > DOWNLOAD_TIME_S)
            {
                LogInfo("** firmware download completed\r\n");
                set_firmwareupdate_state(0, LWM2M_UPDATE_STATE_IMAGE_DOWNLOADED);
                LogInfo("** %s - > APP_UPDATE_STATE_DOWNLOAD_COMPLETE\r\n", ENUM_TO_STRING(APP_UPDATE_STATE, sds->AppUpdateState));
                sds->AppUpdateState = APP_UPDATE_STATE_DOWNLOAD_COMPLETE;
            }
            break;

        case APP_UPDATE_STATE_DOWNLOAD_COMPLETE:
            // Nothing to do
            break;

        case APP_UPDATE_STATE_UPDATE_COMMAND_RECEIVED:
            LogInfo("** firmware update started\r\n");
            sds->UpdateStartTime = time(NULL);
            LogInfo("** %s - > APP_UPDATE_STATE_UPDATE_IN_PROGRESS\r\n", ENUM_TO_STRING(APP_UPDATE_STATE, sds->AppUpdateState));
            sds->AppUpdateState = APP_UPDATE_STATE_UPDATE_IN_PROGRESS;
            break;

        case APP_UPDATE_STATE_UPDATE_IN_PROGRESS:
            if ((time(NULL) - sds->UpdateStartTime) >= FLASH_TIME_S)
            {
                LogInfo("** firmware update completed\r\n");
                set_firmwareupdate_updateresult(0, LWM2M_UPDATE_RESULT_UPDATE_SUCCESSFUL);
                LogInfo("** %s - > APP_UPDATE_STATE_UPDATE_SUCCESSFUL\r\n", ENUM_TO_STRING(APP_UPDATE_STATE, sds->AppUpdateState));
                sds->AppUpdateState = APP_UPDATE_STATE_UPDATE_SUCCESSFUL;
                set_firmwareupdate_state(0, LWM2M_UPDATE_STATE_NONE);
                set_device_firmwareversion(0, target_version);
            }
            break;

        case APP_UPDATE_STATE_UPDATE_SUCCESSFUL:
            // Nothing to do
            break;
    }
}


IOTHUB_CLIENT_RESULT update_property_values(SimulatedDeviceState *sds)
{
    IOTHUB_CLIENT_RESULT result = IOTHUB_CLIENT_OK;

    printf(".");
    update_battery_level(sds);
    update_firmware_udpate_progress(sds);

    return result;
}


IOTHUB_CLIENT_RESULT start_simulated_firmware_download(object_firmwareupdate *obj)
{
    const char *uri = NULL;
    if (obj == NULL)
    {
        return IOTHUB_CLIENT_ERROR;
    }

    uri = obj->propval_firmwareupdate_packageuri;
    if (uri == NULL || *uri == 0)
    {
        LogInfo("** Empty URI received.  Resetting state machine\r\n");
        LogInfo("** %s - > APP_UPDATE_STATE_IDLE\r\n", ENUM_TO_STRING(APP_UPDATE_STATE, g_sds->AppUpdateState));
        g_sds->AppUpdateState = APP_UPDATE_STATE_IDLE;
        set_firmwareupdate_state(0, LWM2M_UPDATE_STATE_NONE);
        set_firmwareupdate_updateresult(0, LWM2M_UPDATE_RESULT_DEFAULT);
        return IOTHUB_CLIENT_OK;
    }

    else
    {
        LogInfo("** URI received from server.  Getting ready to download\r\n");
        LogInfo("** %s - > APP_UPDATE_STATE_URL_RECEIVED\r\n", ENUM_TO_STRING(APP_UPDATE_STATE, g_sds->AppUpdateState));
        g_sds->AppUpdateState = APP_UPDATE_STATE_URL_RECEIVED;
        return IOTHUB_CLIENT_OK;
    }
}


IOTHUB_CLIENT_RESULT start_simulated_firmware_update(object_firmwareupdate *obj)
{
    LogInfo("** firmware update request posted\n");
    if (g_sds->AppUpdateState == APP_UPDATE_STATE_DOWNLOAD_COMPLETE)
    {
        LogInfo("** %s - > APP_UPDATE_STATE_UPDATE_COMMAND_RECEIVED\n", ENUM_TO_STRING(APP_UPDATE_STATE, g_sds->AppUpdateState));
        g_sds->AppUpdateState = APP_UPDATE_STATE_UPDATE_COMMAND_RECEIVED;
        return IOTHUB_CLIENT_OK;
    }

    else
    {
        LogInfo("** returning failure. Invalid state -- state = %s\n", ENUM_TO_STRING(APP_UPDATE_STATE, g_sds->AppUpdateState));
        return IOTHUB_CLIENT_INVALID_ARG;
    }
}


int update_thread(void *v)
{
    SimulatedDeviceState sds = { 0 };
    g_sds = &sds;

	IOTHUB_CLIENT_RESULT result;
	IOTHUB_CHANNEL_HANDLE iotHubChannel = (IOTHUB_CHANNEL_HANDLE *) v;
    while (true)
    {
        ThreadAPI_Sleep(5000);
		if (IotHubClient_DM_EnterCriticalSection(iotHubChannel))
		{
			result = update_property_values(&sds);
			IotHubClient_DM_LeaveCriticalSection(iotHubChannel);
		}

		else
		{
			result = IOTHUB_CLIENT_ERROR;
		}

		if (result != IOTHUB_CLIENT_OK)
		{
            LogError("Failed updating property values.  Exiting thread");
            return -1;
        }
    }

    return 0;
}


IOTHUB_CLIENT_RESULT create_update_thread(IOTHUB_CHANNEL_HANDLE iotHubChannel)
{
    IOTHUB_CLIENT_RESULT result = IOTHUB_CLIENT_OK;
    THREAD_HANDLE th = NULL;

    THREADAPI_RESULT tr = ThreadAPI_Create(&th, &update_thread, (void *) iotHubChannel);
    if (tr != THREADAPI_OK)
    {
        LogError("failed to create background thread, error=%d", tr);
        result = IOTHUB_CLIENT_ERROR;
    }

    return result;
}


IOTHUB_CLIENT_RESULT do_reboot(object_device *obj)
{
    LogInfo("** Rebooting device\r\n");
    return IOTHUB_CLIENT_OK;
}


IOTHUB_CLIENT_RESULT do_factory_reset(object_device *obj)
{
    LogInfo("** Factory resetting device\r\n");
    return IOTHUB_CLIENT_OK;
}


IOTHUB_CLIENT_RESULT do_apply_config(object_config *obj)
{
    LogInfo("** Applying Configuration - name = [%s] value = [%s]\r\n", obj->propval_config_name, obj->propval_config_value);
    return IOTHUB_CLIENT_OK;
}