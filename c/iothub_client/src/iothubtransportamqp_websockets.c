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

static TRANSPORT_PROVIDER thisTransportProvider_WebSocketsOverTls = {
	IoTHubTransportAMQP_GetHostname,
	IoTHubTransportAMQP_SetOption,
	IoTHubTransportAMQP_Create_WebSocketsOverTls,
	IoTHubTransportAMQP_Destroy,
	IoTHubTransportAMQP_Register,
	IoTHubTransportAMQP_Unregister,
	IoTHubTransportAMQP_Subscribe,
	IoTHubTransportAMQP_Unsubscribe,
	IoTHubTransportAMQP_DoWork,
	IoTHubTransportAMQP_GetSendStatus
};

extern const TRANSPORT_PROVIDER* AMQP_Protocol_over_WebSocketsTls(void)
{
	return &thisTransportProvider_WebSocketsOverTls;
}
