// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdio.h>
#include "EthernetInterface.h"
#include "mbed/logging.h"
#include "mbed/mbedtime.h"
#include "runtests.h"

int main(void)
{
	(void)printf("Initializing mbed specific things...\r\n");
	
	mbed_log_init();
	mbedtime_init();
	
	EthernetInterface eth;
	if (eth.init() || eth.connect())
	{
		(void)printf("Error initializing EthernetInterface.\r\n");
		return -1;
	}
	
	return run_tests();
}
