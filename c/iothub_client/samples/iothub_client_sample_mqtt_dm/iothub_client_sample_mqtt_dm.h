// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef IOTHUB_CLIENT_SAMPLE_MQTT_DM_H
#define IOTHUB_CLIENT_SAMPLE_MQTT_DM_H

#include <stdio.h>
#include <stdlib.h>

#include "serializer.h"
#include "iothub_client.h"

#include "azure_c_shared_utility/threadapi.h"
#include "azure_c_shared_utility/platform.h"

#include "iothubtransportmqtt.h"


#define FIRMWARE_UPDATE_STATE_VALUES \
    FIRMWARE_UPDATE_STATE_NONE = 1, \
    FIRMWARE_UPDATE_STATE_DOWNLOADING_IMAGE = 2, \
    FIRMWARE_UPDATE_STATE_IMAGE_DOWNLOADED = 3, \
    FIRMWARE_UPDATE_STATE_IMAGE_APPLYING = 4
DEFINE_ENUM(FIRMWARE_UPDATE_STATE, FIRMWARE_UPDATE_STATE_VALUES)


#ifdef __cplusplus
extern "C" {
#endif

    void device_get_name(char *&name);
    void device_get_time(time_t &time);
    void device_get_tz(char *&tz);

    void device_get_serial_number(char *&out);
    void device_get_firmware_version(char *&out);

    bool device_reboot();
    bool device_factoryreset();
	FIRMWARE_UPDATE_STATE device_get_firmware_update_state();
    bool device_download_firmware(const char *uri);
    bool device_update_firmware();

    void device_run_service();

#ifdef __cplusplus
}
#endif

#endif /* IOTHUB_CLIENT_SAMPLE_MQTT_DM_H */
