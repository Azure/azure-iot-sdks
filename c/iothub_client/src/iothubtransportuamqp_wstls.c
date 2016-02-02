// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "iothubtransportuamqp_wstls.h"
#include "wsio.h"
#include "iothubtransportuamqp.c"

#define DEFAULT_WS_PROTOCOL_NAME "AMQPWSB10"
#define DEFAULT_WS_RELATIVE_PATH "/$iothub/websocket"
#define DEFAULT_WS_PORT 443

XIO_HANDLE default_io_transport_provider_websockets_tls(const char* fqdn, int port, const char* certificates)
{
	WSIO_CONFIG ws_io_config = { fqdn, port, DEFAULT_WS_PROTOCOL_NAME, DEFAULT_WS_RELATIVE_PATH, true, certificates };

	return xio_create(wsio_get_interface_description(), &ws_io_config, NULL);
}
static TRANSPORT_HANDLE IoTHubTransportuAMQP_Create_WebSocketsOverTls(const IOTHUBTRANSPORT_CONFIG* config)
{
	UAMQP_TRANSPORT_INSTANCE* transport_state = (UAMQP_TRANSPORT_INSTANCE*)IoTHubTransportuAMQP_Create(config);

	if (transport_state != NULL)
	{
		transport_state->io_transport_provider_callback = default_io_transport_provider_websockets_tls;
		transport_state->iotHubPort = DEFAULT_WS_PORT;
	}

	return transport_state;
}

static TRANSPORT_PROVIDER thisTransportProvider_WebSocketsOverTls = {
	IoTHubTransportuAMQP_SetOption,
	IoTHubTransportuAMQP_Create_WebSocketsOverTls,
	IoTHubTransportuAMQP_Destroy,
	IoTHubTransportuAMQP_Subscribe,
	IoTHubTransportuAMQP_Unsubscribe,
	IoTHubTransportuAMQP_DoWork,
	IoTHubTransportuAMQP_GetSendStatus
};

extern const void* uAMQP_Protocol_over_WebSocketsTls(void)
{
	return &thisTransportProvider_WebSocketsOverTls;
}