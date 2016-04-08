// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef IOTHUBTEST_H
#define IOTHUBTEST_H

#ifdef __cplusplus
#include <cstddef>
extern "C"
{
#else
#include <stddef.h>
#endif

#include "azure_c_shared_utility/macro_utils.h"
#include "azure_c_shared_utility/buffer_.h"

typedef void* IOTHUB_TEST_HANDLE;

#define IOTHUB_TEST_CLIENT_RESULT_VALUES \
    IOTHUB_TEST_CLIENT_OK, \
    IOTHUB_TEST_CLIENT_ERROR

DEFINE_ENUM(IOTHUB_TEST_CLIENT_RESULT, IOTHUB_TEST_CLIENT_RESULT_VALUES);

typedef int (*pfIoTHubMessageCallback)(void* context, const char* data, size_t size);

extern IOTHUB_TEST_HANDLE IoTHubTest_Initialize(const char* eventhubConnString, const char* iothubConnString, const char* deviceId, const char* deviceKey, const char* eventhubName, const char* eventhubAccessKey, const char* sharedSignature, const char* consumerGroup);
extern void IoTHubTest_Deinit(IOTHUB_TEST_HANDLE devhubHandle);
extern IOTHUB_TEST_CLIENT_RESULT IoTHubTest_ListenForEvent(IOTHUB_TEST_HANDLE devhubHandle, pfIoTHubMessageCallback msgCallback, size_t partitionCount, void* context, time_t receiveTimeRangeStart, double maxDrainTimeInSeconds);
extern IOTHUB_TEST_CLIENT_RESULT IoTHubTest_ListenForRecentEvent(IOTHUB_TEST_HANDLE devhubHandle, pfIoTHubMessageCallback msgCallback, size_t partitionCount, void* context, double maxDrainTimeInSeconds);
extern IOTHUB_TEST_CLIENT_RESULT IoTHubTest_ListenForEventForMaxDrainTime(IOTHUB_TEST_HANDLE devhubHandle, pfIoTHubMessageCallback msgCallback, size_t partitionCount, void* context);
extern IOTHUB_TEST_CLIENT_RESULT IoTHubTest_SendMessage(IOTHUB_TEST_HANDLE devhubHandle, const unsigned char* data, size_t len);

#ifdef __cplusplus
}
#endif

#endif // IOTHUBTEST_H 
