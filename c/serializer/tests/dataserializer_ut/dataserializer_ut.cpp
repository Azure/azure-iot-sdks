// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <cstdlib>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include <stdexcept>
#include "testrunnerswitcher.h"
#include "micromock.h"
#include "micromockenumtostring.h"
#include "micromockcharstararenullterminatedstrings.h"
#include "multitree.h"
#include "azure_c_shared_utility/macro_utils.h"

/*this is what we test*/
#include "dataserializer.h"


#define TEST_MULTITREE_HANDLE               ((MULTITREE_HANDLE)0xDEADBEEF)
#define TEST_BUFFER_VALUE                   ((BUFFER_HANDLE)0xDEADBEEF)

//
// We do this namespace redirection so that we don't have to repeat the utility string and buffer code in the mocks!
//
namespace BASEIMPLEMENTATION
{
#include "strings.c"
#include "buffer.c"
};

static size_t nAllocCalls=0;

TYPED_MOCK_CLASS(CDataSerializeMocks, CGlobalMock)
{
public:
    MOCK_STATIC_METHOD_2(, BUFFER_HANDLE, TestDataSerializerEncode, MULTITREE_HANDLE, multiTreeHandle, DATA_SERIALIZER_MULTITREE_TYPE, dataType)
    {
        if (multiTreeHandle == NULL)
        {
            throw std::runtime_error("this is expected to be called only with non-NULL arguments.");
        }
    }
    MOCK_METHOD_END(BUFFER_HANDLE, TEST_BUFFER_VALUE)

    MOCK_STATIC_METHOD_1(, MULTITREE_HANDLE, TestDataSerializerDecode, BUFFER_HANDLE, decodeData)
    {
        if (decodeData == NULL)
        {
            throw std::runtime_error("this is expected to be called only with non-NULL arguments.");
        }
    }
    MOCK_METHOD_END(MULTITREE_HANDLE, TEST_MULTITREE_HANDLE)

    MOCK_STATIC_METHOD_0(, BUFFER_HANDLE, BUFFER_new)
    MOCK_METHOD_END(BUFFER_HANDLE, BASEIMPLEMENTATION::BUFFER_new())
    MOCK_STATIC_METHOD_1(, void, BUFFER_delete, BUFFER_HANDLE, b)
        BASEIMPLEMENTATION::BUFFER_delete(b);
    MOCK_VOID_METHOD_END()

    MOCK_STATIC_METHOD_2(, MULTITREE_HANDLE, MultiTree_Create, MULTITREE_CLONE_FUNCTION, cloneFunction, MULTITREE_FREE_FUNCTION, freeFunction)
    MOCK_METHOD_END(MULTITREE_HANDLE, TEST_MULTITREE_HANDLE)
    MOCK_STATIC_METHOD_1(, void, MultiTree_Destroy, MULTITREE_HANDLE, treeHandle)
    MOCK_VOID_METHOD_END()
};

DECLARE_GLOBAL_MOCK_METHOD_2(CDataSerializeMocks, , BUFFER_HANDLE, TestDataSerializerEncode, MULTITREE_HANDLE, multiTreeHandle, DATA_SERIALIZER_MULTITREE_TYPE, dataType);
DECLARE_GLOBAL_MOCK_METHOD_1(CDataSerializeMocks, , MULTITREE_HANDLE, TestDataSerializerDecode, BUFFER_HANDLE, decodeData);
DECLARE_GLOBAL_MOCK_METHOD_0(CDataSerializeMocks, , BUFFER_HANDLE, BUFFER_new);
DECLARE_GLOBAL_MOCK_METHOD_1(CDataSerializeMocks, , void, BUFFER_delete, BUFFER_HANDLE, b);
DECLARE_GLOBAL_MOCK_METHOD_2(CDataSerializeMocks, , MULTITREE_HANDLE, MultiTree_Create, MULTITREE_CLONE_FUNCTION, cloneFunction, MULTITREE_FREE_FUNCTION, freeFunction);
DECLARE_GLOBAL_MOCK_METHOD_1(CDataSerializeMocks, , void, MultiTree_Destroy, MULTITREE_HANDLE, treeHandle);

static CDataSerializeMocks* g_pMocks = NULL;
static MICROMOCK_MUTEX_HANDLE g_testByTest;

static MICROMOCK_GLOBAL_SEMAPHORE_HANDLE g_dllByDll;

BEGIN_TEST_SUITE(DataSerializer_ut)

    TEST_SUITE_INITIALIZE(TestClassInitialize)
    {
        TEST_INITIALIZE_MEMORY_DEBUG(g_dllByDll);

        g_testByTest = MicroMockCreateMutex();
        ASSERT_IS_NOT_NULL(g_testByTest);
        g_pMocks = new CDataSerializeMocks();
    }

    TEST_SUITE_CLEANUP(TestClassCleanup)
    {
        delete g_pMocks;
        MicroMockDestroyMutex(g_testByTest);
        TEST_DEINITIALIZE_MEMORY_DEBUG(g_dllByDll);

    }

    TEST_FUNCTION_INITIALIZE(setsBufferTempSize)
    {
        if (!MicroMockAcquireMutex(g_testByTest))
        {
            ASSERT_FAIL("our mutex is ABANDONED. Failure in test framework");
        }
        g_pMocks->ResetAllCalls(); /*so it is fresh and new*/
    }

    TEST_FUNCTION_CLEANUP(cleans)
    {
        g_pMocks->AssertActualAndExpectedCalls();
        if (!MicroMockReleaseMutex(g_testByTest) )
        {
            ASSERT_FAIL("failure in test framework at ReleaseMutex");
        }
    }

    /* DataSerializer_Encode_Tests BEGIN */

    /* Tests_SRS_DATA_SERIALIZER_07_003: [NULL shall be returned when an invalid parameter is supplied.] */
    TEST_FUNCTION(DataSerializer_Encode_with_NULL_MULTITREE_HANDLE_fails)
    {
        ///arrange

        ///act
        BUFFER_HANDLE pBuffer = DataSerializer_Encode(NULL, DATA_SERIALIZER_TYPE_CHAR_PTR, TestDataSerializerEncode);

        ///assert
        ASSERT_IS_NULL(pBuffer);
    }

    /* Tests_SRS_DATA_SERIALIZER_07_003: [NULL shall be returned when an invalid parameter is supplied.] */
    TEST_FUNCTION(DataSerializer_Encode_with_NULL_Serialize_Function_fails)
    {
        ///arrange

        ///act
        BUFFER_HANDLE pBuffer = DataSerializer_Encode(TEST_MULTITREE_HANDLE, DATA_SERIALIZER_TYPE_AGENT_DATA, NULL);		

        ///assert
        ASSERT_IS_NULL(pBuffer);
    }

    /* Tests_SRS_DATA_SERIALIZER_07_002: [DataSerializer_Encode shall return a valid BUFFER_HANDLE when the function executes successfully.] */
    /* Tests_SRS_DATA_SERIALIZER_07_009: [DataSerializer_Encode function shall call into the given DATA_SERIALIZER_ENCODE_FUNC callback with a valid BUFFER object and valid MULTITREE_HANDLE object.] */
    TEST_FUNCTION(DataSerializer_Encode_with_Valid_Parameter_Success)
    {
        ///arrange
        STRICT_EXPECTED_CALL((*g_pMocks), TestDataSerializerEncode(TEST_MULTITREE_HANDLE, DATA_SERIALIZER_TYPE_CHAR_PTR) ).
            SetReturn(TEST_BUFFER_VALUE);

        ///act
        BUFFER_HANDLE pBuffer = DataSerializer_Encode(TEST_MULTITREE_HANDLE, DATA_SERIALIZER_TYPE_CHAR_PTR, TestDataSerializerEncode);		

        ///assert
        ASSERT_ARE_EQUAL(void_ptr, pBuffer, TEST_BUFFER_VALUE);
    }

    /* Tests_SRS_DATA_SERIALIZER_07_010: [Upon a DATA_SERIALIZER_ENCODE_FUNC failure the DataSerializer_Encode function shall return NULL.] */
    TEST_FUNCTION(DataSerializer_Encode_Serialize_Function_Fail)
    {
        ///arrange
        STRICT_EXPECTED_CALL((*g_pMocks), TestDataSerializerEncode(TEST_MULTITREE_HANDLE, DATA_SERIALIZER_TYPE_AGENT_DATA)).
            SetReturn((BUFFER_HANDLE)NULL);

        ///act
        BUFFER_HANDLE pBuffer = DataSerializer_Encode(TEST_MULTITREE_HANDLE, DATA_SERIALIZER_TYPE_AGENT_DATA, TestDataSerializerEncode);

        ///assert
        ASSERT_IS_NULL(pBuffer);
    }
    /* DataSerializer_Encode_Tests END */

    /* DataSerializer_Decode_Tests BEGIN */

    /* Tests_SRS_DATA_SERIALIZER_07_007: [NULL shall be returned when an invalid parameter is supplied.] */
    TEST_FUNCTION(DataSerializer_Decode_with_NULL_BUFFER_fails)
    {
        ///arrange

        ///act
        MULTITREE_HANDLE multitreeHandle = DataSerializer_Decode(NULL, TestDataSerializerDecode);		

        ///assert
        ASSERT_IS_NULL(multitreeHandle);
    }

    /* Tests_SRS_DATA_SERIALIZER_07_007: [NULL shall be returned when an invalid parameter is supplied.] */
    TEST_FUNCTION(DataSerializer_Decode_with_NULL_Decode_Serialize_Function_fails)
    {
        ///arrange

        ///act
        MULTITREE_HANDLE multitreeHandle = DataSerializer_Decode(TEST_BUFFER_VALUE, NULL);		

        ///assert
        ASSERT_IS_NULL(multitreeHandle);
    }

    /* Tests_SRS_DATA_SERIALIZER_07_006: [DataSerializer_Decode shall return a valid MULTITREE_HANDLE when the function executes successfully.] */
    /* Tests_SRS_DATA_SERIALIZER_07_012: [DataSerializer_Decode function shall call into the given DATA_SERIALIZER_DECODE_FUNC callback with a valid BUFFER object and valid MULTITREE_HANDLE object.] */
    TEST_FUNCTION(DataSerializer_Decode_with_Valid_Parameter_Success)
    {
        ///arrange
        STRICT_EXPECTED_CALL((*g_pMocks), TestDataSerializerDecode(TEST_BUFFER_VALUE) ).
            SetReturn(TEST_MULTITREE_HANDLE);

        ///act
        MULTITREE_HANDLE multitreeHandle = DataSerializer_Decode(TEST_BUFFER_VALUE, TestDataSerializerDecode);		

        ///assert
        ASSERT_ARE_EQUAL(void_ptr, (void*)multitreeHandle, (void*)TEST_MULTITREE_HANDLE);
    }

    /* Tests_SRS_DATA_SERIALIZER_07_013: [Upon a DATA_SERIALIZER_DECODE_FUNC callback failure the DataSerializer_Encode function Shall return NULL.] */
    TEST_FUNCTION(DataSerializer_Decode_with_Serialize_Function_Fail)
    {
        ///arrange
        STRICT_EXPECTED_CALL((*g_pMocks), TestDataSerializerDecode(TEST_BUFFER_VALUE)).
            SetReturn((MULTITREE_HANDLE)NULL);

        ///act
        MULTITREE_HANDLE multitreeHandle = DataSerializer_Decode(TEST_BUFFER_VALUE, TestDataSerializerDecode);		

        ///assert
        ASSERT_IS_NULL(multitreeHandle);
    }
    /* DataSerializer_Decode_Tests END */

END_TEST_SUITE(DataSerializer_ut)
