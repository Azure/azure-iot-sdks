// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#if defined(GB_MEASURE_MEMORY_FOR_THIS)
#undef GB_MEASURE_MEMORY_FOR_THIS
#endif

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
static void* TEST_ALLOC_PTR2 = (void*)0x4243;

#define OVERHEAD_SIZE	4096
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

BEGIN_TEST_SUITE(GBAlloc_UnitTests)

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
    CGBAllocMocks mocks;
    mocks.SetPerformAutomaticCallComparison(AUTOMATIC_CALL_COMPARISON_OFF);
    gballoc_deinit();

    if (!MicroMockReleaseMutex(g_testByTest))
    {
        ASSERT_FAIL("failure in test framework at ReleaseMutex");
    }
}


/* gballoc_init */

/* Tests_SRS_GBALLOC_01_002: [Upon initialization the total memory used and maximum total memory used tracked by the module shall be set to 0.] */
TEST_FUNCTION(gballoc_init_resets_memory_used)
{
    //arrange
    CGBAllocMocks mocks;
    mocks.SetPerformAutomaticCallComparison(AUTOMATIC_CALL_COMPARISON_OFF);
    gballoc_init();
    void* allocation = malloc(OVERHEAD_SIZE);

    EXPECTED_CALL(mocks, mock_malloc(0))
        .SetReturn(allocation);
    gballoc_free(gballoc_malloc(1));
    
    gballoc_deinit();

    // act
    gballoc_init();

    // assert
    ASSERT_ARE_EQUAL(size_t, 0, gballoc_getMaximumMemoryUsed());
    ASSERT_ARE_EQUAL(size_t, 0, gballoc_getCurrentMemoryUsed());

    ///cleanup
    free(allocation);
}

/* Tests_SRS_GBALLOC_01_024: [gballoc_init shall initialize the gballoc module and return 0 upon success.] */
/* Tests_SRS_GBALLOC_01_026: [gballoc_Init shall create a lock handle that will be used to make the other gballoc APIs thread-safe.] */
TEST_FUNCTION(when_gballoc_init_calls_lock_init_and_it_succeeds_then_gballoc_init_succeeds)
{
    // arrange
    CGBAllocMocks mocks;
    STRICT_EXPECTED_CALL(mocks, Lock_Init());

    // act
    int result = gballoc_init();

    // assert
    ASSERT_ARE_EQUAL(int, 0, result);
}

/* Tests_SRS_GBALLOC_01_027: [If the Lock creation fails, gballoc_init shall return a non-zero value.] */
TEST_FUNCTION(when_lock_init_fails_gballoc_init_fails)
{
    // arrange
    CGBAllocMocks mocks;
    STRICT_EXPECTED_CALL(mocks, Lock_Init())
        .SetReturn((LOCK_HANDLE)NULL);

    // act
    int result = gballoc_init();

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
}

/* Tests_SRS_GBALLOC_01_025: [Init after Init shall fail and return a non-zero value.] */
TEST_FUNCTION(gballoc_init_after_gballoc_init_fails)
{
    // arrange
    CGBAllocMocks mocks;
    STRICT_EXPECTED_CALL(mocks, Lock_Init());
    gballoc_init();

    //act
    int result = gballoc_init();

    // assert
    ASSERT_ARE_NOT_EQUAL(int, 0, result);
}

/* gballoc_deinit */

/* Tests_SRS_GBALLOC_01_028: [gballoc_deinit shall free all resources allocated by gballoc_init.] */
TEST_FUNCTION(gballoc_deinit_frees_the_lock_when_the_module_was_initialized)
{
    // arrange
    CGBAllocMocks mocks;
    gballoc_init();
    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, Lock_Deinit(TEST_LOCK_HANDLE));

    // act
    gballoc_deinit();

    // assert
    // no explicit assert, uMock checks the calls
}

/* Tests_SRS_GBALLOC_01_029: [if gballoc is not initialized gballoc_deinit shall do nothing.] */
TEST_FUNCTION(gballoc_deinit_after_gballoc_deinit_doesnot_free_lock)
{
    //arrange
    CGBAllocMocks mocks;

    // act
    gballoc_deinit();

    // assert
    // no explicit assert, uMock checks the calls
}

/* gballoc_malloc */

/* Tests_SRS_GBALLOC_01_048: [If acquiring the lock fails, gballoc_malloc shall return NULL.] */
TEST_FUNCTION(when_acquiring_the_lock_fails_gballoc_malloc_fails)
{
    // arrange
    CGBAllocMocks mocks;
    gballoc_init();
    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE))
        .SetReturn(LOCK_ERROR);

    // act
    void* result = gballoc_malloc(1);

    // assert
    ASSERT_IS_NULL(result);
}

/* Tests_SRS_GBALLOC_01_003: [gballoc_malloc shall call the C99 malloc function and return its result.] */
/* Tests_SRS_GBALLOC_01_004: [If the underlying malloc call is successful, gballoc_malloc shall increment the total memory used with the amount indicated by size.] */
/* Tests_SRS_GBALLOC_01_030: [gballoc_malloc shall ensure thread safety by using the lock created by gballoc_Init.] */
TEST_FUNCTION(gballoc_malloc_with_0_Size_Calls_Underlying_malloc)
{
    // arrange
    CGBAllocMocks mocks;
    gballoc_init();
    mocks.ResetAllCalls();
    void* allocation = malloc(OVERHEAD_SIZE);

    STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
    /* don't quite like this, but I'm unsure I want to invest more in this memory counting */
    EXPECTED_CALL(mocks, mock_malloc(0))
        .SetReturn(allocation);
    /* This is the call to the underlying malloc with the size we want to allocate */
    STRICT_EXPECTED_CALL(mocks, mock_malloc(0));
    STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));

    // act
    void* result = gballoc_malloc(0);

    // assert
    ASSERT_ARE_EQUAL(void_ptr, TEST_ALLOC_PTR1, result);
    mocks.AssertActualAndExpectedCalls();
    ASSERT_ARE_EQUAL(size_t, 0, gballoc_getMaximumMemoryUsed());

    // cleanup
    gballoc_free(result);
    free(allocation);
}

/* Tests_SRS_GBALLOC_01_003: [gballoc_malloc shall call the C99 malloc function and return its result.] */
/* Tests_SRS_GBALLOC_01_004: [If the underlying malloc call is successful, gballoc_malloc shall increment the total memory used with the amount indicated by size.] */
TEST_FUNCTION(gballoc_malloc_with_1_Size_Calls_Underlying_malloc_And_Increases_Max_Used)
{
    // arrange
    CGBAllocMocks mocks;
    gballoc_init();
    mocks.ResetAllCalls();
    void* allocation = malloc(OVERHEAD_SIZE);

    STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
    /* don't quite like this, but I'm unsure I want to invest more in this memory counting */
    EXPECTED_CALL(mocks, mock_malloc(0))
        .SetReturn(allocation);
    /* This is the call to the underlying malloc with the size we want to allocate */
    STRICT_EXPECTED_CALL(mocks, mock_malloc(1));
    STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));

    // act
    void* result = gballoc_malloc(1);

    // assert
    ASSERT_ARE_EQUAL(void_ptr, TEST_ALLOC_PTR1, result);
    mocks.AssertActualAndExpectedCalls();
    ASSERT_ARE_EQUAL(size_t, 1, gballoc_getMaximumMemoryUsed());

    // cleanup
    gballoc_free(result);
    free(allocation);
}

/* Tests_SRS_GBALLOC_01_012: [When the underlying malloc call fails, gballoc_malloc shall return NULL and size should not be counted towards total memory used.] */
TEST_FUNCTION(When_malloc_Fails_Then_gballoc_malloc_fails_too)
{
    // arrange
    CGBAllocMocks mocks;
    gballoc_init();
    mocks.ResetAllCalls();
    void* allocation = malloc(OVERHEAD_SIZE);

    STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
    /* don't quite like this, but I'm unsure I want to invest more in this memory counting */
    EXPECTED_CALL(mocks, mock_malloc(0))
        .SetReturn(allocation);
    /* This is the call to the underlying malloc with the size we want to allocate */
    STRICT_EXPECTED_CALL(mocks, mock_malloc(1))
        .SetReturn((void*)NULL);
    STRICT_EXPECTED_CALL(mocks, mock_free(allocation));
    STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));

    // act
    void* result = gballoc_malloc(1);

    // assert
    ASSERT_IS_NULL(result);
    mocks.AssertActualAndExpectedCalls();
    ASSERT_ARE_EQUAL(size_t, 0, gballoc_getMaximumMemoryUsed());

    // cleanup
    free(allocation);
}

/* Tests_SRS_GBALLOC_01_013: [When gballoc_malloc fails allocating memory for its internal use, gballoc_malloc shall return NULL.] */
TEST_FUNCTION(When_allocating_memory_for_tracking_information_fails_Then_gballoc_malloc_fails_too)
{
    // arrange
    CGBAllocMocks mocks;
    gballoc_init();
    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
    /* don't quite like this, but I'm unsure I want to invest more in this memory counting */
    EXPECTED_CALL(mocks, mock_malloc(0))
        .SetReturn((void*)NULL);
    STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));

    // act
    void* result = gballoc_malloc(1);

    // assert
    ASSERT_IS_NULL(result);
    mocks.AssertActualAndExpectedCalls();
    ASSERT_ARE_EQUAL(size_t, 0, gballoc_getMaximumMemoryUsed());
}

/* Tests_SRS_GBALLOC_01_039: [If gballoc was not initialized gballoc_malloc shall simply call malloc without any memory tracking being performed.] */
TEST_FUNCTION(gballoc_malloc_after_deinit_calls_crt_malloc)
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

/* Tests_SRS_GBALLOC_01_046: [If acquiring the lock fails, gballoc_calloc shall return NULL.] */
TEST_FUNCTION(when_acquiring_the_lock_fails_gballoc_calloc_fails)
{
    // arrange
    CGBAllocMocks mocks;
    gballoc_init();
    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE))
    .SetReturn(LOCK_ERROR);

    // act
    void* result = gballoc_calloc(1,1);

    // assert
    ASSERT_IS_NULL(result);
}

/* Tests_SRS_GBALLOC_01_020: [gballoc_calloc shall call the C99 calloc function and return its result.] */
/* Tests_SRS_GBALLOC_01_021: [If the underlying calloc call is successful, gballoc_calloc shall increment the total memory used with nmemb*size.] */
/* Tests_SRS_GBALLOC_01_031: [gballoc_calloc shall ensure thread safety by using the lock created by gballoc_Init] */
TEST_FUNCTION(gballoc_calloc_with_0_Size_And_ItemCount_Calls_Underlying_calloc)
{
    // arrange
    CGBAllocMocks mocks;
    gballoc_init();
    mocks.ResetAllCalls();
    void* allocation = malloc(OVERHEAD_SIZE);

    STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
    /* don't quite like this, but I'm unsure I want to invest more in this memory counting */
    EXPECTED_CALL(mocks, mock_malloc(0))
        .SetReturn(allocation);
    /* This is the call to the underlying malloc with the size we want to allocate */
    STRICT_EXPECTED_CALL(mocks, mock_calloc(0, 0));
    STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));

    // act
    void* result = gballoc_calloc(0, 0);

    // assert
    ASSERT_ARE_EQUAL(void_ptr, TEST_ALLOC_PTR1, result);
    mocks.AssertActualAndExpectedCalls();
    ASSERT_ARE_EQUAL(size_t, 0, gballoc_getMaximumMemoryUsed());

    // cleanup
    gballoc_free(result);
    free(allocation);
}

/* Tests_SRS_GBALLOC_01_020: [gballoc_calloc shall call the C99 calloc function and return its result.] */
/* Tests_SRS_GBALLOC_01_021: [If the underlying calloc call is successful, gballoc_calloc shall increment the total memory used with nmemb*size.] */
TEST_FUNCTION(gballoc_calloc_with_1_Item_Of_1_Size_Calls_Underlying_malloc_And_Increases_Max_Used)
{
    // arrange
    CGBAllocMocks mocks;
    gballoc_init();
    mocks.ResetAllCalls();
    void* allocation = malloc(OVERHEAD_SIZE);

    STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
    /* don't quite like this, but I'm unsure I want to invest more in this memory counting */
    EXPECTED_CALL(mocks, mock_malloc(0))
        .SetReturn(allocation);
    /* This is the call to the underlying malloc with the size we want to allocate */
    STRICT_EXPECTED_CALL(mocks, mock_calloc(1, 1));
    STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));

    // act
    void* result = gballoc_calloc(1, 1);

    // assert
    ASSERT_ARE_EQUAL(void_ptr, TEST_ALLOC_PTR1, result);
    mocks.AssertActualAndExpectedCalls();
    ASSERT_ARE_EQUAL(size_t, 1, gballoc_getMaximumMemoryUsed());

    // cleanup
    gballoc_free(result);
    free(allocation);
}

/* Tests_SRS_GBALLOC_01_020: [gballoc_calloc shall call the C99 calloc function and return its result.] */
/* Tests_SRS_GBALLOC_01_021: [If the underlying calloc call is successful, gballoc_calloc shall increment the total memory used with nmemb*size.] */
TEST_FUNCTION(gballoc_calloc_with_1_Item_Of_0_Size_Calls_Underlying_malloc_And_Does_Not_Increase_Max_Used)
{
    // arrange
    CGBAllocMocks mocks;
    gballoc_init();
    mocks.ResetAllCalls();
    void* allocation = malloc(OVERHEAD_SIZE);

    STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
    /* don't quite like this, but I'm unsure I want to invest more in this memory counting */
    EXPECTED_CALL(mocks, mock_malloc(0))
        .SetReturn(allocation);
    /* This is the call to the underlying malloc with the size we want to allocate */
    STRICT_EXPECTED_CALL(mocks, mock_calloc(1, 0));
    STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));

    // act
    void* result = gballoc_calloc(1, 0);

    // assert
    ASSERT_ARE_EQUAL(void_ptr, TEST_ALLOC_PTR1, result);
    mocks.AssertActualAndExpectedCalls();
    ASSERT_ARE_EQUAL(size_t, 0, gballoc_getMaximumMemoryUsed());

    // cleanup
    gballoc_free(result);
    free(allocation);
}

/* Tests_SRS_GBALLOC_01_020: [gballoc_calloc shall call the C99 calloc function and return its result.] */
/* Tests_SRS_GBALLOC_01_021: [If the underlying calloc call is successful, gballoc_calloc shall increment the total memory used with nmemb*size.] */
TEST_FUNCTION(gballoc_calloc_with_0_Items_Of_1_Size_Calls_Underlying_malloc_And_Does_Not_Increase_Max_Used)
{
    // arrange
    CGBAllocMocks mocks;
    gballoc_init();
    mocks.ResetAllCalls();
    void* allocation = malloc(OVERHEAD_SIZE);

    STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
    /* don't quite like this, but I'm unsure I want to invest more in this memory counting */
    EXPECTED_CALL(mocks, mock_malloc(0))
        .SetReturn(allocation);
    /* This is the call to the underlying malloc with the size we want to allocate */
    STRICT_EXPECTED_CALL(mocks, mock_calloc(0, 1));
    STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));

    // act
    void* result = gballoc_calloc(0, 1);

    // assert
    ASSERT_ARE_EQUAL(void_ptr, TEST_ALLOC_PTR1, result);
    mocks.AssertActualAndExpectedCalls();
    ASSERT_ARE_EQUAL(size_t, 0, gballoc_getMaximumMemoryUsed());

    // cleanup
    gballoc_free(result);
    free(allocation);
}

/* Tests_SRS_GBALLOC_01_020: [gballoc_calloc shall call the C99 calloc function and return its result.] */
/* Tests_SRS_GBALLOC_01_021: [If the underlying calloc call is successful, gballoc_calloc shall increment the total memory used with nmemb*size.] */
TEST_FUNCTION(gballoc_calloc_with_42_Items_Of_2_Size_Calls_Underlying_malloc_And_Increases_Max_Size)
{
    // arrange
    CGBAllocMocks mocks;
    gballoc_init();
    mocks.ResetAllCalls();
    void* allocation = malloc(OVERHEAD_SIZE);

    STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
    /* don't quite like this, but I'm unsure I want to invest more in this memory counting */
    EXPECTED_CALL(mocks, mock_malloc(0))
        .SetReturn(allocation);
    /* This is the call to the underlying malloc with the size we want to allocate */
    STRICT_EXPECTED_CALL(mocks, mock_calloc(42, 2));
    STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));

    // act
    void* result = gballoc_calloc(42, 2);

    // assert
    ASSERT_ARE_EQUAL(void_ptr, TEST_ALLOC_PTR1, result);
    mocks.AssertActualAndExpectedCalls();
    ASSERT_ARE_EQUAL(size_t, 84, gballoc_getMaximumMemoryUsed());

    // cleanup
    gballoc_free(result);
    free(allocation);
}

/* Tests_SRS_GBALLOC_01_022: [When the underlying calloc call fails, gballoc_calloc shall return NULL and size should not be counted towards total memory used.] */
TEST_FUNCTION(When_calloc_Fails_Then_gballoc_calloc_fails_too)
{
    // arrange
    CGBAllocMocks mocks;
    gballoc_init();
    mocks.ResetAllCalls();
    void* allocation = malloc(OVERHEAD_SIZE);

    STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
    /* don't quite like this, but I'm unsure I want to invest more in this memory counting */
    EXPECTED_CALL(mocks, mock_malloc(0))
        .SetReturn(allocation);
    /* This is the call to the underlying malloc with the size we want to allocate */
    STRICT_EXPECTED_CALL(mocks, mock_calloc(1, 1))
        .SetReturn((void*)NULL);
    STRICT_EXPECTED_CALL(mocks, mock_free(allocation));
    STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));

    // act
    void* result = gballoc_calloc(1, 1);

    // assert
    ASSERT_IS_NULL(result);
    mocks.AssertActualAndExpectedCalls();
    ASSERT_ARE_EQUAL(size_t, 0, gballoc_getMaximumMemoryUsed());

    // cleanup
    free(allocation);
}

/* Tests_SRS_GBALLOC_01_023: [When gballoc_calloc fails allocating memory for its internal use, gballoc_calloc shall return NULL.] */
TEST_FUNCTION(When_allocating_memory_for_tracking_information_fails_Then_gballoc_calloc_fails_too)
{
    // arrange
    CGBAllocMocks mocks;
    gballoc_init();
    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
    /* don't quite like this, but I'm unsure I want to invest more in this memory counting */
    EXPECTED_CALL(mocks, mock_malloc(0))
        .SetReturn((void*)NULL);
    STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));

    // act
    void* result = gballoc_calloc(1, 1);

    // assert
    ASSERT_IS_NULL(result);
    mocks.AssertActualAndExpectedCalls();
    ASSERT_ARE_EQUAL(size_t, 0, gballoc_getMaximumMemoryUsed());
}

/* Tests_SRS_GBALLOC_01_040: [If gballoc was not initialized gballoc_calloc shall simply call calloc without any memory tracking being performed.] */
TEST_FUNCTION(gballoc_calloc_after_deinit_calls_crt_calloc)
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

/* Tests_SRS_GBALLOC_01_047: [If acquiring the lock fails, gballoc_realloc shall return NULL.]*/
TEST_FUNCTION(when_acquiring_the_lock_fails_gballoc_realloc_fails)
{
    // arrange
    CGBAllocMocks mocks;
    gballoc_init();
    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE))
        .SetReturn(LOCK_ERROR);

    // act
    void* result = gballoc_realloc(NULL, 1);

    // assert
    ASSERT_IS_NULL(result);
}

/* Tests_SRS_GBALLOC_01_041: [If gballoc was not initialized gballoc_realloc shall shall simply call realloc without any memory tracking being performed.] */
TEST_FUNCTION(gballoc_realloc_after_deinit_fails)
{
    // arrange
    CGBAllocMocks mocks;

    STRICT_EXPECTED_CALL(mocks, mock_realloc(NULL, 1));

    // act
    void* result = gballoc_realloc(NULL, 1);

    // assert
    ASSERT_IS_NOT_NULL(result);
}

/* Tests_SRS_GBALLOC_01_005: [gballoc_realloc shall call the C99 realloc function and return its result.] */
/* Tests_SRS_GBALLOC_01_017: [When ptr is NULL, gballoc_realloc shall call the underlying realloc with ptr being NULL and the realloc result shall be tracked by gballoc.] */
/* Tests_SRS_GBALLOC_01_032: [gballoc_realloc shall ensure thread safety by using the lock created by gballoc_Init.] */
TEST_FUNCTION(gballoc_realloc_with_NULL_Arg_And_0_Size_Calls_Underlying_realloc)
{
    // arrange
    CGBAllocMocks mocks;
    gballoc_init();
    mocks.ResetAllCalls();
    void* allocation = malloc(OVERHEAD_SIZE);

    STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
    /* don't quite like this, but I'm unsure I want to invest more in this memory counting */
    EXPECTED_CALL(mocks, mock_malloc(0))
        .SetReturn(allocation);
    /* This is the call to the underlying malloc with the size we want to allocate */
    STRICT_EXPECTED_CALL(mocks, mock_realloc(NULL, 0));
    STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));

    // act
    void* result = gballoc_realloc(NULL, 0);

    // assert
    ASSERT_ARE_EQUAL(void_ptr, TEST_ALLOC_PTR1, result);
    mocks.AssertActualAndExpectedCalls();
    ASSERT_ARE_EQUAL(size_t, 0, gballoc_getMaximumMemoryUsed());

    // cleanup
    gballoc_free(result);
    free(allocation);
}

/* Tests_SRS_GBALLOC_01_005: [gballoc_realloc shall call the C99 realloc function and return its result.] */
/* Tests_SRS_GBALLOC_01_017: [When ptr is NULL, gballoc_realloc shall call the underlying realloc with ptr being NULL and the realloc result shall be tracked by gballoc.] */
TEST_FUNCTION(gballoc_realloc_with_NULL_Arg_And_1_Size_Calls_Underlying_realloc)
{
    // arrange
    CGBAllocMocks mocks;
    gballoc_init();
    mocks.ResetAllCalls();
    void* allocation = malloc(OVERHEAD_SIZE);

    STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
    /* don't quite like this, but I'm unsure I want to invest more in this memory counting */
    EXPECTED_CALL(mocks, mock_malloc(0))
        .SetReturn(allocation);
    /* This is the call to the underlying malloc with the size we want to allocate */
    STRICT_EXPECTED_CALL(mocks, mock_realloc(NULL, 1));
    STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));

    // act
    void* result = gballoc_realloc(NULL, 1);

    // assert
    ASSERT_ARE_EQUAL(void_ptr, TEST_ALLOC_PTR1, result);
    mocks.AssertActualAndExpectedCalls();
    ASSERT_ARE_EQUAL(size_t, 1, gballoc_getMaximumMemoryUsed());

    // cleanup
    gballoc_free(result);
    free(allocation);
}

/* Tests_SRS_GBALLOC_01_005: [gballoc_realloc shall call the C99 realloc function and return its result.] */
/* Tests_SRS_GBALLOC_01_006: [If the underlying realloc call is successful, gballoc_realloc shall look up the size associated with the pointer ptr and decrease the total memory used with that size.] */
TEST_FUNCTION(gballoc_realloc_with_Previous_1_Byte_Block_Ptr_And_2_Size_Calls_Underlying_realloc_And_Increases_Max_Used_Memory)
{
    // arrange
    CGBAllocMocks mocks;
    gballoc_init();
    mocks.ResetAllCalls();
    void* allocation = malloc(OVERHEAD_SIZE);

    STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
    /* don't quite like this, but I'm unsure I want to invest more in this memory counting */
    EXPECTED_CALL(mocks, mock_malloc(0))
        .SetReturn(allocation);
    /* This is the call to the underlying malloc with the size we want to allocate */
    STRICT_EXPECTED_CALL(mocks, mock_realloc(NULL, 1));
    STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));
    STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
    STRICT_EXPECTED_CALL(mocks, mock_realloc(TEST_ALLOC_PTR1, 2))
        .SetReturn(TEST_ALLOC_PTR2);
    STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));

    void* result = gballoc_realloc(NULL, 1);

    // act
    result = gballoc_realloc(result, 2);

    // assert
    ASSERT_ARE_EQUAL(void_ptr, TEST_ALLOC_PTR2, result);
    mocks.AssertActualAndExpectedCalls();
    ASSERT_ARE_EQUAL(size_t, 2, gballoc_getMaximumMemoryUsed());

    // cleanup
    gballoc_free(result);
    free(allocation);
}

/* Tests_SRS_GBALLOC_01_014: [When the underlying realloc call fails, gballoc_realloc shall return NULL and no change should be made to the counted total memory usage.] */
TEST_FUNCTION(When_realloc_fails_then_gballoc_realloc_Fails_Too_And_No_Change_Is_Made_To_Memory_Counters)
{
    // arrange
    CGBAllocMocks mocks;
    gballoc_init();
    mocks.ResetAllCalls();
    void* allocation = malloc(OVERHEAD_SIZE);

    STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
    /* don't quite like this, but I'm unsure I want to invest more in this memory counting */
    EXPECTED_CALL(mocks, mock_malloc(0))
        .SetReturn(allocation);
    /* This is the call to the underlying malloc with the size we want to allocate */
    STRICT_EXPECTED_CALL(mocks, mock_realloc(NULL, 1));
    STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));
    STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
    STRICT_EXPECTED_CALL(mocks, mock_realloc(TEST_ALLOC_PTR1, 2))
        .SetReturn((void*)NULL);
    STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));

    void* result = gballoc_realloc(NULL, 1);

    // act
    result = gballoc_realloc(result, 2);

    // assert
    ASSERT_IS_NULL(result);
    mocks.AssertActualAndExpectedCalls();
    ASSERT_ARE_EQUAL(size_t, 1, gballoc_getMaximumMemoryUsed());

    // cleanup
    gballoc_free(TEST_ALLOC_PTR1);
    free(allocation);
}

/* Tests_SRS_GBALLOC_01_015: [When allocating memory used for tracking by gballoc_realloc fails, gballoc_realloc shall return NULL and no change should be made to the counted total memory usage.] */
TEST_FUNCTION(When_Allocating_Memory_For_tracking_fails_gballoc_realloc_fails)
{
    // arrange
    CGBAllocMocks mocks;
    gballoc_init();
    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
    /* don't quite like this, but I'm unsure I want to invest more in this memory counting */
    EXPECTED_CALL(mocks, mock_malloc(0))
        .SetReturn((void*)NULL);
    STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));

    // act
    void* result = gballoc_realloc(NULL, 1);

    // assert
    ASSERT_IS_NULL(result);
    mocks.AssertActualAndExpectedCalls();
    ASSERT_ARE_EQUAL(size_t, 0, gballoc_getMaximumMemoryUsed());
}

/* Tests_SRS_GBALLOC_01_016: [When the ptr pointer cannot be found in the pointers tracked by gballoc, gballoc_realloc shall return NULL and the underlying realloc shall not be called.] */
TEST_FUNCTION(When_The_Pointer_Is_Not_Tracked_gballoc_realloc_Returns_NULL)
{
    // arrange
    CGBAllocMocks mocks;
    gballoc_init();
    mocks.ResetAllCalls();
    void* allocation = malloc(OVERHEAD_SIZE);

    STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
    /* don't quite like this, but I'm unsure I want to invest more in this memory counting */
    EXPECTED_CALL(mocks, mock_malloc(0))
        .SetReturn(allocation);
    /* This is the call to the underlying malloc with the size we want to allocate */
    STRICT_EXPECTED_CALL(mocks, mock_realloc(NULL, 1));
    STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));
    STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
    STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));

    void* result1 = gballoc_realloc(NULL, 1);

    // act
    void* result2 = gballoc_realloc((void*)0xDEADBEEF, 2);

    // assert
    ASSERT_IS_NULL(result2);
    mocks.AssertActualAndExpectedCalls();
    ASSERT_ARE_EQUAL(size_t, 1, gballoc_getMaximumMemoryUsed());

    // cleanup
    gballoc_free(result1);
    free(allocation);
}

/* Tests_SRS_GBALLOC_01_014: [When the underlying realloc call fails, gballoc_realloc shall return NULL and no change should be made to the counted total memory usage.] */
TEST_FUNCTION(When_ptr_is_null_and_the_underlying_realloc_fails_then_the_memory_used_for_tracking_is_freed)
{
    // arrange
    CGBAllocMocks mocks;
    gballoc_init();
    mocks.ResetAllCalls();
    void* allocation = malloc(OVERHEAD_SIZE);

    STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
    /* don't quite like this, but I'm unsure I want to invest more in this memory counting */
    EXPECTED_CALL(mocks, mock_malloc(0))
        .SetReturn(allocation);
    /* This is the call to the underlying malloc with the size we want to allocate */
    STRICT_EXPECTED_CALL(mocks, mock_realloc(NULL, 1))
        .SetReturn((void*)NULL);
    STRICT_EXPECTED_CALL(mocks, mock_free(allocation));
    STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));

    // act
    void* result = gballoc_realloc(NULL, 1);

    // assert
    ASSERT_IS_NULL(result);
    mocks.AssertActualAndExpectedCalls();
    ASSERT_ARE_EQUAL(size_t, 0, gballoc_getMaximumMemoryUsed());

    // cleanup
    free(allocation);
}

/* gballoc_free */

/* Tests_SRS_GBALLOC_01_042: [If gballoc was not initialized gballoc_free shall shall simply call free.] */
TEST_FUNCTION(gballoc_free_after_deinit_calls_crt_free)
{
    // arrange
    CGBAllocMocks mocks;

    STRICT_EXPECTED_CALL(mocks, mock_free((void*)0x4242));

    // act
    gballoc_free((void*)0x4242);

    // assert
    // no explicit assert, uMock checks the calls
}

/* Tests_SRS_GBALLOC_01_049: [If acquiring the lock fails, gballoc_free shall do nothing.] */
TEST_FUNCTION(when_acquiring_the_lock_fails_then_gballoc_free_does_nothing)
{
    // arrange
    CGBAllocMocks mocks;
    gballoc_init();
    mocks.ResetAllCalls();
    void* allocation = malloc(OVERHEAD_SIZE);

    /* don't quite like this, but I'm unsure I want to invest more in this memory counting */
    EXPECTED_CALL(mocks, mock_malloc(0))
        .SetReturn(allocation);
    /* This is the call to the underlying malloc with the size we want to allocate */
    STRICT_EXPECTED_CALL(mocks, mock_realloc(NULL, 1));
    void* block = gballoc_realloc(NULL, 1);
    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE))
        .SetReturn(LOCK_ERROR);

    // act
    gballoc_free(block);

    // assert
    mocks.AssertActualAndExpectedCalls();

    // cleanup
    gballoc_free(block);
    free(allocation);
}

/* Tests_SRS_GBALLOC_01_008: [gballoc_free shall call the C99 free function.] */
/* Tests_SRS_GBALLOC_01_009: [gballoc_free shall also look up the size associated with the ptr pointer and decrease the total memory used with the associated size amount.] */
/* Tests_SRS_GBALLOC_01_010: [gballoc_getMaximumMemoryUsed shall return the maximum amount of total memory used recorded since the module initialization.] */
/* Tests_SRS_GBALLOC_01_011: [The maximum total memory used shall be the maximum of the total memory used at any point.] */
/* Tests_SRS_GBALLOC_01_033: [gballoc_free shall ensure thread safety by using the lock created by gballoc_Init.] */
TEST_FUNCTION(gballoc_free_calls_the_underlying_free)
{
    // arrange
    CGBAllocMocks mocks;
    gballoc_init();
    mocks.ResetAllCalls();
    void* allocation = malloc(OVERHEAD_SIZE);

    /* don't quite like this, but I'm unsure I want to invest more in this memory counting */
    EXPECTED_CALL(mocks, mock_malloc(0))
        .SetReturn(allocation);
    /* This is the call to the underlying malloc with the size we want to allocate */
    STRICT_EXPECTED_CALL(mocks, mock_realloc(NULL, 1));
    void* block = gballoc_realloc(NULL, 1);
    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
    STRICT_EXPECTED_CALL(mocks, mock_free(TEST_ALLOC_PTR1));
    STRICT_EXPECTED_CALL(mocks, mock_free(allocation));
    STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));

    // act
    gballoc_free(block);

    // assert
    mocks.AssertActualAndExpectedCalls();
    ASSERT_ARE_EQUAL(size_t, 1, gballoc_getMaximumMemoryUsed());

    // cleanup
    free(allocation);
}

/* Tests_SRS_GBALLOC_01_008: [gballoc_free shall call the C99 free function.] */
/* Tests_SRS_GBALLOC_01_009: [gballoc_free shall also look up the size associated with the ptr pointer and decrease the total memory used with the associated size amount.] */
/* Tests_SRS_GBALLOC_01_010: [gballoc_getMaximumMemoryUsed shall return the maximum amount of total memory used recorded since the module initialization.] */
/* Tests_SRS_GBALLOC_01_010: [gballoc_getMaximumMemoryUsed shall return the maximum amount of total memory used recorded since the module initialization.] */
/* Tests_SRS_GBALLOC_01_011: [The maximum total memory used shall be the maximum of the total memory used at any point.] */
TEST_FUNCTION(gballoc_malloc_free_2_times_with_1_byte_yields_1_byte_as_max)
{
    // arrange
    CGBAllocMocks mocks;
    gballoc_init();
    void* allocation = malloc(OVERHEAD_SIZE);

    /* don't quite like this, but I'm unsure I want to invest more in this memory counting */
    EXPECTED_CALL(mocks, mock_malloc(0))
        .SetReturn(allocation);
    STRICT_EXPECTED_CALL(mocks, mock_realloc(NULL, 1));
    void* block = gballoc_realloc(NULL, 1);
    gballoc_free(block);

    /* don't quite like this, but I'm unsure I want to invest more in this memory counting */
    EXPECTED_CALL(mocks, mock_malloc(0))
        .SetReturn(allocation);
    block = gballoc_realloc(NULL, 1);
    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
    STRICT_EXPECTED_CALL(mocks, mock_free(TEST_ALLOC_PTR1));
    STRICT_EXPECTED_CALL(mocks, mock_free(allocation));
    STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));

    // act
    gballoc_free(block);

    // assert
    mocks.AssertActualAndExpectedCalls();
    ASSERT_ARE_EQUAL(size_t, 1, gballoc_getMaximumMemoryUsed());

    // cleanup
    free(allocation);
}

/* Tests_SRS_GBALLOC_01_019:[When the ptr pointer cannot be found in the pointers tracked by gballoc, gballoc_free shall not free any memory.] */
TEST_FUNCTION(gballoc_free_with_an_untracked_pointer_does_not_alter_total_memory_used)
{
    // arrange
    CGBAllocMocks mocks;
    gballoc_init();
    void* allocation = malloc(OVERHEAD_SIZE);

    /* don't quite like this, but I'm unsure I want to invest more in this memory counting */
    EXPECTED_CALL(mocks, mock_malloc(0))
        .SetReturn(allocation);

    gballoc_realloc(NULL, 1);
    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
    STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));

    // act
    gballoc_free((void*)0xDEADBEEF);

    // assert
    mocks.AssertActualAndExpectedCalls();
    gballoc_realloc(TEST_ALLOC_PTR1, 2);
    ASSERT_ARE_EQUAL(size_t, 2, gballoc_getMaximumMemoryUsed());

    // cleanup
    gballoc_free(TEST_ALLOC_PTR1);
    free(allocation);
}

/* gballoc_getMaximumMemoryUsed */


/* Tests_SRS_GBALLOC_01_034: [gballoc_getMaximumMemoryUsed shall ensure thread safety by using the lock created by gballoc_Init.] */
TEST_FUNCTION(when_gballoc_getMaximumMemoryUsed_called_It_Shall_Lock_And_Unlock)
{
    // arrange
    CGBAllocMocks mocks;
    gballoc_init();
    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
    STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));
    // act
    (void)gballoc_getMaximumMemoryUsed();

    // assert
    // no explicit assert, uMock checks the calls
}

/* Tests_SRS_GBALLOC_01_050: [If the lock cannot be acquired, gballoc_getMaximumMemoryUsed shall return SIZE_MAX.] */
TEST_FUNCTION(when_acquiring_the_lock_fails_then_gballoc_getMaximumMemoryUsed_fails)
{
    // arrange
    CGBAllocMocks mocks;
    gballoc_init();
    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE))
        .SetReturn(LOCK_ERROR);

    // act
    size_t result = gballoc_getMaximumMemoryUsed();

    // assert
    ASSERT_ARE_EQUAL(size_t, SIZE_MAX, result);
}

/* Tests_SRS_GBALLOC_01_038: [If gballoc was not initialized gballoc_getMaximumMemoryUsed shall return MAX_INT_SIZE.]  */
TEST_FUNCTION(gballoc_getMaximumMemoryUsed_after_deinit_fails)
{
    // arrange
    CGBAllocMocks mocks;

    // act
    size_t result = gballoc_getMaximumMemoryUsed();

    // assert
    ASSERT_ARE_EQUAL(size_t, SIZE_MAX, result);

}

/* gballoc_getCurrentMemoryUsed */

/*Tests_SRS_GBALLOC_02_001: [gballoc_getCurrentMemoryUsed shall return the currently used memory size.] */
TEST_FUNCTION(gballoc_getCurrentMemoryUsed_after_1_byte_malloc_returns_1)
{
    // arrange
    CGBAllocMocks mocks;
    gballoc_init();
    
    void* allocation = malloc(OVERHEAD_SIZE);

    EXPECTED_CALL(mocks, mock_malloc(0))
        .SetReturn(allocation);
    STRICT_EXPECTED_CALL(mocks, mock_malloc(1));
    void *toBeFreed = gballoc_malloc(1);
    mocks.ResetAllCalls();
    STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
    STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));

    // act
    size_t result = gballoc_getCurrentMemoryUsed();

    // assert
    ASSERT_ARE_EQUAL(size_t, 1, result);

    ///cleanup
    gballoc_free(toBeFreed);
    free(allocation);
    mocks.ResetAllCalls(); //this is just for mathematics, not for functionality
}

/*Tests_SRS_GBALLOC_02_001: [gballoc_getCurrentMemoryUsed shall return the currently used memory size.] */
TEST_FUNCTION(gballoc_getCurrentMemoryUsed_after_2x3_byte_calloc_returns_6)
{
    // arrange
    CGBAllocMocks mocks;
    gballoc_init();
    void* allocation = malloc(OVERHEAD_SIZE);

    EXPECTED_CALL(mocks, mock_malloc(0))
        .SetReturn(allocation);
    void *toBeFreed = gballoc_calloc(2, 3);
    mocks.ResetAllCalls();
    STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
    STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));

    // act
    size_t result = gballoc_getCurrentMemoryUsed();

    // assert
    ASSERT_ARE_EQUAL(size_t, 6, result);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    gballoc_free(toBeFreed);
    free(allocation);
    mocks.ResetAllCalls(); //this is just for mathematics, not for functionality
}

/*Tests_SRS_GBALLOC_02_001: [gballoc_getCurrentMemoryUsed shall return the currently used memory size.] */
TEST_FUNCTION(gballoc_getCurrentMemoryUsed_after_1_byte_malloc_and_3_bytes_realloc_returns_3)
{
    // arrange
    CGBAllocMocks mocks;
    gballoc_init();
    void* allocation = malloc(OVERHEAD_SIZE);

    EXPECTED_CALL(mocks, mock_malloc(0))
        .SetReturn(allocation);
    STRICT_EXPECTED_CALL(mocks, mock_malloc(1));

    void *toBeFreed = gballoc_malloc(1);
    toBeFreed = gballoc_realloc(toBeFreed, 3);
    mocks.ResetAllCalls();
    STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
    STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));

    // act
    size_t result = gballoc_getCurrentMemoryUsed();

    // assert
    ASSERT_ARE_EQUAL(size_t, 3, result);

    ///cleanup
    gballoc_free(toBeFreed);
    free(allocation);
    mocks.ResetAllCalls(); //this is just for mathematics, not for functionality
}

/*Tests_SRS_GBALLOC_02_001: [gballoc_getCurrentMemoryUsed shall return the currently used memory size.] */
TEST_FUNCTION(gballoc_getCurrentMemoryUsed_after_1_byte_malloc_and_free_returns_0)
{
    // arrange
    CGBAllocMocks mocks;
    gballoc_init();
    void* allocation = malloc(OVERHEAD_SIZE);

    EXPECTED_CALL(mocks, mock_malloc(0))
        .SetReturn(allocation);
    STRICT_EXPECTED_CALL(mocks, mock_malloc(1));

    void *toBeFreed = gballoc_malloc(1);
    gballoc_free(toBeFreed);
    mocks.ResetAllCalls();
    STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
    STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));

    // act
    size_t result = gballoc_getCurrentMemoryUsed();

    // assert
    ASSERT_ARE_EQUAL(size_t, 0, result);

    ///cleanup
    free(allocation);
    mocks.ResetAllCalls(); //this is just for mathematics, not for functionality
}

/*Tests_SRS_GBALLOC_02_001: [gballoc_getCurrentMemoryUsed shall return the currently used memory size.] */
TEST_FUNCTION(gballoc_getCurrentMemoryUsed_after_2x3_byte_calloc_and_free_returns_0)
{
    // arrange
    CGBAllocMocks mocks;
    gballoc_init();
    void* allocation = malloc(OVERHEAD_SIZE);

    EXPECTED_CALL(mocks, mock_malloc(0))
        .SetReturn(allocation);

    void *toBeFreed = gballoc_calloc(2, 3);
    gballoc_free(toBeFreed);
    mocks.ResetAllCalls();
    STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
    STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));

    // act
    size_t result = gballoc_getCurrentMemoryUsed();

    // assert
    ASSERT_ARE_EQUAL(size_t, 0, result);

    ///cleanup
    free(allocation);
    mocks.ResetAllCalls(); //this is just for mathematics, not for functionality
}

/*Tests_SRS_GBALLOC_02_001: [gballoc_getCurrentMemoryUsed shall return the currently used memory size.] */
TEST_FUNCTION(gballoc_getCurrentMemoryUsed_after_realloc_and_free_returns_0)
{
    // arrange
    CGBAllocMocks mocks;
    gballoc_init();
    void* allocation = malloc(OVERHEAD_SIZE);

    EXPECTED_CALL(mocks, mock_malloc(0))
        .SetReturn(allocation);
    STRICT_EXPECTED_CALL(mocks, mock_malloc(1));

    void *toBeFreed = gballoc_malloc(1);
    toBeFreed = gballoc_realloc(toBeFreed, 3);
    gballoc_free(toBeFreed);
    mocks.ResetAllCalls();
    STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
    STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));

    // act
    size_t result = gballoc_getCurrentMemoryUsed();

    // assert
    ASSERT_ARE_EQUAL(size_t, 0, result);

    ///cleanup
    free(allocation);
    mocks.ResetAllCalls(); //this is just for mathematics, not for functionality
}

/*Tests_SRS_GBALLOC_02_001: [gballoc_getCurrentMemoryUsed shall return the currently used memory size.] */
TEST_FUNCTION(gballoc_getCurrentMemoryUsed_after_1_byte_malloc_and_1_byte_malloc_returns_2)
{
    // arrange
    CGBAllocMocks mocks;
    gballoc_init();

    void* allocation1 = malloc(OVERHEAD_SIZE);
    void* allocation2 = malloc(OVERHEAD_SIZE);

    EXPECTED_CALL(mocks, mock_malloc(0))
        .SetReturn(allocation1);
    STRICT_EXPECTED_CALL(mocks, mock_malloc(1));
    EXPECTED_CALL(mocks, mock_malloc(0))
        .SetReturn(allocation2);
    STRICT_EXPECTED_CALL(mocks, mock_malloc(1));

    void *toBeFreed1 = gballoc_malloc(1);
    void *toBeFreed2 = gballoc_malloc(1);
    mocks.ResetAllCalls();
    STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
    STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));

    // act
    size_t result = gballoc_getCurrentMemoryUsed();

    // assert
    ASSERT_ARE_EQUAL(size_t, 2, result);

    ///cleanup
    gballoc_free(toBeFreed1);
    gballoc_free(toBeFreed2);
    free(allocation1);
    free(allocation2);
    mocks.ResetAllCalls(); //this is just for mathematics, not for functionality
}

/*Tests_SRS_GBALLOC_02_001: [gballoc_getCurrentMemoryUsed shall return the currently used memory size.] */
TEST_FUNCTION(gballoc_getCurrentMemoryUsed_after_1_byte_malloc_and_1_byte_malloc_and_3_realloc_returns_4)
{
    // arrange
    CGBAllocMocks mocks;
    gballoc_init();
    mocks.ResetAllCalls();

    void* allocation1 = malloc(OVERHEAD_SIZE);
    void* allocation2 = malloc(OVERHEAD_SIZE);

    EXPECTED_CALL(mocks, mock_malloc(0))
        .SetReturn(allocation1);
    STRICT_EXPECTED_CALL(mocks, mock_malloc(1));
    EXPECTED_CALL(mocks, mock_malloc(0))
        .SetReturn(allocation2);
    STRICT_EXPECTED_CALL(mocks, mock_malloc(1));

    void *toBeFreed1 = gballoc_malloc(1);
    void *toBeFreed2 = gballoc_malloc(1);
    toBeFreed2 = gballoc_realloc(toBeFreed2, 3);
    mocks.ResetAllCalls();
    STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
    STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));

    // act
    size_t result = gballoc_getCurrentMemoryUsed();

    // assert
    ASSERT_ARE_EQUAL(size_t, 4, result);

    ///cleanup
    gballoc_free(toBeFreed1);
    gballoc_free(toBeFreed2);
    free(allocation1);
    free(allocation2);
    mocks.ResetAllCalls(); //this is just for mathematics, not for functionality
}

/*Tests_SRS_GBALLOC_02_001: [gballoc_getCurrentMemoryUsed shall return the currently used memory size.] */
TEST_FUNCTION(gballoc_getCurrentMemoryUsed_after_1_byte_malloc_and_6_byte_calloc_returns_7)
{
    // arrange
    CGBAllocMocks mocks;
    gballoc_init();

    void* allocation1 = malloc(OVERHEAD_SIZE);
    void* allocation2 = malloc(OVERHEAD_SIZE);

    EXPECTED_CALL(mocks, mock_malloc(0))
        .SetReturn(allocation1);
    STRICT_EXPECTED_CALL(mocks, mock_malloc(1));
    EXPECTED_CALL(mocks, mock_malloc(0))
        .SetReturn(allocation2);
    STRICT_EXPECTED_CALL(mocks, mock_calloc(2, 3));

    void *toBeFreed1 = gballoc_malloc(1);
    void *toBeFreed2 = gballoc_calloc(2, 3);
    mocks.ResetAllCalls();
    STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
    STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));

    // act
    size_t result = gballoc_getCurrentMemoryUsed();

    // assert
    ASSERT_ARE_EQUAL(size_t, 7, result);

    ///cleanup
    gballoc_free(toBeFreed1);
    gballoc_free(toBeFreed2);
    free(allocation1);
    free(allocation2);
    mocks.ResetAllCalls(); //this is just for mathematics, not for functionality
}

/*Tests_SRS_GBALLOC_02_001: [gballoc_getCurrentMemoryUsed shall return the currently used memory size.] */
TEST_FUNCTION(gballoc_getCurrentMemoryUsed_after_1_byte_malloc_and_6_byte_calloc_and_free_returns_6)
{
    // arrange
    CGBAllocMocks mocks;
    gballoc_init();
    void* allocation1 = malloc(OVERHEAD_SIZE);
    void* allocation2 = malloc(OVERHEAD_SIZE);

    EXPECTED_CALL(mocks, mock_malloc(0))
        .SetReturn(allocation1);
    STRICT_EXPECTED_CALL(mocks, mock_malloc(1))
        .SetReturn((char*)allocation1 + OVERHEAD_SIZE / 2); /*somewhere in the middle of allocation*/
    EXPECTED_CALL(mocks, mock_malloc(0))
        .SetReturn(allocation2);
    STRICT_EXPECTED_CALL(mocks, mock_calloc(2, 3))
        .SetReturn((char*)allocation2 + OVERHEAD_SIZE / 2);

    void *toBeFreed1 = gballoc_malloc(1);
    void *toBeFreed2 = gballoc_calloc(2, 3);
    gballoc_free(toBeFreed1);
    mocks.ResetAllCalls();
    STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
    STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));

    // act
    size_t result = gballoc_getCurrentMemoryUsed();

    // assert
    ASSERT_ARE_EQUAL(size_t, 6, result);

    ///cleanup
    gballoc_free(toBeFreed2);
    free(allocation1);
    free(allocation2);
    mocks.ResetAllCalls(); //this is just for mathematics, not for functionality
}

/*Tests_SRS_GBALLOC_02_001: [gballoc_getCurrentMemoryUsed shall return the currently used memory size.] */
TEST_FUNCTION(gballoc_getCurrentMemoryUsed_after_1_byte_malloc_and_1_byte_malloc_and_3_realloc_and_free_returns_3)
{
    // arrange
    CGBAllocMocks mocks;
    gballoc_init();
    void* allocation1 = malloc(OVERHEAD_SIZE);
    void* allocation2 = malloc(OVERHEAD_SIZE);

    EXPECTED_CALL(mocks, mock_malloc(0))
        .SetReturn(allocation1);
    STRICT_EXPECTED_CALL(mocks, mock_malloc(1))
        .SetReturn((char*)allocation1 + OVERHEAD_SIZE / 2); /*somewhere in the middle of allocation*/
    EXPECTED_CALL(mocks, mock_malloc(0))
        .SetReturn(allocation2);
    STRICT_EXPECTED_CALL(mocks, mock_realloc(IGNORED_PTR_ARG, 3))
        .IgnoreArgument(1)
        .SetReturn((char*)allocation2 + OVERHEAD_SIZE / 2); /*somewhere in the middle of allocation*/

    void *toBeFreed1 = gballoc_malloc(1);
    void *toBeFreed2 = gballoc_malloc(1);
    toBeFreed2 = gballoc_realloc(toBeFreed2, 3);
    gballoc_free(toBeFreed1);
    mocks.ResetAllCalls();
    STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
    STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));

    // act
    size_t result = gballoc_getCurrentMemoryUsed();

    // assert
    ASSERT_ARE_EQUAL(size_t, 3, result);

    ///cleanup
    gballoc_free(toBeFreed2);
    free(allocation1);
    free(allocation2);
    mocks.ResetAllCalls(); //this is just for mathematics, not for functionality
}

/* Tests_SRS_GBALLOC_01_036: [gballoc_getCurrentMemoryUsed shall ensure thread safety by using the lock created by gballoc_Init.] */
TEST_FUNCTION(gballoc_getCurrentMemoryUsed_locks_and_unlocks)
{
    // assert
    CGBAllocMocks mocks;
    gballoc_init();
    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
    STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));

    // act
    size_t result = gballoc_getCurrentMemoryUsed();

    // assert
    ASSERT_ARE_EQUAL(size_t, 0, result);
}

/* Tests_SRS_GBALLOC_01_044: [If gballoc was not initialized gballoc_getCurrentMemoryUsed shall return SIZE_MAX.] */
TEST_FUNCTION(gballoc_getCurrentMemoryUsed_after_deinit_fails)
{
    // arrange
    CGBAllocMocks mocks;

    // act
    size_t result = gballoc_getCurrentMemoryUsed();

    // assert
    ASSERT_ARE_EQUAL(size_t, SIZE_MAX, result);
}

/* Tests_SRS_GBALLOC_01_051: [If the lock cannot be acquired, gballoc_getCurrentMemoryUsed shall return SIZE_MAX.] */
TEST_FUNCTION(when_acquiring_the_lock_fails_gballoc_getCurrentMemoryUsed_fails)
{
    // arrange
    CGBAllocMocks mocks;
    gballoc_init();
    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE))
        .SetReturn(LOCK_ERROR);

    // act
    size_t result = gballoc_getCurrentMemoryUsed();

    // assert
    ASSERT_ARE_EQUAL(size_t, SIZE_MAX, result);

}

END_TEST_SUITE(GBAlloc_UnitTests)
