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

#include "buffer_.h"
#include "lock.h"
#include "doublylinkedlist.h"
#include "strings.h"
#include "urlencode.h"
#include "crt_abstractions.h"
#include "sastoken.h"
#include "tlsio_schannel.h"
#include "tlsio_openssl.h"
#include "platform.h"
#include "xio.h"
#include "macro_utils.h"

#include "iothubtransportuamqp.h"
#include "iothub_client_private.h"
#include "iothub_message.h"

#include "amqpalloc.h"
#include "amqpvalue.h"
#include "amqpvalue_to_string.h"
#include "amqp_definitions.h"
#include "amqp_frame_codec.h"
#include "amqp_management.h"
#include "amqp_types.h"
#include "cbs.h"
#include "connection.h"
#include "consolelogger.h"
#include "frame_codec.h"
#include "header_detect_io.h"
#include "link.h"
#include "message.h"
#include "message_receiver.h"
#include "message_sender.h"
#include "messaging.h"
#include "saslclientio.h"
#include "sasl_anonymous.h"
#include "sasl_frame_codec.h"
#include "sasl_mechanism.h"
#include "sasl_mssbcbs.h"
#include "sasl_plain.h"
#include "session.h"
#include "socket_listener.h"
#include "wsio.h"


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

DEFINE_MICROMOCK_ENUM_TO_STRING(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_RESULT_VALUES);
DEFINE_MICROMOCK_ENUM_TO_STRING(IOTHUB_BATCHSTATE_RESULT, IOTHUB_BATCHSTATE_RESULT_VALUES);


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
#define TEST_DEVICE_ID "deviceid"
#define TEST_DEVICE_KEY "devicekey"
#define TEST_IOT_HUB_NAME "servername"
#define TEST_IOT_HUB_SUFFIX "domainname"
#define TEST_PROT_GW_HOSTNAME NULL

#define TEST_IOT_HUB_PORT 5671
#define TEST_INCOMING_WINDOW_SIZE SIZE_MAX
#define TEST_OUTGOING_WINDOW_SIZE 100
#define TEST_MESSAGE_RECEIVER_LINK_NAME "receiver-link"
#define TEST_MESSAGE_RECEIVER_TARGET_ADDRESS "ingress-rx"
#define TEST_MESSAGE_RECEIVER_MAX_LINK_SIZE 65536
#define TEST_MESSAGE_SENDER_LINK_NAME "sender-link"
#define TEST_MESSAGE_SENDER_SOURCE_ADDRESS "ingress"
#define TEST_MESSAGE_SENDER_MAX_LINK_SIZE 65536
#define TEST_SAS_TOKEN_LIFETIME_MS 3600000
#define TEST_CBS_REQUEST_TIMEOUT_MS 30000

#define TEST_STRING_HANDLE (STRING_HANDLE)0x46
#define TEST_NULL_STRING_HANDLE (STRING_HANDLE)NULL
#define TEST_STRING_COPY_FAILURE_RESULT 1

#define TEST_IOTHUB_CLIENT_LL_HANDLE (IOTHUB_CLIENT_LL_HANDLE)0x49
#define TEST_TLS_IO_PROVIDER (XIO_HANDLE)0x77
#define TEST_SASL_MECHANISM (SASL_MECHANISM_HANDLE)0x77
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
#define TEST_MESSAGE_HANDLE (IOTHUB_MESSAGE_HANDLE)0x200
#define TEST_RANDOM_CHAR_SEQ "This is a random char sequence"
#define TEST_RANDOM_CHAR_SEQ_SIZE 30
#define TEST_BINARY_BUFFER (const unsigned char*)0x210
#define TEST_BINARY_BUFFER_SIZE 56
#define TEST_EVENT_MESSAGE_HANDLE (MESSAGE_HANDLE)0x220
#define TEST_OPTION_SASTOKEN_LIFETIME "sas_token_lifetime"
#define TEST_OPTION_SASTOKEN_REFRESH_TIME "sas_token_refresh_time"
#define TEST_OPTION_CBS_REQUEST_TIMEOUT "cbs_request_timeout"
#define TEST_OPTION_MESSAGE_SEND_TIMEOUT "message_send_timeout"


time_t test_current_time;
XIO_HANDLE test_TLS_io_transport = NULL;
char* test_latest_getIOTransport_fqdn = NULL;
int test_latest_getIOTransport_port = 0;
size_t test_latest_SASToken_expiry_time = 0;
ON_CBS_OPERATION_COMPLETE test_latest_cbs_put_token_callback;
void* test_latest_cbs_put_token_context;
int test_number_of_event_confirmation_callbacks_invoked;
int test_sum_of_event_confirmation_callback_contexts;
BINARY_DATA test_binary_data;

bool fail_malloc = false;
bool fail_STRING_new = false;
bool fail_STRING_new_with_memory = false;
bool fail_STRING_construct = false;

static STRING_HANDLE test_STRING_HANDLE_EMPTY;
static STRING_HANDLE test_iotHubHostFqdn;

// **  Mocks **
TYPED_MOCK_CLASS(CIoTHubTransportuAMQPMocks, CGlobalMock)
{
public:
	/* Shared Utilities Mocks */
	// platform.h
	MOCK_STATIC_METHOD_0(, int, platform_init)
	MOCK_METHOD_END(int, 0)

	MOCK_STATIC_METHOD_0(, void, platform_deinit)
	MOCK_VOID_METHOD_END()


	/* uAMQP Mocks */
	MOCK_STATIC_METHOD_0(, MESSAGE_HANDLE, message_create)
	MOCK_METHOD_END(MESSAGE_HANDLE, NULL)

	MOCK_STATIC_METHOD_1(, void, amqpvalue_destroy, AMQP_VALUE, value)
	MOCK_VOID_METHOD_END()

	/* crt_abstractions mocks */
	MOCK_STATIC_METHOD_2(, int, mallocAndStrcpy_s, char**, destination, const char*, source)
	MOCK_METHOD_END(int, (*destination = (char*)BASEIMPLEMENTATION::gballoc_malloc(strlen(source) + 1), strcpy(*destination, source), 0))

	/* IoTHub_Message mocks */
	MOCK_STATIC_METHOD_2(, IOTHUB_MESSAGE_HANDLE, IoTHubMessage_CreateFromByteArray, const unsigned char*, buffer, size_t, size)
	MOCK_METHOD_END(IOTHUB_MESSAGE_HANDLE, NULL)

	MOCK_STATIC_METHOD_1(, void, IoTHubMessage_Destroy, IOTHUB_MESSAGE_HANDLE, iotHubMessageHandle)
	MOCK_VOID_METHOD_END()

	MOCK_STATIC_METHOD_3(, IOTHUB_MESSAGE_RESULT, IoTHubMessage_GetByteArray, IOTHUB_MESSAGE_HANDLE, iotHubMessageHandle, const unsigned char**, buffer, size_t*, size)
		*buffer = TEST_BINARY_BUFFER;
		*size = TEST_BINARY_BUFFER_SIZE;
		IOTHUB_MESSAGE_RESULT result2 = IOTHUB_MESSAGE_OK;
	MOCK_METHOD_END(IOTHUB_MESSAGE_RESULT, result2)

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
		MAP_HANDLE result2 = NULL;
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

	MOCK_STATIC_METHOD_3(, void, IoTHubClient_LL_SendComplete, IOTHUB_CLIENT_LL_HANDLE, handle, PDLIST_ENTRY, completedMessages, IOTHUB_BATCHSTATE_RESULT, batchResult)
		PDLIST_ENTRY oldest;
		while ((oldest = BASEIMPLEMENTATION::DList_RemoveHeadList(completedMessages)) != completedMessages)
		{
			;
		}
	MOCK_VOID_METHOD_END();

	MOCK_STATIC_METHOD_1(, time_t, get_time, time_t*, t)
	MOCK_METHOD_END(time_t, 0);

	MOCK_STATIC_METHOD_4(, STRING_HANDLE, SASToken_Create, STRING_HANDLE, key, STRING_HANDLE, scope, STRING_HANDLE, keyName, size_t, expiry)
		test_latest_SASToken_expiry_time = expiry;
		STRING_HANDLE sasToken = BASEIMPLEMENTATION::STRING_construct(TEST_SAS_TOKEN);
		if (sasToken == NULL)
		{
			LogError("Test failure: failed creating STRING instance for SASToken_create\r\n");
		}
	MOCK_METHOD_END(STRING_HANDLE, sasToken);

	// xio.h
	MOCK_STATIC_METHOD_3(, XIO_HANDLE, xio_create, const IO_INTERFACE_DESCRIPTION*, io_interface_description, const void*, io_create_parameters, LOGGER_LOG, logger_log)
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

	// tlsio_openssl.h
	MOCK_STATIC_METHOD_0(, int, tlsio_openssl_init)
	MOCK_METHOD_END(int, 0)

	MOCK_STATIC_METHOD_0(, void, tlsio_openssl_deinit)
	MOCK_VOID_METHOD_END()

	MOCK_STATIC_METHOD_2(, CONCRETE_IO_HANDLE, tlsio_openssl_create, void*, io_create_parameters, LOGGER_LOG, logger_log)
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
	MOCK_STATIC_METHOD_2(, CONCRETE_IO_HANDLE, tlsio_schannel_create, void*, io_create_parameters, LOGGER_LOG, logger_log)
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

	// uAMQP Mocks
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

	MOCK_STATIC_METHOD_1(, void, connection_destroy, CONNECTION_HANDLE, connection)
	MOCK_VOID_METHOD_END()

	MOCK_STATIC_METHOD_1(, void, connection_dowork, CONNECTION_HANDLE, connection)
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

	// message.h
	MOCK_STATIC_METHOD_2(, int, message_add_body_amqp_data, MESSAGE_HANDLE, message, BINARY_DATA, binary_data)
	MOCK_METHOD_END(int, 0)

	MOCK_STATIC_METHOD_1(, void, message_destroy, MESSAGE_HANDLE, message)
	MOCK_VOID_METHOD_END()

	MOCK_STATIC_METHOD_3(, int, message_get_body_amqp_data, MESSAGE_HANDLE, message, size_t, index, BINARY_DATA*, binary_data)
	MOCK_METHOD_END(int, 0)

	MOCK_STATIC_METHOD_2(, int, message_get_body_type, MESSAGE_HANDLE, message, MESSAGE_BODY_TYPE*, body_type)
	MOCK_METHOD_END(int, 0)

	// message_receiver.h
	MOCK_STATIC_METHOD_3(, MESSAGE_RECEIVER_HANDLE, messagereceiver_create, LINK_HANDLE, link, ON_MESSAGE_RECEIVER_STATE_CHANGED, on_message_receiver_state_changed, void*, context)
	MOCK_METHOD_END(MESSAGE_RECEIVER_HANDLE, 0)

	MOCK_STATIC_METHOD_1(, void, messagereceiver_destroy, MESSAGE_RECEIVER_HANDLE, message_receiver)
	MOCK_VOID_METHOD_END()

	MOCK_STATIC_METHOD_3(, int, messagereceiver_open, MESSAGE_RECEIVER_HANDLE, message_receiver, ON_MESSAGE_RECEIVED, on_message_received, const void*, callback_context)
	MOCK_METHOD_END(int, 0)

	MOCK_STATIC_METHOD_1(, int, messagereceiver_close, MESSAGE_RECEIVER_HANDLE, message_receiver)
	MOCK_METHOD_END(int, 0)


	// message_sender.h
	MOCK_STATIC_METHOD_4(, MESSAGE_SENDER_HANDLE, messagesender_create, LINK_HANDLE, link, ON_MESSAGE_SENDER_STATE_CHANGED, on_message_sender_state_changed, void*, context, LOGGER_LOG, logger_log)
	MOCK_METHOD_END(MESSAGE_SENDER_HANDLE, 0)

	MOCK_STATIC_METHOD_1(, void, messagesender_destroy, MESSAGE_SENDER_HANDLE, message_sender)
	MOCK_VOID_METHOD_END()

	MOCK_STATIC_METHOD_1(, int, messagesender_open, MESSAGE_SENDER_HANDLE, message_sender)
	MOCK_METHOD_END(int, 0)

	MOCK_STATIC_METHOD_1(, int, messagesender_close, MESSAGE_SENDER_HANDLE, message_sender)
	MOCK_METHOD_END(int, 0)

	MOCK_STATIC_METHOD_4(, int, messagesender_send, MESSAGE_SENDER_HANDLE, message_sender, MESSAGE_HANDLE, message, ON_MESSAGE_SEND_COMPLETE, on_message_send_complete, const void*, callback_context)
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
	MOCK_STATIC_METHOD_2(, CONCRETE_IO_HANDLE, saslclientio_create, void*, io_create_parameters, LOGGER_LOG, logger_log)
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
};

// ** End Mocks **
DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubTransportuAMQPMocks, , static STRING_HANDLE, concat3Params, const char*, prefix, const char*, infix, const char*, suffix);

DECLARE_GLOBAL_MOCK_METHOD_0(CIoTHubTransportuAMQPMocks, , MESSAGE_HANDLE, message_create);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportuAMQPMocks, , void, amqpvalue_destroy, AMQP_VALUE, value);

DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportuAMQPMocks, , IOTHUB_MESSAGE_HANDLE, IoTHubMessage_CreateFromByteArray, const unsigned char*, buffre, size_t, size);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportuAMQPMocks, , const char*, IoTHubMessage_GetString, IOTHUB_MESSAGE_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportuAMQPMocks, , void, IoTHubMessage_Destroy, IOTHUB_MESSAGE_HANDLE, iotHubMessageHandle);
DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubTransportuAMQPMocks, , IOTHUB_MESSAGE_RESULT, IoTHubMessage_GetByteArray, IOTHUB_MESSAGE_HANDLE, iotHubMessageHandle, const unsigned char**, buffer, size_t*, size);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportuAMQPMocks, , IOTHUB_MESSAGE_HANDLE, IoTHubMessage_Clone, IOTHUB_MESSAGE_HANDLE, iotHubMessageHandle);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportuAMQPMocks, , MAP_HANDLE, IoTHubMessage_Properties, IOTHUB_MESSAGE_HANDLE, iotHubMessageHandle);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportuAMQPMocks, , IOTHUBMESSAGE_CONTENT_TYPE, IoTHubMessage_GetContentType, IOTHUB_MESSAGE_HANDLE, iotHubMessageHandle)
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportuAMQPMocks, , IOTHUB_MESSAGE_RESULT, IoTHubMessage_SetMessageId, IOTHUB_MESSAGE_HANDLE, iotHubMessageHandle, const char*, messageId);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportuAMQPMocks, , const char*, IoTHubMessage_GetMessageId, IOTHUB_MESSAGE_HANDLE, iotHubMessageHandle);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportuAMQPMocks, , IOTHUB_MESSAGE_RESULT, IoTHubMessage_SetCorrelationId, IOTHUB_MESSAGE_HANDLE, iotHubMessageHandle, const char*, messageId);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportuAMQPMocks, , const char*, IoTHubMessage_GetCorrelationId, IOTHUB_MESSAGE_HANDLE, iotHubMessageHandle);

DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportuAMQPMocks, , void, DList_InitializeListHead, PDLIST_ENTRY, listHead);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportuAMQPMocks, , int, DList_IsListEmpty, PDLIST_ENTRY, listHead);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportuAMQPMocks, , void, DList_InsertTailList, PDLIST_ENTRY, listHead, PDLIST_ENTRY, listEntry);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportuAMQPMocks, , void, DList_AppendTailList, PDLIST_ENTRY, listHead, PDLIST_ENTRY, ListToAppend);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportuAMQPMocks, , int, DList_RemoveEntryList, PDLIST_ENTRY, listEntry);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportuAMQPMocks, , PDLIST_ENTRY, DList_RemoveHeadList, PDLIST_ENTRY, listHead);

DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportuAMQPMocks, , STRING_HANDLE, URL_EncodeString, const char*, input);

DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportuAMQPMocks, , STRING_HANDLE, STRING_new_with_memory, const char*, memory);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportuAMQPMocks, , STRING_HANDLE, STRING_construct, const char*, psz);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportuAMQPMocks, , int, STRING_concat, STRING_HANDLE, handle, const char*, s2);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportuAMQPMocks, , int, STRING_concat_with_STRING, STRING_HANDLE, s1, STRING_HANDLE, s2);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportuAMQPMocks, , void, STRING_delete, STRING_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportuAMQPMocks, , const char*, STRING_c_str, STRING_HANDLE, s);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportuAMQPMocks, , size_t, STRING_length, STRING_HANDLE, s)
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportuAMQPMocks, , int, STRING_empty, STRING_HANDLE, s)
DECLARE_GLOBAL_MOCK_METHOD_0(CIoTHubTransportuAMQPMocks, , STRING_HANDLE, STRING_new)
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportuAMQPMocks, , int, STRING_copy, STRING_HANDLE, s1, const char*, s2);

DECLARE_GLOBAL_MOCK_METHOD_0(CIoTHubTransportuAMQPMocks, , BUFFER_HANDLE, BUFFER_new);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportuAMQPMocks, , void, BUFFER_delete, BUFFER_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportuAMQPMocks, , unsigned char*, BUFFER_u_char, BUFFER_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportuAMQPMocks, , size_t, BUFFER_length, BUFFER_HANDLE, handle);

DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubTransportuAMQPMocks, , int, size_tToString, char*, destination, size_t, destinationSize, size_t, value);

DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportuAMQPMocks, , void*, gballoc_malloc, size_t, size);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportuAMQPMocks, , void*, gballoc_realloc, void*, ptr, size_t, size);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportuAMQPMocks, , void, gballoc_free, void*, ptr);

DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportuAMQPMocks, , IOTHUBMESSAGE_DISPOSITION_RESULT, IoTHubClient_LL_MessageCallback, IOTHUB_CLIENT_LL_HANDLE, handle, IOTHUB_MESSAGE_HANDLE, messageHandle);
DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubTransportuAMQPMocks, , void, IoTHubClient_LL_SendComplete, IOTHUB_CLIENT_LL_HANDLE, handle, PDLIST_ENTRY, completedMessages, IOTHUB_BATCHSTATE_RESULT, batchResult);

DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportuAMQPMocks, , time_t, get_time, time_t*, t)
DECLARE_GLOBAL_MOCK_METHOD_4(CIoTHubTransportuAMQPMocks, , STRING_HANDLE, SASToken_Create, STRING_HANDLE, key, STRING_HANDLE, scope, STRING_HANDLE, keyName, size_t, expiry)

DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportuAMQPMocks, , int, mallocAndStrcpy_s, char**, destination, const char*, source);


// platform.h
DECLARE_GLOBAL_MOCK_METHOD_0(CIoTHubTransportuAMQPMocks, , int, platform_init);
DECLARE_GLOBAL_MOCK_METHOD_0(CIoTHubTransportuAMQPMocks, , void, platform_deinit);

// xio.h
DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubTransportuAMQPMocks, , XIO_HANDLE, xio_create, const IO_INTERFACE_DESCRIPTION*, io_interface_description, const void*, io_create_parameters, LOGGER_LOG, logger_log);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportuAMQPMocks, , void, xio_destroy, XIO_HANDLE, xio);
DECLARE_GLOBAL_MOCK_METHOD_5(CIoTHubTransportuAMQPMocks, , int, xio_open, XIO_HANDLE, xio, ON_IO_OPEN_COMPLETE, on_io_open_complete, ON_BYTES_RECEIVED, on_bytes_received, ON_IO_ERROR, on_io_error, void*, callback_context);
DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubTransportuAMQPMocks, , int, xio_close, XIO_HANDLE, xio, ON_IO_CLOSE_COMPLETE, on_io_close_complete, void*, callback_context);
DECLARE_GLOBAL_MOCK_METHOD_5(CIoTHubTransportuAMQPMocks, , int, xio_send, XIO_HANDLE, xio, const void*, buffer, size_t, size, ON_SEND_COMPLETE, on_send_complete, void*, callback_context);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportuAMQPMocks, , void, xio_dowork, XIO_HANDLE, xio);

// tlsio_openssl.h
DECLARE_GLOBAL_MOCK_METHOD_0(CIoTHubTransportuAMQPMocks, , int, tlsio_openssl_init);
DECLARE_GLOBAL_MOCK_METHOD_0(CIoTHubTransportuAMQPMocks, , void, tlsio_openssl_deinit);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportuAMQPMocks, , CONCRETE_IO_HANDLE, tlsio_openssl_create, void*, io_create_parameters, LOGGER_LOG, logger_log);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportuAMQPMocks, , void, tlsio_openssl_destroy, CONCRETE_IO_HANDLE, tls_io);
DECLARE_GLOBAL_MOCK_METHOD_5(CIoTHubTransportuAMQPMocks, , int, tlsio_openssl_open, CONCRETE_IO_HANDLE, tls_io, ON_IO_OPEN_COMPLETE, on_io_open_complete, ON_BYTES_RECEIVED, on_bytes_received, ON_IO_ERROR, on_io_error, void*, callback_context);
DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubTransportuAMQPMocks, , int, tlsio_openssl_close, CONCRETE_IO_HANDLE, tls_io, ON_IO_CLOSE_COMPLETE, on_io_close_complete, void*, callback_context);
DECLARE_GLOBAL_MOCK_METHOD_5(CIoTHubTransportuAMQPMocks, , int, tlsio_openssl_send, CONCRETE_IO_HANDLE, tls_io, const void*, buffer, size_t, size, ON_SEND_COMPLETE, on_send_complete, void*, callback_context);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportuAMQPMocks, , void, tlsio_openssl_dowork, CONCRETE_IO_HANDLE, tls_io);
DECLARE_GLOBAL_MOCK_METHOD_0(CIoTHubTransportuAMQPMocks, , const IO_INTERFACE_DESCRIPTION*, tlsio_openssl_get_interface_description);

// tlsio_schannel.h
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportuAMQPMocks, , CONCRETE_IO_HANDLE, tlsio_schannel_create, void*, io_create_parameters, LOGGER_LOG, logger_log);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportuAMQPMocks, , void, tlsio_schannel_destroy, CONCRETE_IO_HANDLE, tls_io);
DECLARE_GLOBAL_MOCK_METHOD_5(CIoTHubTransportuAMQPMocks, , int, tlsio_schannel_open, CONCRETE_IO_HANDLE, tls_io, ON_IO_OPEN_COMPLETE, on_io_open_complete, ON_BYTES_RECEIVED, on_bytes_received, ON_IO_ERROR, on_io_error, void*, callback_context);
DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubTransportuAMQPMocks, , int, tlsio_schannel_close, CONCRETE_IO_HANDLE, tls_io, ON_IO_CLOSE_COMPLETE, on_io_close_complete, void*, callback_context);
DECLARE_GLOBAL_MOCK_METHOD_5(CIoTHubTransportuAMQPMocks, , int, tlsio_schannel_send, CONCRETE_IO_HANDLE, tls_io, const void*, buffer, size_t, size, ON_SEND_COMPLETE, on_send_complete, void*, callback_context);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportuAMQPMocks, , void, tlsio_schannel_dowork, CONCRETE_IO_HANDLE, tls_io);
DECLARE_GLOBAL_MOCK_METHOD_0(CIoTHubTransportuAMQPMocks, , const IO_INTERFACE_DESCRIPTION*, tlsio_schannel_get_interface_description);

// uAMQP
// cbs.h
DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubTransportuAMQPMocks, , CBS_HANDLE, cbs_create, SESSION_HANDLE, session, ON_AMQP_MANAGEMENT_STATE_CHANGED, on_amqp_management_state_changed, void*, callback_context);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportuAMQPMocks, , void, cbs_destroy, CBS_HANDLE, cbs);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportuAMQPMocks, , int, cbs_open, CBS_HANDLE, amqp_management);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportuAMQPMocks, , int, cbs_close, CBS_HANDLE, amqp_management);
DECLARE_GLOBAL_MOCK_METHOD_6(CIoTHubTransportuAMQPMocks, , int, cbs_put_token, CBS_HANDLE, cbs, const char*, type, const char*, audience, const char*, token, ON_CBS_OPERATION_COMPLETE, on_cbs_operation_complete, void*, context);
DECLARE_GLOBAL_MOCK_METHOD_5(CIoTHubTransportuAMQPMocks, , int, cbs_delete_token, CBS_HANDLE, cbs, const char*, type, const char*, audience, ON_CBS_OPERATION_COMPLETE, on_cbs_operation_complete, void*, context);

// connection.h
DECLARE_GLOBAL_MOCK_METHOD_5(CIoTHubTransportuAMQPMocks, , CONNECTION_HANDLE, connection_create, XIO_HANDLE, io, const char*, hostname, const char*, container_id, ON_NEW_ENDPOINT, on_new_endpoint, void*, callback_context);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportuAMQPMocks, , void, connection_dowork, CONNECTION_HANDLE, connection);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportuAMQPMocks, , void, connection_destroy, CONNECTION_HANDLE, connection);

// link.h
DECLARE_GLOBAL_MOCK_METHOD_5(CIoTHubTransportuAMQPMocks, , LINK_HANDLE, link_create, SESSION_HANDLE, session, const char*, name, role, _role, AMQP_VALUE, source, AMQP_VALUE, target);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportuAMQPMocks, , int, link_set_max_message_size, LINK_HANDLE, link, uint64_t, max_message_size);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportuAMQPMocks, , int, link_set_rcv_settle_mode, LINK_HANDLE, link, receiver_settle_mode, rcv_settle_mode);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportuAMQPMocks, , void, link_destroy, LINK_HANDLE, handle);

// message.h
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportuAMQPMocks, , int, message_add_body_amqp_data, MESSAGE_HANDLE, message, BINARY_DATA, binary_data);
DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubTransportuAMQPMocks, , int, message_get_body_amqp_data, MESSAGE_HANDLE, message, size_t, index, BINARY_DATA*, binary_data);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportuAMQPMocks, , int, message_get_body_type, MESSAGE_HANDLE, message, MESSAGE_BODY_TYPE*, body_type);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportuAMQPMocks, , void, message_destroy, MESSAGE_HANDLE, message);

// message_receiver.h
DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubTransportuAMQPMocks, , MESSAGE_RECEIVER_HANDLE, messagereceiver_create, LINK_HANDLE, link, ON_MESSAGE_RECEIVER_STATE_CHANGED, on_message_receiver_state_changed, void*, context);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportuAMQPMocks, , void, messagereceiver_destroy, MESSAGE_RECEIVER_HANDLE, message_receiver);
DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubTransportuAMQPMocks, , int, messagereceiver_open, MESSAGE_RECEIVER_HANDLE, message_receiver, ON_MESSAGE_RECEIVED, on_message_received, const void*, callback_context);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportuAMQPMocks, , int, messagereceiver_close, MESSAGE_RECEIVER_HANDLE, message_receiver);

// message_sender.h
DECLARE_GLOBAL_MOCK_METHOD_4(CIoTHubTransportuAMQPMocks, , MESSAGE_SENDER_HANDLE, messagesender_create, LINK_HANDLE, link, ON_MESSAGE_SENDER_STATE_CHANGED, on_message_sender_state_changed, void*, context, LOGGER_LOG, logger_log);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportuAMQPMocks, , void, messagesender_destroy, MESSAGE_SENDER_HANDLE, message_sender);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportuAMQPMocks, , int, messagesender_open, MESSAGE_SENDER_HANDLE, message_sender);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportuAMQPMocks, , int, messagesender_close, MESSAGE_SENDER_HANDLE, message_sender);
DECLARE_GLOBAL_MOCK_METHOD_4(CIoTHubTransportuAMQPMocks, , int, messagesender_send, MESSAGE_SENDER_HANDLE, message_sender, MESSAGE_HANDLE, message, ON_MESSAGE_SEND_COMPLETE, on_message_send_complete, const void*, callback_context);

// messaging.h
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportuAMQPMocks, , AMQP_VALUE, messaging_create_source, const char*, address);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportuAMQPMocks, , AMQP_VALUE, messaging_create_target, const char*, address);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportuAMQPMocks, , AMQP_VALUE, messaging_delivery_received, uint32_t, section_number, uint64_t, section_offset);
DECLARE_GLOBAL_MOCK_METHOD_0(CIoTHubTransportuAMQPMocks, , AMQP_VALUE, messaging_delivery_accepted);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportuAMQPMocks, , AMQP_VALUE, messaging_delivery_rejected, const char*, error_condition, const char*, error_description);
DECLARE_GLOBAL_MOCK_METHOD_0(CIoTHubTransportuAMQPMocks, , AMQP_VALUE, messaging_delivery_released);
DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubTransportuAMQPMocks, , AMQP_VALUE, messaging_delivery_modified, bool, delivery_failed, bool, undeliverable_here, fields, message_annotations);

// sasl_mechanism.h
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportuAMQPMocks, , SASL_MECHANISM_HANDLE, saslmechanism_create, const SASL_MECHANISM_INTERFACE_DESCRIPTION*, sasl_mechanism_interface_description, void*, sasl_mechanism_create_parameters);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportuAMQPMocks, , void, saslmechanism_destroy, SASL_MECHANISM_HANDLE, sasl_mechanism);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportuAMQPMocks, , int, saslmechanism_get_init_bytes, SASL_MECHANISM_HANDLE, sasl_mechanism, SASL_MECHANISM_BYTES*, init_bytes);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportuAMQPMocks, , const char*, saslmechanism_get_mechanism_name, SASL_MECHANISM_HANDLE, sasl_mechanism);
DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubTransportuAMQPMocks, , int, saslmechanism_challenge, SASL_MECHANISM_HANDLE, sasl_mechanism, const SASL_MECHANISM_BYTES*, challenge_bytes, SASL_MECHANISM_BYTES*, response_bytes);

// saslclientio.h
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportuAMQPMocks, , CONCRETE_IO_HANDLE, saslclientio_create, void*, io_create_parameters, LOGGER_LOG, logger_log);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportuAMQPMocks, , void, saslclientio_destroy, CONCRETE_IO_HANDLE, sasl_client_io);
DECLARE_GLOBAL_MOCK_METHOD_5(CIoTHubTransportuAMQPMocks, , int, saslclientio_open, CONCRETE_IO_HANDLE, sasl_client_io, ON_IO_OPEN_COMPLETE, on_io_open_complete, ON_BYTES_RECEIVED, on_bytes_received, ON_IO_ERROR, on_io_error, void*, callback_context);
DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubTransportuAMQPMocks, , int, saslclientio_close, CONCRETE_IO_HANDLE, sasl_client_io, ON_IO_CLOSE_COMPLETE, on_io_close_complete, void*, callback_context);
DECLARE_GLOBAL_MOCK_METHOD_5(CIoTHubTransportuAMQPMocks, , int, saslclientio_send, CONCRETE_IO_HANDLE, sasl_client_io, const void*, buffer, size_t, size, ON_SEND_COMPLETE, on_send_complete, void*, callback_context);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportuAMQPMocks, , void, saslclientio_dowork, CONCRETE_IO_HANDLE, sasl_client_io);
DECLARE_GLOBAL_MOCK_METHOD_0(CIoTHubTransportuAMQPMocks, , const IO_INTERFACE_DESCRIPTION*, saslclientio_get_interface_description);

// sasl_mssbcbs.h
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportuAMQPMocks, , CONCRETE_SASL_MECHANISM_HANDLE, saslmssbcbs_create, void*, config);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportuAMQPMocks, , void, saslmssbcbs_destroy, CONCRETE_SASL_MECHANISM_HANDLE, sasl_mechanism_concrete_handle);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportuAMQPMocks, , int, saslmssbcbs_get_init_bytes, CONCRETE_SASL_MECHANISM_HANDLE, sasl_mechanism_concrete_handle, SASL_MECHANISM_BYTES*, init_bytes);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportuAMQPMocks, , const char*, saslmssbcbs_get_mechanism_name, CONCRETE_SASL_MECHANISM_HANDLE, sasl_mechanism);
DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubTransportuAMQPMocks, , int, saslmssbcbs_challenge, CONCRETE_SASL_MECHANISM_HANDLE, concrete_sasl_mechanism, const SASL_MECHANISM_BYTES*, challenge_bytes, SASL_MECHANISM_BYTES*, response_bytes);
DECLARE_GLOBAL_MOCK_METHOD_0(CIoTHubTransportuAMQPMocks, , const SASL_MECHANISM_INTERFACE_DESCRIPTION*, saslmssbcbs_get_interface);

// session.h
DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubTransportuAMQPMocks, , SESSION_HANDLE, session_create, CONNECTION_HANDLE, connection, ON_LINK_ATTACHED, on_link_attached, void*, callback_context);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportuAMQPMocks, , int, session_set_incoming_window, SESSION_HANDLE, session, uint32_t, incoming_window);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportuAMQPMocks, , int, session_get_incoming_window, SESSION_HANDLE, session, uint32_t*, incoming_window);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportuAMQPMocks, , int, session_set_outgoing_window, SESSION_HANDLE, session, uint32_t, outgoing_window);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubTransportuAMQPMocks, , int, session_get_outgoing_window, SESSION_HANDLE, session, uint32_t*, outgoing_window);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubTransportuAMQPMocks, , void, session_destroy, SESSION_HANDLE, session);


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

void setExpectedCallsForTransportCreateUpTo(CIoTHubTransportuAMQPMocks& mocks, IOTHUBTRANSPORT_CONFIG* config, int maximumStepToSet)
{
	int step;
	for (step = 0; step <= maximumStepToSet; step++)
	{
		if (step == STEP_CREATE_LIST_INIT)
		{
			STRICT_EXPECTED_CALL(mocks, gballoc_malloc(0)).IgnoreArgument(1);
			STRICT_EXPECTED_CALL(mocks, DList_InitializeListHead(0)).IgnoreAllArguments();
		}
		else if (step == STEP_CREATE_IOTHUB_FQDN)
		{
			STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(config->upperConfig->iotHubName) + strlen(config->upperConfig->iotHubSuffix) + 2));
			EXPECTED_CALL(mocks, STRING_construct(0));
			EXPECTED_CALL(mocks, gballoc_free(0));
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
			STRICT_EXPECTED_CALL(mocks, STRING_new());
			STRICT_EXPECTED_CALL(mocks, STRING_copy(0, config->upperConfig->deviceKey)).IgnoreArgument(1);
		}
	}
}

void setExpectedCleanupCallsForTransportCreateUpTo(CIoTHubTransportuAMQPMocks& mocks, IOTHUBTRANSPORT_CONFIG* config, int maximumStepToCleanup)
{
	int step;
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

void setExpectedCallsForSASTokenExpiryCheck(CIoTHubTransportuAMQPMocks& mocks, IOTHUBTRANSPORT_CONFIG* config, time_t current_time)
{
	STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(current_time);
}

void setExpectedCallsForCbsAuthentication(CIoTHubTransportuAMQPMocks& mocks, IOTHUBTRANSPORT_CONFIG* config, time_t current_time)
{
	STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(current_time);
	EXPECTED_CALL(mocks, SASToken_Create(NULL, NULL, NULL, 0));
	EXPECTED_CALL(mocks, STRING_c_str(NULL));
	EXPECTED_CALL(mocks, STRING_c_str(NULL));
	EXPECTED_CALL(mocks, cbs_put_token(NULL, NULL, NULL, NULL, NULL, NULL));
	EXPECTED_CALL(mocks, STRING_delete(NULL));
}

void setExpectedCallsForCbsAuthTimeoutCheck(CIoTHubTransportuAMQPMocks& mocks, IOTHUBTRANSPORT_CONFIG* config, time_t current_time)
{
	STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(current_time);
}

void setExpectedCallsForCreateMessageReceiver(CIoTHubTransportuAMQPMocks& mocks, IOTHUBTRANSPORT_CONFIG* config)
{
	EXPECTED_CALL(mocks, STRING_c_str(NULL));
	EXPECTED_CALL(mocks, messaging_create_source(NULL)).SetReturn(TEST_MESSAGERECEIVER_SOURCE);
	EXPECTED_CALL(mocks, messaging_create_target(NULL)).SetReturn(TEST_MESSAGERECEIVER_TARGET);
	EXPECTED_CALL(mocks, link_create(NULL, NULL, NULL, NULL, NULL)).SetReturn(TEST_MESSAGERECEIVER_LINK);
	STRICT_EXPECTED_CALL(mocks, link_set_rcv_settle_mode(NULL, receiver_settle_mode_first)).IgnoreArgument(1);
	EXPECTED_CALL(mocks, link_set_max_message_size(NULL, TEST_MESSAGE_RECEIVER_MAX_LINK_SIZE)).IgnoreArgument(1);
	EXPECTED_CALL(mocks, messagereceiver_create(NULL, NULL, NULL)).SetReturn(TEST_MESSAGE_RECEIVER);
	EXPECTED_CALL(mocks, messagereceiver_open(NULL, NULL, NULL));
	STRICT_EXPECTED_CALL(mocks, amqpvalue_destroy(TEST_MESSAGERECEIVER_SOURCE));
	STRICT_EXPECTED_CALL(mocks, amqpvalue_destroy(TEST_MESSAGERECEIVER_TARGET));
}

void setExpectedCallsForDestroyMessageReceiver(CIoTHubTransportuAMQPMocks& mocks, IOTHUBTRANSPORT_CONFIG* config)
{
	STRICT_EXPECTED_CALL(mocks, messagereceiver_close(TEST_MESSAGE_RECEIVER));
	STRICT_EXPECTED_CALL(mocks, messagereceiver_destroy(TEST_MESSAGE_RECEIVER));
	STRICT_EXPECTED_CALL(mocks, link_destroy(TEST_MESSAGERECEIVER_LINK));
}

void setExpectedCallsForCreateEventSender(CIoTHubTransportuAMQPMocks& mocks, IOTHUBTRANSPORT_CONFIG* config)
{
	STRICT_EXPECTED_CALL(mocks, messaging_create_source(TEST_MESSAGE_SENDER_SOURCE_ADDRESS)).SetReturn(TEST_MESSAGESENDER_SOURCE);
	EXPECTED_CALL(mocks, STRING_c_str(NULL));
	EXPECTED_CALL(mocks, messaging_create_target(NULL)).SetReturn(TEST_MESSAGESENDER_TARGET);
	EXPECTED_CALL(mocks, link_create(NULL, NULL, NULL, NULL, NULL)).SetReturn(TEST_MESSAGESENDER_LINK);
	STRICT_EXPECTED_CALL(mocks, link_set_max_message_size(NULL, TEST_MESSAGE_SENDER_MAX_LINK_SIZE)).IgnoreArgument(1);
	EXPECTED_CALL(mocks, messagesender_create(NULL, NULL, NULL, NULL)).SetReturn(TEST_MESSAGE_SENDER);
	EXPECTED_CALL(mocks, messagesender_open(TEST_MESSAGE_SENDER));
	EXPECTED_CALL(mocks, amqpvalue_destroy(TEST_MESSAGESENDER_SOURCE));
	EXPECTED_CALL(mocks, amqpvalue_destroy(TEST_MESSAGESENDER_TARGET));
}

void setExpectedCallsForDestroyEventSender(CIoTHubTransportuAMQPMocks& mocks, IOTHUBTRANSPORT_CONFIG* config)
{
	STRICT_EXPECTED_CALL(mocks, messagesender_destroy(TEST_MESSAGE_SENDER));
	STRICT_EXPECTED_CALL(mocks, link_destroy(TEST_MESSAGESENDER_LINK));
}

void setExpectedCallsForSendPendingEvents_SingleEvent(CIoTHubTransportuAMQPMocks& mocks, IOTHUBMESSAGE_CONTENT_TYPE message_type, time_t current_time)
{
	EXPECTED_CALL(mocks, DList_IsListEmpty(NULL)).SetReturn(0);
	EXPECTED_CALL(mocks, IoTHubMessage_GetContentType(TEST_MESSAGE_HANDLE)).SetReturn(message_type);

	EXPECTED_CALL(mocks, gballoc_malloc(NULL));

	STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(current_time);
	EXPECTED_CALL(mocks, DList_InitializeListHead(NULL));
	EXPECTED_CALL(mocks, DList_RemoveEntryList(NULL));
	EXPECTED_CALL(mocks, DList_InsertTailList(NULL, NULL));

	if (message_type == IOTHUBMESSAGE_BYTEARRAY)
	{
		EXPECTED_CALL(mocks, IoTHubMessage_GetByteArray(NULL, NULL, NULL));
	}
	else if (message_type == IOTHUBMESSAGE_STRING)
	{
		EXPECTED_CALL(mocks, IoTHubMessage_GetString(NULL));
	}

	EXPECTED_CALL(mocks, message_create()).SetReturn(TEST_EVENT_MESSAGE_HANDLE);
	STRICT_EXPECTED_CALL(mocks, message_add_body_amqp_data(NULL, test_binary_data)).IgnoreArgument(1);
	EXPECTED_CALL(mocks, messagesender_send(NULL, NULL, NULL, NULL));
	STRICT_EXPECTED_CALL(mocks, message_destroy(TEST_EVENT_MESSAGE_HANDLE));
}

void setExpectedCallsForSendPendingEvents(CIoTHubTransportuAMQPMocks& mocks, IOTHUBMESSAGE_CONTENT_TYPE message_type, time_t current_time, int numberOfEvents)
{
	while (numberOfEvents-- > 0)
	{
		setExpectedCallsForSendPendingEvents_SingleEvent(mocks, message_type, current_time);
	}

	EXPECTED_CALL(mocks, DList_IsListEmpty(NULL)).SetReturn(1);
}

void setExpectedCallsForConnectionDoWork(CIoTHubTransportuAMQPMocks& mocks, IOTHUBTRANSPORT_CONFIG* config)
{
	EXPECTED_CALL(mocks, connection_dowork(NULL));
}

void setExpectedCallsForHandleEventSendTimeout(CIoTHubTransportuAMQPMocks& mocks, IOTHUBTRANSPORT_CONFIG* config, time_t current_time)
{
	STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(current_time);
}

void setExpectedCallsForRollEventsBackToWaitList(CIoTHubTransportuAMQPMocks& mocks, IOTHUBTRANSPORT_CONFIG* config)
{
}

void setExpectedCallsForTransportDestroy(CIoTHubTransportuAMQPMocks& mocks, IOTHUBTRANSPORT_CONFIG* config, bool isConnectionInitialized, bool destroyIOtransport, size_t numberOfEventsInProgress)
{
	if (isConnectionInitialized)
	{
		EXPECTED_CALL(mocks, messagesender_destroy(0));
		EXPECTED_CALL(mocks, messagereceiver_destroy(0));
		EXPECTED_CALL(mocks, cbs_destroy(0));
		EXPECTED_CALL(mocks, session_destroy(0));
		EXPECTED_CALL(mocks, connection_destroy(0));
		EXPECTED_CALL(mocks, xio_destroy(0));
		EXPECTED_CALL(mocks, saslmechanism_destroy(0));
	}

	if (destroyIOtransport)
	{
		EXPECTED_CALL(mocks, xio_destroy(0));
	}

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

	setExpectedCallsForRollEventsBackToWaitList(mocks, config);

	EXPECTED_CALL(mocks, gballoc_free(NULL));
}

void setExpectedCallsForTransportDoWorkUpTo(CIoTHubTransportuAMQPMocks& mocks, IOTHUBTRANSPORT_CONFIG* config, int maximumStepToSet, int messageReceiverCreation)
{
	int step;
	for (step = 0; step <= maximumStepToSet; step++)
	{
		if (step == STEP_DOWORK_GET_TLS_IO)
		{
			EXPECTED_CALL(mocks, STRING_c_str(NULL));
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
			EXPECTED_CALL(mocks, xio_create(NULL, NULL, NULL)).SetReturn(TEST_SASL_IO);
		}
		else if (step == STEP_DOWORK_CREATE_CONNECTION)
		{
			EXPECTED_CALL(mocks, STRING_c_str(NULL));
			EXPECTED_CALL(mocks, connection_create(NULL, NULL, NULL, NULL, NULL));
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
			STRICT_EXPECTED_CALL(mocks, get_time(NULL));
		}
		else if (step == STEP_DOWORK_AUTHENTICATION)
		{
			setExpectedCallsForCbsAuthentication(mocks, config, get_time(NULL));
		}
		else if (step == STEP_DOWORK_MESSAGERECEIVER)
		{
			if (messageReceiverCreation == DOWORK_MESSAGERECEIVER_CREATE)
			{
				setExpectedCallsForCreateMessageReceiver(mocks, config);
			}
			else if (messageReceiverCreation == DOWORK_MESSAGERECEIVER_DESTROY)
			{
				setExpectedCallsForDestroyMessageReceiver(mocks, config);
			}
		}
		else if (step == STEP_DOWORK_MESSAGESENDER)
		{
			setExpectedCallsForCreateEventSender(mocks, config);
		}
	}
}

void setExpectedCallsForConnectionDestroyUpTo(CIoTHubTransportuAMQPMocks& mocks, IOTHUBTRANSPORT_CONFIG* config, int maximumStepToSet)
{
	int step;
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
	}
}

void test_IOTHUB_CLIENT_EVENT_CONFIRMATION_CALLBACK(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void* userContextCallback)
{
	test_number_of_event_confirmation_callbacks_invoked++;

	if (userContextCallback != NULL)
	{
		test_sum_of_event_confirmation_callback_contexts += (int)userContextCallback;
	}
}

void addTestEvents(PDLIST_ENTRY waitingToSend, int numberOfEvents, bool setCallback)
{
	while (numberOfEvents-- > 0)
	{
		IOTHUB_MESSAGE_LIST* iml = (IOTHUB_MESSAGE_LIST*)malloc(sizeof(IOTHUB_MESSAGE_LIST));

		if (iml == NULL)
		{
			LogError("Test failure: addTestEvents() failed allocating IOTHUB_MESSAGE_LIST (%i).\r\n", numberOfEvents);
		}
		else
		{
			iml->messageHandle = TEST_MESSAGE_HANDLE;

			if (setCallback)
			{
				iml->context = (void*)numberOfEvents;
				iml->callback = test_IOTHUB_CLIENT_EVENT_CONFIRMATION_CALLBACK;
			}

			DList_InsertTailList(waitingToSend, &(iml->entry));
		}
	}
}

XIO_HANDLE getIOTransport(const char* fqdn, int port)
{
	if (fqdn != NULL)
	{
		if (test_latest_getIOTransport_fqdn != NULL)
		{
			BASEIMPLEMENTATION::gballoc_free(test_latest_getIOTransport_fqdn);
		}
		
		if ((test_latest_getIOTransport_fqdn = (char*)malloc(sizeof(char) * strlen(fqdn))) == NULL)
		{
			LogError("Test failure: failed allocating test_latest_getIOTransport_fqdn.\r\n");
		}
		else
		{
			strcpy(test_latest_getIOTransport_fqdn, fqdn);
		}
	}
	else
	{
		test_latest_getIOTransport_fqdn = NULL;
	}

	test_latest_getIOTransport_port = port;

	return test_TLS_io_transport;
}

static void resetTestSuiteState()
{
	fail_malloc = false;
	fail_STRING_new = false;
	fail_STRING_new_with_memory = false;
	fail_STRING_construct = false;
}

static time_t addSecondsToTime(time_t reference_time, size_t seconds_to_add)
{
	time_t result = NULL;

	tm cal_time = *localtime(&reference_time);
	cal_time.tm_sec += seconds_to_add;
	result = mktime(&cal_time);

	return result;
}


BEGIN_TEST_SUITE(iothubtransportuamqp_unittests)

TEST_SUITE_INITIALIZE(TestClassInitialize)
{
    INITIALIZE_MEMORY_DEBUG(g_dllByDll);
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
    DEINITIALIZE_MEMORY_DEBUG(g_dllByDll);
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


// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_005: [If parameter config (or its fields) is NULL then IoTHubTransportuAMQP_Create shall fail and return NULL.] 
TEST_FUNCTION(uAMQP_Create_with_null_config_fails)
{
    // arrange
    TRANSPORT_PROVIDER* transport_interface;
	transport_interface = (TRANSPORT_PROVIDER*)uAMQP_Protocol();

    // act
    TRANSPORT_HANDLE transportHandle = transport_interface->IoTHubTransport_Create(NULL);

    // assert
    ASSERT_IS_NULL(transportHandle);
}

// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_006: [IoTHubTransportuAMQP_Create shall fail and return NULL if any fields of the config structure are NULL.]
TEST_FUNCTION(uAMQP_Create_with_config_null_field1_fails)
{
	// arrange
	IOTHUBTRANSPORT_CONFIG config;
	TRANSPORT_PROVIDER* transport_interface;

	config.upperConfig = NULL;
	transport_interface = (TRANSPORT_PROVIDER*)uAMQP_Protocol();

	// act
	TRANSPORT_HANDLE transportHandle = transport_interface->IoTHubTransport_Create(&config);

	// assert
	ASSERT_IS_NULL(transportHandle);
}

// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_006: [IoTHubTransportuAMQP_Create shall fail and return NULL if any fields of the config structure are NULL.]
TEST_FUNCTION(uAMQP_Create_with_config_null_field2_fails)
{
	// arrange
	IOTHUBTRANSPORT_CONFIG config;
	TRANSPORT_PROVIDER* transport_interface;

	config.waitingToSend = NULL;
	transport_interface = (TRANSPORT_PROVIDER*)uAMQP_Protocol();

	// act
	TRANSPORT_HANDLE transportHandle = transport_interface->IoTHubTransport_Create(&config);

	// assert
	ASSERT_IS_NULL(transportHandle);
}

// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_007: [IoTHubTransportuAMQP_Create shall fail and return NULL if the deviceId length is greater than 128.]
TEST_FUNCTION(uAMQP_Create_with_config_deviceId_too_long_fails)
{
	// arrange
	DLIST_ENTRY wts;
	TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)uAMQP_Protocol();

	IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
		"012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678",
		TEST_DEVICE_KEY, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME, (IO_TRANSPORT_PROVIDER_CALLBACK)getIOTransport };

	IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };
	
	// act
	TRANSPORT_HANDLE transportHandle = transport_interface->IoTHubTransport_Create(&config);

	// assert
	ASSERT_IS_NULL(transportHandle);
}

// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_008: [IoTHubTransportuAMQP_Create shall fail and return NULL if any config field of type string is zero length.] 
TEST_FUNCTION(uAMQP_Create_with_config_deviceId_NULL_fails)
{
	// arrange
	DLIST_ENTRY wts;
	TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)uAMQP_Protocol();

	IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
		NULL, TEST_DEVICE_KEY, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME, (IO_TRANSPORT_PROVIDER_CALLBACK)getIOTransport };

	IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };

	// act
	TRANSPORT_HANDLE transportHandle = transport_interface->IoTHubTransport_Create(&config);

	// assert
	ASSERT_IS_NULL(transportHandle);
}

// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_008: [IoTHubTransportuAMQP_Create shall fail and return NULL if any config field of type string is zero length.] 
TEST_FUNCTION(uAMQP_Create_with_config_deviceKey_NULL_fails)
{
	// arrange
	DLIST_ENTRY wts;
	TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)uAMQP_Protocol();

	IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
		TEST_DEVICE_ID, NULL, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME, (IO_TRANSPORT_PROVIDER_CALLBACK)getIOTransport };

	IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };

	// act
	TRANSPORT_HANDLE transportHandle = transport_interface->IoTHubTransport_Create(&config);

	// assert
	ASSERT_IS_NULL(transportHandle);
}

// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_008: [IoTHubTransportuAMQP_Create shall fail and return NULL if any config field of type string is zero length.] 
TEST_FUNCTION(uAMQP_Create_with_config_hubName_NULL_fails)
{
	// arrange
	DLIST_ENTRY wts;
	TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)uAMQP_Protocol();

	IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
		TEST_DEVICE_ID, TEST_DEVICE_KEY, NULL, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME, (IO_TRANSPORT_PROVIDER_CALLBACK)getIOTransport };

	IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };

	// act
	TRANSPORT_HANDLE transportHandle = transport_interface->IoTHubTransport_Create(&config);

	// assert
	ASSERT_IS_NULL(transportHandle);
}

// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_008: [IoTHubTransportuAMQP_Create shall fail and return NULL if any config field of type string is zero length.] 
TEST_FUNCTION(uAMQP_Create_with_config_hubSuffix_NULL_fails)
{
	// arrange
	DLIST_ENTRY wts;
	TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)uAMQP_Protocol();

	IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
		TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOT_HUB_NAME, NULL, TEST_PROT_GW_HOSTNAME, (IO_TRANSPORT_PROVIDER_CALLBACK)getIOTransport };

	IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };

	// act
	TRANSPORT_HANDLE transportHandle = transport_interface->IoTHubTransport_Create(&config);

	// assert
	ASSERT_IS_NULL(transportHandle);
}

// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_134: [IoTHubTransportuAMQP_Create shall fail and return NULL if the combined length of config->iotHubName and config->iotHubSuffix exceeds 254 bytes (RFC1035)]
TEST_FUNCTION(uAMQP_Create_with_config_hubFqdn_too_long_fails)
{
	// arrange
	DLIST_ENTRY wts;
	TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)uAMQP_Protocol();

	IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
		TEST_DEVICE_ID, TEST_DEVICE_KEY, 
		"0123456789012345678901234567890123456789", 
		"01234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234", 
		TEST_PROT_GW_HOSTNAME, (IO_TRANSPORT_PROVIDER_CALLBACK)getIOTransport };

	IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };

	// act
	TRANSPORT_HANDLE transportHandle = transport_interface->IoTHubTransport_Create(&config);

	// assert
	ASSERT_IS_NULL(transportHandle);
}

// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_009: [IoTHubTransportuAMQP_Create shall fail and return NULL if memory allocation of the transport’s internal state structure fails.]
TEST_FUNCTION(uAMQP_Create_transport_state_allocation_fails)
{
	// arrange
	CIoTHubTransportuAMQPMocks mocks;

	DLIST_ENTRY wts;
	BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
	TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)uAMQP_Protocol();

	IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
		TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME, (IO_TRANSPORT_PROVIDER_CALLBACK)getIOTransport };

	IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };

	fail_malloc = true;

	STRICT_EXPECTED_CALL(mocks, gballoc_malloc(0)).IgnoreArgument(1);

	// act
	TRANSPORT_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);

	// assert
	ASSERT_IS_NULL(transport);
}

// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_010: [IoTHubTransportuAMQP_Create shall create an immutable string, referred to as iotHubHostFqdn, from the following pieces : config->iotHubName + "." + config->iotHubSuffix.]
// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_012: [IoTHubTransportuAMQP_Create shall create an immutable string, referred to as devicesPath, from the following parts : host_fqdn + “ / devices / ” + deviceId.]
// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_014: [IoTHubTransportuAMQP_Create shall create an immutable string, referred to as targetAddress, from the following parts: "amqps://" + devicesPath + "/messages/events".]
// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_053: [IoTHubTransportuAMQP_Create shall define the source address for receiving messages as "amqps://" + devicesPath + "/messages/devicebound", stored in the transport handle as messageReceiveAddress]
// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_016: [IoTHubTransportuAMQP_Create shall initialize handle->sasTokenKeyName with a zero-length STRING_HANDLE instance.] 
// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_018: [IoTHubTransportuAMQP_Create shall store a copy of config->deviceKey (passed by upper layer) into the transport’s own deviceKey field] 
// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_023: [If IoTHubTransportuAMQP_Create succeeds it shall return a non-NULL pointer to the structure that represents the transport.] 
TEST_FUNCTION(uAMQP_Create_succeeds)
{
	// arrange
	CIoTHubTransportuAMQPMocks mocks;

	DLIST_ENTRY wts;
	BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
	TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)uAMQP_Protocol();

	IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
		TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME, (IO_TRANSPORT_PROVIDER_CALLBACK)getIOTransport };
	IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };

	mocks.ResetAllCalls();
	setExpectedCallsForTransportCreateUpTo(mocks, &config, STEP_CREATE_DEVICEKEY);

	// act
	TRANSPORT_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);

	// assert
	ASSERT_IS_NOT_NULL(transport);
	mocks.AssertActualAndExpectedCalls();

	// cleanup
	transport_interface->IoTHubTransport_Destroy(transport);
}

// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_013: [If creating devicesPath fails for any reason then IoTHubTransportuAMQP_Create shall fail and return NULL.] 
TEST_FUNCTION(uAMQP_Create_devicesPath_malloc_fails)
{
	// arrange
	CIoTHubTransportuAMQPMocks mocks;

	DLIST_ENTRY wts;
	BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
	TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)uAMQP_Protocol();

	IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
		TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME, (IO_TRANSPORT_PROVIDER_CALLBACK)getIOTransport };
	IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };

	char* devicesPath = TEST_IOT_HUB_NAME "." TEST_IOT_HUB_SUFFIX "/devices/" TEST_DEVICE_ID;

	mocks.ResetAllCalls();
	setExpectedCallsForTransportCreateUpTo(mocks, &config, STEP_CREATE_IOTHUB_FQDN);
	EXPECTED_CALL(mocks, STRING_c_str(0));
	STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(devicesPath) + 1)).SetReturn((char*)NULL);
	setExpectedCleanupCallsForTransportCreateUpTo(mocks, &config, STEP_CREATE_IOTHUB_FQDN);

	// act
	TRANSPORT_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);

	// assert
	ASSERT_IS_NULL(transport);
	mocks.AssertActualAndExpectedCalls();
}

// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_013: [If creating devicesPath fails for any reason then IoTHubTransportuAMQP_Create shall fail and return NULL.] 
TEST_FUNCTION(uAMQP_Create_devicesPath_STRING_construct_fails)
{
	// arrange
	CIoTHubTransportuAMQPMocks mocks;

	DLIST_ENTRY wts;
	BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
	TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)uAMQP_Protocol();

	IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
		TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME, (IO_TRANSPORT_PROVIDER_CALLBACK)getIOTransport };
	IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };

	char* devicesPath = TEST_IOT_HUB_NAME "." TEST_IOT_HUB_SUFFIX "/devices/" TEST_DEVICE_ID;

	mocks.ResetAllCalls();
	setExpectedCallsForTransportCreateUpTo(mocks, &config, STEP_CREATE_IOTHUB_FQDN);
	EXPECTED_CALL(mocks, STRING_c_str(0));
	STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(devicesPath) + 1));
	STRICT_EXPECTED_CALL(mocks, STRING_construct(devicesPath)).SetReturn(TEST_NULL_STRING_HANDLE);
	EXPECTED_CALL(mocks, gballoc_free(0));
	setExpectedCleanupCallsForTransportCreateUpTo(mocks, &config, STEP_CREATE_IOTHUB_FQDN);
	
	// act
	TRANSPORT_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);

	// assert
	ASSERT_IS_NULL(transport);
	mocks.AssertActualAndExpectedCalls();
}

// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_015: [If creating the targetAddress fails for any reason then IoTHubTransportuAMQP_Create shall fail and return NULL.]
TEST_FUNCTION(uAMQP_Create_targetAddress_malloc_fails)
{
	// arrange
	CIoTHubTransportuAMQPMocks mocks;

	DLIST_ENTRY wts;
	BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
	TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)uAMQP_Protocol();

	IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
		TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME, (IO_TRANSPORT_PROVIDER_CALLBACK)getIOTransport };
	IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };

	char* targetAddress = "amqps://" TEST_IOT_HUB_NAME "." TEST_IOT_HUB_SUFFIX "/devices/" TEST_DEVICE_ID "/messages/events";

	mocks.ResetAllCalls();
	setExpectedCallsForTransportCreateUpTo(mocks, &config, STEP_CREATE_DEVICES_PATH);
	EXPECTED_CALL(mocks, STRING_c_str(0));
	STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(targetAddress) + 1)).SetReturn((char*)NULL);
	setExpectedCleanupCallsForTransportCreateUpTo(mocks, &config, STEP_CREATE_DEVICES_PATH);

	// act
	TRANSPORT_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);

	// assert
	ASSERT_IS_NULL(transport);
	mocks.AssertActualAndExpectedCalls();
}

// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_015: [If creating the targetAddress fails for any reason then IoTHubTransportuAMQP_Create shall fail and return NULL.] 
TEST_FUNCTION(uAMQP_Create_targetAddress_STRING_construct_fails)
{
	// arrange
	CIoTHubTransportuAMQPMocks mocks;

	DLIST_ENTRY wts;
	BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
	TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)uAMQP_Protocol();

	IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
		TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME, (IO_TRANSPORT_PROVIDER_CALLBACK)getIOTransport };
	IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };

	char* targetAddress = "amqps://" TEST_IOT_HUB_NAME "." TEST_IOT_HUB_SUFFIX "/devices/" TEST_DEVICE_ID "/messages/events";

	mocks.ResetAllCalls();
	setExpectedCallsForTransportCreateUpTo(mocks, &config, STEP_CREATE_DEVICES_PATH);
	EXPECTED_CALL(mocks, STRING_c_str(0));
	STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(targetAddress) + 1));
	STRICT_EXPECTED_CALL(mocks, STRING_construct(targetAddress)).SetReturn(TEST_NULL_STRING_HANDLE);
	EXPECTED_CALL(mocks, gballoc_free(0));
	setExpectedCleanupCallsForTransportCreateUpTo(mocks, &config, STEP_CREATE_DEVICES_PATH);

	// act
	TRANSPORT_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);

	// assert
	ASSERT_IS_NULL(transport);
	mocks.AssertActualAndExpectedCalls();
}

// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_054: [If creating the messageReceiveAddress fails for any reason then IoTHubTransportuAMQP_Create shall fail and return NULL.]
TEST_FUNCTION(uAMQP_Create_receiveAddress_malloc_fails)
{
	// arrange
	CIoTHubTransportuAMQPMocks mocks;

	DLIST_ENTRY wts;
	BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
	TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)uAMQP_Protocol();

	IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
		TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME, (IO_TRANSPORT_PROVIDER_CALLBACK)getIOTransport };
	IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };

	char* receiveAddress = "amqps://"  TEST_IOT_HUB_NAME "." TEST_IOT_HUB_SUFFIX "/devices/" TEST_DEVICE_ID "/messages/devicebound";

	mocks.ResetAllCalls();
	setExpectedCallsForTransportCreateUpTo(mocks, &config, STEP_CREATE_TARGET_ADDRESS);
	EXPECTED_CALL(mocks, STRING_c_str(0));
	STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(receiveAddress) + 1)).SetReturn((char*)NULL);
	setExpectedCleanupCallsForTransportCreateUpTo(mocks, &config, STEP_CREATE_TARGET_ADDRESS);

	// act
	TRANSPORT_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);

	// assert
	ASSERT_IS_NULL(transport);
	mocks.AssertActualAndExpectedCalls();
}

// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_054: [If creating the messageReceiveAddress fails for any reason then IoTHubTransportuAMQP_Create shall fail and return NULL.]
TEST_FUNCTION(uAMQP_Create_receiveAddress_STRING_construct_fails)
{
	// arrange
	CIoTHubTransportuAMQPMocks mocks;

	DLIST_ENTRY wts;
	BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
	TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)uAMQP_Protocol();

	IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
		TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME, (IO_TRANSPORT_PROVIDER_CALLBACK)getIOTransport };
	IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };

	char* receiveAddress = "amqps://"  TEST_IOT_HUB_NAME "." TEST_IOT_HUB_SUFFIX "/devices/" TEST_DEVICE_ID "/messages/devicebound";

	mocks.ResetAllCalls();
	setExpectedCallsForTransportCreateUpTo(mocks, &config, STEP_CREATE_TARGET_ADDRESS);
	EXPECTED_CALL(mocks, STRING_c_str(0));
	STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(receiveAddress) + 1));
	STRICT_EXPECTED_CALL(mocks, STRING_construct(receiveAddress)).SetReturn(TEST_NULL_STRING_HANDLE);
	EXPECTED_CALL(mocks, gballoc_free(0));
	setExpectedCleanupCallsForTransportCreateUpTo(mocks, &config, STEP_CREATE_TARGET_ADDRESS);

	// act
	TRANSPORT_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);

	// assert
	ASSERT_IS_NULL(transport);
	mocks.AssertActualAndExpectedCalls();
}

// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_017: [If IoTHubTransportuAMQP_Create fails to initialize handle->sasTokenKeyName with a zero-length STRING the function shall fail and return NULL.] 
TEST_FUNCTION(uAMQP_Create_sasTokenKeyName_allocation_fails)
{
	// arrange
	CIoTHubTransportuAMQPMocks mocks;

	DLIST_ENTRY wts;
	BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
	TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)uAMQP_Protocol();

	IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
		TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME, (IO_TRANSPORT_PROVIDER_CALLBACK)getIOTransport };
	IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };

	mocks.ResetAllCalls();
	setExpectedCallsForTransportCreateUpTo(mocks, &config, STEP_CREATE_RECEIVE_ADDRESS);
	STRICT_EXPECTED_CALL(mocks, STRING_new()).SetReturn(TEST_NULL_STRING_HANDLE);
	setExpectedCleanupCallsForTransportCreateUpTo(mocks, &config, STEP_CREATE_RECEIVE_ADDRESS);

	// act
	TRANSPORT_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);

	// assert
	ASSERT_IS_NULL(transport);
	mocks.AssertActualAndExpectedCalls();
}

// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_135: [If creating the config->deviceKey fails for any reason then IoTHubTransportuAMQP_Create shall fail and return NULL.]
TEST_FUNCTION(uAMQP_Create_deviceKey_allocation_fails)
{
	// arrange
	CIoTHubTransportuAMQPMocks mocks;

	DLIST_ENTRY wts;
	BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
	TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)uAMQP_Protocol();

	IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
		TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME, (IO_TRANSPORT_PROVIDER_CALLBACK)getIOTransport };
	IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };

	mocks.ResetAllCalls();
	setExpectedCallsForTransportCreateUpTo(mocks, &config, STEP_CREATE_SASTOKEN_KEYNAME);
	STRICT_EXPECTED_CALL(mocks, STRING_new()).SetReturn(TEST_NULL_STRING_HANDLE);
	setExpectedCleanupCallsForTransportCreateUpTo(mocks, &config, STEP_CREATE_SASTOKEN_KEYNAME);

	// act
	TRANSPORT_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);

	// assert
	ASSERT_IS_NULL(transport);
	mocks.AssertActualAndExpectedCalls();
}

// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_019: [If IoTHubTransportuAMQP_Create fails to copy config->deviceKey, the function shall fail and return NULL.] 
TEST_FUNCTION(uAMQP_Create_deviceKey_copy_fails)
{
	// arrange
	CIoTHubTransportuAMQPMocks mocks;

	DLIST_ENTRY wts;
	BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
	TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)uAMQP_Protocol();

	IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
		TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME, (IO_TRANSPORT_PROVIDER_CALLBACK)getIOTransport };
	IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };

	mocks.ResetAllCalls();
	setExpectedCallsForTransportCreateUpTo(mocks, &config, STEP_CREATE_SASTOKEN_KEYNAME);
	STRICT_EXPECTED_CALL(mocks, STRING_new());
	STRICT_EXPECTED_CALL(mocks, STRING_copy(0, config.upperConfig->deviceKey)).IgnoreArgument(1).SetReturn(TEST_STRING_COPY_FAILURE_RESULT);
	setExpectedCleanupCallsForTransportCreateUpTo(mocks, &config, STEP_CREATE_DEVICEKEY);

	// act
	TRANSPORT_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);

	// assert
	ASSERT_IS_NULL(transport);
	mocks.AssertActualAndExpectedCalls();
}

// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_132: [If config->upperConfig->io_transport_provider_callback is not NULL, IoTHubTransportuAMQP_Create shall set transport_state->io_transport_provider_callback to config->upperConfig->io_transport_provider_callback]
TEST_FUNCTION(uAMQP_Create_external_io_transport_provider_succeeds)
{
	// arrange
	CIoTHubTransportuAMQPMocks mocks;

	DLIST_ENTRY wts;
	BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
	TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)uAMQP_Protocol();
	IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
		TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME, (IO_TRANSPORT_PROVIDER_CALLBACK)getIOTransport };
	IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };

	test_TLS_io_transport = TEST_TLS_IO_PROVIDER;
	time_t current_time = time(NULL);

	TRANSPORT_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);

	test_latest_cbs_put_token_callback = NULL;

	mocks.ResetAllCalls();
	setExpectedCallsForTransportDoWorkUpTo(mocks, &config, STEP_DOWORK_OPEN_CBS, DOWORK_MESSAGERECEIVER_NONE);
	setExpectedCallsForCbsAuthentication(mocks, &config, current_time);
	setExpectedCallsForCbsAuthTimeoutCheck(mocks, &config, current_time);
	setExpectedCallsForConnectionDoWork(mocks, &config);
	setExpectedCallsForHandleEventSendTimeout(mocks, &config, current_time);

	// act
	transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);

	// assert
	ASSERT_IS_NOT_NULL(test_latest_cbs_put_token_callback);
	ASSERT_IS_NOT_NULL(test_latest_cbs_put_token_context);
	mocks.AssertActualAndExpectedCalls();

	// cleanup
	transport_interface->IoTHubTransport_Destroy(transport);
}

// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_024: [IoTHubTransportuAMQP_Destroy shall destroy the uAMQP message_sender.]
// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_025: [IoTHubTransportuAMQP_Destroy shall destroy the uAMQP message_receiver.]
// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_027: [IoTHubTransportuAMQP_Destroy shall destroy the uAMQP cbs instance]
// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_029: [IoTHubTransportuAMQP_Destroy shall destroy the uAMQP link.]
// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_030: [IoTHubTransportuAMQP_Destroy shall destroy the uAMQP session.]
// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_031: [IoTHubTransportuAMQP_Destroy shall destroy the uAMQP connection.]
// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_032: [IoTHubTransportuAMQP_Destroy shall destroy the uAMQP SASL I / O transport.]
// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_033: [IoTHubTransportuAMQP_Destroy shall destroy the uAMQP SASL mechanism.]
// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_034: [IoTHubTransportuAMQP_Destroy shall destroy the uAMQP TLS I/O transport if created internally.] 
// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_035: [IoTHubTransportuAMQP_Destroy shall delete its internally - set parameters(deviceKey, targetAddress, devicesPath, sasTokenKeyName).]
// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_036: [IoTHubTransportuAMQP_Destroy shall return the remaining items in inProgress to waitingToSend list.] 
TEST_FUNCTION(uAMQP_Destroy_succeeds_no_DoWork)
{
	// arrange
	CIoTHubTransportuAMQPMocks mocks;

	DLIST_ENTRY wts;
	BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
	TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)uAMQP_Protocol();

	IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
		TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME, (IO_TRANSPORT_PROVIDER_CALLBACK)getIOTransport };
	IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };

	TRANSPORT_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);

	ASSERT_IS_NOT_NULL(transport);

	mocks.ResetAllCalls();
	setExpectedCallsForTransportDestroy(mocks, &config, false, false, 0);

	// act
	transport_interface->IoTHubTransport_Destroy(transport);

	// assert
	mocks.AssertActualAndExpectedCalls();
}

// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_051: [IoTHubTransportuAMQP_DoWork shall fail and return immediately if the transport handle parameter is NULL] 
TEST_FUNCTION(uAMQP_DoWork_transport_handle_NULL_fails)
{
	// arrange
	CIoTHubTransportuAMQPMocks mocks;

	DLIST_ENTRY wts;
	BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
	TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)uAMQP_Protocol();

	IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
		TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME, (IO_TRANSPORT_PROVIDER_CALLBACK)getIOTransport };
	IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };

	mocks.ResetAllCalls();

	// act
	transport_interface->IoTHubTransport_DoWork(NULL, TEST_IOTHUB_CLIENT_LL_HANDLE);

	// assert
	mocks.AssertActualAndExpectedCalls(); // Nothing is expected.
}

// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_052: [IoTHubTransportuAMQP_DoWork shall fail and return immediately if the client handle parameter is NULL] 
TEST_FUNCTION(uAMQP_DoWork_client_handle_NULL_fails)
{
	// arrange
	CIoTHubTransportuAMQPMocks mocks;

	DLIST_ENTRY wts;
	BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
	TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)uAMQP_Protocol();

	IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
		TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME, (IO_TRANSPORT_PROVIDER_CALLBACK)getIOTransport };
	IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };

	TRANSPORT_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);

	mocks.ResetAllCalls();

	// act
	transport_interface->IoTHubTransport_DoWork(transport, NULL);

	// assert
	mocks.AssertActualAndExpectedCalls(); // Nothing is expected.

	// cleanup
	transport_interface->IoTHubTransport_Destroy(transport);
}

// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_110: [IoTHubTransportuAMQP_DoWork shall create the TLS IO using transport_state->io_transport_provider callback function] 
// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_136: [If transport_state->io_transport_provider_callback fails, IoTHubTransportuAMQP_DoWork shall fail and return immediately]
TEST_FUNCTION(uAMQP_DoWork_TLS_IO_provider_fails)
{
	// arrange
	CIoTHubTransportuAMQPMocks mocks;

	DLIST_ENTRY wts;
	BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
	TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)uAMQP_Protocol();

	IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
		TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME, (IO_TRANSPORT_PROVIDER_CALLBACK)getIOTransport };
	IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };

	TRANSPORT_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);

	mocks.ResetAllCalls();
	EXPECTED_CALL(mocks, STRING_c_str(NULL));
	setExpectedCallsForRollEventsBackToWaitList(mocks, &config);
	setExpectedCallsForHandleEventSendTimeout(mocks, &config, test_current_time);
	test_TLS_io_transport = (XIO_HANDLE)NULL;

	// act
	transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);

	// assert
	ASSERT_IS_TRUE(strcmp(test_latest_getIOTransport_fqdn, BASEIMPLEMENTATION::STRING_c_str(test_iotHubHostFqdn)) == 0);
	ASSERT_ARE_EQUAL(int, test_latest_getIOTransport_port, TEST_IOT_HUB_PORT);
	mocks.AssertActualAndExpectedCalls();

	// cleanup
	transport_interface->IoTHubTransport_Destroy(transport);
}

// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_056: [IoTHubTransportuAMQP_DoWork shall create the SASL mechanism using uAMQP’s saslmechanism_create() API] 
// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_057: [If saslmechanism_create() fails, IoTHubTransportuAMQP_DoWork shall fail and return immediately]
TEST_FUNCTION(uAMQP_DoWork_saslmechanism_create_fails)
{
	// arrange
	CIoTHubTransportuAMQPMocks mocks;

	DLIST_ENTRY wts;
	BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
	TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)uAMQP_Protocol();

	IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
		TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME, (IO_TRANSPORT_PROVIDER_CALLBACK)getIOTransport };
	IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };

	TRANSPORT_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);

	test_TLS_io_transport = TEST_TLS_IO_PROVIDER;

	mocks.ResetAllCalls();
	setExpectedCallsForTransportDoWorkUpTo(mocks, &config, STEP_DOWORK_GET_TLS_IO, DOWORK_MESSAGERECEIVER_NONE);
	STRICT_EXPECTED_CALL(mocks, saslmssbcbs_get_interface());
	EXPECTED_CALL(mocks, saslmechanism_create(NULL, NULL)).SetReturn((SASL_MECHANISM_HANDLE)NULL);
	setExpectedCallsForRollEventsBackToWaitList(mocks, &config);
	setExpectedCallsForHandleEventSendTimeout(mocks, &config, test_current_time);

	// act
	transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);

	// assert
	mocks.AssertActualAndExpectedCalls();

	// cleanup
	transport_interface->IoTHubTransport_Destroy(transport);
}

// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_060: [IoTHubTransportuAMQP_DoWork shall create the SASL I/O layer using the xio_create() C Shared Utility API] 
// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_061: [If xio_create() fails creating the SASL I/O layer, IoTHubTransportuAMQP_DoWork shall fail and return immediately]
TEST_FUNCTION(uAMQP_DoWork_saslio_create_fails)
{
	// arrange
	CIoTHubTransportuAMQPMocks mocks;

	DLIST_ENTRY wts;
	BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
	TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)uAMQP_Protocol();

	IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
		TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME, (IO_TRANSPORT_PROVIDER_CALLBACK)getIOTransport };
	IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };

	TRANSPORT_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);

	test_TLS_io_transport = TEST_TLS_IO_PROVIDER;

	mocks.ResetAllCalls();
	setExpectedCallsForTransportDoWorkUpTo(mocks, &config, STEP_DOWORK_SASLIO_GET_INTERFACE, DOWORK_MESSAGERECEIVER_NONE);
	EXPECTED_CALL(mocks, xio_create(NULL, NULL, NULL)).SetReturn((XIO_HANDLE)NULL);
	setExpectedCallsForConnectionDestroyUpTo(mocks, &config, STEP_DOWORK_CREATE_SASLMECHANISM);
	setExpectedCallsForRollEventsBackToWaitList(mocks, &config);
	setExpectedCallsForHandleEventSendTimeout(mocks, &config, test_current_time);

	// act
	transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);

	// assert
	mocks.AssertActualAndExpectedCalls();

	// cleanup
	transport_interface->IoTHubTransport_Destroy(transport);
}

// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_062: [IoTHubTransportuAMQP_DoWork shall create the connection with the IoT service using connection_create() uAMQP API, passing the SASL I/O layer, IoT Hub FQDN and container ID as parameters (pass NULL for callbacks)]
// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_063: [If connection_create() fails, IoTHubTransportuAMQP_DoWork shall fail and return immediately]
TEST_FUNCTION(uAMQP_DoWork_connection_create_fails)
{
	// arrange
	CIoTHubTransportuAMQPMocks mocks;

	DLIST_ENTRY wts;
	BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
	TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)uAMQP_Protocol();

	IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
		TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME, (IO_TRANSPORT_PROVIDER_CALLBACK)getIOTransport };
	IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };

	TRANSPORT_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);

	test_TLS_io_transport = TEST_TLS_IO_PROVIDER;

	mocks.ResetAllCalls();
	setExpectedCallsForTransportDoWorkUpTo(mocks, &config, STEP_DOWORK_CREATE_SASLIO, DOWORK_MESSAGERECEIVER_NONE);
	EXPECTED_CALL(mocks, STRING_c_str(NULL));
	EXPECTED_CALL(mocks, connection_create(NULL, NULL, NULL, NULL, NULL)).SetReturn((CONNECTION_HANDLE)NULL);
	setExpectedCallsForConnectionDestroyUpTo(mocks, &config, STEP_DOWORK_CREATE_SASLIO);
	setExpectedCallsForRollEventsBackToWaitList(mocks, &config);
	setExpectedCallsForHandleEventSendTimeout(mocks, &config, test_current_time);

	// act
	transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);

	// assert
	mocks.AssertActualAndExpectedCalls();

	// cleanup
	transport_interface->IoTHubTransport_Destroy(transport);
}

// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_137: [IoTHubTransportuAMQP_DoWork shall create the AMQP session session_create() uAMQP API, passing the connection instance as parameter]
// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_138: [If session_create() fails, IoTHubTransportuAMQP_DoWork shall fail and return immediately]
TEST_FUNCTION(uAMQP_DoWork_session_create_fails)
{
	// arrange
	CIoTHubTransportuAMQPMocks mocks;

	DLIST_ENTRY wts;
	BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
	TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)uAMQP_Protocol();

	IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
		TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME, (IO_TRANSPORT_PROVIDER_CALLBACK)getIOTransport };
	IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };

	TRANSPORT_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);

	test_TLS_io_transport = TEST_TLS_IO_PROVIDER;

	mocks.ResetAllCalls();
	setExpectedCallsForTransportDoWorkUpTo(mocks, &config, STEP_DOWORK_CREATE_CONNECTION, DOWORK_MESSAGERECEIVER_NONE);
	EXPECTED_CALL(mocks, session_create(NULL, NULL, NULL)).SetReturn((SESSION_HANDLE)NULL);
	setExpectedCallsForConnectionDestroyUpTo(mocks, &config, STEP_DOWORK_CREATE_CONNECTION);

	setExpectedCallsForRollEventsBackToWaitList(mocks, &config);
	setExpectedCallsForHandleEventSendTimeout(mocks, &config, test_current_time);

	// act
	transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);

	// assert
	mocks.AssertActualAndExpectedCalls();

	// cleanup
	transport_interface->IoTHubTransport_Destroy(transport);
}

// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_065: [IoTHubTransportuAMQP_DoWork shall apply a default value of UINT_MAX for the parameter ‘AMQP incoming window’] 
// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_115: [IoTHubTransportuAMQP_DoWork shall apply a default value of 100 for the parameter ‘AMQP outgoing window’] 
// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_066: [IoTHubTransportuAMQP_DoWork shall establish the CBS connection using the cbs_create() uAMQP API] 
// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_067: [If cbs_create() fails, IoTHubTransportuAMQP_DoWork shall fail and return immediately]
TEST_FUNCTION(uAMQP_DoWork_cbs_create_fails)
{
	// arrange
	CIoTHubTransportuAMQPMocks mocks;

	DLIST_ENTRY wts;
	BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
	TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)uAMQP_Protocol();

	IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
		TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME, (IO_TRANSPORT_PROVIDER_CALLBACK)getIOTransport };
	IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };

	TRANSPORT_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);

	test_TLS_io_transport = TEST_TLS_IO_PROVIDER;

	mocks.ResetAllCalls();
	setExpectedCallsForTransportDoWorkUpTo(mocks, &config, STEP_DOWORK_OUTGOING_WINDOW, DOWORK_MESSAGERECEIVER_NONE);
	EXPECTED_CALL(mocks, cbs_create(NULL, NULL, NULL)).SetReturn((CBS_HANDLE)NULL);
	setExpectedCallsForConnectionDestroyUpTo(mocks, &config, STEP_DOWORK_CREATE_SESSION);

	setExpectedCallsForRollEventsBackToWaitList(mocks, &config);
	setExpectedCallsForHandleEventSendTimeout(mocks, &config, test_current_time);

	// act
	transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);

	// assert
	mocks.AssertActualAndExpectedCalls();

	// cleanup
	transport_interface->IoTHubTransport_Destroy(transport);
}

// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_139: [IoTHubTransportuAMQP_DoWork shall open the CBS connection using the cbs_open() uAMQP API]
// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_140: [If cbs_open() fails, IoTHubTransportuAMQP_DoWork shall fail and return immediately]
TEST_FUNCTION(uAMQP_DoWork_cbs_open_fails)
{
	// arrange
	CIoTHubTransportuAMQPMocks mocks;

	DLIST_ENTRY wts;
	BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
	TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)uAMQP_Protocol();

	IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
		TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME, (IO_TRANSPORT_PROVIDER_CALLBACK)getIOTransport };
	IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };

	TRANSPORT_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);

	test_TLS_io_transport = TEST_TLS_IO_PROVIDER;

	mocks.ResetAllCalls();
	setExpectedCallsForTransportDoWorkUpTo(mocks, &config, STEP_DOWORK_CREATE_CBS, DOWORK_MESSAGERECEIVER_NONE);
	EXPECTED_CALL(mocks, cbs_open(NULL)).SetReturn(1);
	setExpectedCallsForConnectionDestroyUpTo(mocks, &config, STEP_DOWORK_CREATE_CBS);
	setExpectedCallsForRollEventsBackToWaitList(mocks, &config);
	setExpectedCallsForHandleEventSendTimeout(mocks, &config, test_current_time);

	// act
	transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);

	// assert
	mocks.AssertActualAndExpectedCalls();

	// cleanup
	transport_interface->IoTHubTransport_Destroy(transport);
}

// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_020: [IoTHubTransportuAMQP_Create shall set parameter transport_state->sas_token_lifetime with the default value of 3600000 (milliseconds).] 
// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_081: [IoTHubTransportuAMQP_DoWork shall put a new SAS token if the one has not been out already, or if the previous one failed to be put due to timeout of cbs_put_token().] 
// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_083: [Each new SAS token created by the transport shall be valid for up to ‘sas_token_lifetime’ milliseconds from the time of creation] 
TEST_FUNCTION(uAMQP_DoWork_SASToken_create_fails)
{
	// arrange
	CIoTHubTransportuAMQPMocks mocks;

	DLIST_ENTRY wts;
	BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
	TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)uAMQP_Protocol();
	IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
		TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME, (IO_TRANSPORT_PROVIDER_CALLBACK)getIOTransport };
	IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };
	TRANSPORT_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);

	test_TLS_io_transport = TEST_TLS_IO_PROVIDER;
	time_t current_time = time(NULL);
	size_t expected_expiry_time = (size_t)(difftime(current_time, 0) + TEST_SAS_TOKEN_LIFETIME_MS / 1000);

	mocks.ResetAllCalls();
	setExpectedCallsForTransportDoWorkUpTo(mocks, &config, STEP_DOWORK_OPEN_CBS, DOWORK_MESSAGERECEIVER_NONE);
	STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(current_time);
	EXPECTED_CALL(mocks, SASToken_Create(NULL, NULL, NULL, 0)).SetReturn((STRING_HANDLE)NULL);
	setExpectedCallsForConnectionDestroyUpTo(mocks, &config, STEP_DOWORK_CREATE_CBS);
	setExpectedCallsForRollEventsBackToWaitList(mocks, &config);
	setExpectedCallsForHandleEventSendTimeout(mocks, &config, current_time);

	// act
	transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);

	// assert
	mocks.AssertActualAndExpectedCalls();
	ASSERT_ARE_EQUAL(int, expected_expiry_time, test_latest_SASToken_expiry_time);

	// cleanup
	transport_interface->IoTHubTransport_Destroy(transport);
}

// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_146: [If the SAS token fails to be sent to CBS (cbs_put_token), IoTHubTransportuAMQP_DoWork shall fail and exit immediately]
// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_145: [Each new SAS token created shall be deleted from memory immediately after sending it to CBS]
TEST_FUNCTION(uAMQP_DoWork_cbs_put_token_fails)
{
	// arrange
	CIoTHubTransportuAMQPMocks mocks;

	DLIST_ENTRY wts;
	BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
	TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)uAMQP_Protocol();
	IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
		TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME, (IO_TRANSPORT_PROVIDER_CALLBACK)getIOTransport };
	IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };
	TRANSPORT_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);

	test_TLS_io_transport = TEST_TLS_IO_PROVIDER;
	time_t current_time = time(NULL);

	mocks.ResetAllCalls();
	setExpectedCallsForTransportDoWorkUpTo(mocks, &config, STEP_DOWORK_OPEN_CBS, DOWORK_MESSAGERECEIVER_NONE);
	STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(current_time);
	EXPECTED_CALL(mocks, SASToken_Create(NULL, NULL, NULL, 0));
	EXPECTED_CALL(mocks, STRING_c_str(NULL));
	EXPECTED_CALL(mocks, STRING_c_str(NULL));
	EXPECTED_CALL(mocks, cbs_put_token(NULL, NULL, NULL, NULL, NULL, NULL)).SetReturn(1);
	EXPECTED_CALL(mocks, STRING_delete(NULL));

	setExpectedCallsForConnectionDestroyUpTo(mocks, &config, STEP_DOWORK_CREATE_CBS);
	setExpectedCallsForRollEventsBackToWaitList(mocks, &config);
	setExpectedCallsForHandleEventSendTimeout(mocks, &config, current_time);

	// act
	transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);

	// assert
	mocks.AssertActualAndExpectedCalls();

	// cleanup
	transport_interface->IoTHubTransport_Destroy(transport);
}

// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_084: [IoTHubTransportuAMQP_DoWork shall wait for ‘cbs_request_timeout’ milliseconds for the cbs_put_token() to complete before failing due to timeout] 
TEST_FUNCTION(uAMQP_DoWork_CBS_auth_timeout_fails)
{
	// arrange
	CIoTHubTransportuAMQPMocks mocks;

	DLIST_ENTRY wts;
	BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
	TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)uAMQP_Protocol();
	IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
		TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME, (IO_TRANSPORT_PROVIDER_CALLBACK)getIOTransport };
	IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };
	TRANSPORT_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);

	test_TLS_io_transport = TEST_TLS_IO_PROVIDER;
	time_t current_time = time(NULL);
	time_t expiration_time = addSecondsToTime(current_time, TEST_CBS_REQUEST_TIMEOUT_MS + 1);

	mocks.ResetAllCalls();
	setExpectedCallsForTransportDoWorkUpTo(mocks, &config, STEP_DOWORK_OPEN_CBS, DOWORK_MESSAGERECEIVER_NONE);
	setExpectedCallsForCbsAuthentication(mocks, &config, current_time);
	EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(expiration_time);
	setExpectedCallsForConnectionDestroyUpTo(mocks, &config, STEP_DOWORK_CREATE_CBS);
	setExpectedCallsForRollEventsBackToWaitList(mocks, &config);
	setExpectedCallsForHandleEventSendTimeout(mocks, &config, current_time);

	// act
	transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);

	// assert
	mocks.AssertActualAndExpectedCalls();

	// cleanup
	transport_interface->IoTHubTransport_Destroy(transport);
}

// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_068: [IoTHubTransportuAMQP_DoWork shall create the AMQP link for sending messages using ‘source’ as “ingress”, target as the IoT hub FQDN, link name as “sender-link” and role as ‘role_sender’] 
// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_069: [If IoTHubTransportuAMQP_DoWork fails to create the AMQP link for sending messages, the function shall fail and return immediately, flagging the connection to be re-stablished] 
TEST_FUNCTION(uAMQP_DoWork_messagesender_create_source_fails)
{
	// arrange
	CIoTHubTransportuAMQPMocks mocks;

	DLIST_ENTRY wts;
	BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
	TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)uAMQP_Protocol();
	IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
		TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME, (IO_TRANSPORT_PROVIDER_CALLBACK)getIOTransport };
	IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };
	TRANSPORT_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);

	test_TLS_io_transport = TEST_TLS_IO_PROVIDER;
	time_t current_time = time(NULL);
	time_t expiration_time = addSecondsToTime(current_time, TEST_CBS_REQUEST_TIMEOUT_MS + 1);

	mocks.ResetAllCalls();
	setExpectedCallsForTransportDoWorkUpTo(mocks, &config, STEP_DOWORK_OPEN_CBS, DOWORK_MESSAGERECEIVER_NONE);
	setExpectedCallsForCbsAuthentication(mocks, &config, current_time);
	setExpectedCallsForCbsAuthTimeoutCheck(mocks, &config, current_time);
	setExpectedCallsForConnectionDoWork(mocks, &config);
	setExpectedCallsForHandleEventSendTimeout(mocks, &config, current_time);
	setExpectedCallsForSASTokenExpiryCheck(mocks, &config, current_time);
	EXPECTED_CALL(mocks, messaging_create_source(NULL)).SetReturn((AMQP_VALUE)NULL);
	setExpectedCallsForConnectionDestroyUpTo(mocks, &config, STEP_DOWORK_CREATE_CBS);
	setExpectedCallsForRollEventsBackToWaitList(mocks, &config);
	setExpectedCallsForHandleEventSendTimeout(mocks, &config, current_time);

	// act
	transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);
	test_latest_cbs_put_token_callback(test_latest_cbs_put_token_context, CBS_OPERATION_RESULT_OK, 0, NULL);
	transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);

	// assert
	mocks.AssertActualAndExpectedCalls();

	// cleanup
	transport_interface->IoTHubTransport_Destroy(transport);
}

// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_068: [IoTHubTransportuAMQP_DoWork shall create the AMQP link for sending messages using ‘source’ as “ingress”, target as the IoT hub FQDN, link name as “sender-link” and role as ‘role_sender’] 
// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_069: [If IoTHubTransportuAMQP_DoWork fails to create the AMQP link for sending messages, the function shall fail and return immediately, flagging the connection to be re-stablished] 
TEST_FUNCTION(uAMQP_DoWork_messagesender_create_target_fails)
{
	// arrange
	CIoTHubTransportuAMQPMocks mocks;

	DLIST_ENTRY wts;
	BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
	TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)uAMQP_Protocol();
	IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
		TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME, (IO_TRANSPORT_PROVIDER_CALLBACK)getIOTransport };
	IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };
	TRANSPORT_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);

	test_TLS_io_transport = TEST_TLS_IO_PROVIDER;
	time_t current_time = time(NULL);
	time_t expiration_time = addSecondsToTime(current_time, TEST_CBS_REQUEST_TIMEOUT_MS + 1);

	mocks.ResetAllCalls();
	setExpectedCallsForTransportDoWorkUpTo(mocks, &config, STEP_DOWORK_OPEN_CBS, DOWORK_MESSAGERECEIVER_NONE);
	setExpectedCallsForCbsAuthentication(mocks, &config, current_time);
	setExpectedCallsForCbsAuthTimeoutCheck(mocks, &config, current_time);
	setExpectedCallsForConnectionDoWork(mocks, &config);
	setExpectedCallsForHandleEventSendTimeout(mocks, &config, current_time);
	setExpectedCallsForSASTokenExpiryCheck(mocks, &config, current_time);
	EXPECTED_CALL(mocks, messaging_create_source(NULL)).SetReturn(TEST_MESSAGESENDER_SOURCE);
	EXPECTED_CALL(mocks, STRING_c_str(NULL));
	EXPECTED_CALL(mocks, messaging_create_target(NULL)).SetReturn((AMQP_VALUE)NULL);
	STRICT_EXPECTED_CALL(mocks, amqpvalue_destroy(TEST_MESSAGESENDER_SOURCE));
	setExpectedCallsForConnectionDestroyUpTo(mocks, &config, STEP_DOWORK_CREATE_CBS);
	setExpectedCallsForRollEventsBackToWaitList(mocks, &config);
	setExpectedCallsForHandleEventSendTimeout(mocks, &config, current_time);

	// act
	transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);
	test_latest_cbs_put_token_callback(test_latest_cbs_put_token_context, CBS_OPERATION_RESULT_OK, 0, NULL);
	transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);

	// assert
	mocks.AssertActualAndExpectedCalls();

	// cleanup
	transport_interface->IoTHubTransport_Destroy(transport);
}

// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_068: [IoTHubTransportuAMQP_DoWork shall create the AMQP link for sending messages using ‘source’ as “ingress”, target as the IoT hub FQDN, link name as “sender-link” and role as ‘role_sender’] 
// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_069: [If IoTHubTransportuAMQP_DoWork fails to create the AMQP link for sending messages, the function shall fail and return immediately, flagging the connection to be re-stablished] 
TEST_FUNCTION(uAMQP_DoWork_messagesender_create_link_fails)
{
	// arrange
	CIoTHubTransportuAMQPMocks mocks;

	DLIST_ENTRY wts;
	BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
	TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)uAMQP_Protocol();
	IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
		TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME, (IO_TRANSPORT_PROVIDER_CALLBACK)getIOTransport };
	IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };
	TRANSPORT_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);

	test_TLS_io_transport = TEST_TLS_IO_PROVIDER;
	time_t current_time = time(NULL);
	time_t expiration_time = addSecondsToTime(current_time, TEST_CBS_REQUEST_TIMEOUT_MS + 1);

	mocks.ResetAllCalls();
	setExpectedCallsForTransportDoWorkUpTo(mocks, &config, STEP_DOWORK_OPEN_CBS, DOWORK_MESSAGERECEIVER_NONE);
	setExpectedCallsForCbsAuthentication(mocks, &config, current_time);
	setExpectedCallsForCbsAuthTimeoutCheck(mocks, &config, current_time);
	setExpectedCallsForConnectionDoWork(mocks, &config);
	setExpectedCallsForHandleEventSendTimeout(mocks, &config, current_time);
	setExpectedCallsForSASTokenExpiryCheck(mocks, &config, current_time);
	EXPECTED_CALL(mocks, messaging_create_source(NULL)).SetReturn(TEST_MESSAGESENDER_SOURCE);
	EXPECTED_CALL(mocks, STRING_c_str(NULL));
	EXPECTED_CALL(mocks, messaging_create_target(NULL)).SetReturn(TEST_MESSAGESENDER_TARGET);
	EXPECTED_CALL(mocks, link_create(NULL, NULL, NULL, NULL, NULL)).SetReturn((LINK_HANDLE)NULL);
	STRICT_EXPECTED_CALL(mocks, amqpvalue_destroy(TEST_MESSAGESENDER_SOURCE));
	STRICT_EXPECTED_CALL(mocks, amqpvalue_destroy(TEST_MESSAGESENDER_TARGET));
	setExpectedCallsForConnectionDestroyUpTo(mocks, &config, STEP_DOWORK_CREATE_CBS);
	setExpectedCallsForRollEventsBackToWaitList(mocks, &config);
	setExpectedCallsForHandleEventSendTimeout(mocks, &config, current_time);

	// act
	transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);
	test_latest_cbs_put_token_callback(test_latest_cbs_put_token_context, CBS_OPERATION_RESULT_OK, 0, NULL);
	transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);

	// assert
	mocks.AssertActualAndExpectedCalls();

	// cleanup
	transport_interface->IoTHubTransport_Destroy(transport);
}

// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_070: [IoTHubTransportuAMQP_DoWork shall create the AMQP message sender using messagesender_create() uAMQP API] 
// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_071: [IoTHubTransportuAMQP_DoWork shall fail and return immediately if the uAMQP message sender instance fails to be created, flagging the connection to be re-established] 
// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_119: [IoTHubTransportuAMQP_DoWork shall apply a default value of 65536 for the parameter ‘Link MAX message size’] 
TEST_FUNCTION(uAMQP_DoWork_messagesender_create_fails)
{
	// arrange
	CIoTHubTransportuAMQPMocks mocks;

	DLIST_ENTRY wts;
	BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
	TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)uAMQP_Protocol();
	IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
		TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME, (IO_TRANSPORT_PROVIDER_CALLBACK)getIOTransport };
	IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };
	TRANSPORT_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);

	test_TLS_io_transport = TEST_TLS_IO_PROVIDER;
	time_t current_time = time(NULL);
	time_t expiration_time = addSecondsToTime(current_time, TEST_CBS_REQUEST_TIMEOUT_MS + 1);

	mocks.ResetAllCalls();
	setExpectedCallsForTransportDoWorkUpTo(mocks, &config, STEP_DOWORK_OPEN_CBS, DOWORK_MESSAGERECEIVER_NONE);
	setExpectedCallsForCbsAuthentication(mocks, &config, current_time);
	setExpectedCallsForCbsAuthTimeoutCheck(mocks, &config, current_time);
	setExpectedCallsForConnectionDoWork(mocks, &config);
	setExpectedCallsForHandleEventSendTimeout(mocks, &config, current_time);
	setExpectedCallsForSASTokenExpiryCheck(mocks, &config, current_time);
	EXPECTED_CALL(mocks, messaging_create_source(NULL)).SetReturn(TEST_MESSAGESENDER_SOURCE);
	EXPECTED_CALL(mocks, STRING_c_str(NULL));
	EXPECTED_CALL(mocks, messaging_create_target(NULL)).SetReturn(TEST_MESSAGESENDER_TARGET);
	EXPECTED_CALL(mocks, link_create(NULL, NULL, NULL, NULL, NULL)).SetReturn(TEST_MESSAGESENDER_LINK);
	EXPECTED_CALL(mocks, link_set_max_message_size(NULL, NULL));
	EXPECTED_CALL(mocks, messagesender_create(NULL, NULL, NULL, NULL)).SetReturn((MESSAGE_SENDER_HANDLE)NULL);
	STRICT_EXPECTED_CALL(mocks, amqpvalue_destroy(TEST_MESSAGESENDER_SOURCE));
	STRICT_EXPECTED_CALL(mocks, amqpvalue_destroy(TEST_MESSAGESENDER_TARGET));
	setExpectedCallsForConnectionDestroyUpTo(mocks, &config, STEP_DOWORK_CREATE_CBS);
	setExpectedCallsForRollEventsBackToWaitList(mocks, &config);
	setExpectedCallsForHandleEventSendTimeout(mocks, &config, current_time);

	// act
	transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);
	test_latest_cbs_put_token_callback(test_latest_cbs_put_token_context, CBS_OPERATION_RESULT_OK, 0, NULL);
	transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);

	// assert
	mocks.AssertActualAndExpectedCalls();

	// cleanup
	transport_interface->IoTHubTransport_Destroy(transport);
}

// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_072: [IoTHubTransportuAMQP_DoWork shall open the AMQP message sender using messagesender_open() uAMQP API] 
// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_073: [IoTHubTransportuAMQP_DoWork shall fail and return immediately if the uAMQP message sender instance fails to be opened, flagging the connection to be re-established] 
TEST_FUNCTION(uAMQP_DoWork_messagesender_open_fails)
{
	// arrange
	CIoTHubTransportuAMQPMocks mocks;

	DLIST_ENTRY wts;
	BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
	TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)uAMQP_Protocol();
	IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
		TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME, (IO_TRANSPORT_PROVIDER_CALLBACK)getIOTransport };
	IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };
	TRANSPORT_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);

	test_TLS_io_transport = TEST_TLS_IO_PROVIDER;
	time_t current_time = time(NULL);
	time_t expiration_time = addSecondsToTime(current_time, TEST_CBS_REQUEST_TIMEOUT_MS + 1);

	mocks.ResetAllCalls();
	setExpectedCallsForTransportDoWorkUpTo(mocks, &config, STEP_DOWORK_OPEN_CBS, DOWORK_MESSAGERECEIVER_NONE);
	setExpectedCallsForCbsAuthentication(mocks, &config, current_time);
	setExpectedCallsForCbsAuthTimeoutCheck(mocks, &config, current_time);
	setExpectedCallsForConnectionDoWork(mocks, &config);
	setExpectedCallsForHandleEventSendTimeout(mocks, &config, current_time);
	setExpectedCallsForSASTokenExpiryCheck(mocks, &config, current_time);
	EXPECTED_CALL(mocks, messaging_create_source(NULL)).SetReturn(TEST_MESSAGESENDER_SOURCE);
	EXPECTED_CALL(mocks, STRING_c_str(NULL));
	EXPECTED_CALL(mocks, messaging_create_target(NULL)).SetReturn(TEST_MESSAGESENDER_TARGET);
	EXPECTED_CALL(mocks, link_create(NULL, NULL, NULL, NULL, NULL)).SetReturn(TEST_MESSAGESENDER_LINK);
	EXPECTED_CALL(mocks, link_set_max_message_size(NULL, NULL));
	EXPECTED_CALL(mocks, messagesender_create(NULL, NULL, NULL, NULL)).SetReturn(TEST_MESSAGE_SENDER);
	EXPECTED_CALL(mocks, messagesender_open(TEST_MESSAGE_SENDER)).SetReturn(1);
	STRICT_EXPECTED_CALL(mocks, amqpvalue_destroy(TEST_MESSAGESENDER_SOURCE));
	STRICT_EXPECTED_CALL(mocks, amqpvalue_destroy(TEST_MESSAGESENDER_TARGET));
	setExpectedCallsForConnectionDestroyUpTo(mocks, &config, STEP_DOWORK_CREATE_CBS);
	setExpectedCallsForRollEventsBackToWaitList(mocks, &config);
	setExpectedCallsForHandleEventSendTimeout(mocks, &config, current_time);

	// act
	transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);
	test_latest_cbs_put_token_callback(test_latest_cbs_put_token_context, CBS_OPERATION_RESULT_OK, 0, NULL);
	transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);

	// assert
	mocks.AssertActualAndExpectedCalls();

	// cleanup
	transport_interface->IoTHubTransport_Destroy(transport);
}

// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_074: [IoTHubTransportuAMQP_DoWork shall create the AMQP link for receiving messages using ‘source’ as messageReceiveAddress, target as the “ingress-rx”, link name as “receiver-link” and role as ‘role_receiver’] 
// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_075: [If IoTHubTransportuAMQP_DoWork fails to create the AMQP link for receiving messages, the function shall fail and return immediately, flagging the connection to be re-stablished] 
// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_038: [IoTHubTransportuAMQP_Subscribe shall set transport_handle->receive_messages to true and return success code.]
TEST_FUNCTION(uAMQP_DoWork_messagereceiver_source_create_fails)
{
	// arrange
	CIoTHubTransportuAMQPMocks mocks;

	DLIST_ENTRY wts;
	BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
	TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)uAMQP_Protocol();
	IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
		TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME, (IO_TRANSPORT_PROVIDER_CALLBACK)getIOTransport };
	IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };

	test_TLS_io_transport = TEST_TLS_IO_PROVIDER;
	time_t current_time = time(NULL);

	TRANSPORT_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);
	int subscribe_result = transport_interface->IoTHubTransport_Subscribe(transport);
	ASSERT_ARE_EQUAL(int, subscribe_result, 0);

	mocks.ResetAllCalls();
	setExpectedCallsForTransportDoWorkUpTo(mocks, &config, STEP_DOWORK_OPEN_CBS, DOWORK_MESSAGERECEIVER_NONE);
	setExpectedCallsForCbsAuthentication(mocks, &config, current_time);
	setExpectedCallsForCbsAuthTimeoutCheck(mocks, &config, current_time);
	setExpectedCallsForConnectionDoWork(mocks, &config);
	setExpectedCallsForHandleEventSendTimeout(mocks, &config, current_time);
	setExpectedCallsForSASTokenExpiryCheck(mocks, &config, current_time);
	EXPECTED_CALL(mocks, STRING_c_str(NULL));
	EXPECTED_CALL(mocks, messaging_create_source(NULL)).SetReturn((AMQP_VALUE)NULL);
	EXPECTED_CALL(mocks, messaging_create_source(NULL)).SetReturn((AMQP_VALUE)NULL);
	setExpectedCallsForConnectionDestroyUpTo(mocks, &config, STEP_DOWORK_CREATE_CBS);
	setExpectedCallsForRollEventsBackToWaitList(mocks, &config);
	setExpectedCallsForHandleEventSendTimeout(mocks, &config, current_time);

	// act
	transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);
	test_latest_cbs_put_token_callback(test_latest_cbs_put_token_context, CBS_OPERATION_RESULT_OK, 0, NULL);
	transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);

	// assert
	mocks.AssertActualAndExpectedCalls();

	// cleanup
	transport_interface->IoTHubTransport_Destroy(transport);
}

// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_074: [IoTHubTransportuAMQP_DoWork shall create the AMQP link for receiving messages using ‘source’ as messageReceiveAddress, target as the “ingress-rx”, link name as “receiver-link” and role as ‘role_receiver’] 
// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_075: [If IoTHubTransportuAMQP_DoWork fails to create the AMQP link for receiving messages, the function shall fail and return immediately, flagging the connection to be re-stablished] 
TEST_FUNCTION(uAMQP_DoWork_messagereceiver_target_create_fails)
{
	// arrange
	CIoTHubTransportuAMQPMocks mocks;

	DLIST_ENTRY wts;
	BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
	TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)uAMQP_Protocol();
	IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
		TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME, (IO_TRANSPORT_PROVIDER_CALLBACK)getIOTransport };
	IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };

	test_TLS_io_transport = TEST_TLS_IO_PROVIDER;
	time_t current_time = time(NULL);

	TRANSPORT_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);
	int subscribe_result = transport_interface->IoTHubTransport_Subscribe(transport);
	ASSERT_ARE_EQUAL(int, subscribe_result, 0);

	mocks.ResetAllCalls();
	setExpectedCallsForTransportDoWorkUpTo(mocks, &config, STEP_DOWORK_OPEN_CBS, DOWORK_MESSAGERECEIVER_NONE);
	setExpectedCallsForCbsAuthentication(mocks, &config, current_time);
	setExpectedCallsForCbsAuthTimeoutCheck(mocks, &config, current_time);
	setExpectedCallsForConnectionDoWork(mocks, &config);
	setExpectedCallsForHandleEventSendTimeout(mocks, &config, current_time);
	setExpectedCallsForSASTokenExpiryCheck(mocks, &config, current_time);
	EXPECTED_CALL(mocks, STRING_c_str(NULL));
	EXPECTED_CALL(mocks, messaging_create_source(NULL)).SetReturn(TEST_MESSAGERECEIVER_SOURCE);
	EXPECTED_CALL(mocks, messaging_create_target(NULL)).SetReturn((AMQP_VALUE)NULL);
	STRICT_EXPECTED_CALL(mocks, amqpvalue_destroy(TEST_MESSAGERECEIVER_SOURCE));
	EXPECTED_CALL(mocks, messaging_create_source(NULL)).SetReturn((AMQP_VALUE)NULL);
	setExpectedCallsForConnectionDestroyUpTo(mocks, &config, STEP_DOWORK_CREATE_CBS);
	setExpectedCallsForRollEventsBackToWaitList(mocks, &config);
	setExpectedCallsForHandleEventSendTimeout(mocks, &config, current_time);

	// act
	transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);
	test_latest_cbs_put_token_callback(test_latest_cbs_put_token_context, CBS_OPERATION_RESULT_OK, 0, NULL);
	transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);

	// assert
	mocks.AssertActualAndExpectedCalls();

	// cleanup
	transport_interface->IoTHubTransport_Destroy(transport);
}

// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_074: [IoTHubTransportuAMQP_DoWork shall create the AMQP link for receiving messages using ‘source’ as messageReceiveAddress, target as the “ingress-rx”, link name as “receiver-link” and role as ‘role_receiver’] 
// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_075: [If IoTHubTransportuAMQP_DoWork fails to create the AMQP link for receiving messages, the function shall fail and return immediately, flagging the connection to be re-stablished] 
TEST_FUNCTION(uAMQP_DoWork_messagereceiver_link_create_fails)
{
	// arrange
	CIoTHubTransportuAMQPMocks mocks;

	DLIST_ENTRY wts;
	BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
	TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)uAMQP_Protocol();
	IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
		TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME, (IO_TRANSPORT_PROVIDER_CALLBACK)getIOTransport };
	IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };

	test_TLS_io_transport = TEST_TLS_IO_PROVIDER;
	time_t current_time = time(NULL);

	TRANSPORT_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);
	int subscribe_result = transport_interface->IoTHubTransport_Subscribe(transport);
	ASSERT_ARE_EQUAL(int, subscribe_result, 0);

	mocks.ResetAllCalls();
	setExpectedCallsForTransportDoWorkUpTo(mocks, &config, STEP_DOWORK_OPEN_CBS, DOWORK_MESSAGERECEIVER_NONE);
	setExpectedCallsForCbsAuthentication(mocks, &config, current_time);
	setExpectedCallsForCbsAuthTimeoutCheck(mocks, &config, current_time);
	setExpectedCallsForConnectionDoWork(mocks, &config);
	setExpectedCallsForHandleEventSendTimeout(mocks, &config, current_time);
	setExpectedCallsForSASTokenExpiryCheck(mocks, &config, current_time);
	EXPECTED_CALL(mocks, STRING_c_str(NULL));
	EXPECTED_CALL(mocks, messaging_create_source(NULL)).SetReturn(TEST_MESSAGERECEIVER_SOURCE);
	EXPECTED_CALL(mocks, messaging_create_target(NULL)).SetReturn(TEST_MESSAGERECEIVER_TARGET);
	EXPECTED_CALL(mocks, link_create(NULL, NULL, NULL, NULL, NULL)).SetReturn((LINK_HANDLE)NULL);
	STRICT_EXPECTED_CALL(mocks, amqpvalue_destroy(TEST_MESSAGERECEIVER_SOURCE));
	STRICT_EXPECTED_CALL(mocks, amqpvalue_destroy(TEST_MESSAGERECEIVER_TARGET));
	EXPECTED_CALL(mocks, messaging_create_source(NULL)).SetReturn((AMQP_VALUE)NULL);
	setExpectedCallsForConnectionDestroyUpTo(mocks, &config, STEP_DOWORK_CREATE_CBS);
	setExpectedCallsForRollEventsBackToWaitList(mocks, &config);
	setExpectedCallsForHandleEventSendTimeout(mocks, &config, current_time);

	// act
	transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);
	test_latest_cbs_put_token_callback(test_latest_cbs_put_token_context, CBS_OPERATION_RESULT_OK, 0, NULL);
	transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);

	// assert
	mocks.AssertActualAndExpectedCalls();

	// cleanup
	transport_interface->IoTHubTransport_Destroy(transport);
}

// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_076: [IoTHubTransportuAMQP_DoWork shall set the receiver link settle mode as receiver_settle_mode_first] 
// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_141: [If IoTHubTransportuAMQP_DoWork fails to set the settle mode on the AMQP link for receiving messages, the function shall fail and return immediately, flagging the connection to be re-stablished] 
TEST_FUNCTION(uAMQP_DoWork_messagereceiver_settle_mode_fails)
{
	// arrange
	CIoTHubTransportuAMQPMocks mocks;

	DLIST_ENTRY wts;
	BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
	TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)uAMQP_Protocol();
	IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
		TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME, (IO_TRANSPORT_PROVIDER_CALLBACK)getIOTransport };
	IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };

	test_TLS_io_transport = TEST_TLS_IO_PROVIDER;
	time_t current_time = time(NULL);

	TRANSPORT_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);
	int subscribe_result = transport_interface->IoTHubTransport_Subscribe(transport);
	ASSERT_ARE_EQUAL(int, subscribe_result, 0);

	mocks.ResetAllCalls();
	setExpectedCallsForTransportDoWorkUpTo(mocks, &config, STEP_DOWORK_OPEN_CBS, DOWORK_MESSAGERECEIVER_NONE);
	setExpectedCallsForCbsAuthentication(mocks, &config, current_time);
	setExpectedCallsForCbsAuthTimeoutCheck(mocks, &config, current_time);
	setExpectedCallsForConnectionDoWork(mocks, &config);
	setExpectedCallsForHandleEventSendTimeout(mocks, &config, current_time);
	setExpectedCallsForSASTokenExpiryCheck(mocks, &config, current_time);
	EXPECTED_CALL(mocks, STRING_c_str(NULL));
	EXPECTED_CALL(mocks, messaging_create_source(NULL)).SetReturn(TEST_MESSAGERECEIVER_SOURCE);
	EXPECTED_CALL(mocks, messaging_create_target(NULL)).SetReturn(TEST_MESSAGERECEIVER_TARGET);
	EXPECTED_CALL(mocks, link_create(NULL, NULL, NULL, NULL, NULL)).SetReturn(TEST_MESSAGERECEIVER_LINK);
	STRICT_EXPECTED_CALL(mocks, link_set_rcv_settle_mode(TEST_MESSAGERECEIVER_LINK, receiver_settle_mode_first)).SetReturn(1);
	STRICT_EXPECTED_CALL(mocks, amqpvalue_destroy(TEST_MESSAGERECEIVER_SOURCE));
	STRICT_EXPECTED_CALL(mocks, amqpvalue_destroy(TEST_MESSAGERECEIVER_TARGET));
	EXPECTED_CALL(mocks, messaging_create_source(NULL)).SetReturn((AMQP_VALUE)NULL);
	setExpectedCallsForConnectionDestroyUpTo(mocks, &config, STEP_DOWORK_CREATE_CBS);
	setExpectedCallsForRollEventsBackToWaitList(mocks, &config);
	setExpectedCallsForHandleEventSendTimeout(mocks, &config, current_time);

												 // act
	transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);
	test_latest_cbs_put_token_callback(test_latest_cbs_put_token_context, CBS_OPERATION_RESULT_OK, 0, NULL);
	transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);

	// assert
	mocks.AssertActualAndExpectedCalls();

	// cleanup
	transport_interface->IoTHubTransport_Destroy(transport);
}

// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_077: [IoTHubTransportuAMQP_DoWork shall create the AMQP message receiver using messagereceiver_create() uAMQP API] 
// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_078: [IoTHubTransportuAMQP_DoWork shall fail and return immediately if the uAMQP message receiver instance fails to be created, flagging the connection to be re-established] 
// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_119: [IoTHubTransportuAMQP_DoWork shall apply a default value of 65536 for the parameter ‘Link MAX message size’] 
TEST_FUNCTION(uAMQP_DoWork_messagereceiver_create_fails)
{
	// arrange
	CIoTHubTransportuAMQPMocks mocks;

	DLIST_ENTRY wts;
	BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
	TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)uAMQP_Protocol();
	IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
		TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME, (IO_TRANSPORT_PROVIDER_CALLBACK)getIOTransport };
	IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };

	test_TLS_io_transport = TEST_TLS_IO_PROVIDER;
	time_t current_time = time(NULL);

	TRANSPORT_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);
	int subscribe_result = transport_interface->IoTHubTransport_Subscribe(transport);
	ASSERT_ARE_EQUAL(int, subscribe_result, 0);

	mocks.ResetAllCalls();
	setExpectedCallsForTransportDoWorkUpTo(mocks, &config, STEP_DOWORK_OPEN_CBS, DOWORK_MESSAGERECEIVER_NONE);
	setExpectedCallsForCbsAuthentication(mocks, &config, current_time);
	setExpectedCallsForCbsAuthTimeoutCheck(mocks, &config, current_time);
	setExpectedCallsForConnectionDoWork(mocks, &config);
	setExpectedCallsForHandleEventSendTimeout(mocks, &config, current_time);
	setExpectedCallsForSASTokenExpiryCheck(mocks, &config, current_time);
	EXPECTED_CALL(mocks, STRING_c_str(NULL));
	EXPECTED_CALL(mocks, messaging_create_source(NULL)).SetReturn(TEST_MESSAGERECEIVER_SOURCE);
	EXPECTED_CALL(mocks, messaging_create_target(NULL)).SetReturn(TEST_MESSAGERECEIVER_TARGET);
	EXPECTED_CALL(mocks, link_create(NULL, NULL, NULL, NULL, NULL)).SetReturn(TEST_MESSAGERECEIVER_LINK);
	STRICT_EXPECTED_CALL(mocks, link_set_rcv_settle_mode(TEST_MESSAGERECEIVER_LINK, receiver_settle_mode_first));
	EXPECTED_CALL(mocks, link_set_max_message_size(NULL, NULL));
	EXPECTED_CALL(mocks, messagereceiver_create(NULL, NULL, NULL)).SetReturn(TEST_MESSAGE_RECEIVER).SetReturn((MESSAGE_RECEIVER_HANDLE)NULL);
	STRICT_EXPECTED_CALL(mocks, amqpvalue_destroy(TEST_MESSAGERECEIVER_SOURCE));
	STRICT_EXPECTED_CALL(mocks, amqpvalue_destroy(TEST_MESSAGERECEIVER_TARGET));
	EXPECTED_CALL(mocks, messaging_create_source(NULL)).SetReturn((AMQP_VALUE)NULL);
	setExpectedCallsForConnectionDestroyUpTo(mocks, &config, STEP_DOWORK_CREATE_CBS);
	setExpectedCallsForRollEventsBackToWaitList(mocks, &config);
	setExpectedCallsForHandleEventSendTimeout(mocks, &config, current_time);

	// act
	transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);
	test_latest_cbs_put_token_callback(test_latest_cbs_put_token_context, CBS_OPERATION_RESULT_OK, 0, NULL);
	transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);

	// assert
	mocks.AssertActualAndExpectedCalls();

	// cleanup
	transport_interface->IoTHubTransport_Destroy(transport);
}

// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_079: [IoTHubTransportuAMQP_DoWork shall open the AMQP message receiver using messagereceiver_open() uAMQP API, passing a callback function for handling C2D incoming messages] 
// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_080: [IoTHubTransportuAMQP_DoWork shall fail and return immediately if the uAMQP message receiver instance fails to be opened, flagging the connection to be re-established] 
TEST_FUNCTION(uAMQP_DoWork_messagereceiver_open_fails)
{
	// arrange
	CIoTHubTransportuAMQPMocks mocks;

	DLIST_ENTRY wts;
	BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
	TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)uAMQP_Protocol();
	IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
		TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME, (IO_TRANSPORT_PROVIDER_CALLBACK)getIOTransport };
	IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };

	test_TLS_io_transport = TEST_TLS_IO_PROVIDER;
	time_t current_time = time(NULL);

	TRANSPORT_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);
	int subscribe_result = transport_interface->IoTHubTransport_Subscribe(transport);
	ASSERT_ARE_EQUAL(int, subscribe_result, 0);

	mocks.ResetAllCalls();
	setExpectedCallsForTransportDoWorkUpTo(mocks, &config, STEP_DOWORK_OPEN_CBS, DOWORK_MESSAGERECEIVER_NONE);
	setExpectedCallsForCbsAuthentication(mocks, &config, current_time);
	setExpectedCallsForCbsAuthTimeoutCheck(mocks, &config, current_time);
	setExpectedCallsForConnectionDoWork(mocks, &config);
	setExpectedCallsForHandleEventSendTimeout(mocks, &config, current_time);
	setExpectedCallsForSASTokenExpiryCheck(mocks, &config, current_time);
	EXPECTED_CALL(mocks, STRING_c_str(NULL));
	EXPECTED_CALL(mocks, messaging_create_source(NULL)).SetReturn(TEST_MESSAGERECEIVER_SOURCE);
	EXPECTED_CALL(mocks, messaging_create_target(NULL)).SetReturn(TEST_MESSAGERECEIVER_TARGET);
	EXPECTED_CALL(mocks, link_create(NULL, NULL, NULL, NULL, NULL)).SetReturn(TEST_MESSAGERECEIVER_LINK);
	STRICT_EXPECTED_CALL(mocks, link_set_rcv_settle_mode(TEST_MESSAGERECEIVER_LINK, receiver_settle_mode_first));
	EXPECTED_CALL(mocks, link_set_max_message_size(NULL, NULL));
	EXPECTED_CALL(mocks, messagereceiver_create(NULL, NULL, NULL)).SetReturn(TEST_MESSAGE_RECEIVER);
	EXPECTED_CALL(mocks, messagereceiver_open(TEST_MESSAGE_RECEIVER, NULL, NULL)).SetReturn(1);
	STRICT_EXPECTED_CALL(mocks, amqpvalue_destroy(TEST_MESSAGERECEIVER_SOURCE));
	STRICT_EXPECTED_CALL(mocks, amqpvalue_destroy(TEST_MESSAGERECEIVER_TARGET));
	EXPECTED_CALL(mocks, messaging_create_source(NULL)).SetReturn((AMQP_VALUE)NULL);
	setExpectedCallsForConnectionDestroyUpTo(mocks, &config, STEP_DOWORK_CREATE_CBS);
	setExpectedCallsForRollEventsBackToWaitList(mocks, &config);
	setExpectedCallsForHandleEventSendTimeout(mocks, &config, current_time);

	// act
	transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);
	test_latest_cbs_put_token_callback(test_latest_cbs_put_token_context, CBS_OPERATION_RESULT_OK, 0, NULL);
	transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);

	// assert
	mocks.AssertActualAndExpectedCalls();

	// cleanup
	transport_interface->IoTHubTransport_Destroy(transport);
}

// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_055: [If the transport handle has a NULL connection, IoTHubTransportuAMQP_DoWork shall instantiate and initialize the uAMQP components and establish the connection] 
// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_082: [IoTHubTransportuAMQP_DoWork shall refresh the SAS token if the current token has been used for more than ‘sas_token_refresh_time’ milliseconds]
// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_128: [IoTHubTransportuAMQP_Create shall set parameter transport_state->sas_token_refresh_time with the default value of sas_token_lifetime/2 (milliseconds).] 
TEST_FUNCTION(uAMQP_DoWork_expired_SASToken_fails)
{
	// arrange
	CIoTHubTransportuAMQPMocks mocks;

	DLIST_ENTRY wts;
	BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
	TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)uAMQP_Protocol();
	IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
		TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME, (IO_TRANSPORT_PROVIDER_CALLBACK)getIOTransport };
	IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };

	test_TLS_io_transport = TEST_TLS_IO_PROVIDER;
	time_t current_time = time(NULL);
	time_t expiration_time = addSecondsToTime(current_time, (TEST_SAS_TOKEN_LIFETIME_MS/2)/1000 + 1);

	TRANSPORT_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);
	int subscribe_result = transport_interface->IoTHubTransport_Subscribe(transport);
	ASSERT_ARE_EQUAL(int, subscribe_result, 0);

	mocks.ResetAllCalls();
	setExpectedCallsForTransportDoWorkUpTo(mocks, &config, STEP_DOWORK_OPEN_CBS, DOWORK_MESSAGERECEIVER_NONE);
	setExpectedCallsForCbsAuthentication(mocks, &config, current_time);
	setExpectedCallsForCbsAuthTimeoutCheck(mocks, &config, current_time);
	setExpectedCallsForConnectionDoWork(mocks, &config);
	setExpectedCallsForHandleEventSendTimeout(mocks, &config, current_time);
	setExpectedCallsForSASTokenExpiryCheck(mocks, &config, expiration_time);
	EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(current_time);
	EXPECTED_CALL(mocks, SASToken_Create(NULL, NULL, NULL, 0)).SetReturn((STRING_HANDLE)NULL);
	setExpectedCallsForConnectionDestroyUpTo(mocks, &config, STEP_DOWORK_CREATE_CBS);
	setExpectedCallsForRollEventsBackToWaitList(mocks, &config);
	setExpectedCallsForHandleEventSendTimeout(mocks, &config, current_time);

	// act
	transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);
	test_latest_cbs_put_token_callback(test_latest_cbs_put_token_context, CBS_OPERATION_RESULT_OK, 0, NULL);
	transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);

	// assert
	mocks.AssertActualAndExpectedCalls();

	// cleanup
	transport_interface->IoTHubTransport_Destroy(transport);
}

// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_086: [IoTHubTransportuAMQP_DoWork shall move queued events to an “in-progress” list right before processing them for sending]
// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_089: [If the event contains a message of type IOTHUBMESSAGE_STRING, IoTHubTransportuAMQP_DoWork shall obtain its char* representation using IoTHubMessage_GetString()]
// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_090: [If the event contains a message of type IOTHUBMESSAGE_STRING, IoTHubTransportuAMQP_DoWork shall obtain the size of its char* representation using strlen()] 
TEST_FUNCTION(uAMQP_DoWork_succeeds)
{
	// arrange
	CIoTHubTransportuAMQPMocks mocks;

	DLIST_ENTRY wts;
	BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
	TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)uAMQP_Protocol();
	IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
		TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME, (IO_TRANSPORT_PROVIDER_CALLBACK)getIOTransport };
	IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };

	test_TLS_io_transport = TEST_TLS_IO_PROVIDER;
	time_t current_time = time(NULL);
	
	TRANSPORT_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);

	addTestEvents(config.waitingToSend, 2, true);

	mocks.ResetAllCalls();
	setExpectedCallsForTransportDoWorkUpTo(mocks, &config, STEP_DOWORK_OPEN_CBS, DOWORK_MESSAGERECEIVER_NONE);
	setExpectedCallsForCbsAuthentication(mocks, &config, current_time);
	setExpectedCallsForCbsAuthTimeoutCheck(mocks, &config, current_time);
	setExpectedCallsForConnectionDoWork(mocks, &config);
	setExpectedCallsForHandleEventSendTimeout(mocks, &config, current_time);
	setExpectedCallsForSASTokenExpiryCheck(mocks, &config, current_time);
	setExpectedCallsForCreateEventSender(mocks, &config);
	setExpectedCallsForSendPendingEvents(mocks, IOTHUBMESSAGE_STRING, current_time, 2);
	setExpectedCallsForConnectionDoWork(mocks, &config);
	setExpectedCallsForHandleEventSendTimeout(mocks, &config, current_time);

	// act
	transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);
	test_latest_cbs_put_token_callback(test_latest_cbs_put_token_context, CBS_OPERATION_RESULT_OK, 0, NULL);
	transport_interface->IoTHubTransport_DoWork(transport, TEST_IOTHUB_CLIENT_LL_HANDLE);

	// assert
	mocks.AssertActualAndExpectedCalls();

	// cleanup
	transport_interface->IoTHubTransport_Destroy(transport);
}

// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_041: [IoTHubTransportuAMQP_GetSendStatus shall return IOTHUB_CLIENT_INVALID_ARG if called with NULL parameter.] 
// Codes_SRS_IOTHUBTRANSPORTUAMQP_09_042: [IoTHubTransportuAMQP_GetSendStatus shall return IOTHUB_CLIENT_OK and status IOTHUB_CLIENT_SEND_STATUS_IDLE if there are currently no event items to be sent or being sent.]
TEST_FUNCTION(uAMQP_GetSendStatus_idle_succeeds)
{
	// arrange
	CIoTHubTransportuAMQPMocks mocks;

	DLIST_ENTRY wts;
	BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
	TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)uAMQP_Protocol();
	IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
		TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME, (IO_TRANSPORT_PROVIDER_CALLBACK)getIOTransport };
	IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };

	test_TLS_io_transport = TEST_TLS_IO_PROVIDER;
	time_t current_time = time(NULL);

	TRANSPORT_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);

	IOTHUB_CLIENT_STATUS iotHubClientStatus;

	mocks.ResetAllCalls();
	EXPECTED_CALL(mocks, DList_IsListEmpty(NULL)).SetReturn(1);
	EXPECTED_CALL(mocks, DList_IsListEmpty(NULL)).SetReturn(1);

	// act
	IOTHUB_CLIENT_RESULT result = transport_interface->IoTHubTransport_GetSendStatus(transport, &iotHubClientStatus);

	// assert
	mocks.AssertActualAndExpectedCalls();
	ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, result, IOTHUB_CLIENT_OK, "IoTHubTransport_GetSendStatus returned unexpected result.");
	ASSERT_ARE_EQUAL(IOTHUB_CLIENT_STATUS, iotHubClientStatus, IOTHUB_CLIENT_SEND_STATUS_IDLE, "IoTHubTransport_GetSendStatus returned unexpected status.");

	// cleanup
	transport_interface->IoTHubTransport_Destroy(transport);
}

// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_041: [IoTHubTransportuAMQP_GetSendStatus shall return IOTHUB_CLIENT_INVALID_ARG if called with NULL parameter.]  
// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_043: [IoTHubTransportuAMQP_GetSendStatus shall return IOTHUB_CLIENT_OK and status IOTHUB_CLIENT_SEND_STATUS_BUSY if there are currently event items to be sent or being sent.] 
TEST_FUNCTION(uAMQP_GetSendStatus_inProgress_not_empty_succeeds)
{
	// arrange
	CIoTHubTransportuAMQPMocks mocks;

	DLIST_ENTRY wts;
	BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
	TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)uAMQP_Protocol();
	IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
		TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME, (IO_TRANSPORT_PROVIDER_CALLBACK)getIOTransport };
	IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };

	test_TLS_io_transport = TEST_TLS_IO_PROVIDER;
	time_t current_time = time(NULL);

	TRANSPORT_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);

	IOTHUB_CLIENT_STATUS iotHubClientStatus;

	mocks.ResetAllCalls();
	EXPECTED_CALL(mocks, DList_IsListEmpty(NULL)).SetReturn(1);
	EXPECTED_CALL(mocks, DList_IsListEmpty(NULL)).SetReturn(0);

	// act
	IOTHUB_CLIENT_RESULT result = transport_interface->IoTHubTransport_GetSendStatus(transport, &iotHubClientStatus);

	// assert
	mocks.AssertActualAndExpectedCalls();
	ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, result, IOTHUB_CLIENT_OK, "IoTHubTransport_GetSendStatus returned unexpected result.");
	ASSERT_ARE_EQUAL(IOTHUB_CLIENT_STATUS, iotHubClientStatus, IOTHUB_CLIENT_SEND_STATUS_BUSY, "IoTHubTransport_GetSendStatus returned unexpected status.");

	// cleanup
	transport_interface->IoTHubTransport_Destroy(transport);
}

// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_041: [IoTHubTransportuAMQP_GetSendStatus shall return IOTHUB_CLIENT_INVALID_ARG if called with NULL parameter.]  
// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_043: [IoTHubTransportuAMQP_GetSendStatus shall return IOTHUB_CLIENT_OK and status IOTHUB_CLIENT_SEND_STATUS_BUSY if there are currently event items to be sent or being sent.] 
TEST_FUNCTION(uAMQP_GetSendStatus_waitingToSend_not_empty_succeeds)
{
	// arrange
	CIoTHubTransportuAMQPMocks mocks;

	DLIST_ENTRY wts;
	BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
	TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)uAMQP_Protocol();
	IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
		TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME, (IO_TRANSPORT_PROVIDER_CALLBACK)getIOTransport };
	IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };

	test_TLS_io_transport = TEST_TLS_IO_PROVIDER;
	time_t current_time = time(NULL);

	TRANSPORT_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);

	IOTHUB_CLIENT_STATUS iotHubClientStatus;

	mocks.ResetAllCalls();
	EXPECTED_CALL(mocks, DList_IsListEmpty(NULL)).SetReturn(0);

	// act
	IOTHUB_CLIENT_RESULT result = transport_interface->IoTHubTransport_GetSendStatus(transport, &iotHubClientStatus);

	// assert
	mocks.AssertActualAndExpectedCalls();
	ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, result, IOTHUB_CLIENT_OK, "IoTHubTransport_GetSendStatus returned unexpected result.");
	ASSERT_ARE_EQUAL(IOTHUB_CLIENT_STATUS, iotHubClientStatus, IOTHUB_CLIENT_SEND_STATUS_BUSY, "IoTHubTransport_GetSendStatus returned unexpected status.");

	// cleanup
	transport_interface->IoTHubTransport_Destroy(transport);
}

// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_037: [IoTHubTransportuAMQP_Subscribe shall fail if the transport handle parameter received is NULL.]
TEST_FUNCTION(uAMQP_Subscribe_NULL_transport_fails)
{
	// arrange
	CIoTHubTransportuAMQPMocks mocks;

	DLIST_ENTRY wts;
	BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
	TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)uAMQP_Protocol();
	IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
		TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME, (IO_TRANSPORT_PROVIDER_CALLBACK)getIOTransport };
	IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };
	TRANSPORT_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);

	mocks.ResetAllCalls();

	// act
	transport_interface->IoTHubTransport_Subscribe(NULL);

	// assert
	mocks.AssertActualAndExpectedCalls();

	// cleanup
	transport_interface->IoTHubTransport_Destroy(transport);
}

// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_039: [IoTHubTransportuAMQP_Unsubscribe shall fail if the transport handle parameter received is NULL.]
TEST_FUNCTION(uAMQP_Unsubscribe_NULL_transport_fails)
{
	// arrange
	CIoTHubTransportuAMQPMocks mocks;

	DLIST_ENTRY wts;
	BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
	TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)uAMQP_Protocol();
	IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
		TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME, (IO_TRANSPORT_PROVIDER_CALLBACK)getIOTransport };
	IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };
	TRANSPORT_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);

	mocks.ResetAllCalls();

	// act
	transport_interface->IoTHubTransport_Unsubscribe(NULL);

	// assert
	mocks.AssertActualAndExpectedCalls();

	// cleanup
	transport_interface->IoTHubTransport_Destroy(transport);
}

// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_044: [If handle parameter is NULL then IoTHubTransportuAMQP_SetOption shall return IOTHUB_CLIENT_INVALID_ARG.]
TEST_FUNCTION(uAMQP_SetOption_NULL_transport_fails)
{
	// arrange
	CIoTHubTransportuAMQPMocks mocks;

	DLIST_ENTRY wts;
	BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
	TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)uAMQP_Protocol();
	IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
		TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME, (IO_TRANSPORT_PROVIDER_CALLBACK)getIOTransport };
	IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };
	TRANSPORT_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);

	mocks.ResetAllCalls();

	// act
	IOTHUB_CLIENT_RESULT result = transport_interface->IoTHubTransport_SetOption(NULL, TEST_OPTION_SASTOKEN_LIFETIME, TEST_RANDOM_CHAR_SEQ);

	// assert
	mocks.AssertActualAndExpectedCalls();
	ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, result, IOTHUB_CLIENT_INVALID_ARG, "IoTHubTransport_SetOption returned unexpected result.");

	// cleanup
	transport_interface->IoTHubTransport_Destroy(transport);
}

// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_045: [If parameter optionName is NULL then IoTHubTransportuAMQP_SetOption shall return IOTHUB_CLIENT_INVALID_ARG.] 
TEST_FUNCTION(uAMQP_SetOption_NULL_option_fails)
{
	// arrange
	CIoTHubTransportuAMQPMocks mocks;

	DLIST_ENTRY wts;
	BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
	TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)uAMQP_Protocol();
	IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
		TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME, (IO_TRANSPORT_PROVIDER_CALLBACK)getIOTransport };
	IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };
	TRANSPORT_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);

	mocks.ResetAllCalls();


	// act
	IOTHUB_CLIENT_RESULT result = transport_interface->IoTHubTransport_SetOption(transport, NULL, TEST_RANDOM_CHAR_SEQ);

	// assert
	mocks.AssertActualAndExpectedCalls();
	ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, result, IOTHUB_CLIENT_INVALID_ARG, "IoTHubTransport_SetOption returned unexpected result.");

	// cleanup
	transport_interface->IoTHubTransport_Destroy(transport);
}

// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_046: [If parameter value is NULL then IoTHubTransportuAMQP_SetOption shall return IOTHUB_CLIENT_INVALID_ARG.]
TEST_FUNCTION(uAMQP_SetOption_NULL_value_fails)
{
	// arrange
	CIoTHubTransportuAMQPMocks mocks;

	DLIST_ENTRY wts;
	BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
	TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)uAMQP_Protocol();
	IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
		TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME, (IO_TRANSPORT_PROVIDER_CALLBACK)getIOTransport };
	IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };
	TRANSPORT_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);

	mocks.ResetAllCalls();

	// act
	IOTHUB_CLIENT_RESULT result = transport_interface->IoTHubTransport_SetOption(transport, TEST_OPTION_SASTOKEN_LIFETIME, NULL);

	// assert
	mocks.AssertActualAndExpectedCalls();
	ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, result, IOTHUB_CLIENT_INVALID_ARG, "IoTHubTransport_SetOption returned unexpected result.");

	// cleanup
	transport_interface->IoTHubTransport_Destroy(transport);
}

// Tests_SRS_IOTHUBTRANSPORTUAMQP_09_047: [If optionName is not an option supported then IoTHubTransportuAMQP_SetOption shall return IOTHUB_CLIENT_INVALID_ARG.]
TEST_FUNCTION(uAMQP_SetOption_invalid_option_fails)
{
	// arrange
	CIoTHubTransportuAMQPMocks mocks;

	DLIST_ENTRY wts;
	BASEIMPLEMENTATION::DList_InitializeListHead(&wts);
	TRANSPORT_PROVIDER* transport_interface = (TRANSPORT_PROVIDER*)uAMQP_Protocol();
	IOTHUB_CLIENT_CONFIG client_config = { (IOTHUB_CLIENT_TRANSPORT_PROVIDER)transport_interface,
		TEST_DEVICE_ID, TEST_DEVICE_KEY, TEST_IOT_HUB_NAME, TEST_IOT_HUB_SUFFIX, TEST_PROT_GW_HOSTNAME, (IO_TRANSPORT_PROVIDER_CALLBACK)getIOTransport };
	IOTHUBTRANSPORT_CONFIG config = { &client_config, &wts };
	TRANSPORT_HANDLE transport = transport_interface->IoTHubTransport_Create(&config);

	mocks.ResetAllCalls();

	// act
	IOTHUB_CLIENT_RESULT result = transport_interface->IoTHubTransport_SetOption(transport, "some_invalid_option", TEST_RANDOM_CHAR_SEQ);

	// assert
	mocks.AssertActualAndExpectedCalls();
	ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, result, IOTHUB_CLIENT_INVALID_ARG, "IoTHubTransport_SetOption returned unexpected result.");

	// cleanup
	transport_interface->IoTHubTransport_Destroy(transport);
}

END_TEST_SUITE(iothubtransportuamqp_unittests)
