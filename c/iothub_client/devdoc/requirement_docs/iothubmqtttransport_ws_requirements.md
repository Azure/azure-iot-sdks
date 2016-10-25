IoTHubMQTTTransport WebSockets Requirements
================

## Overview

IoTHubMQTTTransport_WS is the library that enables communications with the iothub system using the MQTT protocol over websockets. 

## Exposed API

```c
extern const TRANSPORT_PROVIDER* MQTT_Protocol(void);
```

  The following static functions are provided in the fields of the TRANSPORT_PROVIDER structure:
    - IoTHubTransportMqtt_WS_GetHostname,  
    - IoTHubTransportMqtt_WS_SetOption,  
    - IoTHubTransportMqtt_WS_Create,  
    - IoTHubTransportMqtt_WS_Destroy,  
    - IoTHubTransportMqtt_WS_Register,  
    - IoTHubTransportMqtt_WS_Unregister,  
    - IoTHubTransportMqtt_WS_Subscribe,  
    - IoTHubTransportMqtt_WS_Unsubscribe,  
    - IoTHubTransportMqtt_WS_DoWork,  
    - IoTHubTransportMqtt_WS_GetSendStatus

## typedef XIO_HANDLE(*MQTT_GET_IO_TRANSPORT)(const char* fully_qualified_name);

```c
static XIO_HANDLE getIoTransportProvider(const char* fqdn)
```

**SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_07_012: [** getIoTransportProvider shall return the XIO_HANDLE returns by xio_create. **]**

**SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_07_013: [** If platform_get_default_tlsio returns NULL getIoTransportProvider shall return NULL. **]**

## IoTHubTransportMqtt_WS_Create

```c
TRANSPORT_LL_HANDLE IoTHubTransportMqtt_WS_Create(const IOTHUBTRANSPORT_CONFIG* config)
```

**SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_07_001: [** IoTHubTransportMqtt_WS_Create shall create a TRANSPORT_LL_HANDLE by calling into the IoTHubMqttAbstract_Create function. **]**

## IoTHubTransportMqtt_WS_Destroy

```c
void IoTHubTransportMqtt_WS_Destroy(TRANSPORT_LL_HANDLE handle)
```

**SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_07_002: [** IoTHubTransportMqtt_WS_Destroy shall destroy the TRANSPORT_LL_HANDLE by calling into the IoTHubMqttAbstract_Destroy function. **]**

### IoTHubTransportMqtt_WS_Register

```c
extern IOTHUB_DEVICE_HANDLE IoTHubTransportMqtt_WS_Register(RANSPORT_LL_HANDLE handle, const IOTHUB_DEVICE_CONFIG* device, PDLIST_ENTRY waitingToSend);
```

**SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_07_003: [** IoTHubTransportMqtt_WS_Register shall register the TRANSPORT_LL_HANDLE by calling into the IoTHubMqttAbstract_Register function. **]**

### IoTHubTransportMqtt_WS_Unregister

```c
extern void IoTHubTransportMqtt_WS_Unregister(IOTHUB_DEVICE_HANDLE deviceHandle);
```

**SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_07_004: [** IoTHubTransportMqtt_WS_Unregister shall register the TRANSPORT_LL_HANDLE by calling into the IoTHubMqttAbstract_Unregister function. **]**

### IoTHubTransportMqtt_WS_Subscribe

```c
int IoTHubTransportMqtt_WS_Subscribe(TRANSPORT_LL_HANDLE handle)
```

**SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_07_005: [** IoTHubTransportMqtt_WS_Subscribe shall subscribe the TRANSPORT_LL_HANDLE by calling into the IoTHubMqttAbstract_Subscribe function. **]**

### IoTHubTransportMqtt_WS_Unsubscribe

```c
void IoTHubTransportMqtt_WS_Unsubscribe(TRANSPORT_LL_HANDLE handle)
```

**SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_07_006: [** IoTHubTransportMqtt_WS_Unsubscribe shall unsubscribe the TRANSPORT_LL_HANDLE by calling into the IoTHubMqttAbstract_Unsubscribe function. **]**

### IoTHubTransportMqtt_WS_DoWork

```c
void IoTHubTransportMqtt_WS_DoWork(TRANSPORT_LL_HANDLE handle, IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle)
```

**SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_07_007: [** IoTHubTransportMqtt_WS_DoWork shall call into the IoTHubMqttAbstract_DoWork function. **]**

### IoTHubTransportMqtt_WS_GetSendStatus

```c
IOTHUB_CLIENT_RESULT IoTHubTransportMqtt_WS_GetSendStatus(TRANSPORT_LL_HANDLE handle, IOTHUB_CLIENT_STATUS *iotHubClientStatus)
```

**SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_07_008: [** IoTHubTransportMqtt_WS_GetSendStatus shall get the send status by calling into the IoTHubMqttAbstract_GetSendStatus function. **]**

### IoTHubTransportMqtt_WS_SetOption

```c
IOTHUB_CLIENT_RESULT IoTHubTransportMqtt_WS_SetOption(TRANSPORT_LL_HANDLE handle, const char* optionName, const void* value)
```

**SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_07_009: [** IoTHubTransportMqtt_WS_SetOption shall set the options by calling into the IoTHubMqttAbstract_SetOption function. **]**

```c
STRING_HANDLE IoTHubTransportMqtt_WS_GetHostname(TRANSPORT_LL_HANDLE handle)
```

**SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_07_010: [** IoTHubTransportMqtt_WS_GetHostname shall get the hostname by calling into the IoTHubMqttAbstract_GetHostname function. **]**

### MQTT_WS_Protocol

```c
const TRANSPORT_PROVIDER* MQTT_WebSocket_Protocol(void)
```

**SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_07_011: [** This function shall return a pointer to a structure of type TRANSPORT_PROVIDER having the following values for its fields:

IoTHubTransport_GetHostname = IoTHubTransportMqtt_WS_GetHostname  
IoTHubTransport_Create = IoTHubTransportMqtt_WS_Create  
IoTHubTransport_Destroy = IoTHubTransportMqtt_WS_Destroy  
IoTHubTransport_Subscribe = IoTHubTransportMqtt_WS_Subscribe  
IoTHubTransport_Unsubscribe = IoTHubTransportMqtt_WS_Unsubscribe  
IoTHubTransport_DoWork = IoTHubTransportMqtt_WS_DoWork  
IoTHubTransport_SetOption = IoTHubTransportMqtt_WS_SetOption **]**
