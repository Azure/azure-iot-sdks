// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>

void* real_malloc(size_t size)
{
	return malloc(size);
}

void real_free(void* ptr)
{
	free(ptr);
}

#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "testrunnerswitcher.h"
#include "azure_c_shared_utility/macro_utils.h"
#include "umock_c.h"
#include "umocktypes_charptr.h"
#include "umocktypes_stdint.h"
#include "umocktypes_bool.h"
#include "umock_c_negative_tests.h"
#include "umocktypes.h"
#include "umocktypes_c.h"
#include "azure_c_shared_utility/crt_abstractions.h"

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

    /* VECTOR redirects */

    void* my_gballoc_realloc(void* ptr, size_t size)
    {
        return realloc(ptr, size);
    }

#ifdef __cplusplus
}
#endif


#define ENABLE_MOCKS
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/doublylinkedlist.h"
#include "azure_c_shared_utility/platform.h"
#include "azure_c_shared_utility/strings.h"
#include "azure_c_shared_utility/urlencode.h"
#include "azure_c_shared_utility/tlsio.h"
#include "azure_c_shared_utility/vector.h"
#include "azure_c_shared_utility/agenttime.h"

#include "azure_uamqp_c/cbs.h"
#include "azure_uamqp_c/link.h"
#include "azure_uamqp_c/message.h"
#include "azure_uamqp_c/amqpvalue.h"
#include "azure_uamqp_c/message_receiver.h"
#include "azure_uamqp_c/message_sender.h"
#include "azure_uamqp_c/messaging.h"
#include "azure_uamqp_c/sasl_mssbcbs.h"
#include "azure_uamqp_c/saslclientio.h"

#include "uamqp_messaging.h"
#include "iothub_client_ll.h"
#include "iothub_client_options.h"
#include "iothub_client_private.h"
#include "iothubtransportamqp_auth.h"
#include "iothubtransportamqp_methods.h"
#include "iothub_client_version.h"
#undef ENABLE_MOCKS

#include "iothubtransport_amqp_common.h"

#ifdef __cplusplus
extern "C"
{
#endif

    extern VECTOR_HANDLE real_VECTOR_create(size_t elementSize);
    extern void real_VECTOR_destroy(VECTOR_HANDLE handle);
    extern void* real_VECTOR_find_if(const VECTOR_HANDLE handle, PREDICATE_FUNCTION pred, const void* value);
    extern int real_VECTOR_push_back(VECTOR_HANDLE handle, const void* elements, size_t numElements);
    extern size_t real_VECTOR_size(const VECTOR_HANDLE handle);
    extern void* real_VECTOR_element(const VECTOR_HANDLE handle, size_t index);
    extern void real_VECTOR_erase(VECTOR_HANDLE handle, void* elements, size_t numElements);

    VECTOR_HANDLE my_VECTOR_create(size_t elementSize)
    {
        return real_VECTOR_create(elementSize);
    }

    void my_VECTOR_destroy(VECTOR_HANDLE handle)
    {
        real_VECTOR_destroy(handle);
    }

    void* my_VECTOR_find_if(const VECTOR_HANDLE handle, PREDICATE_FUNCTION pred, const void* value)
    {
        return real_VECTOR_find_if(handle, pred, value);
    }

    size_t my_VECTOR_size(const VECTOR_HANDLE handle)
    {
        return real_VECTOR_size(handle);
    }

    int my_VECTOR_push_back(VECTOR_HANDLE handle, const void* elements, size_t numElements)
    {
        return real_VECTOR_push_back(handle, elements, numElements);
    }

    void* my_VECTOR_element(const VECTOR_HANDLE handle, size_t index)
    {
        return real_VECTOR_element(handle, index);
    }

    void my_VECTOR_erase(VECTOR_HANDLE handle, void* elements, size_t numElements)
    {
        real_VECTOR_erase(handle, elements, numElements);
    }

	extern STRING_HANDLE real_STRING_construct(const char* psz);
	extern const char* real_STRING_c_str(STRING_HANDLE handle);
	extern void real_STRING_delete(STRING_HANDLE handle);

	STRING_HANDLE my_STRING_construct(const char* psz)
	{
		return real_STRING_construct(psz);
	}

	const char* my_STRING_c_str(STRING_HANDLE handle)
	{
		return real_STRING_c_str(handle);
	}

	void my_STRING_delete(STRING_HANDLE handle)
	{
		real_STRING_delete(handle);
	}

	extern int real_DList_RemoveEntryList(PDLIST_ENTRY Entry);
	extern void real_DList_InsertTailList(PDLIST_ENTRY ListHead, PDLIST_ENTRY Entry);
	extern int real_DList_IsListEmpty(const PDLIST_ENTRY ListHead);
	extern void real_DList_InitializeListHead(PDLIST_ENTRY ListHead);

	int my_DList_RemoveEntryList(PDLIST_ENTRY Entry)
	{
		return real_DList_RemoveEntryList(Entry);
	}

	void my_DList_InsertTailList(PDLIST_ENTRY ListHead, PDLIST_ENTRY Entry)
	{
		real_DList_InsertTailList(ListHead, Entry);
	}

	int my_DList_IsListEmpty(const PDLIST_ENTRY ListHead)
	{
		return real_DList_IsListEmpty(ListHead);
	}

	void my_DList_InitializeListHead(PDLIST_ENTRY ListHead)
	{
		real_DList_InitializeListHead(ListHead);
	}

	static ON_METHODS_ERROR g_on_methods_error;
	static void* g_on_methods_error_context;
	static ON_METHOD_REQUEST_RECEIVED g_on_method_request_received;
	static void* g_on_method_request_received_context;

	int my_iothubtransportamqp_methods_subscribe(IOTHUBTRANSPORT_AMQP_METHODS_HANDLE iothubtransport_amqp_methods_handle,
		SESSION_HANDLE session_handle, ON_METHODS_ERROR on_methods_error, void* on_methods_error_context,
		ON_METHOD_REQUEST_RECEIVED on_method_request_received, void* on_method_request_received_context)
	{
		(void)iothubtransport_amqp_methods_handle, session_handle;
		g_on_methods_error = on_methods_error;
		g_on_methods_error_context = on_methods_error_context;
		g_on_method_request_received = on_method_request_received;
		g_on_method_request_received_context = on_method_request_received_context;
		return 0;
	}

#ifdef __cplusplus
}
#endif

MOCKABLE_FUNCTION(, time_t, get_time, time_t*, currentTime);
MOCKABLE_FUNCTION(, double, get_difftime, time_t, stopTime, time_t, startTime);


static TEST_MUTEX_HANDLE g_testByTest;
static TEST_MUTEX_HANDLE g_dllByDll;

// Control parameters
#define INDEFINITE_TIME ((time_t)-1)
#define TEST_DEVICE_ID "deviceid"
#define TEST_DEVICE_KEY "devicekey"
#define TEST_DEVICE_SAS_TOKEN "deviceSas"
#define TEST_IOT_HUB_NAME "servername"
#define TEST_IOT_HUB_SUFFIX "domainname"
#define TEST_IOT_HUB_PORT 5671
#define TEST_PROT_GW_HOSTNAME_NULL NULL
#define TEST_PROT_GW_HOSTNAME "gw"

#define TEST_VECTOR_HANDLE (VECTOR_HANDLE)0x4242
#define TEST_STRING_HANDLE (STRING_HANDLE)0x101
#define TEST_WAIT_TO_SEND_LIST ((PDLIST_ENTRY)0x201)
#define TEST_AUTHENTICATION_STATE_HANDLE	((AUTHENTICATION_STATE_HANDLE)0x4243)
#define TEST_IOTHUBTRANSPORTAMQP_METHODS	((IOTHUBTRANSPORT_AMQP_METHODS_HANDLE)0x4244)
#define TEST_SESSION						((SESSION_HANDLE)0x4245)
#define TEST_METHOD_HANDLE                  ((IOTHUBTRANSPORT_AMQP_METHOD_HANDLE)0x4246)
#define TEST_XIO_INTERFACE                  ((const IO_INTERFACE_DESCRIPTION*)0x4247)
#define TEST_XIO_HANDLE                     ((XIO_HANDLE)0x4248)
#define TEST_SASL_MECHANISM                 ((SASL_MECHANISM_HANDLE)0x4249)
#define TEST_SASL_MSSBCBS_INTERFACE         ((const SASL_MECHANISM_INTERFACE_DESCRIPTION*)0x4250)
#define TEST_CONNECTION_HANDLE              ((CONNECTION_HANDLE)0x4251)
#define TEST_CBS_HANDLE                     ((CBS_HANDLE)0x4253)
#define TEST_MESSAGING_SOURCE               ((AMQP_VALUE)0x4254)
#define TEST_MESSAGING_TARGET               ((AMQP_VALUE)0x4255)
#define TEST_BUFFER_HANDLE                  ((BUFFER_HANDLE)0x4256)
#define TEST_LINK                           ((LINK_HANDLE)0x4257)
#define TEST_AMQP_MAP                       ((AMQP_VALUE)0x4258)
#define TEST_MESSAGE_SENDER                 ((MESSAGE_SENDER_HANDLE)0x4259)
#define TEST_AMQP_VALUE                     ((AMQP_VALUE)0x4260)

#define TEST_UNDERLYING_IO_TRANSPORT        ((XIO_HANDLE)0x4261)
#define TEST_TRANSPORT_PROVIDER             ((TRANSPORT_PROVIDER*)0x4263)

AMQP_TRANSPORT_CREDENTIAL test_transport_credential;
static const IOTHUB_CLIENT_LL_HANDLE TEST_IOTHUB_CLIENT_LL_HANDLE = (IOTHUB_CLIENT_LL_HANDLE)0x4343;

const TRANSPORT_PROVIDER* TEST_get_iothub_client_transport_provider(void)
{
	return TEST_TRANSPORT_PROVIDER;
}

static XIO_HANDLE TEST_amqp_get_io_transport(const char* target_fqdn)
{
	(void)target_fqdn;
	return TEST_UNDERLYING_IO_TRANSPORT;
}



DEFINE_ENUM_STRINGS(UMOCK_C_ERROR_CODE, UMOCK_C_ERROR_CODE_VALUES)

static void on_umock_c_error(UMOCK_C_ERROR_CODE error_code)
{
    char temp_str[256];
    (void)snprintf(temp_str, sizeof(temp_str), "umock_c reported error :%s", ENUM_TO_STRING(UMOCK_C_ERROR_CODE, error_code));
    ASSERT_FAIL(temp_str);
}

static STRING_HANDLE TEST_STRING_construct_sprintf(const char* format, ...)
{
	(void)format;
	return NULL;
}

static IOTHUB_CLIENT_CONFIG* create_client_config(IOTHUB_CLIENT_TRANSPORT_PROVIDER transport_provider)
{
	static IOTHUB_CLIENT_CONFIG client_config;
	client_config.protocol = transport_provider;
	client_config.deviceId = TEST_DEVICE_ID;
	client_config.deviceKey = TEST_DEVICE_KEY;
	client_config.deviceSasToken = TEST_DEVICE_SAS_TOKEN;
	client_config.iotHubName = TEST_IOT_HUB_NAME;
	client_config.iotHubSuffix = TEST_IOT_HUB_SUFFIX;
	client_config.protocolGatewayHostName = TEST_PROT_GW_HOSTNAME;
	return &client_config;
}

static IOTHUBTRANSPORT_CONFIG* create_transport_config(IOTHUB_CLIENT_TRANSPORT_PROVIDER transport_provider)
{
	static IOTHUBTRANSPORT_CONFIG config;
	config.upperConfig = create_client_config(transport_provider);
	config.waitingToSend = TEST_WAIT_TO_SEND_LIST;
	return &config;
}

BEGIN_TEST_SUITE(iothubtransport_amqp_common_ut)

TEST_SUITE_INITIALIZE(TestClassInitialize)
{
	AMQP_TRANSPORT_CREDENTIAL* test_transport_credential_ptr = &test_transport_credential;
    TEST_INITIALIZE_MEMORY_DEBUG(g_dllByDll);
    g_testByTest = TEST_MUTEX_CREATE();
    ASSERT_IS_NOT_NULL(g_testByTest);

    umock_c_init(on_umock_c_error);

	int result = umocktypes_charptr_register_types();
	ASSERT_ARE_EQUAL(int, 0, result);
	result = umocktypes_stdint_register_types();
	ASSERT_ARE_EQUAL(int, 0, result);
    result = umocktypes_bool_register_types();
    ASSERT_ARE_EQUAL(int, 0, result);

    /* test by default with X509 as it is simpler */
    test_transport_credential.type = X509;

	REGISTER_UMOCK_ALIAS_TYPE(IOTHUB_MESSAGE_HANDLE, void*);
	REGISTER_UMOCK_ALIAS_TYPE(IOTHUBMESSAGE_CONTENT_TYPE, int);
	REGISTER_UMOCK_ALIAS_TYPE(IOTHUB_MESSAGE_RESULT, void*);
	REGISTER_UMOCK_ALIAS_TYPE(MESSAGE_HANDLE, void*);
	REGISTER_UMOCK_ALIAS_TYPE(PROPERTIES_HANDLE, void*);
	REGISTER_UMOCK_ALIAS_TYPE(BINARY_DATA, void*);
	REGISTER_UMOCK_ALIAS_TYPE(MAP_HANDLE, void*);
	REGISTER_UMOCK_ALIAS_TYPE(AMQP_VALUE, void*);
	REGISTER_UMOCK_ALIAS_TYPE(MAP_RESULT, int);
	REGISTER_UMOCK_ALIAS_TYPE(AMQP_TYPE, int);
	REGISTER_UMOCK_ALIAS_TYPE(VECTOR_HANDLE, void*);
	REGISTER_UMOCK_ALIAS_TYPE(const VECTOR_HANDLE, void*);
	REGISTER_UMOCK_ALIAS_TYPE(PREDICATE_FUNCTION, void*);
	REGISTER_UMOCK_ALIAS_TYPE(PDLIST_ENTRY, void*);
	REGISTER_UMOCK_ALIAS_TYPE(STRING_HANDLE, void*);
	REGISTER_UMOCK_ALIAS_TYPE(AUTHENTICATION_STATE_HANDLE, void*);
	REGISTER_UMOCK_ALIAS_TYPE(MESSAGE_RECEIVER_HANDLE, void*);
	REGISTER_UMOCK_ALIAS_TYPE(LINK_HANDLE, void*);
	REGISTER_UMOCK_ALIAS_TYPE(IOTHUBTRANSPORT_AMQP_METHODS_HANDLE, void*);
	REGISTER_UMOCK_ALIAS_TYPE(SESSION_HANDLE, void*);
	REGISTER_UMOCK_ALIAS_TYPE(ON_METHODS_ERROR, void*);
	REGISTER_UMOCK_ALIAS_TYPE(ON_METHOD_REQUEST_RECEIVED, void*);
    REGISTER_UMOCK_ALIAS_TYPE(XIO_HANDLE, void*);
    REGISTER_UMOCK_ALIAS_TYPE(ON_NEW_ENDPOINT, void*);
    REGISTER_UMOCK_ALIAS_TYPE(ON_CONNECTION_STATE_CHANGED, void*);
    REGISTER_UMOCK_ALIAS_TYPE(ON_IO_ERROR, void*);
    REGISTER_UMOCK_ALIAS_TYPE(CONNECTION_HANDLE, void*);
    REGISTER_UMOCK_ALIAS_TYPE(ON_LINK_ATTACHED, void*);
    REGISTER_UMOCK_ALIAS_TYPE(ON_AMQP_MANAGEMENT_STATE_CHANGED, void*);
    REGISTER_UMOCK_ALIAS_TYPE(CBS_HANDLE, void*);
    REGISTER_UMOCK_ALIAS_TYPE(role, bool);
    REGISTER_UMOCK_ALIAS_TYPE(IOTHUB_CLIENT_LL_HANDLE, void*);
    REGISTER_UMOCK_ALIAS_TYPE(BUFFER_HANDLE, void*);
    REGISTER_UMOCK_ALIAS_TYPE(IOTHUBTRANSPORT_AMQP_METHOD_HANDLE, void*);
    REGISTER_UMOCK_ALIAS_TYPE(ON_MESSAGE_SENDER_STATE_CHANGED, void*);
    REGISTER_UMOCK_ALIAS_TYPE(MESSAGE_SENDER_HANDLE, void*);
    REGISTER_UMOCK_ALIAS_TYPE(fields, void*);

    REGISTER_GLOBAL_MOCK_HOOK(gballoc_malloc, my_gballoc_malloc);
    REGISTER_GLOBAL_MOCK_HOOK(gballoc_free, my_gballoc_free);
    REGISTER_GLOBAL_MOCK_HOOK(gballoc_realloc, my_gballoc_realloc);
    REGISTER_GLOBAL_MOCK_HOOK(iothubtransportamqp_methods_subscribe, my_iothubtransportamqp_methods_subscribe);
    
	REGISTER_GLOBAL_MOCK_RETURN(authentication_create, TEST_AUTHENTICATION_STATE_HANDLE);
	REGISTER_GLOBAL_MOCK_RETURN(authentication_get_credential, test_transport_credential_ptr);
	REGISTER_GLOBAL_MOCK_RETURN(iothubtransportamqp_methods_create, TEST_IOTHUBTRANSPORTAMQP_METHODS);
	REGISTER_GLOBAL_MOCK_RETURN(session_create, TEST_SESSION);
    REGISTER_GLOBAL_MOCK_RETURN(platform_get_default_tlsio, TEST_XIO_INTERFACE);
    REGISTER_GLOBAL_MOCK_RETURN(xio_create, TEST_XIO_HANDLE);
    REGISTER_GLOBAL_MOCK_RETURN(saslmssbcbs_get_interface, TEST_SASL_MSSBCBS_INTERFACE);
    REGISTER_GLOBAL_MOCK_RETURN(saslmechanism_create, TEST_SASL_MECHANISM);
    REGISTER_GLOBAL_MOCK_RETURN(connection_create2, TEST_CONNECTION_HANDLE);
    REGISTER_GLOBAL_MOCK_RETURN(cbs_create, TEST_CBS_HANDLE);
    REGISTER_GLOBAL_MOCK_RETURN(authentication_get_status, AUTHENTICATION_STATUS_OK);
    REGISTER_GLOBAL_MOCK_RETURN(messaging_create_source, TEST_MESSAGING_SOURCE);
    REGISTER_GLOBAL_MOCK_RETURN(messaging_create_target, TEST_MESSAGING_TARGET);
    REGISTER_GLOBAL_MOCK_RETURN(BUFFER_new, TEST_BUFFER_HANDLE);
    REGISTER_GLOBAL_MOCK_RETURN(link_create, TEST_LINK);
    REGISTER_GLOBAL_MOCK_RETURN(amqpvalue_create_map, TEST_AMQP_MAP);
    REGISTER_GLOBAL_MOCK_RETURN(amqpvalue_create_symbol, TEST_AMQP_VALUE);
    REGISTER_GLOBAL_MOCK_RETURN(amqpvalue_create_string, TEST_AMQP_VALUE);
    REGISTER_GLOBAL_MOCK_RETURN(messagesender_create, TEST_MESSAGE_SENDER);

    REGISTER_GLOBAL_MOCK_HOOK(VECTOR_create, my_VECTOR_create);
    REGISTER_GLOBAL_MOCK_HOOK(VECTOR_destroy, my_VECTOR_destroy);
    REGISTER_GLOBAL_MOCK_HOOK(VECTOR_push_back, my_VECTOR_push_back);
    REGISTER_GLOBAL_MOCK_HOOK(VECTOR_element, my_VECTOR_element);
    REGISTER_GLOBAL_MOCK_HOOK(VECTOR_size, my_VECTOR_size);
    REGISTER_GLOBAL_MOCK_HOOK(VECTOR_find_if, my_VECTOR_find_if);
    REGISTER_GLOBAL_MOCK_HOOK(VECTOR_erase, my_VECTOR_erase);

	REGISTER_GLOBAL_MOCK_HOOK(STRING_construct, my_STRING_construct);
	REGISTER_GLOBAL_MOCK_HOOK(STRING_delete, my_STRING_delete);
	REGISTER_GLOBAL_MOCK_HOOK(STRING_c_str, my_STRING_c_str);

	REGISTER_GLOBAL_MOCK_HOOK(DList_RemoveEntryList, my_DList_RemoveEntryList);
	REGISTER_GLOBAL_MOCK_HOOK(DList_InsertTailList, my_DList_InsertTailList);
	REGISTER_GLOBAL_MOCK_HOOK(DList_IsListEmpty, my_DList_IsListEmpty);
	REGISTER_GLOBAL_MOCK_HOOK(DList_InitializeListHead, my_DList_InitializeListHead);
}

TEST_SUITE_CLEANUP(TestClassCleanup)
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

    umock_c_reset_all_calls();
}

TEST_FUNCTION_CLEANUP(TestMethodCleanup)
{
    TEST_MUTEX_RELEASE(g_testByTest);
}

// Tests_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_005: [If parameter config or config->upperConfig are NULL then IoTHubTransport_AMQP_Common_Create shall fail and return NULL.]
TEST_FUNCTION(AMQP_Create_NULL_config)
{
    // arrange
	umock_c_reset_all_calls();

    // act
	TRANSPORT_LL_HANDLE tr_hdl = IoTHubTransport_AMQP_Common_Create(NULL, NULL);

    // assert
	ASSERT_IS_NULL_WITH_MSG(tr_hdl, "Expected NULL transport handle");

	// cleanup
}

// Tests_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_005: [If parameter config or config->upperConfig are NULL then IoTHubTransport_AMQP_Common_Create shall fail and return NULL.]
TEST_FUNCTION(AMQP_Create_NULL_upperConfig)
{
	// arrange
	IOTHUBTRANSPORT_CONFIG config;
	config.upperConfig = NULL;

	umock_c_reset_all_calls();

	// act
	TRANSPORT_LL_HANDLE tr_hdl = IoTHubTransport_AMQP_Common_Create(&config, TEST_amqp_get_io_transport);

	// assert
	ASSERT_IS_NULL(tr_hdl);
}

// Tests_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_006: [IoTHubTransport_AMQP_Common_Create shall fail and return NULL if any fields of the config->upperConfig structure are NULL.]
TEST_FUNCTION(AMQP_Create_upperConfig_protocol_NULL)
{
	// arrange
	IOTHUBTRANSPORT_CONFIG* config = create_transport_config(NULL);

	umock_c_reset_all_calls();

	// act
	TRANSPORT_LL_HANDLE handle = IoTHubTransport_AMQP_Common_Create(config, TEST_amqp_get_io_transport);

	// assert
	ASSERT_IS_NULL(handle);
}

// Tests_SRS_IOTHUBTRANSPORT_AMQP_COMMON_09_006: [IoTHubTransport_AMQP_Common_Create shall fail and return NULL if any fields of the config->upperConfig structure are NULL.]
TEST_FUNCTION(AMQP_Create_upperConfig_iotHubName_NULL)
{
	// arrange
	IOTHUB_CLIENT_CONFIG client_config;
	client_config.protocol = NULL;
	client_config.deviceId = TEST_DEVICE_ID;
	client_config.deviceKey = TEST_DEVICE_KEY;
	client_config.deviceSasToken = TEST_DEVICE_SAS_TOKEN;
	client_config.iotHubName = NULL;
	client_config.iotHubSuffix = TEST_IOT_HUB_SUFFIX;
	client_config.protocolGatewayHostName = TEST_PROT_GW_HOSTNAME;

	IOTHUBTRANSPORT_CONFIG config;
	config.upperConfig = &client_config;
	config.waitingToSend = TEST_WAIT_TO_SEND_LIST;

	umock_c_reset_all_calls();

	// act
	TRANSPORT_LL_HANDLE handle = IoTHubTransport_AMQP_Common_Create(&config, TEST_amqp_get_io_transport);

	// assert
	ASSERT_IS_NULL(handle);
}

/* IoTHubTransport_AMQP_Common_Register */

/* Tests_SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_010: [ `IoTHubTransport_AMQP_Common_Register` shall create a new iothubtransportamqp_methods instance by calling `iothubtransportamqp_methods_create` while passing to it the the fully qualified domain name and the device Id. ]*/
TEST_FUNCTION(IoTHubTransport_AMQP_Common_Register_creates_a_new_methods_handler)
{
    // arrange
    IOTHUB_CLIENT_CONFIG client_config;
    IOTHUBTRANSPORT_CONFIG config;
    IOTHUB_DEVICE_CONFIG device_config;
    DLIST_ENTRY waitingToSend;
    TRANSPORT_LL_HANDLE handle;
    IOTHUB_DEVICE_HANDLE device_handle;

    client_config.protocol = TEST_get_iothub_client_transport_provider;
    client_config.deviceId = TEST_DEVICE_ID;
    client_config.deviceKey = TEST_DEVICE_KEY;
    client_config.deviceSasToken = TEST_DEVICE_SAS_TOKEN;
    client_config.iotHubName = TEST_IOT_HUB_NAME;
    client_config.iotHubSuffix = TEST_IOT_HUB_SUFFIX;
    client_config.protocolGatewayHostName = TEST_PROT_GW_HOSTNAME;

    config.upperConfig = &client_config;
    config.waitingToSend = TEST_WAIT_TO_SEND_LIST;

    device_config.deviceId = "blah";
    device_config.deviceKey = "cucu";
    device_config.deviceSasToken = NULL;

    handle = IoTHubTransport_AMQP_Common_Create(&config, TEST_amqp_get_io_transport);
    umock_c_reset_all_calls();

    EXPECTED_CALL(VECTOR_find_if(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(DList_InitializeListHead(IGNORED_PTR_ARG));
    EXPECTED_CALL(STRING_construct(IGNORED_PTR_ARG));
    EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG));
    EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG));
    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(STRING_construct(IGNORED_PTR_ARG));
    EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));
    EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG));
    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(STRING_construct(IGNORED_PTR_ARG));
    EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));
    EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG));
    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(STRING_construct(IGNORED_PTR_ARG));
    EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));
    EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG))
        .SetReturn(TEST_IOT_HUB_NAME "." TEST_IOT_HUB_SUFFIX);

    STRICT_EXPECTED_CALL(iothubtransportamqp_methods_create(TEST_IOT_HUB_NAME "." TEST_IOT_HUB_SUFFIX, "blah"));

    EXPECTED_CALL(VECTOR_push_back(IGNORED_PTR_ARG, IGNORED_PTR_ARG, 1));
    EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG));
    EXPECTED_CALL(authentication_create(IGNORED_PTR_ARG));
    EXPECTED_CALL(VECTOR_size(IGNORED_PTR_ARG));
    EXPECTED_CALL(authentication_get_credential(IGNORED_PTR_ARG));

    // act
    device_handle = IoTHubTransport_AMQP_Common_Register(handle, &device_config, TEST_IOTHUB_CLIENT_LL_HANDLE, &waitingToSend);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_IS_NOT_NULL(device_handle);

    // cleanup
    IoTHubTransport_AMQP_Common_Destroy(handle);
}

/* Tests_SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_011: [ If `iothubtransportamqp_methods_create` fails, `IoTHubTransport_AMQP_Common_Register` shall fail and return NULL. ]*/
TEST_FUNCTION(when_creating_the_methods_handler_fails_then_IoTHubTransport_AMQP_Common_Register_fails)
{
    // arrange
    IOTHUB_CLIENT_CONFIG client_config;
    IOTHUBTRANSPORT_CONFIG config;
    IOTHUB_DEVICE_CONFIG device_config;
    DLIST_ENTRY waitingToSend;
    TRANSPORT_LL_HANDLE handle;
    IOTHUB_DEVICE_HANDLE device_handle;

    client_config.protocol = TEST_get_iothub_client_transport_provider;
    client_config.deviceId = TEST_DEVICE_ID;
    client_config.deviceKey = TEST_DEVICE_KEY;
    client_config.deviceSasToken = TEST_DEVICE_SAS_TOKEN;
    client_config.iotHubName = TEST_IOT_HUB_NAME;
    client_config.iotHubSuffix = TEST_IOT_HUB_SUFFIX;
    client_config.protocolGatewayHostName = NULL;

    config.upperConfig = &client_config;
    config.waitingToSend = TEST_WAIT_TO_SEND_LIST;

    device_config.deviceId = "blah";
    device_config.deviceKey = "cucu";
    device_config.deviceSasToken = NULL;

    handle = IoTHubTransport_AMQP_Common_Create(&config, TEST_amqp_get_io_transport);
    umock_c_reset_all_calls();

    EXPECTED_CALL(VECTOR_find_if(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(DList_InitializeListHead(IGNORED_PTR_ARG));
    EXPECTED_CALL(STRING_construct(IGNORED_PTR_ARG));
    EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG));
    EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG));
    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(STRING_construct(IGNORED_PTR_ARG));
    EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));
    EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG));
    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(STRING_construct(IGNORED_PTR_ARG));
    EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));
    EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG));
    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(STRING_construct(IGNORED_PTR_ARG));
    EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));
    EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG));

    STRICT_EXPECTED_CALL(iothubtransportamqp_methods_create(TEST_IOT_HUB_NAME "." TEST_IOT_HUB_SUFFIX, "blah"))
        .SetReturn(NULL);

    EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG));
    EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG));
    EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG));
    EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG));
    EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));

    // act
    device_handle = IoTHubTransport_AMQP_Common_Register(handle, &device_config, TEST_IOTHUB_CLIENT_LL_HANDLE, &waitingToSend);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_IS_NULL(device_handle);

    // cleanup
    IoTHubTransport_AMQP_Common_Destroy(handle);
}

/* IoTHubTransport_AMQP_Common_Unregister */

/* Tests_SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_012: [IoTHubTransport_AMQP_Common_Unregister shall destroy the C2D methods handler by calling iothubtransportamqp_methods_destroy.]*/
TEST_FUNCTION(IoTHubTransport_AMQP_Common_Unregister_destroys_the_methods_handler)
{
    // arrange
    IOTHUB_CLIENT_CONFIG client_config;
    IOTHUBTRANSPORT_CONFIG config;
    IOTHUB_DEVICE_CONFIG device_config;
    DLIST_ENTRY waitingToSend;
    TRANSPORT_LL_HANDLE handle;
    IOTHUB_DEVICE_HANDLE device_handle;

    client_config.protocol = TEST_get_iothub_client_transport_provider;
    client_config.deviceId = TEST_DEVICE_ID;
    client_config.deviceKey = TEST_DEVICE_KEY;
    client_config.deviceSasToken = TEST_DEVICE_SAS_TOKEN;
    client_config.iotHubName = TEST_IOT_HUB_NAME;
    client_config.iotHubSuffix = TEST_IOT_HUB_SUFFIX;
    client_config.protocolGatewayHostName = NULL;

    config.upperConfig = &client_config;
    config.waitingToSend = TEST_WAIT_TO_SEND_LIST;

    device_config.deviceId = "blah";
    device_config.deviceKey = "cucu";
    device_config.deviceSasToken = NULL;

    handle = IoTHubTransport_AMQP_Common_Create(&config, TEST_amqp_get_io_transport);
    device_handle = IoTHubTransport_AMQP_Common_Register(handle, &device_config, TEST_IOTHUB_CLIENT_LL_HANDLE, &waitingToSend);
    umock_c_reset_all_calls();

    EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG));
    EXPECTED_CALL(VECTOR_find_if(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG));
    EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG));
    EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG));
    EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG));
    EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG));
    EXPECTED_CALL(authentication_destroy(IGNORED_PTR_ARG));

    STRICT_EXPECTED_CALL(iothubtransportamqp_methods_destroy(TEST_IOTHUBTRANSPORTAMQP_METHODS));

    EXPECTED_CALL(VECTOR_erase(IGNORED_PTR_ARG, IGNORED_PTR_ARG, 1));
    EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));

    // act
    IoTHubTransport_AMQP_Common_Unregister(device_handle);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    IoTHubTransport_AMQP_Common_Destroy(handle);
}

/* IoTHubTransport_AMQP_Common_Subscribe_DeviceMethod */

/* Tests_SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_004: [ If `handle` is NULL, `IoTHubTransport_AMQP_Common_Subscribe_DeviceMethod` shall fail and return a non-zero value. ]*/
TEST_FUNCTION(IoTHubTransport_AMQP_Common_Subscribe_DeviceMethod_with_NULL_handle_fails)
{
	// arrange
	int result;

	// act
	result = IoTHubTransport_AMQP_Common_Subscribe_DeviceMethod(NULL);

	// assert
	ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
	ASSERT_ARE_NOT_EQUAL(int, 0, result);
}

/* Tests_SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_026: [ `IoTHubTransport_AMQP_Common_Subscribe_DeviceMethod` shall remember that a subscribe is to be performed in the next call to DoWork and on success it shall return 0. ]*/
TEST_FUNCTION(IoTHubTransport_AMQP_Common_Subscribe_DeviceMethod_with_valid_handle_succeeds)
{
    // arrange
    int result;

    IOTHUB_CLIENT_CONFIG client_config;
    IOTHUBTRANSPORT_CONFIG config;
    IOTHUB_DEVICE_CONFIG device_config;
    DLIST_ENTRY waitingToSend;
    TRANSPORT_LL_HANDLE handle;
    IOTHUB_DEVICE_HANDLE device_handle;

    client_config.protocol = TEST_get_iothub_client_transport_provider;
    client_config.deviceId = TEST_DEVICE_ID;
    client_config.deviceKey = TEST_DEVICE_KEY;
    client_config.deviceSasToken = TEST_DEVICE_SAS_TOKEN;
    client_config.iotHubName = TEST_IOT_HUB_NAME;
    client_config.iotHubSuffix = TEST_IOT_HUB_SUFFIX;
    client_config.protocolGatewayHostName = TEST_PROT_GW_HOSTNAME;

    config.upperConfig = &client_config;
    config.waitingToSend = TEST_WAIT_TO_SEND_LIST;

    device_config.deviceId = "blah";
    device_config.deviceKey = "cucu";
    device_config.deviceSasToken = NULL;

    handle = IoTHubTransport_AMQP_Common_Create(&config, TEST_amqp_get_io_transport);
    device_handle = IoTHubTransport_AMQP_Common_Register(handle, &device_config, TEST_IOTHUB_CLIENT_LL_HANDLE, &waitingToSend);
    umock_c_reset_all_calls();

    // act
    result = IoTHubTransport_AMQP_Common_Subscribe_DeviceMethod(device_handle);

    // assert
	ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
	ASSERT_ARE_EQUAL(int, 0, result);

    // cleanup
    IoTHubTransport_AMQP_Common_Destroy(handle);
}

/* Tests_SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_026: [ `IoTHubTransport_AMQP_Common_Subscribe_DeviceMethod` shall remember that a subscribe is to be performed in the next call to DoWork and on success it shall return 0. ]*/
TEST_FUNCTION(IoTHubTransport_AMQP_Common_Subscribe_After_Subscribe_succeeds)
{
	// arrange
	int result;

	IOTHUB_CLIENT_CONFIG client_config;
	IOTHUBTRANSPORT_CONFIG config;
	IOTHUB_DEVICE_CONFIG device_config;
	DLIST_ENTRY waitingToSend;
	TRANSPORT_LL_HANDLE handle;
	IOTHUB_DEVICE_HANDLE device_handle;

	client_config.protocol = TEST_get_iothub_client_transport_provider;
	client_config.deviceId = TEST_DEVICE_ID;
	client_config.deviceKey = TEST_DEVICE_KEY;
	client_config.deviceSasToken = TEST_DEVICE_SAS_TOKEN;
	client_config.iotHubName = TEST_IOT_HUB_NAME;
	client_config.iotHubSuffix = TEST_IOT_HUB_SUFFIX;
	client_config.protocolGatewayHostName = TEST_PROT_GW_HOSTNAME;

	config.upperConfig = &client_config;
	config.waitingToSend = TEST_WAIT_TO_SEND_LIST;

	device_config.deviceId = "blah";
	device_config.deviceKey = "cucu";
	device_config.deviceSasToken = NULL;

	handle = IoTHubTransport_AMQP_Common_Create(&config, TEST_amqp_get_io_transport);
	device_handle = IoTHubTransport_AMQP_Common_Register(handle, &device_config, TEST_IOTHUB_CLIENT_LL_HANDLE, &waitingToSend);
	(void)IoTHubTransport_AMQP_Common_Subscribe_DeviceMethod(device_handle);
	umock_c_reset_all_calls();

	// act
	result = IoTHubTransport_AMQP_Common_Subscribe_DeviceMethod(device_handle);

	// assert
	ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
	ASSERT_ARE_EQUAL(int, 0, result);

	// cleanup
	IoTHubTransport_AMQP_Common_Destroy(handle);
}

/* IoTHubTransport_AMQP_Common_Unsubscribe_DeviceMethod */

/* Tests_SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_006: [ If `handle` is NULL, `IoTHubTransport_AMQP_Common_Unsubscribe_DeviceMethod` shall do nothing. ]*/
TEST_FUNCTION(IoTHubTransport_AMQP_Common_Unsubscribe_With_NULL_handle_does_nothing)
{
	// arrange
	// act
	IoTHubTransport_AMQP_Common_Unsubscribe_DeviceMethod(NULL);

	// assert
	ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
}

/* Tests_SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_007: [ `IoTHubTransport_AMQP_Common_Unsubscribe_DeviceMethod` shall unsubscribe from receiving C2D method requests by calling `iothubtransportamqp_methods_unsubscribe`. ]*/
TEST_FUNCTION(IoTHubTransport_AMQP_Common_Unsubscribe_unsubscribes_from_receiving_methods)
{
	// arrange
	IOTHUB_CLIENT_CONFIG client_config;
	IOTHUBTRANSPORT_CONFIG config;
	IOTHUB_DEVICE_CONFIG device_config;
	DLIST_ENTRY waitingToSend;
	TRANSPORT_LL_HANDLE handle;
	IOTHUB_DEVICE_HANDLE device_handle;

	client_config.protocol = TEST_get_iothub_client_transport_provider;
	client_config.deviceId = TEST_DEVICE_ID;
	client_config.deviceKey = TEST_DEVICE_KEY;
	client_config.deviceSasToken = TEST_DEVICE_SAS_TOKEN;
	client_config.iotHubName = TEST_IOT_HUB_NAME;
	client_config.iotHubSuffix = TEST_IOT_HUB_SUFFIX;
	client_config.protocolGatewayHostName = TEST_PROT_GW_HOSTNAME;

	config.upperConfig = &client_config;
	config.waitingToSend = TEST_WAIT_TO_SEND_LIST;

	device_config.deviceId = "blah";
	device_config.deviceKey = "cucu";
	device_config.deviceSasToken = NULL;

	handle = IoTHubTransport_AMQP_Common_Create(&config, TEST_amqp_get_io_transport);
	device_handle = IoTHubTransport_AMQP_Common_Register(handle, &device_config, TEST_IOTHUB_CLIENT_LL_HANDLE, &waitingToSend);
	(void)IoTHubTransport_AMQP_Common_Subscribe_DeviceMethod(device_handle);
	umock_c_reset_all_calls();

	STRICT_EXPECTED_CALL(iothubtransportamqp_methods_unsubscribe(TEST_IOTHUBTRANSPORTAMQP_METHODS));

	// act
	IoTHubTransport_AMQP_Common_Unsubscribe_DeviceMethod(device_handle);

	// assert
	ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

	// cleanup
	IoTHubTransport_AMQP_Common_Destroy(handle);
}

/* Tests_SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_008: [ If the transport is not subscribed to receive C2D method requests then `IoTHubTransport_AMQP_Common_Unsubscribe_DeviceMethod` shall do nothing. ]*/
TEST_FUNCTION(IoTHubTransport_AMQP_Common_Unsubscribe_without_subscribe_does_nothing)
{
	// arrange
	IOTHUB_CLIENT_CONFIG client_config;
	IOTHUBTRANSPORT_CONFIG config;
	IOTHUB_DEVICE_CONFIG device_config;
	DLIST_ENTRY waitingToSend;
	TRANSPORT_LL_HANDLE handle;
	IOTHUB_DEVICE_HANDLE device_handle;

	client_config.protocol = TEST_get_iothub_client_transport_provider;
	client_config.deviceId = TEST_DEVICE_ID;
	client_config.deviceKey = TEST_DEVICE_KEY;
	client_config.deviceSasToken = TEST_DEVICE_SAS_TOKEN;
	client_config.iotHubName = TEST_IOT_HUB_NAME;
	client_config.iotHubSuffix = TEST_IOT_HUB_SUFFIX;
	client_config.protocolGatewayHostName = TEST_PROT_GW_HOSTNAME;

	config.upperConfig = &client_config;
	config.waitingToSend = TEST_WAIT_TO_SEND_LIST;

	device_config.deviceId = "blah";
	device_config.deviceKey = "cucu";
	device_config.deviceSasToken = NULL;

	handle = IoTHubTransport_AMQP_Common_Create(&config, TEST_amqp_get_io_transport);
	device_handle = IoTHubTransport_AMQP_Common_Register(handle, &device_config, TEST_IOTHUB_CLIENT_LL_HANDLE, &waitingToSend);
	umock_c_reset_all_calls();

	// act
	IoTHubTransport_AMQP_Common_Unsubscribe_DeviceMethod(device_handle);

	// assert
	ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

	// cleanup
	IoTHubTransport_AMQP_Common_Destroy(handle);
}

/* IoTHubTransport_AMQP_Common_DoWork */

/* Tests_SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_024: [ If the device authentication status is AUTHENTICATION_STATUS_OK and `IoTHubTransport_AMQP_Common_Subscribe_DeviceMethod` was called to register for methods, `IoTHubTransport_AMQP_Common_DoWork` shall call `iothubtransportamqp_methods_subscribe`. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_027: [ The current session handle shall be passed to `iothubtransportamqp_methods_subscribe`. ]*/
TEST_FUNCTION(IoTHubTransport_AMQP_Common_DoWork_subscribes_for_methods)
{
	// arrange
	IOTHUB_CLIENT_CONFIG client_config;
	IOTHUBTRANSPORT_CONFIG config;
	IOTHUB_DEVICE_CONFIG device_config;
	DLIST_ENTRY waitingToSend;
	TRANSPORT_LL_HANDLE handle;
	IOTHUB_DEVICE_HANDLE device_handle;

	client_config.protocol = TEST_get_iothub_client_transport_provider;
	client_config.deviceId = TEST_DEVICE_ID;
	client_config.deviceKey = TEST_DEVICE_KEY;
	client_config.deviceSasToken = TEST_DEVICE_SAS_TOKEN;
	client_config.iotHubName = TEST_IOT_HUB_NAME;
	client_config.iotHubSuffix = TEST_IOT_HUB_SUFFIX;
	client_config.protocolGatewayHostName = TEST_PROT_GW_HOSTNAME;

	config.upperConfig = &client_config;
	config.waitingToSend = TEST_WAIT_TO_SEND_LIST;

	device_config.deviceId = "blah";
	device_config.deviceKey = "cucu";
	device_config.deviceSasToken = NULL;

	handle = IoTHubTransport_AMQP_Common_Create(&config, TEST_amqp_get_io_transport);
	device_handle = IoTHubTransport_AMQP_Common_Register(handle, &device_config, TEST_IOTHUB_CLIENT_LL_HANDLE, &waitingToSend);
	(void)IoTHubTransport_AMQP_Common_Subscribe_DeviceMethod(device_handle);
	umock_c_reset_all_calls();

    /* EXPECTED call because we don't really care about the arguments for simply testing that the methods subscribe happened */
	EXPECTED_CALL(VECTOR_size(IGNORED_PTR_ARG));
	EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG));
    EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG));
    EXPECTED_CALL(connection_create2(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    EXPECTED_CALL(session_create(IGNORED_PTR_ARG, IGNORED_NUM_ARG, IGNORED_NUM_ARG));
    EXPECTED_CALL(session_set_incoming_window(IGNORED_PTR_ARG, IGNORED_NUM_ARG));
    EXPECTED_CALL(session_set_outgoing_window(IGNORED_PTR_ARG, IGNORED_NUM_ARG));
    EXPECTED_CALL(connection_set_trace(IGNORED_PTR_ARG, false));
    EXPECTED_CALL(xio_setoption(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    EXPECTED_CALL(VECTOR_element(IGNORED_PTR_ARG, IGNORED_NUM_ARG));
    EXPECTED_CALL(authentication_get_status(IGNORED_PTR_ARG));

    /* this is the call we're reall interested in */
    STRICT_EXPECTED_CALL(iothubtransportamqp_methods_subscribe(TEST_IOTHUBTRANSPORTAMQP_METHODS, TEST_SESSION, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument_on_methods_error()
        .IgnoreArgument_on_methods_error_context()
        .IgnoreArgument_on_method_request_received()
        .IgnoreArgument_on_method_request_received_context();

    /* more uninteresting calls */
    EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG));
    EXPECTED_CALL(STRING_construct(IGNORED_PTR_ARG));
    EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG));
    EXPECTED_CALL(STRING_construct(IGNORED_PTR_ARG));
    EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG));
    EXPECTED_CALL(messaging_create_source(IGNORED_PTR_ARG));
    EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG));
    EXPECTED_CALL(messaging_create_target(IGNORED_PTR_ARG));
    EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG));
    EXPECTED_CALL(link_create(IGNORED_PTR_ARG, IGNORED_PTR_ARG, role_sender, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    EXPECTED_CALL(link_set_max_message_size(IGNORED_PTR_ARG, IGNORED_NUM_ARG));
    EXPECTED_CALL(amqpvalue_create_map());
    EXPECTED_CALL(amqpvalue_create_symbol(IGNORED_PTR_ARG));
    EXPECTED_CALL(amqpvalue_create_string(IGNORED_PTR_ARG));
    EXPECTED_CALL(amqpvalue_set_map_value(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    EXPECTED_CALL(link_set_attach_properties(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    EXPECTED_CALL(amqpvalue_destroy(IGNORED_PTR_ARG));
    EXPECTED_CALL(amqpvalue_destroy(IGNORED_PTR_ARG));
    EXPECTED_CALL(amqpvalue_destroy(IGNORED_PTR_ARG));
    EXPECTED_CALL(messagesender_create(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    EXPECTED_CALL(messagesender_open(IGNORED_PTR_ARG));
    EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG));
    EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG));
    EXPECTED_CALL(amqpvalue_destroy(IGNORED_PTR_ARG));
    EXPECTED_CALL(amqpvalue_destroy(IGNORED_PTR_ARG));
    EXPECTED_CALL(connection_dowork(IGNORED_PTR_ARG));

	// act
	IoTHubTransport_AMQP_Common_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

	// assert
	ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

	// cleanup
	IoTHubTransport_AMQP_Common_Destroy(handle);
}

/* Tests_SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_024: [ If the device authentication status is AUTHENTICATION_STATUS_OK and `IoTHubTransport_AMQP_Common_Subscribe_DeviceMethod` was called to register for methods, `IoTHubTransport_AMQP_Common_DoWork` shall call `iothubtransportamqp_methods_subscribe`. ]*/
TEST_FUNCTION(IoTHubTransport_AMQP_Common_DoWork_does_not_subscribe_if_SubScribe_for_methods_was_not_Called_by_the_user)
{
    // arrange
    IOTHUB_CLIENT_CONFIG client_config;
    IOTHUBTRANSPORT_CONFIG config;
    IOTHUB_DEVICE_CONFIG device_config;
    DLIST_ENTRY waitingToSend;
    TRANSPORT_LL_HANDLE handle;
    IOTHUB_DEVICE_HANDLE device_handle;

    client_config.protocol = TEST_get_iothub_client_transport_provider;
    client_config.deviceId = TEST_DEVICE_ID;
    client_config.deviceKey = TEST_DEVICE_KEY;
    client_config.deviceSasToken = TEST_DEVICE_SAS_TOKEN;
    client_config.iotHubName = TEST_IOT_HUB_NAME;
    client_config.iotHubSuffix = TEST_IOT_HUB_SUFFIX;
    client_config.protocolGatewayHostName = TEST_PROT_GW_HOSTNAME;

    config.upperConfig = &client_config;
    config.waitingToSend = TEST_WAIT_TO_SEND_LIST;

    device_config.deviceId = "blah";
    device_config.deviceKey = "cucu";
    device_config.deviceSasToken = NULL;

    handle = IoTHubTransport_AMQP_Common_Create(&config, TEST_amqp_get_io_transport);
    device_handle = IoTHubTransport_AMQP_Common_Register(handle, &device_config, TEST_IOTHUB_CLIENT_LL_HANDLE, &waitingToSend);
    umock_c_reset_all_calls();

    EXPECTED_CALL(VECTOR_size(IGNORED_PTR_ARG));
    EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG));
    EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG));
    EXPECTED_CALL(connection_create2(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    EXPECTED_CALL(session_create(IGNORED_PTR_ARG, IGNORED_NUM_ARG, IGNORED_NUM_ARG));
    EXPECTED_CALL(session_set_incoming_window(IGNORED_PTR_ARG, IGNORED_NUM_ARG));
    EXPECTED_CALL(session_set_outgoing_window(IGNORED_PTR_ARG, IGNORED_NUM_ARG));
    EXPECTED_CALL(connection_set_trace(IGNORED_PTR_ARG, false));
    EXPECTED_CALL(xio_setoption(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    EXPECTED_CALL(VECTOR_element(IGNORED_PTR_ARG, IGNORED_NUM_ARG));
    EXPECTED_CALL(authentication_get_status(IGNORED_PTR_ARG));
    EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG));
    EXPECTED_CALL(STRING_construct(IGNORED_PTR_ARG));
    EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG));
    EXPECTED_CALL(STRING_construct(IGNORED_PTR_ARG));
    EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG));
    EXPECTED_CALL(messaging_create_source(IGNORED_PTR_ARG));
    EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG));
    EXPECTED_CALL(messaging_create_target(IGNORED_PTR_ARG));
    EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG));
    EXPECTED_CALL(link_create(IGNORED_PTR_ARG, IGNORED_PTR_ARG, role_sender, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    EXPECTED_CALL(link_set_max_message_size(IGNORED_PTR_ARG, IGNORED_NUM_ARG));
    EXPECTED_CALL(amqpvalue_create_map());
    EXPECTED_CALL(amqpvalue_create_symbol(IGNORED_PTR_ARG));
    EXPECTED_CALL(amqpvalue_create_string(IGNORED_PTR_ARG));
    EXPECTED_CALL(amqpvalue_set_map_value(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    EXPECTED_CALL(link_set_attach_properties(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    EXPECTED_CALL(amqpvalue_destroy(IGNORED_PTR_ARG));
    EXPECTED_CALL(amqpvalue_destroy(IGNORED_PTR_ARG));
    EXPECTED_CALL(amqpvalue_destroy(IGNORED_PTR_ARG));
    EXPECTED_CALL(messagesender_create(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    EXPECTED_CALL(messagesender_open(IGNORED_PTR_ARG));
    EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG));
    EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG));
    EXPECTED_CALL(amqpvalue_destroy(IGNORED_PTR_ARG));
    EXPECTED_CALL(amqpvalue_destroy(IGNORED_PTR_ARG));
    EXPECTED_CALL(connection_dowork(IGNORED_PTR_ARG));

    // act
    IoTHubTransport_AMQP_Common_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    IoTHubTransport_AMQP_Common_Destroy(handle);
}

/* Tests_SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_031: [ `iothubtransportamqp_methods_subscribe` shall only be called once (subsequent DoWork calls shall not call it if already subscribed). ]*/
TEST_FUNCTION(IoTHubTransport_AMQP_Common_DoWork_does_not_subscribe_if_already_subscribed)
{
    // arrange
    IOTHUB_CLIENT_CONFIG client_config;
    IOTHUBTRANSPORT_CONFIG config;
    IOTHUB_DEVICE_CONFIG device_config;
    DLIST_ENTRY waitingToSend;
    TRANSPORT_LL_HANDLE handle;
    IOTHUB_DEVICE_HANDLE device_handle;

    client_config.protocol = TEST_get_iothub_client_transport_provider;
    client_config.deviceId = TEST_DEVICE_ID;
    client_config.deviceKey = TEST_DEVICE_KEY;
    client_config.deviceSasToken = TEST_DEVICE_SAS_TOKEN;
    client_config.iotHubName = TEST_IOT_HUB_NAME;
    client_config.iotHubSuffix = TEST_IOT_HUB_SUFFIX;
    client_config.protocolGatewayHostName = TEST_PROT_GW_HOSTNAME;

    config.upperConfig = &client_config;
    config.waitingToSend = TEST_WAIT_TO_SEND_LIST;

    device_config.deviceId = "blah";
    device_config.deviceKey = "cucu";
    device_config.deviceSasToken = NULL;

    handle = IoTHubTransport_AMQP_Common_Create(&config, TEST_amqp_get_io_transport);
    device_handle = IoTHubTransport_AMQP_Common_Register(handle, &device_config, TEST_IOTHUB_CLIENT_LL_HANDLE, &waitingToSend);
    (void)IoTHubTransport_AMQP_Common_Subscribe_DeviceMethod(device_handle);
    IoTHubTransport_AMQP_Common_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);
    umock_c_reset_all_calls();

    /* EXPECTED call because we don't really care about the arguments for simply testing that the methods subscribe happened */
    EXPECTED_CALL(VECTOR_size(IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(VECTOR_element(IGNORED_PTR_ARG, 0))
        .IgnoreArgument_handle();
    STRICT_EXPECTED_CALL(authentication_get_status(TEST_AUTHENTICATION_STATE_HANDLE));
    STRICT_EXPECTED_CALL(connection_dowork(TEST_CONNECTION_HANDLE));

    // act
    IoTHubTransport_AMQP_Common_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    IoTHubTransport_AMQP_Common_Destroy(handle);
}

/* on_methods_request_received */

/* Tests_SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_028: [ On success, `on_methods_request_received` shall return 0. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_016: [ `on_methods_request_received` shall create a BUFFER_HANDLE by calling `BUFFER_new`. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_017: [ `on_methods_request_received` shall call the `IoTHubClient_LL_DeviceMethodComplete` passing the method name, request buffer and size and the newly created BUFFER handle. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_019: [ `on_methods_request_received` shall call `iothubtransportamqp_methods_respond` passing to it the `method_handle` argument, the response bytes, response size and the status code. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_020: [ The response bytes shall be obtained by calling `BUFFER_u_char`. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_021: [ The response size shall be obtained by calling `BUFFER_length`. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_022: [ The status code shall be the return value of the call to `IoTHubClient_LL_DeviceMethodComplete`. ]*/
/* Tests_SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_023: [ After calling `iothubtransportamqp_methods_respond`, the allocated buffer shall be freed by using BUFFER_delete. ]*/
TEST_FUNCTION(on_methods_request_received_responds_to_the_method_request)
{
	// arrange
    static unsigned char test_request_payload[] = { 0x42, 0x43 };
    static unsigned char test_method_response[] = { 0x44, 0x45 };
    int result;

	IOTHUB_CLIENT_CONFIG client_config;
	IOTHUBTRANSPORT_CONFIG config;
	IOTHUB_DEVICE_CONFIG device_config;
	DLIST_ENTRY waitingToSend;
	TRANSPORT_LL_HANDLE handle;
	IOTHUB_DEVICE_HANDLE device_handle;

	client_config.protocol = TEST_get_iothub_client_transport_provider;
	client_config.deviceId = TEST_DEVICE_ID;
	client_config.deviceKey = TEST_DEVICE_KEY;
	client_config.deviceSasToken = TEST_DEVICE_SAS_TOKEN;
	client_config.iotHubName = TEST_IOT_HUB_NAME;
	client_config.iotHubSuffix = TEST_IOT_HUB_SUFFIX;
	client_config.protocolGatewayHostName = TEST_PROT_GW_HOSTNAME;

	config.upperConfig = &client_config;
	config.waitingToSend = TEST_WAIT_TO_SEND_LIST;

	device_config.deviceId = "blah";
	device_config.deviceKey = "cucu";
	device_config.deviceSasToken = NULL;

	handle = IoTHubTransport_AMQP_Common_Create(&config, TEST_amqp_get_io_transport);
	device_handle = IoTHubTransport_AMQP_Common_Register(handle, &device_config, TEST_IOTHUB_CLIENT_LL_HANDLE, &waitingToSend);
	(void)IoTHubTransport_AMQP_Common_Subscribe_DeviceMethod(device_handle);
    (void)IoTHubTransport_AMQP_Common_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);
	umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(BUFFER_new());
    STRICT_EXPECTED_CALL(IoTHubClient_LL_DeviceMethodComplete(TEST_IOTHUB_CLIENT_LL_HANDLE, "test_method", IGNORED_PTR_ARG, sizeof(test_request_payload), TEST_BUFFER_HANDLE))
        .ValidateArgumentBuffer(3, test_request_payload, sizeof(test_request_payload))
        .SetReturn(200);
    STRICT_EXPECTED_CALL(BUFFER_u_char(TEST_BUFFER_HANDLE))
        .SetReturn(test_method_response);
    STRICT_EXPECTED_CALL(BUFFER_length(TEST_BUFFER_HANDLE))
        .SetReturn(sizeof(test_method_response));
    STRICT_EXPECTED_CALL(iothubtransportamqp_methods_respond(TEST_METHOD_HANDLE, IGNORED_PTR_ARG, sizeof(test_method_response), 200))
        .ValidateArgumentBuffer(2, test_method_response, sizeof(test_method_response));
    STRICT_EXPECTED_CALL(BUFFER_delete(TEST_BUFFER_HANDLE));

	// act
    result = g_on_method_request_received(g_on_method_request_received_context, "test_method", test_request_payload, sizeof(test_request_payload), TEST_METHOD_HANDLE);

	// assert
	ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_ARE_EQUAL(int, 0, result);

	// cleanup
	IoTHubTransport_AMQP_Common_Destroy(handle);
}

/* Tests_SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_025: [ If creating the buffer fails, on_methods_request_received shall fail and return a non-zero value. ]*/
TEST_FUNCTION(when_BUFFER_new_fails_on_methods_request_received_fails)
{
    // arrange
    static unsigned char test_request_payload[] = { 0x42, 0x43 };
    static unsigned char test_method_response[] = { 0x44, 0x45 };
    int result;

    IOTHUB_CLIENT_CONFIG client_config;
    IOTHUBTRANSPORT_CONFIG config;
    IOTHUB_DEVICE_CONFIG device_config;
    DLIST_ENTRY waitingToSend;
    TRANSPORT_LL_HANDLE handle;
    IOTHUB_DEVICE_HANDLE device_handle;

    client_config.protocol = TEST_get_iothub_client_transport_provider;
    client_config.deviceId = TEST_DEVICE_ID;
    client_config.deviceKey = TEST_DEVICE_KEY;
    client_config.deviceSasToken = TEST_DEVICE_SAS_TOKEN;
    client_config.iotHubName = TEST_IOT_HUB_NAME;
    client_config.iotHubSuffix = TEST_IOT_HUB_SUFFIX;
    client_config.protocolGatewayHostName = TEST_PROT_GW_HOSTNAME;

    config.upperConfig = &client_config;
    config.waitingToSend = TEST_WAIT_TO_SEND_LIST;

    device_config.deviceId = "blah";
    device_config.deviceKey = "cucu";
    device_config.deviceSasToken = NULL;

    handle = IoTHubTransport_AMQP_Common_Create(&config, TEST_amqp_get_io_transport);
    device_handle = IoTHubTransport_AMQP_Common_Register(handle, &device_config, TEST_IOTHUB_CLIENT_LL_HANDLE, &waitingToSend);
    (void)IoTHubTransport_AMQP_Common_Subscribe_DeviceMethod(device_handle);
    (void)IoTHubTransport_AMQP_Common_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(BUFFER_new())
        .SetReturn(NULL);

    // act
    result = g_on_method_request_received(g_on_method_request_received_context, "test_method", test_request_payload, sizeof(test_request_payload), TEST_METHOD_HANDLE);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_ARE_NOT_EQUAL(int, 0, result);

    // cleanup
    IoTHubTransport_AMQP_Common_Destroy(handle);
}

/* Tests_SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_029: [ If `iothubtransportamqp_methods_respond` fails, `on_methods_request_received` shall return a non-zero value. ]*/
TEST_FUNCTION(when_responding_to_the_method_request_Fails_on_methods_request_received_fails)
{
    // arrange
    static unsigned char test_request_payload[] = { 0x42, 0x43 };
    static unsigned char test_method_response[] = { 0x44, 0x45 };
    int result;

    IOTHUB_CLIENT_CONFIG client_config;
    IOTHUBTRANSPORT_CONFIG config;
    IOTHUB_DEVICE_CONFIG device_config;
    DLIST_ENTRY waitingToSend;
    TRANSPORT_LL_HANDLE handle;
    IOTHUB_DEVICE_HANDLE device_handle;

    client_config.protocol = TEST_get_iothub_client_transport_provider;
    client_config.deviceId = TEST_DEVICE_ID;
    client_config.deviceKey = TEST_DEVICE_KEY;
    client_config.deviceSasToken = TEST_DEVICE_SAS_TOKEN;
    client_config.iotHubName = TEST_IOT_HUB_NAME;
    client_config.iotHubSuffix = TEST_IOT_HUB_SUFFIX;
    client_config.protocolGatewayHostName = TEST_PROT_GW_HOSTNAME;

    config.upperConfig = &client_config;
    config.waitingToSend = TEST_WAIT_TO_SEND_LIST;

    device_config.deviceId = "blah";
    device_config.deviceKey = "cucu";
    device_config.deviceSasToken = NULL;

    handle = IoTHubTransport_AMQP_Common_Create(&config, TEST_amqp_get_io_transport);
    device_handle = IoTHubTransport_AMQP_Common_Register(handle, &device_config, TEST_IOTHUB_CLIENT_LL_HANDLE, &waitingToSend);
    (void)IoTHubTransport_AMQP_Common_Subscribe_DeviceMethod(device_handle);
    (void)IoTHubTransport_AMQP_Common_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(BUFFER_new());
    STRICT_EXPECTED_CALL(IoTHubClient_LL_DeviceMethodComplete(TEST_IOTHUB_CLIENT_LL_HANDLE, "test_method", IGNORED_PTR_ARG, sizeof(test_request_payload), TEST_BUFFER_HANDLE))
        .ValidateArgumentBuffer(3, test_request_payload, sizeof(test_request_payload))
        .SetReturn(200);
    STRICT_EXPECTED_CALL(BUFFER_u_char(TEST_BUFFER_HANDLE))
        .SetReturn(test_method_response);
    STRICT_EXPECTED_CALL(BUFFER_length(TEST_BUFFER_HANDLE))
        .SetReturn(sizeof(test_method_response));
    STRICT_EXPECTED_CALL(iothubtransportamqp_methods_respond(TEST_METHOD_HANDLE, IGNORED_PTR_ARG, sizeof(test_method_response), 200))
        .ValidateArgumentBuffer(2, test_method_response, sizeof(test_method_response))
        .SetReturn(42);
    STRICT_EXPECTED_CALL(BUFFER_delete(TEST_BUFFER_HANDLE));

    // act
    result = g_on_method_request_received(g_on_method_request_received_context, "test_method", test_request_payload, sizeof(test_request_payload), TEST_METHOD_HANDLE);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_ARE_NOT_EQUAL(int, 0, result);

    // cleanup
    IoTHubTransport_AMQP_Common_Destroy(handle);
}

/* on_methods_error */

/* Tests_SRS_IOTHUBTRANSPORT_AMQP_COMMON_01_030: [ `on_methods_error` shall do nothing. ]*/
TEST_FUNCTION(on_methods_error_does_nothing)
{
    // arrange
    IOTHUB_CLIENT_CONFIG client_config;
    IOTHUBTRANSPORT_CONFIG config;
    IOTHUB_DEVICE_CONFIG device_config;
    DLIST_ENTRY waitingToSend;
    TRANSPORT_LL_HANDLE handle;
    IOTHUB_DEVICE_HANDLE device_handle;

    client_config.protocol = TEST_get_iothub_client_transport_provider;
    client_config.deviceId = TEST_DEVICE_ID;
    client_config.deviceKey = TEST_DEVICE_KEY;
    client_config.deviceSasToken = TEST_DEVICE_SAS_TOKEN;
    client_config.iotHubName = TEST_IOT_HUB_NAME;
    client_config.iotHubSuffix = TEST_IOT_HUB_SUFFIX;
    client_config.protocolGatewayHostName = TEST_PROT_GW_HOSTNAME;

    config.upperConfig = &client_config;
    config.waitingToSend = TEST_WAIT_TO_SEND_LIST;

    device_config.deviceId = "blah";
    device_config.deviceKey = "cucu";
    device_config.deviceSasToken = NULL;

    handle = IoTHubTransport_AMQP_Common_Create(&config, TEST_amqp_get_io_transport);
    device_handle = IoTHubTransport_AMQP_Common_Register(handle, &device_config, TEST_IOTHUB_CLIENT_LL_HANDLE, &waitingToSend);
    (void)IoTHubTransport_AMQP_Common_Subscribe_DeviceMethod(device_handle);
    (void)IoTHubTransport_AMQP_Common_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);
    umock_c_reset_all_calls();

    // act
    g_on_methods_error(g_on_methods_error_context);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    IoTHubTransport_AMQP_Common_Destroy(handle);
}

END_TEST_SUITE(iothubtransport_amqp_common_ut)
