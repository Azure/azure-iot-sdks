// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "iothubtransportamqp.h" 
#include "iothubtransport_amqp_common.h"
#include "azure_c_shared_utility/tlsio.h"
#include "azure_c_shared_utility/platform.h"

#define RESULT_OK 0
#define DEFAULT_IOTHUB_AMQP_PORT 5671

static XIO_HANDLE getTLSIOTransport(const char* fqdn)
{
    XIO_HANDLE result;
    TLSIO_CONFIG tls_io_config;
    tls_io_config.hostname = fqdn;
    tls_io_config.port = DEFAULT_IOTHUB_AMQP_PORT;
	// Codes_SRS_IOTHUBTRANSPORTAMQP_09_002: [getTLSIOTransport shall get `io_interface_description` using platform_get_default_tlsio())]
    const IO_INTERFACE_DESCRIPTION* io_interface_description = platform_get_default_tlsio();

	// Codes_SRS_IOTHUBTRANSPORTAMQP_09_003: [If `io_interface_description` is NULL getTLSIOTransport shall return NULL.]
	// Codes_SRS_IOTHUBTRANSPORTAMQP_09_004: [getTLSIOTransport shall return the XIO_HANDLE created using xio_create().]
    if ((result = xio_create(io_interface_description, &tls_io_config)) == NULL)
    {
        LogError("Failed to get the TLS/IO transport (xio_create failed)");
    }

    return result;
}

// API functions
static TRANSPORT_LL_HANDLE IoTHubTransportAMQP_Create(const IOTHUBTRANSPORT_CONFIG* config)
{
	// Codes_SRS_IOTHUBTRANSPORTAMQP_09_001: [IoTHubTransportAMQP_Create shall create a TRANSPORT_LL_HANDLE by calling into the IoTHubTransport_AMQP_Common_Create function, passing `config` and getTLSIOTransport.]
	return IoTHubTransport_AMQP_Common_Create(config, getTLSIOTransport);
}

static IOTHUB_PROCESS_ITEM_RESULT IoTHubTransportAMQP_ProcessItem(TRANSPORT_LL_HANDLE handle, IOTHUB_IDENTITY_TYPE item_type, IOTHUB_IDENTITY_INFO* iothub_item)
{
	// Codes_SRS_IOTHUBTRANSPORTAMQP_09_014: [IoTHubTransportAMQP_ProcessItem shall invoke IoTHubTransport_AMQP_Common_ProcessItem() and return its result.]
	return IoTHubTransport_AMQP_Common_ProcessItem(handle, item_type, iothub_item);
}

static void IoTHubTransportAMQP_DoWork(TRANSPORT_LL_HANDLE handle, IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle)
{
	// Codes_SRS_IOTHUBTRANSPORTAMQP_09_015: [IoTHubTransportAMQP_DoWork shall call into the IoTHubTransport_AMQP_Common_DoWork()]
	IoTHubTransport_AMQP_Common_DoWork(handle, iotHubClientHandle);
}

static int IoTHubTransportAMQP_Subscribe(IOTHUB_DEVICE_HANDLE handle)
{
	// Codes_SRS_IOTHUBTRANSPORTAMQP_09_012: [IoTHubTransportAMQP_Subscribe shall subscribe for D2C messages by calling into the IoTHubTransport_AMQP_Common_Subscribe().]
	return IoTHubTransport_AMQP_Common_Subscribe(handle);
}

static void IoTHubTransportAMQP_Unsubscribe(IOTHUB_DEVICE_HANDLE handle)
{
	// Codes_SRS_IOTHUBTRANSPORTAMQP_09_013: [IoTHubTransportAMQP_Unsubscribe shall subscribe for D2C messages by calling into the IoTHubTransport_AMQP_Common_Unsubscribe().]
	IoTHubTransport_AMQP_Common_Unsubscribe(handle);
}

static int IoTHubTransportAMQP_Subscribe_DeviceTwin(IOTHUB_DEVICE_HANDLE handle)
{
	// Codes_SRS_IOTHUBTRANSPORTAMQP_09_008: [IoTHubTransportAMQP_Subscribe_DeviceTwin shall invoke IoTHubTransport_AMQP_Common_Subscribe_DeviceTwin() and return its result.]
	return IoTHubTransport_AMQP_Common_Subscribe_DeviceTwin(handle);
}

static void IoTHubTransportAMQP_Unsubscribe_DeviceTwin(IOTHUB_DEVICE_HANDLE handle)
{
	// Codes_SRS_IOTHUBTRANSPORTAMQP_09_009: [IoTHubTransportAMQP_Unsubscribe_DeviceTwin shall invoke IoTHubTransport_AMQP_Common_Unsubscribe_DeviceTwin()]
	IoTHubTransport_AMQP_Common_Unsubscribe_DeviceTwin(handle);
}

static int IoTHubTransportAMQP_Subscribe_DeviceMethod(IOTHUB_DEVICE_HANDLE handle)
{
	// Codes_SRS_IOTHUBTRANSPORTAMQP_09_010: [IoTHubTransportAMQP_Subscribe_DeviceMethod shall invoke IoTHubTransport_AMQP_Common_Subscribe_DeviceMethod() and return its result.]
	return IoTHubTransport_AMQP_Common_Subscribe_DeviceMethod(handle);
}

static void IoTHubTransportAMQP_Unsubscribe_DeviceMethod(IOTHUB_DEVICE_HANDLE handle)
{
	// Codes_SRS_IOTHUBTRANSPORTAMQP_09_011: [IoTHubTransportAMQP_Unsubscribe_DeviceMethod shall invoke IoTHubTransport_AMQP_Common_Unsubscribe_DeviceMethod()]
	IoTHubTransport_AMQP_Common_Unsubscribe_DeviceMethod(handle);
}

static IOTHUB_CLIENT_RESULT IoTHubTransportAMQP_GetSendStatus(IOTHUB_DEVICE_HANDLE handle, IOTHUB_CLIENT_STATUS *iotHubClientStatus)
{
	// Codes_SRS_IOTHUBTRANSPORTAMQP_09_016: [IoTHubTransportAMQP_GetSendStatus shall get the send status by calling into the IoTHubTransport_AMQP_Common_GetSendStatus()]
	return IoTHubTransport_AMQP_Common_GetSendStatus(handle, iotHubClientStatus);
}

static IOTHUB_CLIENT_RESULT IoTHubTransportAMQP_SetOption(TRANSPORT_LL_HANDLE handle, const char* option, const void* value)
{
	// Codes_SRS_IOTHUBTRANSPORTAMQP_09_017: [IoTHubTransportAMQP_SetOption shall set the options by calling into the IoTHubTransport_AMQP_Common_SetOption()]
	return IoTHubTransport_AMQP_Common_SetOption(handle, option, value);
}

static IOTHUB_DEVICE_HANDLE IoTHubTransportAMQP_Register(TRANSPORT_LL_HANDLE handle, const IOTHUB_DEVICE_CONFIG* device, IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle, PDLIST_ENTRY waitingToSend)
{
	// Codes_SRS_IOTHUBTRANSPORTAMQP_09_006: [IoTHubTransportAMQP_Register shall register the device by calling into the IoTHubTransport_AMQP_Common_Register().]
	return IoTHubTransport_AMQP_Common_Register(handle, device, iotHubClientHandle, waitingToSend);
}

static void IoTHubTransportAMQP_Unregister(IOTHUB_DEVICE_HANDLE deviceHandle)
{
	// Codes_SRS_IOTHUBTRANSPORTAMQP_09_007: [IoTHubTransportAMQP_Unregister shall unregister the device by calling into the IoTHubTransport_AMQP_Common_Unregister().]
	IoTHubTransport_AMQP_Common_Unregister(deviceHandle);
}

static void IoTHubTransportAMQP_Destroy(TRANSPORT_LL_HANDLE handle)
{
	// Codes_SRS_IOTHUBTRANSPORTAMQP_09_005: [IoTHubTransportAMQP_Destroy shall destroy the TRANSPORT_LL_HANDLE by calling into the IoTHubTransport_AMQP_Common_Destroy().]
	IoTHubTransport_AMQP_Common_Destroy(handle);
}

static int IoTHubTransportAMQP_SetRetryPolicy(TRANSPORT_LL_HANDLE handle, IOTHUB_CLIENT_RETRY_POLICY retryPolicy, size_t retryTimeoutLimitInSeconds)
{
	return IoTHubTransport_AMQP_Common_SetRetryPolicy(handle, retryPolicy, retryTimeoutLimitInSeconds);
}

static STRING_HANDLE IoTHubTransportAMQP_GetHostname(TRANSPORT_LL_HANDLE handle)
{
	// Codes_SRS_IOTHUBTRANSPORTAMQP_09_018: [IoTHubTransportAMQP_GetHostname shall get the hostname by calling into the IoTHubTransport_AMQP_Common_GetHostname()]
	return IoTHubTransport_AMQP_Common_GetHostname(handle);
}

static TRANSPORT_PROVIDER thisTransportProvider = 
{
    IoTHubTransportAMQP_Subscribe_DeviceMethod,     /*pfIoTHubTransport_Subscribe_DeviceMethod IoTHubTransport_Subscribe_DeviceMethod;*/
    IoTHubTransportAMQP_Unsubscribe_DeviceMethod,   /*pfIoTHubTransport_Unsubscribe_DeviceMethod IoTHubTransport_Unsubscribe_DeviceMethod;*/
    IoTHubTransportAMQP_Subscribe_DeviceTwin,       /*pfIoTHubTransport_Subscribe_DeviceTwin IoTHubTransport_Subscribe_DeviceTwin;*/
    IoTHubTransportAMQP_Unsubscribe_DeviceTwin,     /*pfIoTHubTransport_Unsubscribe_DeviceTwin IoTHubTransport_Unsubscribe_DeviceTwin;*/
    IoTHubTransportAMQP_ProcessItem,                /*pfIoTHubTransport_ProcessItem IoTHubTransport_ProcessItem;*/
    IoTHubTransportAMQP_GetHostname,                /*pfIoTHubTransport_GetHostname IoTHubTransport_GetHostname;*/
    IoTHubTransportAMQP_SetOption,                  /*pfIoTHubTransport_SetOption IoTHubTransport_SetOption;*/
    IoTHubTransportAMQP_Create,                     /*pfIoTHubTransport_Create IoTHubTransport_Create;*/
    IoTHubTransportAMQP_Destroy,                    /*pfIoTHubTransport_Destroy IoTHubTransport_Destroy;*/
    IoTHubTransportAMQP_Register,                   /*pfIotHubTransport_Register IoTHubTransport_Register;*/
    IoTHubTransportAMQP_Unregister,                 /*pfIotHubTransport_Unregister IoTHubTransport_Unegister;*/
    IoTHubTransportAMQP_Subscribe,                  /*pfIoTHubTransport_Subscribe IoTHubTransport_Subscribe;*/
    IoTHubTransportAMQP_Unsubscribe,                /*pfIoTHubTransport_Unsubscribe IoTHubTransport_Unsubscribe;*/
    IoTHubTransportAMQP_DoWork,                     /*pfIoTHubTransport_DoWork IoTHubTransport_DoWork;*/
    IoTHubTransportAMQP_SetRetryPolicy,             /*pfIoTHubTransport_DoWork IoTHubTransport_SetRetryPolicy;*/
    IoTHubTransportAMQP_GetSendStatus               /*pfIoTHubTransport_GetSendStatus IoTHubTransport_GetSendStatus;*/
};

/* Codes_SRS_IOTHUBTRANSPORTAMQP_09_019: [This function shall return a pointer to a structure of type TRANSPORT_PROVIDER having the following values for it's fields:
IoTHubTransport_Subscribe_DeviceMethod = IoTHubTransportAMQP_Subscribe_DeviceMethod
IoTHubTransport_Unsubscribe_DeviceMethod = IoTHubTransportAMQP_Unsubscribe_DeviceMethod
IoTHubTransport_Subscribe_DeviceTwin = IoTHubTransportAMQP_Subscribe_DeviceTwin
IoTHubTransport_Unsubscribe_DeviceTwin = IoTHubTransportAMQP_Unsubscribe_DeviceTwin
IoTHubTransport_ProcessItem - IoTHubTransportAMQP_ProcessItem
IoTHubTransport_GetHostname = IoTHubTransportAMQP_GetHostname
IoTHubTransport_Create = IoTHubTransportAMQP_Create
IoTHubTransport_Destroy = IoTHubTransportAMQP_Destroy
IoTHubTransport_Subscribe = IoTHubTransportAMQP_Subscribe
IoTHubTransport_Unsubscribe = IoTHubTransportAMQP_Unsubscribe
IoTHubTransport_DoWork = IoTHubTransportAMQP_DoWork
IoTHubTransport_SetOption = IoTHubTransportAMQP_SetOption]*/
extern const TRANSPORT_PROVIDER* AMQP_Protocol(void)
{
    return &thisTransportProvider;
}
