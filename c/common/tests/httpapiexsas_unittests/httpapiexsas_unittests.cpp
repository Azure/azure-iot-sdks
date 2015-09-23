// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <cstdlib>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#include <cstddef>

#include <time.h>

#include "testrunnerswitcher.h"
#include "micromock.h"
#include "micromockcharstararenullterminatedstrings.h"

#include "agenttime.h"
#include "lock.h"
#include "strings.h"
#include "buffer_.h"
#include "sastoken.h"
#include "httpheaders.h"
#include "httpapiex.h"
#include "httpapiexsas.h"

DEFINE_MICROMOCK_ENUM_TO_STRING(HTTPAPIEX_RESULT, HTTPAPIEX_RESULT_VALUES);

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

};

#define TEST_STRING_HANDLE (STRING_HANDLE)0x46
#define TEST_NULL_STRING_HANDLE (STRING_HANDLE)0x00
#define TEST_KEYNAME_HANDLE (STRING_HANDLE)0x48
#define TEST_KEY_HANDLE (STRING_HANDLE)0x49
#define TEST_URIRESOURCE_HANDLE (STRING_HANDLE)0x50
#define TEST_CLONED_KEYNAME_HANDLE  (STRING_HANDLE)0x51
#define TEST_CLONED_URIRESOURCE_HANDLE  (STRING_HANDLE)0x52
#define TEST_CLONED_KEY_HANDLE  (STRING_HANDLE)0x53
#define TEST_HTTPAPIEX_HANDLE (HTTPAPIEX_HANDLE)0x54
#define TEST_HTTPAPI_REQUEST_TYPE (HTTPAPI_REQUEST_TYPE)0x55
#define TEST_REQUEST_HTTP_HEADERS_HANDLE (HTTP_HEADERS_HANDLE)0x56
#define TEST_REQUEST_CONTENT (BUFFER_HANDLE)0x57
#define TEST_RESPONSE_HTTP_HEADERS_HANDLE (HTTP_HEADERS_HANDLE)0x58
#define TEST_RESPONSE_CONTENT (BUFFER_HANDLE)0x59
#define TEST_CONST_CHAR_STAR_NULL (const char*)NULL
#define TEST_SASTOKEN_HANDLE (STRING_HANDLE)0x60
#define TEST_EXPIRY ((size_t)7200)
#define TEST_TIME_T ((time_t)-1)

static const char TEST_CHAR_ARRAY[10] = "ABCD";

static size_t currentmalloc_call;
static size_t whenShallmalloc_fail;


static MICROMOCK_MUTEX_HANDLE g_testByTest;
static MICROMOCK_GLOBAL_SEMAPHORE_HANDLE g_dllByDll;


TYPED_MOCK_CLASS(CHTTPAPIEX_SASMocks, CGlobalMock)
{
public:

    MOCK_STATIC_METHOD_1(, void, STRING_delete, STRING_HANDLE, handle)
    MOCK_VOID_METHOD_END()

    MOCK_STATIC_METHOD_1(, const char*, STRING_c_str, STRING_HANDLE, s)
    MOCK_METHOD_END(const char*, TEST_CONST_CHAR_STAR_NULL)

    MOCK_STATIC_METHOD_1(, size_t, STRING_length, STRING_HANDLE, s)
    MOCK_METHOD_END(size_t, 0)

    MOCK_STATIC_METHOD_1(, STRING_HANDLE, STRING_clone, STRING_HANDLE, s)
    MOCK_METHOD_END(STRING_HANDLE, malloc(1))

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

    MOCK_STATIC_METHOD_1(, void, gballoc_free, void*, ptr)
    BASEIMPLEMENTATION::gballoc_free(ptr);
    MOCK_VOID_METHOD_END()

    MOCK_STATIC_METHOD_8(, HTTPAPIEX_RESULT, HTTPAPIEX_ExecuteRequest, HTTPAPIEX_HANDLE, handle, HTTPAPI_REQUEST_TYPE, requestType, const char*, relativePath, HTTP_HEADERS_HANDLE, requestHttpHeadersHandle, BUFFER_HANDLE, requestContent, unsigned int*, statusCode, HTTP_HEADERS_HANDLE, responseHttpHeadersHandle, BUFFER_HANDLE, responseContent)
    MOCK_METHOD_END(HTTPAPIEX_RESULT, HTTPAPIEX_ERROR)

    MOCK_STATIC_METHOD_2(, const char*, HTTPHeaders_FindHeaderValue,HTTP_HEADERS_HANDLE, httpHeadersHandle, const char*, name)
    MOCK_METHOD_END(const char*, TEST_CONST_CHAR_STAR_NULL)

    MOCK_STATIC_METHOD_3(, HTTP_HEADERS_RESULT, HTTPHeaders_ReplaceHeaderNameValuePair, HTTP_HEADERS_HANDLE, httpHeadersHandle, const char*, name, const char*, value)
    MOCK_METHOD_END(HTTP_HEADERS_RESULT, HTTP_HEADERS_ERROR)

    MOCK_STATIC_METHOD_4(, STRING_HANDLE, SASToken_Create, STRING_HANDLE, key, STRING_HANDLE, scope, STRING_HANDLE, keyName, size_t, expiry)
    MOCK_METHOD_END(STRING_HANDLE, malloc(1))

    MOCK_STATIC_METHOD_1(, time_t, get_time, time_t*, currentTime)
    MOCK_METHOD_END(time_t, TEST_TIME_T)

};

DECLARE_GLOBAL_MOCK_METHOD_1(CHTTPAPIEX_SASMocks, , void, STRING_delete, STRING_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_1(CHTTPAPIEX_SASMocks, , const char*, STRING_c_str, STRING_HANDLE, s);
DECLARE_GLOBAL_MOCK_METHOD_1(CHTTPAPIEX_SASMocks, , size_t, STRING_length, STRING_HANDLE, s);
DECLARE_GLOBAL_MOCK_METHOD_1(CHTTPAPIEX_SASMocks, , STRING_HANDLE, STRING_clone, STRING_HANDLE, s);
DECLARE_GLOBAL_MOCK_METHOD_1(CHTTPAPIEX_SASMocks, , void*, gballoc_malloc, size_t, size);
DECLARE_GLOBAL_MOCK_METHOD_1(CHTTPAPIEX_SASMocks, , void, gballoc_free, void*, ptr);
DECLARE_GLOBAL_MOCK_METHOD_8(CHTTPAPIEX_SASMocks, , HTTPAPIEX_RESULT, HTTPAPIEX_ExecuteRequest, HTTPAPIEX_HANDLE, handle, HTTPAPI_REQUEST_TYPE, requestType, const char*, relativePath, HTTP_HEADERS_HANDLE, requestHttpHeadersHandle, BUFFER_HANDLE, requestContent, unsigned int*, statusCode, HTTP_HEADERS_HANDLE, responseHttpHeadersHandle, BUFFER_HANDLE, responseContent);
DECLARE_GLOBAL_MOCK_METHOD_2(CHTTPAPIEX_SASMocks, , const char*, HTTPHeaders_FindHeaderValue, HTTP_HEADERS_HANDLE, httpHeadersHandle, const char*, name);
DECLARE_GLOBAL_MOCK_METHOD_4(CHTTPAPIEX_SASMocks, , STRING_HANDLE, SASToken_Create, STRING_HANDLE, key, STRING_HANDLE, scope, STRING_HANDLE, keyName, size_t, expiry);
DECLARE_GLOBAL_MOCK_METHOD_3(CHTTPAPIEX_SASMocks, , HTTP_HEADERS_RESULT, HTTPHeaders_ReplaceHeaderNameValuePair, HTTP_HEADERS_HANDLE, httpHeadersHandle, const char*, name, const char*, value);
DECLARE_GLOBAL_MOCK_METHOD_1(CHTTPAPIEX_SASMocks, , time_t, get_time, time_t*, currentTime);

static void mocksResetAllCalls(CHTTPAPIEX_SASMocks* mocks)
{
    if (mocks != NULL)
    {
        mocks->ResetAllCalls();
    }
    currentmalloc_call = 0;
    whenShallmalloc_fail = 0;

}

static void setupSAS_Create_happy_path(CHTTPAPIEX_SASMocks &mocks)
{
    (void)mocks;
    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)).IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_clone(TEST_KEY_HANDLE)).SetReturn(TEST_CLONED_KEY_HANDLE);
    STRICT_EXPECTED_CALL(mocks, STRING_clone(TEST_URIRESOURCE_HANDLE)).SetReturn(TEST_CLONED_URIRESOURCE_HANDLE);
    STRICT_EXPECTED_CALL(mocks, STRING_clone(TEST_KEYNAME_HANDLE)).SetReturn(TEST_CLONED_KEYNAME_HANDLE);

    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)).IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_CLONED_KEY_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_CLONED_KEYNAME_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_CLONED_URIRESOURCE_HANDLE));

}


BEGIN_TEST_SUITE(httpapiexsas_unittests)

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
}

TEST_FUNCTION_CLEANUP(TestMethodCleanup)
{
    if (!MicroMockReleaseMutex(g_testByTest))
    {
        ASSERT_FAIL("failure in test framework at ReleaseMutex");
    }
    mocksResetAllCalls(NULL);
}

TEST_FUNCTION(HTTPAPIEX_SAS_is_zero_the_epoch)
{
    time_t epoch_candidate = 0;
    tm broken_down_time;
    broken_down_time = *gmtime(&epoch_candidate);
    ASSERT_ARE_EQUAL(int, broken_down_time.tm_hour, 0);
    ASSERT_ARE_EQUAL(int, broken_down_time.tm_min, 0);
    ASSERT_ARE_EQUAL(int, broken_down_time.tm_sec, 0);
    ASSERT_ARE_EQUAL(int, broken_down_time.tm_year, 70);
    ASSERT_ARE_EQUAL(int, broken_down_time.tm_mon, 0);
    ASSERT_ARE_EQUAL(int, broken_down_time.tm_mday, 1);

}

/*Tests_SRS_HTTPAPIEXSAS_06_001: [If the parameter key is NULL then HTTPAPIEX_SAS_Create shall return NULL.]*/
TEST_FUNCTION(HTTPAPIEX_SAS_Create_null_key_fails)
{
    // arrange
    HTTPAPIEX_SAS_HANDLE handle;

    // act
    handle = HTTPAPIEX_SAS_Create(TEST_NULL_STRING_HANDLE, TEST_STRING_HANDLE, TEST_STRING_HANDLE);

    // assert
    ASSERT_IS_NULL(handle);
}

/*Tests_SRS_HTTPAPIEXSAS_06_002: [If the parameter uriResource is NULL then HTTPAPIEX_SAS_Create shall return NULL.]*/
TEST_FUNCTION(HTTPAPIEX_SAS_Create_null_uriResource_fails)
{
    // arrange
    HTTPAPIEX_SAS_HANDLE handle;

    // act
    handle = HTTPAPIEX_SAS_Create(TEST_STRING_HANDLE, TEST_NULL_STRING_HANDLE, TEST_STRING_HANDLE);

    // assert
    ASSERT_IS_NULL(handle);
}

/*Tests_SRS_HTTPAPIEXSAS_06_003: [If the parameter keyName is NULL then HTTPAPIEX_SAS_Create shall return NULL.]*/
TEST_FUNCTION(HTTPAPIEX_SAS_Create_null_keyName_fails)
{
    // arrange
    HTTPAPIEX_SAS_HANDLE handle;

    // act
    handle = HTTPAPIEX_SAS_Create(TEST_STRING_HANDLE, TEST_STRING_HANDLE, TEST_NULL_STRING_HANDLE);

    // assert
    ASSERT_IS_NULL(handle);
}

/*Tests_SRS_HTTPAPIEXSAS_06_004: [If there are any other errors in the instantiation of this handle then HTTPAPIEX_SAS_Create shall return NULL.]*/
TEST_FUNCTION(HTTPAPIEX_SAS_Create_malloc_state_fails)
{
    // arrange
    HTTPAPIEX_SAS_HANDLE handle;
    CHTTPAPIEX_SASMocks mocks;

    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)).IgnoreArgument(1);

    whenShallmalloc_fail = 1;

    // act
    handle = HTTPAPIEX_SAS_Create(TEST_STRING_HANDLE, TEST_STRING_HANDLE, TEST_KEYNAME_HANDLE);

    // assert
    ASSERT_IS_NULL(handle);
}

/*Tests_SRS_HTTPAPIEXSAS_06_004: [If there are any other errors in the instantiation of this handle then HTTPAPIEX_SAS_Create shall return NULL.]*/
TEST_FUNCTION(HTTPAPIEX_SAS_Create_first_string_clone_fails)
{
    // arrange
    HTTPAPIEX_SAS_HANDLE handle;
    CHTTPAPIEX_SASMocks mocks;

    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)).IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)).IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_clone(TEST_KEY_HANDLE)).SetReturn(TEST_NULL_STRING_HANDLE);

    // act
    handle = HTTPAPIEX_SAS_Create(TEST_KEY_HANDLE, TEST_URIRESOURCE_HANDLE, TEST_KEYNAME_HANDLE);

    // assert
    ASSERT_IS_NULL(handle);
}

/*Tests_SRS_HTTPAPIEXSAS_06_004: [If there are any other errors in the instantiation of this handle then HTTPAPIEX_SAS_Create shall return NULL.]*/
TEST_FUNCTION(HTTPAPIEX_SAS_Create_second_string_clone_fails)
{
    // arrange
    HTTPAPIEX_SAS_HANDLE handle;
    CHTTPAPIEX_SASMocks mocks;

    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)).IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)).IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_clone(TEST_KEY_HANDLE)).SetReturn(TEST_CLONED_KEY_HANDLE);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_CLONED_KEY_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_clone(TEST_URIRESOURCE_HANDLE)).SetReturn(TEST_NULL_STRING_HANDLE);

    // act
    handle = HTTPAPIEX_SAS_Create(TEST_KEY_HANDLE, TEST_URIRESOURCE_HANDLE, TEST_KEYNAME_HANDLE);

    // assert
    ASSERT_IS_NULL(handle);
}

/*Tests_SRS_HTTPAPIEXSAS_06_004: [If there are any other errors in the instantiation of this handle then HTTPAPIEX_SAS_Create shall return NULL.]*/
TEST_FUNCTION(HTTPAPIEX_SAS_Create_third_string_clone_fails)
{
    // arrange
    HTTPAPIEX_SAS_HANDLE handle;
    CHTTPAPIEX_SASMocks mocks;

    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)).IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)).IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_clone(TEST_KEY_HANDLE)).SetReturn(TEST_CLONED_KEY_HANDLE);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_CLONED_KEY_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_clone(TEST_URIRESOURCE_HANDLE)).SetReturn(TEST_CLONED_URIRESOURCE_HANDLE);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_CLONED_URIRESOURCE_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_clone(TEST_KEYNAME_HANDLE)).SetReturn(TEST_NULL_STRING_HANDLE);

    // act
    handle = HTTPAPIEX_SAS_Create(TEST_KEY_HANDLE, TEST_URIRESOURCE_HANDLE, TEST_KEYNAME_HANDLE);

    // assert
    ASSERT_IS_NULL(handle);
}

/*Tests_SRS_HTTPAPIEXSAS_06_006: [HTTAPIEX_SAS_Destroy shall deallocate any structures denoted by the parameter handle.]*/
TEST_FUNCTION(HTTPAPIEX_SAS_Create_succeeds)
{
    // arrange
    HTTPAPIEX_SAS_HANDLE handle;
    CHTTPAPIEX_SASMocks mocks;

    setupSAS_Create_happy_path(mocks);

    // act
    handle = HTTPAPIEX_SAS_Create(TEST_KEY_HANDLE, TEST_URIRESOURCE_HANDLE, TEST_KEYNAME_HANDLE);

    // assert
    ASSERT_IS_NOT_NULL(handle);

    // Cleanup
    HTTPAPIEX_SAS_Destroy(handle);
}

/*Tests_SRS_HTTPAPIEXSAS_06_005: [If the parameter handle is NULL then HTTAPIEX_SAS_Destroy shall do nothing and return.]*/
TEST_FUNCTION(HTTPAPIEX_SAS_destroy_with_null_succeeds)
{
    // act
    HTTPAPIEX_SAS_Destroy(NULL);
}

/*Tests_SRS_HTTPAPIEXSAS_06_007: [If the parameter sasHandle is NULL then HTTPAPIEX_SAS_ExecuteRequest shall simply invoke HTTPAPIEX_ExecuteRequest with the remaining parameters (following sasHandle) as its arguments and shall return immediately with the result of that call as the result of HTTPAPIEX_SAS_ExecuteRequest.]*/
TEST_FUNCTION(HTTPAPIEX_SAS_invoke_executerequest_with_null_sas_handle_succeeds)
{
    HTTPAPIEX_RESULT result = HTTPAPIEX_ERROR;
    CHTTPAPIEX_SASMocks mocks;
    unsigned int statusCode;

    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_ExecuteRequest(TEST_HTTPAPIEX_HANDLE, TEST_HTTPAPI_REQUEST_TYPE, TEST_CHAR_ARRAY, TEST_REQUEST_HTTP_HEADERS_HANDLE, TEST_REQUEST_CONTENT, &statusCode, TEST_RESPONSE_HTTP_HEADERS_HANDLE, TEST_RESPONSE_CONTENT)).SetReturn(HTTPAPIEX_OK);
    // act
    result = HTTPAPIEX_SAS_ExecuteRequest(NULL, TEST_HTTPAPIEX_HANDLE, TEST_HTTPAPI_REQUEST_TYPE, TEST_CHAR_ARRAY, TEST_REQUEST_HTTP_HEADERS_HANDLE, TEST_REQUEST_CONTENT, &statusCode, TEST_RESPONSE_HTTP_HEADERS_HANDLE, TEST_RESPONSE_CONTENT);

    // assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, result, HTTPAPIEX_OK);
}

/*Tests_SRS_HTTPAPIEXSAS_06_008: [if the parameter requestHttpHeadersHandle is NULL then fallthrough.]*/
TEST_FUNCTION(HTTPAPIEX_SAS_invoke_executerequest_with_null_request_http_headers_handle_succeeds)
{

    HTTPAPIEX_RESULT result;
    CHTTPAPIEX_SASMocks mocks;
    unsigned int statusCode;
    HTTPAPIEX_SAS_HANDLE sasHandle;

    // arrange
    setupSAS_Create_happy_path(mocks);
    sasHandle = HTTPAPIEX_SAS_Create(TEST_KEY_HANDLE, TEST_URIRESOURCE_HANDLE, TEST_KEYNAME_HANDLE);
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_ExecuteRequest(TEST_HTTPAPIEX_HANDLE, TEST_HTTPAPI_REQUEST_TYPE, TEST_CHAR_ARRAY, NULL, TEST_REQUEST_CONTENT, &statusCode, TEST_RESPONSE_HTTP_HEADERS_HANDLE, TEST_RESPONSE_CONTENT)).SetReturn(HTTPAPIEX_OK);

    // act
    result = HTTPAPIEX_SAS_ExecuteRequest(sasHandle, TEST_HTTPAPIEX_HANDLE, TEST_HTTPAPI_REQUEST_TYPE, TEST_CHAR_ARRAY, NULL, TEST_REQUEST_CONTENT, &statusCode, TEST_RESPONSE_HTTP_HEADERS_HANDLE, TEST_RESPONSE_CONTENT);

    // assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, result, HTTPAPIEX_OK);

    // Cleanup
    HTTPAPIEX_SAS_Destroy(sasHandle);
}

/*Tests_SRS_HTTPAPIEXSAS_06_009: [HTTPHeaders_FindHeaderValue shall be invoked with the requestHttpHeadersHandle as its first argument and the string "Authorization" as its second argument.]*/
/*Tests_SRS_HTTPAPIEXSAS_06_010: [If the return result of the invocation of HTTPHeaders_FindHeaderValue is NULL then fallthrough.]*/
TEST_FUNCTION(HTTPAPIEX_SAS_invoke_executerequest_findheadervalues_returns_null_succeeds)
{

    HTTPAPIEX_RESULT result;
    CHTTPAPIEX_SASMocks mocks;
    unsigned int statusCode;
    HTTPAPIEX_SAS_HANDLE sasHandle;

    // arrange
    setupSAS_Create_happy_path(mocks);
    sasHandle = HTTPAPIEX_SAS_Create(TEST_KEY_HANDLE, TEST_URIRESOURCE_HANDLE, TEST_KEYNAME_HANDLE);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_FindHeaderValue(TEST_REQUEST_HTTP_HEADERS_HANDLE, "Authorization")).SetReturn(TEST_CONST_CHAR_STAR_NULL);
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_ExecuteRequest(TEST_HTTPAPIEX_HANDLE, TEST_HTTPAPI_REQUEST_TYPE, TEST_CHAR_ARRAY, TEST_REQUEST_HTTP_HEADERS_HANDLE, TEST_REQUEST_CONTENT, &statusCode, TEST_RESPONSE_HTTP_HEADERS_HANDLE, TEST_RESPONSE_CONTENT)).SetReturn(HTTPAPIEX_OK);

    // act
    result = HTTPAPIEX_SAS_ExecuteRequest(sasHandle, TEST_HTTPAPIEX_HANDLE, TEST_HTTPAPI_REQUEST_TYPE, TEST_CHAR_ARRAY, TEST_REQUEST_HTTP_HEADERS_HANDLE, TEST_REQUEST_CONTENT, &statusCode, TEST_RESPONSE_HTTP_HEADERS_HANDLE, TEST_RESPONSE_CONTENT);

    // assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, result, HTTPAPIEX_OK);

    // Cleanup
    HTTPAPIEX_SAS_Destroy(sasHandle);
}

/*Tests_SRS_HTTPAPIEXSAS_06_018: [A value of type time_t that shall be known as currentTime is obtained from calling get_time.]*/
/*Tests_SRS_HTTPAPIEXSAS_06_019: [If the value of currentTime is (time_t)-1 is then fallthrough.]*/
TEST_FUNCTION(HTTPAPIEX_SAS_invoke_executerequest_get_time_fails)
{

    HTTPAPIEX_RESULT result;
    CHTTPAPIEX_SASMocks mocks;
    unsigned int statusCode;
    HTTPAPIEX_SAS_HANDLE sasHandle;

    // arrange
    setupSAS_Create_happy_path(mocks);
    sasHandle = HTTPAPIEX_SAS_Create(TEST_KEY_HANDLE, TEST_URIRESOURCE_HANDLE, TEST_KEYNAME_HANDLE);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_FindHeaderValue(TEST_REQUEST_HTTP_HEADERS_HANDLE, "Authorization")).SetReturn(TEST_CHAR_ARRAY);
    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn((time_t)-1);
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_ExecuteRequest(TEST_HTTPAPIEX_HANDLE, TEST_HTTPAPI_REQUEST_TYPE, TEST_CHAR_ARRAY, TEST_REQUEST_HTTP_HEADERS_HANDLE, TEST_REQUEST_CONTENT, &statusCode, TEST_RESPONSE_HTTP_HEADERS_HANDLE, TEST_RESPONSE_CONTENT)).SetReturn(HTTPAPIEX_OK);

    // act
    result = HTTPAPIEX_SAS_ExecuteRequest(sasHandle, TEST_HTTPAPIEX_HANDLE, TEST_HTTPAPI_REQUEST_TYPE, TEST_CHAR_ARRAY, TEST_REQUEST_HTTP_HEADERS_HANDLE, TEST_REQUEST_CONTENT, &statusCode, TEST_RESPONSE_HTTP_HEADERS_HANDLE, TEST_RESPONSE_CONTENT);

    // assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, result, HTTPAPIEX_OK);

    // Cleanup
    HTTPAPIEX_SAS_Destroy(sasHandle);
}

/*Tests_SRS_HTTPAPIEXSAS_06_011: [SASToken_Create shall be invoked.]*/
/*Tests_SRS_HTTPAPIEXSAS_06_012: [If the return result of SASToken_Create is NULL then fallthrough.]*/
TEST_FUNCTION(HTTPAPIEX_SAS_invoke_executerequest_sastoken_create_returns_null_succeeds)
{

    HTTPAPIEX_RESULT result;
    CHTTPAPIEX_SASMocks mocks;
    unsigned int statusCode;
    HTTPAPIEX_SAS_HANDLE sasHandle;

    // arrange
    setupSAS_Create_happy_path(mocks);
    sasHandle = HTTPAPIEX_SAS_Create(TEST_KEY_HANDLE, TEST_URIRESOURCE_HANDLE, TEST_KEYNAME_HANDLE);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_FindHeaderValue(TEST_REQUEST_HTTP_HEADERS_HANDLE, "Authorization")).SetReturn(TEST_CHAR_ARRAY);
    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(3600);
    STRICT_EXPECTED_CALL(mocks, SASToken_Create(TEST_CLONED_KEY_HANDLE, TEST_CLONED_URIRESOURCE_HANDLE, TEST_CLONED_KEYNAME_HANDLE, TEST_EXPIRY)).SetReturn(TEST_NULL_STRING_HANDLE);
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_ExecuteRequest(TEST_HTTPAPIEX_HANDLE, TEST_HTTPAPI_REQUEST_TYPE, TEST_CHAR_ARRAY, TEST_REQUEST_HTTP_HEADERS_HANDLE, TEST_REQUEST_CONTENT, &statusCode, TEST_RESPONSE_HTTP_HEADERS_HANDLE, TEST_RESPONSE_CONTENT)).SetReturn(HTTPAPIEX_OK);

    // act
    result = HTTPAPIEX_SAS_ExecuteRequest(sasHandle, TEST_HTTPAPIEX_HANDLE, TEST_HTTPAPI_REQUEST_TYPE, TEST_CHAR_ARRAY, TEST_REQUEST_HTTP_HEADERS_HANDLE, TEST_REQUEST_CONTENT, &statusCode, TEST_RESPONSE_HTTP_HEADERS_HANDLE, TEST_RESPONSE_CONTENT);

    // assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, result, HTTPAPIEX_OK);

    // Cleanup
    HTTPAPIEX_SAS_Destroy(sasHandle);
}

/*Tests_SRS_HTTPAPIEXSAS_06_013: [HTTPHeaders_ReplaceHeaderNameValuePair shall be invoked with "Authorization" as its second argument and STRING_c_str (newSASToken) as its third argument.]*/
/*Tests_SRS_HTTPAPIEXSAS_06_014: [If the result of the invocation of HTTPHeaders_ReplaceHeaderNameValuePair is NOT HTTP_HEADERS_OK then fallthrough.]*/
/*Tests_SRS_HTTPAPIEXSAS_06_015: [STRING_delete(newSASToken) will be invoked.]*/
TEST_FUNCTION(HTTPAPIEX_SAS_invoke_executerequest_replace_header_name_value_pair_fails_succeeds)
{

    HTTPAPIEX_RESULT result;
    CHTTPAPIEX_SASMocks mocks;
    unsigned int statusCode;
    HTTPAPIEX_SAS_HANDLE sasHandle;

    // arrange
    setupSAS_Create_happy_path(mocks);
    sasHandle = HTTPAPIEX_SAS_Create(TEST_KEY_HANDLE, TEST_URIRESOURCE_HANDLE, TEST_KEYNAME_HANDLE);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_FindHeaderValue(TEST_REQUEST_HTTP_HEADERS_HANDLE, "Authorization")).SetReturn(TEST_CHAR_ARRAY);
    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(3600);
    STRICT_EXPECTED_CALL(mocks, SASToken_Create(TEST_CLONED_KEY_HANDLE, TEST_CLONED_URIRESOURCE_HANDLE, TEST_CLONED_KEYNAME_HANDLE, TEST_EXPIRY)).SetReturn(TEST_SASTOKEN_HANDLE);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_SASTOKEN_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(TEST_SASTOKEN_HANDLE)).SetReturn(TEST_CHAR_ARRAY);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(TEST_REQUEST_HTTP_HEADERS_HANDLE, "Authorization", TEST_CHAR_ARRAY)).SetReturn(HTTP_HEADERS_ERROR);
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_ExecuteRequest(TEST_HTTPAPIEX_HANDLE, TEST_HTTPAPI_REQUEST_TYPE, TEST_CHAR_ARRAY, TEST_REQUEST_HTTP_HEADERS_HANDLE, TEST_REQUEST_CONTENT, &statusCode, TEST_RESPONSE_HTTP_HEADERS_HANDLE, TEST_RESPONSE_CONTENT)).SetReturn(HTTPAPIEX_OK);

    // act
    result = HTTPAPIEX_SAS_ExecuteRequest(sasHandle, TEST_HTTPAPIEX_HANDLE, TEST_HTTPAPI_REQUEST_TYPE, TEST_CHAR_ARRAY, TEST_REQUEST_HTTP_HEADERS_HANDLE, TEST_REQUEST_CONTENT, &statusCode, TEST_RESPONSE_HTTP_HEADERS_HANDLE, TEST_RESPONSE_CONTENT);

    // assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, result, HTTPAPIEX_OK);

    // Cleanup
    HTTPAPIEX_SAS_Destroy(sasHandle);
}

/*Tests_SRS_HTTPAPIEXSAS_06_016: [HTTPAPIEX_ExecuteRequest with the remaining parameters (following sasHandle) as its arguments will be invoked and the result of that call is the result of HTTPAPIEX_SAS_ExecuteRequest.]*/
TEST_FUNCTION(HTTPAPIEX_SAS_invoke_executerequest_replace_header_name_value_pair_succeeds_succeeds)
{

    HTTPAPIEX_RESULT result;
    CHTTPAPIEX_SASMocks mocks;
    unsigned int statusCode;
    HTTPAPIEX_SAS_HANDLE sasHandle;

    // arrange
    setupSAS_Create_happy_path(mocks);
    sasHandle = HTTPAPIEX_SAS_Create(TEST_KEY_HANDLE, TEST_URIRESOURCE_HANDLE, TEST_KEYNAME_HANDLE);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_FindHeaderValue(TEST_REQUEST_HTTP_HEADERS_HANDLE, "Authorization")).SetReturn(TEST_CHAR_ARRAY);
    STRICT_EXPECTED_CALL(mocks, get_time(NULL)).SetReturn(3600);
    STRICT_EXPECTED_CALL(mocks, SASToken_Create(TEST_CLONED_KEY_HANDLE, TEST_CLONED_URIRESOURCE_HANDLE, TEST_CLONED_KEYNAME_HANDLE, TEST_EXPIRY)).SetReturn(TEST_SASTOKEN_HANDLE);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_SASTOKEN_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(TEST_SASTOKEN_HANDLE)).SetReturn(TEST_CHAR_ARRAY);
    STRICT_EXPECTED_CALL(mocks, HTTPHeaders_ReplaceHeaderNameValuePair(TEST_REQUEST_HTTP_HEADERS_HANDLE, "Authorization", TEST_CHAR_ARRAY)).SetReturn(HTTP_HEADERS_OK);
    STRICT_EXPECTED_CALL(mocks, HTTPAPIEX_ExecuteRequest(TEST_HTTPAPIEX_HANDLE, TEST_HTTPAPI_REQUEST_TYPE, TEST_CHAR_ARRAY, TEST_REQUEST_HTTP_HEADERS_HANDLE, TEST_REQUEST_CONTENT, &statusCode, TEST_RESPONSE_HTTP_HEADERS_HANDLE, TEST_RESPONSE_CONTENT)).SetReturn(HTTPAPIEX_OK);

    // act
    result = HTTPAPIEX_SAS_ExecuteRequest(sasHandle, TEST_HTTPAPIEX_HANDLE, TEST_HTTPAPI_REQUEST_TYPE, TEST_CHAR_ARRAY, TEST_REQUEST_HTTP_HEADERS_HANDLE, TEST_REQUEST_CONTENT, &statusCode, TEST_RESPONSE_HTTP_HEADERS_HANDLE, TEST_RESPONSE_CONTENT);

    // assert
    ASSERT_ARE_EQUAL(HTTPAPIEX_RESULT, result, HTTPAPIEX_OK);

    // Cleanup
    HTTPAPIEX_SAS_Destroy(sasHandle);
}

END_TEST_SUITE(httpapiexsas_unittests)
