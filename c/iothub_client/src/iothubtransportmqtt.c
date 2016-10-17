// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "iothubtransportmqtt.h"
#include "azure_c_shared_utility/xio.h"
#include "azure_c_shared_utility/tlsio.h"
#include "azure_c_shared_utility/platform.h"
#include "iothubtransport_mqtt_common.h"

static XIO_HANDLE getIoTransportProvider(const char* fqdn)
{
    XIO_HANDLE result;
    const IO_INTERFACE_DESCRIPTION* io_interface_description = platform_get_default_tlsio();
    if (io_interface_description == NULL)
    {
        /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_013: [ If platform_get_default_tlsio returns NULL getIoTransportProvider shall return NULL. ] */
        LogError("Failure constructing the provider interface");
        result = NULL;
    }
    else
    {
        TLSIO_CONFIG tls_io_config;
        tls_io_config.hostname = fqdn;
        tls_io_config.port = 8883;
        /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_012: [ getIoTransportProvider shall return the XIO_HANDLE returns by xio_create. ] */
        result = xio_create(io_interface_description, &tls_io_config);
    }
    return result;
}

static TRANSPORT_LL_HANDLE IoTHubTransportMqtt_Create(const IOTHUBTRANSPORT_CONFIG* config)
{
    /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_001: [IoTHubTransportMqtt_Create shall create a TRANSPORT_LL_HANDLE by calling into the IoTHubMqttAbstract_Create function.] */
    return IoTHubTransport_MQTT_Common_Create(config, getIoTransportProvider);
}

static void IoTHubTransportMqtt_Destroy(TRANSPORT_LL_HANDLE handle)
{
    /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_002: [ IoTHubTransportMqtt_Destroy shall destroy the TRANSPORT_LL_HANDLE by calling into the IoTHubMqttAbstract_Destroy function. ] */
    IoTHubTransport_MQTT_Common_Destroy(handle);
}

static int IoTHubTransportMqtt_Subscribe(IOTHUB_DEVICE_HANDLE handle)
{
    /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_005: [ IoTHubTransportMqtt_Subscribe shall subscribe the TRANSPORT_LL_HANDLE by calling into the IoTHubMqttAbstract_Subscribe function. ] */
    return IoTHubTransport_MQTT_Common_Subscribe(handle);
}

static void IoTHubTransportMqtt_Unsubscribe(IOTHUB_DEVICE_HANDLE handle)
{
    /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_006: [ IoTHubTransportMqtt_Unsubscribe shall unsubscribe the TRANSPORT_LL_HANDLE by calling into the IoTHubMqttAbstract_Unsubscribe function. ] */
    IoTHubTransport_MQTT_Common_Unsubscribe(handle);
}

static void IoTHubTransportMqtt_DoWork(TRANSPORT_LL_HANDLE handle, IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle)
{
    /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_007: [ IoTHubTransportMqtt_DoWork shall call into the IoTHubMqttAbstract_DoWork function. ] */
    IoTHubTransport_MQTT_Common_DoWork(handle, iotHubClientHandle);
}

static IOTHUB_CLIENT_RESULT IoTHubTransportMqtt_GetSendStatus(IOTHUB_DEVICE_HANDLE handle, IOTHUB_CLIENT_STATUS *iotHubClientStatus)
{
    /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_008: [ IoTHubTransportMqtt_GetSendStatus shall get the send status by calling into the IoTHubMqttAbstract_GetSendStatus function. ] */
    return IoTHubTransport_MQTT_Common_GetSendStatus(handle, iotHubClientStatus);
}

static IOTHUB_CLIENT_RESULT IoTHubTransportMqtt_SetOption(TRANSPORT_LL_HANDLE handle, const char* option, const void* value)
{
    /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_009: [ IoTHubTransportMqtt_SetOption shall set the options by calling into the IoTHubMqttAbstract_SetOption function. ] */
    return IoTHubTransport_MQTT_Common_SetOption(handle, option, value);
}

static IOTHUB_DEVICE_HANDLE IoTHubTransportMqtt_Register(TRANSPORT_LL_HANDLE handle, const IOTHUB_DEVICE_CONFIG* device, IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle, PDLIST_ENTRY waitingToSend)
{
    /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_003: [ IoTHubTransportMqtt_Register shall register the TRANSPORT_LL_HANDLE by calling into the IoTHubMqttAbstract_Register function. ] */
    return IoTHubTransport_MQTT_Common_Register(handle, device, iotHubClientHandle, waitingToSend);
}

static void IoTHubTransportMqtt_Unregister(IOTHUB_DEVICE_HANDLE deviceHandle)
{
    /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_004: [ IoTHubTransportMqtt_Unregister shall register the TRANSPORT_LL_HANDLE by calling into the IoTHubMqttAbstract_Unregister function. ] */
    IoTHubTransport_MQTT_Common_Unregister(deviceHandle);
}

static STRING_HANDLE IoTHubTransportMqtt_GetHostname(TRANSPORT_LL_HANDLE handle)
{
    /* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_010: [ IoTHubTransportMqtt_GetHostname shall get the hostname by calling into the IoTHubMqttAbstract_GetHostname function. ] */
    return IoTHubTransport_MQTT_Common_GetHostname(handle);
}

static TRANSPORT_PROVIDER myfunc = {
    IoTHubTransportMqtt_GetHostname,
    IoTHubTransportMqtt_SetOption,
    IoTHubTransportMqtt_Create,
    IoTHubTransportMqtt_Destroy,
    IoTHubTransportMqtt_Register,
    IoTHubTransportMqtt_Unregister,
    IoTHubTransportMqtt_Subscribe,
    IoTHubTransportMqtt_Unsubscribe,
    IoTHubTransportMqtt_DoWork,
    IoTHubTransportMqtt_GetSendStatus
};

/* Codes_SRS_IOTHUB_MQTT_TRANSPORT_07_011: [ This function shall return a pointer to a structure of type TRANSPORT_PROVIDER having the following values for its fields:
IoTHubTransport_GetHostname = IoTHubTransportMqtt_GetHostname
IoTHubTransport_Create = IoTHubTransportMqtt_Create
IoTHubTransport_Destroy = IoTHubTransportMqtt_Destroy
IoTHubTransport_Subscribe = IoTHubTransportMqtt_Subscribe
IoTHubTransport_Unsubscribe = IoTHubTransportMqtt_Unsubscribe
IoTHubTransport_DoWork = IoTHubTransportMqtt_DoWork
IoTHubTransport_SetOption = IoTHubTransportMqtt_SetOption ] */
const TRANSPORT_PROVIDER* MQTT_Protocol(void)
{
    return &myfunc;
}
