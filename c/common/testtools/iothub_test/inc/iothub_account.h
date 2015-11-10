// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef IOTHUBACCOUNT_H
#define IOTHUBACCOUNT_H

#ifdef __cplusplus
#include <cstddef>
extern "C"
{
#else
#include <stddef.h>
#endif

extern const char* IoTHubAccount_GetEventHubConnectionString(void);
extern const char* IoTHubAccount_GetIoTHubName(void);
extern const char* IoTHubAccount_GetIoTHubSuffix(void);
extern const char* IoTHubAccount_GetDeviceId(void);
extern const char* IoTHubAccount_GetDeviceKey(void);
extern const char* IoTHubAccount_GetEventhubListenName(void);
extern const char* IoTHubAccount_GetIoTHubConnString(void);
extern const char* IoTHubAccount_GetSharedAccessSignature(void);
extern const char* IoTHubAccount_GetEventhubAccessKey(void);
extern const size_t IoTHubAccount_GetIoTHubPartitionCount(void);
extern const char* IoTHubAccount_GetEventhubConsumerGroup(void);
extern const char* IoTHubAccount_GetProtocolGatewayHostName(void);
#ifdef __cplusplus
}
#endif

#endif // IOTHUBACCOUNT_H
