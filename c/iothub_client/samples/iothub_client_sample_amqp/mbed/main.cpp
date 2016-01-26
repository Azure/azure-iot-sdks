// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdio.h>
#include "EthernetInterface.h"
#include "mbed/logging.h"
#include "mbed/mbedtime.h"
#include "iothub_client_sample_amqp.h"
#include "NTPClient.h"
#include "azureiot_common/platform.h"

int main(void)
{
	(void)printf("Initializing mbed specific things...\r\n");

        /* These are needed in order to initialize the time provider for Proton-C */
	mbed_log_init();
	mbedtime_init();
	int result;
	
	if ((result = platform_init()) != 0)
	{
		(void)printf("Error initializing the platform: %d\r\n",result);
		return -1;
	}
	
	iothub_client_sample_amqp_run();

	platform_deinit();
}
