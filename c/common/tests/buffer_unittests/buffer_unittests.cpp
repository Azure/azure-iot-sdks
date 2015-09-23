// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

//
// PUT NO INCLUDES BEFORE HERE !!!!
//
#include <cstdlib>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include <stddef.h>

//
// PUT NO CLIENT LIBRARY INCLUDES BEFORE HERE !!!!
//
#include "testrunnerswitcher.h"
#include "buffer_.h"
#include "micromock.h"
#include "lock.h"

#ifdef _MSC_VER
#pragma warning(disable:4505)
#endif

#define ALLOCATION_SIZE             16
#define TOTAL_ALLOCATION_SIZE       32

unsigned char BUFFER_TEST_VALUE[] = {0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x10,0x11,0x12,0x13,0x14,0x15,0x16};
unsigned char ADDITIONAL_BUFFER[] = {0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,0x20,0x21,0x22,0x23,0x24,0x25,0x26};
unsigned char TOTAL_BUFFER[] = {0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,0x20,0x21,0x22,0x23,0x24,0x25,0x26};

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

static size_t currentmalloc_call = 0;
static size_t whenShallmalloc_fail = 0;

static size_t currentrealloc_call = 0;
static size_t whenShallrealloc_fail = 0;

TYPED_MOCK_CLASS(CMocks, CGlobalMock)
{
public:

    MOCK_STATIC_METHOD_1(, void*, gballoc_malloc, size_t, size)
        void* result2;
    currentmalloc_call++;
    if (whenShallmalloc_fail > 0)
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
    if (whenShallrealloc_fail > 0)
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

DECLARE_GLOBAL_MOCK_METHOD_1(CMocks, , void*, gballoc_malloc, size_t, size);
DECLARE_GLOBAL_MOCK_METHOD_2(CMocks, , void*, gballoc_realloc, void*, ptr, size_t, size);
DECLARE_GLOBAL_MOCK_METHOD_1(CMocks, , void, gballoc_free, void*, ptr);

BEGIN_TEST_SUITE(Buffer_UnitTests)

    TEST_SUITE_INITIALIZE(setsBufferTempSize)
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

    TEST_FUNCTION_INITIALIZE(f)
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

    TEST_FUNCTION_CLEANUP(cleans)
    {
        if (!MicroMockReleaseMutex(g_testByTest))
        {
            ASSERT_FAIL("failure in test framework at ReleaseMutex");
        }
    }

    /* Tests_SRS_BUFFER_07_001: [BUFFER_new shall allocate a BUFFER_HANDLE that will contain a NULL unsigned char*.] */
    TEST_FUNCTION(BUFFER_new_Succeed)
    {
        ///arrange
        CMocks mocks;

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);
        

        ///act
        BUFFER_HANDLE g_hBuffer = BUFFER_new();

        ///assert
        ASSERT_IS_NOT_NULL(g_hBuffer);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        BUFFER_delete(g_hBuffer);
    }

    /* BUFFER_delete Tests BEGIN */
    /* Tests_SRS_BUFFER_07_003: [BUFFER_delete shall delete the data associated with the BUFFER_HANDLE.] */
    TEST_FUNCTION(BUFFER_delete_Succeed)
    {
        ///arrange
        CMocks mocks;
        BUFFER_HANDLE g_hBuffer;
        g_hBuffer = BUFFER_new();
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        ///act
        BUFFER_delete(g_hBuffer);

        ///assert
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        //none
    }

    /* Tests_SRS_BUFFER_07_003: [BUFFER_delete shall delete the data associated with the BUFFER_HANDLE.] */
    TEST_FUNCTION(BUFFER_delete_Alloc_Succeed)
    {
        ///arrange
        CMocks mocks;
        BUFFER_HANDLE g_hBuffer;
        g_hBuffer = BUFFER_new();
        BUFFER_build(g_hBuffer, BUFFER_TEST_VALUE, ALLOCATION_SIZE);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        ///act
        BUFFER_delete(g_hBuffer);

        ///assert
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        //none
    }

    /* Tests_SRS_BUFFER_07_004: [BUFFER_delete shall not delete any BUFFER_HANDLE that is NULL.] */
    TEST_FUNCTION(BUFFER_delete_NULL_HANDLE_Succeed)
    {
        ///arrange

        ///act
        BUFFER_delete(NULL);

        ///assert
    }

    /* BUFFER_pre_Build Tests BEGIN */
    /* Tests_SRS_BUFFER_07_005: [BUFFER_pre_build allocates size_t bytes of BUFFER_HANDLE and returns zero on success.] */
    TEST_FUNCTION(BUFFER_pre_build_Succeed)
    {
        ///arrange
        CMocks mocks;
        BUFFER_HANDLE g_hBuffer;
        g_hBuffer = BUFFER_new();
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(ALLOCATION_SIZE));

        ///act
        int nResult = BUFFER_pre_build(g_hBuffer, ALLOCATION_SIZE);

        ///assert
        ASSERT_ARE_EQUAL(int, nResult, 0);
        ASSERT_ARE_EQUAL(int, BUFFER_length(g_hBuffer), ALLOCATION_SIZE);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        BUFFER_delete(g_hBuffer);
    }

    /* Tests_SRS_BUFFER_07_006: [If handle is NULL or size is 0 then BUFFER_pre_build shall return a nonzero value.] */
    /* Tests_SRS_BUFFER_07_013: [BUFFER_pre_build shall return nonzero if any error is encountered.] */
    TEST_FUNCTION(BUFFER_pre_build_HANDLE_NULL_Fail)
    {
        ///arrange
        CMocks mocks;

        ///act
        int nResult = BUFFER_pre_build(NULL, ALLOCATION_SIZE);

        ///assert
        ASSERT_ARE_NOT_EQUAL(int, nResult, 0);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        //none
    }

    /* Tests_SRS_BUFFER_07_006: [If handle is NULL or size is 0 then BUFFER_pre_build shall return a nonzero value.] */
    TEST_FUNCTION(BUFFER_pre_Size_Zero_Fail)
    {
        ///arrange
        CMocks mocks;
        BUFFER_HANDLE g_hBuffer;
        g_hBuffer = BUFFER_new();
        mocks.ResetAllCalls();

        ///act
        int nResult = BUFFER_pre_build(g_hBuffer, 0);

        ///assert
        ASSERT_ARE_NOT_EQUAL(int, nResult, 0);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        BUFFER_delete(g_hBuffer);
    }

    /* Tests_SRS_BUFFER_07_013: [BUFFER_pre_build shall return nonzero if any error is encountered.] */
    TEST_FUNCTION(BUFFER_pre_build_HANDLE_NULL_Size_Zero_Fail)
    {
        ///arrange
        CMocks mocks;

        ///act
        int nResult = BUFFER_pre_build(NULL, 0);

        ///assert
        ASSERT_ARE_NOT_EQUAL(int, nResult, 0);
    }

    /* Tests_SRS_BUFFER_07_007: [BUFFER_pre_build shall return nonzero if the buffer has been previously allocated and is not NULL.] */
    /* Tests_SRS_BUFFER_07_013: [BUFFER_pre_build shall return nonzero if any error is encountered.] */
    TEST_FUNCTION(BUFFER_pre_build_Multiple_Alloc_Fail)
    {
        ///arrange
        CMocks mocks;
        BUFFER_HANDLE g_hBuffer;
        g_hBuffer = BUFFER_new();
        int nResult = BUFFER_pre_build(g_hBuffer, ALLOCATION_SIZE);
        mocks.ResetAllCalls();

        ///act
        nResult = BUFFER_pre_build(g_hBuffer, ALLOCATION_SIZE);

        ///assert
        ASSERT_ARE_NOT_EQUAL(int, nResult, 0);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        BUFFER_delete(g_hBuffer);
    }

    /* Tests_SRS_BUFFER_07_008: [BUFFER_build allocates size_t bytes, copies the unsigned char* into the buffer and returns zero on success.] */
    TEST_FUNCTION(BUFFER_build_Succeed)
    {
        ///arrange
        CMocks mocks;
        BUFFER_HANDLE g_hBuffer;

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, gballoc_realloc(IGNORED_PTR_ARG, ALLOCATION_SIZE))
            .IgnoreArgument(1);

        ///act
        g_hBuffer = BUFFER_new();

        int nResult = BUFFER_build(g_hBuffer, BUFFER_TEST_VALUE, ALLOCATION_SIZE);

        ///assert
        ASSERT_ARE_EQUAL(int, BUFFER_length(g_hBuffer), ALLOCATION_SIZE);
        ASSERT_ARE_EQUAL(int, 0, memcmp(BUFFER_u_char(g_hBuffer), BUFFER_TEST_VALUE, ALLOCATION_SIZE));
        ASSERT_ARE_EQUAL(int, nResult, 0);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        BUFFER_delete(g_hBuffer);
    }

    /* Tests_SRS_BUFFER_07_009: [BUFFER_build shall return nonzero if handle is NULL ] */
    TEST_FUNCTION(BUFFER_build_NULL_HANDLE_Fail)
    {
        ///arrange
        CMocks mocks;

        ///act
        int nResult = BUFFER_build(NULL, BUFFER_TEST_VALUE, ALLOCATION_SIZE);

        ///assert
        ASSERT_ARE_NOT_EQUAL(int, nResult, 0);
    }

    /* Tests_SRS_BUFFER_01_001: [If size is positive and source is NULL, BUFFER_build shall return nonzero] */
    TEST_FUNCTION(BUFFER_build_Content_NULL_Fail)
    {
        ///arrange
        CMocks mocks;
        BUFFER_HANDLE g_hBuffer;
        g_hBuffer = BUFFER_new();
        mocks.ResetAllCalls();

        ///act
        int nResult = BUFFER_build(g_hBuffer, NULL, ALLOCATION_SIZE);

        ///assert
        ASSERT_ARE_NOT_EQUAL(int, nResult, 0);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        BUFFER_delete(g_hBuffer);
    }

    /* Tests_SRS_BUFFER_01_002: [The size argument can be zero, in which case the underlying buffer held by the buffer instance shall be freed.] */
    TEST_FUNCTION(BUFFER_build_Size_Zero_non_NULL_buffer_Succeeds)
    {
        ///arrange
        CMocks mocks;
        BUFFER_HANDLE g_hBuffer;
        g_hBuffer = BUFFER_new();
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        ///act
        int nResult = BUFFER_build(g_hBuffer, BUFFER_TEST_VALUE, 0);

        ///assert
        ASSERT_ARE_EQUAL(int, nResult, 0);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        BUFFER_delete(g_hBuffer);
    }

	/* Tests_SRS_BUFFER_01_002: [The size argument can be zero, in which case the underlying buffer held by the buffer instance shall be freed.] */
	TEST_FUNCTION(BUFFER_build_Size_Zero_NULL_buffer_Succeeds)
	{
        ///arrange
        CMocks mocks;
        BUFFER_HANDLE g_hBuffer;
        g_hBuffer = BUFFER_new();
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

		///act
        int nResult = BUFFER_build(g_hBuffer, NULL, 0);

		///assert
		ASSERT_ARE_EQUAL(int, nResult, 0);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        BUFFER_delete(g_hBuffer);
	}

    /* Tests_SRS_BUFFER_07_011: [BUFFER_build shall overwrite previous contents if the buffer has been previously allocated.] */
    TEST_FUNCTION(BUFFER_build_when_the_buffer_is_already_allocated_and_the_same_amount_of_bytes_is_needed_succeeds)
    {
        ///arrange
        CMocks mocks;
        BUFFER_HANDLE g_hBuffer;
        g_hBuffer = BUFFER_new();
        int nResult = BUFFER_build(g_hBuffer, BUFFER_TEST_VALUE, ALLOCATION_SIZE);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, gballoc_realloc(IGNORED_PTR_ARG, ALLOCATION_SIZE))
            .IgnoreArgument(1);

        ///act
        nResult = BUFFER_build(g_hBuffer, BUFFER_TEST_VALUE, ALLOCATION_SIZE);

        ///assert
        ASSERT_ARE_EQUAL(int, nResult, 0);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        BUFFER_delete(g_hBuffer);
    }

	/* Tests_SRS_BUFFER_07_011: [BUFFER_build shall overwrite previous contents if the buffer has been previously allocated.] */
	TEST_FUNCTION(BUFFER_build_when_the_buffer_is_already_allocated_and_more_bytes_are_needed_succeeds)
	{
        ///arrange
        CMocks mocks;
        BUFFER_HANDLE g_hBuffer;
        g_hBuffer = BUFFER_new();
        int nResult = BUFFER_build(g_hBuffer, BUFFER_TEST_VALUE, ALLOCATION_SIZE - 1);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, gballoc_realloc(IGNORED_PTR_ARG, ALLOCATION_SIZE))
            .IgnoreArgument(1);

		///act
		nResult = BUFFER_build(g_hBuffer, BUFFER_TEST_VALUE, ALLOCATION_SIZE);

		///assert
		ASSERT_ARE_EQUAL(int, nResult, 0);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        BUFFER_delete(g_hBuffer);
	}

	/* Tests_SRS_BUFFER_07_011: [BUFFER_build shall overwrite previous contents if the buffer has been previously allocated.] */
	TEST_FUNCTION(BUFFER_build_when_the_buffer_is_already_allocated_and_less_bytes_are_needed_succeeds)
	{
        ///arrange
        CMocks mocks;
        BUFFER_HANDLE g_hBuffer;
        g_hBuffer = BUFFER_new();
        int nResult = BUFFER_build(g_hBuffer, BUFFER_TEST_VALUE, ALLOCATION_SIZE);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, gballoc_realloc(IGNORED_PTR_ARG, ALLOCATION_SIZE - 1))
            .IgnoreArgument(1);

		///act
		nResult = BUFFER_build(g_hBuffer, BUFFER_TEST_VALUE, ALLOCATION_SIZE - 1);

		///assert
		ASSERT_ARE_EQUAL(int, nResult, 0);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        BUFFER_delete(g_hBuffer);
	}

    /* BUFFER_unbuild Tests BEGIN */
    /* Tests_SRS_BUFFER_07_012: [BUFFER_unbuild shall clear the underlying unsigned char* data associated with the BUFFER_HANDLE this will return zero on success.] */
    TEST_FUNCTION(BUFFER_unbuild_Succeed)
    {
        ///arrange
        CMocks mocks;
        BUFFER_HANDLE g_hBuffer;
        g_hBuffer = BUFFER_new();
        int nResult = BUFFER_build(g_hBuffer, BUFFER_TEST_VALUE, ALLOCATION_SIZE);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        ///act
        nResult = BUFFER_unbuild(g_hBuffer);

        ///assert
        ASSERT_ARE_EQUAL(int, nResult, 0);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        BUFFER_delete(g_hBuffer);
    }

    /* Tests_SRS_BUFFER_07_014: [BUFFER_unbuild shall return a nonzero value if BUFFER_HANDLE is NULL.] */
    TEST_FUNCTION(BUFFER_unbuild_HANDLE_NULL_Fail)
    {
        ///arrange
        CMocks mocks;

        ///act
        int nResult = BUFFER_unbuild(NULL);

        ///assert
        ASSERT_ARE_NOT_EQUAL(int, nResult, 0);
    }

    /* Tests_SRS_BUFFER_07_015: [BUFFER_unbuild shall return a nonzero value if the unsigned char* referenced by BUFFER_HANDLE is NULL.] */
    TEST_FUNCTION(BUFFER_unbuild_Multiple_Alloc_Fail)
    {
        ///arrange
        CMocks mocks;
        BUFFER_HANDLE g_hBuffer;
        g_hBuffer = BUFFER_new();
        int nResult = BUFFER_build(g_hBuffer, BUFFER_TEST_VALUE, ALLOCATION_SIZE);
        nResult = BUFFER_unbuild(g_hBuffer);
        mocks.ResetAllCalls();

        ///act
        nResult = BUFFER_unbuild(g_hBuffer);

        ///assert
        ASSERT_ARE_NOT_EQUAL(int, nResult, 0);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        BUFFER_delete(g_hBuffer);
    }

    /* BUFFER_enlarge Tests BEGIN */
    /* Tests_SRS_BUFFER_07_016: [BUFFER_enlarge shall increase the size of the unsigned char* referenced by BUFFER_HANDLE.] */
    TEST_FUNCTION(BUFFER_enlarge_Succeed)
    {
        ///arrange
        CMocks mocks;
        BUFFER_HANDLE g_hBuffer;
        g_hBuffer = BUFFER_new();
        int nResult = BUFFER_build(g_hBuffer, BUFFER_TEST_VALUE, ALLOCATION_SIZE);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, gballoc_realloc(IGNORED_PTR_ARG, 2 * ALLOCATION_SIZE))
            .IgnoreArgument(1);

        ///act
        nResult = BUFFER_enlarge(g_hBuffer, ALLOCATION_SIZE);

        ///assert
        ASSERT_ARE_EQUAL(int, nResult, 0);
        ASSERT_ARE_EQUAL(int, TOTAL_ALLOCATION_SIZE, BUFFER_length(g_hBuffer) );
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        BUFFER_delete(g_hBuffer);
    }

    /* Tests_SRS_BUFFER_07_017: [BUFFER_enlarge shall return a nonzero result if any parameters are NULL or zero.] */
    /* Tests_SRS_BUFFER_07_018: [BUFFER_enlarge shall return a nonzero result if any error is encountered.] */
    TEST_FUNCTION(BUFFER_enlarge_NULL_HANDLE_Fail)
    {
        ///arrange
        CMocks mocks;

        ///act
        int nResult = BUFFER_enlarge(NULL, ALLOCATION_SIZE);

        ///assert
        ASSERT_ARE_NOT_EQUAL(int, nResult, 0);
    }

    /* Tests_SRS_BUFFER_07_017: [BUFFER_enlarge shall return a nonzero result if any parameters are NULL or zero.] */
    /* Tests_SRS_BUFFER_07_018: [BUFFER_enlarge shall return a nonzero result if any error is encountered.] */
    TEST_FUNCTION(BUFFER_enlarge_Size_Zero_Fail)
    {
        ///arrange
        CMocks mocks;
        BUFFER_HANDLE g_hBuffer;
        g_hBuffer = BUFFER_new();
        int nResult = BUFFER_build(g_hBuffer, BUFFER_TEST_VALUE, ALLOCATION_SIZE);
        mocks.ResetAllCalls();

        ///act
        nResult = BUFFER_enlarge(g_hBuffer, 0);

        ///assert
        ASSERT_ARE_NOT_EQUAL(int, nResult, 0);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        BUFFER_delete(g_hBuffer);
    }

    /* BUFFER_content Tests BEGIN */
    /* Tests_SRS_BUFFER_07_019: [BUFFER_content shall return the data contained within the BUFFER_HANDLE.] */
    TEST_FUNCTION(BUFFER_content_Succeed)
    {
        ///arrange
        CMocks mocks;
        BUFFER_HANDLE g_hBuffer;
        g_hBuffer = BUFFER_new();
        int nResult = BUFFER_build(g_hBuffer, BUFFER_TEST_VALUE, ALLOCATION_SIZE);
        mocks.ResetAllCalls();

        ///act
        const unsigned char* content = NULL;
        nResult = BUFFER_content(g_hBuffer, &content);

        ///assert
        ASSERT_ARE_EQUAL(int, nResult, 0);
        ASSERT_ARE_EQUAL(int, 0, memcmp(content, BUFFER_TEST_VALUE, ALLOCATION_SIZE));
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        BUFFER_delete(g_hBuffer);
    }

    /* Tests_SRS_BUFFER_07_020: [If the handle and/or content*is NULL BUFFER_content shall return nonzero.] */
    TEST_FUNCTION(BUFFER_content_HANDLE_NULL_Fail)
    {
        ///arrange
        CMocks mocks;

        ///act
        const unsigned char* content = NULL;
        int nResult = BUFFER_content(NULL, &content);

        ///assert
        ASSERT_ARE_NOT_EQUAL(int, nResult, 0);
        ASSERT_IS_NULL(content);

        ///cleanup

    }

    /* Tests_SRS_BUFFER_07_020: [If the handle and/or content*is NULL BUFFER_content shall return nonzero.] */
    TEST_FUNCTION(BUFFER_content_Char_NULL_Fail)
    {
        ///arrange
        CMocks mocks;
        BUFFER_HANDLE g_hBuffer;
        g_hBuffer = BUFFER_new();
        int nResult = BUFFER_build(g_hBuffer, BUFFER_TEST_VALUE, ALLOCATION_SIZE);
        mocks.ResetAllCalls();

        ///act
        nResult = BUFFER_content(g_hBuffer, NULL);

        ///assert
        ASSERT_ARE_NOT_EQUAL(int, nResult, 0);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        BUFFER_delete(g_hBuffer);
    }

    /* BUFFER_size Tests BEGIN */
    /* Tests_SRS_BUFFER_07_021: [BUFFER_size shall place the size of the associated buffer in the size variable and return zero on success.] */
    TEST_FUNCTION(BUFFER_size_Succeed)
    {
        ///arrange
        CMocks mocks;
        BUFFER_HANDLE g_hBuffer;
        g_hBuffer = BUFFER_new();
        int nResult = BUFFER_build(g_hBuffer, BUFFER_TEST_VALUE, ALLOCATION_SIZE);
        mocks.ResetAllCalls();

        ///act
        size_t size = 0;
        nResult = BUFFER_size(g_hBuffer, &size);

        ///assert
        ASSERT_ARE_EQUAL(int, nResult, 0);
        ASSERT_ARE_EQUAL(int, size, ALLOCATION_SIZE);
        mocks.AssertActualAndExpectedCalls();


        ///cleanup
        BUFFER_delete(g_hBuffer);
    }

    /* Tests_SRS_BUFFER_07_022: [BUFFER_size shall return a nonzero value for any error that is encountered.] */
    TEST_FUNCTION(BUFFER_size_HANDLE_NULL_Fail)
    {
        ///arrange
        CMocks mocks;

        ///act
        size_t size = 0;
        int nResult = BUFFER_size(NULL, &size);

        ///assert
        ASSERT_ARE_NOT_EQUAL(int, nResult, 0);
    }

    /* Tests_SRS_BUFFER_07_022: [BUFFER_size shall return a nonzero value for any error that is encountered.] */
    TEST_FUNCTION(BUFFER_size_Size_t_NULL_Fail)
    {
        ///arrange
        CMocks mocks;
        BUFFER_HANDLE g_hBuffer;
        g_hBuffer = BUFFER_new();
        int nResult = BUFFER_build(g_hBuffer, BUFFER_TEST_VALUE, ALLOCATION_SIZE);
        mocks.ResetAllCalls();

        ///act
        nResult = BUFFER_size(g_hBuffer, NULL);

        ///assert
        ASSERT_ARE_NOT_EQUAL(int, nResult, 0);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        BUFFER_delete(g_hBuffer);
    }

    /* BUFFER_append Tests BEGIN */
    /* Tests_SRS_BUFFER_07_024: [BUFFER_append concatenates b2 onto b1 without modifying b2 and shall return zero on success.] */
    TEST_FUNCTION(BUFFER_append_Succeed)
    {
        ///arrange
        CMocks mocks;
        BUFFER_HANDLE g_hBuffer;
        g_hBuffer = BUFFER_new();
        int nResult = BUFFER_build(g_hBuffer, BUFFER_TEST_VALUE, ALLOCATION_SIZE);
        BUFFER_HANDLE hAppend = BUFFER_new();
        nResult = BUFFER_build(hAppend, ADDITIONAL_BUFFER, ALLOCATION_SIZE);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, gballoc_realloc(IGNORED_PTR_ARG, ALLOCATION_SIZE + ALLOCATION_SIZE))
            .IgnoreArgument(1);

        ///act
        nResult = BUFFER_append(g_hBuffer, hAppend);

        ///assert
        ASSERT_ARE_EQUAL(int, nResult, 0);
        ASSERT_ARE_EQUAL(int, 0, memcmp(BUFFER_u_char(g_hBuffer), TOTAL_BUFFER, TOTAL_ALLOCATION_SIZE));
        ASSERT_ARE_EQUAL(int, 0, memcmp(BUFFER_u_char(hAppend), ADDITIONAL_BUFFER, ALLOCATION_SIZE));
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        BUFFER_delete(hAppend);
        BUFFER_delete(g_hBuffer);
    }

    /* Tests_SRS_BUFFER_07_023: [BUFFER_append shall return a nonzero upon any error that is encountered.] */
    TEST_FUNCTION(BUFFER_append_HANDLE_NULL_Fail)
    {
        ///arrange
        CMocks mocks;
        BUFFER_HANDLE hAppend = BUFFER_new();
        int nResult = BUFFER_build(hAppend, ADDITIONAL_BUFFER, ALLOCATION_SIZE);
        mocks.ResetAllCalls();

        ///act
        nResult = BUFFER_append(NULL, hAppend);

        ///assert
        ASSERT_ARE_NOT_EQUAL(int, nResult, 0);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        BUFFER_delete(hAppend);
    }

    /* Tests_SRS_BUFFER_07_023: [BUFFER_append shall return a nonzero upon any error that is encountered.] */
    TEST_FUNCTION(BUFFER_append_APPEND_HANDLE_NULL_Fail)
    {
        ///arrange
        CMocks mocks;
        BUFFER_HANDLE g_hBuffer;
        g_hBuffer = BUFFER_new();
        int nResult = BUFFER_build(g_hBuffer, BUFFER_TEST_VALUE, ALLOCATION_SIZE);
        mocks.ResetAllCalls();

        ///act
        nResult = BUFFER_append(g_hBuffer, NULL);

        ///assert
        ASSERT_ARE_NOT_EQUAL(int, nResult, 0);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        BUFFER_delete(g_hBuffer);
    }

    /* BUFFER_u_char Tests BEGIN */
    /* Tests_SRS_BUFFER_07_025: [BUFFER_u_char shall return a pointer to the underlying unsigned char*.] */
    TEST_FUNCTION(BUFFER_U_CHAR_Succeed)
    {
        ///arrange
        CMocks mocks;
        BUFFER_HANDLE g_hBuffer;
        g_hBuffer = BUFFER_new();
        (void)BUFFER_build(g_hBuffer, BUFFER_TEST_VALUE, ALLOCATION_SIZE);
        mocks.ResetAllCalls();
        
        ///act
        unsigned char* u = BUFFER_u_char(g_hBuffer);

        ///assert
        ASSERT_ARE_EQUAL(int, 0, memcmp(u, BUFFER_TEST_VALUE, ALLOCATION_SIZE) );
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        BUFFER_delete(g_hBuffer);
    }

    /* Tests_SRS_BUFFER_07_026: [BUFFER_u_char shall return NULL for any error that is encountered.] */
    TEST_FUNCTION(BUFFER_U_CHAR_HANDLE_NULL_Fail)
    {
        ///arrange
        CMocks mocks;

        ///act
        ASSERT_IS_NULL(BUFFER_u_char(NULL) );
    }

    /* BUFFER_length Tests BEGIN */
    /* Tests_SRS_BUFFER_07_027: [BUFFER_length shall return the size of the underlying buffer.] */
    TEST_FUNCTION(BUFFER_length_Succeed)
    {
        ///arrange
        CMocks mocks;
        BUFFER_HANDLE g_hBuffer;
        g_hBuffer = BUFFER_new();
        (void)BUFFER_build(g_hBuffer, BUFFER_TEST_VALUE, ALLOCATION_SIZE);
        mocks.ResetAllCalls();

        ///act
        size_t l = BUFFER_length(g_hBuffer);

        ///assert
        ASSERT_ARE_EQUAL(size_t, l, ALLOCATION_SIZE);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        BUFFER_delete(g_hBuffer);
    }

    /* Tests_SRS_BUFFER_07_028: [BUFFER_length shall return zero for any error that is encountered.] */
    TEST_FUNCTION(BUFFER_length_HANDLE_NULL_Succeed)
    {
        ///arrange
        CMocks mocks;

        ///act

        ///assert
        ASSERT_ARE_EQUAL(int, BUFFER_length(NULL), 0);
    }

    TEST_FUNCTION(BUFFER_Clone_Succeed)
    {
        ///arrange
        CMocks mocks;
        BUFFER_HANDLE g_hBuffer;
        g_hBuffer = BUFFER_new();
        (void)BUFFER_build(g_hBuffer, BUFFER_TEST_VALUE, ALLOCATION_SIZE);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(ALLOCATION_SIZE))
            .IgnoreArgument(1);

        ///act
        BUFFER_HANDLE hclone = BUFFER_clone(g_hBuffer);

        ///assert
        ASSERT_ARE_EQUAL(int, 0, memcmp(BUFFER_u_char(hclone), BUFFER_TEST_VALUE, ALLOCATION_SIZE) );
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        BUFFER_delete(g_hBuffer);
        BUFFER_delete(hclone);
    }

    TEST_FUNCTION(BUFFER_Clone_HANDLE_NULL_Fail)
    {
        ///arrange
        CMocks mocks;

        ///act

        ///assert
        ASSERT_IS_NULL(BUFFER_clone(NULL) );
    }

    /*Tests_SRS_BUFFER_02_001: [If source is NULL then BUFFER_create shall return NULL.] */
    TEST_FUNCTION(BUFFER_create_with_NULL_source_fails)
    {
        ///arrange
        CMocks mocks;

        ///act
        auto res = BUFFER_create(NULL, 0);

        ///assert
        ASSERT_IS_NULL(res);

        ///cleanup
    }

    /*Tests_SRS_BUFFER_02_002: [Otherwise, BUFFER_create shall allocate memory to hold size bytes and shall copy from source size bytes into the newly allocated memory.] */
    /*Tests_SRS_BUFFER_02_004: [Otherwise, BUFFER_create shall return a non-NULL handle*/
    TEST_FUNCTION(BUFFER_create_happy_path)
    {
        ///arrange
        CMocks mocks;
        char c = '3';

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(1));

        ///act
        auto res = BUFFER_create((const unsigned char*)&c, 1);

        ///assert
        ASSERT_IS_NOT_NULL(res);
        size_t howBig = BUFFER_length(res);
        ASSERT_ARE_EQUAL(size_t, 1, howBig);
        const unsigned char* data = BUFFER_u_char(res);
        ASSERT_ARE_EQUAL(uint8_t, '3', data[0]);

        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        BUFFER_delete(res);
    }

    /*Tests_SRS_BUFFER_02_003: [If allocating memory fails, then BUFFER_create shall return NULL.] */
    TEST_FUNCTION(BUFFER_create_fails_when_gballoc_fails_1)
    {
        ///arrange
        CMocks mocks;
        char c = '3';

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        whenShallmalloc_fail = 2;
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(1));

        ///act
        auto res = BUFFER_create((const unsigned char*)&c, 1);

        ///assert
        ASSERT_IS_NULL(res);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        BUFFER_delete(res);
    }

    /*Tests_SRS_BUFFER_02_003: [If allocating memory fails, then BUFFER_create shall return NULL.] */
    TEST_FUNCTION(BUFFER_create_fails_when_gballoc_fails_2)
    {
        ///arrange
        CMocks mocks;
        char c = '3';

        whenShallmalloc_fail = 1;
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);

        ///act
        auto res = BUFFER_create((const unsigned char*)&c, 1);

        ///assert
        ASSERT_IS_NULL(res);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        BUFFER_delete(res);
    }

END_TEST_SUITE(Buffer_UnitTests)