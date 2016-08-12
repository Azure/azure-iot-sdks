// Copyright (c) Microsoft. All rights reserved
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <cstdlib>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "testrunnerswitcher.h"
#include "micromock.h"
#include "micromockcharstararenullterminatedstrings.h"

#include "iothub_client.h"
#include "iothubtransport.h"

#include "azure_c_shared_utility/lock.h"
#include "azure_c_shared_utility/doublylinkedlist.h"
#include "azure_c_shared_utility/vector.h"


#include "azure_c_shared_utility/string_tokenizer.h"
#include "azure_c_shared_utility/strings.h"

#include "azure_c_shared_utility/tickcounter.h"
#include "azure_c_shared_utility/threadapi.h"

#define GBALLOC_H
extern "C" int gballoc_init(void);
extern "C" void gballoc_deinit(void);
extern "C" void* gballoc_malloc(size_t size);
extern "C" void* gballoc_calloc(size_t nmemb, size_t size);
extern "C" void* gballoc_realloc(void* ptr, size_t size);
extern "C" void gballoc_free(void* ptr);


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
#include "vector.c"

};


static MICROMOCK_MUTEX_HANDLE g_testByTest;
static MICROMOCK_GLOBAL_SEMAPHORE_HANDLE g_dllByDll;

//Check ProtocolGateway Configuration.
static bool checkProtocolGatewayHostName;
static bool checkProtocolGatewayIsNull;
static size_t howManyDoWorkCalls = 0;
static size_t doWorkCallCount = 0;
extern "C" const size_t IoTHubTransport_ThreadTerminationOffset;
static THREAD_START_FUNC threadFunc;
static void* threadFuncArg;

#define TEST_DEVICE_ID "theidofTheDevice"
#define TEST_DEVICE_KEY "theKeyoftheDevice"
#define TEST_IOTHUBNAME "theNameoftheIotHub"
#define TEST_IOTHUBSUFFIX "theSuffixoftheIotHubHostname"
#define TEST_AUTHORIZATIONKEY "theAuthorizationKey"
#define TEST_IOTHUB_CLIENT_HANDLE1 (IOTHUB_CLIENT_HANDLE)0xDEAD
#define TEST_IOTHUB_CLIENT_HANDLE2 (IOTHUB_CLIENT_HANDLE)0xDEAF
#define TEST_LOCK_HANDLE (LOCK_HANDLE)0x4443
#define TEST_THREAD_HANDLE (THREAD_HANDLE)0x4442



#define TEST_HOSTNAME_TOKEN "HostName"
#define TEST_HOSTNAME_VALUE "theNameoftheIotHub.theSuffixoftheIotHubHostname"

#define TEST_DEVICEID_TOKEN "DeviceId"
#define TEST_DEVICEKEY_TOKEN "SharedAccessKey"
#define TEST_PROTOCOL_GATEWAY_HOST_NAME_TOKEN "GatewayHostName"

#define TEST_DEVICEMESSAGE_HANDLE (IOTHUB_MESSAGE_HANDLE)0x52
#define TEST_DEVICEMESSAGE_HANDLE_2 (IOTHUB_MESSAGE_HANDLE)0x53
#define TEST_IOTHUB_CLIENT_LL_HANDLE    (IOTHUB_CLIENT_LL_HANDLE)0x4242

#define TEST_STRING_HANDLE (STRING_HANDLE)0x46
static const char* TEST_CHAR = "TestChar";

static const TRANSPORT_PROVIDER* provideFAKE(void);

static const IOTHUB_CLIENT_CONFIG TEST_CONFIG =
{
	provideFAKE,            /* IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol;   */
	TEST_DEVICE_ID,         /* const char* deviceId;                        */
	TEST_DEVICE_KEY,        /* const char* deviceKey;                       */
	NULL,                   /* const char* deviceSasToken                   */
	TEST_IOTHUBNAME,        /* const char* iotHubName;                      */
	TEST_IOTHUBSUFFIX,      /* const char* iotHubSuffix;                    */
};

static const IOTHUB_CLIENT_CONFIG TEST_CONFIG_NULL_protocol =
{
	NULL,                   /* IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol;   */
	TEST_DEVICE_ID,         /* const char* deviceId;                        */
	TEST_DEVICE_KEY,        /* const char* deviceKey;                       */
	NULL,                   /* const char* deviceSasToken                   */
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

static size_t currentmalloc_call;
static size_t whenShallmalloc_fail;
static IOTHUB_CLIENT_STATUS currentIotHubClientStatus;



TYPED_MOCK_CLASS(CIotHubTransportMocks, CGlobalMock)
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

        MOCK_STATIC_METHOD_1(, STRING_HANDLE, FAKE_IoTHubTransport_GetHostname, TRANSPORT_LL_HANDLE, handle);
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
		doWorkCallCount++;
	MOCK_VOID_METHOD_END()

		MOCK_STATIC_METHOD_2(, IOTHUB_CLIENT_RESULT, FAKE_IoTHubTransport_GetSendStatus, TRANSPORT_LL_HANDLE, handle, IOTHUB_CLIENT_STATUS*, iotHubClientStatus)
		*iotHubClientStatus = currentIotHubClientStatus;
	MOCK_METHOD_END(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_OK)

		MOCK_STATIC_METHOD_2(, void, eventConfirmationCallback, IOTHUB_CLIENT_CONFIRMATION_RESULT, result2, void*, userContextCallback)
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


	/* Version Mocks */
	MOCK_STATIC_METHOD_0(, const char*, IoTHubClient_GetVersionString)
		MOCK_METHOD_END(const char*, (const char*) NULL)

		MOCK_STATIC_METHOD_0(, TICK_COUNTER_HANDLE, tickcounter_create);
	TICK_COUNTER_HANDLE result2 = (TICK_COUNTER_HANDLE )BASEIMPLEMENTATION::gballoc_malloc(1);
	MOCK_METHOD_END(TICK_COUNTER_HANDLE, result2)

		MOCK_STATIC_METHOD_1(, void, tickcounter_destroy, TICK_COUNTER_HANDLE, tick_counter);
	BASEIMPLEMENTATION::gballoc_free(tick_counter);
	MOCK_VOID_METHOD_END()

		MOCK_STATIC_METHOD_2(, int, tickcounter_get_current_ms, TICK_COUNTER_HANDLE, tick_counter, uint64_t*, current_ms);
	MOCK_METHOD_END(int, 0)

		// vector.h
		MOCK_STATIC_METHOD_1(, VECTOR_HANDLE, VECTOR_create, size_t, elementSize)
		VECTOR_HANDLE result2;
	result2 = BASEIMPLEMENTATION::VECTOR_create(elementSize);
	MOCK_METHOD_END(VECTOR_HANDLE, result2)

		MOCK_STATIC_METHOD_1(, void, VECTOR_destroy, VECTOR_HANDLE, vector)
		BASEIMPLEMENTATION::VECTOR_destroy(vector);
	MOCK_VOID_METHOD_END()

		MOCK_STATIC_METHOD_3(, int, VECTOR_push_back, VECTOR_HANDLE, vector, const void*, elements, size_t, numElements)
		int result2;
	result2 = BASEIMPLEMENTATION::VECTOR_push_back(vector, elements, numElements);
	MOCK_METHOD_END(int, result2)

		MOCK_STATIC_METHOD_3(, void, VECTOR_erase, VECTOR_HANDLE, vector, void*, elements, size_t, numElements)
		BASEIMPLEMENTATION::VECTOR_erase(vector, elements, numElements);
	MOCK_VOID_METHOD_END()

		MOCK_STATIC_METHOD_2(, void*, VECTOR_element, VECTOR_HANDLE, vector, size_t, index)
		void* result2 = BASEIMPLEMENTATION::VECTOR_element(vector, index);
	MOCK_METHOD_END(void*, result2)

		MOCK_STATIC_METHOD_1(, void*, VECTOR_front, VECTOR_HANDLE, vector)
		void* result2 = BASEIMPLEMENTATION::VECTOR_front(vector);
	MOCK_METHOD_END(void*, result2)

		MOCK_STATIC_METHOD_1(, void*, VECTOR_back, const VECTOR_HANDLE, vector)
		void* result2 = BASEIMPLEMENTATION::VECTOR_back(vector);
	MOCK_METHOD_END(void*, result2)

		MOCK_STATIC_METHOD_3(, void*, VECTOR_find_if, VECTOR_HANDLE, vector, PREDICATE_FUNCTION, pred, const void*, value)
		void* result2;
	result2 = BASEIMPLEMENTATION::VECTOR_find_if(vector, pred, value);
	MOCK_METHOD_END(void*, result2)

		MOCK_STATIC_METHOD_1(, size_t, VECTOR_size, VECTOR_HANDLE, vector)
		size_t result2 = BASEIMPLEMENTATION::VECTOR_size(vector);
	MOCK_METHOD_END(size_t, result2)


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
			* (sig_atomic_t*)(((char*)threadFuncArg) + IoTHubTransport_ThreadTerminationOffset) = 1; /*tell the thread to stop*/
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

};

DECLARE_GLOBAL_MOCK_METHOD_1(CIotHubTransportMocks, , void, DList_InitializeListHead, PDLIST_ENTRY, listHead);
DECLARE_GLOBAL_MOCK_METHOD_1(CIotHubTransportMocks, , int, DList_IsListEmpty, PDLIST_ENTRY, listHead);
DECLARE_GLOBAL_MOCK_METHOD_2(CIotHubTransportMocks, , void, DList_InsertTailList, PDLIST_ENTRY, listHead, PDLIST_ENTRY, listEntry);
DECLARE_GLOBAL_MOCK_METHOD_2(CIotHubTransportMocks, , void, DList_AppendTailList, PDLIST_ENTRY, listHead, PDLIST_ENTRY, ListToAppend);
DECLARE_GLOBAL_MOCK_METHOD_1(CIotHubTransportMocks, , int, DList_RemoveEntryList, PDLIST_ENTRY, listEntry);
DECLARE_GLOBAL_MOCK_METHOD_1(CIotHubTransportMocks, , PDLIST_ENTRY, DList_RemoveHeadList, PDLIST_ENTRY, listHead);

DECLARE_GLOBAL_MOCK_METHOD_1(CIotHubTransportMocks, , void*, gballoc_malloc, size_t, size);
DECLARE_GLOBAL_MOCK_METHOD_2(CIotHubTransportMocks, , void*, gballoc_realloc, void*, ptr, size_t, size);
DECLARE_GLOBAL_MOCK_METHOD_1(CIotHubTransportMocks, , void, gballoc_free, void*, ptr)

DECLARE_GLOBAL_MOCK_METHOD_1(CIotHubTransportMocks, , STRING_HANDLE, FAKE_IoTHubTransport_GetHostname, TRANSPORT_LL_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_3(CIotHubTransportMocks, , IOTHUB_CLIENT_RESULT, FAKE_IoTHubTransport_SetOption, TRANSPORT_LL_HANDLE, handle, const char*, optionName, const void*, value);
DECLARE_GLOBAL_MOCK_METHOD_1(CIotHubTransportMocks, , TRANSPORT_LL_HANDLE, FAKE_IoTHubTransport_Create, const IOTHUBTRANSPORT_CONFIG*, config);
DECLARE_GLOBAL_MOCK_METHOD_1(CIotHubTransportMocks, , void, FAKE_IoTHubTransport_Destroy, TRANSPORT_LL_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_4(CIotHubTransportMocks, , IOTHUB_DEVICE_HANDLE, FAKE_IoTHubTransport_Register, TRANSPORT_LL_HANDLE, handle, const IOTHUB_DEVICE_CONFIG*, device, IOTHUB_CLIENT_LL_HANDLE, iotHubClientHandle, PDLIST_ENTRY, waitingToSend);
DECLARE_GLOBAL_MOCK_METHOD_1(CIotHubTransportMocks, , void, FAKE_IoTHubTransport_Unregister, IOTHUB_DEVICE_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_1(CIotHubTransportMocks, , int, FAKE_IoTHubTransport_Subscribe, TRANSPORT_LL_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_1(CIotHubTransportMocks, , void, FAKE_IoTHubTransport_Unsubscribe, TRANSPORT_LL_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_2(CIotHubTransportMocks, , void, FAKE_IoTHubTransport_DoWork, TRANSPORT_LL_HANDLE, handle, IOTHUB_CLIENT_LL_HANDLE, iotHubClientHandle);
DECLARE_GLOBAL_MOCK_METHOD_2(CIotHubTransportMocks, , IOTHUB_CLIENT_RESULT, FAKE_IoTHubTransport_GetSendStatus, TRANSPORT_LL_HANDLE, handle, IOTHUB_CLIENT_STATUS*, iotHubClientStatus);

DECLARE_GLOBAL_MOCK_METHOD_2(CIotHubTransportMocks, , void, eventConfirmationCallback, IOTHUB_CLIENT_CONFIRMATION_RESULT, result2, void*, userContextCallback);

DECLARE_GLOBAL_MOCK_METHOD_1(CIotHubTransportMocks, , time_t, get_time, time_t*, t);

DECLARE_GLOBAL_MOCK_METHOD_1(CIotHubTransportMocks, , STRING_HANDLE, STRING_construct, const char*, psz);
DECLARE_GLOBAL_MOCK_METHOD_1(CIotHubTransportMocks, , STRING_HANDLE, STRING_clone, STRING_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_2(CIotHubTransportMocks, , int, STRING_concat, STRING_HANDLE, handle, const char*, s2);
DECLARE_GLOBAL_MOCK_METHOD_2(CIotHubTransportMocks, , int, STRING_concat_with_STRING, STRING_HANDLE, s1, STRING_HANDLE, s2);
DECLARE_GLOBAL_MOCK_METHOD_1(CIotHubTransportMocks, , void, STRING_delete, STRING_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_1(CIotHubTransportMocks, , const char*, STRING_c_str, STRING_HANDLE, s);
DECLARE_GLOBAL_MOCK_METHOD_0(CIotHubTransportMocks, , STRING_HANDLE, STRING_new);

DECLARE_GLOBAL_MOCK_METHOD_0(CIotHubTransportMocks, , const char*, IoTHubClient_GetVersionString);

DECLARE_GLOBAL_MOCK_METHOD_0(CIotHubTransportMocks, ,TICK_COUNTER_HANDLE, tickcounter_create);
DECLARE_GLOBAL_MOCK_METHOD_1(CIotHubTransportMocks, , void, tickcounter_destroy, TICK_COUNTER_HANDLE, tick_counter);
DECLARE_GLOBAL_MOCK_METHOD_2(CIotHubTransportMocks, , int, tickcounter_get_current_ms, TICK_COUNTER_HANDLE, tick_counter, uint64_t*, current_ms);

//vector
DECLARE_GLOBAL_MOCK_METHOD_1(CIotHubTransportMocks, , VECTOR_HANDLE, VECTOR_create, size_t, elementSize);
DECLARE_GLOBAL_MOCK_METHOD_1(CIotHubTransportMocks, , void, VECTOR_destroy, VECTOR_HANDLE, vector);
DECLARE_GLOBAL_MOCK_METHOD_3(CIotHubTransportMocks, , int, VECTOR_push_back, VECTOR_HANDLE, vector, const void*, elements, size_t, numElements);
DECLARE_GLOBAL_MOCK_METHOD_3(CIotHubTransportMocks, , void, VECTOR_erase, VECTOR_HANDLE, vector, void*, elements, size_t, numElements);
DECLARE_GLOBAL_MOCK_METHOD_2(CIotHubTransportMocks, , void*, VECTOR_element, VECTOR_HANDLE, vector, size_t, index);
DECLARE_GLOBAL_MOCK_METHOD_1(CIotHubTransportMocks, , void*, VECTOR_front, VECTOR_HANDLE, vector);
DECLARE_GLOBAL_MOCK_METHOD_1(CIotHubTransportMocks, , void*, VECTOR_back, VECTOR_HANDLE, vector);
DECLARE_GLOBAL_MOCK_METHOD_3(CIotHubTransportMocks, , void*, VECTOR_find_if, VECTOR_HANDLE, vector, PREDICATE_FUNCTION, pred, const void*, value);
DECLARE_GLOBAL_MOCK_METHOD_1(CIotHubTransportMocks, , size_t, VECTOR_size, VECTOR_HANDLE, vector);

DECLARE_GLOBAL_MOCK_METHOD_3(CIotHubTransportMocks, , THREADAPI_RESULT, ThreadAPI_Create, THREAD_HANDLE*, threadHandle, THREAD_START_FUNC, func, void*, arg);
DECLARE_GLOBAL_MOCK_METHOD_2(CIotHubTransportMocks, , THREADAPI_RESULT, ThreadAPI_Join, THREAD_HANDLE, threadHandle, int*, res);
DECLARE_GLOBAL_MOCK_METHOD_1(CIotHubTransportMocks, , void, ThreadAPI_Exit, int, res);
DECLARE_GLOBAL_MOCK_METHOD_1(CIotHubTransportMocks, , void, ThreadAPI_Sleep, unsigned int, milliseconds);


DECLARE_GLOBAL_MOCK_METHOD_0(CIotHubTransportMocks, , LOCK_HANDLE, Lock_Init);
DECLARE_GLOBAL_MOCK_METHOD_1(CIotHubTransportMocks, , LOCK_RESULT, Lock, LOCK_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_1(CIotHubTransportMocks, , LOCK_RESULT, Unlock, LOCK_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_1(CIotHubTransportMocks, , LOCK_RESULT, Lock_Deinit, LOCK_HANDLE, handle);

static TRANSPORT_PROVIDER FAKE_transport_provider =
{
    FAKE_IoTHubTransport_GetHostname,   /*pfIoTHubTransport_GetHostname IoTHubTransport_GetHostname;   */
	FAKE_IoTHubTransport_SetOption,     /*pfIoTHubTransport_SetOption IoTHubTransport_SetOption;       */
	FAKE_IoTHubTransport_Create,        /*pfIoTHubTransport_Create IoTHubTransport_Create;              */
	FAKE_IoTHubTransport_Destroy,       /*pfIoTHubTransport_Destroy IoTHubTransport_Destroy;            */
	FAKE_IoTHubTransport_Register,		/* pfIotHubTransport_Register IoTHubTransport_Register;         */
	FAKE_IoTHubTransport_Unregister,    /* pfIotHubTransport_Unregister IoTHubTransport_Unegister;      */
	FAKE_IoTHubTransport_Subscribe,     /*pfIoTHubTransport_Subscribe IoTHubTransport_Subscribe;        */
	FAKE_IoTHubTransport_Unsubscribe,   /*pfIoTHubTransport_Unsubscribe IoTHubTransport_Unsubscribe;    */
	FAKE_IoTHubTransport_DoWork,        /*pfIoTHubTransport_DoWork IoTHubTransport_DoWork;              */
	FAKE_IoTHubTransport_GetSendStatus  /*pfIoTHubTransport_GetSendStatus IoTHubTransport_GetSendStatus; */
};

static const TRANSPORT_PROVIDER* provideFAKE(void)
{
	return &FAKE_transport_provider; /*by convention... */
}

BEGIN_TEST_SUITE(iothubtransport_ut)

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

/*Tests_SRS_IOTHUBTRANSPORT_17_001: [ IoTHubTransport_Create shall return a non-NULL handle on success.] */
/*Tests_SRS_IOTHUBTRANSPORT_17_005: [ IoTHubTransport_Create shall create the lower layer transport by calling the protocol's IoTHubTransport_Create function. ]*/
/*Tests_SRS_IOTHUBTRANSPORT_17_007: [ IoTHubTransport_Create shall create the transport lock by Calling Lock_Init. */
/*Tests_SRS_IOTHUBTRANSPORT_17_038: [ IoTHubTransport_Create shall call VECTOR_Create to make a list of IOTHUB_CLIENT_HANDLE using this transport. ]*/
//Tests_SRS_IOTHUBTRANSPORT_17_032: [ IoTHubTransport_Create shall allocate memory for the transport data. ]
TEST_FUNCTION(IoTHubTransport_Create_success_returns_non_null)
{
	CIotHubTransportMocks mocks;
	///arrange
	STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
		.IgnoreArgument(1);
	STRICT_EXPECTED_CALL(mocks, FAKE_IoTHubTransport_Create(IGNORED_PTR_ARG))
		.IgnoreArgument(1);
	STRICT_EXPECTED_CALL(mocks, Lock_Init());
	STRICT_EXPECTED_CALL(mocks, VECTOR_create(sizeof(IOTHUB_CLIENT_HANDLE)));

	///act
	auto result = IoTHubTransport_Create(TEST_CONFIG.protocol, TEST_CONFIG.iotHubName, TEST_CONFIG.iotHubSuffix);

	///assert

	ASSERT_IS_NOT_NULL(result);
	mocks.AssertActualAndExpectedCalls();

	///cleanup
	IoTHubTransport_Destroy(result);

}

//Tests_SRS_IOTHUBTRANSPORT_17_009: [ IoTHubTransport_Create shall clean up any resources it creates if the function does not succeed. ]
//Tests_SRS_IOTHUBTRANSPORT_17_039: [ If the Vector creation fails, IoTHubTransport_Create shall return NULL. ]
TEST_FUNCTION(IoTHubTransport_Create_vector_create_fails_returns_null)
{
	CIotHubTransportMocks mocks;
	///arrange
	STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
		.IgnoreArgument(1);
	STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
		.IgnoreArgument(1);
	STRICT_EXPECTED_CALL(mocks, FAKE_IoTHubTransport_Create(IGNORED_PTR_ARG))
		.IgnoreArgument(1);
	STRICT_EXPECTED_CALL(mocks, FAKE_IoTHubTransport_Destroy(IGNORED_PTR_ARG))
		.IgnoreArgument(1);
	STRICT_EXPECTED_CALL(mocks, Lock_Init());
	STRICT_EXPECTED_CALL(mocks, Lock_Deinit(IGNORED_PTR_ARG))
		.IgnoreArgument(1);
	STRICT_EXPECTED_CALL(mocks, VECTOR_create(sizeof(IOTHUB_CLIENT_HANDLE)))
		.SetFailReturn((VECTOR_HANDLE)NULL);

	///act
	auto result = IoTHubTransport_Create(TEST_CONFIG.protocol, TEST_CONFIG.iotHubName, TEST_CONFIG.iotHubSuffix);

	///assert

	ASSERT_IS_NULL(result);

	///cleanup

}

//Tests_SRS_IOTHUBTRANSPORT_17_008: [ If the lock creation fails, IoTHubTransport_Create shall return NULL. ]
TEST_FUNCTION(IoTHubTransport_Create_lock_init_fails_returns_null)
{
	CIotHubTransportMocks mocks;
	///arrange
	STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
		.IgnoreArgument(1);
	STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
		.IgnoreArgument(1);
	STRICT_EXPECTED_CALL(mocks, FAKE_IoTHubTransport_Create(IGNORED_PTR_ARG))
		.IgnoreArgument(1);
	STRICT_EXPECTED_CALL(mocks, FAKE_IoTHubTransport_Destroy(IGNORED_PTR_ARG))
		.IgnoreArgument(1);
	STRICT_EXPECTED_CALL(mocks, Lock_Init())
		.SetFailReturn((LOCK_HANDLE)NULL);

	///act
	auto result = IoTHubTransport_Create(TEST_CONFIG.protocol, TEST_CONFIG.iotHubName, TEST_CONFIG.iotHubSuffix);

	///assert

	ASSERT_IS_NULL(result);

	///cleanup

}

//Tests_SRS_IOTHUBTRANSPORT_17_006: [ If the creation of the transport fails, IoTHubTransport_Create shall return NULL. ]
TEST_FUNCTION(IoTHubTransport_Create_transport_ll_create_fails_returns_null)
{
	CIotHubTransportMocks mocks;
	///arrange
	STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
		.IgnoreArgument(1);
	STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
		.IgnoreArgument(1);
	STRICT_EXPECTED_CALL(mocks, FAKE_IoTHubTransport_Create(IGNORED_PTR_ARG))
		.IgnoreArgument(1)
		.SetFailReturn((void_ptr)NULL);

	///act
	auto result = IoTHubTransport_Create(TEST_CONFIG.protocol, TEST_CONFIG.iotHubName, TEST_CONFIG.iotHubSuffix);

	///assert

	ASSERT_IS_NULL(result);

	///cleanup

}

//Tests_SRS_IOTHUBTRANSPORT_17_040: [ If memory allocation fails, IoTHubTransport_Create shall return NULL. ]
TEST_FUNCTION(IoTHubTransport_Create_alloc_fails_returns_null)
{
	CIotHubTransportMocks mocks;
	///arrange
	STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
		.IgnoreArgument(1)
		.SetFailReturn((void_ptr)NULL);

	///act
	auto result = IoTHubTransport_Create(TEST_CONFIG.protocol, TEST_CONFIG.iotHubName, TEST_CONFIG.iotHubSuffix);

	///assert

	ASSERT_IS_NULL(result);

	///cleanup

}

//Tests_SRS_IOTHUBTRANSPORT_17_002: [ If protocol is NULL, this function shall return NULL. ]
TEST_FUNCTION(IoTHubTransport_Create_alloc_null_protocol_returns_null)
{
	CIotHubTransportMocks mocks;
	///arrange

	///act
	auto result = IoTHubTransport_Create(NULL, TEST_CONFIG.iotHubName, TEST_CONFIG.iotHubSuffix);

	///assert

	ASSERT_IS_NULL(result);

	///cleanup

}

//Tests_SRS_IOTHUBTRANSPORT_17_003: [ If iotHubName is NULL, this function shall return NULL. ]
TEST_FUNCTION(IoTHubTransport_Create_hubname_null_returns_null)
{
	CIotHubTransportMocks mocks;
	///arrange

	///act
	auto result = IoTHubTransport_Create(TEST_CONFIG.protocol, NULL, TEST_CONFIG.iotHubSuffix);

	///assert

	ASSERT_IS_NULL(result);

	///cleanup

}

//Tests_SRS_IOTHUBTRANSPORT_17_004: [ If iotHubSuffix is NULL, this function shall return NULL. ]
TEST_FUNCTION(IoTHubTransport_Create_hubsufix_null_returns_null)
{
	CIotHubTransportMocks mocks;
	///arrange

	///act
	auto result = IoTHubTransport_Create(TEST_CONFIG.protocol, TEST_CONFIG.iotHubName, NULL);

	///assert

	ASSERT_IS_NULL(result);

	///cleanup

}

//Tests_SRS_IOTHUBTRANSPORT_17_010: [ IoTHubTransport_Destroy shall free all resources. ]
TEST_FUNCTION(IoTHubTransport_Destroy_success)
{
	CIotHubTransportMocks mocks;
	///arrange
	auto transportHandle = IoTHubTransport_Create(TEST_CONFIG.protocol, TEST_CONFIG.iotHubName, TEST_CONFIG.iotHubSuffix);
	mocks.ResetAllCalls();

	STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE))
		.IgnoreArgument(1);
	STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE))
		.IgnoreArgument(1);
	STRICT_EXPECTED_CALL(mocks, Lock_Deinit(IGNORED_PTR_ARG))
		.IgnoreArgument(1);
	STRICT_EXPECTED_CALL(mocks, FAKE_IoTHubTransport_Destroy(IGNORED_PTR_ARG))
		.IgnoreArgument(1);
	STRICT_EXPECTED_CALL(mocks, VECTOR_destroy(IGNORED_PTR_ARG))
		.IgnoreArgument(1);
	STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
		.IgnoreArgument(1);

	///act

	IoTHubTransport_Destroy(transportHandle);

	///assert

	///cleanup
}

//Tests_SRS_IOTHUBTRANSPORT_17_010: [ IoTHubTransport_Destroy shall free all resources. ]
TEST_FUNCTION(IoTHubTransport_Destroy_success_thread_join_fails)
{
	CIotHubTransportMocks mocks;
	///arrange
	auto transportHandle = IoTHubTransport_Create(TEST_CONFIG.protocol, TEST_CONFIG.iotHubName, TEST_CONFIG.iotHubSuffix);
	(void)IoTHubTransport_StartWorkerThread(transportHandle, TEST_IOTHUB_CLIENT_HANDLE1);
	mocks.ResetAllCalls();

	STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE))
		.IgnoreArgument(1);
	STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE))
		.IgnoreArgument(1);
	STRICT_EXPECTED_CALL(mocks, ThreadAPI_Join(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
		.IgnoreAllArguments()
		.SetFailReturn(THREADAPI_ERROR);
	STRICT_EXPECTED_CALL(mocks, Lock_Deinit(IGNORED_PTR_ARG))
		.IgnoreArgument(1);
	STRICT_EXPECTED_CALL(mocks, FAKE_IoTHubTransport_Destroy(IGNORED_PTR_ARG))
		.IgnoreArgument(1);
	STRICT_EXPECTED_CALL(mocks, VECTOR_destroy(IGNORED_PTR_ARG))
		.IgnoreArgument(1);
	STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
		.IgnoreArgument(1);

	///act

	IoTHubTransport_Destroy(transportHandle);

	///assert

	///cleanup
}

//Tests_SRS_IOTHUBTRANSPORT_17_033: [ IoTHubTransport_Destroy shall lock the transport lock. ]
TEST_FUNCTION(IoTHubTransport_Destroy_success_lock_fails)
{
	CIotHubTransportMocks mocks;
	///arrange
	auto transportHandle = IoTHubTransport_Create(TEST_CONFIG.protocol, TEST_CONFIG.iotHubName, TEST_CONFIG.iotHubSuffix);
	mocks.ResetAllCalls();

	STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE))
		.IgnoreArgument(1)
		.SetFailReturn(LOCK_ERROR);
	STRICT_EXPECTED_CALL(mocks, Lock_Deinit(IGNORED_PTR_ARG))
		.IgnoreArgument(1);
	STRICT_EXPECTED_CALL(mocks, FAKE_IoTHubTransport_Destroy(IGNORED_PTR_ARG))
		.IgnoreArgument(1);
	STRICT_EXPECTED_CALL(mocks, VECTOR_destroy(IGNORED_PTR_ARG))
		.IgnoreArgument(1);
	STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
		.IgnoreArgument(1);

	///act

	IoTHubTransport_Destroy(transportHandle);

	///assert

	///cleanup
}

//Tests_SRS_IOTHUBTRANSPORT_17_011: [ IoTHubTransport_Destroy shall do nothing if transportHlHandle is NULL. ]
TEST_FUNCTION(IoTHubTransport_Destroy_null_handle_does_nothing)
{
	CIotHubTransportMocks mocks;
	///arrange

	///act

	IoTHubTransport_Destroy(NULL);

	///assert

	///cleanup
}

//Tests_SRS_IOTHUBTRANSPORT_17_012: [ IoTHubTransport_GetLock shall return a handle to the transport lock. ]
TEST_FUNCTION(IoTHubTransport_GetLock_success)
{
	CIotHubTransportMocks mocks;
	///arrange
	auto transportHandle = IoTHubTransport_Create(TEST_CONFIG.protocol, TEST_CONFIG.iotHubName, TEST_CONFIG.iotHubSuffix);
	mocks.ResetAllCalls();
	///act

	LOCK_HANDLE lock = IoTHubTransport_GetLock(transportHandle);

	///assert
	ASSERT_IS_NOT_NULL(lock);
	mocks.AssertActualAndExpectedCalls();

	///cleanup
	IoTHubTransport_Destroy(transportHandle);
}

//Tests_SRS_IOTHUBTRANSPORT_17_013: [ If transportHlHandle is NULL, IoTHubTransport_GetLock shall return NULL. ]
TEST_FUNCTION(IoTHubTransport_GetLock_fail)
{
	CIotHubTransportMocks mocks;
	///arrange
	auto transportHandle = IoTHubTransport_Create(TEST_CONFIG.protocol, TEST_CONFIG.iotHubName, TEST_CONFIG.iotHubSuffix);
	mocks.ResetAllCalls();
	///act

	LOCK_HANDLE lock = IoTHubTransport_GetLock(NULL);

	///assert
	ASSERT_IS_NULL(lock);
	mocks.AssertActualAndExpectedCalls();

	///cleanup
	IoTHubTransport_Destroy(transportHandle);
}

//Tests_SRS_IOTHUBTRANSPORT_17_014: [ IoTHubTransport_GetLLTransport shall return a handle to the lower layer transport. ]
TEST_FUNCTION(IoTHubTransport_GetLLTransport_success)
{
	CIotHubTransportMocks mocks;
	///arrange
	auto transportHandle = IoTHubTransport_Create(TEST_CONFIG.protocol, TEST_CONFIG.iotHubName, TEST_CONFIG.iotHubSuffix);
	mocks.ResetAllCalls();
	///act

	TRANSPORT_LL_HANDLE llTransport = IoTHubTransport_GetLLTransport(transportHandle);

	///assert
	ASSERT_IS_NOT_NULL(llTransport);
	mocks.AssertActualAndExpectedCalls();

	///cleanup
	IoTHubTransport_Destroy(transportHandle);
}

//Tests_SRS_IOTHUBTRANSPORT_17_015: [ If transportHlHandle is NULL, IoTHubTransport_GetLLTransport shall return NULL. ]
TEST_FUNCTION(IoTHubTransport_GetLLTransport_fail)
{
	CIotHubTransportMocks mocks;
	///arrange
	auto transportHandle = IoTHubTransport_Create(TEST_CONFIG.protocol, TEST_CONFIG.iotHubName, TEST_CONFIG.iotHubSuffix);
	mocks.ResetAllCalls();
	///act

	TRANSPORT_LL_HANDLE llTransport = IoTHubTransport_GetLLTransport(NULL);

	///assert
	ASSERT_IS_NULL(llTransport);
	mocks.AssertActualAndExpectedCalls();

	///cleanup
	IoTHubTransport_Destroy(transportHandle);
}

//Tests_SRS_IOTHUBTRANSPORT_17_018: [ If the worker thread does not exist, IoTHubTransport_StartWorkerThread shall start the thread using ThreadAPI_Create. ]
//Tests_SRS_IOTHUBTRANSPORT_17_021: [ If handle is not found, then clientHandle shall be added to the list. ]
//Tests_SRS_IOTHUBTRANSPORT_17_022: [ Upon success, IoTHubTransport_StartWorkerThread shall return IOTHUB_CLIENT_OK.]
TEST_FUNCTION(IoTHubTransport_StartWorkerThread_success)
{
	CIotHubTransportMocks mocks;
	///arrange

	auto transportHandle = IoTHubTransport_Create(TEST_CONFIG.protocol, TEST_CONFIG.iotHubName, TEST_CONFIG.iotHubSuffix);
	mocks.ResetAllCalls();

	STRICT_EXPECTED_CALL(mocks, ThreadAPI_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG, transportHandle))
		.IgnoreArgument(1)
		.IgnoreArgument(2);
	STRICT_EXPECTED_CALL(mocks, VECTOR_size(IGNORED_PTR_ARG))
		.IgnoreArgument(1);
	STRICT_EXPECTED_CALL(mocks, VECTOR_push_back(IGNORED_PTR_ARG, IGNORED_PTR_ARG, 1))
		.IgnoreArgument(1)
		.IgnoreArgument(2);
	///act

	IOTHUB_CLIENT_RESULT result = IoTHubTransport_StartWorkerThread(transportHandle, TEST_IOTHUB_CLIENT_HANDLE1);

	///assert
	ASSERT_ARE_EQUAL(int, (int)result, (int)IOTHUB_CLIENT_OK);
	mocks.AssertActualAndExpectedCalls();

	///cleanup
	IoTHubTransport_Destroy(transportHandle);
}

TEST_FUNCTION(IoTHubTransport_StartWorkerThread_same_client_twice_success)
{
	CIotHubTransportMocks mocks;
	///arrange

	auto transportHandle = IoTHubTransport_Create(TEST_CONFIG.protocol, TEST_CONFIG.iotHubName, TEST_CONFIG.iotHubSuffix);
	IOTHUB_CLIENT_RESULT result = IoTHubTransport_StartWorkerThread(transportHandle, TEST_IOTHUB_CLIENT_HANDLE1);

	mocks.ResetAllCalls();

	STRICT_EXPECTED_CALL(mocks, VECTOR_size(IGNORED_PTR_ARG))
		.IgnoreArgument(1);
	STRICT_EXPECTED_CALL(mocks, VECTOR_find_if(IGNORED_PTR_ARG, IGNORED_PTR_ARG, TEST_IOTHUB_CLIENT_HANDLE1))
		.IgnoreArgument(1)
		.IgnoreArgument(2);

	///act

	result = IoTHubTransport_StartWorkerThread(transportHandle, TEST_IOTHUB_CLIENT_HANDLE1);

	///assert
	ASSERT_ARE_EQUAL(int, (int)result, (int)IOTHUB_CLIENT_OK);
	mocks.AssertActualAndExpectedCalls();

	///cleanup
	IoTHubTransport_Destroy(transportHandle);
}

//Tests_SRS_IOTHUBTRANSPORT_17_020: [ IoTHubTransport_StartWorkerThread shall search for IoTHubClient clientHandle in the list of IoTHubClient handles. ]
//Tests_SRS_IOTHUBTRANSPORT_17_022: [ Upon success, IoTHubTransport_StartWorkerThread shall return IOTHUB_CLIENT_OK.]
TEST_FUNCTION(IoTHubTransport_StartWorkerThread_two_client_success)
{
	CIotHubTransportMocks mocks;
	///arrange

	auto transportHandle = IoTHubTransport_Create(TEST_CONFIG.protocol, TEST_CONFIG.iotHubName, TEST_CONFIG.iotHubSuffix);
	(void)IoTHubTransport_StartWorkerThread(transportHandle, TEST_IOTHUB_CLIENT_HANDLE1);

	mocks.ResetAllCalls();

	STRICT_EXPECTED_CALL(mocks, VECTOR_size(IGNORED_PTR_ARG))
		.IgnoreArgument(1);
	STRICT_EXPECTED_CALL(mocks, VECTOR_find_if(IGNORED_PTR_ARG, IGNORED_PTR_ARG, TEST_IOTHUB_CLIENT_HANDLE2))
		.IgnoreArgument(1)
		.IgnoreArgument(2);
	STRICT_EXPECTED_CALL(mocks, VECTOR_push_back(IGNORED_PTR_ARG, IGNORED_PTR_ARG, 1))
		.IgnoreArgument(1)
		.IgnoreArgument(2);
	///act

	IOTHUB_CLIENT_RESULT result = IoTHubTransport_StartWorkerThread(transportHandle, TEST_IOTHUB_CLIENT_HANDLE2);

	///assert
	ASSERT_ARE_EQUAL(int, (int)result, (int)IOTHUB_CLIENT_OK);
	mocks.AssertActualAndExpectedCalls();

	///cleanup
	IoTHubTransport_Destroy(transportHandle);
}

//Tests_SRS_IOTHUBTRANSPORT_17_016: [ If transportHlHandle is NULL, IoTHubTransport_StartWorkerThread shall return IOTHUB_CLIENT_INVALID_ARG. ]
TEST_FUNCTION(IoTHubTransport_StartWorkerThread_null_transport_returns_bad_arg)
{
	CIotHubTransportMocks mocks;
	///arrange
	///act

	IOTHUB_CLIENT_RESULT result = IoTHubTransport_StartWorkerThread(NULL, TEST_IOTHUB_CLIENT_HANDLE1);

	///assert
	ASSERT_ARE_EQUAL(int, (int)result, (int)IOTHUB_CLIENT_INVALID_ARG);
	mocks.AssertActualAndExpectedCalls();

	///cleanup
}

//Tests_SRS_IOTHUBTRANSPORT_17_017: [ If clientHandle is NULL, IoTHubTransport_StartWorkerThread shall return IOTHUB_CLIENT_INVALID_ARG. ]
TEST_FUNCTION(IoTHubTransport_StartWorkerThread_null_client_returns_bad_arg)
{
	CIotHubTransportMocks mocks;
	///arrange

	auto transportHandle = IoTHubTransport_Create(TEST_CONFIG.protocol, TEST_CONFIG.iotHubName, TEST_CONFIG.iotHubSuffix);
	mocks.ResetAllCalls();

	///act

	IOTHUB_CLIENT_RESULT result = IoTHubTransport_StartWorkerThread(transportHandle, NULL);

	///assert
	ASSERT_ARE_EQUAL(int, (int)result, (int)IOTHUB_CLIENT_INVALID_ARG);
	mocks.AssertActualAndExpectedCalls();

	///cleanup
	IoTHubTransport_Destroy(transportHandle);
}

//Tests_SRS_IOTHUBTRANSPORT_17_019: [ If thread creation fails, IoTHubTransport_StartWorkerThread shall return IOTHUB_CLIENT_ERROR. ]
TEST_FUNCTION(IoTHubTransport_StartWorkerThread_thread_create_fails_returns_error)
{
	CIotHubTransportMocks mocks;
	///arrange

	auto transportHandle = IoTHubTransport_Create(TEST_CONFIG.protocol, TEST_CONFIG.iotHubName, TEST_CONFIG.iotHubSuffix);
	mocks.ResetAllCalls();

	STRICT_EXPECTED_CALL(mocks, ThreadAPI_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG, transportHandle))
		.IgnoreArgument(1)
		.IgnoreArgument(2)
		.SetFailReturn(THREADAPI_ERROR);
	///act

	IOTHUB_CLIENT_RESULT result = IoTHubTransport_StartWorkerThread(transportHandle, TEST_IOTHUB_CLIENT_HANDLE1);

	///assert
	ASSERT_ARE_EQUAL(int, (int)result, (int)IOTHUB_CLIENT_ERROR);
	mocks.AssertActualAndExpectedCalls();

	///cleanup
	IoTHubTransport_Destroy(transportHandle);
}

//Tests_SRS_IOTHUBTRANSPORT_17_042: [ If Adding to the client list fails, IoTHubTransport_StartWorkerThread shall return IOTHUB_CLIENT_ERROR. ]
TEST_FUNCTION(IoTHubTransport_StartWorkerThread_Vector_push_back_returns_error)
{
	CIotHubTransportMocks mocks;
	///arrange

	auto transportHandle = IoTHubTransport_Create(TEST_CONFIG.protocol, TEST_CONFIG.iotHubName, TEST_CONFIG.iotHubSuffix);
	mocks.ResetAllCalls();

	STRICT_EXPECTED_CALL(mocks, ThreadAPI_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG, transportHandle))
		.IgnoreArgument(1)
		.IgnoreArgument(2);
	STRICT_EXPECTED_CALL(mocks, VECTOR_size(IGNORED_PTR_ARG))
		.IgnoreArgument(1);
	STRICT_EXPECTED_CALL(mocks, VECTOR_push_back(IGNORED_PTR_ARG, IGNORED_PTR_ARG, 1))
		.IgnoreArgument(1)
		.IgnoreArgument(2)
		.SetFailReturn(42);
	///act

	IOTHUB_CLIENT_RESULT result = IoTHubTransport_StartWorkerThread(transportHandle, TEST_IOTHUB_CLIENT_HANDLE1);

	///assert
	ASSERT_ARE_EQUAL(int, (int)result, (int)IOTHUB_CLIENT_ERROR);
	mocks.AssertActualAndExpectedCalls();

	///cleanup
	IoTHubTransport_Destroy(transportHandle);
}


//Tests_SRS_IOTHUBTRANSPORT_17_026: [ IoTHubTransport_SignalEndWorkerThread shall remove clientHandlehandle from handle list. ]
//Tests_SRS_IOTHUBTRANSPORT_17_028: [ The thread shall exit when IoTHubTransport_SignalEndWorkerThread has been called for each clientHandle which invoked IoTHubTransport_StartWorkerThread. ]
//Tests_SRS_IOTHUBTRANSPORT_17_043: [ IoTHubTransport_SignalEndWorkerThread shall signal the worker thread to end. ]
TEST_FUNCTION(IoTHubTransport_SignalEndWorkerThread_success)
{
	CIotHubTransportMocks mocks;
	///arrange

	auto transportHandle = IoTHubTransport_Create(TEST_CONFIG.protocol, TEST_CONFIG.iotHubName, TEST_CONFIG.iotHubSuffix);
    (void)IoTHubTransport_StartWorkerThread(transportHandle, TEST_IOTHUB_CLIENT_HANDLE1);
	mocks.ResetAllCalls();

	STRICT_EXPECTED_CALL(mocks, VECTOR_find_if(IGNORED_PTR_ARG, IGNORED_PTR_ARG, TEST_IOTHUB_CLIENT_HANDLE1))
		.IgnoreArgument(1)
		.IgnoreArgument(2);
	STRICT_EXPECTED_CALL(mocks, VECTOR_erase(IGNORED_PTR_ARG, IGNORED_PTR_ARG, 1))
		.IgnoreArgument(1)
		.IgnoreArgument(2);
	STRICT_EXPECTED_CALL(mocks, VECTOR_size(IGNORED_PTR_ARG))
		.IgnoreArgument(1);

	///act
	auto rv = IoTHubTransport_SignalEndWorkerThread(transportHandle, TEST_IOTHUB_CLIENT_HANDLE1);

	///assert
	ASSERT_IS_TRUE(rv);
	mocks.AssertActualAndExpectedCalls();

	///cleanup
	IoTHubTransport_Destroy(transportHandle);
}

//Tests_SRS_IOTHUBTRANSPORT_17_026: [ IoTHubTransport_SignalEndWorkerThread shall remove clientHandlehandle from handle list. ]
TEST_FUNCTION(IoTHubTransport_SignalEndWorkerThread_2_client_success)
{
	CIotHubTransportMocks mocks;
	///arrange

	auto transportHandle = IoTHubTransport_Create(TEST_CONFIG.protocol, TEST_CONFIG.iotHubName, TEST_CONFIG.iotHubSuffix);
    (void)IoTHubTransport_StartWorkerThread(transportHandle, TEST_IOTHUB_CLIENT_HANDLE1);
    (void)IoTHubTransport_StartWorkerThread(transportHandle, TEST_IOTHUB_CLIENT_HANDLE2);

	mocks.ResetAllCalls();

	STRICT_EXPECTED_CALL(mocks, VECTOR_find_if(IGNORED_PTR_ARG, IGNORED_PTR_ARG, TEST_IOTHUB_CLIENT_HANDLE1))
		.IgnoreArgument(1)
		.IgnoreArgument(2);
	STRICT_EXPECTED_CALL(mocks, VECTOR_erase(IGNORED_PTR_ARG, IGNORED_PTR_ARG, 1))
		.IgnoreArgument(1)
		.IgnoreArgument(2);
	STRICT_EXPECTED_CALL(mocks, VECTOR_size(IGNORED_PTR_ARG))
		.IgnoreArgument(1);

	///act
	auto rv = IoTHubTransport_SignalEndWorkerThread(transportHandle, TEST_IOTHUB_CLIENT_HANDLE1);

	///assert
	ASSERT_IS_FALSE(rv);
	mocks.AssertActualAndExpectedCalls();

	///cleanup
	IoTHubTransport_SignalEndWorkerThread(transportHandle, TEST_IOTHUB_CLIENT_HANDLE2);
	IoTHubTransport_Destroy(transportHandle);
}

//Tests_SRS_IOTHUBTRANSPORT_17_023: [ If transportHlHandle is NULL, IoTHubTransport_SignalEndWorkerThread shall return. ]
TEST_FUNCTION(IoTHubTransport_SignalEndWorkerThread_null_transport_does_nothing)
{
	CIotHubTransportMocks mocks;
	///arrange

	///act
	auto rv = IoTHubTransport_SignalEndWorkerThread(NULL, TEST_IOTHUB_CLIENT_HANDLE1);

	///assert
	ASSERT_IS_FALSE(rv);

	///cleanup
}

//Tests_SRS_IOTHUBTRANSPORT_17_024: [ If clientHandle is NULL, IoTHubTransport_SignalEndWorkerThread shall return. ]
TEST_FUNCTION(IoTHubTransport_SignalEndWorkerThread_null_client_does_nothing)
{
	CIotHubTransportMocks mocks;
	///arrange
	int fake = 0x43;
	///act
	auto rv = IoTHubTransport_SignalEndWorkerThread((TRANSPORT_HANDLE)(&fake), NULL);

	///assert
	ASSERT_IS_FALSE(rv);

	///cleanup
}

//Tests_SRS_IOTHUBTRANSPORT_17_025: [If the worker thread does not exist, then IoTHubTransport_SignalEndWorkerThread shall return.]
TEST_FUNCTION(IoTHubTransport_SignalEndWorkerThread_end_thread_client_not_found)
{
	CIotHubTransportMocks mocks;
	///arrange

	auto transportHandle = IoTHubTransport_Create(TEST_CONFIG.protocol, TEST_CONFIG.iotHubName, TEST_CONFIG.iotHubSuffix);
	mocks.ResetAllCalls();

	STRICT_EXPECTED_CALL(mocks, VECTOR_find_if(IGNORED_PTR_ARG, IGNORED_PTR_ARG, TEST_IOTHUB_CLIENT_HANDLE1))
		.IgnoreArgument(1)
		.IgnoreArgument(2);

	///act
	auto rv = IoTHubTransport_SignalEndWorkerThread(transportHandle, TEST_IOTHUB_CLIENT_HANDLE1);

	///assert
	ASSERT_IS_FALSE(rv);
	mocks.AssertActualAndExpectedCalls();

	///cleanup
	IoTHubTransport_Destroy(transportHandle);
}

TEST_FUNCTION(IoTHubTransport_SignalEndWorkerThread_end_thread_client_not_found2)
{
	CIotHubTransportMocks mocks;
	///arrange

	auto transportHandle = IoTHubTransport_Create(TEST_CONFIG.protocol, TEST_CONFIG.iotHubName, TEST_CONFIG.iotHubSuffix);
    (void)IoTHubTransport_StartWorkerThread(transportHandle, TEST_IOTHUB_CLIENT_HANDLE1);
	mocks.ResetAllCalls();

	STRICT_EXPECTED_CALL(mocks, VECTOR_find_if(IGNORED_PTR_ARG, IGNORED_PTR_ARG, TEST_IOTHUB_CLIENT_HANDLE2))
		.IgnoreArgument(1)
		.IgnoreArgument(2);
	STRICT_EXPECTED_CALL(mocks, VECTOR_size(IGNORED_PTR_ARG))
		.IgnoreArgument(1);

	///act
	IoTHubTransport_SignalEndWorkerThread(transportHandle, TEST_IOTHUB_CLIENT_HANDLE2);

	///assert
	mocks.AssertActualAndExpectedCalls();

	///cleanup
	IoTHubTransport_SignalEndWorkerThread(transportHandle, TEST_IOTHUB_CLIENT_HANDLE1);
	IoTHubTransport_Destroy(transportHandle);
}

//Tests_SRS_IOTHUBTRANSPORT_17_044: [ If transportHlHandle is NULL, IoTHubTransport_JoinWorkerThread shall do nothing. ]
TEST_FUNCTION(IoTHubTransport_JoinWorkerThread_null_transport_just_returns)
{
	CIotHubTransportMocks mocks;
	///arrange

	///act
	IoTHubTransport_JoinWorkerThread(NULL, TEST_IOTHUB_CLIENT_HANDLE1);
	///assert
	///cleanup
}

//Tests_SRS_IOTHUBTRANSPORT_17_045: [ If clientHandle is NULL, IoTHubTransport_JoinWorkerThread shall do nothing. ]
TEST_FUNCTION(IoTHubTransport_JoinWorkerThread_null_client_just_returns)
{
	CIotHubTransportMocks mocks;
	///arrange
	int fake = 0x43;
	///act
	IoTHubTransport_JoinWorkerThread((TRANSPORT_HANDLE)(&fake), NULL);

	///assert
	///cleanup
}

//Tests_SRS_IOTHUBTRANSPORT_17_027: [ The worker thread shall be joined. ]
TEST_FUNCTION(IoTHubTransport_JoinWorkerThread_success)
{
	CIotHubTransportMocks mocks;
	///arrange
	auto transportHandle = IoTHubTransport_Create(TEST_CONFIG.protocol, TEST_CONFIG.iotHubName, TEST_CONFIG.iotHubSuffix);
	(void)IoTHubTransport_StartWorkerThread(transportHandle, TEST_IOTHUB_CLIENT_HANDLE1);
	(void)IoTHubTransport_SignalEndWorkerThread(transportHandle, TEST_IOTHUB_CLIENT_HANDLE1);
	mocks.ResetAllCalls();
	STRICT_EXPECTED_CALL(mocks, ThreadAPI_Join(TEST_THREAD_HANDLE, IGNORED_PTR_ARG))
		.IgnoreArgument(2);

	///act
	IoTHubTransport_JoinWorkerThread(transportHandle, TEST_IOTHUB_CLIENT_HANDLE1);

	///assert
	mocks.AssertActualAndExpectedCalls();

	///cleanup
	IoTHubTransport_Destroy(transportHandle);
}

TEST_FUNCTION(IoTHubTransport_JoinWorkerThread_join_fails)
{
	CIotHubTransportMocks mocks;
	///arrange
	auto transportHandle = IoTHubTransport_Create(TEST_CONFIG.protocol, TEST_CONFIG.iotHubName, TEST_CONFIG.iotHubSuffix);
	(void)IoTHubTransport_StartWorkerThread(transportHandle, TEST_IOTHUB_CLIENT_HANDLE1);
	(void)IoTHubTransport_SignalEndWorkerThread(transportHandle, TEST_IOTHUB_CLIENT_HANDLE1);
	mocks.ResetAllCalls();
	STRICT_EXPECTED_CALL(mocks, ThreadAPI_Join(TEST_THREAD_HANDLE, IGNORED_PTR_ARG))
		.IgnoreArgument(2)
		.SetFailReturn(THREADAPI_ERROR);
	///act
	IoTHubTransport_JoinWorkerThread(transportHandle, TEST_IOTHUB_CLIENT_HANDLE1);

	///assert
	mocks.AssertActualAndExpectedCalls();

	///cleanup
	IoTHubTransport_Destroy(transportHandle);
}

//Tests_SRS_IOTHUBTRANSPORT_17_029: [ The thread shall call lower layer transport DoWork every 1 ms. ]
//Tests_SRS_IOTHUBTRANSPORT_17_030: [ All calls to lower layer transport DoWork shall be protected by the lock created in IoTHubTransport_Create. ]
TEST_FUNCTION(IoTHubTransport_worker_thread_runs_every_1_ms)
{
	CIotHubTransportMocks mocks;
	///arrange

	auto transportHandle = IoTHubTransport_Create(TEST_CONFIG.protocol, TEST_CONFIG.iotHubName, TEST_CONFIG.iotHubSuffix);
    (void)IoTHubTransport_StartWorkerThread(transportHandle, TEST_IOTHUB_CLIENT_HANDLE1);
	mocks.ResetAllCalls();

	howManyDoWorkCalls = 2;
	STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
	STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));

	STRICT_EXPECTED_CALL(mocks, FAKE_IoTHubTransport_DoWork((TRANSPORT_LL_HANDLE)(0x42), NULL));
	STRICT_EXPECTED_CALL(mocks, ThreadAPI_Sleep(1));

	STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
	STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));
	STRICT_EXPECTED_CALL(mocks, FAKE_IoTHubTransport_DoWork((TRANSPORT_LL_HANDLE)(0x42), NULL));
	STRICT_EXPECTED_CALL(mocks, ThreadAPI_Sleep(1));

	STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
	STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));

	///act
	threadFunc(threadFuncArg);

	///assert
	mocks.AssertActualAndExpectedCalls();

	///cleanup
	IoTHubTransport_SignalEndWorkerThread(transportHandle, TEST_IOTHUB_CLIENT_HANDLE1);
	IoTHubTransport_SignalEndWorkerThread(transportHandle, TEST_IOTHUB_CLIENT_HANDLE2);
	//If a thread were actually created, we'd need a Join here.
	IoTHubTransport_Destroy(transportHandle);
}

//Tests_SRS_IOTHUBTRANSPORT_17_029: [ The thread shall call lower layer transport DoWork every 1 ms. ]
//Tests_SRS_IOTHUBTRANSPORT_17_030: [ All calls to lower layer transport DoWork shall be protected by the lock created in IoTHubTransport_Create. 
TEST_FUNCTION(IoTHubTransport_worker_thread_runs_two_devices_once)
{
	CIotHubTransportMocks mocks;
	///arrange

	auto transportHandle = IoTHubTransport_Create(TEST_CONFIG.protocol, TEST_CONFIG.iotHubName, TEST_CONFIG.iotHubSuffix);
    (void)IoTHubTransport_StartWorkerThread(transportHandle, TEST_IOTHUB_CLIENT_HANDLE1);
    (void)IoTHubTransport_StartWorkerThread(transportHandle, TEST_IOTHUB_CLIENT_HANDLE2);
	mocks.ResetAllCalls();

	howManyDoWorkCalls = 1;
	STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
	STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));

	STRICT_EXPECTED_CALL(mocks, FAKE_IoTHubTransport_DoWork((TRANSPORT_LL_HANDLE)(0x42), NULL));

	STRICT_EXPECTED_CALL(mocks, ThreadAPI_Sleep(1));

	STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
	STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));
	///act
	threadFunc(threadFuncArg);

	///assert
	mocks.AssertActualAndExpectedCalls();

	///cleanup
	IoTHubTransport_SignalEndWorkerThread(transportHandle, TEST_IOTHUB_CLIENT_HANDLE1);
	IoTHubTransport_SignalEndWorkerThread(transportHandle, TEST_IOTHUB_CLIENT_HANDLE2);
	IoTHubTransport_Destroy(transportHandle);
}

//Tests_SRS_IOTHUBTRANSPORT_17_031: [ If acquiring the lock fails, lower layer transport DoWork shall not be called. ]
TEST_FUNCTION(IoTHubTransport_worker_thread_runs_lock_fails)
{
	CIotHubTransportMocks mocks;
	///arrange

	auto transportHandle = IoTHubTransport_Create(TEST_CONFIG.protocol, TEST_CONFIG.iotHubName, TEST_CONFIG.iotHubSuffix);
    (void)IoTHubTransport_StartWorkerThread(transportHandle, TEST_IOTHUB_CLIENT_HANDLE1);
    (void)IoTHubTransport_StartWorkerThread(transportHandle, TEST_IOTHUB_CLIENT_HANDLE2);
	mocks.ResetAllCalls();

	howManyDoWorkCalls = 1;
	STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE))
		.SetFailReturn(LOCK_ERROR);
	STRICT_EXPECTED_CALL(mocks, ThreadAPI_Sleep(1));

	/* DoWork needs to run at least once, so, the number of calls to DoWork increments. */
	STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
	STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));
	STRICT_EXPECTED_CALL(mocks, FAKE_IoTHubTransport_DoWork((TRANSPORT_LL_HANDLE)(0x42), NULL));
	STRICT_EXPECTED_CALL(mocks, ThreadAPI_Sleep(1));

	STRICT_EXPECTED_CALL(mocks, Lock(TEST_LOCK_HANDLE));
	STRICT_EXPECTED_CALL(mocks, Unlock(TEST_LOCK_HANDLE));

	///act
	threadFunc(threadFuncArg);

	///assert
	mocks.AssertActualAndExpectedCalls();

	///cleanup
	IoTHubTransport_SignalEndWorkerThread(transportHandle, TEST_IOTHUB_CLIENT_HANDLE1);
	IoTHubTransport_SignalEndWorkerThread(transportHandle, TEST_IOTHUB_CLIENT_HANDLE2);

	IoTHubTransport_Destroy(transportHandle);
}

END_TEST_SUITE(iothubtransport_ut)

