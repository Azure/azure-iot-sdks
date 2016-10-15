
# iothubtransportamqp_methods requirements
================

## Overview

This module implements the request/response pattern needed for C2D method handling over AMQP for a specific device.    

## Exposed API

```c
typedef struct IOTHUBTRANSPORT_AMQP_METHODS_TAG* IOTHUBTRANSPORT_AMQP_METHODS_HANDLE;
typedef void (*ON_METHODS_ERROR)(void* context);
typedef int(*ON_METHOD_REQUEST_RECEIVED)(void* context, const unsigned char* request, size_t request_size, BUFFER_HANDLE response);

MOCKABLE_FUNCTION(, IOTHUBTRANSPORT_AMQP_METHODS_HANDLE, iothubtransportamqp_methods_create, const char*, device_id);
MOCKABLE_FUNCTION(, void, iothubtransportamqp_methods_destroy, IOTHUBTRANSPORT_AMQP_METHODS_HANDLE, iothubtransport_amqp_methods_handle);
MOCKABLE_FUNCTION(, int, iothubtransportamqp_methods_subscribe, IOTHUBTRANSPORT_AMQP_METHODS_HANDLE, iothubtransport_amqp_methods_handle,
    SESSION_HANDLE, session_handle, ON_METHODS_ERROR, on_methods_error, void*, on_methods_error_context,
    ON_METHOD_REQUEST_RECEIVED, on_method_request_received, void*, on_method_request_received_context);
MOCKABLE_FUNCTION(, int, iothubtransportamqp_methods_unsubscribe, IOTHUBTRANSPORT_AMQP_METHODS_HANDLE, iothubtransport_amqp_methods_handle);
```

### iothubtransportamqp_methods_create

```c
IOTHUBTRANSPORT_AMQP_METHODS_HANDLE iothubtransportamqp_methods_create(const char* device_id)
```

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_001: [** `iothubtransportamqp_methods_create` shall instantiate a new handler for C2D methods over AMQP for device `device_id` and on success return a non-NULL handle to it. **]**
**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_002: [** If `device_id` is NULL, `iothubtransportamqp_methods_create` shall return NULL. **]**
**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_003: [** `iothubtransportamqp_methods_create` shall allocate memory for the new instance. **]**
**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_004: [** If allocating memory fails, `iothubtransportamqp_methods_create` shall return NULL. **]**

### iothubtransportamqp_methods_destroy

```c
void iothubtransportamqp_methods_destroy(IOTHUBTRANSPORT_AMQP_METHODS_HANDLE iothubtransport_amqp_methods_handle)
```

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_005: [** `iothubtransportamqp_methods_destroy` shall free all resources allocated by `iothubtransportamqp_methods_create` for the handle `iothubtransport_amqp_methods_handle`. **]**
**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_006: [** If `iothubtransport_amqp_methods_handle` is NULL, `iothubtransportamqp_methods_destroy` shall do nothing. **]**
**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_007: [** If the instance pointed to by `iothubtransport_amqp_methods_handle` is subscribed to receive C2D methods, `iothubtransportamqp_methods_destroy` shall perform all actions needed for an unsubscribe. **]**

### iothubtransportamqp_methods_subscribe

```c
int iothubtransportamqp_methods_subscribe(IOTHUBTRANSPORT_AMQP_METHODS_HANDLE iothubtransport_amqp_methods_handle,
    SESSION_HANDLE session_handle, ON_METHODS_ERROR on_methods_error, void* on_methods_error_context,
    ON_METHOD_REQUEST_RECEIVED on_method_request_received, void* on_method_request_received_context);
```

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_008: [** `iothubtransportamqp_methods_subscribe` shall create the links needed for the request/response pattern for C2D methods and on success it shall return 0. **]**
**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_009: [** If any of the argument `iothubtransport_amqp_methods_handle`, `session_handle`, `on_methods_error` or `on_method_request_received` is NULL, `iothubtransportamqp_methods_subscribe` shall fail and return a non-zero value. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_010: [** `iothubtransportamqp_methods_subscribe` shall create a receiver link by calling `link_create` with the following arguments: **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_011: [** - session_handle shall be the session_handle argument passed to iothubtransportamqp_methods_subscribe **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_012: [** - name shall be `methods_requests_link` **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_013: [** - role shall be role_receiver. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_014: [** - source shall be the a source value created by calling `messaging_create_source`. **]** **SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_015: [** The address string used to create the source shall be of the form `/devices/{device id}/methods/devicebound`. **]** **SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_016: [** The string shall be created by using STRING_new and STRING_concat. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_017: [** - target shall be the a target value created by calling `messaging_create_target`. The address string used to create the target shall be `requests`. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_018: [** If any of the `STRING_new` or `STRING_concat` calls fails `iothubtransportamqp_methods_subscribe` shall fail and return a non-zero value. **]**
**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_019: [** If creating the target or source values fails `iothubtransportamqp_methods_subscribe` shall fail and return a non-zero value. **]**
**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_020: [** If creating the receiver link fails `iothubtransportamqp_methods_subscribe` shall fail and return a non-zero value. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_021: [** `iothubtransportamqp_methods_subscribe` shall create a sender link by calling `link_create` with the following arguments: **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_022: [** - session_handle shall be the session_handle argument passed to iothubtransportamqp_methods_subscribe **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_023: [** - name shall be `methods_responses_link` **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_024: [** - role shall be role_sender. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_025: [** - source shall be the a source value created by calling `messaging_create_source`. **]** **SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_026: [** The address string used to create the target shall be `responses`. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_027: [** - target shall be the a target value created by calling `messaging_create_target`. **]** **SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_028: [** The address string used to create the source shall be of the form `/devices/{device id}/methods/devicebound`. **]** **SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_029: [** The string shall be created by using STRING_new and STRING_concat. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_030: [** If any of the `STRING_new` or `STRING_concat` calls fails `iothubtransportamqp_methods_subscribe` shall fail and return a non-zero value. **]**
**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_031: [** If creating the target or source values fails `iothubtransportamqp_methods_subscribe` shall fail and return a non-zero value. **]**
**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_032: [** If creating the receiver link fails `iothubtransportamqp_methods_subscribe` shall fail and return a non-zero value. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_033: [** `iothubtransportamqp_methods_subscribe` shall create a message_receiver associated with the receiver link by calling `message_receiver_create` and passing the receiver link handle to it. **]**
**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_034: [** If `message_receiver_create` fails, `iothubtransportamqp_methods_subscribe` shall fail and return a non-zero value. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_035: [** `iothubtransportamqp_methods_subscribe` shall create a message_sender associated with the sender link by calling `message_sender_create` and passing the sender link handle to it. **]**
**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_036: [** If `message_sender_create` fails, `iothubtransportamqp_methods_subscribe` shall fail and return a non-zero value. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_037: [** `iothubtransportamqp_methods_subscribe` shall open the message sender by calling `message_sender_open`. **]**
**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_038: [** If `message_sender_open` fails, `iothubtransportamqp_methods_subscribe` shall fail and return a non-zero value. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_039: [** `iothubtransportamqp_methods_subscribe` shall open the message sender by calling `message_receiver_open`. **]**
**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_040: [** If `message_receiver_open` fails, `iothubtransportamqp_methods_subscribe` shall fail and return a non-zero value. **]**

### message_received callback

```c
AMQP_VALUE on_message_received(const void* context, MESSAGE_HANDLE message);
```

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_041: [** If `message` is NULL, an error shall be indicated by calling the `on_methods_error` callback passed to `iothubtransportamqp_methods_subscribe`. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_043: [** When `on_message_received` is called to indicate a new message being received over the receiver link, the message shall be processed as below: **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_044: [** - The message properties shall be obtained by calling `message_get_properties`. **]**
**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_045: [** If `message_get_properties` fails an error shall be indicated by calling the `on_methods_error` callback passed to `iothubtransportamqp_methods_subscribe` **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_046: [** - The correlation id shall be obtained by calling `properties_get_correlation_id` on the message properties. **]**
**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_047: [** If `properties_get_correlation_id` fails an error shall be indicated by calling the `on_methods_error` callback passed to `iothubtransportamqp_methods_subscribe`. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_048: [** - The message payload shall be obtained by calling `message_get_body_amqp_data` with the index argument being 0. **]**
**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_049: [** If `message_get_body_amqp_data` fails an error shall be indicated by calling the `on_methods_error` callback passed to `iothubtransportamqp_methods_subscribe`. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_085: [** A new BUFFER handle shall be created to be used for the C2D method respose by calling `BUFFER_new`. **]**
**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_086: [** If `BUFFER_new` fails an error shall be indicated by calling the `on_methods_error` callback passed to `iothubtransportamqp_methods_subscribe`. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_050: [** The binary message payload shall be indicated by calling the `on_method_request_received` callback passed to `iothubtransportamqp_methods_subscribe` with the arguments: **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_051: [** - `context` shall be set to the `on_method_request_received_context` argument passed to `iothubtransportamqp_methods_subscribe`. **]**
**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_052: [** - `request` shall be set to the payload bytes obtained by calling `message_get_body_amqp_data`. **]**
**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_053: [** - `request_size` shall be set to the payload size obtained by calling `message_get_body_amqp_data`. **]**
**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_054: [** - `response` shall be set the buffer handle that was created by the `BUFFER_new` call. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_089: [** If the `on_method_request_received` callback returns a non zero value an error shall be indicated by calling the `on_methods_error` callback passed to `iothubtransportamqp_methods_subscribe`. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_060: [** A response message shall be constructed as follows: **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_061: [** - A new uAMQP message shall be created by calling `message_create`. **]**
**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_062: [** If the `message_create` call fails, an error shall be indicated by calling the `on_methods_error` callback passed to `iothubtransportamqp_methods_subscribe`. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_063: [** - A new properties handle shall be created by calling `properties_create`. **]**
**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_064: [** If the `properties_create call` fails, an error shall be indicated by calling the `on_methods_error` callback passed to `iothubtransportamqp_methods_subscribe`. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_065: [** - The correlation id on the message properties shall be set by calling `properties_set_correlation_id` and passing as argument the correlation id obtained from the request message. **]**
**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_066: [** If the `properties_set_correlation_id` call fails, an error shall be indicated by calling the `on_methods_error` callback passed to `iothubtransportamqp_methods_subscribe`. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_087: [** The response payload buffer memory shall be obtained by calling `BUFFER_u_char` on the response buffer handle created earlier. **]**
**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_088: [** The response payload buffer size shall be obtained by calling `BUFFER_length` on the response buffer handle created earlier. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_078: [** The binary payload for the response shall be set by calling `message_add_body_amqp_data` for the newly created message handle. **]**
**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_079: [** The field `bytes` of the `binary_data` argument shall be set to point to the response buffer memory. **]** 
**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_080: [** The field `length` of the `binary_data` argument shall be set to point to the memory returned in `response_size` argument of the `on_method_request_received` user callback. **]**
**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_081: [** If the `message_add_body_amqp_data` call fails, an error shall be indicated by calling the `on_methods_error` callback passed to `iothubtransportamqp_methods_subscribe`. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_067: [** The message shall be handed over to the message_sender by calling `messagesender_send` and passing as argument: **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_068: [** - The response message handle. **]**
**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_069: [** - a send callback and its context for the `on_message_send_complete` callback. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_071: [** If the `messagesender_send` call fails, an error shall be indicated by calling the `on_methods_error` callback passed to `iothubtransportamqp_methods_subscribe`. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_082: [** The `response` buffer handle shall be freed by calling `BUFFER_delete`. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_056: [** The `on_message_received` callback shall return a newly constructed delivery state obtained by calling `messaging_delivery_accepted`. **]**
**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_057: [** If `messaging_delivery_accepted` fails an error shall be indicated by calling the `on_methods_error` callback passed to `iothubtransportamqp_methods_subscribe` and the callback shall return NULL. **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_042: [** When an error is indicated by calling the `on_methods_error`, it shall be called with the context being the `on_methods_error_context` argument passed to `iothubtransportamqp_methods_subscribe`. **]**

### on_message_sender_state_changed callback

```c
void on_message_sender_state_changed(void* context, MESSAGE_SENDER_STATE new_state, MESSAGE_SENDER_STATE previous_state)
```

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_059: [** When `on_message_sender_state_changed` if called with the `new_state` being `MESSAGE_SENDER_STATE_ERROR`, an error shall be indicated by calling the `on_methods_error` callback passed to `iothubtransportamqp_methods_subscribe`. **]**
**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_058: [** When an error is indicated by calling the `on_methods_error`, it shall be called with the context being the `on_methods_error_context` argument passed to `iothubtransportamqp_methods_subscribe`. **]**

### on_message_send_complete

```c
void on_message_send_complete(void* context, MESSAGE_SEND_RESULT send_result);
```

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_083: [** If `send_result` is `MESSAGE_SEND_ERROR` then an error shall be indicated by calling the `on_methods_error` callback passed to `iothubtransportamqp_methods_subscribe`. **]**
**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_084: [** Otherwise no action shall be taken. **]** 

### iothubtransportamqp_methods_unsubscribe

```c
int iothubtransportamqp_methods_unsubscribe(IOTHUBTRANSPORT_AMQP_METHODS_HANDLE iothubtransport_amqp_methods_handle);
```

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_072: [** If the argument `iothubtransport_amqp_methods_handle` is NULL, `iothubtransportamqp_methods_unsubscribe` shall return a non-zero value. **]**
**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_073: [** `iothubtransportamqp_methods_unsubscribe` shall free all resources allocated in `iothubtransportamqp_methods_subscribe`: **]**

**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_074: [** - It shall free the message sender by calling `messagesender_destroy'. **]**
**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_075: [** - It shall free the message receiver by calling `messagereceiver_destroy'. **]**
**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_076: [** - It shall free the sender link by calling `link_destroy'. **]**
**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_077: [** - It shall free the receiver link by calling `link_destroy'. **]**
