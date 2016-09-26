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
#include "umock_c_negative_tests.h"
#include "umocktypes.h"
#include "umocktypes_c.h"

#define ENABLE_MOCKS
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/doublylinkedlist.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/platform.h"
#include "azure_c_shared_utility/strings.h"
#include "azure_c_shared_utility/urlencode.h"
#include "azure_c_shared_utility/tlsio.h"
#include "azure_c_shared_utility/vector.h"

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
#include "iothubtransportamqp_common.h"
#include "iothubtransportamqp_auth.h"
#include "iothub_client_version.h"
#undef ENABLE_MOCKS

#include "iothubtransportamqp.h"

static TEST_MUTEX_HANDLE g_testByTest;
static TEST_MUTEX_HANDLE g_dllByDll;

DEFINE_ENUM_STRINGS(UMOCK_C_ERROR_CODE, UMOCK_C_ERROR_CODE_VALUES)

static void on_umock_c_error(UMOCK_C_ERROR_CODE error_code)
{
    char temp_str[256];
    (void)snprintf(temp_str, sizeof(temp_str), "umock_c reported error :%s", ENUM_TO_STRING(UMOCK_C_ERROR_CODE, error_code));
    ASSERT_FAIL(temp_str);
}

#define TEST_STRING_HANDLE (STRING_HANDLE)0x101

MOCKABLE_FUNCTION(, IOTHUBMESSAGE_DISPOSITION_RESULT, IoTHubClient_LL_MessageCallback, IOTHUB_CLIENT_LL_HANDLE, handle, IOTHUB_MESSAGE_HANDLE, message);
MOCKABLE_FUNCTION(, const IO_INTERFACE_DESCRIPTION*, saslclientio_get_interface_description);

static STRING_HANDLE TEST_STRING_construct_sprintf(const char* format, ...)
{
	(void)format;
	return NULL;
}

BEGIN_TEST_SUITE(iothubtransportamqp_ut)

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
/*
	REGISTER_GLOBAL_MOCK_HOOK(STRING_construct_sprintf, TEST_STRING_construct_sprintf);
	REGISTER_GLOBAL_MOCK_RETURN(STRING_construct_sprintf, TEST_STRING_HANDLE);
	REGISTER_GLOBAL_MOCK_FAIL_RETURN(STRING_construct_sprintf, NULL);*/
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

TEST_FUNCTION(message_create_from_iothub_message_bytearray_success)
{
    // arrange
	umock_c_reset_all_calls();
	//set_exp_calls_for_message_create_from_iothub_message(1, IOTHUBMESSAGE_BYTEARRAY, true, true, true);

    // act
	

    // assert
	/*ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
	ASSERT_ARE_EQUAL(int, result, 0);
	ASSERT_ARE_EQUAL(void_ptr, (void*)uamqp_message, (void*)TEST_MESSAGE_HANDLE);
*/
    // cleanup
}


END_TEST_SUITE(iothubtransportamqp_ut)
