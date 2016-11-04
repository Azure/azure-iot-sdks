// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "testrunnerswitcher.h"
#include "umock_c.h"
#include "umocktypes_charptr.h"
#include "umock_c_negative_tests.h"
#include "umocktypes_stdint.h"

#define ENABLE_MOCKS
#include "azure_c_shared_utility/strings.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/threadapi.h"
#include "azure_c_shared_utility/lock.h"
#include "parson.h"
#include <signal.h>
#include "iothub_messaging_ll.h"
#undef ENABLE_MOCKS

#include "iothub_messaging.h"

TEST_DEFINE_ENUM_TYPE(IOTHUB_MESSAGING_RESULT, IOTHUB_MESSAGING_RESULT_VALUES);
IMPLEMENT_UMOCK_C_ENUM_TYPE(IOTHUB_MESSAGING_RESULT, IOTHUB_MESSAGING_RESULT_VALUES);

#define ENABLE_MOCKS
#include "azure_c_shared_utility/umock_c_prod.h"
MOCKABLE_FUNCTION(, void, TEST_FUNC_IOTHUB_OPEN_COMPLETE_CALLBACK, void*, context);
MOCKABLE_FUNCTION(, void, TEST_FUNC_IOTHUB_SEND_COMPLETE_CALLBACK, void*, context, IOTHUB_MESSAGING_RESULT, messagingResult);
MOCKABLE_FUNCTION(, void, TEST_FUNC_IOTHUB_FEEDBACK_MESSAGE_RECEIVED_CALLBACK, void*, context, IOTHUB_SERVICE_FEEDBACK_BATCH*, feedbackBatch);
#undef ENABLE_MOCKS

static TEST_MUTEX_HANDLE g_testByTest;
static TEST_MUTEX_HANDLE g_dllByDll;

static void* my_gballoc_malloc(size_t size)
{
    return (void*)malloc(size);
}

static void my_gballoc_free(void* ptr)
{
    if (ptr != NULL)
    {
        free(ptr);
    }
}

static int my_mallocAndStrcpy_s(char** destination, const char* source)
{
    char* p = (char*)malloc(2);
    p[0] = source[0];
    p[1] = '\0';
    *destination = p;
    return 0;
}

#define ENABLE_MOCKS
#include "azure_c_shared_utility/gballoc.h"
#undef ENABLE_MOCKS

static int TEST_ISOPENED = false;

DEFINE_ENUM_STRINGS(UMOCK_C_ERROR_CODE, UMOCK_C_ERROR_CODE_VALUES)

static void on_umock_c_error(UMOCK_C_ERROR_CODE error_code)
{
    char temp_str[256];
    (void)snprintf(temp_str, sizeof(temp_str), "umock_c reported error :%s", ENUM_TO_STRING(UMOCK_C_ERROR_CODE, error_code));
    ASSERT_FAIL(temp_str);
}

static IOTHUB_MESSAGE_HANDLE TEST_IOTHUB_MESSAGE_HANDLE = (IOTHUB_MESSAGE_HANDLE)0x5252;
static IOTHUB_MESSAGING_HANDLE TEST_IOTHUB_MESSAGING_HANDLE = (IOTHUB_MESSAGING_HANDLE)0x5656;
static IOTHUB_MESSAGING_RESULT TEST_IOTHUB_MESSAGING_RESULT = (IOTHUB_MESSAGING_RESULT)0x6767;
static IOTHUB_OPEN_COMPLETE_CALLBACK TEST_IOTHUB_OPEN_COMPLETE_CALLBACK;
static IOTHUB_FEEDBACK_MESSAGE_RECEIVED_CALLBACK TEST_IOTHUB_FEEDBACK_MESSAGE_RECEIVED_CALLBACK;
static IOTHUB_SEND_COMPLETE_CALLBACK TEST_IOTHUB_SEND_COMPLETE_CALLBACK;

typedef struct TEST_IOTHUB_MESSAGING_CLIENT_INSTANCE_TAG
{
    IOTHUB_MESSAGING_HANDLE IoTHubMessagingHandle;
    THREAD_HANDLE ThreadHandle;
    LOCK_HANDLE LockHandle;
    sig_atomic_t StopThread;
} TEST_IOTHUB_MESSAGING_CLIENT_INSTANCE;

static TEST_IOTHUB_MESSAGING_CLIENT_INSTANCE TEST_IOTHUB_MESSAGING_CLIENT;

static LOCK_HANDLE TEST_LOCK_HANDLE = (LOCK_HANDLE)0x1212;
static LOCK_HANDLE my_Lock_Init(void)
{
    return TEST_LOCK_HANDLE;
}

static LOCK_RESULT my_Lock_Deinit(LOCK_HANDLE handle)
{
    (void*)handle;
    return LOCK_OK;
}

static LOCK_RESULT my_Lock(LOCK_HANDLE handle)
{
    (void*)handle;
    return LOCK_OK;
}

static LOCK_RESULT my_Unlock(LOCK_HANDLE handle)
{
    (void*)handle;
    return LOCK_OK;
}

static THREADAPI_RESULT my_ThreadAPI_Join(THREAD_HANDLE threadHandle, int *res)
{
    (void*)threadHandle;
    res = 0;
    return THREADAPI_OK;
}

static IOTHUB_SERVICE_CLIENT_AUTH_HANDLE TEST_IOTHUB_SERVICE_CLIENT_AUTH_HANDLE = (IOTHUB_SERVICE_CLIENT_AUTH_HANDLE)0x4242;
static IOTHUB_MESSAGING_HANDLE my_IoTHubMessaging_LL_Create(IOTHUB_SERVICE_CLIENT_AUTH_HANDLE serviceClientHandle)
{
    (void*)serviceClientHandle;
    return TEST_IOTHUB_MESSAGING_HANDLE;
}

static void my_IoTHubMessaging_LL_Destroy(IOTHUB_MESSAGING_HANDLE messagingHandle)
{
    (void*)messagingHandle;
}

static IOTHUB_MESSAGING_RESULT my_IoTHubMessaging_LL_Open(IOTHUB_MESSAGING_HANDLE messagingHandle, IOTHUB_OPEN_COMPLETE_CALLBACK openCompleteCallback, void* userContextCallback)
{
    (void*)messagingHandle;
    (void)openCompleteCallback;
    (void)userContextCallback;
    return IOTHUB_MESSAGING_OK;
}

static void my_IoTHubMessaging_LL_Close(IOTHUB_MESSAGING_HANDLE messagingHandle)
{
    (void*)messagingHandle;
}

static IOTHUB_MESSAGING_RESULT my_IoTHubMessaging_LL_SetFeedbackMessageCallback(IOTHUB_MESSAGING_HANDLE messagingHandle, IOTHUB_FEEDBACK_MESSAGE_RECEIVED_CALLBACK feedbackMessageReceivedCallback, void* userContextCallback)
{
    (void*)messagingHandle;
    (void)feedbackMessageReceivedCallback;
    (void)userContextCallback;
    return IOTHUB_MESSAGING_OK;
}


BEGIN_TEST_SUITE(iothub_messaging_ut)

TEST_SUITE_INITIALIZE(TestClassInitialize)
{
    TEST_INITIALIZE_MEMORY_DEBUG(g_dllByDll);
    g_testByTest = TEST_MUTEX_CREATE();
    ASSERT_IS_NOT_NULL(g_testByTest);

    umock_c_init(on_umock_c_error);

    int result = umocktypes_charptr_register_types();
    ASSERT_ARE_EQUAL(int, 0, result);
    result = umocktypes_stdint_register_types();
    ASSERT_ARE_EQUAL(int, 0, result);

    REGISTER_UMOCK_ALIAS_TYPE(IOTHUB_SERVICE_CLIENT_AUTH_HANDLE, void*);
    REGISTER_UMOCK_ALIAS_TYPE(LOCK_HANDLE, void*);
    REGISTER_UMOCK_ALIAS_TYPE(IOTHUB_MESSAGING_CLIENT_HANDLE, void*);
    REGISTER_UMOCK_ALIAS_TYPE(IOTHUB_MESSAGING_HANDLE, void*);
    REGISTER_UMOCK_ALIAS_TYPE(THREAD_HANDLE, void*);
    REGISTER_UMOCK_ALIAS_TYPE(IOTHUB_OPEN_COMPLETE_CALLBACK, void*);
    REGISTER_UMOCK_ALIAS_TYPE(LOCK_RESULT, int);
    REGISTER_UMOCK_ALIAS_TYPE(IOTHUB_FEEDBACK_MESSAGE_RECEIVED_CALLBACK, void*);
    REGISTER_UMOCK_ALIAS_TYPE(IOTHUB_MESSAGE_HANDLE, void*);
    REGISTER_UMOCK_ALIAS_TYPE(IOTHUB_SEND_COMPLETE_CALLBACK, void*);
    REGISTER_UMOCK_ALIAS_TYPE(THREAD_START_FUNC, void*);
    REGISTER_UMOCK_ALIAS_TYPE(THREADAPI_RESULT, int);

    REGISTER_GLOBAL_MOCK_HOOK(gballoc_malloc, my_gballoc_malloc);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(gballoc_malloc, NULL);

    REGISTER_GLOBAL_MOCK_HOOK(gballoc_free, my_gballoc_free);

    REGISTER_GLOBAL_MOCK_HOOK(mallocAndStrcpy_s, my_mallocAndStrcpy_s);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(mallocAndStrcpy_s, 42);

    REGISTER_GLOBAL_MOCK_HOOK(Lock_Init, my_Lock_Init);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(Lock_Init, NULL);

    REGISTER_GLOBAL_MOCK_HOOK(Lock_Deinit, my_Lock_Deinit);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(Lock_Deinit, LOCK_ERROR);

    REGISTER_GLOBAL_MOCK_HOOK(Lock, my_Lock);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(Lock, LOCK_ERROR);

    REGISTER_GLOBAL_MOCK_HOOK(Unlock, my_Unlock);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(Unlock, LOCK_ERROR);

    REGISTER_GLOBAL_MOCK_HOOK(ThreadAPI_Join, my_ThreadAPI_Join);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(ThreadAPI_Join, THREADAPI_ERROR);

    REGISTER_GLOBAL_MOCK_HOOK(IoTHubMessaging_LL_Create, my_IoTHubMessaging_LL_Create);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(IoTHubMessaging_LL_Create, NULL);

    REGISTER_GLOBAL_MOCK_HOOK(IoTHubMessaging_LL_Destroy, my_IoTHubMessaging_LL_Destroy);

    REGISTER_GLOBAL_MOCK_HOOK(IoTHubMessaging_LL_Open, my_IoTHubMessaging_LL_Open);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(IoTHubMessaging_LL_Open, IOTHUB_MESSAGING_ERROR);

    REGISTER_GLOBAL_MOCK_HOOK(IoTHubMessaging_LL_Close, my_IoTHubMessaging_LL_Close);

    REGISTER_GLOBAL_MOCK_HOOK(IoTHubMessaging_LL_SetFeedbackMessageCallback, my_IoTHubMessaging_LL_SetFeedbackMessageCallback);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(IoTHubMessaging_LL_SetFeedbackMessageCallback, IOTHUB_MESSAGING_ERROR);
}

TEST_SUITE_CLEANUP(TestClassCleanup)
{
    (void)Lock_Init();
    umock_c_deinit();
    TEST_MUTEX_DESTROY(g_testByTest);
    TEST_DEINITIALIZE_MEMORY_DEBUG(g_dllByDll);
}

TEST_FUNCTION_INITIALIZE(TestMethodInitialize)
{
    if (TEST_MUTEX_ACQUIRE(g_testByTest))
    {
        ASSERT_FAIL("our mutex is ABANDONED. Failure in test framework");
    }

    umock_c_reset_all_calls();

    TEST_IOTHUB_MESSAGING_CLIENT.IoTHubMessagingHandle = (IOTHUB_MESSAGING_HANDLE)0x3434;
    TEST_IOTHUB_MESSAGING_CLIENT.ThreadHandle = (THREAD_HANDLE)0x3535;
    TEST_IOTHUB_MESSAGING_CLIENT.LockHandle = (LOCK_HANDLE)0x3636;
    TEST_IOTHUB_MESSAGING_CLIENT.StopThread = (sig_atomic_t)0x3737;
}

TEST_FUNCTION_CLEANUP(TestMethodCleanup)
{
    TEST_MUTEX_RELEASE(g_testByTest);
}

/*Tests_SRS_IOTHUBMESSAGING_12_001: [ IoTHubMessaging_Create shall verify the serviceClientHandle input parameter and if it is NULL then return NULL. ]*/
TEST_FUNCTION(IoTHubMessaging_Create_return_null_if_input_parameter_serviceClientHandle_is_NULL)
{
    ///arrange

    ///act
    IOTHUB_MESSAGING_CLIENT_HANDLE result = IoTHubMessaging_Create(NULL);

    ///assert
    ASSERT_IS_NULL(result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
}

/*Tests_SRS_IOTHUBMESSAGING_12_002: [ IoTHubMessaging_Create shall allocate a new IoTHubMessagingClient instance. ]*/
/*Tests_SRS_IOTHUBMESSAGING_12_004: [ IoTHubMessaging_Create shall create a lock object to be used later for serializing IoTHubMessagingClient calls. ]*/
/*Tests_SRS_IOTHUBMESSAGING_12_006: [ IoTHubMessaging_Create shall instantiate a new IoTHubMessaging_LL instance by calling IoTHubMessaging_LL_Create and passing the serviceClientHandle argument. ]*/
TEST_FUNCTION(IoTHubMessaging_Create_happy_path)
{
    // arrange
    STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(Lock_Init());
    STRICT_EXPECTED_CALL(IoTHubMessaging_LL_Create(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    // act
    IOTHUB_MESSAGING_CLIENT_HANDLE result = IoTHubMessaging_Create(TEST_IOTHUB_SERVICE_CLIENT_AUTH_HANDLE);

    // assert
    ASSERT_IS_NOT_NULL(result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    if (result != NULL)
    {
        free(result);
        result = NULL;
    }
}
/*Tests_SRS_IOTHUBMESSAGING_12_003 : [ If allocating memory for the new IoTHubMessagingClient instance fails, then IoTHubMessaging_Create shall return NULL. ]*/
/*Tests_SRS_IOTHUBMESSAGING_12_005 : [ If creating the lock fails, then IoTHubMessaging_Create shall return NULL. ]*/
/*Tests_SRS_IOTHUBMESSAGING_12_007 : [ If IoTHubMessaging_LL_Create fails, then IoTHubMessaging_Create shall return NULL. ]*/
/*Tests_SRS_IOTHUBMESSAGING_12_008 : [ If IoTHubMessaging_Create fails, all resources allocated by it shall be freed. ]*/
TEST_FUNCTION(IoTHubMessaging_Create_non_happy_path)
{
    // arrange
    int umockc_result = umock_c_negative_tests_init();
    ASSERT_ARE_EQUAL(int, 0, umockc_result);

    STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(Lock_Init());
    STRICT_EXPECTED_CALL(IoTHubMessaging_LL_Create(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    umock_c_negative_tests_snapshot();

    // act
    for (size_t i = 0; i < umock_c_negative_tests_call_count(); i++)
    {
        umock_c_negative_tests_reset();
        umock_c_negative_tests_fail_call(i);

        IOTHUB_MESSAGING_CLIENT_HANDLE result = IoTHubMessaging_Create(TEST_IOTHUB_SERVICE_CLIENT_AUTH_HANDLE);

        // assert
        ASSERT_IS_NULL(result);
    }
    umock_c_negative_tests_deinit();
}

/*Tests_SRS_IOTHUBMESSAGING_12_009: [ IoTHubMessaging_Destroy shall do nothing if parameter messagingClientHandle is NULL. ]*/
TEST_FUNCTION(IoTHubMessaging_Destroy_return_if_input_parameter_messagingHandle_is_NULL)
{
    // arrange

    // act
    IoTHubMessaging_Destroy(NULL);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
}

/*Tests_SRS_IOTHUBMESSAGING_12_010: [ IoTHubMessaging_Destroy shall lock the serializing lock and signal the worker thread (if any) to end. ]*/
/*Tests_SRS_IOTHUBMESSAGING_12_011: [ IoTHubMessaging_Destroy shall destroy IoTHubMessagingHandle by call IoTHubMessaging_LL_Destroy. ]*/
/*Tests_SRS_IOTHUBMESSAGING_12_012: [ IoTHubMessaging_Destroy shall unlock the serializing lock. ]*/
/*Tests_SRS_IOTHUBMESSAGING_12_013: [ The thread created as part of executing IoTHubMessaging_SendAsync shall be joined. ]*/
/*Tests_SRS_IOTHUBMESSAGING_12_014: [ If the lock was allocated in IoTHubMessaging_Create, it shall be also freed. ]*/
TEST_FUNCTION(IoTHubMessaging_Destroy_happy_path_thread_handle_null)
{
    // arrange
    IOTHUB_MESSAGING_CLIENT_HANDLE messagingClientHandle = IoTHubMessaging_Create(TEST_IOTHUB_SERVICE_CLIENT_AUTH_HANDLE);
    
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(Lock(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    
    STRICT_EXPECTED_CALL(Unlock(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(IoTHubMessaging_LL_Destroy(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(Lock_Deinit(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(gballoc_free(IGNORED_NUM_ARG))
        .IgnoreArgument(1);

    // act
    IoTHubMessaging_Destroy(messagingClientHandle);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
}

/*Tests_SRS_IOTHUBMESSAGING_12_010: [ IoTHubMessaging_Destroy shall lock the serializing lock and signal the worker thread (if any) to end. ]*/
/*Tests_SRS_IOTHUBMESSAGING_12_011: [ IoTHubMessaging_Destroy shall destroy IoTHubMessagingHandle by call IoTHubMessaging_LL_Destroy. ]*/
/*Tests_SRS_IOTHUBMESSAGING_12_012: [ IoTHubMessaging_Destroy shall unlock the serializing lock. ]*/
/*Tests_SRS_IOTHUBMESSAGING_12_013: [ The thread created as part of executing IoTHubMessaging_SendAsync shall be joined. ]*/
/*Tests_SRS_IOTHUBMESSAGING_12_014: [ If the lock was allocated in IoTHubMessaging_Create, it shall be also freed. ]*/
TEST_FUNCTION(IoTHubMessaging_Destroy_happy_path_thread_handle_not_null)
{
    // arrange
    IOTHUB_MESSAGING_CLIENT_HANDLE messagingClientHandle = IoTHubMessaging_Create(TEST_IOTHUB_SERVICE_CLIENT_AUTH_HANDLE);
    TEST_IOTHUB_MESSAGING_CLIENT_INSTANCE* messagingClientInstance = (TEST_IOTHUB_MESSAGING_CLIENT_INSTANCE*)messagingClientHandle;
    messagingClientInstance->ThreadHandle = (THREAD_HANDLE)0X3333;

    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(Lock(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(Unlock(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(ThreadAPI_Join(IGNORED_PTR_ARG, IGNORED_NUM_ARG))
        .IgnoreAllArguments();

    STRICT_EXPECTED_CALL(IoTHubMessaging_LL_Destroy(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(Lock_Deinit(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(gballoc_free(IGNORED_NUM_ARG))
        .IgnoreArgument(1);

    // act
    IoTHubMessaging_Destroy(messagingClientHandle);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
}
//
///*Tests_SRS_IOTHUBMESSAGING_12_010: [ IoTHubMessaging_Destroy shall lock the serializing lock and signal the worker thread (if any) to end. ]*/
///*Tests_SRS_IOTHUBMESSAGING_12_011: [ IoTHubMessaging_Destroy shall destroy IoTHubMessagingHandle by call IoTHubMessaging_LL_Destroy. ]*/
///*Tests_SRS_IOTHUBMESSAGING_12_012: [ IoTHubMessaging_Destroy shall unlock the serializing lock. ]*/
///*Tests_SRS_IOTHUBMESSAGING_12_013: [ The thread created as part of executing IoTHubMessaging_SendAsync shall be joined. ]*/
///*Tests_SRS_IOTHUBMESSAGING_12_014: [ If the lock was allocated in IoTHubMessaging_Create, it shall be also freed. ]*/
TEST_FUNCTION(IoTHubMessaging_Destroy_Lock_fails)
{
    // arrange
    IOTHUB_MESSAGING_CLIENT_HANDLE messagingClientHandle = IoTHubMessaging_Create(TEST_IOTHUB_SERVICE_CLIENT_AUTH_HANDLE);

    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(Lock(IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .SetReturn(LOCK_ERROR);

    STRICT_EXPECTED_CALL(IoTHubMessaging_LL_Destroy(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(Lock_Deinit(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(gballoc_free(IGNORED_NUM_ARG))
        .IgnoreArgument(1);

    // act
    IoTHubMessaging_Destroy(messagingClientHandle);

    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
}

///*Tests_SRS_IOTHUBMESSAGING_12_010: [ IoTHubMessaging_Destroy shall lock the serializing lock and signal the worker thread (if any) to end. ]*/
///*Tests_SRS_IOTHUBMESSAGING_12_011: [ IoTHubMessaging_Destroy shall destroy IoTHubMessagingHandle by call IoTHubMessaging_LL_Destroy. ]*/
///*Tests_SRS_IOTHUBMESSAGING_12_012: [ IoTHubMessaging_Destroy shall unlock the serializing lock. ]*/
///*Tests_SRS_IOTHUBMESSAGING_12_013: [ The thread created as part of executing IoTHubMessaging_SendAsync shall be joined. ]*/
///*Tests_SRS_IOTHUBMESSAGING_12_014: [ If the lock was allocated in IoTHubMessaging_Create, it shall be also freed. ]*/
TEST_FUNCTION(IoTHubMessaging_Destroy_Unlock_fails)
{
    // arrange
    IOTHUB_MESSAGING_CLIENT_HANDLE messagingClientHandle = IoTHubMessaging_Create(TEST_IOTHUB_SERVICE_CLIENT_AUTH_HANDLE);

    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(Lock(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(Unlock(IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .SetReturn(LOCK_ERROR);

    STRICT_EXPECTED_CALL(IoTHubMessaging_LL_Destroy(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(Lock_Deinit(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(gballoc_free(IGNORED_NUM_ARG))
        .IgnoreArgument(1);

    // act
    IoTHubMessaging_Destroy(messagingClientHandle);

    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
}

///*Tests_SRS_IOTHUBMESSAGING_12_010: [ IoTHubMessaging_Destroy shall lock the serializing lock and signal the worker thread (if any) to end. ]*/
///*Tests_SRS_IOTHUBMESSAGING_12_011: [ IoTHubMessaging_Destroy shall destroy IoTHubMessagingHandle by call IoTHubMessaging_LL_Destroy. ]*/
///*Tests_SRS_IOTHUBMESSAGING_12_012: [ IoTHubMessaging_Destroy shall unlock the serializing lock. ]*/
///*Tests_SRS_IOTHUBMESSAGING_12_013: [ The thread created as part of executing IoTHubMessaging_SendAsync shall be joined. ]*/
///*Tests_SRS_IOTHUBMESSAGING_12_014: [ If the lock was allocated in IoTHubMessaging_Create, it shall be also freed. ]*/
TEST_FUNCTION(IoTHubMessaging_Destroy_ThreadAPI_Join_fails)
{
    // arrange
    IOTHUB_MESSAGING_CLIENT_HANDLE messagingClientHandle = IoTHubMessaging_Create(TEST_IOTHUB_SERVICE_CLIENT_AUTH_HANDLE);
    TEST_IOTHUB_MESSAGING_CLIENT_INSTANCE* messagingClientInstance = (TEST_IOTHUB_MESSAGING_CLIENT_INSTANCE*)messagingClientHandle;
    messagingClientInstance->ThreadHandle = (THREAD_HANDLE)0X3333;

    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(Lock(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(Unlock(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(ThreadAPI_Join(IGNORED_PTR_ARG, IGNORED_NUM_ARG))
        .IgnoreAllArguments()
        .SetReturn(THREADAPI_ERROR);

    STRICT_EXPECTED_CALL(IoTHubMessaging_LL_Destroy(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(Lock_Deinit(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(gballoc_free(IGNORED_NUM_ARG))
        .IgnoreArgument(1);

    // act
    IoTHubMessaging_Destroy(messagingClientHandle);

    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
}

/*Tests_SRS_IOTHUBMESSAGING_12_015: [ If messagingClientHandle is NULL, IoTHubMessaging_Open shall return IOTHUB_MESSAGING_INVALID_ARG. ]*/
TEST_FUNCTION(IoTHubMessaging_Open_return_IOTHUB_MESSAGING_INVALID_ARG_if_input_parameter_messagingClientHandle_is_NULL)
{
    ///arrange

    ///act
    IOTHUB_MESSAGING_RESULT result = IoTHubMessaging_Open(NULL, TEST_IOTHUB_OPEN_COMPLETE_CALLBACK, (void*)0x4242);

    ///assert
    ASSERT_ARE_EQUAL(int, IOTHUB_MESSAGING_INVALID_ARG, result);
}

/*Tests_SRS_IOTHUBMESSAGING_12_016: [ IoTHubMessaging_Open shall be made thread-safe by using the lock created in IoTHubMessaging_Create. ]*/
/*Tests_SRS_IOTHUBMESSAGING_12_018: [ IoTHubMessaging_Open shall call IoTHubMessaging_LL_Open, while passing the IOTHUB_MESSAGING_HANDLE handle created by IoTHubMessaging_Create and the parameters openCompleteCallback and userContextCallback. ]*/
/*Tests_SRS_IOTHUBMESSAGING_12_019: [ When IoTHubMessaging_LL_Open is called, IoTHubMessaging_Open shall return the result of IoTHubMessaging_LL_Open. ]*/
/*Tests_SRS_IOTHUBMESSAGING_12_020: [ IoTHubMessaging_Open shall be made thread-safe by using the lock created in IoTHubMessaging_Create. ]*/
TEST_FUNCTION(IoTHubMessaging_Open_happy_path)
{
    // arrange
    IOTHUB_MESSAGING_CLIENT_HANDLE messagingClientHandle = IoTHubMessaging_Create(TEST_IOTHUB_SERVICE_CLIENT_AUTH_HANDLE);
    TEST_IOTHUB_MESSAGING_CLIENT_INSTANCE* messagingClientInstance = (TEST_IOTHUB_MESSAGING_CLIENT_INSTANCE*)messagingClientHandle;
    messagingClientInstance->IoTHubMessagingHandle = (IOTHUB_MESSAGING_HANDLE)0X3333;

    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(Lock(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(IoTHubMessaging_LL_Open((IOTHUB_MESSAGING_HANDLE)0X3333, TEST_IOTHUB_OPEN_COMPLETE_CALLBACK, (void*)0x4242));
    STRICT_EXPECTED_CALL(Unlock(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    // act
    IOTHUB_MESSAGING_RESULT result = IoTHubMessaging_Open(messagingClientHandle, TEST_IOTHUB_OPEN_COMPLETE_CALLBACK, (void*)0x4242);

    // assert
    ASSERT_ARE_EQUAL(int, IOTHUB_MESSAGING_OK, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    free(messagingClientHandle);
}

/*Tests_SRS_IOTHUBMESSAGING_12_017: [ If acquiring the lock fails, IoTHubMessaging_Open shall return IOTHUB_MESSAGING_ERROR. ]*/
TEST_FUNCTION(IoTHubMessaging_Open_Lock_fails)
{
    // arrange
    IOTHUB_MESSAGING_CLIENT_HANDLE messagingClientHandle = IoTHubMessaging_Create(TEST_IOTHUB_SERVICE_CLIENT_AUTH_HANDLE);

    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(Lock(IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .SetReturn(LOCK_ERROR);

    // act
    IOTHUB_MESSAGING_RESULT result = IoTHubMessaging_Open(messagingClientHandle, TEST_IOTHUB_OPEN_COMPLETE_CALLBACK, (void*)0x4242);

    // assert
    ASSERT_ARE_EQUAL(int, IOTHUB_MESSAGING_ERROR, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    free(messagingClientHandle);
}

/*Tests_SRS_IOTHUBMESSAGING_12_021: [ If messagingClientHandle is NULL, IoTHubMessaging_Close shall do nothing. ]*/
TEST_FUNCTION(IoTHubMessaging_Close_do_nothing_if_input_parameter_messagingClientHandle_is_NULL)
{
    ///arrange

    ///act
    IoTHubMessaging_Close(NULL);

    ///assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
}

/*Tests_SRS_IOTHUBMESSAGING_12_022: [ IoTHubMessaging_Close shall be made thread-safe by using the lock created in IoTHubMessaging_Create. ]*/
/*Tests_SRS_IOTHUBMESSAGING_12_024: [ IoTHubMessaging_Close shall call IoTHubMessaging_LL_Close, while passing the IOTHUB_MESSAGING_HANDLE handle created by IoTHubMessaging_Create ]*/
/*Tests_SRS_IOTHUBMESSAGING_12_025: [ When IoTHubMessaging_LL_Close is called, IoTHubMessaging_Close shall return the result of IoTHubMessaging_LL_Close. ]*/
/*Tests_SRS_IOTHUBMESSAGING_12_026: [ IoTHubMessaging_Close shall be made thread-safe by using the lock created in IoTHubMessaging_Create. ]*/
TEST_FUNCTION(IoTHubMessaging_Close_happy_path)
{
    // arrange
    IOTHUB_MESSAGING_CLIENT_HANDLE messagingClientHandle = IoTHubMessaging_Create(TEST_IOTHUB_SERVICE_CLIENT_AUTH_HANDLE);
    TEST_IOTHUB_MESSAGING_CLIENT_INSTANCE* messagingClientInstance = (TEST_IOTHUB_MESSAGING_CLIENT_INSTANCE*)messagingClientHandle;
    messagingClientInstance->IoTHubMessagingHandle = TEST_IOTHUB_MESSAGING_HANDLE;

    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(Lock(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(IoTHubMessaging_LL_Close(TEST_IOTHUB_MESSAGING_HANDLE));
    STRICT_EXPECTED_CALL(Unlock(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    // act
    IoTHubMessaging_Close(messagingClientHandle);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    free(messagingClientHandle);
}

/*Tests_SRS_IOTHUBMESSAGING_12_023: [ If acquiring the lock fails, IoTHubMessaging_Close shall return IOTHUB_MESSAGING_ERROR. ]*/
TEST_FUNCTION(IoTHubMessaging_Close_Lock_fails)
{
    // arrange
    IOTHUB_MESSAGING_CLIENT_HANDLE messagingClientHandle = IoTHubMessaging_Create(TEST_IOTHUB_SERVICE_CLIENT_AUTH_HANDLE);

    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(Lock(IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .SetReturn(LOCK_ERROR);

    // act
    IoTHubMessaging_Close(messagingClientHandle);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    free(messagingClientHandle);
}

/*Tests_SRS_IOTHUBMESSAGING_12_027: [ If messagingClientHandle is NULL, IoTHubMessaging_SetFeedbackMessageCallback shall return IOTHUB_MESSAGING_INVALID_ARG. ]*/
TEST_FUNCTION(IoTHubMessaging_SetFeedbackMessageCallback_return_IOTHUB_MESSAGING_INVALID_ARG_if_input_parameter_messagingClientHandle_is_NULL)
{
    ///arrange

    ///act
    IOTHUB_MESSAGING_RESULT result = IoTHubMessaging_SetFeedbackMessageCallback(NULL, TEST_IOTHUB_FEEDBACK_MESSAGE_RECEIVED_CALLBACK, (void*)0x4242);

    ///assert
    ASSERT_ARE_EQUAL(int, IOTHUB_MESSAGING_INVALID_ARG, result);
}

/*Tests_SRS_IOTHUBMESSAGING_12_028: [ IoTHubMessaging_SetFeedbackMessageCallback shall be made thread-safe by using the lock created in IoTHubMessaging_Create. ]*/
/*Tests_SRS_IOTHUBMESSAGING_12_030: [ IoTHubMessaging_SetFeedbackMessageCallback shall call IoTHubMessaging_LL_SetFeedbackMessageCallback, while passing the IOTHUB_MESSAGING_HANDLE handle created by IoTHubMessaging_Create, feedbackMessageReceivedCallback and userContextCallback ]*/
/*Tests_SRS_IOTHUBMESSAGING_12_031: [ When IoTHubMessaging_LL_SetFeedbackMessageCallback is called, IoTHubMessaging_SetFeedbackMessageCallback shall return the result of IoTHubMessaging_LL_SetFeedbackMessageCallback. ]*/
/*Tests_SRS_IOTHUBMESSAGING_12_032: [ IoTHubMessaging_SetFeedbackMessageCallback shall be made thread-safe by using the lock created in IoTHubMessaging_Create. ]*/
TEST_FUNCTION(IoTHubMessaging_SetFeedbackMessageCallback_happy_path)
{
    // arrange
    IOTHUB_MESSAGING_CLIENT_HANDLE messagingClientHandle = IoTHubMessaging_Create(TEST_IOTHUB_SERVICE_CLIENT_AUTH_HANDLE);
    TEST_IOTHUB_MESSAGING_CLIENT_INSTANCE* messagingClientInstance = (TEST_IOTHUB_MESSAGING_CLIENT_INSTANCE*)messagingClientHandle;
    messagingClientInstance->IoTHubMessagingHandle = (IOTHUB_MESSAGING_HANDLE)0X3333;

    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(Lock(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(IoTHubMessaging_LL_SetFeedbackMessageCallback((IOTHUB_MESSAGING_HANDLE)0X3333, TEST_IOTHUB_FEEDBACK_MESSAGE_RECEIVED_CALLBACK, (void*)0x4242));
    STRICT_EXPECTED_CALL(Unlock(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    // act
    IOTHUB_MESSAGING_RESULT result = IoTHubMessaging_SetFeedbackMessageCallback(messagingClientHandle, TEST_IOTHUB_FEEDBACK_MESSAGE_RECEIVED_CALLBACK, (void*)0x4242);

    // assert
    ASSERT_ARE_EQUAL(int, IOTHUB_MESSAGING_OK, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    free(messagingClientHandle);
}

/*Tests_SRS_IOTHUBMESSAGING_12_029: [ If acquiring the lock fails, IoTHubMessaging_SetFeedbackMessageCallback shall return IOTHUB_MESSAGING_ERROR. ]*/
TEST_FUNCTION(IoTHubMessaging_SetFeedbackMessageCallback_Lock_fails)
{
    // arrange
    IOTHUB_MESSAGING_CLIENT_HANDLE messagingClientHandle = IoTHubMessaging_Create(TEST_IOTHUB_SERVICE_CLIENT_AUTH_HANDLE);

    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(Lock(IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .SetReturn(LOCK_ERROR);

    // act
    IOTHUB_MESSAGING_RESULT result = IoTHubMessaging_SetFeedbackMessageCallback(messagingClientHandle, TEST_IOTHUB_FEEDBACK_MESSAGE_RECEIVED_CALLBACK, (void*)0x4242);

    // assert
    ASSERT_ARE_EQUAL(int, IOTHUB_MESSAGING_ERROR, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    free(messagingClientHandle);
}

/*Tests_SRS_IOTHUBMESSAGING_12_033: [ If messagingClientHandle is NULL, IoTHubMessaging_SendAsync shall return IOTHUB_MESSAGING_INVALID_ARG. ]*/
TEST_FUNCTION(IoTHubMessaging_SendAsync_return_IOTHUB_MESSAGING_INVALID_ARG_if_input_parameter_messagingClientHandle_is_NULL)
{
    ///arrange
    const char* deviceId = "42";
    ///act
    IOTHUB_MESSAGING_RESULT result = IoTHubMessaging_SendAsync(NULL, deviceId, TEST_IOTHUB_MESSAGE_HANDLE, TEST_IOTHUB_SEND_COMPLETE_CALLBACK, (void*)0x4242);

    ///assert
    ASSERT_ARE_EQUAL(int, IOTHUB_MESSAGING_INVALID_ARG, result);
}

/*Tests_SRS_IOTHUBMESSAGING_12_034: [ IoTHubMessaging_SendAsync shall be made thread-safe by using the lock created in IoTHubMessaging_Create. ]*/
/*Tests_SRS_IOTHUBMESSAGING_12_036: [ IoTHubMessaging_SendAsync shall start the worker thread if it was not previously started. ]*/
/*Tests_SRS_IOTHUBMESSAGING_12_038: [ IoTHubMessaging_SendAsync shall call IoTHubMessaging_LL_Send, while passing the IOTHUB_MESSAGING_HANDLE handle created by IoTHubClient_Create and the parameters deviceId, message, sendCompleteCallback and userContextCallback.*/
/*Tests_SRS_IOTHUBMESSAGING_12_039: [ When IoTHubMessaging_LL_Send is called, IoTHubMessaging_SendAsync shall return the result of IoTHubMessaging_LL_Send. ]*/
/*Tests_SRS_IOTHUBMESSAGING_12_040: [ IoTHubClient_SendEventAsync shall be made thread-safe by using the lock created in IoTHubClient_Create. ]*/
TEST_FUNCTION(IoTHubMessaging_SendAsync_happy_path)
{
    // arrange
    const char* deviceId = "42";

    IOTHUB_MESSAGING_CLIENT_HANDLE messagingClientHandle = IoTHubMessaging_Create(TEST_IOTHUB_SERVICE_CLIENT_AUTH_HANDLE);
    TEST_IOTHUB_MESSAGING_CLIENT_INSTANCE* messagingClientInstance = (TEST_IOTHUB_MESSAGING_CLIENT_INSTANCE*)messagingClientHandle;
    messagingClientInstance->IoTHubMessagingHandle = (IOTHUB_MESSAGING_HANDLE)0X3333;

    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(Lock(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(ThreadAPI_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreAllArguments();

    STRICT_EXPECTED_CALL(IoTHubMessaging_LL_Send((IOTHUB_MESSAGING_HANDLE)0X3333, deviceId, TEST_IOTHUB_MESSAGE_HANDLE, TEST_IOTHUB_SEND_COMPLETE_CALLBACK, (void*)0x4242));
    STRICT_EXPECTED_CALL(Unlock(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    // act
    IOTHUB_MESSAGING_RESULT result = IoTHubMessaging_SendAsync(messagingClientHandle, deviceId, TEST_IOTHUB_MESSAGE_HANDLE, TEST_IOTHUB_SEND_COMPLETE_CALLBACK, (void*)0x4242);

    // assert
    ASSERT_ARE_EQUAL(int, IOTHUB_MESSAGING_OK, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    free(messagingClientHandle);
}

/*Tests_SRS_IOTHUBMESSAGING_12_034: [ IoTHubMessaging_SendAsync shall be made thread-safe by using the lock created in IoTHubMessaging_Create. ]*/
/*Tests_SRS_IOTHUBMESSAGING_12_036: [ IoTHubMessaging_SendAsync shall start the worker thread if it was not previously started. ]*/
/*Tests_SRS_IOTHUBMESSAGING_12_038: [ IoTHubMessaging_SendAsync shall call IoTHubMessaging_LL_Send, while passing the IOTHUB_MESSAGING_HANDLE handle created by IoTHubClient_Create and the parameters deviceId, message, sendCompleteCallback and userContextCallback.*/
/*Tests_SRS_IOTHUBMESSAGING_12_039: [ When IoTHubMessaging_LL_Send is called, IoTHubMessaging_SendAsync shall return the result of IoTHubMessaging_LL_Send. ]*/
/*Tests_SRS_IOTHUBMESSAGING_12_040: [ IoTHubClient_SendEventAsync shall be made thread-safe by using the lock created in IoTHubClient_Create. ]*/
TEST_FUNCTION(IoTHubMessaging_SendAsync_happy_path_threadhandle_not_null)
{
    // arrange
    const char* deviceId = "42";

    IOTHUB_MESSAGING_CLIENT_HANDLE messagingClientHandle = IoTHubMessaging_Create(TEST_IOTHUB_SERVICE_CLIENT_AUTH_HANDLE);
    TEST_IOTHUB_MESSAGING_CLIENT_INSTANCE* messagingClientInstance = (TEST_IOTHUB_MESSAGING_CLIENT_INSTANCE*)messagingClientHandle;
    messagingClientInstance->IoTHubMessagingHandle = (IOTHUB_MESSAGING_HANDLE)0X3333;
    messagingClientInstance->ThreadHandle = (IOTHUB_MESSAGING_HANDLE)0x4444;

    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(Lock(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(IoTHubMessaging_LL_Send((IOTHUB_MESSAGING_HANDLE)0X3333, deviceId, TEST_IOTHUB_MESSAGE_HANDLE, TEST_IOTHUB_SEND_COMPLETE_CALLBACK, (void*)0x4242));
    STRICT_EXPECTED_CALL(Unlock(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    // act
    IOTHUB_MESSAGING_RESULT result = IoTHubMessaging_SendAsync(messagingClientHandle, deviceId, TEST_IOTHUB_MESSAGE_HANDLE, TEST_IOTHUB_SEND_COMPLETE_CALLBACK, (void*)0x4242);

    // assert
    ASSERT_ARE_EQUAL(int, IOTHUB_MESSAGING_OK, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    free(messagingClientHandle);
}

/*Tests_SRS_IOTHUBMESSAGING_12_035: [ If acquiring the lock fails, IoTHubMessaging_SendAsync shall return IOTHUB_MESSAGING_ERROR. ]*/
TEST_FUNCTION(IoTHubMessaging_SendAsync_Lock_fails)
{
    // arrange
    const char* deviceId = "42";

    IOTHUB_MESSAGING_CLIENT_HANDLE messagingClientHandle = IoTHubMessaging_Create(TEST_IOTHUB_SERVICE_CLIENT_AUTH_HANDLE);
    TEST_IOTHUB_MESSAGING_CLIENT_INSTANCE* messagingClientInstance = (TEST_IOTHUB_MESSAGING_CLIENT_INSTANCE*)messagingClientHandle;
    messagingClientInstance->IoTHubMessagingHandle = (IOTHUB_MESSAGING_HANDLE)0X3333;

    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(Lock(IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .SetReturn(LOCK_ERROR);

    // act
    IOTHUB_MESSAGING_RESULT result = IoTHubMessaging_SendAsync(messagingClientHandle, deviceId, TEST_IOTHUB_MESSAGE_HANDLE, TEST_IOTHUB_SEND_COMPLETE_CALLBACK, (void*)0x4242);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, IOTHUB_MESSAGING_OK, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    free(messagingClientHandle);
}

/*Tests_SRS_IOTHUBMESSAGING_12_037: [ If starting the thread fails, IoTHubMessaging_SendAsync shall return IOTHUB_CLIENT_ERROR. ]*/
TEST_FUNCTION(IoTHubMessaging_ThreadAPI_Create_fails)
{
    // arrange
    const char* deviceId = "42";

    IOTHUB_MESSAGING_CLIENT_HANDLE messagingClientHandle = IoTHubMessaging_Create(TEST_IOTHUB_SERVICE_CLIENT_AUTH_HANDLE);
    TEST_IOTHUB_MESSAGING_CLIENT_INSTANCE* messagingClientInstance = (TEST_IOTHUB_MESSAGING_CLIENT_INSTANCE*)messagingClientHandle;
    messagingClientInstance->IoTHubMessagingHandle = (IOTHUB_MESSAGING_HANDLE)0X3333;

    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(Lock(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(ThreadAPI_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreAllArguments()
        .SetReturn(THREADAPI_ERROR);

    STRICT_EXPECTED_CALL(Unlock(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    // act
    IOTHUB_MESSAGING_RESULT result = IoTHubMessaging_SendAsync(messagingClientHandle, deviceId, TEST_IOTHUB_MESSAGE_HANDLE, TEST_IOTHUB_SEND_COMPLETE_CALLBACK, (void*)0x4242);

    // assert
    ASSERT_ARE_NOT_EQUAL(int, IOTHUB_MESSAGING_OK, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    free(messagingClientHandle);
}

END_TEST_SUITE(iothub_messaging_ut)
