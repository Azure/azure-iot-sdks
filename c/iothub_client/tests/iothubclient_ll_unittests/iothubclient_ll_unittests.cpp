// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <cstdlib>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "testrunnerswitcher.h"
#include "micromock.h"
#include "micromockcharstararenullterminatedstrings.h"

#include "iothub_client_ll.h"
#include "version.h"

#include "lock.h"
#include "doublylinkedlist.h"
#include "iothub_client_private.h"

#include "string_tokenizer.h"
#include "strings.h"

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

#include "doublylinkedlist.c"
};

DEFINE_MICROMOCK_ENUM_TO_STRING(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_RESULT_VALUES);
DEFINE_MICROMOCK_ENUM_TO_STRING(IOTHUB_BATCHSTATE_RESULT, IOTHUB_BATCHSTATE_RESULT_VALUES);
DEFINE_MICROMOCK_ENUM_TO_STRING(IOTHUBMESSAGE_DISPOSITION_RESULT, IOTHUBMESSAGE_DISPOSITION_RESULT_VALUES);


static MICROMOCK_MUTEX_HANDLE g_testByTest;
static MICROMOCK_GLOBAL_SEMAPHORE_HANDLE g_dllByDll;

#define TEST_DEVICE_ID "theidofTheDevice"
#define TEST_DEVICE_KEY "theKeyoftheDevice"
#define TEST_IOTHUBNAME "theNameoftheIotHub"
#define TEST_IOTHUBSUFFIX "theSuffixoftheIotHubHostname"
#define TEST_AUTHORIZATIONKEY "theAuthorizationKey"

#define TEST_HOSTNAME_TOKEN "HostName"
#define TEST_HOSTNAME_VALUE "theNameoftheIotHub.theSuffixoftheIotHubHostname"

#define TEST_DEVICEID_TOKEN "DeviceId"
#define TEST_DEVICEKEY_TOKEN "SharedAccessKey"

#define TEST_DEVICEMESSAGE_HANDLE (IOTHUB_MESSAGE_HANDLE)0x52
#define TEST_IOTHUB_CLIENT_LL_HANDLE    (IOTHUB_CLIENT_LL_HANDLE)0x4242

#define TEST_STRING_HANDLE (STRING_HANDLE)0x46
#define TEST_STRING_TOKENIZER_HANDLE (STRING_TOKENIZER_HANDLE)0x48
static const char* TEST_CHAR = "TestChar";

static const void* provideFAKE(void);

static const IOTHUB_CLIENT_CONFIG TEST_CONFIG =
{
    provideFAKE,            /* IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol;   */
    TEST_DEVICE_ID,         /* const char* deviceId;                        */
    TEST_DEVICE_KEY,        /* const char* deviceKey;                       */
    TEST_IOTHUBNAME,        /* const char* iotHubName;                      */
    TEST_IOTHUBSUFFIX,      /* const char* iotHubSuffix;                    */
};

static const IOTHUB_CLIENT_CONFIG TEST_CONFIG_NULL_protocol =
{
    NULL,                   /* IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol;   */
    TEST_DEVICE_ID,         /* const char* deviceId;                        */
    TEST_DEVICE_KEY,        /* const char* deviceKey;                       */
    TEST_IOTHUBNAME,        /* const char* iotHubName;                      */
    TEST_IOTHUBSUFFIX,      /* const char* iotHubSuffix;                    */
};

static size_t currentmalloc_call;
static size_t whenShallmalloc_fail;
static IOTHUB_CLIENT_STATUS currentIotHubClientStatus;

TYPED_MOCK_CLASS(CIoTHubClientLLMocks, CGlobalMock)
{
public:

    /* DoublyLinkedList mocks */
    MOCK_STATIC_METHOD_1(, void, DList_InitializeListHead, PDLIST_ENTRY, listHead)
        BASEIMPLEMENTATION::DList_InitializeListHead(listHead);
    MOCK_VOID_METHOD_END()

    MOCK_STATIC_METHOD_1(, int, DList_IsListEmpty, PDLIST_ENTRY, listHead)
        int result2 = BASEIMPLEMENTATION::DList_IsListEmpty(listHead);
    MOCK_METHOD_END(int, result2)

    MOCK_STATIC_METHOD_2(, void, DList_InsertTailList, PDLIST_ENTRY, listHead, PDLIST_ENTRY, listEntry)
        BASEIMPLEMENTATION::DList_InsertTailList(listHead, listEntry);
    MOCK_VOID_METHOD_END()

    MOCK_STATIC_METHOD_2(, void, DList_AppendTailList, PDLIST_ENTRY, listHead, PDLIST_ENTRY, ListToAppend)
        BASEIMPLEMENTATION::DList_AppendTailList(listHead, ListToAppend);
    MOCK_VOID_METHOD_END()

    MOCK_STATIC_METHOD_1(, int, DList_RemoveEntryList, PDLIST_ENTRY, listEntry)
        int result2 = BASEIMPLEMENTATION::DList_RemoveEntryList(listEntry);
    MOCK_METHOD_END(int, result2)

    MOCK_STATIC_METHOD_1(, PDLIST_ENTRY, DList_RemoveHeadList, PDLIST_ENTRY, listHead)
        PDLIST_ENTRY entry = BASEIMPLEMENTATION::DList_RemoveHeadList(listHead);
    MOCK_METHOD_END(PDLIST_ENTRY, entry)

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

    MOCK_STATIC_METHOD_1(, TRANSPORT_HANDLE, FAKE_IoTHubTransport_Create, const IOTHUBTRANSPORT_CONFIG*, config)
    MOCK_METHOD_END(TRANSPORT_HANDLE, (TRANSPORT_HANDLE)(0x42))

    MOCK_STATIC_METHOD_3(, IOTHUB_CLIENT_RESULT, FAKE_IoTHubTransport_SetOption, TRANSPORT_HANDLE, handle, const char*, optionName, const void*, value)
    MOCK_METHOD_END(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK)

    MOCK_STATIC_METHOD_1(, void, FAKE_IoTHubTransport_Destroy, TRANSPORT_HANDLE, handle)
    MOCK_VOID_METHOD_END()

    MOCK_STATIC_METHOD_1(, int, FAKE_IoTHubTransport_Subscribe, TRANSPORT_HANDLE, handle)
    MOCK_METHOD_END(int, 0)

    MOCK_STATIC_METHOD_1(, void, FAKE_IoTHubTransport_Unsubscribe, TRANSPORT_HANDLE, handle)
    MOCK_VOID_METHOD_END()

    MOCK_STATIC_METHOD_2(, void, FAKE_IoTHubTransport_DoWork, TRANSPORT_HANDLE, handle, IOTHUB_CLIENT_LL_HANDLE, iotHubClientHandle)
    MOCK_VOID_METHOD_END()

    MOCK_STATIC_METHOD_2(, IOTHUB_CLIENT_RESULT, FAKE_IoTHubTransport_GetSendStatus, TRANSPORT_HANDLE, handle, IOTHUB_CLIENT_STATUS*, iotHubClientStatus)
        *iotHubClientStatus = currentIotHubClientStatus;
    MOCK_METHOD_END(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK)

    MOCK_STATIC_METHOD_2(, void, eventConfirmationCallback, IOTHUB_CLIENT_CONFIRMATION_RESULT, result2, void*, userContextCallback)
    MOCK_VOID_METHOD_END()

    MOCK_STATIC_METHOD_2(, IOTHUBMESSAGE_DISPOSITION_RESULT, messageCallback, IOTHUB_MESSAGE_HANDLE, message, void*, userContextCallback)
    MOCK_METHOD_END(IOTHUBMESSAGE_DISPOSITION_RESULT, IOTHUBMESSAGE_ACCEPTED);

    MOCK_STATIC_METHOD_1(, IOTHUB_MESSAGE_HANDLE, IoTHubMessage_Clone, IOTHUB_MESSAGE_HANDLE, iotHubMessageHandle)
    MOCK_METHOD_END(IOTHUB_MESSAGE_HANDLE, (IOTHUB_MESSAGE_HANDLE)((uintptr_t)iotHubMessageHandle + 1000))

    MOCK_STATIC_METHOD_1(, void, IoTHubMessage_Destroy, IOTHUB_MESSAGE_HANDLE, iotHubMessageHandle)
    MOCK_VOID_METHOD_END()

    MOCK_STATIC_METHOD_1(, time_t, get_time, time_t*, t)
    MOCK_METHOD_END(time_t, time(t));

    MOCK_STATIC_METHOD_1(, STRING_HANDLE, STRING_clone, STRING_HANDLE, handle);
    MOCK_METHOD_END(STRING_HANDLE, TEST_STRING_HANDLE)

    MOCK_STATIC_METHOD_1(, STRING_HANDLE, STRING_construct, const char*, psz)
    MOCK_METHOD_END(STRING_HANDLE, TEST_STRING_HANDLE)

    MOCK_STATIC_METHOD_2(, int, STRING_concat, STRING_HANDLE, handle, const char*, s2)
    MOCK_METHOD_END(int, 0)

    MOCK_STATIC_METHOD_2(, int, STRING_concat_with_STRING, STRING_HANDLE, s1, STRING_HANDLE, s2)
    MOCK_METHOD_END(int, 0)

    MOCK_STATIC_METHOD_1(, void, STRING_delete, STRING_HANDLE, handle)
    MOCK_VOID_METHOD_END()

    MOCK_STATIC_METHOD_1(, const char*, STRING_c_str, STRING_HANDLE, s)
    MOCK_METHOD_END(const char*, TEST_CHAR)

    MOCK_STATIC_METHOD_0(, STRING_HANDLE, STRING_new)
    MOCK_METHOD_END(STRING_HANDLE, TEST_STRING_HANDLE);

    MOCK_STATIC_METHOD_1(, STRING_TOKENIZER_HANDLE, STRING_TOKENIZER_create, STRING_HANDLE, handle)
    MOCK_METHOD_END(STRING_TOKENIZER_HANDLE, TEST_STRING_TOKENIZER_HANDLE);

    MOCK_STATIC_METHOD_3(, int, STRING_TOKENIZER_get_next_token, STRING_TOKENIZER_HANDLE, t, STRING_HANDLE, output, const char*, delimiters)
    MOCK_METHOD_END(int, 0);

    MOCK_STATIC_METHOD_1(, void, STRING_TOKENIZER_destroy, STRING_TOKENIZER_HANDLE, handle)
    MOCK_VOID_METHOD_END()

    /* Version Mocks */
    MOCK_STATIC_METHOD_0(, const char*, IoTHubClient_GetVersionString)
    MOCK_METHOD_END(const char*, nullptr)
};

DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubClientLLMocks, , void, DList_InitializeListHead, PDLIST_ENTRY, listHead);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubClientLLMocks, , int, DList_IsListEmpty, PDLIST_ENTRY, listHead);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubClientLLMocks, , void, DList_InsertTailList, PDLIST_ENTRY, listHead, PDLIST_ENTRY, listEntry);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubClientLLMocks, , void, DList_AppendTailList, PDLIST_ENTRY, listHead, PDLIST_ENTRY, ListToAppend);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubClientLLMocks, , int, DList_RemoveEntryList, PDLIST_ENTRY, listEntry);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubClientLLMocks, , PDLIST_ENTRY, DList_RemoveHeadList, PDLIST_ENTRY, listHead);

DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubClientLLMocks, , void*, gballoc_malloc, size_t, size);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubClientLLMocks, , void*, gballoc_realloc, void*, ptr, size_t, size);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubClientLLMocks, , void, gballoc_free, void*, ptr)

DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubClientLLMocks, , IOTHUB_CLIENT_RESULT, FAKE_IoTHubTransport_SetOption, TRANSPORT_HANDLE, handle, const char*, optionName, const void*, value);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubClientLLMocks, , TRANSPORT_HANDLE, FAKE_IoTHubTransport_Create,const IOTHUBTRANSPORT_CONFIG*, config);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubClientLLMocks, , void, FAKE_IoTHubTransport_Destroy, TRANSPORT_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubClientLLMocks, , int, FAKE_IoTHubTransport_Subscribe, TRANSPORT_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubClientLLMocks, , void, FAKE_IoTHubTransport_Unsubscribe, TRANSPORT_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubClientLLMocks, , void, FAKE_IoTHubTransport_DoWork, TRANSPORT_HANDLE, handle, IOTHUB_CLIENT_LL_HANDLE, iotHubClientHandle);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubClientLLMocks, , IOTHUB_CLIENT_RESULT, FAKE_IoTHubTransport_GetSendStatus, TRANSPORT_HANDLE, handle, IOTHUB_CLIENT_STATUS*, iotHubClientStatus);

DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubClientLLMocks, , void, eventConfirmationCallback, IOTHUB_CLIENT_CONFIRMATION_RESULT, result2, void*, userContextCallback);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubClientLLMocks, , IOTHUBMESSAGE_DISPOSITION_RESULT, messageCallback, IOTHUB_MESSAGE_HANDLE, message, void*, userContextCallback);


DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubClientLLMocks, , IOTHUB_MESSAGE_HANDLE, IoTHubMessage_Clone, IOTHUB_MESSAGE_HANDLE, iotHubMessageHandle);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubClientLLMocks, , void, IoTHubMessage_Destroy, IOTHUB_MESSAGE_HANDLE, iotHubMessageHandle);

DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubClientLLMocks, , time_t, get_time, time_t*, t);

DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubClientLLMocks, , STRING_HANDLE, STRING_construct, const char*, psz);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubClientLLMocks, , STRING_HANDLE, STRING_clone, STRING_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubClientLLMocks, , int, STRING_concat, STRING_HANDLE, handle, const char*, s2);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubClientLLMocks, , int, STRING_concat_with_STRING, STRING_HANDLE, s1, STRING_HANDLE, s2);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubClientLLMocks, , void, STRING_delete, STRING_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubClientLLMocks, , const char*, STRING_c_str, STRING_HANDLE, s);
DECLARE_GLOBAL_MOCK_METHOD_0(CIoTHubClientLLMocks, , STRING_HANDLE, STRING_new);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubClientLLMocks, , STRING_TOKENIZER_HANDLE, STRING_TOKENIZER_create, STRING_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubClientLLMocks, , int, STRING_TOKENIZER_get_next_token, STRING_TOKENIZER_HANDLE, t, STRING_HANDLE, output, const char*, delimiters);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubClientLLMocks, , void, STRING_TOKENIZER_destroy, STRING_TOKENIZER_HANDLE, handle);

DECLARE_GLOBAL_MOCK_METHOD_0(CIoTHubClientLLMocks, , const char*, IoTHubClient_GetVersionString);

static TRANSPORT_PROVIDER FAKE_transport_provider =
{
    FAKE_IoTHubTransport_SetOption,     /*pfIoTHubTransport_SetOption IoTHubTransport_SetOption;       */
    FAKE_IoTHubTransport_Create,        /*pfIoTHubTransport_Create IoTHubTransport_Create;              */
    FAKE_IoTHubTransport_Destroy,       /*pfIoTHubTransport_Destroy IoTHubTransport_Destroy;            */
    FAKE_IoTHubTransport_Subscribe,     /*pfIoTHubTransport_Subscribe IoTHubTransport_Subscribe;        */
    FAKE_IoTHubTransport_Unsubscribe,   /*pfIoTHubTransport_Unsubscribe IoTHubTransport_Unsubscribe;    */
    FAKE_IoTHubTransport_DoWork,        /*pfIoTHubTransport_DoWork IoTHubTransport_DoWork;              */
    FAKE_IoTHubTransport_GetSendStatus  /*pfIoTHubTransport_GetSendStatus IoTHubTransport_GetSendStatus; */
};

static const void* provideFAKE(void)
{
    return &FAKE_transport_provider; /*by convention... */
}

BEGIN_TEST_SUITE(iothubclient_ll_unittests)

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
    }

    TEST_FUNCTION_CLEANUP(TestMethodCleanup)
    {
        if (!MicroMockReleaseMutex(g_testByTest))
        {
            ASSERT_FAIL("failure in test framework at ReleaseMutex");
        }
    }

    /*Tests_SRS_IOTHUBCLIENT_LL_05_001: [IoTHubClient_LL_CreateFromConnectionString shall obtain the version string by a call to IoTHubClient_GetVersionString.]*/
    TEST_FUNCTION(IoTHubClient_LL_CreateFromConnectionString_gets_the_version_string)
    {
        // arrange
        CNiceCallComparer<CIoTHubClientLLMocks> mocks;
        EXPECTED_CALL(mocks, IoTHubClient_GetVersionString());

        // act
        (void)IoTHubClient_LL_CreateFromConnectionString(NULL, NULL);

        // assert
        // Implicit
    }

    /* SRS_IOTHUBCLIENT_LL_12_011: [IoTHubClient_LL_CreateFromConnectionString shall call into the IoTHubClient_LL_Create API with the current structure and returns with the return value of it] */
    /* SRS_IOTHUBCLIENT_LL_12_010: [IoTHubClient_LL_CreateFromConnectionString shall fill up the IOTHUB_CLIENT_CONFIG structure using the following mapping: iotHubName = Name, iotHubSuffix = Suffix, deviceId = DeviceId, deviceKey = SharedAccessKey] */
    TEST_FUNCTION(IoTHubClient_LL_CreateFromConnectionString_succeeds)
    {
        ///arrange
        CIoTHubClientLLMocks mocks;
        STRICT_EXPECTED_CALL(mocks, IoTHubClient_GetVersionString());

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_CHAR));
        STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_create(TEST_STRING_HANDLE));
        STRICT_EXPECTED_CALL(mocks, STRING_new());
        STRICT_EXPECTED_CALL(mocks, STRING_new());
        STRICT_EXPECTED_CALL(mocks, STRING_new());
        STRICT_EXPECTED_CALL(mocks, STRING_new());

        /* loop 1 */
        EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, IGNORED_PTR_ARG))
            .SetReturn(0);
        EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, IGNORED_PTR_ARG))
            .SetReturn(0);
        EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
            .SetReturn(TEST_HOSTNAME_TOKEN);
        EXPECTED_CALL(mocks, STRING_TOKENIZER_create(TEST_STRING_HANDLE));
        EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, IGNORED_PTR_ARG))
            .SetReturn(0);
        EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, IGNORED_PTR_ARG))
            .SetReturn(0);
        EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, STRING_TOKENIZER_destroy(IGNORED_PTR_ARG));

        /* loop 2 */
        EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, IGNORED_PTR_ARG))
            .SetReturn(0);
        EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, IGNORED_PTR_ARG))
            .SetReturn(0);
        STRICT_EXPECTED_CALL(mocks, STRING_clone(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
            .SetReturn(TEST_DEVICEID_TOKEN);
        EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
            .SetReturn(TEST_CHAR);

        /* loop 3*/
        EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, IGNORED_PTR_ARG))
            .SetReturn(0);
        EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, IGNORED_PTR_ARG))
            .SetReturn(0);
        STRICT_EXPECTED_CALL(mocks, STRING_clone(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
            .SetReturn(TEST_DEVICEKEY_TOKEN);
        EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
            .SetReturn(TEST_CHAR);

        /* loop exit */
        EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, IGNORED_PTR_ARG))
            .SetReturn(1);

        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));

        EXPECTED_CALL(mocks, STRING_TOKENIZER_destroy(IGNORED_PTR_ARG));

        /* underlying IoTHubClient_LL_Create call */
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, DList_InitializeListHead(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, FAKE_IoTHubTransport_Create(IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .SetReturn((TRANSPORT_HANDLE)0x42);
        
        ///act
        auto result = IoTHubClient_LL_CreateFromConnectionString(TEST_CHAR, provideFAKE);

        ///assert
        ASSERT_ARE_NOT_EQUAL(void_ptr, NULL, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubClient_LL_Destroy(result);
    }

    /* SRS_IOTHUBCLIENT_LL_12_003: [IoTHubClient_LL_CreateFromConnectionString shall verify the input parameters and if any of them NULL then return NULL] */
    TEST_FUNCTION(IoTHubClient_LL_CreateFromConnectionString_if_input_parameter_connectionString_is_NULL_then_return_NULL)
    {
        ///arrange
        CIoTHubClientLLMocks mocks;
        STRICT_EXPECTED_CALL(mocks, IoTHubClient_GetVersionString());

        ///act
        auto result = IoTHubClient_LL_CreateFromConnectionString(NULL, provideFAKE);

        ///assert
        ASSERT_ARE_EQUAL(void_ptr, NULL, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubClient_LL_Destroy(result);
    }

    /* SRS_IOTHUBCLIENT_LL_12_003: [IoTHubClient_LL_CreateFromConnectionString shall return NULL if any of the input parameter is NULL] */
    TEST_FUNCTION(IoTHubClient_LL_CreateFromConnectionString_if_input_parameter_protocol_is_NULL_then_return_NULL)
    {
        ///arrange
        CIoTHubClientLLMocks mocks;
        STRICT_EXPECTED_CALL(mocks, IoTHubClient_GetVersionString());

        ///act
        auto result = IoTHubClient_LL_CreateFromConnectionString(TEST_CHAR, NULL);

        ///assert
        ASSERT_ARE_EQUAL(void_ptr, NULL, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubClient_LL_Destroy(result);
    }

    /* SRS_IOTHUBCLIENT_LL_12_004: [IoTHubClient_LL_CreateFromConnectionString shall allocate IOTHUB_CLIENT_CONFIG structure] */
    TEST_FUNCTION(IoTHubClient_LL_CreateFromConnectionString_config_structure_allocation_fails)
    {
        ///arrange
        CIoTHubClientLLMocks mocks;

        STRICT_EXPECTED_CALL(mocks, IoTHubClient_GetVersionString());

        whenShallmalloc_fail = 1;
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);

        ///act
        auto result = IoTHubClient_LL_CreateFromConnectionString(TEST_CHAR, provideFAKE);

        ///assert
        ASSERT_ARE_EQUAL(void_ptr, NULL, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubClient_LL_Destroy(result);
    }

    /* SRS_IOTHUBCLIENT_LL_12_016: [IoTHubClient_LL_CreateFromConnectionString shall return NULL if IoTHubClient_LL_Create call fails] */
    TEST_FUNCTION(IoTHubClient_LL_CreateFromConnectionString_if_IoTHubClient_LL_Create_fails_then_IoTHubClient_LL_CreateFromConnectionString_return_NULL)
    {
        ///arrange
        CIoTHubClientLLMocks mocks;
        STRICT_EXPECTED_CALL(mocks, IoTHubClient_GetVersionString());

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_CHAR));
        STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_create(TEST_STRING_HANDLE));
        STRICT_EXPECTED_CALL(mocks, STRING_new());
        STRICT_EXPECTED_CALL(mocks, STRING_new());
        STRICT_EXPECTED_CALL(mocks, STRING_new());
        STRICT_EXPECTED_CALL(mocks, STRING_new());

        /* loop 1 */
        EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, IGNORED_PTR_ARG))
            .SetReturn(0);
        EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, IGNORED_PTR_ARG))
            .SetReturn(0);
        EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
            .SetReturn(TEST_HOSTNAME_TOKEN);
        EXPECTED_CALL(mocks, STRING_TOKENIZER_create(TEST_STRING_HANDLE));
        EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, IGNORED_PTR_ARG))
            .SetReturn(0);
        EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, IGNORED_PTR_ARG))
            .SetReturn(0);
        EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, STRING_TOKENIZER_destroy(IGNORED_PTR_ARG));

        /* loop 2 */
        EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, IGNORED_PTR_ARG))
            .SetReturn(0);
        EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, IGNORED_PTR_ARG))
            .SetReturn(0);
        EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
            .SetReturn(TEST_DEVICEID_TOKEN);
        STRICT_EXPECTED_CALL(mocks, STRING_clone(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
            .SetReturn(TEST_CHAR);

        /* loop 3*/
        EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, IGNORED_PTR_ARG))
            .SetReturn(0);
        EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, IGNORED_PTR_ARG))
            .SetReturn(0);
        EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
            .SetReturn(TEST_DEVICEKEY_TOKEN);
        STRICT_EXPECTED_CALL(mocks, STRING_clone(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
            .SetReturn(TEST_CHAR);

        /* loop exit */
        EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, IGNORED_PTR_ARG))
            .SetReturn(1);

        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));

        EXPECTED_CALL(mocks, STRING_TOKENIZER_destroy(IGNORED_PTR_ARG));

        /* underlying IoTHubClient_LL_Create call */
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORE))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, DList_InitializeListHead(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, FAKE_IoTHubTransport_Create(IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .SetReturn((TRANSPORT_HANDLE)NULL);

        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        ///act
        auto result = IoTHubClient_LL_CreateFromConnectionString(TEST_CHAR, provideFAKE);

        ///assert
        ASSERT_ARE_EQUAL(void_ptr, NULL, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubClient_LL_Destroy(result);
    }

    /* SRS_IOTHUBCLIENT_LL_12_006: [IoTHubClient_LL_CreateFromConnectionString shall verify the existence of the following Key/Value pairs in the connection string: HostName, DeviceId, SharedAccessKey.]  */
    /* SRS_IOTHUBCLIENT_LL_12_014: [If either of key is missing then IoTHubClient_LL_CreateFromConnectionString returns NULL ] */
    TEST_FUNCTION(IoTHubClient_LL_CreateFromConnectionString_if_hostName_missing_then_return_NULL)
    {
        ///arrange
        CIoTHubClientLLMocks mocks;
        STRICT_EXPECTED_CALL(mocks, IoTHubClient_GetVersionString());

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_CHAR));
        STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_create(TEST_STRING_HANDLE));
        STRICT_EXPECTED_CALL(mocks, STRING_new());
		STRICT_EXPECTED_CALL(mocks, STRING_new());
        STRICT_EXPECTED_CALL(mocks, STRING_new());
        STRICT_EXPECTED_CALL(mocks, STRING_new());

        /* loop 1 */
        EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, IGNORED_PTR_ARG))
            .SetReturn(0);
        EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, IGNORED_PTR_ARG))
            .SetReturn(0);
        STRICT_EXPECTED_CALL(mocks, STRING_clone(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
            .SetReturn(TEST_DEVICEID_TOKEN);
        EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
            .SetReturn(TEST_CHAR);

        /* loop 2*/
        EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, IGNORED_PTR_ARG))
            .SetReturn(0);
        EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, IGNORED_PTR_ARG))
            .SetReturn(0);
        STRICT_EXPECTED_CALL(mocks, STRING_clone(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
            .SetReturn(TEST_DEVICEKEY_TOKEN);
        EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
            .SetReturn(TEST_CHAR);

        /* loop exit */
        EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, IGNORED_PTR_ARG))
            .SetReturn(1);

        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));

        EXPECTED_CALL(mocks, STRING_TOKENIZER_destroy(IGNORED_PTR_ARG));

        ///act
        auto result = IoTHubClient_LL_CreateFromConnectionString(TEST_CHAR, provideFAKE);

        ///assert
        ASSERT_ARE_EQUAL(void_ptr, NULL, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubClient_LL_Destroy(result);
    }

    /* SRS_IOTHUBCLIENT_LL_12_006: [IoTHubClient_LL_CreateFromConnectionString shall verify the existence of the following Key/Value pairs in the connection string: HostName, DeviceId, SharedAccessKey.]  */
    /* SRS_IOTHUBCLIENT_LL_12_014: [If either of key is missing then IoTHubClient_LL_CreateFromConnectionString returns NULL ] */
    TEST_FUNCTION(IoTHubClient_LL_CreateFromConnectionString_if_deviceId_missing_then_return_NULL)
    {
        ///arrange
        CIoTHubClientLLMocks mocks;
        STRICT_EXPECTED_CALL(mocks, IoTHubClient_GetVersionString());

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_CHAR));
        STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_create(TEST_STRING_HANDLE));
        STRICT_EXPECTED_CALL(mocks, STRING_new());
        STRICT_EXPECTED_CALL(mocks, STRING_new());
        STRICT_EXPECTED_CALL(mocks, STRING_new());
        STRICT_EXPECTED_CALL(mocks, STRING_new());

        /* loop 1 */
        EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, IGNORED_PTR_ARG))
            .SetReturn(0);
        EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, IGNORED_PTR_ARG))
            .SetReturn(0);
        EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
            .SetReturn(TEST_HOSTNAME_TOKEN);
        EXPECTED_CALL(mocks, STRING_TOKENIZER_create(TEST_STRING_HANDLE));
        EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, IGNORED_PTR_ARG))
            .SetReturn(0);
        EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, IGNORED_PTR_ARG))
            .SetReturn(0);
        EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, STRING_TOKENIZER_destroy(IGNORED_PTR_ARG));

        /* loop 2*/
        EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, IGNORED_PTR_ARG))
            .SetReturn(0);
        EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, IGNORED_PTR_ARG))
            .SetReturn(0);
        STRICT_EXPECTED_CALL(mocks, STRING_clone(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
            .SetReturn(TEST_DEVICEKEY_TOKEN);
        EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
            .SetReturn(TEST_CHAR);


        /* loop exit */
        EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, IGNORED_PTR_ARG))
            .SetReturn(1);

        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));

        EXPECTED_CALL(mocks, STRING_TOKENIZER_destroy(IGNORED_PTR_ARG));

        ///act
        auto result = IoTHubClient_LL_CreateFromConnectionString(TEST_CHAR, provideFAKE);

        ///assert
        ASSERT_ARE_EQUAL(void_ptr, NULL, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubClient_LL_Destroy(result);
    }

    /* SRS_IOTHUBCLIENT_LL_12_006: [IoTHubClient_LL_CreateFromConnectionString shall verify the existence of the following Key/Value pairs in the connection string: HostName, DeviceId, SharedAccessKey.]  */
    /* SRS_IOTHUBCLIENT_LL_12_014: [If either of key is missing then IoTHubClient_LL_CreateFromConnectionString returns NULL ] */
    TEST_FUNCTION(IoTHubClient_LL_CreateFromConnectionString_if_deviceKey_missing_then_return_NULL)
    {
        ///arrange
        CIoTHubClientLLMocks mocks;
        STRICT_EXPECTED_CALL(mocks, IoTHubClient_GetVersionString());

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_CHAR));
        STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_create(TEST_STRING_HANDLE));
        STRICT_EXPECTED_CALL(mocks, STRING_new());
        STRICT_EXPECTED_CALL(mocks, STRING_new());
        STRICT_EXPECTED_CALL(mocks, STRING_new());
        STRICT_EXPECTED_CALL(mocks, STRING_new());

        /* loop 1 */
        EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, IGNORED_PTR_ARG))
            .SetReturn(0);
        EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, IGNORED_PTR_ARG))
            .SetReturn(0);
        EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
            .SetReturn(TEST_HOSTNAME_TOKEN);
        EXPECTED_CALL(mocks, STRING_TOKENIZER_create(TEST_STRING_HANDLE));
        EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, IGNORED_PTR_ARG))
            .SetReturn(0);
        EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, IGNORED_PTR_ARG))
            .SetReturn(0);
        EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, STRING_TOKENIZER_destroy(IGNORED_PTR_ARG));

        /* loop 2*/
        EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, IGNORED_PTR_ARG))
            .SetReturn(0);
        EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, IGNORED_PTR_ARG))
            .SetReturn(0);
        STRICT_EXPECTED_CALL(mocks, STRING_clone(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
            .SetReturn(TEST_DEVICEID_TOKEN);
        EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
            .SetReturn(TEST_CHAR);

        /* loop exit */
        EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, IGNORED_PTR_ARG))
            .SetReturn(1);

        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));

        EXPECTED_CALL(mocks, STRING_TOKENIZER_destroy(IGNORED_PTR_ARG));

        ///act
        auto result = IoTHubClient_LL_CreateFromConnectionString(TEST_CHAR, provideFAKE);

        ///assert
        ASSERT_ARE_EQUAL(void_ptr, NULL, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubClient_LL_Destroy(result);
    }

    /* Codes_SRS_IOTHUBCLIENT_LL_12_009: [IoTHubClient_LL_CreateFromConnectionString shall split the value of HostName to Name and Suffix using the first "." as a separator] */
    /* Codes_SRS_IOTHUBCLIENT_LL_12_015: [If the string split failed, IoTHubClient_LL_CreateFromConnectionString returns NULL ] */
    TEST_FUNCTION(IoTHubClient_LL_CreateFromConnectionString_if_iotHubName_is_NULL_return_NULL)
    {
        ///arrange
        CIoTHubClientLLMocks mocks;
        STRICT_EXPECTED_CALL(mocks, IoTHubClient_GetVersionString());

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_CHAR));
        STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_create(TEST_STRING_HANDLE));
        STRICT_EXPECTED_CALL(mocks, STRING_new());
        STRICT_EXPECTED_CALL(mocks, STRING_new());
        STRICT_EXPECTED_CALL(mocks, STRING_new());
        STRICT_EXPECTED_CALL(mocks, STRING_new());

        /* loop 1 */
        EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, IGNORED_PTR_ARG))
            .SetReturn(0);
        EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, IGNORED_PTR_ARG))
            .SetReturn(0);
        EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
            .SetReturn(TEST_HOSTNAME_TOKEN);
        EXPECTED_CALL(mocks, STRING_TOKENIZER_create(TEST_STRING_HANDLE));
        EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, IGNORED_PTR_ARG))
            .SetReturn(0);
        EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
            .SetReturn((const char*)NULL);
        EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, IGNORED_PTR_ARG))
            .SetReturn(0);
        EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, STRING_TOKENIZER_destroy(IGNORED_PTR_ARG));

        /* loop 2 */
        EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, IGNORED_PTR_ARG))
            .SetReturn(0);
        EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, IGNORED_PTR_ARG))
            .SetReturn(0);
        STRICT_EXPECTED_CALL(mocks, STRING_clone(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
            .SetReturn(TEST_DEVICEID_TOKEN);
        EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
            .SetReturn(TEST_CHAR);

        /* loop 3*/
        EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, IGNORED_PTR_ARG))
            .SetReturn(0);
        EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, IGNORED_PTR_ARG))
            .SetReturn(0);
        STRICT_EXPECTED_CALL(mocks, STRING_clone(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
            .SetReturn(TEST_DEVICEKEY_TOKEN);
        EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
            .SetReturn(TEST_CHAR);

        /* loop exit */
        EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, IGNORED_PTR_ARG))
            .SetReturn(1);

        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));

        EXPECTED_CALL(mocks, STRING_TOKENIZER_destroy(IGNORED_PTR_ARG));

        ///act
        auto result = IoTHubClient_LL_CreateFromConnectionString(TEST_CHAR, provideFAKE);

        ///assert
        ASSERT_ARE_EQUAL(void_ptr, NULL, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubClient_LL_Destroy(result);
    }

    /* SRS_IOTHUBCLIENT_LL_12_009: [IoTHubClient_LL_CreateFromConnectionString shall split the value of HostName to Name and Suffix using the first "." as a separator] */
    /* SRS_IOTHUBCLIENT_LL_12_015: [If the string split failed, IoTHubClient_LL_CreateFromConnectionString returns NULL ] */
    TEST_FUNCTION(IoTHubClient_LL_CreateFromConnectionString_if_iotHubSuffix_is_NULL_return_NULL)
    {
        ///arrange
        CIoTHubClientLLMocks mocks;
        STRICT_EXPECTED_CALL(mocks, IoTHubClient_GetVersionString());

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_CHAR));
        STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_create(TEST_STRING_HANDLE));
        STRICT_EXPECTED_CALL(mocks, STRING_new());
        STRICT_EXPECTED_CALL(mocks, STRING_new());
        STRICT_EXPECTED_CALL(mocks, STRING_new());
        STRICT_EXPECTED_CALL(mocks, STRING_new());

        /* loop 1 */
        EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, IGNORED_PTR_ARG))
            .SetReturn(0);
        EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, IGNORED_PTR_ARG))
            .SetReturn(0);
        EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
            .SetReturn(TEST_HOSTNAME_TOKEN);
        EXPECTED_CALL(mocks, STRING_TOKENIZER_create(TEST_STRING_HANDLE));
        EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, IGNORED_PTR_ARG))
            .SetReturn(0);
        EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, IGNORED_PTR_ARG))
            .SetReturn(0);
        EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
            .SetReturn((const char*)NULL);
        EXPECTED_CALL(mocks, STRING_TOKENIZER_destroy(IGNORED_PTR_ARG));

        /* loop 2 */
        EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, IGNORED_PTR_ARG))
            .SetReturn(0);
        EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, IGNORED_PTR_ARG))
            .SetReturn(0);
        STRICT_EXPECTED_CALL(mocks, STRING_clone(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
            .SetReturn(TEST_DEVICEID_TOKEN);
        EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
            .SetReturn(TEST_CHAR);

        /* loop 3*/
        EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, IGNORED_PTR_ARG))
            .SetReturn(0);
        EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, IGNORED_PTR_ARG))
            .SetReturn(0);
        STRICT_EXPECTED_CALL(mocks, STRING_clone(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
            .SetReturn(TEST_DEVICEKEY_TOKEN);
        EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
            .SetReturn(TEST_CHAR);

        /* loop exit */
        EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, IGNORED_PTR_ARG))
            .SetReturn(1);

        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));

        EXPECTED_CALL(mocks, STRING_TOKENIZER_destroy(IGNORED_PTR_ARG));

        ///act
        auto result = IoTHubClient_LL_CreateFromConnectionString(TEST_CHAR, provideFAKE);

        ///assert
        ASSERT_ARE_EQUAL(void_ptr, NULL, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubClient_LL_Destroy(result);
    }

    /* SRS_IOTHUBCLIENT_LL_12_013: [If the parsing failed IoTHubClient_LL_CreateFromConnectionString returns NULL]  */
    TEST_FUNCTION(IoTHubClient_LL_CreateFromConnectionString_STRING_construct_fails)
    {
        ///arrange
        CIoTHubClientLLMocks mocks;
        STRICT_EXPECTED_CALL(mocks, IoTHubClient_GetVersionString());

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);


        STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_CHAR))
            .IgnoreArgument(1)
            .SetReturn((STRING_TOKENIZER_HANDLE)NULL);

        ///act
        auto result = IoTHubClient_LL_CreateFromConnectionString(TEST_CHAR, provideFAKE);

        ///assert
        ASSERT_ARE_EQUAL(void_ptr, NULL, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubClient_LL_Destroy(result);
    }

    /* SRS_IOTHUBCLIENT_LL_12_013: [If the parsing failed IoTHubClient_LL_CreateFromConnectionString returns NULL]  */
    TEST_FUNCTION(IoTHubClient_LL_CreateFromConnectionString_STRING_tokenizer_create_fails_1)
    {
        ///arrange
        CIoTHubClientLLMocks mocks;
        STRICT_EXPECTED_CALL(mocks, IoTHubClient_GetVersionString());

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_CHAR));
        STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_create(TEST_STRING_HANDLE))
            .IgnoreArgument(1)
            .SetReturn((STRING_TOKENIZER_HANDLE)NULL);

        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));

        ///act
        auto result = IoTHubClient_LL_CreateFromConnectionString(TEST_CHAR, provideFAKE);

        ///assert
        ASSERT_ARE_EQUAL(void_ptr, NULL, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubClient_LL_Destroy(result);
    }

    /* SRS_IOTHUBCLIENT_LL_12_013: [If the parsing failed IoTHubClient_LL_CreateFromConnectionString returns NULL]  */
    TEST_FUNCTION(IoTHubClient_LL_CreateFromConnectionString_STRING_tokenizer_create_fails_2)
    {
        ///arrange
        CIoTHubClientLLMocks mocks;
        STRICT_EXPECTED_CALL(mocks, IoTHubClient_GetVersionString());

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_CHAR));
        STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_create(TEST_STRING_HANDLE))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_new());
        STRICT_EXPECTED_CALL(mocks, STRING_new());
        STRICT_EXPECTED_CALL(mocks, STRING_new());
        STRICT_EXPECTED_CALL(mocks, STRING_new());

        /* loop 1 */
        EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, IGNORED_PTR_ARG))
            .SetReturn(0);
        EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, IGNORED_PTR_ARG))
            .SetReturn(0);
        EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
            .SetReturn(TEST_HOSTNAME_TOKEN);
        STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_create(TEST_STRING_HANDLE))
            .IgnoreArgument(1)
            .SetReturn((STRING_TOKENIZER_HANDLE)NULL);

        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));

        EXPECTED_CALL(mocks, STRING_TOKENIZER_destroy(IGNORED_PTR_ARG));

        ///act
        auto result = IoTHubClient_LL_CreateFromConnectionString(TEST_CHAR, provideFAKE);

        ///assert
        ASSERT_ARE_EQUAL(void_ptr, NULL, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubClient_LL_Destroy(result);
    }

    /* SRS_IOTHUBCLIENT_LL_12_013: [If the parsing failed IoTHubClient_LL_CreateFromConnectionString returns NULL]  */
    TEST_FUNCTION(IoTHubClient_LL_CreateFromConnectionString_STRING_new_fails_1)
    {
        ///arrange
        CIoTHubClientLLMocks mocks;
        STRICT_EXPECTED_CALL(mocks, IoTHubClient_GetVersionString());

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_CHAR));
        STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_create(TEST_STRING_HANDLE))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_new())
            .SetReturn((STRING_HANDLE)NULL);

        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));

        EXPECTED_CALL(mocks, STRING_TOKENIZER_destroy(IGNORED_PTR_ARG));

        ///act
        auto result = IoTHubClient_LL_CreateFromConnectionString(TEST_CHAR, provideFAKE);

        ///assert
        ASSERT_ARE_EQUAL(void_ptr, NULL, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubClient_LL_Destroy(result);
    }

    /* SRS_IOTHUBCLIENT_LL_12_013: [If the parsing failed IoTHubClient_LL_CreateFromConnectionString returns NULL]  */
    TEST_FUNCTION(IoTHubClient_LL_CreateFromConnectionString_STRING_new_fails_2)
    {
        ///arrange
        CIoTHubClientLLMocks mocks;
        STRICT_EXPECTED_CALL(mocks, IoTHubClient_GetVersionString());

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_CHAR));
        STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_create(TEST_STRING_HANDLE))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_new());
        STRICT_EXPECTED_CALL(mocks, STRING_new())
            .SetReturn((STRING_HANDLE)NULL);

        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));

        EXPECTED_CALL(mocks, STRING_TOKENIZER_destroy(IGNORED_PTR_ARG));

        ///act
        auto result = IoTHubClient_LL_CreateFromConnectionString(TEST_CHAR, provideFAKE);

        ///assert
        ASSERT_ARE_EQUAL(void_ptr, NULL, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubClient_LL_Destroy(result);
    }

    /* SRS_IOTHUBCLIENT_LL_12_013: [If the parsing failed IoTHubClient_LL_CreateFromConnectionString returns NULL]  */
    TEST_FUNCTION(IoTHubClient_LL_CreateFromConnectionString_STRING_new_fails_3)
    {
        ///arrange
        CIoTHubClientLLMocks mocks;
        STRICT_EXPECTED_CALL(mocks, IoTHubClient_GetVersionString());

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_CHAR));
        STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_create(TEST_STRING_HANDLE))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_new());
        STRICT_EXPECTED_CALL(mocks, STRING_new());
        STRICT_EXPECTED_CALL(mocks, STRING_new())
            .SetReturn((STRING_HANDLE)NULL);

        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));

        EXPECTED_CALL(mocks, STRING_TOKENIZER_destroy(IGNORED_PTR_ARG));

        ///act
        auto result = IoTHubClient_LL_CreateFromConnectionString(TEST_CHAR, provideFAKE);

        ///assert
        ASSERT_ARE_EQUAL(void_ptr, NULL, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubClient_LL_Destroy(result);
    }

    /* SRS_IOTHUBCLIENT_LL_12_013: [If the parsing failed IoTHubClient_LL_CreateFromConnectionString returns NULL]  */
    TEST_FUNCTION(IoTHubClient_LL_CreateFromConnectionString_STRING_new_fails_4)
    {
        ///arrange
        CIoTHubClientLLMocks mocks;
        STRICT_EXPECTED_CALL(mocks, IoTHubClient_GetVersionString());

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_CHAR));
        STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_create(TEST_STRING_HANDLE))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_new());
        STRICT_EXPECTED_CALL(mocks, STRING_new());
        STRICT_EXPECTED_CALL(mocks, STRING_new());
        STRICT_EXPECTED_CALL(mocks, STRING_new())
            .SetReturn((STRING_HANDLE)NULL);

        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));

        EXPECTED_CALL(mocks, STRING_TOKENIZER_destroy(IGNORED_PTR_ARG));

        ///act
        auto result = IoTHubClient_LL_CreateFromConnectionString(TEST_CHAR, provideFAKE);

        ///assert
        ASSERT_ARE_EQUAL(void_ptr, NULL, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubClient_LL_Destroy(result);
    }

    /* SRS_IOTHUBCLIENT_LL_12_013: [If the parsing failed IoTHubClient_LL_CreateFromConnectionString returns NULL]  */
    TEST_FUNCTION(IoTHubClient_LL_CreateFromConnectionString_STRING_get_next_token_fails_1)
    {
        ///arrange
        CIoTHubClientLLMocks mocks;
        STRICT_EXPECTED_CALL(mocks, IoTHubClient_GetVersionString());

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_CHAR));
        STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_create(TEST_STRING_HANDLE))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_new());
        STRICT_EXPECTED_CALL(mocks, STRING_new());
        STRICT_EXPECTED_CALL(mocks, STRING_new());
        STRICT_EXPECTED_CALL(mocks, STRING_new());

        EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, IGNORED_PTR_ARG))
            .SetReturn(1);

        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));

        EXPECTED_CALL(mocks, STRING_TOKENIZER_destroy(IGNORED_PTR_ARG));

        ///act
        auto result = IoTHubClient_LL_CreateFromConnectionString(TEST_CHAR, provideFAKE);

        ///assert
        ASSERT_ARE_EQUAL(void_ptr, NULL, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubClient_LL_Destroy(result);
    }

    /* SRS_IOTHUBCLIENT_LL_12_013: [If the parsing failed IoTHubClient_LL_CreateFromConnectionString returns NULL]  */
    TEST_FUNCTION(IoTHubClient_LL_CreateFromConnectionString_STRING_get_next_token_fails_2)
    {
        ///arrange
        CIoTHubClientLLMocks mocks;
        STRICT_EXPECTED_CALL(mocks, IoTHubClient_GetVersionString());

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_CHAR));
        STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_create(TEST_STRING_HANDLE))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_new());
        STRICT_EXPECTED_CALL(mocks, STRING_new());
        STRICT_EXPECTED_CALL(mocks, STRING_new());
        STRICT_EXPECTED_CALL(mocks, STRING_new());

        EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, IGNORED_PTR_ARG))
            .SetReturn(1);

        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));

        EXPECTED_CALL(mocks, STRING_TOKENIZER_destroy(IGNORED_PTR_ARG));

        ///act
        auto result = IoTHubClient_LL_CreateFromConnectionString(TEST_CHAR, provideFAKE);

        ///assert
        ASSERT_ARE_EQUAL(void_ptr, NULL, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubClient_LL_Destroy(result);
    }

    /* SRS_IOTHUBCLIENT_LL_12_013: [If the parsing failed IoTHubClient_LL_CreateFromConnectionString returns NULL]  */
    TEST_FUNCTION(IoTHubClient_LL_CreateFromConnectionString_STRING_get_next_token_fails_3)
    {
        ///arrange
        CIoTHubClientLLMocks mocks;
        STRICT_EXPECTED_CALL(mocks, IoTHubClient_GetVersionString());

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_CHAR));
        STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_create(TEST_STRING_HANDLE))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_new());
        STRICT_EXPECTED_CALL(mocks, STRING_new());
        STRICT_EXPECTED_CALL(mocks, STRING_new());
        STRICT_EXPECTED_CALL(mocks, STRING_new());

        /* loop 1 */
        EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, IGNORED_PTR_ARG))
            .SetReturn(0);
        EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, IGNORED_PTR_ARG))
            .SetReturn(0);
        EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
            .SetReturn(TEST_HOSTNAME_TOKEN);
        EXPECTED_CALL(mocks, STRING_TOKENIZER_create(TEST_STRING_HANDLE));
        EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, IGNORED_PTR_ARG))
            .SetReturn(1);

        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));

        EXPECTED_CALL(mocks, STRING_TOKENIZER_destroy(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, STRING_TOKENIZER_destroy(IGNORED_PTR_ARG));

        ///act
        auto result = IoTHubClient_LL_CreateFromConnectionString(TEST_CHAR, provideFAKE);

        ///assert
        ASSERT_ARE_EQUAL(void_ptr, NULL, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubClient_LL_Destroy(result);
    }

    /* SRS_IOTHUBCLIENT_LL_12_013: [If the parsing failed IoTHubClient_LL_CreateFromConnectionString returns NULL]  */
    TEST_FUNCTION(IoTHubClient_LL_CreateFromConnectionString_STRING_get_next_token_fails_4)
    {
        ///arrange
        CIoTHubClientLLMocks mocks;
        STRICT_EXPECTED_CALL(mocks, IoTHubClient_GetVersionString());

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_CHAR));
        STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_create(TEST_STRING_HANDLE))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, STRING_new());
        STRICT_EXPECTED_CALL(mocks, STRING_new());
        STRICT_EXPECTED_CALL(mocks, STRING_new());
        STRICT_EXPECTED_CALL(mocks, STRING_new());

        /* loop 1 */
        EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, IGNORED_PTR_ARG))
            .SetReturn(0);
        EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, IGNORED_PTR_ARG))
            .SetReturn(0);
        EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
            .SetReturn(TEST_HOSTNAME_TOKEN);
        EXPECTED_CALL(mocks, STRING_TOKENIZER_create(TEST_STRING_HANDLE));
        EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, IGNORED_PTR_ARG))
            .SetReturn(1);

        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));

        EXPECTED_CALL(mocks, STRING_TOKENIZER_destroy(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, STRING_TOKENIZER_destroy(IGNORED_PTR_ARG));

        ///act
        auto result = IoTHubClient_LL_CreateFromConnectionString(TEST_CHAR, provideFAKE);

        ///assert
        ASSERT_ARE_EQUAL(void_ptr, NULL, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubClient_LL_Destroy(result);
    }

    /*Tests_SRS_IOTHUBCLIENT_LL_02_001: [IoTHubClient_LL_Create shall return NULL if config parameter is NULL or protocol field is NULL.]*/
    TEST_FUNCTION(IoTHubClient_LL_Create_with_NULL_parameter_fails)
    {
        // arrange

        // act
        auto result = IoTHubClient_LL_Create(NULL);

        // assert
        ASSERT_ARE_EQUAL(void_ptr, NULL, result);
    }

    /*Tests_SRS_IOTHUBCLIENT_LL_02_001: [IoTHubClient_LL_Create shall return NULL if config parameter is NULL or protocol field is NULL.]*/
    TEST_FUNCTION(IoTHubClient_LL_Create_with_NULL_protocol_fails)
    {
        // arrange

        // act
        auto result = IoTHubClient_LL_Create(&TEST_CONFIG_NULL_protocol);

        // assert
        ASSERT_ARE_EQUAL(void_ptr, NULL, result);
    }

    TEST_FUNCTION(IoTHubClient_LL_Create_fails_when_malloc_fails)
    {
        ///arrange
        CIoTHubClientLLMocks mocks;
        whenShallmalloc_fail = 1;
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORE))
            .IgnoreArgument(1);

        ///act
        auto result = IoTHubClient_LL_Create(&TEST_CONFIG);

        ///assert
        ASSERT_ARE_EQUAL(void_ptr, NULL, result);
    }

    /*Tests_SRS_IOTHUBCLIENT_LL_02_004: [Otherwise IoTHubClient_LL_Create shall initialize a new DLIST (further called "waitingToSend") containing records with fields of the following types: IOTHUB_MESSAGE_HANDLE, IOTHUB_CLIENT_EVENT_CONFIRMATION_CALLBACK, void*.]*/
    /*Tests_SRS_IOTHUBCLIENT_LL_02_006: [IoTHubClient_LL_Create shall populate a structure of type IOTHUBTRANSPORT_CONFIG with the information from config parameter and the previous DLIST and shall pass that to the underlying layer _Create function.]*/
    /*Tests_SRS_IOTHUBCLIENT_LL_02_007: [If the underlaying layer _Create function fails them IoTHubClient_LL_Create shall fail and return NULL.]*/
    TEST_FUNCTION(IoTHubClient_LL_Create_fails_when_underlying_transport_fails)
    {
        ///arrange
        CIoTHubClientLLMocks mocks;
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        
        STRICT_EXPECTED_CALL(mocks, DList_InitializeListHead(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, FAKE_IoTHubTransport_Create(IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .SetReturn((TRANSPORT_HANDLE)NULL);

        ///act
        auto result = IoTHubClient_LL_Create(&TEST_CONFIG);

        ///assert
        ASSERT_ARE_EQUAL(void_ptr, NULL, result);
    }

    /*Tests_SRS_IOTHUBCLIENT_LL_02_004: [Otherwise IoTHubClient_LL_Create shall initialize a new DLIST (further called "waitingToSend") containing records with fields of the following types: IOTHUB_MESSAGE_HANDLE, IOTHUB_CLIENT_EVENT_CONFIRMATION_CALLBACK, void*.]*/
    /*Tests_SRS_IOTHUBCLIENT_LL_02_006: [IoTHubClient_LL_Create shall populate a structure of type IOTHUBTRANSPORT_CONFIG with the information from config parameter and the previous DLIST and shall pass that to the underlying layer _Create function.]*/
    /*Tests_SRS_IOTHUBCLIENT_LL_02_008: [Otherwise, IoTHubClient_LL_Create shall succeed and return a non-NULL handle.] */
    TEST_FUNCTION(IoTHubClient_LL_Create_suceeds)
    {
        ///arrange
        CIoTHubClientLLMocks mocks;
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, DList_InitializeListHead(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, FAKE_IoTHubTransport_Create(IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .SetReturn((TRANSPORT_HANDLE)0x42);

        ///act
        auto result = IoTHubClient_LL_Create(&TEST_CONFIG);

        ///assert
        ASSERT_ARE_NOT_EQUAL(void_ptr, NULL, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubClient_LL_Destroy(result);
    }

    /*Tests_SRS_IOTHUBCLIENT_LL_02_009: [IoTHubClient_LL_Destroy shall do nothing if parameter iotHubClientHandle is NULL.] */
    TEST_FUNCTION(IoTHubClient_LL_Destroy_with_NULL_succeeds)
    {
        ///arrange
        CIoTHubClientLLMocks mocks;
        
        ///act
        IoTHubClient_LL_Destroy(NULL);

        ///assert -uMock does it
    }

    /*Tests_SRS_IOTHUBCLIENT_LL_02_010: [IoTHubClient_LL_Destroy it shall call the underlaying layer's _Destroy function and shall free the resources allocated by IoTHubClient (if any).] */
    TEST_FUNCTION(IoTHubClient_LL_Destroys_the_underlying_transport_succeeds)
    {
        ///arrange
        CIoTHubClientLLMocks mocks;
        auto handle = IoTHubClient_LL_Create(&TEST_CONFIG);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, FAKE_IoTHubTransport_Destroy(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, DList_RemoveHeadList(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        ///act
        IoTHubClient_LL_Destroy(handle);

        ///assert -uMock does it
    }

    /*Tests_SRS_IOTHUBCLIENT_LL_02_011: [IoTHubClient_LL_SendEventAsync shall fail and return IOTHUB_CLIENT_INVALID_ARG if parameter iotHubClientHandle or eventMessageHandle is NULL.]*/
    TEST_FUNCTION(IoTHubClient_LL_SendEventAsync_with_NULL_iotHubClientHandle_fails)
    {
        ///arrange
        CIoTHubClientLLMocks mocks;
        auto messageHandle = (IOTHUB_MESSAGE_HANDLE)1;
        
        ///act
        auto result = IoTHubClient_LL_SendEventAsync(NULL, messageHandle, eventConfirmationCallback, (void*)3);

        ///assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_INVALID_ARG, result);
    }

    /*Tests_SRS_IOTHUBCLIENT_LL_02_011: [IoTHubClient_LL_SendEventAsync shall fail and return IOTHUB_CLIENT_INVALID_ARG if parameter iotHubClientHandle or eventMessageHandle is NULL.]*/
    TEST_FUNCTION(IoTHubClient_LL_SendEventAsync_with_NULL_messageHandle_fails)
    {
        ///arrange
        CIoTHubClientLLMocks mocks;
        auto handle = IoTHubClient_LL_Create(&TEST_CONFIG);
        mocks.ResetAllCalls();

        ///act
        auto result = IoTHubClient_LL_SendEventAsync(handle, NULL, eventConfirmationCallback, (void*)3);

        ///assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_INVALID_ARG, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubClient_LL_Destroy(handle);
    }

    /*Tests_SRS_IOTHUBCLIENT_LL_02_012: [IoTHubClient_LL_SendEventAsync shall fail and return IOTHUB_CLIENT_INVALID_ARG if parameter eventConfirmationCallback is NULL and userContextCallback is not NULL.]*/
    TEST_FUNCTION(IoTHubClient_LL_SendEventAsync_with_NULL_eventConfirmationCallback_and_non_NULL_context_fails)
    {
        ///arrange
        CIoTHubClientLLMocks mocks;
        auto handle = IoTHubClient_LL_Create(&TEST_CONFIG);
        auto messageHandle = (IOTHUB_MESSAGE_HANDLE)1;
        mocks.ResetAllCalls();

        ///act
        auto result = IoTHubClient_LL_SendEventAsync(handle, messageHandle, NULL, (void*)3);

        ///assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_INVALID_ARG, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubClient_LL_Destroy(handle);
        
    }

    /*Tests_SRS_IOTHUBCLIENT_LL_02_013: [IoTHubClient_SendEventAsync shall add the DLIST waitingToSend a new record cloning the information from eventMessageHandle, eventConfirmationCallback, userContextCallback.]*/
    /*Tests_SRS_IOTHUBCLIENT_LL_02_015: [Otherwise IoTHubClient_LL_SendEventAsync shall succeed and return IOTHUB_CLIENT_OK.]*/
    TEST_FUNCTION(IoTHubClient_LL_SendEventAsync_succeeds)
    {
        ///arrange
        CIoTHubClientLLMocks mocks;
        auto handle = IoTHubClient_LL_Create(&TEST_CONFIG);
        auto messageHandle = (IOTHUB_MESSAGE_HANDLE)1;
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, DList_InsertTailList(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .IgnoreArgument(2);

        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Clone(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        ///act
        auto result = IoTHubClient_LL_SendEventAsync(handle, messageHandle, eventConfirmationCallback, (void*)1);

        ///assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubClient_LL_Destroy(handle);
    }

    /*Tests_SRS_IOTHUBCLIENT_LL_02_010: [IoTHubClient_LL_Destroy shall call the underlaying layer's _Destroy function and shall free the resources allocated by IoTHubClient (if any).] */
    /*Tests_SRS_IOTHUBCLIENT_LL_02_033: [Otherwise, IoTHubClient_LL_Destroy shall complete all the event message callbacks that are in the waitingToSend list with the result IOTHUB_CLIENT_CONFIRMATION_BECAUSE_DESTROY.] */
    TEST_FUNCTION(IoTHubClient_LL_Destroy_after_sendEvent_succeeds)
    {
        ///arrange
        CIoTHubClientLLMocks mocks;
        auto handle = IoTHubClient_LL_Create(&TEST_CONFIG);
        auto messageHandle = (IOTHUB_MESSAGE_HANDLE)1;
        IoTHubClient_LL_SendEventAsync(handle, messageHandle, eventConfirmationCallback, (void*)1);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, FAKE_IoTHubTransport_Destroy(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*IOTHUBCLIENT*/
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, DList_RemoveHeadList(IGNORED_PTR_ARG)) /*because there is one item in the list*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, eventConfirmationCallback(IOTHUB_CLIENT_CONFIRMATION_BECAUSE_DESTROY, (void*)1));
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Destroy(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*IOTHUBMESSAGE*/
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, DList_RemoveHeadList(IGNORED_PTR_ARG)) /*because this says "no more items in the list*/
            .IgnoreArgument(1);

        ///act
        IoTHubClient_LL_Destroy(handle);

        ///assert -uMock does it

    }

    /*Tests_SRS_IOTHUBCLIENT_LL_02_014: [If cloning and/or adding the information fails for any reason, IoTHubClient_LL_SendEventAsync shall fail and return IOTHUB_CLIENT_ERROR.] */
    TEST_FUNCTION(IoTHubClient_LL_SendEventAsync_fails_when_IoTHubMessage_Clone_fails)
    {
        ///arrange
        CIoTHubClientLLMocks mocks;
        auto handle = IoTHubClient_LL_Create(&TEST_CONFIG);
        auto messageHandle = (IOTHUB_MESSAGE_HANDLE)1;
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*because _Clone fails below*/
            .IgnoreArgument(1);


        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Clone(IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .SetReturn((IOTHUB_MESSAGE_HANDLE)NULL);

        ///act
        auto result = IoTHubClient_LL_SendEventAsync(handle, messageHandle, eventConfirmationCallback, (void*)1);

        ///assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubClient_LL_Destroy(handle);
    }

    /*Tests_SRS_IOTHUBCLIENT_LL_02_014: [If cloning and/or adding the information fails for any reason, IoTHubClient_LL_SendEventAsync shall fail and return IOTHUB_CLIENT_ERROR.] */
    TEST_FUNCTION(IoTHubClient_LL_SendEventAsync_fails_when_malloc_fails)
    {
        ///arrange
        CIoTHubClientLLMocks mocks;
        auto handle = IoTHubClient_LL_Create(&TEST_CONFIG);
        auto messageHandle = (IOTHUB_MESSAGE_HANDLE)1;
        mocks.ResetAllCalls();

        whenShallmalloc_fail = currentmalloc_call+1;
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);

        ///act
        auto result = IoTHubClient_LL_SendEventAsync(handle, messageHandle, eventConfirmationCallback, (void*)1);

        ///assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubClient_LL_Destroy(handle);
    }

    /*Tests_SRS_IOTHUBCLIENT_LL_02_016: [IoTHubClient_LL_SetMessageCallback shall fail and return IOTHUB_CLIENT_INVALID_ARG if parameter iotHubClientHandle is NULL.]*/
    TEST_FUNCTION(IoTHubClient_LL_SetMessageCallback_with_NULL_iotHubClientHandle_fails)
    {
        ///arrange
        CIoTHubClientLLMocks mocks;

        ///act
        auto result = IoTHubClient_LL_SetMessageCallback(NULL, messageCallback, (void*)1);

        ///assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_INVALID_ARG, result);
        mocks.AssertActualAndExpectedCalls();
    }

    /*Tests_SRS_IOTHUBCLIENT_LL_02_019: [If parameter messageCallback is NULL then IoTHubClient_LL_SetMessageCallback shall call the underlying layer's _Unsubscribe function and return IOTHUB_CLIENT_OK.] */
    TEST_FUNCTION(IoTHubClient_LL_SetMessageCallback_with_NULL_calls_Unsubscribe_and_succeeds)
    {
        ///arrange
        CIoTHubClientLLMocks mocks;
        auto handle = IoTHubClient_LL_Create(&TEST_CONFIG);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, FAKE_IoTHubTransport_Unsubscribe(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        ///act
        auto result = IoTHubClient_LL_SetMessageCallback(handle, NULL, (void*)1);

        ///assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubClient_LL_Destroy(handle);
    }

    /*Tests_SRS_IOTHUBCLIENT_LL_02_017: [If parameter messageCallback is non-NULL then IoTHubClient_LL_SetMessageCallback shall call the underlying layer's _Subscribe function.]*/
    TEST_FUNCTION(IoTHubClient_LL_SetMessageCallback_with_non_NULL_succeeds)
    {
        ///arrange
        CIoTHubClientLLMocks mocks;
        auto handle = IoTHubClient_LL_Create(&TEST_CONFIG);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, FAKE_IoTHubTransport_Subscribe(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        ///act
        auto result = IoTHubClient_LL_SetMessageCallback(handle, messageCallback, (void*)1);

        ///assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubClient_LL_Destroy(handle);
    }

    /*Tests_SRS_IOTHUBCLIENT_LL_02_018: [If the underlying layer's _Subscribe function fails, then IoTHubClient_LL_SetMessageCallback shall fail and return IOTHUB_CLIENT_ERROR. Otherwise IoTHubClient_LL_SetMessageCallback shall succeed and return IOTHUB_CLIENT_OK.]*/
    TEST_FUNCTION(IoTHubClient_LL_SetMessageCallback_with_non_fails_when_underlying_transport_fails)
    {
        ///arrange
        CIoTHubClientLLMocks mocks;
        auto handle = IoTHubClient_LL_Create(&TEST_CONFIG);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, FAKE_IoTHubTransport_Subscribe(IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .SetReturn(1);

        ///act
        auto result = IoTHubClient_LL_SetMessageCallback(handle, messageCallback, (void*)1);

        ///assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubClient_LL_Destroy(handle);
    }

    /*Tests_SRS_IOTHUBCLIENT_LL_02_020: [If parameter iotHubClientHandle is NULL then IoTHubClient_LL_DoWork shall not perform any action.] */
    TEST_FUNCTION(IoTHubClient_LL_DoWork_with_NULL_does_nothing)
    {
        ///arrange
        CIoTHubClientLLMocks mocks;

        ///act
        IoTHubClient_LL_DoWork(NULL);

        ///assert
        mocks.AssertActualAndExpectedCalls();
    }

    /*Tests_SRS_IOTHUBCLIENT_LL_02_021: [Otherwise, IoTHubClient_LL_DoWork shall invoke the underlaying layer's _DoWork function.] */
    TEST_FUNCTION(IoTHubClient_LL_DoWork_calls_underlying_succeeds)
    {
        ///arrange
        CIoTHubClientLLMocks mocks;
        auto handle = IoTHubClient_LL_Create(&TEST_CONFIG);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, FAKE_IoTHubTransport_DoWork(IGNORED_PTR_ARG, handle))
            .IgnoreArgument(1);

        ///act
        IoTHubClient_LL_DoWork(handle);

        ///assert
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubClient_LL_Destroy(handle);
    }

    /*Tests_SRS_IOTHUBCLIENT_LL_02_022: [If parameter completed is NULL or parameter handle is NULL then IoTHubClient_LL_SendBatch shall return.]*/
    TEST_FUNCTION(IoTHubClient_LL_SendComplete_with_NULL_handle_shall_return)
    {
        ///arrange
        CIoTHubClientLLMocks mocks;
        DLIST_ENTRY temp;
        DList_InitializeListHead(&temp);
        mocks.ResetAllCalls();

        ///act
        IoTHubClient_LL_SendComplete(NULL, &temp, IOTHUB_BATCHSTATE_SUCCESS);

        ///assert
        mocks.AssertActualAndExpectedCalls();
    }

    /*Tests_SRS_IOTHUBCLIENT_LL_02_022: [If parameter completed is NULL or parameter handle is NULL then IoTHubClient_LL_SendBatch shall return.]*/
    TEST_FUNCTION(IoTHubClient_LL_SendComplete_with_NULL_completed_shall_return)
    {
        ///arrange
        CIoTHubClientLLMocks mocks;
        auto handle = IoTHubClient_LL_Create(&TEST_CONFIG);
        mocks.ResetAllCalls();

        ///act
        IoTHubClient_LL_SendComplete(handle, NULL, IOTHUB_BATCHSTATE_SUCCESS);

        ///assert
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubClient_LL_Destroy(handle);
    }

    /*Tests_SRS_IOTHUBCLIENT_LL_02_022: [If parameter completed is NULL or parameter handle is NULL then IoTHubClient_LL_SendBatch shall return.]*/
    TEST_FUNCTION(IoTHubClient_LL_SendComplete_with_empty_completed_shall_return)
    {
        ///arrange
        CIoTHubClientLLMocks mocks;
        auto handle = IoTHubClient_LL_Create(&TEST_CONFIG);
        DLIST_ENTRY temp;
        DList_InitializeListHead(&temp);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, DList_RemoveHeadList(&temp));

        ///act
        IoTHubClient_LL_SendComplete(handle, &temp, IOTHUB_BATCHSTATE_SUCCESS);

        ///assert
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubClient_LL_Destroy(handle);
    }

    /*Tests_SRS_IOTHUBCLIENT_LL_02_025: [If parameter result is IOTHUB_BATCHSTATE_SUCCESS then IoTHubClient_LL_SendComplete shall call all the non-NULL callbacks with the result parameter set to IOTHUB_CLIENT_CONFIRMATION_OK and the context set to the context passed originally in the SendEventAsync call.]*/
    TEST_FUNCTION(IoTHubClient_LL_SendComplete_with_1_items_with_callback_succeeds)
    {
        ///arrange
        CIoTHubClientLLMocks mocks;
        auto handle = IoTHubClient_LL_Create(&TEST_CONFIG);
        DLIST_ENTRY temp;
        DList_InitializeListHead(&temp);
        IOTHUB_MESSAGE_LIST* one = (IOTHUB_MESSAGE_LIST*)malloc(sizeof(IOTHUB_MESSAGE_LIST)); /*this is SendEvent wannabe*/
        one->messageHandle = (IOTHUB_MESSAGE_HANDLE)1;
        one->callback = eventConfirmationCallback;
        one->context = (void*)1;
        DList_InsertTailList(&temp, &(one->entry));
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, DList_RemoveHeadList(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, eventConfirmationCallback(IOTHUB_CLIENT_CONFIRMATION_OK, (void*)1));
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Destroy((IOTHUB_MESSAGE_HANDLE)1));
        STRICT_EXPECTED_CALL(mocks, gballoc_free(one));

        STRICT_EXPECTED_CALL(mocks, DList_RemoveHeadList(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        ///act
        IoTHubClient_LL_SendComplete(handle, &temp, IOTHUB_BATCHSTATE_SUCCESS);

        ///assert
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubClient_LL_Destroy(handle);
    }

    /*Tests_SRS_IOTHUBCLIENT_LL_02_025: [If parameter result is IOTHUB_BATCHSTATE_SUCCESS then IoTHubClient_LL_SendComplete shall call all the non-NULL callbacks with the result parameter set to IOTHUB_CLIENT_CONFIRMATION_OK and the context set to the context passed originally in the SendEventAsync call.]*/
    TEST_FUNCTION(IoTHubClient_LL_SendComplete_with_3_items_with_callback_succeeds)
    {
        ///arrange
        CIoTHubClientLLMocks mocks;
        auto handle = IoTHubClient_LL_Create(&TEST_CONFIG);
        DLIST_ENTRY temp;
        DList_InitializeListHead(&temp);
        
        IOTHUB_MESSAGE_LIST* one = (IOTHUB_MESSAGE_LIST*)malloc(sizeof(IOTHUB_MESSAGE_LIST)); /*this is SendEvent wannabe*/
        one->messageHandle = (IOTHUB_MESSAGE_HANDLE)1;
        one->callback = eventConfirmationCallback;
        one->context = (void*)1;
        DList_InsertTailList(&temp, &(one->entry));
        
        IOTHUB_MESSAGE_LIST* two = (IOTHUB_MESSAGE_LIST*)malloc(sizeof(IOTHUB_MESSAGE_LIST)); /*this is SendEvent wannabe*/
        two->messageHandle = (IOTHUB_MESSAGE_HANDLE)2;
        two->callback = eventConfirmationCallback;
        two->context = (void*)2;
        DList_InsertTailList(&temp, &(two->entry));
        
        IOTHUB_MESSAGE_LIST* three = (IOTHUB_MESSAGE_LIST*)malloc(sizeof(IOTHUB_MESSAGE_LIST)); /*this is SendEvent wannabe*/
        three->messageHandle = (IOTHUB_MESSAGE_HANDLE)3;
        three->callback = eventConfirmationCallback;
        three->context = (void*)3;
        DList_InsertTailList(&temp, &(three->entry));
        
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, DList_RemoveHeadList(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, eventConfirmationCallback(IOTHUB_CLIENT_CONFIRMATION_OK, (void*)1));
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Destroy((IOTHUB_MESSAGE_HANDLE)1));
        STRICT_EXPECTED_CALL(mocks, gballoc_free(one));

        STRICT_EXPECTED_CALL(mocks, DList_RemoveHeadList(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, eventConfirmationCallback(IOTHUB_CLIENT_CONFIRMATION_OK, (void*)2));
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Destroy((IOTHUB_MESSAGE_HANDLE)2));
        STRICT_EXPECTED_CALL(mocks, gballoc_free(two));

        STRICT_EXPECTED_CALL(mocks, DList_RemoveHeadList(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, eventConfirmationCallback(IOTHUB_CLIENT_CONFIRMATION_OK, (void*)3));
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Destroy((IOTHUB_MESSAGE_HANDLE)3));
        STRICT_EXPECTED_CALL(mocks, gballoc_free(three));

        STRICT_EXPECTED_CALL(mocks, DList_RemoveHeadList(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        ///act
        IoTHubClient_LL_SendComplete(handle, &temp, IOTHUB_BATCHSTATE_SUCCESS);

        ///assert
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubClient_LL_Destroy(handle);
    }

    /*Tests_SRS_IOTHUBCLIENT_LL_02_025: [If parameter result is IOTHUB_BATCHSTATE_SUCCESS then IoTHubClient_LL_SendComplete shall call all the non-NULL callbacks with the result parameter set to IOTHUB_CLIENT_CONFIRMATION_OK and the context set to the context passed originally in the SendEventAsync call.]*/
    /*Tests_SRS_IOTHUBCLIENT_LL_02_026: [If any callback is NULL then there shall not be a callback call.]*/
    TEST_FUNCTION(IoTHubClient_LL_SendComplete_with_3_items_one_without_callback_succeeds) /*for fun*/
    {
        ///arrange
        CIoTHubClientLLMocks mocks;
        auto handle = IoTHubClient_LL_Create(&TEST_CONFIG);
        DLIST_ENTRY temp;
        DList_InitializeListHead(&temp);

        IOTHUB_MESSAGE_LIST* one = (IOTHUB_MESSAGE_LIST*)malloc(sizeof(IOTHUB_MESSAGE_LIST)); /*this is SendEvent wannabe*/
        one->messageHandle = (IOTHUB_MESSAGE_HANDLE)1;
        one->callback = eventConfirmationCallback;
        one->context = (void*)1;
        DList_InsertTailList(&temp, &(one->entry));

        IOTHUB_MESSAGE_LIST* two = (IOTHUB_MESSAGE_LIST*)malloc(sizeof(IOTHUB_MESSAGE_LIST)); /*this is SendEvent wannabe*/
        two->messageHandle = (IOTHUB_MESSAGE_HANDLE)2;
        two->callback = NULL;
        two->context = NULL;
        DList_InsertTailList(&temp, &(two->entry));

        IOTHUB_MESSAGE_LIST* three = (IOTHUB_MESSAGE_LIST*)malloc(sizeof(IOTHUB_MESSAGE_LIST)); /*this is SendEvent wannabe*/
        three->messageHandle = (IOTHUB_MESSAGE_HANDLE)3;
        three->callback = eventConfirmationCallback;
        three->context = (void*)3;
        DList_InsertTailList(&temp, &(three->entry));

        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, DList_RemoveHeadList(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, eventConfirmationCallback(IOTHUB_CLIENT_CONFIRMATION_OK, (void*)1));
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Destroy((IOTHUB_MESSAGE_HANDLE)1));
        STRICT_EXPECTED_CALL(mocks, gballoc_free(one));

        STRICT_EXPECTED_CALL(mocks, DList_RemoveHeadList(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Destroy((IOTHUB_MESSAGE_HANDLE)2));
        STRICT_EXPECTED_CALL(mocks, gballoc_free(two));

        STRICT_EXPECTED_CALL(mocks, DList_RemoveHeadList(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, eventConfirmationCallback(IOTHUB_CLIENT_CONFIRMATION_OK, (void*)3));
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Destroy((IOTHUB_MESSAGE_HANDLE)3));
        STRICT_EXPECTED_CALL(mocks, gballoc_free(three));

        STRICT_EXPECTED_CALL(mocks, DList_RemoveHeadList(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        ///act
        IoTHubClient_LL_SendComplete(handle, &temp, IOTHUB_BATCHSTATE_SUCCESS);

        ///assert
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubClient_LL_Destroy(handle);
    }

    /*Tests_SRS_IOTHUBCLIENT_LL_02_027: [If parameter result is IOTHUB_BACTCHSTATE_FAILED then IoTHubClient_LL_SendComplete shall call all the non-NULL callbacks with the result parameter set to IOTHUB_CLIENT_CONFIRMATION_ERROR and the context set to the context passed originally in the SendEventAsync call.]*/
    TEST_FUNCTION(IoTHubClient_LL_SendComplete_with_3_items_with_callback__but_batch_failed_succeeds)
    {
        ///arrange
        CIoTHubClientLLMocks mocks;
        auto handle = IoTHubClient_LL_Create(&TEST_CONFIG);
        DLIST_ENTRY temp;
        DList_InitializeListHead(&temp);

        IOTHUB_MESSAGE_LIST* one = (IOTHUB_MESSAGE_LIST*)malloc(sizeof(IOTHUB_MESSAGE_LIST)); /*this is SendEvent wannabe*/
        one->messageHandle = (IOTHUB_MESSAGE_HANDLE)1;
        one->callback = eventConfirmationCallback;
        one->context = (void*)1;
        DList_InsertTailList(&temp, &(one->entry));

        IOTHUB_MESSAGE_LIST* two = (IOTHUB_MESSAGE_LIST*)malloc(sizeof(IOTHUB_MESSAGE_LIST)); /*this is SendEvent wannabe*/
        two->messageHandle = (IOTHUB_MESSAGE_HANDLE)2;
        two->callback = eventConfirmationCallback;
        two->context = (void*)2;
        DList_InsertTailList(&temp, &(two->entry));

        IOTHUB_MESSAGE_LIST* three = (IOTHUB_MESSAGE_LIST*)malloc(sizeof(IOTHUB_MESSAGE_LIST)); /*this is SendEvent wannabe*/
        three->messageHandle = (IOTHUB_MESSAGE_HANDLE)3;
        three->callback = eventConfirmationCallback;
        three->context = (void*)3;
        DList_InsertTailList(&temp, &(three->entry));


        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, DList_RemoveHeadList(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, eventConfirmationCallback(IOTHUB_CLIENT_CONFIRMATION_ERROR, (void*)1));
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Destroy((IOTHUB_MESSAGE_HANDLE)1));
        STRICT_EXPECTED_CALL(mocks, gballoc_free(one));

        STRICT_EXPECTED_CALL(mocks, DList_RemoveHeadList(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, eventConfirmationCallback(IOTHUB_CLIENT_CONFIRMATION_ERROR, (void*)2));
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Destroy((IOTHUB_MESSAGE_HANDLE)2));
        STRICT_EXPECTED_CALL(mocks, gballoc_free(two));

        STRICT_EXPECTED_CALL(mocks, DList_RemoveHeadList(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, eventConfirmationCallback(IOTHUB_CLIENT_CONFIRMATION_ERROR, (void*)3));
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Destroy((IOTHUB_MESSAGE_HANDLE)3));
        STRICT_EXPECTED_CALL(mocks, gballoc_free(three));

        STRICT_EXPECTED_CALL(mocks, DList_RemoveHeadList(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        ///act
        IoTHubClient_LL_SendComplete(handle, &temp, IOTHUB_BATCHSTATE_FAILED);

        ///assert
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubClient_LL_Destroy(handle);
    }
    
    /*Tests_SRS_IOTHUBCLIENT_LL_02_028: [If any callback is NULL then there shall not be a callback call.] */
    TEST_FUNCTION(IoTHubClient_LL_SendComplete_with_3_items_one_with_callback_but_batch_failed_succeeds)
    {
        ///arrange
        CIoTHubClientLLMocks mocks;
        auto handle = IoTHubClient_LL_Create(&TEST_CONFIG);
        DLIST_ENTRY temp;
        DList_InitializeListHead(&temp);

        IOTHUB_MESSAGE_LIST* one = (IOTHUB_MESSAGE_LIST*)malloc(sizeof(IOTHUB_MESSAGE_LIST)); /*this is SendEvent wannabe*/
        one->messageHandle = (IOTHUB_MESSAGE_HANDLE)1;
        one->callback = NULL;
        one->context = NULL;
        DList_InsertTailList(&temp, &(one->entry));

        IOTHUB_MESSAGE_LIST* two = (IOTHUB_MESSAGE_LIST*)malloc(sizeof(IOTHUB_MESSAGE_LIST)); /*this is SendEvent wannabe*/
        two->messageHandle = (IOTHUB_MESSAGE_HANDLE)2;
        two->callback = NULL;
        two->context = NULL;
        DList_InsertTailList(&temp, &(two->entry));

        IOTHUB_MESSAGE_LIST* three = (IOTHUB_MESSAGE_LIST*)malloc(sizeof(IOTHUB_MESSAGE_LIST)); /*this is SendEvent wannabe*/
        three->messageHandle = (IOTHUB_MESSAGE_HANDLE)3;
        three->callback = eventConfirmationCallback;
        three->context = (void*)3;
        DList_InsertTailList(&temp, &(three->entry));

        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, DList_RemoveHeadList(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Destroy((IOTHUB_MESSAGE_HANDLE)1));
        STRICT_EXPECTED_CALL(mocks, gballoc_free(one));

        STRICT_EXPECTED_CALL(mocks, DList_RemoveHeadList(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Destroy((IOTHUB_MESSAGE_HANDLE)2));
        STRICT_EXPECTED_CALL(mocks, gballoc_free(two));

        STRICT_EXPECTED_CALL(mocks, DList_RemoveHeadList(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, eventConfirmationCallback(IOTHUB_CLIENT_CONFIRMATION_ERROR, (void*)3));
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Destroy((IOTHUB_MESSAGE_HANDLE)3));
        STRICT_EXPECTED_CALL(mocks, gballoc_free(three));

        STRICT_EXPECTED_CALL(mocks, DList_RemoveHeadList(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        ///act
        IoTHubClient_LL_SendComplete(handle, &temp, IOTHUB_BATCHSTATE_FAILED);

        ///assert
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubClient_LL_Destroy(handle);
    }

    /*Tests_SRS_IOTHUBCLIENT_LL_02_029: [If parameter handle is NULL then IoTHubClient_LL_MessageCallback shall return IOTHUBMESSAGE_ABANDONED.] */
    TEST_FUNCTION(IoTHubClient_LL_MessageCallback_with_NULL_parameter_fails)
    {
        ///arrange
        
        ///act
        auto result = IoTHubClient_LL_MessageCallback(NULL, (IOTHUB_MESSAGE_HANDLE)1);

        ///assert
        ASSERT_ARE_EQUAL(IOTHUBMESSAGE_DISPOSITION_RESULT, IOTHUBMESSAGE_ABANDONED, result);
    }

    /*Tests_SRS_IOTHUBCLIENT_LL_02_030: [IoTHubClient_LL_MessageCallback shall invoke the last callback function (the parameter messageCallback to IoTHubClient_LL_SetMessageCallback) passing the message and the passed userContextCallback.]*/
    TEST_FUNCTION(IoTHubClient_LL_MessageCallback_calls_upper_layer_succeeds)
    {
        ///arrange
        CIoTHubClientLLMocks mocks;
        auto handle = IoTHubClient_LL_Create(&TEST_CONFIG);
        (void)IoTHubClient_LL_SetMessageCallback(handle, messageCallback, (void*)11);
        mocks.ResetAllCalls();
        
        STRICT_EXPECTED_CALL(mocks, messageCallback((IOTHUB_MESSAGE_HANDLE)1, (void*)11));
        STRICT_EXPECTED_CALL(mocks, get_time(NULL));

        ///act
        auto result = IoTHubClient_LL_MessageCallback(handle, (IOTHUB_MESSAGE_HANDLE)1);

        ///assert
        ASSERT_ARE_EQUAL(IOTHUBMESSAGE_DISPOSITION_RESULT, IOTHUBMESSAGE_ACCEPTED, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubClient_LL_Destroy(handle);
    }

    /*Tests_SRS_IOTHUBCLIENT_LL_02_032: [If the last callback function was NULL, then IoTHubClient_LL_MessageCallback  shall return IOTHUBMESSAGE_ABANDONED.] */
    TEST_FUNCTION(IoTHubClient_LL_MessageCallback_without_subscribe_fails)
    {
        ///arrange
        CIoTHubClientLLMocks mocks;
        auto handle = IoTHubClient_LL_Create(&TEST_CONFIG);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, get_time(NULL));

        ///act
        auto result = IoTHubClient_LL_MessageCallback(handle, (IOTHUB_MESSAGE_HANDLE)1);

        ///assert
        ASSERT_ARE_EQUAL(IOTHUBMESSAGE_DISPOSITION_RESULT, IOTHUBMESSAGE_ABANDONED, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubClient_LL_Destroy(handle);
    }


    /*** IoTHubClient_LL_GetLastMessageReceiveTime ***/

    /* Tests_SRS_IOTHUBCLIENT_LL_09_001: [IoTHubClient_LL_GetLastMessageReceiveTime shall return IOTHUB_CLIENT_INVALID_ARG if any of the arguments is NULL] */
    TEST_FUNCTION(IoTHubClient_LL_GetLastMessageReceiveTime_InvalidClientHandleArg_fails)
    {
        // arrange
        time_t lastMessageReceiveTime;
        CNiceCallComparer<CIoTHubClientLLMocks> niceMocks;

        IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle = IoTHubClient_LL_Create(&TEST_CONFIG);

        // act
        IOTHUB_CLIENT_RESULT result = IoTHubClient_LL_GetLastMessageReceiveTime(NULL, &lastMessageReceiveTime);

        // assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_INVALID_ARG, result);

        // Cleanup
        IoTHubClient_LL_Destroy(iotHubClientHandle);
    }

    /* Tests_SRS_IOTHUBCLIENT_LL_09_001: [IoTHubClient_LL_GetLastMessageReceiveTime shall return IOTHUB_CLIENT_INVALID_ARG if any of the arguments is NULL] */
    TEST_FUNCTION(IoTHubClient_LL_GetLastMessageReceiveTime_InvalidTimeRefArg_fails)
    {
        // arrange
        CNiceCallComparer<CIoTHubClientLLMocks> niceMocks;

        IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle = IoTHubClient_LL_Create(&TEST_CONFIG);

        // act
        IOTHUB_CLIENT_RESULT result = IoTHubClient_LL_GetLastMessageReceiveTime(iotHubClientHandle, NULL);

        // assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_INVALID_ARG, result);

        // Cleanup
        IoTHubClient_LL_Destroy(iotHubClientHandle);
    }

    /* Tests_SRS_IOTHUBCLIENT_LL_09_002: [IoTHubClient_LL_GetLastMessageReceiveTime shall return IOTHUB_CLIENT_INDEFINITE_TIME - and not set 'lastMessageReceiveTime' - if it is unable to provide the time for the last commands] */
    TEST_FUNCTION(IoTHubClient_LL_GetLastMessageReceiveTime_NoMessagesReceived_fails)
    {
        // arrange
        CNiceCallComparer<CIoTHubClientLLMocks> niceMocks;
        time_t lastMessageReceiveTime = (time_t)0;;

        IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle = IoTHubClient_LL_Create(&TEST_CONFIG);

        // act
        IOTHUB_CLIENT_RESULT result = IoTHubClient_LL_GetLastMessageReceiveTime(iotHubClientHandle, &lastMessageReceiveTime);

        // assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_INDEFINITE_TIME, result);
        ASSERT_ARE_EQUAL(int, 0, (int)(lastMessageReceiveTime));

        // Cleanup
        IoTHubClient_LL_Destroy(iotHubClientHandle);
    }

    /* Tests_SRS_IOTHUBCLIENT_LL_09_003: [IoTHubClient_LL_GetLastMessageReceiveTime shall return IOTHUB_CLIENT_OK if it wrote in the lastMessageReceiveTime the time when the last command was received] */
    /* Tests_SRS_IOTHUBCLIENT_LL_09_004: [IoTHubClient_LL_GetLastMessageReceiveTime shall return lastMessageReceiveTime in localtime] */
    TEST_FUNCTION(IoTHubClient_LL_GetLastMessageReceiveTime_MessagesReceived_succeeds)
    {
        // arrange
        CIoTHubClientLLMocks mocks;
        auto handle = IoTHubClient_LL_Create(&TEST_CONFIG);
        (void)IoTHubClient_LL_SetMessageCallback(handle, messageCallback, (void*)11);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, messageCallback((IOTHUB_MESSAGE_HANDLE)1, (void*)11));
        STRICT_EXPECTED_CALL(mocks, get_time(NULL));

        time_t timeBeforeCall = time(NULL);

        (void)IoTHubClient_LL_MessageCallback(handle, (IOTHUB_MESSAGE_HANDLE)1);

        // act
        time_t lastMessageReceiveTime = (time_t)0;
        IOTHUB_CLIENT_RESULT result = IoTHubClient_LL_GetLastMessageReceiveTime(handle, &lastMessageReceiveTime);

        // assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK, result);

        time_t timeAfterCall = time(NULL);

        if (difftime(lastMessageReceiveTime, timeBeforeCall) < 0)
        {
            ASSERT_FAIL("Last message time is earlier than expected.");
        }

        if (difftime(lastMessageReceiveTime, timeAfterCall) > 0)
        {
            ASSERT_FAIL("Last message time is later than expected.");
        }
        
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubClient_LL_Destroy(handle);
    }


    /*** IoTHubClient_LL_GetSendStatus ***/

    /* Tests_SRS_IOTHUBCLIENT_09_007: [IoTHubClient_LL_GetSendStatus shall return IOTHUB_CLIENT_INVALID_ARG if called with NULL parameter] */
    TEST_FUNCTION(IoTHubClient_GetSendStatus_BadHandleArgument_fails)
    {
        // arrange
        CIoTHubClientLLMocks mocks;
        mocks.ResetAllCalls();

        IOTHUB_CLIENT_STATUS status;

        // act
        IOTHUB_CLIENT_RESULT result = IoTHubClient_LL_GetSendStatus(NULL, &status);

        // assert
        mocks.AssertActualAndExpectedCalls();
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_INVALID_ARG, result);

        // cleanup
    }

    /* Tests_SRS_IOTHUBCLIENT_09_007: [IoTHubClient_LL_GetSendStatus shall return IOTHUB_CLIENT_INVALID_ARG if called with NULL parameter] */
    TEST_FUNCTION(IoTHubClient_GetSendStatus_BadStatusArgument_fails)
    {
        // arrange
        CIoTHubClientLLMocks mocks;
        IOTHUB_CLIENT_LL_HANDLE handle = IoTHubClient_LL_Create(&TEST_CONFIG);
        mocks.ResetAllCalls();

        // act
        IOTHUB_CLIENT_RESULT result = IoTHubClient_LL_GetSendStatus(handle, NULL);

        // assert
        mocks.AssertActualAndExpectedCalls();
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_INVALID_ARG, result);

        // cleanup
        IoTHubClient_LL_Destroy(handle);
    }

    /* Tests_SRS_IOTHUBCLIENT_09_008: [IoTHubClient_LL_GetSendStatus shall return IOTHUB_CLIENT_OK and status IOTHUB_CLIENT_SEND_STATUS_IDLE if there is currently no items to be sent] */
    TEST_FUNCTION(IoTHubClient_GetSendStatus_NoEventToSend_Succeeds)
    {
        // arrange
        CIoTHubClientLLMocks mocks;
        IOTHUB_CLIENT_LL_HANDLE handle = IoTHubClient_LL_Create(&TEST_CONFIG);
        mocks.ResetAllCalls();

        IOTHUB_CLIENT_STATUS status;

        STRICT_EXPECTED_CALL(mocks, FAKE_IoTHubTransport_GetSendStatus(IGNORED_PTR_ARG, &status))
            .IgnoreAllArguments()
            .SetReturn(IOTHUB_CLIENT_OK);

        currentIotHubClientStatus = IOTHUB_CLIENT_SEND_STATUS_IDLE;

        // act
        IOTHUB_CLIENT_RESULT result = IoTHubClient_LL_GetSendStatus(handle, &status);

        // assert
        mocks.AssertActualAndExpectedCalls();
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK, result);
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_STATUS, IOTHUB_CLIENT_SEND_STATUS_IDLE, status);

        // cleanup
        IoTHubClient_LL_Destroy(handle);
    }

    /* Tests_SRS_IOTHUBCLIENT_09_009: [IoTHubClient_LL_GetSendStatus shall return IOTHUB_CLIENT_OK and status IOTHUB_CLIENT_SEND_STATUS_BUSY if there are currently items to be sent] */
    TEST_FUNCTION(IoTHubClient_GetSendStatus_HasEventToSend_Succeeds)
    {
        // arrange
        CIoTHubClientLLMocks mocks;
        IOTHUB_CLIENT_LL_HANDLE handle = IoTHubClient_LL_Create(&TEST_CONFIG);
        mocks.ResetAllCalls();

        IOTHUB_CLIENT_STATUS status;

        STRICT_EXPECTED_CALL(mocks, FAKE_IoTHubTransport_GetSendStatus(IGNORED_PTR_ARG, &status))
            .IgnoreAllArguments()
            .SetReturn(IOTHUB_CLIENT_OK);

        currentIotHubClientStatus = IOTHUB_CLIENT_SEND_STATUS_BUSY;

        // act
        IOTHUB_CLIENT_RESULT result = IoTHubClient_LL_GetSendStatus(handle, &status);

        // assert
        mocks.AssertActualAndExpectedCalls();
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK, result);
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_STATUS, IOTHUB_CLIENT_SEND_STATUS_BUSY, status);

        // cleanup
        IoTHubClient_LL_Destroy(handle);
        currentIotHubClientStatus = IOTHUB_CLIENT_SEND_STATUS_IDLE;
    }

    /*Tests_SRS_IOTHUBCLIENT_LL_02_034: [If iotHubClientHandle is NULL then IoTHubClient_LL_SetOption shall return IOTHUB_CLIENT_INVALID_ARG.]*/
    TEST_FUNCTION(IoTHubClient_LL_SetOption_with_NULL_handle_fails)
    {
        ///arrange

        ///act
        auto result = IoTHubClient_LL_SetOption(NULL, "a", "b");

        ///assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_INVALID_ARG, result);

        ///cleanup
    }

    /*Tests_SRS_IOTHUBCLIENT_LL_02_035: [If optionName is NULL then IoTHubClient_LL_SetOption shall return IOTHUB_CLIENT_INVALID_ARG.] */
    TEST_FUNCTION(IoTHubClient_LL_SetOption_with_NULL_optionName_fails)
    {
        ///arrange
        CIoTHubClientLLMocks mocks;
        IOTHUB_CLIENT_LL_HANDLE handle = IoTHubClient_LL_Create(&TEST_CONFIG);
        mocks.ResetAllCalls();

        ///act
        auto result = IoTHubClient_LL_SetOption(handle, NULL, "b");

        ///assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_INVALID_ARG, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubClient_LL_Destroy(handle);

    }

    /*Tests_SRS_IOTHUBCLIENT_LL_02_036: [If value is NULL then IoTHubClient_LL_SetOption shall return IOTHUB_CLIENT_INVALID_ARG.] */
    TEST_FUNCTION(IoTHubClient_LL_SetOption_with_NULL_value_fails)
    {
        ///arrange
        CIoTHubClientLLMocks mocks;
        IOTHUB_CLIENT_LL_HANDLE handle = IoTHubClient_LL_Create(&TEST_CONFIG);
        mocks.ResetAllCalls();

        ///act
        auto result = IoTHubClient_LL_SetOption(handle, "a", NULL);

        ///assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_INVALID_ARG, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubClient_LL_Destroy(handle);

    }

    /*Tests_SRS_IOTHUBCLIENT_LL_02_038: [Otherwise, IoTHubClient_LL shall call the function _SetOption of the underlying transport and return what that function is returning.] */
    TEST_FUNCTION(IoTHubClient_LL_SetOption_happy_path)
    {
        ///arrange
        CIoTHubClientLLMocks mocks;
        IOTHUB_CLIENT_LL_HANDLE handle = IoTHubClient_LL_Create(&TEST_CONFIG);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, FAKE_IoTHubTransport_SetOption(IGNORED_PTR_ARG, "a", "b"))
            .IgnoreArgument(1);

        ///act
        auto result = IoTHubClient_LL_SetOption(handle, "a", "b");

        ///assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubClient_LL_Destroy(handle);

    }

    /*Tests_SRS_IOTHUBCLIENT_LL_02_038: [Otherwise, IoTHubClient_LL shall call the function _SetOption of the underlying transport and return what that function is returning.] */
    TEST_FUNCTION(IoTHubClient_LL_SetOption_fails_when_underlying_transport_fails)
    {
        ///arrange
        CIoTHubClientLLMocks mocks;
        IOTHUB_CLIENT_LL_HANDLE handle = IoTHubClient_LL_Create(&TEST_CONFIG);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, FAKE_IoTHubTransport_SetOption(IGNORED_PTR_ARG, "a", "b"))
            .IgnoreArgument(1)
            .SetReturn(IOTHUB_CLIENT_INDEFINITE_TIME); 

        ///act
        auto result = IoTHubClient_LL_SetOption(handle, "a", "b");

        ///assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_INDEFINITE_TIME, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        IoTHubClient_LL_Destroy(handle);

    }
END_TEST_SUITE(iothubclient_ll_unittests)

