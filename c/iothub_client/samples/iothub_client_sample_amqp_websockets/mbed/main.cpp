// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdio.h>
#include "EthernetInterface.h"
#include "mbed/logging.h"
#include "mbed/mbedtime.h"
#include "iothub_client_sample_amqp_websockets.h"
#include "NTPClient.h"
#includes "platform.h"

int main(void)
{
	(void)printf("Initializing mbed specific things...\r\n");

	mbed_log_init();
	mbedtime_init();

	if (EthernetInterface::init())
	{
		(void)printf("Error initializing EthernetInterface.\r\n");
		return -1;
	}

	if (platform_init() != 0)
	{
		(void)printf("Failed initializing platform.\r\n");
		return -1;
	}

	if (EthernetInterface::connect())
	{
		(void)printf("Error connecting EthernetInterface.\r\n");
		return -1;
	}

	iothub_client_sample_amqp_websockets_run();

	(void)EthernetInterface::disconnect();
}
