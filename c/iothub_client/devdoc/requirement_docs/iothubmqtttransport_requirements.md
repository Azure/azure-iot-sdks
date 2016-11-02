
# IoTHubMQTTTransport Requirements
================

## Overview

IoTHubMQTTTransport is the library that enables communications with the iothub system using the MQTT protocol.


## Exposed API

```c
extern const TRANSPORT_PROVIDER* MQTT_Protocol(void);
```
  The following static functions are provided in the fields of the TRANSPORT_PROVIDER structure:

    - IoTHubTransportMqtt_Subscribe_DeviceMethod,
    - IoTHubTransportMqtt_Unsubscribe_DeviceMethod,
    - IoTHubTransportMqtt_Subscribe_DeviceTwin,
    - IoTHubTransportMqtt_Unsubscribe_DeviceTwin,
    - IoTHubTransportMqtt_ProcessItem,
    - IoTHubTransportMqtt_GetHostname,
    - IoTHubTransportMqtt_SetOption,
    - IoTHubTransportMqtt_Create,
    - IoTHubTransportMqtt_Destroy,
    - IoTHubTransportMqtt_Register,
    - IoTHubTransportMqtt_Unregister,
    - IoTHubTransportMqtt_Subscribe,
    - IoTHubTransportMqtt_Unsubscribe,
    - IoTHubTransportMqtt_DoWork,
    - IoTHubTransportMqtt_SetRetryPolicy,
    - IoTHubTransportMqtt_GetSendStatus

## typedef XIO_HANDLE(*MQTT_GET_IO_TRANSPORT)(const char* fully_qualified_name);

```c
static XIO_HANDLE getIoTransportProvider(const char* fqdn)
```
**SRS_IOTHUB_MQTT_TRANSPORT_07_012: [** getIoTransportProvider shall return the XIO_HANDLE returns by xio_create. **]**

**SRS_IOTHUB_MQTT_TRANSPORT_07_013: [** If platform_get_default_tlsio returns NULL getIoTransportProvider shall return NULL. **]**

## IoTHubTransportMqtt_Create
```c
TRANSPORT_LL_HANDLE IoTHubTransportMqtt_Create(const IOTHUBTRANSPORT_CONFIG* config)
```

**SRS_IOTHUB_MQTT_TRANSPORT_07_001: [** IoTHubTransportMqtt_Create shall create a TRANSPORT_LL_HANDLE by calling into the IoTHubMqttAbstract_Create function.**]**

### IoTHubTransportMqtt_Destroy

```c
void IoTHubTransportMqtt_Destroy(TRANSPORT_LL_HANDLE handle)
```

**SRS_IOTHUB_MQTT_TRANSPORT_07_002: [** IoTHubTransportMqtt_Destroy shall destroy the TRANSPORT_LL_HANDLE by calling into the IoTHubMqttAbstract_Destroy function. **]**

### IoTHubTransportMqtt_Register

```c
extern IOTHUB_DEVICE_HANDLE IoTHubTransportMqtt_Register(TRANSPORT_LL_HANDLE handle, const IOTHUB_DEVICE_CONFIG* device, PDLIST_ENTRY waitingToSend);
```

**SRS_IOTHUB_MQTT_TRANSPORT_07_003: [** IoTHubTransportMqtt_Register shall register the TRANSPORT_LL_HANDLE by calling into the IoTHubMqttAbstract_Register function. **]**

### IoTHubTransportMqtt_Unregister

```c
extern void IoTHubTransportMqtt_Unregister(IOTHUB_DEVICE_HANDLE deviceHandle);
```

**SRS_IOTHUB_MQTT_TRANSPORT_07_004: [** IoTHubTransportMqtt_Unregister shall register the TRANSPORT_LL_HANDLE by calling into the IoTHubMqttAbstract_Unregister function. **]**


### IoTHubTransportMqtt_Subscribe_DeviceTwin

```c
int IoTHubTransportMqtt_Subscribe_DeviceTwin(IOTHUB_DEVICE_HANDLE handle)
```



### IoTHubTransportMqtt_Unsubscribe_DeviceTwin

```c
void IoTHubTransportMqtt_Unsubscribe_DeviceTwin(IOTHUB_DEVICE_HANDLE handle)
```



### IoTHubTransportMqtt_Subscribe_DeviceMethod

```c
int IoTHubTransportMqtt_Subscribe_DeviceMethod(IOTHUB_DEVICE_HANDLE handle)
```



### IoTHubTransportMqtt_Unsubscribe_DeviceMethod

```c
void IoTHubTransportMqtt_Unsubscribe_DeviceMethod(IOTHUB_DEVICE_HANDLE handle)
```



### IoTHubTransportMqtt_Subscribe

```c
int IoTHubTransportMqtt_Subscribe(TRANSPORT_LL_HANDLE handle)
```

**SRS_IOTHUB_MQTT_TRANSPORT_07_005: [** IoTHubTransportMqtt_Subscribe shall subscribe the TRANSPORT_LL_HANDLE by calling into the IoTHubMqttAbstract_Subscribe function. **]**

### IoTHubTransportMqtt_Unsubscribe

```c
void IoTHubTransportMqtt_Unsubscribe(TRANSPORT_LL_HANDLE handle)
```

**SRS_IOTHUB_MQTT_TRANSPORT_07_006: [** IoTHubTransportMqtt_Unsubscribe shall unsubscribe the TRANSPORT_LL_HANDLE by calling into the IoTHubMqttAbstract_Unsubscribe function. **]**


### IoTHubTransportMqtt_ProcessItem

```c
IOTHUB_PROCESS_ITEM_RESULT IoTHubTransportMqtt_ProcessItem(TRANSPORT_LL_HANDLE handle, IOTHUB_IDENTITY_TYPE item_type, IOTHUB_IDENTITY_INFO* iothub_item)
```



### IoTHubTransportMqtt_DoWork

```c
void IoTHubTransportMqtt_DoWork(TRANSPORT_LL_HANDLE handle, IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle)
```

**SRS_IOTHUB_MQTT_TRANSPORT_07_007: [** IoTHubTransportMqtt_DoWork shall call into the IoTHubMqttAbstract_DoWork function. **]**


### IoTHubTransportMqtt_SetRetryPolicy

```c
int IoTHubTransportMqtt_SetRetryPolicy(TRANSPORT_LL_HANDLE handle, IOTHUB_CLIENT_RETRY_POLICY retryPolicy, size_t retryTimeoutLimitinSeconds)
```

**SRS_IOTHUB_MQTT_TRANSPORT_25_012: [** IoTHubTransportMqtt_SetRetryPolicy shall call into the IoTHubMqttAbstract_SetRetryPolicy function. **]**

### IoTHubTransportMqtt_GetSendStatus

```c
IOTHUB_CLIENT_RESULT IoTHubTransportMqtt_GetSendStatus(TRANSPORT_LL_HANDLE handle, IOTHUB_CLIENT_STATUS *iotHubClientStatus)
```

**SRS_IOTHUB_MQTT_TRANSPORT_07_008: [** IoTHubTransportMqtt_GetSendStatus shall get the send status by calling into the IoTHubMqttAbstract_GetSendStatus function. **]**

### IoTHubTransportMqtt_SetOption

```c
IOTHUB_CLIENT_RESULT IoTHubTransportMqtt_SetOption(TRANSPORT_LL_HANDLE handle, const char* optionName, const void* value)
```

**SRS_IOTHUB_MQTT_TRANSPORT_07_009: [** IoTHubTransportMqtt_SetOption shall set the options by calling into the IoTHubMqttAbstract_SetOption function. **]**

```c
STRING_HANDLE IoTHubTransportMqtt_GetHostname(TRANSPORT_LL_HANDLE handle)
```

**SRS_IOTHUB_MQTT_TRANSPORT_07_010: [** IoTHubTransportMqtt_GetHostname shall get the hostname by calling into the IoTHubMqttAbstract_GetHostname function. **]**

### MQTT_Protocol

```c
const TRANSPORT_PROVIDER* MQTT_Protocol(void)
```

**SRS_IOTHUB_MQTT_TRANSPORT_07_022: [**This function shall return a pointer to a structure of type TRANSPORT_PROVIDER having the following values for it's fields:

IoTHubTransport_Subscribe_DeviceMethod = IoTHubTransportMqtt_Subscribe_DeviceMethod
IoTHubTransport_Unsubscribe_DeviceMethod = IoTHubTransportMqtt_Unsubscribe_DeviceMethod
IoTHubTransport_Subscribe_DeviceTwin = IoTHubTransportMqtt_Subscribe_DeviceTwin
IoTHubTransport_Unsubscribe_DeviceTwin = IoTHubTransportMqtt_Unsubscribe_DeviceTwin
IoTHubTransport_ProcessItem - IoTHubTransportMqtt_ProcessItem
IoTHubTransport_GetHostname = IoTHubTransportMqtt_GetHostname
IoTHubTransport_Create = IoTHubTransportMqtt_Create
IoTHubTransport_Destroy = IoTHubTransportMqtt_Destroy
IoTHubTransport_Subscribe = IoTHubTransportMqtt_Subscribe
IoTHubTransport_Unsubscribe = IoTHubTransportMqtt_Unsubscribe
IoTHubTransport_DoWork = IoTHubTransportMqtt_DoWork
IoTHubTransport_SetRetryPolicy = IoTHubTransportMqtt_SetRetryPolicy
IoTHubTransport_SetOption = IoTHubTransportMqtt_SetOption**]**

