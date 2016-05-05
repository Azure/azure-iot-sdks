// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef IOTHUB_SERVICE_CLIENT_AUTH_H
#define IOTHUB_SERVICE_CLIENT_AUTH_H

#ifdef __cplusplus
extern "C"
{
#else
#endif

#include "macro_utils.h"

typedef struct IOTHUB_SERVICE_CLIENT_AUTH_TAG
{
    const char* hostname;
    const char* iothubName;
    const char* iothubSuffix;
    const char* sharedAccessKey;
    const char* keyName;
} IOTHUB_SERVICE_CLIENT_AUTH;

typedef struct IOTHUB_SERVICE_CLIENT_AUTH_TAG* IOTHUB_SERVICE_CLIENT_AUTH_HANDLE;

extern IOTHUB_SERVICE_CLIENT_AUTH_HANDLE IoTHubServiceClientAuth_CreateFromConnectionString(const char* connectionString);
extern void IoTHubServiceClientAuth_Destroy(IOTHUB_SERVICE_CLIENT_AUTH_HANDLE serviceClientHandle);

#ifdef __cplusplus
}
#endif

#endif // IOTHUB_SERVICE_CLIENT_AUTH_H
