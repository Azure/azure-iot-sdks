// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#include "uamqp_messaging.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_uamqp_c/message.h"
#include "azure_uamqp_c/amqpvalue.h"
#include "iothub_message.h"
#ifndef RESULT_OK
#define RESULT_OK 0
#endif

static int addPropertiesTouAMQPMessage(IOTHUB_MESSAGE_HANDLE iothub_message_handle, MESSAGE_HANDLE uamqp_message)
{
	int result = RESULT_OK;
	const char* messageId;
	const char* correlationId;
	PROPERTIES_HANDLE uamqp_message_properties = NULL;
	int api_call_result;

	// Codes_SRS_UAMQP_MESSAGING_09_060: [The uAMQP message properties shall be retrieved using message_get_properties().]
	if ((api_call_result = message_get_properties(uamqp_message, &uamqp_message_properties)) != 0)
	{
		// Codes_SRS_UAMQP_MESSAGING_09_061: [If message_get_properties() fails, message_create_from_iothub_message() shall fail and return immediately.]
		LogError("Failed to get properties map from uAMQP message (error code %d).", api_call_result);
		result = __LINE__;
	}
	// Codes_SRS_UAMQP_MESSAGING_09_062: [If UAMQP message properties were not present then a new properties container shall be created using properties_create()]
	else if (uamqp_message_properties == NULL &&
		(uamqp_message_properties = properties_create()) == NULL)
	{
		// Codes_SRS_UAMQP_MESSAGING_09_063: [If properties_create() fails, message_create_from_iothub_message() shall fail and return immediately.]
		LogError("Failed to create properties map for uAMQP message (error code %d).", api_call_result);
		result = __LINE__;
	}
	else
	{
		// Codes_SRS_UAMQP_MESSAGING_09_064: [Message-id from the IOTHUB_MESSAGE shall be read using IoTHubMessage_GetMessageId()]
		// Codes_SRS_UAMQP_MESSAGING_09_065: [As message-id is optional field, if it is not set on the IOTHUB_MESSAGE, message_create_from_iothub_message() shall ignore it and continue normally.]
		if ((messageId = IoTHubMessage_GetMessageId(iothub_message_handle)) != NULL)
		{
			// Codes_SRS_UAMQP_MESSAGING_09_066: [The message-id value shall be stored on a AMQP_VALUE using amqpvalue_create_string()]
			AMQP_VALUE uamqp_message_id;
			if ((uamqp_message_id = amqpvalue_create_string(messageId)) == NULL)
			{
				// Codes_SRS_UAMQP_MESSAGING_09_067: [If amqpvalue_create_string() fails, message_create_from_iothub_message() shall fail and return immediately.]
				LogError("Failed to create an AMQP_VALUE for the messageId property value.");
				result = __LINE__;
			}
			else
			{
				// Codes_SRS_UAMQP_MESSAGING_09_068: [The message-id AMQP_VALUE shall be set on the uAMQP message using properties_set_message_id()]
				if ((api_call_result = properties_set_message_id(uamqp_message_properties, uamqp_message_id)) != 0)
				{
					// Codes_SRS_UAMQP_MESSAGING_09_069: [If properties_set_message_id() fails, message_create_from_iothub_message() shall fail and return immediately.]
					LogInfo("Failed to set value of uAMQP message 'message-id' property (%d).", api_call_result);
					result = __LINE__;
				}
				// Codes_SRS_UAMQP_MESSAGING_09_070: [The uAMQP message-id AMQP_VALUE instance shall be destroyed using amqpvalue_destroy().]
				amqpvalue_destroy(uamqp_message_id);
			}
		}

		// Codes_SRS_UAMQP_MESSAGING_09_071: [Correlation-id from the IOTHUB_MESSAGE shall be read using IoTHubMessage_GetCorrelationId()]
		// Codes_SRS_UAMQP_MESSAGING_09_072: [As correlation-id is optional field, if it is not set on the IOTHUB_MESSAGE, message_create_from_iothub_message() shall ignore it and continue normally.]
		if ((correlationId = IoTHubMessage_GetCorrelationId(iothub_message_handle)) != NULL)
		{
			// Codes_SRS_UAMQP_MESSAGING_09_073: [The correlation-id value shall be stored on a AMQP_VALUE using amqpvalue_create_string()]
			AMQP_VALUE uamqp_correlation_id;
			if ((uamqp_correlation_id = amqpvalue_create_string(correlationId)) == NULL)
			{
				// Codes_SRS_UAMQP_MESSAGING_09_074: [If amqpvalue_create_string() fails, message_create_from_iothub_message() shall fail and return immediately.]
				LogError("Failed to create an AMQP_VALUE for the messageId property value.");
				result = __LINE__;
			}
			else
			{
				// Codes_SRS_UAMQP_MESSAGING_09_075: [The correlation-id AMQP_VALUE shall be set on the uAMQP message using properties_set_correlation_id()]
				if ((api_call_result = properties_set_correlation_id(uamqp_message_properties, uamqp_correlation_id)) != 0)
				{
					// Codes_SRS_UAMQP_MESSAGING_09_076: [If properties_set_correlation_id() fails, message_create_from_iothub_message() shall fail and return immediately.]
					LogInfo("Failed to set value of uAMQP message 'message-id' property (%d).", api_call_result);
					result = __LINE__;
				}

				// Codes_SRS_UAMQP_MESSAGING_09_077: [The uAMQP correlation-id AMQP_VALUE instance shall be destroyed using amqpvalue_destroy().]
				amqpvalue_destroy(uamqp_correlation_id);
			}
		}
		// Codes_SRS_UAMQP_MESSAGING_09_078: [The updated PROPERTIES_HANDLE instance shall be set on the uAMQP message using message_set_properties()]
		if ((api_call_result = message_set_properties(uamqp_message, uamqp_message_properties)) != 0)
		{
			// Codes_SRS_UAMQP_MESSAGING_09_079: [If message_set_properties() fails, message_create_from_iothub_message() shall fail and return immediately.]
			LogError("Failed to set properties map on uAMQP message (error code %d).", api_call_result);
			result = __LINE__;
		}
	}

	// Codes_SRS_UAMQP_MESSAGING_09_099: [The uAMQP message properties (obtained with message_get_properties()) shall be destroyed by calling properties_destroy().]
	properties_destroy(uamqp_message_properties);

	return result;
}

static int addApplicationPropertiesTouAMQPMessage(IOTHUB_MESSAGE_HANDLE iothub_message_handle, MESSAGE_HANDLE uamqp_message)
{
	int result = RESULT_OK;
	MAP_HANDLE properties_map;
	const char* const* propertyKeys;
	const char* const* propertyValues;
	size_t propertyCount = 0;

	// Codes_SRS_UAMQP_MESSAGING_09_080: [The IOTHUB_MESSAGE_HANDLE properties shall be obtained by calling IoTHubMessage_Properties.]
	if ((properties_map = IoTHubMessage_Properties(iothub_message_handle)) == NULL)
	{
		// Codes_SRS_UAMQP_MESSAGING_09_081: [If IoTHubMessage_Properties() fails, message_create_from_iothub_message() shall fail and return immediately..]
		LogError("Failed to get property map from IoTHub message.");
		result = __LINE__;
	}
	// Codes_SRS_UAMQP_MESSAGING_09_082: [The actual keys and values, as well as the number of properties shall be obtained by calling Map_GetInternals on the handle obtained from IoTHubMessage_Properties.]
	else if (Map_GetInternals(properties_map, &propertyKeys, &propertyValues, &propertyCount) != MAP_OK)
	{
		// Codes_SRS_UAMQP_MESSAGING_09_083: [If Map_GetInternals fails, message_create_from_iothub_message() shall fail and return immediately..]
		LogError("Failed to get the internals of the property map.");
		result = __LINE__;
	}
	else
	{
		// Codes_SRS_UAMQP_MESSAGING_09_085: [If the number of properties is greater than 0, message_create_from_iothub_message() shall iterate through all the properties and add them to the uAMQP message.]
		if (propertyCount != 0)
		{
			size_t i;
			AMQP_VALUE uamqp_map;

			// Codes_SRS_UAMQP_MESSAGING_09_086: [A uAMQP property map shall be created by calling amqpvalue_create_map().]
			if ((uamqp_map = amqpvalue_create_map()) == NULL)
			{
				// Codes_SRS_UAMQP_MESSAGING_09_087: [If amqpvalue_create_map() fails, message_create_from_iothub_message() shall fail and return immediately.]
				LogError("Failed to create uAMQP map for the properties.");
				result = __LINE__;
			}
			else
			{
				for (i = 0; result == RESULT_OK && i < propertyCount; i++)
				{
					AMQP_VALUE map_key_value = NULL;
					AMQP_VALUE map_value_value = NULL;

					// Codes_SRS_UAMQP_MESSAGING_09_088: [An AMQP_VALUE instance shall be created using amqpvalue_create_string() to hold each uAMQP property name.]
					if ((map_key_value = amqpvalue_create_string(propertyKeys[i])) == NULL)
					{
						// Codes_SRS_UAMQP_MESSAGING_09_089: [If amqpvalue_create_string() fails, message_create_from_iothub_message() shall fail and return immediately..]
						LogError("Failed to create uAMQP property key name.");
						result = __LINE__;
					}
					// Codes_SRS_UAMQP_MESSAGING_09_090: [An AMQP_VALUE instance shall be created using amqpvalue_create_string() to hold each uAMQP property value.]
					else if ((map_value_value = amqpvalue_create_string(propertyValues[i])) == NULL)
					{
						// Codes_SRS_UAMQP_MESSAGING_09_091: [If amqpvalue_create_string() fails, message_create_from_iothub_message() shall fail and return immediately..]
						LogError("Failed to create uAMQP property key value.");
						result = __LINE__;
					}
					// Codes_SRS_UAMQP_MESSAGING_09_092: [The property name and value (AMQP_VALUE instances) shall be added to the uAMQP property map by calling amqpvalue_map_set_value().]
					else if (amqpvalue_set_map_value(uamqp_map, map_key_value, map_value_value) != 0)
					{
						// Codes_SRS_UAMQP_MESSAGING_09_093: [If amqpvalue_map_set_value() fails, message_create_from_iothub_message() shall fail and return immediately..]
						LogError("Failed to set key/value into the the uAMQP property map.");
						result = __LINE__;
					}

					// Codes_SRS_UAMQP_MESSAGING_09_094: [After adding the property name and value to the uAMQP property map, both AMQP_VALUE instances shall be destroyed using amqpvalue_destroy().]
					if (map_key_value != NULL)
						amqpvalue_destroy(map_key_value);

					if (map_value_value != NULL)
						amqpvalue_destroy(map_value_value);
				}

				if (result == RESULT_OK)
				{
					// Codes_SRS_UAMQP_MESSAGING_09_095: [If no errors occurred processing the properties, the uAMQP properties map shall be set on the uAMQP message by calling message_set_application_properties().]
					if (message_set_application_properties(uamqp_message, uamqp_map) != 0)
					{
						// Codes_SRS_UAMQP_MESSAGING_09_096: [If message_set_application_properties() fails, message_create_from_iothub_message() shall fail and return immediately..]
						LogError("Failed to transfer the message properties to the uAMQP message.");
						result = __LINE__;
					}
					else
					{
						result = RESULT_OK;
					}
				}

				// Codes_SRS_UAMQP_MESSAGING_09_097: [The uAMQP properties map shall be destroyed using amqpvalue_destroy().]
				amqpvalue_destroy(uamqp_map);
			}
		}
		else
		{
			// Codes_SRS_UAMQP_MESSAGING_09_084: [If the number of properties is 0, no application properties shall be set on the uAMQP message and message_create_from_iothub_message() shall return with success.]
			result = RESULT_OK;
		}
	}

	return result;
}

static int readPropertiesFromuAMQPMessage(IOTHUB_MESSAGE_HANDLE iothub_message_handle, MESSAGE_HANDLE uamqp_message)
{
	int return_value;
	PROPERTIES_HANDLE uamqp_message_properties;
	AMQP_VALUE uamqp_message_property;
	const char* uamqp_message_property_value;
	int api_call_result;

	// Codes_SRS_UAMQP_MESSAGING_09_008: [The uAMQP message properties shall be retrieved using message_get_properties().]
	if ((api_call_result = message_get_properties(uamqp_message, &uamqp_message_properties)) != 0)
	{
		// Codes_SRS_UAMQP_MESSAGING_09_009: [If message_get_properties() fails, IoTHubMessage_CreateFromuAMQPMessage shall fail and return immediately.]
		LogError("Failed to get property properties map from uAMQP message (error code %d).", api_call_result);
		return_value = __LINE__;
	}
	else
	{
		return_value = 0; // Properties 'message-id' and 'correlation-id' are optional according to the AMQP 1.0 spec.

		// Codes_SRS_UAMQP_MESSAGING_09_010: [The message-id property shall be read from the uAMQP message by calling properties_get_message_id.]
		if ((api_call_result = properties_get_message_id(uamqp_message_properties, &uamqp_message_property)) != 0)
		{
			// Codes_SRS_UAMQP_MESSAGING_09_011: [If properties_get_message_id fails, IoTHubMessage_CreateFromuAMQPMessage() shall fail and return immediately.]
			LogInfo("Failed to get value of uAMQP message 'message-id' property (%d). No failure, since it is optional.", api_call_result);
		}
		// Codes_SRS_UAMQP_MESSAGING_09_012: [The type of the message-id property value shall be obtained using amqpvalue_get_type().]
		// Codes_SRS_UAMQP_MESSAGING_09_013: [If the type of the message-id property value is AMQP_TYPE_NULL, IoTHubMessage_CreateFromuAMQPMessage() shall skip processing the message-id (as it is optional) and continue normally.]
		else if (amqpvalue_get_type(uamqp_message_property) != AMQP_TYPE_NULL)
		{
			// Codes_SRS_UAMQP_MESSAGING_09_014: [The message-id value shall be retrieved from the AMQP_VALUE as char* by calling amqpvalue_get_string().]
			if ((api_call_result = amqpvalue_get_string(uamqp_message_property, &uamqp_message_property_value)) != 0)
			{
				// Codes_SRS_UAMQP_MESSAGING_09_015: [If amqpvalue_get_string fails, IoTHubMessage_CreateFromuAMQPMessage() shall fail and return immediately.]
				LogError("Failed to get value of uAMQP message 'message-id' property (%d).", api_call_result);
				return_value = __LINE__;
			}
			// Codes_SRS_UAMQP_MESSAGING_09_016: [The message-id property shall be set on the IOTHUB_MESSAGE_HANDLE instance by calling IoTHubMessage_SetMessageId(), passing the value read from the uAMQP message.]
			else if (IoTHubMessage_SetMessageId(iothub_message_handle, uamqp_message_property_value) != IOTHUB_MESSAGE_OK)
			{
				// Codes_SRS_UAMQP_MESSAGING_09_017: [If IoTHubMessage_SetMessageId fails, IoTHubMessage_CreateFromuAMQPMessage() shall fail and return immediately.]
				LogError("Failed to set IOTHUB_MESSAGE_HANDLE 'message-id' property.");
				return_value = __LINE__;
			}
		}

		// Codes_SRS_UAMQP_MESSAGING_09_018: [The correlation-id property shall be read from the uAMQP message by calling properties_get_correlation_id.]
		if ((api_call_result = properties_get_correlation_id(uamqp_message_properties, &uamqp_message_property)) != 0)
		{
			// Codes_SRS_UAMQP_MESSAGING_09_019: [If properties_get_correlation_id() fails, IoTHubMessage_CreateFromuAMQPMessage() shall fail and return immediately.]
			LogError("Failed to get value of uAMQP message 'correlation-id' property (%d). No failure, since it is optional.", api_call_result);
		}
		// Codes_SRS_UAMQP_MESSAGING_09_020: [The type of the correlation-id property value shall be obtained using amqpvalue_get_type().]
		// Codes_SRS_UAMQP_MESSAGING_09_021: [If the type of the correlation-id property value is AMQP_TYPE_NULL, IoTHubMessage_CreateFromuAMQPMessage() shall skip processing the correlation-id (as it is optional) and continue normally.]
		else if (amqpvalue_get_type(uamqp_message_property) != AMQP_TYPE_NULL)
		{
			// Codes_SRS_UAMQP_MESSAGING_09_022: [The correlation-id value shall be retrieved from the AMQP_VALUE as char* by calling amqpvalue_get_string.]
			if ((api_call_result = amqpvalue_get_string(uamqp_message_property, &uamqp_message_property_value)) != 0)
			{
				// Codes_SRS_UAMQP_MESSAGING_09_023: [If amqpvalue_get_string fails, IoTHubMessage_CreateFromuAMQPMessage() shall fail and return immediately.]
				LogError("Failed to get value of uAMQP message 'correlation-id' property (%d).", api_call_result);
				return_value = __LINE__;
			}
			// Codes_SRS_UAMQP_MESSAGING_09_024: [The correlation-id property shall be set on the IOTHUB_MESSAGE_HANDLE by calling IoTHubMessage_SetCorrelationId, passing the value read from the uAMQP message.]
			else if (IoTHubMessage_SetCorrelationId(iothub_message_handle, uamqp_message_property_value) != IOTHUB_MESSAGE_OK)
			{
				// Codes_SRS_UAMQP_MESSAGING_09_025: [If IoTHubMessage_SetCorrelationId fails, IoTHubMessage_CreateFromuAMQPMessage() shall fail and return immediately.]
				LogError("Failed to set IOTHUB_MESSAGE_HANDLE 'correlation-id' property.");
				return_value = __LINE__;
			}
		}
		// Codes_SRS_UAMQP_MESSAGING_09_026: [IoTHubMessage_CreateFromuAMQPMessage() shall destroy the uAMQP message properties (obtained with message_get_properties()) by calling properties_destroy().]
		properties_destroy(uamqp_message_properties);
	}

	return return_value;
}

static int readApplicationPropertiesFromuAMQPMessage(IOTHUB_MESSAGE_HANDLE iothub_message_handle, MESSAGE_HANDLE uamqp_message)
{
	int result;
	AMQP_VALUE uamqp_app_properties = NULL;
	AMQP_VALUE uamqp_app_properties_ipdv = NULL;
	uint32_t property_count = 0;
	MAP_HANDLE iothub_message_properties_map;

	// Codes_SRS_UAMQP_MESSAGING_09_027: [The IOTHUB_MESSAGE_HANDLE properties shall be retrieved using IoTHubMessage_Properties.]
	if ((iothub_message_properties_map = IoTHubMessage_Properties(iothub_message_handle)) == NULL)
	{
		// Codes_SRS_UAMQP_MESSAGING_09_028: [If IoTHubMessage_Properties fails, IoTHubMessage_CreateFromuAMQPMessage() shall fail and return immediately.]
		LogError("Failed to get property map from IoTHub message.");
		result = __LINE__;
	}
	// Codes_SRS_UAMQP_MESSAGING_09_029: [The uAMQP message application properties shall be retrieved using message_get_application_properties.]
	else if ((result = message_get_application_properties(uamqp_message, &uamqp_app_properties)) != 0)
	{
		// Codes_SRS_UAMQP_MESSAGING_09_030: [If message_get_application_properties fails, IoTHubMessage_CreateFromuAMQPMessage() shall fail and return immediately.]
		LogError("Failed reading the incoming uAMQP message properties (return code %d).", result);
		result = __LINE__;
	}
	else
	{
		// Codes_SRS_UAMQP_MESSAGING_09_031: [If message_get_application_properties succeeds but returns a NULL application properties map (there are no properties), IoTHubMessage_CreateFromuAMQPMessage() shall skip processing the properties and continue normally.]
		if (uamqp_app_properties == NULL)
		{
			result = 0;
		}
		else
		{
			// Codes_SRS_UAMQP_MESSAGING_09_032: [The actual uAMQP message application properties should be extracted from the result of message_get_application_properties using amqpvalue_get_inplace_described_value.]
			if ((uamqp_app_properties_ipdv = amqpvalue_get_inplace_described_value(uamqp_app_properties)) == NULL)
			{
				// Codes_SRS_UAMQP_MESSAGING_09_033: [If amqpvalue_get_inplace_described_value fails, IoTHubMessage_CreateFromuAMQPMessage() shall fail and return immediately.]
				LogError("Failed getting the map of uAMQP message application properties (return code %d).", result);
				result = __LINE__;
			}
			// Codes_SRS_UAMQP_MESSAGING_09_034: [The number of items in the uAMQP message application properties shall be obtained using amqpvalue_get_map_pair_count.]
			else if ((result = amqpvalue_get_map_pair_count(uamqp_app_properties_ipdv, &property_count)) != 0)
			{
				// Codes_SRS_UAMQP_MESSAGING_09_035: [If amqpvalue_get_map_pair_count fails, IoTHubMessage_CreateFromuAMQPMessage() shall fail and return immediately.]
				LogError("Failed reading the number of values in the uAMQP property map (return code %d).", result);
				result = __LINE__;
			}
			else
			{
				// Codes_SRS_UAMQP_MESSAGING_09_036: [IoTHubMessage_CreateFromuAMQPMessage() shall iterate through each uAMQP application property and add it to IOTHUB_MESSAGE_HANDLE properties.]
				uint32_t i;
				for (i = 0; result == RESULT_OK && i < property_count; i++)
				{
					AMQP_VALUE map_key_name = NULL;
					AMQP_VALUE map_key_value = NULL;
					const char *key_name;
					const char* key_value;

					// Codes_SRS_UAMQP_MESSAGING_09_037: [The uAMQP application property name and value shall be obtained using amqpvalue_get_map_key_value_pair.]
					if ((result = amqpvalue_get_map_key_value_pair(uamqp_app_properties_ipdv, i, &map_key_name, &map_key_value)) != 0)
					{
						// Codes_SRS_UAMQP_MESSAGING_09_038: [If amqpvalue_get_map_key_value_pair fails, IoTHubMessage_CreateFromuAMQPMessage() shall fail and return immediately.]
						LogError("Failed reading the key/value pair from the uAMQP property map (return code %d).", result);
						result = __LINE__;
					}

					// Codes_SRS_UAMQP_MESSAGING_09_039: [The uAMQP application property name shall be extracted as string using amqpvalue_get_string.]
					else if ((result = amqpvalue_get_string(map_key_name, &key_name)) != 0)
					{
						// Codes_SRS_UAMQP_MESSAGING_09_040: [If amqpvalue_get_string fails, IoTHubMessage_CreateFromuAMQPMessage() shall fail and return immediately.]
						LogError("Failed parsing the uAMQP property name (return code %d).", result);
						result = __LINE__;
					}
					// Codes_SRS_UAMQP_MESSAGING_09_041: [The uAMQP application property value shall be extracted as string using amqpvalue_get_string.]
					else if ((result = amqpvalue_get_string(map_key_value, &key_value)) != 0)
					{
						// Codes_SRS_UAMQP_MESSAGING_09_042: [If amqpvalue_get_string fails, IoTHubMessage_CreateFromuAMQPMessage() shall fail and return immediately.]
						LogError("Failed parsing the uAMQP property value (return code %d).", result);
						result = __LINE__;
					}
					// Codes_SRS_UAMQP_MESSAGING_09_043: [The application property name and value shall be added to IOTHUB_MESSAGE_HANDLE properties using Map_AddOrUpdate.]
					else if (Map_AddOrUpdate(iothub_message_properties_map, key_name, key_value) != MAP_OK)
					{
						// Codes_SRS_UAMQP_MESSAGING_09_044: [If Map_AddOrUpdate fails, IoTHubMessage_CreateFromuAMQPMessage() shall fail and return immediately.]
						LogError("Failed to add/update IoTHub message property map.");
						result = __LINE__;
					}

					// Codes_SRS_UAMQP_MESSAGING_09_045: [IoTHubMessage_CreateFromuAMQPMessage() shall destroy the uAMQP message property name and value (obtained with amqpvalue_get_string) by calling amqpvalue_destroy().]
					if (map_key_name != NULL)
					{
						amqpvalue_destroy(map_key_name);
					}

					if (map_key_value != NULL)
					{
						amqpvalue_destroy(map_key_value);
					}
				}
			}

			// Codes_SRS_UAMQP_MESSAGING_09_046: [IoTHubMessage_CreateFromuAMQPMessage() shall destroy the uAMQP message property (obtained with message_get_application_properties) by calling amqpvalue_destroy().]
			amqpvalue_destroy(uamqp_app_properties);
		}
	}

	return result;
}

int IoTHubMessage_CreateFromUamqpMessage(MESSAGE_HANDLE uamqp_message, IOTHUB_MESSAGE_HANDLE* iothubclient_message)
{
	int result = __LINE__;

	IOTHUB_MESSAGE_HANDLE iothub_message = NULL;
	MESSAGE_BODY_TYPE body_type;

	// Codes_SRS_UAMQP_MESSAGING_09_001: [The body type of the uAMQP message shall be retrieved using message_get_body_type().]
	if (message_get_body_type(uamqp_message, &body_type) != 0)
	{
		// Codes_SRS_UAMQP_MESSAGING_09_002: [If message_get_body_type() fails, IoTHubMessage_CreateFromuAMQPMessage shall fail and return immediately.]
		LogError("Failed to get the type of the uamqp message.");
		result = __LINE__;
	}
	else
	{
		// Codes_SRS_UAMQP_MESSAGING_09_003: [If the uAMQP message body type is MESSAGE_BODY_TYPE_DATA, the body data shall be treated as binary data.]
		if (body_type == MESSAGE_BODY_TYPE_DATA)
		{
			// Codes_SRS_UAMQP_MESSAGING_09_004: [The uAMQP message body data shall be retrieved using message_get_body_amqp_data().]
			BINARY_DATA binary_data;
			if (message_get_body_amqp_data(uamqp_message, 0, &binary_data) != 0)
			{
				// Codes_SRS_UAMQP_MESSAGING_09_005: [If message_get_body_amqp_data() fails, IoTHubMessage_CreateFromuAMQPMessage shall fail and return immediately.]
				LogError("Failed to get the body of the uamqp message.");
				result = __LINE__;
			}
			// Codes_SRS_UAMQP_MESSAGING_09_006: [The IOTHUB_MESSAGE instance shall be created using IoTHubMessage_CreateFromByteArray(), passing the uAMQP body bytes as parameter.]
			else if ((iothub_message = IoTHubMessage_CreateFromByteArray(binary_data.bytes, binary_data.length)) == NULL)
			{
				// Codes_SRS_UAMQP_MESSAGING_09_007: [If IoTHubMessage_CreateFromByteArray() fails, IoTHubMessage_CreateFromuAMQPMessage shall fail and return immediately.]
				LogError("Failed creating the IOTHUB_MESSAGE_HANDLE instance (IoTHubMessage_CreateFromByteArray failed).");
				result = __LINE__;
			}
		}
	}

	if (iothub_message != NULL)
	{
		if (readPropertiesFromuAMQPMessage(iothub_message, uamqp_message) != RESULT_OK)
		{
			LogError("Failed reading properties of the uamqp message.");
			IoTHubMessage_Destroy(iothub_message);
			result = __LINE__;
		}
		else if (readApplicationPropertiesFromuAMQPMessage(iothub_message, uamqp_message) != RESULT_OK)
		{
			LogError("Failed reading application properties of the uamqp message.");
			IoTHubMessage_Destroy(iothub_message);
			result = __LINE__;
		}
		else
		{
			*iothubclient_message = iothub_message;
			result = RESULT_OK;
		}
	}

	return result;
}

int message_create_from_iothub_message(IOTHUB_MESSAGE_HANDLE iothub_message, MESSAGE_HANDLE* uamqp_message)
{
	int result = __LINE__;
	// Codes_SRS_UAMQP_MESSAGING_09_047: [The content type of the IOTHUB_MESSAGE_HANDLE instance shall be obtained using IoTHubMessage_GetContentType().]
	IOTHUBMESSAGE_CONTENT_TYPE contentType = IoTHubMessage_GetContentType(iothub_message);
	const char* messageContent = NULL;
	size_t messageContentSize = 0;
	MESSAGE_HANDLE uamqp_message_tmp = NULL;

	// Codes_SRS_UAMQP_MESSAGING_09_048: [If the content type of the IOTHUB_MESSAGE_HANDLE instance is IOTHUBMESSAGE_BYTEARRAY, the content shall be obtained using IoTHubMessage_GetByteArray().]
	if (contentType == IOTHUBMESSAGE_BYTEARRAY &&
		IoTHubMessage_GetByteArray(iothub_message, (const unsigned char **)&messageContent, &messageContentSize) != IOTHUB_MESSAGE_OK)
	{
		// Codes_SRS_UAMQP_MESSAGING_09_049: [If IoTHubMessage_GetByteArray() fails, message_create_from_iothub_message() shall fail and return.]
		LogError("Failed getting the BYTE array representation of the IOTHUB_MESSAGE_HANDLE instance.");
		result = __LINE__;
	}
	// Codes_SRS_UAMQP_MESSAGING_09_050: [If the content type of the IOTHUB_MESSAGE_HANDLE instance is IOTHUBMESSAGE_STRING, the content shall be obtained using IoTHubMessage_GetString().]
	else if (contentType == IOTHUBMESSAGE_STRING &&
		((messageContent = IoTHubMessage_GetString(iothub_message)) == NULL))
	{
		// Codes_SRS_UAMQP_MESSAGING_09_051: [If IoTHubMessage_GetString() fails, message_create_from_iothub_message() shall fail and return.]
		LogError("Failed getting the STRING representation of the IOTHUB_MESSAGE_HANDLE instance.");
		result = __LINE__;
	}
	// Codes_SRS_UAMQP_MESSAGING_09_052: [If the content type of the IOTHUB_MESSAGE_HANDLE instance is IOTHUBMESSAGE_UNKNOWN, message_create_from_iothub_message() shall fail and return.]
	else if (contentType == IOTHUBMESSAGE_UNKNOWN)
	{
		LogError("Cannot parse IOTHUB_MESSAGE_HANDLE with content type IOTHUBMESSAGE_UNKNOWN.");
		result = __LINE__;
	}
	// Codes_SRS_UAMQP_MESSAGING_09_053: [A uAMQP MESSAGE_HANDLE shall be created using message_create().]
	else if ((uamqp_message_tmp = message_create()) == NULL)
	{
		// Codes_SRS_UAMQP_MESSAGING_09_054: [If message_create() fails, message_create_from_iothub_message() shall fail and return.]
		LogError("Failed allocating the uAMQP message.");
		result = __LINE__;
	}
	else
	{
		// Codes_SRS_UAMQP_MESSAGING_09_055: [The IOTHUB_MESSAGE instance content bytes and size shall be stored on a BINARY_DATA structure.]
		BINARY_DATA binary_data;

		if (contentType == IOTHUBMESSAGE_STRING)
		{
			messageContentSize = strlen(messageContent);
		}

		binary_data.bytes = (const unsigned char *)messageContent;
		binary_data.length = messageContentSize;

		// Codes_SRS_UAMQP_MESSAGING_09_056: [The BINARY_DATA instance shall be set as the uAMQP message body using message_add_body_amqp_data().]
		if (message_add_body_amqp_data(uamqp_message_tmp, binary_data) != RESULT_OK)
		{
			// Codes_SRS_UAMQP_MESSAGING_09_057: [If message_add_body_amqp_data() fails, message_create_from_iothub_message() shall fail and return.]
			LogError("Failed setting the body of the uAMQP message.");
			result = __LINE__;
		}
		else if (addPropertiesTouAMQPMessage(iothub_message, uamqp_message_tmp) != RESULT_OK)
		{
			LogError("Failed setting properties of the uAMQP message.");
			result = __LINE__;
		}
		else if (addApplicationPropertiesTouAMQPMessage(iothub_message, uamqp_message_tmp) != RESULT_OK)
		{
			LogError("Failed setting application properties of the uAMQP message.");
			result = __LINE__;
		}
		else
		{
			// Codes_SRS_UAMQP_MESSAGING_09_058: [The uAMQP message created by message_create_from_iothub_message() shall be returned only if no failures occurr.]
			*uamqp_message = uamqp_message_tmp;
			// Codes_SRS_UAMQP_MESSAGING_09_098: [If no errors occurr, message_create_from_iothub_message() shall return 0 (success).]
			result = RESULT_OK;
		}
		// Codes_SRS_UAMQP_MESSAGING_09_059: [If message_create_from_iothub_message() fails, the uAMQP message (created with message_create()) shall be destroyed.]
		if (result != RESULT_OK)
		{
			message_destroy(uamqp_message_tmp);
		}
	}

	return result;
}
