// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include <stddef.h>

static void* my_gballoc_malloc(size_t size)
{
    return malloc(size);
}

static void my_gballoc_free(void* ptr)
{
    free(ptr);
}

void* my_gballoc_realloc(void* ptr, size_t size)
{
    return realloc(ptr, size);
}

#include "testrunnerswitcher.h"
#include "umock_c.h"
#include "umock_c_negative_tests.h"
#include "umocktypes_charptr.h"
#include "umocktypes_bool.h"
#include "umocktypes_stdint.h"

#define ENABLE_MOCKS

#include "azure_c_shared_utility/sastoken.h"
#include "azure_c_shared_utility/doublylinkedlist.h"
#include "azure_c_shared_utility/gballoc.h"
//#include "azure_c_shared_utility/macro_utils.h"

#include "azure_umqtt_c/mqtt_client.h"

#include "iothubtransportmqtt.h"
#include "iothub_client_private.h"
#include "iothub_client_options.h"

#include "azure_c_shared_utility/xio.h"
#include "azure_c_shared_utility/tlsio.h"
#include "azure_c_shared_utility/platform.h"

#include "azure_c_shared_utility/tickcounter.h"
#include "azure_c_shared_utility/lock.h"
#include "azure_c_shared_utility/string_tokenizer.h"

#undef ENABLE_MOCKS

#include "azure_c_shared_utility/strings.h"

#ifdef __cplusplus
extern "C"
{
#endif

    int STRING_sprintf(STRING_HANDLE handle, const char* format, ...);
    STRING_HANDLE STRING_construct_sprintf(const char* format, ...);

#ifdef __cplusplus
}
#endif

static STRING_HANDLE my_STRING_new(void)
{
    return (STRING_HANDLE)my_gballoc_malloc(1);
}

static STRING_HANDLE my_STRING_construct(const char* psz)
{
    (void)psz;
    return (STRING_HANDLE)my_gballoc_malloc(1);
}

static void my_STRING_delete(STRING_HANDLE handle)
{
    my_gballoc_free(handle);
}

static int my_mallocAndStrcpy_s(char** destination, const char* source)
{
    (void)source;
    size_t l = strlen(source);
    *destination = (char*)my_gballoc_malloc(l + 1);
    strcpy(*destination, source);
    return 0;
}

static const char* TEST_STRING_VALUE = "Test string value";
static const char* TEST_DEVICE_ID = "thisIsDeviceID";
static const char* TEST_DEVICE_KEY = "thisIsDeviceKey";
static const char* TEST_DEVICE_SAS = "thisIsDeviceSasToken";
static const char* TEST_IOTHUB_NAME = "thisIsIotHubName";
static const char* TEST_IOTHUB_SUFFIX = "thisIsIotHubSuffix";
static const char* TEST_PROTOCOL_GATEWAY_HOSTNAME = "ssl://thisIsAGatewayHostName.net";
static const char* TEST_VERY_LONG_DEVICE_ID = "1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890";
static const char* TEST_MQTT_MESSAGE_TOPIC = "devices/thisIsDeviceID/messages/devicebound/#";
static const char* TEST_MQTT_MSG_TOPIC = "devices/jebrandoDevice/messages/devicebound/iothub-ack=Full&%24.to=%2Fdevices%2FjebrandoDevice%2Fmessages%2FdeviceBound&%24.cid&%24.uid";
static const char* TEST_MQTT_MSG_TOPIC_W_1_PROP = "devices/thisIsDeviceID/messages/devicebound/iothub-ack=Full&propName=PropValue&DeviceInfo=smokeTest&%24.to=%2Fdevices%2FjebrandoDevice%2Fmessages%2FdeviceBound&%24.cid&%24.uid";
static const char* TEST_MQTT_EVENT_TOPIC = "devices/thisIsDeviceID/messages/events/";
static const char* TEST_MQTT_SAS_TOKEN = "thisIsIotHubName.thisIsIotHubSuffix/devices/thisIsDeviceID";
static const char* TEST_HOST_NAME = "thisIsIotHubName.thisIsIotHubSuffix";
static const char* TEST_EMPTY_STRING = "";
static const char* TEST_SAS_TOKEN = "Test_SAS_Token_value";
static const char* X509_CERT_CERTIFICATE = "-----BEGIN CERTIFICATE-----MIID-----END CERTIFICATE-----";
static const char* X509_PRIVATE_KEY_OPTION = "x509privatekey";
static const char* X509_PRIVATE_KEY = "-----BEGIN RSA PRIVATE KEY-----MIIE-----END RSA PRIVATE KEY-----";

const char* PROPERTY_SEPARATOR = "&";

static const IOTHUB_CLIENT_LL_HANDLE TEST_IOTHUB_CLIENT_LL_HANDLE = (IOTHUB_CLIENT_LL_HANDLE)0x4343;
static const TRANSPORT_LL_HANDLE TEST_TRANSPORT_HANDLE = (TRANSPORT_LL_HANDLE)0x4444;
static const MQTT_CLIENT_HANDLE TEST_MQTT_CLIENT_HANDLE = (MQTT_CLIENT_HANDLE)0x1122;
static const PDLIST_ENTRY TEST_PDLIST_ENTRY = (PDLIST_ENTRY)0x1123;
static const MQTT_MESSAGE_HANDLE TEST_MQTT_MESSAGE_HANDLE = (MQTT_MESSAGE_HANDLE)0x1124;

static IO_INTERFACE_DESCRIPTION* TEST_IO_INTERFACE = (IO_INTERFACE_DESCRIPTION*)0x1125;
static XIO_HANDLE TEST_XIO_HANDLE = (XIO_HANDLE)0x1126;

static const STRING_TOKENIZER_HANDLE TEST_STRING_TOKENIZER_HANDLE = (STRING_TOKENIZER_HANDLE)0x1127;

/*this is the default message and has type BYTEARRAY*/
static const IOTHUB_MESSAGE_HANDLE TEST_IOTHUB_MSG_BYTEARRAY = (const IOTHUB_MESSAGE_HANDLE)0x01d1;

/*this is a STRING type message*/
static IOTHUB_MESSAGE_HANDLE TEST_IOTHUB_MSG_STRING = (IOTHUB_MESSAGE_HANDLE)0x01d2;

static const TICK_COUNTER_HANDLE TEST_COUNTER_HANDLE = (TICK_COUNTER_HANDLE)0x12;
static const MAP_HANDLE TEST_MESSAGE_PROP_MAP = (MAP_HANDLE)0x1212;

static char appMessageString[] = "App Message String";
static uint8_t appMessage[] = { 0x54, 0x68, 0x69, 0x73, 0x20, 0x69, 0x73, 0x20, 0x61, 0x20, 0x54, 0x65, 0x73, 0x74, 0x20, 0x4d, 0x73, 0x67 };
static const size_t appMsgSize = sizeof(appMessage) / sizeof(appMessage[0]);

static IOTHUB_CLIENT_CONFIG g_iothubClientConfig = { 0 };
static DLIST_ENTRY g_waitingToSend;

static uint64_t g_current_ms;
static size_t g_tokenizerIndex;

#define TEST_TIME_T ((time_t)-1)

#if 0
//Messages to be used in test
static const IOTHUB_MESSAGE_LIST message1 =  /* this is the oldest message, always the first to be processed, send etc*/
{
    TEST_IOTHUB_MSG_BYTEARRAY,          /* IOTHUB_MESSAGE_HANDLE messageHandle;                 */
    NULL,                               /* IOTHUB_CLIENT_EVENT_CONFIRMATION_CALLBACK callback;  */
    NULL,                               /* void* context;                                       */
    { NULL, NULL },                     /* DLIST_ENTRY entry;                                   */
    0
};

static IOTHUB_MESSAGE_LIST message2 =  /* this is the oldest message, always the first to be processed, send etc*/
{
    TEST_IOTHUB_MSG_STRING,             /* IOTHUB_MESSAGE_HANDLE messageHandle;                 */
    NULL,                               /* IOTHUB_CLIENT_EVENT_CONFIRMATION_CALLBACK callback;  */
    NULL,                               /* void* context;                                       */
    { NULL, NULL },                     /* DLIST_ENTRY entry;                                   */
    0
};
#endif

static APP_PAYLOAD TEST_APP_PAYLOAD;

TEST_DEFINE_ENUM_TYPE(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_RESULT_VALUES);
IMPLEMENT_UMOCK_C_ENUM_TYPE(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_RESULT_VALUES);

TEST_DEFINE_ENUM_TYPE(IOTHUB_CLIENT_STATUS, IOTHUB_CLIENT_STATUS_VALUES);
IMPLEMENT_UMOCK_C_ENUM_TYPE(IOTHUB_CLIENT_STATUS, IOTHUB_CLIENT_STATUS_VALUES);

static TEST_MUTEX_HANDLE test_serialize_mutex;
static TEST_MUTEX_HANDLE g_dllByDll;


//Callbacks for Testing
static ON_MQTT_MESSAGE_RECV_CALLBACK g_fnMqttMsgRecv;
static ON_MQTT_OPERATION_CALLBACK g_fnMqttOperationCallback;
static void* g_callbackCtx;
static bool g_nullMapVariable;

static pfIoTHubTransport_GetHostname    IoTHubTransportMqtt_GetHostname;
static pfIoTHubTransport_SetOption      IoTHubTransportMqtt_SetOption;
static pfIoTHubTransport_Create         IoTHubTransportMqtt_Create;
static pfIoTHubTransport_Destroy        IoTHubTransportMqtt_Destroy;
static pfIotHubTransport_Register       IoTHubTransportMqtt_Register;
static pfIotHubTransport_Unregister     IoTHubTransportMqtt_Unregister;
static pfIoTHubTransport_Subscribe      IoTHubTransportMqtt_Subscribe;
static pfIoTHubTransport_Unsubscribe    IoTHubTransportMqtt_Unsubscribe;
static pfIoTHubTransport_DoWork         IoTHubTransportMqtt_DoWork;
static pfIoTHubTransport_GetSendStatus  IoTHubTransportMqtt_GetSendStatus;

#ifdef __cplusplus
extern "C"
{
#endif
    int STRING_sprintf(STRING_HANDLE handle, const char* format, ...)
    {
        (void)handle;
        (void)format;
        return 0;
    }

    STRING_HANDLE STRING_construct_sprintf(const char* format, ...)
    {
        (void)format;
        return NULL;
    }

    const char* my_STRING_c_str(STRING_HANDLE handle)
    {
        (void)handle;
        return TEST_STRING_VALUE;
    }

    void real_DList_InitializeListHead(PDLIST_ENTRY listHead);
    int real_DList_IsListEmpty(const PDLIST_ENTRY listHead);
    void real_DList_InsertTailList(PDLIST_ENTRY listHead, PDLIST_ENTRY listEntry);
    void real_DList_InsertHeadList(PDLIST_ENTRY listHead, PDLIST_ENTRY listEntry);
    void real_DList_AppendTailList(PDLIST_ENTRY listHead, PDLIST_ENTRY ListToAppend);
    int real_DList_RemoveEntryList(PDLIST_ENTRY listEntry);
    PDLIST_ENTRY real_DList_RemoveHeadList(PDLIST_ENTRY listHead);

#ifdef __cplusplus
}
#endif

static IOTHUBMESSAGE_CONTENT_TYPE my_IoTHubMessage_GetContentType(IOTHUB_MESSAGE_HANDLE iotHubMessageHandle)
{
    IOTHUBMESSAGE_CONTENT_TYPE result2;
    if (iotHubMessageHandle == TEST_IOTHUB_MSG_BYTEARRAY)
    {
        result2 = IOTHUBMESSAGE_BYTEARRAY;
    }
    else if (iotHubMessageHandle == TEST_IOTHUB_MSG_STRING)
    {
        result2 = IOTHUBMESSAGE_STRING;
    }
    else
    {
        result2 = IOTHUBMESSAGE_UNKNOWN;
    }
    return result2;
}

static IOTHUB_MESSAGE_RESULT my_IoTHubMessage_GetByteArray(IOTHUB_MESSAGE_HANDLE iotHubMessageHandle, const unsigned char** buffer, size_t* size)
{
    if (iotHubMessageHandle == TEST_IOTHUB_MSG_BYTEARRAY)
    {
        *buffer = appMessage;
        *size = appMsgSize;
    }
    else
    {
        /*not expected really*/
        *buffer = (const unsigned char*)"333";
        *size = 3;
    }
    return IOTHUB_MESSAGE_OK;
}

static void my_IoTHubMessage_Destroy(IOTHUB_MESSAGE_HANDLE iotHubMessageHandle)
{
    (void)iotHubMessageHandle;
}

static void my_IoTHubClient_LL_SendComplete(IOTHUB_CLIENT_LL_HANDLE handle, PDLIST_ENTRY completed, IOTHUB_CLIENT_CONFIRMATION_RESULT result)
{
    (void)handle;
    (void)completed;
    (void)result;
}

static MQTT_CLIENT_HANDLE my_mqtt_client_init(ON_MQTT_MESSAGE_RECV_CALLBACK msgRecv, ON_MQTT_OPERATION_CALLBACK opCallback, void* callbackCtx)
{
    g_fnMqttMsgRecv = msgRecv;
    g_fnMqttOperationCallback = opCallback;
    g_callbackCtx = callbackCtx;
    return (MQTT_CLIENT_HANDLE)my_gballoc_malloc(1);;
}

static void my_mqtt_client_deinit(MQTT_CLIENT_HANDLE handle)
{
    my_gballoc_free(handle);
}

static void my_mqtt_client_dowork(MQTT_CLIENT_HANDLE handle)
{
    (void)handle;
}

static STRING_TOKENIZER_HANDLE my_STRING_TOKENIZER_create(STRING_HANDLE handle)
{
    (void)handle;
    return (STRING_TOKENIZER_HANDLE)my_gballoc_malloc(1);
}

int my_STRING_TOKENIZER_get_next_token(STRING_TOKENIZER_HANDLE t, STRING_HANDLE output, const char* delimiters)
{
    int result;
    (void)delimiters;
    (void)t;
    const char* text = NULL;
    switch (g_tokenizerIndex++)
    {
        case 0:
            break;
        case 1:
            text = "devices/thisIsDeviceID/messages/devicebound/iothub-ack=Full";
            break;
        case 2:
            text = "propname1=value2";
            break;
        case 3:
            text = "propname2=value2";
            break;
        case 4:
            text = "%24.to=%2Fdevices%2FjebrandoDevice%2Fmessages%2FdeviceBound&%24.cid";
            break;
        case 5:
            text = "%24.cid";
            break;
        case 6:
            text = ""; // Done
            break;
        default:
            break;
    }
    if (text != NULL)
    {
        result = 0;
        output = NULL;
    }
    else
    {
        result = __LINE__;
    }
    return result;
}

static void my_STRING_TOKENIZER_destroy(STRING_TOKENIZER_HANDLE handle)
{
    my_gballoc_free(handle);
}

static STRING_HANDLE my_SASToken_Create(STRING_HANDLE key, STRING_HANDLE scope, STRING_HANDLE keyName, size_t expiry)
{
    (void)key;
    (void)scope;
    (void)keyName;
    (void)expiry;
    return (STRING_HANDLE)my_gballoc_malloc(1);
}

static MAP_RESULT my_Map_GetInternals(MAP_HANDLE handle, const char*const** keys, const char*const** values, size_t* count)
{
    (void)handle;
    *keys = NULL;
    *values = NULL;
    *count = 0;
    return MAP_OK;
}

static XIO_HANDLE my_xio_create(const IO_INTERFACE_DESCRIPTION* io_interface_description, const void* xio_create_parameters)
{
    (void)io_interface_description;
    (void)xio_create_parameters;
    return (XIO_HANDLE)my_gballoc_malloc(1);
}

static void my_xio_destroy(XIO_HANDLE ioHandle)
{
    my_gballoc_free(ioHandle);
}

static TICK_COUNTER_HANDLE my_tickcounter_create(void)
{
    return (TICK_COUNTER_HANDLE)my_gballoc_malloc(1);
}

static int my_tickcounter_get_current_ms(TICK_COUNTER_HANDLE tick_counter, uint64_t* current_ms)
{
    (void)tick_counter;
    g_current_ms += 1000;
    *current_ms = g_current_ms;
    return 0;
}

static void my_tickcounter_destroy(TICK_COUNTER_HANDLE tick_counter)
{
    my_gballoc_free(tick_counter);
}

static MAP_HANDLE my_Map_Create(MAP_FILTER_CALLBACK mapFilterFunc)
{
    (void)mapFilterFunc;
    return (MAP_HANDLE)malloc(1);
}

static MAP_HANDLE my_Map_Clone(MAP_HANDLE handle)
{
    (void)handle;
    return (MAP_HANDLE)malloc(1);
}

static void my_Map_Destroy(MAP_HANDLE handle)
{
    free(handle);
}

DEFINE_ENUM_STRINGS(UMOCK_C_ERROR_CODE, UMOCK_C_ERROR_CODE_VALUES)

static void on_umock_c_error(UMOCK_C_ERROR_CODE error_code)
{
    (void)error_code;
    ASSERT_FAIL("umock_c reported error");
}

BEGIN_TEST_SUITE(iothubtransportmqtt_ut)

TEST_SUITE_INITIALIZE(suite_init)
{
    int result;

    TEST_APP_PAYLOAD.message = appMessage;
    TEST_APP_PAYLOAD.length = appMsgSize;

    TEST_INITIALIZE_MEMORY_DEBUG(g_dllByDll);

    test_serialize_mutex = TEST_MUTEX_CREATE();
    ASSERT_IS_NOT_NULL(test_serialize_mutex);

    umock_c_init(on_umock_c_error);
    result = umocktypes_bool_register_types();
    ASSERT_ARE_EQUAL(int, 0, result);

    result = umocktypes_stdint_register_types();
    ASSERT_ARE_EQUAL(int, 0, result);

    REGISTER_UMOCK_ALIAS_TYPE(XIO_HANDLE, void*);
    REGISTER_UMOCK_ALIAS_TYPE(PDLIST_ENTRY, void*);
    REGISTER_UMOCK_ALIAS_TYPE(const PDLIST_ENTRY, void*);
    REGISTER_UMOCK_ALIAS_TYPE(MQTT_CLIENT_HANDLE, void*);
    REGISTER_UMOCK_ALIAS_TYPE(STRING_HANDLE, void*);
    REGISTER_UMOCK_ALIAS_TYPE(TICK_COUNTER_HANDLE, void*);
    REGISTER_UMOCK_ALIAS_TYPE(ON_MQTT_OPERATION_CALLBACK, void*);
    REGISTER_UMOCK_ALIAS_TYPE(IOTHUB_MESSAGE_HANDLE, void*);
    REGISTER_UMOCK_ALIAS_TYPE(QOS_VALUE, unsigned int);
    REGISTER_UMOCK_ALIAS_TYPE(MQTT_MESSAGE_HANDLE, void*);
    REGISTER_UMOCK_ALIAS_TYPE(ON_MQTT_MESSAGE_RECV_CALLBACK, void*);
    REGISTER_UMOCK_ALIAS_TYPE(MAP_HANDLE, void*);
    REGISTER_UMOCK_ALIAS_TYPE(STRING_TOKENIZER_HANDLE, void*);
    REGISTER_UMOCK_ALIAS_TYPE(IOTHUB_CLIENT_LL_HANDLE, void*);
    REGISTER_UMOCK_ALIAS_TYPE(IOTHUB_CLIENT_CONFIRMATION_RESULT, int);
    REGISTER_UMOCK_ALIAS_TYPE(IOTHUBMESSAGE_DISPOSITION_RESULT, int);

    REGISTER_GLOBAL_MOCK_HOOK(gballoc_malloc, my_gballoc_malloc);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(gballoc_malloc, NULL);
    REGISTER_GLOBAL_MOCK_HOOK(gballoc_free, my_gballoc_free);
    REGISTER_GLOBAL_MOCK_HOOK(gballoc_realloc, my_gballoc_realloc);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(gballoc_realloc, NULL);

    REGISTER_GLOBAL_MOCK_HOOK(mallocAndStrcpy_s, my_mallocAndStrcpy_s);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(mallocAndStrcpy_s, __LINE__);

    REGISTER_GLOBAL_MOCK_HOOK(STRING_new, my_STRING_new);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(STRING_new, NULL);
    REGISTER_GLOBAL_MOCK_HOOK(STRING_construct, my_STRING_construct);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(STRING_construct, NULL);
    REGISTER_GLOBAL_MOCK_HOOK(STRING_delete, my_STRING_delete);

    REGISTER_GLOBAL_MOCK_HOOK(STRING_c_str, my_STRING_c_str);
    
    REGISTER_GLOBAL_MOCK_RETURN(STRING_concat, 0);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(STRING_concat, __LINE__);

    REGISTER_GLOBAL_MOCK_RETURN(IoTHubClient_LL_MessageCallback, IOTHUBMESSAGE_ACCEPTED);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(STRING_concat, IOTHUBMESSAGE_REJECTED);

    REGISTER_GLOBAL_MOCK_HOOK(IoTHubClient_LL_SendComplete, my_IoTHubClient_LL_SendComplete);

    REGISTER_GLOBAL_MOCK_HOOK(IoTHubMessage_GetContentType, my_IoTHubMessage_GetContentType);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(IoTHubMessage_GetContentType, IOTHUBMESSAGE_UNKNOWN);

    REGISTER_GLOBAL_MOCK_RETURN(IoTHubMessage_GetString, appMessageString);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(IoTHubMessage_GetString, NULL);

    REGISTER_GLOBAL_MOCK_RETURN(IoTHubMessage_CreateFromByteArray, TEST_IOTHUB_MSG_BYTEARRAY);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(IoTHubMessage_CreateFromByteArray, NULL);

    REGISTER_GLOBAL_MOCK_HOOK(IoTHubMessage_GetByteArray, my_IoTHubMessage_GetByteArray);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(IoTHubMessage_GetByteArray, IOTHUB_MESSAGE_ERROR);

    REGISTER_GLOBAL_MOCK_HOOK(IoTHubMessage_Destroy, my_IoTHubMessage_Destroy);

    REGISTER_GLOBAL_MOCK_RETURN(IoTHubMessage_Properties, TEST_MESSAGE_PROP_MAP);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(IoTHubMessage_Properties, NULL);

    REGISTER_GLOBAL_MOCK_HOOK(Map_GetInternals, my_Map_GetInternals);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(Map_GetInternals, MAP_ERROR);

    REGISTER_GLOBAL_MOCK_RETURN(Map_AddOrUpdate, MAP_OK);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(Map_GetInternals, MAP_ERROR);

    REGISTER_GLOBAL_MOCK_HOOK(Map_Create, my_Map_Create);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(Map_Create, NULL);
    REGISTER_GLOBAL_MOCK_HOOK(Map_Clone, my_Map_Clone);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(Map_Clone, NULL);
    REGISTER_GLOBAL_MOCK_HOOK(Map_Destroy, my_Map_Destroy);

    REGISTER_GLOBAL_MOCK_HOOK(mqtt_client_init, my_mqtt_client_init);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(mqtt_client_init, NULL);

    REGISTER_GLOBAL_MOCK_RETURN(mqtt_client_connect, 0);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(mqtt_client_connect, __LINE__);

    REGISTER_GLOBAL_MOCK_HOOK(mqtt_client_deinit, my_mqtt_client_deinit);

    REGISTER_GLOBAL_MOCK_RETURN(mqtt_client_disconnect, 0);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(mqtt_client_disconnect, __LINE__);

    REGISTER_GLOBAL_MOCK_RETURN(mqtt_client_subscribe, 0);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(mqtt_client_subscribe, __LINE__);

    REGISTER_GLOBAL_MOCK_RETURN(mqtt_client_unsubscribe, 0);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(mqtt_client_unsubscribe, __LINE__);

    REGISTER_GLOBAL_MOCK_RETURN(mqtt_client_publish, 0);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(mqtt_client_publish, __LINE__);

    REGISTER_GLOBAL_MOCK_HOOK(mqtt_client_dowork, my_mqtt_client_dowork);
    //REGISTER_GLOBAL_MOCK_RETURN(mqtt_client_set_trace, 0);

    REGISTER_GLOBAL_MOCK_RETURN(mqttmessage_create, TEST_MQTT_MESSAGE_HANDLE);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(mqttmessage_create, NULL);

    REGISTER_GLOBAL_MOCK_RETURN(mqttmessage_getApplicationMsg, &TEST_APP_PAYLOAD);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(mqttmessage_getApplicationMsg, NULL);

    REGISTER_GLOBAL_MOCK_RETURN(mqttmessage_getTopicName, TEST_MQTT_MSG_TOPIC);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(mqttmessage_getTopicName, NULL);

    //REGISTER_GLOBAL_MOCK_RETURN(mqttmessage_destroy, 0);

    REGISTER_GLOBAL_MOCK_HOOK(STRING_TOKENIZER_create, my_STRING_TOKENIZER_create);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(STRING_TOKENIZER_create, NULL);

    REGISTER_GLOBAL_MOCK_HOOK(STRING_TOKENIZER_get_next_token, my_STRING_TOKENIZER_get_next_token);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(STRING_TOKENIZER_get_next_token, __LINE__);

    REGISTER_GLOBAL_MOCK_HOOK(STRING_TOKENIZER_destroy, my_STRING_TOKENIZER_destroy);
    
    REGISTER_GLOBAL_MOCK_HOOK(SASToken_Create, my_SASToken_Create);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(SASToken_Create, NULL);

    REGISTER_GLOBAL_MOCK_RETURN(get_time, TEST_TIME_T);

    REGISTER_GLOBAL_MOCK_RETURN(platform_get_default_tlsio, TEST_IO_INTERFACE);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(platform_get_default_tlsio, NULL);

    REGISTER_GLOBAL_MOCK_HOOK(xio_create, my_xio_create);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(platform_get_default_tlsio, NULL);

    REGISTER_GLOBAL_MOCK_RETURN(xio_close, 0);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(xio_close, __LINE__);

    REGISTER_GLOBAL_MOCK_RETURN(xio_setoption, 0);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(xio_setoption, __LINE__);

    REGISTER_GLOBAL_MOCK_HOOK(xio_destroy, my_xio_destroy);

    REGISTER_GLOBAL_MOCK_HOOK(tickcounter_create, my_tickcounter_create);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(tickcounter_create, NULL);

    REGISTER_GLOBAL_MOCK_HOOK(tickcounter_destroy, my_tickcounter_destroy);

    REGISTER_GLOBAL_MOCK_HOOK(tickcounter_get_current_ms, my_tickcounter_get_current_ms);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(tickcounter_get_current_ms, __LINE__);

    REGISTER_GLOBAL_MOCK_HOOK(DList_InitializeListHead, real_DList_InitializeListHead);
    REGISTER_GLOBAL_MOCK_HOOK(DList_IsListEmpty, real_DList_IsListEmpty);
    REGISTER_GLOBAL_MOCK_HOOK(DList_InsertTailList, real_DList_InsertTailList);
    REGISTER_GLOBAL_MOCK_HOOK(DList_InsertHeadList, real_DList_InsertHeadList);
    REGISTER_GLOBAL_MOCK_HOOK(DList_AppendTailList, real_DList_AppendTailList);
    REGISTER_GLOBAL_MOCK_HOOK(DList_RemoveEntryList, real_DList_RemoveEntryList);
    REGISTER_GLOBAL_MOCK_HOOK(DList_RemoveHeadList, real_DList_RemoveHeadList);

    IoTHubTransportMqtt_GetHostname = ((TRANSPORT_PROVIDER*)MQTT_Protocol())->IoTHubTransport_GetHostname;
    IoTHubTransportMqtt_SetOption = ((TRANSPORT_PROVIDER*)MQTT_Protocol())->IoTHubTransport_SetOption;
    IoTHubTransportMqtt_Create = ((TRANSPORT_PROVIDER*)MQTT_Protocol())->IoTHubTransport_Create;
    IoTHubTransportMqtt_Destroy = ((TRANSPORT_PROVIDER*)MQTT_Protocol())->IoTHubTransport_Destroy;
    IoTHubTransportMqtt_Register = ((TRANSPORT_PROVIDER*)MQTT_Protocol())->IoTHubTransport_Register;
    IoTHubTransportMqtt_Unregister = ((TRANSPORT_PROVIDER*)MQTT_Protocol())->IoTHubTransport_Unregister;
    IoTHubTransportMqtt_Subscribe = ((TRANSPORT_PROVIDER*)MQTT_Protocol())->IoTHubTransport_Subscribe;
    IoTHubTransportMqtt_Unsubscribe = ((TRANSPORT_PROVIDER*)MQTT_Protocol())->IoTHubTransport_Unsubscribe;
    IoTHubTransportMqtt_DoWork = ((TRANSPORT_PROVIDER*)MQTT_Protocol())->IoTHubTransport_DoWork;
    IoTHubTransportMqtt_GetSendStatus = ((TRANSPORT_PROVIDER*)MQTT_Protocol())->IoTHubTransport_GetSendStatus;
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

    g_fnMqttMsgRecv = NULL;
    g_fnMqttOperationCallback = NULL;
    g_callbackCtx = NULL;

    g_current_ms = 0;
    g_tokenizerIndex = 0;
    g_nullMapVariable = true;

    real_DList_InitializeListHead(&g_waitingToSend);

    umock_c_reset_all_calls();
}

TEST_FUNCTION_CLEANUP(TestMethodCleanup)
{
    TEST_MUTEX_RELEASE(test_serialize_mutex);
}

static int should_skip_index(size_t current_index, const size_t skip_array[], size_t length)
{
    int result = 0;
    for (size_t index = 0; index < length; index++)
    {
        if (current_index == skip_array[index])
        {
            result = __LINE__;
            break;
        }
    }
    return result;
}

static void SetupIothubTransportConfig(IOTHUBTRANSPORT_CONFIG* config, const char* deviceId, const char* deviceKey, const char* iotHubName,
    const char* iotHubSuffix, const char* protocolGatewayHostName)
{
    g_iothubClientConfig.protocol = MQTT_Protocol;
    g_iothubClientConfig.deviceId = deviceId;
    g_iothubClientConfig.deviceKey = deviceKey;
    g_iothubClientConfig.deviceSasToken = NULL;
    g_iothubClientConfig.iotHubName = iotHubName;
    g_iothubClientConfig.iotHubSuffix = iotHubSuffix;
    g_iothubClientConfig.protocolGatewayHostName = protocolGatewayHostName;
    config->waitingToSend = &g_waitingToSend;
    config->upperConfig = &g_iothubClientConfig;
}

static void SetupIothubTransportConfigWithKeyAndSasToken(IOTHUBTRANSPORT_CONFIG* config, const char* deviceId, const char* deviceKey, const char* deviceSasToken,
    const char* iotHubName, const char* iotHubSuffix, const char* protocolGatewayHostName)
{
    g_iothubClientConfig.protocol = MQTT_Protocol;
    g_iothubClientConfig.deviceId = deviceId;
    g_iothubClientConfig.deviceKey = deviceKey;
    g_iothubClientConfig.deviceSasToken = deviceSasToken;
    g_iothubClientConfig.iotHubName = iotHubName;
    g_iothubClientConfig.iotHubSuffix = iotHubSuffix;
    g_iothubClientConfig.protocolGatewayHostName = protocolGatewayHostName;
    config->waitingToSend = &g_waitingToSend;
    config->upperConfig = &g_iothubClientConfig;
}

static void setup_iothubtransportmqtt_create_mocks()
{
    STRICT_EXPECTED_CALL(tickcounter_create());

    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));
    STRICT_EXPECTED_CALL(STRING_construct(TEST_DEVICE_ID));
    STRICT_EXPECTED_CALL(STRING_construct(TEST_DEVICE_KEY));

    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(STRING_construct(IGNORED_PTR_ARG));
    EXPECTED_CALL(gballoc_free(NULL));

    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(STRING_construct(IGNORED_PTR_ARG));
    EXPECTED_CALL(gballoc_free(NULL));

    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(STRING_construct(IGNORED_PTR_ARG));
    EXPECTED_CALL(gballoc_free(NULL));

    EXPECTED_CALL(mqtt_client_init(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));

    STRICT_EXPECTED_CALL(STRING_construct(TEST_MQTT_MESSAGE_TOPIC)).IgnoreArgument(1);

    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));
    STRICT_EXPECTED_CALL(STRING_construct(TEST_MQTT_MESSAGE_TOPIC)).IgnoreArgument(1);
    EXPECTED_CALL(gballoc_free(NULL));

    EXPECTED_CALL(DList_InitializeListHead(IGNORED_PTR_ARG));
}

static void setup_initialize_connection_mocks()
{
    STRICT_EXPECTED_CALL(tickcounter_get_current_ms(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(STRING_new());
    STRICT_EXPECTED_CALL(get_time(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    EXPECTED_CALL(SASToken_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_NUM_ARG));
    EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG)).SetReturn(TEST_DEVICE_ID);
    EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG)).SetReturn(TEST_STRING_VALUE);
    EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG)).SetReturn(TEST_SAS_TOKEN);
    EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG)).SetReturn(TEST_HOST_NAME);
    STRICT_EXPECTED_CALL(platform_get_default_tlsio());
    STRICT_EXPECTED_CALL(xio_create(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    EXPECTED_CALL(mqtt_client_connect(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(tickcounter_get_current_ms(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG));
    EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(tickcounter_get_current_ms(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
}

static void setup_iothubtransportmqtt_dowork_mocks()
{
    setup_initialize_connection_mocks();

    EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG)).SetReturn(TEST_MQTT_MSG_TOPIC);
    EXPECTED_CALL(mqtt_client_subscribe(IGNORED_PTR_ARG, IGNORED_NUM_ARG, IGNORED_PTR_ARG, IGNORED_NUM_ARG));
    STRICT_EXPECTED_CALL(mqtt_client_dowork(TEST_MQTT_CLIENT_HANDLE))
        .IgnoreArgument(1);
}

static void setup_iothubtransportmqtt_dowork_events_mocks(const char* const** ppKeys, const char* const** ppValues, size_t propCount)
{
    STRICT_EXPECTED_CALL(tickcounter_get_current_ms(TEST_COUNTER_HANDLE, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(IoTHubMessage_GetContentType(TEST_IOTHUB_MSG_BYTEARRAY));
    STRICT_EXPECTED_CALL(IoTHubMessage_GetByteArray(TEST_IOTHUB_MSG_BYTEARRAY, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(2)
        .IgnoreArgument(3);
    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(STRING_construct(TEST_MQTT_EVENT_TOPIC)).IgnoreArgument(1);
    STRICT_EXPECTED_CALL(IoTHubMessage_Properties(TEST_IOTHUB_MSG_BYTEARRAY));
    if (propCount == 0)
    {
        EXPECTED_CALL(Map_GetInternals(TEST_MESSAGE_PROP_MAP, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    }
    else
    {
        STRICT_EXPECTED_CALL(Map_GetInternals(TEST_MESSAGE_PROP_MAP, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .CopyOutArgumentBuffer(2, &ppKeys, sizeof(ppKeys))
            .CopyOutArgumentBuffer(3, &ppValues, sizeof(ppValues))
            .CopyOutArgumentBuffer(4, &propCount, sizeof(propCount));
        for (size_t index = 0; index < propCount; index++)
        {
            EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));
            STRICT_EXPECTED_CALL(STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreArgument(1)
                .IgnoreArgument(2);
            EXPECTED_CALL(gballoc_free(NULL));
        }
    }
    EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG));
    EXPECTED_CALL(mqttmessage_create(IGNORED_NUM_ARG, IGNORED_PTR_ARG, DELIVER_AT_LEAST_ONCE, appMessage, appMsgSize))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mqtt_client_publish(TEST_MQTT_CLIENT_HANDLE, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(tickcounter_get_current_ms(TEST_COUNTER_HANDLE, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mqttmessage_destroy(TEST_MQTT_MESSAGE_HANDLE))
        .IgnoreArgument(1);
    EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG));
    EXPECTED_CALL(DList_RemoveEntryList(IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(DList_InsertTailList(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    EXPECTED_CALL(mqtt_client_dowork(IGNORED_PTR_ARG));
}

static void setup_message_recv_msg_callback_mocks(IOTHUBMESSAGE_DISPOSITION_RESULT msg_disposition)
{
    STRICT_EXPECTED_CALL(mqttmessage_getApplicationMsg(TEST_MQTT_MESSAGE_HANDLE));
    STRICT_EXPECTED_CALL(IoTHubMessage_CreateFromByteArray(appMessage, appMsgSize));
    STRICT_EXPECTED_CALL(mqttmessage_getTopicName(TEST_MQTT_MESSAGE_HANDLE));

    STRICT_EXPECTED_CALL(STRING_construct(TEST_MQTT_MSG_TOPIC));
    EXPECTED_CALL(STRING_TOKENIZER_create(IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(IoTHubMessage_Properties(TEST_IOTHUB_MSG_BYTEARRAY));
    STRICT_EXPECTED_CALL(STRING_new());
    STRICT_EXPECTED_CALL(STRING_TOKENIZER_get_next_token(IGNORED_PTR_ARG, IGNORED_PTR_ARG, "&"))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(3);
    EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG));
    EXPECTED_CALL(STRING_TOKENIZER_destroy(IGNORED_PTR_ARG));
    EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(IoTHubClient_LL_MessageCallback(TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_IOTHUB_MSG_BYTEARRAY))
        .SetReturn(msg_disposition);
    STRICT_EXPECTED_CALL(IoTHubMessage_Destroy(TEST_IOTHUB_MSG_BYTEARRAY));
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_001: [If parameter config is NULL then IoTHubTransportMqtt_Create shall return NULL.] */
TEST_FUNCTION(IoTHubTransportMqtt_Create_with_NULL_parameter_Succeed)
{
    //arrange

    //act
    TRANSPORT_LL_HANDLE result = IoTHubTransportMqtt_Create(NULL);

    //assert
    ASSERT_IS_NULL(result);

    //cleanup
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_002: [If the parameter config's variables upperConfig or waitingToSend are NULL then IoTHubTransportMqtt_Create shall return NULL.] */
TEST_FUNCTION(IoTHubTransportMqtt_Create_with_NULL_config_parameter_fails)
{
    //arrange
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    config.waitingToSend = &g_waitingToSend;

    //act
    TRANSPORT_LL_HANDLE result = IoTHubTransportMqtt_Create(&config);

    //assert
    ASSERT_IS_NULL(result);

    //cleanup
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_002: [If the parameter config's variables upperConfig or waitingToSend are NULL then IoTHubTransportMqtt_Create shall return NULL.] */
TEST_FUNCTION(IoTHubTransportMqtt_Create_with_NULL_waitingToSend_fails)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);
    config.waitingToSend = NULL;

    // act
    TRANSPORT_LL_HANDLE result = IoTHubTransportMqtt_Create(&config);

    // assert
    ASSERT_IS_NULL(result);
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_009: [If any error is encountered then IoTHubTransportMqtt_Create shall return NULL.] */
TEST_FUNCTION(IoTHubTransportMqtt_Create_with_NULL_protocol_fails)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);
    g_iothubClientConfig.protocol = NULL;

    // act
    TRANSPORT_LL_HANDLE result = IoTHubTransportMqtt_Create(&config);

    // assert
    ASSERT_IS_NULL(result);
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_003: [If the upperConfig's variables deviceId, deviceKey, iotHubName, protocol, or iotHubSuffix are NULL then IoTHubTransportMqtt_Create shall return NULL.] */
TEST_FUNCTION(IoTHubTransportMqtt_Create_with_NULL_device_id_fails)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, NULL, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);

    // act
    TRANSPORT_LL_HANDLE result = IoTHubTransportMqtt_Create(&config);

    // assert
    ASSERT_IS_NULL(result);
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_005: [If the upperConfig's variables deviceKey, iotHubName, or iotHubSuffix are empty strings then IoTHubTransportMqtt_Create shall return NULL.] */
TEST_FUNCTION(IoTHubTransportMqtt_Create_with_deviceSasToken_blank_fails)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfigWithKeyAndSasToken(&config, TEST_DEVICE_ID, NULL, TEST_EMPTY_STRING, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);

    // act
    TRANSPORT_LL_HANDLE result = IoTHubTransportMqtt_Create(&config);

    // assert
    ASSERT_IS_NULL(result);
}

TEST_FUNCTION(IoTHubTransportMqtt_Create_with_deviceKey_blank_fails)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config ={ 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, "", TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);

    // act
    TRANSPORT_LL_HANDLE result = IoTHubTransportMqtt_Create(&config);

    // assert
    ASSERT_IS_NULL(result);
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_003: [If the upperConfig's variables deviceId, deviceKey, iotHubName, protocol, or iotHubSuffix are NULL then IoTHubTransportMqtt_Create shall return NULL.] */
TEST_FUNCTION(IoTHubTransportMqtt_Create_with_NULL_iothub_name_fails)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, NULL, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);

    // act
    TRANSPORT_LL_HANDLE result = IoTHubTransportMqtt_Create(&config);

    // assert
    ASSERT_IS_NULL(result);
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_006: [If the upperConfig's variables deviceId is an empty strings or length is greater then 128 then IoTHubTransportMqtt_Create shall return NULL.] */
TEST_FUNCTION(IoTHubTransportMqtt_Create_with_very_long_device_id_fails)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config ={ 0 };
    SetupIothubTransportConfig(&config, TEST_VERY_LONG_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);

    // act
    TRANSPORT_LL_HANDLE result = IoTHubTransportMqtt_Create(&config);

    // assert
    ASSERT_IS_NULL(result);
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_003: [If the upperConfig's variables deviceId, deviceKey, iotHubName, protocol, or iotHubSuffix are NULL then IoTHubTransportMqtt_Create shall return NULL.] */
TEST_FUNCTION(IoTHubTransportMqtt_Create_with_empty_device_id_fails)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config ={ 0 };
    SetupIothubTransportConfig(&config, TEST_EMPTY_STRING, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);

    // act
    TRANSPORT_LL_HANDLE result = IoTHubTransportMqtt_Create(&config);

    // assert
    ASSERT_IS_NULL(result);
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_003: [If the upperConfig's variables deviceId, deviceKey, iotHubName, protocol, or iotHubSuffix are NULL then IoTHubTransportMqtt_Create shall return NULL.] */
TEST_FUNCTION(IoTHubTransportMqtt_Create_with_empty_device_key_fails)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config ={ 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_EMPTY_STRING, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);

    // act
    TRANSPORT_LL_HANDLE result = IoTHubTransportMqtt_Create(&config);

    // assert
    ASSERT_IS_NULL(result);
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_03_003: [If both deviceKey & deviceSasToken fields are NOT NULL then IoTHubTransportMqtt_Create shall return NULL.]*/
TEST_FUNCTION(IoTHubTransportMqtt_Create_with_both_deviceKey_and_deviceSasToken_defined_fails)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config ={ 0 };

    SetupIothubTransportConfigWithKeyAndSasToken(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_DEVICE_SAS, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);

    // act
    TRANSPORT_LL_HANDLE result = IoTHubTransportMqtt_Create(&config);

    // assert
    ASSERT_IS_NULL(result);
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_003: [If the upperConfig's variables deviceId, deviceKey, iotHubName, protocol, or iotHubSuffix are NULL then IoTHubTransportMqtt_Create shall return NULL.] */
TEST_FUNCTION(IoTHubTransportMqtt_Create_with_empty_iothub_name_fails)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config ={ 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_EMPTY_STRING, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);

    // act
    TRANSPORT_LL_HANDLE result = IoTHubTransportMqtt_Create(&config);

    // assert
    ASSERT_IS_NULL(result);
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_010: [IoTHubTransportMqtt_Create shall allocate memory to save its internal state where all topics, hostname, device_id, device_key, sasTokenSr and client handle shall be saved.] */
/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_011: [On Success IoTHubTransportMqtt_Create shall return a non-NULL value.] */
TEST_FUNCTION(IoTHubTransportMqtt_Create_validConfig_Succeed)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config ={ 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    setup_iothubtransportmqtt_create_mocks();

    // act
    TRANSPORT_LL_HANDLE result = IoTHubTransportMqtt_Create(&config);

    // assert
    ASSERT_IS_NOT_NULL(result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // clean up
    IoTHubTransportMqtt_Destroy(result);
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_007: [If the upperConfig's variables protocolGatewayHostName is non-Null and the length is an empty string then IoTHubTransportMqtt_Create shall return NULL.] */
TEST_FUNCTION(IoTHubTransportMqtt_Create_with_NULL_protocol_gateway_hostname_Succeeds)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config ={ 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    setup_iothubtransportmqtt_create_mocks();

    // act
    TRANSPORT_LL_HANDLE result = IoTHubTransportMqtt_Create(&config);

    // assert
    ASSERT_IS_NOT_NULL(result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // clean up
    IoTHubTransportMqtt_Destroy(result);
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_003: [If the upperConfig's variables deviceId, deviceKey, iotHubName, protocol, or iotHubSuffix are NULL then IoTHubTransportMqtt_Create shall return NULL.] */
/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_009: [If any error is encountered then IoTHubTransportMqtt_Create shall return NULL.] */
/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_011: [On Success IoTHubTransportMqtt_Create shall return a non-NULL value.] */
TEST_FUNCTION(IoTHubTransportMqtt_Create_validConfig_fail)
{
    // arrange
    int negativeTestsInitResult = umock_c_negative_tests_init();
    ASSERT_ARE_EQUAL(int, 0, negativeTestsInitResult);

    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);
    umock_c_reset_all_calls();

    setup_iothubtransportmqtt_create_mocks();

    umock_c_negative_tests_snapshot();

    size_t calls_cannot_fail[] = { 6, 9, 12, 15, 17, 18 };

    // act
    size_t count = umock_c_negative_tests_call_count();
    for (size_t index = 0; index < count; index++)
    {
        if (should_skip_index(index, calls_cannot_fail, sizeof(calls_cannot_fail)/sizeof(calls_cannot_fail[0])) != 0)
        {
            continue;
        }

        umock_c_negative_tests_reset();
        umock_c_negative_tests_fail_call(index);

        char tmp_msg[64];
        sprintf(tmp_msg, "IoTHubTransportMqtt_Create failure in test %zu/%zu", index, count);

        TRANSPORT_LL_HANDLE result = IoTHubTransportMqtt_Create(&config);

        // assert
        ASSERT_IS_NULL_WITH_MSG(result, tmp_msg);
    }

    // clean up
    umock_c_negative_tests_deinit();
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_012: [IoTHubTransportMqtt_Destroy shall do nothing if parameter handle is NULL.] */
TEST_FUNCTION(IoTHubTransportMqtt_Destroy_parameter_NULL_succeed)
{
    // arrange

    // act
    IoTHubTransportMqtt_Destroy(NULL);

    // assert
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_013: [If the parameter subscribe is true then IoTHubTransportMqtt_Destroy shall call IoTHubTransportMqtt_Unsubscribe.] */
TEST_FUNCTION(IoTHubTransportMqtt_Destroy_Unsubscribe_succeeds)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config ={ 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);

    TRANSPORT_LL_HANDLE handle = IoTHubTransportMqtt_Create(&config);
    (void)IoTHubTransportMqtt_Subscribe(handle);
    CONNECT_ACK connack ={ true, CONNECTION_ACCEPTED };
    g_fnMqttOperationCallback(TEST_MQTT_CLIENT_HANDLE, MQTT_CLIENT_ON_CONNACK, &connack, g_callbackCtx);
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);
    umock_c_reset_all_calls();

    EXPECTED_CALL(STRING_c_str(NULL));

    EXPECTED_CALL(DList_IsListEmpty(IGNORED_PTR_ARG));

    EXPECTED_CALL(STRING_delete(NULL));
    EXPECTED_CALL(STRING_delete(NULL));
    EXPECTED_CALL(STRING_delete(NULL));
    EXPECTED_CALL(STRING_delete(NULL));
    EXPECTED_CALL(STRING_delete(NULL));
    EXPECTED_CALL(STRING_delete(NULL));
    EXPECTED_CALL(STRING_delete(NULL));

    STRICT_EXPECTED_CALL(mqtt_client_disconnect(TEST_MQTT_CLIENT_HANDLE));
    STRICT_EXPECTED_CALL(mqtt_client_deinit(TEST_MQTT_CLIENT_HANDLE));
    STRICT_EXPECTED_CALL(mqtt_client_unsubscribe(TEST_MQTT_CLIENT_HANDLE, IGNORED_NUM_ARG, IGNORED_PTR_ARG, 1))
        .IgnoreArgument(2)
        .IgnoreArgument(3);
    EXPECTED_CALL(gballoc_free(NULL));
    STRICT_EXPECTED_CALL(xio_destroy(TEST_XIO_HANDLE));
    STRICT_EXPECTED_CALL(tickcounter_destroy(TEST_COUNTER_HANDLE));

    // act
    IoTHubTransportMqtt_Destroy(handle);

    // assert
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_014: [IoTHubTransportMqtt_Destroy shall free all the resources currently in use.] */
TEST_FUNCTION(IoTHubTransportMqtt_Destroy_One_Message_Ack_succeeds)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config ={ 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);

    QOS_VALUE QosValue[] ={ DELIVER_AT_LEAST_ONCE };
    SUBSCRIBE_ACK suback;
    suback.packetId = 1234;
    suback.qosCount = 1;
    suback.qosReturn = QosValue;

    IOTHUB_MESSAGE_LIST message1;
    memset(&message1, 0, sizeof(IOTHUB_MESSAGE_LIST) );
    message1.messageHandle = TEST_IOTHUB_MSG_BYTEARRAY;

    DList_InsertTailList(config.waitingToSend, &(message1.entry));
    TRANSPORT_LL_HANDLE handle = IoTHubTransportMqtt_Create(&config);
    g_fnMqttOperationCallback(TEST_MQTT_CLIENT_HANDLE, MQTT_CLIENT_ON_SUBSCRIBE_ACK, &suback, g_callbackCtx);
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(mqtt_client_disconnect(TEST_MQTT_CLIENT_HANDLE))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(xio_destroy(TEST_XIO_HANDLE))
        .IgnoreArgument(1);
    EXPECTED_CALL(DList_IsListEmpty(IGNORED_PTR_ARG));
    EXPECTED_CALL(DList_RemoveHeadList(IGNORED_PTR_ARG));
    EXPECTED_CALL(DList_InitializeListHead(IGNORED_PTR_ARG));
    EXPECTED_CALL(DList_InsertTailList(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(IoTHubClient_LL_SendComplete(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IOTHUB_CLIENT_CONFIRMATION_BECAUSE_DESTROY))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    EXPECTED_CALL(gballoc_free(NULL));
    EXPECTED_CALL(DList_IsListEmpty(IGNORED_PTR_ARG));
    EXPECTED_CALL(STRING_delete(NULL));
    EXPECTED_CALL(STRING_delete(NULL));
    STRICT_EXPECTED_CALL(mqtt_client_deinit(TEST_MQTT_CLIENT_HANDLE)).IgnoreArgument(1);
    EXPECTED_CALL(STRING_delete(NULL));
    EXPECTED_CALL(STRING_delete(NULL));
    EXPECTED_CALL(STRING_delete(NULL));
    EXPECTED_CALL(STRING_delete(NULL));
    EXPECTED_CALL(STRING_delete(NULL));
    STRICT_EXPECTED_CALL(tickcounter_destroy(TEST_COUNTER_HANDLE)).IgnoreArgument(1);
    EXPECTED_CALL(gballoc_free(NULL));

    // act
    IoTHubTransportMqtt_Destroy(handle);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_014: [IoTHubTransportMqtt_Destroy shall free all the resources currently in use.] */
TEST_FUNCTION(IoTHubTransportMqtt_Destroy_succeeds)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config ={ 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);

    TRANSPORT_LL_HANDLE handle = IoTHubTransportMqtt_Create(&config);
    umock_c_reset_all_calls();

    EXPECTED_CALL(DList_IsListEmpty(IGNORED_PTR_ARG));

    EXPECTED_CALL(STRING_delete(NULL));
    EXPECTED_CALL(STRING_delete(NULL));
    EXPECTED_CALL(STRING_delete(NULL));
    EXPECTED_CALL(STRING_delete(NULL));
    EXPECTED_CALL(STRING_delete(NULL));
    EXPECTED_CALL(STRING_delete(NULL));
    EXPECTED_CALL(STRING_delete(NULL));

    STRICT_EXPECTED_CALL(mqtt_client_deinit(TEST_MQTT_CLIENT_HANDLE));
    STRICT_EXPECTED_CALL(mqtt_client_disconnect(TEST_MQTT_CLIENT_HANDLE));
    EXPECTED_CALL(xio_destroy(NULL));
    EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(tickcounter_destroy(IGNORED_PTR_ARG)).IgnoreArgument(1);

    // act
    IoTHubTransportMqtt_Destroy(handle);

    // assert
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_015: [If parameter handle is NULL than IoTHubTransportMqtt_Subscribe shall return a non-zero value.] */
TEST_FUNCTION(IoTHubTransportMqtt_Subscribe_parameter_NULL_fail)
{
    // arrange

    // act
    int res = IoTHubTransportMqtt_Subscribe(NULL);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, res);
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_016: [IoTHubTransportMqtt_Subscribe shall set a flag to enable mqtt_client_subscribe to be called to subscribe to the Message Topic.] */
/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_018: [On success IoTHubTransportMqtt_Subscribe shall return 0.] */
TEST_FUNCTION(IoTHubTransportMqtt_Subscribe_Succeed)
{
    // arrange
    CONNECT_ACK connack = { true, CONNECTION_ACCEPTED };
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);

    TRANSPORT_LL_HANDLE handle = IoTHubTransportMqtt_Create(&config);

    g_fnMqttOperationCallback(TEST_MQTT_CLIENT_HANDLE, MQTT_CLIENT_ON_CONNACK, &connack, g_callbackCtx);
    umock_c_reset_all_calls();

    setup_iothubtransportmqtt_dowork_mocks();

    // act
    int result = IoTHubTransportMqtt_Subscribe(handle);
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // assert
    ASSERT_ARE_EQUAL(int, result, 0);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_017: [Upon failure IoTHubTransportMqtt_Subscribe shall return a non-zero value.] */
TEST_FUNCTION(IoTHubTransportMqtt_Subscribe_fail)
{
    // arrange
    int negativeTestsInitResult = umock_c_negative_tests_init();
    ASSERT_ARE_EQUAL(int, 0, negativeTestsInitResult);

    CONNECT_ACK connack = { true, CONNECTION_ACCEPTED };
    IOTHUBTRANSPORT_CONFIG config ={ 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);

    TRANSPORT_LL_HANDLE handle = IoTHubTransportMqtt_Create(&config);

    g_fnMqttOperationCallback(TEST_MQTT_CLIENT_HANDLE, MQTT_CLIENT_ON_CONNACK, &connack, g_callbackCtx);
    umock_c_reset_all_calls();

    setup_iothubtransportmqtt_dowork_mocks();

    umock_c_negative_tests_snapshot();

    size_t calls_cannot_fail[] = { 1, 2 };

    // act
    size_t count = umock_c_negative_tests_call_count();
    for (size_t index = 0; index < count; index++)
    {
        if (should_skip_index(index, calls_cannot_fail, sizeof(calls_cannot_fail)/sizeof(calls_cannot_fail[0])) != 0)
        {
            continue;
        }

        umock_c_negative_tests_reset();
        umock_c_negative_tests_fail_call(index);

        char tmp_msg[64];
        sprintf(tmp_msg, "IoTHubTransportMqtt_DoWork failure in test %zu/%zu", index, count);
        
        int result = IoTHubTransportMqtt_Subscribe(handle);
        IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

        // assert
        ASSERT_ARE_EQUAL_WITH_MSG(int, result, 0, tmp_msg);
    }

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
    umock_c_negative_tests_deinit();
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_017: [Upon failure IoTHubTransportMqtt_Subscribe shall return a non-zero value.] */
TEST_FUNCTION(IoTHubTransportMqtt_Subscribe_set_subscribe_type_Succeed)
{
    // arrange
    CONNECT_ACK connack ={ true, CONNECTION_ACCEPTED };
    IOTHUBTRANSPORT_CONFIG config ={ 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);

    TRANSPORT_LL_HANDLE handle = IoTHubTransportMqtt_Create(&config);

    g_fnMqttOperationCallback(TEST_MQTT_CLIENT_HANDLE, MQTT_CLIENT_ON_CONNACK, &connack, g_callbackCtx);
    umock_c_reset_all_calls();

    setup_iothubtransportmqtt_dowork_mocks();

    // act
    int result = IoTHubTransportMqtt_Subscribe(handle);
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // assert
    ASSERT_ARE_EQUAL(int, result, 0);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

TEST_FUNCTION(IoTHubTransportMqtt_Subscribe_set_subscribe_after_publish_Succeed)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);

    QOS_VALUE QosValue[] = { DELIVER_AT_LEAST_ONCE };
    SUBSCRIBE_ACK suback;
    suback.packetId = 1234;
    suback.qosCount = 1;
    suback.qosReturn = QosValue;

    TRANSPORT_LL_HANDLE handle = IoTHubTransportMqtt_Create(&config);

    g_fnMqttOperationCallback(TEST_MQTT_CLIENT_HANDLE, MQTT_CLIENT_ON_SUBSCRIBE_ACK, &suback, g_callbackCtx);
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(tickcounter_get_current_ms(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mqtt_client_dowork(TEST_MQTT_CLIENT_HANDLE))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(tickcounter_get_current_ms(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    EXPECTED_CALL(STRING_c_str(NULL)).SetReturn(TEST_MQTT_MESSAGE_TOPIC);
    STRICT_EXPECTED_CALL(mqtt_client_subscribe(TEST_MQTT_CLIENT_HANDLE, IGNORED_NUM_ARG, IGNORED_PTR_ARG, 1))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(3);
    STRICT_EXPECTED_CALL(mqtt_client_dowork(TEST_MQTT_CLIENT_HANDLE))
        .IgnoreArgument(1);

    // act
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);
    int result = IoTHubTransportMqtt_Subscribe(handle);
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // assert
    ASSERT_ARE_EQUAL(int, result, 0);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_019: [If parameter handle is NULL then IoTHubTransportMqtt_Unsubscribe shall do nothing.] */
TEST_FUNCTION(IoTHubTransportMqtt_Unsubscribe_parameter_NULL_fail)
{
    // arrange

    // act
    IoTHubTransportMqtt_Unsubscribe(NULL);

    // assert
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_020: [IoTHubTransportMqtt_Unsubscribe shall call mqtt_client_unsubscribe to unsubscribe the mqtt message topic.] */
TEST_FUNCTION(IoTHubTransportMqtt_Unsubscribe_Succeeds)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);

    TRANSPORT_LL_HANDLE handle = IoTHubTransportMqtt_Create(&config);
    IoTHubTransportMqtt_Subscribe(handle);

    CONNECT_ACK connack = { true, CONNECTION_ACCEPTED };
    g_fnMqttOperationCallback(TEST_MQTT_CLIENT_HANDLE, MQTT_CLIENT_ON_CONNACK, &connack, g_callbackCtx);

    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    umock_c_reset_all_calls();

    EXPECTED_CALL(STRING_c_str(NULL)).SetReturn(TEST_MQTT_MESSAGE_TOPIC);
    EXPECTED_CALL(mqtt_client_unsubscribe(TEST_MQTT_CLIENT_HANDLE, IGNORED_NUM_ARG, IGNORED_PTR_ARG, 1));

    // act
    IoTHubTransportMqtt_Unsubscribe(handle);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_032: [IoTHubTransportMqtt_SetOption shall pass down the option to xio_setoption if the option parameter is not a known option string for the MQTT transport.] */
TEST_FUNCTION(IoTHubTransportMqtt_Setoption_invokes_xio_setoption_when_option_not_consumed_by_mqtt_transport)
{
    // arrange
    const char* SOME_OPTION = "AnOption";
    const void* SOME_VALUE = (void*)42;

    IOTHUBTRANSPORT_CONFIG config ={ 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);

    TRANSPORT_LL_HANDLE handle = IoTHubTransportMqtt_Create(&config);
    umock_c_reset_all_calls();

    EXPECTED_CALL(STRING_c_str(NULL));
    EXPECTED_CALL(platform_get_default_tlsio());
    EXPECTED_CALL(xio_create(NULL, NULL));
    STRICT_EXPECTED_CALL(xio_setoption(NULL, SOME_OPTION, SOME_VALUE))
        .IgnoreArgument(1);

    // act
    IOTHUB_CLIENT_RESULT result = IoTHubTransportMqtt_SetOption(handle, SOME_OPTION, SOME_VALUE);

    // assert
    ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_021: [If any parameter is NULL then IoTHubTransportMqtt_SetOption shall return IOTHUB_CLIENT_INVALID_ARG.] */
TEST_FUNCTION(IoTHubTransportMqtt_Setoption_option_NULL_fail)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config ={ 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);

    TRANSPORT_LL_HANDLE handle = IoTHubTransportMqtt_Create(&config);
    umock_c_reset_all_calls();

    bool traceOn = true;

    // act
    IOTHUB_CLIENT_RESULT result = IoTHubTransportMqtt_SetOption(handle, NULL, &traceOn);

    // assert
    ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_INVALID_ARG, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_021: [If any parameter is NULL then IoTHubTransportMqtt_SetOption shall return IOTHUB_CLIENT_INVALID_ARG.] */
TEST_FUNCTION(IoTHubTransportMqtt_Setoption_value_NULL_fail)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config ={ 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);

    TRANSPORT_LL_HANDLE handle = IoTHubTransportMqtt_Create(&config);
    umock_c_reset_all_calls();

    // act
    IOTHUB_CLIENT_RESULT result = IoTHubTransportMqtt_SetOption(handle, OPTION_LOG_TRACE, NULL);

    // assert
    ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_INVALID_ARG, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_039: [If the option parameter is set to "x509certificate" then the value shall be a const char of the certificate to be used for x509.] */
TEST_FUNCTION(IoTHubTransportMqtt_Setoption_x509Certificate_no_509_fail)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config ={ 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);

    TRANSPORT_LL_HANDLE handle = IoTHubTransportMqtt_Create(&config);
    umock_c_reset_all_calls();

    // act
    IOTHUB_CLIENT_RESULT result = IoTHubTransportMqtt_SetOption(handle, OPTION_X509_CERT, X509_CERT_CERTIFICATE);

    // assert
    ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_INVALID_ARG, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

TEST_FUNCTION(IoTHubTransportMqtt_Setoption_x509Private_key_no_509_fail)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config ={ 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);

    TRANSPORT_LL_HANDLE handle = IoTHubTransportMqtt_Create(&config);
    umock_c_reset_all_calls();

    // act
    IOTHUB_CLIENT_RESULT result = IoTHubTransportMqtt_SetOption(handle, X509_PRIVATE_KEY_OPTION, X509_PRIVATE_KEY);

    // assert
    ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_INVALID_ARG, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

TEST_FUNCTION(IoTHubTransportMqtt_Setoption_x509Certificate_succeed)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config ={ 0 };
    SetupIothubTransportConfigWithKeyAndSasToken(&config, TEST_DEVICE_ID, NULL, NULL, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);

    TRANSPORT_LL_HANDLE handle = IoTHubTransportMqtt_Create(&config);
    umock_c_reset_all_calls();

    EXPECTED_CALL(STRING_c_str(NULL));
    EXPECTED_CALL(platform_get_default_tlsio());
    EXPECTED_CALL(xio_create(NULL, NULL));
    STRICT_EXPECTED_CALL(xio_setoption(NULL, OPTION_X509_CERT, X509_CERT_CERTIFICATE))
        .IgnoreArgument(1);

    // act
    IOTHUB_CLIENT_RESULT result = IoTHubTransportMqtt_SetOption(handle, OPTION_X509_CERT, X509_CERT_CERTIFICATE);

    // assert
    ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

TEST_FUNCTION(IoTHubTransportMqtt_Setoption_x509Private_key_succeed)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config ={ 0 };
    SetupIothubTransportConfigWithKeyAndSasToken(&config, TEST_DEVICE_ID, NULL, NULL, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);

    TRANSPORT_LL_HANDLE handle = IoTHubTransportMqtt_Create(&config);
    umock_c_reset_all_calls();

    EXPECTED_CALL(STRING_c_str(NULL));
    EXPECTED_CALL(platform_get_default_tlsio());
    EXPECTED_CALL(xio_create(NULL, NULL));
    STRICT_EXPECTED_CALL(xio_setoption(NULL, X509_PRIVATE_KEY_OPTION, X509_PRIVATE_KEY))
        .IgnoreArgument(1);

    // act
    IOTHUB_CLIENT_RESULT result = IoTHubTransportMqtt_SetOption(handle, X509_PRIVATE_KEY_OPTION, X509_PRIVATE_KEY);

    // assert
    ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_031: [If the option parameter is set to "logtrace" then the value shall be a bool_ptr and the value will determine if the mqtt client log is on or off.] */
TEST_FUNCTION(IoTHubTransportMqtt_Setoption_succeed)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config ={ 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);

    TRANSPORT_LL_HANDLE handle = IoTHubTransportMqtt_Create(&config);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(mqtt_client_set_trace(IGNORED_PTR_ARG, IGNORED_NUM_ARG, IGNORED_NUM_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(3);

    bool traceOn = true;

    // act
    IOTHUB_CLIENT_RESULT result = IoTHubTransportMqtt_SetOption(handle, OPTION_LOG_TRACE, &traceOn);

    // assert
    ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_036: [If the option parameter is set to "keepalive" then the value shall be a int_ptr and the value will determine the mqtt keepalive time that is set for pings.] */
TEST_FUNCTION(IoTHubTransportMqtt_Setoption_keepAlive_succeed)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config ={ 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);

    TRANSPORT_LL_HANDLE handle = IoTHubTransportMqtt_Create(&config);
    umock_c_reset_all_calls();

    int keepAlive = 10;

    // act
    IOTHUB_CLIENT_RESULT result = IoTHubTransportMqtt_SetOption(handle, OPTION_KEEP_ALIVE, &keepAlive);

    // assert
    ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_038: [If the client is connected when the keepalive is set then IoTHubTransportMqtt_SetOption shall disconnect and reconnect with the specified keepalive value.] */
TEST_FUNCTION(IoTHubTransportMqtt_Setoption_keepAlive_previous_connection_succeed)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config ={ 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);

    TRANSPORT_LL_HANDLE handle = IoTHubTransportMqtt_Create(&config);

    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    CONNECT_ACK connack ={ true, CONNECTION_ACCEPTED };
    g_fnMqttOperationCallback(TEST_MQTT_CLIENT_HANDLE, MQTT_CLIENT_ON_CONNACK, &connack, g_callbackCtx);

    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(mqtt_client_disconnect(IGNORED_PTR_ARG)).IgnoreArgument(1);
    STRICT_EXPECTED_CALL(xio_destroy(TEST_XIO_HANDLE)).IgnoreArgument(1);

    int keepAlive = 10;

    // act
    IOTHUB_CLIENT_RESULT result = IoTHubTransportMqtt_SetOption(handle, OPTION_KEEP_ALIVE, &keepAlive);

    // assert
    ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_032: [IoTHubTransportMqtt_SetOption shall pass down the option to xio_setoption if the option parameter is not a known option string for the MQTT transport.] */
TEST_FUNCTION(IoTHubTransportMqtt_Setoption_xio_create_fail)
{
    // arrange
    const char* SOME_OPTION = "AnOption";
    const void* SOME_VALUE = (void*)42;

    IOTHUBTRANSPORT_CONFIG config ={ 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);

    TRANSPORT_LL_HANDLE handle = IoTHubTransportMqtt_Create(&config);
    umock_c_reset_all_calls();

    EXPECTED_CALL(STRING_c_str(NULL));
    EXPECTED_CALL(platform_get_default_tlsio());
    EXPECTED_CALL(xio_create(NULL, NULL)).SetReturn((XIO_HANDLE)NULL);

    // act
    IOTHUB_CLIENT_RESULT result = IoTHubTransportMqtt_SetOption(handle, SOME_OPTION, SOME_VALUE);

    // assert
    ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_ERROR, result);

    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_132: [IoTHubTransportMqtt_SetOption shall return IOTHUB_CLIENT_INVALID_ARG xio_setoption fails] */
TEST_FUNCTION(IoTHubTransportMqtt_Setoption_fails_when_xio_setoption_fails)
{
    // arrange
    const char* SOME_OPTION = "AnOption";
    const void* SOME_VALUE = (void*)42;

    IOTHUBTRANSPORT_CONFIG config ={ 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);

    TRANSPORT_LL_HANDLE handle = IoTHubTransportMqtt_Create(&config);
    umock_c_reset_all_calls();

    EXPECTED_CALL(STRING_c_str(NULL));
    EXPECTED_CALL(platform_get_default_tlsio());
    EXPECTED_CALL(xio_create(NULL, NULL));
    STRICT_EXPECTED_CALL(xio_setoption(NULL, SOME_OPTION, SOME_VALUE))
        .IgnoreArgument(1)
        .SetReturn(42);

    // act
    IOTHUB_CLIENT_RESULT result = IoTHubTransportMqtt_SetOption(handle, SOME_OPTION, SOME_VALUE);

    // assert
    ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_INVALID_ARG, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}


/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_037 : [If the option parameter is set to supplied int_ptr keepalive is the same value as the existing keepalive then IoTHubTransportMqtt_SetOption shall do nothing.] */
TEST_FUNCTION(IoTHubTransportMqtt_Setoption_keepAlive_same_value_succeed)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config ={ 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);

    TRANSPORT_LL_HANDLE handle = IoTHubTransportMqtt_Create(&config);

    int keepAlive = 30;
    IOTHUB_CLIENT_RESULT result = IoTHubTransportMqtt_SetOption(handle, OPTION_KEEP_ALIVE, &keepAlive);

    CONNECT_ACK connack ={ true, CONNECTION_ACCEPTED };
    g_fnMqttOperationCallback(TEST_MQTT_CLIENT_HANDLE, MQTT_CLIENT_ON_CONNACK, &connack, g_callbackCtx);

    umock_c_reset_all_calls();

    // act
    result = IoTHubTransportMqtt_SetOption(handle, OPTION_KEEP_ALIVE, &keepAlive);

    // assert
    ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_026: [IoTHubTransportMqtt_DoWork shall do nothing if parameter handle and/or iotHubClientHandle is NULL.] */
TEST_FUNCTION(IoTHubTransportMqtt_DoWork_parameter_handle_NULL_fail)
{
    // arrange

    // act
    IoTHubTransportMqtt_DoWork(NULL, TEST_IOTHUB_CLIENT_LL_HANDLE);
    // assert

}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_026: [IoTHubTransportMqtt_DoWork shall do nothing if parameter handle and/or iotHubClientHandle is NULL.] */
TEST_FUNCTION(IoTHubTransportMqtt_DoWork_parameter_iothubClient_NULL_fail)
{
    // arrange

    // act
    IoTHubTransportMqtt_DoWork(TEST_TRANSPORT_HANDLE, NULL);
    // assert

}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_026: [IoTHubTransportMqtt_DoWork shall do nothing if parameter handle and/or iotHubClientHandle is NULL.] */
TEST_FUNCTION(IoTHubTransportMqtt_DoWork_all_parameters_NULL_fail)
{
    // arrange

    // act
    IoTHubTransportMqtt_DoWork(NULL, NULL);
    // assert

}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_027: [IoTHubTransportMqtt_DoWork shall inspect the “waitingToSend” DLIST passed in config structure.] */
TEST_FUNCTION(IoTHubTransportMqtt_DoWork_mqtt_client_connect_fail)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config ={ 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);

    TRANSPORT_LL_HANDLE handle = IoTHubTransportMqtt_Create(&config);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(tickcounter_get_current_ms(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(STRING_new());
    STRICT_EXPECTED_CALL(get_time(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    EXPECTED_CALL(SASToken_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_NUM_ARG));
    EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG)).SetReturn(TEST_DEVICE_ID);
    EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG)).SetReturn(TEST_STRING_VALUE);
    EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG)).SetReturn(TEST_SAS_TOKEN);
    EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG)).SetReturn(TEST_HOST_NAME);
    STRICT_EXPECTED_CALL(platform_get_default_tlsio());
    STRICT_EXPECTED_CALL(xio_create(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    EXPECTED_CALL(mqtt_client_connect(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(tickcounter_get_current_ms(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG));
    EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(tickcounter_get_current_ms(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mqtt_client_dowork(TEST_MQTT_CLIENT_HANDLE))
        .IgnoreArgument(1);

/*#ifndef NO_LOGGING
    EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG));
    EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG));
    EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG));
    EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG));
    EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG));
#else
    EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG));
    EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG));
    EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG));
    EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG));
#endif*/

    // act
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    //assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_027: [IoTHubTransportMqtt_DoWork shall inspect the “waitingToSend” DLIST passed in config structure.] */
TEST_FUNCTION(IoTHubTransportMqtt_DoWork_no_messages_succeed)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config ={ 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);

    TRANSPORT_LL_HANDLE handle = IoTHubTransportMqtt_Create(&config);
    umock_c_reset_all_calls();

    setup_initialize_connection_mocks();
    STRICT_EXPECTED_CALL(mqtt_client_dowork(TEST_MQTT_CLIENT_HANDLE))
        .IgnoreArgument(1);

    // act
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    //assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

TEST_FUNCTION(IoTHubTransportMqtt_DoWork_no_messages_fail)
{
    // arrange
    int negativeTestsInitResult = umock_c_negative_tests_init();
    ASSERT_ARE_EQUAL(int, 0, negativeTestsInitResult);

    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);

    TRANSPORT_LL_HANDLE handle = IoTHubTransportMqtt_Create(&config);
    umock_c_reset_all_calls();

    setup_initialize_connection_mocks();
    STRICT_EXPECTED_CALL(mqtt_client_dowork(TEST_MQTT_CLIENT_HANDLE))
        .IgnoreArgument(1);

    umock_c_negative_tests_snapshot();

    // act
    size_t calls_cannot_fail[] = { 2 };

    // act
    size_t count = umock_c_negative_tests_call_count();
    for (size_t index = 0; index < count; index++)
    {
        if (should_skip_index(index, calls_cannot_fail, sizeof(calls_cannot_fail)/sizeof(calls_cannot_fail[0])) != 0)
        {
            continue;
        }

        umock_c_negative_tests_reset();
        umock_c_negative_tests_fail_call(index);

        char tmp_msg[64];
        sprintf(tmp_msg, "IoTHubTransportMqtt_DoWork failure in test %zu/%zu", index, count);

        IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

        //assert
    }

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
    umock_c_negative_tests_deinit();
}

TEST_FUNCTION(IoTHubTransportMqtt_DoWork_SAS_token_from_user_succeed)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config ={ 0 };
    SetupIothubTransportConfigWithKeyAndSasToken(&config, TEST_DEVICE_ID, NULL, TEST_DEVICE_SAS, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);

    TRANSPORT_LL_HANDLE handle = IoTHubTransportMqtt_Create(&config);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(tickcounter_get_current_ms(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(STRING_new());
    EXPECTED_CALL(STRING_clone(IGNORED_PTR_ARG));
    EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG));
    EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG));
    EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(platform_get_default_tlsio());
    STRICT_EXPECTED_CALL(xio_create(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    EXPECTED_CALL(mqtt_client_connect(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(tickcounter_get_current_ms(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG));
    EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(tickcounter_get_current_ms(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mqtt_client_dowork(TEST_MQTT_CLIENT_HANDLE))
        .IgnoreArgument(1);

    // act
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    //assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_041: [If both deviceKey and deviceSasToken fields are NULL then IoTHubTransportMqtt_Create shall assume a x509 authentication.] */
TEST_FUNCTION(IoTHubTransportMqtt_DoWork_x509_succeed)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config ={ 0 };
    SetupIothubTransportConfigWithKeyAndSasToken(&config, TEST_DEVICE_ID, NULL, NULL, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);

    TRANSPORT_LL_HANDLE handle = IoTHubTransportMqtt_Create(&config);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(tickcounter_get_current_ms(TEST_COUNTER_HANDLE, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(STRING_new());
    EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG));
    EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG));
    EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(platform_get_default_tlsio());
    EXPECTED_CALL(xio_create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    EXPECTED_CALL(mqtt_client_connect(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(tickcounter_get_current_ms(TEST_COUNTER_HANDLE, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG));
    EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG));

    STRICT_EXPECTED_CALL(tickcounter_get_current_ms(TEST_COUNTER_HANDLE, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);

    STRICT_EXPECTED_CALL(mqtt_client_dowork(TEST_MQTT_CLIENT_HANDLE)).IgnoreArgument(1);


    // act
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    //assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_027: [IoTHubTransportMqtt_DoWork shall inspect the “waitingToSend” DLIST passed in config structure.] */
TEST_FUNCTION(IoTHubTransportMqtt_DoWork_with_1_event_item_succeeds)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);

    QOS_VALUE QosValue[] = { DELIVER_AT_LEAST_ONCE };
    SUBSCRIBE_ACK suback;
    suback.packetId = 1234;
    suback.qosCount = 1;
    suback.qosReturn = QosValue;

    IOTHUB_MESSAGE_LIST message1;
    memset(&message1, 0, sizeof(IOTHUB_MESSAGE_LIST));
    message1.messageHandle = TEST_IOTHUB_MSG_BYTEARRAY;

    DList_InsertTailList(config.waitingToSend, &(message1.entry));
    TRANSPORT_LL_HANDLE handle = IoTHubTransportMqtt_Create(&config);
    g_fnMqttOperationCallback(TEST_MQTT_CLIENT_HANDLE, MQTT_CLIENT_ON_SUBSCRIBE_ACK, &suback, g_callbackCtx);
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);
    umock_c_reset_all_calls();

    setup_iothubtransportmqtt_dowork_events_mocks(NULL, NULL, 0);

    // act
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    //assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

TEST_FUNCTION(IoTHubTransportMqtt_DoWork_with_1_event_item_fail)
{
    // arrange
    int negativeTestsInitResult = umock_c_negative_tests_init();
    ASSERT_ARE_EQUAL(int, 0, negativeTestsInitResult);

    IOTHUBTRANSPORT_CONFIG config ={ 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);

    QOS_VALUE QosValue[] ={ DELIVER_AT_LEAST_ONCE };
    SUBSCRIBE_ACK suback;
    suback.packetId = 1234;
    suback.qosCount = 1;
    suback.qosReturn = QosValue;

    IOTHUB_MESSAGE_LIST message1;
    memset(&message1, 0, sizeof(IOTHUB_MESSAGE_LIST));
    message1.messageHandle = TEST_IOTHUB_MSG_BYTEARRAY;

    DList_InsertTailList(config.waitingToSend, &(message1.entry));
    TRANSPORT_LL_HANDLE handle = IoTHubTransportMqtt_Create(&config);
    g_fnMqttOperationCallback(TEST_MQTT_CLIENT_HANDLE, MQTT_CLIENT_ON_SUBSCRIBE_ACK, &suback, g_callbackCtx);
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);
    umock_c_reset_all_calls();

    setup_iothubtransportmqtt_dowork_events_mocks(NULL, NULL, 0);

    umock_c_negative_tests_snapshot();

    // act
    size_t calls_cannot_fail[] = { 4 };

    // act
    size_t count = umock_c_negative_tests_call_count();
    for (size_t index = 0; index < count; index++)
    {
        if (should_skip_index(index, calls_cannot_fail, sizeof(calls_cannot_fail)/sizeof(calls_cannot_fail[0])) != 0)
        {
            continue;
        }

        umock_c_negative_tests_reset();
        umock_c_negative_tests_fail_call(index);

        char tmp_msg[64];
        sprintf(tmp_msg, "IoTHubTransportMqtt_DoWork failure in test %zu/%zu", index, count);

        IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

        //assert
    }

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
    umock_c_negative_tests_deinit();
}

TEST_FUNCTION(IoTHubTransportMqtt_DoWork_with_1_event_item_with_properties_succeeds)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);

    QOS_VALUE QosValue[] = { DELIVER_AT_LEAST_ONCE };
    SUBSCRIBE_ACK suback;
    suback.packetId = 1234;
    suback.qosCount = 1;
    suback.qosReturn = QosValue;

    g_nullMapVariable = false;

    const size_t propCount = 1;
    const char* keys[1] = { "propKey1" };
    const char* values[1] = { "propValue1" };

    IOTHUB_MESSAGE_LIST message1;
    memset(&message1, 0, sizeof(IOTHUB_MESSAGE_LIST));
    message1.messageHandle = TEST_IOTHUB_MSG_BYTEARRAY;

    DList_InsertTailList(config.waitingToSend, &(message1.entry));
    TRANSPORT_LL_HANDLE handle = IoTHubTransportMqtt_Create(&config);
    g_fnMqttOperationCallback(TEST_MQTT_CLIENT_HANDLE, MQTT_CLIENT_ON_SUBSCRIBE_ACK, &suback, g_callbackCtx);
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);
    umock_c_reset_all_calls();

    setup_iothubtransportmqtt_dowork_events_mocks((const char* const**)&keys, (const char* const**)&values, propCount);

    // act
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    //assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

TEST_FUNCTION(IoTHubTransportMqtt_DoWork_with_1_event_item_with_2_properties_succeeds)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config ={ 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);

    QOS_VALUE QosValue[] ={ DELIVER_AT_LEAST_ONCE };
    SUBSCRIBE_ACK suback;
    suback.packetId = 1234;
    suback.qosCount = 1;
    suback.qosReturn = QosValue;

    g_nullMapVariable = false;

    const size_t propCount = 2;
    const char* keys[2] = { "propKey1", "propKey2" };
    const char* values[2] = { "propValue1", "propValue2" };

    IOTHUB_MESSAGE_LIST message1;
    memset(&message1, 0, sizeof(IOTHUB_MESSAGE_LIST));
    message1.messageHandle = TEST_IOTHUB_MSG_BYTEARRAY;

    DList_InsertTailList(config.waitingToSend, &(message1.entry));
    TRANSPORT_LL_HANDLE handle = IoTHubTransportMqtt_Create(&config);
    g_fnMqttOperationCallback(TEST_MQTT_CLIENT_HANDLE, MQTT_CLIENT_ON_SUBSCRIBE_ACK, &suback, g_callbackCtx);
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);
    umock_c_reset_all_calls();

    setup_iothubtransportmqtt_dowork_events_mocks((const char* const**)&keys, (const char* const**)&values, propCount);

    // act
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    //assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

/* Test_SRS_IOTHUB_MQTT_TRANSPORT_07_033: [IoTHubTransportMqtt_DoWork shall iterate through the Waiting Acknowledge messages looking for any message that has been waiting longer than 2 min.]*/
TEST_FUNCTION(IoTHubTransportMqtt_DoWork_no_resend_message_succeeds)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config ={ 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);

    QOS_VALUE QosValue[] = { DELIVER_AT_LEAST_ONCE };
    SUBSCRIBE_ACK suback;
    suback.packetId = 1234;
    suback.qosCount = 1;
    suback.qosReturn = QosValue;

    IOTHUB_MESSAGE_LIST message2;
    memset(&message2, 0, sizeof(IOTHUB_MESSAGE_LIST));
    message2.messageHandle = TEST_IOTHUB_MSG_STRING;

    DList_InsertTailList(config.waitingToSend, &(message2.entry));
    TRANSPORT_LL_HANDLE handle = IoTHubTransportMqtt_Create(&config);
    g_fnMqttOperationCallback(TEST_MQTT_CLIENT_HANDLE, MQTT_CLIENT_ON_SUBSCRIBE_ACK, &suback, g_callbackCtx);
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(tickcounter_get_current_ms(TEST_COUNTER_HANDLE, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(tickcounter_get_current_ms(TEST_COUNTER_HANDLE, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mqtt_client_dowork(TEST_MQTT_CLIENT_HANDLE))
        .IgnoreArgument(1);

    // act
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    //assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

/* Test_SRS_IOTHUB_MQTT_TRANSPORT_07_033: [IoTHubTransportMqtt_DoWork shall iterate through the Waiting Acknowledge messages looking for any message that has been waiting longer than 2 min.]*/
TEST_FUNCTION(IoTHubTransportMqtt_DoWork_resend_message_succeeds)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config ={ 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);

    QOS_VALUE QosValue[] ={ DELIVER_AT_LEAST_ONCE };
    SUBSCRIBE_ACK suback;
    suback.packetId = 1234;
    suback.qosCount = 1;
    suback.qosReturn = QosValue;

    IOTHUB_MESSAGE_LIST message2;
    memset(&message2, 0, sizeof(IOTHUB_MESSAGE_LIST));
    message2.messageHandle = TEST_IOTHUB_MSG_STRING;

    DList_InsertTailList(config.waitingToSend, &(message2.entry));
    TRANSPORT_LL_HANDLE handle = IoTHubTransportMqtt_Create(&config);
    g_fnMqttOperationCallback(TEST_MQTT_CLIENT_HANDLE, MQTT_CLIENT_ON_SUBSCRIBE_ACK, &suback, g_callbackCtx);
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(tickcounter_get_current_ms(TEST_COUNTER_HANDLE, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .CopyOutArgumentBuffer(2, &g_current_ms, sizeof(g_current_ms));
    g_current_ms += 5*60*1000;
    STRICT_EXPECTED_CALL(tickcounter_get_current_ms(TEST_COUNTER_HANDLE, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .CopyOutArgumentBuffer(2, &g_current_ms, sizeof(g_current_ms));
    STRICT_EXPECTED_CALL(IoTHubMessage_GetContentType(TEST_IOTHUB_MSG_STRING));
    STRICT_EXPECTED_CALL(IoTHubMessage_GetString(TEST_IOTHUB_MSG_STRING));
    EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(STRING_construct(TEST_MQTT_EVENT_TOPIC))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(IoTHubMessage_Properties(TEST_IOTHUB_MSG_STRING))
        .IgnoreArgument(1);
    EXPECTED_CALL(Map_GetInternals(TEST_MESSAGE_PROP_MAP, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mqttmessage_create(IGNORED_NUM_ARG, IGNORED_PTR_ARG, DELIVER_AT_LEAST_ONCE, (const uint8_t*)appMessageString, strlen(appMessageString)))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mqtt_client_publish(TEST_MQTT_CLIENT_HANDLE, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(tickcounter_get_current_ms(TEST_COUNTER_HANDLE, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mqttmessage_destroy(TEST_MQTT_MESSAGE_HANDLE));
    EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mqtt_client_dowork(TEST_MQTT_CLIENT_HANDLE))
        .IgnoreArgument(1);

    // act
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    //assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_027: [IoTHubTransportMqtt_DoWork shall inspect the “waitingToSend” DLIST passed in config structure.] */
/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_029: [IoTHubTransportMqtt_DoWork shall create a MQTT_MESSAGE_HANDLE and pass this to a call to mqtt_client_publish.] */
/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_030: [IoTHubTransportMqtt_DoWork shall call mqtt_client_dowork everytime it is called if it is connected.] */
TEST_FUNCTION(IoTHubTransportMqtt_DoWork_with_1_event_item_STRING_type_succeeds)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config ={ 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);

    QOS_VALUE QosValue[] ={ DELIVER_AT_LEAST_ONCE };
    SUBSCRIBE_ACK suback;
    suback.packetId = 1234;
    suback.qosCount = 1;
    suback.qosReturn = QosValue;

    IOTHUB_MESSAGE_LIST message2;
    memset(&message2, 0, sizeof(IOTHUB_MESSAGE_LIST));
    message2.messageHandle = TEST_IOTHUB_MSG_STRING;

    DList_InsertTailList(config.waitingToSend, &(message2.entry));
    TRANSPORT_LL_HANDLE handle = IoTHubTransportMqtt_Create(&config);
    g_fnMqttOperationCallback(TEST_MQTT_CLIENT_HANDLE, MQTT_CLIENT_ON_SUBSCRIBE_ACK, &suback, g_callbackCtx);
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(tickcounter_get_current_ms(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(IoTHubMessage_GetContentType(TEST_IOTHUB_MSG_STRING))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(IoTHubMessage_GetString(TEST_IOTHUB_MSG_STRING))
        .IgnoreArgument(1);
    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(STRING_construct(TEST_MQTT_EVENT_TOPIC))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(IoTHubMessage_Properties(TEST_IOTHUB_MSG_STRING))
        .IgnoreArgument(1);
    EXPECTED_CALL(Map_GetInternals(TEST_MESSAGE_PROP_MAP, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mqttmessage_create(IGNORED_NUM_ARG, IGNORED_PTR_ARG, DELIVER_AT_LEAST_ONCE, (const uint8_t*)appMessageString, strlen(appMessageString)))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mqtt_client_publish(TEST_MQTT_CLIENT_HANDLE, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(tickcounter_get_current_ms(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mqttmessage_destroy(TEST_MQTT_MESSAGE_HANDLE))
        .IgnoreArgument(1);
    EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG));
    EXPECTED_CALL(DList_RemoveEntryList(IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(DList_InsertTailList(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mqtt_client_dowork(TEST_MQTT_CLIENT_HANDLE))
        .IgnoreArgument(1);

    // act
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    //assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_034: [If IoTHubTransportMqtt_DoWork has resent the message two times then it shall fail the message] */
TEST_FUNCTION(IoTHubTransportMqtt_DoWork_resend_max_recount_reached_message_succeeds)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config ={ 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);

    QOS_VALUE QosValue[] ={ DELIVER_AT_LEAST_ONCE };
    SUBSCRIBE_ACK suback;
    suback.packetId = 1234;
    suback.qosCount = 1;
    suback.qosReturn = QosValue;

    IOTHUB_MESSAGE_LIST message2;
    memset(&message2, 0, sizeof(IOTHUB_MESSAGE_LIST));
    message2.messageHandle = TEST_IOTHUB_MSG_STRING;

    DList_InsertTailList(config.waitingToSend, &(message2.entry));
    TRANSPORT_LL_HANDLE handle = IoTHubTransportMqtt_Create(&config);
    g_fnMqttOperationCallback(TEST_MQTT_CLIENT_HANDLE, MQTT_CLIENT_ON_SUBSCRIBE_ACK, &suback, g_callbackCtx);
    for (size_t index = 0; index < 3; index++)
    {
        IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);
        g_current_ms = 3 * index * 60 * 1000;
    }
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(tickcounter_get_current_ms(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mqtt_client_dowork(TEST_MQTT_CLIENT_HANDLE))
        .IgnoreArgument(1);

    /*EXPECTED_CALL(DList_InitializeListHead(IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(DList_InsertTailList(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(IoTHubClient_LL_SendComplete(TEST_IOTHUB_CLIENT_LL_HANDLE, IGNORED_PTR_ARG, IOTHUB_CLIENT_CONFIRMATION_MESSAGE_TIMEOUT))
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mqtt_client_dowork(TEST_MQTT_CLIENT_HANDLE));
    EXPECTED_CALL(DList_RemoveEntryList(IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(tickcounter_get_current_ms(TEST_COUNTER_HANDLE, IGNORED_PTR_ARG))
        .IgnoreArgument(2);
    EXPECTED_CALL(gballoc_free(NULL));*/

    // act
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    //assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

TEST_FUNCTION(IoTHubTransportMqtt_DoWork_connectFailCount_exceed_succeed)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config ={ 0 };
    const size_t iterationCount = 7;
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);

    TRANSPORT_LL_HANDLE handle = IoTHubTransportMqtt_Create(&config);
    umock_c_reset_all_calls();

    setup_initialize_connection_mocks();

    for (size_t index = 0; index < iterationCount-1; index++)
    {
        STRICT_EXPECTED_CALL(mqtt_client_dowork(TEST_MQTT_CLIENT_HANDLE))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(tickcounter_get_current_ms(TEST_COUNTER_HANDLE, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2);
    }
    STRICT_EXPECTED_CALL(mqtt_client_dowork(TEST_MQTT_CLIENT_HANDLE))
        .IgnoreArgument(1);

    // act
    for (size_t index = 0; index < iterationCount; index++)
    {
        IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);
    }

    //assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

/* Test_SRS_IOTHUB_MQTT_TRANSPORT_07_023: [IoTHubTransportMqtt_GetSendStatus shall return IOTHUB_CLIENT_INVALID_ARG if called with NULL parameter.] */
TEST_FUNCTION(IoTHubTransportMqtt_GetSendStatus_InvalidHandleArgument_fail)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config ={ 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);

    TRANSPORT_LL_HANDLE handle = IoTHubTransportMqtt_Create(&config);
    umock_c_reset_all_calls();

    IOTHUB_CLIENT_STATUS status;

    // act
    IOTHUB_CLIENT_RESULT result = IoTHubTransportMqtt_GetSendStatus(NULL, &status);

    // assert
    ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, result, IOTHUB_CLIENT_INVALID_ARG);

    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

/* Test_SRS_IOTHUB_MQTT_TRANSPORT_07_023: [IoTHubTransportMqtt_GetSendStatus shall return IOTHUB_CLIENT_INVALID_ARG if called with NULL parameter.] */
TEST_FUNCTION(IoTHubTransportMqtt_GetSendStatus_InvalidStatusArgument_fail)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config ={ 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);

    TRANSPORT_LL_HANDLE handle = IoTHubTransportMqtt_Create(&config);
    umock_c_reset_all_calls();

    // act
    IOTHUB_CLIENT_RESULT result = IoTHubTransportMqtt_GetSendStatus(handle, NULL);

    // assert
    ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, result, IOTHUB_CLIENT_INVALID_ARG);

    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_024: [IoTHubTransportMqtt_GetSendStatus shall return IOTHUB_CLIENT_OK and status IOTHUB_CLIENT_SEND_STATUS_IDLE if there are currently no event items to be sent or being sent.] */
TEST_FUNCTION(IoTHubTransportMqtt_GetSendStatus_empty_waitingToSend_and_empty_waitingforAck_success)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config ={ 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);

    TRANSPORT_LL_HANDLE handle = IoTHubTransportMqtt_Create(&config);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(DList_IsListEmpty(config.waitingToSend));
    STRICT_EXPECTED_CALL(DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1);

    IOTHUB_CLIENT_STATUS status;

    // act
    IOTHUB_CLIENT_RESULT result = IoTHubTransportMqtt_GetSendStatus(handle, &status);

    // assert
    ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, result, IOTHUB_CLIENT_OK);
    ASSERT_ARE_EQUAL(IOTHUB_CLIENT_STATUS, status, IOTHUB_CLIENT_SEND_STATUS_IDLE);

    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_025: [IoTHubTransportMqtt_GetSendStatus shall return IOTHUB_CLIENT_OK and status IOTHUB_CLIENT_SEND_STATUS_BUSY if there are currently event items to be sent or being sent.] */
TEST_FUNCTION(IoTHubTransportMqtt_GetSendStatus_waitingToSend_not_empty_success)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config ={ 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);

    TRANSPORT_LL_HANDLE handle = IoTHubTransportMqtt_Create(&config);

    IOTHUB_MESSAGE_HANDLE eventMessageHandle = IoTHubMessage_CreateFromByteArray(IGNORED_PTR_ARG, IGNORED_NUM_ARG);
    IOTHUB_MESSAGE_LIST newEntry;
    newEntry.messageHandle = eventMessageHandle;
    DList_InsertTailList(config.waitingToSend, &(newEntry.entry));

    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(DList_IsListEmpty(config.waitingToSend));

    IOTHUB_CLIENT_STATUS status;

    // act
    IOTHUB_CLIENT_RESULT result = IoTHubTransportMqtt_GetSendStatus(handle, &status);

    // assert
    ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, result, IOTHUB_CLIENT_OK);
    ASSERT_ARE_EQUAL(IOTHUB_CLIENT_STATUS, status, IOTHUB_CLIENT_SEND_STATUS_BUSY);

    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    IoTHubTransportMqtt_Destroy(handle);
    IoTHubMessage_Destroy(eventMessageHandle);
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_022: [This function shall return a pointer to a structure of type TRANSPORT_PROVIDER having the following values for it’s fields: IoTHubTransport_Create = IoTHubTransportMqtt_Create
IoTHubTransport_Destroy = IoTHubTransportMqtt_Destroy
IoTHubTransport_Subscribe = IoTHubTransportMqtt_Subscribe
IoTHubTransport_Unsubscribe = IoTHubTransportMqtt_Unsubscribe
IoTHubTransport_DoWork = IoTHubTransportMqtt_DoWork
IoTHubTransport_SetOption = IoTHubTransportMqtt_SetOption] */
TEST_FUNCTION(IoTHubTransportMqtt_Protocol_succeeds)
{
    ///arrange

    ///act
    const TRANSPORT_PROVIDER* result = MQTT_Protocol();

    ///assert
    ASSERT_IS_NOT_NULL(result);
    /*ASSERT_ARE_EQUAL(void_ptr, (void*)((TRANSPORT_PROVIDER*)result)->IoTHubTransport_SetOption, (void*)IoTHubTransportMqtt_SetOption);
    ASSERT_ARE_EQUAL(void_ptr, (void*)((TRANSPORT_PROVIDER*)result)->IoTHubTransport_Create, (void*)IoTHubTransportMqtt_Create);
    ASSERT_ARE_EQUAL(void_ptr, (void*)((TRANSPORT_PROVIDER*)result)->IoTHubTransport_Destroy, (void*)IoTHubTransportMqtt_Destroy);
    ASSERT_ARE_EQUAL(void_ptr, (void*)((TRANSPORT_PROVIDER*)result)->IoTHubTransport_Subscribe, (void*)IoTHubTransportMqtt_Subscribe);
    ASSERT_ARE_EQUAL(void_ptr, (void*)((TRANSPORT_PROVIDER*)result)->IoTHubTransport_Unsubscribe, (void*)IoTHubTransportMqtt_Unsubscribe);
    ASSERT_ARE_EQUAL(void_ptr, (void*)((TRANSPORT_PROVIDER*)result)->IoTHubTransport_DoWork, (void*)IoTHubTransportMqtt_DoWork);
    ASSERT_ARE_EQUAL(void_ptr, (void*)((TRANSPORT_PROVIDER*)result)->IoTHubTransport_GetSendStatus, (void*)IoTHubTransportMqtt_GetSendStatus);*/

    ///cleanup
}

TEST_FUNCTION(IoTHubTransportMqtt_delivered_NULL_context_do_Nothing)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);

    TRANSPORT_LL_HANDLE handle = IoTHubTransportMqtt_Create(&config);

    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(xio_close(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(2)
        .IgnoreArgument(3);

    // act
    g_fnMqttOperationCallback(TEST_MQTT_CLIENT_HANDLE, MQTT_CLIENT_ON_ERROR, NULL, g_callbackCtx);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

TEST_FUNCTION(IoTHubTransportMqtt_delivered_MQTT_CLIENT_NO_PING_RESPONSE_success)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config ={ 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);

    TRANSPORT_LL_HANDLE handle = IoTHubTransportMqtt_Create(&config);

    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(xio_close(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(2)
        .IgnoreArgument(3);

    // act
    g_fnMqttOperationCallback(TEST_MQTT_CLIENT_HANDLE, MQTT_CLIENT_NO_PING_RESPONSE, NULL, g_callbackCtx);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

TEST_FUNCTION(IoTHubTransportMqtt_MqttOpCompleteCallback_DISCONNECT_succeed)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config ={ 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);

    TRANSPORT_LL_HANDLE handle = IoTHubTransportMqtt_Create(&config);
    umock_c_reset_all_calls();

    // act
    g_fnMqttOperationCallback(TEST_MQTT_CLIENT_HANDLE, MQTT_CLIENT_ON_DISCONNECT, NULL, g_callbackCtx);

    //assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

TEST_FUNCTION(IoTHubTransportMqtt_MqttOpCompleteCallback_CONN_ACK_NOT_CONNECTED_succeed)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config ={ 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);

    CONNECT_ACK connack;
    connack.isSessionPresent = false;
    connack.returnCode = CONN_REFUSED_BAD_USERNAME_PASSWORD;

    TRANSPORT_LL_HANDLE handle = IoTHubTransportMqtt_Create(&config);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(mqtt_client_disconnect(TEST_MQTT_CLIENT_HANDLE))
        .IgnoreArgument(1);

    // act
    g_fnMqttOperationCallback(TEST_MQTT_CLIENT_HANDLE, MQTT_CLIENT_ON_CONNACK, &connack, g_callbackCtx);

    //assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

TEST_FUNCTION(IoTHubTransportMqtt_MqttOpCompleteCallback_PUBLISH_ACK_succeed)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config ={ 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);

    PUBLISH_ACK puback;
    puback.packetId = 2;

    QOS_VALUE QosValue[] ={ DELIVER_AT_LEAST_ONCE };
    SUBSCRIBE_ACK suback;
    suback.packetId = 1234;
    suback.qosCount = 1;
    suback.qosReturn = QosValue;

    IOTHUB_MESSAGE_LIST message1;
    memset(&message1, 0, sizeof(IOTHUB_MESSAGE_LIST));
    message1.messageHandle = TEST_IOTHUB_MSG_BYTEARRAY;

    DList_InsertTailList(config.waitingToSend, &(message1.entry));
    TRANSPORT_LL_HANDLE handle = IoTHubTransportMqtt_Create(&config);
    g_fnMqttOperationCallback(TEST_MQTT_CLIENT_HANDLE, MQTT_CLIENT_ON_SUBSCRIBE_ACK, &suback, g_callbackCtx);
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(DList_RemoveEntryList(IGNORED_PTR_ARG))
        .IgnoreAllArguments();
    STRICT_EXPECTED_CALL(DList_InitializeListHead(IGNORED_PTR_ARG))
        .IgnoreAllArguments();
    STRICT_EXPECTED_CALL(DList_InsertTailList(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreAllArguments();
    STRICT_EXPECTED_CALL(IoTHubClient_LL_SendComplete(TEST_IOTHUB_CLIENT_LL_HANDLE, IGNORED_PTR_ARG, IOTHUB_CLIENT_CONFIRMATION_OK))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(gballoc_free(NULL))
        .IgnoreArgument(1);

    // act
    g_fnMqttOperationCallback(TEST_MQTT_CLIENT_HANDLE, MQTT_CLIENT_ON_PUBLISH_ACK, &puback, g_callbackCtx);

    //assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

TEST_FUNCTION(IoTHubTransportMqtt_MessageRecv_message_NULL_fail)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config ={ 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);

    TRANSPORT_LL_HANDLE handle = IoTHubTransportMqtt_Create(&config);

    umock_c_reset_all_calls();

    // act
    ASSERT_IS_NOT_NULL(g_fnMqttMsgRecv);
    g_fnMqttMsgRecv(NULL, g_callbackCtx);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

TEST_FUNCTION(IoTHubTransportMqtt_MessageRecv_context_NULL_fail)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config ={ 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);

    TRANSPORT_LL_HANDLE handle = IoTHubTransportMqtt_Create(&config);

    umock_c_reset_all_calls();

    // act
    ASSERT_IS_NOT_NULL(g_fnMqttMsgRecv);
    g_fnMqttMsgRecv(TEST_MQTT_MESSAGE_HANDLE, NULL);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

TEST_FUNCTION(IoTHubTransportMqtt_MessageRecv_Message_context_NULL_fail)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config ={ 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);

    TRANSPORT_LL_HANDLE handle = IoTHubTransportMqtt_Create(&config);

    umock_c_reset_all_calls();

    // act
    ASSERT_IS_NOT_NULL(g_fnMqttMsgRecv);
    g_fnMqttMsgRecv(NULL, NULL);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

TEST_FUNCTION(IoTHubTransportMqtt_MessageRecv_succeed)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config ={ 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);

    TRANSPORT_LL_HANDLE handle = IoTHubTransportMqtt_Create(&config);
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(mqttmessage_getApplicationMsg(TEST_MQTT_MESSAGE_HANDLE))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(IoTHubMessage_CreateFromByteArray(appMessage, appMsgSize));
    STRICT_EXPECTED_CALL(mqttmessage_getTopicName(TEST_MQTT_MESSAGE_HANDLE))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(STRING_construct(TEST_MQTT_MSG_TOPIC))
        .IgnoreArgument(1);
    EXPECTED_CALL(STRING_TOKENIZER_create(IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(IoTHubMessage_Properties(TEST_IOTHUB_MSG_BYTEARRAY));
    STRICT_EXPECTED_CALL(STRING_new());
    STRICT_EXPECTED_CALL(STRING_TOKENIZER_get_next_token(IGNORED_PTR_ARG, IGNORED_PTR_ARG, "&"))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(3);
    EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG));
    EXPECTED_CALL(STRING_TOKENIZER_destroy(IGNORED_PTR_ARG));
    EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(IoTHubClient_LL_MessageCallback(TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_IOTHUB_MSG_BYTEARRAY))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(IoTHubMessage_Destroy(TEST_IOTHUB_MSG_BYTEARRAY))
        .IgnoreArgument(1);

    // act
    ASSERT_IS_NOT_NULL(g_fnMqttMsgRecv);
    g_fnMqttMsgRecv(TEST_MQTT_MESSAGE_HANDLE, g_callbackCtx);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

TEST_FUNCTION(IoTHubTransportMqtt_MessageRecv_with_Properties_succeed)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config ={ 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);

    TRANSPORT_LL_HANDLE handle = IoTHubTransportMqtt_Create(&config);
    g_tokenizerIndex = 6;
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(mqttmessage_getApplicationMsg(TEST_MQTT_MESSAGE_HANDLE));
    STRICT_EXPECTED_CALL(IoTHubMessage_CreateFromByteArray(appMessage, appMsgSize));
    STRICT_EXPECTED_CALL(mqttmessage_getTopicName(TEST_MQTT_MESSAGE_HANDLE)).SetReturn(TEST_MQTT_MSG_TOPIC_W_1_PROP);
    STRICT_EXPECTED_CALL(STRING_construct(TEST_MQTT_MSG_TOPIC_W_1_PROP));
    EXPECTED_CALL(STRING_TOKENIZER_create(IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(IoTHubMessage_Properties(TEST_IOTHUB_MSG_BYTEARRAY));
    STRICT_EXPECTED_CALL(STRING_new());
    STRICT_EXPECTED_CALL(STRING_TOKENIZER_get_next_token(IGNORED_PTR_ARG, IGNORED_PTR_ARG, "&"))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(3);
    STRICT_EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .SetReturn("devices/thisIsDeviceID/messages/devicebound/iothub-ack=Full");
    STRICT_EXPECTED_CALL(STRING_TOKENIZER_get_next_token(IGNORED_PTR_ARG, IGNORED_PTR_ARG, "&"))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(3);
    EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG));
    EXPECTED_CALL(STRING_TOKENIZER_destroy(IGNORED_PTR_ARG));
    EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(IoTHubClient_LL_MessageCallback(TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_IOTHUB_MSG_BYTEARRAY));
    STRICT_EXPECTED_CALL(IoTHubMessage_Destroy(TEST_IOTHUB_MSG_BYTEARRAY));

    // act
    ASSERT_IS_NOT_NULL(g_fnMqttMsgRecv);
    g_fnMqttMsgRecv(TEST_MQTT_MESSAGE_HANDLE, g_callbackCtx);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

TEST_FUNCTION(IoTHubTransportMqtt_MessageRecv_messagecallback_ABANDONED_fail)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config ={ 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);

    QOS_VALUE QosValue[] ={ DELIVER_AT_LEAST_ONCE };
    SUBSCRIBE_ACK suback;
    suback.packetId = 1234;
    suback.qosCount = 1;
    suback.qosReturn = QosValue;

    IOTHUB_MESSAGE_LIST message1;
    memset(&message1, 0, sizeof(IOTHUB_MESSAGE_LIST));
    message1.messageHandle = TEST_IOTHUB_MSG_BYTEARRAY;

    DList_InsertTailList(config.waitingToSend, &(message1.entry));
    TRANSPORT_LL_HANDLE handle = IoTHubTransportMqtt_Create(&config);
    g_fnMqttOperationCallback(TEST_MQTT_CLIENT_HANDLE, MQTT_CLIENT_ON_SUBSCRIBE_ACK, &suback, g_callbackCtx);
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);
    umock_c_reset_all_calls();

    setup_message_recv_msg_callback_mocks(IOTHUBMESSAGE_ABANDONED);

    // act
    ASSERT_IS_NOT_NULL(g_fnMqttMsgRecv);
    g_fnMqttMsgRecv(TEST_MQTT_MESSAGE_HANDLE, g_callbackCtx);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

TEST_FUNCTION(IoTHubTransportMqtt_MessageRecv_messagecallback_REJECTED_fail)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);

    QOS_VALUE QosValue[] = { DELIVER_AT_LEAST_ONCE };
    SUBSCRIBE_ACK suback;
    suback.packetId = 1234;
    suback.qosCount = 1;
    suback.qosReturn = QosValue;

    IOTHUB_MESSAGE_LIST message1;
    memset(&message1, 0, sizeof(IOTHUB_MESSAGE_LIST));
    message1.messageHandle = TEST_IOTHUB_MSG_BYTEARRAY;

    DList_InsertTailList(config.waitingToSend, &(message1.entry));
    TRANSPORT_LL_HANDLE handle = IoTHubTransportMqtt_Create(&config);
    g_fnMqttOperationCallback(TEST_MQTT_CLIENT_HANDLE, MQTT_CLIENT_ON_SUBSCRIBE_ACK, &suback, g_callbackCtx);
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);
    umock_c_reset_all_calls();

    setup_message_recv_msg_callback_mocks(IOTHUBMESSAGE_REJECTED);

    // act
    ASSERT_IS_NOT_NULL(g_fnMqttMsgRecv);
    g_fnMqttMsgRecv(TEST_MQTT_MESSAGE_HANDLE, g_callbackCtx);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

// Tests_SRS_IOTHUB_MQTT_TRANSPORT_03_001: [ IoTHubTransportMqtt_Register shall return NULL if deviceId, or both deviceKey and deviceSasToken are NULL.]
TEST_FUNCTION(IoTHubTransportMqtt_Register_deviceKey_null_and_deviceSasToken_null_returns_null)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);
    IOTHUB_DEVICE_CONFIG deviceConfig;
    deviceConfig.deviceId = TEST_DEVICE_ID;
    deviceConfig.deviceKey = NULL;
    deviceConfig.deviceSasToken = NULL;

    TRANSPORT_LL_HANDLE handle = IoTHubTransportMqtt_Create(&config);
    umock_c_reset_all_calls();

    EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG));

    // act
    IOTHUB_DEVICE_HANDLE devHandle = IoTHubTransportMqtt_Register(handle, &deviceConfig, TEST_IOTHUB_CLIENT_LL_HANDLE, config.waitingToSend);

    // assert
    ASSERT_IS_NULL(devHandle);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

// Tests_SRS_IOTHUB_MQTT_TRANSPORT_17_004: [ IoTHubTransportMqtt_Register shall return the TRANSPORT_LL_HANDLE as the IOTHUB_DEVICE_HANDLE. ]
TEST_FUNCTION(IoTHubTransportMqtt_Register_succeeds_returns_transport)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config ={ 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);

    IOTHUB_DEVICE_CONFIG TEST_DEVICE_1;
    TEST_DEVICE_1.deviceId = TEST_DEVICE_ID;
    TEST_DEVICE_1.deviceKey = TEST_DEVICE_KEY;
    TEST_DEVICE_1.deviceSasToken = NULL;

    TRANSPORT_LL_HANDLE handle = IoTHubTransportMqtt_Create(&config);
    umock_c_reset_all_calls();

    EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG)).SetReturn(TEST_DEVICE_ID);
    EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG)).SetReturn(TEST_DEVICE_KEY);

    // act
    IOTHUB_DEVICE_HANDLE devHandle = IoTHubTransportMqtt_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, config.waitingToSend);

    // assert
    ASSERT_IS_NOT_NULL(devHandle);
    ASSERT_ARE_EQUAL(void_ptr, handle, devHandle);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

// Tests_SRS_IOTHUB_MQTT_TRANSPORT_17_004: [ IoTHubTransportMqtt_Register shall return the TRANSPORT_LL_HANDLE as the IOTHUB_DEVICE_HANDLE. ]
TEST_FUNCTION(IoTHubTransportMqtt_Register_twice_fails_second_time)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config ={ 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);

    IOTHUB_DEVICE_CONFIG TEST_DEVICE_1;
    TEST_DEVICE_1.deviceId = TEST_DEVICE_ID;
    TEST_DEVICE_1.deviceKey = TEST_DEVICE_KEY;
    TEST_DEVICE_1.deviceSasToken = NULL;

    TRANSPORT_LL_HANDLE handle = IoTHubTransportMqtt_Create(&config);

    umock_c_reset_all_calls();

    EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG)).SetReturn(TEST_DEVICE_ID);
    EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG)).SetReturn(TEST_DEVICE_KEY);
    EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG)).SetReturn(TEST_DEVICE_ID);
    EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG)).SetReturn(TEST_DEVICE_KEY);

    // act
    IOTHUB_DEVICE_HANDLE devHandle = IoTHubTransportMqtt_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, config.waitingToSend);
    IOTHUB_DEVICE_HANDLE devHandle2 = IoTHubTransportMqtt_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, config.waitingToSend);

    // assert
    ASSERT_IS_NOT_NULL(devHandle);
    ASSERT_IS_NULL(devHandle2);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

// Tests_SRS_IOTHUB_MQTT_TRANSPORT_17_003: [ IoTHubTransportMqtt_Register shall return NULL if deviceId or deviceKey do not match the deviceId and deviceKey passed in during IoTHubTransportMqtt_Create.]
TEST_FUNCTION(IoTHubTransportMqtt_Register_deviceKey_mismatch_returns_null)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config ={ 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);
    IOTHUB_DEVICE_CONFIG deviceConfig;
    deviceConfig.deviceId = TEST_DEVICE_ID;
    deviceConfig.deviceKey = "not the right device key";
    deviceConfig.deviceSasToken = NULL;


    TRANSPORT_LL_HANDLE handle = IoTHubTransportMqtt_Create(&config);
    umock_c_reset_all_calls();

    EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG)).SetReturn(TEST_DEVICE_ID);
    EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG)).SetReturn(TEST_DEVICE_KEY);

    // act
    IOTHUB_DEVICE_HANDLE devHandle = IoTHubTransportMqtt_Register(handle, &deviceConfig, TEST_IOTHUB_CLIENT_LL_HANDLE, config.waitingToSend);

    // assert
    ASSERT_IS_NULL(devHandle);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

// Tests_SRS_IOTHUB_MQTT_TRANSPORT_17_003: [ IoTHubTransportMqtt_Register shall return NULL if deviceId or deviceKey do not match the deviceId and deviceKey passed in during IoTHubTransportMqtt_Create.]
TEST_FUNCTION(IoTHubTransportMqtt_Register_deviceid_mismatch_returns_null)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config ={ 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);
    IOTHUB_DEVICE_CONFIG deviceConfig;
    deviceConfig.deviceId = "not a good id after all";
    deviceConfig.deviceKey = TEST_DEVICE_KEY;
    deviceConfig.deviceSasToken = NULL;

    TRANSPORT_LL_HANDLE handle = IoTHubTransportMqtt_Create(&config);
    umock_c_reset_all_calls();

    EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG));

    // act
    IOTHUB_DEVICE_HANDLE devHandle = IoTHubTransportMqtt_Register(handle, &deviceConfig, TEST_IOTHUB_CLIENT_LL_HANDLE, config.waitingToSend);

    // assert
    ASSERT_IS_NULL(devHandle);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

// Tests_SRS_IOTHUB_MQTT_TRANSPORT_17_002: [ IoTHubTransportMqtt_Register shall return NULL if device or waitingToSend are NULL.]
TEST_FUNCTION(IoTHubTransportMqtt_Register_wts_null_returns_null)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config ={ 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);

    IOTHUB_DEVICE_CONFIG TEST_DEVICE_1;
    TEST_DEVICE_1.deviceId = TEST_DEVICE_ID;
    TEST_DEVICE_1.deviceKey = TEST_DEVICE_KEY;
    TEST_DEVICE_1.deviceSasToken = NULL;

    TRANSPORT_LL_HANDLE handle = IoTHubTransportMqtt_Create(&config);
    umock_c_reset_all_calls();

    // act
    IOTHUB_DEVICE_HANDLE devHandle = IoTHubTransportMqtt_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, NULL);

    // assert
    ASSERT_IS_NULL(devHandle);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

// Tests_SRS_IOTHUB_MQTT_TRANSPORT_17_002: [ IoTHubTransportMqtt_Register shall return NULL if device or waitingToSend are NULL.]
TEST_FUNCTION(IoTHubTransportMqtt_Register_device_null_returns_null)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config ={ 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);

    TRANSPORT_LL_HANDLE handle = IoTHubTransportMqtt_Create(&config);
    umock_c_reset_all_calls();

    // act
    IOTHUB_DEVICE_HANDLE devHandle = IoTHubTransportMqtt_Register(handle, NULL, TEST_IOTHUB_CLIENT_LL_HANDLE, config.waitingToSend);

    // assert
    ASSERT_IS_NULL(devHandle);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

// Tests_SRS_IOTHUB_MQTT_TRANSPORT_03_001: [ IoTHubTransportMqtt_Register shall return NULL if deviceId, or both deviceKey and deviceSasToken are NULL.]
TEST_FUNCTION(IoTHubTransportMqtt_Register_deviceId_null_returns_null)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config ={ 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);
    IOTHUB_DEVICE_CONFIG deviceConfig;
    deviceConfig.deviceId = NULL;
    deviceConfig.deviceKey = TEST_DEVICE_KEY;
    deviceConfig.deviceSasToken = NULL;

    TRANSPORT_LL_HANDLE handle = IoTHubTransportMqtt_Create(&config);
    umock_c_reset_all_calls();

    // act
    IOTHUB_DEVICE_HANDLE devHandle = IoTHubTransportMqtt_Register(handle, &deviceConfig, TEST_IOTHUB_CLIENT_LL_HANDLE, config.waitingToSend);

    // assert
    ASSERT_IS_NULL(devHandle);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

// Tests_SRS_IOTHUB_MQTT_TRANSPORT_03_002: [ IoTHubTransportMqtt_Register shall return NULL if both deviceKey and deviceSasToken are provided.]
TEST_FUNCTION(IoTHubTransportMqtt_Register_deviceKey_and_deviceSasToken_both_provided_returns_null)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config ={ 0 };
    SetupIothubTransportConfigWithKeyAndSasToken(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_DEVICE_SAS, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);
    IOTHUB_DEVICE_CONFIG deviceConfig;
    deviceConfig.deviceId = TEST_DEVICE_ID;
    deviceConfig.deviceKey = TEST_DEVICE_KEY;
    deviceConfig.deviceSasToken = TEST_DEVICE_SAS;

    TRANSPORT_LL_HANDLE handle = IoTHubTransportMqtt_Create(&config);
    umock_c_reset_all_calls();

    // act
    IOTHUB_DEVICE_HANDLE devHandle = IoTHubTransportMqtt_Register(handle, &deviceConfig, TEST_IOTHUB_CLIENT_LL_HANDLE, config.waitingToSend);

    // assert
    ASSERT_IS_NULL(devHandle);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

// SRS_IOTHUB_MQTT_TRANSPORT_17_004: [IoTHubTransportMqtt_Register shall return the TRANSPORT_LL_HANDLE as the IOTHUB_DEVICE_HANDLE.]
TEST_FUNCTION(IoTHubTransportMqtt_Register_deviceKey_null_and_deviceSas_valid_succeeds)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config ={ 0 };
    SetupIothubTransportConfigWithKeyAndSasToken(&config, TEST_DEVICE_ID, NULL, TEST_DEVICE_SAS, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);
    IOTHUB_DEVICE_CONFIG deviceConfig;
    deviceConfig.deviceId = TEST_DEVICE_ID;
    deviceConfig.deviceKey = NULL;
    deviceConfig.deviceSasToken = TEST_DEVICE_SAS;

    TRANSPORT_LL_HANDLE handle = IoTHubTransportMqtt_Create(&config);
    umock_c_reset_all_calls();

    EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG)).SetReturn(TEST_DEVICE_ID);

    // act
    IOTHUB_DEVICE_HANDLE devHandle = IoTHubTransportMqtt_Register(handle, &deviceConfig, TEST_IOTHUB_CLIENT_LL_HANDLE, config.waitingToSend);

    // assert
    ASSERT_IS_NOT_NULL(devHandle);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

// Tests_SRS_IOTHUB_MQTT_TRANSPORT_17_001: [ IoTHubTransportMqtt_Register shall return NULL if the TRANSPORT_LL_HANDLE is NULL.]
TEST_FUNCTION(IoTHubTransportMqtt_Register_transport_null_returns_null)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config ={ 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);

    IOTHUB_DEVICE_CONFIG TEST_DEVICE_1;
    TEST_DEVICE_1.deviceId = TEST_DEVICE_ID;
    TEST_DEVICE_1.deviceKey = TEST_DEVICE_KEY;
    TEST_DEVICE_1.deviceSasToken = NULL;

    TRANSPORT_LL_HANDLE handle = IoTHubTransportMqtt_Create(&config);
    umock_c_reset_all_calls();

    // act
    IOTHUB_DEVICE_HANDLE devHandle = IoTHubTransportMqtt_Register(NULL, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, config.waitingToSend);

    // assert
    ASSERT_IS_NULL(devHandle);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

// Tests_SRS_IOTHUB_MQTT_TRANSPORT_17_005: [ IoTHubTransportMqtt_Unregister shall return. ]
TEST_FUNCTION(IoTHubTransportMqtt_Unregister_succeeds)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config ={ 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);

    IOTHUB_DEVICE_CONFIG TEST_DEVICE_1;
    TEST_DEVICE_1.deviceId = TEST_DEVICE_ID;
    TEST_DEVICE_1.deviceKey = TEST_DEVICE_KEY;
    TEST_DEVICE_1.deviceSasToken = NULL;

    TRANSPORT_LL_HANDLE handle = IoTHubTransportMqtt_Create(&config);
    umock_c_reset_all_calls();

    EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG)).SetReturn(TEST_DEVICE_ID);
    EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG)).SetReturn(TEST_DEVICE_KEY);

    // act
    IOTHUB_DEVICE_HANDLE devHandle = IoTHubTransportMqtt_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, config.waitingToSend);

    IoTHubTransportMqtt_Unregister(devHandle);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

// Tests_SRS_IOTHUB_MQTT_TRANSPORT_17_004: [ IoTHubTransportMqtt_Register shall return the TRANSPORT_LL_HANDLE as the IOTHUB_DEVICE_HANDLE. ]
TEST_FUNCTION(IoTHubTransportMqtt_Register_Unregister_Register_returns_handle)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config ={ 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);

    IOTHUB_DEVICE_CONFIG TEST_DEVICE_1;
    TEST_DEVICE_1.deviceId = TEST_DEVICE_ID;
    TEST_DEVICE_1.deviceKey = TEST_DEVICE_KEY;
    TEST_DEVICE_1.deviceSasToken = NULL;

    TRANSPORT_LL_HANDLE handle = IoTHubTransportMqtt_Create(&config);
    IOTHUB_DEVICE_HANDLE devHandle = IoTHubTransportMqtt_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, config.waitingToSend);
    IoTHubTransportMqtt_Unregister(devHandle);

    umock_c_reset_all_calls();

    EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG)).SetReturn(TEST_DEVICE_ID);
    EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG)).SetReturn(TEST_DEVICE_KEY);

    // act
    IOTHUB_DEVICE_HANDLE devHandle2 = IoTHubTransportMqtt_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, config.waitingToSend);

    // assert
    ASSERT_IS_NOT_NULL(devHandle2);
    ASSERT_ARE_EQUAL(void_ptr, handle, devHandle2);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

/*Tests_SRS_IOTHUB_MQTT_TRANSPORT_02_001: [ If handle is NULL then IoTHubTransportMqtt_GetHostname shall fail and return NULL. ]*/
TEST_FUNCTION(IoTHubTransportMqtt_GetHostname_with_NULL_handle_fails)
{
    //arrange

    //act
    STRING_HANDLE hostname = IoTHubTransportMqtt_GetHostname(NULL);

    //assert
    ASSERT_IS_NULL(hostname);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
}

TEST_FUNCTION(IoTHubTransportMqtt_GetHostname_with_non_NULL_handle_succeeds)
{
    //arrange
    IOTHUBTRANSPORT_CONFIG config ={ 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);
    TRANSPORT_LL_HANDLE handle = IoTHubTransportMqtt_Create(&config);
    umock_c_reset_all_calls();

    //act
    STRING_HANDLE hostname = IoTHubTransportMqtt_GetHostname(handle);

    //assert
    ASSERT_IS_NOT_NULL(hostname);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

END_TEST_SUITE(iothubtransportmqtt_ut)
