
# iothubtransportamqp_methods requirements
================

## Overview

This module implements the request/response pattern needed for C2D method handling over AMQP for a specific device.    

## Exposed API

```c
typedef struct IOTHUBTRANSPORT_AMQP_METHOD_TAG* IOTHUBTRANSPORT_AMQP_METHOD_HANDLE;
typedef struct IOTHUBTRANSPORT_AMQP_METHODS_TAG* IOTHUBTRANSPORT_AMQP_METHODS_HANDLE;
typedef void(*ON_METHODS_ERROR)(void* context);
typedef void(*ON_METHOD_REQUEST_RECEIVED)(void* context, const char* method_name, const unsigned char* request, size_t request_size, IOTHUBTRANSPORT_AMQP_METHOD_HANDLE response);

MOCKABLE_FUNCTION(, IOTHUBTRANSPORT_AMQP_METHODS_HANDLE, iothubtransportamqp_methods_create, const char*, hostname, const char*, device_id);
MOCKABLE_FUNCTION(, void, iothubtransportamqp_methods_destroy, IOTHUBTRANSPORT_AMQP_METHODS_HANDLE, iothubtransport_amqp_methods_handle);
MOCKABLE_FUNCTION(, int, iothubtransportamqp_methods_subscribe, IOTHUBTRANSPORT_AMQP_METHODS_HANDLE, iothubtransport_amqp_methods_handle,
    SESSION_HANDLE, session_handle, ON_METHODS_ERROR, on_methods_error, void*, on_methods_error_context,
    ON_METHOD_REQUEST_RECEIVED, on_method_request_received, void*, on_method_request_received_context);
MOCKABLE_FUNCTION(, int, iothubtransportamqp_methods_respond, IOTHUBTRANSPORT_AMQP_METHOD_HANDLE, method_handle,
        const unsigned char*, response, size_t, response_size, int, status_code);
MOCKABLE_FUNCTION(, void, iothubtransportamqp_methods_unsubscribe, IOTHUBTRANSPORT_AMQP_METHODS_HANDLE, iothubtransport_amqp_methods_handle);
```

### iothubtransportamqp_methods_create

```c
IOTHUBTRANSPORT_AMQP_METHODS_HANDLE iothubtransportamqp_methods_create(const char* hostname, const char* device_id)
```

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_001: [** `iothubtransportamqp_methods_create` shall instantiate a new handler for C2D methods over AMQP for device `device_id` and on success return a non-NULL handle to it. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_002: [** If any argument is NULL, `iothubtransportamqp_methods_create` shall return NULL. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_003: [** `iothubtransportamqp_methods_create` shall allocate memory for the new instance. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_004: [** If allocating memory fails, `iothubtransportamqp_methods_create` shall return NULL. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_115: [** `iothubtransportamqp_methods_create` shall save the device id for later use by using `mallocAndStrcpy_s`. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_139: [** `iothubtransportamqp_methods_create` shall save the `hostname` for later use by using `mallocAndStrcpy_s`. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_116: [** If `mallocAndStrcpy_s` fails, `iothubtransportamqp_methods_create` shall return NULL. **]**

### iothubtransportamqp_methods_destroy

```c
void iothubtransportamqp_methods_destroy(IOTHUBTRANSPORT_AMQP_METHODS_HANDLE iothubtransport_amqp_methods_handle)
```

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_005: [** `iothubtransportamqp_methods_destroy` shall free all resources allocated by `iothubtransportamqp_methods_create` for the handle `iothubtransport_amqp_methods_handle`. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_006: [** If `iothubtransport_amqp_methods_handle` is NULL, `iothubtransportamqp_methods_destroy` shall do nothing. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_007: [** If the instance pointed to by `iothubtransport_amqp_methods_handle` is subscribed to receive C2D methods, `iothubtransportamqp_methods_destroy` shall free all resources allocated by the subscribe. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_110: [** `iothubtransportamqp_methods_destroy` shall free all tracked method handles indicated to the user via the callback `on_method_request_received` and than have not yet been completed by calls to `iothubtransportamqp_methods_respond`. **]**

### iothubtransportamqp_methods_subscribe

```c
int iothubtransportamqp_methods_subscribe(IOTHUBTRANSPORT_AMQP_METHODS_HANDLE iothubtransport_amqp_methods_handle,
    SESSION_HANDLE session_handle, ON_METHODS_ERROR on_methods_error, void* on_methods_error_context,
    ON_METHOD_REQUEST_RECEIVED on_method_request_received, void* on_method_request_received_context);
```

`iothubtransportamqp_methods_subscribe` creates the links needed for the request/response pattern for C2D methods.

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_008: [** On success it shall return 0. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_137: [** `iothubtransportamqp_methods_subscribe` after another succesfull `iothubtransportamqp_methods_subscribe` without any unsubscribe shall return a non-zero value without performing any subscribe action. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_009: [** If any of the argument `iothubtransport_amqp_methods_handle`, `session_handle`, `on_methods_error` or `on_method_request_received` is NULL, `iothubtransportamqp_methods_subscribe` shall fail and return a non-zero value. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_010: [** `iothubtransportamqp_methods_subscribe` shall create a receiver link by calling `link_create` with the following arguments: **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_011: [** - `session_handle` shall be the session_handle argument passed to iothubtransportamqp_methods_subscribe **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_012: [** - `name` shall be in the format `methods_requests_link-{device_id}`, where device_id is the `device_id` argument passed to `iothubtransportamqp_methods_create`. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_153: [** If constructing the requests link name fails, `iothubtransportamqp_methods_subscribe` shall fail and return a non-zero value. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_013: [** - `role` shall be role_receiver. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_014: [** - `source` shall be the a source value created by calling `messaging_create_source`. **]** **SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_015: [** The address string used to create the source shall be of the form `/devices/{device id}/methods/devicebound`. **]** **SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_016: [** The string shall be created by using `STRING_construct_sprintf`. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_017: [** - `target` shall be the a target value created by calling `messaging_create_target`. **]** **SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_117: [** The address string used to create the target shall be `requests`. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_018: [** If `STRING_construct_sprintf` fails `iothubtransportamqp_methods_subscribe` shall fail and return a non-zero value. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_019: [** If creating the target or source values fails `iothubtransportamqp_methods_subscribe` shall fail and return a non-zero value. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_020: [** If creating the receiver link fails `iothubtransportamqp_methods_subscribe` shall fail and return a non-zero value. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_021: [** `iothubtransportamqp_methods_subscribe` shall create a sender link by calling `link_create` with the following arguments: **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_022: [** - `session_handle` shall be the session_handle argument passed to iothubtransportamqp_methods_subscribe **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_023: [** - `name` shall be format `methods_responses_link-{device_id}`, where device_id is the `device_id` argument passed to `iothubtransportamqp_methods_create`. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_154: [** If constructing the responses link name fails, `iothubtransportamqp_methods_subscribe` shall fail and return a non-zero value. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_024: [** - `role` shall be role_sender. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_025: [** - `source` shall be the a source value created by calling `messaging_create_source`. **]** **SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_026: [** The address string used to create the target shall be `responses`. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_027: [** - `target` shall be the a target value created by calling `messaging_create_target`. **]** **SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_028: [** The address string used to create the source shall be of the form `/devices/{device id}/methods/devicebound`. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_031: [** If creating the target or source values fails `iothubtransportamqp_methods_subscribe` shall fail and return a non-zero value. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_032: [** If creating the receiver link fails `iothubtransportamqp_methods_subscribe` shall fail and return a non-zero value. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_140: [** A link attach properties map shall be created by calling `amqpvalue_create_map`. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_141: [** A property key which shall be a symbol named `com.microsoft:channel-correlation-id` shall be created by calling `amqp_create_symbol`. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_142: [** A property value of type string that shall contain the device id shall be created by calling `amqpvalue_create_string`. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_143: [** The `com.microsoft:channel-correlation-id` shall be added to the link attach properties by calling `amqpvalue_set_map_value`. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_150: [** A property key which shall be a symbol named `com.microsoft:api-version` shall be created by calling `amqp_create_symbol`. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_151: [** A property value of type string that shall contain the `2016-11-14` shall be created by calling `amqpvalue_create_string`. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_152: [** The `com.microsoft:api-version` shall be added to the link attach properties by calling `amqpvalue_set_map_value`. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_144: [** The link attach properties shall be set on the receiver and sender link by calling `link_set_attach_properties`. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_145: [** If any call for creating or setting the link attach properties fails `iothubtransportamqp_methods_subscribe` shall fail and return a non-zero value. **]**  

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_146: [** The link attach properties and all associated values shall be freed by calling `amqpvalue_destroy` after setting the link attach properties. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_105: [** The string created in order to hold the source and target addresses shall be freed by calling `STRING_delete`. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_033: [** `iothubtransportamqp_methods_subscribe` shall create a message receiver associated with the receiver link by calling `messagereceiver_create` and passing the receiver link handle to it. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_118: [** An `on_message_receiver_state_changed` callback together with its context shall be passed to `messagereceiver_create`. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_034: [** If `messagereceiver_create` fails, `iothubtransportamqp_methods_subscribe` shall fail and return a non-zero value. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_035: [** `iothubtransportamqp_methods_subscribe` shall create a message sender associated with the sender link by calling `messagesender_create` and passing the sender link handle to it. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_106: [** An `on_message_sender_state_changed` callback together with its context shall be passed to `messagesender_create`. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_036: [** If `messagesender_create` fails, `iothubtransportamqp_methods_subscribe` shall fail and return a non-zero value. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_037: [** `iothubtransportamqp_methods_subscribe` shall open the message sender by calling `messagesender_open`. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_038: [** If `messagesender_open` fails, `iothubtransportamqp_methods_subscribe` shall fail and return a non-zero value. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_039: [** `iothubtransportamqp_methods_subscribe` shall open the message sender by calling `messagereceiver_open`. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_104: [** An `on_message_received` callback together with its context shall be passed to `messagereceiver_open`. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_040: [** If `messagereceiver_open` fails, `iothubtransportamqp_methods_subscribe` shall fail and return a non-zero value. **]**

### message_received callback

```c
AMQP_VALUE on_message_received(const void* context, MESSAGE_HANDLE message);
```

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_041: [** If `message` is NULL, the RELEASED outcome shall be returned and an error shall be indicated. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_043: [** When `on_message_received` is called (to indicate a new message being received over the receiver link), the message shall be processed as below: **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_044: [** - The message properties shall be obtained by calling `message_get_properties`. **]**
**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_045: [** If `message_get_properties` fails, the REJECTED outcome with `amqp:decode-error` shall be returned. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_046: [** - The correlation id shall be obtained by calling `properties_get_correlation_id` on the message properties. **]**
**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_047: [** If `properties_get_correlation_id` fails the REJECTED outcome with `amqp:decode-error` shall be returned. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_121: [** The uuid value for the correlation ID shall be obtained by calling `amqpvalue_get_uuid`. **]**
**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_122: [** If `amqpvalue_get_uuid` fails the REJECTED outcome with `amqp:decode-error` shall be returned. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_048: [** - The message payload shall be obtained by calling `message_get_body_amqp_data` with the index argument being 0. **]**
**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_049: [** If `message_get_body_amqp_data` fails the REJECTED outcome with `amqp:decode-error` shall be returned. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_099: [** The application properties for the received message shall be obtained by calling `message_get_application_properties`. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_133: [** If `message_get_application_properties` fails the REJECTED outcome with `amqp:decode-error` shall be returned. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_123: [** The AMQP map shall be retrieve from the application properties by calling `amqpvalue_get_inplace_described_value`. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_134: [** If `amqpvalue_get_inplace_described_value` fails the RELEASED outcome with `amqp:decode-error` shall be returned. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_100: [** A property key `IoThub-methodname` shall be created by calling `amqpvalue_create_string`. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_132: [** If `amqpvalue_create_string` fails the RELEASED outcome shall be returned. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_101: [** The method name property value shall be found in the map by calling `amqpvalue_get_map_value`. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_103: [** If `amqpvalue_get_map_value` fails the REJECTED outcome with `amqp:decode-error` shall be returned. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_102: [** The string contained by the property value shall be obtained by calling `amqpvalue_get_string`. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_131: [** If `amqpvalue_get_string` fails the REJECTED outcome with `amqp:decode-error` shall be returned. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_050: [** The binary message payload shall be indicated by calling the `on_method_request_received` callback passed to `iothubtransportamqp_methods_subscribe` with the arguments: **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_051: [** - `context` shall be set to the `on_method_request_received_context` argument passed to `iothubtransportamqp_methods_subscribe`. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_098: [** - `method_name` shall be set to the application property value for `IoThub-methodname`. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_052: [** - `request` shall be set to the payload bytes obtained by calling `message_get_body_amqp_data`. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_053: [** - `request_size` shall be set to the payload size obtained by calling `message_get_body_amqp_data`. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_054: [** - `method_handle` shall be set to a newly created `IOTHUBTRANSPORT_AMQP_METHOD_HANDLE` that can be passed later as an argument to `iothubtransportamqp_methods_respond`. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_112: [** Memory shall be allocated for the `IOTHUBTRANSPORT_AMQP_METHOD_HANDLE` to hold the correlation-id, so that it can be used in the `iothubtransportamqp_methods_respond` function. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_130: [** If allocating memory for the `IOTHUBTRANSPORT_AMQP_METHOD_HANDLE` handle fails, the RELEASED outcome shall be returned and an error shall be indicated. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_147: [** If `on_method_request_received` fails, the REJECTED outcome shall be returned with `amqp:internal-error`. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_113: [** All `IOTHUBTRANSPORT_AMQP_METHOD_HANDLE` handles shall be tracked in an array of handles that shall be resized accordingly when a method handle is added to it. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_138: [** If resizing the tracked method handles array fails, the RELEASED outcome shall be returned and an error shall be indicated. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_056: [** On success the `on_message_received` callback shall return a newly constructed delivery state obtained by calling `messaging_delivery_accepted`. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_057: [** If `messaging_delivery_accepted` fails the RELEASED outcome with `amqp:decode-error` shall be returned. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_129: [** The released outcome shall be created by calling `messaging_delivery_released`. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_128: [** When the RELEASED outcome is returned, an error shall be indicated by calling the `on_methods_error` callback passed to `iothubtransportamqp_methods_subscribe`. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_042: [** When an error is indicated by calling the `on_methods_error`, it shall be called with the context being the `on_methods_error_context` argument passed to `iothubtransportamqp_methods_subscribe`. **]**

### on_message_sender_state_changed callback

```c
void on_message_sender_state_changed(void* context, MESSAGE_SENDER_STATE new_state, MESSAGE_SENDER_STATE previous_state)
```

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_059: [** When `on_message_sender_state_changed` is called with the `new_state` being `MESSAGE_SENDER_STATE_ERROR`, an error shall be indicated by calling the `on_methods_error` callback passed to `iothubtransportamqp_methods_subscribe`. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_058: [** When an error is indicated by calling the `on_methods_error`, it shall be called with the context being the `on_methods_error_context` argument passed to `iothubtransportamqp_methods_subscribe`. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_127: [** For the other state changes, on_message_sender_state_changed shall do nothing. **]**

```c
void on_message_receiver_state_changed(void* context, MESSAGE_RECEIVER_STATE new_state, MESSAGE_RECEIVER_STATE previous_state)
```

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_119: [** When `on_message_receiver_state_changed` is called with the `new_state` being `MESSAGE_RECEIVER_STATE_ERROR`, an error shall be indicated by calling the `on_methods_error` callback passed to `iothubtransportamqp_methods_subscribe`. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_120: [** When an error is indicated by calling the `on_methods_error`, it shall be called with the context being the `on_methods_error_context` argument passed to `iothubtransportamqp_methods_subscribe`. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_126: [** For the other state changes, on_message_receiver_state_changed shall do nothing. **]**

### on_message_send_complete

```c
void on_message_send_complete(void* context, MESSAGE_SEND_RESULT send_result);
```

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_083: [** If `send_result` is `MESSAGE_SEND_ERROR` then an error shall be indicated by calling the `on_methods_error` callback passed to `iothubtransportamqp_methods_subscribe`. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_084: [** Otherwise no action shall be taken. **]**

### iothubtransportamqp_methods_respond 

```c
int iothubtransportamqp_methods_respond(IOTHUBTRANSPORT_AMQP_METHOD_HANDLE method_handle, const unsigned char* response, size_t response_size, int status_code);
```

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_060: [** `iothubtransportamqp_methods_respond` shall construct a response message and on success it shall return 0. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_107: [** If the argument `method_handle` is NULL, `iothubtransportamqp_methods_respond` shall fail and return a non-zero value. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_108: [** If `response_size` is greater than zero and `response` is NULL, `iothubtransportamqp_methods_respond` shall fail and return a non-zero value. **]**

The steps for creating the response message are:

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_061: [** - A new uAMQP message shall be created by calling `message_create`. **]**
**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_062: [** If the `message_create` call fails, `iothubtransportamqp_methods_respond` shall fail and return a non-zero value. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_063: [** - A new properties handle shall be created by calling `properties_create`. **]**
**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_064: [** If the `properties_create call` fails, `iothubtransportamqp_methods_respond` shall fail and return a non-zero value. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_124: [** - An AMQP value holding the correlation id associated with the `method_handle` handle shall be created by calling `amqpvalue_create_uuid`. **]**
**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_125: [** If `amqpvalue_create_uuid` fails, `iothubtransportamqp_methods_respond` shall fail and return a non-zero value. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_065: [** - The correlation id on the message properties shall be set by calling `properties_set_correlation_id` and passing as argument already create correlation ID AMQP value. **]**
**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_066: [** If the `properties_set_correlation_id` call fails, `iothubtransportamqp_methods_respond` shall fail and return a non-zero value. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_148: [** The properties shall be set on the message by calling `message_set_properties`. **]**
**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_149: [** If `message_set_properties` fails, `iothubtransportamqp_methods_respond` shall fail and return a non-zero value. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_090: [** An AMQP map shall be created to hold the application properties for the response by calling `amqpvalue_create_map`. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_091: [** A property key `IoThub-status` shall be created by calling `amqpvalue_create_string`. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_097: [** A property value of type int shall be created from the `status_code` argument by calling `amqpvalue_create_int`. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_093: [** A new entry shall be added in the application properties map by calling `amqpvalue_set_map_value` and passing the key and value that were previously created. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_094: [** The application properties map shall be set on the response message by calling `message_set_application_properties`. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_095: [** The application property map and all intermediate values shall be freed after being passed to `message_set_application_properties`. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_096: [** If any of the calls `amqpvalue_create_string`, `amqpvalue_create_int`, `amqpvalue_create_map`, `amqpvalue_set_map_value` or `message_set_application_properties` fails `iothubtransportamqp_methods_respond` shall fail and return a non-zero value. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_078: [** The binary payload for the response shall be set by calling `message_add_body_amqp_data` for the newly created message handle. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_079: [** The field `bytes` of the `binary_data` argument shall be set to the `response` argument value. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_080: [** The field `length` of the `binary_data` argument shall be set to the `response_size` argument value. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_081: [** If the `message_add_body_amqp_data` call fails, `iothubtransportamqp_methods_respond` shall fail and return a non-zero value. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_067: [** The message shall be handed over to the message_sender by calling `messagesender_send` and passing as arguments: **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_068: [** - The response message handle. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_069: [** - A send callback and its context for the `on_message_send_complete` callback. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_071: [** If the `messagesender_send` call fails, `iothubtransportamqp_methods_respond` shall fail and return a non-zero value. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_109: [** `iothubtransportamqp_methods_respond` shall be allowed to be called from the callback `on_method_request_received`. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_114: [** The handle `method_handle` shall be removed from the array used to track the method handles. **]**    

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_111: [** The handle `method_handle` shall be freed (have no meaning) after `iothubtransportamqp_methods_respond` has been executed. **]**

### iothubtransportamqp_methods_unsubscribe

```c
void iothubtransportamqp_methods_unsubscribe(IOTHUBTRANSPORT_AMQP_METHODS_HANDLE iothubtransport_amqp_methods_handle);
```

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_072: [** If the argument `iothubtransport_amqp_methods_handle` is NULL, `iothubtransportamqp_methods_unsubscribe` shall do nothing. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_135: [** If subscribe was not called yet, `iothubtransportamqp_methods_unsubscribe` shall do nothing. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_136: [** `iothubtransportamqp_methods_unsubscribe` after `iothubtransportamqp_methods_unsubscribe` shall do nothing. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_073: [** Otherwise `iothubtransportamqp_methods_unsubscribe` shall free all resources allocated in `iothubtransportamqp_methods_subscribe`: **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_075: [** - It shall free the message receiver by calling `messagereceiver_destroy'. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_074: [** - It shall free the message sender by calling `messagesender_destroy'. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_076: [** - It shall free the sender link by calling `link_destroy'. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_077: [** - It shall free the receiver link by calling `link_destroy'. **]**
 
