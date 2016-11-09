
# IoTHubTransport_AMQP_Common Requirements
================

## Overview

This module provides an implementation of the transport layer of the IoT Hub client based on the AMQP API, which implements the AMQP protocol.  
It is the base for the implentation of the actual respective AMQP transports, which will on their side provide the underlying I/O transport for this module.


## Exposed API

```c
typedef XIO_HANDLE(*AMQP_GET_IO_TRANSPORT)(const char* target_fqdn);

extern TRANSPORT_LL_HANDLE IoTHubTransport_AMQP_Common_Create(const IOTHUBTRANSPORT_CONFIG* config, AMQP_GET_IO_TRANSPORT get_io_transport);
extern void IoTHubTransport_AMQP_Common_Destroy(TRANSPORT_LL_HANDLE handle);
extern int IoTHubTransport_AMQP_Common_Subscribe(IOTHUB_DEVICE_HANDLE handle);
extern void IoTHubTransport_AMQP_Common_Unsubscribe(IOTHUB_DEVICE_HANDLE handle);
extern int IoTHubTransport_AMQP_Common_Subscribe_DeviceTwin(IOTHUB_DEVICE_HANDLE handle);
extern void IoTHubTransport_AMQP_Common_Unsubscribe_DeviceTwin(IOTHUB_DEVICE_HANDLE handle);
extern int IoTHubTransport_AMQP_Common_Subscribe_DeviceMethod(IOTHUB_DEVICE_HANDLE handle);
extern void IoTHubTransport_AMQP_Common_Unsubscribe_DeviceMethod(IOTHUB_DEVICE_HANDLE handle);
extern IOTHUB_PROCESS_ITEM_RESULT IoTHubTransport_AMQP_Common_ProcessItem(TRANSPORT_LL_HANDLE handle, IOTHUB_IDENTITY_TYPE item_type, IOTHUB_IDENTITY_INFO* iothub_item);
extern void IoTHubTransport_AMQP_Common_DoWork(TRANSPORT_LL_HANDLE handle, IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle);
extern IOTHUB_CLIENT_RESULT IoTHubTransport_AMQP_Common_GetSendStatus(IOTHUB_DEVICE_HANDLE handle, IOTHUB_CLIENT_STATUS* iotHubClientStatus);
extern IOTHUB_CLIENT_RESULT IoTHubTransport_AMQP_Common_SetOption(TRANSPORT_LL_HANDLE handle, const char* option, const void* value);
extern IOTHUB_DEVICE_HANDLE IoTHubTransport_AMQP_Common_Register(TRANSPORT_LL_HANDLE handle, const IOTHUB_DEVICE_CONFIG* device, IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle, PDLIST_ENTRY waitingToSend);
extern void IoTHubTransport_AMQP_Common_Unregister(IOTHUB_DEVICE_HANDLE deviceHandle);
extern STRING_HANDLE IoTHubTransport_AMQP_Common_GetHostname(TRANSPORT_LL_HANDLE handle);

```


### IoTHubTransport_AMQP_Common_GetHostname
```c
 STRING_HANDLE IoTHubTransport_AMQP_Common_GetHostname(TRANSPORT_LL_HANDLE handle)
```
`
IoTHubTransport_AMQP_Common_GetHostname provides a STRING_HANDLE containing the hostname with which the transport has been created.

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_02_001: [**If parameter `handle` is NULL then `IoTHubTransport_AMQP_Common_GetHostname` shall return NULL.**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_02_002: [**Otherwise IoTHubTransport_AMQP_Common_GetHostname shall return the target IoT Hub FQDN as a STRING_HANDLE.**]**


### IoTHubTransport_AMQP_Common_Create

```c
TRANSPORT_LL_HANDLE IoTHubTransport_AMQP_Common_Create(const IOTHUBTRANSPORT_CONFIG* config, AMQP_GET_IO_TRANSPORT get_io_transport)
```

This function creates all the inner components required by the IoT Hub client to work properly, returning a handle for a structure that represents it.

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_005: [**If parameter config or config->upperConfig are NULL then IoTHubTransport_AMQP_Common_Create shall fail and return NULL.**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_006: [**IoTHubTransport_AMQP_Common_Create shall fail and return NULL if any fields of the config->upperConfig structure are NULL.**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_134: [**IoTHubTransport_AMQP_Common_Create shall fail and return NULL if the combined length of config->iotHubName and config->iotHubSuffix exceeds 254 bytes (RFC1035)**]**
 
**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_254: [**If parameter `get_io_transport` is NULL then IoTHubTransport_AMQP_Common_Create shall fail and return NULL.**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_009: [**IoTHubTransport_AMQP_Common_Create shall fail and return NULL if memory allocation of the transport's internal state structure fails.**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_010: [**If config->upperConfig->protocolGatewayHostName is NULL, IoTHubTransport_AMQP_Common_Create shall create an immutable string, referred to as iotHubHostFqdn, from the following pieces: config->iotHubName + "." + config->iotHubSuffix.**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_20_001: [**If config->upperConfig->protocolGatewayHostName is not NULL, IoTHubTransport_AMQP_Common_Create shall use it as iotHubHostFqdn**]**


The below requirements only apply when authentication method is NOT x509:

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_020: [**IoTHubTransport_AMQP_Common_Create shall set parameter transport_state->sas_token_lifetime with the default value of 3600000 (milliseconds).**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_128: [**IoTHubTransport_AMQP_Common_Create shall set parameter transport_state->sas_token_refresh_time with the default value of sas_token_lifetime/2 (milliseconds).**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_129: [**IoTHubTransport_AMQP_Common_Create shall set parameter transport_state->cbs_request_timeout with the default value of 30000 (milliseconds).**]**
  
  
Summary of timeout parameters:

|Parameter	                    |Default Value            |
|-------------------------------|-------------------------|
|double sas_token_lifetime	    | 3600000 (milliseconds)  |
|double sas_token_refresh_time  | 1800000 (milliseconds)  |
|double cbs_request_timeout     | 30000 (milliseconds)    |

The below requirements apply independent of the authentication method:

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_236: [**If IoTHubTransport_AMQP_Common_Create fails it shall free any memory it allocated (iotHubHostFqdn, transport state).**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_023: [**If IoTHubTransport_AMQP_Common_Create succeeds it shall return a non-NULL pointer to the structure that represents the transport.**]**
  
 
### IoTHubTransport_AMQP_Common_Destroy

```c
void IoTHubTransport_AMQP_Common_Destroy(TRANSPORT_LL_HANDLE handle)
```

This function will close connection established through AMQP API, as well as destroy all the components allocated internally for its proper functionality.

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_209: [**IoTHubTransport_AMQP_Common_Destroy shall invoke IoTHubTransport_AMQP_Common_Unregister on each of its registered devices.**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_210: [**IoTHubTransport_AMQP_Common_Destroy shall its list of registered devices using VECTOR_destroy().**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_027: [**IoTHubTransport_AMQP_Common_Destroy shall destroy the AMQP cbs instance**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_030: [**IoTHubTransport_AMQP_Common_Destroy shall destroy the AMQP session.**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_031: [**IoTHubTransport_AMQP_Common_Destroy shall destroy the AMQP connection.**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_032: [**IoTHubTransport_AMQP_Common_Destroy shall destroy the AMQP SASL I/O transport.**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_033: [**IoTHubTransport_AMQP_Common_Destroy shall destroy the AMQP SASL mechanism.**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_034: [**IoTHubTransport_AMQP_Common_Destroy shall destroy the AMQP TLS I/O transport.**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_212: [**IoTHubTransport_AMQP_Common_Destroy shall destroy the IoTHub FQDN value saved on the transport instance**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_213: [**IoTHubTransport_AMQP_Common_Destroy shall destroy any TLS I/O options saved on the transport instance using OptionHandler_Destroy()**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_150: [**IoTHubTransport_AMQP_Common_Destroy shall destroy the transport instance**]**
  
### IoTHubTransport_AMQP_Common_DoWork

```c
void IoTHubTransport_AMQP_Common_DoWork(TRANSPORT_LL_HANDLE handle, IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle)
```
  
#### Parameter Verification

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_051: [**IoTHubTransport_AMQP_Common_DoWork shall fail and return immediately if the transport handle parameter is NULL**]**

#### General

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_237: [**IoTHubTransport_AMQP_Common_DoWork shall return immediately if there are no devices registered on the transport**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_238: [**If the transport state has a faulty connection state, IoTHubTransport_AMQP_Common_DoWork shall trigger the connection-retry logic**]**

#### Connection Establishment

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_055: [**If the transport handle has a NULL connection, IoTHubTransport_AMQP_Common_DoWork shall instantiate and initialize the AMQP components and establish the connection**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_110: [**IoTHubTransport_AMQP_Common_DoWork shall create the TLS I/O**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_136: [**If the creation of the TLS I/O transport fails, IoTHubTransport_AMQP_Common_DoWork shall fail and return immediately**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_239: [**IoTHubTransport_AMQP_Common_DoWork shall apply any TLS I/O saved options to the new TLS instance using OptionHandler_FeedOptions**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_240: [**If OptionHandler_FeedOptions succeeds, IoTHubTransport_AMQP_Common_DoWork shall destroy any TLS options saved on the transport state**]**


The below requirements only apply when the authentication type is NOT x509:

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_056: [**IoTHubTransport_AMQP_Common_DoWork shall create the SASL mechanism using AMQP's saslmechanism_create() API**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_057: [**If saslmechanism_create() fails, IoTHubTransport_AMQP_Common_DoWork shall fail and return immediately**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_060: [**IoTHubTransport_AMQP_Common_DoWork shall create the SASL I/O layer using the xio_create() C Shared Utility API**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_061: [**If xio_create() fails creating the SASL I/O layer, IoTHubTransport_AMQP_Common_DoWork shall fail and return immediately**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_062: [**IoTHubTransport_AMQP_Common_DoWork shall create the connection with the IoT service using connection_create2() AMQP API, passing the SASL I/O layer, IoT Hub FQDN and container ID as parameters (pass NULL for callbacks)**]**

Note: the following requirements apply after the AMQP session has been created and configured (see starting on SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_063 below).

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_066: [**IoTHubTransport_AMQP_Common_DoWork shall establish the CBS connection using the cbs_create() AMQP API**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_067: [**If cbs_create() fails, IoTHubTransport_AMQP_Common_DoWork shall fail and return immediately**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_139: [**IoTHubTransport_AMQP_Common_DoWork shall open the CBS connection using the cbs_open() AMQP API**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_140: [**If cbs_open() fails, IoTHubTransport_AMQP_Common_DoWork shall fail and return immediately**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_200: [**The value of the option `logtrace` saved by the transport instance shall be applied to each new SASL_IO instance using xio_setoption().**]**


The below requirement only apply when the authentication type is x509:

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_02_006: [** IoTHubTransport_AMQP_Common_DoWork shall not establish a CBS connection. **]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_02_005: [** IoTHubTransport_AMQP_Common_DoWork shall create the connection with the IoT service using connection_create2() AMQP API, passing the TLS I/O layer, IoT Hub FQDN and container ID as parameters (pass NULL for callbacks) **]**


The below requirements apply independent of the authentication mode:

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_064: [**IoTHubTransport_AMQP_Common_DoWork shall inquire the IoT hub for the preferred value for parameter 'AMQP incoming window', and set them on AMQP using session_set_incoming_window() if provided**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_201: [**The value of the option `logtrace` saved by the transport instance shall be applied to each new TLS_IO instance using xio_setoption().**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_199: [**The value of the option `logtrace` saved by the transport instance shall be applied to each new connection instance using connection_set_trace().**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_063: [**If connection_create2() fails, IoTHubTransport_AMQP_Common_DoWork shall fail and return immediately**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_137: [**IoTHubTransport_AMQP_Common_DoWork shall create the AMQP session session_create() AMQP API, passing the connection instance as parameter**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_138: [**If session_create() fails, IoTHubTransport_AMQP_Common_DoWork shall fail and return immediately**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_065: [**IoTHubTransport_AMQP_Common_DoWork shall apply a default value of UINT_MAX for the parameter 'AMQP incoming window'**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_115: [**IoTHubTransport_AMQP_Common_DoWork shall apply a default value of 100 for the parameter 'AMQP outgoing window'**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_119: [**IoTHubTransport_AMQP_Common_DoWork shall apply a default value of 65536 for the parameter 'Link MAX message size'**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_241: [**IoTHubTransport_AMQP_Common_DoWork shall iterate through all its registered devices to process authentication, events to be sent, messages to be received**]**

Summary of internal AMQP parameters:

|Parameter              |AMQP API for setting value     | Default value|
|-----------------------|-------------------------------|--------------|
|AMQP incoming window   |session_set_incoming_window    |INTMAX        |
|AMQP outgoing window   |session_set_outgoing_window    |100           |
|AMQP frame size        |connection_set_max_frame_size  |10            |
|Link MAX message size  |link_set_max_message_size      |65536         |


#### Per-Device DoWork Requirements

##### Authentication

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_243: [**IoTHubTransport_AMQP_Common_DoWork shall retrieve the authenticatication status of the device using authentication_get_status()**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_243: [**If the device authentication status is AUTHENTICATION_STATUS_IDLE, IoTHubTransport_AMQP_Common_DoWork shall authenticate it using authentication_authenticate()**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_146: [**If authentication_authenticate() fails, IoTHubTransport_AMQP_Common_DoWork shall fail and and process the next device**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_081: [**If the device authentication status is AUTHENTICATION_STATUS_REFRESH_REQUIRED, IoTHubTransport_AMQP_Common_DoWork shall refresh it using authentication_refresh()**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_082: [**If authentication_refresh() fails, IoTHubTransport_AMQP_Common_DoWork shall fail and process the next device**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_083: [**If the device authentication status is AUTHENTICATION_STATUS_FAILURE, IoTHubTransport_AMQP_Common_DoWork shall fail and process the next device**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_084: [**If the device authentication status is AUTHENTICATION_STATUS_TIMEOUT, IoTHubTransport_AMQP_Common_DoWork shall fail and process the next device**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_145: [**If the device authentication status is AUTHENTICATION_STATUS_OK, IoTHubTransport_AMQP_Common_DoWork shall proceed to sending events, registering for messages**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_244: [**If the device authentication status is AUTHENTICATION_STATUS_IN_PROGRESS, IoTHubTransport_AMQP_Common_DoWork shall skip and process the next device**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_024: [** If the device authentication status is AUTHENTICATION_STATUS_OK and `IoTHubTransport_AMQP_Common_Subscribe_DeviceMethod` was called to register for methods, `IoTHubTransport_AMQP_Common_DoWork` shall call `iothubtransportamqp_methods_subscribe`. **]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_027: [** The current session handle shall be passed to `iothubtransportamqp_methods_subscribe`. **]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_031: [** `iothubtransportamqp_methods_subscribe` shall only be called once (subsequent DoWork calls shall not call it if already subscribed). **]**  

#### Creation of the message sender

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_068: [**IoTHubTransport_AMQP_Common_DoWork shall create the AMQP link using link_create(), with role as 'role_sender'**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_251: [**Every new message_sender AMQP link shall be created using unique link and source names per device, per connection**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_252: [**The message_sender AMQP link shall be created using the `target` address created according to SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_014**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_069: [**If IoTHubTransport_AMQP_Common_DoWork fails to create the AMQP link for sending messages, the function shall fail and return immediately, flagging the connection to be re-stablished**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_249: [**The message sender link should have a property set with the type and version of the IoT Hub client application, set as `CLIENT_DEVICE_TYPE_PREFIX/IOTHUB_SDK_VERSION`**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_250: [**If the message sender link fails to have the client type and version set on its properties, the failure shall be ignored**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_070: [**IoTHubTransport_AMQP_Common_DoWork shall create the AMQP message sender using messagesender_create() AMQP API**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_191: [**IoTHubTransport_AMQP_Common_DoWork shall create each AMQP message sender tracking its state changes with a callback function**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_192: [**If a message sender instance changes its state to MESSAGE_SENDER_STATE_ERROR (first transition only) the connection retry logic shall be triggered**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_071: [**IoTHubTransport_AMQP_Common_DoWork shall fail and return immediately if the AMQP message sender instance fails to be created, flagging the connection to be re-established**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_072: [**IoTHubTransport_AMQP_Common_DoWork shall open the AMQP message sender using messagesender_open() AMQP API**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_073: [**IoTHubTransport_AMQP_Common_DoWork shall fail and return immediately if the AMQP message sender instance fails to be opened, flagging the connection to be re-established**]**


##### Creation and Destruction of the message receiver

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_121: [**IoTHubTransport_AMQP_Common_DoWork shall create an AMQP message_receiver if transport_state->message_receive is NULL and transport_state->receive_messages is true**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_122: [**IoTHubTransport_AMQP_Common_DoWork shall destroy the transport_state->message_receiver (and set it to NULL) if it exists and transport_state->receive_messages is false**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_074: [**IoTHubTransport_AMQP_Common_DoWork shall create the AMQP link using link_create(), with role as 'role_receiver'**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_246: [**Every new message_receiver AMQP link shall be created using unique link and target names per device, per connection**]** 

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_253: [**The message_receiver AMQP link shall be created using the `source` address created according to SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_053**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_075: [**If IoTHubTransport_AMQP_Common_DoWork fails to create the AMQP link for receiving messages, the function shall fail and return immediately, flagging the connection to be re-stablished**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_076: [**IoTHubTransport_AMQP_Common_DoWork shall set the receiver link settle mode as receiver_settle_mode_first**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_141: [**If IoTHubTransport_AMQP_Common_DoWork fails to set the settle mode on the AMQP link for receiving messages, the function shall fail and return immediately, flagging the connection to be re-stablished**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_247: [**The message receiver link should have a property set with the type and version of the IoT Hub client application, set as `CLIENT_DEVICE_TYPE_PREFIX/IOTHUB_SDK_VERSION`**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_248: [**If the message receiver link fails to have the client type and version set on its properties, the failure shall be ignored**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_077: [**IoTHubTransport_AMQP_Common_DoWork shall create the AMQP message receiver using messagereceiver_create() AMQP API**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_189: [**IoTHubTransport_AMQP_Common_DoWork shall create each AMQP message receiver tracking its state changes with a callback function**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_190: [**If a message receiver instance changes its state to MESSAGE_RECEIVER_STATE_ERROR (first transition only) the connection retry logic shall be triggered**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_078: [**IoTHubTransport_AMQP_Common_DoWork shall fail and return immediately if the AMQP message receiver instance fails to be created, flagging the connection to be re-established**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_079: [**IoTHubTransport_AMQP_Common_DoWork shall open the AMQP message receiver using messagereceiver_open() AMQP API, passing a callback function for handling C2D incoming messages**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_123: [**IoTHubTransport_AMQP_Common_DoWork shall open each AMQP message_receiver passing the 'on_message_received' as the callback function**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_080: [**IoTHubTransport_AMQP_Common_DoWork shall fail and return immediately if the AMQP message receiver instance fails to be opened, flagging the connection to be re-established**]**


This section defines the functionality of the callback function 'on_message_received' (passed to AMQP message receiver).

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_195: [**The callback 'on_message_received' shall shall get a IOTHUB_MESSAGE_HANDLE instance out of the uamqp's MESSAGE_HANDLE instance by using IoTHubMessage_CreateFromUamqpMessage()**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_196: [**If IoTHubMessage_CreateFromUamqpMessage fails, the callback 'on_message_received' shall reject the incoming message by calling messaging_delivery_rejected() and return.**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_104: [**The callback 'on_message_received' shall invoke IoTHubClient_LL_MessageCallback() passing the client and the incoming message handles as parameters**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_197: [**The callback 'on_message_received' shall destroy the IOTHUB_MESSAGE_HANDLE instance after invoking IoTHubClient_LL_MessageCallback().**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_105: [**The callback 'on_message_received' shall return the result of messaging_delivery_accepted() if the IoTHubClient_LL_MessageCallback() returns IOTHUBMESSAGE_ACCEPTED**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_106: [**The callback 'on_message_received' shall return the result of messaging_delivery_released() if the IoTHubClient_LL_MessageCallback() returns IOTHUBMESSAGE_ABANDONED**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_107: [**The callback 'on_message_received' shall return the result of messaging_delivery_rejected(“Rejected by application”, “Rejected by application”) if the IoTHubClient_LL_MessageCallback() returns IOTHUBMESSAGE_REJECTED**]**
  

  
##### Sending Events

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_245: [**IoTHubTransport_AMQP_Common_DoWork shall skip sending events if the state of the message_sender is not MESSAGE_SENDER_STATE_OPEN**]**

Note: 
SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_245 is specially important for device multiplexing scenarios.
On a given connection, there must not be messages sent to the Hub before the device has been authenticated, despite if other multiplexed devices have already been authenticated on such connection.

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_086: [**IoTHubTransport_AMQP_Common_DoWork shall move queued events to an “in-progress” list right before processing them for sending**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_193: [**IoTHubTransport_AMQP_Common_DoWork shall get a MESSAGE_HANDLE instance out of the event's IOTHUB_MESSAGE_HANDLE instance by using message_create_from_iothub_message().**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_111: [**If message_create_from_iothub_message() fails, IoTHubTransport_AMQP_Common_DoWork notify the failure, roll back the event to waitToSend list and return**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_097: [**IoTHubTransport_AMQP_Common_DoWork shall pass the MESSAGE_HANDLE intance to uAMQP for sending (along with on_message_send_complete callback) using messagesender_send()**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_113: [**If messagesender_send() fails, IoTHubTransport_AMQP_Common_DoWork notify the failure, roll back the event to waitToSend list and return**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_194: [**IoTHubTransport_AMQP_Common_DoWork shall destroy the MESSAGE_HANDLE instance after messagesender_send() is invoked.**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_100: [**The callback 'on_message_send_complete' shall remove the target message from the in-progress list after the upper layer callback**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_142: [**The callback 'on_message_send_complete' shall pass to the upper layer callback an IOTHUB_CLIENT_CONFIRMATION_OK if the result received is MESSAGE_SEND_OK**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_143: [**The callback 'on_message_send_complete' shall pass to the upper layer callback an IOTHUB_CLIENT_CONFIRMATION_ERROR if the result received is MESSAGE_SEND_ERROR**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_102: [**The callback 'on_message_send_complete' shall invoke the upper layer callback for message received if provided**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_151: [**The callback 'on_message_send_complete' shall destroy the message handle (IOTHUB_MESSAGE_HANDLE) using IoTHubMessage_Destroy()**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_152: [**The callback 'on_message_send_complete' shall destroy the IOTHUB_MESSAGE_LIST instance**]**
  

#### General

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_103: [**IoTHubTransport_AMQP_Common_DoWork shall invoke connection_dowork() on AMQP for triggering sending and receiving messages**]**


### IoTHubTransport_AMQP_Common_Register

```c
IOTHUB_DEVICE_HANDLE IoTHubTransport_AMQP_Common_Register(TRANSPORT_LL_HANDLE handle, const IOTHUB_DEVICE_CONFIG* device, IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle, PDLIST_ENTRY waitingToSend)
```

This function registers a device with the transport.  The AMQP transport only supports a single device established on create, so this function will prevent multiple devices from being registered.

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_17_005: [**IoTHubTransport_AMQP_Common_Register shall return NULL if the TRANSPORT_LL_HANDLE is NULL.**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_17_001: [**IoTHubTransport_AMQP_Common_Register shall return NULL if device, or waitingToSend are NULL.**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_220: [**IoTHubTransport_AMQP_Common_Register shall fail and return NULL if the IOTHUB_CLIENT_LL_HANDLE is NULL.**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_03_002: [**IoTHubTransport_AMQP_Common_Register shall return NULL if deviceId is NULL.**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_03_003: [**IoTHubTransport_AMQP_Common_Register shall return NULL if both deviceKey and deviceSasToken are not NULL.**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_221: [**IoTHubTransport_AMQP_Common_Register shall fail and return NULL if the device is not using an authentication mode compatible with the currently used by the transport.**]**

Note: There should be no devices using different authentication modes registered on the transport at the same time (i.e., either all registered devices use CBS authentication, or all use x509 certificate authentication). 

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_222: [**If a device matching the deviceId provided is already registered, IoTHubTransport_AMQP_Common_Register shall fail and return NULL.**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_223: [**IoTHubTransport_AMQP_Common_Register shall allocate an instance of AMQP_TRANSPORT_DEVICE_STATE to store the state of the new registered device.**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_224: [**If malloc fails to allocate memory for AMQP_TRANSPORT_DEVICE_STATE, IoTHubTransport_AMQP_Common_Register shall fail and return NULL.**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_225: [**IoTHubTransport_AMQP_Common_Register shall save the handle references to the IoTHubClient, transport, waitingToSend list on the device state.**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_226: [**IoTHubTransport_AMQP_Common_Register shall initialize the device state inProgress list using DList_InitializeListHead().**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_227: [**IoTHubTransport_AMQP_Common_Register shall store a copy of config->deviceId into device_state->deviceId.**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_228: [**If STRING_construct fails to copy config->deviceId, IoTHubTransport_AMQP_Common_Register shall fail and return NULL.**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_012: [**IoTHubTransport_AMQP_Common_Register shall create an immutable string, referred to as devicesPath, from the following parts: host_fqdn + “/devices/” + deviceId.**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_013: [**If creating devicesPath fails for any reason then IoTHubTransport_AMQP_Common_Register shall fail and return NULL.**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_229: [**IoTHubTransport_AMQP_Common_Register shall create an authentication state for the device using authentication_create() and store it on the device state.**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_230: [**If authentication_create() fails, IoTHubTransport_AMQP_Common_Register shall fail and return NULL.**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_014: [**IoTHubTransport_AMQP_Common_Register shall create an immutable string, referred to as targetAddress, from the following parts: "amqps://" + devicesPath + "/messages/events".**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_015: [**If creating the targetAddress fails for any reason then IoTHubTransport_AMQP_Common_Register shall fail and return NULL.**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_053: [**IoTHubTransport_AMQP_Common_Register shall define the source address for receiving messages as
"amqps://" + devicesPath + "/messages/devicebound", stored in the transport handle as messageReceiveAddress**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_054: [**If creating the messageReceiveAddress fails for any reason then IoTHubTransport_AMQP_Common_Register shall fail and return NULL.**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_010: [** `IoTHubTransport_AMQP_Common_Register` shall create a new iothubtransportamqp_methods instance by calling `iothubtransportamqp_methods_create` while passing to it the the fully qualified domain name and the device Id. **]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_011: [** If `iothubtransportamqp_methods_create` fails, `IoTHubTransport_AMQP_Common_Register` shall fail and return NULL. **]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_231: [**IoTHubTransport_AMQP_Common_Register shall add the device to transport_state->registered_devices using VECTOR_push_back().**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_232: [**If VECTOR_push_back() fails to add the new registered device, IoTHubTransport_AMQP_Common_Register shall clean the memory it allocated, fail and return NULL.**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_234: [**If the device is the first being registered on the transport, IoTHubTransport_AMQP_Common_Register shall save its authentication mode as the transport preferred authentication mode.**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_233: [**If IoTHubTransport_AMQP_Common_Register fails, it shall free all memory it alloacated (destroy deviceId, authentication state, targetAddress, messageReceiveAddress, devicesPath, device state).**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_233: [**IoTHubTransport_AMQP_Common_Register shall return its internal device representation as a IOTHUB_DEVICE_HANDLE.**]**


### IoTHubTransport_AMQP_Common_Unregister

```c
void IoTHubTransport_AMQP_Common_Unregister(IOTHUB_DEVICE_HANDLE deviceHandle)
```

This function is intended to remove a device if it is registered with the transport.  

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_214: [**IoTHubTransport_AMQP_Common_Unregister should fail and return if the IOTHUB_DEVICE_HANDLE parameter provided is NULL.**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_215: [**IoTHubTransport_AMQP_Common_Unregister should fail and return if the IOTHUB_DEVICE_HANDLE parameter provided has a NULL reference to its transport instance.**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_216: [**IoTHubTransport_AMQP_Common_Unregister should fail and return if the device is not registered with this transport.**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_024: [**IoTHubTransport_AMQP_Common_Unregister shall destroy the AMQP message_sender.**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_029: [**IoTHubTransport_AMQP_Common_Unregister shall destroy the AMQP message_sender link.**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_025: [**IoTHubTransport_AMQP_Common_Unregister shall destroy the AMQP message_receiver.**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_211: [**IoTHubTransport_AMQP_Common_Unregister shall destroy the AMQP message_receiver link.**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_036: [**IoTHubTransport_AMQP_Common_Unregister shall return the remaining items in inProgress to waitingToSend list.**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_035: [**IoTHubTransport_AMQP_Common_Unregister shall delete its internally-set parameters (targetAddress, messageReceiveAddress, devicesPath, deviceId).**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_217: [**IoTHubTransport_AMQP_Common_Unregister shall destroy the authentication state of the device using authentication_destroy.**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_012: [**IoTHubTransport_AMQP_Common_Unregister shall destroy the C2D methods handler by calling iothubtransportamqp_methods_destroy.**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_197: [**IoTHubTransport_AMQP_Common_Unregister shall remove the device from its list of registered devices using VECTOR_erase().**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_218: [**IoTHubTransport_AMQP_Common_Unregister shall remove the device from its list of registered devices using VECTOR_erase().**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_219: [**IoTHubTransport_AMQP_Common_Unregister shall destroy the IOTHUB_DEVICE_HANDLE instance provided.**]**


### IoTHubTransport_AMQP_Common_Subscribe

```c
int IoTHubTransport_AMQP_Common_Subscribe(IOTHUB_DEVICE_HANDLE handle)
```

This function enables the transport to notify the upper client layer of new messages received from the cloud to the device.

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_037: [**IoTHubTransport_AMQP_Common_Subscribe shall fail if the transport handle parameter received is NULL.**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_038: [**IoTHubTransport_AMQP_Common_Subscribe shall set transport_handle->receive_messages to true and return success code.**]**
  

### IoTHubTransport_AMQP_Common_Unsubscribe

```c
void IoTHubTransport_AMQP_Common_Unsubscribe(IOTHUB_DEVICE_HANDLE handle)
```

This function disables the notifications to the upper client layer of new messages received from the cloud to the device.

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_039: [**IoTHubTransport_AMQP_Common_Unsubscribe shall fail if the transport handle parameter received is NULL.**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_040: [**IoTHubTransport_AMQP_Common_Unsubscribe shall set transport_handle->receive_messages to false.**]**
  
  
### IoTHubTransport_AMQP_Common_GetSendStatus

```c
IOTHUB_CLIENT_RESULT IoTHubTransport_AMQP_Common_GetSendStatus(IOTHUB_DEVICE_HANDLE handle, IOTHUB_CLIENT_STATUS *iotHubClientStatus)
```

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_041: [**IoTHubTransport_AMQP_Common_GetSendStatus shall return IOTHUB_CLIENT_INVALID_ARG if called with NULL parameter.**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_042: [**IoTHubTransport_AMQP_Common_GetSendStatus shall return IOTHUB_CLIENT_OK and status IOTHUB_CLIENT_SEND_STATUS_IDLE if there are currently no event items to be sent or being sent.**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_043: [**IoTHubTransport_AMQP_Common_GetSendStatus shall return IOTHUB_CLIENT_OK and status IOTHUB_CLIENT_SEND_STATUS_BUSY if there are currently event items to be sent or being sent.**]**
  
  
  
### IoTHubTransport_AMQP_Common_SetOption

```c
IOTHUB_CLIENT_RESULT IoTHubTransport_AMQP_Common_SetOption(TRANSPORT_LL_HANDLE handle, const char* option, const void* value)
```

Summary of options:

|Parameter              |Possible Values               |Details                                          |
|-----------------------|------------------------------|-------------------------------------------------|
|TrustedCerts           |                              |Sets the certificate to be used by the transport.|
|sas_token_lifetime     | 0 to TIME_MAX (milliseconds) |Default: 3600000 milliseconds (1 hour)	How long a SAS token created by the transport is valid, in milliseconds.|
|sas_token_refresh_time | 0 to TIME_MAX (milliseconds) |Default: sas_token_lifetime/2	Maximum period of time for the transport to wait before refreshing the SAS token it created previously.|
|cbs_request_timeout    | 1 to TIME_MAX (milliseconds) |Default: 30 millisecond	Maximum time the transport waits for AMQP cbs_put_token() to complete before marking it a failure.|
|x509certificate        | const char*                  |Default: NONE. An x509 certificate in PEM format |
|x509privatekey         | const char*                  |Default: NONE. An x509 RSA private key in PEM format|
|logtrace               | true or false                |Default: false|


**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_044: [**If handle parameter is NULL then IoTHubTransport_AMQP_Common_SetOption shall return IOTHUB_CLIENT_INVALID_ARG.**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_045: [**If parameter optionName is NULL then IoTHubTransport_AMQP_Common_SetOption shall return IOTHUB_CLIENT_INVALID_ARG.**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_046: [**If parameter value is NULL then IoTHubTransport_AMQP_Common_SetOption shall return IOTHUB_CLIENT_INVALID_ARG.**]**


The following requirements only apply if the authentication is NOT x509:

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_048: [**IoTHubTransport_AMQP_Common_SetOption shall save and apply the value if the option name is "sas_token_lifetime", returning IOTHUB_CLIENT_OK**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_049: [**IoTHubTransport_AMQP_Common_SetOption shall save and apply the value if the option name is "sas_token_refresh_time", returning IOTHUB_CLIENT_OK**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_148: [**IoTHubTransport_AMQP_Common_SetOption shall save and apply the value if the option name is "cbs_request_timeout", returning IOTHUB_CLIENT_OK**]**


The following requirements only apply to x509 authentication:

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_02_007: [** If `optionName` is `x509certificate` and the authentication method is not x509 then `IoTHubTransport_AMQP_Common_SetOption` shall return IOTHUB_CLIENT_INVALID_ARG. **]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_02_008: [** If `optionName` is `x509privatekey` and the authentication method is not x509 then `IoTHubTransport_AMQP_Common_SetOption` shall return IOTHUB_CLIENT_INVALID_ARG. **]**


The remaining requirements apply independent of the authentication mode:

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_198: [**If `optionName` is `logtrace`, IoTHubTransport_AMQP_Common_SetOption shall save the value on the transport instance.**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_202: [**If `optionName` is `logtrace`, IoTHubTransport_AMQP_Common_SetOption shall apply it using connection_set_trace() to current connection instance if it exists and return IOTHUB_CLIENT_OK.**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_203: [**If `optionName` is `logtrace`, IoTHubTransport_AMQP_Common_SetOption shall apply it using xio_setoption() to current SASL IO instance if it exists.**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_204: [**If xio_setoption() fails, IoTHubTransport_AMQP_Common_SetOption shall fail and return IOTHUB_CLIENT_ERROR.**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_205: [**If xio_setoption() succeeds, IoTHubTransport_AMQP_Common_SetOption shall return IOTHUB_CLIENT_OK.**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_047: [**If the option name does not match one of the options handled by this module, IoTHubTransport_AMQP_Common_SetOption shall pass the value and name to the XIO using xio_setoption().**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_206: [**If the TLS IO does not exist, IoTHubTransport_AMQP_Common_SetOption shall create it and save it on the transport instance.**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_207: [**If IoTHubTransport_AMQP_Common_SetOption fails creating the TLS IO instance, it shall fail and return IOTHUB_CLIENT_ERROR.**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_208: [**When a new TLS IO instance is created, IoTHubTransport_AMQP_Common_SetOption shall apply the TLS I/O Options with OptionHandler_FeedOptions() if it is has any saved.**]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_03_001: [**If xio_setoption fails,  IoTHubTransport_AMQP_Common_SetOption shall return IOTHUB_CLIENT_ERROR.**]**

### IoTHubTransport_AMQP_Common_Subscribe_DeviceTwin
```c
int IoTHubTransport_AMQP_Common_Subscribe_DeviceTwin(IOTHUB_DEVICE_HANDLE handle, IOTHUB_DEVICE_TWIN_STATE subscribe_state)
```

`IoTHubTransport_AMQP_Common_Subscribe_DeviceTwin` subscribes to DeviceTwin's messages. Not implemented at the moment.

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_02_009: [** `IoTHubTransport_AMQP_Common_Subscribe_DeviceTwin` shall return a non-zero value. **]**

### IoTHubTransport_AMQP_Common_Unsubscribe_DeviceTwin
```c
void IoTHubTransport_AMQP_Common_Unsubscribe_DeviceTwin(IOTHUB_DEVICE_HANDLE handle, IOTHUB_DEVICE_TWIN_STATE subscribe_state)
```

`IoTHubTransport_AMQP_Common_Unsubscribe_DeviceTwin` unsubscribes from DeviceTwin's messages. Not implemented.

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_02_010: [** `IoTHubTransport_AMQP_Common_Unsubscribe_DeviceTwin` shall return. **]**


### IoTHubTransport_AMQP_Common_Subscribe_DeviceMethod
```c
int IoTHubTransport_AMQP_Common_Subscribe_DeviceMethod(IOTHUB_DEVICE_HANDLE handle)
```

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_026: [** `IoTHubTransport_AMQP_Common_Subscribe_DeviceMethod` shall remember that a subscribe is to be performed in the next call to DoWork and on success it shall return 0. **]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_004: [** If `handle` is NULL, `IoTHubTransport_AMQP_Common_Subscribe_DeviceMethod` shall fail and return a non-zero value. **]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_005: [** If the transport is already subscribed to receive C2D method requests, `IoTHubTransport_AMQP_Common_Subscribe_DeviceMethod` shall perform no additional action and return 0. **]**

### IoTHubTransport_AMQP_Common_Unsubscribe_DeviceMethod
```c
void IoTHubTransport_AMQP_Common_Unsubscribe_DeviceMethod(IOTHUB_DEVICE_HANDLE handle)
```

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_006: [** If `handle` is NULL, `IoTHubTransport_AMQP_Common_Unsubscribe_DeviceMethod` shall do nothing. **]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_007: [** `IoTHubTransport_AMQP_Common_Unsubscribe_DeviceMethod` shall unsubscribe from receiving C2D method requests by calling `iothubtransportamqp_methods_unsubscribe`. **]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_008: [** If the transport is not subscribed to receive C2D method requests then `IoTHubTransport_AMQP_Common_Unsubscribe_DeviceMethod` shall do nothing. **]**

### on_methods_request_received

```c
void on_methods_request_received(void* context, const char* method_name, const unsigned char* request, size_t request_size, IOTHUBTRANSPORT_AMQP_METHOD_HANDLE method_handle);
```

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_016: [** `on_methods_request_received` shall create a BUFFER_HANDLE by calling `BUFFER_new`. **]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_028: [** On success, `on_methods_request_received` shall return 0. **]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_025: [** If creating the buffer fails, on_methods_request_received shall fail and return a non-zero value. **]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_017: [** `on_methods_request_received` shall call the `IoTHubClient_LL_DeviceMethodComplete` passing the method name, request buffer and size and the newly created BUFFER handle. **]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_019: [** `on_methods_request_received` shall call `iothubtransportamqp_methods_respond` passing to it the `method_handle` argument, the response bytes, response size and the status code. **]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_020: [** The response bytes shall be obtained by calling `BUFFER_u_char`. **]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_021: [** The response size shall be obtained by calling `BUFFER_length`. **]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_022: [** The status code shall be the return value of the call to `IoTHubClient_LL_DeviceMethodComplete`. **]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_023: [** After calling `iothubtransportamqp_methods_respond`, the allocated buffer shall be freed by using BUFFER_delete. **]**

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_029: [** If `iothubtransportamqp_methods_respond` fails, `on_methods_request_received` shall return a non-zero value. **]**

###on_methods_error

```c
void on_methods_error(void* context)
```

**SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_030: [** `on_methods_error` shall do nothing. **]**