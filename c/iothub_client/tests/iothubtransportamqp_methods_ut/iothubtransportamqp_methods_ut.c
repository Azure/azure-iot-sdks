// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include <stddef.h>

#include "testrunnerswitcher.h"
#include "umock_c.h"
#include "umocktypes_charptr.h"
#include "umocktypes_stdint.h"
#include "umocktypes_bool.h"
#include "umock_c_negative_tests.h"

static TEST_MUTEX_HANDLE g_testByTest;
static TEST_MUTEX_HANDLE g_dllByDll;

#ifdef __cplusplus
extern "C"
{
#endif
    void* my_gballoc_malloc(size_t size)
    {
        return malloc(size);
    }

    void my_gballoc_free(void* ptr)
    {
        free(ptr);
    }

    void* my_gballoc_realloc(void* ptr, size_t size)
    {
        return realloc(ptr, size);
    }

    int real_mallocAndStrcpy_s(char** destination, const char* source);

#ifdef __cplusplus
}
#endif

#define ENABLE_MOCKS

#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/strings.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_uamqp_c/amqp_definitions.h"
#include "azure_uamqp_c/link.h"
#include "azure_uamqp_c/message_receiver.h"
#include "azure_uamqp_c/message_sender.h"
#include "azure_uamqp_c/messaging.h"
#include "azure_uamqp_c/message.h"

#undef ENABLE_MOCKS

#include "iothubtransportamqp_methods.h"

static IOTHUBTRANSPORT_AMQP_METHOD_HANDLE g_method_handle;
static int g_respond_result;

MOCK_FUNCTION_WITH_CODE(, void, test_on_methods_error, void*, context);
MOCK_FUNCTION_END()
MOCK_FUNCTION_WITH_CODE(, int, test_on_method_request_received, void*, context, const char*, method_name, const unsigned char*, request, size_t, request_size, IOTHUBTRANSPORT_AMQP_METHOD_HANDLE, method_handle);
    g_method_handle = method_handle;
MOCK_FUNCTION_END(0)
MOCK_FUNCTION_WITH_CODE(, int, test_on_method_request_received_calling_respond, void*, context, const char*, method_name, const unsigned char*, request, size_t, request_size, IOTHUBTRANSPORT_AMQP_METHOD_HANDLE, method_handle);
    const unsigned char response_payload[] = { 0x43 };
    g_respond_result = iothubtransportamqp_methods_respond(method_handle, response_payload, sizeof(response_payload), 242);
MOCK_FUNCTION_END(0)

#define TEST_SESSION_HANDLE         (SESSION_HANDLE)0x4245
#define TEST_SENDER_LINK            (LINK_HANDLE)0x4246
#define TEST_RECEIVER_LINK          (LINK_HANDLE)0x4247
#define TEST_MESSAGE_RECEIVER       (MESSAGE_RECEIVER_HANDLE)0x4248
#define TEST_MESSAGE_SENDER         (MESSAGE_SENDER_HANDLE)0x4249
#define TEST_RECEIVER_SOURCE        (AMQP_VALUE)0x4250
#define TEST_RECEIVER_TARGET        (AMQP_VALUE)0x4251
#define TEST_SENDER_SOURCE          (AMQP_VALUE)0x4252
#define TEST_SENDER_TARGET          (AMQP_VALUE)0x4253
#define TEST_STRING_HANDLE          (STRING_HANDLE)0x4254
#define TEST_UAMQP_MESSAGE          (MESSAGE_HANDLE)0x4255
#define TEST_RESPONSE_UAMQP_MESSAGE (MESSAGE_HANDLE)0x4256
#define TEST_DELIVERY_ACCEPTED      (AMQP_VALUE)0x4257
#define TEST_DELIVERY_RELEASED      (AMQP_VALUE)0x4258
#define TEST_DELIVERY_REJECTED      (AMQP_VALUE)0x4259

#define LINK_ATTACH_PROPERTIES_MAP		(AMQP_VALUE)0x4260
#define CHANNEL_CORRELATION_ID_KEY		(AMQP_VALUE)0x4261
#define CHANNEL_CORRELATION_ID_VALUE	(AMQP_VALUE)0x4262
#define API_VERSION_KEY					(AMQP_VALUE)0x4263
#define API_VERSION_VALUE				(AMQP_VALUE)0x4264

#define TEST_METHOD_NAME            "test_method_name"

static PROPERTIES_HANDLE test_properties_handle = (PROPERTIES_HANDLE)0x4256;

#ifdef __cplusplus
extern "C"
{
#endif

    static ON_MESSAGE_RECEIVED g_on_message_received;
    static void* g_on_message_received_callback;
    static ON_MESSAGE_SEND_COMPLETE g_on_message_send_complete;
    static void* g_on_message_send_complete_context;
    static ON_MESSAGE_SENDER_STATE_CHANGED g_on_message_sender_state_changed;
    static void* g_on_message_sender_state_changed_context;
    static ON_MESSAGE_RECEIVER_STATE_CHANGED g_on_message_receiver_state_changed;
    static void* g_on_message_receiver_state_changed_context;

    static MESSAGE_SENDER_HANDLE my_messagesender_create(LINK_HANDLE link, ON_MESSAGE_SENDER_STATE_CHANGED on_message_sender_state_changed, void* context)
    {
        (void)link;
        g_on_message_sender_state_changed = on_message_sender_state_changed;
        g_on_message_sender_state_changed_context = context;
        return TEST_MESSAGE_SENDER;
    }

    static MESSAGE_RECEIVER_HANDLE my_messagereceiver_create(LINK_HANDLE link, ON_MESSAGE_RECEIVER_STATE_CHANGED on_message_receiver_state_changed, void* context)
    {
        (void)link;
        g_on_message_receiver_state_changed = on_message_receiver_state_changed;
        g_on_message_receiver_state_changed_context = context;
        return TEST_MESSAGE_RECEIVER;
    }

    static int my_messagereceiver_open(MESSAGE_RECEIVER_HANDLE message_receiver, ON_MESSAGE_RECEIVED on_message_received, const void* callback_context)
    {
        (void)message_receiver;
        g_on_message_received = on_message_received;
        g_on_message_received_callback = (void*)callback_context;
        return 0;
    }

    static int my_messagesender_send(MESSAGE_SENDER_HANDLE message_sender, MESSAGE_HANDLE message, ON_MESSAGE_SEND_COMPLETE on_message_send_complete, void* callback_context)
    {
        (void)message_sender, message;
        g_on_message_send_complete = on_message_send_complete;
        g_on_message_send_complete_context = callback_context;
        return 0;
    }

	static int g_STRING_construct_sprintf_call_count;
	static int g_when_shall_STRING_construct_sprintf_fail;

    STRING_HANDLE STRING_construct_sprintf(const char* format, ...)
    {
		STRING_HANDLE result;
		(void)format;

		g_STRING_construct_sprintf_call_count++;
		if (g_STRING_construct_sprintf_call_count == g_when_shall_STRING_construct_sprintf_fail)
		{
			result = NULL;
		}
		else
		{
			result = TEST_STRING_HANDLE;
		}

        return result;
    }

    char* umock_stringify_uuid(const uuid* value)
    {
        size_t i;
        char* result = (char*)my_gballoc_malloc(49);
        for (i = 0; i < 16; i++)
        {
            if (sprintf(result + i * 3, "%02x ", (*value)[i]) < 0)
            {
                break;
            }
        }

        if (i < 16)
        {
            my_gballoc_free(result);
            result = NULL;
        }
        else
        {
            result[48] = '\0';
        }

        return result;
    }

    int umock_are_equal_uuid(const uuid* left, const uuid* right)
    {
        int result;

        if (memcmp(*left, *right, sizeof(uuid)) == 0)
        {
            result = 1;
        }
        else
        {
            result = 0;
        }

        return result;
    }

    int umock_copy_uuid(uuid* destination, const uuid* source)
    {
        (void)memcpy(destination, source, sizeof(uuid));
        return 0;
    }

    void umock_free_uuid(uuid* value)
    {
        (void)value;
    }

    char* umock_stringify_BINARY_DATA(const BINARY_DATA* value)
    {
        char* result = (char*)my_gballoc_malloc(1);
        (void)value;
        result[0] = '\0';
        return result;
    }

    int umock_are_equal_BINARY_DATA(const BINARY_DATA* left, const BINARY_DATA* right)
    {
        int result;

        if (left->length != right->length)
        {
            result = 0;
        }
        else
        {
            if (memcmp(left->bytes, right->bytes, left->length) == 0)
            {
                result = 1;
            }
            else
            {
                result = 0;
            }
        }

        return result;
    }

    int umock_copy_BINARY_DATA(BINARY_DATA* destination, const BINARY_DATA* source)
    {
        int result;

        destination->bytes = (const unsigned char*)my_gballoc_malloc(source->length);
        if (destination->bytes == NULL)
        {
            result = -1;
        }
        else
        {
            (void)memcpy((void*)destination->bytes, source->bytes, source->length);
            destination->length = source->length;
            result = 0;
        }

        return result;
    }

    void umock_free_BINARY_DATA(BINARY_DATA* value)
    {
        my_gballoc_free((void*)value->bytes);
        value->bytes = NULL;
        value->length = 0;
    }

#ifdef __cplusplus
}
#endif

DEFINE_ENUM_STRINGS(UMOCK_C_ERROR_CODE, UMOCK_C_ERROR_CODE_VALUES)

static void on_umock_c_error(UMOCK_C_ERROR_CODE error_code)
{
    char temp_str[256];
    (void)snprintf(temp_str, sizeof(temp_str), "umock_c reported error :%s", ENUM_TO_STRING(UMOCK_C_ERROR_CODE, error_code));
    ASSERT_FAIL(temp_str);
}

BEGIN_TEST_SUITE(iothubtransportamqp_methods_unittests)

TEST_SUITE_INITIALIZE(suite_init)
{
    int result;

    TEST_INITIALIZE_MEMORY_DEBUG(g_dllByDll);
    g_testByTest = TEST_MUTEX_CREATE();
    ASSERT_IS_NOT_NULL(g_testByTest);

    result = umock_c_init(on_umock_c_error);
    ASSERT_ARE_EQUAL(int, 0, result);
    result = umocktypes_charptr_register_types();
    ASSERT_ARE_EQUAL(int, 0, result);
    result = umocktypes_stdint_register_types();
    ASSERT_ARE_EQUAL(int, 0, result);
    result = umocktypes_bool_register_types();
    ASSERT_ARE_EQUAL(int, 0, result);

    REGISTER_GLOBAL_MOCK_RETURN(messagereceiver_create, TEST_MESSAGE_RECEIVER);
    REGISTER_GLOBAL_MOCK_RETURN(messagesender_create, TEST_MESSAGE_SENDER);
    REGISTER_GLOBAL_MOCK_RETURN(messaging_delivery_accepted, TEST_DELIVERY_ACCEPTED);
    REGISTER_GLOBAL_MOCK_RETURN(messaging_delivery_released, TEST_DELIVERY_RELEASED);
    REGISTER_GLOBAL_MOCK_RETURN(messaging_delivery_rejected, TEST_DELIVERY_REJECTED);

    REGISTER_UMOCK_VALUE_TYPE(uuid);
    REGISTER_UMOCK_VALUE_TYPE(BINARY_DATA);

    REGISTER_GLOBAL_MOCK_HOOK(gballoc_malloc, my_gballoc_malloc);
    REGISTER_GLOBAL_MOCK_HOOK(gballoc_free, my_gballoc_free);
    REGISTER_GLOBAL_MOCK_HOOK(gballoc_realloc, my_gballoc_realloc);
    REGISTER_GLOBAL_MOCK_HOOK(mallocAndStrcpy_s, real_mallocAndStrcpy_s);
    REGISTER_GLOBAL_MOCK_HOOK(messagereceiver_open, my_messagereceiver_open);
    REGISTER_GLOBAL_MOCK_HOOK(messagesender_send, my_messagesender_send);
    REGISTER_GLOBAL_MOCK_HOOK(messagesender_create, my_messagesender_create);
    REGISTER_GLOBAL_MOCK_HOOK(messagereceiver_create, my_messagereceiver_create);
    
    REGISTER_UMOCK_ALIAS_TYPE(MESSAGE_RECEIVER_HANDLE, void*);
    REGISTER_UMOCK_ALIAS_TYPE(MESSAGE_SENDER_HANDLE, void*);
    REGISTER_UMOCK_ALIAS_TYPE(LINK_HANDLE, void*);
    REGISTER_UMOCK_ALIAS_TYPE(SESSION_HANDLE, void*);
    REGISTER_UMOCK_ALIAS_TYPE(STRING_HANDLE, void*);
    REGISTER_UMOCK_ALIAS_TYPE(AMQP_VALUE, void*);
    REGISTER_UMOCK_ALIAS_TYPE(role, bool);
    REGISTER_UMOCK_ALIAS_TYPE(ON_MESSAGE_RECEIVER_STATE_CHANGED, void*);
    REGISTER_UMOCK_ALIAS_TYPE(ON_MESSAGE_SENDER_STATE_CHANGED, void*);
    REGISTER_UMOCK_ALIAS_TYPE(ON_MESSAGE_RECEIVED, void*);
    REGISTER_UMOCK_ALIAS_TYPE(MESSAGE_HANDLE, void*);
    REGISTER_UMOCK_ALIAS_TYPE(PROPERTIES_HANDLE, void*);
    REGISTER_UMOCK_ALIAS_TYPE(IOTHUBTRANSPORT_AMQP_METHOD_HANDLE, void*);
    REGISTER_UMOCK_ALIAS_TYPE(ON_MESSAGE_SEND_COMPLETE, void*);
	REGISTER_UMOCK_ALIAS_TYPE(fields, void*);
}

TEST_SUITE_CLEANUP(suite_cleanup)
{
    umock_c_deinit();

    TEST_MUTEX_DESTROY(g_testByTest);
    TEST_DEINITIALIZE_MEMORY_DEBUG(g_dllByDll);
}

TEST_FUNCTION_INITIALIZE(TestMethodInitialize)
{
    if (TEST_MUTEX_ACQUIRE(g_testByTest))
    {
        ASSERT_FAIL("our mutex is ABANDONED. Failure in test framework");
    }

	g_STRING_construct_sprintf_call_count = 0;
	g_when_shall_STRING_construct_sprintf_fail = 0;
    umock_c_reset_all_calls();
}

TEST_FUNCTION_CLEANUP(TestMethodCleanup)
{
    TEST_MUTEX_RELEASE(g_testByTest);
}

static void setup_subscribe_expected_calls(void)
{
    STRICT_EXPECTED_CALL(STRING_c_str(TEST_STRING_HANDLE))
        .SetReturn("/devices/test_device/methods/devicebound");
    STRICT_EXPECTED_CALL(messaging_create_source("/devices/test_device/methods/devicebound"))
        .SetReturn(TEST_RECEIVER_SOURCE);
    STRICT_EXPECTED_CALL(messaging_create_target("requests"))
        .SetReturn(TEST_RECEIVER_TARGET);
	STRICT_EXPECTED_CALL(STRING_c_str(TEST_STRING_HANDLE))
		.SetReturn("methods_requests_link-testdevice");
	STRICT_EXPECTED_CALL(link_create(TEST_SESSION_HANDLE, "methods_requests_link-testdevice", role_receiver, TEST_RECEIVER_SOURCE, TEST_RECEIVER_TARGET))
        .SetReturn(TEST_RECEIVER_LINK);
    STRICT_EXPECTED_CALL(messaging_create_source("responses"))
        .SetReturn(TEST_SENDER_SOURCE);
    STRICT_EXPECTED_CALL(STRING_c_str(TEST_STRING_HANDLE))
        .SetReturn("/devices/test_device/methods/devicebound");
    STRICT_EXPECTED_CALL(messaging_create_target("/devices/test_device/methods/devicebound"))
        .SetReturn(TEST_SENDER_TARGET);
	STRICT_EXPECTED_CALL(STRING_c_str(TEST_STRING_HANDLE))
		.SetReturn("methods_responses_link-testdevice");
	STRICT_EXPECTED_CALL(link_create(TEST_SESSION_HANDLE, "methods_responses_link-testdevice", role_sender, TEST_SENDER_SOURCE, TEST_SENDER_TARGET))
        .SetReturn(TEST_SENDER_LINK);
	STRICT_EXPECTED_CALL(amqpvalue_create_map())
		.SetReturn(LINK_ATTACH_PROPERTIES_MAP);
	STRICT_EXPECTED_CALL(amqpvalue_create_symbol("com.microsoft:channel-correlation-id"))
		.SetReturn(CHANNEL_CORRELATION_ID_KEY);
	STRICT_EXPECTED_CALL(amqpvalue_create_string("testdevice"))
		.SetReturn(CHANNEL_CORRELATION_ID_VALUE);
	STRICT_EXPECTED_CALL(amqpvalue_set_map_value(LINK_ATTACH_PROPERTIES_MAP, CHANNEL_CORRELATION_ID_KEY, CHANNEL_CORRELATION_ID_VALUE));
	STRICT_EXPECTED_CALL(amqpvalue_create_symbol("com.microsoft:api-version"))
		.SetReturn(API_VERSION_KEY);
	STRICT_EXPECTED_CALL(amqpvalue_create_string("2016-11-14"))
		.SetReturn(API_VERSION_VALUE);
	STRICT_EXPECTED_CALL(amqpvalue_set_map_value(LINK_ATTACH_PROPERTIES_MAP, API_VERSION_KEY, API_VERSION_VALUE));
	STRICT_EXPECTED_CALL(link_set_attach_properties(TEST_SENDER_LINK, LINK_ATTACH_PROPERTIES_MAP));
	STRICT_EXPECTED_CALL(link_set_attach_properties(TEST_RECEIVER_LINK, LINK_ATTACH_PROPERTIES_MAP));
	STRICT_EXPECTED_CALL(amqpvalue_destroy(API_VERSION_VALUE));
	STRICT_EXPECTED_CALL(amqpvalue_destroy(API_VERSION_KEY));
	STRICT_EXPECTED_CALL(amqpvalue_destroy(CHANNEL_CORRELATION_ID_VALUE));
	STRICT_EXPECTED_CALL(amqpvalue_destroy(CHANNEL_CORRELATION_ID_KEY));
	STRICT_EXPECTED_CALL(amqpvalue_destroy(LINK_ATTACH_PROPERTIES_MAP));
	STRICT_EXPECTED_CALL(messagereceiver_create(TEST_RECEIVER_LINK, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument_on_message_receiver_state_changed()
        .IgnoreArgument_context();
    STRICT_EXPECTED_CALL(messagesender_create(TEST_SENDER_LINK, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument_on_message_sender_state_changed()
        .IgnoreArgument_context();
    STRICT_EXPECTED_CALL(messagesender_open(TEST_MESSAGE_SENDER));
    STRICT_EXPECTED_CALL(messagereceiver_open(TEST_MESSAGE_RECEIVER, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument_on_message_received()
        .IgnoreArgument_callback_context();
	STRICT_EXPECTED_CALL(STRING_delete(TEST_STRING_HANDLE));
    STRICT_EXPECTED_CALL(amqpvalue_destroy(TEST_SENDER_TARGET));
    STRICT_EXPECTED_CALL(amqpvalue_destroy(TEST_SENDER_SOURCE));
	STRICT_EXPECTED_CALL(STRING_delete(TEST_STRING_HANDLE));
    STRICT_EXPECTED_CALL(amqpvalue_destroy(TEST_RECEIVER_TARGET));
    STRICT_EXPECTED_CALL(amqpvalue_destroy(TEST_RECEIVER_SOURCE));
    STRICT_EXPECTED_CALL(STRING_delete(TEST_STRING_HANDLE));
}

static void setup_message_received_calls(void)
{
    AMQP_VALUE correlation_id = (AMQP_VALUE)0x5000;
    AMQP_VALUE application_properties = (AMQP_VALUE)0x5001;
    AMQP_VALUE application_properties_map = (AMQP_VALUE)0x5002;
    AMQP_VALUE test_property_key = (AMQP_VALUE)0x5003;
    AMQP_VALUE test_property_value = (AMQP_VALUE)0x5004;
    static uuid correlation_id_uuid;
    static const unsigned char test_method_request_payload[] = { 42 };
    static BINARY_DATA binary_data;
    const char* method_name_ptr = TEST_METHOD_NAME;

    binary_data.bytes = test_method_request_payload;
    binary_data.length = sizeof(test_method_request_payload);

    STRICT_EXPECTED_CALL(message_get_properties(TEST_UAMQP_MESSAGE, IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(2, &test_properties_handle, sizeof(test_properties_handle));
    STRICT_EXPECTED_CALL(properties_get_correlation_id(test_properties_handle, IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(2, &correlation_id, sizeof(correlation_id));
    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(gballoc_realloc(IGNORED_PTR_ARG, IGNORED_NUM_ARG));
    STRICT_EXPECTED_CALL(amqpvalue_get_uuid(correlation_id, IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(2, &correlation_id_uuid, sizeof(correlation_id_uuid));
    STRICT_EXPECTED_CALL(message_get_body_amqp_data(TEST_UAMQP_MESSAGE, 0, IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &binary_data, sizeof(binary_data));
    STRICT_EXPECTED_CALL(message_get_application_properties(TEST_UAMQP_MESSAGE, IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(2, &application_properties, sizeof(application_properties));
    STRICT_EXPECTED_CALL(amqpvalue_get_inplace_described_value(application_properties))
        .SetReturn(application_properties_map);
    STRICT_EXPECTED_CALL(amqpvalue_create_string("IoThub-methodname"))
        .SetReturn(test_property_key);
    STRICT_EXPECTED_CALL(amqpvalue_get_map_value(application_properties_map, test_property_key))
        .SetReturn(test_property_value);
    STRICT_EXPECTED_CALL(amqpvalue_get_string(test_property_value, IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(2, &method_name_ptr, sizeof(method_name_ptr));
    STRICT_EXPECTED_CALL(messaging_delivery_accepted());
    STRICT_EXPECTED_CALL(test_on_method_request_received((void*)0x4243, TEST_METHOD_NAME, IGNORED_PTR_ARG, sizeof(test_method_request_payload), IGNORED_PTR_ARG))
        .ValidateArgumentBuffer(3, test_method_request_payload, sizeof(test_method_request_payload))
        .IgnoreArgument_method_handle();
    STRICT_EXPECTED_CALL(amqpvalue_destroy(test_property_value));
    STRICT_EXPECTED_CALL(amqpvalue_destroy(test_property_key));
    STRICT_EXPECTED_CALL(application_properties_destroy(application_properties));
    STRICT_EXPECTED_CALL(properties_destroy(test_properties_handle));
}

static void setup_method_respond_calls(void)
{
    static const unsigned char response_payload[] = { 0x43 };
    static uuid correlation_id_uuid;
    AMQP_VALUE response_correlation_id = (AMQP_VALUE)0x6000;
    AMQP_VALUE response_properties_map = (AMQP_VALUE)0x6001;
    AMQP_VALUE status_property_key = (AMQP_VALUE)0x6002;
    AMQP_VALUE status_property_value = (AMQP_VALUE)0x6003;
    PROPERTIES_HANDLE response_properties_handle = (PROPERTIES_HANDLE)0x6004;
    static BINARY_DATA response_binary_data;

    response_binary_data.bytes = response_payload;
    response_binary_data.length = sizeof(response_payload);

    STRICT_EXPECTED_CALL(message_create())
        .SetReturn(TEST_RESPONSE_UAMQP_MESSAGE);
    STRICT_EXPECTED_CALL(properties_create())
        .SetReturn(response_properties_handle);
    EXPECTED_CALL(amqpvalue_create_uuid(correlation_id_uuid))
        .SetReturn(response_correlation_id);
    STRICT_EXPECTED_CALL(properties_set_correlation_id(response_properties_handle, response_correlation_id));
    STRICT_EXPECTED_CALL(message_set_properties(TEST_RESPONSE_UAMQP_MESSAGE, response_properties_handle));
    STRICT_EXPECTED_CALL(amqpvalue_create_map())
        .SetReturn(response_properties_map);
    STRICT_EXPECTED_CALL(amqpvalue_create_string("IoThub-status"))
        .SetReturn(status_property_key);
    STRICT_EXPECTED_CALL(amqpvalue_create_int(100))
        .SetReturn(status_property_value);
    STRICT_EXPECTED_CALL(amqpvalue_set_map_value(response_properties_map, status_property_key, status_property_value));
    STRICT_EXPECTED_CALL(message_set_application_properties(TEST_RESPONSE_UAMQP_MESSAGE, response_properties_map));
    STRICT_EXPECTED_CALL(message_add_body_amqp_data(TEST_RESPONSE_UAMQP_MESSAGE, response_binary_data));
    STRICT_EXPECTED_CALL(messagesender_send(TEST_MESSAGE_SENDER, TEST_RESPONSE_UAMQP_MESSAGE, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument_on_message_send_complete()
        .IgnoreArgument_callback_context();
    EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));
}

static void setup_respond_calls(int status)
{
    static const unsigned char response_payload[] = { 0x43 };
    AMQP_VALUE response_correlation_id = (AMQP_VALUE)0x6000;
    AMQP_VALUE response_properties_map = (AMQP_VALUE)0x6001;
    AMQP_VALUE status_property_key = (AMQP_VALUE)0x6002;
    AMQP_VALUE status_property_value = (AMQP_VALUE)0x6003;
    PROPERTIES_HANDLE response_properties_handle = (PROPERTIES_HANDLE)0x6004;
    static BINARY_DATA response_binary_data;
    static uuid correlation_id_uuid;

    response_binary_data.bytes = response_payload;
    response_binary_data.length = sizeof(response_payload);

    STRICT_EXPECTED_CALL(message_create())
        .SetReturn(TEST_RESPONSE_UAMQP_MESSAGE);
    STRICT_EXPECTED_CALL(properties_create())
        .SetReturn(response_properties_handle);
    EXPECTED_CALL(amqpvalue_create_uuid(correlation_id_uuid))
        .SetReturn(response_correlation_id);
    STRICT_EXPECTED_CALL(properties_set_correlation_id(response_properties_handle, response_correlation_id));
    STRICT_EXPECTED_CALL(message_set_properties(TEST_RESPONSE_UAMQP_MESSAGE, response_properties_handle));
    STRICT_EXPECTED_CALL(amqpvalue_create_map())
        .SetReturn(response_properties_map);
    STRICT_EXPECTED_CALL(amqpvalue_create_string("IoThub-status"))
        .SetReturn(status_property_key);
    STRICT_EXPECTED_CALL(amqpvalue_create_int(status))
        .SetReturn(status_property_value);
    STRICT_EXPECTED_CALL(amqpvalue_set_map_value(response_properties_map, status_property_key, status_property_value));
    STRICT_EXPECTED_CALL(message_set_application_properties(TEST_RESPONSE_UAMQP_MESSAGE, response_properties_map));
    STRICT_EXPECTED_CALL(message_add_body_amqp_data(TEST_RESPONSE_UAMQP_MESSAGE, response_binary_data));
    STRICT_EXPECTED_CALL(messagesender_send(TEST_MESSAGE_SENDER, TEST_RESPONSE_UAMQP_MESSAGE, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument_on_message_send_complete()
        .IgnoreArgument_callback_context();
    EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));
    EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(amqpvalue_destroy(status_property_value));
    STRICT_EXPECTED_CALL(amqpvalue_destroy(status_property_key));
    STRICT_EXPECTED_CALL(amqpvalue_destroy(response_properties_map));
    STRICT_EXPECTED_CALL(amqpvalue_destroy(response_correlation_id));
    STRICT_EXPECTED_CALL(properties_destroy(response_properties_handle));
    STRICT_EXPECTED_CALL(message_destroy(TEST_RESPONSE_UAMQP_MESSAGE));
}

/* iothubtransportamqp_methods_create */

/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_001: [ `iothubtransportamqp_methods_create` shall instantiate a new handler for C2D methods over AMQP for device `device_id` and on success return a non-NULL handle to it. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_003: [ `iothubtransportamqp_methods_create` shall allocate memory for the new instance. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_115: [ `iothubtransportamqp_methods_create` shall save the device id for later use by using `mallocAndStrcpy_s`. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_139: [ `iothubtransportamqp_methods_create` shall save the `hostname` for later use by using `mallocAndStrcpy_s`. ]*/
TEST_FUNCTION(iothubtransportamqp_methods_create_copies_the_device_id_and_succeeds)
{
    /// arrange
    IOTHUBTRANSPORT_AMQP_METHODS_HANDLE amqp_methods_handle;
    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));
    STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, "testdevice"))
        .IgnoreArgument_destination();
	STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, "testhost"))
		.IgnoreArgument_destination();

    /// act
    amqp_methods_handle = iothubtransportamqp_methods_create("testhost", "testdevice");

    /// assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_IS_NOT_NULL(amqp_methods_handle);

    /// cleanup
    iothubtransportamqp_methods_destroy(amqp_methods_handle);
}

/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_002: [ If any argument is NULL, `iothubtransportamqp_methods_create` shall return NULL. ]*/
TEST_FUNCTION(iothubtransportamqp_methods_create_with_NULL_hostname_fails)
{
    /// arrange
    IOTHUBTRANSPORT_AMQP_METHODS_HANDLE amqp_methods_handle;

    /// act
    amqp_methods_handle = iothubtransportamqp_methods_create(NULL, "testdevice");

    /// assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_IS_NULL(amqp_methods_handle);
}

/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_002: [ If any argument is NULL, `iothubtransportamqp_methods_create` shall return NULL. ]*/
TEST_FUNCTION(iothubtransportamqp_methods_create_with_NULL_device_id_fails)
{
	/// arrange
	IOTHUBTRANSPORT_AMQP_METHODS_HANDLE amqp_methods_handle;
	/// act
	amqp_methods_handle = iothubtransportamqp_methods_create("test.something", NULL);

	/// assert
	ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
	ASSERT_IS_NULL(amqp_methods_handle);
}

/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_004: [ If allocating memory fails, `iothubtransportamqp_methods_create` shall return NULL. ]*/
TEST_FUNCTION(when_allocating_memory_fails_then_iothubtransportamqp_methods_create_fails)
{
    /// arrange
    IOTHUBTRANSPORT_AMQP_METHODS_HANDLE amqp_methods_handle;
    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
        .SetReturn(NULL);

    /// act
    amqp_methods_handle = iothubtransportamqp_methods_create("testhost", "testdevice");

    /// assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_IS_NULL(amqp_methods_handle);
}

/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_116: [ If `mallocAndStrcpy_s` fails, `iothubtransportamqp_methods_create` shall return NULL. ]*/
TEST_FUNCTION(when_copying_the_device_id_fails_iothubtransportamqp_methods_create_fails)
{
    /// arrange
    IOTHUBTRANSPORT_AMQP_METHODS_HANDLE amqp_methods_handle;
    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));
    STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, "testdevice"))
        .IgnoreArgument_destination()
        .SetReturn(1);
    EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));

    /// act
    amqp_methods_handle = iothubtransportamqp_methods_create("testhost", "testdevice");

    /// assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_IS_NULL(amqp_methods_handle);
}

/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_116: [ If `mallocAndStrcpy_s` fails, `iothubtransportamqp_methods_create` shall return NULL. ]*/
TEST_FUNCTION(when_copying_the_hostname_fails_iothubtransportamqp_methods_create_fails)
{
	/// arrange
	IOTHUBTRANSPORT_AMQP_METHODS_HANDLE amqp_methods_handle;
	EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));
	STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, "testdevice"))
		.IgnoreArgument_destination();
	STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, "testhost"))
		.IgnoreArgument_destination()
		.SetReturn(42);
	EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));
	EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));

	/// act
	amqp_methods_handle = iothubtransportamqp_methods_create("testhost", "testdevice");

	/// assert
	ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
	ASSERT_IS_NULL(amqp_methods_handle);
}

/* iothubtransportamqp_methods_destroy */

/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_005: [ `iothubtransportamqp_methods_destroy` shall free all resources allocated by `iothubtransportamqp_methods_create` for the handle `iothubtransport_amqp_methods_handle`. ]*/
TEST_FUNCTION(iothubtransportamqp_methods_destroy_frees_the_device_id_and_handle)
{
    /// arrange
    IOTHUBTRANSPORT_AMQP_METHODS_HANDLE amqp_methods_handle = iothubtransportamqp_methods_create("testhost", "testdevice");
    umock_c_reset_all_calls();

	EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));
    EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));
    EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));

    /// act
    iothubtransportamqp_methods_destroy(amqp_methods_handle);

    /// assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
}

/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_006: [ If `iothubtransport_amqp_methods_handle` is NULL, `iothubtransportamqp_methods_destroy` shall do nothing. ]*/
TEST_FUNCTION(iothubtransportamqp_methods_destroy_with_NULL_frees_nothing)
{
    /// arrange

    /// act
    iothubtransportamqp_methods_destroy(NULL);

    /// assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
}

/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_007: [ If the instance pointed to by `iothubtransport_amqp_methods_handle` is subscribed to receive C2D methods, `iothubtransportamqp_methods_destroy` shall free all resources allocated by the subscribe. ]*/
TEST_FUNCTION(destroy_after_subscribe_also_destroys_links_and_the_message_receiver_and_sender)
{
    /// arrange
    IOTHUBTRANSPORT_AMQP_METHODS_HANDLE amqp_methods_handle = iothubtransportamqp_methods_create("testhost", "testdevice");
    umock_c_reset_all_calls();
    setup_subscribe_expected_calls();
    (void)iothubtransportamqp_methods_subscribe(amqp_methods_handle, TEST_SESSION_HANDLE, test_on_methods_error, (void*)0x4242, test_on_method_request_received, (void*)0x4243);
    setup_subscribe_expected_calls();
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(messagereceiver_destroy(TEST_MESSAGE_RECEIVER));
    STRICT_EXPECTED_CALL(messagesender_destroy(TEST_MESSAGE_SENDER));
    STRICT_EXPECTED_CALL(link_destroy(TEST_SENDER_LINK));
    STRICT_EXPECTED_CALL(link_destroy(TEST_RECEIVER_LINK));
    EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));
    EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));
	EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));

    /// act
    iothubtransportamqp_methods_destroy(amqp_methods_handle);

    /// assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
}

/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_110: [ `iothubtransportamqp_methods_unsubscribe` shall free all tracked method handles indicated to the user via the callback `on_method_request_received` and than have not yet been completed by calls to `iothubtransportamqp_methods_respond`. ]*/
TEST_FUNCTION(iothubtransportamqp_methods_destroy_frees_tracked_handles)
{
    /// arrange
    IOTHUBTRANSPORT_AMQP_METHODS_HANDLE amqp_methods_handle = iothubtransportamqp_methods_create("testhost", "testdevice");
    umock_c_reset_all_calls();
    setup_subscribe_expected_calls();
    (void)iothubtransportamqp_methods_subscribe(amqp_methods_handle, TEST_SESSION_HANDLE, test_on_methods_error, (void*)0x4242, test_on_method_request_received, (void*)0x4243);
    umock_c_reset_all_calls();
    setup_message_received_calls();
    g_on_message_received(amqp_methods_handle, TEST_UAMQP_MESSAGE);
    iothubtransportamqp_methods_unsubscribe(amqp_methods_handle);
    umock_c_reset_all_calls();

    /* one extra free for the handle and one extra for the handle array */
    EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));
    EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));

    EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));
    EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));
	EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));

    /// act
    iothubtransportamqp_methods_destroy(amqp_methods_handle);

    /// assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
}

/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_110: [ `iothubtransportamqp_methods_unsubscribe` shall free all tracked method handles indicated to the user via the callback `on_method_request_received` and than have not yet been completed by calls to `iothubtransportamqp_methods_respond`. ]*/
TEST_FUNCTION(iothubtransportamqp_methods_destroy_frees_2_tracked_handles)
{
    /// arrange
    IOTHUBTRANSPORT_AMQP_METHODS_HANDLE amqp_methods_handle = iothubtransportamqp_methods_create("testhost", "testdevice");
    umock_c_reset_all_calls();
    setup_subscribe_expected_calls();
    (void)iothubtransportamqp_methods_subscribe(amqp_methods_handle, TEST_SESSION_HANDLE, test_on_methods_error, (void*)0x4242, test_on_method_request_received, (void*)0x4243);
    umock_c_reset_all_calls();
    setup_message_received_calls();
    g_on_message_received(amqp_methods_handle, TEST_UAMQP_MESSAGE);
    setup_message_received_calls();
    g_on_message_received(amqp_methods_handle, TEST_UAMQP_MESSAGE);
    iothubtransportamqp_methods_unsubscribe(amqp_methods_handle);
    umock_c_reset_all_calls();

    /* 2 extra frees for the handle and one extra for the handle array */
    EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));
    EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));
    EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));

    EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));
    EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));
	EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));

    /// act
    iothubtransportamqp_methods_destroy(amqp_methods_handle);

    /// assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
}

/* iothubtransportamqp_methods_subscribe */

/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_008: [ On success it shall return 0. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_010: [ `iothubtransportamqp_methods_subscribe` shall create a receiver link by calling `link_create` with the following arguments: ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_011: [ - `session_handle` shall be the session_handle argument passed to iothubtransportamqp_methods_subscribe ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_012: [ - `name` shall be in the format `methods_requests_link-{device_id}`, where device_id is the `device_id` argument passed to `iothubtransportamqp_methods_create`. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_013: [ - `role` shall be role_receiver. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_014: [ - `source` shall be the a source value created by calling `messaging_create_source`. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_014: [ - `source` shall be the a source value created by calling `messaging_create_source`. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_015: [ The address string used to create the source shall be of the form `/devices/{device id}/methods/devicebound`. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_016: [ The string shall be created by using `STRING_construct_sprintf`. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_017: [ - `target` shall be the a target value created by calling `messaging_create_target`. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_117: [ The address string used to create the target shall be `requests`. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_021: [ `iothubtransportamqp_methods_subscribe` shall create a sender link by calling `link_create` with the following arguments: ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_022: [ - `session_handle` shall be the session_handle argument passed to iothubtransportamqp_methods_subscribe ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_023: [ - `name` shall be format `methods_responses_link-{device_id}`, where device_id is the `device_id` argument passed to `iothubtransportamqp_methods_create`. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_024: [ - `role` shall be role_sender. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_025: [ - `source` shall be the a source value created by calling `messaging_create_source`. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_026: [ The address string used to create the target shall be `responses`. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_027: [ - `target` shall be the a target value created by calling `messaging_create_target`. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_028: [ The address string used to create the source shall be of the form `/devices/{device id}/methods/devicebound`. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_105: [ The string created in order to hold the source and target addresses shall be freed by calling `STRING_delete`. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_140: [ A link attach properties map shall be created by calling `amqpvalue_create_map`. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_141: [ A property key which shall be a symbol named `com.microsoft:channel-correlation-id` shall be created by calling `amqp_create_symbol`. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_142: [ A property value of type string that shall contain the device id shall be created by calling `amqpvalue_create_string`. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_143: [ The `com.microsoft:channel-correlation-id` shall be added to the link attach properties by calling `amqpvalue_set_map_value`. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_150: [ A property key which shall be a symbol named `com.microsoft:api-version` shall be created by calling `amqp_create_symbol`. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_151: [ A property value of type string that shall contain the `2016-11-14` shall be created by calling `amqpvalue_create_string`. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_152: [ The `com.microsoft:api-version` shall be added to the link attach properties by calling `amqpvalue_set_map_value`. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_144: [ The link attach properties shall be set on the receiver and sender link by calling `link_set_attach_properties`. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_146: [ The link attach properties and all associated values shall be freed by calling `amqpvalue_destroy` after setting the link attach properties. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_033: [ `iothubtransportamqp_methods_subscribe` shall create a message receiver associated with the receiver link by calling `messagereceiver_create` and passing the receiver link handle to it. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_118: [ An `on_message_receiver_state_changed` callback together with its context shall be passed to `messagereceiver_create`. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_035: [ `iothubtransportamqp_methods_subscribe` shall create a message sender associated with the sender link by calling `messagesender_create` and passing the sender link handle to it. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_106: [ An `on_message_sender_state_changed` callback together with its context shall be passed to `messagesender_create`. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_037: [ `iothubtransportamqp_methods_subscribe` shall open the message sender by calling `messagesender_open`. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_039: [ `iothubtransportamqp_methods_subscribe` shall open the message sender by calling `messagereceiver_open`. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_104: [ An `on_message_received` callback together with its context shall be passed to `messagereceiver_open`. ]*/
TEST_FUNCTION(iothubtransportamqp_methods_subscribe_creates_links_and_sender_and_receiver)
{
    /// arrange
    IOTHUBTRANSPORT_AMQP_METHODS_HANDLE amqp_methods_handle = iothubtransportamqp_methods_create("testhost", "testdevice");
    int result;

    umock_c_reset_all_calls();
    setup_subscribe_expected_calls();

    /// act
    result = iothubtransportamqp_methods_subscribe(amqp_methods_handle, TEST_SESSION_HANDLE, test_on_methods_error, (void*)0x4242, test_on_method_request_received, (void*)0x4243);

    /// assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_ARE_EQUAL(int, 0, result);

    /// cleanup
    iothubtransportamqp_methods_destroy(amqp_methods_handle);
}

/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_137: [ `iothubtransportamqp_methods_subscribe` after another succesfull `iothubtransportamqp_methods_subscribe` without any unsubscribe shall return a non-zero value without performing any subscribe action. ]*/
TEST_FUNCTION(iothubtransportamqp_methods_subscribe_after_subscribe_fails)
{
    /// arrange
    IOTHUBTRANSPORT_AMQP_METHODS_HANDLE amqp_methods_handle = iothubtransportamqp_methods_create("testhost", "testdevice");
    int result;

    umock_c_reset_all_calls();
    setup_subscribe_expected_calls();
    (void)iothubtransportamqp_methods_subscribe(amqp_methods_handle, TEST_SESSION_HANDLE, test_on_methods_error, (void*)0x4242, test_on_method_request_received, (void*)0x4243);
    umock_c_reset_all_calls();

    /// act
    result = iothubtransportamqp_methods_subscribe(amqp_methods_handle, TEST_SESSION_HANDLE, test_on_methods_error, (void*)0x4242, test_on_method_request_received, (void*)0x4243);

    /// assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_ARE_NOT_EQUAL(int, 0, result);

    /// cleanup
    iothubtransportamqp_methods_destroy(amqp_methods_handle);
}

/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_009: [ If any of the argument `iothubtransport_amqp_methods_handle`, `session_handle`, `on_methods_error` or `on_method_request_received` is NULL, `iothubtransportamqp_methods_subscribe` shall fail and return a non-zero value. ]*/
TEST_FUNCTION(when_iothubtransport_amqp_methods_handle_is_NULL_iothubtransportamqp_methods_subscribe_fails)
{
    /// arrange
    IOTHUBTRANSPORT_AMQP_METHODS_HANDLE amqp_methods_handle = iothubtransportamqp_methods_create("testhost", "testdevice");
    int result;
    umock_c_reset_all_calls();

    /// act
    result = iothubtransportamqp_methods_subscribe(NULL, TEST_SESSION_HANDLE, test_on_methods_error, (void*)0x4242, test_on_method_request_received, (void*)0x4243);

    /// assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_ARE_NOT_EQUAL(int, 0, result);

    /// cleanup
    iothubtransportamqp_methods_destroy(amqp_methods_handle);
}

/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_009: [ If any of the argument `iothubtransport_amqp_methods_handle`, `session_handle`, `on_methods_error` or `on_method_request_received` is NULL, `iothubtransportamqp_methods_subscribe` shall fail and return a non-zero value. ]*/
TEST_FUNCTION(iothubtransportamqp_methods_subscribe_with_NULL_session_fails)
{
    /// arrange
    IOTHUBTRANSPORT_AMQP_METHODS_HANDLE amqp_methods_handle = iothubtransportamqp_methods_create("testhost", "testdevice");
    int result;
    umock_c_reset_all_calls();

    /// act
    result = iothubtransportamqp_methods_subscribe(amqp_methods_handle, NULL, test_on_methods_error, (void*)0x4242, test_on_method_request_received, (void*)0x4243);

    /// assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_ARE_NOT_EQUAL(int, 0, result);

    /// cleanup
    iothubtransportamqp_methods_destroy(amqp_methods_handle);
}

/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_009: [ If any of the argument `iothubtransport_amqp_methods_handle`, `session_handle`, `on_methods_error` or `on_method_request_received` is NULL, `iothubtransportamqp_methods_subscribe` shall fail and return a non-zero value. ]*/
TEST_FUNCTION(iothubtransportamqp_methods_subscribe_with_NULL_on_methods_error_callback_fails)
{
    /// arrange
    IOTHUBTRANSPORT_AMQP_METHODS_HANDLE amqp_methods_handle = iothubtransportamqp_methods_create("testhost", "testdevice");
    int result;
    umock_c_reset_all_calls();

    /// act
    result = iothubtransportamqp_methods_subscribe(amqp_methods_handle, TEST_SESSION_HANDLE, NULL, (void*)0x4242, test_on_method_request_received, (void*)0x4243);

    /// assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_ARE_NOT_EQUAL(int, 0, result);

    /// cleanup
    iothubtransportamqp_methods_destroy(amqp_methods_handle);
}

/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_009: [ If any of the argument `iothubtransport_amqp_methods_handle`, `session_handle`, `on_methods_error` or `on_method_request_received` is NULL, `iothubtransportamqp_methods_subscribe` shall fail and return a non-zero value. ]*/
TEST_FUNCTION(iothubtransportamqp_methods_subscribe_with_NULL_on_method_request_received_callback_fails)
{
    /// arrange
    IOTHUBTRANSPORT_AMQP_METHODS_HANDLE amqp_methods_handle = iothubtransportamqp_methods_create("testhost", "testdevice");
    int result;
    umock_c_reset_all_calls();

    /// act
    result = iothubtransportamqp_methods_subscribe(amqp_methods_handle, TEST_SESSION_HANDLE, test_on_methods_error, (void*)0x4242, NULL, (void*)0x4243);

    /// assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_ARE_NOT_EQUAL(int, 0, result);

    /// cleanup
    iothubtransportamqp_methods_destroy(amqp_methods_handle);
}

/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_008: [ On success it shall return 0. ]*/
TEST_FUNCTION(iothubtransportamqp_methods_subscribe_with_NULL_error_context_succeeds)
{
    /// arrange
    IOTHUBTRANSPORT_AMQP_METHODS_HANDLE amqp_methods_handle = iothubtransportamqp_methods_create("testhost", "testdevice");
    int result;

    umock_c_reset_all_calls();
    setup_subscribe_expected_calls();

    /// act
    result = iothubtransportamqp_methods_subscribe(amqp_methods_handle, TEST_SESSION_HANDLE, test_on_methods_error, NULL, test_on_method_request_received, (void*)0x4243);

    /// assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_ARE_EQUAL(int, 0, result);

    /// cleanup
    iothubtransportamqp_methods_destroy(amqp_methods_handle);
}

/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_008: [ On success it shall return 0. ]*/
TEST_FUNCTION(iothubtransportamqp_methods_subscribe_with_NULL_methods_receive_callback_context_succeeds)
{
    /// arrange
    IOTHUBTRANSPORT_AMQP_METHODS_HANDLE amqp_methods_handle = iothubtransportamqp_methods_create("testhost", "testdevice");
    int result;

    umock_c_reset_all_calls();
    setup_subscribe_expected_calls();

    /// act
    result = iothubtransportamqp_methods_subscribe(amqp_methods_handle, TEST_SESSION_HANDLE, test_on_methods_error, (void*)0x4243, test_on_method_request_received, NULL);

    /// assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_ARE_EQUAL(int, 0, result);

    /// cleanup
    iothubtransportamqp_methods_destroy(amqp_methods_handle);
}

/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_153: [ If constructing the requests link name fails, `iothubtransportamqp_methods_subscribe` shall fail and return a non-zero value. ]*/
TEST_FUNCTION(when_constructing_the_requests_link_name_fails_iothubtransportamqp_methods_subscribe_fails)
{
	/// arrange
	IOTHUBTRANSPORT_AMQP_METHODS_HANDLE amqp_methods_handle = iothubtransportamqp_methods_create("testhost", "testdevice");
	int result;

	umock_c_reset_all_calls();

	g_when_shall_STRING_construct_sprintf_fail = 2;
	STRICT_EXPECTED_CALL(STRING_c_str(TEST_STRING_HANDLE))
		.SetReturn("/devices/test_device/methods/devicebound");
	STRICT_EXPECTED_CALL(messaging_create_source("/devices/test_device/methods/devicebound"))
		.SetReturn(TEST_RECEIVER_SOURCE);
	STRICT_EXPECTED_CALL(messaging_create_target("requests"))
		.SetReturn(TEST_RECEIVER_TARGET);
	STRICT_EXPECTED_CALL(amqpvalue_destroy(TEST_RECEIVER_TARGET));
	STRICT_EXPECTED_CALL(amqpvalue_destroy(TEST_RECEIVER_SOURCE));
	STRICT_EXPECTED_CALL(STRING_delete(TEST_STRING_HANDLE));

	/// act
	result = iothubtransportamqp_methods_subscribe(amqp_methods_handle, TEST_SESSION_HANDLE, test_on_methods_error, (void*)0x4242, test_on_method_request_received, (void*)0x4243);

	/// assert
	ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
	ASSERT_ARE_NOT_EQUAL(int, 0, result);

	/// cleanup
	iothubtransportamqp_methods_destroy(amqp_methods_handle);
}

/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_154: [ If constructing the responses link name fails, `iothubtransportamqp_methods_subscribe` shall fail and return a non-zero value. ]*/
TEST_FUNCTION(when_constructing_the_responses_link_name_fails_iothubtransportamqp_methods_subscribe_fails)
{
	/// arrange
	IOTHUBTRANSPORT_AMQP_METHODS_HANDLE amqp_methods_handle = iothubtransportamqp_methods_create("testhost", "testdevice");
	int result;

	umock_c_reset_all_calls();

	g_when_shall_STRING_construct_sprintf_fail = 3;
	STRICT_EXPECTED_CALL(STRING_c_str(TEST_STRING_HANDLE))
		.SetReturn("/devices/test_device/methods/devicebound");
	STRICT_EXPECTED_CALL(messaging_create_source("/devices/test_device/methods/devicebound"))
		.SetReturn(TEST_RECEIVER_SOURCE);
	STRICT_EXPECTED_CALL(messaging_create_target("requests"))
		.SetReturn(TEST_RECEIVER_TARGET);
	STRICT_EXPECTED_CALL(STRING_c_str(TEST_STRING_HANDLE))
		.SetReturn("methods_requests_link-testdevice");
	STRICT_EXPECTED_CALL(link_create(TEST_SESSION_HANDLE, "methods_requests_link-testdevice", role_receiver, TEST_RECEIVER_SOURCE, TEST_RECEIVER_TARGET))
		.SetReturn(TEST_RECEIVER_LINK);
	STRICT_EXPECTED_CALL(messaging_create_source("responses"))
		.SetReturn(TEST_SENDER_SOURCE);
	STRICT_EXPECTED_CALL(STRING_c_str(TEST_STRING_HANDLE))
		.SetReturn("/devices/test_device/methods/devicebound");
	STRICT_EXPECTED_CALL(messaging_create_target("/devices/test_device/methods/devicebound"))
		.SetReturn(TEST_SENDER_TARGET);
	STRICT_EXPECTED_CALL(amqpvalue_destroy(TEST_SENDER_TARGET));
	STRICT_EXPECTED_CALL(amqpvalue_destroy(TEST_SENDER_SOURCE));
	STRICT_EXPECTED_CALL(STRING_delete(TEST_STRING_HANDLE));
	STRICT_EXPECTED_CALL(amqpvalue_destroy(TEST_RECEIVER_TARGET));
	STRICT_EXPECTED_CALL(amqpvalue_destroy(TEST_RECEIVER_SOURCE));
	STRICT_EXPECTED_CALL(STRING_delete(TEST_STRING_HANDLE));

	/// act
	result = iothubtransportamqp_methods_subscribe(amqp_methods_handle, TEST_SESSION_HANDLE, test_on_methods_error, (void*)0x4242, test_on_method_request_received, (void*)0x4243);

	/// assert
	ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
	ASSERT_ARE_NOT_EQUAL(int, 0, result);

	/// cleanup
	iothubtransportamqp_methods_destroy(amqp_methods_handle);
}

/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_019: [ If creating the target or source values fails `iothubtransportamqp_methods_subscribe` shall fail and return a non-zero value. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_020: [ If creating the receiver link fails `iothubtransportamqp_methods_subscribe` shall fail and return a non-zero value. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_031: [ If creating the target or source values fails `iothubtransportamqp_methods_subscribe` shall fail and return a non-zero value. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_032: [ If creating the receiver link fails `iothubtransportamqp_methods_subscribe` shall fail and return a non-zero value. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_034: [ If `messagereceiver_create` fails, `iothubtransportamqp_methods_subscribe` shall fail and return a non-zero value. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_036: [ If `messagesender_create` fails, `iothubtransportamqp_methods_subscribe` shall fail and return a non-zero value. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_038: [ If `messagesender_open` fails, `iothubtransportamqp_methods_subscribe` shall fail and return a non-zero value. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_040: [ If `messagereceiver_open` fails, `iothubtransportamqp_methods_subscribe` shall fail and return a non-zero value. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_145: [ If any call for creating or setting the link attach properties fails `iothubtransportamqp_methods_subscribe` shall fail and return a non-zero value. ]*/
TEST_FUNCTION(when_a_failure_occurs_iothubtransportamqp_methods_subscribe_fails)
{
    ///arrange
    int negativeTestsInitResult = umock_c_negative_tests_init();
    IOTHUBTRANSPORT_AMQP_METHODS_HANDLE amqp_methods_handle;
    ASSERT_ARE_EQUAL(int, 0, negativeTestsInitResult);
    amqp_methods_handle = iothubtransportamqp_methods_create("testhost", "testdevice");
    int result;

    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(STRING_c_str(TEST_STRING_HANDLE))
        .SetReturn("/devices/test_device/methods/devicebound");
    STRICT_EXPECTED_CALL(messaging_create_source("/devices/test_device/methods/devicebound"))
        .SetReturn(TEST_RECEIVER_SOURCE).SetFailReturn(NULL);
    STRICT_EXPECTED_CALL(messaging_create_target("requests"))
        .SetReturn(TEST_RECEIVER_TARGET).SetFailReturn(NULL);
	STRICT_EXPECTED_CALL(STRING_c_str(TEST_STRING_HANDLE))
		.SetReturn("methods_requests_link-testdevice");
	STRICT_EXPECTED_CALL(link_create(TEST_SESSION_HANDLE, "methods_requests_link-testdevice", role_receiver, TEST_RECEIVER_SOURCE, TEST_RECEIVER_TARGET))
        .SetReturn(TEST_RECEIVER_LINK).SetFailReturn(NULL);
    STRICT_EXPECTED_CALL(messaging_create_source("responses"))
        .SetReturn(TEST_SENDER_SOURCE).SetFailReturn(NULL);
    STRICT_EXPECTED_CALL(STRING_c_str(TEST_STRING_HANDLE))
        .SetReturn("/devices/test_device/methods/devicebound");
    STRICT_EXPECTED_CALL(messaging_create_target("/devices/test_device/methods/devicebound"))
        .SetReturn(TEST_SENDER_TARGET).SetFailReturn(NULL);
	STRICT_EXPECTED_CALL(STRING_c_str(TEST_STRING_HANDLE))
		.SetReturn("methods_responses_link-testdevice");
	STRICT_EXPECTED_CALL(link_create(TEST_SESSION_HANDLE, "methods_responses_link-testdevice", role_sender, TEST_SENDER_SOURCE, TEST_SENDER_TARGET))
        .SetReturn(TEST_SENDER_LINK).SetFailReturn(NULL);
	STRICT_EXPECTED_CALL(amqpvalue_create_map())
		.SetReturn(LINK_ATTACH_PROPERTIES_MAP).SetFailReturn(NULL);
	STRICT_EXPECTED_CALL(amqpvalue_create_symbol("com.microsoft:channel-correlation-id"))
		.SetReturn(CHANNEL_CORRELATION_ID_KEY).SetFailReturn(NULL);
	STRICT_EXPECTED_CALL(amqpvalue_create_string("testdevice"))
		.SetReturn(CHANNEL_CORRELATION_ID_VALUE).SetFailReturn(NULL);
	STRICT_EXPECTED_CALL(amqpvalue_set_map_value(LINK_ATTACH_PROPERTIES_MAP, CHANNEL_CORRELATION_ID_KEY, CHANNEL_CORRELATION_ID_VALUE))
		.SetFailReturn(42);
	STRICT_EXPECTED_CALL(amqpvalue_create_symbol("com.microsoft:api-version"))
		.SetReturn(API_VERSION_KEY).SetFailReturn(NULL);
	STRICT_EXPECTED_CALL(amqpvalue_create_string("2016-11-14"))
		.SetReturn(API_VERSION_VALUE).SetFailReturn(NULL);
	STRICT_EXPECTED_CALL(amqpvalue_set_map_value(LINK_ATTACH_PROPERTIES_MAP, API_VERSION_KEY, API_VERSION_VALUE))
		.SetFailReturn(42);
	STRICT_EXPECTED_CALL(link_set_attach_properties(TEST_SENDER_LINK, LINK_ATTACH_PROPERTIES_MAP))
		.SetFailReturn(42);
	STRICT_EXPECTED_CALL(link_set_attach_properties(TEST_RECEIVER_LINK, LINK_ATTACH_PROPERTIES_MAP))
		.SetFailReturn(43);
	STRICT_EXPECTED_CALL(amqpvalue_destroy(API_VERSION_VALUE));
	STRICT_EXPECTED_CALL(amqpvalue_destroy(API_VERSION_KEY));
	STRICT_EXPECTED_CALL(amqpvalue_destroy(CHANNEL_CORRELATION_ID_VALUE));
	STRICT_EXPECTED_CALL(amqpvalue_destroy(CHANNEL_CORRELATION_ID_KEY));
	STRICT_EXPECTED_CALL(amqpvalue_destroy(LINK_ATTACH_PROPERTIES_MAP));
	STRICT_EXPECTED_CALL(messagereceiver_create(TEST_RECEIVER_LINK, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument_on_message_receiver_state_changed()
        .IgnoreArgument_context()
        .SetFailReturn(NULL);
    STRICT_EXPECTED_CALL(messagesender_create(TEST_SENDER_LINK, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument_on_message_sender_state_changed()
        .IgnoreArgument_context()
        .SetFailReturn(NULL);
    STRICT_EXPECTED_CALL(messagesender_open(TEST_MESSAGE_SENDER))
        .SetFailReturn(1);
    STRICT_EXPECTED_CALL(messagereceiver_open(TEST_MESSAGE_RECEIVER, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument_on_message_received()
        .IgnoreArgument_callback_context()
        .SetFailReturn(1);
    STRICT_EXPECTED_CALL(STRING_delete(TEST_STRING_HANDLE));

    umock_c_negative_tests_snapshot();

    for (size_t i = 0; i < umock_c_negative_tests_call_count(); i++)
    {
        if ((i != 0) && // STRING_c_str
			(i != 3) && // STRING_c_str
            (i != 6) && // STRING_c_str
			(i != 8) && // STRING_c_str
			(i != 19) && // amqpvalue_destroy
			(i != 20) && // amqpvalue_destroy
			(i != 21) && // amqpvalue_destroy
			(i != 22) && // amqpvalue_destroy
			(i != 23) && // amqpvalue_destroy
			(i != 28)) // STRING_delete
		{
            umock_c_negative_tests_reset();
            umock_c_negative_tests_fail_call(i);

            char temp_str[128];
            (void)sprintf(temp_str, "On failed call %zu", i);

            ///act
            result = iothubtransportamqp_methods_subscribe(amqp_methods_handle, TEST_SESSION_HANDLE, test_on_methods_error, (void*)0x4242, test_on_method_request_received, (void*)0x4243);

            ///assert
            ASSERT_ARE_NOT_EQUAL_WITH_MSG(int, 0, result, temp_str);
        }
    }

    ///cleanup
    iothubtransportamqp_methods_destroy(amqp_methods_handle);
    umock_c_negative_tests_deinit();
}

/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_018: [ If `STRING_construct_sprintf` fails `iothubtransportamqp_methods_subscribe` shall fail and return a non-zero value. ]*/
TEST_FUNCTION(when_STRING_construct_sprintf_fails_iothubtransportamqp_methods_subscribe_fails)
{
    /// arrange
    IOTHUBTRANSPORT_AMQP_METHODS_HANDLE amqp_methods_handle = iothubtransportamqp_methods_create("testhost", "testdevice");
    int result;

    umock_c_reset_all_calls();
    g_when_shall_STRING_construct_sprintf_fail = 1;

    /// act
    result = iothubtransportamqp_methods_subscribe(amqp_methods_handle, TEST_SESSION_HANDLE, test_on_methods_error, (void*)0x4242, test_on_method_request_received, (void*)0x4243);

    /// assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_ARE_NOT_EQUAL(int, 0, result);

    /// cleanup
    iothubtransportamqp_methods_destroy(amqp_methods_handle);
}

/* iothubtransportamqp_methods_unsubscribe */

/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_073: [ Otherwise `iothubtransportamqp_methods_unsubscribe` shall free all resources allocated in `iothubtransportamqp_methods_subscribe`: ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_075: [ - It shall free the message receiver by calling `messagereceiver_destroy'. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_074: [ - It shall free the message sender by calling `messagesender_destroy'. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_076: [ - It shall free the sender link by calling `link_destroy'. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_077: [ - It shall free the receiver link by calling `link_destroy'. ]*/
TEST_FUNCTION(iothubtransportamqp_methods_unsubscribe_frees_the_message_sender_receiver_and_links)
{
    /// arrange
    IOTHUBTRANSPORT_AMQP_METHODS_HANDLE amqp_methods_handle = iothubtransportamqp_methods_create("testhost", "testdevice");
    umock_c_reset_all_calls();
    setup_subscribe_expected_calls();
    (void)iothubtransportamqp_methods_subscribe(amqp_methods_handle, TEST_SESSION_HANDLE, test_on_methods_error, (void*)0x4242, test_on_method_request_received, (void*)0x4243);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(messagereceiver_destroy(TEST_MESSAGE_RECEIVER));
    STRICT_EXPECTED_CALL(messagesender_destroy(TEST_MESSAGE_SENDER));
    STRICT_EXPECTED_CALL(link_destroy(TEST_SENDER_LINK));
    STRICT_EXPECTED_CALL(link_destroy(TEST_RECEIVER_LINK));

    /// act
    iothubtransportamqp_methods_unsubscribe(amqp_methods_handle);

    /// assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    /// cleanup
    iothubtransportamqp_methods_destroy(amqp_methods_handle);
}

/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_072: [ If the argument `iothubtransport_amqp_methods_handle` is NULL, `iothubtransportamqp_methods_unsubscribe` shall do nothing. ]*/
TEST_FUNCTION(iothubtransportamqp_methods_unsubscribe_with_NULL_does_not_destroy_links_and_message_handlers)
{
    /// arrange

    /// act
    iothubtransportamqp_methods_unsubscribe(NULL);

    /// assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
}

/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_135: [ If subscribe was not called yet, `iothubtransportamqp_methods_unsubscribe` shall do nothing. ]*/
TEST_FUNCTION(iothubtransportamqp_methods_unsubscribe_before_subscribe_does_not_destroy_links_and_message_handlers)
{
    /// arrange
    IOTHUBTRANSPORT_AMQP_METHODS_HANDLE amqp_methods_handle = iothubtransportamqp_methods_create("testhost", "testdevice");
    umock_c_reset_all_calls();

    /// act
    iothubtransportamqp_methods_unsubscribe(amqp_methods_handle);

    /// assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    /// cleanup
    iothubtransportamqp_methods_destroy(amqp_methods_handle);
}

/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_136: [ `iothubtransportamqp_methods_unsubscribe` after `iothubtransportamqp_methods_unsubscribe` shall do nothing. ]*/
TEST_FUNCTION(iothubtransportamqp_methods_unsubscribe_after_unsubscribe_does_not_destroy_links_and_message_handlers)
{
    /// arrange
    IOTHUBTRANSPORT_AMQP_METHODS_HANDLE amqp_methods_handle = iothubtransportamqp_methods_create("testhost", "testdevice");
    umock_c_reset_all_calls();
    setup_subscribe_expected_calls();
    (void)iothubtransportamqp_methods_subscribe(amqp_methods_handle, TEST_SESSION_HANDLE, test_on_methods_error, (void*)0x4242, test_on_method_request_received, (void*)0x4243);
    iothubtransportamqp_methods_unsubscribe(amqp_methods_handle);
    umock_c_reset_all_calls();

    /// act
    iothubtransportamqp_methods_unsubscribe(amqp_methods_handle);

    /// assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    /// cleanup
    iothubtransportamqp_methods_destroy(amqp_methods_handle);
}

/* on_message_received */

/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_043: [ When `on_message_received` is called (to indicate a new message being received over the receiver link), the message shall be processed as below: ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_044: [ - The message properties shall be obtained by calling `message_get_properties`. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_046: [ - The correlation id shall be obtained by calling `properties_get_correlation_id` on the message properties. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_121: [ The uuid value for the correlation ID shall be obtained by calling `amqpvalue_get_uuid`. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_048: [ - The message payload shall be obtained by calling `message_get_body_amqp_data` with the index argument being 0. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_099: [ The application properties for the received message shall be obtained by calling `message_get_application_properties`. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_123: [ The AMQP map shall be retrieve from the application properties by calling `amqpvalue_get_inplace_described_value`. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_100: [ A property key `IoThub-methodname` shall be created by calling `amqpvalue_create_string`. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_101: [ The method name property value shall be found in the map by calling `amqpvalue_get_map_value`. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_102: [ The string contained by the property value shall be obtained by calling `amqpvalue_get_string`. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_050: [ The binary message payload shall be indicated by calling the `on_method_request_received` callback passed to `iothubtransportamqp_methods_subscribe` with the arguments: ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_051: [ - `context` shall be set to the `on_method_request_received_context` argument passed to `iothubtransportamqp_methods_subscribe`. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_098: [ - `method_name` shall be set to the application property value for `IoThub-methodname`. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_052: [ - `request` shall be set to the payload bytes obtained by calling `message_get_body_amqp_data`. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_053: [ - `request_size` shall be set to the payload size obtained by calling `message_get_body_amqp_data`. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_054: [ - `method_handle` shall be set to a newly created `IOTHUBTRANSPORT_AMQP_METHOD_HANDLE` that can be passed later as an argument to `iothubtransportamqp_methods_respond`. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_112: [ Memory shall be allocated for the `IOTHUBTRANSPORT_AMQP_METHOD_HANDLE` to hold the correlation-id, so that it can be used in the `iothubtransportamqp_methods_respond` function. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_056: [ On success the `on_message_received` callback shall return a newly constructed delivery state obtained by calling `messaging_delivery_accepted`. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_113: [ All `IOTHUBTRANSPORT_AMQP_METHOD_HANDLE` handles shall be tracked in an array of handles that shall be resized accordingly when a methopd handle is added to it. ]*/
TEST_FUNCTION(when_a_message_is_received_a_new_method_request_is_indicated)
{
    /// arrange
    IOTHUBTRANSPORT_AMQP_METHODS_HANDLE amqp_methods_handle = iothubtransportamqp_methods_create("testhost", "testdevice");
    AMQP_VALUE result;
    umock_c_reset_all_calls();
    setup_subscribe_expected_calls();
    (void)iothubtransportamqp_methods_subscribe(amqp_methods_handle, TEST_SESSION_HANDLE, test_on_methods_error, (void*)0x4242, test_on_method_request_received, (void*)0x4243);
    umock_c_reset_all_calls();

    setup_message_received_calls();

    /// act
    result = g_on_message_received(amqp_methods_handle, TEST_UAMQP_MESSAGE);

    /// assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_ARE_EQUAL(void_ptr, TEST_DELIVERY_ACCEPTED, result);

    /// cleanup
    iothubtransportamqp_methods_destroy(amqp_methods_handle);
}

/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_041: [ If `message` is NULL, the RELEASED outcome shall be returned and an error shall be indicated. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_129: [ The released outcome shall be created by calling `messaging_delivery_released`. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_128: [ When the RELEASED outcome is returned, an error shall be indicated by calling the `on_methods_error` callback passed to `iothubtransportamqp_methods_subscribe`. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_042: [ When an error is indicated by calling the `on_methods_error`, it shall be called with the context being the `on_methods_error_context` argument passed to `iothubtransportamqp_methods_subscribe`. ]*/
TEST_FUNCTION(when_a_message_is_received_and_a_NULL_message_handle_is_passed_the_error_shall_be_indicated_through_on_methods_error)
{
    /// arrange
    IOTHUBTRANSPORT_AMQP_METHODS_HANDLE amqp_methods_handle = iothubtransportamqp_methods_create("testhost", "testdevice");
    AMQP_VALUE result;
    umock_c_reset_all_calls();
    setup_subscribe_expected_calls();
    (void)iothubtransportamqp_methods_subscribe(amqp_methods_handle, TEST_SESSION_HANDLE, test_on_methods_error, (void*)0x4242, test_on_method_request_received, (void*)0x4243);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(messaging_delivery_released());
    STRICT_EXPECTED_CALL(test_on_methods_error((void*)0x4242));

    /// act
    result = g_on_message_received(amqp_methods_handle, NULL);

    /// assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_ARE_EQUAL(void_ptr, TEST_DELIVERY_RELEASED, result);

    /// cleanup
    iothubtransportamqp_methods_destroy(amqp_methods_handle);
}

/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_045: [ If `message_get_properties` fails, the REJECTED outcome with `amqp:decode-error` shall be returned. ]*/
TEST_FUNCTION(when_message_get_properties_fails_the_message_is_rejected)
{
    /// arrange
    IOTHUBTRANSPORT_AMQP_METHODS_HANDLE amqp_methods_handle = iothubtransportamqp_methods_create("testhost", "testdevice");
    AMQP_VALUE result;
    umock_c_reset_all_calls();
    setup_subscribe_expected_calls();
    (void)iothubtransportamqp_methods_subscribe(amqp_methods_handle, TEST_SESSION_HANDLE, test_on_methods_error, (void*)0x4242, test_on_method_request_received, (void*)0x4243);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(message_get_properties(TEST_UAMQP_MESSAGE, IGNORED_PTR_ARG))
        .IgnoreArgument_properties()
        .SetReturn(1);
    STRICT_EXPECTED_CALL(messaging_delivery_rejected("amqp:decode-error", IGNORED_PTR_ARG))
        .IgnoreArgument_error_description();

    /// act
    result = g_on_message_received(amqp_methods_handle, TEST_UAMQP_MESSAGE);

    /// assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_ARE_EQUAL(void_ptr, TEST_DELIVERY_REJECTED, result);

    /// cleanup
    iothubtransportamqp_methods_destroy(amqp_methods_handle);
}

/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_047: [ If `properties_get_correlation_id` fails the REJECTED outcome with `amqp:decode-error` shall be returned. ]*/
TEST_FUNCTION(when_properties_get_correlation_id_fails_the_message_is_rejected)
{
    /// arrange
    IOTHUBTRANSPORT_AMQP_METHODS_HANDLE amqp_methods_handle = iothubtransportamqp_methods_create("testhost", "testdevice");
    AMQP_VALUE correlation_id = (AMQP_VALUE)0x5000;
    AMQP_VALUE result;
    umock_c_reset_all_calls();
    setup_subscribe_expected_calls();
    (void)iothubtransportamqp_methods_subscribe(amqp_methods_handle, TEST_SESSION_HANDLE, test_on_methods_error, (void*)0x4242, test_on_method_request_received, (void*)0x4243);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(message_get_properties(TEST_UAMQP_MESSAGE, IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(2, &test_properties_handle, sizeof(test_properties_handle));
    STRICT_EXPECTED_CALL(properties_get_correlation_id(test_properties_handle, IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(2, &correlation_id, sizeof(correlation_id))
        .SetReturn(1);
    STRICT_EXPECTED_CALL(messaging_delivery_rejected("amqp:decode-error", IGNORED_PTR_ARG))
        .IgnoreArgument_error_description();
    STRICT_EXPECTED_CALL(properties_destroy(test_properties_handle));

    /// act
    result = g_on_message_received(amqp_methods_handle, TEST_UAMQP_MESSAGE);

    /// assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_ARE_EQUAL(void_ptr, TEST_DELIVERY_REJECTED, result);

    /// cleanup
    iothubtransportamqp_methods_destroy(amqp_methods_handle);
}

/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_130: [ If allocating memory for the `IOTHUBTRANSPORT_AMQP_METHOD_HANDLE` handle fails, the RELEASED outcome shall be returned and an error shall be indicated. ]*/
TEST_FUNCTION(when_allocating_memory_for_the_method_handle_fails_the_message_is_rejected)
{
    /// arrange
    IOTHUBTRANSPORT_AMQP_METHODS_HANDLE amqp_methods_handle = iothubtransportamqp_methods_create("testhost", "testdevice");
    AMQP_VALUE correlation_id = (AMQP_VALUE)0x5000;
    AMQP_VALUE result;
    umock_c_reset_all_calls();
    setup_subscribe_expected_calls();
    (void)iothubtransportamqp_methods_subscribe(amqp_methods_handle, TEST_SESSION_HANDLE, test_on_methods_error, (void*)0x4242, test_on_method_request_received, (void*)0x4243);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(message_get_properties(TEST_UAMQP_MESSAGE, IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(2, &test_properties_handle, sizeof(test_properties_handle));
    STRICT_EXPECTED_CALL(properties_get_correlation_id(test_properties_handle, IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(2, &correlation_id, sizeof(correlation_id));
    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
        .SetReturn(NULL);
    STRICT_EXPECTED_CALL(properties_destroy(test_properties_handle));
    STRICT_EXPECTED_CALL(messaging_delivery_released());
    STRICT_EXPECTED_CALL(test_on_methods_error((void*)0x4242));

    /// act
    result = g_on_message_received(amqp_methods_handle, TEST_UAMQP_MESSAGE);

    /// assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_ARE_EQUAL(void_ptr, TEST_DELIVERY_RELEASED, result);

    /// cleanup
    iothubtransportamqp_methods_destroy(amqp_methods_handle);
}

/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_138: [ If resizing the tracked method handles array fails, the RELEASED outcome shall be returned and an error shall be indicated. ]*/
TEST_FUNCTION(when_reallocating_memory_for_tracked_handles_fails_the_message_is_rejected)
{
    /// arrange
    IOTHUBTRANSPORT_AMQP_METHODS_HANDLE amqp_methods_handle = iothubtransportamqp_methods_create("testhost", "testdevice");
    AMQP_VALUE correlation_id = (AMQP_VALUE)0x5000;
    AMQP_VALUE result;
    umock_c_reset_all_calls();
    setup_subscribe_expected_calls();
    (void)iothubtransportamqp_methods_subscribe(amqp_methods_handle, TEST_SESSION_HANDLE, test_on_methods_error, (void*)0x4242, test_on_method_request_received, (void*)0x4243);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(message_get_properties(TEST_UAMQP_MESSAGE, IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(2, &test_properties_handle, sizeof(test_properties_handle));
    STRICT_EXPECTED_CALL(properties_get_correlation_id(test_properties_handle, IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(2, &correlation_id, sizeof(correlation_id));
    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(gballoc_realloc(IGNORED_PTR_ARG, IGNORED_NUM_ARG))
        .SetReturn(NULL);
    EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(properties_destroy(test_properties_handle));
    STRICT_EXPECTED_CALL(messaging_delivery_released());
    STRICT_EXPECTED_CALL(test_on_methods_error((void*)0x4242));
    /// act
    result = g_on_message_received(amqp_methods_handle, TEST_UAMQP_MESSAGE);

    /// assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_ARE_EQUAL(void_ptr, TEST_DELIVERY_RELEASED, result);

    /// cleanup
    iothubtransportamqp_methods_destroy(amqp_methods_handle);
}

/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_122: [ If `amqpvalue_get_uuid` fails the REJECTED outcome with `amqp:decode-error` shall be returned. ]*/
TEST_FUNCTION(when_amqpvalue_get_uuid_fails_the_message_is_rejected)
{
    /// arrange
    IOTHUBTRANSPORT_AMQP_METHODS_HANDLE amqp_methods_handle = iothubtransportamqp_methods_create("testhost", "testdevice");
    AMQP_VALUE correlation_id = (AMQP_VALUE)0x5000;
    uuid correlation_id_uuid;
    AMQP_VALUE result;
    umock_c_reset_all_calls();
    setup_subscribe_expected_calls();
    (void)iothubtransportamqp_methods_subscribe(amqp_methods_handle, TEST_SESSION_HANDLE, test_on_methods_error, (void*)0x4242, test_on_method_request_received, (void*)0x4243);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(message_get_properties(TEST_UAMQP_MESSAGE, IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(2, &test_properties_handle, sizeof(test_properties_handle));
    STRICT_EXPECTED_CALL(properties_get_correlation_id(test_properties_handle, IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(2, &correlation_id, sizeof(correlation_id));
    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(gballoc_realloc(IGNORED_PTR_ARG, IGNORED_NUM_ARG));
    STRICT_EXPECTED_CALL(amqpvalue_get_uuid(correlation_id, IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(2, &correlation_id_uuid, sizeof(correlation_id_uuid))
        .SetReturn(42);
    EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(messaging_delivery_rejected("amqp:decode-error", IGNORED_PTR_ARG))
        .IgnoreArgument_error_description();
    STRICT_EXPECTED_CALL(properties_destroy(test_properties_handle));

    /// act
    result = g_on_message_received(amqp_methods_handle, TEST_UAMQP_MESSAGE);

    /// assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_ARE_EQUAL(void_ptr, TEST_DELIVERY_REJECTED, result);

    /// cleanup
    iothubtransportamqp_methods_destroy(amqp_methods_handle);
}

/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_049: [ If `message_get_body_amqp_data` fails the REJECTED outcome with `amqp:decode-error` shall be returned. ]*/
TEST_FUNCTION(when_message_get_body_amqp_data_fails_the_message_is_rejected)
{
    /// arrange
    IOTHUBTRANSPORT_AMQP_METHODS_HANDLE amqp_methods_handle = iothubtransportamqp_methods_create("testhost", "testdevice");
    AMQP_VALUE correlation_id = (AMQP_VALUE)0x5000;
    uuid correlation_id_uuid;
    const unsigned char test_method_request_payload[] = { 42 };
    BINARY_DATA binary_data;
    AMQP_VALUE result;

    binary_data.bytes = test_method_request_payload;
    binary_data.length = sizeof(test_method_request_payload);

    umock_c_reset_all_calls();
    setup_subscribe_expected_calls();
    (void)iothubtransportamqp_methods_subscribe(amqp_methods_handle, TEST_SESSION_HANDLE, test_on_methods_error, (void*)0x4242, test_on_method_request_received, (void*)0x4243);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(message_get_properties(TEST_UAMQP_MESSAGE, IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(2, &test_properties_handle, sizeof(test_properties_handle));
    STRICT_EXPECTED_CALL(properties_get_correlation_id(test_properties_handle, IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(2, &correlation_id, sizeof(correlation_id));
    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(gballoc_realloc(IGNORED_PTR_ARG, IGNORED_NUM_ARG));
    STRICT_EXPECTED_CALL(amqpvalue_get_uuid(correlation_id, IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(2, &correlation_id_uuid, sizeof(correlation_id_uuid));
    STRICT_EXPECTED_CALL(message_get_body_amqp_data(TEST_UAMQP_MESSAGE, 0, IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &binary_data, sizeof(binary_data))
        .SetReturn(42);
    EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(messaging_delivery_rejected("amqp:decode-error", IGNORED_PTR_ARG))
        .IgnoreArgument_error_description();
    STRICT_EXPECTED_CALL(properties_destroy(test_properties_handle));

    /// act
    result = g_on_message_received(amqp_methods_handle, TEST_UAMQP_MESSAGE);

    /// assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_ARE_EQUAL(void_ptr, TEST_DELIVERY_REJECTED, result);

    /// cleanup
    iothubtransportamqp_methods_destroy(amqp_methods_handle);
}

/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_133: [ If `message_get_application_properties` fails the REJECTED outcome with `amqp:decode-error` shall be returned. ]*/
TEST_FUNCTION(when_message_get_application_properties_fails_the_message_is_rejected)
{
    /// arrange
    IOTHUBTRANSPORT_AMQP_METHODS_HANDLE amqp_methods_handle = iothubtransportamqp_methods_create("testhost", "testdevice");
    AMQP_VALUE application_properties = (AMQP_VALUE)0x5001;
    AMQP_VALUE correlation_id = (AMQP_VALUE)0x5000;
    uuid correlation_id_uuid;
    const unsigned char test_method_request_payload[] = { 42 };
    BINARY_DATA binary_data;
    AMQP_VALUE result;

    binary_data.bytes = test_method_request_payload;
    binary_data.length = sizeof(test_method_request_payload);

    umock_c_reset_all_calls();
    setup_subscribe_expected_calls();
    (void)iothubtransportamqp_methods_subscribe(amqp_methods_handle, TEST_SESSION_HANDLE, test_on_methods_error, (void*)0x4242, test_on_method_request_received, (void*)0x4243);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(message_get_properties(TEST_UAMQP_MESSAGE, IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(2, &test_properties_handle, sizeof(test_properties_handle));
    STRICT_EXPECTED_CALL(properties_get_correlation_id(test_properties_handle, IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(2, &correlation_id, sizeof(correlation_id));
    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(gballoc_realloc(IGNORED_PTR_ARG, IGNORED_NUM_ARG));
    STRICT_EXPECTED_CALL(amqpvalue_get_uuid(correlation_id, IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(2, &correlation_id_uuid, sizeof(correlation_id_uuid));
    STRICT_EXPECTED_CALL(message_get_body_amqp_data(TEST_UAMQP_MESSAGE, 0, IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &binary_data, sizeof(binary_data));
    STRICT_EXPECTED_CALL(message_get_application_properties(TEST_UAMQP_MESSAGE, IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(2, &application_properties, sizeof(application_properties))
        .SetReturn(42);
    EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(messaging_delivery_rejected("amqp:decode-error", IGNORED_PTR_ARG))
        .IgnoreArgument_error_description();
    STRICT_EXPECTED_CALL(properties_destroy(test_properties_handle));

    /// act
    result = g_on_message_received(amqp_methods_handle, TEST_UAMQP_MESSAGE);

    /// assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_ARE_EQUAL(void_ptr, TEST_DELIVERY_REJECTED, result);

    /// cleanup
    iothubtransportamqp_methods_destroy(amqp_methods_handle);
}

/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_134: [ If `amqpvalue_get_inplace_described_value` fails the REJECTED outcome with `amqp:decode-error` shall be returned. ]*/
TEST_FUNCTION(when_amqpvalue_get_inplace_described_value_fails_the_message_is_released)
{
    /// arrange
    IOTHUBTRANSPORT_AMQP_METHODS_HANDLE amqp_methods_handle = iothubtransportamqp_methods_create("testhost", "testdevice");
    AMQP_VALUE application_properties = (AMQP_VALUE)0x5001;
    AMQP_VALUE correlation_id = (AMQP_VALUE)0x5000;
    uuid correlation_id_uuid;
    const unsigned char test_method_request_payload[] = { 42 };
    BINARY_DATA binary_data;
    AMQP_VALUE result;

    binary_data.bytes = test_method_request_payload;
    binary_data.length = sizeof(test_method_request_payload);

    umock_c_reset_all_calls();
    setup_subscribe_expected_calls();
    (void)iothubtransportamqp_methods_subscribe(amqp_methods_handle, TEST_SESSION_HANDLE, test_on_methods_error, (void*)0x4242, test_on_method_request_received, (void*)0x4243);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(message_get_properties(TEST_UAMQP_MESSAGE, IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(2, &test_properties_handle, sizeof(test_properties_handle));
    STRICT_EXPECTED_CALL(properties_get_correlation_id(test_properties_handle, IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(2, &correlation_id, sizeof(correlation_id));
    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(gballoc_realloc(IGNORED_PTR_ARG, IGNORED_NUM_ARG));
    STRICT_EXPECTED_CALL(amqpvalue_get_uuid(correlation_id, IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(2, &correlation_id_uuid, sizeof(correlation_id_uuid));
    STRICT_EXPECTED_CALL(message_get_body_amqp_data(TEST_UAMQP_MESSAGE, 0, IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &binary_data, sizeof(binary_data));
    STRICT_EXPECTED_CALL(message_get_application_properties(TEST_UAMQP_MESSAGE, IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(2, &application_properties, sizeof(application_properties));
    STRICT_EXPECTED_CALL(amqpvalue_get_inplace_described_value(application_properties))
        .SetReturn(NULL);
    EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(application_properties_destroy(application_properties));
    STRICT_EXPECTED_CALL(properties_destroy(test_properties_handle));
    STRICT_EXPECTED_CALL(messaging_delivery_released());
    STRICT_EXPECTED_CALL(test_on_methods_error((void*)0x4242));

    /// act
    result = g_on_message_received(amqp_methods_handle, TEST_UAMQP_MESSAGE);

    /// assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_ARE_EQUAL(void_ptr, TEST_DELIVERY_RELEASED, result);

    /// cleanup
    iothubtransportamqp_methods_destroy(amqp_methods_handle);
}

/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_132: [ If `amqpvalue_create_string` fails the RELEASED outcome shall be returned. ]*/
TEST_FUNCTION(when_amqpvalue_create_string_fails_the_message_is_released)
{
    /// arrange
    IOTHUBTRANSPORT_AMQP_METHODS_HANDLE amqp_methods_handle = iothubtransportamqp_methods_create("testhost", "testdevice");
    AMQP_VALUE application_properties = (AMQP_VALUE)0x5001;
    AMQP_VALUE application_properties_map = (AMQP_VALUE)0x5002;
    AMQP_VALUE correlation_id = (AMQP_VALUE)0x5000;
    uuid correlation_id_uuid;
    const unsigned char test_method_request_payload[] = { 42 };
    BINARY_DATA binary_data;
    AMQP_VALUE result;

    binary_data.bytes = test_method_request_payload;
    binary_data.length = sizeof(test_method_request_payload);

    umock_c_reset_all_calls();
    setup_subscribe_expected_calls();
    (void)iothubtransportamqp_methods_subscribe(amqp_methods_handle, TEST_SESSION_HANDLE, test_on_methods_error, (void*)0x4242, test_on_method_request_received, (void*)0x4243);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(message_get_properties(TEST_UAMQP_MESSAGE, IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(2, &test_properties_handle, sizeof(test_properties_handle));
    STRICT_EXPECTED_CALL(properties_get_correlation_id(test_properties_handle, IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(2, &correlation_id, sizeof(correlation_id));
    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(gballoc_realloc(IGNORED_PTR_ARG, IGNORED_NUM_ARG));
    STRICT_EXPECTED_CALL(amqpvalue_get_uuid(correlation_id, IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(2, &correlation_id_uuid, sizeof(correlation_id_uuid));
    STRICT_EXPECTED_CALL(message_get_body_amqp_data(TEST_UAMQP_MESSAGE, 0, IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &binary_data, sizeof(binary_data));
    STRICT_EXPECTED_CALL(message_get_application_properties(TEST_UAMQP_MESSAGE, IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(2, &application_properties, sizeof(application_properties));
    STRICT_EXPECTED_CALL(amqpvalue_get_inplace_described_value(application_properties))
        .SetReturn(application_properties_map);
    STRICT_EXPECTED_CALL(amqpvalue_create_string("IoThub-methodname"))
        .SetReturn(NULL);
    EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(application_properties_destroy(application_properties));
    STRICT_EXPECTED_CALL(properties_destroy(test_properties_handle));
    STRICT_EXPECTED_CALL(messaging_delivery_released());
    STRICT_EXPECTED_CALL(test_on_methods_error((void*)0x4242));

    /// act
    result = g_on_message_received(amqp_methods_handle, TEST_UAMQP_MESSAGE);

    /// assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_ARE_EQUAL(void_ptr, TEST_DELIVERY_RELEASED, result);

    /// cleanup
    iothubtransportamqp_methods_destroy(amqp_methods_handle);
}

/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_103: [ If `amqpvalue_get_map_value` fails the REJECTED outcome with `amqp:decode-error` shall be returned. ]*/
TEST_FUNCTION(when_amqpvalue_get_map_value_fails_the_message_is_rejected)
{
    /// arrange
    IOTHUBTRANSPORT_AMQP_METHODS_HANDLE amqp_methods_handle = iothubtransportamqp_methods_create("testhost", "testdevice");
    AMQP_VALUE application_properties = (AMQP_VALUE)0x5001;
    AMQP_VALUE application_properties_map = (AMQP_VALUE)0x5002;
    AMQP_VALUE test_property_key = (AMQP_VALUE)0x5003;
    AMQP_VALUE correlation_id = (AMQP_VALUE)0x5000;
    uuid correlation_id_uuid;
    const unsigned char test_method_request_payload[] = { 42 };
    BINARY_DATA binary_data;
    AMQP_VALUE result;

    binary_data.bytes = test_method_request_payload;
    binary_data.length = sizeof(test_method_request_payload);

    umock_c_reset_all_calls();
    setup_subscribe_expected_calls();
    (void)iothubtransportamqp_methods_subscribe(amqp_methods_handle, TEST_SESSION_HANDLE, test_on_methods_error, (void*)0x4242, test_on_method_request_received, (void*)0x4243);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(message_get_properties(TEST_UAMQP_MESSAGE, IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(2, &test_properties_handle, sizeof(test_properties_handle));
    STRICT_EXPECTED_CALL(properties_get_correlation_id(test_properties_handle, IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(2, &correlation_id, sizeof(correlation_id));
    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(gballoc_realloc(IGNORED_PTR_ARG, IGNORED_NUM_ARG));
    STRICT_EXPECTED_CALL(amqpvalue_get_uuid(correlation_id, IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(2, &correlation_id_uuid, sizeof(correlation_id_uuid));
    STRICT_EXPECTED_CALL(message_get_body_amqp_data(TEST_UAMQP_MESSAGE, 0, IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &binary_data, sizeof(binary_data));
    STRICT_EXPECTED_CALL(message_get_application_properties(TEST_UAMQP_MESSAGE, IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(2, &application_properties, sizeof(application_properties));
    STRICT_EXPECTED_CALL(amqpvalue_get_inplace_described_value(application_properties))
        .SetReturn(application_properties_map);
    STRICT_EXPECTED_CALL(amqpvalue_create_string("IoThub-methodname"))
        .SetReturn(test_property_key);
    STRICT_EXPECTED_CALL(amqpvalue_get_map_value(application_properties_map, test_property_key))
        .SetReturn(NULL);
    EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(messaging_delivery_rejected("amqp:decode-error", IGNORED_PTR_ARG))
        .IgnoreArgument_error_description();
    STRICT_EXPECTED_CALL(amqpvalue_destroy(test_property_key));
    STRICT_EXPECTED_CALL(application_properties_destroy(application_properties));
    STRICT_EXPECTED_CALL(properties_destroy(test_properties_handle));

    /// act
    result = g_on_message_received(amqp_methods_handle, TEST_UAMQP_MESSAGE);

    /// assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_ARE_EQUAL(void_ptr, TEST_DELIVERY_REJECTED, result);

    /// cleanup
    iothubtransportamqp_methods_destroy(amqp_methods_handle);
}

/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_131: [ If `amqpvalue_get_string` fails the REJECTED outcome with `amqp:decode-error` shall be returned. ]*/
TEST_FUNCTION(when_amqpvalue_get_string_fails_the_message_is_rejected)
{
    /// arrange
    IOTHUBTRANSPORT_AMQP_METHODS_HANDLE amqp_methods_handle = iothubtransportamqp_methods_create("testhost", "testdevice");
    AMQP_VALUE application_properties = (AMQP_VALUE)0x5001;
    AMQP_VALUE application_properties_map = (AMQP_VALUE)0x5002;
    AMQP_VALUE test_property_key = (AMQP_VALUE)0x5003;
    AMQP_VALUE test_property_value = (AMQP_VALUE)0x5004;
    AMQP_VALUE correlation_id = (AMQP_VALUE)0x5000;
    uuid correlation_id_uuid;
    const unsigned char test_method_request_payload[] = { 42 };
    BINARY_DATA binary_data;
    AMQP_VALUE result;

    binary_data.bytes = test_method_request_payload;
    binary_data.length = sizeof(test_method_request_payload);

    umock_c_reset_all_calls();
    setup_subscribe_expected_calls();
    (void)iothubtransportamqp_methods_subscribe(amqp_methods_handle, TEST_SESSION_HANDLE, test_on_methods_error, (void*)0x4242, test_on_method_request_received, (void*)0x4243);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(message_get_properties(TEST_UAMQP_MESSAGE, IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(2, &test_properties_handle, sizeof(test_properties_handle));
    STRICT_EXPECTED_CALL(properties_get_correlation_id(test_properties_handle, IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(2, &correlation_id, sizeof(correlation_id));
    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(gballoc_realloc(IGNORED_PTR_ARG, IGNORED_NUM_ARG));
    STRICT_EXPECTED_CALL(amqpvalue_get_uuid(correlation_id, IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(2, &correlation_id_uuid, sizeof(correlation_id_uuid));
    STRICT_EXPECTED_CALL(message_get_body_amqp_data(TEST_UAMQP_MESSAGE, 0, IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &binary_data, sizeof(binary_data));
    STRICT_EXPECTED_CALL(message_get_application_properties(TEST_UAMQP_MESSAGE, IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(2, &application_properties, sizeof(application_properties));
    STRICT_EXPECTED_CALL(amqpvalue_get_inplace_described_value(application_properties))
        .SetReturn(application_properties_map);
    STRICT_EXPECTED_CALL(amqpvalue_create_string("IoThub-methodname"))
        .SetReturn(test_property_key);
    STRICT_EXPECTED_CALL(amqpvalue_get_map_value(application_properties_map, test_property_key))
        .SetReturn(test_property_value);
    STRICT_EXPECTED_CALL(amqpvalue_get_string(test_property_value, IGNORED_PTR_ARG))
        .IgnoreArgument_string_value()
        .SetReturn(1);
    EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(messaging_delivery_rejected("amqp:decode-error", IGNORED_PTR_ARG))
        .IgnoreArgument_error_description();
    STRICT_EXPECTED_CALL(amqpvalue_destroy(test_property_value));
    STRICT_EXPECTED_CALL(amqpvalue_destroy(test_property_key));
    STRICT_EXPECTED_CALL(application_properties_destroy(application_properties));
    STRICT_EXPECTED_CALL(properties_destroy(test_properties_handle));

    /// act
    result = g_on_message_received(amqp_methods_handle, TEST_UAMQP_MESSAGE);

    /// assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_ARE_EQUAL(void_ptr, TEST_DELIVERY_REJECTED, result);

    /// cleanup
    iothubtransportamqp_methods_destroy(amqp_methods_handle);
}

/* Tests_**SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_057: [ If `messaging_delivery_accepted` fails the RELEASED outcome with `amqp:decode-error` shall be returned. ]*/
TEST_FUNCTION(when_messaging_delivery_accepted_fails_an_RELEASED_outcome_shall_be_created)
{
    /// arrange
    IOTHUBTRANSPORT_AMQP_METHODS_HANDLE amqp_methods_handle = iothubtransportamqp_methods_create("testhost", "testdevice");
    AMQP_VALUE application_properties = (AMQP_VALUE)0x5001;
    AMQP_VALUE application_properties_map = (AMQP_VALUE)0x5002;
    AMQP_VALUE test_property_key = (AMQP_VALUE)0x5003;
    AMQP_VALUE test_property_value = (AMQP_VALUE)0x5004;
    const char* method_name_ptr = TEST_METHOD_NAME;
    AMQP_VALUE correlation_id = (AMQP_VALUE)0x5000;
    uuid correlation_id_uuid;
    const unsigned char test_method_request_payload[] = { 42 };
    BINARY_DATA binary_data;
    AMQP_VALUE result;

    binary_data.bytes = test_method_request_payload;
    binary_data.length = sizeof(test_method_request_payload);

    umock_c_reset_all_calls();
    setup_subscribe_expected_calls();
    (void)iothubtransportamqp_methods_subscribe(amqp_methods_handle, TEST_SESSION_HANDLE, test_on_methods_error, (void*)0x4242, test_on_method_request_received, (void*)0x4243);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(message_get_properties(TEST_UAMQP_MESSAGE, IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(2, &test_properties_handle, sizeof(test_properties_handle));
    STRICT_EXPECTED_CALL(properties_get_correlation_id(test_properties_handle, IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(2, &correlation_id, sizeof(correlation_id));
    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(gballoc_realloc(IGNORED_PTR_ARG, IGNORED_NUM_ARG));
    STRICT_EXPECTED_CALL(amqpvalue_get_uuid(correlation_id, IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(2, &correlation_id_uuid, sizeof(correlation_id_uuid));
    STRICT_EXPECTED_CALL(message_get_body_amqp_data(TEST_UAMQP_MESSAGE, 0, IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &binary_data, sizeof(binary_data));
    STRICT_EXPECTED_CALL(message_get_application_properties(TEST_UAMQP_MESSAGE, IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(2, &application_properties, sizeof(application_properties));
    STRICT_EXPECTED_CALL(amqpvalue_get_inplace_described_value(application_properties))
        .SetReturn(application_properties_map);
    STRICT_EXPECTED_CALL(amqpvalue_create_string("IoThub-methodname"))
        .SetReturn(test_property_key);
    STRICT_EXPECTED_CALL(amqpvalue_get_map_value(application_properties_map, test_property_key))
        .SetReturn(test_property_value);
    STRICT_EXPECTED_CALL(amqpvalue_get_string(test_property_value, IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(2, &method_name_ptr, sizeof(method_name_ptr));
    STRICT_EXPECTED_CALL(messaging_delivery_accepted())
        .SetReturn(NULL);
    EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(amqpvalue_destroy(test_property_value));
    STRICT_EXPECTED_CALL(amqpvalue_destroy(test_property_key));
    STRICT_EXPECTED_CALL(application_properties_destroy(application_properties));
    STRICT_EXPECTED_CALL(properties_destroy(test_properties_handle));
    STRICT_EXPECTED_CALL(messaging_delivery_released());
    STRICT_EXPECTED_CALL(test_on_methods_error((void*)0x4242));

    /// act
    result = g_on_message_received(amqp_methods_handle, TEST_UAMQP_MESSAGE);

    /// assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_ARE_EQUAL(void_ptr, TEST_DELIVERY_RELEASED, result);

    /// cleanup
    iothubtransportamqp_methods_destroy(amqp_methods_handle);
}

/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_128: [ When the RELEASED outcome is returned, an error shall be indicated by calling the `on_methods_error` callback passed to `iothubtransportamqp_methods_subscribe`. ]*/
TEST_FUNCTION(when_creating_the_RELEASED_outcome_fails_an_error_is_triggered)
{
    /// arrange
    IOTHUBTRANSPORT_AMQP_METHODS_HANDLE amqp_methods_handle = iothubtransportamqp_methods_create("testhost", "testdevice");
    AMQP_VALUE application_properties = (AMQP_VALUE)0x5001;
    AMQP_VALUE application_properties_map = (AMQP_VALUE)0x5002;
    AMQP_VALUE test_property_key = (AMQP_VALUE)0x5003;
    AMQP_VALUE test_property_value = (AMQP_VALUE)0x5004;
    const char* method_name_ptr = TEST_METHOD_NAME;
    AMQP_VALUE correlation_id = (AMQP_VALUE)0x5000;
    uuid correlation_id_uuid;
    const unsigned char test_method_request_payload[] = { 42 };
    BINARY_DATA binary_data;
    AMQP_VALUE result;

    binary_data.bytes = test_method_request_payload;
    binary_data.length = sizeof(test_method_request_payload);

    umock_c_reset_all_calls();
    setup_subscribe_expected_calls();
    (void)iothubtransportamqp_methods_subscribe(amqp_methods_handle, TEST_SESSION_HANDLE, test_on_methods_error, (void*)0x4242, test_on_method_request_received, (void*)0x4243);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(message_get_properties(TEST_UAMQP_MESSAGE, IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(2, &test_properties_handle, sizeof(test_properties_handle));
    STRICT_EXPECTED_CALL(properties_get_correlation_id(test_properties_handle, IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(2, &correlation_id, sizeof(correlation_id));
    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(gballoc_realloc(IGNORED_PTR_ARG, IGNORED_NUM_ARG));
    STRICT_EXPECTED_CALL(amqpvalue_get_uuid(correlation_id, IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(2, &correlation_id_uuid, sizeof(correlation_id_uuid));
    STRICT_EXPECTED_CALL(message_get_body_amqp_data(TEST_UAMQP_MESSAGE, 0, IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &binary_data, sizeof(binary_data));
    STRICT_EXPECTED_CALL(message_get_application_properties(TEST_UAMQP_MESSAGE, IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(2, &application_properties, sizeof(application_properties));
    STRICT_EXPECTED_CALL(amqpvalue_get_inplace_described_value(application_properties))
        .SetReturn(application_properties_map);
    STRICT_EXPECTED_CALL(amqpvalue_create_string("IoThub-methodname"))
        .SetReturn(test_property_key);
    STRICT_EXPECTED_CALL(amqpvalue_get_map_value(application_properties_map, test_property_key))
        .SetReturn(test_property_value);
    STRICT_EXPECTED_CALL(amqpvalue_get_string(test_property_value, IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(2, &method_name_ptr, sizeof(method_name_ptr));
    STRICT_EXPECTED_CALL(messaging_delivery_accepted())
        .SetReturn(NULL);
    EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(amqpvalue_destroy(test_property_value));
    STRICT_EXPECTED_CALL(amqpvalue_destroy(test_property_key));
    STRICT_EXPECTED_CALL(application_properties_destroy(application_properties));
    STRICT_EXPECTED_CALL(properties_destroy(test_properties_handle));
    STRICT_EXPECTED_CALL(messaging_delivery_released())
        .SetReturn(NULL);
    STRICT_EXPECTED_CALL(test_on_methods_error((void*)0x4242));

    /// act
    result = g_on_message_received(amqp_methods_handle, TEST_UAMQP_MESSAGE);

    /// assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_IS_NULL(result);

    /// cleanup
    iothubtransportamqp_methods_destroy(amqp_methods_handle);
}

/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_147: [ If `on_method_request_received` fails, the REJECTED outcome shall be returned with `amqp:internal-error`. ]*/
TEST_FUNCTION(when_calling_the_method_request_received_callback_fails_then_RELEASED_shall_be_returned)
{
    /// arrange
    IOTHUBTRANSPORT_AMQP_METHODS_HANDLE amqp_methods_handle = iothubtransportamqp_methods_create("testhost", "testdevice");
    AMQP_VALUE application_properties = (AMQP_VALUE)0x5001;
    AMQP_VALUE application_properties_map = (AMQP_VALUE)0x5002;
    AMQP_VALUE test_property_key = (AMQP_VALUE)0x5003;
    AMQP_VALUE test_property_value = (AMQP_VALUE)0x5004;
    const char* method_name_ptr = TEST_METHOD_NAME;
    AMQP_VALUE correlation_id = (AMQP_VALUE)0x5000;
    uuid correlation_id_uuid;
    const unsigned char test_method_request_payload[] = { 42 };
    BINARY_DATA binary_data;
    AMQP_VALUE result;

    binary_data.bytes = test_method_request_payload;
    binary_data.length = sizeof(test_method_request_payload);

    umock_c_reset_all_calls();
    setup_subscribe_expected_calls();
    (void)iothubtransportamqp_methods_subscribe(amqp_methods_handle, TEST_SESSION_HANDLE, test_on_methods_error, (void*)0x4242, test_on_method_request_received, (void*)0x4243);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(message_get_properties(TEST_UAMQP_MESSAGE, IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(2, &test_properties_handle, sizeof(test_properties_handle));
    STRICT_EXPECTED_CALL(properties_get_correlation_id(test_properties_handle, IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(2, &correlation_id, sizeof(correlation_id));
    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(gballoc_realloc(IGNORED_PTR_ARG, IGNORED_NUM_ARG));
    STRICT_EXPECTED_CALL(amqpvalue_get_uuid(correlation_id, IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(2, &correlation_id_uuid, sizeof(correlation_id_uuid));
    STRICT_EXPECTED_CALL(message_get_body_amqp_data(TEST_UAMQP_MESSAGE, 0, IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &binary_data, sizeof(binary_data));
    STRICT_EXPECTED_CALL(message_get_application_properties(TEST_UAMQP_MESSAGE, IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(2, &application_properties, sizeof(application_properties));
    STRICT_EXPECTED_CALL(amqpvalue_get_inplace_described_value(application_properties))
        .SetReturn(application_properties_map);
    STRICT_EXPECTED_CALL(amqpvalue_create_string("IoThub-methodname"))
        .SetReturn(test_property_key);
    STRICT_EXPECTED_CALL(amqpvalue_get_map_value(application_properties_map, test_property_key))
        .SetReturn(test_property_value);
    STRICT_EXPECTED_CALL(amqpvalue_get_string(test_property_value, IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(2, &method_name_ptr, sizeof(method_name_ptr));
    STRICT_EXPECTED_CALL(messaging_delivery_accepted());
    STRICT_EXPECTED_CALL(test_on_method_request_received((void*)0x4243, TEST_METHOD_NAME, IGNORED_PTR_ARG, sizeof(test_method_request_payload), IGNORED_PTR_ARG))
        .ValidateArgumentBuffer(3, test_method_request_payload, sizeof(test_method_request_payload))
        .IgnoreArgument_method_handle()
        .SetReturn(42);
    STRICT_EXPECTED_CALL(amqpvalue_destroy(TEST_DELIVERY_ACCEPTED));
    EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(messaging_delivery_rejected("amqp:internal-error", IGNORED_PTR_ARG))
        .IgnoreArgument_error_description();
    STRICT_EXPECTED_CALL(amqpvalue_destroy(test_property_value));
    STRICT_EXPECTED_CALL(amqpvalue_destroy(test_property_key));
    STRICT_EXPECTED_CALL(application_properties_destroy(application_properties));
    STRICT_EXPECTED_CALL(properties_destroy(test_properties_handle));

    /// act
    result = g_on_message_received(amqp_methods_handle, TEST_UAMQP_MESSAGE);

    /// assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_ARE_EQUAL(void_ptr, result, TEST_DELIVERY_REJECTED);

    /// cleanup
    iothubtransportamqp_methods_destroy(amqp_methods_handle);
}

/* iothubtransportamqp_methods_respond */

/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_107: [ If the argument `method_handle` is NULL, `iothubtransportamqp_methods_respond` shall fail and return a non-zero value. ]*/
TEST_FUNCTION(iothubtransportamqp_methods_respond_with_NULL_handle_fails)
{
    /// arrange
    int result;
    const unsigned char response_payload[] = { 0x43 };

    /// act
    result = iothubtransportamqp_methods_respond(NULL, response_payload, sizeof(response_payload), 100);

    /// assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
}

/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_108: [ If `response_size` is greater than zero and `response` is NULL, `iothubtransportamqp_methods_respond` shall fail and return a non-zero value. ]*/
TEST_FUNCTION(iothubtransportamqp_methods_respond_with_NULL_response_fails)
{
    /// arrange
    int result;
    IOTHUBTRANSPORT_AMQP_METHODS_HANDLE amqp_methods_handle = iothubtransportamqp_methods_create("testhost", "testdevice");
    const unsigned char response_payload[] = { 0x43 };
    umock_c_reset_all_calls();
    setup_subscribe_expected_calls();
    (void)iothubtransportamqp_methods_subscribe(amqp_methods_handle, TEST_SESSION_HANDLE, test_on_methods_error, (void*)0x4242, test_on_method_request_received, (void*)0x4243);
    umock_c_reset_all_calls();
    setup_message_received_calls();
    g_on_message_received(amqp_methods_handle, TEST_UAMQP_MESSAGE);

    /// act
    result = iothubtransportamqp_methods_respond(g_method_handle, NULL, sizeof(response_payload), 100);

    /// assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_ARE_NOT_EQUAL(int, 0, result);

    ///cleanup
    iothubtransportamqp_methods_destroy(amqp_methods_handle);
}

/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_060: [ `iothubtransportamqp_methods_respond` shall construct a response message and on success it shall return 0. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_061: [ - A new uAMQP message shall be created by calling `message_create`. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_063: [ - A new properties handle shall be created by calling `properties_create`. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_124: [ - An AMQP value holding the correlation id associated with the `method_handle` handle shall be created by calling `amqpvalue_create_uuid`. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_065: [ - The correlation id on the message properties shall be set by calling `properties_set_correlation_id` and passing as argument the already create correlation ID AMQP value. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_148: [ The properties shall be set on the message by calling `message_set_properties`. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_090: [ An AMQP map shall be created to hold the application properties for the response by calling `amqpvalue_create_map`. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_091: [ A property key `IoThub-status` shall be created by calling `amqpvalue_create_int`. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_097: [ A property value of type int shall be created from the `status_code` argument by calling `amqpvalue_create_int`. ] ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_093: [ A new entry shall be added in the application properties map by calling `amqpvalue_set_map_value` and passing the key and value that were previously created. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_094: [ The application properties map shall be set on the response message by calling `message_set_application_properties`. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_078: [ The binary payload for the response shall be set by calling `message_add_body_amqp_data` for the newly created message handle. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_079: [ The field `bytes` of the `binary_data` argument shall be set to the `response` argument value. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_080: [ The field `length` of the `binary_data` argument shall be set to the `response_size` argument value. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_067: [ The message shall be handed over to the message_sender by calling `messagesender_send` and passing as arguments: ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_068: [ - The response message handle. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_069: [ - A send callback and its context for the `on_message_send_complete` callback. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_111: [ The handle `method_handle` shall be freed (have no meaning) after `iothubtransportamqp_methods_respond` has been executed. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_095: [ The application property map and all intermediate values shall be freed after being passed to `message_set_application_properties`. ]*/
TEST_FUNCTION(iothubtransportamqp_methods_respond_sends_the_uAMQP_message)
{
    /// arrange
    int result;
    IOTHUBTRANSPORT_AMQP_METHODS_HANDLE amqp_methods_handle = iothubtransportamqp_methods_create("testhost", "testdevice");
    const unsigned char response_payload[] = { 0x43 };
    uuid correlation_id_uuid;
    AMQP_VALUE response_correlation_id = (AMQP_VALUE)0x6000;
    AMQP_VALUE response_properties_map = (AMQP_VALUE)0x6001;
    AMQP_VALUE status_property_key = (AMQP_VALUE)0x6002;
    AMQP_VALUE status_property_value = (AMQP_VALUE)0x6003;
    PROPERTIES_HANDLE response_properties_handle = (PROPERTIES_HANDLE)0x6004;
    BINARY_DATA response_binary_data;

    response_binary_data.bytes = response_payload;
    response_binary_data.length = sizeof(response_payload);

    umock_c_reset_all_calls();
    setup_subscribe_expected_calls();
    (void)iothubtransportamqp_methods_subscribe(amqp_methods_handle, TEST_SESSION_HANDLE, test_on_methods_error, (void*)0x4242, test_on_method_request_received, (void*)0x4243);
    umock_c_reset_all_calls();
    setup_message_received_calls();
    g_on_message_received(amqp_methods_handle, TEST_UAMQP_MESSAGE);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(message_create())
        .SetReturn(TEST_RESPONSE_UAMQP_MESSAGE);
    STRICT_EXPECTED_CALL(properties_create())
        .SetReturn(response_properties_handle);
    EXPECTED_CALL(amqpvalue_create_uuid(correlation_id_uuid))
        .SetReturn(response_correlation_id);
    STRICT_EXPECTED_CALL(properties_set_correlation_id(response_properties_handle, response_correlation_id));
    STRICT_EXPECTED_CALL(message_set_properties(TEST_RESPONSE_UAMQP_MESSAGE, response_properties_handle));
    STRICT_EXPECTED_CALL(amqpvalue_create_map())
        .SetReturn(response_properties_map);
    STRICT_EXPECTED_CALL(amqpvalue_create_string("IoThub-status"))
        .SetReturn(status_property_key);
    STRICT_EXPECTED_CALL(amqpvalue_create_int(100))
        .SetReturn(status_property_value);
    STRICT_EXPECTED_CALL(amqpvalue_set_map_value(response_properties_map, status_property_key, status_property_value));
    STRICT_EXPECTED_CALL(message_set_application_properties(TEST_RESPONSE_UAMQP_MESSAGE, response_properties_map));
    STRICT_EXPECTED_CALL(message_add_body_amqp_data(TEST_RESPONSE_UAMQP_MESSAGE, response_binary_data));
    STRICT_EXPECTED_CALL(messagesender_send(TEST_MESSAGE_SENDER, TEST_RESPONSE_UAMQP_MESSAGE, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument_on_message_send_complete()
        .IgnoreArgument_callback_context();
    EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));
    EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(amqpvalue_destroy(status_property_value));
    STRICT_EXPECTED_CALL(amqpvalue_destroy(status_property_key));
    STRICT_EXPECTED_CALL(amqpvalue_destroy(response_properties_map));
    STRICT_EXPECTED_CALL(amqpvalue_destroy(response_correlation_id));
    STRICT_EXPECTED_CALL(properties_destroy(response_properties_handle));
    STRICT_EXPECTED_CALL(message_destroy(TEST_RESPONSE_UAMQP_MESSAGE));

    /// act
    result = iothubtransportamqp_methods_respond(g_method_handle, response_payload, sizeof(response_payload), 100);

    /// assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_IS_NOT_NULL(g_on_message_send_complete);

    /// cleanup
    iothubtransportamqp_methods_destroy(amqp_methods_handle);
}

/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_097: [ A property value of type int shall be created from the `status_code` argument by calling `amqpvalue_create_int`. ]*/
TEST_FUNCTION(iothubtransportamqp_methods_respond_encodes_the_status)
{
    /// arrange
    int result;
    IOTHUBTRANSPORT_AMQP_METHODS_HANDLE amqp_methods_handle = iothubtransportamqp_methods_create("testhost", "testdevice");
    const unsigned char response_payload[] = { 0x43 };

    umock_c_reset_all_calls();
    setup_subscribe_expected_calls();
    (void)iothubtransportamqp_methods_subscribe(amqp_methods_handle, TEST_SESSION_HANDLE, test_on_methods_error, (void*)0x4242, test_on_method_request_received, (void*)0x4243);
    umock_c_reset_all_calls();
    setup_message_received_calls();
    g_on_message_received(amqp_methods_handle, TEST_UAMQP_MESSAGE);
    umock_c_reset_all_calls();
    setup_respond_calls(242);

    /// act
    result = iothubtransportamqp_methods_respond(g_method_handle, response_payload, sizeof(response_payload), 242);

    /// assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_IS_NOT_NULL(g_on_message_send_complete);

    /// cleanup
    iothubtransportamqp_methods_destroy(amqp_methods_handle);
}

/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_062: [ If the `message_create` call fails, `iothubtransportamqp_methods_respond` shall fail and return a non-zero value. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_064: [ If the `properties_create call` fails, `iothubtransportamqp_methods_respond` shall fail and return a non-zero value. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_125: [ If `amqpvalue_create_uuid` fails, `iothubtransportamqp_methods_respond` shall fail and return a non-zero value. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_066: [ If the `properties_set_correlation_id` call fails, `iothubtransportamqp_methods_respond` shall fail and return a non-zero value. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_149: [ If `message_set_properties` fails, `iothubtransportamqp_methods_respond` shall fail and return a non-zero value. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_096: [ If any of the calls `amqpvalue_create_string`, `amqpvalue_create_int`, `amqpvalue_create_map`, `amqpvalue_set_map_value` or `message_set_application_properties` fails `iothubtransportamqp_methods_respond` shall fail and return a non-zero value. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_081: [ If the `message_add_body_amqp_data` call fails, `iothubtransportamqp_methods_respond` shall fail and return a non-zero value. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_071: [ If the `messagesender_send` call fails, `iothubtransportamqp_methods_respond` shall fail and return a non-zero value. ]*/
TEST_FUNCTION(when_a_failure_occurs_iothubtransportamqp_methods_respond_fails)
{
    ///arrange
    int negativeTestsInitResult = umock_c_negative_tests_init();
    int result;
    const unsigned char response_payload[] = { 0x43 };
    uuid correlation_id_uuid;
    AMQP_VALUE response_correlation_id = (AMQP_VALUE)0x6000;
    AMQP_VALUE response_properties_map = (AMQP_VALUE)0x6001;
    AMQP_VALUE status_property_key = (AMQP_VALUE)0x6002;
    AMQP_VALUE status_property_value = (AMQP_VALUE)0x6003;
    PROPERTIES_HANDLE response_properties_handle = (PROPERTIES_HANDLE)0x6004;
    BINARY_DATA response_binary_data;
    IOTHUBTRANSPORT_AMQP_METHODS_HANDLE amqp_methods_handle;
    ASSERT_ARE_EQUAL(int, 0, negativeTestsInitResult);
    amqp_methods_handle = iothubtransportamqp_methods_create("testhost", "testdevice");

    response_binary_data.bytes = response_payload;
    response_binary_data.length = sizeof(response_payload);

    umock_c_reset_all_calls();
    setup_subscribe_expected_calls();
    (void)iothubtransportamqp_methods_subscribe(amqp_methods_handle, TEST_SESSION_HANDLE, test_on_methods_error, (void*)0x4242, test_on_method_request_received, (void*)0x4243);
    umock_c_reset_all_calls();
    setup_message_received_calls();
    g_on_message_received(amqp_methods_handle, TEST_UAMQP_MESSAGE);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(message_create())
        .SetReturn(TEST_RESPONSE_UAMQP_MESSAGE).SetFailReturn(NULL);
    STRICT_EXPECTED_CALL(properties_create())
        .SetReturn(response_properties_handle).SetFailReturn(NULL);
    EXPECTED_CALL(amqpvalue_create_uuid(correlation_id_uuid))
        .SetReturn(response_correlation_id).SetFailReturn(NULL);
    STRICT_EXPECTED_CALL(properties_set_correlation_id(response_properties_handle, response_correlation_id))
        .SetFailReturn(1);
    STRICT_EXPECTED_CALL(message_set_properties(TEST_RESPONSE_UAMQP_MESSAGE, response_properties_handle))
        .SetFailReturn(1);
    STRICT_EXPECTED_CALL(amqpvalue_create_map())
        .SetReturn(response_properties_map).SetFailReturn(NULL);
    STRICT_EXPECTED_CALL(amqpvalue_create_string("IoThub-status"))
        .SetReturn(status_property_key).SetFailReturn(NULL);
    STRICT_EXPECTED_CALL(amqpvalue_create_int(100))
        .SetReturn(status_property_value).SetFailReturn(NULL);
    STRICT_EXPECTED_CALL(amqpvalue_set_map_value(response_properties_map, status_property_key, status_property_value))
        .SetFailReturn(1);
    STRICT_EXPECTED_CALL(message_set_application_properties(TEST_RESPONSE_UAMQP_MESSAGE, response_properties_map))
        .SetFailReturn(1);
    STRICT_EXPECTED_CALL(message_add_body_amqp_data(TEST_RESPONSE_UAMQP_MESSAGE, response_binary_data))
        .SetFailReturn(1);
    STRICT_EXPECTED_CALL(messagesender_send(TEST_MESSAGE_SENDER, TEST_RESPONSE_UAMQP_MESSAGE, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument_on_message_send_complete()
        .IgnoreArgument_callback_context()
        .SetFailReturn(1);
    EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));

    umock_c_negative_tests_snapshot();

    for (size_t i = 0; i < umock_c_negative_tests_call_count() - 1; i++)
    {
        umock_c_negative_tests_reset();
        umock_c_negative_tests_fail_call(i);

        char temp_str[128];
        (void)sprintf(temp_str, "On failed call %zu", i);

        ///act
        result = iothubtransportamqp_methods_respond(g_method_handle, response_payload, sizeof(response_payload), 100);

        /// assert
        ASSERT_ARE_NOT_EQUAL_WITH_MSG(int, 0, result, temp_str);
    }

    ///cleanup
    iothubtransportamqp_methods_destroy(amqp_methods_handle);
    umock_c_negative_tests_deinit();
}

/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_109: [ `iothubtransportamqp_methods_respond` shall be allowed to be called from the callback `on_method_request_received`. ]*/
TEST_FUNCTION(iothubtransportamqp_methods_respond_can_be_called_from_the_method_request_callback)
{
    /// arrange
    IOTHUBTRANSPORT_AMQP_METHODS_HANDLE amqp_methods_handle = iothubtransportamqp_methods_create("testhost", "testdevice");
    const unsigned char response_payload[] = { 0x43 };
    uuid correlation_id_uuid;
    AMQP_VALUE response_correlation_id = (AMQP_VALUE)0x6000;
    AMQP_VALUE response_properties_map = (AMQP_VALUE)0x6001;
    AMQP_VALUE status_property_key = (AMQP_VALUE)0x6002;
    AMQP_VALUE status_property_value = (AMQP_VALUE)0x6003;
    PROPERTIES_HANDLE response_properties_handle = (PROPERTIES_HANDLE)0x6004;
    BINARY_DATA response_binary_data;
    AMQP_VALUE correlation_id = (AMQP_VALUE)0x5000;
    AMQP_VALUE application_properties = (AMQP_VALUE)0x5001;
    AMQP_VALUE application_properties_map = (AMQP_VALUE)0x5002;
    AMQP_VALUE test_property_key = (AMQP_VALUE)0x5003;
    AMQP_VALUE test_property_value = (AMQP_VALUE)0x5004;
    const unsigned char test_method_request_payload[] = { 42 };
    BINARY_DATA binary_data;
    const char* method_name_ptr = TEST_METHOD_NAME;

    binary_data.bytes = test_method_request_payload;
    binary_data.length = sizeof(test_method_request_payload);

    response_binary_data.bytes = response_payload;
    response_binary_data.length = sizeof(response_payload);

    umock_c_reset_all_calls();
    setup_subscribe_expected_calls();
    (void)iothubtransportamqp_methods_subscribe(amqp_methods_handle, TEST_SESSION_HANDLE, test_on_methods_error, (void*)0x4242, test_on_method_request_received_calling_respond, (void*)0x4243);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(message_get_properties(TEST_UAMQP_MESSAGE, IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(2, &test_properties_handle, sizeof(test_properties_handle));
    STRICT_EXPECTED_CALL(properties_get_correlation_id(test_properties_handle, IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(2, &correlation_id, sizeof(correlation_id));
    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(gballoc_realloc(IGNORED_PTR_ARG, IGNORED_NUM_ARG));
    STRICT_EXPECTED_CALL(amqpvalue_get_uuid(correlation_id, IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(2, &correlation_id_uuid, sizeof(correlation_id_uuid));
    STRICT_EXPECTED_CALL(message_get_body_amqp_data(TEST_UAMQP_MESSAGE, 0, IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &binary_data, sizeof(binary_data));
    STRICT_EXPECTED_CALL(message_get_application_properties(TEST_UAMQP_MESSAGE, IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(2, &application_properties, sizeof(application_properties));
    STRICT_EXPECTED_CALL(amqpvalue_get_inplace_described_value(application_properties))
        .SetReturn(application_properties_map);
    STRICT_EXPECTED_CALL(amqpvalue_create_string("IoThub-methodname"))
        .SetReturn(test_property_key);
    STRICT_EXPECTED_CALL(amqpvalue_get_map_value(application_properties_map, test_property_key))
        .SetReturn(test_property_value);
    STRICT_EXPECTED_CALL(amqpvalue_get_string(test_property_value, IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(2, &method_name_ptr, sizeof(method_name_ptr));
    STRICT_EXPECTED_CALL(messaging_delivery_accepted());
    STRICT_EXPECTED_CALL(test_on_method_request_received_calling_respond((void*)0x4243, TEST_METHOD_NAME, IGNORED_PTR_ARG, sizeof(test_method_request_payload), IGNORED_PTR_ARG))
        .ValidateArgumentBuffer(3, test_method_request_payload, sizeof(test_method_request_payload))
        .IgnoreArgument(5);

    /* here are the calls due to respond being called in the callback */
    STRICT_EXPECTED_CALL(message_create())
        .SetReturn(TEST_RESPONSE_UAMQP_MESSAGE);
    STRICT_EXPECTED_CALL(properties_create())
        .SetReturn(response_properties_handle);
    EXPECTED_CALL(amqpvalue_create_uuid(correlation_id_uuid))
        .SetReturn(response_correlation_id);
    STRICT_EXPECTED_CALL(properties_set_correlation_id(response_properties_handle, response_correlation_id));
    STRICT_EXPECTED_CALL(message_set_properties(TEST_RESPONSE_UAMQP_MESSAGE, response_properties_handle));
    STRICT_EXPECTED_CALL(amqpvalue_create_map())
        .SetReturn(response_properties_map);
    STRICT_EXPECTED_CALL(amqpvalue_create_string("IoThub-status"))
        .SetReturn(status_property_key);
    STRICT_EXPECTED_CALL(amqpvalue_create_int(242))
        .SetReturn(status_property_value);
    STRICT_EXPECTED_CALL(amqpvalue_set_map_value(response_properties_map, status_property_key, status_property_value));
    STRICT_EXPECTED_CALL(message_set_application_properties(TEST_RESPONSE_UAMQP_MESSAGE, response_properties_map));
    STRICT_EXPECTED_CALL(message_add_body_amqp_data(TEST_RESPONSE_UAMQP_MESSAGE, response_binary_data));
    STRICT_EXPECTED_CALL(messagesender_send(TEST_MESSAGE_SENDER, TEST_RESPONSE_UAMQP_MESSAGE, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument_on_message_send_complete()
        .IgnoreArgument_callback_context();
    EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));
    EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(amqpvalue_destroy(status_property_value));
    STRICT_EXPECTED_CALL(amqpvalue_destroy(status_property_key));
    STRICT_EXPECTED_CALL(amqpvalue_destroy(response_properties_map));
    STRICT_EXPECTED_CALL(amqpvalue_destroy(response_correlation_id));
    STRICT_EXPECTED_CALL(properties_destroy(response_properties_handle));
    STRICT_EXPECTED_CALL(message_destroy(TEST_RESPONSE_UAMQP_MESSAGE));

    STRICT_EXPECTED_CALL(amqpvalue_destroy(test_property_value));
    STRICT_EXPECTED_CALL(amqpvalue_destroy(test_property_key));
    STRICT_EXPECTED_CALL(application_properties_destroy(application_properties));
    STRICT_EXPECTED_CALL(properties_destroy(test_properties_handle));


    /// act
    g_on_message_received(amqp_methods_handle, TEST_UAMQP_MESSAGE);

    /// assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_ARE_EQUAL(int, 0, g_respond_result);

    /// cleanup
    iothubtransportamqp_methods_destroy(amqp_methods_handle);
}

/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_109: [ `iothubtransportamqp_methods_respond` shall be allowed to be called from the callback `on_method_request_received`. ]*/
TEST_FUNCTION(iothubtransportamqp_methods_respond_to_the_second_method_succeeds)
{
    /// arrange
    int result;
    IOTHUBTRANSPORT_AMQP_METHODS_HANDLE amqp_methods_handle = iothubtransportamqp_methods_create("testhost", "testdevice");
    const unsigned char response_payload[] = { 0x43 };
    static uuid correlation_id_uuid;
    AMQP_VALUE response_correlation_id = (AMQP_VALUE)0x6000;
    AMQP_VALUE response_properties_map = (AMQP_VALUE)0x6001;
    AMQP_VALUE status_property_key = (AMQP_VALUE)0x6002;
    AMQP_VALUE status_property_value = (AMQP_VALUE)0x6003;
    PROPERTIES_HANDLE response_properties_handle = (PROPERTIES_HANDLE)0x6004;
    BINARY_DATA response_binary_data;
    IOTHUBTRANSPORT_AMQP_METHOD_HANDLE g_first_method_handle;
    IOTHUBTRANSPORT_AMQP_METHOD_HANDLE g_second_method_handle;

    response_binary_data.bytes = response_payload;
    response_binary_data.length = sizeof(response_payload);

    umock_c_reset_all_calls();
    setup_subscribe_expected_calls();
    (void)iothubtransportamqp_methods_subscribe(amqp_methods_handle, TEST_SESSION_HANDLE, test_on_methods_error, (void*)0x4242, test_on_method_request_received, (void*)0x4243);
    umock_c_reset_all_calls();
    /* setup first request */
    setup_message_received_calls();
    g_on_message_received(amqp_methods_handle, TEST_UAMQP_MESSAGE);
    g_first_method_handle = g_method_handle;
    /* setup second request */
    setup_message_received_calls();
    g_on_message_received(amqp_methods_handle, TEST_UAMQP_MESSAGE);
    g_second_method_handle = g_method_handle;
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(message_create())
        .SetReturn(TEST_RESPONSE_UAMQP_MESSAGE);
    STRICT_EXPECTED_CALL(properties_create())
        .SetReturn(response_properties_handle);
    EXPECTED_CALL(amqpvalue_create_uuid(correlation_id_uuid))
        .SetReturn(response_correlation_id);
    STRICT_EXPECTED_CALL(properties_set_correlation_id(response_properties_handle, response_correlation_id));
    STRICT_EXPECTED_CALL(message_set_properties(TEST_RESPONSE_UAMQP_MESSAGE, response_properties_handle));
    STRICT_EXPECTED_CALL(amqpvalue_create_map())
        .SetReturn(response_properties_map);
    STRICT_EXPECTED_CALL(amqpvalue_create_string("IoThub-status"))
        .SetReturn(status_property_key);
    STRICT_EXPECTED_CALL(amqpvalue_create_int(242))
        .SetReturn(status_property_value);
    STRICT_EXPECTED_CALL(amqpvalue_set_map_value(response_properties_map, status_property_key, status_property_value));
    STRICT_EXPECTED_CALL(message_set_application_properties(TEST_RESPONSE_UAMQP_MESSAGE, response_properties_map));
    STRICT_EXPECTED_CALL(message_add_body_amqp_data(TEST_RESPONSE_UAMQP_MESSAGE, response_binary_data));
    STRICT_EXPECTED_CALL(messagesender_send(TEST_MESSAGE_SENDER, TEST_RESPONSE_UAMQP_MESSAGE, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument_on_message_send_complete()
        .IgnoreArgument_callback_context();
    EXPECTED_CALL(gballoc_realloc(IGNORED_PTR_ARG, IGNORED_NUM_ARG));
    EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(amqpvalue_destroy(status_property_value));
    STRICT_EXPECTED_CALL(amqpvalue_destroy(status_property_key));
    STRICT_EXPECTED_CALL(amqpvalue_destroy(response_properties_map));
    STRICT_EXPECTED_CALL(amqpvalue_destroy(response_correlation_id));
    STRICT_EXPECTED_CALL(properties_destroy(response_properties_handle));
    STRICT_EXPECTED_CALL(message_destroy(TEST_RESPONSE_UAMQP_MESSAGE));

    /// act
    result = iothubtransportamqp_methods_respond(g_second_method_handle, response_payload, sizeof(response_payload), 242);

    /// assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_ARE_EQUAL(int, 0, result);
    ASSERT_IS_NOT_NULL(g_on_message_send_complete);

    /// cleanup
    iothubtransportamqp_methods_destroy(amqp_methods_handle);
}

/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_114: [ The handle `method_handle` shall be removed from the array used to track the method handles. ]*/
TEST_FUNCTION(iothubtransportamqp_methods_respond_removes_the_handle_from_the_tracked_handles)
{
    /// arrange
    IOTHUBTRANSPORT_AMQP_METHODS_HANDLE amqp_methods_handle = iothubtransportamqp_methods_create("testhost", "testdevice");
    const unsigned char response_payload[] = { 0x43 };
    IOTHUBTRANSPORT_AMQP_METHOD_HANDLE g_first_method_handle;
    IOTHUBTRANSPORT_AMQP_METHOD_HANDLE g_second_method_handle;

    umock_c_reset_all_calls();
    setup_subscribe_expected_calls();
    (void)iothubtransportamqp_methods_subscribe(amqp_methods_handle, TEST_SESSION_HANDLE, test_on_methods_error, (void*)0x4242, test_on_method_request_received, (void*)0x4243);
    umock_c_reset_all_calls();
    /* setup first request */
    setup_message_received_calls();
    g_on_message_received(amqp_methods_handle, TEST_UAMQP_MESSAGE);
    g_first_method_handle = g_method_handle;
    /* setup second request */
    setup_message_received_calls();
    g_on_message_received(amqp_methods_handle, TEST_UAMQP_MESSAGE);
    g_second_method_handle = g_method_handle;
    umock_c_reset_all_calls();
    setup_respond_calls(242);
    (void)iothubtransportamqp_methods_respond(g_second_method_handle, response_payload, sizeof(response_payload), 242);
    iothubtransportamqp_methods_unsubscribe(amqp_methods_handle);
    umock_c_reset_all_calls();

    /* 1 extra free for the handle and one extra for the handle array */
    EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));
    EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));

    EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));
    EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));
	EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));

    /// act
    iothubtransportamqp_methods_destroy(amqp_methods_handle);

    /// assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
}

/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_114: [ The handle `method_handle` shall be removed from the array used to track the method handles. ]*/
TEST_FUNCTION(iothubtransportamqp_methods_respond_after_a_handle_has_been_removed_works)
{
    /// arrange
    IOTHUBTRANSPORT_AMQP_METHODS_HANDLE amqp_methods_handle = iothubtransportamqp_methods_create("testhost", "testdevice");
    const unsigned char response_payload[] = { 0x43 };

    umock_c_reset_all_calls();
    setup_subscribe_expected_calls();
    (void)iothubtransportamqp_methods_subscribe(amqp_methods_handle, TEST_SESSION_HANDLE, test_on_methods_error, (void*)0x4242, test_on_method_request_received, (void*)0x4243);
    umock_c_reset_all_calls();
    /* setup first request */
    setup_message_received_calls();
    g_on_message_received(amqp_methods_handle, TEST_UAMQP_MESSAGE);
    umock_c_reset_all_calls();
    setup_respond_calls(242);
    (void)iothubtransportamqp_methods_respond(g_method_handle, response_payload, sizeof(response_payload), 242);
    umock_c_reset_all_calls();

    /* setup second request */
    setup_message_received_calls();

    /// act
    g_on_message_received(amqp_methods_handle, TEST_UAMQP_MESSAGE);

    /// assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    ///cleanup
    iothubtransportamqp_methods_destroy(amqp_methods_handle);
}

/* on_message_receiver_state_changed */

/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_119: [ When `on_message_receiver_state_changed` if called with the `new_state` being `MESSAGE_RECEIVER_STATE_ERROR`, an error shall be indicated by calling the `on_methods_error` callback passed to `iothubtransportamqp_methods_subscribe`. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_120: [ When an error is indicated by calling the `on_methods_error`, it shall be called with the context being the `on_methods_error_context` argument passed to `iothubtransportamqp_methods_subscribe`. ]*/
TEST_FUNCTION(when_on_message_receiver_state_changed_is_called_with_error_the_on_error_callback_shall_be_triggered)
{
    /// arrange
    IOTHUBTRANSPORT_AMQP_METHODS_HANDLE amqp_methods_handle = iothubtransportamqp_methods_create("testhost", "testdevice");

    umock_c_reset_all_calls();
    setup_subscribe_expected_calls();
    (void)iothubtransportamqp_methods_subscribe(amqp_methods_handle, TEST_SESSION_HANDLE, test_on_methods_error, (void*)0x4242, test_on_method_request_received, (void*)0x4243);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(test_on_methods_error((void*)0x4242));

    /// act
    g_on_message_receiver_state_changed(g_on_message_receiver_state_changed_context, MESSAGE_RECEIVER_STATE_ERROR, MESSAGE_RECEIVER_STATE_OPEN);

    /// assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    /// cleanup
    iothubtransportamqp_methods_destroy(amqp_methods_handle);
}

/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_119: [ When `on_message_receiver_state_changed` if called with the `new_state` being `MESSAGE_RECEIVER_STATE_ERROR`, an error shall be indicated by calling the `on_methods_error` callback passed to `iothubtransportamqp_methods_subscribe`. ]*/
TEST_FUNCTION(when_on_message_receiver_state_changed_is_called_with_error_as_new_and_previous_state_no_error_callback_is_triggered)
{
    /// arrange
    IOTHUBTRANSPORT_AMQP_METHODS_HANDLE amqp_methods_handle = iothubtransportamqp_methods_create("testhost", "testdevice");

    umock_c_reset_all_calls();
    setup_subscribe_expected_calls();
    (void)iothubtransportamqp_methods_subscribe(amqp_methods_handle, TEST_SESSION_HANDLE, test_on_methods_error, (void*)0x4242, test_on_method_request_received, (void*)0x4243);
    umock_c_reset_all_calls();

    /// act
    g_on_message_receiver_state_changed(g_on_message_receiver_state_changed_context, MESSAGE_RECEIVER_STATE_ERROR, MESSAGE_RECEIVER_STATE_ERROR);

    /// assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    /// cleanup
    iothubtransportamqp_methods_destroy(amqp_methods_handle);
}

/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_126: [ For the other state changes, on_message_receiver_state_changed shall do nothing. ]*/
TEST_FUNCTION(when_state_changes_from_OPENING_to_OPEN_for_the_message_receiver_no_error_callback_is_triggered)
{
    /// arrange
    IOTHUBTRANSPORT_AMQP_METHODS_HANDLE amqp_methods_handle = iothubtransportamqp_methods_create("testhost", "testdevice");

    umock_c_reset_all_calls();
    setup_subscribe_expected_calls();
    (void)iothubtransportamqp_methods_subscribe(amqp_methods_handle, TEST_SESSION_HANDLE, test_on_methods_error, (void*)0x4242, test_on_method_request_received, (void*)0x4243);
    umock_c_reset_all_calls();

    /// act
    g_on_message_receiver_state_changed(g_on_message_receiver_state_changed_context, MESSAGE_RECEIVER_STATE_OPENING, MESSAGE_RECEIVER_STATE_OPEN);

    /// assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    /// cleanup
    iothubtransportamqp_methods_destroy(amqp_methods_handle);
}

/* on_message_receiver_state_changed */

/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_059: [ When `on_message_sender_state_changed` is called with the `new_state` being `MESSAGE_SENDER_STATE_ERROR`, an error shall be indicated by calling the `on_methods_error` callback passed to `iothubtransportamqp_methods_subscribe`. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_058: [ When an error is indicated by calling the `on_methods_error`, it shall be called with the context being the `on_methods_error_context` argument passed to `iothubtransportamqp_methods_subscribe`. ]*/
TEST_FUNCTION(when_on_message_sender_state_changed_is_called_with_error_the_on_error_callback_shall_be_triggered)
{
    /// arrange
    IOTHUBTRANSPORT_AMQP_METHODS_HANDLE amqp_methods_handle = iothubtransportamqp_methods_create("testhost", "testdevice");

    umock_c_reset_all_calls();
    setup_subscribe_expected_calls();
    (void)iothubtransportamqp_methods_subscribe(amqp_methods_handle, TEST_SESSION_HANDLE, test_on_methods_error, (void*)0x4242, test_on_method_request_received, (void*)0x4243);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(test_on_methods_error((void*)0x4242));

    /// act
    g_on_message_sender_state_changed(g_on_message_receiver_state_changed_context, MESSAGE_SENDER_STATE_ERROR, MESSAGE_SENDER_STATE_OPEN);

    /// assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    /// cleanup
    iothubtransportamqp_methods_destroy(amqp_methods_handle);
}

/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_059: [ When `on_message_sender_state_changed` is called with the `new_state` being `MESSAGE_SENDER_STATE_ERROR`, an error shall be indicated by calling the `on_methods_error` callback passed to `iothubtransportamqp_methods_subscribe`. ]*/
TEST_FUNCTION(when_on_message_sender_state_changed_is_called_with_error_as_new_and_previous_state_no_error_callback_is_triggered)
{
    /// arrange
    IOTHUBTRANSPORT_AMQP_METHODS_HANDLE amqp_methods_handle = iothubtransportamqp_methods_create("testhost", "testdevice");

    umock_c_reset_all_calls();
    setup_subscribe_expected_calls();
    (void)iothubtransportamqp_methods_subscribe(amqp_methods_handle, TEST_SESSION_HANDLE, test_on_methods_error, (void*)0x4242, test_on_method_request_received, (void*)0x4243);
    umock_c_reset_all_calls();

    /// act
    g_on_message_sender_state_changed(g_on_message_receiver_state_changed_context, MESSAGE_SENDER_STATE_ERROR, MESSAGE_SENDER_STATE_ERROR);

    /// assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    /// cleanup
    iothubtransportamqp_methods_destroy(amqp_methods_handle);
}

/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_127: [ For the other state changes, on_message_sender_state_changed shall do nothing. ]*/
TEST_FUNCTION(when_state_changes_from_OPENING_to_OPEN_for_the_message_sender_no_error_callback_is_triggered)
{
    /// arrange
    IOTHUBTRANSPORT_AMQP_METHODS_HANDLE amqp_methods_handle = iothubtransportamqp_methods_create("testhost", "testdevice");

    umock_c_reset_all_calls();
    setup_subscribe_expected_calls();
    (void)iothubtransportamqp_methods_subscribe(amqp_methods_handle, TEST_SESSION_HANDLE, test_on_methods_error, (void*)0x4242, test_on_method_request_received, (void*)0x4243);
    umock_c_reset_all_calls();

    /// act
    g_on_message_sender_state_changed(g_on_message_receiver_state_changed_context, MESSAGE_SENDER_STATE_OPENING, MESSAGE_SENDER_STATE_OPEN);

    /// assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    /// cleanup
    iothubtransportamqp_methods_destroy(amqp_methods_handle);
}

/* on_message_send_complete */

/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_083: [ If `send_result` is `MESSAGE_SEND_ERROR` then an error shall be indicated by calling the `on_methods_error` callback passed to `iothubtransportamqp_methods_subscribe`. ]*/
TEST_FUNCTION(when_an_error_is_indicated_in_the_send_complete_callback_an_error_is_indicated_through_on_methods_error)
{
    /// arrange
    IOTHUBTRANSPORT_AMQP_METHODS_HANDLE amqp_methods_handle = iothubtransportamqp_methods_create("testhost", "testdevice");
    const unsigned char response_payload[] = { 0x43 };

    umock_c_reset_all_calls();
    setup_subscribe_expected_calls();
    (void)iothubtransportamqp_methods_subscribe(amqp_methods_handle, TEST_SESSION_HANDLE, test_on_methods_error, (void*)0x4242, test_on_method_request_received, (void*)0x4243);
    umock_c_reset_all_calls();
    setup_message_received_calls();
    g_on_message_received(amqp_methods_handle, TEST_UAMQP_MESSAGE);
    umock_c_reset_all_calls();
    setup_method_respond_calls();
    (void)iothubtransportamqp_methods_respond(g_method_handle, response_payload, sizeof(response_payload), 100);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(test_on_methods_error((void*)0x4242));

    /// act
    g_on_message_send_complete(g_on_message_send_complete_context, MESSAGE_SEND_ERROR);

    /// assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    /// cleanup
    iothubtransportamqp_methods_destroy(amqp_methods_handle);
}

/* Tests_SRS_IOTHUBTRANSPORT_AMQP_METHODS_01_084: [ Otherwise no action shall be taken. ]*/
TEST_FUNCTION(when_no_error_is_indicated_in_the_send_complete_callback_no_error_is_buubled_up_to_the_user)
{
    /// arrange
    IOTHUBTRANSPORT_AMQP_METHODS_HANDLE amqp_methods_handle = iothubtransportamqp_methods_create("testhost", "testdevice");
    const unsigned char response_payload[] = { 0x43 };

    umock_c_reset_all_calls();
    setup_subscribe_expected_calls();
    (void)iothubtransportamqp_methods_subscribe(amqp_methods_handle, TEST_SESSION_HANDLE, test_on_methods_error, (void*)0x4242, test_on_method_request_received, (void*)0x4243);
    umock_c_reset_all_calls();
    setup_message_received_calls();
    g_on_message_received(amqp_methods_handle, TEST_UAMQP_MESSAGE);
    umock_c_reset_all_calls();
    setup_method_respond_calls();
    (void)iothubtransportamqp_methods_respond(g_method_handle, response_payload, sizeof(response_payload), 100);
    umock_c_reset_all_calls();

    /// act
    g_on_message_send_complete(g_on_message_send_complete_context, MESSAGE_SEND_OK);

    /// assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    /// cleanup
    iothubtransportamqp_methods_destroy(amqp_methods_handle);
}

END_TEST_SUITE(iothubtransportamqp_methods_unittests)
