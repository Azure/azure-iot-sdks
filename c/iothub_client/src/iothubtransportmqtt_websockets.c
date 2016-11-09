// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "azure_c_shared_utility/xio.h"
#include "azure_c_shared_utility/wsio.h"
#include "iothubtransportmqtt_websockets.h"
#include "iothubtransport_mqtt_common.h"

static XIO_HANDLE getWebSocketsIOTransport(const char* fully_qualified_name)
{
    XIO_HANDLE result;
    const IO_INTERFACE_DESCRIPTION* io_interface_description = wsio_get_interface_description();
    if (io_interface_description == NULL)
    {
        /* Codes_SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_07_013: [ If platform_get_default_tlsio returns NULL getIoTransportProvider shall return NULL. ] */
        LogError("Failure constructing the provider interface");
        result = NULL;
    }
    else
    {
        /* Codes_SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_07_012: [ getIoTransportProvider shall return the XIO_HANDLE returns by xio_create. ] */
        WSIO_CONFIG ws_io_config;
        ws_io_config.host = fully_qualified_name;
        ws_io_config.port = 443;
        ws_io_config.protocol_name = "MQTT";
        ws_io_config.relative_path = "/$iothub/websocket";
        ws_io_config.use_ssl = true;
        result = xio_create(io_interface_description, &ws_io_config);
    }
    return result;
}

/* Codes_SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_07_001: [ IoTHubTransportMqtt_WS_Create shall create a TRANSPORT_LL_HANDLE by calling into the IoTHubMqttAbstract_Create function. ] */
static TRANSPORT_LL_HANDLE IoTHubTransportMqtt_WS_Create(const IOTHUBTRANSPORT_CONFIG* config)
{
    return IoTHubTransport_MQTT_Common_Create(config, getWebSocketsIOTransport);
}

/* Codes_SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_07_002: [ IoTHubTransportMqtt_WS_Destroy shall destroy the TRANSPORT_LL_HANDLE by calling into the IoTHubMqttAbstract_Destroy function. ] */
static void IoTHubTransportMqtt_WS_Destroy(TRANSPORT_LL_HANDLE handle)
{
    IoTHubTransport_MQTT_Common_Destroy(handle);
}

/* Codes_SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_07_005: [ IoTHubTransportMqtt_WS_Subscribe shall subscribe the TRANSPORT_LL_HANDLE by calling into the IoTHubMqttAbstract_Subscribe function. ] */
static int IoTHubTransportMqtt_WS_Subscribe(IOTHUB_DEVICE_HANDLE handle)
{
    return IoTHubTransport_MQTT_Common_Subscribe(handle);
}

/* Codes_SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_07_006: [ IoTHubTransportMqtt_WS_Unsubscribe shall unsubscribe the TRANSPORT_LL_HANDLE by calling into the IoTHubMqttAbstract_Unsubscribe function. ] */
static void IoTHubTransportMqtt_WS_Unsubscribe(IOTHUB_DEVICE_HANDLE handle)
{
    IoTHubTransport_MQTT_Common_Unsubscribe(handle);
}

/* Codes_SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_07_015: [ IoTHubTransportMqtt_WS_Subscribe_DeviceMethod shall call into the IoTHubTransport_MQTT_Common_Subscribe_DeviceMethod function ] */
static int IoTHubTransportMqtt_WS_Subscribe_DeviceMethod(IOTHUB_DEVICE_HANDLE handle)
{
    return IoTHubTransport_MQTT_Common_Subscribe_DeviceMethod(handle);
}

/* Codes_SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_07_016: [ IoTHubTransportMqtt_WS_Unsubscribe_DeviceMethod shall call into the IoTHubTransport_MQTT_Common_Unsubscribe_DeviceMethod ] */
static void IoTHubTransportMqtt_WS_Unsubscribe_DeviceMethod(IOTHUB_DEVICE_HANDLE handle)
{
    IoTHubTransport_MQTT_Common_Unsubscribe_DeviceTwin(handle);
}

/* Codes_SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_07_017: [ IoTHubTransportMqtt_WS_Subscribe_DeviceTwin shall call into the IoTHubTransport_MQTT_Common_Subscribe_DeviceTwin ] */
static int IoTHubTransportMqtt_WS_Subscribe_DeviceTwin(IOTHUB_DEVICE_HANDLE handle)
{
    return IoTHubTransport_MQTT_Common_Subscribe_DeviceMethod(handle);
}

/* Codes_SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_07_018: [ IoTHubTransportMqtt_WS_Unsubscribe_DeviceTwin shall call into the IoTHubTransport_MQTT_Common_Unsubscribe_DeviceTwin ] */
static void IoTHubTransportMqtt_WS_Unsubscribe_DeviceTwin(IOTHUB_DEVICE_HANDLE handle)
{
    IoTHubTransport_MQTT_Common_Unsubscribe_DeviceMethod(handle);
}

/* Codes_SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_07_014: [ IoTHubTransportMqtt_WS_ProcessItem shall call into the IoTHubTransport_MQTT_Common_DoWork function ] */
static IOTHUB_PROCESS_ITEM_RESULT IoTHubTransportMqtt_WS_ProcessItem(TRANSPORT_LL_HANDLE handle, IOTHUB_IDENTITY_TYPE item_type, IOTHUB_IDENTITY_INFO* iothub_item)
{
    return IoTHubTransport_MQTT_Common_ProcessItem(handle, item_type, iothub_item);
}

/* Codes_SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_07_007: [ IoTHubTransportMqtt_WS_DoWork shall call into the IoTHubMqttAbstract_DoWork function. ] */
static void IoTHubTransportMqtt_WS_DoWork(TRANSPORT_LL_HANDLE handle, IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle)
{
    IoTHubTransport_MQTT_Common_DoWork(handle, iotHubClientHandle);
}

/* Codes_SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_07_008: [ IoTHubTransportMqtt_WS_GetSendStatus shall get the send status by calling into the IoTHubMqttAbstract_GetSendStatus function. ] */
static IOTHUB_CLIENT_RESULT IoTHubTransportMqtt_WS_GetSendStatus(IOTHUB_DEVICE_HANDLE handle, IOTHUB_CLIENT_STATUS *iotHubClientStatus)
{
    return IoTHubTransport_MQTT_Common_GetSendStatus(handle, iotHubClientStatus);
}

/* Codes_SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_07_009: [ IoTHubTransportMqtt_WS_SetOption shall set the options by calling into the IoTHubMqttAbstract_SetOption function. ] */
static IOTHUB_CLIENT_RESULT IoTHubTransportMqtt_WS_SetOption(TRANSPORT_LL_HANDLE handle, const char* option, const void* value)
{
    return IoTHubTransport_MQTT_Common_SetOption(handle, option, value);
}

/* Codes_SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_07_003: [ IoTHubTransportMqtt_WS_Register shall register the TRANSPORT_LL_HANDLE by calling into the IoTHubMqttAbstract_Register function. ]*/
static IOTHUB_DEVICE_HANDLE IoTHubTransportMqtt_WS_Register(TRANSPORT_LL_HANDLE handle, const IOTHUB_DEVICE_CONFIG* device, IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle, PDLIST_ENTRY waitingToSend)
{
    return IoTHubTransport_MQTT_Common_Register(handle, device, iotHubClientHandle, waitingToSend);
}

/* Codes_SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_07_004: [ IoTHubTransportMqtt_WS_Unregister shall register the TRANSPORT_LL_HANDLE by calling into the IoTHubMqttAbstract_Unregister function. ] */
static void IoTHubTransportMqtt_WS_Unregister(IOTHUB_DEVICE_HANDLE deviceHandle)
{
    IoTHubTransport_MQTT_Common_Unregister(deviceHandle);
}

/* Codes_SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_07_010: [ IoTHubTransportMqtt_WS_GetHostname shall get the hostname by calling into the IoTHubMqttAbstract_GetHostname function. ] */
static STRING_HANDLE IoTHubTransportMqtt_WS_GetHostname(TRANSPORT_LL_HANDLE handle)
{
    return IoTHubTransport_MQTT_Common_GetHostname(handle);
}

static int IoTHubTransportMqtt_WS_SetRetryPolicy(TRANSPORT_LL_HANDLE handle, IOTHUB_CLIENT_RETRY_POLICY retryPolicy, size_t retryTimeoutLimitinSeconds)
{
    /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_25_012: [** IoTHubTransportMqtt_WS_SetRetryPolicy shall call into the IoTHubMqttAbstract_SetRetryPolicy function.]*/
    return IoTHubTransport_MQTT_Common_SetRetryPolicy(handle, retryPolicy, retryTimeoutLimitinSeconds);
}

/* Codes_SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_07_011: [ This function shall return a pointer to a structure of type TRANSPORT_PROVIDER having the following values for its fields:
IoTHubTransport_Subscribe_DeviceMethod = IoTHubTransport_WS_Subscribe_DeviceMethod
IoTHubTransport_Unsubscribe_DeviceMethod IoTHubTransport_WS_Unsubscribe_DeviceMethod
IoTHubTransport_Subscribe_DeviceTwin IoTHubTransport_WS_Subscribe_DeviceTwin
IoTHubTransport_Unsubscribe_DeviceTwin IoTHubTransport_WS_Unsubscribe_DeviceTwin
IoTHubTransport_GetHostname = IoTHubTransportMqtt_WS_GetHostname
IoTHubTransport_Create = IoTHubTransportMqtt_WS_Create
IoTHubTransport_Destroy = IoTHubTransportMqtt_WS_Destroy
IoTHubTransport_Subscribe = IoTHubTransportMqtt_WS_Subscribe
IoTHubTransport_Unsubscribe = IoTHubTransportMqtt_WS_Unsubscribe
IoTHubTransport_DoWork = IoTHubTransportMqtt_WS_DoWork
IoTHubTransport_SetOption = IoTHubTransportMqtt_WS_SetOption ] */
static TRANSPORT_PROVIDER thisTransportProvider_WebSocketsOverTls = {
    IoTHubTransportMqtt_WS_Subscribe_DeviceMethod,
    IoTHubTransportMqtt_WS_Unsubscribe_DeviceMethod,
    IoTHubTransportMqtt_WS_Subscribe_DeviceTwin,
    IoTHubTransportMqtt_WS_Unsubscribe_DeviceTwin,
    IoTHubTransportMqtt_WS_ProcessItem,
    IoTHubTransportMqtt_WS_GetHostname,
    IoTHubTransportMqtt_WS_SetOption,
    IoTHubTransportMqtt_WS_Create,
    IoTHubTransportMqtt_WS_Destroy,
    IoTHubTransportMqtt_WS_Register,
    IoTHubTransportMqtt_WS_Unregister,
    IoTHubTransportMqtt_WS_Subscribe,
    IoTHubTransportMqtt_WS_Unsubscribe,
    IoTHubTransportMqtt_WS_DoWork,
    IoTHubTransportMqtt_WS_SetRetryPolicy,
    IoTHubTransportMqtt_WS_GetSendStatus
};

const TRANSPORT_PROVIDER* MQTT_WebSocket_Protocol(void)
{
    return &thisTransportProvider_WebSocketsOverTls;
}
