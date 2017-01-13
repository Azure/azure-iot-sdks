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

#include "macro_utils.h"
#undef DEFINE_ENUM
#define DEFINE_ENUM(enumName, ...) typedef enum C2(enumName, _TAG) { FOR_EACH_1(DEFINE_ENUMERATION_CONSTANT, __VA_ARGS__)} enumName; 

#include "iothubtransporthttp.h"
#include "iothub_client_options.h"
#include "iothub_client_version.h"
#include "iothub_client_private.h"

#include "azure_c_shared_utility/urlencode.h"
#include "azure_c_shared_utility/doublylinkedlist.h"
#include "azure_c_shared_utility/httpheaders.h"
#include "azure_c_shared_utility/strings.h"
#include "azure_c_shared_utility/buffer_.h"
#include "azure_c_shared_utility/httpapiex.h"
#include "azure_c_shared_utility/httpapiexsas.h"
#include "azure_c_shared_utility/base64.h"
#include "azure_c_shared_utility/vector.h"
#include "azure_c_shared_utility/lock.h"

#define IOTHUB_ACK "iothub-ack"
#define IOTHUB_ACK_NONE "none"
#define IOTHUB_ACK_FULL "full"
#define IOTHUB_ACK_POSITIVE "positive"
#define IOTHUB_ACK_NEGATIVE "negative"
static const char* TEST_MESSAGE_ID = "FC365F0A-D432-4AB9-8307-59C0EB8F0447";

#define GBALLOC_H
extern "C" int gballoc_init(void);
extern "C" void gballoc_deinit(void);
extern "C" void* gballoc_malloc(size_t size);
extern "C" void* gballoc_calloc(size_t nmemb, size_t size);
extern "C" void* gballoc_realloc(void* ptr, size_t size);
extern "C" void gballoc_free(void* ptr);

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
#include "base64.c"
#include "strings.c"
#include "buffer.c"
#include "vector.c"
};

class RefCountObject
{
private:
    size_t ref_count;

public:
    RefCountObject() : ref_count(1)
    {
    }

    size_t inc_ref()
    {
        return ++ref_count;
    }

    void dec_ref()
    {
        if (--ref_count == 0)
        {
            delete this;
        }
    }
};

static MICROMOCK_MUTEX_HANDLE g_testByTest;
static MICROMOCK_GLOBAL_SEMAPHORE_HANDLE g_dllByDll;

#define TEST_DEVICE_ID "thisIsDeviceID"
#define TEST_DEVICE_ID2 "aSecondDeviceID"
#define TEST_DEVICE_KEY "thisIsDeviceKey"
#define TEST_DEVICE_KEY2 "aSecondDeviceKey"
#define TEST_DEVICE_TOKEN "thisIsDeviceSasToken"
#define TEST_IOTHUB_NAME "thisIsIotBuhName"
#define TEST_IOTHUB_SUFFIX "thisIsIotHubSuffix"
#define TEST_IOTHUB_GWHOSTNAME "thisIsGWHostname"
#define TEST_BLANK_SAS_TOKEN " "
#define TEST_IOTHUB_CLIENT_LL_HANDLE (IOTHUB_CLIENT_LL_HANDLE)0x34333
#define TEST_IOTHUB_CLIENT_LL_HANDLE2 (IOTHUB_CLIENT_LL_HANDLE)0x34344
#define TEST_ETAG_VALUE_UNQUOTED "thisIsSomeETAGValueSomeGUIDMaybe"
#define TEST_ETAG_VALUE TOSTRING(TEST_ETAG_VALUE_UNQUOTED)
#define TEST_PROPERTY_A_NAME "a"
#define TEST_PROPERTY_A_VALUE "value_of_a"

#define TEST_HTTPAPIEX_HANDLE (HTTPAPIEX_HANDLE)0x343

static const bool thisIsTrue = true;
static const bool thisIsFalse = false;
#define ENABLE_BATCHING() do{(void)IoTHubTransportHttp_SetOption(handle, "Batching", &thisIsTrue);} while(BASEIMPLEMENTATION::gballocState-BASEIMPLEMENTATION::gballocState)
#define DISABLE_BATCHING() do{(void)IoTHubTransportHttp_SetOption(handle, "Batching", &thisIsFalse);} while(BASEIMPLEMENTATION::gballocState-BASEIMPLEMENTATION::gballocState)

static unsigned char contains3[1] = { '3' };

static const IOTHUB_DEVICE_CONFIG TEST_DEVICE_1 =
{
    TEST_DEVICE_ID,
    TEST_DEVICE_KEY,
    NULL
};

static const IOTHUB_DEVICE_CONFIG TEST_DEVICE_2 =
{
    TEST_DEVICE_ID2,
    TEST_DEVICE_KEY2,
    NULL
};

static const IOTHUB_DEVICE_CONFIG TEST_DEVICE_3 =
{
    TEST_DEVICE_ID,
    NULL,
    TEST_DEVICE_TOKEN
};

static const IOTHUB_CLIENT_CONFIG TEST_CONFIG_IOTHUBCLIENT_CONFIG =
{
    HTTP_Protocol,                                  /* IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol;   */
    TEST_DEVICE_ID,                                 /* const char* deviceId;                        */
    TEST_DEVICE_KEY,                                /* const char* deviceKey;                       */
    NULL,                                           /* const char* deviceSasToken;                  */
    TEST_IOTHUB_NAME,                               /* const char* iotHubName;                      */
    TEST_IOTHUB_SUFFIX,                              /* const char* iotHubSuffix;                    */
    NULL                                       /* const char* protocolGatewayHostName;                  */
};

static DLIST_ENTRY waitingToSend;

static IOTHUBTRANSPORT_CONFIG TEST_CONFIG =
{
    &TEST_CONFIG_IOTHUBCLIENT_CONFIG,
    &waitingToSend
};

static const IOTHUB_CLIENT_CONFIG TEST_CONFIG_IOTHUBCLIENT_GW_CONFIG =
{
    HTTP_Protocol,                                  /* IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol;   */
    TEST_DEVICE_ID,                                 /* const char* deviceId;                        */
    TEST_DEVICE_KEY,                                /* const char* deviceKey;                       */
    NULL,                                           /* const char* deviceSasToken;                  */
    TEST_IOTHUB_NAME,                               /* const char* iotHubName;                      */
    TEST_IOTHUB_SUFFIX,                              /* const char* iotHubSuffix;                    */
    TEST_IOTHUB_GWHOSTNAME	                    /* const char* protocolGatewayHostName;                  */
};

static IOTHUBTRANSPORT_CONFIG TEST_GW_CONFIG =
{
    &TEST_CONFIG_IOTHUBCLIENT_GW_CONFIG,
    &waitingToSend
};

static const IOTHUB_CLIENT_CONFIG TEST_CONFIG_IOTHUBCLIENT_CONFIG2 =
{
    HTTP_Protocol,                                  /* IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol;   */
    TEST_DEVICE_ID2,                                 /* const char* deviceId;                        */
    TEST_DEVICE_KEY2,                                /* const char* deviceKey;                       */
    NULL,                                           /* const char* deviceSasToken;                  */
    TEST_IOTHUB_NAME,                               /* const char* iotHubName;                      */
    TEST_IOTHUB_SUFFIX,                              /* const char* iotHubSuffix;                    */
    NULL                                       /* const char* protocolGatewayHostName;                  */
};

static DLIST_ENTRY waitingToSend2;

static IOTHUBTRANSPORT_CONFIG TEST_CONFIG2 =
{
    &TEST_CONFIG_IOTHUBCLIENT_CONFIG2,
    &waitingToSend2
};

static IOTHUBTRANSPORT_CONFIG TEST_CONFIG_NULL_CONFIG =
{
    NULL,
    (PDLIST_ENTRY)0x1
};

static const IOTHUB_CLIENT_CONFIG TEST_CONFIG_IOTHUBCLIENT_CONFIG_NULL_PROTOCOL =
{
    NULL,                                       /*IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol;  */
    TEST_DEVICE_ID,                             /*const char* deviceId;                       */
    TEST_DEVICE_KEY,                            /*const char* deviceKey;                      */
    NULL,                                       /* const char* deviceSasToken;                  */
    TEST_IOTHUB_NAME,                           /*const char* iotHubName;                     */
    TEST_IOTHUB_SUFFIX,                          /* const char* iotHubSuffix;                    */
    NULL                                       /* const char* protocolGatewayHostName;                  */
};

static IOTHUBTRANSPORT_CONFIG TEST_CONFIG_NULL_PROTOCOL =
{
    &TEST_CONFIG_IOTHUBCLIENT_CONFIG_NULL_PROTOCOL,
    (PDLIST_ENTRY)0x1
};

static const IOTHUB_CLIENT_CONFIG TEST_CONFIG_IOTHUBCLIENT_CONFIG_NULL_IOTHUB_NAME =
{
    HTTP_Protocol,                              /*IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol;  */
    TEST_DEVICE_ID,                             /*const char* deviceId;                       */
    TEST_DEVICE_KEY,                            /*const char* deviceKey;                      */
    NULL,                                       /* const char* deviceSasToken;                  */
    NULL,                                       /*const char* iotHubName;                     */
    TEST_IOTHUB_SUFFIX,                          /* const char* iotHubSuffix;                    */
    NULL                                       /* const char* protocolGatewayHostName;                  */
};

static const IOTHUB_CLIENT_CONFIG TEST_CONFIG_IOTHUBCLIENT_CONFIG_NULL_IOTHUB_SUFFIX =
{
    HTTP_Protocol,                                  /* IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol;   */
    TEST_DEVICE_ID,                                 /* const char* deviceId;                        */
    TEST_DEVICE_KEY,                                /* const char* deviceKey;                       */
    NULL,                                           /* const char* deviceSasToken;                  */
    TEST_IOTHUB_NAME,                               /* const char* iotHubName;                      */
    NULL,                                            /* const char* iotHubSuffix;                    */
    NULL                                       /* const char* protocolGatewayHostName;                  */
};

static IOTHUBTRANSPORT_CONFIG TEST_CONFIG_NULL_IOTHUB_NAME =
{
    &TEST_CONFIG_IOTHUBCLIENT_CONFIG_NULL_IOTHUB_NAME,
    (PDLIST_ENTRY)0x1
};

static IOTHUBTRANSPORT_CONFIG TEST_CONFIG_NULL_IOTHUB_SUFFIX =
{
    &TEST_CONFIG_IOTHUBCLIENT_CONFIG_NULL_IOTHUB_SUFFIX,
    (PDLIST_ENTRY)0x1
};

#define TEST_IOTHUB_MESSAGE_HANDLE_1 ((IOTHUB_MESSAGE_HANDLE)0x01d1)
#define TEST_IOTHUB_MESSAGE_HANDLE_2 ((IOTHUB_MESSAGE_HANDLE)0x01d2)
#define TEST_IOTHUB_MESSAGE_HANDLE_3 ((IOTHUB_MESSAGE_HANDLE)0x01d3)
#define TEST_IOTHUB_MESSAGE_HANDLE_4 ((IOTHUB_MESSAGE_HANDLE)0x01d4)
#define TEST_IOTHUB_MESSAGE_HANDLE_5 ((IOTHUB_MESSAGE_HANDLE)0x01d5)
#define TEST_IOTHUB_MESSAGE_HANDLE_6 ((IOTHUB_MESSAGE_HANDLE)0x01d6)
#define TEST_IOTHUB_MESSAGE_HANDLE_7 ((IOTHUB_MESSAGE_HANDLE)0x01d7)
#define TEST_IOTHUB_MESSAGE_HANDLE_8 ((IOTHUB_MESSAGE_HANDLE)0x01d8)
#define TEST_IOTHUB_MESSAGE_HANDLE_9 ((IOTHUB_MESSAGE_HANDLE)0x01d9)
#define TEST_IOTHUB_MESSAGE_HANDLE_10 ((IOTHUB_MESSAGE_HANDLE)0x01da)
#define TEST_IOTHUB_MESSAGE_HANDLE_11 ((IOTHUB_MESSAGE_HANDLE)0x01db)
#define TEST_IOTHUB_MESSAGE_HANDLE_12 ((IOTHUB_MESSAGE_HANDLE)0x01dc)

static IOTHUB_MESSAGE_LIST message1 =  /*this is the oldest message, always the first to be processed, send etc*/
{
    TEST_IOTHUB_MESSAGE_HANDLE_1,                    /*IOTHUB_MESSAGE_HANDLE messageHandle;                        */
    NULL,                                           /*IOTHUB_CLIENT_EVENT_CONFIRMATION_CALLBACK callback;     */
    NULL,                                           /*void* context;                                              */
    { NULL, NULL }                                  /*DLIST_ENTRY entry;                                          */
};

static IOTHUB_MESSAGE_LIST message2 = /*this is the message next to the oldest message, it is processed after the oldest message*/
{
    TEST_IOTHUB_MESSAGE_HANDLE_2,                  /*IOTHUB_MESSAGE_HANDLE messageHandle;                        */
    NULL,                                           /*IOTHUB_CLIENT_EVENT_CONFIRMATION_CALLBACK callback;     */
    NULL,                                           /*void* context;                                              */
    { NULL, NULL }                                  /*DLIST_ENTRY entry;                                          */
};

static IOTHUB_MESSAGE_LIST message3 = /*this is the newest message, always last to be processed*/
{
    TEST_IOTHUB_MESSAGE_HANDLE_3,                  /*IOTHUB_MESSAGE_HANDLE messageHandle;                        */
    NULL,                                           /*IOTHUB_CLIENT_EVENT_CONFIRMATION_CALLBACK callback;     */
    NULL,                                           /*void* context;                                              */
    { NULL, NULL }                                  /*DLIST_ENTRY entry;                                          */
};

static IOTHUB_MESSAGE_LIST message4 = /*this is outof bounds message (>256K)*/
{
    TEST_IOTHUB_MESSAGE_HANDLE_4,                  /*IOTHUB_MESSAGE_HANDLE messageHandle;                        */
    NULL,                                           /*IOTHUB_CLIENT_EVENT_CONFIRMATION_CALLBACK callback;     */
    NULL,                                           /*void* context;                                              */
    { NULL, NULL }                                  /*DLIST_ENTRY entry;                                          */
};

static IOTHUB_MESSAGE_LIST message5 = /*this is almost out of bounds message (<256K)*/
{
    TEST_IOTHUB_MESSAGE_HANDLE_5,                  /*IOTHUB_MESSAGE_HANDLE messageHandle;                        */
    NULL,                                           /*IOTHUB_CLIENT_EVENT_CONFIRMATION_CALLBACK callback;     */
    NULL,                                           /*void* context;                                              */
    { NULL, NULL }                                  /*DLIST_ENTRY entry;                                          */
};

static IOTHUB_MESSAGE_LIST message6 = /*this has properties (1)*/
{
    TEST_IOTHUB_MESSAGE_HANDLE_6,                  /*IOTHUB_MESSAGE_HANDLE messageHandle;                        */
    NULL,                                           /*IOTHUB_CLIENT_EVENT_CONFIRMATION_CALLBACK callback;     */
    NULL,                                           /*void* context;                                              */
    { NULL, NULL }                                  /*DLIST_ENTRY entry;                                          */
};

static IOTHUB_MESSAGE_LIST message7 = /*this has properties (2)*/
{
    TEST_IOTHUB_MESSAGE_HANDLE_7,                  /*IOTHUB_MESSAGE_HANDLE messageHandle;                        */
    NULL,                                           /*IOTHUB_CLIENT_EVENT_CONFIRMATION_CALLBACK callback;     */
    NULL,                                           /*void* context;                                              */
    { NULL, NULL }                                  /*DLIST_ENTRY entry;                                          */
};

static IOTHUB_MESSAGE_LIST message9 = /*this has 256*1024 bytes - must fail @ send unbatched*/
{
    TEST_IOTHUB_MESSAGE_HANDLE_9,                  /*IOTHUB_MESSAGE_HANDLE messageHandle;                        */
    NULL,                                           /*IOTHUB_CLIENT_EVENT_CONFIRMATION_CALLBACK callback;     */
    NULL,                                           /*void* context;                                              */
    { NULL, NULL }                                  /*DLIST_ENTRY entry;                                          */
};

static IOTHUB_MESSAGE_LIST message10 = /*this is a simple string message*/
{
    TEST_IOTHUB_MESSAGE_HANDLE_10,                  /*IOTHUB_MESSAGE_HANDLE messageHandle;                        */
    NULL,                                           /*IOTHUB_CLIENT_EVENT_CONFIRMATION_CALLBACK callback;     */
    NULL,                                           /*void* context;                                              */
    { NULL, NULL }                                  /*DLIST_ENTRY entry;                                          */
};

static IOTHUB_MESSAGE_LIST message11 = /*this is message of 255*1024 bytes - 384 - 16 -2 bytes length, so that adding a property "a":"b" brings it to the maximum size*/
{
    TEST_IOTHUB_MESSAGE_HANDLE_11,                  /*IOTHUB_MESSAGE_HANDLE messageHandle;                        */
    NULL,                                           /*IOTHUB_CLIENT_EVENT_CONFIRMATION_CALLBACK callback;     */
    NULL,                                           /*void* context;                                              */
    { NULL, NULL }                                  /*DLIST_ENTRY entry;                                          */
};

static IOTHUB_MESSAGE_LIST message12 = /*this is message of 255*1024 bytes - 384 - 16 - 2 bytes length, so that adding a property "aa":"b" brings it over the maximum size*/
{
    TEST_IOTHUB_MESSAGE_HANDLE_12,                  /*IOTHUB_MESSAGE_HANDLE messageHandle;                        */
    NULL,                                           /*IOTHUB_CLIENT_EVENT_CONFIRMATION_CALLBACK callback;     */
    NULL,                                           /*void* context;                                              */
    { NULL, NULL }                                  /*DLIST_ENTRY entry;                                          */
};

#define TEST_MAP_EMPTY (MAP_HANDLE) 0xe0
#define TEST_MAP_1_PROPERTY (MAP_HANDLE) 0xe1
#define TEST_MAP_2_PROPERTY (MAP_HANDLE) 0xe2
#define TEST_MAP_3_PROPERTY (MAP_HANDLE) 0xe3
#define TEST_MAP_1_PROPERTY_A_B (MAP_HANDLE) 0xe4
#define TEST_MAP_1_PROPERTY_AA_B (MAP_HANDLE) 0xe5

#define TEST_RED_KEY "redkey"
#define TEST_RED_KEY_STRING TEST_RED_KEY
#define TEST_RED_KEY_STRING_WITH_IOTHUBAPP "\"" "iothub-app-" TEST_RED_KEY "\""

#define TEST_RED_VALUE "redvalue"
#define TEST_RED_VALUE_STRING TOSTRING(TEST_RED_VALUE)

#define TEST_BLUE_KEY "bluekey"
#define TEST_BLUE_KEY_STRING TEST_BLUE_KEY
#define TEST_BLUE_KEY_STRING_WITH_IOTHUBAPP "\"" "iothub-app-" TEST_BLUE_KEY "\""

#define TEST_BLUE_VALUE "bluevalue"
#define TEST_BLUE_VALUE_STRING TOSTRING(TEST_BLUE_VALUE)

#define TEST_YELLOW_KEY "yellowkey"
#define TEST_YELLOW_KEY_STRING TEST_YELLOW_KEY
#define TEST_YELLOW_KEY_STRING_WITH_IOTHUBAPP "\"" "iothub-app-" TEST_YELLOW_KEY "\""

#define TEST_YELLOW_VALUE "yellowvaluekey"
#define TEST_YELLOW_VALUE_STRING TOSTRING(TEST_YELLOW_VALUE)

static const char* TEST_KEYS1[] = { TEST_RED_KEY_STRING };
static const char* TEST_VALUES1[] = { TEST_RED_VALUE };

static const char* TEST_KEYS2[] = { TEST_BLUE_KEY_STRING, TEST_YELLOW_KEY_STRING };
static const char* TEST_VALUES2[] = { TEST_BLUE_VALUE,  TEST_YELLOW_VALUE };

static const char* TEST_KEYS1_A_B[] = { "a" };
static const char* TEST_VALUES1_A_B[] = { "b" };

static const char* TEST_KEYS1_AA_B[] = { "aa" };
static const char* TEST_VALUES1_AA_B[] = { "b" };

static size_t currentmalloc_call;
static size_t whenShallmalloc_fail;

/*different STRING constructors*/
static size_t currentSTRING_new_call;
static size_t whenShallSTRING_new_fail;

static size_t currentSTRING_new_JSON_call;
static size_t whenShallSTRING_new_JSON_fail;

static size_t currentSTRING_clone_call;
static size_t whenShallSTRING_clone_fail;

static size_t currentSTRING_construct_call;
static size_t whenShallSTRING_construct_fail;

static size_t currentSTRING_construct_n_call;
static size_t whenShallSTRING_construct_n_fail;


static size_t currentSTRING_concat_call;
static size_t whenShallSTRING_concat_fail;

static size_t currentSTRING_empty_call;
static size_t whenShallSTRING_empty_fail;

static size_t currentSTRING_concat_with_STRING_call;
static size_t whenShallSTRING_concat_with_STRING_fail;

static size_t currentHTTPHeaders_Alloc_call;
static size_t whenShallHTTPHeaders_Alloc_fail;

static size_t currentHTTPHeaders_Clone_call;
static size_t whenShallHTTPHeaders_Clone_fail;


static size_t currentBUFFER_new_call;
static size_t whenShallBUFFER_new_fail;

static size_t currentBUFFER_build_call;
static size_t whenShallBUFFER_build_fail;

static size_t currentBase64_Encode_Bytes_call;
static size_t whenShallBase64_Encode_Bytes_fail;

static size_t currentURL_Encode_String_call;
static size_t whenShallURL_Encode_String_fail;

static size_t whenShallHTTPHeaders_GetHeader_fail;
static size_t currentHTTPHeaders_GetHeader_call;

static size_t currentVECTOR_create_call;
static size_t whenShallVECTOR_create_fail;

static size_t currentVECTOR_push_back_call;
static size_t whenShallVECTOR_push_back_fail;

static size_t currentVECTOR_find_if_call;
static size_t whenShallVECTOR_find_if_fail;


#define MAXIMUM_MESSAGE_SIZE (255*1024-1)
#define PAYLOAD_OVERHEAD (384)
#define PROPERTY_OVERHEAD (16)


#define TEST_MINIMAL_PAYLOAD [{"body":""}]
#define TEST_MINIMAL_PAYLOAD_STRING TOSTRING(TEST_MINIMAL_PAYLOAD)

#define TEST_BIG_BUFFER_1_SIZE          (MAXIMUM_MESSAGE_SIZE - PAYLOAD_OVERHEAD)
#define TEST_BIG_BUFFER_1_OVERFLOW_SIZE (TEST_BIG_BUFFER_1_SIZE +1 )
#define TEST_BIG_BUFFER_1_FIT_SIZE      (TEST_BIG_BUFFER_1_SIZE)

#define TEST_BIG_BUFFER_9_OVERFLOW_SIZE (256*1024)

static const unsigned char buffer1[1] = { '1' };
static const size_t buffer1_size = sizeof(buffer1);

static const unsigned char buffer2[2] = { '2', '2' };
static const size_t buffer2_size = sizeof(buffer2);

static const unsigned char buffer3[3] = { '3','3','3' };
static const size_t buffer3_size = sizeof(buffer3);

static const unsigned char buffer6[6] = { '1', '2', '3', '4', '5', '6' };
static const size_t buffer6_size = sizeof(buffer6);

static const unsigned char buffer7[7] = { '1', '2', '3', '4', '5', '6', '7' };
static const size_t buffer7_size = sizeof(buffer7);

static const unsigned char* buffer9;
static const size_t buffer9_size = TEST_BIG_BUFFER_9_OVERFLOW_SIZE;

static const unsigned char* buffer11;
static const size_t buffer11_size = MAXIMUM_MESSAGE_SIZE - PAYLOAD_OVERHEAD - 2 - PROPERTY_OVERHEAD;

static unsigned char* bigBufferOverflow; /*this is a buffer that contains just enough characters to go over the limit of 256K as a single message*/
static unsigned char* bigBufferFit; /*this is a buffer that contains just enough characters to NOT go over the limit of 256K as a single message*/

static const char* string10 = "thisgoestoJ\\s//on\"ToBeEn\r\n\bcoded";

const unsigned int httpStatus200 = 200;
const unsigned int httpStatus201 = 201;
const unsigned int httpStatus204 = 204;
const unsigned int httpStatus404 = 404;

static BUFFER_HANDLE last_BUFFER_HANDLE_to_HTTPAPIEX_ExecuteRequest = NULL;

static bool HTTPHeaders_GetHeaderCount_writes_to_its_outputs = true;

#define TEST_HEADER_1 "iothub-app-NAME1: VALUE1"
#define TEST_HEADER_1_5 "not-iothub-app-NAME1: VALUE1"
#define TEST_HEADER_2 "iothub-app-NAME2: VALUE2"
#define TEST_HEADER_3 "iothub-messageid: VALUE3"
/*value returned by time() function*/
/*for the purpose of this implementation, time_t represents the number of seconds since 1970, 1st jan, 0:0:0*/
#define TEST_GET_TIME_VALUE 384739233
#define TEST_DEFAULT_GETMINIMUMPOLLINGTIME 1500


TYPED_MOCK_CLASS(CIoTHubTransportHttpMocks, CGlobalMock)
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
    MOCK_STATIC_METHOD_0(, STRING_HANDLE, STRING_new)
        STRING_HANDLE result2;
    currentSTRING_new_call++;
    if (whenShallSTRING_new_fail > 0)
    {
        if (currentSTRING_new_call == whenShallSTRING_new_fail)
        {
            result2 = (STRING_HANDLE)NULL;
        }
        else
        {
            result2 = BASEIMPLEMENTATION::STRING_new();
        }
    }
    else
    {
        result2 = BASEIMPLEMENTATION::STRING_new();
    }
    MOCK_METHOD_END(STRING_HANDLE, result2)

        MOCK_STATIC_METHOD_1(, STRING_HANDLE, STRING_new_JSON, const char*, source)
        STRING_HANDLE result2;
    currentSTRING_new_JSON_call++;
    if (whenShallSTRING_new_JSON_fail > 0)
    {
        if (currentSTRING_new_JSON_call == whenShallSTRING_new_JSON_fail)
        {
            result2 = (STRING_HANDLE)NULL;
        }
        else
        {
            result2 = BASEIMPLEMENTATION::STRING_new_JSON(source);
        }
    }
    else
    {
        result2 = BASEIMPLEMENTATION::STRING_new_JSON(source);
    }
    MOCK_METHOD_END(STRING_HANDLE, result2)

        MOCK_STATIC_METHOD_1(, STRING_HANDLE, STRING_new_with_memory, const char*, memory)
        MOCK_METHOD_END(STRING_HANDLE, BASEIMPLEMENTATION::STRING_new_with_memory(memory))

        MOCK_STATIC_METHOD_1(, STRING_HANDLE, STRING_clone, STRING_HANDLE, handle)
        STRING_HANDLE result2;
    currentSTRING_clone_call++;
    if (whenShallSTRING_clone_fail > 0)
    {
        if (currentSTRING_clone_call == whenShallSTRING_clone_fail)
        {
            result2 = (STRING_HANDLE)NULL;
        }
        else
        {
            result2 = BASEIMPLEMENTATION::STRING_clone(handle);
        }
    }
    else
    {
        result2 = BASEIMPLEMENTATION::STRING_clone(handle);
    }
    MOCK_METHOD_END(STRING_HANDLE, result2)

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

        MOCK_STATIC_METHOD_2(, STRING_HANDLE, STRING_construct_n, const char*, source, size_t, size)
        STRING_HANDLE result2;
    currentSTRING_construct_n_call++;
    if (whenShallSTRING_construct_n_fail > 0)
    {
        if (currentSTRING_construct_n_call == whenShallSTRING_construct_n_fail)
        {
            result2 = (STRING_HANDLE)NULL;
        }
        else
        {
            result2 = BASEIMPLEMENTATION::STRING_construct_n(source, size);
        }
    }
    else
    {
        result2 = BASEIMPLEMENTATION::STRING_construct_n(source, size);
    }
    MOCK_METHOD_END(STRING_HANDLE, result2)

        MOCK_STATIC_METHOD_1(, void, STRING_delete, STRING_HANDLE, s)
        BASEIMPLEMENTATION::STRING_delete(s);
    MOCK_VOID_METHOD_END()

        MOCK_STATIC_METHOD_2(, int, STRING_concat, STRING_HANDLE, s1, const char*, s2)
        currentSTRING_concat_call++;
    MOCK_METHOD_END(int, (((whenShallSTRING_concat_fail > 0) && (currentSTRING_concat_call == whenShallSTRING_concat_fail)) ? __LINE__ : BASEIMPLEMENTATION::STRING_concat(s1, s2)));

    MOCK_STATIC_METHOD_2(, int, STRING_concat_with_STRING, STRING_HANDLE, s1, STRING_HANDLE, s2)
        currentSTRING_concat_with_STRING_call++;
    MOCK_METHOD_END(int, (((currentSTRING_concat_with_STRING_call > 0) && (currentSTRING_concat_with_STRING_call == whenShallSTRING_concat_with_STRING_fail)) ? __LINE__ : BASEIMPLEMENTATION::STRING_concat_with_STRING(s1, s2)));


    MOCK_STATIC_METHOD_1(, int, STRING_empty, STRING_HANDLE, s)
        currentSTRING_empty_call++;
    MOCK_METHOD_END(int, BASEIMPLEMENTATION::STRING_empty(s));

    MOCK_STATIC_METHOD_1(, const char*, STRING_c_str, STRING_HANDLE, s)
        MOCK_METHOD_END(const char*, BASEIMPLEMENTATION::STRING_c_str(s))

        MOCK_STATIC_METHOD_1(, size_t, STRING_length, STRING_HANDLE, handle)
        MOCK_METHOD_END(size_t, BASEIMPLEMENTATION::STRING_length(handle))

        /* HTTPHeaders mocks */
        MOCK_STATIC_METHOD_0(, HTTP_HEADERS_HANDLE, HTTPHeaders_Alloc)
        HTTP_HEADERS_HANDLE result2;
    currentHTTPHeaders_Alloc_call++;
    if ((whenShallHTTPHeaders_Alloc_fail > 0) && (whenShallHTTPHeaders_Alloc_fail == currentHTTPHeaders_Alloc_call))
    {
        result2 = NULL;
    }
    else
    {
        result2 = (HTTP_HEADERS_HANDLE)malloc(1);
    }
    MOCK_METHOD_END(HTTP_HEADERS_HANDLE, result2)

        MOCK_STATIC_METHOD_1(, HTTP_HEADERS_HANDLE, HTTPHeaders_Clone, HTTP_HEADERS_HANDLE, handle)
        HTTP_HEADERS_HANDLE result2;
    currentHTTPHeaders_Clone_call++;
    if ((whenShallHTTPHeaders_Clone_fail > 0) && (whenShallHTTPHeaders_Clone_fail == currentHTTPHeaders_Clone_call))
    {
        result2 = NULL;
    }
    else
    {
        result2 = (HTTP_HEADERS_HANDLE)malloc(1);
    }
    MOCK_METHOD_END(HTTP_HEADERS_HANDLE, result2)

        MOCK_STATIC_METHOD_1(, void, HTTPHeaders_Free, HTTP_HEADERS_HANDLE, httpHeadersHandle)
        free(httpHeadersHandle);
    MOCK_VOID_METHOD_END()

        MOCK_STATIC_METHOD_3(, HTTP_HEADERS_RESULT, HTTPHeaders_AddHeaderNameValuePair, HTTP_HEADERS_HANDLE, httpHeadersHandle, const char*, name, const char*, value)
        MOCK_METHOD_END(HTTP_HEADERS_RESULT, HTTP_HEADERS_OK)

        MOCK_STATIC_METHOD_3(, HTTP_HEADERS_RESULT, HTTPHeaders_ReplaceHeaderNameValuePair, HTTP_HEADERS_HANDLE, httpHeadersHandle, const char*, name, const char*, value)
        MOCK_METHOD_END(HTTP_HEADERS_RESULT, HTTP_HEADERS_OK)

        MOCK_STATIC_METHOD_2(, const char*, HTTPHeaders_FindHeaderValue, HTTP_HEADERS_HANDLE, httpHeadersHandle, const char*, name)
        MOCK_METHOD_END(const char*, NULL)

        MOCK_STATIC_METHOD_1(, HTTPAPIEX_HANDLE, HTTPAPIEX_Create, const char*, hostName)
        MOCK_METHOD_END(HTTPAPIEX_HANDLE, TEST_HTTPAPIEX_HANDLE)

        MOCK_STATIC_METHOD_3(, HTTPAPIEX_RESULT, HTTPAPIEX_SetOption, HTTPAPIEX_HANDLE, handle, const char*, optionName, const void*, value)
        MOCK_METHOD_END(HTTPAPIEX_RESULT, HTTPAPIEX_OK)

        MOCK_STATIC_METHOD_1(, void, HTTPAPIEX_Destroy, HTTPAPIEX_HANDLE, handle)
        MOCK_VOID_METHOD_END()

        /* IoTHubMessage mocks */
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
        case ((uintptr_t)TEST_IOTHUB_MESSAGE_HANDLE_2) :
        {
            *buffer = buffer2;
            *size = buffer2_size;
            break;
        }
        case ((uintptr_t)TEST_IOTHUB_MESSAGE_HANDLE_3) :
        {
            *buffer = buffer3;
            *size = buffer3_size;
            break;
        }
        case ((uintptr_t)TEST_IOTHUB_MESSAGE_HANDLE_4) : /*this is out of bounds message (>256K)*/
        {
            *buffer = bigBufferOverflow;
            *size = TEST_BIG_BUFFER_1_OVERFLOW_SIZE;
            break;
        }
        case ((uintptr_t)TEST_IOTHUB_MESSAGE_HANDLE_5) : /*this is a message that just fits*/
        {
            *buffer = bigBufferFit;
            *size = TEST_BIG_BUFFER_1_FIT_SIZE;
            break;
        }
        case ((uintptr_t)TEST_IOTHUB_MESSAGE_HANDLE_6) : /*this is a message that just fits*/
        {
            *buffer = buffer6;
            *size = buffer6_size;
            break;
        }
        case ((uintptr_t)TEST_IOTHUB_MESSAGE_HANDLE_7) : /*this is a message that just fits*/
        {
            *buffer = buffer7;
            *size = buffer7_size;
            break;
        }
        case ((uintptr_t)TEST_IOTHUB_MESSAGE_HANDLE_9) : /*this is a message that is +1 byte over the unbtached send limit*/
        {
            *buffer = buffer9;
            *size = buffer9_size;
            break;
        }
        case ((uintptr_t)TEST_IOTHUB_MESSAGE_HANDLE_11) : /*this is a message that has a property and together with that property fit at maximum*/
        {
            *buffer = buffer11;
            *size = buffer11_size;
            break;
        }
        case ((uintptr_t)TEST_IOTHUB_MESSAGE_HANDLE_12) : /*this is a message that has a property and together with that property does NOT fit at maximum*/
        {
            *buffer = buffer11; /*this is not a copy&paste mistake, it is intended to use the same "to the limit" buffer as 11*/
            *size = buffer11_size;
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

        MOCK_STATIC_METHOD_1(, const char*, IoTHubMessage_GetString, IOTHUB_MESSAGE_HANDLE, handle)

        const char* result2;
    switch ((uintptr_t)handle)
    {
    case ((uintptr_t)TEST_IOTHUB_MESSAGE_HANDLE_1) :
    case ((uintptr_t)TEST_IOTHUB_MESSAGE_HANDLE_2) :
    case ((uintptr_t)TEST_IOTHUB_MESSAGE_HANDLE_3) :
    case ((uintptr_t)TEST_IOTHUB_MESSAGE_HANDLE_4) :
    case ((uintptr_t)TEST_IOTHUB_MESSAGE_HANDLE_5) :
    case ((uintptr_t)TEST_IOTHUB_MESSAGE_HANDLE_6) :
    case ((uintptr_t)TEST_IOTHUB_MESSAGE_HANDLE_7) :
    case ((uintptr_t)TEST_IOTHUB_MESSAGE_HANDLE_8) :
    case ((uintptr_t)TEST_IOTHUB_MESSAGE_HANDLE_9) :
    case ((uintptr_t)TEST_IOTHUB_MESSAGE_HANDLE_11) :
    case ((uintptr_t)TEST_IOTHUB_MESSAGE_HANDLE_12) :
    {
        result2 = NULL;
        break;
    }
    case ((uintptr_t)TEST_IOTHUB_MESSAGE_HANDLE_10) :
    {
        result2 = string10;
        break;
    }
    default:
    {
        /*not expected really*/
        result2 = NULL;
    }
    }
    MOCK_METHOD_END(const char*, result2)

    MOCK_STATIC_METHOD_1(, MAP_HANDLE, IoTHubMessage_Properties, IOTHUB_MESSAGE_HANDLE, iotHubMessageHandle)
        MAP_HANDLE result2;
    switch ((uintptr_t)iotHubMessageHandle)
    {
    case ((uintptr_t)TEST_IOTHUB_MESSAGE_HANDLE_1) :
    {
        result2 = TEST_MAP_EMPTY;
        break;
    }
    case ((uintptr_t)TEST_IOTHUB_MESSAGE_HANDLE_2) :
    {
        result2 = TEST_MAP_EMPTY;
        break;
    }
    case ((uintptr_t)TEST_IOTHUB_MESSAGE_HANDLE_3) :
    {
        result2 = TEST_MAP_EMPTY;
        break;
    }
    case ((uintptr_t)TEST_IOTHUB_MESSAGE_HANDLE_4) : /*this is out of bounds message (>256K)*/
    {
        result2 = TEST_MAP_EMPTY;
        break;
    }
    case ((uintptr_t)TEST_IOTHUB_MESSAGE_HANDLE_5) : /*this is a message that just fits*/
    {
        result2 = TEST_MAP_EMPTY;
        break;
    }
    case ((uintptr_t)TEST_IOTHUB_MESSAGE_HANDLE_6) :
    {
        result2 = TEST_MAP_1_PROPERTY;
        break;
    }
    case ((uintptr_t)TEST_IOTHUB_MESSAGE_HANDLE_7) :
    {
        result2 = TEST_MAP_2_PROPERTY;
        break;
    }
    case ((uintptr_t)TEST_IOTHUB_MESSAGE_HANDLE_8) :
    {
        result2 = TEST_MAP_3_PROPERTY;
        break;
    }
    case ((uintptr_t)TEST_IOTHUB_MESSAGE_HANDLE_9) :
    {
        result2 = TEST_MAP_EMPTY;
        break;
    }
    case ((uintptr_t)TEST_IOTHUB_MESSAGE_HANDLE_10) :
    {
        result2 = TEST_MAP_EMPTY;
        break;
    }
    case ((uintptr_t)TEST_IOTHUB_MESSAGE_HANDLE_11) :
    {
        result2 = TEST_MAP_1_PROPERTY_A_B;
        break;
    }
    case ((uintptr_t)TEST_IOTHUB_MESSAGE_HANDLE_12) :
    {
        result2 = TEST_MAP_1_PROPERTY_AA_B;
        break;
    }
    default:
    {
        /*not expected really*/
        result2 = NULL;
        ASSERT_FAIL("not expected");
    }
    }
    MOCK_METHOD_END(MAP_HANDLE, result2)

        MOCK_STATIC_METHOD_2(, IOTHUB_MESSAGE_RESULT, IoTHubMessage_SetMessageId, IOTHUB_MESSAGE_HANDLE, iotHubMessageHandle, const char*, messageId)
        MOCK_METHOD_END(IOTHUB_MESSAGE_RESULT, IOTHUB_MESSAGE_OK)

        MOCK_STATIC_METHOD_1(, const char*, IoTHubMessage_GetMessageId, IOTHUB_MESSAGE_HANDLE, iotHubMessageHandle)
        MOCK_METHOD_END(const char*, NULL)

        MOCK_STATIC_METHOD_2(, IOTHUB_MESSAGE_RESULT, IoTHubMessage_SetCorrelationId, IOTHUB_MESSAGE_HANDLE, iotHubMessageHandle, const char*, messageId)
        MOCK_METHOD_END(IOTHUB_MESSAGE_RESULT, IOTHUB_MESSAGE_OK)

        MOCK_STATIC_METHOD_1(, const char*, IoTHubMessage_GetCorrelationId, IOTHUB_MESSAGE_HANDLE, iotHubMessageHandle)
        MOCK_METHOD_END(const char*, NULL)

        MOCK_STATIC_METHOD_1(, IOTHUBMESSAGE_CONTENT_TYPE, IoTHubMessage_GetContentType, IOTHUB_MESSAGE_HANDLE, iotHubMessageHandle)
        IOTHUBMESSAGE_CONTENT_TYPE result2;
    switch ((uintptr_t)iotHubMessageHandle)
    {
    case ((uintptr_t)TEST_IOTHUB_MESSAGE_HANDLE_1) :
    case ((uintptr_t)TEST_IOTHUB_MESSAGE_HANDLE_2) :
    case ((uintptr_t)TEST_IOTHUB_MESSAGE_HANDLE_3) :
    case ((uintptr_t)TEST_IOTHUB_MESSAGE_HANDLE_4) :
    case ((uintptr_t)TEST_IOTHUB_MESSAGE_HANDLE_5) :
    case ((uintptr_t)TEST_IOTHUB_MESSAGE_HANDLE_6) :
    case ((uintptr_t)TEST_IOTHUB_MESSAGE_HANDLE_7) :
    case ((uintptr_t)TEST_IOTHUB_MESSAGE_HANDLE_8) :
    case ((uintptr_t)TEST_IOTHUB_MESSAGE_HANDLE_9) :
    case ((uintptr_t)TEST_IOTHUB_MESSAGE_HANDLE_11) :
    case ((uintptr_t)TEST_IOTHUB_MESSAGE_HANDLE_12) :
    {
        result2 = IOTHUBMESSAGE_BYTEARRAY;
        break;
    }
    case ((uintptr_t)TEST_IOTHUB_MESSAGE_HANDLE_10) :
    {
        result2 = IOTHUBMESSAGE_STRING;
        break;
    }
    default:
        result2 = IOTHUBMESSAGE_STRING;
        break;
    }
    MOCK_METHOD_END(IOTHUBMESSAGE_CONTENT_TYPE, result2)

    MOCK_STATIC_METHOD_1(, void, IoTHubMessage_Destroy, IOTHUB_MESSAGE_HANDLE, iotHubMessageHandle)
    MOCK_VOID_METHOD_END()

    MOCK_STATIC_METHOD_2(, IOTHUBMESSAGE_DISPOSITION_RESULT, IoTHubClient_LL_MessageCallback, IOTHUB_CLIENT_LL_HANDLE, handle, IOTHUB_MESSAGE_HANDLE, message)
    MOCK_METHOD_END(IOTHUBMESSAGE_DISPOSITION_RESULT, IOTHUBMESSAGE_ACCEPTED)

    MOCK_STATIC_METHOD_3(, void, IoTHubClient_LL_SendComplete, IOTHUB_CLIENT_LL_HANDLE, handle, PDLIST_ENTRY, completed, IOTHUB_CLIENT_CONFIRMATION_RESULT, result2)
    MOCK_VOID_METHOD_END()

    /*buffer*/
    /* BUFFER Mocks */
    MOCK_STATIC_METHOD_0(, BUFFER_HANDLE, BUFFER_new)
        BUFFER_HANDLE result2;
        currentBUFFER_new_call++;
        if (whenShallBUFFER_new_fail > 0)
        {
            if (currentBUFFER_new_call == whenShallBUFFER_new_fail)
            {
                result2 = (BUFFER_HANDLE)NULL;
            }
            else
            {
                result2 = BASEIMPLEMENTATION::BUFFER_new();
            }
        }
        else
        {
            result2 = BASEIMPLEMENTATION::BUFFER_new();
        }
    MOCK_METHOD_END(BUFFER_HANDLE, result2)

    MOCK_STATIC_METHOD_1(, void, BUFFER_delete, BUFFER_HANDLE, s)
        BASEIMPLEMENTATION::BUFFER_delete(s);
    MOCK_VOID_METHOD_END()


    MOCK_STATIC_METHOD_3(, int, BUFFER_build, BUFFER_HANDLE, handle, const unsigned char*, source, size_t, size)
    MOCK_METHOD_END(int, BASEIMPLEMENTATION::BUFFER_build(handle, source, size))

    MOCK_STATIC_METHOD_1(, unsigned char*, BUFFER_u_char, BUFFER_HANDLE, handle);
        unsigned char* result2;
        result2 = BASEIMPLEMENTATION::BUFFER_u_char(handle);
    MOCK_METHOD_END(unsigned char*, result2)

    MOCK_STATIC_METHOD_1(, size_t, BUFFER_length, BUFFER_HANDLE, handle);
        size_t result2;
        result2 = BASEIMPLEMENTATION::BUFFER_length(handle);
    MOCK_METHOD_END(size_t, result2)

    MOCK_STATIC_METHOD_2(, int, BUFFER_pre_build, BUFFER_HANDLE, handle, size_t, size);
    MOCK_METHOD_END(int, BASEIMPLEMENTATION::BUFFER_pre_build(handle, size))

    MOCK_STATIC_METHOD_2(, int, BUFFER_content, BUFFER_HANDLE, handle, const unsigned char**, content);
    MOCK_METHOD_END(int, BASEIMPLEMENTATION::BUFFER_content(handle, content))

    MOCK_STATIC_METHOD_2(, int, BUFFER_size, BUFFER_HANDLE, handle, size_t*, size);
    MOCK_METHOD_END(int, BASEIMPLEMENTATION::BUFFER_size(handle, size))

    MOCK_STATIC_METHOD_2(, STRING_HANDLE, Base64_Encode_Bytes, const unsigned char*, source, size_t, size);
        currentBase64_Encode_Bytes_call++;
    MOCK_METHOD_END(STRING_HANDLE, (((currentBase64_Encode_Bytes_call > 0) && (currentBase64_Encode_Bytes_call == whenShallBase64_Encode_Bytes_fail)) ? ((STRING_HANDLE)NULL) : BASEIMPLEMENTATION::Base64_Encode_Bytes(source, size)));

    MOCK_STATIC_METHOD_4(, MAP_RESULT, Map_GetInternals, MAP_HANDLE, handle, const char*const**, keys, const char*const**, values, size_t*, count)
        switch ((uintptr_t)handle)
        {
        case((uintptr_t)TEST_MAP_EMPTY) :
        {
            *keys = NULL;
            *values = NULL;
            *count = 0;
            break;
        }
        case((uintptr_t)TEST_MAP_1_PROPERTY) :
        {
            *keys = (const char*const*)TEST_KEYS1;
            *values = (const char*const*)TEST_VALUES1;
            *count = 1;
            break;
        }
        case((uintptr_t)TEST_MAP_2_PROPERTY) :
        {
            *keys = (const char*const*)TEST_KEYS2;
            *values = (const char*const*)TEST_VALUES2;
            *count = 2;
            break;
        }
        case((uintptr_t)TEST_MAP_1_PROPERTY_A_B) :
        {
            *keys = (const char*const*)TEST_KEYS1_A_B;
            *values = (const char*const*)TEST_VALUES1_A_B;
            *count = 1;
            break;
        }
        case((uintptr_t)TEST_MAP_1_PROPERTY_AA_B) :
        {
            *keys = (const char*const*)TEST_KEYS1_AA_B;
            *values = (const char*const*)TEST_VALUES1_AA_B;
            *count = 1;
            break;
        }
        default:
        {
            ASSERT_FAIL("unexpected value");
        }
        }
    MOCK_METHOD_END(MAP_RESULT, MAP_OK);

    MOCK_STATIC_METHOD_3(, MAP_RESULT, Map_AddOrUpdate, MAP_HANDLE, handle, const char*, key, const char*, value)
        MOCK_METHOD_END(MAP_RESULT, MAP_OK)

        MOCK_STATIC_METHOD_2(, HTTP_HEADERS_RESULT, HTTPHeaders_GetHeaderCount, HTTP_HEADERS_HANDLE, httpHeadersHandle, size_t*, headersCount);
    if (HTTPHeaders_GetHeaderCount_writes_to_its_outputs)
    {
        *headersCount = 0;
    }
    MOCK_METHOD_END(HTTP_HEADERS_RESULT, HTTP_HEADERS_OK)

        MOCK_STATIC_METHOD_3(, HTTP_HEADERS_RESULT, HTTPHeaders_GetHeader, HTTP_HEADERS_HANDLE, handle, size_t, index, char**, destination)
        HTTP_HEADERS_RESULT result2;
    currentHTTPHeaders_GetHeader_call++;
    if (whenShallHTTPHeaders_GetHeader_fail != currentHTTPHeaders_GetHeader_call)
    {
        if (index == 0)
        {
            *destination = (char*)BASEIMPLEMENTATION::gballoc_malloc(strlen(TEST_HEADER_1) + 1);
            strcpy(*destination, TEST_HEADER_1);
        }
        else if (index == 1)
        {
            *destination = (char*)BASEIMPLEMENTATION::gballoc_malloc(strlen(TEST_HEADER_1_5) + 1);
            strcpy(*destination, TEST_HEADER_1_5);

        }
        else if (index == 2)
        {
            *destination = (char*)BASEIMPLEMENTATION::gballoc_malloc(strlen(TEST_HEADER_2) + 1);
            strcpy(*destination, TEST_HEADER_2);

        }
        else if (index == 3)
        {
            *destination = (char*)BASEIMPLEMENTATION::gballoc_malloc(strlen(TEST_HEADER_3) + 1);
            strcpy(*destination, TEST_HEADER_3);

        }
        else
        {
            *destination = NULL; /*never to be reached*/
        }
        result2 = HTTP_HEADERS_OK;
    }
    else
    {
        result2 = HTTP_HEADERS_ERROR;
    }
    MOCK_METHOD_END(HTTP_HEADERS_RESULT, result2)

        MOCK_STATIC_METHOD_1(, STRING_HANDLE, URL_EncodeString, const char*, textEncode)
        currentURL_Encode_String_call++;
    MOCK_METHOD_END(STRING_HANDLE, (((whenShallURL_Encode_String_fail > 0) && (currentURL_Encode_String_call == whenShallURL_Encode_String_fail)) ? (STRING_HANDLE)NULL : BASEIMPLEMENTATION::STRING_construct(textEncode)));

    MOCK_STATIC_METHOD_3(, HTTPAPIEX_SAS_HANDLE, HTTPAPIEX_SAS_Create, STRING_HANDLE, key, STRING_HANDLE, uriResource, STRING_HANDLE, keyName)
        MOCK_METHOD_END(HTTPAPIEX_SAS_HANDLE, (HTTPAPIEX_SAS_HANDLE)malloc(1))

        MOCK_STATIC_METHOD_1(, void, HTTPAPIEX_SAS_Destroy, HTTPAPIEX_SAS_HANDLE, handle)
        free(handle);
    MOCK_VOID_METHOD_END()

        MOCK_STATIC_METHOD_8(, HTTPAPIEX_RESULT, HTTPAPIEX_ExecuteRequest2, HTTPAPIEX_HANDLE, handle, HTTPAPI_REQUEST_TYPE, requestType, const char*, relativePath, HTTP_HEADERS_HANDLE, requestHttpHeadersHandle, BUFFER_HANDLE, requestContent, unsigned int*, statusCode, HTTP_HEADERS_HANDLE, responseHttpHeadersHandle, BUFFER_HANDLE, responseContent)
        if (last_BUFFER_HANDLE_to_HTTPAPIEX_ExecuteRequest != NULL)
        {
            BASEIMPLEMENTATION::BUFFER_delete(last_BUFFER_HANDLE_to_HTTPAPIEX_ExecuteRequest);
        }
    last_BUFFER_HANDLE_to_HTTPAPIEX_ExecuteRequest = BASEIMPLEMENTATION::BUFFER_clone(requestContent);
    MOCK_METHOD_END(HTTPAPIEX_RESULT, HTTPAPIEX_OK)

        MOCK_STATIC_METHOD_9(, HTTPAPIEX_RESULT, HTTPAPIEX_SAS_ExecuteRequest2, HTTPAPIEX_SAS_HANDLE, sasHandle, HTTPAPIEX_HANDLE, handle, HTTPAPI_REQUEST_TYPE, requestType, const char*, relativePath, HTTP_HEADERS_HANDLE, requestHttpHeadersHandle, BUFFER_HANDLE, requestContent, unsigned int*, statusCode, HTTP_HEADERS_HANDLE, responseHttpHeadersHandle, BUFFER_HANDLE, responseContent)
        if (last_BUFFER_HANDLE_to_HTTPAPIEX_ExecuteRequest != NULL)
        {
            BASEIMPLEMENTATION::BUFFER_delete(last_BUFFER_HANDLE_to_HTTPAPIEX_ExecuteRequest);
        }
    last_BUFFER_HANDLE_to_HTTPAPIEX_ExecuteRequest = BASEIMPLEMENTATION::BUFFER_clone(requestContent);
    MOCK_METHOD_END(HTTPAPIEX_RESULT, HTTPAPIEX_OK)

        MOCK_STATIC_METHOD_1(, time_t, get_time, time_t*, currentTime)
        MOCK_METHOD_END(time_t, TEST_GET_TIME_VALUE)

        MOCK_STATIC_METHOD_2(, double, get_difftime, time_t, stopTime, time_t, startTime)
        MOCK_METHOD_END(double, stopTime - startTime)

        // vector.h
        MOCK_STATIC_METHOD_1(, VECTOR_HANDLE, VECTOR_create, size_t, elementSize)
        VECTOR_HANDLE result2;
    ++currentVECTOR_create_call;
    if ((whenShallVECTOR_create_fail > 0) &&
        (currentVECTOR_create_call == whenShallVECTOR_create_fail))
    {
        result2 = NULL;
    }
    else
    {
        result2 = BASEIMPLEMENTATION::VECTOR_create(elementSize);
    }
    MOCK_METHOD_END(VECTOR_HANDLE, result2)

        MOCK_STATIC_METHOD_1(, void, VECTOR_destroy, VECTOR_HANDLE, vector)
        BASEIMPLEMENTATION::VECTOR_destroy(vector);
    MOCK_VOID_METHOD_END()

        MOCK_STATIC_METHOD_3(, int, VECTOR_push_back, VECTOR_HANDLE, vector, const void*, elements, size_t, numElements)
        int result2;
    ++currentVECTOR_push_back_call;
    if ((whenShallVECTOR_push_back_fail > 0) &&
        (currentVECTOR_push_back_call == whenShallVECTOR_push_back_fail))
    {
        result2 = __LINE__;
    }
    else
    {
        result2 = BASEIMPLEMENTATION::VECTOR_push_back(vector, elements, numElements);
    }
    MOCK_METHOD_END(int, result2)

        MOCK_STATIC_METHOD_3(, void, VECTOR_erase, VECTOR_HANDLE, vector, void*, elements, size_t, numElements)
        BASEIMPLEMENTATION::VECTOR_erase(vector, elements, numElements);
    MOCK_VOID_METHOD_END()

        MOCK_STATIC_METHOD_2(, void*, VECTOR_element, VECTOR_HANDLE, vector, size_t, index)
        void* result2 = BASEIMPLEMENTATION::VECTOR_element(vector, index);
    MOCK_METHOD_END(void*, result2)

        MOCK_STATIC_METHOD_1(, void*, VECTOR_front, VECTOR_HANDLE, vector)
        void* result2 = BASEIMPLEMENTATION::VECTOR_front(vector);
    MOCK_METHOD_END(void*, result2)

        MOCK_STATIC_METHOD_1(, void*, VECTOR_back, const VECTOR_HANDLE, vector)
        void* result2 = BASEIMPLEMENTATION::VECTOR_back(vector);
    MOCK_METHOD_END(void*, result2)

        MOCK_STATIC_METHOD_3(, void*, VECTOR_find_if, VECTOR_HANDLE, vector, PREDICATE_FUNCTION, pred, const void*, value)
        void* result2;
    ++currentVECTOR_find_if_call;
    if ((whenShallVECTOR_find_if_fail > 0) &&
        (currentVECTOR_find_if_call == whenShallVECTOR_find_if_fail))
    {
        result2 = NULL;
    }
    else
    {
        result2 = BASEIMPLEMENTATION::VECTOR_find_if(vector, pred, value);
    }
    MOCK_METHOD_END(void*, result2)

        MOCK_STATIC_METHOD_1(, size_t, VECTOR_size, VECTOR_HANDLE, vector)
        size_t result2 = BASEIMPLEMENTATION::VECTOR_size(vector);
    MOCK_METHOD_END(size_t, result2)
};

DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportHttpMocks, , void, DList_InitializeListHead, PDLIST_ENTRY, listHead);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportHttpMocks, , int, DList_IsListEmpty, PDLIST_ENTRY, listHead);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportHttpMocks, , void, DList_InsertTailList, PDLIST_ENTRY, listHead, PDLIST_ENTRY, listEntry);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportHttpMocks, , void, DList_InsertHeadList, PDLIST_ENTRY, listHead, PDLIST_ENTRY, listEntry);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportHttpMocks, , void, DList_AppendTailList, PDLIST_ENTRY, listHead, PDLIST_ENTRY, ListToAppend);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportHttpMocks, , int, DList_RemoveEntryList, PDLIST_ENTRY, listEntry);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportHttpMocks, , PDLIST_ENTRY, DList_RemoveHeadList, PDLIST_ENTRY, listHead);

DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportHttpMocks, , void*, gballoc_malloc, size_t, size);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportHttpMocks, , void*, gballoc_realloc, void*, ptr, size_t, size);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportHttpMocks, , void, gballoc_free, void*, ptr);

DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubTransportHttpMocks, , void, eventConfirmationCallback, IOTHUB_CLIENT_LL_HANDLE, iotHubClientHandle, IOTHUB_CLIENT_CONFIRMATION_RESULT, result2, void*, userContextCallback);
DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubTransportHttpMocks, , int, messageCallback, IOTHUB_CLIENT_LL_HANDLE, iotHubClientHandle, IOTHUB_MESSAGE_HANDLE, message, void*, userContextCallback);

DECLARE_GLOBAL_MOCK_METHOD_0(CIoTHubTransportHttpMocks, , STRING_HANDLE, STRING_new);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportHttpMocks, , STRING_HANDLE, STRING_new_JSON, const char*, source);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportHttpMocks, , STRING_HANDLE, STRING_clone, STRING_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportHttpMocks, , STRING_HANDLE, STRING_construct, const char*, s);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportHttpMocks, , STRING_HANDLE, STRING_construct_n, const char*, s, size_t, size);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportHttpMocks, , void, STRING_delete, STRING_HANDLE, s);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportHttpMocks, , int, STRING_concat, STRING_HANDLE, s1, const char*, s2);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportHttpMocks, , int, STRING_concat_with_STRING, STRING_HANDLE, s1, STRING_HANDLE, s2);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportHttpMocks, , int, STRING_empty, STRING_HANDLE, s1);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportHttpMocks, , const char*, STRING_c_str, STRING_HANDLE, s);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportHttpMocks, , size_t, STRING_length, STRING_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportHttpMocks, , STRING_HANDLE, STRING_new_with_memory, const char*, memory);


DECLARE_GLOBAL_MOCK_METHOD_0(CIoTHubTransportHttpMocks, , HTTP_HEADERS_HANDLE, HTTPHeaders_Alloc);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportHttpMocks, , HTTP_HEADERS_HANDLE, HTTPHeaders_Clone, HTTP_HEADERS_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportHttpMocks, , void, HTTPHeaders_Free, HTTP_HEADERS_HANDLE, httpHeadersHandle);
DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubTransportHttpMocks, , HTTP_HEADERS_RESULT, HTTPHeaders_AddHeaderNameValuePair, HTTP_HEADERS_HANDLE, httpHeadersHandle, const char*, name, const char*, value);
DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubTransportHttpMocks, , HTTP_HEADERS_RESULT, HTTPHeaders_ReplaceHeaderNameValuePair, HTTP_HEADERS_HANDLE, httpHeadersHandle, const char*, name, const char*, value);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportHttpMocks, , const char*, HTTPHeaders_FindHeaderValue, HTTP_HEADERS_HANDLE, httpHeadersHandle, const char*, name);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportHttpMocks, , HTTP_HEADERS_RESULT, HTTPHeaders_GetHeaderCount, HTTP_HEADERS_HANDLE, httpHeadersHandle, size_t*, headersCount);
DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubTransportHttpMocks, , HTTP_HEADERS_RESULT, HTTPHeaders_GetHeader, HTTP_HEADERS_HANDLE, handle, size_t, index, char**, destination);


DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportHttpMocks, , HTTPAPIEX_HANDLE, HTTPAPIEX_Create, const char*, hostName);
DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubTransportHttpMocks, , HTTPAPIEX_RESULT, HTTPAPIEX_SetOption, HTTPAPIEX_HANDLE, handle, const char*, optionName, const void*, value);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportHttpMocks, , void, HTTPAPIEX_Destroy, HTTPAPIEX_HANDLE, handle);

DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportHttpMocks, , IOTHUB_MESSAGE_HANDLE, IoTHubMessage_CreateFromByteArray, const unsigned char*, buffer, size_t, size);
DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubTransportHttpMocks, , IOTHUB_MESSAGE_RESULT, IoTHubMessage_GetByteArray, IOTHUB_MESSAGE_HANDLE, iotHubMessageHandle, const unsigned char**, buffer, size_t*, size);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportHttpMocks, , const char*, IoTHubMessage_GetString, IOTHUB_MESSAGE_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportHttpMocks, , void, IoTHubMessage_Destroy, IOTHUB_MESSAGE_HANDLE, iotHubMessageHandle);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportHttpMocks, , MAP_HANDLE, IoTHubMessage_Properties, IOTHUB_MESSAGE_HANDLE, iotHubMessageHandle)
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportHttpMocks, , IOTHUBMESSAGE_CONTENT_TYPE, IoTHubMessage_GetContentType, IOTHUB_MESSAGE_HANDLE, iotHubMessageHandle)
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportHttpMocks, , IOTHUB_MESSAGE_RESULT, IoTHubMessage_SetMessageId, IOTHUB_MESSAGE_HANDLE, iotHubMessageHandle, const char*, messageId);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportHttpMocks, , const char*, IoTHubMessage_GetMessageId, IOTHUB_MESSAGE_HANDLE, iotHubMessageHandle);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportHttpMocks, , IOTHUB_MESSAGE_RESULT, IoTHubMessage_SetCorrelationId, IOTHUB_MESSAGE_HANDLE, iotHubMessageHandle, const char*, messageId);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportHttpMocks, , const char*, IoTHubMessage_GetCorrelationId, IOTHUB_MESSAGE_HANDLE, iotHubMessageHandle);

DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubTransportHttpMocks, , MAP_RESULT, Map_AddOrUpdate, MAP_HANDLE, handle, const char*, key, const char*, value);
DECLARE_GLOBAL_MOCK_METHOD_4(CIoTHubTransportHttpMocks, , MAP_RESULT, Map_GetInternals, MAP_HANDLE, handle, const char*const**, keys, const char*const**, values, size_t*, count);

DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportHttpMocks, , IOTHUBMESSAGE_DISPOSITION_RESULT, IoTHubClient_LL_MessageCallback, IOTHUB_CLIENT_LL_HANDLE, handle, IOTHUB_MESSAGE_HANDLE, message)
DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubTransportHttpMocks, , void, IoTHubClient_LL_SendComplete, IOTHUB_CLIENT_LL_HANDLE, handle, PDLIST_ENTRY, completed, IOTHUB_CLIENT_CONFIRMATION_RESULT, result2)


DECLARE_GLOBAL_MOCK_METHOD_0(CIoTHubTransportHttpMocks, , BUFFER_HANDLE, BUFFER_new);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportHttpMocks, , void, BUFFER_delete, BUFFER_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubTransportHttpMocks, , int, BUFFER_build, BUFFER_HANDLE, handle, const unsigned char*, source, size_t, size)
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportHttpMocks, , unsigned char*, BUFFER_u_char, BUFFER_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportHttpMocks, , size_t, BUFFER_length, BUFFER_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportHttpMocks, , int, BUFFER_pre_build, BUFFER_HANDLE, handle, size_t, size);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportHttpMocks, , int, BUFFER_content, BUFFER_HANDLE, handle, const unsigned char**, content);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportHttpMocks, , int, BUFFER_size, BUFFER_HANDLE, handle, size_t*, size);


DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportHttpMocks, , STRING_HANDLE, Base64_Encode_Bytes, const unsigned char*, source, size_t, size);

DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportHttpMocks, , STRING_HANDLE, URL_EncodeString, const char*, textEncode);

DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubTransportHttpMocks, , HTTPAPIEX_SAS_HANDLE, HTTPAPIEX_SAS_Create, STRING_HANDLE, key, STRING_HANDLE, uriResource, STRING_HANDLE, keyName);

DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportHttpMocks, , void, HTTPAPIEX_SAS_Destroy, HTTPAPIEX_SAS_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_9(CIoTHubTransportHttpMocks, , HTTPAPIEX_RESULT, HTTPAPIEX_SAS_ExecuteRequest2, HTTPAPIEX_SAS_HANDLE, sasHandle, HTTPAPIEX_HANDLE, handle, HTTPAPI_REQUEST_TYPE, requestType, const char*, relativePath, HTTP_HEADERS_HANDLE, requestHttpHeadersHandle, BUFFER_HANDLE, requestContent, unsigned int*, statusCode, HTTP_HEADERS_HANDLE, responseHttpHeadersHandle, BUFFER_HANDLE, responseContent);
DECLARE_GLOBAL_MOCK_METHOD_8(CIoTHubTransportHttpMocks, , HTTPAPIEX_RESULT, HTTPAPIEX_ExecuteRequest2, HTTPAPIEX_HANDLE, handle, HTTPAPI_REQUEST_TYPE, requestType, const char*, relativePath, HTTP_HEADERS_HANDLE, requestHttpHeadersHandle, BUFFER_HANDLE, requestContent, unsigned int*, statusCode, HTTP_HEADERS_HANDLE, responseHttpHeadersHandle, BUFFER_HANDLE, responseContent);

DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportHttpMocks, , time_t, get_time, time_t*, currentTime);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportHttpMocks, , double, get_difftime, time_t, stopTime, time_t, startTime);

//vector
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportHttpMocks, , VECTOR_HANDLE, VECTOR_create, size_t, elementSize);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportHttpMocks, , void, VECTOR_destroy, VECTOR_HANDLE, vector);
DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubTransportHttpMocks, , int, VECTOR_push_back, VECTOR_HANDLE, vector, const void*, elements, size_t, numElements);
DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubTransportHttpMocks, , void, VECTOR_erase, VECTOR_HANDLE, vector, void*, elements, size_t, numElements);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportHttpMocks, , void*, VECTOR_element, VECTOR_HANDLE, vector, size_t, index);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportHttpMocks, , void*, VECTOR_front, VECTOR_HANDLE, vector);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportHttpMocks, , void*, VECTOR_back, VECTOR_HANDLE, vector);
DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubTransportHttpMocks, , void*, VECTOR_find_if, VECTOR_HANDLE, vector, PREDICATE_FUNCTION, pred, const void*, value);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportHttpMocks, , size_t, VECTOR_size, VECTOR_HANDLE, vector);

extern "C" HTTPAPIEX_RESULT HTTPAPIEX_SAS_ExecuteRequest(HTTPAPIEX_SAS_HANDLE sasHandle, HTTPAPIEX_HANDLE handle, HTTPAPI_REQUEST_TYPE requestType, const char* relativePath, HTTP_HEADERS_HANDLE requestHttpHeadersHandle, BUFFER_HANDLE requestContent, unsigned int* statusCode, HTTP_HEADERS_HANDLE responseHttpHeadersHandle, BUFFER_HANDLE responseContent)
{
    *statusCode = 204;
    return HTTPAPIEX_SAS_ExecuteRequest2(sasHandle, handle, requestType, relativePath, requestHttpHeadersHandle, requestContent, statusCode, responseHttpHeadersHandle, responseContent);
}

extern "C" HTTPAPIEX_RESULT HTTPAPIEX_ExecuteRequest(HTTPAPIEX_HANDLE handle, HTTPAPI_REQUEST_TYPE requestType, const char* relativePath, HTTP_HEADERS_HANDLE requestHttpHeadersHandle, BUFFER_HANDLE requestContent, unsigned int* statusCode, HTTP_HEADERS_HANDLE responseHttpHeadersHandle, BUFFER_HANDLE responseContent)
{
    *statusCode = 204;
    return HTTPAPIEX_ExecuteRequest2(handle, requestType, relativePath, requestHttpHeadersHandle, requestContent, statusCode, responseHttpHeadersHandle, responseContent);
}

static void setupCreateHappyPathAlloc(CIoTHubTransportHttpMocks &mocks, bool deallocateCreated)
{
    (void)mocks;

    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
        .IgnoreArgument(1);

    if (deallocateCreated == true)
    {
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
    }
}
static void setupCreateHappyPathHostname(CIoTHubTransportHttpMocks &mocks, bool deallocateCreated)
{
    (void)mocks;

    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_IOTHUB_NAME));
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, "."))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, TEST_IOTHUB_SUFFIX))
        .IgnoreArgument(1);
    if (deallocateCreated == true)
    {
        STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
    }
}
static void setupCreateHappyPathGWHostname(CIoTHubTransportHttpMocks &mocks, bool deallocateCreated)
{
    (void)mocks;

    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_IOTHUB_GWHOSTNAME));
    if (deallocateCreated == true)
    {
        STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
    }
}
static void setupCreateHappyPathApiExHandle(CIoTHubTransportHttpMocks &mocks, bool deallocateCreated)
{
    (void)mocks;
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_Create(TEST_IOTHUB_NAME "." TEST_IOTHUB_SUFFIX))
        .IgnoreArgument(1);
    if (deallocateCreated == true)
    {
        STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_Destroy(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
    }
}
static void setupCreateHappyPathPerDeviceList(CIoTHubTransportHttpMocks &mocks, bool deallocateCreated)
{
    (void)mocks;

    STRICT_EXPECTED_CALL(mocks, VECTOR_create(IGNORED_NUM_ARG))
        .IgnoreArgument(1);
    if (deallocateCreated == true)
    {
        STRICT_EXPECTED_CALL(mocks, VECTOR_destroy(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
    }
}

static void setupCreateHappyPath(CIoTHubTransportHttpMocks &mocks, bool deallocateCreated)
{
    setupCreateHappyPathAlloc(mocks, deallocateCreated);
    setupCreateHappyPathHostname(mocks, deallocateCreated);
    setupCreateHappyPathApiExHandle(mocks, deallocateCreated);
    setupCreateHappyPathPerDeviceList(mocks, deallocateCreated);
}

static void setupRegisterHappyPathNotFoundInList(CIoTHubTransportHttpMocks &mocks)
{
    (void)mocks;
    STRICT_EXPECTED_CALL(mocks, VECTOR_find_if(IGNORED_PTR_ARG, IGNORED_PTR_ARG, TEST_DEVICE_ID))
        .IgnoreAllArguments();
}

static void setupRegisterHappyPathAllocHandle(CIoTHubTransportHttpMocks &mocks, bool deallocateCreated)
{
    (void)mocks;

    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
        .IgnoreArgument(1);

    if (deallocateCreated == true)
    {
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
    }
}

static void setupRegisterHappyPathcreate_deviceId(CIoTHubTransportHttpMocks &mocks, bool deallocateCreated)
{
    (void)mocks;
    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_DEVICE_ID));
    if (deallocateCreated == true)
    {
        STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
    }
}

static void setupRegisterHappyPathcreate_deviceKey(CIoTHubTransportHttpMocks &mocks, bool deallocateCreated, bool is_x509_used=false)
{
    (void)mocks;

    if(is_x509_used==false)
    {
        STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_DEVICE_KEY));
        if (deallocateCreated == true)
        {
            STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
                .IgnoreArgument(1);
        }
    }
}

static void setupRegisterHappyPathcreate_deviceSasToken(CIoTHubTransportHttpMocks &mocks, bool deallocateCreated)
{
    (void)mocks;

    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_DEVICE_TOKEN));
    if (deallocateCreated == true)
    {
        STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
    }
}

static void setupRegisterHappyPatheventHTTPrelativePath(CIoTHubTransportHttpMocks &mocks, bool deallocateCreated)
{
    (void)mocks;
    /*creating eventHTTPrelativePath*/
    STRICT_EXPECTED_CALL(mocks, STRING_construct("/devices/"));
    STRICT_EXPECTED_CALL(mocks, URL_EncodeString(TEST_DEVICE_ID));
    STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, EVENT_ENDPOINT API_VERSION))
        .IgnoreArgument(1);
    if (deallocateCreated == true)
    {
        STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
    }
    /*the url encoded device id*/
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
}

static void setupRegisterHappyPathmessageHTTPrelativePath(CIoTHubTransportHttpMocks &mocks, bool deallocateCreated)
{
    (void)mocks;
    STRICT_EXPECTED_CALL(mocks, STRING_construct("/devices/"));
    STRICT_EXPECTED_CALL(mocks, URL_EncodeString(TEST_DEVICE_ID));
    STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, MESSAGE_ENDPOINT_HTTP API_VERSION))
        .IgnoreArgument(1);
    if (deallocateCreated == true)
    {
        STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
    }
    /*the url encoded device id*/
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
}

static void setupRegisterHappyPatheventHTTPrequestHeaders(CIoTHubTransportHttpMocks &mocks, bool deallocateCreated, bool is_x509_used=false)
{
    (void)mocks;
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc());
    STRICT_EXPECTED_CALL(mocks, STRING_construct("/devices/"));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, URL_EncodeString(TEST_DEVICE_ID));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, EVENT_ENDPOINT))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "iothub-to", "/devices/"  TEST_DEVICE_ID  EVENT_ENDPOINT))
        .IgnoreArgument(1);
    
    if(is_x509_used==false)
    {
        STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "Authorization", TEST_BLANK_SAS_TOKEN))
            .IgnoreArgument(1);
    }

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "Accept", "application/json"))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "Connection", "Keep-Alive"))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "User-Agent", CLIENT_DEVICE_TYPE_PREFIX CLIENT_DEVICE_BACKSLASH IOTHUB_SDK_VERSION))
        .IgnoreArgument(1);
    if (deallocateCreated == true)
    {
        STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
    }
}

static void setupRegisterHappyPathmessageHTTPrequestHeaders(CIoTHubTransportHttpMocks &mocks, bool deallocateCreated, bool is_x509_used=false)
{
    (void)mocks;
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc());
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "User-Agent", CLIENT_DEVICE_TYPE_PREFIX CLIENT_DEVICE_BACKSLASH IOTHUB_SDK_VERSION))
        .IgnoreArgument(1);
    if(is_x509_used==false)
    {
        STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "Authorization", TEST_BLANK_SAS_TOKEN))
            .IgnoreArgument(1);
    }
    if (deallocateCreated == true)
    {
        STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
    }
}

static void setupRegisterHappyPathabandonHTTPrelativePathBegin(CIoTHubTransportHttpMocks &mocks, bool deallocateCreated)
{
    (void)mocks;
    STRICT_EXPECTED_CALL(mocks, STRING_construct("/devices/"));
    STRICT_EXPECTED_CALL(mocks, URL_EncodeString(TEST_DEVICE_ID));
    STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, MESSAGE_ENDPOINT_HTTP_ETAG))
        .IgnoreArgument(1);

    if (deallocateCreated == true)
    {
        STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
    }
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
}

static void setupRegisterHappyPathsasObject(CIoTHubTransportHttpMocks &mocks, bool deallocateCreated, bool is_x509_used=false)
{
    (void)mocks;
    if(is_x509_used==false)
    {
        STRICT_EXPECTED_CALL(mocks, URL_EncodeString(TEST_DEVICE_ID));
        STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG)) /*encoded device id*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_clone(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG)) /*uriResource*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, "/devices/"))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2);
        STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_DEVICE_KEY));
        STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG)) /*key*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_empty(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2)
            .IgnoreArgument(3);
        if (deallocateCreated)
        {
            STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_Destroy(IGNORED_PTR_ARG))
                .IgnoreArgument(1);
        }
    }
}

static void setupRegisterHappyPatheventConfirmations(CIoTHubTransportHttpMocks &mocks)
{
    (void)mocks;
    STRICT_EXPECTED_CALL(mocks, DList_InitializeListHead(IGNORED_PTR_ARG)).IgnoreAllArguments();
}

static void setupRegisterHappyPathDeviceListAdd(CIoTHubTransportHttpMocks &mocks)
{
    (void)mocks;
    STRICT_EXPECTED_CALL(mocks, VECTOR_push_back(IGNORED_PTR_ARG, IGNORED_PTR_ARG, 1))
        .IgnoreArgument(1).IgnoreArgument(2);
}


static void setupRegisterHappyPath(CIoTHubTransportHttpMocks &mocks, bool deallocateCreated, bool is_x509_used=false)
{
    setupRegisterHappyPathNotFoundInList(mocks);
    setupRegisterHappyPathAllocHandle(mocks, deallocateCreated);
    setupRegisterHappyPathcreate_deviceId(mocks, deallocateCreated);
    setupRegisterHappyPathcreate_deviceKey(mocks, deallocateCreated, is_x509_used);
    setupRegisterHappyPatheventHTTPrelativePath(mocks, deallocateCreated);
    setupRegisterHappyPathmessageHTTPrelativePath(mocks, deallocateCreated);
    setupRegisterHappyPatheventHTTPrequestHeaders(mocks, deallocateCreated, is_x509_used);
    setupRegisterHappyPathmessageHTTPrequestHeaders(mocks, deallocateCreated, is_x509_used);
    setupRegisterHappyPathabandonHTTPrelativePathBegin(mocks, deallocateCreated);
    setupRegisterHappyPathsasObject(mocks, deallocateCreated, is_x509_used);
    setupRegisterHappyPathDeviceListAdd(mocks);
    setupRegisterHappyPatheventConfirmations(mocks);
}

static void setupRegisterHappyPathWithSasToken(CIoTHubTransportHttpMocks &mocks, bool deallocateCreated)
{
    setupRegisterHappyPathNotFoundInList(mocks);
    setupRegisterHappyPathAllocHandle(mocks, deallocateCreated);
    setupRegisterHappyPathcreate_deviceId(mocks, deallocateCreated);
    setupRegisterHappyPathcreate_deviceSasToken(mocks, deallocateCreated);
    setupRegisterHappyPatheventHTTPrelativePath(mocks, deallocateCreated);
    setupRegisterHappyPathmessageHTTPrelativePath(mocks, deallocateCreated);
    setupRegisterHappyPatheventHTTPrequestHeaders(mocks, deallocateCreated);
    setupRegisterHappyPathmessageHTTPrequestHeaders(mocks, deallocateCreated);
    setupRegisterHappyPathabandonHTTPrelativePathBegin(mocks, deallocateCreated);
    setupRegisterHappyPathDeviceListAdd(mocks);
    setupRegisterHappyPatheventConfirmations(mocks);
}

static void setupUnregisterOneDevice(CIoTHubTransportHttpMocks &mocks)
{
    (void)mocks;
    //destroy_deviceId(perDeviceItem);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    //destroy_deviceKey(perDeviceItem);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    //destroy_eventHTTPrelativePath(perDeviceItem);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    //destroy_messageHTTPrelativePath(perDeviceItem);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    //destroy_eventHTTPrequestHeaders(perDeviceItem);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    //destroy_messageHTTPrequestHeaders(perDeviceItem);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    //destroy_abandonHTTPrelativePathBegin(perDeviceItem);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    //destroy_SASObject(perDeviceItem);
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_Destroy(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

}

static void setupDoWorkLoopOnceForOneDevice(CIoTHubTransportHttpMocks &mocks)
{
    (void)mocks;

    STRICT_EXPECTED_CALL(mocks, VECTOR_size(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, VECTOR_element(IGNORED_PTR_ARG, 0))
        .IgnoreArgument(1);
}

static void setupDoWorkLoopForNextDevice(CIoTHubTransportHttpMocks &mocks, size_t next)
{
    (void)mocks;

    STRICT_EXPECTED_CALL(mocks, VECTOR_element(IGNORED_PTR_ARG, next))
        .IgnoreArgument(1);
}

//
//static void setupInitHappyPathUpThroughHostName(CIoTHubTransportHttpMocks &mocks, bool deallocateCreated)
//{
//    (void)mocks;
//
//    setupInitHappyPathUpThroughEventHTTPrequestHeaders(mocks, deallocateCreated);
//    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_IOTHUB_NAME));
//    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, "."))
//        .IgnoreArgument(1);
//    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, TEST_IOTHUB_SUFFIX))
//        .IgnoreArgument(1);
//    if (deallocateCreated == true)
//    {
//        STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
//            .IgnoreArgument(1);
//    }
//}
//
//static void setupInitHappyPathUpThroughHttpApiExHandle(CIoTHubTransportHttpMocks &mocks, bool deallocateCreated)
//{
//    (void)mocks;
//
//    setupInitHappyPathUpThroughHostName(mocks, deallocateCreated);
//    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
//        .IgnoreArgument(1);
//    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_Create(TEST_IOTHUB_NAME "." TEST_IOTHUB_SUFFIX))
//        .IgnoreArgument(1);
//    if (deallocateCreated == true)
//    {
//        STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_Destroy(IGNORED_PTR_ARG))
//            .IgnoreArgument(1);
//    }
//}
//
//
//static void setupInitHappyPathUpThroughAbandonHTTPrelativePathBegin(CIoTHubTransportHttpMocks &mocks, bool deallocateCreated)
//{
//    (void)mocks;
//
//    setupInitHappyPathUpThroughMessageHTTPrequestHeaders(mocks, deallocateCreated);
//    STRICT_EXPECTED_CALL(mocks, STRING_construct("/devices/"));
//    STRICT_EXPECTED_CALL(mocks, URL_EncodeString(TEST_DEVICE_ID));
//    STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
//        .IgnoreArgument(1)
//        .IgnoreArgument(2);
//    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, MESSAGE_ENDPOINT_HTTP_ETAG))
//        .IgnoreArgument(1);
//
//    if (deallocateCreated == true)
//    {
//        STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
//            .IgnoreArgument(1);
//    }
//    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
//        .IgnoreArgument(1);
//
//}
//

static pfIoTHubTransport_GetHostname    IoTHubTransportHttp_GetHostname;
static pfIoTHubTransport_SetOption      IoTHubTransportHttp_SetOption;
static pfIoTHubTransport_Create         IoTHubTransportHttp_Create;
static pfIoTHubTransport_Destroy        IoTHubTransportHttp_Destroy;
static pfIotHubTransport_Register       IoTHubTransportHttp_Register;
static pfIotHubTransport_Unregister     IoTHubTransportHttp_Unregister;
static pfIoTHubTransport_Subscribe      IoTHubTransportHttp_Subscribe;
static pfIoTHubTransport_Unsubscribe    IoTHubTransportHttp_Unsubscribe;
static pfIoTHubTransport_DoWork         IoTHubTransportHttp_DoWork;
static pfIoTHubTransport_GetSendStatus  IoTHubTransportHttp_GetSendStatus;

BEGIN_TEST_SUITE(iothubtransporthttp)

TEST_SUITE_INITIALIZE(TestClassInitialize)
{
    TEST_INITIALIZE_MEMORY_DEBUG(g_dllByDll);
    g_testByTest = MicroMockCreateMutex();
    ASSERT_IS_NOT_NULL(g_testByTest);
    /*the size of bigBuffer needs to be such that it doesn't fit alone 1 payload of 256KB.*/ /*assume a limit of "less than 1 than 256"*/
                                                                                             /*the payload already must have the following string: [{"body":""}], which is 13 characters. From (256*1024-1) - 13 =  262130 characters that can go in the base64 encoding.*/
                                                                                             /*that means 2621301/4*3 = 196597.5 original characters. that means:
                                                                                             1. 196597 original characters shall fit
                                                                                             2. 196598 original characters shall not fit*/
    bigBufferOverflow = (unsigned char*)malloc(TEST_BIG_BUFFER_1_OVERFLOW_SIZE);
    memset(bigBufferOverflow, '3', TEST_BIG_BUFFER_1_OVERFLOW_SIZE);
    bigBufferFit = (unsigned char*)malloc(TEST_BIG_BUFFER_1_FIT_SIZE);
    memset(bigBufferFit, '3', TEST_BIG_BUFFER_1_FIT_SIZE);

    unsigned char* temp = (unsigned char*)malloc(buffer9_size);
    memset(temp, '3', TEST_BIG_BUFFER_9_OVERFLOW_SIZE);
    buffer9 = temp;

    temp = (unsigned char*)malloc(buffer11_size);
    memset(temp, '3', buffer11_size);
    buffer11 = temp;

    IoTHubTransportHttp_GetHostname = ((TRANSPORT_PROVIDER*)HTTP_Protocol())->IoTHubTransport_GetHostname;
    IoTHubTransportHttp_SetOption = ((TRANSPORT_PROVIDER*)HTTP_Protocol())->IoTHubTransport_SetOption;
    IoTHubTransportHttp_Create = ((TRANSPORT_PROVIDER*)HTTP_Protocol())->IoTHubTransport_Create;
    IoTHubTransportHttp_Destroy = ((TRANSPORT_PROVIDER*)HTTP_Protocol())->IoTHubTransport_Destroy;
    IoTHubTransportHttp_Register = ((TRANSPORT_PROVIDER*)HTTP_Protocol())->IoTHubTransport_Register;
    IoTHubTransportHttp_Unregister = ((TRANSPORT_PROVIDER*)HTTP_Protocol())->IoTHubTransport_Unregister;
    IoTHubTransportHttp_Subscribe = ((TRANSPORT_PROVIDER*)HTTP_Protocol())->IoTHubTransport_Subscribe;
    IoTHubTransportHttp_Unsubscribe = ((TRANSPORT_PROVIDER*)HTTP_Protocol())->IoTHubTransport_Unsubscribe;
    IoTHubTransportHttp_DoWork = ((TRANSPORT_PROVIDER*)HTTP_Protocol())->IoTHubTransport_DoWork;
    IoTHubTransportHttp_GetSendStatus = ((TRANSPORT_PROVIDER*)HTTP_Protocol())->IoTHubTransport_GetSendStatus;

}

TEST_SUITE_CLEANUP(TestClassCleanup)
{
    free((void*)buffer9);
    free((void*)buffer11);
    free(bigBufferFit);
    free(bigBufferOverflow);

    MicroMockDestroyMutex(g_testByTest);
    TEST_DEINITIALIZE_MEMORY_DEBUG(g_dllByDll);
}

TEST_FUNCTION_INITIALIZE(TestMethodInitialize)
{
    if (!MicroMockAcquireMutex(g_testByTest))
    {
        ASSERT_FAIL("our mutex is ABANDONED. Failure in test framework");
    }

    currentmalloc_call = 0;
    whenShallmalloc_fail = 0;

    currentSTRING_new_call = 0;
    whenShallSTRING_new_fail = 0;

    currentSTRING_new_JSON_call = 0;
    whenShallSTRING_new_JSON_fail = 0;

    currentSTRING_clone_call = 0;
    whenShallSTRING_clone_fail = 0;

    currentSTRING_construct_call = 0;
    whenShallSTRING_construct_fail = 0;

    currentSTRING_concat_call = 0;
    whenShallSTRING_concat_fail = 0;

    currentSTRING_empty_call = 0;
    whenShallSTRING_empty_fail = 0;

    currentSTRING_concat_with_STRING_call = 0;
    whenShallSTRING_concat_with_STRING_fail = 0;

    currentHTTPHeaders_Alloc_call = 0;
    whenShallHTTPHeaders_Alloc_fail = 0;

    currentHTTPHeaders_Clone_call = 0;
    whenShallHTTPHeaders_Clone_fail = 0;


    BASEIMPLEMENTATION::DList_InitializeListHead(&waitingToSend);
    BASEIMPLEMENTATION::DList_InitializeListHead(&waitingToSend2);

    currentBUFFER_new_call = 0;
    whenShallBUFFER_new_fail = 0;

    currentBUFFER_build_call = 0;
    whenShallBUFFER_build_fail = 0;

    currentBase64_Encode_Bytes_call = 0;
    whenShallBase64_Encode_Bytes_fail = 0;

    HTTPHeaders_GetHeaderCount_writes_to_its_outputs = true;

    whenShallHTTPHeaders_GetHeader_fail = 0;
    currentHTTPHeaders_GetHeader_call = 0;

    currentURL_Encode_String_call = 0;
    whenShallURL_Encode_String_fail = 0;

    currentVECTOR_create_call = 0;
    whenShallVECTOR_create_fail = 0;

    currentVECTOR_push_back_call = 0;
    whenShallVECTOR_push_back_fail = 0;

    currentVECTOR_find_if_call = 0;
    whenShallVECTOR_find_if_fail = 0;

    last_BUFFER_HANDLE_to_HTTPAPIEX_ExecuteRequest = NULL;
}


TEST_FUNCTION_CLEANUP(TestMethodCleanup)
{
    if (last_BUFFER_HANDLE_to_HTTPAPIEX_ExecuteRequest != NULL)
    {
        BASEIMPLEMENTATION::BUFFER_delete(last_BUFFER_HANDLE_to_HTTPAPIEX_ExecuteRequest);
    }
    if (!MicroMockReleaseMutex(g_testByTest))
    {
        ASSERT_FAIL("failure in test framework at ReleaseMutex");
    }
}

// Testing the values of constants allows me to use them throughout the unit tests (e.g., when setting expectations on mocks) without fear of missing a bug.

//Tests_SRS_TRANSPORTMULTITHTTP_17_017: [ IoTHubTransportHttp_Register shall create an immutable string (further called "event HTTP relative path") from the following pieces: "/devices/" + URL_ENCODED(config->deviceConfig->deviceId) + "/messages/events" + APIVERSION. ]
TEST_FUNCTION(IotHubTransportHttp_EVENT_ENDPOINT_constant_is_expected_value)
{
    ASSERT_ARE_EQUAL(char_ptr, "/messages/events", EVENT_ENDPOINT);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_019: [ IoTHubTransportHttp_Register shall create an immutable string (further called "message HTTP relative path") from the following pieces: "/devices/" + URL_ENCODED(config->deviceConfig->deviceId) + "/messages/devicebound" + APIVERSION. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_024: [ IoTHubTransportHttp_Register shall create a STRING containing: "/devices/" + URL_ENCODED(device id) +"/messages/deviceBound/" called abandonHTTPrelativePathBegin. ]/
TEST_FUNCTION(IotHubTransportHttp_MESSAGE_ENDPOINT_HTTP_constant_is_expected_value)
{
    ASSERT_ARE_EQUAL(char_ptr, "/messages/devicebound", MESSAGE_ENDPOINT_HTTP);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_019: [ IoTHubTransportHttp_Register shall create an immutable string (further called "message HTTP relative path") from the following pieces: "/devices/" + URL_ENCODED(config->deviceConfig->deviceId) + "/messages/devicebound" + APIVERSION. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_024: [ IoTHubTransportHttp_Register shall create a STRING containing: "/devices/" + URL_ENCODED(device id) +"/messages/deviceBound/" called abandonHTTPrelativePathBegin. ]/
TEST_FUNCTION(IotHubTransportHttp_MESSAGE_ENDPOINT_HTTP_ETAG_constant_is_expected_value)
{
    ASSERT_ARE_EQUAL(char_ptr, "/messages/devicebound/", MESSAGE_ENDPOINT_HTTP_ETAG);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_017: [ IoTHubTransportHttp_Register shall create an immutable string (further called "event HTTP relative path") from the following pieces: "/devices/" + URL_ENCODED(config->deviceConfig->deviceId) + "/messages/events" + APIVERSION. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_019: [ IoTHubTransportHttp_Register shall create an immutable string (further called "message HTTP relative path") from the following pieces: "/devices/" + URL_ENCODED(config->deviceConfig->deviceId) + "/messages/devicebound" + APIVERSION. ]
TEST_FUNCTION(IotHubTransportHttp_API_VERSION_constant_is_expected_value)
{
    ASSERT_ARE_EQUAL(char_ptr, "?api-version=2016-02-03", API_VERSION);
}


//Tests_SRS_TRANSPORTMULTITHTTP_17_001: [If parameter config is NULL, then IoTHubTransportHttp_Create shall return NULL.]
TEST_FUNCTION(IoTHubTransportHttp_Create_with_NULL_parameter_fails)
{
    // arrange

    // act
    auto result = IoTHubTransportHttp_Create(NULL);

    // assert
    ASSERT_IS_NULL(result);

}

//Tests_SRS_TRANSPORTMULTITHTTP_17_002: [ If field transportConfig is NULL, then IoTHubTransportHttp_Create shall return NULL. 
TEST_FUNCTION(IoTHubTransportHttp_Create_with_NULL_config_parameter_fails)
{
    // arrange

    // act
    auto result = IoTHubTransportHttp_Create(&TEST_CONFIG_NULL_CONFIG);

    // assert
    ASSERT_IS_NULL(result);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_003: [ If fields protocol, iotHubName or iotHubSuffix in transportConfig are NULL, then IoTHubTransportHttp_Create shall return NULL. ]
TEST_FUNCTION(IoTHubTransportHttp_Create_with_NULL_protocol_parameter_fails)
{
    // arrange

    // act
    auto result = IoTHubTransportHttp_Create(&TEST_CONFIG_NULL_PROTOCOL);

    // assert
    ASSERT_IS_NULL(result);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_003: [ If fields protocol, iotHubName or iotHubSuffix in transportConfig are NULL, then IoTHubTransportHttp_Create shall return NULL. ]
TEST_FUNCTION(IoTHubTransportHttp_Create_with_NULL_iothub_name_fails)
{
    // arrange

    // act
    auto result = IoTHubTransportHttp_Create(&TEST_CONFIG_NULL_IOTHUB_NAME);

    // assert
    ASSERT_IS_NULL(result);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_003: [ If fields protocol, iotHubName or iotHubSuffix in transportConfig are NULL, then IoTHubTransportHttp_Create shall return NULL. ]
TEST_FUNCTION(IoTHubTransportHttp_Create_with_NULL_iothub_suffix_fails)
{
    // arrange

    // act
    auto result = IoTHubTransportHttp_Create(&TEST_CONFIG_NULL_IOTHUB_SUFFIX);

    // assert
    ASSERT_IS_NULL(result);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_005: [If config->upperConfig->protocolGatewayHostName is NULL, `IoTHubTransportHttp_Create` shall create an immutable string (further called hostname) containing `config->transportConfig->iotHubName + config->transportConfig->iotHubSuffix`.] 
//Tests_SRS_TRANSPORTMULTITHTTP_17_007: [ IoTHubTransportHttp_Create shall create a HTTPAPIEX_HANDLE by a call to HTTPAPIEX_Create passing for hostName the hostname so far constructed by IoTHubTransportHttp_Create. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_009: [ IoTHubTransportHttp_Create shall call VECTOR_create to create a list of registered devices. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_130: [ IoTHubTransportHttp_Create shall allocate memory for the handle. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_011: [ Otherwise, IoTHubTransportHttp_Create shall succeed and return a non-NULL value. ]
TEST_FUNCTION(IoTHubTransportHttp_Create_happy_path)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;

    setupCreateHappyPath(mocks, false);

    ///act
    auto result = IoTHubTransportHttp_Create(&TEST_CONFIG);

    ///assert
    ASSERT_IS_NOT_NULL(result);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(result);
}

//Tests_SRS_TRANSPORTMULTITHTTP_20_001: [If config->upperConfig->protocolGatewayHostName is not NULL, IoTHubTransportHttp_Create shall use it as hostname] 
//Tests_SRS_TRANSPORTMULTITHTTP_17_007: [ IoTHubTransportHttp_Create shall create a HTTPAPIEX_HANDLE by a call to HTTPAPIEX_Create passing for hostName the hostname so far constructed by IoTHubTransportHttp_Create. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_009: [ IoTHubTransportHttp_Create shall call VECTOR_create to create a list of registered devices. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_130: [ IoTHubTransportHttp_Create shall allocate memory for the handle. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_011: [ Otherwise, IoTHubTransportHttp_Create shall succeed and return a non-NULL value. ]
TEST_FUNCTION(IoTHubTransportHttp_Create_happy_path_with_gwhostname)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;

    setupCreateHappyPathAlloc(mocks, false);
    setupCreateHappyPathGWHostname(mocks, false);
    setupCreateHappyPathApiExHandle(mocks, false);
    setupCreateHappyPathPerDeviceList(mocks, false);

    ///act
    auto result = IoTHubTransportHttp_Create(&TEST_GW_CONFIG);

    ///assert
    ASSERT_IS_NOT_NULL(result);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(result);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_010: [ If creating the list fails, then IoTHubTransportHttp_Create shall fail and return NULL. ]
TEST_FUNCTION(IoTHubTransportHttp_Create_fails_when_perDeviceList_fails)
{
    CIoTHubTransportHttpMocks mocks;

    setupCreateHappyPathAlloc(mocks, true);
    setupCreateHappyPathHostname(mocks, true);
    setupCreateHappyPathApiExHandle(mocks, true);
    whenShallVECTOR_create_fail = 1;
    setupCreateHappyPathPerDeviceList(mocks, false);

    ///act
    auto result = IoTHubTransportHttp_Create(&TEST_CONFIG);

    ///assert
    ASSERT_IS_NULL(result);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_008: [ If creating the HTTPAPIEX_HANDLE fails then IoTHubTransportHttp_Create shall fail and return NULL. ]
TEST_FUNCTION(IoTHubTransportHttp_Create_fails_when_ApiExCreate_fails)
{
    CIoTHubTransportHttpMocks mocks;

    setupCreateHappyPathAlloc(mocks, true);
    setupCreateHappyPathHostname(mocks, true);
    /*creating httpApiExHandle*/
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_Create(TEST_IOTHUB_NAME "." TEST_IOTHUB_SUFFIX))
        .IgnoreArgument(1)
        .SetReturn((HTTPAPIEX_HANDLE)NULL);


    ///act
    auto result = IoTHubTransportHttp_Create(&TEST_CONFIG);

    ///assert
    ASSERT_IS_NULL(result);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_006: [ If creating the hostname fails then IoTHubTransportHttp_Create shall fail and return NULL. ]
TEST_FUNCTION(IoTHubTransportHttp_Create_fails_when_hostname_fails_1)
{
    CIoTHubTransportHttpMocks mocks;

    setupCreateHappyPathAlloc(mocks, true);
    /*Creating hostName*/
    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_IOTHUB_NAME));
    whenShallSTRING_concat_fail = 2;
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, "."))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, TEST_IOTHUB_SUFFIX))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    ///act
    auto result = IoTHubTransportHttp_Create(&TEST_CONFIG);

    ///assert
    ASSERT_IS_NULL(result);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_006: [ If creating the hostname fails then IoTHubTransportHttp_Create shall fail and return NULL. ]
TEST_FUNCTION(IoTHubTransportHttp_Create_fails_when_hostname_fails_2)
{
    CIoTHubTransportHttpMocks mocks;

    setupCreateHappyPathAlloc(mocks, true);
    /*Creating hostName*/
    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_IOTHUB_NAME));
    whenShallSTRING_concat_fail = 1;
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, "."))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    ///act
    auto result = IoTHubTransportHttp_Create(&TEST_CONFIG);

    ///assert
    ASSERT_IS_NULL(result);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_006: [ If creating the hostname fails then IoTHubTransportHttp_Create shall fail and return NULL. ]
TEST_FUNCTION(IoTHubTransportHttp_Create_fails_when_hostname_fails_3)
{
    CIoTHubTransportHttpMocks mocks;

    setupCreateHappyPathAlloc(mocks, true);
    /*Creating hostName*/
    whenShallSTRING_construct_fail = 1;

    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_IOTHUB_NAME));

    ///act
    auto result = IoTHubTransportHttp_Create(&TEST_CONFIG);

    ///assert
    ASSERT_IS_NULL(result);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_131: [ If allocation fails, IoTHubTransportHttp_Create shall fail and return NULL. ]
TEST_FUNCTION(IoTHubTransportHttp_Create_fails_when_Alloc_fails)
{
    CIoTHubTransportHttpMocks mocks;

    whenShallmalloc_fail = 1;
    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
        .IgnoreArgument(1);

    ///act
    auto result = IoTHubTransportHttp_Create(&TEST_CONFIG);

    ///assert
    ASSERT_IS_NULL(result);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_012: [ IoTHubTransportHttp_Destroy shall do nothing is handle is NULL. ]
TEST_FUNCTION(IoTHubTransportHttp_Destroy_with_NULL_handle_does_nothing)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;

    ///act
    IoTHubTransportHttp_Destroy(NULL);

    ///assert - uMock

}

//Tests_SRS_TRANSPORTMULTITHTTP_17_013: [ Otherwise, IoTHubTransportHttp_Destroy shall free all the resources currently in use. ]
TEST_FUNCTION(IoTHubTransportHttp_Destroy_succeeds_NoRegistered_devices)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);                                             //STRING_HANDLE hostName;
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_Destroy(IGNORED_PTR_ARG))
        .IgnoreArgument(1);                                             //HTTPAPIEX_HANDLE httpApiExHandle;
    STRICT_EXPECTED_CALL(mocks, VECTOR_size(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, VECTOR_destroy(IGNORED_PTR_ARG))
        .IgnoreArgument(1);                                             //VECTOR_HANDLE perDeviceList;

    STRICT_EXPECTED_CALL(mocks, gballoc_free(handle));

    ///act
    IoTHubTransportHttp_Destroy(handle);

    ///assert

}

//Tests_SRS_TRANSPORTMULTITHTTP_17_013: [ Otherwise, IoTHubTransportHttp_Destroy shall free all the resources currently in use. ]
TEST_FUNCTION(IoTHubTransportHttp_Destroy_succeeds_One_Registered_device)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;

    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);
    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);                                             //STRING_HANDLE hostName;
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_Destroy(IGNORED_PTR_ARG))
        .IgnoreArgument(1);                                             //HTTPAPIEX_HANDLE httpApiExHandle;
    STRICT_EXPECTED_CALL(mocks, VECTOR_size(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, VECTOR_element(IGNORED_PTR_ARG, 0))
        .IgnoreArgument(1);
    setupUnregisterOneDevice(mocks);
    STRICT_EXPECTED_CALL(mocks, gballoc_free(devHandle));


    STRICT_EXPECTED_CALL(mocks, VECTOR_destroy(IGNORED_PTR_ARG))
        .IgnoreArgument(1);                                             //VECTOR_HANDLE perDeviceList;

    STRICT_EXPECTED_CALL(mocks, gballoc_free(handle));

    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);                                             //STRING_HANDLE deviceSasToken;

    ///act
    IoTHubTransportHttp_Destroy(handle);

    ///assert

}

//Tests_SRS_TRANSPORTMULTITHTTP_17_133: [ IoTHubTransportHttp_Register shall create an immutable string (further called "deviceId") from config->deviceConfig->deviceId. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_135: [ IoTHubTransportHttp_Register shall create an immutable string (further called "deviceKey") from deviceKey. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_017: [ IoTHubTransportHttp_Register shall create an immutable string (further called "event HTTP relative path") from the following pieces: "/devices/" + URL_ENCODED(config->deviceConfig->deviceId) + "/messages/events" + APIVERSION. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_017: [ IoTHubTransportHttp_Register shall create an immutable string (further called "event HTTP relative path") from the following pieces: "/devices/" + URL_ENCODED(config->deviceConfig->deviceId) + "/messages/events" + APIVERSION. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_021: [ IoTHubTransportHttp_Register shall create a set of HTTP headers (further called "event HTTP request headers") consisting of the following fixed field names and values: "iothub-to":"/devices/" + URL_ENCODED(config->deviceConfig->deviceId) + "/messages/events"; "Authorization":""
//"Accept":"application/json"
//"Connection" : "Keep-Alive" ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_132: [ IoTHubTransportHttp_Register shall create a set of HTTP headers (further called "message HTTP request headers") consisting of the following fixed field names and values:
//"Authorization": "" ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_024: [ IoTHubTransportHttp_Register shall create a STRING containing: "/devices/" + URL_ENCODED(device id) +"/messages/deviceBound/" called abandonHTTPrelativePathBegin. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_026: [ IoTHubTransportHttp_Register shall invoke URL_EncodeString with an argument of device id. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_028: [ IoTHubTransportHttp_Register shall invoke STRING_clone using the previously created hostname. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_030: [ IoTHubTransportHttp_Register shall invoke STRING_concat with arguments uriResource and the string "/devices/". ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_031: [ IoTHubTransportHttp_Register shall invoke STRING_concat_with_STRING with arguments uriResource and keyName. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_033: [ IoTHubTransportHttp_Register shall invoke STRING_construct with an argument of config->deviceConfig->deviceKey. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_035: [ The keyName is shortened to zero length, if that fails then IoTHubTransportHttp_Register shall fail and return NULL. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_036: [ IoTHubTransportHttp_Register shall invoke HTTPAPIEX_SAS_Create with arguments key, uriResource, and keyName. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_038: [ Otherwise, IoTHubTransportHttp_Register shall allocate the IOTHUB_DEVICE_HANDLE structure. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_040: [ IoTHubTransportHttp_Register shall put event HTTP relative path, message HTTP relative path, event HTTP request headers, message HTTP request headers, abandonHTTPrelativePathBegin, HTTPAPIEX_SAS_HANDLE, and the device handle into a device structure. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_128: [ IoTHubTransportHttp_Register shall mark this device as unsubscribed. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_041: [ IoTHubTransportHttp_Register shall call VECTOR_push_back to store the new device information. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_043: [ Upon success, IoTHubTransportHttp_Register shall store the transport handle, iotHubClientHandle, and the waitingToSend queue in the device handle return a non-NULL value. ]
TEST_FUNCTION(IoTHubTransportHttp_Register_HappyPath_with_deviceKey_success_fun_time)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;

    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    mocks.ResetAllCalls();

    setupRegisterHappyPath(mocks, false);

    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    ///assert

    ASSERT_IS_NOT_NULL(devHandle);
    ASSERT_ARE_NOT_EQUAL(void_ptr, handle, devHandle);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);

}

//Tests_SRS_TRANSPORTMULTITHTTP_17_133: [ IoTHubTransportHttp_Register shall create an immutable string (further called "deviceId") from config->deviceConfig->deviceId. ]
//Tests_SRS_TRANSPORTMULTITHTTP_03_135: [ IoTHubTransportHttp_Register shall create an immutable string (further called "deviceSasToken") from deviceSasToken.]
//Tests_SRS_TRANSPORTMULTITHTTP_17_017: [ IoTHubTransportHttp_Register shall create an immutable string (further called "event HTTP relative path") from the following pieces: "/devices/" + URL_ENCODED(config->deviceConfig->deviceId) + "/messages/events" + APIVERSION. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_017: [ IoTHubTransportHttp_Register shall create an immutable string (further called "event HTTP relative path") from the following pieces: "/devices/" + URL_ENCODED(config->deviceConfig->deviceId) + "/messages/events" + APIVERSION. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_021: [ IoTHubTransportHttp_Register shall create a set of HTTP headers (further called "event HTTP request headers") consisting of the following fixed field names and values: "iothub-to":"/devices/" + URL_ENCODED(config->deviceConfig->deviceId) + "/messages/events"; "Authorization":""
//"Accept":"application/json"
//"Connection" : "Keep-Alive" ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_132: [ IoTHubTransportHttp_Register shall create a set of HTTP headers (further called "message HTTP request headers") consisting of the following fixed field names and values:
//"Authorization": "" ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_024: [ IoTHubTransportHttp_Register shall create a STRING containing: "/devices/" + URL_ENCODED(device id) +"/messages/deviceBound/" called abandonHTTPrelativePathBegin. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_026: [ IoTHubTransportHttp_Register shall invoke URL_EncodeString with an argument of device id. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_028: [ IoTHubTransportHttp_Register shall invoke STRING_clone using the previously created hostname. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_030: [ IoTHubTransportHttp_Register shall invoke STRING_concat with arguments uriResource and the string "/devices/". ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_031: [ IoTHubTransportHttp_Register shall invoke STRING_concat_with_STRING with arguments uriResource and keyName. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_033: [ IoTHubTransportHttp_Register shall invoke STRING_construct with an argument of config->deviceConfig->deviceKey. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_035: [ The keyName is shortened to zero length, if that fails then IoTHubTransportHttp_Register shall fail and return NULL. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_036: [ IoTHubTransportHttp_Register shall invoke HTTPAPIEX_SAS_Create with arguments key, uriResource, and keyName. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_038: [ Otherwise, IoTHubTransportHttp_Register shall allocate the IOTHUB_DEVICE_HANDLE structure. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_040: [ IoTHubTransportHttp_Register shall put event HTTP relative path, message HTTP relative path, event HTTP request headers, message HTTP request headers, abandonHTTPrelativePathBegin, HTTPAPIEX_SAS_HANDLE, and the device handle into a device structure. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_128: [ IoTHubTransportHttp_Register shall mark this device as unsubscribed. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_041: [ IoTHubTransportHttp_Register shall call VECTOR_push_back to store the new device information. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_043: [ Upon success, IoTHubTransportHttp_Register shall store the transport handle, iotHubClientHandle, and the waitingToSend queue in the device handle return a non-NULL value. ]
TEST_FUNCTION(IoTHubTransportHttp_Register_HappyPath_with_deviceSas_success_fun_time)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;

    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    mocks.ResetAllCalls();

    setupRegisterHappyPathWithSasToken(mocks, false);

    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_3, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    ///assert

    ASSERT_IS_NOT_NULL(devHandle);
    ASSERT_ARE_NOT_EQUAL(void_ptr, handle, devHandle);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);

}

//Tests_SRS_TRANSPORTMULTITHTTP_17_043: [ Upon success, IoTHubTransportHttp_Register shall store the transport handle and the waitingToSend queue in the device handle return a non-NULL value. ]
TEST_FUNCTION(IoTHubTransportHttp_Register_2nd_device_HappyPath_success_fun_time)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;

    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    auto devHandle2 = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_2, TEST_IOTHUB_CLIENT_LL_HANDLE2, TEST_CONFIG2.waitingToSend);

    mocks.ResetAllCalls();

    // find in vector..
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    // actual register
    setupRegisterHappyPath(mocks, false);

    ///act 

    auto devHandle1 = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    ///assert

    ASSERT_IS_NOT_NULL(devHandle2);
    ASSERT_ARE_NOT_EQUAL(void_ptr, handle, devHandle1);
    ASSERT_ARE_NOT_EQUAL(void_ptr, devHandle1, devHandle2);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);

}

//Tests_SRS_TRANSPORTMULTITHTTP_17_137: [ IoTHubTransportHttp_Register shall search the devices list for any device matching name deviceId. If deviceId is found it shall return NULL. ]
TEST_FUNCTION(IoTHubTransportHttp_Register_sameDevice_twice_returns_null)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;

    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    (void)IoTHubTransportHttp_Register(handle, &TEST_DEVICE_2, TEST_IOTHUB_CLIENT_LL_HANDLE2, TEST_CONFIG2.waitingToSend);
    (void)IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    mocks.ResetAllCalls();

    // find in vector.. 2 and 1a
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, VECTOR_find_if(IGNORED_PTR_ARG, IGNORED_PTR_ARG, TEST_DEVICE_ID))
        .IgnoreAllArguments();

    ///act 

    auto devHandle1b = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    ///assert

    ASSERT_IS_NULL(devHandle1b);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);

}

//Tests_SRS_TRANSPORTMULTITHTTP_17_042: [ If the VECTOR_push_back fails then IoTHubTransportHttp_Register shall fail and return NULL. ]
TEST_FUNCTION(IoTHubTransportHttp_Register_vector_pushback_fails)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;

    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    mocks.ResetAllCalls();

    bool deallocateCreated = true;
    setupRegisterHappyPathNotFoundInList(mocks);
    setupRegisterHappyPathAllocHandle(mocks, deallocateCreated);
    setupRegisterHappyPathcreate_deviceId(mocks, deallocateCreated);
    setupRegisterHappyPathcreate_deviceKey(mocks, deallocateCreated);
    setupRegisterHappyPatheventHTTPrelativePath(mocks, deallocateCreated);
    setupRegisterHappyPathmessageHTTPrelativePath(mocks, deallocateCreated);
    setupRegisterHappyPatheventHTTPrequestHeaders(mocks, deallocateCreated);
    setupRegisterHappyPathmessageHTTPrequestHeaders(mocks, deallocateCreated);
    setupRegisterHappyPathabandonHTTPrelativePathBegin(mocks, deallocateCreated);
    setupRegisterHappyPathsasObject(mocks, deallocateCreated);
    whenShallVECTOR_push_back_fail = 1;
    setupRegisterHappyPathDeviceListAdd(mocks);

    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    ///assert

    ASSERT_IS_NULL(devHandle);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_037: [ If the HTTPAPIEX_SAS_Create fails then IoTHubTransportHttp_Register shall fail and return NULL. ]
TEST_FUNCTION(IoTHubTransportHttp_Register_createSASObject_fails_1)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    mocks.ResetAllCalls();

    bool deallocateCreated = true;
    setupRegisterHappyPathNotFoundInList(mocks);
    setupRegisterHappyPathAllocHandle(mocks, deallocateCreated);
    setupRegisterHappyPathcreate_deviceId(mocks, deallocateCreated);
    setupRegisterHappyPathcreate_deviceKey(mocks, deallocateCreated);
    setupRegisterHappyPatheventHTTPrelativePath(mocks, deallocateCreated);
    setupRegisterHappyPathmessageHTTPrelativePath(mocks, deallocateCreated);
    setupRegisterHappyPatheventHTTPrequestHeaders(mocks, deallocateCreated);
    setupRegisterHappyPathmessageHTTPrequestHeaders(mocks, deallocateCreated);
    setupRegisterHappyPathabandonHTTPrelativePathBegin(mocks, deallocateCreated);
    STRICT_EXPECTED_CALL(mocks, URL_EncodeString(TEST_DEVICE_ID)).SetReturn((STRING_HANDLE)NULL);

    ///act
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    ///assert
    ASSERT_IS_NULL(devHandle);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_037: [ If the HTTPAPIEX_SAS_Create fails then IoTHubTransportHttp_Register shall fail and return NULL. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_029: [ If the clone fails then IoTHubTransportHttp_Register shall fail and return NULL. ] 
TEST_FUNCTION(IoTHubTransportHttp_Register_createSASObject_fails_2)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    mocks.ResetAllCalls();

    bool deallocateCreated = true;
    setupRegisterHappyPathNotFoundInList(mocks);
    setupRegisterHappyPathAllocHandle(mocks, deallocateCreated);
    setupRegisterHappyPathcreate_deviceId(mocks, deallocateCreated);
    setupRegisterHappyPathcreate_deviceKey(mocks, deallocateCreated);
    setupRegisterHappyPatheventHTTPrelativePath(mocks, deallocateCreated);
    setupRegisterHappyPathmessageHTTPrelativePath(mocks, deallocateCreated);
    setupRegisterHappyPatheventHTTPrequestHeaders(mocks, deallocateCreated);
    setupRegisterHappyPathmessageHTTPrequestHeaders(mocks, deallocateCreated);
    setupRegisterHappyPathabandonHTTPrelativePathBegin(mocks, deallocateCreated);
    STRICT_EXPECTED_CALL(mocks, URL_EncodeString(TEST_DEVICE_ID));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG)) /*encoded device id*/
        .IgnoreArgument(1);
    whenShallSTRING_clone_fail = 1;
    STRICT_EXPECTED_CALL(mocks, STRING_clone(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    ///act
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    ///assert
    ASSERT_IS_NULL(devHandle);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);

}

//Tests_SRS_TRANSPORTMULTITHTTP_17_037: [ If the HTTPAPIEX_SAS_Create fails then IoTHubTransportHttp_Register shall fail and return NULL. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_141: [ If the STRING_concat fails, then it shall fail and return NULL. ]
TEST_FUNCTION(IoTHubTransportHttp_Register_createSASObject_fails_3)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    mocks.ResetAllCalls();

    bool deallocateCreated = true;
    setupRegisterHappyPathNotFoundInList(mocks);
    setupRegisterHappyPathAllocHandle(mocks, deallocateCreated);
    setupRegisterHappyPathcreate_deviceId(mocks, deallocateCreated);
    setupRegisterHappyPathcreate_deviceKey(mocks, deallocateCreated);
    setupRegisterHappyPatheventHTTPrelativePath(mocks, deallocateCreated);
    setupRegisterHappyPathmessageHTTPrelativePath(mocks, deallocateCreated);
    setupRegisterHappyPatheventHTTPrequestHeaders(mocks, deallocateCreated);
    setupRegisterHappyPathmessageHTTPrequestHeaders(mocks, deallocateCreated);
    setupRegisterHappyPathabandonHTTPrelativePathBegin(mocks, deallocateCreated);
    STRICT_EXPECTED_CALL(mocks, URL_EncodeString(TEST_DEVICE_ID));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG)) /*encoded device id*/
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_clone(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG)) /*uriResource*/
        .IgnoreArgument(1);

    whenShallSTRING_concat_fail = 7;
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, "/devices/"))
        .IgnoreArgument(1);

    ///act
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    ///assert
    ASSERT_IS_NULL(devHandle);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_037: [ If the HTTPAPIEX_SAS_Create fails then IoTHubTransportHttp_Register shall fail and return NULL. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_032: [ If the STRING_concat_with_STRING fails then IoTHubTransportHttp_Register shall fail and return NULL. ]
TEST_FUNCTION(IoTHubTransportHttp_Register_createSASObject_fails_4)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    mocks.ResetAllCalls();

    bool deallocateCreated = true;
    setupRegisterHappyPathNotFoundInList(mocks);
    setupRegisterHappyPathAllocHandle(mocks, deallocateCreated);
    setupRegisterHappyPathcreate_deviceId(mocks, deallocateCreated);
    setupRegisterHappyPathcreate_deviceKey(mocks, deallocateCreated);
    setupRegisterHappyPatheventHTTPrelativePath(mocks, deallocateCreated);
    setupRegisterHappyPathmessageHTTPrelativePath(mocks, deallocateCreated);
    setupRegisterHappyPatheventHTTPrequestHeaders(mocks, deallocateCreated);
    setupRegisterHappyPathmessageHTTPrequestHeaders(mocks, deallocateCreated);
    setupRegisterHappyPathabandonHTTPrelativePathBegin(mocks, deallocateCreated);
    STRICT_EXPECTED_CALL(mocks, URL_EncodeString(TEST_DEVICE_ID));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG)) /*encoded device id*/
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_clone(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG)) /*uriResource*/
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, "/devices/"))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .SetReturn(1);

    ///act
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    ///assert
    ASSERT_IS_NULL(devHandle);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_037: [ If the HTTPAPIEX_SAS_Create fails then IoTHubTransportHttp_Register shall fail and return NULL. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_034: [ If the STRING_construct fails then IoTHubTransportHttp_Register shall fail and return NULL. ] 
TEST_FUNCTION(IoTHubTransportHttp_Register_createSASObject_fails_5)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    mocks.ResetAllCalls();

    bool deallocateCreated = true;
    setupRegisterHappyPathNotFoundInList(mocks);
    setupRegisterHappyPathAllocHandle(mocks, deallocateCreated);
    setupRegisterHappyPathcreate_deviceId(mocks, deallocateCreated);
    setupRegisterHappyPathcreate_deviceKey(mocks, deallocateCreated);
    setupRegisterHappyPatheventHTTPrelativePath(mocks, deallocateCreated);
    setupRegisterHappyPathmessageHTTPrelativePath(mocks, deallocateCreated);
    setupRegisterHappyPatheventHTTPrequestHeaders(mocks, deallocateCreated);
    setupRegisterHappyPathmessageHTTPrequestHeaders(mocks, deallocateCreated);
    setupRegisterHappyPathabandonHTTPrelativePathBegin(mocks, deallocateCreated);
    STRICT_EXPECTED_CALL(mocks, URL_EncodeString(TEST_DEVICE_ID));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG)) /*encoded device id*/
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_clone(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG)) /*uriResource*/
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, "/devices/"))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    whenShallSTRING_construct_fail = 8;
    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_DEVICE_KEY));

    ///act
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    ///assert
    ASSERT_IS_NULL(devHandle);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_037: [ If the HTTPAPIEX_SAS_Create fails then IoTHubTransportHttp_Register shall fail and return NULL. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_035: [ The keyName is shortened to zero length, if that fails then IoTHubTransportHttp_Register shall fail and return NULL. ]
TEST_FUNCTION(IoTHubTransportHttp_Register_createSASObject_fails_6)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    mocks.ResetAllCalls();

    bool deallocateCreated = true;
    setupRegisterHappyPathNotFoundInList(mocks);
    setupRegisterHappyPathAllocHandle(mocks, deallocateCreated);
    setupRegisterHappyPathcreate_deviceId(mocks, deallocateCreated);
    setupRegisterHappyPathcreate_deviceKey(mocks, deallocateCreated);
    setupRegisterHappyPatheventHTTPrelativePath(mocks, deallocateCreated);
    setupRegisterHappyPathmessageHTTPrelativePath(mocks, deallocateCreated);
    setupRegisterHappyPatheventHTTPrequestHeaders(mocks, deallocateCreated);
    setupRegisterHappyPathmessageHTTPrequestHeaders(mocks, deallocateCreated);
    setupRegisterHappyPathabandonHTTPrelativePathBegin(mocks, deallocateCreated);
    STRICT_EXPECTED_CALL(mocks, URL_EncodeString(TEST_DEVICE_ID));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG)) /*encoded device id*/
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_clone(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG)) /*uriResource*/
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, "/devices/"))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_DEVICE_KEY));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG)) /*key*/
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_empty(IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .SetReturn(1);

    ///act
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    ///assert
    ASSERT_IS_NULL(devHandle);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_037: [ If the HTTPAPIEX_SAS_Create fails then IoTHubTransportHttp_Register shall fail and return NULL. ]
TEST_FUNCTION(IoTHubTransportHttp_Register_createSASObject_fails_7)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    mocks.ResetAllCalls();

    bool deallocateCreated = true;
    setupRegisterHappyPathNotFoundInList(mocks);
    setupRegisterHappyPathAllocHandle(mocks, deallocateCreated);
    setupRegisterHappyPathcreate_deviceId(mocks, deallocateCreated);
    setupRegisterHappyPathcreate_deviceKey(mocks, deallocateCreated);
    setupRegisterHappyPatheventHTTPrelativePath(mocks, deallocateCreated);
    setupRegisterHappyPathmessageHTTPrelativePath(mocks, deallocateCreated);
    setupRegisterHappyPatheventHTTPrequestHeaders(mocks, deallocateCreated);
    setupRegisterHappyPathmessageHTTPrequestHeaders(mocks, deallocateCreated);
    setupRegisterHappyPathabandonHTTPrelativePathBegin(mocks, deallocateCreated);
    STRICT_EXPECTED_CALL(mocks, URL_EncodeString(TEST_DEVICE_ID));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG)) /*encoded device id*/
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_clone(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG)) /*uriResource*/
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, "/devices/"))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_DEVICE_KEY));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG)) /*key*/
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_empty(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(3)
        .SetReturn((HTTPAPIEX_SAS_HANDLE)NULL);

    ///act
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    ///assert
    ASSERT_IS_NULL(devHandle);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_025: [ If creating the abandonHTTPrelativePathBegin fails then IoTHubTransportHttp_Register shall fail and return NULL. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_030: [ IoTHubTransportHttp_Register shall invoke STRING_concat with arguments uriResource and the string "/devices/". ]
TEST_FUNCTION(IoTHubTransportHttp_Register_abandonHTTPrelativePathBegin_fails_1)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    mocks.ResetAllCalls();

    bool deallocateCreated = true;
    setupRegisterHappyPathNotFoundInList(mocks);
    setupRegisterHappyPathAllocHandle(mocks, deallocateCreated);
    setupRegisterHappyPathcreate_deviceId(mocks, deallocateCreated);
    setupRegisterHappyPathcreate_deviceKey(mocks, deallocateCreated);
    setupRegisterHappyPatheventHTTPrelativePath(mocks, deallocateCreated);
    setupRegisterHappyPathmessageHTTPrelativePath(mocks, deallocateCreated);
    setupRegisterHappyPatheventHTTPrequestHeaders(mocks, deallocateCreated);
    setupRegisterHappyPathmessageHTTPrequestHeaders(mocks, deallocateCreated);
    /*creating abandonHTTPrelativePathBegin*/
    STRICT_EXPECTED_CALL(mocks, STRING_construct("/devices/"));
    STRICT_EXPECTED_CALL(mocks, URL_EncodeString(TEST_DEVICE_ID));
    STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, MESSAGE_ENDPOINT_HTTP_ETAG))
        .IgnoreArgument(1)
        .SetReturn(1);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG)) /*url encoded device id*/
        .IgnoreArgument(1);

    ///act
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    ///assert
    ASSERT_IS_NULL(devHandle);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_025: [ If creating the abandonHTTPrelativePathBegin fails then IoTHubTransportHttp_Register shall fail and return NULL. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_032: [ If the STRING_concat_with_STRING fails then IoTHubTransportHttp_Register shall fail and return NULL. ]
TEST_FUNCTION(IoTHubTransportHttp_Register_abandonHTTPrelativePathBegin_fails_2)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    mocks.ResetAllCalls();

    bool deallocateCreated = true;
    setupRegisterHappyPathNotFoundInList(mocks);
    setupRegisterHappyPathAllocHandle(mocks, deallocateCreated);
    setupRegisterHappyPathcreate_deviceId(mocks, deallocateCreated);
    setupRegisterHappyPathcreate_deviceKey(mocks, deallocateCreated);
    setupRegisterHappyPatheventHTTPrelativePath(mocks, deallocateCreated);
    setupRegisterHappyPathmessageHTTPrelativePath(mocks, deallocateCreated);
    setupRegisterHappyPatheventHTTPrequestHeaders(mocks, deallocateCreated);
    setupRegisterHappyPathmessageHTTPrequestHeaders(mocks, deallocateCreated);
    STRICT_EXPECTED_CALL(mocks, STRING_construct("/devices/"));
    STRICT_EXPECTED_CALL(mocks, URL_EncodeString(TEST_DEVICE_ID));
    STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .SetReturn(1);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG)) /*url encoded device id*/
        .IgnoreArgument(1);

    ///act
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    ///assert
    ASSERT_IS_NULL(devHandle);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_025: [ If creating the abandonHTTPrelativePathBegin fails then IoTHubTransportHttp_Register shall fail and return NULL. ]
TEST_FUNCTION(IoTHubTransportHttp_Register_abandonHTTPrelativePathBegin_fails_3)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    mocks.ResetAllCalls();

    bool deallocateCreated = true;
    setupRegisterHappyPathNotFoundInList(mocks);
    setupRegisterHappyPathAllocHandle(mocks, deallocateCreated);
    setupRegisterHappyPathcreate_deviceId(mocks, deallocateCreated);
    setupRegisterHappyPathcreate_deviceKey(mocks, deallocateCreated);
    setupRegisterHappyPatheventHTTPrelativePath(mocks, deallocateCreated);
    setupRegisterHappyPathmessageHTTPrelativePath(mocks, deallocateCreated);
    setupRegisterHappyPatheventHTTPrequestHeaders(mocks, deallocateCreated);
    setupRegisterHappyPathmessageHTTPrequestHeaders(mocks, deallocateCreated);
    /*creating abandonHTTPrelativePathBegin*/
    whenShallSTRING_construct_fail = 7;
    STRICT_EXPECTED_CALL(mocks, STRING_construct("/devices/"));

    ///act
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    ///assert
    ASSERT_IS_NULL(devHandle);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_025: [ If creating the abandonHTTPrelativePathBegin fails then IoTHubTransportHttp_Register shall fail and return NULL. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_027: [ If the encode fails then IoTHubTransportHttp_Register shall fail and return NULL. ] 
TEST_FUNCTION(IoTHubTransportHttp_Register_abandonHTTPrelativePathBegin_fails_4)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    mocks.ResetAllCalls();

    bool deallocateCreated = true;
    setupRegisterHappyPathNotFoundInList(mocks);
    setupRegisterHappyPathAllocHandle(mocks, deallocateCreated);
    setupRegisterHappyPathcreate_deviceId(mocks, deallocateCreated);
    setupRegisterHappyPathcreate_deviceKey(mocks, deallocateCreated);
    setupRegisterHappyPatheventHTTPrelativePath(mocks, deallocateCreated);
    setupRegisterHappyPathmessageHTTPrelativePath(mocks, deallocateCreated);
    setupRegisterHappyPatheventHTTPrequestHeaders(mocks, deallocateCreated);
    setupRegisterHappyPathmessageHTTPrequestHeaders(mocks, deallocateCreated);
    /*creating abandonHTTPrelativePathBegin*/
    STRICT_EXPECTED_CALL(mocks, STRING_construct("/devices/"));
    STRICT_EXPECTED_CALL(mocks, URL_EncodeString(TEST_DEVICE_ID)).SetReturn((STRING_HANDLE)NULL);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    ///act
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    ///assert
    ASSERT_IS_NULL(devHandle);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_023: [ If creating message HTTP request headers then IoTHubTransportHttp_Register shall fail and return NULL. ]
TEST_FUNCTION(IoTHubTransportHttp_Register_messageHTTPrequestheaders_fails_1)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    mocks.ResetAllCalls();

    bool deallocateCreated = true;
    setupRegisterHappyPathNotFoundInList(mocks);
    setupRegisterHappyPathAllocHandle(mocks, deallocateCreated);
    setupRegisterHappyPathcreate_deviceId(mocks, deallocateCreated);
    setupRegisterHappyPathcreate_deviceKey(mocks, deallocateCreated);
    setupRegisterHappyPatheventHTTPrelativePath(mocks, deallocateCreated);
    setupRegisterHappyPathmessageHTTPrelativePath(mocks, deallocateCreated);
    setupRegisterHappyPatheventHTTPrequestHeaders(mocks, deallocateCreated);
    /*creating message HTTP request headers*/
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc());
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "User-Agent", CLIENT_DEVICE_TYPE_PREFIX CLIENT_DEVICE_BACKSLASH IOTHUB_SDK_VERSION))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "Authorization", TEST_BLANK_SAS_TOKEN))
        .IgnoreArgument(1)
        .SetReturn(HTTP_HEADERS_ERROR);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    ///act
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    ///assert
    ASSERT_IS_NULL(devHandle);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_023: [ If creating message HTTP request headers then IoTHubTransportHttp_Register shall fail and return NULL. ]
TEST_FUNCTION(IoTHubTransportHttp_Register_messageHTTPrequestheaders_fails_2)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    mocks.ResetAllCalls();

    bool deallocateCreated = true;
    setupRegisterHappyPathNotFoundInList(mocks);
    setupRegisterHappyPathAllocHandle(mocks, deallocateCreated);
    setupRegisterHappyPathcreate_deviceId(mocks, deallocateCreated);
    setupRegisterHappyPathcreate_deviceKey(mocks, deallocateCreated);
    setupRegisterHappyPatheventHTTPrelativePath(mocks, deallocateCreated);
    setupRegisterHappyPathmessageHTTPrelativePath(mocks, deallocateCreated);
    setupRegisterHappyPatheventHTTPrequestHeaders(mocks, deallocateCreated);
    /*creating message HTTP request headers*/
    whenShallHTTPHeaders_Alloc_fail = 2;
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc());

    ///act
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    ///assert
    ASSERT_IS_NULL(devHandle);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_022: [ If creating the event HTTP request headers fails, then IoTHubTransportHttp_Register shall fail and return NULL.]
TEST_FUNCTION(IoTHubTransportHttp_Register_eventHTTPrequestHeaders_fails_1)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    mocks.ResetAllCalls();

    bool deallocateCreated = true;
    setupRegisterHappyPathNotFoundInList(mocks);
    setupRegisterHappyPathAllocHandle(mocks, deallocateCreated);
    setupRegisterHappyPathcreate_deviceId(mocks, deallocateCreated);
    setupRegisterHappyPathcreate_deviceKey(mocks, deallocateCreated);
    setupRegisterHappyPatheventHTTPrelativePath(mocks, deallocateCreated);
    setupRegisterHappyPathmessageHTTPrelativePath(mocks, deallocateCreated);
    /*creating eventHTTPrequestHeaders*/
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc());
    STRICT_EXPECTED_CALL(mocks, STRING_construct("/devices/"));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, URL_EncodeString(TEST_DEVICE_ID));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, EVENT_ENDPOINT))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "iothub-to", "/devices/"  TEST_DEVICE_ID  EVENT_ENDPOINT))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "Authorization", TEST_BLANK_SAS_TOKEN))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "Accept", "application/json"))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "Connection", "Keep-Alive"))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "User-Agent", CLIENT_DEVICE_TYPE_PREFIX CLIENT_DEVICE_BACKSLASH IOTHUB_SDK_VERSION))
        .IgnoreArgument(1)
        .SetReturn(HTTP_HEADERS_ERROR);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    ///act
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    ///assert
    ASSERT_IS_NULL(devHandle);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_022: [ If creating the event HTTP request headers fails, then IoTHubTransportHttp_Register shall fail and return NULL.]
TEST_FUNCTION(IoTHubTransportHttp_Register_eventHTTPrequestHeaders_fails_2)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    mocks.ResetAllCalls();

    bool deallocateCreated = true;
    setupRegisterHappyPathNotFoundInList(mocks);
    setupRegisterHappyPathAllocHandle(mocks, deallocateCreated);
    setupRegisterHappyPathcreate_deviceId(mocks, deallocateCreated);
    setupRegisterHappyPathcreate_deviceKey(mocks, deallocateCreated);
    setupRegisterHappyPatheventHTTPrelativePath(mocks, deallocateCreated);
    setupRegisterHappyPathmessageHTTPrelativePath(mocks, deallocateCreated);
    /*creating eventHTTPrequestHeaders*/
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc());
    STRICT_EXPECTED_CALL(mocks, STRING_construct("/devices/"));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, URL_EncodeString(TEST_DEVICE_ID));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, EVENT_ENDPOINT))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "iothub-to", "/devices/"  TEST_DEVICE_ID  EVENT_ENDPOINT))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "Authorization", TEST_BLANK_SAS_TOKEN))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "Accept", "application/json"))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "Connection", "Keep-Alive"))
        .IgnoreArgument(1)
        .SetReturn(HTTP_HEADERS_ERROR);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    ///act
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    ///assert
    ASSERT_IS_NULL(devHandle);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_022: [ If creating the event HTTP request headers fails, then IoTHubTransportHttp_Register shall fail and return NULL.]
TEST_FUNCTION(IoTHubTransportHttp_Register_eventHTTPrequestHeaders_fails_3)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    mocks.ResetAllCalls();

    bool deallocateCreated = true;
    setupRegisterHappyPathNotFoundInList(mocks);
    setupRegisterHappyPathAllocHandle(mocks, deallocateCreated);
    setupRegisterHappyPathcreate_deviceId(mocks, deallocateCreated);
    setupRegisterHappyPathcreate_deviceKey(mocks, deallocateCreated);
    setupRegisterHappyPatheventHTTPrelativePath(mocks, deallocateCreated);
    setupRegisterHappyPathmessageHTTPrelativePath(mocks, deallocateCreated);

    /*creating eventHTTPrequestHeaders*/
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc());
    STRICT_EXPECTED_CALL(mocks, STRING_construct("/devices/"));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, URL_EncodeString(TEST_DEVICE_ID));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, EVENT_ENDPOINT))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "iothub-to", "/devices/"  TEST_DEVICE_ID  EVENT_ENDPOINT))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "Authorization", TEST_BLANK_SAS_TOKEN))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "Accept", "application/json"))
        .IgnoreArgument(1)
        .SetReturn(HTTP_HEADERS_ERROR);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    ///act
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    ///assert
    ASSERT_IS_NULL(devHandle);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_022: [ If creating the event HTTP request headers fails, then IoTHubTransportHttp_Register shall fail and return NULL.]
TEST_FUNCTION(IoTHubTransportHttp_Register_eventHTTPrequestHeaders_fails_4)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    mocks.ResetAllCalls();

    bool deallocateCreated = true;
    setupRegisterHappyPathNotFoundInList(mocks);
    setupRegisterHappyPathAllocHandle(mocks, deallocateCreated);
    setupRegisterHappyPathcreate_deviceId(mocks, deallocateCreated);
    setupRegisterHappyPathcreate_deviceKey(mocks, deallocateCreated);
    setupRegisterHappyPatheventHTTPrelativePath(mocks, deallocateCreated);
    setupRegisterHappyPathmessageHTTPrelativePath(mocks, deallocateCreated);

    /*creating eventHTTPrequestHeaders*/
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc());
    STRICT_EXPECTED_CALL(mocks, STRING_construct("/devices/"));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, URL_EncodeString(TEST_DEVICE_ID));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, EVENT_ENDPOINT))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "iothub-to", "/devices/"  TEST_DEVICE_ID  EVENT_ENDPOINT))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "Authorization", TEST_BLANK_SAS_TOKEN))
        .IgnoreArgument(1)
        .SetReturn(HTTP_HEADERS_ERROR);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    ///act
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    ///assert
    ASSERT_IS_NULL(devHandle);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_022: [ If creating the event HTTP request headers fails, then IoTHubTransportHttp_Register shall fail and return NULL.]
TEST_FUNCTION(IoTHubTransportHttp_Register_eventHTTPrequestHeaders_fails_5)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    mocks.ResetAllCalls();

    bool deallocateCreated = true;
    setupRegisterHappyPathNotFoundInList(mocks);
    setupRegisterHappyPathAllocHandle(mocks, deallocateCreated);
    setupRegisterHappyPathcreate_deviceId(mocks, deallocateCreated);
    setupRegisterHappyPathcreate_deviceKey(mocks, deallocateCreated);
    setupRegisterHappyPatheventHTTPrelativePath(mocks, deallocateCreated);
    setupRegisterHappyPathmessageHTTPrelativePath(mocks, deallocateCreated);

    /*creating eventHTTPrequestHeaders*/
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc());
    STRICT_EXPECTED_CALL(mocks, STRING_construct("/devices/"));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, URL_EncodeString(TEST_DEVICE_ID));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, EVENT_ENDPOINT))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "iothub-to", "/devices/"  TEST_DEVICE_ID  EVENT_ENDPOINT))
        .IgnoreArgument(1)
        .SetReturn(HTTP_HEADERS_ERROR);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    ///act
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    ///assert
    ASSERT_IS_NULL(devHandle);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_022: [ If creating the event HTTP request headers fails, then IoTHubTransportHttp_Register shall fail and return NULL.]
TEST_FUNCTION(IoTHubTransportHttp_Register_eventHTTPrequestHeaders_fails_6)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    mocks.ResetAllCalls();
    currentSTRING_concat_call = 0;

    bool deallocateCreated = true;
    setupRegisterHappyPathNotFoundInList(mocks);
    setupRegisterHappyPathAllocHandle(mocks, deallocateCreated);
    setupRegisterHappyPathcreate_deviceId(mocks, deallocateCreated);
    setupRegisterHappyPathcreate_deviceKey(mocks, deallocateCreated);
    setupRegisterHappyPatheventHTTPrelativePath(mocks, deallocateCreated);
    setupRegisterHappyPathmessageHTTPrelativePath(mocks, deallocateCreated);

    /*creating eventHTTPrequestHeaders*/
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc());
    STRICT_EXPECTED_CALL(mocks, STRING_construct("/devices/"));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, URL_EncodeString(TEST_DEVICE_ID));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    whenShallSTRING_concat_fail = 3;
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, EVENT_ENDPOINT))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    ///act
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    ///assert
    ASSERT_IS_NULL(devHandle);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_022: [ If creating the event HTTP request headers fails, then IoTHubTransportHttp_Register shall fail and return NULL.]
TEST_FUNCTION(IoTHubTransportHttp_Register_eventHTTPrequestHeaders_fails_7)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    mocks.ResetAllCalls();
    currentSTRING_concat_with_STRING_call = 0;

    bool deallocateCreated = true;
    setupRegisterHappyPathNotFoundInList(mocks);
    setupRegisterHappyPathAllocHandle(mocks, deallocateCreated);
    setupRegisterHappyPathcreate_deviceId(mocks, deallocateCreated);
    setupRegisterHappyPathcreate_deviceKey(mocks, deallocateCreated);
    setupRegisterHappyPatheventHTTPrelativePath(mocks, deallocateCreated);
    setupRegisterHappyPathmessageHTTPrelativePath(mocks, deallocateCreated);

    /*creating eventHTTPrequestHeaders*/
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc());
    STRICT_EXPECTED_CALL(mocks, STRING_construct("/devices/"));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, URL_EncodeString(TEST_DEVICE_ID));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    whenShallSTRING_concat_with_STRING_fail = 3;
    STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    ///act
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    ///assert
    ASSERT_IS_NULL(devHandle);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_022: [ If creating the event HTTP request headers fails, then IoTHubTransportHttp_Register shall fail and return NULL.]
TEST_FUNCTION(IoTHubTransportHttp_Register_eventHTTPrequestHeaders_fails_8)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    mocks.ResetAllCalls();

    bool deallocateCreated = true;
    setupRegisterHappyPathNotFoundInList(mocks);
    setupRegisterHappyPathAllocHandle(mocks, deallocateCreated);
    setupRegisterHappyPathcreate_deviceId(mocks, deallocateCreated);
    setupRegisterHappyPathcreate_deviceKey(mocks, deallocateCreated);
    setupRegisterHappyPatheventHTTPrelativePath(mocks, deallocateCreated);
    setupRegisterHappyPathmessageHTTPrelativePath(mocks, deallocateCreated);

    /*creating eventHTTPrequestHeaders*/
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc());
    STRICT_EXPECTED_CALL(mocks, STRING_construct("/devices/"));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    whenShallURL_Encode_String_fail = 3;
    STRICT_EXPECTED_CALL(mocks, URL_EncodeString(TEST_DEVICE_ID));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    ///act
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    ///assert
    ASSERT_IS_NULL(devHandle);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_022: [ If creating the event HTTP request headers fails, then IoTHubTransportHttp_Register shall fail and return NULL.]
TEST_FUNCTION(IoTHubTransportHttp_Register_eventHTTPrequestHeaders_fails_9)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    mocks.ResetAllCalls();
    currentSTRING_construct_call = 0;

    bool deallocateCreated = true;
    setupRegisterHappyPathNotFoundInList(mocks);
    setupRegisterHappyPathAllocHandle(mocks, deallocateCreated);
    setupRegisterHappyPathcreate_deviceId(mocks, deallocateCreated);
    setupRegisterHappyPathcreate_deviceKey(mocks, deallocateCreated);
    setupRegisterHappyPatheventHTTPrelativePath(mocks, deallocateCreated);
    setupRegisterHappyPathmessageHTTPrelativePath(mocks, deallocateCreated);

    /*creating eventHTTPrequestHeaders*/
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc());
    whenShallSTRING_construct_fail = 5;
    STRICT_EXPECTED_CALL(mocks, STRING_construct("/devices/"));
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    ///act
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    ///assert
    ASSERT_IS_NULL(devHandle);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_022: [ If creating the event HTTP request headers fails, then IoTHubTransportHttp_Register shall fail and return NULL.]
TEST_FUNCTION(IoTHubTransportHttp_Register_eventHTTPrequestHeaders_fails_10)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    mocks.ResetAllCalls();

    bool deallocateCreated = true;
    setupRegisterHappyPathNotFoundInList(mocks);
    setupRegisterHappyPathAllocHandle(mocks, deallocateCreated);
    setupRegisterHappyPathcreate_deviceId(mocks, deallocateCreated);
    setupRegisterHappyPathcreate_deviceKey(mocks, deallocateCreated);
    setupRegisterHappyPatheventHTTPrelativePath(mocks, deallocateCreated);
    setupRegisterHappyPathmessageHTTPrelativePath(mocks, deallocateCreated);

    /*creating eventHTTPrequestHeaders*/
    whenShallHTTPHeaders_Alloc_fail = 1;
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc());

    ///act
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    ///assert
    ASSERT_IS_NULL(devHandle);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_020: [ If creating the message HTTP relative path fails, then IoTHubTransportHttp_Register shall fail and return NULL. ]
TEST_FUNCTION(IoTHubTransportHttp_Register_messageHTTPrelativePath_fails_1)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    mocks.ResetAllCalls();
    currentSTRING_concat_call = 0;

    bool deallocateCreated = true;
    setupRegisterHappyPathNotFoundInList(mocks);
    setupRegisterHappyPathAllocHandle(mocks, deallocateCreated);
    setupRegisterHappyPathcreate_deviceId(mocks, deallocateCreated);
    setupRegisterHappyPathcreate_deviceKey(mocks, deallocateCreated);
    setupRegisterHappyPatheventHTTPrelativePath(mocks, deallocateCreated);

    /*creating messageHTTPrelativePath*/
    STRICT_EXPECTED_CALL(mocks, STRING_construct("/devices/"));
    STRICT_EXPECTED_CALL(mocks, URL_EncodeString(TEST_DEVICE_ID));
    STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    whenShallSTRING_concat_fail = 2;
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, MESSAGE_ENDPOINT_HTTP API_VERSION))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    /*the url encoded device id*/
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    ///act
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    ///assert
    ASSERT_IS_NULL(devHandle);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_020: [ If creating the message HTTP relative path fails, then IoTHubTransportHttp_Register shall fail and return NULL. ]
TEST_FUNCTION(IoTHubTransportHttp_Register_messageHTTPrelativePath_fails_2)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    mocks.ResetAllCalls();

    bool deallocateCreated = true;
    setupRegisterHappyPathNotFoundInList(mocks);
    setupRegisterHappyPathAllocHandle(mocks, deallocateCreated);
    setupRegisterHappyPathcreate_deviceId(mocks, deallocateCreated);
    setupRegisterHappyPathcreate_deviceKey(mocks, deallocateCreated);
    setupRegisterHappyPatheventHTTPrelativePath(mocks, deallocateCreated);


    /*creating messageHTTPrelativePath*/
    STRICT_EXPECTED_CALL(mocks, STRING_construct("/devices/"));
    STRICT_EXPECTED_CALL(mocks, URL_EncodeString(TEST_DEVICE_ID));
    whenShallSTRING_concat_with_STRING_fail = 2;
    STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    /*the url encoded device id*/
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    ///act
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    ///assert
    ASSERT_IS_NULL(devHandle);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_020: [ If creating the message HTTP relative path fails, then IoTHubTransportHttp_Register shall fail and return NULL. ]
TEST_FUNCTION(IoTHubTransportHttp_Register_messageHTTPrelativePath_fails_3)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    mocks.ResetAllCalls();

    bool deallocateCreated = true;
    setupRegisterHappyPathNotFoundInList(mocks);
    setupRegisterHappyPathAllocHandle(mocks, deallocateCreated);
    setupRegisterHappyPathcreate_deviceId(mocks, deallocateCreated);
    setupRegisterHappyPathcreate_deviceKey(mocks, deallocateCreated);
    setupRegisterHappyPatheventHTTPrelativePath(mocks, deallocateCreated);

    /*creating messageHTTPrelativePath*/
    STRICT_EXPECTED_CALL(mocks, STRING_construct("/devices/"));
    whenShallURL_Encode_String_fail = 2;
    STRICT_EXPECTED_CALL(mocks, URL_EncodeString(TEST_DEVICE_ID));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    /*the url encoded device id*/
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    ///act
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    ///assert
    ASSERT_IS_NULL(devHandle);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_020: [ If creating the message HTTP relative path fails, then IoTHubTransportHttp_Register shall fail and return NULL. ]
TEST_FUNCTION(IoTHubTransportHttp_Register_messageHTTPrelativePath_fails_4)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    mocks.ResetAllCalls();
    currentSTRING_construct_call = 0;

    bool deallocateCreated = true;
    setupRegisterHappyPathNotFoundInList(mocks);
    setupRegisterHappyPathAllocHandle(mocks, deallocateCreated);
    setupRegisterHappyPathcreate_deviceId(mocks, deallocateCreated);
    setupRegisterHappyPathcreate_deviceKey(mocks, deallocateCreated);
    setupRegisterHappyPatheventHTTPrelativePath(mocks, deallocateCreated);

    /*creating messageHTTPrelativePath*/
    whenShallSTRING_construct_fail = 4;
    STRICT_EXPECTED_CALL(mocks, STRING_construct("/devices/"));

    ///act
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    ///assert
    ASSERT_IS_NULL(devHandle);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_018: [ If creating the string fail for any reason then IoTHubTransportHttp_Register shall fail and return NULL. ]
TEST_FUNCTION(IoTHubTransportHttp_Register_eventHTTPrelativePath_fails_1)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    mocks.ResetAllCalls();
    currentSTRING_concat_call = 0;

    bool deallocateCreated = true;
    setupRegisterHappyPathNotFoundInList(mocks);
    setupRegisterHappyPathAllocHandle(mocks, deallocateCreated);
    setupRegisterHappyPathcreate_deviceId(mocks, deallocateCreated);
    setupRegisterHappyPathcreate_deviceKey(mocks, deallocateCreated);

    /*creating eventHTTPrelativePath*/
    STRICT_EXPECTED_CALL(mocks, STRING_construct("/devices/"));
    STRICT_EXPECTED_CALL(mocks, URL_EncodeString(TEST_DEVICE_ID));
    STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    whenShallSTRING_concat_fail = 1;
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, EVENT_ENDPOINT API_VERSION))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    /*the url encoded device id*/
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    ///act
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    ///assert
    ASSERT_IS_NULL(devHandle);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_018: [ If creating the string fail for any reason then IoTHubTransportHttp_Register shall fail and return NULL. ]
TEST_FUNCTION(IoTHubTransportHttp_Register_eventHTTPrelativePath_fails_2)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    mocks.ResetAllCalls();

    bool deallocateCreated = true;
    setupRegisterHappyPathNotFoundInList(mocks);
    setupRegisterHappyPathAllocHandle(mocks, deallocateCreated);
    setupRegisterHappyPathcreate_deviceId(mocks, deallocateCreated);
    setupRegisterHappyPathcreate_deviceKey(mocks, deallocateCreated);

    /*creating eventHTTPrelativePath*/
    STRICT_EXPECTED_CALL(mocks, STRING_construct("/devices/"));
    STRICT_EXPECTED_CALL(mocks, URL_EncodeString(TEST_DEVICE_ID));
    whenShallSTRING_concat_with_STRING_fail = 1;
    STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    /*the url encoded device id*/
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    ///act
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    ///assert
    ASSERT_IS_NULL(devHandle);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_018: [ If creating the string fail for any reason then IoTHubTransportHttp_Register shall fail and return NULL. ]
TEST_FUNCTION(IoTHubTransportHttp_Register_eventHTTPrelativePath_fails_3)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    mocks.ResetAllCalls();
    currentSTRING_construct_call = 0;

    bool deallocateCreated = true;
    setupRegisterHappyPathNotFoundInList(mocks);
    setupRegisterHappyPathAllocHandle(mocks, deallocateCreated);
    setupRegisterHappyPathcreate_deviceId(mocks, deallocateCreated);
    setupRegisterHappyPathcreate_deviceKey(mocks, deallocateCreated);

    /*creating eventHTTPrelativePath*/
    whenShallSTRING_construct_fail = 3;
    STRICT_EXPECTED_CALL(mocks, STRING_construct("/devices/"));

    ///act
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    ///assert
    ASSERT_IS_NULL(devHandle);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_018: [ If creating the string fail for any reason then IoTHubTransportHttp_Register shall fail and return NULL. ]
TEST_FUNCTION(IoTHubTransportHttp_Register_eventHTTPrelativePath_fails_4)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    mocks.ResetAllCalls();

    bool deallocateCreated = true;
    setupRegisterHappyPathNotFoundInList(mocks);
    setupRegisterHappyPathAllocHandle(mocks, deallocateCreated);
    setupRegisterHappyPathcreate_deviceId(mocks, deallocateCreated);
    setupRegisterHappyPathcreate_deviceKey(mocks, deallocateCreated);

    /*creating eventHTTPrelativePath*/
    whenShallURL_Encode_String_fail = 1;
    STRICT_EXPECTED_CALL(mocks, STRING_construct("/devices/"));
    whenShallSTRING_concat_with_STRING_fail = 1;
    STRICT_EXPECTED_CALL(mocks, URL_EncodeString(TEST_DEVICE_ID));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    /*the url encoded device id*/
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    ///act
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    ///assert
    ASSERT_IS_NULL(devHandle);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_136: [ If deviceKey is not created, then IoTHubTransportHttp_Register shall fail and return NULL. ]
TEST_FUNCTION(IoTHubTransportHttp_Register_create_deviceKey_fails)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    mocks.ResetAllCalls();
    currentSTRING_construct_call = 0;

    bool deallocateCreated = true;
    setupRegisterHappyPathNotFoundInList(mocks);
    setupRegisterHappyPathAllocHandle(mocks, deallocateCreated);
    setupRegisterHappyPathcreate_deviceId(mocks, deallocateCreated);

    whenShallSTRING_construct_fail = 2;
    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_DEVICE_KEY));

    ///act
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    ///assert
    ASSERT_IS_NULL(devHandle);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_03_136: [ If deviceSasToken is not created, then IoTHubTransportHttp_Register shall fail and return NULL.]
TEST_FUNCTION(IoTHubTransportHttp_Register_create_deviceSasToken_fails)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    mocks.ResetAllCalls();
    currentSTRING_construct_call = 0;

    bool deallocateCreated = true;
    setupRegisterHappyPathNotFoundInList(mocks);
    setupRegisterHappyPathAllocHandle(mocks, deallocateCreated);
    setupRegisterHappyPathcreate_deviceId(mocks, deallocateCreated);

    whenShallSTRING_construct_fail = 2;
    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_DEVICE_TOKEN));

    ///act
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_3, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    ///assert
    ASSERT_IS_NULL(devHandle);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_134: [ If deviceId is not created, then IoTHubTransportHttp_Register shall fail and return NULL. ]
TEST_FUNCTION(IoTHubTransportHttp_Register_create_deviceId_fails)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    mocks.ResetAllCalls();
    currentSTRING_construct_call = 0;

    bool deallocateCreated = true;
    setupRegisterHappyPathNotFoundInList(mocks);
    setupRegisterHappyPathAllocHandle(mocks, deallocateCreated);

    whenShallSTRING_construct_fail = 1;
    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_DEVICE_ID));

    ///act
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    ///assert
    ASSERT_IS_NULL(devHandle);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_039: [ If the allocating the device handle fails then IoTHubTransportHttp_Register shall fail and return NULL. ]
TEST_FUNCTION(IoTHubTransportHttp_Register_deviceHandle_not_allocated)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    mocks.ResetAllCalls();
    currentmalloc_call = 0;
    setupRegisterHappyPathNotFoundInList(mocks);

    whenShallmalloc_fail = 1;
    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
        .IgnoreArgument(1);

    ///act
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    ///assert
    ASSERT_IS_NULL(devHandle);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_137: [ IoTHubTransportHttp_Register shall search the devices list for any device matching name deviceId. If deviceId is found it shall return NULL. ]
TEST_FUNCTION(IoTHubTransportHttp_Register_deviceFoundInList_fails)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, VECTOR_find_if(IGNORED_PTR_ARG, IGNORED_PTR_ARG, TEST_DEVICE_ID))
        .IgnoreAllArguments()
        .SetReturn((void_ptr)0x1);

    ///act
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    ///assert
    ASSERT_IS_NULL(devHandle);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_014: [ If parameter deviceId is NULL, then IoTHubTransportHttp_Register shall return NULL. ]
TEST_FUNCTION(IoTHubTransportHttp_Register_deviceId_null_returns_null)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    IOTHUB_DEVICE_CONFIG myDevice;
    myDevice.deviceId = NULL;
    myDevice.deviceKey = TEST_DEVICE_KEY;
    myDevice.deviceSasToken = NULL;

    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    mocks.ResetAllCalls();

    ///act
    auto devHandle = IoTHubTransportHttp_Register(handle, &myDevice, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    ///assert
    ASSERT_IS_NULL(devHandle);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);

}

//Tests_SRS_TRANSPORTMULTITHTTP_17_015: [ If IOTHUB_DEVICE_CONFIG fields deviceKey and deviceSasToken are NULL, then IoTHubTransportHttp_Register shall assume a x509 authentication. ]*/
TEST_FUNCTION(IoTHubTransportHttp_Register_deviceKey_null_and_deviceSasToken_succeeds)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    IOTHUB_DEVICE_CONFIG myDevice;
    myDevice.deviceId = TEST_DEVICE_ID;
    myDevice.deviceKey = NULL;
    myDevice.deviceSasToken = NULL;

    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    mocks.ResetAllCalls();

    setupRegisterHappyPath(mocks, false, true);

    ///act
    auto devHandle = IoTHubTransportHttp_Register(handle, &myDevice, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    ///assert
    ASSERT_IS_NOT_NULL(devHandle);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_143: [ If parameter iotHubClientHandle is NULL, then IoTHubTransportHttp_Register shall return NULL. ]
TEST_FUNCTION(IoTHubTransportHttp_Register_hubClient_null_returns_null)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    mocks.ResetAllCalls();

    ///act
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, NULL, TEST_CONFIG.waitingToSend);

    ///assert
    ASSERT_IS_NULL(devHandle);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);

}

//Tests_SRS_TRANSPORTMULTITHTTP_17_016: [ If parameter waitingToSend is NULL, then IoTHubTransportHttp_Register shall return NULL. ]
TEST_FUNCTION(IoTHubTransportHttp_Register_wts_null_returns_null)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    mocks.ResetAllCalls();

    ///act
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, NULL);

    ///assert
    ASSERT_IS_NULL(devHandle);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);

}

//Tests_SRS_TRANSPORTMULTITHTTP_17_142: [ If handle is NULL, then IoTHubTransportHttp_Register shall return NULL. ]
TEST_FUNCTION(IoTHubTransportHttp_Register_handle_null_returns_null)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;

    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    mocks.ResetAllCalls();

    ///act
    auto devHandle = IoTHubTransportHttp_Register(NULL, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    ///assert
    ASSERT_IS_NULL(devHandle);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);

}

//Tests_SRS_TRANSPORTMULTITHTTP_17_044: [ If deviceHandle is NULL, then IoTHubTransportHttp_Unregister shall do nothing. ]
TEST_FUNCTION(IoTHubTransportHttp_Unregister_null_does_nothing)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;

    ///act
    IoTHubTransportHttp_Unregister(NULL);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_045: [IoTHubTransportHttp_Unregister shall locate deviceHandle in the transport device list by calling VECTOR_find_if.]
//Tests_SRS_TRANSPORTMULTITHTTP_17_047 : [IoTHubTransportHttp_Unregister shall free all the resources used in the device structure.]
//Tests_SRS_TRANSPORTMULTITHTTP_17_048 : [IoTHubTransportHttp_Unregister shall call VECTOR_erase to remove device from devices list.]
TEST_FUNCTION(IoTHubTransportHttp_Unregister_superHappyFunPath)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    mocks.ResetAllCalls();


    STRICT_EXPECTED_CALL(mocks, VECTOR_find_if(IGNORED_PTR_ARG, IGNORED_PTR_ARG, devHandle))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    setupUnregisterOneDevice(mocks);
    STRICT_EXPECTED_CALL(mocks, VECTOR_erase(IGNORED_PTR_ARG, IGNORED_PTR_ARG, 1))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, gballoc_free(devHandle));

    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);                                             //STRING_HANDLE deviceSasToken;

    ///act
    IoTHubTransportHttp_Unregister(devHandle);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_045: [IoTHubTransportHttp_Unregister shall locate deviceHandle in the transport device list by calling VECTOR_find_if.]
//Tests_SRS_TRANSPORTMULTITHTTP_17_047 : [IoTHubTransportHttp_Unregister shall free all the resources used in the device structure.]
//Tests_SRS_TRANSPORTMULTITHTTP_17_048 : [IoTHubTransportHttp_Unregister shall call VECTOR_erase to remove device from devices list.]
TEST_FUNCTION(IoTHubTransportHttp_Unregister_2nd_device_superHappyFunPath)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    (void)IoTHubTransportHttp_Register(handle, &TEST_DEVICE_2, TEST_IOTHUB_CLIENT_LL_HANDLE2, TEST_CONFIG2.waitingToSend);
    auto devHandle1 = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    mocks.ResetAllCalls();


    STRICT_EXPECTED_CALL(mocks, VECTOR_find_if(IGNORED_PTR_ARG, IGNORED_PTR_ARG, devHandle1))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    setupUnregisterOneDevice(mocks);
    STRICT_EXPECTED_CALL(mocks, VECTOR_erase(IGNORED_PTR_ARG, IGNORED_PTR_ARG, 1))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, gballoc_free(devHandle1));

    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);                                             //STRING_HANDLE deviceSasToken;

    ///act
    IoTHubTransportHttp_Unregister(devHandle1);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_046 : [If the device structure is not found, then this function shall fail and do nothing.]
TEST_FUNCTION(IoTHubTransportHttp_Unregister_DeviceNotFound_fails)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    mocks.ResetAllCalls();


    STRICT_EXPECTED_CALL(mocks, VECTOR_find_if(IGNORED_PTR_ARG, IGNORED_PTR_ARG, devHandle))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .SetFailReturn((void_ptr)NULL);


    ///act
    IoTHubTransportHttp_Unregister(devHandle);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_103: [ If parameter deviceHandle is NULL then IoTHubTransportHttp_Subscribe shall fail and return a non-zero value. ]
TEST_FUNCTION(IoTHubTransportHttp_Subscribe_with_NULL_parameter_fails)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;

    ///act
    auto result = IoTHubTransportHttp_Subscribe(NULL);

    ///assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
}


//Tests_SRS_TRANSPORTMULTITHTTP_17_104: [ IoTHubTransportHttp_Subscribe shall locate deviceHandle in the transport device list by calling VECTOR_find_if. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_106: [ Otherwise, IoTHubTransportHttp_Subscribe shall set the device so that subsequent calls to DoWork should execute HTTP requests. 
TEST_FUNCTION(IoTHubTransportHttp_Subscribe_with_non_NULL_parameter_succeeds)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, VECTOR_find_if(IGNORED_PTR_ARG, IGNORED_PTR_ARG, devHandle))
        .IgnoreArgument(1)
        .IgnoreArgument(2);

    ///act
    auto result = IoTHubTransportHttp_Subscribe(devHandle);

    ///assert
    ASSERT_ARE_EQUAL(int, 0, result);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_104: [ IoTHubTransportHttp_Subscribe shall locate deviceHandle in the transport device list by calling VECTOR_find_if. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_106: [ Otherwise, IoTHubTransportHttp_Subscribe shall set the device so that subsequent calls to DoWork should execute HTTP requests. 
TEST_FUNCTION(IoTHubTransportHttp_Subscribe_2devices_succeeds)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    auto devHandle1 = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);
    auto devHandle2 = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_2, TEST_IOTHUB_CLIENT_LL_HANDLE2, TEST_CONFIG2.waitingToSend);

    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, VECTOR_find_if(IGNORED_PTR_ARG, IGNORED_PTR_ARG, devHandle1))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, VECTOR_find_if(IGNORED_PTR_ARG, IGNORED_PTR_ARG, devHandle2))
        .IgnoreArgument(1)
        .IgnoreArgument(2);

    ///act
    auto result1 = IoTHubTransportHttp_Subscribe(devHandle1);
    auto result2 = IoTHubTransportHttp_Subscribe(devHandle2);

    ///assert
    ASSERT_ARE_EQUAL(int, 0, result1);
    ASSERT_ARE_EQUAL(int, 0, result2);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_105: [ If the device structure is not found, then this function shall fail and return a non-zero value. ]
TEST_FUNCTION(IoTHubTransportHttp_Subscribe_with_list_find_fails)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, VECTOR_find_if(IGNORED_PTR_ARG, IGNORED_PTR_ARG, devHandle))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .SetReturn((void_ptr)NULL);

    ///act
    auto result = IoTHubTransportHttp_Subscribe(devHandle);

    ///assert
    ASSERT_ARE_EQUAL(int, 0, result);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_107: [ If parameter deviceHandle is NULL then IoTHubTransportHttp_Unsubscribe shall fail do nothing. ]
TEST_FUNCTION(IoTHubTransportHttp_Unsubscribe_with_NULL_parameter_succeeds) /*does nothing*/
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;

    ///act
    IoTHubTransportHttp_Unsubscribe(NULL);

    ///assert
    mocks.AssertActualAndExpectedCalls();

}

//Tests_SRS_TRANSPORTMULTITHTTP_17_108: [IoTHubTransportHttp_Unsubscribe shall locate deviceHandle in the transport device list by calling VECTOR_find_if.]
//Tests_SRS_TRANSPORTMULTITHTTP_17_110 : [Otherwise, IoTHubTransportHttp_Subscribe shall set the device so that subsequent calls to DoWork shall not execute HTTP requests.]
TEST_FUNCTION(IoTHubTransportHttp_Unsubscribe_with_non_NULL_parameter_succeeds)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, VECTOR_find_if(IGNORED_PTR_ARG, IGNORED_PTR_ARG, devHandle))
        .IgnoreArgument(1)
        .IgnoreArgument(2);

    ///act
    IoTHubTransportHttp_Unsubscribe(devHandle);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_108: [IoTHubTransportHttp_Unsubscribe shall locate deviceHandle in the transport device list by calling VECTOR_find_if.]
//Tests_SRS_TRANSPORTMULTITHTTP_17_110 : [Otherwise, IoTHubTransportHttp_Subscribe shall set the device so that subsequent calls to DoWork should not execute HTTP requests.]
TEST_FUNCTION(IoTHubTransportHttp_Unsubscribe_with_2devices_succeeds)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);
    auto devHandle2 = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_2, TEST_IOTHUB_CLIENT_LL_HANDLE2, TEST_CONFIG2.waitingToSend);

    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, VECTOR_find_if(IGNORED_PTR_ARG, IGNORED_PTR_ARG, devHandle))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, VECTOR_find_if(IGNORED_PTR_ARG, IGNORED_PTR_ARG, devHandle2))
        .IgnoreArgument(1)
        .IgnoreArgument(2);

    ///act
    IoTHubTransportHttp_Unsubscribe(devHandle);
    IoTHubTransportHttp_Unsubscribe(devHandle2);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_109 : [If the device structure is not found, then this function shall fail and do nothing.]
TEST_FUNCTION(IoTHubTransportHttp_Unsubscribe_with_device_not_found_does_nothing)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, VECTOR_find_if(IGNORED_PTR_ARG, IGNORED_PTR_ARG, devHandle))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .SetFailReturn((void_ptr)NULL);

    ///act
    IoTHubTransportHttp_Unsubscribe(devHandle);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_049: [ If handle is NULL, then IoTHubTransportHttp_DoWork shall do nothing. ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_with_NULL_handle_does_nothing)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;

    ///act
    IoTHubTransportHttp_DoWork(NULL, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_050: [ IoTHubTransportHttp_DoWork shall call loop through the device list. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_051: [IF the list is empty, then IoTHubTransportHttp_DoWork shall do nothing.]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_happy_path_with_no_devices)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, VECTOR_size(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    /*no message work because DoWork_PullMessage is set to false by create*/

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_050: [ IoTHubTransportHttp_DoWork shall call loop through the device list. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_051: [IF the list is empty, then IoTHubTransportHttp_DoWork shall do nothing.]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_happy_path_with_all_devices_unregistered)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    auto devHandle2 = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_2, TEST_IOTHUB_CLIENT_LL_HANDLE2, TEST_CONFIG2.waitingToSend);
    auto devHandle1 = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);
    (void)IoTHubTransportHttp_Unregister(devHandle1);
    (void)IoTHubTransportHttp_Unregister(devHandle2);
    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, VECTOR_size(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    /*no message work because DoWork_PullMessage is set to false by create*/

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_060: [ If the list is empty then IoTHubTransportHttp_DoWork shall proceed to the following action. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_083: [ If device is not subscribed then _DoWork shall advance to the next action. ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_happy_path_with_empty_waitingToSend_and_no_service_messages)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    (void)IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    mocks.ResetAllCalls();

    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend));

    /*no message work because DoWork_PullMessage is set to false by create*/

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_083: [ If device is not subscribed then _DoWork shall advance to the next action. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_110: [ Otherwise, IoTHubTransportHttp_Subscribe shall set the device so that subsequent calls to DoWork shall not execute HTTP requests. ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_happy_path_no_work_check_unsubscribe)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    auto devHandle1 = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);
    auto devHandle2 = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_2, TEST_IOTHUB_CLIENT_LL_HANDLE2, TEST_CONFIG2.waitingToSend);
    (void)IoTHubTransportHttp_Subscribe(devHandle2);
    (void)IoTHubTransportHttp_Subscribe(devHandle1);
    IoTHubTransportHttp_Unsubscribe(devHandle2);
    IoTHubTransportHttp_Unsubscribe(devHandle1);

    mocks.ResetAllCalls();

    setupDoWorkLoopOnceForOneDevice(mocks);
    setupDoWorkLoopForNextDevice(mocks, 1);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend));
    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend2));

    /*no message work because DoWork_PullMessage is unset due to unsubscribe*/

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_052: [ IoTHubTransportHttp_DoWork shall perform a round-robin loop through every deviceHandle in the transport device list, using the iotHubClientHandle field saved in the IOTHUB_DEVICE_HANDLE.]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_happy_path_with_2_registered_devices_empty_wts_no_service_msgs)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    (void)IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);
    (void)IoTHubTransportHttp_Register(handle, &TEST_DEVICE_2, TEST_IOTHUB_CLIENT_LL_HANDLE2, TEST_CONFIG2.waitingToSend);

    mocks.ResetAllCalls();

    setupDoWorkLoopOnceForOneDevice(mocks);
    setupDoWorkLoopForNextDevice(mocks, 1);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend));
    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend2));

    /*no message work because DoWork_PullMessage is set to false by create*/

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}


//Tests_SRS_TRANSPORTMULTITHTTP_17_084: [ Otherwise, IoTHubTransportHttp_DoWork shall call HTTPAPIEX_SAS_ExecuteRequest passing the following parameters
//requestType: GET
//	relativePath : the message HTTP relative path
//	requestHttpHeadersHandle : message HTTP request headers created by _Create
//	requestContent : NULL
//	statusCode : a pointer to unsigned int which shall be later examined
//	responseHeadearsHandle : a new instance of HTTP headers
//	responseContent : a new instance of buffer ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_087: [ If status code is 200, then _DoWork shall make a copy of the value of the "ETag" http header. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_089: [ _DoWork shall assemble an IOTHUBMESSAGE_HANDLE from the received HTTP content (using the responseContent buffer). ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_093: [ Otherwise, _DoWork shall call IoTHubClient_LL_MessageCallback with parameters handle = iotHubClientHandle and message = newly created message. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_094: [ If IoTHubClient_LL_MessageCallback returns IOTHUBMESSAGE_ACCEPTED then _DoWork shall "accept" the message. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_099: [ _DoWork shall call HTTPAPIEX_SAS_ExecuteRequest with the following parameters:
//
//requestType: DELETE
//	relativePath : abandon relative path begin + value of ETag + APIVERSION
//	requestHttpHeadersHandle : an HTTP headers instance containing the following
//	Authorization : " "
//	If - Match : value of ETag
//	requestContent : NULL
//	statusCode : a pointer to unsigned int which might be used by logging
//	responseHeadearsHandle : NULL
//	responseContent : NULL ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_100: [ Accepting a message is successful when HTTPAPIEX_SAS_ExecuteRequest completes successfully and the status code is 204. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_090: [ All the HTTP headers of the form iothub-app-name:somecontent shall be transformed in message properties {name, somecontent}. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_123: [ After client creation, the first GET shall be allowed no matter what the value of GetMinimumPollingTime. ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_happy_path_with_empty_waitingToSend_and_1_service_message_succeeds)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    unsigned int statusCode200 = 200;
    unsigned int statusCode204 = 204;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    (void)IoTHubTransportHttp_Subscribe(devHandle);
    mocks.ResetAllCalls();

    setupDoWorkLoopOnceForOneDevice(mocks);


    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend)); /*because DoWork for event*/

    STRICT_EXPECTED_CALL(mocks, get_time(NULL));
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc()); /*because responseHeadearsHandle: a new instance of HTTP headers*/
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, BUFFER_new());
    STRICT_EXPECTED_CALL(mocks, BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1); /*because relativePath is a STRING_HANDLE*/
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,                                    /*sasObject handle                                             */
        IGNORED_PTR_ARG,                                    /*HTTPAPIEX_HANDLE handle,                                     */
        HTTPAPI_REQUEST_GET,                                /*HTTPAPI_REQUEST_TYPE requestType,                            */
        "/devices/" TEST_DEVICE_ID MESSAGE_ENDPOINT_HTTP API_VERSION,    /*const char* relativePath,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,                */
        NULL,                                               /*BUFFER_HANDLE requestContent,                                */
        IGNORED_PTR_ARG,                                    /*unsigned int* statusCode,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,               */
        IGNORED_PTR_ARG                                     /*BUFFER_HANDLE responseContent))                              */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(5)
        .IgnoreArgument(7)
        .IgnoreArgument(8)
        .IgnoreArgument(9)
        .CopyOutArgumentBuffer(7, &statusCode200, sizeof(statusCode200));

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_FindHeaderValue(IGNORED_PTR_ARG, "ETag"))
        .IgnoreArgument(1)
        .SetReturn(TEST_ETAG_VALUE);

    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_CreateFromByteArray(IGNORED_PTR_ARG, IGNORED_NUM_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        ;
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Destroy(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_u_char(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_GetHeaderCount(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);

    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_MessageCallback(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);

    /*this returns "0" so the message needs to be "accepted"*/
    /*this is "accepting"*/
    STRICT_EXPECTED_CALL(mocks, STRING_clone(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_construct_n(TEST_ETAG_VALUE_UNQUOTED, sizeof(TEST_ETAG_VALUE_UNQUOTED) - 1))
        .ValidateArgumentBuffer(1, TEST_ETAG_VALUE_UNQUOTED, sizeof(TEST_ETAG_VALUE_UNQUOTED) - 1);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, API_VERSION))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc());
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "User-Agent", CLIENT_DEVICE_TYPE_PREFIX CLIENT_DEVICE_BACKSLASH IOTHUB_SDK_VERSION))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "Authorization", TEST_BLANK_SAS_TOKEN))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "If-Match", TEST_ETAG_VALUE))
        .IgnoreArgument(1);


    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1); /*because abandon relativePath is a STRING_HANDLE*/
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,                                    /*sasObject handle                                             */
        IGNORED_PTR_ARG,                                    /*HTTPAPIEX_HANDLE handle,                                     */
        HTTPAPI_REQUEST_DELETE,                                /*HTTPAPI_REQUEST_TYPE requestType,                            */
        "/devices/" TEST_DEVICE_ID MESSAGE_ENDPOINT_HTTP_ETAG TEST_ETAG_VALUE_UNQUOTED API_VERSION,    /*const char* relativePath,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,                */
        NULL,                                               /*BUFFER_HANDLE requestContent,                                */
        IGNORED_PTR_ARG,                                    /*unsigned int* statusCode,                                    */
        NULL,                                               /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,               */
        NULL                                                /*BUFFER_HANDLE responseContent))                              */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(5)
        .IgnoreArgument(7)
        .CopyOutArgumentBuffer(7, &statusCode204, sizeof(statusCode204));

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_084: [ Otherwise, IoTHubTransportHttp_DoWork shall call HTTPAPIEX_SAS_ExecuteRequest passing the following parameters
//requestType: GET
//	relativePath : the message HTTP relative path
//	requestHttpHeadersHandle : message HTTP request headers created by _Create
//	requestContent : NULL
//	statusCode : a pointer to unsigned int which shall be later examined
//	responseHeadearsHandle : a new instance of HTTP headers
//	responseContent : a new instance of buffer ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_087: [ If status code is 200, then _DoWork shall make a copy of the value of the "ETag" http header. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_089: [ _DoWork shall assemble an IOTHUBMESSAGE_HANDLE from the received HTTP content (using the responseContent buffer). ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_093: [ Otherwise, _DoWork shall call IoTHubClient_LL_MessageCallback with parameters handle = iotHubClientHandle and message = newly created message. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_094: [ If IoTHubClient_LL_MessageCallback returns IOTHUBMESSAGE_ACCEPTED then _DoWork shall "accept" the message. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_099: [ _DoWork shall call HTTPAPIEX_SAS_ExecuteRequest with the following parameters:
//
//requestType: DELETE
//	relativePath : abandon relative path begin + value of ETag + APIVERSION
//	requestHttpHeadersHandle : an HTTP headers instance containing the following
//	Authorization : " "
//	If - Match : value of ETag
//	requestContent : NULL
//	statusCode : a pointer to unsigned int which might be used by logging
//	responseHeadearsHandle : NULL
//	responseContent : NULL ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_100: [ Accepting a message is successful when HTTPAPIEX_SAS_ExecuteRequest completes successfully and the status code is 204. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_090: [ All the HTTP headers of the form iothub-app-name:somecontent shall be transformed in message properties {name, somecontent}. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_123: [ After client creation, the first GET shall be allowed no matter what the value of GetMinimumPollingTime. ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_happy_path_2device_with_2nd_empty_waitingToSend_and_1_service_message_succeeds)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    unsigned int statusCode200 = 200;
    unsigned int statusCode204 = 204;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    (void)IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);
    auto devHandle2 = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_2, TEST_IOTHUB_CLIENT_LL_HANDLE2, TEST_CONFIG2.waitingToSend);

    (void)IoTHubTransportHttp_Subscribe(devHandle2);
    mocks.ResetAllCalls();

    setupDoWorkLoopOnceForOneDevice(mocks);
    setupDoWorkLoopForNextDevice(mocks, 1);


    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend)); /* DoWork DoEvent for device 1*/

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend2)); /* DoWork DoEvent for device 1*/


    STRICT_EXPECTED_CALL(mocks, get_time(NULL));
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc()); /*because responseHeadearsHandle: a new instance of HTTP headers*/
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, BUFFER_new());
    STRICT_EXPECTED_CALL(mocks, BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1); /*because relativePath is a STRING_HANDLE*/
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,                                    /*sasObject handle                                             */
        IGNORED_PTR_ARG,                                    /*HTTPAPIEX_HANDLE handle,                                     */
        HTTPAPI_REQUEST_GET,                                /*HTTPAPI_REQUEST_TYPE requestType,                            */
        "/devices/" TEST_DEVICE_ID2 MESSAGE_ENDPOINT_HTTP API_VERSION,    /*const char* relativePath,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,                */
        NULL,                                               /*BUFFER_HANDLE requestContent,                                */
        IGNORED_PTR_ARG,                                    /*unsigned int* statusCode,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,               */
        IGNORED_PTR_ARG                                     /*BUFFER_HANDLE responseContent))                              */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(5)
        .IgnoreArgument(7)
        .IgnoreArgument(8)
        .IgnoreArgument(9)
        .CopyOutArgumentBuffer(7, &statusCode200, sizeof(statusCode200));

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_FindHeaderValue(IGNORED_PTR_ARG, "ETag"))
        .IgnoreArgument(1)
        .SetReturn(TEST_ETAG_VALUE);

    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_CreateFromByteArray(IGNORED_PTR_ARG, IGNORED_NUM_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        ;
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Destroy(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_u_char(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_GetHeaderCount(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);

    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_MessageCallback(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);

    /*this returns "0" so the message needs to be "accepted"*/
    /*this is "accepting"*/
    STRICT_EXPECTED_CALL(mocks, STRING_clone(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_construct_n(TEST_ETAG_VALUE_UNQUOTED, sizeof(TEST_ETAG_VALUE_UNQUOTED) - 1))
        .ValidateArgumentBuffer(1, TEST_ETAG_VALUE_UNQUOTED, sizeof(TEST_ETAG_VALUE_UNQUOTED) - 1);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, API_VERSION))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc());
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "User-Agent", CLIENT_DEVICE_TYPE_PREFIX CLIENT_DEVICE_BACKSLASH IOTHUB_SDK_VERSION))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "Authorization", TEST_BLANK_SAS_TOKEN))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "If-Match", TEST_ETAG_VALUE))
        .IgnoreArgument(1);


    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1); /*because abandon relativePath is a STRING_HANDLE*/
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,                                    /*sasObject handle                                             */
        IGNORED_PTR_ARG,                                    /*HTTPAPIEX_HANDLE handle,                                     */
        HTTPAPI_REQUEST_DELETE,                                /*HTTPAPI_REQUEST_TYPE requestType,                            */
        "/devices/" TEST_DEVICE_ID2 MESSAGE_ENDPOINT_HTTP_ETAG TEST_ETAG_VALUE_UNQUOTED API_VERSION,    /*const char* relativePath,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,                */
        NULL,                                               /*BUFFER_HANDLE requestContent,                                */
        IGNORED_PTR_ARG,                                    /*unsigned int* statusCode,                                    */
        NULL,                                               /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,               */
        NULL                                                /*BUFFER_HANDLE responseContent))                              */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(5)
        .IgnoreArgument(7)
        .CopyOutArgumentBuffer(7, &statusCode204, sizeof(statusCode204));

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_057: [ If a messages to be send has type IOTHUBMESSAGE_STRING, then its serialization shall be {"body":"JSON encoding of the string", "base64Encoded":false} ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_2devices_2nd_has_1_event_item_as_string_happy_path_succeeds)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    DList_InsertTailList(&(waitingToSend2), &(message10.entry));
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    (void)IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);
    (void)IoTHubTransportHttp_Register(handle, &TEST_DEVICE_2, TEST_IOTHUB_CLIENT_LL_HANDLE2, TEST_CONFIG2.waitingToSend);

    mocks.ResetAllCalls();

    setupDoWorkLoopOnceForOneDevice(mocks);
    setupDoWorkLoopForNextDevice(mocks, 1);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend));
    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend2));

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Content-Type", "application/vnd.microsoft.iothub.json"))
        .IgnoreArgument(1);

    /*starting to prepare the "big" payload*/
    STRICT_EXPECTED_CALL(mocks, STRING_construct("["));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    /*this is first batched payload*/
    {
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetContentType(message10.messageHandle));
        STRICT_EXPECTED_CALL(mocks, STRING_construct("{\"body\":"));
        STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetString(message10.messageHandle));

        STRICT_EXPECTED_CALL(mocks, STRING_new_JSON(string10));
        STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2);

        STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, ",\"base64Encoded\":false")) /*closing the value of the body*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Properties(message10.messageHandle));
        STRICT_EXPECTED_CALL(mocks, Map_GetInternals(TEST_MAP_EMPTY, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(2)
            .IgnoreArgument(3)
            .IgnoreArgument(4);
        STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, "},")) /* this extra "," is going to be harshly overwritten by a "]"*/
            .IgnoreArgument(1);
        /*end of the first batched payload*/
    }

    {
        /*adding the first payload to the "big" payload*/
        STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2);

        /*closing the "big" payload*/
        STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_length(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
    }

    /*building the list of messages to be notified if HTTP is fine*/
    STRICT_EXPECTED_CALL(mocks, DList_RemoveHeadList(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, DList_InsertTailList(IGNORED_PTR_ARG, &(message10.entry)))
        .IgnoreArgument(1);

    {
        /*this is building the HTTP payload... from a STRING_HANDLE (as it comes as "big payload"), into an array of bytes*/
        STRICT_EXPECTED_CALL(mocks, BUFFER_new());
        STRICT_EXPECTED_CALL(mocks, BUFFER_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_length(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, BUFFER_build(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_NUM_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2)
            .IgnoreArgument(3);
    }

    /*executing HTTP goodies*/
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG)) /*because relativePath*/
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,                                    /*sasObject handle                                             */
        IGNORED_PTR_ARG,
        HTTPAPI_REQUEST_POST,                                                           /*HTTPAPI_REQUEST_TYPE requestType,                  */
        "/devices/" TEST_DEVICE_ID2 EVENT_ENDPOINT API_VERSION,                 /*const char* relativePath,                          */
        IGNORED_PTR_ARG,                                                                /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,      */
        IGNORED_PTR_ARG,                                                                /*BUFFER_HANDLE requestContent,                      */
        IGNORED_PTR_ARG,                                                                /*unsigned int* statusCode,                          */
        NULL,                                                                           /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,     */
        NULL                                                                            /*BUFFER_HANDLE responseContent)                     */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(5)
        .IgnoreArgument(6)
        .CopyOutArgumentBuffer(7, &httpStatus200, sizeof(httpStatus200));

    /*once the event has been succesfull...*/

    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_SendComplete(TEST_IOTHUB_CLIENT_LL_HANDLE2, IGNORED_PTR_ARG, IOTHUB_CLIENT_CONFIRMATION_OK))
        .IgnoreArgument(2);

    ENABLE_BATCHING();

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

TEST_FUNCTION(IoTHubTransportHttp_DoWork_2devices_2subscriptions_happy_path_succeeds)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    unsigned int statusCode200 = 200;
    unsigned int statusCode204 = 204;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    auto devHandle1 = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);
    auto devHandle2 = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_2, TEST_IOTHUB_CLIENT_LL_HANDLE2, TEST_CONFIG2.waitingToSend);

    (void)IoTHubTransportHttp_Subscribe(devHandle2);
    (void)IoTHubTransportHttp_Subscribe(devHandle1);
    mocks.ResetAllCalls();

    setupDoWorkLoopOnceForOneDevice(mocks);
    setupDoWorkLoopForNextDevice(mocks, 1);


    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend)); /* DoWork DoEvent for device 1*/

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend2)); /* DoWork DoEvent for device 1*/

                                                                     // Device 1
    {
        STRICT_EXPECTED_CALL(mocks, get_time(NULL));
        STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc()); /*because responseHeadearsHandle: a new instance of HTTP headers*/
        STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, BUFFER_new());
        STRICT_EXPECTED_CALL(mocks, BUFFER_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
            .IgnoreArgument(1); /*because relativePath is a STRING_HANDLE*/
        STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
            IGNORED_PTR_ARG,                                    /*sasObject handle                                             */
            IGNORED_PTR_ARG,                                    /*HTTPAPIEX_HANDLE handle,                                     */
            HTTPAPI_REQUEST_GET,                                /*HTTPAPI_REQUEST_TYPE requestType,                            */
            "/devices/" TEST_DEVICE_ID MESSAGE_ENDPOINT_HTTP API_VERSION,    /*const char* relativePath,                                    */
            IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,                */
            NULL,                                               /*BUFFER_HANDLE requestContent,                                */
            IGNORED_PTR_ARG,                                    /*unsigned int* statusCode,                                    */
            IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,               */
            IGNORED_PTR_ARG                                     /*BUFFER_HANDLE responseContent))                              */
            ))
            .IgnoreArgument(1)
            .IgnoreArgument(2)
            .IgnoreArgument(5)
            .IgnoreArgument(7)
            .IgnoreArgument(8)
            .IgnoreArgument(9)
            .CopyOutArgumentBuffer(7, &statusCode200, sizeof(statusCode200));

        STRICT_EXPECTED_CALL(mocks, HTTPHeaders_FindHeaderValue(IGNORED_PTR_ARG, "ETag"))
            .IgnoreArgument(1)
            .SetReturn(TEST_ETAG_VALUE);

        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_CreateFromByteArray(IGNORED_PTR_ARG, IGNORED_NUM_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2)
            ;
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Destroy(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, BUFFER_u_char(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, BUFFER_length(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, HTTPHeaders_GetHeaderCount(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2);

        STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_MessageCallback(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2);

        /*this returns "0" so the message needs to be "accepted"*/
        /*this is "accepting"*/
        STRICT_EXPECTED_CALL(mocks, STRING_clone(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_construct_n(TEST_ETAG_VALUE_UNQUOTED, sizeof(TEST_ETAG_VALUE_UNQUOTED) - 1))
            .ValidateArgumentBuffer(1, TEST_ETAG_VALUE_UNQUOTED, sizeof(TEST_ETAG_VALUE_UNQUOTED) - 1);
        STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2);
        STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, API_VERSION))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc());
        STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "User-Agent", CLIENT_DEVICE_TYPE_PREFIX CLIENT_DEVICE_BACKSLASH IOTHUB_SDK_VERSION))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "Authorization", TEST_BLANK_SAS_TOKEN))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "If-Match", TEST_ETAG_VALUE))
            .IgnoreArgument(1);


        STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
            .IgnoreArgument(1); /*because abandon relativePath is a STRING_HANDLE*/
        STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
            IGNORED_PTR_ARG,                                    /*sasObject handle                                             */
            IGNORED_PTR_ARG,                                    /*HTTPAPIEX_HANDLE handle,                                     */
            HTTPAPI_REQUEST_DELETE,                                /*HTTPAPI_REQUEST_TYPE requestType,                            */
            "/devices/" TEST_DEVICE_ID MESSAGE_ENDPOINT_HTTP_ETAG TEST_ETAG_VALUE_UNQUOTED API_VERSION,    /*const char* relativePath,                                    */
            IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,                */
            NULL,                                               /*BUFFER_HANDLE requestContent,                                */
            IGNORED_PTR_ARG,                                    /*unsigned int* statusCode,                                    */
            NULL,                                               /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,               */
            NULL                                                /*BUFFER_HANDLE responseContent))                              */
            ))
            .IgnoreArgument(1)
            .IgnoreArgument(2)
            .IgnoreArgument(5)
            .IgnoreArgument(7)
            .CopyOutArgumentBuffer(7, &statusCode204, sizeof(statusCode204));


    }

    //device 2
    {
        STRICT_EXPECTED_CALL(mocks, get_time(NULL));
        STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc()); /*because responseHeadearsHandle: a new instance of HTTP headers*/
        STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, BUFFER_new());
        STRICT_EXPECTED_CALL(mocks, BUFFER_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
            .IgnoreArgument(1); /*because relativePath is a STRING_HANDLE*/
        STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
            IGNORED_PTR_ARG,                                    /*sasObject handle                                             */
            IGNORED_PTR_ARG,                                    /*HTTPAPIEX_HANDLE handle,                                     */
            HTTPAPI_REQUEST_GET,                                /*HTTPAPI_REQUEST_TYPE requestType,                            */
            "/devices/" TEST_DEVICE_ID2 MESSAGE_ENDPOINT_HTTP API_VERSION,    /*const char* relativePath,                                    */
            IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,                */
            NULL,                                               /*BUFFER_HANDLE requestContent,                                */
            IGNORED_PTR_ARG,                                    /*unsigned int* statusCode,                                    */
            IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,               */
            IGNORED_PTR_ARG                                     /*BUFFER_HANDLE responseContent))                              */
            ))
            .IgnoreArgument(1)
            .IgnoreArgument(2)
            .IgnoreArgument(5)
            .IgnoreArgument(7)
            .IgnoreArgument(8)
            .IgnoreArgument(9)
            .CopyOutArgumentBuffer(7, &statusCode200, sizeof(statusCode200));

        STRICT_EXPECTED_CALL(mocks, HTTPHeaders_FindHeaderValue(IGNORED_PTR_ARG, "ETag"))
            .IgnoreArgument(1)
            .SetReturn(TEST_ETAG_VALUE);

        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_CreateFromByteArray(IGNORED_PTR_ARG, IGNORED_NUM_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2)
            ;
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Destroy(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, BUFFER_u_char(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, BUFFER_length(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, HTTPHeaders_GetHeaderCount(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2);

        STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_MessageCallback(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2);

        /*this returns "0" so the message needs to be "accepted"*/
        /*this is "accepting"*/
        STRICT_EXPECTED_CALL(mocks, STRING_clone(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_construct_n(TEST_ETAG_VALUE_UNQUOTED, sizeof(TEST_ETAG_VALUE_UNQUOTED) - 1))
            .ValidateArgumentBuffer(1, TEST_ETAG_VALUE_UNQUOTED, sizeof(TEST_ETAG_VALUE_UNQUOTED) - 1);
        STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2);
        STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, API_VERSION))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc());
        STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "User-Agent", CLIENT_DEVICE_TYPE_PREFIX CLIENT_DEVICE_BACKSLASH IOTHUB_SDK_VERSION))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "Authorization", TEST_BLANK_SAS_TOKEN))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "If-Match", TEST_ETAG_VALUE))
            .IgnoreArgument(1);


        STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
            .IgnoreArgument(1); /*because abandon relativePath is a STRING_HANDLE*/
        STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
            IGNORED_PTR_ARG,                                    /*sasObject handle                                             */
            IGNORED_PTR_ARG,                                    /*HTTPAPIEX_HANDLE handle,                                     */
            HTTPAPI_REQUEST_DELETE,                                /*HTTPAPI_REQUEST_TYPE requestType,                            */
            "/devices/" TEST_DEVICE_ID2 MESSAGE_ENDPOINT_HTTP_ETAG TEST_ETAG_VALUE_UNQUOTED API_VERSION,    /*const char* relativePath,                                    */
            IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,                */
            NULL,                                               /*BUFFER_HANDLE requestContent,                                */
            IGNORED_PTR_ARG,                                    /*unsigned int* statusCode,                                    */
            NULL,                                               /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,               */
            NULL                                                /*BUFFER_HANDLE responseContent))                              */
            ))
            .IgnoreArgument(1)
            .IgnoreArgument(2)
            .IgnoreArgument(5)
            .IgnoreArgument(7)
            .CopyOutArgumentBuffer(7, &statusCode204, sizeof(statusCode204));
    }

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);

}

//Tests_SRS_TRANSPORTMULTITHTTP_17_124: [ If time is not available then all calls shall be treated as if they are the first one. ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_happy_path_with_empty_waitingToSend_and_1_service_message_when_time_is_minus_one_succeeds)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    unsigned int statusCode200 = 200;
    unsigned int statusCode204 = 204;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    (void)IoTHubTransportHttp_Subscribe(devHandle);
    mocks.ResetAllCalls();

    setupDoWorkLoopOnceForOneDevice(mocks);


    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend)); /*because DoWork for event*/

    STRICT_EXPECTED_CALL(mocks, get_time(NULL))
        .SetReturn((time_t)(-1));
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc()); /*because responseHeadearsHandle: a new instance of HTTP headers*/
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, BUFFER_new());
    STRICT_EXPECTED_CALL(mocks, BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1); /*because relativePath is a STRING_HANDLE*/
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,                                    /*sasObject handle                                             */
        IGNORED_PTR_ARG,                                    /*HTTPAPIEX_HANDLE handle,                                     */
        HTTPAPI_REQUEST_GET,                                /*HTTPAPI_REQUEST_TYPE requestType,                            */
        "/devices/" TEST_DEVICE_ID MESSAGE_ENDPOINT_HTTP API_VERSION,    /*const char* relativePath,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,                */
        NULL,                                               /*BUFFER_HANDLE requestContent,                                */
        IGNORED_PTR_ARG,                                    /*unsigned int* statusCode,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,               */
        IGNORED_PTR_ARG                                     /*BUFFER_HANDLE responseContent))                              */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(5)
        .IgnoreArgument(7)
        .IgnoreArgument(8)
        .IgnoreArgument(9)
        .CopyOutArgumentBuffer(7, &statusCode200, sizeof(statusCode200));

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_FindHeaderValue(IGNORED_PTR_ARG, "ETag"))
        .IgnoreArgument(1)
        .SetReturn(TEST_ETAG_VALUE);

    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_CreateFromByteArray(IGNORED_PTR_ARG, IGNORED_NUM_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        ;
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Destroy(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_u_char(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_GetHeaderCount(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);

    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_MessageCallback(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);

    /*this returns "0" so the message needs to be "accepted"*/
    /*this is "accepting"*/
    STRICT_EXPECTED_CALL(mocks, STRING_clone(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_construct_n(TEST_ETAG_VALUE_UNQUOTED, sizeof(TEST_ETAG_VALUE_UNQUOTED) - 1))
        .ValidateArgumentBuffer(1, TEST_ETAG_VALUE_UNQUOTED, sizeof(TEST_ETAG_VALUE_UNQUOTED) - 1);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, API_VERSION))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc());
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "User-Agent", CLIENT_DEVICE_TYPE_PREFIX CLIENT_DEVICE_BACKSLASH IOTHUB_SDK_VERSION))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "Authorization", TEST_BLANK_SAS_TOKEN))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "If-Match", TEST_ETAG_VALUE))
        .IgnoreArgument(1);


    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1); /*because abandon relativePath is a STRING_HANDLE*/
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,                                    /*sasObject handle                                             */
        IGNORED_PTR_ARG,                                    /*HTTPAPIEX_HANDLE handle,                                     */
        HTTPAPI_REQUEST_DELETE,                                /*HTTPAPI_REQUEST_TYPE requestType,                            */
        "/devices/" TEST_DEVICE_ID MESSAGE_ENDPOINT_HTTP_ETAG TEST_ETAG_VALUE_UNQUOTED API_VERSION,    /*const char* relativePath,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,                */
        NULL,                                               /*BUFFER_HANDLE requestContent,                                */
        IGNORED_PTR_ARG,                                    /*unsigned int* statusCode,                                    */
        NULL,                                               /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,               */
        NULL                                                /*BUFFER_HANDLE responseContent))                              */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(5)
        .IgnoreArgument(7)
        .CopyOutArgumentBuffer(7, &statusCode204, sizeof(statusCode204));

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_124: [ If time is not available then all calls shall be treated as if they are the first one. ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_happy_path_with_empty_waitingToSend_and_2_service_message_when_time_is_minus_one_succeeds)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    unsigned int statusCode200 = 200;
    unsigned int statusCode204 = 204;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    (void)IoTHubTransportHttp_Subscribe(devHandle);
    mocks.ResetAllCalls();


    for (int i = 0; i < 2; i++)
    {
        setupDoWorkLoopOnceForOneDevice(mocks);

        STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend)); /*because DoWork for event*/

        STRICT_EXPECTED_CALL(mocks, get_time(NULL))
            .SetReturn((time_t)(-1));
        STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc()); /*because responseHeadearsHandle: a new instance of HTTP headers*/
        STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, BUFFER_new());
        STRICT_EXPECTED_CALL(mocks, BUFFER_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
            .IgnoreArgument(1); /*because relativePath is a STRING_HANDLE*/
        STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
            IGNORED_PTR_ARG,                                    /*sasObject handle                                             */
            IGNORED_PTR_ARG,                                    /*HTTPAPIEX_HANDLE handle,                                     */
            HTTPAPI_REQUEST_GET,                                /*HTTPAPI_REQUEST_TYPE requestType,                            */
            "/devices/" TEST_DEVICE_ID MESSAGE_ENDPOINT_HTTP API_VERSION,    /*const char* relativePath,                                    */
            IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,                */
            NULL,                                               /*BUFFER_HANDLE requestContent,                                */
            IGNORED_PTR_ARG,                                    /*unsigned int* statusCode,                                    */
            IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,               */
            IGNORED_PTR_ARG                                     /*BUFFER_HANDLE responseContent))                              */
            ))
            .IgnoreArgument(1)
            .IgnoreArgument(2)
            .IgnoreArgument(5)
            .IgnoreArgument(7)
            .IgnoreArgument(8)
            .IgnoreArgument(9)
            .CopyOutArgumentBuffer(7, &statusCode200, sizeof(statusCode200));

        STRICT_EXPECTED_CALL(mocks, HTTPHeaders_FindHeaderValue(IGNORED_PTR_ARG, "ETag"))
            .IgnoreArgument(1)
            .SetReturn(TEST_ETAG_VALUE);

        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_CreateFromByteArray(IGNORED_PTR_ARG, IGNORED_NUM_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2)
            ;
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Destroy(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, BUFFER_u_char(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, BUFFER_length(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, HTTPHeaders_GetHeaderCount(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2);

        STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_MessageCallback(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2);

        /*this returns "0" so the message needs to be "accepted"*/
        /*this is "accepting"*/
        STRICT_EXPECTED_CALL(mocks, STRING_clone(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_construct_n(TEST_ETAG_VALUE_UNQUOTED, sizeof(TEST_ETAG_VALUE_UNQUOTED) - 1))
            .ValidateArgumentBuffer(1, TEST_ETAG_VALUE_UNQUOTED, sizeof(TEST_ETAG_VALUE_UNQUOTED) - 1);
        STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2);
        STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, API_VERSION))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc());
        STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "User-Agent", CLIENT_DEVICE_TYPE_PREFIX CLIENT_DEVICE_BACKSLASH IOTHUB_SDK_VERSION))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "Authorization", TEST_BLANK_SAS_TOKEN))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "If-Match", TEST_ETAG_VALUE))
            .IgnoreArgument(1);


        STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
            .IgnoreArgument(1); /*because abandon relativePath is a STRING_HANDLE*/
        STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
            IGNORED_PTR_ARG,                                    /*sasObject handle                                             */
            IGNORED_PTR_ARG,                                    /*HTTPAPIEX_HANDLE handle,                                     */
            HTTPAPI_REQUEST_DELETE,                                /*HTTPAPI_REQUEST_TYPE requestType,                            */
            "/devices/" TEST_DEVICE_ID MESSAGE_ENDPOINT_HTTP_ETAG TEST_ETAG_VALUE_UNQUOTED API_VERSION,    /*const char* relativePath,                                    */
            IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,                */
            NULL,                                               /*BUFFER_HANDLE requestContent,                                */
            IGNORED_PTR_ARG,                                    /*unsigned int* statusCode,                                    */
            NULL,                                               /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,               */
            NULL                                                /*BUFFER_HANDLE responseContent))                              */
            ))
            .IgnoreArgument(1)
            .IgnoreArgument(2)
            .IgnoreArgument(5)
            .IgnoreArgument(7)
            .CopyOutArgumentBuffer(7, &statusCode204, sizeof(statusCode204));
    }

    ///act
    for (int i = 0; i < 2; i++)
    {
        IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);
    }


    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_121: [ "MinimumPollingTime" ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_happy_path_with_empty_waitingToSend_and_1_service_message_with_non_default_minimumPollingTime_succeeds)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    unsigned int statusCode200 = 200;
    unsigned int statusCode204 = 204;
    unsigned int thisIs20Seconds = 20;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);

    (void)IoTHubTransportHttp_SetOption(handle, OPTION_MIN_POLLING_TIME, &thisIs20Seconds);
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    (void)IoTHubTransportHttp_Subscribe(devHandle);
    mocks.ResetAllCalls();

    /*everything below is for the second time this is called*/

    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend)); /*because DoWork for event*/

    STRICT_EXPECTED_CALL(mocks, get_time(NULL));
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc()); /*because responseHeadearsHandle: a new instance of HTTP headers*/
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, BUFFER_new());
    STRICT_EXPECTED_CALL(mocks, BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1); /*because relativePath is a STRING_HANDLE*/
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,                                    /*sasObject handle                                             */
        IGNORED_PTR_ARG,                                    /*HTTPAPIEX_HANDLE handle,                                     */
        HTTPAPI_REQUEST_GET,                                /*HTTPAPI_REQUEST_TYPE requestType,                            */
        "/devices/" TEST_DEVICE_ID MESSAGE_ENDPOINT_HTTP API_VERSION,    /*const char* relativePath,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,                */
        NULL,                                               /*BUFFER_HANDLE requestContent,                                */
        IGNORED_PTR_ARG,                                    /*unsigned int* statusCode,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,               */
        IGNORED_PTR_ARG                                     /*BUFFER_HANDLE responseContent))                              */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(5)
        .IgnoreArgument(7)
        .IgnoreArgument(8)
        .IgnoreArgument(9)
        .CopyOutArgumentBuffer(7, &statusCode200, sizeof(statusCode200));

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_FindHeaderValue(IGNORED_PTR_ARG, "ETag"))
        .IgnoreArgument(1)
        .SetReturn(TEST_ETAG_VALUE);

    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_CreateFromByteArray(IGNORED_PTR_ARG, IGNORED_NUM_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        ;
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Destroy(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_u_char(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_GetHeaderCount(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);

    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_MessageCallback(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);

    /*this returns "0" so the message needs to be "accepted"*/
    /*this is "accepting"*/
    STRICT_EXPECTED_CALL(mocks, STRING_clone(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_construct_n(TEST_ETAG_VALUE_UNQUOTED, sizeof(TEST_ETAG_VALUE_UNQUOTED) - 1))
        .ValidateArgumentBuffer(1, TEST_ETAG_VALUE_UNQUOTED, sizeof(TEST_ETAG_VALUE_UNQUOTED) - 1);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, API_VERSION))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc());
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "User-Agent", CLIENT_DEVICE_TYPE_PREFIX CLIENT_DEVICE_BACKSLASH IOTHUB_SDK_VERSION))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "Authorization", TEST_BLANK_SAS_TOKEN))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "If-Match", TEST_ETAG_VALUE))
        .IgnoreArgument(1);


    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1); /*because abandon relativePath is a STRING_HANDLE*/
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,                                    /*sasObject handle                                             */
        IGNORED_PTR_ARG,                                    /*HTTPAPIEX_HANDLE handle,                                     */
        HTTPAPI_REQUEST_DELETE,                                /*HTTPAPI_REQUEST_TYPE requestType,                            */
        "/devices/" TEST_DEVICE_ID MESSAGE_ENDPOINT_HTTP_ETAG TEST_ETAG_VALUE_UNQUOTED API_VERSION,    /*const char* relativePath,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,                */
        NULL,                                               /*BUFFER_HANDLE requestContent,                                */
        IGNORED_PTR_ARG,                                    /*unsigned int* statusCode,                                    */
        NULL,                                               /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,               */
        NULL                                                /*BUFFER_HANDLE responseContent))                              */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(5)
        .IgnoreArgument(7)
        .CopyOutArgumentBuffer(7, &statusCode204, sizeof(statusCode204));

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_122: [ A GET request that happens earlier than GetMinimumPollingTime shall be ignored. ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_happy_path_with_empty_waitingToSend_and_1_service_message_later_than_minimumPollingTime_does_nothing_succeeds)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    (void)IoTHubTransportHttp_Subscribe(devHandle);
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);
    mocks.ResetAllCalls();

    /*everything below is for the second time _DoWork this is called*/

    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend)); /*because DoWork for event*/

    STRICT_EXPECTED_CALL(mocks, get_time(NULL))
        .SetReturn(TEST_GET_TIME_VALUE + TEST_DEFAULT_GETMINIMUMPOLLINGTIME); /*right on the verge of the time*/
    STRICT_EXPECTED_CALL(mocks, get_difftime(TEST_GET_TIME_VALUE + TEST_DEFAULT_GETMINIMUMPOLLINGTIME, TEST_GET_TIME_VALUE));

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_122: [ A GET request that happens earlier than GetMinimumPollingTime shall be ignored. ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_happy_path_with_empty_waitingToSend_and_1_service_message_immediately_after_minimumPollingTime_polls_succeeds)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    unsigned int statusCode200 = 200;
    unsigned int statusCode204 = 204;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    (void)IoTHubTransportHttp_Subscribe(devHandle);
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);
    mocks.ResetAllCalls();

    /*everything below is for the second time _DoWork this is called*/

    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend)); /*because DoWork for event*/

    STRICT_EXPECTED_CALL(mocks, get_time(NULL))
        .SetReturn(TEST_GET_TIME_VALUE + TEST_DEFAULT_GETMINIMUMPOLLINGTIME + 1); /*right on the verge of the time*/
    STRICT_EXPECTED_CALL(mocks, get_difftime(TEST_GET_TIME_VALUE + TEST_DEFAULT_GETMINIMUMPOLLINGTIME + 1, TEST_GET_TIME_VALUE));

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc()); /*because responseHeadearsHandle: a new instance of HTTP headers*/
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, BUFFER_new());
    STRICT_EXPECTED_CALL(mocks, BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1); /*because relativePath is a STRING_HANDLE*/
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,                                    /*sasObject handle                                             */
        IGNORED_PTR_ARG,                                    /*HTTPAPIEX_HANDLE handle,                                     */
        HTTPAPI_REQUEST_GET,                                /*HTTPAPI_REQUEST_TYPE requestType,                            */
        "/devices/" TEST_DEVICE_ID MESSAGE_ENDPOINT_HTTP API_VERSION,    /*const char* relativePath,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,                */
        NULL,                                               /*BUFFER_HANDLE requestContent,                                */
        IGNORED_PTR_ARG,                                    /*unsigned int* statusCode,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,               */
        IGNORED_PTR_ARG                                     /*BUFFER_HANDLE responseContent))                              */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(5)
        .IgnoreArgument(7)
        .IgnoreArgument(8)
        .IgnoreArgument(9)
        .CopyOutArgumentBuffer(7, &statusCode200, sizeof(statusCode200));

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_FindHeaderValue(IGNORED_PTR_ARG, "ETag"))
        .IgnoreArgument(1)
        .SetReturn(TEST_ETAG_VALUE);

    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_CreateFromByteArray(IGNORED_PTR_ARG, IGNORED_NUM_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        ;
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Destroy(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_u_char(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_GetHeaderCount(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);

    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_MessageCallback(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);

    /*this returns "0" so the message needs to be "accepted"*/
    /*this is "accepting"*/
    STRICT_EXPECTED_CALL(mocks, STRING_clone(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_construct_n(TEST_ETAG_VALUE_UNQUOTED, sizeof(TEST_ETAG_VALUE_UNQUOTED) - 1))
        .ValidateArgumentBuffer(1, TEST_ETAG_VALUE_UNQUOTED, sizeof(TEST_ETAG_VALUE_UNQUOTED) - 1);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, API_VERSION))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc());
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "User-Agent", CLIENT_DEVICE_TYPE_PREFIX CLIENT_DEVICE_BACKSLASH IOTHUB_SDK_VERSION))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "Authorization", TEST_BLANK_SAS_TOKEN))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "If-Match", TEST_ETAG_VALUE))
        .IgnoreArgument(1);


    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1); /*because abandon relativePath is a STRING_HANDLE*/
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,                                    /*sasObject handle                                             */
        IGNORED_PTR_ARG,                                    /*HTTPAPIEX_HANDLE handle,                                     */
        HTTPAPI_REQUEST_DELETE,                                /*HTTPAPI_REQUEST_TYPE requestType,                            */
        "/devices/" TEST_DEVICE_ID MESSAGE_ENDPOINT_HTTP_ETAG TEST_ETAG_VALUE_UNQUOTED API_VERSION,    /*const char* relativePath,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,                */
        NULL,                                               /*BUFFER_HANDLE requestContent,                                */
        IGNORED_PTR_ARG,                                    /*unsigned int* statusCode,                                    */
        NULL,                                               /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,               */
        NULL                                                /*BUFFER_HANDLE responseContent))                              */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(5)
        .IgnoreArgument(7)
        .CopyOutArgumentBuffer(7, &statusCode204, sizeof(statusCode204));
    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

/*undefined behavior*/
/*purpose of this test is to see that gremlins don't emerge when the http return code is 404 from the service*/
TEST_FUNCTION(IoTHubTransportHttp_DoWork_happy_path_with_empty_waitingToSend_and_1_service_message_with_accept_code_404_succeeds)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    unsigned int statusCode200 = 200;
    unsigned int statusCode404 = 404;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    (void)IoTHubTransportHttp_Subscribe(devHandle);
    mocks.ResetAllCalls();
    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend)); /*because DoWork for event*/

    STRICT_EXPECTED_CALL(mocks, get_time(NULL));
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc()); /*because responseHeadearsHandle: a new instance of HTTP headers*/
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, BUFFER_new());
    STRICT_EXPECTED_CALL(mocks, BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1); /*because relativePath is a STRING_HANDLE*/
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,                                    /*sasObject handle                                             */
        IGNORED_PTR_ARG,                                    /*HTTPAPIEX_HANDLE handle,                                     */
        HTTPAPI_REQUEST_GET,                                /*HTTPAPI_REQUEST_TYPE requestType,                            */
        "/devices/" TEST_DEVICE_ID MESSAGE_ENDPOINT_HTTP API_VERSION,    /*const char* relativePath,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,                */
        NULL,                                               /*BUFFER_HANDLE requestContent,                                */
        IGNORED_PTR_ARG,                                    /*unsigned int* statusCode,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,               */
        IGNORED_PTR_ARG                                     /*BUFFER_HANDLE responseContent))                              */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(5)
        .IgnoreArgument(7)
        .IgnoreArgument(8)
        .IgnoreArgument(9)
        .CopyOutArgumentBuffer(7, &statusCode200, sizeof(statusCode200));

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_FindHeaderValue(IGNORED_PTR_ARG, "ETag"))
        .IgnoreArgument(1)
        .SetReturn(TEST_ETAG_VALUE);

    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_CreateFromByteArray(IGNORED_PTR_ARG, IGNORED_NUM_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Destroy(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_u_char(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_GetHeaderCount(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);

    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_MessageCallback(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);

    /*this returns "0" so the message needs to be "accepted"*/
    /*this is "accepting"*/
    STRICT_EXPECTED_CALL(mocks, STRING_clone(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_construct_n(TEST_ETAG_VALUE_UNQUOTED, sizeof(TEST_ETAG_VALUE_UNQUOTED) - 1))
        .ValidateArgumentBuffer(1, TEST_ETAG_VALUE_UNQUOTED, sizeof(TEST_ETAG_VALUE_UNQUOTED) - 1);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, API_VERSION))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc());
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "User-Agent", CLIENT_DEVICE_TYPE_PREFIX CLIENT_DEVICE_BACKSLASH IOTHUB_SDK_VERSION))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "Authorization", TEST_BLANK_SAS_TOKEN))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "If-Match", TEST_ETAG_VALUE))
        .IgnoreArgument(1);


    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1); /*because abandon relativePath is a STRING_HANDLE*/
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,                                    /*sasObject handle                                             */
        IGNORED_PTR_ARG,                                    /*HTTPAPIEX_HANDLE handle,                                     */
        HTTPAPI_REQUEST_DELETE,                                /*HTTPAPI_REQUEST_TYPE requestType,                            */
        "/devices/" TEST_DEVICE_ID MESSAGE_ENDPOINT_HTTP_ETAG TEST_ETAG_VALUE_UNQUOTED API_VERSION,    /*const char* relativePath,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,                */
        NULL,                                               /*BUFFER_HANDLE requestContent,                                */
        IGNORED_PTR_ARG,                                    /*unsigned int* statusCode,                                    */
        NULL,                                               /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,               */
        NULL                                                /*BUFFER_HANDLE responseContent))                              */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(5)
        .IgnoreArgument(7)
        .CopyOutArgumentBuffer(7, &statusCode404, sizeof(statusCode404));

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

/*undefined behavior*/
TEST_FUNCTION(IoTHubTransportHttp_DoWork_happy_path_with_empty_waitingToSend_and_1_service_message_with_httpapi_execute_request_failed_succeeds)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    unsigned int statusCode200 = 200;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    (void)IoTHubTransportHttp_Subscribe(devHandle);
    mocks.ResetAllCalls();
    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend)); /*because DoWork for event*/

    STRICT_EXPECTED_CALL(mocks, get_time(NULL));
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc()); /*because responseHeadearsHandle: a new instance of HTTP headers*/
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, BUFFER_new());
    STRICT_EXPECTED_CALL(mocks, BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1); /*because relativePath is a STRING_HANDLE*/
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,                                    /*sasObject handle                                             */
        IGNORED_PTR_ARG,                                    /*HTTPAPIEX_HANDLE handle,                                     */
        HTTPAPI_REQUEST_GET,                                /*HTTPAPI_REQUEST_TYPE requestType,                            */
        "/devices/" TEST_DEVICE_ID MESSAGE_ENDPOINT_HTTP API_VERSION,    /*const char* relativePath,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,                */
        NULL,                                               /*BUFFER_HANDLE requestContent,                                */
        IGNORED_PTR_ARG,                                    /*unsigned int* statusCode,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,               */
        IGNORED_PTR_ARG                                     /*BUFFER_HANDLE responseContent))                              */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(5)
        .IgnoreArgument(7)
        .IgnoreArgument(8)
        .IgnoreArgument(9)
        .CopyOutArgumentBuffer(7, &statusCode200, sizeof(statusCode200));

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_FindHeaderValue(IGNORED_PTR_ARG, "ETag"))
        .IgnoreArgument(1)
        .SetReturn(TEST_ETAG_VALUE);

    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_CreateFromByteArray(IGNORED_PTR_ARG, IGNORED_NUM_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);

    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Destroy(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_u_char(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_GetHeaderCount(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);

    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_MessageCallback(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);

    /*this returns "0" so the message needs to be "accepted"*/
    /*this is "accepting"*/
    STRICT_EXPECTED_CALL(mocks, STRING_clone(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_construct_n(TEST_ETAG_VALUE_UNQUOTED, sizeof(TEST_ETAG_VALUE_UNQUOTED) - 1))
        .ValidateArgumentBuffer(1, TEST_ETAG_VALUE_UNQUOTED, sizeof(TEST_ETAG_VALUE_UNQUOTED) - 1);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, API_VERSION))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc());
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "User-Agent", CLIENT_DEVICE_TYPE_PREFIX CLIENT_DEVICE_BACKSLASH IOTHUB_SDK_VERSION))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "Authorization", TEST_BLANK_SAS_TOKEN))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "If-Match", TEST_ETAG_VALUE))
        .IgnoreArgument(1);


    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1); /*because abandon relativePath is a STRING_HANDLE*/
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,                                    /*sasObject handle                                             */
        IGNORED_PTR_ARG,                                    /*HTTPAPIEX_HANDLE handle,                                     */
        HTTPAPI_REQUEST_DELETE,                                /*HTTPAPI_REQUEST_TYPE requestType,                            */
        "/devices/" TEST_DEVICE_ID MESSAGE_ENDPOINT_HTTP_ETAG TEST_ETAG_VALUE_UNQUOTED API_VERSION,    /*const char* relativePath,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,                */
        NULL,                                               /*BUFFER_HANDLE requestContent,                                */
        IGNORED_PTR_ARG,                                    /*unsigned int* statusCode,                                    */
        NULL,                                               /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,               */
        NULL                                                /*BUFFER_HANDLE responseContent))                              */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(5)
        .IgnoreArgument(7)
        .SetReturn(HTTPAPIEX_ERROR);

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

/*undefined behavior*/
TEST_FUNCTION(IoTHubTransportHttp_DoWork_happy_path_with_empty_waitingToSend_and_1_service_message_acceptmessage_fails_at_HTTPHeaders_AddHeaderNameValuePair_succeeds_1)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    unsigned int statusCode200 = 200;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    (void)IoTHubTransportHttp_Subscribe(devHandle);
    mocks.ResetAllCalls();
    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend)); /*because DoWork for event*/

    STRICT_EXPECTED_CALL(mocks, get_time(NULL));
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc()); /*because responseHeadearsHandle: a new instance of HTTP headers*/
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, BUFFER_new());
    STRICT_EXPECTED_CALL(mocks, BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1); /*because relativePath is a STRING_HANDLE*/
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,                                    /*sasObject handle                                             */
        IGNORED_PTR_ARG,                                    /*HTTPAPIEX_HANDLE handle,                                     */
        HTTPAPI_REQUEST_GET,                                /*HTTPAPI_REQUEST_TYPE requestType,                            */
        "/devices/" TEST_DEVICE_ID MESSAGE_ENDPOINT_HTTP API_VERSION,    /*const char* relativePath,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,                */
        NULL,                                               /*BUFFER_HANDLE requestContent,                                */
        IGNORED_PTR_ARG,                                    /*unsigned int* statusCode,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,               */
        IGNORED_PTR_ARG                                     /*BUFFER_HANDLE responseContent))                              */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(5)
        .IgnoreArgument(7)
        .IgnoreArgument(8)
        .IgnoreArgument(9)
        .CopyOutArgumentBuffer(7, &statusCode200, sizeof(statusCode200));

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_FindHeaderValue(IGNORED_PTR_ARG, "ETag"))
        .IgnoreArgument(1)
        .SetReturn(TEST_ETAG_VALUE);

    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_CreateFromByteArray(IGNORED_PTR_ARG, IGNORED_NUM_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        ;
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Destroy(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_u_char(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_GetHeaderCount(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);

    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_MessageCallback(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);

    /*this returns "0" so the message needs to be "accepted"*/
    /*this is "accepting"*/
    STRICT_EXPECTED_CALL(mocks, STRING_clone(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_construct_n(TEST_ETAG_VALUE_UNQUOTED, sizeof(TEST_ETAG_VALUE_UNQUOTED) - 1))
        .ValidateArgumentBuffer(1, TEST_ETAG_VALUE_UNQUOTED, sizeof(TEST_ETAG_VALUE_UNQUOTED) - 1);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, API_VERSION))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc());
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "User-Agent", CLIENT_DEVICE_TYPE_PREFIX CLIENT_DEVICE_BACKSLASH IOTHUB_SDK_VERSION))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "Authorization", TEST_BLANK_SAS_TOKEN))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "If-Match", TEST_ETAG_VALUE))
        .IgnoreArgument(1)
        .SetReturn(HTTP_HEADERS_ERROR);

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

/*undefined behavior*/
TEST_FUNCTION(IoTHubTransportHttp_DoWork_happy_path_with_empty_waitingToSend_and_1_service_message_acceptmessage_fails_at_HTTPHeaders_AddHeaderNameValuePair_succeeds_2)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    unsigned int statusCode200 = 200;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    (void)IoTHubTransportHttp_Subscribe(devHandle);
    mocks.ResetAllCalls();
    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend)); /*because DoWork for event*/

    STRICT_EXPECTED_CALL(mocks, get_time(NULL));
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc()); /*because responseHeadearsHandle: a new instance of HTTP headers*/
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, BUFFER_new());
    STRICT_EXPECTED_CALL(mocks, BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1); /*because relativePath is a STRING_HANDLE*/
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,                                    /*sasObject handle                                             */
        IGNORED_PTR_ARG,                                    /*HTTPAPIEX_HANDLE handle,                                     */
        HTTPAPI_REQUEST_GET,                                /*HTTPAPI_REQUEST_TYPE requestType,                            */
        "/devices/" TEST_DEVICE_ID MESSAGE_ENDPOINT_HTTP API_VERSION,    /*const char* relativePath,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,                */
        NULL,                                               /*BUFFER_HANDLE requestContent,                                */
        IGNORED_PTR_ARG,                                    /*unsigned int* statusCode,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,               */
        IGNORED_PTR_ARG                                     /*BUFFER_HANDLE responseContent))                              */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(5)
        .IgnoreArgument(7)
        .IgnoreArgument(8)
        .IgnoreArgument(9)
        .CopyOutArgumentBuffer(7, &statusCode200, sizeof(statusCode200));

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_FindHeaderValue(IGNORED_PTR_ARG, "ETag"))
        .IgnoreArgument(1)
        .SetReturn(TEST_ETAG_VALUE);

    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_CreateFromByteArray(IGNORED_PTR_ARG, IGNORED_NUM_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Destroy(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_u_char(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_GetHeaderCount(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);

    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_MessageCallback(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);

    /*this returns "0" so the message needs to be "accepted"*/
    /*this is "accepting"*/
    STRICT_EXPECTED_CALL(mocks, STRING_clone(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_construct_n(TEST_ETAG_VALUE_UNQUOTED, sizeof(TEST_ETAG_VALUE_UNQUOTED) - 1))
        .ValidateArgumentBuffer(1, TEST_ETAG_VALUE_UNQUOTED, sizeof(TEST_ETAG_VALUE_UNQUOTED) - 1);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, API_VERSION))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc());
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "User-Agent", CLIENT_DEVICE_TYPE_PREFIX CLIENT_DEVICE_BACKSLASH IOTHUB_SDK_VERSION))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "Authorization", TEST_BLANK_SAS_TOKEN))
        .IgnoreArgument(1)
        .SetReturn(HTTP_HEADERS_ERROR);

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

/*undefined behavior*/
TEST_FUNCTION(IoTHubTransportHttp_DoWork_happy_path_with_empty_waitingToSend_and_1_service_message_acceptmessage_fails_at_HTTPHeaders_Alloc_succeeds)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    unsigned int statusCode200 = 200;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    (void)IoTHubTransportHttp_Subscribe(devHandle);
    mocks.ResetAllCalls();
    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend)); /*because DoWork for event*/

    STRICT_EXPECTED_CALL(mocks, get_time(NULL));
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc()); /*because responseHeadearsHandle: a new instance of HTTP headers*/
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, BUFFER_new());
    STRICT_EXPECTED_CALL(mocks, BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1); /*because relativePath is a STRING_HANDLE*/
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,                                    /*sasObject handle                                             */
        IGNORED_PTR_ARG,                                    /*HTTPAPIEX_HANDLE handle,                                     */
        HTTPAPI_REQUEST_GET,                                /*HTTPAPI_REQUEST_TYPE requestType,                            */
        "/devices/" TEST_DEVICE_ID MESSAGE_ENDPOINT_HTTP API_VERSION,    /*const char* relativePath,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,                */
        NULL,                                               /*BUFFER_HANDLE requestContent,                                */
        IGNORED_PTR_ARG,                                    /*unsigned int* statusCode,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,               */
        IGNORED_PTR_ARG                                     /*BUFFER_HANDLE responseContent))                              */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(5)
        .IgnoreArgument(7)
        .IgnoreArgument(8)
        .IgnoreArgument(9)
        .CopyOutArgumentBuffer(7, &statusCode200, sizeof(statusCode200));

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_FindHeaderValue(IGNORED_PTR_ARG, "ETag"))
        .IgnoreArgument(1)
        .SetReturn(TEST_ETAG_VALUE);

    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_CreateFromByteArray(IGNORED_PTR_ARG, IGNORED_NUM_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Destroy(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_u_char(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_GetHeaderCount(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);

    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_MessageCallback(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);

    /*this returns "0" so the message needs to be "accepted"*/
    /*this is "accepting"*/
    STRICT_EXPECTED_CALL(mocks, STRING_clone(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_construct_n(TEST_ETAG_VALUE_UNQUOTED, sizeof(TEST_ETAG_VALUE_UNQUOTED) - 1))
        .ValidateArgumentBuffer(1, TEST_ETAG_VALUE_UNQUOTED, sizeof(TEST_ETAG_VALUE_UNQUOTED) - 1);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, API_VERSION))
        .IgnoreArgument(1);

    whenShallHTTPHeaders_Alloc_fail = currentHTTPHeaders_Alloc_call + 2;
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc());


    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

/*undefined behavior*/
TEST_FUNCTION(IoTHubTransportHttp_DoWork_happy_path_with_empty_waitingToSend_and_1_service_message_acceptmessage_fails_at_STRING_concat_succeeds_1)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    unsigned int statusCode200 = 200;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    (void)IoTHubTransportHttp_Subscribe(devHandle);
    mocks.ResetAllCalls();
    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend)); /*because DoWork for event*/

    STRICT_EXPECTED_CALL(mocks, get_time(NULL));
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc()); /*because responseHeadearsHandle: a new instance of HTTP headers*/
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, BUFFER_new());
    STRICT_EXPECTED_CALL(mocks, BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1); /*because relativePath is a STRING_HANDLE*/
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,                                    /*sasObject handle                                             */
        IGNORED_PTR_ARG,                                    /*HTTPAPIEX_HANDLE handle,                                     */
        HTTPAPI_REQUEST_GET,                                /*HTTPAPI_REQUEST_TYPE requestType,                            */
        "/devices/" TEST_DEVICE_ID MESSAGE_ENDPOINT_HTTP API_VERSION,    /*const char* relativePath,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,                */
        NULL,                                               /*BUFFER_HANDLE requestContent,                                */
        IGNORED_PTR_ARG,                                    /*unsigned int* statusCode,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,               */
        IGNORED_PTR_ARG                                     /*BUFFER_HANDLE responseContent))                              */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(5)
        .IgnoreArgument(7)
        .IgnoreArgument(8)
        .IgnoreArgument(9)
        .CopyOutArgumentBuffer(7, &statusCode200, sizeof(statusCode200));

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_FindHeaderValue(IGNORED_PTR_ARG, "ETag"))
        .IgnoreArgument(1)
        .SetReturn(TEST_ETAG_VALUE);

    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_CreateFromByteArray(IGNORED_PTR_ARG, IGNORED_NUM_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Destroy(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_u_char(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_GetHeaderCount(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);

    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_MessageCallback(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);

    /*this returns "0" so the message needs to be "accepted"*/
    /*this is "accepting"*/
    STRICT_EXPECTED_CALL(mocks, STRING_clone(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_construct_n(TEST_ETAG_VALUE_UNQUOTED, sizeof(TEST_ETAG_VALUE_UNQUOTED) - 1))
        .ValidateArgumentBuffer(1, TEST_ETAG_VALUE_UNQUOTED, sizeof(TEST_ETAG_VALUE_UNQUOTED) - 1);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    whenShallSTRING_concat_fail = currentSTRING_concat_call + 1;
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, API_VERSION))
        .IgnoreArgument(1);

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_088: [ If no such header is found or is invalid, then _DoWork shall advance to the next action. ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_happy_path_with_empty_waitingToSend_and_1_service_message_acceptmessage_fails_at_STRING_concat_with_STRING_succeeds_1)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    unsigned int statusCode200 = 200;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    (void)IoTHubTransportHttp_Subscribe(devHandle);
    mocks.ResetAllCalls();
    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend)); /*because DoWork for event*/

    STRICT_EXPECTED_CALL(mocks, get_time(NULL));
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc()); /*because responseHeadearsHandle: a new instance of HTTP headers*/
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, BUFFER_new());
    STRICT_EXPECTED_CALL(mocks, BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1); /*because relativePath is a STRING_HANDLE*/
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,                                    /*sasObject handle                                             */
        IGNORED_PTR_ARG,                                    /*HTTPAPIEX_HANDLE handle,                                     */
        HTTPAPI_REQUEST_GET,                                /*HTTPAPI_REQUEST_TYPE requestType,                            */
        "/devices/" TEST_DEVICE_ID MESSAGE_ENDPOINT_HTTP API_VERSION,    /*const char* relativePath,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,                */
        NULL,                                               /*BUFFER_HANDLE requestContent,                                */
        IGNORED_PTR_ARG,                                    /*unsigned int* statusCode,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,               */
        IGNORED_PTR_ARG                                     /*BUFFER_HANDLE responseContent))                              */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(5)
        .IgnoreArgument(7)
        .IgnoreArgument(8)
        .IgnoreArgument(9)
        .CopyOutArgumentBuffer(7, &statusCode200, sizeof(statusCode200));

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_FindHeaderValue(IGNORED_PTR_ARG, "ETag"))
        .IgnoreArgument(1)
        .SetReturn(TEST_ETAG_VALUE);

    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_CreateFromByteArray(IGNORED_PTR_ARG, IGNORED_NUM_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Destroy(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_u_char(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_GetHeaderCount(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);

    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_MessageCallback(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);

    /*this returns "0" so the message needs to be "accepted"*/
    /*this is "accepting"*/
    STRICT_EXPECTED_CALL(mocks, STRING_clone(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_construct_n(TEST_ETAG_VALUE_UNQUOTED, sizeof(TEST_ETAG_VALUE_UNQUOTED) - 1))
        .ValidateArgumentBuffer(1, TEST_ETAG_VALUE_UNQUOTED, sizeof(TEST_ETAG_VALUE_UNQUOTED) - 1);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    whenShallSTRING_concat_with_STRING_fail = currentSTRING_concat_with_STRING_call + 1;
    STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

/*undefined behavior*/
TEST_FUNCTION(IoTHubTransportHttp_DoWork_happy_path_with_empty_waitingToSend_and_1_service_message_acceptmessage_fails_at_STRING_construct_n_succeeds_2)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    unsigned int statusCode200 = 200;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    (void)IoTHubTransportHttp_Subscribe(devHandle);
    mocks.ResetAllCalls();
    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend)); /*because DoWork for event*/

    STRICT_EXPECTED_CALL(mocks, get_time(NULL));
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc()); /*because responseHeadearsHandle: a new instance of HTTP headers*/
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, BUFFER_new());
    STRICT_EXPECTED_CALL(mocks, BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1); /*because relativePath is a STRING_HANDLE*/
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,                                    /*sasObject handle                                             */
        IGNORED_PTR_ARG,                                    /*HTTPAPIEX_HANDLE handle,                                     */
        HTTPAPI_REQUEST_GET,                                /*HTTPAPI_REQUEST_TYPE requestType,                            */
        "/devices/" TEST_DEVICE_ID MESSAGE_ENDPOINT_HTTP API_VERSION,    /*const char* relativePath,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,                */
        NULL,                                               /*BUFFER_HANDLE requestContent,                                */
        IGNORED_PTR_ARG,                                    /*unsigned int* statusCode,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,               */
        IGNORED_PTR_ARG                                     /*BUFFER_HANDLE responseContent))                              */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(5)
        .IgnoreArgument(7)
        .IgnoreArgument(8)
        .IgnoreArgument(9)
        .CopyOutArgumentBuffer(7, &statusCode200, sizeof(statusCode200));

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_FindHeaderValue(IGNORED_PTR_ARG, "ETag"))
        .IgnoreArgument(1)
        .SetReturn(TEST_ETAG_VALUE);

    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_CreateFromByteArray(IGNORED_PTR_ARG, IGNORED_NUM_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Destroy(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_u_char(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_GetHeaderCount(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);

    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_MessageCallback(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);

    /*this returns "0" so the message needs to be "accepted"*/
    /*this is "accepting"*/
    STRICT_EXPECTED_CALL(mocks, STRING_clone(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    whenShallSTRING_construct_n_fail = currentSTRING_construct_n_call + 1;
    STRICT_EXPECTED_CALL(mocks, STRING_construct_n(TEST_ETAG_VALUE_UNQUOTED, sizeof(TEST_ETAG_VALUE_UNQUOTED) - 1))
        .ValidateArgumentBuffer(1, TEST_ETAG_VALUE_UNQUOTED, sizeof(TEST_ETAG_VALUE_UNQUOTED) - 1);

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

/*undefined behavior*/
TEST_FUNCTION(IoTHubTransportHttp_DoWork_happy_path_with_empty_waitingToSend_and_1_service_message_acceptmessage_fails_at_STRING_clone_succeeds_1)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    unsigned int statusCode200 = 200;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    (void)IoTHubTransportHttp_Subscribe(devHandle);
    mocks.ResetAllCalls();
    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend)); /*because DoWork for event*/

    STRICT_EXPECTED_CALL(mocks, get_time(NULL));
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc()); /*because responseHeadearsHandle: a new instance of HTTP headers*/
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, BUFFER_new());
    STRICT_EXPECTED_CALL(mocks, BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1); /*because relativePath is a STRING_HANDLE*/
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,                                    /*sasObject handle                                             */
        IGNORED_PTR_ARG,                                    /*HTTPAPIEX_HANDLE handle,                                     */
        HTTPAPI_REQUEST_GET,                                /*HTTPAPI_REQUEST_TYPE requestType,                            */
        "/devices/" TEST_DEVICE_ID MESSAGE_ENDPOINT_HTTP API_VERSION,    /*const char* relativePath,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,                */
        NULL,                                               /*BUFFER_HANDLE requestContent,                                */
        IGNORED_PTR_ARG,                                    /*unsigned int* statusCode,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,               */
        IGNORED_PTR_ARG                                     /*BUFFER_HANDLE responseContent))                              */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(5)
        .IgnoreArgument(7)
        .IgnoreArgument(8)
        .IgnoreArgument(9)
        .CopyOutArgumentBuffer(7, &statusCode200, sizeof(statusCode200));

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_FindHeaderValue(IGNORED_PTR_ARG, "ETag"))
        .IgnoreArgument(1)
        .SetReturn(TEST_ETAG_VALUE);

    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_CreateFromByteArray(IGNORED_PTR_ARG, IGNORED_NUM_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Destroy(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_u_char(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_GetHeaderCount(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);

    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_MessageCallback(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);

    /*this returns "0" so the message needs to be "accepted"*/
    /*this is "accepting"*/
    whenShallSTRING_clone_fail = currentSTRING_clone_call + 1;
    STRICT_EXPECTED_CALL(mocks, STRING_clone(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_097: [ _DoWork shall call HTTPAPIEX_SAS_ExecuteRequest with the following parameters:
//
//requestType: POST
//	relativePath : abandon relative path begin(as created by _Create) + value of ETag + "/abandon" + APIVERSION
//	requestHttpHeadersHandle : an HTTP headers instance containing the following
//	Authorization : " "
//	If - Match : value of ETag
//	requestContent : NULL
//	statusCode : a pointer to unsigned int which might be examined for logging
//	responseHeadearsHandle : NULL
//	responseContent : NULL ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_098: [ Abandoning the message is considered successful if the HTTPAPIEX_SAS_ExecuteRequest doesn't fail and the statusCode is 204. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_095: [ If IoTHubClient_LL_MessageCallback returns IOTHUBMESSAGE_REJECTED then _DoWork shall "reject" the message. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_101: [ _DoWork shall call HTTPAPIEX_SAS_ExecuteRequest with the following parameters:
//
//requestType: DELETE
//	relativePath : abandon relative path begin + value of ETag + APIVERSION
//	requestHttpHeadersHandle : an HTTP headers instance containing the following
//	Authorization : " "
//	If - Match : value of ETag
//	requestContent : NULL
//	statusCode : a pointer to unsigned int which might be used by logging
//	responseHeadearsHandle : NULL
//	responseContent : NULL ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_102: [ Rejecting a message is successful when HTTPAPIEX_SAS_ExecuteRequest completes successfully and the status code is 204. ]

TEST_FUNCTION(IoTHubTransportHttp_DoWork_happy_path_with_empty_waitingToSend_and_1_service_message_with_reject_succeeds_1)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    unsigned int statusCode200 = 200;
    unsigned int statusCode204 = 204;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    (void)IoTHubTransportHttp_Subscribe(devHandle);
    mocks.ResetAllCalls();
    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend)); /*because DoWork for event*/

    STRICT_EXPECTED_CALL(mocks, get_time(NULL));
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc()); /*because responseHeadearsHandle: a new instance of HTTP headers*/
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, BUFFER_new());
    STRICT_EXPECTED_CALL(mocks, BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1); /*because relativePath is a STRING_HANDLE*/
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,                                    /*sasObject handle                                             */
        IGNORED_PTR_ARG,                                    /*HTTPAPIEX_HANDLE handle,                                     */
        HTTPAPI_REQUEST_GET,                                /*HTTPAPI_REQUEST_TYPE requestType,                            */
        "/devices/" TEST_DEVICE_ID MESSAGE_ENDPOINT_HTTP API_VERSION,    /*const char* relativePath,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,                */
        NULL,                                               /*BUFFER_HANDLE requestContent,                                */
        IGNORED_PTR_ARG,                                    /*unsigned int* statusCode,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,               */
        IGNORED_PTR_ARG                                     /*BUFFER_HANDLE responseContent))                              */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(5)
        .IgnoreArgument(7)
        .IgnoreArgument(8)
        .IgnoreArgument(9)
        .CopyOutArgumentBuffer(7, &statusCode200, sizeof(statusCode200));

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_FindHeaderValue(IGNORED_PTR_ARG, "ETag"))
        .IgnoreArgument(1)
        .SetReturn(TEST_ETAG_VALUE);

    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_CreateFromByteArray(IGNORED_PTR_ARG, IGNORED_NUM_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Destroy(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_u_char(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_GetHeaderCount(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);

    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_MessageCallback(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .SetReturn(IOTHUBMESSAGE_REJECTED);

    /*this is "reject"*/
    STRICT_EXPECTED_CALL(mocks, STRING_clone(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_construct_n(TEST_ETAG_VALUE_UNQUOTED, sizeof(TEST_ETAG_VALUE_UNQUOTED) - 1))
        .ValidateArgumentBuffer(1, TEST_ETAG_VALUE_UNQUOTED, sizeof(TEST_ETAG_VALUE_UNQUOTED) - 1);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, API_VERSION REJECT_QUERY_PARAMETER))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc());
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "User-Agent", CLIENT_DEVICE_TYPE_PREFIX CLIENT_DEVICE_BACKSLASH IOTHUB_SDK_VERSION))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "Authorization", TEST_BLANK_SAS_TOKEN))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "If-Match", TEST_ETAG_VALUE))
        .IgnoreArgument(1);


    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1); /*because abandon relativePath is a STRING_HANDLE*/
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,                                    /*sasObject handle                                             */
        IGNORED_PTR_ARG,                                    /*HTTPAPIEX_HANDLE handle,                                     */
        HTTPAPI_REQUEST_DELETE,                               /*HTTPAPI_REQUEST_TYPE requestType,                            */
        "/devices/" TEST_DEVICE_ID MESSAGE_ENDPOINT_HTTP_ETAG TEST_ETAG_VALUE_UNQUOTED API_VERSION REJECT_QUERY_PARAMETER,    /*const char* relativePath,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,                */
        NULL,                                               /*BUFFER_HANDLE requestContent,                                */
        IGNORED_PTR_ARG,                                    /*unsigned int* statusCode,                                    */
        NULL,                                               /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,               */
        NULL                                                /*BUFFER_HANDLE responseContent))                              */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(5)
        .IgnoreArgument(7)
        .CopyOutArgumentBuffer(7, &statusCode204, sizeof(statusCode204));

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_095: [ If IoTHubClient_LL_MessageCallback returns IOTHUBMESSAGE_REJECTED then _DoWork shall "reject" the message. ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_happy_path_with_empty_waitingToSend_and_1_service_message_with_abandon_statusCode404_succeeds_1)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    unsigned int statusCode200 = 200;
    unsigned int statusCode404 = 404;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    (void)IoTHubTransportHttp_Subscribe(devHandle);
    mocks.ResetAllCalls();
    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend)); /*because DoWork for event*/

    STRICT_EXPECTED_CALL(mocks, get_time(NULL));
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc()); /*because responseHeadearsHandle: a new instance of HTTP headers*/
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, BUFFER_new());
    STRICT_EXPECTED_CALL(mocks, BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1); /*because relativePath is a STRING_HANDLE*/
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,                                    /*sasObject handle                                             */
        IGNORED_PTR_ARG,                                    /*HTTPAPIEX_HANDLE handle,                                     */
        HTTPAPI_REQUEST_GET,                                /*HTTPAPI_REQUEST_TYPE requestType,                            */
        "/devices/" TEST_DEVICE_ID MESSAGE_ENDPOINT_HTTP API_VERSION,    /*const char* relativePath,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,                */
        NULL,                                               /*BUFFER_HANDLE requestContent,                                */
        IGNORED_PTR_ARG,                                    /*unsigned int* statusCode,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,               */
        IGNORED_PTR_ARG                                     /*BUFFER_HANDLE responseContent))                              */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(5)
        .IgnoreArgument(7)
        .IgnoreArgument(8)
        .IgnoreArgument(9)
        .CopyOutArgumentBuffer(7, &statusCode200, sizeof(statusCode200));

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_FindHeaderValue(IGNORED_PTR_ARG, "ETag"))
        .IgnoreArgument(1)
        .SetReturn(TEST_ETAG_VALUE);

    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_CreateFromByteArray(IGNORED_PTR_ARG, IGNORED_NUM_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Destroy(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_u_char(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_GetHeaderCount(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);

    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_MessageCallback(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .SetReturn(IOTHUBMESSAGE_REJECTED);

    /*this is "reject"*/
    STRICT_EXPECTED_CALL(mocks, STRING_clone(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_construct_n(TEST_ETAG_VALUE_UNQUOTED, sizeof(TEST_ETAG_VALUE_UNQUOTED) - 1))
        .ValidateArgumentBuffer(1, TEST_ETAG_VALUE_UNQUOTED, sizeof(TEST_ETAG_VALUE_UNQUOTED) - 1);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, API_VERSION REJECT_QUERY_PARAMETER))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc());
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "User-Agent", CLIENT_DEVICE_TYPE_PREFIX CLIENT_DEVICE_BACKSLASH IOTHUB_SDK_VERSION))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "Authorization", TEST_BLANK_SAS_TOKEN))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "If-Match", TEST_ETAG_VALUE))
        .IgnoreArgument(1);


    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1); /*because abandon relativePath is a STRING_HANDLE*/
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,                                    /*sasObject handle                                             */
        IGNORED_PTR_ARG,                                    /*HTTPAPIEX_HANDLE handle,                                     */
        HTTPAPI_REQUEST_DELETE,                               /*HTTPAPI_REQUEST_TYPE requestType,                            */
        "/devices/" TEST_DEVICE_ID MESSAGE_ENDPOINT_HTTP_ETAG TEST_ETAG_VALUE_UNQUOTED API_VERSION REJECT_QUERY_PARAMETER,    /*const char* relativePath,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,                */
        NULL,                                               /*BUFFER_HANDLE requestContent,                                */
        IGNORED_PTR_ARG,                                    /*unsigned int* statusCode,                                    */
        NULL,                                               /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,               */
        NULL                                                /*BUFFER_HANDLE responseContent))                              */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(4)
        .IgnoreArgument(5)
        .CopyOutArgumentBuffer(7, &statusCode404, sizeof(statusCode404));

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_095: [ If IoTHubClient_LL_MessageCallback returns IOTHUBMESSAGE_REJECTED then _DoWork shall "reject" the message. ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_happy_path_with_empty_waitingToSend_and_1_service_message_with_HTTPAPIEX_SAS_ExecuteRequest2_fails_succeeds)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    unsigned int statusCode200 = 200;
    unsigned int statusCode404 = 404;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    (void)IoTHubTransportHttp_Subscribe(devHandle);
    mocks.ResetAllCalls();
    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend)); /*because DoWork for event*/

    STRICT_EXPECTED_CALL(mocks, get_time(NULL));
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc()); /*because responseHeadearsHandle: a new instance of HTTP headers*/
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, BUFFER_new());
    STRICT_EXPECTED_CALL(mocks, BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1); /*because relativePath is a STRING_HANDLE*/
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,                                    /*sasObject handle                                             */
        IGNORED_PTR_ARG,                                    /*HTTPAPIEX_HANDLE handle,                                     */
        HTTPAPI_REQUEST_GET,                                /*HTTPAPI_REQUEST_TYPE requestType,                            */
        "/devices/" TEST_DEVICE_ID MESSAGE_ENDPOINT_HTTP API_VERSION,    /*const char* relativePath,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,                */
        NULL,                                               /*BUFFER_HANDLE requestContent,                                */
        IGNORED_PTR_ARG,                                    /*unsigned int* statusCode,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,               */
        IGNORED_PTR_ARG                                     /*BUFFER_HANDLE responseContent))                              */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(5)
        .IgnoreArgument(7)
        .IgnoreArgument(8)
        .IgnoreArgument(9)
        .CopyOutArgumentBuffer(7, &statusCode200, sizeof(statusCode200));

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_FindHeaderValue(IGNORED_PTR_ARG, "ETag"))
        .IgnoreArgument(1)
        .SetReturn(TEST_ETAG_VALUE);

    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_CreateFromByteArray(IGNORED_PTR_ARG, IGNORED_NUM_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Destroy(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_u_char(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_GetHeaderCount(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);

    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_MessageCallback(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .SetReturn(IOTHUBMESSAGE_REJECTED);

    /*this is "reject"*/
    STRICT_EXPECTED_CALL(mocks, STRING_clone(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_construct_n(TEST_ETAG_VALUE_UNQUOTED, sizeof(TEST_ETAG_VALUE_UNQUOTED) - 1))
        .ValidateArgumentBuffer(1, TEST_ETAG_VALUE_UNQUOTED, sizeof(TEST_ETAG_VALUE_UNQUOTED) - 1);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, API_VERSION REJECT_QUERY_PARAMETER))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc());
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "User-Agent", CLIENT_DEVICE_TYPE_PREFIX CLIENT_DEVICE_BACKSLASH IOTHUB_SDK_VERSION))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "Authorization", TEST_BLANK_SAS_TOKEN))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "If-Match", TEST_ETAG_VALUE))
        .IgnoreArgument(1);


    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1); /*because abandon relativePath is a STRING_HANDLE*/
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,                                    /*sasObject handle                                             */
        IGNORED_PTR_ARG,                                    /*HTTPAPIEX_HANDLE handle,                                     */
        HTTPAPI_REQUEST_DELETE,                               /*HTTPAPI_REQUEST_TYPE requestType,                            */
        "/devices/" TEST_DEVICE_ID MESSAGE_ENDPOINT_HTTP_ETAG TEST_ETAG_VALUE_UNQUOTED API_VERSION REJECT_QUERY_PARAMETER,    /*const char* relativePath,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,                */
        NULL,                                               /*BUFFER_HANDLE requestContent,                                */
        IGNORED_PTR_ARG,                                    /*unsigned int* statusCode,                                    */
        NULL,                                               /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,               */
        NULL                                                /*BUFFER_HANDLE responseContent))                              */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(5)
        .IgnoreArgument(7)
        .CopyOutArgumentBuffer(7, &statusCode404, sizeof(statusCode404))
        .SetReturn(HTTPAPIEX_ERROR);

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_095: [ If IoTHubClient_LL_MessageCallback returns IOTHUBMESSAGE_REJECTED then _DoWork shall "reject" the message. ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_happy_path_with_empty_waitingToSend_and_1_service_message_with_HTTPHeaders_AddHeaderNameValuePair_fails_succeeds_1)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    unsigned int statusCode200 = 200;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    (void)IoTHubTransportHttp_Subscribe(devHandle);
    mocks.ResetAllCalls();
    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend)); /*because DoWork for event*/

    STRICT_EXPECTED_CALL(mocks, get_time(NULL));
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc()); /*because responseHeadearsHandle: a new instance of HTTP headers*/
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, BUFFER_new());
    STRICT_EXPECTED_CALL(mocks, BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1); /*because relativePath is a STRING_HANDLE*/
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,                                    /*sasObject handle                                             */
        IGNORED_PTR_ARG,                                    /*HTTPAPIEX_HANDLE handle,                                     */
        HTTPAPI_REQUEST_GET,                                /*HTTPAPI_REQUEST_TYPE requestType,                            */
        "/devices/" TEST_DEVICE_ID MESSAGE_ENDPOINT_HTTP API_VERSION,    /*const char* relativePath,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,                */
        NULL,                                               /*BUFFER_HANDLE requestContent,                                */
        IGNORED_PTR_ARG,                                    /*unsigned int* statusCode,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,               */
        IGNORED_PTR_ARG                                     /*BUFFER_HANDLE responseContent))                              */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(5)
        .IgnoreArgument(7)
        .IgnoreArgument(8)
        .IgnoreArgument(9)
        .CopyOutArgumentBuffer(7, &statusCode200, sizeof(statusCode200));

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_FindHeaderValue(IGNORED_PTR_ARG, "ETag"))
        .IgnoreArgument(1)
        .SetReturn(TEST_ETAG_VALUE);

    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_CreateFromByteArray(IGNORED_PTR_ARG, IGNORED_NUM_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Destroy(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_u_char(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_GetHeaderCount(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);

    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_MessageCallback(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .SetReturn(IOTHUBMESSAGE_REJECTED);

    /*this is "reject"*/
    STRICT_EXPECTED_CALL(mocks, STRING_clone(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_construct_n(TEST_ETAG_VALUE_UNQUOTED, sizeof(TEST_ETAG_VALUE_UNQUOTED) - 1))
        .ValidateArgumentBuffer(1, TEST_ETAG_VALUE_UNQUOTED, sizeof(TEST_ETAG_VALUE_UNQUOTED) - 1);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, API_VERSION REJECT_QUERY_PARAMETER))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc());
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "User-Agent", CLIENT_DEVICE_TYPE_PREFIX CLIENT_DEVICE_BACKSLASH IOTHUB_SDK_VERSION))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "Authorization", TEST_BLANK_SAS_TOKEN))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "If-Match", TEST_ETAG_VALUE))
        .IgnoreArgument(1)
        .SetReturn(HTTP_HEADERS_ERROR);


    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_095: [ If IoTHubClient_LL_MessageCallback returns IOTHUBMESSAGE_REJECTED then _DoWork shall "reject" the message. ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_happy_path_with_empty_waitingToSend_and_1_service_message_with_HTTPHeaders_AddHeaderNameValuePair_fails_succeeds_2)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    unsigned int statusCode200 = 200;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    (void)IoTHubTransportHttp_Subscribe(devHandle);
    mocks.ResetAllCalls();
    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend)); /*because DoWork for event*/

    STRICT_EXPECTED_CALL(mocks, get_time(NULL));
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc()); /*because responseHeadearsHandle: a new instance of HTTP headers*/
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, BUFFER_new());
    STRICT_EXPECTED_CALL(mocks, BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1); /*because relativePath is a STRING_HANDLE*/
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,                                    /*sasObject handle                                             */
        IGNORED_PTR_ARG,                                    /*HTTPAPIEX_HANDLE handle,                                     */
        HTTPAPI_REQUEST_GET,                                /*HTTPAPI_REQUEST_TYPE requestType,                            */
        "/devices/" TEST_DEVICE_ID MESSAGE_ENDPOINT_HTTP API_VERSION,    /*const char* relativePath,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,                */
        NULL,                                               /*BUFFER_HANDLE requestContent,                                */
        IGNORED_PTR_ARG,                                    /*unsigned int* statusCode,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,               */
        IGNORED_PTR_ARG                                     /*BUFFER_HANDLE responseContent))                              */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(5)
        .IgnoreArgument(7)
        .IgnoreArgument(8)
        .IgnoreArgument(9)
        .CopyOutArgumentBuffer(7, &statusCode200, sizeof(statusCode200));

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_FindHeaderValue(IGNORED_PTR_ARG, "ETag"))
        .IgnoreArgument(1)
        .SetReturn(TEST_ETAG_VALUE);

    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_CreateFromByteArray(IGNORED_PTR_ARG, IGNORED_NUM_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Destroy(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_u_char(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_GetHeaderCount(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);

    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_MessageCallback(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .SetReturn(IOTHUBMESSAGE_REJECTED); /*1 means "do not acept it"*/

                                            /*this is "reject"*/
    STRICT_EXPECTED_CALL(mocks, STRING_clone(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_construct_n(TEST_ETAG_VALUE_UNQUOTED, sizeof(TEST_ETAG_VALUE_UNQUOTED) - 1))
        .ValidateArgumentBuffer(1, TEST_ETAG_VALUE_UNQUOTED, sizeof(TEST_ETAG_VALUE_UNQUOTED) - 1);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, API_VERSION REJECT_QUERY_PARAMETER))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc());
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "User-Agent", CLIENT_DEVICE_TYPE_PREFIX CLIENT_DEVICE_BACKSLASH IOTHUB_SDK_VERSION))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "Authorization", TEST_BLANK_SAS_TOKEN))
        .IgnoreArgument(1)
        .SetReturn(HTTP_HEADERS_ERROR);


    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_095: [ If IoTHubClient_LL_MessageCallback returns IOTHUBMESSAGE_REJECTED then _DoWork shall "reject" the message. ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_happy_path_with_empty_waitingToSend_and_1_service_message_with_HTTPHeaders_Alloc_fails_succeeds)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    unsigned int statusCode200 = 200;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    (void)IoTHubTransportHttp_Subscribe(devHandle);
    mocks.ResetAllCalls();
    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend)); /*because DoWork for event*/

    STRICT_EXPECTED_CALL(mocks, get_time(NULL));
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc()); /*because responseHeadearsHandle: a new instance of HTTP headers*/
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, BUFFER_new());
    STRICT_EXPECTED_CALL(mocks, BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1); /*because relativePath is a STRING_HANDLE*/
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,                                    /*sasObject handle                                             */
        IGNORED_PTR_ARG,                                    /*HTTPAPIEX_HANDLE handle,                                     */
        HTTPAPI_REQUEST_GET,                                /*HTTPAPI_REQUEST_TYPE requestType,                            */
        "/devices/" TEST_DEVICE_ID MESSAGE_ENDPOINT_HTTP API_VERSION,    /*const char* relativePath,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,                */
        NULL,                                               /*BUFFER_HANDLE requestContent,                                */
        IGNORED_PTR_ARG,                                    /*unsigned int* statusCode,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,               */
        IGNORED_PTR_ARG                                     /*BUFFER_HANDLE responseContent))                              */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(5)
        .IgnoreArgument(7)
        .IgnoreArgument(8)
        .IgnoreArgument(9)
        .CopyOutArgumentBuffer(7, &statusCode200, sizeof(statusCode200));

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_FindHeaderValue(IGNORED_PTR_ARG, "ETag"))
        .IgnoreArgument(1)
        .SetReturn(TEST_ETAG_VALUE);

    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_CreateFromByteArray(IGNORED_PTR_ARG, IGNORED_NUM_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Destroy(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_u_char(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_GetHeaderCount(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);

    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_MessageCallback(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .SetReturn(IOTHUBMESSAGE_REJECTED);

    /*this is "reject"*/
    STRICT_EXPECTED_CALL(mocks, STRING_clone(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_construct_n(TEST_ETAG_VALUE_UNQUOTED, sizeof(TEST_ETAG_VALUE_UNQUOTED) - 1))
        .ValidateArgumentBuffer(1, TEST_ETAG_VALUE_UNQUOTED, sizeof(TEST_ETAG_VALUE_UNQUOTED) - 1);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, API_VERSION REJECT_QUERY_PARAMETER))
        .IgnoreArgument(1);

    whenShallHTTPHeaders_Alloc_fail = currentHTTPHeaders_Alloc_call + 2;
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc());

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_095: [ If IoTHubClient_LL_MessageCallback returns IOTHUBMESSAGE_REJECTED then _DoWork shall "reject" the message. ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_happy_path_with_empty_waitingToSend_and_1_service_message_with_STRING_concat_fails_succeeds_1)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    unsigned int statusCode200 = 200;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    (void)IoTHubTransportHttp_Subscribe(devHandle);
    mocks.ResetAllCalls();
    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend)); /*because DoWork for event*/

    STRICT_EXPECTED_CALL(mocks, get_time(NULL));
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc()); /*because responseHeadearsHandle: a new instance of HTTP headers*/
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, BUFFER_new());
    STRICT_EXPECTED_CALL(mocks, BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1); /*because relativePath is a STRING_HANDLE*/
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,                                    /*sasObject handle                                             */
        IGNORED_PTR_ARG,                                    /*HTTPAPIEX_HANDLE handle,                                     */
        HTTPAPI_REQUEST_GET,                                /*HTTPAPI_REQUEST_TYPE requestType,                            */
        "/devices/" TEST_DEVICE_ID MESSAGE_ENDPOINT_HTTP API_VERSION,    /*const char* relativePath,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,                */
        NULL,                                               /*BUFFER_HANDLE requestContent,                                */
        IGNORED_PTR_ARG,                                    /*unsigned int* statusCode,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,               */
        IGNORED_PTR_ARG                                     /*BUFFER_HANDLE responseContent))                              */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(5)
        .IgnoreArgument(7)
        .IgnoreArgument(8)
        .IgnoreArgument(9)
        .CopyOutArgumentBuffer(7, &statusCode200, sizeof(statusCode200));

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_FindHeaderValue(IGNORED_PTR_ARG, "ETag"))
        .IgnoreArgument(1)
        .SetReturn(TEST_ETAG_VALUE);

    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_CreateFromByteArray(IGNORED_PTR_ARG, IGNORED_NUM_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Destroy(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_u_char(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_GetHeaderCount(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);

    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_MessageCallback(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .SetReturn(IOTHUBMESSAGE_REJECTED);

    /*this is "reject"*/
    STRICT_EXPECTED_CALL(mocks, STRING_clone(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_construct_n(TEST_ETAG_VALUE_UNQUOTED, sizeof(TEST_ETAG_VALUE_UNQUOTED) - 1))
        .ValidateArgumentBuffer(1, TEST_ETAG_VALUE_UNQUOTED, sizeof(TEST_ETAG_VALUE_UNQUOTED) - 1);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    whenShallSTRING_concat_fail = currentSTRING_concat_call + 1;
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, API_VERSION REJECT_QUERY_PARAMETER))
        .IgnoreArgument(1);

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_095: [ If IoTHubClient_LL_MessageCallback returns IOTHUBMESSAGE_REJECTED then _DoWork shall "reject" the message. ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_happy_path_with_empty_waitingToSend_and_1_service_message_with_STRING_concat_with_STRING_fails_succeeds_2)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    unsigned int statusCode200 = 200;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    (void)IoTHubTransportHttp_Subscribe(devHandle);
    mocks.ResetAllCalls();
    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend)); /*because DoWork for event*/

    STRICT_EXPECTED_CALL(mocks, get_time(NULL));
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc()); /*because responseHeadearsHandle: a new instance of HTTP headers*/
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, BUFFER_new());
    STRICT_EXPECTED_CALL(mocks, BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1); /*because relativePath is a STRING_HANDLE*/
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,                                    /*sasObject handle                                             */
        IGNORED_PTR_ARG,                                    /*HTTPAPIEX_HANDLE handle,                                     */
        HTTPAPI_REQUEST_GET,                                /*HTTPAPI_REQUEST_TYPE requestType,                            */
        "/devices/" TEST_DEVICE_ID MESSAGE_ENDPOINT_HTTP API_VERSION,    /*const char* relativePath,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,                */
        NULL,                                               /*BUFFER_HANDLE requestContent,                                */
        IGNORED_PTR_ARG,                                    /*unsigned int* statusCode,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,               */
        IGNORED_PTR_ARG                                     /*BUFFER_HANDLE responseContent))                              */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(5)
        .IgnoreArgument(7)
        .IgnoreArgument(8)
        .IgnoreArgument(9)
        .CopyOutArgumentBuffer(7, &statusCode200, sizeof(statusCode200));

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_FindHeaderValue(IGNORED_PTR_ARG, "ETag"))
        .IgnoreArgument(1)
        .SetReturn(TEST_ETAG_VALUE);

    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_CreateFromByteArray(IGNORED_PTR_ARG, IGNORED_NUM_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Destroy(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_u_char(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_GetHeaderCount(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);

    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_MessageCallback(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .SetReturn(IOTHUBMESSAGE_REJECTED);

    /*this is "reject"*/
    STRICT_EXPECTED_CALL(mocks, STRING_clone(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_construct_n(TEST_ETAG_VALUE_UNQUOTED, sizeof(TEST_ETAG_VALUE_UNQUOTED) - 1))
        .ValidateArgumentBuffer(1, TEST_ETAG_VALUE_UNQUOTED, sizeof(TEST_ETAG_VALUE_UNQUOTED) - 1);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    whenShallSTRING_concat_with_STRING_fail = currentSTRING_concat_with_STRING_call + 1;
    STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_095: [ If IoTHubClient_LL_MessageCallback returns IOTHUBMESSAGE_REJECTED then _DoWork shall "reject" the message. ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_happy_path_with_empty_waitingToSend_and_1_service_message_with_STRING_construct_n_fails_succeeds_2)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    unsigned int statusCode200 = 200;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    (void)IoTHubTransportHttp_Subscribe(devHandle);
    mocks.ResetAllCalls();
    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend)); /*because DoWork for event*/

    STRICT_EXPECTED_CALL(mocks, get_time(NULL));
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc()); /*because responseHeadearsHandle: a new instance of HTTP headers*/
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, BUFFER_new());
    STRICT_EXPECTED_CALL(mocks, BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1); /*because relativePath is a STRING_HANDLE*/
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,                                    /*sasObject handle                                             */
        IGNORED_PTR_ARG,                                    /*HTTPAPIEX_HANDLE handle,                                     */
        HTTPAPI_REQUEST_GET,                                /*HTTPAPI_REQUEST_TYPE requestType,                            */
        "/devices/" TEST_DEVICE_ID MESSAGE_ENDPOINT_HTTP API_VERSION,    /*const char* relativePath,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,                */
        NULL,                                               /*BUFFER_HANDLE requestContent,                                */
        IGNORED_PTR_ARG,                                    /*unsigned int* statusCode,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,               */
        IGNORED_PTR_ARG                                     /*BUFFER_HANDLE responseContent))                              */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(5)
        .IgnoreArgument(7)
        .IgnoreArgument(8)
        .IgnoreArgument(9)
        .CopyOutArgumentBuffer(7, &statusCode200, sizeof(statusCode200));

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_FindHeaderValue(IGNORED_PTR_ARG, "ETag"))
        .IgnoreArgument(1)
        .SetReturn(TEST_ETAG_VALUE);

    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_CreateFromByteArray(IGNORED_PTR_ARG, IGNORED_NUM_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Destroy(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_u_char(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_GetHeaderCount(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);

    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_MessageCallback(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .SetReturn(IOTHUBMESSAGE_REJECTED);

    /*this is "reject"*/
    STRICT_EXPECTED_CALL(mocks, STRING_clone(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    whenShallSTRING_construct_n_fail = currentSTRING_construct_n_call + 1;
    STRICT_EXPECTED_CALL(mocks, STRING_construct_n(TEST_ETAG_VALUE_UNQUOTED, sizeof(TEST_ETAG_VALUE_UNQUOTED) - 1))
        .ValidateArgumentBuffer(1, TEST_ETAG_VALUE_UNQUOTED, sizeof(TEST_ETAG_VALUE_UNQUOTED) - 1);

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_095: [ If IoTHubClient_LL_MessageCallback returns IOTHUBMESSAGE_REJECTED then _DoWork shall "reject" the message. ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_happy_path_with_empty_waitingToSend_and_1_service_message_with_STRING_clone_fails_succeeds)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    unsigned int statusCode200 = 200;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    (void)IoTHubTransportHttp_Subscribe(devHandle);
    mocks.ResetAllCalls();
    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend)); /*because DoWork for event*/

    STRICT_EXPECTED_CALL(mocks, get_time(NULL));
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc()); /*because responseHeadearsHandle: a new instance of HTTP headers*/
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, BUFFER_new());
    STRICT_EXPECTED_CALL(mocks, BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1); /*because relativePath is a STRING_HANDLE*/
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,                                    /*sasObject handle                                             */
        IGNORED_PTR_ARG,                                    /*HTTPAPIEX_HANDLE handle,                                     */
        HTTPAPI_REQUEST_GET,                                /*HTTPAPI_REQUEST_TYPE requestType,                            */
        "/devices/" TEST_DEVICE_ID MESSAGE_ENDPOINT_HTTP API_VERSION,    /*const char* relativePath,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,                */
        NULL,                                               /*BUFFER_HANDLE requestContent,                                */
        IGNORED_PTR_ARG,                                    /*unsigned int* statusCode,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,               */
        IGNORED_PTR_ARG                                     /*BUFFER_HANDLE responseContent))                              */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(5)
        .IgnoreArgument(7)
        .IgnoreArgument(8)
        .IgnoreArgument(9)
        .CopyOutArgumentBuffer(7, &statusCode200, sizeof(statusCode200));

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_FindHeaderValue(IGNORED_PTR_ARG, "ETag"))
        .IgnoreArgument(1)
        .SetReturn(TEST_ETAG_VALUE);

    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_CreateFromByteArray(IGNORED_PTR_ARG, IGNORED_NUM_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Destroy(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_u_char(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_GetHeaderCount(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);

    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_MessageCallback(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .SetReturn(IOTHUBMESSAGE_REJECTED);

    /*this is "reject"*/
    whenShallSTRING_clone_fail = currentSTRING_clone_call + 1;
    STRICT_EXPECTED_CALL(mocks, STRING_clone(IGNORED_PTR_ARG))
        .IgnoreArgument(1);


    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_092: [ If assembling the message fails in any way, then _DoWork shall "abandon" the message. ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_happy_path_with_empty_waitingToSend_and_1_service_message_abandons_when_IoTHubMessage_Create_fails)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    unsigned int statusCode200 = 200;
    unsigned int statusCode204 = 204;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    (void)IoTHubTransportHttp_Subscribe(devHandle);
    mocks.ResetAllCalls();
    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend)); /*because DoWork for event*/

    STRICT_EXPECTED_CALL(mocks, get_time(NULL));
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc()); /*because responseHeadearsHandle: a new instance of HTTP headers*/
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, BUFFER_new());
    STRICT_EXPECTED_CALL(mocks, BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1); /*because relativePath is a STRING_HANDLE*/
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,                                    /*sasObject handle                                             */
        IGNORED_PTR_ARG,                                    /*HTTPAPIEX_HANDLE handle,                                     */
        HTTPAPI_REQUEST_GET,                                /*HTTPAPI_REQUEST_TYPE requestType,                            */
        "/devices/" TEST_DEVICE_ID MESSAGE_ENDPOINT_HTTP API_VERSION,    /*const char* relativePath,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,                */
        NULL,                                               /*BUFFER_HANDLE requestContent,                                */
        IGNORED_PTR_ARG,                                    /*unsigned int* statusCode,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,               */
        IGNORED_PTR_ARG                                     /*BUFFER_HANDLE responseContent))                              */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(5)
        .IgnoreArgument(7)
        .IgnoreArgument(8)
        .IgnoreArgument(9)
        .CopyOutArgumentBuffer(7, &statusCode200, sizeof(statusCode200));

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_FindHeaderValue(IGNORED_PTR_ARG, "ETag"))
        .IgnoreArgument(1)
        .SetReturn(TEST_ETAG_VALUE);

    STRICT_EXPECTED_CALL(mocks, BUFFER_u_char(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_CreateFromByteArray(IGNORED_PTR_ARG, IGNORED_NUM_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .SetReturn((IOTHUB_MESSAGE_HANDLE)NULL);

    /*this is "abandon"*/
    STRICT_EXPECTED_CALL(mocks, STRING_clone(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_construct_n(TEST_ETAG_VALUE_UNQUOTED, sizeof(TEST_ETAG_VALUE_UNQUOTED) - 1))
        .ValidateArgumentBuffer(1, TEST_ETAG_VALUE_UNQUOTED, sizeof(TEST_ETAG_VALUE_UNQUOTED) - 1);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, "/abandon" API_VERSION))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc());
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "User-Agent", CLIENT_DEVICE_TYPE_PREFIX CLIENT_DEVICE_BACKSLASH IOTHUB_SDK_VERSION))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "Authorization", TEST_BLANK_SAS_TOKEN))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "If-Match", TEST_ETAG_VALUE))
        .IgnoreArgument(1);


    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1); /*because abandon relativePath is a STRING_HANDLE*/
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,                                    /*sasObject handle                                             */
        IGNORED_PTR_ARG,                                    /*HTTPAPIEX_HANDLE handle,                                     */
        HTTPAPI_REQUEST_POST,                               /*HTTPAPI_REQUEST_TYPE requestType,                            */
        "/devices/" TEST_DEVICE_ID MESSAGE_ENDPOINT_HTTP_ETAG TEST_ETAG_VALUE_UNQUOTED "/abandon" API_VERSION,    /*const char* relativePath,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,                */
        NULL,                                               /*BUFFER_HANDLE requestContent,                                */
        IGNORED_PTR_ARG,                                    /*unsigned int* statusCode,                                    */
        NULL,                                               /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,               */
        NULL                                                /*BUFFER_HANDLE responseContent))                              */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(5)
        .IgnoreArgument(7)
        .CopyOutArgumentBuffer(7, &statusCode204, sizeof(statusCode204));

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_088: [ If no such header is found or is invalid, then _DoWork shall advance to the next action. ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_happy_path_with_empty_waitingToSend_and_1_service_message_fails_when_no_ETag_header_fails)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    unsigned int statusCode200 = 200;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    (void)IoTHubTransportHttp_Subscribe(devHandle);
    mocks.ResetAllCalls();
    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend)); /*because DoWork for event*/

    STRICT_EXPECTED_CALL(mocks, get_time(NULL));
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc()); /*because responseHeadearsHandle: a new instance of HTTP headers*/
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, BUFFER_new());
    STRICT_EXPECTED_CALL(mocks, BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1); /*because relativePath is a STRING_HANDLE*/
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,                                    /*sasObject handle                                             */
        IGNORED_PTR_ARG,                                    /*HTTPAPIEX_HANDLE handle,                                     */
        HTTPAPI_REQUEST_GET,                                /*HTTPAPI_REQUEST_TYPE requestType,                            */
        "/devices/" TEST_DEVICE_ID MESSAGE_ENDPOINT_HTTP API_VERSION,    /*const char* relativePath,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,                */
        NULL,                                               /*BUFFER_HANDLE requestContent,                                */
        IGNORED_PTR_ARG,                                    /*unsigned int* statusCode,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,               */
        IGNORED_PTR_ARG                                     /*BUFFER_HANDLE responseContent))                              */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(5)
        .IgnoreArgument(7)
        .IgnoreArgument(8)
        .IgnoreArgument(9)
        .CopyOutArgumentBuffer(7, &statusCode200, sizeof(statusCode200));

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_FindHeaderValue(IGNORED_PTR_ARG, "ETag"))
        .IgnoreArgument(1)
        .SetReturn((const char*)NULL);

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_088: [ If no such header is found or is invalid, then _DoWork shall advance to the next action. ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_happy_path_with_empty_waitingToSend_and_1_service_message_fails_when_ETag_zero_characters_fails)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    unsigned int statusCode200 = 200;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    (void)IoTHubTransportHttp_Subscribe(devHandle);
    mocks.ResetAllCalls();
    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend)); /*because DoWork for event*/

    STRICT_EXPECTED_CALL(mocks, get_time(NULL));
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc()); /*because responseHeadearsHandle: a new instance of HTTP headers*/
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, BUFFER_new());
    STRICT_EXPECTED_CALL(mocks, BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1); /*because relativePath is a STRING_HANDLE*/
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,                                    /*sasObject handle                                             */
        IGNORED_PTR_ARG,                                    /*HTTPAPIEX_HANDLE handle,                                     */
        HTTPAPI_REQUEST_GET,                                /*HTTPAPI_REQUEST_TYPE requestType,                            */
        "/devices/" TEST_DEVICE_ID MESSAGE_ENDPOINT_HTTP API_VERSION,    /*const char* relativePath,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,                */
        NULL,                                               /*BUFFER_HANDLE requestContent,                                */
        IGNORED_PTR_ARG,                                    /*unsigned int* statusCode,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,               */
        IGNORED_PTR_ARG                                     /*BUFFER_HANDLE responseContent))                              */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(5)
        .IgnoreArgument(7)
        .IgnoreArgument(8)
        .IgnoreArgument(9)
        .CopyOutArgumentBuffer(7, &statusCode200, sizeof(statusCode200));

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_FindHeaderValue(IGNORED_PTR_ARG, "ETag"))
        .IgnoreArgument(1)
        .SetReturn("");

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_088: [ If no such header is found or is invalid, then _DoWork shall advance to the next action. ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_happy_path_with_empty_waitingToSend_and_1_service_message_fails_when_ETag_1_characters_not_quote_fails)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    unsigned int statusCode200 = 200;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    (void)IoTHubTransportHttp_Subscribe(devHandle);
    mocks.ResetAllCalls();
    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend)); /*because DoWork for event*/

    STRICT_EXPECTED_CALL(mocks, get_time(NULL));
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc()); /*because responseHeadearsHandle: a new instance of HTTP headers*/
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, BUFFER_new());
    STRICT_EXPECTED_CALL(mocks, BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1); /*because relativePath is a STRING_HANDLE*/
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,                                    /*sasObject handle                                             */
        IGNORED_PTR_ARG,                                    /*HTTPAPIEX_HANDLE handle,                                     */
        HTTPAPI_REQUEST_GET,                                /*HTTPAPI_REQUEST_TYPE requestType,                            */
        "/devices/" TEST_DEVICE_ID MESSAGE_ENDPOINT_HTTP API_VERSION,    /*const char* relativePath,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,                */
        NULL,                                               /*BUFFER_HANDLE requestContent,                                */
        IGNORED_PTR_ARG,                                    /*unsigned int* statusCode,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,               */
        IGNORED_PTR_ARG                                     /*BUFFER_HANDLE responseContent))                              */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(5)
        .IgnoreArgument(7)
        .IgnoreArgument(8)
        .IgnoreArgument(9)
        .CopyOutArgumentBuffer(7, &statusCode200, sizeof(statusCode200));

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_FindHeaderValue(IGNORED_PTR_ARG, "ETag"))
        .IgnoreArgument(1)
        .SetReturn("a");

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_088: [ If no such header is found or is invalid, then _DoWork shall advance to the next action. ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_happy_path_with_empty_waitingToSend_and_1_service_message_fails_when_ETag_2_characters_last_not_quote_fails)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    unsigned int statusCode200 = 200;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    (void)IoTHubTransportHttp_Subscribe(devHandle);
    mocks.ResetAllCalls();
    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend)); /*because DoWork for event*/

    STRICT_EXPECTED_CALL(mocks, get_time(NULL));
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc()); /*because responseHeadearsHandle: a new instance of HTTP headers*/
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, BUFFER_new());
    STRICT_EXPECTED_CALL(mocks, BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1); /*because relativePath is a STRING_HANDLE*/
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,                                    /*sasObject handle                                             */
        IGNORED_PTR_ARG,                                    /*HTTPAPIEX_HANDLE handle,                                     */
        HTTPAPI_REQUEST_GET,                                /*HTTPAPI_REQUEST_TYPE requestType,                            */
        "/devices/" TEST_DEVICE_ID MESSAGE_ENDPOINT_HTTP API_VERSION,    /*const char* relativePath,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,                */
        NULL,                                               /*BUFFER_HANDLE requestContent,                                */
        IGNORED_PTR_ARG,                                    /*unsigned int* statusCode,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,               */
        IGNORED_PTR_ARG                                     /*BUFFER_HANDLE responseContent))                              */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(5)
        .IgnoreArgument(7)
        .IgnoreArgument(8)
        .IgnoreArgument(9)
        .CopyOutArgumentBuffer(7, &statusCode200, sizeof(statusCode200));

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_FindHeaderValue(IGNORED_PTR_ARG, "ETag"))
        .IgnoreArgument(1)
        .SetReturn("\"a");

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_088: [ If no such header is found or is invalid, then _DoWork shall advance to the next action. ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_happy_path_with_empty_waitingToSend_and_1_service_message_fails_when_ETag_1_characters_quote_fails)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    unsigned int statusCode200 = 200;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    (void)IoTHubTransportHttp_Subscribe(devHandle);
    mocks.ResetAllCalls();
    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend)); /*because DoWork for event*/

    STRICT_EXPECTED_CALL(mocks, get_time(NULL));
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc()); /*because responseHeadearsHandle: a new instance of HTTP headers*/
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, BUFFER_new());
    STRICT_EXPECTED_CALL(mocks, BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1); /*because relativePath is a STRING_HANDLE*/
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,                                    /*sasObject handle                                             */
        IGNORED_PTR_ARG,                                    /*HTTPAPIEX_HANDLE handle,                                     */
        HTTPAPI_REQUEST_GET,                                /*HTTPAPI_REQUEST_TYPE requestType,                            */
        "/devices/" TEST_DEVICE_ID MESSAGE_ENDPOINT_HTTP API_VERSION,    /*const char* relativePath,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,                */
        NULL,                                               /*BUFFER_HANDLE requestContent,                                */
        IGNORED_PTR_ARG,                                    /*unsigned int* statusCode,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,               */
        IGNORED_PTR_ARG                                     /*BUFFER_HANDLE responseContent))                              */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(5)
        .IgnoreArgument(7)
        .IgnoreArgument(8)
        .IgnoreArgument(9)
        .CopyOutArgumentBuffer(7, &statusCode200, sizeof(statusCode200));

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_FindHeaderValue(IGNORED_PTR_ARG, "ETag"))
        .IgnoreArgument(1)
        .SetReturn("\"");

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_088: [ If no such header is found or is invalid, then _DoWork shall advance to the next action. ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_happy_path_with_empty_waitingToSend_and_1_service_message_fails_when_ETag_many_last_not_quote_fails)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    unsigned int statusCode200 = 200;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    (void)IoTHubTransportHttp_Subscribe(devHandle);
    mocks.ResetAllCalls();
    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend)); /*because DoWork for event*/

    STRICT_EXPECTED_CALL(mocks, get_time(NULL));
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc()); /*because responseHeadearsHandle: a new instance of HTTP headers*/
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, BUFFER_new());
    STRICT_EXPECTED_CALL(mocks, BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1); /*because relativePath is a STRING_HANDLE*/
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,                                    /*sasObject handle                                             */
        IGNORED_PTR_ARG,                                    /*HTTPAPIEX_HANDLE handle,                                     */
        HTTPAPI_REQUEST_GET,                                /*HTTPAPI_REQUEST_TYPE requestType,                            */
        "/devices/" TEST_DEVICE_ID MESSAGE_ENDPOINT_HTTP API_VERSION,    /*const char* relativePath,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,                */
        NULL,                                               /*BUFFER_HANDLE requestContent,                                */
        IGNORED_PTR_ARG,                                    /*unsigned int* statusCode,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,               */
        IGNORED_PTR_ARG                                     /*BUFFER_HANDLE responseContent))                              */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(5)
        .IgnoreArgument(7)
        .IgnoreArgument(8)
        .IgnoreArgument(9)
        .CopyOutArgumentBuffer(7, &statusCode200, sizeof(statusCode200));

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_FindHeaderValue(IGNORED_PTR_ARG, "ETag"))
        .IgnoreArgument(1)
        .SetReturn("\"abcd");

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_086: [ If the HTTPAPIEX_SAS_ExecuteRequest executed successfully then status code shall be examined. Any status code different than 200 causes _DoWork to advance to the next action. ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_happy_path_with_empty_waitingToSend_and_1_service_message_goes_top_next_action_when_httpstatus_is_500_fails)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    unsigned int statusCode500 = 500;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    (void)IoTHubTransportHttp_Subscribe(devHandle);
    mocks.ResetAllCalls();
    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend)); /*because DoWork for event*/

    STRICT_EXPECTED_CALL(mocks, get_time(NULL));
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc()); /*because responseHeadearsHandle: a new instance of HTTP headers*/
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, BUFFER_new());
    STRICT_EXPECTED_CALL(mocks, BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1); /*because relativePath is a STRING_HANDLE*/
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,                                    /*sasObject handle                                             */
        IGNORED_PTR_ARG,                                    /*HTTPAPIEX_HANDLE handle,                                     */
        HTTPAPI_REQUEST_GET,                                /*HTTPAPI_REQUEST_TYPE requestType,                            */
        "/devices/" TEST_DEVICE_ID MESSAGE_ENDPOINT_HTTP API_VERSION,    /*const char* relativePath,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,                */
        NULL,                                               /*BUFFER_HANDLE requestContent,                                */
        IGNORED_PTR_ARG,                                    /*unsigned int* statusCode,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,               */
        IGNORED_PTR_ARG                                     /*BUFFER_HANDLE responseContent))                              */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(5)
        .IgnoreArgument(7)
        .IgnoreArgument(8)
        .IgnoreArgument(9)
        .CopyOutArgumentBuffer(7, &statusCode500, sizeof(statusCode500));

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_085: [ If the call to HTTPAPIEX_SAS_ExecuteRequest did not executed successfully or building any part of the prerequisites of the call fails, then _DoWork shall advance to the next action in this description. ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_happy_path_with_empty_waitingToSend_and_1_service_message_goes_to_next_action_when_HTTPAPIEX_SAS_ExecuteRequest2_fails)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    unsigned int statusCode200 = 200;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    (void)IoTHubTransportHttp_Subscribe(devHandle);
    mocks.ResetAllCalls();
    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend)); /*because DoWork for event*/

    STRICT_EXPECTED_CALL(mocks, get_time(NULL));
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc()); /*because responseHeadearsHandle: a new instance of HTTP headers*/
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, BUFFER_new());
    STRICT_EXPECTED_CALL(mocks, BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1); /*because relativePath is a STRING_HANDLE*/
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,                                    /*sasObject handle                                             */
        IGNORED_PTR_ARG,                                    /*HTTPAPIEX_HANDLE handle,                                     */
        HTTPAPI_REQUEST_GET,                                /*HTTPAPI_REQUEST_TYPE requestType,                            */
        "/devices/" TEST_DEVICE_ID MESSAGE_ENDPOINT_HTTP API_VERSION,    /*const char* relativePath,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,                */
        NULL,                                               /*BUFFER_HANDLE requestContent,                                */
        IGNORED_PTR_ARG,                                    /*unsigned int* statusCode,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,               */
        IGNORED_PTR_ARG                                     /*BUFFER_HANDLE responseContent))                              */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(5)
        .IgnoreArgument(7)
        .IgnoreArgument(8)
        .IgnoreArgument(9)
        .CopyOutArgumentBuffer(7, &statusCode200, sizeof(statusCode200))
        .SetReturn(HTTPAPIEX_ERROR);

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_085: [ If the call to HTTPAPIEX_SAS_ExecuteRequest did not executed successfully or building any part of the prerequisites of the call fails, then _DoWork shall advance to the next action in this description. ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_happy_path_with_empty_waitingToSend_and_1_service_message_goes_to_next_action_when_BUFFER_new_fails)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    (void)IoTHubTransportHttp_Subscribe(devHandle);
    mocks.ResetAllCalls();
    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend)); /*because DoWork for event*/

    STRICT_EXPECTED_CALL(mocks, get_time(NULL));
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc()); /*because responseHeadearsHandle: a new instance of HTTP headers*/
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    whenShallBUFFER_new_fail = currentBUFFER_new_call + 1;
    STRICT_EXPECTED_CALL(mocks, BUFFER_new());

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_085: [ If the call to HTTPAPIEX_SAS_ExecuteRequest did not executed successfully or building any part of the prerequisites of the call fails, then _DoWork shall advance to the next action in this description. ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_happy_path_with_empty_waitingToSend_and_1_service_message_goes_to_next_action_when_HTTPHeaders_Alloc_fails)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    (void)IoTHubTransportHttp_Subscribe(devHandle);
    mocks.ResetAllCalls();
    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend)); /*because DoWork for event*/

    whenShallHTTPHeaders_Alloc_fail = currentHTTPHeaders_Alloc_call + 1;
    STRICT_EXPECTED_CALL(mocks, get_time(NULL));
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc()); /*because responseHeadearsHandle: a new instance of HTTP headers*/

                                                      ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_125: [ This function shall return a pointer to a structure of type IOTHUB_TRANSPORT_PROVIDER having the following values for its fields: ]
TEST_FUNCTION(HTTP_Protocol_succeeds)
{
    ///arrange

    ///act
    auto result = HTTP_Protocol();

    ///assert
    ASSERT_IS_NOT_NULL(result);
    ASSERT_ARE_EQUAL(void_ptr, (void*)((TRANSPORT_PROVIDER*)result)->IoTHubTransport_Create, (void*)IoTHubTransportHttp_Create);
    ASSERT_ARE_EQUAL(void_ptr, (void*)((TRANSPORT_PROVIDER*)result)->IoTHubTransport_Destroy, (void*)IoTHubTransportHttp_Destroy);
    ASSERT_ARE_EQUAL(void_ptr, (void*)((TRANSPORT_PROVIDER*)result)->IoTHubTransport_Register, (void*)IoTHubTransportHttp_Register);
    ASSERT_ARE_EQUAL(void_ptr, (void*)((TRANSPORT_PROVIDER*)result)->IoTHubTransport_Unregister, (void*)IoTHubTransportHttp_Unregister);
    ASSERT_ARE_EQUAL(void_ptr, (void*)((TRANSPORT_PROVIDER*)result)->IoTHubTransport_Subscribe, (void*)IoTHubTransportHttp_Subscribe);
    ASSERT_ARE_EQUAL(void_ptr, (void*)((TRANSPORT_PROVIDER*)result)->IoTHubTransport_Unsubscribe, (void*)IoTHubTransportHttp_Unsubscribe);
    ASSERT_ARE_EQUAL(void_ptr, (void*)((TRANSPORT_PROVIDER*)result)->IoTHubTransport_DoWork, (void*)IoTHubTransportHttp_DoWork);
    ASSERT_ARE_EQUAL(void_ptr, (void*)((TRANSPORT_PROVIDER*)result)->IoTHubTransport_GetSendStatus, (void*)IoTHubTransportHttp_GetSendStatus);
    ASSERT_ARE_EQUAL(void_ptr, (void*)((TRANSPORT_PROVIDER*)result)->IoTHubTransport_SetOption, (void*)IoTHubTransportHttp_SetOption);

    ///cleanup
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_056: [ IoTHubTransportHttp_DoWork shall build the following string:[{"body":"base64 encoding of the message1 content"},{"body":"base64 encoding of the message2 content"}...] ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_068: [ Once a final payload has been obtained, IoTHubTransportHttp_DoWork shall call HTTPAPIEX_SAS_ExecuteRequest passing the following parameters: ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_070: [ If HTTPAPIEX_SAS_ExecuteRequest2 does not fail and http status code < 300 then IoTHubTransportHttp_DoWork shall call IoTHubClient_LL_SendComplete. Parameter PDLIST_ENTRY completed shall point to a list containing all the items batched, and parameter IOTHUB_BATCHSTATE result shall be set to IOTHUB_BATCHSTATE_OK. The batched items shall be removed from waitingToSend. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_064: [ If IoTHubMessage does not have properties, then "properties":{...} shall be missing from the payload. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_054: [ Request HTTP headers shall have the value of "Content-Type" created or updated to "application/vnd.microsoft.iothub.json" by a call to HTTPHeaders_ReplaceHeaderNameValuePair. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_053: [ If option SetBatching is true then _DoWork shall send batched event message as specced below. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_117: [ If optionName is an option handled by IoTHubTransportHttp then it shall be set. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_120: [ "Batching" ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_with_1_event_item_happy_path_succeeds)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    DList_InsertTailList(&(waitingToSend), &(message1.entry));
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    (void)IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    mocks.ResetAllCalls();
    setupDoWorkLoopOnceForOneDevice(mocks);


    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend));

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Content-Type", "application/vnd.microsoft.iothub.json"))
        .IgnoreArgument(1);

    EXPECTED_CALL(mocks, STRING_new_with_memory(IGNORED_PTR_ARG));

    /*starting to prepare the "big" payload*/
    STRICT_EXPECTED_CALL(mocks, STRING_construct("["));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    /*this is first batched payload*/
    {
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetContentType(message1.messageHandle));
        STRICT_EXPECTED_CALL(mocks, STRING_construct("{\"body\":\""));
        STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetByteArray(message1.messageHandle, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(2)
            .IgnoreArgument(3);

        STRICT_EXPECTED_CALL(mocks, Base64_Encode_Bytes(buffer1, buffer1_size));
        STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2);

        STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, "\"")) /*closing the value of the body*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Properties(message1.messageHandle));
        STRICT_EXPECTED_CALL(mocks, Map_GetInternals(TEST_MAP_EMPTY, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(2)
            .IgnoreArgument(3)
            .IgnoreArgument(4);
        STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, "},")) /* this extra "," is going to be harshly overwritten by a "]"*/
            .IgnoreArgument(1);
        /*end of the first batched payload*/
    }

    {
        /*adding the first payload to the "big" payload*/
        STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2);

        /*closing the "big" payload*/
        STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_length(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
    }

    /*building the list of messages to be notified if HTTP is fine*/
    STRICT_EXPECTED_CALL(mocks, DList_RemoveHeadList(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, DList_InsertTailList(IGNORED_PTR_ARG, &(message1.entry)))
        .IgnoreArgument(1);

    {
        /*this is building the HTTP payload... from a STRING_HANDLE (as it comes as "big payload"), into an array of bytes*/
        STRICT_EXPECTED_CALL(mocks, BUFFER_new());
        STRICT_EXPECTED_CALL(mocks, BUFFER_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_length(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, BUFFER_build(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_NUM_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2)
            .IgnoreArgument(3);
    }

    /*executing HTTP goodies*/
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG)) /*because relativePath*/
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,                                    /*sasObject handle                                             */
        IGNORED_PTR_ARG,
        HTTPAPI_REQUEST_POST,                                                           /*HTTPAPI_REQUEST_TYPE requestType,                  */
        "/devices/" TEST_DEVICE_ID EVENT_ENDPOINT API_VERSION,                 /*const char* relativePath,                          */
        IGNORED_PTR_ARG,                                                                /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,      */
        IGNORED_PTR_ARG,                                                                /*BUFFER_HANDLE requestContent,                      */
        IGNORED_PTR_ARG,                                                                /*unsigned int* statusCode,                          */
        NULL,                                                                           /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,     */
        NULL                                                                            /*BUFFER_HANDLE responseContent)                     */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(5)
        .IgnoreArgument(6)
        .CopyOutArgumentBuffer(7, &httpStatus200, sizeof(httpStatus200));

    /*once the event has been succesfull...*/

    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_SendComplete(TEST_IOTHUB_CLIENT_LL_HANDLE, IGNORED_PTR_ARG, IOTHUB_CLIENT_CONFIRMATION_OK))
        .IgnoreArgument(2);

    ENABLE_BATCHING();

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_081: [ If HTTPAPIEX_SAS_ExecuteRequest2 fails or the http status code >=300 then IoTHubTransportHttp_DoWork shall not do any other action (it is assumed at the next _DoWork it shall be retried). ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_with_1_event_items_puts_it_back_when_http_status_is_404)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    DList_InsertTailList(&(waitingToSend), &(message1.entry));
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    (void)IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    mocks.ResetAllCalls();

    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend));

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Content-Type", "application/vnd.microsoft.iothub.json"))
        .IgnoreArgument(1);

    /*starting to prepare the "big" payload*/
    STRICT_EXPECTED_CALL(mocks, STRING_construct("["));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    EXPECTED_CALL(mocks, STRING_new_with_memory(IGNORED_PTR_ARG));

    /*this is first batched payload*/
    {
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetContentType(message1.messageHandle));
        STRICT_EXPECTED_CALL(mocks, STRING_construct("{\"body\":\""));
        STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetByteArray(message1.messageHandle, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(2)
            .IgnoreArgument(3);

        STRICT_EXPECTED_CALL(mocks, Base64_Encode_Bytes(buffer1, buffer1_size));
        STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2);

        STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, "\"")) /*closing the value of the body*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Properties(message1.messageHandle));
        STRICT_EXPECTED_CALL(mocks, Map_GetInternals(TEST_MAP_EMPTY, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(2)
            .IgnoreArgument(3)
            .IgnoreArgument(4);
        STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, "},")) /* this extra "," is going to be harshly overwritten by a "]"*/
            .IgnoreArgument(1);
        /*end of the first batched payload*/
    }

    {
        /*adding the first payload to the "big" payload*/
        STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2);

        /*closing the "big" payload*/
        STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_length(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
    }

    /*building the list of messages to be notified if HTTP is fine*/
    STRICT_EXPECTED_CALL(mocks, DList_RemoveHeadList(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, DList_InsertTailList(IGNORED_PTR_ARG, &(message1.entry)))
        .IgnoreArgument(1);

    {
        /*this is building the HTTP payload... from a STRING_HANDLE (as it comes as "big payload"), into an array of bytes*/
        STRICT_EXPECTED_CALL(mocks, BUFFER_new());
        STRICT_EXPECTED_CALL(mocks, BUFFER_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_length(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, BUFFER_build(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_NUM_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2)
            .IgnoreArgument(3);
    }

    /*executing HTTP goodies*/
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG)) /*because relativePath*/
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,                                    /*sasObject handle                                             */
        IGNORED_PTR_ARG,
        HTTPAPI_REQUEST_POST,                                                           /*HTTPAPI_REQUEST_TYPE requestType,                  */
        "/devices/" TEST_DEVICE_ID EVENT_ENDPOINT API_VERSION,                 /*const char* relativePath,                          */
        IGNORED_PTR_ARG,                                                                /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,      */
        IGNORED_PTR_ARG,                                                                /*BUFFER_HANDLE requestContent,                      */
        IGNORED_PTR_ARG,                                                                /*unsigned int* statusCode,                          */
        NULL,                                                                           /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,     */
        NULL                                                                            /*BUFFER_HANDLE responseContent)                     */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(5)
        .IgnoreArgument(6)
        .CopyOutArgumentBuffer(7, &httpStatus404, sizeof(httpStatus404));

    STRICT_EXPECTED_CALL(mocks, DList_AppendTailList(IGNORED_PTR_ARG, IGNORED_PTR_ARG)).IgnoreAllArguments();
    STRICT_EXPECTED_CALL(mocks, DList_RemoveEntryList(IGNORED_PTR_ARG)).IgnoreAllArguments();
    STRICT_EXPECTED_CALL(mocks, DList_InitializeListHead(IGNORED_PTR_ARG)).IgnoreAllArguments();

    ENABLE_BATCHING();

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_081: [ If HTTPAPIEX_SAS_ExecuteRequest2 fails or the http status code >=300 then IoTHubTransportHttp_DoWork shall not do any other action (it is assumed at the next _DoWork it shall be retried). ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_with_1_event_items_puts_it_back_when_HTTPAPIEX_SAS_ExecuteRequest2_fails)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    DList_InsertTailList(&(waitingToSend), &(message1.entry));
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    (void)IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    mocks.ResetAllCalls();

    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend));

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Content-Type", "application/vnd.microsoft.iothub.json"))
        .IgnoreArgument(1);

    EXPECTED_CALL(mocks, STRING_new_with_memory(IGNORED_PTR_ARG));

    /*starting to prepare the "big" payload*/
    STRICT_EXPECTED_CALL(mocks, STRING_construct("["));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    /*this is first batched payload*/
    {
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetContentType(message1.messageHandle));
        STRICT_EXPECTED_CALL(mocks, STRING_construct("{\"body\":\""));
        STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetByteArray(message1.messageHandle, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(2)
            .IgnoreArgument(3);

        STRICT_EXPECTED_CALL(mocks, Base64_Encode_Bytes(buffer1, buffer1_size));
        STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2);

        STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, "\"")) /*closing the value of the body*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Properties(message1.messageHandle));
        STRICT_EXPECTED_CALL(mocks, Map_GetInternals(TEST_MAP_EMPTY, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(2)
            .IgnoreArgument(3)
            .IgnoreArgument(4);
        STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, "},")) /* this extra "," is going to be harshly overwritten by a "]"*/
            .IgnoreArgument(1);
        /*end of the first batched payload*/
    }

    {
        /*adding the first payload to the "big" payload*/
        STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2);

        /*closing the "big" payload*/
        STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_length(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
    }

    /*building the list of messages to be notified if HTTP is fine*/
    STRICT_EXPECTED_CALL(mocks, DList_RemoveHeadList(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, DList_InsertTailList(IGNORED_PTR_ARG, &(message1.entry)))
        .IgnoreArgument(1);

    {
        /*this is building the HTTP payload... from a STRING_HANDLE (as it comes as "big payload"), into an array of bytes*/
        STRICT_EXPECTED_CALL(mocks, BUFFER_new());
        STRICT_EXPECTED_CALL(mocks, BUFFER_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_length(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, BUFFER_build(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_NUM_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2)
            .IgnoreArgument(3);
    }

    /*executing HTTP goodies*/
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG)) /*because relativePath*/
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,                                    /*sasObject handle                                             */
        IGNORED_PTR_ARG,
        HTTPAPI_REQUEST_POST,                                                           /*HTTPAPI_REQUEST_TYPE requestType,                  */
        "/devices/" TEST_DEVICE_ID EVENT_ENDPOINT API_VERSION,                 /*const char* relativePath,                          */
        IGNORED_PTR_ARG,                                                                /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,      */
        IGNORED_PTR_ARG,                                                                /*BUFFER_HANDLE requestContent,                      */
        IGNORED_PTR_ARG,                                                                /*unsigned int* statusCode,                          */
        NULL,                                                                           /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,     */
        NULL                                                                            /*BUFFER_HANDLE responseContent)                     */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(5)
        .IgnoreArgument(6)
        .CopyOutArgumentBuffer(7, &httpStatus200, sizeof(httpStatus200))
        .SetReturn(HTTPAPIEX_ERROR);

    STRICT_EXPECTED_CALL(mocks, DList_AppendTailList(IGNORED_PTR_ARG, IGNORED_PTR_ARG)).IgnoreAllArguments();
    STRICT_EXPECTED_CALL(mocks, DList_RemoveEntryList(IGNORED_PTR_ARG)).IgnoreAllArguments();
    STRICT_EXPECTED_CALL(mocks, DList_InitializeListHead(IGNORED_PTR_ARG)).IgnoreAllArguments();

    ENABLE_BATCHING();

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_067: [ If there is no valid payload, IoTHubTransportHttp_DoWork shall advance to the next activity. ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_with_1_event_items_puts_it_back_when_BUFFER_build_fails)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    DList_InsertTailList(&(waitingToSend), &(message1.entry));
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    (void)IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    mocks.ResetAllCalls();

    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend));

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Content-Type", "application/vnd.microsoft.iothub.json"))
        .IgnoreArgument(1);

    EXPECTED_CALL(mocks, STRING_new_with_memory(IGNORED_PTR_ARG));

    /*starting to prepare the "big" payload*/
    STRICT_EXPECTED_CALL(mocks, STRING_construct("["));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    /*this is first batched payload*/
    {
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetContentType(message1.messageHandle));
        STRICT_EXPECTED_CALL(mocks, STRING_construct("{\"body\":\""));
        STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetByteArray(message1.messageHandle, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(2)
            .IgnoreArgument(3);

        STRICT_EXPECTED_CALL(mocks, Base64_Encode_Bytes(buffer1, buffer1_size));
        STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2);

        STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, "\"")) /*closing the value of the body*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Properties(message1.messageHandle));
        STRICT_EXPECTED_CALL(mocks, Map_GetInternals(TEST_MAP_EMPTY, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(2)
            .IgnoreArgument(3)
            .IgnoreArgument(4);
        STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, "},")) /* this extra "," is going to be harshly overwritten by a "]"*/
            .IgnoreArgument(1);
        /*end of the first batched payload*/
    }

    {
        /*adding the first payload to the "big" payload*/
        STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2);

        /*closing the "big" payload*/
        STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_length(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
    }

    /*building the list of messages to be notified if HTTP is fine*/
    STRICT_EXPECTED_CALL(mocks, DList_RemoveHeadList(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, DList_InsertTailList(IGNORED_PTR_ARG, &(message1.entry)))
        .IgnoreArgument(1);

    {
        /*this is building the HTTP payload... from a STRING_HANDLE (as it comes as "big payload"), into an array of bytes*/
        STRICT_EXPECTED_CALL(mocks, BUFFER_new());
        STRICT_EXPECTED_CALL(mocks, BUFFER_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_length(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        whenShallBUFFER_build_fail = 1;
        STRICT_EXPECTED_CALL(mocks, BUFFER_build(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_NUM_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2)
            .IgnoreArgument(3)
            .SetReturn(__LINE__);
    }

    STRICT_EXPECTED_CALL(mocks, DList_AppendTailList(IGNORED_PTR_ARG, IGNORED_PTR_ARG)).IgnoreAllArguments();
    STRICT_EXPECTED_CALL(mocks, DList_RemoveEntryList(IGNORED_PTR_ARG)).IgnoreAllArguments();
    STRICT_EXPECTED_CALL(mocks, DList_InitializeListHead(IGNORED_PTR_ARG)).IgnoreAllArguments();

    ENABLE_BATCHING();

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_067: [ If there is no valid payload, IoTHubTransportHttp_DoWork shall advance to the next activity. ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_with_1_event_items_puts_it_back_when_BUFFER_new_fails)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    DList_InsertTailList(&(waitingToSend), &(message1.entry));
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    (void)IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    mocks.ResetAllCalls();

    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend));

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Content-Type", "application/vnd.microsoft.iothub.json"))
        .IgnoreArgument(1);

    EXPECTED_CALL(mocks, STRING_new_with_memory(IGNORED_PTR_ARG));

    /*starting to prepare the "big" payload*/
    STRICT_EXPECTED_CALL(mocks, STRING_construct("["));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    /*this is first batched payload*/
    {
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetContentType(message1.messageHandle));
        STRICT_EXPECTED_CALL(mocks, STRING_construct("{\"body\":\""));
        STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetByteArray(message1.messageHandle, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(2)
            .IgnoreArgument(3);

        STRICT_EXPECTED_CALL(mocks, Base64_Encode_Bytes(buffer1, buffer1_size));
        STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2);

        STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, "\"")) /*closing the value of the body*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Properties(message1.messageHandle));
        STRICT_EXPECTED_CALL(mocks, Map_GetInternals(TEST_MAP_EMPTY, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(2)
            .IgnoreArgument(3)
            .IgnoreArgument(4);
        STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, "},")) /* this extra "," is going to be harshly overwritten by a "]"*/
            .IgnoreArgument(1);
        /*end of the first batched payload*/
    }

    {
        /*adding the first payload to the "big" payload*/
        STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2);

        /*closing the "big" payload*/
        STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_length(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
    }

    /*building the list of messages to be notified if HTTP is fine*/
    STRICT_EXPECTED_CALL(mocks, DList_RemoveHeadList(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, DList_InsertTailList(IGNORED_PTR_ARG, &(message1.entry)))
        .IgnoreArgument(1);

    {
        /*this is building the HTTP payload... from a STRING_HANDLE (as it comes as "big payload"), into an array of bytes*/
        whenShallBUFFER_new_fail = 1;
        STRICT_EXPECTED_CALL(mocks, BUFFER_new());
    }

    STRICT_EXPECTED_CALL(mocks, DList_AppendTailList(IGNORED_PTR_ARG, IGNORED_PTR_ARG)).IgnoreAllArguments();
    STRICT_EXPECTED_CALL(mocks, DList_RemoveEntryList(IGNORED_PTR_ARG)).IgnoreAllArguments();
    STRICT_EXPECTED_CALL(mocks, DList_InitializeListHead(IGNORED_PTR_ARG)).IgnoreAllArguments();

    ENABLE_BATCHING();

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_067: [ If there is no valid payload, IoTHubTransportHttp_DoWork shall advance to the next activity. ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_with_1_event_item_when_STRING_concat_with_STRING_fails_it_fails)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    DList_InsertTailList(&(waitingToSend), &(message1.entry));
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    (void)IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    mocks.ResetAllCalls();

    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend));

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Content-Type", "application/vnd.microsoft.iothub.json"))
        .IgnoreArgument(1);

    EXPECTED_CALL(mocks, STRING_new_with_memory(IGNORED_PTR_ARG));

    /*starting to prepare the "big" payload*/
    STRICT_EXPECTED_CALL(mocks, STRING_construct("["));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    /*this is first batched payload*/
    {
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetContentType(message1.messageHandle));
        STRICT_EXPECTED_CALL(mocks, STRING_construct("{\"body\":\""));
        STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetByteArray(message1.messageHandle, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(2)
            .IgnoreArgument(3);

        STRICT_EXPECTED_CALL(mocks, Base64_Encode_Bytes(buffer1, buffer1_size));
        STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2);

        STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, "\"")) /*closing the value of the body*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Properties(message1.messageHandle));
        STRICT_EXPECTED_CALL(mocks, Map_GetInternals(TEST_MAP_EMPTY, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(2)
            .IgnoreArgument(3)
            .IgnoreArgument(4);
        STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, "},")) /* this extra "," is going to be harshly overwritten by a "]"*/
            .IgnoreArgument(1);
        /*end of the first batched payload*/
    }

    {
        /*adding the first payload to the "big" payload*/
        whenShallSTRING_concat_with_STRING_fail = 7;
        STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2);
    }

    ENABLE_BATCHING();

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_067: [ If there is no valid payload, IoTHubTransportHttp_DoWork shall advance to the next activity. ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_with_1_event_item_when_STRING_concat_it_fails)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    DList_InsertTailList(&(waitingToSend), &(message1.entry));
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    (void)IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    mocks.ResetAllCalls();

    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend));

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Content-Type", "application/vnd.microsoft.iothub.json"))
        .IgnoreArgument(1);

    EXPECTED_CALL(mocks, STRING_new_with_memory(IGNORED_PTR_ARG));

    /*starting to prepare the "big" payload*/
    STRICT_EXPECTED_CALL(mocks, STRING_construct("["));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    /*this is first batched payload*/
    {
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetContentType(message1.messageHandle));
        STRICT_EXPECTED_CALL(mocks, STRING_construct("{\"body\":\""));
        STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetByteArray(message1.messageHandle, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(2)
            .IgnoreArgument(3);

        STRICT_EXPECTED_CALL(mocks, Base64_Encode_Bytes(buffer1, buffer1_size));
        STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2);


        STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, "\"")) /*closing the value of the body*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Properties(message1.messageHandle));
        STRICT_EXPECTED_CALL(mocks, Map_GetInternals(TEST_MAP_EMPTY, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(2)
            .IgnoreArgument(3)
            .IgnoreArgument(4);
        whenShallSTRING_concat_fail = currentSTRING_concat_call + 2;
        STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, "},")) /* this extra "," is going to be harshly overwritten by a "]"*/
            .IgnoreArgument(1);
        /*end of the first batched payload*/
    }

    ENABLE_BATCHING();

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_067: [ If there is no valid payload, IoTHubTransportHttp_DoWork shall advance to the next activity. ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_with_1_event_item_when_STRING_concat_it_fails_2)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    DList_InsertTailList(&(waitingToSend), &(message1.entry));
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    (void)IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    mocks.ResetAllCalls();

    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend));

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Content-Type", "application/vnd.microsoft.iothub.json"))
        .IgnoreArgument(1);

    EXPECTED_CALL(mocks, STRING_new_with_memory(IGNORED_PTR_ARG));

    /*starting to prepare the "big" payload*/
    STRICT_EXPECTED_CALL(mocks, STRING_construct("["));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    /*this is first batched payload*/
    {
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetContentType(message1.messageHandle));
        STRICT_EXPECTED_CALL(mocks, STRING_construct("{\"body\":\""));
        STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetByteArray(message1.messageHandle, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(2)
            .IgnoreArgument(3);

        STRICT_EXPECTED_CALL(mocks, Base64_Encode_Bytes(buffer1, buffer1_size));
        STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2);


        whenShallSTRING_concat_fail = currentSTRING_concat_call + 1;
        STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, "\"")) /*closing the value of the body*/
            .IgnoreArgument(1);
        /*end of the first batched payload*/
    }

    ENABLE_BATCHING();

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_067: [ If there is no valid payload, IoTHubTransportHttp_DoWork shall advance to the next activity. ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_with_1_event_item_when_STRING_concat_with_STRING_it_fails_2)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    DList_InsertTailList(&(waitingToSend), &(message1.entry));
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    (void)IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    mocks.ResetAllCalls();

    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend));

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Content-Type", "application/vnd.microsoft.iothub.json"))
        .IgnoreArgument(1);

    EXPECTED_CALL(mocks, STRING_new_with_memory(IGNORED_PTR_ARG));

    /*starting to prepare the "big" payload*/
    STRICT_EXPECTED_CALL(mocks, STRING_construct("["));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    /*this is first batched payload*/
    {
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetContentType(message1.messageHandle));
        STRICT_EXPECTED_CALL(mocks, STRING_construct("{\"body\":\""));
        STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetByteArray(message1.messageHandle, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(2)
            .IgnoreArgument(3);

        STRICT_EXPECTED_CALL(mocks, Base64_Encode_Bytes(buffer1, buffer1_size));
        STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        whenShallSTRING_concat_with_STRING_fail = 6;
        STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2);
        /*end of the first batched payload*/
    }

    ENABLE_BATCHING();

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_067: [ If there is no valid payload, IoTHubTransportHttp_DoWork shall advance to the next activity. ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_with_1_event_item_when_Base64_Encode_Bytes_it_fails)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    DList_InsertTailList(&(waitingToSend), &(message1.entry));
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    (void)IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    mocks.ResetAllCalls();

    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend));

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Content-Type", "application/vnd.microsoft.iothub.json"))
        .IgnoreArgument(1);

    /*starting to prepare the "big" payload*/
    STRICT_EXPECTED_CALL(mocks, STRING_construct("["));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    /*this is first batched payload*/
    {
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetContentType(message1.messageHandle));
        STRICT_EXPECTED_CALL(mocks, STRING_construct("{\"body\":\""));
        STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetByteArray(message1.messageHandle, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(2)
            .IgnoreArgument(3);

        whenShallBase64_Encode_Bytes_fail = 1;
        STRICT_EXPECTED_CALL(mocks, Base64_Encode_Bytes(buffer1, buffer1_size));
        /*end of the first batched payload*/
    }

    ENABLE_BATCHING();

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_067: [ If there is no valid payload, IoTHubTransportHttp_DoWork shall advance to the next activity. ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_with_1_event_item_when_IoTHubMessage_GetByteArray_it_fails)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    DList_InsertTailList(&(waitingToSend), &(message1.entry));
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    (void)IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    mocks.ResetAllCalls();

    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend));

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Content-Type", "application/vnd.microsoft.iothub.json"))
        .IgnoreArgument(1);

    /*starting to prepare the "big" payload*/
    STRICT_EXPECTED_CALL(mocks, STRING_construct("["));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    /*this is first batched payload*/
    {
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetContentType(message1.messageHandle));
        STRICT_EXPECTED_CALL(mocks, STRING_construct("{\"body\":\""));
        STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetByteArray(message1.messageHandle, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(2)
            .IgnoreArgument(3)
            .SetReturn(IOTHUB_MESSAGE_ERROR);

        /*end of the first batched payload*/
    }

    ENABLE_BATCHING();

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_067: [ If there is no valid payload, IoTHubTransportHttp_DoWork shall advance to the next activity. ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_with_1_event_item_when_STRING_construct_fails_it_fails)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    DList_InsertTailList(&(waitingToSend), &(message1.entry));
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    (void)IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    mocks.ResetAllCalls();

    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend));

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Content-Type", "application/vnd.microsoft.iothub.json"))
        .IgnoreArgument(1);

    /*starting to prepare the "big" payload*/
    STRICT_EXPECTED_CALL(mocks, STRING_construct("["));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    /*this is first batched payload*/
    {
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetContentType(message1.messageHandle));
        whenShallSTRING_construct_fail = currentSTRING_construct_call + 2;
        STRICT_EXPECTED_CALL(mocks, STRING_construct("{\"body\":\""));
        /*end of the first batched payload*/
    }

    ENABLE_BATCHING();

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_067: [ If there is no valid payload, IoTHubTransportHttp_DoWork shall advance to the next activity. ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_with_1_event_item_when_STRING_construct_it_fails)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    DList_InsertTailList(&(waitingToSend), &(message1.entry));
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    (void)IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    mocks.ResetAllCalls();

    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend));

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Content-Type", "application/vnd.microsoft.iothub.json"))
        .IgnoreArgument(1);

    /*starting to prepare the "big" payload*/
    whenShallSTRING_construct_fail = currentSTRING_construct_call + 1;
    STRICT_EXPECTED_CALL(mocks, STRING_construct("["));

    ENABLE_BATCHING();

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_055: [ If updating Content-Type fails for any reason, then _DoWork shall advance to the next action. ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_with_1_event_item_when_HTTP_headers_fails_it_fails)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    DList_InsertTailList(&(waitingToSend), &(message1.entry));
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    (void)IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    mocks.ResetAllCalls();

    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend));

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Content-Type", "application/vnd.microsoft.iothub.json"))
        .IgnoreArgument(1)
        .SetReturn(HTTP_HEADERS_ERROR);

    ENABLE_BATCHING();

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_065: [ If the oldest message in waitingToSend causes the message size to exceed the message size limit then it shall be removed from waitingToSend, and IoTHubClient_LL_SendComplete shall be called. Parameter PDLIST_ENTRY completed shall point to a list containing only the oldest item, and parameter IOTHUB_BATCHSTATE result shall be set to IOTHUB_CLIENT_CONFIRMATION_ERROR. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_061: [ The message size shall be limited to 255KB - 1 byte. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_062: [ The message size is computed from the length of the payload + 384. ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_with_1_event_item_bigger_than_256K_path_succeeds)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    DList_InsertTailList(&(waitingToSend), &(message4.entry));
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    (void)IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    mocks.ResetAllCalls();

    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend));

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Content-Type", "application/vnd.microsoft.iothub.json"))
        .IgnoreArgument(1);

    /*starting to prepare the "big" payload*/
    STRICT_EXPECTED_CALL(mocks, STRING_construct("["));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    EXPECTED_CALL(mocks, STRING_new_with_memory(IGNORED_PTR_ARG));

    /*this is first batched payload*/
    {
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetContentType(message4.messageHandle));
        STRICT_EXPECTED_CALL(mocks, STRING_construct("{\"body\":\""));
        STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetByteArray(message4.messageHandle, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(2)
            .IgnoreArgument(3);

        STRICT_EXPECTED_CALL(mocks, Base64_Encode_Bytes(bigBufferOverflow, TEST_BIG_BUFFER_1_OVERFLOW_SIZE));
        STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2);

        STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, "\"")) /*closing the value of the body*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Properties(message4.messageHandle));
        STRICT_EXPECTED_CALL(mocks, Map_GetInternals(TEST_MAP_EMPTY, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(2)
            .IgnoreArgument(3)
            .IgnoreArgument(4);
        STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, "},")) /* this extra "," is going to be harshly overwritten by a "]"*/
            .IgnoreArgument(1);
        /*end of the first batched payload*/
    }

    {
        /*adding the first payload to the "big" payload*/
        /*building the list of messages to be notified because this is 100% fail (>256K)*/
        STRICT_EXPECTED_CALL(mocks, DList_RemoveHeadList(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, DList_InsertTailList(IGNORED_PTR_ARG, &(message4.entry)))
            .IgnoreArgument(1);
    }

    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_SendComplete(TEST_IOTHUB_CLIENT_LL_HANDLE, IGNORED_PTR_ARG, IOTHUB_CLIENT_CONFIRMATION_ERROR))
        .IgnoreArgument(2);

    ENABLE_BATCHING();

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

/*this is a test that wants to see that "almost" 255KB message still fits*/
//Tests_SRS_TRANSPORTMULTITHTTP_17_062: [ The message size is computed from the length of the payload + 384. ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_with_1_event_item_almost255_happy_path_succeeds)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    DList_InsertTailList(&(waitingToSend), &(message5.entry));
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    (void)IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    mocks.ResetAllCalls();

    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend));

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Content-Type", "application/vnd.microsoft.iothub.json"))
        .IgnoreArgument(1);

    /*starting to prepare the "big" payload*/
    STRICT_EXPECTED_CALL(mocks, STRING_construct("["));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    /*this is first batched payload*/
    {
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetContentType(message5.messageHandle));
        STRICT_EXPECTED_CALL(mocks, STRING_construct("{\"body\":\""));
        STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetByteArray(message5.messageHandle, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(2)
            .IgnoreArgument(3);

        STRICT_EXPECTED_CALL(mocks, Base64_Encode_Bytes(bigBufferFit, TEST_BIG_BUFFER_1_FIT_SIZE));
        STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2);

        EXPECTED_CALL(mocks, STRING_new_with_memory(IGNORED_PTR_ARG));

        STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, "\"")) /*closing the value of the body*/
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Properties(message5.messageHandle));
        STRICT_EXPECTED_CALL(mocks, Map_GetInternals(TEST_MAP_EMPTY, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(2)
            .IgnoreArgument(3)
            .IgnoreArgument(4);
        STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, "},")) /* this extra "," is going to be harshly overwritten by a "]"*/
            .IgnoreArgument(1);
        /*end of the first batched payload*/
    }

    {
        /*adding the first payload to the "big" payload*/
        STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2);

        /*closing the "big" payload*/
        STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_length(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
    }

    /*building the list of messages to be notified if HTTP is fine*/
    STRICT_EXPECTED_CALL(mocks, DList_RemoveHeadList(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, DList_InsertTailList(IGNORED_PTR_ARG, &(message5.entry)))
        .IgnoreArgument(1);

    {
        /*this is building the HTTP payload... from a STRING_HANDLE (as it comes as "big payload"), into an array of bytes*/
        STRICT_EXPECTED_CALL(mocks, BUFFER_new());
        STRICT_EXPECTED_CALL(mocks, BUFFER_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_length(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, BUFFER_build(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_NUM_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2)
            .IgnoreArgument(3);
    }

    /*executing HTTP goodies*/
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG)) /*because relativePath*/
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,                                    /*sasObject handle                                             */
        IGNORED_PTR_ARG,
        HTTPAPI_REQUEST_POST,                                                           /*HTTPAPI_REQUEST_TYPE requestType,                  */
        "/devices/" TEST_DEVICE_ID EVENT_ENDPOINT API_VERSION,                 /*const char* relativePath,                          */
        IGNORED_PTR_ARG,                                                                /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,      */
        IGNORED_PTR_ARG,                                                                /*BUFFER_HANDLE requestContent,                      */
        IGNORED_PTR_ARG,                                                                /*unsigned int* statusCode,                          */
        NULL,                                                                           /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,     */
        NULL                                                                            /*BUFFER_HANDLE responseContent)                     */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(5)
        .IgnoreArgument(6)
        .CopyOutArgumentBuffer(7, &httpStatus200, sizeof(httpStatus200));

    /*once the event has been succesfull...*/

    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_SendComplete(TEST_IOTHUB_CLIENT_LL_HANDLE, IGNORED_PTR_ARG, IOTHUB_CLIENT_CONFIRMATION_OK))
        .IgnoreArgument(2);

    ENABLE_BATCHING();

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_056: [ IoTHubTransportHttp_DoWork shall build the following string:[{"body":"base64 encoding of the message1 content"},{"body":"base64 encoding of the message2 content"}...] ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_with_2_event_items_makes_1_batch_succeeds)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    DList_InsertTailList(&(waitingToSend), &(message1.entry));
    DList_InsertTailList(&(waitingToSend), &(message2.entry));
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    (void)IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    mocks.ResetAllCalls();

    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend));

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Content-Type", "application/vnd.microsoft.iothub.json"))
        .IgnoreArgument(1);

    EXPECTED_CALL(mocks, STRING_new_with_memory(IGNORED_PTR_ARG))
        .ExpectedAtLeastTimes(2);

    /*starting to prepare the "big" payload*/
    STRICT_EXPECTED_CALL(mocks, STRING_construct("["));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    /*this is first batched payload*/
    {
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetContentType(message1.messageHandle));
        STRICT_EXPECTED_CALL(mocks, STRING_construct("{\"body\":\""));
        STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetByteArray(message1.messageHandle, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(2)
            .IgnoreArgument(3);

        STRICT_EXPECTED_CALL(mocks, Base64_Encode_Bytes(buffer1, buffer1_size));
        STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2);

        STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, "\"")) /*closing the value of the body*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Properties(message1.messageHandle));
        STRICT_EXPECTED_CALL(mocks, Map_GetInternals(TEST_MAP_EMPTY, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(2)
            .IgnoreArgument(3)
            .IgnoreArgument(4);
        STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, "},")) /* this extra "," is going to be harshly overwritten by a "]"*/
            .IgnoreArgument(1);
        /*end of the first batched payload*/
    }

    {
        /*adding the first payload to the "big" payload*/
        STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2);
    }

    /*this is second batched payload*/
    {
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetContentType(message2.messageHandle));
        STRICT_EXPECTED_CALL(mocks, STRING_construct("{\"body\":\""));
        STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetByteArray(message2.messageHandle, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(2)
            .IgnoreArgument(3);

        STRICT_EXPECTED_CALL(mocks, Base64_Encode_Bytes(buffer2, buffer2_size));
        STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2);

        STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, "\"")) /*closing the value of the body*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Properties(message2.messageHandle));
        STRICT_EXPECTED_CALL(mocks, Map_GetInternals(TEST_MAP_EMPTY, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(2)
            .IgnoreArgument(3)
            .IgnoreArgument(4);
        STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, "},")) /* this extra "," is going to be harshly overwritten by a "]"*/
            .IgnoreArgument(1);
        /*end of the first batched payload*/
    }

    {
        /*adding the second payload to the "big" payload*/
        STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2);
    }

    {
        /*closing the "big" payload*/
        STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_length(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
    }

    /*building the list of messages to be notified if HTTP is fine*/
    STRICT_EXPECTED_CALL(mocks, DList_RemoveHeadList(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, DList_InsertTailList(IGNORED_PTR_ARG, &(message1.entry)))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, DList_RemoveHeadList(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, DList_InsertTailList(IGNORED_PTR_ARG, &(message2.entry)))
        .IgnoreArgument(1);

    {
        /*this is building the HTTP payload... from a STRING_HANDLE (as it comes as "big payload"), into an array of bytes*/
        STRICT_EXPECTED_CALL(mocks, BUFFER_new());
        STRICT_EXPECTED_CALL(mocks, BUFFER_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_length(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, BUFFER_build(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_NUM_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2)
            .IgnoreArgument(3);
    }

    /*executing HTTP goodies*/
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG)) /*because relativePath*/
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,                                    /*sasObject handle                                             */
        IGNORED_PTR_ARG,
        HTTPAPI_REQUEST_POST,                                                           /*HTTPAPI_REQUEST_TYPE requestType,                  */
        "/devices/" TEST_DEVICE_ID EVENT_ENDPOINT API_VERSION,                 /*const char* relativePath,                          */
        IGNORED_PTR_ARG,                                                                /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,      */
        IGNORED_PTR_ARG,                                                                /*BUFFER_HANDLE requestContent,                      */
        IGNORED_PTR_ARG,                                                                /*unsigned int* statusCode,                          */
        NULL,                                                                           /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,     */
        NULL                                                                            /*BUFFER_HANDLE responseContent)                     */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(5)
        .IgnoreArgument(6)
        .CopyOutArgumentBuffer(7, &httpStatus200, sizeof(httpStatus200));

    /*once the event has been succesfull...*/

    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_SendComplete(TEST_IOTHUB_CLIENT_LL_HANDLE, IGNORED_PTR_ARG, IOTHUB_CLIENT_CONFIRMATION_OK))
        .IgnoreArgument(2);

    ENABLE_BATCHING();

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_066: [ If at any point during construction of the string there are errors, IoTHubTransportHttp_DoWork shall use the so far constructed string as payload. ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_with_2_event_items_when_the_second_items_fails_the_first_one_still_makes_1_batch_succeeds_1)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    DList_InsertTailList(&(waitingToSend), &(message1.entry));
    DList_InsertTailList(&(waitingToSend), &(message2.entry));
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    (void)IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    mocks.ResetAllCalls();
    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend));

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Content-Type", "application/vnd.microsoft.iothub.json"))
        .IgnoreArgument(1);

    /*starting to prepare the "big" payload*/
    STRICT_EXPECTED_CALL(mocks, STRING_construct("["));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    EXPECTED_CALL(mocks, STRING_new_with_memory(IGNORED_PTR_ARG))
        .ExpectedAtLeastTimes(2);
    /*this is first batched payload*/
    {
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetContentType(message1.messageHandle));
        STRICT_EXPECTED_CALL(mocks, STRING_construct("{\"body\":\""));
        STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetByteArray(message1.messageHandle, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(2)
            .IgnoreArgument(3);

        STRICT_EXPECTED_CALL(mocks, Base64_Encode_Bytes(buffer1, buffer1_size));
        STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2);

        STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, "\"")) /*closing the value of the body*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Properties(message1.messageHandle));
        STRICT_EXPECTED_CALL(mocks, Map_GetInternals(TEST_MAP_EMPTY, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(2)
            .IgnoreArgument(3)
            .IgnoreArgument(4);
        STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, "},")) /* this extra "," is going to be harshly overwritten by a "]"*/
            .IgnoreArgument(1);
        /*end of the first batched payload*/
    }

    {
        /*adding the first payload to the "big" payload*/
        STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2);
    }

    /*this is second batched payload*/
    {
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetContentType(message2.messageHandle));
        STRICT_EXPECTED_CALL(mocks, STRING_construct("{\"body\":\""));
        STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetByteArray(message2.messageHandle, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(2)
            .IgnoreArgument(3);

        STRICT_EXPECTED_CALL(mocks, Base64_Encode_Bytes(buffer2, buffer2_size));
        STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2);

        STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, "\"")) /*closing the value of the body*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Properties(message2.messageHandle));
        STRICT_EXPECTED_CALL(mocks, Map_GetInternals(TEST_MAP_EMPTY, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(2)
            .IgnoreArgument(3)
            .IgnoreArgument(4);
        whenShallSTRING_concat_fail = currentSTRING_concat_call + 4;
        STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, "},")) /* this extra "," is going to be harshly overwritten by a "]"*/
            .IgnoreArgument(1);
        /*end of the second batched payload*/
    }

    {
        /*closing the "big" payload*/
        STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_length(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
    }

    /*building the list of messages to be notified if HTTP is fine*/
    STRICT_EXPECTED_CALL(mocks, DList_RemoveHeadList(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, DList_InsertTailList(IGNORED_PTR_ARG, &(message1.entry)))
        .IgnoreArgument(1);

    {
        /*this is building the HTTP payload... from a STRING_HANDLE (as it comes as "big payload"), into an array of bytes*/
        STRICT_EXPECTED_CALL(mocks, BUFFER_new());
        STRICT_EXPECTED_CALL(mocks, BUFFER_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_length(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, BUFFER_build(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_NUM_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2)
            .IgnoreArgument(3);
    }

    /*executing HTTP goodies*/
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG)) /*because relativePath*/
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,                                    /*sasObject handle                                             */
        IGNORED_PTR_ARG,
        HTTPAPI_REQUEST_POST,                                                           /*HTTPAPI_REQUEST_TYPE requestType,                  */
        "/devices/" TEST_DEVICE_ID EVENT_ENDPOINT API_VERSION,                 /*const char* relativePath,                          */
        IGNORED_PTR_ARG,                                                                /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,      */
        IGNORED_PTR_ARG,                                                                /*BUFFER_HANDLE requestContent,                      */
        IGNORED_PTR_ARG,                                                                /*unsigned int* statusCode,                          */
        NULL,                                                                           /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,     */
        NULL                                                                            /*BUFFER_HANDLE responseContent)                     */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(5)
        .IgnoreArgument(6)
        .CopyOutArgumentBuffer(7, &httpStatus200, sizeof(httpStatus200));

    /*once the event has been succesfull...*/

    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_SendComplete(TEST_IOTHUB_CLIENT_LL_HANDLE, IGNORED_PTR_ARG, IOTHUB_CLIENT_CONFIRMATION_OK))
        .IgnoreArgument(2);

    ENABLE_BATCHING();

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_066: [ If at any point during construction of the string there are errors, IoTHubTransportHttp_DoWork shall use the so far constructed string as payload. ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_with_2_event_items_when_the_second_items_fails_the_first_one_still_makes_1_batch_succeeds_2)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    DList_InsertTailList(&(waitingToSend), &(message1.entry));
    DList_InsertTailList(&(waitingToSend), &(message2.entry));
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    (void)IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    mocks.ResetAllCalls();
    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend));

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Content-Type", "application/vnd.microsoft.iothub.json"))
        .IgnoreArgument(1);

    /*starting to prepare the "big" payload*/
    STRICT_EXPECTED_CALL(mocks, STRING_construct("["));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    EXPECTED_CALL(mocks, STRING_new_with_memory(IGNORED_PTR_ARG))
        .ExpectedAtLeastTimes(2);

    /*this is first batched payload*/
    {
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetContentType(message1.messageHandle));
        STRICT_EXPECTED_CALL(mocks, STRING_construct("{\"body\":\""));
        STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetByteArray(message1.messageHandle, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(2)
            .IgnoreArgument(3);

        STRICT_EXPECTED_CALL(mocks, Base64_Encode_Bytes(buffer1, buffer1_size));
        STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2);

        STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, "\"")) /*closing the value of the body*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Properties(message1.messageHandle));
        STRICT_EXPECTED_CALL(mocks, Map_GetInternals(TEST_MAP_EMPTY, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(2)
            .IgnoreArgument(3)
            .IgnoreArgument(4);
        STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, "},")) /* this extra "," is going to be harshly overwritten by a "]"*/
            .IgnoreArgument(1);;
        /*end of the first batched payload*/
    }

    {
        /*adding the first payload to the "big" payload*/
        STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2);
    }

    /*this is second batched payload*/
    {
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetContentType(message2.messageHandle));
        STRICT_EXPECTED_CALL(mocks, STRING_construct("{\"body\":\""));
        STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetByteArray(message2.messageHandle, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(2)
            .IgnoreArgument(3);

        STRICT_EXPECTED_CALL(mocks, Base64_Encode_Bytes(buffer2, buffer2_size));
        STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2);
        STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, "\"")) /*closing the value of the body*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Properties(message2.messageHandle));
        STRICT_EXPECTED_CALL(mocks, Map_GetInternals(TEST_MAP_EMPTY, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(2)
            .IgnoreArgument(3)
            .IgnoreArgument(4);
        STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, "},")) /* this extra "," is going to be harshly overwritten by a "]"*/
            .IgnoreArgument(1);
        /*end of the second batched payload*/
    }

    {
        /*adding the second payload to the "big" payload*/
        whenShallSTRING_concat_with_STRING_fail = currentSTRING_concat_with_STRING_call + 4;
        STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2);
    }

    {
        /*closing the "big" payload*/
        STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_length(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
    }

    /*building the list of messages to be notified if HTTP is fine*/
    STRICT_EXPECTED_CALL(mocks, DList_RemoveHeadList(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, DList_InsertTailList(IGNORED_PTR_ARG, &(message1.entry)))
        .IgnoreArgument(1);

    {
        /*this is building the HTTP payload... from a STRING_HANDLE (as it comes as "big payload"), into an array of bytes*/
        STRICT_EXPECTED_CALL(mocks, BUFFER_new());
        STRICT_EXPECTED_CALL(mocks, BUFFER_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_length(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, BUFFER_build(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_NUM_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2)
            .IgnoreArgument(3);
    }

    /*executing HTTP goodies*/
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG)) /*because relativePath*/
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,                                    /*sasObject handle                                             */
        IGNORED_PTR_ARG,
        HTTPAPI_REQUEST_POST,                                                           /*HTTPAPI_REQUEST_TYPE requestType,                  */
        "/devices/" TEST_DEVICE_ID EVENT_ENDPOINT API_VERSION,                 /*const char* relativePath,                          */
        IGNORED_PTR_ARG,                                                                /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,      */
        IGNORED_PTR_ARG,                                                                /*BUFFER_HANDLE requestContent,                      */
        IGNORED_PTR_ARG,                                                                /*unsigned int* statusCode,                          */
        NULL,                                                                           /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,     */
        NULL                                                                            /*BUFFER_HANDLE responseContent)                     */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(5)
        .IgnoreArgument(6)
        .CopyOutArgumentBuffer(7, &httpStatus200, sizeof(httpStatus200));

    /*once the event has been succesfull...*/

    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_SendComplete(TEST_IOTHUB_CLIENT_LL_HANDLE, IGNORED_PTR_ARG, IOTHUB_CLIENT_CONFIRMATION_OK))
        .IgnoreArgument(2);

    ENABLE_BATCHING();

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_066: [ If at any point during construction of the string there are errors, IoTHubTransportHttp_DoWork shall use the so far constructed string as payload. ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_with_2_event_items_the_second_one_does_not_fit_256K_makes_1_batch_of_the_first_item_succeeds)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    DList_InsertTailList(&(waitingToSend), &(message1.entry));
    DList_InsertTailList(&(waitingToSend), &(message5.entry));
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    (void)IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    mocks.ResetAllCalls();
    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend));

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Content-Type", "application/vnd.microsoft.iothub.json"))
        .IgnoreArgument(1);

    /*starting to prepare the "big" payload*/
    STRICT_EXPECTED_CALL(mocks, STRING_construct("["));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    EXPECTED_CALL(mocks, STRING_new_with_memory(IGNORED_PTR_ARG))
        .ExpectedAtLeastTimes(2);

    /*this is first batched payload*/
    {
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetContentType(message1.messageHandle));
        STRICT_EXPECTED_CALL(mocks, STRING_construct("{\"body\":\""));
        STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetByteArray(message1.messageHandle, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(2)
            .IgnoreArgument(3);

        STRICT_EXPECTED_CALL(mocks, Base64_Encode_Bytes(buffer1, buffer1_size));
        STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2);

        STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, "\"")) /*closing the value of the body*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Properties(message1.messageHandle));
        STRICT_EXPECTED_CALL(mocks, Map_GetInternals(TEST_MAP_EMPTY, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(2)
            .IgnoreArgument(3)
            .IgnoreArgument(4);
        STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, "},")) /* this extra "," is going to be harshly overwritten by a "]"*/
            .IgnoreArgument(1);
        /*end of the first batched payload*/
    }

    {
        /*adding the first payload to the "big" payload*/
        STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2);
    }

    /*this is second batched payload*/
    {
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetContentType(message5.messageHandle));
        STRICT_EXPECTED_CALL(mocks, STRING_construct("{\"body\":\""));
        STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetByteArray(message5.messageHandle, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(2)
            .IgnoreArgument(3);

        STRICT_EXPECTED_CALL(mocks, Base64_Encode_Bytes(bigBufferFit, TEST_BIG_BUFFER_1_FIT_SIZE));
        STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2);

        STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, "\"")) /*closing the value of the body*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Properties(message5.messageHandle));
        STRICT_EXPECTED_CALL(mocks, Map_GetInternals(TEST_MAP_EMPTY, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(2)
            .IgnoreArgument(3)
            .IgnoreArgument(4);
        STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, "},")) /* this extra "," is going to be harshly overwritten by a "]"*/
            .IgnoreArgument(1);
        /*end of the first batched payload*/
    }

    {
        /*adding the second payload to the "big" payload*/
    }

    {
        /*closing the "big" payload*/
        STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_length(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
    }

    /*building the list of messages to be notified if HTTP is fine*/
    STRICT_EXPECTED_CALL(mocks, DList_RemoveHeadList(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, DList_InsertTailList(IGNORED_PTR_ARG, &(message1.entry)))
        .IgnoreArgument(1);
    {
        /*this is building the HTTP payload... from a STRING_HANDLE (as it comes as "big payload"), into an array of bytes*/
        STRICT_EXPECTED_CALL(mocks, BUFFER_new());
        STRICT_EXPECTED_CALL(mocks, BUFFER_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_length(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, BUFFER_build(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_NUM_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2)
            .IgnoreArgument(3);
    }

    /*executing HTTP goodies*/
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG)) /*because relativePath*/
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,                                    /*sasObject handle                                             */
        IGNORED_PTR_ARG,
        HTTPAPI_REQUEST_POST,                                                           /*HTTPAPI_REQUEST_TYPE requestType,                  */
        "/devices/" TEST_DEVICE_ID EVENT_ENDPOINT API_VERSION,                 /*const char* relativePath,                          */
        IGNORED_PTR_ARG,                                                                /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,      */
        IGNORED_PTR_ARG,                                                                /*BUFFER_HANDLE requestContent,                      */
        IGNORED_PTR_ARG,                                                                /*unsigned int* statusCode,                          */
        NULL,                                                                           /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,     */
        NULL                                                                            /*BUFFER_HANDLE responseContent)                     */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(5)
        .IgnoreArgument(6)
        .CopyOutArgumentBuffer(7, &httpStatus200, sizeof(httpStatus200));

    /*once the event has been succesfull...*/

    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_SendComplete(TEST_IOTHUB_CLIENT_LL_HANDLE, IGNORED_PTR_ARG, IOTHUB_CLIENT_CONFIRMATION_OK))
        .IgnoreArgument(2);

    ENABLE_BATCHING();

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}


/*** IoTHubTransportHttp_GetSendStatus ***/

//Tests_SRS_TRANSPORTMULTITHTTP_17_111: [ IoTHubTransportHttp_GetSendStatus shall return IOTHUB_CLIENT_INVALID_ARG if called with NULL parameter. ]
TEST_FUNCTION(IoTHubTransportHttp_GetSendStatus_InvalidHandleArgument_fail)
{
    // arrange
    CIoTHubTransportHttpMocks mocks;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);

    mocks.ResetAllCalls();

    IOTHUB_CLIENT_STATUS status;

    // act
    IOTHUB_CLIENT_RESULT result = IoTHubTransportHttp_GetSendStatus(NULL, &status);

    // assert
    ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, result, IOTHUB_CLIENT_INVALID_ARG);

    mocks.AssertActualAndExpectedCalls();

    // cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_111: [ IoTHubTransportHttp_GetSendStatus shall return IOTHUB_CLIENT_INVALID_ARG if called with NULL parameter. ]
TEST_FUNCTION(IoTHubTransportHttp_GetSendStatus_InvalidStatusArgument_fail)
{
    // arrange
    CIoTHubTransportHttpMocks mocks;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    mocks.ResetAllCalls();

    // act
    IOTHUB_CLIENT_RESULT result = IoTHubTransportHttp_GetSendStatus(devHandle, NULL);

    // assert
    ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, result, IOTHUB_CLIENT_INVALID_ARG);

    mocks.AssertActualAndExpectedCalls();

    // cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_112: [ IoTHubTransportHttp_GetSendStatus shall return IOTHUB_CLIENT_OK and status IOTHUB_CLIENT_SEND_STATUS_IDLE if there are currently no event items to be sent or being sent. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_138: [ IoTHubTransportHttp_GetSendStatus shall locate deviceHandle in the transport device list by calling VECTOR_find_if. ]
TEST_FUNCTION(IoTHubTransportHttp_GetSendStatus_empty_waitingToSend_and_empty_eventConfirmations_success)
{
    // arrange
    CIoTHubTransportHttpMocks mocks;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, VECTOR_find_if(IGNORED_PTR_ARG, IGNORED_PTR_ARG, devHandle))
        .IgnoreArgument(1)
        .IgnoreArgument(2);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend));

    IOTHUB_CLIENT_STATUS status;

    // act
    IOTHUB_CLIENT_RESULT result = IoTHubTransportHttp_GetSendStatus(devHandle, &status);

    // assert
    ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, result, IOTHUB_CLIENT_OK);
    ASSERT_ARE_EQUAL(IOTHUB_CLIENT_STATUS, status, IOTHUB_CLIENT_SEND_STATUS_IDLE);

    mocks.AssertActualAndExpectedCalls();

    // cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_113: [ IoTHubTransportHttp_GetSendStatus shall return IOTHUB_CLIENT_OK and status IOTHUB_CLIENT_SEND_STATUS_BUSY if there are currently event items to be sent or being sent. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_138: [ IoTHubTransportHttp_GetSendStatus shall locate deviceHandle in the transport device list by calling VECTOR_find_if. ]
TEST_FUNCTION(IoTHubTransportHttp_GetSendStatus_waitingToSend_not_empty_success)
{
    // arrange
    CIoTHubTransportHttpMocks mocks;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    IOTHUB_MESSAGE_HANDLE eventMessageHandle = IoTHubMessage_CreateFromByteArray(contains3, 1);
    IOTHUB_MESSAGE_LIST newEntry;
    newEntry.messageHandle = (IOTHUB_MESSAGE_HANDLE)&newEntry;
    DList_InsertTailList(&(waitingToSend), &(newEntry.entry));

    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, VECTOR_find_if(IGNORED_PTR_ARG, IGNORED_PTR_ARG, devHandle))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend));

    IOTHUB_CLIENT_STATUS status;

    // act
    IOTHUB_CLIENT_RESULT result = IoTHubTransportHttp_GetSendStatus(devHandle, &status);

    // assert
    ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, result, IOTHUB_CLIENT_OK);
    ASSERT_ARE_EQUAL(IOTHUB_CLIENT_STATUS, status, IOTHUB_CLIENT_SEND_STATUS_BUSY);

    mocks.AssertActualAndExpectedCalls();

    // cleanup
    IoTHubTransportHttp_Destroy(handle);
    IoTHubMessage_Destroy(eventMessageHandle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_139: [ If the device structure is not found, then this function shall fail and return with IOTHUB_CLIENT_INVALID_ARG. ]
TEST_FUNCTION(IoTHubTransportHttp_GetSendStatus_deviceData_is_not_found_fails)
{
    // arrange
    CIoTHubTransportHttpMocks mocks;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    IOTHUB_MESSAGE_HANDLE eventMessageHandle = IoTHubMessage_CreateFromByteArray(contains3, 1);
    IOTHUB_MESSAGE_LIST newEntry;
    newEntry.messageHandle = (IOTHUB_MESSAGE_HANDLE) &newEntry;
    DList_InsertTailList(&(waitingToSend), &(newEntry.entry));

    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, VECTOR_find_if(IGNORED_PTR_ARG, IGNORED_PTR_ARG, devHandle))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .SetFailReturn((void_ptr)NULL);

    IOTHUB_CLIENT_STATUS status;

    // act
    IOTHUB_CLIENT_RESULT result = IoTHubTransportHttp_GetSendStatus(devHandle, &status);

    // assert
    ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, result, IOTHUB_CLIENT_INVALID_ARG);

    mocks.AssertActualAndExpectedCalls();

    // cleanup
    IoTHubTransportHttp_Destroy(handle);
    IoTHubMessage_Destroy(eventMessageHandle);
}

void setupIrrelevantMocksForProperties(CIoTHubTransportHttpMocks *mocks, IOTHUB_MESSAGE_HANDLE messageHandle) /*these are copy pasted from TEST_FUNCTION(IoTHubTransportHttp_DoWork_with_1_event_items))*/
{
    (void)(*mocks);
    STRICT_EXPECTED_CALL((*mocks), DList_IsListEmpty(&waitingToSend));

    STRICT_EXPECTED_CALL((*mocks), HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Content-Type", "application/vnd.microsoft.iothub.json"))
        .IgnoreArgument(1);

    /*starting to prepare the "big" payload*/
    STRICT_EXPECTED_CALL((*mocks), STRING_construct("["));
    STRICT_EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    /*this is first batched payload*/
    {
        STRICT_EXPECTED_CALL((*mocks), IoTHubMessage_GetContentType(messageHandle));
        STRICT_EXPECTED_CALL((*mocks), STRING_construct("{\"body\":\""));
        STRICT_EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL((*mocks), IoTHubMessage_GetByteArray(messageHandle, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(2)
            .IgnoreArgument(3);


        switch ((uintptr_t)messageHandle)
        {
        case((uintptr_t)TEST_IOTHUB_MESSAGE_HANDLE_11) :
        {
            STRICT_EXPECTED_CALL((*mocks), Base64_Encode_Bytes(buffer11, buffer11_size));
            break;
        }
        default:
        {
            STRICT_EXPECTED_CALL((*mocks), Base64_Encode_Bytes(buffer6, buffer6_size));
            break;
        }
        }

        STRICT_EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL((*mocks), STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2);

        STRICT_EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, "\"")) /*closing the value of the body*/
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, "},")) /* this extra "," is going to be harshly overwritten by a "]"*/
            .IgnoreArgument(1);
        /*end of the first batched payload*/
    }

    {
        /*adding the first payload to the "big" payload*/
        STRICT_EXPECTED_CALL((*mocks), STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2);

        /*closing the "big" payload*/
        STRICT_EXPECTED_CALL((*mocks), STRING_c_str(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL((*mocks), STRING_length(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
    }

    /*building the list of messages to be notified if HTTP is fine*/
    STRICT_EXPECTED_CALL((*mocks), DList_RemoveHeadList(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    switch ((uintptr_t)messageHandle)
    {
    case((uintptr_t)TEST_IOTHUB_MESSAGE_HANDLE_11) :
    {
        STRICT_EXPECTED_CALL((*mocks), DList_InsertTailList(IGNORED_PTR_ARG, &(message11.entry)))
            .IgnoreArgument(1);
        break;
    }

    default:
    {
        STRICT_EXPECTED_CALL((*mocks), DList_InsertTailList(IGNORED_PTR_ARG, &(message6.entry)))
            .IgnoreArgument(1);
        break;
    }
    }


    {
        /*this is building the HTTP payload... from a STRING_HANDLE (as it comes as "big payload"), into an array of bytes*/
        STRICT_EXPECTED_CALL((*mocks), BUFFER_new());
        STRICT_EXPECTED_CALL((*mocks), BUFFER_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL((*mocks), STRING_length(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL((*mocks), STRING_c_str(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL((*mocks), BUFFER_build(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_NUM_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2)
            .IgnoreArgument(3);
    }

    /*executing HTTP goodies*/
    STRICT_EXPECTED_CALL((*mocks), STRING_c_str(IGNORED_PTR_ARG)) /*because relativePath*/
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL((*mocks), HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,                                    /*sasObject handle                                             */
        IGNORED_PTR_ARG,
        HTTPAPI_REQUEST_POST,                                                           /*HTTPAPI_REQUEST_TYPE requestType,                  */
        "/devices/" TEST_DEVICE_ID EVENT_ENDPOINT API_VERSION,                 /*const char* relativePath,                          */
        IGNORED_PTR_ARG,                                                                /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,      */
        IGNORED_PTR_ARG,                                                                /*BUFFER_HANDLE requestContent,                      */
        IGNORED_PTR_ARG,                                                                /*unsigned int* statusCode,                          */
        NULL,                                                                           /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,     */
        NULL                                                                            /*BUFFER_HANDLE responseContent)                     */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(5)
        .IgnoreArgument(6)
        .CopyOutArgumentBuffer(7, &httpStatus200, sizeof(httpStatus200));

    /*once the event has been succesfull...*/

    STRICT_EXPECTED_CALL((*mocks), IoTHubClient_LL_SendComplete(TEST_IOTHUB_CLIENT_LL_HANDLE, IGNORED_PTR_ARG, IOTHUB_CLIENT_CONFIRMATION_OK))
        .IgnoreArgument(2);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_064: [ If IoTHubMessage does not have properties, then "properties":{...} shall be missing from the payload. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_059: [ It shall inspect the "waitingToSend" DLIST passed in config structure. ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_with_1_event_items_with_properties_succeeds)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    DList_InsertTailList(&(waitingToSend), &(message6.entry));
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    (void)IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    mocks.ResetAllCalls();

    setupDoWorkLoopOnceForOneDevice(mocks);

    setupIrrelevantMocksForProperties(&mocks, message6.messageHandle);

    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Properties(message6.messageHandle));
    STRICT_EXPECTED_CALL(mocks, Map_GetInternals(TEST_MAP_1_PROPERTY, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(2)
        .IgnoreArgument(3)
        .IgnoreArgument(4);

    EXPECTED_CALL(mocks, STRING_new_with_memory(IGNORED_PTR_ARG));

    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, ",\"properties\":"))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, "{"))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, "\"iothub-app-"))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, TEST_VALUES1[0]))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, "\":\""))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, TEST_KEYS1[0]))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, "\""))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, "}"))
        .IgnoreArgument(1);

    ENABLE_BATCHING();

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_063: [ Every property name shall add to the message size the length of the property name + the length of the property value + 16 bytes. ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_with_1_event_items_with_1_properties_at_maximum_message_size_succeeds)
{
    /*this test shall use a message that has a payload of MAXIMUM_MESSAGE_SIZE - PAYLOAD_OVERHEAD - PROPERTY_OVERHEAD - 2*/
    /*it will also have a property of "a":"b", therefore reaching the MAXIMUM_MESSAGE_SIZE*/
    /*the next test will increase either the propertyname or value by 1 character as the message is expected to fail*/
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    DList_InsertTailList(&(waitingToSend), &(message11.entry));
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    (void)IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    mocks.ResetAllCalls();

    setupDoWorkLoopOnceForOneDevice(mocks);

    setupIrrelevantMocksForProperties(&mocks, message11.messageHandle);

    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Properties(message11.messageHandle));
    STRICT_EXPECTED_CALL(mocks, Map_GetInternals(TEST_MAP_1_PROPERTY_A_B, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(2)
        .IgnoreArgument(3)
        .IgnoreArgument(4);

    EXPECTED_CALL(mocks, STRING_new_with_memory(IGNORED_PTR_ARG));

    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, ",\"properties\":"))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, "{"))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, "\"iothub-app-"))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, TEST_VALUES1_A_B[0]))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, "\":\""))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, TEST_KEYS1_A_B[0]))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, "\""))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, "}"))
        .IgnoreArgument(1);

    ENABLE_BATCHING();

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_063: [ Every property name shall add to the message size the length of the property name + the length of the property value + 16 bytes. ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_with_1_event_items_with_1_properties_past_maximum_message_size_fails)
{
    /*this test shall use a message that has a payload of MAXIMUM_MESSAGE_SIZE - PAYLOAD_OVERHEAD - PROPERTY_OVERHEAD - 2*/
    /*it will also have a property of "aa":"b", therefore reaching 1 byte past the MAXIMUM_MESSAGE_SIZE*/
    /*this is done in a very e2e manner*/
    ///arrange
    CNiceCallComparer<CIoTHubTransportHttpMocks>mocks;
    DList_InsertTailList(&(waitingToSend), &(message12.entry));
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    (void)IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    ENABLE_BATCHING();


    mocks.ResetAllCalls();

    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_SendComplete(TEST_IOTHUB_CLIENT_LL_HANDLE, IGNORED_PTR_ARG, IOTHUB_CLIENT_CONFIRMATION_ERROR))
        .IgnoreArgument(2);

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_064: [ If IoTHubMessage does not have properties, then "properties":{...} shall be missing from the payload. ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_with_1_event_items_fails_when_Map_GetInternals_fails)
{
    ///arrange
    CNiceCallComparer<CIoTHubTransportHttpMocks> mocks;
    DList_InsertTailList(&(waitingToSend), &(message6.entry));
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    (void)IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    mocks.ResetAllCalls();

    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, Map_GetInternals(TEST_MAP_1_PROPERTY, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(2)
        .IgnoreArgument(3)
        .IgnoreArgument(4)
        .SetReturn(MAP_ERROR);

    ENABLE_BATCHING();

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    ASSERT_IS_NULL(last_BUFFER_HANDLE_to_HTTPAPIEX_ExecuteRequest);
    mocks.AssertActualAndExpectedCalls();


    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

void setupIrrelevantMocksForProperties2(CIoTHubTransportHttpMocks *mocks, IOTHUB_MESSAGE_HANDLE h1, IOTHUB_MESSAGE_HANDLE h2) /*these are copy pasted from TEST_FUNCTION(IoTHubTransportHttp_DoWork_with_1_event_items))*/
{
    (void)(*mocks);
    STRICT_EXPECTED_CALL((*mocks), DList_IsListEmpty(&waitingToSend));

    STRICT_EXPECTED_CALL((*mocks), HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Content-Type", "application/vnd.microsoft.iothub.json"))
        .IgnoreArgument(1);

    /*starting to prepare the "big" payload*/
    STRICT_EXPECTED_CALL((*mocks), STRING_construct("["));
    STRICT_EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    /*this is first batched payload*/
    {
        STRICT_EXPECTED_CALL((*mocks), IoTHubMessage_GetContentType(h1));
        STRICT_EXPECTED_CALL((*mocks), STRING_construct("{\"body\":\""));
        STRICT_EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL((*mocks), IoTHubMessage_GetByteArray(message6.messageHandle, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(2)
            .IgnoreArgument(3);

        STRICT_EXPECTED_CALL((*mocks), Base64_Encode_Bytes(buffer6, buffer6_size));
        STRICT_EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL((*mocks), STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2);

        STRICT_EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, "\"")) /*closing the value of the body*/
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, "},")) /* this extra "," is going to be harshly overwritten by a "]"*/
            .IgnoreArgument(1);
        /*end of the first batched payload*/
    }

    {
        /*adding the first payload to the "big" payload*/
        STRICT_EXPECTED_CALL((*mocks), STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2);
    }

    /*this is second batched payload*/
    {
        STRICT_EXPECTED_CALL((*mocks), IoTHubMessage_GetContentType(h2));
        STRICT_EXPECTED_CALL((*mocks), STRING_construct("{\"body\":\""));
        STRICT_EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL((*mocks), IoTHubMessage_GetByteArray(message7.messageHandle, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(2)
            .IgnoreArgument(3);

        STRICT_EXPECTED_CALL((*mocks), Base64_Encode_Bytes(buffer7, buffer7_size));
        STRICT_EXPECTED_CALL((*mocks), STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL((*mocks), STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2);

        STRICT_EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, "\"")) /*closing the value of the body*/
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL((*mocks), STRING_concat(IGNORED_PTR_ARG, "},")) /* this extra "," is going to be harshly overwritten by a "]"*/
            .IgnoreArgument(1);
        /*end of the first batched payload*/
    }

    {
        /*adding the second payload to the "big" payload*/
        STRICT_EXPECTED_CALL((*mocks), STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2);

        /*closing the "big" payload*/
        STRICT_EXPECTED_CALL((*mocks), STRING_c_str(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL((*mocks), STRING_length(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
    }

    /*building the list of messages to be notified if HTTP is fine*/
    STRICT_EXPECTED_CALL((*mocks), DList_RemoveHeadList(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL((*mocks), DList_InsertTailList(IGNORED_PTR_ARG, &(message6.entry)))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL((*mocks), DList_RemoveHeadList(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL((*mocks), DList_InsertTailList(IGNORED_PTR_ARG, &(message7.entry)))
        .IgnoreArgument(1);


    {
        /*this is building the HTTP payload... from a STRING_HANDLE (as it comes as "big payload"), into an array of bytes*/
        STRICT_EXPECTED_CALL((*mocks), BUFFER_new());
        STRICT_EXPECTED_CALL((*mocks), BUFFER_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL((*mocks), STRING_length(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL((*mocks), STRING_c_str(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL((*mocks), BUFFER_build(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_NUM_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2)
            .IgnoreArgument(3);
    }

    /*executing HTTP goodies*/
    STRICT_EXPECTED_CALL((*mocks), STRING_c_str(IGNORED_PTR_ARG)) /*because relativePath*/
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL((*mocks), HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,                                    /*sasObject handle                                             */
        IGNORED_PTR_ARG,
        HTTPAPI_REQUEST_POST,                                                           /*HTTPAPI_REQUEST_TYPE requestType,                  */
        "/devices/" TEST_DEVICE_ID EVENT_ENDPOINT API_VERSION,                 /*const char* relativePath,                          */
        IGNORED_PTR_ARG,                                                                /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,      */
        IGNORED_PTR_ARG,                                                                /*BUFFER_HANDLE requestContent,                      */
        IGNORED_PTR_ARG,                                                                /*unsigned int* statusCode,                          */
        NULL,                                                                           /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,     */
        NULL                                                                            /*BUFFER_HANDLE responseContent)                     */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(5)
        .IgnoreArgument(6)
        .CopyOutArgumentBuffer(7, &httpStatus200, sizeof(httpStatus200));

    /*once the event has been succesfull...*/

    STRICT_EXPECTED_CALL((*mocks), IoTHubClient_LL_SendComplete(TEST_IOTHUB_CLIENT_LL_HANDLE, IGNORED_PTR_ARG, IOTHUB_CLIENT_CONFIRMATION_OK))
        .IgnoreArgument(2);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_064: [ If IoTHubMessage does not have properties, then "properties":{...} shall be missing from the payload. ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_with_2_event_items_with_properties_succeeds)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    DList_InsertTailList(&(waitingToSend), &(message6.entry));
    DList_InsertTailList(&(waitingToSend), &(message7.entry));
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    (void)IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    mocks.ResetAllCalls();

    setupDoWorkLoopOnceForOneDevice(mocks);

    setupIrrelevantMocksForProperties2(&mocks, message6.messageHandle, message7.messageHandle);

    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Properties(message6.messageHandle));
    STRICT_EXPECTED_CALL(mocks, Map_GetInternals(TEST_MAP_1_PROPERTY, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(2)
        .IgnoreArgument(3)
        .IgnoreArgument(4);

    EXPECTED_CALL(mocks, STRING_new_with_memory(IGNORED_PTR_ARG))
        .ExpectedAtLeastTimes(2);

    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, ",\"properties\":"))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, "{")) /*opening of the properties*/
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, "\"iothub-app-"))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, TEST_VALUES1[0]))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, "\":\""))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, TEST_KEYS1[0]))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, "\""))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, "}"))/*closing of the properties*/
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Properties(message7.messageHandle));
    STRICT_EXPECTED_CALL(mocks, Map_GetInternals(TEST_MAP_2_PROPERTY, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(2)
        .IgnoreArgument(3)
        .IgnoreArgument(4);

    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, ",\"properties\":"))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, "{")) /*opening of the properties*/
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, "\"iothub-app-"))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, TEST_VALUES2[0]))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, "\":\""))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, TEST_KEYS2[0]))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, "\""))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, ",\"iothub-app-"))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, TEST_VALUES2[1]))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, "\":\""))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, TEST_KEYS2[1]))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, "\""))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, "}"))/*closing of the properties*/
        .IgnoreArgument(1);

    ENABLE_BATCHING();

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}


#define TEST_2_ITEM_STRING "[{\"body\":\"MTIzNDU2\",\"properties\":{" TEST_RED_KEY_STRING_WITH_IOTHUBAPP ":" TEST_RED_VALUE_STRING "}},{\"body\":\"MTIzNDU2Nw==\",\"properties\":{" TEST_BLUE_KEY_STRING_WITH_IOTHUBAPP ":" TEST_BLUE_VALUE_STRING "," TEST_YELLOW_KEY_STRING_WITH_IOTHUBAPP ":" TEST_YELLOW_VALUE_STRING "}}]"
#define TEST_1_ITEM_STRING "[{\"body\":\"MTIzNDU2\",\"properties\":{" TEST_RED_KEY_STRING_WITH_IOTHUBAPP ":" TEST_RED_VALUE_STRING "}}]"

//Tests_SRS_TRANSPORTMULTITHTTP_17_058: [ If IoTHubMessage has properties, then they shall be serialized at the same level as "body" using the following pattern: "properties":{"iothub-app-name1":"value1","iothub-app-name2":"value2"} ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_with_2_event_items)
{
    ///arrange
    CNiceCallComparer<CIoTHubTransportHttpMocks> mocks; /*a very e2e test... */
    DList_InsertTailList(&(waitingToSend), &(message6.entry));
    DList_InsertTailList(&(waitingToSend), &(message7.entry));
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    (void)IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    mocks.ResetAllCalls();

    setupDoWorkLoopOnceForOneDevice(mocks);

    ENABLE_BATCHING();

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    ASSERT_ARE_EQUAL(int, 0, memcmp(BASEIMPLEMENTATION::BUFFER_u_char(last_BUFFER_HANDLE_to_HTTPAPIEX_ExecuteRequest), TEST_2_ITEM_STRING, sizeof(TEST_2_ITEM_STRING) - 1));
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

#define THRESHOLD1 32 /*between THRESHOLD1 and THRESHOLD2 of STRING_concat fails there still is produced a payload, consisting of only REDKEY, REDVALUE*/
#define THRESHOLD2 17
#define THRESHOLD3 7 /*below 7, STRING_concat would fail not in producing properties*/

//Tests_SRS_TRANSPORTMULTITHTTP_17_058: [ If IoTHubMessage has properties, then they shall be serialized at the same level as "body" using the following pattern: "properties":{"iothub-app-name1":"value1","iothub-app-name2":"value2"} ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_with_2_event_items_THRESHOLD1_succeeds)
{
    for (size_t i = THRESHOLD1 + 1; i > THRESHOLD1; i--)
    {
        ///arrange
        currentSTRING_concat_call = 0;
        whenShallSTRING_concat_fail = 0;
        BASEIMPLEMENTATION::DList_InitializeListHead(&waitingToSend);
        CNiceCallComparer<CIoTHubTransportHttpMocks> mocks; /*a very e2e test... */
        DList_InsertTailList(&(waitingToSend), &(message6.entry));
        DList_InsertTailList(&(waitingToSend), &(message7.entry));
        auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
        (void)IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

        mocks.ResetAllCalls();

        setupDoWorkLoopOnceForOneDevice(mocks);

        whenShallSTRING_concat_fail = i;

        ENABLE_BATCHING();

        ///act
        IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

        ///assert
        ASSERT_ARE_EQUAL(int, 0, memcmp(BASEIMPLEMENTATION::BUFFER_u_char(last_BUFFER_HANDLE_to_HTTPAPIEX_ExecuteRequest), TEST_2_ITEM_STRING, sizeof(TEST_2_ITEM_STRING) - 1));
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubTransportHttp_Destroy(handle);
        if (last_BUFFER_HANDLE_to_HTTPAPIEX_ExecuteRequest != NULL)
        {
            BASEIMPLEMENTATION::BUFFER_delete(last_BUFFER_HANDLE_to_HTTPAPIEX_ExecuteRequest);
            last_BUFFER_HANDLE_to_HTTPAPIEX_ExecuteRequest = NULL;
        }
    }
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_058: [ If IoTHubMessage has properties, then they shall be serialized at the same level as "body" using the following pattern: "properties":{"iothub-app-name1":"value1","iothub-app-name2":"value2"} ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_with_2_event_items_send_only_1_when_properties_for_second_fail)
{
    for (size_t i = THRESHOLD1; i > THRESHOLD2; i--)
    {
        ///arrange
        currentSTRING_concat_call = 0;
        whenShallSTRING_concat_fail = 0;
        BASEIMPLEMENTATION::DList_InitializeListHead(&waitingToSend);
        CNiceCallComparer<CIoTHubTransportHttpMocks> mocks; /*a very e2e test... */
        DList_InsertTailList(&(waitingToSend), &(message6.entry));
        DList_InsertTailList(&(waitingToSend), &(message7.entry));
        auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
        (void)IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

        mocks.ResetAllCalls();

        setupDoWorkLoopOnceForOneDevice(mocks);

        whenShallSTRING_concat_fail = i;

        ENABLE_BATCHING();

        ///act
        IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

        ///assert
        ASSERT_ARE_EQUAL(int, 0, memcmp(BASEIMPLEMENTATION::BUFFER_u_char(last_BUFFER_HANDLE_to_HTTPAPIEX_ExecuteRequest), TEST_1_ITEM_STRING, sizeof(TEST_1_ITEM_STRING) - 1));
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubTransportHttp_Destroy(handle);
        if (last_BUFFER_HANDLE_to_HTTPAPIEX_ExecuteRequest != NULL)
        {
            BASEIMPLEMENTATION::BUFFER_delete(last_BUFFER_HANDLE_to_HTTPAPIEX_ExecuteRequest);
            last_BUFFER_HANDLE_to_HTTPAPIEX_ExecuteRequest = NULL;
        }
    }
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_058: [ If IoTHubMessage has properties, then they shall be serialized at the same level as "body" using the following pattern: "properties":{"iothub-app-name1":"value1","iothub-app-name2":"value2"} ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_with_2_event_items_send_nothing)
{
    for (size_t i = THRESHOLD2; i > THRESHOLD3; i--)
    {
        ///arrange
        currentSTRING_concat_call = 0;
        whenShallSTRING_concat_fail = 0;
        BASEIMPLEMENTATION::DList_InitializeListHead(&waitingToSend);
        CNiceCallComparer<CIoTHubTransportHttpMocks> mocks; /*a very e2e test... */
        DList_InsertTailList(&(waitingToSend), &(message6.entry));
        DList_InsertTailList(&(waitingToSend), &(message7.entry));
        auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
        mocks.ResetAllCalls();

        whenShallSTRING_concat_fail = i;

        ENABLE_BATCHING();

        ///act
        IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

        ///assert
        ASSERT_IS_NULL(last_BUFFER_HANDLE_to_HTTPAPIEX_ExecuteRequest);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubTransportHttp_Destroy(handle);
        if (last_BUFFER_HANDLE_to_HTTPAPIEX_ExecuteRequest != NULL)
        {
            BASEIMPLEMENTATION::BUFFER_delete(last_BUFFER_HANDLE_to_HTTPAPIEX_ExecuteRequest);
            last_BUFFER_HANDLE_to_HTTPAPIEX_ExecuteRequest = NULL;
        }
    }
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_114: [ If handle parameter is NULL then IoTHubTransportHttp_SetOption shall return IOTHUB_CLIENT_INVALID_ARG. ]
TEST_FUNCTION(IoTHubTransportHttp_SetOption_with_NULL_handle_fails)
{
    ///arrange

    ///act
    auto result = IoTHubTransportHttp_SetOption(NULL, "someOption", "someValue");

    ///assert
    ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_INVALID_ARG, result);

    ///cleanup
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_115: [ If option parameter is NULL then IoTHubTransportHttp_SetOption shall return IOTHUB_CLIENT_INVALID_ARG. ]
TEST_FUNCTION(IoTHubTransportHttp_SetOption_with_NULL_optionName_fails)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    mocks.ResetAllCalls();

    ///act
    auto result = IoTHubTransportHttp_SetOption(handle, NULL, "someValue");

    ///assert
    ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_INVALID_ARG, result);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_116: [ If value parameter is NULL then IoTHubTransportHttp_SetOption shall return IOTHUB_CLIENT_INVALID_ARG. ]
TEST_FUNCTION(IoTHubTransportHttp_SetOption_with_NULL_value_fails)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    mocks.ResetAllCalls();

    ///act
    auto result = IoTHubTransportHttp_SetOption(handle, "someOption", NULL);

    ///assert
    ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_INVALID_ARG, result);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_119: [ The following table translates HTTPAPIEX return codes to IOTHUB_CLIENT_RESULT return codes: ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_118: [ Otherwise, IoTHubTransport_Http shall call HTTPAPIEX_SetOption with the same parameters and return the translated code. ]
TEST_FUNCTION(IoTHubTransportHttp_SetOption_succeeds_when_HTTPAPIEX_succeeds)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SetOption(TEST_HTTPAPIEX_HANDLE, "someOption", (void*)42));

    ///act
    auto result = IoTHubTransportHttp_SetOption(handle, "someOption", (void*)42);

    ///assert
    ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK, result);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_119: [ The following table translates HTTPAPIEX return codes to IOTHUB_CLIENT_RESULT return codes: ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_118: [ Otherwise, IoTHubTransport_Http shall call HTTPAPIEX_SetOption with the same parameters and return the translated code. ]
TEST_FUNCTION(IoTHubTransportHttp_SetOption_fails_when_HTTPAPIEX_returns_HTTPAPIEX_INVALID_ARG)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SetOption(TEST_HTTPAPIEX_HANDLE, "someOption", (void*)42))
        .SetReturn(HTTPAPIEX_INVALID_ARG);

    ///act
    auto result = IoTHubTransportHttp_SetOption(handle, "someOption", (void*)42);

    ///assert
    ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_INVALID_ARG, result);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_119: [ The following table translates HTTPAPIEX return codes to IOTHUB_CLIENT_RESULT return codes: ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_118: [ Otherwise, IoTHubTransport_Http shall call HTTPAPIEX_SetOption with the same parameters and return the translated code. ]
TEST_FUNCTION(IoTHubTransportHttp_SetOption_fails_when_HTTPAPIEX_returns_HTTPAPIEX_ERROR)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SetOption(TEST_HTTPAPIEX_HANDLE, "someOption", (void*)42))
        .SetReturn(HTTPAPIEX_ERROR);

    ///act
    auto result = IoTHubTransportHttp_SetOption(handle, "someOption", (void*)42);

    ///assert
    ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_ERROR, result);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_119: [ The following table translates HTTPAPIEX return codes to IOTHUB_CLIENT_RESULT return codes: ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_118: [ Otherwise, IoTHubTransport_Http shall call HTTPAPIEX_SetOption with the same parameters and return the translated code. ]
TEST_FUNCTION(IoTHubTransportHttp_SetOption_fails_when_HTTPAPIEX_returns_any_other_error)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SetOption(TEST_HTTPAPIEX_HANDLE, "someOption", (void*)42))
        .SetReturn(HTTPAPIEX_RECOVERYFAILED);

    ///act
    auto result = IoTHubTransportHttp_SetOption(handle, "someOption", (void*)42);

    ///assert
    ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_ERROR, result);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_096: [ If IoTHubClient_LL_MessageCallback returns IOTHUBMESSAGE_ABANDONED then _DoWork shall "abandon" the message. ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_happy_path_with_empty_waitingToSend_and_1_service_message_with_abandon_succeeds)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    unsigned int statusCode200 = 200;
    unsigned int statusCode204 = 204;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    (void)IoTHubTransportHttp_Subscribe(devHandle);
    mocks.ResetAllCalls();
    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend)); /*because DoWork for event*/

    STRICT_EXPECTED_CALL(mocks, get_time(NULL));
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc()); /*because responseHeadearsHandle: a new instance of HTTP headers*/
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, BUFFER_new());
    STRICT_EXPECTED_CALL(mocks, BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1); /*because relativePath is a STRING_HANDLE*/
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,                                    /*sasObject handle                                             */
        IGNORED_PTR_ARG,                                    /*HTTPAPIEX_HANDLE handle,                                     */
        HTTPAPI_REQUEST_GET,                                /*HTTPAPI_REQUEST_TYPE requestType,                            */
        "/devices/" TEST_DEVICE_ID MESSAGE_ENDPOINT_HTTP API_VERSION,    /*const char* relativePath,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,                */
        NULL,                                               /*BUFFER_HANDLE requestContent,                                */
        IGNORED_PTR_ARG,                                    /*unsigned int* statusCode,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,               */
        IGNORED_PTR_ARG                                     /*BUFFER_HANDLE responseContent))                              */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(5)
        .IgnoreArgument(7)
        .IgnoreArgument(8)
        .IgnoreArgument(9)
        .CopyOutArgumentBuffer(7, &statusCode200, sizeof(statusCode200));

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_FindHeaderValue(IGNORED_PTR_ARG, "ETag"))
        .IgnoreArgument(1)
        .SetReturn(TEST_ETAG_VALUE);

    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_CreateFromByteArray(IGNORED_PTR_ARG, IGNORED_NUM_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Destroy(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_u_char(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_GetHeaderCount(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);

    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_MessageCallback(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .SetReturn(IOTHUBMESSAGE_ABANDONED);

    /*this is "abandon"*/
    STRICT_EXPECTED_CALL(mocks, STRING_clone(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_construct_n(TEST_ETAG_VALUE_UNQUOTED, sizeof(TEST_ETAG_VALUE_UNQUOTED) - 1))
        .ValidateArgumentBuffer(1, TEST_ETAG_VALUE_UNQUOTED, sizeof(TEST_ETAG_VALUE_UNQUOTED) - 1);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, "/abandon" API_VERSION))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc());
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "User-Agent", CLIENT_DEVICE_TYPE_PREFIX CLIENT_DEVICE_BACKSLASH IOTHUB_SDK_VERSION))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "Authorization", TEST_BLANK_SAS_TOKEN))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "If-Match", TEST_ETAG_VALUE))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1); /*because abandon relativePath is a STRING_HANDLE*/
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,
        IGNORED_PTR_ARG,                                    /*HTTPAPIEX_HANDLE handle,                                     */
        HTTPAPI_REQUEST_POST,                               /*HTTPAPI_REQUEST_TYPE requestType,                            */
        "/devices/" TEST_DEVICE_ID MESSAGE_ENDPOINT_HTTP_ETAG TEST_ETAG_VALUE_UNQUOTED "/abandon" API_VERSION,    /*const char* relativePath,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,                */
        NULL,                                               /*BUFFER_HANDLE requestContent,                                */
        IGNORED_PTR_ARG,                                    /*unsigned int* statusCode,                                    */
        NULL,                                               /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,               */
        NULL                                                /*BUFFER_HANDLE responseContent))                              */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(5)
        .IgnoreArgument(7)
        .CopyOutArgumentBuffer(7, &statusCode204, sizeof(statusCode204));

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_090: [ All the HTTP headers of the form iothub-app-name:somecontent shall be transformed in message properties {name, somecontent}. ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_happy_path_with_empty_waitingToSend_and_1_service_message_with_1_property_succeeds)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    unsigned int statusCode200 = 200;
    unsigned int statusCode204 = 204;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    (void)IoTHubTransportHttp_Subscribe(devHandle);
    size_t nHeaders = 1;
    mocks.ResetAllCalls();

    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend)); /*because DoWork for event*/

    STRICT_EXPECTED_CALL(mocks, get_time(NULL));
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc()); /*because responseHeadearsHandle: a new instance of HTTP headers*/
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, BUFFER_new());
    STRICT_EXPECTED_CALL(mocks, BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1); /*because relativePath is a STRING_HANDLE*/
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,
        IGNORED_PTR_ARG,                                    /*HTTPAPIEX_HANDLE handle,                                     */
        HTTPAPI_REQUEST_GET,                                /*HTTPAPI_REQUEST_TYPE requestType,                            */
        "/devices/" TEST_DEVICE_ID MESSAGE_ENDPOINT_HTTP API_VERSION,    /*const char* relativePath,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,                */
        NULL,                                               /*BUFFER_HANDLE requestContent,                                */
        IGNORED_PTR_ARG,                                    /*unsigned int* statusCode,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,               */
        IGNORED_PTR_ARG                                     /*BUFFER_HANDLE responseContent))                              */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(5)
        .IgnoreArgument(7)
        .IgnoreArgument(8)
        .IgnoreArgument(9)
        .CopyOutArgumentBuffer(7, &statusCode200, sizeof(statusCode200));

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_FindHeaderValue(IGNORED_PTR_ARG, "ETag"))
        .IgnoreArgument(1)
        .SetReturn(TEST_ETAG_VALUE);

    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_CreateFromByteArray(IGNORED_PTR_ARG, IGNORED_NUM_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .SetReturn(TEST_IOTHUB_MESSAGE_HANDLE_8);
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Destroy(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_u_char(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    {
        /*this scope is for is properties code*/

        HTTPHeaders_GetHeaderCount_writes_to_its_outputs = false;
        STRICT_EXPECTED_CALL(mocks, HTTPHeaders_GetHeaderCount(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .CopyOutArgumentBuffer(2, &nHeaders, sizeof(nHeaders));

        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Properties(TEST_IOTHUB_MESSAGE_HANDLE_8));

        STRICT_EXPECTED_CALL(mocks, HTTPHeaders_GetHeader(IGNORED_PTR_ARG, 0, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(3);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, Map_AddOrUpdate(TEST_MAP_3_PROPERTY, "NAME1", "VALUE1"));
    }

    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_MessageCallback(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);


    /*this returns "0" so the message needs to be "accepted"*/
    /*this is "accepting"*/
    STRICT_EXPECTED_CALL(mocks, STRING_clone(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_construct_n(TEST_ETAG_VALUE_UNQUOTED, sizeof(TEST_ETAG_VALUE_UNQUOTED) - 1))
        .ValidateArgumentBuffer(1, TEST_ETAG_VALUE_UNQUOTED, sizeof(TEST_ETAG_VALUE_UNQUOTED) - 1);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, API_VERSION))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc());
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "User-Agent", CLIENT_DEVICE_TYPE_PREFIX CLIENT_DEVICE_BACKSLASH IOTHUB_SDK_VERSION))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "Authorization", TEST_BLANK_SAS_TOKEN))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "If-Match", TEST_ETAG_VALUE))
        .IgnoreArgument(1);


    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1); /*because abandon relativePath is a STRING_HANDLE*/
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,
        IGNORED_PTR_ARG,                                    /*HTTPAPIEX_HANDLE handle,                                     */
        HTTPAPI_REQUEST_DELETE,                                /*HTTPAPI_REQUEST_TYPE requestType,                            */
        "/devices/" TEST_DEVICE_ID MESSAGE_ENDPOINT_HTTP_ETAG TEST_ETAG_VALUE_UNQUOTED API_VERSION,    /*const char* relativePath,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,                */
        NULL,                                               /*BUFFER_HANDLE requestContent,                                */
        IGNORED_PTR_ARG,                                    /*unsigned int* statusCode,                                    */
        NULL,                                               /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,               */
        NULL                                                /*BUFFER_HANDLE responseContent))                              */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(5)
        .IgnoreArgument(7)
        .CopyOutArgumentBuffer(7, &statusCode204, sizeof(statusCode204));

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_090: [ All the HTTP headers of the form iothub-app-name:somecontent shall be transformed in message properties {name, somecontent}. ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_happy_path_with_empty_waitingToSend_and_1_service_message_with_2_property_succeeds)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    unsigned int statusCode200 = 200;
    unsigned int statusCode204 = 204;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    (void)IoTHubTransportHttp_Subscribe(devHandle);
    size_t nHeaders = 3;
    mocks.ResetAllCalls();

    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend)); /*because DoWork for event*/

    STRICT_EXPECTED_CALL(mocks, get_time(NULL));
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc()); /*because responseHeadearsHandle: a new instance of HTTP headers*/
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, BUFFER_new());
    STRICT_EXPECTED_CALL(mocks, BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1); /*because relativePath is a STRING_HANDLE*/
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,
        IGNORED_PTR_ARG,                                    /*HTTPAPIEX_HANDLE handle,                                     */
        HTTPAPI_REQUEST_GET,                                /*HTTPAPI_REQUEST_TYPE requestType,                            */
        "/devices/" TEST_DEVICE_ID MESSAGE_ENDPOINT_HTTP API_VERSION,    /*const char* relativePath,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,                */
        NULL,                                               /*BUFFER_HANDLE requestContent,                                */
        IGNORED_PTR_ARG,                                    /*unsigned int* statusCode,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,               */
        IGNORED_PTR_ARG                                     /*BUFFER_HANDLE responseContent))                              */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(5)
        .IgnoreArgument(7)
        .IgnoreArgument(8)
        .IgnoreArgument(9)
        .CopyOutArgumentBuffer(7, &statusCode200, sizeof(statusCode200));

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_FindHeaderValue(IGNORED_PTR_ARG, "ETag"))
        .IgnoreArgument(1)
        .SetReturn(TEST_ETAG_VALUE);

    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_CreateFromByteArray(IGNORED_PTR_ARG, IGNORED_NUM_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .SetReturn(TEST_IOTHUB_MESSAGE_HANDLE_8);
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Destroy(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_u_char(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    {
        /*this scope is for is properties code*/

        HTTPHeaders_GetHeaderCount_writes_to_its_outputs = false;
        STRICT_EXPECTED_CALL(mocks, HTTPHeaders_GetHeaderCount(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .CopyOutArgumentBuffer(2, &nHeaders, sizeof(nHeaders));

        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Properties(TEST_IOTHUB_MESSAGE_HANDLE_8));

        STRICT_EXPECTED_CALL(mocks, HTTPHeaders_GetHeader(IGNORED_PTR_ARG, 0, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(3);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, Map_AddOrUpdate(TEST_MAP_3_PROPERTY, "NAME1", "VALUE1"));

        STRICT_EXPECTED_CALL(mocks, HTTPHeaders_GetHeader(IGNORED_PTR_ARG, 1, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(3);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, HTTPHeaders_GetHeader(IGNORED_PTR_ARG, 2, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(3);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, Map_AddOrUpdate(TEST_MAP_3_PROPERTY, "NAME2", "VALUE2"));
    }

    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_MessageCallback(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);


    /*this returns "0" so the message needs to be "accepted"*/
    /*this is "accepting"*/
    STRICT_EXPECTED_CALL(mocks, STRING_clone(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_construct_n(TEST_ETAG_VALUE_UNQUOTED, sizeof(TEST_ETAG_VALUE_UNQUOTED) - 1))
        .ValidateArgumentBuffer(1, TEST_ETAG_VALUE_UNQUOTED, sizeof(TEST_ETAG_VALUE_UNQUOTED) - 1);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, API_VERSION))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc());
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "User-Agent", CLIENT_DEVICE_TYPE_PREFIX CLIENT_DEVICE_BACKSLASH IOTHUB_SDK_VERSION))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "Authorization", TEST_BLANK_SAS_TOKEN))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "If-Match", TEST_ETAG_VALUE))
        .IgnoreArgument(1);


    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1); /*because abandon relativePath is a STRING_HANDLE*/
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,
        IGNORED_PTR_ARG,                                    /*HTTPAPIEX_HANDLE handle,                                     */
        HTTPAPI_REQUEST_DELETE,                                /*HTTPAPI_REQUEST_TYPE requestType,                            */
        "/devices/" TEST_DEVICE_ID MESSAGE_ENDPOINT_HTTP_ETAG TEST_ETAG_VALUE_UNQUOTED API_VERSION,    /*const char* relativePath,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,                */
        NULL,                                               /*BUFFER_HANDLE requestContent,                                */
        IGNORED_PTR_ARG,                                    /*unsigned int* statusCode,                                    */
        NULL,                                               /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,               */
        NULL                                                /*BUFFER_HANDLE responseContent))                              */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(5)
        .IgnoreArgument(7)
        .CopyOutArgumentBuffer(7, &statusCode204, sizeof(statusCode204));

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_090: [ All the HTTP headers of the form iothub-app-name:somecontent shall be transformed in message properties {name, somecontent}. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_092: [ If assembling the message fails in any way, then _DoWork shall "abandon" the message. ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_happy_path_with_empty_waitingToSend_and_1_service_message_with_2_property_when_properties_fails_it_abandons_1)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    unsigned int statusCode200 = 200;
    unsigned int statusCode204 = 204;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    (void)IoTHubTransportHttp_Subscribe(devHandle);
    size_t nHeaders = 3;
    mocks.ResetAllCalls();

    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend)); /*because DoWork for event*/

    STRICT_EXPECTED_CALL(mocks, get_time(NULL));
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc()); /*because responseHeadearsHandle: a new instance of HTTP headers*/
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, BUFFER_new());
    STRICT_EXPECTED_CALL(mocks, BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1); /*because relativePath is a STRING_HANDLE*/
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,
        IGNORED_PTR_ARG,                                    /*HTTPAPIEX_HANDLE handle,                                     */
        HTTPAPI_REQUEST_GET,                                /*HTTPAPI_REQUEST_TYPE requestType,                            */
        "/devices/" TEST_DEVICE_ID MESSAGE_ENDPOINT_HTTP API_VERSION,    /*const char* relativePath,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,                */
        NULL,                                               /*BUFFER_HANDLE requestContent,                                */
        IGNORED_PTR_ARG,                                    /*unsigned int* statusCode,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,               */
        IGNORED_PTR_ARG                                     /*BUFFER_HANDLE responseContent))                              */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(5)
        .IgnoreArgument(7)
        .IgnoreArgument(8)
        .IgnoreArgument(9)
        .CopyOutArgumentBuffer(7, &statusCode200, sizeof(statusCode200));

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_FindHeaderValue(IGNORED_PTR_ARG, "ETag"))
        .IgnoreArgument(1)
        .SetReturn(TEST_ETAG_VALUE);

    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_CreateFromByteArray(IGNORED_PTR_ARG, IGNORED_NUM_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .SetReturn(TEST_IOTHUB_MESSAGE_HANDLE_8);
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Destroy(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_u_char(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    {
        /*this scope is for is properties code*/

        HTTPHeaders_GetHeaderCount_writes_to_its_outputs = false;
        STRICT_EXPECTED_CALL(mocks, HTTPHeaders_GetHeaderCount(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .CopyOutArgumentBuffer(2, &nHeaders, sizeof(nHeaders));

        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Properties(TEST_IOTHUB_MESSAGE_HANDLE_8));

        STRICT_EXPECTED_CALL(mocks, HTTPHeaders_GetHeader(IGNORED_PTR_ARG, 0, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(3);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, Map_AddOrUpdate(TEST_MAP_3_PROPERTY, "NAME1", "VALUE1"));

        STRICT_EXPECTED_CALL(mocks, HTTPHeaders_GetHeader(IGNORED_PTR_ARG, 1, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(3);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, HTTPHeaders_GetHeader(IGNORED_PTR_ARG, 2, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(3);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, Map_AddOrUpdate(TEST_MAP_3_PROPERTY, "NAME2", "VALUE2"))
            .SetReturn(MAP_ERROR);
    }

    /*this is "abandon"*/
    STRICT_EXPECTED_CALL(mocks, STRING_clone(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_construct_n(TEST_ETAG_VALUE_UNQUOTED, sizeof(TEST_ETAG_VALUE_UNQUOTED) - 1))
        .ValidateArgumentBuffer(1, TEST_ETAG_VALUE_UNQUOTED, sizeof(TEST_ETAG_VALUE_UNQUOTED) - 1);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, "/abandon" API_VERSION))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc());
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "User-Agent", CLIENT_DEVICE_TYPE_PREFIX CLIENT_DEVICE_BACKSLASH IOTHUB_SDK_VERSION))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "Authorization", TEST_BLANK_SAS_TOKEN))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "If-Match", TEST_ETAG_VALUE))
        .IgnoreArgument(1);


    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1); /*because abandon relativePath is a STRING_HANDLE*/
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,
        IGNORED_PTR_ARG,                                    /*HTTPAPIEX_HANDLE handle,                                     */
        HTTPAPI_REQUEST_POST,                               /*HTTPAPI_REQUEST_TYPE requestType,                            */
        "/devices/" TEST_DEVICE_ID MESSAGE_ENDPOINT_HTTP_ETAG TEST_ETAG_VALUE_UNQUOTED "/abandon" API_VERSION,    /*const char* relativePath,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,                */
        NULL,                                               /*BUFFER_HANDLE requestContent,                                */
        IGNORED_PTR_ARG,                                    /*unsigned int* statusCode,                                    */
        NULL,                                               /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,               */
        NULL                                                /*BUFFER_HANDLE responseContent))                              */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(5)
        .IgnoreArgument(7)
        .CopyOutArgumentBuffer(7, &statusCode204, sizeof(statusCode204));

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}


//Tests_SRS_TRANSPORTMULTITHTTP_17_090: [ All the HTTP headers of the form iothub-app-name:somecontent shall be transformed in message properties {name, somecontent}. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_092: [ If assembling the message fails in any way, then _DoWork shall "abandon" the message. ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_happy_path_with_empty_waitingToSend_and_1_service_message_with_2_property_when_properties_fails_it_abandons_2)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    unsigned int statusCode200 = 200;
    unsigned int statusCode204 = 204;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    (void)IoTHubTransportHttp_Subscribe(devHandle);
    size_t nHeaders = 3;
    mocks.ResetAllCalls();

    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend)); /*because DoWork for event*/

    STRICT_EXPECTED_CALL(mocks, get_time(NULL));
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc()); /*because responseHeadearsHandle: a new instance of HTTP headers*/
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, BUFFER_new());
    STRICT_EXPECTED_CALL(mocks, BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1); /*because relativePath is a STRING_HANDLE*/
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,
        IGNORED_PTR_ARG,                                    /*HTTPAPIEX_HANDLE handle,                                     */
        HTTPAPI_REQUEST_GET,                                /*HTTPAPI_REQUEST_TYPE requestType,                            */
        "/devices/" TEST_DEVICE_ID MESSAGE_ENDPOINT_HTTP API_VERSION,    /*const char* relativePath,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,                */
        NULL,                                               /*BUFFER_HANDLE requestContent,                                */
        IGNORED_PTR_ARG,                                    /*unsigned int* statusCode,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,               */
        IGNORED_PTR_ARG                                     /*BUFFER_HANDLE responseContent))                              */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(5)
        .IgnoreArgument(7)
        .IgnoreArgument(8)
        .IgnoreArgument(9)
        .CopyOutArgumentBuffer(7, &statusCode200, sizeof(statusCode200));

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_FindHeaderValue(IGNORED_PTR_ARG, "ETag"))
        .IgnoreArgument(1)
        .SetReturn(TEST_ETAG_VALUE);

    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_CreateFromByteArray(IGNORED_PTR_ARG, IGNORED_NUM_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .SetReturn(TEST_IOTHUB_MESSAGE_HANDLE_8);
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Destroy(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_u_char(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    {
        /*this scope is for is properties code*/

        HTTPHeaders_GetHeaderCount_writes_to_its_outputs = false;
        STRICT_EXPECTED_CALL(mocks, HTTPHeaders_GetHeaderCount(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .CopyOutArgumentBuffer(2, &nHeaders, sizeof(nHeaders));

        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Properties(TEST_IOTHUB_MESSAGE_HANDLE_8));

        STRICT_EXPECTED_CALL(mocks, HTTPHeaders_GetHeader(IGNORED_PTR_ARG, 0, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(3);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, Map_AddOrUpdate(TEST_MAP_3_PROPERTY, "NAME1", "VALUE1"));

        STRICT_EXPECTED_CALL(mocks, HTTPHeaders_GetHeader(IGNORED_PTR_ARG, 1, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(3);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        whenShallHTTPHeaders_GetHeader_fail = currentHTTPHeaders_GetHeader_call + 3;
        STRICT_EXPECTED_CALL(mocks, HTTPHeaders_GetHeader(IGNORED_PTR_ARG, 2, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(3);
    }

    /*this is "abandon"*/
    STRICT_EXPECTED_CALL(mocks, STRING_clone(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_construct_n(TEST_ETAG_VALUE_UNQUOTED, sizeof(TEST_ETAG_VALUE_UNQUOTED) - 1))
        .ValidateArgumentBuffer(1, TEST_ETAG_VALUE_UNQUOTED, sizeof(TEST_ETAG_VALUE_UNQUOTED) - 1);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, "/abandon" API_VERSION))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc());
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "User-Agent", CLIENT_DEVICE_TYPE_PREFIX CLIENT_DEVICE_BACKSLASH IOTHUB_SDK_VERSION))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "Authorization", TEST_BLANK_SAS_TOKEN))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "If-Match", TEST_ETAG_VALUE))
        .IgnoreArgument(1);


    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1); /*because abandon relativePath is a STRING_HANDLE*/
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,
        IGNORED_PTR_ARG,                                    /*HTTPAPIEX_HANDLE handle,                                     */
        HTTPAPI_REQUEST_POST,                               /*HTTPAPI_REQUEST_TYPE requestType,                            */
        "/devices/" TEST_DEVICE_ID MESSAGE_ENDPOINT_HTTP_ETAG TEST_ETAG_VALUE_UNQUOTED "/abandon" API_VERSION,    /*const char* relativePath,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,                */
        NULL,                                               /*BUFFER_HANDLE requestContent,                                */
        IGNORED_PTR_ARG,                                    /*unsigned int* statusCode,                                    */
        NULL,                                               /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,               */
        NULL                                                /*BUFFER_HANDLE responseContent))                              */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(5)
        .IgnoreArgument(7)
        .CopyOutArgumentBuffer(7, &statusCode204, sizeof(statusCode204));

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_090: [ All the HTTP headers of the form iothub-app-name:somecontent shall be transformed in message properties {name, somecontent}. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_092: [ If assembling the message fails in any way, then _DoWork shall "abandon" the message. ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_happy_path_with_empty_waitingToSend_and_1_service_message_with_2_property_when_properties_fails_it_abandons_3)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    unsigned int statusCode200 = 200;
    unsigned int statusCode204 = 204;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    (void)IoTHubTransportHttp_Subscribe(devHandle);
    size_t nHeaders = 3;
    mocks.ResetAllCalls();

    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend)); /*because DoWork for event*/

    STRICT_EXPECTED_CALL(mocks, get_time(NULL));
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc()); /*because responseHeadearsHandle: a new instance of HTTP headers*/
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, BUFFER_new());
    STRICT_EXPECTED_CALL(mocks, BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1); /*because relativePath is a STRING_HANDLE*/
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,
        IGNORED_PTR_ARG,                                    /*HTTPAPIEX_HANDLE handle,                                     */
        HTTPAPI_REQUEST_GET,                                /*HTTPAPI_REQUEST_TYPE requestType,                            */
        "/devices/" TEST_DEVICE_ID MESSAGE_ENDPOINT_HTTP API_VERSION,    /*const char* relativePath,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,                */
        NULL,                                               /*BUFFER_HANDLE requestContent,                                */
        IGNORED_PTR_ARG,                                    /*unsigned int* statusCode,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,               */
        IGNORED_PTR_ARG                                     /*BUFFER_HANDLE responseContent))                              */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(5)
        .IgnoreArgument(7)
        .IgnoreArgument(8)
        .IgnoreArgument(9)
        .CopyOutArgumentBuffer(7, &statusCode200, sizeof(statusCode200));

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_FindHeaderValue(IGNORED_PTR_ARG, "ETag"))
        .IgnoreArgument(1)
        .SetReturn(TEST_ETAG_VALUE);

    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_CreateFromByteArray(IGNORED_PTR_ARG, IGNORED_NUM_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .SetReturn(TEST_IOTHUB_MESSAGE_HANDLE_8);
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Destroy(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_u_char(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    {
        /*this scope is for is properties code*/

        HTTPHeaders_GetHeaderCount_writes_to_its_outputs = false;
        STRICT_EXPECTED_CALL(mocks, HTTPHeaders_GetHeaderCount(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .CopyOutArgumentBuffer(2, &nHeaders, sizeof(nHeaders));

        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Properties(TEST_IOTHUB_MESSAGE_HANDLE_8));

        STRICT_EXPECTED_CALL(mocks, HTTPHeaders_GetHeader(IGNORED_PTR_ARG, 0, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(3);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, Map_AddOrUpdate(TEST_MAP_3_PROPERTY, "NAME1", "VALUE1"));

        whenShallHTTPHeaders_GetHeader_fail = currentHTTPHeaders_GetHeader_call + 2;
        STRICT_EXPECTED_CALL(mocks, HTTPHeaders_GetHeader(IGNORED_PTR_ARG, 1, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(3);
    }

    /*this is "abandon"*/
    STRICT_EXPECTED_CALL(mocks, STRING_clone(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_construct_n(TEST_ETAG_VALUE_UNQUOTED, sizeof(TEST_ETAG_VALUE_UNQUOTED) - 1))
        .ValidateArgumentBuffer(1, TEST_ETAG_VALUE_UNQUOTED, sizeof(TEST_ETAG_VALUE_UNQUOTED) - 1);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, "/abandon" API_VERSION))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc());
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "User-Agent", CLIENT_DEVICE_TYPE_PREFIX CLIENT_DEVICE_BACKSLASH IOTHUB_SDK_VERSION))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "Authorization", TEST_BLANK_SAS_TOKEN))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "If-Match", TEST_ETAG_VALUE))
        .IgnoreArgument(1);


    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1); /*because abandon relativePath is a STRING_HANDLE*/
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,
        IGNORED_PTR_ARG,                                    /*HTTPAPIEX_HANDLE handle,                                     */
        HTTPAPI_REQUEST_POST,                               /*HTTPAPI_REQUEST_TYPE requestType,                            */
        "/devices/" TEST_DEVICE_ID MESSAGE_ENDPOINT_HTTP_ETAG TEST_ETAG_VALUE_UNQUOTED "/abandon" API_VERSION,    /*const char* relativePath,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,                */
        NULL,                                               /*BUFFER_HANDLE requestContent,                                */
        IGNORED_PTR_ARG,                                    /*unsigned int* statusCode,                                    */
        NULL,                                               /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,               */
        NULL                                                /*BUFFER_HANDLE responseContent))                              */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(5)
        .IgnoreArgument(7)
        .CopyOutArgumentBuffer(7, &statusCode204, sizeof(statusCode204));

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_090: [ All the HTTP headers of the form iothub-app-name:somecontent shall be transformed in message properties {name, somecontent}. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_092: [ If assembling the message fails in any way, then _DoWork shall "abandon" the message. ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_happy_path_with_empty_waitingToSend_and_1_service_message_with_2_property_when_properties_fails_it_abandons_4)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    unsigned int statusCode200 = 200;
    unsigned int statusCode204 = 204;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    (void)IoTHubTransportHttp_Subscribe(devHandle);
    size_t nHeaders = 3;
    mocks.ResetAllCalls();

    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend)); /*because DoWork for event*/

    STRICT_EXPECTED_CALL(mocks, get_time(NULL));
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc()); /*because responseHeadearsHandle: a new instance of HTTP headers*/
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, BUFFER_new());
    STRICT_EXPECTED_CALL(mocks, BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1); /*because relativePath is a STRING_HANDLE*/
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,
        IGNORED_PTR_ARG,                                    /*HTTPAPIEX_HANDLE handle,                                     */
        HTTPAPI_REQUEST_GET,                                /*HTTPAPI_REQUEST_TYPE requestType,                            */
        "/devices/" TEST_DEVICE_ID MESSAGE_ENDPOINT_HTTP API_VERSION,    /*const char* relativePath,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,                */
        NULL,                                               /*BUFFER_HANDLE requestContent,                                */
        IGNORED_PTR_ARG,                                    /*unsigned int* statusCode,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,               */
        IGNORED_PTR_ARG                                     /*BUFFER_HANDLE responseContent))                              */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(5)
        .IgnoreArgument(7)
        .IgnoreArgument(8)
        .IgnoreArgument(9)
        .CopyOutArgumentBuffer(7, &statusCode200, sizeof(statusCode200));

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_FindHeaderValue(IGNORED_PTR_ARG, "ETag"))
        .IgnoreArgument(1)
        .SetReturn(TEST_ETAG_VALUE);

    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_CreateFromByteArray(IGNORED_PTR_ARG, IGNORED_NUM_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .SetReturn(TEST_IOTHUB_MESSAGE_HANDLE_8);
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Destroy(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_u_char(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    {
        /*this scope is for is properties code*/

        HTTPHeaders_GetHeaderCount_writes_to_its_outputs = false;
        STRICT_EXPECTED_CALL(mocks, HTTPHeaders_GetHeaderCount(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .CopyOutArgumentBuffer(2, &nHeaders, sizeof(nHeaders));

        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Properties(TEST_IOTHUB_MESSAGE_HANDLE_8));

        STRICT_EXPECTED_CALL(mocks, HTTPHeaders_GetHeader(IGNORED_PTR_ARG, 0, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(3);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, Map_AddOrUpdate(TEST_MAP_3_PROPERTY, "NAME1", "VALUE1"))
            .SetReturn(MAP_ERROR);
    }

    /*this is "abandon"*/
    STRICT_EXPECTED_CALL(mocks, STRING_clone(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_construct_n(TEST_ETAG_VALUE_UNQUOTED, sizeof(TEST_ETAG_VALUE_UNQUOTED) - 1))
        .ValidateArgumentBuffer(1, TEST_ETAG_VALUE_UNQUOTED, sizeof(TEST_ETAG_VALUE_UNQUOTED) - 1);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, "/abandon" API_VERSION))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc());
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "User-Agent", CLIENT_DEVICE_TYPE_PREFIX CLIENT_DEVICE_BACKSLASH IOTHUB_SDK_VERSION))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "Authorization", TEST_BLANK_SAS_TOKEN))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "If-Match", TEST_ETAG_VALUE))
        .IgnoreArgument(1);


    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1); /*because abandon relativePath is a STRING_HANDLE*/
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,
        IGNORED_PTR_ARG,                                    /*HTTPAPIEX_HANDLE handle,                                     */
        HTTPAPI_REQUEST_POST,                               /*HTTPAPI_REQUEST_TYPE requestType,                            */
        "/devices/" TEST_DEVICE_ID MESSAGE_ENDPOINT_HTTP_ETAG TEST_ETAG_VALUE_UNQUOTED "/abandon" API_VERSION,    /*const char* relativePath,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,                */
        NULL,                                               /*BUFFER_HANDLE requestContent,                                */
        IGNORED_PTR_ARG,                                    /*unsigned int* statusCode,                                    */
        NULL,                                               /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,               */
        NULL                                                /*BUFFER_HANDLE responseContent))                              */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(5)
        .IgnoreArgument(7)
        .CopyOutArgumentBuffer(7, &statusCode204, sizeof(statusCode204));

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_090: [ All the HTTP headers of the form iothub-app-name:somecontent shall be transformed in message properties {name, somecontent}. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_092: [ If assembling the message fails in any way, then _DoWork shall "abandon" the message. ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_happy_path_with_empty_waitingToSend_and_1_service_message_with_2_property_when_properties_fails_it_abandons_5)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    unsigned int statusCode200 = 200;
    unsigned int statusCode204 = 204;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    (void)IoTHubTransportHttp_Subscribe(devHandle);
    size_t nHeaders = 3;
    mocks.ResetAllCalls();

    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend)); /*because DoWork for event*/

    STRICT_EXPECTED_CALL(mocks, get_time(NULL));
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc()); /*because responseHeadearsHandle: a new instance of HTTP headers*/
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, BUFFER_new());
    STRICT_EXPECTED_CALL(mocks, BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1); /*because relativePath is a STRING_HANDLE*/
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,
        IGNORED_PTR_ARG,                                    /*HTTPAPIEX_HANDLE handle,                                     */
        HTTPAPI_REQUEST_GET,                                /*HTTPAPI_REQUEST_TYPE requestType,                            */
        "/devices/" TEST_DEVICE_ID MESSAGE_ENDPOINT_HTTP API_VERSION,    /*const char* relativePath,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,                */
        NULL,                                               /*BUFFER_HANDLE requestContent,                                */
        IGNORED_PTR_ARG,                                    /*unsigned int* statusCode,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,               */
        IGNORED_PTR_ARG                                     /*BUFFER_HANDLE responseContent))                              */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(5)
        .IgnoreArgument(7)
        .IgnoreArgument(8)
        .IgnoreArgument(9)
        .CopyOutArgumentBuffer(7, &statusCode200, sizeof(statusCode200));

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_FindHeaderValue(IGNORED_PTR_ARG, "ETag"))
        .IgnoreArgument(1)
        .SetReturn(TEST_ETAG_VALUE);

    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_CreateFromByteArray(IGNORED_PTR_ARG, IGNORED_NUM_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .SetReturn(TEST_IOTHUB_MESSAGE_HANDLE_8);
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Destroy(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_u_char(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    {
        /*this scope is for is properties code*/

        HTTPHeaders_GetHeaderCount_writes_to_its_outputs = false;
        STRICT_EXPECTED_CALL(mocks, HTTPHeaders_GetHeaderCount(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .CopyOutArgumentBuffer(2, &nHeaders, sizeof(nHeaders))
            .SetReturn(HTTP_HEADERS_ERROR);
    }

    /*this is "abandon"*/
    STRICT_EXPECTED_CALL(mocks, STRING_clone(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_construct_n(TEST_ETAG_VALUE_UNQUOTED, sizeof(TEST_ETAG_VALUE_UNQUOTED) - 1))
        .ValidateArgumentBuffer(1, TEST_ETAG_VALUE_UNQUOTED, sizeof(TEST_ETAG_VALUE_UNQUOTED) - 1);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, "/abandon" API_VERSION))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc());
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "User-Agent", CLIENT_DEVICE_TYPE_PREFIX CLIENT_DEVICE_BACKSLASH IOTHUB_SDK_VERSION))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "Authorization", TEST_BLANK_SAS_TOKEN))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "If-Match", TEST_ETAG_VALUE))
        .IgnoreArgument(1);


    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1); /*because abandon relativePath is a STRING_HANDLE*/
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,                                    /*sasObject handle                                             */
        IGNORED_PTR_ARG,                                    /*HTTPAPIEX_HANDLE handle,                                     */
        HTTPAPI_REQUEST_POST,                               /*HTTPAPI_REQUEST_TYPE requestType,                            */
        "/devices/" TEST_DEVICE_ID MESSAGE_ENDPOINT_HTTP_ETAG TEST_ETAG_VALUE_UNQUOTED "/abandon" API_VERSION,    /*const char* relativePath,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,                */
        NULL,                                               /*BUFFER_HANDLE requestContent,                                */
        IGNORED_PTR_ARG,                                    /*unsigned int* statusCode,                                    */
        NULL,                                               /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,               */
        NULL                                                /*BUFFER_HANDLE responseContent))                              */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(5)
        .IgnoreArgument(7)
        .CopyOutArgumentBuffer(7, &statusCode204, sizeof(statusCode204));

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_071: [ If option SetBatching is false then _DoWork shall send individual event message as specced below. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_076: [ A clone of the event HTTP request headers shall be created. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_077: [ The cloned HTTP headers shall have the HTTP header "Content-Type" set to "application/octet-stream". ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_078: [ Every message property "property":"value" shall be added to the HTTP headers as an individual header "iothub-app-property":"value". ] /*well - this tests that no "phantom" properties are added when there are no properties to add*/
//Tests_SRS_TRANSPORTMULTITHTTP_17_080: [ IoTHubTransportHttp_DoWork shall call HTTPAPIEX_SAS_ExecuteRequest2 passing the following parameters ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_082: [ If HTTPAPIEX_SAS_ExecuteRequest2 does not fail and http status code < 300 then IoTHubTransportHttp_DoWork shall call IoTHubClient_LL_SendComplete. Parameter PDLIST_ENTRY completed shall point to a list the item send, and parameter IOTHUB_BATCHSTATE result shall be set to IOTHUB_CLIENT_CONFIRMATION_OK. The item shall be removed from waitingToSend. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_117: [ If optionName is an option handled by IoTHubTransportHttp then it shall be set. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_120: [ "Batching" ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_with_1_event_item_no_properties_unbatched_happy_path_succeeds)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    DList_InsertTailList(&(waitingToSend), &(message1.entry));
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    (void)IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    mocks.ResetAllCalls();

    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend));

    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetContentType(TEST_IOTHUB_MESSAGE_HANDLE_1));
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetByteArray(TEST_IOTHUB_MESSAGE_HANDLE_1, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(2)
        .IgnoreArgument(3);

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Clone(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Content-Type", "application/octet-stream"))
        .IgnoreArgument(1);

    /*no properties, so no more headers*/
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Properties(TEST_IOTHUB_MESSAGE_HANDLE_1));
    STRICT_EXPECTED_CALL(mocks, Map_GetInternals(TEST_MAP_EMPTY, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(2)
        .IgnoreArgument(3)
        .IgnoreArgument(4);

    STRICT_EXPECTED_CALL(mocks, BUFFER_new());
    STRICT_EXPECTED_CALL(mocks, BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_build(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_NUM_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(3);

    /*executing HTTP goodies*/
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG)) /*because relativePath*/
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,                                    /*sasObject handle                                             */
        IGNORED_PTR_ARG,
        HTTPAPI_REQUEST_POST,                                                           /*HTTPAPI_REQUEST_TYPE requestType,                  */
        "/devices/" TEST_DEVICE_ID EVENT_ENDPOINT API_VERSION,                 /*const char* relativePath,                          */
        IGNORED_PTR_ARG,                                                                /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,      */
        IGNORED_PTR_ARG,                                                                /*BUFFER_HANDLE requestContent,                      */
        IGNORED_PTR_ARG,                                                                /*unsigned int* statusCode,                          */
        NULL,                                                                           /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,     */
        NULL                                                                            /*BUFFER_HANDLE responseContent)                     */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(5)
        .IgnoreArgument(6)
        .CopyOutArgumentBuffer(7, &httpStatus200, sizeof(httpStatus200));

    /*once the event has been succesfull...*/

    /*building the list of messages to be notified if HTTP is fine*/
    STRICT_EXPECTED_CALL(mocks, DList_RemoveHeadList(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, DList_InsertTailList(IGNORED_PTR_ARG, &(message1.entry)))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_SendComplete(TEST_IOTHUB_CLIENT_LL_HANDLE, IGNORED_PTR_ARG, IOTHUB_CLIENT_CONFIRMATION_OK))
        .IgnoreArgument(2);

    EXPECTED_CALL(mocks, IoTHubMessage_GetMessageId(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, IoTHubMessage_GetCorrelationId(IGNORED_PTR_ARG));

    DISABLE_BATCHING();

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    ASSERT_ARE_EQUAL(int, 0, memcmp(BASEIMPLEMENTATION::BUFFER_u_char(last_BUFFER_HANDLE_to_HTTPAPIEX_ExecuteRequest), buffer1, buffer1_size));
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_071: [ If option SetBatching is false then _DoWork shall send individual event message as specced below. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_076: [ A clone of the event HTTP request headers shall be created. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_077: [ The cloned HTTP headers shall have the HTTP header "Content-Type" set to "application/octet-stream". ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_078: [ Every message property "property":"value" shall be added to the HTTP headers as an individual header "iothub-app-property":"value". ] /*well - this tests that no "phantom" properties are added when there are no properties to add*/
//Tests_SRS_TRANSPORTMULTITHTTP_17_080: [ IoTHubTransportHttp_DoWork shall call HTTPAPIEX_SAS_ExecuteRequest2 passing the following parameters ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_082: [ If HTTPAPIEX_SAS_ExecuteRequest2 does not fail and http status code < 300 then IoTHubTransportHttp_DoWork shall call IoTHubClient_LL_SendComplete. Parameter PDLIST_ENTRY completed shall point to a list the item send, and parameter IOTHUB_BATCHSTATE result shall be set to IOTHUB_CLIENT_CONFIRMATION_OK. The item shall be removed from waitingToSend. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_117: [ If optionName is an option handled by IoTHubTransportHttp then it shall be set. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_120: [ "Batching" ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_with_1_event_item_no_properties_string_type_unbatched_happy_path_succeeds)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    DList_InsertTailList(&(waitingToSend), &(message10.entry));
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    (void)IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    mocks.ResetAllCalls();

    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend));

    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetContentType(TEST_IOTHUB_MESSAGE_HANDLE_10));
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetString(TEST_IOTHUB_MESSAGE_HANDLE_10));

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Clone(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Content-Type", "application/octet-stream"))
        .IgnoreArgument(1);

    /*no properties, so no more headers*/
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Properties(TEST_IOTHUB_MESSAGE_HANDLE_10));
    STRICT_EXPECTED_CALL(mocks, Map_GetInternals(TEST_MAP_EMPTY, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(2)
        .IgnoreArgument(3)
        .IgnoreArgument(4);

    STRICT_EXPECTED_CALL(mocks, BUFFER_new());
    STRICT_EXPECTED_CALL(mocks, BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_build(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_NUM_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(3);

    /*executing HTTP goodies*/
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG)) /*because relativePath*/
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,                                    /*sasObject handle                                             */
        IGNORED_PTR_ARG,
        HTTPAPI_REQUEST_POST,                                                           /*HTTPAPI_REQUEST_TYPE requestType,                  */
        "/devices/" TEST_DEVICE_ID EVENT_ENDPOINT API_VERSION,                 /*const char* relativePath,                          */
        IGNORED_PTR_ARG,                                                                /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,      */
        IGNORED_PTR_ARG,                                                                /*BUFFER_HANDLE requestContent,                      */
        IGNORED_PTR_ARG,                                                                /*unsigned int* statusCode,                          */
        NULL,                                                                           /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,     */
        NULL                                                                            /*BUFFER_HANDLE responseContent)                     */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(5)
        .IgnoreArgument(6)
        .CopyOutArgumentBuffer(7, &httpStatus200, sizeof(httpStatus200));

    /*once the event has been succesfull...*/

    /*building the list of messages to be notified if HTTP is fine*/
    STRICT_EXPECTED_CALL(mocks, DList_RemoveHeadList(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, DList_InsertTailList(IGNORED_PTR_ARG, &(message10.entry)))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_SendComplete(TEST_IOTHUB_CLIENT_LL_HANDLE, IGNORED_PTR_ARG, IOTHUB_CLIENT_CONFIRMATION_OK))
        .IgnoreArgument(2);

    EXPECTED_CALL(mocks, IoTHubMessage_GetMessageId(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, IoTHubMessage_GetCorrelationId(IGNORED_PTR_ARG));

    DISABLE_BATCHING();

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    ASSERT_ARE_EQUAL(int, 0, memcmp(BASEIMPLEMENTATION::BUFFER_u_char(last_BUFFER_HANDLE_to_HTTPAPIEX_ExecuteRequest), string10, strlen(string10)));
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_072: [ The message size shall be limited to 255KB -1 bytes. ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_with_1_event_unbatched_happy_path_at_the_message_limit_succeeds)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    DList_InsertTailList(&(waitingToSend), &(message11.entry));
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    (void)IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    mocks.ResetAllCalls();

    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend));

    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetContentType(TEST_IOTHUB_MESSAGE_HANDLE_11));
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetByteArray(TEST_IOTHUB_MESSAGE_HANDLE_11, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(2)
        .IgnoreArgument(3);

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Clone(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Content-Type", "application/octet-stream"))
        .IgnoreArgument(1);

    /*1 property*/
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Properties(TEST_IOTHUB_MESSAGE_HANDLE_11));
    STRICT_EXPECTED_CALL(mocks, Map_GetInternals(TEST_MAP_1_PROPERTY_A_B, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(2)
        .IgnoreArgument(3)
        .IgnoreArgument(4);

    STRICT_EXPECTED_CALL(mocks, STRING_construct("iothub-app-"));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, TEST_KEYS1_A_B[0]))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "iothub-app-a", TEST_VALUES1_A_B[0]))
        .IgnoreArgument(1);


    STRICT_EXPECTED_CALL(mocks, BUFFER_new());
    STRICT_EXPECTED_CALL(mocks, BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_build(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_NUM_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(3);

    /*executing HTTP goodies*/
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG)) /*because relativePath*/
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,                                    /*sasObject handle                                             */
        IGNORED_PTR_ARG,
        HTTPAPI_REQUEST_POST,                                                           /*HTTPAPI_REQUEST_TYPE requestType,                  */
        "/devices/" TEST_DEVICE_ID EVENT_ENDPOINT API_VERSION,                 /*const char* relativePath,                          */
        IGNORED_PTR_ARG,                                                                /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,      */
        IGNORED_PTR_ARG,                                                                /*BUFFER_HANDLE requestContent,                      */
        IGNORED_PTR_ARG,                                                                /*unsigned int* statusCode,                          */
        NULL,                                                                           /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,     */
        NULL                                                                            /*BUFFER_HANDLE responseContent)                     */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(5)
        .IgnoreArgument(6)
        .CopyOutArgumentBuffer(7, &httpStatus200, sizeof(httpStatus200));

    /*once the event has been succesfull...*/

    /*building the list of messages to be notified if HTTP is fine*/
    STRICT_EXPECTED_CALL(mocks, DList_RemoveHeadList(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, DList_InsertTailList(IGNORED_PTR_ARG, &(message11.entry)))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_SendComplete(TEST_IOTHUB_CLIENT_LL_HANDLE, IGNORED_PTR_ARG, IOTHUB_CLIENT_CONFIRMATION_OK))
        .IgnoreArgument(2);

    EXPECTED_CALL(mocks, IoTHubMessage_GetMessageId(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, IoTHubMessage_GetCorrelationId(IGNORED_PTR_ARG));

    DISABLE_BATCHING();

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    ASSERT_ARE_EQUAL(int, 0, memcmp(BASEIMPLEMENTATION::BUFFER_u_char(last_BUFFER_HANDLE_to_HTTPAPIEX_ExecuteRequest), buffer11, buffer11_size));
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_072: [ The message size shall be limited to 255KB -1 bytes. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_073: [ The message size is computed from the length of the payload + 384. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_074: [ Every property name shall add to the message size the length of the property name + the length of the property value + 16 bytes. ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_with_1_event_item_no_properties_string_type_unbatched_happy_path_at_the_message_limit_succeeds)
{
    ///arrange
    CNiceCallComparer<CIoTHubTransportHttpMocks> mocks;
    DList_InsertTailList(&(waitingToSend), &(message12.entry));
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);

    (void)IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    mocks.ResetAllCalls();

    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_SendComplete(TEST_IOTHUB_CLIENT_LL_HANDLE, IGNORED_PTR_ARG, IOTHUB_CLIENT_CONFIRMATION_ERROR))
        .IgnoreArgument(2);

    DISABLE_BATCHING();

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_071: [ If option SetBatching is false then _DoWork shall send individual event message as specced below. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_076: [ A clone of the event HTTP request headers shall be created. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_077: [ The cloned HTTP headers shall have the HTTP header "Content-Type" set to "application/octet-stream". ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_078: [ Every message property "property":"value" shall be added to the HTTP headers as an individual header "iothub-app-property":"value". ] /*well - this tests that no "phantom" properties are added when there are no properties to add*/
//Tests_SRS_TRANSPORTMULTITHTTP_17_080: [ IoTHubTransportHttp_DoWork shall call HTTPAPIEX_SAS_ExecuteRequest2 passing the following parameters ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_082: [ If HTTPAPIEX_SAS_ExecuteRequest2 does not fail and http status code < 300 then IoTHubTransportHttp_DoWork shall call IoTHubClient_LL_SendComplete. Parameter PDLIST_ENTRY completed shall point to a list the item send, and parameter IOTHUB_BATCHSTATE result shall be set to IOTHUB_CLIENT_CONFIRMATION_OK. The item shall be removed from waitingToSend. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_117: [ If optionName is an option handled by IoTHubTransportHttp then it shall be set. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_120: [ "Batching" ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_with_1_event_item_no_properties_string_type_unbatched_fails_when_getString_fails)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    DList_InsertTailList(&(waitingToSend), &(message10.entry));
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    (void)IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    mocks.ResetAllCalls();

    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend));

    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetContentType(TEST_IOTHUB_MESSAGE_HANDLE_10));
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetString(TEST_IOTHUB_MESSAGE_HANDLE_10))
        .SetReturn((const char*)NULL);

    DISABLE_BATCHING();

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_071: [ If option SetBatching is false then _DoWork shall send individual event message as specced below. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_076: [ A clone of the event HTTP request headers shall be created. ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_077: [ The cloned HTTP headers shall have the HTTP header "Content-Type" set to "application/octet-stream". ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_078: [ Every message property "property":"value" shall be added to the HTTP headers as an individual header "iothub-app-property":"value". ] /*well - this tests that no "phantom" properties are added when there are no properties to add*/
//Tests_SRS_TRANSPORTMULTITHTTP_17_080: [ IoTHubTransportHttp_DoWork shall call HTTPAPIEX_SAS_ExecuteRequest2 passing the following parameters ]
//Tests_SRS_TRANSPORTMULTITHTTP_17_082: [ If HTTPAPIEX_SAS_ExecuteRequest2 does not fail and http status code < 300 then IoTHubTransportHttp_DoWork shall call IoTHubClient_LL_SendComplete. Parameter PDLIST_ENTRY completed shall point to a list the item send, and parameter IOTHUB_BATCHSTATE result shall be set to IOTHUB_CLIENT_CONFIRMATION_OK. The item shall be removed from waitingToSend. ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_with_1_event_item_1_property_unbatched_happy_path_succeeds)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    DList_InsertTailList(&(waitingToSend), &(message6.entry));
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    (void)IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    mocks.ResetAllCalls();

    setupDoWorkLoopOnceForOneDevice(mocks);


    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend));

    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetContentType(TEST_IOTHUB_MESSAGE_HANDLE_6));
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetByteArray(TEST_IOTHUB_MESSAGE_HANDLE_6, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(2)
        .IgnoreArgument(3);

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Clone(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Content-Type", "application/octet-stream"))
        .IgnoreArgument(1);

    /*no properties, so no more headers*/
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Properties(TEST_IOTHUB_MESSAGE_HANDLE_6));
    STRICT_EXPECTED_CALL(mocks, Map_GetInternals(TEST_MAP_1_PROPERTY, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(2)
        .IgnoreArgument(3)
        .IgnoreArgument(4);

    /*this is making http headers*/
    STRICT_EXPECTED_CALL(mocks, STRING_construct("iothub-app-"));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, TEST_RED_KEY))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "iothub-app-" TEST_RED_KEY, TEST_RED_VALUE))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, BUFFER_new());
    STRICT_EXPECTED_CALL(mocks, BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_build(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_NUM_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(3);

    /*executing HTTP goodies*/
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG)) /*because relativePath*/
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,                                    /*sasObject handle                                             */
        IGNORED_PTR_ARG,
        HTTPAPI_REQUEST_POST,                                                           /*HTTPAPI_REQUEST_TYPE requestType,                  */
        "/devices/" TEST_DEVICE_ID EVENT_ENDPOINT API_VERSION,                 /*const char* relativePath,                          */
        IGNORED_PTR_ARG,                                                                /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,      */
        IGNORED_PTR_ARG,                                                                /*BUFFER_HANDLE requestContent,                      */
        IGNORED_PTR_ARG,                                                                /*unsigned int* statusCode,                          */
        NULL,                                                                           /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,     */
        NULL                                                                            /*BUFFER_HANDLE responseContent)                     */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(5)
        .IgnoreArgument(6)
        .CopyOutArgumentBuffer(7, &httpStatus200, sizeof(httpStatus200));

    /*once the event has been succesfull...*/

    /*building the list of messages to be notified if HTTP is fine*/
    STRICT_EXPECTED_CALL(mocks, DList_RemoveHeadList(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, DList_InsertTailList(IGNORED_PTR_ARG, &(message6.entry)))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_SendComplete(TEST_IOTHUB_CLIENT_LL_HANDLE, IGNORED_PTR_ARG, IOTHUB_CLIENT_CONFIRMATION_OK))
        .IgnoreArgument(2);

    EXPECTED_CALL(mocks, IoTHubMessage_GetMessageId(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, IoTHubMessage_GetCorrelationId(IGNORED_PTR_ARG));

    DISABLE_BATCHING();

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    ASSERT_ARE_EQUAL(int, 0, memcmp(BASEIMPLEMENTATION::BUFFER_u_char(last_BUFFER_HANDLE_to_HTTPAPIEX_ExecuteRequest), buffer6, buffer6_size));
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}


//Tests_SRS_TRANSPORTMULTITHTTP_17_069: [ If HTTPAPIEX_SAS_ExecuteRequest2 fails or the http status code >=300 then IoTHubTransportHttp_DoWork shall not do any other action (it is assumed at the next _DoWork it shall be retried). ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_with_1_event_item_1_property_unbatched_does_nothing_when_httpStatusCode_is_not_succeess)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    DList_InsertTailList(&(waitingToSend), &(message6.entry));
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    (void)IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    mocks.ResetAllCalls();

    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend));

    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetContentType(TEST_IOTHUB_MESSAGE_HANDLE_6));
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetByteArray(TEST_IOTHUB_MESSAGE_HANDLE_6, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(2)
        .IgnoreArgument(3);

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Clone(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Content-Type", "application/octet-stream"))
        .IgnoreArgument(1);

    /*no properties, so no more headers*/
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Properties(TEST_IOTHUB_MESSAGE_HANDLE_6));
    STRICT_EXPECTED_CALL(mocks, Map_GetInternals(TEST_MAP_1_PROPERTY, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(2)
        .IgnoreArgument(3)
        .IgnoreArgument(4);

    /*this is making http headers*/
    STRICT_EXPECTED_CALL(mocks, STRING_construct("iothub-app-"));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, TEST_RED_KEY))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "iothub-app-" TEST_RED_KEY, TEST_RED_VALUE))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, BUFFER_new());
    STRICT_EXPECTED_CALL(mocks, BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_build(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_NUM_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(3);

    /*executing HTTP goodies*/
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG)) /*because relativePath*/
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,                                    /*sasObject handle                                             */
        IGNORED_PTR_ARG,
        HTTPAPI_REQUEST_POST,                                                           /*HTTPAPI_REQUEST_TYPE requestType,                  */
        "/devices/" TEST_DEVICE_ID EVENT_ENDPOINT API_VERSION,                 /*const char* relativePath,                          */
        IGNORED_PTR_ARG,                                                                /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,      */
        IGNORED_PTR_ARG,                                                                /*BUFFER_HANDLE requestContent,                      */
        IGNORED_PTR_ARG,                                                                /*unsigned int* statusCode,                          */
        NULL,                                                                           /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,     */
        NULL                                                                            /*BUFFER_HANDLE responseContent)                     */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(5)
        .IgnoreArgument(6)
        .CopyOutArgumentBuffer(7, &httpStatus404, sizeof(httpStatus404));

    EXPECTED_CALL(mocks, IoTHubMessage_GetMessageId(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, IoTHubMessage_GetCorrelationId(IGNORED_PTR_ARG));

    DISABLE_BATCHING();

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    ASSERT_ARE_EQUAL(int, 0, memcmp(BASEIMPLEMENTATION::BUFFER_u_char(last_BUFFER_HANDLE_to_HTTPAPIEX_ExecuteRequest), buffer6, buffer6_size));
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_081: [ If HTTPAPIEX_SAS_ExecuteRequest2 fails or the http status code >=300 then IoTHubTransportHttp_DoWork shall not do any other action (it is assumed at the next _DoWork it shall be retried). ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_with_1_event_item_1_property_unbatched_does_nothing_when_HTTPAPIEXSAS_fails)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    DList_InsertTailList(&(waitingToSend), &(message6.entry));
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    (void)IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    mocks.ResetAllCalls();

    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend));

    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetContentType(TEST_IOTHUB_MESSAGE_HANDLE_6));
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetByteArray(TEST_IOTHUB_MESSAGE_HANDLE_6, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(2)
        .IgnoreArgument(3);

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Clone(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Content-Type", "application/octet-stream"))
        .IgnoreArgument(1);

    /*no properties, so no more headers*/
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Properties(TEST_IOTHUB_MESSAGE_HANDLE_6));
    STRICT_EXPECTED_CALL(mocks, Map_GetInternals(TEST_MAP_1_PROPERTY, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(2)
        .IgnoreArgument(3)
        .IgnoreArgument(4);

    /*this is making http headers*/
    STRICT_EXPECTED_CALL(mocks, STRING_construct("iothub-app-"));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, TEST_RED_KEY))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "iothub-app-" TEST_RED_KEY, TEST_RED_VALUE))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, BUFFER_new());
    STRICT_EXPECTED_CALL(mocks, BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_build(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_NUM_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(3);

    /*executing HTTP goodies*/
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG)) /*because relativePath*/
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,                                    /*sasObject handle                                             */
        IGNORED_PTR_ARG,
        HTTPAPI_REQUEST_POST,                                                           /*HTTPAPI_REQUEST_TYPE requestType,                  */
        "/devices/" TEST_DEVICE_ID EVENT_ENDPOINT API_VERSION,                 /*const char* relativePath,                          */
        IGNORED_PTR_ARG,                                                                /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,      */
        IGNORED_PTR_ARG,                                                                /*BUFFER_HANDLE requestContent,                      */
        IGNORED_PTR_ARG,                                                                /*unsigned int* statusCode,                          */
        NULL,                                                                           /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,     */
        NULL                                                                            /*BUFFER_HANDLE responseContent)                     */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(5)
        .IgnoreArgument(6)
        .CopyOutArgumentBuffer(7, &httpStatus200, sizeof(httpStatus200))
        .SetReturn(HTTPAPIEX_ERROR);

    EXPECTED_CALL(mocks, IoTHubMessage_GetMessageId(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, IoTHubMessage_GetCorrelationId(IGNORED_PTR_ARG));

    DISABLE_BATCHING();

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    ASSERT_ARE_EQUAL(int, 0, memcmp(BASEIMPLEMENTATION::BUFFER_u_char(last_BUFFER_HANDLE_to_HTTPAPIEX_ExecuteRequest), buffer6, buffer6_size));
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_079: [ If any HTTP header operation fails, _DoWork shall advance to the next action. ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_with_1_event_item_1_property_unbatched_does_nothing_when_buffer_fails_1)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    DList_InsertTailList(&(waitingToSend), &(message6.entry));
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    (void)IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    mocks.ResetAllCalls();
    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend));

    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetContentType(TEST_IOTHUB_MESSAGE_HANDLE_6));
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetByteArray(TEST_IOTHUB_MESSAGE_HANDLE_6, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(2)
        .IgnoreArgument(3);

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Clone(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Content-Type", "application/octet-stream"))
        .IgnoreArgument(1);

    /*no properties, so no more headers*/
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Properties(TEST_IOTHUB_MESSAGE_HANDLE_6));
    STRICT_EXPECTED_CALL(mocks, Map_GetInternals(TEST_MAP_1_PROPERTY, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(2)
        .IgnoreArgument(3)
        .IgnoreArgument(4);

    /*this is making http headers*/
    STRICT_EXPECTED_CALL(mocks, STRING_construct("iothub-app-"));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, TEST_RED_KEY))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "iothub-app-" TEST_RED_KEY, TEST_RED_VALUE))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, BUFFER_new());
    STRICT_EXPECTED_CALL(mocks, BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, BUFFER_build(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_NUM_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(3)
        .SetReturn(1);

    EXPECTED_CALL(mocks, IoTHubMessage_GetMessageId(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, IoTHubMessage_GetCorrelationId(IGNORED_PTR_ARG));

    DISABLE_BATCHING();

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_079: [ If any HTTP header operation fails, _DoWork shall advance to the next action. ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_with_1_event_item_1_property_unbatched_does_nothing_when_buffer_fails_2)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    DList_InsertTailList(&(waitingToSend), &(message6.entry));
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    (void)IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    mocks.ResetAllCalls();

    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend));

    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetContentType(TEST_IOTHUB_MESSAGE_HANDLE_6));
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetByteArray(TEST_IOTHUB_MESSAGE_HANDLE_6, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(2)
        .IgnoreArgument(3);

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Clone(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Content-Type", "application/octet-stream"))
        .IgnoreArgument(1);

    /*no properties, so no more headers*/
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Properties(TEST_IOTHUB_MESSAGE_HANDLE_6));
    STRICT_EXPECTED_CALL(mocks, Map_GetInternals(TEST_MAP_1_PROPERTY, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(2)
        .IgnoreArgument(3)
        .IgnoreArgument(4);

    /*this is making http headers*/
    STRICT_EXPECTED_CALL(mocks, STRING_construct("iothub-app-"));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, TEST_RED_KEY))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "iothub-app-" TEST_RED_KEY, TEST_RED_VALUE))
        .IgnoreArgument(1);

    whenShallBUFFER_new_fail = currentBUFFER_new_call + 1;
    STRICT_EXPECTED_CALL(mocks, BUFFER_new());

    EXPECTED_CALL(mocks, IoTHubMessage_GetMessageId(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, IoTHubMessage_GetCorrelationId(IGNORED_PTR_ARG));

    DISABLE_BATCHING();

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_079: [ If any HTTP header operation fails, _DoWork shall advance to the next action. ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_with_1_event_item_1_property_unbatched_does_nothing_when_http_headers_fail_1)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    DList_InsertTailList(&(waitingToSend), &(message6.entry));
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    (void)IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    mocks.ResetAllCalls();

    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend));

    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetContentType(TEST_IOTHUB_MESSAGE_HANDLE_6));
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetByteArray(TEST_IOTHUB_MESSAGE_HANDLE_6, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(2)
        .IgnoreArgument(3);

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Clone(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Content-Type", "application/octet-stream"))
        .IgnoreArgument(1);

    /*no properties, so no more headers*/
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Properties(TEST_IOTHUB_MESSAGE_HANDLE_6));
    STRICT_EXPECTED_CALL(mocks, Map_GetInternals(TEST_MAP_1_PROPERTY, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(2)
        .IgnoreArgument(3)
        .IgnoreArgument(4);

    /*this is making http headers*/
    STRICT_EXPECTED_CALL(mocks, STRING_construct("iothub-app-"));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, TEST_RED_KEY))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "iothub-app-" TEST_RED_KEY, TEST_RED_VALUE))
        .IgnoreArgument(1)
        .SetReturn(HTTP_HEADERS_ERROR);

    EXPECTED_CALL(mocks, IoTHubMessage_GetMessageId(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, IoTHubMessage_GetCorrelationId(IGNORED_PTR_ARG));

    DISABLE_BATCHING();

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_079: [ If any HTTP header operation fails, _DoWork shall advance to the next action. ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_with_1_event_item_1_property_unbatched_does_nothing_when_http_headers_fail_2)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    DList_InsertTailList(&(waitingToSend), &(message6.entry));
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    (void)IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    mocks.ResetAllCalls();

    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend));

    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetContentType(TEST_IOTHUB_MESSAGE_HANDLE_6));
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetByteArray(TEST_IOTHUB_MESSAGE_HANDLE_6, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(2)
        .IgnoreArgument(3);

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Clone(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Content-Type", "application/octet-stream"))
        .IgnoreArgument(1);

    /*no properties, so no more headers*/
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Properties(TEST_IOTHUB_MESSAGE_HANDLE_6));
    STRICT_EXPECTED_CALL(mocks, Map_GetInternals(TEST_MAP_1_PROPERTY, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(2)
        .IgnoreArgument(3)
        .IgnoreArgument(4);

    /*this is making http headers*/
    STRICT_EXPECTED_CALL(mocks, STRING_construct("iothub-app-"));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, TEST_RED_KEY))
        .IgnoreArgument(1)
        .SetReturn(1111); /*unpredictable value which is an error*/

    EXPECTED_CALL(mocks, IoTHubMessage_GetMessageId(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, IoTHubMessage_GetCorrelationId(IGNORED_PTR_ARG));

    DISABLE_BATCHING();

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_079: [ If any HTTP header operation fails, _DoWork shall advance to the next action. ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_with_1_event_item_1_property_unbatched_does_nothing_when_http_headers_fail_3)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    DList_InsertTailList(&(waitingToSend), &(message6.entry));
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    (void)IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    mocks.ResetAllCalls();

    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend));

    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetContentType(TEST_IOTHUB_MESSAGE_HANDLE_6));
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetByteArray(TEST_IOTHUB_MESSAGE_HANDLE_6, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(2)
        .IgnoreArgument(3);

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Clone(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Content-Type", "application/octet-stream"))
        .IgnoreArgument(1);

    /*no properties, so no more headers*/
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Properties(TEST_IOTHUB_MESSAGE_HANDLE_6));
    STRICT_EXPECTED_CALL(mocks, Map_GetInternals(TEST_MAP_1_PROPERTY, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(2)
        .IgnoreArgument(3)
        .IgnoreArgument(4);

    /*this is making http headers*/
    whenShallSTRING_construct_fail = currentSTRING_construct_call + 1;
    STRICT_EXPECTED_CALL(mocks, STRING_construct("iothub-app-"));

    EXPECTED_CALL(mocks, IoTHubMessage_GetMessageId(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, IoTHubMessage_GetCorrelationId(IGNORED_PTR_ARG));

    DISABLE_BATCHING();

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_079: [ If any HTTP header operation fails, _DoWork shall advance to the next action. ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_with_1_event_item_1_property_unbatched_does_nothing_when_map_fails)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    DList_InsertTailList(&(waitingToSend), &(message6.entry));
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    (void)IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    mocks.ResetAllCalls();

    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend));

    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetContentType(TEST_IOTHUB_MESSAGE_HANDLE_6));
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetByteArray(TEST_IOTHUB_MESSAGE_HANDLE_6, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(2)
        .IgnoreArgument(3);

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Clone(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Content-Type", "application/octet-stream"))
        .IgnoreArgument(1);

    /*no properties, so no more headers*/
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Properties(TEST_IOTHUB_MESSAGE_HANDLE_6));
    STRICT_EXPECTED_CALL(mocks, Map_GetInternals(TEST_MAP_1_PROPERTY, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(2)
        .IgnoreArgument(3)
        .IgnoreArgument(4)
        .SetReturn(MAP_ERROR);

    DISABLE_BATCHING();

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_079: [ If any HTTP header operation fails, _DoWork shall advance to the next action. ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_with_1_event_item_1_property_unbatched_does_nothing_when_http_fails_4)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    DList_InsertTailList(&(waitingToSend), &(message6.entry));
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    (void)IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    mocks.ResetAllCalls();

    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend));

    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetContentType(TEST_IOTHUB_MESSAGE_HANDLE_6));
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetByteArray(TEST_IOTHUB_MESSAGE_HANDLE_6, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(2)
        .IgnoreArgument(3);

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Clone(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Content-Type", "application/octet-stream"))
        .IgnoreArgument(1)
        .SetReturn(HTTP_HEADERS_ERROR);

    DISABLE_BATCHING();

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_079: [ If any HTTP header operation fails, _DoWork shall advance to the next action. ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_with_1_event_item_1_property_unbatched_does_nothing_when_http_fails_5)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    DList_InsertTailList(&(waitingToSend), &(message6.entry));
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    (void)IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    mocks.ResetAllCalls();

    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend));

    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetContentType(TEST_IOTHUB_MESSAGE_HANDLE_6));
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetByteArray(TEST_IOTHUB_MESSAGE_HANDLE_6, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(2)
        .IgnoreArgument(3);

    whenShallHTTPHeaders_Clone_fail = currentHTTPHeaders_Clone_call + 1;
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Clone(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    DISABLE_BATCHING();

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_079: [ If any HTTP header operation fails, _DoWork shall advance to the next action. ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_with_1_event_item_1_property_unbatched_does_nothing_when_IoTHubMessage_GetByteArray_fails)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    DList_InsertTailList(&(waitingToSend), &(message6.entry));
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    (void)IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    mocks.ResetAllCalls();

    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend));

    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetContentType(TEST_IOTHUB_MESSAGE_HANDLE_6));
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetByteArray(TEST_IOTHUB_MESSAGE_HANDLE_6, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(2)
        .IgnoreArgument(3)
        .SetReturn(IOTHUB_MESSAGE_ERROR);

    DISABLE_BATCHING();

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_075: [ If the oldest message in waitingToSend causes the message to exceed the message size limit then it shall be removed from waitingToSend, and IoTHubClient_LL_SendComplete shall be called. Parameter PDLIST_ENTRY completed shall point to a list containing only the oldest item, and parameter IOTHUB_BATCHSTATE result shall be set to IOTHUB_CLIENT_CONFIRMATION_ERROR. ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_with_1_event_item_1_property_unbatched_overlimit_calls_SendComplete_with_BATCHSTATE_FAILED)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    DList_InsertTailList(&(waitingToSend), &(message9.entry));
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    (void)IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    mocks.ResetAllCalls();

    setupDoWorkLoopOnceForOneDevice(mocks);


    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend));

    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetContentType(TEST_IOTHUB_MESSAGE_HANDLE_9));
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetByteArray(TEST_IOTHUB_MESSAGE_HANDLE_9, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(2)
        .IgnoreArgument(3);

    /*ooops - over 256K*/

    /*building the list of messages to be notified if HTTP is fine*/
    STRICT_EXPECTED_CALL(mocks, DList_RemoveHeadList(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, DList_InsertTailList(IGNORED_PTR_ARG, &(message9.entry)))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_SendComplete(TEST_IOTHUB_CLIENT_LL_HANDLE, IGNORED_PTR_ARG, IOTHUB_CLIENT_CONFIRMATION_ERROR))
        .IgnoreArgument(2);

    DISABLE_BATCHING();

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_057: [ If a messages to be send has type IOTHUBMESSAGE_STRING, then its serialization shall be {"body":"JSON encoding of the string", "base64Encoded":false} ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_with_1_event_item_as_string_happy_path_succeeds)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    DList_InsertTailList(&(waitingToSend), &(message10.entry));
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    (void)IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    mocks.ResetAllCalls();

    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend));

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Content-Type", "application/vnd.microsoft.iothub.json"))
        .IgnoreArgument(1);

    /*starting to prepare the "big" payload*/
    STRICT_EXPECTED_CALL(mocks, STRING_construct("["));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    /*this is first batched payload*/
    {
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetContentType(message10.messageHandle));
        STRICT_EXPECTED_CALL(mocks, STRING_construct("{\"body\":"));
        STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetString(message10.messageHandle));

        STRICT_EXPECTED_CALL(mocks, STRING_new_JSON(string10));
        STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2);

        STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, ",\"base64Encoded\":false")) /*closing the value of the body*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Properties(message10.messageHandle));
        STRICT_EXPECTED_CALL(mocks, Map_GetInternals(TEST_MAP_EMPTY, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(2)
            .IgnoreArgument(3)
            .IgnoreArgument(4);
        STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, "},")) /* this extra "," is going to be harshly overwritten by a "]"*/
            .IgnoreArgument(1);
        /*end of the first batched payload*/
    }

    {
        /*adding the first payload to the "big" payload*/
        STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2);

        /*closing the "big" payload*/
        STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_length(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
    }

    /*building the list of messages to be notified if HTTP is fine*/
    STRICT_EXPECTED_CALL(mocks, DList_RemoveHeadList(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, DList_InsertTailList(IGNORED_PTR_ARG, &(message10.entry)))
        .IgnoreArgument(1);

    {
        /*this is building the HTTP payload... from a STRING_HANDLE (as it comes as "big payload"), into an array of bytes*/
        STRICT_EXPECTED_CALL(mocks, BUFFER_new());
        STRICT_EXPECTED_CALL(mocks, BUFFER_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_length(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, BUFFER_build(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_NUM_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2)
            .IgnoreArgument(3);
    }

    /*executing HTTP goodies*/
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG)) /*because relativePath*/
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,                                    /*sasObject handle                                             */
        IGNORED_PTR_ARG,
        HTTPAPI_REQUEST_POST,                                                           /*HTTPAPI_REQUEST_TYPE requestType,                  */
        "/devices/" TEST_DEVICE_ID EVENT_ENDPOINT API_VERSION,                 /*const char* relativePath,                          */
        IGNORED_PTR_ARG,                                                                /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,      */
        IGNORED_PTR_ARG,                                                                /*BUFFER_HANDLE requestContent,                      */
        IGNORED_PTR_ARG,                                                                /*unsigned int* statusCode,                          */
        NULL,                                                                           /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,     */
        NULL                                                                            /*BUFFER_HANDLE responseContent)                     */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(5)
        .IgnoreArgument(6)
        .CopyOutArgumentBuffer(7, &httpStatus200, sizeof(httpStatus200));

    /*once the event has been succesfull...*/

    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_SendComplete(TEST_IOTHUB_CLIENT_LL_HANDLE, IGNORED_PTR_ARG, IOTHUB_CLIENT_CONFIRMATION_OK))
        .IgnoreArgument(2);

    ENABLE_BATCHING();

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_057: [ If a messages to be send has type IOTHUBMESSAGE_STRING, then its serialization shall be {"body":"JSON encoding of the string", "base64Encoded":false} ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_with_1_event_item_as_string_when_string_concat_fails_it_fails)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    DList_InsertTailList(&(waitingToSend), &(message10.entry));
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    (void)IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    mocks.ResetAllCalls();

    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend));

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Content-Type", "application/vnd.microsoft.iothub.json"))
        .IgnoreArgument(1);

    /*starting to prepare the "big" payload*/
    STRICT_EXPECTED_CALL(mocks, STRING_construct("["));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    /*this is first batched payload*/
    {
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetContentType(message10.messageHandle));
        STRICT_EXPECTED_CALL(mocks, STRING_construct("{\"body\":"));
        STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetString(message10.messageHandle));

        STRICT_EXPECTED_CALL(mocks, STRING_new_JSON(string10));
        STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2);

        STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, ",\"base64Encoded\":false")) /*closing the value of the body*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Properties(message10.messageHandle));
        STRICT_EXPECTED_CALL(mocks, Map_GetInternals(TEST_MAP_EMPTY, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(2)
            .IgnoreArgument(3)
            .IgnoreArgument(4);
        whenShallSTRING_concat_fail = currentSTRING_concat_call + 2;
        STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, "},")) /* this extra "," is going to be harshly overwritten by a "]"*/
            .IgnoreArgument(1);
        /*end of the first batched payload*/
    }

    ENABLE_BATCHING();

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_057: [ If a messages to be send has type IOTHUBMESSAGE_STRING, then its serialization shall be {"body":"JSON encoding of the string", "base64Encoded":false} ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_with_1_event_item_as_string_when_Map_GetInternals_fails_it_fails)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    DList_InsertTailList(&(waitingToSend), &(message10.entry));
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    (void)IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    mocks.ResetAllCalls();

    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend));

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Content-Type", "application/vnd.microsoft.iothub.json"))
        .IgnoreArgument(1);

    /*starting to prepare the "big" payload*/
    STRICT_EXPECTED_CALL(mocks, STRING_construct("["));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    /*this is first batched payload*/
    {
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetContentType(message10.messageHandle));
        STRICT_EXPECTED_CALL(mocks, STRING_construct("{\"body\":"));
        STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetString(message10.messageHandle));

        STRICT_EXPECTED_CALL(mocks, STRING_new_JSON(string10));
        STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2);

        STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, ",\"base64Encoded\":false")) /*closing the value of the body*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Properties(message10.messageHandle));
        STRICT_EXPECTED_CALL(mocks, Map_GetInternals(TEST_MAP_EMPTY, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(2)
            .IgnoreArgument(3)
            .IgnoreArgument(4)
            .SetReturn(MAP_ERROR);
        /*end of the first batched payload*/
    }

    ENABLE_BATCHING();

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_057: [ If a messages to be send has type IOTHUBMESSAGE_STRING, then its serialization shall be {"body":"JSON encoding of the string", "base64Encoded":false} ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_with_1_event_item_as_string_when_STRING_concat_fails_it_fails_2)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    DList_InsertTailList(&(waitingToSend), &(message10.entry));
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    (void)IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    mocks.ResetAllCalls();

    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend));

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Content-Type", "application/vnd.microsoft.iothub.json"))
        .IgnoreArgument(1);

    /*starting to prepare the "big" payload*/
    STRICT_EXPECTED_CALL(mocks, STRING_construct("["));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    /*this is first batched payload*/
    {
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetContentType(message10.messageHandle));
        STRICT_EXPECTED_CALL(mocks, STRING_construct("{\"body\":"));
        STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetString(message10.messageHandle));

        STRICT_EXPECTED_CALL(mocks, STRING_new_JSON(string10));
        STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2);

        whenShallSTRING_concat_fail = currentSTRING_concat_call + 1;
        STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, ",\"base64Encoded\":false")) /*closing the value of the body*/
            .IgnoreArgument(1);
        /*end of the first batched payload*/
    }

    ENABLE_BATCHING();

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_057: [ If a messages to be send has type IOTHUBMESSAGE_STRING, then its serialization shall be {"body":"JSON encoding of the string", "base64Encoded":false} ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_with_1_event_item_as_string_when_STRING_concat_with_STRING_fails_it_fails)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    DList_InsertTailList(&(waitingToSend), &(message10.entry));
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    (void)IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    mocks.ResetAllCalls();

    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend));

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Content-Type", "application/vnd.microsoft.iothub.json"))
        .IgnoreArgument(1);

    /*starting to prepare the "big" payload*/
    STRICT_EXPECTED_CALL(mocks, STRING_construct("["));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    /*this is first batched payload*/
    {
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetContentType(message10.messageHandle));
        STRICT_EXPECTED_CALL(mocks, STRING_construct("{\"body\":"));
        STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetString(message10.messageHandle));

        STRICT_EXPECTED_CALL(mocks, STRING_new_JSON(string10));
        STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        whenShallSTRING_concat_with_STRING_fail = currentSTRING_concat_with_STRING_call + 1;
        STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2);
        /*end of the first batched payload*/
    }

    ENABLE_BATCHING();

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_057: [ If a messages to be send has type IOTHUBMESSAGE_STRING, then its serialization shall be {"body":"JSON encoding of the string", "base64Encoded":false} ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_with_1_event_item_as_string_when_STRING_new_JSON_fails_it_fails)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    DList_InsertTailList(&(waitingToSend), &(message10.entry));
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    (void)IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    mocks.ResetAllCalls();

    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend));

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Content-Type", "application/vnd.microsoft.iothub.json"))
        .IgnoreArgument(1);

    /*starting to prepare the "big" payload*/
    STRICT_EXPECTED_CALL(mocks, STRING_construct("["));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    /*this is first batched payload*/
    {
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetContentType(message10.messageHandle));
        STRICT_EXPECTED_CALL(mocks, STRING_construct("{\"body\":"));
        STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetString(message10.messageHandle));

        whenShallSTRING_new_JSON_fail = currentSTRING_new_JSON_call + 1;
        STRICT_EXPECTED_CALL(mocks, STRING_new_JSON(string10));

        /*end of the first batched payload*/
    }

    ENABLE_BATCHING();

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_057: [ If a messages to be send has type IOTHUBMESSAGE_STRING, then its serialization shall be {"body":"JSON encoding of the string", "base64Encoded":false} ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_with_1_event_item_as_string_when_IoTHubMessage_GetString_it_fails)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    DList_InsertTailList(&(waitingToSend), &(message10.entry));
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    (void)IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    mocks.ResetAllCalls();

    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend));

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Content-Type", "application/vnd.microsoft.iothub.json"))
        .IgnoreArgument(1);

    /*starting to prepare the "big" payload*/
    STRICT_EXPECTED_CALL(mocks, STRING_construct("["));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    /*this is first batched payload*/
    {
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetContentType(message10.messageHandle));
        STRICT_EXPECTED_CALL(mocks, STRING_construct("{\"body\":"));
        STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetString(message10.messageHandle))
            .SetReturn((const char*)NULL);

        /*end of the first batched payload*/
    }

    ENABLE_BATCHING();

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_057: [ If a messages to be send has type IOTHUBMESSAGE_STRING, then its serialization shall be {"body":"JSON encoding of the string", "base64Encoded":false} ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_with_1_event_item_as_string_when_STRING_construct_it_fails)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    DList_InsertTailList(&(waitingToSend), &(message10.entry));
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    (void)IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    mocks.ResetAllCalls();

    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend));

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Content-Type", "application/vnd.microsoft.iothub.json"))
        .IgnoreArgument(1);

    /*starting to prepare the "big" payload*/
    STRICT_EXPECTED_CALL(mocks, STRING_construct("["));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    /*this is first batched payload*/
    {
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetContentType(message10.messageHandle));
        whenShallSTRING_construct_fail = currentSTRING_construct_call + 2;
        STRICT_EXPECTED_CALL(mocks, STRING_construct("{\"body\":"));

        /*end of the first batched payload*/
    }

    ENABLE_BATCHING();

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_091: [ The HTTP header value of iothub-messageid shall be set in the IoTHub_SetMessageId. ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_SetMessageId_SUCCEED)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    size_t nHeaders = 4;
    unsigned int statusCode200 = 200;
    unsigned int statusCode204 = 204;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    (void)IoTHubTransportHttp_Subscribe(devHandle);
    mocks.ResetAllCalls();
    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend)); /*because DoWork for event*/

    STRICT_EXPECTED_CALL(mocks, get_time(NULL));
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc()); /*because responseHeadearsHandle: a new instance of HTTP headers*/
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, BUFFER_new());
    STRICT_EXPECTED_CALL(mocks, BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1); /*because relativePath is a STRING_HANDLE*/
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,                                    /*sasObject handle                                             */
        IGNORED_PTR_ARG,                                    /*HTTPAPIEX_HANDLE handle,                                     */
        HTTPAPI_REQUEST_GET,                                /*HTTPAPI_REQUEST_TYPE requestType,                            */
        "/devices/" TEST_DEVICE_ID MESSAGE_ENDPOINT_HTTP API_VERSION,    /*const char* relativePath,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,                */
        NULL,                                               /*BUFFER_HANDLE requestContent,                                */
        IGNORED_PTR_ARG,                                    /*unsigned int* statusCode,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,               */
        IGNORED_PTR_ARG                                     /*BUFFER_HANDLE responseContent))                              */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(5)
        .IgnoreArgument(7)
        .IgnoreArgument(8)
        .IgnoreArgument(9)
        .CopyOutArgumentBuffer(7, &statusCode200, sizeof(statusCode200));

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_FindHeaderValue(IGNORED_PTR_ARG, "ETag"))
        .IgnoreArgument(1)
        .SetReturn(TEST_ETAG_VALUE);

    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_CreateFromByteArray(IGNORED_PTR_ARG, IGNORED_NUM_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .SetReturn(TEST_IOTHUB_MESSAGE_HANDLE_8);
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Destroy(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_u_char(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    {
        /*this scope is for is properties code*/

        HTTPHeaders_GetHeaderCount_writes_to_its_outputs = false;
        STRICT_EXPECTED_CALL(mocks, HTTPHeaders_GetHeaderCount(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .CopyOutArgumentBuffer(2, &nHeaders, sizeof(nHeaders));

        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Properties(TEST_IOTHUB_MESSAGE_HANDLE_8));

        STRICT_EXPECTED_CALL(mocks, HTTPHeaders_GetHeader(IGNORED_PTR_ARG, 0, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(3);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, Map_AddOrUpdate(TEST_MAP_3_PROPERTY, "NAME1", "VALUE1"));

        STRICT_EXPECTED_CALL(mocks, HTTPHeaders_GetHeader(IGNORED_PTR_ARG, 1, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(3);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, HTTPHeaders_GetHeader(IGNORED_PTR_ARG, 2, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(3);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, Map_AddOrUpdate(TEST_MAP_3_PROPERTY, "NAME2", "VALUE2"));

        STRICT_EXPECTED_CALL(mocks, HTTPHeaders_GetHeader(IGNORED_PTR_ARG, 3, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(3);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_SetMessageId(IGNORED_PTR_ARG, "VALUE3"))
            .IgnoreArgument(1);
    }

    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_MessageCallback(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);

    /*this returns "0" so the message needs to be "accepted"*/
    /*this is "accepting"*/
    STRICT_EXPECTED_CALL(mocks, STRING_clone(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_construct_n(TEST_ETAG_VALUE_UNQUOTED, sizeof(TEST_ETAG_VALUE_UNQUOTED) - 1))
        .ValidateArgumentBuffer(1, TEST_ETAG_VALUE_UNQUOTED, sizeof(TEST_ETAG_VALUE_UNQUOTED) - 1);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, API_VERSION))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc());
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "User-Agent", CLIENT_DEVICE_TYPE_PREFIX CLIENT_DEVICE_BACKSLASH IOTHUB_SDK_VERSION))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "Authorization", TEST_BLANK_SAS_TOKEN))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "If-Match", TEST_ETAG_VALUE))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1); /*because abandon relativePath is a STRING_HANDLE*/
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,                                    /*sasObject handle                                             */
        IGNORED_PTR_ARG,                                    /*HTTPAPIEX_HANDLE handle,                                     */
        HTTPAPI_REQUEST_DELETE,                                /*HTTPAPI_REQUEST_TYPE requestType,                            */
        "/devices/" TEST_DEVICE_ID MESSAGE_ENDPOINT_HTTP_ETAG TEST_ETAG_VALUE_UNQUOTED API_VERSION,    /*const char* relativePath,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,                */
        NULL,                                               /*BUFFER_HANDLE requestContent,                                */
        IGNORED_PTR_ARG,                                    /*unsigned int* statusCode,                                    */
        NULL,                                               /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,               */
        NULL                                                /*BUFFER_HANDLE responseContent))                              */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(5)
        .IgnoreArgument(7)
        .CopyOutArgumentBuffer(7, &statusCode204, sizeof(statusCode204));

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_091: [ The HTTP header value of iothub-messageid shall be set in the IoTHub_SetMessageId. ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_SetMessageId_FAILED)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    size_t nHeaders = 4;
    unsigned int statusCode200 = 200;
    unsigned int statusCode204 = 204;
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    auto devHandle = IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    (void)IoTHubTransportHttp_Subscribe(devHandle);
    mocks.ResetAllCalls();
    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend)); /*because DoWork for event*/

    STRICT_EXPECTED_CALL(mocks, get_time(NULL));
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc()); /*because responseHeadearsHandle: a new instance of HTTP headers*/
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, BUFFER_new());
    STRICT_EXPECTED_CALL(mocks, BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1); /*because relativePath is a STRING_HANDLE*/
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,                                    /*sasObject handle                                             */
        IGNORED_PTR_ARG,                                    /*HTTPAPIEX_HANDLE handle,                                     */
        HTTPAPI_REQUEST_GET,                                /*HTTPAPI_REQUEST_TYPE requestType,                            */
        "/devices/" TEST_DEVICE_ID MESSAGE_ENDPOINT_HTTP API_VERSION,    /*const char* relativePath,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,                */
        NULL,                                               /*BUFFER_HANDLE requestContent,                                */
        IGNORED_PTR_ARG,                                    /*unsigned int* statusCode,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,               */
        IGNORED_PTR_ARG                                     /*BUFFER_HANDLE responseContent))                              */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(5)
        .IgnoreArgument(7)
        .IgnoreArgument(8)
        .IgnoreArgument(9)
        .CopyOutArgumentBuffer(7, &statusCode200, sizeof(statusCode200));

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_FindHeaderValue(IGNORED_PTR_ARG, "ETag"))
        .IgnoreArgument(1)
        .SetReturn(TEST_ETAG_VALUE);

    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_CreateFromByteArray(IGNORED_PTR_ARG, IGNORED_NUM_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .SetReturn(TEST_IOTHUB_MESSAGE_HANDLE_8);
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Destroy(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_u_char(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    {
        /*this scope is for is properties code*/

        HTTPHeaders_GetHeaderCount_writes_to_its_outputs = false;
        STRICT_EXPECTED_CALL(mocks, HTTPHeaders_GetHeaderCount(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .CopyOutArgumentBuffer(2, &nHeaders, sizeof(nHeaders));

        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Properties(TEST_IOTHUB_MESSAGE_HANDLE_8));

        STRICT_EXPECTED_CALL(mocks, HTTPHeaders_GetHeader(IGNORED_PTR_ARG, 0, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(3);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, Map_AddOrUpdate(TEST_MAP_3_PROPERTY, "NAME1", "VALUE1"));

        STRICT_EXPECTED_CALL(mocks, HTTPHeaders_GetHeader(IGNORED_PTR_ARG, 1, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(3);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, HTTPHeaders_GetHeader(IGNORED_PTR_ARG, 2, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(3);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, Map_AddOrUpdate(TEST_MAP_3_PROPERTY, "NAME2", "VALUE2"));

        STRICT_EXPECTED_CALL(mocks, HTTPHeaders_GetHeader(IGNORED_PTR_ARG, 3, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(3);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_SetMessageId(IGNORED_PTR_ARG, "VALUE3"))
            .IgnoreArgument(1)
            .SetReturn(IOTHUB_MESSAGE_ERROR);
    }

    /*this is "abandon"*/
    STRICT_EXPECTED_CALL(mocks, STRING_clone(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_construct_n(TEST_ETAG_VALUE_UNQUOTED, sizeof(TEST_ETAG_VALUE_UNQUOTED) - 1))
        .ValidateArgumentBuffer(1, TEST_ETAG_VALUE_UNQUOTED, sizeof(TEST_ETAG_VALUE_UNQUOTED) - 1);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, "/abandon" API_VERSION))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Alloc());
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "User-Agent", CLIENT_DEVICE_TYPE_PREFIX CLIENT_DEVICE_BACKSLASH IOTHUB_SDK_VERSION))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "Authorization", TEST_BLANK_SAS_TOKEN))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, "If-Match", TEST_ETAG_VALUE))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1); /*because abandon relativePath is a STRING_HANDLE*/
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,
        IGNORED_PTR_ARG,                                    /*HTTPAPIEX_HANDLE handle,                                     */
        HTTPAPI_REQUEST_POST,                               /*HTTPAPI_REQUEST_TYPE requestType,                            */
        "/devices/" TEST_DEVICE_ID MESSAGE_ENDPOINT_HTTP_ETAG TEST_ETAG_VALUE_UNQUOTED "/abandon" API_VERSION,    /*const char* relativePath,                                    */
        IGNORED_PTR_ARG,                                    /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,                */
        NULL,                                               /*BUFFER_HANDLE requestContent,                                */
        IGNORED_PTR_ARG,                                    /*unsigned int* statusCode,                                    */
        NULL,                                               /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,               */
        NULL                                                /*BUFFER_HANDLE responseContent))                              */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(5)
        .IgnoreArgument(7)
        .CopyOutArgumentBuffer(7, &statusCode204, sizeof(statusCode204));

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

//Tests_SRS_TRANSPORTMULTITHTTP_17_091: [ The HTTP header value of iothub-messageid shall be set in the IoTHub_SetMessageId. ]
TEST_FUNCTION(IoTHubTransportHttp_DoWork_GetMessageId_succeeds)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    DList_InsertTailList(&(waitingToSend), &(message6.entry));
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    (void)IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    mocks.ResetAllCalls();

    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend));

    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetContentType(TEST_IOTHUB_MESSAGE_HANDLE_6));
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetByteArray(TEST_IOTHUB_MESSAGE_HANDLE_6, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(2)
        .IgnoreArgument(3);

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Clone(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Content-Type", "application/octet-stream"))
        .IgnoreArgument(1);

    /*no properties, so no more headers*/
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Properties(TEST_IOTHUB_MESSAGE_HANDLE_6));
    STRICT_EXPECTED_CALL(mocks, Map_GetInternals(TEST_MAP_1_PROPERTY, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(2)
        .IgnoreArgument(3)
        .IgnoreArgument(4);

    /*this is making http headers*/
    STRICT_EXPECTED_CALL(mocks, STRING_construct("iothub-app-"));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, TEST_RED_KEY))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "iothub-app-" TEST_RED_KEY, TEST_RED_VALUE))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, BUFFER_new());
    STRICT_EXPECTED_CALL(mocks, BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_build(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_NUM_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(3);

    /*executing HTTP goodies*/
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG)) /*because relativePath*/
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,                                    /*sasObject handle                                             */
        IGNORED_PTR_ARG,
        HTTPAPI_REQUEST_POST,                                                           /*HTTPAPI_REQUEST_TYPE requestType,                  */
        "/devices/" TEST_DEVICE_ID EVENT_ENDPOINT API_VERSION,                 /*const char* relativePath,                          */
        IGNORED_PTR_ARG,                                                                /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,      */
        IGNORED_PTR_ARG,                                                                /*BUFFER_HANDLE requestContent,                      */
        IGNORED_PTR_ARG,                                                                /*unsigned int* statusCode,                          */
        NULL,                                                                           /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,     */
        NULL                                                                            /*BUFFER_HANDLE responseContent)                     */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(5)
        .IgnoreArgument(6)
        .CopyOutArgumentBuffer(7, &httpStatus200, sizeof(httpStatus200));

    /*once the event has been succesfull...*/

    /*building the list of messages to be notified if HTTP is fine*/
    STRICT_EXPECTED_CALL(mocks, DList_RemoveHeadList(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, DList_InsertTailList(IGNORED_PTR_ARG, &(message6.entry)))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_SendComplete(TEST_IOTHUB_CLIENT_LL_HANDLE, IGNORED_PTR_ARG, IOTHUB_CLIENT_CONFIRMATION_OK))
        .IgnoreArgument(2);

    EXPECTED_CALL(mocks, IoTHubMessage_GetMessageId(IGNORED_PTR_ARG)).SetReturn(TEST_MESSAGE_ID);
    EXPECTED_CALL(mocks, IoTHubMessage_GetCorrelationId(IGNORED_PTR_ARG));

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "iothub-messageid", TEST_MESSAGE_ID))
        .IgnoreArgument(1);

    DISABLE_BATCHING();

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    ASSERT_ARE_EQUAL(int, 0, memcmp(BASEIMPLEMENTATION::BUFFER_u_char(last_BUFFER_HANDLE_to_HTTPAPIEX_ExecuteRequest), buffer6, buffer6_size));
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

TEST_FUNCTION(IoTHubTransportHttp_DoWork_GetCorrelationId_succeeds)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    DList_InsertTailList(&(waitingToSend), &(message6.entry));
    auto handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    (void)IoTHubTransportHttp_Register(handle, &TEST_DEVICE_1, TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_CONFIG.waitingToSend);

    mocks.ResetAllCalls();

    setupDoWorkLoopOnceForOneDevice(mocks);

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(&waitingToSend));

    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetContentType(TEST_IOTHUB_MESSAGE_HANDLE_6));
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetByteArray(TEST_IOTHUB_MESSAGE_HANDLE_6, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(2)
        .IgnoreArgument(3);

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Clone(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "Content-Type", "application/octet-stream"))
        .IgnoreArgument(1);

    /*no properties, so no more headers*/
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Properties(TEST_IOTHUB_MESSAGE_HANDLE_6));
    STRICT_EXPECTED_CALL(mocks, Map_GetInternals(TEST_MAP_1_PROPERTY, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(2)
        .IgnoreArgument(3)
        .IgnoreArgument(4);

    /*this is making http headers*/
    STRICT_EXPECTED_CALL(mocks, STRING_construct("iothub-app-"));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_concat(IGNORED_PTR_ARG, TEST_RED_KEY))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "iothub-app-" TEST_RED_KEY, TEST_RED_VALUE))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, BUFFER_new());
    STRICT_EXPECTED_CALL(mocks, BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, BUFFER_build(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_NUM_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(3);

    /*executing HTTP goodies*/
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG)) /*because relativePath*/
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_SAS_ExecuteRequest2(
        IGNORED_PTR_ARG,                                    /*sasObject handle                                             */
        IGNORED_PTR_ARG,
        HTTPAPI_REQUEST_POST,                                                           /*HTTPAPI_REQUEST_TYPE requestType,                  */
        "/devices/" TEST_DEVICE_ID EVENT_ENDPOINT API_VERSION,                 /*const char* relativePath,                          */
        IGNORED_PTR_ARG,                                                                /*HTTP_HEADERS_HANDLE requestHttpHeadersHandle,      */
        IGNORED_PTR_ARG,                                                                /*BUFFER_HANDLE requestContent,                      */
        IGNORED_PTR_ARG,                                                                /*unsigned int* statusCode,                          */
        NULL,                                                                           /*HTTP_HEADERS_HANDLE responseHttpHeadersHandle,     */
        NULL                                                                            /*BUFFER_HANDLE responseContent)                     */
        ))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .IgnoreArgument(5)
        .IgnoreArgument(6)
        .CopyOutArgumentBuffer(7, &httpStatus200, sizeof(httpStatus200));

    /*once the event has been succesfull...*/

    /*building the list of messages to be notified if HTTP is fine*/
    STRICT_EXPECTED_CALL(mocks, DList_RemoveHeadList(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, DList_InsertTailList(IGNORED_PTR_ARG, &(message6.entry)))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_SendComplete(TEST_IOTHUB_CLIENT_LL_HANDLE, IGNORED_PTR_ARG, IOTHUB_CLIENT_CONFIRMATION_OK))
        .IgnoreArgument(2);

    EXPECTED_CALL(mocks, IoTHubMessage_GetMessageId(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, IoTHubMessage_GetCorrelationId(IGNORED_PTR_ARG)).SetReturn(TEST_MESSAGE_ID);

    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(IGNORED_PTR_ARG, "iothub-correlationid", TEST_MESSAGE_ID))
        .IgnoreArgument(1);

    DISABLE_BATCHING();

    ///act
    IoTHubTransportHttp_DoWork(handle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ///assert
    ASSERT_ARE_EQUAL(int, 0, memcmp(BASEIMPLEMENTATION::BUFFER_u_char(last_BUFFER_HANDLE_to_HTTPAPIEX_ExecuteRequest), buffer6, buffer6_size));
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

/*Tests_SRS_TRANSPORTMULTITHTTP_02_001: [ If handle is NULL then IoTHubTransportHttp_GetHostname shall fail and return NULL. ]*/
TEST_FUNCTION(IoTHubTransportHttp_GetHostname_with_NULL_handle_fails)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;

    ///act
    STRING_HANDLE hostname = IoTHubTransportHttp_GetHostname(NULL);

    ///assert
    ASSERT_IS_NULL(hostname);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
}

/*Tests_SRS_TRANSPORTMULTITHTTP_02_001: [ If handle is NULL then IoTHubTransportHttp_GetHostname shall fail and return NULL. ]*/
TEST_FUNCTION(IoTHubTransportHttp_GetHostname_with_non_NULL_handle_succeeds)
{
    ///arrange
    CIoTHubTransportHttpMocks mocks;
    TRANSPORT_LL_HANDLE handle = IoTHubTransportHttp_Create(&TEST_CONFIG);
    mocks.ResetAllCalls();

    ///act
    STRING_HANDLE hostname = IoTHubTransportHttp_GetHostname(handle);

    ///assert
    ASSERT_IS_NOT_NULL(hostname);
    mocks.AssertActualAndExpectedCalls();
    ASSERT_ARE_EQUAL(char_ptr, TEST_IOTHUB_NAME "." TEST_IOTHUB_SUFFIX, STRING_c_str(hostname));

    ///cleanup
    IoTHubTransportHttp_Destroy(handle);
}

END_TEST_SUITE(iothubtransporthttp)

