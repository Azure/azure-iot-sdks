/*
Microsoft Azure IoT Client Libraries
Copyright (c) Microsoft Corporation
All rights reserved.
MIT License
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
documentation files (the Software), to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED *AS IS*, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
IN THE SOFTWARE.
*/

#include <cstdlib>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "testrunnerswitcher.h"
#include "micromock.h"
#include "micromockcharstararenullterminatedstrings.h"
/*the below is a horrible hack*/
#include "macro_utils.h"
#undef DEFINE_ENUM
#define DEFINE_ENUM(enumName, ...) typedef enum C2(enumName, _TAG) { FOR_EACH_1(DEFINE_ENUMERATION_CONSTANT, __VA_ARGS__)} enumName; 

#include "map.h"
#include "mqttapi.h"
#include "lock.h"
#include "strings.h"
#include "crt_abstractions.h"
#include "vector.h"
#include "MQTTClient.h"
#include "doublylinkedlist.h"
#include "sastoken.h"
#include "agenttime.h"


DEFINE_MICROMOCK_ENUM_TO_STRING(MQTTAPI_RESULT, MQTTAPI_RESULT_VALUES);

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

#include "strings.c"
#include "buffer.c"
#include "vector.c"
#include "doublylinkedlist.c"
};

static size_t currentmalloc_call;
static size_t whenShallmalloc_fail;

static size_t currentrealloc_call;
static size_t whenShallrealloc_fail;

/*different STRING constructors*/
static size_t currentSTRING_new_call;
static size_t whenShallSTRING_new_fail;

static size_t currentSTRING_clone_call;
static size_t whenShallSTRING_clone_fail;

static size_t currentSTRING_construct_call;
static size_t whenShallSTRING_construct_fail;

static size_t currentSTRING_concat_call;
static size_t whenShallSTRING_concat_fail;

static size_t currentSTRING_empty_call;
static size_t whenShallSTRING_empty_fail;

static size_t currentSTRING_concat_with_STRING_call;
static size_t whenShallSTRING_concat_with_STRING_fail;


//CONSTANTS
#define TEST_DEVICE_ID "thisIsDeviceId"
#define TEST_DEVICE_KEY "thisIsDeviceKey"
#define TEST_SASTOKEN_SR "thisIsIotHubName.thisIsIoTHubNameSuffix\\Devices\\thisIsDeviceId"
#define TEST_IOTHUB_NAME "thisIsIotHubName"
#define TEST_LOCK_HANDLE (LOCK_HANDLE)0x4443


//Configuration Variations
static const MQTTAPI_ConnectOptions TEST_CONFIG_MQTTAPIconnectionOptions =
{
	TEST_DEVICE_ID,/*const char* deviceId;                       */
	TEST_DEVICE_KEY,/*const char* deviceKey;                      */
	TEST_SASTOKEN_SR, /*const char* sasTokenSr;					 */
	TEST_IOTHUB_NAME,/*const char* serverURI;                     */
};


//DEFINE HERE ALL OUR CONSTANTS

TYPED_MOCK_CLASS(CMQTTAPIMocks, CGlobalMock)
{
public:

	/* Lock mocks */
	MOCK_STATIC_METHOD_0(, LOCK_HANDLE, Lock_Init)
	MOCK_METHOD_END(LOCK_HANDLE, TEST_LOCK_HANDLE)

	MOCK_STATIC_METHOD_1(, LOCK_RESULT, Lock, LOCK_HANDLE, handle)
	MOCK_METHOD_END(LOCK_RESULT, LOCK_OK)

	MOCK_STATIC_METHOD_1(, LOCK_RESULT, Unlock, LOCK_HANDLE, handle)
	MOCK_METHOD_END(LOCK_RESULT, LOCK_OK)

	MOCK_STATIC_METHOD_1(, LOCK_RESULT, Lock_Deinit, LOCK_HANDLE, handle)
	MOCK_METHOD_END(LOCK_RESULT, LOCK_OK)



    MOCK_STATIC_METHOD_1(, void*, gballoc_malloc, size_t, size)
        void* result2;
    currentmalloc_call++;
    if (whenShallmalloc_fail>0)
    {
        if (currentmalloc_call == whenShallmalloc_fail)
        {
            result2 = (STRING_HANDLE)NULL;
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
    if (whenShallrealloc_fail>0)
    {
        if (currentrealloc_call == whenShallrealloc_fail)
        {
            result2 = (STRING_HANDLE)NULL;
        }
        else
        {
            result2 = BASEIMPLEMENTATION::gballoc_realloc(ptr,size);
        }
    }
    else
    {
        result2 = BASEIMPLEMENTATION::gballoc_realloc(ptr, size);
    }
    MOCK_METHOD_END(void*,result2);

    MOCK_STATIC_METHOD_1(, void, gballoc_free, void*, ptr)
        BASEIMPLEMENTATION::gballoc_free(ptr);
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

    //MQTTClient STubs
	
	MOCK_STATIC_METHOD_5(, int, MQTTClient_setCallbacks, MQTTClient, handle, void*, context, MQTTClient_connectionLost*, cl, MQTTClient_messageArrived*, ma, MQTTClient_deliveryComplete*, dc)
	MOCK_METHOD_END(int, MQTTCLIENT_SUCCESS)

		
	MOCK_STATIC_METHOD_5(, int, MQTTClient_create,MQTTClient*, handle, const char*, serverURI, const char*, clientId, int, persistence_type, void*, persistence_context)
	MOCK_METHOD_END(int, MQTTCLIENT_SUCCESS)
		

	MOCK_STATIC_METHOD_2(, int, MQTTClient_connect, MQTTClient, handle, MQTTClient_connectOptions*, options)
	MOCK_METHOD_END(int, MQTTCLIENT_SUCCESS)

	MOCK_STATIC_METHOD_1(, void, MQTTClient_free, void*, ptr)
	MOCK_VOID_METHOD_END()
		
	MOCK_STATIC_METHOD_2(, int, MQTTClient_disconnect, MQTTClient, handle, int, timeout)
	MOCK_METHOD_END(int, MQTTCLIENT_SUCCESS)

	MOCK_STATIC_METHOD_1(, void, MQTTClient_destroy, MQTTClient*, handle)
	MOCK_VOID_METHOD_END()

	MOCK_STATIC_METHOD_3(, int, MQTTClient_subscribe,MQTTClient, handle, const char*, topic, int, qos)
	MOCK_METHOD_END(int, MQTTCLIENT_SUCCESS)

	MOCK_STATIC_METHOD_2(, int, MQTTClient_unsubscribe,MQTTClient, handle, const char*, topic)
	MOCK_METHOD_END(int, MQTTCLIENT_SUCCESS)

	MOCK_STATIC_METHOD_1(, void, MQTTClient_freeMessage, MQTTClient_message**, msg)
	MOCK_VOID_METHOD_END()

	MOCK_STATIC_METHOD_4(, int, MQTTClient_publishMessage,MQTTClient, handle, const char*, topicName, MQTTClient_message*, msg, MQTTClient_deliveryToken*, dt)
	MOCK_METHOD_END(int, MQTTCLIENT_SUCCESS)


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

	MOCK_STATIC_METHOD_2(, void, DList_InsertHeadList, PDLIST_ENTRY, listHead, PDLIST_ENTRY, listEntry)
		BASEIMPLEMENTATION::DList_InsertHeadList(listHead, listEntry);
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

	MOCK_STATIC_METHOD_1(, time_t, get_time, time_t*, t)
	MOCK_METHOD_END(time_t, 0);

	MOCK_STATIC_METHOD_4(, STRING_HANDLE, SASToken_Create, STRING_HANDLE, key, STRING_HANDLE, scope, STRING_HANDLE, keyName, size_t, expiry)
		STRING_HANDLE tokenResult = BASEIMPLEMENTATION::STRING_new();
	MOCK_METHOD_END(STRING_HANDLE, tokenResult);
};

DECLARE_GLOBAL_MOCK_METHOD_0(CMQTTAPIMocks, , LOCK_HANDLE, Lock_Init);
DECLARE_GLOBAL_MOCK_METHOD_1(CMQTTAPIMocks, , LOCK_RESULT, Lock, LOCK_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_1(CMQTTAPIMocks, , LOCK_RESULT, Unlock, LOCK_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_1(CMQTTAPIMocks, , LOCK_RESULT, Lock_Deinit, LOCK_HANDLE, handle);

DECLARE_GLOBAL_MOCK_METHOD_1(CMQTTAPIMocks, , void*, gballoc_malloc, size_t, size);
DECLARE_GLOBAL_MOCK_METHOD_2(CMQTTAPIMocks, , void*, gballoc_realloc, void*, ptr, size_t, size);
DECLARE_GLOBAL_MOCK_METHOD_1(CMQTTAPIMocks, , void, gballoc_free, void*, ptr);

DECLARE_GLOBAL_MOCK_METHOD_0(CMQTTAPIMocks, , STRING_HANDLE, STRING_new);
DECLARE_GLOBAL_MOCK_METHOD_1(CMQTTAPIMocks, , STRING_HANDLE, STRING_clone, STRING_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_1(CMQTTAPIMocks, , STRING_HANDLE, STRING_construct, const char*, s);

DECLARE_GLOBAL_MOCK_METHOD_1(CMQTTAPIMocks, , void, STRING_delete, STRING_HANDLE, s);
DECLARE_GLOBAL_MOCK_METHOD_2(CMQTTAPIMocks, , int, STRING_concat, STRING_HANDLE, s1, const char*, s2);
DECLARE_GLOBAL_MOCK_METHOD_2(CMQTTAPIMocks, , int, STRING_concat_with_STRING, STRING_HANDLE, s1, STRING_HANDLE, s2);
DECLARE_GLOBAL_MOCK_METHOD_1(CMQTTAPIMocks, , int, STRING_empty, STRING_HANDLE, s1);
DECLARE_GLOBAL_MOCK_METHOD_1(CMQTTAPIMocks, , const char*, STRING_c_str, STRING_HANDLE, s);

DECLARE_GLOBAL_MOCK_METHOD_5(CMQTTAPIMocks, , int, MQTTClient_setCallbacks,MQTTClient, handle, void*, context, MQTTClient_connectionLost* ,cl, MQTTClient_messageArrived*, ma, MQTTClient_deliveryComplete*, dc);
DECLARE_GLOBAL_MOCK_METHOD_5(CMQTTAPIMocks, , int, MQTTClient_create, MQTTClient*, handle, const char*, serverURI, const char*, clientId, int, persistence_type, void*, persistence_context);
DECLARE_GLOBAL_MOCK_METHOD_2(CMQTTAPIMocks, , int, MQTTClient_connect, MQTTClient, handle, MQTTClient_connectOptions*, options);
DECLARE_GLOBAL_MOCK_METHOD_1(CMQTTAPIMocks, , void, MQTTClient_free, void*, ptr);
DECLARE_GLOBAL_MOCK_METHOD_1(CMQTTAPIMocks, , void, MQTTClient_destroy, MQTTClient*, handle);
DECLARE_GLOBAL_MOCK_METHOD_2(CMQTTAPIMocks, , int, MQTTClient_disconnect, MQTTClient, handle, int, timeout);
DECLARE_GLOBAL_MOCK_METHOD_3(CMQTTAPIMocks, , int, MQTTClient_subscribe, MQTTClient, handle, const char*, topic, int, qos);
DECLARE_GLOBAL_MOCK_METHOD_2(CMQTTAPIMocks, , int, MQTTClient_unsubscribe, MQTTClient, handle, const char*, topic);
DECLARE_GLOBAL_MOCK_METHOD_1(CMQTTAPIMocks, ,void, MQTTClient_freeMessage,MQTTClient_message**, msg);
DECLARE_GLOBAL_MOCK_METHOD_4(CMQTTAPIMocks, , int, MQTTClient_publishMessage, MQTTClient, handle, const char*, topicName, MQTTClient_message*, msg, MQTTClient_deliveryToken*, dt);

DECLARE_GLOBAL_MOCK_METHOD_1(CMQTTAPIMocks, , void, DList_InitializeListHead, PDLIST_ENTRY, listHead);
DECLARE_GLOBAL_MOCK_METHOD_1(CMQTTAPIMocks, , int, DList_IsListEmpty, PDLIST_ENTRY, listHead);
DECLARE_GLOBAL_MOCK_METHOD_2(CMQTTAPIMocks, , void, DList_InsertTailList, PDLIST_ENTRY, listHead, PDLIST_ENTRY, listEntry);
DECLARE_GLOBAL_MOCK_METHOD_2(CMQTTAPIMocks, , void, DList_InsertHeadList, PDLIST_ENTRY, listHead, PDLIST_ENTRY, listEntry);
DECLARE_GLOBAL_MOCK_METHOD_2(CMQTTAPIMocks, , void, DList_AppendTailList, PDLIST_ENTRY, listHead, PDLIST_ENTRY, ListToAppend);
DECLARE_GLOBAL_MOCK_METHOD_1(CMQTTAPIMocks, , int, DList_RemoveEntryList, PDLIST_ENTRY, listEntry);
DECLARE_GLOBAL_MOCK_METHOD_1(CMQTTAPIMocks, , PDLIST_ENTRY, DList_RemoveHeadList, PDLIST_ENTRY, listHead);

DECLARE_GLOBAL_MOCK_METHOD_1(CMQTTAPIMocks, , time_t, get_time, time_t*, t)
DECLARE_GLOBAL_MOCK_METHOD_4(CMQTTAPIMocks, , STRING_HANDLE, SASToken_Create, STRING_HANDLE, key, STRING_HANDLE, scope, STRING_HANDLE, keyName, size_t, expiry)

static MICROMOCK_MUTEX_HANDLE g_testByTest;
static MICROMOCK_GLOBAL_SEMAPHORE_HANDLE g_dllByDll;


BEGIN_TEST_SUITE(mqttapi_paho_unittests)

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

    currentrealloc_call = 0;
    whenShallrealloc_fail = 0;

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

/* Tests_SRS_MQTTAPI_04_011: [If any member of the parameter options is NULL or empty then MQTTAPI_Create shall return NULL.]  */
TEST_FUNCTION(MQTTAPI_Create_with_NULL_options_fails)
{
	/// arrange
	CMQTTAPIMocks mocks;

    /// act
	auto result = MQTTAPI_Create(NULL);

	/// assert
	ASSERT_IS_NULL(result);
	mocks.AssertActualAndExpectedCalls();

	///clenup

}

/* Tests_SRS_MQTTAPI_04_011: [If any member of the parameter options is NULL or empty then MQTTAPI_Create shall return NULL.]  */
TEST_FUNCTION(MQTTAPI_Create_with_NULL_deviceId_fails)
{
	/// arrange
	CMQTTAPIMocks mocks;

	const MQTTAPI_ConnectOptions TEST_CONFIG_MQTTAPIconnectionOptions_NULL_DEVICEID =
	{
		NULL,/*const char* deviceId;                       */
		TEST_DEVICE_KEY,/*const char* deviceKey;                      */
		TEST_IOTHUB_NAME,/*const char* serverURI;                     */
	};


	/// act
	auto result = MQTTAPI_Create(&TEST_CONFIG_MQTTAPIconnectionOptions_NULL_DEVICEID);

	/// assert
	ASSERT_IS_NULL(result);
	mocks.AssertActualAndExpectedCalls();

	///clenup

}

/* Tests_SRS_MQTTAPI_04_011: [If any member of the parameter options is NULL or empty then MQTTAPI_Create shall return NULL.]  */
TEST_FUNCTION(MQTTAPI_Create_with_Empty_deviceId_fails)
{
	/// arrange
	CMQTTAPIMocks mocks;

	const MQTTAPI_ConnectOptions TEST_CONFIG_MQTTAPIconnectionOptions_EMPTY_DEVICEID =
	{
		"",/*const char* deviceId;                       */
		TEST_DEVICE_KEY,/*const char* deviceKey;                      */
		TEST_IOTHUB_NAME,/*const char* serverURI;                     */
	};


	/// act
	auto result = MQTTAPI_Create(&TEST_CONFIG_MQTTAPIconnectionOptions_EMPTY_DEVICEID);

	/// assert
	ASSERT_IS_NULL(result);
	mocks.AssertActualAndExpectedCalls();

	///clenup

}

/* Tests_SRS_MQTTAPI_04_011: [If any member of the parameter options is NULL or empty then MQTTAPI_Create shall return NULL.]  */
TEST_FUNCTION(MQTTAPI_Create_with_NULL_deviceKey_fails)
{
	/// arrange
	CMQTTAPIMocks mocks;

	const MQTTAPI_ConnectOptions TEST_CONFIG_MQTTAPIconnectionOptions_NULL_DEVICEKEY =
	{
	    TEST_DEVICE_ID,/*const char* deviceId;                       */
		NULL,/*const char* deviceKey;                      */
		TEST_IOTHUB_NAME,/*const char* serverURI;                     */
	};

	/// act
	auto result = MQTTAPI_Create(&TEST_CONFIG_MQTTAPIconnectionOptions_NULL_DEVICEKEY);

	/// assert
	ASSERT_IS_NULL(result);
	mocks.AssertActualAndExpectedCalls();

	///clenup

}

/* Tests_SRS_MQTTAPI_04_011: [If any member of the parameter options is NULL or empty then MQTTAPI_Create shall return NULL.]  */
TEST_FUNCTION(MQTTAPI_Create_with_EMPTY_deviceKey_fails)
{
	/// arrange
	CMQTTAPIMocks mocks;

	const MQTTAPI_ConnectOptions TEST_CONFIG_MQTTAPIconnectionOptions_EMPTY_DEVICEKEY =
	{
		TEST_DEVICE_ID,/*const char* deviceId;                       */
		NULL,/*const char* deviceKey;                      */
		TEST_IOTHUB_NAME,/*const char* serverURI;                     */
	};

	/// act
	auto result = MQTTAPI_Create(&TEST_CONFIG_MQTTAPIconnectionOptions_EMPTY_DEVICEKEY);

	/// assert
	ASSERT_IS_NULL(result);
	mocks.AssertActualAndExpectedCalls();

	///clenup

}

/* Tests_SRS_MQTTAPI_04_011: [If any member of the parameter options is NULL or empty then MQTTAPI_Create shall return NULL.]  */
TEST_FUNCTION(MQTTAPI_Create_with_NULL_serverURI_fails)
{
	/// arrange
	CMQTTAPIMocks mocks;

	const MQTTAPI_ConnectOptions TEST_CONFIG_MQTTAPIconnectionOptions_NULL_SERVERURI =
	{
		TEST_DEVICE_ID,/*const char* deviceId;                       */
		TEST_DEVICE_KEY,/*const char* deviceKey;                      */
		NULL,/*const char* serverURI;                     */
	};

	/// act
	auto result = MQTTAPI_Create(&TEST_CONFIG_MQTTAPIconnectionOptions_NULL_SERVERURI);

	/// assert
	ASSERT_IS_NULL(result);
	mocks.AssertActualAndExpectedCalls();

	///clenup

}

/* Tests_SRS_MQTTAPI_04_011: [If any member of the parameter options is NULL or empty then MQTTAPI_Create shall return NULL.]  */
TEST_FUNCTION(MQTTAPI_Create_with_EMPTY_serverURI_fails)
{
	/// arrange
	CMQTTAPIMocks mocks;

	const MQTTAPI_ConnectOptions TEST_CONFIG_MQTTAPIconnectionOptions_EMPTY_SERVERURI =
	{
		TEST_DEVICE_ID,/*const char* deviceId;                       */
		TEST_DEVICE_KEY,/*const char* deviceKey;                      */
		"",/*const char* serverURI;                     */
	};

	/// act
	auto result = MQTTAPI_Create(&TEST_CONFIG_MQTTAPIconnectionOptions_EMPTY_SERVERURI);

	/// assert
	ASSERT_IS_NULL(result);
	mocks.AssertActualAndExpectedCalls();

	///clenup

}

/* Tests_SRS_MQTTAPI_04_012: [Parameters deviceId, deviceKey and sasTokenSr shall be saved.]  */
/* Tests_SRS_MQTTAPI_04_014: [Otherwise, MQTTAPI_Create shall return a MQTTAPI_HANDLE suitable for further calls to the module.] */
/* Tests_SRS_MQTTAPI_04_024: [MQTTAPI_Create shall call underlying library connection functions to establish connection with the server.] */
/* Tests_SRS_MQTTAPI_04_047: [Otherwise MQTTAPI_Create shall return a non-NULL MQTTAPI_HANDLE]  */
TEST_FUNCTION(MQTTAPI_Create_with_valid_connectionOptions_succeed)
{
	/// arrange
	CMQTTAPIMocks mocks;

	STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
		.IgnoreArgument(1);

	STRICT_EXPECTED_CALL(mocks, DList_InitializeListHead(IGNORED_PTR_ARG)).IgnoreArgument(1);
	STRICT_EXPECTED_CALL(mocks, DList_InitializeListHead(IGNORED_PTR_ARG)).IgnoreArgument(1);
	STRICT_EXPECTED_CALL(mocks, DList_InitializeListHead(IGNORED_PTR_ARG)).IgnoreArgument(1);

	STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_CONFIG_MQTTAPIconnectionOptions.deviceId));
	STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_CONFIG_MQTTAPIconnectionOptions.deviceKey));
	STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_CONFIG_MQTTAPIconnectionOptions.sasTokenSr));

	STRICT_EXPECTED_CALL(mocks, get_time(IGNORED_PTR_ARG))
		.IgnoreArgument(1);
	STRICT_EXPECTED_CALL(mocks, STRING_new());

	STRICT_EXPECTED_CALL(mocks, SASToken_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_NUM_ARG))
		.IgnoreAllArguments();
	STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
		.IgnoreAllArguments();
	STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
		.IgnoreAllArguments();

	STRICT_EXPECTED_CALL(mocks, MQTTClient_create(IGNORED_PTR_ARG, TEST_CONFIG_MQTTAPIconnectionOptions.serverURI, TEST_CONFIG_MQTTAPIconnectionOptions.deviceId, MQTTCLIENT_PERSISTENCE_NONE, NULL))
		.IgnoreArgument(1);

	STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
		.IgnoreAllArguments();

	STRICT_EXPECTED_CALL(mocks, MQTTClient_setCallbacks(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
		.IgnoreAllArguments();

	STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
		.IgnoreAllArguments(); //UserName for connect.

	STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
		.IgnoreAllArguments(); //Password for connect.

	STRICT_EXPECTED_CALL(mocks, MQTTClient_connect(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
		.IgnoreAllArguments();

	STRICT_EXPECTED_CALL(mocks, Lock_Init());

	/// act
	auto result = MQTTAPI_Create(&TEST_CONFIG_MQTTAPIconnectionOptions);

	/// assert
	ASSERT_IS_NOT_NULL(result);
	mocks.AssertActualAndExpectedCalls();

	///clenup
	MQTTAPI_Destroy(result);
}

/* Tests_SRS_MQTTAPI_04_025: [If connection fails MQTTAPI_Create shall return NULL. */
TEST_FUNCTION(MQTTAPI_Create_MQTTClient_connect_fail_fails)
{
	/// arrange
	CMQTTAPIMocks mocks;

	STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
		.IgnoreArgument(1);

	STRICT_EXPECTED_CALL(mocks, DList_InitializeListHead(IGNORED_PTR_ARG)).IgnoreArgument(1);
	STRICT_EXPECTED_CALL(mocks, DList_InitializeListHead(IGNORED_PTR_ARG)).IgnoreArgument(1);
	STRICT_EXPECTED_CALL(mocks, DList_InitializeListHead(IGNORED_PTR_ARG)).IgnoreArgument(1);

	STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_CONFIG_MQTTAPIconnectionOptions.deviceId));
	STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_CONFIG_MQTTAPIconnectionOptions.deviceKey));
	STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_CONFIG_MQTTAPIconnectionOptions.sasTokenSr));

	STRICT_EXPECTED_CALL(mocks, MQTTClient_create(IGNORED_PTR_ARG, TEST_CONFIG_MQTTAPIconnectionOptions.serverURI, TEST_CONFIG_MQTTAPIconnectionOptions.deviceId, MQTTCLIENT_PERSISTENCE_NONE, NULL))
		.IgnoreArgument(1);

	STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
		.IgnoreAllArguments();

	STRICT_EXPECTED_CALL(mocks, MQTTClient_setCallbacks(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
		.IgnoreAllArguments();

	STRICT_EXPECTED_CALL(mocks, STRING_new());

	STRICT_EXPECTED_CALL(mocks, get_time(IGNORED_PTR_ARG))
		.IgnoreArgument(1);

	STRICT_EXPECTED_CALL(mocks, SASToken_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_NUM_ARG))
		.IgnoreAllArguments();
	STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
		.IgnoreAllArguments();
	STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
		.IgnoreAllArguments();

	STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
		.IgnoreAllArguments(); //UserName for connect.

	STRICT_EXPECTED_CALL(mocks, STRING_c_str(IGNORED_PTR_ARG))
		.IgnoreAllArguments(); //Password for connect.

	STRICT_EXPECTED_CALL(mocks, MQTTClient_connect(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
		.IgnoreAllArguments()
		.SetReturn(__LINE__);

	STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
		.IgnoreAllArguments();
	STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
		.IgnoreAllArguments();
	STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
		.IgnoreAllArguments();
	STRICT_EXPECTED_CALL(mocks, MQTTClient_destroy(IGNORED_PTR_ARG))
		.IgnoreAllArguments();
	STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
		.IgnoreAllArguments();

	STRICT_EXPECTED_CALL(mocks, Lock_Init());
	STRICT_EXPECTED_CALL(mocks, Lock_Deinit(TEST_LOCK_HANDLE));


	/// act
	auto result = MQTTAPI_Create(&TEST_CONFIG_MQTTAPIconnectionOptions);

	/// assert
	ASSERT_IS_NULL(result);
	mocks.AssertActualAndExpectedCalls();

	///clenup
	MQTTAPI_Destroy(result);
}

/* Tests_SRS_MQTTAPI_04_015: [If creating instance fails for any reason, then MQTTAPI_Create shall return NULL.] */
TEST_FUNCTION(MQTTAPI_Create_allocation_of_handledata_fails_fail)
{
	/// arrange
	CMQTTAPIMocks mocks;
	
	whenShallmalloc_fail = 1;
	STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
		.IgnoreArgument(1);

	/// act
	auto result = MQTTAPI_Create(&TEST_CONFIG_MQTTAPIconnectionOptions);

	/// assert
	ASSERT_IS_NULL(result);
	mocks.AssertActualAndExpectedCalls();

	///clenup
	MQTTAPI_Destroy(result);
}
/* Tests_SRS_MQTTAPI_04_013: [If saving these parameters fail for any reason MQTTAPI_Create shall return NULL.] */
TEST_FUNCTION(MQTTAPI_Create_construct_deviceId_fail_fail)
{
	/// arrange
	CMQTTAPIMocks mocks;

	STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
		.IgnoreArgument(1);

	STRICT_EXPECTED_CALL(mocks, DList_InitializeListHead(IGNORED_PTR_ARG)).IgnoreArgument(1);
	STRICT_EXPECTED_CALL(mocks, DList_InitializeListHead(IGNORED_PTR_ARG)).IgnoreArgument(1);
	STRICT_EXPECTED_CALL(mocks, DList_InitializeListHead(IGNORED_PTR_ARG)).IgnoreArgument(1);

	STRICT_EXPECTED_CALL(mocks, Lock_Init());
	STRICT_EXPECTED_CALL(mocks, Lock_Deinit(TEST_LOCK_HANDLE));

	whenShallSTRING_construct_fail = 1;
	STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_CONFIG_MQTTAPIconnectionOptions.deviceId));

	STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
		.IgnoreArgument(1);;

	/// act
	auto result = MQTTAPI_Create(&TEST_CONFIG_MQTTAPIconnectionOptions);

	/// assert
	ASSERT_IS_NULL(result);
	mocks.AssertActualAndExpectedCalls();

	///clenup
	MQTTAPI_Destroy(result);
}

/* Tests_SRS_MQTTAPI_04_013: [If saving these parameters fail for any reason MQTTAPI_Create shall return NULL.] */
TEST_FUNCTION(MQTTAPI_Create_construct_deviceKey_fail_fail)
{
	/// arrange
	CMQTTAPIMocks mocks;

	STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
		.IgnoreArgument(1);

	STRICT_EXPECTED_CALL(mocks, DList_InitializeListHead(IGNORED_PTR_ARG)).IgnoreArgument(1);
	STRICT_EXPECTED_CALL(mocks, DList_InitializeListHead(IGNORED_PTR_ARG)).IgnoreArgument(1);
	STRICT_EXPECTED_CALL(mocks, DList_InitializeListHead(IGNORED_PTR_ARG)).IgnoreArgument(1);

	whenShallSTRING_construct_fail = 2;
	STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_CONFIG_MQTTAPIconnectionOptions.deviceId));
	STRICT_EXPECTED_CALL(mocks, STRING_construct(TEST_CONFIG_MQTTAPIconnectionOptions.deviceKey));

	STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
		.IgnoreArgument(1);
	STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
		.IgnoreArgument(1);

	STRICT_EXPECTED_CALL(mocks, Lock_Init());
	STRICT_EXPECTED_CALL(mocks, Lock_Deinit(TEST_LOCK_HANDLE));

	/// act
	auto result = MQTTAPI_Create(&TEST_CONFIG_MQTTAPIconnectionOptions);

	/// assert
	ASSERT_IS_NULL(result);
	mocks.AssertActualAndExpectedCalls();

	///clenup
	MQTTAPI_Destroy(result);
}

/* Tests_SRS_MQTTAPI_04_021: [If parameter instance is NULL then MQTTAPI_Destroy shall take no action.] */
TEST_FUNCTION(MQTTAPI_Destroy_with_NULL_instance_shall_do_nothing)
{
	/// arrange
	CMQTTAPIMocks mocks;
	auto result = MQTTAPI_Create(&TEST_CONFIG_MQTTAPIconnectionOptions);
	mocks.ResetAllCalls();

	/// act

	MQTTAPI_Destroy(NULL);

	/// assert
	mocks.AssertActualAndExpectedCalls();

	///clenup
	MQTTAPI_Destroy(result);
}

/* Tests_SRS_MQTTAPI_04_022: [MQTTAPI_Destroy shall free all resources used by MQTTAPI_HANDLE.]  */
/* Tests_SRS_MQTTAPI_04_049: [If the instance is connected, MQTTAPI_Destroy shall disconnect the instance] */
TEST_FUNCTION(MQTTAPI_Destroy_shall_release_all_mqttapihandle_resources)
{
	/// arrange
	CMQTTAPIMocks mocks;
	auto result = MQTTAPI_Create(&TEST_CONFIG_MQTTAPIconnectionOptions);
	mocks.ResetAllCalls();

	/// act
	STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
		.IgnoreAllArguments();
	STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
		.IgnoreAllArguments();
	STRICT_EXPECTED_CALL(mocks, STRING_delete(IGNORED_PTR_ARG))
		.IgnoreAllArguments();

	STRICT_EXPECTED_CALL(mocks, MQTTClient_disconnect(IGNORED_PTR_ARG, 0))
		.IgnoreArgument(1);

	STRICT_EXPECTED_CALL(mocks, MQTTClient_destroy(IGNORED_PTR_ARG))
		.IgnoreAllArguments();

	STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
		.IgnoreArgument(1);

	STRICT_EXPECTED_CALL(mocks, Lock_Deinit(TEST_LOCK_HANDLE));

	STRICT_EXPECTED_CALL(mocks, DList_RemoveHeadList(IGNORED_PTR_ARG))
		.IgnoreArgument(1);
	STRICT_EXPECTED_CALL(mocks, DList_RemoveHeadList(IGNORED_PTR_ARG))
		.IgnoreArgument(1);
	STRICT_EXPECTED_CALL(mocks, DList_RemoveHeadList(IGNORED_PTR_ARG))
		.IgnoreArgument(1);

	MQTTAPI_Destroy(result);

	/// assert
	mocks.AssertActualAndExpectedCalls();

	///clenup
}

END_TEST_SUITE(mqttapi_paho_unittests)
