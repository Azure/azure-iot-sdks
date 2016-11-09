
# IoTHubTransportAMQP_WS Requirements
================

## Overview

IoTHubTransportAMQP_WS is the library that enables communications with the iothub system using the AMQP protocol over Websockets with TLS connection.


## Dependencies

iothubtransport_amqp_common
azure_c_shared_utility


## Exposed API

```c
static const TRANSPORT_PROVIDER* AMQP_Protocol_over_WebSocketsTls(void);
```
  The following static functions are provided in the fields of the TRANSPORT_PROVIDER structure:

    - IoTHubTransportAMQP_WS_Subscribe_DeviceMethod,
    - IoTHubTransportAMQP_WS_Unsubscribe_DeviceMethod,
    - IoTHubTransportAMQP_WS_Subscribe_DeviceTwin,
    - IoTHubTransportAMQP_WS_Unsubscribe_DeviceTwin,
    - IoTHubTransportAMQP_WS_ProcessItem,
    - IoTHubTransportAMQP_WS_GetHostname,
    - IoTHubTransportAMQP_WS_SetOption,
    - IoTHubTransportAMQP_WS_Create,
    - IoTHubTransportAMQP_WS_Destroy,
    - IoTHubTransportAMQP_WS_SetRetryLogic,
    - IoTHubTransportAMQP_WS_Register,
    - IoTHubTransportAMQP_WS_Unregister,
    - IoTHubTransportAMQP_WS_Subscribe,
    - IoTHubTransportAMQP_WS_Unsubscribe,
    - IoTHubTransportAMQP_WS_DoWork,
    - IoTHubTransportAMQP_WS_GetSendStatus



## IoTHubTransportAMQP_WS_Create

```c
static TRANSPORT_LL_HANDLE IoTHubTransportAMQP_WS_Create(const IOTHUBTRANSPORT_CONFIG* config)
```

**SRS_IoTHubTransportAMQP_WS_09_001: [**IoTHubTransportAMQP_WS_Create shall create a TRANSPORT_LL_HANDLE by calling into the IoTHubTransport_AMQP_Common_Create function, passing `config` and getWebSocketsIOTransport.**]**


### getWebSocketsIOTransport

```c
static XIO_HANDLE getWebSocketsIOTransport(const char* fqdn)
```
**SRS_IoTHubTransportAMQP_WS_09_002: [**getWebSocketsIOTransport shall get `io_interface_description` using wsio_get_interface_description()**]**
**SRS_IoTHubTransportAMQP_WS_09_003: [**If `io_interface_description` is NULL getWebSocketsIOTransport shall return NULL.**]**
**SRS_IoTHubTransportAMQP_WS_09_004: [**getWebSocketsIOTransport shall return the XIO_HANDLE created using xio_create().**]**


## IoTHubTransportAMQP_WS_Destroy

```c
static void IoTHubTransportAMQP_WS_Destroy(TRANSPORT_LL_HANDLE handle)
```

**SRS_IoTHubTransportAMQP_WS_09_005: [**IoTHubTransportAMQP_WS_Destroy shall destroy the TRANSPORT_LL_HANDLE by calling into the IoTHubTransport_AMQP_Common_Destroy().**]**


## IoTHubTransportAMQP_WS_Register

```c
static IOTHUB_DEVICE_HANDLE IoTHubTransportAMQP_WS_Register(TRANSPORT_LL_HANDLE handle, const IOTHUB_DEVICE_CONFIG* device, IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle, PDLIST_ENTRY waitingToSend)
```

**SRS_IoTHubTransportAMQP_WS_09_006: [**IoTHubTransportAMQP_WS_Register shall register the device by calling into the IoTHubTransport_AMQP_Common_Register().**]**


## IoTHubTransportAMQP_WS_Unregister

```c
static void IoTHubTransportAMQP_WS_Unregister(IOTHUB_DEVICE_HANDLE deviceHandle)
```

**SRS_IoTHubTransportAMQP_WS_09_007: [**IoTHubTransportAMQP_WS_Unregister shall unregister the device by calling into the IoTHubTransport_AMQP_Common_Unregister().**]**


## IoTHubTransportAMQP_WS_Subscribe_DeviceTwin

```c
int IoTHubTransportAMQP_WS_Subscribe_DeviceTwin(IOTHUB_DEVICE_HANDLE handle)
```

**SRS_IoTHubTransportAMQP_WS_09_008: [**IoTHubTransportAMQP_WS_Subscribe_DeviceTwin shall invoke IoTHubTransport_AMQP_Common_Subscribe_DeviceTwin() and return its result.**]**


## IoTHubTransportAMQP_WS_Unsubscribe_DeviceTwin

```c
void IoTHubTransportAMQP_WS_Unsubscribe_DeviceTwin(IOTHUB_DEVICE_HANDLE handle)
```

**SRS_IoTHubTransportAMQP_WS_09_009: [**IoTHubTransportAMQP_WS_Unsubscribe_DeviceTwin shall invoke IoTHubTransport_AMQP_Common_Unsubscribe_DeviceTwin()**]**


## IoTHubTransportAMQP_WS_Subscribe_DeviceMethod

```c
int IoTHubTransportAMQP_WS_Subscribe_DeviceMethod(IOTHUB_DEVICE_HANDLE handle)
```

**SRS_IoTHubTransportAMQP_WS_09_010: [**IoTHubTransportAMQP_WS_Subscribe_DeviceMethod shall invoke IoTHubTransport_AMQP_Common_Subscribe_DeviceMethod() and return its result.**]**


## IoTHubTransportAMQP_WS_Unsubscribe_DeviceMethod

```c
void IoTHubTransportAMQP_WS_Unsubscribe_DeviceMethod(IOTHUB_DEVICE_HANDLE handle)
```

**SRS_IoTHubTransportAMQP_WS_09_011: [**IoTHubTransportAMQP_WS_Unsubscribe_DeviceMethod shall invoke IoTHubTransport_AMQP_Common_Unsubscribe_DeviceMethod()**]**


## IoTHubTransportAMQP_WS_Subscribe

```c
static int IoTHubTransportAMQP_WS_Subscribe(TRANSPORT_LL_HANDLE handle)
```

**SRS_IoTHubTransportAMQP_WS_09_012: [**IoTHubTransportAMQP_WS_Subscribe shall subscribe for D2C messages by calling into the IoTHubTransport_AMQP_Common_Subscribe().**]**


## IoTHubTransportAMQP_WS_Unsubscribe

```c
static void IoTHubTransportAMQP_WS_Unsubscribe(TRANSPORT_LL_HANDLE handle)
```

**SRS_IoTHubTransportAMQP_WS_09_013: [**IoTHubTransportAMQP_WS_Unsubscribe shall subscribe for D2C messages by calling into the IoTHubTransport_AMQP_Common_Unsubscribe().**]**


## IoTHubTransportAMQP_WS_ProcessItem

```c
static IOTHUB_PROCESS_ITEM_RESULT IoTHubTransportAMQP_WS_ProcessItem(TRANSPORT_LL_HANDLE handle, IOTHUB_IDENTITY_TYPE item_type, IOTHUB_IDENTITY_INFO* iothub_item)
```

**SRS_IoTHubTransportAMQP_WS_09_014: [**IoTHubTransportAMQP_WS_ProcessItem shall invoke IoTHubTransport_AMQP_Common_ProcessItem() and return its result.**]**


## IoTHubTransportAMQP_WS_DoWork

```c
static void IoTHubTransportAMQP_WS_DoWork(TRANSPORT_LL_HANDLE handle, IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle)
```

**SRS_IoTHubTransportAMQP_WS_09_015: [**IoTHubTransportAMQP_WS_DoWork shall call into the IoTHubTransport_AMQP_Common_DoWork()**]**


## IoTHubTransportAMQP_WS_GetSendStatus

```c
IOTHUB_CLIENT_RESULT IoTHubTransportAMQP_WS_GetSendStatus(TRANSPORT_LL_HANDLE handle, IOTHUB_CLIENT_STATUS *iotHubClientStatus)
```

**SRS_IoTHubTransportAMQP_WS_09_016: [**IoTHubTransportAMQP_WS_GetSendStatus shall get the send status by calling into the IoTHubTransport_AMQP_Common_GetSendStatus()**]**


## IoTHubTransportAMQP_WS_SetOption

```c
IOTHUB_CLIENT_RESULT IoTHubTransportAMQP_WS_SetOption(TRANSPORT_LL_HANDLE handle, const char* optionName, const void* value)
```

**SRS_IoTHubTransportAMQP_WS_09_017: [**IoTHubTransportAMQP_WS_SetOption shall set the options by calling into the IoTHubTransport_AMQP_Common_SetOption()**]**


## IoTHubTransportAMQP_WS_GetHostname

```c
static STRING_HANDLE IoTHubTransportAMQP_WS_GetHostname(TRANSPORT_LL_HANDLE handle)
```

**SRS_IoTHubTransportAMQP_WS_09_018: [**IoTHubTransportAMQP_WS_GetHostname shall get the hostname by calling into the IoTHubTransport_AMQP_Common_GetHostname()**]**


### AMQP_Protocol_over_WebSocketsTls

```c
const TRANSPORT_PROVIDER* AMQP_Protocol_over_WebSocketsTls(void)
```

**SRS_IoTHubTransportAMQP_WS_09_019: [**This function shall return a pointer to a structure of type TRANSPORT_PROVIDER having the following values for it's fields:
IoTHubTransport_Subscribe_DeviceMethod = IoTHubTransportAMQP_WS_Subscribe_DeviceMethod
IoTHubTransport_Unsubscribe_DeviceMethod = IoTHubTransportAMQP_WS_Unsubscribe_DeviceMethod
IoTHubTransport_Subscribe_DeviceTwin = IoTHubTransportAMQP_WS_Subscribe_DeviceTwin
IoTHubTransport_Unsubscribe_DeviceTwin = IoTHubTransportAMQP_WS_Unsubscribe_DeviceTwin
IoTHubTransport_ProcessItem - IoTHubTransportAMQP_WS_ProcessItem
IoTHubTransport_GetHostname = IoTHubTransportAMQP_WS_GetHostname
IoTHubTransport_Create = IoTHubTransportAMQP_WS_Create
IoTHubTransport_Destroy = IoTHubTransportAMQP_WS_Destroy
IoTHubTransport_Subscribe = IoTHubTransportAMQP_WS_Subscribe
IoTHubTransport_Unsubscribe = IoTHubTransportAMQP_WS_Unsubscribe
IoTHubTransport_DoWork = IoTHubTransportAMQP_WS_DoWork
IoTHubTransport_SetRetryLogic = IoTHubTransportAMQP_WS_SetRetryLogic
IoTHubTransport_SetOption = IoTHubTransportAMQP_WS_SetOption**]**
