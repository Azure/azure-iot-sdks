// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

//
// APPLICATION TEMPLATE FOR IoTHub LWM2M client code
//

#include "iothub_lwm2m.h"
#include "threadapi.h"
#include "device_object.h"
#include "firmwareupdate_object.h"

IOTHUB_CLIENT_RESULT start_simulated_firmware_download(object_firmwareupdate *obj);
IOTHUB_CLIENT_RESULT start_simulated_firmware_update(object_firmwareupdate *obj);

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
#define BATTERY_DRAIN_INTERVAL_S 10

// time (in seconds) that simulated download takes
#define DOWNLOAD_TIME_S   20

// time (in seconds) that simulated flash takes
#define FLASH_TIME_S 20

// Forwards declarations
IOTHUB_CLIENT_RESULT create_update_thread();

/**********************************************************************************
 * MAIN LOOP
 *
 **********************************************************************************/
int main(int argc, char *argv[])
{
    const char *cs;

    if (argc == 2) cs = argv[1];
    else cs = connectionString;

    IOTHUB_CHANNEL_HANDLE IoTHubChannel = IoTHubClient_DM_Open(cs, COAP_TCPIP);
    if (NULL == IoTHubChannel)
    {
        LogError("IoTHubClientHandle is NULL!\r\n");

        return -1;
    }

    LogInfo("IoTHubClientHandle: %p\r\n", IoTHubChannel);

    LogInfo("prepare LWM2M objects");
    if (IOTHUB_CLIENT_OK != IoTHubClient_DM_CreateDefaultObjects(IoTHubChannel))
    {
        LogError("failure to create LWM2M objects for client: %p\r\n", IoTHubChannel);

        return -1;
    }
    
    set_firmwareupdate_state(0, LWM2M_UPDATE_STATE_NONE);
    set_firmwareupdate_updateresult(0, LWM2M_UPDATE_RESULT_DEFAULT);
    object_firmwareupdate *obj = get_firmwareupdate_object(0);
    if (obj == NULL)
    {
        LogError("failure to get firmware update object for %p\r\n", IoTHubChannel);

        return -1;
    }
    obj->firmwareupdate_packageuri_write_callback = start_simulated_firmware_download;
    obj->firmwareupdate_update_execute_callback = start_simulated_firmware_update;

    if (IOTHUB_CLIENT_OK != create_update_thread())
    {
        LogError("failure to create the udpate thread\r\n");

        return -1;
    }

    if (IOTHUB_CLIENT_OK != IoTHubClient_DM_Start(IoTHubChannel))
    {
        LogError("failure to start the client: %p\r\n", IoTHubChannel);

        return -1;
    }

    /* Disconnect and cleanup */
    IoTHubClient_DM_Close(IoTHubChannel);
}

void update_battery_level(SimulatedDeviceState *sds)
{
    if (sds->LastBatteryUpdateTime == 0)
    {
        sds->BatteryLevel = 100;
        sds->LastBatteryUpdateTime = time(NULL);
        set_device_batterylevel(0,sds->BatteryLevel);
    }
    else if ((time(NULL) - sds->LastBatteryUpdateTime) >= BATTERY_DRAIN_INTERVAL_S)
    {
        sds->BatteryLevel--;
        if (sds->BatteryLevel <= 0)
            sds->BatteryLevel = 100;
        sds->LastBatteryUpdateTime = time(NULL);
        set_device_batterylevel(0,sds->BatteryLevel);
    }
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
        set_firmwareupdate_state(0, LWM2M_UPDATE_STATE_NONE);
        break;
    case APP_UPDATE_STATE_UPDATE_IN_PROGRESS:
        if ((time(NULL) - sds->UpdateStartTime) >= FLASH_TIME_S)
        {
            LogInfo("** firmware update completed\r\n");
            set_firmwareupdate_updateresult(0, LWM2M_UPDATE_RESULT_UPDATE_SUCCESSFUL);
            LogInfo("** %s - > APP_UPDATE_STATE_UPDATE_SUCCESSFUL\r\n", ENUM_TO_STRING(APP_UPDATE_STATE, sds->AppUpdateState));
            sds->AppUpdateState = APP_UPDATE_STATE_UPDATE_SUCCESSFUL;
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
    LogInfo("** firmware update request posted\r\n");
    if (g_sds->AppUpdateState == APP_UPDATE_STATE_DOWNLOAD_COMPLETE)
    {
        LogInfo("** %s - > APP_UPDATE_STATE_UPDATE_COMMAND_RECEIVED\r\n", ENUM_TO_STRING(APP_UPDATE_STATE, g_sds->AppUpdateState));
        g_sds->AppUpdateState = APP_UPDATE_STATE_UPDATE_COMMAND_RECEIVED;
        return IOTHUB_CLIENT_OK;
    }
    else
    {
        LogInfo("** returning failure. Invalid state -- state = %s\r\n", ENUM_TO_STRING(APP_UPDATE_STATE, g_sds->AppUpdateState));
        return IOTHUB_CLIENT_INVALID_ARG;
    }
}


int update_thread(void *v)
{
    SimulatedDeviceState sds = { 0 };
    g_sds = &sds;

    while (true)
    {
        ThreadAPI_Sleep(1000);

        if (update_property_values(&sds) != IOTHUB_CLIENT_OK)
        {
            LogError("Failed updating property values.  Exiting thread");
            return -1;
        }
    }

    return 0;
    
}

IOTHUB_CLIENT_RESULT create_update_thread()
{
    IOTHUB_CLIENT_RESULT result = IOTHUB_CLIENT_OK;
    THREAD_HANDLE th = NULL;

    THREADAPI_RESULT tr = ThreadAPI_Create(&th, &update_thread, NULL);
    if (tr != THREADAPI_OK)
    {
        LogError("failed to create background thread, error=%d", tr);
        result = IOTHUB_CLIENT_ERROR;
    }

    return result;
}

