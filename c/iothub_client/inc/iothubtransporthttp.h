// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef IOTHUBTRANSPORTHTTP_H
#define IOTHUBTRANSPORTHTTP_H

#include "iothub_client_ll.h"
#include "iothub_transport_ll.h"

#ifdef __cplusplus
extern "C"
{
#endif

	extern TRANSPORT_LL_HANDLE IoTHubTransportHttp_Create(const IOTHUBTRANSPORT_CONFIG* config);
	extern void IoTHubTransportHttp_Destroy(TRANSPORT_LL_HANDLE handle);

	extern IOTHUB_DEVICE_HANDLE IoTHubTransportHttp_Register(TRANSPORT_LL_HANDLE handle, const IOTHUB_DEVICE_CONFIG* device, IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle, PDLIST_ENTRY waitingToSend);
	extern void IoTHubTransportHttp_Unregister(IOTHUB_DEVICE_HANDLE deviceHandle);

	extern int IoTHubTransportHttp_Subscribe(IOTHUB_DEVICE_HANDLE handle);
	extern void IoTHubTransportHttp_Unsubscribe(IOTHUB_DEVICE_HANDLE handle);

	extern void IoTHubTransportHttp_DoWork(TRANSPORT_LL_HANDLE handle, IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle);

	extern IOTHUB_CLIENT_RESULT IoTHubTransportHttp_GetSendStatus(IOTHUB_DEVICE_HANDLE handle, IOTHUB_CLIENT_STATUS *iotHubClientStatus);
	extern IOTHUB_CLIENT_RESULT IoTHubTransportHttp_SetOption(TRANSPORT_LL_HANDLE handle, const char* optionName, const void* value);
	extern const void* HTTP_Protocol(void);

#ifdef __cplusplus
}
#endif

#endif /*IOTHUBTRANSPORTHTTP_H*/
