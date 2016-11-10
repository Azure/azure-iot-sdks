// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>

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
#include "azure_c_shared_utility/xlogging.h"

#define ENABLE_MOCKS
#include "azure_c_shared_utility\xio.h"
#include "azure_c_shared_utility\wsio.h"
#include "azure_c_shared_utility\platform.h"
#include "iothubtransport_amqp_common.h"
#undef ENABLE_MOCKS

#include "iothubtransportamqp_websockets.h"


static TEST_MUTEX_HANDLE g_testByTest;
static TEST_MUTEX_HANDLE g_dllByDll;

// Control parameters
DEFINE_ENUM_STRINGS(UMOCK_C_ERROR_CODE, UMOCK_C_ERROR_CODE_VALUES)
DEFINE_ENUM_STRINGS(IOTHUB_IDENTITY_TYPE, IOTHUB_IDENTITY_TYPE_VALUE)

static void on_umock_c_error(UMOCK_C_ERROR_CODE error_code)
{
    char temp_str[256];
    (void)snprintf(temp_str, sizeof(temp_str), "umock_c reported error :%s", ENUM_TO_STRING(UMOCK_C_ERROR_CODE, error_code));
    ASSERT_FAIL(temp_str);
}

#define TEST_IOTHUB_CLIENT_LL_HANDLE        ((IOTHUB_CLIENT_LL_HANDLE)0x4239)
#define TEST_IOTHUBTRANSPORT_CONFIG_HANDLE  ((IOTHUBTRANSPORT_CONFIG*)0x4240)
#define TEST_XIO_INTERFACE                  ((const IO_INTERFACE_DESCRIPTION*)0x4247)
#define TEST_XIO_HANDLE                     ((XIO_HANDLE)0x4248)
#define TEST_TRANSPORT_LL_HANDLE            ((TRANSPORT_LL_HANDLE)0x4249)
#define TEST_STRING_HANDLE                  ((STRING_HANDLE)0x4250)
#define TEST_STRING                         "SOME TEXT"
#define TEST_IOTHUB_DEVICE_CONFIG_HANDLE    ((const IOTHUB_DEVICE_CONFIG*)0x4444)
#define TEST_WAITING_TO_SEND_LIST           ((PDLIST_ENTRY)0x4445)
#define TEST_IOTHUB_DEVICE_HANDLE           ((IOTHUB_DEVICE_HANDLE)0x4446)
#define TEST_IOTHUB_IDENTITY_TYPE           IOTHUB_TYPE_DEVICE_TWIN
#define TEST_IOTHUB_IDENTITY_INFO_HANDLE    ((IOTHUB_IDENTITY_INFO*)0x4449)
#define TEST_IO_INTERFACE_DESCRIPTION_HANDLE ((IO_INTERFACE_DESCRIPTION*)0x4461)

static const IOTHUBTRANSPORT_CONFIG* saved_IoTHubTransport_AMQP_Common_Create_config;
static AMQP_GET_IO_TRANSPORT saved_IoTHubTransport_AMQP_Common_Create_get_io_transport;

static TRANSPORT_LL_HANDLE TEST_IoTHubTransport_AMQP_Common_Create(const IOTHUBTRANSPORT_CONFIG* config, AMQP_GET_IO_TRANSPORT get_io_transport)
{
	saved_IoTHubTransport_AMQP_Common_Create_config = config;
	saved_IoTHubTransport_AMQP_Common_Create_get_io_transport = get_io_transport;
	return TEST_TRANSPORT_LL_HANDLE;
}

BEGIN_TEST_SUITE(iothubtransportamqp_ws_ut)

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
	REGISTER_UMOCK_ALIAS_TYPE(IOTHUB_CLIENT_LL_HANDLE, void*);
	REGISTER_UMOCK_ALIAS_TYPE(TRANSPORT_LL_HANDLE, void*);
	REGISTER_UMOCK_ALIAS_TYPE(IOTHUB_DEVICE_HANDLE, void*);
	REGISTER_UMOCK_ALIAS_TYPE(IO_INTERFACE_DESCRIPTION, void*);
	REGISTER_UMOCK_ALIAS_TYPE(PDLIST_ENTRY, void*);
	REGISTER_UMOCK_ALIAS_TYPE(IOTHUB_IDENTITY_TYPE, void*);
	REGISTER_UMOCK_ALIAS_TYPE(AMQP_GET_IO_TRANSPORT, void*);

	REGISTER_GLOBAL_MOCK_HOOK(IoTHubTransport_AMQP_Common_Create, TEST_IoTHubTransport_AMQP_Common_Create);

    REGISTER_GLOBAL_MOCK_RETURN(wsio_get_interface_description, TEST_XIO_INTERFACE);
    REGISTER_GLOBAL_MOCK_RETURN(xio_create, TEST_XIO_HANDLE);
	REGISTER_GLOBAL_MOCK_RETURN(IoTHubTransport_AMQP_Common_GetHostname, TEST_STRING_HANDLE);
	REGISTER_GLOBAL_MOCK_RETURN(IoTHubTransport_AMQP_Common_SetOption, IOTHUB_CLIENT_OK);
	REGISTER_GLOBAL_MOCK_RETURN(IoTHubTransport_AMQP_Common_Register, TEST_IOTHUB_DEVICE_HANDLE);
	REGISTER_GLOBAL_MOCK_RETURN(IoTHubTransport_AMQP_Common_Subscribe, 0);
	REGISTER_GLOBAL_MOCK_RETURN(IoTHubTransport_AMQP_Common_Subscribe_DeviceTwin, 0);
	REGISTER_GLOBAL_MOCK_RETURN(IoTHubTransport_AMQP_Common_Subscribe_DeviceMethod, 0);
	REGISTER_GLOBAL_MOCK_RETURN(IoTHubTransport_AMQP_Common_ProcessItem, IOTHUB_PROCESS_OK);
	REGISTER_GLOBAL_MOCK_RETURN(IoTHubTransport_AMQP_Common_GetSendStatus, IOTHUB_CLIENT_OK);
	REGISTER_GLOBAL_MOCK_RETURN(wsio_get_interface_description, TEST_IO_INTERFACE_DESCRIPTION_HANDLE);
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

// Tests_SRS_IOTHUBTRANSPORTAMQP_WS_09_001: [IoTHubTransportAMQP_WS_Create shall create a TRANSPORT_LL_HANDLE by calling into the IoTHubTransport_AMQP_Common_Create function, passing `config` and getWebSocketsIOTransport.]
/* Tests_SRS_IOTHUBTRANSPORTAMQP_WS_09_019: [This function shall return a pointer to a structure of type TRANSPORT_PROVIDER having the following values for it's fields:
IoTHubTransport_Subscribe_DeviceMethod = IoTHubTransportAMQP_WS_Subscribe_DeviceMethod
IoTHubTransport_Unsubscribe_DeviceMethod = IoTHubTransportAMQP_WS_Unsubscribe_DeviceMethod
IoTHubTransport_Subscribe_DeviceTwin = IoTHubTransportAMQP_WS_Subscribe_DeviceTwin
IoTHubTransport_Unsubscribe_DeviceTwin = IoTHubTransportAMQP_WS_Unsubscribe_DeviceTwin
IoTHubTransport_ProcessItem - IoTHubTransportAMQP_WS_ProcessItem
IoTHubTransport_GetHostname = IoTHubTransportAMQP_WS_GetHostname
IoTHubTransport_Create = IoTHubTransportAMQP_WS_Create
IoTHubTransport_Destroy = IoTHubTransportAMQP_WS_Destroy
IoTHubTransport_Subscribe = IoTHubTransportAMQP_WS_Subscribe
IoTHubTransport_Unsubscribe = IoTHubTransportAMQP_WS_Unsubscribe
IoTHubTransport_DoWork = IoTHubTransportAMQP_WS_DoWork
IoTHubTransport_SetRetryLogic = IoTHubTransportAMQP_WS_SetRetryLogic
IoTHubTransport_SetOption = IoTHubTransportAMQP_WS_SetOption]*/
TEST_FUNCTION(AMQP_Create)
{
	// arrange
	TRANSPORT_PROVIDER* provider = (TRANSPORT_PROVIDER*)AMQP_Protocol_over_WebSocketsTls();

	umock_c_reset_all_calls();
	STRICT_EXPECTED_CALL(IoTHubTransport_AMQP_Common_Create(TEST_IOTHUBTRANSPORT_CONFIG_HANDLE, NULL)).IgnoreArgument_get_io_transport();
	
	saved_IoTHubTransport_AMQP_Common_Create_get_io_transport = NULL;

	// act
	TRANSPORT_LL_HANDLE tr_hdl = provider->IoTHubTransport_Create(TEST_IOTHUBTRANSPORT_CONFIG_HANDLE);

	// assert
	ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
	ASSERT_ARE_EQUAL(void_ptr, tr_hdl, TEST_TRANSPORT_LL_HANDLE);
	ASSERT_IS_NOT_NULL(saved_IoTHubTransport_AMQP_Common_Create_get_io_transport);

	// cleanup
}


// Tests_SRS_IOTHUBTRANSPORTAMQP_WS_09_002: [getWebSocketsIOTransport shall get `io_interface_description` using wsio_get_interface_description())]
// Tests_SRS_IOTHUBTRANSPORTAMQP_WS_09_003: [If `io_interface_description` is NULL getWebSocketsIOTransport shall return NULL.]
// Tests_SRS_IOTHUBTRANSPORTAMQP_WS_09_004: [getWebSocketsIOTransport shall return the XIO_HANDLE created using xio_create().]
TEST_FUNCTION(AMQP_Create_getWebSocketsIOTransport)
{
	// arrange
	TRANSPORT_PROVIDER* provider = (TRANSPORT_PROVIDER*)AMQP_Protocol_over_WebSocketsTls();

	(void)provider->IoTHubTransport_Create(TEST_IOTHUBTRANSPORT_CONFIG_HANDLE);

	umock_c_reset_all_calls();
	STRICT_EXPECTED_CALL(wsio_get_interface_description());
	STRICT_EXPECTED_CALL(xio_create(TEST_IO_INTERFACE_DESCRIPTION_HANDLE, IGNORED_PTR_ARG)).IgnoreArgument_io_create_parameters();

	// act
	XIO_HANDLE underlying_io_transport = saved_IoTHubTransport_AMQP_Common_Create_get_io_transport(TEST_STRING);

	// assert
	ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
	ASSERT_ARE_EQUAL(void_ptr, underlying_io_transport, TEST_XIO_HANDLE);

	// cleanup
}

// Tests_SRS_IOTHUBTRANSPORTAMQP_WS_09_015: [IoTHubTransportAMQP_WS_DoWork shall call into the IoTHubTransport_AMQP_Common_DoWork()]
TEST_FUNCTION(AMQP_DoWork)
{
	// arrange
	TRANSPORT_PROVIDER* provider = (TRANSPORT_PROVIDER*)AMQP_Protocol_over_WebSocketsTls();

	umock_c_reset_all_calls();
	STRICT_EXPECTED_CALL(IoTHubTransport_AMQP_Common_DoWork(TEST_TRANSPORT_LL_HANDLE, TEST_IOTHUB_CLIENT_LL_HANDLE));

	// act
	provider->IoTHubTransport_DoWork(TEST_TRANSPORT_LL_HANDLE, TEST_IOTHUB_CLIENT_LL_HANDLE);

	// assert
	ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

	// cleanup
}

// Tests_SRS_IOTHUBTRANSPORTAMQP_WS_09_006: [IoTHubTransportAMQP_WS_Register shall register the device by calling into the IoTHubTransport_AMQP_Common_Register().]
TEST_FUNCTION(AMQP_Register)
{
	// arrange
	TRANSPORT_PROVIDER* provider = (TRANSPORT_PROVIDER*)AMQP_Protocol_over_WebSocketsTls();

	umock_c_reset_all_calls();
	STRICT_EXPECTED_CALL(IoTHubTransport_AMQP_Common_Register(TEST_TRANSPORT_LL_HANDLE, TEST_IOTHUB_DEVICE_CONFIG_HANDLE, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_WAITING_TO_SEND_LIST));

	// act
	IOTHUB_DEVICE_HANDLE dev_hdl = provider->IoTHubTransport_Register(TEST_TRANSPORT_LL_HANDLE, TEST_IOTHUB_DEVICE_CONFIG_HANDLE, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_WAITING_TO_SEND_LIST);

	// assert
	ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
	ASSERT_ARE_EQUAL(void_ptr, (void_ptr)dev_hdl, (void_ptr)TEST_IOTHUB_DEVICE_HANDLE);

	// cleanup
}

// Tests_SRS_IOTHUBTRANSPORTAMQP_WS_09_007: [IoTHubTransportAMQP_WS_Unregister shall unregister the device by calling into the IoTHubTransport_AMQP_Common_Unregister().]
TEST_FUNCTION(AMQP_Unregister)
{
	// arrange
	TRANSPORT_PROVIDER* provider = (TRANSPORT_PROVIDER*)AMQP_Protocol_over_WebSocketsTls();

	umock_c_reset_all_calls();
	STRICT_EXPECTED_CALL(IoTHubTransport_AMQP_Common_Unregister(TEST_IOTHUB_DEVICE_HANDLE));

	// act
	provider->IoTHubTransport_Unregister(TEST_IOTHUB_DEVICE_HANDLE);

	// assert
	ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

	// cleanup
}

// Tests_SRS_IOTHUBTRANSPORTAMQP_WS_09_008: [IoTHubTransportAMQP_WS_Subscribe_DeviceTwin shall invoke IoTHubTransport_AMQP_Common_Subscribe_DeviceTwin() and return its result.]
TEST_FUNCTION(AMQP_Subscribe_DeviceTwin)
{
	// arrange
	TRANSPORT_PROVIDER* provider = (TRANSPORT_PROVIDER*)AMQP_Protocol_over_WebSocketsTls();

	umock_c_reset_all_calls();
	STRICT_EXPECTED_CALL(IoTHubTransport_AMQP_Common_Subscribe_DeviceTwin(TEST_IOTHUB_DEVICE_HANDLE));

	// act
	int result = provider->IoTHubTransport_Subscribe_DeviceTwin(TEST_IOTHUB_DEVICE_HANDLE);

	// assert
	ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
	ASSERT_ARE_EQUAL(int, result, 0);

	// cleanup
}

// Tests_SRS_IOTHUBTRANSPORTAMQP_WS_09_009: [IoTHubTransportAMQP_WS_Unsubscribe_DeviceTwin shall invoke IoTHubTransport_AMQP_Common_Unsubscribe_DeviceTwin()]
TEST_FUNCTION(AMQP_Unsubscribe_DeviceTwin)
{
	// arrange
	TRANSPORT_PROVIDER* provider = (TRANSPORT_PROVIDER*)AMQP_Protocol_over_WebSocketsTls();

	umock_c_reset_all_calls();
	STRICT_EXPECTED_CALL(IoTHubTransport_AMQP_Common_Unsubscribe_DeviceTwin(TEST_IOTHUB_DEVICE_HANDLE));

	// act
	provider->IoTHubTransport_Unsubscribe_DeviceTwin(TEST_IOTHUB_DEVICE_HANDLE);

	// assert
	ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

	// cleanup
}

// Tests_SRS_IOTHUBTRANSPORTAMQP_WS_09_010: [IoTHubTransportAMQP_WS_Subscribe_DeviceMethod shall invoke IoTHubTransport_AMQP_Common_Subscribe_DeviceMethod() and return its result.]
TEST_FUNCTION(AMQP_Subscribe_DeviceMethod)
{
	// arrange
	TRANSPORT_PROVIDER* provider = (TRANSPORT_PROVIDER*)AMQP_Protocol_over_WebSocketsTls();

	umock_c_reset_all_calls();
	STRICT_EXPECTED_CALL(IoTHubTransport_AMQP_Common_Subscribe_DeviceMethod(TEST_IOTHUB_DEVICE_HANDLE));

	// act
	int result = provider->IoTHubTransport_Subscribe_DeviceMethod(TEST_IOTHUB_DEVICE_HANDLE);

	// assert
	ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
	ASSERT_ARE_EQUAL(int, result, 0);

	// cleanup
}

// Tests_SRS_IOTHUBTRANSPORTAMQP_WS_09_011: [IoTHubTransportAMQP_WS_Unsubscribe_DeviceMethod shall invoke IoTHubTransport_AMQP_Common_Unsubscribe_DeviceMethod()]
TEST_FUNCTION(AMQP_Unsubscribe_DeviceMethod)
{
	// arrange
	TRANSPORT_PROVIDER* provider = (TRANSPORT_PROVIDER*)AMQP_Protocol_over_WebSocketsTls();

	umock_c_reset_all_calls();
	STRICT_EXPECTED_CALL(IoTHubTransport_AMQP_Common_Unsubscribe_DeviceMethod(TEST_IOTHUB_DEVICE_HANDLE));

	// act
	provider->IoTHubTransport_Unsubscribe_DeviceMethod(TEST_IOTHUB_DEVICE_HANDLE);

	// assert
	ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

	// cleanup
}

// Tests_SRS_IOTHUBTRANSPORTAMQP_WS_09_012: [IoTHubTransportAMQP_WS_Subscribe shall subscribe for D2C messages by calling into the IoTHubTransport_AMQP_Common_Subscribe().]
TEST_FUNCTION(AMQP_Subscribe)
{
	// arrange
	TRANSPORT_PROVIDER* provider = (TRANSPORT_PROVIDER*)AMQP_Protocol_over_WebSocketsTls();

	umock_c_reset_all_calls();
	STRICT_EXPECTED_CALL(IoTHubTransport_AMQP_Common_Subscribe(TEST_IOTHUB_DEVICE_HANDLE));

	// act
	int result = provider->IoTHubTransport_Subscribe(TEST_IOTHUB_DEVICE_HANDLE);

	// assert
	ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
	ASSERT_ARE_EQUAL(int, result, 0);

	// cleanup
}

// Tests_SRS_IOTHUBTRANSPORTAMQP_WS_09_013: [IoTHubTransportAMQP_WS_Unsubscribe shall subscribe for D2C messages by calling into the IoTHubTransport_AMQP_Common_Unsubscribe().]
TEST_FUNCTION(AMQP_Unsubscribe)
{
	// arrange
	TRANSPORT_PROVIDER* provider = (TRANSPORT_PROVIDER*)AMQP_Protocol_over_WebSocketsTls();

	umock_c_reset_all_calls();
	STRICT_EXPECTED_CALL(IoTHubTransport_AMQP_Common_Unsubscribe(TEST_IOTHUB_DEVICE_HANDLE));

	// act
	provider->IoTHubTransport_Unsubscribe(TEST_IOTHUB_DEVICE_HANDLE);

	// assert
	ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

	// cleanup
}

// Tests_SRS_IOTHUBTRANSPORTAMQP_WS_09_014: [IoTHubTransportAMQP_WS_ProcessItem shall invoke IoTHubTransport_AMQP_Common_ProcessItem() and return its result.]
TEST_FUNCTION(AMQP_ProcessItem)
{
	// arrange
	TRANSPORT_PROVIDER* provider = (TRANSPORT_PROVIDER*)AMQP_Protocol_over_WebSocketsTls();

	umock_c_reset_all_calls();
	STRICT_EXPECTED_CALL(IoTHubTransport_AMQP_Common_ProcessItem(TEST_TRANSPORT_LL_HANDLE, TEST_IOTHUB_IDENTITY_TYPE, TEST_IOTHUB_IDENTITY_INFO_HANDLE));

	// act
	IOTHUB_PROCESS_ITEM_RESULT result = provider->IoTHubTransport_ProcessItem(TEST_TRANSPORT_LL_HANDLE, TEST_IOTHUB_IDENTITY_TYPE, TEST_IOTHUB_IDENTITY_INFO_HANDLE);

	// assert
	ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
	ASSERT_ARE_EQUAL(int, result, IOTHUB_PROCESS_OK);

	// cleanup
}

// Tests_SRS_IOTHUBTRANSPORTAMQP_WS_09_017: [IoTHubTransportAMQP_WS_SetOption shall set the options by calling into the IoTHubTransport_AMQP_Common_SetOption()]
TEST_FUNCTION(AMQP_SetOption)
{
	// arrange
	TRANSPORT_PROVIDER* provider = (TRANSPORT_PROVIDER*)AMQP_Protocol_over_WebSocketsTls();

	umock_c_reset_all_calls();
	STRICT_EXPECTED_CALL(IoTHubTransport_AMQP_Common_SetOption(TEST_TRANSPORT_LL_HANDLE, TEST_STRING, TEST_STRING_HANDLE));

	// act
	int result = provider->IoTHubTransport_SetOption(TEST_TRANSPORT_LL_HANDLE, TEST_STRING, TEST_STRING_HANDLE);

	// assert
	ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
	ASSERT_ARE_EQUAL(int, result, 0);

	// cleanup
}

// Tests_SRS_IOTHUBTRANSPORTAMQP_WS_09_016: [IoTHubTransportAMQP_WS_GetSendStatus shall get the send status by calling into the IoTHubTransport_AMQP_Common_GetSendStatus()]
TEST_FUNCTION(AMQP_GetSendStatus)
{
	// arrange
	TRANSPORT_PROVIDER* provider = (TRANSPORT_PROVIDER*)AMQP_Protocol_over_WebSocketsTls();

	IOTHUB_CLIENT_STATUS client_status;

	umock_c_reset_all_calls();
	STRICT_EXPECTED_CALL(IoTHubTransport_AMQP_Common_GetSendStatus(TEST_IOTHUB_DEVICE_HANDLE, &client_status));

	// act
	IOTHUB_CLIENT_RESULT result = provider->IoTHubTransport_GetSendStatus(TEST_IOTHUB_DEVICE_HANDLE, &client_status);

	// assert
	ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
	ASSERT_ARE_EQUAL(int, result, IOTHUB_CLIENT_OK);

	// cleanup
}


// Tests_SRS_IOTHUBTRANSPORTAMQP_WS_09_018: [IoTHubTransportAMQP_WS_GetHostname shall get the hostname by calling into the IoTHubTransport_AMQP_Common_GetHostname()]
TEST_FUNCTION(AMQP_GetHostname)
{
	// arrange
	TRANSPORT_PROVIDER* provider = (TRANSPORT_PROVIDER*)AMQP_Protocol_over_WebSocketsTls();

	umock_c_reset_all_calls();
	STRICT_EXPECTED_CALL(IoTHubTransport_AMQP_Common_GetHostname(TEST_TRANSPORT_LL_HANDLE));

	// act
	STRING_HANDLE result = provider->IoTHubTransport_GetHostname(TEST_TRANSPORT_LL_HANDLE);

	// assert
	ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
	ASSERT_ARE_EQUAL(void_ptr, result, TEST_STRING_HANDLE);

	// cleanup
}

// Tests_SRS_IOTHUBTRANSPORTAMQP_WS_09_005: [IoTHubTransportAMQP_WS_Destroy shall destroy the TRANSPORT_LL_HANDLE by calling into the IoTHubTransport_AMQP_Common_Destroy().]
TEST_FUNCTION(AMQP_Destroy)
{
    // arrange
	TRANSPORT_PROVIDER* provider = (TRANSPORT_PROVIDER*)AMQP_Protocol_over_WebSocketsTls();

	umock_c_reset_all_calls();
	STRICT_EXPECTED_CALL(IoTHubTransport_AMQP_Common_Destroy(TEST_TRANSPORT_LL_HANDLE));

    // act
	provider->IoTHubTransport_Destroy(TEST_TRANSPORT_LL_HANDLE);

    // assert
	ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

	// cleanup
}

END_TEST_SUITE(iothubtransportamqp_ws_ut)
