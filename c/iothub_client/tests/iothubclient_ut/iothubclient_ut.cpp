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
#include "azure_c_shared_utility/threadapi.h"
#include "azure_c_shared_utility/lock.h"
#include "azure_c_shared_utility/singlylinkedlist.h"
#include "iothubtransport.h"

extern "C" int gballoc_init(void);
extern "C" void gballoc_deinit(void);
extern "C" void* gballoc_malloc(size_t size);
extern "C" void* gballoc_calloc(size_t nmemb, size_t size);
extern "C" void* gballoc_realloc(void* ptr, size_t size);
extern "C" void gballoc_free(void* ptr);

DEFINE_MICROMOCK_ENUM_TO_STRING(IOTHUB_CLIENT_STATUS, IOTHUB_CLIENT_STATUS_VALUES);
DEFINE_MICROMOCK_ENUM_TO_STRING(IOTHUB_CLIENT_RETRY_POLICY, IOTHUB_CLIENT_RETRY_POLICY_VALUES);

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

// Overloading operators for Micromock
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
static const TRANSPORT_PROVIDER* provideFAKE(void);
static IOTHUB_CLIENT_RETRY_POLICY saved_retry_policy;
static size_t saved_retry_timeout_in_secs;
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
#define TEST_IOTHUBTRANSPORT_HANDLE (TRANSPORT_HANDLE)0xDEAD
#define TEST_IOTHUBTRANSPORT_LOCK (TRANSPORT_HANDLE)0xDEAF
#define TEST_IOTHUBTRANSPORT_LL (TRANSPORT_HANDLE)0xDEDE
#define TEST_LIST_HANDLE				(SINGLYLINKEDLIST_HANDLE)0x4246
#define TEST_RETRY_POLICY      IOTHUB_CLIENT_RETRY_EXPONENTIAL_BACKOFF_WITH_JITTER
#define TEST_RETRY_TIMEOUT_SECS      60
typedef struct TEST_LIST_ITEM_TAG
{
    const void* item_value;
} TEST_LIST_ITEM;

static TEST_LIST_ITEM** list_items = NULL;
static size_t list_item_count = 0;
TYPED_MOCK_CLASS(CIoTHubClientMocks, CGlobalMock)
{
public:

    /* IoTHubClient_LL mocks */
    MOCK_STATIC_METHOD_2(, IOTHUB_CLIENT_LL_HANDLE, IoTHubClient_LL_CreateFromConnectionString, const char*, connectionString, IOTHUB_CLIENT_TRANSPORT_PROVIDER, protocol);
    MOCK_METHOD_END(IOTHUB_CLIENT_LL_HANDLE, TEST_IOTHUB_CLIENT_LL_HANDLE);
    MOCK_STATIC_METHOD_1(, IOTHUB_CLIENT_LL_HANDLE, IoTHubClient_LL_Create, const IOTHUB_CLIENT_CONFIG*, config);
    MOCK_METHOD_END(IOTHUB_CLIENT_LL_HANDLE, TEST_IOTHUB_CLIENT_LL_HANDLE);
    MOCK_STATIC_METHOD_1(, IOTHUB_CLIENT_LL_HANDLE, IoTHubClient_LL_CreateWithTransport, const IOTHUB_CLIENT_DEVICE_CONFIG*,  config);
    MOCK_METHOD_END(IOTHUB_CLIENT_LL_HANDLE, TEST_IOTHUB_CLIENT_LL_HANDLE);
    MOCK_STATIC_METHOD_1(, void, IoTHubClient_LL_Destroy, IOTHUB_CLIENT_LL_HANDLE, iotHubClientHandle);
    MOCK_VOID_METHOD_END();
    MOCK_STATIC_METHOD_4(, IOTHUB_CLIENT_RESULT, IoTHubClient_LL_SendEventAsync, IOTHUB_CLIENT_LL_HANDLE, iotHubClientHandle, IOTHUB_MESSAGE_HANDLE, eventMessageHandle, IOTHUB_CLIENT_EVENT_CONFIRMATION_CALLBACK, eventConfirmationCallback, void*, userContextCallback)
    MOCK_METHOD_END(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK);
    MOCK_STATIC_METHOD_3(, IOTHUB_CLIENT_RESULT, IoTHubClient_LL_SetMessageCallback, IOTHUB_CLIENT_LL_HANDLE, iotHubClientHandle, IOTHUB_CLIENT_MESSAGE_CALLBACK_ASYNC, messageCallback, void*, userContextCallback)
    MOCK_METHOD_END(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK);
    MOCK_STATIC_METHOD_3(, IOTHUB_CLIENT_RESULT, IoTHubClient_LL_SetRetryPolicy, IOTHUB_CLIENT_LL_HANDLE, iotHubClientHandle, IOTHUB_CLIENT_RETRY_POLICY, retryPolicy, size_t, retryTimeoutLimitInSeconds)
        saved_retry_policy = retryPolicy;
        saved_retry_timeout_in_secs = retryTimeoutLimitInSeconds;
    MOCK_METHOD_END(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK);
    MOCK_STATIC_METHOD_3(, IOTHUB_CLIENT_RESULT, IoTHubClient_LL_SetConnectionStatusCallback, IOTHUB_CLIENT_LL_HANDLE, iotHubClientHandle, IOTHUB_CLIENT_CONNECTION_STATUS_CALLBACK, connectionStatusCallback, void*, userContextCallback)
    MOCK_METHOD_END(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK);
    MOCK_STATIC_METHOD_3(, IOTHUB_CLIENT_RESULT, IoTHubClient_LL_GetRetryPolicy, IOTHUB_CLIENT_LL_HANDLE, iotHubClientHandle, IOTHUB_CLIENT_RETRY_POLICY*, retryPolicy, size_t*, retryTimeoutLimitInSeconds)
        *retryPolicy = saved_retry_policy;
        *retryTimeoutLimitInSeconds = saved_retry_timeout_in_secs;
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

    MOCK_STATIC_METHOD_6(, void, getDesiredCallback, IOTHUB_CLIENT_CONFIRMATION_RESULT, result2, const unsigned char*, desiredState, size_t, size, uint32_t, desiredVersion, uint32_t, lastSeenReportedVersion, void*, userContextCallback)
    MOCK_VOID_METHOD_END();

    MOCK_STATIC_METHOD_4(, void, deviceTwinCallback, DEVICE_TWIN_UPDATE_STATE, update_state, const unsigned char*, payLoad, size_t, size, void*, userContextCallback)
    MOCK_VOID_METHOD_END();

    MOCK_STATIC_METHOD_3(, IOTHUB_CLIENT_RESULT, IoTHubClient_LL_SetDeviceTwinCallback, IOTHUB_CLIENT_LL_HANDLE, iotHubClientHandle, IOTHUB_CLIENT_DEVICE_TWIN_CALLBACK, deviceTwinCallback, void*, userContextCallback)
    MOCK_METHOD_END(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK);

    MOCK_STATIC_METHOD_6(, int, deviceMethodCallback, const char*, method_name, const unsigned char*, payLoad, size_t, size, unsigned char**, response, size_t*, resp_size, void*, userContextCallback)
    MOCK_METHOD_END(int, 200);

    MOCK_STATIC_METHOD_3(, IOTHUB_CLIENT_RESULT, IoTHubClient_LL_SetDeviceMethodCallback, IOTHUB_CLIENT_LL_HANDLE, iotHubClientHandle, IOTHUB_CLIENT_DEVICE_METHOD_CALLBACK_ASYNC, deviceMethodCallback, void*, userContextCallback);
    MOCK_METHOD_END(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK);

    MOCK_STATIC_METHOD_2(, void, sendReportedCallback, int, status_code, void*, userContextCallback)
    MOCK_VOID_METHOD_END();
    MOCK_STATIC_METHOD_5(, IOTHUB_CLIENT_RESULT, IoTHubClient_LL_SendReportedState, IOTHUB_CLIENT_LL_HANDLE, iotHubClientHandle, const unsigned char*, reportedState, size_t, size, IOTHUB_CLIENT_REPORTED_STATE_CALLBACK, reportedStateCallback, void*, userContextCallback)
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

    MOCK_STATIC_METHOD_3(, void, connectionStatusCallback, IOTHUB_CLIENT_CONNECTION_STATUS, result3, IOTHUB_CLIENT_CONNECTION_STATUS_REASON, reason, void*, userContextCallback)
    MOCK_VOID_METHOD_END()
    /* TRANSPORT mocks*/

    MOCK_STATIC_METHOD_1(, LOCK_HANDLE, IoTHubTransport_GetLock, TRANSPORT_HANDLE, transportHlHandle)
    MOCK_METHOD_END(LOCK_HANDLE, TEST_IOTHUBTRANSPORT_LOCK)

    MOCK_STATIC_METHOD_1(, TRANSPORT_LL_HANDLE, IoTHubTransport_GetLLTransport, TRANSPORT_HANDLE, transportHlHandle)
    MOCK_METHOD_END(TRANSPORT_LL_HANDLE, TEST_IOTHUBTRANSPORT_LL)

    MOCK_STATIC_METHOD_2(, IOTHUB_CLIENT_RESULT, IoTHubTransport_StartWorkerThread, TRANSPORT_HANDLE, transportHlHandle, IOTHUB_CLIENT_HANDLE, clientHandle)
    MOCK_METHOD_END(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK)

    MOCK_STATIC_METHOD_2(, bool, IoTHubTransport_SignalEndWorkerThread, TRANSPORT_HANDLE, transportHlHandle, IOTHUB_CLIENT_HANDLE, clientHandle)
    MOCK_METHOD_END(bool, true)

    MOCK_STATIC_METHOD_2(, void, IoTHubTransport_JoinWorkerThread, TRANSPORT_HANDLE, transportHlHandle, IOTHUB_CLIENT_HANDLE, clientHandle)
    MOCK_VOID_METHOD_END()

    MOCK_STATIC_METHOD_2(, int, mallocAndStrcpy_s, char**, destination, const char*, source)
        int result2;
        if ((destination == NULL) || (source == NULL))
        {
            result2 = __LINE__;
        }
        else
        {
            size_t l = strlen(source);
            *destination = (char*)malloc(l + 1);
            if (*destination == NULL)
            {
                result2 = __LINE__;
            }
            else
            {
                memcpy(*destination, source, l+1);
                result2 = 0;
            }
        }
    MOCK_METHOD_END(int, result2);

#ifndef DONT_USE_UPLOADTOBLOB
    MOCK_STATIC_METHOD_4(, IOTHUB_CLIENT_RESULT, IoTHubClient_LL_UploadToBlob, IOTHUB_CLIENT_LL_HANDLE, iotHubClientHandle, const char*, destinationFileName, const unsigned char*, source, size_t, size);
    MOCK_METHOD_END(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK);
#endif
    
    /* list mocks */
    MOCK_STATIC_METHOD_0(, SINGLYLINKEDLIST_HANDLE, singlylinkedlist_create)
    MOCK_METHOD_END(SINGLYLINKEDLIST_HANDLE, TEST_LIST_HANDLE);
    MOCK_STATIC_METHOD_1(, void, singlylinkedlist_destroy, SINGLYLINKEDLIST_HANDLE, list)
    MOCK_VOID_METHOD_END();
    MOCK_STATIC_METHOD_2(, LIST_ITEM_HANDLE, singlylinkedlist_add, SINGLYLINKEDLIST_HANDLE, list, const void*, item)
        TEST_LIST_ITEM** items = (TEST_LIST_ITEM**)realloc(list_items, (list_item_count + 1) * sizeof(TEST_LIST_ITEM*));
        LIST_ITEM_HANDLE to_return = NULL;
        if (items != NULL)
        {
            list_items = items;
            list_items[list_item_count] = (TEST_LIST_ITEM*)malloc(sizeof(TEST_LIST_ITEM));
            if (list_items[list_item_count] != NULL)
            {
                list_items[list_item_count]->item_value = item;
                to_return = (LIST_ITEM_HANDLE)list_items[list_item_count];
                list_item_count++;
            }
        }
    MOCK_METHOD_END(LIST_ITEM_HANDLE, to_return);

    MOCK_STATIC_METHOD_1(, const void*, singlylinkedlist_item_get_value, LIST_ITEM_HANDLE, item_handle)
    MOCK_METHOD_END(const void*, ((TEST_LIST_ITEM*)item_handle)->item_value);

    MOCK_STATIC_METHOD_3(, LIST_ITEM_HANDLE, singlylinkedlist_find, SINGLYLINKEDLIST_HANDLE, handle, LIST_MATCH_FUNCTION, match_function, const void*, match_context)
        size_t i;
    LIST_ITEM_HANDLE found_item = NULL;
    for (i = 0; i < list_item_count; i++)
    {
        if (match_function((LIST_ITEM_HANDLE)list_items[i], match_context))
        {
            found_item = (LIST_ITEM_HANDLE)list_items[i];
            break;
        }
    }
    MOCK_METHOD_END(LIST_ITEM_HANDLE, found_item);
    MOCK_STATIC_METHOD_2(, int, singlylinkedlist_remove, SINGLYLINKEDLIST_HANDLE, list, LIST_ITEM_HANDLE, list_item)
        size_t i;
    for (i = 0; i < list_item_count; i++)
    {
        if (((LIST_ITEM_HANDLE)list_items[i]) == list_item)
        {
            break;
        }
    }
    if (i < list_item_count)
    {
        free(list_items[i]);
        (void)memmove(&list_items[i], &list_items[i + 1], (list_item_count - i - 1) * sizeof(TEST_LIST_ITEM*));
        list_item_count--;
        if (list_item_count == 0)
        {
            free(list_items);
            list_items = NULL;
        }
    }
    MOCK_METHOD_END(int, 0);

    MOCK_STATIC_METHOD_1(, LIST_ITEM_HANDLE, singlylinkedlist_get_head_item, SINGLYLINKEDLIST_HANDLE, list)
        LIST_ITEM_HANDLE result2;
        if (list_items == NULL)
        {
            result2 = NULL;
        }
        else
        {
            result2 = (LIST_ITEM_HANDLE)list_items[0];
        }
    MOCK_METHOD_END(LIST_ITEM_HANDLE, result2);

    MOCK_STATIC_METHOD_1(, LIST_ITEM_HANDLE, singlylinkedlist_get_next_item, LIST_ITEM_HANDLE, item)
    MOCK_METHOD_END(LIST_ITEM_HANDLE, (LIST_ITEM_HANDLE)NULL);

#ifndef DONT_USE_UPLOADTOBLOB
    MOCK_STATIC_METHOD_2(, void, uploadToBlobAsyncCallback, IOTHUB_CLIENT_FILE_UPLOAD_RESULT, result3, void*, userContextCallback)
    MOCK_VOID_METHOD_END()
#endif

};

DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubClientMocks, , IOTHUB_CLIENT_LL_HANDLE, IoTHubClient_LL_CreateFromConnectionString, const char*, connectionString, IOTHUB_CLIENT_TRANSPORT_PROVIDER, protocol);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubClientMocks, , IOTHUB_CLIENT_LL_HANDLE, IoTHubClient_LL_Create, const IOTHUB_CLIENT_CONFIG*, config);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubClientMocks, , IOTHUB_CLIENT_LL_HANDLE, IoTHubClient_LL_CreateWithTransport, const IOTHUB_CLIENT_DEVICE_CONFIG*, config);

DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubClientMocks, , void, IoTHubClient_LL_Destroy, IOTHUB_CLIENT_LL_HANDLE, iotHubClientHandle);
DECLARE_GLOBAL_MOCK_METHOD_4(CIoTHubClientMocks, , IOTHUB_CLIENT_RESULT, IoTHubClient_LL_SendEventAsync, IOTHUB_CLIENT_LL_HANDLE, iotHubClientHandle, IOTHUB_MESSAGE_HANDLE, eventMessageHandle, IOTHUB_CLIENT_EVENT_CONFIRMATION_CALLBACK, eventConfirmationCallback, void*, userContextCallback)
DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubClientMocks, , IOTHUB_CLIENT_RESULT, IoTHubClient_LL_SetMessageCallback, IOTHUB_CLIENT_LL_HANDLE, iotHubClientHandle, IOTHUB_CLIENT_MESSAGE_CALLBACK_ASYNC, messageCallback, void*, userContextCallback)
DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubClientMocks, , IOTHUB_CLIENT_RESULT, IoTHubClient_LL_SetConnectionStatusCallback, IOTHUB_CLIENT_LL_HANDLE, iotHubClientHandle, IOTHUB_CLIENT_CONNECTION_STATUS_CALLBACK, connectionStatusCallback, void*, userContextCallback)
DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubClientMocks, , IOTHUB_CLIENT_RESULT, IoTHubClient_LL_SetRetryPolicy, IOTHUB_CLIENT_LL_HANDLE, iotHubClientHandle, IOTHUB_CLIENT_RETRY_POLICY, retryPolicy, size_t, retryTimeoutLimitInSeconds)
DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubClientMocks, , IOTHUB_CLIENT_RESULT, IoTHubClient_LL_GetRetryPolicy, IOTHUB_CLIENT_LL_HANDLE, iotHubClientHandle, IOTHUB_CLIENT_RETRY_POLICY*, retryPolicy, size_t*, retryTimeoutLimitInSeconds)
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
DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubClientMocks, , void, connectionStatusCallback, IOTHUB_CLIENT_CONNECTION_STATUS, result3, IOTHUB_CLIENT_CONNECTION_STATUS_REASON, reason, void*, userContextCallback);

DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubClientMocks, , LOCK_HANDLE, IoTHubTransport_GetLock, TRANSPORT_HANDLE, transportHlHandle);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubClientMocks, , TRANSPORT_LL_HANDLE, IoTHubTransport_GetLLTransport, TRANSPORT_HANDLE, transportHlHandle);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubClientMocks, , IOTHUB_CLIENT_RESULT, IoTHubTransport_StartWorkerThread, TRANSPORT_HANDLE, transportHlHandle, IOTHUB_CLIENT_HANDLE, clientHandle);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubClientMocks, , bool, IoTHubTransport_SignalEndWorkerThread, TRANSPORT_HANDLE, transportHlHandle, IOTHUB_CLIENT_HANDLE, clientHandle);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubClientMocks, , void, IoTHubTransport_JoinWorkerThread, TRANSPORT_HANDLE, transportHlHandle, IOTHUB_CLIENT_HANDLE, clientHandle);

DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubClientMocks, , int, mallocAndStrcpy_s, char**, destination, const char*, source);

DECLARE_GLOBAL_MOCK_METHOD_6(CIoTHubClientMocks, , void, getDesiredCallback, IOTHUB_CLIENT_CONFIRMATION_RESULT, result, const unsigned char*, desiredState, size_t, size, uint32_t, desiredVersion, uint32_t, lastSeenReportedVersion, void*, userContextCallback);

DECLARE_GLOBAL_MOCK_METHOD_4(CIoTHubClientMocks, , void, deviceTwinCallback, DEVICE_TWIN_UPDATE_STATE, update_state, const unsigned char*, payLoad, size_t, size, void*, userContextCallback)

DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubClientMocks, , IOTHUB_CLIENT_RESULT, IoTHubClient_LL_SetDeviceTwinCallback, IOTHUB_CLIENT_LL_HANDLE, iotHubClientHandle, IOTHUB_CLIENT_DEVICE_TWIN_CALLBACK, deviceTwinCallback, void*, userContextCallback)

DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubClientMocks, , void, sendReportedCallback, int, status_code, void*, userContextCallback);
DECLARE_GLOBAL_MOCK_METHOD_5(CIoTHubClientMocks, , IOTHUB_CLIENT_RESULT, IoTHubClient_LL_SendReportedState, IOTHUB_CLIENT_LL_HANDLE, iotHubClientHandle, const unsigned char*, reportedState, size_t, size, IOTHUB_CLIENT_REPORTED_STATE_CALLBACK, reportedStateCallback, void*, userContextCallback);

DECLARE_GLOBAL_MOCK_METHOD_6(CIoTHubClientMocks, , int, deviceMethodCallback, const char*, method_name, const unsigned char*, payLoad, size_t, size, unsigned char**, response, size_t*, resp_size, void*, userContextCallback)
DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubClientMocks, , IOTHUB_CLIENT_RESULT, IoTHubClient_LL_SetDeviceMethodCallback, IOTHUB_CLIENT_LL_HANDLE, iotHubClientHandle, IOTHUB_CLIENT_DEVICE_METHOD_CALLBACK_ASYNC, deviceMethodCallback, void*, userContextCallback);

#ifndef DONT_USE_UPLOADTOBLOB
DECLARE_GLOBAL_MOCK_METHOD_4(CIoTHubClientMocks, , IOTHUB_CLIENT_RESULT, IoTHubClient_LL_UploadToBlob, IOTHUB_CLIENT_LL_HANDLE, iotHubClientHandle, const char*, destinationFileName, const unsigned char*, source, size_t, size);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubClientMocks, , void, uploadToBlobAsyncCallback, IOTHUB_CLIENT_FILE_UPLOAD_RESULT, result, void*, userContextCallback);
#endif

DECLARE_GLOBAL_MOCK_METHOD_0(CIoTHubClientMocks, , SINGLYLINKEDLIST_HANDLE, singlylinkedlist_create);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubClientMocks, , void, singlylinkedlist_destroy, SINGLYLINKEDLIST_HANDLE, list);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubClientMocks, , LIST_ITEM_HANDLE, singlylinkedlist_add, SINGLYLINKEDLIST_HANDLE, list, const void*, item);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubClientMocks, , const void*, singlylinkedlist_item_get_value, LIST_ITEM_HANDLE, item_handle);
DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubClientMocks, , LIST_ITEM_HANDLE, singlylinkedlist_find, SINGLYLINKEDLIST_HANDLE, handle, LIST_MATCH_FUNCTION, match_function, const void*, match_context);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubClientMocks, , int, singlylinkedlist_remove, SINGLYLINKEDLIST_HANDLE, list, LIST_ITEM_HANDLE, list_item);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubClientMocks, , LIST_ITEM_HANDLE, singlylinkedlist_get_head_item, SINGLYLINKEDLIST_HANDLE, list);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubClientMocks, , LIST_ITEM_HANDLE, singlylinkedlist_get_next_item, LIST_ITEM_HANDLE, item);

static const TRANSPORT_PROVIDER* provideFAKE(void)
{
    return NULL;
}

BEGIN_TEST_SUITE(iothubclient_ut)

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
        currentmalloc_call = 0;
        whenShallmalloc_fail = 0;
        howManyDoWorkCalls = 0;
        doWorkCallCount = 0;
        threadFunc = NULL;
        threadFuncArg = NULL;
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
    /*Tests_SRS_IOTHUBCLIENT_02_059: [ IoTHubClient_CreateFromConnectionString shall create a SINGLYLINKEDLIST_HANDLE containing informations saved by IoTHubClient_UploadToBlobAsync. ]*/
    TEST_FUNCTION(IoTHubClient_CreateFromConnectionString_succeeds)
    {
        // arrange
        CIoTHubClientMocks mocks;
        EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG));
        STRICT_EXPECTED_CALL(mocks, Lock_Init());
#ifndef DONT_USE_UPLOADTOBLOB
        STRICT_EXPECTED_CALL(mocks, singlylinkedlist_create());
#endif
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

#ifndef DONT_USE_UPLOADTOBLOB
    /*Tests_SRS_IOTHUBCLIENT_02_070: [ If creating the SINGLYLINKEDLIST_HANDLE fails then IoTHubClient_CreateFromConnectionString shall fail and return NULL]*/
    TEST_FUNCTION(IoTHubClient_CreateFromConnectionString_if_list_create_fails_then_it_fails)
    {
        // arrange
        CIoTHubClientMocks mocks;
        EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG));
        EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG));

        STRICT_EXPECTED_CALL(mocks, Lock_Init());
        STRICT_EXPECTED_CALL(mocks, Lock_Deinit(TEST_LOCK_HANDLE));

        STRICT_EXPECTED_CALL(mocks, singlylinkedlist_create())
            .SetReturn((SINGLYLINKEDLIST_HANDLE)NULL);
        
        // act
        IOTHUB_CLIENT_HANDLE iotHubClient = IoTHubClient_CreateFromConnectionString(TEST_CHAR, provideFAKE);

        // assert
        ASSERT_IS_NULL(iotHubClient);
        mocks.AssertActualAndExpectedCalls();
    }
#endif

    /* Tests_SRS_IOTHUBCLIENT_12_010: [If IoTHubClient_LL_CreateFromConnectionString fails then IoTHubClient_CreateFromConnectionString shall do clean - up and return NULL] */
    TEST_FUNCTION(IoTHubClient_CreateFromConnectionString_if_IoTHubClient_LL_CreateFromConnectionString_fails_then_IoTHubClient_CreateFromConnectionString_do_clean_up_and_fails)
    {
        // arrange
        CIoTHubClientMocks mocks;
        EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG));

        STRICT_EXPECTED_CALL(mocks, Lock_Init());
#ifndef DONT_USE_UPLOADTOBLOB
        STRICT_EXPECTED_CALL(mocks, singlylinkedlist_create());
        STRICT_EXPECTED_CALL(mocks, singlylinkedlist_destroy(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
#endif
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
    /*Tests_SRS_IOTHUBCLIENT_02_060: [ IoTHubClient_Create shall create a SINGLYLINKEDLIST_HANDLE that shall be used beIoTHubClient_UploadToBlobAsync. ]*/
    TEST_FUNCTION(IoTHubClient_Create_with_valid_arguments_when_all_underlying_calls_are_OK_succeeds)
    {
        // arrange
        CIoTHubClientMocks mocks;
        EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG));
        STRICT_EXPECTED_CALL(mocks, Lock_Init());
#ifndef DONT_USE_UPLOADTOBLOB
        STRICT_EXPECTED_CALL(mocks, singlylinkedlist_create());
#endif
        STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_Create(&TEST_CONFIG));

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
#ifndef DONT_USE_UPLOADTOBLOB
        STRICT_EXPECTED_CALL(mocks, singlylinkedlist_create());
#endif

        STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_Create(&TEST_CONFIG))
            .SetReturn((IOTHUB_CLIENT_LL_HANDLE)NULL);
        STRICT_EXPECTED_CALL(mocks, Lock_Deinit(TEST_LOCK_HANDLE));
#ifndef DONT_USE_UPLOADTOBLOB
        STRICT_EXPECTED_CALL(mocks, singlylinkedlist_destroy(TEST_LIST_HANDLE));
#endif
        EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG));

        // act
        IOTHUB_CLIENT_HANDLE iotHubClient = IoTHubClient_Create(&TEST_CONFIG);

        // assert
        ASSERT_IS_NULL(iotHubClient);
        mocks.AssertActualAndExpectedCalls();
    }

#ifndef DONT_USE_UPLOADTOBLOB
    /*Tests_SRS_IOTHUBCLIENT_02_061: [ If creating the SINGLYLINKEDLIST_HANDLE fails then IoTHubClient_Create shall fail and return NULL. ]*/
    TEST_FUNCTION(IoTHubClient_LL_Create_fails_when_list_create_fails)
    {
        // arrange
        CIoTHubClientMocks mocks;
        EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG));
        EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG));

        STRICT_EXPECTED_CALL(mocks, Lock_Init());
        STRICT_EXPECTED_CALL(mocks, Lock_Deinit(TEST_LOCK_HANDLE));

        STRICT_EXPECTED_CALL(mocks, singlylinkedlist_create())
            .SetFailReturn((SINGLYLINKEDLIST_HANDLE)NULL);

        // act
        IOTHUB_CLIENT_HANDLE iotHubClient = IoTHubClient_Create(&TEST_CONFIG);

        // assert
        ASSERT_IS_NULL(iotHubClient);
        mocks.AssertActualAndExpectedCalls();
    }
#endif

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

    //Tests_SRS_IOTHUBCLIENT_17_013: [ IoTHubClient_CreateWithTransport shall return NULL if transportHandle is NULL. ]
    TEST_FUNCTION(When_creating_with_transport_null_transport_returns_null)
    {
        // arrange
        CIoTHubClientMocks mocks;

        // act
        IOTHUB_CLIENT_HANDLE iotHubClient = IoTHubClient_CreateWithTransport(NULL,&TEST_CONFIG);

        // assert
        ASSERT_IS_NULL(iotHubClient);
        mocks.AssertActualAndExpectedCalls();
    }

    //Tests_SRS_IOTHUBCLIENT_17_014: [ IoTHubClient_CreateWithTransport shall return NULL if config is NULL. ]
    TEST_FUNCTION(When_creating_with_transport_null_config_returns_null)
    {
        // arrange
        CIoTHubClientMocks mocks;

        // act
        IOTHUB_CLIENT_HANDLE iotHubClient = IoTHubClient_CreateWithTransport(TEST_IOTHUBTRANSPORT_HANDLE,NULL);

        // assert
        ASSERT_IS_NULL(iotHubClient);
        mocks.AssertActualAndExpectedCalls();
    }

    //Tests_SRS_IOTHUBCLIENT_17_001: [ IoTHubClient_CreateWithTransport shall allocate a new IoTHubClient instance and return a non-NULL handle to it. ]
    //Tests_SRS_IOTHUBCLIENT_17_003: [ IoTHubClient_CreateWithTransport shall call IoTHubTransport_HL_GetLLTransport on transportHandle to get lower layer transport. ]
    //Tests_SRS_IOTHUBCLIENT_17_005: [ IoTHubClient_CreateWithTransport shall call IoTHubTransport_GetLock to get the transport lock to be used later for serializing IoTHubClient calls. ]
    //Tests_SRS_IOTHUBCLIENT_17_007: [ IoTHubClient_CreateWithTransport shall instantiate a new IoTHubClient_LL instance by calling IoTHubClient_LL_CreateWithTransport and passing the lower layer transport and config argument. ]
    /*Tests_SRS_IOTHUBCLIENT_02_073: [ IoTHubClient_CreateWithTransport shall create a SINGLYLINKEDLIST_HANDLE that shall be used by IoTHubClient_UploadToBlobAsync. ]*/
    TEST_FUNCTION(When_creating_with_transport_success_returns_non_null)
    {
        // arrange
        CIoTHubClientMocks mocks;

        STRICT_EXPECTED_CALL(mocks, Lock(TEST_IOTHUBTRANSPORT_LOCK));
        STRICT_EXPECTED_CALL(mocks, Unlock(TEST_IOTHUBTRANSPORT_LOCK));

        EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG));

#ifndef DONT_USE_UPLOADTOBLOB
        STRICT_EXPECTED_CALL(mocks, singlylinkedlist_create()); /*this is the list of SAVED_DATA*/
#endif

        STRICT_EXPECTED_CALL(mocks, IoTHubTransport_GetLock(TEST_IOTHUBTRANSPORT_HANDLE));
        STRICT_EXPECTED_CALL(mocks, IoTHubTransport_GetLLTransport(TEST_IOTHUBTRANSPORT_HANDLE));
        STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_CreateWithTransport(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        // act
        IOTHUB_CLIENT_HANDLE iotHubClient = IoTHubClient_CreateWithTransport(TEST_IOTHUBTRANSPORT_HANDLE, &TEST_CONFIG);

        // assert
        ASSERT_IS_NOT_NULL(iotHubClient);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubClient_Destroy(iotHubClient);
    }

    TEST_FUNCTION(When_creating_with_transport_Unlock_fails_non_null)
    {
        // arrange
        CIoTHubClientMocks mocks;

        STRICT_EXPECTED_CALL(mocks, Lock(TEST_IOTHUBTRANSPORT_LOCK));
        STRICT_EXPECTED_CALL(mocks, Unlock(TEST_IOTHUBTRANSPORT_LOCK))
            .SetFailReturn((LOCK_RESULT)LOCK_ERROR);

        EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG));

#ifndef DONT_USE_UPLOADTOBLOB
        STRICT_EXPECTED_CALL(mocks, singlylinkedlist_create()); /*this is the list of SAVED_DATA*/
#endif

        STRICT_EXPECTED_CALL(mocks, IoTHubTransport_GetLock(TEST_IOTHUBTRANSPORT_HANDLE));
        STRICT_EXPECTED_CALL(mocks, IoTHubTransport_GetLLTransport(TEST_IOTHUBTRANSPORT_HANDLE));
        STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_CreateWithTransport(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        // act
        IOTHUB_CLIENT_HANDLE iotHubClient = IoTHubClient_CreateWithTransport(TEST_IOTHUBTRANSPORT_HANDLE, &TEST_CONFIG);

        // assert
        ASSERT_IS_NOT_NULL(iotHubClient);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubClient_Destroy(iotHubClient);
    }

    //Tests_SRS_IOTHUBCLIENT_17_008: [ If IoTHubClient_LL_CreateWithTransport fails, then IoTHubClient_Create shall return NULL. ]
    //Tests_SRS_IOTHUBCLIENT_17_009: [ If IoTHubClient_LL_CreateWithTransport fails, all resources allocated by it shall be freed. ]
    TEST_FUNCTION(When_creating_with_transport_IoTHubClient_LL_CreateWithTransport_fails_returns_null)
    {
        // arrange
        CIoTHubClientMocks mocks;

        STRICT_EXPECTED_CALL(mocks, Lock(TEST_IOTHUBTRANSPORT_LOCK));
        STRICT_EXPECTED_CALL(mocks, Unlock(TEST_IOTHUBTRANSPORT_LOCK));

        EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG));
        EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG));

#ifndef DONT_USE_UPLOADTOBLOB
        STRICT_EXPECTED_CALL(mocks, singlylinkedlist_create()); /*this is the list of SAVED_DATA*/
        STRICT_EXPECTED_CALL(mocks, singlylinkedlist_destroy(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
#endif

        STRICT_EXPECTED_CALL(mocks, IoTHubTransport_GetLock(TEST_IOTHUBTRANSPORT_HANDLE));
        STRICT_EXPECTED_CALL(mocks, IoTHubTransport_GetLLTransport(TEST_IOTHUBTRANSPORT_HANDLE));
        STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_CreateWithTransport(IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .SetFailReturn((IOTHUB_CLIENT_LL_HANDLE)NULL);

        // act
        IOTHUB_CLIENT_HANDLE iotHubClient = IoTHubClient_CreateWithTransport(TEST_IOTHUBTRANSPORT_HANDLE, &TEST_CONFIG);

        // assert
        ASSERT_IS_NULL(iotHubClient);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
    }

    TEST_FUNCTION(When_creating_with_transport_Lock_fails_returns_null)
    {
        // arrange
        CIoTHubClientMocks mocks;

        STRICT_EXPECTED_CALL(mocks, Lock(TEST_IOTHUBTRANSPORT_LOCK))
            .SetFailReturn((LOCK_RESULT)LOCK_ERROR);

        EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG));
        EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG));

#ifndef DONT_USE_UPLOADTOBLOB
        STRICT_EXPECTED_CALL(mocks, singlylinkedlist_create()); /*this is the list of SAVED_DATA*/
        STRICT_EXPECTED_CALL(mocks, singlylinkedlist_destroy(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
#endif

        STRICT_EXPECTED_CALL(mocks, IoTHubTransport_GetLock(TEST_IOTHUBTRANSPORT_HANDLE));
        STRICT_EXPECTED_CALL(mocks, IoTHubTransport_GetLLTransport(TEST_IOTHUBTRANSPORT_HANDLE));

        // act
        IOTHUB_CLIENT_HANDLE iotHubClient = IoTHubClient_CreateWithTransport(TEST_IOTHUBTRANSPORT_HANDLE, &TEST_CONFIG);

        // assert
        ASSERT_IS_NULL(iotHubClient);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
    }
    //Tests_SRS_IOTHUBCLIENT_17_004: [ If IoTHubTransport_GetLLTransport fails, then IoTHubClient_CreateWithTransport shall return NULL. ]
    TEST_FUNCTION(When_creating_with_transport_IoTHubClient_get_ll_transport_fails_returns_null)
    {
        // arrange
        CIoTHubClientMocks mocks;

        EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG));
#ifndef DONT_USE_UPLOADTOBLOB
        STRICT_EXPECTED_CALL(mocks, singlylinkedlist_create()); /*this is the list of SAVED_DATA*/
        STRICT_EXPECTED_CALL(mocks, singlylinkedlist_destroy(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
#endif

        EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(mocks, IoTHubTransport_GetLock(TEST_IOTHUBTRANSPORT_HANDLE));
        STRICT_EXPECTED_CALL(mocks, IoTHubTransport_GetLLTransport(TEST_IOTHUBTRANSPORT_HANDLE))
            .SetFailReturn((TRANSPORT_LL_HANDLE)NULL);

        // act
        IOTHUB_CLIENT_HANDLE iotHubClient = IoTHubClient_CreateWithTransport(TEST_IOTHUBTRANSPORT_HANDLE, &TEST_CONFIG);

        // assert
        ASSERT_IS_NULL(iotHubClient);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
    }

    //Tests_SRS_IOTHUBCLIENT_17_006: [ If IoTHubTransport_GetLock fails, then IoTHubClient_CreateWithTransport shall return NULL. ]
    TEST_FUNCTION(When_creating_with_transport_IoTHubClient_getLock_fails_returns_null)
    {
        // arrange
        CIoTHubClientMocks mocks;

        EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG));
        EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG));

#ifndef DONT_USE_UPLOADTOBLOB
        STRICT_EXPECTED_CALL(mocks, singlylinkedlist_create()); /*this is the list of SAVED_DATA*/
        STRICT_EXPECTED_CALL(mocks, singlylinkedlist_destroy(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
#endif

        STRICT_EXPECTED_CALL(mocks, IoTHubTransport_GetLock(TEST_IOTHUBTRANSPORT_HANDLE))
            .SetFailReturn((LOCK_HANDLE)NULL);

        // act
        IOTHUB_CLIENT_HANDLE iotHubClient = IoTHubClient_CreateWithTransport(TEST_IOTHUBTRANSPORT_HANDLE, &TEST_CONFIG);

        // assert
        ASSERT_IS_NULL(iotHubClient);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
    }

#ifndef DONT_USE_UPLOADTOBLOB
    /*Tests_SRS_IOTHUBCLIENT_02_074: [ If creating the SINGLYLINKEDLIST_HANDLE fails then IoTHubClient_CreateWithTransport shall fail and return NULL. ]*/
    TEST_FUNCTION(When_creating_with_transport_list_create_fails_returns_null)
    {
        // arrange
        CIoTHubClientMocks mocks;

        EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG));
        EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG));

        STRICT_EXPECTED_CALL(mocks, singlylinkedlist_create()) /*this is the list of SAVED_DATA*/
            .SetFailReturn((SINGLYLINKEDLIST_HANDLE)NULL);

        // act
        IOTHUB_CLIENT_HANDLE iotHubClient = IoTHubClient_CreateWithTransport(TEST_IOTHUBTRANSPORT_HANDLE, &TEST_CONFIG);

        // assert
        ASSERT_IS_NULL(iotHubClient);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
    }
#endif

    //Tests_SRS_IOTHUBCLIENT_17_002: [ If allocating memory for the new IoTHubClient instance fails, then IoTHubClient_CreateWithTransport shall return NULL. ]
    TEST_FUNCTION(When_creating_with_transport_IoTHubClient_alloc_fails_returns_null)
    {
        // arrange
        CIoTHubClientMocks mocks;

        EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
            .SetFailReturn((void_ptr)NULL);
        // act
        IOTHUB_CLIENT_HANDLE iotHubClient = IoTHubClient_CreateWithTransport(TEST_IOTHUBTRANSPORT_HANDLE, &TEST_CONFIG);

        // assert
        ASSERT_IS_NULL(iotHubClient);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
    }

    //Tests_SRS_IOTHUBCLIENT_17_012: [ If the transport connection is shared, the thread shall be started by calling IoTHubTransport_StartWorkerThread. ]
    TEST_FUNCTION(When_SendAsync_shared_transport_no_thread_created_success)
    {
        // arrange
        CIoTHubClientMocks mocks;
        IOTHUB_CLIENT_HANDLE iotHubClient = IoTHubClient_CreateWithTransport(TEST_IOTHUBTRANSPORT_HANDLE, &TEST_CONFIG);

        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Lock(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, Unlock(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, IoTHubTransport_StartWorkerThread(TEST_IOTHUBTRANSPORT_HANDLE, iotHubClient));
        STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_SendEventAsync(TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_DEVICEMESSAGE_HANDLE, eventConfirmationCallback, (void*)0x42));

        // act
        auto result = IoTHubClient_SendEventAsync(iotHubClient, TEST_DEVICEMESSAGE_HANDLE, eventConfirmationCallback, (void*)0x42);

        // assert
        ASSERT_ARE_EQUAL(int, (int)result, (int)IOTHUB_CLIENT_OK);
        ASSERT_IS_NULL((void_ptr)threadFunc);
        ASSERT_IS_NULL(threadFuncArg);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubClient_Destroy(iotHubClient);
    }

    //Tests_SRS_IOTHUBCLIENT_17_011: [ If the transport connection is shared, the thread shall be started by calling IoTHubTransport_StartWorkerThread. ]
    TEST_FUNCTION(When_SetMessageCallback_shared_transport_no_thread_created_success)
    {
        // arrange
        CIoTHubClientMocks mocks;
        IOTHUB_CLIENT_HANDLE iotHubClient = IoTHubClient_CreateWithTransport(TEST_IOTHUBTRANSPORT_HANDLE, &TEST_CONFIG);

        mocks.ResetAllCalls();
        STRICT_EXPECTED_CALL(mocks, Lock(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, Unlock(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, IoTHubTransport_StartWorkerThread(TEST_IOTHUBTRANSPORT_HANDLE, iotHubClient));
        STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_SetMessageCallback(TEST_IOTHUB_CLIENT_LL_HANDLE, messageCallback, (void*)0x42));

        // act
        auto result = IoTHubClient_SetMessageCallback(iotHubClient, messageCallback, (void*)0x42);

        // assert
        ASSERT_ARE_EQUAL(int, (int)result, (int)IOTHUB_CLIENT_OK);
        ASSERT_IS_NULL(threadFunc);
        ASSERT_IS_NULL(threadFuncArg);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubClient_Destroy(iotHubClient);
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
    /*Tests_SRS_IOTHUBCLIENT_02_069: [ IoTHubClient_Destroy shall free all data created by IoTHubClient_UploadToBlobAsync ]*/
    /*Tests_SRS_IOTHUBCLIENT_02_072: [ All threads marked as disposable (upon completion of a file upload) shall be joined and the data structures build for them shall be freed. ]*/
    TEST_FUNCTION(IoTHubClient_Destroy_frees_underlying_LL_client)
    {
        // arrange
        CIoTHubClientMocks mocks;
        IOTHUB_CLIENT_HANDLE iotHubClient = IoTHubClient_Create(&TEST_CONFIG);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
#ifndef DONT_USE_UPLOADTOBLOB
        STRICT_EXPECTED_CALL(mocks, singlylinkedlist_get_head_item(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
#endif
        STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));

        STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_Destroy(TEST_IOTHUB_CLIENT_LL_HANDLE));
#ifndef DONT_USE_UPLOADTOBLOB
        STRICT_EXPECTED_CALL(mocks, singlylinkedlist_destroy(TEST_LIST_HANDLE));
#endif
        STRICT_EXPECTED_CALL(mocks, Lock_Deinit(TEST_LOCK_HANDLE));
        EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG));

        // act
        IoTHubClient_Destroy(iotHubClient);

        // assert
        // uMock checks the calls
    }

    TEST_FUNCTION(IoTHubClient_Destroy_lock_fails_still_continues)
    {
        // arrange
        CIoTHubClientMocks mocks;
        IOTHUB_CLIENT_HANDLE iotHubClient = IoTHubClient_Create(&TEST_CONFIG);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE))
            .SetFailReturn((LOCK_RESULT)LOCK_ERROR);
#ifndef DONT_USE_UPLOADTOBLOB
        STRICT_EXPECTED_CALL(mocks, singlylinkedlist_get_head_item(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
#endif
        STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE))
            .SetFailReturn((LOCK_RESULT)LOCK_ERROR);

        STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_Destroy(TEST_IOTHUB_CLIENT_LL_HANDLE));
#ifndef DONT_USE_UPLOADTOBLOB
        STRICT_EXPECTED_CALL(mocks, singlylinkedlist_destroy(TEST_LIST_HANDLE));
#endif
        STRICT_EXPECTED_CALL(mocks, Lock_Deinit(TEST_LOCK_HANDLE));
        EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG));

        // act
        IoTHubClient_Destroy(iotHubClient);

        // assert
        // uMock checks the calls
    }

    TEST_FUNCTION(IoTHubClient_Destroy_shared_transport_frees_LL_client)
    {
        // arrange
        CIoTHubClientMocks mocks;
        IOTHUB_CLIENT_HANDLE iotHubClient = IoTHubClient_CreateWithTransport(TEST_IOTHUBTRANSPORT_HANDLE, &TEST_CONFIG);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Lock(TEST_IOTHUBTRANSPORT_LOCK));
#ifndef DONT_USE_UPLOADTOBLOB
        STRICT_EXPECTED_CALL(mocks, singlylinkedlist_get_head_item(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, singlylinkedlist_destroy(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
#endif
        STRICT_EXPECTED_CALL(mocks, Unlock(TEST_IOTHUBTRANSPORT_LOCK));

        STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_Destroy(TEST_IOTHUB_CLIENT_LL_HANDLE));
        STRICT_EXPECTED_CALL(mocks, IoTHubTransport_SignalEndWorkerThread(TEST_IOTHUBTRANSPORT_HANDLE, iotHubClient));
        STRICT_EXPECTED_CALL(mocks, IoTHubTransport_JoinWorkerThread(TEST_IOTHUBTRANSPORT_HANDLE, iotHubClient));
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

        STRICT_EXPECTED_CALL(mocks, Lock(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        /*here StopThread=1 is set*/
#ifndef DONT_USE_UPLOADTOBLOB
        STRICT_EXPECTED_CALL(mocks, singlylinkedlist_get_head_item(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
#endif
        STRICT_EXPECTED_CALL(mocks, Unlock(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, ThreadAPI_Join(TEST_THREAD_HANDLE, IGNORED_PTR_ARG))
            .IgnoreArgument(2);
        STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_Destroy(TEST_IOTHUB_CLIENT_LL_HANDLE));
#ifndef DONT_USE_UPLOADTOBLOB
        STRICT_EXPECTED_CALL(mocks, singlylinkedlist_destroy(TEST_LIST_HANDLE));
#endif
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

        STRICT_EXPECTED_CALL(mocks, Lock(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        /*here StopThread=1 is set*/
#ifndef DONT_USE_UPLOADTOBLOB
        STRICT_EXPECTED_CALL(mocks, singlylinkedlist_get_head_item(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
#endif
        STRICT_EXPECTED_CALL(mocks, Unlock(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, ThreadAPI_Join(TEST_THREAD_HANDLE, IGNORED_PTR_ARG))
            .IgnoreArgument(2);
        STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_Destroy(TEST_IOTHUB_CLIENT_LL_HANDLE));
        STRICT_EXPECTED_CALL(mocks, Lock_Deinit(TEST_LOCK_HANDLE));
#ifndef DONT_USE_UPLOADTOBLOB
        STRICT_EXPECTED_CALL(mocks, singlylinkedlist_destroy(TEST_LIST_HANDLE));
#endif

        EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG));

        // act
        IoTHubClient_Destroy(iotHubClient);

        // assert
        // uMock checks the calls
    }

    /* Tests_SRS_IOTHUBCLIENT_01_007: [The thread created as part of executing IoTHubClient_SendEventAsync or IoTHubClient_SetMessageCallback shall be joined.] */
    /*Tests_SRS_IOTHUBCLIENT_02_043: [IoTHubClient_Destroy shall lock the serializing lock.]*/
    /*Tests_SRS_IOTHUBCLIENT_02_045: [ IoTHubClient_Destroy shall unlock the serializing lock. ]*/
    TEST_FUNCTION(When_Thread_Join_Fails_The_Rest_Of_Resources_Are_Still_Freed)
    {
        // arrange
        CIoTHubClientMocks mocks;
        IOTHUB_CLIENT_HANDLE iotHubClient = IoTHubClient_Create(&TEST_CONFIG);
        (void)IoTHubClient_SetMessageCallback(iotHubClient, messageCallback, (void*)0x42);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Lock(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        /*here StopThread=1 is set*/
#ifndef DONT_USE_UPLOADTOBLOB
        STRICT_EXPECTED_CALL(mocks, singlylinkedlist_get_head_item(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
#endif
        STRICT_EXPECTED_CALL(mocks, Unlock(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, ThreadAPI_Join(TEST_THREAD_HANDLE, IGNORED_PTR_ARG))
            .IgnoreArgument(2)
            .SetReturn(THREADAPI_ERROR);

        STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_Destroy(TEST_IOTHUB_CLIENT_LL_HANDLE));
        STRICT_EXPECTED_CALL(mocks, Lock_Deinit(TEST_LOCK_HANDLE));
#ifndef DONT_USE_UPLOADTOBLOB
        STRICT_EXPECTED_CALL(mocks, singlylinkedlist_destroy(TEST_LIST_HANDLE));
#endif
        EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG));

        // act
        IoTHubClient_Destroy(iotHubClient);

        // assert
        // uMock checks the calls
    }

    /* Tests_SRS_IOTHUBCLIENT_01_007: [The thread created as part of executing IoTHubClient_SendEventAsync or IoTHubClient_SetMessageCallback shall be joined.] */
    /*Tests_SRS_IOTHUBCLIENT_02_043: [IoTHubClient_Destroy shall lock the serializing lock and signal the worker thread (if any) to end.]*/
    /*Tests_SRS_IOTHUBCLIENT_02_045: [ IoTHubClient_Destroy shall unlock the serializing lock. ]*/
    TEST_FUNCTION(When_Thread_Join_succeeds_with_condition_The_Rest_Of_Resources_Are_Still_Freed)
    {
        // arrange
        CIoTHubClientMocks mocks;
        IOTHUB_CLIENT_HANDLE iotHubClient = IoTHubClient_Create(&TEST_CONFIG);
        (void)IoTHubClient_SetMessageCallback(iotHubClient, messageCallback, (void*)0x42);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Lock(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
#ifndef DONT_USE_UPLOADTOBLOB
        STRICT_EXPECTED_CALL(mocks, singlylinkedlist_get_head_item(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
#endif
        STRICT_EXPECTED_CALL(mocks, Unlock(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, ThreadAPI_Join(TEST_THREAD_HANDLE, IGNORED_PTR_ARG))
            .IgnoreArgument(2);

        STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_Destroy(TEST_IOTHUB_CLIENT_LL_HANDLE));
        STRICT_EXPECTED_CALL(mocks, Lock_Deinit(TEST_LOCK_HANDLE));
#ifndef DONT_USE_UPLOADTOBLOB
        STRICT_EXPECTED_CALL(mocks, singlylinkedlist_destroy(TEST_LIST_HANDLE));
#endif
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

    /* Tests_SRS_IOTHUBCLIENT_25_084: [ If `iotHubClientHandle` is `NULL`, `IoTHubClient_SetConnectionStatusCallback` shall return `IOTHUB_CLIENT_INVALID_ARG`.]*/
    TEST_FUNCTION(IoTHubClient_SetConnectionStatusCallback_with_NULL_handle_fails)
    {
        // arrange
        CIoTHubClientMocks mocks;

        // act
        IOTHUB_CLIENT_RESULT result = IoTHubClient_SetConnectionStatusCallback(NULL, connectionStatusCallback, (void*)0x42);

        // assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, result, IOTHUB_CLIENT_INVALID_ARG);
    }

    /* Tests_SRS_IOTHUBCLIENT_25_087: [ `IoTHubClient_SetConnectionStatusCallback` shall be made thread-safe by using the lock created in `IoTHubClient_Create`. ]*/
    /* Tests_SRS_IOTHUBCLIENT_25_086: [ When `IoTHubClient_LL_SetConnectionStatusCallback` is called, `IoTHubClient_SetConnectionStatusCallback` shall return the result of `IoTHubClient_LL_SetConnectionStatusCallback`.]*/
    TEST_FUNCTION(IoTHubClient_SetConnectionStatusCallback_returns_the_result_of_calling_IoTHubClient_LL_SetConnectionStatusCallback)
    {
        // arrange
        CIoTHubClientMocks mocks;
        IOTHUB_CLIENT_HANDLE iotHubClient = IoTHubClient_Create(&TEST_CONFIG);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
        EXPECTED_CALL(mocks, ThreadAPI_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_SetConnectionStatusCallback(TEST_IOTHUB_CLIENT_LL_HANDLE, connectionStatusCallback, (void*)0x42))
            .SetReturn(IOTHUB_CLIENT_INVALID_SIZE);
        STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));

        // act
        IOTHUB_CLIENT_RESULT result = IoTHubClient_SetConnectionStatusCallback(iotHubClient, connectionStatusCallback, (void*)0x42);

        // assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, result, IOTHUB_CLIENT_INVALID_SIZE);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        IoTHubClient_Destroy(iotHubClient);
    }

    TEST_FUNCTION(IoTHubClient_SetConnectionStatusCallback_starts_the_worker_thread_and_calls_the_underlayer)
    {
        // arrange
        CIoTHubClientMocks mocks;
        IOTHUB_CLIENT_HANDLE iotHubClient = IoTHubClient_Create(&TEST_CONFIG);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
        EXPECTED_CALL(mocks, ThreadAPI_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_SetConnectionStatusCallback(TEST_IOTHUB_CLIENT_LL_HANDLE, connectionStatusCallback, (void*)0x42));
        STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));

        // act
        IOTHUB_CLIENT_RESULT result = IoTHubClient_SetConnectionStatusCallback(iotHubClient, connectionStatusCallback, (void*)0x42);

        // assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        IoTHubClient_Destroy(iotHubClient);
    }

    /* Tests_SRS_IOTHUBCLIENT_25_092: [ If `iotHubClientHandle` is `NULL`, `IoTHubClient_GetRetryPolicy` shall return `IOTHUB_CLIENT_INVALID_ARG`. ]*/
    TEST_FUNCTION(IoTHubClient_GetRetryPolicy_with_NULL_handle_fails)
    {
        // arrange
        CIoTHubClientMocks mocks;

        // act
        IOTHUB_CLIENT_RESULT result = IoTHubClient_GetRetryPolicy(NULL, IGNORED_PTR_ARG, IGNORED_PTR_ARG);

        // assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, result, IOTHUB_CLIENT_INVALID_ARG);
    }

    /* Tests_SRS_IOTHUBCLIENT_25_094: [ When `IoTHubClient_LL_GetRetryPolicy` is called, `IoTHubClient_GetRetryPolicy` shall return the result of `IoTHubClient_LL_GetRetryPolicy`.]*/
    TEST_FUNCTION(IoTHubClient_GetRetryPolicy_returns_the_result_of_calling_IoTHubClient_LL_GetRetryPolicy)
    {
        // arrange
        CIoTHubClientMocks mocks;
        IOTHUB_CLIENT_HANDLE iotHubClient = IoTHubClient_Create(&TEST_CONFIG);
        IoTHubClient_SetRetryPolicy(iotHubClient, TEST_RETRY_POLICY, TEST_RETRY_TIMEOUT_SECS);
        IOTHUB_CLIENT_RETRY_POLICY policy;
        size_t retryTimeout;
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
        STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_GetRetryPolicy(TEST_IOTHUB_CLIENT_LL_HANDLE, &policy, &retryTimeout))
            .SetReturn(IOTHUB_CLIENT_INVALID_SIZE);
        STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));

        // act
        IOTHUB_CLIENT_RESULT result = IoTHubClient_GetRetryPolicy(iotHubClient, &policy, &retryTimeout);

        // assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, result, IOTHUB_CLIENT_INVALID_SIZE);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        IoTHubClient_Destroy(iotHubClient);
    }

    TEST_FUNCTION(IoTHubClient_GetRetryPolicy_starts_the_worker_thread_and_calls_the_underlayer)
    {
        // arrange
        CIoTHubClientMocks mocks;
        IOTHUB_CLIENT_HANDLE iotHubClient = IoTHubClient_Create(&TEST_CONFIG);
        IoTHubClient_SetRetryPolicy(iotHubClient, TEST_RETRY_POLICY, TEST_RETRY_TIMEOUT_SECS);
        IOTHUB_CLIENT_RETRY_POLICY policy;
        size_t retryTimeout;
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
        STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_GetRetryPolicy(TEST_IOTHUB_CLIENT_LL_HANDLE, &policy, &retryTimeout));
        STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));

        // act
        IOTHUB_CLIENT_RESULT result = IoTHubClient_GetRetryPolicy(iotHubClient, &policy, &retryTimeout);

        // assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK, result);
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RETRY_POLICY, policy, TEST_RETRY_POLICY);
        ASSERT_ARE_EQUAL(size_t, retryTimeout, TEST_RETRY_TIMEOUT_SECS);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        IoTHubClient_Destroy(iotHubClient);
    }

    /* Tests_SRS_IOTHUBCLIENT_25_076: [ If `iotHubClientHandle` is `NULL`, `IoTHubClient_SetRetryPolicy` shall return `IOTHUB_CLIENT_INVALID_ARG`. ]*/
    TEST_FUNCTION(IoTHubClient_SetRetryPolicy_with_NULL_handle_fails)
    {
        // arrange
        CIoTHubClientMocks mocks;

        // act
        IOTHUB_CLIENT_RESULT result = IoTHubClient_SetRetryPolicy(NULL, TEST_RETRY_POLICY, TEST_RETRY_TIMEOUT_SECS);

        // assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, result, IOTHUB_CLIENT_INVALID_ARG);
    }

    /* Tests_SRS_IOTHUBCLIENT_25_078: [ When `IoTHubClient_LL_SetRetryPolicy` is called, `IoTHubClient_SetRetryPolicy` shall return the result of `IoTHubClient_LL_SetRetryPolicy`. ]*/
    TEST_FUNCTION(IoTHubClient_SetRetryPolicy_returns_the_result_of_calling_IoTHubClient_LL_SetRetryPolicy)
    {
        // arrange
        CIoTHubClientMocks mocks;
        IOTHUB_CLIENT_HANDLE iotHubClient = IoTHubClient_Create(&TEST_CONFIG);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
        EXPECTED_CALL(mocks, ThreadAPI_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_SetRetryPolicy(TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_RETRY_POLICY, TEST_RETRY_TIMEOUT_SECS))
            .SetReturn(IOTHUB_CLIENT_INVALID_SIZE);
        STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));

        // act
        IOTHUB_CLIENT_RESULT result = IoTHubClient_SetRetryPolicy(iotHubClient, TEST_RETRY_POLICY, TEST_RETRY_TIMEOUT_SECS);

        // assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, result, IOTHUB_CLIENT_INVALID_SIZE);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        IoTHubClient_Destroy(iotHubClient);
    }


    TEST_FUNCTION(IoTHubClient_SetRetryPolicy_starts_the_worker_thread_and_calls_the_underlayer)
    {
        // arrange
        CIoTHubClientMocks mocks;
        IOTHUB_CLIENT_HANDLE iotHubClient = IoTHubClient_Create(&TEST_CONFIG);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
        EXPECTED_CALL(mocks, ThreadAPI_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_SetRetryPolicy(TEST_IOTHUB_CLIENT_LL_HANDLE, TEST_RETRY_POLICY, TEST_RETRY_TIMEOUT_SECS));
        STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));

        // act
        IOTHUB_CLIENT_RESULT result = IoTHubClient_SetRetryPolicy(iotHubClient, TEST_RETRY_POLICY, TEST_RETRY_TIMEOUT_SECS);

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
    /*Tests_SRS_IOTHUBCLIENT_02_072: [ All threads marked as disposable (upon completion of a file upload) shall be joined and the data structures build for them shall be freed. ]*/
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
#ifndef DONT_USE_UPLOADTOBLOB
        STRICT_EXPECTED_CALL(mocks, singlylinkedlist_get_head_item(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
#endif
        STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));
        
        STRICT_EXPECTED_CALL(mocks, ThreadAPI_Sleep(1));
        STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
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
#ifndef DONT_USE_UPLOADTOBLOB
        STRICT_EXPECTED_CALL(mocks, singlylinkedlist_get_head_item(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
#endif
        STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));

        STRICT_EXPECTED_CALL(mocks, ThreadAPI_Sleep(1));
        STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
        STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_DoWork(TEST_IOTHUB_CLIENT_LL_HANDLE));
#ifndef DONT_USE_UPLOADTOBLOB
        STRICT_EXPECTED_CALL(mocks, singlylinkedlist_get_head_item(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
#endif
        STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));

        STRICT_EXPECTED_CALL(mocks, ThreadAPI_Sleep(1));
        STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
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
#ifndef DONT_USE_UPLOADTOBLOB
        STRICT_EXPECTED_CALL(mocks, singlylinkedlist_get_head_item(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
#endif
        STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));

        STRICT_EXPECTED_CALL(mocks, ThreadAPI_Sleep(1));
        STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
        STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));

        // act
        threadFunc(threadFuncArg);

        // assert
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubClient_Destroy(iotHubClient);
    }

    /* IoTHubClient_SetOption */

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
    TEST_FUNCTION(IoTHubClient_SetOption_with_NULL_optionName_fails)
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
    /* Tests_SRS_IOTHUBCLIENT_01_041: [ IoTHubClient_SetOption shall be made thread-safe by using the lock created in IoTHubClient_Create. ]*/
    TEST_FUNCTION(IoTHubClient_SetOption_happy_path)
    {
        /// arrange
        CIoTHubClientMocks mocks;

        IOTHUB_CLIENT_HANDLE handle = IoTHubClient_Create(&TEST_CONFIG);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
        EXPECTED_CALL(mocks, IoTHubClient_LL_SetOption(IGNORED_PTR_ARG, "a", "b"));
        STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));

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

        STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
        EXPECTED_CALL(mocks, IoTHubClient_LL_SetOption(IGNORED_PTR_ARG, "a", "b"))
            .SetReturn(IOTHUB_CLIENT_ERROR);
        STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));

        ///act
        auto result = IoTHubClient_SetOption(handle, "a", "b");

        ///assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubClient_Destroy(handle);
    }

    /* Tests_SRS_IOTHUBCLIENT_01_042: [ If acquiring the lock fails, IoTHubClient_GetLastMessageReceiveTime shall return IOTHUB_CLIENT_ERROR. ]*/
    TEST_FUNCTION(when_Lock_fails_IoTHubClient_SetOption_fails)
    {
        /// arrange
        CIoTHubClientMocks mocks;

        IOTHUB_CLIENT_HANDLE handle = IoTHubClient_Create(&TEST_CONFIG);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE))
            .SetReturn(LOCK_ERROR);

        ///act
        auto result = IoTHubClient_SetOption(handle, "a", "b");

        ///assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubClient_Destroy(handle);
    }

    /*Tests_SRS_IOTHUBCLIENT_LL_10_007: [** `IoTHubClient_SetDeviceTwinCallback` shall fail and return `IOTHUB_CLIENT_INVALID_ARG` if parameter `iotHubClientHandle` is `NULL`. ]*/
    TEST_FUNCTION(IoTHubClient_SetDeviceTwinCallback_Fails_When_Handle_Is_NULL)
    {
        // arrange
        CIoTHubClientMocks mocks;

        // act
        IOTHUB_CLIENT_RESULT result = IoTHubClient_SetDeviceTwinCallback(NULL, deviceTwinCallback, (void*)0x42);

        // assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_INVALID_ARG, result);
    }

    /*Tests_SRS_IOTHUBCLIENT_10_002: [** If acquiring the lock fails, `IoTHubClient_SetDeviceTwinCallback` shall return `IOTHUB_CLIENT_ERROR`. ]*/
    TEST_FUNCTION(IoTHubClient_SetDeviceTwinCallback_Fails_When_Lock_Fails)
    {
        // arrange
        CIoTHubClientMocks mocks;

        IOTHUB_CLIENT_HANDLE handle = IoTHubClient_Create(&TEST_CONFIG);
        mocks.ResetAllCalls();

        EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE))
            .SetReturn(LOCK_ERROR);
        // act
        IOTHUB_CLIENT_RESULT result = IoTHubClient_SetDeviceTwinCallback(handle, deviceTwinCallback, (void*)0x42);

        // assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubClient_Destroy(handle);
    }

    /*Tests_SRS_IOTHUBCLIENT_10_004: [** If starting the thread fails, `IoTHubClient_SetDeviceTwinCallback` shall return `IOTHUB_CLIENT_ERROR`. ]*/
    TEST_FUNCTION(IoTHubClient_SetDeviceTwinCallback_Fails_When_Thread_Start_Fails)
    {
        // arrange
        CIoTHubClientMocks mocks;

        IOTHUB_CLIENT_HANDLE handle = IoTHubClient_Create(&TEST_CONFIG);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
        EXPECTED_CALL(mocks, ThreadAPI_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .SetReturn(THREADAPI_ERROR);
        STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));

        // act
        IOTHUB_CLIENT_RESULT result = IoTHubClient_SetDeviceTwinCallback(handle, deviceTwinCallback, (void*)0x42);

        // assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubClient_Destroy(handle);
    }

    TEST_FUNCTION(IoTHubClient_SetDeviceTwinCallback_Calls_LL_SetDeviceTwinCallback)
    {
        // arrange
        CIoTHubClientMocks mocks;

        IOTHUB_CLIENT_HANDLE handle = IoTHubClient_Create(&TEST_CONFIG);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
        EXPECTED_CALL(mocks, ThreadAPI_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));
        STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_SetDeviceTwinCallback(TEST_IOTHUB_CLIENT_LL_HANDLE, deviceTwinCallback, (void*)0x42));

        // act
        IOTHUB_CLIENT_RESULT result = IoTHubClient_SetDeviceTwinCallback(handle, deviceTwinCallback, (void*)0x42);
        ///assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubClient_Destroy(handle);
    }

    /*Tests_SRS_IOTHUBCLIENT_12_012: [ If iotHubClientHandle is NULL, IoTHubClient_SetDeviceMethodCallback shall return IOTHUB_CLIENT_INVALID_ARG. ]*/
    TEST_FUNCTION(IoTHubClient_SetDeviceMethodCallback_Fails_When_Handle_Is_NULL)
    {
        // arrange
        CIoTHubClientMocks mocks;

        // act
        IOTHUB_CLIENT_RESULT result = IoTHubClient_SetDeviceMethodCallback(NULL, deviceMethodCallback, (void*)0x42);

        // assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_INVALID_ARG, result);
    }

    /*Tests_*SRS_IOTHUBCLIENT_12_013: [ If acquiring the lock fails, IoTHubClient_SetDeviceMethodCallback shall return IOTHUB_CLIENT_ERROR. ]*/
    TEST_FUNCTION(IoTHubClient_SetDeviceMethodCallback_Fails_When_Lock_Fails)
    {
        // arrange
        CIoTHubClientMocks mocks;

        IOTHUB_CLIENT_HANDLE handle = IoTHubClient_Create(&TEST_CONFIG);
        mocks.ResetAllCalls();

        EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE))
            .SetReturn(LOCK_ERROR);
        // act
        IOTHUB_CLIENT_RESULT result = IoTHubClient_SetDeviceMethodCallback(handle, deviceMethodCallback, (void*)0x42);

        // assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubClient_Destroy(handle);
    }

    /*Tests_SRS_IOTHUBCLIENT_12_014: [ If the transport handle is null and the worker thread is not initialized, the thread shall be started by calling IoTHubTransport_StartWorkerThread. ]*/
    /*Tests_SRS_IOTHUBCLIENT_12_015: [ If starting the thread fails, IoTHubClient_SetDeviceMethodCallback shall return IOTHUB_CLIENT_ERROR. ]*/
    /*Tests_SRS_IOTHUBCLIENT_12_018: [ IoTHubClient_SetDeviceMethodCallback shall be made thread - safe by using the lock created in IoTHubClient_Create. ]*/
    TEST_FUNCTION(IoTHubClient_SetDeviceMethodCallback_Fails_When_Thread_Start_Fails)
    {
        // arrange
        CIoTHubClientMocks mocks;

        IOTHUB_CLIENT_HANDLE handle = IoTHubClient_Create(&TEST_CONFIG);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
        EXPECTED_CALL(mocks, ThreadAPI_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .SetReturn(THREADAPI_ERROR);
        STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));

        // act
        IOTHUB_CLIENT_RESULT result = IoTHubClient_SetDeviceMethodCallback(handle, deviceMethodCallback, (void*)0x42);

        // assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubClient_Destroy(handle);
    }

    /*Tests_SRS_IOTHUBCLIENT_12_016: [ IoTHubClient_SetDeviceMethodCallback shall call IoTHubClient_LL_SetDeviceMethodCallback, while passing the IoTHubClient_LL_handle created by IoTHubClient_LL_Create along with the parameters deviceMethodCallback and userContextCallback. ]*/
    /*Tests_SRS_IOTHUBCLIENT_12_018: [ IoTHubClient_SetDeviceMethodCallback shall be made thread - safe by using the lock created in IoTHubClient_Create. ]*/
    TEST_FUNCTION(IoTHubClient_SetDeviceMethodCallback_Calls_IoTHubClient_LL_SetDeviceMethodCallback)
    {
        // arrange
        CIoTHubClientMocks mocks;

        IOTHUB_CLIENT_HANDLE handle = IoTHubClient_Create(&TEST_CONFIG);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
        EXPECTED_CALL(mocks, ThreadAPI_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));
        STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_SetDeviceMethodCallback(TEST_IOTHUB_CLIENT_LL_HANDLE, deviceMethodCallback, (void*)0x42));

        // act
        IOTHUB_CLIENT_RESULT result = IoTHubClient_SetDeviceMethodCallback(handle, deviceMethodCallback, (void*)0x42);
        ///assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubClient_Destroy(handle);
    }

    /*Tests_SRS_IOTHUBCLIENT_12_016: [ IoTHubClient_SetDeviceMethodCallback shall call IoTHubClient_LL_SetDeviceMethodCallback, while passing the IoTHubClient_LL_handle created by IoTHubClient_LL_Create along with the parameters deviceMethodCallback and userContextCallback. ]*/
    /*Tests_SRS_IOTHUBCLIENT_12_018: [ IoTHubClient_SetDeviceMethodCallback shall be made thread - safe by using the lock created in IoTHubClient_Create. ]*/
    TEST_FUNCTION(IoTHubClient_SetDeviceMethodCallback_Calls_IoTHubClient_LL_SetDeviceMethodCallback_with_all_param_NULL)
    {
        // arrange
        CIoTHubClientMocks mocks;

        IOTHUB_CLIENT_HANDLE handle = IoTHubClient_Create(&TEST_CONFIG);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
        EXPECTED_CALL(mocks, ThreadAPI_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));
        STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_SetDeviceMethodCallback(TEST_IOTHUB_CLIENT_LL_HANDLE, NULL, NULL));

        // act
        IOTHUB_CLIENT_RESULT result = IoTHubClient_SetDeviceMethodCallback(handle, NULL, NULL);
        ///assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubClient_Destroy(handle);
    }

    /*Tests_SRS_IOTHUBCLIENT_12_017: [ When IoTHubClient_LL_SetDeviceMethodCallback is called, IoTHubClient_SetDeviceMethodCallback shall return the result of IoTHubClient_LL_SetDeviceMethodCallback. ]*/
    /*Tests_SRS_IOTHUBCLIENT_12_018: [ IoTHubClient_SetDeviceMethodCallback shall be made thread - safe by using the lock created in IoTHubClient_Create. ]*/
    TEST_FUNCTION(IoTHubClient_SetDeviceMethodCallback_Calls_IoTHubClient_LL_SetDeviceMethodCallback_and_return_with_the_result)
    {
        // arrange
        CIoTHubClientMocks mocks;

        IOTHUB_CLIENT_HANDLE handle = IoTHubClient_Create(&TEST_CONFIG);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
        EXPECTED_CALL(mocks, ThreadAPI_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));
        STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_SetDeviceMethodCallback(TEST_IOTHUB_CLIENT_LL_HANDLE, deviceMethodCallback, (void*)0x42))
            .SetReturn(IOTHUB_CLIENT_ERROR);

        // act
        IOTHUB_CLIENT_RESULT result = IoTHubClient_SetDeviceMethodCallback(handle, deviceMethodCallback, (void*)0x42);
        ///assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubClient_Destroy(handle);
    }

    /*Tests_SRS_IOTHUBCLIENT_10_013: [** If `iotHubClientHandle` is `NULL`, `IoTHubClient_SendReportedState` shall return `IOTHUB_CLIENT_INVALID_ARG`. ]*/
    TEST_FUNCTION(IoTHubClient_SendReportedState_Fails_When_Handle_Is_NULL)
    {
        // arrange
        CIoTHubClientMocks mocks;

        // act
        IOTHUB_CLIENT_RESULT result = IoTHubClient_SendReportedState(NULL, NULL, 0, sendReportedCallback, (void*)0x42);

        // assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_INVALID_ARG, result);
    }

    /*Tests_SRS_IOTHUBCLIENT_10_014: [** If acquiring the lock fails, `IoTHubClient_SendReportedState` shall return `IOTHUB_CLIENT_ERROR`. ]*/
    TEST_FUNCTION(IoTHubClient_SendReportedState_Fails_When_Lock_Fails)
    {
        // arrange
        CIoTHubClientMocks mocks;

        IOTHUB_CLIENT_HANDLE handle = IoTHubClient_Create(&TEST_CONFIG);
        mocks.ResetAllCalls();

        EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE))
            .SetReturn(LOCK_ERROR);

        // act
        IOTHUB_CLIENT_RESULT result = IoTHubClient_SendReportedState(handle, NULL, 0, sendReportedCallback, (void*)0x42);

        ///assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubClient_Destroy(handle);
    }

    /*Tests_SRS_IOTHUBCLIENT_10_016: [** If starting the thread fails, `IoTHubClient_SendReportedState` shall return `IOTHUB_CLIENT_ERROR`. ]*/
    TEST_FUNCTION(IoTHubClient_SendReportedState_Start_Worker_Thread_Fails)
    {
        // arrange
        CIoTHubClientMocks mocks;

        IOTHUB_CLIENT_HANDLE handle = IoTHubClient_Create(&TEST_CONFIG);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
        EXPECTED_CALL(mocks, ThreadAPI_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .SetReturn(THREADAPI_ERROR);
        STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));

        // act
        IOTHUB_CLIENT_RESULT result = IoTHubClient_SendReportedState(handle, NULL, 0, sendReportedCallback, (void*)0x42);

        ///assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubClient_Destroy(handle);
    }

    /*Tests_SRS_IOTHUBCLIENT_10_017: [** `IoTHubClient_SendReportedState` shall call `IoTHubClient_LL_SendReportedState`, while passing the `IoTHubClient_LL handle` created by `IoTHubClient_LL_Create` along with the parameters `reportedState`, `size`, `reportedVersion`, `lastSeenDesiredVersion`, `reportedStateCallback`, and `userContextCallback`. ]*/
    TEST_FUNCTION(IoTHubClient_SendReportedState_Happy_Path)
    {
        // arrange
        CIoTHubClientMocks mocks;

        IOTHUB_CLIENT_HANDLE handle = IoTHubClient_Create(&TEST_CONFIG);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
        EXPECTED_CALL(mocks, ThreadAPI_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_SendReportedState(TEST_IOTHUB_CLIENT_LL_HANDLE, NULL, 0, sendReportedCallback, (void*)0x42));
        STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));

        // act
        IOTHUB_CLIENT_RESULT result = IoTHubClient_SendReportedState(handle, NULL, 0, sendReportedCallback, (void*)0x42);

        ///assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubClient_Destroy(handle);
    }

    /*Tests_SRS_IOTHUBCLIENT_10_017: [** `IoTHubClient_SendReportedState` shall call `IoTHubClient_LL_SendReportedState`, while passing the `IoTHubClient_LL handle` created by `IoTHubClient_LL_Create` along with the parameters `reportedState`, `size`, `reportedVersion`, `lastSeenDesiredVersion`, `reportedStateCallback`, and `userContextCallback`. ]*/
    TEST_FUNCTION(IoTHubClient_SendReportedState_Fails)
    {
        // arrange
        CIoTHubClientMocks mocks;

        IOTHUB_CLIENT_HANDLE handle = IoTHubClient_Create(&TEST_CONFIG);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
        EXPECTED_CALL(mocks, ThreadAPI_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, IoTHubClient_LL_SendReportedState(TEST_IOTHUB_CLIENT_LL_HANDLE, NULL, 0, sendReportedCallback, (void*)0x42))
            .SetReturn(IOTHUB_CLIENT_ERROR);
        STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));

        // act
        IOTHUB_CLIENT_RESULT result = IoTHubClient_SendReportedState(handle, NULL, 0, sendReportedCallback, (void*)0x42);

        ///assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubClient_Destroy(handle);
    }

#ifndef DONT_USE_UPLOADTOBLOB
    /*Tests_SRS_IOTHUBCLIENT_02_047: [ If iotHubClientHandle is NULL then IoTHubClient_UploadToBlobAsync shall fail and return IOTHUB_CLIENT_INVALID_ARG. ]*/
    TEST_FUNCTION(IoTHubClient_UploadToBlobAsync_with_NULL_iotHubClientHandle_fails)
    {
        ///arrange
        IOTHUB_CLIENT_RESULT result;

        ///act
        result = IoTHubClient_UploadToBlobAsync(NULL, "a", (const unsigned char*)"b", 1, NULL, NULL);

        ///assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_INVALID_ARG, result);
    }
#endif

#ifdef USE_UPOLOADTOBLOB
    /*Tests_SRS_IOTHUBCLIENT_02_048: [ If destinationFileName is NULL then IoTHubClient_UploadToBlobAsync shall fail and return IOTHUB_CLIENT_INVALID_ARG. ]*/
    TEST_FUNCTION(IoTHubClient_UploadToBlobAsync_with_NULL_destinationFileName_fails)
    {
        ///arrange
        CIoTHubClientMocks mocks;

        IOTHUB_CLIENT_HANDLE h = IoTHubClient_Create(&TEST_CONFIG);
        IOTHUB_CLIENT_RESULT result;
        mocks.ResetAllCalls();

        ///act
        result = IoTHubClient_UploadToBlobAsync(h, NULL, (const unsigned char*)"b", 1, NULL, NULL);

        ///assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_INVALID_ARG, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubClient_Destroy(h);
    }
#endif

#ifdef USE_UPOLOADTOBLOB
    /*Tests_SRS_IOTHUBCLIENT_02_049: [ If source is NULL and size is greated than 0 then IoTHubClient_UploadToBlobAsync shall fail and return IOTHUB_CLIENT_INVALID_ARG. ]*/
    TEST_FUNCTION(IoTHubClient_UploadToBlobAsync_with_NULL_source_and_size_1_fails)
    {
        ///arrange
        CIoTHubClientMocks mocks;

        IOTHUB_CLIENT_HANDLE h = IoTHubClient_Create(&TEST_CONFIG);
        IOTHUB_CLIENT_RESULT result;
        mocks.ResetAllCalls();

        ///act
        result = IoTHubClient_UploadToBlobAsync(h, "someFileName.txt", NULL, 1, NULL, NULL);

        ///assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_INVALID_ARG, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubClient_Destroy(h);
    }
#endif

#ifdef USE_UPOLOADTOBLOB
    /*Tests_SRS_IOTHUBCLIENT_02_051: [ IoTHubClient_UploadToBlobAsync shall copy the souce, size, iotHubClientFileUploadCallback, context and a non-initialized(1) THREAD_HANDLE parameters into a structure. ]*/
    /*Tests_SRS_IOTHUBCLIENT_02_058: [ IoTHubClient_UploadToBlobAsync shall add the structure to the list of structures that need to be cleaned once file upload finishes. ]*/
    /*Tests_SRS_IOTHUBCLIENT_02_052: [ IoTHubClient_UploadToBlobAsync shall spawn a thread passing the structure build in SRS IOTHUBCLIENT 02 051 as thread data.]*/
    /*Tests_SRS_IOTHUBCLIENT_02_054: [ The thread shall call IoTHubClient_LL_UploadToBlob passing the information packed in the structure. ]*/
    /*Tests_SRS_IOTHUBCLIENT_02_056: [ Otherwise the thread iotHubClientFileUploadCallbackInternal passing as result FILE_UPLOAD_OK and the structure from SRS IOTHUBCLIENT 02 051. ]*/
    /*Tests_SRS_IOTHUBCLIENT_02_071: [ The thread shall mark itself as disposable. ]*/
    TEST_FUNCTION(IoTHubClient_UploadToBlobAsync_succeeds)
    {
        ///arrange
        CIoTHubClientMocks mocks;

        IOTHUB_CLIENT_HANDLE h = IoTHubClient_Create(&TEST_CONFIG);
        IOTHUB_CLIENT_RESULT result;
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is creating a UPLOADTOBLOB_SAVED_DATA*/
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, mallocAndStrcpy_s(IGNORED_PTR_ARG, "someFileName.txt")) /*this is making a copy of the filename*/
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(1)); /*this is making a copy of the content*/

        STRICT_EXPECTED_CALL(mocks, Lock(IGNORED_PTR_ARG)) /*this is locking the IOTHUB_CLIENT_HANDLE because it's savedDataToBeCleaned member is about to be modified*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, ThreadAPI_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG)) /*this is starting the worker thread*/
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(mocks, Unlock(IGNORED_PTR_ARG)) /*what has been locked shall be unlocked*/
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, singlylinkedlist_add(IGNORED_PTR_ARG, IGNORED_PTR_ARG)) /*this is adding UPLOADTOBLOB_SAVED_DATA to the list of UPLOADTOBLOB_SAVED_DATAs to be cleaned*/
            .IgnoreArgument(1)
            .IgnoreArgument(2);

        STRICT_EXPECTED_CALL(mocks, Lock_Init()); /*this is creating a lock for the canBeGarbageCollected */

        STRICT_EXPECTED_CALL(mocks, ThreadAPI_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG)) /*this is spawning the thread*/
            .IgnoreAllArguments();

        STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_UploadToBlob(IGNORED_PTR_ARG, "someFileName.txt", IGNORED_PTR_ARG, 1)) /*this is the thread calling into _LL layer*/
            .IgnoreArgument(1)
            .IgnoreArgument(3);

        STRICT_EXPECTED_CALL(mocks, uploadToBlobAsyncCallback(FILE_UPLOAD_OK, (void*)1)); /*the thread completes successfully*/

        STRICT_EXPECTED_CALL(mocks, Lock(IGNORED_PTR_ARG)) /*this is the thread marking UPLOADTOBLOB_SAVED_DATA as disposeable*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, Unlock(IGNORED_PTR_ARG)) /*what has been locked, shall be unlocked*/
            .IgnoreArgument(1);

        ///act
        result = IoTHubClient_UploadToBlobAsync(h, "someFileName.txt", (const unsigned char*)"a", 1, uploadToBlobAsyncCallback, (void*)1);

        threadFunc(threadFuncArg); /*this is the thread uploading function*/

        ///assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubClient_Destroy(h);
    }
#endif

#ifdef USE_UPOLOADTOBLOB
    /*Tests_SRS_IOTHUBCLIENT_02_051: [ IoTHubClient_UploadToBlobAsync shall copy the souce, size, iotHubClientFileUploadCallback, context and a non-initialized(1) THREAD_HANDLE parameters into a structure. ]*/
    /*Tests_SRS_IOTHUBCLIENT_02_058: [ IoTHubClient_UploadToBlobAsync shall add the structure to the list of structures that need to be cleaned once file upload finishes. ]*/
    /*Tests_SRS_IOTHUBCLIENT_02_052: [ IoTHubClient_UploadToBlobAsync shall spawn a thread passing the structure build in SRS IOTHUBCLIENT 02 051 as thread data.]*/
    /*Tests_SRS_IOTHUBCLIENT_02_054: [ The thread shall call IoTHubClient_LL_UploadToBlob passing the information packed in the structure. ]*/
    /*Tests_SRS_IOTHUBCLIENT_02_056: [ Otherwise the thread iotHubClientFileUploadCallbackInternal passing as result FILE_UPLOAD_OK and the structure from SRS IOTHUBCLIENT 02 051. ]*/
    /*Tests_SRS_IOTHUBCLIENT_02_071: [ The thread shall mark itself as disposable. ]*/
    TEST_FUNCTION(IoTHubClient_UploadToBlobAsync_with_0_size_succeeds)
    {
        ///arrange
        CIoTHubClientMocks mocks;

        IOTHUB_CLIENT_HANDLE h = IoTHubClient_Create(&TEST_CONFIG);
        IOTHUB_CLIENT_RESULT result;
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is creating a UPLOADTOBLOB_SAVED_DATA*/
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, mallocAndStrcpy_s(IGNORED_PTR_ARG, "someFileName.txt")) /*this is making a copy of the filename*/
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, Lock(IGNORED_PTR_ARG)) /*this is locking the IOTHUB_CLIENT_HANDLE because it's savedDataToBeCleaned member is about to be modified*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, ThreadAPI_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG)) /*this is starting the worker thread*/
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(mocks, Unlock(IGNORED_PTR_ARG)) /*what has been locked shall be unlocked*/
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, singlylinkedlist_add(IGNORED_PTR_ARG, IGNORED_PTR_ARG)) /*this is adding UPLOADTOBLOB_SAVED_DATA to the list of UPLOADTOBLOB_SAVED_DATAs to be cleaned*/
            .IgnoreArgument(1)
            .IgnoreArgument(2);

        STRICT_EXPECTED_CALL(mocks, Lock_Init()); /*this is creating a lock for the canBeGarbageCollected */

        STRICT_EXPECTED_CALL(mocks, ThreadAPI_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG)) /*this is spawning the thread*/
            .IgnoreAllArguments();

        STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_UploadToBlob(IGNORED_PTR_ARG, "someFileName.txt", IGNORED_PTR_ARG, 0)) /*this is the thread calling into _LL layer*/
            .IgnoreArgument(1)
            .IgnoreArgument(3);

        STRICT_EXPECTED_CALL(mocks, uploadToBlobAsyncCallback(FILE_UPLOAD_OK, (void*)1)); /*the thread completes successfully*/

        STRICT_EXPECTED_CALL(mocks, Lock(IGNORED_PTR_ARG)) /*this is the thread marking UPLOADTOBLOB_SAVED_DATA as disposeable*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, Unlock(IGNORED_PTR_ARG)) /*what has been locked, shall be unlocked*/
            .IgnoreArgument(1);

        ///act
        result = IoTHubClient_UploadToBlobAsync(h, "someFileName.txt", NULL, 0, uploadToBlobAsyncCallback, (void*)1);

        threadFunc(threadFuncArg); /*this is the thread uploading function*/

        ///assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubClient_Destroy(h);
    }
#endif

#ifdef USE_UPOLOADTOBLOB
    /*Tests_SRS_IOTHUBCLIENT_02_051: [ IoTHubClient_UploadToBlobAsync shall copy the souce, size, iotHubClientFileUploadCallback, context and a non-initialized(1) THREAD_HANDLE parameters into a structure. ]*/
    /*Tests_SRS_IOTHUBCLIENT_02_058: [ IoTHubClient_UploadToBlobAsync shall add the structure to the list of structures that need to be cleaned once file upload finishes. ]*/
    /*Tests_SRS_IOTHUBCLIENT_02_052: [ IoTHubClient_UploadToBlobAsync shall spawn a thread passing the structure build in SRS IOTHUBCLIENT 02 051 as thread data.]*/
    /*Tests_SRS_IOTHUBCLIENT_02_054: [ The thread shall call IoTHubClient_LL_UploadToBlob passing the information packed in the structure. ]*/
    /*Tests_SRS_IOTHUBCLIENT_02_055: [ If IoTHubClient_LL_UploadToBlob fails then the thread shall call the callback passing as result FILE_UPLOAD_ERROR and as context the structure from SRS IOTHUBCLIENT 02 051. ]*/
    /*Tests_SRS_IOTHUBCLIENT_02_071: [ The thread shall mark itself as disposable. ]*/
    TEST_FUNCTION(IoTHubClient_UploadToBlobAsync_indicates_error)
    {
        ///arrange
        CIoTHubClientMocks mocks;

        IOTHUB_CLIENT_HANDLE h = IoTHubClient_Create(&TEST_CONFIG);
        IOTHUB_CLIENT_RESULT result;
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is creating a UPLOADTOBLOB_SAVED_DATA*/
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, mallocAndStrcpy_s(IGNORED_PTR_ARG, "someFileName.txt")) /*this is making a copy of the filename*/
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(1)); /*this is making a copy of the content*/

        STRICT_EXPECTED_CALL(mocks, Lock(IGNORED_PTR_ARG)) /*this is locking the IOTHUB_CLIENT_HANDLE because it's savedDataToBeCleaned member is about to be modified*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, ThreadAPI_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG)) /*this is starting the worker thread*/
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(mocks, Unlock(IGNORED_PTR_ARG)) /*what has been locked shall be unlocked*/
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, singlylinkedlist_add(IGNORED_PTR_ARG, IGNORED_PTR_ARG)) /*this is adding UPLOADTOBLOB_SAVED_DATA to the list of UPLOADTOBLOB_SAVED_DATAs to be cleaned*/
            .IgnoreArgument(1)
            .IgnoreArgument(2);

        STRICT_EXPECTED_CALL(mocks, Lock_Init()); /*this is creating a lock for the canBeGarbageCollected */

        STRICT_EXPECTED_CALL(mocks, ThreadAPI_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG)) /*this is spawning the thread*/
            .IgnoreAllArguments();

        STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_UploadToBlob(IGNORED_PTR_ARG, "someFileName.txt", IGNORED_PTR_ARG, 1)) /*this is the thread calling into _LL layer*/
            .IgnoreArgument(1)
            .IgnoreArgument(3)
            .SetReturn(IOTHUB_CLIENT_ERROR);

        STRICT_EXPECTED_CALL(mocks, uploadToBlobAsyncCallback(FILE_UPLOAD_ERROR, (void*)1)); /*the thread completes the upload, but fails*/

        STRICT_EXPECTED_CALL(mocks, Lock(IGNORED_PTR_ARG)) /*this is the thread marking UPLOADTOBLOB_SAVED_DATA as disposeable*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, Unlock(IGNORED_PTR_ARG)) /*what has been locked, shall be unlocked*/
            .IgnoreArgument(1);

        ///act
        result = IoTHubClient_UploadToBlobAsync(h, "someFileName.txt", (const unsigned char*)"a", 1, uploadToBlobAsyncCallback, (void*)1);

        threadFunc(threadFuncArg); /*this is the thread uploading function*/

        ///assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubClient_Destroy(h);
    }
#endif

#ifdef USE_UPOLOADTOBLOB
    /*Tests_SRS_IOTHUBCLIENT_02_053: [ If copying to the structure or spawning the thread fails, then IoTHubClient_UploadToBlobAsync shall fail and return IOTHUB_CLIENT_ERROR. ]*/
    TEST_FUNCTION(IoTHubClient_UploadToBlobAsync_fails_when_ThreadAPI_Create_fails)
    {
        ///arrange
        CIoTHubClientMocks mocks;

        IOTHUB_CLIENT_HANDLE h = IoTHubClient_Create(&TEST_CONFIG);
        IOTHUB_CLIENT_RESULT result;
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is creating a UPLOADTOBLOB_SAVED_DATA*/
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, mallocAndStrcpy_s(IGNORED_PTR_ARG, "someFileName.txt")) /*this is making a copy of the filename*/
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(1)); /*this is making a copy of the content*/

        STRICT_EXPECTED_CALL(mocks, Lock(IGNORED_PTR_ARG)) /*this is locking the IOTHUB_CLIENT_HANDLE because it's savedDataToBeCleaned member is about to be modified*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, ThreadAPI_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG)) /*this is starting the worker thread*/
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(mocks, Unlock(IGNORED_PTR_ARG)) /*what has been locked shall be unlocked*/
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, singlylinkedlist_add(IGNORED_PTR_ARG, IGNORED_PTR_ARG)) /*this is adding UPLOADTOBLOB_SAVED_DATA to the list of UPLOADTOBLOB_SAVED_DATAs to be cleaned*/
            .IgnoreArgument(1)
            .IgnoreArgument(2);

        STRICT_EXPECTED_CALL(mocks, Lock_Init()); /*this is creating a lock for the canBeGarbageCollected */

        STRICT_EXPECTED_CALL(mocks, ThreadAPI_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG)) /*this is spawning the thread*/
            .IgnoreAllArguments()
            .SetFailReturn(THREADAPI_ERROR);

        STRICT_EXPECTED_CALL(mocks, Lock_Deinit(IGNORED_PTR_ARG)) /*this is uncreating a lock for the canBeGarbageCollected */
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, singlylinkedlist_remove(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();

        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        ///act
        result = IoTHubClient_UploadToBlobAsync(h, "someFileName.txt", (const unsigned char*)"a", 1, uploadToBlobAsyncCallback, (void*)1);

        ///assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubClient_Destroy(h);
    }
#endif

#ifdef USE_UPOLOADTOBLOB
    /*Tests_SRS_IOTHUBCLIENT_02_053: [ If copying to the structure or spawning the thread fails, then IoTHubClient_UploadToBlobAsync shall fail and return IOTHUB_CLIENT_ERROR. ]*/
    TEST_FUNCTION(IoTHubClient_UploadToBlobAsync_fails_when_Lock_Init_fails)
    {
        ///arrange
        CIoTHubClientMocks mocks;

        IOTHUB_CLIENT_HANDLE h = IoTHubClient_Create(&TEST_CONFIG);
        IOTHUB_CLIENT_RESULT result;
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is creating a UPLOADTOBLOB_SAVED_DATA*/
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, mallocAndStrcpy_s(IGNORED_PTR_ARG, "someFileName.txt")) /*this is making a copy of the filename*/
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(1)); /*this is making a copy of the content*/

        STRICT_EXPECTED_CALL(mocks, Lock(IGNORED_PTR_ARG)) /*this is locking the IOTHUB_CLIENT_HANDLE because it's savedDataToBeCleaned member is about to be modified*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, ThreadAPI_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG)) /*this is starting the worker thread*/
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(mocks, Unlock(IGNORED_PTR_ARG)) /*what has been locked shall be unlocked*/
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, singlylinkedlist_add(IGNORED_PTR_ARG, IGNORED_PTR_ARG)) /*this is adding UPLOADTOBLOB_SAVED_DATA to the list of UPLOADTOBLOB_SAVED_DATAs to be cleaned*/
            .IgnoreArgument(1)
            .IgnoreArgument(2);

        STRICT_EXPECTED_CALL(mocks, Lock_Init()) /*this is creating a lock for the canBeGarbageCollected */
            .SetFailReturn((LOCK_HANDLE)NULL);

        STRICT_EXPECTED_CALL(mocks, singlylinkedlist_remove(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreAllArguments();

        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        ///act
        result = IoTHubClient_UploadToBlobAsync(h, "someFileName.txt", (const unsigned char*)"a", 1, uploadToBlobAsyncCallback, (void*)1);

        ///assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubClient_Destroy(h);
    }
#endif

#ifdef USE_UPOLOADTOBLOB
    /*Tests_SRS_IOTHUBCLIENT_02_053: [ If copying to the structure or spawning the thread fails, then IoTHubClient_UploadToBlobAsync shall fail and return IOTHUB_CLIENT_ERROR. ]*/
    TEST_FUNCTION(IoTHubClient_UploadToBlobAsync_fails_when_list_add_fails)
    {
        ///arrange
        CIoTHubClientMocks mocks;

        IOTHUB_CLIENT_HANDLE h = IoTHubClient_Create(&TEST_CONFIG);
        IOTHUB_CLIENT_RESULT result;
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is creating a UPLOADTOBLOB_SAVED_DATA*/
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, mallocAndStrcpy_s(IGNORED_PTR_ARG, "someFileName.txt")) /*this is making a copy of the filename*/
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(1)); /*this is making a copy of the content*/

        STRICT_EXPECTED_CALL(mocks, Lock(IGNORED_PTR_ARG)) /*this is locking the IOTHUB_CLIENT_HANDLE because it's savedDataToBeCleaned member is about to be modified*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, ThreadAPI_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG)) /*this is starting the worker thread*/
            .IgnoreAllArguments();
        STRICT_EXPECTED_CALL(mocks, Unlock(IGNORED_PTR_ARG)) /*what has been locked shall be unlocked*/
            .IgnoreArgument(1);
    
        STRICT_EXPECTED_CALL(mocks, singlylinkedlist_add(IGNORED_PTR_ARG, IGNORED_PTR_ARG)) /*this is adding UPLOADTOBLOB_SAVED_DATA to the list of UPLOADTOBLOB_SAVED_DATAs to be cleaned*/
            .IgnoreArgument(1)
            .IgnoreArgument(2)
            .SetFailReturn((LIST_ITEM_HANDLE)NULL);

        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        ///act
        result = IoTHubClient_UploadToBlobAsync(h, "someFileName.txt", (const unsigned char*)"a", 1, uploadToBlobAsyncCallback, (void*)1);

        ///assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubClient_Destroy(h);
    }
#endif

#ifdef USE_UPOLOADTOBLOB
    /*Tests_SRS_IOTHUBCLIENT_02_053: [ If copying to the structure or spawning the thread fails, then IoTHubClient_UploadToBlobAsync shall fail and return IOTHUB_CLIENT_ERROR. ]*/
    TEST_FUNCTION(IoTHubClient_UploadToBlobAsync_fails_when_StartWorkerThreadIfNeeded_fails)
    {
        ///arrange
        CIoTHubClientMocks mocks;

        IOTHUB_CLIENT_HANDLE h = IoTHubClient_Create(&TEST_CONFIG);
        IOTHUB_CLIENT_RESULT result;
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is creating a UPLOADTOBLOB_SAVED_DATA*/
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, mallocAndStrcpy_s(IGNORED_PTR_ARG, "someFileName.txt")) /*this is making a copy of the filename*/
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(1)); /*this is making a copy of the content*/

        STRICT_EXPECTED_CALL(mocks, Lock(IGNORED_PTR_ARG)) /*this is locking the IOTHUB_CLIENT_HANDLE because it's savedDataToBeCleaned member is about to be modified*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, ThreadAPI_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG)) /*this is starting the worker thread*/
            .IgnoreAllArguments()
            .SetReturn(THREADAPI_ERROR);

        STRICT_EXPECTED_CALL(mocks, Unlock(IGNORED_PTR_ARG)) /*what has been locked shall be unlocked*/
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        ///act
        result = IoTHubClient_UploadToBlobAsync(h, "someFileName.txt", (const unsigned char*)"a", 1, uploadToBlobAsyncCallback, (void*)1);

        ///assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubClient_Destroy(h);
    }
#endif

#ifdef USE_UPOLOADTOBLOB
    /*Tests_SRS_IOTHUBCLIENT_02_053: [ If copying to the structure or spawning the thread fails, then IoTHubClient_UploadToBlobAsync shall fail and return IOTHUB_CLIENT_ERROR. ]*/
    TEST_FUNCTION(IoTHubClient_UploadToBlobAsync_fails_when_Lock_fails)
    {
        ///arrange
        CIoTHubClientMocks mocks;

        IOTHUB_CLIENT_HANDLE h = IoTHubClient_Create(&TEST_CONFIG);
        IOTHUB_CLIENT_RESULT result;
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is creating a UPLOADTOBLOB_SAVED_DATA*/
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, mallocAndStrcpy_s(IGNORED_PTR_ARG, "someFileName.txt")) /*this is making a copy of the filename*/
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(1)); /*this is making a copy of the content*/

        STRICT_EXPECTED_CALL(mocks, Lock(IGNORED_PTR_ARG)) /*this is locking the IOTHUB_CLIENT_HANDLE because it's savedDataToBeCleaned member is about to be modified*/
            .IgnoreArgument(1)
            .SetReturn(LOCK_ERROR);

        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        ///act
        result = IoTHubClient_UploadToBlobAsync(h, "someFileName.txt", (const unsigned char*)"a", 1, uploadToBlobAsyncCallback, (void*)1);

        ///assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubClient_Destroy(h);
    }
#endif

#ifdef USE_UPOLOADTOBLOB
    /*Tests_SRS_IOTHUBCLIENT_02_053: [ If copying to the structure or spawning the thread fails, then IoTHubClient_UploadToBlobAsync shall fail and return IOTHUB_CLIENT_ERROR. ]*/
    TEST_FUNCTION(IoTHubClient_UploadToBlobAsync_fails_when_malloc_fails_1)
    {
        ///arrange
        CIoTHubClientMocks mocks;

        IOTHUB_CLIENT_HANDLE h = IoTHubClient_Create(&TEST_CONFIG);
        IOTHUB_CLIENT_RESULT result;
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is creating a UPLOADTOBLOB_SAVED_DATA*/
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, mallocAndStrcpy_s(IGNORED_PTR_ARG, "someFileName.txt")) /*this is making a copy of the filename*/
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(1)) /*this is making a copy of the content*/
            .SetFailReturn((void*)NULL);

        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        ///act
        result = IoTHubClient_UploadToBlobAsync(h, "someFileName.txt", (const unsigned char*)"a", 1, uploadToBlobAsyncCallback, (void*)1);

        ///assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubClient_Destroy(h);
    }
#endif

#ifdef USE_UPOLOADTOBLOB
    /*Tests_SRS_IOTHUBCLIENT_02_053: [ If copying to the structure or spawning the thread fails, then IoTHubClient_UploadToBlobAsync shall fail and return IOTHUB_CLIENT_ERROR. ]*/
    TEST_FUNCTION(IoTHubClient_UploadToBlobAsync_fails_when_malloc_fails_2)
    {
        ///arrange
        CIoTHubClientMocks mocks;

        IOTHUB_CLIENT_HANDLE h = IoTHubClient_Create(&TEST_CONFIG);
        IOTHUB_CLIENT_RESULT result;
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is creating a UPLOADTOBLOB_SAVED_DATA*/
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, mallocAndStrcpy_s(IGNORED_PTR_ARG, "someFileName.txt")) /*this is making a copy of the filename*/
            .IgnoreArgument(1)
            .SetFailReturn(__LINE__);

        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        ///act
        result = IoTHubClient_UploadToBlobAsync(h, "someFileName.txt", (const unsigned char*)"a", 1, uploadToBlobAsyncCallback, (void*)1);

        ///assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubClient_Destroy(h);
    }
#endif

#ifdef USE_UPOLOADTOBLOB
    /*Tests_SRS_IOTHUBCLIENT_02_053: [ If copying to the structure or spawning the thread fails, then IoTHubClient_UploadToBlobAsync shall fail and return IOTHUB_CLIENT_ERROR. ]*/
    TEST_FUNCTION(IoTHubClient_UploadToBlobAsync_fails_when_malloc_fails_3)
    {
        ///arrange
        CIoTHubClientMocks mocks;

        IOTHUB_CLIENT_HANDLE h = IoTHubClient_Create(&TEST_CONFIG);
        IOTHUB_CLIENT_RESULT result;
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is creating a UPLOADTOBLOB_SAVED_DATA*/
            .IgnoreArgument(1)
            .SetFailReturn((void*)NULL);

        ///act
        result = IoTHubClient_UploadToBlobAsync(h, "someFileName.txt", (const unsigned char*)"a", 1, uploadToBlobAsyncCallback, (void*)1);

        ///assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubClient_Destroy(h);
    }
#endif

END_TEST_SUITE(iothubclient_ut)
