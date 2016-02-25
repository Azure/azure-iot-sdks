// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdio.h>
#include "simplesample_amqp.h"
#include "platform.h"


int main(void)
{
	int result;
    (void)printf("Initializing mbed specific things...\r\n");

	if ((result = platform_init()) != 0)
	{
		(void)printf("Error initializing the platform: %d\r\n",result);
		return -1;
	}

    simplesample_amqp_run();

	platform_deinit();
	return 0;
}
