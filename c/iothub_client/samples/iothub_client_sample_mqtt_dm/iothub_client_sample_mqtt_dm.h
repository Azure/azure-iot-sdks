// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef IOTHUB_CLIENT_SAMPLE_MQTT_DM_H
#define IOTHUB_CLIENT_SAMPLE_MQTT_DM_H

#include "iothub_client.h"

#ifdef __cplusplus
extern "C"
{
#endif

    char* device_get_firmware_version(void);
    char* device_get_connection_string(void);

    bool device_download_firmware(const char *param);
    bool device_update_firmware(void);
    void device_reboot(void);

    bool device_run_service(void);

#ifdef __cplusplus
}
#endif


#endif /* IOTHUB_CLIENT_SAMPLE_MQTT_DM_H */
