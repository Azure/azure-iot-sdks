// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <cstdlib>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include <cstdbool>
#include <cstddef>
#include <csignal>
#include "testrunnerswitcher.h"
#include "micromock.h"
#include "micromockcharstararenullterminatedstrings.h"

#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/threadapi.h"
#include "azure_c_shared_utility/map.h"
#include "azure_c_shared_utility/lock.h"
#include "azure_c_shared_utility/strings.h"
#include "azure_c_shared_utility/string_tokenizer.h"

#include "connection_string_parser.h"
#include "iothub_service_client_auth.h"

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
};

static MICROMOCK_MUTEX_HANDLE g_testByTest;
static MICROMOCK_GLOBAL_SEMAPHORE_HANDLE g_dllByDll;

static const char TEST_CONST_CHAR[] = "a";
static const char* TEST_CONST_CHAR_PTR = "TestConstChar";
static const char* TEST_CONST_CHAR_PTR_NULL = (const char*)NULL;

static STRING_HANDLE TEST_STRING_HANDLE = (STRING_HANDLE)0x4242;
static STRING_HANDLE TEST_STRING_HANDLE_NULL = (STRING_HANDLE)NULL;

static MAP_HANDLE TEST_MAP_HANDLE = (MAP_HANDLE)0x4343;
static MAP_HANDLE TEST_MAP_HANDLE_NULL = (MAP_HANDLE)NULL;

static STRING_TOKENIZER_HANDLE TEST_STRING_TOKENIZER_HANDLE = (STRING_TOKENIZER_HANDLE)0x4444;
static STRING_TOKENIZER_HANDLE TEST_STRING_TOKENIZER_HANDLE_NULL = (STRING_TOKENIZER_HANDLE)NULL;

static STRING_HANDLE TEST_KEY_STRING_HANDLE = (STRING_HANDLE)0x4545;
static STRING_HANDLE TEST_VALUE_STRING_HANDLE = (STRING_HANDLE)0x4646;

static const char* TEST_CHAR_PTR = "TestString";
static const char* TEST_CONNECTION_STRING = "HostName=aaa.bbb.net;SharedAccessKeyName=xxx;SharedAccessKey=yyy";
static size_t currentmalloc_call;
static size_t whenShallmalloc_fail;

char* TEST_HOSTNAME = NULL; // "theHostName";
char* TEST_IOTHUBNAME = NULL; // "theIotHubName";
char* TEST_IOTHUBSUFFIX = NULL; // "theIotHubSuffix";
char* TEST_SHAREDACCESSKEY = NULL; // "theSharedAccessKey";
char* TEST_SHAREDACCESSKEYNAME = NULL; // "theSharedAccessKeyName";

TYPED_MOCK_CLASS(CIoTHubServiceClientAuthMocks, CGlobalMock)
{
public:
    /* crt_abstractions mocks */
    MOCK_STATIC_METHOD_2(, int, mallocAndStrcpy_s, char**, destination, const char*, source)
    MOCK_METHOD_END(int, (*destination = (char*)BASEIMPLEMENTATION::gballoc_malloc(strlen(source) + 1), strcpy(*destination, source), 0));

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

    MOCK_STATIC_METHOD_1(, void, gballoc_free, void*, ptr)
        BASEIMPLEMENTATION::gballoc_free(ptr);
    MOCK_VOID_METHOD_END()

    /* Map mocks */
    MOCK_STATIC_METHOD_1(, MAP_HANDLE, Map_Create, MAP_FILTER_CALLBACK, mapFilterFunc)
        MOCK_METHOD_END(MAP_HANDLE, TEST_MAP_HANDLE);
    MOCK_STATIC_METHOD_1(, void, Map_Destroy, MAP_HANDLE, handle)
        MOCK_VOID_METHOD_END();
    MOCK_STATIC_METHOD_3(, MAP_RESULT, Map_Add, MAP_HANDLE, handle, const char*, key, const char*, value)
        MOCK_METHOD_END(MAP_RESULT, MAP_OK);
    MOCK_STATIC_METHOD_2(, const char*, Map_GetValueFromKey, MAP_HANDLE, handle, const char*, key)
        MOCK_METHOD_END(const char*, TEST_CONST_CHAR_PTR);

    /* STRING mocks */
    MOCK_STATIC_METHOD_0(, STRING_HANDLE, STRING_new)
        MOCK_METHOD_END(STRING_HANDLE, TEST_STRING_HANDLE);
    MOCK_STATIC_METHOD_1(, STRING_HANDLE, STRING_construct, const char*, s)
        MOCK_METHOD_END(STRING_HANDLE, TEST_STRING_HANDLE);
    MOCK_STATIC_METHOD_1(, void, STRING_delete, STRING_HANDLE, handle)
        MOCK_VOID_METHOD_END();
    MOCK_STATIC_METHOD_1(, const char*, STRING_c_str, STRING_HANDLE, s)
        MOCK_METHOD_END(const char*, TEST_CONST_CHAR);

    /* STRING_Tokenizer mocks */
    MOCK_STATIC_METHOD_1(, STRING_TOKENIZER_HANDLE, STRING_TOKENIZER_create, STRING_HANDLE, handle)
        MOCK_METHOD_END(STRING_TOKENIZER_HANDLE, TEST_STRING_TOKENIZER_HANDLE);
    MOCK_STATIC_METHOD_3(, int, STRING_TOKENIZER_get_next_token, STRING_TOKENIZER_HANDLE, t, STRING_HANDLE, output, const char*, delimiters)
        MOCK_METHOD_END(int, 1);
    MOCK_STATIC_METHOD_1(, void, STRING_TOKENIZER_destroy, STRING_TOKENIZER_HANDLE, t)
        MOCK_VOID_METHOD_END();

    /* Connection string parser mock */
    MOCK_STATIC_METHOD_1(, MAP_HANDLE, connectionstringparser_parse, STRING_HANDLE, connectionString)
    MOCK_METHOD_END(MAP_HANDLE, TEST_MAP_HANDLE);
};

DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubServiceClientAuthMocks, , int, mallocAndStrcpy_s, char**, destination, const char*, source);

DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubServiceClientAuthMocks, , void*, gballoc_malloc, size_t, size);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubServiceClientAuthMocks, , void, gballoc_free, void*, ptr)

DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubServiceClientAuthMocks, , MAP_HANDLE, Map_Create, MAP_FILTER_CALLBACK, mapFilterFunc);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubServiceClientAuthMocks, , void, Map_Destroy, MAP_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubServiceClientAuthMocks, , MAP_RESULT, Map_Add, MAP_HANDLE, handle, const char*, key, const char*, value);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubServiceClientAuthMocks, , const char*, Map_GetValueFromKey, MAP_HANDLE, handle, const char*, key);

DECLARE_GLOBAL_MOCK_METHOD_0(CIoTHubServiceClientAuthMocks, , STRING_HANDLE, STRING_new);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubServiceClientAuthMocks, , STRING_HANDLE, STRING_construct, const char*, s);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubServiceClientAuthMocks, , void, STRING_delete, STRING_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubServiceClientAuthMocks, , const char*, STRING_c_str, STRING_HANDLE, s);

DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubServiceClientAuthMocks, , STRING_TOKENIZER_HANDLE, STRING_TOKENIZER_create, STRING_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubServiceClientAuthMocks, , int, STRING_TOKENIZER_get_next_token, STRING_TOKENIZER_HANDLE, t, STRING_HANDLE, output, const char*, delimiters);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubServiceClientAuthMocks, , void, STRING_TOKENIZER_destroy, STRING_TOKENIZER_HANDLE, t);

DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubServiceClientAuthMocks, , MAP_HANDLE, connectionstringparser_parse, STRING_HANDLE, connectionString);

BEGIN_TEST_SUITE(iothub_service_client_auth_ut)

TEST_SUITE_INITIALIZE(TestClassInitialize)
{
    TEST_INITIALIZE_MEMORY_DEBUG(g_dllByDll);
    g_testByTest = MicroMockCreateMutex();
    ASSERT_IS_NOT_NULL(g_testByTest);
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
    currentmalloc_call = 0;
    whenShallmalloc_fail = 0;
}

TEST_FUNCTION_CLEANUP(TestMethodCleanup)
{
    if (!MicroMockReleaseMutex(g_testByTest))
    {
        ASSERT_FAIL("failure in test framework at ReleaseMutex");
    }
}

/* Tests_SRS_IOTHUBSERVICECLIENT_12_001: [/* IoTHubServiceClientAuth_CreateFromConnectionString shall verify the input parameter and if it is NULL then return NULL **]*/
TEST_FUNCTION(IoTHubServiceClientAuth_CreateFromConnectionString_return_null_if_input_parameter_connectionString_is_NULL)
{
    // arrange
    CIoTHubServiceClientAuthMocks mocks;

    // act
    IOTHUB_SERVICE_CLIENT_AUTH_HANDLE result = IoTHubServiceClientAuth_CreateFromConnectionString(NULL);

    // assert
    ASSERT_IS_NULL(result);
    mocks.AssertActualAndExpectedCalls();
}

/* Tests_SRS_IOTHUBSERVICECLIENT_12_003: [** If the allocation failed, IoTHubServiceClientAuth_CreateFromConnectionString shall return NULL **]*/
TEST_FUNCTION(IoTHubServiceClientAuth_CreateFromConnectionString_return_null_if_allocate_memory_fails)
{
    // arrange
    CIoTHubServiceClientAuthMocks mocks;

    whenShallmalloc_fail = 1;
    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(sizeof(IOTHUB_SERVICE_CLIENT_AUTH)))
        .IgnoreArgument(1);

    // act
    IOTHUB_SERVICE_CLIENT_AUTH_HANDLE result = IoTHubServiceClientAuth_CreateFromConnectionString(TEST_CHAR_PTR);

    // assert
    ASSERT_ARE_EQUAL(void_ptr, NULL, result);
    mocks.AssertActualAndExpectedCalls();
}

/* Tests_SRS_IOTHUBSERVICECLIENT_12_002: [** IoTHubServiceClientAuth_CreateFromConnectionString shall allocate memory for a new service client instance. **]*/
/* Tests_SRS_IOTHUBSERVICECLIENT_12_009: [** IoTHubServiceClientAuth_CreateFromConnectionString shall create a STRING_HANDLE from the given connection string by calling STRING_construct. **] */
/* Tests_SRS_IOTHUBSERVICECLIENT_12_010: [** If the STRING_construct fails, IoTHubServiceClientAuth_CreateFromConnectionString shall do clean up and return NULL. **] */
TEST_FUNCTION(IoTHubServiceClientAuth_CreateFromConnectionString_do_clean_up_and_return_null_if_STRING_construct_fails)
{
    // arrange
    CIoTHubServiceClientAuthMocks mocks;

    whenShallmalloc_fail = 0;
    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_CHAR_PTR))
        .SetReturn((STRING_HANDLE)NULL);

    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    // act
    IOTHUB_SERVICE_CLIENT_AUTH_HANDLE result = IoTHubServiceClientAuth_CreateFromConnectionString(TEST_CHAR_PTR);

    // assert
    ASSERT_ARE_EQUAL(void_ptr, NULL, result);
    mocks.AssertActualAndExpectedCalls();
}

/* Tests_SRS_IOTHUBSERVICECLIENT_12_004: [** IoTHubServiceClientAuth_CreateFromConnectionString shall populate hostName, iotHubName, iotHubSuffix, sharedAccessKeyName, sharedAccessKeyValue from the given connection string by calling connectionstringparser_parse **]*/
/* Tests_SRS_IOTHUBSERVICECLIENT_12_005: [** If populating the IOTHUB_SERVICE_CLIENT_AUTH fails, IoTHubServiceClientAuth_CreateFromConnectionString shall do clean up and return NULL **]*/
TEST_FUNCTION(IoTHubServiceClientAuth_CreateFromConnectionString_do_clean_up_and_return_null_if_connectionstringparser_parse_fails)
{
    // arrange
    CIoTHubServiceClientAuthMocks mocks;

    whenShallmalloc_fail = 0;
    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_CHAR_PTR));

    STRICT_EXPECTED_CALL(mocks, connectionstringparser_parse(TEST_STRING_HANDLE))
        .SetReturn(TEST_MAP_HANDLE_NULL);

    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_STRING_HANDLE));

    // act
    IOTHUB_SERVICE_CLIENT_AUTH_HANDLE result = IoTHubServiceClientAuth_CreateFromConnectionString(TEST_CHAR_PTR);

    // assert
    ASSERT_ARE_EQUAL(void_ptr, NULL, result);
    mocks.AssertActualAndExpectedCalls();
}

/* Tests_SRS_IOTHUBSERVICECLIENT_12_004: [** IoTHubServiceClientAuth_CreateFromConnectionString shall populate hostName, iotHubName, iotHubSuffix, sharedAccessKeyName, sharedAccessKeyValue from the given connection string by calling connectionstringparser_parse **] */
/* Tests_SRS_IOTHUBSERVICECLIENT_12_011: [** If the populating HostName fails, IoTHubServiceClientAuth_CreateFromConnectionString shall do clean up and return NULL. **] */
TEST_FUNCTION(IoTHubServiceClientAuth_CreateFromConnectionString_do_clean_up_if_hostname_fails)
{
    // arrange
    CIoTHubServiceClientAuthMocks mocks;

    whenShallmalloc_fail = 0;
    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_CHAR_PTR));

    STRICT_EXPECTED_CALL(mocks, connectionstringparser_parse(TEST_STRING_HANDLE))
        .SetReturn(TEST_MAP_HANDLE);

    STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(TEST_MAP_HANDLE, (const char*)"HostName"))
        .SetReturn(TEST_CONST_CHAR_PTR_NULL);

    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_STRING_HANDLE_NULL));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_STRING_HANDLE_NULL));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_STRING_HANDLE_NULL));
    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_destroy(TEST_STRING_TOKENIZER_HANDLE_NULL));
    STRICT_EXPECTED_CALL(mocks, Map_Destroy(TEST_MAP_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_STRING_HANDLE));

    // act
    IOTHUB_SERVICE_CLIENT_AUTH_HANDLE result = IoTHubServiceClientAuth_CreateFromConnectionString(TEST_CHAR_PTR);

    // assert
    ASSERT_ARE_EQUAL(void_ptr, NULL, result);
    mocks.AssertActualAndExpectedCalls();
}

/* Tests_SRS_IOTHUBSERVICECLIENT_12_004: [** IoTHubServiceClientAuth_CreateFromConnectionString shall populate hostName, iotHubName, iotHubSuffix, sharedAccessKeyName, sharedAccessKeyValue from the given connection string by calling connectionstringparser_parse **] */
/* Tests_SRS_IOTHUBSERVICECLIENT_12_012: [** If the populating SharedAccessKeyName fails, IoTHubServiceClientAuth_CreateFromConnectionString shall do clean up and return NULL. **] */
TEST_FUNCTION(IoTHubServiceClientAuth_CreateFromConnectionString_do_clean_up_if_sharedaccesskeyname_fails)
{
    // arrange
    CIoTHubServiceClientAuthMocks mocks;

    whenShallmalloc_fail = 0;
    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_CHAR_PTR));

    STRICT_EXPECTED_CALL(mocks, connectionstringparser_parse(TEST_STRING_HANDLE))
        .SetReturn(TEST_MAP_HANDLE);

    STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(TEST_MAP_HANDLE, (const char*)"HostName"))
        .SetReturn(TEST_CONST_CHAR_PTR);

    STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(TEST_MAP_HANDLE, (const char*)"SharedAccessKeyName"))
        .SetReturn(TEST_CONST_CHAR_PTR_NULL);

    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
    .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_STRING_HANDLE_NULL));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_STRING_HANDLE_NULL));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_STRING_HANDLE_NULL));
    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_destroy(TEST_STRING_TOKENIZER_HANDLE_NULL));
    STRICT_EXPECTED_CALL(mocks, Map_Destroy(TEST_MAP_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_STRING_HANDLE));

    // act
    IOTHUB_SERVICE_CLIENT_AUTH_HANDLE result = IoTHubServiceClientAuth_CreateFromConnectionString(TEST_CHAR_PTR);

    // assert
    ASSERT_ARE_EQUAL(void_ptr, NULL, result);
    mocks.AssertActualAndExpectedCalls();
}

/* Tests_SRS_IOTHUBSERVICECLIENT_12_004: [** IoTHubServiceClientAuth_CreateFromConnectionString shall populate hostName, iotHubName, iotHubSuffix, sharedAccessKeyName, sharedAccessKeyValue from the given connection string by calling connectionstringparser_parse **] */
/* Tests_SRS_IOTHUBSERVICECLIENT_12_013: [** If the populating SharedAccessKey fails, IoTHubServiceClientAuth_CreateFromConnectionString shall do clean up and return NULL. **] */
TEST_FUNCTION(IoTHubServiceClientAuth_CreateFromConnectionString_do_clean_up_if_sharedaccesskey_fails)
{
    // arrange
    CIoTHubServiceClientAuthMocks mocks;

    whenShallmalloc_fail = 0;
    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_CHAR_PTR));

    STRICT_EXPECTED_CALL(mocks, connectionstringparser_parse(TEST_STRING_HANDLE))
        .SetReturn(TEST_MAP_HANDLE);

    STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(TEST_MAP_HANDLE, (const char*)"HostName"))
        .SetReturn(TEST_CONST_CHAR_PTR);

    STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(TEST_MAP_HANDLE, (const char*)"SharedAccessKeyName"))
        .SetReturn(TEST_CONST_CHAR_PTR);

    STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(TEST_MAP_HANDLE, (const char*)"SharedAccessKey"))
        .SetReturn(TEST_CONST_CHAR_PTR_NULL);

    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_STRING_HANDLE_NULL));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_STRING_HANDLE_NULL));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_STRING_HANDLE_NULL));
    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_destroy(TEST_STRING_TOKENIZER_HANDLE_NULL));
    STRICT_EXPECTED_CALL(mocks, Map_Destroy(TEST_MAP_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_STRING_HANDLE));

    // act
    IOTHUB_SERVICE_CLIENT_AUTH_HANDLE result = IoTHubServiceClientAuth_CreateFromConnectionString(TEST_CHAR_PTR);

    // assert
    ASSERT_ARE_EQUAL(void_ptr, NULL, result);
    mocks.AssertActualAndExpectedCalls();
}

/* Tests_SRS_IOTHUBSERVICECLIENT_12_038: [** IoTHubServiceClientAuth_CreateFromConnectionString shall create a STRING_handle from hostName by calling STRING_construct. **] */
/* Tests_SRS_IOTHUBSERVICECLIENT_12_039: [** If the STRING_construct fails, IoTHubServiceClientAuth_CreateFromConnectionString shall do clean up and return NULL. **] */
TEST_FUNCTION(IoTHubServiceClientAuth_CreateFromConnectionString_do_clean_up_if_STRING_construct_for_hostname_fails)
{
    // arrange
    CIoTHubServiceClientAuthMocks mocks;

    whenShallmalloc_fail = 0;
    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_CHAR_PTR));

    STRICT_EXPECTED_CALL(mocks, connectionstringparser_parse(TEST_STRING_HANDLE))
        .SetReturn(TEST_MAP_HANDLE);

    STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(TEST_MAP_HANDLE, (const char*)"HostName"))
        .SetReturn(TEST_CONST_CHAR_PTR);

    STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(TEST_MAP_HANDLE, (const char*)"SharedAccessKeyName"))
        .SetReturn(TEST_CONST_CHAR_PTR);

    STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(TEST_MAP_HANDLE, (const char*)"SharedAccessKey"))
        .SetReturn(TEST_CONST_CHAR_PTR);

    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_CONST_CHAR_PTR))
        .SetReturn(TEST_STRING_HANDLE_NULL);

    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_STRING_HANDLE_NULL));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_STRING_HANDLE_NULL));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_STRING_HANDLE_NULL));
    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_destroy(TEST_STRING_TOKENIZER_HANDLE_NULL));
    STRICT_EXPECTED_CALL(mocks, Map_Destroy(TEST_MAP_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_STRING_HANDLE));

    // act
    IOTHUB_SERVICE_CLIENT_AUTH_HANDLE result = IoTHubServiceClientAuth_CreateFromConnectionString(TEST_CHAR_PTR);

    // assert
    ASSERT_ARE_EQUAL(void_ptr, NULL, result);
    mocks.AssertActualAndExpectedCalls();
}

/* Tests_SRS_IOTHUBSERVICECLIENT_12_014: [** IoTHubServiceClientAuth_CreateFromConnectionString shall create a STRING_TOKENIZER to parse HostName by calling STRING_TOKENIZER_create. **] */
/* Tests_SRS_IOTHUBSERVICECLIENT_12_015: [** If the STRING_TOKENIZER_create fails, IoTHubServiceClientAuth_CreateFromConnectionString shall do clean up and return NULL. **] */
TEST_FUNCTION(IoTHubServiceClientAuth_CreateFromConnectionString_do_clean_up_if_STRING_TOKENIZER_create_fails)
{
    // arrange
    CIoTHubServiceClientAuthMocks mocks;

    whenShallmalloc_fail = 0;
    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_CHAR_PTR));

    STRICT_EXPECTED_CALL(mocks, connectionstringparser_parse(TEST_STRING_HANDLE))
        .SetReturn(TEST_MAP_HANDLE);

    STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(TEST_MAP_HANDLE, (const char*)"HostName"))
        .SetReturn(TEST_CONST_CHAR_PTR);

    STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(TEST_MAP_HANDLE, (const char*)"SharedAccessKeyName"))
        .SetReturn(TEST_CONST_CHAR_PTR);

    STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(TEST_MAP_HANDLE, (const char*)"SharedAccessKey"))
        .SetReturn(TEST_CONST_CHAR_PTR);

    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_CONST_CHAR_PTR));

    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_create(TEST_STRING_HANDLE))
        .SetReturn(TEST_STRING_TOKENIZER_HANDLE_NULL);

    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    
    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_STRING_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_STRING_HANDLE_NULL));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_STRING_HANDLE_NULL));
    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_destroy(TEST_STRING_TOKENIZER_HANDLE_NULL));
    STRICT_EXPECTED_CALL(mocks, Map_Destroy(TEST_MAP_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_STRING_HANDLE));

    // act
    IOTHUB_SERVICE_CLIENT_AUTH_HANDLE result = IoTHubServiceClientAuth_CreateFromConnectionString(TEST_CHAR_PTR);

    // assert
    ASSERT_ARE_EQUAL(void_ptr, NULL, result);
    mocks.AssertActualAndExpectedCalls();
}

/* Tests_SRS_IOTHUBSERVICECLIENT_12_016: [** IoTHubServiceClientAuth_CreateFromConnectionString shall create a new STRING_HANDLE for token key string by calling STRING_new. **] */
/* Tests_SRS_IOTHUBSERVICECLIENT_12_017: [** If the STRING_new fails, IoTHubServiceClientAuth_CreateFromConnectionString shall do clean up and return NULL. **] */
TEST_FUNCTION(IoTHubServiceClientAuth_CreateFromConnectionString_do_clean_up_if_STRING_new_for_key_fails)
{
    // arrange
    CIoTHubServiceClientAuthMocks mocks;

    whenShallmalloc_fail = 0;
    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_CHAR_PTR));

    STRICT_EXPECTED_CALL(mocks, connectionstringparser_parse(TEST_STRING_HANDLE))
        .SetReturn(TEST_MAP_HANDLE);

    STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(TEST_MAP_HANDLE, (const char*)"HostName"))
        .SetReturn(TEST_CONST_CHAR_PTR);

    STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(TEST_MAP_HANDLE, (const char*)"SharedAccessKeyName"))
        .SetReturn(TEST_CONST_CHAR_PTR);

    STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(TEST_MAP_HANDLE, (const char*)"SharedAccessKey"))
        .SetReturn(TEST_CONST_CHAR_PTR);

    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_CONST_CHAR_PTR));

    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_create(TEST_STRING_HANDLE))
        .SetReturn(TEST_STRING_TOKENIZER_HANDLE);

    STRICT_EXPECTED_CALL(mocks, STRING_new())
        .SetReturn(TEST_STRING_HANDLE_NULL);

    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_STRING_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_STRING_HANDLE_NULL));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_STRING_HANDLE_NULL));
    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_destroy(TEST_STRING_TOKENIZER_HANDLE));
    STRICT_EXPECTED_CALL(mocks, Map_Destroy(TEST_MAP_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_STRING_HANDLE));

    // act
    IOTHUB_SERVICE_CLIENT_AUTH_HANDLE result = IoTHubServiceClientAuth_CreateFromConnectionString(TEST_CHAR_PTR);

    // assert
    ASSERT_ARE_EQUAL(void_ptr, NULL, result);
    mocks.AssertActualAndExpectedCalls();
}

/* Tests_SRS_IOTHUBSERVICECLIENT_12_018: [** IoTHubServiceClientAuth_CreateFromConnectionString shall create a new STRING_HANDLE for token value string by calling STRING_new. **] */
/* Tests_SRS_IOTHUBSERVICECLIENT_12_019: [** If the STRING_new fails, IoTHubServiceClientAuth_CreateFromConnectionString shall do clean up and return NULL. **] */
TEST_FUNCTION(IoTHubServiceClientAuth_CreateFromConnectionString_do_clean_up_if_STRING_new_for_value_fails)
{
    // arrange
    CIoTHubServiceClientAuthMocks mocks;

    whenShallmalloc_fail = 0;
    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_CHAR_PTR));

    STRICT_EXPECTED_CALL(mocks, connectionstringparser_parse(TEST_STRING_HANDLE))
        .SetReturn(TEST_MAP_HANDLE);

    STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(TEST_MAP_HANDLE, (const char*)"HostName"))
        .SetReturn(TEST_CONST_CHAR_PTR);

    STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(TEST_MAP_HANDLE, (const char*)"SharedAccessKeyName"))
        .SetReturn(TEST_CONST_CHAR_PTR);

    STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(TEST_MAP_HANDLE, (const char*)"SharedAccessKey"))
        .SetReturn(TEST_CONST_CHAR_PTR);
    
    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_CONST_CHAR_PTR));

    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_create(TEST_STRING_HANDLE))
        .SetReturn(TEST_STRING_TOKENIZER_HANDLE);

    STRICT_EXPECTED_CALL(mocks, STRING_new())
        .SetReturn(TEST_STRING_HANDLE);

    STRICT_EXPECTED_CALL(mocks, STRING_new())
        .SetReturn(TEST_STRING_HANDLE_NULL);

    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_STRING_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_STRING_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_STRING_HANDLE_NULL));
    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_destroy(TEST_STRING_TOKENIZER_HANDLE));
    STRICT_EXPECTED_CALL(mocks, Map_Destroy(TEST_MAP_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_STRING_HANDLE));

    // act
    IOTHUB_SERVICE_CLIENT_AUTH_HANDLE result = IoTHubServiceClientAuth_CreateFromConnectionString(TEST_CHAR_PTR);

    // assert
    ASSERT_ARE_EQUAL(void_ptr, NULL, result);
    mocks.AssertActualAndExpectedCalls();
}

/* Tests_SRS_IOTHUBSERVICECLIENT_12_020: [** IoTHubServiceClientAuth_CreateFromConnectionString shall call STRING_TOKENIZER_get_next_token to get token key string. **] */
/* Tests_SRS_IOTHUBSERVICECLIENT_12_021: [** If the STRING_TOKENIZER_get_next_token fails, IoTHubServiceClientAuth_CreateFromConnectionString shall do clean up and return NULL. **] */
TEST_FUNCTION(IoTHubServiceClientAuth_CreateFromConnectionString_do_clean_up_if_STRING_TOKENIZER_get_next_token_for_key_fails)
{
    // arrange
    CIoTHubServiceClientAuthMocks mocks;

    whenShallmalloc_fail = 0;
    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_CHAR_PTR));

    STRICT_EXPECTED_CALL(mocks, connectionstringparser_parse(TEST_STRING_HANDLE))
        .SetReturn(TEST_MAP_HANDLE);

    STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(TEST_MAP_HANDLE, (const char*)"HostName"))
        .SetReturn(TEST_CONST_CHAR_PTR);

    STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(TEST_MAP_HANDLE, (const char*)"SharedAccessKeyName"))
        .SetReturn(TEST_CONST_CHAR_PTR);

    STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(TEST_MAP_HANDLE, (const char*)"SharedAccessKey"))
        .SetReturn(TEST_CONST_CHAR_PTR);

    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_CONST_CHAR_PTR));

    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_create(TEST_STRING_HANDLE))
        .SetReturn(TEST_STRING_TOKENIZER_HANDLE);

    STRICT_EXPECTED_CALL(mocks, STRING_new())
        .SetReturn(TEST_STRING_HANDLE);

    STRICT_EXPECTED_CALL(mocks, STRING_new())
        .SetReturn(TEST_STRING_HANDLE);

    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, "."))
        .SetReturn(1);

    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_STRING_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_STRING_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_STRING_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_destroy(TEST_STRING_TOKENIZER_HANDLE));
    STRICT_EXPECTED_CALL(mocks, Map_Destroy(TEST_MAP_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_STRING_HANDLE));

    // act
    IOTHUB_SERVICE_CLIENT_AUTH_HANDLE result = IoTHubServiceClientAuth_CreateFromConnectionString(TEST_CHAR_PTR);

    // assert
    ASSERT_ARE_EQUAL(void_ptr, NULL, result);
    mocks.AssertActualAndExpectedCalls();
}

/* Tests_SRS_IOTHUBSERVICECLIENT_12_022: [** IoTHubServiceClientAuth_CreateFromConnectionString shall call STRING_TOKENIZER_get_next_token to get token value string. **] */
/* Tests_SRS_IOTHUBSERVICECLIENT_12_023: [** If the STRING_TOKENIZER_get_next_token fails, IoTHubServiceClientAuth_CreateFromConnectionString shall do clean up and return NULL. **] */
TEST_FUNCTION(IoTHubServiceClientAuth_CreateFromConnectionString_do_clean_up_if_STRING_TOKENIZER_get_next_token_for_value_fails)
{
    // arrange
    CIoTHubServiceClientAuthMocks mocks;

    whenShallmalloc_fail = 0;
    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_CHAR_PTR));

    STRICT_EXPECTED_CALL(mocks, connectionstringparser_parse(TEST_STRING_HANDLE))
        .SetReturn(TEST_MAP_HANDLE);

    STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(TEST_MAP_HANDLE, (const char*)"HostName"))
        .SetReturn(TEST_CONST_CHAR_PTR);

    STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(TEST_MAP_HANDLE, (const char*)"SharedAccessKeyName"))
        .SetReturn(TEST_CONST_CHAR_PTR);

    STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(TEST_MAP_HANDLE, (const char*)"SharedAccessKey"))
        .SetReturn(TEST_CONST_CHAR_PTR);

    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_CONST_CHAR_PTR));

    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_create(TEST_STRING_HANDLE))
        .SetReturn(TEST_STRING_TOKENIZER_HANDLE);

    STRICT_EXPECTED_CALL(mocks, STRING_new())
        .SetReturn(TEST_STRING_HANDLE);

    STRICT_EXPECTED_CALL(mocks, STRING_new())
        .SetReturn(TEST_STRING_HANDLE);

    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, "."))
        .SetReturn(0);

    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, "0"))
        .SetReturn(1);

    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_STRING_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_STRING_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_STRING_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_destroy(TEST_STRING_TOKENIZER_HANDLE));
    STRICT_EXPECTED_CALL(mocks, Map_Destroy(TEST_MAP_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_STRING_HANDLE));

    // act
    IOTHUB_SERVICE_CLIENT_AUTH_HANDLE result = IoTHubServiceClientAuth_CreateFromConnectionString(TEST_CHAR_PTR);

    // assert
    ASSERT_ARE_EQUAL(void_ptr, NULL, result);
    mocks.AssertActualAndExpectedCalls();
}

/* Tests_SRS_IOTHUBSERVICECLIENT_12_024: [** IoTHubServiceClientAuth_CreateFromConnectionString shall allocate memory and copy hostName to result->hostName by calling mallocAndStrcpy_s. **] */
/* Tests_SRS_IOTHUBSERVICECLIENT_12_025: [** If the mallocAndStrcpy_s fails, IoTHubServiceClientAuth_CreateFromConnectionString shall do clean up and return NULL. **] */
TEST_FUNCTION(IoTHubServiceClientAuth_CreateFromConnectionString_do_clean_up_if_mallocAndStrcpy_s_for_hostName_fails)
{
    // arrange
    CIoTHubServiceClientAuthMocks mocks;

    whenShallmalloc_fail = 0;
    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_CHAR_PTR));

    STRICT_EXPECTED_CALL(mocks, connectionstringparser_parse(TEST_STRING_HANDLE))
        .SetReturn(TEST_MAP_HANDLE);

    STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(TEST_MAP_HANDLE, (const char*)"HostName"))
        .SetReturn(TEST_CONST_CHAR_PTR);

    STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(TEST_MAP_HANDLE, (const char*)"SharedAccessKeyName"))
        .SetReturn(TEST_CONST_CHAR_PTR);

    STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(TEST_MAP_HANDLE, (const char*)"SharedAccessKey"))
        .SetReturn(TEST_CONST_CHAR_PTR);

    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_CONST_CHAR_PTR));

    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_create(TEST_STRING_HANDLE))
        .SetReturn(TEST_STRING_TOKENIZER_HANDLE);

    STRICT_EXPECTED_CALL(mocks, STRING_new())
        .SetReturn(TEST_STRING_HANDLE);

    STRICT_EXPECTED_CALL(mocks, STRING_new())
        .SetReturn(TEST_STRING_HANDLE);

    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, "."))
        .SetReturn(0);

    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, "0"))
        .SetReturn(0);

    EXPECTED_CALL(mocks, mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .SetReturn(1);

    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_STRING_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_STRING_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_STRING_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_destroy(TEST_STRING_TOKENIZER_HANDLE));
    STRICT_EXPECTED_CALL(mocks, Map_Destroy(TEST_MAP_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_STRING_HANDLE));

    // act
    IOTHUB_SERVICE_CLIENT_AUTH_HANDLE result = IoTHubServiceClientAuth_CreateFromConnectionString(TEST_CHAR_PTR);

    // assert
    ASSERT_ARE_EQUAL(void_ptr, NULL, result);
    mocks.AssertActualAndExpectedCalls();
}

/* Tests_SRS_IOTHUBSERVICECLIENT_12_026: [** IoTHubServiceClientAuth_CreateFromConnectionString shall allocate memory and copy keyName to result->keyName by calling mallocAndStrcpy_s. **] */
/* Tests_SRS_IOTHUBSERVICECLIENT_12_027: [** If the mallocAndStrcpy_s fails, IoTHubServiceClientAuth_CreateFromConnectionString shall do clean up and return NULL. **] */
TEST_FUNCTION(IoTHubServiceClientAuth_CreateFromConnectionString_do_clean_up_if_mallocAndStrcpy_s_for_keyName_fails)
{
    // arrange
    CIoTHubServiceClientAuthMocks mocks;

    whenShallmalloc_fail = 0;
    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_CHAR_PTR));

    STRICT_EXPECTED_CALL(mocks, connectionstringparser_parse(TEST_STRING_HANDLE))
        .SetReturn(TEST_MAP_HANDLE);

    STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(TEST_MAP_HANDLE, (const char*)"HostName"))
        .SetReturn(TEST_CONST_CHAR_PTR);

    STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(TEST_MAP_HANDLE, (const char*)"SharedAccessKeyName"))
        .SetReturn(TEST_CONST_CHAR_PTR);

    STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(TEST_MAP_HANDLE, (const char*)"SharedAccessKey"))
        .SetReturn(TEST_CONST_CHAR_PTR);

    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_CONST_CHAR_PTR));

    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_create(TEST_STRING_HANDLE))
        .SetReturn(TEST_STRING_TOKENIZER_HANDLE);

    STRICT_EXPECTED_CALL(mocks, STRING_new())
        .SetReturn(TEST_STRING_HANDLE);

    STRICT_EXPECTED_CALL(mocks, STRING_new())
        .SetReturn(TEST_STRING_HANDLE);

    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, "."))
        .SetReturn(0);

    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, "0"))
        .SetReturn(0);

    EXPECTED_CALL(mocks, mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .SetReturn(0);

    EXPECTED_CALL(mocks, mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .SetReturn(1);

    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_STRING_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_STRING_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_STRING_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_destroy(TEST_STRING_TOKENIZER_HANDLE));
    STRICT_EXPECTED_CALL(mocks, Map_Destroy(TEST_MAP_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_STRING_HANDLE));

    // act
    IOTHUB_SERVICE_CLIENT_AUTH_HANDLE result = IoTHubServiceClientAuth_CreateFromConnectionString(TEST_CHAR_PTR);

    // assert
    ASSERT_ARE_EQUAL(void_ptr, NULL, result);
    mocks.AssertActualAndExpectedCalls();
}

/* Tests_SRS_IOTHUBSERVICECLIENT_12_028: [** IoTHubServiceClientAuth_CreateFromConnectionString shall allocate memory and copy sharedAccessKey to result->sharedAccessKey by calling mallocAndStrcpy_s. **] */
/* Tests_SRS_IOTHUBSERVICECLIENT_12_029: [** If the mallocAndStrcpy_s fails, IoTHubServiceClientAuth_CreateFromConnectionString shall do clean up and return NULL. **] */
TEST_FUNCTION(IoTHubServiceClientAuth_CreateFromConnectionString_do_clean_up_if_mallocAndStrcpy_s_for_sharedAccessKey_fails)
{
    // arrange
    CIoTHubServiceClientAuthMocks mocks;

    whenShallmalloc_fail = 0;
    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_CHAR_PTR));

    STRICT_EXPECTED_CALL(mocks, connectionstringparser_parse(TEST_STRING_HANDLE))
        .SetReturn(TEST_MAP_HANDLE);

    STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(TEST_MAP_HANDLE, (const char*)"HostName"))
        .SetReturn(TEST_CONST_CHAR_PTR);

    STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(TEST_MAP_HANDLE, (const char*)"SharedAccessKeyName"))
        .SetReturn(TEST_CONST_CHAR_PTR);

    STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(TEST_MAP_HANDLE, (const char*)"SharedAccessKey"))
        .SetReturn(TEST_CONST_CHAR_PTR);

    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_CONST_CHAR_PTR));

    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_create(TEST_STRING_HANDLE))
        .SetReturn(TEST_STRING_TOKENIZER_HANDLE);

    STRICT_EXPECTED_CALL(mocks, STRING_new())
        .SetReturn(TEST_STRING_HANDLE);

    STRICT_EXPECTED_CALL(mocks, STRING_new())
        .SetReturn(TEST_STRING_HANDLE);

    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, "."))
        .SetReturn(0);

    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, "0"))
        .SetReturn(0);

    EXPECTED_CALL(mocks, mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .SetReturn(0);

    EXPECTED_CALL(mocks, mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .SetReturn(0);

    EXPECTED_CALL(mocks, mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .SetReturn(1);

    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_STRING_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_STRING_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_STRING_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_destroy(TEST_STRING_TOKENIZER_HANDLE));
    STRICT_EXPECTED_CALL(mocks, Map_Destroy(TEST_MAP_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_STRING_HANDLE));

    // act
    IOTHUB_SERVICE_CLIENT_AUTH_HANDLE result = IoTHubServiceClientAuth_CreateFromConnectionString(TEST_CHAR_PTR);

    // assert
    ASSERT_ARE_EQUAL(void_ptr, NULL, result);
    mocks.AssertActualAndExpectedCalls();
}

/* Tests_SRS_IOTHUBSERVICECLIENT_12_030: [** IoTHubServiceClientAuth_CreateFromConnectionString shall allocate memory and copy iothubName to result->iothubName by calling mallocAndStrcpy_s. **] */
/* Tests_SRS_IOTHUBSERVICECLIENT_12_031 : [** If the mallocAndStrcpy_s fails, IoTHubServiceClientAuth_CreateFromConnectionString shall do clean up and return NULL. **] */
TEST_FUNCTION(IoTHubServiceClientAuth_CreateFromConnectionString_do_clean_up_if_mallocAndStrcpy_s_for_iothubName_fails)
{
    // arrange
    CIoTHubServiceClientAuthMocks mocks;

    whenShallmalloc_fail = 0;
    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_CHAR_PTR));

    STRICT_EXPECTED_CALL(mocks, connectionstringparser_parse(TEST_STRING_HANDLE))
        .SetReturn(TEST_MAP_HANDLE);

    STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(TEST_MAP_HANDLE, (const char*)"HostName"))
        .SetReturn(TEST_CONST_CHAR_PTR);

    STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(TEST_MAP_HANDLE, (const char*)"SharedAccessKeyName"))
        .SetReturn(TEST_CONST_CHAR_PTR);

    STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(TEST_MAP_HANDLE, (const char*)"SharedAccessKey"))
        .SetReturn(TEST_CONST_CHAR_PTR);

    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_CONST_CHAR_PTR));

    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_create(TEST_STRING_HANDLE))
        .SetReturn(TEST_STRING_TOKENIZER_HANDLE);

    STRICT_EXPECTED_CALL(mocks, STRING_new())
        .SetReturn(TEST_STRING_HANDLE);

    STRICT_EXPECTED_CALL(mocks, STRING_new())
        .SetReturn(TEST_STRING_HANDLE);

    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, "."))
        .SetReturn(0);

    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, "0"))
        .SetReturn(0);

    EXPECTED_CALL(mocks, mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .SetReturn(0);

    EXPECTED_CALL(mocks, mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .SetReturn(0);

    EXPECTED_CALL(mocks, mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .SetReturn(0);

    EXPECTED_CALL(mocks, STRING_c_str(TEST_STRING_HANDLE))
        .SetReturn(TEST_CHAR_PTR);

    EXPECTED_CALL(mocks, STRING_c_str(TEST_STRING_HANDLE))
        .SetReturn(TEST_CHAR_PTR);

    EXPECTED_CALL(mocks, mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .SetReturn(1);

    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_STRING_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_STRING_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_STRING_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_destroy(TEST_STRING_TOKENIZER_HANDLE));
    STRICT_EXPECTED_CALL(mocks, Map_Destroy(TEST_MAP_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_STRING_HANDLE));

    // act
    IOTHUB_SERVICE_CLIENT_AUTH_HANDLE result = IoTHubServiceClientAuth_CreateFromConnectionString(TEST_CHAR_PTR);

    // assert
    ASSERT_ARE_EQUAL(void_ptr, NULL, result);
    mocks.AssertActualAndExpectedCalls();
}

/* Tests_SRS_IOTHUBSERVICECLIENT_12_032: [** IoTHubServiceClientAuth_CreateFromConnectionString shall allocate memory and copy iothubSuffix to result->iothubSuffix by calling mallocAndStrcpy_s. **] */
/* Tests_SRS_IOTHUBSERVICECLIENT_12_033 : [** If the mallocAndStrcpy_s fails, IoTHubServiceClientAuth_CreateFromConnectionString shall do clean up and return NULL. **] */
TEST_FUNCTION(IoTHubServiceClientAuth_CreateFromConnectionString_do_clean_up_if_mallocAndStrcpy_s_for_iothubSuffix_fails)
{
    // arrange
    CIoTHubServiceClientAuthMocks mocks;

    whenShallmalloc_fail = 0;
    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_CHAR_PTR));

    STRICT_EXPECTED_CALL(mocks, connectionstringparser_parse(TEST_STRING_HANDLE))
        .SetReturn(TEST_MAP_HANDLE);

    STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(TEST_MAP_HANDLE, (const char*)"HostName"))
        .SetReturn(TEST_CONST_CHAR_PTR);

    STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(TEST_MAP_HANDLE, (const char*)"SharedAccessKeyName"))
        .SetReturn(TEST_CONST_CHAR_PTR);

    STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(TEST_MAP_HANDLE, (const char*)"SharedAccessKey"))
        .SetReturn(TEST_CONST_CHAR_PTR);

    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_CONST_CHAR_PTR));

    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_create(TEST_STRING_HANDLE))
        .SetReturn(TEST_STRING_TOKENIZER_HANDLE);

    STRICT_EXPECTED_CALL(mocks, STRING_new())
        .SetReturn(TEST_STRING_HANDLE);

    STRICT_EXPECTED_CALL(mocks, STRING_new())
        .SetReturn(TEST_STRING_HANDLE);

    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, "."))
        .SetReturn(0);

    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, "0"))
        .SetReturn(0);

    EXPECTED_CALL(mocks, mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .SetReturn(0);

    EXPECTED_CALL(mocks, mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .SetReturn(0);

    EXPECTED_CALL(mocks, mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .SetReturn(0);

    EXPECTED_CALL(mocks, STRING_c_str(TEST_STRING_HANDLE))
        .SetReturn(TEST_CHAR_PTR);

    EXPECTED_CALL(mocks, STRING_c_str(TEST_STRING_HANDLE))
        .SetReturn(TEST_CHAR_PTR);

    EXPECTED_CALL(mocks, mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .SetReturn(0);

    EXPECTED_CALL(mocks, mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .SetReturn(1);

    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_STRING_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_STRING_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_STRING_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_destroy(TEST_STRING_TOKENIZER_HANDLE));
    STRICT_EXPECTED_CALL(mocks, Map_Destroy(TEST_MAP_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_STRING_HANDLE));

    // act
    IOTHUB_SERVICE_CLIENT_AUTH_HANDLE result = IoTHubServiceClientAuth_CreateFromConnectionString(TEST_CHAR_PTR);

    // assert
    ASSERT_ARE_EQUAL(void_ptr, NULL, result);
    mocks.AssertActualAndExpectedCalls();
}

/* Tests_SRS_IOTHUBSERVICECLIENT_12_034: [** IoTHubServiceClientAuth_CreateFromConnectionString shall create C string from token key string handle by calling STRING_c_str. **] */
/* Tests_SRS_IOTHUBSERVICECLIENT_12_035 : [** If the STRING_c_str fails, IoTHubServiceClientAuth_CreateFromConnectionString shall do clean up and return NULL. **] */
TEST_FUNCTION(IoTHubServiceClientAuth_CreateFromConnectionString_do_clean_up_if_STRING_c_str_for_iothubName_fails)
{
    // arrange
    CIoTHubServiceClientAuthMocks mocks;

    whenShallmalloc_fail = 0;
    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_CHAR_PTR));

    STRICT_EXPECTED_CALL(mocks, connectionstringparser_parse(TEST_STRING_HANDLE))
        .SetReturn(TEST_MAP_HANDLE);

    STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(TEST_MAP_HANDLE, (const char*)"HostName"))
        .SetReturn(TEST_CONST_CHAR_PTR);

    STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(TEST_MAP_HANDLE, (const char*)"SharedAccessKeyName"))
        .SetReturn(TEST_CONST_CHAR_PTR);

    STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(TEST_MAP_HANDLE, (const char*)"SharedAccessKey"))
        .SetReturn(TEST_CONST_CHAR_PTR);

    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_CONST_CHAR_PTR));

    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_create(TEST_STRING_HANDLE))
        .SetReturn(TEST_STRING_TOKENIZER_HANDLE);

    STRICT_EXPECTED_CALL(mocks, STRING_new())
        .SetReturn(TEST_STRING_HANDLE);

    STRICT_EXPECTED_CALL(mocks, STRING_new())
        .SetReturn(TEST_STRING_HANDLE);

    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, "."))
        .SetReturn(0);

    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, "0"))
        .SetReturn(0);

    EXPECTED_CALL(mocks, mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .SetReturn(0);

    EXPECTED_CALL(mocks, mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .SetReturn(0);

    EXPECTED_CALL(mocks, mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .SetReturn(0);

    EXPECTED_CALL(mocks, STRING_c_str(TEST_STRING_HANDLE))
        .SetReturn(TEST_CONST_CHAR_PTR_NULL);

    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_STRING_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_STRING_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_STRING_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_destroy(TEST_STRING_TOKENIZER_HANDLE));
    STRICT_EXPECTED_CALL(mocks, Map_Destroy(TEST_MAP_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_STRING_HANDLE));

    // act
    IOTHUB_SERVICE_CLIENT_AUTH_HANDLE result = IoTHubServiceClientAuth_CreateFromConnectionString(TEST_CHAR_PTR);

    // assert
    ASSERT_ARE_EQUAL(void_ptr, NULL, result);
    mocks.AssertActualAndExpectedCalls();
}

/* Tests_SRS_IOTHUBSERVICECLIENT_12_036 : [** IoTHubServiceClientAuth_CreateFromConnectionString shall create C string from token value string handle by calling STRING_c_str. **] */
/* Tests_SRS_IOTHUBSERVICECLIENT_12_037 : [** If the mallocAndStrcpy_s fails, IoTHubServiceClientAuth_CreateFromConnectionString shall do clean up and return NULL. **] */
TEST_FUNCTION(IoTHubServiceClientAuth_CreateFromConnectionString_do_clean_up_if_STRING_c_str_for_iothubSuffix_fails)
{
    // arrange
    CIoTHubServiceClientAuthMocks mocks;

    whenShallmalloc_fail = 0;
    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_CHAR_PTR));

    STRICT_EXPECTED_CALL(mocks, connectionstringparser_parse(TEST_STRING_HANDLE))
        .SetReturn(TEST_MAP_HANDLE);

    STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(TEST_MAP_HANDLE, (const char*)"HostName"))
        .SetReturn(TEST_CONST_CHAR_PTR);

    STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(TEST_MAP_HANDLE, (const char*)"SharedAccessKeyName"))
        .SetReturn(TEST_CONST_CHAR_PTR);

    STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(TEST_MAP_HANDLE, (const char*)"SharedAccessKey"))
        .SetReturn(TEST_CONST_CHAR_PTR);

    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_CONST_CHAR_PTR));

    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_create(TEST_STRING_HANDLE))
        .SetReturn(TEST_STRING_TOKENIZER_HANDLE);

    STRICT_EXPECTED_CALL(mocks, STRING_new())
        .SetReturn(TEST_STRING_HANDLE);

    STRICT_EXPECTED_CALL(mocks, STRING_new())
        .SetReturn(TEST_STRING_HANDLE);

    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, "."))
        .SetReturn(0);

    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, "0"))
        .SetReturn(0);

    EXPECTED_CALL(mocks, mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .SetReturn(0);

    EXPECTED_CALL(mocks, mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .SetReturn(0);

    EXPECTED_CALL(mocks, mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .SetReturn(0);

    EXPECTED_CALL(mocks, STRING_c_str(TEST_STRING_HANDLE))
        .SetReturn(TEST_CONST_CHAR_PTR);

    EXPECTED_CALL(mocks, STRING_c_str(TEST_STRING_HANDLE))
        .SetReturn(TEST_CONST_CHAR_PTR_NULL);

    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_STRING_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_STRING_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_STRING_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_destroy(TEST_STRING_TOKENIZER_HANDLE));
    STRICT_EXPECTED_CALL(mocks, Map_Destroy(TEST_MAP_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_STRING_HANDLE));

    // act
    IOTHUB_SERVICE_CLIENT_AUTH_HANDLE result = IoTHubServiceClientAuth_CreateFromConnectionString(TEST_CHAR_PTR);

    // assert
    ASSERT_ARE_EQUAL(void_ptr, NULL, result);

    mocks.AssertActualAndExpectedCalls();
}

/* Tests_SRS_IOTHUBSERVICECLIENT_12_006: [** If the IOTHUB_SERVICE_CLIENT_AUTH has been populated IoTHubServiceClientAuth_CreateFromConnectionString shall return with a IOTHUB_SERVICE_CLIENT_AUTH_HANDLE to it **]*/
TEST_FUNCTION(IoTHubServiceClientAuth_CreateFromConnectionString_succeed)
{
    // arrange
    CIoTHubServiceClientAuthMocks mocks;
    
    whenShallmalloc_fail = 0;
    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
        .IgnoreArgument(1);
    
    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_CHAR_PTR));
    
    STRICT_EXPECTED_CALL(mocks, connectionstringparser_parse(TEST_STRING_HANDLE))
        .SetReturn(TEST_MAP_HANDLE);
    
    STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(TEST_MAP_HANDLE, (const char*)"HostName"))
        .SetReturn(TEST_CONST_CHAR_PTR);
    
    STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(TEST_MAP_HANDLE, (const char*)"SharedAccessKeyName"))
        .SetReturn(TEST_CONST_CHAR_PTR);
    
    STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(TEST_MAP_HANDLE, (const char*)"SharedAccessKey"))
        .SetReturn(TEST_CONST_CHAR_PTR);
    
    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_CONST_CHAR_PTR));

    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_create(TEST_STRING_HANDLE))
        .SetReturn(TEST_STRING_TOKENIZER_HANDLE);
    
    STRICT_EXPECTED_CALL(mocks, STRING_new())
        .SetReturn(TEST_STRING_HANDLE);
    
    STRICT_EXPECTED_CALL(mocks, STRING_new())
        .SetReturn(TEST_STRING_HANDLE);
    
    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, "."))
        .SetReturn(0);
    
    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, "0"))
        .SetReturn(0);
    
    EXPECTED_CALL(mocks, mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .SetReturn(0);
    
    EXPECTED_CALL(mocks, mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .SetReturn(0);
    
    EXPECTED_CALL(mocks, mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .SetReturn(0);
    
    EXPECTED_CALL(mocks, STRING_c_str(TEST_STRING_HANDLE))
        .SetReturn(TEST_CHAR_PTR);
    
    EXPECTED_CALL(mocks, STRING_c_str(TEST_STRING_HANDLE))
        .SetReturn(TEST_CHAR_PTR);
    
    EXPECTED_CALL(mocks, mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .SetReturn(0);
    
    EXPECTED_CALL(mocks, mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .SetReturn(0);
    
    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_STRING_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_STRING_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_STRING_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_destroy(TEST_STRING_TOKENIZER_HANDLE));
    STRICT_EXPECTED_CALL(mocks, Map_Destroy(TEST_MAP_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_STRING_HANDLE));

    // act
    IOTHUB_SERVICE_CLIENT_AUTH_HANDLE result = IoTHubServiceClientAuth_CreateFromConnectionString(TEST_CHAR_PTR);

    // assert
    ASSERT_IS_NOT_NULL(result);
    if (result != NULL)
    {
        free(result);
    }

    mocks.AssertActualAndExpectedCalls();
}

/* Tests_SRS_IOTHUBSERVICECLIENT_12_007: [** If the serviceClientHandle input parameter is NULL IoTHubServiceClient_Destroy shall return **] */
TEST_FUNCTION(IoTHubServiceClient_Destroy_return_if_input_parameter_serviceClientHandle_is_NULL)
{
    // arrange
    CIoTHubServiceClientAuthMocks mocks;

    // act
    IoTHubServiceClientAuth_Destroy(NULL);

    // assert
    mocks.AssertActualAndExpectedCalls();
}

/* Tests_SRS_IOTHUBSERVICECLIENT_12_008 : [** If the serviceClientHandle input parameter is not NULL IoTHubServiceClient_Destroy shall free the memory of it and return **] */
TEST_FUNCTION(IoTHubServiceClient_Destroy_do_clean_up_and_return_if_input_parameter_serviceClientHandle_is_not_NULL)
{
    // arrange

    CIoTHubServiceClientAuthMocks mocks;

    whenShallmalloc_fail = 0;
    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_construct(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, connectionstringparser_parse(IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .SetReturn(TEST_MAP_HANDLE);

    STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .SetReturn(TEST_CONST_CHAR_PTR);

    STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .SetReturn(TEST_CONST_CHAR_PTR);

    STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .IgnoreArgument(2)
        .SetReturn(TEST_CONST_CHAR_PTR);

    STRICT_EXPECTED_CALL(mocks, STRING_construct(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_create(IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .SetReturn(TEST_STRING_TOKENIZER_HANDLE);

    STRICT_EXPECTED_CALL(mocks, STRING_new())
        .SetReturn(TEST_STRING_HANDLE);

    STRICT_EXPECTED_CALL(mocks, STRING_new())
        .SetReturn(TEST_STRING_HANDLE);

    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(IGNORED_PTR_ARG, IGNORED_PTR_ARG, "."))
        .IgnoreAllArguments()
        .SetReturn(0);

    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(IGNORED_PTR_ARG, IGNORED_PTR_ARG, "0"))
        .IgnoreAllArguments()
        .SetReturn(0);

    EXPECTED_CALL(mocks, mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreAllArguments()
        .SetReturn(0);

    EXPECTED_CALL(mocks, mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreAllArguments()
        .SetReturn(0);

    EXPECTED_CALL(mocks, mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreAllArguments()
        .SetReturn(0);

    EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreAllArguments()
        .SetReturn(TEST_CHAR_PTR);

    EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreAllArguments()
        .SetReturn(TEST_CHAR_PTR);

    EXPECTED_CALL(mocks, mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreAllArguments()
        .SetReturn(0);

    EXPECTED_CALL(mocks, mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreAllArguments()
        .SetReturn(0);

    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreAllArguments();
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreAllArguments();
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreAllArguments();
    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_destroy(IGNORED_PTR_ARG))
        .IgnoreAllArguments();
    STRICT_EXPECTED_CALL(mocks, Map_Destroy(IGNORED_PTR_ARG))
        .IgnoreAllArguments();
    STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
        .IgnoreAllArguments();

    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    // act
    IOTHUB_SERVICE_CLIENT_AUTH_HANDLE handle = IoTHubServiceClientAuth_CreateFromConnectionString(TEST_CONNECTION_STRING);
    IoTHubServiceClientAuth_Destroy(handle);

    // assert
    mocks.AssertActualAndExpectedCalls();
}

END_TEST_SUITE(iothub_service_client_auth_ut)

