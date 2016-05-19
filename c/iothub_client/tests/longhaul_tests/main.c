// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "longhaul_tests.h"

int main(void)
{
    int failedTestCount = 0;
	
	if (IoTHubClient_LongHaul_12h_Run_1_Event_Per_Sec() != 0)
	{
		failedTestCount++;
	}
    
	return failedTestCount;
}
