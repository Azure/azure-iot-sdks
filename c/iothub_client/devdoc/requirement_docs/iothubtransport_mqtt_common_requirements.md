IoTHubTransport_MQTT_Common Requirements
================

## Overview

IoTHubTransport_MQTT_Common is the library that enables communications with the iothub system using the MQTT protocol. 

## Exposed API

```c
typedef XIO_HANDLE(*MQTT_GET_IO_TRANSPORT)(const char* fully_qualified_name);

MOCKABLE_FUNCTION(, TRANSPORT_LL_HANDLE, IoTHubTransport_MQTT_Common_Create, const IOTHUBTRANSPORT_CONFIG*,  config, MQTT_GET_IO_TRANSPORT, get_io_transport);
MOCKABLE_FUNCTION(, void, IoTHubTransport_MQTT_Common_Destroy, TRANSPORT_LL_HANDLE, handle);
MOCKABLE_FUNCTION(, int, IoTHubTransport_MQTT_Common_Subscribe, IOTHUB_DEVICE_HANDLE, handle);
MOCKABLE_FUNCTION(, void, IoTHubTransport_MQTT_Common_Unsubscribe, IOTHUB_DEVICE_HANDLE, handle);
MOCKABLE_FUNCTION(, void, IoTHubTransport_MQTT_Common_DoWork, TRANSPORT_LL_HANDLE, handle, IOTHUB_CLIENT_LL_HANDLE, iotHubClientHandle);
MOCKABLE_FUNCTION(, IOTHUB_CLIENT_RESULT, IoTHubTransport_MQTT_Common_GetSendStatus, IOTHUB_DEVICE_HANDLE, handle, IOTHUB_CLIENT_STATUS*, iotHubClientStatus);
MOCKABLE_FUNCTION(, IOTHUB_CLIENT_RESULT, IoTHubTransport_MQTT_Common_SetOption, TRANSPORT_LL_HANDLE, handle, const char*, option, const void*, value);
MOCKABLE_FUNCTION(, IOTHUB_DEVICE_HANDLE, IoTHubTransport_MQTT_Common_Register, TRANSPORT_LL_HANDLE, handle, const IOTHUB_DEVICE_CONFIG*, device, IOTHUB_CLIENT_LL_HANDLE, iotHubClientHandle, PDLIST_ENTRY, waitingToSend);
MOCKABLE_FUNCTION(, void, IoTHubTransport_MQTT_Common_Unregister, IOTHUB_DEVICE_HANDLE, deviceHandle);
MOCKABLE_FUNCTION(, STRING_HANDLE, IoTHubTransport_MQTT_Common_GetHostname, TRANSPORT_LL_HANDLE, handle);
MOCKABLE_FUNCTION(, int, IoTHubTransport_MQTT_Common_SetRetryPolicy, TRANSPORT_LL_HANDLE, handle, IOTHUB_CLIENT_RETRY_POLICY, retryPolicy, size_t, retryTimeoutLimitinSeconds);
```

## IoTHubTransport_MQTT_Common_Create

```c
TRANSPORT_LL_HANDLE IoTHubTransport_MQTT_Common_Create(const IOTHUBTRANSPORT_CONFIG* config, MQTT_GET_IO_TRANSPORT get_io_transport)
```

IoTHubTransport_MQTT_Common_Create shall create a TRANSPORT_LL_HANDLE that can be further used in the calls to this module’s APIS.  

**SRS_IOTHUB_TRANSPORT_MQTT_COMMON_07_001: [**If parameter config is NULL then IoTHubTransport_MQTT_Common_Create shall return NULL.**]**

**SRS_IOTHUB_TRANSPORT_MQTT_COMMON_07_041: [** if get_io_transport is NULL then IoTHubTransport_MQTT_Common_Create shall return NULL. **]**

**SRS_IOTHUB_TRANSPORT_MQTT_COMMON_07_002: [**If the parameter config's variables upperConfig or waitingToSend are NULL then IoTHubTransport_MQTT_Common_Create shall return NULL.**]**

**SRS_IOTHUB_TRANSPORT_MQTT_COMMON_07_003: [**If the upperConfig's variables deviceId, iotHubName, protocol, or iotHubSuffix are NULL then IoTHubTransport_MQTT_Common_Create shall return NULL.**]**

**SRS_IOTHUB_TRANSPORT_MQTT_COMMON_07_004: [**If the config's waitingToSend variable is NULL then IoTHubTransport_MQTT_Common_Create shall return NULL.**]**

**SRS_IOTHUB_TRANSPORT_MQTT_COMMON_07_005: [**If the upperConfig's variables deviceKey, iotHubName, or iotHubSuffix are empty strings then IoTHubTransport_MQTT_Common_Create shall return NULL.**]**  

**SRS_IOTHUB_TRANSPORT_MQTT_COMMON_07_006: [**If the upperConfig's variables deviceId is an empty strings or length is greater then 128 then IoTHubTransport_MQTT_Common_Create shall return NULL.**]**  

**SRS_IOTHUB_TRANSPORT_MQTT_COMMON_07_008: [**If the upperConfig contains a valid protocolGatewayHostName value the this shall be used for the hostname, otherwise the hostname shall be constructed using the iothubname and iothubSuffix.**]**  

**SRS_IOTHUB_TRANSPORT_MQTT_COMMON_07_009: [**If any error is encountered then IoTHubTransport_MQTT_Common_Create shall return NULL.**]**  

**SRS_IOTHUB_TRANSPORT_MQTT_COMMON_07_010: [**IoTHubTransport_MQTT_Common_Create shall allocate memory to save its internal state where all topics, hostname, device_id, device_key, sasTokenSr and client handle shall be saved.**]**  

**SRS_IOTHUB_TRANSPORT_MQTT_COMMON_07_011: [**On Success IoTHubTransport_MQTT_Common_Create shall return a non-NULL value.**]**  

**SRS_IOTHUB_TRANSPORT_MQTT_COMMON_07_041: [**If both deviceKey and deviceSasToken fields are NULL then IoTHubTransport_MQTT_Common_Create shall assume a x509 authentication.**]**  

### IoTHubTransport_MQTT_Common_Destroy

```c
void IoTHubTransport_MQTT_Common_Destroy(TRANSPORT_LL_HANDLE handle)
```

**SRS_IOTHUB_TRANSPORT_MQTT_COMMON_07_012: [**IoTHubTransport_MQTT_Common_Destroy shall do nothing if parameter handle is NULL.**]**  

**SRS_IOTHUB_TRANSPORT_MQTT_COMMON_07_014: [**IoTHubTransport_MQTT_Common_Destroy shall free all the resources currently in use.**]**  

### IoTHubTransport_MQTT_Common_Register

```c
extern IOTHUB_DEVICE_HANDLE IoTHubTransport_MQTT_Common_Register(RANSPORT_LL_HANDLE handle, const IOTHUB_DEVICE_CONFIG* device, PDLIST_ENTRY waitingToSend);
```

This function registers a device with the transport.  The MQTT transport only supports a single device established on create, so this function will prevent multiple devices from being registered.

**SRS_IOTHUB_TRANSPORT_MQTT_COMMON_17_001: [** `IoTHubTransport_MQTT_Common_Register` shall return `NULL` if the `TRANSPORT_LL_HANDLE` is `NULL`.**]**  

**SRS_IOTHUB_TRANSPORT_MQTT_COMMON_17_002: [** `IoTHubTransport_MQTT_Common_Register` shall return `NULL` if `device`, `waitingToSend` are `NULL`.**]**  

**SRS_IOTHUB_TRANSPORT_MQTT_COMMON_03_001: [** `IoTHubTransport_MQTT_Common_Register` shall return `NULL` if `deviceId`, or both `deviceKey` and `deviceSasToken` are `NULL`.**]**  

**SRS_IOTHUB_TRANSPORT_MQTT_COMMON_03_002: [** `IoTHubTransport_MQTT_Common_Register` shall return `NULL` if both `deviceKey` and `deviceSasToken` are provided.**]**  

**SRS_IOTHUB_TRANSPORT_MQTT_COMMON_17_003: [** `IoTHubTransport_MQTT_Common_Register` shall return `NULL` if `deviceId` or `deviceKey` do not match the `deviceId` and `deviceKey` passed in during `IoTHubTransport_MQTT_Common_Create`.**]**  

**SRS_IOTHUB_TRANSPORT_MQTT_COMMON_17_004: [** `IoTHubTransport_MQTT_Common_Register` shall return the `TRANSPORT_LL_HANDLE` as the `IOTHUB_DEVICE_HANDLE`. **]**  

### IoTHubTransport_MQTT_Common_Unregister

```c
extern void IoTHubTransport_MQTT_Common_Unregister(IOTHUB_DEVICE_HANDLE deviceHandle);
```

This function is intended to remove a device as registered with the transport.  As there is only one IoT Hub Device per MQTT transport established on create, this function is a placeholder not intended to do meaningful work.

**SRS_IOTHUB_TRANSPORT_MQTT_COMMON_17_005: [** `IoTHubTransport_MQTT_Common_Unregister` shall return. **]**  


### IoTHubTransport_MQTT_Common_Subscribe_DeviceTwin

```c
int IoTHubTransport_MQTT_Common_Subscribe_DeviceTwin(IOTHUB_DEVICE_HANDLE handle)
```

**SRS_IOTHUB_MQTT_TRANSPORT_07_042: [**If the parameter `handle` is `NULL` than `IoTHubTransport_MQTT_Common_Subscribe` shall return a non-zero value. **]**

**SRS_IOTHUB_MQTT_TRANSPORT_07_044: [**`IoTHubTransport_MQTT_Common_Subscribe_DeviceTwin` shall construct the get state topic string and the notify state topic string. **]**

**SRS_IOTHUB_MQTT_TRANSPORT_07_046: [**Upon failure `IoTHubTransport_MQTT_Common_Subscribe_DeviceTwin` shall return a non-zero value. **]**

**SRS_IOTHUB_MQTT_TRANSPORT_07_047: [**On success `IoTHubTransport_MQTT_Common_Subscribe_DeviceTwin` shall return 0. **]**


### IoTHubTransport_MQTT_Common_Unsubscribe_DeviceTwin

```c
void IoTHubTransport_MQTT_Common_Unsubscribe_DeviceTwin(IOTHUB_DEVICE_HANDLE handle)
```

**SRS_IOTHUB_MQTT_TRANSPORT_07_048: [** If the parameter `handle` is `NULL` than `IoTHubTransport_MQTT_Common_Unsubscribe_DeviceTwin` shall do nothing. **]**

**SRS_IOTHUB_MQTT_TRANSPORT_07_049: [** If `subscribe_state` is set to `IOTHUB_DEVICE_TWIN_DESIRED_STATE` then `IoTHubTransport_MQTT_Common_Unsubscribe_DeviceTwin` shall remove the get state topic from the subscription flag. **]**

**SRS_IOTHUB_MQTT_TRANSPORT_07_050: [** If `subscribe_state` is set to `IOTHUB_DEVICE_TWIN_NOTIFICATION_STATE` then `IoTHubTransport_MQTT_Common_Unsubscribe_DeviceTwin` shall remove the get state topic from the subscription flag. **]**


### IoTHubTransport_MQTT_Common_Subscribe_DeviceMethod

```c
int IoTHubTransport_MQTT_Common_Subscribe_DeviceMethod(IOTHUB_DEVICE_HANDLE handle)
```

**SRS_IOTHUB_MQTT_TRANSPORT_12_001: [** If the parameter `handle` is `NULL` than `IoTHubTransport_MQTT_Common_Subscribe_DeviceMethod` shall return a non-zero value. **]**

**SRS_IOTHUB_MQTT_TRANSPORT_12_002: [** If the MQTT transport has been previously subscribed to DEVICE_METHOD topic `IoTHubTransport_MQTT_Common_Subscribe_DeviceMethod` shall do nothing and return 0. **]**

**SRS_IOTHUB_MQTT_TRANSPORT_12_003: [** `IoTHubTransport_MQTT_Common_Subscribe_DeviceMethod` shall set the signaling flag for DEVICE_METHOD topic for the receiver's topic list. **]**

**SRS_IOTHUB_MQTT_TRANSPORT_12_004: [** `IoTHubTransport_MQTT_Common_Subscribe_DeviceMethod` shall construct the DEVICE_METHOD topic string for subscribe. **]**

**SRS_IOTHUB_MQTT_TRANSPORT_12_005: [** `IoTHubTransport_MQTT_Common_Subscribe_DeviceMethod` shall schedule the send of the subscription. **]**

**SRS_IOTHUB_MQTT_TRANSPORT_12_006: [** Upon failure `IoTHubTransport_MQTT_Common_Subscribe_DeviceMethod` shall return a non-zero value. **]**

**SRS_IOTHUB_MQTT_TRANSPORT_12_007: [** On success `IoTHubTransport_MQTT_Common_Subscribe_DeviceMethod` shall return 0. **]**


### IoTHubTransport_MQTT_Common_Unsubscribe_DeviceMethod

```c
void IoTHubTransport_MQTT_Common_Unsubscribe_DeviceMethod(IOTHUB_DEVICE_HANDLE handle)
```

**SRS_IOTHUB_MQTT_TRANSPORT_12_008: [** If the parameter `handle` is `NULL` than `IoTHubTransport_MQTT_Common_Unsubscribe_DeviceMethod` shall do nothing and return. **]**

**SRS_IOTHUB_MQTT_TRANSPORT_12_009: [** If the MQTT transport has not been subscribed to DEVICE_METHOD topic `IoTHubTransport_MQTT_Common_Unsubscribe_DeviceMethod` shall do nothing and return. **]**

**SRS_IOTHUB_MQTT_TRANSPORT_12_010: [** `IoTHubTransport_MQTT_Common_Unsubscribe_DeviceMethod` shall construct the DEVICE_METHOD topic string for unsubscribe. **]**

**SRS_IOTHUB_MQTT_TRANSPORT_12_011: [** `IoTHubTransport_MQTT_Common_Unsubscribe_DeviceMethod` shall send the unsubscribe. **]**

**SRS_IOTHUB_MQTT_TRANSPORT_12_012: [** `IoTHubTransport_MQTT_Common_Unsubscribe_DeviceMethod` shall removes the signaling flag for DEVICE_METHOD topic from the receiver's topic list. **]**


### IoTHubTransport_MQTT_Common_Subscribe

```c
int IoTHubTransport_MQTT_Common_Subscribe(TRANSPORT_LL_HANDLE handle)
```

**SRS_IOTHUB_TRANSPORT_MQTT_COMMON_07_015: [**If parameter handle is NULL than IoTHubTransport_MQTT_Common_Subscribe shall return a non-zero value.**]**  

**SRS_IOTHUB_TRANSPORT_MQTT_COMMON_07_016: [**IoTHubTransport_MQTT_Common_Subscribe shall set a flag to enable mqtt_client_subscribe to be called to subscribe to the Message Topic.**]**

**SRS_IOTHUB_TRANSPORT_MQTT_COMMON_07_035: [**If current packet state is not CONNACK, DISCONNECT_TYPE, or PACKET_TYPE_ERROR then IoTHubTransport_MQTT_Common_Subscribe shall set the packet state to SUBSCRIBE_TYPE.**]**   

**SRS_IOTHUB_TRANSPORT_MQTT_COMMON_07_017: [**Upon failure IoTHubTransport_MQTT_Common_Subscribe shall return a non-zero value.**]**    

**SRS_IOTHUB_TRANSPORT_MQTT_COMMON_07_018: [**On success IoTHubTransport_MQTT_Common_Subscribe shall return 0.**]**  

### IoTHubTransport_MQTT_Common_Unsubscribe

```c
void IoTHubTransport_MQTT_Common_Unsubscribe(TRANSPORT_LL_HANDLE handle)
```

**SRS_IOTHUB_TRANSPORT_MQTT_COMMON_07_019: [**If parameter handle is NULL then IoTHubTransport_MQTT_Common_Unsubscribe shall do nothing.**]**  

**SRS_IOTHUB_TRANSPORT_MQTT_COMMON_07_020: [**IoTHubTransport_MQTT_Common_Unsubscribe shall call mqtt_client_unsubscribe to unsubscribe the mqtt message topic.**]**  

### IoTHubTransport_MQTT_Common_ProcessItem

```c
IOTHUB_PROCESS_ITEM_RESULT IoTHubTransport_MQTT_Common_ProcessItem(TRANSPORT_LL_HANDLE handle, IOTHUB_IDENTITY_TYPE item_type, IOTHUB_IDENTITY_INFO* iothub_item)
```

**SRS_IOTHUBCLIENT_LL_07_001: [** If `handle` or `iothub_item` are `NULL` then `IoTHubTransport_MQTT_Common_ProcessItem` shall return `IOTHUB_PROCESS_ERROR`. **]**

**SRS_IOTHUBCLIENT_LL_07_002: [** If the mqtt is not ready to publish messages `IoTHubTransport_MQTT_Common_ProcessItem` shall return `IOTHUB_PROCESS_NOT_CONNECTED`. **]**

**SRS_IOTHUBCLIENT_LL_07_003: [** `IoTHubTransport_MQTT_Common_ProcessItem` shall publish a message to the mqtt protocol with the message topic for the message type. **]**

**SRS_IOTHUBCLIENT_LL_07_004: [** If any errors are encountered `IoTHubTransport_MQTT_Common_ProcessItem` shall return `IOTHUB_PROCESS_ERROR`. **]**

**SRS_IOTHUBCLIENT_LL_07_005: [** If successful `IoTHubTransport_MQTT_Common_ProcessItem` shall add mqtt info structure acknowledgement queue. **]**

**SRS_IOTHUBCLIENT_LL_07_006: [** If the `item_type` is not a supported type `IoTHubTransport_MQTT_Common_ProcessItem` shall return `IOTHUB_PROCESS_CONTINUE`. **]**


### IoTHubTransport_MQTT_Common_DoWork

```c
void IoTHubTransport_MQTT_Common_DoWork(TRANSPORT_LL_HANDLE handle, IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle)
```

**SRS_IOTHUB_TRANSPORT_MQTT_COMMON_07_026: [**IoTHubTransport_MQTT_Common_DoWork shall do nothing if parameter handle and/or iotHubClientHandle is NULL.**]**  

**SRS_IOTHUB_TRANSPORT_MQTT_COMMON_07_027: [**IoTHubTransport_MQTT_Common_DoWork shall inspect the “waitingToSend” DLIST passed in config structure.**]**  

**SRS_IOTHUB_TRANSPORT_MQTT_COMMON_07_028: [**IoTHubTransport_MQTT_Common_DoWork shall retrieve the payload message from the messageHandle parameter.**]**  

**SRS_IOTHUB_TRANSPORT_MQTT_COMMON_07_029: [**IoTHubTransport_MQTT_Common_DoWork shall create a MQTT_MESSAGE_HANDLE and pass this to a call to  mqtt_client_publish.**]**  

**SRS_IOTHUB_TRANSPORT_MQTT_COMMON_07_030: [**IoTHubTransport_MQTT_Common_DoWork shall call mqtt_client_dowork everytime it is called if it is connected.**]**  

**SRS_IOTHUB_TRANSPORT_MQTT_COMMON_07_033: [**IoTHubTransport_MQTT_Common_DoWork shall iterate through the Waiting Acknowledge messages looking for any message that has been waiting longer than 2 min.**]**  

**SRS_IOTHUB_TRANSPORT_MQTT_COMMON_07_034: [**If IoTHubTransport_MQTT_Common_DoWork has previously resent the message two times then it shall fail the message**]**  

### IoTHubTransport_MQTT_Common_GetSendStatus

```c
IOTHUB_CLIENT_RESULT IoTHubTransport_MQTT_Common_GetSendStatus(TRANSPORT_LL_HANDLE handle, IOTHUB_CLIENT_STATUS *iotHubClientStatus)
```

**SRS_IOTHUB_TRANSPORT_MQTT_COMMON_07_023: [**IoTHubTransport_MQTT_Common_GetSendStatus shall return IOTHUB_CLIENT_INVALID_ARG if called with NULL parameter.**]**  

**SRS_IOTHUB_TRANSPORT_MQTT_COMMON_07_024: [**IoTHubTransport_MQTT_Common_GetSendStatus shall return IOTHUB_CLIENT_OK and status IOTHUB_CLIENT_SEND_STATUS_IDLE if there are currently no event items to be sent or being sent.**]**   

**SRS_IOTHUB_TRANSPORT_MQTT_COMMON_07_025: [**IoTHubTransport_MQTT_Common_GetSendStatus shall return IOTHUB_CLIENT_OK and status IOTHUB_CLIENT_SEND_STATUS_BUSY if there are currently event items to be sent or being sent.**]**  

### IoTHubTransport_MQTT_Common_SetOption

```c
IOTHUB_CLIENT_RESULT IoTHubTransport_MQTT_Common_SetOption(TRANSPORT_LL_HANDLE handle, const char* optionName, const void* value)
```

**SRS_IOTHUB_TRANSPORT_MQTT_COMMON_07_021: [**If any parameter is NULL then IoTHubTransport_MQTT_Common_SetOption shall return IOTHUB_CLIENT_INVALID_ARG.**]**

**SRS_IOTHUB_TRANSPORT_MQTT_COMMON_07_031: [**If the option parameter is set to "logtrace" then the value shall be a bool_ptr and the value will determine if the mqtt client log is on or off.**]**  

**SRS_IOTHUB_TRANSPORT_MQTT_COMMON_07_032: [**IoTHubTransport_MQTT_Common_SetOption shall pass down the option to xio_setoption if the option parameter is not a known option string for the MQTT transport.**]**  

**SRS_IOTHUB_TRANSPORT_MQTT_COMMON_07_036: [**If the option parameter is set to "keepalive" then the value shall be a int_ptr and the value will determine the mqtt keepalive time that is set for pings.**]**  

**SRS_IOTHUB_TRANSPORT_MQTT_COMMON_07_037: [**If the option parameter is set to supplied int_ptr keepalive is the same value as the existing keepalive then IoTHubTransport_MQTT_Common_SetOption shall do nothing.**]**  

**SRS_IOTHUB_TRANSPORT_MQTT_COMMON_07_038: [**If the client is connected when the keepalive is set then IoTHubTransport_MQTT_Common_SetOption shall disconnect and reconnect with the specified keepalive value.**]**  

**SRS_IOTHUB_TRANSPORT_MQTT_COMMON_07_039: [**If the option parameter is set to "x509certificate" then the value shall be a const char* of the certificate to be used for x509.**]**  

**SRS_IOTHUB_TRANSPORT_MQTT_COMMON_07_040: [**If the option parameter is set to "x509privatekey" then the value shall be a const char* of the RSA Private Key to be used for x509.**]**  

### IoTHubTransport_MQTT_Common_SetRetryPolicy
```c
int IoTHubTransport_MQTT_Common_SetRetryPolicy(TRANSPORT_LL_HANDLE handle, IOTHUB_CLIENT_RETRY_POLICY retryPolicy, size_t retryTimeoutLimitinSeconds)
```

**SRS_IOTHUB_TRANSPORT_MQTT_COMMON_25_041: [**If any handle is NULL then IoTHubTransport_MQTT_Common_SetRetryPolicy shall return resultant line.**]**

**SRS_IOTHUB_TRANSPORT_MQTT_COMMON_25_042: [**If the retry logic is not already created then IoTHubTransport_MQTT_Common_SetRetryPolicy shall create retry logic by calling CreateRetryLogic with retry policy and retryTimeout as parameters**]**  

**SRS_IOTHUB_TRANSPORT_MQTT_COMMON_25_043: [**If the retry logic is already created then IoTHubTransport_MQTT_Common_SetRetryPolicy shall destroy existing retry logic and create retry logic by calling CreateRetryLogic with retry policy and retryTimeout as parameters**]**  

**SRS_IOTHUB_TRANSPORT_MQTT_COMMON_25_044: [**If retry logic for specified parameters of retry policy and retryTimeoutLimitinSeconds cannot be created then IoTHubTransport_MQTT_Common_SetRetryPolicy shall return resultant line*]**  

**SRS_IOTHUB_TRANSPORT_MQTT_COMMON_25_045: [**If retry logic for specified parameters of retry policy and retryTimeoutLimitinSeconds is created successfully then IoTHubTransport_MQTT_Common_SetRetryPolicy shall return 0 **]**  

```c
STRING_HANDLE IoTHubTransport_MQTT_Common_GetHostname(TRANSPORT_LL_HANDLE handle)
```

IoTHubTransport_MQTT_Common_GetHostname returns a STRING_HANDLE for the hostname.

**SRS_IOTHUB_TRANSPORT_MQTT_COMMON_02_001: [** If `handle` is NULL then `IoTHubTransport_MQTT_Common_GetHostname` shall fail and return NULL. **]**

**SRS_IOTHUB_TRANSPORT_MQTT_COMMON_02_002: [** Otherwise `IoTHubTransport_MQTT_Common_GetHostname` shall return a non-NULL STRING_HANDLE containg the hostname. **]**

