// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdio.h>
#include "platform.h"
#include "iothub_client_sample_amqp.h"

int main(void)
{
    (void)printf("Initializing mbed specific things...\r\n");

    if (platform_init() != 0)
    {
        (void)printf("Failed initializing platform.\r\n");
        return -1;
    }

    iothub_client_sample_amqp_run();

    platform_deinit();
    return 0;
}
