// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef IOTHUBTRANSPORTMQTT_H
#define IOTHUBTRANSPORTMQTT_H

#include "iothub_client_private.h"

#ifdef __cplusplus
extern "C"
{
#endif
    extern TRANSPORT_HANDLE IoTHubTransportMqtt_Create(const IOTHUBTRANSPORT_CONFIG* config);
    extern void IoTHubTransportMqtt_Destroy(TRANSPORT_HANDLE handle);

	extern IOTHUB_DEVICE_HANDLE IoTHubTransportMqtt_Register(TRANSPORT_HANDLE handle, const char* deviceId, const char* deviceKey, PDLIST_ENTRY waitingToSend);
	extern void IoTHubTransportMqtt_Unregister(IOTHUB_DEVICE_HANDLE deviceHandle);

    extern int IoTHubTransportMqtt_Subscribe(IOTHUB_DEVICE_HANDLE handle);
    extern void IoTHubTransportMqtt_Unsubscribe(IOTHUB_DEVICE_HANDLE handle);

    extern void IoTHubTransportMqtt_DoWork(TRANSPORT_HANDLE handle, IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle);

    extern IOTHUB_CLIENT_RESULT IoTHubTransportMqtt_GetSendStatus(IOTHUB_DEVICE_HANDLE handle, IOTHUB_CLIENT_STATUS *iotHubClientStatus);
    extern IOTHUB_CLIENT_RESULT IoTHubTransportMqtt_SetOption(TRANSPORT_HANDLE handle, const char* optionName, const void* value);
    extern const void* MQTT_Protocol(void);

#ifdef __cplusplus
}
#endif

#endif /*IOTHUBTRANSPORTMQTT_H*/
