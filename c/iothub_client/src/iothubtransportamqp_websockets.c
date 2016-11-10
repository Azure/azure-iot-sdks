// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "iothubtransportamqp_websockets.h"
#include "azure_c_shared_utility/wsio.h"
#include "iothubtransport_amqp_common.h"

#define DEFAULT_WS_PROTOCOL_NAME "AMQPWSB10"
#define DEFAULT_WS_RELATIVE_PATH "/$iothub/websocket"
#define DEFAULT_WS_PORT 443

static XIO_HANDLE getWebSocketsIOTransport(const char* fqdn)
{
    WSIO_CONFIG ws_io_config;
    ws_io_config.host = fqdn;
    ws_io_config.port = DEFAULT_WS_PORT;
    ws_io_config.protocol_name = DEFAULT_WS_PROTOCOL_NAME;
    ws_io_config.relative_path = DEFAULT_WS_RELATIVE_PATH;
    ws_io_config.use_ssl = true;

	// Codes_SRS_IoTHubTransportAMQP_WS_09_002: [getWebSocketsIOTransport shall get `io_interface_description` using wsio_get_interface_description()]
	// Codes_SRS_IoTHubTransportAMQP_WS_09_003: [If `io_interface_description` is NULL getWebSocketsIOTransport shall return NULL.]
	// Codes_SRS_IoTHubTransportAMQP_WS_09_004: [getWebSocketsIOTransport shall return the XIO_HANDLE created using xio_create().]
    return xio_create(wsio_get_interface_description(), &ws_io_config);
}

// API functions
static TRANSPORT_LL_HANDLE IoTHubTransportAMQP_WS_Create(const IOTHUBTRANSPORT_CONFIG* config)
{
	// Codes_SRS_IoTHubTransportAMQP_WS_09_001: [IoTHubTransportAMQP_WS_Create shall create a TRANSPORT_LL_HANDLE by calling into the IoTHubTransport_AMQP_Common_Create function, passing `config` and getWebSocketsIOTransport.]
	return IoTHubTransport_AMQP_Common_Create(config, getWebSocketsIOTransport);
}

static IOTHUB_PROCESS_ITEM_RESULT IoTHubTransportAMQP_WS_ProcessItem(TRANSPORT_LL_HANDLE handle, IOTHUB_IDENTITY_TYPE item_type, IOTHUB_IDENTITY_INFO* iothub_item)
{
	// Codes_SRS_IoTHubTransportAMQP_WS_09_014: [IoTHubTransportAMQP_WS_ProcessItem shall invoke IoTHubTransport_AMQP_Common_ProcessItem() and return its result.]
	return IoTHubTransport_AMQP_Common_ProcessItem(handle, item_type, iothub_item);
}

static void IoTHubTransportAMQP_WS_DoWork(TRANSPORT_LL_HANDLE handle, IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle)
{
	// Codes_SRS_IoTHubTransportAMQP_WS_09_015: [IoTHubTransportAMQP_WS_DoWork shall call into the IoTHubTransport_AMQP_Common_DoWork()]
	IoTHubTransport_AMQP_Common_DoWork(handle, iotHubClientHandle);
}

static int IoTHubTransportAMQP_WS_Subscribe(IOTHUB_DEVICE_HANDLE handle)
{
	// Codes_SRS_IoTHubTransportAMQP_WS_09_012: [IoTHubTransportAMQP_WS_Subscribe shall subscribe for D2C messages by calling into the IoTHubTransport_AMQP_Common_Subscribe().]
	return IoTHubTransport_AMQP_Common_Subscribe(handle);
}

static void IoTHubTransportAMQP_WS_Unsubscribe(IOTHUB_DEVICE_HANDLE handle)
{
	// Codes_SRS_IoTHubTransportAMQP_WS_09_013: [IoTHubTransportAMQP_WS_Unsubscribe shall subscribe for D2C messages by calling into the IoTHubTransport_AMQP_Common_Unsubscribe().]
	IoTHubTransport_AMQP_Common_Unsubscribe(handle);
}

static int IoTHubTransportAMQP_WS_Subscribe_DeviceTwin(IOTHUB_DEVICE_HANDLE handle)
{
	// Codes_SRS_IoTHubTransportAMQP_WS_09_008: [IoTHubTransportAMQP_WS_Subscribe_DeviceTwin shall invoke IoTHubTransport_AMQP_Common_Subscribe_DeviceTwin() and return its result.]
	return IoTHubTransport_AMQP_Common_Subscribe_DeviceTwin(handle);
}

static void IoTHubTransportAMQP_WS_Unsubscribe_DeviceTwin(IOTHUB_DEVICE_HANDLE handle)
{
	// Codes_SRS_IoTHubTransportAMQP_WS_09_009: [IoTHubTransportAMQP_WS_Unsubscribe_DeviceTwin shall invoke IoTHubTransport_AMQP_Common_Unsubscribe_DeviceTwin()]
	IoTHubTransport_AMQP_Common_Unsubscribe_DeviceTwin(handle);
}

static int IoTHubTransportAMQP_WS_Subscribe_DeviceMethod(IOTHUB_DEVICE_HANDLE handle)
{
	// Codes_SRS_IoTHubTransportAMQP_WS_09_010: [IoTHubTransportAMQP_WS_Subscribe_DeviceMethod shall invoke IoTHubTransport_AMQP_Common_Subscribe_DeviceMethod() and return its result.]
	return IoTHubTransport_AMQP_Common_Subscribe_DeviceMethod(handle);
}

static void IoTHubTransportAMQP_WS_Unsubscribe_DeviceMethod(IOTHUB_DEVICE_HANDLE handle)
{
	// Codes_SRS_IoTHubTransportAMQP_WS_09_011: [IoTHubTransportAMQP_WS_Unsubscribe_DeviceMethod shall invoke IoTHubTransport_AMQP_Common_Unsubscribe_DeviceMethod()]
	IoTHubTransport_AMQP_Common_Unsubscribe_DeviceMethod(handle);
}

static IOTHUB_CLIENT_RESULT IoTHubTransportAMQP_WS_GetSendStatus(IOTHUB_DEVICE_HANDLE handle, IOTHUB_CLIENT_STATUS *iotHubClientStatus)
{
	// Codes_SRS_IoTHubTransportAMQP_WS_09_016: [IoTHubTransportAMQP_WS_GetSendStatus shall get the send status by calling into the IoTHubTransport_AMQP_Common_GetSendStatus()]
	return IoTHubTransport_AMQP_Common_GetSendStatus(handle, iotHubClientStatus);
}

static IOTHUB_CLIENT_RESULT IoTHubTransportAMQP_WS_SetOption(TRANSPORT_LL_HANDLE handle, const char* option, const void* value)
{
	// Codes_SRS_IoTHubTransportAMQP_WS_09_017: [IoTHubTransportAMQP_WS_SetOption shall set the options by calling into the IoTHubTransport_AMQP_Common_SetOption()]
	return IoTHubTransport_AMQP_Common_SetOption(handle, option, value);
}

static IOTHUB_DEVICE_HANDLE IoTHubTransportAMQP_WS_Register(TRANSPORT_LL_HANDLE handle, const IOTHUB_DEVICE_CONFIG* device, IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle, PDLIST_ENTRY waitingToSend)
{
	// Codes_SRS_IoTHubTransportAMQP_WS_09_006: [IoTHubTransportAMQP_WS_Register shall register the device by calling into the IoTHubTransport_AMQP_Common_Register().]
	return IoTHubTransport_AMQP_Common_Register(handle, device, iotHubClientHandle, waitingToSend);
}

static void IoTHubTransportAMQP_WS_Unregister(IOTHUB_DEVICE_HANDLE deviceHandle)
{
	// Codes_SRS_IoTHubTransportAMQP_WS_09_007: [IoTHubTransportAMQP_WS_Unregister shall unregister the device by calling into the IoTHubTransport_AMQP_Common_Unregister().]
	IoTHubTransport_AMQP_Common_Unregister(deviceHandle);
}

static void IoTHubTransportAMQP_WS_Destroy(TRANSPORT_LL_HANDLE handle)
{
	// Codes_SRS_IoTHubTransportAMQP_WS_09_005: [IoTHubTransportAMQP_WS_Destroy shall destroy the TRANSPORT_LL_HANDLE by calling into the IoTHubTransport_AMQP_Common_Destroy().]
	IoTHubTransport_AMQP_Common_Destroy(handle);
}

static int IoTHubTransportAMQP_WS_SetRetryPolicy(TRANSPORT_LL_HANDLE handle, IOTHUB_CLIENT_RETRY_POLICY retryPolicy, size_t retryTimeoutLimitInSeconds)
{
	return IoTHubTransport_AMQP_Common_SetRetryPolicy(handle, retryPolicy, retryTimeoutLimitInSeconds);
}

static STRING_HANDLE IoTHubTransportAMQP_WS_GetHostname(TRANSPORT_LL_HANDLE handle)
{
	// Codes_SRS_IoTHubTransportAMQP_WS_09_018: [IoTHubTransportAMQP_WS_GetHostname shall get the hostname by calling into the IoTHubTransport_AMQP_Common_GetHostname()]
	return IoTHubTransport_AMQP_Common_GetHostname(handle);
}

static TRANSPORT_PROVIDER thisTransportProvider_WebSocketsOverTls = 
{
    IoTHubTransportAMQP_WS_Subscribe_DeviceMethod,                     /*pfIoTHubTransport_Subscribe_DeviceMethod IoTHubTransport_Subscribe_DeviceMethod;*/
    IoTHubTransportAMQP_WS_Unsubscribe_DeviceMethod,                   /*pfIoTHubTransport_Unsubscribe_DeviceMethod IoTHubTransport_Unsubscribe_DeviceMethod;*/
    IoTHubTransportAMQP_WS_Subscribe_DeviceTwin,                       /*pfIoTHubTransport_Subscribe_DeviceTwin IoTHubTransport_Subscribe_DeviceTwin;*/
    IoTHubTransportAMQP_WS_Unsubscribe_DeviceTwin,                     /*pfIoTHubTransport_Unsubscribe_DeviceTwin IoTHubTransport_Unsubscribe_DeviceTwin;*/
    IoTHubTransportAMQP_WS_ProcessItem,                                /*pfIoTHubTransport_ProcessItem IoTHubTransport_ProcessItem;*/
    IoTHubTransportAMQP_WS_GetHostname,                                /*pfIoTHubTransport_GetHostname IoTHubTransport_GetHostname;*/
    IoTHubTransportAMQP_WS_SetOption,                                  /*pfIoTHubTransport_SetOption IoTHubTransport_SetOption;*/
    IoTHubTransportAMQP_WS_Create,                                     /*pfIoTHubTransport_Create IoTHubTransport_Create;*/
    IoTHubTransportAMQP_WS_Destroy,                                    /*pfIoTHubTransport_Destroy IoTHubTransport_Destroy;*/
    IoTHubTransportAMQP_WS_Register,                                   /*pfIotHubTransport_Register IoTHubTransport_Register;*/
    IoTHubTransportAMQP_WS_Unregister,                                 /*pfIotHubTransport_Unregister IoTHubTransport_Unegister;*/
    IoTHubTransportAMQP_WS_Subscribe,                                  /*pfIoTHubTransport_Subscribe IoTHubTransport_Subscribe;*/
    IoTHubTransportAMQP_WS_Unsubscribe,                                /*pfIoTHubTransport_Unsubscribe IoTHubTransport_Unsubscribe;*/
    IoTHubTransportAMQP_WS_DoWork,                                     /*pfIoTHubTransport_DoWork IoTHubTransport_DoWork;*/
    IoTHubTransportAMQP_WS_SetRetryPolicy,                             /*pfIoTHubTransport_SetRetryLogic IoTHubTransport_SetRetryPolicy;*/
    IoTHubTransportAMQP_WS_GetSendStatus                               /*pfIoTHubTransport_GetSendStatus IoTHubTransport_GetSendStatus;*/
};

/* Codes_SRS_IoTHubTransportAMQP_WS_09_019: [This function shall return a pointer to a structure of type TRANSPORT_PROVIDER having the following values for it's fields:
IoTHubTransport_Subscribe_DeviceMethod = IoTHubTransportAMQP_WS_Subscribe_DeviceMethod
IoTHubTransport_Unsubscribe_DeviceMethod = IoTHubTransportAMQP_WS_Unsubscribe_DeviceMethod
IoTHubTransport_Subscribe_DeviceTwin = IoTHubTransportAMQP_WS_Subscribe_DeviceTwin
IoTHubTransport_Unsubscribe_DeviceTwin = IoTHubTransportAMQP_WS_Unsubscribe_DeviceTwin
IoTHubTransport_ProcessItem - IoTHubTransportAMQP_WS_ProcessItem
IoTHubTransport_GetHostname = IoTHubTransportAMQP_WS_GetHostname
IoTHubTransport_Create = IoTHubTransportAMQP_WS_Create
IoTHubTransport_Destroy = IoTHubTransportAMQP_WS_Destroy
IoTHubTransport_Register = IoTHubTransportAMQP_WS_Register
IoTHubTransport_Unregister = IoTHubTransportAMQP_WS_Unregister
IoTHubTransport_Subscribe = IoTHubTransportAMQP_WS_Subscribe
IoTHubTransport_Unsubscribe = IoTHubTransportAMQP_WS_Unsubscribe
IoTHubTransport_DoWork = IoTHubTransportAMQP_WS_DoWork
IoTHubTransport_SetRetryLogic = IoTHubTransportAMQP_WS_SetRetryLogic
IoTHubTransport_SetOption = IoTHubTransportAMQP_WS_SetOption
IoTHubTransport_GetSendStatus = IoTHubTransportAMQP_WS_GetSendStatus] */
extern const TRANSPORT_PROVIDER* AMQP_Protocol_over_WebSocketsTls(void)
{
    return &thisTransportProvider_WebSocketsOverTls;
}
