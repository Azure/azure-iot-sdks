// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include <stddef.h>

#include "testrunnerswitcher.h"
#include "umock_c.h"
#include "umock_c_negative_tests.h"
#include "umocktypes_charptr.h"
#include "umocktypes_stdint.h"

#define ENABLE_MOCKS

#include "azure_c_shared_utility/gballoc.h"

#include "azure_c_shared_utility/xio.h"
#include "azure_c_shared_utility/wsio.h"
#include "iothubtransport_mqtt_common.h"

#undef ENABLE_MOCKS

#include "iothubtransportmqtt_websockets.h"

static const char* TEST_STRING_VALUE = "FULLY_QUALIFIED_HOSTNAME";
static const char* TEST_DEVICE_ID = "thisIsDeviceID";
static const char* TEST_DEVICE_KEY = "thisIsDeviceKey";
static const char* TEST_DEVICE_SAS = "thisIsDeviceSasToken";
static const char* TEST_IOTHUB_NAME = "thisIsIotHubName";
static const char* TEST_IOTHUB_SUFFIX = "thisIsIotHubSuffix";
static const char* TEST_PROTOCOL_GATEWAY_HOSTNAME = NULL;
static const char* TEST_PROTOCOL_GATEWAY_HOSTNAME_NON_NULL = "ssl://thisIsAGatewayHostName.net";

static const char* TEST_OPTION_NAME = "TEST_OPTION_NAME";
static const char* TEST_OPTION_VALUE = "test_option_value";

static const IOTHUB_CLIENT_LL_HANDLE TEST_IOTHUB_CLIENT_LL_HANDLE = (IOTHUB_CLIENT_LL_HANDLE)0x4343;
static const TRANSPORT_LL_HANDLE TEST_TRANSPORT_HANDLE = (TRANSPORT_LL_HANDLE)0x4444;
static XIO_HANDLE TEST_XIO_HANDLE = (XIO_HANDLE)0x1126;
static IOTHUB_DEVICE_HANDLE TEST_DEVICE_HANDLE = (IOTHUB_DEVICE_HANDLE)0x1181;
static IO_INTERFACE_DESCRIPTION* TEST_INTERFACE_DESC = (IO_INTERFACE_DESCRIPTION*)0x1182;

static IOTHUB_CLIENT_CONFIG g_iothubClientConfig = { 0 };
static DLIST_ENTRY g_waitingToSend;

static MQTT_GET_IO_TRANSPORT g_get_io_transport;

TEST_DEFINE_ENUM_TYPE(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_RESULT_VALUES);
IMPLEMENT_UMOCK_C_ENUM_TYPE(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_RESULT_VALUES);

TEST_DEFINE_ENUM_TYPE(IOTHUB_CLIENT_STATUS, IOTHUB_CLIENT_STATUS_VALUES);
IMPLEMENT_UMOCK_C_ENUM_TYPE(IOTHUB_CLIENT_STATUS, IOTHUB_CLIENT_STATUS_VALUES);

TEST_DEFINE_ENUM_TYPE(IOTHUB_CLIENT_RETRY_POLICY, IOTHUB_CLIENT_RETRY_POLICY_VALUES);
IMPLEMENT_UMOCK_C_ENUM_TYPE(IOTHUB_CLIENT_RETRY_POLICY, IOTHUB_CLIENT_RETRY_POLICY_VALUES);

static TEST_MUTEX_HANDLE test_serialize_mutex;
static TEST_MUTEX_HANDLE g_dllByDll;

#define TEST_RETRY_POLICY IOTHUB_CLIENT_RETRY_EXPONENTIAL_BACKOFF_WITH_JITTER
#define TEST_RETRY_TIMEOUT_SECS 60

//Callbacks for Testing
static void* g_callbackCtx;

static pfIoTHubTransport_GetHostname                IoTHubTransportMqtt_WS_GetHostname;
static pfIoTHubTransport_SetOption                  IoTHubTransportMqtt_WS_SetOption;
static pfIoTHubTransport_Create                     IoTHubTransportMqtt_WS_Create;
static pfIoTHubTransport_Destroy                    IoTHubTransportMqtt_WS_Destroy;
static pfIotHubTransport_Register                   IoTHubTransportMqtt_WS_Register;
static pfIotHubTransport_Unregister                 IoTHubTransportMqtt_WS_Unregister;
static pfIoTHubTransport_Subscribe                  IoTHubTransportMqtt_WS_Subscribe;
static pfIoTHubTransport_Unsubscribe                IoTHubTransportMqtt_WS_Unsubscribe;
static pfIoTHubTransport_DoWork                     IoTHubTransportMqtt_WS_DoWork;
static pfIoTHubTransport_SetRetryPolicy             IoTHubTransportMqtt_WS_SetRetryPolicy;
static pfIoTHubTransport_GetSendStatus              IoTHubTransportMqtt_WS_GetSendStatus;
static pfIoTHubTransport_Subscribe_DeviceTwin       IoTHubTransportMqtt_WS_Subscribe_DeviceTwin;
static pfIoTHubTransport_Unsubscribe_DeviceTwin     IoTHubTransportMqtt_WS_Unsubscribe_DeviceTwin;
static pfIoTHubTransport_Subscribe_DeviceMethod     IoTHubTransportMqtt_WS_Subscribe_DeviceMethod;
static pfIoTHubTransport_Unsubscribe_DeviceMethod   IoTHubTransportMqtt_WS_Unsubscribe_DeviceMethod;
static pfIoTHubTransport_ProcessItem                IoTHubTransportMqtt_WS_ProcessItem;

static TRANSPORT_LL_HANDLE my_IoTHubTransport_MQTT_Common_Create(const IOTHUBTRANSPORT_CONFIG* config, MQTT_GET_IO_TRANSPORT get_io_transport)
{
    (void)config;
    g_get_io_transport = get_io_transport;
    return TEST_TRANSPORT_HANDLE;
}

DEFINE_ENUM_STRINGS(UMOCK_C_ERROR_CODE, UMOCK_C_ERROR_CODE_VALUES)

static void on_umock_c_error(UMOCK_C_ERROR_CODE error_code)
{
    (void)error_code;
    ASSERT_FAIL("umock_c reported error");
}

BEGIN_TEST_SUITE(iothubtransportmqtt_ws_ut)

TEST_SUITE_INITIALIZE(suite_init)
{
    //int result;

    TEST_INITIALIZE_MEMORY_DEBUG(g_dllByDll);

    test_serialize_mutex = TEST_MUTEX_CREATE();
    ASSERT_IS_NOT_NULL(test_serialize_mutex);

    umock_c_init(on_umock_c_error);

    REGISTER_UMOCK_ALIAS_TYPE(XIO_HANDLE, void*);
    REGISTER_UMOCK_ALIAS_TYPE(TRANSPORT_LL_HANDLE, void*);
    REGISTER_UMOCK_ALIAS_TYPE(MQTT_GET_IO_TRANSPORT, void*);
    REGISTER_UMOCK_ALIAS_TYPE(IOTHUB_DEVICE_HANDLE, void*);
    REGISTER_UMOCK_ALIAS_TYPE(PDLIST_ENTRY, void*);
    REGISTER_UMOCK_ALIAS_TYPE(const PDLIST_ENTRY, void*);
    REGISTER_UMOCK_ALIAS_TYPE(IOTHUB_CLIENT_LL_HANDLE, void*);
    REGISTER_UMOCK_ALIAS_TYPE(IOTHUB_CLIENT_CONFIRMATION_RESULT, int);
    REGISTER_UMOCK_ALIAS_TYPE(IOTHUBMESSAGE_DISPOSITION_RESULT, int);
    REGISTER_UMOCK_ALIAS_TYPE(IOTHUB_CLIENT_RETRY_POLICY, int);

    REGISTER_GLOBAL_MOCK_HOOK(IoTHubTransport_MQTT_Common_Create, my_IoTHubTransport_MQTT_Common_Create);

    REGISTER_GLOBAL_MOCK_RETURN(IoTHubTransport_MQTT_Common_Subscribe, 0);
    REGISTER_GLOBAL_MOCK_RETURN(IoTHubTransport_MQTT_Common_GetSendStatus, IOTHUB_CLIENT_OK);
    REGISTER_GLOBAL_MOCK_RETURN(IoTHubTransport_MQTT_Common_SetOption, IOTHUB_CLIENT_OK);
    REGISTER_GLOBAL_MOCK_RETURN(IoTHubTransport_MQTT_Common_Register, TEST_DEVICE_HANDLE);
    REGISTER_GLOBAL_MOCK_RETURN(IoTHubTransport_MQTT_Common_GetHostname, (STRING_HANDLE)0x1182);
    REGISTER_GLOBAL_MOCK_RETURN(IoTHubTransport_MQTT_Common_SetRetryPolicy, 0);

    REGISTER_GLOBAL_MOCK_RETURN(xio_create, TEST_XIO_HANDLE);
    REGISTER_GLOBAL_MOCK_RETURN(wsio_get_interface_description, TEST_INTERFACE_DESC);

    /* Tests_SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_07_011: [ This function shall return a pointer to a structure of type TRANSPORT_PROVIDER having the following values for its fields:

    IoTHubTransport_GetHostname = IoTHubTransportMqtt_WS_GetHostname
    IoTHubTransport_Create = IoTHubTransportMqtt_WS_Create
    IoTHubTransport_Destroy = IoTHubTransportMqtt_WS_Destroy
    IoTHubTransport_Subscribe = IoTHubTransportMqtt_WS_Subscribe
    IoTHubTransport_Unsubscribe = IoTHubTransportMqtt_WS_Unsubscribe
    IoTHubTransport_DoWork = IoTHubTransportMqtt_WS_DoWork
    IoTHubTransport_SetOption = IoTHubTransportMqtt_WS_SetOption
    IoTHubTransport_Subscribe_DeviceTwin = IoTHubTransportMqtt_WS_Subscribe_DeviceTwin
    IoTHubTransport_Unsubscribe_DeviceTwin = IoTHubTransportMqtt_WS_Unsubscribe_DeviceTwin
    IoTHubTransport_Subscribe_DeviceMethod = IoTHubTransportMqtt_WS_Subscribe_DeviceMethod
    IoTHubTransport_Unsubscribe_DeviceMethod = IoTHubTransportMqtt_WS_Unsubscribe_DeviceMethod
    IoTHubTransport_ProcessItem = IoTHubTransportMqtt_WS_ProcessItem ] */
    IoTHubTransportMqtt_WS_GetHostname = ((TRANSPORT_PROVIDER*)MQTT_WebSocket_Protocol())->IoTHubTransport_GetHostname;
    IoTHubTransportMqtt_WS_SetOption = ((TRANSPORT_PROVIDER*)MQTT_WebSocket_Protocol())->IoTHubTransport_SetOption;
    IoTHubTransportMqtt_WS_Create = ((TRANSPORT_PROVIDER*)MQTT_WebSocket_Protocol())->IoTHubTransport_Create;
    IoTHubTransportMqtt_WS_Destroy = ((TRANSPORT_PROVIDER*)MQTT_WebSocket_Protocol())->IoTHubTransport_Destroy;
    IoTHubTransportMqtt_WS_Register = ((TRANSPORT_PROVIDER*)MQTT_WebSocket_Protocol())->IoTHubTransport_Register;
    IoTHubTransportMqtt_WS_Unregister = ((TRANSPORT_PROVIDER*)MQTT_WebSocket_Protocol())->IoTHubTransport_Unregister;
    IoTHubTransportMqtt_WS_Subscribe = ((TRANSPORT_PROVIDER*)MQTT_WebSocket_Protocol())->IoTHubTransport_Subscribe;
    IoTHubTransportMqtt_WS_Unsubscribe = ((TRANSPORT_PROVIDER*)MQTT_WebSocket_Protocol())->IoTHubTransport_Unsubscribe;
    IoTHubTransportMqtt_WS_DoWork = ((TRANSPORT_PROVIDER*)MQTT_WebSocket_Protocol())->IoTHubTransport_DoWork;
    IoTHubTransportMqtt_WS_SetRetryPolicy = ((TRANSPORT_PROVIDER*)MQTT_WebSocket_Protocol())->IoTHubTransport_SetRetryPolicy;
    IoTHubTransportMqtt_WS_GetSendStatus = ((TRANSPORT_PROVIDER*)MQTT_WebSocket_Protocol())->IoTHubTransport_GetSendStatus;
    IoTHubTransportMqtt_WS_Subscribe_DeviceTwin = ((TRANSPORT_PROVIDER*)MQTT_WebSocket_Protocol())->IoTHubTransport_Subscribe_DeviceTwin;
    IoTHubTransportMqtt_WS_Unsubscribe_DeviceTwin = ((TRANSPORT_PROVIDER*)MQTT_WebSocket_Protocol())->IoTHubTransport_Unsubscribe_DeviceTwin;
    IoTHubTransportMqtt_WS_Subscribe_DeviceMethod = ((TRANSPORT_PROVIDER*)MQTT_WebSocket_Protocol())->IoTHubTransport_Subscribe_DeviceMethod;
    IoTHubTransportMqtt_WS_Unsubscribe_DeviceMethod = ((TRANSPORT_PROVIDER*)MQTT_WebSocket_Protocol())->IoTHubTransport_Unsubscribe_DeviceMethod;
    IoTHubTransportMqtt_WS_ProcessItem = ((TRANSPORT_PROVIDER*)MQTT_WebSocket_Protocol())->IoTHubTransport_ProcessItem;
}

TEST_SUITE_CLEANUP(suite_cleanup)
{
    umock_c_deinit();
    TEST_MUTEX_DESTROY(test_serialize_mutex);
    TEST_DEINITIALIZE_MEMORY_DEBUG(g_dllByDll);
}

TEST_FUNCTION_INITIALIZE(method_init)
{
    TEST_MUTEX_ACQUIRE(test_serialize_mutex);

    g_callbackCtx = NULL;
    umock_c_reset_all_calls();
}

TEST_FUNCTION_CLEANUP(TestMethodCleanup)
{
    TEST_MUTEX_RELEASE(test_serialize_mutex);
}

static void SetupIothubTransportConfig(IOTHUBTRANSPORT_CONFIG* config, const char* deviceId, const char* deviceKey, const char* iotHubName,
    const char* iotHubSuffix, const char* protocolGatewayHostName)
{
    g_iothubClientConfig.protocol = MQTT_WebSocket_Protocol;
    g_iothubClientConfig.deviceId = deviceId;
    g_iothubClientConfig.deviceKey = deviceKey;
    g_iothubClientConfig.deviceSasToken = NULL;
    g_iothubClientConfig.iotHubName = iotHubName;
    g_iothubClientConfig.iotHubSuffix = iotHubSuffix;
    g_iothubClientConfig.protocolGatewayHostName = protocolGatewayHostName;
    config->waitingToSend = &g_waitingToSend;
    config->upperConfig = &g_iothubClientConfig;
}

/* Tests_SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_07_001: [ IoTHubTransportMqtt_WS_Create shall create a TRANSPORT_LL_HANDLE by calling into the IoTHubMqttAbstract_Create function. ] */
TEST_FUNCTION(IoTHubTransportMqtt_WS_Create_success)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);

    // act
    STRICT_EXPECTED_CALL(IoTHubTransport_MQTT_Common_Create(&config, IGNORED_PTR_ARG)).IgnoreArgument_get_io_transport();

    TRANSPORT_LL_HANDLE handle = IoTHubTransportMqtt_WS_Create(&config);

    // assert
    ASSERT_IS_NOT_NULL(handle);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
}

/* Tests_SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_07_012: [ getIoTransportProvider shall return the XIO_HANDLE returns by xio_create. ] */
TEST_FUNCTION(IoTHubTransportMqtt_WS_getWebSocketsIOTransport_success)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);
    (void)IoTHubTransportMqtt_WS_Create(&config);
    umock_c_reset_all_calls();

    // act
    STRICT_EXPECTED_CALL(wsio_get_interface_description());
    STRICT_EXPECTED_CALL(xio_create(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreAllArguments();

    ASSERT_IS_NOT_NULL(g_get_io_transport);
    XIO_HANDLE xioTest = g_get_io_transport(TEST_STRING_VALUE);

    // assert
    ASSERT_IS_NOT_NULL(xioTest);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
}

/* Tests_SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_07_013: [ If platform_get_default_tlsio returns NULL getIoTransportProvider shall return NULL. ] */
TEST_FUNCTION(IoTHubTransportMqtt_WS_getWebSocketsIOTransport_wsio_get_interface_description_NULL_fail)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);
    (void)IoTHubTransportMqtt_WS_Create(&config);
    umock_c_reset_all_calls();

    // act
    STRICT_EXPECTED_CALL(wsio_get_interface_description()).SetReturn(NULL);

    ASSERT_IS_NOT_NULL(g_get_io_transport);
    XIO_HANDLE xioTest = g_get_io_transport(TEST_STRING_VALUE);

    // assert
    ASSERT_IS_NULL(xioTest);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
}

/* Tests_SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_07_002: [ IoTHubTransportMqtt_WS_Destroy shall destroy the TRANSPORT_LL_HANDLE by calling into the IoTHubMqttAbstract_Destroy function. ] */
TEST_FUNCTION(IoTHubTransportMqtt_WS_Destroy_success)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);
    TRANSPORT_LL_HANDLE handle = IoTHubTransportMqtt_WS_Create(&config);
    umock_c_reset_all_calls();

    // act
    STRICT_EXPECTED_CALL(IoTHubTransport_MQTT_Common_Destroy(handle));
    
    IoTHubTransportMqtt_WS_Destroy(handle);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
}

/* Tests_SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_07_005: [ IoTHubTransportMqtt_WS_Subscribe shall subscribe the TRANSPORT_LL_HANDLE by calling into the IoTHubMqttAbstract_Subscribe function. ] */
TEST_FUNCTION(IoTHubTransportMqtt_WS_Subscribe_success)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);
    TRANSPORT_LL_HANDLE handle = IoTHubTransportMqtt_WS_Create(&config);
    umock_c_reset_all_calls();

    // act
    STRICT_EXPECTED_CALL(IoTHubTransport_MQTT_Common_Subscribe(handle));

    int result = IoTHubTransportMqtt_WS_Subscribe(handle);

    // assert
    ASSERT_ARE_EQUAL(int, result, 0);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_25_012: [** IoTHubTransportMqtt_WS_SetRetryPolicy shall call into the IoTHubMqttAbstract_SetRetryPolicy function.]*/
TEST_FUNCTION(IoTHubTransportMqtt_WS_SetRetryPolicy_success)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);
    TRANSPORT_LL_HANDLE handle = IoTHubTransportMqtt_WS_Create(&config);
    umock_c_reset_all_calls();

    // act
    STRICT_EXPECTED_CALL(IoTHubTransport_MQTT_Common_SetRetryPolicy(handle, TEST_RETRY_POLICY, TEST_RETRY_TIMEOUT_SECS));

    int result = IoTHubTransportMqtt_WS_SetRetryPolicy(handle, TEST_RETRY_POLICY, TEST_RETRY_TIMEOUT_SECS);

    // assert
    ASSERT_ARE_EQUAL(int, result, 0);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
}

/* Tests_SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_07_006: [ IoTHubTransportMqtt_WS_Unsubscribe shall unsubscribe the TRANSPORT_LL_HANDLE by calling into the IoTHubMqttAbstract_Unsubscribe function. ] */
TEST_FUNCTION(IoTHubTransportMqtt_WS_Unsubscribe_success)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);
    TRANSPORT_LL_HANDLE handle = IoTHubTransportMqtt_WS_Create(&config);
    umock_c_reset_all_calls();

    // act
    STRICT_EXPECTED_CALL(IoTHubTransport_MQTT_Common_Unsubscribe(handle));

    IoTHubTransportMqtt_WS_Unsubscribe(handle);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
}

/* Tests_SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_07_007: [ IoTHubTransportMqtt_WS_DoWork shall call into the IoTHubMqttAbstract_DoWork function. ] */
TEST_FUNCTION(IoTHubTransportMqtt_WS_DoWork_success)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);
    TRANSPORT_LL_HANDLE handle = IoTHubTransportMqtt_WS_Create(&config);
    umock_c_reset_all_calls();

    // act
    STRICT_EXPECTED_CALL(IoTHubTransport_MQTT_Common_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE));

    IoTHubTransportMqtt_WS_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
}

/* Tests_SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_07_008: [ IoTHubTransportMqtt_WS_GetSendStatus shall get the send status by calling into the IoTHubMqttAbstract_GetSendStatus function. ] */
TEST_FUNCTION(IoTHubTransportMqtt_WS_GetSendStatus_success)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);
    TRANSPORT_LL_HANDLE handle = IoTHubTransportMqtt_WS_Create(&config);
    umock_c_reset_all_calls();

    IOTHUB_CLIENT_STATUS iotHubClientStatus;

    // act
    STRICT_EXPECTED_CALL(IoTHubTransport_MQTT_Common_GetSendStatus(handle, &iotHubClientStatus));

    IOTHUB_CLIENT_RESULT result = IoTHubTransportMqtt_WS_GetSendStatus(handle, &iotHubClientStatus);

    // assert
    ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
}

/* Tests_SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_07_009: [ IoTHubTransportMqtt_WS_SetOption shall set the options by calling into the IoTHubMqttAbstract_SetOption function. ] */
TEST_FUNCTION(IoTHubTransportMqtt_WS_SetOption_success)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);
    TRANSPORT_LL_HANDLE handle = IoTHubTransportMqtt_WS_Create(&config);
    umock_c_reset_all_calls();

    // act
    STRICT_EXPECTED_CALL(IoTHubTransport_MQTT_Common_SetOption(handle, TEST_OPTION_NAME, TEST_OPTION_VALUE));

    IOTHUB_CLIENT_RESULT result = IoTHubTransportMqtt_WS_SetOption(handle, TEST_OPTION_NAME, TEST_OPTION_VALUE);

    // assert
    ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
}

/* Tests_SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_07_003: [ IoTHubTransportMqtt_WS_Register shall register the TRANSPORT_LL_HANDLE by calling into the IoTHubMqttAbstract_Register function. ]*/
TEST_FUNCTION(IoTHubTransportMqtt_WS_Register_success)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);
    TRANSPORT_LL_HANDLE handle = IoTHubTransportMqtt_WS_Create(&config);
    umock_c_reset_all_calls();

    IOTHUB_DEVICE_CONFIG deviceConfig;
    deviceConfig.deviceId = TEST_DEVICE_ID;
    deviceConfig.deviceKey = NULL;
    deviceConfig.deviceSasToken = NULL;

    // act
    STRICT_EXPECTED_CALL(IoTHubTransport_MQTT_Common_Register(handle, &deviceConfig, TEST_IOTHUB_CLIENT_LL_HANDLE, config.waitingToSend));

    IOTHUB_DEVICE_HANDLE result = IoTHubTransportMqtt_WS_Register(handle, &deviceConfig, TEST_IOTHUB_CLIENT_LL_HANDLE, config.waitingToSend);

    // assert
    ASSERT_IS_NOT_NULL(result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
}

/* Tests_SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_07_004: [ IoTHubTransportMqtt_WS_Unregister shall register the TRANSPORT_LL_HANDLE by calling into the IoTHubMqttAbstract_Unregister function. ] */
TEST_FUNCTION(IoTHubTransportMqtt_WS_Unregister_success)
{
    // arrange

    // act
    STRICT_EXPECTED_CALL(IoTHubTransport_MQTT_Common_Unregister(TEST_DEVICE_HANDLE));

    IoTHubTransportMqtt_WS_Unregister(TEST_DEVICE_HANDLE);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
}

/* Tests_SRS_IOTHUB_MQTT_WEBSOCKET_TRANSPORT_07_010: [ IoTHubTransportMqtt_WS_GetHostname shall get the hostname by calling into the IoTHubMqttAbstract_GetHostname function. ] */
TEST_FUNCTION(IoTHubTransportMqtt_WS_GetHostname_success)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);
    TRANSPORT_LL_HANDLE handle = IoTHubTransportMqtt_WS_Create(&config);
    umock_c_reset_all_calls();

    // act
    STRICT_EXPECTED_CALL(IoTHubTransport_MQTT_Common_GetHostname(handle));

    STRING_HANDLE result = IoTHubTransportMqtt_WS_GetHostname(handle);

    // assert
    ASSERT_IS_NOT_NULL(result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
}

END_TEST_SUITE(iothubtransportmqtt_ws_ut)
