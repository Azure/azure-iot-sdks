// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "iothubtransportamqp_websockets.h"
#include "azure_c_shared_utility/wsio.h"
#include "iothubtransportamqp.c"

#define DEFAULT_WS_PROTOCOL_NAME "AMQPWSB10"
#define DEFAULT_WS_RELATIVE_PATH "/$iothub/websocket"
#define DEFAULT_WS_PORT 443

XIO_HANDLE getWebSocketsIOTransport(const char* fqdn, int port)
{
    WSIO_CONFIG ws_io_config;
    ws_io_config.host = fqdn;
    ws_io_config.port = port;
    ws_io_config.protocol_name = DEFAULT_WS_PROTOCOL_NAME;
    ws_io_config.relative_path = DEFAULT_WS_RELATIVE_PATH;
    ws_io_config.use_ssl = true;

    return xio_create(wsio_get_interface_description(), &ws_io_config);
}
static TRANSPORT_LL_HANDLE IoTHubTransportAMQP_Create_WebSocketsOverTls(const IOTHUBTRANSPORT_CONFIG* config)
{
    AMQP_TRANSPORT_INSTANCE* transport_state = (AMQP_TRANSPORT_INSTANCE*)IoTHubTransportAMQP_Create(config);

    if (transport_state != NULL)
    {
        transport_state->tls_io_transport_provider = getWebSocketsIOTransport;
        transport_state->iotHubPort = DEFAULT_WS_PORT;
    }

    return transport_state;
}

static  int IoTHubTransportAMQP_Subscribe_DeviceTwin_WebSocketsOverTls(IOTHUB_DEVICE_HANDLE handle)
{
    (void)(handle);
    LogError("not implemented yet");
    return __LINE__;
}

static  void IoTHubTransportAMQP_Unsubscribe_DeviceTwin_WebSocketsOverTls(IOTHUB_DEVICE_HANDLE handle)
{
    (void)(handle);
    LogError("not implemented yet");
}

static int IoTHubTransportAMQP_Subscribe_DeviceMethod_WebSocketsOverTls(IOTHUB_DEVICE_HANDLE handle)
{
    (void)handle;
    int result = __LINE__;
    LogError("not implemented (yet)");
    return result;
}

static void IoTHubTransportAMQP_Unsubscribe_DeviceMethod_WebSocketsOverTls(IOTHUB_DEVICE_HANDLE handle)
{
    (void)handle;
    LogError("not implemented (yet)");
}

static IOTHUB_PROCESS_ITEM_RESULT IoTHubTransportAMQP_ProcessItem_WebSocketsOverTls(TRANSPORT_LL_HANDLE handle, IOTHUB_IDENTITY_TYPE item_type, IOTHUB_IDENTITY_INFO* iothub_item)
{
    (void)handle;
    (void)item_type;
    (void)iothub_item;
    LogError("Currently Not Supported.");
    return IOTHUB_CLIENT_ERROR;
}

static TRANSPORT_PROVIDER thisTransportProvider_WebSocketsOverTls = 
{
    IoTHubTransportAMQP_Subscribe_DeviceMethod_WebSocketsOverTls,   /*pfIoTHubTransport_Subscribe_DeviceMethod IoTHubTransport_Subscribe_DeviceMethod;*/
    IoTHubTransportAMQP_Unsubscribe_DeviceMethod_WebSocketsOverTls, /*pfIoTHubTransport_Unsubscribe_DeviceMethod IoTHubTransport_Unsubscribe_DeviceMethod;*/
    IoTHubTransportAMQP_Subscribe_DeviceTwin_WebSocketsOverTls,     /*pfIoTHubTransport_Subscribe_DeviceTwin IoTHubTransport_Subscribe_DeviceTwin;*/
    IoTHubTransportAMQP_Unsubscribe_DeviceTwin_WebSocketsOverTls,   /*pfIoTHubTransport_Unsubscribe_DeviceTwin IoTHubTransport_Unsubscribe_DeviceTwin;*/
    IoTHubTransportAMQP_ProcessItem_WebSocketsOverTls,              /*pfIoTHubTransport_ProcessItem IoTHubTransport_ProcessItem;*/
    IoTHubTransportAMQP_GetHostname,                                /*pfIoTHubTransport_GetHostname IoTHubTransport_GetHostname;*/
    IoTHubTransportAMQP_SetOption,                                  /*pfIoTHubTransport_SetOption IoTHubTransport_SetOption;*/
    IoTHubTransportAMQP_Create_WebSocketsOverTls,                   /*pfIoTHubTransport_Create IoTHubTransport_Create;*/
    IoTHubTransportAMQP_Destroy,                                    /*pfIoTHubTransport_Destroy IoTHubTransport_Destroy;*/
    IoTHubTransportAMQP_Register,                                   /*pfIotHubTransport_Register IoTHubTransport_Register;*/
    IoTHubTransportAMQP_Unregister,                                 /*pfIotHubTransport_Unregister IoTHubTransport_Unegister;*/
    IoTHubTransportAMQP_Subscribe,                                  /*pfIoTHubTransport_Subscribe IoTHubTransport_Subscribe;*/
    IoTHubTransportAMQP_Unsubscribe,                                /*pfIoTHubTransport_Unsubscribe IoTHubTransport_Unsubscribe;*/
    IoTHubTransportAMQP_DoWork,                                     /*pfIoTHubTransport_DoWork IoTHubTransport_DoWork;*/
    IoTHubTransportAMQP_SetRetryPolicy,                             /*pfIoTHubTransport_DoWork IoTHubTransport_SetRetryPolicy;*/
    IoTHubTransportAMQP_GetSendStatus                               /*pfIoTHubTransport_GetSendStatus IoTHubTransport_GetSendStatus;*/
};

extern const TRANSPORT_PROVIDER* AMQP_Protocol_over_WebSocketsTls(void)
{
    return &thisTransportProvider_WebSocketsOverTls;
}
