<style>
table, td, th {
	text-align:left;
	border:1px solid white;
	border-collapse: collapse
}
</style>

<!-- 
Last requirement IDs:
09: 152
-->

#IoTHubTransportAMQP Requirements


 
##Overview
This module provides an implementation of the transport layer of the IoT Hub client based on the AMQP API, which implements the AMQP protocol.  
Access to the static functions described in this document is possible through the set of function pointers provided by the function AMQP_Protocol.  
   
   
##Exposed API

```c
static TRANSPORT_LL_HANDLE IoTHubTransportAMQP_Create(const IOTHUBTRANSPORT_CONFIG* config)

static void IoTHubTransportAMQP_Destroy(TRANSPORT_LL_HANDLE handle)

static void IoTHubTransportAMQP_DoWork(TRANSPORT_LL_HANDLE handle, IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle)

static IOTHUB_DEVICE_HANDLE IoTHubTransportAMQP_Register(TRANSPORT_LL_HANDLE handle, const char* deviceId, const char* deviceKey, PDLIST_ENTRY waitingToSend)

static void IoTHubTransportAMQP_Unregister(IOTHUB_DEVICE_HANDLE deviceHandle)

static int IoTHubTransportAMQP_Subscribe(IOTHUB_DEVICE_HANDLE handle)

static void IoTHubTransportAMQP_Unsubscribe(IOTHUB_DEVICE_HANDLE handle)

static IOTHUB_CLIENT_RESULT IoTHubTransportAMQP_GetSendStatus(IOTHUB_DEVICE_HANDLE handle, IOTHUB_CLIENT_STATUS *iotHubClientStatus)

static IOTHUB_CLIENT_RESULT IoTHubTransportAMQP_SetOption(TRANSPORT_LL_HANDLE handle, const char* option, const void* value);
```
  
  
  
 ###IoTHubTransportAMQP_Create

This function creates all the inner components required by the IoT Hub client to work properly, returning a handle for a structure that represents it.

**SRS_IOTHUBTRANSPORTAMQP_09_005: [**If parameter config (or its fields) is NULL then IoTHubTransportAMQP_Create shall fail and return NULL.**]**

**SRS_IOTHUBTRANSPORTAMQP_09_006: [**IoTHubTransportAMQP_Create shall fail and return NULL if any fields of the config structure are NULL.**]**

**SRS_IOTHUBTRANSPORTAMQP_09_007: [**IoTHubTransportAMQP_Create shall fail and return NULL if the deviceId length is greater than 128.**]**
 
**SRS_IOTHUBTRANSPORTAMQP_09_008: [**IoTHubTransportAMQP_Create shall fail and return NULL if any config field of type string is zero length.**]**

**SRS_IOTHUBTRANSPORTAMQP_09_134: [**IoTHubTransportAMQP_Create shall fail and return NULL if the combined length of config->iotHubName and config->iotHubSuffix exceeds 254 bytes (RFC1035)**]**
 
**SRS_IOTHUBTRANSPORTAMQP_09_009: [**IoTHubTransportAMQP_Create shall fail and return NULL if memory allocation of the transport’s internal state structure fails.**]**

**SRS_IOTHUBTRANSPORTAMQP_09_010: [**IoTHubTransportAMQP_Create shall create an immutable string, referred to as iotHubHostFqdn, from the following pieces: config->iotHubName + "." + config->iotHubSuffix.**]**

**SRS_IOTHUBTRANSPORTAMQP_09_012: [**IoTHubTransportAMQP_Create shall create an immutable string, referred to as devicesPath, from the following parts: host_fqdn + “/devices/” + deviceId.**]**

**SRS_IOTHUBTRANSPORTAMQP_09_013: [**If creating devicesPath fails for any reason then IoTHubTransportAMQP_Create shall fail and return NULL.**]**

**SRS_IOTHUBTRANSPORTAMQP_09_014: [**IoTHubTransportAMQP_Create shall create an immutable string, referred to as targetAddress, from the following parts: "amqps://" + devicesPath + "/messages/events".**]**

**SRS_IOTHUBTRANSPORTAMQP_09_015: [**If creating the targetAddress fails for any reason then IoTHubTransportAMQP_Create shall fail and return NULL.**]**

**SRS_IOTHUBTRANSPORTAMQP_09_053: [**IoTHubTransportAMQP_Create shall define the source address for receiving messages as
"amqps://" + devicesPath + "/messages/devicebound", stored in the transport handle as messageReceiveAddress**]**

**SRS_IOTHUBTRANSPORTAMQP_09_054: [**If creating the messageReceiveAddress fails for any reason then IoTHubTransportAMQP_Create shall fail and return NULL.**]**

**SRS_IOTHUBTRANSPORTAMQP_09_016: [**IoTHubTransportAMQP_Create shall initialize handle->sasTokenKeyName with a zero-length STRING_HANDLE instance.**]**

**SRS_IOTHUBTRANSPORTAMQP_09_017: [**If IoTHubTransportAMQP_Create fails to initialize handle->sasTokenKeyName with a zero-length STRING the function shall fail and return NULL.**]**

**SRS_IOTHUBTRANSPORTAMQP_09_018: [**IoTHubTransportAMQP_Create shall store a copy of config->deviceKey (passed by upper layer) into the transport’s own deviceKey field**]**

**SRS_IOTHUBTRANSPORTAMQP_09_135: [**If creating the config->deviceKey fails for any reason then IoTHubTransportAMQP_Create shall fail and return NULL.**]**

**SRS_IOTHUBTRANSPORTAMQP_09_019: [**If IoTHubTransportAMQP_Create fails to copy config->deviceKey, the function shall fail and return NULL.**]**

**SRS_IOTHUBTRANSPORTAMQP_09_020: [**IoTHubTransportAMQP_Create shall set parameter transport_state->sas_token_lifetime with the default value of 3600000 (milliseconds).**]**

**SRS_IOTHUBTRANSPORTAMQP_09_128: [**IoTHubTransportAMQP_Create shall set parameter transport_state->sas_token_refresh_time with the default value of sas_token_lifetime/2 (milliseconds).**]**

**SRS_IOTHUBTRANSPORTAMQP_09_129: [**IoTHubTransportAMQP_Create shall set parameter transport_state->cbs_request_timeout with the default value of 30000 (milliseconds).**]**
  
  
Summary of timeout parameters:

Parameter	Default Value
double sas_token_lifetime	3600000 (milliseconds)
double sas_token_refresh_time	1800000 (milliseconds)
double cbs_request_timeout	30000 (milliseconds)


**SRS_IOTHUBTRANSPORTAMQP_09_023: [**If IoTHubTransportAMQP_Create succeeds it shall return a non-NULL pointer to the structure that represents the transport.**]**
  
  
  
###IoTHubTransportAMQP_Destroy

This function will close connection established through AMQP API, as well as destroy all the components allocated internally for its proper functionality.

**SRS_IOTHUBTRANSPORTAMQP_09_024: [**IoTHubTransportAMQP_Destroy shall destroy the AMQP message_sender.**]**

**SRS_IOTHUBTRANSPORTAMQP_09_025: [**IoTHubTransportAMQP_Destroy shall destroy the AMQP message_receiver.**]**


**SRS_IOTHUBTRANSPORTAMQP_09_027: [**IoTHubTransportAMQP_Destroy shall destroy the AMQP cbs instance**]**

**SRS_IOTHUBTRANSPORTAMQP_09_029: [**IoTHubTransportAMQP_Destroy shall destroy the AMQP link.**]**

**SRS_IOTHUBTRANSPORTAMQP_09_030: [**IoTHubTransportAMQP_Destroy shall destroy the AMQP session.**]**

**SRS_IOTHUBTRANSPORTAMQP_09_031: [**IoTHubTransportAMQP_Destroy shall destroy the AMQP connection.**]**

**SRS_IOTHUBTRANSPORTAMQP_09_032: [**IoTHubTransportAMQP_Destroy shall destroy the AMQP SASL I/O transport.**]**

**SRS_IOTHUBTRANSPORTAMQP_09_033: [**IoTHubTransportAMQP_Destroy shall destroy the AMQP SASL mechanism.**]**

**SRS_IOTHUBTRANSPORTAMQP_09_034: [**IoTHubTransportAMQP_Destroy shall destroy the AMQP TLS I/O transport.**]**

**SRS_IOTHUBTRANSPORTAMQP_09_035: [**IoTHubTransportAMQP_Destroy shall delete its internally-set parameters (deviceKey, targetAddress, devicesPath, sasTokenKeyName).**]**

**SRS_IOTHUBTRANSPORTAMQP_09_036: [**IoTHubTransportAMQP_Destroy shall return the remaining items in inProgress to waitingToSend list.**]**

**SRS_IOTHUBTRANSPORTAMQP_09_150: [**IoTHubTransportAMQP_Destroy shall destroy the transport instance**]**
  
  
  
###IoTHubTransportAMQP_DoWork
  
  
####Parameter Verification

**SRS_IOTHUBTRANSPORTAMQP_09_051: [**IoTHubTransportAMQP_DoWork shall fail and return immediately if the transport handle parameter is NULL**]**

**SRS_IOTHUBTRANSPORTAMQP_09_052: [**IoTHubTransportAMQP_DoWork shall fail and return immediately if the client handle parameter is NULL**]**

**SRS_IOTHUBTRANSPORTAMQP_09_147: [**IoTHubTransportAMQP_DoWork shall save a reference to the client handle in transport_state->iothub_client_handle**]**
  
  
####Connection Establishment

**SRS_IOTHUBTRANSPORTAMQP_09_055: [**If the transport handle has a NULL connection, IoTHubTransportAMQP_DoWork shall instantiate and initialize the AMQP components and establish the connection**]**

**SRS_IOTHUBTRANSPORTAMQP_09_110: [**IoTHubTransportAMQP_DoWork shall create the TLS I/O**]**

**SRS_IOTHUBTRANSPORTAMQP_09_136: [**If the creation of the TLS I/O transport fails, IoTHubTransportAMQP_DoWork shall fail and return immediately**]**

**SRS_IOTHUBTRANSPORTAMQP_09_056: [**IoTHubTransportAMQP_DoWork shall create the SASL mechanism using AMQP’s saslmechanism_create() API**]**

**SRS_IOTHUBTRANSPORTAMQP_09_057: [**If saslmechanism_create() fails, IoTHubTransportAMQP_DoWork shall fail and return immediately**]**

**SRS_IOTHUBTRANSPORTAMQP_09_060: [**IoTHubTransportAMQP_DoWork shall create the SASL I/O layer using the xio_create() C Shared Utility API**]**

**SRS_IOTHUBTRANSPORTAMQP_09_061: [**If xio_create() fails creating the SASL I/O layer, IoTHubTransportAMQP_DoWork shall fail and return immediately**]**

**SRS_IOTHUBTRANSPORTAMQP_09_062: [**IoTHubTransportAMQP_DoWork shall create the connection with the IoT service using connection_create2() AMQP API, passing the SASL I/O layer, IoT Hub FQDN and container ID as parameters (pass NULL for callbacks)**]**

**SRS_IOTHUBTRANSPORTAMQP_09_063: [**If connection_create2() fails, IoTHubTransportAMQP_DoWork shall fail and return immediately**]**

**SRS_IOTHUBTRANSPORTAMQP_09_137: [**IoTHubTransportAMQP_DoWork shall create the AMQP session session_create() AMQP API, passing the connection instance as parameter**]**

**SRS_IOTHUBTRANSPORTAMQP_09_138: [**If session_create() fails, IoTHubTransportAMQP_DoWork shall fail and return immediately**]**

**SRS_IOTHUBTRANSPORTAMQP_09_064: [**IoTHubTransportAMQP_DoWork shall inquire the IoT hub for the preferred value for parameter ‘AMQP incoming window’, and set them on AMQP using session_set_incoming_window() if provided**]**

**SRS_IOTHUBTRANSPORTAMQP_09_065: [**IoTHubTransportAMQP_DoWork shall apply a default value of UINT_MAX for the parameter ‘AMQP incoming window’**]**

**SRS_IOTHUBTRANSPORTAMQP_09_114: [**IoTHubTransportAMQP_DoWork shall inquire the IoT hub for the preferred value for parameter ‘AMQP outgoing window’, and set them on AMQP using session_set_outgoing_window() if provided**]**

**SRS_IOTHUBTRANSPORTAMQP_09_115: [**IoTHubTransportAMQP_DoWork shall apply a default value of 100 for the parameter ‘AMQP outgoing window’**]**

**SRS_IOTHUBTRANSPORTAMQP_09_116: [**IoTHubTransportAMQP_DoWork shall inquire the IoT hub for the preferred value for parameter ‘AMQP frame size’, and set them on AMQP using connection_set_max_frame_size() if provided**]**

**SRS_IOTHUBTRANSPORTAMQP_09_117: [**IoTHubTransportAMQP_DoWork shall apply a default value of 10 for the parameter ‘AMQP frame size’**]**

**SRS_IOTHUBTRANSPORTAMQP_09_118: [**IoTHubTransportAMQP_DoWork shall inquire the IoT hub for the preferred value for parameter ‘Link MAX message size’, and set them on AMQP using link_set_max_message_size() if provided**]**

**SRS_IOTHUBTRANSPORTAMQP_09_119: [**IoTHubTransportAMQP_DoWork shall apply a default value of 65536 for the parameter ‘Link MAX message size’**]**
  
  
Summary of internal AMQP parameters:

<table>
<tr><th>Parameter</th><th>AMQP API for setting value</th><th>Default value</th></tr>
<tr><td>AMQP incoming window</td><td>session_set_incoming_window</td><td>INTMAX</td></tr>
<tr><td>AMQP outgoing window</td><td>session_set_outgoing_window</td><td>100</td></tr>
<tr><td>AMQP frame size</td><td>connection_set_max_frame_size</td><td>10</td></tr>
<tr><td>Link MAX message size</td><td>link_set_max_message_size</td><td>65536</td></tr>
</table>
   
**SRS_IOTHUBTRANSPORTAMQP_09_066: [**IoTHubTransportAMQP_DoWork shall establish the CBS connection using the cbs_create() AMQP API**]**

**SRS_IOTHUBTRANSPORTAMQP_09_067: [**If cbs_create() fails, IoTHubTransportAMQP_DoWork shall fail and return immediately**]**

**SRS_IOTHUBTRANSPORTAMQP_09_139: [**IoTHubTransportAMQP_DoWork shall open the CBS connection using the cbs_open() AMQP API**]**

**SRS_IOTHUBTRANSPORTAMQP_09_140: [**If cbs_open() fails, IoTHubTransportAMQP_DoWork shall fail and return immediately**]**

**SRS_IOTHUBTRANSPORTAMQP_09_068: [**IoTHubTransportAMQP_DoWork shall create the AMQP link for sending messages using ‘source’ as “ingress”, target as the IoT hub FQDN, link name as “sender-link” and role as ‘role_sender’**]**

**SRS_IOTHUBTRANSPORTAMQP_09_069: [**If IoTHubTransportAMQP_DoWork fails to create the AMQP link for sending messages, the function shall fail and return immediately, flagging the connection to be re-stablished**]**


**SRS_IOTHUBTRANSPORTAMQP_06_187: [**If IotHubTransportAMQP_DoWork fails to create an attach properties map and assign that map to the link the function will STILL proceed with the attempt to create the message sender.**]**

**SRS_IOTHUBTRANSPORTAMQP_09_070: [**IoTHubTransportAMQP_DoWork shall create the AMQP message sender using messagesender_create() AMQP API**]**

**SRS_IOTHUBTRANSPORTAMQP_09_071: [**IoTHubTransportAMQP_DoWork shall fail and return immediately if the AMQP message sender instance fails to be created, flagging the connection to be re-established**]**

**SRS_IOTHUBTRANSPORTAMQP_09_072: [**IoTHubTransportAMQP_DoWork shall open the AMQP message sender using messagesender_open() AMQP API**]**

**SRS_IOTHUBTRANSPORTAMQP_09_073: [**IoTHubTransportAMQP_DoWork shall fail and return immediately if the AMQP message sender instance fails to be opened, flagging the connection to be re-established**]**

**SRS_IOTHUBTRANSPORTAMQP_09_074: [**IoTHubTransportAMQP_DoWork shall create the AMQP link for receiving messages using ‘source’ as messageReceiveAddress, target as the “ingress-rx”, link name as “receiver-link” and role as ‘role_receiver’**]**

**SRS_IOTHUBTRANSPORTAMQP_09_075: [**If IoTHubTransportAMQP_DoWork fails to create the AMQP link for receiving messages, the function shall fail and return immediately, flagging the connection to be re-stablished**]**

**SRS_IOTHUBTRANSPORTAMQP_09_076: [**IoTHubTransportAMQP_DoWork shall set the receiver link settle mode as receiver_settle_mode_first**]**

**SRS_IOTHUBTRANSPORTAMQP_09_141: [**If IoTHubTransportAMQP_DoWork fails to set the settle mode on the AMQP link for receiving messages, the function shall fail and return immediately, flagging the connection to be re-stablished**]**

**SRS_IOTHUBTRANSPORTAMQP_09_077: [**IoTHubTransportAMQP_DoWork shall create the AMQP message receiver using messagereceiver_create() AMQP API**]**

**SRS_IOTHUBTRANSPORTAMQP_09_078: [**IoTHubTransportAMQP_DoWork shall fail and return immediately if the AMQP message receiver instance fails to be created, flagging the connection to be re-established**]**

**SRS_IOTHUBTRANSPORTAMQP_09_079: [**IoTHubTransportAMQP_DoWork shall open the AMQP message receiver using messagereceiver_open() AMQP API, passing a callback function for handling C2D incoming messages**]**

**SRS_IOTHUBTRANSPORTAMQP_09_080: [**IoTHubTransportAMQP_DoWork shall fail and return immediately if the AMQP message receiver instance fails to be opened, flagging the connection to be re-established**]**
  
  
  
####SAS Token Refresh

**SRS_IOTHUBTRANSPORTAMQP_09_081: [**IoTHubTransportAMQP_DoWork shall put a new SAS token if the one has not been out already, or if the previous one failed to be put due to timeout of cbs_put_token().**]**

**SRS_IOTHUBTRANSPORTAMQP_09_146: [**If the SAS token fails to be sent to CBS (cbs_put_token), IoTHubTransportAMQP_DoWork shall fail and exit immediately**]**

**SRS_IOTHUBTRANSPORTAMQP_09_082: [**IoTHubTransportAMQP_DoWork shall refresh the SAS token if the current token has been used for more than ‘sas_token_refresh_time’ milliseconds**]**

**SRS_IOTHUBTRANSPORTAMQP_09_083: [**Each new SAS token created by the transport shall be valid for up to ‘sas_token_lifetime’ milliseconds from the time of creation**]**

**SRS_IOTHUBTRANSPORTAMQP_09_145: [**Each new SAS token created shall be deleted from memory immediately after sending it to CBS**]**

**SRS_IOTHUBTRANSPORTAMQP_09_084: [**IoTHubTransportAMQP_DoWork shall wait for ‘cbs_request_timeout’ milliseconds for the cbs_put_token() to complete before failing due to timeout**]**
  
  
  
####Send Events

**SRS_IOTHUBTRANSPORTAMQP_09_086: [**IoTHubTransportAMQP_DoWork shall move queued events to an “in-progress” list right before processing them for sending**]**

**SRS_IOTHUBTRANSPORTAMQP_09_087: [**If the event contains a message of type IOTHUBMESSAGE_BYTEARRAY, IoTHubTransportAMQP_DoWork shall obtain its char* representation and size using IoTHubMessage_GetByteArray()**]**

**SRS_IOTHUBTRANSPORTAMQP_09_088: [**If IoTHubMessage_GetByteArray() fails, IoTHubTransportAMQP_DoWork shall remove the event from the in-progress list and invoke the upper layer callback reporting the error**]**

**SRS_IOTHUBTRANSPORTAMQP_09_089: [**If the event contains a message of type IOTHUBMESSAGE_STRING, IoTHubTransportAMQP_DoWork shall obtain its char* representation using IoTHubMessage_GetString()**]**

**SRS_IOTHUBTRANSPORTAMQP_09_090: [**If the event contains a message of type IOTHUBMESSAGE_STRING, IoTHubTransportAMQP_DoWork shall obtain the size of its char* representation using strlen()**]**

**SRS_IOTHUBTRANSPORTAMQP_09_091: [**If IoTHubMessage_GetString() fails, IoTHubTransportAMQP_DoWork shall remove the event from the in-progress list and invoke the upper layer callback reporting the error**]**

**SRS_IOTHUBTRANSPORTAMQP_09_092: [**If the event contains a message of type IOTHUBMESSAGE_UNKNOWN, IoTHubTransportAMQP_DoWork shall remove the event from the in-progress list and invoke the upper layer callback reporting the error**]**

**SRS_IOTHUBTRANSPORTAMQP_09_093: [**IoTHubTransportAMQP_DoWork shall create an amqp message using message_create() uAMQP API**]**

**SRS_IOTHUBTRANSPORTAMQP_09_111: [**If message_create() fails, IoTHubTransportAMQP_DoWork notify the failure, roll back the event to waitToSent list and return**]**

**SRS_IOTHUBTRANSPORTAMQP_09_095: [**IoTHubTransportAMQP_DoWork shall set the AMQP message body using message_add_body_amqp_data() uAMQP API**]**

SRS_IOTHUBTRANSPORTUAMQP_01_007: [**The IoTHub message properties shall be obtained by calling IoTHubMessage_Properties.] SRS_IOTHUBTRANSPORTUAMQP_01_015: [**The actual keys and values, as well as the number of properties shall be obtained by calling Map_GetInternals on the handle obtained from IoTHubMessage_Properties.] SRS_IOTHUBTRANSPORTUAMQP_01_016: [**If the number of properties is 0, no uAMQP map shall be created and no application properties shall be set on the uAMQP message.] SRS_IOTHUBTRANSPORTUAMQP_01_008: [**All properties shall be transferred to a uAMQP map.] SRS_IOTHUBTRANSPORTUAMQP_01_009: [**The uAMQP map shall be created by calling amqpvalue_create_map.**]**
    
For each property:  
**SRS_IOTHUBTRANSPORTUAMQP_01_010: [**A key uAMQP value shall be created by using amqpvalue_create_string.**]**
**SRS_IOTHUBTRANSPORTUAMQP_01_011: [**A value uAMQP value shall be created by using amqpvalue_create_string.**]**
**SRS_IOTHUBTRANSPORTUAMQP_01_012: [**The key/value pair for the property shall be set into the uAMQP property map by calling amqpvalue_map_set_value.**]**
**SRS_IOTHUBTRANSPORTUAMQP_01_013: [**After all properties have been filled in the uAMQP map, the uAMQP properties map shall be set on the uAMQP message by calling message_set_application_properties.**]**
**SRS_IOTHUBTRANSPORTUAMQP_01_014: [**If any of the APIs fails while building the property map and setting it on the uAMQP message, IoTHubTransportAMQP_DoWork shall notify the failure by invoking the upper layer message send callback with IOTHUB_CLIENT_CONFIRMATION_ERROR.**]**
  
  
**SRS_IOTHUBTRANSPORTAMQP_09_112: [**If message_add_body_amqp_data() fails, IoTHubTransportAMQP_DoWork notify the failure, roll back the event to waitToSent list and return**]**

**SRS_IOTHUBTRANSPORTAMQP_09_097: [**IoTHubTransportAMQP_DoWork shall pass the encoded AMQP message to AMQP for sending (along with on_message_send_complete callback) using messagesender_send()**]**

**SRS_IOTHUBTRANSPORTAMQP_09_113: [**If messagesender_send() fails, IoTHubTransportAMQP_DoWork notify the failure, roll back the event to waitToSent list and return**]**

**SRS_IOTHUBTRANSPORTAMQP_09_100: [**The callback ‘on_message_send_complete’ shall remove the target message from the in-progress list after the upper layer callback**]**

**SRS_IOTHUBTRANSPORTAMQP_09_142: [**The callback ‘on_message_send_complete’ shall pass to the upper layer callback an IOTHUB_CLIENT_CONFIRMATION_OK if the result received is MESSAGE_SEND_OK**]**

**SRS_IOTHUBTRANSPORTAMQP_09_143: [**The callback ‘on_message_send_complete’ shall pass to the upper layer callback an IOTHUB_CLIENT_CONFIRMATION_ERROR if the result received is MESSAGE_SEND_ERROR**]**

**SRS_IOTHUBTRANSPORTAMQP_09_102: [**The callback ‘on_message_send_complete’ shall invoke the upper layer callback for message received if provided**]**

**SRS_IOTHUBTRANSPORTAMQP_09_151: [**The callback ‘on_message_send_complete’ shall destroy the message handle (IOTHUB_MESSAGE_HANDLE) using IoTHubMessage_Destroy()**]**

**SRS_IOTHUBTRANSPORTAMQP_09_152: [**The callback ‘on_message_send_complete’ shall destroy the IOTHUB_MESSAGE_LIST instance**]**

**SRS_IOTHUBTRANSPORTAMQP_09_103: [**IoTHubTransportAMQP_DoWork shall invoke connection_dowork() on AMQP for triggering sending and receiving messages**]**
  
  
  
####Receive Messages

**SRS_IOTHUBTRANSPORTAMQP_09_121: [**IoTHubTransportAMQP_DoWork shall create an AMQP message_receiver if transport_state->message_receive is NULL and transport_state->receive_messages is true**]**

**SRS_IOTHUBTRANSPORTAMQP_09_122: [**IoTHubTransportAMQP_DoWork shall destroy the transport_state->message_receiver (and set it to NULL) if it exists and transport_state->receive_messages is false**]**

**SRS_IOTHUBTRANSPORTAMQP_09_123: [**IoTHubTransportAMQP_DoWork shall create each AMQP message_receiver passing the ‘on_message_received’ as the callback function**]**
  
  
  
This section defines the functionality of the callback function ‘on_message_received’ (passed to AMQP message receiver).

**SRS_IOTHUBTRANSPORTAMQP_09_104: [**The callback ‘on_message_received’ shall invoke IoTHubClient_LL_MessageCallback() passing the client and the incoming message handles as parameters**]**

**SRS_IOTHUBTRANSPORTAMQP_09_105: [**The callback ‘on_message_received’ shall return the result of messaging_delivery_accepted() if the IoTHubClient_LL_MessageCallback() returns IOTHUBMESSAGE_ACCEPTED**]**

**SRS_IOTHUBTRANSPORTAMQP_09_106: [**The callback ‘on_message_received’ shall return the result of messaging_delivery_released() if the IoTHubClient_LL_MessageCallback() returns IOTHUBMESSAGE_ABANDONED**]**

**SRS_IOTHUBTRANSPORTAMQP_09_107: [**The callback ‘on_message_received’ shall return the result of messaging_delivery_rejected(“Rejected by application”, “Rejected by application”) if the IoTHubClient_LL_MessageCallback() returns IOTHUBMESSAGE_REJECTED**]**
  
  
    
###IoTHubTransportAMQP_Register

This function registers a device with the transport.  The AMQP transport only supports a single device established on create, so this function will prevent multiple devices from being registered.
SRS_IOTHUBTRANSPORTUAMQP_17_005: [**IoTHubTransportAMQP_Register shall return NULL if the TRANSPORT_LL_HANDLE is NULL.**]**
SRS_IOTHUBTRANSPORTUAMQP_17_001: [**IoTHubTransportAMQP_Register shall return NULL if deviceId, deviceKey or waitingToSend are NULL.**]**
SRS_IOTHUBTRANSPORTUAMQP_17_002: [**IoTHubTransportAMQP_Register shall return NULL if deviceId or deviceKey do not match the deviceId and deviceKey passed in during IoTHubTransportAMQP_Create.**]**
SRS_IOTHUBTRANSPORTUAMQP_17_003: [**IoTHubTransportAMQP_Register shall return the TRANSPORT_LL_HANDLE as the IOTHUB_DEVICE_HANDLE.**]**
  
  
  
###IoTHubTransportAMQP_Unregister

This function is intended to remove a device as registered with the transport.  As there is only one IoT Hub Device per AMQP transport established on create, this function is a placeholder not intended to do meaningful work.

SRS_IOTHUBTRANSPORTUAMQP_17_004: [**IoTHubTransportAMQP_Unregister shall return.**]**
  
  
  
###IoTHubTransportAMQP_Subscribe

This function enables the transport to notify the upper client layer of new messages received from the cloud to the device.

**SRS_IOTHUBTRANSPORTAMQP_09_037: [**IoTHubTransportAMQP_Subscribe shall fail if the transport handle parameter received is NULL.**]**

**SRS_IOTHUBTRANSPORTAMQP_09_038: [**IoTHubTransportAMQP_Subscribe shall set transport_handle->receive_messages to true and return success code.**]**
  
  
  
###IoTHubTransportAMQP_Unsubscribe

This function disables the notifications to the upper client layer of new messages received from the cloud to the device.

**SRS_IOTHUBTRANSPORTAMQP_09_039: [**IoTHubTransportAMQP_Unsubscribe shall fail if the transport handle parameter received is NULL.**]**

**SRS_IOTHUBTRANSPORTAMQP_09_040: [**IoTHubTransportAMQP_Unsubscribe shall set transport_handle->receive_messages to false and return success code.**]**
  
  
  
###IoTHubTransportAMQP_GetSendStatus

**SRS_IOTHUBTRANSPORTAMQP_09_041: [**IoTHubTransportAMQP_GetSendStatus shall return IOTHUB_CLIENT_INVALID_ARG if called with NULL parameter.**]**

**SRS_IOTHUBTRANSPORTAMQP_09_042: [**IoTHubTransportAMQP_GetSendStatus shall return IOTHUB_CLIENT_OK and status IOTHUB_CLIENT_SEND_STATUS_IDLE if there are currently no event items to be sent or being sent.**]**

**SRS_IOTHUBTRANSPORTAMQP_09_043: [**IoTHubTransportAMQP_GetSendStatus shall return IOTHUB_CLIENT_OK and status IOTHUB_CLIENT_SEND_STATUS_BUSY if there are currently event items to be sent or being sent.**]**
  
  
  
###IoTHubTransportAMQP_SetOption

**SRS_IOTHUBTRANSPORTAMQP_09_044: [**If handle parameter is NULL then IoTHubTransportAMQP_SetOption shall return IOTHUB_CLIENT_INVALID_ARG.**]**

**SRS_IOTHUBTRANSPORTAMQP_09_045: [**If parameter optionName is NULL then IoTHubTransportAMQP_SetOption shall return IOTHUB_CLIENT_INVALID_ARG.**]**

**SRS_IOTHUBTRANSPORTAMQP_09_046: [**If parameter value is NULL then IoTHubTransportAMQP_SetOption shall return IOTHUB_CLIENT_INVALID_ARG.**]**

**SRS_IOTHUBTRANSPORTAMQP_09_048: [**IoTHubTransportAMQP_SetOption shall save and apply the value if the option name is "sas_token_lifetime", returning IOTHUB_CLIENT_OK**]**

**SRS_IOTHUBTRANSPORTAMQP_09_049: [**IoTHubTransportAMQP_SetOption shall save and apply the value if the option name is "sas_token_refresh_time", returning IOTHUB_CLIENT_OK**]**

**SRS_IOTHUBTRANSPORTAMQP_09_148: [**IoTHubTransportAMQP_SetOption shall save and apply the value if the option name is "cbs_request_timeout", returning IOTHUB_CLIENT_OK**]**


<table>
<tr><th>Parameter</th><th>Possible Values</th><th>Details</th></tr>
<tr><td>TrustedCerts</td><td></td><td>Sets the certificate to be used by the transport.</td></tr>
<tr><td>sas_token_lifetime</td><td>0 to TIME_MAX (milliseconds)</td><td>Default: 3600000 milliseconds (1 hour)	How long a SAS token created by the transport is valid, in milliseconds.</td></tr>
<tr><td>sas_token_refresh_time</td><td>0 to TIME_MAX (milliseconds)</td><td>Default: sas_token_lifetime/2	Maximum period of time for the transport to wait before refreshing the SAS token it created previously.</td></tr>
<tr><td>cbs_request_timeout</td><td>1 to TIME_MAX (milliseconds)</td><td>Default: 30 millisecond	Maximum time the transport waits for  AMQP cbs_put_token() to complete before marking it a failure.</td></tr>
<table>
  
  
**SRS_IOTHUBTRANSPORTAMQP_09_047: [**If the option name does not match one of the options handled by this module, then IoTHubTransportAMQP_SetOption shall get  the handle to the XIO and invoke the xio_setoption passing down the option name and value parameters.**]**

**SRS_IOTHUBTRANSPORTUAMQP_03_001: [**If xio_setoption fails,  IoTHubTransportAMQP_SetOption shall return IOTHUB_CLIENT_ERROR.**]**
