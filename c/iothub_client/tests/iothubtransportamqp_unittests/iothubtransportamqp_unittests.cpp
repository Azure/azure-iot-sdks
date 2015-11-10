// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <cstdlib>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include <csignal>

#include "testrunnerswitcher.h"
#include "micromock.h"
#include "micromockcharstararenullterminatedstrings.h"

static MICROMOCK_MUTEX_HANDLE g_testByTest;
static MICROMOCK_GLOBAL_SEMAPHORE_HANDLE g_dllByDll;


#include "iothub_message.h"
#include "proton/message.h"
#include "proton/messenger.h"

#include "buffer_.h"
#include "lock.h"
#include "doublylinkedlist.h"
#include "strings.h"
#include "urlencode.h"
#include "crt_abstractions.h"
#include "sastoken.h"
#include "iothubtransportamqp.h"
#include "iothub_client_private.h"
#include "iothub_client_amqp_internal.h"

DEFINE_MICROMOCK_ENUM_TO_STRING(IOTHUB_CLIENT_STATUS, IOTHUB_CLIENT_STATUS_VALUES);

#ifndef SIZE_MAX
#define SIZE_MAX ((size_t)~(size_t)0)
#endif

#define GBALLOC_H

extern "C" int gballoc_init(void);
extern "C" void gballoc_deinit(void);
extern "C" void* gballoc_malloc(size_t size);
extern "C" void* gballoc_calloc(size_t nmemb, size_t size);
extern "C" void* gballoc_realloc(void* ptr, size_t size);
extern "C" void gballoc_free(void* ptr);

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
#include "buffer.c"
};

DEFINE_MICROMOCK_ENUM_TO_STRING(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_RESULT_VALUES);
DEFINE_MICROMOCK_ENUM_TO_STRING(IOTHUB_BATCHSTATE_RESULT, IOTHUB_BATCHSTATE_RESULT_VALUES);

#define TEST_HAPPY_PATH SIZE_MAX
#define TEST_DEVICE_ID "deviceId"
#define TEST_DEVICE_KEY "deviceKey"
#define TEST_IOTHUB_NAME "iotHubName"
#define TEST_IOTHUB_SUFFIX "iotHubSuffix"
#define TEST_HOST TEST_IOTHUB_NAME "." TEST_IOTHUB_SUFFIX
#define TEST_SCHEME_AND_HOST AMQPS_SCHEME TEST_HOST
#define TEST_SAS_TOKEN "A SAS_TOKEN"
#define TEST_CBS_FULL_PATH TEST_SCHEME_AND_HOST CBS_ENDPOINT
#define TEST_MESSAGE_FULL_PATH TEST_SCHEME_AND_HOST MESSAGE_ENDPOINT "/" TEST_DEVICE_ID

static const IOTHUB_CLIENT_CONFIG TEST_IOTHUBCLIENT_CONFIG =
{
    AMQP_Protocol,  /* IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol;   */
    TEST_DEVICE_ID,                                 /* const char* deviceId;                        */
    TEST_DEVICE_KEY,                                /* const char* deviceKey;                       */
    TEST_IOTHUB_NAME,                               /* const char* iotHubName;                      */
    TEST_IOTHUB_SUFFIX                              /* const char* iotHubSuffix;                    */
};

#define TEST_PN_MESSAGE (pn_message_t*)0x42
#define TEST_NULL_PN_MESSAGE ((pn_message_t*)NULL)
#define TEST_PN_DATA (pn_data_t*)0x43
#define TEST_NULL_PN_DATA (pn_data_t*)NULL
#define TEST_PN_MESSENGER (pn_messenger_t*)0x44
#define TEST_NULL_PN_MESSENGER ((pn_messenger_t*)NULL)
#define TEST_IOTHUB_MESSAGE_HANDLE (IOTHUB_MESSAGE_HANDLE)0x45
#define TEST_NULL_IOTHUB_MESSAGE_HANDLE (IOTHUB_MESSAGE_HANDLE)NULL
#define TEST_PN_SUBSCRIPTION (pn_subscription_t*)0x46
#define TEST_NULL_PN_SUBSCRIPTION ((pn_subscription_t*)NULL)
#define TEST_PN_TRACKER (pn_tracker_t)0x4c
#define TEST_PN_GOOD_RESULT (int)0x0
#define TEST_PDLIST_ENTRY (PDLIST_ENTRY)0x22
#define TEST_STRING_HANDLE (STRING_HANDLE)0x46
#define TEST_NULL_STRING_HANDLE (STRING_HANDLE)NULL
#define TEST_ENCODED_STRING_HANDLE (STRING_HANDLE)0x47
#define TEST_BUFER_HANDLE (BUFFER_HANDLE) 0x48
#define TEST_IOTHUB_CLIENT_LL_HANDLE (IOTHUB_CLIENT_LL_HANDLE)0x49
#define TEST_MESSAGE_HANDLE_OVERIDE_DEFAULTS (IOTHUB_MESSAGE_HANDLE)0x424243
#define TEST_MESSAGE_HANDLE_STRING (IOTHUB_MESSAGE_HANDLE)0x44D4E1
#define TEST_PN_WORK_MILLISECONDS PROTON_PROCESSING_YIELD_IN_MILLISECONDS
#define TEST_NUMBER_OF_STOPPEDS PROTON_MESSENGER_STOP_TRIES

#define TEST_INITIAL_TIME_VALUE (0)
#define TEST_EXPIRY_TEST_VALUE ((size_t)TEST_INITIAL_TIME_VALUE + SAS_TOKEN_DEFAULT_LIFETIME)

static STRING_HANDLE STRING_HANDLE_NULL = (STRING_HANDLE)NULL;


#define TEST_MAP_EMPTY                  (MAP_HANDLE)0xe0
#define TEST_MAP_1_PROPERTY             (MAP_HANDLE)0xe1
#define TEST_MAP_2_PROPERTY             (MAP_HANDLE)0xe2
#define TEST_MAP_3_PROPERTY             (MAP_HANDLE)0xe3
#define TEST_IOTHUB_MESSAGE_HANDLE_1    ((IOTHUB_MESSAGE_HANDLE)0x01d1)
#define TEST_IOTHUB_MESSAGE_HANDLE_2    ((IOTHUB_MESSAGE_HANDLE)0x01d2)
#define TEST_IOTHUB_MESSAGE_HANDLE_3    ((IOTHUB_MESSAGE_HANDLE)0x01d3)
#define TEST_IOTHUB_MESSAGE_HANDLE_4    ((IOTHUB_MESSAGE_HANDLE)0x01d4)
#define TEST_IOTHUB_MESSAGE_HANDLE_5    ((IOTHUB_MESSAGE_HANDLE)0x01d5)
#define TEST_IOTHUB_MESSAGE_HANDLE_6    ((IOTHUB_MESSAGE_HANDLE)0x01d6)
#define TEST_IOTHUB_MESSAGE_HANDLE_7    ((IOTHUB_MESSAGE_HANDLE)0x01d7)
#define TEST_IOTHUB_MESSAGE_HANDLE_8    ((IOTHUB_MESSAGE_HANDLE)0x01d8)

static const char* TEST_KEYS1[] = { "Test_Key1" };
static const char* TEST_VALUES1[] = { "Test_Value1" };

static const char* TEST_KEYS2[] = { "Test_Key2_A", "Test_Key2_B" };
static const char* TEST_VALUES2[] = { "Test_Value_2_A",  "Test_Value_2_B" };

static pn_bytes_t TEST_PN_GOOD_BYTES = { 1, NULL };
static pn_bytes_t TEST_PN_STRING_BYTES = { 12, "STRING_BYTES" };

static const char* TEXT_MESSAGE = "Hello From amqptransport Unit Tests";
static const char* TEST_CHAR = "TestChar";

static const char* TEST_MESSAGE_ID = "FC365F0A-D432-4AB9-8307-59C0EB8F0447";

/*different STRING constructors*/
static size_t currentSTRING_new_call;
static size_t whenShallSTRING_new_fail;

static size_t currentSTRING_new_quoted_call;
static size_t whenShallSTRING_new_quoted_fail;

static size_t currentBase64_Encode_call;
static size_t whenShallBase64_Encode_fail;

static size_t currentSTRING_clone_call;
static size_t whenShallSTRING_clone_fail;

static size_t currentSTRING_construct_call;
static size_t whenShallSTRING_construct_fail;

static size_t currentSTRING_concat_call;
static size_t whenShallSTRING_concat_fail;

static size_t currentSTRING_concat_with_STRING_call;
static size_t whenShallSTRING_concat_with_STRING_fail;

static size_t currentCorrelationId;

static size_t currentmalloc_call;
static size_t whenShallmalloc_fail;

static size_t currentlockinit_call;
static size_t whenShalllockinit_fail;

static size_t currentlock_call;
static size_t whenShalllock_fail;

static size_t currentBUFFER_new_call;
static size_t whenShallBUFFER_new_fail;

static size_t currentBUFFER_build_call;
static size_t whenShallBUFFER_build_fail;

static size_t currentBUFFER_content_call;
static size_t whenShallBUFFER_content_fail;

static size_t currentBUFFER_size_call;
static size_t whenShallBUFFER_size_fail;

static size_t currentpn_messenger_status_call;
static size_t whenShallpn_messenger_status_return_alternate;
static pn_status_t alternate_pn_status;

static pn_bytes_t test_pn_bytes =
{
    sizeof(TEXT_MESSAGE),
    TEXT_MESSAGE
};

static pn_bytes_t test_pn_bytes_correct_status_name =
{
    sizeof(PROTON_MAP_STATUS_CODE),
    PROTON_MAP_STATUS_CODE
};
static pn_bytes_t test_pn_bytes_incorrect_status_name =
{
    sizeof(AMQPS_SCHEME),
    AMQPS_SCHEME
};

static pn_bytes_t msgid_bytes = { sizeof(TEST_MESSAGE_ID), TEST_MESSAGE_ID };

static pn_atom_t test_pn_atom = { PN_ULONG,(bool)1 };
static pn_atom_t test_messageId_atom;
static pn_atom_t test_correlationId_atom;
static pn_atom_t test_NULL_atom = { PN_NULL, 0 };

std::ostream& operator<<(std::ostream& left, const pn_atom_t& pnAtom)
{
    (pnAtom);
    left << "output pnAtom to stream";
    return left;
}

static bool operator==(const pn_atom_t left, const pn_atom_t& right)
{
    return left.u.as_ulong == right.u.as_ulong;
}

std::ostream& operator<<(std::ostream& left, const pn_bytes_t& pnBytes)
{
    std::ios::fmtflags f(left.flags());
    left << std::hex;
    for (size_t i = 0; i < pnBytes.size; i++)
    {
        left << pnBytes.start[i];
    }
    left.flags(f);
    return left;
}

static bool operator==(const pn_bytes_t left, const pn_bytes_t& right)
{
    if (left.size != right.size)
    {
        return false;
    }
    else
    {
        return memcmp(left.start, right.start, left.size) == 0;
    }
}

// **  Mocks **
TYPED_MOCK_CLASS(CIoTHubTransportAMQPMocks, CGlobalMock)
{
public:
    /* Proton Mocks */
    MOCK_STATIC_METHOD_0(, pn_message_t*, pn_message)
    MOCK_METHOD_END(pn_message_t*, TEST_PN_MESSAGE)

    MOCK_STATIC_METHOD_2(, int, pn_message_set_address, pn_message_t*, msg, const char*, address)
    MOCK_METHOD_END(int, TEST_PN_GOOD_RESULT)

    MOCK_STATIC_METHOD_2(, int, pn_message_set_inferred, pn_message_t*, msg, bool, inferred)
    MOCK_METHOD_END(int, TEST_PN_GOOD_RESULT)

    MOCK_STATIC_METHOD_1(, pn_data_t*, pn_message_body, pn_message_t*, msg)
    MOCK_METHOD_END(pn_data_t*, TEST_PN_DATA)

    MOCK_STATIC_METHOD_2(, int, pn_data_put_binary, pn_data_t*, data, pn_bytes_t, bytes)
    MOCK_METHOD_END(int, TEST_PN_GOOD_RESULT)

    MOCK_STATIC_METHOD_1(, pn_messenger_t*, pn_messenger, const char*, name)
    MOCK_METHOD_END(pn_messenger_t*, TEST_PN_MESSENGER)

    MOCK_STATIC_METHOD_1(, int, pn_messenger_start, pn_messenger_t*, messsenger)
    MOCK_METHOD_END(int, TEST_PN_GOOD_RESULT)

    MOCK_STATIC_METHOD_2(, int, pn_messenger_put, pn_messenger_t*, messenger, pn_message_t*, msg)
    MOCK_METHOD_END(int, TEST_PN_GOOD_RESULT)

    MOCK_STATIC_METHOD_2(, int, pn_messenger_send, pn_messenger_t*, messenger, int, n)
    MOCK_METHOD_END(int, TEST_PN_GOOD_RESULT)

    MOCK_STATIC_METHOD_1(, int, pn_messenger_stop, pn_messenger_t*, messenger)
    MOCK_METHOD_END(int, TEST_PN_GOOD_RESULT)

    MOCK_STATIC_METHOD_1(, void, pn_messenger_free, pn_messenger_t*, messenger)
    MOCK_VOID_METHOD_END()

    MOCK_STATIC_METHOD_1(, void, pn_message_free, pn_message_t*, msg)
    MOCK_VOID_METHOD_END()

    MOCK_STATIC_METHOD_1(, void, pn_message_clear, pn_message_t*, msg)
    MOCK_VOID_METHOD_END()

    MOCK_STATIC_METHOD_2(, pn_bytes_t, pn_bytes, size_t, size, const char*, start)
    MOCK_METHOD_END(pn_bytes_t, test_pn_bytes)

    MOCK_STATIC_METHOD_2(, int, pn_messenger_set_blocking, pn_messenger_t*, messenger, bool, blocking)
    MOCK_METHOD_END(int, TEST_PN_GOOD_RESULT)

    MOCK_STATIC_METHOD_2(, int, pn_messenger_set_timeout, pn_messenger_t*, messenger, int, timeout)
    MOCK_METHOD_END(int, TEST_PN_GOOD_RESULT)

    MOCK_STATIC_METHOD_1(, bool, pn_messenger_stopped, pn_messenger_t*, messenger)
    MOCK_METHOD_END(bool, true)

    MOCK_STATIC_METHOD_2(, int, pn_messenger_set_outgoing_window, pn_messenger_t*, messenger, int, window)
    MOCK_METHOD_END(int, TEST_PN_GOOD_RESULT)

    MOCK_STATIC_METHOD_2(, int, pn_messenger_set_trusted_certificates, pn_messenger_t*, messenger, const char*, cert_db)
    MOCK_METHOD_END(int, TEST_PN_GOOD_RESULT)

    MOCK_STATIC_METHOD_2(, int, pn_messenger_set_incoming_window, pn_messenger_t*, messenger, int, window)
    MOCK_METHOD_END(int, TEST_PN_GOOD_RESULT)

    MOCK_STATIC_METHOD_2(, pn_status_t, pn_messenger_status, pn_messenger_t*, messenger, pn_tracker_t, tracker)
        pn_status_t result2;
        currentpn_messenger_status_call++;
        if (currentpn_messenger_status_call == whenShallpn_messenger_status_return_alternate)
        {
            result2 = alternate_pn_status;
        }
        else
        {
            result2 = PN_STATUS_ACCEPTED;
        }
    MOCK_METHOD_END(pn_status_t, result2)

    MOCK_STATIC_METHOD_3(, int, pn_messenger_settle, pn_messenger_t*, messenger, pn_tracker_t, tracker, int, flags)
    MOCK_METHOD_END(int, TEST_PN_GOOD_RESULT)

    MOCK_STATIC_METHOD_1(, pn_tracker_t, pn_messenger_outgoing_tracker, pn_messenger_t*, messenger)
    MOCK_METHOD_END(pn_tracker_t, TEST_PN_TRACKER)

    MOCK_STATIC_METHOD_2(, int, pn_messenger_set_snd_settle_mode, pn_messenger_t*, messenger, pn_snd_settle_mode_t, mode)
    MOCK_METHOD_END(int, TEST_PN_GOOD_RESULT)

    MOCK_STATIC_METHOD_2(, int, pn_messenger_set_rcv_settle_mode, pn_messenger_t*, messenger, pn_rcv_settle_mode_t, mode)
    MOCK_METHOD_END(int, TEST_PN_GOOD_RESULT)

    MOCK_STATIC_METHOD_2(, int, pn_messenger_work, pn_messenger_t*, messenger, int, timeout)
    {
    }
    MOCK_METHOD_END(int, TEST_PN_GOOD_RESULT)

    MOCK_STATIC_METHOD_2(, pn_subscription_t*, pn_messenger_subscribe, pn_messenger_t*, messenger, const char*, source)
    MOCK_METHOD_END(pn_subscription_t*, TEST_PN_SUBSCRIPTION)

    MOCK_STATIC_METHOD_1(, pn_subscription_t*, pn_messenger_incoming_subscription, pn_messenger_t*, messenger)
    MOCK_METHOD_END(pn_subscription_t*, TEST_PN_SUBSCRIPTION)

    MOCK_STATIC_METHOD_2(, int, pn_messenger_set_client_sasl_mechanism, pn_messenger_t*, messenger, const char*, source)
    MOCK_METHOD_END(int, 1)

    MOCK_STATIC_METHOD_1(, int, pn_error_code, pn_error_t*, es)
    MOCK_METHOD_END(int,0)

    MOCK_STATIC_METHOD_1(, int, pn_messenger_incoming, pn_messenger_t*, messenger)
    MOCK_METHOD_END(int, TEST_PN_GOOD_RESULT)

    MOCK_STATIC_METHOD_1(, bool, pn_data_next, pn_data_t*, data)
    MOCK_METHOD_END(bool, true)

    MOCK_STATIC_METHOD_1(, pn_type_t, pn_data_type, pn_data_t*, data)
    MOCK_METHOD_END(pn_type_t, PN_STRING)

    MOCK_STATIC_METHOD_1(, pn_bytes_t, pn_data_get_binary, pn_data_t*, data)
    MOCK_METHOD_END(pn_bytes_t, TEST_PN_GOOD_BYTES)

    MOCK_STATIC_METHOD_3(, int, pn_messenger_accept, pn_messenger_t*, messenger, pn_tracker_t, tracker, int, flags)
    MOCK_METHOD_END(int, TEST_PN_GOOD_RESULT)

    MOCK_STATIC_METHOD_3(, int, pn_messenger_reject, pn_messenger_t*, messenger, pn_tracker_t, tracker, int, flags)
    MOCK_METHOD_END(int, TEST_PN_GOOD_RESULT)

    MOCK_STATIC_METHOD_3(, int, pn_messenger_release, pn_messenger_t*, messenger, pn_tracker_t, tracker, int, flags)
    MOCK_METHOD_END(int, TEST_PN_GOOD_RESULT)

    MOCK_STATIC_METHOD_2(, int, pn_messenger_get, pn_messenger_t*, messenger, pn_message_t*, message)
    MOCK_METHOD_END(int, TEST_PN_GOOD_RESULT)

    MOCK_STATIC_METHOD_1(, pn_tracker_t, pn_messenger_incoming_tracker, pn_messenger_t*, messenger)
    MOCK_METHOD_END(pn_tracker_t, TEST_PN_TRACKER)

    MOCK_STATIC_METHOD_2(, int, pn_messenger_recv, pn_messenger_t*, messenger, int, limit)
    MOCK_METHOD_END(int, TEST_PN_GOOD_RESULT)

    MOCK_STATIC_METHOD_1(, bool, pn_data_enter, pn_data_t*, data)
    MOCK_METHOD_END(bool, true)

    MOCK_STATIC_METHOD_1(, bool, pn_data_exit, pn_data_t*, data)
    MOCK_METHOD_END(bool, true)

    MOCK_STATIC_METHOD_1(, int, pn_data_put_map, pn_data_t*, data)
    MOCK_METHOD_END(int, 0)

    MOCK_STATIC_METHOD_1(, size_t, pn_data_get_map, pn_data_t*, data)
    MOCK_METHOD_END(size_t, 0)

    MOCK_STATIC_METHOD_2(, int, pn_data_put_string, pn_data_t*, data, pn_bytes_t, bytes)
    MOCK_METHOD_END(int, 0)

    MOCK_STATIC_METHOD_2(, int, pn_data_put_symbol, pn_data_t*, data, pn_bytes_t, bytes)
    MOCK_METHOD_END(int, 0)

    MOCK_STATIC_METHOD_1(, pn_bytes_t, pn_data_get_string, pn_data_t*, data)
    MOCK_METHOD_END(pn_bytes_t, TEST_PN_STRING_BYTES)

    MOCK_STATIC_METHOD_1(, int32_t, pn_data_get_int, pn_data_t*, data)
    MOCK_METHOD_END(int32_t, 0)

    MOCK_STATIC_METHOD_1(, pn_atom_t, pn_message_get_correlation_id, pn_message_t*, msg)
        pn_atom_t* pnCurrId = &test_NULL_atom;
        if (currentCorrelationId == 1)
        {
            pnCurrId = &test_correlationId_atom;
        }
        else if (currentCorrelationId == 2)
        {
            pnCurrId = &test_pn_atom;
        }
    MOCK_METHOD_END(pn_atom_t, *pnCurrId)

    MOCK_STATIC_METHOD_1(, pn_atom_t, pn_message_get_id, pn_message_t*, msg)
    MOCK_METHOD_END(pn_atom_t, test_messageId_atom)

    MOCK_STATIC_METHOD_1(, pn_data_t*, pn_message_properties, pn_message_t*, msg)
    MOCK_METHOD_END(pn_data_t*, (pn_data_t*)TEST_PN_DATA)

    MOCK_STATIC_METHOD_2(, int, pn_message_set_reply_to, pn_message_t*, msg, const char*, reply_to)
    MOCK_METHOD_END(int, 0)

    MOCK_STATIC_METHOD_2(, int, pn_message_set_id, pn_message_t*, msg, pn_atom_t, id)
    MOCK_METHOD_END(int, 0)

    MOCK_STATIC_METHOD_2(, int, pn_message_set_correlation_id, pn_message_t*, msg, pn_atom_t, id)
    MOCK_METHOD_END(int, 0)

    MOCK_STATIC_METHOD_1(, pn_error_t*, pn_messenger_error, pn_messenger_t*, messenger)
    MOCK_METHOD_END(pn_error_t*, (pn_error_t*)NULL)

    MOCK_STATIC_METHOD_1(, const char*, pn_error_text, pn_error_t*, error)
    MOCK_METHOD_END(const char*, NULL)


    /* crt_abstractions mocks */
    MOCK_STATIC_METHOD_2(, int, mallocAndStrcpy_s, char**, destination, const char*, source)
    MOCK_METHOD_END(int, (*destination = (char*)BASEIMPLEMENTATION::gballoc_malloc(strlen(source) + 1), strcpy(*destination, source), 0))

    /* IoTHub_Message mocks */
    MOCK_STATIC_METHOD_2(, IOTHUB_MESSAGE_HANDLE, IoTHubMessage_CreateFromByteArray, const unsigned char*, buffer, size_t, size)
    MOCK_METHOD_END(IOTHUB_MESSAGE_HANDLE, TEST_IOTHUB_MESSAGE_HANDLE)

    MOCK_STATIC_METHOD_1(, void, IoTHubMessage_Destroy, IOTHUB_MESSAGE_HANDLE, iotHubMessageHandle)
    MOCK_VOID_METHOD_END()

    MOCK_STATIC_METHOD_3(, IOTHUB_MESSAGE_RESULT, IoTHubMessage_GetByteArray, IOTHUB_MESSAGE_HANDLE, iotHubMessageHandle, const unsigned char**, buffer, size_t*, size)
    IOTHUB_MESSAGE_RESULT result2;
        if (iotHubMessageHandle != TEST_MESSAGE_HANDLE_OVERIDE_DEFAULTS)
        {
            *buffer = (const unsigned char*)TEXT_MESSAGE;
            *size = sizeof(TEXT_MESSAGE);
            result2 = IOTHUB_MESSAGE_OK;
        }
        else if (iotHubMessageHandle == TEST_MESSAGE_HANDLE_STRING)
        {
            result2 = IOTHUB_MESSAGE_ERROR;
        }
        else
        {
            *buffer = (const unsigned char*)TEXT_MESSAGE;
            *size = PROTON_MAXIMUM_EVENT_LENGTH + 1;
            result2 = IOTHUB_MESSAGE_OK;
        }
    
    MOCK_METHOD_END(IOTHUB_MESSAGE_RESULT, result2)

    MOCK_STATIC_METHOD_1(, const char*, IoTHubMessage_GetString, IOTHUB_MESSAGE_HANDLE, handle)
        const char* result2;
        if (handle == TEST_MESSAGE_HANDLE_STRING)
        {
            result2 = (const char*)TEXT_MESSAGE;
    }
        else
        {
            result2 = NULL;
        }
    MOCK_METHOD_END(const char*, result2)

    MOCK_STATIC_METHOD_1(, IOTHUBMESSAGE_CONTENT_TYPE, IoTHubMessage_GetContentType, IOTHUB_MESSAGE_HANDLE, iotHubMessageHandle)
        IOTHUBMESSAGE_CONTENT_TYPE result2;
        if(iotHubMessageHandle == TEST_MESSAGE_HANDLE_STRING)
        {
            result2 = IOTHUBMESSAGE_STRING;
        }
        else
        {
            result2 = IOTHUBMESSAGE_BYTEARRAY;
        }
    MOCK_METHOD_END(IOTHUBMESSAGE_CONTENT_TYPE, result2)

    MOCK_STATIC_METHOD_1(, IOTHUB_MESSAGE_HANDLE, IoTHubMessage_Clone, IOTHUB_MESSAGE_HANDLE, iotHubMessageHandle)
    MOCK_METHOD_END(IOTHUB_MESSAGE_HANDLE, TEST_IOTHUB_MESSAGE_HANDLE)

    MOCK_STATIC_METHOD_1(, MAP_HANDLE, IoTHubMessage_Properties, IOTHUB_MESSAGE_HANDLE, iotHubMessageHandle)
        MAP_HANDLE result2 = TEST_MAP_EMPTY;
        /*if (iotHubMessageHandle == TEST_IOTHUB_MESSAGE_HANDLE)
        {
            result2 = TEST_MAP_1_PROPERTY;
        }*/
    MOCK_METHOD_END(MAP_HANDLE, result2)

    MOCK_STATIC_METHOD_2(, IOTHUB_MESSAGE_RESULT, IoTHubMessage_SetMessageId, IOTHUB_MESSAGE_HANDLE, iotHubMessageHandle, const char*, messageId)
    MOCK_METHOD_END(IOTHUB_MESSAGE_RESULT, IOTHUB_MESSAGE_OK)

    MOCK_STATIC_METHOD_1(, const char*, IoTHubMessage_GetMessageId, IOTHUB_MESSAGE_HANDLE, iotHubMessageHandle)
    MOCK_METHOD_END(const char*, NULL)

    MOCK_STATIC_METHOD_2(, IOTHUB_MESSAGE_RESULT, IoTHubMessage_SetCorrelationId, IOTHUB_MESSAGE_HANDLE, iotHubMessageHandle, const char*, messageId)
    MOCK_METHOD_END(IOTHUB_MESSAGE_RESULT, IOTHUB_MESSAGE_OK)

    MOCK_STATIC_METHOD_1(, const char*, IoTHubMessage_GetCorrelationId, IOTHUB_MESSAGE_HANDLE, iotHubMessageHandle)
    MOCK_METHOD_END(const char*, NULL)

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

    MOCK_STATIC_METHOD_2(, void, DList_AppendTailList, PDLIST_ENTRY, listHead, PDLIST_ENTRY, ListToAppend)
        BASEIMPLEMENTATION::DList_AppendTailList(listHead, ListToAppend);
    MOCK_VOID_METHOD_END()

    MOCK_STATIC_METHOD_1(, int, DList_RemoveEntryList, PDLIST_ENTRY, listEntry)
        int result2 = BASEIMPLEMENTATION::DList_RemoveEntryList(listEntry);
    MOCK_METHOD_END(int, result2)

    MOCK_STATIC_METHOD_1(, PDLIST_ENTRY, DList_RemoveHeadList, PDLIST_ENTRY, listHead)
        PDLIST_ENTRY entry = BASEIMPLEMENTATION::DList_RemoveHeadList(listHead);
    MOCK_METHOD_END(PDLIST_ENTRY, entry)

    /* URL_Encode Mocks */
    MOCK_STATIC_METHOD_1(, STRING_HANDLE, URL_EncodeString, const char*, input)
    MOCK_METHOD_END(STRING_HANDLE, TEST_ENCODED_STRING_HANDLE)

    /* STRING Mocks */
    MOCK_STATIC_METHOD_1(, STRING_HANDLE, STRING_new_with_memory, const char*, memory)
    MOCK_METHOD_END(STRING_HANDLE, TEST_STRING_HANDLE)

    MOCK_STATIC_METHOD_0(, STRING_HANDLE, STRING_new)
    MOCK_METHOD_END(STRING_HANDLE, TEST_STRING_HANDLE)

    MOCK_STATIC_METHOD_1(, STRING_HANDLE, STRING_construct, const char*, psz)
    MOCK_METHOD_END(STRING_HANDLE, TEST_STRING_HANDLE)

    MOCK_STATIC_METHOD_2(, int, STRING_concat, STRING_HANDLE, handle, const char*, s2)
    MOCK_METHOD_END(int, 0)

    MOCK_STATIC_METHOD_2(, int, STRING_concat_with_STRING, STRING_HANDLE, s1, STRING_HANDLE, s2)
    MOCK_METHOD_END(int, 0)

    MOCK_STATIC_METHOD_1(, void, STRING_delete, STRING_HANDLE, handle)
    MOCK_VOID_METHOD_END()

    MOCK_STATIC_METHOD_1(, const char*, STRING_c_str, STRING_HANDLE, s)
    MOCK_METHOD_END(const char*, TEST_CHAR)

    MOCK_STATIC_METHOD_1(, size_t, STRING_length, STRING_HANDLE, s)
    MOCK_METHOD_END(size_t, 0)
    MOCK_STATIC_METHOD_1(, int, STRING_empty, STRING_HANDLE, s)
    MOCK_METHOD_END(int, 0);


    /* BUFFER Mocks */
    MOCK_STATIC_METHOD_0(, BUFFER_HANDLE, BUFFER_new)
        BUFFER_HANDLE result2;
        currentBUFFER_new_call++;
        if (whenShallBUFFER_new_fail>0)
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

    MOCK_STATIC_METHOD_1(, unsigned char*, BUFFER_u_char, BUFFER_HANDLE, handle);
        unsigned char* result2;
        result2 = BASEIMPLEMENTATION::BUFFER_u_char(handle);
    MOCK_METHOD_END(unsigned char*, result2)

    MOCK_STATIC_METHOD_1(, size_t, BUFFER_length, BUFFER_HANDLE, handle);
        size_t result2;
        result2 = BASEIMPLEMENTATION::BUFFER_length(handle);
    MOCK_METHOD_END(size_t, result2)

    MOCK_STATIC_METHOD_3(, int, size_tToString, char*, destination, size_t, destinationSize, size_t, value)
    MOCK_METHOD_END(int, 0)

    MOCK_STATIC_METHOD_1(, void*, gballoc_malloc, size_t, size)
        void* result2;
        currentmalloc_call++;
        if (whenShallmalloc_fail>0)
        {
            if (currentmalloc_call == whenShallmalloc_fail)
            {
                result2 = (STRING_HANDLE)NULL;
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

    MOCK_STATIC_METHOD_2(, IOTHUBMESSAGE_DISPOSITION_RESULT, IoTHubClient_LL_MessageCallback, IOTHUB_CLIENT_LL_HANDLE, handle, IOTHUB_MESSAGE_HANDLE, messageHandle)
    MOCK_METHOD_END(IOTHUBMESSAGE_DISPOSITION_RESULT, IOTHUBMESSAGE_ACCEPTED);

    MOCK_STATIC_METHOD_3(, void, IoTHubClient_LL_SendComplete, IOTHUB_CLIENT_LL_HANDLE, handle, PDLIST_ENTRY, completedMessages, IOTHUB_BATCHSTATE_RESULT, batchResult)
        PDLIST_ENTRY oldest;
        while ((oldest = BASEIMPLEMENTATION::DList_RemoveHeadList(completedMessages)) != completedMessages)
        {
            ;
        }
    MOCK_VOID_METHOD_END();

    MOCK_STATIC_METHOD_3(, MAP_RESULT, Map_AddOrUpdate, MAP_HANDLE, handle, const char*, key, const char*, value)
    MOCK_METHOD_END(MAP_RESULT, MAP_OK)

    MOCK_STATIC_METHOD_4(, MAP_RESULT, Map_GetInternals, MAP_HANDLE, handle, const char*const**, keys, const char*const**, values, size_t*, count)
        switch ((uintptr_t)handle)
        {
            case ((uintptr_t)TEST_MAP_EMPTY):
            {
                *keys = NULL;
                *values = NULL;
                *count = 0;
                break;
            }
            case ((uintptr_t)TEST_MAP_1_PROPERTY):
            {
                *keys = (const char*const*)TEST_KEYS1;
                *values = (const char*const*)TEST_VALUES1;
                *count = sizeof(TEST_VALUES1)/sizeof(TEST_VALUES1);
                break;
            }
            case ((uintptr_t)TEST_MAP_2_PROPERTY):
            {
                *keys = (const char*const*)TEST_KEYS2;
                *values = (const char*const*)TEST_VALUES2;
                *count = sizeof(TEST_VALUES2)/sizeof(TEST_VALUES2);
                break;
            }
            default:
            {
                ASSERT_FAIL("unexpected value");
            }
        }
    MOCK_METHOD_END(MAP_RESULT, MAP_OK);

    MOCK_STATIC_METHOD_1(, time_t, get_time, time_t*, t)
    MOCK_METHOD_END(time_t, 0);

    MOCK_STATIC_METHOD_4(, STRING_HANDLE, SASToken_Create, STRING_HANDLE, key, STRING_HANDLE, scope, STRING_HANDLE, keyName, size_t, expiry)
    MOCK_METHOD_END(STRING_HANDLE, malloc(1));


};

DECLARE_GLOBAL_MOCK_METHOD_0(CIoTHubTransportAMQPMocks, , pn_message_t*, pn_message);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportAMQPMocks, , int, pn_message_set_address, pn_message_t*, msg, const char*, address);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportAMQPMocks, , int, pn_message_set_inferred, pn_message_t*, msg, bool, inferred);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , pn_data_t*, pn_message_body, pn_message_t*, msg);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportAMQPMocks, , int, pn_data_put_binary, pn_data_t*, data, pn_bytes_t, bytes);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , pn_messenger_t*, pn_messenger, const char*, name);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , int, pn_messenger_start, pn_messenger_t*, messsenger);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportAMQPMocks, , int, pn_messenger_put, pn_messenger_t*, messenger, pn_message_t*, msg);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportAMQPMocks, , int, pn_messenger_send, pn_messenger_t*, messenger, int, n);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , int, pn_messenger_stop, pn_messenger_t*, messenger);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , void, pn_messenger_free, pn_messenger_t*, messenger);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , void, pn_message_free, pn_message_t*, msg);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , void, pn_message_clear, pn_message_t*, msg);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportAMQPMocks, , pn_bytes_t, pn_bytes, size_t, size, const char*, start);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportAMQPMocks, , int, pn_messenger_set_blocking, pn_messenger_t*, messenger, bool, blocking);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportAMQPMocks, , int, pn_messenger_set_timeout, pn_messenger_t*, messenger, int, timeout);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , bool, pn_messenger_stopped, pn_messenger_t*, messenger);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportAMQPMocks, , int, pn_messenger_set_outgoing_window, pn_messenger_t*, messenger, int, window);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportAMQPMocks, , int, pn_messenger_set_trusted_certificates, pn_messenger_t*, messenger, const char*, cert_db);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportAMQPMocks, , int, pn_messenger_set_incoming_window, pn_messenger_t*, messenger, int, wndow);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportAMQPMocks, , pn_status_t, pn_messenger_status, pn_messenger_t*, messenger, pn_tracker_t, tracker);
DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubTransportAMQPMocks, , int, pn_messenger_settle, pn_messenger_t*, messenger, pn_tracker_t, tracker, int, flags);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , pn_tracker_t, pn_messenger_outgoing_tracker, pn_messenger_t*, messenger);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportAMQPMocks, , int, pn_messenger_set_snd_settle_mode, pn_messenger_t*, messenger, pn_snd_settle_mode_t, mode);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportAMQPMocks, , int, pn_messenger_set_rcv_settle_mode, pn_messenger_t*, messenger, pn_rcv_settle_mode_t, mode);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportAMQPMocks, , int, pn_messenger_work, pn_messenger_t*, messenger, int, timeout);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportAMQPMocks, , pn_subscription_t*, pn_messenger_subscribe, pn_messenger_t*, messenger, const char*, source);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , int, pn_messenger_incoming, pn_messenger_t*, messenger);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , bool, pn_data_next, pn_data_t*, data);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , pn_type_t, pn_data_type, pn_data_t*, data);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , pn_bytes_t, pn_data_get_binary, pn_data_t*, data);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , int32_t, pn_data_get_int, pn_data_t*, data);
DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubTransportAMQPMocks, , int, pn_messenger_accept, pn_messenger_t*, messenger, pn_tracker_t, tracker, int, flags);
DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubTransportAMQPMocks, , int, pn_messenger_reject, pn_messenger_t*, messenger, pn_tracker_t, tracker, int, flags);
DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubTransportAMQPMocks, , int, pn_messenger_release, pn_messenger_t*, messenger, pn_tracker_t, tracker, int, flags);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportAMQPMocks, , int, pn_messenger_get, pn_messenger_t*, messenger, pn_message_t*, message);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , pn_tracker_t, pn_messenger_incoming_tracker, pn_messenger_t*, messenger);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportAMQPMocks, , int, pn_messenger_recv, pn_messenger_t*, messenger, int, limit);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , bool, pn_data_enter, pn_data_t*, data);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , bool, pn_data_exit, pn_data_t*, data);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , int, pn_data_put_map, pn_data_t*, data);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , size_t, pn_data_get_map, pn_data_t*, data);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportAMQPMocks, , int, pn_data_put_string, pn_data_t*, data, pn_bytes_t, bytes);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportAMQPMocks, , int, pn_data_put_symbol, pn_data_t*, data, pn_bytes_t, bytes);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , pn_bytes_t, pn_data_get_string, pn_data_t*, data);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , pn_data_t*, pn_message_properties, pn_message_t*, msg);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , pn_subscription_t*, pn_messenger_incoming_subscription, pn_messenger_t*, messenger)
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportAMQPMocks, , int, pn_messenger_set_client_sasl_mechanism, pn_messenger_t*, messenger, const char*, source)
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , pn_atom_t, pn_message_get_correlation_id, pn_message_t*, msg)
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , pn_atom_t, pn_message_get_id, pn_message_t*, msg)
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportAMQPMocks, , int, pn_message_set_reply_to, pn_message_t*, msg, const char*, reply_to)
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportAMQPMocks, , int, pn_message_set_id, pn_message_t*, msg, pn_atom_t, id)
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportAMQPMocks, , int, pn_message_set_correlation_id, pn_message_t*, msg, pn_atom_t, id)
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , pn_error_t*, pn_messenger_error, pn_messenger_t*, messenger)
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , const char*, pn_error_text, pn_error_t*, error)
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , int, pn_error_code, pn_error_t*, es)

DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportAMQPMocks, , IOTHUB_MESSAGE_HANDLE, IoTHubMessage_CreateFromByteArray, const unsigned char*, buffre, size_t, size);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , const char*, IoTHubMessage_GetString, IOTHUB_MESSAGE_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , void, IoTHubMessage_Destroy, IOTHUB_MESSAGE_HANDLE, iotHubMessageHandle);
DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubTransportAMQPMocks, , IOTHUB_MESSAGE_RESULT, IoTHubMessage_GetByteArray, IOTHUB_MESSAGE_HANDLE, iotHubMessageHandle, const unsigned char**, buffer, size_t*, size);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , IOTHUB_MESSAGE_HANDLE, IoTHubMessage_Clone, IOTHUB_MESSAGE_HANDLE, iotHubMessageHandle);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , MAP_HANDLE, IoTHubMessage_Properties, IOTHUB_MESSAGE_HANDLE, iotHubMessageHandle);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , IOTHUBMESSAGE_CONTENT_TYPE, IoTHubMessage_GetContentType, IOTHUB_MESSAGE_HANDLE, iotHubMessageHandle)
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportAMQPMocks, , IOTHUB_MESSAGE_RESULT, IoTHubMessage_SetMessageId, IOTHUB_MESSAGE_HANDLE, iotHubMessageHandle, const char*, messageId);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , const char*, IoTHubMessage_GetMessageId, IOTHUB_MESSAGE_HANDLE, iotHubMessageHandle);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportAMQPMocks, , IOTHUB_MESSAGE_RESULT, IoTHubMessage_SetCorrelationId, IOTHUB_MESSAGE_HANDLE, iotHubMessageHandle, const char*, messageId);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , const char*, IoTHubMessage_GetCorrelationId, IOTHUB_MESSAGE_HANDLE, iotHubMessageHandle);

DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , void, DList_InitializeListHead, PDLIST_ENTRY, listHead);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , int, DList_IsListEmpty, PDLIST_ENTRY, listHead);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportAMQPMocks, , void, DList_InsertTailList, PDLIST_ENTRY, listHead, PDLIST_ENTRY, listEntry);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportAMQPMocks, , void, DList_AppendTailList, PDLIST_ENTRY, listHead, PDLIST_ENTRY, ListToAppend);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , int, DList_RemoveEntryList, PDLIST_ENTRY, listEntry);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , PDLIST_ENTRY, DList_RemoveHeadList, PDLIST_ENTRY, listHead);

DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , STRING_HANDLE, URL_EncodeString, const char*, input);

DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , STRING_HANDLE, STRING_new_with_memory, const char*, memory);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , STRING_HANDLE, STRING_construct, const char*, psz);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportAMQPMocks, , int, STRING_concat, STRING_HANDLE, handle, const char*, s2);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportAMQPMocks, , int, STRING_concat_with_STRING, STRING_HANDLE, s1, STRING_HANDLE, s2);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , void, STRING_delete, STRING_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , const char*, STRING_c_str, STRING_HANDLE, s);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , size_t, STRING_length, STRING_HANDLE, s)
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , int, STRING_empty, STRING_HANDLE, s)
DECLARE_GLOBAL_MOCK_METHOD_0(CIoTHubTransportAMQPMocks, , STRING_HANDLE, STRING_new)


DECLARE_GLOBAL_MOCK_METHOD_0(CIoTHubTransportAMQPMocks, , BUFFER_HANDLE, BUFFER_new);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , void, BUFFER_delete, BUFFER_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , unsigned char*, BUFFER_u_char, BUFFER_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , size_t, BUFFER_length, BUFFER_HANDLE, handle);

DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubTransportAMQPMocks, , int, size_tToString, char*, destination, size_t, destinationSize, size_t, value);

DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , void*, gballoc_malloc, size_t, size);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportAMQPMocks, , void*, gballoc_realloc, void*, ptr, size_t, size);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , void, gballoc_free, void*, ptr);

DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportAMQPMocks, , IOTHUBMESSAGE_DISPOSITION_RESULT, IoTHubClient_LL_MessageCallback, IOTHUB_CLIENT_LL_HANDLE, handle, IOTHUB_MESSAGE_HANDLE, messageHandle);
DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubTransportAMQPMocks, , void, IoTHubClient_LL_SendComplete, IOTHUB_CLIENT_LL_HANDLE, handle, PDLIST_ENTRY, completedMessages, IOTHUB_BATCHSTATE_RESULT, batchResult);

DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubTransportAMQPMocks, , MAP_RESULT, Map_AddOrUpdate, MAP_HANDLE, handle, const char*, key, const char*, value);
DECLARE_GLOBAL_MOCK_METHOD_4(CIoTHubTransportAMQPMocks, , MAP_RESULT, Map_GetInternals, MAP_HANDLE, handle, const char*const**, keys, const char*const**, values, size_t*, count);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , time_t, get_time, time_t*, t)
DECLARE_GLOBAL_MOCK_METHOD_4(CIoTHubTransportAMQPMocks, , STRING_HANDLE, SASToken_Create, STRING_HANDLE, key, STRING_HANDLE, scope, STRING_HANDLE, keyName, size_t, expiry)

DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportAMQPMocks, , int, mallocAndStrcpy_s, char**, destination, const char*, source);

// ** End Mocks **

static void mocksResetAllCalls(CIoTHubTransportAMQPMocks* mocks)
{
    if (mocks != NULL)
    {
        mocks->ResetAllCalls();
    }
    currentSTRING_new_call = 0;
    whenShallSTRING_new_fail = 0;

    currentSTRING_new_quoted_call = 0;
    whenShallSTRING_new_quoted_fail = 0;

    currentBase64_Encode_call = 0;
    whenShallBase64_Encode_fail = 0;

    currentSTRING_clone_call = 0;
    whenShallSTRING_clone_fail = 0;

    currentSTRING_construct_call = 0;
    whenShallSTRING_construct_fail = 0;

    currentSTRING_concat_call = 0;
    whenShallSTRING_concat_fail = 0;

    currentSTRING_concat_with_STRING_call = 0;
    whenShallSTRING_concat_with_STRING_fail = 0;

    currentmalloc_call = 0;
    whenShallmalloc_fail = 0;

    currentlockinit_call = 0;
    whenShalllockinit_fail = 0;

    currentlock_call = 0;
    whenShalllock_fail = 0;

    currentBUFFER_new_call = 0;
    whenShallBUFFER_new_fail = 0;

    currentBUFFER_build_call = 0;
    whenShallBUFFER_build_fail = 0;

    currentBUFFER_content_call = 0;
    whenShallBUFFER_content_fail = 0;

    currentBUFFER_size_call = 0;
    whenShallBUFFER_size_fail = 0;

    currentpn_messenger_status_call = 0;
    whenShallpn_messenger_status_return_alternate = 0;
    alternate_pn_status = PN_STATUS_ACCEPTED;

    test_messageId_atom.type = PN_STRING;
    test_messageId_atom.u.as_bytes = msgid_bytes;

    test_correlationId_atom.type = PN_STRING;
    test_correlationId_atom.u.as_bytes = msgid_bytes;

    currentCorrelationId = 0;
}

static STRING_HANDLE hostStringHandle = (STRING_HANDLE)0x201;
static STRING_HANDLE urledDeviceIdStringHandle = (STRING_HANDLE)0x202;
static STRING_HANDLE devicesPortionPathStringHandle = (STRING_HANDLE)0x203;
static STRING_HANDLE eventAmqpsStringHandle = (STRING_HANDLE)0x204;
static STRING_HANDLE messageAmqpsStringHandle = (STRING_HANDLE)0x205;
static STRING_HANDLE cbsAmqpsStringHandle = (STRING_HANDLE)0x206;
static STRING_HANDLE deviceKeyStringHandle = (STRING_HANDLE)0x207;
static STRING_HANDLE sasTokenStringHandle = (STRING_HANDLE)0x208;
static pn_subscription_t* messageSubscriptionValue = (pn_subscription_t*)0x208;
static pn_subscription_t* cbsSubscriptionValue = (pn_subscription_t*)0x209;
static pn_error_t* protonErrorHandle = (pn_error_t*)0x20a;
static STRING_HANDLE zeroLengthStringHandle = (STRING_HANDLE)0x20b;

static void mocksFor_processCBS(CIoTHubTransportAMQPMocks &mocks, size_t whichPath);
static void mocksFor_processReceives(CIoTHubTransportAMQPMocks &mocks, size_t whichPath);


static void mocksFor_protonWork(CIoTHubTransportAMQPMocks &mocks, size_t whichPath)
{
    (void)mocks;

    STRICT_EXPECTED_CALL(mocks, pn_messenger_error(TEST_PN_MESSENGER)).SetReturn(protonErrorHandle);
    if (whichPath == 1)
    {
        STRICT_EXPECTED_CALL(mocks, pn_error_code(protonErrorHandle)).SetReturn(PN_ERR);
        STRICT_EXPECTED_CALL(mocks, pn_error_code(protonErrorHandle)).SetReturn(PN_ERR);
    }
    else
    {
        STRICT_EXPECTED_CALL(mocks, pn_error_code(protonErrorHandle)).SetReturn(0);
        if (whichPath == 2)
        {
            STRICT_EXPECTED_CALL(mocks, pn_messenger_work(TEST_PN_MESSENGER, TEST_PN_WORK_MILLISECONDS)).SetReturn(PN_ERR);
        }
        else
        {
            STRICT_EXPECTED_CALL(mocks, pn_messenger_work(TEST_PN_MESSENGER, TEST_PN_WORK_MILLISECONDS));
        }
    }
}

#define PUT_TOKEN_PATH_THROUGH_PROCESS_RECEIVES (8)
#define PUT_TOKEN_PATH_THROUGH_PROCESS_CBS (9)
static void mocksFor_putToken(CIoTHubTransportAMQPMocks &mocks, size_t whichPath)
{
    (void)mocks;

    STRICT_EXPECTED_CALL(mocks, pn_message_clear(TEST_PN_MESSAGE));
    if (whichPath == 1)
    {
        STRICT_EXPECTED_CALL(mocks, pn_message_properties(TEST_PN_MESSAGE)).SetReturn(TEST_NULL_PN_DATA);
    }
    else
    {
        STRICT_EXPECTED_CALL(mocks, pn_message_properties(TEST_PN_MESSAGE)).SetReturn(TEST_PN_DATA);
        if (whichPath == 2)
        {
            STRICT_EXPECTED_CALL(mocks, pn_data_put_map(TEST_PN_DATA)).SetReturn(1);
        }
        else
        {
            STRICT_EXPECTED_CALL(mocks, pn_data_put_map(TEST_PN_DATA)).SetReturn(0);
            STRICT_EXPECTED_CALL(mocks, pn_data_enter(TEST_PN_DATA)).SetReturn(true);

            pn_bytes_t operKey;
            operKey.size = strlen(PROTON_MAP_OPERATIONS_KEY);
            operKey.start = PROTON_MAP_OPERATIONS_KEY;
            STRICT_EXPECTED_CALL(mocks, pn_bytes(strlen(PROTON_MAP_OPERATIONS_KEY), PROTON_MAP_OPERATIONS_KEY)).SetReturn(operKey);
            STRICT_EXPECTED_CALL(mocks, pn_data_put_string(TEST_PN_DATA, operKey)).SetReturn(0);

            pn_bytes_t operValue;
            operValue.size = strlen(PROTON_MAP_PUT_TOKEN_OPERATION);
            operValue.start = PROTON_MAP_PUT_TOKEN_OPERATION;
            STRICT_EXPECTED_CALL(mocks, pn_bytes(strlen(PROTON_MAP_PUT_TOKEN_OPERATION), PROTON_MAP_PUT_TOKEN_OPERATION)).SetReturn(operValue);
            STRICT_EXPECTED_CALL(mocks, pn_data_put_string(TEST_PN_DATA, operValue)).SetReturn(0);

            pn_bytes_t typeKey;
            typeKey.size = strlen(PROTON_MAP_TYPE_KEY);
            typeKey.start = PROTON_MAP_TYPE_KEY;
            STRICT_EXPECTED_CALL(mocks, pn_bytes(strlen(PROTON_MAP_TYPE_KEY), PROTON_MAP_TYPE_KEY)).SetReturn(typeKey);
            STRICT_EXPECTED_CALL(mocks, pn_data_put_string(TEST_PN_DATA, typeKey)).SetReturn(0);

            pn_bytes_t typeValue;
            typeValue.size = strlen(PROTON_MAP_TOKEN_TYPE);
            typeValue.start = PROTON_MAP_TOKEN_TYPE;
            STRICT_EXPECTED_CALL(mocks, pn_bytes(strlen(PROTON_MAP_TOKEN_TYPE), PROTON_MAP_TOKEN_TYPE)).SetReturn(typeValue);
            STRICT_EXPECTED_CALL(mocks, pn_data_put_string(TEST_PN_DATA, typeValue)).SetReturn(0);

            pn_bytes_t nameKey;
            nameKey.size = strlen(PROTON_MAP_NAME_KEY);
            nameKey.start = PROTON_MAP_NAME_KEY;
            STRICT_EXPECTED_CALL(mocks, pn_bytes(strlen(PROTON_MAP_NAME_KEY), PROTON_MAP_NAME_KEY)).SetReturn(nameKey);
            STRICT_EXPECTED_CALL(mocks, pn_data_put_string(TEST_PN_DATA, nameKey)).SetReturn(0);

            STRICT_EXPECTED_CALL(mocks, STRING_length(devicesPortionPathStringHandle)).SetReturn(strlen(TEST_DEVICE_ID));
            STRICT_EXPECTED_CALL(mocks, STRING_c_str(devicesPortionPathStringHandle)).SetReturn(TEST_DEVICE_ID);
            pn_bytes_t nameValue;
            nameValue.size = strlen(TEST_DEVICE_ID);
            nameValue.start = TEST_DEVICE_ID;
            STRICT_EXPECTED_CALL(mocks, pn_bytes(strlen(TEST_DEVICE_ID), TEST_DEVICE_ID)).SetReturn(nameValue);
            STRICT_EXPECTED_CALL(mocks, pn_data_put_string(TEST_PN_DATA, nameValue)).SetReturn(0);

            STRICT_EXPECTED_CALL(mocks, pn_data_exit(TEST_PN_DATA)).SetReturn(true);
            STRICT_EXPECTED_CALL(mocks, pn_message_set_reply_to(TEST_PN_MESSAGE, CBS_REPLY_TO)).SetReturn(0);
            STRICT_EXPECTED_CALL(mocks, STRING_c_str(cbsAmqpsStringHandle)).SetReturn(TEST_SCHEME_AND_HOST CBS_ENDPOINT);
            STRICT_EXPECTED_CALL(mocks, pn_message_set_address(TEST_PN_MESSAGE, TEST_SCHEME_AND_HOST CBS_ENDPOINT)).SetReturn(0);
            STRICT_EXPECTED_CALL(mocks, pn_message_set_inferred(TEST_PN_MESSAGE, false)).SetReturn(0);
            pn_atom_t messageId;
            messageId.u.as_ulong = TEST_EXPIRY_TEST_VALUE;
            if (whichPath == 3)
            {
                STRICT_EXPECTED_CALL(mocks, pn_message_set_id(TEST_PN_MESSAGE, messageId)).SetReturn(1);
            }
            else
            {
                STRICT_EXPECTED_CALL(mocks, pn_message_set_id(TEST_PN_MESSAGE, messageId)).SetReturn(0);
                STRICT_EXPECTED_CALL(mocks, pn_message_body(TEST_PN_MESSAGE)).SetReturn(TEST_PN_DATA);
                STRICT_EXPECTED_CALL(mocks, STRING_length(sasTokenStringHandle)).SetReturn(sizeof(TEST_SAS_TOKEN));
                STRICT_EXPECTED_CALL(mocks, STRING_c_str(sasTokenStringHandle)).SetReturn(TEST_SAS_TOKEN);
                pn_bytes_t sasData;
                sasData.size = sizeof(TEST_SAS_TOKEN);
                sasData.start = TEST_SAS_TOKEN;
                STRICT_EXPECTED_CALL(mocks, pn_bytes(sizeof(TEST_SAS_TOKEN), TEST_SAS_TOKEN)).SetReturn(sasData);
                STRICT_EXPECTED_CALL(mocks, pn_data_put_string(TEST_PN_DATA, sasData)).SetReturn(0);
                if (whichPath == 4)
                {
                    STRICT_EXPECTED_CALL(mocks, pn_messenger_put(TEST_PN_MESSENGER, TEST_PN_MESSAGE)).SetReturn(1);
                }
                else
                {
                    STRICT_EXPECTED_CALL(mocks, pn_messenger_put(TEST_PN_MESSENGER, TEST_PN_MESSAGE)).SetReturn(0);
                    STRICT_EXPECTED_CALL(mocks, pn_messenger_outgoing_tracker(TEST_PN_MESSENGER)).SetReturn(TEST_PN_TRACKER);
                    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(0);
                    STRICT_EXPECTED_CALL(mocks, pn_messenger_settle(TEST_PN_MESSENGER, TEST_PN_TRACKER, 0));
                    if (whichPath == 5)
                    {
                        mocksFor_protonWork(mocks, 1);
                    }
                    else
                    {
                        mocksFor_protonWork(mocks, TEST_HAPPY_PATH);
                        if (whichPath == 6)
                        {
                            STRICT_EXPECTED_CALL(mocks, pn_messenger_status(TEST_PN_MESSENGER, TEST_PN_TRACKER)).SetReturn(PN_STATUS_PENDING);
                            STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(CBS_DEFAULT_REPLY_TIME+10);
                            ;
                        }
                        else
                        {
                            if (whichPath == 7)
                            {
                                STRICT_EXPECTED_CALL(mocks, pn_messenger_status(TEST_PN_MESSENGER, TEST_PN_TRACKER)).SetReturn(PN_STATUS_REJECTED);
                                STRICT_EXPECTED_CALL(mocks, pn_messenger_error(TEST_PN_MESSENGER)).SetReturn(protonErrorHandle);
                                STRICT_EXPECTED_CALL(mocks, pn_error_text(protonErrorHandle)).SetReturn("error text");
                            }
                            else
                            {
                                STRICT_EXPECTED_CALL(mocks, pn_messenger_status(TEST_PN_MESSENGER, TEST_PN_TRACKER)).SetReturn(PN_STATUS_ACCEPTED);
                                STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(0);

                                if (whichPath == PUT_TOKEN_PATH_THROUGH_PROCESS_RECEIVES)
                                {
                                    mocksFor_processReceives(mocks, 5);
                                }
                                else
                                {
                                    mocksFor_processReceives(mocks, 5);
                                    mocksFor_processCBS(mocks, TEST_HAPPY_PATH);
                                }
                                ;
                            }
                        }
                    }
                }
            }
        }
    }
    STRICT_EXPECTED_CALL(mocks, pn_message_clear(TEST_PN_MESSAGE));
}

static void mocksFor_putSecondAfterFirst(CIoTHubTransportAMQPMocks &mocks)
{
    (void)mocks;
    STRICT_EXPECTED_CALL(mocks, DList_AppendTailList(IGNORED_PTR_ARG, IGNORED_PTR_ARG)).IgnoreAllArguments();
    STRICT_EXPECTED_CALL(mocks, DList_RemoveEntryList(IGNORED_PTR_ARG)).IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, DList_InitializeListHead(IGNORED_PTR_ARG)).IgnoreArgument(1);
}

static void mocksFor_rollbackEvent(CIoTHubTransportAMQPMocks &mocks, size_t whichPath)
{
    (void)mocks;

    if (whichPath == 1)
    {
        STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1);
    }
    else
    {
        STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1); // First time through loop
        STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1); // Second time through loop
        STRICT_EXPECTED_CALL(mocks, pn_messenger_settle(TEST_PN_MESSENGER, TEST_PN_TRACKER, 0)).SetReturn(0);
        STRICT_EXPECTED_CALL(mocks, DList_RemoveEntryList(IGNORED_PTR_ARG)).IgnoreAllArguments();
        STRICT_EXPECTED_CALL(mocks, DList_InsertTailList(IGNORED_PTR_ARG, IGNORED_PTR_ARG)).IgnoreAllArguments();
        if (whichPath == 2) // for things still pending
        {
            STRICT_EXPECTED_CALL(mocks, pn_messenger_status(TEST_PN_MESSENGER, TEST_PN_TRACKER)).SetReturn(PN_STATUS_PENDING);
            STRICT_EXPECTED_CALL(mocks, DList_AppendTailList(IGNORED_PTR_ARG, IGNORED_PTR_ARG)).IgnoreAllArguments();
            STRICT_EXPECTED_CALL(mocks, DList_RemoveEntryList(IGNORED_PTR_ARG)).IgnoreAllArguments();
            STRICT_EXPECTED_CALL(mocks, DList_InitializeListHead(IGNORED_PTR_ARG)).IgnoreAllArguments();
        }
        else // for things done.
        {
            STRICT_EXPECTED_CALL(mocks, pn_messenger_status(TEST_PN_MESSENGER, TEST_PN_TRACKER)).SetReturn(PN_STATUS_ACCEPTED);
            STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_SendComplete(TEST_IOTHUB_CLIENT_LL_HANDLE, IGNORED_PTR_ARG, IOTHUB_BATCHSTATE_SUCCESS)).IgnoreArgument(2); // Complete the in progress one.
        }
    }
}
static void setupAmqpPrepareBatch(CIoTHubTransportAMQPMocks &mocks, size_t whichPath, IOTHUB_MESSAGE_HANDLE handle = TEST_IOTHUB_MESSAGE_HANDLE)
{
    (void)mocks;

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, DList_InitializeListHead(IGNORED_PTR_ARG)).IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, DList_RemoveHeadList(IGNORED_PTR_ARG)).IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, DList_InsertTailList(IGNORED_PTR_ARG, IGNORED_PTR_ARG)).IgnoreAllArguments();
    if (whichPath == 1) // Fail the IoTHubMessage with a bad status
    {
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetContentType(handle));
        if (handle != TEST_MESSAGE_HANDLE_STRING)
        {
            STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetByteArray(handle, IGNORED_PTR_ARG, IGNORED_PTR_ARG)).IgnoreArgument(2).IgnoreArgument(3).SetReturn(IOTHUB_MESSAGE_INVALID_ARG);
        }
        else
        {
            STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetString(handle)).SetReturn((const char*)NULL);
        }
        STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_SendComplete(TEST_IOTHUB_CLIENT_LL_HANDLE, IGNORED_PTR_ARG, IOTHUB_BATCHSTATE_FAILED)).IgnoreArgument(2);
        STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1);
    }
    else
    {
        if (whichPath == 2)
        {
            STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetContentType(handle));
            if (handle != TEST_MESSAGE_HANDLE_STRING)
            {
                STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetByteArray(handle, IGNORED_PTR_ARG, IGNORED_PTR_ARG)).IgnoreArgument(2).IgnoreArgument(3);
            }
            else
            {
                STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetString(handle));
            }

            STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_SendComplete(TEST_IOTHUB_CLIENT_LL_HANDLE, IGNORED_PTR_ARG, IOTHUB_BATCHSTATE_FAILED)).IgnoreArgument(2);
            STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1);
            STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1);
        }
        else
        {
            STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetContentType(handle));
            if (handle != TEST_MESSAGE_HANDLE_STRING)
            {
                STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetByteArray(handle, IGNORED_PTR_ARG, IGNORED_PTR_ARG)).IgnoreArgument(2).IgnoreArgument(3);
            }
            else
            {
                STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetString(handle));
            }
        }
    }
}

static void setupAmqpPrepareSimpleProtonMessage(CIoTHubTransportAMQPMocks &mocks, size_t whichPath)
{
    (void)mocks;
    STRICT_EXPECTED_CALL(mocks, pn_message_clear(TEST_PN_MESSAGE));
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(eventAmqpsStringHandle));
    if (whichPath == 1)
    {
        STRICT_EXPECTED_CALL(mocks, pn_message_set_address(TEST_PN_MESSAGE, IGNORED_PTR_ARG)).IgnoreArgument(2).SetReturn(1);
        mocksFor_putSecondAfterFirst(mocks);
    }
    else
    {
        STRICT_EXPECTED_CALL(mocks, pn_message_set_address(TEST_PN_MESSAGE, IGNORED_PTR_ARG)).IgnoreArgument(2);
        if (whichPath == 2)
        {
            STRICT_EXPECTED_CALL(mocks, pn_message_set_inferred(TEST_PN_MESSAGE, true)).SetReturn(1);
            mocksFor_putSecondAfterFirst(mocks);
        }
        else
        {
            STRICT_EXPECTED_CALL(mocks, pn_message_set_inferred(TEST_PN_MESSAGE, true));
            if (whichPath == 3)
            {
                STRICT_EXPECTED_CALL(mocks, pn_message_body(TEST_PN_MESSAGE)).SetReturn(TEST_NULL_PN_DATA);
                mocksFor_putSecondAfterFirst(mocks);
            }
            else
            {
                STRICT_EXPECTED_CALL(mocks, pn_message_body(TEST_PN_MESSAGE));
                STRICT_EXPECTED_CALL(mocks, pn_bytes(sizeof(TEXT_MESSAGE), (const char*)TEXT_MESSAGE)).SetReturn(test_pn_bytes);
                if (whichPath == 4)
                {
                    STRICT_EXPECTED_CALL(mocks, pn_data_put_binary(TEST_PN_DATA, test_pn_bytes)).SetReturn(1);
                    mocksFor_putSecondAfterFirst(mocks);
                }
                else
                {
                    STRICT_EXPECTED_CALL(mocks, pn_data_put_binary(TEST_PN_DATA, test_pn_bytes));
                }
            }
        }
    }
}

static void mocksFor_processCBS(CIoTHubTransportAMQPMocks &mocks, size_t whichPath)
{
    (void)mocks;
    bool goodMessage = false;
    if (whichPath == 1)
    {
        pn_atom_t bad_correlation;
        bad_correlation.u.as_ulong = (size_t)~0;
        STRICT_EXPECTED_CALL(mocks, pn_message_get_correlation_id(TEST_PN_MESSAGE)).SetReturn(bad_correlation);
    }
    else
    {
        pn_atom_t good_correlation;
        good_correlation.u.as_ulong = TEST_EXPIRY_TEST_VALUE;
        STRICT_EXPECTED_CALL(mocks, pn_message_get_correlation_id(TEST_PN_MESSAGE)).SetReturn(good_correlation);
        if (whichPath == 2)
        {
            STRICT_EXPECTED_CALL(mocks, pn_message_properties(TEST_PN_MESSAGE)).SetReturn(TEST_NULL_PN_DATA);
        }
        else
        {
            STRICT_EXPECTED_CALL(mocks, pn_message_properties(TEST_PN_MESSAGE)).SetReturn(TEST_PN_DATA);
            if (whichPath == 3)
            {
                STRICT_EXPECTED_CALL(mocks, pn_data_next(TEST_PN_DATA)).SetReturn(false);
            }
            else
            {
                STRICT_EXPECTED_CALL(mocks, pn_data_next(TEST_PN_DATA)).SetReturn(true);
                if (whichPath == 4)
                {
                    STRICT_EXPECTED_CALL(mocks, pn_data_get_map(TEST_PN_DATA)).SetReturn(1);
                    //
                    // In the error case we call it again so that we can log the number of properties returned.
                    //
                    STRICT_EXPECTED_CALL(mocks, pn_data_get_map(TEST_PN_DATA)).SetReturn(1);
                }
                else
                {
                    STRICT_EXPECTED_CALL(mocks, pn_data_get_map(TEST_PN_DATA)).SetReturn(2);
                    if (whichPath == 5)
                    {
                        STRICT_EXPECTED_CALL(mocks, pn_data_enter(TEST_PN_DATA)).SetReturn(false);
                    }
                    else
                    {
                        STRICT_EXPECTED_CALL(mocks, pn_data_enter(TEST_PN_DATA)).SetReturn(true);
                        if (whichPath == 6) // Fail the getting of the property name for the status code.  Do this by failing the data next.
                        {
                            STRICT_EXPECTED_CALL(mocks, pn_data_next(TEST_PN_DATA)).SetReturn(false);
                        }
                        else
                        {
                            STRICT_EXPECTED_CALL(mocks, pn_data_next(TEST_PN_DATA)).SetReturn(true);
                            STRICT_EXPECTED_CALL(mocks, pn_data_type(TEST_PN_DATA)).SetReturn(PN_STRING);
                            if (whichPath == 7) // Fail by returning an pointer to a string that ISN't the status code.
                            {
                                STRICT_EXPECTED_CALL(mocks, pn_data_get_string(TEST_PN_DATA)).SetReturn(test_pn_bytes_incorrect_status_name);
                            }
                            else
                            {
                                STRICT_EXPECTED_CALL(mocks, pn_data_get_string(TEST_PN_DATA)).SetReturn(test_pn_bytes_correct_status_name);
                                if (whichPath == 8) // Fail the GetMapInt
                                {
                                    STRICT_EXPECTED_CALL(mocks, pn_data_next(TEST_PN_DATA)).SetReturn(false);
                                }
                                else
                                {
                                    STRICT_EXPECTED_CALL(mocks, pn_data_next(TEST_PN_DATA)).SetReturn(true);
                                    STRICT_EXPECTED_CALL(mocks, pn_data_type(TEST_PN_DATA)).SetReturn(PN_INT);
                                    goodMessage = true;
                                    if (whichPath == 9)
                                    {
                                        STRICT_EXPECTED_CALL(mocks, pn_data_get_int(TEST_PN_DATA)).SetReturn(500);
                                    }
                                    else
                                    {
                                        STRICT_EXPECTED_CALL(mocks, pn_data_get_int(TEST_PN_DATA)).SetReturn(200);
                                    }
                                }
                            }
                        }

                    }
                }
            }
        }
    }
    if (goodMessage == true)
    {
        STRICT_EXPECTED_CALL(mocks, pn_messenger_accept(TEST_PN_MESSENGER, TEST_PN_TRACKER, 0));
    }
    else
    {
        STRICT_EXPECTED_CALL(mocks, pn_messenger_release(TEST_PN_MESSENGER, TEST_PN_TRACKER, 0));

    }
    STRICT_EXPECTED_CALL(mocks, pn_messenger_settle(TEST_PN_MESSENGER, TEST_PN_TRACKER, 0));
}

static void mocksFor_NoTelementry(CIoTHubTransportAMQPMocks &mocks)
{
    (void)mocks;

    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1);
}

static void mocksFor_processReceives(CIoTHubTransportAMQPMocks &mocks, size_t whichPath)
{
    (void)mocks;

    if (whichPath == 1)
    {
        // The no subscription path.
        STRICT_EXPECTED_CALL(mocks, pn_messenger_recv(TEST_PN_MESSENGER, 0)).SetReturn(0);
    }
    else
    {
        if (whichPath == 11) // fails the receive.
        {
            STRICT_EXPECTED_CALL(mocks, pn_messenger_recv(TEST_PN_MESSENGER, 0)).SetReturn(PN_STATE_ERR);
        }
        else
        {
            if (whichPath == 22) // Fails the recevie
            {
                STRICT_EXPECTED_CALL(mocks, pn_messenger_recv(TEST_PN_MESSENGER, PROTON_INCOMING_WINDOW_SIZE)).SetReturn(PN_STATE_ERR);
            }
            else
            {
                STRICT_EXPECTED_CALL(mocks, pn_messenger_recv(TEST_PN_MESSENGER, PROTON_INCOMING_WINDOW_SIZE));
                if (whichPath == 2) // No messages received.
                {
                    STRICT_EXPECTED_CALL(mocks, pn_messenger_incoming(TEST_PN_MESSENGER)).SetReturn(0);
                }
                else
                {
                    STRICT_EXPECTED_CALL(mocks, pn_messenger_incoming(TEST_PN_MESSENGER)).SetReturn(1);
                    STRICT_EXPECTED_CALL(mocks, pn_message_clear(TEST_PN_MESSAGE));
                    if (whichPath == 3) // The get fails.
                    {
                        STRICT_EXPECTED_CALL(mocks, pn_messenger_get(TEST_PN_MESSENGER, TEST_PN_MESSAGE)).SetReturn(1);
                    }
                    else
                    {
                        STRICT_EXPECTED_CALL(mocks, pn_messenger_get(TEST_PN_MESSENGER, TEST_PN_MESSAGE));
                        STRICT_EXPECTED_CALL(mocks, pn_messenger_incoming_tracker(TEST_PN_MESSENGER));
                        if (whichPath == 4) // Do a message path
                        {
                            STRICT_EXPECTED_CALL(mocks, pn_messenger_incoming_subscription(TEST_PN_MESSENGER)).SetReturn(messageSubscriptionValue);
                        }
                        else if (whichPath == 5) // Do a CBS path
                        {
                            STRICT_EXPECTED_CALL(mocks, pn_messenger_incoming_subscription(TEST_PN_MESSENGER)).SetReturn(cbsSubscriptionValue);
                        }
                        else if (whichPath == 6) // Do an unknown path
                        {
                            STRICT_EXPECTED_CALL(mocks, pn_messenger_incoming_subscription(TEST_PN_MESSENGER)).SetReturn(TEST_NULL_PN_SUBSCRIPTION);
                        }
                    }
                }
                STRICT_EXPECTED_CALL(mocks, pn_message_clear(TEST_PN_MESSAGE));
            }
        }
    }
}

static void mocksFor_processMessages(CIoTHubTransportAMQPMocks &mocks, size_t whichPath, bool nullPath)
{
    (void)mocks;

    if (whichPath == 111)
    {
        STRICT_EXPECTED_CALL(mocks, pn_messenger_reject(TEST_PN_MESSENGER, TEST_PN_TRACKER, 0));
        STRICT_EXPECTED_CALL(mocks, pn_messenger_settle(TEST_PN_MESSENGER, TEST_PN_TRACKER, 0));
    }
    else if (whichPath == 1) // Getting the body failed.
    {
        STRICT_EXPECTED_CALL(mocks, pn_message_body(TEST_PN_MESSAGE)).SetReturn(TEST_NULL_PN_DATA);
        STRICT_EXPECTED_CALL(mocks, pn_messenger_release(TEST_PN_MESSENGER, TEST_PN_TRACKER, 0));
        STRICT_EXPECTED_CALL(mocks, pn_messenger_settle(TEST_PN_MESSENGER, TEST_PN_TRACKER, 0));
    }
    else
    {
        STRICT_EXPECTED_CALL(mocks, pn_message_body(TEST_PN_MESSAGE));
        if (whichPath == 3)
        {
            EXPECTED_CALL(mocks, pn_data_next(IGNORED_PTR_ARG)).SetReturn(true);
            STRICT_EXPECTED_CALL(mocks, pn_data_type(TEST_PN_DATA)).SetReturn(PN_BYTE);
            STRICT_EXPECTED_CALL(mocks, pn_messenger_release(TEST_PN_MESSENGER, TEST_PN_TRACKER, 0));
            STRICT_EXPECTED_CALL(mocks, pn_messenger_settle(TEST_PN_MESSENGER, TEST_PN_TRACKER, 0));

        }
        else
        {
            size_t lengthOfData;
            const char* dataPointer;
            if (nullPath == true) // null message body.
            {
                EXPECTED_CALL(mocks, pn_data_next(IGNORED_PTR_ARG)).SetReturn(false);
                lengthOfData = 0;
                dataPointer = NULL;
            }
            else
            {
                lengthOfData = TEST_PN_GOOD_BYTES.size;
                dataPointer = TEST_PN_GOOD_BYTES.start;
                EXPECTED_CALL(mocks, pn_data_next(IGNORED_PTR_ARG)).SetReturn(true);
                STRICT_EXPECTED_CALL(mocks, pn_data_type(TEST_PN_DATA)).SetReturn(PN_BINARY);
                EXPECTED_CALL(mocks, pn_data_get_binary(TEST_PN_DATA)).SetReturn(TEST_PN_GOOD_BYTES);
            }
            if (whichPath == 4)
            {
                STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Destroy(TEST_NULL_IOTHUB_MESSAGE_HANDLE));
                STRICT_EXPECTED_CALL(mocks, IoTHubMessage_CreateFromByteArray((const unsigned char*)dataPointer, lengthOfData)).SetReturn(TEST_NULL_IOTHUB_MESSAGE_HANDLE);
                STRICT_EXPECTED_CALL(mocks, pn_messenger_release(TEST_PN_MESSENGER, TEST_PN_TRACKER, 0));
                STRICT_EXPECTED_CALL(mocks, pn_messenger_settle(TEST_PN_MESSENGER, TEST_PN_TRACKER, 0));
            }
            else if (whichPath == 5)
            {
                STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Destroy(TEST_IOTHUB_MESSAGE_HANDLE));
                STRICT_EXPECTED_CALL(mocks, IoTHubMessage_CreateFromByteArray((const unsigned char*)dataPointer, lengthOfData)).SetReturn(TEST_IOTHUB_MESSAGE_HANDLE);
                STRICT_EXPECTED_CALL(mocks, pn_messenger_release(TEST_PN_MESSENGER, TEST_PN_TRACKER, 0));
                STRICT_EXPECTED_CALL(mocks, pn_messenger_settle(TEST_PN_MESSENGER, TEST_PN_TRACKER, 0));
            }
            else
            {
                STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Destroy(TEST_IOTHUB_MESSAGE_HANDLE));
                STRICT_EXPECTED_CALL(mocks, IoTHubMessage_CreateFromByteArray((const unsigned char*)dataPointer, lengthOfData)).SetReturn(TEST_IOTHUB_MESSAGE_HANDLE);
                STRICT_EXPECTED_CALL(mocks, pn_messenger_settle(TEST_PN_MESSENGER, TEST_PN_TRACKER, 0));
                switch (whichPath)
                {
                    case 8:
                    {
                        STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_MessageCallback(TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_IOTHUB_MESSAGE_HANDLE)).SetReturn(IOTHUBMESSAGE_ACCEPTED);
                        STRICT_EXPECTED_CALL(mocks, pn_messenger_accept(TEST_PN_MESSENGER, TEST_PN_TRACKER, 0));
                        break;
                    }
                    case 10:
                    {
                        STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_MessageCallback(TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_IOTHUB_MESSAGE_HANDLE)).SetReturn(IOTHUBMESSAGE_REJECTED);
                        STRICT_EXPECTED_CALL(mocks, pn_messenger_reject(TEST_PN_MESSENGER, TEST_PN_TRACKER, 0));
                        break;
                    }
                    case 11:
                    {
                        STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_MessageCallback(TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_IOTHUB_MESSAGE_HANDLE)).SetReturn(IOTHUBMESSAGE_ABANDONED);
                        STRICT_EXPECTED_CALL(mocks, pn_messenger_release(TEST_PN_MESSENGER, TEST_PN_TRACKER, 0));
                        break;
                    }
                    case 12:
                    {
                        STRICT_EXPECTED_CALL(mocks, pn_messenger_release(TEST_PN_MESSENGER, TEST_PN_TRACKER, 0));
                        break;
                    }
                    default:
                    {
                        ASSERT_FAIL("default case reached");
                    }
                }
            }
        }
    }
}

static void setupAmqpMessengerStop(CIoTHubTransportAMQPMocks &mocks, size_t whichPath)
{
    (void)mocks;
    if (whichPath == 1)
    {
        STRICT_EXPECTED_CALL(mocks, pn_messenger_stop(IGNORED_PTR_ARG)).IgnoreArgument(1).SetReturn(PN_ARG_ERR);
    }
    else
    {
        if (whichPath == 2)
        {
            STRICT_EXPECTED_CALL(mocks, pn_messenger_stop(IGNORED_PTR_ARG)).IgnoreArgument(1).SetReturn(0); // The messenger was stopped!
        }
        else
        {
            size_t invokeStopped;
            STRICT_EXPECTED_CALL(mocks, pn_messenger_stop(IGNORED_PTR_ARG)).IgnoreArgument(1).SetReturn(PN_INPROGRESS);
            /*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_024: [If pn_messenger_stop returns PN_INPROGRESS invoke pn_messenger_stopped a fixed number of time, seeking a return of true.]*/
            for (invokeStopped = 3; invokeStopped < TEST_NUMBER_OF_STOPPEDS + 3; invokeStopped++)
            {
                STRICT_EXPECTED_CALL(mocks, pn_messenger_work(TEST_PN_MESSENGER, 100));
                if (whichPath == invokeStopped)
                {
                    STRICT_EXPECTED_CALL(mocks, pn_messenger_stopped(IGNORED_PTR_ARG)).IgnoreArgument(1).SetReturn(true);
                    break;
                }
                else
                {
                    STRICT_EXPECTED_CALL(mocks, pn_messenger_stopped(IGNORED_PTR_ARG)).IgnoreArgument(1).SetReturn(false);
                }
            }
        }
    }
}

static void setupAmqpMessengerDispose(CIoTHubTransportAMQPMocks &mocks, size_t whichPath)
{
    (void)mocks;
    if (whichPath == 1) // Have pn_messenger_stop error out
    {
        /*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_023: [If pn_messenger_stop returns any result other than PN_INPROGRESS then place the messenger on a list for attempting to stop later.]*/
        setupAmqpMessengerStop(mocks, 1);
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(0)).IgnoreArgument(1); // Allocate the container.
        STRICT_EXPECTED_CALL(mocks, DList_InsertTailList(IGNORED_PTR_ARG, IGNORED_PTR_ARG)).IgnoreAllArguments();
    }
    else
    {
        if (whichPath == 2) // Have the messenger stop! Yay!
        {
            setupAmqpMessengerStop(mocks, 2);
            /*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_022: [If pn_messenger_stop returns 0 then invoke pn_messenger_free on the messenger.]*/
            /*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_025: [If pn_messenger_stopped returns true, then call pn_messenger_free.]*/
            STRICT_EXPECTED_CALL(mocks, pn_messenger_free(IGNORED_PTR_ARG)).IgnoreArgument(1);
        }
        else
        {
            if (whichPath != TEST_HAPPY_PATH)
            {
                setupAmqpMessengerStop(mocks, whichPath);
            }
            else
            {
                setupAmqpMessengerStop(mocks, TEST_HAPPY_PATH); // Don't stop it.
                /*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_026: [If pn_messenger_stopped never returns true, then place the messenger on a list for attempting to stop later.]*/
                STRICT_EXPECTED_CALL(mocks, gballoc_malloc(0)).IgnoreArgument(1); // Allocate the container.
                STRICT_EXPECTED_CALL(mocks, DList_InsertTailList(IGNORED_PTR_ARG, IGNORED_PTR_ARG)).IgnoreAllArguments();
            }
        }
    }
}

static void mocksFor_initialCreate(CIoTHubTransportAMQPMocks &mocks, size_t whereToFail)
{
    (void)mocks;
    if (whereToFail == 1)
    {
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(0)).IgnoreArgument(1);
    }
    else
    {
        size_t i;
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(0)).IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, DList_InitializeListHead(IGNORED_PTR_ARG)).IgnoreArgument(1); // work in progress
        STRICT_EXPECTED_CALL(mocks, DList_InitializeListHead(IGNORED_PTR_ARG)).IgnoreArgument(1); // available for work
        STRICT_EXPECTED_CALL(mocks, DList_InitializeListHead(IGNORED_PTR_ARG)).IgnoreArgument(1); // messenger corral
        if (whereToFail == 2)
        {
            STRICT_EXPECTED_CALL(mocks, pn_message()).SetReturn(TEST_NULL_PN_MESSAGE);
        }
        else
        {
            STRICT_EXPECTED_CALL(mocks, pn_message());
            for (i = 3; i < (PROTON_EVENT_OUTGOING_WINDOW_SIZE + 3); i++)
            {
                if (whereToFail == i)
                {
                    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(0)).IgnoreArgument(1);
                    break;
                }
                else
                {
                    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(0)).IgnoreArgument(1);
                    STRICT_EXPECTED_CALL(mocks, DList_InsertTailList(IGNORED_PTR_ARG, IGNORED_PTR_ARG)).IgnoreAllArguments();
                }
            }
        }
    }
}

static void mocksFor_createHost(CIoTHubTransportAMQPMocks &mocks, size_t whereToFail, IOTHUBTRANSPORT_CONFIG* tconfig)
{

    (mocks);
    if (whereToFail == 1)
    {
        STRICT_EXPECTED_CALL(mocks, STRING_construct(tconfig->upperConfig->iotHubName)).SetReturn(TEST_NULL_STRING_HANDLE);
    }
    else
    {
        STRICT_EXPECTED_CALL(mocks, STRING_construct(tconfig->upperConfig->iotHubName)).SetReturn(hostStringHandle);
        STRICT_EXPECTED_CALL(mocks, STRING_delete(hostStringHandle));
        if (whereToFail == 2)
        {
            STRICT_EXPECTED_CALL(mocks, STRING_concat(hostStringHandle, ".")).SetReturn(1);
        }
        else
        {
            STRICT_EXPECTED_CALL(mocks, STRING_concat(hostStringHandle, "."));
            if (whereToFail == 3)
            {
                STRICT_EXPECTED_CALL(mocks, STRING_concat(hostStringHandle, tconfig->upperConfig->iotHubSuffix)).SetReturn(1);
            }
            else
            {
                STRICT_EXPECTED_CALL(mocks, STRING_concat(hostStringHandle, tconfig->upperConfig->iotHubSuffix));
            }
        }
    }
}

static void mocksFor_createVariousStrings(CIoTHubTransportAMQPMocks &mocks, size_t whereToFail, IOTHUBTRANSPORT_CONFIG* tconfig)
{
    (mocks);
    if (whereToFail == 1) // initial device id constructions via url encoding
    {
        STRICT_EXPECTED_CALL(mocks, URL_EncodeString(tconfig->upperConfig->deviceId)).SetReturn(STRING_HANDLE_NULL);
        STRICT_EXPECTED_CALL(mocks, STRING_delete(STRING_HANDLE_NULL)); //deviceId
        STRICT_EXPECTED_CALL(mocks, STRING_delete(STRING_HANDLE_NULL)); //path
        STRICT_EXPECTED_CALL(mocks, STRING_delete(STRING_HANDLE_NULL)); //event
        STRICT_EXPECTED_CALL(mocks, STRING_delete(STRING_HANDLE_NULL)); //message
        STRICT_EXPECTED_CALL(mocks, STRING_delete(STRING_HANDLE_NULL)); //cbs
        STRICT_EXPECTED_CALL(mocks, STRING_delete(STRING_HANDLE_NULL)); //device key
        STRICT_EXPECTED_CALL(mocks, STRING_delete(STRING_HANDLE_NULL)); //zero length string
    }
    else
    {
        STRICT_EXPECTED_CALL(mocks, URL_EncodeString(tconfig->upperConfig->deviceId)).SetReturn(urledDeviceIdStringHandle);
        STRICT_EXPECTED_CALL(mocks, STRING_delete(urledDeviceIdStringHandle));
        STRICT_EXPECTED_CALL(mocks, STRING_c_str(hostStringHandle)).SetReturn(TEST_HOST);
        if (whereToFail == 2) // initial devices path portion
        {
            STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_HOST)).SetReturn(STRING_HANDLE_NULL);
            STRICT_EXPECTED_CALL(mocks, STRING_delete(STRING_HANDLE_NULL)); //path 
            STRICT_EXPECTED_CALL(mocks, STRING_delete(STRING_HANDLE_NULL)); //event 
            STRICT_EXPECTED_CALL(mocks, STRING_delete(STRING_HANDLE_NULL)); //message 
            STRICT_EXPECTED_CALL(mocks, STRING_delete(STRING_HANDLE_NULL)); //cbs 
            STRICT_EXPECTED_CALL(mocks, STRING_delete(STRING_HANDLE_NULL)); //device key
            STRICT_EXPECTED_CALL(mocks, STRING_delete(STRING_HANDLE_NULL)); //zero length string
        }
        else
        {
            STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_HOST)).SetReturn(devicesPortionPathStringHandle);
            STRICT_EXPECTED_CALL(mocks, STRING_delete(devicesPortionPathStringHandle));
            if (whereToFail == 3) // remainder of devices portions path with the actual device id
            {
                STRICT_EXPECTED_CALL(mocks, STRING_concat(devicesPortionPathStringHandle, "/devices/")).SetReturn(1);
                STRICT_EXPECTED_CALL(mocks, STRING_delete(STRING_HANDLE_NULL)); //event
                STRICT_EXPECTED_CALL(mocks, STRING_delete(STRING_HANDLE_NULL)); //message
                STRICT_EXPECTED_CALL(mocks, STRING_delete(STRING_HANDLE_NULL)); //cbs
                STRICT_EXPECTED_CALL(mocks, STRING_delete(STRING_HANDLE_NULL)); //device key
                STRICT_EXPECTED_CALL(mocks, STRING_delete(STRING_HANDLE_NULL)); //zero length string
            }
            else
            {
                STRICT_EXPECTED_CALL(mocks, STRING_concat(devicesPortionPathStringHandle, "/devices/"));
                if (whereToFail == 4)
                {
                    STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(devicesPortionPathStringHandle, urledDeviceIdStringHandle)).SetReturn(1);
                    STRICT_EXPECTED_CALL(mocks, STRING_delete(STRING_HANDLE_NULL)); //event
                    STRICT_EXPECTED_CALL(mocks, STRING_delete(STRING_HANDLE_NULL)); //message
                    STRICT_EXPECTED_CALL(mocks, STRING_delete(STRING_HANDLE_NULL)); //cbs
                    STRICT_EXPECTED_CALL(mocks, STRING_delete(STRING_HANDLE_NULL)); //device key
                    STRICT_EXPECTED_CALL(mocks, STRING_delete(STRING_HANDLE_NULL)); //zero length string
                }
                else
                {
                    STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(devicesPortionPathStringHandle, urledDeviceIdStringHandle)).SetReturn(0);
                    if (whereToFail == 5) // initial part of event address with the scheme and host string
                    {
                        STRICT_EXPECTED_CALL(mocks, STRING_construct(AMQPS_SCHEME)).SetReturn(STRING_HANDLE_NULL);
                        STRICT_EXPECTED_CALL(mocks, STRING_delete(STRING_HANDLE_NULL)); //event
                        STRICT_EXPECTED_CALL(mocks, STRING_delete(STRING_HANDLE_NULL)); //message
                        STRICT_EXPECTED_CALL(mocks, STRING_delete(STRING_HANDLE_NULL)); //cbs
                        STRICT_EXPECTED_CALL(mocks, STRING_delete(STRING_HANDLE_NULL)); //device key
                        STRICT_EXPECTED_CALL(mocks, STRING_delete(STRING_HANDLE_NULL)); //zero length string
                    }
                    else
                    {
                        STRICT_EXPECTED_CALL(mocks, STRING_construct(AMQPS_SCHEME)).SetReturn(eventAmqpsStringHandle);
                        STRICT_EXPECTED_CALL(mocks, STRING_delete(eventAmqpsStringHandle));
                        if (whereToFail == 6) // next part with the devices path portion
                        {
                            STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(eventAmqpsStringHandle, devicesPortionPathStringHandle)).SetReturn(1);
                            STRICT_EXPECTED_CALL(mocks, STRING_delete(STRING_HANDLE_NULL)); //message
                            STRICT_EXPECTED_CALL(mocks, STRING_delete(STRING_HANDLE_NULL)); //cbs
                            STRICT_EXPECTED_CALL(mocks, STRING_delete(STRING_HANDLE_NULL)); //device key
                            STRICT_EXPECTED_CALL(mocks, STRING_delete(STRING_HANDLE_NULL)); //zero length string
                        }
                        else
                        {
                            STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(eventAmqpsStringHandle, devicesPortionPathStringHandle));
                            if (whereToFail == 7) // Final part of the event path with the actual endpoint
                            {
                                STRICT_EXPECTED_CALL(mocks, STRING_concat(eventAmqpsStringHandle, EVENT_ENDPOINT)).SetReturn(1);
                                STRICT_EXPECTED_CALL(mocks, STRING_delete(STRING_HANDLE_NULL)); //message
                                STRICT_EXPECTED_CALL(mocks, STRING_delete(STRING_HANDLE_NULL)); //cbs
                                STRICT_EXPECTED_CALL(mocks, STRING_delete(STRING_HANDLE_NULL)); //device key
                                STRICT_EXPECTED_CALL(mocks, STRING_delete(STRING_HANDLE_NULL)); //zero length string
                            }
                            else
                            {
                                STRICT_EXPECTED_CALL(mocks, STRING_concat(eventAmqpsStringHandle, EVENT_ENDPOINT));
                                if (whereToFail == 8) // initial part of the message address with the scheme and host
                                {
                                    STRICT_EXPECTED_CALL(mocks, STRING_construct(AMQPS_SCHEME)).SetReturn(STRING_HANDLE_NULL);
                                    STRICT_EXPECTED_CALL(mocks, STRING_delete(STRING_HANDLE_NULL)); //message
                                    STRICT_EXPECTED_CALL(mocks, STRING_delete(STRING_HANDLE_NULL)); //cbs
                                    STRICT_EXPECTED_CALL(mocks, STRING_delete(STRING_HANDLE_NULL)); //device key
                                    STRICT_EXPECTED_CALL(mocks, STRING_delete(STRING_HANDLE_NULL)); //zero length string
                                }
                                else
                                {
                                    STRICT_EXPECTED_CALL(mocks, STRING_construct(AMQPS_SCHEME)).SetReturn(messageAmqpsStringHandle);
                                    STRICT_EXPECTED_CALL(mocks, STRING_delete(messageAmqpsStringHandle));
                                    if (whereToFail == 9) // Next part of the message with the devicdes portion
                                    {
                                        STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(messageAmqpsStringHandle, devicesPortionPathStringHandle)).SetReturn(1);
                                        STRICT_EXPECTED_CALL(mocks, STRING_delete(STRING_HANDLE_NULL)); //cbs
                                        STRICT_EXPECTED_CALL(mocks, STRING_delete(STRING_HANDLE_NULL)); //device key
                                        STRICT_EXPECTED_CALL(mocks, STRING_delete(STRING_HANDLE_NULL)); //zero length string
                                    }
                                    else
                                    {
                                        STRICT_EXPECTED_CALL(mocks, STRING_concat_with_STRING(messageAmqpsStringHandle, devicesPortionPathStringHandle));
                                        if (whereToFail == 10) // Final part of the message with the message endpoint
                                        {
                                            STRICT_EXPECTED_CALL(mocks, STRING_concat(messageAmqpsStringHandle, MESSAGE_ENDPOINT)).SetReturn(1);
                                            STRICT_EXPECTED_CALL(mocks, STRING_delete(STRING_HANDLE_NULL)); //cbs
                                            STRICT_EXPECTED_CALL(mocks, STRING_delete(STRING_HANDLE_NULL)); //device key
                                            STRICT_EXPECTED_CALL(mocks, STRING_delete(STRING_HANDLE_NULL)); //zero length string
                                        }
                                        else
                                        {
                                            STRICT_EXPECTED_CALL(mocks, STRING_concat(messageAmqpsStringHandle, MESSAGE_ENDPOINT));
                                            STRICT_EXPECTED_CALL(mocks, STRING_c_str(hostStringHandle)).SetReturn(TEST_HOST);
                                            if (whereToFail == 11) // Initial part of the construction of the cbs address with the scheme and host
                                            {
                                                STRICT_EXPECTED_CALL(mocks, STRING_construct(AMQPS_SCHEME)).SetReturn(STRING_HANDLE_NULL);
                                                STRICT_EXPECTED_CALL(mocks, STRING_delete(STRING_HANDLE_NULL)); //cbs
                                                STRICT_EXPECTED_CALL(mocks, STRING_delete(STRING_HANDLE_NULL)); //device key
                                                STRICT_EXPECTED_CALL(mocks, STRING_delete(STRING_HANDLE_NULL)); //zero length string
                                            }
                                            else
                                            {
                                                STRICT_EXPECTED_CALL(mocks, STRING_construct(AMQPS_SCHEME)).SetReturn(cbsAmqpsStringHandle);
                                                STRICT_EXPECTED_CALL(mocks, STRING_delete(cbsAmqpsStringHandle));
                                                if (whereToFail == 12) // the host portion
                                                {
                                                    STRICT_EXPECTED_CALL(mocks, STRING_concat(cbsAmqpsStringHandle, TEST_HOST)).SetReturn(1);
                                                    STRICT_EXPECTED_CALL(mocks, STRING_delete(STRING_HANDLE_NULL)); //device key
                                                    STRICT_EXPECTED_CALL(mocks, STRING_delete(STRING_HANDLE_NULL)); //zero length string
                                                }
                                                else
                                                {
                                                    STRICT_EXPECTED_CALL(mocks, STRING_concat(cbsAmqpsStringHandle, TEST_HOST)).SetReturn(0);
                                                    if (whereToFail == 13)
                                                    {
                                                        STRICT_EXPECTED_CALL(mocks, STRING_concat(cbsAmqpsStringHandle, CBS_ENDPOINT)).SetReturn(1);
                                                        STRICT_EXPECTED_CALL(mocks, STRING_delete(STRING_HANDLE_NULL)); //device key
                                                        STRICT_EXPECTED_CALL(mocks, STRING_delete(STRING_HANDLE_NULL)); //zero length string
                                                    }
                                                    else
                                                    {
                                                        STRICT_EXPECTED_CALL(mocks, STRING_concat(cbsAmqpsStringHandle, CBS_ENDPOINT));
                                                        if (whereToFail == 14) // Construct the device key.
                                                        {
                                                            STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_DEVICE_KEY)).SetReturn(STRING_HANDLE_NULL);
                                                            STRICT_EXPECTED_CALL(mocks, STRING_delete(STRING_HANDLE_NULL)); //device key
                                                            STRICT_EXPECTED_CALL(mocks, STRING_delete(STRING_HANDLE_NULL)); //zero length string
                                                        }
                                                        else
                                                        {
                                                            STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_DEVICE_KEY)).SetReturn(deviceKeyStringHandle);
                                                            STRICT_EXPECTED_CALL(mocks, STRING_delete(deviceKeyStringHandle));
                                                            if (whereToFail == 15)
                                                            {
                                                                STRICT_EXPECTED_CALL(mocks, STRING_new()).SetReturn(STRING_HANDLE_NULL);
                                                                STRICT_EXPECTED_CALL(mocks, STRING_delete(STRING_HANDLE_NULL)); //zero Length String
                                                            }
                                                            else
                                                            {
                                                                STRICT_EXPECTED_CALL(mocks, STRING_new()).SetReturn(zeroLengthStringHandle);
                                                                STRICT_EXPECTED_CALL(mocks, STRING_delete(zeroLengthStringHandle));
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

static void mock_renewUpToProcessCBS(CIoTHubTransportAMQPMocks &mocks)
{
    // Renew call itself.
    (void)mocks;
    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(TEST_INITIAL_TIME_VALUE);
    STRICT_EXPECTED_CALL(mocks, SASToken_Create(deviceKeyStringHandle, devicesPortionPathStringHandle, zeroLengthStringHandle, TEST_EXPIRY_TEST_VALUE)).SetReturn(sasTokenStringHandle);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(sasTokenStringHandle));

    mocksFor_putToken(mocks, PUT_TOKEN_PATH_THROUGH_PROCESS_RECEIVES);
}

static void mock_renewHappyPath(CIoTHubTransportAMQPMocks &mocks)
{
    (void)mocks;
    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(TEST_INITIAL_TIME_VALUE);
    STRICT_EXPECTED_CALL(mocks, SASToken_Create(deviceKeyStringHandle, devicesPortionPathStringHandle, zeroLengthStringHandle, TEST_EXPIRY_TEST_VALUE)).SetReturn(sasTokenStringHandle);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(sasTokenStringHandle));
    mocksFor_putToken(mocks, PUT_TOKEN_PATH_THROUGH_PROCESS_CBS);
}

static void mocks_createHappyPath(CIoTHubTransportAMQPMocks &mocks, IOTHUBTRANSPORT_CONFIG* tconfig)
{
    (void)mocks;
    mocksFor_initialCreate(mocks, TEST_HAPPY_PATH);
    mocksFor_createHost(mocks, TEST_HAPPY_PATH, tconfig);
    mocksFor_createVariousStrings(mocks, TEST_HAPPY_PATH, tconfig);
}
static void proton_messenger_init_path(CIoTHubTransportAMQPMocks &mocks, size_t whichPath)
{
    (void)mocks;
    if (whichPath == 1)
    {
        STRICT_EXPECTED_CALL(mocks, pn_messenger(NULL)).SetReturn(TEST_NULL_PN_MESSENGER);
    }
    else
    {
        STRICT_EXPECTED_CALL(mocks, pn_messenger(NULL));
        if (whichPath == 2)
        {
            STRICT_EXPECTED_CALL(mocks, pn_messenger_start(TEST_PN_MESSENGER)).SetReturn(PN_ARG_ERR);
        }
        else
        {
            STRICT_EXPECTED_CALL(mocks, pn_messenger_start(TEST_PN_MESSENGER));
            if (whichPath == 3)
            {
                STRICT_EXPECTED_CALL(mocks, pn_messenger_set_blocking(TEST_PN_MESSENGER, false)).SetReturn(PN_ARG_ERR);
            }
            else
            {
                STRICT_EXPECTED_CALL(mocks, pn_messenger_set_blocking(TEST_PN_MESSENGER, false));
                if (whichPath == 4)
                {
                    STRICT_EXPECTED_CALL(mocks, pn_messenger_set_snd_settle_mode(TEST_PN_MESSENGER, PN_SND_UNSETTLED)).SetReturn(PN_ARG_ERR);
                }
                else
                {
                    STRICT_EXPECTED_CALL(mocks, pn_messenger_set_snd_settle_mode(TEST_PN_MESSENGER, PN_SND_UNSETTLED));
                    if (whichPath == 5)
                    {
                        STRICT_EXPECTED_CALL(mocks, pn_messenger_set_rcv_settle_mode(TEST_PN_MESSENGER, PN_RCV_FIRST)).SetReturn(PN_ARG_ERR);
                    }
                    else
                    {
                        STRICT_EXPECTED_CALL(mocks, pn_messenger_set_rcv_settle_mode(TEST_PN_MESSENGER, PN_RCV_FIRST));
                        if (whichPath == 6)
                        {
                            STRICT_EXPECTED_CALL(mocks, pn_messenger_set_outgoing_window(TEST_PN_MESSENGER, PROTON_OUTGOING_WINDOW_SIZE)).SetReturn(PN_ARG_ERR);
                        }
                        else
                        {
                            STRICT_EXPECTED_CALL(mocks, pn_messenger_set_outgoing_window(TEST_PN_MESSENGER, PROTON_OUTGOING_WINDOW_SIZE));
                            if (whichPath == 7)
                            {
                                STRICT_EXPECTED_CALL(mocks, pn_messenger_set_incoming_window(TEST_PN_MESSENGER, PROTON_INCOMING_WINDOW_SIZE)).SetReturn(PN_ARG_ERR);
                            }
                            else
                            {
                                STRICT_EXPECTED_CALL(mocks, pn_messenger_set_incoming_window(TEST_PN_MESSENGER, PROTON_INCOMING_WINDOW_SIZE));
                                STRICT_EXPECTED_CALL(mocks, STRING_c_str(cbsAmqpsStringHandle)).SetReturn(TEST_CBS_FULL_PATH);
                                if (whichPath == 8) // CBS
                                {
                                    STRICT_EXPECTED_CALL(mocks, pn_messenger_subscribe(TEST_PN_MESSENGER, TEST_CBS_FULL_PATH)).SetReturn(TEST_NULL_PN_SUBSCRIPTION);
                                }
                                else
                                {
                                    STRICT_EXPECTED_CALL(mocks, pn_messenger_subscribe(TEST_PN_MESSENGER, TEST_CBS_FULL_PATH)).SetReturn(cbsSubscriptionValue);
                                    if (whichPath == 9)
                                    {
                                        // This is a path where the cbs put token fails.  The mocking is set up by other functions
                                        ;
                                    }
                                    else
                                    {
                                        STRICT_EXPECTED_CALL(mocks, STRING_c_str(messageAmqpsStringHandle)).SetReturn(TEST_MESSAGE_FULL_PATH);
                                        if (whichPath == 10)
                                        {
                                            STRICT_EXPECTED_CALL(mocks, pn_messenger_subscribe(TEST_PN_MESSENGER, TEST_MESSAGE_FULL_PATH)).SetReturn(TEST_NULL_PN_SUBSCRIPTION);
                                        }
                                        else
                                        {
                                            STRICT_EXPECTED_CALL(mocks, pn_messenger_subscribe(TEST_PN_MESSENGER, TEST_MESSAGE_FULL_PATH)).SetReturn(messageSubscriptionValue);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

static void setupDestroyStateHappyPath(CIoTHubTransportAMQPMocks &mocks, size_t whereToFail)
{
    (void)mocks;
    if (whereToFail == 1)
    {
        // Nothing to check if the state couldn't be initially allocated.
        ;
    }
    else
    {
        size_t i;
        STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1); // work in progress
        STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1); // available
        if (whereToFail == 2)
        {
            ;
        }
        else
        {
            STRICT_EXPECTED_CALL(mocks, pn_message_free(TEST_PN_MESSAGE));
            for (i = 3; i < (PROTON_EVENT_OUTGOING_WINDOW_SIZE + 3); i++)
            {
                if (whereToFail == i)
                {
                    break;
                }
                else
                {
                    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1); // available (for every one that is actually created).
                    STRICT_EXPECTED_CALL(mocks, DList_RemoveHeadList(IGNORED_PTR_ARG)).IgnoreArgument(1); // available
                    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)).IgnoreArgument(1);
                }
            }
        }
        if (whereToFail < TEST_HAPPY_PATH)
        {
            //
            // None of the strings were allocated so we have to do delete NULLs for all of them.
            //
            STRICT_EXPECTED_CALL(mocks, STRING_delete(STRING_HANDLE_NULL)); // message address
            STRICT_EXPECTED_CALL(mocks, STRING_delete(STRING_HANDLE_NULL)); // event address
            STRICT_EXPECTED_CALL(mocks, STRING_delete(STRING_HANDLE_NULL)); // urled device id
            STRICT_EXPECTED_CALL(mocks, STRING_delete(STRING_HANDLE_NULL)); // devices portion path
            STRICT_EXPECTED_CALL(mocks, STRING_delete(STRING_HANDLE_NULL)); // cbs address
            STRICT_EXPECTED_CALL(mocks, STRING_delete(STRING_HANDLE_NULL)); // device key.
            STRICT_EXPECTED_CALL(mocks, STRING_delete(STRING_HANDLE_NULL)); // zero length string handle.
        }
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)).IgnoreArgument(1); // The actual state!

    }

}


BEGIN_TEST_SUITE(iothubtransportamqp_unittests)

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
    mocksResetAllCalls(NULL);
    int result = BASEIMPLEMENTATION::gballoc_init();
    ASSERT_ARE_EQUAL(int, 0, result);
}

TEST_FUNCTION_CLEANUP(TestMethodCleanup)
{
    BASEIMPLEMENTATION::gballoc_deinit();
    if (!MicroMockReleaseMutex(g_testByTest))
    {
        ASSERT_FAIL("failure in test framework at ReleaseMutex");
    }
}

/*Tests_ Make sure that zero is the epoch.*/
TEST_FUNCTION(is_zero_the_epoch)
{
    time_t epoch_candidate = EPOCH_TIME_T_VALUE;
    tm broken_down_time;
    broken_down_time = *gmtime(&epoch_candidate);
    ASSERT_ARE_EQUAL(int, broken_down_time.tm_hour, 0);
    ASSERT_ARE_EQUAL(int, broken_down_time.tm_min, 0);
    ASSERT_ARE_EQUAL(int, broken_down_time.tm_sec, 0);
    ASSERT_ARE_EQUAL(int, broken_down_time.tm_year, 70);
    ASSERT_ARE_EQUAL(int, broken_down_time.tm_mon, 0);
    ASSERT_ARE_EQUAL(int, broken_down_time.tm_mday, 1);

}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_001: [If parameter config is NULL then clientTransportAMQP_Create shall fail and return NULL.]*/
TEST_FUNCTION(Create_with_null_config_fails)
{
    // arrange
    TRANSPORT_PROVIDER* transportFunctions;
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();

    // act
    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(NULL);

    // assert
    ASSERT_IS_NULL(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_002: [clientTransportAMQP_Create shall fail and return NULL if any other fields of the config structure are NULL.]*/
TEST_FUNCTION(Create_with_null_config_fields_protocol_fails)
{
    // arrange
    IOTHUB_CLIENT_CONFIG iotConfig = { NULL, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX};
    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &iotConfig, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();

    // act
    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    // assert
    ASSERT_IS_NULL(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_002: [clientTransportAMQP_Create shall fail and return NULL if any other fields of the config structure are NULL.]*/
TEST_FUNCTION(Create_with_null_config_fields_devicdId_fails)
{
    // arrange
    IOTHUB_CLIENT_CONFIG iotConfig = { AMQP_Protocol, NULL, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX};
    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &iotConfig, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    // act

    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    // assert
    ASSERT_IS_NULL(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_002: [clientTransportAMQP_Create shall fail and return NULL if any other fields of the config structure are NULL.]*/
TEST_FUNCTION(Create_with_null_config_fields_deviceKey_fails)
{
    // arrange
    IOTHUB_CLIENT_CONFIG iotConfig = { AMQP_Protocol, TEST_DEVICE_ID, NULL, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX};
    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &iotConfig, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();

    // act
    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    // assert
    ASSERT_IS_NULL(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_002: [clientTransportAMQP_Create shall fail and return NULL if any other fields of the config structure are NULL.]*/
TEST_FUNCTION(Create_with_null_config_fields_iotHubName_fails)
{
    // arrange
    IOTHUB_CLIENT_CONFIG iotConfig = { AMQP_Protocol, TEST_DEVICE_ID, TEST_DEVICE_KEY, NULL, TEST_IOTHUB_SUFFIX};
    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &iotConfig, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();

    // act
    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    // assert
    ASSERT_IS_NULL(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_002: [clientTransportAMQP_Create shall fail and return NULL if any other fields of the config structure are NULL.]*/
TEST_FUNCTION(Create_with_null_config_fields_iotHubSuffix_fails)
{
    // arrange
    IOTHUB_CLIENT_CONFIG iotConfig = { AMQP_Protocol, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, NULL};
    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &iotConfig, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();

    // act
    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    // assert
    ASSERT_IS_NULL(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_002: [clientTransportAMQP_Create shall fail and return NULL if any other fields of the config structure are NULL.]*/
TEST_FUNCTION(Create_with_null_config_fields_wts_fails)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, NULL };
    TRANSPORT_PROVIDER* transportFunctions;
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();

    // act
    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    // assert
    ASSERT_IS_NULL(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_016: [clientTransportAMQP_Create shall fail and return NULL if the deviceId length is greater than 128.]*/
TEST_FUNCTION(Create_with_too_long_deviceID_fails)
{
    // arrange
    DLIST_ENTRY wts;
    IOTHUB_CLIENT_CONFIG iotConfig = { AMQP_Protocol,
        "012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678",
        TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX};
    IOTHUBTRANSPORT_CONFIG config = { &iotConfig, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();

    // act
    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    // assert
    ASSERT_IS_NULL(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_017: [clientTransportAMQP_Create shall fail and return NULL if any string config field is zero length.]*/
TEST_FUNCTION(Create_with_zero_length_deviceId_fails)
{
    // arrange
    DLIST_ENTRY wts;
    IOTHUB_CLIENT_CONFIG iotConfig = { AMQP_Protocol, "", TEST_DEVICE_KEY, TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX};
    IOTHUBTRANSPORT_CONFIG config = { &iotConfig, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();

    // act
    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    // assert
    ASSERT_IS_NULL(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_017: [clientTransportAMQP_Create shall fail and return NULL if any string config field is zero length.]*/
TEST_FUNCTION(Create_with_zero_length_deviceKey_fails)
{
    // arrange
    DLIST_ENTRY wts;
    IOTHUB_CLIENT_CONFIG iotConfig = { AMQP_Protocol, TEST_DEVICE_ID, "", TEST_IOTHUB_NAME, TEST_IOTHUB_SUFFIX};
    IOTHUBTRANSPORT_CONFIG config = { &iotConfig, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();

    // act
    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    // assert
    ASSERT_IS_NULL(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_017: [clientTransportAMQP_Create shall fail and return NULL if any string config field is zero length.]*/
TEST_FUNCTION(Create_with_zero_length_iotHubName_fails)
{
    // arrange
    DLIST_ENTRY wts;
    IOTHUB_CLIENT_CONFIG iotConfig = { AMQP_Protocol, TEST_DEVICE_ID, TEST_DEVICE_KEY, "", TEST_IOTHUB_SUFFIX};
    IOTHUBTRANSPORT_CONFIG config = { &iotConfig, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();

    // act
    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    // assert
    ASSERT_IS_NULL(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_017: [clientTransportAMQP_Create shall fail and return NULL if any string config field is zero length.]*/
TEST_FUNCTION(Create_with_zero_length_iotHubSuffix_fails)
{
    // arrange
    DLIST_ENTRY wts;
    IOTHUB_CLIENT_CONFIG iotConfig = { AMQP_Protocol, TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOTHUB_NAME, ""};
    IOTHUBTRANSPORT_CONFIG config = { &iotConfig, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();

    // act
    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    // assert
    ASSERT_IS_NULL(transportHandle);
}


/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_012: [clientTransportAMQP_Create shall succeed and return a non-NULL value.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_018: [clientTransportAMQP_Destroy shall free all the resources currently in use.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_091: [clientTransportAMQP_Create shall create an immutable string, referred henceforth as urledDeviceId, by url encoding the config->deviceId.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_093: [clientTransportAMQP_Create shall create an immutable string, referred henceforth as devicePortionOfPath, from the following pieces: "/devices/" + urledDeviceId.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_097: [clientTransportAMQP_Create shall create an immutable string, referred henceforth as eventAddress, from the following pieces: schemeAndHost + devicesPortionOfPath + "/messages/events".]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_099: [clientTransportAMQP_Create shall create an immutable string, referred henceforth as messageAddress, from the following pieces: schemeAndHost + devicesPortionOfPath + "/messages/devicebound".]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_101: [clientTransportAMQP_Create shall create an immutable string, referred henceforth as cbsAddress, from the following pieces: schemeAndHost + devicesPortionOfPath + "/$cbs".]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_103: [clientTransportAMQP_Create shall create an immutable string, referred henceforth as deviceKey, from the config->deviceId.]*/
TEST_FUNCTION(Create_Destroy_Succeed)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);

    // act
    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    // assert
    ASSERT_IS_NOT_NULL(transportHandle);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_003: [clientTransportAMQP_Create shall fail and return NULL if memory allocation of the transports basic internal state structures fails.]*/
TEST_FUNCTION(Create_Destroy_Initial_Allocate_Fail)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    whenShallmalloc_fail = 1;
    mocksFor_initialCreate(mocks, 1);
    setupDestroyStateHappyPath(mocks, 1);

    // act
    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    // assert
    ASSERT_IS_NULL(transportHandle);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_003: [clientTransportAMQP_Create shall fail and return NULL if memory allocation of the transports basic internal state structures fails.]*/
TEST_FUNCTION(Create_Destroy_Allocate_message_Fail)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocksFor_initialCreate(mocks, 2);
    setupDestroyStateHappyPath(mocks, 2);

    // act
    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    // assert
    ASSERT_IS_NULL(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_003: [clientTransportAMQP_Create shall fail and return NULL if memory allocation of the transports basic internal state structures fails.]*/
TEST_FUNCTION(Create_Destroy_Allocate_Work_Item_1_Fail)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    whenShallmalloc_fail = 2;
    mocksFor_initialCreate(mocks, 3);
    setupDestroyStateHappyPath(mocks, 3);

    // act
    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    // assert
    ASSERT_IS_NULL(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_003: [clientTransportAMQP_Create shall fail and return NULL if memory allocation of the transports basic internal state structures fails.]*/
TEST_FUNCTION(Create_Destroy_Allocate_Work_Item_3_Fail)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    whenShallmalloc_fail = 3;
    mocksFor_initialCreate(mocks, 4);
    setupDestroyStateHappyPath(mocks, 4);

    // act
    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    // assert
    ASSERT_IS_NULL(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_003: [clientTransportAMQP_Create shall fail and return NULL if memory allocation of the transports basic internal state structures fails.]*/
TEST_FUNCTION(Create_Destroy_Allocate_Work_Item_11_Fail)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    whenShallmalloc_fail = 11;
    mocksFor_initialCreate(mocks, 12);
    setupDestroyStateHappyPath(mocks, 12);

    // act
    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    // assert
    ASSERT_IS_NULL(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_096: [If creating the host fails for any reason then clientTransportAMQP_Create shall fail and return NULL.]*/
TEST_FUNCTION(Create_host_initial_construct_fail)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocksFor_initialCreate(mocks, TEST_HAPPY_PATH);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH - 1);

    mocksFor_createHost(mocks, 1, &config);

    // act
    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    // assert
    ASSERT_IS_NULL(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_096: [If creating the schemeAndHost fails for any reason then clientTransportAMQP_Create shall fail and return NULL.]*/
TEST_FUNCTION(Create_host_concat_dot_fail)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocksFor_initialCreate(mocks, TEST_HAPPY_PATH);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH - 1);

    mocksFor_createHost(mocks, 2, &config);

    // act
    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    // assert
    ASSERT_IS_NULL(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_096: [If creating the schemeAndHost fails for any reason then clientTransportAMQP_Create shall fail and return NULL.]*/
TEST_FUNCTION(Create_host_concat_suffix_fail)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocksFor_initialCreate(mocks, TEST_HAPPY_PATH);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH - 1);

    mocksFor_createHost(mocks, 3, &config);

    // act
    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    // assert
    ASSERT_IS_NULL(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_095: [clientTransportAMQP_Create shall create an immutable string, referred henceforth as host, from the following pieces: "config->iotHubName + "." + config->iotHubSuffix.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_092: [If creating the urledDeviceId fails for any reason then clientTransportAMQP_Create shall fail and return NULL.]*/
TEST_FUNCTION(Create_urledDeviceId_urlencode_fail)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocksFor_initialCreate(mocks, TEST_HAPPY_PATH);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    mocksFor_createHost(mocks, TEST_HAPPY_PATH, &config);

    mocksFor_createVariousStrings(mocks, 1, &config);

    // act
    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    // assert
    ASSERT_IS_NULL(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_094: [If creating the devicePortionOfPath fails for any reason then clientTransportAMQP_Create shall fail and return NULL.]*/
TEST_FUNCTION(Create_devicesPortionPath_initial_construct_fail)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocksFor_initialCreate(mocks, TEST_HAPPY_PATH);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    mocksFor_createHost(mocks, TEST_HAPPY_PATH, &config);

    mocksFor_createVariousStrings(mocks, 2, &config);

    // act
    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    // assert
    ASSERT_IS_NULL(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_094: [If creating the devicePortionOfPath fails for any reason then clientTransportAMQP_Create shall fail and return NULL.]*/
TEST_FUNCTION(Create_devicesPortionPath_concat_devices_fail)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocksFor_initialCreate(mocks, TEST_HAPPY_PATH);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    mocksFor_createHost(mocks, TEST_HAPPY_PATH, &config);

    mocksFor_createVariousStrings(mocks, 3, &config);

    // act
    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    // assert
    ASSERT_IS_NULL(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_094: [If creating the devicePortionOfPath fails for any reason then clientTransportAMQP_Create shall fail and return NULL.]*/
TEST_FUNCTION(Create_devicesPortionPath_concat_id_fail)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocksFor_initialCreate(mocks, TEST_HAPPY_PATH);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    mocksFor_createHost(mocks, TEST_HAPPY_PATH, &config);

    mocksFor_createVariousStrings(mocks, 4, &config);

    // act
    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    // assert
    ASSERT_IS_NULL(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_098: [If creating the eventAddress fails for any reason then clientTransportAMQP_Create shall fail and return NULL.]*/
TEST_FUNCTION(Create_telementry_initialconstruct_fail)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocksFor_initialCreate(mocks, TEST_HAPPY_PATH);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    mocksFor_createHost(mocks, TEST_HAPPY_PATH, &config);

    mocksFor_createVariousStrings(mocks, 5, &config);

    // act
    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    // assert
    ASSERT_IS_NULL(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_098: [If creating the eventAddress fails for any reason then clientTransportAMQP_Create shall fail and return NULL.]*/
TEST_FUNCTION(Create_telementry_concat_devicesPath_fail)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocksFor_initialCreate(mocks, TEST_HAPPY_PATH);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    mocksFor_createHost(mocks, TEST_HAPPY_PATH, &config);

    mocksFor_createVariousStrings(mocks, 6, &config);

    // act
    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    // assert
    ASSERT_IS_NULL(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_098: [If creating the eventAddress fails for any reason then clientTransportAMQP_Create shall fail and return NULL.]*/
TEST_FUNCTION(Create_telementry_concat_with_endpoint_fail)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocksFor_initialCreate(mocks, TEST_HAPPY_PATH);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    mocksFor_createHost(mocks, TEST_HAPPY_PATH, &config);

    mocksFor_createVariousStrings(mocks, 7, &config);

    // act
    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    // assert
    ASSERT_IS_NULL(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_100: [If creating the messageAddress fails for any reason then clientTransportAMQP_Create shall fail and return NULL.]*/
TEST_FUNCTION(Create_message_initialconstruct_fail)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocksFor_initialCreate(mocks, TEST_HAPPY_PATH);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    mocksFor_createHost(mocks, TEST_HAPPY_PATH, &config);

    mocksFor_createVariousStrings(mocks, 8, &config);

    // act
    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    // assert
    ASSERT_IS_NULL(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_100: [If creating the messageAddress fails for any reason then clientTransportAMQP_Create shall fail and return NULL.]*/
TEST_FUNCTION(Create_message_concat_with_devicesPath_fail)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocksFor_initialCreate(mocks, TEST_HAPPY_PATH);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    mocksFor_createHost(mocks, TEST_HAPPY_PATH, &config);

    mocksFor_createVariousStrings(mocks, 9, &config);

    // act
    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    // assert
    ASSERT_IS_NULL(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_100: [If creating the messageAddress fails for any reason then clientTransportAMQP_Create shall fail and return NULL.]*/
TEST_FUNCTION(Create_message_concat_with_endpoint_fail)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocksFor_initialCreate(mocks, TEST_HAPPY_PATH);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    mocksFor_createHost(mocks, TEST_HAPPY_PATH, &config);

    mocksFor_createVariousStrings(mocks, 10, &config);

    // act
    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    // assert
    ASSERT_IS_NULL(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_102: [If creating the cbsAddress fails for any reason then clientTransportAMQP_Create shall fail and return NULL.]*/
TEST_FUNCTION(Create_cbs_initial_construct_fail)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocksFor_initialCreate(mocks, TEST_HAPPY_PATH);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    mocksFor_createHost(mocks, TEST_HAPPY_PATH, &config);

    mocksFor_createVariousStrings(mocks, 11, &config);

    // act
    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    // assert
    ASSERT_IS_NULL(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_102: [If creating the cbsAddress fails for any reason then clientTransportAMQP_Create shall fail and return NULL.]*/
TEST_FUNCTION(Create_cbs_concat_host_fail)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocksFor_initialCreate(mocks, TEST_HAPPY_PATH);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    mocksFor_createHost(mocks, TEST_HAPPY_PATH, &config);

    mocksFor_createVariousStrings(mocks, 12, &config);

    // act
    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    // assert
    ASSERT_IS_NULL(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_102: [If creating the cbsAddress fails for any reason then clientTransportAMQP_Create shall fail and return NULL.]*/
TEST_FUNCTION(Create_cbs_concat_cbs_fail)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocksFor_initialCreate(mocks, TEST_HAPPY_PATH);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    mocksFor_createHost(mocks, TEST_HAPPY_PATH, &config);

    mocksFor_createVariousStrings(mocks, 13, &config);

    // act
    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    // assert
    ASSERT_IS_NULL(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_104: [If creating the deviceKey fails for any reason then clientTransportAMQP_Create shall fail and return NULL.]*/
TEST_FUNCTION(Create_deviceKey_fail)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocksFor_initialCreate(mocks, TEST_HAPPY_PATH);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    mocksFor_createHost(mocks, TEST_HAPPY_PATH, &config);

    mocksFor_createVariousStrings(mocks, 14, &config);

    // act
    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    // assert
    ASSERT_IS_NULL(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_186: [An empty string shall be created.  If this fails then clientTransportAMQP shall fail and return NULL.]*/
TEST_FUNCTION(Create_zeroLengthString_fail)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocksFor_initialCreate(mocks, TEST_HAPPY_PATH);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    mocksFor_createHost(mocks, TEST_HAPPY_PATH, &config);

    mocksFor_createVariousStrings(mocks, 15, &config);

    // act
    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    // assert
    ASSERT_IS_NULL(transportHandle);
}


/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_014: [clientTransportAMQP_Subscribe shall succeed and return a value of zero.]*/
TEST_FUNCTION(Subscribe_with_non_null_succeeds)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    int resultOfSubscribe = 1;
    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);

    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    // act
    resultOfSubscribe = transportFunctions->IoTHubTransport_Subscribe(transportHandle);

    // assert
    ASSERT_ARE_EQUAL(int, resultOfSubscribe, (int)0);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_013: [If the parameter handle is NULL then clientTransportAMQP_Subscribe shall fail and return a non-zero value.]*/
TEST_FUNCTION(Subscribe_with_null_fails)
{
    // arrange
    TRANSPORT_PROVIDER* transportFunctions;
    int resultOfSubscribe = 1;
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();

    // act
    resultOfSubscribe = transportFunctions->IoTHubTransport_Subscribe(NULL);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, resultOfSubscribe, (int)0);
}

TEST_FUNCTION(Unsubscribe_with_non_null_succeeds)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);

    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    // act
    transportFunctions->IoTHubTransport_Unsubscribe(transportHandle);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_015: [clientTransportAMQP_Unsubscribe shall do nothing if handle is NULL.]*/
TEST_FUNCTION(Unsubscribe_with_null_succeeds)
{
    // arrange
    TRANSPORT_PROVIDER* transportFunctions;
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();

    // act

    transportFunctions->IoTHubTransport_Unsubscribe(NULL);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_019: [clientTransportAMQP_Dowork shall do no work if handle is NULL.]*/
TEST_FUNCTION(DoWork_with_null_transport)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);

    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    // act
    transportFunctions->IoTHubTransport_DoWork(NULL, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_020: [clientTransportAMQP_Dowork shall do no work if iotHubClientHandle is NULL.]*/
TEST_FUNCTION(DoWork_with_null_client)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);

    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    // act
    transportFunctions->IoTHubTransport_DoWork(transportHandle, NULL);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_020: [clientTransportAMQP_Dowork shall do no work if iotHubClientHandle is NULL.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_019: [clientTransportAMQP_Dowork shall do no work if handle is NULL.]*/
TEST_FUNCTION(DoWork_with_null_transport_and_null_client)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);

    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    // act
    transportFunctions->IoTHubTransport_DoWork(NULL, NULL);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_021: [Invoke pn_messenger_stop on the messenger.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_022: [If pn_messenger_stop returns 0 then invoke pn_messenger_free on the messenger.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_027: [DoWork shall invoke pn_messenger to create a new messenger.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_029: [pn_messenger_start shall be invoked.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_031: [pn_messenger_set_blocking shall be invoked.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_033: [pn_messenger_set_snd_settle_mode shall be invoked.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_035: [pn_messenger_set_rcv_settle_mode shall be invoked.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_037: [pn_messenger_set_outgoing_window shall be invoked.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_039: [pn_messenger_set_incoming_window shall be invoked.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_041: [pn_messenger_subscribe shall be invoked.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_088: [pn_messenger_work shall be invoked following all the above actions.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_089: [If there is nothing on the list then go on to the message.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_129: [If the status code was 200 then set the state variable putTokenWasSuccessful, accept the message and return.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_163: [Invocation of renewIfNecessaryTheCBS will first obtain the current number of seconds since the epoch.*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_164: [If the difference between lastExpiryUsed and the current value is less than refresh seconds then renewIfNecessaryTheCBS will attempt to renew the SAS.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_165: [If SAS renewal is to occur then lifetime seconds is added to the current number of seconds.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_166: [This value is saved in the state as sendTokenMessageId.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_169: [SASToken_Create is invoked.]   */
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_171: [renewIfNecessaryTheCBS will invoke a local static function putToken.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_173: [Note that putToken actually sets a state value which will indicate whether the CBS accepted the token and responded with success.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_130: [putToken clears the single messages that is used for all communication via proton.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_131: [putToken obtains the properties of the message.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_134: [putToken sets the address (CBS endpoint) that this put token operation is targeted to.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_136: [putToken sets the reply to address for the put token operation.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_138: [putToken sets the message to not be inferred.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_140: [putToken sets the messageId of the message to the expiry of the token.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_142: [putToken obtains the body of the message.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_144: [putToken places the actual SAS token into the body of the message.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_146: [putToken invokes pn_messenger_put.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_148: [A proton tracker is obtained.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_149: [putToken will loop invoking pn_messenger_work waiting for the message to reach a terminal delivery state.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_152: [The tracker is settled.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_153: [putToken will again loop waiting for a reply.]*/
TEST_FUNCTION(DoWork_One_Invoke_Succeeds)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    proton_messenger_init_path(mocks, TEST_HAPPY_PATH);
    mocksFor_rollbackEvent(mocks, 1);
    mock_renewHappyPath(mocks);
    setupAmqpMessengerDispose(mocks, 2);
    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(0); // From the call to renew if necessary.
    mocksFor_NoTelementry(mocks);
    STRICT_EXPECTED_CALL(mocks, pn_messenger_recv(TEST_PN_MESSENGER, 0));
    mocksFor_protonWork(mocks, TEST_HAPPY_PATH);

    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    // act
    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_027: [DoWork shall invoke pn_messenger to create a new messenger.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_028: [If pn_messenger returns NULL then the messenger initialization fails.]*/
TEST_FUNCTION(DoWork_One_Invoke_messenger_init_fails_1)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    proton_messenger_init_path(mocks, 1);
    mocksFor_rollbackEvent(mocks, 1);

    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    // act
    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_029: [pn_messenger_start shall be invoked.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_030: [If pn_messenger_start returns a non-zero value then messenger initialization fails.]*/
TEST_FUNCTION(DoWork_One_Invoke_messenger_init_fails_2)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    proton_messenger_init_path(mocks, 2);
    mocksFor_rollbackEvent(mocks, 1);
    setupAmqpMessengerDispose(mocks, 2);

    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    // act
    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_031: [pn_messenger_set_blocking shall be invoked.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_032: [If pn_messenger_set_blocking returns a non-zero value then messenger initialization fails.]*/
TEST_FUNCTION(DoWork_One_Invoke_messenger_init_fails_3)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    proton_messenger_init_path(mocks, 3);
    mocksFor_rollbackEvent(mocks, 1);
    setupAmqpMessengerDispose(mocks, 2);

    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    // act
    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_033: [pn_messenger_set_snd_settle_mode shall be invoked.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_034: [If pn_messenger_set_snd_settle_mode returns a non-zero value then messenger initialization fails.]*/
TEST_FUNCTION(DoWork_One_Invoke_messenger_init_fails_4)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    proton_messenger_init_path(mocks, 4);
    mocksFor_rollbackEvent(mocks, 1);
    setupAmqpMessengerDispose(mocks, 2);

    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    // act
    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_035: [pn_messenger_set_rcv_settle_mode shall be invoked.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_036: [If pn_messenger_set_rcv_settle_mode returns a non-zero value then messenger initialization fails.]*/
TEST_FUNCTION(DoWork_One_Invoke_messenger_init_fails_5)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    proton_messenger_init_path(mocks, 5);
    mocksFor_rollbackEvent(mocks, 1);
    setupAmqpMessengerDispose(mocks, 2);

    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    // act
    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_037: [pn_messenger_set_outgoing_window shall be invoked.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_038: [If pn_messenger_set_outgoing_window returns a non-zero value then messenger initialization fails.]*/
TEST_FUNCTION(DoWork_One_Invoke_messenger_init_fails_6)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    proton_messenger_init_path(mocks, 6);
    mocksFor_rollbackEvent(mocks, 1);
    setupAmqpMessengerDispose(mocks, 2);

    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    // act
    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_039: [pn_messenger_set_incoming_window shall be invoked.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_040: [If pn_messenger_set_incoming_window returns a non-zero value then messenger initialization fails.]*/
TEST_FUNCTION(DoWork_One_Invoke_messenger_init_fails_7)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    proton_messenger_init_path(mocks, 7);
    mocksFor_rollbackEvent(mocks, 1);
    setupAmqpMessengerDispose(mocks, 2);

    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    // act
    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_156: [pn_messenger_subscribe will be invoked on the cbs address.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_157: [If the pn_messenger_subscribe returns a non-zero value then messenger initialization fails.]*/
TEST_FUNCTION(DoWork_One_Invoke_messenger_init_fails_8)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    proton_messenger_init_path(mocks, 8);
    mocksFor_rollbackEvent(mocks, 1);
    setupAmqpMessengerDispose(mocks, 2);


    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    ASSERT_IS_NOT_NULL(transportHandle);

    // act

    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_041: [pn_messenger_subscribe shall be invoked.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_042: [If pn_messenger_subscribe returns a non-zero value then messenger initialization fails.]*/
TEST_FUNCTION(DoWork_One_Invoke_messenger_init_fails_9)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    proton_messenger_init_path(mocks, 10);
    mocksFor_rollbackEvent(mocks, 1);
    mock_renewHappyPath(mocks);
    setupAmqpMessengerDispose(mocks, 2);


    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    ASSERT_IS_NOT_NULL(transportHandle);

    // act

    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_160: [If after renewIfNecessaryCBS is invoked, the state variable putTokenWasSuccessful is false then the messenger initialization fails.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_170: [If SASToken_Create returns NULL then renewal is considered a failure and we move on to the next phase of DoWork.]*/
TEST_FUNCTION(DoWork_One_Invoke_messenger_init_fails_renew_fails_SASToken_Create)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    proton_messenger_init_path(mocks, 9);
    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(TEST_INITIAL_TIME_VALUE);
    STRICT_EXPECTED_CALL(mocks, SASToken_Create(deviceKeyStringHandle, devicesPortionPathStringHandle, zeroLengthStringHandle, TEST_EXPIRY_TEST_VALUE)).SetReturn(TEST_NULL_STRING_HANDLE);
    mocksFor_rollbackEvent(mocks, 1);
    setupAmqpMessengerDispose(mocks, 2);

    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    ASSERT_IS_NOT_NULL(transportHandle);

    // act

    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);

}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_172: [If putToken fails then the renewal is considered a failure and we move on to the next phase of DoWork.]*/
TEST_FUNCTION(DoWork_One_Invoke_messenger_init_fails_renew_fails_putToken)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    proton_messenger_init_path(mocks, 9);
    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(TEST_INITIAL_TIME_VALUE);
    STRICT_EXPECTED_CALL(mocks, SASToken_Create(deviceKeyStringHandle, devicesPortionPathStringHandle, zeroLengthStringHandle, TEST_EXPIRY_TEST_VALUE)).SetReturn(sasTokenStringHandle);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(sasTokenStringHandle));
    mocksFor_putToken(mocks, 1);
    mocksFor_rollbackEvent(mocks, 1);
    setupAmqpMessengerDispose(mocks, 2);

    PAMQP_TRANSPORT_STATE transportHandle = (PAMQP_TRANSPORT_STATE)transportFunctions->IoTHubTransport_Create(&config);

    ASSERT_IS_NOT_NULL(transportHandle);

    // act

    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // assert
    ASSERT_IS_FALSE(transportHandle->putTokenWasSuccessful);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);

}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_132: [If this fails then putToken fails.]*/
TEST_FUNCTION(DoWork_One_Invoke_messenger_init_fails_renew_fails_putToken_get_initial_properties_fail)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    proton_messenger_init_path(mocks, 9);
    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(TEST_INITIAL_TIME_VALUE);
    STRICT_EXPECTED_CALL(mocks, SASToken_Create(deviceKeyStringHandle, devicesPortionPathStringHandle, zeroLengthStringHandle, TEST_EXPIRY_TEST_VALUE)).SetReturn(sasTokenStringHandle);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(sasTokenStringHandle));
    mocksFor_putToken(mocks, 1);
    mocksFor_rollbackEvent(mocks, 1);
    setupAmqpMessengerDispose(mocks, 2);

    PAMQP_TRANSPORT_STATE transportHandle = (PAMQP_TRANSPORT_STATE)transportFunctions->IoTHubTransport_Create(&config);

    ASSERT_IS_NOT_NULL(transportHandle);

    // act

    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // assert
    ASSERT_IS_FALSE(transportHandle->putTokenWasSuccessful);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);

}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_133: [putToken constructs a proton map which describes the put token operation.  If this construction fails then putToken fails.]*/
TEST_FUNCTION(DoWork_One_Invoke_messenger_init_fails_renew_fails_putToken_map_construct_fail)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    proton_messenger_init_path(mocks, 9);
    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(TEST_INITIAL_TIME_VALUE);
    STRICT_EXPECTED_CALL(mocks, SASToken_Create(deviceKeyStringHandle, devicesPortionPathStringHandle, zeroLengthStringHandle, TEST_EXPIRY_TEST_VALUE)).SetReturn(sasTokenStringHandle);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(sasTokenStringHandle));
    mocksFor_putToken(mocks, 2);
    mocksFor_rollbackEvent(mocks, 1);
    setupAmqpMessengerDispose(mocks, 2);

    PAMQP_TRANSPORT_STATE transportHandle = (PAMQP_TRANSPORT_STATE)transportFunctions->IoTHubTransport_Create(&config);

    ASSERT_IS_NOT_NULL(transportHandle);

    // act

    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // assert
    ASSERT_IS_FALSE(transportHandle->putTokenWasSuccessful);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);

}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_140: [putToken sets the messageId of the message to the expiry of the token.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_141: [If this fails then putToken fails.]*/
TEST_FUNCTION(DoWork_One_Invoke_messenger_init_fails_renew_set_message_id_fail)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    proton_messenger_init_path(mocks, 9);
    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(TEST_INITIAL_TIME_VALUE);
    STRICT_EXPECTED_CALL(mocks, SASToken_Create(deviceKeyStringHandle, devicesPortionPathStringHandle, zeroLengthStringHandle, TEST_EXPIRY_TEST_VALUE)).SetReturn(sasTokenStringHandle);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(sasTokenStringHandle));
    mocksFor_putToken(mocks, 3);
    mocksFor_rollbackEvent(mocks, 1);
    setupAmqpMessengerDispose(mocks, 2);

    PAMQP_TRANSPORT_STATE transportHandle = (PAMQP_TRANSPORT_STATE)transportFunctions->IoTHubTransport_Create(&config);

    ASSERT_IS_NOT_NULL(transportHandle);

    // act

    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // assert
    ASSERT_IS_FALSE(transportHandle->putTokenWasSuccessful);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);

}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_146: [putToken invokes pn_messenger_put.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_147: [If this fails then putToken fails.]*/
TEST_FUNCTION(DoWork_One_Invoke_messenger_init_fails_renew_put_fail)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    proton_messenger_init_path(mocks, 9);
    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(TEST_INITIAL_TIME_VALUE);
    STRICT_EXPECTED_CALL(mocks, SASToken_Create(deviceKeyStringHandle, devicesPortionPathStringHandle, zeroLengthStringHandle, TEST_EXPIRY_TEST_VALUE)).SetReturn(sasTokenStringHandle);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(sasTokenStringHandle));
    mocksFor_putToken(mocks, 4);
    mocksFor_rollbackEvent(mocks, 1);
    setupAmqpMessengerDispose(mocks, 2);

    PAMQP_TRANSPORT_STATE transportHandle = (PAMQP_TRANSPORT_STATE)transportFunctions->IoTHubTransport_Create(&config);

    ASSERT_IS_NOT_NULL(transportHandle);

    // act

    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // assert
    ASSERT_IS_FALSE(transportHandle->putTokenWasSuccessful);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);

}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_150: [If a terminal delivery state is not reached then putToken fails.]*/
TEST_FUNCTION(DoWork_One_Invoke_messenger_init_fails_renew_fails_work_fails)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    proton_messenger_init_path(mocks, 9);
    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(TEST_INITIAL_TIME_VALUE);
    STRICT_EXPECTED_CALL(mocks, SASToken_Create(deviceKeyStringHandle, devicesPortionPathStringHandle, zeroLengthStringHandle, TEST_EXPIRY_TEST_VALUE)).SetReturn(sasTokenStringHandle);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(sasTokenStringHandle));
    mocksFor_putToken(mocks, 5);
    mocksFor_rollbackEvent(mocks, 1);
    setupAmqpMessengerDispose(mocks, 2);

    PAMQP_TRANSPORT_STATE transportHandle = (PAMQP_TRANSPORT_STATE)transportFunctions->IoTHubTransport_Create(&config);

    ASSERT_IS_NOT_NULL(transportHandle);

    // act

    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // assert
    ASSERT_IS_FALSE(transportHandle->putTokenWasSuccessful);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);

}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_150: [If a terminal delivery state is not reached then putToken fails.]*/
TEST_FUNCTION(DoWork_One_Invoke_messenger_init_fails_renew_times_out_fails)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    proton_messenger_init_path(mocks, 9);
    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(TEST_INITIAL_TIME_VALUE);
    STRICT_EXPECTED_CALL(mocks, SASToken_Create(deviceKeyStringHandle, devicesPortionPathStringHandle, zeroLengthStringHandle, TEST_EXPIRY_TEST_VALUE)).SetReturn(sasTokenStringHandle);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(sasTokenStringHandle));
    mocksFor_putToken(mocks, 6);
    mocksFor_rollbackEvent(mocks, 1);
    setupAmqpMessengerDispose(mocks, 2);

    PAMQP_TRANSPORT_STATE transportHandle = (PAMQP_TRANSPORT_STATE)transportFunctions->IoTHubTransport_Create(&config);

    ASSERT_IS_NOT_NULL(transportHandle);

    // act

    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // assert
    ASSERT_IS_FALSE(transportHandle->putTokenWasSuccessful);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);

}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_151: [On reaching a terminal delivery state, if the message status is NOT accepted then putToken fails.]*/
TEST_FUNCTION(DoWork_One_Invoke_messenger_init_fails_renew_rejected_fails)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    proton_messenger_init_path(mocks, 9);
    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(TEST_INITIAL_TIME_VALUE);
    STRICT_EXPECTED_CALL(mocks, SASToken_Create(deviceKeyStringHandle, devicesPortionPathStringHandle, zeroLengthStringHandle, TEST_EXPIRY_TEST_VALUE)).SetReturn(sasTokenStringHandle);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(sasTokenStringHandle));
    mocksFor_putToken(mocks, 7);
    mocksFor_rollbackEvent(mocks, 1);
    setupAmqpMessengerDispose(mocks, 2);

    PAMQP_TRANSPORT_STATE transportHandle = (PAMQP_TRANSPORT_STATE)transportFunctions->IoTHubTransport_Create(&config);

    ASSERT_IS_NOT_NULL(transportHandle);

    // act

    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // assert
    ASSERT_IS_FALSE(transportHandle->putTokenWasSuccessful);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);

}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_021: [Invoke pn_messenger_stop on the messenger.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_023: [If pn_messenger_stop returns any result other than PN_INPROGRESS then place the messenger on a list for attempting to stop later.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_027: [DoWork shall invoke pn_messenger to create a new messenger.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_029: [pn_messenger_start shall be invoked.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_031: [pn_messenger_set_blocking shall be invoked.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_033: [pn_messenger_set_snd_settle_mode shall be invoked.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_035: [pn_messenger_set_rcv_settle_mode shall be invoked.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_037: [pn_messenger_set_outgoing_window shall be invoked.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_039: [pn_messenger_set_incoming_window shall be invoked.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_041: [pn_messenger_subscribe shall be invoked.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_159: [renewIfNecessaryCBS will be invoked.]*/
/*Tests_During messenger initialization a state variable known as lastExpiryUsed will be set to zero.*/
TEST_FUNCTION(DoWork_One_Invoke_fail_the_stop_messenger)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    proton_messenger_init_path(mocks, TEST_HAPPY_PATH);
    mocksFor_rollbackEvent(mocks, 1);
    mock_renewHappyPath(mocks);
    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(0); // From the call to renew if necessary.
    setupAmqpMessengerDispose(mocks, 1);
    mocksFor_NoTelementry(mocks);
    STRICT_EXPECTED_CALL(mocks, pn_messenger_recv(TEST_PN_MESSENGER, 0));
    mocksFor_protonWork(mocks, TEST_HAPPY_PATH);
    STRICT_EXPECTED_CALL(mocks, pn_messenger_stop(TEST_PN_MESSENGER));
    STRICT_EXPECTED_CALL(mocks, pn_messenger_free(TEST_PN_MESSENGER));
    STRICT_EXPECTED_CALL(mocks, DList_RemoveEntryList(IGNORED_PTR_ARG)).IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)).IgnoreArgument(1);

    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    // act
    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_021: [Invoke pn_messenger_stop on the messenger.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_024: [If pn_messenger_stop returns PN_INPROGRESS invoke pn_messenger_stopped a fixed number of time, seeking a return of true.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_026: [If pn_messenger_stopped never returns true, then place the messenger on a list for attempting to stop later.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_027: [DoWork shall invoke pn_messenger to create a new messenger.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_029: [pn_messenger_start shall be invoked.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_031: [pn_messenger_set_blocking shall be invoked.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_033: [pn_messenger_set_snd_settle_mode shall be invoked.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_035: [pn_messenger_set_rcv_settle_mode shall be invoked.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_037: [pn_messenger_set_outgoing_window shall be invoked.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_039: [pn_messenger_set_incoming_window shall be invoked.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_041: [pn_messenger_subscribe shall be invoked.]*/
TEST_FUNCTION(DoWork_One_Invoke_stopped_never_does)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    proton_messenger_init_path(mocks, TEST_HAPPY_PATH);
    mocksFor_rollbackEvent(mocks, 1);
    mock_renewHappyPath(mocks);
    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(0); // From the call to renew if necessary.
    setupAmqpMessengerDispose(mocks, TEST_HAPPY_PATH);
    mocksFor_NoTelementry(mocks);
    STRICT_EXPECTED_CALL(mocks, pn_messenger_recv(TEST_PN_MESSENGER, 0));
    mocksFor_protonWork(mocks, TEST_HAPPY_PATH);
    STRICT_EXPECTED_CALL(mocks, pn_messenger_stop(TEST_PN_MESSENGER));
    STRICT_EXPECTED_CALL(mocks, pn_messenger_free(TEST_PN_MESSENGER));
    STRICT_EXPECTED_CALL(mocks, DList_RemoveEntryList(IGNORED_PTR_ARG)).IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)).IgnoreArgument(1);

    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    // act
    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_021: [Invoke pn_messenger_stop on the messenger.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_024: [If pn_messenger_stop returns PN_INPROGRESS invoke pn_messenger_stopped a fixed number of time, seeking a return of true.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_025: [If pn_messenger_stopped returns true, then call pn_messenger_free.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_027: [DoWork shall invoke pn_messenger to create a new messenger.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_029: [pn_messenger_start shall be invoked.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_031: [pn_messenger_set_blocking shall be invoked.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_033: [pn_messenger_set_snd_settle_mode shall be invoked.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_035: [pn_messenger_set_rcv_settle_mode shall be invoked.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_037: [pn_messenger_set_outgoing_window shall be invoked.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_039: [pn_messenger_set_incoming_window shall be invoked.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_041: [pn_messenger_subscribe shall be invoked.]*/
TEST_FUNCTION(DoWork_One_Invoke_stopped_finally_does)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    proton_messenger_init_path(mocks, TEST_HAPPY_PATH);
    mocksFor_rollbackEvent(mocks, 1);
    mock_renewHappyPath(mocks);
    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(0); // From the call to renew if necessary.
    setupAmqpMessengerDispose(mocks, 5);
    mocksFor_NoTelementry(mocks);
    STRICT_EXPECTED_CALL(mocks, pn_messenger_recv(TEST_PN_MESSENGER, 0));
    mocksFor_protonWork(mocks, TEST_HAPPY_PATH);
    STRICT_EXPECTED_CALL(mocks, pn_messenger_free(TEST_PN_MESSENGER));

    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    // act
    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_046: [If pn_messenger_recv fails it will be logged.]*/
TEST_FUNCTION(DoWork_One_Invoke_recevie_nosub_fails)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    proton_messenger_init_path(mocks, TEST_HAPPY_PATH);
    mocksFor_rollbackEvent(mocks, 1);
    mock_renewHappyPath(mocks);
    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(0); // From the call to renew if necessary.
    setupAmqpMessengerDispose(mocks, 2);
    mocksFor_NoTelementry(mocks);
    mocksFor_processReceives(mocks, 11);

    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    ASSERT_IS_NOT_NULL(transportHandle);

    // act

    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_046: [If pn_messenger_recv fails it will be logged.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_105: [processReceives will then go on to the next action of DoWork.]*/
TEST_FUNCTION(DoWork_One_Invoke_receive_fails)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    proton_messenger_init_path(mocks, TEST_HAPPY_PATH);
    mocksFor_rollbackEvent(mocks, 1);
    mock_renewHappyPath(mocks);
    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(0); // From the call to renew if necessary.
    setupAmqpMessengerDispose(mocks, 2);
    mocksFor_NoTelementry(mocks);
    mocksFor_processReceives(mocks, 22);

    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    ASSERT_IS_NOT_NULL(transportHandle);
    transportFunctions->IoTHubTransport_Subscribe(transportHandle);

    // act

    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_043: [If DoWork_PullMessage is false then processReceives will cancel any preceding messages by invoking pn_messenger_recv.]*/
TEST_FUNCTION(DoWork_messages_NoSubScribe_succeeds)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    proton_messenger_init_path(mocks, TEST_HAPPY_PATH);
    mocksFor_rollbackEvent(mocks, 1);
    mock_renewHappyPath(mocks);
    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(0); // From the call to renew if necessary.
    setupAmqpMessengerDispose(mocks, 2);
    mocksFor_protonWork(mocks, TEST_HAPPY_PATH);
    mocksFor_NoTelementry(mocks);
    mocksFor_processReceives(mocks, 1);

    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    // act
    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_044: [If the DoWork_PullMessage or the waitingForPutTokenReply flag is true then processReceives will pull messages with pn_messenger_recv.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_045: [processReceives will check the number of messages received by invoking pn_messenger_incoming.]*/
TEST_FUNCTION(DoWork_messages_subScribed_no_messages_succeeds)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    proton_messenger_init_path(mocks, TEST_HAPPY_PATH);
    mocksFor_rollbackEvent(mocks, 1);
    mock_renewHappyPath(mocks);
    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(0); // From the call to renew if necessary.
    setupAmqpMessengerDispose(mocks, 2);
    mocksFor_protonWork(mocks, TEST_HAPPY_PATH);
    mocksFor_NoTelementry(mocks);
    mocksFor_processReceives(mocks, 2);

    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    transportFunctions->IoTHubTransport_Subscribe(transportHandle);

    // act
    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_045: [processReceives will check the number of messages received by invoking pn_messenger_incoming.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_047: [processReceives will retrieve a message with pn_messenger_get.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_048: [If pn_messenger_get fails we log and break the receive loop.]*/
TEST_FUNCTION(DoWork_messages_subScribed_no_message_found_when_when_should)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    proton_messenger_init_path(mocks, TEST_HAPPY_PATH);
    mocksFor_rollbackEvent(mocks, 1);
    mock_renewHappyPath(mocks);
    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(0); // From the call to renew if necessary.
    setupAmqpMessengerDispose(mocks, 2);
    mocksFor_protonWork(mocks, TEST_HAPPY_PATH);
    mocksFor_NoTelementry(mocks);
    mocksFor_processReceives(mocks, 3);

    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    transportFunctions->IoTHubTransport_Subscribe(transportHandle);

    // act
    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}



/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_108: [If theMessageSource is equal to the state variable cbsSubscription invoke processCBSReply.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_112: [The correlationId message property is obtained via a call to pn_message_get_correlation_id.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_113: [The id is compared to the state variable sendTokenMessageId.  If there is no match, an error is logged, the message is abandoned and processCBSReply returns.]*/
TEST_FUNCTION(DoWork_replyOnCBS_Invalid_correlation_fail)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    proton_messenger_init_path(mocks, TEST_HAPPY_PATH);
    mocksFor_rollbackEvent(mocks, 1);
    mock_renewHappyPath(mocks);
    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(0); // From the call to renew if necessary.
    setupAmqpMessengerDispose(mocks, 2);
    mocksFor_protonWork(mocks, TEST_HAPPY_PATH);
    mocksFor_NoTelementry(mocks);
    mocksFor_processReceives(mocks, 5);
    mocksFor_processCBS(mocks, 1);

    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    ASSERT_IS_NOT_NULL(transportHandle);

    //
    // Have to do this because we don't have access to the state variables.
    //
    transportFunctions->IoTHubTransport_Subscribe(transportHandle);

    // act

    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_114: [Obtain the properties of the message by invoking pn_message_properties.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_115: [If NULL then abandon the message and return.]*/
TEST_FUNCTION(DoWork_replyOnCBS_get_properties_fail)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    proton_messenger_init_path(mocks, 9);
    mocksFor_rollbackEvent(mocks, 1);
    mock_renewUpToProcessCBS(mocks);
    setupAmqpMessengerDispose(mocks, 2);
    mocksFor_processCBS(mocks, 2);

    PAMQP_TRANSPORT_STATE transportHandle = (PAMQP_TRANSPORT_STATE)transportFunctions->IoTHubTransport_Create(&config);

    ASSERT_IS_NOT_NULL(transportHandle);

    // act

    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_116: [Advance to the first property of the message via pn_data_next.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_117: [If this fails, abandon and return.]*/
TEST_FUNCTION(DoWork_replyOnCBS_data_next_fail)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    proton_messenger_init_path(mocks, 9);
    mocksFor_rollbackEvent(mocks, 1);
    mock_renewUpToProcessCBS(mocks);
    setupAmqpMessengerDispose(mocks, 2);
    mocksFor_processCBS(mocks, 3);

    PAMQP_TRANSPORT_STATE transportHandle = (PAMQP_TRANSPORT_STATE)transportFunctions->IoTHubTransport_Create(&config);

    ASSERT_IS_NOT_NULL(transportHandle);

    // act

    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_118: [Invoke pn_data_get_map to obtain the number of properties.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_119: [If the number of properties is 0 then abandon and return.]*/
TEST_FUNCTION(DoWork_replyOnCBS_data_get_map_fail)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    proton_messenger_init_path(mocks, 9);
    mocksFor_rollbackEvent(mocks, 1);
    mock_renewUpToProcessCBS(mocks);
    setupAmqpMessengerDispose(mocks, 2);
    mocksFor_processCBS(mocks, 4);

    PAMQP_TRANSPORT_STATE transportHandle = (PAMQP_TRANSPORT_STATE)transportFunctions->IoTHubTransport_Create(&config);

    ASSERT_IS_NOT_NULL(transportHandle);

    // act

    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_120: [Enter the property map by invoking pn_data_enter.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_121: [If that fails then abandon and return.]*/
TEST_FUNCTION(DoWork_replyOnCBS_data_enter_fail)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    proton_messenger_init_path(mocks, 9);
    mocksFor_rollbackEvent(mocks, 1);
    mock_renewUpToProcessCBS(mocks);
    setupAmqpMessengerDispose(mocks, 2);
    mocksFor_processCBS(mocks, 5);

    PAMQP_TRANSPORT_STATE transportHandle = (PAMQP_TRANSPORT_STATE)transportFunctions->IoTHubTransport_Create(&config);

    ASSERT_IS_NOT_NULL(transportHandle);

    // act

    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_122: [Get the property name.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_123: [If that fails then abandon and return.]*/
TEST_FUNCTION(DoWork_replyOnCBS_get_property_name_fail)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    proton_messenger_init_path(mocks, 9);
    mocksFor_rollbackEvent(mocks, 1);
    mock_renewUpToProcessCBS(mocks);
    setupAmqpMessengerDispose(mocks, 2);
    mocksFor_processCBS(mocks, 6);

    PAMQP_TRANSPORT_STATE transportHandle = (PAMQP_TRANSPORT_STATE)transportFunctions->IoTHubTransport_Create(&config);

    ASSERT_IS_NOT_NULL(transportHandle);

    // act

    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_124: [If the property name is not "status-code" than abandon and return.]*/
TEST_FUNCTION(DoWork_replyOnCBS_get_incorrect_property_name_fail)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    proton_messenger_init_path(mocks, 9);
    mocksFor_rollbackEvent(mocks, 1);
    mock_renewUpToProcessCBS(mocks);
    setupAmqpMessengerDispose(mocks, 2);
    mocksFor_processCBS(mocks, 7);

    PAMQP_TRANSPORT_STATE transportHandle = (PAMQP_TRANSPORT_STATE)transportFunctions->IoTHubTransport_Create(&config);

    ASSERT_IS_NOT_NULL(transportHandle);

    // act

    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/*Testss_SRS_IOTHUBTRANSPORTTAMQP_06_125: [Get the actual status code.]*/
/*Codes_SRS_IOTHUBTRANSPORTTAMQP_06_126: [If that fails then abandon and return.]*/
TEST_FUNCTION(DoWork_replyOnCBS_get_status_code_fail)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    proton_messenger_init_path(mocks, 9);
    mocksFor_rollbackEvent(mocks, 1);
    mock_renewUpToProcessCBS(mocks);
    setupAmqpMessengerDispose(mocks, 2);
    mocksFor_processCBS(mocks, 8);

    PAMQP_TRANSPORT_STATE transportHandle = (PAMQP_TRANSPORT_STATE)transportFunctions->IoTHubTransport_Create(&config);

    ASSERT_IS_NOT_NULL(transportHandle);

    // act

    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_128: [Accept the message and return.]*/
TEST_FUNCTION(DoWork_replyOnCBS_not_success_status_code_fail)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    proton_messenger_init_path(mocks, 9);
    mocksFor_rollbackEvent(mocks, 1);
    mock_renewUpToProcessCBS(mocks);
    setupAmqpMessengerDispose(mocks, 2);
    mocksFor_processCBS(mocks, 9);

    PAMQP_TRANSPORT_STATE transportHandle = (PAMQP_TRANSPORT_STATE)transportFunctions->IoTHubTransport_Create(&config);

    ASSERT_IS_NOT_NULL(transportHandle);

    // act

    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_109: [Otherwise, log an error and abandon the message so that it never returns.]*/
TEST_FUNCTION(DoWork_processReceives_unknownSubscription_fail)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    proton_messenger_init_path(mocks, TEST_HAPPY_PATH);
    mocksFor_rollbackEvent(mocks, 1);
    mock_renewHappyPath(mocks);
    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(0); // From the call to renew if necessary.
    setupAmqpMessengerDispose(mocks, 2);
    mocksFor_protonWork(mocks, TEST_HAPPY_PATH);
    mocksFor_NoTelementry(mocks);
    mocksFor_processReceives(mocks, 6);
    STRICT_EXPECTED_CALL(mocks, pn_messenger_release(TEST_PN_MESSENGER, TEST_PN_TRACKER, 0));
    STRICT_EXPECTED_CALL(mocks, pn_messenger_settle(TEST_PN_MESSENGER, TEST_PN_TRACKER, 0));

    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    ASSERT_IS_NOT_NULL(transportHandle);

    //
    // Have to do this because we don't have access to the state variables.
    //
    transportFunctions->IoTHubTransport_Subscribe(transportHandle);

    // act

    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_111: [If the state variable DoWork_PullMessage is false then the message shall be rejected and processMessage shall return.]*/
TEST_FUNCTION(DoWork_messages_notsubscribed_failed)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    proton_messenger_init_path(mocks, TEST_HAPPY_PATH);
    mocksFor_rollbackEvent(mocks, 1);
    mock_renewHappyPath(mocks);
    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(0); // From the call to renew if necessary.
    setupAmqpMessengerDispose(mocks, 2);
    mocksFor_protonWork(mocks, TEST_HAPPY_PATH);
    mocksFor_NoTelementry(mocks);
    mocksFor_processReceives(mocks, 1);

    PAMQP_TRANSPORT_STATE transportHandle = (PAMQP_TRANSPORT_STATE)transportFunctions->IoTHubTransport_Create(&config);
    ASSERT_IS_NOT_NULL(transportHandle);
    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE); // This gets through initalization.

    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(0); // From the call to renew if necessary.
    transportHandle->waitingForPutTokenReply = true; // We do this so that it will fall into the dispatch code.
    mocksFor_NoTelementry(mocks);
    mocksFor_processReceives(mocks, 4);
    mocksFor_processMessages(mocks, 111, false);
    mocksFor_protonWork(mocks, TEST_HAPPY_PATH);

    // act

    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}


/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_049: [processReceives shall acquire a tracker by invoking pn_messenger_incoming_tracker.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_050: [processMessage will acquire the body of the message by invoking pn_message_body.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_051: [If the body is NULL then the message will be abandoned.]*/
TEST_FUNCTION(DoWork_messages_subScribed_no_body_found_failed)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    proton_messenger_init_path(mocks, TEST_HAPPY_PATH);
    mocksFor_rollbackEvent(mocks, 1);
    mock_renewHappyPath(mocks);
    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(0); // From the call to renew if necessary.
    setupAmqpMessengerDispose(mocks, 2);
    mocksFor_protonWork(mocks, TEST_HAPPY_PATH);
    mocksFor_NoTelementry(mocks);
    mocksFor_processReceives(mocks, 4);
    mocksFor_processMessages(mocks, 1, false);

    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    ASSERT_IS_NOT_NULL(transportHandle);
    transportFunctions->IoTHubTransport_Subscribe(transportHandle);
    // act

    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}


/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_055: [If the body is NOT null then processMessage will check that the body is of type PN_BINARY.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_056: [processMessage will abandon the message with body types that are NOT PN_BINARY, the IOTHUB_MESSAGE_HANDLE will be destroyed and the loop will be continued.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_063: [processMessage will destroy the IOTHUB_MESSAGE_HANDLE by invoking IoTHubMessage_Destroy.]*/
TEST_FUNCTION(DoWork_messages_subScribed_bad_data_type_fails)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    proton_messenger_init_path(mocks, TEST_HAPPY_PATH);
    mocksFor_rollbackEvent(mocks, 1);
    mock_renewHappyPath(mocks);
    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(0); // From the call to renew if necessary.
    setupAmqpMessengerDispose(mocks, 2);
    mocksFor_protonWork(mocks, TEST_HAPPY_PATH);
    mocksFor_NoTelementry(mocks);
    mocksFor_processReceives(mocks, 4);
    mocksFor_processMessages(mocks, 3, false);

    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    transportFunctions->IoTHubTransport_Subscribe(transportHandle);

    // act
    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_054: [processMessage will check for a null (note the lowercase) body by invoking pn_data_next.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_058: [The IOTHUB_MESSAGE_HANDLE will be set by invoking IotHubMessage_SetData with the previously saved length and pointer to payload.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_059: [processMessage will abandon the message if IotHubMessage_SetData returns any status other than IOTHUB_MESSAGE_OK.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_063: [processMessage will destroy the IOTHUB_MESSAGE_HANDLE by invoking IoTHubMessage_Destroy.]*/
TEST_FUNCTION(DoWork_messages_subScribed_null_message_message_set_fails)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    proton_messenger_init_path(mocks, TEST_HAPPY_PATH);
    mocksFor_rollbackEvent(mocks, 1);
    mock_renewHappyPath(mocks);
    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(0); // From the call to renew if necessary.
    setupAmqpMessengerDispose(mocks, 2);
    mocksFor_protonWork(mocks, TEST_HAPPY_PATH);
    mocksFor_NoTelementry(mocks);
    mocksFor_processReceives(mocks, 4);
    mocksFor_processMessages(mocks, 4, true);

    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    transportFunctions->IoTHubTransport_Subscribe(transportHandle);

    // act
    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_054: [processMessage will check for a null (note the lowercase) body by invoking pn_data_next.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_057: [The length and a pointer to the payload will be acquired and saved by invoking pn_data_get_binary.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_058: [The IOTHUB_MESSAGE_HANDLE will be set by invoking IotHubMessage_SetData with the previously saved length and pointer to payload.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_059: [processMessage will abandon the message if IotHubMessage_SetData returns any status other than IOTHUB_MESSAGE_OK.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_063: [processMessage will destroy the IOTHUB_MESSAGE_HANDLE by invoking IoTHubMessage_Destroy.]*/
TEST_FUNCTION(DoWork_messages_subScribed_nonnull_message_message_set_fails)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    proton_messenger_init_path(mocks, TEST_HAPPY_PATH);
    mocksFor_rollbackEvent(mocks, 1);
    mock_renewHappyPath(mocks);
    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(0); // From the call to renew if necessary.
    setupAmqpMessengerDispose(mocks, 2);
    mocksFor_protonWork(mocks, TEST_HAPPY_PATH);
    mocksFor_NoTelementry(mocks);
    mocksFor_processReceives(mocks, 4);
    mocksFor_processMessages(mocks, 4, false);

    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    ASSERT_IS_NOT_NULL(transportHandle);
    transportFunctions->IoTHubTransport_Subscribe(transportHandle);
    // act

    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_060: [If IOTHUB_MESSAGE_OK had been returned IoTHubClient_LL_MessageCallback will be invoked with the IOTHUB_MESSAGE_HANDLE.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_02_006: [If IoTHubClient_MessageCallback returns IOTHUBMESSAGE_ACCEPTED value then the message will be accepted.] */
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_063: [processMessage will destroy the IOTHUB_MESSAGE_HANDLE by invoking IoTHubMessage_Destroy.]*/
TEST_FUNCTION(DoWork_messages_subScribed_nonnull_message_callbacked_accepted_succeeds)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    proton_messenger_init_path(mocks, TEST_HAPPY_PATH);
    mocksFor_rollbackEvent(mocks, 1);
    mock_renewHappyPath(mocks);
    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(0); // From the call to renew if necessary.
    setupAmqpMessengerDispose(mocks, 2);
    mocksFor_protonWork(mocks, TEST_HAPPY_PATH);
    mocksFor_NoTelementry(mocks);
    mocksFor_processReceives(mocks, 4);
    mocksFor_processMessages(mocks, 8, false);
    EXPECTED_CALL(mocks, pn_message_properties(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, pn_data_next(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, pn_data_get_map(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, pn_message_get_id(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, pn_message_get_correlation_id(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, IoTHubMessage_SetMessageId(TEST_IOTHUB_MESSAGE_HANDLE, IGNORED_PTR_ARG));

    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    transportFunctions->IoTHubTransport_Subscribe(transportHandle);

    // act
    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_060: [If IOTHUB_MESSAGE_OK had been returned IoTHubClient_LL_MessageCallback will be invoked with the IOTHUB_MESSAGE_HANDLE.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_061: [If IoTHubClient_MessageCallback returns IOTHUBMESSAGE_REJECTED value then the message will be rejected.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_063: [processMessage will destroy the IOTHUB_MESSAGE_HANDLE by invoking IoTHubMessage_Destroy.]*/
TEST_FUNCTION(DoWork_messages_subScribed_nonnull_message_callbacked_rejected_succeeds)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    proton_messenger_init_path(mocks, TEST_HAPPY_PATH);
    mocksFor_rollbackEvent(mocks, 1);
    mock_renewHappyPath(mocks);
    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(0); // From the call to renew if necessary.
    setupAmqpMessengerDispose(mocks, 2);
    mocksFor_protonWork(mocks, TEST_HAPPY_PATH);
    mocksFor_NoTelementry(mocks);
    mocksFor_processReceives(mocks, 4);
    mocksFor_processMessages(mocks, 10, false);
    EXPECTED_CALL(mocks, pn_message_properties(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, pn_data_next(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, pn_data_get_map(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, pn_message_get_id(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, pn_message_get_correlation_id(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, IoTHubMessage_SetMessageId(TEST_IOTHUB_MESSAGE_HANDLE, IGNORED_PTR_ARG));

    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    transportFunctions->IoTHubTransport_Subscribe(transportHandle);

    // act
    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_02_007: [If IoTHubClient_MessageCallback returns IOTHUBMESSAGE_ABANDONED value then the message will be abandoned.] */
TEST_FUNCTION(DoWork_messages_subScribed_nonnull_message_callbacked_abandon_succeeds)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, SIZE_MAX);
    proton_messenger_init_path(mocks, SIZE_MAX);
    mocksFor_rollbackEvent(mocks, 1);
    mock_renewHappyPath(mocks);
    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(0); // From the call to renew if necessary.
    setupAmqpMessengerDispose(mocks, 2);
    mocksFor_protonWork(mocks, TEST_HAPPY_PATH);
    mocksFor_NoTelementry(mocks);
    mocksFor_processReceives(mocks, 4);
    mocksFor_processMessages(mocks, 11, false);
    EXPECTED_CALL(mocks, pn_message_properties(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, pn_data_next(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, pn_data_get_map(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, pn_message_get_id(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, pn_message_get_correlation_id(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, IoTHubMessage_SetMessageId(TEST_IOTHUB_MESSAGE_HANDLE, IGNORED_PTR_ARG));

    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    transportFunctions->IoTHubTransport_Subscribe(transportHandle);

    // act
    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_073: [While the availableWorkItems is NOT empty loop.]   */
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_074: [If there is a waitingToSend entry remove it from the list.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_074: [If there is a waitingToSend entry remove it from the list.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_075: [Get its size and payload address by calling IoTHubMessage_GetData.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_076: [If that fails then fail that event.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_078: [If the event failed call invoke IotHubClient_SendBatchComplete as above utilizing the eventMessages list of the head of the availableWorkItems and a status of IOTHUB_BATCHSTATE_FAILED.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_079: [Continue this send loop.]*/
TEST_FUNCTION(DoWork_SendEvent_GetData_Fail)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    IOTHUB_MESSAGE_LIST TEST_SIMPLE_MESSAGE_LIST = { TEST_IOTHUB_MESSAGE_HANDLE, NULL, NULL };
    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    BASEIMPLEMENTATION::DList_InsertTailList(&wts, &TEST_SIMPLE_MESSAGE_LIST.entry);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    proton_messenger_init_path(mocks, TEST_HAPPY_PATH);
    mocksFor_rollbackEvent(mocks, 1);
    mock_renewHappyPath(mocks);
    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(0); // From the call to renew if necessary.
    setupAmqpMessengerDispose(mocks, 2);
    STRICT_EXPECTED_CALL(mocks, pn_messenger_recv(TEST_PN_MESSENGER, 0));
    mocksFor_protonWork(mocks, TEST_HAPPY_PATH);
    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1);
    setupAmqpPrepareBatch(mocks, 1);

    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    // act
    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_073: [While the availableWorkItems is NOT empty loop.]   */
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_074: [If there is a waitingToSend entry remove it from the list.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_074: [If there is a waitingToSend entry remove it from the list.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_075: [Get its size and payload address by calling IoTHubMessage_GetData.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_076: [If that fails then fail that event.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_078: [If the event failed call invoke IotHubClient_SendBatchComplete as above utilizing the eventMessages list of the head of the availableWorkItems and a status of IOTHUB_BATCHSTATE_FAILED.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_079: [Continue this send loop.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_02_008: [If the message type is IOTHUBMESSAGE_STRING then get the data by using IoTHubMesage_GetString and set the size to the length of the string.*/
TEST_FUNCTION(DoWork_SendEvent_GetString_Fail)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    IOTHUB_MESSAGE_LIST TEST_SIMPLE_MESSAGE_LIST = { TEST_MESSAGE_HANDLE_STRING, NULL, NULL };
    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    BASEIMPLEMENTATION::DList_InsertTailList(&wts, &TEST_SIMPLE_MESSAGE_LIST.entry);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    proton_messenger_init_path(mocks, TEST_HAPPY_PATH);
    mocksFor_rollbackEvent(mocks, 1);
    mock_renewHappyPath(mocks);
    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(0); // From the call to renew if necessary.
    setupAmqpMessengerDispose(mocks, 2);
    STRICT_EXPECTED_CALL(mocks, pn_messenger_recv(TEST_PN_MESSENGER, 0));
    mocksFor_protonWork(mocks, TEST_HAPPY_PATH);
    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1);
    setupAmqpPrepareBatch(mocks, 1, TEST_MESSAGE_HANDLE_STRING);

    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    // act
    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_073: [While the availableWorkItems is NOT empty loop.]   */
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_074: [If there is a waitingToSend entry remove it from the list.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_074: [If there is a waitingToSend entry remove it from the list.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_075: [Get its size and payload address by calling IoTHubMessage_GetData.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_077: [If the size of the payload is greater than the maximum (currently 256*1024) fail that event.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_078: [If the event failed call invoke IotHubClient_SendBatchComplete as above utilizing the eventMessages list of the head of the availableWorkItems and a status of IOTHUB_BATCHSTATE_FAILED.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_079: [Continue this send loop.]*/
TEST_FUNCTION(DoWork_SendEvent_GetData_Way_Too_Big_Fail)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    IOTHUB_MESSAGE_LIST TEST_WAYBIG_MESSAGE_LIST = { TEST_MESSAGE_HANDLE_OVERIDE_DEFAULTS, NULL, NULL };
    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    BASEIMPLEMENTATION::DList_InsertTailList(&wts, &TEST_WAYBIG_MESSAGE_LIST.entry);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    proton_messenger_init_path(mocks, TEST_HAPPY_PATH);
    mocksFor_rollbackEvent(mocks, 1);
    mock_renewHappyPath(mocks);
    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(0); // From the call to renew if necessary.
    setupAmqpMessengerDispose(mocks, 2);
    STRICT_EXPECTED_CALL(mocks, pn_messenger_recv(TEST_PN_MESSENGER, 0));
    mocksFor_protonWork(mocks, TEST_HAPPY_PATH);
    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1);
    setupAmqpPrepareBatch(mocks, 2, TEST_MESSAGE_HANDLE_OVERIDE_DEFAULTS);

    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    // act
    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_073: [While the availableWorkItems is NOT empty loop.]   */
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_074: [If there is a waitingToSend entry remove it from the list.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_074: [If there is a waitingToSend entry remove it from the list.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_075: [Get its size and payload address by calling IoTHubMessage_GetData.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_080: [If the event did not fail then prepare a proton message using the following proton API: pn_message_set_address, pn_message_set_inferred, pn_message_body and pn_data_put_binary with the above saved size and address values.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_081: [sendEvent will take the item that had been the head of the waitingToSend and put it back at the head of the waitingToSend list.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_082: [sendEvent will then break out of the send loop.]*/
TEST_FUNCTION(DoWork_SendEvent_proton_set_message_fail)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    IOTHUB_MESSAGE_LIST TEST_SIMPLE_MESSAGE_LIST = { TEST_IOTHUB_MESSAGE_HANDLE, NULL, NULL };
    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    BASEIMPLEMENTATION::DList_InsertTailList(&wts, &TEST_SIMPLE_MESSAGE_LIST.entry);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    proton_messenger_init_path(mocks, TEST_HAPPY_PATH);
    mocksFor_rollbackEvent(mocks, 1);
    mock_renewHappyPath(mocks);
    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(0); // From the call to renew if necessary.
    setupAmqpMessengerDispose(mocks, 2);
    STRICT_EXPECTED_CALL(mocks, pn_messenger_recv(TEST_PN_MESSENGER, 0));
    mocksFor_protonWork(mocks, TEST_HAPPY_PATH);
    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1);
    setupAmqpPrepareBatch(mocks, TEST_HAPPY_PATH);
    setupAmqpPrepareSimpleProtonMessage(mocks, 1);

    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    // act
    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // assert
    ASSERT_ARE_EQUAL(int, BASEIMPLEMENTATION::DList_IsListEmpty(&wts), 0);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_073: [While the availableWorkItems is NOT empty loop.]   */
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_074: [If there is a waitingToSend entry remove it from the list.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_074: [If there is a waitingToSend entry remove it from the list.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_075: [Get its size and payload address by calling IoTHubMessage_GetData.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_080: [If the event did not fail then prepare a proton message using the following proton API: pn_message_set_address, pn_message_set_inferred, pn_message_body and pn_data_put_binary with the above saved size and address values.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_081: [sendEvent will take the item that had been the head of the waitingToSend and put it back at the head of the waitingToSend list.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_082: [sendEvent will then break out of the send loop.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_02_008: [If the message type is IOTHUBMESSAGE_STRING then get the data by using IoTHubMesage_GetString and set the size to the length of the string.*/
TEST_FUNCTION(DoWork_SendEvent_proton_set_message_with_STRING_fail)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    IOTHUB_MESSAGE_LIST TEST_SIMPLE_MESSAGE_LIST = { TEST_MESSAGE_HANDLE_STRING, NULL, NULL };
    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    BASEIMPLEMENTATION::DList_InsertTailList(&wts, &TEST_SIMPLE_MESSAGE_LIST.entry);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    proton_messenger_init_path(mocks, TEST_HAPPY_PATH);
    mocksFor_rollbackEvent(mocks, 1);
    mock_renewHappyPath(mocks);
    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(0); // From the call to renew if necessary.
    setupAmqpMessengerDispose(mocks, 2);
    STRICT_EXPECTED_CALL(mocks, pn_messenger_recv(TEST_PN_MESSENGER, 0));
    mocksFor_protonWork(mocks, TEST_HAPPY_PATH);
    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1);
    setupAmqpPrepareBatch(mocks, TEST_HAPPY_PATH, TEST_MESSAGE_HANDLE_STRING);
    setupAmqpPrepareSimpleProtonMessage(mocks, 1);

    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    // act
    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // assert
    ASSERT_ARE_EQUAL(int, BASEIMPLEMENTATION::DList_IsListEmpty(&wts), 0);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_073: [While the availableWorkItems is NOT empty loop.]   */
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_074: [If there is a waitingToSend entry remove it from the list.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_074: [If there is a waitingToSend entry remove it from the list.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_075: [Get its size and payload address by calling IoTHubMessage_GetData.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_080: [If the event did not fail then prepare a proton message using the following proton API: pn_message_set_address, pn_message_set_inferred, pn_message_body and pn_data_put_binary with the above saved size and address values.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_081: [sendEvent will take the item that had been the head of the waitingToSend and put it back at the head of the waitingToSend list.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_082: [sendEvent will then break out of the send loop.]*/
TEST_FUNCTION(DoWork_SendEvent_proton_set_inferred_fail)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    IOTHUB_MESSAGE_LIST TEST_SIMPLE_MESSAGE_LIST = { TEST_IOTHUB_MESSAGE_HANDLE, NULL, NULL };
    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    BASEIMPLEMENTATION::DList_InsertTailList(&wts, &TEST_SIMPLE_MESSAGE_LIST.entry);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    proton_messenger_init_path(mocks, TEST_HAPPY_PATH);
    mocksFor_rollbackEvent(mocks, 1);
    mock_renewHappyPath(mocks);
    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(0); // From the call to renew if necessary.
    setupAmqpMessengerDispose(mocks, 2);
    STRICT_EXPECTED_CALL(mocks, pn_messenger_recv(TEST_PN_MESSENGER, 0));
    mocksFor_protonWork(mocks, TEST_HAPPY_PATH);
    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1);
    setupAmqpPrepareBatch(mocks, TEST_HAPPY_PATH);
    setupAmqpPrepareSimpleProtonMessage(mocks, 2);

    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    // act
    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // assert
    ASSERT_ARE_EQUAL(int, BASEIMPLEMENTATION::DList_IsListEmpty(&wts), 0);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_073: [While the availableWorkItems is NOT empty loop.]   */
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_074: [If there is a waitingToSend entry remove it from the list.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_074: [If there is a waitingToSend entry remove it from the list.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_075: [Get its size and payload address by calling IoTHubMessage_GetData.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_080: [If the event did not fail then prepare a proton message using the following proton API: pn_message_set_address, pn_message_set_inferred, pn_message_body and pn_data_put_binary with the above saved size and address values.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_081: [sendEvent will take the item that had been the head of the waitingToSend and put it back at the head of the waitingToSend list.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_082: [sendEvent will then break out of the send loop.]*/
TEST_FUNCTION(DoWork_SendEvent_proton_message_body_fail)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    IOTHUB_MESSAGE_LIST TEST_SIMPLE_MESSAGE_LIST = { TEST_IOTHUB_MESSAGE_HANDLE, NULL, NULL };
    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    BASEIMPLEMENTATION::DList_InsertTailList(&wts, &TEST_SIMPLE_MESSAGE_LIST.entry);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    proton_messenger_init_path(mocks, TEST_HAPPY_PATH);
    mocksFor_rollbackEvent(mocks, 1);
    mock_renewHappyPath(mocks);
    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(0); // From the call to renew if necessary.
    setupAmqpMessengerDispose(mocks, 2);
    STRICT_EXPECTED_CALL(mocks, pn_messenger_recv(TEST_PN_MESSENGER, 0));
    mocksFor_protonWork(mocks, TEST_HAPPY_PATH);
    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1);
    setupAmqpPrepareBatch(mocks, TEST_HAPPY_PATH);
    setupAmqpPrepareSimpleProtonMessage(mocks, 3);

    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    // act
    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // assert
    ASSERT_ARE_EQUAL(int, BASEIMPLEMENTATION::DList_IsListEmpty(&wts), 0);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_073: [While the availableWorkItems is NOT empty loop.]   */
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_074: [If there is a waitingToSend entry remove it from the list.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_074: [If there is a waitingToSend entry remove it from the list.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_075: [Get its size and payload address by calling IoTHubMessage_GetData.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_080: [If the event did not fail then prepare a proton message using the following proton API: pn_message_set_address, pn_message_set_inferred, pn_message_body and pn_data_put_binary with the above saved size and address values.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_081: [sendEvent will take the item that had been the head of the waitingToSend and put it back at the head of the waitingToSend list.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_082: [sendEvent will then break out of the send loop.]*/
TEST_FUNCTION(DoWork_SendEvent_proton_data_put_binary_fail)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    IOTHUB_MESSAGE_LIST TEST_SIMPLE_MESSAGE_LIST = { TEST_IOTHUB_MESSAGE_HANDLE, NULL, NULL };
    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    BASEIMPLEMENTATION::DList_InsertTailList(&wts, &TEST_SIMPLE_MESSAGE_LIST.entry);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    proton_messenger_init_path(mocks, TEST_HAPPY_PATH);
    mocksFor_rollbackEvent(mocks, 1);
    mock_renewHappyPath(mocks);
    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(0); // From the call to renew if necessary.
    setupAmqpMessengerDispose(mocks, 2);
    STRICT_EXPECTED_CALL(mocks, pn_messenger_recv(TEST_PN_MESSENGER, 0));
    mocksFor_protonWork(mocks, TEST_HAPPY_PATH);
    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1);
    setupAmqpPrepareBatch(mocks, TEST_HAPPY_PATH);
    setupAmqpPrepareSimpleProtonMessage(mocks, 4);

    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    // act
    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // assert
    ASSERT_ARE_EQUAL(int, BASEIMPLEMENTATION::DList_IsListEmpty(&wts), 0);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_073: [While the availableWorkItems is NOT empty loop.]   */
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_074: [If there is a waitingToSend entry remove it from the list.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_074: [If there is a waitingToSend entry remove it from the list.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_075: [Get its size and payload address by calling IoTHubMessage_GetData.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_083: [If the proton API is successful then invoke pn_messenger_put.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_084: [If that fails, sendEvent will take the item that had been the head of the waitingToSend and put it back at the head of the waitingToSend list.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_085: [sendEvent will then break out of the send loop.]*/
TEST_FUNCTION(DoWork_SendEvent_proton_messenger_put_fail)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    IOTHUB_MESSAGE_LIST TEST_SIMPLE_MESSAGE_LIST = { TEST_IOTHUB_MESSAGE_HANDLE, NULL, NULL };
    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    BASEIMPLEMENTATION::DList_InsertTailList(&wts, &TEST_SIMPLE_MESSAGE_LIST.entry);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    proton_messenger_init_path(mocks, TEST_HAPPY_PATH);
    mocksFor_rollbackEvent(mocks, 1);
    mock_renewHappyPath(mocks);
    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(0); // From the call to renew if necessary.
    setupAmqpMessengerDispose(mocks, 2);
    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1);
    setupAmqpPrepareBatch(mocks, TEST_HAPPY_PATH);
    setupAmqpPrepareSimpleProtonMessage(mocks, TEST_HAPPY_PATH);
    STRICT_EXPECTED_CALL(mocks, pn_messenger_put(TEST_PN_MESSENGER, TEST_PN_MESSAGE)).SetReturn(1);
    mocksFor_putSecondAfterFirst(mocks);
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Properties(TEST_IOTHUB_MESSAGE_HANDLE));
    STRICT_EXPECTED_CALL(mocks, Map_GetInternals(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG)).IgnoreAllArguments();

    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetMessageId(TEST_IOTHUB_MESSAGE_HANDLE));
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetCorrelationId(TEST_IOTHUB_MESSAGE_HANDLE));

    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    // act
    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // assert
    ASSERT_ARE_EQUAL(int, BASEIMPLEMENTATION::DList_IsListEmpty(&wts), 0);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_073: [While the availableWorkItems is NOT empty loop.]   */
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_074: [If there is a waitingToSend entry remove it from the list.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_074: [If there is a waitingToSend entry remove it from the list.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_075: [Get its size and payload address by calling IoTHubMessage_GetData.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_083: [If the proton API is successful then invoke pn_messenger_put.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_087: [The head of the availableWorkItems will be removed and placed on the workInProgress list.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_086: [If the pn_messenger_put succeeds then sendEvent will save off a tracker obtained by invoking pn_messenger_outgoing_tracker and save it the head of the availableWorkItems.  The eventMessages in the AMQP_WORK_ITEM head will also contain the IOTHUB_MESSAGE_LIST.]*/
TEST_FUNCTION(DoWork_one_invoke_cleanOut_in_progress_SendEvent_proton_messenger_put_succeed)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    IOTHUB_MESSAGE_LIST TEST_SIMPLE_MESSAGE_LIST = { TEST_IOTHUB_MESSAGE_HANDLE, NULL, NULL };
    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    BASEIMPLEMENTATION::DList_InsertTailList(&wts, &TEST_SIMPLE_MESSAGE_LIST.entry);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    proton_messenger_init_path(mocks, TEST_HAPPY_PATH);
    mocksFor_rollbackEvent(mocks, 1);
    mock_renewHappyPath(mocks);
    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(0); // From the call to renew if necessary.
    setupAmqpMessengerDispose(mocks, 2);
    STRICT_EXPECTED_CALL(mocks, pn_messenger_recv(TEST_PN_MESSENGER, 0));
    mocksFor_protonWork(mocks, TEST_HAPPY_PATH);
    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1);
    setupAmqpPrepareBatch(mocks, TEST_HAPPY_PATH);
    setupAmqpPrepareSimpleProtonMessage(mocks, TEST_HAPPY_PATH);
    STRICT_EXPECTED_CALL(mocks, pn_messenger_put(TEST_PN_MESSENGER, TEST_PN_MESSAGE));
    STRICT_EXPECTED_CALL(mocks, get_time(NULL));
    STRICT_EXPECTED_CALL(mocks, pn_messenger_outgoing_tracker(TEST_PN_MESSENGER));
    STRICT_EXPECTED_CALL(mocks, DList_RemoveHeadList(IGNORED_PTR_ARG)).IgnoreAllArguments(); // Remove it from available.
    STRICT_EXPECTED_CALL(mocks, DList_InsertTailList(IGNORED_PTR_ARG, IGNORED_PTR_ARG)).IgnoreAllArguments(); // Insert it on the in progress
    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1); // Look again in the available.
    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1); // Look again to see if any on waiting to send.
    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1); //Second pass through in progress
    STRICT_EXPECTED_CALL(mocks, DList_RemoveHeadList(IGNORED_PTR_ARG)).IgnoreAllArguments(); // Remove from work in progress
    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_SendComplete(TEST_IOTHUB_CLIENT_LL_HANDLE, IGNORED_PTR_ARG, IOTHUB_BATCHSTATE_FAILED)).IgnoreArgument(2); // Complete the in progress one.
    STRICT_EXPECTED_CALL(mocks, DList_InsertTailList(IGNORED_PTR_ARG, IGNORED_PTR_ARG)).IgnoreAllArguments(); // Put it on the available.
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Properties(TEST_IOTHUB_MESSAGE_HANDLE));
    STRICT_EXPECTED_CALL(mocks, Map_GetInternals(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG)).IgnoreAllArguments();

    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetMessageId(TEST_IOTHUB_MESSAGE_HANDLE));
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetCorrelationId(TEST_IOTHUB_MESSAGE_HANDLE));

    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    // act
    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, BASEIMPLEMENTATION::DList_IsListEmpty(&wts), 0);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_073: [While the availableWorkItems is NOT empty loop.]   */
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_074: [If there is a waitingToSend entry remove it from the list.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_074: [If there is a waitingToSend entry remove it from the list.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_075: [Get its size and payload address by calling IoTHubMessage_GetData.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_083: [If the proton API is successful then invoke pn_messenger_put.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_084: [If that fails, sendEvent will take the item that had been the head of the waitingToSend and put it back at the head of the waitingToSend list.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_085: [sendEvent will then break out of the send loop.]*/
/*Tess that with a two item waiting to send that when we resource fail the first it goes back to the head.*/
TEST_FUNCTION(DoWork_SendEvent_proton_messenger_put_fail_with_multiple_waiting_to_send_coherent_wts)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    IOTHUB_MESSAGE_LIST TEST_SIMPLE_MESSAGE_LIST = { TEST_IOTHUB_MESSAGE_HANDLE, NULL, NULL };
    IOTHUB_MESSAGE_LIST TEST_SIMPLE_MESSAGE_LIST2 = { TEST_IOTHUB_MESSAGE_HANDLE, NULL, NULL };
    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    BASEIMPLEMENTATION::DList_InsertTailList(&wts, &TEST_SIMPLE_MESSAGE_LIST.entry);
    BASEIMPLEMENTATION::DList_InsertTailList(&wts, &TEST_SIMPLE_MESSAGE_LIST2.entry);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    proton_messenger_init_path(mocks, TEST_HAPPY_PATH);
    mocksFor_rollbackEvent(mocks, 1);
    mock_renewHappyPath(mocks);
    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(0); // From the call to renew if necessary.
    setupAmqpMessengerDispose(mocks, 2);
    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1);
    setupAmqpPrepareBatch(mocks, TEST_HAPPY_PATH);
    setupAmqpPrepareSimpleProtonMessage(mocks, TEST_HAPPY_PATH);
    STRICT_EXPECTED_CALL(mocks, pn_messenger_put(TEST_PN_MESSENGER, TEST_PN_MESSAGE)).SetReturn(1);
    mocksFor_putSecondAfterFirst(mocks);
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Properties(TEST_IOTHUB_MESSAGE_HANDLE));
    STRICT_EXPECTED_CALL(mocks, Map_GetInternals(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG)).IgnoreAllArguments();

    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetMessageId(TEST_IOTHUB_MESSAGE_HANDLE));
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetCorrelationId(TEST_IOTHUB_MESSAGE_HANDLE));

    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    // act
    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // assert
    ASSERT_ARE_EQUAL(int, BASEIMPLEMENTATION::DList_IsListEmpty(&wts), 0);
    PDLIST_ENTRY firstOnList = BASEIMPLEMENTATION::DList_RemoveHeadList(&wts);
    ASSERT_ARE_EQUAL(int, BASEIMPLEMENTATION::DList_IsListEmpty(&wts), 0);
    PDLIST_ENTRY secondOnList = BASEIMPLEMENTATION::DList_RemoveHeadList(&wts);
    ASSERT_ARE_NOT_EQUAL(int, BASEIMPLEMENTATION::DList_IsListEmpty(&wts), 0);
    ASSERT_ARE_EQUAL(void_ptr, firstOnList, &TEST_SIMPLE_MESSAGE_LIST.entry);
    ASSERT_ARE_EQUAL(void_ptr, secondOnList, &TEST_SIMPLE_MESSAGE_LIST2.entry);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_073: [While the availableWorkItems is NOT empty loop.]   */
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_074: [If there is a waitingToSend entry remove it from the list.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_074: [If there is a waitingToSend entry remove it from the list.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_075: [Get its size and payload address by calling IoTHubMessage_GetData.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_080: [If the event did not fail then prepare a proton message using the following proton API: pn_message_set_address, pn_message_set_inferred, pn_message_body and pn_data_put_binary with the above saved size and address values.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_081: [sendEvent will take the item that had been the head of the waitingToSend and put it back at the head of the waitingToSend list.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_082: [sendEvent will then break out of the send loop.]*/
/* Tess that with a two item waiting to send that when we resource fail the first it goes back to the head.*/
TEST_FUNCTION(DoWork_SendEvent_proton_data_put_binary_fail_with_two_wts)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    IOTHUB_MESSAGE_LIST TEST_SIMPLE_MESSAGE_LIST = { TEST_IOTHUB_MESSAGE_HANDLE, NULL, NULL };
    IOTHUB_MESSAGE_LIST TEST_SIMPLE_MESSAGE_LIST2 = { TEST_IOTHUB_MESSAGE_HANDLE, NULL, NULL };
    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    BASEIMPLEMENTATION::DList_InsertTailList(&wts, &TEST_SIMPLE_MESSAGE_LIST.entry);
    BASEIMPLEMENTATION::DList_InsertTailList(&wts, &TEST_SIMPLE_MESSAGE_LIST2.entry);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    proton_messenger_init_path(mocks, TEST_HAPPY_PATH);
    mocksFor_rollbackEvent(mocks, 1);
    mock_renewHappyPath(mocks);
    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(0); // From the call to renew if necessary.
    setupAmqpMessengerDispose(mocks, 2);
    STRICT_EXPECTED_CALL(mocks, pn_messenger_recv(TEST_PN_MESSENGER, 0));
    mocksFor_protonWork(mocks, TEST_HAPPY_PATH);
    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1);
    setupAmqpPrepareBatch(mocks, TEST_HAPPY_PATH);
    setupAmqpPrepareSimpleProtonMessage(mocks, 4);

    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    // act
    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // assert
    ASSERT_ARE_EQUAL(int, BASEIMPLEMENTATION::DList_IsListEmpty(&wts), 0);
    PDLIST_ENTRY firstOnList = BASEIMPLEMENTATION::DList_RemoveHeadList(&wts);
    ASSERT_ARE_EQUAL(int, BASEIMPLEMENTATION::DList_IsListEmpty(&wts), 0);
    PDLIST_ENTRY secondOnList = BASEIMPLEMENTATION::DList_RemoveHeadList(&wts);
    ASSERT_ARE_NOT_EQUAL(int, BASEIMPLEMENTATION::DList_IsListEmpty(&wts), 0);
    ASSERT_ARE_EQUAL(void_ptr, firstOnList, &TEST_SIMPLE_MESSAGE_LIST.entry);
    ASSERT_ARE_EQUAL(void_ptr, secondOnList, &TEST_SIMPLE_MESSAGE_LIST2.entry);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_064: [Obtain the head of the workInProgress]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_065: [While head != listhead reclaimEventResources will do as follows.] */
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_066: [Check the current status via pn_messenger_status via the tracker stored in the AMQP_WORK_ITEM.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_068: [Otherwise reclaimEventResources will use the result of IOTHUB_BATCHSTATE_SUCCESS if the proton status was PN_STATUS_ACCEPTED.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_069: [Otherwise reclaimEventResources will use the status of IOTHUB_BATCHSTATE_FAILED if the proton status was NOT PN_STATUS_ACCEPTED.] */
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_070: [reclaimEventResources will invoke IotHubClient_SendBatchComplete with the IOTHUB_CLIENT_LL_HANDLE, supplied to reclaimEventResources, also pass the eventMessages stored in the AMQP_WORK_ITEM, and finally pass the above IOTHUB_BATCH status.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_071: [reclaimEventResources will take the current work item from the workInProgress list and insert it on the tail of the availableWorkItems list.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_072: [reclaimEventResources will continue to iterate over the workInProgress list until it returns to the list head.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_090: [reclaimEventResources will release the tracker by invoking pn_messenger_settle.]*/
TEST_FUNCTION(reclaimEventResources_reclaim_work_in_progress_succeed)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    IOTHUB_MESSAGE_LIST TEST_SIMPLE_MESSAGE_LIST = { TEST_IOTHUB_MESSAGE_HANDLE, NULL, NULL };
    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    BASEIMPLEMENTATION::DList_InsertTailList(&wts, &TEST_SIMPLE_MESSAGE_LIST.entry);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    proton_messenger_init_path(mocks, TEST_HAPPY_PATH);
    mocksFor_rollbackEvent(mocks, 1);
    mock_renewHappyPath(mocks);
    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(0); // From the call to renew if necessary.
    setupAmqpMessengerDispose(mocks, 2);
    STRICT_EXPECTED_CALL(mocks, pn_messenger_recv(TEST_PN_MESSENGER, 0));
    mocksFor_protonWork(mocks, TEST_HAPPY_PATH);
    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1);
    setupAmqpPrepareBatch(mocks, TEST_HAPPY_PATH);
    setupAmqpPrepareSimpleProtonMessage(mocks, TEST_HAPPY_PATH);
    STRICT_EXPECTED_CALL(mocks, pn_messenger_put(TEST_PN_MESSENGER, TEST_PN_MESSAGE));
    STRICT_EXPECTED_CALL(mocks, get_time(NULL));
    STRICT_EXPECTED_CALL(mocks, pn_messenger_outgoing_tracker(TEST_PN_MESSENGER));
    STRICT_EXPECTED_CALL(mocks, DList_RemoveHeadList(IGNORED_PTR_ARG)).IgnoreAllArguments(); // Remove it from available.
    STRICT_EXPECTED_CALL(mocks, DList_InsertTailList(IGNORED_PTR_ARG, IGNORED_PTR_ARG)).IgnoreAllArguments(); // Insert it on the in progress
    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1); // Look again in the available.
    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1); // Look again to see if any on waiting to send.
    //
    // Second invocation of DoWork
    //
    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(0); // From the call to renew if necessary.
    STRICT_EXPECTED_CALL(mocks, pn_messenger_recv(TEST_PN_MESSENGER, 0));
    mocksFor_protonWork(mocks, TEST_HAPPY_PATH);
    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1); // check of available
    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1); // check of waitingtosend
    STRICT_EXPECTED_CALL(mocks, pn_messenger_status(TEST_PN_MESSENGER, TEST_PN_TRACKER)).SetReturn(PN_STATUS_ACCEPTED);
    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_SendComplete(TEST_IOTHUB_CLIENT_LL_HANDLE, IGNORED_PTR_ARG, IOTHUB_BATCHSTATE_SUCCESS)).IgnoreArgument(2); // Complete the in progress one.
    STRICT_EXPECTED_CALL(mocks, pn_messenger_settle(TEST_PN_MESSENGER, TEST_PN_TRACKER, 0));
    STRICT_EXPECTED_CALL(mocks, DList_RemoveEntryList(IGNORED_PTR_ARG)).IgnoreAllArguments(); // Remove from work in progress
    STRICT_EXPECTED_CALL(mocks, DList_InsertTailList(IGNORED_PTR_ARG, IGNORED_PTR_ARG)).IgnoreAllArguments(); // Put it on the available.
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Properties(TEST_IOTHUB_MESSAGE_HANDLE));
    STRICT_EXPECTED_CALL(mocks, Map_GetInternals(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG)).IgnoreAllArguments();
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetMessageId(TEST_IOTHUB_MESSAGE_HANDLE));
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetCorrelationId(TEST_IOTHUB_MESSAGE_HANDLE));

    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    // act
    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, BASEIMPLEMENTATION::DList_IsListEmpty(&wts), 0);

    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_064: [Obtain the head of the workInProgress]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_065: [While head != listhead reclaimEventResources will do as follows.] */
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_066: [Check the current status via pn_messenger_status via the tracker stored in the AMQP_WORK_ITEM.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_068: [Otherwise reclaimEventResources will use the result of IOTHUB_BATCHSTATE_SUCCESS if the proton status was PN_STATUS_ACCEPTED.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_069: [Otherwise reclaimEventResources will use the status of IOTHUB_BATCHSTATE_FAILED if the proton status was NOT PN_STATUS_ACCEPTED.] */
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_070: [reclaimEventResources will invoke IotHubClient_SendBatchComplete with the IOTHUB_CLIENT_HANDLE, supplied to reclaimEventResources, also pass the eventMessages stored in the AMQP_WORK_ITEM, and finally pass the above IOTHUB_BATCH status.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_071: [reclaimEventResources will take the current work item from the workInProgress list and insert it on the tail of the availableWorkItems list.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_072: [reclaimEventResources will continue to iterate over the workInProgress list until it returns to the list head.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_090: [reclaimEventResources will release the tracker by invoking pn_messenger_settle.]*/
TEST_FUNCTION(reclaimEventResources_reclaim_work_in_progress_settle_fail_succeed)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    IOTHUB_MESSAGE_LIST TEST_SIMPLE_MESSAGE_LIST = { TEST_IOTHUB_MESSAGE_HANDLE, NULL, NULL };
    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    BASEIMPLEMENTATION::DList_InsertTailList(&wts, &TEST_SIMPLE_MESSAGE_LIST.entry);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    proton_messenger_init_path(mocks, TEST_HAPPY_PATH);
    mocksFor_rollbackEvent(mocks, 1);
    mock_renewHappyPath(mocks);
    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(0); // From the call to renew if necessary.
    setupAmqpMessengerDispose(mocks, 2);
    STRICT_EXPECTED_CALL(mocks, pn_messenger_recv(TEST_PN_MESSENGER, 0));
    mocksFor_protonWork(mocks, TEST_HAPPY_PATH);
    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1);
    setupAmqpPrepareBatch(mocks, TEST_HAPPY_PATH);
    setupAmqpPrepareSimpleProtonMessage(mocks, TEST_HAPPY_PATH);
    STRICT_EXPECTED_CALL(mocks, pn_messenger_put(TEST_PN_MESSENGER, TEST_PN_MESSAGE));
    STRICT_EXPECTED_CALL(mocks, get_time(NULL));
    STRICT_EXPECTED_CALL(mocks, pn_messenger_outgoing_tracker(TEST_PN_MESSENGER));
    STRICT_EXPECTED_CALL(mocks, DList_RemoveHeadList(IGNORED_PTR_ARG)).IgnoreAllArguments(); // Remove it from available.
    STRICT_EXPECTED_CALL(mocks, DList_InsertTailList(IGNORED_PTR_ARG, IGNORED_PTR_ARG)).IgnoreAllArguments(); // Insert it on the in progress
    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1); // Look again in the available.
    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1); // Look again to see if any on waiting to send.
    //
    // Second invocation of DoWork
    //
    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(0); // From the call to renew if necessary.
    STRICT_EXPECTED_CALL(mocks, pn_messenger_recv(TEST_PN_MESSENGER, 0));
    mocksFor_protonWork(mocks, TEST_HAPPY_PATH);
    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1); // check of available
    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1); // check of waitingtosend
    STRICT_EXPECTED_CALL(mocks, pn_messenger_status(TEST_PN_MESSENGER, TEST_PN_TRACKER)).SetReturn(PN_STATUS_ACCEPTED);
    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_SendComplete(TEST_IOTHUB_CLIENT_LL_HANDLE, IGNORED_PTR_ARG, IOTHUB_BATCHSTATE_SUCCESS)).IgnoreArgument(2); // Complete the in progress one.
    STRICT_EXPECTED_CALL(mocks, pn_messenger_settle(TEST_PN_MESSENGER, TEST_PN_TRACKER, 0)).SetReturn(1);
    STRICT_EXPECTED_CALL(mocks, DList_RemoveEntryList(IGNORED_PTR_ARG)).IgnoreAllArguments(); // Remove from work in progress
    STRICT_EXPECTED_CALL(mocks, DList_InsertTailList(IGNORED_PTR_ARG, IGNORED_PTR_ARG)).IgnoreAllArguments(); // Put it on the available.
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Properties(TEST_IOTHUB_MESSAGE_HANDLE));
    STRICT_EXPECTED_CALL(mocks, Map_GetInternals(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG)).IgnoreAllArguments();

    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetMessageId(TEST_IOTHUB_MESSAGE_HANDLE));
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetCorrelationId(TEST_IOTHUB_MESSAGE_HANDLE));

    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    // act
    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, BASEIMPLEMENTATION::DList_IsListEmpty(&wts), 0);

    //transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_183: [if the amount of time that the work item has been pending exceeds a timeout stored in the work item, the messenger will be marked as not initialized.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_184: [The work item will also contain a timeout that shall denote how long the transport will wait before initiating error recovery.]*/
TEST_FUNCTION(reclaimEventResources_reclaim_work_in_progress_item_timeout)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    IOTHUB_MESSAGE_LIST TEST_SIMPLE_MESSAGE_LIST = { TEST_IOTHUB_MESSAGE_HANDLE, NULL, NULL };
    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    BASEIMPLEMENTATION::DList_InsertTailList(&wts, &TEST_SIMPLE_MESSAGE_LIST.entry);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    proton_messenger_init_path(mocks, TEST_HAPPY_PATH);
    mocksFor_rollbackEvent(mocks, 1);
    mock_renewHappyPath(mocks);
    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(0); // From the call to renew if necessary.
    setupAmqpMessengerDispose(mocks, 2);
    STRICT_EXPECTED_CALL(mocks, pn_messenger_recv(TEST_PN_MESSENGER, 0));
    mocksFor_protonWork(mocks, TEST_HAPPY_PATH);
    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1);
    setupAmqpPrepareBatch(mocks, TEST_HAPPY_PATH);
    setupAmqpPrepareSimpleProtonMessage(mocks, TEST_HAPPY_PATH);
    STRICT_EXPECTED_CALL(mocks, pn_messenger_put(TEST_PN_MESSENGER, TEST_PN_MESSAGE));
    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(100);
    STRICT_EXPECTED_CALL(mocks, pn_messenger_outgoing_tracker(TEST_PN_MESSENGER));
    STRICT_EXPECTED_CALL(mocks, DList_RemoveHeadList(IGNORED_PTR_ARG)).IgnoreAllArguments(); // Remove it from available.
    STRICT_EXPECTED_CALL(mocks, DList_InsertTailList(IGNORED_PTR_ARG, IGNORED_PTR_ARG)).IgnoreAllArguments(); // Insert it on the in progress
    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1); // Look again in the available.
    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1); // Look again to see if any on waiting to send.
                                                                                       //
                                                                                       // Second invocation of DoWork
                                                                                       //
    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(0); // From the call to renew if necessary.
    STRICT_EXPECTED_CALL(mocks, pn_messenger_status(TEST_PN_MESSENGER, TEST_PN_TRACKER)).SetReturn(PN_STATUS_PENDING);
    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(100 + EVENT_TIMEOUT_DEFAULT + 10); // From the call in reclaim to see how long it's been on the list

    // Since it timed out it it won't get completed by the regular code but by the destroy.
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Properties(TEST_IOTHUB_MESSAGE_HANDLE));
    STRICT_EXPECTED_CALL(mocks, Map_GetInternals(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG)).IgnoreAllArguments();
    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_SendComplete(TEST_IOTHUB_CLIENT_LL_HANDLE, IGNORED_PTR_ARG, IOTHUB_BATCHSTATE_FAILED)).IgnoreArgument(2);
    STRICT_EXPECTED_CALL(mocks, DList_InsertTailList(IGNORED_PTR_ARG, IGNORED_PTR_ARG)).IgnoreAllArguments();
    STRICT_EXPECTED_CALL(mocks, DList_RemoveHeadList(IGNORED_PTR_ARG)).IgnoreAllArguments();
    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetMessageId(TEST_IOTHUB_MESSAGE_HANDLE));
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetCorrelationId(TEST_IOTHUB_MESSAGE_HANDLE));

    PAMQP_TRANSPORT_STATE transportHandle = (PAMQP_TRANSPORT_STATE)transportFunctions->IoTHubTransport_Create(&config);

    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    ASSERT_ARE_NOT_EQUAL(int, BASEIMPLEMENTATION::DList_IsListEmpty(&wts), 0);

    // Act
    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // Assert
    ASSERT_IS_FALSE(transportHandle->messengerInitialized);
    ASSERT_ARE_EQUAL(int, BASEIMPLEMENTATION::DList_IsListEmpty(&transportHandle->workInProgress), 0);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/* Test_SRS_IOTHUBTRANSPORTTAMQP_07_001: [All IoTHubMessage_Properties shall be enumerated and entered in the pn_message_properties Map.] */
TEST_FUNCTION(DoWork_SetProperties_succeed)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    IOTHUB_MESSAGE_LIST TEST_SIMPLE_MESSAGE_LIST = { TEST_IOTHUB_MESSAGE_HANDLE, NULL, NULL };
    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    BASEIMPLEMENTATION::DList_InsertTailList(&wts, &TEST_SIMPLE_MESSAGE_LIST.entry);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    proton_messenger_init_path(mocks, TEST_HAPPY_PATH);
    mocksFor_rollbackEvent(mocks, 1);
    mock_renewHappyPath(mocks);
    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(0); // From the call to renew if necessary.
    setupAmqpMessengerDispose(mocks, 2);
    STRICT_EXPECTED_CALL(mocks, pn_messenger_recv(TEST_PN_MESSENGER, 0));
    mocksFor_protonWork(mocks, TEST_HAPPY_PATH);
    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1);
    setupAmqpPrepareBatch(mocks, TEST_HAPPY_PATH);
    setupAmqpPrepareSimpleProtonMessage(mocks, TEST_HAPPY_PATH);
    STRICT_EXPECTED_CALL(mocks, pn_messenger_put(TEST_PN_MESSENGER, TEST_PN_MESSAGE));
    STRICT_EXPECTED_CALL(mocks, get_time(NULL));
    STRICT_EXPECTED_CALL(mocks, pn_messenger_outgoing_tracker(TEST_PN_MESSENGER));
    STRICT_EXPECTED_CALL(mocks, DList_RemoveHeadList(IGNORED_PTR_ARG)).IgnoreAllArguments(); // Remove it from available.
    STRICT_EXPECTED_CALL(mocks, DList_InsertTailList(IGNORED_PTR_ARG, IGNORED_PTR_ARG)).IgnoreAllArguments(); // Insert it on the in progress
    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1); // Look again in the available.
    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1); // Look again to see if any on waiting to send.
    //
    // Second invocation of DoWork
    //
    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(0); // From the call to renew if necessary.
    STRICT_EXPECTED_CALL(mocks, pn_messenger_recv(TEST_PN_MESSENGER, 0));
    mocksFor_protonWork(mocks, TEST_HAPPY_PATH);
    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1); // check of available
    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1); // check of waitingtosend
    STRICT_EXPECTED_CALL(mocks, pn_messenger_status(TEST_PN_MESSENGER, TEST_PN_TRACKER)).SetReturn(PN_STATUS_ACCEPTED);
    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_SendComplete(TEST_IOTHUB_CLIENT_LL_HANDLE, IGNORED_PTR_ARG, IOTHUB_BATCHSTATE_SUCCESS)).IgnoreArgument(2); // Complete the in progress one.
    STRICT_EXPECTED_CALL(mocks, pn_messenger_settle(TEST_PN_MESSENGER, TEST_PN_TRACKER, 0)).SetReturn(1);
    STRICT_EXPECTED_CALL(mocks, DList_RemoveEntryList(IGNORED_PTR_ARG)).IgnoreAllArguments(); // Remove from work in progress
    STRICT_EXPECTED_CALL(mocks, DList_InsertTailList(IGNORED_PTR_ARG, IGNORED_PTR_ARG)).IgnoreAllArguments(); // Put it on the available.
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Properties(TEST_IOTHUB_MESSAGE_HANDLE)).SetReturn(TEST_MAP_1_PROPERTY);
    STRICT_EXPECTED_CALL(mocks, Map_GetInternals(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG)).IgnoreAllArguments();
    STRICT_EXPECTED_CALL(mocks, pn_message_properties(IGNORED_PTR_ARG)).IgnoreAllArguments();
    STRICT_EXPECTED_CALL(mocks, pn_data_put_map(IGNORED_PTR_ARG)).IgnoreAllArguments();
    STRICT_EXPECTED_CALL(mocks, pn_data_enter(IGNORED_PTR_ARG)).IgnoreAllArguments();
    STRICT_EXPECTED_CALL(mocks, pn_data_put_symbol(IGNORED_PTR_ARG, TEST_PN_GOOD_BYTES)).IgnoreAllArguments();
    STRICT_EXPECTED_CALL(mocks, pn_data_put_string(IGNORED_PTR_ARG, TEST_PN_GOOD_BYTES)).IgnoreAllArguments();
    STRICT_EXPECTED_CALL(mocks, pn_bytes(strlen(TEST_KEYS1[0]), TEST_KEYS1[0]));
    STRICT_EXPECTED_CALL(mocks, pn_bytes(strlen(TEST_VALUES1[0]), TEST_VALUES1[0]));
    STRICT_EXPECTED_CALL(mocks, pn_data_exit(IGNORED_PTR_ARG)).IgnoreAllArguments();

    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetMessageId(TEST_IOTHUB_MESSAGE_HANDLE));
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetCorrelationId(TEST_IOTHUB_MESSAGE_HANDLE));

    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    // act
    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, BASEIMPLEMENTATION::DList_IsListEmpty(&wts), 0);

    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/* Test_SRS_IOTHUBTRANSPORTTAMQP_07_001: [All IoTHubMessage_Properties shall be enumerated and entered in the pn_message_properties Map.] */
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_081: [sendEvent will take the item that had been the head of the waitingToSend and put it back at the head of the waitingToSend list.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_082: [sendEvent will then break out of the send loop.]*/
TEST_FUNCTION(DoWork_SetProperties_put_symbol_fail)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    IOTHUB_MESSAGE_LIST TEST_SIMPLE_MESSAGE_LIST = { TEST_IOTHUB_MESSAGE_HANDLE, NULL, NULL };
    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    BASEIMPLEMENTATION::DList_InsertTailList(&wts, &TEST_SIMPLE_MESSAGE_LIST.entry);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    proton_messenger_init_path(mocks, TEST_HAPPY_PATH);
    mocksFor_rollbackEvent(mocks, 1);
    mock_renewHappyPath(mocks);
    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(0); // From the call to renew if necessary.
    setupAmqpMessengerDispose(mocks, 2);
    STRICT_EXPECTED_CALL(mocks, pn_messenger_recv(TEST_PN_MESSENGER, 0));
    mocksFor_protonWork(mocks, TEST_HAPPY_PATH);
    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1);
    setupAmqpPrepareBatch(mocks, TEST_HAPPY_PATH);
    setupAmqpPrepareSimpleProtonMessage(mocks, TEST_HAPPY_PATH);

    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Properties(TEST_IOTHUB_MESSAGE_HANDLE)).SetReturn(TEST_MAP_1_PROPERTY);
    STRICT_EXPECTED_CALL(mocks, Map_GetInternals(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG)).IgnoreAllArguments();
    mocksFor_putSecondAfterFirst(mocks);
    EXPECTED_CALL(mocks, pn_message_properties(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, pn_data_put_map(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, pn_data_enter(IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, pn_data_put_symbol(IGNORED_PTR_ARG, TEST_PN_GOOD_BYTES)).IgnoreAllArguments().SetReturn(__LINE__);
    STRICT_EXPECTED_CALL(mocks, pn_bytes(strlen(TEST_KEYS1[0]), TEST_KEYS1[0]));

    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    // act
    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // assert
    ASSERT_ARE_EQUAL(int, BASEIMPLEMENTATION::DList_IsListEmpty(&wts), 0);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/* Test_SRS_IOTHUBTRANSPORTTAMQP_07_001: [All IoTHubMessage_Properties shall be enumerated and entered in the pn_message_properties Map.] */
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_081: [sendEvent will take the item that had been the head of the waitingToSend and put it back at the head of the waitingToSend list.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_082: [sendEvent will then break out of the send loop.]*/
TEST_FUNCTION(DoWork_SetProperties_IoTHubMessage_Properties_Fail)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    IOTHUB_MESSAGE_LIST TEST_SIMPLE_MESSAGE_LIST = { TEST_IOTHUB_MESSAGE_HANDLE, NULL, NULL };
    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    BASEIMPLEMENTATION::DList_InsertTailList(&wts, &TEST_SIMPLE_MESSAGE_LIST.entry);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    proton_messenger_init_path(mocks, TEST_HAPPY_PATH);
    mocksFor_rollbackEvent(mocks, 1);
    mock_renewHappyPath(mocks);
    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(0); // From the call to renew if necessary.
    setupAmqpMessengerDispose(mocks, 2);
    STRICT_EXPECTED_CALL(mocks, pn_messenger_recv(TEST_PN_MESSENGER, 0));
    mocksFor_protonWork(mocks, TEST_HAPPY_PATH);
    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1);
    setupAmqpPrepareBatch(mocks, TEST_HAPPY_PATH);
    setupAmqpPrepareSimpleProtonMessage(mocks, TEST_HAPPY_PATH);

    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Properties(TEST_IOTHUB_MESSAGE_HANDLE)).SetReturn((MAP_HANDLE)NULL);
    mocksFor_putSecondAfterFirst(mocks);

    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    // act
    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // assert
    ASSERT_ARE_EQUAL(int, BASEIMPLEMENTATION::DList_IsListEmpty(&wts), 0);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/* Test_SRS_IOTHUBTRANSPORTTAMQP_07_001: [All IoTHubMessage_Properties shall be enumerated and entered in the pn_message_properties Map.] */
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_081: [sendEvent will take the item that had been the head of the waitingToSend and put it back at the head of the waitingToSend list.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_082: [sendEvent will then break out of the send loop.]*/
TEST_FUNCTION(DoWork_SetProperties_Map_GetInternals_Fail)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    IOTHUB_MESSAGE_LIST TEST_SIMPLE_MESSAGE_LIST = { TEST_IOTHUB_MESSAGE_HANDLE, NULL, NULL };
    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    BASEIMPLEMENTATION::DList_InsertTailList(&wts, &TEST_SIMPLE_MESSAGE_LIST.entry);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    proton_messenger_init_path(mocks, TEST_HAPPY_PATH);
    mocksFor_rollbackEvent(mocks, 1);
    mock_renewHappyPath(mocks);
    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(0); // From the call to renew if necessary.
    setupAmqpMessengerDispose(mocks, 2);
    STRICT_EXPECTED_CALL(mocks, pn_messenger_recv(TEST_PN_MESSENGER, 0));
    mocksFor_protonWork(mocks, TEST_HAPPY_PATH);
    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1);
    setupAmqpPrepareBatch(mocks, TEST_HAPPY_PATH);
    setupAmqpPrepareSimpleProtonMessage(mocks, TEST_HAPPY_PATH);

    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Properties(TEST_IOTHUB_MESSAGE_HANDLE)).SetReturn(TEST_MAP_1_PROPERTY);
    STRICT_EXPECTED_CALL(mocks, Map_GetInternals(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG)).IgnoreAllArguments().SetReturn(MAP_ERROR);
    mocksFor_putSecondAfterFirst(mocks);

    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    // act
    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // assert
    ASSERT_ARE_EQUAL(int, BASEIMPLEMENTATION::DList_IsListEmpty(&wts), 0);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/* Test_SRS_IOTHUBTRANSPORTTAMQP_07_002: [On messages the properties shall be retrieved from the message using pn_message_properties and will be entered in the IoTHubMessage_Properties Map.] */
TEST_FUNCTION(DoWork_GetProperties_Succeed)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    proton_messenger_init_path(mocks, TEST_HAPPY_PATH);
    mocksFor_rollbackEvent(mocks, 1);
    mock_renewHappyPath(mocks);
    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(0); // From the call to renew if necessary.
    setupAmqpMessengerDispose(mocks, 2);
    mocksFor_protonWork(mocks, TEST_HAPPY_PATH);
    mocksFor_NoTelementry(mocks);
    mocksFor_processReceives(mocks, 4);
    mocksFor_processMessages(mocks, 8, false);
    EXPECTED_CALL(mocks, pn_message_properties(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, pn_data_next(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, pn_data_get_map(IGNORED_PTR_ARG)).SetReturn(2);
    EXPECTED_CALL(mocks, pn_data_enter(IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Properties(TEST_IOTHUB_MESSAGE_HANDLE)).SetReturn(TEST_MAP_1_PROPERTY);
    EXPECTED_CALL(mocks, pn_data_next(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, pn_data_type(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, pn_data_get_string(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, pn_data_next(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, pn_data_type(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, pn_data_get_string(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, Map_AddOrUpdate(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, pn_data_exit(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, pn_message_get_id(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, pn_message_get_correlation_id(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, IoTHubMessage_SetMessageId(TEST_IOTHUB_MESSAGE_HANDLE, IGNORED_PTR_ARG));

    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    transportFunctions->IoTHubTransport_Subscribe(transportHandle);

    // act
    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/* Test_SRS_IOTHUBTRANSPORTTAMQP_07_002: [On messages the properties shall be retrieved from the message using pn_message_properties and will be entered in the IoTHubMessage_Properties Map.] */
/* Test_SRS_IOTHUBTRANSPORTTAMQP_06_059: [processMessage will abandon the message if IotHubMessage_SetData returns any status other than IOTHUB_MESSAGE_OK.]*/
TEST_FUNCTION(DoWork_GetProperties_pn_message_properties_Fail)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    proton_messenger_init_path(mocks, TEST_HAPPY_PATH);
    mocksFor_rollbackEvent(mocks, 1);
    mock_renewHappyPath(mocks);
    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(0); // From the call to renew if necessary.
    setupAmqpMessengerDispose(mocks, 2);
    mocksFor_protonWork(mocks, TEST_HAPPY_PATH);
    mocksFor_NoTelementry(mocks);
    mocksFor_processReceives(mocks, 4);
    mocksFor_processMessages(mocks, 5, false);
    EXPECTED_CALL(mocks, pn_message_properties(IGNORED_PTR_ARG)).SetReturn((pn_data_t*)NULL);

    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    transportFunctions->IoTHubTransport_Subscribe(transportHandle);

    // act
    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/* Test_SRS_IOTHUBTRANSPORTTAMQP_07_002: [On messages the properties shall be retrieved from the message using pn_message_properties and will be entered in the IoTHubMessage_Properties Map.] */
/* Test_SRS_IOTHUBTRANSPORTTAMQP_06_059: [processMessage will abandon the message if IotHubMessage_SetData returns any status other than IOTHUB_MESSAGE_OK.]*/
TEST_FUNCTION(DoWork_GetProperties_pn_data_next_Fail)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    proton_messenger_init_path(mocks, TEST_HAPPY_PATH);
    mocksFor_rollbackEvent(mocks, 1);
    mock_renewHappyPath(mocks);
    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(0); // From the call to renew if necessary.
    setupAmqpMessengerDispose(mocks, 2);
    mocksFor_protonWork(mocks, TEST_HAPPY_PATH);
    mocksFor_NoTelementry(mocks);
    mocksFor_processReceives(mocks, 4);
    mocksFor_processMessages(mocks, 5, false);
    EXPECTED_CALL(mocks, pn_message_properties(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, pn_data_next(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, pn_data_get_map(IGNORED_PTR_ARG)).SetReturn(2);
    EXPECTED_CALL(mocks, pn_data_enter(IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Properties(TEST_IOTHUB_MESSAGE_HANDLE)).SetReturn(TEST_MAP_1_PROPERTY);
    EXPECTED_CALL(mocks, pn_data_next(IGNORED_PTR_ARG)).SetReturn(false);

    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    transportFunctions->IoTHubTransport_Subscribe(transportHandle);

    // act
    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/* Test_SRS_IOTHUBTRANSPORTTAMQP_07_002: [On messages the properties shall be retrieved from the message using pn_message_properties and will be entered in the IoTHubMessage_Properties Map.] */
/* Test_SRS_IOTHUBTRANSPORTTAMQP_06_059: [processMessage will abandon the message if IotHubMessage_SetData returns any status other than IOTHUB_MESSAGE_OK.]*/
TEST_FUNCTION(DoWork_GetProperties_Map_AddOrUpdate_Fail)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    proton_messenger_init_path(mocks, TEST_HAPPY_PATH);
    mocksFor_rollbackEvent(mocks, 1);
    mock_renewHappyPath(mocks);
    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(0); // From the call to renew if necessary.
    setupAmqpMessengerDispose(mocks, 2);
    mocksFor_protonWork(mocks, TEST_HAPPY_PATH);
    mocksFor_NoTelementry(mocks);
    mocksFor_processReceives(mocks, 4);
    mocksFor_processMessages(mocks, 5, false);
    EXPECTED_CALL(mocks, pn_message_properties(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, pn_data_next(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, pn_data_get_map(IGNORED_PTR_ARG)).SetReturn(2);
    EXPECTED_CALL(mocks, pn_data_enter(IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Properties(TEST_IOTHUB_MESSAGE_HANDLE)).SetReturn(TEST_MAP_1_PROPERTY);
    EXPECTED_CALL(mocks, pn_data_next(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, pn_data_type(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, pn_data_get_string(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, pn_data_next(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, pn_data_type(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, pn_data_get_string(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, Map_AddOrUpdate(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG)).SetReturn(MAP_ERROR);

    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    transportFunctions->IoTHubTransport_Subscribe(transportHandle);

    // act
    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_067: [reclaimEventResources shall go onto the next work item if the proton status is PN_STATUS_PENDING.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_073: [While the availableWorkItems is NOT empty loop.]   */
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_074: [If there is a waitingToSend entry remove it from the list.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_074: [If there is a waitingToSend entry remove it from the list.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_075: [Get its size and payload address by calling IoTHubMessage_GetData.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_083: [If the proton API is successful then invoke pn_messenger_put.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_087: [The head of the availableWorkItems will be removed and placed on the workInProgress list.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_086: [If the pn_messenger_put succeeds then sendEvent will save off a tracker obtained by invoking pn_messenger_outgoing_tracker and save it the head of the availableWorkItems.  The eventMessages in the AMQP_WORK_ITEM head will also contain the IOTHUB_MESSAGE_LIST.]*/
TEST_FUNCTION(DoWork_cleanOut_in_progress_SendEvent_second_invoke_dowork_status_pending_succeed)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    IOTHUB_MESSAGE_LIST TEST_SIMPLE_MESSAGE_LIST = { TEST_IOTHUB_MESSAGE_HANDLE, NULL, NULL };
    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    BASEIMPLEMENTATION::DList_InsertTailList(&wts, &TEST_SIMPLE_MESSAGE_LIST.entry);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    proton_messenger_init_path(mocks, TEST_HAPPY_PATH);
    mocksFor_rollbackEvent(mocks, 1);
    setupAmqpMessengerDispose(mocks, 2);
    mock_renewHappyPath(mocks);
    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(0); // From the first renew in dowork.
    STRICT_EXPECTED_CALL(mocks, pn_messenger_recv(TEST_PN_MESSENGER, 0));
    mocksFor_protonWork(mocks, TEST_HAPPY_PATH);
    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1);
    setupAmqpPrepareBatch(mocks, TEST_HAPPY_PATH);
    setupAmqpPrepareSimpleProtonMessage(mocks, TEST_HAPPY_PATH);
    STRICT_EXPECTED_CALL(mocks, pn_messenger_put(TEST_PN_MESSENGER, TEST_PN_MESSAGE));
    STRICT_EXPECTED_CALL(mocks, get_time(NULL));
    STRICT_EXPECTED_CALL(mocks, pn_messenger_outgoing_tracker(TEST_PN_MESSENGER));
    STRICT_EXPECTED_CALL(mocks, DList_RemoveHeadList(IGNORED_PTR_ARG)).IgnoreAllArguments(); // Remove it from available.
    STRICT_EXPECTED_CALL(mocks, DList_InsertTailList(IGNORED_PTR_ARG, IGNORED_PTR_ARG)).IgnoreAllArguments(); // Insert it on the in progress
    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1); // Look again in the available.
    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1); // Look again to see if any on waiting to send.

    //
    // During destroy
    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1); //Second pass through in progress
    STRICT_EXPECTED_CALL(mocks, DList_RemoveHeadList(IGNORED_PTR_ARG)).IgnoreAllArguments(); // Remove from work in progress
    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_SendComplete(TEST_IOTHUB_CLIENT_LL_HANDLE, IGNORED_PTR_ARG, IOTHUB_BATCHSTATE_FAILED)).IgnoreArgument(2); // Complete the in progress one.
    STRICT_EXPECTED_CALL(mocks, DList_InsertTailList(IGNORED_PTR_ARG, IGNORED_PTR_ARG)).IgnoreAllArguments(); // Put it on the available.

    //
    // Second invocation of DoWork
    //
    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(0); // From the second renew in dowork.
    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(0); // From the second sweep through the in progress.
    STRICT_EXPECTED_CALL(mocks, pn_messenger_recv(TEST_PN_MESSENGER, 0));
    mocksFor_protonWork(mocks, TEST_HAPPY_PATH);
    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1); // check of available
    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1); // check of waitingtosend
    STRICT_EXPECTED_CALL(mocks, pn_messenger_status(TEST_PN_MESSENGER, TEST_PN_TRACKER)).SetReturn(PN_STATUS_PENDING);
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Properties(TEST_IOTHUB_MESSAGE_HANDLE));
    STRICT_EXPECTED_CALL(mocks, Map_GetInternals(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG)).IgnoreAllArguments();

    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetMessageId(TEST_IOTHUB_MESSAGE_HANDLE));
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetCorrelationId(TEST_IOTHUB_MESSAGE_HANDLE));

    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);
    ASSERT_ARE_NOT_EQUAL(int, BASEIMPLEMENTATION::DList_IsListEmpty(&wts), 0);

    // act
    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_064: [Obtain the head of the workInProgress]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_065: [While head != listhead reclaimEventResources will do as follows.] */
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_066: [Check the current status via pn_messenger_status via the tracker stored in the AMQP_WORK_ITEM.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_068: [Otherwise reclaimEventResources will use the result of IOTHUB_BATCHSTATE_SUCCESS if the proton status was PN_STATUS_ACCEPTED.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_069: [Otherwise reclaimEventResources will use the status of IOTHUB_BATCHSTATE_FAILED if the proton status was NOT PN_STATUS_ACCEPTED.] */
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_070: [reclaimEventResources will invoke IotHubClient_SendBatchComplete with the IOTHUB_CLIENT_LL_HANDLE, supplied to reclaimEventResources, also pass the eventMessages stored in the AMQP_WORK_ITEM, and finally pass the above IOTHUB_BATCH status.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_071: [reclaimEventResources will take the current work item from the workInProgress list and insert it on the tail of the availableWorkItems list.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_072: [reclaimEventResources will continue to iterate over the workInProgress list until it returns to the list head.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_090: [reclaimEventResources will release the tracker by invoking pn_messenger_settle.]*/
TEST_FUNCTION(DoWork_reclaim_work_in_progress_non_success_succeed)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    IOTHUB_MESSAGE_LIST TEST_SIMPLE_MESSAGE_LIST = { TEST_IOTHUB_MESSAGE_HANDLE, NULL, NULL };
    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    BASEIMPLEMENTATION::DList_InsertTailList(&wts, &TEST_SIMPLE_MESSAGE_LIST.entry);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    proton_messenger_init_path(mocks, TEST_HAPPY_PATH);
    mocksFor_rollbackEvent(mocks, 1);
    mock_renewHappyPath(mocks);
    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(0); // From the call to renew if necessary.
    setupAmqpMessengerDispose(mocks, 2);
    STRICT_EXPECTED_CALL(mocks, pn_messenger_recv(TEST_PN_MESSENGER, 0));
    mocksFor_protonWork(mocks, TEST_HAPPY_PATH);
    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1);
    setupAmqpPrepareBatch(mocks, TEST_HAPPY_PATH);
    setupAmqpPrepareSimpleProtonMessage(mocks, TEST_HAPPY_PATH);
    STRICT_EXPECTED_CALL(mocks, pn_messenger_put(TEST_PN_MESSENGER, TEST_PN_MESSAGE));
    STRICT_EXPECTED_CALL(mocks, get_time(NULL));
    STRICT_EXPECTED_CALL(mocks, pn_messenger_outgoing_tracker(TEST_PN_MESSENGER));
    STRICT_EXPECTED_CALL(mocks, DList_RemoveHeadList(IGNORED_PTR_ARG)).IgnoreAllArguments(); // Remove it from available.
    STRICT_EXPECTED_CALL(mocks, DList_InsertTailList(IGNORED_PTR_ARG, IGNORED_PTR_ARG)).IgnoreAllArguments(); // Insert it on the in progress
    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1); // Look again in the available.
    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1); // Look again to see if any on waiting to send.
    //
    // Second invocation of DoWork
    //
    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(0); // From the call to renew if necessary.
    STRICT_EXPECTED_CALL(mocks, pn_messenger_recv(TEST_PN_MESSENGER, 0));
    mocksFor_protonWork(mocks, TEST_HAPPY_PATH);
    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1); // check of available
    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1); // check of waitingtosend
    STRICT_EXPECTED_CALL(mocks, pn_messenger_status(TEST_PN_MESSENGER, TEST_PN_TRACKER)).SetReturn(PN_STATUS_REJECTED);
    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_SendComplete(TEST_IOTHUB_CLIENT_LL_HANDLE, IGNORED_PTR_ARG, IOTHUB_BATCHSTATE_FAILED)).IgnoreArgument(2); // Complete the in progress one.
    STRICT_EXPECTED_CALL(mocks, pn_messenger_settle(TEST_PN_MESSENGER, TEST_PN_TRACKER, 0));
    STRICT_EXPECTED_CALL(mocks, DList_RemoveEntryList(IGNORED_PTR_ARG)).IgnoreAllArguments(); // Remove from work in progress
    STRICT_EXPECTED_CALL(mocks, DList_InsertTailList(IGNORED_PTR_ARG, IGNORED_PTR_ARG)).IgnoreAllArguments(); // Put it on the available.

    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Properties(TEST_IOTHUB_MESSAGE_HANDLE));
    STRICT_EXPECTED_CALL(mocks, Map_GetInternals(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG)).IgnoreAllArguments();

    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetMessageId(TEST_IOTHUB_MESSAGE_HANDLE));
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetCorrelationId(TEST_IOTHUB_MESSAGE_HANDLE));

    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);
    ASSERT_ARE_NOT_EQUAL(int, BASEIMPLEMENTATION::DList_IsListEmpty(&wts), 0);


    // act
    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_175: [The list of work in progress is tested to see if it is empty.]*/
TEST_FUNCTION(DoWork_DoWork_detects_error_causes_reconnect_succeeds)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    IOTHUB_MESSAGE_LIST TEST_SIMPLE_MESSAGE_LIST = { TEST_IOTHUB_MESSAGE_HANDLE, NULL, NULL };
    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    proton_messenger_init_path(mocks, TEST_HAPPY_PATH);
    mocksFor_rollbackEvent(mocks, 1);
    setupAmqpMessengerDispose(mocks, 2);
    mock_renewHappyPath(mocks);
    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(0); // From the first renew in dowork.
    STRICT_EXPECTED_CALL(mocks, pn_messenger_recv(TEST_PN_MESSENGER, 0));
    mocksFor_protonWork(mocks, TEST_HAPPY_PATH);
    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1); // From the empty call to prepare batch.
    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1);

    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(0); // From the second renew in dowork.
    STRICT_EXPECTED_CALL(mocks, pn_messenger_recv(TEST_PN_MESSENGER, 0));
    mocksFor_protonWork(mocks, 1);
    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1); // From the empty call to prepare batch.
    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1);
    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);
    proton_messenger_init_path(mocks, TEST_HAPPY_PATH);
    mocksFor_rollbackEvent(mocks, 1);
    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(0); // From the third renew in dowork.
    STRICT_EXPECTED_CALL(mocks, pn_messenger_recv(TEST_PN_MESSENGER, 0));
    mocksFor_protonWork(mocks, TEST_HAPPY_PATH);
    setupAmqpMessengerDispose(mocks, 2);
    mock_renewHappyPath(mocks);
    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1); // From the empty call to prepare batch.
    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1);

    //ASSERT_ARE_NOT_EQUAL(int, BASEIMPLEMENTATION::DList_IsListEmpty(&wts), 0);

    //// act
    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_175: [The list of work in progress is tested to see if it is empty.]*/
TEST_FUNCTION(DoWork_DoWork_work_error_causes_reconnect_succeeds)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    IOTHUB_MESSAGE_LIST TEST_SIMPLE_MESSAGE_LIST = { TEST_IOTHUB_MESSAGE_HANDLE, NULL, NULL };
    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    proton_messenger_init_path(mocks, TEST_HAPPY_PATH);
    mocksFor_rollbackEvent(mocks, 1);
    setupAmqpMessengerDispose(mocks, 2);
    mock_renewHappyPath(mocks);
    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(0); // From the first renew in dowork.
    STRICT_EXPECTED_CALL(mocks, pn_messenger_recv(TEST_PN_MESSENGER, 0));
    mocksFor_protonWork(mocks, TEST_HAPPY_PATH);
    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1); // From the empty call to prepare batch.
    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1);

    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(0); // From the second renew in dowork.
    STRICT_EXPECTED_CALL(mocks, pn_messenger_recv(TEST_PN_MESSENGER, 0));
    mocksFor_protonWork(mocks, 2);
    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1); // From the empty call to prepare batch.
    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1);
    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);
    proton_messenger_init_path(mocks, TEST_HAPPY_PATH);
    mocksFor_rollbackEvent(mocks, 1);
    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(0); // From the third renew in dowork.
    STRICT_EXPECTED_CALL(mocks, pn_messenger_recv(TEST_PN_MESSENGER, 0));
    mocksFor_protonWork(mocks, TEST_HAPPY_PATH);
    setupAmqpMessengerDispose(mocks, 2);
    mock_renewHappyPath(mocks);
    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1); // From the empty call to prepare batch.
    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1);

    //ASSERT_ARE_NOT_EQUAL(int, BASEIMPLEMENTATION::DList_IsListEmpty(&wts), 0);

    //// act
    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_176: [If it is not empty then the list is traversed.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_182: [Items that have been settled by the service will have their IOTHUB_MESSAGE_LIST completed.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_179: [The status of the work item is acquired.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_180: [The item is settled.]*/
/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_181: [The work item is removed from the in progress list and placed on the available list.]*/
TEST_FUNCTION(DoWork_DoWork_work_error_causes_reconnect_with_work_in_progress_was_done_succeeds)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    IOTHUB_MESSAGE_LIST TEST_SIMPLE_MESSAGE_LIST = { TEST_IOTHUB_MESSAGE_HANDLE, NULL, NULL };
    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    proton_messenger_init_path(mocks, TEST_HAPPY_PATH);
    mocksFor_rollbackEvent(mocks, 1);
    setupAmqpMessengerDispose(mocks, 2);
    mock_renewHappyPath(mocks);
    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(0); // From the first renew in dowork.
    STRICT_EXPECTED_CALL(mocks, pn_messenger_recv(TEST_PN_MESSENGER, 0));
    mocksFor_protonWork(mocks, TEST_HAPPY_PATH);
    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1); // From beginning of send event available work items
    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1); // From the prepare batch of waiting to send (should be empty)

    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    BASEIMPLEMENTATION::DList_InsertTailList(&wts, &TEST_SIMPLE_MESSAGE_LIST.entry);
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetMessageId(TEST_IOTHUB_MESSAGE_HANDLE));
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetCorrelationId(TEST_IOTHUB_MESSAGE_HANDLE));
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Properties(TEST_IOTHUB_MESSAGE_HANDLE));
    STRICT_EXPECTED_CALL(mocks, Map_GetInternals(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG)).IgnoreAllArguments();
    setupAmqpPrepareBatch(mocks, TEST_HAPPY_PATH);
    setupAmqpPrepareSimpleProtonMessage(mocks, TEST_HAPPY_PATH);
    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(0); // From the second renew in dowork.
    STRICT_EXPECTED_CALL(mocks, pn_messenger_recv(TEST_PN_MESSENGER, 0));
    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1); // From beginning of send event available work items first loop
    STRICT_EXPECTED_CALL(mocks, pn_messenger_put(TEST_PN_MESSENGER, TEST_PN_MESSAGE));
    STRICT_EXPECTED_CALL(mocks, get_time(NULL));
    STRICT_EXPECTED_CALL(mocks, pn_messenger_outgoing_tracker(TEST_PN_MESSENGER));
    STRICT_EXPECTED_CALL(mocks, DList_RemoveHeadList(IGNORED_PTR_ARG)).IgnoreAllArguments(); // Remove it from available.
    STRICT_EXPECTED_CALL(mocks, DList_InsertTailList(IGNORED_PTR_ARG, IGNORED_PTR_ARG)).IgnoreAllArguments(); // Insert it on the in progress
    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1); // Look again in the available. second time through looop
    // We do the below explicitly.  The setupAmqpPrepareBatch did it before.
    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1); // Look again to see if any on waiting to send. (in prep batch)
    // The below will cause the failure of the proton work.
    mocksFor_protonWork(mocks, 2);
    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);
    //
    // The above failure of proton work will cause us to go through the messenger initialization code
    // Since we have an item on the pending list, it will be rolled back.
    mocksFor_rollbackEvent(mocks, TEST_HAPPY_PATH);
    setupAmqpMessengerDispose(mocks, 2);
    mock_renewHappyPath(mocks);
    proton_messenger_init_path(mocks, TEST_HAPPY_PATH);

    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(0); // From the third renew in dowork.
    STRICT_EXPECTED_CALL(mocks, pn_messenger_recv(TEST_PN_MESSENGER, 0)); // The process recieves of the third call to dowork
    mocksFor_protonWork(mocks, TEST_HAPPY_PATH);  // The third finishing up of do work
    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1); // From beginning of send event available work items
    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1); // From the prepare batch of waiting to send (should be empty)

    // act
    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_06_178: [Items that are not settled by the service will have their IOTHUB_MESSAGE_LIST removed from the work item and placed back on the waiting to send.]*/
TEST_FUNCTION(DoWork_DoWork_work_error_causes_reconnect_with_work_in_progress_still_pending_fails)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    IOTHUB_MESSAGE_LIST TEST_SIMPLE_MESSAGE_LIST = { TEST_IOTHUB_MESSAGE_HANDLE, NULL, NULL };
    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    proton_messenger_init_path(mocks, TEST_HAPPY_PATH);
    mocksFor_rollbackEvent(mocks, 1);
    setupAmqpMessengerDispose(mocks, 2);
    mock_renewHappyPath(mocks);
    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(0); // From the first renew in dowork.
    STRICT_EXPECTED_CALL(mocks, pn_messenger_recv(TEST_PN_MESSENGER, 0));
    mocksFor_protonWork(mocks, TEST_HAPPY_PATH);
    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1); // From beginning of send event available work items
    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1); // From the prepare batch of waiting to send (should be empty)

    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    BASEIMPLEMENTATION::DList_InsertTailList(&wts, &TEST_SIMPLE_MESSAGE_LIST.entry);
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Properties(TEST_IOTHUB_MESSAGE_HANDLE));
    STRICT_EXPECTED_CALL(mocks, Map_GetInternals(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG)).IgnoreAllArguments();
    setupAmqpPrepareBatch(mocks, TEST_HAPPY_PATH);
    setupAmqpPrepareSimpleProtonMessage(mocks, TEST_HAPPY_PATH);
    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(0); // From the second renew in dowork.
    STRICT_EXPECTED_CALL(mocks, pn_messenger_recv(TEST_PN_MESSENGER, 0));
    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1); // From beginning of send event available work items first loop
    STRICT_EXPECTED_CALL(mocks, pn_messenger_put(TEST_PN_MESSENGER, TEST_PN_MESSAGE));
    STRICT_EXPECTED_CALL(mocks, get_time(NULL));
    STRICT_EXPECTED_CALL(mocks, pn_messenger_outgoing_tracker(TEST_PN_MESSENGER));
    STRICT_EXPECTED_CALL(mocks, DList_RemoveHeadList(IGNORED_PTR_ARG)).IgnoreAllArguments(); // Remove it from available.
    STRICT_EXPECTED_CALL(mocks, DList_InsertTailList(IGNORED_PTR_ARG, IGNORED_PTR_ARG)).IgnoreAllArguments(); // Insert it on the in progress
    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1); // Look again in the available. second time through looop
                                                                                       // We do the below explicitly.  The setupAmqpPrepareBatch did it before.
    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1); // Look again to see if any on waiting to send. (in prep batch)
                                                                                       // The below will cause the failure of the proton work.
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetMessageId(TEST_IOTHUB_MESSAGE_HANDLE));
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetCorrelationId(TEST_IOTHUB_MESSAGE_HANDLE));

    mocksFor_protonWork(mocks, 2);
    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);
    //
    // The above failure of proton work will cause us to go into the messenger initialization code
    // Since we have an item on the pending list, it will be rolled back.
    mocksFor_rollbackEvent(mocks, 2);
    setupAmqpMessengerDispose(mocks, 2);

    // The below setup of mocks for init will cause the rollback to happen, but we really don't want to go through the pushing this IO all
    // the way through again.  So cut the initialization short.  This will make sure nothing else happens in DoWork.
    // We have another test that makes sure we can re-initialize after failure.
    proton_messenger_init_path(mocks, 3);

    //STRICT_EXPECTED_CALL(mocks, pn_messenger_recv(TEST_PN_MESSENGER, 0));
    //mocksFor_protonWork(mocks, TEST_HAPPY_PATH);
    //STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreArgument(1);
    //setupAmqpPrepareBatch(mocks, TEST_HAPPY_PATH);
    //setupAmqpPrepareSimpleProtonMessage(mocks, TEST_HAPPY_PATH);

    // act
    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // Make sure that there is only one item on waiting to send
    ASSERT_ARE_EQUAL(void_ptr, wts.Flink, wts.Blink); // Make sure only one.
    ASSERT_ARE_NOT_EQUAL(void_ptr, wts.Flink, &wts);  // Make sure it's not the list head!

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/*** IoTHubTransportHttp_GetSendStatus ***/

/* Tests_SRS_IOTHUBTRANSPORTTAMQP_09_001: [clientTransportAMQP_GetSendStatus  IoTHubTransportHttp_GetSendStatus shall return IOTHUB_CLIENT_INVALID_ARG if called with NULL parameter] */
TEST_FUNCTION(IoTHubTransportAmqp_GetSendStatus_InvalidHandleArgument_fail)
{
    // arrange
    CIoTHubTransportAMQPMocks iotHubMocks;

    DLIST_ENTRY waitingToSend;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &waitingToSend };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&waitingToSend);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    iotHubMocks.ResetAllCalls();

    IOTHUB_CLIENT_STATUS status;

    // act
    IOTHUB_CLIENT_RESULT result = transportFunctions->IoTHubTransport_GetSendStatus(NULL, &status);

    // assert
    iotHubMocks.AssertActualAndExpectedCalls();
    ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, result, IOTHUB_CLIENT_INVALID_ARG);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/* Tests_SRS_IOTHUBTRANSPORTTAMQP_09_001: [clientTransportAMQP_GetSendStatus  IoTHubTransportHttp_GetSendStatus shall return IOTHUB_CLIENT_INVALID_ARG if called with NULL parameter] */
TEST_FUNCTION(IoTHubTransportAmqp_GetSendStatus_InvalidStatusArgument_fail)
{
    // arrange
    CIoTHubTransportAMQPMocks iotHubMocks;

    DLIST_ENTRY waitingToSend;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &waitingToSend };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&waitingToSend);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    iotHubMocks.ResetAllCalls();

    // act
    IOTHUB_CLIENT_RESULT result = transportFunctions->IoTHubTransport_GetSendStatus(transportHandle, NULL);

    // assert
    iotHubMocks.AssertActualAndExpectedCalls();
    ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, result, IOTHUB_CLIENT_INVALID_ARG);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/* Tests_SRS_IOTHUBTRANSPORTTAMQP_09_002: [clientTransportAMQP_GetSendStatus  shall return IOTHUB_CLIENT_OK and status IOTHUB_CLIENT_SEND_STATUS_IDLE if there are currently no event items to be sent or being sent] */
TEST_FUNCTION(IoTHubTransportAmqp_GetSendStatus_empty_waitingToSend_and_empty_workInProgress_success)
{
    // arrange
    CIoTHubTransportAMQPMocks iotHubMocks;

    DLIST_ENTRY waitingToSend;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &waitingToSend };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&waitingToSend);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    iotHubMocks.ResetAllCalls();
    
    STRICT_EXPECTED_CALL(iotHubMocks, DList_IsListEmpty(&waitingToSend));
    STRICT_EXPECTED_CALL(iotHubMocks, DList_IsListEmpty(IGNORED_PTR_ARG)).IgnoreAllArguments();

    IOTHUB_CLIENT_STATUS status;

    // act
    IOTHUB_CLIENT_RESULT result = transportFunctions->IoTHubTransport_GetSendStatus(transportHandle, &status);

    // assert
    iotHubMocks.AssertActualAndExpectedCalls();
    ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, result, IOTHUB_CLIENT_OK);
    ASSERT_ARE_EQUAL(IOTHUB_CLIENT_STATUS, status, IOTHUB_CLIENT_SEND_STATUS_IDLE);
    
    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/* Tests_SRS_IOTHUBTRANSPORTTAMQP_09_003: [clientTransportAMQP_GetSendStatus  shall return IOTHUB_CLIENT_OK and status IOTHUB_CLIENT_SEND_STATUS_BUSY if there are currently event items to be sent or being sent] */
TEST_FUNCTION(IoTHubTransportAmqp_GetSendStatus_waitingToSend_not_empty_success)
{
    // arrange
    CIoTHubTransportAMQPMocks iotHubMocks;

    DLIST_ENTRY waitingToSend;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &waitingToSend };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&waitingToSend);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    IOTHUB_MESSAGE_LIST TEST_SIMPLE_MESSAGE_LIST = { TEST_IOTHUB_MESSAGE_HANDLE, NULL, NULL };
    BASEIMPLEMENTATION::DList_InsertTailList(&waitingToSend, &TEST_SIMPLE_MESSAGE_LIST.entry);

    iotHubMocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(iotHubMocks, DList_IsListEmpty(&waitingToSend));

    IOTHUB_CLIENT_STATUS status;

    // act
    IOTHUB_CLIENT_RESULT result = transportFunctions->IoTHubTransport_GetSendStatus(transportHandle, &status);

    // assert
    iotHubMocks.AssertActualAndExpectedCalls();
    ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, result, IOTHUB_CLIENT_OK);
    ASSERT_ARE_EQUAL(IOTHUB_CLIENT_STATUS, status, IOTHUB_CLIENT_SEND_STATUS_BUSY);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/* Tests_SRS_IOTHUBTRANSPORTTAMQP_09_003: [clientTransportAMQP_GetSendStatus  shall return IOTHUB_CLIENT_OK and status IOTHUB_CLIENT_SEND_STATUS_BUSY if there are currently event items to be sent or being sent] */
TEST_FUNCTION(IoTHubTransportAmqp_GetSendStatus_workInProgress_not_empty_success)
{
    // arrange
    CIoTHubTransportAMQPMocks iotHubMocks;

    DLIST_ENTRY waitingToSend;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &waitingToSend };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&waitingToSend);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    PAMQP_TRANSPORT_STATE amqpTransportHandle = (PAMQP_TRANSPORT_STATE)transportHandle;

    IOTHUB_MESSAGE_LIST TEST_SIMPLE_MESSAGE_LIST = { TEST_IOTHUB_MESSAGE_HANDLE, NULL, NULL };
    BASEIMPLEMENTATION::DList_InsertTailList(&(amqpTransportHandle->workInProgress), &TEST_SIMPLE_MESSAGE_LIST.entry);

    iotHubMocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(iotHubMocks, DList_IsListEmpty(&waitingToSend));
    STRICT_EXPECTED_CALL(iotHubMocks, DList_IsListEmpty(&(amqpTransportHandle->workInProgress)));

    IOTHUB_CLIENT_STATUS status;

    // act
    IOTHUB_CLIENT_RESULT result = transportFunctions->IoTHubTransport_GetSendStatus(transportHandle, &status);

    // assert
    iotHubMocks.AssertActualAndExpectedCalls();
    ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, result, IOTHUB_CLIENT_OK);
    ASSERT_ARE_EQUAL(IOTHUB_CLIENT_STATUS, status, IOTHUB_CLIENT_SEND_STATUS_BUSY);

    // Cleanup
    BASEIMPLEMENTATION::DList_RemoveHeadList(&(amqpTransportHandle->workInProgress));
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_02_001: [If handle parameter is NULL then clientTransportAMQP_SetOption shall return IOTHUB_CLIENT_INVALID_ARG.]*/
TEST_FUNCTION(IoTHubTransportAmqp_SetOption_with_NULL_handle_fails)
{
    ///arrange
    TRANSPORT_PROVIDER* transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();

    ///act
    auto result = transportFunctions->IoTHubTransport_SetOption(NULL, "a", "b");

    ///assert
    ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_INVALID_ARG, result);

    ///cleanup
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_02_002: [If parameter optionName is NULL then clientTransportAMQP_SetOption shall return IOTHUB_CLIENT_INVALID_ARG.] */
TEST_FUNCTION(IoTHubTransportAmqp_SetOption_with_NULL_optionName_fails)
{
    ///arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY waitingToSend;
    TRANSPORT_PROVIDER* transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &waitingToSend };
    BASEIMPLEMENTATION::DList_InitializeListHead(&waitingToSend);
    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);
    mocks.ResetAllCalls();

    ///act
    auto result = transportFunctions->IoTHubTransport_SetOption(transportHandle, NULL, "b");

    ///assert
    ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_INVALID_ARG, result);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_02_003: [If parameter value is NULL then clientTransportAMQP_SetOption shall return IOTHUB_CLIENT_INVALID_ARG.] */
TEST_FUNCTION(IoTHubTransportAmqp_SetOption_with_NULL_value_fails)
{
    ///arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY waitingToSend;
    TRANSPORT_PROVIDER* transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &waitingToSend };
    BASEIMPLEMENTATION::DList_InitializeListHead(&waitingToSend);
    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);
    mocks.ResetAllCalls();

    ///act
    auto result = transportFunctions->IoTHubTransport_SetOption(transportHandle, "a", NULL);

    ///assert
    ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_INVALID_ARG, result);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/*Tests_SRS_IOTHUBTRANSPORTTAMQP_02_004: [If optionName is not an option supported then clientTransportAMQP_SetOption shall return IOTHUB_CLIENT_INVALID_ARG.] */
TEST_FUNCTION(IoTHubTransportAmqp_SetOption_with_unhandled_value_fails)
{
    ///arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY waitingToSend;
    TRANSPORT_PROVIDER* transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &waitingToSend };
    BASEIMPLEMENTATION::DList_InitializeListHead(&waitingToSend);
    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);
    mocks.ResetAllCalls();

    ///act
    auto result = transportFunctions->IoTHubTransport_SetOption(transportHandle, "a", "b"); /*assumes "a" is not an option handled. At the moment, there are no handled options*/

    ///assert
    ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_INVALID_ARG, result);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/* Tests_SRS_IOTHUBTRANSPORTTAMQP_01_003: ["SetTrustedCAs"] */
/* Tests_SRS_IOTHUBTRANSPORTTAMQP_02_003: [If parameter value is NULL then clientTransportAMQP_SetOption shall return IOTHUB_CLIENT_INVALID_ARG.] */
TEST_FUNCTION(IoTHubTransportAmqp_SetOption_with_TrustedCerts_NULL_fails)
{
    ///arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY waitingToSend;
    TRANSPORT_PROVIDER* transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &waitingToSend };
    BASEIMPLEMENTATION::DList_InitializeListHead(&waitingToSend);
    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);
    mocks.ResetAllCalls();

    ///act
    IOTHUB_CLIENT_RESULT result = transportFunctions->IoTHubTransport_SetOption(transportHandle, "TrustedCerts", NULL);

    ///assert
    ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_INVALID_ARG, result);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/* Tests_SRS_IOTHUBTRANSPORTTAMQP_01_003: ["TrustedCerts"] */
TEST_FUNCTION(IoTHubTransportAmqp_SetOption_with_TrustedCerts_empty_string_succeeds)
{
    ///arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY waitingToSend;
    TRANSPORT_PROVIDER* transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &waitingToSend };
    BASEIMPLEMENTATION::DList_InitializeListHead(&waitingToSend);
    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);
    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, mallocAndStrcpy_s(IGNORED_PTR_ARG, ""))
        .IgnoreArgument(1);

    ///act
    IOTHUB_CLIENT_RESULT result = transportFunctions->IoTHubTransport_SetOption(transportHandle, "TrustedCerts", "");

    ///assert
    ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK, result);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/* Tests_SRS_IOTHUBTRANSPORTTAMQP_02_005: [clientTransportAMQP_SetOption shall set the option "optionName" to *value.] */
/* Tests_SRS_IOTHUBTRANSPORTTAMQP_01_003: ["TrustedCerts"] */
TEST_FUNCTION(IoTHubTransportAmqp_SetOption_with_TrustedCerts_one_char_succeeds)
{
    ///arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY waitingToSend;
    TRANSPORT_PROVIDER* transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &waitingToSend };
    BASEIMPLEMENTATION::DList_InitializeListHead(&waitingToSend);
    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);
    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, mallocAndStrcpy_s(IGNORED_PTR_ARG, "a"))
        .IgnoreArgument(1);

    ///act
    IOTHUB_CLIENT_RESULT result = transportFunctions->IoTHubTransport_SetOption(transportHandle, "TrustedCerts", "a");

    ///assert
    ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK, result);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/* Tests_SRS_IOTHUBTRANSPORTTAMQP_01_004: [Sets a string that should be used as trusted certificates by the transport, freeing any previous TrustedCerts option value.] */
TEST_FUNCTION(IoTHubTransportAmqp_SetOption_TrustedCerts_a_second_time_frees_previously_allocated_option)
{
    ///arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY waitingToSend;
    TRANSPORT_PROVIDER* transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &waitingToSend };
    BASEIMPLEMENTATION::DList_InitializeListHead(&waitingToSend);
    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);
    (void)transportFunctions->IoTHubTransport_SetOption(transportHandle, "TrustedCerts", "a");
    mocks.ResetAllCalls();

    EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, mallocAndStrcpy_s(IGNORED_PTR_ARG, "b"))
        .IgnoreArgument(1);

    ///act
    IOTHUB_CLIENT_RESULT result = transportFunctions->IoTHubTransport_SetOption(transportHandle, "TrustedCerts", "b");

    ///assert
    ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK, result);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/* Tests_SRS_IOTHUBTRANSPORTTAMQP_01_006: [If any other error occurs while setting the option, clientTransportAMQP_SetOption shall return IOTHUB_CLIENT_ERROR.] */
TEST_FUNCTION(IoTHubTransportAmqp_When_allocating_the_cert_string_fails_SetOption_fails)
{
    ///arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY waitingToSend;
    TRANSPORT_PROVIDER* transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &waitingToSend };
    BASEIMPLEMENTATION::DList_InitializeListHead(&waitingToSend);
    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);
    (void)transportFunctions->IoTHubTransport_SetOption(transportHandle, "TrustedCerts", "a");
    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, mallocAndStrcpy_s(IGNORED_PTR_ARG, "b"))
        .IgnoreArgument(1).SetReturn(1);

    ///act
    IOTHUB_CLIENT_RESULT result = transportFunctions->IoTHubTransport_SetOption(transportHandle, "TrustedCerts", "b");

    ///assert
    ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_ERROR, result);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/* Tests_SRS_IOTHUBTRANSPORTTAMQP_01_001: [If the option "TrustedCerts" has been set (length greater than 0), the trusted certificates string shall be passed to Proton by a call to pn_messenger_set_trusted_certificates.] */
TEST_FUNCTION(IoTHubTransportAmqp_SetOption_TrustedCerts_gets_passed_to_Proton)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    proton_messenger_init_path(mocks, TEST_HAPPY_PATH);
    mocksFor_rollbackEvent(mocks, 1);
    mock_renewHappyPath(mocks);
    setupAmqpMessengerDispose(mocks, 2);
    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(0); // From the call to renew if necessary.
    mocksFor_NoTelementry(mocks);
    STRICT_EXPECTED_CALL(mocks, pn_messenger_recv(TEST_PN_MESSENGER, 0));
    mocksFor_protonWork(mocks, TEST_HAPPY_PATH);

    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    STRICT_EXPECTED_CALL(mocks, mallocAndStrcpy_s(IGNORED_PTR_ARG, "abcd"))
        .IgnoreArgument(1);
    EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG));

    (void)transportFunctions->IoTHubTransport_SetOption(transportHandle, "TrustedCerts", "abcd");

    STRICT_EXPECTED_CALL(mocks, pn_messenger_set_trusted_certificates(TEST_PN_MESSENGER, "abcd"));

    // act
    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/* Tests_SRS_IOTHUBTRANSPORTTAMQP_01_001: [If the option "TrustedCerts" has been set (length greater than 0), the trusted certificates string shall be passed to Proton by a call to pn_messenger_set_trusted_certificates.] */
TEST_FUNCTION(IoTHubTransportAmqp_SetOption_Empty_TrustedCerts_does_not_get_passed_to_Proton)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    proton_messenger_init_path(mocks, TEST_HAPPY_PATH);
    mocksFor_rollbackEvent(mocks, 1);
    mock_renewHappyPath(mocks);
    setupAmqpMessengerDispose(mocks, 2);
    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(0); // From the call to renew if necessary.
    mocksFor_NoTelementry(mocks);
    STRICT_EXPECTED_CALL(mocks, pn_messenger_recv(TEST_PN_MESSENGER, 0));
    mocksFor_protonWork(mocks, TEST_HAPPY_PATH);

    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    STRICT_EXPECTED_CALL(mocks, mallocAndStrcpy_s(IGNORED_PTR_ARG, ""))
        .IgnoreArgument(1);
    EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG));

    (void)transportFunctions->IoTHubTransport_SetOption(transportHandle, "TrustedCerts", "");

    // act
    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/* Tests_SRS_IOTHUBTRANSPORTTAMQP_01_002: [If pn_messenger_set_trusted_certificates fails, then messenger initialization shall fail.] */
TEST_FUNCTION(when_pn_messenger_set_trusted_certificates_fails_initializing_the_messenger_fails)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    STRICT_EXPECTED_CALL(mocks, pn_messenger(NULL));
    mocksFor_rollbackEvent(mocks, 1);
    setupAmqpMessengerDispose(mocks, 2);

    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    STRICT_EXPECTED_CALL(mocks, mallocAndStrcpy_s(IGNORED_PTR_ARG, "ab"))
        .IgnoreArgument(1);
        
    EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG));

    (void)transportFunctions->IoTHubTransport_SetOption(transportHandle, "TrustedCerts", "ab");

    STRICT_EXPECTED_CALL(mocks, pn_messenger_set_trusted_certificates(TEST_PN_MESSENGER, "ab"))
        .SetReturn(1);

    // act
    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/* Tests_SRS_IOTHUBTRANSPORTTAMQP_07_003: [If the protocol id is set it will be set as in the MessageId.] */
TEST_FUNCTION(DoWork_messages_SetIotMessageId_Succeeds)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    proton_messenger_init_path(mocks, TEST_HAPPY_PATH);
    mocksFor_rollbackEvent(mocks, 1);
    mock_renewHappyPath(mocks);
    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(0); // From the call to renew if necessary.
    setupAmqpMessengerDispose(mocks, 2);
    mocksFor_protonWork(mocks, TEST_HAPPY_PATH);
    mocksFor_NoTelementry(mocks);
    mocksFor_processReceives(mocks, 4);
    mocksFor_processMessages(mocks, 8, false);
    EXPECTED_CALL(mocks, pn_message_properties(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, pn_data_next(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, pn_data_get_map(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, pn_message_get_id(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, pn_message_get_correlation_id(IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_SetMessageId(TEST_IOTHUB_MESSAGE_HANDLE, TEST_MESSAGE_ID));

    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    transportFunctions->IoTHubTransport_Subscribe(transportHandle);

    // act
    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

/* Tests_SRS_IOTHUBTRANSPORTTAMQP_07_003: [If the protocol id is set it will be set as in the MessageId.] */
TEST_FUNCTION(DoWork_messages_SetIotMessageId_Fail)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    proton_messenger_init_path(mocks, TEST_HAPPY_PATH);
    mocksFor_rollbackEvent(mocks, 1);
    mock_renewHappyPath(mocks);
    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(0); // From the call to renew if necessary.
    setupAmqpMessengerDispose(mocks, 2);
    mocksFor_protonWork(mocks, TEST_HAPPY_PATH);
    mocksFor_NoTelementry(mocks);
    mocksFor_processReceives(mocks, 4);
    mocksFor_processMessages(mocks, 12, false);
    EXPECTED_CALL(mocks, pn_message_properties(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, pn_data_next(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, pn_data_get_map(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, pn_message_get_id(IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_SetMessageId(TEST_IOTHUB_MESSAGE_HANDLE, TEST_MESSAGE_ID)).SetReturn(IOTHUB_MESSAGE_ERROR);

    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    transportFunctions->IoTHubTransport_Subscribe(transportHandle);

    // act
    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

TEST_FUNCTION(DoWork_messages_SetCorrelationId_Succeeds)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    proton_messenger_init_path(mocks, TEST_HAPPY_PATH);
    mocksFor_rollbackEvent(mocks, 1);
    mock_renewHappyPath(mocks);
    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(0); // From the call to renew if necessary.
    setupAmqpMessengerDispose(mocks, 2);
    mocksFor_protonWork(mocks, TEST_HAPPY_PATH);
    mocksFor_NoTelementry(mocks);
    mocksFor_processReceives(mocks, 4);
    mocksFor_processMessages(mocks, 8, false);
    EXPECTED_CALL(mocks, pn_message_properties(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, pn_data_next(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, pn_data_get_map(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, pn_message_get_id(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, pn_message_get_correlation_id(IGNORED_PTR_ARG)).SetReturn(test_correlationId_atom);
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_SetMessageId(TEST_IOTHUB_MESSAGE_HANDLE, TEST_MESSAGE_ID));
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_SetCorrelationId(TEST_IOTHUB_MESSAGE_HANDLE, TEST_MESSAGE_ID));

    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    transportFunctions->IoTHubTransport_Subscribe(transportHandle);

    // act
    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

TEST_FUNCTION(DoWork_messages_pn_message_get_correlation_id_Fail)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    proton_messenger_init_path(mocks, TEST_HAPPY_PATH);
    mocksFor_rollbackEvent(mocks, 1);
    mock_renewHappyPath(mocks);
    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(0); // From the call to renew if necessary.
    setupAmqpMessengerDispose(mocks, 2);
    mocksFor_protonWork(mocks, TEST_HAPPY_PATH);
    mocksFor_NoTelementry(mocks);
    mocksFor_processReceives(mocks, 4);
    mocksFor_processMessages(mocks, 12, false);
    EXPECTED_CALL(mocks, pn_message_properties(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, pn_data_next(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, pn_data_get_map(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, pn_message_get_id(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, pn_message_get_correlation_id(IGNORED_PTR_ARG)).SetReturn(test_pn_atom);
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_SetMessageId(TEST_IOTHUB_MESSAGE_HANDLE, TEST_MESSAGE_ID));

    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    transportFunctions->IoTHubTransport_Subscribe(transportHandle);

    // act
    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

TEST_FUNCTION(DoWork_messages_SetCorrelationId_Fail)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    IOTHUBTRANSPORT_CONFIG config = { &TEST_IOTHUBCLIENT_CONFIG, &wts };
    TRANSPORT_PROVIDER* transportFunctions;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    transportFunctions = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    mocks_createHappyPath(mocks, &config);
    setupDestroyStateHappyPath(mocks, TEST_HAPPY_PATH);
    proton_messenger_init_path(mocks, TEST_HAPPY_PATH);
    mocksFor_rollbackEvent(mocks, 1);
    mock_renewHappyPath(mocks);
    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(0); // From the call to renew if necessary.
    setupAmqpMessengerDispose(mocks, 2);
    mocksFor_protonWork(mocks, TEST_HAPPY_PATH);
    mocksFor_NoTelementry(mocks);
    mocksFor_processReceives(mocks, 4);
    mocksFor_processMessages(mocks, 12, false);
    EXPECTED_CALL(mocks, pn_message_properties(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, pn_data_next(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, pn_data_get_map(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, pn_message_get_id(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, pn_message_get_correlation_id(IGNORED_PTR_ARG)).SetReturn(test_correlationId_atom);
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_SetMessageId(TEST_IOTHUB_MESSAGE_HANDLE, TEST_MESSAGE_ID));
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_SetCorrelationId(TEST_IOTHUB_MESSAGE_HANDLE, TEST_MESSAGE_ID)).SetReturn(IOTHUB_MESSAGE_ERROR);

    TRANSPORT_HANDLE transportHandle = transportFunctions->IoTHubTransport_Create(&config);

    transportFunctions->IoTHubTransport_Subscribe(transportHandle);

    // act
    transportFunctions->IoTHubTransport_DoWork(transportHandle, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // Cleanup
    transportFunctions->IoTHubTransport_Destroy(transportHandle);
}

END_TEST_SUITE(iothubtransportamqp_unittests)
