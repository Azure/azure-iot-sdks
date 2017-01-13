// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <cstdlib>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include <cstddef>

#include "azure_c_shared_utility/sastoken.h"

#include "testrunnerswitcher.h"
#include "micromock.h"
#include "micromockcharstararenullterminatedstrings.h"
#include "azure_c_shared_utility/doublylinkedlist.h"
#include "azure_c_shared_utility/strings.h"
#include "azure_umqtt_c/mqtt_client.h"

#include "azure_c_shared_utility/macro_utils.h"
#undef DEFINE_ENUM
#define DEFINE_ENUM(enumName, ...) typedef enum C2(enumName, _TAG) { FOR_EACH_1(DEFINE_ENUMERATION_CONSTANT, __VA_ARGS__)} enumName; 

#include "iothubtransportmqtt.h"
#include "iothub_client_private.h"
#include "iothub_client_options.h"

#include "azure_c_shared_utility/xio.h"
#include "azure_c_shared_utility/tlsio.h"
#include "azure_c_shared_utility/platform.h"

#include "azure_c_shared_utility/tickcounter.h"
#include "azure_c_shared_utility/lock.h"
#include "azure_c_shared_utility/string_tokenizer.h"

#define GBALLOC_H
extern "C" int gballoc_init(void);
extern "C" void gballoc_deinit(void);
extern "C" void* gballoc_malloc(size_t size);
extern "C" void* gballoc_calloc(size_t nmemb, size_t size);
extern "C" void* gballoc_realloc(void* ptr, size_t size);
extern "C" void gballoc_free(void* ptr);

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

static const IOTHUB_DEVICE_CONFIG TEST_DEVICE_1 = { TEST_DEVICE_ID, TEST_DEVICE_KEY, NULL };

static const IOTHUB_CLIENT_LL_HANDLE TEST_IOTHUB_CLIENT_LL_HANDLE = (IOTHUB_CLIENT_LL_HANDLE)0x4343;
static const TRANSPORT_LL_HANDLE TEST_TRANSPORT_HANDLE = (TRANSPORT_LL_HANDLE)0x4444;
static const MQTT_CLIENT_HANDLE TEST_MQTT_CLIENT_HANDLE = (MQTT_CLIENT_HANDLE)0x1122;
static const PDLIST_ENTRY TEST_PDLIST_ENTRY = (PDLIST_ENTRY)0x1123;
static const MQTT_MESSAGE_HANDLE TEST_MQTT_MESSAGE_HANDLE = (MQTT_MESSAGE_HANDLE)0x1124;

static IO_INTERFACE_DESCRIPTION* TEST_IO_INTERFACE = (IO_INTERFACE_DESCRIPTION*)0x1125;
static XIO_HANDLE TEST_XIO_HANDLE = (XIO_HANDLE)0x1126;

static const STRING_TOKENIZER_HANDLE TEST_STRING_TOKENIZER_HANDLE = (STRING_TOKENIZER_HANDLE)0x1127;

/*this is the default message and has type BYTEARRAY*/
static IOTHUB_MESSAGE_HANDLE TEST_IOTHUB_MSG_BYTEARRAY = (IOTHUB_MESSAGE_HANDLE)0x01d1;

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

//Messages to be used in test
static IOTHUB_MESSAGE_LIST message1 =  /* this is the oldest message, always the first to be processed, send etc*/
{
    TEST_IOTHUB_MSG_BYTEARRAY,          /* IOTHUB_MESSAGE_HANDLE messageHandle;                 */
    NULL,                               /* IOTHUB_CLIENT_EVENT_CONFIRMATION_CALLBACK callback;  */
    NULL,                               /* void* context;                                       */
    { NULL, NULL }                      /* DLIST_ENTRY entry;                                   */
};

static IOTHUB_MESSAGE_LIST message2 =  /* this is the oldest message, always the first to be processed, send etc*/
{
    TEST_IOTHUB_MSG_STRING,             /* IOTHUB_MESSAGE_HANDLE messageHandle;                 */
    NULL,                               /* IOTHUB_CLIENT_EVENT_CONFIRMATION_CALLBACK callback;  */
    NULL,                               /* void* context;                                       */
    { NULL, NULL }                      /* DLIST_ENTRY entry;                                   */
};

static APP_PAYLOAD_TAG TEST_APP_PAYLOAD =
{
    appMessage, appMsgSize
};

DEFINE_MICROMOCK_ENUM_TO_STRING(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_RESULT_VALUES);
DEFINE_MICROMOCK_ENUM_TO_STRING(IOTHUB_CLIENT_STATUS, IOTHUB_CLIENT_STATUS_VALUES);

namespace BASEIMPLEMENTATION
{
#define Lock(x) (LOCK_OK + gballocState - gballocState) /*compiler warning about constant in if condition*/
#define Unlock(x) (LOCK_OK + gballocState - gballocState)
#define Lock_Init() (LOCK_HANDLE)0x42
#define Lock_Deinit(x) (LOCK_OK + gballocState - gballocState)
#include "gballoc.c"
#undef Lock
#undef Unlock
#undef Lock_Init
#undef Lock_Deinit

#include "doublylinkedlist.c"
#include "strings.c"
#include "buffer.c"
#include "base64.c"
};

static MICROMOCK_MUTEX_HANDLE g_testByTest;
static MICROMOCK_GLOBAL_SEMAPHORE_HANDLE g_dllByDll;

static size_t currentmalloc_call;
static size_t whenShallmalloc_fail;

static size_t currentSTRING_construct_call;
static size_t whenShallSTRING_construct_fail;

static size_t currentSTRING_concat_call;
static size_t whenShallSTRING_concat_fail;

//Callbacks for Testing
static ON_MQTT_MESSAGE_RECV_CALLBACK g_fnMqttMsgRecv;
static ON_MQTT_OPERATION_CALLBACK g_fnMqttOperationCallback;
static void* g_callbackCtx;
static bool g_nullMapVariable;

TYPED_MOCK_CLASS(CIoTHubTransportMqttMocks, CGlobalMock)
{
public:
    /* DoublyLinkedList mocks */
    MOCK_STATIC_METHOD_1(, void, DList_InitializeListHead, PDLIST_ENTRY, listHead)
        BASEIMPLEMENTATION::DList_InitializeListHead(listHead);
    MOCK_VOID_METHOD_END()

    MOCK_STATIC_METHOD_1(, int, DList_IsListEmpty, PDLIST_ENTRY, listHead)
        int result2 = BASEIMPLEMENTATION::DList_IsListEmpty(listHead);
    MOCK_METHOD_END(int, result2)

    MOCK_STATIC_METHOD_2(, void, DList_InsertTailList, PDLIST_ENTRY, listHead, PDLIST_ENTRY, listEntry)
        BASEIMPLEMENTATION::DList_InsertTailList(listHead, listEntry);
    MOCK_VOID_METHOD_END()

    MOCK_STATIC_METHOD_2(, void, DList_InsertHeadList, PDLIST_ENTRY, listHead, PDLIST_ENTRY, listEntry)
        BASEIMPLEMENTATION::DList_InsertHeadList(listHead, listEntry);
    MOCK_VOID_METHOD_END()

    MOCK_STATIC_METHOD_2(, void, DList_AppendTailList, PDLIST_ENTRY, listHead, PDLIST_ENTRY, ListToAppend)
        BASEIMPLEMENTATION::DList_AppendTailList(listHead, ListToAppend);
    MOCK_VOID_METHOD_END()

    MOCK_STATIC_METHOD_1(, int, DList_RemoveEntryList, PDLIST_ENTRY, listEntry)
    int result2 = BASEIMPLEMENTATION::DList_RemoveEntryList(listEntry);
    MOCK_METHOD_END(int, result2)

    MOCK_STATIC_METHOD_1(, PDLIST_ENTRY, DList_RemoveHeadList, PDLIST_ENTRY, listHead)
    PDLIST_ENTRY entry = BASEIMPLEMENTATION::DList_RemoveHeadList(listHead);
    MOCK_METHOD_END(PDLIST_ENTRY, entry)

    MOCK_STATIC_METHOD_2(, int, mallocAndStrcpy_s, char**, destination, const char*, source)
    MOCK_METHOD_END(int, (*destination = (char*)BASEIMPLEMENTATION::gballoc_malloc(strlen(source) + 1), strcpy(*destination, source), 0))

    MOCK_STATIC_METHOD_1(, void*, gballoc_malloc, size_t, size)
        void* result2;
        currentmalloc_call++;
        if (whenShallmalloc_fail > 0)
        {
            if (currentmalloc_call == whenShallmalloc_fail)
            {
                result2 = (void*)NULL;
            }
            else
            {
                result2 = BASEIMPLEMENTATION::gballoc_malloc(size);
            }
        }
        else
        {
            result2 = BASEIMPLEMENTATION::gballoc_malloc(size);
        }
    MOCK_METHOD_END(void*, result2);

    MOCK_STATIC_METHOD_2(, void*, gballoc_realloc, void*, ptr, size_t, size)
    MOCK_METHOD_END(void*, BASEIMPLEMENTATION::gballoc_realloc(ptr, size));

    MOCK_STATIC_METHOD_1(, void, gballoc_free, void*, ptr)
        BASEIMPLEMENTATION::gballoc_free(ptr);
    MOCK_VOID_METHOD_END()

    MOCK_STATIC_METHOD_3(, void, eventConfirmationCallback, IOTHUB_CLIENT_LL_HANDLE, iotHubClientHandle, IOTHUB_CLIENT_CONFIRMATION_RESULT, result2, void*, userContextCallback)
    MOCK_VOID_METHOD_END()

    MOCK_STATIC_METHOD_3(, int, messageCallback, IOTHUB_CLIENT_LL_HANDLE, iotHubClientHandle, IOTHUB_MESSAGE_HANDLE, message, void*, userContextCallback)
    MOCK_METHOD_END(int, 0);

    /*Strings*/
    MOCK_STATIC_METHOD_0(, STRING_HANDLE, STRING_new)
    MOCK_METHOD_END(STRING_HANDLE, BASEIMPLEMENTATION::STRING_new())

    MOCK_STATIC_METHOD_1(, STRING_HANDLE, STRING_construct, const char*, source)
        STRING_HANDLE result2;
        currentSTRING_construct_call++;
        if (whenShallSTRING_construct_fail > 0)
        {
            if (currentSTRING_construct_call == whenShallSTRING_construct_fail)
            {
                result2 = (STRING_HANDLE)NULL;
            }
            else
            {
                result2 = BASEIMPLEMENTATION::STRING_construct(source);
            }
        }
        else
        {
            result2 = BASEIMPLEMENTATION::STRING_construct(source);
        }
    MOCK_METHOD_END(STRING_HANDLE, result2)

    MOCK_STATIC_METHOD_2(, int, STRING_concat, STRING_HANDLE, s1, const char*, s2)
        currentSTRING_concat_call++;
    MOCK_METHOD_END(int, (((whenShallSTRING_concat_fail > 0) && (currentSTRING_concat_call == whenShallSTRING_concat_fail)) ? __LINE__ : BASEIMPLEMENTATION::STRING_concat(s1, s2)));

    MOCK_STATIC_METHOD_1(, void, STRING_delete, STRING_HANDLE, s)
        BASEIMPLEMENTATION::STRING_delete(s);
    MOCK_VOID_METHOD_END()

    MOCK_STATIC_METHOD_1(, const char*, STRING_c_str, STRING_HANDLE, s)
    MOCK_METHOD_END(const char*, BASEIMPLEMENTATION::STRING_c_str(s))

    MOCK_STATIC_METHOD_1(, STRING_HANDLE, STRING_clone, STRING_HANDLE, s)
    MOCK_METHOD_END(STRING_HANDLE, BASEIMPLEMENTATION::STRING_clone(s))

    /* IoTHubClient mocks*/
    MOCK_STATIC_METHOD_2(, IOTHUBMESSAGE_DISPOSITION_RESULT, IoTHubClient_LL_MessageCallback, IOTHUB_CLIENT_LL_HANDLE, handle, IOTHUB_MESSAGE_HANDLE, message)
    MOCK_METHOD_END(IOTHUBMESSAGE_DISPOSITION_RESULT, IOTHUBMESSAGE_ACCEPTED)

    MOCK_STATIC_METHOD_3(, void, IoTHubClient_LL_SendComplete, IOTHUB_CLIENT_LL_HANDLE, handle, PDLIST_ENTRY, completed, IOTHUB_CLIENT_CONFIRMATION_RESULT, result2)
    MOCK_VOID_METHOD_END()

    /* IoTHubMessage mocks */
    MOCK_STATIC_METHOD_1(, IOTHUBMESSAGE_CONTENT_TYPE, IoTHubMessage_GetContentType, IOTHUB_MESSAGE_HANDLE, iotHubMessageHandle)
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
    MOCK_METHOD_END(IOTHUBMESSAGE_CONTENT_TYPE, result2)

    MOCK_STATIC_METHOD_1(, const char*, IoTHubMessage_GetString, IOTHUB_MESSAGE_HANDLE, handle)
        const char* result2;
        if (handle == TEST_IOTHUB_MSG_STRING)
        {
            result2 = appMessageString;
        }
        else
        {
            result2 = (const char*)NULL;
        }
    MOCK_METHOD_END(const char*, result2)

    MOCK_STATIC_METHOD_2(, IOTHUB_MESSAGE_HANDLE, IoTHubMessage_CreateFromByteArray, const unsigned char*, buffer, size_t, size)
    MOCK_METHOD_END(IOTHUB_MESSAGE_HANDLE, TEST_IOTHUB_MSG_BYTEARRAY)

    MOCK_STATIC_METHOD_3(, IOTHUB_MESSAGE_RESULT, IoTHubMessage_GetByteArray, IOTHUB_MESSAGE_HANDLE, iotHubMessageHandle, const unsigned char**, buffer, size_t*, size)
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
    }
    MOCK_METHOD_END(IOTHUB_MESSAGE_RESULT, IOTHUB_MESSAGE_OK)

        MOCK_STATIC_METHOD_1(, void, IoTHubMessage_Destroy, IOTHUB_MESSAGE_HANDLE, iotHubMessageHandle)
        MOCK_VOID_METHOD_END()

        MOCK_STATIC_METHOD_1(, MAP_HANDLE, IoTHubMessage_Properties, IOTHUB_MESSAGE_HANDLE, iotHubMessageHandle)
        MOCK_METHOD_END(MAP_HANDLE, TEST_MESSAGE_PROP_MAP)

        MOCK_STATIC_METHOD_4(, MAP_RESULT, Map_GetInternals, MAP_HANDLE, handle, const char*const**, keys, const char*const**, values, size_t*, count);
    if (g_nullMapVariable)
    {
        *values = NULL;
        *keys = NULL;
        *count = 0;
    }
    MOCK_METHOD_END(MAP_RESULT, MAP_OK)

    MOCK_STATIC_METHOD_3(, MAP_RESULT, Map_AddOrUpdate, MAP_HANDLE, handle, const char*, key, const char*, value)
    MOCK_METHOD_END(MAP_RESULT, MAP_OK)

    MOCK_STATIC_METHOD_1(, void, Map_Destroy, MAP_HANDLE, handle)
    MOCK_VOID_METHOD_END()

    // Azure Mqtt
    MOCK_STATIC_METHOD_3(, MQTT_CLIENT_HANDLE, mqtt_client_init, ON_MQTT_MESSAGE_RECV_CALLBACK, msgRecv, ON_MQTT_OPERATION_CALLBACK, opCallback, void*, callbackCtx)
        g_fnMqttMsgRecv = msgRecv;
        g_fnMqttOperationCallback = opCallback;
        g_callbackCtx = callbackCtx;
    MOCK_METHOD_END(MQTT_CLIENT_HANDLE, TEST_MQTT_CLIENT_HANDLE);

    MOCK_STATIC_METHOD_3(, int, mqtt_client_connect, MQTT_CLIENT_HANDLE, handle, XIO_HANDLE, xioHandle, MQTT_CLIENT_OPTIONS*, mqttOptions)
    MOCK_METHOD_END(int, 0);

    MOCK_STATIC_METHOD_1(, void, mqtt_client_deinit, MQTT_CLIENT_HANDLE, handle)
    MOCK_VOID_METHOD_END();

    MOCK_STATIC_METHOD_1(, int, mqtt_client_disconnect, MQTT_CLIENT_HANDLE, handle)
        MOCK_METHOD_END(int, 0);

    MOCK_STATIC_METHOD_4(, int, mqtt_client_subscribe, MQTT_CLIENT_HANDLE, handle, uint16_t, packetId, SUBSCRIBE_PAYLOAD*, subscribeList, size_t, count)
        MOCK_METHOD_END(int, 0);

    MOCK_STATIC_METHOD_4(, int, mqtt_client_unsubscribe, MQTT_CLIENT_HANDLE, handle, uint16_t, packetId, const char**, unsubscribeList, size_t, count)
        MOCK_METHOD_END(int, 0);

    MOCK_STATIC_METHOD_2(, int, mqtt_client_publish, MQTT_CLIENT_HANDLE, handle, MQTT_MESSAGE_HANDLE, msgHandle)
        MOCK_METHOD_END(int, 0);

    MOCK_STATIC_METHOD_1(, void, mqtt_client_dowork, MQTT_CLIENT_HANDLE, handle)
        MOCK_VOID_METHOD_END()

        MOCK_STATIC_METHOD_3(, void, mqtt_client_set_trace, MQTT_CLIENT_HANDLE, handle, bool, traceVal, bool, rawTraceLog)
        MOCK_VOID_METHOD_END()

        MOCK_STATIC_METHOD_5(, MQTT_MESSAGE_HANDLE, mqttmessage_create, uint16_t, packetId, const char*, topicName, QOS_VALUE, qosValue, const uint8_t*, appMsg, size_t, appMsgLength)
        MOCK_METHOD_END(MQTT_MESSAGE_HANDLE, TEST_MQTT_MESSAGE_HANDLE);

    MOCK_STATIC_METHOD_1(, const APP_PAYLOAD*, mqttmessage_getApplicationMsg, MQTT_MESSAGE_HANDLE, handle)
        MOCK_METHOD_END(const APP_PAYLOAD*, &TEST_APP_PAYLOAD);

    MOCK_STATIC_METHOD_1(, const char*, mqttmessage_getTopicName, MQTT_MESSAGE_HANDLE, handle)
        MOCK_METHOD_END(const char*, TEST_MQTT_MSG_TOPIC);

    MOCK_STATIC_METHOD_1(, void, mqttmessage_destroy, MQTT_MESSAGE_HANDLE, handle)
        MOCK_VOID_METHOD_END()

        MOCK_STATIC_METHOD_1(, STRING_TOKENIZER_HANDLE, STRING_TOKENIZER_create, STRING_HANDLE, handle)
        MOCK_METHOD_END(STRING_TOKENIZER_HANDLE, TEST_STRING_TOKENIZER_HANDLE);

    MOCK_STATIC_METHOD_3(, int, STRING_TOKENIZER_get_next_token, STRING_TOKENIZER_HANDLE, t, STRING_HANDLE, output, const char*, delimiters)
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
    }
    if (text != NULL)
    {
        BASEIMPLEMENTATION::STRING_copy(output, text);
    }
    MOCK_METHOD_END(int, 0);

    MOCK_STATIC_METHOD_1(, void, STRING_TOKENIZER_destroy, STRING_TOKENIZER_HANDLE, handle)
        MOCK_VOID_METHOD_END()

        MOCK_STATIC_METHOD_4(, STRING_HANDLE, SASToken_Create, STRING_HANDLE, key, STRING_HANDLE, scope, STRING_HANDLE, keyName, size_t, expiry)
        MOCK_METHOD_END(STRING_HANDLE, BASEIMPLEMENTATION::STRING_construct(TEST_SAS_TOKEN));

    MOCK_STATIC_METHOD_1(, time_t, get_time, time_t*, currentTime)
        MOCK_METHOD_END(time_t, TEST_TIME_T);

    MOCK_STATIC_METHOD_0(, const IO_INTERFACE_DESCRIPTION*, platform_get_default_tlsio)
        MOCK_METHOD_END(const IO_INTERFACE_DESCRIPTION*, TEST_IO_INTERFACE)

    MOCK_STATIC_METHOD_2(, XIO_HANDLE, xio_create, const IO_INTERFACE_DESCRIPTION*, io_interface_description, const void*, xio_create_parameters)
    MOCK_METHOD_END(XIO_HANDLE, TEST_XIO_HANDLE);

    MOCK_STATIC_METHOD_3(, int, xio_close, XIO_HANDLE, ioHandle, ON_IO_CLOSE_COMPLETE, on_io_close_complete, void*, callback_context)
        MOCK_METHOD_END(int, 0);

    MOCK_STATIC_METHOD_3(, int, xio_setoption, XIO_HANDLE, xio, const char*, optionName, const void*, value)
    MOCK_METHOD_END(int, 0)

    MOCK_STATIC_METHOD_1(, void, xio_destroy, XIO_HANDLE, ioHandle)
    MOCK_VOID_METHOD_END();

    MOCK_STATIC_METHOD_0(, TICK_COUNTER_HANDLE, tickcounter_create)
    MOCK_METHOD_END(TICK_COUNTER_HANDLE, TEST_COUNTER_HANDLE);

    MOCK_STATIC_METHOD_1(, void, tickcounter_destroy, TICK_COUNTER_HANDLE, tick_counter)
    MOCK_VOID_METHOD_END();

    MOCK_STATIC_METHOD_2(, int, tickcounter_get_current_ms, TICK_COUNTER_HANDLE, tick_counter, uint64_t*, current_ms)
        *current_ms = g_current_ms;
    MOCK_METHOD_END(int, 0);

};

DECLARE_GLOBAL_MOCK_METHOD_0(CIoTHubTransportMqttMocks, , const IO_INTERFACE_DESCRIPTION*, platform_get_default_tlsio);

DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportMqttMocks, , XIO_HANDLE, xio_create, const IO_INTERFACE_DESCRIPTION*, io_interface_description, const void*, xio_create_parameters);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportMqttMocks, , void, xio_destroy, XIO_HANDLE, ioHandle);
DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubTransportMqttMocks, , int, xio_close, XIO_HANDLE, ioHandle, ON_IO_CLOSE_COMPLETE, on_io_close_complete, void*, callback_context);
DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubTransportMqttMocks, , int, xio_setoption, XIO_HANDLE, xio, const char*, optionName, const void*, value);

DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportMqttMocks, , void, DList_InitializeListHead, PDLIST_ENTRY, listHead);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportMqttMocks, , int, DList_IsListEmpty, PDLIST_ENTRY, listHead);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportMqttMocks, , void, DList_InsertTailList, PDLIST_ENTRY, listHead, PDLIST_ENTRY, listEntry);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportMqttMocks, , void, DList_InsertHeadList, PDLIST_ENTRY, listHead, PDLIST_ENTRY, listEntry);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportMqttMocks, , void, DList_AppendTailList, PDLIST_ENTRY, listHead, PDLIST_ENTRY, ListToAppend);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportMqttMocks, , int, DList_RemoveEntryList, PDLIST_ENTRY, listEntry);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportMqttMocks, , PDLIST_ENTRY, DList_RemoveHeadList, PDLIST_ENTRY, listHead);

DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportMqttMocks, , void*, gballoc_malloc, size_t, size);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportMqttMocks, , void*, gballoc_realloc, void*, ptr, size_t, size);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportMqttMocks, , void, gballoc_free, void*, ptr);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportMqttMocks, , int, mallocAndStrcpy_s, char**, destination, const char*, source);

DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubTransportMqttMocks, , void, eventConfirmationCallback, IOTHUB_CLIENT_LL_HANDLE, iotHubClientHandle, IOTHUB_CLIENT_CONFIRMATION_RESULT, result2, void*, userContextCallback);
DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubTransportMqttMocks, , int, messageCallback, IOTHUB_CLIENT_LL_HANDLE, iotHubClientHandle, IOTHUB_MESSAGE_HANDLE, message, void*, userContextCallback);

DECLARE_GLOBAL_MOCK_METHOD_0(CIoTHubTransportMqttMocks, , STRING_HANDLE, STRING_new);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportMqttMocks, , STRING_HANDLE, STRING_construct, const char*, s);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportMqttMocks, , int, STRING_concat, STRING_HANDLE, s1, const char*, s2);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportMqttMocks, , void, STRING_delete, STRING_HANDLE, s);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportMqttMocks, , const char*, STRING_c_str, STRING_HANDLE, s);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportMqttMocks, , STRING_HANDLE, STRING_clone, STRING_HANDLE, s);

DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportMqttMocks, , IOTHUB_MESSAGE_HANDLE, IoTHubMessage_CreateFromByteArray, const unsigned char*, buffer, size_t, size);
DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubTransportMqttMocks, , IOTHUB_MESSAGE_RESULT, IoTHubMessage_GetByteArray, IOTHUB_MESSAGE_HANDLE, iotHubMessageHandle, const unsigned char**, buffer, size_t*, size);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportMqttMocks, , void, IoTHubMessage_Destroy, IOTHUB_MESSAGE_HANDLE, iotHubMessageHandle);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportMqttMocks, , IOTHUBMESSAGE_CONTENT_TYPE, IoTHubMessage_GetContentType, IOTHUB_MESSAGE_HANDLE, iotHubMessageHandle);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportMqttMocks, , const char*, IoTHubMessage_GetString, IOTHUB_MESSAGE_HANDLE, handle);

DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportMqttMocks, , MAP_HANDLE, IoTHubMessage_Properties, IOTHUB_MESSAGE_HANDLE, iotHubMessageHandle);

DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubTransportMqttMocks, , MQTT_CLIENT_HANDLE, mqtt_client_init, ON_MQTT_MESSAGE_RECV_CALLBACK, msgRecv, ON_MQTT_OPERATION_CALLBACK, opCallback, void*, callbackCtx);
DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubTransportMqttMocks, , int, mqtt_client_connect, MQTT_CLIENT_HANDLE, handle, XIO_HANDLE, xioHandle, MQTT_CLIENT_OPTIONS*, mqttOptions);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportMqttMocks, , int, mqtt_client_disconnect, MQTT_CLIENT_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportMqttMocks, , void, mqtt_client_deinit, MQTT_CLIENT_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_4(CIoTHubTransportMqttMocks, , int, mqtt_client_subscribe, MQTT_CLIENT_HANDLE, handle, uint16_t, packetId, SUBSCRIBE_PAYLOAD*, subscribeList, size_t, count);
DECLARE_GLOBAL_MOCK_METHOD_4(CIoTHubTransportMqttMocks, , int, mqtt_client_unsubscribe, MQTT_CLIENT_HANDLE, handle, uint16_t, packetId, const char**, unsubscribeList, size_t, count);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportMqttMocks, , int, mqtt_client_publish, MQTT_CLIENT_HANDLE, handle, MQTT_MESSAGE_HANDLE, msgHandle);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportMqttMocks, , void, mqtt_client_dowork, MQTT_CLIENT_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubTransportMqttMocks, , void, mqtt_client_set_trace, MQTT_CLIENT_HANDLE, handle, bool, traceVal, bool, rawTraceLog);

DECLARE_GLOBAL_MOCK_METHOD_5(CIoTHubTransportMqttMocks, , MQTT_MESSAGE_HANDLE, mqttmessage_create, uint16_t, packetId, const char*, topicName, QOS_VALUE, qosValue, const uint8_t*, appMsg, size_t, appMsgLength);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportMqttMocks, , const APP_PAYLOAD*, mqttmessage_getApplicationMsg, MQTT_MESSAGE_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportMqttMocks, , const char*, mqttmessage_getTopicName, MQTT_MESSAGE_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportMqttMocks, , void, mqttmessage_destroy, MQTT_MESSAGE_HANDLE, handle);

DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportMqttMocks, , STRING_TOKENIZER_HANDLE, STRING_TOKENIZER_create, STRING_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubTransportMqttMocks, , int, STRING_TOKENIZER_get_next_token, STRING_TOKENIZER_HANDLE, t, STRING_HANDLE, output, const char*, delimiters);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportMqttMocks, , void, STRING_TOKENIZER_destroy, STRING_TOKENIZER_HANDLE, handle);

DECLARE_GLOBAL_MOCK_METHOD_4(CIoTHubTransportMqttMocks, , MAP_RESULT, Map_GetInternals, MAP_HANDLE, handle, const char*const**, keys, const char*const**, values, size_t*, count);
DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubTransportMqttMocks, , MAP_RESULT, Map_AddOrUpdate, MAP_HANDLE, handle, const char*, key, const char*, value);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportMqttMocks, , void, Map_Destroy, MAP_HANDLE, handle);

DECLARE_GLOBAL_MOCK_METHOD_4(CIoTHubTransportMqttMocks, , STRING_HANDLE, SASToken_Create, STRING_HANDLE, key, STRING_HANDLE, scope, STRING_HANDLE, keyName, size_t, expiry);

DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportMqttMocks, , IOTHUBMESSAGE_DISPOSITION_RESULT, IoTHubClient_LL_MessageCallback, IOTHUB_CLIENT_LL_HANDLE, handle, IOTHUB_MESSAGE_HANDLE, message);
DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubTransportMqttMocks, , void, IoTHubClient_LL_SendComplete, IOTHUB_CLIENT_LL_HANDLE, handle, PDLIST_ENTRY, completed, IOTHUB_CLIENT_CONFIRMATION_RESULT, result2);

DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportMqttMocks, , time_t, get_time, time_t*, currentTime);

DECLARE_GLOBAL_MOCK_METHOD_0(CIoTHubTransportMqttMocks, , TICK_COUNTER_HANDLE, tickcounter_create);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportMqttMocks, , void, tickcounter_destroy, TICK_COUNTER_HANDLE, tick_counter);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportMqttMocks, , int, tickcounter_get_current_ms, TICK_COUNTER_HANDLE, tick_counter, uint64_t*, current_ms);


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

BEGIN_TEST_SUITE(iothubtransportmqtt)

static void SetupMocksForInitConnection(CIoTHubTransportMqttMocks& mocks)
{
    (void)mocks;
    STRICT_EXPECTED_CALL(mocks, get_time(IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, STRING_new());
    EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG)).ExpectedTimesExactly(4);
    STRICT_EXPECTED_CALL(mocks, platform_get_default_tlsio());
    EXPECTED_CALL(mocks, mqtt_client_connect(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, SASToken_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_NUM_ARG));

    EXPECTED_CALL(mocks, xio_create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
}

TEST_SUITE_INITIALIZE(TestClassInitialize)
{
    TEST_INITIALIZE_MEMORY_DEBUG(g_dllByDll);
    g_testByTest = MicroMockCreateMutex();
    ASSERT_IS_NOT_NULL(g_testByTest);

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

TEST_SUITE_CLEANUP(TestClassCleanup)
{
    MicroMockDestroyMutex(g_testByTest);
    TEST_DEINITIALIZE_MEMORY_DEBUG(g_dllByDll);
}

TEST_FUNCTION_INITIALIZE(TestMethodInitialize)
{
    if (!MicroMockAcquireMutex(g_testByTest))
    {
        ASSERT_FAIL("our mutex is ABANDONED. Failure in test framework");
    }

    currentmalloc_call=0;
    whenShallmalloc_fail = 0;

    currentSTRING_construct_call = 0;;
    whenShallSTRING_construct_fail = 0;

    currentSTRING_concat_call = 0;
    whenShallSTRING_concat_fail = 0;

    g_fnMqttMsgRecv = NULL;
    g_fnMqttOperationCallback = NULL;
    g_callbackCtx = NULL;

    g_current_ms = 0;
    g_tokenizerIndex = 0;
    g_nullMapVariable = true;

    BASEIMPLEMENTATION::DList_InitializeListHead(&g_waitingToSend);
}

TEST_FUNCTION_CLEANUP(TestMethodCleanup)
{
    if (!MicroMockReleaseMutex(g_testByTest))
    {
        ASSERT_FAIL("failure in test framework at ReleaseMutex");
    }
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

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_001: [If parameter config is NULL then IoTHubTransportMqtt_Create shall return NULL.] */
TEST_FUNCTION(IoTHubTransportMqtt_Create_with_NULL_parameter_Succeed)
{
    ///arrange
    CIoTHubTransportMqttMocks mocks;

    ///act
    auto result = IoTHubTransportMqtt_Create(NULL);

    ///assert
    ASSERT_IS_NULL(result);

    ///cleanup
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_002: [If the parameter config's variables upperConfig or waitingToSend are NULL then IoTHubTransportMqtt_Create shall return NULL.] */
TEST_FUNCTION(IoTHubTransportMqtt_Create_with_NULL_config_parameter_fails)
{
    ///arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    config.waitingToSend = &g_waitingToSend;

    ///act
    auto result = IoTHubTransportMqtt_Create(&config);

    ///assert
    ASSERT_IS_NULL(result);

    ///cleanup
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_002: [If the parameter config's variables upperConfig or waitingToSend are NULL then IoTHubTransportMqtt_Create shall return NULL.] */
TEST_FUNCTION(IoTHubTransportMqtt_Create_with_NULL_waitingToSend_fails)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);
    config.waitingToSend = NULL;

    // act
    auto result = IoTHubTransportMqtt_Create(&config);

    // assert
    ASSERT_IS_NULL(result);
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_009: [If any error is encountered then IoTHubTransportMqtt_Create shall return NULL.] */
TEST_FUNCTION(IoTHubTransportMqtt_Create_with_NULL_protocol_fails)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);
    g_iothubClientConfig.protocol = NULL;

    // act
    auto result = IoTHubTransportMqtt_Create(&config);

    // assert
    ASSERT_IS_NULL(result);
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_003: [If the upperConfig's variables deviceId, deviceKey, iotHubName, protocol, or iotHubSuffix are NULL then IoTHubTransportMqtt_Create shall return NULL.] */
TEST_FUNCTION(IoTHubTransportMqtt_Create_with_NULL_device_id_fails)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, NULL, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    // act
    auto result = IoTHubTransportMqtt_Create(&config);

    // assert
    ASSERT_IS_NULL(result);
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_005: [If the upperConfig's variables deviceKey, iotHubName, or iotHubSuffix are empty strings then IoTHubTransportMqtt_Create shall return NULL.] */
TEST_FUNCTION(IoTHubTransportMqtt_Create_with_deviceSasToken_blank_fails)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfigWithKeyAndSasToken(&config, TEST_DEVICE_ID, NULL, TEST_EMPTY_STRING, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    // act
    auto result = IoTHubTransportMqtt_Create(&config);

    // assert
    ASSERT_IS_NULL(result);
}

TEST_FUNCTION(IoTHubTransportMqtt_Create_with_deviceKey_blank_fails)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config ={ 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, "", TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    // act
    auto result = IoTHubTransportMqtt_Create(&config);

    // assert
    ASSERT_IS_NULL(result);
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_003: [If the upperConfig's variables deviceId, deviceKey, iotHubName, protocol, or iotHubSuffix are NULL then IoTHubTransportMqtt_Create shall return NULL.] */
TEST_FUNCTION(IoTHubTransportMqtt_Create_with_NULL_iothub_name_fails)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, NULL, TEST_IOTHUB_SUFFIX, NULL);

    // act
    auto result = IoTHubTransportMqtt_Create(&config);

    // assert
    ASSERT_IS_NULL(result);
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_003: [If the upperConfig's variables deviceId, deviceKey, iotHubName, protocol, or iotHubSuffix are NULL then IoTHubTransportMqtt_Create shall return NULL.] */
TEST_FUNCTION(IoTHubTransportMqtt_Create_tickcounter_create_null_fails)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config ={ 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    STRICT_EXPECTED_CALL(mocks, tickcounter_create()).SetReturn((TICK_COUNTER_HANDLE)NULL);

    // act
    auto result = IoTHubTransportMqtt_Create(&config);

    // assert
    ASSERT_IS_NULL(result);
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_003: [If the upperConfig's variables deviceId, deviceKey, iotHubName, protocol, or iotHubSuffix are NULL then IoTHubTransportMqtt_Create shall return NULL.] */
TEST_FUNCTION(IoTHubTransportMqtt_Create_with_NULL_iothub_suffix_fails)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, NULL, NULL);

    // act
    auto result = IoTHubTransportMqtt_Create(&config);

    // assert
    ASSERT_IS_NULL(result);
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_006: [If the upperConfig's variables deviceId is an empty strings or length is greater then 128 then IoTHubTransportMqtt_Create shall return NULL.] */
TEST_FUNCTION(IoTHubTransportMqtt_Create_with_very_long_device_id_fails)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_VERY_LONG_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    // act
    auto result = IoTHubTransportMqtt_Create(&config);

    // assert
    ASSERT_IS_NULL(result);
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_007: [If the upperConfig's variables protocolGatewayHostName is non-Null and the length is an empty string then IoTHubTransportMqtt_Create shall return NULL.] */
TEST_FUNCTION(IoTHubTransportMqtt_Create_with_NULL_protocol_gateway_hostname_Succeeds)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG));

    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_DEVICE_ID));
    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_DEVICE_KEY));
    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_MQTT_MESSAGE_TOPIC));
    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_MQTT_EVENT_TOPIC));
    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_MQTT_SAS_TOKEN));
    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_HOST_NAME));

    EXPECTED_CALL(mocks, mqtt_client_init(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, DList_InitializeListHead(IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, STRING_construct(IGNORED_PTR_ARG)).IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, tickcounter_create());

    EXPECTED_CALL(mocks, gballoc_free(NULL));
    EXPECTED_CALL(mocks, gballoc_free(NULL));
    EXPECTED_CALL(mocks, gballoc_free(NULL));
    EXPECTED_CALL(mocks, gballoc_free(NULL));

    // act
    auto result = IoTHubTransportMqtt_Create(&config);

    // assert
    ASSERT_IS_NOT_NULL(result);
    mocks.AssertActualAndExpectedCalls();

    // clean up
    IoTHubTransportMqtt_Destroy(result);
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_010: [IoTHubTransportMqtt_Create shall allocate memory to save its internal state where all topics, hostname, device_id, device_key, sasTokenSr and client handle shall be saved.] */
/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_011: [On Success IoTHubTransportMqtt_Create shall return a non-NULL value.] */
TEST_FUNCTION(IoTHubTransportMqtt_Create_validConfig_Succeed)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG));

    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_DEVICE_ID));
    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_DEVICE_KEY));
    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_MQTT_MESSAGE_TOPIC));
    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_MQTT_EVENT_TOPIC));
    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_MQTT_SAS_TOKEN));
    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_HOST_NAME));

    EXPECTED_CALL(mocks, mqtt_client_init(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, DList_InitializeListHead(IGNORED_PTR_ARG));

    STRICT_EXPECTED_CALL(mocks, STRING_construct(IGNORED_PTR_ARG)).IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, tickcounter_create());

    EXPECTED_CALL(mocks, gballoc_free(NULL));
    EXPECTED_CALL(mocks, gballoc_free(NULL));
    EXPECTED_CALL(mocks, gballoc_free(NULL));
    EXPECTED_CALL(mocks, gballoc_free(NULL));

    // act
    auto result = IoTHubTransportMqtt_Create(&config);

    // assert
    ASSERT_IS_NOT_NULL(result);
    mocks.AssertActualAndExpectedCalls();

    // clean up
    IoTHubTransportMqtt_Destroy(result);
}

TEST_FUNCTION(IoTHubTransportMqtt_Create_alloc_return_null_fail)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config ={ 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG));

    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_DEVICE_ID));
    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_DEVICE_KEY));
    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_MQTT_MESSAGE_TOPIC));
    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_MQTT_EVENT_TOPIC));
    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_MQTT_SAS_TOKEN));
    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_HOST_NAME));

    EXPECTED_CALL(mocks, mqtt_client_init(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));

    EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, gballoc_free(NULL));

    STRICT_EXPECTED_CALL(mocks, tickcounter_create());
    STRICT_EXPECTED_CALL(mocks, tickcounter_destroy(TEST_COUNTER_HANDLE));

    EXPECTED_CALL(mocks, gballoc_free(NULL));
    EXPECTED_CALL(mocks, gballoc_free(NULL));
    EXPECTED_CALL(mocks, gballoc_free(NULL));

    whenShallmalloc_fail = 5;

    // act
    auto result = IoTHubTransportMqtt_Create(&config);

    // assert
    ASSERT_IS_NULL(result);
    mocks.AssertActualAndExpectedCalls();

    // clean up
    IoTHubTransportMqtt_Destroy(result);
}

TEST_FUNCTION(IoTHubTransportMqtt_Create_device_key_NULL_fail)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config ={ 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    whenShallSTRING_construct_fail = 2;

    EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG));
    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_DEVICE_ID));
    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_DEVICE_KEY));

    STRICT_EXPECTED_CALL(mocks, tickcounter_create());
    STRICT_EXPECTED_CALL(mocks, tickcounter_destroy(TEST_COUNTER_HANDLE));

    EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, gballoc_free(NULL));

    // act
    auto result = IoTHubTransportMqtt_Create(&config);

    // assert
    ASSERT_IS_NULL(result);
    mocks.AssertActualAndExpectedCalls();

    // clean up
    IoTHubTransportMqtt_Destroy(result);
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_009: [If any error is encountered then IoTHubTransportMqtt_Create shall return NULL.] */
TEST_FUNCTION(IoTHubTransportMqtt_Create_hostAddress_fails)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    whenShallSTRING_construct_fail = 6;

    EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG));

    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_DEVICE_ID));
    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_DEVICE_KEY));
    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_MQTT_MESSAGE_TOPIC));
    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_MQTT_EVENT_TOPIC));
    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_MQTT_SAS_TOKEN));
    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_HOST_NAME));

    EXPECTED_CALL(mocks, mqtt_client_init(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));

    EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));

    STRICT_EXPECTED_CALL(mocks, tickcounter_create());
    STRICT_EXPECTED_CALL(mocks, tickcounter_destroy(TEST_COUNTER_HANDLE));

    EXPECTED_CALL(mocks, gballoc_free(NULL));
    EXPECTED_CALL(mocks, gballoc_free(NULL));
    EXPECTED_CALL(mocks, gballoc_free(NULL));
    EXPECTED_CALL(mocks, gballoc_free(NULL));

    // act
    auto result = IoTHubTransportMqtt_Create(&config);

    // assert
    ASSERT_IS_NULL(result);
    mocks.AssertActualAndExpectedCalls();

    // clean up
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_003: [If the upperConfig's variables deviceId, deviceKey, iotHubName, protocol, or iotHubSuffix are NULL then IoTHubTransportMqtt_Create shall return NULL.] */
TEST_FUNCTION(IoTHubTransportMqtt_Create_with_empty_device_id_fails)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_EMPTY_STRING, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    // act
    auto result = IoTHubTransportMqtt_Create(&config);

    // assert
    ASSERT_IS_NULL(result);
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_003: [If the upperConfig's variables deviceId, deviceKey, iotHubName, protocol, or iotHubSuffix are NULL then IoTHubTransportMqtt_Create shall return NULL.] */
TEST_FUNCTION(IoTHubTransportMqtt_Create_with_empty_device_key_fails)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_EMPTY_STRING, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    // act
    auto result = IoTHubTransportMqtt_Create(&config);

    // assert
    ASSERT_IS_NULL(result);
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_03_003: [If both deviceKey & deviceSasToken fields are NOT NULL then IoTHubTransportMqtt_Create shall return NULL.]*/
TEST_FUNCTION(IoTHubTransportMqtt_Create_with_both_deviceKey_and_deviceSasToken_defined_fails)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config = { 0 };

    SetupIothubTransportConfigWithKeyAndSasToken(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_DEVICE_SAS, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    // act
    auto result = IoTHubTransportMqtt_Create(&config);

    // assert
    ASSERT_IS_NULL(result);
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_003: [If the upperConfig's variables deviceId, deviceKey, iotHubName, protocol, or iotHubSuffix are NULL then IoTHubTransportMqtt_Create shall return NULL.] */
TEST_FUNCTION(IoTHubTransportMqtt_Create_with_empty_iothub_name_fails)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_EMPTY_STRING, TEST_IOTHUB_SUFFIX, NULL);

    // act
    auto result = IoTHubTransportMqtt_Create(&config);

    // assert
    ASSERT_IS_NULL(result);
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_011: [On Success IoTHubTransportMqtt_Create shall return a non-NULL value.] */
TEST_FUNCTION(IoTHubTransportMqtt_Create_validConfig_String_construct_fails_fail)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    whenShallSTRING_construct_fail = 5;

    EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG));

    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_DEVICE_ID));
    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_DEVICE_KEY));
    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_MQTT_SAS_TOKEN));
    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_MQTT_MESSAGE_TOPIC));
    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_MQTT_EVENT_TOPIC));

    EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));

    STRICT_EXPECTED_CALL(mocks, tickcounter_create());
    STRICT_EXPECTED_CALL(mocks, tickcounter_destroy(TEST_COUNTER_HANDLE));

    EXPECTED_CALL(mocks, gballoc_free(NULL));
    EXPECTED_CALL(mocks, gballoc_free(NULL));
    EXPECTED_CALL(mocks, gballoc_free(NULL));
    EXPECTED_CALL(mocks, gballoc_free(NULL));

    // act
    auto result = IoTHubTransportMqtt_Create(&config);

    // assert
    ASSERT_IS_NULL(result);
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_011: [On Success IoTHubTransportMqtt_Create shall return a non-NULL value.] */
TEST_FUNCTION(IoTHubTransportMqtt_Create_validConfig_string_contruct_for_message_fails_fail)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    whenShallSTRING_construct_fail = 4;

    EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG));

    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_DEVICE_ID));
    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_DEVICE_KEY));
    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_MQTT_SAS_TOKEN));
    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_MQTT_EVENT_TOPIC));

    EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));

    STRICT_EXPECTED_CALL(mocks, tickcounter_create());
    STRICT_EXPECTED_CALL(mocks, tickcounter_destroy(TEST_COUNTER_HANDLE));

    EXPECTED_CALL(mocks, gballoc_free(NULL));
    EXPECTED_CALL(mocks, gballoc_free(NULL));
    EXPECTED_CALL(mocks, gballoc_free(NULL));

    // act
    auto result = IoTHubTransportMqtt_Create(&config);

    // assert
    ASSERT_IS_NULL(result);
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_011: [On Success IoTHubTransportMqtt_Create shall return a non-NULL value.] */
TEST_FUNCTION(IoTHubTransportMqtt_Create_validConfig_string_construct_sasTokenSr_fails_fail)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    whenShallSTRING_construct_fail = 3;

    EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG));

    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_DEVICE_ID));
    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_DEVICE_KEY));
    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_MQTT_SAS_TOKEN));

    EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));

    STRICT_EXPECTED_CALL(mocks, tickcounter_create());
    STRICT_EXPECTED_CALL(mocks, tickcounter_destroy(TEST_COUNTER_HANDLE));

    EXPECTED_CALL(mocks, gballoc_free(NULL));
    EXPECTED_CALL(mocks, gballoc_free(NULL));

    // act
    auto result = IoTHubTransportMqtt_Create(&config);

    // assert
    ASSERT_IS_NULL(result);
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_011: [On Success IoTHubTransportMqtt_Create shall return a non-NULL value.] */
TEST_FUNCTION(IoTHubTransportMqtt_Create_validConfig_state_Allocation_fails_fail)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    STRICT_EXPECTED_CALL(mocks, tickcounter_create());
    STRICT_EXPECTED_CALL(mocks, tickcounter_destroy(TEST_COUNTER_HANDLE));

    whenShallmalloc_fail = 1;
    EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG));

    // act
    auto result = IoTHubTransportMqtt_Create(&config);

    // assert
    ASSERT_IS_NULL(result);
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_011: [On Success IoTHubTransportMqtt_Create shall return a non-NULL value.] */
TEST_FUNCTION(IoTHubTransportMqtt_Create_validConfig_mqtt_client_init_Create_fails_fail)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG));

    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_DEVICE_ID));
    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_DEVICE_KEY));
    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_MQTT_MESSAGE_TOPIC));
    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_MQTT_EVENT_TOPIC));
    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_MQTT_SAS_TOKEN));

    EXPECTED_CALL(mocks, mqtt_client_init(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG)).SetReturn((MQTT_CLIENT_HANDLE)NULL);

    EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));

    STRICT_EXPECTED_CALL(mocks, tickcounter_create());
    STRICT_EXPECTED_CALL(mocks, tickcounter_destroy(TEST_COUNTER_HANDLE));

    EXPECTED_CALL(mocks, gballoc_free(NULL));
    EXPECTED_CALL(mocks, gballoc_free(NULL));
    EXPECTED_CALL(mocks, gballoc_free(NULL));
    EXPECTED_CALL(mocks, gballoc_free(NULL));

    // act
    auto result = IoTHubTransportMqtt_Create(&config);

    // assert
    ASSERT_IS_NULL(result);
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_012: [IoTHubTransportMqtt_Destroy shall do nothing if parameter handle is NULL.] */
TEST_FUNCTION(IoTHubTransportMqtt_Destroy_parameter_NULL_succeed)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;

    // act
    IoTHubTransportMqtt_Destroy(NULL);

    // assert
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_014: [IoTHubTransportMqtt_Destroy shall free all the resources currently in use.] */
TEST_FUNCTION(IoTHubTransportMqtt_Destroy_One_Message_Ack_succeeds)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;

    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    QOS_VALUE QosValue[] = { DELIVER_AT_LEAST_ONCE };
    SUBSCRIBE_ACK suback;
    suback.packetId = 1234;
    suback.qosCount = 1;
    suback.qosReturn = QosValue;

    DList_InsertTailList(config.waitingToSend, &(message1.entry));
    auto handle = IoTHubTransportMqtt_Create(&config);
    g_fnMqttOperationCallback(TEST_MQTT_CLIENT_HANDLE, MQTT_CLIENT_ON_SUBSCRIBE_ACK, &suback, g_callbackCtx);
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);
    mocks.ResetAllCalls();

    EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).ExpectedTimesExactly(2);
    EXPECTED_CALL(mocks, DList_RemoveHeadList(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, DList_InitializeListHead(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, DList_InsertTailList(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_SendComplete(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IOTHUB_CLIENT_CONFIRMATION_BECAUSE_DESTROY))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    EXPECTED_CALL(mocks, STRING_delete(NULL));
    EXPECTED_CALL(mocks, STRING_delete(NULL));
    EXPECTED_CALL(mocks, STRING_delete(NULL));
    EXPECTED_CALL(mocks, STRING_delete(NULL));
    EXPECTED_CALL(mocks, STRING_delete(NULL));
    EXPECTED_CALL(mocks, STRING_delete(NULL));
    EXPECTED_CALL(mocks, STRING_delete(NULL));

    STRICT_EXPECTED_CALL(mocks, mqtt_client_disconnect(TEST_MQTT_CLIENT_HANDLE));
    STRICT_EXPECTED_CALL(mocks, mqtt_client_deinit(TEST_MQTT_CLIENT_HANDLE));
    EXPECTED_CALL(mocks, gballoc_free(NULL));
    EXPECTED_CALL(mocks, gballoc_free(NULL));
    STRICT_EXPECTED_CALL(mocks, xio_destroy(TEST_XIO_HANDLE));
    STRICT_EXPECTED_CALL(mocks, tickcounter_destroy(TEST_COUNTER_HANDLE));

    // act
    IoTHubTransportMqtt_Destroy(handle);

    // assert
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_014: [IoTHubTransportMqtt_Destroy shall free all the resources currently in use.] */
TEST_FUNCTION(IoTHubTransportMqtt_Destroy_succeeds)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    auto handle = IoTHubTransportMqtt_Create(&config);
    mocks.ResetAllCalls();

    EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG));

    EXPECTED_CALL(mocks, STRING_delete(NULL));
    EXPECTED_CALL(mocks, STRING_delete(NULL));
    EXPECTED_CALL(mocks, STRING_delete(NULL));
    EXPECTED_CALL(mocks, STRING_delete(NULL));
    EXPECTED_CALL(mocks, STRING_delete(NULL));
    EXPECTED_CALL(mocks, STRING_delete(NULL));
    EXPECTED_CALL(mocks, STRING_delete(NULL));

    STRICT_EXPECTED_CALL(mocks, mqtt_client_deinit(TEST_MQTT_CLIENT_HANDLE));
    STRICT_EXPECTED_CALL(mocks, mqtt_client_disconnect(TEST_MQTT_CLIENT_HANDLE));
    EXPECTED_CALL(mocks, xio_destroy(NULL));
    EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, tickcounter_destroy(TEST_COUNTER_HANDLE));

    // act
    IoTHubTransportMqtt_Destroy(handle);

    // assert
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_015: [If parameter handle is NULL than IoTHubTransportMqtt_Subscribe shall return a non-zero value.] */
TEST_FUNCTION(IoTHubTransportMqtt_Subscribe_parameter_NULL_fail)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;

    // act
    int res = IoTHubTransportMqtt_Subscribe(NULL);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, res);
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_017: [Upon failure IoTHubTransportMqtt_Subscribe shall return a non-zero value.] */
TEST_FUNCTION(IoTHubTransportMqtt_Subscribe_mqtt_client_subscribe_return_error_fail)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    auto handle = IoTHubTransportMqtt_Create(&config);
    (void)IoTHubTransportMqtt_Subscribe(handle);
    CONNECT_ACK connack = { true, CONNECTION_ACCEPTED };
    g_fnMqttOperationCallback(TEST_MQTT_CLIENT_HANDLE, MQTT_CLIENT_ON_CONNACK, &connack, g_callbackCtx);

    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, mqtt_client_subscribe(TEST_MQTT_CLIENT_HANDLE, IGNORED_NUM_ARG, IGNORED_PTR_ARG, 1))
        .IgnoreArgument(2)
        .IgnoreArgument(3)
        .SetReturn(__LINE__);
    EXPECTED_CALL(mocks, STRING_c_str(NULL));
    STRICT_EXPECTED_CALL(mocks, mqtt_client_dowork(TEST_MQTT_CLIENT_HANDLE));
    STRICT_EXPECTED_CALL(mocks, tickcounter_get_current_ms(TEST_COUNTER_HANDLE, IGNORED_PTR_ARG))
        .IgnoreArgument(2)
        .ExpectedTimesExactly(3);
    SetupMocksForInitConnection(mocks);

    // act
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // assert
    mocks.AssertActualAndExpectedCalls();

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_016: [IoTHubTransportMqtt_Subscribe shall set a flag to enable mqtt_client_subscribe to be called to subscribe to the Message Topic.] */
/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_018: [On success IoTHubTransportMqtt_Subscribe shall return 0.] */
TEST_FUNCTION(IoTHubTransportMqtt_Subscribe_Succeed)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    CONNECT_ACK connack = { true, CONNECTION_ACCEPTED };
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    TRANSPORT_LL_HANDLE handle = IoTHubTransportMqtt_Create(&config);

    g_fnMqttOperationCallback(TEST_MQTT_CLIENT_HANDLE, MQTT_CLIENT_ON_CONNACK, &connack, g_callbackCtx);
    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, mqtt_client_subscribe(TEST_MQTT_CLIENT_HANDLE, IGNORED_NUM_ARG, IGNORED_PTR_ARG, 1))
        .IgnoreArgument(2)
        .IgnoreArgument(3);
    EXPECTED_CALL(mocks, STRING_c_str(NULL) ).SetReturn(TEST_MQTT_MESSAGE_TOPIC);
    STRICT_EXPECTED_CALL(mocks, mqtt_client_dowork(TEST_MQTT_CLIENT_HANDLE) );
    STRICT_EXPECTED_CALL(mocks, tickcounter_get_current_ms(TEST_COUNTER_HANDLE, IGNORED_PTR_ARG))
        .IgnoreArgument(2)
        .ExpectedAtLeastTimes(3);
    SetupMocksForInitConnection(mocks);

    // act
    int result = IoTHubTransportMqtt_Subscribe(handle);
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ASSERT_ARE_EQUAL(int, result, 0);

    // assert
    mocks.AssertActualAndExpectedCalls();

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_017: [Upon failure IoTHubTransportMqtt_Subscribe shall return a non-zero value.] */
TEST_FUNCTION(IoTHubTransportMqtt_Subscribe_set_subscribe_type_Succeed)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    CONNECT_ACK connack = { true, CONNECTION_ACCEPTED };
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    TRANSPORT_LL_HANDLE handle = IoTHubTransportMqtt_Create(&config);

    g_fnMqttOperationCallback(TEST_MQTT_CLIENT_HANDLE, MQTT_CLIENT_ON_CONNACK, &connack, g_callbackCtx);
    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, mqtt_client_subscribe(TEST_MQTT_CLIENT_HANDLE, IGNORED_NUM_ARG, IGNORED_PTR_ARG, 1))
        .IgnoreArgument(2)
        .IgnoreArgument(3);
    EXPECTED_CALL(mocks, STRING_c_str(NULL)).SetReturn(TEST_MQTT_MESSAGE_TOPIC);
    STRICT_EXPECTED_CALL(mocks, mqtt_client_dowork(TEST_MQTT_CLIENT_HANDLE));
    STRICT_EXPECTED_CALL(mocks, tickcounter_get_current_ms(TEST_COUNTER_HANDLE, IGNORED_PTR_ARG))
        .IgnoreArgument(2)
        .ExpectedTimesExactly(3);
    SetupMocksForInitConnection(mocks);

    // act
    int result = IoTHubTransportMqtt_Subscribe(handle);
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ASSERT_ARE_EQUAL(int, result, 0);

    // assert
    mocks.AssertActualAndExpectedCalls();

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

TEST_FUNCTION(IoTHubTransportMqtt_Subscribe_set_subscribe_after_publish_Succeed)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    CONNECT_ACK connack = { true, CONNECTION_ACCEPTED };
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    QOS_VALUE QosValue[] = { DELIVER_AT_LEAST_ONCE };
    SUBSCRIBE_ACK suback;
    suback.packetId = 1234;
    suback.qosCount = 1;
    suback.qosReturn = QosValue;

    TRANSPORT_LL_HANDLE handle = IoTHubTransportMqtt_Create(&config);

    g_fnMqttOperationCallback(TEST_MQTT_CLIENT_HANDLE, MQTT_CLIENT_ON_SUBSCRIBE_ACK, &suback, g_callbackCtx);
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, mqtt_client_subscribe(TEST_MQTT_CLIENT_HANDLE, IGNORED_NUM_ARG, IGNORED_PTR_ARG, 1))
        .IgnoreArgument(2)
        .IgnoreArgument(3);
    EXPECTED_CALL(mocks, STRING_c_str(NULL)).SetReturn(TEST_MQTT_MESSAGE_TOPIC);
    STRICT_EXPECTED_CALL(mocks, mqtt_client_dowork(TEST_MQTT_CLIENT_HANDLE)).ExpectedAtLeastTimes(2);
    STRICT_EXPECTED_CALL(mocks, tickcounter_get_current_ms(TEST_COUNTER_HANDLE, IGNORED_PTR_ARG))
        .IgnoreArgument(2)
        .ExpectedTimesExactly(2);

    // act
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);
    int result = IoTHubTransportMqtt_Subscribe(handle);
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ASSERT_ARE_EQUAL(int, result, 0);

    // assert
    mocks.AssertActualAndExpectedCalls();

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_019: [If parameter handle is NULL then IoTHubTransportMqtt_Unsubscribe shall do nothing.] */
TEST_FUNCTION(IoTHubTransportMqtt_Unsubscribe_parameter_NULL_fail)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;

    // act
    IoTHubTransportMqtt_Unsubscribe(NULL);

    // assert
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_020: [IoTHubTransportMqtt_Unsubscribe shall call mqtt_client_unsubscribe to unsubscribe the mqtt message topic.] */
TEST_FUNCTION(IoTHubTransportMqtt_Unsubscribe_Succeeds)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    auto handle = IoTHubTransportMqtt_Create(&config);
    IoTHubTransportMqtt_Subscribe(handle);

    CONNECT_ACK connack = { true, CONNECTION_ACCEPTED };
    g_fnMqttOperationCallback(TEST_MQTT_CLIENT_HANDLE, MQTT_CLIENT_ON_CONNACK, &connack, g_callbackCtx);

    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, mqtt_client_unsubscribe(TEST_MQTT_CLIENT_HANDLE, IGNORED_NUM_ARG, IGNORED_PTR_ARG, 1))
        .IgnoreArgument(2)
        .IgnoreArgument(3);
    EXPECTED_CALL(mocks, STRING_c_str(NULL)).SetReturn(TEST_MQTT_MESSAGE_TOPIC);

    // act
    IoTHubTransportMqtt_Unsubscribe(handle);

    // assert
    mocks.AssertActualAndExpectedCalls();

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_032: [IoTHubTransportMqtt_SetOption shall pass down the option to xio_setoption if the option parameter is not a known option string for the MQTT transport.] */
TEST_FUNCTION(IoTHubTransportMqtt_Setoption_invokes_xio_setoption_when_option_not_consumed_by_mqtt_transport)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;

    const char* SOME_OPTION = "AnOption";
    const void* SOME_VALUE = (void*)42;

    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    auto handle = IoTHubTransportMqtt_Create(&config);
    mocks.ResetAllCalls();

    EXPECTED_CALL(mocks, STRING_c_str(NULL));
    EXPECTED_CALL(mocks, platform_get_default_tlsio());
    EXPECTED_CALL(mocks, xio_create(NULL, NULL));
    STRICT_EXPECTED_CALL(mocks, xio_setoption(NULL, SOME_OPTION, SOME_VALUE))
        .IgnoreArgument(1);

    // act
    auto result = IoTHubTransportMqtt_SetOption(handle, SOME_OPTION, SOME_VALUE);

    // assert
    ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK, result);

    mocks.AssertActualAndExpectedCalls();

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_032: [IoTHubTransportMqtt_SetOption shall pass down the option to xio_setoption if the option parameter is not a known option string for the MQTT transport.] */
TEST_FUNCTION(IoTHubTransportMqtt_Setoption_xio_create_fail)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;

    const char* SOME_OPTION = "AnOption";
    const void* SOME_VALUE = (void*)42;

    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    auto handle = IoTHubTransportMqtt_Create(&config);
    mocks.ResetAllCalls();

    EXPECTED_CALL(mocks, STRING_c_str(NULL));
    EXPECTED_CALL(mocks, platform_get_default_tlsio());
    EXPECTED_CALL(mocks, xio_create(NULL, NULL)).SetReturn((XIO_HANDLE)NULL);

    // act
    auto result = IoTHubTransportMqtt_SetOption(handle, SOME_OPTION, SOME_VALUE);

    // assert
    ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_ERROR, result);

    mocks.AssertActualAndExpectedCalls();

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_132: [IoTHubTransportMqtt_SetOption shall return IOTHUB_CLIENT_INVALID_ARG xio_setoption fails] */
TEST_FUNCTION(IoTHubTransportMqtt_Setoption_fails_when_xio_setoption_fails)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;

    const char* SOME_OPTION = "AnOption";
    const void* SOME_VALUE = (void*)42;

    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    auto handle = IoTHubTransportMqtt_Create(&config);
    mocks.ResetAllCalls();

    EXPECTED_CALL(mocks, STRING_c_str(NULL));
    EXPECTED_CALL(mocks, platform_get_default_tlsio());
    EXPECTED_CALL(mocks, xio_create(NULL, NULL));
    STRICT_EXPECTED_CALL(mocks, xio_setoption(NULL, SOME_OPTION, SOME_VALUE))
        .IgnoreArgument(1)
        .SetReturn(42);

    // act
    auto result = IoTHubTransportMqtt_SetOption(handle, SOME_OPTION, SOME_VALUE);

    // assert
    ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_INVALID_ARG, result);

    mocks.AssertActualAndExpectedCalls();

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}


/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_021: [If any parameter is NULL then IoTHubTransportMqtt_SetOption shall return IOTHUB_CLIENT_INVALID_ARG.] */
TEST_FUNCTION(IoTHubTransportMqtt_Setoption_option_NULL_fail)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    auto handle = IoTHubTransportMqtt_Create(&config);
    mocks.ResetAllCalls();

    bool traceOn = true;

    // act
    auto result = IoTHubTransportMqtt_SetOption(handle, NULL, &traceOn);

    // assert
    ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_INVALID_ARG, result);

    mocks.AssertActualAndExpectedCalls();

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_021: [If any parameter is NULL then IoTHubTransportMqtt_SetOption shall return IOTHUB_CLIENT_INVALID_ARG.] */
TEST_FUNCTION(IoTHubTransportMqtt_Setoption_value_NULL_fail)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    auto handle = IoTHubTransportMqtt_Create(&config);
    mocks.ResetAllCalls();

    // act
    auto result = IoTHubTransportMqtt_SetOption(handle, OPTION_LOG_TRACE, NULL);

    // assert
    ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_INVALID_ARG, result);

    mocks.AssertActualAndExpectedCalls();

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_039: [If the option parameter is set to "x509certificate" then the value shall be a const char of the certificate to be used for x509.] */
TEST_FUNCTION(IoTHubTransportMqtt_Setoption_x509Certificate_no_509_fail)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    auto handle = IoTHubTransportMqtt_Create(&config);
    mocks.ResetAllCalls();

    // act
    auto result = IoTHubTransportMqtt_SetOption(handle, OPTION_X509_CERT, X509_CERT_CERTIFICATE);

    // assert
    ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_INVALID_ARG, result);

    mocks.AssertActualAndExpectedCalls();

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

TEST_FUNCTION(IoTHubTransportMqtt_Setoption_x509Private_key_no_509_fail)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config ={ 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    auto handle = IoTHubTransportMqtt_Create(&config);
    mocks.ResetAllCalls();

    // act
    auto result = IoTHubTransportMqtt_SetOption(handle, X509_PRIVATE_KEY_OPTION, X509_PRIVATE_KEY);

    // assert
    ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_INVALID_ARG, result);

    mocks.AssertActualAndExpectedCalls();

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

TEST_FUNCTION(IoTHubTransportMqtt_Setoption_x509Certificate_succeed)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfigWithKeyAndSasToken(&config, TEST_DEVICE_ID, NULL, NULL, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    auto handle = IoTHubTransportMqtt_Create(&config);
    mocks.ResetAllCalls();

    EXPECTED_CALL(mocks, STRING_c_str(NULL));
    EXPECTED_CALL(mocks, platform_get_default_tlsio());
    EXPECTED_CALL(mocks, xio_create(NULL, NULL));
    STRICT_EXPECTED_CALL(mocks, xio_setoption(NULL, OPTION_X509_CERT, X509_CERT_CERTIFICATE))
        .IgnoreArgument(1);

    // act
    auto result = IoTHubTransportMqtt_SetOption(handle, OPTION_X509_CERT, X509_CERT_CERTIFICATE);

    // assert
    ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK, result);

    mocks.AssertActualAndExpectedCalls();

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

TEST_FUNCTION(IoTHubTransportMqtt_Setoption_x509Private_key_succeed)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfigWithKeyAndSasToken(&config, TEST_DEVICE_ID, NULL, NULL, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    auto handle = IoTHubTransportMqtt_Create(&config);
    mocks.ResetAllCalls();

    EXPECTED_CALL(mocks, STRING_c_str(NULL));
    EXPECTED_CALL(mocks, platform_get_default_tlsio());
    EXPECTED_CALL(mocks, xio_create(NULL, NULL));
    STRICT_EXPECTED_CALL(mocks, xio_setoption(NULL, X509_PRIVATE_KEY_OPTION, X509_PRIVATE_KEY))
        .IgnoreArgument(1);

    // act
    auto result = IoTHubTransportMqtt_SetOption(handle, X509_PRIVATE_KEY_OPTION, X509_PRIVATE_KEY);

    // assert
    ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK, result);

    mocks.AssertActualAndExpectedCalls();

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_031: [If the option parameter is set to "logtrace" then the value shall be a bool_ptr and the value will determine if the mqtt client log is on or off.] */
TEST_FUNCTION(IoTHubTransportMqtt_Setoption_succeed)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    auto handle = IoTHubTransportMqtt_Create(&config);
    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, mqtt_client_set_trace(TEST_MQTT_CLIENT_HANDLE, IGNORED_PTR_ARG, IGNORED_PTR_ARG)).IgnoreArgument(2).IgnoreArgument(3);

    bool traceOn = true;

    // act
    auto result = IoTHubTransportMqtt_SetOption(handle, OPTION_LOG_TRACE, &traceOn);

    // assert
    ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK, result);

    mocks.AssertActualAndExpectedCalls();

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_036: [If the option parameter is set to "keepalive" then the value shall be a int_ptr and the value will determine the mqtt keepalive time that is set for pings.] */
TEST_FUNCTION(IoTHubTransportMqtt_Setoption_keepAlive_succeed)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    auto handle = IoTHubTransportMqtt_Create(&config);
    mocks.ResetAllCalls();

    int keepAlive = 10;

    // act
    auto result = IoTHubTransportMqtt_SetOption(handle, OPTION_KEEP_ALIVE, &keepAlive);

    // assert
    ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK, result);

    mocks.AssertActualAndExpectedCalls();

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_038: [If the client is connected when the keepalive is set then IoTHubTransportMqtt_SetOption shall disconnect and reconnect with the specified keepalive value.] */
TEST_FUNCTION(IoTHubTransportMqtt_Setoption_keepAlive_previous_connection_succeed)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    auto handle = IoTHubTransportMqtt_Create(&config);

    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    CONNECT_ACK connack = { true, CONNECTION_ACCEPTED };
    g_fnMqttOperationCallback(TEST_MQTT_CLIENT_HANDLE, MQTT_CLIENT_ON_CONNACK, &connack, g_callbackCtx);

    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, mqtt_client_disconnect(TEST_MQTT_CLIENT_HANDLE));
    STRICT_EXPECTED_CALL(mocks, xio_destroy(TEST_XIO_HANDLE));

    int keepAlive = 10;

    // act
    auto result = IoTHubTransportMqtt_SetOption(handle, OPTION_KEEP_ALIVE, &keepAlive);

    // assert
    ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK, result);

    mocks.AssertActualAndExpectedCalls();

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_037 : [If the option parameter is set to supplied int_ptr keepalive is the same value as the existing keepalive then IoTHubTransportMqtt_SetOption shall do nothing.] */
TEST_FUNCTION(IoTHubTransportMqtt_Setoption_keepAlive_same_value_succeed)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    auto handle = IoTHubTransportMqtt_Create(&config);

    int keepAlive = 30;
    auto result = IoTHubTransportMqtt_SetOption(handle, OPTION_KEEP_ALIVE, &keepAlive);

    CONNECT_ACK connack = { true, CONNECTION_ACCEPTED };
    g_fnMqttOperationCallback(TEST_MQTT_CLIENT_HANDLE, MQTT_CLIENT_ON_CONNACK, &connack, g_callbackCtx);

    mocks.ResetAllCalls();

    // act
    result = IoTHubTransportMqtt_SetOption(handle, OPTION_KEEP_ALIVE, &keepAlive);

    // assert
    ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK, result);

    mocks.AssertActualAndExpectedCalls();

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_026: [IoTHubTransportMqtt_DoWork shall do nothing if parameter handle and/or iotHubClientHandle is NULL.] */
TEST_FUNCTION(IoTHubTransportMqtt_DoWork_parameter_handle_NULL_fail)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;

    // act
    IoTHubTransportMqtt_DoWork(NULL, TEST_IOTHUB_CLIENT_LL_HANDLE);
    // assert

}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_026: [IoTHubTransportMqtt_DoWork shall do nothing if parameter handle and/or iotHubClientHandle is NULL.] */
TEST_FUNCTION(IoTHubTransportMqtt_DoWork_parameter_iothubClient_NULL_fail)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;

    // act
    IoTHubTransportMqtt_DoWork(TEST_TRANSPORT_HANDLE, NULL);
    // assert

}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_026: [IoTHubTransportMqtt_DoWork shall do nothing if parameter handle and/or iotHubClientHandle is NULL.] */
TEST_FUNCTION(IoTHubTransportMqtt_DoWork_all_parameters_NULL_fail)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;

    // act
    IoTHubTransportMqtt_DoWork(NULL, NULL);
    // assert

}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_027: [IoTHubTransportMqtt_DoWork shall inspect the �waitingToSend� DLIST passed in config structure.] */
TEST_FUNCTION(IoTHubTransportMqtt_DoWork_mqtt_client_connect_fail)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    auto handle = IoTHubTransportMqtt_Create(&config);
    mocks.ResetAllCalls();

    //STRICT_EXPECTED_CALL(mocks, mqtt_client_dowork(TEST_MQTT_CLIENT_HANDLE));
    STRICT_EXPECTED_CALL(mocks, get_time(IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, STRING_new());

#ifndef NO_LOGGING
    EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG)).ExpectedTimesExactly(5);
#else
    EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG)).ExpectedTimesExactly(4);
#endif

    STRICT_EXPECTED_CALL(mocks, platform_get_default_tlsio());

    EXPECTED_CALL(mocks, mqtt_client_connect(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG)).SetReturn(__LINE__);
    EXPECTED_CALL(mocks, SASToken_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_NUM_ARG));

    EXPECTED_CALL(mocks, xio_create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, tickcounter_get_current_ms(TEST_COUNTER_HANDLE, IGNORED_PTR_ARG)).IgnoreArgument(2);

    // act
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    //assert
    mocks.AssertActualAndExpectedCalls();

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_027: [IoTHubTransportMqtt_DoWork shall inspect the �waitingToSend� DLIST passed in config structure.] */
TEST_FUNCTION(IoTHubTransportMqtt_DoWork_no_messages_succeed)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    auto handle = IoTHubTransportMqtt_Create(&config);
    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, tickcounter_get_current_ms(TEST_COUNTER_HANDLE, IGNORED_PTR_ARG)).ExpectedAtLeastTimes(3).IgnoreArgument(2);

    STRICT_EXPECTED_CALL(mocks, mqtt_client_dowork(TEST_MQTT_CLIENT_HANDLE));
    SetupMocksForInitConnection(mocks);

    // act
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    //assert
    mocks.AssertActualAndExpectedCalls();

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

TEST_FUNCTION(IoTHubTransportMqtt_DoWork_SAS_token_from_user_succeed)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config ={ 0 };
    SetupIothubTransportConfigWithKeyAndSasToken(&config, TEST_DEVICE_ID, NULL, TEST_DEVICE_SAS, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    auto handle = IoTHubTransportMqtt_Create(&config);
    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, tickcounter_get_current_ms(TEST_COUNTER_HANDLE, IGNORED_PTR_ARG)).ExpectedAtLeastTimes(3).IgnoreArgument(2);

    STRICT_EXPECTED_CALL(mocks, mqtt_client_dowork(TEST_MQTT_CLIENT_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_new());
    EXPECTED_CALL(mocks, STRING_clone(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG)).ExpectedTimesExactly(4);
    STRICT_EXPECTED_CALL(mocks, platform_get_default_tlsio());
    EXPECTED_CALL(mocks, mqtt_client_connect(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));

    EXPECTED_CALL(mocks, xio_create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));

    // act
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    //assert
    mocks.AssertActualAndExpectedCalls();

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_041: [If both deviceKey and deviceSasToken fields are NULL then IoTHubTransportMqtt_Create shall assume a x509 authentication.] */
TEST_FUNCTION(IoTHubTransportMqtt_DoWork_x509_succeed)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config ={ 0 };
    SetupIothubTransportConfigWithKeyAndSasToken(&config, TEST_DEVICE_ID, NULL, NULL, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    auto handle = IoTHubTransportMqtt_Create(&config);
    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, tickcounter_get_current_ms(TEST_COUNTER_HANDLE, IGNORED_PTR_ARG)).ExpectedAtLeastTimes(3).IgnoreArgument(2);

    STRICT_EXPECTED_CALL(mocks, mqtt_client_dowork(TEST_MQTT_CLIENT_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_new());
    EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG)).ExpectedTimesExactly(3);
    STRICT_EXPECTED_CALL(mocks, platform_get_default_tlsio());
    EXPECTED_CALL(mocks, mqtt_client_connect(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));

    EXPECTED_CALL(mocks, xio_create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));

    // act
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    //assert
    mocks.AssertActualAndExpectedCalls();

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_027: [IoTHubTransportMqtt_DoWork shall inspect the �waitingToSend� DLIST passed in config structure.] */
TEST_FUNCTION(IoTHubTransportMqtt_DoWork_with_1_event_item_succeeds)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    QOS_VALUE QosValue[] = { DELIVER_AT_LEAST_ONCE };
    SUBSCRIBE_ACK suback;
    suback.packetId = 1234;
    suback.qosCount = 1;
    suback.qosReturn = QosValue;

    DList_InsertTailList(config.waitingToSend, &(message1.entry));
    auto handle = IoTHubTransportMqtt_Create(&config);
    g_fnMqttOperationCallback(TEST_MQTT_CLIENT_HANDLE, MQTT_CLIENT_ON_SUBSCRIBE_ACK, &suback, g_callbackCtx);
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);
    mocks.ResetAllCalls();

    EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG));
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetContentType(TEST_IOTHUB_MSG_BYTEARRAY));
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetByteArray(TEST_IOTHUB_MSG_BYTEARRAY, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(2)
        .IgnoreArgument(3);
    STRICT_EXPECTED_CALL(mocks, mqttmessage_create(IGNORED_NUM_ARG, IGNORED_PTR_ARG, DELIVER_AT_LEAST_ONCE, appMessage, appMsgSize))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, mqtt_client_publish(TEST_MQTT_CLIENT_HANDLE, IGNORED_PTR_ARG))
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, DList_InsertTailList(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, mqtt_client_dowork(TEST_MQTT_CLIENT_HANDLE));
    EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, DList_RemoveEntryList(IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, tickcounter_get_current_ms(TEST_COUNTER_HANDLE, IGNORED_PTR_ARG))
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, mqttmessage_destroy(TEST_MQTT_MESSAGE_HANDLE));
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Properties(TEST_IOTHUB_MSG_BYTEARRAY));
    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_MQTT_EVENT_TOPIC));
    EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, Map_GetInternals(TEST_MESSAGE_PROP_MAP, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, tickcounter_get_current_ms(TEST_COUNTER_HANDLE, IGNORED_PTR_ARG))
        .IgnoreArgument(2);

    // act
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    //assert
    mocks.AssertActualAndExpectedCalls();

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

TEST_FUNCTION(IoTHubTransportMqtt_DoWork_with_1_event_item_with_properties_succeeds)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    QOS_VALUE QosValue[] = { DELIVER_AT_LEAST_ONCE };
    SUBSCRIBE_ACK suback;
    suback.packetId = 1234;
    suback.qosCount = 1;
    suback.qosReturn = QosValue;

    g_nullMapVariable = false;

    const size_t propCount = 1;
    const char* TOPIC_PROPERTY_VALUE = "devices/thisIsDeviceID/messages/events/propKey1=propValue1";
    const char* PROPERTY_VALUE = "propKey1=propValue1";
    const char* keys[propCount] = { "propKey1" };
    const char* values[propCount] = { "propValue1" };

    const char* const** ppKeys = (const char* const**)&keys;
    const char* const** ppValues = (const char* const**)&values;

    DList_InsertTailList(config.waitingToSend, &(message1.entry));
    auto handle = IoTHubTransportMqtt_Create(&config);
    g_fnMqttOperationCallback(TEST_MQTT_CLIENT_HANDLE, MQTT_CLIENT_ON_SUBSCRIBE_ACK, &suback, g_callbackCtx);
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);
    mocks.ResetAllCalls();

    EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
        .ExpectedTimesExactly(2);
    EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetContentType(TEST_IOTHUB_MSG_BYTEARRAY));
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetByteArray(TEST_IOTHUB_MSG_BYTEARRAY, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(2)
        .IgnoreArgument(3);
    STRICT_EXPECTED_CALL(mocks, mqttmessage_create(IGNORED_NUM_ARG, TOPIC_PROPERTY_VALUE, DELIVER_AT_LEAST_ONCE, appMessage, appMsgSize))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, mqtt_client_publish(TEST_MQTT_CLIENT_HANDLE, IGNORED_PTR_ARG))
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, DList_InsertTailList(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, mqtt_client_dowork(TEST_MQTT_CLIENT_HANDLE));
    EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, DList_RemoveEntryList(IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, tickcounter_get_current_ms(TEST_COUNTER_HANDLE, IGNORED_PTR_ARG))
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, mqttmessage_destroy(TEST_MQTT_MESSAGE_HANDLE));
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Properties(TEST_IOTHUB_MSG_BYTEARRAY));
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, PROPERTY_VALUE))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_MQTT_EVENT_TOPIC));
    STRICT_EXPECTED_CALL(mocks, Map_GetInternals(TEST_MESSAGE_PROP_MAP, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(2, &ppKeys, sizeof(ppKeys) )
        .CopyOutArgumentBuffer(3, &ppValues, sizeof(ppValues) )
        .CopyOutArgumentBuffer(4, &propCount, sizeof(propCount) );
    EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, tickcounter_get_current_ms(TEST_COUNTER_HANDLE, IGNORED_PTR_ARG))
        .IgnoreArgument(2);

    // act
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    //assert
    mocks.AssertActualAndExpectedCalls();

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

TEST_FUNCTION(IoTHubTransportMqtt_DoWork_with_1_event_item_with_2_properties_succeeds)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    QOS_VALUE QosValue[] = { DELIVER_AT_LEAST_ONCE };
    SUBSCRIBE_ACK suback;
    suback.packetId = 1234;
    suback.qosCount = 1;
    suback.qosReturn = QosValue;

    g_nullMapVariable = false;

    const size_t propCount = 2;

    const char* TOPIC_PROPERTY_VALUE = "devices/thisIsDeviceID/messages/events/propKey1=propValue1&propKey2=propValue2";

    const char* keys[propCount] = { "propKey1", "propKey2" };
    const char* values[propCount] = { "propValue1", "propValue2" };
    const char* const** ppKeys = (const char* const **)&keys;
    const char* const** ppValues = (const char* const **)&values;

    DList_InsertTailList(config.waitingToSend, &(message1.entry));
    auto handle = IoTHubTransportMqtt_Create(&config);
    g_fnMqttOperationCallback(TEST_MQTT_CLIENT_HANDLE, MQTT_CLIENT_ON_SUBSCRIBE_ACK, &suback, g_callbackCtx);
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);
    mocks.ResetAllCalls();

    EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
        .ExpectedTimesExactly(3);
    EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
        .ExpectedTimesExactly(2);
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetContentType(TEST_IOTHUB_MSG_BYTEARRAY));
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetByteArray(TEST_IOTHUB_MSG_BYTEARRAY, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(2)
        .IgnoreArgument(3);
    STRICT_EXPECTED_CALL(mocks, mqttmessage_create(IGNORED_NUM_ARG, TOPIC_PROPERTY_VALUE, DELIVER_AT_LEAST_ONCE, appMessage, appMsgSize))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, mqtt_client_publish(TEST_MQTT_CLIENT_HANDLE, IGNORED_PTR_ARG))
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, DList_InsertTailList(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, mqtt_client_dowork(TEST_MQTT_CLIENT_HANDLE));
    EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, DList_RemoveEntryList(IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, tickcounter_get_current_ms(TEST_COUNTER_HANDLE, IGNORED_PTR_ARG))
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, mqttmessage_destroy(TEST_MQTT_MESSAGE_HANDLE));
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Properties(TEST_IOTHUB_MSG_BYTEARRAY)).SetReturn(TEST_MESSAGE_PROP_MAP);
    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_MQTT_EVENT_TOPIC));
    STRICT_EXPECTED_CALL(mocks, Map_GetInternals(TEST_MESSAGE_PROP_MAP, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(2, &ppKeys, sizeof(ppKeys))
        .CopyOutArgumentBuffer(3, &ppValues, sizeof(ppValues))
        .CopyOutArgumentBuffer(4, &propCount, sizeof(propCount));
    EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, IGNORED_PTR_ARG)).ExpectedTimesExactly(2);
    STRICT_EXPECTED_CALL(mocks, tickcounter_get_current_ms(TEST_COUNTER_HANDLE, IGNORED_PTR_ARG))
        .IgnoreArgument(2);

    // act
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    //assert
    mocks.AssertActualAndExpectedCalls();

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

TEST_FUNCTION(IoTHubTransportMqtt_DoWork_with_1_event_item_with_properties_STRING_concat_fail)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    QOS_VALUE QosValue[] = { DELIVER_AT_LEAST_ONCE };
    SUBSCRIBE_ACK suback;
    suback.packetId = 1234;
    suback.qosCount = 1;
    suback.qosReturn = QosValue;

    g_nullMapVariable = false;

    const size_t propCount = 1;
    const char* PROPERTY_VALUE = "propKey1=propValue1";
    const char* keys[propCount] = { "propKey1" };
    const char* values[propCount] = { "propValue1" };

    const char* const** ppKeys = (const char* const**)&keys;
    const char* const** ppValues = (const char* const**)&values;

    DList_InsertTailList(config.waitingToSend, &(message1.entry));
    auto handle = IoTHubTransportMqtt_Create(&config);
    g_fnMqttOperationCallback(TEST_MQTT_CLIENT_HANDLE, MQTT_CLIENT_ON_SUBSCRIBE_ACK, &suback, g_callbackCtx);
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);
    mocks.ResetAllCalls();

    EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)).ExpectedTimesExactly(2);
    EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)).ExpectedTimesExactly(2);
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetContentType(TEST_IOTHUB_MSG_BYTEARRAY));
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetByteArray(TEST_IOTHUB_MSG_BYTEARRAY, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(2)
        .IgnoreArgument(3);
    STRICT_EXPECTED_CALL(mocks, mqtt_client_dowork(TEST_MQTT_CLIENT_HANDLE));
    EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, DList_RemoveEntryList(IGNORED_PTR_ARG));

    EXPECTED_CALL(mocks, DList_InitializeListHead(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, DList_InsertTailList(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, IoTHubClient_LL_SendComplete(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IOTHUB_CLIENT_CONFIRMATION_OK));

    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Properties(TEST_IOTHUB_MSG_BYTEARRAY));
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, PROPERTY_VALUE))
        .IgnoreArgument(1)
        .SetReturn(__LINE__);
    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_MQTT_EVENT_TOPIC));
    STRICT_EXPECTED_CALL(mocks, Map_GetInternals(TEST_MESSAGE_PROP_MAP, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(2, &ppKeys, sizeof(ppKeys))
        .CopyOutArgumentBuffer(3, &ppValues, sizeof(ppValues))
        .CopyOutArgumentBuffer(4, &propCount, sizeof(propCount));
    EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, tickcounter_get_current_ms(TEST_COUNTER_HANDLE, IGNORED_PTR_ARG))
        .IgnoreArgument(2);

    // act
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    //assert
    mocks.AssertActualAndExpectedCalls();

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

TEST_FUNCTION(IoTHubTransportMqtt_DoWork_no_subscribe_succeeds)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    CONNECT_ACK connack;
    connack.isSessionPresent = false;
    connack.returnCode = CONNECTION_ACCEPTED;


    //DList_InsertTailList(config.waitingToSend, &(message1.entry));
    auto handle = IoTHubTransportMqtt_Create(&config);

    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);
    g_fnMqttOperationCallback(TEST_MQTT_CLIENT_HANDLE, MQTT_CLIENT_ON_CONNACK, &connack, g_callbackCtx);
    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, mqtt_client_dowork(TEST_MQTT_CLIENT_HANDLE));
    STRICT_EXPECTED_CALL(mocks, tickcounter_get_current_ms(TEST_COUNTER_HANDLE, IGNORED_PTR_ARG))
        .IgnoreArgument(2);

    // act
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    //assert
    mocks.AssertActualAndExpectedCalls();

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_027: [IoTHubTransportMqtt_DoWork shall inspect the �waitingToSend� DLIST passed in config structure.] */
/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_029: [IoTHubTransportMqtt_DoWork shall create a MQTT_MESSAGE_HANDLE and pass this to a call to mqtt_client_publish.] */
/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_030: [IoTHubTransportMqtt_DoWork shall call mqtt_client_dowork everytime it is called if it is connected.] */
TEST_FUNCTION(IoTHubTransportMqtt_DoWork_with_1_event_item_STRING_type_succeeds)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    QOS_VALUE QosValue[] = { DELIVER_AT_LEAST_ONCE };
    SUBSCRIBE_ACK suback;
    suback.packetId = 1234;
    suback.qosCount = 1;
    suback.qosReturn = QosValue;

    DList_InsertTailList(config.waitingToSend, &(message2.entry));
    auto handle = IoTHubTransportMqtt_Create(&config);
    g_fnMqttOperationCallback(TEST_MQTT_CLIENT_HANDLE, MQTT_CLIENT_ON_SUBSCRIBE_ACK, &suback, g_callbackCtx);
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);
    mocks.ResetAllCalls();

    EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG));
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetContentType(TEST_IOTHUB_MSG_STRING));
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetString(TEST_IOTHUB_MSG_STRING));
    STRICT_EXPECTED_CALL(mocks, mqttmessage_create(IGNORED_NUM_ARG, IGNORED_PTR_ARG, DELIVER_AT_LEAST_ONCE, (const uint8_t*)appMessageString, strlen(appMessageString) ))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, mqtt_client_publish(TEST_MQTT_CLIENT_HANDLE, IGNORED_PTR_ARG))
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, DList_InsertTailList(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, mqtt_client_dowork(TEST_MQTT_CLIENT_HANDLE));
    EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, DList_RemoveEntryList(IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, tickcounter_get_current_ms(TEST_COUNTER_HANDLE, IGNORED_PTR_ARG))
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, mqttmessage_destroy(TEST_MQTT_MESSAGE_HANDLE));
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Properties(TEST_IOTHUB_MSG_STRING));
    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_MQTT_EVENT_TOPIC));
    EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, Map_GetInternals(TEST_MESSAGE_PROP_MAP, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, tickcounter_get_current_ms(TEST_COUNTER_HANDLE, IGNORED_PTR_ARG))
        .IgnoreArgument(2);

    // act
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    //assert
    mocks.AssertActualAndExpectedCalls();

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

/* Test_SRS_IOTHUB_MQTT_TRANSPORT_07_033: [IoTHubTransportMqtt_DoWork shall iterate through the Waiting Acknowledge messages looking for any message that has been waiting longer than 2 min.]*/
TEST_FUNCTION(IoTHubTransportMqtt_DoWork_no_resend_message_succeeds)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    QOS_VALUE QosValue[] = { DELIVER_AT_LEAST_ONCE };
    SUBSCRIBE_ACK suback;
    suback.packetId = 1234;
    suback.qosCount = 1;
    suback.qosReturn = QosValue;

    DList_InsertTailList(config.waitingToSend, &(message2.entry));
    auto handle = IoTHubTransportMqtt_Create(&config);
    g_fnMqttOperationCallback(TEST_MQTT_CLIENT_HANDLE, MQTT_CLIENT_ON_SUBSCRIBE_ACK, &suback, g_callbackCtx);
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);
    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, mqtt_client_dowork(TEST_MQTT_CLIENT_HANDLE));
    STRICT_EXPECTED_CALL(mocks, tickcounter_get_current_ms(TEST_COUNTER_HANDLE, IGNORED_PTR_ARG))
        .IgnoreArgument(2)
        .ExpectedAtLeastTimes(2);

    // act
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    //assert
    mocks.AssertActualAndExpectedCalls();

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

/* Test_SRS_IOTHUB_MQTT_TRANSPORT_07_033: [IoTHubTransportMqtt_DoWork shall iterate through the Waiting Acknowledge messages looking for any message that has been waiting longer than 2 min.]*/
TEST_FUNCTION(IoTHubTransportMqtt_DoWork_resend_message_succeeds)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    QOS_VALUE QosValue[] = { DELIVER_AT_LEAST_ONCE };
    SUBSCRIBE_ACK suback;
    suback.packetId = 1234;
    suback.qosCount = 1;
    suback.qosReturn = QosValue;

    DList_InsertTailList(config.waitingToSend, &(message2.entry));
    auto handle = IoTHubTransportMqtt_Create(&config);
    g_fnMqttOperationCallback(TEST_MQTT_CLIENT_HANDLE, MQTT_CLIENT_ON_SUBSCRIBE_ACK, &suback, g_callbackCtx);
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);
    mocks.ResetAllCalls();

    g_current_ms = 5*60*1000;

    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetContentType(TEST_IOTHUB_MSG_STRING));
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetString(TEST_IOTHUB_MSG_STRING));
    STRICT_EXPECTED_CALL(mocks, mqttmessage_create(IGNORED_NUM_ARG, IGNORED_PTR_ARG, DELIVER_AT_LEAST_ONCE, (const uint8_t*)appMessageString, strlen(appMessageString)))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, mqtt_client_publish(TEST_MQTT_CLIENT_HANDLE, IGNORED_PTR_ARG))
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, mqtt_client_dowork(TEST_MQTT_CLIENT_HANDLE));
    EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, tickcounter_get_current_ms(TEST_COUNTER_HANDLE, IGNORED_PTR_ARG))
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, tickcounter_get_current_ms(TEST_COUNTER_HANDLE, IGNORED_PTR_ARG))
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, mqttmessage_destroy(TEST_MQTT_MESSAGE_HANDLE));
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Properties(TEST_IOTHUB_MSG_STRING));
    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_MQTT_EVENT_TOPIC));
    EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, Map_GetInternals(TEST_MESSAGE_PROP_MAP, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, tickcounter_get_current_ms(TEST_COUNTER_HANDLE, IGNORED_PTR_ARG))
        .IgnoreArgument(2);

    // act
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    //assert
    mocks.AssertActualAndExpectedCalls();

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_034: [If IoTHubTransportMqtt_DoWork has resent the message two times then it shall fail the message] */
TEST_FUNCTION(IoTHubTransportMqtt_DoWork_resend_max_recount_reached_message_succeeds)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    QOS_VALUE QosValue[] = { DELIVER_AT_LEAST_ONCE };
    SUBSCRIBE_ACK suback;
    suback.packetId = 1234;
    suback.qosCount = 1;
    suback.qosReturn = QosValue;

    DList_InsertTailList(config.waitingToSend, &(message2.entry));
    auto handle = IoTHubTransportMqtt_Create(&config);
    g_fnMqttOperationCallback(TEST_MQTT_CLIENT_HANDLE, MQTT_CLIENT_ON_SUBSCRIBE_ACK, &suback, g_callbackCtx);
    for (size_t index = 0; index < 3; index++)
    {
        IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);
        g_current_ms = 3 * index * 60 * 1000;
    }
    mocks.ResetAllCalls();

    EXPECTED_CALL(mocks, DList_InitializeListHead(IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, DList_InsertTailList(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_SendComplete(TEST_IOTHUB_CLIENT_LL_HANDLE, IGNORED_PTR_ARG, IOTHUB_CLIENT_CONFIRMATION_MESSAGE_TIMEOUT))
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, mqtt_client_dowork(TEST_MQTT_CLIENT_HANDLE));
    EXPECTED_CALL(mocks, DList_RemoveEntryList(IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, tickcounter_get_current_ms(TEST_COUNTER_HANDLE, IGNORED_PTR_ARG))
        .IgnoreArgument(2)
        .ExpectedAtLeastTimes(2);
    EXPECTED_CALL(mocks, gballoc_free(NULL));

    // act
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    //assert
    mocks.AssertActualAndExpectedCalls();

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_030: [IoTHubTransportMqtt_DoWork shall call mqtt_client_dowork everytime it is called if it is connected.] */
TEST_FUNCTION(IoTHubTransportMqtt_DoWork_GetString_fails)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    QOS_VALUE QosValue[] = { DELIVER_AT_LEAST_ONCE };
    SUBSCRIBE_ACK suback;
    suback.packetId = 1234;
    suback.qosCount = 1;
    suback.qosReturn = QosValue;

    DList_InsertTailList(config.waitingToSend, &(message2.entry));
    auto handle = IoTHubTransportMqtt_Create(&config);
    g_fnMqttOperationCallback(TEST_MQTT_CLIENT_HANDLE, MQTT_CLIENT_ON_SUBSCRIBE_ACK, &suback, g_callbackCtx);
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);
    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetContentType(TEST_IOTHUB_MSG_STRING));
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetString(TEST_IOTHUB_MSG_STRING)).SetReturn((const char*)NULL);
    STRICT_EXPECTED_CALL(mocks, mqtt_client_dowork(TEST_MQTT_CLIENT_HANDLE));
    STRICT_EXPECTED_CALL(mocks, tickcounter_get_current_ms(TEST_COUNTER_HANDLE, IGNORED_PTR_ARG)).IgnoreArgument(2);

    // act
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    //assert
    mocks.AssertActualAndExpectedCalls();

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_030: [IoTHubTransportMqtt_DoWork shall call mqtt_client_dowork everytime it is called if it is connected.] */
TEST_FUNCTION(IoTHubTransportMqtt_DoWork_GetByteArray_Error_fail)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    QOS_VALUE QosValue[] = { DELIVER_AT_LEAST_ONCE };
    SUBSCRIBE_ACK suback;
    suback.packetId = 1234;
    suback.qosCount = 1;
    suback.qosReturn = QosValue;

    DList_InsertTailList(config.waitingToSend, &(message1.entry));
    auto handle = IoTHubTransportMqtt_Create(&config);
    g_fnMqttOperationCallback(TEST_MQTT_CLIENT_HANDLE, MQTT_CLIENT_ON_SUBSCRIBE_ACK, &suback, g_callbackCtx);
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);
    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetByteArray(TEST_IOTHUB_MSG_BYTEARRAY, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(2)
        .IgnoreArgument(3)
        .SetReturn(IOTHUB_MESSAGE_ERROR);
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetContentType(TEST_IOTHUB_MSG_BYTEARRAY));
    STRICT_EXPECTED_CALL(mocks, mqtt_client_dowork(TEST_MQTT_CLIENT_HANDLE));
    STRICT_EXPECTED_CALL(mocks, tickcounter_get_current_ms(TEST_COUNTER_HANDLE, IGNORED_PTR_ARG)).IgnoreArgument(2);

    // act
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    //assert
    mocks.AssertActualAndExpectedCalls();

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_030: [IoTHubTransportMqtt_DoWork shall call mqtt_client_dowork everytime it is called if it is connected.] */
TEST_FUNCTION(IoTHubTransportMqtt_DoWork_mqtt_client_publish_fails)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    QOS_VALUE QosValue[] = { DELIVER_AT_LEAST_ONCE };
    SUBSCRIBE_ACK suback;
    suback.packetId = 1234;
    suback.qosCount = 1;
    suback.qosReturn = QosValue;

    DList_InsertTailList(config.waitingToSend, &(message1.entry));
    auto handle = IoTHubTransportMqtt_Create(&config);
    g_fnMqttOperationCallback(TEST_MQTT_CLIENT_HANDLE, MQTT_CLIENT_ON_SUBSCRIBE_ACK, &suback, g_callbackCtx);
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);
    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetContentType(TEST_IOTHUB_MSG_BYTEARRAY));
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetByteArray(TEST_IOTHUB_MSG_BYTEARRAY, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(2)
        .IgnoreArgument(3);
    STRICT_EXPECTED_CALL(mocks, mqttmessage_create(IGNORED_NUM_ARG, IGNORED_PTR_ARG, DELIVER_AT_LEAST_ONCE, appMessage, appMsgSize))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, mqtt_client_publish(TEST_MQTT_CLIENT_HANDLE, IGNORED_PTR_ARG))
        .IgnoreArgument(2)
        .SetReturn(__LINE__);
    EXPECTED_CALL(mocks, DList_InitializeListHead(IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, DList_InsertTailList(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, mqtt_client_dowork(TEST_MQTT_CLIENT_HANDLE));
    EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, DList_RemoveEntryList(IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_SendComplete(TEST_IOTHUB_CLIENT_LL_HANDLE, IGNORED_PTR_ARG, IOTHUB_CLIENT_CONFIRMATION_ERROR))
        .IgnoreArgument(2);
    EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(mocks, gballoc_free(NULL));
    STRICT_EXPECTED_CALL(mocks, mqttmessage_destroy(TEST_MQTT_MESSAGE_HANDLE));
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Properties(TEST_IOTHUB_MSG_BYTEARRAY));
    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_MQTT_EVENT_TOPIC));
    EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, Map_GetInternals(TEST_MESSAGE_PROP_MAP, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, tickcounter_get_current_ms(TEST_COUNTER_HANDLE, IGNORED_PTR_ARG)).IgnoreArgument(2);

    // act
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    //assert
    mocks.AssertActualAndExpectedCalls();

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_030: [IoTHubTransportMqtt_DoWork shall call mqtt_client_dowork everytime it is called if it is connected.] */
TEST_FUNCTION(IoTHubTransportMqtt_DoWork_mqttmessage_create_fails)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    QOS_VALUE QosValue[] = { DELIVER_AT_LEAST_ONCE };
    SUBSCRIBE_ACK suback;
    suback.packetId = 1234;
    suback.qosCount = 1;
    suback.qosReturn = QosValue;

    DList_InsertTailList(config.waitingToSend, &(message1.entry));
    auto handle = IoTHubTransportMqtt_Create(&config);
    g_fnMqttOperationCallback(TEST_MQTT_CLIENT_HANDLE, MQTT_CLIENT_ON_SUBSCRIBE_ACK, &suback, g_callbackCtx);
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);
    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetContentType(TEST_IOTHUB_MSG_BYTEARRAY));
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetByteArray(TEST_IOTHUB_MSG_BYTEARRAY, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(2)
        .IgnoreArgument(3);
    STRICT_EXPECTED_CALL(mocks, mqttmessage_create(IGNORED_NUM_ARG, IGNORED_PTR_ARG, DELIVER_AT_LEAST_ONCE, appMessage, appMsgSize))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .SetReturn( (MQTT_MESSAGE_HANDLE)NULL);
    EXPECTED_CALL(mocks, DList_InitializeListHead(IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, DList_InsertTailList(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, mqtt_client_dowork(TEST_MQTT_CLIENT_HANDLE));
    EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_SendComplete(TEST_IOTHUB_CLIENT_LL_HANDLE, IGNORED_PTR_ARG, IOTHUB_CLIENT_CONFIRMATION_ERROR))
        .IgnoreArgument(2);
    EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(mocks, gballoc_free(NULL));
    EXPECTED_CALL(mocks, DList_RemoveEntryList(IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Properties(TEST_IOTHUB_MSG_BYTEARRAY));
    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_MQTT_EVENT_TOPIC));
    EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, Map_GetInternals(TEST_MESSAGE_PROP_MAP, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, tickcounter_get_current_ms(TEST_COUNTER_HANDLE, IGNORED_PTR_ARG)).IgnoreArgument(2);

    // act
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    //assert
    mocks.AssertActualAndExpectedCalls();

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

TEST_FUNCTION(IoTHubTransportMqtt_DoWork_connectFailCount_exceed_succeed)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config ={ 0 };
    const size_t iterationCount = 7;
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    auto handle = IoTHubTransportMqtt_Create(&config);
    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, platform_get_default_tlsio());
    EXPECTED_CALL(mocks, xio_create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));

    //STRICT_EXPECTED_CALL(mocks, mqtt_client_dowork(TEST_MQTT_CLIENT_HANDLE)).ExpectedAtLeastTimes(iterationCount-1);
    //EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG)).ExpectedTimesExactly(9);
    for (size_t index = 0; index < iterationCount-1; index++)
    {
        STRICT_EXPECTED_CALL(mocks, tickcounter_get_current_ms(TEST_COUNTER_HANDLE, IGNORED_PTR_ARG)).IgnoreArgument(2);
        STRICT_EXPECTED_CALL(mocks, get_time(IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(mocks, STRING_new());
        EXPECTED_CALL(mocks, mqtt_client_connect(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG)).SetReturn(__LINE__);
        EXPECTED_CALL(mocks, SASToken_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_NUM_ARG));
        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
    }
    STRICT_EXPECTED_CALL(mocks, tickcounter_get_current_ms(TEST_COUNTER_HANDLE, IGNORED_PTR_ARG)).IgnoreArgument(2);

#ifndef NO_LOGGING
    EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG)).ExpectedTimesExactly(25);
#else
    EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG)).ExpectedTimesExactly(19);
#endif

    // act
    for (size_t index = 0; index < iterationCount; index++)
    {
        IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);
    }

    //assert
    mocks.AssertActualAndExpectedCalls();

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

/* Test_SRS_IOTHUB_MQTT_TRANSPORT_07_023: [IoTHubTransportMqtt_GetSendStatus shall return IOTHUB_CLIENT_INVALID_ARG if called with NULL parameter.] */
TEST_FUNCTION(IoTHubTransportMqtt_GetSendStatus_InvalidHandleArgument_fail)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    auto handle = IoTHubTransportMqtt_Create(&config);
    mocks.ResetAllCalls();

    IOTHUB_CLIENT_STATUS status;

    // act
    IOTHUB_CLIENT_RESULT result = IoTHubTransportMqtt_GetSendStatus(NULL, &status);

    // assert
    ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, result, IOTHUB_CLIENT_INVALID_ARG);

    mocks.AssertActualAndExpectedCalls();

    // cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

/* Test_SRS_IOTHUB_MQTT_TRANSPORT_07_023: [IoTHubTransportMqtt_GetSendStatus shall return IOTHUB_CLIENT_INVALID_ARG if called with NULL parameter.] */
TEST_FUNCTION(IoTHubTransportMqtt_GetSendStatus_InvalidStatusArgument_fail)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    auto handle = IoTHubTransportMqtt_Create(&config);
    mocks.ResetAllCalls();

    // act
    IOTHUB_CLIENT_RESULT result = IoTHubTransportMqtt_GetSendStatus(handle, NULL);

    // assert
    ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, result, IOTHUB_CLIENT_INVALID_ARG);

    mocks.AssertActualAndExpectedCalls();

    // cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_024: [IoTHubTransportMqtt_GetSendStatus shall return IOTHUB_CLIENT_OK and status IOTHUB_CLIENT_SEND_STATUS_IDLE if there are currently no event items to be sent or being sent.] */
TEST_FUNCTION(IoTHubTransportMqtt_GetSendStatus_empty_waitingToSend_and_empty_waitingforAck_success)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    auto handle = IoTHubTransportMqtt_Create(&config);
    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(config.waitingToSend) );
    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG) ).IgnoreArgument(1);

    IOTHUB_CLIENT_STATUS status;

    // act
    IOTHUB_CLIENT_RESULT result = IoTHubTransportMqtt_GetSendStatus(handle, &status);

    // assert
    ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, result, IOTHUB_CLIENT_OK);
    ASSERT_ARE_EQUAL(IOTHUB_CLIENT_STATUS, status, IOTHUB_CLIENT_SEND_STATUS_IDLE);

    mocks.AssertActualAndExpectedCalls();

    // cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_025: [IoTHubTransportMqtt_GetSendStatus shall return IOTHUB_CLIENT_OK and status IOTHUB_CLIENT_SEND_STATUS_BUSY if there are currently event items to be sent or being sent.] */
TEST_FUNCTION(IoTHubTransportMqtt_GetSendStatus_waitingToSend_not_empty_success)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    auto handle = IoTHubTransportMqtt_Create(&config);

    IOTHUB_MESSAGE_HANDLE eventMessageHandle = IoTHubMessage_CreateFromByteArray(IGNORED_PTR_ARG, IGNORED_NUM_ARG);
    IOTHUB_MESSAGE_LIST newEntry;
    newEntry.messageHandle = (IOTHUB_MESSAGE_HANDLE) &newEntry;
    DList_InsertTailList(config.waitingToSend, &(newEntry.entry));

    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(config.waitingToSend) );

    IOTHUB_CLIENT_STATUS status;

    // act
    IOTHUB_CLIENT_RESULT result = IoTHubTransportMqtt_GetSendStatus(handle, &status);

    // assert
    ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, result, IOTHUB_CLIENT_OK);
    ASSERT_ARE_EQUAL(IOTHUB_CLIENT_STATUS, status, IOTHUB_CLIENT_SEND_STATUS_BUSY);

    mocks.AssertActualAndExpectedCalls();

    // cleanup
    IoTHubTransportMqtt_Destroy(handle);
    IoTHubMessage_Destroy(eventMessageHandle);
}

/* Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_022: [This function shall return a pointer to a structure of type TRANSPORT_PROVIDER having the following values for it�s fields: IoTHubTransport_Create = IoTHubTransportMqtt_Create
IoTHubTransport_Destroy = IoTHubTransportMqtt_Destroy
IoTHubTransport_Subscribe = IoTHubTransportMqtt_Subscribe
IoTHubTransport_Unsubscribe = IoTHubTransportMqtt_Unsubscribe
IoTHubTransport_DoWork = IoTHubTransportMqtt_DoWork
IoTHubTransport_SetOption = IoTHubTransportMqtt_SetOption] */
TEST_FUNCTION(IoTHubTransportMqtt_Protocol_succeeds)
{
    ///arrange

    ///act
    auto result = MQTT_Protocol();

    ///assert
    ASSERT_IS_NOT_NULL(result);
    ASSERT_ARE_EQUAL(void_ptr, (void*)((TRANSPORT_PROVIDER*)result)->IoTHubTransport_SetOption, (void*)IoTHubTransportMqtt_SetOption);
    ASSERT_ARE_EQUAL(void_ptr, (void*)((TRANSPORT_PROVIDER*)result)->IoTHubTransport_Create, (void*)IoTHubTransportMqtt_Create);
    ASSERT_ARE_EQUAL(void_ptr, (void*)((TRANSPORT_PROVIDER*)result)->IoTHubTransport_Destroy, (void*)IoTHubTransportMqtt_Destroy);
    ASSERT_ARE_EQUAL(void_ptr, (void*)((TRANSPORT_PROVIDER*)result)->IoTHubTransport_Subscribe, (void*)IoTHubTransportMqtt_Subscribe);
    ASSERT_ARE_EQUAL(void_ptr, (void*)((TRANSPORT_PROVIDER*)result)->IoTHubTransport_Unsubscribe, (void*)IoTHubTransportMqtt_Unsubscribe);
    ASSERT_ARE_EQUAL(void_ptr, (void*)((TRANSPORT_PROVIDER*)result)->IoTHubTransport_DoWork, (void*)IoTHubTransportMqtt_DoWork);
    ASSERT_ARE_EQUAL(void_ptr, (void*)((TRANSPORT_PROVIDER*)result)->IoTHubTransport_GetSendStatus, (void*)IoTHubTransportMqtt_GetSendStatus);

    ///cleanup
}

TEST_FUNCTION(IoTHubTransportMqtt_delivered_NULL_context_do_Nothing)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    auto handle = IoTHubTransportMqtt_Create(&config);

    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, xio_close(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(2)
        .IgnoreArgument(3);

    // act
    g_fnMqttOperationCallback(TEST_MQTT_CLIENT_HANDLE, MQTT_CLIENT_ON_ERROR, NULL, g_callbackCtx);

    // assert
    mocks.AssertActualAndExpectedCalls();

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

TEST_FUNCTION(IoTHubTransportMqtt_delivered_MQTT_CLIENT_NO_PING_RESPONSE_success)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config ={ 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    auto handle = IoTHubTransportMqtt_Create(&config);

    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, xio_close(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(2)
        .IgnoreArgument(3);

    // act
    g_fnMqttOperationCallback(TEST_MQTT_CLIENT_HANDLE, MQTT_CLIENT_NO_PING_RESPONSE, NULL, g_callbackCtx);

    // assert
    mocks.AssertActualAndExpectedCalls();

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

TEST_FUNCTION(IoTHubTransportMqtt_MqttOpCompleteCallback_DISCONNECT_succeed)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    auto handle = IoTHubTransportMqtt_Create(&config);
    mocks.ResetAllCalls();

    // act
    g_fnMqttOperationCallback(TEST_MQTT_CLIENT_HANDLE, MQTT_CLIENT_ON_DISCONNECT, NULL, g_callbackCtx);

    //assert
    mocks.AssertActualAndExpectedCalls();

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

TEST_FUNCTION(IoTHubTransportMqtt_MqttOpCompleteCallback_CONN_ACK_NOT_CONNECTED_succeed)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    CONNECT_ACK connack;
    connack.isSessionPresent = false;
    connack.returnCode = CONN_REFUSED_BAD_USERNAME_PASSWORD;

    auto handle = IoTHubTransportMqtt_Create(&config);
    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, mqtt_client_disconnect(TEST_MQTT_CLIENT_HANDLE))
        .IgnoreArgument(1);

    // act
    g_fnMqttOperationCallback(TEST_MQTT_CLIENT_HANDLE, MQTT_CLIENT_ON_CONNACK, &connack, g_callbackCtx);

    //assert
    mocks.AssertActualAndExpectedCalls();

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

TEST_FUNCTION(IoTHubTransportMqtt_MqttOpCompleteCallback_CONNACK_NULL_msgInfo_fail)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    auto handle = IoTHubTransportMqtt_Create(&config);
    mocks.ResetAllCalls();

    // act
    g_fnMqttOperationCallback(TEST_MQTT_CLIENT_HANDLE, MQTT_CLIENT_ON_CONNACK, NULL, g_callbackCtx);

    //assert
    mocks.AssertActualAndExpectedCalls();

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

TEST_FUNCTION(IoTHubTransportMqtt_MqttOpCompleteCallback_PUBLISH_ACK_succeed)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    PUBLISH_ACK puback;
    puback.packetId = 2;

    QOS_VALUE QosValue[] = { DELIVER_AT_LEAST_ONCE };
    SUBSCRIBE_ACK suback;
    suback.packetId = 1234;
    suback.qosCount = 1;
    suback.qosReturn = QosValue;

    DList_InsertTailList(config.waitingToSend, &(message1.entry));
    auto handle = IoTHubTransportMqtt_Create(&config);
    g_fnMqttOperationCallback(TEST_MQTT_CLIENT_HANDLE, MQTT_CLIENT_ON_SUBSCRIBE_ACK, &suback, g_callbackCtx);
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);
    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, DList_RemoveEntryList(IGNORED_PTR_ARG))
        .IgnoreAllArguments();
    STRICT_EXPECTED_CALL(mocks, DList_InitializeListHead(IGNORED_PTR_ARG))
        .IgnoreAllArguments();
    STRICT_EXPECTED_CALL(mocks, DList_InsertTailList(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreAllArguments();
    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_SendComplete(TEST_IOTHUB_CLIENT_LL_HANDLE, IGNORED_PTR_ARG, IOTHUB_CLIENT_CONFIRMATION_OK))
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, gballoc_free(NULL))
        .IgnoreArgument(1);

    // act
    g_fnMqttOperationCallback(TEST_MQTT_CLIENT_HANDLE, MQTT_CLIENT_ON_PUBLISH_ACK, &puback, g_callbackCtx);

    //assert
    mocks.AssertActualAndExpectedCalls();

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

TEST_FUNCTION(IoTHubTransportMqtt_MessageRecv_message_NULL_fail)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    auto handle = IoTHubTransportMqtt_Create(&config);

    mocks.ResetAllCalls();

    // act
    ASSERT_IS_NOT_NULL((void*)g_fnMqttMsgRecv);
    g_fnMqttMsgRecv(NULL, g_callbackCtx);

    // assert
    mocks.AssertActualAndExpectedCalls();

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

TEST_FUNCTION(IoTHubTransportMqtt_MessageRecv_context_NULL_fail)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    auto handle = IoTHubTransportMqtt_Create(&config);

    mocks.ResetAllCalls();

    // act
    ASSERT_IS_NOT_NULL((void*)g_fnMqttMsgRecv);
    g_fnMqttMsgRecv(TEST_MQTT_MESSAGE_HANDLE, NULL);

    // assert
    mocks.AssertActualAndExpectedCalls();

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

TEST_FUNCTION(IoTHubTransportMqtt_MessageRecv_Message_context_NULL_fail)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    auto handle = IoTHubTransportMqtt_Create(&config);

    mocks.ResetAllCalls();

    // act
    ASSERT_IS_NOT_NULL((void*)g_fnMqttMsgRecv);
    g_fnMqttMsgRecv(NULL, NULL);

    // assert
    mocks.AssertActualAndExpectedCalls();

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

TEST_FUNCTION(IoTHubTransportMqtt_MessageRecv_succeed)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    auto handle = IoTHubTransportMqtt_Create(&config);
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);
    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, mqttmessage_getApplicationMsg(TEST_MQTT_MESSAGE_HANDLE) );
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_CreateFromByteArray(appMessage, appMsgSize) );
    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_MessageCallback(TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_IOTHUB_MSG_BYTEARRAY));
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Destroy(TEST_IOTHUB_MSG_BYTEARRAY));

    STRICT_EXPECTED_CALL(mocks, mqttmessage_getTopicName(TEST_MQTT_MESSAGE_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_MQTT_MSG_TOPIC));
    EXPECTED_CALL(mocks, STRING_TOKENIZER_create(IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, STRING_new());
    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(IGNORED_PTR_ARG, IGNORED_PTR_ARG, "&"))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    EXPECTED_CALL(mocks, STRING_TOKENIZER_destroy(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Properties(TEST_IOTHUB_MSG_BYTEARRAY));
    EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG));

    // act
    ASSERT_IS_NOT_NULL((void*)g_fnMqttMsgRecv);
    g_fnMqttMsgRecv(TEST_MQTT_MESSAGE_HANDLE, g_callbackCtx);

    // assert
    mocks.AssertActualAndExpectedCalls();

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

TEST_FUNCTION(IoTHubTransportMqtt_MessageRecv_with_Properties_succeed)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    auto handle = IoTHubTransportMqtt_Create(&config);
    g_tokenizerIndex = 1;
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);
    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, mqttmessage_getApplicationMsg(TEST_MQTT_MESSAGE_HANDLE));
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_CreateFromByteArray(appMessage, appMsgSize));
    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_MessageCallback(TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_IOTHUB_MSG_BYTEARRAY));
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Destroy(TEST_IOTHUB_MSG_BYTEARRAY));

    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Properties(TEST_IOTHUB_MSG_BYTEARRAY));
    STRICT_EXPECTED_CALL(mocks, mqttmessage_getTopicName(TEST_MQTT_MESSAGE_HANDLE)).SetReturn(TEST_MQTT_MSG_TOPIC_W_1_PROP);
    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_MQTT_MSG_TOPIC_W_1_PROP));
    EXPECTED_CALL(mocks, STRING_TOKENIZER_create(IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, STRING_new());
    EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(IGNORED_PTR_ARG, IGNORED_PTR_ARG, "&"))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    EXPECTED_CALL(mocks, STRING_TOKENIZER_destroy(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));

    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(IGNORED_PTR_ARG, IGNORED_PTR_ARG, "&"))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .ExpectedTimesExactly(5);
    EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .ExpectedTimesExactly(5);
    EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
        .ExpectedTimesExactly(4);
    EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
        .ExpectedTimesExactly(4);
    EXPECTED_CALL(mocks, Map_AddOrUpdate(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .ExpectedTimesExactly(2);

    // act
    ASSERT_IS_NOT_NULL((void*)g_fnMqttMsgRecv);
    g_fnMqttMsgRecv(TEST_MQTT_MESSAGE_HANDLE, g_callbackCtx);

    // assert
    mocks.AssertActualAndExpectedCalls();

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

TEST_FUNCTION(IoTHubTransportMqtt_MessageRecv_with_Properties_Map_AddorUpdate_fail)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    auto handle = IoTHubTransportMqtt_Create(&config);
    g_tokenizerIndex = 1;
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);
    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, mqttmessage_getApplicationMsg(TEST_MQTT_MESSAGE_HANDLE));
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_CreateFromByteArray(appMessage, appMsgSize));
    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_MessageCallback(TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_IOTHUB_MSG_BYTEARRAY));
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Destroy(TEST_IOTHUB_MSG_BYTEARRAY));

    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Properties(TEST_IOTHUB_MSG_BYTEARRAY));
    STRICT_EXPECTED_CALL(mocks, mqttmessage_getTopicName(TEST_MQTT_MESSAGE_HANDLE)).SetReturn(TEST_MQTT_MSG_TOPIC_W_1_PROP);
    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_MQTT_MSG_TOPIC_W_1_PROP));
    EXPECTED_CALL(mocks, STRING_TOKENIZER_create(IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, STRING_new());
    EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(IGNORED_PTR_ARG, IGNORED_PTR_ARG, "&"))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    EXPECTED_CALL(mocks, STRING_TOKENIZER_destroy(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));

    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(IGNORED_PTR_ARG, IGNORED_PTR_ARG, "&"))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .ExpectedTimesExactly(2);
    EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
        .ExpectedTimesExactly(2);
    EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
        .ExpectedTimesExactly(2);
    EXPECTED_CALL(mocks, Map_AddOrUpdate(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG)).SetReturn(MAP_ERROR);

    // act
    ASSERT_IS_NOT_NULL((void*)g_fnMqttMsgRecv);
    g_fnMqttMsgRecv(TEST_MQTT_MESSAGE_HANDLE, g_callbackCtx);

    // assert
    mocks.AssertActualAndExpectedCalls();

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

TEST_FUNCTION(IoTHubTransportMqtt_MessageRecv_STRING_construct_fail)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    auto handle = IoTHubTransportMqtt_Create(&config);
    g_tokenizerIndex = 1;
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);
    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, mqttmessage_getApplicationMsg(TEST_MQTT_MESSAGE_HANDLE));
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_CreateFromByteArray(appMessage, appMsgSize));
    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_MessageCallback(TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_IOTHUB_MSG_BYTEARRAY));
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Destroy(TEST_IOTHUB_MSG_BYTEARRAY));

    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_MQTT_MSG_TOPIC_W_1_PROP));
    STRICT_EXPECTED_CALL(mocks, mqttmessage_getTopicName(TEST_MQTT_MESSAGE_HANDLE)).SetReturn(TEST_MQTT_MSG_TOPIC_W_1_PROP);
    EXPECTED_CALL(mocks, STRING_TOKENIZER_create(IGNORED_PTR_ARG)).SetReturn((STRING_TOKENIZER_HANDLE)NULL);
    EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));

    // act
    ASSERT_IS_NOT_NULL((void*)g_fnMqttMsgRecv);
    g_fnMqttMsgRecv(TEST_MQTT_MESSAGE_HANDLE, g_callbackCtx);

    // assert
    mocks.AssertActualAndExpectedCalls();

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

TEST_FUNCTION(IoTHubTransportMqtt_MessageRecv_IoTHubMessage_Properties_fail)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    auto handle = IoTHubTransportMqtt_Create(&config);
    g_tokenizerIndex = 1;
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);
    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, mqttmessage_getApplicationMsg(TEST_MQTT_MESSAGE_HANDLE));
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_CreateFromByteArray(appMessage, appMsgSize));
    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_MessageCallback(TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_IOTHUB_MSG_BYTEARRAY));
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Destroy(TEST_IOTHUB_MSG_BYTEARRAY));

    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_MQTT_MSG_TOPIC_W_1_PROP));
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Properties(TEST_IOTHUB_MSG_BYTEARRAY)).SetReturn((MAP_HANDLE)NULL);
    STRICT_EXPECTED_CALL(mocks, mqttmessage_getTopicName(TEST_MQTT_MESSAGE_HANDLE)).SetReturn(TEST_MQTT_MSG_TOPIC_W_1_PROP);
    EXPECTED_CALL(mocks, STRING_TOKENIZER_create(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, STRING_TOKENIZER_destroy(TEST_STRING_TOKENIZER_HANDLE));

    // act
    ASSERT_IS_NOT_NULL((void*)g_fnMqttMsgRecv);
    g_fnMqttMsgRecv(TEST_MQTT_MESSAGE_HANDLE, g_callbackCtx);

    // assert
    mocks.AssertActualAndExpectedCalls();

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

TEST_FUNCTION(IoTHubTransportMqtt_MessageRecv_with_System_Properties_succeed)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    auto handle = IoTHubTransportMqtt_Create(&config);
    g_tokenizerIndex = 1;
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);
    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, mqttmessage_getApplicationMsg(TEST_MQTT_MESSAGE_HANDLE));
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_CreateFromByteArray(appMessage, appMsgSize));
    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_MessageCallback(TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_IOTHUB_MSG_BYTEARRAY));
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Destroy(TEST_IOTHUB_MSG_BYTEARRAY));

    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Properties(TEST_IOTHUB_MSG_BYTEARRAY));
    STRICT_EXPECTED_CALL(mocks, mqttmessage_getTopicName(TEST_MQTT_MESSAGE_HANDLE)).SetReturn(TEST_MQTT_MSG_TOPIC_W_1_PROP);
    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_MQTT_MSG_TOPIC_W_1_PROP));
    EXPECTED_CALL(mocks, STRING_TOKENIZER_create(IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, STRING_new());
    EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG));

    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(IGNORED_PTR_ARG, IGNORED_PTR_ARG, "&"))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .ExpectedTimesExactly(6);
    EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .ExpectedTimesExactly(5);
    EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
        .ExpectedTimesExactly(4);
    EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
        .ExpectedTimesExactly(4);
    EXPECTED_CALL(mocks, Map_AddOrUpdate(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .ExpectedTimesExactly(2);

    EXPECTED_CALL(mocks, STRING_TOKENIZER_destroy(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));

    // act
    ASSERT_IS_NOT_NULL((void*)g_fnMqttMsgRecv);
    g_fnMqttMsgRecv(TEST_MQTT_MESSAGE_HANDLE, g_callbackCtx);

    // assert
    mocks.AssertActualAndExpectedCalls();

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

TEST_FUNCTION(IoTHubTransportMqtt_MessageRecv_CreateIotHubMessage_Fail)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    auto handle = IoTHubTransportMqtt_Create(&config);

    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, mqttmessage_getApplicationMsg(TEST_MQTT_MESSAGE_HANDLE));
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_CreateFromByteArray(appMessage, appMsgSize))
        .SetReturn((IOTHUB_MESSAGE_HANDLE)NULL);

    // act
    ASSERT_IS_NOT_NULL((void*)g_fnMqttMsgRecv);
    g_fnMqttMsgRecv(TEST_MQTT_MESSAGE_HANDLE, g_callbackCtx);

    // assert
    mocks.AssertActualAndExpectedCalls();

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

TEST_FUNCTION(IoTHubTransportMqtt_MessageRecv_messagecallback_ABANDONED_fail)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    QOS_VALUE QosValue[] = { DELIVER_AT_LEAST_ONCE };
    SUBSCRIBE_ACK suback;
    suback.packetId = 1234;
    suback.qosCount = 1;
    suback.qosReturn = QosValue;

    DList_InsertTailList(config.waitingToSend, &(message1.entry));
    auto handle = IoTHubTransportMqtt_Create(&config);
    g_fnMqttOperationCallback(TEST_MQTT_CLIENT_HANDLE, MQTT_CLIENT_ON_SUBSCRIBE_ACK, &suback, g_callbackCtx);
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);
    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, mqttmessage_getApplicationMsg(TEST_MQTT_MESSAGE_HANDLE));
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_CreateFromByteArray(appMessage, appMsgSize));
    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_MessageCallback(TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_IOTHUB_MSG_BYTEARRAY))
        .SetReturn((IOTHUBMESSAGE_DISPOSITION_RESULT)IOTHUBMESSAGE_ABANDONED);
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Destroy(TEST_IOTHUB_MSG_BYTEARRAY));

    STRICT_EXPECTED_CALL(mocks, mqttmessage_getTopicName(TEST_MQTT_MESSAGE_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_MQTT_MSG_TOPIC));
    EXPECTED_CALL(mocks, STRING_TOKENIZER_create(IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, STRING_new());
    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(IGNORED_PTR_ARG, IGNORED_PTR_ARG, "&"))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    EXPECTED_CALL(mocks, STRING_TOKENIZER_destroy(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Properties(TEST_IOTHUB_MSG_BYTEARRAY));
    EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG));

    // act
    ASSERT_IS_NOT_NULL((void*)g_fnMqttMsgRecv);
    g_fnMqttMsgRecv(TEST_MQTT_MESSAGE_HANDLE, g_callbackCtx);

    // assert
    mocks.AssertActualAndExpectedCalls();

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

TEST_FUNCTION(IoTHubTransportMqtt_MessageRecv_messagecallback_REJECTED_fail)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    QOS_VALUE QosValue[] = { DELIVER_AT_LEAST_ONCE };
    SUBSCRIBE_ACK suback;
    suback.packetId = 1234;
    suback.qosCount = 1;
    suback.qosReturn = QosValue;

    DList_InsertTailList(config.waitingToSend, &(message1.entry));
    auto handle = IoTHubTransportMqtt_Create(&config);
    g_fnMqttOperationCallback(TEST_MQTT_CLIENT_HANDLE, MQTT_CLIENT_ON_SUBSCRIBE_ACK, &suback, g_callbackCtx);
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);
    IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);
    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, mqttmessage_getApplicationMsg(TEST_MQTT_MESSAGE_HANDLE));
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_CreateFromByteArray(appMessage, appMsgSize));
    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_MessageCallback(TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_IOTHUB_MSG_BYTEARRAY))
        .SetReturn((IOTHUBMESSAGE_DISPOSITION_RESULT)IOTHUBMESSAGE_REJECTED);
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Destroy(TEST_IOTHUB_MSG_BYTEARRAY));

    STRICT_EXPECTED_CALL(mocks, mqttmessage_getTopicName(TEST_MQTT_MESSAGE_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_MQTT_MSG_TOPIC));
    EXPECTED_CALL(mocks, STRING_TOKENIZER_create(IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, STRING_new());
    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(IGNORED_PTR_ARG, IGNORED_PTR_ARG, "&"))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    EXPECTED_CALL(mocks, STRING_TOKENIZER_destroy(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Properties(TEST_IOTHUB_MSG_BYTEARRAY));
    EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG));

    // act
    ASSERT_IS_NOT_NULL( (void*)g_fnMqttMsgRecv);
    g_fnMqttMsgRecv(TEST_MQTT_MESSAGE_HANDLE, g_callbackCtx);

    // assert
    mocks.AssertActualAndExpectedCalls();

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

// Tests_SRS_IOTHUB_MQTT_TRANSPORT_17_004: [ IoTHubTransportMqtt_Register shall return the TRANSPORT_LL_HANDLE as the IOTHUB_DEVICE_HANDLE. ]
TEST_FUNCTION(IoTHubTransportMqtt_Register_succeeds_returns_transport)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    auto handle = IoTHubTransportMqtt_Create(&config);
    mocks.ResetAllCalls();

    EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG));

    // act
    auto devHandle = IoTHubTransportMqtt_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, config.waitingToSend);

    // assert
    ASSERT_IS_NOT_NULL(devHandle);
    ASSERT_ARE_EQUAL(void_ptr, handle, devHandle);
    mocks.AssertActualAndExpectedCalls();

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

// Tests_SRS_IOTHUB_MQTT_TRANSPORT_17_004: [ IoTHubTransportMqtt_Register shall return the TRANSPORT_LL_HANDLE as the IOTHUB_DEVICE_HANDLE. ]
TEST_FUNCTION(IoTHubTransportMqtt_Register_twice_fails_second_time)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    auto handle = IoTHubTransportMqtt_Create(&config);
    (void)IoTHubTransportMqtt_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, config.waitingToSend);

    mocks.ResetAllCalls();

    EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG));

    // act
    auto devHandle2 = IoTHubTransportMqtt_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, config.waitingToSend);

    // assert
    ASSERT_IS_NULL(devHandle2);
    mocks.AssertActualAndExpectedCalls();

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

// Tests_SRS_IOTHUB_MQTT_TRANSPORT_17_003: [ IoTHubTransportMqtt_Register shall return NULL if deviceId or deviceKey do not match the deviceId and deviceKey passed in during IoTHubTransportMqtt_Create.]
TEST_FUNCTION(IoTHubTransportMqtt_Register_deviceKey_mismatch_returns_null)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);
    IOTHUB_DEVICE_CONFIG deviceConfig = { TEST_DEVICE_ID, "not the right device key", NULL };

    auto handle = IoTHubTransportMqtt_Create(&config);
    mocks.ResetAllCalls();

    EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG));

    // act
    auto devHandle = IoTHubTransportMqtt_Register(handle, &deviceConfig, TEST_IOTHUB_CLIENT_LL_HANDLE, config.waitingToSend);

    // assert
    ASSERT_IS_NULL(devHandle);
    mocks.AssertActualAndExpectedCalls();

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

// Tests_SRS_IOTHUB_MQTT_TRANSPORT_17_003: [ IoTHubTransportMqtt_Register shall return NULL if deviceId or deviceKey do not match the deviceId and deviceKey passed in during IoTHubTransportMqtt_Create.]
TEST_FUNCTION(IoTHubTransportMqtt_Register_deviceid_mismatch_returns_null)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);
    IOTHUB_DEVICE_CONFIG deviceConfig = { "not a good id after all", TEST_DEVICE_KEY, NULL };

    auto handle = IoTHubTransportMqtt_Create(&config);
    mocks.ResetAllCalls();

    EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG));

    // act
    auto devHandle = IoTHubTransportMqtt_Register(handle, &deviceConfig, TEST_IOTHUB_CLIENT_LL_HANDLE, config.waitingToSend);

    // assert
    ASSERT_IS_NULL(devHandle);
    mocks.AssertActualAndExpectedCalls();

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

// Tests_SRS_IOTHUB_MQTT_TRANSPORT_17_002: [ IoTHubTransportMqtt_Register shall return NULL if device or waitingToSend are NULL.]
TEST_FUNCTION(IoTHubTransportMqtt_Register_wts_null_returns_null)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    auto handle = IoTHubTransportMqtt_Create(&config);
    mocks.ResetAllCalls();

    // act
    auto devHandle = IoTHubTransportMqtt_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, NULL);

    // assert
    ASSERT_IS_NULL(devHandle);
    mocks.AssertActualAndExpectedCalls();

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

// Tests_SRS_IOTHUB_MQTT_TRANSPORT_17_002: [ IoTHubTransportMqtt_Register shall return NULL if device or waitingToSend are NULL.]
TEST_FUNCTION(IoTHubTransportMqtt_Register_device_null_returns_null)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    auto handle = IoTHubTransportMqtt_Create(&config);
    mocks.ResetAllCalls();


    // act
    auto devHandle = IoTHubTransportMqtt_Register(handle, NULL, TEST_IOTHUB_CLIENT_LL_HANDLE, config.waitingToSend);

    // assert
    ASSERT_IS_NULL(devHandle);
    mocks.AssertActualAndExpectedCalls();

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

// Tests_SRS_IOTHUB_MQTT_TRANSPORT_03_001: [ IoTHubTransportMqtt_Register shall return NULL if deviceId, or both deviceKey and deviceSasToken are NULL.]
TEST_FUNCTION(IoTHubTransportMqtt_Register_deviceId_null_returns_null)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);
    IOTHUB_DEVICE_CONFIG deviceConfig = { NULL, TEST_DEVICE_KEY, NULL };

    auto handle = IoTHubTransportMqtt_Create(&config);
    mocks.ResetAllCalls();

    // act
    auto devHandle = IoTHubTransportMqtt_Register(handle, &deviceConfig, TEST_IOTHUB_CLIENT_LL_HANDLE, config.waitingToSend);

    // assert
    ASSERT_IS_NULL(devHandle);
    mocks.AssertActualAndExpectedCalls();

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

// Tests_SRS_IOTHUB_MQTT_TRANSPORT_03_001: [ IoTHubTransportMqtt_Register shall return NULL if deviceId, or both deviceKey and deviceSasToken are NULL.]
/*TEST_FUNCTION(IoTHubTransportMqtt_Register_deviceKey_null_and_deviceSasToken_null_returns_null)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);
    IOTHUB_DEVICE_CONFIG deviceConfig = { TEST_DEVICE_ID, NULL, NULL };

    auto handle = IoTHubTransportMqtt_Create(&config);
    mocks.ResetAllCalls();

    // act
    auto devHandle = IoTHubTransportMqtt_Register(handle, &deviceConfig, TEST_IOTHUB_CLIENT_LL_HANDLE, config.waitingToSend);

    // assert
    ASSERT_IS_NULL(devHandle);
    mocks.AssertActualAndExpectedCalls();

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}*/

// Tests_SRS_IOTHUB_MQTT_TRANSPORT_03_002: [ IoTHubTransportMqtt_Register shall return NULL if both deviceKey and deviceSasToken are provided.]
TEST_FUNCTION(IoTHubTransportMqtt_Register_deviceKey_and_deviceSasToken_both_provided_returns_null)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfigWithKeyAndSasToken(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_DEVICE_SAS, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);
    IOTHUB_DEVICE_CONFIG deviceConfig = { TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_DEVICE_SAS };

    auto handle = IoTHubTransportMqtt_Create(&config);
    mocks.ResetAllCalls();

    // act
    auto devHandle = IoTHubTransportMqtt_Register(handle, &deviceConfig, TEST_IOTHUB_CLIENT_LL_HANDLE, config.waitingToSend);

    // assert
    ASSERT_IS_NULL(devHandle);
    mocks.AssertActualAndExpectedCalls();

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

// SRS_IOTHUB_MQTT_TRANSPORT_17_004: [IoTHubTransportMqtt_Register shall return the TRANSPORT_LL_HANDLE as the IOTHUB_DEVICE_HANDLE.]
TEST_FUNCTION(IoTHubTransportMqtt_Register_deviceKey_null_and_deviceSas_valid_succeeds)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfigWithKeyAndSasToken(&config, TEST_DEVICE_ID, NULL, TEST_DEVICE_SAS, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);
    IOTHUB_DEVICE_CONFIG deviceConfig = { TEST_DEVICE_ID, NULL, TEST_DEVICE_SAS };

    auto handle = IoTHubTransportMqtt_Create(&config);
    mocks.ResetAllCalls();

    EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG));

    // act
    auto devHandle = IoTHubTransportMqtt_Register(handle, &deviceConfig, TEST_IOTHUB_CLIENT_LL_HANDLE, config.waitingToSend);

    // assert
    ASSERT_IS_NOT_NULL(devHandle);
    mocks.AssertActualAndExpectedCalls();

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

// Tests_SRS_IOTHUB_MQTT_TRANSPORT_17_001: [ IoTHubTransportMqtt_Register shall return NULL if the TRANSPORT_LL_HANDLE is NULL.]
TEST_FUNCTION(IoTHubTransportMqtt_Register_transport_null_returns_null)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    auto handle = IoTHubTransportMqtt_Create(&config);
    mocks.ResetAllCalls();

    // act
    auto devHandle = IoTHubTransportMqtt_Register(NULL, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, config.waitingToSend);

    // assert
    ASSERT_IS_NULL(devHandle);
    mocks.AssertActualAndExpectedCalls();

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

// Tests_SRS_IOTHUB_MQTT_TRANSPORT_17_005: [ IoTHubTransportMqtt_Unregister shall return. ]
TEST_FUNCTION(IoTHubTransportMqtt_Unregister_succeeds)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    auto handle = IoTHubTransportMqtt_Create(&config);
    auto devHandle = IoTHubTransportMqtt_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, config.waitingToSend);

    mocks.ResetAllCalls();


    // act
    IoTHubTransportMqtt_Unregister(devHandle);

    // assert
    mocks.AssertActualAndExpectedCalls();

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

// Tests_SRS_IOTHUB_MQTT_TRANSPORT_17_004: [ IoTHubTransportMqtt_Register shall return the TRANSPORT_LL_HANDLE as the IOTHUB_DEVICE_HANDLE. ]
TEST_FUNCTION(IoTHubTransportMqtt_Register_Unregister_Register_returns_handle)
{
    // arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);

    auto handle = IoTHubTransportMqtt_Create(&config);
    auto devHandle = IoTHubTransportMqtt_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, config.waitingToSend);
    IoTHubTransportMqtt_Unregister(devHandle);

    mocks.ResetAllCalls();

    EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG));

    // act
    auto devHandle2 = IoTHubTransportMqtt_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, config.waitingToSend);

    // assert
    ASSERT_IS_NOT_NULL(devHandle2);
    ASSERT_ARE_EQUAL(void_ptr, handle, devHandle2);
    mocks.AssertActualAndExpectedCalls();

    //cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

/*Tests_SRS_IOTHUB_MQTT_TRANSPORT_02_001: [ If handle is NULL then IoTHubTransportMqtt_GetHostname shall fail and return NULL. ]*/
TEST_FUNCTION(IoTHubTransportMqtt_GetHostname_with_NULL_handle_fails)
{
    ///arrange
    CIoTHubTransportMqttMocks mocks;

    ///act
    STRING_HANDLE hostname = IoTHubTransportMqtt_GetHostname(NULL);

    ///assert
    ASSERT_IS_NULL(hostname);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
}

/*Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_008: [If the upperConfig contains a valid protocolGatewayHostName value the this shall be used for the hostname, otherwise the hostname shall be constructed using the iothubname and iothubSuffix.]*/
TEST_FUNCTION(IoTHubTransportMqtt_GetHostname_with_non_NULL_handle_succeeds)
{
    ///arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, NULL);
    auto handle = IoTHubTransportMqtt_Create(&config);
    mocks.ResetAllCalls();

    ///act
    STRING_HANDLE hostname = IoTHubTransportMqtt_GetHostname(handle);

    ///assert
    ASSERT_IS_NOT_NULL(hostname);
    mocks.AssertActualAndExpectedCalls();
    ASSERT_ARE_EQUAL(char_ptr, "thisIsIotHubName" "." "thisIsIotHubSuffix", STRING_c_str(hostname));

    ///cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

/*Tests_SRS_IOTHUB_MQTT_TRANSPORT_07_008: [If the upperConfig contains a valid protocolGatewayHostName value the this shall be used for the hostname, otherwise the hostname shall be constructed using the iothubname and iothubSuffix.]*/
TEST_FUNCTION(IoTHubTransportMqtt_GetHostname_with_GatewayHostName_and_non_NULL_handle_succeeds)
{
    ///arrange
    CIoTHubTransportMqttMocks mocks;
    IOTHUBTRANSPORT_CONFIG config = { 0 };
    SetupIothubTransportConfig(&config, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX, TEST_PROTOCOL_GATEWAY_HOSTNAME);
    auto handle = IoTHubTransportMqtt_Create(&config);
    mocks.ResetAllCalls();

    ///act
    STRING_HANDLE hostname = IoTHubTransportMqtt_GetHostname(handle);

    ///assert
    ASSERT_IS_NOT_NULL(hostname);
    mocks.AssertActualAndExpectedCalls();
    ASSERT_ARE_EQUAL(char_ptr, TEST_PROTOCOL_GATEWAY_HOSTNAME, STRING_c_str(hostname));

    ///cleanup
    IoTHubTransportMqtt_Destroy(handle);
}

END_TEST_SUITE(iothubtransportmqtt)
