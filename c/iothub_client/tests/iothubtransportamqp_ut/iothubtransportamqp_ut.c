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
#include "iothubtransportamqp_auth.h"
#include "iothub_client_version.h"
#undef ENABLE_MOCKS

#include "iothubtransportamqp.h"

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

#define TEST_STRING_HANDLE (STRING_HANDLE)0x101
#define TEST_WAIT_TO_SEND_LIST ((PDLIST_ENTRY)0x201)

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

// Tests_IOTHUBTRANSPORTAMQP_09_005: [If parameter config or config->upperConfig are NULL then IoTHubTransportAMQP_Create shall fail and return NULL.]
TEST_FUNCTION(AMQP_Create_NULL_config)
{
    // arrange
	umock_c_reset_all_calls();
	TRANSPORT_PROVIDER* provider = (TRANSPORT_PROVIDER*)AMQP_Protocol();

    // act
	TRANSPORT_LL_HANDLE tr_hdl = provider->IoTHubTransport_Create(NULL);

    // assert
	ASSERT_IS_NULL_WITH_MSG(tr_hdl, "Expected NULL transport handle");

	// cleanup
}

// Tests_IOTHUBTRANSPORTAMQP_09_005: [If parameter config or config->upperConfig are NULL then IoTHubTransportAMQP_Create shall fail and return NULL.]
TEST_FUNCTION(AMQP_Create_NULL_upperConfig)
{
	// arrange
	TRANSPORT_PROVIDER* provider = (TRANSPORT_PROVIDER*)AMQP_Protocol();
	IOTHUBTRANSPORT_CONFIG config;
	config.upperConfig = NULL;

	umock_c_reset_all_calls();

	// act
	TRANSPORT_LL_HANDLE tr_hdl = provider->IoTHubTransport_Create(&config);

	// assert
	ASSERT_IS_NULL(tr_hdl);
}

// Tests_IOTHUBTRANSPORTAMQP_09_006: [IoTHubTransportAMQP_Create shall fail and return NULL if any fields of the config->upperConfig structure are NULL.]
TEST_FUNCTION(AMQP_Create_upperConfig_protocol_NULL)
{
	// arrange
	TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();
	IOTHUBTRANSPORT_CONFIG* config = create_transport_config(NULL);

	umock_c_reset_all_calls();

	// act
	TRANSPORT_LL_HANDLE handle = transport_interface->IoTHubTransport_Create(config);

	// assert
	ASSERT_IS_NULL(handle);
}

// Tests_IOTHUBTRANSPORTAMQP_09_006: [IoTHubTransportAMQP_Create shall fail and return NULL if any fields of the config->upperConfig structure are NULL.]
TEST_FUNCTION(AMQP_Create_upperConfig_iotHubName_NULL)
{
	// arrange
	TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();

	IOTHUB_CLIENT_CONFIG client_config;
	client_config.protocol = AMQP_Protocol;
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
	TRANSPORT_LL_HANDLE handle = transport_interface->IoTHubTransport_Create(&config);

	// assert
	ASSERT_IS_NULL(handle);
}


END_TEST_SUITE(iothubtransportamqp_ut)
