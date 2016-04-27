// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef IOTDM_CONNECT_MOCKS_H
#define IOTDM_CONNECT_MOCKS_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <../../../wakaama/core/er-coap-13/er-coap-13.h>
#include <../../../wakaama/core/internals.h>

#ifdef __cplusplus
}
#endif

#include "iotdm_internal.h"

#include "micromock.h"
#include "micromockcharstararenullterminatedstrings.h"

#include <cstdint>

namespace REAL
{
#include "strings.c"
};

const char sasTokenValue[] = "token";
const char optionValue[] = "option";

class countdown
{
    bool _enabled;
    uint32_t _val;
public:
    countdown() : _val(0), _enabled(false) {}
    countdown(uint32_t value) : _val(value), _enabled(true) {}
    countdown& operator=(const countdown& other)
    {
        _val = other._val;
        _enabled = other._enabled;
        return *this;
    }
    bool are_we_there_yet()
    {
        if (!_enabled) return false;
        if (!_val) return true;
        --_val;
        return !_val;
    }
};

struct function_failer
{
    countdown counter;

    function_failer() : counter() {}
    void fail_on(uint32_t whichInvocation)
    {
        counter = countdown(whichInvocation);
    }
    void reset()
    {
        counter = countdown();
    }
};

function_failer STRING_construct_failer;
function_failer STRING_concat_failer;
function_failer STRING_new_failer;

TYPED_MOCK_CLASS(iotdm_connect_mocks, CGlobalMock)
{
public:
    // time_t lwm2m_gettime(void);
    MOCK_STATIC_METHOD_0(, time_t, lwm2m_gettime)
    MOCK_METHOD_END(time_t, 1)
    // void lwm2m_free(void);
    MOCK_STATIC_METHOD_1(, void, lwm2m_free, void*, block)
        free(block);
    MOCK_VOID_METHOD_END()
    // lwm2m_list_t * lwm2m_list_add(lwm2m_list_t * head, lwm2m_list_t * node);
    MOCK_STATIC_METHOD_2(, lwm2m_list_t*, lwm2m_list_add, lwm2m_list_t*, head, lwm2m_list_t*, node)
    MOCK_METHOD_END(lwm2m_list_t*, NULL)
    // void lwm2m_handle_packet(lwm2m_context_t * contextP, uint8_t * buffer, int length, void * fromSessionH);
    MOCK_STATIC_METHOD_4(, void, lwm2m_handle_packet, lwm2m_context_t*, contextP, uint8_t*, buffer, int, length, void*, fromSessionH)
    MOCK_VOID_METHOD_END()
    // char * coap_get_multi_option_as_string(multi_option_t * option);
    MOCK_STATIC_METHOD_1(, char*, coap_get_multi_option_as_string, multi_option_t*, option)
    MOCK_METHOD_END(char*, (char*)optionValue)
    // int coap_set_header_content_type(void *packet, unsigned int content_type);
    MOCK_STATIC_METHOD_2(, int, coap_set_header_content_type, void*, packet, unsigned int, content_type)
    MOCK_METHOD_END(int, 0)
    // int coap_set_header_uri_host(void *packet, const char *host);
    MOCK_STATIC_METHOD_2(, int, coap_set_header_uri_host, void*, packet, const char*, host)
    MOCK_METHOD_END(int, 0)
    // int coap_set_header_uri_path(void *packet, const char *path);
    MOCK_STATIC_METHOD_2(, int, coap_set_header_uri_path, void*, packet, const char*, path)
    MOCK_METHOD_END(int, 0)
    // int coap_set_header_uri_query(void *packet, const char *query);
    MOCK_STATIC_METHOD_2(, int, coap_set_header_uri_query, void*, packet, const char*, query)
    MOCK_METHOD_END(int, 0)
    // int coap_set_payload(void *packet, const void *payload, size_t length);
    MOCK_STATIC_METHOD_3(, int, coap_set_payload, void*, packet, const void*, payload, size_t, length)
    MOCK_METHOD_END(int, 0)
    // lwm2m_transaction_t * transaction_new(coap_message_type_t type, coap_method_t method, char * altPath, lwm2m_uri_t * uriP, uint16_t mID, uint8_t token_len, uint8_t* token, lwm2m_endpoint_type_t peerType, void * peerP);
    MOCK_STATIC_METHOD_9(, lwm2m_transaction_t*, transaction_new, coap_message_type_t, type, coap_method_t, method, char*, altPath, lwm2m_uri_t*, uriP, uint16_t, mID, uint8_t, token_len, uint8_t*, token, lwm2m_endpoint_type_t, peerType, void*, peerP)
    MOCK_METHOD_END(lwm2m_transaction_t*, NULL)
    // int transaction_send(lwm2m_context_t * contextP, lwm2m_transaction_t * transacP);
    MOCK_STATIC_METHOD_2(, int, transaction_send, lwm2m_context_t*, contextP, lwm2m_transaction_t*, transacP)
    MOCK_METHOD_END(int, 0)
    // time_t get_time(time_t* currentTime);
    MOCK_STATIC_METHOD_1(, time_t, get_time, time_t*, currentTime)
    MOCK_METHOD_END(time_t, 0)
    // const IO_INTERFACE_DESCRIPTION* socketio_get_interface_description(void);
    MOCK_STATIC_METHOD_0(, const IO_INTERFACE_DESCRIPTION*, socketio_get_interface_description)
    MOCK_METHOD_END(const IO_INTERFACE_DESCRIPTION*, NULL)
    // const IO_INTERFACE_DESCRIPTION* platform_get_default_tlsio(void);
    MOCK_STATIC_METHOD_0(, const IO_INTERFACE_DESCRIPTION*, platform_get_default_tlsio)
    MOCK_METHOD_END(const IO_INTERFACE_DESCRIPTION*, NULL)
    // XIO_HANDLE xio_create(const IO_INTERFACE_DESCRIPTION* io_interface_description, cont void* io_create_parameters, LOGGER_LOG logger_log);
    MOCK_STATIC_METHOD_3(, XIO_HANDLE, xio_create, const IO_INTERFACE_DESCRIPTION*, io_interface_description, const void*, io_create_parameters, LOGGER_LOG, logger_log)
    MOCK_METHOD_END(XIO_HANDLE, (XIO_HANDLE)1)
    // int xio_open(XIO_HANDLE xio, ON_IO_OPEN_COMPLETE on_io_open_complete, void* on_io_open_complete_context, ON_BYTES_RECEIVED on_bytes_received, void* on_bytes_received_context, ON_IO_ERROR on_io_error, void* on_io_error_context);
    MOCK_STATIC_METHOD_7(, int, xio_open, XIO_HANDLE, socket_io, ON_IO_OPEN_COMPLETE, on_io_open_complete, void*, on_io_open_complete_context, ON_BYTES_RECEIVED, on_bytes_received, void*, on_bytes_received_context, ON_IO_ERROR, on_io_error, void*, on_io_error_context)
    MOCK_METHOD_END(int, (free(on_io_open_complete_context), 0))
    // int xio_setoption(XIO_HANDLE xio, const char* optionName, const void* value)
    MOCK_STATIC_METHOD_3(, int, xio_setoption, XIO_HANDLE, xio, const char*, optionName, const void*, value)
    MOCK_METHOD_END(int, 0)
    // int xio_send(XIO_HANDLE socket_io, const void* buffer, size_t size, ON_SEND_COMPLETE on_send_complete, void* callback_context);
    MOCK_STATIC_METHOD_5(, int, xio_send, XIO_HANDLE, socket_io, const void*, buffer, size_t, size, ON_SEND_COMPLETE, on_send_complete, void*, callback_context)
    MOCK_METHOD_END(int, 0)
    // int xio_close(XIO_HANDLE xio, ON_IO_CLOSE_COMPLETE on_io_close_complete, void* callback_context)
    MOCK_STATIC_METHOD_3(, int, xio_close, XIO_HANDLE, xio, ON_IO_CLOSE_COMPLETE, on_io_close_complete, void*, callback_context)
    MOCK_METHOD_END(int, 0)
    // void xio_destroy(XIO_HANDLE xio);
    MOCK_STATIC_METHOD_1(, void, xio_destroy, XIO_HANDLE, xio)
    MOCK_VOID_METHOD_END()
    // STRING_HANDLE STRING_new(void);
    MOCK_STATIC_METHOD_0(, STRING_HANDLE, STRING_new)
        STRING_HANDLE h = STRING_new_failer.counter.are_we_there_yet() ? NULL : REAL::STRING_new();
    MOCK_METHOD_END(STRING_HANDLE, h)
    // STRING_HANDLE STRING_construct(const char* psz);
    MOCK_STATIC_METHOD_1(, STRING_HANDLE, STRING_construct, const char*, psz)
        STRING_HANDLE h = STRING_construct_failer.counter.are_we_there_yet() ? NULL : REAL::STRING_construct(psz);
    MOCK_METHOD_END(STRING_HANDLE, h)
    // void STRING_delete(STRING_HANDLE handle);
    MOCK_STATIC_METHOD_1(, void, STRING_delete, STRING_HANDLE, handle)
        REAL::STRING_delete(handle);
    MOCK_VOID_METHOD_END()
    // int STRING_concat(STRING_HANDLE handle, const char* s2);
    MOCK_STATIC_METHOD_2(, int, STRING_concat, STRING_HANDLE, handle, const char*, s2)
        int val = STRING_concat_failer.counter.are_we_there_yet() ? 1 : REAL::STRING_concat(handle, s2);
    MOCK_METHOD_END(int, val)
    // int STRING_concat_with_STRING(STRING_HANDLE s1, STRING_HANDLE s2);
    MOCK_STATIC_METHOD_2(, int, STRING_concat_with_STRING, STRING_HANDLE, s1, STRING_HANDLE, s2)
        int val = REAL::STRING_concat_with_STRING(s1, s2);
    MOCK_METHOD_END(int, val)
    // const char* STRING_c_str(STRING_HANDLE handle);
    MOCK_STATIC_METHOD_1(, const char*, STRING_c_str, STRING_HANDLE, handle)
        const char* str = REAL::STRING_c_str(handle);
    MOCK_METHOD_END(const char*, str)
    // size_t STRING_length(STRING_HANDLE handle);
    MOCK_STATIC_METHOD_1(, size_t, STRING_length, STRING_HANDLE, handle)
        size_t len = REAL::STRING_length(handle);
    MOCK_METHOD_END(size_t, len)
    // STRING_HANDLE SASToken_Create(STRING_HANDLE key, STRING_HANDLE scope, STRING_HANDLE keyName, size_t expiry);
    MOCK_STATIC_METHOD_4(, STRING_HANDLE, SASToken_Create, STRING_HANDLE, key, STRING_HANDLE, scope, STRING_HANDLE, keyName, size_t, expiry)
        STRING_HANDLE token = REAL::STRING_construct(sasTokenValue);
    MOCK_METHOD_END(STRING_HANDLE, token)
    // void ThreadAPI_Sleep(unsigned int milliseconds);
    MOCK_STATIC_METHOD_1(, void, ThreadAPI_Sleep, unsigned int, milliseconds)
    MOCK_VOID_METHOD_END()
};

DECLARE_GLOBAL_MOCK_METHOD_0(iotdm_connect_mocks, , time_t, lwm2m_gettime);
DECLARE_GLOBAL_MOCK_METHOD_1(iotdm_connect_mocks, , void, lwm2m_free, void*, location);
DECLARE_GLOBAL_MOCK_METHOD_2(iotdm_connect_mocks, , lwm2m_list_t*, lwm2m_list_add, lwm2m_list_t*, head, lwm2m_list_t*, node);
DECLARE_GLOBAL_MOCK_METHOD_4(iotdm_connect_mocks, , void, lwm2m_handle_packet, lwm2m_context_t*, contextP, uint8_t*, buffer, int, length, void*, fromSessionH);
DECLARE_GLOBAL_MOCK_METHOD_1(iotdm_connect_mocks, , char*, coap_get_multi_option_as_string, multi_option_t*, option);
DECLARE_GLOBAL_MOCK_METHOD_2(iotdm_connect_mocks, , int, coap_set_header_content_type, void*, packet, unsigned int, content_type);
DECLARE_GLOBAL_MOCK_METHOD_2(iotdm_connect_mocks, , int, coap_set_header_uri_host, void*, packet, const char*, host);
DECLARE_GLOBAL_MOCK_METHOD_2(iotdm_connect_mocks, , int, coap_set_header_uri_path, void*, packet, const char*, path);
DECLARE_GLOBAL_MOCK_METHOD_2(iotdm_connect_mocks, , int, coap_set_header_uri_query, void*, packet, const char*, query);
DECLARE_GLOBAL_MOCK_METHOD_3(iotdm_connect_mocks, , int, coap_set_payload, void*, packet, const void*, payload, size_t, length);
DECLARE_GLOBAL_MOCK_METHOD_9(iotdm_connect_mocks, , lwm2m_transaction_t*, transaction_new, coap_message_type_t, type, coap_method_t, method, char*, altPath, lwm2m_uri_t*, uriP, uint16_t, mID, uint8_t, token_len, uint8_t*, token, lwm2m_endpoint_type_t, peerType, void*, peerP);
DECLARE_GLOBAL_MOCK_METHOD_2(iotdm_connect_mocks, , int, transaction_send, lwm2m_context_t*, contextP, lwm2m_transaction_t*, transacP);
DECLARE_GLOBAL_MOCK_METHOD_1(iotdm_connect_mocks, , time_t, get_time, time_t*, currentTime);
DECLARE_GLOBAL_MOCK_METHOD_0(iotdm_connect_mocks, , const IO_INTERFACE_DESCRIPTION*, socketio_get_interface_description);
DECLARE_GLOBAL_MOCK_METHOD_0(iotdm_connect_mocks, , const IO_INTERFACE_DESCRIPTION*, platform_get_default_tlsio);
DECLARE_GLOBAL_MOCK_METHOD_3(iotdm_connect_mocks, , XIO_HANDLE, xio_create, const IO_INTERFACE_DESCRIPTION*, io_interface_description, const void*, io_create_parameters, LOGGER_LOG, logger_log);
DECLARE_GLOBAL_MOCK_METHOD_7(iotdm_connect_mocks, , int, xio_open, XIO_HANDLE, socket_io, ON_IO_OPEN_COMPLETE, on_io_open_complete, void*, on_io_open_complete_context, ON_BYTES_RECEIVED, on_bytes_received, void*, on_bytes_received_context, ON_IO_ERROR, on_io_error, void*, on_io_error_context);
DECLARE_GLOBAL_MOCK_METHOD_3(iotdm_connect_mocks, , int, xio_setoption, XIO_HANDLE, xio, const char*, optionName, const void*, value);
DECLARE_GLOBAL_MOCK_METHOD_5(iotdm_connect_mocks, , int, xio_send, XIO_HANDLE, socket_io, const void*, buffer, size_t, size, ON_SEND_COMPLETE, on_send_complete, void*, callback_context);
DECLARE_GLOBAL_MOCK_METHOD_3(iotdm_connect_mocks, , int, xio_close, XIO_HANDLE, xio, ON_IO_CLOSE_COMPLETE, on_io_close_complete, void*, callback_context);
DECLARE_GLOBAL_MOCK_METHOD_1(iotdm_connect_mocks, , void, xio_destroy, XIO_HANDLE, xio);
DECLARE_GLOBAL_MOCK_METHOD_0(iotdm_connect_mocks, , STRING_HANDLE, STRING_new);
DECLARE_GLOBAL_MOCK_METHOD_1(iotdm_connect_mocks, , STRING_HANDLE, STRING_construct, const char*, psz);
DECLARE_GLOBAL_MOCK_METHOD_1(iotdm_connect_mocks, , void, STRING_delete, STRING_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_2(iotdm_connect_mocks, , int, STRING_concat, STRING_HANDLE, handle, const char*, s2);
DECLARE_GLOBAL_MOCK_METHOD_2(iotdm_connect_mocks, , int, STRING_concat_with_STRING, STRING_HANDLE, s1, STRING_HANDLE, s2);
DECLARE_GLOBAL_MOCK_METHOD_1(iotdm_connect_mocks, , const char*, STRING_c_str, STRING_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_1(iotdm_connect_mocks, , size_t, STRING_length, STRING_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_4(iotdm_connect_mocks, , STRING_HANDLE, SASToken_Create, STRING_HANDLE, key, STRING_HANDLE, scope, STRING_HANDLE, keyName, size_t, expiry);
extern "C" DECLARE_GLOBAL_MOCK_METHOD_1(iotdm_connect_mocks, , void, ThreadAPI_Sleep, unsigned int, milliseconds);

static MICROMOCK_MUTEX_HANDLE g_testByTest;
static MICROMOCK_GLOBAL_SEMAPHORE_HANDLE g_dllByDll;

#define MICROMOCK_BOILERPLATE \
    TEST_SUITE_INITIALIZE(Before)                                               \
    {                                                                           \
        TEST_INITIALIZE_MEMORY_DEBUG(g_dllByDll);                                    \
        g_testByTest = MicroMockCreateMutex();                                  \
        ASSERT_IS_NOT_NULL(g_testByTest);                                       \
    }                                                                           \
                                                                                \
    TEST_SUITE_CLEANUP(After)                                                   \
    {                                                                           \
        MicroMockDestroyMutex(g_testByTest);                                    \
        TEST_DEINITIALIZE_MEMORY_DEBUG(g_dllByDll);                                  \
    }                                                                           \
                                                                                \
    TEST_FUNCTION_INITIALIZE(BeforeEach)                                        \
    {                                                                           \
        if (!MicroMockAcquireMutex(g_testByTest))                               \
        {                                                                       \
            ASSERT_FAIL("our mutex is ABANDONED. Failure in test framework");   \
        }                                                                       \
                                                                                \
        BeforeEachTest();                                                       \
    }                                                                           \
                                                                                \
    TEST_FUNCTION_CLEANUP(AfterEach)                                            \
    {                                                                           \
        if (!MicroMockReleaseMutex(g_testByTest))                               \
        {                                                                       \
            ASSERT_FAIL("failure in test framework at ReleaseMutex");           \
        }                                                                       \
    }                                                                           \


#endif // !IOTDM_CONNECT_MOCKS_H
