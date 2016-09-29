// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef IOTHUB_CLIENT_SAMPLE_MQTT_DM_H
#define IOTHUB_CLIENT_SAMPLE_MQTT_DM_H

#include "iothub_client.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /* enum values are in lower case per PM design */
    #define FIRMWARE_UPDATE_STATUS_VALUES \
        waiting, \
        downloading, \
        downloadComplete, \
        applying
    DEFINE_ENUM(FIRMWARE_UPDATE_STATUS, FIRMWARE_UPDATE_STATUS_VALUES)

    char* device_get_firmware_version(void);
    char* device_read_string_from_file(const char *fileName);

    FIRMWARE_UPDATE_STATUS device_get_firmware_update_status(void);
    bool device_download_firmware(const char *param);
    bool device_update_firmware(void);

    bool device_run_service(void);

#ifdef __cplusplus
}
#endif


#endif /* IOTHUB_CLIENT_SAMPLE_MQTT_DM_H */
