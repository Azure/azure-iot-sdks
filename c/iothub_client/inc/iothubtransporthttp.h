// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef IOTHUBTRANSPORTHTTP_H
#define IOTHUBTRANSPORTHTTP_H

#include "iothub_client_private.h"

#ifdef __cplusplus
extern "C"
{
#endif

    extern TRANSPORT_HANDLE IoTHubTransportHttp_Create(const IOTHUBTRANSPORT_CONFIG* config);
    extern void IoTHubTransportHttp_Destroy(TRANSPORT_HANDLE handle);

    extern int IoTHubTransportHttp_Subscribe(TRANSPORT_HANDLE handle);
    extern void IoTHubTransportHttp_Unsubscribe(TRANSPORT_HANDLE handle);

    extern void IoTHubTransportHttp_DoWork(TRANSPORT_HANDLE handle, IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle);

    extern IOTHUB_CLIENT_RESULT IoTHubTransportHttp_GetSendStatus(TRANSPORT_HANDLE handle, IOTHUB_CLIENT_STATUS *iotHubClientStatus);
    extern IOTHUB_CLIENT_RESULT IoTHubTransportHttp_SetOption(TRANSPORT_HANDLE handle, const char* optionName, const void* value);
    extern const void* HTTP_Protocol(void);

#ifdef __cplusplus
}
#endif

#endif /*IOTHUBTRANSPORTHTTP_H*/
