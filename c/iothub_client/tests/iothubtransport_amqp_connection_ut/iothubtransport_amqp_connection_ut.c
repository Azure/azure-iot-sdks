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
#include "umock_c.h"
#include "umock_c_negative_tests.h"
#include "umocktypes_charptr.h"
#include "umocktypes_stdint.h"
#include "umocktypes_bool.h"
#include "umocktypes.h"
#include "umocktypes_c.h"
#include <limits.h>

#define ENABLE_MOCKS
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/xio.h"
#include "azure_c_shared_utility/uniqueid.h"
#include "azure_c_shared_utility/strings.h"
#include "azure_uamqp_c/session.h"
#include "azure_uamqp_c/cbs.h"
#include "azure_uamqp_c/sasl_mechanism.h"
#include "azure_uamqp_c/saslclientio.h"
#include "azure_uamqp_c/sasl_mssbcbs.h"
#include "azure_uamqp_c/connection.h"
#undef ENABLE_MOCKS

#include "iothubtransport_amqp_connection.h"

static TEST_MUTEX_HANDLE g_testByTest;
static TEST_MUTEX_HANDLE g_dllByDll;

DEFINE_ENUM_STRINGS(UMOCK_C_ERROR_CODE, UMOCK_C_ERROR_CODE_VALUES)

static void on_umock_c_error(UMOCK_C_ERROR_CODE error_code)
{
    char temp_str[256];
    (void)snprintf(temp_str, sizeof(temp_str), "umock_c reported error :%s", ENUM_TO_STRING(UMOCK_C_ERROR_CODE, error_code));
    ASSERT_FAIL(temp_str);
}

#define DEFAULT_INCOMING_WINDOW_SIZE UINT_MAX
#define DEFAULT_OUTGOING_WINDOW_SIZE 100

#define TEST_STRING                                       "Test string!! $%^%2F0x011"
#define TEST_IOTHUB_HOST_FQDN                             "some.fqdn.com"
#define TEST_UNDERLYING_IO_TRANSPORT                      (XIO_HANDLE)0x4444
#define TEST_SASL_INTERFACE_HANDLE                        (SASL_MECHANISM_INTERFACE_DESCRIPTION*)0x4446
#define TEST_SASLCLIENTIO_INTERFACE_DESCRIPTION_HANDLE    (IO_INTERFACE_DESCRIPTION*)0x4447
#define TEST_STRING_HANDLE                                (STRING_HANDLE)0x4448
#define TEST_SASL_MECHANISM_HANDLE                        (SASL_MECHANISM_HANDLE)0x4449
#define TEST_SASL_IO_HANDLE                               (XIO_HANDLE)0x4450
#define TEST_CHAR_PTR                                     (char*)0x4451
#define TEST_CONNECTION_HANDLE                            (CONNECTION_HANDLE)0x4452
#define TEST_UNIQUE_ID                                    "ab345cd00829ef12"
#define TEST_SESSION_HANDLE                               (SESSION_HANDLE)0x4453
#define TEST_CBS_HANDLE                                   (CBS_HANDLE)0x4454

// Helpers
static int saved_malloc_returns_count = 0;
static void* saved_malloc_returns[20];

static void* TEST_malloc(size_t size)
{
	saved_malloc_returns[saved_malloc_returns_count] = real_malloc(size);

	return saved_malloc_returns[saved_malloc_returns_count++];
}

static void TEST_free(void* ptr)
{
	int i, j;
	for (i = 0, j = 0; j < saved_malloc_returns_count; i++, j++)
	{
		if (saved_malloc_returns[i] == ptr) j++;
		
		saved_malloc_returns[i] = saved_malloc_returns[j];
	}

	if (i != j) saved_malloc_returns_count--;

	real_free(ptr);
}

static ON_AMQP_MANAGEMENT_STATE_CHANGED saved_cbs_create_on_amqp_management_state_changed;
static void* saved_cbs_create_callback_context;
static CBS_HANDLE TEST_cbs_create(SESSION_HANDLE session, ON_AMQP_MANAGEMENT_STATE_CHANGED on_amqp_management_state_changed, void* callback_context)
{
	(void)session;
	saved_cbs_create_on_amqp_management_state_changed = on_amqp_management_state_changed;
	saved_cbs_create_callback_context = callback_context;
	return TEST_CBS_HANDLE;
}

static ON_CONNECTION_STATE_CHANGED connection_create2_on_connection_state_changed;
static void* connection_create2_on_connection_state_changed_context;
static ON_IO_ERROR connection_create2_on_io_error;
static void* connection_create2_on_io_error_context;
static CONNECTION_HANDLE TEST_connection_create2(XIO_HANDLE xio, const char* hostname, const char* container_id, ON_NEW_ENDPOINT on_new_endpoint, void* callback_context, ON_CONNECTION_STATE_CHANGED on_connection_state_changed, void* on_connection_state_changed_context, ON_IO_ERROR on_io_error, void* on_io_error_context)
{
	(void)xio;
	(void)hostname;
	(void)container_id;
	(void)on_new_endpoint;
	(void)callback_context;
	
	connection_create2_on_connection_state_changed = on_connection_state_changed;
	connection_create2_on_connection_state_changed_context = on_connection_state_changed_context;
	connection_create2_on_io_error = on_io_error;
	connection_create2_on_io_error_context = on_io_error_context;

	return TEST_CONNECTION_HANDLE;
}

static const void* on_state_changed_callback_context;
static AMQP_CONNECTION_STATE on_state_changed_callback_previous_state;
static AMQP_CONNECTION_STATE on_state_changed_callback_new_state;
static void on_state_changed_callback(const void* context, AMQP_CONNECTION_STATE previous_state, AMQP_CONNECTION_STATE new_state)
{
	on_state_changed_callback_context = context;
	on_state_changed_callback_previous_state = previous_state;
	on_state_changed_callback_new_state = new_state;
}

static AMQP_CONNECTION_CONFIG global_amqp_connection_config;
static AMQP_CONNECTION_CONFIG* get_amqp_connection_config()
{
	global_amqp_connection_config.iothub_host_fqdn = TEST_IOTHUB_HOST_FQDN;
	global_amqp_connection_config.underlying_io_transport = TEST_UNDERLYING_IO_TRANSPORT;
	global_amqp_connection_config.on_state_changed_callback = on_state_changed_callback;
	global_amqp_connection_config.create_sasl_io = true;
	global_amqp_connection_config.create_cbs_connection = true;
	global_amqp_connection_config.is_trace_on = true;

	return &global_amqp_connection_config;
}

static void set_exp_calls_for_amqp_connection_create(AMQP_CONNECTION_CONFIG* amqp_connection_config)
{
	XIO_HANDLE target_underlying_io;

	STRICT_EXPECTED_CALL(malloc(IGNORED_NUM_ARG)).IgnoreArgument(1);
	STRICT_EXPECTED_CALL(STRING_construct(amqp_connection_config->iothub_host_fqdn));

	// SASL
	if (amqp_connection_config->create_sasl_io || amqp_connection_config->create_cbs_connection)
	{
		STRICT_EXPECTED_CALL(saslmssbcbs_get_interface());
		STRICT_EXPECTED_CALL(saslmechanism_create(TEST_SASL_INTERFACE_HANDLE, NULL));
		STRICT_EXPECTED_CALL(saslclientio_get_interface_description());
		STRICT_EXPECTED_CALL(xio_create(TEST_SASLCLIENTIO_INTERFACE_DESCRIPTION_HANDLE, IGNORED_PTR_ARG))
			.IgnoreArgument_io_create_parameters()
			.SetReturn(TEST_SASL_IO_HANDLE);
		STRICT_EXPECTED_CALL(xio_setoption(TEST_SASL_IO_HANDLE, "logtrace", IGNORED_PTR_ARG))
			.IgnoreArgument_value();
	}

	// Connection
	STRICT_EXPECTED_CALL(UniqueId_Generate(IGNORED_PTR_ARG, 16)).IgnoreArgument(1);
	STRICT_EXPECTED_CALL(STRING_c_str(TEST_STRING_HANDLE)).SetReturn(TEST_IOTHUB_HOST_FQDN);

	if (amqp_connection_config->create_sasl_io || amqp_connection_config->create_cbs_connection)
	{
		target_underlying_io = TEST_SASL_IO_HANDLE;
	}
	else
	{
		target_underlying_io = TEST_UNDERLYING_IO_TRANSPORT;
	}

	STRICT_EXPECTED_CALL(connection_create2(target_underlying_io, TEST_IOTHUB_HOST_FQDN, IGNORED_PTR_ARG, NULL, NULL, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
		.IgnoreArgument_container_id()
		.IgnoreArgument_on_connection_state_changed()
		.IgnoreArgument_on_connection_state_changed_context()
		.IgnoreArgument_on_io_error()
		.IgnoreArgument_on_io_error_context();

	STRICT_EXPECTED_CALL(connection_set_trace(TEST_CONNECTION_HANDLE, amqp_connection_config->is_trace_on));

	// Session
	STRICT_EXPECTED_CALL(session_create(TEST_CONNECTION_HANDLE, NULL, NULL));
	STRICT_EXPECTED_CALL(session_set_incoming_window(TEST_SESSION_HANDLE, (uint32_t)DEFAULT_INCOMING_WINDOW_SIZE));
	STRICT_EXPECTED_CALL(session_set_outgoing_window(TEST_SESSION_HANDLE, (uint32_t)DEFAULT_OUTGOING_WINDOW_SIZE));

	// CBS
	if (amqp_connection_config->create_cbs_connection)
	{
		STRICT_EXPECTED_CALL(cbs_create(TEST_SESSION_HANDLE, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
			.IgnoreArgument(2)
			.IgnoreArgument(3);
		STRICT_EXPECTED_CALL(cbs_open(TEST_CBS_HANDLE));
	}
}

static void set_exp_calls_for_amqp_connection_destroy(AMQP_CONNECTION_CONFIG* config, AMQP_CONNECTION_HANDLE handle)
{
	if (config->create_cbs_connection)
	{
		STRICT_EXPECTED_CALL(cbs_destroy(TEST_CBS_HANDLE));
	}

	STRICT_EXPECTED_CALL(session_destroy(TEST_SESSION_HANDLE));
	STRICT_EXPECTED_CALL(connection_destroy(TEST_CONNECTION_HANDLE));

	if (config->create_sasl_io || config->create_cbs_connection)
	{
		STRICT_EXPECTED_CALL(xio_destroy(TEST_SASL_IO_HANDLE));
		STRICT_EXPECTED_CALL(saslmechanism_destroy(TEST_SASL_MECHANISM_HANDLE));
	}

	STRICT_EXPECTED_CALL(STRING_delete(TEST_STRING_HANDLE));
	STRICT_EXPECTED_CALL(free(handle));
}


BEGIN_TEST_SUITE(iothubtransport_amqp_connection_ut)

TEST_SUITE_INITIALIZE(TestClassInitialize)
{
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

	REGISTER_UMOCK_ALIAS_TYPE(STRING_HANDLE, void*);
	REGISTER_UMOCK_ALIAS_TYPE(XIO_HANDLE, void*);
	REGISTER_UMOCK_ALIAS_TYPE(AMQP_CONNECTION_HANDLE, void*);
	REGISTER_UMOCK_ALIAS_TYPE(SASL_MECHANISM_HANDLE, void*);
	REGISTER_UMOCK_ALIAS_TYPE(SASL_MECHANISM_INTERFACE_DESCRIPTION, void*);
	REGISTER_UMOCK_ALIAS_TYPE(IO_INTERFACE_DESCRIPTION, void*);
	REGISTER_UMOCK_ALIAS_TYPE(ON_NEW_ENDPOINT, void*);
	REGISTER_UMOCK_ALIAS_TYPE(ON_CONNECTION_STATE_CHANGED, void*);
	REGISTER_UMOCK_ALIAS_TYPE(ON_IO_ERROR, void*);
	REGISTER_UMOCK_ALIAS_TYPE(UNIQUEID_RESULT, int);
	REGISTER_UMOCK_ALIAS_TYPE(CONNECTION_HANDLE, void*);
	REGISTER_UMOCK_ALIAS_TYPE(SESSION_HANDLE, void*);
	REGISTER_UMOCK_ALIAS_TYPE(ON_AMQP_MANAGEMENT_STATE_CHANGED, void*);
	REGISTER_UMOCK_ALIAS_TYPE(CBS_HANDLE, void*);
	REGISTER_UMOCK_ALIAS_TYPE(ON_LINK_ATTACHED, void*);
	

	REGISTER_GLOBAL_MOCK_HOOK(gballoc_malloc, TEST_malloc);
	REGISTER_GLOBAL_MOCK_HOOK(malloc, TEST_malloc);
	REGISTER_GLOBAL_MOCK_HOOK(gballoc_free, TEST_free);
	REGISTER_GLOBAL_MOCK_HOOK(free, TEST_free);
	REGISTER_GLOBAL_MOCK_HOOK(cbs_create, TEST_cbs_create);
	REGISTER_GLOBAL_MOCK_HOOK(connection_create2, TEST_connection_create2);


	REGISTER_GLOBAL_MOCK_RETURN(saslmssbcbs_get_interface, TEST_SASL_INTERFACE_HANDLE);
	REGISTER_GLOBAL_MOCK_FAIL_RETURN(saslmssbcbs_get_interface, NULL);
 
	REGISTER_GLOBAL_MOCK_RETURN(saslmechanism_create, TEST_SASL_MECHANISM_HANDLE);
	REGISTER_GLOBAL_MOCK_FAIL_RETURN(saslmechanism_create, NULL);

	REGISTER_GLOBAL_MOCK_RETURN(saslclientio_get_interface_description, TEST_SASLCLIENTIO_INTERFACE_DESCRIPTION_HANDLE);
	REGISTER_GLOBAL_MOCK_FAIL_RETURN(saslclientio_get_interface_description, NULL);

	REGISTER_GLOBAL_MOCK_RETURN(STRING_construct, TEST_STRING_HANDLE);
	REGISTER_GLOBAL_MOCK_FAIL_RETURN(STRING_construct, NULL);

	REGISTER_GLOBAL_MOCK_RETURN(UniqueId_Generate, UNIQUEID_OK);
	REGISTER_GLOBAL_MOCK_FAIL_RETURN(UniqueId_Generate, UNIQUEID_ERROR);

	REGISTER_GLOBAL_MOCK_RETURN(connection_create2, TEST_CONNECTION_HANDLE);
	REGISTER_GLOBAL_MOCK_FAIL_RETURN(connection_create2, NULL);

	REGISTER_GLOBAL_MOCK_RETURN(session_create, TEST_SESSION_HANDLE);
	REGISTER_GLOBAL_MOCK_FAIL_RETURN(session_create, NULL);

	REGISTER_GLOBAL_MOCK_RETURN(session_set_incoming_window, 0);
	REGISTER_GLOBAL_MOCK_FAIL_RETURN(session_set_incoming_window, 1);

	REGISTER_GLOBAL_MOCK_RETURN(session_set_outgoing_window, 0);
	REGISTER_GLOBAL_MOCK_FAIL_RETURN(session_set_outgoing_window, 1);

	REGISTER_GLOBAL_MOCK_RETURN(cbs_create, TEST_CBS_HANDLE);
	REGISTER_GLOBAL_MOCK_FAIL_RETURN(cbs_create, NULL);

	REGISTER_GLOBAL_MOCK_RETURN(cbs_open, 0);
	REGISTER_GLOBAL_MOCK_FAIL_RETURN(cbs_open, 1);

	REGISTER_GLOBAL_MOCK_RETURN(xio_setoption, 0);
	REGISTER_GLOBAL_MOCK_FAIL_RETURN(xio_setoption, 1);

	REGISTER_GLOBAL_MOCK_RETURN(STRING_c_str, TEST_IOTHUB_HOST_FQDN);
	REGISTER_GLOBAL_MOCK_FAIL_RETURN(STRING_c_str, NULL);
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

// Tests_SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_001: [If `config` is NULL, amqp_connection_create() shall fail and return NULL]
TEST_FUNCTION(amqp_connection_create_NULL_config)
{
	// arrange
	umock_c_reset_all_calls();

	// act
	AMQP_CONNECTION_HANDLE handle = amqp_connection_create(NULL);

	// assert
	ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
	ASSERT_ARE_EQUAL(void_ptr, (void*)handle, NULL);

	// cleanup
}

// Tests_SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_002: [If `config->iothub_host_fqdn` is NULL, amqp_connection_create() shall fail and return NULL]
TEST_FUNCTION(amqp_connection_create_NULL_iothub_fqdn)
{
	// arrange
	AMQP_CONNECTION_CONFIG* config = get_amqp_connection_config();
	config->iothub_host_fqdn = NULL;

	umock_c_reset_all_calls();

	// act
	AMQP_CONNECTION_HANDLE handle = amqp_connection_create(config);

	// assert
	ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
	ASSERT_ARE_EQUAL(void_ptr, (void*)handle, NULL);

	// cleanup
}

// Tests_SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_003: [If `config->underlying_io_transport` is NULL, amqp_connection_create() shall fail and return NULL]
TEST_FUNCTION(amqp_connection_create_NULL_underlying_io_transport)
{
	// arrange
	AMQP_CONNECTION_CONFIG* config = get_amqp_connection_config();
	config->underlying_io_transport = NULL;

	// act
	AMQP_CONNECTION_HANDLE handle = amqp_connection_create(config);

	// assert
	ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
	ASSERT_ARE_EQUAL(void_ptr, (void*)handle, NULL);

	// cleanup
}

// Tests_SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_057: [amqp_connection_create() shall allocate memory for an instance of the connection instance]
// Tests_SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_005: [A copy of `config->iothub_host_fqdn` shall be saved on `instance->iothub_host_fqdn`]
// Tests_SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_006: [`config->underlying_io_transport` shall be saved on `instance->underlying_io_transport`]
// Tests_SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_008: [`config->is_trace_on` shall be saved on `instance->is_trace_on`]
// Tests_SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_060: [`config->on_state_changed_callback` shall be saved on `instance->on_state_changed_callback`]
// Tests_SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_061: [`config->on_state_changed_context` shall be saved on `instance->on_state_changed_context`]
// Tests_SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_009: [`config->on_error_callback` shall be saved on `instance->on_error_callback`]
// Tests_SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_010: [`config->on_error_context` shall be saved on `instance->on_error_context`]

// Tests_SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_011: [If `config->create_sasl_io` is true or `config->create_cbs_connection` is true, amqp_connection_create() shall create SASL I/O]
// Tests_SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_012: [`instance->sasl_mechanism` shall be created using saslmechanism_create()]
// Tests_SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_014: [A SASLCLIENTIO_CONFIG shall be set with `instance->underlying_io_transport` and `instance->sasl_mechanism`]
// Tests_SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_015: [`instance->sasl_io` shall be created using xio_create() passing saslclientio_get_interface_description() and the SASLCLIENTIO_CONFIG instance]
// Tests_SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_017: [The sasl_io "logtrace" option shall be set using xio_setoption(), passing `instance->is_trace_on`]

// Tests_SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_007: [If `instance->sasl_io` is defined it shall be used as parameter `xio` in connection_create2()]
// Tests_SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_019: [amqp_connection_create() shall use the `instance->sasl_io` as the `connection_underlying_io`]
// Tests_SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_020: [connection_create2() shall also receive `on_connection_state_changed` and `on_connection_error` callback functions]
// Tests_SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_023: [The connection tracing shall be set using connection_set_trace(), passing `instance->is_trace_on`]

// Tests_SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_024: [`instance->session_handle` shall be created using session_create(), passing `instance->connection_handle`]
// Tests_SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_026: [The `instance->session_handle` incoming window size shall be set as UINT_MAX using session_set_incoming_window()]
// Tests_SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_027: [The `instance->session_handle` outgoing window size shall be set as 100 using session_set_outgoing_window()]

// Tests_SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_028: [Only if `config->create_cbs_connection` is true, amqp_connection_create() shall create and open the CBS_HANDLE]
// Tests_SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_029: [`instance->cbs_handle` shall be created using cbs_create(), passing `instance->session_handle` and `on_cbs_state_changed` callback]
// Tests_SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_031: [`instance->cbs_handle` shall be opened using cbs_open()]

// Tests_SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_034: [If no failures occur, amqp_connection_create() shall return the handle to the connection state]
TEST_FUNCTION(amqp_connection_create_SASL_and_CBS_success)
{
    // arrange
	AMQP_CONNECTION_CONFIG* config = get_amqp_connection_config();

	umock_c_reset_all_calls();
	set_exp_calls_for_amqp_connection_create(config);

    // act
	AMQP_CONNECTION_HANDLE handle = amqp_connection_create(config);

    // assert
	ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
	ASSERT_ARE_EQUAL(void_ptr, (void*)handle, (void*)saved_malloc_returns[0]);

    // cleanup
	amqp_connection_destroy(handle);
}

// Tests_SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_028: [Only if `config->create_cbs_connection` is true, amqp_connection_create() shall create and open the CBS_HANDLE]
TEST_FUNCTION(amqp_connection_create_SASL_only_success)
{
	// arrange
	AMQP_CONNECTION_CONFIG* config = get_amqp_connection_config();
	config->create_cbs_connection = false;

	umock_c_reset_all_calls();
	set_exp_calls_for_amqp_connection_create(config);

	// act
	AMQP_CONNECTION_HANDLE handle = amqp_connection_create(config);

	// assert
	ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
	ASSERT_ARE_EQUAL(void_ptr, (void*)handle, (void*)saved_malloc_returns[0]);

	// cleanup
	amqp_connection_destroy(handle);
}

// Tests_SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_018: [If `instance->sasl_io` is not defined, `instance->underlying_io_transport` shall be used as parameter `xio` in connection_create2()]
TEST_FUNCTION(amqp_connection_create_no_SASL_no_CBS_success)
{
	// arrange
	AMQP_CONNECTION_CONFIG* config = get_amqp_connection_config();
	config->create_sasl_io = false;
	config->create_cbs_connection = false;

	umock_c_reset_all_calls();
	set_exp_calls_for_amqp_connection_create(config);

	// act
	AMQP_CONNECTION_HANDLE handle = amqp_connection_create(config);

	// assert
	ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
	ASSERT_ARE_EQUAL(void_ptr, (void*)handle, (void*)saved_malloc_returns[0]);

	// cleanup
	amqp_connection_destroy(handle);
}

// Tests_SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_058: [If malloc() fails, amqp_connection_create() shall fail and return NULL]
// Tests_SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_066: [If STRING_construct() fails, amqp_connection_create() shall fail and return NULL]
// Tests_SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_013: [If saslmechanism_create() fails, amqp_connection_create() shall fail and return NULL]
// Tests_SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_016: [If xio_create() fails, amqp_connection_create() shall fail and return NULL]
// Tests_SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_021: [If connection_create2() fails, amqp_connection_create() shall fail and return NULL]
// Tests_SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_025: [If session_create() fails, amqp_connection_create() shall fail and return NULL]
// Tests_SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_030: [If cbs_create() fails, amqp_connection_create() shall fail and return NULL]
// Tests_SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_032: [If cbs_open() fails, amqp_connection_create() shall fail and return NULL]
TEST_FUNCTION(amqp_connection_create_SASL_and_CBS_negative_checks)
{
	// arrange
	ASSERT_ARE_EQUAL(int, 0, umock_c_negative_tests_init());

	AMQP_CONNECTION_CONFIG* config = get_amqp_connection_config();

	umock_c_reset_all_calls();
	set_exp_calls_for_amqp_connection_create(config);
	umock_c_negative_tests_snapshot();

	// act
	for (size_t i = 0; i < umock_c_negative_tests_call_count(); i++)
	{
		// arrange
		char error_msg[64];

		umock_c_negative_tests_reset();
		umock_c_negative_tests_fail_call(i);

		if (i == 8 || i == 10 || i == 12 || i == 13)
		{
			continue; // these lines have functions that do not return anything (void) or do not cause failures.
		}

		AMQP_CONNECTION_HANDLE handle = amqp_connection_create(config);

		// cleanup
		amqp_connection_destroy(handle);

		// assert
		sprintf(error_msg, "On failed call %zu", i);
		ASSERT_IS_NULL_WITH_MSG(handle, error_msg);
	}

	// cleanup
	umock_c_negative_tests_reset();
	umock_c_negative_tests_deinit();
}

// Tests_SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_068: [If `on_cbs_state_changed` is called back, `instance->on_state_changed_callback` shall be invoked, if defined, only if the new state is different than the previous]
// Tests_SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_069: [If `on_cbs_state_changed` new state is AMQP_MANAGEMENT_STATE_OPEN, `instance->on_state_changed_callback` shall be invoked with state AMQP_CONNECTION_STATE_OPENED]
// Tests_SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_070: [If `on_cbs_state_changed` new state is AMQP_MANAGEMENT_STATE_IDLE, `instance->on_state_changed_callback` shall be invoked with state AMQP_CONNECTION_STATE_CLOSED]
// Tests_SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_072: [If `on_cbs_state_changed` new state is AMQP_MANAGEMENT_STATE_ERROR, `instance->on_state_changed_callback` shall be invoked with state AMQP_CONNECTION_STATE_ERROR]
TEST_FUNCTION(amqp_connection_create_on_cbs_state_changed)
{
	// arrange
	AMQP_MANAGEMENT_STATE previous_cbs_state[4];
	previous_cbs_state[0] = AMQP_MANAGEMENT_STATE_IDLE;
	previous_cbs_state[1] = AMQP_MANAGEMENT_STATE_OPEN;
	previous_cbs_state[2] = AMQP_MANAGEMENT_STATE_ERROR;
	previous_cbs_state[3] = AMQP_MANAGEMENT_STATE_IDLE;

	AMQP_MANAGEMENT_STATE new_cbs_state[4];
	new_cbs_state[0] = AMQP_MANAGEMENT_STATE_OPEN;
	new_cbs_state[1] = AMQP_MANAGEMENT_STATE_ERROR;
	new_cbs_state[2] = AMQP_MANAGEMENT_STATE_IDLE;
	new_cbs_state[3] = AMQP_MANAGEMENT_STATE_ERROR;

	AMQP_CONNECTION_STATE previous_amqp_connection_state[4];
	previous_amqp_connection_state[0] = AMQP_CONNECTION_STATE_CLOSED;
	previous_amqp_connection_state[1] = AMQP_CONNECTION_STATE_OPENED;
	previous_amqp_connection_state[2] = AMQP_CONNECTION_STATE_ERROR;
	previous_amqp_connection_state[3] = AMQP_CONNECTION_STATE_CLOSED;

	AMQP_CONNECTION_STATE new_amqp_connection_state[4];
	new_amqp_connection_state[0] = AMQP_CONNECTION_STATE_OPENED;
	new_amqp_connection_state[1] = AMQP_CONNECTION_STATE_ERROR;
	new_amqp_connection_state[2] = AMQP_CONNECTION_STATE_CLOSED;
	new_amqp_connection_state[3] = AMQP_CONNECTION_STATE_ERROR;


	AMQP_CONNECTION_CONFIG* config = get_amqp_connection_config();

	umock_c_reset_all_calls();
	set_exp_calls_for_amqp_connection_create(config);

	saved_cbs_create_on_amqp_management_state_changed = NULL;
	saved_cbs_create_callback_context = NULL;

	AMQP_CONNECTION_HANDLE handle = amqp_connection_create(config);

	ASSERT_IS_NOT_NULL(saved_cbs_create_on_amqp_management_state_changed);

	// act
	int i;
	for (i = 0; i < 4; i++)
	{
		// act
		saved_cbs_create_on_amqp_management_state_changed(handle, new_cbs_state[i], previous_cbs_state[i]);

		// assert
		ASSERT_ARE_EQUAL(int, on_state_changed_callback_previous_state, previous_amqp_connection_state[i]);
		ASSERT_ARE_EQUAL(int, on_state_changed_callback_new_state, new_amqp_connection_state[i]);
	}

	// cleanup
	amqp_connection_destroy(handle);
}

// Tests_SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_063: [If `on_connection_state_changed` is called back, `instance->on_state_changed_callback` shall be invoked, if defined, only if the new state is different than the previous]
// Tests_SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_064: [If `on_connection_state_changed` new state is CONNECTION_STATE_OPENED, `instance->on_state_changed_callback` shall be invoked with state AMQP_CONNECTION_STATE_OPENED]
// Tests_SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_065: [If `on_connection_state_changed` new state is CONNECTION_STATE_END or CONNECTION_STATE_ERROR, `instance->on_state_changed_callback` shall be invoked with state AMQP_CONNECTION_STATE_CLOSED]
// Tests_SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_071: [If `on_connection_state_changed` new state is CONNECTION_STATE_ERROR, `instance->on_state_changed_callback` shall be invoked with state AMQP_CONNECTION_STATE_ERROR]
TEST_FUNCTION(amqp_connection_create_on_connection_state_changed)
{
	// arrange
	CONNECTION_STATE previous_connection_state[4];
	previous_connection_state[0] = CONNECTION_STATE_END;
	previous_connection_state[1] = CONNECTION_STATE_OPENED;
	previous_connection_state[2] = CONNECTION_STATE_ERROR;
	previous_connection_state[3] = CONNECTION_STATE_END;

	CONNECTION_STATE new_connection_state[4];
	new_connection_state[0] = CONNECTION_STATE_OPENED;
	new_connection_state[1] = CONNECTION_STATE_ERROR;
	new_connection_state[2] = CONNECTION_STATE_END;
	new_connection_state[3] = CONNECTION_STATE_ERROR;

	AMQP_CONNECTION_STATE previous_amqp_connection_state[4];
	previous_amqp_connection_state[0] = AMQP_CONNECTION_STATE_CLOSED;
	previous_amqp_connection_state[1] = AMQP_CONNECTION_STATE_OPENED;
	previous_amqp_connection_state[2] = AMQP_CONNECTION_STATE_ERROR;
	previous_amqp_connection_state[3] = AMQP_CONNECTION_STATE_CLOSED;

	AMQP_CONNECTION_STATE new_amqp_connection_state[4];
	new_amqp_connection_state[0] = AMQP_CONNECTION_STATE_OPENED;
	new_amqp_connection_state[1] = AMQP_CONNECTION_STATE_ERROR;
	new_amqp_connection_state[2] = AMQP_CONNECTION_STATE_CLOSED;
	new_amqp_connection_state[3] = AMQP_CONNECTION_STATE_ERROR;


	AMQP_CONNECTION_CONFIG* config = get_amqp_connection_config();

	umock_c_reset_all_calls();
	set_exp_calls_for_amqp_connection_create(config);

	connection_create2_on_connection_state_changed = NULL;
	connection_create2_on_connection_state_changed_context = NULL;

	AMQP_CONNECTION_HANDLE handle = amqp_connection_create(config);

	ASSERT_IS_NOT_NULL(connection_create2_on_connection_state_changed);

	// act
	int i;
	for (i = 0; i < 4; i++)
	{
		// act
		connection_create2_on_connection_state_changed(handle, new_connection_state[i], previous_connection_state[i]);

		// assert
		ASSERT_ARE_EQUAL(int, on_state_changed_callback_previous_state, previous_amqp_connection_state[i]);
		ASSERT_ARE_EQUAL(int, on_state_changed_callback_new_state, new_amqp_connection_state[i]);
	}

	// cleanup
	amqp_connection_destroy(handle);
}

// Tests_SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_022: [If the connection calls back with an I/O error, `instance->on_state_changed_callback` shall be invoked if set passing code AMQP_CONNECTION_STATE_ERROR and `instance->on_state_changed_context`]
TEST_FUNCTION(amqp_connection_create_on_io_error)
{
	// arrange
	AMQP_CONNECTION_CONFIG* config = get_amqp_connection_config();

	umock_c_reset_all_calls();
	set_exp_calls_for_amqp_connection_create(config);

	connection_create2_on_connection_state_changed = NULL;
	connection_create2_on_connection_state_changed_context = NULL;
	on_state_changed_callback_previous_state = AMQP_CONNECTION_STATE_CLOSED;
	on_state_changed_callback_new_state = AMQP_CONNECTION_STATE_CLOSED;

	AMQP_CONNECTION_HANDLE handle = amqp_connection_create(config);

	ASSERT_IS_NOT_NULL(connection_create2_on_io_error);

	// act
	connection_create2_on_io_error(handle);

	// assert
	ASSERT_ARE_EQUAL(int, on_state_changed_callback_previous_state, AMQP_CONNECTION_STATE_CLOSED);
	ASSERT_ARE_EQUAL(int, on_state_changed_callback_new_state, AMQP_CONNECTION_STATE_ERROR);

	// cleanup
	amqp_connection_destroy(handle);
}

// Tests_SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_035: [If `conn_handle` is NULL, amqp_connection_destroy() shall fail and return]
TEST_FUNCTION(amqp_connection_destroy_NULL_handle)
{
	// arrange
	umock_c_reset_all_calls();

	// act
	amqp_connection_destroy(NULL);

	// assert
	ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

	// cleanup
}

// Tests_SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_036: [amqp_connection_destroy() shall destroy `instance->cbs_handle` if set using cbs_destroy()]
// Tests_SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_037: [amqp_connection_destroy() shall destroy `instance->session_handle` if set using session_destroy()]
// Tests_SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_067: [amqp_connection_destroy() shall destroy `instance->connection_handle` if set using connection_destroy()]
// Tests_SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_038: [amqp_connection_destroy() shall destroy `instance->sasl_io` if set using xio_destroy()]
// Tests_SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_039: [amqp_connection_destroy() shall destroy `instance->sasl_mechanism` if set using saslmechanism_destroy()]
// Tests_SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_059: [amqp_connection_destroy() shall destroy `instance->iothub_host_fqdn` if set using STRING_delete()]
// Tests_SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_040: [amqp_connection_destroy() shall free the memory allocated for the connection instance]
TEST_FUNCTION(amqp_connection_destroy_SASL_and_CBS_success)
{
	// arrange
	AMQP_CONNECTION_CONFIG* config = get_amqp_connection_config();

	umock_c_reset_all_calls();
	set_exp_calls_for_amqp_connection_create(config);
	
	AMQP_CONNECTION_HANDLE handle = amqp_connection_create(config);

	umock_c_reset_all_calls();
	set_exp_calls_for_amqp_connection_destroy(config, handle);

	// act
	amqp_connection_destroy(handle);

	// assert
	ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

	// cleanup
}

// Tests_SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_037: [amqp_connection_destroy() shall destroy `instance->session_handle` if set using session_destroy()]
// Tests_SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_067: [amqp_connection_destroy() shall destroy `instance->connection_handle` if set using connection_destroy()]
// Tests_SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_059: [amqp_connection_destroy() shall destroy `instance->iothub_host_fqdn` if set using STRING_delete()]
// Tests_SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_040: [amqp_connection_destroy() shall free the memory allocated for the connection instance]
TEST_FUNCTION(amqp_connection_destroy_no_SASL_no_CBS_success)
{
	// arrange
	AMQP_CONNECTION_CONFIG* config = get_amqp_connection_config();
	config->create_sasl_io = false;
	config->create_cbs_connection = false;

	umock_c_reset_all_calls();
	set_exp_calls_for_amqp_connection_create(config);

	AMQP_CONNECTION_HANDLE handle = amqp_connection_create(config);

	umock_c_reset_all_calls();
	set_exp_calls_for_amqp_connection_destroy(config, handle);

	// act
	amqp_connection_destroy(handle);

	// assert
	ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

	// cleanup
}

// Tests_SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_041: [If `conn_handle` is NULL, amqp_connection_do_work() shall fail and return]
TEST_FUNCTION(amqp_connection_do_work_NULL_handle)
{
	// arrange
	umock_c_reset_all_calls();

	// act
	amqp_connection_do_work(NULL);

	// assert
	ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

	// cleanup
}

// Tests_SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_042: [connection_dowork() shall be invoked passing `instance->connection_handle`]
TEST_FUNCTION(amqp_connection_do_work_success)
{
	// arrange
	AMQP_CONNECTION_CONFIG* config = get_amqp_connection_config();

	umock_c_reset_all_calls();
	set_exp_calls_for_amqp_connection_create(config);

	AMQP_CONNECTION_HANDLE handle = amqp_connection_create(config);

	umock_c_reset_all_calls();
	STRICT_EXPECTED_CALL(connection_dowork(TEST_CONNECTION_HANDLE));

	// act
	amqp_connection_do_work(handle);

	// assert
	ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

	// cleanup
	amqp_connection_destroy(handle);
}

// Tests_SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_043: [If `conn_handle` is NULL, amqp_connection_get_session_handle() shall fail and return __LINE__]
TEST_FUNCTION(amqp_connection_get_session_handle_NULL_handle)
{
	// arrange
	SESSION_HANDLE session_handle;

	// act
	int result = amqp_connection_get_session_handle(NULL, &session_handle);

	// assert
	ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
	ASSERT_ARE_NOT_EQUAL(int, result, 0);

	// cleanup
}

// Tests_SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_044: [If `session_handle` is NULL, amqp_connection_get_session_handle() shall fail and return __LINE__]
TEST_FUNCTION(amqp_connection_get_session_handle_NULL_session_handle)
{
	// arrange
	AMQP_CONNECTION_CONFIG* config = get_amqp_connection_config();

	umock_c_reset_all_calls();
	set_exp_calls_for_amqp_connection_create(config);

	AMQP_CONNECTION_HANDLE handle = amqp_connection_create(config);

	umock_c_reset_all_calls();

	// act
	int result = amqp_connection_get_session_handle(handle, NULL);

	// assert
	ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
	ASSERT_ARE_NOT_EQUAL(int, result, 0);

	// cleanup
	amqp_connection_destroy(handle);
}

// Tests_SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_045: [`session_handle` shall be set to point to `instance->session_handle`]
// Tests_SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_046: [amqp_connection_get_session_handle() shall return success code 0]
TEST_FUNCTION(amqp_connection_get_session_handle_success)
{
	// arrange
	AMQP_CONNECTION_CONFIG* config = get_amqp_connection_config();

	umock_c_reset_all_calls();
	set_exp_calls_for_amqp_connection_create(config);

	AMQP_CONNECTION_HANDLE handle = amqp_connection_create(config);

	umock_c_reset_all_calls();

	SESSION_HANDLE session_handle;

	// act
	int result = amqp_connection_get_session_handle(handle, &session_handle);

	// assert
	ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
	ASSERT_ARE_EQUAL(int, result, 0);
	ASSERT_ARE_EQUAL(void_ptr, session_handle, TEST_SESSION_HANDLE);

	// cleanup
	amqp_connection_destroy(handle);
}

// Tests_SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_047: [If `conn_handle` is NULL, amqp_connection_get_cbs_handle() shall fail and return __LINE__]
TEST_FUNCTION(amqp_connection_get_cbs_handle_NULL_handle)
{
	// arrange
	CBS_HANDLE cbs_handle;

	// act
	int result = amqp_connection_get_cbs_handle(NULL, &cbs_handle);

	// assert
	ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
	ASSERT_ARE_NOT_EQUAL(int, result, 0);

	// cleanup
}

// Tests_SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_048: [If `cbs_handle` is NULL, amqp_connection_get_cbs_handle() shall fail and return __LINE__]
TEST_FUNCTION(amqp_connection_get_cbs_handle_NULL_cbs_handle)
{
	// arrange
	AMQP_CONNECTION_CONFIG* config = get_amqp_connection_config();

	umock_c_reset_all_calls();
	set_exp_calls_for_amqp_connection_create(config);

	AMQP_CONNECTION_HANDLE handle = amqp_connection_create(config);

	umock_c_reset_all_calls();

	// act
	int result = amqp_connection_get_cbs_handle(handle, NULL);

	// assert
	ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
	ASSERT_ARE_NOT_EQUAL(int, result, 0);

	// cleanup
	amqp_connection_destroy(handle);
}

// Tests_SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_050: [`cbs_handle` shall be set to point to `instance->cbs_handle`]
// Tests_SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_051: [amqp_connection_get_cbs_handle() shall return success code 0]
TEST_FUNCTION(amqp_connection_get_cbs_handle_success)
{
	// arrange
	AMQP_CONNECTION_CONFIG* config = get_amqp_connection_config();

	umock_c_reset_all_calls();
	set_exp_calls_for_amqp_connection_create(config);

	AMQP_CONNECTION_HANDLE handle = amqp_connection_create(config);

	umock_c_reset_all_calls();

	CBS_HANDLE cbs_handle;

	// act
	int result = amqp_connection_get_cbs_handle(handle, &cbs_handle);

	// assert
	ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
	ASSERT_ARE_EQUAL(int, result, 0);
	ASSERT_ARE_EQUAL(void_ptr, cbs_handle, TEST_CBS_HANDLE);

	// cleanup
	amqp_connection_destroy(handle);
}

// Tests_SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_049: [If `instance->cbs_handle` is NULL, amqp_connection_get_cbs_handle() shall fail and return __LINE__]
TEST_FUNCTION(amqp_connection_get_cbs_handle_no_CBS)
{
	// arrange
	AMQP_CONNECTION_CONFIG* config = get_amqp_connection_config();
	config->create_sasl_io = false;
	config->create_cbs_connection = false;

	umock_c_reset_all_calls();
	set_exp_calls_for_amqp_connection_create(config);

	AMQP_CONNECTION_HANDLE handle = amqp_connection_create(config);

	umock_c_reset_all_calls();

	CBS_HANDLE cbs_handle = NULL;

	// act
	int result = amqp_connection_get_cbs_handle(handle, &cbs_handle);

	// assert
	ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
	ASSERT_ARE_NOT_EQUAL(int, result, 0);
	ASSERT_ARE_EQUAL(void_ptr, cbs_handle, NULL);

	// cleanup
	amqp_connection_destroy(handle);
}

// Tests_SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_052: [If `conn_handle` is NULL, amqp_connection_set_logging() shall fail and return __LINE__]
TEST_FUNCTION(amqp_connection_set_logging_NULL_handle)
{
	// act
	int result = amqp_connection_set_logging(NULL, true);

	// assert
	ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
	ASSERT_ARE_NOT_EQUAL(int, result, 0);

	// cleanup
}

// Tests_SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_053: [`instance->is_trace_on` shall be set to `is_trace_on`]
// Tests_SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_054: [Tracing on `instance->sasl_io` shall be set to `instance->is_trace_on` if the value has changed]
// Tests_SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_055: [Tracing on `instance->connection_handle` shall be set to `instance->is_trace_on` if the value has changed]
// Tests_SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_056: [amqp_connection_set_logging() shall return success code 0]
TEST_FUNCTION(amqp_connection_set_logging_SASL_and_CBS_success)
{
	// arrange
	AMQP_CONNECTION_CONFIG* config = get_amqp_connection_config();

	umock_c_reset_all_calls();
	set_exp_calls_for_amqp_connection_create(config);

	AMQP_CONNECTION_HANDLE handle = amqp_connection_create(config);

	umock_c_reset_all_calls();
	STRICT_EXPECTED_CALL(xio_setoption(TEST_SASL_IO_HANDLE, "logtrace", IGNORED_PTR_ARG)).IgnoreArgument(3);
	STRICT_EXPECTED_CALL(connection_set_trace(TEST_CONNECTION_HANDLE, true));

	// act
	int result = amqp_connection_set_logging(handle, true);

	// assert
	ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
	ASSERT_ARE_EQUAL(int, result, 0);

	// cleanup
	amqp_connection_destroy(handle);
}

// Tests_SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_072: [If xio_setoption() fails, amqp_connection_set_logging() shall fail and return __LINE__]
TEST_FUNCTION(amqp_connection_set_logging_SASL_and_CBS_xio_setoption_fails)
{
	// arrange
	AMQP_CONNECTION_CONFIG* config = get_amqp_connection_config();

	umock_c_reset_all_calls();
	set_exp_calls_for_amqp_connection_create(config);

	AMQP_CONNECTION_HANDLE handle = amqp_connection_create(config);

	umock_c_reset_all_calls();
	STRICT_EXPECTED_CALL(xio_setoption(TEST_SASL_IO_HANDLE, "logtrace", IGNORED_PTR_ARG))
		.IgnoreArgument(3)
		.SetReturn(1);

	// act
	int result = amqp_connection_set_logging(handle, true);

	// assert
	ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
	ASSERT_ARE_NOT_EQUAL(int, result, 0);

	// cleanup
	amqp_connection_destroy(handle);
}

// Tests_SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_053: [`instance->is_trace_on` shall be set to `is_trace_on`]
// Tests_SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_055: [Tracing on `instance->connection_handle` shall be set to `instance->is_trace_on` if the value has changed]
// Tests_SRS_IOTHUBTRANSPORT_AMQP_CONNECTION_09_056: [amqp_connection_set_logging() shall return success code 0]
TEST_FUNCTION(amqp_connection_set_logging_no_SASL_no_CBS_success)
{
	// arrange
	AMQP_CONNECTION_CONFIG* config = get_amqp_connection_config();
	config->create_sasl_io = false;
	config->create_cbs_connection = false;

	umock_c_reset_all_calls();
	set_exp_calls_for_amqp_connection_create(config);

	AMQP_CONNECTION_HANDLE handle = amqp_connection_create(config);

	umock_c_reset_all_calls();
	STRICT_EXPECTED_CALL(connection_set_trace(TEST_CONNECTION_HANDLE, true));

	// act
	int result = amqp_connection_set_logging(handle, true);

	// assert
	ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
	ASSERT_ARE_EQUAL(int, result, 0);

	// cleanup
	amqp_connection_destroy(handle);
}

END_TEST_SUITE(iothubtransport_amqp_connection_ut)
