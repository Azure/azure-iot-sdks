// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#include "gballoc.h"
#include "testrunnerswitcher.h"
#include "micromock.h"
#include "lock.h"

#ifndef SIZE_MAX
#define SIZE_MAX ((size_t)~(size_t)0)
#endif


static MICROMOCK_MUTEX_HANDLE g_testByTest;

static void* TEST_ALLOC_PTR1 = (void*)0x4242;
static const LOCK_HANDLE TEST_LOCK_HANDLE = (LOCK_HANDLE)0x4244;

TYPED_MOCK_CLASS(CGBAllocMocks, CGlobalMock)
{
public:
    MOCK_STATIC_METHOD_1(, void*, mock_malloc, size_t, size)
    MOCK_METHOD_END(void*, TEST_ALLOC_PTR1)
    MOCK_STATIC_METHOD_2(, void*, mock_calloc, size_t, nmemb, size_t, size)
    MOCK_METHOD_END(void*, TEST_ALLOC_PTR1)
    MOCK_STATIC_METHOD_2(, void*, mock_realloc, void*, ptr, size_t, size)
    MOCK_METHOD_END(void*, TEST_ALLOC_PTR1)
    MOCK_STATIC_METHOD_1(, void, mock_free, void*, ptr)
    MOCK_VOID_METHOD_END()

    /* Lock Mocks */
    MOCK_STATIC_METHOD_0(, LOCK_HANDLE, Lock_Init)
    MOCK_METHOD_END(LOCK_HANDLE, TEST_LOCK_HANDLE)
    MOCK_STATIC_METHOD_1(, LOCK_RESULT, Lock_Deinit, LOCK_HANDLE, handle)
    MOCK_METHOD_END(LOCK_RESULT, LOCK_OK)
    MOCK_STATIC_METHOD_1(, LOCK_RESULT, Lock, LOCK_HANDLE, handle)
    MOCK_METHOD_END(LOCK_RESULT, LOCK_OK)
    MOCK_STATIC_METHOD_1(, LOCK_RESULT, Unlock, LOCK_HANDLE, handle)
    MOCK_METHOD_END(LOCK_RESULT, LOCK_OK)
};

extern "C"
{
    DECLARE_GLOBAL_MOCK_METHOD_1(CGBAllocMocks, , void*, mock_malloc, size_t, size);
    DECLARE_GLOBAL_MOCK_METHOD_2(CGBAllocMocks, , void*, mock_calloc, size_t, nmemb, size_t, size);
    DECLARE_GLOBAL_MOCK_METHOD_2(CGBAllocMocks, , void*, mock_realloc, void*, ptr, size_t, size);
    DECLARE_GLOBAL_MOCK_METHOD_1(CGBAllocMocks, , void, mock_free, void*, ptr);

    DECLARE_GLOBAL_MOCK_METHOD_0(CGBAllocMocks, , LOCK_HANDLE, Lock_Init);
    DECLARE_GLOBAL_MOCK_METHOD_1(CGBAllocMocks, , LOCK_RESULT, Lock_Deinit, LOCK_HANDLE, handle);
    DECLARE_GLOBAL_MOCK_METHOD_1(CGBAllocMocks, , LOCK_RESULT, Lock, LOCK_HANDLE, handle);
    DECLARE_GLOBAL_MOCK_METHOD_1(CGBAllocMocks, , LOCK_RESULT, Unlock, LOCK_HANDLE, handle);
}

static MICROMOCK_GLOBAL_SEMAPHORE_HANDLE g_dllByDll;

BEGIN_TEST_SUITE(GBAlloc_For_Init_UnitTests)

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
}

/* gballoc_deinit */

/* Tests_SRS_GBALLOC_01_029: [if gballoc is not initialized gballoc_deinit shall do nothing.] */
TEST_FUNCTION(when_gballoc_is_not_initialized_gballoc_deinit_doesnot_free_lock)
{
    //arrange
    CGBAllocMocks mocks;

    // act
    gballoc_deinit();

    // assert
    // no explicit assert, uMock checks the calls
}

/* gballoc_malloc */

/* Tests_SRS_GBALLOC_01_039: [If gballoc was not initialized gballoc_malloc shall simply call malloc without any memory tracking being performed.] */
TEST_FUNCTION(when_gballoc_is_not_initialized_gballoc_malloc_calls_crt_malloc)
{
    // arrange
    CGBAllocMocks mocks;

    STRICT_EXPECTED_CALL(mocks, mock_malloc(1));

    //act
    void* result = gballoc_malloc(1);

    //assert
    ASSERT_IS_NOT_NULL(result);
}

/* gballoc_calloc */

/* Tests_SRS_GBALLOC_01_040: [If gballoc was not initialized gballoc_calloc shall simply call calloc without any memory tracking being performed.] */
TEST_FUNCTION(when_gballoc_is_not_initialized_then_gballoc_calloc_calls_crt_calloc)
{
    // arrange
    CGBAllocMocks mocks;

    STRICT_EXPECTED_CALL(mocks, mock_calloc(1, 1));

    // act
    void* result = gballoc_calloc(1, 1);

    // assert
    ASSERT_IS_NOT_NULL(result);
}

/* gballoc_realloc */

/* Tests_SRS_GBALLOC_01_041: [If gballoc was not initialized gballoc_realloc shall shall simply call realloc without any memory tracking being performed.] */
TEST_FUNCTION(when_gballoc_is_not_initialized_then_gballoc_realloc_calls_crt_realloc)
{
    // arrange
    CGBAllocMocks mocks;

    STRICT_EXPECTED_CALL(mocks, mock_realloc(NULL, 1));

    // act
    void* result = gballoc_realloc(NULL, 1);

    // assert
    ASSERT_IS_NOT_NULL(result);
}

/* gballoc_free */

/* Tests_SRS_GBALLOC_01_042: [If gballoc was not initialized gballoc_free shall shall simply call free.] */
TEST_FUNCTION(when_gballoc_is_not_initialized_then_gballoc_free_does_nothing)
{
    // arrange
    CGBAllocMocks mocks;

    STRICT_EXPECTED_CALL(mocks, mock_free((void*)0x4242));

    // act
    gballoc_free((void*)0x4242);

    // assert
    // no explicit assert, uMock checks the calls
}

/* gballoc_getMaximumMemoryUsed */

/* Tests_SRS_GBALLOC_01_038: [If gballoc was not initialized gballoc_getMaximumMemoryUsed shall return MAX_INT_SIZE.]  */
TEST_FUNCTION(without_gballoc_being_initialized_gballoc_getMaximumMemoryUsed_fails)
{
    // arrange
    CGBAllocMocks mocks;

    // act
    size_t result = gballoc_getMaximumMemoryUsed();

    // assert
    ASSERT_ARE_EQUAL(size_t, SIZE_MAX, result);
}

/* gballoc_getCurrentMemoryUsed */

/* Tests_SRS_GBALLOC_01_044: [If gballoc was not initialized gballoc_getCurrentMemoryUsed shall return SIZE_MAX.] */
TEST_FUNCTION(without_gballoc_being_initialized_gballoc_getCurrentMemoryUsed_fails)
{
    // arrange
    CGBAllocMocks mocks;

    // act
    size_t result = gballoc_getCurrentMemoryUsed();

    // assert
    ASSERT_ARE_EQUAL(size_t, SIZE_MAX, result);
}

END_TEST_SUITE(GBAlloc_For_Init_UnitTests)
