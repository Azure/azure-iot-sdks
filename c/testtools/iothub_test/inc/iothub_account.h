// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef IOTHUBACCOUNT_H
#define IOTHUBACCOUNT_H

#include "iothub_messaging_ll.h"
#include "azure_c_shared_utility/crt_abstractions.h"

#ifdef __cplusplus
#include <cstddef>
extern "C"
{
#else
#include <stddef.h>
#endif

typedef struct IOTHUB_ACCOUNT_INFO_TAG* IOTHUB_ACCOUNT_INFO_HANDLE;

extern IOTHUB_ACCOUNT_INFO_HANDLE IoTHubAccount_Init(bool createDevice);
extern void IoTHubAccount_deinit(IOTHUB_ACCOUNT_INFO_HANDLE acctHandle);

extern const char* IoTHubAccount_GetEventHubConnectionString(IOTHUB_ACCOUNT_INFO_HANDLE acctHandle);
extern const char* IoTHubAccount_GetIoTHubName(IOTHUB_ACCOUNT_INFO_HANDLE acctHandle);
extern const char* IoTHubAccount_GetIoTHubSuffix(IOTHUB_ACCOUNT_INFO_HANDLE acctHandle);
extern const char* IoTHubAccount_GetDeviceId(IOTHUB_ACCOUNT_INFO_HANDLE acctHandle);
extern const char* IoTHubAccount_GetDeviceKey(IOTHUB_ACCOUNT_INFO_HANDLE acctHandle);
extern const char* IoTHubAccount_GetEventhubListenName(IOTHUB_ACCOUNT_INFO_HANDLE acctHandle);
extern const char* IoTHubAccount_GetIoTHubConnString(IOTHUB_ACCOUNT_INFO_HANDLE acctHandle);
extern const char* IoTHubAccount_GetSharedAccessSignature(IOTHUB_ACCOUNT_INFO_HANDLE acctHandle);
extern const char* IoTHubAccount_GetEventhubAccessKey(IOTHUB_ACCOUNT_INFO_HANDLE acctHandle);
extern const size_t IoTHubAccount_GetIoTHubPartitionCount(IOTHUB_ACCOUNT_INFO_HANDLE acctHandle);
extern const char* IoTHubAccount_GetEventhubConsumerGroup(IOTHUB_ACCOUNT_INFO_HANDLE acctHandle);
extern const IOTHUB_MESSAGING_HANDLE IoTHubAccount_GetMessagingHandle(IOTHUB_ACCOUNT_INFO_HANDLE acctHandle);

#ifdef __cplusplus
}
#endif

#endif // IOTHUBACCOUNT_H
