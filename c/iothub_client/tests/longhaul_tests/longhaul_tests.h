// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef LONGHAUL_TESTS_H
#define LONGHAUL_TESTS_H

#ifdef __cplusplus
extern "C" {
#endif

	// Description: 
	//		Runs IoT Hub client longhaul tests for 12 hours, attempting to send 1 event per second.
	// Return: 
	//		0 if test is successful, or a positive value if any failure occurs.
	// Remarks: 
	//		Can have its duration modified by the env var IOTHUB_LONGHAUL_TEST_DURATION_IN_SECONDS.
	int IoTHubClient_LongHaul_12h_Run_1_Event_Per_Sec(void);

#ifdef __cplusplus
}
#endif

#endif /* LONGHAUL_TESTS_H */
