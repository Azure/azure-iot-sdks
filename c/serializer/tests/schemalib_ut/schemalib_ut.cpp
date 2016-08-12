// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <cstdlib>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "testrunnerswitcher.h"
#include "micromock.h"
#include "micromockcharstararenullterminatedstrings.h"
#include "schemalib.h"
#include "codefirst.h"
#include "iotdevice.h"
#include "datamarshaller.h"
#include "serializer.h"
#include <climits>


static MICROMOCK_MUTEX_HANDLE g_testByTest;

#define TEST_DEVICE_ENDPOINT_1          ("TestDeviceEndpoint42")
#define TEST_DEVICE_TOKEN               ("TestDeviceToken")
#define TEST_API_KEY_1                  ("APIKey1")
#define TEST_API_KEY_2                  ("BabaCloanta")
#define TEST_PER_DEVICE_TOKEN_1         ("ImparatulVerde")
#define TEST_ACCOUNT_NAME_1             ("TestAccountName")
#define TEST_ACCOUNT_NAME_2             ("BananaCo")
#define TEST_SCHEMA_NAMESPACE           "SchemaNamespace42"
#define TEST_ENTITY_SET_NAME            "SomeEntitySetName"
#define TEST_SERVICEBUS_NAMESPACE       "SomeNamespace"
#define TEST_SERVICEBUS_TOKEN           "SomeToken"

#define TEST_MODEL_HANDLE           ((SCHEMA_MODEL_TYPE_HANDLE)0x4242)
#define TEST_SCHEMA_HANDLE          ((SCHEMA_HANDLE)0x4243)

#define EXPECTED_QUALIFIED_ENTITYSET_NAME TEST_SCHEMA_NAMESPACE "." "ROOT_ENTITY_SET_NAME"

#define TEST_BUFFER_STORAGE_HANDLE_1 ((BUFFER_STORAGE_HANDLE)0x4242)

static const size_t actionCount = 42;
static const DEVICE_HANDLE TEST_DEVICE_HANDLE = (DEVICE_HANDLE)0x4747;

DEFINE_MICROMOCK_ENUM_TO_STRING(SERIALIZER_RESULT, SERIALIZER_RESULT_VALUES);

std::ostream& operator<<(std::ostream& left, const EDM_DATE_TIME_OFFSET dateTimeOffset)
{
    return left << "struct tm = (" <<
        dateTimeOffset.dateTime.tm_year << "-" <<
        dateTimeOffset.dateTime.tm_mon << "-" <<
        dateTimeOffset.dateTime.tm_mday << " " <<
        dateTimeOffset.dateTime.tm_hour << ":" <<
        dateTimeOffset.dateTime.tm_min << ":" <<
        dateTimeOffset.dateTime.tm_sec << " ) " <<
        dateTimeOffset.hasFractionalSecond << " " <<
        dateTimeOffset.fractionalSecond << " " <<
        dateTimeOffset.hasTimeZone << " " <<
        dateTimeOffset.timeZoneHour << " " <<
        dateTimeOffset.timeZoneMinute;
}

std::wostream& operator<<(std::wostream& left, const EDM_DATE_TIME_OFFSET dateTimeOffset)
{
    return left << L"struct tm = (" <<
        dateTimeOffset.dateTime.tm_year << L"-" <<
        dateTimeOffset.dateTime.tm_mon << L"-" <<
        dateTimeOffset.dateTime.tm_mday << L" " <<
        dateTimeOffset.dateTime.tm_hour << L":" <<
        dateTimeOffset.dateTime.tm_min << L":" <<
        dateTimeOffset.dateTime.tm_sec << L" ) " <<
        dateTimeOffset.hasFractionalSecond << L" " <<
        dateTimeOffset.fractionalSecond << L" " <<
        dateTimeOffset.hasTimeZone << L" " <<
        dateTimeOffset.timeZoneHour << L" " <<
        dateTimeOffset.timeZoneMinute;
}

static bool operator==(EDM_DATE_TIME_OFFSET left, EDM_DATE_TIME_OFFSET right)
{
    return memcmp(&left, &right, sizeof(left)) == 0;
}

static int EDM_DATE_TIME_OFFSET_Compare(EDM_DATE_TIME_OFFSET left, EDM_DATE_TIME_OFFSET right)
{
    return (memcmp(&left, &right, sizeof(EDM_DATE_TIME_OFFSET)) != 0);
}

static void EDM_DATE_TIME_OFFSET_ToString(char* string, size_t bufferSize, EDM_DATE_TIME_OFFSET val)
{
    (void)bufferSize;
    std::ostringstream o;
    o << val;
    strcpy(string, o.str().c_str());
}


std::ostream& operator<<(std::ostream& left, const EDM_GUID edmGuid)
{
    return left << std::hex <<
        edmGuid.GUID[0] <<
        edmGuid.GUID[1] <<
        edmGuid.GUID[2] <<
        edmGuid.GUID[3] <<
        "-" <<
        edmGuid.GUID[4] <<
        edmGuid.GUID[5] <<
        "-" <<
        edmGuid.GUID[6] <<
        edmGuid.GUID[7] <<
        "-" <<
        edmGuid.GUID[8] <<
        edmGuid.GUID[9] <<
        edmGuid.GUID[10] <<
        edmGuid.GUID[11] <<
        edmGuid.GUID[12] <<
        edmGuid.GUID[13] <<
        edmGuid.GUID[14] <<
        edmGuid.GUID[15];
}

std::ostream& operator<<(std::ostream& left, const EDM_BINARY edmBinary)
{
    left << std::hex;
    for (size_t i = 0; i < edmBinary.size; i++)
    {
        left << edmBinary.data[i];
    }
    return left;
}

std::wostream& operator<<(std::wostream& left, const EDM_GUID edmGuid)
{
    return left << std::hex <<
        edmGuid.GUID[0] <<
        edmGuid.GUID[1] <<
        edmGuid.GUID[2] <<
        edmGuid.GUID[3] <<
        L"-" <<
        edmGuid.GUID[4] <<
        edmGuid.GUID[5] <<
        "-" <<
        edmGuid.GUID[6] <<
        edmGuid.GUID[7] <<
        L"-" <<
        edmGuid.GUID[8] <<
        edmGuid.GUID[9] <<
        edmGuid.GUID[10] <<
        edmGuid.GUID[11] <<
        edmGuid.GUID[12] <<
        edmGuid.GUID[13] <<
        edmGuid.GUID[14] <<
        edmGuid.GUID[15];
}

std::wostream& operator<<(std::wostream& left, const EDM_BINARY edmBinary)
{
    left << std::hex;
    for (size_t i = 0; i < edmBinary.size; i++)
    {
        left << edmBinary.data[i];
    }
    return left;
}

static bool operator==(EDM_GUID left, EDM_GUID right)
{
    return memcmp(&left, &right, sizeof(left)) == 0;
}

static bool operator==(EDM_BINARY left, EDM_BINARY right)
{
    if (left.size != right.size)
    {
        return false;
    }
    else
    {
        return memcmp(left.data, right.data, left.size) == 0;
    }
}

static int EDM_GUID_Compare(EDM_GUID left, EDM_GUID right)
{
    return (memcmp(&left, &right, sizeof(EDM_GUID)) != 0);
}

/*retuns !=0 if they are equal*/
static int EDM_BINARY_Compare(EDM_BINARY left, EDM_BINARY right)
{
    return !((left.size == right.size) && (memcmp(left.data, right.data, left.size) == 0));
}


static void EDM_GUID_ToString(char* string, size_t bufferSize, EDM_GUID val)
{
    (void)bufferSize;
    std::ostringstream o;
    o << val;
    strcpy(string, o.str().c_str());
}

static void EDM_BINARY_ToString(char* string, size_t bufferSize, EDM_BINARY val)
{
    (void)bufferSize;
    std::ostringstream o;
    o << val;
    strcpy(string, o.str().c_str());
}

static int bool_Compare(bool left, bool right)
{
    return left != right;
}

static void bool_ToString(char* string, size_t bufferSize, bool val)
{
    (void)bufferSize;
    (void)strcpy(string, val ? "true" : "false");
}


#ifdef CPP_UNITTEST
template <> static std::wstring Microsoft::VisualStudio::CppUnitTestFramework::ToString < EDM_DATE_TIME_OFFSET >(const EDM_DATE_TIME_OFFSET & q)
{
    std::wstring result;
    std::wostringstream o;
    o << q;
    return o.str();
}

template <> static std::wstring Microsoft::VisualStudio::CppUnitTestFramework::ToString < EDM_GUID >(const EDM_GUID & q)
{
    std::wstring result;
    std::wostringstream o;
    o << q;
    return o.str();
}

template <> static std::wstring Microsoft::VisualStudio::CppUnitTestFramework::ToString < EDM_BINARY >(const EDM_BINARY& q)
{
    std::wstring result;
    std::wostringstream o;
    o << q;
    return o.str();

}
#endif

#define TEST_MANAGEMENT_API_KEY "greenKey"
#define TEST_MANAGEMENT_HOSTNAME "contoso!!!"
#define TEST_DEVICE_API_KEY "redKey"
#define TEST_DEVICE_HOSTNAME "Kontoso!!!"
//
// We do this namespace redirection so that we don't have to repeat the utility string and buffer code in the mocks!
//
namespace BASEIMPLEMENTATION
{
#include "strings.c"
};

static const char* lastmanagementAPIKey = NULL;
static const char* lastmanagementEndpointHost = NULL;
static const char* lastdeviceAPIKey = NULL;
static const char* lastdeviceEndpointHost = NULL;

static size_t currentSTRING_new_call;
static size_t whenShallSTRING_new_fail;

static bool currentSTRING_concat_called_from_another_mock;
static size_t currentSTRING_concat_call;
static size_t whenShallSTRING_concat_fail;

static size_t nSTRING_new_calls = 0;
static size_t nSTRING_delete_calls = 0;

TYPED_MOCK_CLASS(CIoTHubSchemaClientMocks, CGlobalMock)
{
public:
    /* CodeFirst mocks */
    MOCK_STATIC_METHOD_1(, CODEFIRST_RESULT, CodeFirst_Init, const char*, overrideSchemaNamespace)
    MOCK_METHOD_END(CODEFIRST_RESULT, CODEFIRST_OK)
    MOCK_STATIC_METHOD_0(, void, CodeFirst_Deinit)
    MOCK_VOID_METHOD_END()

    /* Schema mocks */
    MOCK_STATIC_METHOD_1(, SCHEMA_HANDLE, Schema_GetSchemaForModelType, SCHEMA_MODEL_TYPE_HANDLE, modelHandle)
    MOCK_METHOD_END(SCHEMA_HANDLE, TEST_SCHEMA_HANDLE)
    MOCK_STATIC_METHOD_1(, const char*, Schema_GetSchemaNamespace, SCHEMA_HANDLE, schemaHandle)
    MOCK_METHOD_END(const char*, TEST_SCHEMA_NAMESPACE)

    /* ServiceBus mocks */
    MOCK_STATIC_METHOD_0(, void, ServiceBus_Deinit)
    MOCK_VOID_METHOD_END()

    /*Strings*/
    MOCK_STATIC_METHOD_0(, STRING_HANDLE, STRING_new)

    STRING_HANDLE result2;
    currentSTRING_new_call++;
    if (whenShallSTRING_new_fail>0)
    {
        if (currentSTRING_new_call == whenShallSTRING_new_fail)
        {
            result2 = (STRING_HANDLE)NULL;
        }
        else
        {
            nSTRING_new_calls++;
            result2 = BASEIMPLEMENTATION::STRING_new();
        }
    }
    else
    {
        nSTRING_new_calls++;
        result2 = BASEIMPLEMENTATION::STRING_new();
    }
    MOCK_METHOD_END(STRING_HANDLE, result2)
    MOCK_STATIC_METHOD_1(, void, STRING_delete, STRING_HANDLE, s)
    nSTRING_delete_calls++;
    BASEIMPLEMENTATION::STRING_delete(s);
    MOCK_VOID_METHOD_END()

    MOCK_STATIC_METHOD_2(, int, STRING_concat, STRING_HANDLE, s1, const char*, s2)
    currentSTRING_concat_call += (currentSTRING_concat_called_from_another_mock) ? 0 : 1;
    MOCK_METHOD_END(int, (((whenShallSTRING_concat_fail>0) && (currentSTRING_concat_call == whenShallSTRING_concat_fail)) ? __LINE__ : BASEIMPLEMENTATION::STRING_concat(s1, s2)));

    MOCK_STATIC_METHOD_1(, const char*, STRING_c_str, STRING_HANDLE, s)
    MOCK_METHOD_END(const char*, BASEIMPLEMENTATION::STRING_c_str(s))

    /* AgentTypeSystem mocks */
    MOCK_STATIC_METHOD_2(, AGENT_DATA_TYPES_RESULT, Create_EDM_BOOLEAN_from_int, AGENT_DATA_TYPE*, agentData, int, v)
    MOCK_METHOD_END(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK);
    MOCK_STATIC_METHOD_2(, AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_UINT8, AGENT_DATA_TYPE*, agentData, uint8_t, v)
    MOCK_METHOD_END(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK);
    MOCK_STATIC_METHOD_2(, AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_DOUBLE, AGENT_DATA_TYPE*, agentData, double, v)
    MOCK_METHOD_END(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK);
    MOCK_STATIC_METHOD_2(, AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_SINT16, AGENT_DATA_TYPE*, agentData, int16_t, v)
    MOCK_METHOD_END(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK);
    MOCK_STATIC_METHOD_2(, AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_SINT32, AGENT_DATA_TYPE*, agentData, int32_t, v)
    MOCK_METHOD_END(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK);
    MOCK_STATIC_METHOD_2(, AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_SINT64, AGENT_DATA_TYPE*, agentData, int64_t, v)
    MOCK_METHOD_END(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK);
    MOCK_STATIC_METHOD_2(, AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_SINT8, AGENT_DATA_TYPE*, agentData, int8_t, v)
    MOCK_METHOD_END(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK);
    MOCK_STATIC_METHOD_2(, AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_FLOAT, AGENT_DATA_TYPE*, agentData, float, v)
    MOCK_METHOD_END(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK);
    MOCK_STATIC_METHOD_2(, AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_charz, AGENT_DATA_TYPE*, agentData, const char*, v)
    MOCK_METHOD_END(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK);
    MOCK_STATIC_METHOD_2(, AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_charz_no_quotes, AGENT_DATA_TYPE*, agentData, const char*, v)
    MOCK_METHOD_END(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK);
    MOCK_STATIC_METHOD_1(, void, Destroy_AGENT_DATA_TYPE, AGENT_DATA_TYPE*, agentData)
    MOCK_VOID_METHOD_END()
    MOCK_STATIC_METHOD_5(, AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_Members, AGENT_DATA_TYPE*, agentData, const char*, typeName, size_t, nMembers, const char* const *, memberNames, const AGENT_DATA_TYPE*, memberValues)
    MOCK_METHOD_END(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK);
    MOCK_STATIC_METHOD_2(, AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_EDM_DATE_TIME_OFFSET, AGENT_DATA_TYPE*, agentData, EDM_DATE_TIME_OFFSET, v)
    MOCK_METHOD_END(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK);
    MOCK_STATIC_METHOD_2(, AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_EDM_GUID, AGENT_DATA_TYPE*, agentData, EDM_GUID, v)
    MOCK_METHOD_END(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK);
    MOCK_STATIC_METHOD_2(, AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_EDM_BINARY, AGENT_DATA_TYPE*, agentData, EDM_BINARY, v)
    MOCK_METHOD_END(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK);

    /* BufferProcess mocks */
    MOCK_STATIC_METHOD_1(, void, BufferProcess_SetRetryInterval, uint64_t,  milliseconds)
    MOCK_VOID_METHOD_END()

    /* DataMarshaller mocks */
    MOCK_STATIC_METHOD_1(, void, DataMarshaller_SetMaxBufferSize, size_t, bytes)
    MOCK_VOID_METHOD_END()

    /* DataPublisher mocks */
    MOCK_STATIC_METHOD_1(, void, DataPublisher_SetMaxBufferSize, size_t, bytes)
    MOCK_VOID_METHOD_END()
};

DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubSchemaClientMocks, , CODEFIRST_RESULT, CodeFirst_Init, const char*, overrideSchemaNamespace);
DECLARE_GLOBAL_MOCK_METHOD_0(CIoTHubSchemaClientMocks, , void, CodeFirst_Deinit);

DECLARE_GLOBAL_MOCK_METHOD_0(CIoTHubSchemaClientMocks, , STRING_HANDLE, STRING_new);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubSchemaClientMocks, , void, STRING_delete, STRING_HANDLE, s);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubSchemaClientMocks, , int, STRING_concat, STRING_HANDLE, s1, const char*, s2);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubSchemaClientMocks, , const char*, STRING_c_str, STRING_HANDLE, s);

DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubSchemaClientMocks, , SCHEMA_HANDLE, Schema_GetSchemaForModelType, SCHEMA_MODEL_TYPE_HANDLE, modelHandle);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubSchemaClientMocks, , const char*, Schema_GetSchemaNamespace, SCHEMA_HANDLE, schemaHandle);

DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubSchemaClientMocks, , AGENT_DATA_TYPES_RESULT, Create_EDM_BOOLEAN_from_int, AGENT_DATA_TYPE*, agentData, int, v);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubSchemaClientMocks, , AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_UINT8, AGENT_DATA_TYPE*, agentData, uint8_t, v);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubSchemaClientMocks, , AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_DOUBLE, AGENT_DATA_TYPE*, agentData, double, v);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubSchemaClientMocks, , AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_SINT16, AGENT_DATA_TYPE*, agentData, int16_t, v);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubSchemaClientMocks, , AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_SINT32, AGENT_DATA_TYPE*, agentData, int32_t, v);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubSchemaClientMocks, , AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_SINT64, AGENT_DATA_TYPE*, agentData, int64_t, v);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubSchemaClientMocks, , AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_SINT8, AGENT_DATA_TYPE*, agentData, int8_t, v);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubSchemaClientMocks, , AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_FLOAT, AGENT_DATA_TYPE*, agentData, float, v);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubSchemaClientMocks, , AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_charz, AGENT_DATA_TYPE*, agentData, const char*, v);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubSchemaClientMocks, , AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_charz_no_quotes, AGENT_DATA_TYPE*, agentData, const char*, v);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubSchemaClientMocks, , void, Destroy_AGENT_DATA_TYPE, AGENT_DATA_TYPE*, agentData);
DECLARE_GLOBAL_MOCK_METHOD_5(CIoTHubSchemaClientMocks, , AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_Members, AGENT_DATA_TYPE*, agentData, const char*, typeName, size_t, nMembers, const char* const *, memberNames, const AGENT_DATA_TYPE*, memberValues);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubSchemaClientMocks, , AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_EDM_DATE_TIME_OFFSET, AGENT_DATA_TYPE*, agentData, EDM_DATE_TIME_OFFSET, v);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubSchemaClientMocks, , AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_EDM_GUID, AGENT_DATA_TYPE*, agentData, EDM_GUID, v);
DECLARE_GLOBAL_MOCK_METHOD_2(CIoTHubSchemaClientMocks, , AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_EDM_BINARY, AGENT_DATA_TYPE*, agentData, EDM_BINARY, v);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubSchemaClientMocks, , void, BufferProcess_SetRetryInterval, uint64_t, milliseconds);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubSchemaClientMocks, , void, DataMarshaller_SetMaxBufferSize, size_t, bytes);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubSchemaClientMocks, , void, DataPublisher_SetMaxBufferSize, size_t, bytes);

/*because serializer.h*/

#define IGNORED_INT_ARG FRONTDOOR_DEVICE_KEY

/* Requirements tested by the virtue of using the exposed API:
Tests_SRS_SCHEMALIB_99_001:[ IoTHubSchemaClient shall expose the following API ... ] */

static MICROMOCK_GLOBAL_SEMAPHORE_HANDLE g_dllByDll;

BEGIN_TEST_SUITE(serializer_ut)

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

            nSTRING_new_calls = 0;
            nSTRING_delete_calls = 0;
            currentSTRING_new_call = 0;
            whenShallSTRING_new_fail = 0;

            currentSTRING_concat_called_from_another_mock = false;
            currentSTRING_concat_call = 0;
            whenShallSTRING_concat_fail = 0;
        }

        TEST_FUNCTION_CLEANUP(TestMethodCleanup)
        {
            CIoTHubSchemaClientMocks mocks;
            mocks.SetPerformAutomaticCallComparison(AUTOMATIC_CALL_COMPARISON_OFF);

            serializer_deinit();
            ASSERT_ARE_EQUAL(size_t, nSTRING_new_calls, nSTRING_delete_calls);
            if (!MicroMockReleaseMutex(g_testByTest))
            {
                ASSERT_FAIL("failure in test framework at ReleaseMutex");
            }
        }

        /* serializer_init */

        /* Tests_SRS_SCHEMALIB_99_076:[serializer_init shall pass the value of overrideSchemaNamespace argument to CodeFirst_Init.] */
        TEST_FUNCTION(serializer_init_Passes_The_Override_Schema_Namespace_to_CodeFirst)
        {
            // arrange
            CIoTHubSchemaClientMocks mocks;

            STRICT_EXPECTED_CALL(mocks, CodeFirst_Init(TEST_SCHEMA_NAMESPACE));

            // act
            SERIALIZER_RESULT result = serializer_init(TEST_SCHEMA_NAMESPACE);

            // assert
            ASSERT_ARE_EQUAL(SERIALIZER_RESULT, SERIALIZER_OK, result);
        }

        /* Tests_SRS_SCHEMALIB_99_007:[ On error SERIALIZER_CODEFIRST_INIT_FAILED shall be returned.] */
        TEST_FUNCTION(IoTHubSchemaClient_When_CodeFirst_Init_Fails_Init_Fails)
        {
            // arrange
            CIoTHubSchemaClientMocks mocks;

            EXPECTED_CALL(mocks, CodeFirst_Init(NULL))
                .SetReturn(CODEFIRST_INVALID_ARG);

            // act
            SERIALIZER_RESULT result = serializer_init(NULL);

            // assert
            ASSERT_ARE_EQUAL(SERIALIZER_RESULT, SERIALIZER_CODEFIRST_INIT_FAILED, result);
        }

        /* Tests_SRS_SCHEMALIB_99_074:[serializer_init when already initialized shall return SERIALIZER_ALREADY_INIT.] */
        TEST_FUNCTION(serializer_init_After_Init_Fails)
        {
            // arrange
            CIoTHubSchemaClientMocks mocks;

            (void)serializer_init(NULL);

            mocks.ResetAllCalls();

            // act
            SERIALIZER_RESULT result = serializer_init(NULL);

            // assert
            ASSERT_ARE_EQUAL(SERIALIZER_RESULT, SERIALIZER_ALREADY_INIT, result);
        }

        /* Tests_SRS_SCHEMALIB_99_075:[When an serializer_init call fails for any reason the previous initialization state shall be preserved. The initialized state shall only be changed on a succesfull Init.] */
        TEST_FUNCTION(IoTHubSchemaClient_After_CodeFirst_Init_Fails_A_Second_Init_Succeeds)
        {
            // arrange
            CNiceCallComparer<CIoTHubSchemaClientMocks> mocks;

            /* make the first init fail due to a CodeFirst_Init failure */
            EXPECTED_CALL(mocks, CodeFirst_Init(NULL))
                .SetReturn(CODEFIRST_INVALID_ARG);

            (void)serializer_init(NULL);
            mocks.ResetAllCalls();

            // act
            SERIALIZER_RESULT result = serializer_init(NULL);

            // assert
            ASSERT_ARE_EQUAL(SERIALIZER_RESULT, SERIALIZER_OK, result);
        }

        /* serializer_deinit */

        /* Tests_SRS_SCHEMALIB_99_025:[ serializer_deinit shall de-initialize all modules initialized be IoTHubSchemaClient_Start.] */
        TEST_FUNCTION(serializer_deinit_When_Only_Init_Deinitializes_Just_The_Modules_Initialized_By_Init)
        {
            // arrange
            CIoTHubSchemaClientMocks mocks;

            (void)serializer_init(NULL);

            mocks.ResetAllCalls();

            STRICT_EXPECTED_CALL(mocks, CodeFirst_Deinit());

            // act
            serializer_deinit();

            // assert
            // uMock checks the calls
        }

        /* Tests_SRS_SCHEMALIB_99_025:[ serializer_deinit shall de-initialize all modules initialized be IoTHubSchemaClient_Start.] */
        TEST_FUNCTION(serializer_deinit_Deinitializes_All_Modules)
        {
            // arrange
            CIoTHubSchemaClientMocks mocks;

            (void)serializer_init(NULL);

            mocks.ResetAllCalls();

            STRICT_EXPECTED_CALL(mocks, CodeFirst_Deinit());

            // act
            serializer_deinit();

            // assert
            // uMock checks the calls
        }

        /* Tests_SRS_SCHEMALIB_99_044:[ If IoTHubSchemaClient is not initialized, serializer_deinit shall not attempt to de-initialize any module.] */
        TEST_FUNCTION(serializer_deinit_When_Not_Initialized_Does_Not_Deinitialize_Any_Module)
        {
            // arrange
            CIoTHubSchemaClientMocks mocks;

            // act
            serializer_deinit();

            // assert
            // uMock checks the calls
        }

        /* Tests_SRS_SCHEMALIB_99_044:[ If IoTHubSchemaClient is not initialized, serializer_deinit shall not attempt to de-initialize any module.] */
        TEST_FUNCTION(serializer_deinit_After_Init_And_Deinit_Does_Not_Deinitialize_Any_Module)
        {
            // arrange
            CIoTHubSchemaClientMocks mocks;

            (void)serializer_init(NULL);
            serializer_deinit();

            mocks.ResetAllCalls();

            // act
            serializer_deinit();

            // assert
            // uMock checks the calls
        }

        /* serializer_setconfig */

        /* Tests_SRS_SCHEMALIB_99_137:[ If the value argument is NULL, serializer_setconfig shall return SERIALIZER_INVALID_ARG.] */
        TEST_FUNCTION(serializer_setconfig_fails_when_value_argument_is_NULL)
        {
            // arrange
            // act
            SERIALIZER_RESULT result = serializer_setconfig(CommandPollingInterval, NULL);

            // assert
            ASSERT_ARE_EQUAL(SERIALIZER_RESULT, SERIALIZER_INVALID_ARG, result);
        }

        /* Tests_SRS_SCHEMALIB_99_138:[ If the which argument is not one of the declared members of the SERIALIZER_CONFIG enum, serializer_setconfig shall return SERIALIZER_INVALID_ARG.] */
        TEST_FUNCTION(serializer_setconfig_fails_when_SERIALIZER_CONFIG_arg_is_invalid)
        {
            // arrange
            uint64_t milliseconds = 42;

            // act
            SERIALIZER_RESULT result = serializer_setconfig((SERIALIZER_CONFIG)INT_MAX, &milliseconds);

            // assert
            ASSERT_ARE_EQUAL(SERIALIZER_RESULT, SERIALIZER_INVALID_ARG, result);
        }

        /* Tests_SRS_SCHEMALIB_99_142:[ When the which argument is TransmitDelayedBufferMaxSize, serializer_setconfig shall invoke DataPublisher_SetMaxBufferSize with the dereferenced value argument, and shall return SERIALIZER_OK.] */
        TEST_FUNCTION(serializer_setconfig_passes_the_max_delayed_buffer_size_to_the_data_publisher)
        {
            // arrange
            CNiceCallComparer<CIoTHubSchemaClientMocks> mocks;
            size_t bytes = 42;

            STRICT_EXPECTED_CALL(mocks, DataPublisher_SetMaxBufferSize(bytes));

            // act
            SERIALIZER_RESULT result = serializer_setconfig(SerializeDelayedBufferMaxSize, &bytes);

            // assert
            ASSERT_ARE_EQUAL(SERIALIZER_RESULT, SERIALIZER_OK, result);
        }

END_TEST_SUITE(serializer_ut)
