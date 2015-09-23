// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <cstdlib>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "testrunnerswitcher.h"
#include "micromock.h"
#include "micromockcharstararenullterminatedstrings.h"
#include "map.h"
#include "lock.h"

static MICROMOCK_MUTEX_HANDLE g_testByTest;
static MICROMOCK_GLOBAL_SEMAPHORE_HANDLE g_dllByDll;

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

static size_t currentmalloc_call;
static size_t whenShallmalloc_fail;

static size_t currentrealloc_call;
static size_t whenShallrealloc_fail;

DEFINE_MICROMOCK_ENUM_TO_STRING(MAP_RESULT, MAP_RESULT_VALUES);

static int DontAllowCapitalsFilters(const char* mapProperty, const char* mapValue)
{
    int result = 0;
    const char* iterator = mapProperty;
    while (iterator != NULL && *iterator != '\0')
    {
        if (*iterator >= 'A' && *iterator <= 'Z')
        {
            result = __LINE__;
            break;
        }
        iterator++;
    }

    if (result != 0)
    {
        iterator = mapValue;
        while (iterator != NULL && *iterator != '\0')
        {
            if (*iterator >= 'A' && *iterator <= 'Z')
            {
                result = __LINE__;
                break;
            }
            iterator++;
        }
    }

    return result;
}

TYPED_MOCK_CLASS(CMapMocks, CGlobalMock)
{
public:

    MOCK_STATIC_METHOD_1(, void*, gballoc_malloc, size_t, size)
        void* result2;
    currentmalloc_call++;
    if (whenShallmalloc_fail>0)
    {
        if (currentmalloc_call == whenShallmalloc_fail)
        {
            result2 = NULL;
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
            result2 = NULL;
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

DECLARE_GLOBAL_MOCK_METHOD_1(CMapMocks, , void*, gballoc_malloc, size_t, size);

DECLARE_GLOBAL_MOCK_METHOD_2(CMapMocks, , void*, gballoc_realloc, void*, ptr, size_t, size);
DECLARE_GLOBAL_MOCK_METHOD_1(CMapMocks, , void, gballoc_free, void*, ptr);

/* capacity */

static const char* TEST_REDKEY = "testRedKey";
static const char* TEST_REDVALUE = "testRedValue";

static const char* TEST_YELLOWKEY = "testYellowKey";
static const char* TEST_YELLOWVALUE = "testYellowValue";

static const char* TEST_BLUEKEY = "testBlueKey";
static const char* TEST_BLUEVALUE = "cyan";

static const char* TEST_GREENKEY = "testgreenkey";
static const char* TEST_GREENVALUE = "green";

BEGIN_TEST_SUITE(map_unittests)

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

    /*Tests_SRS_MAP_02_001: [Map_Create shall create a new, empty map.]*/ /*this tests "create"*/
    /*Tests_SRS_MAP_02_003: [Otherwise, it shall return a non-NULL handle that can be used in subsequent calls.] */
    /*Tests_SRS_MAP_02_004: [Map_Destroy shall release all resources associated with the map.] */
    TEST_FUNCTION(Map_Create_Destroy_succeeds)
    {
        ///arrange
        CMapMocks mocks;
        
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*keys*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*values*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*handleData*/
            .IgnoreArgument(1);

        ///act
        auto handle = Map_Create(NULL);
        Map_Destroy(handle);

        ///assert

        ///cleanup
    }

    /*Tests_SRS_MAP_02_004: [Map_Destroy shall release all resources associated with the map.] */
    TEST_FUNCTION(Map_Destroy_on_non_empty_map_succeeds_1)
    {
        ///arrange
        CMapMocks mocks;
        auto handle = Map_Create(NULL);
        Map_AddOrUpdate(handle, TEST_REDKEY, TEST_REDVALUE);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*free the red key*/
            .ValidateArgumentBuffer(1, TEST_REDKEY, strlen(TEST_REDKEY)+1);

        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))/*free the red value*/
            .ValidateArgumentBuffer(1, TEST_REDVALUE, strlen(TEST_REDVALUE)+1);

        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*free keys array*/
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*free values array*/
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*free handle*/
            .IgnoreArgument(1);

        ///act
        Map_Destroy(handle);

        ///assert
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
    }

    /*Tests_SRS_MAP_02_004: [Map_Destroy shall release all resources associated with the map.] */
    TEST_FUNCTION(Map_Destroy_on_non_empty_map_succeeds_2)
    {
        ///arrange
        CMapMocks mocks;
        auto handle = Map_Create(NULL);
        Map_AddOrUpdate(handle, TEST_REDKEY, TEST_REDVALUE);
        Map_AddOrUpdate(handle, TEST_REDKEY, "a"); /*overwrites to something smaller*/
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*free the red key*/
            .ValidateArgumentBuffer(1, TEST_REDKEY, strlen(TEST_REDKEY)+1);

        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))/*free the red value*/
            .ValidateArgumentBuffer(1, "a", 2);

        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*free keys array*/
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*free values array*/
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*free handle*/
            .IgnoreArgument(1);

        ///act
        Map_Destroy(handle);

        ///assert
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
    }

    /*Tests_SRS_MAP_02_001: [Map_Create shall create a new, empty map.]*/ /*this tests "empty"*/
    TEST_FUNCTION(Map_Create_Destroy_succeeds_2)
    {
        ///arrange
        CMapMocks mocks;
        auto handle = Map_Create(NULL);
        mocks.ResetAllCalls();
        const char*const* keys;
        const char*const* values;
        size_t count;

        ///act
        auto result = Map_GetInternals(handle, &keys, &values, &count);

        ///assert
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_OK, result);
        ASSERT_ARE_EQUAL(size_t, 0, count);
        ASSERT_ARE_EQUAL(void_ptr, NULL, keys);
        ASSERT_ARE_EQUAL(void_ptr, NULL, values);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        Map_Destroy(handle);
    }

    /*Tests_SRS_MAP_02_002: [If during creation there are any error, then Map_Create shall return NULL.]*/
    TEST_FUNCTION(Map_Create_fails_when_malloc_fails)
    {
        ///arrange
        CMapMocks mocks;
        whenShallmalloc_fail = 1;
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);

        ///act
        auto handle = Map_Create(NULL);

        ///assert
        ASSERT_IS_NULL(handle);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        Map_Destroy(handle);
    }

    /*Tests_SRS_MAP_02_005: [If parameter handle is NULL then Map_Destroy shall take no action.]*/
    TEST_FUNCTION(Map_Destroy_with_NULL_argument_does_nothing)
    {
        ///arrange
        CMapMocks mocks;

        ///act
        Map_Destroy(NULL);

        ///assert
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
    }

    /*Tests_SRS_MAP_02_006: [If parameter handle is NULL then Map_Add shall return MAP_INVALID_ARG.]*/
    TEST_FUNCTION(Map_Add_with_NULL_parameter_handle_fails)
    {
        ///arrange
        CMapMocks mocks;
        auto handle = Map_Create(NULL);
        mocks.ResetAllCalls();

        ///act
        auto result= Map_Add(NULL, TEST_REDKEY, TEST_REDVALUE);

        ///assert
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_INVALIDARG, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        Map_Destroy(handle);
    }

    /*Tests_SRS_MAP_02_007: [If parameter key is NULL then Map_Add shall return MAP_INVALID_ARG.]*/
    TEST_FUNCTION(Map_Add_with_NULL_parameter_key_fails)
    {
        ///arrange
        CMapMocks mocks;
        auto handle = Map_Create(NULL);
        mocks.ResetAllCalls();

        ///act
        auto result = Map_Add(handle, NULL, TEST_REDVALUE);

        ///assert
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_INVALIDARG, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        Map_Destroy(handle);
    }

    /*Tests_SRS_MAP_02_008: [If parameter value is NULL then Map_Add shall return MAP_INVALID_ARG.]*/
    TEST_FUNCTION(Map_Add_with_NULL_parameter_value_fails)
    {
        ///arrange
        CMapMocks mocks;
        auto handle = Map_Create(NULL);
        mocks.ResetAllCalls();

        ///act
        auto result = Map_Add(handle, TEST_REDKEY, NULL);

        ///assert
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_INVALIDARG, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        Map_Destroy(handle);
    }

    /*Tests_SRS_MAP_02_010: [Otherwise, Map_Add shall add the pair <key,value> to the map.] */
    /*Tests_SRS_MAP_02_012: [Otherwise, Map_Add shall return MAP_OK.] */
    TEST_FUNCTION(Map_Add_succeeds_1)
    {
        ///arrange
        CMapMocks mocks;
        const char*const* keys;
        const char*const* values;
        size_t count;
        auto handle = Map_Create(NULL);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, gballoc_realloc(NULL, sizeof(const char*))); /*growing keys*/

        STRICT_EXPECTED_CALL(mocks, gballoc_realloc(NULL, sizeof(const char*))); /*growing values*/

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_REDKEY) + 1)); /*copy of red key*/

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_REDVALUE) + 1)); /*copy of red value*/

        ///act
        auto result1 = Map_Add(handle, TEST_REDKEY, TEST_REDVALUE);
        auto result2 = Map_GetInternals(handle, &keys, &values, &count);

        ///assert
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_OK, result1);
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_OK, result2);
        ASSERT_ARE_EQUAL(size_t, 1, count);
        ASSERT_ARE_EQUAL(char_ptr, TEST_REDKEY, keys[0]);
        ASSERT_ARE_EQUAL(char_ptr, TEST_REDVALUE, values[0]);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        Map_Destroy(handle);
    }

    /*Tests_SRS_MAP_02_010: [Otherwise, Map_Add shall add the pair <key,value> to the map.] */
    /*Tests_SRS_MAP_02_012: [Otherwise, Map_Add shall return MAP_OK.] */
    /*Tests_SRS_MAP_02_043: [Map_GetInternals shall produce in *keys an pointer to an array of const char* having all the keys stored so far by the map.]*/
    /*Tests_SRS_MAP_02_044: [Map_GetInternals shall produce in *values a pointer to an array of const char* having all the values stored so far by the map.]*/
    /*Tests_SRS_MAP_02_045: [Map_GetInternals shall produce in *count the number of stored keys and values.]*/
    TEST_FUNCTION(Map_Add_succeeds_2)
    {
        ///arrange
        CMapMocks mocks;
        const char*const* keys;
        const char*const* values;
        size_t count;
        auto handle = Map_Create(NULL);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, gballoc_realloc(NULL, sizeof(const char*))); /*growing keys*/

        STRICT_EXPECTED_CALL(mocks, gballoc_realloc(NULL, sizeof(const char*))); /*growing values*/

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_REDKEY) + 1)); /*copy of red key*/

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_REDVALUE) + 1)); /*copy of red value*/

        STRICT_EXPECTED_CALL(mocks, gballoc_realloc(IGNORED_PTR_ARG, 2*sizeof(const char*))) /*growing keys*/
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, gballoc_realloc(IGNORED_PTR_ARG, 2*sizeof(const char*))) /*growing values*/
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_BLUEKEY) + 1)); /*copy of blue key*/

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_BLUEVALUE) + 1)); /*copy of blue value*/

        ///act
        auto result1 = Map_Add(handle, TEST_REDKEY, TEST_REDVALUE);
        auto result2 = Map_Add(handle, TEST_BLUEKEY, TEST_BLUEVALUE);
        auto result3 = Map_GetInternals(handle, &keys, &values, &count);

        ///assert
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_OK, result1);
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_OK, result2);
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_OK, result3);
        ASSERT_ARE_EQUAL(size_t, 2, count);
        ASSERT_ARE_EQUAL(char_ptr, TEST_REDKEY, keys[0]);
        ASSERT_ARE_EQUAL(char_ptr, TEST_REDVALUE, values[0]);
        ASSERT_ARE_EQUAL(char_ptr, TEST_BLUEKEY, keys[1]);
        ASSERT_ARE_EQUAL(char_ptr, TEST_BLUEVALUE, values[1]);

        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        Map_Destroy(handle);
    }

    /*Tests_SRS_MAP_02_011: [If adding the pair <key,value> fails then Map_Add shall return MAP_ERROR.] */
    TEST_FUNCTION(Map_Add_fails_when_gballoc_fails_1)
    {
        ///arrange
        CMapMocks mocks;
        const char*const* keys;
        const char*const* values;
        size_t count;
        auto handle = Map_Create(NULL);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, gballoc_realloc(NULL, sizeof(const char*))); /*growing keys*/

        STRICT_EXPECTED_CALL(mocks, gballoc_realloc(NULL, sizeof(const char*))); /*growing values*/

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_REDKEY) + 1)); /*copy of red key*/

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_REDVALUE) + 1)); /*copy of red value*/

        STRICT_EXPECTED_CALL(mocks, gballoc_realloc(IGNORED_PTR_ARG, 2 * sizeof(const char*))) /*growing keys*/
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, gballoc_realloc(IGNORED_PTR_ARG, 2 * sizeof(const char*))) /*growing values*/
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_BLUEKEY) + 1)); /*copy of blue key*/
        
        whenShallmalloc_fail =currentmalloc_call+ 4;
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_BLUEVALUE) + 1)); /*copy of blue value*/

        /*below are undo actions*/
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*undo copy of blue key*/
            .ValidateArgumentBuffer(1, TEST_BLUEKEY, strlen(TEST_BLUEKEY) + 1);
        STRICT_EXPECTED_CALL(mocks, gballoc_realloc(IGNORED_PTR_ARG, 1 * sizeof(const char*))) /*undo growing keys*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_realloc(IGNORED_PTR_ARG, 1 * sizeof(const char*))) /*undo growing values*/
            .IgnoreArgument(1);


        ///act
        auto result1 = Map_Add(handle, TEST_REDKEY, TEST_REDVALUE);
        auto result2 = Map_Add(handle, TEST_BLUEKEY, TEST_BLUEVALUE);
        auto result3 = Map_GetInternals(handle, &keys, &values, &count);

        ///assert
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_OK, result1);
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_ERROR, result2);
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_OK, result3);
        ASSERT_ARE_EQUAL(size_t, 1, count);
        ASSERT_ARE_EQUAL(char_ptr, TEST_REDKEY, keys[0]);
        ASSERT_ARE_EQUAL(char_ptr, TEST_REDVALUE, values[0]);

        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        Map_Destroy(handle);
    }

    /*Tests_SRS_MAP_02_011: [If adding the pair <key,value> fails then Map_Add shall return MAP_ERROR.] */
    TEST_FUNCTION(Map_Add_fails_when_gballoc_fails_2)
    {
        ///arrange
        CMapMocks mocks;
        const char*const* keys;
        const char*const* values;
        size_t count;
        auto handle = Map_Create(NULL);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, gballoc_realloc(NULL, sizeof(const char*))); /*growing keys*/

        STRICT_EXPECTED_CALL(mocks, gballoc_realloc(NULL, sizeof(const char*))); /*growing values*/

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_REDKEY) + 1)); /*copy of red key*/

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_REDVALUE) + 1)); /*copy of red value*/

        STRICT_EXPECTED_CALL(mocks, gballoc_realloc(IGNORED_PTR_ARG, 2 * sizeof(const char*))) /*growing keys*/
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, gballoc_realloc(IGNORED_PTR_ARG, 2 * sizeof(const char*))) /*growing values*/
            .IgnoreArgument(1);

        whenShallmalloc_fail = currentmalloc_call + 3;
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_BLUEKEY) + 1)); /*copy of blue key*/

        /*below are undo actions*/
        STRICT_EXPECTED_CALL(mocks, gballoc_realloc(IGNORED_PTR_ARG, 1 * sizeof(const char*))) /*undo growing keys*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_realloc(IGNORED_PTR_ARG, 1 * sizeof(const char*))) /*undo growing values*/
            .IgnoreArgument(1);


        ///act
        auto result1 = Map_Add(handle, TEST_REDKEY, TEST_REDVALUE);
        auto result2 = Map_Add(handle, TEST_BLUEKEY, TEST_BLUEVALUE);
        auto result3 = Map_GetInternals(handle, &keys, &values, &count);

        ///assert
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_OK, result1);
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_ERROR, result2);
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_OK, result3);
        ASSERT_ARE_EQUAL(size_t, 1, count);
        ASSERT_ARE_EQUAL(char_ptr, TEST_REDKEY, keys[0]);
        ASSERT_ARE_EQUAL(char_ptr, TEST_REDVALUE, values[0]);

        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        Map_Destroy(handle);
    }

    /*Tests_SRS_MAP_02_011: [If adding the pair <key,value> fails then Map_Add shall return MAP_ERROR.] */
    TEST_FUNCTION(Map_Add_fails_when_gballoc_fails_3)
    {
        ///arrange
        CMapMocks mocks;
        const char*const* keys;
        const char*const* values;
        size_t count;
        auto handle = Map_Create(NULL);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, gballoc_realloc(NULL, sizeof(const char*))); /*growing keys*/

        STRICT_EXPECTED_CALL(mocks, gballoc_realloc(NULL, sizeof(const char*))); /*growing values*/

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_REDKEY) + 1)); /*copy of red key*/

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_REDVALUE) + 1)); /*copy of red value*/

        STRICT_EXPECTED_CALL(mocks, gballoc_realloc(IGNORED_PTR_ARG, 2 * sizeof(const char*))) /*growing keys*/
            .IgnoreArgument(1);

        whenShallrealloc_fail = currentrealloc_call + 4;
        STRICT_EXPECTED_CALL(mocks, gballoc_realloc(IGNORED_PTR_ARG, 2 * sizeof(const char*))) /*growing values*/
            .IgnoreArgument(1);

        /*below are undo actions*/
        STRICT_EXPECTED_CALL(mocks, gballoc_realloc(IGNORED_PTR_ARG, 1 * sizeof(const char*))) /*undo growing keys*/
            .IgnoreArgument(1);

        ///act
        auto result1 = Map_Add(handle, TEST_REDKEY, TEST_REDVALUE);
        auto result2 = Map_Add(handle, TEST_BLUEKEY, TEST_BLUEVALUE);
        auto result3 = Map_GetInternals(handle, &keys, &values, &count);

        ///assert
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_OK, result1);
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_ERROR, result2);
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_OK, result3);
        ASSERT_ARE_EQUAL(size_t, 1, count);
        ASSERT_ARE_EQUAL(char_ptr, TEST_REDKEY, keys[0]);
        ASSERT_ARE_EQUAL(char_ptr, TEST_REDVALUE, values[0]);

        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        Map_Destroy(handle);
    }

    /*Tests_SRS_MAP_02_011: [If adding the pair <key,value> fails then Map_Add shall return MAP_ERROR.] */
    TEST_FUNCTION(Map_Add_fails_when_gballoc_fails_4)
    {
        ///arrange
        CMapMocks mocks;
        const char*const* keys;
        const char*const* values;
        size_t count;
        auto handle = Map_Create(NULL);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, gballoc_realloc(NULL, sizeof(const char*))); /*growing keys*/

        STRICT_EXPECTED_CALL(mocks, gballoc_realloc(NULL, sizeof(const char*))); /*growing values*/

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_REDKEY) + 1)); /*copy of red key*/

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_REDVALUE) + 1)); /*copy of red value*/

        whenShallrealloc_fail = currentrealloc_call + 3;
        STRICT_EXPECTED_CALL(mocks, gballoc_realloc(IGNORED_PTR_ARG, 2 * sizeof(const char*))) /*growing keys*/
            .IgnoreArgument(1);

        /*below are undo actions*/

        ///act
        auto result1 = Map_Add(handle, TEST_REDKEY, TEST_REDVALUE);
        auto result2 = Map_Add(handle, TEST_BLUEKEY, TEST_BLUEVALUE);
        auto result3 = Map_GetInternals(handle, &keys, &values, &count);

        ///assert
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_OK, result1);
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_ERROR, result2);
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_OK, result3);
        ASSERT_ARE_EQUAL(size_t, 1, count);
        ASSERT_ARE_EQUAL(char_ptr, TEST_REDKEY, keys[0]);
        ASSERT_ARE_EQUAL(char_ptr, TEST_REDVALUE, values[0]);

        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        Map_Destroy(handle);
    }

    /*Tests_SRS_MAP_02_011: [If adding the pair <key,value> fails then Map_Add shall return MAP_ERROR.] */
    TEST_FUNCTION(Map_Add_fails_when_gballoc_fails_5)
    {
        ///arrange
        CMapMocks mocks;
        const char*const* keys;
        const char*const* values;
        size_t count;
        auto handle = Map_Create(NULL);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, gballoc_realloc(NULL, sizeof(const char*))); /*growing keys*/

        STRICT_EXPECTED_CALL(mocks, gballoc_realloc(NULL, sizeof(const char*))); /*growing values*/

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_REDKEY) + 1)); /*copy of red key*/

        whenShallmalloc_fail = currentmalloc_call + 2;
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_REDVALUE) + 1)); /*copy of red value*/

        /*below are undo actions*/
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*undo copy of red key*/
            .ValidateArgumentBuffer(1, TEST_REDKEY, strlen(TEST_REDKEY) + 1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*undo growing keys*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))/*undo growing values*/
            .IgnoreArgument(1);

        ///act
        auto result1 = Map_Add(handle, TEST_REDKEY, TEST_REDVALUE);
        auto result3 = Map_GetInternals(handle, &keys, &values, &count);

        ///assert
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_ERROR, result1);
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_OK, result3);
        ASSERT_ARE_EQUAL(size_t, 0, count);

        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        Map_Destroy(handle);
    }

    /*Tests_SRS_MAP_02_011: [If adding the pair <key,value> fails then Map_Add shall return MAP_ERROR.] */
    TEST_FUNCTION(Map_Add_fails_when_gballoc_fails_6)
    {
        ///arrange
        CMapMocks mocks;
        const char*const* keys;
        const char*const* values;
        size_t count;
        auto handle = Map_Create(NULL);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, gballoc_realloc(NULL, sizeof(const char*))); /*growing keys*/

        STRICT_EXPECTED_CALL(mocks, gballoc_realloc(NULL, sizeof(const char*))); /*growing values*/

        whenShallmalloc_fail = currentmalloc_call + 1;
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_REDKEY) + 1)); /*copy of red key*/

        /*below are undo actions*/
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*undo growing keys*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))/*undo growing values*/
            .IgnoreArgument(1);

        ///act
        auto result1 = Map_Add(handle, TEST_REDKEY, TEST_REDVALUE);
        auto result3 = Map_GetInternals(handle, &keys, &values, &count);

        ///assert
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_ERROR, result1);
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_OK, result3);
        ASSERT_ARE_EQUAL(size_t, 0, count);

        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        Map_Destroy(handle);
    }

    /*Tests_SRS_MAP_02_011: [If adding the pair <key,value> fails then Map_Add shall return MAP_ERROR.] */
    TEST_FUNCTION(Map_Add_fails_when_gballoc_fails_7)
    {
        ///arrange
        CMapMocks mocks;
        const char*const* keys;
        const char*const* values;
        size_t count;
        auto handle = Map_Create(NULL);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, gballoc_realloc(NULL, sizeof(const char*))); /*growing keys*/

        whenShallrealloc_fail = currentrealloc_call + 2;
        STRICT_EXPECTED_CALL(mocks, gballoc_realloc(NULL, sizeof(const char*))); /*growing values*/

        /*below are undo actions*/
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*undo growing keys*/
            .IgnoreArgument(1);

        ///act
        auto result1 = Map_Add(handle, TEST_REDKEY, TEST_REDVALUE);
        auto result3 = Map_GetInternals(handle, &keys, &values, &count);

        ///assert
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_ERROR, result1);
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_OK, result3);
        ASSERT_ARE_EQUAL(size_t, 0, count);

        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        Map_Destroy(handle);
    }

    /*Tests_SRS_MAP_02_011: [If adding the pair <key,value> fails then Map_Add shall return MAP_ERROR.] */
    TEST_FUNCTION(Map_Add_fails_when_gballoc_fails_8)
    {
        ///arrange
        CMapMocks mocks;
        const char*const* keys;
        const char*const* values;
        size_t count;
        auto handle = Map_Create(NULL);
        mocks.ResetAllCalls();

        whenShallrealloc_fail = currentrealloc_call + 1;
        STRICT_EXPECTED_CALL(mocks, gballoc_realloc(NULL, sizeof(const char*))); /*growing keys*/

        /*below are undo actions*/ /*none*/

        ///act
        auto result1 = Map_Add(handle, TEST_REDKEY, TEST_REDVALUE);
        auto result3 = Map_GetInternals(handle, &keys, &values, &count);

        ///assert
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_ERROR, result1);
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_OK, result3);
        ASSERT_ARE_EQUAL(size_t, 0, count);

        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        Map_Destroy(handle);
    }

    /*Tests_SRS_MAP_02_009: [If the key already exists, then Map_Add shall return MAP_KEYEXISTS.]*/
    TEST_FUNCTION(Map_Add_with_existing_key_fails_1)
    {
        ///arrange
        CMapMocks mocks;
        const char*const* keys;
        const char*const* values;
        size_t count;
        auto handle = Map_Create(NULL);
        (void)Map_Add(handle, TEST_REDKEY, TEST_REDVALUE);
        mocks.ResetAllCalls();

        /*below are undo actions*/ /*none*/

        ///act
        auto result1 = Map_Add(handle, TEST_REDKEY, TEST_REDVALUE);
        auto result3 = Map_GetInternals(handle, &keys, &values, &count);

        ///assert
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_KEYEXISTS, result1);
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_OK, result3);
        ASSERT_ARE_EQUAL(size_t, 1, count);
        ASSERT_ARE_EQUAL(char_ptr, TEST_REDKEY, keys[0]);
        ASSERT_ARE_EQUAL(char_ptr, TEST_REDVALUE, values[0]);

        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        Map_Destroy(handle);
    }

    /*Tests_SRS_MAP_02_009: [If the key already exists, then Map_Add shall return MAP_KEYEXISTS.]*/
    TEST_FUNCTION(Map_Add_with_existing_key_fails_2)
    {
        ///arrange
        CMapMocks mocks;
        const char*const* keys;
        const char*const* values;
        size_t count;
        auto handle = Map_Create(NULL);
        (void)Map_Add(handle, TEST_REDKEY, TEST_REDVALUE);
        (void)Map_Add(handle, TEST_BLUEKEY, TEST_BLUEVALUE);
        mocks.ResetAllCalls();

        /*below are undo actions*/ /*none*/

        ///act
        auto result1 = Map_Add(handle, TEST_BLUEKEY, TEST_YELLOWVALUE);
        auto result3 = Map_GetInternals(handle, &keys, &values, &count);

        ///assert
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_KEYEXISTS, result1);
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_OK, result3);
        ASSERT_ARE_EQUAL(size_t, 2, count);
        ASSERT_ARE_EQUAL(char_ptr, TEST_REDKEY, keys[0]);
        ASSERT_ARE_EQUAL(char_ptr, TEST_REDVALUE, values[0]);
        ASSERT_ARE_EQUAL(char_ptr, TEST_BLUEKEY, keys[1]);
        ASSERT_ARE_EQUAL(char_ptr, TEST_BLUEVALUE, values[1]);

        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        Map_Destroy(handle);
    }

    /*Tests_SRS_MAP_02_013: [If parameter handle is NULL then Map_AddOrUpdate shall return MAP_INVALID_ARG.]*/
    TEST_FUNCTION(Map_AddOrUpdate_with_NULL_parameter_handle_fails)
    {
        ///arrange
        CMapMocks mocks;
        mocks.ResetAllCalls();

        ///act
        auto result1 = Map_AddOrUpdate(NULL, TEST_BLUEKEY, TEST_YELLOWVALUE);

        ///assert
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_INVALIDARG, result1);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
    }

    /*Tests_SRS_MAP_02_014: [If parameter key is NULL then Map_AddOrUpdate shall return MAP_INVALID_ARG.] */
    TEST_FUNCTION(Map_AddOrUpdate_with_NULL_key_handle_fails)
    {
        ///arrange
        CMapMocks mocks;
        auto handle = Map_Create(NULL);
        mocks.ResetAllCalls();

        ///act
        auto result1 = Map_AddOrUpdate(handle, NULL, TEST_YELLOWVALUE);

        ///assert
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_INVALIDARG, result1);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        Map_Destroy(handle);
    }

    /*Tests_SRS_MAP_02_015: [If parameter value is NULL then Map_AddOrUpdate shall return MAP_INVALID_ARG.]*/
    TEST_FUNCTION(Map_AddOrUpdate_with_NULL_value_handle_fails)
    {
        ///arrange
        CMapMocks mocks;
        auto handle = Map_Create(NULL);
        mocks.ResetAllCalls();

        ///act
        auto result1 = Map_AddOrUpdate(handle, TEST_REDKEY, NULL);

        ///assert
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_INVALIDARG, result1);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        Map_Destroy(handle);
    }

    /*Tests_SRS_MAP_02_017: [Otherwise, Map_AddOrUpdate shall add the pair <key,value> to the map.]*/
    /*Tests_SRS_MAP_02_019: [Otherwise, Map_AddOrUpdate shall return MAP_OK.] */
    TEST_FUNCTION(Map_AddOrUpdate_with_1_pair_succeeded)
    {
        ///arrange
        CMapMocks mocks;
        const char*const* keys;
        const char*const* values;
        size_t count;
        auto handle = Map_Create(NULL);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, gballoc_realloc(NULL, sizeof(const char*))); /*growing keys*/
        STRICT_EXPECTED_CALL(mocks, gballoc_realloc(NULL, sizeof(const char*))); /*growing values*/
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_REDKEY)+1)); /*copy of red key*/
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_REDVALUE) + 1)); /*copy of red value*/

        ///act
        auto result1 = Map_AddOrUpdate(handle, TEST_REDKEY, TEST_REDVALUE);
        auto result2 = Map_GetInternals(handle, &keys, &values, &count);

        ///assert
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_OK, result1);
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_OK, result2);
        ASSERT_ARE_EQUAL(size_t, 1, count);
        ASSERT_ARE_EQUAL(char_ptr, TEST_REDKEY, keys[0]);
        ASSERT_ARE_EQUAL(char_ptr, TEST_REDVALUE, values[0]);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        Map_Destroy(handle);
    }

    /*Tests_SRS_MAP_02_017: [Otherwise, Map_AddOrUpdate shall add the pair <key,value> to the map.]*/
    TEST_FUNCTION(Map_AddOrUpdate_with_2_pair_succeeded)
    {
        ///arrange
        CMapMocks mocks;
        const char*const* keys;
        const char*const* values;
        size_t count;
        auto handle = Map_Create(NULL);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, gballoc_realloc(NULL, sizeof(const char*))); /*growing keys*/
        STRICT_EXPECTED_CALL(mocks, gballoc_realloc(NULL, sizeof(const char*))); /*growing values*/
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_REDKEY) + 1)); /*copy of red key*/
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_REDVALUE) + 1)); /*copy of red value*/

        STRICT_EXPECTED_CALL(mocks, gballoc_realloc(IGNORED_PTR_ARG, 2 * sizeof(const char*))) /*growing keys*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_realloc(IGNORED_PTR_ARG, 2 * sizeof(const char*))) /*growing values*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_BLUEKEY) + 1)); /*copy of red key*/
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_BLUEVALUE) + 1)); /*copy of red value*/

        ///act
        auto result1 = Map_AddOrUpdate(handle, TEST_REDKEY, TEST_REDVALUE);
        auto result2 = Map_AddOrUpdate(handle, TEST_BLUEKEY, TEST_BLUEVALUE);
        auto result3 = Map_GetInternals(handle, &keys, &values, &count);

        ///assert
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_OK, result1);
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_OK, result2);
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_OK, result3);
        ASSERT_ARE_EQUAL(size_t, 2, count);
        ASSERT_ARE_EQUAL(char_ptr, TEST_REDKEY, keys[0]);
        ASSERT_ARE_EQUAL(char_ptr, TEST_REDVALUE, values[0]);
        ASSERT_ARE_EQUAL(char_ptr, TEST_BLUEKEY, keys[1]);
        ASSERT_ARE_EQUAL(char_ptr, TEST_BLUEVALUE, values[1]);

        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        Map_Destroy(handle);
    }

    /*Tests_SRS_MAP_02_018: [If there are any failures then Map_AddOrUpdate shall return MAP_ERROR.] */
    TEST_FUNCTION(Map_AddOrUpdate_with_2_differnt_pair_fails_when_gballoc_fails_1)
    {
        ///arrange
        CMapMocks mocks;
        const char*const* keys;
        const char*const* values;
        size_t count;
        auto handle = Map_Create(NULL);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, gballoc_realloc(NULL, sizeof(const char*))); /*growing keys*/
        STRICT_EXPECTED_CALL(mocks, gballoc_realloc(NULL, sizeof(const char*))); /*growing values*/
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_REDKEY) + 1)); /*copy of red key*/
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_REDVALUE) + 1)); /*copy of red value*/

        STRICT_EXPECTED_CALL(mocks, gballoc_realloc(IGNORED_PTR_ARG, 2 * sizeof(const char*))) /*growing keys*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_realloc(IGNORED_PTR_ARG, 2 * sizeof(const char*))) /*growing values*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_BLUEKEY) + 1)); /*copy of red key*/

        whenShallmalloc_fail = currentmalloc_call + 4;
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_BLUEVALUE) + 1)); /*copy of red value*/

        /*below are undo actions*/
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*undo blue key value*/
            .ValidateArgumentBuffer(1, TEST_BLUEKEY, strlen(TEST_BLUEKEY) + 1);
        STRICT_EXPECTED_CALL(mocks, gballoc_realloc(IGNORED_PTR_ARG, 1 * sizeof(const char*))) /*undo growing keys*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_realloc(IGNORED_PTR_ARG, 1 * sizeof(const char*))) /*undo growing values*/
            .IgnoreArgument(1);

        ///act
        auto result1 = Map_AddOrUpdate(handle, TEST_REDKEY, TEST_REDVALUE);
        auto result2 = Map_AddOrUpdate(handle, TEST_BLUEKEY, TEST_BLUEVALUE);
        auto result3 = Map_GetInternals(handle, &keys, &values, &count);

        ///assert
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_OK, result1);
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_ERROR, result2);
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_OK, result3);
        ASSERT_ARE_EQUAL(size_t, 1, count);
        ASSERT_ARE_EQUAL(char_ptr, TEST_REDKEY, keys[0]);
        ASSERT_ARE_EQUAL(char_ptr, TEST_REDVALUE, values[0]);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        Map_Destroy(handle);
    }

    /*Tests_SRS_MAP_02_018: [If there are any failures then Map_AddOrUpdate shall return MAP_ERROR.] */
    TEST_FUNCTION(Map_AddOrUpdate_with_2_differnt_pair_fails_when_gballoc_fails_2)
    {
        ///arrange
        CMapMocks mocks;
        const char*const* keys;
        const char*const* values;
        size_t count;
        auto handle = Map_Create(NULL);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, gballoc_realloc(NULL, sizeof(const char*))); /*growing keys*/
        STRICT_EXPECTED_CALL(mocks, gballoc_realloc(NULL, sizeof(const char*))); /*growing values*/
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_REDKEY) + 1)); /*copy of red key*/
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_REDVALUE) + 1)); /*copy of red value*/

        STRICT_EXPECTED_CALL(mocks, gballoc_realloc(IGNORED_PTR_ARG, 2 * sizeof(const char*))) /*growing keys*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_realloc(IGNORED_PTR_ARG, 2 * sizeof(const char*))) /*growing values*/
            .IgnoreArgument(1);
        whenShallmalloc_fail = currentmalloc_call + 3;
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_BLUEKEY) + 1)); /*copy of red key*/

        /*below are undo actions*/
        STRICT_EXPECTED_CALL(mocks, gballoc_realloc(IGNORED_PTR_ARG, 1 * sizeof(const char*))) /*undo growing keys*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_realloc(IGNORED_PTR_ARG, 1 * sizeof(const char*))) /*undo growing values*/
            .IgnoreArgument(1);

        ///act
        auto result1 = Map_AddOrUpdate(handle, TEST_REDKEY, TEST_REDVALUE);
        auto result2 = Map_AddOrUpdate(handle, TEST_BLUEKEY, TEST_BLUEVALUE);
        auto result3 = Map_GetInternals(handle, &keys, &values, &count);

        ///assert
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_OK, result1);
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_ERROR, result2);
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_OK, result3);
        ASSERT_ARE_EQUAL(size_t, 1, count);
        ASSERT_ARE_EQUAL(char_ptr, TEST_REDKEY, keys[0]);
        ASSERT_ARE_EQUAL(char_ptr, TEST_REDVALUE, values[0]);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        Map_Destroy(handle);
    }

    /*Tests_SRS_MAP_02_018: [If there are any failures then Map_AddOrUpdate shall return MAP_ERROR.] */
    TEST_FUNCTION(Map_AddOrUpdate_with_2_differnt_pair_fails_when_gballoc_fails_3)
    {
        ///arrange
        CMapMocks mocks;
        const char*const* keys;
        const char*const* values;
        size_t count;
        auto handle = Map_Create(NULL);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, gballoc_realloc(NULL, sizeof(const char*))); /*growing keys*/
        STRICT_EXPECTED_CALL(mocks, gballoc_realloc(NULL, sizeof(const char*))); /*growing values*/
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_REDKEY) + 1)); /*copy of red key*/
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_REDVALUE) + 1)); /*copy of red value*/

        STRICT_EXPECTED_CALL(mocks, gballoc_realloc(IGNORED_PTR_ARG, 2 * sizeof(const char*))) /*growing keys*/
            .IgnoreArgument(1);
        whenShallrealloc_fail = currentrealloc_call + 4;
        STRICT_EXPECTED_CALL(mocks, gballoc_realloc(IGNORED_PTR_ARG, 2 * sizeof(const char*))) /*growing values*/
            .IgnoreArgument(1);

        /*below are undo actions*/
        STRICT_EXPECTED_CALL(mocks, gballoc_realloc(IGNORED_PTR_ARG, 1 * sizeof(const char*))) /*undo growing keys*/
            .IgnoreArgument(1);

        ///act
        auto result1 = Map_AddOrUpdate(handle, TEST_REDKEY, TEST_REDVALUE);
        auto result2 = Map_AddOrUpdate(handle, TEST_BLUEKEY, TEST_BLUEVALUE);
        auto result3 = Map_GetInternals(handle, &keys, &values, &count);

        ///assert
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_OK, result1);
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_ERROR, result2);
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_OK, result3);
        ASSERT_ARE_EQUAL(size_t, 1, count);
        ASSERT_ARE_EQUAL(char_ptr, TEST_REDKEY, keys[0]);
        ASSERT_ARE_EQUAL(char_ptr, TEST_REDVALUE, values[0]);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        Map_Destroy(handle);
    }

    /*Tests_SRS_MAP_02_018: [If there are any failures then Map_AddOrUpdate shall return MAP_ERROR.] */
    TEST_FUNCTION(Map_AddOrUpdate_with_2_differnt_pair_fails_when_gballoc_fails_4)
    {
        ///arrange
        CMapMocks mocks;
        const char*const* keys;
        const char*const* values;
        size_t count;
        auto handle = Map_Create(NULL);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, gballoc_realloc(NULL, sizeof(const char*))); /*growing keys*/
        STRICT_EXPECTED_CALL(mocks, gballoc_realloc(NULL, sizeof(const char*))); /*growing values*/
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_REDKEY) + 1)); /*copy of red key*/
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_REDVALUE) + 1)); /*copy of red value*/

        whenShallrealloc_fail = currentrealloc_call + 3;
        STRICT_EXPECTED_CALL(mocks, gballoc_realloc(IGNORED_PTR_ARG, 2 * sizeof(const char*))) /*growing keys*/
            .IgnoreArgument(1);
        
        /*below are undo actions*/ /*none*/

        ///act
        auto result1 = Map_AddOrUpdate(handle, TEST_REDKEY, TEST_REDVALUE);
        auto result2 = Map_AddOrUpdate(handle, TEST_BLUEKEY, TEST_BLUEVALUE);
        auto result3 = Map_GetInternals(handle, &keys, &values, &count);

        ///assert
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_OK, result1);
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_ERROR, result2);
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_OK, result3);
        ASSERT_ARE_EQUAL(size_t, 1, count);
        ASSERT_ARE_EQUAL(char_ptr, TEST_REDKEY, keys[0]);
        ASSERT_ARE_EQUAL(char_ptr, TEST_REDVALUE, values[0]);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        Map_Destroy(handle);
    }

    /*Tests_SRS_MAP_02_018: [If there are any failures then Map_AddOrUpdate shall return MAP_ERROR.] */
    TEST_FUNCTION(Map_AddOrUpdate_with_2_differnt_pair_fails_when_gballoc_fails_5)
    {
        ///arrange
        CMapMocks mocks;
        const char*const* keys;
        const char*const* values;
        size_t count;
        auto handle = Map_Create(NULL);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, gballoc_realloc(NULL, sizeof(const char*))); /*growing keys*/
        STRICT_EXPECTED_CALL(mocks, gballoc_realloc(NULL, sizeof(const char*))); /*growing values*/
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_REDKEY) + 1)); /*copy of red key*/
        whenShallmalloc_fail = currentmalloc_call + 2;
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_REDVALUE) + 1)); /*copy of red value*/

        /*below are undo actions*/
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*undo red key value*/
            .ValidateArgumentBuffer(1, TEST_REDKEY, strlen(TEST_REDKEY) + 1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*undo growing keys*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*undo growing values*/
            .IgnoreArgument(1);

        ///act
        auto result1 = Map_AddOrUpdate(handle, TEST_REDKEY, TEST_REDVALUE);
        auto result3 = Map_GetInternals(handle, &keys, &values, &count);

        ///assert
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_ERROR, result1);
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_OK, result3);
        ASSERT_ARE_EQUAL(size_t, 0, count);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        Map_Destroy(handle);
    }

    /*Tests_SRS_MAP_02_018: [If there are any failures then Map_AddOrUpdate shall return MAP_ERROR.] */
    TEST_FUNCTION(Map_AddOrUpdate_with_2_differnt_pair_fails_when_gballoc_fails_6)
    {
        ///arrange
        CMapMocks mocks;
        const char*const* keys;
        const char*const* values;
        size_t count;
        auto handle = Map_Create(NULL);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, gballoc_realloc(NULL, sizeof(const char*))); /*growing keys*/
        STRICT_EXPECTED_CALL(mocks, gballoc_realloc(NULL, sizeof(const char*))); /*growing values*/

        whenShallmalloc_fail = currentmalloc_call + 1;
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_REDKEY) + 1)); /*copy of red key*/
        
        /*below are undo actions*/
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*undo growing keys*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*undo growing values*/
            .IgnoreArgument(1);

        ///act
        auto result1 = Map_AddOrUpdate(handle, TEST_REDKEY, TEST_REDVALUE);
        auto result3 = Map_GetInternals(handle, &keys, &values, &count);

        ///assert
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_ERROR, result1);
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_OK, result3);
        ASSERT_ARE_EQUAL(size_t, 0, count);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        Map_Destroy(handle);
    }

    /*Tests_SRS_MAP_02_018: [If there are any failures then Map_AddOrUpdate shall return MAP_ERROR.] */
    TEST_FUNCTION(Map_AddOrUpdate_with_2_differnt_pair_fails_when_gballoc_fails_7)
    {
        ///arrange
        CMapMocks mocks;
        const char*const* keys;
        const char*const* values;
        size_t count;
        auto handle = Map_Create(NULL);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, gballoc_realloc(NULL, sizeof(const char*))); /*growing keys*/
        whenShallrealloc_fail = currentrealloc_call + 2;
        STRICT_EXPECTED_CALL(mocks, gballoc_realloc(NULL, sizeof(const char*))); /*growing values*/

        /*below are undo actions*/
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*undo growing keys*/
            .IgnoreArgument(1);

        ///act
        auto result1 = Map_AddOrUpdate(handle, TEST_REDKEY, TEST_REDVALUE);
        auto result3 = Map_GetInternals(handle, &keys, &values, &count);

        ///assert
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_ERROR, result1);
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_OK, result3);
        ASSERT_ARE_EQUAL(size_t, 0, count);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        Map_Destroy(handle);
    }

    /*Tests_SRS_MAP_02_018: [If there are any failures then Map_AddOrUpdate shall return MAP_ERROR.] */
    TEST_FUNCTION(Map_AddOrUpdate_with_2_differnt_pair_fails_when_gballoc_fails_8)
    {
        ///arrange
        CMapMocks mocks;
        const char*const* keys;
        const char*const* values;
        size_t count;
        auto handle = Map_Create(NULL);
        mocks.ResetAllCalls();

        whenShallrealloc_fail = currentrealloc_call + 1;
        STRICT_EXPECTED_CALL(mocks, gballoc_realloc(NULL, sizeof(const char*))); /*growing keys*/

        /*below are undo actions*/

        ///act
        auto result1 = Map_AddOrUpdate(handle, TEST_REDKEY, TEST_REDVALUE);
        auto result3 = Map_GetInternals(handle, &keys, &values, &count);

        ///assert
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_ERROR, result1);
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_OK, result3);
        ASSERT_ARE_EQUAL(size_t, 0, count);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        Map_Destroy(handle);
    }

    /*Tests_SRS_MAP_02_016: [If the key already exists, then Map_AddOrUpdate shall overwrite the value of the existing key with parameter value.]*/
    TEST_FUNCTION(Map_AddOrUpdate_with_2_pair_overwrites_firstValue_succeeds)
    {
        ///arrange
        CMapMocks mocks;
        const char*const* keys;
        const char*const* values;
        size_t count;
        auto handle = Map_Create(NULL);
        (void)Map_AddOrUpdate(handle, TEST_REDKEY, TEST_REDVALUE);
        (void)Map_AddOrUpdate(handle, TEST_BLUEKEY, TEST_BLUEVALUE);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, gballoc_realloc(IGNORED_PTR_ARG, strlen(TEST_YELLOWVALUE) + 1)) /*changing redkey value to yellow*/
            .ValidateArgumentBuffer(1, TEST_REDVALUE, strlen(TEST_REDVALUE) + 1);

        ///act
        auto result1 = Map_AddOrUpdate(handle, TEST_REDKEY, TEST_YELLOWVALUE);
        auto result3 = Map_GetInternals(handle, &keys, &values, &count);

        ///assert
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_OK, result1);
       
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_OK, result3);
        ASSERT_ARE_EQUAL(size_t, 2, count);
        ASSERT_ARE_EQUAL(char_ptr, TEST_REDKEY, keys[0]);
        ASSERT_ARE_EQUAL(char_ptr, TEST_YELLOWVALUE, values[0]);
        ASSERT_ARE_EQUAL(char_ptr, TEST_BLUEKEY, keys[1]);
        ASSERT_ARE_EQUAL(char_ptr, TEST_BLUEVALUE, values[1]);

        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        Map_Destroy(handle);
    }

    /*Tests_SRS_MAP_02_016: [If the key already exists, then Map_AddOrUpdate shall overwrite the value of the existing key with parameter value.]*/
    TEST_FUNCTION(Map_AddOrUpdate_with_2_pair_overwrites_firstValue_when_gballoc_fails_it_does_not_change_the_value)
    {
        ///arrange
        CMapMocks mocks;
        const char*const* keys;
        const char*const* values;
        size_t count;
        auto handle = Map_Create(NULL);
        (void)Map_AddOrUpdate(handle, TEST_REDKEY, TEST_REDVALUE);
        (void)Map_AddOrUpdate(handle, TEST_BLUEKEY, TEST_BLUEVALUE);
        mocks.ResetAllCalls();

        whenShallrealloc_fail = currentrealloc_call + 1;
        STRICT_EXPECTED_CALL(mocks, gballoc_realloc(IGNORED_PTR_ARG, strlen(TEST_YELLOWVALUE) + 1)) /*changing redkey value to yellow*/
            .ValidateArgumentBuffer(1, TEST_REDVALUE, strlen(TEST_REDVALUE) + 1);

        ///act
        auto result1 = Map_AddOrUpdate(handle, TEST_REDKEY, TEST_YELLOWVALUE);
        auto result3 = Map_GetInternals(handle, &keys, &values, &count);

        ///assert
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_ERROR, result1);
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_OK, result3);
        ASSERT_ARE_EQUAL(size_t, 2, count);
        ASSERT_ARE_EQUAL(char_ptr, TEST_REDKEY, keys[0]);
        ASSERT_ARE_EQUAL(char_ptr, TEST_REDVALUE, values[0]);
        ASSERT_ARE_EQUAL(char_ptr, TEST_BLUEKEY, keys[1]);
        ASSERT_ARE_EQUAL(char_ptr, TEST_BLUEVALUE, values[1]);

        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        Map_Destroy(handle);
    }

    /*Tests_SRS_MAP_02_016: [If the key already exists, then Map_AddOrUpdate shall overwrite the value of the existing key with parameter value.]*/
    TEST_FUNCTION(Map_AddOrUpdate_with_2_pair_overwrites_secondValue)
    {
        ///arrange
        CMapMocks mocks;
        const char*const* keys;
        const char*const* values;
        size_t count;
        auto handle = Map_Create(NULL);
        (void)Map_AddOrUpdate(handle, TEST_REDKEY, TEST_REDVALUE);
        (void)Map_AddOrUpdate(handle, TEST_BLUEKEY, TEST_BLUEVALUE);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, gballoc_realloc(IGNORED_PTR_ARG, strlen(TEST_YELLOWVALUE) + 1)) /*changing bluekey value to yellow*/
            .ValidateArgumentBuffer(1, TEST_BLUEVALUE, strlen(TEST_BLUEVALUE) + 1);

        ///act
        auto result1 = Map_AddOrUpdate(handle, TEST_BLUEKEY, TEST_YELLOWVALUE);
        auto result3 = Map_GetInternals(handle, &keys, &values, &count);

        ///assert
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_OK, result1);
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_OK, result3);
        ASSERT_ARE_EQUAL(size_t, 2, count);
        ASSERT_ARE_EQUAL(char_ptr, TEST_REDKEY, keys[0]);
        ASSERT_ARE_EQUAL(char_ptr, TEST_REDVALUE, values[0]);
        ASSERT_ARE_EQUAL(char_ptr, TEST_BLUEKEY, keys[1]);
        ASSERT_ARE_EQUAL(char_ptr, TEST_YELLOWVALUE, values[1]);

        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        Map_Destroy(handle);
    }

    /*Tests_SRS_MAP_02_016: [If the key already exists, then Map_AddOrUpdate shall overwrite the value of the existing key with parameter value.]*/
    TEST_FUNCTION(Map_AddOrUpdate_with_2_pair_overwrites_secondValue_doesn_not_change_the_value_when_gballoc_fails)
    {
        ///arrange
        CMapMocks mocks;
        const char*const* keys;
        const char*const* values;
        size_t count;
        auto handle = Map_Create(NULL);
        (void)Map_AddOrUpdate(handle, TEST_REDKEY, TEST_REDVALUE);
        (void)Map_AddOrUpdate(handle, TEST_BLUEKEY, TEST_BLUEVALUE);
        mocks.ResetAllCalls();

        whenShallrealloc_fail = currentrealloc_call + 1;
        STRICT_EXPECTED_CALL(mocks, gballoc_realloc(IGNORED_PTR_ARG, strlen(TEST_YELLOWVALUE) + 1)) /*changing bluekey value to yellow*/
            .ValidateArgumentBuffer(1, TEST_BLUEVALUE, strlen(TEST_BLUEVALUE) + 1);

        ///act
        auto result1 = Map_AddOrUpdate(handle, TEST_BLUEKEY, TEST_YELLOWVALUE);
        auto result3 = Map_GetInternals(handle, &keys, &values, &count);

        ///assert
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_ERROR, result1);
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_OK, result3);
        ASSERT_ARE_EQUAL(size_t, 2, count);
        ASSERT_ARE_EQUAL(char_ptr, TEST_REDKEY, keys[0]);
        ASSERT_ARE_EQUAL(char_ptr, TEST_REDVALUE, values[0]);
        ASSERT_ARE_EQUAL(char_ptr, TEST_BLUEKEY, keys[1]);
        ASSERT_ARE_EQUAL(char_ptr, TEST_BLUEVALUE, values[1]);

        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        Map_Destroy(handle);
    }

    /*Tests_SRS_MAP_02_020: [If parameter handle is NULL then Map_Delete shall return MAP_INVALIDARG.]*/
    TEST_FUNCTION(Map_Delete_with_NULL_handle_fails)
    {
        ///arrange
        CMapMocks mocks;
        
        ///act
        auto result1 = Map_Delete(NULL, TEST_BLUEKEY);

        ///assert
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_INVALIDARG, result1);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
    }

    /*Tests_SRS_MAP_02_021: [If parameter key is NULL then Map_Delete shall return MAP_INVALIDARG.]*/
    TEST_FUNCTION(Map_Delete_with_NULL_key_fails)
    {
        ///arrange
        CMapMocks mocks;
        auto handle = Map_Create(NULL);
        mocks.ResetAllCalls();

        ///act
        auto result1 = Map_Delete(handle, NULL);

        ///assert
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_INVALIDARG, result1);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        Map_Destroy(handle);
    }

    /*Tests_SRS_MAP_02_022: [If key does not exist then Map_Delete shall return MAP_KEYNOTFOUND.]*/
    TEST_FUNCTION(Map_Delete_with_not_found_key_succeeds_1)
    {
        ///arrange
        CMapMocks mocks;
        auto handle = Map_Create(NULL);
        mocks.ResetAllCalls();

        ///act
        auto result1 = Map_Delete(handle, TEST_REDKEY);

        ///assert
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_KEYNOTFOUND, result1);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        Map_Destroy(handle);
    }

    /*Tests_SRS_MAP_02_022: [If key does not exist then Map_Delete shall return MAP_KEYNOTFOUND.]*/
    TEST_FUNCTION(Map_Delete_with_not_found_key_succeeds_2)
    {
        ///arrange
        CMapMocks mocks;
        auto handle = Map_Create(NULL);
        (void)Map_AddOrUpdate(handle, TEST_YELLOWKEY, TEST_YELLOWVALUE);
        mocks.ResetAllCalls();

        ///act
        auto result1 = Map_Delete(handle, TEST_REDKEY);

        ///assert
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_KEYNOTFOUND, result1);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        Map_Destroy(handle);
    }

    /*Tests_SRS_MAP_02_023: [Otherwise, Map_Delete shall remove the key and its associated value from the map and return MAP_OK.] */
    TEST_FUNCTION(Map_Delete_with_1_found_key_succeeds)
    {
        ///arrange
        CMapMocks mocks;
        auto handle = Map_Create(NULL);
        const char*const* keys;
        const char*const* values;
        size_t count;
        (void)Map_AddOrUpdate(handle, TEST_YELLOWKEY, TEST_YELLOWVALUE);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*freeing yellow value*/
            .ValidateArgumentBuffer(1, TEST_YELLOWVALUE, strlen(TEST_YELLOWVALUE) + 1);

        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*freeing yellow key*/
            .ValidateArgumentBuffer(1, TEST_YELLOWKEY, strlen(TEST_YELLOWKEY) + 1);

        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*ungrowing values*/
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*ungowing keys*/
            .IgnoreArgument(1);

        ///act
        auto result1 = Map_Delete(handle, TEST_YELLOWKEY);
        auto result3 = Map_GetInternals(handle, &keys, &values, &count);

        ///assert
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_OK, result1);
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_OK, result3);
        ASSERT_ARE_EQUAL(size_t, 0, count);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        Map_Destroy(handle);
    }

    /*Tests_SRS_MAP_02_023: [Otherwise, Map_Delete shall remove the key and its associated value from the map and return MAP_OK.] */
    TEST_FUNCTION(Map_Delete_with_1_found_key_succeeds_2)
    {
        ///arrange
        CMapMocks mocks;
        auto handle = Map_Create(NULL);
        const char*const* keys;
        const char*const* values;
        size_t count;
        (void)Map_AddOrUpdate(handle, TEST_REDKEY, TEST_REDVALUE);
        (void)Map_AddOrUpdate(handle, TEST_YELLOWKEY, TEST_YELLOWVALUE);
        
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*freeing yellow value*/
            .ValidateArgumentBuffer(1, TEST_YELLOWVALUE, strlen(TEST_YELLOWVALUE) + 1);

        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*freeing yellow key*/
            .ValidateArgumentBuffer(1, TEST_YELLOWKEY, strlen(TEST_YELLOWKEY) + 1);

        STRICT_EXPECTED_CALL(mocks, gballoc_realloc(IGNORED_PTR_ARG, 1* sizeof(const char*))) /*ungrowing values*/
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, gballoc_realloc(IGNORED_PTR_ARG, 1 * sizeof(const char*))) /*ungrowing keys*/
            .IgnoreArgument(1);

        ///act
        auto result1 = Map_Delete(handle, TEST_YELLOWKEY);
        auto result3 = Map_GetInternals(handle, &keys, &values, &count);

        ///assert
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_OK, result1);
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_OK, result3);
        ASSERT_ARE_EQUAL(size_t, 1, count);
        ASSERT_ARE_EQUAL(char_ptr, TEST_REDKEY, keys[0]);
        ASSERT_ARE_EQUAL(char_ptr, TEST_REDVALUE, values[0]);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        Map_Destroy(handle);
    }

    /*Tests_SRS_MAP_02_023: [Otherwise, Map_Delete shall remove the key and its associated value from the map and return MAP_OK.] */
    TEST_FUNCTION(Map_Delete_with_1_found_key_succeeds_3)
    {
        ///arrange
        CMapMocks mocks;
        auto handle = Map_Create(NULL);
        const char*const* keys;
        const char*const* values;
        size_t count;
        (void)Map_AddOrUpdate(handle, TEST_REDKEY, TEST_REDVALUE);
        (void)Map_AddOrUpdate(handle, TEST_YELLOWKEY, TEST_YELLOWVALUE);

        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*freeing yellow value*/
            .ValidateArgumentBuffer(1, TEST_REDVALUE, strlen(TEST_REDVALUE) + 1);

        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*freeing yellow key*/
            .ValidateArgumentBuffer(1, TEST_REDKEY, strlen(TEST_REDKEY) + 1);

        STRICT_EXPECTED_CALL(mocks, gballoc_realloc(IGNORED_PTR_ARG, 1 * sizeof(const char*))) /*ungrowing values*/
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, gballoc_realloc(IGNORED_PTR_ARG, 1 * sizeof(const char*))) /*ungrowing keys*/
            .IgnoreArgument(1);

        ///act
        auto result1 = Map_Delete(handle, TEST_REDKEY);
        auto result3 = Map_GetInternals(handle, &keys, &values, &count);

        ///assert
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_OK, result1);
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_OK, result3);
        ASSERT_ARE_EQUAL(size_t, 1, count);
        ASSERT_ARE_EQUAL(char_ptr, TEST_YELLOWKEY, keys[0]);
        ASSERT_ARE_EQUAL(char_ptr, TEST_YELLOWVALUE, values[0]);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        Map_Destroy(handle);
    }

    /*Tests_SRS_MAP_02_024: [If parameter handle, key or keyExists are NULL then Map_ContainsKey shall return MAP_INVALIDARG.]*/
    TEST_FUNCTION(Map_ContainsKey_fails_with_invalid_arg_1)
    {
        ///arrange
        CMapMocks mocks;
        bool exists;
        mocks.ResetAllCalls();

        ///act
        auto result1 = Map_ContainsKey(NULL,TEST_REDKEY, &exists);

        ///assert
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_INVALIDARG, result1);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
    }

    /*Tests_SRS_MAP_02_024: [If parameter handle, key or keyExists are NULL then Map_ContainsKey shall return MAP_INVALIDARG.]*/
    TEST_FUNCTION(Map_ContainsKey_fails_with_invalid_arg_2)
    {
        ///arrange
        CMapMocks mocks;
        bool exists;
        auto handle = Map_Create(NULL);
        mocks.ResetAllCalls();

        ///act
        auto result1 = Map_ContainsKey(handle, NULL, &exists);

        ///assert
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_INVALIDARG, result1);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        Map_Destroy(handle);
    }

    /*Tests_SRS_MAP_02_024: [If parameter handle, key or keyExists are NULL then Map_ContainsKey shall return MAP_INVALIDARG.]*/
    TEST_FUNCTION(Map_ContainsKey_fails_with_invalid_arg_3)
    {
        ///arrange
        CMapMocks mocks;
        auto handle = Map_Create(NULL);
        mocks.ResetAllCalls();

        ///act
        auto result1 = Map_ContainsKey(handle, TEST_REDKEY, NULL);

        ///assert
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_INVALIDARG, result1);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        Map_Destroy(handle);
    }

    /*Tests_SRS_MAP_02_025: [Otherwise if a key exists then Map_ContainsKey shall return MAP_OK and shall write in keyExists "true".]*/
    /*Tests_SRS_MAP_02_026: [If a key doesn't exist, then Map_ContainsKey shall return MAP_OK and write in keyExists "false".] */
    TEST_FUNCTION(Map_ContainsKey_fails_with_known_key_succeeds)
    {
        ///arrange
        CMapMocks mocks;
        auto handle = Map_Create(NULL);
        bool e1, e2;
        (void)Map_AddOrUpdate(handle, TEST_REDKEY, TEST_REDVALUE);
        mocks.ResetAllCalls();

        ///act
        auto result1 = Map_ContainsKey(handle, TEST_REDKEY, &e1);
        auto result2 = Map_ContainsKey(handle, TEST_BLUEKEY, &e2);

        ///assert
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_OK, result1);
        ASSERT_IS_TRUE(e1);

        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_OK, result2);
        ASSERT_IS_FALSE(e2);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        Map_Destroy(handle);
    }

    /*Tests_SRS_MAP_02_027: [If parameter handle, value or valueExists is NULL then Map_ContainsValue shall return MAP_INVALIDARG.]*/
    TEST_FUNCTION(Map_ContainsValue_fails_with_NULL_handle)
    {
        ///arrange
        CMapMocks mocks;
        bool e1;
        mocks.ResetAllCalls();

        ///act
        auto result1 = Map_ContainsValue(NULL, TEST_REDKEY, &e1);

        ///assert
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_INVALIDARG, result1);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
    }

    /*Tests_SRS_MAP_02_027: [If parameter handle, value or valueExists is NULL then Map_ContainsValue shall return MAP_INVALIDARG.]*/
    TEST_FUNCTION(Map_ContainsValue_fails_with_NULL_key)
    {
        ///arrange
        CMapMocks mocks;
        auto handle = Map_Create(NULL);
        bool e1;
        mocks.ResetAllCalls();

        ///act
        auto result1 = Map_ContainsValue(handle, NULL, &e1);

        ///assert
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_INVALIDARG, result1);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        Map_Destroy(handle);
    }

    /*Tests_SRS_MAP_02_027: [If parameter handle, value or valueExists is NULL then Map_ContainsValue shall return MAP_INVALIDARG.]*/
    TEST_FUNCTION(Map_ContainsValue_fails_with_NULL_exists)
    {
        ///arrange
        CMapMocks mocks;
        auto handle = Map_Create(NULL);
        mocks.ResetAllCalls();

        ///act
        auto result1 = Map_ContainsValue(handle, TEST_REDKEY, NULL);

        ///assert
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_INVALIDARG, result1);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        Map_Destroy(handle);
    }

    /*Tests_SRS_MAP_02_028: [Otherwise, if a pair <key, value> has its value equal to the parameter value, the Map_ContainsValue shall return MAP_OK and shall write in valueExists "true".]*/
    /*Tests_SRS_MAP_02_029: [Otherwise, if such a <key, value> does not exist, then Map_ContainsValue shall return MAP_OK and shall write in valueExists "false".] */
    TEST_FUNCTION(Map_ContainsValue_succeeds)
    {
        ///arrange
        CMapMocks mocks;
        auto handle = Map_Create(NULL);
        bool e1, e2;
        (void)Map_AddOrUpdate(handle, TEST_REDKEY, TEST_REDVALUE);
        mocks.ResetAllCalls();

        ///act
        auto result1 = Map_ContainsValue(handle, TEST_REDVALUE, &e1);
        auto result2 = Map_ContainsValue(handle, TEST_BLUEVALUE, &e2);

        ///assert
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_OK, result1);
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_OK, result2);
        ASSERT_IS_TRUE(e1);
        ASSERT_IS_FALSE(e2);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        Map_Destroy(handle);
    }

    /*Tests_SRS_MAP_02_040: [If parameter handle or key is NULL then Map_GetValueFromKey returns NULL.]*/
    TEST_FUNCTION(Map_GetValueFromKey_returns_NULL__for_invalid_handle)
    {
        ///arrange
        CMapMocks mocks;
        mocks.ResetAllCalls();

        ///act
        auto result = Map_GetValueFromKey(NULL, TEST_REDKEY);

        ///assert
        ASSERT_IS_NULL(result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
    }

    /*Tests_SRS_MAP_02_040: [If parameter handle or key is NULL then Map_GetValueFromKey returns NULL.]*/
    TEST_FUNCTION(Map_GetValueFromKey_returns_NULL_for_NULL_key)
    {
        ///arrange
        CMapMocks mocks;
        auto handle = Map_Create(NULL);
        mocks.ResetAllCalls();

        ///act
        auto result = Map_GetValueFromKey(handle, NULL);

        ///assert
        ASSERT_IS_NULL(result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        Map_Destroy(handle);
    }

    /*Tests_SRS_MAP_02_041: [If the key is not found, then Map_GetValueFromKey returns NULL.]*/
    TEST_FUNCTION(Map_GetValueFromKey_returns_NULL_for_notfound_key)
    {
        ///arrange
        CMapMocks mocks;
        auto handle = Map_Create(NULL);
        mocks.ResetAllCalls();

        ///act
        auto result = Map_GetValueFromKey(handle, TEST_REDKEY);

        ///assert
        ASSERT_IS_NULL(result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        Map_Destroy(handle);
    }

    /*Tests_SRS_MAP_02_042: [Otherwise, Map_GetValueFromKey returns the key's value.] */
    TEST_FUNCTION(Map_GetValueFromKey_returns_non_NULL_for_found_key)
    {
        ///arrange
        CMapMocks mocks;
        auto handle = Map_Create(NULL);
        Map_AddOrUpdate(handle, TEST_REDKEY, TEST_REDVALUE);
        mocks.ResetAllCalls();

        ///act
        auto result = Map_GetValueFromKey(handle, TEST_REDKEY);

        ///assert
        ASSERT_ARE_EQUAL(char_ptr, TEST_REDVALUE, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        Map_Destroy(handle);
    }

    /*Tests_SRS_MAP_02_046: [If parameter handle, keys, values or count is NULL then Map_GetInternals shall return MAP_INVALIDARG.] */
    TEST_FUNCTION(Map_GetInternals_fails_with_NULL_arg_1)
    {
        ///arrange
        const char*const* keys;
        const char*const* values;
        size_t size;

        ///act
        auto result = Map_GetInternals(NULL, &keys, &values, &size);

        ///assert
        ASSERT_ARE_EQUAL(MAP_RESULT,MAP_INVALIDARG, result);
        
    }

    /*Tests_SRS_MAP_02_046: [If parameter handle, keys, values or count is NULL then Map_GetInternals shall return MAP_INVALIDARG.] */
    TEST_FUNCTION(Map_GetInternals_fails_with_NULL_arg_2)
    {
        ///arrange
        const char*const* values;
        size_t size;
        CMapMocks mocks;
        auto handle = Map_Create(NULL);
        mocks.ResetAllCalls();

        ///act
        auto result = Map_GetInternals(handle, NULL, &values, &size);

        ///assert
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_INVALIDARG, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        Map_Destroy(handle);
    }

    /*Tests_SRS_MAP_02_046: [If parameter handle, keys, values or count is NULL then Map_GetInternals shall return MAP_INVALIDARG.] */
    TEST_FUNCTION(Map_GetInternals_fails_with_NULL_arg_3)
    {
        ///arrange
        const char*const* keys;
        size_t size;
        CMapMocks mocks;
        auto handle = Map_Create(NULL);
        mocks.ResetAllCalls();

        ///act
        auto result = Map_GetInternals(handle, &keys, NULL, &size);

        ///assert
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_INVALIDARG, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        Map_Destroy(handle);
    }

    /*Tests_SRS_MAP_02_046: [If parameter handle, keys, values or count is NULL then Map_GetInternals shall return MAP_INVALIDARG.] */
    TEST_FUNCTION(Map_GetInternals_fails_with_NULL_arg_4)
    {
        ///arrange
        const char*const* keys;
        const char*const* values;
        CMapMocks mocks;
        auto handle = Map_Create(NULL);
        mocks.ResetAllCalls();

        ///act
        auto result = Map_GetInternals(handle, &keys, &values, NULL);

        ///assert
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_INVALIDARG, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        Map_Destroy(handle);
    }

    /*Tests_SRS_MAP_02_043: [Map_GetInternals shall produce in *keys an pointer to an array of const char* having all the keys stored so far by the map.]*/
    /*Tests_SRS_MAP_02_044: [Map_GetInternals shall produce in *values a pointer to an array of const char* having all the values stored so far by the map.]*/
    /*Tests_SRS_MAP_02_045: [  Map_GetInternals shall produce in *count the number of stored keys and values.]*/
    /*tested by every test in this suite... almost*/

    /*Tests_SRS_MAP_02_038: [Map_Clone returns NULL if parameter handle is NULL.]*/
    TEST_FUNCTION(Map_Clone_with_NULL_handle_returns_NULL)
    {
        ///arrange

        ///act
        auto result = Map_Clone(NULL);

        ///assert
        ASSERT_IS_NULL(result);
        ///cleanup
    }

    /*Tests_SRS_MAP_02_039: [Map_Clone shall make a copy of the map indicated by parameter handle and return a non-NULL handle to it.]*/
    TEST_FUNCTION(Map_Clone_with_empty_map_returns_empty_map)
    {
        ///arrange
        CMapMocks mocks;        
        const char*const* keys;
        const char*const* values;
        size_t count;
        auto handle = Map_Create(NULL);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);

        ///act
        auto result = Map_Clone(handle);

        ///assert
        ASSERT_IS_NOT_NULL(result);
        (void)Map_GetInternals(result, &keys, &values, &count);
        ASSERT_IS_NULL(keys);
        ASSERT_IS_NULL(values);
        ASSERT_ARE_EQUAL(size_t, 0, count);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        Map_Destroy(handle);
        Map_Destroy(result);
    }

    /*Tests_SRS_MAP_02_039: [Map_Clone shall make a copy of the map indicated by parameter handle and return a non-NULL handle to it.]*/
    TEST_FUNCTION(Map_Clone_with_empty_fails_when_malloc_fails)
    {
        ///arrange
        CMapMocks mocks;
        auto handle = Map_Create(NULL);
        mocks.ResetAllCalls();

        whenShallmalloc_fail = currentmalloc_call + 1;
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);

        ///act
        auto result = Map_Clone(handle);

        ///assert
        ASSERT_IS_NULL(result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        Map_Destroy(handle);
    }

    /*Tests_SRS_MAP_02_039: [Map_Clone shall make a copy of the map indicated by parameter handle and return a non-NULL handle to it.]*/
    TEST_FUNCTION(Map_Clone_with_map_with_1_element_succeeds)
    {
        ///arrange
        CMapMocks mocks;
        const char*const* keys;
        const char*const* values;
        size_t count;
        auto handle = Map_Create(NULL);
        (void)Map_AddOrUpdate(handle, TEST_REDKEY, TEST_REDVALUE);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is creating the HANDLE structure*/
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(sizeof(char*))); /*this is creating a clone of the storage for keys*/

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_REDKEY) + 1)); /*this is creating a clone of RED key*/

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(sizeof(char*))); /*this is creating a clone of the storage for values*/

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_REDVALUE) + 1)); /*this is creating a clone of RED value*/

        ///act
        auto result = Map_Clone(handle);

        ///assert
        ASSERT_IS_NOT_NULL(result);
        (void)Map_GetInternals(result, &keys, &values, &count);
        ASSERT_IS_NOT_NULL(keys);
        ASSERT_IS_NOT_NULL(values);
        ASSERT_ARE_EQUAL(size_t, 1, count);
        ASSERT_ARE_EQUAL(char_ptr, TEST_REDKEY, keys[0]);
        ASSERT_ARE_EQUAL(char_ptr, TEST_REDVALUE, values[0]);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        Map_Destroy(handle);
        Map_Destroy(result);
    }

    /*Tests_SRS_MAP_02_047: [If during cloning, any operation fails, then Map_Clone shall return NULL.] */
    TEST_FUNCTION(Map_Clone_with_map_with_1_element_fails_when_gbaloc_fails_1)
    {
        ///arrange
        CMapMocks mocks;
        auto handle = Map_Create(NULL);
        (void)Map_AddOrUpdate(handle, TEST_REDKEY, TEST_REDVALUE);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is creating the HANDLE structure*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(sizeof(char*))); /*this is creating a clone of the storage for keys*/
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_REDKEY) + 1)); /*this is creating a clone of RED key*/
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(sizeof(char*))); /*this is creating a clone of the storage for values*/
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        whenShallmalloc_fail = currentmalloc_call + 5;
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_REDVALUE) + 1)); /*this is creating a clone of RED value*/

        ///act
        auto result = Map_Clone(handle);

        ///assert
        ASSERT_IS_NULL(result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        Map_Destroy(handle);
        Map_Destroy(result);
    }

    /*Tests_SRS_MAP_02_047: [If during cloning, any operation fails, then Map_Clone shall return NULL.] */
    TEST_FUNCTION(Map_Clone_with_map_with_1_element_fails_when_gbaloc_fails_2)
    {
        ///arrange
        CMapMocks mocks;
        auto handle = Map_Create(NULL);
        (void)Map_AddOrUpdate(handle, TEST_REDKEY, TEST_REDVALUE);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is creating the HANDLE structure*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(sizeof(char*))); /*this is creating a clone of the storage for keys*/
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_REDKEY) + 1)); /*this is creating a clone of RED key*/
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        whenShallmalloc_fail = currentmalloc_call + 4;
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(sizeof(char*))); /*this is creating a clone of the storage for values*/

        ///act
        auto result = Map_Clone(handle);

        ///assert
        ASSERT_IS_NULL(result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        Map_Destroy(handle);
        Map_Destroy(result);
    }

    /*Tests_SRS_MAP_02_047: [If during cloning, any operation fails, then Map_Clone shall return NULL.] */
    TEST_FUNCTION(Map_Clone_with_map_with_1_element_fails_when_gbaloc_fails_3)
    {
        ///arrange
        CMapMocks mocks;
        auto handle = Map_Create(NULL);
        (void)Map_AddOrUpdate(handle, TEST_REDKEY, TEST_REDVALUE);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is creating the HANDLE structure*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(sizeof(char*))); /*this is creating a clone of the storage for keys*/
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        whenShallmalloc_fail = currentmalloc_call + 3;
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_REDKEY) + 1)); /*this is creating a clone of RED key*/

        ///act
        auto result = Map_Clone(handle);

        ///assert
        ASSERT_IS_NULL(result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        Map_Destroy(handle);
        Map_Destroy(result);
    }

    /*Tests_SRS_MAP_02_047: [If during cloning, any operation fails, then Map_Clone shall return NULL.] */
    TEST_FUNCTION(Map_Clone_with_map_with_1_element_fails_when_gbaloc_fails_4)
    {
        ///arrange
        CMapMocks mocks;
        auto handle = Map_Create(NULL);
        (void)Map_AddOrUpdate(handle, TEST_REDKEY, TEST_REDVALUE);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is creating the HANDLE structure*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        whenShallmalloc_fail = currentmalloc_call + 2;
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(sizeof(char*))); /*this is creating a clone of the storage for keys*/

        ///act
        auto result = Map_Clone(handle);

        ///assert
        ASSERT_IS_NULL(result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        Map_Destroy(handle);
        Map_Destroy(result);
    }

    /*Tests_SRS_MAP_02_047: [If during cloning, any operation fails, then Map_Clone shall return NULL.] */
    TEST_FUNCTION(Map_Clone_with_map_with_1_element_fails_when_gbaloc_fails_5)
    {
        ///arrange
        CMapMocks mocks;
        auto handle = Map_Create(NULL);
        (void)Map_AddOrUpdate(handle, TEST_REDKEY, TEST_REDVALUE);
        mocks.ResetAllCalls();

        whenShallmalloc_fail = currentmalloc_call + 1;
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is creating the HANDLE structure*/
            .IgnoreArgument(1);

        ///act
        auto result = Map_Clone(handle);

        ///assert
        ASSERT_IS_NULL(result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        Map_Destroy(handle);
        Map_Destroy(result);
    }

    /*Tests_SRS_MAP_02_039: [Map_Clone shall make a copy of the map indicated by parameter handle and return a non-NULL handle to it.]*/
    TEST_FUNCTION(Map_Clone_with_map_with_2_element_succeeds)
    {
        ///arrange
        CMapMocks mocks;
        const char*const* keys;
        const char*const* values;
        size_t count;
        auto handle = Map_Create(NULL);
        (void)Map_AddOrUpdate(handle, TEST_REDKEY, TEST_REDVALUE);
        (void)Map_AddOrUpdate(handle, TEST_BLUEKEY, TEST_BLUEVALUE);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is creating the HANDLE structure*/
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(2*sizeof(char*))); /*this is creating a clone of the storage for keys*/

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_REDKEY) + 1)); /*this is creating a clone of RED key*/
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_BLUEKEY) + 1)); /*this is creating a clone of BLUE key*/

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(2*sizeof(char*))); /*this is creating a clone of the storage for values*/

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_REDVALUE) + 1)); /*this is creating a clone of RED value*/
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_BLUEVALUE) + 1)); /*this is creating a clone of RED value*/

        ///act
        auto result = Map_Clone(handle);

        ///assert
        ASSERT_IS_NOT_NULL(result);
        (void)Map_GetInternals(result, &keys, &values, &count);
        ASSERT_IS_NOT_NULL(keys);
        ASSERT_IS_NOT_NULL(values);
        ASSERT_ARE_EQUAL(size_t, 2, count);
        ASSERT_ARE_EQUAL(char_ptr, TEST_REDKEY, keys[0]);
        ASSERT_ARE_EQUAL(char_ptr, TEST_REDVALUE, values[0]);
        ASSERT_ARE_EQUAL(char_ptr, TEST_BLUEKEY, keys[1]);
        ASSERT_ARE_EQUAL(char_ptr, TEST_BLUEVALUE, values[1]);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        Map_Destroy(handle);
        Map_Destroy(result);
    }

    /*Tests_SRS_MAP_02_047: [If during cloning, any operation fails, then Map_Clone shall return NULL.] */
    TEST_FUNCTION(Map_Clone_with_map_with_2_element_fails_when_gballoc_fails_1)
    {
        ///arrange
        CMapMocks mocks;
        auto handle = Map_Create(NULL);
        (void)Map_AddOrUpdate(handle, TEST_REDKEY, TEST_REDVALUE);
        (void)Map_AddOrUpdate(handle, TEST_BLUEKEY, TEST_BLUEVALUE);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is creating the HANDLE structure*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(2 * sizeof(char*))); /*this is creating a clone of the storage for keys*/
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_REDKEY) + 1)); /*this is creating a clone of RED key*/
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_BLUEKEY) + 1)); /*this is creating a clone of BLUE key*/
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(2 * sizeof(char*))); /*this is creating a clone of the storage for values*/
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_REDVALUE) + 1)); /*this is creating a clone of RED value*/
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        whenShallmalloc_fail = currentmalloc_call + 7;
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_BLUEVALUE) + 1)); /*this is creating a clone of BLUE value*/

        ///act
        auto result = Map_Clone(handle);

        ///assert
        ASSERT_IS_NULL(result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        Map_Destroy(handle);
        Map_Destroy(result);
    }

    /*Tests_SRS_MAP_02_047: [If during cloning, any operation fails, then Map_Clone shall return NULL.] */
    TEST_FUNCTION(Map_Clone_with_map_with_2_element_fails_when_gballoc_fails_2)
    {
        ///arrange
        CMapMocks mocks;
        auto handle = Map_Create(NULL);
        (void)Map_AddOrUpdate(handle, TEST_REDKEY, TEST_REDVALUE);
        (void)Map_AddOrUpdate(handle, TEST_BLUEKEY, TEST_BLUEVALUE);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is creating the HANDLE structure*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(2 * sizeof(char*))); /*this is creating a clone of the storage for keys*/
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_REDKEY) + 1)); /*this is creating a clone of RED key*/
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_BLUEKEY) + 1)); /*this is creating a clone of BLUE key*/
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(2 * sizeof(char*))); /*this is creating a clone of the storage for values*/
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        whenShallmalloc_fail = currentmalloc_call + 6;
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_REDVALUE) + 1)); /*this is creating a clone of RED value*/

        ///act
        auto result = Map_Clone(handle);

        ///assert
        ASSERT_IS_NULL(result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        Map_Destroy(handle);
        Map_Destroy(result);
    }

    /*Tests_SRS_MAP_02_047: [If during cloning, any operation fails, then Map_Clone shall return NULL.] */
    TEST_FUNCTION(Map_Clone_with_map_with_2_element_fails_when_gballoc_fails_3)
    {
        ///arrange
        CMapMocks mocks;
        auto handle = Map_Create(NULL);
        (void)Map_AddOrUpdate(handle, TEST_REDKEY, TEST_REDVALUE);
        (void)Map_AddOrUpdate(handle, TEST_BLUEKEY, TEST_BLUEVALUE);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is creating the HANDLE structure*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(2 * sizeof(char*))); /*this is creating a clone of the storage for keys*/
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_REDKEY) + 1)); /*this is creating a clone of RED key*/
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_BLUEKEY) + 1)); /*this is creating a clone of BLUE key*/
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        whenShallmalloc_fail = currentmalloc_call + 5;
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(2 * sizeof(char*))); /*this is creating a clone of the storage for values*/

        ///act
        auto result = Map_Clone(handle);

        ///assert
        ASSERT_IS_NULL(result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        Map_Destroy(handle);
        Map_Destroy(result);
    }

    /*Tests_SRS_MAP_02_047: [If during cloning, any operation fails, then Map_Clone shall return NULL.] */
    TEST_FUNCTION(Map_Clone_with_map_with_2_element_fails_when_gballoc_fails_4)
    {
        ///arrange
        CMapMocks mocks;
        auto handle = Map_Create(NULL);
        (void)Map_AddOrUpdate(handle, TEST_REDKEY, TEST_REDVALUE);
        (void)Map_AddOrUpdate(handle, TEST_BLUEKEY, TEST_BLUEVALUE);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is creating the HANDLE structure*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(2 * sizeof(char*))); /*this is creating a clone of the storage for keys*/
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_REDKEY) + 1)); /*this is creating a clone of RED key*/
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        whenShallmalloc_fail = currentmalloc_call + 4;
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_BLUEKEY) + 1)); /*this is creating a clone of BLUE key*/

        ///act
        auto result = Map_Clone(handle);

        ///assert
        ASSERT_IS_NULL(result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        Map_Destroy(handle);
        Map_Destroy(result);
    }

    /*Tests_SRS_MAP_02_047: [If during cloning, any operation fails, then Map_Clone shall return NULL.] */
    TEST_FUNCTION(Map_Clone_with_map_with_2_element_fails_when_gballoc_fails_5)
    {
        ///arrange
        CMapMocks mocks;
        auto handle = Map_Create(NULL);
        (void)Map_AddOrUpdate(handle, TEST_REDKEY, TEST_REDVALUE);
        (void)Map_AddOrUpdate(handle, TEST_BLUEKEY, TEST_BLUEVALUE);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is creating the HANDLE structure*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(2 * sizeof(char*))); /*this is creating a clone of the storage for keys*/
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        whenShallmalloc_fail = currentmalloc_call + 3;
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_REDKEY) + 1)); /*this is creating a clone of RED key*/

        ///act
        auto result = Map_Clone(handle);

        ///assert
        ASSERT_IS_NULL(result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        Map_Destroy(handle);
        Map_Destroy(result);
    }

    /*Tests_SRS_MAP_02_047: [If during cloning, any operation fails, then Map_Clone shall return NULL.] */
    TEST_FUNCTION(Map_Clone_with_map_with_2_element_fails_when_gballoc_fails_6)
    {
        ///arrange
        CMapMocks mocks;
        auto handle = Map_Create(NULL);
        (void)Map_AddOrUpdate(handle, TEST_REDKEY, TEST_REDVALUE);
        (void)Map_AddOrUpdate(handle, TEST_BLUEKEY, TEST_BLUEVALUE);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is creating the HANDLE structure*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        whenShallmalloc_fail = currentmalloc_call + 2;
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(2 * sizeof(char*))); /*this is creating a clone of the storage for keys*/

        ///act
        auto result = Map_Clone(handle);

        ///assert
        ASSERT_IS_NULL(result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        Map_Destroy(handle);
        Map_Destroy(result);
    }

    /*Tests_SRS_MAP_02_047: [If during cloning, any operation fails, then Map_Clone shall return NULL.] */
    TEST_FUNCTION(Map_Clone_with_map_with_2_element_fails_when_gballoc_fails_7)
    {
        ///arrange
        CMapMocks mocks;
        auto handle = Map_Create(NULL);
        (void)Map_AddOrUpdate(handle, TEST_REDKEY, TEST_REDVALUE);
        (void)Map_AddOrUpdate(handle, TEST_BLUEKEY, TEST_BLUEVALUE);
        mocks.ResetAllCalls();

        whenShallmalloc_fail = currentmalloc_call + 1;
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is creating the HANDLE structure*/
            .IgnoreArgument(1);

        ///act
        auto result = Map_Clone(handle);

        ///assert
        ASSERT_IS_NULL(result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        Map_Destroy(handle);
        Map_Destroy(result);
    }

    /* Tests_SRS_MAP_07_009: [If the mapFilterCallback function is not NULL, then the return value will be check and if it is not zero then Map_Add shall return MAP_FILTER_REJECT.] */
    TEST_FUNCTION(Map_Add_With_Filter_Succeed)
    {
        ///arrange
        CMapMocks mocks;
        auto handle = Map_Create(DontAllowCapitalsFilters);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, gballoc_realloc(NULL, sizeof(const char*)));
        STRICT_EXPECTED_CALL(mocks, gballoc_realloc(NULL, sizeof(const char*)));
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_GREENKEY) + 1));
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_GREENVALUE) + 1));

        ///act
        auto result1 = Map_Add(handle, TEST_REDKEY, TEST_REDVALUE);
        auto result2 = Map_Add(handle, TEST_YELLOWKEY, TEST_YELLOWVALUE);
        auto result3 = Map_Add(handle, TEST_GREENKEY, TEST_GREENVALUE);

        ///assert
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_FILTER_REJECT, result1);
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_FILTER_REJECT, result2);
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_OK, result3);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        Map_Destroy(handle);
    }

    /* Tests_SRS_MAP_07_008: [If the mapFilterCallback function is not NULL, then the return value will be check and if it is not zero then Map_AddOrUpdate shall return MAP_FILTER_REJECT.] */
    TEST_FUNCTION(Map_AddOrUpdate_With_Filter_Succeed)
    {
        ///arrange
        CMapMocks mocks;
        auto handle = Map_Create(DontAllowCapitalsFilters);
        mocks.ResetAllCalls();

        EXPECTED_CALL(mocks, gballoc_realloc(NULL, sizeof(const char*)));
        EXPECTED_CALL(mocks, gballoc_realloc(NULL, sizeof(const char*)));
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_GREENKEY) + 1));
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_GREENVALUE) + 1));

        ///act
        auto result1 = Map_AddOrUpdate(handle, TEST_REDKEY, TEST_REDVALUE);
        auto result2 = Map_AddOrUpdate(handle, TEST_GREENKEY, TEST_GREENVALUE);

        ///assert
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_FILTER_REJECT, result1);
        ASSERT_ARE_EQUAL(MAP_RESULT, MAP_OK, result2);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        Map_Destroy(handle);
    }

END_TEST_SUITE(map_unittests)
