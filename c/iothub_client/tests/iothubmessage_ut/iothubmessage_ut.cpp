// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <cstdlib>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "testrunnerswitcher.h"
#include "micromock.h"
#include "micromockcharstararenullterminatedstrings.h"
#include "iothub_message.h"
#include "azure_c_shared_utility/buffer_.h"
#include "azure_c_shared_utility/strings.h"
#include "azure_c_shared_utility/lock.h"
#include "azure_c_shared_utility/map.h"

static MICROMOCK_MUTEX_HANDLE g_testByTest;

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

#include "buffer.c"
#include "strings.c"

};

#define NUMBER_OF_CHAR      8

static const BUFFER_HANDLE TEST_BUFFER_HANDLE = (BUFFER_HANDLE)0x4242;
static const BUFFER_HANDLE TEST_BUFFER_HANDLE2 = (BUFFER_HANDLE)0x4343;

static size_t currentmalloc_call;
static size_t whenShallmalloc_fail;

static size_t currentBUFFER_new_call;
static size_t whenShallBUFFER_new_fail;

static size_t currentBUFFER_create_call;
static size_t whenShallBUFFER_create_fail;

static size_t currentBUFFER_clone_call;
static size_t whenShallBUFFER_clone_fail;

static size_t currentBUFFER_build_call;
static size_t whenShallBUFFER_build_fail;

static size_t currentBUFFER_content_call;
static size_t whenShallBUFFER_content_fail;

static size_t currentMap_Create_call;
static size_t whenShallMap_Create_fail;

static size_t currentMap_Clone_call;
static size_t whenShallMap_Clone_fail;

/*different STRING constructors*/
static size_t currentSTRING_new_call;
static size_t whenShallSTRING_new_fail;

static size_t currentSTRING_clone_call;
static size_t whenShallSTRING_clone_fail;

static size_t currentSTRING_construct_call;
static size_t whenShallSTRING_construct_fail;

static size_t currentSTRING_construct_n_call;
static size_t whenShallSTRING_construct_n_fail;


static size_t currentSTRING_concat_call;
static size_t whenShallSTRING_concat_fail;

static size_t currentSTRING_empty_call;
static size_t whenShallSTRING_empty_fail;

static size_t currentSTRING_concat_with_STRING_call;
static size_t whenShallSTRING_concat_with_STRING_fail;

static MAP_FILTER_CALLBACK g_mapFilterFunc;

static const unsigned char c[1] = { '3' };
static const char* TEST_MESSAGE_ID = "3820ADAE-E3CA-4065-843A-A6BDE950D8DC";
static const char* TEST_MESSAGE_ID2 = "052BA01A-ECBF-48CF-BC7B-64B315D898B7";

TYPED_MOCK_CLASS(CIoTHubMessageMocks, CGlobalMock)
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
    MOCK_METHOD_END(void*, BASEIMPLEMENTATION::gballoc_realloc(ptr, size));

    MOCK_STATIC_METHOD_1(, void, gballoc_free, void*, ptr)
        BASEIMPLEMENTATION::gballoc_free(ptr);
    MOCK_VOID_METHOD_END()

    MOCK_STATIC_METHOD_2(, int, mallocAndStrcpy_s, char**, destination, const char*, source)
    MOCK_METHOD_END(int, (*destination = (char*)BASEIMPLEMENTATION::gballoc_malloc(strlen(source) + 1), strcpy(*destination, source), 0))

    /* BUFFER Mocks */
    MOCK_STATIC_METHOD_0(, BUFFER_HANDLE, BUFFER_new)
        BUFFER_HANDLE result2;
        currentBUFFER_new_call++;
        if (whenShallBUFFER_new_fail > 0)
        {
            if (currentBUFFER_new_call == whenShallBUFFER_new_fail)
            {
                result2 = NULL;
            }
            else
            {
                result2 = BASEIMPLEMENTATION::BUFFER_new();
            }
        }
        else
        {
            result2 = BASEIMPLEMENTATION::BUFFER_new();
        }
    MOCK_METHOD_END(BUFFER_HANDLE, result2)

    MOCK_STATIC_METHOD_2(, BUFFER_HANDLE, BUFFER_create, const unsigned char*, source, size_t, size)
        BUFFER_HANDLE result2;
    currentBUFFER_create_call++;
    if (whenShallBUFFER_create_fail > 0)
    {
        if (currentBUFFER_create_call == whenShallBUFFER_create_fail)
        {
            result2 = NULL;
        }
        else
        {
            result2 = BASEIMPLEMENTATION::BUFFER_create(source, size);
        }
    }
    else
    {
        result2 = BASEIMPLEMENTATION::BUFFER_create(source, size);
    }
    MOCK_METHOD_END(BUFFER_HANDLE, result2)


    MOCK_STATIC_METHOD_1(, void, BUFFER_delete, BUFFER_HANDLE, s)
        BASEIMPLEMENTATION::BUFFER_delete(s);
    MOCK_VOID_METHOD_END()

    MOCK_STATIC_METHOD_1(, BUFFER_HANDLE, BUFFER_clone, BUFFER_HANDLE, handle);
        BUFFER_HANDLE result2;
        currentBUFFER_clone_call++;
        if (currentBUFFER_clone_call == whenShallBUFFER_clone_fail)
        {
            result2 = (BUFFER_HANDLE)NULL;
        }
        else
        {
            result2 = BASEIMPLEMENTATION::BUFFER_clone(handle);
        }
    MOCK_METHOD_END(BUFFER_HANDLE, result2)

    MOCK_STATIC_METHOD_1(, unsigned char*, BUFFER_u_char, BUFFER_HANDLE, handle);
        unsigned char* result2;
        result2 = BASEIMPLEMENTATION::BUFFER_u_char(handle);
    MOCK_METHOD_END(unsigned char*, result2)

    MOCK_STATIC_METHOD_1(, size_t, BUFFER_length, BUFFER_HANDLE, handle);
        size_t result2;
        result2 = BASEIMPLEMENTATION::BUFFER_length(handle);
    MOCK_METHOD_END(size_t, result2)

    MOCK_STATIC_METHOD_3(, int, BUFFER_build, BUFFER_HANDLE, handle, const unsigned char*, source, size_t, size)
    MOCK_METHOD_END(int, BASEIMPLEMENTATION::BUFFER_build(handle, source, size))

    MOCK_STATIC_METHOD_2(, int, BUFFER_content, BUFFER_HANDLE, b, const unsigned char**, content)
    MOCK_METHOD_END(int, BASEIMPLEMENTATION::BUFFER_content(b, content))

    MOCK_STATIC_METHOD_2(, int, BUFFER_size, BUFFER_HANDLE, b, size_t*, size)
    MOCK_METHOD_END(int, BASEIMPLEMENTATION::BUFFER_size(b, size))

    MOCK_STATIC_METHOD_1(, MAP_HANDLE, Map_Create, MAP_FILTER_CALLBACK, mapFilterFunc)
        MAP_HANDLE result2;
        g_mapFilterFunc = mapFilterFunc;
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

        /*Strings*/
        MOCK_STATIC_METHOD_0(, STRING_HANDLE, STRING_new)
        STRING_HANDLE result2;
    currentSTRING_new_call++;
    if (whenShallSTRING_new_fail > 0)
    {
        if (currentSTRING_new_call == whenShallSTRING_new_fail)
        {
            result2 = (STRING_HANDLE)NULL;
        }
        else
        {
            result2 = BASEIMPLEMENTATION::STRING_new();
        }
    }
    else
    {
        result2 = BASEIMPLEMENTATION::STRING_new();
    }
    MOCK_METHOD_END(STRING_HANDLE, result2)

    MOCK_STATIC_METHOD_1(, STRING_HANDLE, STRING_clone, STRING_HANDLE, handle)
        STRING_HANDLE result2;
        currentSTRING_clone_call++;
        if (whenShallSTRING_clone_fail > 0)
        {
            if (currentSTRING_clone_call == whenShallSTRING_clone_fail)
            {
                result2 = (STRING_HANDLE)NULL;
            }
            else
            {
                result2 = BASEIMPLEMENTATION::STRING_clone(handle);
            }
        }
        else
        {
            result2 = BASEIMPLEMENTATION::STRING_clone(handle);
        }
    MOCK_METHOD_END(STRING_HANDLE, result2)

    MOCK_STATIC_METHOD_1(, STRING_HANDLE, STRING_construct, const char*, source)
        STRING_HANDLE result2;
        currentSTRING_construct_call++;
        if (whenShallSTRING_construct_fail > 0)
        {
            if (currentSTRING_construct_call == whenShallSTRING_construct_fail)
            {
                result2 = (STRING_HANDLE)NULL;
            }
            else
            {
                result2 = BASEIMPLEMENTATION::STRING_construct(source);
            }
        }
        else
        {
            result2 = BASEIMPLEMENTATION::STRING_construct(source);
        }
    MOCK_METHOD_END(STRING_HANDLE, result2)

    MOCK_STATIC_METHOD_2(, STRING_HANDLE, STRING_construct_n, const char*, source, size_t, size)
        STRING_HANDLE result2;
        currentSTRING_construct_n_call++;
        if (whenShallSTRING_construct_n_fail > 0)
        {
            if (currentSTRING_construct_n_call == whenShallSTRING_construct_n_fail)
            {
                result2 = (STRING_HANDLE)NULL;
            }
            else
            {
                result2 = BASEIMPLEMENTATION::STRING_construct_n(source, size);
            }
        }
        else
        {
            result2 = BASEIMPLEMENTATION::STRING_construct_n(source, size);
        }
    MOCK_METHOD_END(STRING_HANDLE, result2)

    MOCK_STATIC_METHOD_1(, void, STRING_delete, STRING_HANDLE, s)
        BASEIMPLEMENTATION::STRING_delete(s);
    MOCK_VOID_METHOD_END()

    MOCK_STATIC_METHOD_2(, int, STRING_concat, STRING_HANDLE, s1, const char*, s2)
        currentSTRING_concat_call++;
    MOCK_METHOD_END(int, (((whenShallSTRING_concat_fail > 0) && (currentSTRING_concat_call == whenShallSTRING_concat_fail)) ? __LINE__ : BASEIMPLEMENTATION::STRING_concat(s1, s2)));

    MOCK_STATIC_METHOD_2(, int, STRING_concat_with_STRING, STRING_HANDLE, s1, STRING_HANDLE, s2)
        currentSTRING_concat_with_STRING_call++;
    MOCK_METHOD_END(int, (((currentSTRING_concat_with_STRING_call > 0) && (currentSTRING_concat_with_STRING_call == whenShallSTRING_concat_with_STRING_fail)) ? __LINE__ : BASEIMPLEMENTATION::STRING_concat_with_STRING(s1, s2)));




    MOCK_STATIC_METHOD_1(, int, STRING_empty, STRING_HANDLE, s)
        currentSTRING_concat_call++;
    MOCK_METHOD_END(int, BASEIMPLEMENTATION::STRING_empty(s));

    MOCK_STATIC_METHOD_1(, const char*, STRING_c_str, STRING_HANDLE, s)
        MOCK_METHOD_END(const char*, BASEIMPLEMENTATION::STRING_c_str(s))

        MOCK_STATIC_METHOD_1(, size_t, STRING_length, STRING_HANDLE, handle)
        MOCK_METHOD_END(size_t, BASEIMPLEMENTATION::STRING_length(handle))
};

DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubMessageMocks, , void*, gballoc_malloc, size_t, size);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubMessageMocks, , void*, gballoc_realloc, void*, ptr, size_t, size);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubMessageMocks, , void, gballoc_free, void*, ptr);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubMessageMocks, , int, mallocAndStrcpy_s, char**, destination, const char*, source);

DECLARE_GLOBAL_MOCK_METHOD_0(CIoTHubMessageMocks, , BUFFER_HANDLE, BUFFER_new);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubMessageMocks, , BUFFER_HANDLE, BUFFER_create, const unsigned char*, source, size_t, size);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubMessageMocks, , void, BUFFER_delete, BUFFER_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubMessageMocks, , BUFFER_HANDLE, BUFFER_clone, BUFFER_HANDLE, handle);

DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubMessageMocks, , unsigned char*, BUFFER_u_char, BUFFER_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubMessageMocks, , size_t, BUFFER_length, BUFFER_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubMessageMocks, , int, BUFFER_build, BUFFER_HANDLE, handle, const unsigned char*, source, size_t, size);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubMessageMocks, , int, BUFFER_content, BUFFER_HANDLE, b, const unsigned char**, content);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubMessageMocks, , int, BUFFER_size, BUFFER_HANDLE, handle, size_t*, size);

DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubMessageMocks, , MAP_HANDLE, Map_Create, MAP_FILTER_CALLBACK, mapFilterFunc);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubMessageMocks, , void, Map_Destroy, MAP_HANDLE, handle)
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubMessageMocks, , MAP_HANDLE, Map_Clone, MAP_HANDLE, handle);

DECLARE_GLOBAL_MOCK_METHOD_0(CIoTHubMessageMocks, , STRING_HANDLE, STRING_new);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubMessageMocks, , STRING_HANDLE, STRING_clone, STRING_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubMessageMocks, , STRING_HANDLE, STRING_construct, const char*, s);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubMessageMocks, , STRING_HANDLE, STRING_construct_n, const char*, s, size_t, size);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubMessageMocks, , void, STRING_delete, STRING_HANDLE, s);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubMessageMocks, , int, STRING_concat, STRING_HANDLE, s1, const char*, s2);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubMessageMocks, , int, STRING_concat_with_STRING, STRING_HANDLE, s1, STRING_HANDLE, s2);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubMessageMocks, , int, STRING_empty, STRING_HANDLE, s1);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubMessageMocks, , const char*, STRING_c_str, STRING_HANDLE, s);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubMessageMocks, , size_t, STRING_length, STRING_HANDLE, handle);

DEFINE_MICROMOCK_ENUM_TO_STRING(IOTHUB_MESSAGE_RESULT, IOTHUB_MESSAGE_RESULT_VALUES);
DEFINE_MICROMOCK_ENUM_TO_STRING(IOTHUBMESSAGE_CONTENT_TYPE, IOTHUBMESSAGE_CONTENT_TYPE_VALUES);

static MICROMOCK_GLOBAL_SEMAPHORE_HANDLE g_dllByDll;

BEGIN_TEST_SUITE(iothubmessage_ut)

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

        currentBUFFER_new_call = 0;
        whenShallBUFFER_new_fail = 0;

        currentBUFFER_create_call = 0;
        whenShallBUFFER_create_fail = 0;

        currentBUFFER_clone_call = 0;
        whenShallBUFFER_clone_fail = 0;

        currentBUFFER_build_call = 0;
        whenShallBUFFER_build_fail = 0;

        currentBUFFER_content_call = 0;
        whenShallBUFFER_content_fail = 0;

        currentMap_Create_call = 0;
        whenShallMap_Create_fail = 0;

        currentMap_Clone_call = 0;
        whenShallMap_Clone_fail = 0;

        currentmalloc_call = 0;
        whenShallmalloc_fail = 0;

        currentSTRING_new_call = 0;
        whenShallSTRING_new_fail = 0;

        currentSTRING_clone_call = 0;
        whenShallSTRING_clone_fail = 0;

        currentSTRING_construct_call = 0;
        whenShallSTRING_construct_fail = 0;

        currentSTRING_concat_call = 0;
        whenShallSTRING_concat_fail = 0;

        currentSTRING_empty_call = 0;
        whenShallSTRING_empty_fail = 0;

        currentSTRING_concat_with_STRING_call = 0;
        whenShallSTRING_concat_with_STRING_fail = 0;
    }

    TEST_FUNCTION_CLEANUP(TestMethodCleanup)
    {
        if (!MicroMockReleaseMutex(g_testByTest))
        {
            ASSERT_FAIL("failure in test framework at ReleaseMutex");
        }
    }

    /*Tests_SRS_IOTHUBMESSAGE_02_022: [IoTHubMessage_CreateFromByteArray shall call BUFFER_create passing byteArray and size as parameters.]*/
    /*Tests_SRS_IOTHUBMESSAGE_02_023: [IoTHubMessage_CreateFromByteArray shall call Map_Create to create the message properties.]*/
    /*Tests_SRS_IOTHUBMESSAGE_02_025: [Otherwise, IoTHubMessage_CreateFromByteArray shall return a non-NULL handle.] */
    /*Tests_SRS_IOTHUBMESSAGE_02_026: [The type of the new message shall be IOTHUBMESSAGE_BYTEARRAY.] */
    /*Tests_SRS_IOTHUBMESSAGE_02_009: [Otherwise IoTHubMessage_GetContentType shall return the type of the message.] */
    TEST_FUNCTION(IoTHubMessage_CreateFromByteArray_happy_path)
    {
        ///arrange
        CIoTHubMessageMocks mocks;

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, BUFFER_create(c, 1));
        STRICT_EXPECTED_CALL(mocks, Map_Create(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        ///act
        auto h = IoTHubMessage_CreateFromByteArray(c, 1);

        ///assert
        ASSERT_IS_NOT_NULL(h);
        mocks.AssertActualAndExpectedCalls();
        auto messageType = IoTHubMessage_GetContentType(h);
        ASSERT_ARE_EQUAL(IOTHUBMESSAGE_CONTENT_TYPE, IOTHUBMESSAGE_BYTEARRAY, messageType);

        ///cleanup
        IoTHubMessage_Destroy(h);
    }

    /*Tests_SRS_IOTHUBMESSAGE_06_002: [If size is NOT zero then byteArray MUST NOT be NULL*/
    TEST_FUNCTION(IoTHubMessage_CreateFromByteArray_fails_when_size_non_zero_buffer_NULL)
    {
        ///arrange
        CIoTHubMessageMocks mocks;

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        ///act
        auto h = IoTHubMessage_CreateFromByteArray(NULL, 1);

        ///assert
        ASSERT_IS_NULL(h);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
    }

    /*Tests_SRS_IOTHUBMESSAGE_06_001: [If size is zero then byteArray may be NULL.]*/
    TEST_FUNCTION(IoTHubMessage_CreateFromByteArray_succeeds_when_size_0_and_buffer_NULL)
    {
        ///arrange
        CIoTHubMessageMocks mocks;

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, BUFFER_create(IGNORED_PTR_ARG, 0)).IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, Map_Create(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        ///act
        auto h = IoTHubMessage_CreateFromByteArray(NULL, 0);

        ///assert
        ASSERT_IS_NOT_NULL(h);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubMessage_Destroy(h);
    }

    /*Tests_SRS_IOTHUBMESSAGE_06_001: [If size is zero then byteArray may be NULL.]*/
    TEST_FUNCTION(IoTHubMessage_CreateFromByteArray_succeeds_when_size_0_and_buffer_non_NULL)
    {
        ///arrange
        CIoTHubMessageMocks mocks;

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, BUFFER_create(IGNORED_PTR_ARG, 0)).IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, Map_Create(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        ///act
        auto h = IoTHubMessage_CreateFromByteArray(c, 0);

        ///assert
        ASSERT_IS_NOT_NULL(h);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubMessage_Destroy(h);
    }

    /*Tests_SRS_IOTHUBMESSAGE_02_024: [If there are any errors then IoTHubMessage_CreateFromByteArray shall return NULL*/
    TEST_FUNCTION(IoTHubMessage_CreateFromByteArray_fails_when_Map_CReate_fails)
    {
        ///arrange
        CIoTHubMessageMocks mocks;

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, BUFFER_create(c, 1));
        STRICT_EXPECTED_CALL(mocks, BUFFER_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        whenShallMap_Create_fail = currentMap_Create_call + 1;
        STRICT_EXPECTED_CALL(mocks, Map_Create(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        ///act
        auto h = IoTHubMessage_CreateFromByteArray(c, 1);

        ///assert
        ASSERT_IS_NULL(h);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
    }

    /*Tests_SRS_IOTHUBMESSAGE_02_024: [If there are any errors then IoTHubMessage_CreateFromByteArray shall return NULL*/
    TEST_FUNCTION(IoTHubMessage_CreateFromByteArray_fails_when_Buffer_CReate_fails)
    {
        ///arrange
        CIoTHubMessageMocks mocks;

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        whenShallBUFFER_create_fail = currentBUFFER_create_call + 1;
        STRICT_EXPECTED_CALL(mocks, BUFFER_create(c, 1));

        ///act
        auto h = IoTHubMessage_CreateFromByteArray(c, 1);

        ///assert
        ASSERT_IS_NULL(h);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
    }

    /*Tests_SRS_IOTHUBMESSAGE_02_024: [If there are any errors then IoTHubMessage_CreateFromByteArray shall return NULL*/
    TEST_FUNCTION(IoTHubMessage_CreateFromByteArray_fails_when_gballoc_fails)
    {
        ///arrange
        CIoTHubMessageMocks mocks;

        whenShallmalloc_fail = currentmalloc_call + 1;
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);
        
        ///act
        auto h = IoTHubMessage_CreateFromByteArray(c, 1);

        ///assert
        ASSERT_IS_NULL(h);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
    }

    /*Tests_SRS_IOTHUBMESSAGE_02_027: [IoTHubMessage_CreateFromString shall call STRING_construct passing source as parameter.] */
    /*Tests_SRS_IOTHUBMESSAGE_02_028: [IoTHubMessage_CreateFromString shall call Map_Create to create the message properties.] */
    /*Tests_SRS_IOTHUBMESSAGE_02_031: [Otherwise, IoTHubMessage_CreateFromString shall return a non-NULL handle.] */
    /*Tests_SRS_IOTHUBMESSAGE_02_032: [The type of the new message shall be IOTHUBMESSAGE_STRING.] */
    /*Tests_SRS_IOTHUBMESSAGE_02_009: [Otherwise IoTHubMessage_GetContentType shall return the type of the message.] */
    TEST_FUNCTION(IoTHubMessage_CreateFromString_happy_path)
    {
        ///arrange
        CIoTHubMessageMocks mocks;

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_construct("a"));
        STRICT_EXPECTED_CALL(mocks, Map_Create(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        ///act
        auto h = IoTHubMessage_CreateFromString("a");

        ///assert
        ASSERT_IS_NOT_NULL(h);
        mocks.AssertActualAndExpectedCalls();
        auto messageType = IoTHubMessage_GetContentType(h);
        ASSERT_ARE_EQUAL(IOTHUBMESSAGE_CONTENT_TYPE, IOTHUBMESSAGE_STRING, messageType);

        ///cleanup
        IoTHubMessage_Destroy(h);
    }

    /*Tests_SRS_IOTHUBMESSAGE_02_029: [If there are any encountered in the execution of IoTHubMessage_CreateFromString then IoTHubMessage_CreateFromString shall return NULL.] */
    TEST_FUNCTION(IoTHubMessage_CreateFromString_fails_when_Map_Create_fails)
    {
        ///arrange
        CIoTHubMessageMocks mocks;

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);


        STRICT_EXPECTED_CALL(mocks, STRING_construct("a"));
        STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        whenShallMap_Create_fail = currentMap_Create_call + 1;
        STRICT_EXPECTED_CALL(mocks, Map_Create(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        ///act
        auto h = IoTHubMessage_CreateFromString("a");

        ///assert
        ASSERT_IS_NULL(h);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
    }

    /*Tests_SRS_IOTHUBMESSAGE_02_029: [If there are any encountered in the execution of IoTHubMessage_CreateFromString then IoTHubMessage_CreateFromString shall return NULL.] */
    TEST_FUNCTION(IoTHubMessage_CreateFromString_fails_when_String_construct_fails)
    {
        ///arrange
        CIoTHubMessageMocks mocks;

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        whenShallSTRING_construct_fail = currentSTRING_construct_call + 1;
        STRICT_EXPECTED_CALL(mocks, STRING_construct("a"));

        ///act
        auto h = IoTHubMessage_CreateFromString("a");

        ///assert
        ASSERT_IS_NULL(h);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
    }

    /*Tests_SRS_IOTHUBMESSAGE_02_029: [If there are any encountered in the execution of IoTHubMessage_CreateFromString then IoTHubMessage_CreateFromString shall return NULL.] */
    TEST_FUNCTION(IoTHubMessage_CreateFromString_fails_when_gbaloc_fails)
    {
        ///arrange
        CIoTHubMessageMocks mocks;

        whenShallmalloc_fail = currentmalloc_call + 1;
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);

        ///act
        auto h = IoTHubMessage_CreateFromString("a");

        ///assert
        ASSERT_IS_NULL(h);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
    }

    /*Tests_SRS_IOTHUBMESSAGE_01_004: [If iotHubMessageHandle is NULL, IoTHubMessage_Destroy shall do nothing.] */
    TEST_FUNCTION(IoTHubMessage_Destroy_With_NULL_handle_does_nothing)
    {
        ///arrange
        CIoTHubMessageMocks mocks;

        ///act
        IoTHubMessage_Destroy(NULL);

        ///assert
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
    }

    /*Tests_SRS_IOTHUBMESSAGE_01_003: [IoTHubMessage_Destroy shall free all resources associated with iotHubMessageHandle.]  */
    TEST_FUNCTION(IoTHubMessage_Destroy_destroys_a_BYTEARRAY_IoTHubMEssage)
    {
        ///arrange
        CIoTHubMessageMocks mocks;
        auto h = IoTHubMessage_CreateFromByteArray(c, 1);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Map_Destroy(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, BUFFER_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(h));
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)).IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)).IgnoreArgument(1);

        ///act
        IoTHubMessage_Destroy(h);

        ///assert
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
    }

    /*Tests_SRS_IOTHUBMESSAGE_01_003: [IoTHubMessage_Destroy shall free all resources associated with iotHubMessageHandle.]  */
    TEST_FUNCTION(IoTHubMessage_Destroy_destroys_a_STRING_IoTHubMEssage)
    {
        ///arrange
        CIoTHubMessageMocks mocks;
        auto h = IoTHubMessage_CreateFromString("aaaa");
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Map_Destroy(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(h));
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)).IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)).IgnoreArgument(1);

        ///act
        IoTHubMessage_Destroy(h);

        ///assert
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
    }

    /*Tests_SRS_IOTHUBMESSAGE_01_011: [The pointer shall be obtained by using BUFFER_u_char and it shall be copied in the buffer argument.]*/
    /*Tests_SRS_IOTHUBMESSAGE_01_012: [The size of the associated data shall be obtained by using BUFFER_length and it shall be copied to the size argument.]*/
    /*Tests_SRS_IOTHUBMESSAGE_02_033: [IoTHubMessage_GetByteArray shall return IOTHUBMESSAGE_OK when all oeprations complete succesfully.] */
    TEST_FUNCTION(IoTHubMessage_GetByteArray_happy_path)
    {
        ///arrange
        CIoTHubMessageMocks mocks;
        auto h = IoTHubMessage_CreateFromByteArray(c, 1);
        const unsigned char* byteArray;
        size_t size;
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, BUFFER_length(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, BUFFER_u_char(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        ///act
        auto r = IoTHubMessage_GetByteArray(h, &byteArray, &size);

        ///assert
        ASSERT_ARE_EQUAL(IOTHUB_MESSAGE_RESULT, IOTHUB_MESSAGE_OK, r);
        ASSERT_ARE_EQUAL(uint8_t, c[0], byteArray[0]);
        ASSERT_ARE_EQUAL(size_t, 1, size);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubMessage_Destroy(h);
    }

    /*Tests_SRS_IOTHUBMESSAGE_01_014: [If any of the arguments passed to IoTHubMessage_GetByteArray  is NULL IoTHubMessage_GetByteArray shall return IOTHUBMESSAGE_INVALID_ARG.] */
    TEST_FUNCTION(IoTHubMessage_GetByteArray_with_NULL_handle_fails)
    {
        ///arrange
        CIoTHubMessageMocks mocks;
        auto h = IoTHubMessage_CreateFromByteArray(c, 1);
        const unsigned char* byteArray;
        size_t size;
        mocks.ResetAllCalls();

        ///act
        auto r = IoTHubMessage_GetByteArray(NULL, &byteArray, &size);

        ///assert
        ASSERT_ARE_EQUAL(IOTHUB_MESSAGE_RESULT, IOTHUB_MESSAGE_INVALID_ARG, r);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubMessage_Destroy(h);
    }

    /*Tests_SRS_IOTHUBMESSAGE_01_014: [If any of the arguments passed to IoTHubMessage_GetByteArray  is NULL IoTHubMessage_GetByteArray shall return IOTHUBMESSAGE_INVALID_ARG.] */
    TEST_FUNCTION(IoTHubMessage_GetByteArray_with_NULL_byteArray_fails)
    {
        ///arrange
        CIoTHubMessageMocks mocks;
        auto h = IoTHubMessage_CreateFromByteArray(c, 1);
        size_t size;
        mocks.ResetAllCalls();

        ///act
        auto r = IoTHubMessage_GetByteArray(h, NULL, &size);

        ///assert
        ASSERT_ARE_EQUAL(IOTHUB_MESSAGE_RESULT, IOTHUB_MESSAGE_INVALID_ARG, r);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubMessage_Destroy(h);
    }

    /*Tests_SRS_IOTHUBMESSAGE_01_014: [If any of the arguments passed to IoTHubMessage_GetByteArray  is NULL IoTHubMessage_GetByteArray shall return IOTHUBMESSAGE_INVALID_ARG.] */
    TEST_FUNCTION(IoTHubMessage_GetByteArray_with_NULL_size_fails)
    {
        ///arrange
        CIoTHubMessageMocks mocks;
        auto h = IoTHubMessage_CreateFromByteArray(c, 1);
        const unsigned char* byteArray;
        mocks.ResetAllCalls();

        ///act
        auto r = IoTHubMessage_GetByteArray(h, &byteArray, NULL);

        ///assert
        ASSERT_ARE_EQUAL(IOTHUB_MESSAGE_RESULT, IOTHUB_MESSAGE_INVALID_ARG, r);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubMessage_Destroy(h);
    }

    /*Tests_SRS_IOTHUBMESSAGE_02_021: [If iotHubMessageHandle is not a iothubmessage containing BYTEARRAY data, then IoTHubMessage_GetByteArray  shall return IOTHUBMESSAGE_INVALID_ARG.]*/
    TEST_FUNCTION(IoTHubMessage_GetByteArray_with_STRING_fails)
    {
        ///arrange
        CIoTHubMessageMocks mocks;
        auto h = IoTHubMessage_CreateFromString("c, 1");
        const unsigned char* byteArray;
        size_t size;
        mocks.ResetAllCalls();

        ///act
        auto r = IoTHubMessage_GetByteArray(h, &byteArray, &size);

        ///assert
        ASSERT_ARE_EQUAL(IOTHUB_MESSAGE_RESULT, IOTHUB_MESSAGE_INVALID_ARG, r);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubMessage_Destroy(h);
    }

    /*Tests_SRS_IOTHUBMESSAGE_03_001: [IoTHubMessage_Clone shall create a new IoT hub message with data content identical to that of the iotHubMessageHandle parameter.]*/
    /*Tests_SRS_IOTHUBMESSAGE_02_006: [IoTHubMessage_Clone shall clone the content by a call to BUFFER_clone or STRING_clone] */
    /*Tests_SRS_IOTHUBMESSAGE_02_005: [IoTHubMessage_Clone shall clone the properties map by using Map_Clone.] */
    /*Tests_SRS_IOTHUBMESSAGE_03_002: [IoTHubMessage_Clone shall return upon success a non-NULL handle to the newly created IoT hub message.]*/
    TEST_FUNCTION(IoTHubMessage_Clone_with_BYTE_ARRAY_happy_path) 
    {
        ///arrange
        CIoTHubMessageMocks mocks;
        auto h = IoTHubMessage_CreateFromByteArray(c, 1);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, BUFFER_clone(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, Map_Clone(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        ///act
        auto r = IoTHubMessage_Clone(h);

        ///assert
        ASSERT_IS_NOT_NULL(r);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubMessage_Destroy(r);
        IoTHubMessage_Destroy(h);
    }

    /*Tests_SRS_IOTHUBMESSAGE_03_004: [IoTHubMessage_Clone shall return NULL if it fails for any reason.]*/
    TEST_FUNCTION(IoTHubMessage_Clone_with_BYTE_ARRAY_fails_when_Map_Clone_fails)
    {
        ///arrange
        CIoTHubMessageMocks mocks;
        auto h = IoTHubMessage_CreateFromByteArray(c, 1);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, BUFFER_clone(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, BUFFER_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        whenShallMap_Clone_fail = currentMap_Clone_call + 1;
        STRICT_EXPECTED_CALL(mocks, Map_Clone(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        ///act
        auto r = IoTHubMessage_Clone(h);

        ///assert
        ASSERT_IS_NULL(r);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubMessage_Destroy(h);
    }

    /*Tests_SRS_IOTHUBMESSAGE_03_004: [IoTHubMessage_Clone shall return NULL if it fails for any reason.]*/
    TEST_FUNCTION(IoTHubMessage_Clone_with_BYTE_ARRAY_fails_when_BUFFER_clone_fails)
    {
        ///arrange
        CIoTHubMessageMocks mocks;
        auto h = IoTHubMessage_CreateFromByteArray(c, 1);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        whenShallBUFFER_clone_fail = currentBUFFER_clone_call + 1;
        STRICT_EXPECTED_CALL(mocks, BUFFER_clone(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        ///act
        auto r = IoTHubMessage_Clone(h);

        ///assert
        ASSERT_IS_NULL(r);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubMessage_Destroy(h);
    }


    /*Tests_SRS_IOTHUBMESSAGE_03_004: [IoTHubMessage_Clone shall return NULL if it fails for any reason.]*/
    TEST_FUNCTION(IoTHubMessage_Clone_with_BYTE_ARRAY_fails_when_gballoc_fails)
    {
        ///arrange
        CIoTHubMessageMocks mocks;
        auto h = IoTHubMessage_CreateFromByteArray(c, 1);
        mocks.ResetAllCalls();

        whenShallmalloc_fail = currentmalloc_call + 1;
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);

        ///act
        auto r = IoTHubMessage_Clone(h);

        ///assert
        ASSERT_IS_NULL(r);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubMessage_Destroy(h);
    }

    /*Tests_SRS_IOTHUBMESSAGE_03_001: [IoTHubMessage_Clone shall create a new IoT hub message with data content identical to that of the iotHubMessageHandle parameter.]*/
    /*Tests_SRS_IOTHUBMESSAGE_02_006: [IoTHubMessage_Clone shall clone the content by a call to BUFFER_clone or STRING_clone] */
    /*Tests_SRS_IOTHUBMESSAGE_02_005: [IoTHubMessage_Clone shall clone the properties map by using Map_Clone.] */
    /*Tests_SRS_IOTHUBMESSAGE_03_002: [IoTHubMessage_Clone shall return upon success a non-NULL handle to the newly created IoT hub message.]*/
    TEST_FUNCTION(IoTHubMessage_Clone_with_STRING_happy_path)
    {
        ///arrange
        CIoTHubMessageMocks mocks;
        auto h = IoTHubMessage_CreateFromString("c, 1");
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_clone(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, Map_Clone(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        ///act
        auto r = IoTHubMessage_Clone(h);

        ///assert
        ASSERT_IS_NOT_NULL(r);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubMessage_Destroy(r);
        IoTHubMessage_Destroy(h);
    }

    /*Tests_SRS_IOTHUBMESSAGE_03_004: [IoTHubMessage_Clone shall return NULL if it fails for any reason.]*/
    TEST_FUNCTION(IoTHubMessage_Clone_with_STRING_fails_when_Map_Clone_fails)
    {
        ///arrange
        CIoTHubMessageMocks mocks;
        auto h = IoTHubMessage_CreateFromString("c, 1");
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_clone(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        whenShallMap_Clone_fail = currentMap_Clone_call + 1;
        STRICT_EXPECTED_CALL(mocks, Map_Clone(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        ///act
        auto r = IoTHubMessage_Clone(h);

        ///assert
        ASSERT_IS_NULL(r);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubMessage_Destroy(h);
    }

    /*Tests_SRS_IOTHUBMESSAGE_03_004: [IoTHubMessage_Clone shall return NULL if it fails for any reason.]*/
    TEST_FUNCTION(IoTHubMessage_Clone_with_STRING_fails_when_STRING_clone_fails)
    {
        ///arrange
        CIoTHubMessageMocks mocks;
        auto h = IoTHubMessage_CreateFromString("c, 1");
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        whenShallSTRING_clone_fail = currentSTRING_clone_call + 1;
        STRICT_EXPECTED_CALL(mocks, STRING_clone(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        ///act
        auto r = IoTHubMessage_Clone(h);

        ///assert
        ASSERT_IS_NULL(r);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubMessage_Destroy(h);
    }

    /*Tests_SRS_IOTHUBMESSAGE_03_004: [IoTHubMessage_Clone shall return NULL if it fails for any reason.]*/
    TEST_FUNCTION(IoTHubMessage_Clone_with_STRING_fails_when_gballoc_fails)
    {
        ///arrange
        CIoTHubMessageMocks mocks;
        auto h = IoTHubMessage_CreateFromString("c, 1");
        mocks.ResetAllCalls();

        whenShallmalloc_fail = currentmalloc_call + 1;
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);

        ///act
        auto r = IoTHubMessage_Clone(h);

        ///assert
        ASSERT_IS_NULL(r);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubMessage_Destroy(h);
    }

    /*Tests_SRS_IOTHUBMESSAGE_02_002: [Otherwise, for any non-NULL iotHubMessageHandle it shall return a non-NULL MAP_HANDLE.] */
    TEST_FUNCTION(IoTHubMessage_Properties_happy_path)
    {        
        ///arrange
        CIoTHubMessageMocks mocks;
        auto h = IoTHubMessage_CreateFromString("c, 1");
        mocks.ResetAllCalls();

        ///act
        auto r = IoTHubMessage_Properties(h);

        ///assert
        ASSERT_IS_NOT_NULL(r);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubMessage_Destroy(h);
    }

    /*Tests_SRS_IOTHUBMESSAGE_02_001: [If iotHubMessageHandle is NULL then IoTHubMessage_Properties shall return NULL.] */
    TEST_FUNCTION(IoTHubMessage_Properties_with_NULL_handle_retuns_NULL)
    {
        ///arrange
        CIoTHubMessageMocks mocks;

        ///act
        auto r = IoTHubMessage_Properties(NULL);

        ///assert
        ASSERT_IS_NULL(r);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
    }

    /*Tests_SRS_IOTHUBMESSAGE_02_008: [If any parameter is NULL then IoTHubMessage_GetContentType shall return IOTHUBMESSAGE_UNKNOWN.] */
    TEST_FUNCTION(IoTHubMessage_GetContentType_with_NULL_handle_fails)
    {
        ///arrange
        CIoTHubMessageMocks mocks;
        mocks.ResetAllCalls();

        ///act
        auto r = IoTHubMessage_GetContentType(NULL);

        ///assert
        ASSERT_ARE_EQUAL(IOTHUBMESSAGE_CONTENT_TYPE, IOTHUBMESSAGE_UNKNOWN, r);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
    }

    /*Tests_SRS_IOTHUBMESSAGE_02_016: [If any parameter is NULL then IoTHubMessage_GetString  shall return NULL.] */
    TEST_FUNCTION(IoTHubMessage_GetString_with_NULL_handle_returns_NULL)
    {
        ///arrange
        CIoTHubMessageMocks mocks;
        mocks.ResetAllCalls();

        ///act
        auto r = IoTHubMessage_GetString(NULL);

        ///assert
        ASSERT_IS_NULL(r);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
    }

    /*Tests_SRS_IOTHUBMESSAGE_02_018: [IoTHubMessage_GetStringData shall return the currently stored null terminated string.] */
    TEST_FUNCTION(IoTHubMessage_GetString_happy_path)
    {
        ///arrange
        CIoTHubMessageMocks mocks;
        auto h = IoTHubMessage_CreateFromString("c, 1");
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        ///act
        auto r = IoTHubMessage_GetString(h);

        ///assert
        ASSERT_ARE_EQUAL(char_ptr, "c, 1", r);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubMessage_Destroy(h);
    }

    /*Tests_SRS_IOTHUBMESSAGE_02_017: [IoTHubMessage_GetString shall return NULL if the iotHubMessageHandle does not refer to a IOTHUBMESSAGE of type STRING.] */
    TEST_FUNCTION(IoTHubMessage_GetString_with_BYTEARRAY_fails)
    {
        ///arrange
        CIoTHubMessageMocks mocks;
        auto h = IoTHubMessage_CreateFromByteArray(c, 1);
        mocks.ResetAllCalls();

        ///act
        auto r = IoTHubMessage_GetString(h);

        ///assert
        ASSERT_IS_NULL(r);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubMessage_Destroy(h);
    }

    /* Tests_SRS_IOTHUBMESSAGE_07_012: [if any of the parameters are NULL then IoTHubMessage_SetMessageId shall return a IOTHUB_MESSAGE_INVALID_ARG value.] */
    TEST_FUNCTION(IoTHubMessage_SetMessageId_NULL_handle_Fails)
    {
        ///arrange
        CIoTHubMessageMocks mocks;

        ///act
        IOTHUB_MESSAGE_RESULT result = IoTHubMessage_SetMessageId(NULL, TEST_MESSAGE_ID);

        ///assert
        ASSERT_ARE_EQUAL(IOTHUB_MESSAGE_RESULT, IOTHUB_MESSAGE_INVALID_ARG, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
    }

    /* Tests_SRS_IOTHUBMESSAGE_07_012: [if any of the parameters are NULL then IoTHubMessage_SetMessageId shall return a IOTHUB_MESSAGE_INVALID_ARG value.] */
    TEST_FUNCTION(IoTHubMessage_SetMessageId_NULL_MessageId_Fails)
    {
        ///arrange
        CIoTHubMessageMocks mocks;
        auto h = IoTHubMessage_CreateFromByteArray(c, 1);
        mocks.ResetAllCalls();

        ///act
        IOTHUB_MESSAGE_RESULT result = IoTHubMessage_SetMessageId(h, NULL);

        ///assert
        ASSERT_ARE_EQUAL(IOTHUB_MESSAGE_RESULT, IOTHUB_MESSAGE_INVALID_ARG, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubMessage_Destroy(h);
    }

    /* Tests_SRS_IOTHUBMESSAGE_07_015: [IoTHubMessage_SetMessageId finishes successfully it shall return IOTHUB_MESSAGE_OK.] */
    TEST_FUNCTION(IoTHubMessage_SetMessageId_SUCCEED)
    {
        ///arrange
        CIoTHubMessageMocks mocks;
        auto h = IoTHubMessage_CreateFromByteArray(c, 1);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, mallocAndStrcpy_s(IGNORED_PTR_ARG, TEST_MESSAGE_ID))
            .IgnoreArgument(1);

        ///act
        IOTHUB_MESSAGE_RESULT result = IoTHubMessage_SetMessageId(h, TEST_MESSAGE_ID);

        ///assert
        ASSERT_ARE_EQUAL(IOTHUB_MESSAGE_RESULT, IOTHUB_MESSAGE_OK, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubMessage_Destroy(h);
    }

    /* SRS_IOTHUBMESSAGE_07_013: [If the IOTHUB_MESSAGE_HANDLE messageId is not NULL, then the IOTHUB_MESSAGE_HANDLE messageId will be deallocated.] */
    TEST_FUNCTION(IoTHubMessage_SetMessageId_MessageId_Not_NULL_SUCCEED)
    {
        ///arrange
        CIoTHubMessageMocks mocks;
        auto h = IoTHubMessage_CreateFromByteArray(c, 1);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, mallocAndStrcpy_s(IGNORED_PTR_ARG, TEST_MESSAGE_ID))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, mallocAndStrcpy_s(IGNORED_PTR_ARG, TEST_MESSAGE_ID2))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        ///act
        IOTHUB_MESSAGE_RESULT result = IoTHubMessage_SetMessageId(h, TEST_MESSAGE_ID);
        ASSERT_ARE_EQUAL(IOTHUB_MESSAGE_RESULT, IOTHUB_MESSAGE_OK, result);

        result = IoTHubMessage_SetMessageId(h, TEST_MESSAGE_ID2);

        ///assert
        ASSERT_ARE_EQUAL(IOTHUB_MESSAGE_RESULT, IOTHUB_MESSAGE_OK, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubMessage_Destroy(h);
    }

    /* Tests_SRS_IOTHUBMESSAGE_07_010: [if the iotHubMessageHandle parameter is NULL then IoTHubMessage_GetMessageId shall return a NULL value.] */
    TEST_FUNCTION(IoTHubMessage_GetMessageId_NULL_handle_Fails)
    {
        ///arrange
        CIoTHubMessageMocks mocks;
        mocks.ResetAllCalls();

        ///act
        const char* result = IoTHubMessage_GetMessageId(NULL);

        ///assert
        ASSERT_IS_NULL(result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
    }

    /* Tests_SRS_IOTHUBMESSAGE_07_011: [IoTHubMessage_GetMessageId shall return the messageId as a const char*.] */
    TEST_FUNCTION(IoTHubMessage_GetMessageId_MessageId_Not_Set_Fails)
    {
        ///arrange
        CIoTHubMessageMocks mocks;
        auto handle = IoTHubMessage_CreateFromString("c, 1");
        mocks.ResetAllCalls();

        ///act
        const char* result = IoTHubMessage_GetMessageId(handle);

        ///assert
        ASSERT_IS_NULL(result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubMessage_Destroy(handle);
    }

    /* Tests_SRS_IOTHUBMESSAGE_07_011: [IoTHubMessage_GetMessageId shall return the messageId as a const char*.] */
    TEST_FUNCTION(IoTHubMessage_GetMessageId_SUCCEED)
    {
        ///arrange
        CIoTHubMessageMocks mocks;
        auto handle = IoTHubMessage_CreateFromString("c, 1");
        IOTHUB_MESSAGE_RESULT msgResult = IoTHubMessage_SetMessageId(handle, TEST_MESSAGE_ID);
        ASSERT_ARE_EQUAL(IOTHUB_MESSAGE_RESULT, IOTHUB_MESSAGE_OK, msgResult);
        //
        mocks.ResetAllCalls();

        ///act
        const char* result = IoTHubMessage_GetMessageId(handle);

        ///assert
        ASSERT_ARE_EQUAL(char_ptr, TEST_MESSAGE_ID, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubMessage_Destroy(handle);
    }

    /* Tests_SRS_IOTHUBMESSAGE_07_016: [if the iotHubMessageHandle parameter is NULL then IoTHubMessage_GetCorrelationId shall return a NULL value.] */
    TEST_FUNCTION(IoTHubMessage_GetCorrelationId_NULL_handle_Fails)
    {
        ///arrange
        CIoTHubMessageMocks mocks;
        mocks.ResetAllCalls();

        ///act
        const char* result = IoTHubMessage_GetCorrelationId(NULL);

        ///assert
        ASSERT_IS_NULL(result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
    }

    /* Tests_SRS_IOTHUBMESSAGE_07_016: [if the iotHubMessageHandle parameter is NULL then IoTHubMessage_GetCorrelationId shall return a NULL value.] */
    TEST_FUNCTION(IoTHubMessage_GetCorrelationId_MessageId_Not_Set_Fails)
    {
        ///arrange
        CIoTHubMessageMocks mocks;
        auto handle = IoTHubMessage_CreateFromString("c, 1");
        mocks.ResetAllCalls();

        ///act
        const char* result = IoTHubMessage_GetCorrelationId(handle);

        ///assert
        ASSERT_IS_NULL(result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubMessage_Destroy(handle);
    }

    /* Tests_SRS_IOTHUBMESSAGE_07_017: [IoTHubMessage_GetCorrelationId shall return the correlationId as a const char*.] */
    TEST_FUNCTION(IoTHubMessage_GetCorrelationId_SUCCEED)
    {
        ///arrange
        CIoTHubMessageMocks mocks;
        auto handle = IoTHubMessage_CreateFromString("c, 1");
        IOTHUB_MESSAGE_RESULT msgResult = IoTHubMessage_SetCorrelationId(handle, TEST_MESSAGE_ID);
        ASSERT_ARE_EQUAL(IOTHUB_MESSAGE_RESULT, IOTHUB_MESSAGE_OK, msgResult);
        //
        mocks.ResetAllCalls();

        ///act
        const char* result = IoTHubMessage_GetCorrelationId(handle);

        ///assert
        ASSERT_ARE_EQUAL(char_ptr, TEST_MESSAGE_ID, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubMessage_Destroy(handle);
    }

    /* Tests_SRS_IOTHUBMESSAGE_07_018: [if any of the parameters are NULL then IoTHubMessage_SetCorrelationId shall return a IOTHUB_MESSAGE_INVALID_ARG value.]*/
    TEST_FUNCTION(IoTHubMessage_SetCorrelationId_NULL_CorrelationId_Fails)
    {
        ///arrange
        CIoTHubMessageMocks mocks;
        auto h = IoTHubMessage_CreateFromByteArray(c, 1);
        mocks.ResetAllCalls();

        ///act
        IOTHUB_MESSAGE_RESULT result = IoTHubMessage_SetCorrelationId(h, NULL);

        ///assert
        ASSERT_ARE_EQUAL(IOTHUB_MESSAGE_RESULT, IOTHUB_MESSAGE_INVALID_ARG, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubMessage_Destroy(h);
    }

    /* Tests_SRS_IOTHUBMESSAGE_07_021: [IoTHubMessage_SetCorrelationId finishes successfully it shall return IOTHUB_MESSAGE_OK.] */
    TEST_FUNCTION(IoTHubMessage_SetCorrelationId_SUCCEED)
    {
        ///arrange
        CIoTHubMessageMocks mocks;
        auto h = IoTHubMessage_CreateFromByteArray(c, 1);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, mallocAndStrcpy_s(IGNORED_PTR_ARG, TEST_MESSAGE_ID))
            .IgnoreArgument(1);

        ///act
        IOTHUB_MESSAGE_RESULT result = IoTHubMessage_SetCorrelationId(h, TEST_MESSAGE_ID);

        ///assert
        ASSERT_ARE_EQUAL(IOTHUB_MESSAGE_RESULT, IOTHUB_MESSAGE_OK, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubMessage_Destroy(h);
    }

    /* Tests_SRS_IOTHUBMESSAGE_07_019: [If the IOTHUB_MESSAGE_HANDLE correlationId is not NULL, then the IOTHUB_MESSAGE_HANDLE correlationId will be deallocated.] */
    TEST_FUNCTION(IoTHubMessage_SetCorrelationId_CorrelationId_Not_NULL_SUCCEED)
    {
        ///arrange
        CIoTHubMessageMocks mocks;
        auto h = IoTHubMessage_CreateFromByteArray(c, 1);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, mallocAndStrcpy_s(IGNORED_PTR_ARG, TEST_MESSAGE_ID))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, mallocAndStrcpy_s(IGNORED_PTR_ARG, TEST_MESSAGE_ID2))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        ///act
        IOTHUB_MESSAGE_RESULT result = IoTHubMessage_SetCorrelationId(h, TEST_MESSAGE_ID);
        ASSERT_ARE_EQUAL(IOTHUB_MESSAGE_RESULT, IOTHUB_MESSAGE_OK, result);

        result = IoTHubMessage_SetCorrelationId(h, TEST_MESSAGE_ID2);

        ///assert
        ASSERT_ARE_EQUAL(IOTHUB_MESSAGE_RESULT, IOTHUB_MESSAGE_OK, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubMessage_Destroy(h);
    }

END_TEST_SUITE(iothubmessage_ut)
