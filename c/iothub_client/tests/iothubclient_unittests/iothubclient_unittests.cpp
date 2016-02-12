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

#include "iothub_client.h"
#include "iothub_client_ll.h"
#include "threadapi.h"
#include "lock.h"

extern "C" int gballoc_init(void);
extern "C" void gballoc_deinit(void);
extern "C" void* gballoc_malloc(size_t size);
extern "C" void* gballoc_calloc(size_t nmemb, size_t size);
extern "C" void* gballoc_realloc(void* ptr, size_t size);
extern "C" void gballoc_free(void* ptr);

DEFINE_MICROMOCK_ENUM_TO_STRING(IOTHUB_CLIENT_STATUS, IOTHUB_CLIENT_STATUS_VALUES);

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

DEFINE_MICROMOCK_ENUM_TO_STRING(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_RESULT_VALUES);

static MICROMOCK_MUTEX_HANDLE g_testByTest;
static MICROMOCK_GLOBAL_SEMAPHORE_HANDLE g_dllByDll;

#define TEST_DEVICE_ID "theidofTheDevice"
#define TEST_DEVICE_KEY "theKeyoftheDevice"
#define TEST_IOTHUBNAME "theNameoftheIotHub"
#define TEST_IOTHUBSUFFIX "theSuffixoftheIotHubHostname"
#define TEST_DEVICEMESSAGE_HANDLE (IOTHUB_MESSAGE_HANDLE)0x52
#define TEST_THREAD_HANDLE (THREAD_HANDLE)0x4442
#define TEST_LOCK_HANDLE (LOCK_HANDLE)0x4443
static const char* TEST_CHAR = "TestChar";

static size_t howManyDoWorkCalls = 0;
static size_t doWorkCallCount = 0;
static THREAD_START_FUNC threadFunc;
static void* threadFuncArg;
static const void* provideFAKE(void);
extern "C" const size_t IoTHubClient_ThreadTerminationOffset;

static const IOTHUB_CLIENT_CONFIG TEST_CONFIG =
{
    provideFAKE,            /* IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol;   */
    TEST_DEVICE_ID,         /* const char* deviceId;                        */
    TEST_DEVICE_KEY,        /* const char* deviceKey;                       */
    TEST_IOTHUBNAME,        /* const char* iotHubName;                      */
    TEST_IOTHUBSUFFIX,      /* const char* iotHubSuffix;                    */
};

static size_t currentmalloc_call;
static size_t whenShallmalloc_fail;
static IOTHUB_CLIENT_HANDLE current_iothub_client;

#define TEST_IOTHUB_CLIENT_LL_HANDLE    (IOTHUB_CLIENT_LL_HANDLE)0x4242

TYPED_MOCK_CLASS(CIoTHubClientMocks, CGlobalMock)
{
public:

    /* IoTHubClient_LL mocks */
    MOCK_STATIC_METHOD_2(, IOTHUB_CLIENT_LL_HANDLE, IoTHubClient_LL_CreateFromConnectionString, const char*, connectionString, IOTHUB_CLIENT_TRANSPORT_PROVIDER, protocol);
    MOCK_METHOD_END(IOTHUB_CLIENT_LL_HANDLE, TEST_IOTHUB_CLIENT_LL_HANDLE);
    MOCK_STATIC_METHOD_1(, IOTHUB_CLIENT_LL_HANDLE, IoTHubClient_LL_Create, const IOTHUB_CLIENT_CONFIG*, config);
    MOCK_METHOD_END(IOTHUB_CLIENT_LL_HANDLE, TEST_IOTHUB_CLIENT_LL_HANDLE);
    MOCK_STATIC_METHOD_1(, void, IoTHubClient_LL_Destroy, IOTHUB_CLIENT_LL_HANDLE, iotHubClientHandle);
    MOCK_VOID_METHOD_END();
    MOCK_STATIC_METHOD_4(, IOTHUB_CLIENT_RESULT, IoTHubClient_LL_SendEventAsync, IOTHUB_CLIENT_LL_HANDLE, iotHubClientHandle, IOTHUB_MESSAGE_HANDLE, eventMessageHandle, IOTHUB_CLIENT_EVENT_CONFIRMATION_CALLBACK, eventConfirmationCallback, void*, userContextCallback)
    MOCK_METHOD_END(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK);
    MOCK_STATIC_METHOD_3(, IOTHUB_CLIENT_RESULT, IoTHubClient_LL_SetMessageCallback, IOTHUB_CLIENT_LL_HANDLE, iotHubClientHandle, IOTHUB_CLIENT_MESSAGE_CALLBACK_ASYNC, messageCallback, void*, userContextCallback)
    MOCK_METHOD_END(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK);
    MOCK_STATIC_METHOD_1(, void, IoTHubClient_LL_DoWork, IOTHUB_CLIENT_LL_HANDLE, iotHubClientHandle)
        doWorkCallCount++;
    MOCK_VOID_METHOD_END();
    MOCK_STATIC_METHOD_2(, IOTHUB_CLIENT_RESULT, IoTHubClient_LL_GetSendStatus, IOTHUB_CLIENT_LL_HANDLE, iotHubClientHandle, IOTHUB_CLIENT_STATUS*, iotHubClientStatus)
    MOCK_METHOD_END(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK);
    MOCK_STATIC_METHOD_2(, IOTHUB_CLIENT_RESULT, IoTHubClient_LL_GetLastMessageReceiveTime, IOTHUB_CLIENT_LL_HANDLE, iotHubClientHandle, time_t*, lastMessageReceiveTime)
    MOCK_METHOD_END(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK);

    MOCK_STATIC_METHOD_3(, IOTHUB_CLIENT_RESULT, IoTHubClient_LL_SetOption, IOTHUB_CLIENT_LL_HANDLE, iotHubClientHandle, const char*, optionName, const void*, value)
    MOCK_METHOD_END(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK);

    /* ThreadAPI mocks */
    MOCK_STATIC_METHOD_3(, THREADAPI_RESULT, ThreadAPI_Create, THREAD_HANDLE*, threadHandle, THREAD_START_FUNC, func, void*, arg);
        *threadHandle = TEST_THREAD_HANDLE;
        threadFunc = func;
        threadFuncArg = arg;
    MOCK_METHOD_END(THREADAPI_RESULT, THREADAPI_OK);
    MOCK_STATIC_METHOD_2(, THREADAPI_RESULT, ThreadAPI_Join, THREAD_HANDLE, threadHandle, int*, res);
    MOCK_METHOD_END(THREADAPI_RESULT, THREADAPI_OK);
    MOCK_STATIC_METHOD_1(, void, ThreadAPI_Exit, int, res);
    MOCK_VOID_METHOD_END();
    MOCK_STATIC_METHOD_1(, void, ThreadAPI_Sleep, unsigned int, milliseconds)
        if ((howManyDoWorkCalls > 0) && (howManyDoWorkCalls == doWorkCallCount))
        {
            *(sig_atomic_t*)(((char*)threadFuncArg) + IoTHubClient_ThreadTerminationOffset) = 1; /*tell the thread to stop*/
        }
    MOCK_VOID_METHOD_END();

    /* Lock mocks */
    MOCK_STATIC_METHOD_0(, LOCK_HANDLE, Lock_Init);
    MOCK_METHOD_END(LOCK_HANDLE, TEST_LOCK_HANDLE);
    MOCK_STATIC_METHOD_1(, LOCK_RESULT, Lock, LOCK_HANDLE, handle);
    MOCK_METHOD_END(LOCK_RESULT, LOCK_OK);
    MOCK_STATIC_METHOD_1(, LOCK_RESULT, Unlock, LOCK_HANDLE, handle);
    MOCK_METHOD_END(LOCK_RESULT, LOCK_OK);
    MOCK_STATIC_METHOD_1(, LOCK_RESULT, Lock_Deinit, LOCK_HANDLE, handle);
    MOCK_METHOD_END(LOCK_RESULT, LOCK_OK);

    /* gballoc mocks */
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

    MOCK_STATIC_METHOD_2(, void*, gballoc_realloc, void*, ptr, size_t, size)
     MOCK_METHOD_END(void*, BASEIMPLEMENTATION::gballoc_realloc(ptr, size));

    MOCK_STATIC_METHOD_1(, void, gballoc_free, void*, ptr)
        BASEIMPLEMENTATION::gballoc_free(ptr);
    MOCK_VOID_METHOD_END()

    MOCK_STATIC_METHOD_2(, void, eventConfirmationCallback, IOTHUB_CLIENT_CONFIRMATION_RESULT, result2, void*, userContextCallback)
    MOCK_VOID_METHOD_END()

    MOCK_STATIC_METHOD_2(, IOTHUBMESSAGE_DISPOSITION_RESULT, messageCallback, IOTHUB_MESSAGE_HANDLE, message, void*, userContextCallback)
    MOCK_METHOD_END(IOTHUBMESSAGE_DISPOSITION_RESULT, IOTHUBMESSAGE_ACCEPTED);
};

DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubClientMocks, , IOTHUB_CLIENT_LL_HANDLE, IoTHubClient_LL_CreateFromConnectionString, const char*, connectionString, IOTHUB_CLIENT_TRANSPORT_PROVIDER, protocol);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubClientMocks, , IOTHUB_CLIENT_LL_HANDLE, IoTHubClient_LL_Create, const IOTHUB_CLIENT_CONFIG*, config);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubClientMocks, , void, IoTHubClient_LL_Destroy, IOTHUB_CLIENT_LL_HANDLE, iotHubClientHandle);
DECLARE_GLOBAL_MOCK_METHOD_4(CIoTHubClientMocks, , IOTHUB_CLIENT_RESULT, IoTHubClient_LL_SendEventAsync, IOTHUB_CLIENT_LL_HANDLE, iotHubClientHandle, IOTHUB_MESSAGE_HANDLE, eventMessageHandle, IOTHUB_CLIENT_EVENT_CONFIRMATION_CALLBACK, eventConfirmationCallback, void*, userContextCallback)
DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubClientMocks, , IOTHUB_CLIENT_RESULT, IoTHubClient_LL_SetMessageCallback, IOTHUB_CLIENT_LL_HANDLE, iotHubClientHandle, IOTHUB_CLIENT_MESSAGE_CALLBACK_ASYNC, messageCallback, void*, userContextCallback)
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubClientMocks, , void, IoTHubClient_LL_DoWork, IOTHUB_CLIENT_LL_HANDLE, iotHubClientHandle)
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubClientMocks, , IOTHUB_CLIENT_RESULT, IoTHubClient_LL_GetSendStatus, IOTHUB_CLIENT_LL_HANDLE, iotHubClientHandle, IOTHUB_CLIENT_STATUS*, iotHubClientStatus)
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubClientMocks, , IOTHUB_CLIENT_RESULT, IoTHubClient_LL_GetLastMessageReceiveTime, IOTHUB_CLIENT_LL_HANDLE, iotHubClientHandle, time_t*, lastMessageReceiveTime)
DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubClientMocks, , IOTHUB_CLIENT_RESULT, IoTHubClient_LL_SetOption, IOTHUB_CLIENT_LL_HANDLE, iotHubClientHandle, const char*, optionName, const void*, value)

DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubClientMocks, , THREADAPI_RESULT, ThreadAPI_Create, THREAD_HANDLE*, threadHandle, THREAD_START_FUNC, func, void*, arg);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubClientMocks, , THREADAPI_RESULT, ThreadAPI_Join, THREAD_HANDLE, threadHandle, int*, res);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubClientMocks, , void, ThreadAPI_Exit, int, res);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubClientMocks, , void, ThreadAPI_Sleep, unsigned int, milliseconds);

DECLARE_GLOBAL_MOCK_METHOD_0(CIoTHubClientMocks, , LOCK_HANDLE, Lock_Init);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubClientMocks, , LOCK_RESULT, Lock, LOCK_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubClientMocks, , LOCK_RESULT, Unlock, LOCK_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubClientMocks, , LOCK_RESULT, Lock_Deinit, LOCK_HANDLE, handle);

DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubClientMocks, , void*, gballoc_malloc, size_t, size);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubClientMocks, , void*, gballoc_realloc, void*, ptr, size_t, size);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubClientMocks, , void, gballoc_free, void*, ptr)

DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubClientMocks, , void, eventConfirmationCallback, IOTHUB_CLIENT_CONFIRMATION_RESULT, result2, void*, userContextCallback);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubClientMocks, , IOTHUBMESSAGE_DISPOSITION_RESULT, messageCallback, IOTHUB_MESSAGE_HANDLE, message, void*, userContextCallback);

static const void* provideFAKE(void)
{
    return NULL;
}

BEGIN_TEST_SUITE(iothubclient_unittests)

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
        howManyDoWorkCalls = 0;
        doWorkCallCount = 0;
    }

    TEST_FUNCTION_CLEANUP(TestMethodCleanup)
    {
        if (!MicroMockReleaseMutex(g_testByTest))
        {
            ASSERT_FAIL("failure in test framework at ReleaseMutex");
        }
    }

    /* Tests_SRS_IOTHUBCLIENT_12_004: [IoTHubClient_CreateFromConnectionString shall allocate a new IoTHubClient instance.] */
    /* Tests_SRS_IOTHUBCLIENT_12_005: [IoTHubClient_CreateFromConnectionString shall create a lock object to be used later for serializing IoTHubClient calls] */
    /* Tests_SRS_IOTHUBCLIENT_12_006: [IoTHubClient_CreateFromConnectionString shall instantiate a new IoTHubClient_LL instance by calling IoTHubClient_LL_CreateFromConnectionString and passing the connectionString and protocol] */
    TEST_FUNCTION(IoTHubClient_CreateFromConnectionString_succeeds)
    {
        // arrange
        CIoTHubClientMocks mocks;
        EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG));
        STRICT_EXPECTED_CALL(mocks, Lock_Init());
        STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_CreateFromConnectionString(TEST_CHAR, provideFAKE));

        // act
        IOTHUB_CLIENT_HANDLE iotHubClient = IoTHubClient_CreateFromConnectionString(TEST_CHAR, provideFAKE);

        // assert
        ASSERT_IS_NOT_NULL(iotHubClient);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        IoTHubClient_Destroy(iotHubClient);
    }

    /* Tests_SRS_IOTHUBCLIENT_12_003: [IoTHubClient_CreateFromConnectionString shall verify the input parameters and if any of them NULL then return NULL] */
    TEST_FUNCTION(IoTHubClient_CreateFromConnectionString_if_input_parameter_connectionString_is_NULL_then_return_NULL)
    {
        // arrange
        CIoTHubClientMocks mocks;

        // act
        IOTHUB_CLIENT_HANDLE iotHubClient = IoTHubClient_CreateFromConnectionString(NULL, provideFAKE);

        // assert
        ASSERT_IS_NULL(iotHubClient);
        mocks.AssertActualAndExpectedCalls();
    }

    /* Tests_SRS_IOTHUBCLIENT_12_003: [IoTHubClient_CreateFromConnectionString shall verify the input parameters and if any of them NULL then return NULL] */
    TEST_FUNCTION(IoTHubClient_CreateFromConnectionString_if_input_parameter_protocol_is_NULL_then_return_NULL)
    {
        // arrange
        CIoTHubClientMocks mocks;

        // act
        IOTHUB_CLIENT_HANDLE iotHubClient = IoTHubClient_CreateFromConnectionString(TEST_CHAR, NULL);

        // assert
        ASSERT_IS_NULL(iotHubClient);
        mocks.AssertActualAndExpectedCalls();
    }

    /* Tests_SRS_IOTHUBCLIENT_12_011: [If the allocation failed, IoTHubClient_CreateFromConnectionString returns NULL] */
    TEST_FUNCTION(IoTHubClient_CreateFromConnectionString_if_malloc_fails_return_NULL)
    {
        // arrange
        CIoTHubClientMocks mocks;

        whenShallmalloc_fail = 1;
        EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG));

        // act
        IOTHUB_CLIENT_HANDLE iotHubClient = IoTHubClient_CreateFromConnectionString(TEST_CHAR, provideFAKE);

        // assert
        ASSERT_IS_NULL(iotHubClient);
        mocks.AssertActualAndExpectedCalls();
    }

    /* Tests_SRS_IOTHUBCLIENT_12_009: [If lock creation failed, IoTHubClient_CreateFromConnectionString shall do clean up and return NULL] */
    TEST_FUNCTION(IoTHubClient_CreateFromConnectionString_if_Lock_fails_then_do_clean_up_and_return_NULL)
    {
        // arrange
        CIoTHubClientMocks mocks;
        EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG));
        STRICT_EXPECTED_CALL(mocks, Lock_Init())
            .SetReturn((LOCK_HANDLE)NULL);
        EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG));

        // act
        IOTHUB_CLIENT_HANDLE iotHubClient = IoTHubClient_CreateFromConnectionString(TEST_CHAR, provideFAKE);

        // assert
        ASSERT_IS_NULL(iotHubClient);
        mocks.AssertActualAndExpectedCalls();
    }

    /* Tests_SRS_IOTHUBCLIENT_12_010: [If IoTHubClient_LL_CreateFromConnectionString fails then IoTHubClient_CreateFromConnectionString shall do clean - up and return NULL] */
    TEST_FUNCTION(IoTHubClient_CreateFromConnectionString_if_IoTHubClient_LL_CreateFromConnectionString_fails_then_IoTHubClient_CreateFromConnectionString_do_clean_up_and_fails)
    {
        // arrange
        CIoTHubClientMocks mocks;
        EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG));
        STRICT_EXPECTED_CALL(mocks, Lock_Init());
        STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_CreateFromConnectionString(TEST_CHAR, provideFAKE))
            .SetReturn((IOTHUB_CLIENT_LL_HANDLE)NULL);
        STRICT_EXPECTED_CALL(mocks, Lock_Deinit(TEST_LOCK_HANDLE));
        EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG));

        // act
        IOTHUB_CLIENT_HANDLE iotHubClient = IoTHubClient_CreateFromConnectionString(TEST_CHAR, provideFAKE);

        // assert
        ASSERT_IS_NULL(iotHubClient);
        mocks.AssertActualAndExpectedCalls();
    }

    /* IoTHubClient_Create */

    /* Tests_SRS_IOTHUBCLIENT_01_001: [IoTHubClient_Create shall allocate a new IoTHubClient instance and return a non-NULL handle to it.] */
    /* Tests_SRS_IOTHUBCLIENT_01_002: [IoTHubClient_Create shall instantiate a new IoTHubClient_LL instance by calling IoTHubClient_LL_Create and passing the config argument.] */
    /* Tests_SRS_IOTHUBCLIENT_01_029: [IoTHubClient_Create shall create a lock object to be used later for serializing IoTHubClient calls.] */
    TEST_FUNCTION(IoTHubClient_Create_with_valid_arguments_when_all_underlying_calls_are_OK_succeeds)
    {
        // arrange
        CIoTHubClientMocks mocks;
        EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG));
        STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_Create(&TEST_CONFIG));
        STRICT_EXPECTED_CALL(mocks, Lock_Init());

        // act
        IOTHUB_CLIENT_HANDLE iotHubClient = IoTHubClient_Create(&TEST_CONFIG);

        // assert
        ASSERT_IS_NOT_NULL(iotHubClient);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        IoTHubClient_Destroy(iotHubClient);
    }

    /* Tests_SRS_IOTHUBCLIENT_01_003: [If IoTHubClient_LL_Create fails, then IoTHubClient_Create shall return NULL.] */
    /* Tests_SRS_IOTHUBCLIENT_01_031: [If IoTHubClient_Create fails, all resources allocated by it shall be freed.] */
    TEST_FUNCTION(When_IoTHubClient_LL_Create_fails_then_IoTHubClient_Create_fails)
    {
        // arrange
        CIoTHubClientMocks mocks;
        EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG));
        STRICT_EXPECTED_CALL(mocks, Lock_Init());
        STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_Create(&TEST_CONFIG))
            .SetReturn((IOTHUB_CLIENT_LL_HANDLE)NULL);
        STRICT_EXPECTED_CALL(mocks, Lock_Deinit(TEST_LOCK_HANDLE));
        EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG));

        // act
        IOTHUB_CLIENT_HANDLE iotHubClient = IoTHubClient_Create(&TEST_CONFIG);

        // assert
        ASSERT_IS_NULL(iotHubClient);
        mocks.AssertActualAndExpectedCalls();
    }

    /* Tests_SRS_IOTHUBCLIENT_01_030: [If creating the lock fails, then IoTHubClient_Create shall return NULL.] */
    /* Tests_SRS_IOTHUBCLIENT_01_031: [If IoTHubClient_Create fails, all resources allocated by it shall be freed.] */
    TEST_FUNCTION(When_Creating_The_Lock_Fails_then_IoTHubClient_Create_fails)
    {
        // arrange
        CIoTHubClientMocks mocks;
        EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG));
        STRICT_EXPECTED_CALL(mocks, Lock_Init())
            .SetReturn((LOCK_HANDLE)NULL);
        EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG));

        // act
        IOTHUB_CLIENT_HANDLE iotHubClient = IoTHubClient_Create(&TEST_CONFIG);

        // assert
        ASSERT_IS_NULL(iotHubClient);
        mocks.AssertActualAndExpectedCalls();
    }

    /* Tests_SRS_IOTHUBCLIENT_01_004: [If allocating memory for the new IoTHubClient instance fails, then IoTHubClient_Create shall return NULL.] */
    TEST_FUNCTION(When_malloc_fails_then_IoTHubClient_Create_fails)
    {
        // arrange
        CIoTHubClientMocks mocks;

        whenShallmalloc_fail = 1;
        EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG));

        // act
        IOTHUB_CLIENT_HANDLE iotHubClient = IoTHubClient_Create(&TEST_CONFIG);

        // assert
        ASSERT_IS_NULL(iotHubClient);
        mocks.AssertActualAndExpectedCalls();
    }

    /* IoTHubClient_Destroy */

    /* Tests_SRS_IOTHUBCLIENT_01_005: [IoTHubClient_Destroy shall free all resources associated with the iotHubClientHandle instance.] */
    /* Tests_SRS_IOTHUBCLIENT_01_006: [That includes destroying the IoTHubClient_LL instance by calling IoTHubClient_LL_Destroy.] */
    /* Tests_SRS_IOTHUBCLIENT_01_032: [The lock allocated in IoTHubClient_Create shall be also freed.] */
    TEST_FUNCTION(IoTHubClient_Destroy_frees_underlying_LL_client)
    {
        // arrange
        CIoTHubClientMocks mocks;
        IOTHUB_CLIENT_HANDLE iotHubClient = IoTHubClient_Create(&TEST_CONFIG);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_Destroy(TEST_IOTHUB_CLIENT_LL_HANDLE));
        STRICT_EXPECTED_CALL(mocks, Lock_Deinit(TEST_LOCK_HANDLE));
        EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG));

        // act
        IoTHubClient_Destroy(iotHubClient);

        // assert
        // uMock checks the calls
    }

    /* Tests_SRS_IOTHUBCLIENT_01_008: [IoTHubClient_Destroy shall do nothing if parameter iotHubClientHandle is NULL.] */
    TEST_FUNCTION(IoTHubClient_Destroy_With_NULL_does_nothing)
    {
        // arrange
        CIoTHubClientMocks mocks;

        // act
        IoTHubClient_Destroy(NULL);

        // assert
        // uMock checks the calls
    }

    /* Tests_SRS_IOTHUBCLIENT_01_007: [The thread created as part of executing IoTHubClient_SendEventAsync or IoTHubClient_SetMessageCallback shall be joined.] */
    TEST_FUNCTION(IoTHubClient_Destroy_After_A_SendEvent_Joins_The_Thread)
    {
        // arrange
        CIoTHubClientMocks mocks;
        IOTHUB_CLIENT_HANDLE iotHubClient = IoTHubClient_Create(&TEST_CONFIG);
        (void)IoTHubClient_SendEventAsync(iotHubClient, TEST_DEVICEMESSAGE_HANDLE, eventConfirmationCallback, (void*)0x42);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, ThreadAPI_Join(TEST_THREAD_HANDLE, IGNORED_PTR_ARG))
            .IgnoreArgument(2);
        STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_Destroy(TEST_IOTHUB_CLIENT_LL_HANDLE));
        STRICT_EXPECTED_CALL(mocks, Lock_Deinit(TEST_LOCK_HANDLE));
        EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG));

        // act
        IoTHubClient_Destroy(iotHubClient);

        // assert
        // uMock checks the calls
    }

    /* Tests_SRS_IOTHUBCLIENT_01_007: [The thread created as part of executing IoTHubClient_SendEventAsync or IoTHubClient_SetMessageCallback shall be joined.] */
    TEST_FUNCTION(IoTHubClient_Destroy_After_A_SetMessageCallback_Joins_The_Thread)
    {
        // arrange
        CIoTHubClientMocks mocks;
        IOTHUB_CLIENT_HANDLE iotHubClient = IoTHubClient_Create(&TEST_CONFIG);
        (void)IoTHubClient_SetMessageCallback(iotHubClient, messageCallback, (void*)0x42);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, ThreadAPI_Join(TEST_THREAD_HANDLE, IGNORED_PTR_ARG))
            .IgnoreArgument(2);
        STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_Destroy(TEST_IOTHUB_CLIENT_LL_HANDLE));
        STRICT_EXPECTED_CALL(mocks, Lock_Deinit(TEST_LOCK_HANDLE));
        EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG));

        // act
        IoTHubClient_Destroy(iotHubClient);

        // assert
        // uMock checks the calls
    }

    /* Tests_SRS_IOTHUBCLIENT_01_007: [The thread created as part of executing IoTHubClient_SendEventAsync or IoTHubClient_SetMessageCallback shall be joined.] */
    TEST_FUNCTION(When_Thread_Join_Fails_The_Rest_Of_Resources_Are_Still_Freed)
    {
        // arrange
        CIoTHubClientMocks mocks;
        IOTHUB_CLIENT_HANDLE iotHubClient = IoTHubClient_Create(&TEST_CONFIG);
        (void)IoTHubClient_SetMessageCallback(iotHubClient, messageCallback, (void*)0x42);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, ThreadAPI_Join(TEST_THREAD_HANDLE, IGNORED_PTR_ARG))
            .IgnoreArgument(2)
            .SetReturn(THREADAPI_ERROR);
        STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_Destroy(TEST_IOTHUB_CLIENT_LL_HANDLE));
        STRICT_EXPECTED_CALL(mocks, Lock_Deinit(TEST_LOCK_HANDLE));
        EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG));

        // act
        IoTHubClient_Destroy(iotHubClient);

        // assert
        // uMock checks the calls
    }

    /* IoTHubClient_SendEventAsync */

    /* Tests_SRS_IOTHUBCLIENT_01_009: [IoTHubClient_SendEventAsync shall start the worker thread if it was not previously started.] */
    /* Tests_SRS_IOTHUBCLIENT_01_012: [IoTHubClient_SendEventAsync shall call IoTHubClient_LL_SendEventAsync, while passing the IoTHubClient_LL handle created by IoTHubClient_Create and the parameters eventMessageHandle, eventConfirmationCallback and userContextCallback.] */
    /* Tests_SRS_IOTHUBCLIENT_01_025: [IoTHubClient_SendEventAsync shall be made thread-safe by using the lock created in IoTHubClient_Create.] */
    TEST_FUNCTION(On_The_First_SendEventAsync_The_Worker_Thread_Is_Created_And_the_Underlayer_Is_Called_To_Send_Event)
    {
        // arrange
        CIoTHubClientMocks mocks;
        IOTHUB_CLIENT_HANDLE iotHubClient = IoTHubClient_Create(&TEST_CONFIG);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
        EXPECTED_CALL(mocks, ThreadAPI_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_SendEventAsync(TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_DEVICEMESSAGE_HANDLE, eventConfirmationCallback, (void*)0x42));
        STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));

        // act
        IOTHUB_CLIENT_RESULT result = IoTHubClient_SendEventAsync(iotHubClient, TEST_DEVICEMESSAGE_HANDLE, eventConfirmationCallback, (void*)0x42);

        // assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        IoTHubClient_Destroy(iotHubClient);
    }

    /* Tests_SRS_IOTHUBCLIENT_01_010: [If starting the thread fails, IoTHubClient_SendEventAsync shall return IOTHUB_CLIENT_ERROR.] */
    TEST_FUNCTION(When_Starting_The_Worker_Thread_Fails_Then_IoTHubClient_SendEventAsync_Fails)
    {
        // arrange
        CIoTHubClientMocks mocks;
        IOTHUB_CLIENT_HANDLE iotHubClient = IoTHubClient_Create(&TEST_CONFIG);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
        EXPECTED_CALL(mocks, ThreadAPI_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .SetReturn(THREADAPI_ERROR);
        STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));

        // act
        IOTHUB_CLIENT_RESULT result = IoTHubClient_SendEventAsync(iotHubClient, TEST_DEVICEMESSAGE_HANDLE, eventConfirmationCallback, (void*)0x42);

        // assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, result, IOTHUB_CLIENT_ERROR);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        IoTHubClient_Destroy(iotHubClient);
    }

    /* Tests_SRS_IOTHUBCLIENT_01_011: [If iotHubClientHandle is NULL, IoTHubClient_SendEventAsync shall return IOTHUB_CLIENT_INVALID_ARG.] */
    TEST_FUNCTION(IoTHubClient_SendEventAsync_With_NULL_Handle_Fails)
    {
        // arrange
        CIoTHubClientMocks mocks;

        // act
        IOTHUB_CLIENT_RESULT result = IoTHubClient_SendEventAsync(NULL, TEST_DEVICEMESSAGE_HANDLE, eventConfirmationCallback, (void*)0x42);

        // assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, result, IOTHUB_CLIENT_INVALID_ARG);
    }

    /* Tests_SRS_IOTHUBCLIENT_01_013: [When IoTHubClient_LL_SendEventAsync is called, IoTHubClient_SendEventAsync shall return the result of IoTHubClient_LL_SendEventAsync.] */
    TEST_FUNCTION(IoTHubClient_SendEventAsync_Returns_The_Error_Code_From_The_Underlayer)
    {
        // arrange
        CIoTHubClientMocks mocks;
        IOTHUB_CLIENT_HANDLE iotHubClient = IoTHubClient_Create(&TEST_CONFIG);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
        EXPECTED_CALL(mocks, ThreadAPI_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_SendEventAsync(TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_DEVICEMESSAGE_HANDLE, eventConfirmationCallback, (void*)0x42))
            .SetReturn(IOTHUB_CLIENT_INVALID_SIZE);
        STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));

        // act
        IOTHUB_CLIENT_RESULT result = IoTHubClient_SendEventAsync(iotHubClient, TEST_DEVICEMESSAGE_HANDLE, eventConfirmationCallback, (void*)0x42);

        // assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, result, IOTHUB_CLIENT_INVALID_SIZE);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        IoTHubClient_Destroy(iotHubClient);
    }

    /* SRS_IOTHUBCLIENT_01_009: [IoTHubClient_SendEventAsync shall start the worker thread if it was not previously started.] */
    TEST_FUNCTION(When_The_Worker_Thread_Was_Started_Already_Due_To_SendEventAsync_Thread_Is_Not_Started_Again_On_A_New_SendEventAsync)
    {
        // arrange
        CIoTHubClientMocks mocks;
        IOTHUB_CLIENT_HANDLE iotHubClient = IoTHubClient_Create(&TEST_CONFIG);
        (void)IoTHubClient_SendEventAsync(iotHubClient, TEST_DEVICEMESSAGE_HANDLE, eventConfirmationCallback, (void*)0x42);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
        STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_SendEventAsync(TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_DEVICEMESSAGE_HANDLE, eventConfirmationCallback, (void*)0x42));
        STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));

        // act
        IOTHUB_CLIENT_RESULT result = IoTHubClient_SendEventAsync(iotHubClient, TEST_DEVICEMESSAGE_HANDLE, eventConfirmationCallback, (void*)0x42);

        // assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        IoTHubClient_Destroy(iotHubClient);
    }

    /* SRS_IOTHUBCLIENT_01_009: [IoTHubClient_SendEventAsync shall start the worker thread if it was not previously started.] */
    TEST_FUNCTION(When_The_Worker_Thread_Was_Started_Already_Due_To_SetMessageCallback_Thread_Is_Not_Started_Again_On_A_New_SendEventAsync)
    {
        // arrange
        CIoTHubClientMocks mocks;
        IOTHUB_CLIENT_HANDLE iotHubClient = IoTHubClient_Create(&TEST_CONFIG);
        (void)IoTHubClient_SetMessageCallback(iotHubClient, messageCallback, (void*)0x42);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
        STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_SendEventAsync(TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_DEVICEMESSAGE_HANDLE, eventConfirmationCallback, (void*)0x42));
        STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));

        // act
        IOTHUB_CLIENT_RESULT result = IoTHubClient_SendEventAsync(iotHubClient, TEST_DEVICEMESSAGE_HANDLE, eventConfirmationCallback, (void*)0x42);

        // assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        IoTHubClient_Destroy(iotHubClient);
    }

    /* Tests_SRS_IOTHUBCLIENT_01_026: [If acquiring the lock fails, IoTHubClient_SendEventAsync shall return IOTHUB_CLIENT_ERROR.] */
    TEST_FUNCTION(When_Acquiring_The_lock_fails_then_IoTHubClient_SendEventAsync_fails)
    {
        // arrange
        CIoTHubClientMocks mocks;
        IOTHUB_CLIENT_HANDLE iotHubClient = IoTHubClient_Create(&TEST_CONFIG);
        (void)IoTHubClient_SetMessageCallback(iotHubClient, messageCallback, (void*)0x42);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE))
            .SetReturn(LOCK_ERROR);

        // act
        IOTHUB_CLIENT_RESULT result = IoTHubClient_SendEventAsync(iotHubClient, TEST_DEVICEMESSAGE_HANDLE, eventConfirmationCallback, (void*)0x42);

        // assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, result, IOTHUB_CLIENT_ERROR);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        IoTHubClient_Destroy(iotHubClient);
    }

    TEST_FUNCTION(When_Unlock_fails_SendEventAsync_still_succeeds)
    {
        // arrange
        CIoTHubClientMocks mocks;
        IOTHUB_CLIENT_HANDLE iotHubClient = IoTHubClient_Create(&TEST_CONFIG);
        (void)IoTHubClient_SetMessageCallback(iotHubClient, messageCallback, (void*)0x42);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
        STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_SendEventAsync(TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_DEVICEMESSAGE_HANDLE, eventConfirmationCallback, (void*)0x42));
        STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE))
            .SetReturn(LOCK_ERROR);

        // act
        IOTHUB_CLIENT_RESULT result = IoTHubClient_SendEventAsync(iotHubClient, TEST_DEVICEMESSAGE_HANDLE, eventConfirmationCallback, (void*)0x42);

        // assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        IoTHubClient_Destroy(iotHubClient);
    }

    /* IoTHubClient_SetMessageCallback */

    /* Tests_SRS_IOTHUBCLIENT_01_014: [IoTHubClient_SetMessageCallback shall start the worker thread if it was not previously started.] */
    /* Tests_SRS_IOTHUBCLIENT_01_017: [IoTHubClient_SetMessageCallback shall call IoTHubClient_LL_SetMessageCallback, while passing the IoTHubClient_LL handle created by IoTHubClient_Create and the parameters messageCallback and userContextCallback.] */
    /* Tests_SRS_IOTHUBCLIENT_01_027: [IoTHubClient_SetMessageCallback shall be made thread-safe by using the lock created in IoTHubClient_Create.] */
    TEST_FUNCTION(IoTHubClient_SetMessageCallback_starts_the_worker_thread_and_calls_the_underlayer)
    {
        // arrange
        CIoTHubClientMocks mocks;
        IOTHUB_CLIENT_HANDLE iotHubClient = IoTHubClient_Create(&TEST_CONFIG);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
        EXPECTED_CALL(mocks, ThreadAPI_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_SetMessageCallback(TEST_IOTHUB_CLIENT_LL_HANDLE, messageCallback, (void*)0x42));
        STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));

        // act
        IOTHUB_CLIENT_RESULT result = IoTHubClient_SetMessageCallback(iotHubClient, messageCallback, (void*)0x42);

        // assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        IoTHubClient_Destroy(iotHubClient);
    }

    /* Tests_SRS_IOTHUBCLIENT_01_015: [If starting the thread fails, IoTHubClient_SetMessageCallback shall return IOTHUB_CLIENT_ERROR.] */
    TEST_FUNCTION(When_Thread_API_Create_fails_then_IoTHubClient_SetMessageCallback_fails)
    {
        // arrange
        CIoTHubClientMocks mocks;
        IOTHUB_CLIENT_HANDLE iotHubClient = IoTHubClient_Create(&TEST_CONFIG);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
        EXPECTED_CALL(mocks, ThreadAPI_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .SetReturn(THREADAPI_ERROR);
        STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));

        // act
        IOTHUB_CLIENT_RESULT result = IoTHubClient_SetMessageCallback(iotHubClient, messageCallback, (void*)0x42);

        // assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, result, IOTHUB_CLIENT_ERROR);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        IoTHubClient_Destroy(iotHubClient);
    }

    /* Tests_SRS_IOTHUBCLIENT_01_016: [If iotHubClientHandle is NULL, IoTHubClient_SetMessageCallback shall return IOTHUB_CLIENT_INVALID_ARG.] */
    TEST_FUNCTION(IoTHubClient_SetMessageCallback_with_NULL_handle_fails)
    {
        // arrange
        CIoTHubClientMocks mocks;

        // act
        IOTHUB_CLIENT_RESULT result = IoTHubClient_SetMessageCallback(NULL, messageCallback, (void*)0x42);

        // assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, result, IOTHUB_CLIENT_INVALID_ARG);
    }

    /* Tests_SRS_IOTHUBCLIENT_01_018: [When IoTHubClient_LL_SetMessageCallback is called, IoTHubClient_SetMessageCallback shall return the result of IoTHubClient_LL_SetMessageCallback.] */
    TEST_FUNCTION(IoTHubClient_SetMessageCallback_returns_the_result_of_calling_IoTHubClient_LL_SetMessageCallback)
    {
        // arrange
        CIoTHubClientMocks mocks;
        IOTHUB_CLIENT_HANDLE iotHubClient = IoTHubClient_Create(&TEST_CONFIG);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
        EXPECTED_CALL(mocks, ThreadAPI_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_SetMessageCallback(TEST_IOTHUB_CLIENT_LL_HANDLE, messageCallback, (void*)0x42))
            .SetReturn(IOTHUB_CLIENT_INVALID_SIZE);
        STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));

        // act
        IOTHUB_CLIENT_RESULT result = IoTHubClient_SetMessageCallback(iotHubClient, messageCallback, (void*)0x42);

        // assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, result, IOTHUB_CLIENT_INVALID_SIZE);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        IoTHubClient_Destroy(iotHubClient);
    }

    /* Tests_SRS_IOTHUBCLIENT_01_014: [IoTHubClient_SetMessageCallback shall start the worker thread if it was not previously started.] */
    TEST_FUNCTION(When_the_worker_thread_was_started_by_a_SendEventAsync_A_Call_To_SetMessageCallback_Does_NOt_Start_A_New_Thread)
    {
        // arrange
        CIoTHubClientMocks mocks;
        IOTHUB_CLIENT_HANDLE iotHubClient = IoTHubClient_Create(&TEST_CONFIG);
        (void)IoTHubClient_SendEventAsync(iotHubClient, TEST_DEVICEMESSAGE_HANDLE, eventConfirmationCallback, (void*)0x42);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
        STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_SetMessageCallback(TEST_IOTHUB_CLIENT_LL_HANDLE, messageCallback, (void*)0x42));
        STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));

        // act
        IOTHUB_CLIENT_RESULT result = IoTHubClient_SetMessageCallback(iotHubClient, messageCallback, (void*)0x42);

        // assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        IoTHubClient_Destroy(iotHubClient);
    }

    /* Tests_SRS_IOTHUBCLIENT_01_014: [IoTHubClient_SetMessageCallback shall start the worker thread if it was not previously started.] */
    TEST_FUNCTION(When_the_worker_thread_was_started_by_a_previous_SetMessageCallback_A_Call_To_SetMessageCallback_Does_Not_Start_A_New_Thread)
    {
        // arrange
        CIoTHubClientMocks mocks;
        IOTHUB_CLIENT_HANDLE iotHubClient = IoTHubClient_Create(&TEST_CONFIG);
        (void)IoTHubClient_SetMessageCallback(iotHubClient, messageCallback, (void*)0x42);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
        STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_SetMessageCallback(TEST_IOTHUB_CLIENT_LL_HANDLE, messageCallback, (void*)0x42));
        STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));

        // act
        IOTHUB_CLIENT_RESULT result = IoTHubClient_SetMessageCallback(iotHubClient, messageCallback, (void*)0x42);

        // assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        IoTHubClient_Destroy(iotHubClient);
    }

    /* Tests_SRS_IOTHUBCLIENT_01_028: [If acquiring the lock fails, IoTHubClient_SetMessageCallback shall return IOTHUB_CLIENT_ERROR.] */
    TEST_FUNCTION(When_Acquiring_The_Lock_Fails_Then_IoTHubClient_SetMessageCallback_Fails)
    {
        // arrange
        CIoTHubClientMocks mocks;
        IOTHUB_CLIENT_HANDLE iotHubClient = IoTHubClient_Create(&TEST_CONFIG);
        (void)IoTHubClient_SetMessageCallback(iotHubClient, messageCallback, (void*)0x42);;
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE))
            .SetReturn(LOCK_ERROR);

        // act
        IOTHUB_CLIENT_RESULT result = IoTHubClient_SetMessageCallback(iotHubClient, messageCallback, (void*)0x42);

        // assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, result, IOTHUB_CLIENT_ERROR);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        IoTHubClient_Destroy(iotHubClient);
    }

    /* IoTHubClient_GetLastMessageReceiveTime */

    /* Tests_SRS_IOTHUBCLIENT_01_019: [IoTHubClient_GetLastMessageReceiveTime shall call IoTHubClient_LL_GetLastMessageReceiveTime, while passing the IoTHubClient_LL handle created by IoTHubClient_Create and the parameter lastMessageReceiveTime.] */
    /* Tests_SRS_IOTHUBCLIENT_01_035: [IoTHubClient_GetLastMessageReceiveTime shall be made thread-safe by using the lock created in IoTHubClient_Create.] */
    TEST_FUNCTION(IoTHubClient_GetLastMessageReceiveTime_Calls_the_Underlayer)
    {
        // arrange
        CIoTHubClientMocks mocks;
        IOTHUB_CLIENT_HANDLE iotHubClient = IoTHubClient_Create(&TEST_CONFIG);
        mocks.ResetAllCalls();

        time_t lastMessageReceiveTime;
        time_t currentTime;
        time(&currentTime);
        STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
        STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_GetLastMessageReceiveTime(TEST_IOTHUB_CLIENT_LL_HANDLE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &currentTime, sizeof(currentTime));
        STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));

        // act
        IOTHUB_CLIENT_RESULT result = IoTHubClient_GetLastMessageReceiveTime(iotHubClient, &lastMessageReceiveTime);

        // assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK, result);
        ASSERT_ARE_EQUAL(double, 0.0, difftime(currentTime, lastMessageReceiveTime));
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        IoTHubClient_Destroy(iotHubClient);
    }

    /* Tests_SRS_IOTHUBCLIENT_01_020: [If iotHubClientHandle is NULL, IoTHubClient_GetLastMessageReceiveTime shall return IOTHUB_CLIENT_INVALID_ARG.] */
    TEST_FUNCTION(IoTHubClient_GetLastMessageReceiveTime_with_NULL_handle_fails)
    {
        // arrange
        CIoTHubClientMocks mocks;
        time_t lastMessageReceiveTime;

        // act
        IOTHUB_CLIENT_RESULT result = IoTHubClient_GetLastMessageReceiveTime(NULL, &lastMessageReceiveTime);

        // assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, result, IOTHUB_CLIENT_INVALID_ARG);
    }

    /* Tests_SRS_IOTHUBCLIENT_01_021: [Otherwise, IoTHubClient_GetLastMessageReceiveTime shall return the result of IoTHubClient_LL_GetLastMessageReceiveTime.] */
    TEST_FUNCTION(IoTHubClient_GetLastMessageReceiveTime_Returns_The_Result_Of_the_underlayer)
    {
        // arrange
        CIoTHubClientMocks mocks;
        IOTHUB_CLIENT_HANDLE iotHubClient = IoTHubClient_Create(&TEST_CONFIG);
        mocks.ResetAllCalls();

        time_t lastMessageReceiveTime;
        time_t currentTime;
        time(&currentTime);
        STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
        STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_GetLastMessageReceiveTime(TEST_IOTHUB_CLIENT_LL_HANDLE, IGNORED_PTR_ARG))
            .IgnoreArgument(2)
            .SetReturn(IOTHUB_CLIENT_INVALID_SIZE);
        STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));

        // act
        IOTHUB_CLIENT_RESULT result = IoTHubClient_GetLastMessageReceiveTime(iotHubClient, &lastMessageReceiveTime);

        // assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, result, IOTHUB_CLIENT_INVALID_SIZE);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        IoTHubClient_Destroy(iotHubClient);
    }

    /* Tests_SRS_IOTHUBCLIENT_01_036: [If acquiring the lock fails, IoTHubClient_GetLastMessageReceiveTime shall return IOTHUB_CLIENT_ERROR.] */
    TEST_FUNCTION(When_Accquiring_The_Lock_Fails_Then_IoTHubClient_GetLastMessageReceiveTime_fails)
    {
        // arrange
        CIoTHubClientMocks mocks;
        IOTHUB_CLIENT_HANDLE iotHubClient = IoTHubClient_Create(&TEST_CONFIG);
        mocks.ResetAllCalls();

        time_t lastMessageReceiveTime;
        STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE))
            .SetReturn(LOCK_ERROR);

        // act
        IOTHUB_CLIENT_RESULT result = IoTHubClient_GetLastMessageReceiveTime(iotHubClient, &lastMessageReceiveTime);

        // assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, result, IOTHUB_CLIENT_ERROR);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        IoTHubClient_Destroy(iotHubClient);
    }

    TEST_FUNCTION(When_unlock_fails_IoTHubClient_GetLastMessageReceiveTime_still_succeeds)
    {
        // arrange
        CIoTHubClientMocks mocks;
        IOTHUB_CLIENT_HANDLE iotHubClient = IoTHubClient_Create(&TEST_CONFIG);
        mocks.ResetAllCalls();

        time_t lastMessageReceiveTime;
        time_t currentTime;
        time(&currentTime);
        STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
        STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_GetLastMessageReceiveTime(TEST_IOTHUB_CLIENT_LL_HANDLE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &currentTime, sizeof(currentTime));
        STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE))
            .SetReturn(LOCK_ERROR);

        // act
        IOTHUB_CLIENT_RESULT result = IoTHubClient_GetLastMessageReceiveTime(iotHubClient, &lastMessageReceiveTime);

        // assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        IoTHubClient_Destroy(iotHubClient);
    }

    /* IoTHubClient_GetSendStatus */

    /* Tests_SRS_IOTHUBCLIENT_01_022: [IoTHubClient_GetSendStatus shall call IoTHubClient_LL_GetSendStatus, while passing the IoTHubClient_LL handle created by IoTHubClient_Create and the parameter iotHubClientStatus.] */
    /* Tests_SRS_IOTHUBCLIENT_01_033: [IoTHubClient_GetSendStatus shall be made thread-safe by using the lock created in IoTHubClient_Create.] */
    TEST_FUNCTION(IoTHubClient_GetSendStatus_Calls_The_Underslayer_With_Lock_On)
    {
        // arrange
        CIoTHubClientMocks mocks;
        IOTHUB_CLIENT_HANDLE iotHubClient = IoTHubClient_Create(&TEST_CONFIG);
        mocks.ResetAllCalls();

        IOTHUB_CLIENT_STATUS injectedSendStatus = IOTHUB_CLIENT_SEND_STATUS_BUSY;
        STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
        STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_GetSendStatus(TEST_IOTHUB_CLIENT_LL_HANDLE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &injectedSendStatus, sizeof(injectedSendStatus));
        STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));

        // act
        IOTHUB_CLIENT_STATUS sendStatus;
        IOTHUB_CLIENT_RESULT result = IoTHubClient_GetSendStatus(iotHubClient, &sendStatus);

        // assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_STATUS, sendStatus, IOTHUB_CLIENT_SEND_STATUS_BUSY);
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        IoTHubClient_Destroy(iotHubClient);
    }

    /* Tests_SRS_IOTHUBCLIENT_01_023: [If iotHubClientHandle is NULL, IoTHubClient_ GetSendStatus shall return IOTHUB_CLIENT_INVALID_ARG.] */
    TEST_FUNCTION(IoTHubClient_GetSendStatus_With_NULL_handle_fails)
    {
        // arrange
        CIoTHubClientMocks mocks;

        // act
        IOTHUB_CLIENT_STATUS sendStatus;
        IOTHUB_CLIENT_RESULT result = IoTHubClient_GetSendStatus(NULL, &sendStatus);

        // assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, result, IOTHUB_CLIENT_INVALID_ARG);
        mocks.AssertActualAndExpectedCalls();
    }

    /* Tests_SRS_IOTHUBCLIENT_01_024: [Otherwise, IoTHubClient_GetSendStatus shall return the result of IoTHubClient_LL_GetSendStatus.] */
    TEST_FUNCTION(IoTHubClient_GetSendStatus_Returns_The_Result_From_The_Underlayer)
    {
        // arrange
        CIoTHubClientMocks mocks;
        IOTHUB_CLIENT_HANDLE iotHubClient = IoTHubClient_Create(&TEST_CONFIG);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
        STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_GetSendStatus(TEST_IOTHUB_CLIENT_LL_HANDLE, IGNORED_PTR_ARG))
            .IgnoreArgument(2)
            .SetReturn(IOTHUB_CLIENT_INVALID_SIZE);
        STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));

        // act
        IOTHUB_CLIENT_STATUS sendStatus;
        IOTHUB_CLIENT_RESULT result = IoTHubClient_GetSendStatus(iotHubClient, &sendStatus);

        // assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, result, IOTHUB_CLIENT_INVALID_SIZE);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        IoTHubClient_Destroy(iotHubClient);
    }

    /* Tests_SRS_IOTHUBCLIENT_01_034: [If acquiring the lock fails, IoTHubClient_GetSendStatus shall return IOTHUB_CLIENT_ERROR.] */
    TEST_FUNCTION(When_acquiring_the_lock_fails_then_IoTHubClient_GetSendStatus_fails)
    {
        // arrange
        CIoTHubClientMocks mocks;
        IOTHUB_CLIENT_HANDLE iotHubClient = IoTHubClient_Create(&TEST_CONFIG);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE))
            .SetReturn(LOCK_ERROR);

        // act
        IOTHUB_CLIENT_STATUS sendStatus;
        IOTHUB_CLIENT_RESULT result = IoTHubClient_GetSendStatus(iotHubClient, &sendStatus);

        // assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, result, IOTHUB_CLIENT_ERROR);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        IoTHubClient_Destroy(iotHubClient);
    }

    TEST_FUNCTION(When_unlock_fails_then_IoTHubClient_GetSendStatus_still_succeeds)
    {
        // arrange
        CIoTHubClientMocks mocks;
        IOTHUB_CLIENT_HANDLE iotHubClient = IoTHubClient_Create(&TEST_CONFIG);
        mocks.ResetAllCalls();

        IOTHUB_CLIENT_STATUS injectedSendStatus = IOTHUB_CLIENT_SEND_STATUS_BUSY;
        STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
        STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_GetSendStatus(TEST_IOTHUB_CLIENT_LL_HANDLE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &injectedSendStatus, sizeof(injectedSendStatus));
        STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE))
            .SetReturn(LOCK_ERROR);

        // act
        IOTHUB_CLIENT_STATUS sendStatus;
        IOTHUB_CLIENT_RESULT result = IoTHubClient_GetSendStatus(iotHubClient, &sendStatus);

        // assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        IoTHubClient_Destroy(iotHubClient);
    }

    /* Work scheduling */

    /* Tests_SRS_IOTHUBCLIENT_01_037: [The thread created by IoTHubClient_Create shall call IoTHubClient_LL_DoWork every 1 ms.] */
    /* Tests_SRS_IOTHUBCLIENT_01_038: [The thread shall exit when IoTHubClient_Destroy is called.] */
    /* Tests_SRS_IOTHUBCLIENT_01_039: [All calls to IoTHubClient_LL_DoWork shall be protected by the lock created in IotHubClient_Create.] */
    TEST_FUNCTION(Worker_Thread_calls_DoWork_Every_1_ms)
    {
        // arrange
        CIoTHubClientMocks mocks;
        IOTHUB_CLIENT_HANDLE iotHubClient = IoTHubClient_Create(&TEST_CONFIG);
        (void)IoTHubClient_SetMessageCallback(iotHubClient, messageCallback, (void*)0x42);
        mocks.ResetAllCalls();

        howManyDoWorkCalls = 1;
        current_iothub_client = iotHubClient;
        STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
        STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_DoWork(TEST_IOTHUB_CLIENT_LL_HANDLE));
        STRICT_EXPECTED_CALL(mocks, ThreadAPI_Sleep(1));
        STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));

        // act
        threadFunc(threadFuncArg);
        
        // assert
        mocks.AssertActualAndExpectedCalls();
        
        ///cleanup
        IoTHubClient_Destroy(iotHubClient);
    }

    /* Tests_SRS_IOTHUBCLIENT_01_037: [The thread created by IoTHubClient_Create shall call IoTHubClient_LL_DoWork every 1 ms.] */
    /* Tests_SRS_IOTHUBCLIENT_01_038: [The thread shall exit when IoTHubClient_Destroy is called.] */
    /* Tests_SRS_IOTHUBCLIENT_01_039: [All calls to IoTHubClient_LL_DoWork shall be protected by the lock created in IotHubClient_Create.] */
    TEST_FUNCTION(Worker_Thread_calls_DoWork_Every_1_ms_2_times)
    {
        // arrange
        CIoTHubClientMocks mocks;
        IOTHUB_CLIENT_HANDLE iotHubClient = IoTHubClient_Create(&TEST_CONFIG);
        (void)IoTHubClient_SetMessageCallback(iotHubClient, messageCallback, (void*)0x42);
        mocks.ResetAllCalls();

        howManyDoWorkCalls = 2;
        current_iothub_client = iotHubClient;
        STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
        STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_DoWork(TEST_IOTHUB_CLIENT_LL_HANDLE));
        STRICT_EXPECTED_CALL(mocks, ThreadAPI_Sleep(1));
        STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));

        STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
        STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_DoWork(TEST_IOTHUB_CLIENT_LL_HANDLE));
        STRICT_EXPECTED_CALL(mocks, ThreadAPI_Sleep(1));
        STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));

        // act
        threadFunc(threadFuncArg);

        // assert
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubClient_Destroy(iotHubClient);
    }

    /* Tests_SRS_IOTHUBCLIENT_01_040: [If acquiring the lock fails, IoTHubClient_LL_DoWork shall not be called.] */
    TEST_FUNCTION(When_Acquiring_The_lock_fails_then_DoWork_Is_Not_called)
    {
        // arrange
        CIoTHubClientMocks mocks;
        IOTHUB_CLIENT_HANDLE iotHubClient = IoTHubClient_Create(&TEST_CONFIG);
        (void)IoTHubClient_SetMessageCallback(iotHubClient, messageCallback, (void*)0x42);
        mocks.ResetAllCalls();

        howManyDoWorkCalls = 1;
        current_iothub_client = iotHubClient;

        /* first round, when lock fails */
        STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE))
            .SetReturn(LOCK_ERROR);
        STRICT_EXPECTED_CALL(mocks, ThreadAPI_Sleep(1));

        /* second round, when lock does not fail and DoWork gets called */
        STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
        STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_DoWork(TEST_IOTHUB_CLIENT_LL_HANDLE));
        STRICT_EXPECTED_CALL(mocks, ThreadAPI_Sleep(1));
        STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));

        // act
        threadFunc(threadFuncArg);

        // act
        threadFunc(threadFuncArg);

        // assert
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubClient_Destroy(iotHubClient);
    }

    /*Tests_SRS_IOTHUBCLIENT_02_034: [If parameter iotHubClientHandle is NULL then IoTHubClient_SetOption shall return IOTHUB_CLIENT_INVALID_ARG.] */
    TEST_FUNCTION(IoTHubClient_SetOption_with_NULL_handle_fails)
    {
        /// arrange
        CIoTHubClientMocks mocks;

        ///act
        auto result = IoTHubClient_SetOption(NULL, "a", "b");

        ///assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_INVALID_ARG, result);

        ///cleanup
        
    }

    /*Tests_SRS_IOTHUBCLIENT_02_035: [If parameter optionName is NULL then IoTHubClient_SetOption shall return IOTHUB_CLIENT_INVALID_ARG.] */
    TEST_FUNCTION(IoTHubClient_SetOption_with_NULL_optioName_fails)
    {
        /// arrange
        CIoTHubClientMocks mocks;

        IOTHUB_CLIENT_HANDLE handle = IoTHubClient_Create(&TEST_CONFIG);
        mocks.ResetAllCalls();

        ///act
        auto result = IoTHubClient_SetOption(handle, NULL, "b");

        ///assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_INVALID_ARG, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubClient_Destroy(handle);
    }

    /*Tests_SRS_IOTHUBCLIENT_02_036: [If parameter value is NULL then IoTHubClient_SetOption shall return IOTHUB_CLIENT_INVALID_ARG.] */
    TEST_FUNCTION(IoTHubClient_SetOption_with_NULL_value_fails)
    {
        /// arrange
        CIoTHubClientMocks mocks;

        IOTHUB_CLIENT_HANDLE handle = IoTHubClient_Create(&TEST_CONFIG);
        mocks.ResetAllCalls();

        ///act
        auto result = IoTHubClient_SetOption(handle, "a", NULL);

        ///assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_INVALID_ARG, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubClient_Destroy(handle);
    }

    /*Tests_SRS_IOTHUBCLIENT_02_038: [If optionName doesn't match one of the options handled by this module then IoTHubClient_SetOption shall call IoTHubClient_LL_SetOption passing the same parameters and return what IoTHubClient_LL_SetOption returns.]*/
    TEST_FUNCTION(IoTHubClient_SetOption_happy_path)
    {
        /// arrange
        CIoTHubClientMocks mocks;

        IOTHUB_CLIENT_HANDLE handle = IoTHubClient_Create(&TEST_CONFIG);
        mocks.ResetAllCalls();

        EXPECTED_CALL(mocks, IoTHubClient_LL_SetOption(IGNORED_PTR_ARG, "a", "b"));

        ///act
        auto result = IoTHubClient_SetOption(handle, "a", "b");

        ///assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubClient_Destroy(handle);
    }

    /*Tests_SRS_IOTHUBCLIENT_02_038: [If optionName doesn't match one of the options handled by this module then IoTHubClient_SetOption shall call IoTHubClient_LL_SetOption passing the same parameters and return what IoTHubClient_LL_SetOption returns.]*/
    TEST_FUNCTION(IoTHubClient_SetOption_fails_when_LL_fails)
    {
        /// arrange
        CIoTHubClientMocks mocks;

        IOTHUB_CLIENT_HANDLE handle = IoTHubClient_Create(&TEST_CONFIG);
        mocks.ResetAllCalls();

        EXPECTED_CALL(mocks, IoTHubClient_LL_SetOption(IGNORED_PTR_ARG, "a", "b"))
            .SetReturn(IOTHUB_CLIENT_ERROR);

        ///act
        auto result = IoTHubClient_SetOption(handle, "a", "b");

        ///assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubClient_Destroy(handle);
    }

END_TEST_SUITE(iothubclient_unittests)

