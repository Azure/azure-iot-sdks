// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <cstdlib>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include <cstdint>
#include <csignal>

#include "testrunnerswitcher.h"
#include "micromock.h"
#include "micromockcharstararenullterminatedstrings.h"

static MICROMOCK_MUTEX_HANDLE g_testByTest;
static MICROMOCK_GLOBAL_SEMAPHORE_HANDLE g_dllByDll;

#include "azure_c_shared_utility/buffer_.h"
#include "azure_c_shared_utility/lock.h"
#include "azure_c_shared_utility/doublylinkedlist.h"
#include "azure_c_shared_utility/strings.h"
#include "azure_c_shared_utility/urlencode.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/sastoken.h"
#include "azure_c_shared_utility/tlsio_schannel.h"
#include "azure_c_shared_utility/tlsio_openssl.h"
#include "azure_c_shared_utility/platform.h"
#include "azure_c_shared_utility/xio.h"
#include "azure_c_shared_utility/macro_utils.h"
#include "azure_c_shared_utility/map.h"
#include "azure_c_shared_utility/wsio.h"

#include "uamqp_messaging.h"
#include "iothubtransportamqp.h"
#include "iothub_client_options.h"
#include "iothub_client_private.h"
#include "iothub_message.h"

#include "azure_uamqp_c/amqpvalue.h"
#include "azure_uamqp_c/amqpvalue_to_string.h"
#include "azure_uamqp_c/amqp_definitions.h"
#include "azure_uamqp_c/amqp_frame_codec.h"
#include "azure_uamqp_c/amqp_management.h"
#include "azure_uamqp_c/amqp_types.h"
#include "azure_uamqp_c/cbs.h"
#include "azure_uamqp_c/connection.h"
#include "azure_uamqp_c/frame_codec.h"
#include "azure_uamqp_c/header_detect_io.h"
#include "azure_uamqp_c/link.h"
#include "azure_uamqp_c/message.h"
#include "azure_uamqp_c/message_receiver.h"
#include "azure_uamqp_c/message_sender.h"
#include "azure_uamqp_c/messaging.h"
#include "azure_uamqp_c/saslclientio.h"
#include "azure_uamqp_c/sasl_anonymous.h"
#include "azure_uamqp_c/sasl_frame_codec.h"
#include "azure_uamqp_c/sasl_mechanism.h"
#include "azure_uamqp_c/sasl_mssbcbs.h"
#include "azure_uamqp_c/sasl_plain.h"
#include "azure_uamqp_c/session.h"
#include "azure_uamqp_c/socket_listener.h"


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
#include "strings.c"
#include "../adapters/agenttime.c"
};

DEFINE_ENUM(MESSAGERECEIVER_CREATION_ACTION, MESSAGERECEIVER_CREATE, MESSAGERECEIVER_DESTROY, MESSAGERECEIVER_NONE)

DEFINE_MICROMOCK_ENUM_TO_STRING(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_RESULT_VALUES);
DEFINE_MICROMOCK_ENUM_TO_STRING(IOTHUB_CLIENT_CONFIRMATION_RESULT, IOTHUB_CLIENT_CONFIRMATION_RESULT_VALUES);

// Overloading operators for Micromock
static bool operator==(BINARY_DATA left, BINARY_DATA right)
{
    bool areEqual = false;

    if (left.length == right.length)
    {
        size_t i;
        areEqual = true;
        
        for (i = 0; i < left.length; i++)
        {
            if (left.bytes[i] != right.bytes[i])
            {
                areEqual = false;
                break;
            }
        }
    }

    return areEqual;
}

std::ostream& operator<<(std::ostream& left, const BINARY_DATA bindata)
{
    return left << "struct BINARY_DATA = ([length=" << bindata.length << " bytes] " << bindata.bytes << ")";
}


// Control parameters
#define INDEFINITE_TIME ((time_t)-1)
#define TEST_DEVICE_ID "deviceid"
#define TEST_DEVICE_KEY "devicekey"
#define TEST_DEVICE_SAS "deviceSas"
#define TEST_IOT_HUB_NAME "servername"
#define TEST_IOT_HUB_SUFFIX "domainname"
#define TEST_PROT_GW_HOSTNAME_NULL NULL
#define TEST_PROT_GW_HOSTNAME "gw"

#define TEST_IOT_HUB_PORT 5671
#define TEST_INCOMING_WINDOW_SIZE UINT32_MAX
#define TEST_OUTGOING_WINDOW_SIZE 100
#define TEST_MESSAGE_RECEIVER_LINK_NAME "receiver-link"
#define TEST_MESSAGE_RECEIVER_TARGET_ADDRESS "ingress-rx"
#define TEST_MESSAGE_RECEIVER_MAX_LINK_SIZE 65536
#define TEST_MESSAGE_SENDER_LINK_NAME "sender-link"
#define TEST_MESSAGE_SENDER_SOURCE_ADDRESS "ingress"
#define TEST_MESSAGE_SENDER_MAX_LINK_SIZE UINT64_MAX
#define TEST_SAS_TOKEN_LIFETIME_MS 3600000
#define TEST_CBS_REQUEST_TIMEOUT_MS 30000

#define TEST_STRING_HANDLE (STRING_HANDLE)0x46
#define TEST_NULL_STRING_HANDLE (STRING_HANDLE)NULL
#define TEST_STRING_COPY_FAILURE_RESULT 1

#define TEST_IOTHUB_CLIENT_LL_HANDLE (IOTHUB_CLIENT_LL_HANDLE)0x49
#define TEST_TLS_IO_INTERFACE_DESC (IO_INTERFACE_DESCRIPTION*)0x77
#define TEST_TLS_IO_INTERFACE (XIO_HANDLE)0x81
#define TEST_SASL_MECHANISM (SASL_MECHANISM_HANDLE)0x90
#define TEST_SASL_IO (XIO_HANDLE)0x101
#define TEST_CONNECTION (CONNECTION_HANDLE)0x110
#define TEST_SESSION (SESSION_HANDLE)0x120
#define TEST_CBS (CBS_HANDLE)0x130
#define TEST_SAS_TOKEN "SharedAccessSignature sr=" TEST_IOT_HUB_NAME "." TEST_IOT_HUB_SUFFIX "/devices/" TEST_DEVICE_ID "&sig=up5khAl%2fsAI2s4fJ7OnLQBRPrb4y4Z53K%2fJMn1Leu4Q%3d&se=1453961445&skn="
#define TEST_MESSAGESENDER_SOURCE (AMQP_VALUE)0x140
#define TEST_MESSAGESENDER_TARGET (AMQP_VALUE)0x142
#define TEST_MESSAGERECEIVER_SOURCE (AMQP_VALUE)0x144
#define TEST_MESSAGERECEIVER_TARGET (AMQP_VALUE)0x146
#define TEST_MESSAGESENDER_LINK (LINK_HANDLE)0x160
#define TEST_MESSAGERECEIVER_LINK (LINK_HANDLE)0x166
#define TEST_MESSAGE_SENDER (MESSAGE_SENDER_HANDLE)0x180
#define TEST_MESSAGE_RECEIVER (MESSAGE_RECEIVER_HANDLE)0x190
#define TEST_IOTHUB_MESSAGE_HANDLE (IOTHUB_MESSAGE_HANDLE)0x200
#define TEST_RANDOM_CHAR_SEQ "This is a random char sequence"
#define TEST_RANDOM_CHAR_SEQ_SIZE 30
#define TEST_BINARY_BUFFER (const unsigned char*)0x210
#define TEST_BINARY_BUFFER_SIZE 56
#define TEST_OPTION_SASTOKEN_LIFETIME "sas_token_lifetime"
#define TEST_OPTION_SASTOKEN_REFRESH_TIME "sas_token_refresh_time"
#define TEST_OPTION_CBS_REQUEST_TIMEOUT "cbs_request_timeout"
#define TEST_OPTION_MESSAGE_SEND_TIMEOUT "message_send_timeout"
#define TEST_AMQP_VALUE_TEST_HANDLE         (AMQP_VALUE)0x300
#define TEST_UAMQP_MAP                      (AMQP_VALUE)0x301
#define TEST_IOTHUB_MESSAGE_PROPERTIES_MAP  (MAP_HANDLE)0x302


#define TEST_PROPERTY_1_KEY_UAMQP_VALUE     (AMQP_VALUE)0x303
#define TEST_PROPERTY_1_VALUE_UAMQP_VALUE   (AMQP_VALUE)0x304
#define TEST_PROPERTY_2_KEY_UAMQP_VALUE     (AMQP_VALUE)0x305
#define TEST_PROPERTY_2_VALUE_UAMQP_VALUE   (AMQP_VALUE)0x306

#define TEST_UAMQP_PROPERTIES				(PROPERTIES_HANDLE)0x307
#define TEST_MESSAGE_ID						"Message-id:Test"
#define TEST_CORRELATION_ID					"Correlation-id:Test"
#define TEST_UAMQP_MESSAGE_ID				(AMQP_VALUE)0x310
#define TEST_UAMQP_CORRELATION_ID			(AMQP_VALUE)0x311

#define TEST_MESSAGE_HANDLE (MESSAGE_HANDLE)0x440
#define TEST_MAP_HANDLE (MAP_HANDLE)0x448
#define TEST_AMQP_MAP_VALUE (AMQP_VALUE)0x449

#define TEST_AMQP_DISPOSITION_ACCEPTED (AMQP_VALUE)0x450
#define TEST_AMQP_DISPOSITION_ABANDONED (AMQP_VALUE)0x451
#define TEST_AMQP_DISPOSITION_REJECTED (AMQP_VALUE)0x452


static const char* const no_property_keys[] = { "test_property_key" };
static const char* const no_property_values[] = { "test_property_value" };
static const char* const* no_property_keys_ptr = no_property_keys;
static const char* const* no_property_values_ptr = no_property_values;
static size_t no_property_size = 0;

static const char* const two_property_keys[] = { "test_property_key", "prop_key_2" };
static const char* const two_property_values[] = { "test_property_value", "prop_value_2" };
static const char* const* two_property_keys_ptr = two_property_keys;
static const char* const* two_property_values_ptr = two_property_values;
static size_t two_properties_size = 2;

static time_t test_current_time;
static size_t test_latest_SASToken_expiry_time = 0;
static ON_CBS_OPERATION_COMPLETE test_latest_cbs_put_token_callback;
static void* test_latest_cbs_put_token_context;
static int test_number_of_event_confirmation_callbacks_invoked;
static int test_sum_of_event_confirmation_callback_contexts;
static BINARY_DATA test_binary_data;
static MESSAGE_BODY_TYPE test_message_get_body_type = MESSAGE_BODY_TYPE_DATA;

static bool fail_malloc = false;
static bool fail_STRING_new = false;
static bool fail_STRING_new_with_memory = false;
static bool fail_STRING_construct = false;

static STRING_HANDLE test_STRING_HANDLE_EMPTY;
static STRING_HANDLE test_iotHubHostFqdn;

ON_MESSAGE_SENDER_STATE_CHANGED saved_on_message_sender_state_changed_callback;
void* saved_on_message_sender_state_changed_context;
ON_MESSAGE_RECEIVER_STATE_CHANGED saved_on_message_receiver_state_changed_callback;
void* saved_on_message_receiver_state_changed_context;

static ON_MESSAGE_RECEIVED saved_on_message_received_callback;
static const void* saved_on_message_received_context;
static BINARY_DATA* saved_message_get_body_amqp_data_binary_data;
static PROPERTIES_HANDLE saved_message_get_properties_properties = TEST_UAMQP_PROPERTIES;
static PROPERTIES_HANDLE saved_message_set_properties_properties;
static AMQP_VALUE* saved_properties_get_message_id_value;
static AMQP_VALUE* saved_properties_get_correlation_id_value;
static const char* test_amqpvalue_get_string_values[] = {"abcd", "bike", "ball", "boat", "kite", "bell", "boys", "girl", "monk", "wolf"};
static int test_amqpvalue_get_string_index = 0;
static int test_amqpvalue_get_string_length = 10;
static AMQP_VALUE test_message_get_application_properties_return = TEST_AMQP_MAP_VALUE;

static int test_amqpvalue_get_map_pair_count = 1;

// **  Mocks **
TYPED_MOCK_CLASS(CIoTHubTransportAMQPMocks, CGlobalMock)
{
public:
    /* Shared Utilities Mocks */
    // platform.h
    MOCK_STATIC_METHOD_0(, int, platform_init)
    MOCK_METHOD_END(int, 0)

    MOCK_STATIC_METHOD_0(, void, platform_deinit)
    MOCK_VOID_METHOD_END()

    MOCK_STATIC_METHOD_0(, const IO_INTERFACE_DESCRIPTION*, platform_get_default_tlsio)
    MOCK_METHOD_END(const IO_INTERFACE_DESCRIPTION*, 0)

    /* amqpvalue Mocks */
    MOCK_STATIC_METHOD_1(, void, amqpvalue_destroy, AMQP_VALUE, value)
    MOCK_VOID_METHOD_END()

    MOCK_STATIC_METHOD_0(, AMQP_VALUE, amqpvalue_create_map)
    MOCK_METHOD_END(AMQP_VALUE, NULL)

    MOCK_STATIC_METHOD_1(, AMQP_VALUE, amqpvalue_create_symbol, const char*, value)
    MOCK_METHOD_END(AMQP_VALUE, NULL)

    MOCK_STATIC_METHOD_1(, AMQP_VALUE, amqpvalue_create_string, const char*, value)
    MOCK_METHOD_END(AMQP_VALUE, NULL)

    MOCK_STATIC_METHOD_1(, AMQP_TYPE, amqpvalue_get_type, AMQP_VALUE, value)
    MOCK_METHOD_END(AMQP_TYPE, AMQP_TYPE_UNKNOWN)

    MOCK_STATIC_METHOD_2(, int, amqpvalue_get_string, AMQP_VALUE, value, const char**, string_value)
        if (test_amqpvalue_get_string_length > 0 && test_amqpvalue_get_string_index < test_amqpvalue_get_string_length)
        {
            *string_value = (const char*)test_amqpvalue_get_string_values[test_amqpvalue_get_string_index++];
        }
        else
        {
            *string_value = (const char*)NULL;
        }
    MOCK_METHOD_END(int, 0)

    MOCK_STATIC_METHOD_3(, int, amqpvalue_set_map_value, AMQP_VALUE, map, AMQP_VALUE, key, AMQP_VALUE, value)
    MOCK_METHOD_END(int, 0)

    /* Map mocks */
    MOCK_STATIC_METHOD_4(, MAP_RESULT, Map_GetInternals, MAP_HANDLE, handle, const char*const**, keys, const char*const**, values, size_t*, count);
    MOCK_METHOD_END(MAP_RESULT, MAP_OK)

    MOCK_STATIC_METHOD_3(, MAP_RESULT, Map_AddOrUpdate, MAP_HANDLE, handle, const char*, key, const char*, value)
    MOCK_METHOD_END(MAP_RESULT, MAP_OK)

    /* crt_abstractions mocks */
    MOCK_STATIC_METHOD_2(, int, mallocAndStrcpy_s, char**, destination, const char*, source)
    MOCK_METHOD_END(int, (*destination = (char*)BASEIMPLEMENTATION::gballoc_malloc(strlen(source) + 1), strcpy(*destination, source), 0))

    /* IoTHub_Message mocks */
    MOCK_STATIC_METHOD_2(, IOTHUB_MESSAGE_HANDLE, IoTHubMessage_CreateFromByteArray, const unsigned char*, buffer, size_t, size)
    MOCK_METHOD_END(IOTHUB_MESSAGE_HANDLE, NULL)

    MOCK_STATIC_METHOD_1(, void, IoTHubMessage_Destroy, IOTHUB_MESSAGE_HANDLE, iotHubMessageHandle)
    MOCK_VOID_METHOD_END()

    MOCK_STATIC_METHOD_3(, IOTHUB_MESSAGE_RESULT, IoTHubMessage_GetByteArray, IOTHUB_MESSAGE_HANDLE, iotHubMessageHandle, const unsigned char**, buffer, size_t*, size)
    MOCK_METHOD_END(IOTHUB_MESSAGE_RESULT, IOTHUB_MESSAGE_OK)

    MOCK_STATIC_METHOD_1(, const char*, IoTHubMessage_GetString, IOTHUB_MESSAGE_HANDLE, handle)
        const char* result2;
        if (handle == NULL)
        {
            result2 = (const char*)NULL;
        }
        else
        {
            result2 = TEST_RANDOM_CHAR_SEQ;
        }
    MOCK_METHOD_END(const char*, result2)

    MOCK_STATIC_METHOD_1(, IOTHUBMESSAGE_CONTENT_TYPE, IoTHubMessage_GetContentType, IOTHUB_MESSAGE_HANDLE, iotHubMessageHandle)
        IOTHUBMESSAGE_CONTENT_TYPE result2;
        if (iotHubMessageHandle == NULL)
        {
            result2 = IOTHUBMESSAGE_STRING;
        }
        else
        {
            result2 = IOTHUBMESSAGE_BYTEARRAY;
        }
    MOCK_METHOD_END(IOTHUBMESSAGE_CONTENT_TYPE, result2)

    MOCK_STATIC_METHOD_1(, IOTHUB_MESSAGE_HANDLE, IoTHubMessage_Clone, IOTHUB_MESSAGE_HANDLE, iotHubMessageHandle)
    MOCK_METHOD_END(IOTHUB_MESSAGE_HANDLE, NULL)

    MOCK_STATIC_METHOD_1(, MAP_HANDLE, IoTHubMessage_Properties, IOTHUB_MESSAGE_HANDLE, iotHubMessageHandle)
    MOCK_METHOD_END(MAP_HANDLE, NULL)

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
    MOCK_METHOD_END(STRING_HANDLE, NULL)

    /* STRING Mocks */
    MOCK_STATIC_METHOD_1(, STRING_HANDLE, STRING_new_with_memory, const char*, memory)
        STRING_HANDLE handle = BASEIMPLEMENTATION::STRING_new_with_memory(memory);
    MOCK_METHOD_END(STRING_HANDLE, handle)

    MOCK_STATIC_METHOD_0(, STRING_HANDLE, STRING_new)
        STRING_HANDLE handle = BASEIMPLEMENTATION::STRING_new();
    MOCK_METHOD_END(STRING_HANDLE, handle)

    MOCK_STATIC_METHOD_1(, STRING_HANDLE, STRING_construct, const char*, psz)
        STRING_HANDLE handle = BASEIMPLEMENTATION::STRING_construct(psz);
    MOCK_METHOD_END(STRING_HANDLE, handle)

    MOCK_STATIC_METHOD_2(, int, STRING_concat, STRING_HANDLE, handle, const char*, s2)
        int local_result = BASEIMPLEMENTATION::STRING_concat(handle, s2);
    MOCK_METHOD_END(int, local_result)

    MOCK_STATIC_METHOD_2(, int, STRING_concat_with_STRING, STRING_HANDLE, s1, STRING_HANDLE, s2)
        int local_result = BASEIMPLEMENTATION::STRING_concat_with_STRING(s1, s2);
    MOCK_METHOD_END(int, local_result)

    MOCK_STATIC_METHOD_1(, void, STRING_delete, STRING_HANDLE, handle)
        BASEIMPLEMENTATION::STRING_delete(handle);
    MOCK_VOID_METHOD_END()

    MOCK_STATIC_METHOD_1(, const char*, STRING_c_str, STRING_HANDLE, s)
        const char* content = BASEIMPLEMENTATION::STRING_c_str(s);
    MOCK_METHOD_END(const char*, content)

    MOCK_STATIC_METHOD_1(, size_t, STRING_length, STRING_HANDLE, s)
        size_t size = BASEIMPLEMENTATION::STRING_length(s);
    MOCK_METHOD_END(size_t, size)

    MOCK_STATIC_METHOD_1(, int, STRING_empty, STRING_HANDLE, s)
        int local_result = BASEIMPLEMENTATION::STRING_empty(s);
    MOCK_METHOD_END(int, local_result)

    MOCK_STATIC_METHOD_2(, int, STRING_copy, STRING_HANDLE, s1, const char*, s2)
        int local_result = BASEIMPLEMENTATION::STRING_copy(s1, s2);
    MOCK_METHOD_END(int, local_result)

        MOCK_STATIC_METHOD_1(, STRING_HANDLE, STRING_clone, STRING_HANDLE, sHandle)
        STRING_HANDLE new_handle = BASEIMPLEMENTATION::STRING_clone(sHandle);
    MOCK_METHOD_END(STRING_HANDLE, new_handle)

    /* BUFFER Mocks */
    MOCK_STATIC_METHOD_0(, BUFFER_HANDLE, BUFFER_new)
        BUFFER_HANDLE result2 = NULL;
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
        void* transport_state = NULL;
        if (!fail_malloc)
        {
            transport_state = BASEIMPLEMENTATION::gballoc_malloc(size);
        }
    MOCK_METHOD_END(void*, transport_state);

    MOCK_STATIC_METHOD_2(, void*, gballoc_realloc, void*, ptr, size_t, size)
    MOCK_METHOD_END(void*, BASEIMPLEMENTATION::gballoc_realloc(ptr, size));

    MOCK_STATIC_METHOD_1(, void, gballoc_free, void*, ptr)
        BASEIMPLEMENTATION::gballoc_free(ptr);
    MOCK_VOID_METHOD_END()

    MOCK_STATIC_METHOD_2(, IOTHUBMESSAGE_DISPOSITION_RESULT, IoTHubClient_LL_MessageCallback, IOTHUB_CLIENT_LL_HANDLE, handle, IOTHUB_MESSAGE_HANDLE, messageHandle)
    MOCK_METHOD_END(IOTHUBMESSAGE_DISPOSITION_RESULT, IOTHUBMESSAGE_ACCEPTED);

    MOCK_STATIC_METHOD_3(, void, IoTHubClient_LL_SendComplete, IOTHUB_CLIENT_LL_HANDLE, handle, PDLIST_ENTRY, completedMessages, IOTHUB_CLIENT_CONFIRMATION_RESULT, batchResult)
        PDLIST_ENTRY oldest;
        while ((oldest = BASEIMPLEMENTATION::DList_RemoveHeadList(completedMessages)) != completedMessages)
        {
            ;
        }
    MOCK_VOID_METHOD_END();

    MOCK_STATIC_METHOD_1(, time_t, get_time, time_t*, t)
    MOCK_METHOD_END(time_t, 0);

	MOCK_STATIC_METHOD_1(, struct tm*, get_gmtime, time_t*, t)
		struct tm* get_gmtime_result = BASEIMPLEMENTATION::get_gmtime(t);
	MOCK_METHOD_END(struct tm*, get_gmtime_result);
	MOCK_STATIC_METHOD_1(, time_t, get_mktime, struct tm*, t)
		time_t get_mktime_result = BASEIMPLEMENTATION::get_mktime(t);
	MOCK_METHOD_END(time_t, get_mktime_result);
	MOCK_STATIC_METHOD_2(, double, get_difftime, time_t, stop_time, time_t, start_time)
		double get_difftime = BASEIMPLEMENTATION::get_difftime(stop_time, start_time);
	MOCK_METHOD_END(double, get_difftime);
    MOCK_STATIC_METHOD_4(, STRING_HANDLE, SASToken_Create, STRING_HANDLE, key, STRING_HANDLE, scope, STRING_HANDLE, keyName, size_t, expiry)
        test_latest_SASToken_expiry_time = expiry;
    MOCK_METHOD_END(STRING_HANDLE, BASEIMPLEMENTATION::STRING_construct(TEST_SAS_TOKEN));

    // xio.h
    MOCK_STATIC_METHOD_2(, XIO_HANDLE, xio_create, const IO_INTERFACE_DESCRIPTION*, io_interface_description, const void*, io_create_parameters)
    MOCK_METHOD_END(XIO_HANDLE, 0)

    MOCK_STATIC_METHOD_1(, void, xio_destroy, XIO_HANDLE, xio)
    MOCK_VOID_METHOD_END()

    MOCK_STATIC_METHOD_5(, int, xio_open, XIO_HANDLE, xio, ON_IO_OPEN_COMPLETE, on_io_open_complete, ON_BYTES_RECEIVED, on_bytes_received, ON_IO_ERROR, on_io_error, void*, callback_context)
    MOCK_METHOD_END(int, 0)

    MOCK_STATIC_METHOD_3(, int, xio_close, XIO_HANDLE, xio, ON_IO_CLOSE_COMPLETE, on_io_close_complete, void*, callback_context)
    MOCK_METHOD_END(int, 0)

    MOCK_STATIC_METHOD_5(, int, xio_send, XIO_HANDLE, xio, const void*, buffer, size_t, size, ON_SEND_COMPLETE, on_send_complete, void*, callback_context)
    MOCK_METHOD_END(int, 0)

    MOCK_STATIC_METHOD_1(, void, xio_dowork, XIO_HANDLE, xio)
    MOCK_VOID_METHOD_END()

    MOCK_STATIC_METHOD_3(, int, xio_setoption, XIO_HANDLE, xio, const char*, optionName, const void*, value)
    MOCK_METHOD_END(int, 0)
        
    // tlsio_openssl.h
    MOCK_STATIC_METHOD_0(, int, tlsio_openssl_init)
    MOCK_METHOD_END(int, 0)

    MOCK_STATIC_METHOD_0(, void, tlsio_openssl_deinit)
    MOCK_VOID_METHOD_END()

    MOCK_STATIC_METHOD_1(, CONCRETE_IO_HANDLE, tlsio_openssl_create, void*, io_create_parameters)
    MOCK_METHOD_END(CONCRETE_IO_HANDLE, 0)

    MOCK_STATIC_METHOD_1(, void, tlsio_openssl_destroy, CONCRETE_IO_HANDLE, tls_io)
    MOCK_VOID_METHOD_END()

    MOCK_STATIC_METHOD_5(, int, tlsio_openssl_open, CONCRETE_IO_HANDLE, tls_io, ON_IO_OPEN_COMPLETE, on_io_open_complete, ON_BYTES_RECEIVED, on_bytes_received, ON_IO_ERROR, on_io_error, void*, callback_context)
    MOCK_METHOD_END(int, 0)

    MOCK_STATIC_METHOD_3(, int, tlsio_openssl_close, CONCRETE_IO_HANDLE, tls_io, ON_IO_CLOSE_COMPLETE, on_io_close_complete, void*, callback_context)
    MOCK_METHOD_END(int, 0)

    MOCK_STATIC_METHOD_5(, int, tlsio_openssl_send, CONCRETE_IO_HANDLE, tls_io, const void*, buffer, size_t, size, ON_SEND_COMPLETE, on_send_complete, void*, callback_context)
    MOCK_METHOD_END(int, 0)

    MOCK_STATIC_METHOD_1(, void, tlsio_openssl_dowork, CONCRETE_IO_HANDLE, tls_io)
    MOCK_VOID_METHOD_END()

    MOCK_STATIC_METHOD_0(, const IO_INTERFACE_DESCRIPTION*, tlsio_openssl_get_interface_description)
    MOCK_METHOD_END(const IO_INTERFACE_DESCRIPTION*, 0)

    // tlsio_schannel.h
    MOCK_STATIC_METHOD_1(, CONCRETE_IO_HANDLE, tlsio_schannel_create, void*, io_create_parameters)
    MOCK_METHOD_END(CONCRETE_IO_HANDLE, 0)

    MOCK_STATIC_METHOD_1(, void, tlsio_schannel_destroy, CONCRETE_IO_HANDLE, tls_io)
    MOCK_VOID_METHOD_END()

    MOCK_STATIC_METHOD_5(, int, tlsio_schannel_open, CONCRETE_IO_HANDLE, tls_io, ON_IO_OPEN_COMPLETE, on_io_open_complete, ON_BYTES_RECEIVED, on_bytes_received, ON_IO_ERROR, on_io_error, void*, callback_context)
    MOCK_METHOD_END(int, 0)

    MOCK_STATIC_METHOD_3(, int, tlsio_schannel_close, CONCRETE_IO_HANDLE, tls_io, ON_IO_CLOSE_COMPLETE, on_io_close_complete, void*, callback_context)
    MOCK_METHOD_END(int, 0)

    MOCK_STATIC_METHOD_5(, int, tlsio_schannel_send, CONCRETE_IO_HANDLE, tls_io, const void*, buffer, size_t, size, ON_SEND_COMPLETE, on_send_complete, void*, callback_context)
    MOCK_METHOD_END(int, 0)

    MOCK_STATIC_METHOD_1(, void, tlsio_schannel_dowork, CONCRETE_IO_HANDLE, tls_io)
    MOCK_VOID_METHOD_END()

    MOCK_STATIC_METHOD_0(, const IO_INTERFACE_DESCRIPTION*, tlsio_schannel_get_interface_description)
    MOCK_METHOD_END(const IO_INTERFACE_DESCRIPTION*, 0)

    // AMQP Mocks
    // amqp_definitions.h
    MOCK_STATIC_METHOD_0(, PROPERTIES_HANDLE, properties_create)
    MOCK_METHOD_END(PROPERTIES_HANDLE, 0)

    MOCK_STATIC_METHOD_2(, int, amqpvalue_get_map_pair_count, AMQP_VALUE, map, uint32_t*, pair_count)
        *pair_count = test_amqpvalue_get_map_pair_count;
    MOCK_METHOD_END(int, 0)

    MOCK_STATIC_METHOD_4(, int, amqpvalue_get_map_key_value_pair, AMQP_VALUE, map, uint32_t, index, AMQP_VALUE*, key, AMQP_VALUE*, value)
    MOCK_METHOD_END(int, 0)

    MOCK_STATIC_METHOD_2(, int, properties_get_message_id, PROPERTIES_HANDLE, properties, AMQP_VALUE*, message_id_value)
        saved_properties_get_message_id_value = message_id_value;
    MOCK_METHOD_END(int, 0)

    MOCK_STATIC_METHOD_2(, int, properties_set_message_id, PROPERTIES_HANDLE, properties, AMQP_VALUE, message_id_value)
    MOCK_METHOD_END(int, 0)

    MOCK_STATIC_METHOD_1(, AMQP_VALUE, amqpvalue_get_inplace_described_value, AMQP_VALUE, value)
    MOCK_METHOD_END(AMQP_VALUE, 0)

    MOCK_STATIC_METHOD_2(, int, properties_get_correlation_id, PROPERTIES_HANDLE, properties, AMQP_VALUE*, correlation_id_value)
        saved_properties_get_correlation_id_value = correlation_id_value;
    MOCK_METHOD_END(int, 0)

    MOCK_STATIC_METHOD_2(, int, properties_set_correlation_id, PROPERTIES_HANDLE, properties, AMQP_VALUE, correlation_id_value)
    MOCK_METHOD_END(int, 0)

    MOCK_STATIC_METHOD_2(, int, message_get_properties, MESSAGE_HANDLE, message, PROPERTIES_HANDLE*, properties)
        *properties = saved_message_get_properties_properties;
    MOCK_METHOD_END(int, 0)

    MOCK_STATIC_METHOD_2(, int, message_set_properties, MESSAGE_HANDLE, message, PROPERTIES_HANDLE, properties)
        saved_message_set_properties_properties = properties;
    MOCK_METHOD_END(int, 0)

    MOCK_STATIC_METHOD_1(, void, properties_destroy, PROPERTIES_HANDLE, properties)
        /*because */
    MOCK_VOID_METHOD_END()

    // cbs.h
    MOCK_STATIC_METHOD_3(, CBS_HANDLE, cbs_create, SESSION_HANDLE, session, ON_AMQP_MANAGEMENT_STATE_CHANGED, on_amqp_management_state_changed, void*, callback_context)
    MOCK_METHOD_END(CBS_HANDLE, TEST_CBS)

    MOCK_STATIC_METHOD_1(, void, cbs_destroy, CBS_HANDLE, cbs)
    MOCK_VOID_METHOD_END()

    MOCK_STATIC_METHOD_1(, int, cbs_open, CBS_HANDLE, amqp_management)
    MOCK_METHOD_END(int, 0)

    MOCK_STATIC_METHOD_1(, int, cbs_close, CBS_HANDLE, amqp_management)
    MOCK_METHOD_END(int, 0)

    MOCK_STATIC_METHOD_6(, int, cbs_put_token, CBS_HANDLE, cbs, const char*, type, const char*, audience, const char*, token, ON_CBS_OPERATION_COMPLETE, on_cbs_operation_complete, void*, context)
        test_latest_cbs_put_token_callback = on_cbs_operation_complete;
        test_latest_cbs_put_token_context = context;
    MOCK_METHOD_END(int, 0)

    MOCK_STATIC_METHOD_5(, int, cbs_delete_token, CBS_HANDLE, cbs, const char*, type, const char*, audience, ON_CBS_OPERATION_COMPLETE, on_cbs_operation_complete, void*, context)
    MOCK_METHOD_END(int, 0)

    // connection.h
    MOCK_STATIC_METHOD_5(, CONNECTION_HANDLE, connection_create, XIO_HANDLE, io, const char*, hostname, const char*, container_id, ON_NEW_ENDPOINT, on_new_endpoint, void*, callback_context)
    MOCK_METHOD_END(CONNECTION_HANDLE, TEST_CONNECTION)

    MOCK_STATIC_METHOD_9(, CONNECTION_HANDLE, connection_create2, XIO_HANDLE, io, const char*, hostname, const char*, container_id, ON_NEW_ENDPOINT, on_new_endpoint, void*, callback_context, ON_CONNECTION_STATE_CHANGED, on_connection_state_changed, void*, on_connection_state_changed_context, ON_IO_ERROR, on_io_error, void*, on_io_error_context)
    MOCK_METHOD_END(CONNECTION_HANDLE, TEST_CONNECTION)

    MOCK_STATIC_METHOD_1(, void, connection_destroy, CONNECTION_HANDLE, connection)
    MOCK_VOID_METHOD_END()

    MOCK_STATIC_METHOD_1(, void, connection_dowork, CONNECTION_HANDLE, connection)
    MOCK_VOID_METHOD_END()

    MOCK_STATIC_METHOD_2(, void, connection_set_trace, CONNECTION_HANDLE, connection, bool, traceOn)
    MOCK_VOID_METHOD_END()

    // link.h
    MOCK_STATIC_METHOD_5(, LINK_HANDLE, link_create, SESSION_HANDLE, session, const char*, name, role, _role, AMQP_VALUE, source, AMQP_VALUE, target)
    MOCK_METHOD_END(LINK_HANDLE, 0)

    MOCK_STATIC_METHOD_2(, int, link_set_max_message_size, LINK_HANDLE, link, uint64_t, max_message_size)
    MOCK_METHOD_END(int, 0)
        
    MOCK_STATIC_METHOD_2(, int, link_set_rcv_settle_mode, LINK_HANDLE, link, receiver_settle_mode, rcv_settle_mode)
    MOCK_METHOD_END(int, 0)

    MOCK_STATIC_METHOD_1(, void, link_destroy, LINK_HANDLE, handle)
    MOCK_VOID_METHOD_END()

    MOCK_STATIC_METHOD_2(, int, link_set_attach_properties, LINK_HANDLE, link, fields, attach_properties)
    MOCK_METHOD_END(int, 0)

    // message.h
    MOCK_STATIC_METHOD_0(, MESSAGE_HANDLE, message_create)
    MOCK_METHOD_END(MESSAGE_HANDLE, NULL)

    MOCK_STATIC_METHOD_2(, int, message_get_application_properties, MESSAGE_HANDLE, message, AMQP_VALUE*, application_properties)
        *application_properties = test_message_get_application_properties_return;
    MOCK_METHOD_END(int, 0)

    MOCK_STATIC_METHOD_2(, int, message_set_application_properties, MESSAGE_HANDLE, message, AMQP_VALUE, application_properties);
    MOCK_METHOD_END(int, 0)

    MOCK_STATIC_METHOD_2(, int, message_add_body_amqp_data, MESSAGE_HANDLE, message, BINARY_DATA, binary_data)
    MOCK_METHOD_END(int, 0)

    MOCK_STATIC_METHOD_1(, void, message_destroy, MESSAGE_HANDLE, message)
    MOCK_VOID_METHOD_END()

    MOCK_STATIC_METHOD_3(, int, message_get_body_amqp_data, MESSAGE_HANDLE, message, size_t, index, BINARY_DATA*, binary_data)
        saved_message_get_body_amqp_data_binary_data = binary_data;
    MOCK_METHOD_END(int, 0)

    MOCK_STATIC_METHOD_2(, int, message_get_body_type, MESSAGE_HANDLE, message, MESSAGE_BODY_TYPE*, body_type)
        *body_type = test_message_get_body_type;
    MOCK_METHOD_END(int, 0)

    // message_receiver.h
    MOCK_STATIC_METHOD_3(, MESSAGE_RECEIVER_HANDLE, messagereceiver_create, LINK_HANDLE, link, ON_MESSAGE_RECEIVER_STATE_CHANGED, on_message_receiver_state_changed, void*, context)
        saved_on_message_receiver_state_changed_callback = on_message_receiver_state_changed;
        saved_on_message_receiver_state_changed_context = context;
    MOCK_METHOD_END(MESSAGE_RECEIVER_HANDLE, 0)

    MOCK_STATIC_METHOD_1(, void, messagereceiver_destroy, MESSAGE_RECEIVER_HANDLE, message_receiver)
    MOCK_VOID_METHOD_END()

    MOCK_STATIC_METHOD_3(, int, messagereceiver_open, MESSAGE_RECEIVER_HANDLE, message_receiver, ON_MESSAGE_RECEIVED, on_message_received, const void*, callback_context)
        saved_on_message_received_callback = on_message_received;
        saved_on_message_received_context = callback_context;
    MOCK_METHOD_END(int, 0)

    MOCK_STATIC_METHOD_1(, int, messagereceiver_close, MESSAGE_RECEIVER_HANDLE, message_receiver)
    MOCK_METHOD_END(int, 0)


    // message_sender.h
    MOCK_STATIC_METHOD_3(, MESSAGE_SENDER_HANDLE, messagesender_create, LINK_HANDLE, link, ON_MESSAGE_SENDER_STATE_CHANGED, on_message_sender_state_changed, void*, context)
        saved_on_message_sender_state_changed_callback = on_message_sender_state_changed;
        saved_on_message_sender_state_changed_context = context;
    MOCK_METHOD_END(MESSAGE_SENDER_HANDLE, 0)

    MOCK_STATIC_METHOD_1(, void, messagesender_destroy, MESSAGE_SENDER_HANDLE, message_sender)
    MOCK_VOID_METHOD_END()

    MOCK_STATIC_METHOD_1(, int, messagesender_open, MESSAGE_SENDER_HANDLE, message_sender)
    MOCK_METHOD_END(int, 0)

    MOCK_STATIC_METHOD_1(, int, messagesender_close, MESSAGE_SENDER_HANDLE, message_sender)
    MOCK_METHOD_END(int, 0)

    MOCK_STATIC_METHOD_4(, int, messagesender_send, MESSAGE_SENDER_HANDLE, message_sender, MESSAGE_HANDLE, message, ON_MESSAGE_SEND_COMPLETE, on_message_send_complete, void*, callback_context)
    MOCK_METHOD_END(int, 0)

    // messaging.h
    MOCK_STATIC_METHOD_1(, AMQP_VALUE, messaging_create_source, const char*, address)
    MOCK_METHOD_END(AMQP_VALUE, 0)

    MOCK_STATIC_METHOD_1(, AMQP_VALUE, messaging_create_target, const char*, address)
    MOCK_METHOD_END(AMQP_VALUE, 0)

    MOCK_STATIC_METHOD_2(, AMQP_VALUE, messaging_delivery_received, uint32_t, section_number, uint64_t, section_offset)
    MOCK_METHOD_END(AMQP_VALUE, 0)

    MOCK_STATIC_METHOD_0(, AMQP_VALUE, messaging_delivery_accepted)
    MOCK_METHOD_END(AMQP_VALUE, 0)

    MOCK_STATIC_METHOD_2(, AMQP_VALUE, messaging_delivery_rejected, const char*, error_condition, const char*, error_description)
    MOCK_METHOD_END(AMQP_VALUE, 0)

    MOCK_STATIC_METHOD_0(, AMQP_VALUE, messaging_delivery_released)
    MOCK_METHOD_END(AMQP_VALUE, 0)

    MOCK_STATIC_METHOD_3(, AMQP_VALUE, messaging_delivery_modified, bool, delivery_failed, bool, undeliverable_here, fields, message_annotations)
    MOCK_METHOD_END(AMQP_VALUE, 0)

    // sasl_mechanism.h
    MOCK_STATIC_METHOD_2(, SASL_MECHANISM_HANDLE, saslmechanism_create, const SASL_MECHANISM_INTERFACE_DESCRIPTION*, sasl_mechanism_interface_description, void*, sasl_mechanism_create_parameters)
    MOCK_METHOD_END(SASL_MECHANISM_HANDLE, TEST_SASL_MECHANISM)

    MOCK_STATIC_METHOD_1(, void, saslmechanism_destroy, SASL_MECHANISM_HANDLE, sasl_mechanism)
    MOCK_VOID_METHOD_END()

    MOCK_STATIC_METHOD_2(, int, saslmechanism_get_init_bytes, SASL_MECHANISM_HANDLE, sasl_mechanism, SASL_MECHANISM_BYTES*, init_bytes)
    MOCK_METHOD_END(int, 0)

    MOCK_STATIC_METHOD_1(, const char*, saslmechanism_get_mechanism_name, SASL_MECHANISM_HANDLE, sasl_mechanism)
    MOCK_METHOD_END(const char*, 0)

    MOCK_STATIC_METHOD_3(, int, saslmechanism_challenge, SASL_MECHANISM_HANDLE, sasl_mechanism, const SASL_MECHANISM_BYTES*, challenge_bytes, SASL_MECHANISM_BYTES*, response_bytes)
    MOCK_METHOD_END(int, 0)

    // saslclientio.h
    MOCK_STATIC_METHOD_1(, CONCRETE_IO_HANDLE, saslclientio_create, void*, io_create_parameters)
    MOCK_METHOD_END(CONCRETE_IO_HANDLE, 0)

    MOCK_STATIC_METHOD_1(, void, saslclientio_destroy, CONCRETE_IO_HANDLE, sasl_client_io)
    MOCK_VOID_METHOD_END()

    MOCK_STATIC_METHOD_5(, int, saslclientio_open, CONCRETE_IO_HANDLE, sasl_client_io, ON_IO_OPEN_COMPLETE, on_io_open_complete, ON_BYTES_RECEIVED, on_bytes_received, ON_IO_ERROR, on_io_error, void*, callback_context)
    MOCK_METHOD_END(int, 0)

    MOCK_STATIC_METHOD_3(, int, saslclientio_close, CONCRETE_IO_HANDLE, sasl_client_io, ON_IO_CLOSE_COMPLETE, on_io_close_complete, void*, callback_context)
    MOCK_METHOD_END(int, 0)

    MOCK_STATIC_METHOD_5(, int, saslclientio_send, CONCRETE_IO_HANDLE, sasl_client_io, const void*, buffer, size_t, size, ON_SEND_COMPLETE, on_send_complete, void*, callback_context)
    MOCK_METHOD_END(int, 0)

    MOCK_STATIC_METHOD_1(, void, saslclientio_dowork, CONCRETE_IO_HANDLE, sasl_client_io)
    MOCK_VOID_METHOD_END()

    MOCK_STATIC_METHOD_0(, const IO_INTERFACE_DESCRIPTION*, saslclientio_get_interface_description)
    MOCK_METHOD_END(const IO_INTERFACE_DESCRIPTION*, 0)

    // sasl_mssbcbs.h
    MOCK_STATIC_METHOD_1(, CONCRETE_SASL_MECHANISM_HANDLE, saslmssbcbs_create, void*, config)
    MOCK_METHOD_END(CONCRETE_SASL_MECHANISM_HANDLE, 0)

    MOCK_STATIC_METHOD_1(, void, saslmssbcbs_destroy, CONCRETE_SASL_MECHANISM_HANDLE, sasl_mechanism_concrete_handle)
    MOCK_VOID_METHOD_END()

    MOCK_STATIC_METHOD_2(, int, saslmssbcbs_get_init_bytes, CONCRETE_SASL_MECHANISM_HANDLE, sasl_mechanism_concrete_handle, SASL_MECHANISM_BYTES*, init_bytes)
    MOCK_METHOD_END(int, 0)

    MOCK_STATIC_METHOD_1(, const char*, saslmssbcbs_get_mechanism_name, CONCRETE_SASL_MECHANISM_HANDLE, sasl_mechanism)
    MOCK_METHOD_END(const char*, 0)

    MOCK_STATIC_METHOD_3(, int, saslmssbcbs_challenge, CONCRETE_SASL_MECHANISM_HANDLE, concrete_sasl_mechanism, const SASL_MECHANISM_BYTES*, challenge_bytes, SASL_MECHANISM_BYTES*, response_bytes)
    MOCK_METHOD_END(int, 0)

    MOCK_STATIC_METHOD_0(, const SASL_MECHANISM_INTERFACE_DESCRIPTION*, saslmssbcbs_get_interface)
    MOCK_METHOD_END(const SASL_MECHANISM_INTERFACE_DESCRIPTION*, 0)

    // session.h
    MOCK_STATIC_METHOD_3(, SESSION_HANDLE, session_create, CONNECTION_HANDLE, connection, ON_LINK_ATTACHED, on_link_attached, void*, callback_context)
    MOCK_METHOD_END(SESSION_HANDLE, TEST_SESSION)

    MOCK_STATIC_METHOD_2(, int, session_set_incoming_window, SESSION_HANDLE, session, uint32_t, incoming_window)
    MOCK_METHOD_END(int, 0)

    MOCK_STATIC_METHOD_2(, int, session_get_incoming_window, SESSION_HANDLE, session, uint32_t*, incoming_window)
    MOCK_METHOD_END(int, 0)

    MOCK_STATIC_METHOD_2(, int, session_set_outgoing_window, SESSION_HANDLE, session, uint32_t, outgoing_window)
    MOCK_METHOD_END(int, 0)

    MOCK_STATIC_METHOD_2(, int, session_get_outgoing_window, SESSION_HANDLE, session, uint32_t*, outgoing_window)
    MOCK_METHOD_END(int, 0)

    MOCK_STATIC_METHOD_1(, void, session_destroy, SESSION_HANDLE, session)
    MOCK_VOID_METHOD_END()

    MOCK_STATIC_METHOD_3(, STRING_HANDLE, concat3Params, const char*, prefix, const char*, infix, const char*, suffix)
    MOCK_METHOD_END(STRING_HANDLE, 0)

    MOCK_STATIC_METHOD_2(, void, test_iothubclient_send_confirmation_callback, IOTHUB_CLIENT_CONFIRMATION_RESULT, _result, void*, userContextCallback)
    MOCK_VOID_METHOD_END()

    MOCK_STATIC_METHOD_2(, OPTIONHANDLER_RESULT, OptionHandler_FeedOptions, OPTIONHANDLER_HANDLE, handle, void*, destinationHandle)
    MOCK_METHOD_END(OPTIONHANDLER_RESULT, OPTIONHANDLER_OK)

    MOCK_STATIC_METHOD_1(, void, OptionHandler_Destroy, OPTIONHANDLER_HANDLE, handle)
        free(handle);
    MOCK_VOID_METHOD_END()

    MOCK_STATIC_METHOD_1(, OPTIONHANDLER_HANDLE, xio_retrieveoptions, XIO_HANDLE, xio)
        OPTIONHANDLER_HANDLE result2= (OPTIONHANDLER_HANDLE)malloc(1);
    MOCK_METHOD_END(OPTIONHANDLER_HANDLE, result2)


	// uamqp_messaging.h
	MOCK_STATIC_METHOD_2(, int, message_create_from_iothub_message, IOTHUB_MESSAGE_HANDLE, iothub_message, MESSAGE_HANDLE*, uamqp_message)
		*uamqp_message = TEST_MESSAGE_HANDLE;
	MOCK_METHOD_END(int, 0)

	MOCK_STATIC_METHOD_2(, int, IoTHubMessage_CreateFromUamqpMessage, MESSAGE_HANDLE, uamqp_message, IOTHUB_MESSAGE_HANDLE*, iothub_message)
		*iothub_message = TEST_IOTHUB_MESSAGE_HANDLE;
	MOCK_METHOD_END(int, 0)
};

// ** End Mocks **
DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubTransportAMQPMocks, , static STRING_HANDLE, concat3Params, const char*, prefix, const char*, infix, const char*, suffix);

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
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportAMQPMocks, , int, STRING_copy, STRING_HANDLE, s1, const char*, s2);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , STRING_HANDLE, STRING_clone, STRING_HANDLE, sHandle)

DECLARE_GLOBAL_MOCK_METHOD_0(CIoTHubTransportAMQPMocks, , BUFFER_HANDLE, BUFFER_new);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , void, BUFFER_delete, BUFFER_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , unsigned char*, BUFFER_u_char, BUFFER_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , size_t, BUFFER_length, BUFFER_HANDLE, handle);

DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubTransportAMQPMocks, , int, size_tToString, char*, destination, size_t, destinationSize, size_t, value);

DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , void*, gballoc_malloc, size_t, size);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportAMQPMocks, , void*, gballoc_realloc, void*, ptr, size_t, size);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , void, gballoc_free, void*, ptr);

DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportAMQPMocks, , IOTHUBMESSAGE_DISPOSITION_RESULT, IoTHubClient_LL_MessageCallback, IOTHUB_CLIENT_LL_HANDLE, handle, IOTHUB_MESSAGE_HANDLE, messageHandle);
DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubTransportAMQPMocks, , void, IoTHubClient_LL_SendComplete, IOTHUB_CLIENT_LL_HANDLE, handle, PDLIST_ENTRY, completedMessages, IOTHUB_CLIENT_CONFIRMATION_RESULT, batchResult);

DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , time_t, get_time, time_t*, t);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , struct tm*, get_gmtime, time_t*, t);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , time_t, get_mktime, struct tm*, t);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportAMQPMocks, , double, get_difftime, time_t, stop_time, time_t, start_time);
DECLARE_GLOBAL_MOCK_METHOD_4(CIoTHubTransportAMQPMocks, , STRING_HANDLE, SASToken_Create, STRING_HANDLE, key, STRING_HANDLE, scope, STRING_HANDLE, keyName, size_t, expiry);

DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportAMQPMocks, , int, mallocAndStrcpy_s, char**, destination, const char*, source);

// map.h
DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubTransportAMQPMocks, , MAP_RESULT, Map_AddOrUpdate, MAP_HANDLE, handle, const char*, key, const char*, value);
DECLARE_GLOBAL_MOCK_METHOD_4(CIoTHubTransportAMQPMocks, , MAP_RESULT, Map_GetInternals, MAP_HANDLE, handle, const char*const**, keys, const char*const**, values, size_t*, count);

// platform.h
DECLARE_GLOBAL_MOCK_METHOD_0(CIoTHubTransportAMQPMocks, , int, platform_init);
DECLARE_GLOBAL_MOCK_METHOD_0(CIoTHubTransportAMQPMocks, , void, platform_deinit);
DECLARE_GLOBAL_MOCK_METHOD_0(CIoTHubTransportAMQPMocks, , const IO_INTERFACE_DESCRIPTION*, platform_get_default_tlsio);


// xio.h
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportAMQPMocks, , XIO_HANDLE, xio_create, const IO_INTERFACE_DESCRIPTION*, io_interface_description, const void*, io_create_parameters);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , void, xio_destroy, XIO_HANDLE, xio);
DECLARE_GLOBAL_MOCK_METHOD_5(CIoTHubTransportAMQPMocks, , int, xio_open, XIO_HANDLE, xio, ON_IO_OPEN_COMPLETE, on_io_open_complete, ON_BYTES_RECEIVED, on_bytes_received, ON_IO_ERROR, on_io_error, void*, callback_context);
DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubTransportAMQPMocks, , int, xio_close, XIO_HANDLE, xio, ON_IO_CLOSE_COMPLETE, on_io_close_complete, void*, callback_context);
DECLARE_GLOBAL_MOCK_METHOD_5(CIoTHubTransportAMQPMocks, , int, xio_send, XIO_HANDLE, xio, const void*, buffer, size_t, size, ON_SEND_COMPLETE, on_send_complete, void*, callback_context);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , void, xio_dowork, XIO_HANDLE, xio);
DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubTransportAMQPMocks, , int, xio_setoption, XIO_HANDLE, xio, const char*, optionName, const void*, value);


// tlsio_openssl.h
DECLARE_GLOBAL_MOCK_METHOD_0(CIoTHubTransportAMQPMocks, , int, tlsio_openssl_init);
DECLARE_GLOBAL_MOCK_METHOD_0(CIoTHubTransportAMQPMocks, , void, tlsio_openssl_deinit);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , CONCRETE_IO_HANDLE, tlsio_openssl_create, void*, io_create_parameters);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , void, tlsio_openssl_destroy, CONCRETE_IO_HANDLE, tls_io);
DECLARE_GLOBAL_MOCK_METHOD_5(CIoTHubTransportAMQPMocks, , int, tlsio_openssl_open, CONCRETE_IO_HANDLE, tls_io, ON_IO_OPEN_COMPLETE, on_io_open_complete, ON_BYTES_RECEIVED, on_bytes_received, ON_IO_ERROR, on_io_error, void*, callback_context);
DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubTransportAMQPMocks, , int, tlsio_openssl_close, CONCRETE_IO_HANDLE, tls_io, ON_IO_CLOSE_COMPLETE, on_io_close_complete, void*, callback_context);
DECLARE_GLOBAL_MOCK_METHOD_5(CIoTHubTransportAMQPMocks, , int, tlsio_openssl_send, CONCRETE_IO_HANDLE, tls_io, const void*, buffer, size_t, size, ON_SEND_COMPLETE, on_send_complete, void*, callback_context);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , void, tlsio_openssl_dowork, CONCRETE_IO_HANDLE, tls_io);
DECLARE_GLOBAL_MOCK_METHOD_0(CIoTHubTransportAMQPMocks, , const IO_INTERFACE_DESCRIPTION*, tlsio_openssl_get_interface_description);

// tlsio_schannel.h
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , CONCRETE_IO_HANDLE, tlsio_schannel_create, void*, io_create_parameters);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , void, tlsio_schannel_destroy, CONCRETE_IO_HANDLE, tls_io);
DECLARE_GLOBAL_MOCK_METHOD_5(CIoTHubTransportAMQPMocks, , int, tlsio_schannel_open, CONCRETE_IO_HANDLE, tls_io, ON_IO_OPEN_COMPLETE, on_io_open_complete, ON_BYTES_RECEIVED, on_bytes_received, ON_IO_ERROR, on_io_error, void*, callback_context);
DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubTransportAMQPMocks, , int, tlsio_schannel_close, CONCRETE_IO_HANDLE, tls_io, ON_IO_CLOSE_COMPLETE, on_io_close_complete, void*, callback_context);
DECLARE_GLOBAL_MOCK_METHOD_5(CIoTHubTransportAMQPMocks, , int, tlsio_schannel_send, CONCRETE_IO_HANDLE, tls_io, const void*, buffer, size_t, size, ON_SEND_COMPLETE, on_send_complete, void*, callback_context);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , void, tlsio_schannel_dowork, CONCRETE_IO_HANDLE, tls_io);
DECLARE_GLOBAL_MOCK_METHOD_0(CIoTHubTransportAMQPMocks, , const IO_INTERFACE_DESCRIPTION*, tlsio_schannel_get_interface_description);

// AMQPctest
// amqp_definitions.h
DECLARE_GLOBAL_MOCK_METHOD_0(CIoTHubTransportAMQPMocks, , PROPERTIES_HANDLE, properties_create);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportAMQPMocks, , int, amqpvalue_get_map_pair_count, AMQP_VALUE, map, uint32_t*, pair_count);
DECLARE_GLOBAL_MOCK_METHOD_4(CIoTHubTransportAMQPMocks, , int, amqpvalue_get_map_key_value_pair, AMQP_VALUE, map, uint32_t, index, AMQP_VALUE*, key, AMQP_VALUE*, value);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , AMQP_VALUE, amqpvalue_get_inplace_described_value, AMQP_VALUE, value);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportAMQPMocks, , int, properties_get_message_id, PROPERTIES_HANDLE, properties, AMQP_VALUE*, message_id_value);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportAMQPMocks, , int, properties_set_message_id, PROPERTIES_HANDLE, properties, AMQP_VALUE, message_id_value);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportAMQPMocks, , int, properties_get_correlation_id, PROPERTIES_HANDLE, properties, AMQP_VALUE*, correlation_id_value);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportAMQPMocks, , int, properties_set_correlation_id, PROPERTIES_HANDLE, properties, AMQP_VALUE, correlation_id_value);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , void, properties_destroy, PROPERTIES_HANDLE, properties);

// amqpvalue.h
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , void, amqpvalue_destroy, AMQP_VALUE, value);
DECLARE_GLOBAL_MOCK_METHOD_0(CIoTHubTransportAMQPMocks, , AMQP_VALUE, amqpvalue_create_map);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , AMQP_VALUE, amqpvalue_create_symbol, const char*, value);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , AMQP_VALUE, amqpvalue_create_string, const char*, value);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportAMQPMocks, , int, amqpvalue_get_string, AMQP_VALUE, value, const char**, string_value);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , AMQP_TYPE, amqpvalue_get_type, AMQP_VALUE, value);
DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubTransportAMQPMocks, , int, amqpvalue_set_map_value, AMQP_VALUE, map, AMQP_VALUE, key, AMQP_VALUE, value);

// cbs.h
DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubTransportAMQPMocks, , CBS_HANDLE, cbs_create, SESSION_HANDLE, session, ON_AMQP_MANAGEMENT_STATE_CHANGED, on_amqp_management_state_changed, void*, callback_context);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , void, cbs_destroy, CBS_HANDLE, cbs);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , int, cbs_open, CBS_HANDLE, amqp_management);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , int, cbs_close, CBS_HANDLE, amqp_management);
DECLARE_GLOBAL_MOCK_METHOD_6(CIoTHubTransportAMQPMocks, , int, cbs_put_token, CBS_HANDLE, cbs, const char*, type, const char*, audience, const char*, token, ON_CBS_OPERATION_COMPLETE, on_cbs_operation_complete, void*, context);
DECLARE_GLOBAL_MOCK_METHOD_5(CIoTHubTransportAMQPMocks, , int, cbs_delete_token, CBS_HANDLE, cbs, const char*, type, const char*, audience, ON_CBS_OPERATION_COMPLETE, on_cbs_operation_complete, void*, context);

// connection.h
DECLARE_GLOBAL_MOCK_METHOD_5(CIoTHubTransportAMQPMocks, , CONNECTION_HANDLE, connection_create, XIO_HANDLE, io, const char*, hostname, const char*, container_id, ON_NEW_ENDPOINT, on_new_endpoint, void*, callback_context);
DECLARE_GLOBAL_MOCK_METHOD_9(CIoTHubTransportAMQPMocks, , CONNECTION_HANDLE, connection_create2, XIO_HANDLE, io, const char*, hostname, const char*, container_id, ON_NEW_ENDPOINT, on_new_endpoint, void*, callback_context, ON_CONNECTION_STATE_CHANGED, on_connection_state_changed, void*, on_connection_state_changed_context, ON_IO_ERROR, on_io_error, void*, on_io_error_context);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , void, connection_dowork, CONNECTION_HANDLE, connection);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , void, connection_destroy, CONNECTION_HANDLE, connection);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportAMQPMocks, , void, connection_set_trace, CONNECTION_HANDLE, connection, bool, traceOn);

// link.h
DECLARE_GLOBAL_MOCK_METHOD_5(CIoTHubTransportAMQPMocks, , LINK_HANDLE, link_create, SESSION_HANDLE, session, const char*, name, role, _role, AMQP_VALUE, source, AMQP_VALUE, target);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportAMQPMocks, , int, link_set_max_message_size, LINK_HANDLE, link, uint64_t, max_message_size);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportAMQPMocks, , int, link_set_rcv_settle_mode, LINK_HANDLE, link, receiver_settle_mode, rcv_settle_mode);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , void, link_destroy, LINK_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportAMQPMocks, , int, link_set_attach_properties, LINK_HANDLE, link, fields, attach_properties);


// message.h
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportAMQPMocks, , int, message_add_body_amqp_data, MESSAGE_HANDLE, message, BINARY_DATA, binary_data);
DECLARE_GLOBAL_MOCK_METHOD_0(CIoTHubTransportAMQPMocks, , MESSAGE_HANDLE, message_create);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , void, message_destroy, MESSAGE_HANDLE, message);
DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubTransportAMQPMocks, , int, message_get_body_amqp_data, MESSAGE_HANDLE, message, size_t, index, BINARY_DATA*, binary_data);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportAMQPMocks, , int, message_get_body_type, MESSAGE_HANDLE, message, MESSAGE_BODY_TYPE*, body_type);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportAMQPMocks, , int, message_get_properties, MESSAGE_HANDLE, message, PROPERTIES_HANDLE*, properties);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportAMQPMocks, , int, message_set_properties, MESSAGE_HANDLE, message, PROPERTIES_HANDLE, properties);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportAMQPMocks, , int, message_get_application_properties, MESSAGE_HANDLE, message, AMQP_VALUE*, application_properties);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportAMQPMocks, , int, message_set_application_properties, MESSAGE_HANDLE, message, AMQP_VALUE, application_properties);

// message_receiver.h
DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubTransportAMQPMocks, , MESSAGE_RECEIVER_HANDLE, messagereceiver_create, LINK_HANDLE, link, ON_MESSAGE_RECEIVER_STATE_CHANGED, on_message_receiver_state_changed, void*, context);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , void, messagereceiver_destroy, MESSAGE_RECEIVER_HANDLE, message_receiver);
DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubTransportAMQPMocks, , int, messagereceiver_open, MESSAGE_RECEIVER_HANDLE, message_receiver, ON_MESSAGE_RECEIVED, on_message_received, const void*, callback_context);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , int, messagereceiver_close, MESSAGE_RECEIVER_HANDLE, message_receiver);

// message_sender.h
DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubTransportAMQPMocks, , MESSAGE_SENDER_HANDLE, messagesender_create, LINK_HANDLE, link, ON_MESSAGE_SENDER_STATE_CHANGED, on_message_sender_state_changed, void*, context);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , void, messagesender_destroy, MESSAGE_SENDER_HANDLE, message_sender);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , int, messagesender_open, MESSAGE_SENDER_HANDLE, message_sender);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , int, messagesender_close, MESSAGE_SENDER_HANDLE, message_sender);
DECLARE_GLOBAL_MOCK_METHOD_4(CIoTHubTransportAMQPMocks, , int, messagesender_send, MESSAGE_SENDER_HANDLE, message_sender, MESSAGE_HANDLE, message, ON_MESSAGE_SEND_COMPLETE, on_message_send_complete, void*, callback_context);

// messaging.h
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , AMQP_VALUE, messaging_create_source, const char*, address);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , AMQP_VALUE, messaging_create_target, const char*, address);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportAMQPMocks, , AMQP_VALUE, messaging_delivery_received, uint32_t, section_number, uint64_t, section_offset);
DECLARE_GLOBAL_MOCK_METHOD_0(CIoTHubTransportAMQPMocks, , AMQP_VALUE, messaging_delivery_accepted);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportAMQPMocks, , AMQP_VALUE, messaging_delivery_rejected, const char*, error_condition, const char*, error_description);
DECLARE_GLOBAL_MOCK_METHOD_0(CIoTHubTransportAMQPMocks, , AMQP_VALUE, messaging_delivery_released);
DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubTransportAMQPMocks, , AMQP_VALUE, messaging_delivery_modified, bool, delivery_failed, bool, undeliverable_here, fields, message_annotations);

// sasl_mechanism.h
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportAMQPMocks, , SASL_MECHANISM_HANDLE, saslmechanism_create, const SASL_MECHANISM_INTERFACE_DESCRIPTION*, sasl_mechanism_interface_description, void*, sasl_mechanism_create_parameters);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , void, saslmechanism_destroy, SASL_MECHANISM_HANDLE, sasl_mechanism);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportAMQPMocks, , int, saslmechanism_get_init_bytes, SASL_MECHANISM_HANDLE, sasl_mechanism, SASL_MECHANISM_BYTES*, init_bytes);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , const char*, saslmechanism_get_mechanism_name, SASL_MECHANISM_HANDLE, sasl_mechanism);
DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubTransportAMQPMocks, , int, saslmechanism_challenge, SASL_MECHANISM_HANDLE, sasl_mechanism, const SASL_MECHANISM_BYTES*, challenge_bytes, SASL_MECHANISM_BYTES*, response_bytes);

// saslclientio.h
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , CONCRETE_IO_HANDLE, saslclientio_create, void*, io_create_parameters);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , void, saslclientio_destroy, CONCRETE_IO_HANDLE, sasl_client_io);
DECLARE_GLOBAL_MOCK_METHOD_5(CIoTHubTransportAMQPMocks, , int, saslclientio_open, CONCRETE_IO_HANDLE, sasl_client_io, ON_IO_OPEN_COMPLETE, on_io_open_complete, ON_BYTES_RECEIVED, on_bytes_received, ON_IO_ERROR, on_io_error, void*, callback_context);
DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubTransportAMQPMocks, , int, saslclientio_close, CONCRETE_IO_HANDLE, sasl_client_io, ON_IO_CLOSE_COMPLETE, on_io_close_complete, void*, callback_context);
DECLARE_GLOBAL_MOCK_METHOD_5(CIoTHubTransportAMQPMocks, , int, saslclientio_send, CONCRETE_IO_HANDLE, sasl_client_io, const void*, buffer, size_t, size, ON_SEND_COMPLETE, on_send_complete, void*, callback_context);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , void, saslclientio_dowork, CONCRETE_IO_HANDLE, sasl_client_io);
DECLARE_GLOBAL_MOCK_METHOD_0(CIoTHubTransportAMQPMocks, , const IO_INTERFACE_DESCRIPTION*, saslclientio_get_interface_description);

// sasl_mssbcbs.h
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , CONCRETE_SASL_MECHANISM_HANDLE, saslmssbcbs_create, void*, config);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , void, saslmssbcbs_destroy, CONCRETE_SASL_MECHANISM_HANDLE, sasl_mechanism_concrete_handle);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportAMQPMocks, , int, saslmssbcbs_get_init_bytes, CONCRETE_SASL_MECHANISM_HANDLE, sasl_mechanism_concrete_handle, SASL_MECHANISM_BYTES*, init_bytes);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , const char*, saslmssbcbs_get_mechanism_name, CONCRETE_SASL_MECHANISM_HANDLE, sasl_mechanism);
DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubTransportAMQPMocks, , int, saslmssbcbs_challenge, CONCRETE_SASL_MECHANISM_HANDLE, concrete_sasl_mechanism, const SASL_MECHANISM_BYTES*, challenge_bytes, SASL_MECHANISM_BYTES*, response_bytes);
DECLARE_GLOBAL_MOCK_METHOD_0(CIoTHubTransportAMQPMocks, , const SASL_MECHANISM_INTERFACE_DESCRIPTION*, saslmssbcbs_get_interface);

// session.h
DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubTransportAMQPMocks, , SESSION_HANDLE, session_create, CONNECTION_HANDLE, connection, ON_LINK_ATTACHED, on_link_attached, void*, callback_context);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportAMQPMocks, , int, session_set_incoming_window, SESSION_HANDLE, session, uint32_t, incoming_window);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportAMQPMocks, , int, session_get_incoming_window, SESSION_HANDLE, session, uint32_t*, incoming_window);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportAMQPMocks, , int, session_set_outgoing_window, SESSION_HANDLE, session, uint32_t, outgoing_window);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportAMQPMocks, , int, session_get_outgoing_window, SESSION_HANDLE, session, uint32_t*, outgoing_window);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , void, session_destroy, SESSION_HANDLE, session);

DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportAMQPMocks, , void, test_iothubclient_send_confirmation_callback, IOTHUB_CLIENT_CONFIRMATION_RESULT, _result, void*, userContextCallback);

DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportAMQPMocks, , OPTIONHANDLER_RESULT, OptionHandler_FeedOptions, OPTIONHANDLER_HANDLE, handle, void*, destinationHandle);

DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , void, OptionHandler_Destroy, OPTIONHANDLER_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportAMQPMocks, , OPTIONHANDLER_HANDLE, xio_retrieveoptions, XIO_HANDLE, xio);

// uamqp_messaging.h
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportAMQPMocks, , int, message_create_from_iothub_message, IOTHUB_MESSAGE_HANDLE, iothub_message, MESSAGE_HANDLE*, uamqp_message);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportAMQPMocks, , int, IoTHubMessage_CreateFromUamqpMessage, MESSAGE_HANDLE, uamqp_message, IOTHUB_MESSAGE_HANDLE*, iothub_message);

// Auxiliary Functions

#define STEP_CREATE_LIST_INIT 0
#define STEP_CREATE_IOTHUB_FQDN 1
#define STEP_CREATE_DEVICES_PATH 2
#define STEP_CREATE_TARGET_ADDRESS 3
#define STEP_CREATE_RECEIVE_ADDRESS 4
#define STEP_CREATE_SASTOKEN_KEYNAME 5
#define STEP_CREATE_DEVICEKEY 6

#define STEP_DOWORK_GET_TLS_IO 0
#define STEP_DOWORK_CREATE_SASLMECHANISM 1
#define STEP_DOWORK_SASLIO_GET_INTERFACE 2
#define STEP_DOWORK_CREATE_SASLIO 3
#define STEP_DOWORK_CREATE_CONNECTION 4
#define STEP_DOWORK_CREATE_SESSION 5
#define STEP_DOWORK_INCOMING_WINDOW 6
#define STEP_DOWORK_OUTGOING_WINDOW 7
#define STEP_DOWORK_CREATE_CBS 8
#define STEP_DOWORK_OPEN_CBS 9
#define STEP_DOWORK_AUTHENTICATION 10

#define STEP_DOWORK_MESSAGERECEIVER 11
#define STEP_DOWORK_MESSAGESENDER 12

#define DOWORK_MESSAGERECEIVER_NONE 0
#define DOWORK_MESSAGERECEIVER_CREATE 1
#define DOWORK_MESSAGERECEIVER_DESTROY 2

/* This cleanup list is needed so that we can cleanup the waiting to send list so that the tests do not leak.
This is needed due to the fact that we populate this list for tests, rather than recording/performing calls if we would have a regular function interface */
static void cleanupList(PDLIST_ENTRY list)
{
    while (!DList_IsListEmpty(list))
    {
        PDLIST_ENTRY next_entry = list->Flink;
        DList_RemoveEntryList(next_entry);
        gballoc_free(containingRecord(next_entry, IOTHUB_MESSAGE_LIST, entry));
    }
}

static void addTestEvents(PDLIST_ENTRY waitingToSend, int numberOfEvents, bool setCallback)
{
    while (numberOfEvents-- > 0)
    {
        IOTHUB_MESSAGE_LIST* iml = (IOTHUB_MESSAGE_LIST*)gballoc_malloc(sizeof(IOTHUB_MESSAGE_LIST));

        if (iml == NULL)
        {
            LogError("Test failure: addTestEvents() failed allocating IOTHUB_MESSAGE_LIST (%i).", numberOfEvents);
        }
        else
        {
            iml->messageHandle = TEST_IOTHUB_MESSAGE_HANDLE;

            if (setCallback)
            {
                iml->context = (void*)((intptr_t)numberOfEvents);
                iml->callback = test_iothubclient_send_confirmation_callback;
            }

            DList_InsertTailList(waitingToSend, &(iml->entry));
        }
    }
}

static void setExpectedCallsForTransportCreateUpTo(CIoTHubTransportAMQPMocks& mocks, IOTHUBTRANSPORT_CONFIG* config, int maximumStepToSet)
{
    int step;
    (void)mocks;
    for (step = 0; step <= maximumStepToSet; step++)
    {
        if (step == STEP_CREATE_LIST_INIT)
        {
            STRICT_EXPECTED_CALL(mocks, gballoc_malloc(0)).IgnoreArgument(1);
            STRICT_EXPECTED_CALL(mocks, DList_InitializeListHead(0)).IgnoreAllArguments();
        }
        else if (step == STEP_CREATE_IOTHUB_FQDN)
        {
            if (config->upperConfig->protocolGatewayHostName == NULL)
            {
                STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(config->upperConfig->iotHubName) + strlen(config->upperConfig->iotHubSuffix) + 2));
                EXPECTED_CALL(mocks, STRING_construct(0));
                EXPECTED_CALL(mocks, gballoc_free(0));
            }
            else
            {
                STRICT_EXPECTED_CALL(mocks, STRING_construct(config->upperConfig->protocolGatewayHostName));
            }
        }
        else if (step == STEP_CREATE_DEVICES_PATH)
        {
            EXPECTED_CALL(mocks, STRING_c_str(0));
            EXPECTED_CALL(mocks, gballoc_malloc(0));
            EXPECTED_CALL(mocks, STRING_construct(0));
            EXPECTED_CALL(mocks, gballoc_free(0));
        }
        else if (step == STEP_CREATE_TARGET_ADDRESS)
        {
            EXPECTED_CALL(mocks, STRING_c_str(0));
            EXPECTED_CALL(mocks, gballoc_malloc(0));
            EXPECTED_CALL(mocks, STRING_construct(0));
            EXPECTED_CALL(mocks, gballoc_free(0));
        }
        else if (step == STEP_CREATE_RECEIVE_ADDRESS)
        {
            EXPECTED_CALL(mocks, STRING_c_str(0));
            EXPECTED_CALL(mocks, gballoc_malloc(0));
            EXPECTED_CALL(mocks, STRING_construct(0));
            EXPECTED_CALL(mocks, gballoc_free(0));
        }
        else if (step == STEP_CREATE_SASTOKEN_KEYNAME)
        {
            STRICT_EXPECTED_CALL(mocks, STRING_new());
        }
        else if (step == STEP_CREATE_DEVICEKEY)
        {
            STRICT_EXPECTED_CALL(mocks, STRING_construct(config->upperConfig->deviceKey)).IgnoreArgument(1);
        }
    }
}

static void setExpectedCleanupCallsForTransportCreateUpTo(CIoTHubTransportAMQPMocks& mocks, int maximumStepToCleanup)
{
    int step;
    (void)mocks;
    for (step = maximumStepToCleanup; step >= 0; step--)
    {
        if (step == STEP_CREATE_LIST_INIT)
        {
            EXPECTED_CALL(mocks, gballoc_free(0));
        }
        else if (step == STEP_CREATE_IOTHUB_FQDN)
        {
            EXPECTED_CALL(mocks, STRING_delete(0));
        }
        else if (step == STEP_CREATE_DEVICES_PATH)
        {
            EXPECTED_CALL(mocks, STRING_delete(0));
        }
        else if (step == STEP_CREATE_TARGET_ADDRESS)
        {
            EXPECTED_CALL(mocks, STRING_delete(0));
        }
        else if (step == STEP_CREATE_RECEIVE_ADDRESS)
        {
            EXPECTED_CALL(mocks, STRING_delete(0));
        }
        else if (step == STEP_CREATE_SASTOKEN_KEYNAME)
        {
            EXPECTED_CALL(mocks, STRING_delete(0));
        }
        else if (step == STEP_CREATE_DEVICEKEY)
        {
            EXPECTED_CALL(mocks, STRING_delete(0));
        }
    }
}

static void setExpectedCallsForGetSecondsSinceEpoch(CIoTHubTransportAMQPMocks& mocks, time_t current_time)
{
	(void)mocks;
	STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(current_time);
	EXPECTED_CALL(mocks, get_difftime((time_t)0, (time_t)0));
}

static void setExpectedCallsForSASTokenExpiryCheck(CIoTHubTransportAMQPMocks& mocks, time_t current_time)
{
    (void)mocks;
	setExpectedCallsForGetSecondsSinceEpoch(mocks, current_time);
}

static void setExpectedCallsForCbsAuthentication(CIoTHubTransportAMQPMocks& mocks, time_t current_time)
{
    (void)mocks;
	setExpectedCallsForGetSecondsSinceEpoch(mocks, current_time);
    EXPECTED_CALL(mocks, SASToken_Create(NULL, NULL, NULL, 0));
    EXPECTED_CALL(mocks, STRING_c_str(NULL));
    EXPECTED_CALL(mocks, STRING_c_str(NULL));
    EXPECTED_CALL(mocks, cbs_put_token(NULL, NULL, NULL, NULL, NULL, NULL));
    EXPECTED_CALL(mocks, STRING_delete(NULL));
}

static void setExpectedCallsForCbsAuthTimeoutCheck(CIoTHubTransportAMQPMocks& mocks, time_t current_time)
{
    (void)mocks;
	setExpectedCallsForGetSecondsSinceEpoch(mocks, current_time);
}

static void setExpectedCallsForCreateMessageReceiver(CIoTHubTransportAMQPMocks& mocks)
{
    (void)mocks;
    EXPECTED_CALL(mocks, STRING_c_str(NULL));
    EXPECTED_CALL(mocks, messaging_create_source(NULL)).SetReturn(TEST_MESSAGERECEIVER_SOURCE);
    EXPECTED_CALL(mocks, messaging_create_target(NULL)).SetReturn(TEST_MESSAGERECEIVER_TARGET);
    EXPECTED_CALL(mocks, link_create(NULL, NULL, NULL, NULL, NULL)).SetReturn(TEST_MESSAGERECEIVER_LINK);
    STRICT_EXPECTED_CALL(mocks, link_set_rcv_settle_mode(NULL, receiver_settle_mode_first)).IgnoreArgument(1);
    EXPECTED_CALL(mocks, link_set_max_message_size(NULL, TEST_MESSAGE_RECEIVER_MAX_LINK_SIZE)).IgnoreArgument(1);
    EXPECTED_CALL(mocks, amqpvalue_create_map()).SetReturn(TEST_AMQP_VALUE_TEST_HANDLE);
    EXPECTED_CALL(mocks, amqpvalue_create_symbol(NULL)).IgnoreArgument(1).SetReturn(TEST_AMQP_VALUE_TEST_HANDLE);
    EXPECTED_CALL(mocks, amqpvalue_create_string(NULL)).IgnoreArgument(1).SetReturn(TEST_AMQP_VALUE_TEST_HANDLE);
    EXPECTED_CALL(mocks, amqpvalue_set_map_value(NULL, NULL, NULL)).IgnoreAllArguments();
    EXPECTED_CALL(mocks, link_set_attach_properties(NULL, NULL)).IgnoreAllArguments().SetReturn(0);
    EXPECTED_CALL(mocks, amqpvalue_destroy(NULL)).IgnoreArgument(1);
    EXPECTED_CALL(mocks, amqpvalue_destroy(NULL)).IgnoreArgument(1);
    EXPECTED_CALL(mocks, amqpvalue_destroy(NULL)).IgnoreArgument(1);
    EXPECTED_CALL(mocks, messagereceiver_create(NULL, NULL, NULL)).SetReturn(TEST_MESSAGE_RECEIVER);
    EXPECTED_CALL(mocks, messagereceiver_open(NULL, NULL, NULL));
    STRICT_EXPECTED_CALL(mocks, amqpvalue_destroy(TEST_MESSAGERECEIVER_SOURCE));
    STRICT_EXPECTED_CALL(mocks, amqpvalue_destroy(TEST_MESSAGERECEIVER_TARGET));
}

static void setExpectedCallsForDestroyMessageReceiver(CIoTHubTransportAMQPMocks& mocks)
{
    (void)mocks;
    STRICT_EXPECTED_CALL(mocks, messagereceiver_close(TEST_MESSAGE_RECEIVER));
    STRICT_EXPECTED_CALL(mocks, messagereceiver_destroy(TEST_MESSAGE_RECEIVER));
    STRICT_EXPECTED_CALL(mocks, link_destroy(TEST_MESSAGERECEIVER_LINK));
}

static void setExpectedCallsForCreateEventSender(CIoTHubTransportAMQPMocks& mocks)
{
    (void)mocks;
    STRICT_EXPECTED_CALL(mocks, messaging_create_source(TEST_MESSAGE_SENDER_SOURCE_ADDRESS)).SetReturn(TEST_MESSAGESENDER_SOURCE);
    EXPECTED_CALL(mocks, STRING_c_str(NULL));
    EXPECTED_CALL(mocks, messaging_create_target(NULL)).SetReturn(TEST_MESSAGESENDER_TARGET);
    EXPECTED_CALL(mocks, link_create(NULL, NULL, NULL, NULL, NULL)).SetReturn(TEST_MESSAGESENDER_LINK);
    STRICT_EXPECTED_CALL(mocks, link_set_max_message_size(NULL, TEST_MESSAGE_SENDER_MAX_LINK_SIZE)).IgnoreArgument(1);
    EXPECTED_CALL(mocks, amqpvalue_create_map()).SetReturn(TEST_AMQP_VALUE_TEST_HANDLE);
    EXPECTED_CALL(mocks, amqpvalue_create_symbol(NULL)).IgnoreArgument(1).SetReturn(TEST_AMQP_VALUE_TEST_HANDLE);
    EXPECTED_CALL(mocks, amqpvalue_create_string(NULL)).IgnoreArgument(1).SetReturn(TEST_AMQP_VALUE_TEST_HANDLE);
    EXPECTED_CALL(mocks, amqpvalue_set_map_value(NULL, NULL, NULL)).IgnoreAllArguments();
    EXPECTED_CALL(mocks, link_set_attach_properties(NULL, NULL)).IgnoreAllArguments().SetReturn(0);
    EXPECTED_CALL(mocks, amqpvalue_destroy(NULL)).IgnoreArgument(1);
    EXPECTED_CALL(mocks, amqpvalue_destroy(NULL)).IgnoreArgument(1);
    EXPECTED_CALL(mocks, amqpvalue_destroy(NULL)).IgnoreArgument(1);
    EXPECTED_CALL(mocks, messagesender_create(NULL, NULL, NULL)).SetReturn(TEST_MESSAGE_SENDER);
    EXPECTED_CALL(mocks, messagesender_open(TEST_MESSAGE_SENDER));
    EXPECTED_CALL(mocks, amqpvalue_destroy(TEST_MESSAGESENDER_SOURCE));
    EXPECTED_CALL(mocks, amqpvalue_destroy(TEST_MESSAGESENDER_TARGET));
}

static void setExpectedCallsForDestroyEventSender(CIoTHubTransportAMQPMocks& mocks)
{
    (void)mocks;
    STRICT_EXPECTED_CALL(mocks, messagesender_destroy(TEST_MESSAGE_SENDER));
    STRICT_EXPECTED_CALL(mocks, link_destroy(TEST_MESSAGESENDER_LINK));
}

static void setAddPropertiesTouAMQPMessage(CIoTHubTransportAMQPMocks& mocks)
{
    (void)mocks;
    /*addPropertiesTouAMQPMessage*/
    STRICT_EXPECTED_CALL(mocks, message_get_properties(TEST_MESSAGE_HANDLE, IGNORED_PTR_ARG)).IgnoreArgument(2).SetReturn(0);
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetMessageId(TEST_IOTHUB_MESSAGE_HANDLE));
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_GetCorrelationId(TEST_IOTHUB_MESSAGE_HANDLE));
    EXPECTED_CALL(mocks, message_set_properties(TEST_MESSAGE_HANDLE, TEST_UAMQP_PROPERTIES));
    STRICT_EXPECTED_CALL(mocks, properties_destroy(IGNORED_PTR_ARG)).IgnoreArgument(1);
}

static void setExpectedCallsForSendPendingEvents_SingleEvent(CIoTHubTransportAMQPMocks& mocks, IOTHUBMESSAGE_CONTENT_TYPE message_type)
{
    (void)mocks;
	(void)message_type;

    EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).SetReturn(0);

    EXPECTED_CALL(mocks, DList_RemoveEntryList(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, DList_InsertTailList(IGNORED_PTR_ARG, IGNORED_PTR_ARG));

	STRICT_EXPECTED_CALL(mocks, message_create_from_iothub_message(TEST_IOTHUB_MESSAGE_HANDLE, IGNORED_PTR_ARG)).IgnoreArgument(2).SetReturn(0);

    EXPECTED_CALL(mocks, messagesender_send(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, message_destroy(TEST_MESSAGE_HANDLE));
}

static void setExpectedCallsForSendPendingEvents(CIoTHubTransportAMQPMocks& mocks, IOTHUBMESSAGE_CONTENT_TYPE message_type, int numberOfEvents)
{
    (void)mocks;
    while (numberOfEvents-- > 0)
    {
        setExpectedCallsForSendPendingEvents_SingleEvent(mocks, message_type);
    }

    EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).SetReturn(1);
}

static void setExpectedCallsForConnectionDoWork(CIoTHubTransportAMQPMocks& mocks)
{
    (void)mocks;
    EXPECTED_CALL(mocks, connection_dowork(NULL));
}

static void setExpectedCallsForTransportDestroy(CIoTHubTransportAMQPMocks& mocks, bool isConnectionInitialized, bool destroyIOtransport, size_t numberOfEventsInProgress)
{
    (void)mocks;
    if (isConnectionInitialized)
    {
        EXPECTED_CALL(mocks, messagesender_destroy(0));
        EXPECTED_CALL(mocks, messagereceiver_destroy(0));
        EXPECTED_CALL(mocks, cbs_destroy(0));
        EXPECTED_CALL(mocks, session_destroy(0));
        EXPECTED_CALL(mocks, connection_destroy(0));
        STRICT_EXPECTED_CALL(mocks, xio_retrieveoptions(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        EXPECTED_CALL(mocks, xio_destroy(0));
        EXPECTED_CALL(mocks, saslmechanism_destroy(0));
    }

    if (destroyIOtransport)
    {
        STRICT_EXPECTED_CALL(mocks, xio_retrieveoptions(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        EXPECTED_CALL(mocks, xio_destroy(0));
    }

    EXPECTED_CALL(mocks, STRING_delete(0));
    EXPECTED_CALL(mocks, STRING_delete(0));
    EXPECTED_CALL(mocks, STRING_delete(0));
    EXPECTED_CALL(mocks, STRING_delete(0));
    EXPECTED_CALL(mocks, STRING_delete(0));
    EXPECTED_CALL(mocks, STRING_delete(0));

    while (numberOfEventsInProgress-- > 0)
    {
        EXPECTED_CALL(mocks, DList_RemoveEntryList(0));
        EXPECTED_CALL(mocks, DList_InitializeListHead(0));
        EXPECTED_CALL(mocks, DList_InsertTailList(0, 0));
    }

    EXPECTED_CALL(mocks, gballoc_free(NULL));
}

static void setExpectedCallsForTransportDoWorkUpTo(CIoTHubTransportAMQPMocks& mocks, int maximumStepToSet, int messageReceiverCreation, time_t current_time)
{
    int step;
    (void)mocks;
    for (step = 0; step <= maximumStepToSet; step++)
    {
        if (step == STEP_DOWORK_GET_TLS_IO)
        {
            EXPECTED_CALL(mocks, STRING_c_str(NULL));
            EXPECTED_CALL(mocks, platform_get_default_tlsio()).SetReturn(TEST_TLS_IO_INTERFACE_DESC);
            EXPECTED_CALL(mocks, xio_create(NULL, NULL)).SetReturn(TEST_TLS_IO_INTERFACE);
        }
        else if (step == STEP_DOWORK_CREATE_SASLMECHANISM)
        {
            STRICT_EXPECTED_CALL(mocks, saslmssbcbs_get_interface());
            EXPECTED_CALL(mocks, saslmechanism_create(NULL, NULL));
        }
        else if (step == STEP_DOWORK_SASLIO_GET_INTERFACE)
        {
            STRICT_EXPECTED_CALL(mocks, saslclientio_get_interface_description());
        }
        else if (step == STEP_DOWORK_CREATE_SASLIO)
        {
            EXPECTED_CALL(mocks, xio_create(NULL, NULL)).SetReturn(TEST_SASL_IO);
        }
        else if (step == STEP_DOWORK_CREATE_CONNECTION)
        {
            EXPECTED_CALL(mocks, STRING_c_str(NULL));
            EXPECTED_CALL(mocks, connection_create2(NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL));
        }
        else if (step == STEP_DOWORK_CREATE_SESSION)
        {
            EXPECTED_CALL(mocks, session_create(NULL, NULL, NULL));
        }
        else if (step == STEP_DOWORK_INCOMING_WINDOW)
        {
            STRICT_EXPECTED_CALL(mocks, session_set_incoming_window(NULL, TEST_INCOMING_WINDOW_SIZE)).IgnoreArgument(1);
        }
        else if (step == STEP_DOWORK_OUTGOING_WINDOW)
        {
            STRICT_EXPECTED_CALL(mocks, session_set_outgoing_window(NULL, TEST_OUTGOING_WINDOW_SIZE)).IgnoreArgument(1);
        }
        else if (step == STEP_DOWORK_CREATE_CBS)
        {
            EXPECTED_CALL(mocks, cbs_create(NULL, NULL, NULL));
        }
        else if (step == STEP_DOWORK_OPEN_CBS)
        {
            EXPECTED_CALL(mocks, cbs_open(NULL));
			setExpectedCallsForGetSecondsSinceEpoch(mocks, current_time);

			EXPECTED_CALL(mocks, connection_set_trace(IGNORED_PTR_ARG, false));
			EXPECTED_CALL(mocks, xio_setoption(IGNORED_PTR_ARG, OPTION_LOG_TRACE, IGNORED_PTR_ARG));
		}
        else if (step == STEP_DOWORK_AUTHENTICATION)
        {
            setExpectedCallsForCbsAuthentication(mocks, current_time);
        }
        else if (step == STEP_DOWORK_MESSAGERECEIVER)
        {
            if (messageReceiverCreation == DOWORK_MESSAGERECEIVER_CREATE)
            {
                setExpectedCallsForCreateMessageReceiver(mocks);
            }
            else if (messageReceiverCreation == DOWORK_MESSAGERECEIVER_DESTROY)
            {
                setExpectedCallsForDestroyMessageReceiver(mocks);
            }
        }
        else if (step == STEP_DOWORK_MESSAGESENDER)
        {
            setExpectedCallsForCreateEventSender(mocks);
        }
    }
}

static void setExpectedCallsForConnectionDestroyUpTo(CIoTHubTransportAMQPMocks& mocks, int maximumStepToSet)
{
    int step;
    (void)mocks;
    for (step = maximumStepToSet; step >= 0; step--)
    {
        if (step == STEP_DOWORK_CREATE_CBS)
        {
            EXPECTED_CALL(mocks, cbs_destroy(NULL));
        }
        else if (step == STEP_DOWORK_CREATE_SESSION)
        {
            EXPECTED_CALL(mocks, session_destroy(NULL));
        }
        else if (step == STEP_DOWORK_CREATE_CONNECTION)
        {
            EXPECTED_CALL(mocks, connection_destroy(NULL));
        }
        else if (step == STEP_DOWORK_CREATE_SASLIO)
        {
            EXPECTED_CALL(mocks, xio_destroy(NULL));
        }
        else if (step == STEP_DOWORK_CREATE_SASLMECHANISM)
        {
            EXPECTED_CALL(mocks, saslmechanism_destroy(NULL));
        }
        else if (step == STEP_DOWORK_GET_TLS_IO)
        {
            STRICT_EXPECTED_CALL(mocks, xio_retrieveoptions(IGNORED_PTR_ARG))
                .IgnoreArgument(1);
            EXPECTED_CALL(mocks, xio_destroy(NULL));
        }
    }
}

static void setExpectedCallsForPrepareForConnectionRetry(CIoTHubTransportAMQPMocks& mocks, bool wasMessageReceiverCreated, bool wasEventSenderCreated)
{
    if (wasMessageReceiverCreated)
    {
        setExpectedCallsForDestroyMessageReceiver(mocks);
    }

    if (wasEventSenderCreated)
    {
        setExpectedCallsForDestroyEventSender(mocks);
    }
}

// This is for a call to DoWork after the transport has connected and authenticated.
static void setupSuccessfulDoWork(CIoTHubTransportAMQPMocks& mocks, IOTHUBTRANSPORT_CONFIG& config, time_t current_time, MESSAGERECEIVER_CREATION_ACTION msg_rcvr_action)
{
    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(config.waitingToSend)).SetReturn(1);
    setExpectedCallsForSASTokenExpiryCheck(mocks, current_time);
    
    if (msg_rcvr_action == MESSAGERECEIVER_CREATE)
    {
        setExpectedCallsForCreateMessageReceiver(mocks);
    }
    else if (msg_rcvr_action == MESSAGERECEIVER_DESTROY)
    {
        setExpectedCallsForDestroyMessageReceiver(mocks);
    }

    setExpectedCallsForConnectionDoWork(mocks);
}

static void setupSuccessfulDoWorkAndAuthenticate(TRANSPORT_LL_HANDLE transport, CIoTHubTransportAMQPMocks& mocks, time_t current_time)
{
    setExpectedCallsForTransportDoWorkUpTo(mocks, STEP_DOWORK_OPEN_CBS, DOWORK_MESSAGERECEIVER_NONE, current_time);
    setExpectedCallsForCbsAuthentication(mocks, current_time);
    setExpectedCallsForCbsAuthTimeoutCheck(mocks, current_time);
    setExpectedCallsForConnectionDoWork(mocks);
    setExpectedCallsForSASTokenExpiryCheck(mocks, current_time);
    setExpectedCallsForCreateEventSender(mocks);
    setExpectedCallsForConnectionDoWork(mocks);
    ((TRANSPORT_PROVIDER*)AMQP_Protocol())->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);
    test_latest_cbs_put_token_callback(test_latest_cbs_put_token_context, CBS_OPERATION_RESULT_OK, 0, NULL);
    ((TRANSPORT_PROVIDER*)AMQP_Protocol())->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);
    mocks.ResetAllCalls();
}


static void resetTestSuiteState()
{
    fail_malloc = false;
    fail_STRING_new = false;
    fail_STRING_new_with_memory = false;
    fail_STRING_construct = false;
    saved_on_message_received_callback = NULL;
    saved_on_message_received_context = NULL;
    saved_message_get_body_amqp_data_binary_data = NULL;
    test_amqpvalue_get_string_index = 0;
    saved_on_message_receiver_state_changed_callback = NULL;
    saved_on_message_receiver_state_changed_context = NULL;
    saved_on_message_sender_state_changed_callback = NULL;
    saved_on_message_sender_state_changed_context = NULL;
}

static time_t addSecondsToTime(time_t reference_time, int seconds_to_add)
{
    time_t result = 0;

    tm cal_time = *localtime(&reference_time);
    cal_time.tm_sec += seconds_to_add;
    result = mktime(&cal_time);

    return result;
}


BEGIN_TEST_SUITE(iothubtransportamqp_ut)

TEST_SUITE_INITIALIZE(TestClassInitialize)
{
    TEST_INITIALIZE_MEMORY_DEBUG(g_dllByDll);
    g_testByTest = MicroMockCreateMutex();
    ASSERT_IS_NOT_NULL(g_testByTest);
    
    test_iotHubHostFqdn = BASEIMPLEMENTATION::STRING_construct(TEST_IOT_HUB_NAME "." TEST_IOT_HUB_SUFFIX);
    ASSERT_IS_NOT_NULL(test_iotHubHostFqdn);

    test_STRING_HANDLE_EMPTY = BASEIMPLEMENTATION::STRING_new();
    ASSERT_IS_NOT_NULL(test_STRING_HANDLE_EMPTY);

    test_current_time = BASEIMPLEMENTATION::get_time(NULL);

    test_binary_data.bytes = (const unsigned char*)TEST_RANDOM_CHAR_SEQ;
    test_binary_data.length = TEST_RANDOM_CHAR_SEQ_SIZE;

}

TEST_SUITE_CLEANUP(TestClassCleanup)
{
    BASEIMPLEMENTATION::STRING_delete(test_iotHubHostFqdn);
    BASEIMPLEMENTATION::STRING_delete(test_STRING_HANDLE_EMPTY);

    MicroMockDestroyMutex(g_testByTest);
    TEST_DEINITIALIZE_MEMORY_DEBUG(g_dllByDll);
}

TEST_FUNCTION_INITIALIZE(TestMethodInitialize)
{
    if (!MicroMockAcquireMutex(g_testByTest))
    {
        ASSERT_FAIL("our mutex is ABANDONED. Failure in test framework");
    }
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

    resetTestSuiteState();
}

// Tests_SRS_IOTHUBTRANSPORTAMQP_09_005: [If parameter config (or its fields) is NULL then IoTHubTransportAMQP_Create shall fail and return NULL.] 
TEST_FUNCTION(AMQP_Create_with_null_config_fails)
{
    // arrange
    TRANSPORT_PROVIDER* transport_interface;
    transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();

    // act
    TRANSPORT_LL_HANDLE transportHandle = transport_interface->IoTHubTransport_Create(NULL);

    // assert
    ASSERT_IS_NULL(transportHandle);
}

// Tests_SRS_IOTHUBTRANSPORTAMQP_09_006: [IoTHubTransportAMQP_Create shall fail and return NULL if any fields of the config structure are NULL.]
TEST_FUNCTION(AMQP_Create_with_config_null_field1_fails)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config;
    TRANSPORT_PROVIDER* transport_interface;

    config.upperConfig = NULL;
    transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();

    // act
    TRANSPORT_LL_HANDLE transportHandle = transport_interface->IoTHubTransport_Create(&config);

    // assert
    ASSERT_IS_NULL(transportHandle);
}

// Tests_SRS_IOTHUBTRANSPORTAMQP_09_006: [IoTHubTransportAMQP_Create shall fail and return NULL if any fields of the config structure are NULL.]
TEST_FUNCTION(AMQP_Create_with_config_null_field2_fails)
{
    // arrange
    IOTHUBTRANSPORT_CONFIG config;
    TRANSPORT_PROVIDER* transport_interface;
    IOTHUB_CLIENT_CONFIG client_config;

    config.waitingToSend = NULL;
    config.upperConfig = &client_config;
    transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();

    // act
    TRANSPORT_LL_HANDLE transportHandle = transport_interface->IoTHubTransport_Create(&config);

    // assert
    ASSERT_IS_NULL(transportHandle);
}

// Tests_SRS_IOTHUBTRANSPORTAMQP_09_007: [IoTHubTransportAMQP_Create shall fail and return NULL if the deviceId length is greater than 128.]
TEST_FUNCTION(AMQP_Create_with_config_deviceId_too_long_fails)
{
    // arrange
    DLIST_ENTRY wts;
    TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();

    IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
        "012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678",
        TEST_DEVICE_KEY, TEST_DEVICE_SAS, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME_NULL };

    IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };
    
    // act
    TRANSPORT_LL_HANDLE transportHandle = transport_interface->IoTHubTransport_Create(&config);

    // assert
    ASSERT_IS_NULL(transportHandle);
}

// Tests_SRS_IOTHUBTRANSPORTAMQP_09_008: [IoTHubTransportAMQP_Create shall fail and return NULL if any config field of type string is zero length.] 
TEST_FUNCTION(AMQP_Create_with_config_deviceId_NULL_fails)
{
    // arrange
    DLIST_ENTRY wts;
    TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();

    IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
        NULL, TEST_DEVICE_KEY, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME_NULL };

    IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };

    // act
    TRANSPORT_LL_HANDLE transportHandle = transport_interface->IoTHubTransport_Create(&config);

    // assert
    ASSERT_IS_NULL(transportHandle);
}

// Tests_SRS_IOTHUBTRANSPORTAMQP_03_001: [IoTHubTransportAMQP_Create shall fail and return NULL if both deviceKey & deviceSasToken fields are NOT NULL.]
TEST_FUNCTION(AMQP_Create_with_config_deviceKey_and_deviceSasToken_defined_fails)
{
    // arrange
    DLIST_ENTRY wts;
    TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();

    IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
        TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_DEVICE_SAS, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME_NULL };

    IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };

    // act
    TRANSPORT_LL_HANDLE transportHandle = transport_interface->IoTHubTransport_Create(&config);

    // assert
    ASSERT_IS_NULL(transportHandle);
}



// Tests_SRS_IOTHUBTRANSPORTAMQP_09_008: [IoTHubTransportAMQP_Create shall fail and return NULL if any config field of type string is zero length.] 
TEST_FUNCTION(AMQP_Create_with_config_hubName_NULL_fails)
{
    // arrange
    DLIST_ENTRY wts;
    TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();

    IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
        TEST_DEVICE_ID, TEST_DEVICE_KEY, NULL, NULL, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME_NULL };

    IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };

    // act
    TRANSPORT_LL_HANDLE transportHandle = transport_interface->IoTHubTransport_Create(&config);

    // assert
    ASSERT_IS_NULL(transportHandle);
}

// Tests_SRS_IOTHUBTRANSPORTAMQP_09_008: [IoTHubTransportAMQP_Create shall fail and return NULL if any config field of type string is zero length.] 
TEST_FUNCTION(AMQP_Create_with_config_hubSuffix_NULL_fails)
{
    // arrange
    DLIST_ENTRY wts;
    TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();

    IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
        TEST_DEVICE_ID, TEST_DEVICE_KEY, NULL, TEST_IOT_HUB_NAME, NULL, TEST_PROT_GW_HOSTNAME_NULL };

    IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };

    // act
    TRANSPORT_LL_HANDLE transportHandle = transport_interface->IoTHubTransport_Create(&config);

    // assert
    ASSERT_IS_NULL(transportHandle);
}

// Tests_SRS_IOTHUBTRANSPORTAMQP_09_134: [IoTHubTransportAMQP_Create shall fail and return NULL if the combined length of config->iotHubName and config->iotHubSuffix exceeds 254 bytes (RFC1035)]
TEST_FUNCTION(AMQP_Create_with_config_hubFqdn_too_long_fails)
{
    // arrange
    DLIST_ENTRY wts;
    TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();

    IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
        TEST_DEVICE_ID, TEST_DEVICE_KEY, 
        "0123456789012345678901234567890123456789", 
        "01234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234", 
        TEST_PROT_GW_HOSTNAME_NULL };

    IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };

    // act
    TRANSPORT_LL_HANDLE transportHandle = transport_interface->IoTHubTransport_Create(&config);

    // assert
    ASSERT_IS_NULL(transportHandle);
}

// Tests_SRS_IOTHUBTRANSPORTAMQP_09_009: [IoTHubTransportAMQP_Create shall fail and return NULL if memory allocation of the transport's internal state structure fails.]
TEST_FUNCTION(AMQP_Create_transport_state_allocation_fails)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();

    IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
        TEST_DEVICE_ID, TEST_DEVICE_KEY, NULL, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME_NULL };

    IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };

    fail_malloc = true;

    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(0)).IgnoreArgument(1);

    // act
    TRANSPORT_LL_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);

    // assert
    ASSERT_IS_NULL(transport);
}

// Tests_SRS_IOTHUBTRANSPORTAMQP_09_010: [If config->upperConfig->protocolGatewayHostName is NULL, IoTHubTransportAMQP_Create shall create an immutable string, referred to as iotHubHostFqdn, from the following pieces: config->iotHubName + "." + config->iotHubSuffix.] 
// Tests_SRS_IOTHUBTRANSPORTAMQP_09_012: [IoTHubTransportAMQP_Create shall create an immutable string, referred to as devicesPath, from the following parts : host_fqdn + " / devices / " + deviceId.]
// Tests_SRS_IOTHUBTRANSPORTAMQP_09_014: [IoTHubTransportAMQP_Create shall create an immutable string, referred to as targetAddress, from the following parts: "amqps://" + devicesPath + "/messages/events".]
// Tests_SRS_IOTHUBTRANSPORTAMQP_09_053: [IoTHubTransportAMQP_Create shall define the source address for receiving messages as "amqps://" + devicesPath + "/messages/devicebound", stored in the transport handle as messageReceiveAddress]
// Tests_SRS_IOTHUBTRANSPORTAMQP_09_016: [IoTHubTransportAMQP_Create shall initialize handle->sasTokenKeyName with a zero-length STRING_HANDLE instance.] 
// Tests_SRS_IOTHUBTRANSPORTAMQP_09_018: [IoTHubTransportAMQP_Create shall store a copy of config->deviceKey (passed by upper layer) into the transport's own deviceKey field.] 
// Tests_SRS_IOTHUBTRANSPORTAMQP_09_023: [If IoTHubTransportAMQP_Create succeeds it shall return a non-NULL pointer to the structure that represents the transport.] 
TEST_FUNCTION(AMQP_Create_succeeds)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();

    IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
        TEST_DEVICE_ID, TEST_DEVICE_KEY, NULL, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME_NULL };
    IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };

    mocks.ResetAllCalls();
    setExpectedCallsForTransportCreateUpTo(mocks, &config, STEP_CREATE_DEVICEKEY);

    // act
    TRANSPORT_LL_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);

    // assert
    ASSERT_IS_NOT_NULL(transport);
    mocks.AssertActualAndExpectedCalls();

    // cleanup
    transport_interface->IoTHubTransport_Destroy(transport);
}

// Tests_SRS_IOTHUBTRANSPORTAMQP_20_001: [If config->upperConfig->protocolGatewayHostName is not NULL, IoTHubTransportAMQP_Create shall use it as iotHubHostFqdn]
// Tests_SRS_IOTHUBTRANSPORTAMQP_09_012: [IoTHubTransportAMQP_Create shall create an immutable string, referred to as devicesPath, from the following parts : host_fqdn + " / devices / " + deviceId.]
// Tests_SRS_IOTHUBTRANSPORTAMQP_09_014: [IoTHubTransportAMQP_Create shall create an immutable string, referred to as targetAddress, from the following parts: "amqps://" + devicesPath + "/messages/events".]
// Tests_SRS_IOTHUBTRANSPORTAMQP_09_053: [IoTHubTransportAMQP_Create shall define the source address for receiving messages as "amqps://" + devicesPath + "/messages/devicebound", stored in the transport handle as messageReceiveAddress]
// Tests_SRS_IOTHUBTRANSPORTAMQP_09_016: [IoTHubTransportAMQP_Create shall initialize handle->sasTokenKeyName with a zero-length STRING_HANDLE instance.] 
// Tests_SRS_IOTHUBTRANSPORTAMQP_09_018: [IoTHubTransportAMQP_Create shall store a copy of config->deviceKey (passed by upper layer) into the transport's own deviceKey field.] 
// Tests_SRS_IOTHUBTRANSPORTAMQP_09_023: [If IoTHubTransportAMQP_Create succeeds it shall return a non-NULL pointer to the structure that represents the transport.] 
TEST_FUNCTION(AMQP_Create_succeeds_with_gw_hostname)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();

    IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
        TEST_DEVICE_ID, TEST_DEVICE_KEY, NULL, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME };
    IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };

    mocks.ResetAllCalls();
    setExpectedCallsForTransportCreateUpTo(mocks, &config, STEP_CREATE_DEVICEKEY);

    // act
    TRANSPORT_LL_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);

    // assert
    ASSERT_IS_NOT_NULL(transport);
    mocks.AssertActualAndExpectedCalls();

    // cleanup
    transport_interface->IoTHubTransport_Destroy(transport);
}

// Tests_SRS_IOTHUBTRANSPORTAMQP_09_013: [If creating devicesPath fails for any reason then IoTHubTransportAMQP_Create shall fail and return NULL.] 
TEST_FUNCTION(AMQP_Create_devicesPath_malloc_fails)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();

    IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
        TEST_DEVICE_ID, TEST_DEVICE_KEY, NULL, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME_NULL };
    IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };

    const char* devicesPath = TEST_IOT_HUB_NAME "." TEST_IOT_HUB_SUFFIX "/devices/" TEST_DEVICE_ID;

    mocks.ResetAllCalls();
    setExpectedCallsForTransportCreateUpTo(mocks, &config, STEP_CREATE_IOTHUB_FQDN);
    EXPECTED_CALL(mocks, STRING_c_str(0));
    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(devicesPath) + 1)).SetFailReturn((char*)NULL);
    setExpectedCleanupCallsForTransportCreateUpTo(mocks, STEP_CREATE_IOTHUB_FQDN);

    // act
    TRANSPORT_LL_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);

    // assert
    ASSERT_IS_NULL(transport);
    mocks.AssertActualAndExpectedCalls();
}

// Tests_SRS_IOTHUBTRANSPORTAMQP_09_013: [If creating devicesPath fails for any reason then IoTHubTransportAMQP_Create shall fail and return NULL.] 
TEST_FUNCTION(AMQP_Create_devicesPath_STRING_construct_fails)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();

    IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
        TEST_DEVICE_ID, TEST_DEVICE_KEY, NULL, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME_NULL };
    IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };

    const char* devicesPath = TEST_IOT_HUB_NAME "." TEST_IOT_HUB_SUFFIX "/devices/" TEST_DEVICE_ID;

    mocks.ResetAllCalls();
    setExpectedCallsForTransportCreateUpTo(mocks, &config, STEP_CREATE_IOTHUB_FQDN);
    EXPECTED_CALL(mocks, STRING_c_str(0));
    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(devicesPath) + 1));
    STRICT_EXPECTED_CALL(mocks, STRING_construct(devicesPath)).SetFailReturn(TEST_NULL_STRING_HANDLE);
    EXPECTED_CALL(mocks, gballoc_free(0));
    setExpectedCleanupCallsForTransportCreateUpTo(mocks, STEP_CREATE_IOTHUB_FQDN);
    
    // act
    TRANSPORT_LL_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);

    // assert
    ASSERT_IS_NULL(transport);
    mocks.AssertActualAndExpectedCalls();
}

// Tests_SRS_IOTHUBTRANSPORTAMQP_09_015: [If creating the targetAddress fails for any reason then IoTHubTransportAMQP_Create shall fail and return NULL.]
TEST_FUNCTION(AMQP_Create_targetAddress_malloc_fails)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();

    IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
        TEST_DEVICE_ID, TEST_DEVICE_KEY, NULL, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME_NULL };
    IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };

    const char* targetAddress = "amqps://" TEST_IOT_HUB_NAME "." TEST_IOT_HUB_SUFFIX "/devices/" TEST_DEVICE_ID "/messages/events";

    mocks.ResetAllCalls();
    setExpectedCallsForTransportCreateUpTo(mocks, &config, STEP_CREATE_DEVICES_PATH);
    EXPECTED_CALL(mocks, STRING_c_str(0));
    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(targetAddress) + 1)).SetFailReturn((char*)NULL);
    setExpectedCleanupCallsForTransportCreateUpTo(mocks, STEP_CREATE_DEVICES_PATH);

    // act
    TRANSPORT_LL_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);

    // assert
    ASSERT_IS_NULL(transport);
    mocks.AssertActualAndExpectedCalls();
}

// Tests_SRS_IOTHUBTRANSPORTAMQP_09_015: [If creating the targetAddress fails for any reason then IoTHubTransportAMQP_Create shall fail and return NULL.] 
TEST_FUNCTION(AMQP_Create_targetAddress_STRING_construct_fails)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();

    IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
        TEST_DEVICE_ID, TEST_DEVICE_KEY, NULL, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME_NULL };
    IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };

    const char* targetAddress = "amqps://" TEST_IOT_HUB_NAME "." TEST_IOT_HUB_SUFFIX "/devices/" TEST_DEVICE_ID "/messages/events";

    mocks.ResetAllCalls();
    setExpectedCallsForTransportCreateUpTo(mocks, &config, STEP_CREATE_DEVICES_PATH);
    EXPECTED_CALL(mocks, STRING_c_str(0));
    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(targetAddress) + 1));
    STRICT_EXPECTED_CALL(mocks, STRING_construct(targetAddress)).SetFailReturn(TEST_NULL_STRING_HANDLE);
    EXPECTED_CALL(mocks, gballoc_free(0));
    setExpectedCleanupCallsForTransportCreateUpTo(mocks, STEP_CREATE_DEVICES_PATH);

    // act
    TRANSPORT_LL_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);

    // assert
    ASSERT_IS_NULL(transport);
    mocks.AssertActualAndExpectedCalls();
}

// Tests_SRS_IOTHUBTRANSPORTAMQP_09_054: [If creating the messageReceiveAddress fails for any reason then IoTHubTransportAMQP_Create shall fail and return NULL.]
TEST_FUNCTION(AMQP_Create_receiveAddress_malloc_fails)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();

    IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
        TEST_DEVICE_ID, TEST_DEVICE_KEY, NULL, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME_NULL };
    IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };

    const char* receiveAddress = "amqps://"  TEST_IOT_HUB_NAME "." TEST_IOT_HUB_SUFFIX "/devices/" TEST_DEVICE_ID "/messages/devicebound";

    mocks.ResetAllCalls();
    setExpectedCallsForTransportCreateUpTo(mocks, &config, STEP_CREATE_TARGET_ADDRESS);
    EXPECTED_CALL(mocks, STRING_c_str(0));
    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(receiveAddress) + 1)).SetFailReturn((char*)NULL);
    setExpectedCleanupCallsForTransportCreateUpTo(mocks, STEP_CREATE_TARGET_ADDRESS);

    // act
    TRANSPORT_LL_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);

    // assert
    ASSERT_IS_NULL(transport);
    mocks.AssertActualAndExpectedCalls();
}

// Tests_SRS_IOTHUBTRANSPORTAMQP_09_054: [If creating the messageReceiveAddress fails for any reason then IoTHubTransportAMQP_Create shall fail and return NULL.]
TEST_FUNCTION(AMQP_Create_receiveAddress_STRING_construct_fails)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();

    IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
        TEST_DEVICE_ID, TEST_DEVICE_KEY, NULL, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME_NULL };
    IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };

    const char* receiveAddress = "amqps://"  TEST_IOT_HUB_NAME "." TEST_IOT_HUB_SUFFIX "/devices/" TEST_DEVICE_ID "/messages/devicebound";

    mocks.ResetAllCalls();
    setExpectedCallsForTransportCreateUpTo(mocks, &config, STEP_CREATE_TARGET_ADDRESS);
    EXPECTED_CALL(mocks, STRING_c_str(0));
    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(receiveAddress) + 1));
    STRICT_EXPECTED_CALL(mocks, STRING_construct(receiveAddress)).SetFailReturn(TEST_NULL_STRING_HANDLE);
    EXPECTED_CALL(mocks, gballoc_free(0));
    setExpectedCleanupCallsForTransportCreateUpTo(mocks, STEP_CREATE_TARGET_ADDRESS);

    // act
    TRANSPORT_LL_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);

    // assert
    ASSERT_IS_NULL(transport);
    mocks.AssertActualAndExpectedCalls();
}

// Tests_SRS_IOTHUBTRANSPORTAMQP_09_017: [If IoTHubTransportAMQP_Create fails to initialize handle->sasTokenKeyName with a zero-length STRING the function shall fail and return NULL.] 
TEST_FUNCTION(AMQP_Create_sasTokenKeyName_allocation_fails)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();

    IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
        TEST_DEVICE_ID, TEST_DEVICE_KEY, NULL, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME_NULL };
    IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };

    mocks.ResetAllCalls();
    setExpectedCallsForTransportCreateUpTo(mocks, &config, STEP_CREATE_RECEIVE_ADDRESS);
    STRICT_EXPECTED_CALL(mocks, STRING_new()).SetFailReturn(TEST_NULL_STRING_HANDLE);
    setExpectedCleanupCallsForTransportCreateUpTo(mocks, STEP_CREATE_RECEIVE_ADDRESS);

    // act
    TRANSPORT_LL_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);

    // assert
    ASSERT_IS_NULL(transport);
    mocks.AssertActualAndExpectedCalls();
}

// Tests_SRS_IOTHUBTRANSPORTAMQP_09_135: [If creating the config->deviceKey fails for any reason then IoTHubTransportAMQP_Create shall fail and return NULL.]
TEST_FUNCTION(AMQP_Create_deviceKey_allocation_fails)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();

    IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
        TEST_DEVICE_ID, TEST_DEVICE_KEY, NULL, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME_NULL };
    IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };

    mocks.ResetAllCalls();
    setExpectedCallsForTransportCreateUpTo(mocks, &config, STEP_CREATE_SASTOKEN_KEYNAME);
    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_DEVICE_KEY)).SetFailReturn((STRING_HANDLE)(NULL));
    setExpectedCleanupCallsForTransportCreateUpTo(mocks, STEP_CREATE_SASTOKEN_KEYNAME);

    // act
    TRANSPORT_LL_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);

    // assert
    ASSERT_IS_NULL(transport);
    mocks.AssertActualAndExpectedCalls();
}

// Tests_SRS_IOTHUBTRANSPORTAMQP_09_019: [If IoTHubTransportAMQP_Create fails to copy config->deviceKey, the function shall fail and return NULL.] 
TEST_FUNCTION(AMQP_Create_deviceKey_copy_fails)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();

    IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
        TEST_DEVICE_ID, TEST_DEVICE_KEY, NULL, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME_NULL };
    IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };

    mocks.ResetAllCalls();
    setExpectedCallsForTransportCreateUpTo(mocks, &config, STEP_CREATE_SASTOKEN_KEYNAME);
    STRICT_EXPECTED_CALL(mocks, STRING_construct(config.upperConfig->deviceKey))
        .SetFailReturn((STRING_HANDLE)NULL);
    setExpectedCleanupCallsForTransportCreateUpTo(mocks, STEP_CREATE_SASTOKEN_KEYNAME);

    // act
    TRANSPORT_LL_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);

    // assert
    ASSERT_IS_NULL(transport);
    mocks.AssertActualAndExpectedCalls();
}

// Tests_SRS_IOTHUBTRANSPORTAMQP_09_024: [IoTHubTransportAMQP_Destroy shall destroy the AMQP message_sender.]
// Tests_SRS_IOTHUBTRANSPORTAMQP_09_025: [IoTHubTransportAMQP_Destroy shall destroy the AMQP message_receiver.]
// Tests_SRS_IOTHUBTRANSPORTAMQP_09_027: [IoTHubTransportAMQP_Destroy shall destroy the AMQP cbs instance]
// Tests_SRS_IOTHUBTRANSPORTAMQP_09_029: [IoTHubTransportAMQP_Destroy shall destroy the AMQP link.]
// Tests_SRS_IOTHUBTRANSPORTAMQP_09_030: [IoTHubTransportAMQP_Destroy shall destroy the AMQP session.]
// Tests_SRS_IOTHUBTRANSPORTAMQP_09_031: [IoTHubTransportAMQP_Destroy shall destroy the AMQP connection.]
// Tests_SRS_IOTHUBTRANSPORTAMQP_09_032: [IoTHubTransportAMQP_Destroy shall destroy the AMQP SASL I / O transport.]
// Tests_SRS_IOTHUBTRANSPORTAMQP_09_033: [IoTHubTransportAMQP_Destroy shall destroy the AMQP SASL mechanism.]
// Tests_SRS_IOTHUBTRANSPORTAMQP_09_034: [IoTHubTransportAMQP_Destroy shall destroy the AMQP TLS I/O transport.] 
// Tests_SRS_IOTHUBTRANSPORTAMQP_09_035: [IoTHubTransportAMQP_Destroy shall delete its internally - set parameters(deviceKey, targetAddress, devicesPath, sasTokenKeyName).]
// Tests_SRS_IOTHUBTRANSPORTAMQP_09_036: [IoTHubTransportAMQP_Destroy shall return the remaining items in inProgress to waitingToSend list.] 
TEST_FUNCTION(AMQP_Destroy_succeeds_no_DoWork)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();

    IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
        TEST_DEVICE_ID, TEST_DEVICE_KEY, NULL, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME_NULL };
    IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };

    TRANSPORT_LL_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);

    ASSERT_IS_NOT_NULL(transport);

    mocks.ResetAllCalls();
    setExpectedCallsForTransportDestroy(mocks, false, false, 0);

    // act
    transport_interface->IoTHubTransport_Destroy(transport);

    // assert
    mocks.AssertActualAndExpectedCalls();
}

// Tests_SRS_IOTHUBTRANSPORTAMQP_09_051: [IoTHubTransportAMQP_DoWork shall fail and return immediately if the transport handle parameter is NULL] 
TEST_FUNCTION(AMQP_DoWork_transport_handle_NULL_fails)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();

    IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
        TEST_DEVICE_ID, TEST_DEVICE_KEY, NULL, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME_NULL };
    IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };

    mocks.ResetAllCalls();

    // act
    transport_interface->IoTHubTransport_DoWork(NULL, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // assert
    mocks.AssertActualAndExpectedCalls(); // Nothing is expected.
}

// Tests_SRS_IOTHUBTRANSPORTAMQP_09_052: [IoTHubTransportAMQP_DoWork shall fail and return immediately if the client handle parameter is NULL] 
TEST_FUNCTION(AMQP_DoWork_client_handle_NULL_fails)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();

    IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
        TEST_DEVICE_ID, TEST_DEVICE_KEY, NULL, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME_NULL };
    IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };

    TRANSPORT_LL_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);

    mocks.ResetAllCalls();

    // act
    transport_interface->IoTHubTransport_DoWork(transport, NULL);

    // assert
    mocks.AssertActualAndExpectedCalls(); // Nothing is expected.

    // cleanup
    transport_interface->IoTHubTransport_Destroy(transport);
}
// Tests_SRS_IOTHUBTRANSPORTAMQP_09_056: [IoTHubTransportAMQP_DoWork shall create the SASL mechanism using AMQP's saslmechanism_create() API] 
// Tests_SRS_IOTHUBTRANSPORTAMQP_09_057: [If saslmechanism_create() fails, IoTHubTransportAMQP_DoWork shall fail and return immediately]
TEST_FUNCTION(AMQP_DoWork_saslmechanism_create_fails)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();

    IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
        TEST_DEVICE_ID, TEST_DEVICE_KEY, NULL, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME_NULL };
    IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };

    TRANSPORT_LL_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);

    mocks.ResetAllCalls();
    setExpectedCallsForTransportDoWorkUpTo(mocks, STEP_DOWORK_GET_TLS_IO, DOWORK_MESSAGERECEIVER_NONE, time(NULL));
    STRICT_EXPECTED_CALL(mocks, saslmssbcbs_get_interface());
    EXPECTED_CALL(mocks, saslmechanism_create(NULL, NULL)).SetReturn((SASL_MECHANISM_HANDLE)NULL);
    setExpectedCallsForConnectionDestroyUpTo(mocks, STEP_DOWORK_GET_TLS_IO);

    // act
    transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // assert
    mocks.AssertActualAndExpectedCalls();

    // cleanup
    transport_interface->IoTHubTransport_Destroy(transport);
}

// Tests_SRS_IOTHUBTRANSPORTAMQP_09_060: [IoTHubTransportAMQP_DoWork shall create the SASL I/O layer using the xio_create() C Shared Utility API] 
// Tests_SRS_IOTHUBTRANSPORTAMQP_09_061: [If xio_create() fails creating the SASL I/O layer, IoTHubTransportAMQP_DoWork shall fail and return immediately]
TEST_FUNCTION(AMQP_DoWork_saslio_create_fails)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();

    IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
        TEST_DEVICE_ID, TEST_DEVICE_KEY, NULL, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME_NULL };
    IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };

    TRANSPORT_LL_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);

    mocks.ResetAllCalls();
    setExpectedCallsForTransportDoWorkUpTo(mocks, STEP_DOWORK_SASLIO_GET_INTERFACE, DOWORK_MESSAGERECEIVER_NONE, time(NULL));
    EXPECTED_CALL(mocks, xio_create(NULL, NULL)).SetReturn((XIO_HANDLE)NULL);
    setExpectedCallsForConnectionDestroyUpTo(mocks, STEP_DOWORK_CREATE_SASLMECHANISM);

    // act
    transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // assert
    mocks.AssertActualAndExpectedCalls();

    // cleanup
    transport_interface->IoTHubTransport_Destroy(transport);
}

// Tests_SRS_IOTHUBTRANSPORTAMQP_09_062: [IoTHubTransportAMQP_DoWork shall create the connection with the IoT service using connection_create2() AMQP API, passing the SASL I/O layer, IoT Hub FQDN and container ID as parameters (pass NULL for callbacks)]
// Tests_SRS_IOTHUBTRANSPORTAMQP_09_063: [If connection_create2() fails, IoTHubTransportAMQP_DoWork shall fail and return immediately]
TEST_FUNCTION(AMQP_DoWork_connection_create2_fails)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();

    IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
        TEST_DEVICE_ID, TEST_DEVICE_KEY, NULL, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME_NULL };
    IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };

    TRANSPORT_LL_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);

    mocks.ResetAllCalls();
    setExpectedCallsForTransportDoWorkUpTo(mocks, STEP_DOWORK_CREATE_SASLIO, DOWORK_MESSAGERECEIVER_NONE, time(NULL));
    EXPECTED_CALL(mocks, STRING_c_str(NULL));
    EXPECTED_CALL(mocks, connection_create2(NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)).SetReturn((CONNECTION_HANDLE)NULL);
    setExpectedCallsForConnectionDestroyUpTo(mocks, STEP_DOWORK_CREATE_SASLIO);

    // act
    transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // assert
    mocks.AssertActualAndExpectedCalls();

    // cleanup
    transport_interface->IoTHubTransport_Destroy(transport);
}

// Tests_SRS_IOTHUBTRANSPORTAMQP_09_137: [IoTHubTransportAMQP_DoWork shall create the AMQP session session_create() AMQP API, passing the connection instance as parameter]
// Tests_SRS_IOTHUBTRANSPORTAMQP_09_138: [If session_create() fails, IoTHubTransportAMQP_DoWork shall fail and return immediately]
TEST_FUNCTION(AMQP_DoWork_session_create_fails)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();

    IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
        TEST_DEVICE_ID, TEST_DEVICE_KEY, NULL, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME_NULL };
    IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };

    TRANSPORT_LL_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);

    mocks.ResetAllCalls();
    setExpectedCallsForTransportDoWorkUpTo(mocks, STEP_DOWORK_CREATE_CONNECTION, DOWORK_MESSAGERECEIVER_NONE, time(NULL));
    EXPECTED_CALL(mocks, session_create(NULL, NULL, NULL)).SetReturn((SESSION_HANDLE)NULL);
    setExpectedCallsForConnectionDestroyUpTo(mocks, STEP_DOWORK_CREATE_CONNECTION);

    // act
    transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // assert
    mocks.AssertActualAndExpectedCalls();

    // cleanup
    transport_interface->IoTHubTransport_Destroy(transport);
}

// Tests_SRS_IOTHUBTRANSPORTAMQP_09_065: [IoTHubTransportAMQP_DoWork shall apply a default value of UINT_MAX for the parameter 'AMQP incoming window'] 
// Tests_SRS_IOTHUBTRANSPORTAMQP_09_115: [IoTHubTransportAMQP_DoWork shall apply a default value of 100 for the parameter 'AMQP outgoing window'] 
// Tests_SRS_IOTHUBTRANSPORTAMQP_09_066: [IoTHubTransportAMQP_DoWork shall establish the CBS connection using the cbs_create() AMQP API] 
// Tests_SRS_IOTHUBTRANSPORTAMQP_09_067: [If cbs_create() fails, IoTHubTransportAMQP_DoWork shall fail and return immediately]
TEST_FUNCTION(AMQP_DoWork_cbs_create_fails)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();

    IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
        TEST_DEVICE_ID, TEST_DEVICE_KEY, NULL, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME_NULL };
    IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };

    TRANSPORT_LL_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);

    mocks.ResetAllCalls();
    setExpectedCallsForTransportDoWorkUpTo(mocks, STEP_DOWORK_OUTGOING_WINDOW, DOWORK_MESSAGERECEIVER_NONE, time(NULL));
    EXPECTED_CALL(mocks, cbs_create(NULL, NULL, NULL)).SetReturn((CBS_HANDLE)NULL);
    setExpectedCallsForConnectionDestroyUpTo(mocks, STEP_DOWORK_CREATE_SESSION);

    // act
    transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // assert
    mocks.AssertActualAndExpectedCalls();

    // cleanup
    transport_interface->IoTHubTransport_Destroy(transport);
}

// Tests_SRS_IOTHUBTRANSPORTAMQP_09_139: [IoTHubTransportAMQP_DoWork shall open the CBS connection using the cbs_open() AMQP API]
// Tests_SRS_IOTHUBTRANSPORTAMQP_09_140: [If cbs_open() fails, IoTHubTransportAMQP_DoWork shall fail and return immediately]
TEST_FUNCTION(AMQP_DoWork_cbs_open_fails)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();

    IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
        TEST_DEVICE_ID, TEST_DEVICE_KEY, NULL, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME_NULL };
    IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };

    TRANSPORT_LL_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);

    mocks.ResetAllCalls();
    setExpectedCallsForTransportDoWorkUpTo(mocks, STEP_DOWORK_CREATE_CBS, DOWORK_MESSAGERECEIVER_NONE, time(NULL));
    EXPECTED_CALL(mocks, cbs_open(NULL)).SetReturn(1);
    setExpectedCallsForConnectionDestroyUpTo(mocks, STEP_DOWORK_CREATE_CBS);

    // act
    transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // assert
    mocks.AssertActualAndExpectedCalls();

    // cleanup
    transport_interface->IoTHubTransport_Destroy(transport);
}

// Tests_SRS_IOTHUBTRANSPORTAMQP_09_020: [IoTHubTransportAMQP_Create shall set parameter transport_state->sas_token_lifetime with the default value of 3600000 (milliseconds).] 
// Tests_SRS_IOTHUBTRANSPORTAMQP_09_081: [IoTHubTransportAMQP_DoWork shall put a new SAS token if the one has not been out already, or if the previous one failed to be put due to timeout of cbs_put_token().] 
// Tests_SRS_IOTHUBTRANSPORTAMQP_09_083: [SAS tokens expiration time shall be calculated using the number of seconds since Epoch UTC (Jan 1st 1970 00h00m00s000 GMT) to now (GMT), plus the 'sas_token_lifetime'.]
TEST_FUNCTION(AMQP_DoWork_SASToken_create_fails)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
        TEST_DEVICE_ID, TEST_DEVICE_KEY, NULL, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME_NULL };
    IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };
    TRANSPORT_LL_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);
    time_t current_time = time(NULL);
    size_t expected_expiry_time = (size_t)(difftime(current_time, 0) + TEST_SAS_TOKEN_LIFETIME_MS / 1000);

    mocks.ResetAllCalls();
    setExpectedCallsForTransportDoWorkUpTo(mocks, STEP_DOWORK_OPEN_CBS, DOWORK_MESSAGERECEIVER_NONE, current_time);
	setExpectedCallsForGetSecondsSinceEpoch(mocks, current_time);
    EXPECTED_CALL(mocks, SASToken_Create(NULL, NULL, NULL, 0)).SetReturn((STRING_HANDLE)NULL);
    setExpectedCallsForConnectionDestroyUpTo(mocks, STEP_DOWORK_CREATE_CBS);

    // act
    transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // assert
    mocks.AssertActualAndExpectedCalls();
    ASSERT_ARE_EQUAL(size_t, expected_expiry_time, test_latest_SASToken_expiry_time);

    // cleanup
    transport_interface->IoTHubTransport_Destroy(transport);
}

// Tests_SRS_IOTHUBTRANSPORTAMQP_09_146: [If the SAS token fails to be sent to CBS (cbs_put_token), IoTHubTransportAMQP_DoWork shall fail and exit immediately]
// Tests_SRS_IOTHUBTRANSPORTAMQP_09_145: [Each new SAS token created shall be deleted from memory immediately after sending it to CBS]
TEST_FUNCTION(AMQP_DoWork_cbs_put_token_fails)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
        TEST_DEVICE_ID, TEST_DEVICE_KEY, NULL, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME_NULL };
    IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };
    TRANSPORT_LL_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);
    time_t current_time = time(NULL);

    mocks.ResetAllCalls();
    setExpectedCallsForTransportDoWorkUpTo(mocks, STEP_DOWORK_OPEN_CBS, DOWORK_MESSAGERECEIVER_NONE, current_time);
	setExpectedCallsForGetSecondsSinceEpoch(mocks, current_time);
    EXPECTED_CALL(mocks, SASToken_Create(NULL, NULL, NULL, 0));
    EXPECTED_CALL(mocks, STRING_c_str(NULL));
    EXPECTED_CALL(mocks, STRING_c_str(NULL));
    EXPECTED_CALL(mocks, cbs_put_token(NULL, NULL, NULL, NULL, NULL, NULL)).SetReturn(1);
    EXPECTED_CALL(mocks, STRING_delete(NULL));

    setExpectedCallsForConnectionDestroyUpTo(mocks, STEP_DOWORK_CREATE_CBS);

    // act
    transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // assert
    mocks.AssertActualAndExpectedCalls();

    // cleanup
    transport_interface->IoTHubTransport_Destroy(transport);
}

// Tests_SRS_IOTHUBTRANSPORTAMQP_09_084: [IoTHubTransportAMQP_DoWork shall wait for 'cbs_request_timeout' milliseconds for the cbs_put_token() to complete before failing due to timeout] 
TEST_FUNCTION(AMQP_DoWork_CBS_auth_timeout_fails)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
        TEST_DEVICE_ID, TEST_DEVICE_KEY, NULL, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME_NULL };
    IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };
    TRANSPORT_LL_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);
    time_t current_time = time(NULL);
    time_t expiration_time = addSecondsToTime(current_time, TEST_CBS_REQUEST_TIMEOUT_MS + 1);

    mocks.ResetAllCalls();
    setExpectedCallsForTransportDoWorkUpTo(mocks, STEP_DOWORK_OPEN_CBS, DOWORK_MESSAGERECEIVER_NONE, current_time);
    setExpectedCallsForCbsAuthentication(mocks, current_time);
	setExpectedCallsForGetSecondsSinceEpoch(mocks, expiration_time);
    setExpectedCallsForConnectionDestroyUpTo(mocks, STEP_DOWORK_CREATE_CBS);

    // act
    transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // assert
    mocks.AssertActualAndExpectedCalls();

    // cleanup
    transport_interface->IoTHubTransport_Destroy(transport);
}

// Tests_SRS_IOTHUBTRANSPORTAMQP_09_068: [IoTHubTransportAMQP_DoWork shall create the AMQP link for sending messages using 'source' as "ingress", target as the IoT hub FQDN, link name as "sender-link" and role as 'role_sender'] 
// Tests_SRS_IOTHUBTRANSPORTAMQP_09_069: [If IoTHubTransportAMQP_DoWork fails to create the AMQP link for sending messages, the function shall fail and return immediately, flagging the connection to be re-stablished] 
TEST_FUNCTION(AMQP_DoWork_messagesender_create_source_fails)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
        TEST_DEVICE_ID, TEST_DEVICE_KEY, NULL, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME_NULL };
    IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };
    TRANSPORT_LL_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);
    time_t current_time = time(NULL);

    mocks.ResetAllCalls();
    setExpectedCallsForTransportDoWorkUpTo(mocks, STEP_DOWORK_OPEN_CBS, DOWORK_MESSAGERECEIVER_NONE, current_time);
    setExpectedCallsForCbsAuthentication(mocks, current_time);
    setExpectedCallsForCbsAuthTimeoutCheck(mocks, current_time);
    setExpectedCallsForConnectionDoWork(mocks);
    setExpectedCallsForSASTokenExpiryCheck(mocks, current_time);
    EXPECTED_CALL(mocks, messaging_create_source(NULL)).SetReturn((AMQP_VALUE)NULL);
    setExpectedCallsForConnectionDestroyUpTo(mocks, STEP_DOWORK_CREATE_CBS);

    // act
    transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);
    test_latest_cbs_put_token_callback(test_latest_cbs_put_token_context, CBS_OPERATION_RESULT_OK, 0, NULL);
    transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // assert
    mocks.AssertActualAndExpectedCalls();

    // cleanup
    transport_interface->IoTHubTransport_Destroy(transport);
}

// Tests_SRS_IOTHUBTRANSPORTAMQP_09_068: [IoTHubTransportAMQP_DoWork shall create the AMQP link for sending messages using 'source' as "ingress", target as the IoT hub FQDN, link name as "sender-link" and role as 'role_sender'] 
// Tests_SRS_IOTHUBTRANSPORTAMQP_09_069: [If IoTHubTransportAMQP_DoWork fails to create the AMQP link for sending messages, the function shall fail and return immediately, flagging the connection to be re-stablished] 
TEST_FUNCTION(AMQP_DoWork_messagesender_create_target_fails)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
        TEST_DEVICE_ID, TEST_DEVICE_KEY, NULL, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME_NULL };
    IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };
    TRANSPORT_LL_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);
    time_t current_time = time(NULL);

    mocks.ResetAllCalls();
    setExpectedCallsForTransportDoWorkUpTo(mocks, STEP_DOWORK_OPEN_CBS, DOWORK_MESSAGERECEIVER_NONE, current_time);
    setExpectedCallsForCbsAuthentication(mocks, current_time);
    setExpectedCallsForCbsAuthTimeoutCheck(mocks, current_time);
    setExpectedCallsForConnectionDoWork(mocks);
    setExpectedCallsForSASTokenExpiryCheck(mocks, current_time);
    EXPECTED_CALL(mocks, messaging_create_source(NULL)).SetReturn(TEST_MESSAGESENDER_SOURCE);
    EXPECTED_CALL(mocks, STRING_c_str(NULL));
    EXPECTED_CALL(mocks, messaging_create_target(NULL)).SetReturn((AMQP_VALUE)NULL);
    STRICT_EXPECTED_CALL(mocks, amqpvalue_destroy(TEST_MESSAGESENDER_SOURCE));
    setExpectedCallsForConnectionDestroyUpTo(mocks, STEP_DOWORK_CREATE_CBS);

    // act
    transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);
    test_latest_cbs_put_token_callback(test_latest_cbs_put_token_context, CBS_OPERATION_RESULT_OK, 0, NULL);
    transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // assert
    mocks.AssertActualAndExpectedCalls();

    // cleanup
    transport_interface->IoTHubTransport_Destroy(transport);
}

// Tests_SRS_IOTHUBTRANSPORTAMQP_09_068: [IoTHubTransportAMQP_DoWork shall create the AMQP link for sending messages using 'source' as "ingress", target as the IoT hub FQDN, link name as "sender-link" and role as 'role_sender'] 
// Tests_SRS_IOTHUBTRANSPORTAMQP_09_069: [If IoTHubTransportAMQP_DoWork fails to create the AMQP link for sending messages, the function shall fail and return immediately, flagging the connection to be re-stablished] 
TEST_FUNCTION(AMQP_DoWork_messagesender_create_link_fails)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
        TEST_DEVICE_ID, TEST_DEVICE_KEY, NULL, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME_NULL };
    IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };
    TRANSPORT_LL_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);
    time_t current_time = time(NULL);

    mocks.ResetAllCalls();
    setExpectedCallsForTransportDoWorkUpTo(mocks, STEP_DOWORK_OPEN_CBS, DOWORK_MESSAGERECEIVER_NONE, current_time);
    setExpectedCallsForCbsAuthentication(mocks, current_time);
    setExpectedCallsForCbsAuthTimeoutCheck(mocks, current_time);
    setExpectedCallsForConnectionDoWork(mocks);
    setExpectedCallsForSASTokenExpiryCheck(mocks, current_time);
    EXPECTED_CALL(mocks, messaging_create_source(NULL)).SetReturn(TEST_MESSAGESENDER_SOURCE);
    EXPECTED_CALL(mocks, STRING_c_str(NULL));
    EXPECTED_CALL(mocks, messaging_create_target(NULL)).SetReturn(TEST_MESSAGESENDER_TARGET);
    EXPECTED_CALL(mocks, link_create(NULL, NULL, NULL, NULL, NULL)).SetReturn((LINK_HANDLE)NULL);
    STRICT_EXPECTED_CALL(mocks, amqpvalue_destroy(TEST_MESSAGESENDER_SOURCE));
    STRICT_EXPECTED_CALL(mocks, amqpvalue_destroy(TEST_MESSAGESENDER_TARGET));
    setExpectedCallsForConnectionDestroyUpTo(mocks, STEP_DOWORK_CREATE_CBS);

    // act
    transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);
    test_latest_cbs_put_token_callback(test_latest_cbs_put_token_context, CBS_OPERATION_RESULT_OK, 0, NULL);
    transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // assert
    mocks.AssertActualAndExpectedCalls();

    // cleanup
    transport_interface->IoTHubTransport_Destroy(transport);
}

// Tests_SRS_IOTHUBTRANSPORTAMQP_09_070: [IoTHubTransportAMQP_DoWork shall create the AMQP message sender using messagesender_create() AMQP API] 
// Tests_SRS_IOTHUBTRANSPORTAMQP_09_071: [IoTHubTransportAMQP_DoWork shall fail and return immediately if the AMQP message sender instance fails to be created, flagging the connection to be re-established] 
// Tests_SRS_IOTHUBTRANSPORTAMQP_09_119: [IoTHubTransportAMQP_DoWork shall apply a default value of 65536 for the parameter 'Link MAX message size'] 
TEST_FUNCTION(AMQP_DoWork_messagesender_create_fails)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
        TEST_DEVICE_ID, TEST_DEVICE_KEY, NULL, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME_NULL };
    IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };
    TRANSPORT_LL_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);
    time_t current_time = time(NULL);

    mocks.ResetAllCalls();
    setExpectedCallsForTransportDoWorkUpTo(mocks, STEP_DOWORK_OPEN_CBS, DOWORK_MESSAGERECEIVER_NONE, current_time);
    setExpectedCallsForCbsAuthentication(mocks, current_time);
    setExpectedCallsForCbsAuthTimeoutCheck(mocks, current_time);
    setExpectedCallsForConnectionDoWork(mocks);
    setExpectedCallsForSASTokenExpiryCheck(mocks, current_time);
    EXPECTED_CALL(mocks, messaging_create_source(NULL)).SetReturn(TEST_MESSAGESENDER_SOURCE);
    EXPECTED_CALL(mocks, STRING_c_str(NULL));
    EXPECTED_CALL(mocks, messaging_create_target(NULL)).SetReturn(TEST_MESSAGESENDER_TARGET);
    EXPECTED_CALL(mocks, link_create(NULL, NULL, NULL, NULL, NULL)).SetReturn(TEST_MESSAGESENDER_LINK);
    EXPECTED_CALL(mocks, link_set_max_message_size(IGNORED_PTR_ARG, IGNORED_NUM_ARG));
    EXPECTED_CALL(mocks, amqpvalue_create_map()).SetReturn(TEST_AMQP_VALUE_TEST_HANDLE);
    EXPECTED_CALL(mocks, amqpvalue_create_symbol(NULL)).IgnoreArgument(1).SetReturn(TEST_AMQP_VALUE_TEST_HANDLE);
    EXPECTED_CALL(mocks, amqpvalue_create_string(NULL)).IgnoreArgument(1).SetReturn(TEST_AMQP_VALUE_TEST_HANDLE);
    EXPECTED_CALL(mocks, amqpvalue_set_map_value(NULL, NULL, NULL)).IgnoreAllArguments();
    EXPECTED_CALL(mocks, amqpvalue_destroy(NULL)).IgnoreArgument(1);
    EXPECTED_CALL(mocks, amqpvalue_destroy(NULL)).IgnoreArgument(1);
    EXPECTED_CALL(mocks, amqpvalue_destroy(NULL)).IgnoreArgument(1);
    EXPECTED_CALL(mocks, link_set_attach_properties(NULL, NULL)).IgnoreAllArguments().SetReturn(0);
    EXPECTED_CALL(mocks, messagesender_create(NULL, NULL, NULL)).SetReturn((MESSAGE_SENDER_HANDLE)NULL);
    STRICT_EXPECTED_CALL(mocks, amqpvalue_destroy(TEST_MESSAGESENDER_SOURCE));
    STRICT_EXPECTED_CALL(mocks, amqpvalue_destroy(TEST_MESSAGESENDER_TARGET));
    setExpectedCallsForConnectionDestroyUpTo(mocks, STEP_DOWORK_CREATE_CBS);

    // act
    transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);
    test_latest_cbs_put_token_callback(test_latest_cbs_put_token_context, CBS_OPERATION_RESULT_OK, 0, NULL);
    transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // assert
    mocks.AssertActualAndExpectedCalls();

    // cleanup
    transport_interface->IoTHubTransport_Destroy(transport);
}

// Tests_SRS_IOTHUBTRANSPORTAMQP_09_072: [IoTHubTransportAMQP_DoWork shall open the AMQP message sender using messagesender_open() AMQP API] 
// Tests_SRS_IOTHUBTRANSPORTAMQP_09_073: [IoTHubTransportAMQP_DoWork shall fail and return immediately if the AMQP message sender instance fails to be opened, flagging the connection to be re-established] 
TEST_FUNCTION(AMQP_DoWork_messagesender_open_fails)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
        TEST_DEVICE_ID, TEST_DEVICE_KEY, NULL, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME_NULL };
    IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };
    TRANSPORT_LL_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);
    time_t current_time = time(NULL);

    mocks.ResetAllCalls();
    setExpectedCallsForTransportDoWorkUpTo(mocks, STEP_DOWORK_OPEN_CBS, DOWORK_MESSAGERECEIVER_NONE, current_time);
    setExpectedCallsForCbsAuthentication(mocks, current_time);
    setExpectedCallsForCbsAuthTimeoutCheck(mocks, current_time);
    setExpectedCallsForConnectionDoWork(mocks);
    setExpectedCallsForSASTokenExpiryCheck(mocks, current_time);
    EXPECTED_CALL(mocks, messaging_create_source(NULL)).SetReturn(TEST_MESSAGESENDER_SOURCE);
    EXPECTED_CALL(mocks, STRING_c_str(NULL));
    EXPECTED_CALL(mocks, messaging_create_target(NULL)).SetReturn(TEST_MESSAGESENDER_TARGET);
    EXPECTED_CALL(mocks, link_create(NULL, NULL, NULL, NULL, NULL)).SetReturn(TEST_MESSAGESENDER_LINK);
    EXPECTED_CALL(mocks, link_set_max_message_size(IGNORED_PTR_ARG, IGNORED_NUM_ARG));
    EXPECTED_CALL(mocks, amqpvalue_create_map()).SetReturn(TEST_AMQP_VALUE_TEST_HANDLE);
    EXPECTED_CALL(mocks, amqpvalue_create_symbol(NULL)).IgnoreArgument(1).SetReturn(TEST_AMQP_VALUE_TEST_HANDLE);
    EXPECTED_CALL(mocks, amqpvalue_create_string(NULL)).IgnoreArgument(1).SetReturn(TEST_AMQP_VALUE_TEST_HANDLE);
    EXPECTED_CALL(mocks, amqpvalue_set_map_value(NULL, NULL, NULL)).IgnoreAllArguments();
    EXPECTED_CALL(mocks, link_set_attach_properties(NULL, NULL)).IgnoreAllArguments().SetReturn(0);
    EXPECTED_CALL(mocks, amqpvalue_destroy(NULL)).IgnoreArgument(1);
    EXPECTED_CALL(mocks, amqpvalue_destroy(NULL)).IgnoreArgument(1);
    EXPECTED_CALL(mocks, amqpvalue_destroy(NULL)).IgnoreArgument(1);
    EXPECTED_CALL(mocks, messagesender_create(NULL, NULL, NULL)).SetReturn(TEST_MESSAGE_SENDER);
    EXPECTED_CALL(mocks, messagesender_open(TEST_MESSAGE_SENDER)).SetReturn(1);
    STRICT_EXPECTED_CALL(mocks, amqpvalue_destroy(TEST_MESSAGESENDER_SOURCE));
    STRICT_EXPECTED_CALL(mocks, amqpvalue_destroy(TEST_MESSAGESENDER_TARGET));
    setExpectedCallsForPrepareForConnectionRetry(mocks, false, true);
    setExpectedCallsForConnectionDestroyUpTo(mocks, STEP_DOWORK_CREATE_CBS);

    // act
    transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);
    test_latest_cbs_put_token_callback(test_latest_cbs_put_token_context, CBS_OPERATION_RESULT_OK, 0, NULL);
    transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // assert
    mocks.AssertActualAndExpectedCalls();

    // cleanup
    transport_interface->IoTHubTransport_Destroy(transport);
}

// Tests_SRS_IOTHUBTRANSPORTAMQP_09_074: [IoTHubTransportAMQP_DoWork shall create the AMQP link for receiving messages using 'source' as messageReceiveAddress, target as the "ingress-rx", link name as "receiver-link" and role as 'role_receiver'] 
// Tests_SRS_IOTHUBTRANSPORTAMQP_09_075: [If IoTHubTransportAMQP_DoWork fails to create the AMQP link for receiving messages, the function shall fail and return immediately, flagging the connection to be re-stablished] 
// Tests_SRS_IOTHUBTRANSPORTAMQP_09_038: [IoTHubTransportAMQP_Subscribe shall set transport_handle->receive_messages to true and return success code.]
TEST_FUNCTION(AMQP_DoWork_messagereceiver_source_create_fails)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
        TEST_DEVICE_ID, TEST_DEVICE_KEY, NULL, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME_NULL };
    IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };
    time_t current_time = time(NULL);

    TRANSPORT_LL_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);
    int subscribe_result = transport_interface->IoTHubTransport_Subscribe(transport);
    ASSERT_ARE_EQUAL(int, subscribe_result, 0);

    mocks.ResetAllCalls();
    setExpectedCallsForTransportDoWorkUpTo(mocks, STEP_DOWORK_OPEN_CBS, DOWORK_MESSAGERECEIVER_NONE, current_time);
    setExpectedCallsForCbsAuthentication(mocks, current_time);
    setExpectedCallsForCbsAuthTimeoutCheck(mocks, current_time);
    setExpectedCallsForConnectionDoWork(mocks);
    setExpectedCallsForSASTokenExpiryCheck(mocks, current_time);
    EXPECTED_CALL(mocks, STRING_c_str(NULL));
    EXPECTED_CALL(mocks, messaging_create_source(NULL)).SetReturn((AMQP_VALUE)NULL);
    EXPECTED_CALL(mocks, messaging_create_source(NULL)).SetReturn((AMQP_VALUE)NULL);
    setExpectedCallsForConnectionDestroyUpTo(mocks, STEP_DOWORK_CREATE_CBS);

    // act
    transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);
    test_latest_cbs_put_token_callback(test_latest_cbs_put_token_context, CBS_OPERATION_RESULT_OK, 0, NULL);
    transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // assert
    mocks.AssertActualAndExpectedCalls();

    // cleanup
    transport_interface->IoTHubTransport_Destroy(transport);
}

// Tests_SRS_IOTHUBTRANSPORTAMQP_09_074: [IoTHubTransportAMQP_DoWork shall create the AMQP link for receiving messages using 'source' as messageReceiveAddress, target as the "ingress-rx", link name as "receiver-link" and role as 'role_receiver'] 
// Tests_SRS_IOTHUBTRANSPORTAMQP_09_075: [If IoTHubTransportAMQP_DoWork fails to create the AMQP link for receiving messages, the function shall fail and return immediately, flagging the connection to be re-stablished] 
TEST_FUNCTION(AMQP_DoWork_messagereceiver_target_create_fails)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
        TEST_DEVICE_ID, TEST_DEVICE_KEY, NULL, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME_NULL };
    IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };
    time_t current_time = time(NULL);

    TRANSPORT_LL_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);
    int subscribe_result = transport_interface->IoTHubTransport_Subscribe(transport);
    ASSERT_ARE_EQUAL(int, subscribe_result, 0);

    mocks.ResetAllCalls();
    setExpectedCallsForTransportDoWorkUpTo(mocks, STEP_DOWORK_OPEN_CBS, DOWORK_MESSAGERECEIVER_NONE, current_time);
    setExpectedCallsForCbsAuthentication(mocks, current_time);
    setExpectedCallsForCbsAuthTimeoutCheck(mocks, current_time);
    setExpectedCallsForConnectionDoWork(mocks);
    setExpectedCallsForSASTokenExpiryCheck(mocks, current_time);
    EXPECTED_CALL(mocks, STRING_c_str(NULL));
    EXPECTED_CALL(mocks, messaging_create_source(NULL)).SetReturn(TEST_MESSAGERECEIVER_SOURCE);
    EXPECTED_CALL(mocks, messaging_create_target(NULL)).SetReturn((AMQP_VALUE)NULL);
    STRICT_EXPECTED_CALL(mocks, amqpvalue_destroy(TEST_MESSAGERECEIVER_SOURCE));
    EXPECTED_CALL(mocks, messaging_create_source(NULL)).SetReturn((AMQP_VALUE)NULL);
    setExpectedCallsForConnectionDestroyUpTo(mocks, STEP_DOWORK_CREATE_CBS);

    // act
    transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);
    test_latest_cbs_put_token_callback(test_latest_cbs_put_token_context, CBS_OPERATION_RESULT_OK, 0, NULL);
    transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // assert
    mocks.AssertActualAndExpectedCalls();

    // cleanup
    transport_interface->IoTHubTransport_Destroy(transport);
}

// Tests_SRS_IOTHUBTRANSPORTAMQP_09_074: [IoTHubTransportAMQP_DoWork shall create the AMQP link for receiving messages using 'source' as messageReceiveAddress, target as the "ingress-rx", link name as "receiver-link" and role as 'role_receiver'] 
// Tests_SRS_IOTHUBTRANSPORTAMQP_09_075: [If IoTHubTransportAMQP_DoWork fails to create the AMQP link for receiving messages, the function shall fail and return immediately, flagging the connection to be re-stablished] 
TEST_FUNCTION(AMQP_DoWork_messagereceiver_link_create_fails)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
        TEST_DEVICE_ID, TEST_DEVICE_KEY, NULL, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME_NULL };
    IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };
    time_t current_time = time(NULL);

    TRANSPORT_LL_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);
    int subscribe_result = transport_interface->IoTHubTransport_Subscribe(transport);
    ASSERT_ARE_EQUAL(int, subscribe_result, 0);

    mocks.ResetAllCalls();
    setExpectedCallsForTransportDoWorkUpTo(mocks, STEP_DOWORK_OPEN_CBS, DOWORK_MESSAGERECEIVER_NONE, current_time);
    setExpectedCallsForCbsAuthentication(mocks, current_time);
    setExpectedCallsForCbsAuthTimeoutCheck(mocks, current_time);
    setExpectedCallsForConnectionDoWork(mocks);
    setExpectedCallsForSASTokenExpiryCheck(mocks, current_time);
    EXPECTED_CALL(mocks, STRING_c_str(NULL));
    EXPECTED_CALL(mocks, messaging_create_source(NULL)).SetReturn(TEST_MESSAGERECEIVER_SOURCE);
    EXPECTED_CALL(mocks, messaging_create_target(NULL)).SetReturn(TEST_MESSAGERECEIVER_TARGET);
    EXPECTED_CALL(mocks, link_create(NULL, NULL, NULL, NULL, NULL)).SetReturn((LINK_HANDLE)NULL);
    STRICT_EXPECTED_CALL(mocks, amqpvalue_destroy(TEST_MESSAGERECEIVER_SOURCE));
    STRICT_EXPECTED_CALL(mocks, amqpvalue_destroy(TEST_MESSAGERECEIVER_TARGET));
    EXPECTED_CALL(mocks, messaging_create_source(NULL)).SetReturn((AMQP_VALUE)NULL);
    setExpectedCallsForConnectionDestroyUpTo(mocks, STEP_DOWORK_CREATE_CBS);

    // act
    transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);
    test_latest_cbs_put_token_callback(test_latest_cbs_put_token_context, CBS_OPERATION_RESULT_OK, 0, NULL);
    transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // assert
    mocks.AssertActualAndExpectedCalls();

    // cleanup
    transport_interface->IoTHubTransport_Destroy(transport);
}

// Tests_SRS_IOTHUBTRANSPORTAMQP_09_076: [IoTHubTransportAMQP_DoWork shall set the receiver link settle mode as receiver_settle_mode_first] 
// Tests_SRS_IOTHUBTRANSPORTAMQP_09_141: [If IoTHubTransportAMQP_DoWork fails to set the settle mode on the AMQP link for receiving messages, the function shall fail and return immediately, flagging the connection to be re-stablished] 
TEST_FUNCTION(AMQP_DoWork_messagereceiver_settle_mode_fails)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
        TEST_DEVICE_ID, TEST_DEVICE_KEY, NULL, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME_NULL };
    IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };
    time_t current_time = time(NULL);

    TRANSPORT_LL_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);
    int subscribe_result = transport_interface->IoTHubTransport_Subscribe(transport);
    ASSERT_ARE_EQUAL(int, subscribe_result, 0);

    mocks.ResetAllCalls();
    setExpectedCallsForTransportDoWorkUpTo(mocks, STEP_DOWORK_OPEN_CBS, DOWORK_MESSAGERECEIVER_NONE, current_time);
    setExpectedCallsForCbsAuthentication(mocks, current_time);
    setExpectedCallsForCbsAuthTimeoutCheck(mocks, current_time);
    setExpectedCallsForConnectionDoWork(mocks);
    setExpectedCallsForSASTokenExpiryCheck(mocks, current_time);
    EXPECTED_CALL(mocks, STRING_c_str(NULL));
    EXPECTED_CALL(mocks, messaging_create_source(NULL)).SetReturn(TEST_MESSAGERECEIVER_SOURCE);
    EXPECTED_CALL(mocks, messaging_create_target(NULL)).SetReturn(TEST_MESSAGERECEIVER_TARGET);
    EXPECTED_CALL(mocks, link_create(NULL, NULL, NULL, NULL, NULL)).SetReturn(TEST_MESSAGERECEIVER_LINK);
    STRICT_EXPECTED_CALL(mocks, link_set_rcv_settle_mode(TEST_MESSAGERECEIVER_LINK, receiver_settle_mode_first)).SetReturn(1);
    STRICT_EXPECTED_CALL(mocks, amqpvalue_destroy(TEST_MESSAGERECEIVER_SOURCE));
    STRICT_EXPECTED_CALL(mocks, amqpvalue_destroy(TEST_MESSAGERECEIVER_TARGET));
    EXPECTED_CALL(mocks, messaging_create_source(NULL)).SetReturn((AMQP_VALUE)NULL);
    setExpectedCallsForConnectionDestroyUpTo(mocks, STEP_DOWORK_CREATE_CBS);

    // act
    transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);
    test_latest_cbs_put_token_callback(test_latest_cbs_put_token_context, CBS_OPERATION_RESULT_OK, 0, NULL);
    transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // assert
    mocks.AssertActualAndExpectedCalls();

    // cleanup
    transport_interface->IoTHubTransport_Destroy(transport);
}

// Tests_SRS_IOTHUBTRANSPORTAMQP_09_077: [IoTHubTransportAMQP_DoWork shall create the AMQP message receiver using messagereceiver_create() AMQP API] 
// Tests_SRS_IOTHUBTRANSPORTAMQP_09_078: [IoTHubTransportAMQP_DoWork shall fail and return immediately if the AMQP message receiver instance fails to be created, flagging the connection to be re-established] 
// Tests_SRS_IOTHUBTRANSPORTAMQP_09_119: [IoTHubTransportAMQP_DoWork shall apply a default value of 65536 for the parameter 'Link MAX message size'] 
TEST_FUNCTION(AMQP_DoWork_messagereceiver_create_fails)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
        TEST_DEVICE_ID, TEST_DEVICE_KEY, NULL, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME_NULL };
    IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };
    time_t current_time = time(NULL);

    TRANSPORT_LL_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);
    int subscribe_result = transport_interface->IoTHubTransport_Subscribe(transport);
    ASSERT_ARE_EQUAL(int, subscribe_result, 0);

    mocks.ResetAllCalls();
    setExpectedCallsForTransportDoWorkUpTo(mocks, STEP_DOWORK_OPEN_CBS, DOWORK_MESSAGERECEIVER_NONE, current_time);
    setExpectedCallsForCbsAuthentication(mocks, current_time);
    setExpectedCallsForCbsAuthTimeoutCheck(mocks, current_time);
    setExpectedCallsForConnectionDoWork(mocks);
    setExpectedCallsForSASTokenExpiryCheck(mocks, current_time);
    EXPECTED_CALL(mocks, STRING_c_str(NULL));
    EXPECTED_CALL(mocks, messaging_create_source(NULL)).SetReturn(TEST_MESSAGERECEIVER_SOURCE);
    EXPECTED_CALL(mocks, messaging_create_target(NULL)).SetReturn(TEST_MESSAGERECEIVER_TARGET);
    EXPECTED_CALL(mocks, link_create(NULL, NULL, NULL, NULL, NULL)).SetReturn(TEST_MESSAGERECEIVER_LINK);
    STRICT_EXPECTED_CALL(mocks, link_set_rcv_settle_mode(TEST_MESSAGERECEIVER_LINK, receiver_settle_mode_first));
    EXPECTED_CALL(mocks, link_set_max_message_size(IGNORED_PTR_ARG, IGNORED_NUM_ARG));
    EXPECTED_CALL(mocks, amqpvalue_create_map()).SetReturn(TEST_AMQP_VALUE_TEST_HANDLE);
    EXPECTED_CALL(mocks, amqpvalue_create_symbol(NULL)).IgnoreArgument(1).SetReturn(TEST_AMQP_VALUE_TEST_HANDLE);
    EXPECTED_CALL(mocks, amqpvalue_create_string(NULL)).IgnoreArgument(1).SetReturn(TEST_AMQP_VALUE_TEST_HANDLE);
    EXPECTED_CALL(mocks, amqpvalue_set_map_value(NULL, NULL, NULL)).IgnoreAllArguments();
    EXPECTED_CALL(mocks, link_set_attach_properties(NULL, NULL)).IgnoreAllArguments().SetReturn(0);
    EXPECTED_CALL(mocks, amqpvalue_destroy(NULL)).IgnoreArgument(1);
    EXPECTED_CALL(mocks, amqpvalue_destroy(NULL)).IgnoreArgument(1);
    EXPECTED_CALL(mocks, amqpvalue_destroy(NULL)).IgnoreArgument(1);
    EXPECTED_CALL(mocks, messagereceiver_create(NULL, NULL, NULL)).SetReturn(TEST_MESSAGE_RECEIVER).SetReturn((MESSAGE_RECEIVER_HANDLE)NULL);
    STRICT_EXPECTED_CALL(mocks, amqpvalue_destroy(TEST_MESSAGERECEIVER_SOURCE));
    STRICT_EXPECTED_CALL(mocks, amqpvalue_destroy(TEST_MESSAGERECEIVER_TARGET));
    EXPECTED_CALL(mocks, messaging_create_source(NULL)).SetReturn((AMQP_VALUE)NULL);
    setExpectedCallsForConnectionDestroyUpTo(mocks, STEP_DOWORK_CREATE_CBS);

    // act
    transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);
    test_latest_cbs_put_token_callback(test_latest_cbs_put_token_context, CBS_OPERATION_RESULT_OK, 0, NULL);
    transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // assert
    mocks.AssertActualAndExpectedCalls();

    // cleanup
    transport_interface->IoTHubTransport_Destroy(transport);
}

// Tests_SRS_IOTHUBTRANSPORTAMQP_09_079: [IoTHubTransportAMQP_DoWork shall open the AMQP message receiver using messagereceiver_open() AMQP API, passing a callback function for handling C2D incoming messages] 
// Tests_SRS_IOTHUBTRANSPORTAMQP_09_080: [IoTHubTransportAMQP_DoWork shall fail and return immediately if the AMQP message receiver instance fails to be opened, flagging the connection to be re-established] 
TEST_FUNCTION(AMQP_DoWork_messagereceiver_open_fails)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
        TEST_DEVICE_ID, TEST_DEVICE_KEY, NULL, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME_NULL };
    IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };
    time_t current_time = time(NULL);

    TRANSPORT_LL_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);
    int subscribe_result = transport_interface->IoTHubTransport_Subscribe(transport);
    ASSERT_ARE_EQUAL(int, subscribe_result, 0);

    mocks.ResetAllCalls();
    setExpectedCallsForTransportDoWorkUpTo(mocks, STEP_DOWORK_OPEN_CBS, DOWORK_MESSAGERECEIVER_NONE, current_time);
    setExpectedCallsForCbsAuthentication(mocks, current_time);
    setExpectedCallsForCbsAuthTimeoutCheck(mocks, current_time);
    setExpectedCallsForConnectionDoWork(mocks);
    setExpectedCallsForSASTokenExpiryCheck(mocks, current_time);
    EXPECTED_CALL(mocks, STRING_c_str(NULL));
    EXPECTED_CALL(mocks, messaging_create_source(NULL)).SetReturn(TEST_MESSAGERECEIVER_SOURCE);
    EXPECTED_CALL(mocks, messaging_create_target(NULL)).SetReturn(TEST_MESSAGERECEIVER_TARGET);
    EXPECTED_CALL(mocks, link_create(NULL, NULL, NULL, NULL, NULL)).SetReturn(TEST_MESSAGERECEIVER_LINK);
    STRICT_EXPECTED_CALL(mocks, link_set_rcv_settle_mode(TEST_MESSAGERECEIVER_LINK, receiver_settle_mode_first));
    EXPECTED_CALL(mocks, link_set_max_message_size(IGNORED_PTR_ARG, IGNORED_NUM_ARG));
    EXPECTED_CALL(mocks, amqpvalue_create_map()).SetReturn(TEST_AMQP_VALUE_TEST_HANDLE);
    EXPECTED_CALL(mocks, amqpvalue_create_symbol(NULL)).IgnoreArgument(1).SetReturn(TEST_AMQP_VALUE_TEST_HANDLE);
    EXPECTED_CALL(mocks, amqpvalue_create_string(NULL)).IgnoreArgument(1).SetReturn(TEST_AMQP_VALUE_TEST_HANDLE);
    EXPECTED_CALL(mocks, amqpvalue_set_map_value(NULL, NULL, NULL)).IgnoreAllArguments();
    EXPECTED_CALL(mocks, link_set_attach_properties(NULL, NULL)).IgnoreAllArguments().SetReturn(0);
    EXPECTED_CALL(mocks, amqpvalue_destroy(NULL)).IgnoreArgument(1);
    EXPECTED_CALL(mocks, amqpvalue_destroy(NULL)).IgnoreArgument(1);
    EXPECTED_CALL(mocks, amqpvalue_destroy(NULL)).IgnoreArgument(1);
    EXPECTED_CALL(mocks, messagereceiver_create(NULL, NULL, NULL)).SetReturn(TEST_MESSAGE_RECEIVER);
    EXPECTED_CALL(mocks, messagereceiver_open(TEST_MESSAGE_RECEIVER, NULL, NULL)).SetReturn(1);
    STRICT_EXPECTED_CALL(mocks, amqpvalue_destroy(TEST_MESSAGERECEIVER_SOURCE));
    STRICT_EXPECTED_CALL(mocks, amqpvalue_destroy(TEST_MESSAGERECEIVER_TARGET));
    EXPECTED_CALL(mocks, messaging_create_source(NULL)).SetReturn((AMQP_VALUE)NULL);
    setExpectedCallsForPrepareForConnectionRetry(mocks, true, false);
    setExpectedCallsForConnectionDestroyUpTo(mocks, STEP_DOWORK_CREATE_CBS);

    // act
    transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);
    test_latest_cbs_put_token_callback(test_latest_cbs_put_token_context, CBS_OPERATION_RESULT_OK, 0, NULL);
    transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // assert
    mocks.AssertActualAndExpectedCalls();

    // cleanup
    transport_interface->IoTHubTransport_Destroy(transport);
}

// Tests_SRS_IOTHUBTRANSPORTAMQP_09_055: [If the transport handle has a NULL connection, IoTHubTransportAMQP_DoWork shall instantiate and initialize the AMQP components and establish the connection] 
// Tests_SRS_IOTHUBTRANSPORTAMQP_09_082: [IoTHubTransportAMQP_DoWork shall refresh the SAS token if the current token has been used for more than 'sas_token_refresh_time' milliseconds]
// Tests_SRS_IOTHUBTRANSPORTAMQP_09_128: [IoTHubTransportAMQP_Create shall set parameter transport_state->sas_token_refresh_time with the default value of sas_token_lifetime/2 (milliseconds).] 
TEST_FUNCTION(AMQP_DoWork_expired_SASToken_fails)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
        TEST_DEVICE_ID, TEST_DEVICE_KEY, NULL, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME_NULL };
    IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };
    time_t current_time = time(NULL);
    time_t expiration_time = addSecondsToTime(current_time, (TEST_SAS_TOKEN_LIFETIME_MS / 2) / 1000 + 1);

    TRANSPORT_LL_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);
    int subscribe_result = transport_interface->IoTHubTransport_Subscribe(transport);
    ASSERT_ARE_EQUAL(int, subscribe_result, 0);

    mocks.ResetAllCalls();
    setExpectedCallsForTransportDoWorkUpTo(mocks, STEP_DOWORK_OPEN_CBS, DOWORK_MESSAGERECEIVER_NONE, current_time);
    setExpectedCallsForCbsAuthentication(mocks, current_time);
    setExpectedCallsForCbsAuthTimeoutCheck(mocks, current_time);
    setExpectedCallsForConnectionDoWork(mocks);
    setExpectedCallsForSASTokenExpiryCheck(mocks, expiration_time);
	setExpectedCallsForGetSecondsSinceEpoch(mocks, current_time);
    EXPECTED_CALL(mocks, SASToken_Create(NULL, NULL, NULL, 0)).SetReturn((STRING_HANDLE)NULL);
    setExpectedCallsForConnectionDestroyUpTo(mocks, STEP_DOWORK_CREATE_CBS);

    // act
    transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);
    test_latest_cbs_put_token_callback(test_latest_cbs_put_token_context, CBS_OPERATION_RESULT_OK, 0, NULL);
    transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // assert
    mocks.AssertActualAndExpectedCalls();

    // cleanup
    transport_interface->IoTHubTransport_Destroy(transport);
}

// Tests_SRS_IOTHUBTRANSPORTAMQP_09_086: [IoTHubTransportAMQP_DoWork shall move queued events to an "in-progress" list right before processing them for sending]
// Tests_SRS_IOTHUBTRANSPORTAMQP_09_193: [IoTHubTransportAMQP_DoWork shall get a MESSAGE_HANDLE instance out of the event's IOTHUB_MESSAGE_HANDLE instance by using message_create_from_iothub_message().]
// Tests_SRS_IOTHUBTRANSPORTAMQP_09_194: [IoTHubTransportAMQP_DoWork shall destroy the MESSAGE_HANDLE instance after messagesender_send() is invoked.]
TEST_FUNCTION(AMQP_DoWork_succeeds_when_2_waiting_to_send_messages_are_in_the_list)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
        TEST_DEVICE_ID, TEST_DEVICE_KEY, NULL, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME_NULL };
    IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };
    time_t current_time = time(NULL);
    
    TRANSPORT_LL_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);

    addTestEvents(config.waitingToSend, 2, true);

    mocks.ResetAllCalls();
    setExpectedCallsForTransportDoWorkUpTo(mocks, STEP_DOWORK_OPEN_CBS, DOWORK_MESSAGERECEIVER_NONE, current_time);
    setExpectedCallsForCbsAuthentication(mocks, current_time);
    setExpectedCallsForCbsAuthTimeoutCheck(mocks, current_time);
    setExpectedCallsForConnectionDoWork(mocks);
    setExpectedCallsForSASTokenExpiryCheck(mocks, current_time);
    setExpectedCallsForCreateEventSender(mocks);
    setExpectedCallsForSendPendingEvents(mocks, IOTHUBMESSAGE_STRING, 2);
    setExpectedCallsForConnectionDoWork(mocks);

    // act
    transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);
    test_latest_cbs_put_token_callback(test_latest_cbs_put_token_context, CBS_OPERATION_RESULT_OK, 0, NULL);
    transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // assert
    mocks.AssertActualAndExpectedCalls();

    // cleanup
    transport_interface->IoTHubTransport_Destroy(transport);
    cleanupList(config.waitingToSend);
}

TEST_FUNCTION(AMQP_send_pending_events_parse_iothub_message_handle_fails)
{
	// arrange
	CIoTHubTransportAMQPMocks mocks;

	DLIST_ENTRY wts;
	BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
	TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();
	IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
        TEST_DEVICE_ID, TEST_DEVICE_KEY, NULL, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME_NULL };
	IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };
	time_t current_time = time(NULL);

	TRANSPORT_LL_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);

	addTestEvents(config.waitingToSend, 1, true);

	mocks.ResetAllCalls();
	setExpectedCallsForTransportDoWorkUpTo(mocks, STEP_DOWORK_OPEN_CBS, DOWORK_MESSAGERECEIVER_NONE, current_time);
	setExpectedCallsForCbsAuthentication(mocks, current_time);
	setExpectedCallsForCbsAuthTimeoutCheck(mocks, current_time);
	setExpectedCallsForConnectionDoWork(mocks);
	setExpectedCallsForSASTokenExpiryCheck(mocks, current_time);
	setExpectedCallsForCreateEventSender(mocks);

	EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).SetReturn(0);
	EXPECTED_CALL(mocks, DList_RemoveEntryList(IGNORED_PTR_ARG));
	EXPECTED_CALL(mocks, DList_InsertTailList(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
	STRICT_EXPECTED_CALL(mocks, message_create_from_iothub_message(TEST_IOTHUB_MESSAGE_HANDLE, IGNORED_PTR_ARG)).IgnoreArgument(2).SetReturn(1);
	STRICT_EXPECTED_CALL(mocks, message_destroy(TEST_MESSAGE_HANDLE)); // mocked function still assigns the MESSAGE_HANDLE, so need to expect a destroy.
	STRICT_EXPECTED_CALL(mocks, test_iothubclient_send_confirmation_callback(IOTHUB_CLIENT_CONFIRMATION_ERROR, 0));
	EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).SetReturn(0);
	EXPECTED_CALL(mocks, DList_RemoveEntryList(IGNORED_PTR_ARG));
	EXPECTED_CALL(mocks, DList_InitializeListHead(IGNORED_PTR_ARG));
	STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Destroy(TEST_IOTHUB_MESSAGE_HANDLE));
	EXPECTED_CALL(mocks, gballoc_free(NULL));
	EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).SetReturn(1);

	setExpectedCallsForConnectionDoWork(mocks);

	// act
	transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);
	test_latest_cbs_put_token_callback(test_latest_cbs_put_token_context, CBS_OPERATION_RESULT_OK, 0, NULL);
	transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);

	// assert
	mocks.AssertActualAndExpectedCalls();

	// cleanup
	transport_interface->IoTHubTransport_Destroy(transport);
	cleanupList(config.waitingToSend);
}

// Tests_SRS_IOTHUBTRANSPORTAMQP_09_191: [IoTHubTransportAMQP_DoWork shall create each AMQP message sender tracking its state changes with a callback function]
// Tests_SRS_IOTHUBTRANSPORTAMQP_09_192: [If a message sender instance changes its state to MESSAGE_SENDER_STATE_ERROR (first transition only) the connection retry logic shall be triggered]
TEST_FUNCTION(AMQP_messagesender_ERROR_state_change_triggers_reconnection)
{
    // arrange
    resetTestSuiteState();

    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
        TEST_DEVICE_ID, TEST_DEVICE_KEY, NULL, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME_NULL };
    IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };
    time_t current_time = time(NULL);

    TRANSPORT_LL_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);

    setupSuccessfulDoWorkAndAuthenticate(transport, mocks, current_time);

    mocks.ResetAllCalls();
    setExpectedCallsForConnectionDestroyUpTo(mocks, STEP_DOWORK_CREATE_CBS);
    setExpectedCallsForDestroyEventSender(mocks);

    // act
    saved_on_message_sender_state_changed_callback(saved_on_message_sender_state_changed_context, MESSAGE_SENDER_STATE_ERROR, MESSAGE_SENDER_STATE_OPEN);

    transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // assert
    mocks.AssertActualAndExpectedCalls();

    // cleanup
    transport_interface->IoTHubTransport_Destroy(transport);
}

// Tests_SRS_IOTHUBTRANSPORTAMQP_09_189: [IoTHubTransportAMQP_DoWork shall create each AMQP message receiver tracking its state changes with a callback function]
// Tests_SRS_IOTHUBTRANSPORTAMQP_09_190: [If a message receiver instance changes its state to MESSAGE_RECEIVER_STATE_ERROR (first transition only) the connection retry logic shall be triggered]
TEST_FUNCTION(AMQP_messagereceiver_ERROR_state_change_triggers_reconnection)
{
    // arrange
    resetTestSuiteState();

    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
        TEST_DEVICE_ID, TEST_DEVICE_KEY, NULL, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME_NULL };
    IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };
    time_t current_time = time(NULL);

    TRANSPORT_LL_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);

    transport_interface->IoTHubTransport_Subscribe(transport);
    setExpectedCallsForCreateMessageReceiver(mocks);
    setupSuccessfulDoWorkAndAuthenticate(transport, mocks, current_time);

    mocks.ResetAllCalls();
    setExpectedCallsForConnectionDestroyUpTo(mocks, STEP_DOWORK_CREATE_CBS);
    setExpectedCallsForDestroyEventSender(mocks);
    setExpectedCallsForDestroyMessageReceiver(mocks);

    // act
    saved_on_message_receiver_state_changed_callback(saved_on_message_receiver_state_changed_context, MESSAGE_RECEIVER_STATE_ERROR, MESSAGE_RECEIVER_STATE_OPEN);

    transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // assert
    mocks.AssertActualAndExpectedCalls();

    // cleanup
    transport_interface->IoTHubTransport_Destroy(transport);
}


        TEST_DEVICE_ID, TEST_DEVICE_KEY, NULL, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME };

// Codes_SRS_IOTHUBTRANSPORTAMQP_09_041: [IoTHubTransportAMQP_GetSendStatus shall return IOTHUB_CLIENT_INVALID_ARG if called with NULL parameter.] 
// Codes_SRS_IOTHUBTRANSPORTAMQP_09_042: [IoTHubTransportAMQP_GetSendStatus shall return IOTHUB_CLIENT_OK and status IOTHUB_CLIENT_SEND_STATUS_IDLE if there are currently no event items to be sent or being sent.]
TEST_FUNCTION(AMQP_GetSendStatus_idle_succeeds)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
        TEST_DEVICE_ID, TEST_DEVICE_KEY, NULL, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME_NULL };
    IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };

    TRANSPORT_LL_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);

    IOTHUB_CLIENT_STATUS iotHubClientStatus;

    mocks.ResetAllCalls();
    EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).SetReturn(1);
    EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).SetReturn(1);

    // act
    IOTHUB_CLIENT_RESULT result = transport_interface->IoTHubTransport_GetSendStatus(transport, &iotHubClientStatus);

    // assert
    mocks.AssertActualAndExpectedCalls();
    ASSERT_ARE_EQUAL_WITH_MSG(IOTHUB_CLIENT_RESULT, result, IOTHUB_CLIENT_OK, "IoTHubTransport_GetSendStatus returned unexpected result.");
    ASSERT_ARE_EQUAL_WITH_MSG(IOTHUB_CLIENT_STATUS, iotHubClientStatus, IOTHUB_CLIENT_SEND_STATUS_IDLE, "IoTHubTransport_GetSendStatus returned unexpected status.");

    // cleanup
    transport_interface->IoTHubTransport_Destroy(transport);
}

// Tests_SRS_IOTHUBTRANSPORTAMQP_09_041: [IoTHubTransportAMQP_GetSendStatus shall return IOTHUB_CLIENT_INVALID_ARG if called with NULL parameter.]  
// Tests_SRS_IOTHUBTRANSPORTAMQP_09_043: [IoTHubTransportAMQP_GetSendStatus shall return IOTHUB_CLIENT_OK and status IOTHUB_CLIENT_SEND_STATUS_BUSY if there are currently event items to be sent or being sent.] 
TEST_FUNCTION(AMQP_GetSendStatus_inProgress_not_empty_succeeds)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
        TEST_DEVICE_ID, TEST_DEVICE_KEY, NULL, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME_NULL };
    IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };

    TRANSPORT_LL_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);

    IOTHUB_CLIENT_STATUS iotHubClientStatus;

    mocks.ResetAllCalls();
    EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).SetReturn(1);
    EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).SetReturn(0);

    // act
    IOTHUB_CLIENT_RESULT result = transport_interface->IoTHubTransport_GetSendStatus(transport, &iotHubClientStatus);

    // assert
    mocks.AssertActualAndExpectedCalls();
    ASSERT_ARE_EQUAL_WITH_MSG(IOTHUB_CLIENT_RESULT, result, IOTHUB_CLIENT_OK, "IoTHubTransport_GetSendStatus returned unexpected result.");
    ASSERT_ARE_EQUAL_WITH_MSG(IOTHUB_CLIENT_STATUS, iotHubClientStatus, IOTHUB_CLIENT_SEND_STATUS_BUSY, "IoTHubTransport_GetSendStatus returned unexpected status.");

    // cleanup
    transport_interface->IoTHubTransport_Destroy(transport);
}

// Tests_SRS_IOTHUBTRANSPORTAMQP_09_041: [IoTHubTransportAMQP_GetSendStatus shall return IOTHUB_CLIENT_INVALID_ARG if called with NULL parameter.]  
// Tests_SRS_IOTHUBTRANSPORTAMQP_09_043: [IoTHubTransportAMQP_GetSendStatus shall return IOTHUB_CLIENT_OK and status IOTHUB_CLIENT_SEND_STATUS_BUSY if there are currently event items to be sent or being sent.] 
TEST_FUNCTION(AMQP_GetSendStatus_waitingToSend_not_empty_succeeds)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
        TEST_DEVICE_ID, TEST_DEVICE_KEY, NULL, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME_NULL };
    IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };

    TRANSPORT_LL_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);

    IOTHUB_CLIENT_STATUS iotHubClientStatus;

    mocks.ResetAllCalls();
    EXPECTED_CALL(mocks, DList_IsListEmpty(IGNORED_PTR_ARG)).SetReturn(0);

    // act
    IOTHUB_CLIENT_RESULT result = transport_interface->IoTHubTransport_GetSendStatus(transport, &iotHubClientStatus);

    // assert
    mocks.AssertActualAndExpectedCalls();
    ASSERT_ARE_EQUAL_WITH_MSG(IOTHUB_CLIENT_RESULT, result, IOTHUB_CLIENT_OK, "IoTHubTransport_GetSendStatus returned unexpected result.");
    ASSERT_ARE_EQUAL_WITH_MSG(IOTHUB_CLIENT_STATUS, iotHubClientStatus, IOTHUB_CLIENT_SEND_STATUS_BUSY, "IoTHubTransport_GetSendStatus returned unexpected status.");

    // cleanup
    transport_interface->IoTHubTransport_Destroy(transport);
}

// Tests_SRS_IOTHUBTRANSPORTAMQP_09_037: [IoTHubTransportAMQP_Subscribe shall fail if the transport handle parameter received is NULL.]
TEST_FUNCTION(AMQP_Subscribe_NULL_transport_fails)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
        TEST_DEVICE_ID, TEST_DEVICE_KEY, NULL, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME_NULL };
    IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };
    TRANSPORT_LL_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);

    mocks.ResetAllCalls();

    // act
    transport_interface->IoTHubTransport_Subscribe(NULL);

    // assert
    mocks.AssertActualAndExpectedCalls();

    // cleanup
    transport_interface->IoTHubTransport_Destroy(transport);
}

// Tests_SRS_IOTHUBTRANSPORTAMQP_09_121: [IoTHubTransportAMQP_DoWork shall create an AMQP message_receiver if transport_state->message_receive is NULL and transport_state->receive_messages is true]
// Tests_SRS_IOTHUBTRANSPORTAMQP_09_123: [IoTHubTransportAMQP_DoWork shall create each AMQP message_receiver passing the 'on_message_received' as the callback function]
TEST_FUNCTION(AMQP_Subscribe_and_messagereceiver_create_succeeds)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
        TEST_DEVICE_ID, TEST_DEVICE_KEY, NULL, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME_NULL };
    IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };
    time_t current_time = time(NULL);
    int subscribe_result;

    resetTestSuiteState();

    TRANSPORT_LL_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);

    mocks.ResetAllCalls();
    setupSuccessfulDoWorkAndAuthenticate(transport, mocks, current_time);
    setupSuccessfulDoWork(mocks, config, current_time, MESSAGERECEIVER_CREATE);

    // act
    subscribe_result = transport_interface->IoTHubTransport_Subscribe(transport);
    transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // assert
    mocks.AssertActualAndExpectedCalls();
    ASSERT_ARE_EQUAL(int, subscribe_result, 0);
    ASSERT_IS_NOT_NULL(saved_on_message_received_callback);
    ASSERT_ARE_EQUAL(void_ptr, (void_ptr)saved_on_message_received_context, (void_ptr)TEST_IOTHUB_CLIENT_LL_HANDLE);

    // cleanup
    transport_interface->IoTHubTransport_Destroy(transport);
    cleanupList(config.waitingToSend);
}

// Tests_SRS_IOTHUBTRANSPORTAMQP_09_039: [IoTHubTransportAMQP_Unsubscribe shall fail if the transport handle parameter received is NULL.]
TEST_FUNCTION(AMQP_Unsubscribe_NULL_transport_fails)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
        TEST_DEVICE_ID, TEST_DEVICE_KEY, NULL, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME_NULL };
    IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };
    TRANSPORT_LL_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);

    mocks.ResetAllCalls();

    // act
    transport_interface->IoTHubTransport_Unsubscribe(NULL);

    // assert
    mocks.AssertActualAndExpectedCalls();

    // cleanup
    transport_interface->IoTHubTransport_Destroy(transport);
}

// Tests_SRS_IOTHUBTRANSPORTAMQP_09_195: [The callback 'on_message_received' shall shall get a IOTHUB_MESSAGE_HANDLE instance out of the uamqp's MESSAGE_HANDLE instance by using IoTHubMessage_CreateFromUamqpMessage()]
// Tests_SRS_IOTHUBTRANSPORTAMQP_09_197: [The callback 'on_message_received' shall destroy the IOTHUB_MESSAGE_HANDLE instance after invoking IoTHubClient_LL_MessageCallback().]
TEST_FUNCTION(AMQP_on_message_received_callback_succeeds)
{
	// arrange
	CIoTHubTransportAMQPMocks mocks;

	DLIST_ENTRY wts;
	BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
	TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();
	IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
		TEST_DEVICE_ID, TEST_DEVICE_KEY, NULL, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME_NULL };
	IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };
	time_t current_time = time(NULL);
	int subscribe_result;

	resetTestSuiteState();

	TRANSPORT_LL_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);

	mocks.ResetAllCalls();
	setupSuccessfulDoWorkAndAuthenticate(transport, mocks, current_time);
	setupSuccessfulDoWork(mocks, config, current_time, MESSAGERECEIVER_CREATE);

	subscribe_result = transport_interface->IoTHubTransport_Subscribe(transport);
	transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);

	mocks.ResetAllCalls();

	STRICT_EXPECTED_CALL(mocks, IoTHubMessage_CreateFromUamqpMessage(TEST_MESSAGE_HANDLE, IGNORED_PTR_ARG)).IgnoreArgument(2).SetReturn(0);
	STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_MessageCallback(TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_IOTHUB_MESSAGE_HANDLE)).SetReturn(IOTHUBMESSAGE_ACCEPTED);
	STRICT_EXPECTED_CALL(mocks, messaging_delivery_accepted()).SetReturn(TEST_AMQP_DISPOSITION_ACCEPTED);
	STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Destroy(TEST_IOTHUB_MESSAGE_HANDLE));

	// act
	AMQP_VALUE disposition_result = saved_on_message_received_callback(saved_on_message_received_context, TEST_MESSAGE_HANDLE);

	// assert
	mocks.AssertActualAndExpectedCalls();

	ASSERT_ARE_EQUAL_WITH_MSG(void_ptr, (void*)disposition_result, (void*)TEST_AMQP_DISPOSITION_ACCEPTED, "Expected ACCEPTED disposition");

	// cleanup
	transport_interface->IoTHubTransport_Destroy(transport);
	cleanupList(config.waitingToSend);
}

// Tests_SRS_IOTHUBTRANSPORTAMQP_09_196: [If IoTHubMessage_CreateFromUamqpMessage fails, the callback 'on_message_received' shall reject the incoming message by calling messaging_delivery_rejected() and return.]
TEST_FUNCTION(AMQP_on_message_received_callback_uamqp_message_parse_fails)
{
	// arrange
	CIoTHubTransportAMQPMocks mocks;

	DLIST_ENTRY wts;
	BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
	TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();
	IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
		TEST_DEVICE_ID, TEST_DEVICE_KEY, NULL, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME_NULL };
	IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };
	time_t current_time = time(NULL);
	int subscribe_result;

	resetTestSuiteState();

	TRANSPORT_LL_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);

	mocks.ResetAllCalls();
	setupSuccessfulDoWorkAndAuthenticate(transport, mocks, current_time);
	setupSuccessfulDoWork(mocks, config, current_time, MESSAGERECEIVER_CREATE);

	subscribe_result = transport_interface->IoTHubTransport_Subscribe(transport);
	transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);

	mocks.ResetAllCalls();

	STRICT_EXPECTED_CALL(mocks, IoTHubMessage_CreateFromUamqpMessage(TEST_MESSAGE_HANDLE, IGNORED_PTR_ARG)).IgnoreArgument(2).SetReturn(1);
	EXPECTED_CALL(mocks, messaging_delivery_rejected(IGNORED_PTR_ARG, IGNORED_PTR_ARG)).SetReturn(TEST_AMQP_DISPOSITION_REJECTED);

	// act
	AMQP_VALUE disposition_result = saved_on_message_received_callback(saved_on_message_received_context, TEST_MESSAGE_HANDLE);

	// assert
	mocks.AssertActualAndExpectedCalls();
	ASSERT_ARE_EQUAL_WITH_MSG(void_ptr, (void*)disposition_result, (void*)TEST_AMQP_DISPOSITION_REJECTED, "Expected REJECTED disposition");

	// cleanup
	transport_interface->IoTHubTransport_Destroy(transport);
	cleanupList(config.waitingToSend);
}

// Tests_SRS_IOTHUBTRANSPORTAMQP_09_044: [If handle parameter is NULL then IoTHubTransportAMQP_SetOption shall return IOTHUB_CLIENT_INVALID_ARG.]
TEST_FUNCTION(AMQP_SetOption_NULL_transport_fails)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
        TEST_DEVICE_ID, TEST_DEVICE_KEY, NULL, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME_NULL };
    IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };
    TRANSPORT_LL_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);

    mocks.ResetAllCalls();

    // act
    IOTHUB_CLIENT_RESULT result = transport_interface->IoTHubTransport_SetOption(NULL, TEST_OPTION_SASTOKEN_LIFETIME, TEST_RANDOM_CHAR_SEQ);

    // assert
    mocks.AssertActualAndExpectedCalls();
    ASSERT_ARE_EQUAL_WITH_MSG(IOTHUB_CLIENT_RESULT, result, IOTHUB_CLIENT_INVALID_ARG, "IoTHubTransport_SetOption returned unexpected result.");

    // cleanup
    transport_interface->IoTHubTransport_Destroy(transport);
}

// Tests_SRS_IOTHUBTRANSPORTAMQP_09_045: [If parameter optionName is NULL then IoTHubTransportAMQP_SetOption shall return IOTHUB_CLIENT_INVALID_ARG.] 
TEST_FUNCTION(AMQP_SetOption_NULL_option_fails)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
        TEST_DEVICE_ID, TEST_DEVICE_KEY, NULL, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME_NULL };
    IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };
    TRANSPORT_LL_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);

    mocks.ResetAllCalls();


    // act
    IOTHUB_CLIENT_RESULT result = transport_interface->IoTHubTransport_SetOption(transport, NULL, TEST_RANDOM_CHAR_SEQ);

    // assert
    mocks.AssertActualAndExpectedCalls();
    ASSERT_ARE_EQUAL_WITH_MSG(IOTHUB_CLIENT_RESULT, result, IOTHUB_CLIENT_INVALID_ARG, "IoTHubTransport_SetOption returned unexpected result.");

    // cleanup
    transport_interface->IoTHubTransport_Destroy(transport);
}

// Tests_SRS_IOTHUBTRANSPORTAMQP_09_046: [If parameter value is NULL then IoTHubTransportAMQP_SetOption shall return IOTHUB_CLIENT_INVALID_ARG.]
TEST_FUNCTION(AMQP_SetOption_NULL_value_fails)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
        TEST_DEVICE_ID, TEST_DEVICE_KEY, NULL, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME_NULL };
    IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };
    TRANSPORT_LL_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);

    mocks.ResetAllCalls();

    // act
    IOTHUB_CLIENT_RESULT result = transport_interface->IoTHubTransport_SetOption(transport, TEST_OPTION_SASTOKEN_LIFETIME, NULL);

    // assert
    mocks.AssertActualAndExpectedCalls();
    ASSERT_ARE_EQUAL_WITH_MSG(IOTHUB_CLIENT_RESULT, result, IOTHUB_CLIENT_INVALID_ARG, "IoTHubTransport_SetOption returned unexpected result.");

    // cleanup
    transport_interface->IoTHubTransport_Destroy(transport);
}

// Tests_SRS_IOTHUBTRANSPORTAMQP_09_047: [If the option name does not match one of the options handled by this module, then IoTHubTransportAMQP_SetOption shall get  the handle to the XIO and invoke the xio_setoption passing down the option name and value parameters.] 
TEST_FUNCTION(AMQP_SetOption_invokes_xio_setoption_succeeds)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
        TEST_DEVICE_ID, TEST_DEVICE_KEY, NULL, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME_NULL };
    IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };
    TRANSPORT_LL_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);
    const char* SOME_OPTION = "AnOption";
    const void* SOME_VALUE = (void*)42;

    mocks.ResetAllCalls();

    EXPECTED_CALL(mocks, STRING_c_str(NULL));
    EXPECTED_CALL(mocks, platform_get_default_tlsio()).SetReturn(TEST_TLS_IO_INTERFACE_DESC);
    EXPECTED_CALL(mocks, xio_create(NULL, NULL)).SetReturn(TEST_TLS_IO_INTERFACE);
    STRICT_EXPECTED_CALL(mocks, xio_setoption(NULL, SOME_OPTION, SOME_VALUE))
        .IgnoreArgument(1);

    // act
    IOTHUB_CLIENT_RESULT result = transport_interface->IoTHubTransport_SetOption(transport, SOME_OPTION, SOME_VALUE);

    // assert
    mocks.AssertActualAndExpectedCalls();
    ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, result, IOTHUB_CLIENT_OK);

    // cleanup
    transport_interface->IoTHubTransport_Destroy(transport);
}

/* Tests_SRS_IOTHUBTRANSPORTAMQP_03_001: [If xio_setoption fails, IoTHubTransportAMQP_SetOption shall return IOTHUB_CLIENT_ERROR.] */
TEST_FUNCTION(AMQP_SetOption_fails_when_xio_setoption_fails)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
        TEST_DEVICE_ID, TEST_DEVICE_KEY, NULL, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME_NULL };
    IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };
    TRANSPORT_LL_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);
    const char* SOME_OPTION = "AnOption";
    const void* SOME_VALUE = (void*)42;

    mocks.ResetAllCalls();

    EXPECTED_CALL(mocks, STRING_c_str(NULL));
    EXPECTED_CALL(mocks, platform_get_default_tlsio()).SetReturn(TEST_TLS_IO_INTERFACE_DESC);
    EXPECTED_CALL(mocks, xio_create(NULL, NULL)).SetReturn(TEST_TLS_IO_INTERFACE);
    STRICT_EXPECTED_CALL(mocks, xio_setoption(NULL, SOME_OPTION, SOME_VALUE))
        .IgnoreArgument(1)
        .SetReturn(42);

    // act
    IOTHUB_CLIENT_RESULT result = transport_interface->IoTHubTransport_SetOption(transport, SOME_OPTION, SOME_VALUE);

    // assert
    mocks.AssertActualAndExpectedCalls();
    ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, result, IOTHUB_CLIENT_ERROR);

    // cleanup
    transport_interface->IoTHubTransport_Destroy(transport);
}

// Tests_SRS_IOTHUBTRANSPORTAMQP_17_003: [IoTHubTransportAMQP_Register shall return the TRANSPORT_LL_HANDLE as the IOTHUB_DEVICE_HANDLE.] 
TEST_FUNCTION(AMQP_Register_transport_success_returns_transport)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;
    IOTHUB_DEVICE_CONFIG device;
    device.deviceId = TEST_DEVICE_ID;
    device.deviceKey = TEST_DEVICE_KEY;
    device.deviceSasToken = NULL;

    DLIST_ENTRY wts;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
        TEST_DEVICE_ID, TEST_DEVICE_KEY, NULL, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME_NULL };
    IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };

    TRANSPORT_LL_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);
    mocks.ResetAllCalls();

    EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(mocks, STRING_construct(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG));

    // act
    IOTHUB_DEVICE_HANDLE devHandle = transport_interface->IoTHubTransport_Register(transport, &device, TEST_IOTHUB_CLIENT_LL_HANDLE, &wts);

    // assert
    ASSERT_IS_NOT_NULL(devHandle);
    ASSERT_ARE_EQUAL(void_ptr, transport, devHandle);
    mocks.AssertActualAndExpectedCalls();

    // cleanup
    transport_interface->IoTHubTransport_Destroy(transport);
    cleanupList(config.waitingToSend);
}

TEST_FUNCTION(AMQP_Register_twice_returns_null_second_time)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;
    IOTHUB_DEVICE_CONFIG device;
    device.deviceId = TEST_DEVICE_ID;
    device.deviceKey = TEST_DEVICE_KEY;
    device.deviceSasToken = NULL;

    DLIST_ENTRY wts;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
        TEST_DEVICE_ID, TEST_DEVICE_KEY, NULL, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME_NULL };
    IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };

    TRANSPORT_LL_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);
    (void)transport_interface->IoTHubTransport_Register(transport, &device, TEST_IOTHUB_CLIENT_LL_HANDLE, &wts);
    mocks.ResetAllCalls();

    EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(mocks, STRING_construct(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG));

    // act
    IOTHUB_DEVICE_HANDLE devHandle2 = transport_interface->IoTHubTransport_Register(transport, &device, TEST_IOTHUB_CLIENT_LL_HANDLE, &wts);

    // assert
    ASSERT_IS_NULL(devHandle2);
    mocks.AssertActualAndExpectedCalls();

    // cleanup
    transport_interface->IoTHubTransport_Destroy(transport);
    cleanupList(config.waitingToSend);
}

// Tests_SRS_IOTHUBTRANSPORTAMQP_17_002: [IoTHubTransportAMQP_Register shall return NULL if deviceId or deviceKey do not match the deviceId and deviceKey passed in during IoTHubTransportAMQP_Create.] 
TEST_FUNCTION(AMQP_Register_transport_deviceKey_mismatch_returns_null)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;
    IOTHUB_DEVICE_CONFIG device;
    device.deviceId = TEST_DEVICE_ID;
    device.deviceKey = "this is a different device key";
    device.deviceSasToken = NULL;

    DLIST_ENTRY wts;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
        TEST_DEVICE_ID, TEST_DEVICE_KEY, NULL, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME_NULL };
    IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };

    TRANSPORT_LL_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);
    mocks.ResetAllCalls();

    EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(mocks, STRING_construct(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG));

    // act
    IOTHUB_DEVICE_HANDLE devHandle = transport_interface->IoTHubTransport_Register(transport, &device, TEST_IOTHUB_CLIENT_LL_HANDLE, &wts);

    // assert
    ASSERT_IS_NULL(devHandle);
    mocks.AssertActualAndExpectedCalls();

    // cleanup
    transport_interface->IoTHubTransport_Destroy(transport);
    cleanupList(config.waitingToSend);
}

// Tests_SRS_IOTHUBTRANSPORTAMQP_17_002: [IoTHubTransportAMQP_Register shall return NULL if deviceId or deviceKey do not match the deviceId and deviceKey passed in during IoTHubTransportAMQP_Create.] 
TEST_FUNCTION(AMQP_Register_transport_deviceId_mismatch_returns_null)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;
    IOTHUB_DEVICE_CONFIG device;
    device.deviceId = "not the right devId";
    device.deviceKey = TEST_DEVICE_KEY;
    device.deviceSasToken = NULL;

    DLIST_ENTRY wts;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
        TEST_DEVICE_ID, TEST_DEVICE_KEY, NULL, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME_NULL };
    IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };

    TRANSPORT_LL_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);
    mocks.ResetAllCalls();

    EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(mocks, STRING_construct(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG));

    // act
    IOTHUB_DEVICE_HANDLE devHandle = transport_interface->IoTHubTransport_Register(transport, &device, TEST_IOTHUB_CLIENT_LL_HANDLE, &wts);

    // assert
    ASSERT_IS_NULL(devHandle);
    mocks.AssertActualAndExpectedCalls();

    // cleanup
    transport_interface->IoTHubTransport_Destroy(transport);
    cleanupList(config.waitingToSend);
}

// Tests_SRS_IOTHUBTRANSPORTAMQP_17_001: [IoTHubTransportAMQP_Register shall return NULL if deviceId, deviceKey or waitingToSend are NULL.] 
TEST_FUNCTION(AMQP_Register_transport_deviceId_null_returns_null)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;
    IOTHUB_DEVICE_CONFIG device;
    device.deviceId = NULL;
    device.deviceKey = TEST_DEVICE_KEY;
    device.deviceSasToken = NULL;

    DLIST_ENTRY wts;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
        TEST_DEVICE_ID, TEST_DEVICE_KEY, NULL, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME_NULL };
    IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };

    TRANSPORT_LL_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);
    mocks.ResetAllCalls();

    // act
    IOTHUB_DEVICE_HANDLE devHandle = transport_interface->IoTHubTransport_Register(transport, &device, TEST_IOTHUB_CLIENT_LL_HANDLE, &wts);

    // assert
    ASSERT_IS_NULL(devHandle);
    mocks.AssertActualAndExpectedCalls();

    // cleanup
    transport_interface->IoTHubTransport_Destroy(transport);
    cleanupList(config.waitingToSend);
}

// Tests_SRS_IOTHUBTRANSPORTAMQP_17_001: [IoTHubTransportAMQP_Register shall return NULL if device or waitingToSend are NULL.] 
TEST_FUNCTION(AMQP_Register_transport_device_null_returns_null)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
        TEST_DEVICE_ID, TEST_DEVICE_KEY, NULL, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME_NULL };
    IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };

    TRANSPORT_LL_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);
    mocks.ResetAllCalls();

    // act
    IOTHUB_DEVICE_HANDLE devHandle = transport_interface->IoTHubTransport_Register(transport, NULL, TEST_IOTHUB_CLIENT_LL_HANDLE, &wts);

    // assert
    ASSERT_IS_NULL(devHandle);
    mocks.AssertActualAndExpectedCalls();

    // cleanup
    transport_interface->IoTHubTransport_Destroy(transport);
    cleanupList(config.waitingToSend);
}

// Tests_SRS_IOTHUBTRANSPORTAMQP_03_002: [IoTHubTransportAMQP_Register shall return NULL if deviceId is NULL.]
TEST_FUNCTION(AMQP_Register_transport_deviceKey_null_and_deviceSasToken_null_returns_null)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;
    IOTHUB_DEVICE_CONFIG device;
    device.deviceId = NULL;
    device.deviceKey = TEST_DEVICE_KEY;
    device.deviceSasToken = NULL;

    DLIST_ENTRY wts;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
        NULL, TEST_DEVICE_KEY, NULL, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME_NULL };
    IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };

    TRANSPORT_LL_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);
    mocks.ResetAllCalls();

    // act
    IOTHUB_DEVICE_HANDLE devHandle = transport_interface->IoTHubTransport_Register(transport, &device, TEST_IOTHUB_CLIENT_LL_HANDLE, &wts);

    // assert
    ASSERT_IS_NULL(devHandle);
    mocks.AssertActualAndExpectedCalls();

    // cleanup
    transport_interface->IoTHubTransport_Destroy(transport);
    cleanupList(config.waitingToSend);
}

/*Tests_SRS_IOTHUBTRANSPORTAMQP_02_003: [ IoTHubTransportAMQP_Register shall assume a x509 authentication mechanism when both deviceKey and deviceSasToken are NULL. ]*/
TEST_FUNCTION(AMQP_Register_transport_deviceKey_null_and_deviceSasToken_null_assumes_x509_succeeds)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;
    IOTHUB_DEVICE_CONFIG device;
    device.deviceId = TEST_DEVICE_ID;
    device.deviceKey = NULL;
    device.deviceSasToken = NULL;

    DLIST_ENTRY wts;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
        TEST_DEVICE_ID, NULL, NULL, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME_NULL };
    IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };

    TRANSPORT_LL_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);
    mocks.ResetAllCalls();

    EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(mocks, STRING_construct(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG));

    // act
    IOTHUB_DEVICE_HANDLE devHandle = transport_interface->IoTHubTransport_Register(transport, &device, TEST_IOTHUB_CLIENT_LL_HANDLE, &wts);

    // assert
    ASSERT_IS_NOT_NULL(devHandle);
    mocks.AssertActualAndExpectedCalls();

    // cleanup
    transport_interface->IoTHubTransport_Destroy(transport);
    cleanupList(config.waitingToSend);
}

// Tests_SRS_IOTHUBTRANSPORTAMQP_03_003: [IoTHubTransportAMQP_Register shall return NULL if both deviceKey and deviceSasToken are not NULL.] 
TEST_FUNCTION(AMQP_Register_transport_deviceKey_and_deviceSasToken_provided_returns_null)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;
    IOTHUB_DEVICE_CONFIG device;
    device.deviceId = TEST_DEVICE_ID;
    device.deviceKey = TEST_DEVICE_KEY;
    device.deviceSasToken = TEST_DEVICE_SAS;

    DLIST_ENTRY wts;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
        TEST_DEVICE_ID, TEST_DEVICE_KEY, NULL, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME_NULL };
    IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };

    TRANSPORT_LL_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);
    mocks.ResetAllCalls();

    // act
    IOTHUB_DEVICE_HANDLE devHandle = transport_interface->IoTHubTransport_Register(transport, &device, TEST_IOTHUB_CLIENT_LL_HANDLE, &wts);

    // assert
    ASSERT_IS_NULL(devHandle);
    mocks.AssertActualAndExpectedCalls();

    // cleanup
    transport_interface->IoTHubTransport_Destroy(transport);
    cleanupList(config.waitingToSend);
}

// Tests_SRS_IOTHUBTRANSPORTAMQP_17_001: [IoTHubTransportAMQP_Register shall return NULL if deviceId, deviceKey or waitingToSend are NULL.] 
TEST_FUNCTION(AMQP_Register_transport_wts_null_returns_null)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;
    IOTHUB_DEVICE_CONFIG device;
    device.deviceId = "not the right devId";
    device.deviceKey = TEST_DEVICE_KEY;
    device.deviceSasToken = NULL;

    DLIST_ENTRY wts;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
        TEST_DEVICE_ID, TEST_DEVICE_KEY, NULL, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME_NULL };
    IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };

    TRANSPORT_LL_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);
    mocks.ResetAllCalls();

    // act
    IOTHUB_DEVICE_HANDLE devHandle = transport_interface->IoTHubTransport_Register(transport, &device, TEST_IOTHUB_CLIENT_LL_HANDLE, NULL);

    // assert
    ASSERT_IS_NULL(devHandle);
    mocks.AssertActualAndExpectedCalls();

    // cleanup
    transport_interface->IoTHubTransport_Destroy(transport);
    cleanupList(config.waitingToSend);
}

// Tests_SRS_IOTHUBTRANSPORTAMQP_17_005: [IoTHubTransportAMQP_Register shall return NULL if the TRANSPORT_LL_HANDLE is NULL.] 
TEST_FUNCTION(AMQP_Register_transport_null_returns_null)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;
    IOTHUB_DEVICE_CONFIG device;
    device.deviceId = TEST_DEVICE_ID;
    device.deviceKey = TEST_DEVICE_KEY;
    device.deviceSasToken = NULL;

    DLIST_ENTRY wts;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();

    TRANSPORT_LL_HANDLE transport = NULL;
    mocks.ResetAllCalls();

    // act
    IOTHUB_DEVICE_HANDLE devHandle = transport_interface->IoTHubTransport_Register(transport, &device, TEST_IOTHUB_CLIENT_LL_HANDLE, &wts);

    // assert
    ASSERT_IS_NULL(devHandle);
    mocks.AssertActualAndExpectedCalls();

    // cleanup
    cleanupList(&wts);
}

// Tests_SRS_IOTHUBTRANSPORTAMQP_17_004: [IoTHubTransportAMQP_Unregister shall return.] 
TEST_FUNCTION(AMQP_Unregister_transport_success)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;
    IOTHUB_DEVICE_CONFIG device;
    device.deviceId = TEST_DEVICE_ID;
    device.deviceKey = TEST_DEVICE_KEY;
    device.deviceSasToken = NULL;

    DLIST_ENTRY wts;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
        TEST_DEVICE_ID, TEST_DEVICE_KEY, NULL, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME_NULL };
    IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };

    TRANSPORT_LL_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);

    IOTHUB_DEVICE_HANDLE devHandle = transport_interface->IoTHubTransport_Register(transport, &device, TEST_IOTHUB_CLIENT_LL_HANDLE, &wts);

    mocks.ResetAllCalls();

    // act
    transport_interface->IoTHubTransport_Unregister(devHandle);

    // assert
    mocks.AssertActualAndExpectedCalls();

    // cleanup
    transport_interface->IoTHubTransport_Destroy(transport);
    cleanupList(config.waitingToSend);
}

TEST_FUNCTION(AMQP_Register_transport_Register_Unregister_Register_success_returns_transport)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;
    IOTHUB_DEVICE_CONFIG device;
    device.deviceId = TEST_DEVICE_ID;
    device.deviceKey = TEST_DEVICE_KEY;
    device.deviceSasToken = NULL;

    DLIST_ENTRY wts;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
        TEST_DEVICE_ID, TEST_DEVICE_KEY, NULL, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME_NULL };
    IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };

    TRANSPORT_LL_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);
    IOTHUB_DEVICE_HANDLE devHandle = transport_interface->IoTHubTransport_Register(transport, &device, TEST_IOTHUB_CLIENT_LL_HANDLE, &wts);
    transport_interface->IoTHubTransport_Unregister(devHandle);
    mocks.ResetAllCalls();

    EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(mocks, STRING_construct(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG));

    // act
    IOTHUB_DEVICE_HANDLE devHandle2 = transport_interface->IoTHubTransport_Register(transport, &device, TEST_IOTHUB_CLIENT_LL_HANDLE, &wts);

    // assert
    ASSERT_IS_NOT_NULL(devHandle2);
    ASSERT_ARE_EQUAL(void_ptr, transport, devHandle2);
    mocks.AssertActualAndExpectedCalls();

    // cleanup
    transport_interface->IoTHubTransport_Destroy(transport);
    cleanupList(config.waitingToSend);
}

/*Tests_SRS_IOTHUBTRANSPORTAMQP_02_001: [ If parameter handle is NULL then IoTHubTransportAMQP_GetHostname shall return NULL. ]*/
TEST_FUNCTION(IoTHubTransportAMQP_GetHostname_with_NULL_handle_fails)
{
    ///arrange
    CIoTHubTransportAMQPMocks mocks;
    TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();

    ///act
    STRING_HANDLE hostname = transport_interface->IoTHubTransport_GetHostname(NULL);

    ///assert
    ASSERT_IS_NULL(hostname);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
}

/*Tests_SRS_IOTHUBTRANSPORTAMQP_02_002: [ Otherwise IoTHubTransportAMQP_GetHostname shall return a STRING_HANDLE for the hostname. ]*/
TEST_FUNCTION(IoTHubTransportAMQP_GetHostname_succeeds)
{
    /// arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();

    IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
        TEST_DEVICE_ID, TEST_DEVICE_KEY, NULL, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME_NULL };
    IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };
    TRANSPORT_LL_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);
    mocks.ResetAllCalls();

    ///act
    STRING_HANDLE hostname = transport_interface->IoTHubTransport_GetHostname(transport);

    ///assert
    ASSERT_IS_NOT_NULL(hostname);
    mocks.AssertActualAndExpectedCalls();
    ASSERT_ARE_EQUAL(char_ptr, TEST_IOT_HUB_NAME "." TEST_IOT_HUB_SUFFIX, STRING_c_str(hostname));
    
    ///cleanup
    transport_interface->IoTHubTransport_Destroy(transport);
}

/*Tests_SRS_IOTHUBTRANSPORTAMQP_02_004: [ If both deviceKey and deviceSasToken fields are NULL then IoTHubTransportAMQP_Create shall assume a x509 authentication. ]*/
TEST_FUNCTION(IoTHubTransportAMQP_Create_for_x509_succeeds)
{
    /// arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();

    IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
        TEST_DEVICE_ID, NULL, NULL, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME_NULL }; /*notice both deviceKey and deviceSasToken are NULL*/
    IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };

    setExpectedCallsForTransportCreateUpTo(mocks, &config, STEP_CREATE_RECEIVE_ADDRESS);

    ///act
    TRANSPORT_LL_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);

    ///assert
    ASSERT_IS_NOT_NULL(transport);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    transport_interface->IoTHubTransport_Destroy(transport);
}

/*Tests_SRS_IOTHUBTRANSPORTAMQP_02_005: [ IoTHubTransportAMQP_DoWork shall create the connection with the IoT service using connection_create2() AMQP API, passing the TLS I/O layer, IoT Hub FQDN and container ID as parameters (pass NULL for callbacks) ]*/
/*Tests_SRS_IOTHUBTRANSPORTAMQP_02_006: [ IoTHubTransportAMQP_DoWork shall not establish a CBS connection. ]*/
TEST_FUNCTION(IoTHubTransportAMQP_DoWork_x509_doesn_t_do_CBS)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
        TEST_DEVICE_ID, NULL, NULL, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME_NULL }; /*note: no deviceKey, no deviceSasToken => x509*/
    IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };
    time_t current_time = time(NULL);

    TRANSPORT_LL_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);
    int subscribe_result = transport_interface->IoTHubTransport_Subscribe(transport);
    ASSERT_ARE_EQUAL(int, subscribe_result, 0);
    mocks.ResetAllCalls();

    setExpectedCallsForTransportDoWorkUpTo(mocks, STEP_DOWORK_GET_TLS_IO, DOWORK_MESSAGERECEIVER_NONE, current_time);

    EXPECTED_CALL(mocks, STRING_c_str(NULL));
    EXPECTED_CALL(mocks, connection_create2(NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL));

    EXPECTED_CALL(mocks, session_create(NULL, NULL, NULL));

    STRICT_EXPECTED_CALL(mocks, session_set_incoming_window(NULL, TEST_INCOMING_WINDOW_SIZE)).IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, session_set_outgoing_window(NULL, TEST_OUTGOING_WINDOW_SIZE)).IgnoreArgument(1);
	setExpectedCallsForGetSecondsSinceEpoch(mocks, current_time);
    EXPECTED_CALL(mocks, connection_set_trace(IGNORED_PTR_ARG, false));
    EXPECTED_CALL(mocks, xio_setoption(IGNORED_PTR_ARG, OPTION_LOG_TRACE, IGNORED_PTR_ARG));

    setExpectedCallsForCreateMessageReceiver(mocks);
    setExpectedCallsForCreateEventSender(mocks);
    STRICT_EXPECTED_CALL(mocks, DList_IsListEmpty(config.waitingToSend)).SetReturn(1);
    setExpectedCallsForConnectionDoWork(mocks);

    // act
    transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // assert
    mocks.AssertActualAndExpectedCalls();

    // cleanup
    transport_interface->IoTHubTransport_Destroy(transport);
}

/*Tests_SRS_IOTHUBTRANSPORTAMQP_02_005: [ IoTHubTransportAMQP_DoWork shall create the connection with the IoT service using connection_create2() AMQP API, passing the TLS I/O layer, IoT Hub FQDN and container ID as parameters (pass NULL for callbacks) ]*/

TEST_FUNCTION(IoTHubTransportAMQP_DoWork_x509_doesn_t_do_CBS_fails_when_connection_create_fails)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
        TEST_DEVICE_ID, NULL, NULL, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME_NULL }; /*note: no deviceKey, no deviceSasToken => x509*/
    IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };
    time_t current_time = time(NULL);

    TRANSPORT_LL_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);
    int subscribe_result = transport_interface->IoTHubTransport_Subscribe(transport);
    ASSERT_ARE_EQUAL(int, subscribe_result, 0);
    mocks.ResetAllCalls();

    setExpectedCallsForTransportDoWorkUpTo(mocks, STEP_DOWORK_GET_TLS_IO, DOWORK_MESSAGERECEIVER_NONE, current_time);
    EXPECTED_CALL(mocks, xio_retrieveoptions(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks,xio_destroy(IGNORED_PTR_ARG));

    EXPECTED_CALL(mocks, STRING_c_str(NULL));
    EXPECTED_CALL(mocks, connection_create2(NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL))
        .SetFailReturn((CONNECTION_HANDLE)NULL);

    // act
    transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // assert
    mocks.AssertActualAndExpectedCalls();

    // cleanup
    transport_interface->IoTHubTransport_Destroy(transport);
}

/*Tests_SRS_IOTHUBTRANSPORTAMQP_02_005: [ IoTHubTransportAMQP_DoWork shall create the connection with the IoT service using connection_create2() AMQP API, passing the TLS I/O layer, IoT Hub FQDN and container ID as parameters (pass NULL for callbacks) ]*/
TEST_FUNCTION(IoTHubTransportAMQP_DoWork_x509_doesn_t_do_CBS_fails_when_session_create_fails)
{
    // arrange
    CIoTHubTransportAMQPMocks mocks;

    DLIST_ENTRY wts;
    BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
    TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();
    IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
        TEST_DEVICE_ID, NULL, NULL, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME_NULL }; /*note: no deviceKey, no deviceSasToken => x509*/
    IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };
    time_t current_time = time(NULL);

    TRANSPORT_LL_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);
    int subscribe_result = transport_interface->IoTHubTransport_Subscribe(transport);
    ASSERT_ARE_EQUAL(int, subscribe_result, 0);
    mocks.ResetAllCalls();

    setExpectedCallsForTransportDoWorkUpTo(mocks, STEP_DOWORK_GET_TLS_IO, DOWORK_MESSAGERECEIVER_NONE, current_time);

    EXPECTED_CALL(mocks, xio_retrieveoptions(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, xio_destroy(IGNORED_PTR_ARG));

    EXPECTED_CALL(mocks, STRING_c_str(NULL));
    EXPECTED_CALL(mocks, connection_create2(NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL));
    EXPECTED_CALL(mocks, connection_destroy(IGNORED_PTR_ARG));


    EXPECTED_CALL(mocks, session_create(NULL, NULL, NULL))
        .SetFailReturn((SESSION_HANDLE)NULL);

    // act
    transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);

    // assert
    mocks.AssertActualAndExpectedCalls();

    // cleanup
    transport_interface->IoTHubTransport_Destroy(transport);
}

TEST_FUNCTION(AMQP_DoWork_create_connection_getSecondsSinceEpoch_get_time_fails)
{
	// arrange
	CIoTHubTransportAMQPMocks mocks;

	DLIST_ENTRY wts;
	BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
	TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();

	IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
		TEST_DEVICE_ID, TEST_DEVICE_KEY, NULL, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME_NULL };
	IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };

	TRANSPORT_LL_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);

	mocks.ResetAllCalls();
	setExpectedCallsForTransportDoWorkUpTo(mocks, STEP_DOWORK_CREATE_CBS, DOWORK_MESSAGERECEIVER_NONE, time(NULL));
	EXPECTED_CALL(mocks, cbs_open(NULL));
	EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(INDEFINITE_TIME);
	setExpectedCallsForConnectionDestroyUpTo(mocks, STEP_DOWORK_CREATE_CBS);

	// act
	transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);

	// assert
	mocks.AssertActualAndExpectedCalls();

	// cleanup
	transport_interface->IoTHubTransport_Destroy(transport);
}

// Tests_SRS_IOTHUBTRANSPORTAMQP_09_083: [SAS tokens expiration time shall be calculated using the number of seconds since Epoch UTC (Jan 1st 1970 00h00m00s000 GMT) to now (GMT), plus the 'sas_token_lifetime'.] 
TEST_FUNCTION(AMQP_DoWork_authentication_SASToken_get_time_current_local_time_fails)
{
	// arrange
	CIoTHubTransportAMQPMocks mocks;

	DLIST_ENTRY wts;
	BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
	TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)AMQP_Protocol();
	IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
		TEST_DEVICE_ID, TEST_DEVICE_KEY, NULL, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME_NULL };
	IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };
	time_t current_time = time(NULL);
	time_t expiration_time = addSecondsToTime(current_time, (TEST_SAS_TOKEN_LIFETIME_MS / 2) / 1000 + 1);

	TRANSPORT_LL_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);
	int subscribe_result = transport_interface->IoTHubTransport_Subscribe(transport);
	ASSERT_ARE_EQUAL(int, subscribe_result, 0);

	mocks.ResetAllCalls();
	setExpectedCallsForTransportDoWorkUpTo(mocks, STEP_DOWORK_OPEN_CBS, DOWORK_MESSAGERECEIVER_NONE, current_time);
	setExpectedCallsForCbsAuthentication(mocks, current_time);
	setExpectedCallsForCbsAuthTimeoutCheck(mocks, current_time);
	setExpectedCallsForConnectionDoWork(mocks);
	setExpectedCallsForSASTokenExpiryCheck(mocks, expiration_time);
	EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(INDEFINITE_TIME);
	setExpectedCallsForConnectionDestroyUpTo(mocks, STEP_DOWORK_CREATE_CBS);

	// act
	transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);
	test_latest_cbs_put_token_callback(test_latest_cbs_put_token_context, CBS_OPERATION_RESULT_OK, 0, NULL);
	transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);

	// assert
	mocks.AssertActualAndExpectedCalls();

	// cleanup
	transport_interface->IoTHubTransport_Destroy(transport);
}

END_TEST_SUITE(iothubtransportamqp_ut)
