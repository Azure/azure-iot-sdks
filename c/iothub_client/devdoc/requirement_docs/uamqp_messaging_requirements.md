# uamqp_messaging Requirements

 
## Overview

This module is a helper to convert message structures supported by IoTHub client (IOTHUB_MESSAGE_HANDLE) to uAMQP (MESSAGE_HANDLE) and vice-versa. 

## Exposed API

```c
extern int IoTHubMessage_CreateFromuAMQPMessage(MESSAGE_HANDLE uamqp_message, IOTHUB_MESSAGE_HANDLE* iothubclient_message);
extern int message_create_from_iothub_message(IOTHUB_MESSAGE_HANDLE iothub_message, MESSAGE_HANDLE* uamqp_message);
```


### IoTHubMessage_CreateFromuAMQPMessage

Creates an IOTHUB_MESSAGE_HANDLE instance which represents the same message defined by the MESSAGE_HANDLE provided.

**SRS_UAMQP_MESSAGING_09_001: [**The body type of the uAMQP message shall be retrieved using message_get_body_type().**]**
**SRS_UAMQP_MESSAGING_09_002: [**If message_get_body_type() fails, IoTHubMessage_CreateFromuAMQPMessage shall fail and return immediately.**]**
**SRS_UAMQP_MESSAGING_09_003: [**If the uAMQP message body type is MESSAGE_BODY_TYPE_DATA, the body data shall be treated as binary data.**]**
**SRS_UAMQP_MESSAGING_09_004: [**The uAMQP message body data shall be retrieved using message_get_body_amqp_data().**]**
**SRS_UAMQP_MESSAGING_09_005: [**If message_get_body_amqp_data() fails, IoTHubMessage_CreateFromuAMQPMessage shall fail and return immediately.**]**
**SRS_UAMQP_MESSAGING_09_006: [**The IOTHUB_MESSAGE instance shall be created using IoTHubMessage_CreateFromByteArray(), passing the uAMQP body bytes as parameter.**]**
**SRS_UAMQP_MESSAGING_09_007: [**If IoTHubMessage_CreateFromByteArray() fails, IoTHubMessage_CreateFromuAMQPMessage shall fail and return immediately.**]**

Copying the AMQP-specific properties:
**SRS_UAMQP_MESSAGING_09_008: [**The uAMQP message properties shall be retrieved using message_get_properties().**]**
**SRS_UAMQP_MESSAGING_09_009: [**If message_get_properties() fails, IoTHubMessage_CreateFromuAMQPMessage shall fail and return immediately.**]**
**SRS_UAMQP_MESSAGING_09_010: [**The message-id property shall be read from the uAMQP message by calling properties_get_message_id.**]**
**SRS_UAMQP_MESSAGING_09_011: [**If properties_get_message_id fails, IoTHubMessage_CreateFromuAMQPMessage() shall fail and return immediately.**]**
**SRS_UAMQP_MESSAGING_09_012: [**The type of the message-id property value shall be obtained using amqpvalue_get_type().**]**
**SRS_UAMQP_MESSAGING_09_013: [**If the type of the message-id property value is AMQP_TYPE_NULL, IoTHubMessage_CreateFromuAMQPMessage() shall skip processing the message-id (as it is optional) and continue normally.**]**
**SRS_UAMQP_MESSAGING_09_014: [**The message-id value shall be retrieved from the AMQP_VALUE as char* by calling amqpvalue_get_string().**]**
**SRS_UAMQP_MESSAGING_09_015: [**If amqpvalue_get_string fails, IoTHubMessage_CreateFromuAMQPMessage() shall fail and return immediately.**]**
**SRS_UAMQP_MESSAGING_09_016: [**The message-id property shall be set on the IOTHUB_MESSAGE_HANDLE instance by calling IoTHubMessage_SetMessageId(), passing the value read from the uAMQP message.**]**
**SRS_UAMQP_MESSAGING_09_017: [**If IoTHubMessage_SetMessageId fails, IoTHubMessage_CreateFromuAMQPMessage() shall fail and return immediately.**]**
**SRS_UAMQP_MESSAGING_09_018: [**The correlation-id property shall be read from the uAMQP message by calling properties_get_correlation_id.**]**
**SRS_UAMQP_MESSAGING_09_019: [**If properties_get_correlation_id() fails, IoTHubMessage_CreateFromuAMQPMessage() shall fail and return immediately.**]**
**SRS_UAMQP_MESSAGING_09_020: [**The type of the correlation-id property value shall be obtained using amqpvalue_get_type().**]**
**SRS_UAMQP_MESSAGING_09_021: [**If the type of the correlation-id property value is AMQP_TYPE_NULL, IoTHubMessage_CreateFromuAMQPMessage() shall skip processing the correlation-id (as it is optional) and continue normally.**]**
**SRS_UAMQP_MESSAGING_09_022: [**The correlation-id value shall be retrieved from the AMQP_VALUE as char* by calling amqpvalue_get_string.**]**
**SRS_UAMQP_MESSAGING_09_023: [**If amqpvalue_get_string fails, IoTHubMessage_CreateFromuAMQPMessage() shall fail and return immediately.**]**
**SRS_UAMQP_MESSAGING_09_024: [**The correlation-id property shall be set on the IOTHUB_MESSAGE_HANDLE by calling IoTHubMessage_SetCorrelationId, passing the value read from the uAMQP message.**]**
**SRS_UAMQP_MESSAGING_09_025: [**If IoTHubMessage_SetCorrelationId fails, IoTHubMessage_CreateFromuAMQPMessage() shall fail and return immediately.**]**
**SRS_UAMQP_MESSAGING_09_026: [**IoTHubMessage_CreateFromuAMQPMessage() shall destroy the uAMQP message properties (obtained with message_get_properties()) by calling properties_destroy().**]**

Copying the AMQP application-properties:
**SRS_UAMQP_MESSAGING_09_027: [**The IOTHUB_MESSAGE_HANDLE properties shall be retrieved using IoTHubMessage_Properties.**]**
**SRS_UAMQP_MESSAGING_09_028: [**If IoTHubMessage_Properties fails, IoTHubMessage_CreateFromuAMQPMessage() shall fail and return immediately.**]**
**SRS_UAMQP_MESSAGING_09_029: [**The uAMQP message application properties shall be retrieved using message_get_application_properties.**]**
**SRS_UAMQP_MESSAGING_09_030: [**If message_get_application_properties fails, IoTHubMessage_CreateFromuAMQPMessage() shall fail and return immediately.**]**
**SRS_UAMQP_MESSAGING_09_031: [**If message_get_application_properties succeeds but returns a NULL application properties map (there are no properties), IoTHubMessage_CreateFromuAMQPMessage() shall skip processing the properties and continue normally.**]**
**SRS_UAMQP_MESSAGING_09_032: [**The actual uAMQP message application properties should be extracted from the result of message_get_application_properties using amqpvalue_get_inplace_described_value.**]**
**SRS_UAMQP_MESSAGING_09_033: [**If amqpvalue_get_inplace_described_value fails, IoTHubMessage_CreateFromuAMQPMessage() shall fail and return immediately.**]**
**SRS_UAMQP_MESSAGING_09_034: [**The number of items in the uAMQP message application properties shall be obtained using amqpvalue_get_map_pair_count.**]**
**SRS_UAMQP_MESSAGING_09_035: [**If amqpvalue_get_map_pair_count fails, IoTHubMessage_CreateFromuAMQPMessage() shall fail and return immediately.**]**
**SRS_UAMQP_MESSAGING_09_036: [**IoTHubMessage_CreateFromuAMQPMessage() shall iterate through each uAMQP application property and add it to IOTHUB_MESSAGE_HANDLE properties.**]**
**SRS_UAMQP_MESSAGING_09_037: [**The uAMQP application property name and value shall be obtained using amqpvalue_get_map_key_value_pair.**]**
**SRS_UAMQP_MESSAGING_09_038: [**If amqpvalue_get_map_key_value_pair fails, IoTHubMessage_CreateFromuAMQPMessage() shall fail and return immediately.**]**
**SRS_UAMQP_MESSAGING_09_039: [**The uAMQP application property name shall be extracted as string using amqpvalue_get_string.**]**
**SRS_UAMQP_MESSAGING_09_040: [**If amqpvalue_get_string fails, IoTHubMessage_CreateFromuAMQPMessage() shall fail and return immediately.**]**
**SRS_UAMQP_MESSAGING_09_041: [**The uAMQP application property value shall be extracted as string using amqpvalue_get_string.**]**
**SRS_UAMQP_MESSAGING_09_042: [**If amqpvalue_get_string fails, IoTHubMessage_CreateFromuAMQPMessage() shall fail and return immediately.**]**
**SRS_UAMQP_MESSAGING_09_043: [**The application property name and value shall be added to IOTHUB_MESSAGE_HANDLE properties using Map_AddOrUpdate.**]**
**SRS_UAMQP_MESSAGING_09_044: [**If Map_AddOrUpdate fails, IoTHubMessage_CreateFromuAMQPMessage() shall fail and return immediately.**]**
**SRS_UAMQP_MESSAGING_09_045: [**IoTHubMessage_CreateFromuAMQPMessage() shall destroy the uAMQP message property name and value (obtained with amqpvalue_get_string) by calling amqpvalue_destroy().**]**
**SRS_UAMQP_MESSAGING_09_046: [**IoTHubMessage_CreateFromuAMQPMessage() shall destroy the uAMQP message property (obtained with message_get_application_properties) by calling amqpvalue_destroy().**]**


### message_create_from_iothub_message

Creates an MESSAGE_HANDLE instance which represents the same message defined by the IOTHUB_MESSAGE_HANDLE provided.

**SRS_UAMQP_MESSAGING_09_047: [**The content type of the IOTHUB_MESSAGE_HANDLE instance shall be obtained using IoTHubMessage_GetContentType().**]**
**SRS_UAMQP_MESSAGING_09_048: [**If the content type of the IOTHUB_MESSAGE_HANDLE instance is IOTHUBMESSAGE_BYTEARRAY, the content shall be obtained using IoTHubMessage_GetByteArray().**]**
**SRS_UAMQP_MESSAGING_09_049: [**If IoTHubMessage_GetByteArray() fails, message_create_from_iothub_message() shall fail and return.**]**
**SRS_UAMQP_MESSAGING_09_050: [**If the content type of the IOTHUB_MESSAGE_HANDLE instance is IOTHUBMESSAGE_STRING, the content shall be obtained using IoTHubMessage_GetString().**]**
**SRS_UAMQP_MESSAGING_09_051: [**If IoTHubMessage_GetString() fails, message_create_from_iothub_message() shall fail and return.**]**
**SRS_UAMQP_MESSAGING_09_052: [**If the content type of the IOTHUB_MESSAGE_HANDLE instance is IOTHUBMESSAGE_UNKNOWN, message_create_from_iothub_message() shall fail and return.**]**
**SRS_UAMQP_MESSAGING_09_053: [**A uAMQP MESSAGE_HANDLE shall be created using message_create().**]**
**SRS_UAMQP_MESSAGING_09_054: [**If message_create() fails, message_create_from_iothub_message() shall fail and return.**]**
**SRS_UAMQP_MESSAGING_09_055: [**The IOTHUB_MESSAGE instance content bytes and size shall be stored on a BINARY_DATA structure.**]**
**SRS_UAMQP_MESSAGING_09_056: [**The BINARY_DATA instance shall be set as the uAMQP message body using message_add_body_amqp_data().**]**
**SRS_UAMQP_MESSAGING_09_057: [**If message_add_body_amqp_data() fails, message_create_from_iothub_message() shall fail and return.**]**
**SRS_UAMQP_MESSAGING_09_058: [**The uAMQP message created by message_create_from_iothub_message() shall be returned only if no failures occurr.**]**
**SRS_UAMQP_MESSAGING_09_059: [**If message_create_from_iothub_message() fails, the uAMQP message (created with message_create()) shall be destroyed.**]**

Copying the AMQP-specific properties:
**SRS_UAMQP_MESSAGING_09_060: [**The uAMQP message properties shall be retrieved using message_get_properties().**]**
**SRS_UAMQP_MESSAGING_09_061: [**If message_get_properties() fails, message_create_from_iothub_message() shall fail and return immediately.**]**
**SRS_UAMQP_MESSAGING_09_062: [**If UAMQP message properties were not present then a new properties container shall be created using properties_create()**]**
**SRS_UAMQP_MESSAGING_09_063: [**If properties_create() fails, message_create_from_iothub_message() shall fail and return immediately.**]**
**SRS_UAMQP_MESSAGING_09_064: [**Message-id from the IOTHUB_MESSAGE shall be read using IoTHubMessage_GetMessageId()**]**
**SRS_UAMQP_MESSAGING_09_065: [**As message-id is optional field, if it is not set on the IOTHUB_MESSAGE, message_create_from_iothub_message() shall ignore it and continue normally.**]**
**SRS_UAMQP_MESSAGING_09_066: [**The message-id value shall be stored on a AMQP_VALUE using amqpvalue_create_string()**]**
**SRS_UAMQP_MESSAGING_09_067: [**If amqpvalue_create_string() fails, message_create_from_iothub_message() shall fail and return immediately.**]**
**SRS_UAMQP_MESSAGING_09_068: [**The message-id AMQP_VALUE shall be set on the uAMQP message using properties_set_message_id()**]**
**SRS_UAMQP_MESSAGING_09_069: [**If properties_set_message_id() fails, message_create_from_iothub_message() shall fail and return immediately.**]**
**SRS_UAMQP_MESSAGING_09_070: [**The uAMQP message-id AMQP_VALUE instance shall be destroyed using amqpvalue_destroy().**]**
**SRS_UAMQP_MESSAGING_09_071: [**Correlation-id from the IOTHUB_MESSAGE shall be read using IoTHubMessage_GetCorrelationId()**]**
**SRS_UAMQP_MESSAGING_09_072: [**As correlation-id is optional field, if it is not set on the IOTHUB_MESSAGE, message_create_from_iothub_message() shall ignore it and continue normally.**]**
**SRS_UAMQP_MESSAGING_09_073: [**The correlation-id value shall be stored on a AMQP_VALUE using amqpvalue_create_string()**]**
**SRS_UAMQP_MESSAGING_09_074: [**If amqpvalue_create_string() fails, message_create_from_iothub_message() shall fail and return immediately.**]**
**SRS_UAMQP_MESSAGING_09_075: [**The correlation-id AMQP_VALUE shall be set on the uAMQP message using properties_set_correlation_id()**]**
**SRS_UAMQP_MESSAGING_09_076: [**If properties_set_correlation_id() fails, message_create_from_iothub_message() shall fail and return immediately.**]**
**SRS_UAMQP_MESSAGING_09_077: [**The uAMQP correlation-id AMQP_VALUE instance shall be destroyed using amqpvalue_destroy().**]**
**SRS_UAMQP_MESSAGING_09_078: [**The updated PROPERTIES_HANDLE instance shall be set on the uAMQP message using message_set_properties()**]**
**SRS_UAMQP_MESSAGING_09_079: [**If message_set_properties() fails, message_create_from_iothub_message() shall fail and return immediately.**]**
**SRS_UAMQP_MESSAGING_09_099: [**The uAMQP message properties (obtained with message_get_properties()) shall be destroyed by calling properties_destroy().**]**

Copying the AMQP application-properties:
**SRS_UAMQP_MESSAGING_09_080: [**The IOTHUB_MESSAGE_HANDLE properties shall be obtained by calling IoTHubMessage_Properties.**]**
**SRS_UAMQP_MESSAGING_09_081: [**If IoTHubMessage_Properties() fails, message_create_from_iothub_message() shall fail and return immediately..**]**
**SRS_UAMQP_MESSAGING_09_082: [**The actual keys and values, as well as the number of properties shall be obtained by calling Map_GetInternals on the handle obtained from IoTHubMessage_Properties.**]**
**SRS_UAMQP_MESSAGING_09_083: [**If Map_GetInternals fails, message_create_from_iothub_message() shall fail and return immediately..**]**
**SRS_UAMQP_MESSAGING_09_084: [**If the number of properties is 0, no application properties shall be set on the uAMQP message and message_create_from_iothub_message() shall return with success.**]**
**SRS_UAMQP_MESSAGING_09_085: [**If the number of properties is greater than 0, message_create_from_iothub_message() shall iterate through all the properties and add them to the uAMQP message.**]**
**SRS_UAMQP_MESSAGING_09_086: [**A uAMQP property map shall be created by calling amqpvalue_create_map().**]**
**SRS_UAMQP_MESSAGING_09_087: [**If amqpvalue_create_map() fails, message_create_from_iothub_message() shall fail and return immediately.**]**
**SRS_UAMQP_MESSAGING_09_088: [**An AMQP_VALUE instance shall be created using amqpvalue_create_string() to hold each uAMQP property name.**]**
**SRS_UAMQP_MESSAGING_09_089: [**If amqpvalue_create_string() fails, message_create_from_iothub_message() shall fail and return immediately..**]**
**SRS_UAMQP_MESSAGING_09_090: [**An AMQP_VALUE instance shall be created using amqpvalue_create_string() to hold each uAMQP property value.**]**
**SRS_UAMQP_MESSAGING_09_091: [**If amqpvalue_create_string() fails, message_create_from_iothub_message() shall fail and return immediately..**]**
**SRS_UAMQP_MESSAGING_09_092: [**The property name and value (AMQP_VALUE instances) shall be added to the uAMQP property map by calling amqpvalue_map_set_value().**]**
**SRS_UAMQP_MESSAGING_09_093: [**If amqpvalue_map_set_value() fails, message_create_from_iothub_message() shall fail and return immediately..**]**
**SRS_UAMQP_MESSAGING_09_094: [**After adding the property name and value to the uAMQP property map, both AMQP_VALUE instances shall be destroyed using amqpvalue_destroy().**]**
**SRS_UAMQP_MESSAGING_09_095: [**If no errors occurred processing the properties, the uAMQP properties map shall be set on the uAMQP message by calling message_set_application_properties().**]**
**SRS_UAMQP_MESSAGING_09_096: [**If message_set_application_properties() fails, message_create_from_iothub_message() shall fail and return immediately..**]**
**SRS_UAMQP_MESSAGING_09_097: [**The uAMQP properties map shall be destroyed using amqpvalue_destroy().**]**

**SRS_UAMQP_MESSAGING_09_098: [**If no errors occurr, message_create_from_iothub_message() shall return 0 (success).**]**