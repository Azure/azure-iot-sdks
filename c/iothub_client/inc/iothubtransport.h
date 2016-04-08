// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef IOTHUB_TRANSPORT_H
#define IOTHUB_TRANSPORT_H

typedef struct TRANSPORT_HANDLE_DATA_TAG* TRANSPORT_HANDLE;

#include "azure_c_shared_utility/lock.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "iothub_client.h"
#include "iothub_client_private.h"
#include "iothub_transport_ll.h"

#ifdef __cplusplus
extern "C"
{
#endif

extern TRANSPORT_HANDLE		IoTHubTransport_Create(IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol, const char* iotHubName, const char* iotHubSuffix);
extern void					IoTHubTransport_Destroy(TRANSPORT_HANDLE transportHandle);
extern LOCK_HANDLE			IoTHubTransport_GetLock(TRANSPORT_HANDLE transportHandle);
extern TRANSPORT_LL_HANDLE	IoTHubTransport_GetLLTransport(TRANSPORT_HANDLE transportHandle);
extern IOTHUB_CLIENT_RESULT IoTHubTransport_StartWorkerThread(TRANSPORT_HANDLE transportHandle, IOTHUB_CLIENT_HANDLE clientHandle);
extern bool					IoTHubTransport_SignalEndWorkerThread(TRANSPORT_HANDLE transportHandle, IOTHUB_CLIENT_HANDLE clientHandle);
extern void					IoTHubTransport_JoinWorkerThread(TRANSPORT_HANDLE transportHandle, IOTHUB_CLIENT_HANDLE clientHandle);

#ifdef __cplusplus
}
#endif

#endif /* IOTHUB_TRANSPORT_H */
