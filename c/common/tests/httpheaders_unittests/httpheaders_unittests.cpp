// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <cstdlib>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include <climits>

#include "testrunnerswitcher.h"
#include "micromock.h"
#include "micromockcharstararenullterminatedstrings.h"
#include "httpheaders.h"
#include "map.h"
#include "lock.h"

#define GBALLOC_H

extern "C" int gballoc_init(void);
extern "C" void gballoc_deinit(void);
extern "C" void* gballoc_malloc(size_t size);
extern "C" void* gballoc_calloc(size_t nmemb, size_t size);
extern "C" void* gballoc_realloc(void* ptr, size_t size);
extern "C" void gballoc_free(void* ptr);


namespace BASEIMPLEMENTATION
{
    /*if malloc is defined as gballoc_malloc at this moment, there'd be serious trouble*/
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

/*Below tags exists for traceability reasons only, they canot be really tested by automated means, except "this file compiles"*/
/*Tests_SRS_HTTP_HEADERS_99_001:[ HttpHeaders shall have the following interface]*/

static MICROMOCK_MUTEX_HANDLE g_testByTest;

DEFINE_MICROMOCK_ENUM_TO_STRING(HTTP_HEADERS_RESULT, HTTP_HEADERS_RESULT_VALUES);

/*test assets*/
#define NAME1 "name1"
#define VALUE1 "value1"
#define HEADER1 NAME1 ": " VALUE1
static const char *NAME1_TRICK1 = "name1:";
static const char *NAME1_TRICK2 = "name1: ";
static const char *NAME1_TRICK3 = "name1: value1";

#define NAME2 "name2"
#define VALUE2 "value2"
#define HEADER2 NAME2 ": " VALUE2

#define TEMP_BUFFER_SIZE 1024
static char tempBuffer[TEMP_BUFFER_SIZE];

#define MAX_NAME_VALUE_PAIR 100

static MICROMOCK_GLOBAL_SEMAPHORE_HANDLE g_dllByDll;

static size_t currentMap_Create_call;
static size_t whenShallMap_Create_fail;

static size_t currentMap_Clone_call;
static size_t whenShallMap_Clone_fail;


static size_t currentmalloc_call;
static size_t whenShallmalloc_fail;

static size_t currentrealloc_call;
static size_t whenShallrealloc_fail;




TYPED_MOCK_CLASS(CHTTPHeadersMocks, CGlobalMock)
{
public:
    MOCK_STATIC_METHOD_1(, MAP_HANDLE, Map_Create, MAP_FILTER_CALLBACK, mapFilterFunc)
        MAP_HANDLE result2;
    currentMap_Create_call++;
    if (currentMap_Create_call == whenShallMap_Create_fail)
    {
        result2 = (MAP_HANDLE)NULL;
    }
    else
    {
        result2 = (MAP_HANDLE)malloc(1);
    }
    MOCK_METHOD_END(MAP_HANDLE, result2)

        MOCK_STATIC_METHOD_1(, MAP_HANDLE, Map_Clone, MAP_HANDLE, handle)
        MAP_HANDLE result2;
    currentMap_Clone_call++;
    if (currentMap_Clone_call == whenShallMap_Clone_fail)
    {
        result2 = (MAP_HANDLE)NULL;
    }
    else
    {
        result2 = (MAP_HANDLE)malloc(1);
    }
    MOCK_METHOD_END(MAP_HANDLE, result2)


    MOCK_STATIC_METHOD_1(, void, Map_Destroy, MAP_HANDLE, handle)
        free(handle);
    MOCK_VOID_METHOD_END()

    MOCK_STATIC_METHOD_3(, MAP_RESULT, Map_AddOrUpdate, MAP_HANDLE, handle, const char*, key, const char*, value)
    MOCK_METHOD_END(MAP_RESULT, MAP_OK)

    MOCK_STATIC_METHOD_2(, const char*, Map_GetValueFromKey, MAP_HANDLE, handle, const char*, key)
    MOCK_METHOD_END(const char*, VALUE1)

    MOCK_STATIC_METHOD_4(, MAP_RESULT, Map_GetInternals, MAP_HANDLE, handle, const char*const**, keys, const char*const**, values, size_t*, count)
    MOCK_METHOD_END(MAP_RESULT, MAP_OK)

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
        void* result2;
    currentrealloc_call++;
    if (whenShallrealloc_fail>0)
    {
        if (currentrealloc_call == whenShallrealloc_fail)
        {
            result2 = (STRING_HANDLE)NULL;
        }
        else
        {
            result2 = BASEIMPLEMENTATION::gballoc_realloc(ptr, size);
        }
    }
    else
    {
        result2 = BASEIMPLEMENTATION::gballoc_realloc(ptr, size);
    }
    MOCK_METHOD_END(void*, result2);

    MOCK_STATIC_METHOD_1(, void, gballoc_free, void*, ptr)
        BASEIMPLEMENTATION::gballoc_free(ptr);
    MOCK_VOID_METHOD_END()

};

DECLARE_GLOBAL_MOCK_METHOD_1(CHTTPHeadersMocks, , MAP_HANDLE, Map_Create, MAP_FILTER_CALLBACK, mapFilterFunc);
DECLARE_GLOBAL_MOCK_METHOD_1(CHTTPHeadersMocks, , void, Map_Destroy, MAP_HANDLE, handle)
DECLARE_GLOBAL_MOCK_METHOD_1(CHTTPHeadersMocks, , MAP_HANDLE, Map_Clone, MAP_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_3(CHTTPHeadersMocks, , MAP_RESULT, Map_AddOrUpdate, MAP_HANDLE, handle, const char*, key, const char*, value);
DECLARE_GLOBAL_MOCK_METHOD_2(CHTTPHeadersMocks, , const char*, Map_GetValueFromKey, MAP_HANDLE, handle, const char*, key);
DECLARE_GLOBAL_MOCK_METHOD_4(CHTTPHeadersMocks, , MAP_RESULT, Map_GetInternals, MAP_HANDLE, handle, const char*const**, keys, const char*const**, values, size_t*, count);

DECLARE_GLOBAL_MOCK_METHOD_1(CHTTPHeadersMocks, , void*, gballoc_malloc, size_t, size);
DECLARE_GLOBAL_MOCK_METHOD_2(CHTTPHeadersMocks, , void*, gballoc_realloc, void*, ptr, size_t, size);
DECLARE_GLOBAL_MOCK_METHOD_1(CHTTPHeadersMocks, , void, gballoc_free, void*, ptr);

BEGIN_TEST_SUITE(HTTPHeaders_UnitTests)

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

            currentMap_Create_call = 0;
            whenShallMap_Create_fail = 0;

            currentMap_Clone_call = 0;
            whenShallMap_Clone_fail = 0;

            currentmalloc_call = 0;
            whenShallmalloc_fail = 0;

            currentrealloc_call = 0;
            whenShallrealloc_fail = 0;
        }

        TEST_FUNCTION_CLEANUP(TestMethodCleanup)
        {
            if (!MicroMockReleaseMutex(g_testByTest))
            {
                ASSERT_FAIL("failure in test framework at ReleaseMutex");
            }
        }


        /*Tests_SRS_HTTP_HEADERS_99_002:[ This API shall produce a HTTP_HANDLE that can later be used in subsequent calls to the module.]*/
        TEST_FUNCTION(HTTPHeaders_Alloc_happy_path_succeeds)
        {
            ///arrange
            CHTTPHeadersMocks mocks;

            STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
                .IgnoreArgument(1);

            STRICT_EXPECTED_CALL(mocks, Map_Create(IGNORED_PTR_ARG));

            ///act
            auto handle = HTTPHeaders_Alloc();

            ///assert
            ASSERT_IS_NOT_NULL(handle);
            mocks.AssertActualAndExpectedCalls();

            /// cleanup
            HTTPHeaders_Free(handle);
        }


        /*Tests_SRS_HTTP_HEADERS_99_003:[ The function shall return NULL when the function cannot execute properly]*/
        TEST_FUNCTION(HTTPHeaders_Alloc_fails_when_malloc_fails)
        {
            ///arrange
            CHTTPHeadersMocks mocks;

            whenShallmalloc_fail = currentmalloc_call + 1;
            STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
                .IgnoreArgument(1);

            ///act
            auto httpHandle = HTTPHeaders_Alloc();

            ///assert
            ASSERT_IS_NULL(httpHandle);
            
        }

        /*Tests_SRS_HTTP_HEADERS_02_001: [If httpHeadersHandle is NULL then HTTPHeaders_Free shall perform no action.] */
        TEST_FUNCTION(HTTPHeaders_Free_with_NULL_handle_does_nothing)
        {
            ///arrange
            CHTTPHeadersMocks mocks;

            ///act
            HTTPHeaders_Free(NULL);

            ///assert
            mocks.AssertActualAndExpectedCalls();

        }

        /*Tests_SRS_HTTP_HEADERS_99_005:[ Calling this API shall de-allocate the data structures allocated by previous API calls to the same handle.]*/
        TEST_FUNCTION(HTTPHeaders_Free_with_valid_handle_succeeds)
        {
            ///arrange
            CHTTPHeadersMocks mocks;
            auto handle = HTTPHeaders_Alloc();
            mocks.ResetAllCalls();

            STRICT_EXPECTED_CALL(mocks, Map_Destroy(IGNORED_PTR_ARG))
                .IgnoreArgument(1);
            STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
                .IgnoreArgument(1);

            ///act
            HTTPHeaders_Free(handle);

            ///assert
            mocks.AssertActualAndExpectedCalls();

        }


        /*Tests_SRS_HTTP_HEADERS_99_003:[ The function shall return NULL when the function cannot execute properly]*/
        TEST_FUNCTION(HTTPHeaders_Alloc_fails_when_Map_Create_fails)
        {
            ///arrange
            CHTTPHeadersMocks mocks;

            STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
                .IgnoreArgument(1);
            STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
                .IgnoreArgument(1);

            whenShallMap_Create_fail = currentMap_Create_call + 1;
            STRICT_EXPECTED_CALL(mocks, Map_Create(IGNORED_PTR_ARG)); 

            ///act
            auto httpHandle = HTTPHeaders_Alloc();

            ///assert
            ASSERT_IS_NULL(httpHandle);

        }

        /*Tests_SRS_HTTP_HEADERS_99_004:[ After a successful init, HTTPHeaders_GetHeaderCount shall report 0 existing headers.]*/
        TEST_FUNCTION(HTTPHeaders_Alloc_succeeds_and_GetHeaderCount_returns_0)
        {
            ///arrange
            CHTTPHeadersMocks mocks;
            auto httpHandle = HTTPHeaders_Alloc();
            size_t nHeaders;
            size_t zero = 0;
            mocks.ResetAllCalls();

            STRICT_EXPECTED_CALL(mocks, Map_GetInternals(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreAllArguments()
                .CopyOutArgumentBuffer(4, &zero, sizeof(zero));

            ///act
            auto res = HTTPHeaders_GetHeaderCount(httpHandle, &nHeaders);

            ///assert
            ASSERT_ARE_EQUAL(HTTP_HEADERS_RESULT, HTTP_HEADERS_OK, res);
            ASSERT_ARE_EQUAL(size_t, 0, nHeaders);
            mocks.AssertActualAndExpectedCalls();

            ///cleanup
            HTTPHeaders_Free(httpHandle);
        }

        /*Tests_SRS_HTTP_HEADERS_99_012:[ Calling this API shall record a header from name and value parameters.]*/
        /*Tests_SRS_HTTP_HEADERS_99_013:[ The function shall return HTTP_HEADERS_OK when execution is successful.]*/
        TEST_FUNCTION(HTTPHeaders_AddHeaderNameValuePair_happy_path_succeeds)
        {
            ///arrange
            CHTTPHeadersMocks mocks;
            auto httpHandle = HTTPHeaders_Alloc();
            mocks.ResetAllCalls();

            STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(IGNORED_PTR_ARG, NAME1))
                .IgnoreArgument(1)
                .SetReturn((const char*)NULL); /*this key does not exist*/

            STRICT_EXPECTED_CALL(mocks, Map_AddOrUpdate(IGNORED_PTR_ARG, NAME1, VALUE1))
                .IgnoreArgument(1);

            ///act
            auto res = HTTPHeaders_AddHeaderNameValuePair(httpHandle, NAME1, VALUE1);

            ///assert
            ASSERT_ARE_EQUAL(HTTP_HEADERS_RESULT, HTTP_HEADERS_OK, res);
            mocks.AssertActualAndExpectedCalls();

            //checking content

            ///cleanup
            HTTPHeaders_Free(httpHandle);
        }

        /*Codes_SRS_HTTP_HEADERS_99_015:[ The function shall return HTTP_HEADERS_ALLOC_FAILED when an internal request to allocate memory fails.]*/
        TEST_FUNCTION(HTTPHeaders_AddHeaderNameValuePair_fails_when_Map_AddOrUpdate_fails)
        {
            ///arrange
            CHTTPHeadersMocks mocks;
            auto httpHandle = HTTPHeaders_Alloc();
            mocks.ResetAllCalls();

            STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(IGNORED_PTR_ARG, NAME1))
                .IgnoreArgument(1)
                .SetReturn((const char*)NULL); /*this key does not exist*/


            STRICT_EXPECTED_CALL(mocks, Map_AddOrUpdate(IGNORED_PTR_ARG, NAME1, VALUE1))
                .IgnoreArgument(1)
                .SetReturn(MAP_ERROR);

            ///act
            auto res = HTTPHeaders_AddHeaderNameValuePair(httpHandle, NAME1, VALUE1);

            ///assert
            ASSERT_ARE_EQUAL(HTTP_HEADERS_RESULT, HTTP_HEADERS_ALLOC_FAILED, res);
            mocks.AssertActualAndExpectedCalls();

            //checking content

            ///cleanup
            HTTPHeaders_Free(httpHandle);
        }

        /*Tests_SRS_HTTP_HEADERS_99_016:[ The function shall store the name:value pair in such a way that when later retrieved by a call to GetHeader it will return a string that shall strcmp equal to the name+": "+value.]*/
        TEST_FUNCTION(HTTPHeaders_AddHeaderNameValuePair_succeeds)
        {
            ///arrange
            CHTTPHeadersMocks mocks;
            auto httpHandle = HTTPHeaders_Alloc();
            mocks.ResetAllCalls();

            STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(IGNORED_PTR_ARG, NAME1))
                .IgnoreArgument(1)
                .SetReturn((const char*)NULL); /*this key does not exist*/

            STRICT_EXPECTED_CALL(mocks, Map_AddOrUpdate(IGNORED_PTR_ARG, NAME1, VALUE1))
                .IgnoreArgument(1);

            ///act
            auto res = HTTPHeaders_AddHeaderNameValuePair(httpHandle, NAME1, VALUE1);

            ///assert
            ASSERT_ARE_EQUAL(HTTP_HEADERS_RESULT, HTTP_HEADERS_OK, res);
            mocks.AssertActualAndExpectedCalls();

            //checking content

            ///cleanup
            HTTPHeaders_Free(httpHandle);
        }

        

        /*Tests_SRS_HTTP_HEADERS_99_014:[ The function shall return when the handle is not valid or when name parameter is NULL or when value parameter is NULL.]*/
        TEST_FUNCTION(HTTPHeaders_AddHeaderNameValuePair_with_NULL_handle_fails)
        {
            ///arrange
            CHTTPHeadersMocks mocks;

            ///act
            auto res = HTTPHeaders_AddHeaderNameValuePair(NULL, NAME1, VALUE1);

            ///assert
            ASSERT_ARE_EQUAL(HTTP_HEADERS_RESULT, HTTP_HEADERS_INVALID_ARG, res);
        }

        /*Tests_SRS_HTTP_HEADERS_99_014:[ The function shall return when the handle is not valid or when name parameter is NULL or when value parameter is NULL.]*/
        TEST_FUNCTION(HTTPHeaders_AddHeaderNameValuePair_with_NULL_name_fails)
        {
            ///arrange
            CHTTPHeadersMocks mocks;
            auto httpHandle = HTTPHeaders_Alloc();
            mocks.ResetAllCalls();

            ///act
            auto res = HTTPHeaders_AddHeaderNameValuePair(httpHandle, NULL, VALUE1);

            ///assert
            ASSERT_ARE_EQUAL(HTTP_HEADERS_RESULT, HTTP_HEADERS_INVALID_ARG, res);
            mocks.AssertActualAndExpectedCalls();

            ///cleanup
            HTTPHeaders_Free(httpHandle);
        }

        /*Tests_SRS_HTTP_HEADERS_99_014:[ The function shall return when the handle is not valid or when name parameter is NULL or when value parameter is NULL.]*/
        TEST_FUNCTION(HTTPHeaders_AddHeaderNameValuePair_with_NULL_value_fails)
        {
            ///arrange
            CHTTPHeadersMocks mocks;
            auto httpHandle = HTTPHeaders_Alloc();
            mocks.ResetAllCalls();

            ///act
            auto res = HTTPHeaders_AddHeaderNameValuePair(httpHandle, NAME1, NULL);

            ///assert
            ASSERT_ARE_EQUAL(HTTP_HEADERS_RESULT, HTTP_HEADERS_INVALID_ARG, res);
            mocks.AssertActualAndExpectedCalls();

            ///cleanup
            HTTPHeaders_Free(httpHandle);
        }

        /*Tests_SRS_HTTP_HEADERS_99_017:[ If the name already exists in the collection of headers, the function shall concatenate the new value after the existing value, separated by a comma and a space as in: old-value+", "+new-value.]*/
        TEST_FUNCTION(HTTPHeaders_AddHeaderNameValuePair_with_same_Name_appends_to_existing_value_succeeds)
        {
            ///arrange
            CHTTPHeadersMocks mocks;
            auto httpHandle = HTTPHeaders_Alloc();
            STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(IGNORED_PTR_ARG, NAME1))
                .IgnoreArgument(1)
                .SetReturn((const char*)NULL); /*this key does not exist, the line below is adding it*/
            (void)HTTPHeaders_AddHeaderNameValuePair(httpHandle, NAME1, VALUE1);
            mocks.ResetAllCalls();

            STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(IGNORED_PTR_ARG, NAME1))
                .IgnoreArgument(1)
                .SetReturn(VALUE1);

            STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
                .IgnoreArgument(1);
            STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
                .IgnoreArgument(1);

            STRICT_EXPECTED_CALL(mocks, Map_AddOrUpdate(IGNORED_PTR_ARG, NAME1, VALUE1 ", " VALUE1))
                .IgnoreArgument(1);
            
            ///act
            auto res = HTTPHeaders_AddHeaderNameValuePair(httpHandle, NAME1, VALUE1);

            ///assert
            ASSERT_ARE_EQUAL(HTTP_HEADERS_RESULT, HTTP_HEADERS_OK, res);
            mocks.AssertActualAndExpectedCalls();

            ///cleanup
            HTTPHeaders_Free(httpHandle);
        }

        /*Tests_SRS_HTTP_HEADERS_99_015:[ The function shall return HTTP_HEADERS_ALLOC_FAILED when an internal request to allocate memory fails.]*/
        TEST_FUNCTION(HTTPHeaders_AddHeaderNameValuePair_with_same_Name_appends_fails_when_Map_AddOrUpdate_fails)
        {
            ///arrange
            CHTTPHeadersMocks mocks;
            auto httpHandle = HTTPHeaders_Alloc();
            STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(IGNORED_PTR_ARG, NAME1))
                .IgnoreArgument(1)
                .SetReturn((const char*)NULL); /*this key does not exist, the line below is adding it*/
            (void)HTTPHeaders_AddHeaderNameValuePair(httpHandle, NAME1, VALUE1);
            mocks.ResetAllCalls();

            STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(IGNORED_PTR_ARG, NAME1))
                .IgnoreArgument(1)
                .SetReturn(VALUE1);

            STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
                .IgnoreArgument(1);
            STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
                .IgnoreArgument(1);

            STRICT_EXPECTED_CALL(mocks, Map_AddOrUpdate(IGNORED_PTR_ARG, NAME1, VALUE1 ", " VALUE1))
                .IgnoreArgument(1)
                .SetReturn(MAP_ERROR);

            ///act
            auto res = HTTPHeaders_AddHeaderNameValuePair(httpHandle, NAME1, VALUE1);

            ///assert
            ASSERT_ARE_EQUAL(HTTP_HEADERS_RESULT, HTTP_HEADERS_ERROR, res);
            mocks.AssertActualAndExpectedCalls();

            ///cleanup
            HTTPHeaders_Free(httpHandle);
        }

        /*Tests_SRS_HTTP_HEADERS_99_015:[ The function shall return HTTP_HEADERS_ALLOC_FAILED when an internal request to allocate memory fails.]*/
        TEST_FUNCTION(HTTPHeaders_AddHeaderNameValuePair_with_same_Name_fails_when_gballoc_fails)
        {
            ///arrange
            CHTTPHeadersMocks mocks;
            auto httpHandle = HTTPHeaders_Alloc();
            STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(IGNORED_PTR_ARG, NAME1))
                .IgnoreArgument(1)
                .SetReturn((const char*)NULL); /*this key does not exist, the line below is adding it*/
            (void)HTTPHeaders_AddHeaderNameValuePair(httpHandle, NAME1, VALUE1);
            mocks.ResetAllCalls();

            STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(IGNORED_PTR_ARG, NAME1))
                .IgnoreArgument(1)
                .SetReturn(VALUE1);

            whenShallmalloc_fail = currentmalloc_call + 1;
            STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
                .IgnoreArgument(1);

            ///act
            auto res = HTTPHeaders_AddHeaderNameValuePair(httpHandle, NAME1, VALUE1);

            ///assert
            ASSERT_ARE_EQUAL(HTTP_HEADERS_RESULT, HTTP_HEADERS_ALLOC_FAILED, res);
            mocks.AssertActualAndExpectedCalls();

            ///cleanup
            HTTPHeaders_Free(httpHandle);
        }

        /*Tests_SRS_HTTP_HEADERS_99_012:[ Calling this API shall record a header from name and value parameters.]*/
        TEST_FUNCTION(HTTPHeaders_AddHeaderNameValuePair_add_two_headers_produces_two_headers)
        {
            ///arrange
            CHTTPHeadersMocks mocks;
            auto httpHandle = HTTPHeaders_Alloc();
            STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(IGNORED_PTR_ARG, NAME1))
                .IgnoreArgument(1)
                .SetReturn((const char*)NULL); /*this key does not exist, the line below is adding it*/
            (void)HTTPHeaders_AddHeaderNameValuePair(httpHandle, NAME1, VALUE1);
            mocks.ResetAllCalls();

            STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(IGNORED_PTR_ARG, NAME2))
                .IgnoreArgument(1)
                .SetReturn((const char*)NULL); /*this key does not exist, the line below is adding it*/

            STRICT_EXPECTED_CALL(mocks, Map_AddOrUpdate(IGNORED_PTR_ARG, NAME2, VALUE2))
                .IgnoreArgument(1);

            ///act
            auto res = HTTPHeaders_AddHeaderNameValuePair(httpHandle, NAME2, VALUE2);

            ///assert
            ASSERT_ARE_EQUAL(HTTP_HEADERS_RESULT, HTTP_HEADERS_OK, res);
            mocks.AssertActualAndExpectedCalls();

            ///cleanup
            HTTPHeaders_Free(httpHandle);
        }

        /*Tests_SRS_HTTP_HEADERS_99_017:[ If the name already exists in the collection of headers, the function shall concatenate the new value after the existing value, separated by a comma and a space as in: old-value+", "+new-value.]*/
        TEST_FUNCTION(HTTPHeaders_When_Second_Added_Header_Is_A_Substring_Of_An_Existing_Header_2_Headers_Are_Added)
        {
            ///arrange
            CHTTPHeadersMocks mocks;
            HTTP_HEADERS_HANDLE httpHandle = HTTPHeaders_Alloc();
            STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(IGNORED_PTR_ARG, "ab"))
                .IgnoreArgument(1)
                .SetReturn((const char*)NULL); /*this key does not exist, the line below is adding it*/
            (void)HTTPHeaders_AddHeaderNameValuePair(httpHandle, "ab", VALUE1);
            mocks.ResetAllCalls();
            
            STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(IGNORED_PTR_ARG, "a"))
                .IgnoreArgument(1)
                .SetReturn((const char*)NULL); /*this key does not exist, the line below is adding it*/

            STRICT_EXPECTED_CALL(mocks, Map_AddOrUpdate(IGNORED_PTR_ARG, "a", VALUE1))
                .IgnoreArgument(1);

            ///act
            HTTP_HEADERS_RESULT result = HTTPHeaders_AddHeaderNameValuePair(httpHandle, "a", VALUE1);

            ///assert
            ASSERT_ARE_EQUAL(HTTP_HEADERS_RESULT, HTTP_HEADERS_OK, result);
            mocks.AssertActualAndExpectedCalls();

            ///cleanup
            HTTPHeaders_Free(httpHandle);
        }

        /*Tests_SRS_HTTP_HEADERS_99_022:[ The return value shall be NULL if name parameter is NULL or if httpHeadersHandle is NULL]*/
        TEST_FUNCTION(HTTPHeaders_FindHeaderValue_with_NULL_handle_returns_NULL)
        {
            ///arrange
            CHTTPHeadersMocks mocks;

            ///act
            auto res = HTTPHeaders_FindHeaderValue(NULL, NAME1);

            ///assert
            ASSERT_IS_NULL(res);
            mocks.AssertActualAndExpectedCalls();
        }

        /*Tests_SRS_HTTP_HEADERS_99_022:[ The return value shall be NULL if name parameter is NULL or if httpHeadersHandle is NULL]*/
        TEST_FUNCTION(HTTPHeaders_FindHeaderValue_with_NULL_name_returns_NULL)
        {
            ///arrange
            CHTTPHeadersMocks mocks;
            auto httpHandle = HTTPHeaders_Alloc();
            mocks.ResetAllCalls();

            ///act
            auto res = HTTPHeaders_FindHeaderValue(httpHandle, NULL);

            ///assert
            ASSERT_IS_NULL(res);
            mocks.AssertActualAndExpectedCalls();

            ///cleanup
            HTTPHeaders_Free(httpHandle);
        }

        /*Tests_SRS_HTTP_HEADERS_99_018:[ Calling this API shall retrieve the value for a previously stored name.]*/
        /*Tests_SRS_HTTP_HEADERS_99_021:[ In this case the return value shall point to a string that shall strcmp equal to the original stored string.]*/
        TEST_FUNCTION(HTTPHeaders_FindHeaderValue_retrieves_previously_stored_value_succeeds)
        {
            ///arrange
            CHTTPHeadersMocks mocks;
            auto httpHandle = HTTPHeaders_Alloc();
            STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(IGNORED_PTR_ARG, NAME1))
                .IgnoreArgument(1)
                .SetReturn((const char*)NULL); /*this key does not exist, the line below is adding it*/
            (void)HTTPHeaders_AddHeaderNameValuePair(httpHandle, NAME1, VALUE1);
            mocks.ResetAllCalls();

            STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(IGNORED_PTR_ARG, NAME1))
                .IgnoreArgument(1);

            ///act
            auto res1 = HTTPHeaders_FindHeaderValue(httpHandle, NAME1);

            ///assert
            ASSERT_ARE_EQUAL(char_ptr, VALUE1, res1);
            mocks.AssertActualAndExpectedCalls();

            ///cleanup
            HTTPHeaders_Free(httpHandle);
        }

        /*Tests_SRS_HTTP_HEADERS_99_018:[ Calling this API shall retrieve the value for a previously stored name.]*/
        /*Tests_SRS_HTTP_HEADERS_99_021:[ In this case the return value shall point to a string that shall strcmp equal to the original stored string.]*/
        TEST_FUNCTION(HTTPHeaders_FindHeaderValue_retrieves_previously_stored_value_for_two_headers_succeeds)
        {
            ///arrange
            CHTTPHeadersMocks mocks;
            auto httpHandle = HTTPHeaders_Alloc();
            STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(IGNORED_PTR_ARG, NAME1))
                .IgnoreArgument(1)
                .SetReturn((const char*)NULL); /*this key does not exist, the line below is adding it*/
            (void)HTTPHeaders_AddHeaderNameValuePair(httpHandle, NAME1, VALUE1);
            STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(IGNORED_PTR_ARG, NAME2))
                .IgnoreArgument(1)
                .SetReturn((const char*)NULL); /*this key does not exist, the line below is adding it*/
            (void)HTTPHeaders_AddHeaderNameValuePair(httpHandle, NAME2, VALUE2);
            mocks.ResetAllCalls();

            STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(IGNORED_PTR_ARG, NAME1))
                .IgnoreArgument(1)
                .SetReturn(VALUE1);
            STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(IGNORED_PTR_ARG, NAME2))
                .IgnoreArgument(1)
                .SetReturn(VALUE2);

            ///act
            auto res1 = HTTPHeaders_FindHeaderValue(httpHandle, NAME1);
            auto res2 = HTTPHeaders_FindHeaderValue(httpHandle, NAME2);

            ///assert
            ASSERT_ARE_EQUAL(char_ptr, VALUE1, res1);
            ASSERT_ARE_EQUAL(char_ptr, VALUE2, res2);
            mocks.AssertActualAndExpectedCalls();

            ///cleanup
            HTTPHeaders_Free(httpHandle);
        }

        /*Tests_SRS_HTTP_HEADERS_99_018:[ Calling this API shall retrieve the value for a previously stored name.]*/
        /*Tests_SRS_HTTP_HEADERS_99_021:[ In this case the return value shall point to a string that shall strcmp equal to the original stored string.]*/
        TEST_FUNCTION(HTTPHeaders_FindHeaderValue_retrieves_concatenation_of_previously_stored_values_for_header_name_succeeds)
        {
            CHTTPHeadersMocks mocks;
            auto httpHandle = HTTPHeaders_Alloc();
            STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(IGNORED_PTR_ARG, NAME1))
                .IgnoreArgument(1)
                .SetReturn((const char*)NULL); /*this key does not exist, the line below is adding it*/
            (void)HTTPHeaders_AddHeaderNameValuePair(httpHandle, NAME1, VALUE1);
            STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(IGNORED_PTR_ARG, NAME1))
                .IgnoreArgument(1)
                .SetReturn(VALUE1); /*this key exists, was added above*/
            (void)HTTPHeaders_AddHeaderNameValuePair(httpHandle, NAME1, VALUE2);
            mocks.ResetAllCalls();

            STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(IGNORED_PTR_ARG, NAME1))
                .IgnoreArgument(1)
                .SetReturn(VALUE1 ", " VALUE2);

            ///act
            auto res = HTTPHeaders_FindHeaderValue(httpHandle, NAME1);

            ///assert
            ASSERT_ARE_EQUAL(char_ptr,VALUE1 ", " VALUE2, res);
            mocks.AssertActualAndExpectedCalls();

            ///cleanup
            HTTPHeaders_Free(httpHandle);
        }

        /*Tests_SRS_HTTP_HEADERS_99_020:[ The return value shall be different than NULL when the name matches the name of a previously stored name:value pair.]*/
        /*actually we are trying to see that finding a nonexisting value produces NULL*/
        TEST_FUNCTION(HTTPHeaders_FindHeaderValue_returns_NULL_for_nonexistent_value)
        {
            ///arrange
            CHTTPHeadersMocks mocks;
            auto httpHandle = HTTPHeaders_Alloc();
            STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(IGNORED_PTR_ARG, NAME1))
                .IgnoreArgument(1)
                .SetReturn((const char*)NULL); /*this key does not exist, the line below is adding it*/
            (void)HTTPHeaders_AddHeaderNameValuePair(httpHandle, NAME1, VALUE1);
            mocks.ResetAllCalls();

            STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(IGNORED_PTR_ARG, NAME2))
                .IgnoreArgument(1)
                .SetReturn((const char*)NULL);

            ///act
            auto res2 = HTTPHeaders_FindHeaderValue(httpHandle, NAME2);

            ///assert
            ASSERT_IS_NULL(res2);
            mocks.AssertActualAndExpectedCalls();

            ///cleanup
            HTTPHeaders_Free(httpHandle);
        }

        /*trying to catch some errors here*/
        /*Tests_SRS_HTTP_HEADERS_99_020:[ The return value shall be different than NULL when the name matches the name of a previously stored name:value pair.]*/
        TEST_FUNCTION(HTTPHeaders_FindHeaderValue_with_nonexistent_header_succeeds)
        {
            ///arrange
            CHTTPHeadersMocks mocks;
            auto httpHandle = HTTPHeaders_Alloc();
            STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(IGNORED_PTR_ARG, NAME1))
                .IgnoreArgument(1)
                .SetReturn((const char*)NULL); /*this key does not exist, the line below is adding it*/
            (void)HTTPHeaders_AddHeaderNameValuePair(httpHandle, NAME1, VALUE1);
            mocks.ResetAllCalls();

            STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(IGNORED_PTR_ARG, NAME1_TRICK1))
                .IgnoreArgument(1)
                .SetReturn((const char*)NULL);
            STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(IGNORED_PTR_ARG, NAME1_TRICK2))
                .IgnoreArgument(1)
                .SetReturn((const char*)NULL);
            STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(IGNORED_PTR_ARG, NAME1_TRICK3))
                .IgnoreArgument(1)
                .SetReturn((const char*)NULL);

            ///act
            auto res1 = HTTPHeaders_FindHeaderValue(httpHandle, NAME1_TRICK1);
            auto res2 = HTTPHeaders_FindHeaderValue(httpHandle, NAME1_TRICK2);
            auto res3 = HTTPHeaders_FindHeaderValue(httpHandle, NAME1_TRICK3);

            ///assert
            ASSERT_IS_NULL(res1);
            ASSERT_IS_NULL(res2);
            ASSERT_IS_NULL(res3);
            mocks.AssertActualAndExpectedCalls();

            ///cleanup
            HTTPHeaders_Free(httpHandle);
        }

        /* Tests_SRS_HTTP_HEADERS_06_001: [This API will perform exactly as HTTPHeaders_AddHeaderNameValuePair except that if the header name already exists the already existing value will be replaced as opposed to concatenated to.] */
        TEST_FUNCTION(HTTPHeaders_ReplaceHeaderNameValuePair_succeeds)
        {
            ///arrange
            CHTTPHeadersMocks mocks;
            auto httpHandle = HTTPHeaders_Alloc();
            STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(IGNORED_PTR_ARG, NAME1))
                .IgnoreArgument(1)
                .SetReturn((const char*)NULL); /*this key does not exist, the line below is adding it*/
            (void)HTTPHeaders_AddHeaderNameValuePair(httpHandle, NAME1, VALUE1);
            mocks.ResetAllCalls();

            STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(IGNORED_PTR_ARG, NAME1))
                .IgnoreArgument(1)
                .SetReturn(VALUE1); /*this key does not exist, the line below is adding it*/

            STRICT_EXPECTED_CALL(mocks, Map_AddOrUpdate(IGNORED_PTR_ARG, NAME1, VALUE2))
                .IgnoreArgument(1);

            ///act
            auto res = HTTPHeaders_ReplaceHeaderNameValuePair(httpHandle, NAME1, VALUE2);

            ///assert
            ASSERT_ARE_EQUAL(HTTP_HEADERS_RESULT, HTTP_HEADERS_OK, res);
            mocks.AssertActualAndExpectedCalls();

            ///cleanup
            HTTPHeaders_Free(httpHandle);
        }

        /* Tests_SRS_HTTP_HEADERS_06_001: [This API will perform exactly as HTTPHeaders_AddHeaderNameValuePair except that if the header name already exists the already existing value will be replaced as opposed to concatenated to.] */
        TEST_FUNCTION(HTTPHeaders_ReplaceHeaderNameValuePair_for_none_existing_header_succeeds)
        {
            ///arrange
            CHTTPHeadersMocks mocks;
            auto httpHandle = HTTPHeaders_Alloc();
            mocks.ResetAllCalls();

            STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(IGNORED_PTR_ARG, NAME1))
                .IgnoreArgument(1)
                .SetReturn((const char*)NULL);

            STRICT_EXPECTED_CALL(mocks, Map_AddOrUpdate(IGNORED_PTR_ARG, NAME1, VALUE2))
                .IgnoreArgument(1);

            ///act
            auto res = HTTPHeaders_ReplaceHeaderNameValuePair(httpHandle, NAME1, VALUE2);

            ///assert
            ASSERT_ARE_EQUAL(HTTP_HEADERS_RESULT, HTTP_HEADERS_OK, res);
            mocks.AssertActualAndExpectedCalls();

            ///cleanup
            HTTPHeaders_Free(httpHandle);
        }

        /*Tests_SRS_HTTP_HEADERS_99_024:[ The function shall return HTTP_HEADERS_INVALID_ARG when an invalid handle is passed.]*/
        TEST_FUNCTION(HTTPHeaders_GetHeaderCount_with_NULL_handle_fails)
        {
            ///arrange
            size_t nHeaders;

            ///act
            auto res1 = HTTPHeaders_GetHeaderCount(NULL, &nHeaders);

            ///assert
            ASSERT_ARE_EQUAL(HTTP_HEADERS_RESULT, HTTP_HEADERS_INVALID_ARG, res1);
        }

        /*Tests_SRS_HTTP_HEADERS_99_025:[ The function shall return HTTP_HEADERS_INVALID_ARG when headersCount is NULL.]*/
        TEST_FUNCTION(HTTPHeaders_GetHeaderCount_with_NULL_headersCount_fails)
        {
            ///arrange
            CHTTPHeadersMocks mocks;
            auto httpHandle = HTTPHeaders_Alloc();
            mocks.ResetAllCalls();

            ///act
            auto res1 = HTTPHeaders_GetHeaderCount(httpHandle, NULL);

            ///assert
            ASSERT_ARE_EQUAL(HTTP_HEADERS_RESULT, HTTP_HEADERS_INVALID_ARG, res1);
            mocks.AssertActualAndExpectedCalls();

            ///cleanup
            HTTPHeaders_Free(httpHandle);
        }

        /*Tests_SRS_HTTP_HEADERS_99_026:[ The function shall write in *headersCount the number of currently stored headers and shall return HTTP_HEADERS_OK]*/
        /*Tests_SRS_HTTP_HEADERS_99_023:[ Calling this API shall provide the number of stored headers.]*/
        /*Tests_SRS_HTTP_HEADERS_99_016:[ The function shall store the name:value pair in such a way that when later retrieved by a call to GetHeader it will return a string that shall strcmp equal to the name+": "+value.]*/
        TEST_FUNCTION(HTTPHeaders_GetHeaderCount_with_1_header_produces_1)
        {
            ///arrange
            CHTTPHeadersMocks mocks;
            auto httpHandle = HTTPHeaders_Alloc();
            STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(IGNORED_PTR_ARG, NAME1))
                .IgnoreArgument(1)
                .SetReturn((const char*)NULL); /*this key does not exist, the line below is adding it*/
            (void)HTTPHeaders_AddHeaderNameValuePair(httpHandle, NAME1, VALUE1);
            size_t nHeaders;
            const char* keys[] = { "NAME1" }; 
            const char* values[] = { "VALUE1" }; 
            const char* const ** pKeys = (const char* const **)&keys; 
            const char* const ** pValues = (const char* const **)&values;
            const size_t one = 1;
            mocks.ResetAllCalls();

            STRICT_EXPECTED_CALL(mocks, Map_GetInternals(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreArgument(1)
                .CopyOutArgumentBuffer(2, &pKeys, sizeof(pKeys))
                .CopyOutArgumentBuffer(3, &pValues, sizeof(pValues))
                .CopyOutArgumentBuffer(4, &one, sizeof(one));


            ///act
            auto res = HTTPHeaders_GetHeaderCount(httpHandle, &nHeaders);
            
            ///assert
            ASSERT_ARE_EQUAL(HTTP_HEADERS_RESULT, HTTP_HEADERS_OK, res);
            ASSERT_ARE_EQUAL(size_t, (size_t)1, nHeaders);
            mocks.AssertActualAndExpectedCalls();

            ///cleanup
            HTTPHeaders_Free(httpHandle);
        }

        /*Tests_SRS_HTTP_HEADERS_99_037:[ The function shall return HTTP_HEADERS_ERROR when an internal error occurs.]*/
        TEST_FUNCTION(HTTPHeaders_GetHeaderCount_fails_when_Map_GetInternals_fails)
        {
            ///arrange
            CHTTPHeadersMocks mocks;
            auto httpHandle = HTTPHeaders_Alloc();
            size_t nHeaders;
            mocks.ResetAllCalls();

            STRICT_EXPECTED_CALL(mocks, Map_GetInternals(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreAllArguments()
                .SetReturn(MAP_ERROR);

            ///act
            auto res = HTTPHeaders_GetHeaderCount(httpHandle, &nHeaders);

            ///assert
            ASSERT_ARE_EQUAL(HTTP_HEADERS_RESULT, HTTP_HEADERS_ERROR, res);
            mocks.AssertActualAndExpectedCalls();


            ///cleanup
            HTTPHeaders_Free(httpHandle);
        }

        /*Tests_SRS_HTTP_HEADERS_99_026:[ The function shall write in *headersCount the number of currently stored headers and shall return HTTP_HEADERS_OK]*/
        /*Tests_SRS_HTTP_HEADERS_99_023:[ Calling this API shall provide the number of stored headers.]*/
        TEST_FUNCTION(HTTPHeaders_GetHeaderCount_with_2_header_produces_2)
        {
            ///arrange
            CHTTPHeadersMocks mocks;
            auto httpHandle = HTTPHeaders_Alloc();
            STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(IGNORED_PTR_ARG, NAME1))
                .IgnoreArgument(1)
                .SetReturn((const char*)NULL); /*this key does not exist, the line below is adding it*/
            (void)HTTPHeaders_AddHeaderNameValuePair(httpHandle, NAME1, VALUE1);
            STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(IGNORED_PTR_ARG, NAME2))
                .IgnoreArgument(1)
                .SetReturn((const char*)NULL); /*this key does not exist, the line below is adding it*/
            (void)HTTPHeaders_AddHeaderNameValuePair(httpHandle, NAME2, VALUE2);
            size_t nHeaders;
            const char* keys[2] = { "NAME1", "NAME2" };
            const char* values[2] = { "VALUE1", "VALUE2" };
            const char* const ** pKeys = (const char* const **)&keys;
            const char* const ** pValues = (const char* const **)&values;

            const size_t two = 2;
            mocks.ResetAllCalls();


            STRICT_EXPECTED_CALL(mocks, Map_GetInternals(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreArgument(1)
                .CopyOutArgumentBuffer(2, &pKeys, sizeof(pKeys))
                .CopyOutArgumentBuffer(3, &pValues, sizeof(pValues))
                .CopyOutArgumentBuffer(4, &two, sizeof(two));

            ///act
            auto res = HTTPHeaders_GetHeaderCount(httpHandle, &nHeaders);

            ///assert
            ASSERT_ARE_EQUAL(HTTP_HEADERS_RESULT, HTTP_HEADERS_OK, res);
            ASSERT_ARE_EQUAL(size_t, 2, nHeaders);
            mocks.AssertActualAndExpectedCalls(); 

            ///cleanup
            HTTPHeaders_Free(httpHandle);
        }

        /*Tests_SRS_HTTP_HEADERS_99_028:[ The function shall return NULL if the handle is invalid.]*/
        TEST_FUNCTION(HTTPHeaders_GetHeader_with_NULL_handle_fails)
        {
            ///arrange
            char* headerValue;

            ///act
            auto res = HTTPHeaders_GetHeader(NULL, 0, &headerValue);

            ///assert
            ASSERT_ARE_EQUAL(HTTP_HEADERS_RESULT, HTTP_HEADERS_INVALID_ARG, res);
        }

        /*Tests_SRS_HTTP_HEADERS_99_032:[ The function shall return HTTP_HEADERS_INVALID_ARG if the destination  is NULL]*/
        TEST_FUNCTION(HTTPHeaders_GetHeader_with_NULL_buffer_fails)
        {
            ///arrange
            CHTTPHeadersMocks mocks;
            auto httpHandle = HTTPHeaders_Alloc();
            STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(IGNORED_PTR_ARG, NAME1))
                .IgnoreArgument(1)
                .SetReturn((const char*)NULL); /*this key does not exist, the line below is adding it*/
            (void)HTTPHeaders_AddHeaderNameValuePair(httpHandle, NAME1, VALUE1);
            mocks.ResetAllCalls();

            ///act
            auto res = HTTPHeaders_GetHeader(httpHandle, 0, NULL);

            ///assert
            ASSERT_ARE_EQUAL(HTTP_HEADERS_RESULT, HTTP_HEADERS_INVALID_ARG, res);
            mocks.AssertActualAndExpectedCalls();

            ///cleanup
            HTTPHeaders_Free(httpHandle);
        }

        /*Tests_SRS_HTTP_HEADERS_99_029:[ The function shall return HTTP_HEADERS_INVALID_ARG if index is not valid (for example, out of range) for the currently stored headers.]*/
        /*Tests that not adding something to the collection fails tso retrieve for index 0*/
        TEST_FUNCTION(HTTPHeaders_GetHeader_with_index_too_big_fails_1)
        {
            ///arrange
            CHTTPHeadersMocks mocks;
            auto httpHandle = HTTPHeaders_Alloc();
            char* headerValue;
            const char* thisIsNULL = NULL;
            const char* zero = 0;
            mocks.ResetAllCalls();

            STRICT_EXPECTED_CALL(mocks, Map_GetInternals(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreArgument(1)
                .CopyOutArgumentBuffer(2, &thisIsNULL, sizeof(thisIsNULL))
                .CopyOutArgumentBuffer(3, &thisIsNULL, sizeof(thisIsNULL))
                .CopyOutArgumentBuffer(4, &zero, sizeof(zero));

            ///act
            auto res1 = HTTPHeaders_GetHeader(httpHandle, 0, &headerValue);

            ///assert
            ASSERT_ARE_EQUAL(HTTP_HEADERS_RESULT, HTTP_HEADERS_INVALID_ARG, res1);
            mocks.AssertActualAndExpectedCalls();

            ///cleanup
            HTTPHeaders_Free(httpHandle);
        }

        /*Tests_SRS_HTTP_HEADERS_99_029:[ The function shall return HTTP_HEADERS_INVALID_ARG if index is not valid (for example, out of range) for the currently stored headers.]*/
        /*Tests that not adding something to the collection fails tso retrieve for index 0*/
        TEST_FUNCTION(HTTPHeaders_GetHeader_with_index_too_big_fails_2)
        {
            ///arrange
            CHTTPHeadersMocks mocks;
            auto httpHandle = HTTPHeaders_Alloc();
            STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(IGNORED_PTR_ARG, NAME1))
                .IgnoreArgument(1)
                .SetReturn((const char*)NULL); /*this key does not exist, the line below is adding it*/
            (void)HTTPHeaders_AddHeaderNameValuePair(httpHandle, NAME1, VALUE1);
            const char* keys[1] = { "NAME1" };
            const char* values[1] = { "VALUE1" };
            const char* const ** pKeys = (const char* const **)&keys;
            const char* const ** pValues = (const char* const **)&values;
            const size_t one = 1;
            mocks.ResetAllCalls();
            char* headerValue;

            STRICT_EXPECTED_CALL(mocks, Map_GetInternals(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreArgument(1)
                .CopyOutArgumentBuffer(2, pKeys, sizeof(pKeys))
                .CopyOutArgumentBuffer(3, pValues, sizeof(pValues))
                .CopyOutArgumentBuffer(4, &one, sizeof(one));

            ///act
            auto res1 = HTTPHeaders_GetHeader(httpHandle, 1, &headerValue);

            ///assert
            ASSERT_ARE_EQUAL(HTTP_HEADERS_RESULT, HTTP_HEADERS_INVALID_ARG, res1);
            mocks.AssertActualAndExpectedCalls();

            ///cleanup
            HTTPHeaders_Free(httpHandle);
        }

        /*Tests_SRS_HTTP_HEADERS_99_027:[ Calling this API shall produce the string value+": "+pair) for the index header in the buffer pointed to by buffer.]*/
        /*Tests_SRS_HTTP_HEADERS_99_035:[ The function shall return HTTP_HEADERS_OK when the function executed without error.]*/
        TEST_FUNCTION(HTTPHeaders_GetHeader_succeeds_1)
        {
            ///arrange
            CHTTPHeadersMocks mocks;
            auto httpHandle = HTTPHeaders_Alloc();
            STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(IGNORED_PTR_ARG, "a"))
                .IgnoreArgument(1)
                .SetReturn((const char*)NULL); /*this key does not exist, the line below is adding it*/
            (void)HTTPHeaders_AddHeaderNameValuePair(httpHandle, "a", "b");
            mocks.ResetAllCalls();
            char* headerValue;
            const char* keys[1] = { "a" };
            auto pKeys = &keys;
            const char* values[1] = { "b" };
            auto pValues = &values;
            const size_t one = 1;

            STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
                .IgnoreArgument(1);

            STRICT_EXPECTED_CALL(mocks, Map_GetInternals(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreArgument(1)
                .CopyOutArgumentBuffer(2, &pKeys, sizeof(pKeys))
                .CopyOutArgumentBuffer(3, &pValues, sizeof(pValues))
                .CopyOutArgumentBuffer(4, &one, sizeof(one));

            ///act
            auto res1 = HTTPHeaders_GetHeader(httpHandle, 0, &headerValue);

            ///assert
            ASSERT_ARE_EQUAL(HTTP_HEADERS_RESULT, HTTP_HEADERS_OK, res1);
            ASSERT_ARE_EQUAL(char_ptr, "a: b", headerValue);
            mocks.AssertActualAndExpectedCalls();

            ///cleanup
            HTTPHeaders_Free(httpHandle);
            free(headerValue);
        }

        /*Tests_SRS_HTTP_HEADERS_99_034:[ The function shall return HTTP_HEADERS_ERROR when an internal error occurs]*/
        TEST_FUNCTION(HTTPHeaders_GetHeader_fails_when_Map_GetInternals_fails)
        {
            ///arrange
            CHTTPHeadersMocks mocks;
            auto httpHandle = HTTPHeaders_Alloc();
            STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(IGNORED_PTR_ARG, "a"))
                .IgnoreArgument(1)
                .SetReturn((const char*)NULL); /*this key does not exist, the line below is adding it*/
            (void)HTTPHeaders_AddHeaderNameValuePair(httpHandle, "a", "b");
            mocks.ResetAllCalls();
            char* headerValue;
            const char* keys[1] = { "a" };
            auto pKeys = &keys;
            const char* values[1] = { "b" };
            auto pValues = &values;
            const size_t one = 1;

            STRICT_EXPECTED_CALL(mocks, Map_GetInternals(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreArgument(1)
                .CopyOutArgumentBuffer(2, &pKeys, sizeof(pKeys))
                .CopyOutArgumentBuffer(3, &pValues, sizeof(pValues))
                .CopyOutArgumentBuffer(4, &one, sizeof(one))
                .SetReturn(MAP_ERROR);

            ///act
            auto res1 = HTTPHeaders_GetHeader(httpHandle, 0, &headerValue);

            ///assert
            ASSERT_ARE_EQUAL(HTTP_HEADERS_RESULT, HTTP_HEADERS_ERROR, res1);
            mocks.AssertActualAndExpectedCalls();

            ///cleanup
            HTTPHeaders_Free(httpHandle);
        }

        /*Tests_SRS_HTTP_HEADERS_99_034:[ The function shall return HTTP_HEADERS_ERROR when an internal error occurs]*/
        TEST_FUNCTION(HTTPHeaders_GetHeader_succeeds_fails_when_malloc_fails)
        {
            ///arrange
            CHTTPHeadersMocks mocks;
            auto httpHandle = HTTPHeaders_Alloc();
            STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(IGNORED_PTR_ARG, "a"))
                .IgnoreArgument(1)
                .SetReturn((const char*)NULL); /*this key does not exist, the line below is adding it*/
            (void)HTTPHeaders_AddHeaderNameValuePair(httpHandle, "a", "b");
            mocks.ResetAllCalls();
            char* headerValue;
            const char* keys[1] = { "a" };
            auto pKeys = &keys;
            const char* values[1] = { "b" };
            auto pValues = &values;
            const size_t one = 1;

            STRICT_EXPECTED_CALL(mocks, Map_GetInternals(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreArgument(1)
                .CopyOutArgumentBuffer(2, &pKeys, sizeof(pKeys))
                .CopyOutArgumentBuffer(3, &pValues, sizeof(pValues))
                .CopyOutArgumentBuffer(4, &one, sizeof(one));

            whenShallmalloc_fail = currentmalloc_call + 1;
            STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
                .IgnoreArgument(1);

            ///act
            auto res1 = HTTPHeaders_GetHeader(httpHandle, 0, &headerValue);

            ///assert
            ASSERT_ARE_EQUAL(HTTP_HEADERS_RESULT, HTTP_HEADERS_ERROR, res1);
            mocks.AssertActualAndExpectedCalls();

            ///cleanup
            HTTPHeaders_Free(httpHandle);
            free(headerValue);
        }

        /*Tests_SRS_HTTP_HEADERS_99_031:[ If name contains the character ":" then the return value shall be HTTP_HEADERS_INVALID_ARG.]*/
        TEST_FUNCTION(HTTPHeaders_AddHeaderNameValuePair_with_colon_in_name_fails)
        {
            ///arrange
            CHTTPHeadersMocks mocks;
            auto httpHandle = HTTPHeaders_Alloc();
            mocks.ResetAllCalls();

            ///act
            auto res = HTTPHeaders_AddHeaderNameValuePair(httpHandle, "a:", "b");

            ///assert
            ASSERT_ARE_EQUAL(HTTP_HEADERS_RESULT, HTTP_HEADERS_INVALID_ARG, res);
            mocks.AssertActualAndExpectedCalls();

            ///cleanup
            HTTPHeaders_Free(httpHandle);
        }

        /*Tests_SRS_HTTP_HEADERS_99_031:[ If name contains the character ":" then the return value shall be HTTP_HEADERS_INVALID_ARG.]*/
        TEST_FUNCTION(HTTPHeaders_AddHeaderNameVAluePair_with_colon_in_value_succeeds_1)
        {
            ///arrange
            CHTTPHeadersMocks mocks;
            auto httpHandle = HTTPHeaders_Alloc();
            char* headerValue;
            STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(IGNORED_PTR_ARG, "a"))
                .IgnoreArgument(1)
                .SetReturn((const char*)NULL); /*this key does not exist, the line below is adding it*/
            (void)HTTPHeaders_AddHeaderNameValuePair(httpHandle, "a", ":");
            const char* keys[1] = { "a" };
            auto pKeys = &keys;
            const char* values[1] = { ":" };
            auto pValues = &values;
            const size_t one = 1;
            mocks.ResetAllCalls();

            STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
                .IgnoreArgument(1);

            STRICT_EXPECTED_CALL(mocks, Map_GetInternals(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreArgument(1)
                .CopyOutArgumentBuffer(2, &pKeys, sizeof(pKeys))
                .CopyOutArgumentBuffer(3, &pValues, sizeof(pValues))
                .CopyOutArgumentBuffer(4, &one, sizeof(one));

            ///act
            auto res1 = HTTPHeaders_GetHeader(httpHandle, 0, &headerValue);

            ///assert
            ASSERT_ARE_EQUAL(HTTP_HEADERS_RESULT, HTTP_HEADERS_OK, res1);
            ASSERT_ARE_EQUAL(char_ptr, "a: :", headerValue);
            mocks.AssertActualAndExpectedCalls();

            ///cleanup
            HTTPHeaders_Free(httpHandle);
            free(headerValue);

        }

        /*Tests_SRS_HTTP_HEADERS_99_036:[ If name contains the characters outside character codes 33 to 126 then the return value shall be HTTP_HEADERS_INVALID_ARG]*/
        /*so says http://tools.ietf.org/html/rfc822#section-3.1*/
        TEST_FUNCTION(HTTP_HEADERS_AddHeaderNameValuePair_fails_for_every_invalid_character)
        {
            ///arrange
            CNiceCallComparer<CHTTPHeadersMocks>mocks;
            auto httpHandle = HTTPHeaders_Alloc();
            char unacceptableString[2]={'\0', '\0'};
            
            for(int c=SCHAR_MIN;c <=SCHAR_MAX; c++)
            {
                if(c=='\0') continue;

                if((c<33) ||( 126<c)|| (c==':'))
                {
                    /*so it is an unacceptable character*/
                    unacceptableString[0]=(char)c;

                    ///act
                    auto res = HTTPHeaders_AddHeaderNameValuePair(httpHandle, unacceptableString, VALUE1);

                    ///assert
                    ASSERT_ARE_EQUAL(HTTP_HEADERS_RESULT, HTTP_HEADERS_INVALID_ARG, res);
                }
            }

            ///cleanup
            HTTPHeaders_Free(httpHandle);
        }

        /*Tests_SRS_HTTP_HEADERS_02_002: [The LWS from the beginning of the value shall not be stored.] */
        TEST_FUNCTION(HTTPHeaders_AddHeaderNameValuePair_with_LWS_value_stores_without_LWS_characters_succeeds)
        {
            ///arrange
            CHTTPHeadersMocks mocks;
            auto httpHandle = HTTPHeaders_Alloc();
            mocks.ResetAllCalls();

            STRICT_EXPECTED_CALL(mocks, Map_GetValueFromKey(IGNORED_PTR_ARG, NAME1))
                .IgnoreArgument(1)
                .SetReturn((const char*)NULL); /*this key does not exist*/

            STRICT_EXPECTED_CALL(mocks, Map_AddOrUpdate(IGNORED_PTR_ARG, NAME1, VALUE1))
                .IgnoreArgument(1);

            ///act
            auto res = HTTPHeaders_AddHeaderNameValuePair(httpHandle, NAME1, " \r\t\n" VALUE1); /*notice how there are some LWS characters in the value*/

            ///assert
            ASSERT_ARE_EQUAL(HTTP_HEADERS_RESULT, HTTP_HEADERS_OK, res);
            mocks.AssertActualAndExpectedCalls();

            //checking content

            ///cleanup
            HTTPHeaders_Free(httpHandle);
        }

        /*Tests_SRS_HTTP_HEADERS_02_003: [If handle is NULL then HTTPHeaders_Clone shall return NULL.] */
        TEST_FUNCTION(HTTPHEADERS_Clone_with_NULL_parameter_returns_NULL)
        {
            ///arrange
            ///act
            auto result= HTTPHeaders_Clone(NULL);

            ///assert
            ASSERT_IS_NULL(result);
            ///cleanup
        }

        /*Tests_SRS_HTTP_HEADERS_02_004: [Otherwise HTTPHeaders_Clone shall clone the content of handle to a new handle.*/
        TEST_FUNCTION(HTTPHEADERS_Clone_happy_path)
        {
            ///arrange
            CHTTPHeadersMocks mocks;
            auto source = HTTPHeaders_Alloc();
            mocks.ResetAllCalls();

            STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
                .IgnoreArgument(1);
            STRICT_EXPECTED_CALL(mocks, Map_Clone(IGNORED_PTR_ARG))
                .IgnoreArgument(1);

            ///act
            auto result = HTTPHeaders_Clone(source);

            ///assert
            ASSERT_IS_NOT_NULL(result);
            mocks.AssertActualAndExpectedCalls();

            ///cleanup
            HTTPHeaders_Free(source);
            HTTPHeaders_Free(result);
        }

        /*Tests_SRS_HTTP_HEADERS_02_005: [If cloning fails for any reason, then HTTPHeaders_Clone shall return NULL.] */
        TEST_FUNCTION(HTTPHEADERS_Clone_fails_when_map_clone_fails)
        {
            ///arrange
            CHTTPHeadersMocks mocks;
            auto source = HTTPHeaders_Alloc();
            mocks.ResetAllCalls();

            STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
                .IgnoreArgument(1);
            STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
                .IgnoreArgument(1);

            whenShallMap_Clone_fail = currentMap_Clone_call + 1;
            STRICT_EXPECTED_CALL(mocks, Map_Clone(IGNORED_PTR_ARG))
                .IgnoreArgument(1);

            ///act
            auto result = HTTPHeaders_Clone(source);

            ///assert
            ASSERT_IS_NULL(result);
            mocks.AssertActualAndExpectedCalls();

            ///cleanup
            HTTPHeaders_Free(source);
            HTTPHeaders_Free(result);
        }

        /*Tests_SRS_HTTP_HEADERS_02_005: [If cloning fails for any reason, then HTTPHeaders_Clone shall return NULL.] */
        TEST_FUNCTION(HTTPHEADERS_Clone_fails_when_gballoc_fails)
        {
            ///arrange
            CHTTPHeadersMocks mocks;
            auto source = HTTPHeaders_Alloc();
            mocks.ResetAllCalls();

            whenShallmalloc_fail = currentmalloc_call + 1;
            STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
                .IgnoreArgument(1);

            ///act
            auto result = HTTPHeaders_Clone(source);

            ///assert
            ASSERT_IS_NULL(result);
            mocks.AssertActualAndExpectedCalls();

            ///cleanup
            HTTPHeaders_Free(source);
            HTTPHeaders_Free(result);
        }


END_TEST_SUITE(HTTPHeaders_UnitTests)
