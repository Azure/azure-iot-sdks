// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <cstdlib>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include <cstddef>


#include "testrunnerswitcher.h"
#include "micromock.h"
#include "micromockcharstararenullterminatedstrings.h"
#include "doublylinkedlist.h"
#include "strings.h"
#include "mqttapi.h"

#include "macro_utils.h"
#undef DEFINE_ENUM
#define DEFINE_ENUM(enumName, ...) typedef enum C2(enumName, _TAG) { FOR_EACH_1(DEFINE_ENUMERATION_CONSTANT, __VA_ARGS__)} enumName; 

#include "iothubtransportmqtt.h"
#include "iothub_client_private.h"

#define GBALLOC_H
extern "C" int gballoc_init(void);
extern "C" void gballoc_deinit(void);
extern "C" void* gballoc_malloc(size_t size);
extern "C" void* gballoc_calloc(size_t nmemb, size_t size);
extern "C" void* gballoc_realloc(void* ptr, size_t size);
extern "C" void gballoc_free(void* ptr);


#define TEST_DEVICE_ID "thisIsDeviceID"
#define TEST_DEVICE_KEY "thisIsDeviceKey"
#define TEST_IOTHUB_NAME "thisIsIotHubName"
#define TEST_IOTHUB_SUFFIX "thisIsIotHubSuffix"
#define TEST_PROTOCOL_GATEWAY_HOSTNAME "thisIsAGatewayHostName"
#define TEST_VERY_LONG_DEVICE_ID "blablablablablablasd;flaksdjf;laskdfjal;sdkfjasdlfkjasdlfkjdslkfalsdkfjasdlkfjasldkfjasdlkfjasldkfjasldkfjasdlkfjadslkfjasldkfjl;ajsdf"
#define TEST_MQTTAPI_HANDLE (MQTTAPI_HANDLE)0x4242
#define TEST_IOTHUB_CLIENT_LL_HANDLE (IOTHUB_CLIENT_LL_HANDLE)0x4343
#define TEST_TRANSPORT_HANDLE (TRANSPORT_HANDLE)0x4444
#define TEST_MQTTAPI_TOPIC_HANDLE (MQTTAPI_TOPIC_HANDLE)0x4545
#define TEST_MQTTAPI_MESSAGE_TOPIC "devices/thisIsDeviceID/messages/devicebound"
#define TEST_MQTTAPI_EVENT_TOPIC "devices/thisIsDeviceID/messages/events"
#define TEST_DEFAULT_QOS_VALUE 1

/*this is the default message and has type BYTEARRAY*/
#define TEST_IOTHUB_MESSAGE_HANDLE_1 ((IOTHUB_MESSAGE_HANDLE)0x01d1)

/*this is a STRING type message*/
#define TEST_IOTHUB_MESSAGE_HANDLE_2 ((IOTHUB_MESSAGE_HANDLE)0x01d2)

static unsigned char buffer1[1] = { '1' };
static const size_t buffer1_size = sizeof(buffer1);


//Messages to be used in test
static IOTHUB_MESSAGE_LIST message1 =  /*this is the oldest message, always the first to be processed, send etc*/
{
    TEST_IOTHUB_MESSAGE_HANDLE_1,                    /*IOTHUB_MESSAGE_HANDLE messageHandle;                        */
    NULL,                                           /*IOTHUB_CLIENT_EVENT_CONFIRMATION_CALLBACK callback;     */
    NULL,                                           /*void* context;                                              */
    { NULL, NULL }                                  /*DLIST_ENTRY entry;                                          */
};

static IOTHUB_MESSAGE_LIST message2 =  /*this is the oldest message, always the first to be processed, send etc*/
{
    TEST_IOTHUB_MESSAGE_HANDLE_2,                    /*IOTHUB_MESSAGE_HANDLE messageHandle;                        */
    NULL,                                           /*IOTHUB_CLIENT_EVENT_CONFIRMATION_CALLBACK callback;     */
    NULL,                                           /*void* context;                                              */
    { NULL, NULL }                                  /*DLIST_ENTRY entry;                                          */
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
#include "map.c"

};

static MICROMOCK_MUTEX_HANDLE g_testByTest;
static MICROMOCK_GLOBAL_SEMAPHORE_HANDLE g_dllByDll;

static size_t currentmalloc_call;
static size_t whenShallmalloc_fail;

static size_t currentSTRING_construct_call;
static size_t whenShallSTRING_construct_fail;

static size_t currentSTRING_concat_call;
static size_t whenShallSTRING_concat_fail;



static const IOTHUB_CLIENT_CONFIG TEST_CONFIG_IOTHUBCLIENT_CONFIG=
{
    MQTT_Protocol,  /* IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol;   */
    TEST_DEVICE_ID,/*const char* deviceId;                       */
    TEST_DEVICE_KEY,/*const char* deviceKey;                      */
    TEST_IOTHUB_NAME,/*const char* iotHubName;                     */
    TEST_IOTHUB_SUFFIX,/* const char* iotHubSuffix;                    */
    TEST_PROTOCOL_GATEWAY_HOSTNAME /*const char* protocolGatewayHostName; */
};

static DLIST_ENTRY waitingToSend;

static const IOTHUB_CLIENT_CONFIG TEST_CONFIG_IOTHUBCLIENT_CONFIG_NULL_PROTOCOL =
{
    NULL, /*IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol;  */
    TEST_DEVICE_ID,/*const char* deviceId;                       */
    TEST_DEVICE_KEY,/*const char* deviceKey;                      */
    TEST_IOTHUB_NAME,/*const char* iotHubName;                     */
    TEST_IOTHUB_SUFFIX,/* const char* iotHubSuffix;                    */
    TEST_PROTOCOL_GATEWAY_HOSTNAME /*const char* protocolGatewayHostName; */
};

static const IOTHUB_CLIENT_CONFIG TEST_CONFIG_IOTHUBCLIENT_CONFIG_NULL_DEVICE_ID =
{
    MQTT_Protocol, /*IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol;  */
    NULL,/*const char* deviceId;                       */
    TEST_DEVICE_KEY,/*const char* deviceKey;                      */
    TEST_IOTHUB_NAME,/*const char* iotHubName;                     */
    TEST_IOTHUB_SUFFIX,/* const char* iotHubSuffix;                    */
    TEST_PROTOCOL_GATEWAY_HOSTNAME /*const char* protocolGatewayHostName; */
};

static const IOTHUB_CLIENT_CONFIG TEST_CONFIG_IOTHUBCLIENT_CONFIG_NULL_DEVICE_KEY =
{
    MQTT_Protocol,  /* IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol;   */
    TEST_DEVICE_ID,                                 /* const char* deviceId;                        */
    NULL,                                           /* const char* deviceKey;                       */
    TEST_IOTHUB_NAME,                               /* const char* iotHubName;                      */
    TEST_IOTHUB_SUFFIX,                             /* const char* iotHubSuffix;                    */
    TEST_PROTOCOL_GATEWAY_HOSTNAME                  /*const char* protocolGatewayHostName;          */
};

static const IOTHUB_CLIENT_CONFIG TEST_CONFIG_IOTHUBCLIENT_CONFIG_NULL_IOTHUB_NAME =
{
    MQTT_Protocol,       /*IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol;  */
    TEST_DEVICE_ID,                                      /*const char* deviceId;                       */
    TEST_DEVICE_KEY,                                     /*const char* deviceKey;                      */
    NULL,                                                /*const char* iotHubName;                     */
    TEST_IOTHUB_SUFFIX,                                  /* const char* iotHubSuffix;                  */
    TEST_PROTOCOL_GATEWAY_HOSTNAME                       /*const char* protocolGatewayHostName;        */
};

static const IOTHUB_CLIENT_CONFIG TEST_CONFIG_IOTHUBCLIENT_CONFIG_NULL_IOTHUB_SUFFIX =
{
    MQTT_Protocol,  /* IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol;   */
    TEST_DEVICE_ID,                                 /* const char* deviceId;                        */
    TEST_DEVICE_KEY,                                /* const char* deviceKey;                       */
    TEST_IOTHUB_NAME,                               /* const char* iotHubName;                      */
    NULL,                                           /* const char* iotHubSuffix;                    */
    TEST_PROTOCOL_GATEWAY_HOSTNAME                  /*const char* protocolGatewayHostName;          */
};

static const IOTHUB_CLIENT_CONFIG TEST_CONFIG_IOTHUBCLIENT_CONFIG_NULL_PROTOCOL_GATEWAY_HOSTNAME =
{
    MQTT_Protocol,  /* IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol;   */
    TEST_DEVICE_ID,                                 /* const char* deviceId;                        */
    TEST_DEVICE_KEY,                                /* const char* deviceKey;                       */
    TEST_IOTHUB_NAME,                               /* const char* iotHubName;                      */
    TEST_IOTHUB_SUFFIX,                             /* const char* iotHubSuffix;                    */
    NULL                                            /*const char* protocolGatewayHostName;          */
};


static const IOTHUB_CLIENT_CONFIG TEST_CONFIG_IOTHUBCLIENT_CONFIG_VERY_LONG_DEVICE_ID =
{
    MQTT_Protocol,  /* IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol;   */
    TEST_VERY_LONG_DEVICE_ID,                                 /* const char* deviceId;                        */
    TEST_DEVICE_KEY,                                /* const char* deviceKey;                       */
    TEST_IOTHUB_NAME,                               /* const char* iotHubName;                      */
    TEST_IOTHUB_SUFFIX,                             /* const char* iotHubSuffix;                    */
    TEST_PROTOCOL_GATEWAY_HOSTNAME                  /*const char* protocolGatewayHostName;          */
};

static const IOTHUB_CLIENT_CONFIG TEST_CONFIG_IOTHUBCLIENT_CONFIG_EMPTY_DEVICE_ID =
{
    MQTT_Protocol,  /* IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol;   */
    "",                                             /* const char* deviceId;                        */
    TEST_DEVICE_KEY,                                /* const char* deviceKey;                       */
    TEST_IOTHUB_NAME,                               /* const char* iotHubName;                      */
    TEST_IOTHUB_SUFFIX,                             /* const char* iotHubSuffix;                    */
    TEST_PROTOCOL_GATEWAY_HOSTNAME                  /*const char* protocolGatewayHostName;          */
};

static const IOTHUB_CLIENT_CONFIG TEST_CONFIG_IOTHUBCLIENT_CONFIG_EMPTY_DEVICE_KEY =
{
    MQTT_Protocol,  /* IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol;   */
    TEST_DEVICE_ID,                                 /* const char* deviceId;                        */
    "",                                             /* const char* deviceKey;                       */
    TEST_IOTHUB_NAME,                               /* const char* iotHubName;                      */
    TEST_IOTHUB_SUFFIX,                             /* const char* iotHubSuffix;                    */
    TEST_PROTOCOL_GATEWAY_HOSTNAME                  /*const char* protocolGatewayHostName;          */
};

static const IOTHUB_CLIENT_CONFIG TEST_CONFIG_IOTHUBCLIENT_CONFIG_EMPTY_IOTHUB_NAME =
{
    MQTT_Protocol,  /* IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol;   */
    TEST_DEVICE_ID,                                 /* const char* deviceId;                        */
    TEST_DEVICE_KEY,                                /* const char* deviceKey;                       */
    "",                                             /* const char* iotHubName;                      */
    TEST_IOTHUB_SUFFIX,                             /* const char* iotHubSuffix;                    */
    TEST_PROTOCOL_GATEWAY_HOSTNAME                  /*const char* protocolGatewayHostName;          */
};

static const IOTHUB_CLIENT_CONFIG TEST_CONFIG_IOTHUBCLIENT_CONFIG_EMPTY_PROTOCOL_GATEWAY_HOSTNAME =
{
    MQTT_Protocol,  /* IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol;   */
    TEST_DEVICE_ID,                                 /* const char* deviceId;                        */
    TEST_DEVICE_KEY,                                /* const char* deviceKey;                       */
    TEST_IOTHUB_NAME,                               /* const char* iotHubName;                      */
    TEST_IOTHUB_SUFFIX,                             /* const char* iotHubSuffix;                    */
    ""                                              /*const char* protocolGatewayHostName;          */
};

static IOTHUBTRANSPORT_CONFIG TEST_CONFIG
{
    &TEST_CONFIG_IOTHUBCLIENT_CONFIG,
    &waitingToSend
};

static IOTHUBTRANSPORT_CONFIG TEST_CONFIG_NULL_WAITING_TO_SEND
{
    &TEST_CONFIG_IOTHUBCLIENT_CONFIG,
    NULL
};

static IOTHUBTRANSPORT_CONFIG TEST_CONFIG_NULL_CONFIG
{
    NULL,
    (PDLIST_ENTRY)0x1
};

static IOTHUBTRANSPORT_CONFIG TEST_CONFIG_NULL_PROTOCOL
{
    &TEST_CONFIG_IOTHUBCLIENT_CONFIG_NULL_PROTOCOL,
    (PDLIST_ENTRY)0x1
};

static IOTHUBTRANSPORT_CONFIG TEST_CONFIG_NULL_DEVICE_ID
{
    &TEST_CONFIG_IOTHUBCLIENT_CONFIG_NULL_DEVICE_ID,
    (PDLIST_ENTRY)0x1
};

static IOTHUBTRANSPORT_CONFIG TEST_CONFIG_NULL_DEVICE_KEY
{
    &TEST_CONFIG_IOTHUBCLIENT_CONFIG_NULL_DEVICE_KEY,
    (PDLIST_ENTRY)0x1
};

static IOTHUBTRANSPORT_CONFIG TEST_CONFIG_NULL_IOTHUB_NAME
{
    &TEST_CONFIG_IOTHUBCLIENT_CONFIG_NULL_IOTHUB_NAME,
    (PDLIST_ENTRY)0x1
};

static IOTHUBTRANSPORT_CONFIG TEST_CONFIG_NULL_IOTHUB_SUFFIX
{
    &TEST_CONFIG_IOTHUBCLIENT_CONFIG_NULL_IOTHUB_SUFFIX,
    (PDLIST_ENTRY)0x1
};

static IOTHUBTRANSPORT_CONFIG TEST_CONFIG_NULL_PROTOCOL_GATEWAY_HOSTNAME
{
    &TEST_CONFIG_IOTHUBCLIENT_CONFIG_NULL_PROTOCOL_GATEWAY_HOSTNAME,
    (PDLIST_ENTRY)0x1
};


static IOTHUBTRANSPORT_CONFIG TEST_CONFIG_VERY_LONG_DEVICE_ID
{
    &TEST_CONFIG_IOTHUBCLIENT_CONFIG_VERY_LONG_DEVICE_ID,
    (PDLIST_ENTRY)0x1
};

static IOTHUBTRANSPORT_CONFIG TEST_CONFIG_EMPTY_DEVICE_ID
{
    &TEST_CONFIG_IOTHUBCLIENT_CONFIG_EMPTY_DEVICE_ID,
    (PDLIST_ENTRY)0x1
};

static IOTHUBTRANSPORT_CONFIG TEST_CONFIG_EMPTY_DEVICE_KEY
{
    &TEST_CONFIG_IOTHUBCLIENT_CONFIG_EMPTY_DEVICE_KEY,
    (PDLIST_ENTRY)0x1
};

static IOTHUBTRANSPORT_CONFIG TEST_CONFIG_EMPTY_IOTHUB_NAME
{
    &TEST_CONFIG_IOTHUBCLIENT_CONFIG_EMPTY_IOTHUB_NAME,
    (PDLIST_ENTRY)0x1
};

static IOTHUBTRANSPORT_CONFIG TEST_CONFIG_EMPTY_PROTOCOL_GATEWAY_HOSTNAME
{
    &TEST_CONFIG_IOTHUBCLIENT_CONFIG_EMPTY_PROTOCOL_GATEWAY_HOSTNAME,
    (PDLIST_ENTRY)0x1
};


//Callbacks for Testing
MQTTAPI_DeliveryComplete * testDeliverycompletedCallBack;
void* contextReceivedAtPublishMessage;

MQTTAPI_MessageArrived * testMessageArrivedCallback;
void* contextMessageArrived;

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
        if (whenShallmalloc_fail>0)
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


    /* IoTHubClient mocks*/
    MOCK_STATIC_METHOD_2(, IOTHUBMESSAGE_DISPOSITION_RESULT, IoTHubClient_LL_MessageCallback, IOTHUB_CLIENT_LL_HANDLE, handle, IOTHUB_MESSAGE_HANDLE, message)
    MOCK_METHOD_END(IOTHUBMESSAGE_DISPOSITION_RESULT, IOTHUBMESSAGE_ACCEPTED)

    MOCK_STATIC_METHOD_3(, void, IoTHubClient_LL_SendComplete, IOTHUB_CLIENT_LL_HANDLE, handle, PDLIST_ENTRY, completed, IOTHUB_BATCHSTATE_RESULT, result2)
    MOCK_VOID_METHOD_END()

    /* IoTHubMessage mocks */

    MOCK_STATIC_METHOD_1(, IOTHUBMESSAGE_CONTENT_TYPE, IoTHubMessage_GetContentType, IOTHUB_MESSAGE_HANDLE, iotHubMessageHandle)
        IOTHUBMESSAGE_CONTENT_TYPE result2;
        if (iotHubMessageHandle == TEST_IOTHUB_MESSAGE_HANDLE_1)
        {
            result2 = IOTHUBMESSAGE_BYTEARRAY;
        }
        else if (iotHubMessageHandle == TEST_IOTHUB_MESSAGE_HANDLE_2)
        {
            result2 = IOTHUBMESSAGE_STRING;
        }
        
    MOCK_METHOD_END(IOTHUBMESSAGE_CONTENT_TYPE, result2)

    MOCK_STATIC_METHOD_1(, const char*, IoTHubMessage_GetString, IOTHUB_MESSAGE_HANDLE, handle)
        const char* result2;
        if (handle == TEST_IOTHUB_MESSAGE_HANDLE_2)
        {
            result2 = "some text";
        }
        else
        {
            result2 = (const char*)NULL;
        }
    MOCK_METHOD_END(const char*, result2)

    MOCK_STATIC_METHOD_2(, IOTHUB_MESSAGE_HANDLE, IoTHubMessage_CreateFromByteArray, const unsigned char*, buffer, size_t, size)
    MOCK_METHOD_END(IOTHUB_MESSAGE_HANDLE, (IOTHUB_MESSAGE_HANDLE)0x42)

        MOCK_STATIC_METHOD_3(, IOTHUB_MESSAGE_RESULT, IoTHubMessage_GetByteArray, IOTHUB_MESSAGE_HANDLE, iotHubMessageHandle, const unsigned char**, buffer, size_t*, size)
    {
        switch ((uintptr_t)iotHubMessageHandle)
        {
        case ((uintptr_t)TEST_IOTHUB_MESSAGE_HANDLE_1) :
        {
            *buffer = buffer1;
            *size = buffer1_size;
            break;
        }
        default:
        {
            /*not expected really*/
            *buffer = (const unsigned char*)"333";
            *size = 3;
        }
        }

    }
    MOCK_METHOD_END(IOTHUB_MESSAGE_RESULT, IOTHUB_MESSAGE_OK)

    MOCK_STATIC_METHOD_1(, void, IoTHubMessage_Destroy, IOTHUB_MESSAGE_HANDLE, iotHubMessageHandle)
    MOCK_VOID_METHOD_END()

    /* MQTTAPI Mocks*/
    MOCK_STATIC_METHOD_1(, MQTTAPI_HANDLE, MQTTAPI_Create, const MQTTAPI_ConnectOptions*, options)
    MOCK_METHOD_END(MQTTAPI_HANDLE, TEST_MQTTAPI_HANDLE);

    MOCK_STATIC_METHOD_1(, void, MQTTAPI_Destroy, MQTTAPI_HANDLE, instance)
    MOCK_VOID_METHOD_END()

    MOCK_STATIC_METHOD_2(, MQTTAPI_TOPIC_HANDLE, MQTTAPI_Subscribe, MQTTAPI_HANDLE, instance, const char*, topic)
    MOCK_METHOD_END(MQTTAPI_TOPIC_HANDLE, TEST_MQTTAPI_TOPIC_HANDLE);

    MOCK_STATIC_METHOD_1(, void, MQTTAPI_Unsubscribe, MQTTAPI_TOPIC_HANDLE, topicInstance)
        MOCK_VOID_METHOD_END()

    MOCK_STATIC_METHOD_4(, MQTTAPI_RESULT, MQTTAPI_PublishMessage, MQTTAPI_HANDLE, instance, const char*, topicName, const MQTTAPI_Message*, msg, void*, context)
        contextReceivedAtPublishMessage = context;
    MOCK_METHOD_END(MQTTAPI_RESULT, MQTTAPI_OK);

    MOCK_STATIC_METHOD_3(, MQTTAPI_RESULT, MQTTAPI_SetMessageCallback, MQTTAPI_HANDLE, instance, void*, context, MQTTAPI_MessageArrived *, ma)
        testMessageArrivedCallback = ma;
        contextMessageArrived = context;
    MOCK_METHOD_END(MQTTAPI_RESULT, MQTTAPI_OK);

    MOCK_STATIC_METHOD_2(, MQTTAPI_RESULT, MQTTAPI_SetDeliveryCompletedCallback, MQTTAPI_HANDLE, instance, MQTTAPI_DeliveryComplete *, dc)
        testDeliverycompletedCallBack = dc;
    MOCK_METHOD_END(MQTTAPI_RESULT, MQTTAPI_OK);

    
    MOCK_STATIC_METHOD_1(, void, MQTTAPI_DoWork, MQTTAPI_HANDLE, instance);
    MOCK_VOID_METHOD_END()
};

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

DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportMqttMocks, , STRING_HANDLE, STRING_construct, const char*, s);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportMqttMocks, , int, STRING_concat, STRING_HANDLE, s1, const char*, s2);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportMqttMocks, , void, STRING_delete, STRING_HANDLE, s);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportMqttMocks, , const char*, STRING_c_str, STRING_HANDLE, s);

DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportMqttMocks, , IOTHUB_MESSAGE_HANDLE, IoTHubMessage_CreateFromByteArray, const unsigned char*, buffer, size_t, size);
DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubTransportMqttMocks, , IOTHUB_MESSAGE_RESULT, IoTHubMessage_GetByteArray, IOTHUB_MESSAGE_HANDLE, iotHubMessageHandle, const unsigned char**, buffer, size_t*, size);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportMqttMocks, , void, IoTHubMessage_Destroy, IOTHUB_MESSAGE_HANDLE, iotHubMessageHandle);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportMqttMocks, , IOTHUBMESSAGE_CONTENT_TYPE, IoTHubMessage_GetContentType, IOTHUB_MESSAGE_HANDLE, iotHubMessageHandle)
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportMqttMocks, , const char*, IoTHubMessage_GetString, IOTHUB_MESSAGE_HANDLE, handle);

DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportMqttMocks, , MQTTAPI_HANDLE, MQTTAPI_Create, const MQTTAPI_ConnectOptions*, options);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportMqttMocks, , void, MQTTAPI_Destroy, MQTTAPI_HANDLE, instance);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportMqttMocks, , MQTTAPI_TOPIC_HANDLE, MQTTAPI_Subscribe, MQTTAPI_HANDLE, instance, const char*, topic);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportMqttMocks, , void, MQTTAPI_Unsubscribe, MQTTAPI_TOPIC_HANDLE, topicInstance);
DECLARE_GLOBAL_MOCK_METHOD_4(CIoTHubTransportMqttMocks, ,MQTTAPI_RESULT, MQTTAPI_PublishMessage, MQTTAPI_HANDLE, handle, const char*, topicName, const MQTTAPI_Message*, msg, void*, context);
DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubTransportMqttMocks, , MQTTAPI_RESULT, MQTTAPI_SetMessageCallback, MQTTAPI_HANDLE, instance, void*, context, MQTTAPI_MessageArrived *, ma);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportMqttMocks, , MQTTAPI_RESULT, MQTTAPI_SetDeliveryCompletedCallback, MQTTAPI_HANDLE, instance, MQTTAPI_DeliveryComplete *, dc);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportMqttMocks, , void, MQTTAPI_DoWork, MQTTAPI_HANDLE, instance);


DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportMqttMocks, , IOTHUBMESSAGE_DISPOSITION_RESULT, IoTHubClient_LL_MessageCallback, IOTHUB_CLIENT_LL_HANDLE, handle, IOTHUB_MESSAGE_HANDLE, message)
DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubTransportMqttMocks, , void, IoTHubClient_LL_SendComplete, IOTHUB_CLIENT_LL_HANDLE, handle, PDLIST_ENTRY, completed, IOTHUB_BATCHSTATE_RESULT, result2)



BEGIN_TEST_SUITE(iothubtransportmqtt)

    TEST_SUITE_INITIALIZE(TestClassInitialize)
    {
        INITIALIZE_MEMORY_DEBUG(g_dllByDll);
        g_testByTest = MicroMockCreateMutex();
        ASSERT_IS_NOT_NULL(g_testByTest);
    }

    TEST_SUITE_CLEANUP(TestClassCleanup)
    {
        MicroMockDestroyMutex(g_testByTest);
        DEINITIALIZE_MEMORY_DEBUG(g_dllByDll);
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

       testDeliverycompletedCallBack = NULL;
       contextReceivedAtPublishMessage = NULL;

       BASEIMPLEMENTATION::DList_InitializeListHead(&waitingToSend);
    }


    TEST_FUNCTION_CLEANUP(TestMethodCleanup)
    {
        if (!MicroMockReleaseMutex(g_testByTest))
        {
            ASSERT_FAIL("failure in test framework at ReleaseMutex");
        }
    }


    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_001: [If parameter config is NULL then IoTHubTransportMqtt_Create shall fail and return NULL.]  */
    TEST_FUNCTION(IoTHubTransportMqtt_Create_with_NULL_parameter_fails)
    {
        ///arrange
        CIoTHubTransportMqttMocks mocks;

        ///act
        auto result = IoTHubTransportMqtt_Create(NULL);

        ///assert
        ASSERT_IS_NULL(result);

        ///cleanup
    }

    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_001: [If parameter config is NULL then IoTHubTransportMqtt_Create shall fail and return NULL.]  */
    TEST_FUNCTION(IoTHubTransportMqtt_Create_with_NULL_config_parameter_fails)
    {
        ///arrange
        CIoTHubTransportMqttMocks mocks;
        
        ///act
        auto result = IoTHubTransportMqtt_Create(&TEST_CONFIG_NULL_CONFIG);

        ///assert
        ASSERT_IS_NULL(result);

        ///cleanup
    }

    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_002: [IoTHubTransportMqtt_Create shall fail and return NULL if any fields of the config structure are NULL.]  */
    TEST_FUNCTION(IoTHubTransportMqtt_Create_with_NULL_waitingToSend_fails)
    {
        // arrange

        // act
        auto result = IoTHubTransportMqtt_Create(&TEST_CONFIG_NULL_WAITING_TO_SEND);

        // assert
        ASSERT_IS_NULL(result);
    }

    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_002: [IoTHubTransportMqtt_Create shall fail and return NULL if any fields of the config structure are NULL.]  */
    TEST_FUNCTION(IoTHubTransportMqtt_Create_with_NULL_protocol_fails)
    {
        // arrange

        // act
        auto result = IoTHubTransportMqtt_Create(&TEST_CONFIG_NULL_PROTOCOL);

        // assert
        ASSERT_IS_NULL(result);
    }

    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_002: [IoTHubTransportMqtt_Create shall fail and return NULL if any fields of the config structure are NULL.]  */
    TEST_FUNCTION(IoTHubTransportMqtt_Create_with_NULL_device_id_fails)
    {
        // arrange

        // act
        auto result = IoTHubTransportMqtt_Create(&TEST_CONFIG_NULL_DEVICE_ID);

        // assert
        ASSERT_IS_NULL(result);
    }

    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_002: [IoTHubTransportMqtt_Create shall fail and return NULL if any fields of the config structure are NULL.]  */
    TEST_FUNCTION(IoTHubTransportMqtt_Create_with_NULL_device_key_fails)
    {
        // arrange

        // act
        auto result = IoTHubTransportMqtt_Create(&TEST_CONFIG_NULL_DEVICE_KEY);

        // assert
        ASSERT_IS_NULL(result);
    }

    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_002: [IoTHubTransportMqtt_Create shall fail and return NULL if any fields of the config structure are NULL.]  */
    TEST_FUNCTION(IoTHubTransportMqtt_Create_with_NULL_iothub_name_fails)
    {
        // arrange

        // act
        auto result = IoTHubTransportMqtt_Create(&TEST_CONFIG_NULL_IOTHUB_NAME);

        // assert
        ASSERT_IS_NULL(result);
    }

    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_002: [IoTHubTransportMqtt_Create shall fail and return NULL if any fields of the config structure are NULL.]  */
    TEST_FUNCTION(IoTHubTransportMqtt_Create_with_NULL_iothub_suffix_fails)
    {
        // arrange

        // act
        auto result = IoTHubTransportMqtt_Create(&TEST_CONFIG_NULL_IOTHUB_SUFFIX);

        // assert
        ASSERT_IS_NULL(result);
    }

    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_002: [IoTHubTransportMqtt_Create shall fail and return NULL if any fields of the config structure are NULL.]  */
    TEST_FUNCTION(IoTHubTransportMqtt_Create_with_NULL_protocol_gateway_hostname_fails)
    {
        // arrange

        // act
        auto result = IoTHubTransportMqtt_Create(&TEST_CONFIG_NULL_PROTOCOL_GATEWAY_HOSTNAME);

        // assert
        ASSERT_IS_NULL(result);
    }


    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_003: [IoTHubTransportMqtt_Create shall fail and return NULL if the configuration deviceId is greater than 128 ascii characters.]  */
    TEST_FUNCTION(IoTHubTransportMqtt_Create_with_very_long_device_id_fails)
    {
        // arrange

        // act
        auto result = IoTHubTransportMqtt_Create(&TEST_CONFIG_VERY_LONG_DEVICE_ID);

        // assert
        ASSERT_IS_NULL(result);
    }

    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_013: [Otherwise, IoTHubTransportMqtt_Create shall succeed and return a non-NULL value]  */
    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_005: [Otherwise, IoTHubTransportMqtt_Create shall create an immutable string (further called “Event mqttEventTopic”) that will be formed by “devices/<deviceId>/messages/events”.] */
    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_007: [Otherwise, IoTHubTransportMqtt_Create shall create an immutable string(further called “Event mqttMessageTopic”) that will be formed by “devices / <deviceId> / messages / devicebound”.] */
    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_009: [IoTHubTransportMqtt_Create shall allocate memory to save its internal state where all topics, hostname, device_id, device_key, sasTokenSr and client handle shall be saved.]  */
    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_011: [IoTHubTransportMqtt_Create shall call MQTTAPI_Create passing MQTTAPI_connectOptions (deviceId, deviceKey and HostAddress).]   */
    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_025: [IoTHubTransportMqtt_Create shall call MQTTAPI_SetMessageCallback and MQTTAPI_SetDeliveryCompletedCallback for message arrived and message delivered (Answer to Event).] */
    TEST_FUNCTION(IoTHubTransportMqtt_Create_validConfig_happy_path_Succeed)
    {
        // arrange
        CIoTHubTransportMqttMocks mocks;

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_PROTOCOL_GATEWAY_HOSTNAME));
        STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_DEVICE_ID));
        STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_DEVICE_KEY));

        STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_IOTHUB_NAME));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(NULL, "."))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_concat(NULL, TEST_IOTHUB_SUFFIX))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_concat(NULL, "/devices/"))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_concat(NULL, TEST_DEVICE_ID))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_construct("devices/"));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(NULL, TEST_DEVICE_ID))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_concat(NULL, EVENT_ENDPOINT))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_construct("devices/"));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(NULL, TEST_DEVICE_ID))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_concat(NULL, MESSAGE_ENDPOINT))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_c_str(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_c_str(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_c_str(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_c_str(NULL))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, MQTTAPI_Create(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        
        STRICT_EXPECTED_CALL(mocks, MQTTAPI_SetMessageCallback(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();

        STRICT_EXPECTED_CALL(mocks, MQTTAPI_SetDeliveryCompletedCallback(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();

        STRICT_EXPECTED_CALL(mocks, DList_InitializeListHead(NULL))
            .IgnoreArgument(1);

        // act
        auto result = IoTHubTransportMqtt_Create(&TEST_CONFIG);

        // assert
        ASSERT_IS_NOT_NULL(result);

        mocks.AssertActualAndExpectedCalls();

        // clean up
        IoTHubTransportMqtt_Destroy(result);
    }

    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_004: [IoTHubTransportMqtt_Create shall fail and return NULL if the configuration parameters deviceId, deviceKey, IoTHubName and protocolGatewayHostName are an empty string in size.]  */
    TEST_FUNCTION(IoTHubTransportMqtt_Create_with_empty_device_id_fails)
    {
        // arrange

        // act
        auto result = IoTHubTransportMqtt_Create(&TEST_CONFIG_EMPTY_DEVICE_ID);

        // assert
        ASSERT_IS_NULL(result);

    }

    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_004: [IoTHubTransportMqtt_Create shall fail and return NULL if the configuration parameters deviceId, deviceKey, IoTHubName and protocolGatewayHostName are an empty string in size.]  */
    TEST_FUNCTION(IoTHubTransportMqtt_Create_with_empty_device_key_fails)
    {
        // arrange

        // act
        auto result = IoTHubTransportMqtt_Create(&TEST_CONFIG_EMPTY_DEVICE_KEY);

        // assert
        ASSERT_IS_NULL(result);
    }

    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_004: [IoTHubTransportMqtt_Create shall fail and return NULL if the configuration parameters deviceId, deviceKey, IoTHubName and protocolGatewayHostName are an empty string in size.]  */
    TEST_FUNCTION(IoTHubTransportMqtt_Create_with_empty_iothub_name_fails)
    {
        // arrange

        // act
        auto result = IoTHubTransportMqtt_Create(&TEST_CONFIG_EMPTY_IOTHUB_NAME);

        // assert
        ASSERT_IS_NULL(result);
    }

    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_004: [IoTHubTransportMqtt_Create shall fail and return NULL if the configuration parameters deviceId, deviceKey, IoTHubName and protocolGatewayHostName are an empty string in size.]  */
    TEST_FUNCTION(IoTHubTransportMqtt_Create_with_empty_protocol_gateway_hostname_fails)
    {
        // arrange

        // act
        auto result = IoTHubTransportMqtt_Create(&TEST_CONFIG_EMPTY_PROTOCOL_GATEWAY_HOSTNAME);

        // assert
        ASSERT_IS_NULL(result);
    }


    /*Tests_SRS_IOTHUBTRANSPORTMQTT_04_005: [Otherwise, IoTHubTransportMqtt_Create shall create an immutable string (further called “Event mqttEventTopic”) that will be formed by “devices/<deviceId>/messages/events”.] */
    TEST_FUNCTION(IotHubTransportMqtt_EVENT_ENDPOINT_constant_is_expected_value)
    {
        ASSERT_ARE_EQUAL(char_ptr, "/messages/events", EVENT_ENDPOINT);
    }

    
    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_006: [If creating the string fail for any reason then IoTHubTransportMqtt_Create shall fail and return NULL.] */
    TEST_FUNCTION(IoTHubTransportMqtt_Create_validConfig_String_construct_fails_fail)
    {
        // arrange
        CIoTHubTransportMqttMocks mocks;

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_PROTOCOL_GATEWAY_HOSTNAME));
        STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_DEVICE_ID));
        STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_DEVICE_KEY));

        STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_IOTHUB_NAME));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(NULL, "."))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_concat(NULL, TEST_IOTHUB_SUFFIX))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_concat(NULL, "/devices/"))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_concat(NULL, TEST_DEVICE_ID))
            .IgnoreArgument(1);

        whenShallSTRING_construct_fail = 5;
        STRICT_EXPECTED_CALL(mocks, STRING_construct("devices/"));
        STRICT_EXPECTED_CALL(mocks, STRING_delete(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_delete(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_delete(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_delete(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_delete(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(NULL))
            .IgnoreArgument(1);

        // act
        auto result = IoTHubTransportMqtt_Create(&TEST_CONFIG);

        // assert
        ASSERT_IS_NULL(result);
    }

    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_006: [If creating the string fail for any reason then IoTHubTransportMqtt_Create shall fail and return NULL.] */
    TEST_FUNCTION(IoTHubTransportMqtt_Create_validConfig_string_concat_devices_fails_fail)
    {
        // arrange
        CIoTHubTransportMqttMocks mocks;

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_PROTOCOL_GATEWAY_HOSTNAME));
        STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_DEVICE_ID));
        STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_DEVICE_KEY));

        STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_IOTHUB_NAME));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(NULL, "."))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_concat(NULL, TEST_IOTHUB_SUFFIX))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_concat(NULL, "/devices/"))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_concat(NULL, TEST_DEVICE_ID))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_construct("devices/"));

        STRICT_EXPECTED_CALL(mocks, STRING_concat(NULL, TEST_DEVICE_ID))
            .IgnoreArgument(1)
            .SetReturn((int)__LINE__);

        STRICT_EXPECTED_CALL(mocks, STRING_delete(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_delete(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_delete(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_delete(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_delete(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(NULL))
            .IgnoreArgument(1);

        // act
        auto result = IoTHubTransportMqtt_Create(&TEST_CONFIG);

        // assert
        ASSERT_IS_NULL(result);
    }

    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_006: [If creating the string fail for any reason then IoTHubTransportMqtt_Create shall fail and return NULL.] */
    TEST_FUNCTION(IoTHubTransportMqtt_Create_validConfig_string_concat_deviceid_fails_fail)
    {
        // arrange
        CIoTHubTransportMqttMocks mocks;

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_PROTOCOL_GATEWAY_HOSTNAME));
        STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_DEVICE_ID));
        STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_DEVICE_KEY));

        STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_IOTHUB_NAME));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(NULL, "."))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_concat(NULL, TEST_IOTHUB_SUFFIX))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_concat(NULL, "/devices/"))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_concat(NULL, TEST_DEVICE_ID))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_construct("devices/"));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(NULL, TEST_DEVICE_ID))
            .IgnoreArgument(1)
            .SetReturn((int)__LINE__);
        STRICT_EXPECTED_CALL(mocks, STRING_delete(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_delete(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_delete(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_delete(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_delete(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(NULL))
            .IgnoreArgument(1);

        // act
        auto result = IoTHubTransportMqtt_Create(&TEST_CONFIG);

        // assert
        ASSERT_IS_NULL(result);
    }

    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_006: [If creating the string fail for any reason then IoTHubTransportMqtt_Create shall fail and return NULL.] */
    TEST_FUNCTION(IoTHubTransportMqtt_Create_validConfig_string_concat_event_endpoint_fails_fail)
    {
        // arrange
        CIoTHubTransportMqttMocks mocks;

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_PROTOCOL_GATEWAY_HOSTNAME));
        STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_DEVICE_ID));
        STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_DEVICE_KEY));

        STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_IOTHUB_NAME));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(NULL, "."))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_concat(NULL, TEST_IOTHUB_SUFFIX))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_concat(NULL, "/devices/"))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_concat(NULL, TEST_DEVICE_ID))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_construct("devices/"));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(NULL, TEST_DEVICE_ID))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_concat(NULL, EVENT_ENDPOINT))
            .IgnoreArgument(1)
            .SetReturn((int)__LINE__);
        STRICT_EXPECTED_CALL(mocks, STRING_delete(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_delete(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_delete(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_delete(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_delete(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(NULL))
            .IgnoreArgument(1);

        // act
        auto result = IoTHubTransportMqtt_Create(&TEST_CONFIG);

        // assert
        ASSERT_IS_NULL(result);
    }


    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_008: [If creating the string fail for any reason then IoTHubTransportMqtt_Create shall fail and return NULL.] */
    TEST_FUNCTION(IoTHubTransportMqtt_Create_validConfig_string_contruct_for_message_fails_fail)
    {
        // arrange
        CIoTHubTransportMqttMocks mocks;

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_PROTOCOL_GATEWAY_HOSTNAME));
        STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_DEVICE_ID));
        STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_DEVICE_KEY));

        STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_IOTHUB_NAME));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(NULL, "."))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_concat(NULL, TEST_IOTHUB_SUFFIX))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_concat(NULL, "/devices/"))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_concat(NULL, TEST_DEVICE_ID))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_construct("devices/"));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(NULL, TEST_DEVICE_ID))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_concat(NULL, EVENT_ENDPOINT))
            .IgnoreArgument(1);

        whenShallSTRING_construct_fail = 6;
        STRICT_EXPECTED_CALL(mocks, STRING_construct("devices/"));

        STRICT_EXPECTED_CALL(mocks, STRING_delete(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_delete(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_delete(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_delete(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_delete(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_delete(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(NULL))
            .IgnoreArgument(1);

        // act
        auto result = IoTHubTransportMqtt_Create(&TEST_CONFIG);

        // assert
        ASSERT_IS_NULL(result);
    }

    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_008: [If creating the string fail for any reason then IoTHubTransportMqtt_Create shall fail and return NULL.] */
    TEST_FUNCTION(IoTHubTransportMqtt_Create_validConfig_string_concat_devices_for_message_fails_fail)
    {
        // arrange
        CIoTHubTransportMqttMocks mocks;

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_PROTOCOL_GATEWAY_HOSTNAME));
        STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_DEVICE_ID));
        STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_DEVICE_KEY));

        STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_IOTHUB_NAME));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(NULL, "."))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_concat(NULL, TEST_IOTHUB_SUFFIX))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_concat(NULL, "/devices/"))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_concat(NULL, TEST_DEVICE_ID))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_construct("devices/"));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(NULL, TEST_DEVICE_ID))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_concat(NULL, EVENT_ENDPOINT))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_construct("devices/"));

        STRICT_EXPECTED_CALL(mocks, STRING_concat(NULL, TEST_DEVICE_ID))
            .IgnoreArgument(1)
            .SetReturn((int)__LINE__);

        STRICT_EXPECTED_CALL(mocks, STRING_delete(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_delete(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_delete(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_delete(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_delete(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_delete(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(NULL))
            .IgnoreArgument(1);

        // act
        auto result = IoTHubTransportMqtt_Create(&TEST_CONFIG);

        // assert
        ASSERT_IS_NULL(result);
    }


    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_008: [If creating the string fail for any reason then IoTHubTransportMqtt_Create shall fail and return NULL.] */
    TEST_FUNCTION(IoTHubTransportMqtt_Create_validConfig_string_construct_sasTokenSr_fails_fail)
    {
        // arrange
        CIoTHubTransportMqttMocks mocks;

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_PROTOCOL_GATEWAY_HOSTNAME));
        STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_DEVICE_ID));
        STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_DEVICE_KEY));


        whenShallSTRING_construct_fail = 4;
        STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_IOTHUB_NAME));


        STRICT_EXPECTED_CALL(mocks, STRING_delete(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_delete(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_delete(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_delete(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(NULL))
            .IgnoreArgument(1);

        // act
        auto result = IoTHubTransportMqtt_Create(&TEST_CONFIG);

        // assert
        ASSERT_IS_NULL(result);
    }


    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_008: [If creating the string fail for any reason then IoTHubTransportMqtt_Create shall fail and return NULL.] */
    TEST_FUNCTION(IoTHubTransportMqtt_Create_validConfig_string_concat_deviceId_for_message_fails_fail)
    {
        // arrange
        CIoTHubTransportMqttMocks mocks;

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_PROTOCOL_GATEWAY_HOSTNAME));
        STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_DEVICE_ID));
        STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_DEVICE_KEY));

        STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_IOTHUB_NAME));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(NULL, "."))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_concat(NULL, TEST_IOTHUB_SUFFIX))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_concat(NULL, "/devices/"))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_concat(NULL, TEST_DEVICE_ID))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_construct("devices/"));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(NULL, TEST_DEVICE_ID))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_concat(NULL, EVENT_ENDPOINT))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_construct("devices/"));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(NULL, TEST_DEVICE_ID))
            .IgnoreArgument(1)
            .SetReturn(__LINE__);

        STRICT_EXPECTED_CALL(mocks, STRING_delete(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_delete(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_delete(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_delete(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_delete(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_delete(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(NULL))
            .IgnoreArgument(1);

        // act
        auto result = IoTHubTransportMqtt_Create(&TEST_CONFIG);

        // assert
        ASSERT_IS_NULL(result);
    }

    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_008: [If creating the string fail for any reason then IoTHubTransportMqtt_Create shall fail and return NULL.] */
    TEST_FUNCTION(IoTHubTransportMqtt_Create_validConfig_string_concat_messageEndpoint_for_message_fails_fail)
    {
        // arrange
        CIoTHubTransportMqttMocks mocks;

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_PROTOCOL_GATEWAY_HOSTNAME));
        STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_DEVICE_ID));
        STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_DEVICE_KEY));

        STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_IOTHUB_NAME));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(NULL, "."))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_concat(NULL, TEST_IOTHUB_SUFFIX))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_concat(NULL, "/devices/"))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_concat(NULL, TEST_DEVICE_ID))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_construct("devices/"));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(NULL, TEST_DEVICE_ID))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_concat(NULL, EVENT_ENDPOINT))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_construct("devices/"));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(NULL, TEST_DEVICE_ID))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_concat(NULL, MESSAGE_ENDPOINT))
            .IgnoreArgument(1)
            .SetReturn(__LINE__);

        STRICT_EXPECTED_CALL(mocks, STRING_delete(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_delete(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_delete(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_delete(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_delete(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_delete(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(NULL))
            .IgnoreArgument(1);

        // act
        auto result = IoTHubTransportMqtt_Create(&TEST_CONFIG);

        // assert
        ASSERT_IS_NULL(result);
    }

    /* Tetsts_SRS_IOTHUBTRANSPORTMQTT_04_010: [If memory allocation fails IoTHubTransportMqtt_Create shall fail and return NULL.]  */
    TEST_FUNCTION(IoTHubTransportMqtt_Create_validConfig_state_Allocation_fails_fail)
    {
        // arrange
        CIoTHubTransportMqttMocks mocks;

        whenShallmalloc_fail = 1;
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_PROTOCOL_GATEWAY_HOSTNAME));

        STRICT_EXPECTED_CALL(mocks, STRING_delete(NULL))
            .IgnoreArgument(1);

        // act
        auto result = IoTHubTransportMqtt_Create(&TEST_CONFIG);

        // assert
        ASSERT_IS_NULL(result);
    }

    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_012: [If MQTTAPI_Create fail, than IoTHubTransportMqtt_Create shall fail and return NULL.]  */
    TEST_FUNCTION(IoTHubTransportMqtt_Create_validConfig_MQTTAPI_Create_fails_fail)
    {
        // arrange
        CIoTHubTransportMqttMocks mocks;

        STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_PROTOCOL_GATEWAY_HOSTNAME));

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_DEVICE_ID));
        STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_DEVICE_KEY));

        STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_IOTHUB_NAME));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(NULL, "."))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_concat(NULL, TEST_IOTHUB_SUFFIX))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_concat(NULL, "/devices/"))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_concat(NULL, TEST_DEVICE_ID))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_construct("devices/"));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(NULL, TEST_DEVICE_ID))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_concat(NULL, EVENT_ENDPOINT))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_construct("devices/"));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(NULL, TEST_DEVICE_ID))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_concat(NULL, MESSAGE_ENDPOINT))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_c_str(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_c_str(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_c_str(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_c_str(NULL))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, MQTTAPI_Create(IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .SetReturn((MQTTAPI_HANDLE)NULL);

        STRICT_EXPECTED_CALL(mocks, STRING_delete(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_delete(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_delete(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_delete(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_delete(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_delete(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(NULL))
            .IgnoreArgument(1);

        // act
        auto result = IoTHubTransportMqtt_Create(&TEST_CONFIG);

        // assert
        ASSERT_IS_NULL(result);
    }

    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_026: [If MQTTAPI_SetMessageCallback or MQTTAPI_SetDeliveryCompletedCallback fail, IoTHubTransportMqtt_Create shall fail and return NULL.]  */
    TEST_FUNCTION(IoTHubTransportMqtt_Create_validConfig_MQTTAPI_SetMessageCallback_fails_fail)
    {
        // arrange
        CIoTHubTransportMqttMocks mocks;

        STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_PROTOCOL_GATEWAY_HOSTNAME));

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_DEVICE_ID));
        STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_DEVICE_KEY));

        STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_IOTHUB_NAME));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(NULL, "."))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_concat(NULL, TEST_IOTHUB_SUFFIX))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_concat(NULL, "/devices/"))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_concat(NULL, TEST_DEVICE_ID))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_construct("devices/"));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(NULL, TEST_DEVICE_ID))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_concat(NULL, EVENT_ENDPOINT))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_construct("devices/"));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(NULL, TEST_DEVICE_ID))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_concat(NULL, MESSAGE_ENDPOINT))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_c_str(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_c_str(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_c_str(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_c_str(NULL))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, MQTTAPI_Create(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, MQTTAPI_SetMessageCallback(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments()
            .SetReturn((MQTTAPI_RESULT)MQTTAPI_ERROR);

        STRICT_EXPECTED_CALL(mocks, STRING_delete(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_delete(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_delete(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_delete(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_delete(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_delete(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(NULL))
            .IgnoreArgument(1);

        // act
        auto result = IoTHubTransportMqtt_Create(&TEST_CONFIG);

        // assert
        ASSERT_IS_NULL(result);
    }

    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_026: [If MQTTAPI_SetMessageCallback or MQTTAPI_SetDeliveryCompletedCallback fail, IoTHubTransportMqtt_Create shall fail and return NULL.]  */
    TEST_FUNCTION(IoTHubTransportMqtt_Create_validConfig_MQTTAPI_SetDeliveryCompletedCallback_fails_fail)
    {
        // arrange
        CIoTHubTransportMqttMocks mocks;

        STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_PROTOCOL_GATEWAY_HOSTNAME));

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_DEVICE_ID));
        STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_DEVICE_KEY));

        STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_IOTHUB_NAME));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(NULL, "."))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_concat(NULL, TEST_IOTHUB_SUFFIX))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_concat(NULL, "/devices/"))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_concat(NULL, TEST_DEVICE_ID))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_construct("devices/"));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(NULL, TEST_DEVICE_ID))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_concat(NULL, EVENT_ENDPOINT))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_construct("devices/"));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(NULL, TEST_DEVICE_ID))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_concat(NULL, MESSAGE_ENDPOINT))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_c_str(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_c_str(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_c_str(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_c_str(NULL))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, MQTTAPI_Create(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, MQTTAPI_SetMessageCallback(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();

        STRICT_EXPECTED_CALL(mocks, MQTTAPI_SetDeliveryCompletedCallback(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments()
            .SetReturn((MQTTAPI_RESULT)MQTTAPI_ERROR);

        STRICT_EXPECTED_CALL(mocks, STRING_delete(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_delete(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_delete(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_delete(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_delete(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_delete(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(NULL))
            .IgnoreArgument(1);

        // act
        auto result = IoTHubTransportMqtt_Create(&TEST_CONFIG);

        // assert
        ASSERT_IS_NULL(result);
    }

    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_014: [IoTHubTransportMqtt_Destroy shall do nothing if parameter handle is NULL.] */
    TEST_FUNCTION(IoTHubTransportMqtt_Destroy_parameter_NULL_Do_Nothing)
    {
        // arrange
        CIoTHubTransportMqttMocks mocks;

        // act
        IoTHubTransportMqtt_Destroy(NULL);
        // assert

    }

    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_015: [Otherwise IoTHubTransportMqtt_Destroy shall free all the resources currently in use.]  */
    TEST_FUNCTION(IoTHubTransportMqtt_Destroy_succeeds)
    {
        // arrange
        CIoTHubTransportMqttMocks mocks;
        auto handle = IoTHubTransportMqtt_Create(&TEST_CONFIG);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_delete(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_delete(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_delete(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_delete(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_delete(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_delete(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(NULL))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, MQTTAPI_Destroy(NULL))
            .IgnoreArgument(1);

        // act
        IoTHubTransportMqtt_Destroy(handle);
        // assert

    }

    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_016: [If parameter handle is NULL than IoTHubTransportMqtt_Subscribe shall fail and return a non-zero value.] */
    TEST_FUNCTION(IoTHubTransportMqtt_Subscribe_parameter_NULL_Do_Nothing)
    {
        // arrange
        CIoTHubTransportMqttMocks mocks;

        // act
        IoTHubTransportMqtt_Subscribe(NULL);
        // assert

    }

    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_017: [Otherwise, IoTHubTransportMqtt_Subscribe shall call MQTTAPI_Subscribe.] */
    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_018: [If MQTTAPI_Subscribe succeeed, IoTHubTransportMqtt_Subscribe shall succeed.]  */
    TEST_FUNCTION(IoTHubTransportMqtt_Subscribe_happy_path_Succeed)
    {
        // arrange
        CIoTHubTransportMqttMocks mocks;
        auto handle = IoTHubTransportMqtt_Create(&TEST_CONFIG);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, MQTTAPI_Subscribe(TEST_MQTTAPI_HANDLE, TEST_MQTTAPI_MESSAGE_TOPIC));

        STRICT_EXPECTED_CALL(mocks, STRING_c_str(NULL))
            .IgnoreArgument(1)
            .SetReturn((const char*)TEST_MQTTAPI_MESSAGE_TOPIC);

        // act
        auto result = IoTHubTransportMqtt_Subscribe(handle);

        // assert
        ASSERT_ARE_EQUAL(int, 0, result);
        mocks.AssertActualAndExpectedCalls();

        //cleanup
        IoTHubTransportMqtt_Destroy(handle);
    }

    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_019: [Otherwise, IoTHubTransportMqtt_Subscribe shall fail and return a non - zero value.] */
    TEST_FUNCTION(IoTHubTransportMqtt_Subscribe_MQTTAPI_Subscribe_fails_fail)
    {
        // arrange
        CIoTHubTransportMqttMocks mocks;
        auto handle = IoTHubTransportMqtt_Create(&TEST_CONFIG);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, STRING_c_str(NULL))
            .IgnoreArgument(1)
            .SetReturn((const char*)TEST_MQTTAPI_MESSAGE_TOPIC);

        STRICT_EXPECTED_CALL(mocks, MQTTAPI_Subscribe(TEST_MQTTAPI_HANDLE, TEST_MQTTAPI_MESSAGE_TOPIC))
            .SetReturn((MQTTAPI_TOPIC_HANDLE)NULL);



        // act
        IoTHubTransportMqtt_Subscribe(handle);

        // assert
        //ASSERT_ARE_NOT_EQUAL(int, 0, result);
        mocks.AssertActualAndExpectedCalls();

        //cleanup
        IoTHubTransportMqtt_Destroy(handle);
    }

    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_020: [If parameter handle is NULL then IoTHubTransportMqtt_Unsubscribe shall do nothing.] */\
    TEST_FUNCTION(IoTHubTransportMqtt_UnSubscribe_parameter_NULL_Do_Nothing)
    {
        // arrange
        CIoTHubTransportMqttMocks mocks;

        // act
        IoTHubTransportMqtt_Unsubscribe(NULL);
        // assert

    }

    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_021: [Otherwise it shall call MQTTAPI_Unsubscribe.] */
    TEST_FUNCTION(IoTHubTransportMqtt_UnSubscribe_happyPath)
    {
        // arrange
        CIoTHubTransportMqttMocks mocks;
        auto handle = IoTHubTransportMqtt_Create(&TEST_CONFIG);
        IoTHubTransportMqtt_Subscribe(handle);
        mocks.ResetAllCalls();
        
        STRICT_EXPECTED_CALL(mocks, MQTTAPI_Unsubscribe(TEST_MQTTAPI_TOPIC_HANDLE));
        
        // act
        IoTHubTransportMqtt_Unsubscribe(handle);

        // assert
        mocks.AssertActualAndExpectedCalls();

        //cleanup
        IoTHubTransportMqtt_Destroy(handle);
    }

    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_038: [IoTHubTransportMqtt_SetOption shall return IOTHUB_CLIENT_ERROR , since it does not support any extra parameter today.]  */
    TEST_FUNCTION(IoTHubTransportMqtt_setoption_returnerror)
    {
        // arrange
        CIoTHubTransportMqttMocks mocks;
        auto handle = IoTHubTransportMqtt_Create(&TEST_CONFIG);
        mocks.ResetAllCalls();

        // act
        auto result = IoTHubTransportMqtt_SetOption(handle, "", "");

        // assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_ERROR , result);

        mocks.AssertActualAndExpectedCalls();

        //cleanup
        IoTHubTransportMqtt_Destroy(handle);
    }

    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_022: [If parameter handle is NULL or parameter iotHubClientHandle then IoTHubTransportMqtt_DoWork shall immediately return.]  */
    TEST_FUNCTION(IoTHubTransportMqtt_DoWork_parameter_handle_NULL_Do_Nothing)
    {
        // arrange
        CIoTHubTransportMqttMocks mocks;

        // act
        IoTHubTransportMqtt_DoWork(NULL, TEST_IOTHUB_CLIENT_LL_HANDLE);
        // assert

    }

    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_022: [If parameter handle is NULL or parameter iotHubClientHandle then IoTHubTransportMqtt_DoWork shall immediately return.]  */
    TEST_FUNCTION(IoTHubTransportMqtt_DoWork_parameter_iothubClient_NULL_Do_Nothing)
    {
        // arrange
        CIoTHubTransportMqttMocks mocks;

        // act
        IoTHubTransportMqtt_DoWork(TEST_TRANSPORT_HANDLE, NULL);
        // assert

    }

    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_022: [If parameter handle is NULL or parameter iotHubClientHandle then IoTHubTransportMqtt_DoWork shall immediately return.]  */
    TEST_FUNCTION(IoTHubTransportMqtt_DoWork_all_parameters_NULL_Do_Nothing)
    {
        // arrange
        CIoTHubTransportMqttMocks mocks;

        // act
        IoTHubTransportMqtt_DoWork(NULL, NULL);
        // assert

    }

    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_027: [IoTHubTransportMqtt_DoWork shall inspect the “waitingToSend” DLIST passed in config structure.]  */
    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_028: [If the list is empty then IoTHubTransportMqtt_DoWork shall proceed to the following action.]  */
    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_040: [IoTHubTransportMqtt_DoWork shall call MQTTAPI_DoWork everytime it is called, once.] */
    TEST_FUNCTION(IoTHubTransportMqtt_DoWork_happy_path_no_messages_succeed)
    {
        // arrange
        CIoTHubTransportMqttMocks mocks;
        auto handle = IoTHubTransportMqtt_Create(&TEST_CONFIG);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, MQTTAPI_DoWork(TEST_MQTTAPI_HANDLE));

        // act
        IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);
        
        //assert
        mocks.AssertActualAndExpectedCalls();

        //cleanup
        IoTHubTransportMqtt_Destroy(handle);

    }

    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_027: [IoTHubTransportMqtt_DoWork shall inspect the “waitingToSend” DLIST passed in config structure.]  */
    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_031: [If the list has at least one entry IotHubTransportMQtt_DoWork shall call IotHubMessage_GetData to receive message content and size.]*/
    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_033: [Otherwise IoTHubTransportMqtt_DoWork shall call MQTTAPI_PublishMessage.] */
    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_030: [Otherwise the message shall be removed from waitingToSend and moved to a list where it will be waiting for receiving an ACK from Mqtt server (IoTHub or ProtocolGateway)]  */
    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_040: [IoTHubTransportMqtt_DoWork shall call MQTTAPI_DoWork everytime it is called, once.] */
    TEST_FUNCTION(IoTHubTransportMqtt_DoWork_with_1_event_item_happy_path_succeeds)
    {
        // arrange
        CIoTHubTransportMqttMocks mocks;
        DList_InsertTailList(&(waitingToSend), &(message1.entry));
        auto handle = IoTHubTransportMqtt_Create(&TEST_CONFIG);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetContentType(message1.messageHandle));
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetByteArray(message1.messageHandle, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(2)
            .IgnoreArgument(3);

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, DList_RemoveEntryList(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_c_str(NULL))
            .IgnoreArgument(1);


        STRICT_EXPECTED_CALL(mocks, MQTTAPI_PublishMessage(IGNORED_PTR_ARG, TEST_MQTTAPI_EVENT_TOPIC, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(3)
            .IgnoreArgument(4);

        STRICT_EXPECTED_CALL(mocks, DList_InsertTailList(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2);

        STRICT_EXPECTED_CALL(mocks, MQTTAPI_DoWork(TEST_MQTTAPI_HANDLE));

        // act
        IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);
        
        //assert
        mocks.AssertActualAndExpectedCalls();

        //cleanup
        IoTHubTransportMqtt_Destroy(handle);
    }

    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_027: [IoTHubTransportMqtt_DoWork shall inspect the “waitingToSend” DLIST passed in config structure.]  */
    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_031: [If the list has at least one entry IotHubTransportMQtt_DoWork shall call IotHubMessage_GetData to receive message content and size.]*/
    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_033: [Otherwise IoTHubTransportMqtt_DoWork shall call MQTTAPI_PublishMessage.] */
    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_030: [Otherwise the message shall be removed from waitingToSend and moved to a list where it will be waiting for receiving an ACK from Mqtt server (IoTHub or ProtocolGateway)]  */
    /*Tests_SRS_IOTHUBTRANSPORTMQTT_02_001: [If the message type is IOTHUBMESSAGE_STRING then IotHubTransportMQtt_DoWork shall call IotHubMessage_GetString to receive message content and shall compute the size as strlen.] */
    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_040: [IoTHubTransportMqtt_DoWork shall call MQTTAPI_DoWork everytime it is called, once.] */
    TEST_FUNCTION(IoTHubTransportMqtt_DoWork_with_1_event_item_STRING_type_happy_path_succeeds)
    {
        // arrange
        CIoTHubTransportMqttMocks mocks;
        DList_InsertTailList(&(waitingToSend), &(message2.entry));
        auto handle = IoTHubTransportMqtt_Create(&TEST_CONFIG);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetContentType(message2.messageHandle));
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetString(message2.messageHandle));

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, DList_RemoveEntryList(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_c_str(NULL))
            .IgnoreArgument(1);


        STRICT_EXPECTED_CALL(mocks, MQTTAPI_PublishMessage(IGNORED_PTR_ARG, TEST_MQTTAPI_EVENT_TOPIC, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(3)
            .IgnoreArgument(4);

        STRICT_EXPECTED_CALL(mocks, DList_InsertTailList(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2);

        STRICT_EXPECTED_CALL(mocks, MQTTAPI_DoWork(TEST_MQTTAPI_HANDLE));

        // act
        IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

        //assert
        mocks.AssertActualAndExpectedCalls();

        //cleanup
        IoTHubTransportMqtt_Destroy(handle);
    }

    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_027: [IoTHubTransportMqtt_DoWork shall inspect the “waitingToSend” DLIST passed in config structure.]  */
    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_031: [If the list has at least one entry IotHubTransportMQtt_DoWork shall call IotHubMessage_GetData to receive message content and size.]*/
    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_033: [Otherwise IoTHubTransportMqtt_DoWork shall call MQTTAPI_PublishMessage.] */
    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_030: [Otherwise the message shall be removed from waitingToSend and moved to a list where it will be waiting for receiving an ACK from Mqtt server (IoTHub or ProtocolGateway)]  */
    /*Tests_SRS_IOTHUBTRANSPORTMQTT_02_001: [If the message type is IOTHUBMESSAGE_STRING then IotHubTransportMQtt_DoWork shall call IotHubMessage_GetString to receive message content and shall compute the size as strlen.] */
    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_040: [IoTHubTransportMqtt_DoWork shall call MQTTAPI_DoWork everytime it is called, once.] */
    TEST_FUNCTION(IoTHubTransportMqtt_DoWork_with_1_event_item_STRING_type_when_GetString_fails_it_fails)
    {
        // arrange
        CIoTHubTransportMqttMocks mocks;
        DList_InsertTailList(&(waitingToSend), &(message2.entry));
        auto handle = IoTHubTransportMqtt_Create(&TEST_CONFIG);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetContentType(message2.messageHandle));
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetString(message2.messageHandle))
            .SetReturn((const char*)NULL);

        STRICT_EXPECTED_CALL(mocks, MQTTAPI_DoWork(TEST_MQTTAPI_HANDLE));

        // act
        IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

        //assert
        mocks.AssertActualAndExpectedCalls();

        //cleanup
        IoTHubTransportMqtt_Destroy(handle);
    }
    
    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_032: [If IoTHubMessage_GetDatafail, IoTHubTransportMqtt_DoWork shall go to the next item on the list or move to the next action.]  */
    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_040: [IoTHubTransportMqtt_DoWork shall call MQTTAPI_DoWork everytime it is called, once.] */
    TEST_FUNCTION(IoTHubTransportMqtt_DoWork_with_1_event_item_GetDatafail_fail)
    {
        // arrange
        CIoTHubTransportMqttMocks mocks;
        DList_InsertTailList(&(waitingToSend), &(message1.entry));
        auto handle = IoTHubTransportMqtt_Create(&TEST_CONFIG);
        mocks.ResetAllCalls();

        

        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetContentType(message1.messageHandle));

        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetByteArray(message1.messageHandle, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(2)
            .IgnoreArgument(3)
            .SetReturn(IOTHUB_MESSAGE_ERROR);

        STRICT_EXPECTED_CALL(mocks, MQTTAPI_DoWork(TEST_MQTTAPI_HANDLE));

        // act
        IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

        //assert
        mocks.AssertActualAndExpectedCalls();

        //cleanup
        IoTHubTransportMqtt_Destroy(handle);
    }


    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_034: [If MQTTAPI_PublishMessage fails, IoTHubTransportMqtt_DoWork shall call IoTHubClient_LL_SendComplete with status of IOTHUB_BATCHSTATE_FAILED.] */
    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_040: [IoTHubTransportMqtt_DoWork shall call MQTTAPI_DoWork everytime it is called, once.] */
    TEST_FUNCTION(IoTHubTransportMqtt_DoWork_with_1_event_item_publish_message_fails_call_sendComplete)
    {
        // arrange
        CIoTHubTransportMqttMocks mocks;
        DList_InsertTailList(&(waitingToSend), &(message1.entry));
        auto handle = IoTHubTransportMqtt_Create(&TEST_CONFIG);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetContentType(message1.messageHandle));
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetByteArray(message1.messageHandle, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(2)
            .IgnoreArgument(3);

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, DList_RemoveEntryList(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_c_str(NULL))
            .IgnoreArgument(1);


        STRICT_EXPECTED_CALL(mocks, MQTTAPI_PublishMessage(IGNORED_PTR_ARG, TEST_MQTTAPI_EVENT_TOPIC, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(3)
            .IgnoreArgument(4)
            .SetReturn(MQTTAPI_ERROR);

        STRICT_EXPECTED_CALL(mocks, DList_InitializeListHead(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, DList_InsertTailList(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2);

        STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_SendComplete(TEST_IOTHUB_CLIENT_LL_HANDLE, IGNORED_PTR_ARG, IOTHUB_BATCHSTATE_FAILED))
            .IgnoreArgument(2);

        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, MQTTAPI_DoWork(TEST_MQTTAPI_HANDLE));

        // act
        IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

        //assert
        mocks.AssertActualAndExpectedCalls();

        //cleanup
        IoTHubTransportMqtt_Destroy(handle);
    }


    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_035: [IoTHubTransportMqtt_GetSendStatus shall return IOTHUB_CLIENT_INVALID_ARG if called with NULL parameter.] */
    TEST_FUNCTION(IoTHubTransportMqtt_GetSendStatus_InvalidHandleArgument_fail)
    {
        // arrange
        CIoTHubTransportMqttMocks mocks;
        auto handle = IoTHubTransportMqtt_Create(&TEST_CONFIG);
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

    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_035: [IoTHubTransportMqtt_GetSendStatus shall return IOTHUB_CLIENT_INVALID_ARG if called with NULL parameter.] */
    TEST_FUNCTION(IoTHubTransportMqtt_GetSendStatus_InvalidStatusArgument_fail)
    {
        // arrange
        CIoTHubTransportMqttMocks mocks;
        auto handle = IoTHubTransportMqtt_Create(&TEST_CONFIG);
        mocks.ResetAllCalls();

        // act
        IOTHUB_CLIENT_RESULT result = IoTHubTransportMqtt_GetSendStatus(handle, NULL);

        // assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, result, IOTHUB_CLIENT_INVALID_ARG);

        mocks.AssertActualAndExpectedCalls();

        // cleanup
        IoTHubTransportMqtt_Destroy(handle);
    }

    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_036: [IoTHubTransportMqtt_GetSendStatus shall return IOTHUB_CLIENT_OK and status IOTHUB_CLIENT_SEND_STATUS_IDLE if there are currently no event items to be sent or being sent.]  */
    TEST_FUNCTION(IoTHubTransportMqtt_GetSendStatus_empty_waitingToSend_and_empty_waitingforAck_success)
    {
        // arrange
        CIoTHubTransportMqttMocks mocks;
        auto handle = IoTHubTransportMqtt_Create(&TEST_CONFIG);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend));
        STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1);

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

    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_037: [IoTHubTransportMqtt_GetSendStatus shall return IOTHUB_CLIENT_OK and status IOTHUB_CLIENT_SEND_STATUS_BUSY if there are currently event items to be sent or being sent.] */
    TEST_FUNCTION(IoTHubTransportMqtt_GetSendStatus_waitingToSend_not_empty_success)
    {
        // arrange
        CIoTHubTransportMqttMocks mocks;
        auto handle = IoTHubTransportMqtt_Create(&TEST_CONFIG);

        IOTHUB_MESSAGE_HANDLE eventMessageHandle = IoTHubMessage_CreateFromByteArray(IGNORED_PTR_ARG, IGNORED_NUM_ARG);
        IOTHUB_MESSAGE_LIST newEntry;
        newEntry.messageHandle = &newEntry;
        DList_InsertTailList(&(waitingToSend), &(newEntry.entry));

        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend));

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

    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_039: [This function shall return a pointer to a structure of type TRANSPORT_PROVIDER having the following values for it’s fields:] */
    TEST_FUNCTION(MQTT_Protocol_succeeds)
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

    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_045: [If context is NULL, IoTHubTransportMqtt delivery completed callback shall do nothing.]  */
    TEST_FUNCTION(IoTHubTransportMqtt_delivered_NULL_context_do_Nothing)
    {
        // arrange
        CIoTHubTransportMqttMocks mocks;
        auto handle = IoTHubTransportMqtt_Create(&TEST_CONFIG);

        mocks.ResetAllCalls();

        // act
        testDeliverycompletedCallBack(NULL, MQTTAPI_CONFIRMATION_ERROR);
        // assert
        mocks.AssertActualAndExpectedCalls();

        //cleanup
        IoTHubTransportMqtt_Destroy(handle);
    }

    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_041: [IoTHubTransportMqtt shall search for the message received on context parameter to identify the message.] */
    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_042: [Once identified, IoTHubTransportMqtt shall remove the message from the WaitingforAck List.] */
    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_043: [If result IoTHubTransportMqtt is MQTTAPI_CONFIRMATION_OK IoTHubTransportMqtt shall call IoTHubClient_LL_SendComplete passing the message and status IOTHUB_BATCHSTATE_SUCCESS.] */
    TEST_FUNCTION(IoTHubTransportMqtt_delivered_with_1_event_item_happy_path_succeeds)
    {
        // arrange
        CIoTHubTransportMqttMocks mocks;
        DList_InsertTailList(&(waitingToSend), &(message1.entry));
        auto handle = IoTHubTransportMqtt_Create(&TEST_CONFIG);
        IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, DList_RemoveEntryList(IGNORED_PTR_ARG))
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(mocks, DList_InitializeListHead(IGNORED_PTR_ARG))
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(mocks, DList_InsertHeadList(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_SendComplete(TEST_IOTHUB_CLIENT_LL_HANDLE, IGNORED_PTR_ARG, IOTHUB_BATCHSTATE_SUCCESS))
            .IgnoreArgument(2);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(NULL))
            .IgnoreArgument(1);

        // act
        testDeliverycompletedCallBack(contextReceivedAtPublishMessage, MQTTAPI_CONFIRMATION_OK);

        //assert
        mocks.AssertActualAndExpectedCalls();

        //cleanup
        IoTHubTransportMqtt_Destroy(handle);
    }

    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_044: [Otherwise, IoTHubTransportMqtt shall call IoTHubClient_LL_SendComplete passing the message and status IOTHUB_BATCHSTATE_FAIL.] */
    TEST_FUNCTION(IoTHubTransportMqtt_delivered_with_1_event_item_MQTTAPI_CONFIRMATATION_ERROR_batchstate_failed)
    {
        // arrange
        CIoTHubTransportMqttMocks mocks;
        DList_InsertTailList(&(waitingToSend), &(message1.entry));
        auto handle = IoTHubTransportMqtt_Create(&TEST_CONFIG);
        IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, DList_RemoveEntryList(IGNORED_PTR_ARG))
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(mocks, DList_InitializeListHead(IGNORED_PTR_ARG))
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(mocks, DList_InsertHeadList(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_SendComplete(TEST_IOTHUB_CLIENT_LL_HANDLE, IGNORED_PTR_ARG, IOTHUB_BATCHSTATE_FAILED))
            .IgnoreArgument(2);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(NULL))
            .IgnoreArgument(1);

        // act
        testDeliverycompletedCallBack(contextReceivedAtPublishMessage, MQTTAPI_CONFIRMATION_ERROR);

        //assert
        mocks.AssertActualAndExpectedCalls();

        //cleanup
        IoTHubTransportMqtt_Destroy(handle);
    }

    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_044: [Otherwise, IoTHubTransportMqtt shall call IoTHubClient_LL_SendComplete passing the message and status IOTHUB_BATCHSTATE_FAIL.] */
    TEST_FUNCTION(IoTHubTransportMqtt_delivered_with_1_event_item_MQTTAPI_CONFIRMATATION_BECAUSE_DESTROY_batchstate_failed)
    {
        // arrange
        CIoTHubTransportMqttMocks mocks;
        DList_InsertTailList(&(waitingToSend), &(message1.entry));
        auto handle = IoTHubTransportMqtt_Create(&TEST_CONFIG);
        IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, DList_RemoveEntryList(IGNORED_PTR_ARG))
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(mocks, DList_InitializeListHead(IGNORED_PTR_ARG))
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(mocks, DList_InsertHeadList(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_SendComplete(TEST_IOTHUB_CLIENT_LL_HANDLE, IGNORED_PTR_ARG, IOTHUB_BATCHSTATE_FAILED))
            .IgnoreArgument(2);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(NULL))
            .IgnoreArgument(1);

        // act
        testDeliverycompletedCallBack(contextReceivedAtPublishMessage, MQTTAPI_CONFIRMATION_BECAUSE_DESTROY);

        //assert
        mocks.AssertActualAndExpectedCalls();

        //cleanup
        IoTHubTransportMqtt_Destroy(handle);
    }

    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_046: [If IoTHubTransportMqtt receives a call to MQTTAPI_MessageArrived with message parameter or context NULL, it shall return false]    */
    TEST_FUNCTION(IoTHubTransportMqtt_messageArrived_NULL_message_return_false)
    {
        // arrange
        CIoTHubTransportMqttMocks mocks;
        auto handle = IoTHubTransportMqtt_Create(&TEST_CONFIG);

        mocks.ResetAllCalls();

        // act
        auto result = testMessageArrivedCallback(contextMessageArrived, NULL);
        // assert
        ASSERT_IS_FALSE(result);

        mocks.AssertActualAndExpectedCalls();

        //cleanup
        IoTHubTransportMqtt_Destroy(handle);
    }

    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_046: [If IoTHubTransportMqtt receives a call to MQTTAPI_MessageArrived with message parameter or context NULL, it shall return false]    */
    TEST_FUNCTION(IoTHubTransportMqtt_messageArrived_NULL_contex_return_false)
    {
        // arrange
        CIoTHubTransportMqttMocks mocks;
        auto handle = IoTHubTransportMqtt_Create(&TEST_CONFIG);

        mocks.ResetAllCalls();

        MQTTAPI_Message testMessage;
        testMessage.payload = buffer1;
        testMessage.payloadlen = buffer1_size;

        // act
        auto result = testMessageArrivedCallback(NULL, &testMessage);
        // assert
        ASSERT_IS_FALSE(result);

        mocks.AssertActualAndExpectedCalls();

        //cleanup
        IoTHubTransportMqtt_Destroy(handle);
    }

    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_047: [IotHubTransportMqtt shall Create an IoTHubMessage by calling IoTHubMessage_create when MQTTAPI_MessageArrived is called.] */
    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_051: [IoTHubTransportMqtt shall call IoTHubClient_LL_MessageCallback.]  */
    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_053: [Otherwise it shall return true.] */
    TEST_FUNCTION(IoTHubTransportMqtt_messageArrived_happypath_return_true)
    {
        // arrange
        CIoTHubTransportMqttMocks mocks;
        auto handle = IoTHubTransportMqtt_Create(&TEST_CONFIG);
        IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);
        mocks.ResetAllCalls();

        MQTTAPI_Message testMessage;
        testMessage.payload = buffer1;
        testMessage.payloadlen = buffer1_size;

        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_CreateFromByteArray(buffer1, buffer1_size));

        STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_MessageCallback(TEST_IOTHUB_CLIENT_LL_HANDLE, IGNORED_PTR_ARG))
            .IgnoreArgument(2);

        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Destroy(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        // act
        auto result = testMessageArrivedCallback(contextMessageArrived, &testMessage);		

        // assert
        ASSERT_IS_TRUE(result);

        mocks.AssertActualAndExpectedCalls();

        //cleanup
        IoTHubTransportMqtt_Destroy(handle);
    }

    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_048: [If the call to IoTHubMessage_CreateFromByteArray fails, IoTHubTransportMqtt – message arrived shall return false.]  */
    TEST_FUNCTION(IoTHubTransportMqtt_messageArrived_CreateIotHubMessageFail_return_false)
    {
        // arrange
        CIoTHubTransportMqttMocks mocks;
        auto handle = IoTHubTransportMqtt_Create(&TEST_CONFIG);

        mocks.ResetAllCalls();

        MQTTAPI_Message testMessage;
        testMessage.payload = buffer1;
        testMessage.payloadlen = buffer1_size;

        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_CreateFromByteArray(buffer1, buffer1_size))
            .SetReturn((IOTHUB_MESSAGE_HANDLE)NULL);

        // act
        auto result = testMessageArrivedCallback(contextMessageArrived, &testMessage);

        // assert
        ASSERT_IS_FALSE(result);

        mocks.AssertActualAndExpectedCalls();

        //cleanup
        IoTHubTransportMqtt_Destroy(handle);
    }
    
    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_052: [If this calls fails, it shall return false] */
    TEST_FUNCTION(IoTHubTransportMqtt_messageArrived_messagecallback_ABANDONED_return_false)
    {
        // arrange
        CIoTHubTransportMqttMocks mocks;
        auto handle = IoTHubTransportMqtt_Create(&TEST_CONFIG);
        IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);
        mocks.ResetAllCalls();

        MQTTAPI_Message testMessage;
        testMessage.payload = buffer1;
        testMessage.payloadlen = buffer1_size;

        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_CreateFromByteArray(buffer1, buffer1_size));

        STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_MessageCallback(TEST_IOTHUB_CLIENT_LL_HANDLE, IGNORED_PTR_ARG))
            .IgnoreArgument(2)
            .SetReturn((IOTHUBMESSAGE_DISPOSITION_RESULT)IOTHUBMESSAGE_ABANDONED);

        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Destroy(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        // act
        auto result = testMessageArrivedCallback(contextMessageArrived, &testMessage);

        // assert
        ASSERT_IS_FALSE(result);

        mocks.AssertActualAndExpectedCalls();

        //cleanup
        IoTHubTransportMqtt_Destroy(handle);
    }

    /* Tests_SRS_IOTHUBTRANSPORTMQTT_04_052: [If this calls fails, it shall return false] */
    TEST_FUNCTION(IoTHubTransportMqtt_messageArrived_messagecallback_REJECTED_return_false)
    {
        // arrange
        CIoTHubTransportMqttMocks mocks;
        auto handle = IoTHubTransportMqtt_Create(&TEST_CONFIG);
        IoTHubTransportMqtt_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);
        mocks.ResetAllCalls();

        MQTTAPI_Message testMessage;
        testMessage.payload = buffer1;
        testMessage.payloadlen = buffer1_size;

        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_CreateFromByteArray(buffer1, buffer1_size));

        STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_MessageCallback(TEST_IOTHUB_CLIENT_LL_HANDLE, IGNORED_PTR_ARG))
            .IgnoreArgument(2)
            .SetReturn((IOTHUBMESSAGE_DISPOSITION_RESULT)IOTHUBMESSAGE_REJECTED);

        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Destroy(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        // act
        auto result = testMessageArrivedCallback(contextMessageArrived, &testMessage);

        // assert
        ASSERT_IS_FALSE(result);

        mocks.AssertActualAndExpectedCalls();

        //cleanup
        IoTHubTransportMqtt_Destroy(handle);
    }
END_TEST_SUITE(iothubtransportmqtt)

