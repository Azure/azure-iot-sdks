// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef IOTHUB_TRANSPORT_H
#define IOTHUB_TRANSPORT_H

typedef struct TRANSPORT_HL_HANDLE_DATA_TAG* TRANSPORT_HL_HANDLE;

#include "lock.h"
#include "crt_abstractions.h"
#include "iothub_client.h"
#include "iothub_client_private.h"

#ifdef __cplusplus
extern "C"
{
#endif

extern TRANSPORT_HL_HANDLE  IoTHubTransport_HL_Create(IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol, const char* iotHubName, const char* iotHubSuffix);
extern void					IoTHubTransport_HL_Destroy(TRANSPORT_HL_HANDLE transportHlHandle);
extern LOCK_HANDLE			IoTHubTransport_HL_GetLock(TRANSPORT_HL_HANDLE transportHlHandle);
extern TRANSPORT_HANDLE		IoTHubTransport_HL_GetLLTransport(TRANSPORT_HL_HANDLE transportHlHandle);
extern IOTHUB_CLIENT_RESULT IoTHubTransport_HL_StartWorkerThread(TRANSPORT_HL_HANDLE transportHlHandle, IOTHUB_CLIENT_HANDLE clientHandle);
extern void					IoTHubTransport_HL_EndWorkerThread(TRANSPORT_HL_HANDLE transportHlHandle, IOTHUB_CLIENT_HANDLE clientHandle);

#ifdef __cplusplus
}
#endif

#endif /* IOTHUB_TRANSPORT_H */
