// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef _EDISON_H_
#define _EDISON_H_

bool is_download_happening();

bool is_update_happening();

char *get_serial_number();

char *get_firmware_version();

bool spawn_reboot_process();

bool spawn_factoryreset_process();

bool spawn_download_process(const char *uri);

bool spawn_update_process();

#endif // _EDISON_H_


