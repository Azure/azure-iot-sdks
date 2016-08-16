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
#include "iothub_client_version.h"

#include "azure_c_shared_utility/lock.h"
#include "azure_c_shared_utility/doublylinkedlist.h"
#include "iothub_client_private.h"
#include "iothub_transport_ll.h"

#ifndef DONT_USE_UPLOADTOBLOB
#include "iothub_client_ll_uploadtoblob.h"
#endif

#include "azure_c_shared_utility/string_tokenizer.h"
#include "azure_c_shared_utility/strings.h"

#include "azure_c_shared_utility/tickcounter.h"

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
DEFINE_MICROMOCK_ENUM_TO_STRING(IOTHUB_CLIENT_CONFIRMATION_RESULT, IOTHUB_CLIENT_CONFIRMATION_RESULT_VALUES);
DEFINE_MICROMOCK_ENUM_TO_STRING(IOTHUBMESSAGE_DISPOSITION_RESULT, IOTHUBMESSAGE_DISPOSITION_RESULT_VALUES);


static MICROMOCK_MUTEX_HANDLE g_testByTest;
static MICROMOCK_GLOBAL_SEMAPHORE_HANDLE g_dllByDll;

//Check ProtocolGateway Configuration.
static bool checkProtocolGatewayHostName;
static bool checkProtocolGatewayIsNull;

#define TEST_DEVICE_ID "theidofTheDevice"
#define TEST_DEVICE_KEY "theKeyoftheDevice"
#define TEST_DEVICE_SAS "theSasOfTheDevice"
#define TEST_IOTHUBNAME "theNameoftheIotHub"
#define TEST_IOTHUBSUFFIX "theSuffixoftheIotHubHostname"
#define TEST_AUTHORIZATIONKEY "theAuthorizationKey"

#define TEST_HOSTNAME_TOKEN "HostName"
#define TEST_HOSTNAME_VALUE "theNameoftheIotHub.theSuffixoftheIotHubHostname"

#define TEST_DEVICEID_TOKEN "DeviceId"
#define TEST_DEVICEKEY_TOKEN "SharedAccessKey"
#define TEST_DEVICESAS_TOKEN "SharedAccessSignature"
#define TEST_PROTOCOL_GATEWAY_HOST_NAME_TOKEN "GatewayHostName"
#define TEST_X509 "x509"

#define TEST_DEVICEMESSAGE_HANDLE (IOTHUB_MESSAGE_HANDLE)0x52
#define TEST_DEVICEMESSAGE_HANDLE_2 (IOTHUB_MESSAGE_HANDLE)0x53
#define TEST_IOTHUB_CLIENT_LL_HANDLE    (IOTHUB_CLIENT_LL_HANDLE)0x4242

#define TEST_STRING_HANDLE (STRING_HANDLE)0x46
#define TEST_STRING_TOKENIZER_HANDLE (STRING_TOKENIZER_HANDLE)0x48
static const char* TEST_CHAR = "TestChar";

static const TRANSPORT_PROVIDER* provideFAKE(void);

static const IOTHUB_CLIENT_CONFIG TEST_CONFIG =
{
    provideFAKE,            /* IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol;   */
    TEST_DEVICE_ID,         /* const char* deviceId;                        */
    TEST_DEVICE_KEY,        /* const char* deviceKey;                       */
    TEST_DEVICE_SAS,   /* const char* deviceSasToken;                  */
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

#define FAKE_TRANSPORT_HANDLE (TRANSPORT_LL_HANDLE)0xDEAD

static const IOTHUB_CLIENT_DEVICE_CONFIG TEST_DEVICE_CONFIG =
{
    provideFAKE,
    FAKE_TRANSPORT_HANDLE,
    TEST_DEVICE_ID,
    TEST_DEVICE_KEY
};

static const IOTHUB_CLIENT_DEVICE_CONFIG TEST_DEVICE_CONFIG_null_protocol =
{
    NULL,
    FAKE_TRANSPORT_HANDLE,
    TEST_DEVICE_ID,
    TEST_DEVICE_KEY
};

static const IOTHUB_CLIENT_DEVICE_CONFIG TEST_DEVICE_CONFIG_null_handle =
{
    provideFAKE,
    NULL,
    TEST_DEVICE_ID,
    TEST_DEVICE_KEY
};

static const IOTHUB_CLIENT_DEVICE_CONFIG TEST_DEVICE_CONFIG_NULL_device_key_NULL_sas_token =
{
    provideFAKE,
    FAKE_TRANSPORT_HANDLE,
    NULL,
    NULL
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

        MOCK_STATIC_METHOD_1(, TRANSPORT_LL_HANDLE, FAKE_IoTHubTransport_Create, const IOTHUBTRANSPORT_CONFIG*, config)
        TRANSPORT_LL_HANDLE result2;
    if (checkProtocolGatewayHostName)
    {
        if (config != NULL && config->upperConfig != NULL && config->upperConfig->protocolGatewayHostName != NULL && strcmp(config->upperConfig->protocolGatewayHostName, TEST_CHAR) == 0)
        {
            result2 = (TRANSPORT_LL_HANDLE)(0x42);
        }
        else
        {
            result2 = NULL;
        }
    }
    else if (checkProtocolGatewayIsNull)
    {
        if (config != NULL && config->upperConfig != NULL && config->upperConfig->protocolGatewayHostName == NULL)
        {
            result2 = (TRANSPORT_LL_HANDLE)(0x42);
        }
        else
        {
            result2 = NULL;
        }
    }
    else
    {
        result2 = (TRANSPORT_LL_HANDLE)(0x42);
    }
    MOCK_METHOD_END(TRANSPORT_LL_HANDLE, result2)

        MOCK_STATIC_METHOD_1(, STRING_HANDLE, FAKE_IoTHubTransport_GetHostname, TRANSPORT_LL_HANDLE, handle)
        MOCK_METHOD_END(STRING_HANDLE, (STRING_HANDLE)0x42)

        MOCK_STATIC_METHOD_3(, IOTHUB_CLIENT_RESULT, FAKE_IoTHubTransport_SetOption, TRANSPORT_LL_HANDLE, handle, const char*, optionName, const void*, value)
        MOCK_METHOD_END(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK)

        MOCK_STATIC_METHOD_1(, void, FAKE_IoTHubTransport_Destroy, TRANSPORT_LL_HANDLE, handle)
        MOCK_VOID_METHOD_END()

        MOCK_STATIC_METHOD_4(, IOTHUB_DEVICE_HANDLE, FAKE_IoTHubTransport_Register, TRANSPORT_LL_HANDLE, handle, const IOTHUB_DEVICE_CONFIG*, device, IOTHUB_CLIENT_LL_HANDLE, iotHubClientHandle, PDLIST_ENTRY, waitingToSend)
        MOCK_METHOD_END(IOTHUB_DEVICE_HANDLE, (IOTHUB_DEVICE_HANDLE)handle)

        MOCK_STATIC_METHOD_1(, void, FAKE_IoTHubTransport_Unregister, IOTHUB_DEVICE_HANDLE, handle)
        MOCK_VOID_METHOD_END()

        MOCK_STATIC_METHOD_1(, int, FAKE_IoTHubTransport_Subscribe, TRANSPORT_LL_HANDLE, handle)
        MOCK_METHOD_END(int, 0)

        MOCK_STATIC_METHOD_1(, void, FAKE_IoTHubTransport_Unsubscribe, TRANSPORT_LL_HANDLE, handle)
        MOCK_VOID_METHOD_END()

        MOCK_STATIC_METHOD_2(, void, FAKE_IoTHubTransport_DoWork, TRANSPORT_LL_HANDLE, handle, IOTHUB_CLIENT_LL_HANDLE, iotHubClientHandle)
        MOCK_VOID_METHOD_END()

        MOCK_STATIC_METHOD_2(, IOTHUB_CLIENT_RESULT, FAKE_IoTHubTransport_GetSendStatus, TRANSPORT_LL_HANDLE, handle, IOTHUB_CLIENT_STATUS*, iotHubClientStatus)
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
        MOCK_METHOD_END(const char*, (const char*)NULL)

        MOCK_STATIC_METHOD_0(, TICK_COUNTER_HANDLE, tickcounter_create);
    TICK_COUNTER_HANDLE result2 = (TICK_COUNTER_HANDLE)BASEIMPLEMENTATION::gballoc_malloc(1);
    MOCK_METHOD_END(TICK_COUNTER_HANDLE, result2)

        MOCK_STATIC_METHOD_1(, void, tickcounter_destroy, TICK_COUNTER_HANDLE, tick_counter);
    BASEIMPLEMENTATION::gballoc_free(tick_counter);
    MOCK_VOID_METHOD_END()

        MOCK_STATIC_METHOD_2(, int, tickcounter_get_current_ms, TICK_COUNTER_HANDLE, tick_counter, uint64_t*, current_ms);
    MOCK_METHOD_END(int, 0)

#ifndef DONT_USE_UPLOADTOBLOB
    MOCK_STATIC_METHOD_1(, IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE, IoTHubClient_LL_UploadToBlob_Create, const IOTHUB_CLIENT_CONFIG*, config)
        IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE result2 = (IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE)BASEIMPLEMENTATION::gballoc_malloc(1);
    MOCK_METHOD_END(IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE, result2)

    MOCK_STATIC_METHOD_4(, IOTHUB_CLIENT_RESULT, IoTHubClient_LL_UploadToBlob_Impl, IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE, handle, const char*, destinationFileName, const unsigned char*, source, size_t, size)
    MOCK_METHOD_END(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK)

    MOCK_STATIC_METHOD_1(, void, IoTHubClient_LL_UploadToBlob_Destroy, IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE, handle)
        BASEIMPLEMENTATION::gballoc_free(handle);
    MOCK_VOID_METHOD_END()

    MOCK_STATIC_METHOD_3(, IOTHUB_CLIENT_RESULT, IoTHubClient_LL_UploadToBlob_SetOption, IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE, handle, const char*, option, const void*, value)
    MOCK_METHOD_END(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK)
#endif

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

DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubClientLLMocks, , STRING_HANDLE, FAKE_IoTHubTransport_GetHostname, TRANSPORT_LL_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubClientLLMocks, , IOTHUB_CLIENT_RESULT, FAKE_IoTHubTransport_SetOption, TRANSPORT_LL_HANDLE, handle, const char*, optionName, const void*, value);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubClientLLMocks, , TRANSPORT_LL_HANDLE, FAKE_IoTHubTransport_Create, const IOTHUBTRANSPORT_CONFIG*, config);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubClientLLMocks, , void, FAKE_IoTHubTransport_Destroy, TRANSPORT_LL_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_4(CIoTHubClientLLMocks, , IOTHUB_DEVICE_HANDLE, FAKE_IoTHubTransport_Register, TRANSPORT_LL_HANDLE, handle, const IOTHUB_DEVICE_CONFIG*, device, IOTHUB_CLIENT_LL_HANDLE, iotHubClientHandle, PDLIST_ENTRY, waitingToSend);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubClientLLMocks, , void, FAKE_IoTHubTransport_Unregister, IOTHUB_DEVICE_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubClientLLMocks, , int, FAKE_IoTHubTransport_Subscribe, TRANSPORT_LL_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubClientLLMocks, , void, FAKE_IoTHubTransport_Unsubscribe, TRANSPORT_LL_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubClientLLMocks, , void, FAKE_IoTHubTransport_DoWork, TRANSPORT_LL_HANDLE, handle, IOTHUB_CLIENT_LL_HANDLE, iotHubClientHandle);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubClientLLMocks, , IOTHUB_CLIENT_RESULT, FAKE_IoTHubTransport_GetSendStatus, TRANSPORT_LL_HANDLE, handle, IOTHUB_CLIENT_STATUS*, iotHubClientStatus);

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

DECLARE_GLOBAL_MOCK_METHOD_0(CIoTHubClientLLMocks, , TICK_COUNTER_HANDLE, tickcounter_create);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubClientLLMocks, , void, tickcounter_destroy, TICK_COUNTER_HANDLE, tick_counter);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubClientLLMocks, , int, tickcounter_get_current_ms, TICK_COUNTER_HANDLE, tick_counter, uint64_t*, current_ms);

#ifndef DONT_USE_UPLOADTOBLOB
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubClientLLMocks, , IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE, IoTHubClient_LL_UploadToBlob_Create, const IOTHUB_CLIENT_CONFIG*, config);
DECLARE_GLOBAL_MOCK_METHOD_4(CIoTHubClientLLMocks, , IOTHUB_CLIENT_RESULT, IoTHubClient_LL_UploadToBlob_Impl, IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE, handle, const char*, destinationFileName, const unsigned char*, source, size_t, size);
DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubClientLLMocks, , IOTHUB_CLIENT_RESULT, IoTHubClient_LL_UploadToBlob_SetOption, IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE, handle, const char*, option, const void*, value)
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubClientLLMocks, , void, IoTHubClient_LL_UploadToBlob_Destroy, IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE, handle);
#endif

static TRANSPORT_PROVIDER FAKE_transport_provider =
{
    FAKE_IoTHubTransport_GetHostname,   /*pfIoTHubTransport_GetHostname IoTHubTransport_GetHostname     */
    FAKE_IoTHubTransport_SetOption,     /*pfIoTHubTransport_SetOption IoTHubTransport_SetOption;        */
    FAKE_IoTHubTransport_Create,        /*pfIoTHubTransport_Create IoTHubTransport_Create;              */
    FAKE_IoTHubTransport_Destroy,       /*pfIoTHubTransport_Destroy IoTHubTransport_Destroy;            */
    FAKE_IoTHubTransport_Register,		/*pfIotHubTransport_Register IoTHubTransport_Register;          */
    FAKE_IoTHubTransport_Unregister,    /*pfIotHubTransport_Unregister IoTHubTransport_Unegister;       */
    FAKE_IoTHubTransport_Subscribe,     /*pfIoTHubTransport_Subscribe IoTHubTransport_Subscribe;        */
    FAKE_IoTHubTransport_Unsubscribe,   /*pfIoTHubTransport_Unsubscribe IoTHubTransport_Unsubscribe;    */
    FAKE_IoTHubTransport_DoWork,        /*pfIoTHubTransport_DoWork IoTHubTransport_DoWork;              */
    FAKE_IoTHubTransport_GetSendStatus  /*pfIoTHubTransport_GetSendStatus IoTHubTransport_GetSendStatus;*/
};

static const TRANSPORT_PROVIDER* provideFAKE(void)
{
    return &FAKE_transport_provider;
}

BEGIN_TEST_SUITE(iothubclient_ll_ut)

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
    checkProtocolGatewayHostName = false;
    checkProtocolGatewayIsNull = false;
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

#ifndef NO_LOGGING
    EXPECTED_CALL(mocks, IoTHubClient_GetVersionString());
#endif

    // act
    (void)IoTHubClient_LL_CreateFromConnectionString(NULL, NULL);

    // assert
    // Implicit
}

/* Tests_SRS_IOTHUBCLIENT_LL_02_092: [ IoTHubClient_LL_CreateFomConnectionString shall create a IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE from IOTHUB_CLIENT_CONFIG. ]*/
/* Tests_SRS_IOTHUBCLIENT_LL_12_011: [IoTHubClient_LL_CreateFromConnectionString shall call into the IoTHubClient_LL_Create API with the current structure and returns with the return value of it] */
/* Tests_SRS_IOTHUBCLIENT_LL_12_010: [IoTHubClient_LL_CreateFromConnectionString shall fill up the IOTHUB_CLIENT_CONFIG structure using the following mapping: iotHubName = Name, iotHubSuffix = Suffix, deviceId = DeviceId, deviceKey = SharedAccessKey or deviceSasToken = SharedAccessSignature] */
TEST_FUNCTION(IoTHubClient_LL_CreateFromConnectionString_with_DeviceKey_succeeds)
{
    ///arrange
    CIoTHubClientLLMocks mocks;

#ifndef NO_LOGGING
    STRICT_EXPECTED_CALL(mocks, IoTHubClient_GetVersionString());
#endif

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

#ifndef DONT_USE_UPLOADTOBLOB
	/* underlying IoTHubClient_LL_Create call */
    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_UploadToBlob_Create(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
#endif 

    STRICT_EXPECTED_CALL(mocks, tickcounter_create());

    STRICT_EXPECTED_CALL(mocks, DList_InitializeListHead(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, FAKE_IoTHubTransport_Create(IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .SetReturn((TRANSPORT_LL_HANDLE)0x42);

    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(sizeof(IOTHUB_DEVICE_CONFIG)))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, FAKE_IoTHubTransport_Register(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreAllArguments();

    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    
    ///act
    auto result = IoTHubClient_LL_CreateFromConnectionString(TEST_CHAR, provideFAKE);

    ///assert
    ASSERT_ARE_NOT_EQUAL(void_ptr, NULL, result);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubClient_LL_Destroy(result);
}

#ifndef DONT_USE_UPLOADTOBLOB
/*Tests_SRS_IOTHUBCLIENT_LL_02_093: [ If creating the IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE fails then IoTHubClient_LL_CreateFromConnectionString shall fail and return NULL. ]*/
TEST_FUNCTION(IoTHubClient_LL_CreateFromConnectionString_fails_when_uploadToBlob_Handle_creation_fails)
{
    ///arrange
    CIoTHubClientLLMocks mocks;

#ifndef NO_LOGGING
    STRICT_EXPECTED_CALL(mocks, IoTHubClient_GetVersionString());
#endif

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
    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_UploadToBlob_Create(IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .SetFailReturn((IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE)NULL);

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
#endif /*DONT_USE_UPLOADTOBLOB*/

/* Tests_SRS_IOTHUBCLIENT_LL_12_011: [IoTHubClient_LL_CreateFromConnectionString shall call into the IoTHubClient_LL_Create API with the current structure and returns with the return value of it] */
/* Tests_SRS_IOTHUBCLIENT_LL_12_010: [IoTHubClient_LL_CreateFromConnectionString shall fill up the IOTHUB_CLIENT_CONFIG structure using the following mapping: iotHubName = Name, iotHubSuffix = Suffix, deviceId = DeviceId, deviceKey = SharedAccessKey or deviceSasToken = SharedAccessSignature] */
/* Tests_SRS_IOTHUBCLIENT_LL_02_092: [ IoTHubClient_LL_CreateFomConnectionString shall create a IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE from IOTHUB_CLIENT_CONFIG. ]*/
TEST_FUNCTION(IoTHubClient_LL_CreateFromConnectionString_with_DeviceSasToken_succeeds)
{
    ///arrange
    CIoTHubClientLLMocks mocks;

#ifndef NO_LOGGING
    STRICT_EXPECTED_CALL(mocks, IoTHubClient_GetVersionString());
#endif

    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
        .IgnoreArgument(1);

#ifndef DONT_USE_UPLOADTOBLOB
    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_UploadToBlob_Create(IGNORED_PTR_ARG))
        .IgnoreArgument(1);;
#endif

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
    EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .SetReturn(TEST_DEVICESAS_TOKEN);
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
    STRICT_EXPECTED_CALL(mocks, tickcounter_create());

    STRICT_EXPECTED_CALL(mocks, DList_InitializeListHead(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, FAKE_IoTHubTransport_Create(IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .SetReturn((TRANSPORT_LL_HANDLE)0x42);

    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, FAKE_IoTHubTransport_Register(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreAllArguments();

    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    
    ///act
    auto result = IoTHubClient_LL_CreateFromConnectionString(TEST_CHAR, provideFAKE);

    ///assert
    ASSERT_ARE_NOT_EQUAL(void_ptr, NULL, result);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubClient_LL_Destroy(result);
}

/*Tests_SRS_IOTHUBCLIENT_LL_03_010: [IoTHubClient_LL_CreateFromConnectionString shall return NULL if both a deviceKey & a deviceSasToken are specified.] */
TEST_FUNCTION(IoTHubClient_LL_CreateFromConnectionString_with_DeviceKey_and_DeviceSasToken_fails)
{
    ///arrange
    CIoTHubClientLLMocks mocks;

#ifndef NO_LOGGING
    STRICT_EXPECTED_CALL(mocks, IoTHubClient_GetVersionString());
#endif

    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_CHAR));
    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_create(TEST_STRING_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_new());
    STRICT_EXPECTED_CALL(mocks, STRING_new());
    STRICT_EXPECTED_CALL(mocks, STRING_new());
    STRICT_EXPECTED_CALL(mocks, STRING_new());

    /* loop 1 - HostName*/
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

    /* loop 2 - DeviceId */
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

    /* loop 3 - DeviceSasToken */
    EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, IGNORED_PTR_ARG))
        .SetReturn(0);
    EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, IGNORED_PTR_ARG))
        .SetReturn(0);
    EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .SetReturn(TEST_DEVICESAS_TOKEN);
    STRICT_EXPECTED_CALL(mocks, STRING_clone(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .SetReturn(TEST_CHAR);

    /* loop 4 - DeviceKey */
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
    EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));

    EXPECTED_CALL(mocks, STRING_TOKENIZER_destroy(IGNORED_PTR_ARG));

    EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG));

    ///act
    auto result = IoTHubClient_LL_CreateFromConnectionString(TEST_CHAR, provideFAKE);

    ///assert
    ASSERT_ARE_EQUAL(void_ptr, NULL, result);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubClient_LL_Destroy(result);
}

/* Tests_SRS_IOTHUBCLIENT_LL_04_001: [IoTHubClient_LL_CreateFromConnectionString shall verify the existence of key/value pair GatewayHostName. If it does exist it shall pass the value to IoTHubClient_LL_Create API.] */
TEST_FUNCTION(IoTHubClient_LL_CreateFromConnectionString_withGatewayHostName_succeeds)
{
    ///arrange
    CIoTHubClientLLMocks mocks;
    checkProtocolGatewayHostName = true;

#ifndef NO_LOGGING
    STRICT_EXPECTED_CALL(mocks, IoTHubClient_GetVersionString());
#endif

    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
        .IgnoreArgument(1);

#ifndef DONT_USE_UPLOADTOBLOB
    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_UploadToBlob_Create(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
#endif 

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

    /* loop 4*/
    EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, IGNORED_PTR_ARG))
        .SetReturn(0);
    EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, IGNORED_PTR_ARG))
        .SetReturn(0);
    STRICT_EXPECTED_CALL(mocks, STRING_clone(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .SetReturn(TEST_PROTOCOL_GATEWAY_HOST_NAME_TOKEN);
    EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .SetReturn(TEST_CHAR);

    /* loop exit */
    EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, IGNORED_PTR_ARG))
        .SetReturn(1);
    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG));

    EXPECTED_CALL(mocks, STRING_TOKENIZER_destroy(IGNORED_PTR_ARG));

    /* underlying IoTHubClient_LL_Create call */
    STRICT_EXPECTED_CALL(mocks, tickcounter_create());

    STRICT_EXPECTED_CALL(mocks, DList_InitializeListHead(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, FAKE_IoTHubTransport_Create(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, FAKE_IoTHubTransport_Register(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreAllArguments();

    ///act
    auto result = IoTHubClient_LL_CreateFromConnectionString(TEST_CHAR, provideFAKE);

    ///assert
    ASSERT_ARE_NOT_EQUAL(void_ptr, NULL, result);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubClient_LL_Destroy(result);
}

/* Tests_SRS_IOTHUBCLIENT_LL_04_002: [If it does not, it shall pass the protocolGatewayHostName NULL.] */
TEST_FUNCTION(IoTHubClient_LL_CreateFromConnectionString_withoutGatewayHostName_sets_the_gatewayhostname_to_NULL)
{
    ///arrange
    CIoTHubClientLLMocks mocks;
    checkProtocolGatewayIsNull = true;

#ifndef NO_LOGGING
    STRICT_EXPECTED_CALL(mocks, IoTHubClient_GetVersionString());
#endif

    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
        .IgnoreArgument(1);

#ifndef DONT_USE_UPLOADTOBLOB
    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_UploadToBlob_Create(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
#endif

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
    STRICT_EXPECTED_CALL(mocks, tickcounter_create());

    STRICT_EXPECTED_CALL(mocks, DList_InitializeListHead(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, FAKE_IoTHubTransport_Create(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, FAKE_IoTHubTransport_Register(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreAllArguments();

    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    
    ///act
    auto result = IoTHubClient_LL_CreateFromConnectionString(TEST_CHAR, provideFAKE);

    ///assert
    ASSERT_ARE_NOT_EQUAL(void_ptr, NULL, result);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubClient_LL_Destroy(result);
}

/*Tests_SRS_IOTHUBCLIENT_LL_12_003: [IoTHubClient_LL_CreateFromConnectionString shall verify the input parameters and if any of them NULL then return NULL] */
TEST_FUNCTION(IoTHubClient_LL_CreateFromConnectionString_if_input_parameter_connectionString_is_NULL_then_return_NULL)
{
    ///arrange
    CIoTHubClientLLMocks mocks;

#ifndef NO_LOGGING
    STRICT_EXPECTED_CALL(mocks, IoTHubClient_GetVersionString());
#endif

    ///act
    auto result = IoTHubClient_LL_CreateFromConnectionString(NULL, provideFAKE);

    ///assert
    ASSERT_ARE_EQUAL(void_ptr, NULL, result);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubClient_LL_Destroy(result);
}

/*Tests_SRS_IOTHUBCLIENT_LL_12_003: [IoTHubClient_LL_CreateFromConnectionString shall return NULL if any of the input parameter is NULL] */
TEST_FUNCTION(IoTHubClient_LL_CreateFromConnectionString_if_input_parameter_protocol_is_NULL_then_return_NULL)
{
    ///arrange
    CIoTHubClientLLMocks mocks;

#ifndef NO_LOGGING
    STRICT_EXPECTED_CALL(mocks, IoTHubClient_GetVersionString());
#endif

    ///act
    auto result = IoTHubClient_LL_CreateFromConnectionString(TEST_CHAR, NULL);

    ///assert
    ASSERT_ARE_EQUAL(void_ptr, NULL, result);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubClient_LL_Destroy(result);
}

/*Tests_SRS_IOTHUBCLIENT_LL_12_004: [IoTHubClient_LL_CreateFromConnectionString shall allocate IOTHUB_CLIENT_CONFIG structure] */
TEST_FUNCTION(IoTHubClient_LL_CreateFromConnectionString_config_structure_allocation_fails)
{
    ///arrange
    CIoTHubClientLLMocks mocks;

#ifndef NO_LOGGING
    STRICT_EXPECTED_CALL(mocks, IoTHubClient_GetVersionString());
#endif

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

/*Tests_SRS_IOTHUBCLIENT_LL_12_016: [IoTHubClient_LL_CreateFromConnectionString shall return NULL if IoTHubClient_LL_Create call fails] */
TEST_FUNCTION(IoTHubClient_LL_CreateFromConnectionString_if_IoTHubClient_LL_Create_fails_then_IoTHubClient_LL_CreateFromConnectionString_return_NULL)
{
    ///arrange
    CIoTHubClientLLMocks mocks;

#ifndef NO_LOGGING
    STRICT_EXPECTED_CALL(mocks, IoTHubClient_GetVersionString());
#endif

    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

#ifndef DONT_USE_UPLOADTOBLOB
    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_UploadToBlob_Create(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_UploadToBlob_Destroy(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
#endif

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
    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, tickcounter_create());
    STRICT_EXPECTED_CALL(mocks, tickcounter_destroy(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, DList_InitializeListHead(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, FAKE_IoTHubTransport_Create(IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .SetReturn((TRANSPORT_LL_HANDLE)NULL);

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

/* Tests_IoTHubClient_LL_CreateFromConnectionString shall verify the existence of the following Key/Value pairs in the connection string: HostName, DeviceId, SharedAccessKey, SharedAccessSignature or x509.]  */
/*Tests_SRS_IOTHUBCLIENT_LL_12_014: [If either of key is missing or x509 is not set to "true" then IoTHubClient_LL_CreateFromConnectionString returns NULL ]*/
TEST_FUNCTION(IoTHubClient_LL_CreateFromConnectionString_if_hostName_missing_then_return_NULL)
{
    ///arrange
    CIoTHubClientLLMocks mocks;

#ifndef NO_LOGGING
    STRICT_EXPECTED_CALL(mocks, IoTHubClient_GetVersionString());
#endif

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

/* Tests_SRS_IOTHUBCLIENT_LL_12_006: [IoTHubClient_LL_CreateFromConnectionString shall verify the existence of the following Key/Value pairs in the connection string: HostName, DeviceId, SharedAccessKey, SharedAccessSignature or x509.]  */
/*Tests_SRS_IOTHUBCLIENT_LL_12_014: [If either of key is missing or x509 is not set to "true" then IoTHubClient_LL_CreateFromConnectionString returns NULL ]*/
TEST_FUNCTION(IoTHubClient_LL_CreateFromConnectionString_if_deviceId_missing_then_return_NULL)
{
    ///arrange
    CIoTHubClientLLMocks mocks;

#ifndef NO_LOGGING
    STRICT_EXPECTED_CALL(mocks, IoTHubClient_GetVersionString());
#endif

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

/* Tests_SRS_IOTHUBCLIENT_LL_12_006: [IoTHubClient_LL_CreateFromConnectionString shall verify the existence of the following Key/Value pairs in the connection string: HostName, DeviceId, SharedAccessKey, SharedAccessSignature or x509.]  */
/*Tests_SRS_IOTHUBCLIENT_LL_12_014: [If either of key is missing or x509 is not set to "true" then IoTHubClient_LL_CreateFromConnectionString returns NULL ]*/
TEST_FUNCTION(IoTHubClient_LL_CreateFromConnectionString_if_deviceKey_and_deviceSasToken_missing_then_return_NULL)
{
    ///arrange
    CIoTHubClientLLMocks mocks;

#ifndef NO_LOGGING
    STRICT_EXPECTED_CALL(mocks, IoTHubClient_GetVersionString());
#endif

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

    /* loop 1 -- HostName*/
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

    /* loop 2 -- DeviceId*/
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

    /* loop exit -- DeviceKey or DeviceSasToken */
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

/*Tests_SRS_IOTHUBCLIENT_LL_12_014: [If either of key is missing or x509 is not set to "true" then IoTHubClient_LL_CreateFromConnectionString returns NULL ]*/
TEST_FUNCTION(IoTHubClient_LL_CreateFromConnectionString_with_x509_true_succeeds)
{
    ///arrange
    CIoTHubClientLLMocks mocks;

#ifndef NO_LOGGING
    STRICT_EXPECTED_CALL(mocks, IoTHubClient_GetVersionString());
#endif

    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
        .IgnoreArgument(1);

#ifndef DONT_USE_UPLOADTOBLOB
    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_UploadToBlob_Create(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
#endif 

    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_CHAR));
    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_create(TEST_STRING_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_new());
    STRICT_EXPECTED_CALL(mocks, STRING_new());
    STRICT_EXPECTED_CALL(mocks, STRING_new());
    STRICT_EXPECTED_CALL(mocks, STRING_new());

    /* loop 1 -- HostName*/
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

    /* loop 2 -- DeviceId*/
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

    /* loop 3 -- x509*/
    EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, IGNORED_PTR_ARG))
        .SetReturn(0);
    EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, IGNORED_PTR_ARG))
        .SetReturn(0);
    EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .SetReturn(TEST_X509);
    EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .SetReturn("true");

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

    /* underlying IoTHubClient_LL_Create call */
    STRICT_EXPECTED_CALL(mocks, tickcounter_create());

    STRICT_EXPECTED_CALL(mocks, DList_InitializeListHead(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, FAKE_IoTHubTransport_Create(IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .SetReturn((TRANSPORT_LL_HANDLE)0x42);

    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, FAKE_IoTHubTransport_Register(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreAllArguments();

    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    ///act
    auto result = IoTHubClient_LL_CreateFromConnectionString(TEST_CHAR, provideFAKE);

    ///assert
    ASSERT_ARE_NOT_EQUAL(void_ptr, NULL, result);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubClient_LL_Destroy(result);

}

/*Tests_SRS_IOTHUBCLIENT_LL_12_014: [If either of key is missing or x509 is not set to "true" then IoTHubClient_LL_CreateFromConnectionString returns NULL ]*/
TEST_FUNCTION(IoTHubClient_LL_CreateFromConnectionString_with_x509_BLA_BLA_BLA_fails)
{
    ///arrange
    CIoTHubClientLLMocks mocks;

#ifndef NO_LOGGING
    STRICT_EXPECTED_CALL(mocks, IoTHubClient_GetVersionString());
#endif

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

    /* loop 1 -- HostName*/
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

    /* loop 2 -- DeviceId*/
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

    /* loop 3 -- x509*/
    EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, IGNORED_PTR_ARG))
        .SetReturn(0);
    EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_STRING_HANDLE, IGNORED_PTR_ARG))
        .SetReturn(0);
    EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .SetReturn(TEST_X509);
    EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .SetReturn("BLKA_BLA_BLA");

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
/* Tests_SRS_IOTHUBCLIENT_LL_12_009: [IoTHubClient_LL_CreateFromConnectionString shall split the value of HostName to Name and Suffix using the first "." as a separator] */
/* Tests_SRS_IOTHUBCLIENT_LL_12_015: [If the string split failed, IoTHubClient_LL_CreateFromConnectionString returns NULL ] */
TEST_FUNCTION(IoTHubClient_LL_CreateFromConnectionString_if_iotHubName_is_NULL_return_NULL)
{
    ///arrange
    CIoTHubClientLLMocks mocks;

#ifndef NO_LOGGING
    STRICT_EXPECTED_CALL(mocks, IoTHubClient_GetVersionString());
#endif

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

/*Tests_SRS_IOTHUBCLIENT_LL_12_009: [IoTHubClient_LL_CreateFromConnectionString shall split the value of HostName to Name and Suffix using the first "." as a separator] */
/*Tests_SRS_IOTHUBCLIENT_LL_12_015: [If the string split failed, IoTHubClient_LL_CreateFromConnectionString returns NULL ] */
TEST_FUNCTION(IoTHubClient_LL_CreateFromConnectionString_if_iotHubSuffix_is_NULL_return_NULL)
{
    ///arrange
    CIoTHubClientLLMocks mocks;

#ifndef NO_LOGGING
    STRICT_EXPECTED_CALL(mocks, IoTHubClient_GetVersionString());
#endif

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

/*Tests_SRS_IOTHUBCLIENT_LL_12_013: [If the parsing failed IoTHubClient_LL_CreateFromConnectionString returns NULL]  */
TEST_FUNCTION(IoTHubClient_LL_CreateFromConnectionString_STRING_construct_fails)
{
    ///arrange
    CIoTHubClientLLMocks mocks;

#ifndef NO_LOGGING
    STRICT_EXPECTED_CALL(mocks, IoTHubClient_GetVersionString());
#endif

    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);


    STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_CHAR))
        .IgnoreArgument(1)
        .SetReturn((STRING_HANDLE)NULL);

    ///act
    auto result = IoTHubClient_LL_CreateFromConnectionString(TEST_CHAR, provideFAKE);

    ///assert
    ASSERT_ARE_EQUAL(void_ptr, NULL, result);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubClient_LL_Destroy(result);
}

/*Tests_SRS_IOTHUBCLIENT_LL_12_013: [If the parsing failed IoTHubClient_LL_CreateFromConnectionString returns NULL]  */
TEST_FUNCTION(IoTHubClient_LL_CreateFromConnectionString_STRING_tokenizer_create_fails_1)
{
    ///arrange
    CIoTHubClientLLMocks mocks;

#ifndef NO_LOGGING
    STRICT_EXPECTED_CALL(mocks, IoTHubClient_GetVersionString());
#endif

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

/*Tests_SRS_IOTHUBCLIENT_LL_12_013: [If the parsing failed IoTHubClient_LL_CreateFromConnectionString returns NULL]  */
TEST_FUNCTION(IoTHubClient_LL_CreateFromConnectionString_STRING_tokenizer_create_fails_2)
{
    ///arrange
    CIoTHubClientLLMocks mocks;

#ifndef NO_LOGGING
    STRICT_EXPECTED_CALL(mocks, IoTHubClient_GetVersionString());
#endif

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

/*Tests_SRS_IOTHUBCLIENT_LL_12_013: [If the parsing failed IoTHubClient_LL_CreateFromConnectionString returns NULL]  */
TEST_FUNCTION(IoTHubClient_LL_CreateFromConnectionString_STRING_new_fails_1)
{
    ///arrange
    CIoTHubClientLLMocks mocks;

#ifndef NO_LOGGING
    STRICT_EXPECTED_CALL(mocks, IoTHubClient_GetVersionString());
#endif

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

/*Tests_SRS_IOTHUBCLIENT_LL_12_013: [If the parsing failed IoTHubClient_LL_CreateFromConnectionString returns NULL]  */
TEST_FUNCTION(IoTHubClient_LL_CreateFromConnectionString_STRING_new_fails_2)
{
    ///arrange
    CIoTHubClientLLMocks mocks;

#ifndef NO_LOGGING
    STRICT_EXPECTED_CALL(mocks, IoTHubClient_GetVersionString());
#endif

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

/*Tests_SRS_IOTHUBCLIENT_LL_12_013: [If the parsing failed IoTHubClient_LL_CreateFromConnectionString returns NULL]  */
TEST_FUNCTION(IoTHubClient_LL_CreateFromConnectionString_STRING_new_fails_3)
{
    ///arrange
    CIoTHubClientLLMocks mocks;

#ifndef NO_LOGGING
    STRICT_EXPECTED_CALL(mocks, IoTHubClient_GetVersionString());
#endif

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

/*Tests_SRS_IOTHUBCLIENT_LL_12_013: [If the parsing failed IoTHubClient_LL_CreateFromConnectionString returns NULL]  */
TEST_FUNCTION(IoTHubClient_LL_CreateFromConnectionString_STRING_new_fails_4)
{
    ///arrange
    CIoTHubClientLLMocks mocks;

#ifndef NO_LOGGING
    STRICT_EXPECTED_CALL(mocks, IoTHubClient_GetVersionString());
#endif

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

/*Tests_SRS_IOTHUBCLIENT_LL_12_013: [If the parsing failed IoTHubClient_LL_CreateFromConnectionString returns NULL]  */
TEST_FUNCTION(IoTHubClient_LL_CreateFromConnectionString_STRING_get_next_token_fails_1)
{
    ///arrange
    CIoTHubClientLLMocks mocks;

#ifndef NO_LOGGING
    STRICT_EXPECTED_CALL(mocks, IoTHubClient_GetVersionString());
#endif

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

/*Tests_SRS_IOTHUBCLIENT_LL_12_013: [If the parsing failed IoTHubClient_LL_CreateFromConnectionString returns NULL]  */
TEST_FUNCTION(IoTHubClient_LL_CreateFromConnectionString_STRING_get_next_token_fails_2)
{
    ///arrange
    CIoTHubClientLLMocks mocks;

#ifndef NO_LOGGING
    STRICT_EXPECTED_CALL(mocks, IoTHubClient_GetVersionString());
#endif

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

/*Tests_SRS_IOTHUBCLIENT_LL_12_013: [If the parsing failed IoTHubClient_LL_CreateFromConnectionString returns NULL]  */
TEST_FUNCTION(IoTHubClient_LL_CreateFromConnectionString_STRING_get_next_token_fails_3)
{
    ///arrange
    CIoTHubClientLLMocks mocks;

#ifndef NO_LOGGING
    STRICT_EXPECTED_CALL(mocks, IoTHubClient_GetVersionString());
#endif

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

/*Tests_SRS_IOTHUBCLIENT_LL_12_013: [If the parsing failed IoTHubClient_LL_CreateFromConnectionString returns NULL]  */
TEST_FUNCTION(IoTHubClient_LL_CreateFromConnectionString_STRING_get_next_token_fails_4)
{
    ///arrange
    CIoTHubClientLLMocks mocks;

#ifndef NO_LOGGING
    STRICT_EXPECTED_CALL(mocks, IoTHubClient_GetVersionString());
#endif

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
    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
        .IgnoreArgument(1);

    ///act
    auto result = IoTHubClient_LL_Create(&TEST_CONFIG);

    ///assert
    ASSERT_ARE_EQUAL(void_ptr, NULL, result);
}

#ifndef DONT_USE_UPLOADTOBLOB
/*Tests_SRS_IOTHUBCLIENT_LL_02_095: [ If creating the IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE fails then IoTHubClient_LL_Create shall fail and return NULL. ]*/
TEST_FUNCTION(IoTHubClient_LL_Create_fails_when_IoTHubClient_LL_UploadToBlob_Create_fails)
{
    ///arrange
    CIoTHubClientLLMocks mocks;

    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_UploadToBlob_Create(&TEST_CONFIG))
        .SetFailReturn((IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE)NULL);

    ///act
    auto result = IoTHubClient_LL_Create(&TEST_CONFIG);

    ///assert
    ASSERT_ARE_EQUAL(void_ptr, NULL, result);
}
#endif /*DONT_USE_UPLOADTOBLOB*/

/*Tests_SRS_IOTHUBCLIENT_LL_02_046: [ If creating the TICK_COUNTER_HANDLE fails then IoTHubClient_LL_Create shall fail and return NULL. ]*/
TEST_FUNCTION(IoTHubClient_LL_Create_fails_when_tick_counter_create_fails)
{
    ///arrange
    CIoTHubClientLLMocks mocks;

    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

#ifndef DONT_USE_UPLOADTOBLOB
    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_UploadToBlob_Create(&TEST_CONFIG));
    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_UploadToBlob_Destroy(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
#endif

    STRICT_EXPECTED_CALL(mocks, tickcounter_create())
        .SetFailReturn((TICK_COUNTER_HANDLE)NULL);

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

#ifndef DONT_USE_UPLOADTOBLOB
    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_UploadToBlob_Create(&TEST_CONFIG));
    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_UploadToBlob_Destroy(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
#endif

    STRICT_EXPECTED_CALL(mocks, tickcounter_create());
    STRICT_EXPECTED_CALL(mocks, tickcounter_destroy(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, DList_InitializeListHead(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, FAKE_IoTHubTransport_Create(IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .SetReturn((TRANSPORT_LL_HANDLE)NULL);

    ///act
    auto result = IoTHubClient_LL_Create(&TEST_CONFIG);

    ///assert
    ASSERT_ARE_EQUAL(void_ptr, NULL, result);
}

/*Tests_SRS_IOTHUBCLIENT_LL_17_009: [If the _Register function fails, this function shall fail and return NULL.]*/
TEST_FUNCTION(IoTHubClient_LL_Create_fails_when_underlying_transport_register_fails)
{
    ///arrange
    CIoTHubClientLLMocks mocks;
    IOTHUB_DEVICE_CONFIG device;
    device.deviceId = TEST_CONFIG.deviceId;
    device.deviceKey = TEST_CONFIG.deviceKey;
    device.deviceSasToken = NULL;

    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

#ifndef DONT_USE_UPLOADTOBLOB
    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_UploadToBlob_Create(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_UploadToBlob_Destroy(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
#endif /*DONT_USE_UPLOADTOBLOB*/

    STRICT_EXPECTED_CALL(mocks, tickcounter_create());
    STRICT_EXPECTED_CALL(mocks, tickcounter_destroy(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, DList_InitializeListHead(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, FAKE_IoTHubTransport_Create(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, FAKE_IoTHubTransport_Destroy(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, FAKE_IoTHubTransport_Register(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreAllArguments()
        .SetFailReturn((IOTHUB_DEVICE_HANDLE)NULL);

    ///act
    auto result = IoTHubClient_LL_Create(&TEST_CONFIG);

    ///assert
    ASSERT_ARE_EQUAL(void_ptr, NULL, result);
}

/*Tests_SRS_IOTHUBCLIENT_LL_02_094: [ IoTHubClient_LL_Create shall create a IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE from IOTHUB_CLIENT_CONFIG. ]*/
/*Tests_SRS_IOTHUBCLIENT_LL_02_045: [ Otherwise IoTHubClient_LL_Create shall create a new TICK_COUNTER_HANDLE ]*/
/*Tests_SRS_IOTHUBCLIENT_LL_02_004: [Otherwise IoTHubClient_LL_Create shall initialize a new DLIST (further called "waitingToSend") containing records with fields of the following types: IOTHUB_MESSAGE_HANDLE, IOTHUB_CLIENT_EVENT_CONFIRMATION_CALLBACK, void*.]*/
/*Tests_SRS_IOTHUBCLIENT_LL_02_006: [IoTHubClient_LL_Create shall populate a structure of type IOTHUBTRANSPORT_CONFIG with the information from config parameter and the previous DLIST and shall pass that to the underlying layer _Create function.]*/
/*Tests_SRS_IOTHUBCLIENT_LL_02_008: [Otherwise, IoTHubClient_LL_Create shall succeed and return a non-NULL handle.] */
/*Tests_SRS_IOTHUBCLIENT_LL_17_008: [IoTHubClient_LL_Create shall call the transport _Register function with the deviceId, DeviceKey and waitingToSend list.] */
TEST_FUNCTION(IoTHubClient_LL_Create_suceeds)
{
    ///arrange
    CIoTHubClientLLMocks mocks;
    IOTHUB_DEVICE_CONFIG device;
    device.deviceId = TEST_CONFIG.deviceId;
    device.deviceKey = TEST_CONFIG.deviceKey;
    device.deviceSasToken = NULL;

    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
        .IgnoreArgument(1);

#ifndef DONT_USE_UPLOADTOBLOB
    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_UploadToBlob_Create(&TEST_CONFIG));
#endif /*DONT_USE_UPLOADTOBLOB*/

    STRICT_EXPECTED_CALL(mocks, tickcounter_create());

    STRICT_EXPECTED_CALL(mocks, DList_InitializeListHead(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, FAKE_IoTHubTransport_Create(IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .SetReturn((TRANSPORT_LL_HANDLE)0x42);

    STRICT_EXPECTED_CALL(mocks, FAKE_IoTHubTransport_Register((TRANSPORT_LL_HANDLE)0x42, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(2)
        .IgnoreArgument(3)
        .IgnoreArgument(4);
   
    ///act
    auto result = IoTHubClient_LL_Create(&TEST_CONFIG);

    ///assert
    ASSERT_ARE_NOT_EQUAL(void_ptr, NULL, result);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubClient_LL_Destroy(result);
}

/*Tests_SRS_IOTHUBCLIENT_LL_17_001: [IoTHubClient_LL_CreateWithTransport shall return NULL if config parameter is NULL, or protocol field is NULL or transportHandle is NULL.]*/
TEST_FUNCTION(IoTHubClient_LL_CreateWithTransport_null_config_fails)
{
    CIoTHubClientLLMocks mocks;

    auto result = IoTHubClient_LL_CreateWithTransport(NULL);

    ASSERT_IS_NULL(result);

}

/*Tests_SRS_IOTHUBCLIENT_LL_17_001: [IoTHubClient_LL_CreateWithTransport shall return NULL if config parameter is NULL, or protocol field is NULL or transportHandle is NULL.]*/
TEST_FUNCTION(IoTHubClient_LL_CreateWithTransport_null_protocol_fails)
{
    CIoTHubClientLLMocks mocks;

    auto result = IoTHubClient_LL_CreateWithTransport(&TEST_DEVICE_CONFIG_null_protocol);

    ASSERT_IS_NULL(result);

}

/*Tests_SRS_IOTHUBCLIENT_LL_17_001: [IoTHubClient_LL_CreateWithTransport shall return NULL if config parameter is NULL, or protocol field is NULL or transportHandle is NULL.]*/
TEST_FUNCTION(IoTHubClient_LL_CreateWithTransport_null_transportHandle_fails)
{
    CIoTHubClientLLMocks mocks;

    auto result = IoTHubClient_LL_CreateWithTransport(&TEST_DEVICE_CONFIG_null_handle);

    ASSERT_IS_NULL(result);
}

/*Tests_SRS_IOTHUBCLIENT_LL_02_098: [ IoTHubClient_LL_CreateWithTransport shall fail and return NULL if both config->deviceKey AND config->deviceSasToken are NULL. ]*/
TEST_FUNCTION(IoTHubClient_LL_CreateWithTransport_with_NULL_deviceKey_AND_NULL_sas_token_fails)
{
    CIoTHubClientLLMocks mocks;

    auto result = IoTHubClient_LL_CreateWithTransport(&TEST_DEVICE_CONFIG_NULL_device_key_NULL_sas_token);

    ASSERT_IS_NULL(result);
}

/*Tests_SRS_IOTHUBCLIENT_LL_17_002: [IoTHubClient_LL_CreateWithTransport shall allocate data for the IOTHUB_CLIENT_LL_HANDLE.] */
/*Tests_SRS_IOTHUBCLIENT_LL_02_096: [ IoTHubClient_LL_CreateWithTransport shall create the data structures needed to instantiate a IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE. ]*/
/*Tests_SRS_IOTHUBCLIENT_LL_02_047: [ IoTHubClient_LL_CreateWithTransport shall create a TICK_COUNTER_HANDLE. ]*/
/*Tests_SRS_IOTHUBCLIENT_LL_17_004: [IoTHubClient_LL_CreateWithTransport shall initialize a new DLIST (further called "waitingToSend") containing records with fields of the following types: IOTHUB_MESSAGE_HANDLE, IOTHUB_CLIENT_EVENT_CONFIRMATION_CALLBACK, void*.] */
/*Tests_SRS_IOTHUBCLIENT_LL_17_006: [IoTHubClient_LL_CreateWithTransport shall call the transport _Register function with the deviceId, DeviceKey and waitingToSend list.]*/
TEST_FUNCTION(IoTHubClient_LL_CreateWithTransport_Succeeds)
{
    ///arrange
    CIoTHubClientLLMocks mocks;
    IOTHUB_DEVICE_CONFIG device;
    device.deviceId = TEST_DEVICE_CONFIG.deviceId;
    device.deviceKey = TEST_DEVICE_CONFIG.deviceKey;
    device.deviceSasToken = NULL;

#ifndef DONT_USE_UPLOADTOBLOB
    STRICT_EXPECTED_CALL(mocks, FAKE_IoTHubTransport_GetHostname(IGNORED_PTR_ARG)) /*this is getting the hostname as STRING_HANDLE*/
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG)) /*this is getting the hostname as const char* */
        .IgnoreArgument(1)
        .SetReturn(TEST_HOSTNAME_VALUE);
    
    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is IoTHubName (temporary)*/
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_UploadToBlob_Create(IGNORED_PTR_ARG)) /*this is creating the UploadToBlob HANDLE*/
        .IgnoreArgument(1);
#endif

    STRICT_EXPECTED_CALL(mocks, tickcounter_create());

    STRICT_EXPECTED_CALL(mocks, DList_InitializeListHead(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, FAKE_IoTHubTransport_Register(TEST_DEVICE_CONFIG.transportHandle, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(2)
        .IgnoreArgument(3)
        .IgnoreArgument(4);

    ///act
    auto result = IoTHubClient_LL_CreateWithTransport(&TEST_DEVICE_CONFIG);

    ///assert
    ASSERT_IS_NOT_NULL(result);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubClient_LL_Destroy(result);
}

/*Tests_SRS_IOTHUBCLIENT_LL_17_007: [If the _Register function fails, this function shall fail and return NULL.]*/
TEST_FUNCTION(IoTHubClient_LL_CreateWithTransport_transport_register_fails_returns_null)
{
    ///arrange
    CIoTHubClientLLMocks mocks;
    IOTHUB_DEVICE_CONFIG device;
    device.deviceId = TEST_DEVICE_CONFIG.deviceId;
    device.deviceKey = TEST_DEVICE_CONFIG.deviceKey;
    device.deviceSasToken = NULL;

#ifndef DONT_USE_UPLOADTOBLOB
    STRICT_EXPECTED_CALL(mocks, FAKE_IoTHubTransport_GetHostname(IGNORED_PTR_ARG)) /*this is getting the hostname as STRING_HANDLE*/
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG)) /*this is getting the hostname as const char* */
        .IgnoreArgument(1)
        .SetReturn(TEST_HOSTNAME_VALUE);

    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is IoTHubName (temporary)*/
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_UploadToBlob_Create(IGNORED_PTR_ARG)) /*this is creating the UploadToBlob HANDLE*/
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_UploadToBlob_Destroy(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
#endif

    STRICT_EXPECTED_CALL(mocks, tickcounter_create());
    STRICT_EXPECTED_CALL(mocks, tickcounter_destroy(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, DList_InitializeListHead(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, FAKE_IoTHubTransport_Register(TEST_DEVICE_CONFIG.transportHandle, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(2)
        .IgnoreArgument(3)
        .IgnoreArgument(4)
        .SetFailReturn((IOTHUB_DEVICE_HANDLE)NULL);

    ///act
    auto result = IoTHubClient_LL_CreateWithTransport(&TEST_DEVICE_CONFIG);

    ///assert
    ASSERT_IS_NULL(result);

    ///cleanup
}

/*Tests_SRS_IOTHUBCLIENT_LL_02_048: [ If creating the handle fails, then IoTHubClient_LL_CreateWithTransport shall fail and return NULL ]*/
TEST_FUNCTION(IoTHubClient_LL_CreateWithTransport_tick_counter_fails_returns_null)
{
    ///arrange
    CIoTHubClientLLMocks mocks;

    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

#ifndef DONT_USE_UPLOADTOBLOB
    STRICT_EXPECTED_CALL(mocks, FAKE_IoTHubTransport_GetHostname(IGNORED_PTR_ARG)) /*this is getting the hostname as STRING_HANDLE*/
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG)) /*this is getting the hostname as const char* */
        .IgnoreArgument(1)
        .SetReturn(TEST_HOSTNAME_VALUE);

    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is IoTHubName (temporary)*/
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_UploadToBlob_Create(IGNORED_PTR_ARG)) /*this is creating the UploadToBlob HANDLE*/
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_UploadToBlob_Destroy(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
#endif

    STRICT_EXPECTED_CALL(mocks, tickcounter_create())
        .SetFailReturn((TICK_COUNTER_HANDLE)NULL);

    ///act
    auto result = IoTHubClient_LL_CreateWithTransport(&TEST_DEVICE_CONFIG);

    ///assert
    ASSERT_IS_NULL(result);

    ///cleanup
}

#ifndef DONT_USE_UPLOADTOBLOB
/*Tests_SRS_IOTHUBCLIENT_LL_02_048: [ If creating the handle fails, then IoTHubClient_LL_CreateWithTransport shall fail and return NULL ]*/
TEST_FUNCTION(IoTHubClient_LL_CreateWithTransport_IoTHubClient_LL_UploadToBlob_Create_fails_returns_null)
{
    ///arrange
    CIoTHubClientLLMocks mocks;

    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, FAKE_IoTHubTransport_GetHostname(IGNORED_PTR_ARG)) /*this is getting the hostname as STRING_HANDLE*/
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG)) /*this is getting the hostname as const char* */
        .IgnoreArgument(1)
        .SetReturn(TEST_HOSTNAME_VALUE);

    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is IoTHubName (temporary)*/
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_UploadToBlob_Create(IGNORED_PTR_ARG)) /*this is creating the UploadToBlob HANDLE*/
        .IgnoreArgument(1)
        .SetFailReturn((IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE)NULL);

    ///act
    auto result = IoTHubClient_LL_CreateWithTransport(&TEST_DEVICE_CONFIG);

    ///assert
    ASSERT_IS_NULL(result);

    ///cleanup
}
#endif

#ifndef DONT_USE_UPLOADTOBLOB
/*Tests_SRS_IOTHUBCLIENT_LL_02_097: [ If creating the data structures fails or instantiating the IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE fails then IoTHubClient_LL_CreateWithTransport shall fail and return NULL. ]*/
TEST_FUNCTION(IoTHubClient_LL_CreateWithTransport_when_malloc_fails_returns_NULL)
{
    ///arrange
    CIoTHubClientLLMocks mocks;

    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);


    STRICT_EXPECTED_CALL(mocks, FAKE_IoTHubTransport_GetHostname(IGNORED_PTR_ARG)) /*this is getting the hostname as STRING_HANDLE*/
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG)) /*this is getting the hostname as const char* */
        .IgnoreArgument(1)
        .SetReturn(TEST_HOSTNAME_VALUE);

    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is IoTHubName (temporary)*/
        .IgnoreArgument(1)
        .SetFailReturn((void*)NULL);

    ///act
    auto result = IoTHubClient_LL_CreateWithTransport(&TEST_DEVICE_CONFIG);

    ///assert
    ASSERT_IS_NULL(result);

    ///cleanup
}
#endif

/*Tests_SRS_IOTHUBCLIENT_LL_17_003: [If allocation fails, the function shall fail and return NULL.]*/
TEST_FUNCTION(IoTHubClient_LL_CreateWithTransport_allocation_fails_returns_null)
{
    ///arrange
    CIoTHubClientLLMocks mocks;

    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
        .IgnoreArgument(1)
        .SetFailReturn((void*)NULL);

    ///act
    auto result = IoTHubClient_LL_CreateWithTransport(&TEST_DEVICE_CONFIG);

    ///assert
    ASSERT_IS_NULL(result);

    ///cleanup
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

/*Tests_SRS_IOTHUBCLIENT_LL_02_010: [If iotHubClientHandle was not created by IoTHubClient_LL_CreateWithTransport, IoTHubClient_LL_Destroy  shall call the underlaying layer's _Destroy function.] */
/*Tests_SRS_IOTHUBCLIENT_LL_17_010: [IoTHubClient_LL_Destroy  shall call the underlaying layer's _Unregister function] */
/*Tests_SRS_IOTHUBCLIENT_LL_17_011: [IoTHubClient_LL_Destroy  shall free the resources allocated by IoTHubClient (if any).] */
TEST_FUNCTION(IoTHubClient_LL_Destroys_the_underlying_transport_succeeds)
{
    ///arrange
    CIoTHubClientLLMocks mocks;
    auto handle = IoTHubClient_LL_Create(&TEST_CONFIG);
    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, FAKE_IoTHubTransport_Unregister(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, FAKE_IoTHubTransport_Destroy(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, tickcounter_destroy(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

#ifndef DONT_USE_UPLOADTOBLOB
    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_UploadToBlob_Destroy(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
#endif

    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, DList_RemoveHeadList(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    ///act
    IoTHubClient_LL_Destroy(handle);

    ///assert -uMock does it
}

/*Tests_SRS_IOTHUBCLIENT_LL_17_005: [IoTHubClient_LL_CreateWithTransport shall save the transport handle and mark this transport as shared.] */
TEST_FUNCTION(IoTHubClient_LL_Destroys_unregisters_but_does_not_destroy_transport_succeeds)
{
    ///arrange
    CIoTHubClientLLMocks mocks;
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .SetReturn(TEST_HOSTNAME_VALUE);
    auto handle = IoTHubClient_LL_CreateWithTransport(&TEST_DEVICE_CONFIG);
    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, FAKE_IoTHubTransport_Unregister(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, tickcounter_destroy(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

#ifndef DONT_USE_UPLOADTOBLOB
    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_UploadToBlob_Destroy(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
#endif

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

    STRICT_EXPECTED_CALL(mocks, FAKE_IoTHubTransport_Unregister(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, FAKE_IoTHubTransport_Destroy(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*IOTHUBCLIENT*/
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, DList_RemoveHeadList(IGNORED_PTR_ARG)) /*because there is one item in the list*/
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, eventConfirmationCallback(IOTHUB_CLIENT_CONFIRMATION_BECAUSE_DESTROY, (void*)1));
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Destroy(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, tickcounter_destroy(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

#ifndef DONT_USE_UPLOADTOBLOB
    STRICT_EXPECTED_CALL(mocks, IoTHubClient_LL_UploadToBlob_Destroy(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
#endif

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

/*Tests_SRS_IOTHUBCLIENT_LL_02_014: [If cloning and/or adding the information fails for any reason, IoTHubClient_LL_SendEventAsync shall fail and return IOTHUB_CLIENT_ERROR.]*/
TEST_FUNCTION(IoTHubClient_LL_SendEventAsync_fails_when_current_ms_cannot_be_obtained)
{
    ///arrange
    CIoTHubClientLLMocks mocks;
    auto handle = IoTHubClient_LL_Create(&TEST_CONFIG);
    uint64_t thisIsNotZero = 312984751;
    (void)IoTHubClient_LL_SetOption(handle, "messageTimeout", &thisIsNotZero); /*this forces _SendEventAsync to query the currentTime. If that fails, _SendEvent should fail as well*/
    auto messageHandle = (IOTHUB_MESSAGE_HANDLE)1;
    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*because _Clone fails below*/
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(mocks, tickcounter_get_current_ms(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreAllArguments()
        .SetFailReturn(__LINE__);

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

    STRICT_EXPECTED_CALL(mocks, tickcounter_get_current_ms(IGNORED_PTR_ARG, IGNORED_PTR_ARG)) /*_DoWork will ask "what's the time"*/
        .IgnoreAllArguments();

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
    IoTHubClient_LL_SendComplete(NULL, &temp, IOTHUB_CLIENT_CONFIRMATION_OK);

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
    IoTHubClient_LL_SendComplete(handle, NULL, IOTHUB_CLIENT_CONFIRMATION_OK);

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
    IoTHubClient_LL_SendComplete(handle, &temp, IOTHUB_CLIENT_CONFIRMATION_OK);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubClient_LL_Destroy(handle);
}

/*Tests_SRS_IOTHUBCLIENT_LL_02_025: [If parameter result is IOTHUB_CLIENT_CONFIRMATION_OK then IoTHubClient_LL_SendComplete shall call all the non-NULL callbacks with the result parameter set to IOTHUB_CLIENT_CONFIRMATION_OK and the context set to the context passed originally in the SendEventAsync call.]*/
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
    IoTHubClient_LL_SendComplete(handle, &temp, IOTHUB_CLIENT_CONFIRMATION_OK);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubClient_LL_Destroy(handle);
}

/*Tests_SRS_IOTHUBCLIENT_LL_02_025: [If parameter result is IOTHUB_CLIENT_CONFIRMATION_OK then IoTHubClient_LL_SendComplete shall call all the non-NULL callbacks with the result parameter set to IOTHUB_CLIENT_CONFIRMATION_OK and the context set to the context passed originally in the SendEventAsync call.]*/
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
    IoTHubClient_LL_SendComplete(handle, &temp, IOTHUB_CLIENT_CONFIRMATION_OK);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubClient_LL_Destroy(handle);
}

/*Tests_SRS_IOTHUBCLIENT_LL_02_025: [If parameter result is IOTHUB_CLIENT_CONFIRMATION_OK then IoTHubClient_LL_SendComplete shall call all the non-NULL callbacks with the result parameter set to IOTHUB_CLIENT_CONFIRMATION_OK and the context set to the context passed originally in the SendEventAsync call.]*/
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
    IoTHubClient_LL_SendComplete(handle, &temp, IOTHUB_CLIENT_CONFIRMATION_OK);

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
    IoTHubClient_LL_SendComplete(handle, &temp, IOTHUB_CLIENT_CONFIRMATION_ERROR);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubClient_LL_Destroy(handle);
}

/*Tests_SRS_IOTHUBCLIENT_LL_02_026: [If any callback is NULL then there shall not be a callback call.] */
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
    IoTHubClient_LL_SendComplete(handle, &temp, IOTHUB_CLIENT_CONFIRMATION_ERROR);

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
/*Tests_SRS_IOTHUBCLIENT_LL_02_031: [Then IoTHubClient_LL_MessageCallback shall return what the user function returns.]*/
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

#ifndef DONT_USE_UPLOADTOBLOB
/*these tests are to be run when upload to blob functionality exists*/

/*Tests_SRS_IOTHUBCLIENT_LL_02_099 : [IoTHubClient_LL_SetOption shall return according to the table below]*/
TEST_FUNCTION(IoTHubClient_LL_SetOption_all_combinations)
{

    static struct SetOptionTriplet
    {
        IOTHUB_CLIENT_RESULT IoTHubClient_UploadToBlob_SetOption_return;
        IOTHUB_CLIENT_RESULT Transport_SetOption_return;
        IOTHUB_CLIENT_RESULT expected_return;
    } allCombinations[] = 
    {
        { IOTHUB_CLIENT_OK, IOTHUB_CLIENT_OK , IOTHUB_CLIENT_OK},
        { IOTHUB_CLIENT_OK, IOTHUB_CLIENT_ERROR, IOTHUB_CLIENT_ERROR },
        { IOTHUB_CLIENT_OK, IOTHUB_CLIENT_INVALID_ARG, IOTHUB_CLIENT_OK },
        { IOTHUB_CLIENT_ERROR, IOTHUB_CLIENT_OK , IOTHUB_CLIENT_ERROR },
        { IOTHUB_CLIENT_ERROR, IOTHUB_CLIENT_ERROR, IOTHUB_CLIENT_ERROR },
        { IOTHUB_CLIENT_ERROR, IOTHUB_CLIENT_INVALID_ARG, IOTHUB_CLIENT_ERROR },
        { IOTHUB_CLIENT_INVALID_ARG, IOTHUB_CLIENT_OK , IOTHUB_CLIENT_OK },
        { IOTHUB_CLIENT_INVALID_ARG, IOTHUB_CLIENT_ERROR, IOTHUB_CLIENT_ERROR },
        { IOTHUB_CLIENT_INVALID_ARG, IOTHUB_CLIENT_INVALID_ARG, IOTHUB_CLIENT_INVALID_ARG }
    };

    ///arrange
    CIoTHubClientLLMocks mocks;
    IOTHUB_CLIENT_LL_HANDLE handle = IoTHubClient_LL_Create(&TEST_CONFIG);
    

    for (size_t i = 0;i < sizeof(allCombinations) / sizeof(allCombinations[0]);i++)
    {
        mocks.ResetAllCalls();
        EXPECTED_CALL(mocks, IoTHubClient_LL_UploadToBlob_SetOption(IGNORED_PTR_ARG, "a", "b"))
            .SetReturn(allCombinations[i].IoTHubClient_UploadToBlob_SetOption_return);

        EXPECTED_CALL(mocks, FAKE_IoTHubTransport_SetOption(IGNORED_PTR_ARG, "a", "b"))
            .SetReturn(allCombinations[i].Transport_SetOption_return);

        ///act
        auto result = IoTHubClient_LL_SetOption(handle, "a", "b");

        ///assert
        ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, allCombinations[i].expected_return, result);
    }

    ///cleanup
    IoTHubClient_LL_Destroy(handle);
    mocks.ResetAllCalls();
}

/*Tests_SRS_IOTHUBCLIENT_LL_02_038: [Otherwise, IoTHubClient_LL shall call the function _SetOption of the underlying transport and return what that function is returning.] */
TEST_FUNCTION(IoTHubClient_LL_SetOption_fails_when_underlying_transport_fails)
{
    ///arrange
    CIoTHubClientLLMocks mocks;
    IOTHUB_CLIENT_LL_HANDLE handle = IoTHubClient_LL_Create(&TEST_CONFIG);
    mocks.ResetAllCalls();

    EXPECTED_CALL(mocks, IoTHubClient_LL_UploadToBlob_SetOption(IGNORED_PTR_ARG, "a", "b"))
        .SetReturn(IOTHUB_CLIENT_INVALID_ARG);

    EXPECTED_CALL(mocks, FAKE_IoTHubTransport_SetOption(IGNORED_PTR_ARG, "a", "b"))
        .SetReturn(IOTHUB_CLIENT_INDEFINITE_TIME);

    ///act
    auto result = IoTHubClient_LL_SetOption(handle, "a", "b");

    ///assert
    ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_INDEFINITE_TIME, result);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubClient_LL_Destroy(handle);

}
#else
/*these tests are to be run when uploadtoblob is not present*/
TEST_FUNCTION(IoTHubClient_LL_SetOption_fails_when_underlying_transport_fails)
{
    ///arrange
    CIoTHubClientLLMocks mocks;
    IOTHUB_CLIENT_LL_HANDLE handle = IoTHubClient_LL_Create(&TEST_CONFIG);
    mocks.ResetAllCalls();

    EXPECTED_CALL(mocks, FAKE_IoTHubTransport_SetOption(IGNORED_PTR_ARG, "a", "b"))
        .SetReturn(IOTHUB_CLIENT_INDEFINITE_TIME);

    ///act
    auto result = IoTHubClient_LL_SetOption(handle, "a", "b");

    ///assert
    ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_INDEFINITE_TIME, result);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubClient_LL_Destroy(handle);

}
#endif
/*Tests_SRS_IOTHUBCLIENT_LL_02_039: [ "messageTimeout" - once IoTHubClient_LL_SendEventAsync is called the message shall timeout after value miliseconds. Value is a pointer to a uint64. ]*/
TEST_FUNCTION(IoTHubClient_LL_SetOption_messageTimeout_to_zero_after_Create_succeeds)
{
    ///arrange
    CIoTHubClientLLMocks mocks;
    IOTHUB_CLIENT_LL_HANDLE handle = IoTHubClient_LL_Create(&TEST_CONFIG);
    mocks.ResetAllCalls();

    ///act
    uint64_t zero = 0;
    auto result = IoTHubClient_LL_SetOption(handle, "messageTimeout", &zero);

    ///assert
    ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK, result);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubClient_LL_Destroy(handle);
}

/*Tests_SRS_IOTHUBCLIENT_LL_02_039: [ "messageTimeout" - once IoTHubClient_LL_SendEventAsync is called the message shall timeout after value miliseconds. Value is a pointer to a uint64. ]*/
TEST_FUNCTION(IoTHubClient_LL_SetOption_messageTimeout_to_one_after_Create_succeeds)
{
    ///arrange
    CIoTHubClientLLMocks mocks;
    IOTHUB_CLIENT_LL_HANDLE handle = IoTHubClient_LL_Create(&TEST_CONFIG);
    mocks.ResetAllCalls();

    ///act
    uint64_t one = 1;
    auto result = IoTHubClient_LL_SetOption(handle, "messageTimeout", &one);

    ///assert
    ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK, result);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubClient_LL_Destroy(handle);

}

/*Tests_SRS_IOTHUBCLIENT_LL_02_039: [ "messageTimeout" - once IoTHubClient_LL_SendEventAsync is called the message shall timeout after value miliseconds. Value is a pointer to a uint64. ]*/
TEST_FUNCTION(IoTHubClient_LL_SetOption_messageTimeout_can_be_reverted_back_to_zero_succeeds)
{
    ///arrange
    CIoTHubClientLLMocks mocks;
    IOTHUB_CLIENT_LL_HANDLE handle = IoTHubClient_LL_Create(&TEST_CONFIG);
    uint64_t one = 1;
    (void)IoTHubClient_LL_SetOption(handle, "messageTimeout", &one);
    mocks.ResetAllCalls();

    ///act
    uint64_t zero = 0;
    auto result = IoTHubClient_LL_SetOption(handle, "messageTimeout", &zero);

    ///assert
    ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK, result);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubClient_LL_Destroy(handle);
}

/*Tests_SRS_IOTHUBCLIENT_LL_02_039: [ "messageTimeout" - once IoTHubClient_LL_SendEventAsync is called the message shall timeout after value miliseconds. Value is a pointer to a uint64. ]*/
/*Tests_SRS_IOTHUBCLIENT_LL_02_041: [ If more than value miliseconds have passed since the call to IoTHubClient_LL_SendEventAsync then the message callback shall be called with a status code of IOTHUB_CLIENT_CONFIRMATION_TIMEOUT. ]*/
TEST_FUNCTION(IoTHubClient_LL_SetOption_messageTimeout_calls_timeout_callback)
{
    ///arrange
    CIoTHubClientLLMocks mocks;
    IOTHUB_CLIENT_LL_HANDLE handle = IoTHubClient_LL_Create(&TEST_CONFIG);
    uint64_t one = 1;
    (void)IoTHubClient_LL_SetOption(handle, "messageTimeout", &one);

    /*send 1 messages that will expire*/
    /*because sending messages stamps the message's timeout, the call to tickcounter_get_current_ms needs to be here, so the test can says
    "the message has been received at time=10*/
    uint64_t ten = 10;
    STRICT_EXPECTED_CALL(mocks, tickcounter_get_current_ms(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .CopyOutArgumentBuffer(2, &ten, sizeof(ten));
    (void)IoTHubClient_LL_SendEventAsync(handle, TEST_DEVICEMESSAGE_HANDLE, eventConfirmationCallback, (void*)TEST_DEVICEMESSAGE_HANDLE);
    mocks.ResetAllCalls();

    /*we don't care what happens in the Transport, so let's ignore all those calls*/
    EXPECTED_CALL(mocks, FAKE_IoTHubTransport_DoWork(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreAllCalls();

    uint64_t twelve = 12; /*12 > 10 (receive time) + 1 (timeout) => timeout*/
    STRICT_EXPECTED_CALL(mocks, tickcounter_get_current_ms(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .CopyOutArgumentBuffer(2, &twelve, sizeof(twelve));

    STRICT_EXPECTED_CALL(mocks, DList_RemoveEntryList(IGNORED_PTR_ARG)) /*this is removing the item from waitingToSend*/
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, eventConfirmationCallback(IOTHUB_CLIENT_CONFIRMATION_MESSAGE_TIMEOUT, (void*)TEST_DEVICEMESSAGE_HANDLE)); /*calling the callback*/
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Destroy(IGNORED_PTR_ARG)) /*destroying the message clone*/
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*destroying the IOTHUB_MESSAGE_LIST*/
        .IgnoreArgument(1);

    ///act
    IoTHubClient_LL_DoWork(handle);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubClient_LL_Destroy(handle);
}

/*Tests_SRS_IOTHUBCLIENT_LL_02_042: [ By default, messages shall not timeout. ]*/
TEST_FUNCTION(IoTHubClient_LL_SetOption_defaults_to_zero)
{
    ///arrange
    CIoTHubClientLLMocks mocks;
    IOTHUB_CLIENT_LL_HANDLE handle = IoTHubClient_LL_Create(&TEST_CONFIG);

    /*send 1 messages that will expire*/
    /*because sending messages stamps the message's timeout, the call to tickcounter_get_current_ms needs to be here, so the test can says
    "the message has been received at time=10*/
    uint64_t ten = 10;
    STRICT_EXPECTED_CALL(mocks, tickcounter_get_current_ms(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .CopyOutArgumentBuffer(2, &ten, sizeof(ten));
    (void)IoTHubClient_LL_SendEventAsync(handle, TEST_DEVICEMESSAGE_HANDLE, eventConfirmationCallback, (void*)TEST_DEVICEMESSAGE_HANDLE);
    mocks.ResetAllCalls();

    /*we don't care what happens in the Transport, so let's ignore all those calls*/
    EXPECTED_CALL(mocks, FAKE_IoTHubTransport_DoWork(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreAllCalls();

    uint64_t twelve = 12; /*12 > 10 (receive time) + 1 (timeout) => would result in timeout, except the fact that messageTimeout option has never been set, therefore no timeout shall be called*/
    STRICT_EXPECTED_CALL(mocks, tickcounter_get_current_ms(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .CopyOutArgumentBuffer(2, &twelve, sizeof(twelve));

    ///act
    IoTHubClient_LL_DoWork(handle);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubClient_LL_Destroy(handle);
}

/*Tests_SRS_IOTHUBCLIENT_LL_02_039: [ "messageTimeout" - once IoTHubClient_LL_SendEventAsync is called the message shall timeout after value miliseconds. Value is a pointer to a uint64. ]*/
TEST_FUNCTION(IoTHubClient_LL_SetOption_messageTimeout_only_removes_the_message_if_it_has_NULL_callback)
{
    ///arrange
    CIoTHubClientLLMocks mocks;
    IOTHUB_CLIENT_LL_HANDLE handle = IoTHubClient_LL_Create(&TEST_CONFIG);
    uint64_t one = 1;
    (void)IoTHubClient_LL_SetOption(handle, "messageTimeout", &one);

    /*send 1 messages that will expire*/
    /*because sending messages stamps the message's timeout, the call to tickcounter_get_current_ms needs to be here, so the test can says
    "the message has been received at time=10*/
    uint64_t ten = 10;
    STRICT_EXPECTED_CALL(mocks, tickcounter_get_current_ms(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .CopyOutArgumentBuffer(2, &ten, sizeof(ten));
    (void)IoTHubClient_LL_SendEventAsync(handle, TEST_DEVICEMESSAGE_HANDLE, NULL, NULL); /*this is a message with a NULL callback*/
    mocks.ResetAllCalls();

    /*we don't care what happens in the Transport, so let's ignore all those calls*/
    EXPECTED_CALL(mocks, FAKE_IoTHubTransport_DoWork(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreAllCalls();

    uint64_t twelve = 12; /*12 > 10 (receive time) + 1 (timeout) => timeout*/
    STRICT_EXPECTED_CALL(mocks, tickcounter_get_current_ms(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .CopyOutArgumentBuffer(2, &twelve, sizeof(twelve));

    STRICT_EXPECTED_CALL(mocks, DList_RemoveEntryList(IGNORED_PTR_ARG)) /*this is removing the item from waitingToSend*/
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Destroy(IGNORED_PTR_ARG)) /*destroying the message clone*/
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*destroying the IOTHUB_MESSAGE_LIST*/
        .IgnoreArgument(1);

    ///act
    IoTHubClient_LL_DoWork(handle);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubClient_LL_Destroy(handle);
}

/*Tests_SRS_IOTHUBCLIENT_LL_02_039: [ "messageTimeout" - once IoTHubClient_LL_SendEventAsync is called the message shall timeout after value miliseconds. Value is a pointer to a uint64. ]*/
/*Tests_SRS_IOTHUBCLIENT_LL_02_041: [ If more than value miliseconds have passed since the call to IoTHubClient_LL_SendEventAsync then the message callback shall be called with a status code of IOTHUB_CLIENT_CONFIRMATION_TIMEOUT. ]*/
TEST_FUNCTION(IoTHubClient_LL_SetOption_messageTimeout_when_exactly_on_the_edge_does_not_call_the_callback) /*because "more"*/
{
    ///arrange
    CIoTHubClientLLMocks mocks;
    IOTHUB_CLIENT_LL_HANDLE handle = IoTHubClient_LL_Create(&TEST_CONFIG);
    uint64_t one = 1;
    (void)IoTHubClient_LL_SetOption(handle, "messageTimeout", &one);

    /*send 1 messages that will expire*/
    /*because sending messages stamps the message's timeout, the call to tickcounter_get_current_ms needs to be here, so the test can says
    "the message has been received at time=10*/
    uint64_t ten = 10;
    STRICT_EXPECTED_CALL(mocks, tickcounter_get_current_ms(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .CopyOutArgumentBuffer(2, &ten, sizeof(ten));
    (void)IoTHubClient_LL_SendEventAsync(handle, TEST_DEVICEMESSAGE_HANDLE, eventConfirmationCallback, (void*)TEST_DEVICEMESSAGE_HANDLE);
    mocks.ResetAllCalls();

    /*we don't care what happens in the Transport, so let's ignore all those calls*/
    EXPECTED_CALL(mocks, FAKE_IoTHubTransport_DoWork(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreAllCalls();

    uint64_t eleven = 11; /*11 = 10 (receive time) + 1 (timeout) => NO timeout*/
    STRICT_EXPECTED_CALL(mocks, tickcounter_get_current_ms(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .CopyOutArgumentBuffer(2, &eleven, sizeof(eleven));

    ///act
    IoTHubClient_LL_DoWork(handle);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubClient_LL_Destroy(handle);
}

/*Tests_SRS_IOTHUBCLIENT_LL_02_039: [ "messageTimeout" - once IoTHubClient_LL_SendEventAsync is called the message shall timeout after value miliseconds. Value is a pointer to a uint64. ]*/
/*Tests_SRS_IOTHUBCLIENT_LL_02_041: [ If more than value miliseconds have passed since the call to IoTHubClient_LL_SendEventAsync then the message callback shall be called with a status code of IOTHUB_CLIENT_CONFIRMATION_TIMEOUT. ]*/
TEST_FUNCTION(IoTHubClient_LL_SetOption_2_messages_with_timeouts_at_11_and_12_calls_1_timeout) /*test wants to see that message that did not timeout yet do not have their callbacks called*/
{
    ///arrange
    CIoTHubClientLLMocks mocks;
    IOTHUB_CLIENT_LL_HANDLE handle = IoTHubClient_LL_Create(&TEST_CONFIG);
    uint64_t one = 1;
    (void)IoTHubClient_LL_SetOption(handle, "messageTimeout", &one);

    /*send 2 messages that will expire at 12 and 13, both of these messages are send at time=10*/
    /*because sending messages stamps the message's timeout, the call to tickcounter_get_current_ms needs to be here, so the test can says
    "the message has been received at time=10*/
    uint64_t ten = 10;
    STRICT_EXPECTED_CALL(mocks, tickcounter_get_current_ms(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .CopyOutArgumentBuffer(2, &ten, sizeof(ten));
    (void)IoTHubClient_LL_SendEventAsync(handle, TEST_DEVICEMESSAGE_HANDLE, eventConfirmationCallback, (void*)TEST_DEVICEMESSAGE_HANDLE);

    uint64_t two = 2;
    (void)IoTHubClient_LL_SetOption(handle, "messageTimeout", &two);
    STRICT_EXPECTED_CALL(mocks, tickcounter_get_current_ms(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .CopyOutArgumentBuffer(2, &ten, sizeof(ten));
    (void)IoTHubClient_LL_SendEventAsync(handle, TEST_DEVICEMESSAGE_HANDLE, eventConfirmationCallback, (void*)(TEST_DEVICEMESSAGE_HANDLE_2 ));

    mocks.ResetAllCalls();

    /*we don't care what happens in the Transport, so let's ignore all those calls*/
    EXPECTED_CALL(mocks, FAKE_IoTHubTransport_DoWork(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreAllCalls();

    uint64_t twelve = 12; /*12 > 10 (receive time) + 1 (timeout) => timeout!!!*/
    STRICT_EXPECTED_CALL(mocks, tickcounter_get_current_ms(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .CopyOutArgumentBuffer(2, &twelve, sizeof(twelve));

    STRICT_EXPECTED_CALL(mocks, DList_RemoveEntryList(IGNORED_PTR_ARG)) /*this is removing the item from waitingToSend*/
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, eventConfirmationCallback(IOTHUB_CLIENT_CONFIRMATION_MESSAGE_TIMEOUT, (void*)TEST_DEVICEMESSAGE_HANDLE)); /*calling the callback*/
    STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Destroy(IGNORED_PTR_ARG)) /*destroying the message clone*/
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*destroying the IOTHUB_MESSAGE_LIST*/
        .IgnoreArgument(1);

    /*because we're at time = 12 in this test, the second message is untouched*/

    ///act
    IoTHubClient_LL_DoWork(handle);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubClient_LL_Destroy(handle);
}

/*Tests_SRS_IOTHUBCLIENT_LL_02_039: [ "messageTimeout" - once IoTHubClient_LL_SendEventAsync is called the message shall timeout after value miliseconds. Value is a pointer to a uint64. ]*/
/*Tests_SRS_IOTHUBCLIENT_LL_02_041: [ If more than value miliseconds have passed since the call to IoTHubClient_LL_SendEventAsync then the message callback shall be called with a status code of IOTHUB_CLIENT_CONFIRMATION_TIMEOUT. ]*/
/*Tests_SRS_IOTHUBCLIENT_LL_02_044: [ Messages already delivered to IoTHubClient_LL shall not have their timeouts modified by a new call to IoTHubClient_LL_SetOption. ]*/
TEST_FUNCTION(IoTHubClient_LL_SetOption_2_messages_with_timeouts_at_11_and_12_calls_2_timeouts) /*test wants to see that message that did not timeout yet do not have their callbacks called*/
{
    ///arrange
    CIoTHubClientLLMocks mocks;
    IOTHUB_CLIENT_LL_HANDLE handle = IoTHubClient_LL_Create(&TEST_CONFIG);
    uint64_t one = 1;
    (void)IoTHubClient_LL_SetOption(handle, "messageTimeout", &one);

    /*send 2 messages that will expire at 12 and 13, both of these messages are send at time=10*/
    /*because sending messages stamps the message's timeout, the call to tickcounter_get_current_ms needs to be here, so the test can says
    "the message has been received at time=10*/
    uint64_t ten = 10;
    STRICT_EXPECTED_CALL(mocks, tickcounter_get_current_ms(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .CopyOutArgumentBuffer(2, &ten, sizeof(ten));
    (void)IoTHubClient_LL_SendEventAsync(handle, TEST_DEVICEMESSAGE_HANDLE, eventConfirmationCallback, (void*)TEST_DEVICEMESSAGE_HANDLE);

    uint64_t two = 2;
    (void)IoTHubClient_LL_SetOption(handle, "messageTimeout", &two);
    STRICT_EXPECTED_CALL(mocks, tickcounter_get_current_ms(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .CopyOutArgumentBuffer(2, &ten, sizeof(ten));
    (void)IoTHubClient_LL_SendEventAsync(handle, TEST_DEVICEMESSAGE_HANDLE, eventConfirmationCallback, (void*)(TEST_DEVICEMESSAGE_HANDLE_2));

    mocks.ResetAllCalls();

    /*we don't care what happens in the Transport, so let's ignore all those calls*/
    EXPECTED_CALL(mocks, FAKE_IoTHubTransport_DoWork(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreAllCalls();

    {/*this scope happen in the first _DoWork call*/
        uint64_t timeIsNow = 12; /*12 > 10 (receive time) + 1 (timeout) => timeout!!!*/
        STRICT_EXPECTED_CALL(mocks, tickcounter_get_current_ms(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .CopyOutArgumentBuffer(2, &timeIsNow, sizeof(timeIsNow));

        STRICT_EXPECTED_CALL(mocks, DList_RemoveEntryList(IGNORED_PTR_ARG)) /*this is removing the item from waitingToSend*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, eventConfirmationCallback(IOTHUB_CLIENT_CONFIRMATION_MESSAGE_TIMEOUT, (void*)TEST_DEVICEMESSAGE_HANDLE)); /*calling the callback*/
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Destroy(IGNORED_PTR_ARG)) /*destroying the message clone*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*destroying the IOTHUB_MESSAGE_LIST*/
            .IgnoreArgument(1);
    }

    {/*this scope happen in the second _DoWork call*/
        uint64_t timeIsNow = 13; /*13 > 10 (receive time) + 2 (timeout) => timeout!!!*/
        STRICT_EXPECTED_CALL(mocks, tickcounter_get_current_ms(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .CopyOutArgumentBuffer(2, &timeIsNow, sizeof(timeIsNow));

        STRICT_EXPECTED_CALL(mocks, DList_RemoveEntryList(IGNORED_PTR_ARG)) /*this is removing the item from waitingToSend*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, eventConfirmationCallback(IOTHUB_CLIENT_CONFIRMATION_MESSAGE_TIMEOUT, (void*)(TEST_DEVICEMESSAGE_HANDLE_2))); /*calling the callback*/
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Destroy(IGNORED_PTR_ARG)) /*destroying the message clone*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*destroying the IOTHUB_MESSAGE_LIST*/
            .IgnoreArgument(1);
    }

    /*because we're at time = 13 in this test, the second message times out too*/

    ///act
    IoTHubClient_LL_DoWork(handle);
    IoTHubClient_LL_DoWork(handle);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubClient_LL_Destroy(handle);
}

/*Tests_SRS_IOTHUBCLIENT_LL_02_039: [ "messageTimeout" - once IoTHubClient_LL_SendEventAsync is called the message shall timeout after value miliseconds. Value is a pointer to a uint64. ]*/
/*Tests_SRS_IOTHUBCLIENT_LL_02_041: [ If more than value miliseconds have passed since the call to IoTHubClient_LL_SendEventAsync then the message callback shall be called with a status code of IOTHUB_CLIENT_CONFIRMATION_TIMEOUT. ]*/
/*Tests_SRS_IOTHUBCLIENT_LL_02_043: [ Calling IoTHubClient_LL_SetOption with value set to "0" shall disable the timeout mechanism for all new messages. ]*/
/*Tests_SRS_IOTHUBCLIENT_LL_02_044: [ Messages already delivered to IoTHubClient_LL shall not have their timeouts modified by a new call to IoTHubClient_LL_SetOption. ]*/
TEST_FUNCTION(IoTHubClient_LL_SetOption_2_messages_one_with_timeout_one_without_call_1_callback) /*test wants to see that message that did not timeout yet do not have their callbacks called*/
{
    ///arrange
    CIoTHubClientLLMocks mocks;
    IOTHUB_CLIENT_LL_HANDLE handle = IoTHubClient_LL_Create(&TEST_CONFIG);
    uint64_t one = 1;
    (void)IoTHubClient_LL_SetOption(handle, "messageTimeout", &one);

    /*send 2 messages that will expire at 12 and 13, both of these messages are send at time=10*/
    /*because sending messages stamps the message's timeout, the call to tickcounter_get_current_ms needs to be here, so the test can says
    "the message has been received at time=10*/
    uint64_t ten = 10;
    STRICT_EXPECTED_CALL(mocks, tickcounter_get_current_ms(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .CopyOutArgumentBuffer(2, &ten, sizeof(ten));
    (void)IoTHubClient_LL_SendEventAsync(handle, TEST_DEVICEMESSAGE_HANDLE, eventConfirmationCallback, (void*)TEST_DEVICEMESSAGE_HANDLE);

    uint64_t zero = 0;
    (void)IoTHubClient_LL_SetOption(handle, "messageTimeout", &zero); /*essentially no timeout*/
    (void)IoTHubClient_LL_SendEventAsync(handle, TEST_DEVICEMESSAGE_HANDLE, eventConfirmationCallback, (void*)(TEST_DEVICEMESSAGE_HANDLE_2));

    mocks.ResetAllCalls();

    /*we don't care what happens in the Transport, so let's ignore all those calls*/
    EXPECTED_CALL(mocks, FAKE_IoTHubTransport_DoWork(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreAllCalls();

    {/*this scope happen in the first _DoWork call*/
        uint64_t timeIsNow = 12; /*12 > 10 (receive time) + 1 (timeout) => timeout!!!*/
        STRICT_EXPECTED_CALL(mocks, tickcounter_get_current_ms(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .CopyOutArgumentBuffer(2, &timeIsNow, sizeof(timeIsNow));

        STRICT_EXPECTED_CALL(mocks, DList_RemoveEntryList(IGNORED_PTR_ARG)) /*this is removing the item from waitingToSend*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, eventConfirmationCallback(IOTHUB_CLIENT_CONFIRMATION_MESSAGE_TIMEOUT, (void*)TEST_DEVICEMESSAGE_HANDLE)); /*calling the callback*/
        STRICT_EXPECTED_CALL(mocks, IoTHubMessage_Destroy(IGNORED_PTR_ARG)) /*destroying the message clone*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*destroying the IOTHUB_MESSAGE_LIST*/
            .IgnoreArgument(1);
    }

    {/*this scope happen in the second _DoWork call*/
        uint64_t timeIsNow = 999999999999999ULL; /*some very big number*/
        STRICT_EXPECTED_CALL(mocks, tickcounter_get_current_ms(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .CopyOutArgumentBuffer(2, &timeIsNow, sizeof(timeIsNow));
    }

    ///act
    IoTHubClient_LL_DoWork(handle);
    IoTHubClient_LL_DoWork(handle);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubClient_LL_Destroy(handle);
}

/*Tests_SRS_IOTHUBCLIENT_LL_02_041: [ If more than value miliseconds have passed since the call to IoTHubClient_LL_SendEventAsync then the message callback shall be called with a status code of IOTHUB_CLIENT_CONFIRMATION_TIMEOUT. ]*/
TEST_FUNCTION(IoTHubClient_LL_SetOption_messageTimeout_when_tickcounter_fails_in_do_work_no_timeout_callbacks_are_called) /*test wants to see that message that did not timeout yet do not have their callbacks called*/
{
    ///arrange
    CIoTHubClientLLMocks mocks;
    IOTHUB_CLIENT_LL_HANDLE handle = IoTHubClient_LL_Create(&TEST_CONFIG);
    uint64_t one = 1;
    (void)IoTHubClient_LL_SetOption(handle, "messageTimeout", &one);

    /*send 2 messages that will expire at 12 and 13, both of these messages are send at time=10*/
    /*because sending messages stamps the message's timeout, the call to tickcounter_get_current_ms needs to be here, so the test can says
    "the message has been received at time=10*/
    uint64_t ten = 10;
    STRICT_EXPECTED_CALL(mocks, tickcounter_get_current_ms(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .CopyOutArgumentBuffer(2, &ten, sizeof(ten));
    (void)IoTHubClient_LL_SendEventAsync(handle, TEST_DEVICEMESSAGE_HANDLE, eventConfirmationCallback, (void*)TEST_DEVICEMESSAGE_HANDLE);

    mocks.ResetAllCalls();

    /*we don't care what happens in the Transport, so let's ignore all those calls*/
    EXPECTED_CALL(mocks, FAKE_IoTHubTransport_DoWork(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreAllCalls();

    {/*this scope happen in the _DoWork call*/
        uint64_t timeIsNow = 12; /*12 > 10 (receive time) + 1 (timeout) => timeout!!! (well - normally - but here the code doesn't call any callbacks because time cannot be obtained*/
        STRICT_EXPECTED_CALL(mocks, tickcounter_get_current_ms(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(1)
            .CopyOutArgumentBuffer(2, &timeIsNow, sizeof(timeIsNow))
            .SetFailReturn(__LINE__);
    }

    ///act
    IoTHubClient_LL_DoWork(handle);

    ///assert
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubClient_LL_Destroy(handle);
}

#ifndef DONT_USE_UPLOADTOBLOB
/*Tests_SRS_IOTHUBCLIENT_LL_02_061: [ If iotHubClientHandle is NULL then IoTHubClient_LL_UploadToBlob shall fail and return IOTHUB_CLIENT_INVALID_ARG. ]*/
TEST_FUNCTION(IoTHubClient_LL_UploadToBlob_with_NULL_handle_fails)
{
    ///arrange

    ///act
    IOTHUB_CLIENT_RESULT result = IoTHubClient_LL_UploadToBlob(NULL, "irrelevantFileName", (const unsigned char*)"a", 1);

    ///assert
    ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_INVALID_ARG, result);

    ///cleanup
}
#endif

#ifndef DONT_USE_UPLOADTOBLOB
/*Tests_SRS_IOTHUBCLIENT_LL_02_062: [ If destinationFileName is NULL then IoTHubClient_LL_UploadToBlob shall fail and return IOTHUB_CLIENT_INVALID_ARG. ]*/
TEST_FUNCTION(IoTHubClient_LL_UploadToBlob_with_NULL_fileName_fails)
{
    ///arrange
    CIoTHubClientLLMocks mocks;
    IOTHUB_CLIENT_LL_HANDLE h = IoTHubClient_LL_Create(&TEST_CONFIG);
    mocks.ResetAllCalls();

    ///act
    IOTHUB_CLIENT_RESULT result = IoTHubClient_LL_UploadToBlob(h, NULL, (const unsigned char*)"a", 1);

    ///assert
    ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_INVALID_ARG, result);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubClient_LL_Destroy(h);
}
#endif

#ifndef DONT_USE_UPLOADTOBLOB
/*Tests_SRS_IOTHUBCLIENT_LL_02_063: [ If source is NULL and size is greater than 0 then IoTHubClient_LL_UploadToBlob shall fail and return IOTHUB_CLIENT_INVALID_ARG. ]*/
TEST_FUNCTION(IoTHubClient_LL_UploadToBlob_with_NULL_source_and_size_greater_than_0_fails)
{
    ///arrange
    CIoTHubClientLLMocks mocks;
    IOTHUB_CLIENT_LL_HANDLE h = IoTHubClient_LL_Create(&TEST_CONFIG);
    mocks.ResetAllCalls();

    ///act
    IOTHUB_CLIENT_RESULT result = IoTHubClient_LL_UploadToBlob(h, "someFileName.txt",NULL, 1);

    ///assert
    ASSERT_ARE_EQUAL(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_INVALID_ARG, result);
    mocks.AssertActualAndExpectedCalls();

    ///cleanup
    IoTHubClient_LL_Destroy(h);
}
#endif 

END_TEST_SUITE(iothubclient_ll_ut)

