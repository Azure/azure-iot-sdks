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
#include "iotdevice.h"
#include "codefirst.h"
#include "datamarshaller.h"
#include "serializer.h"


static MICROMOCK_MUTEX_HANDLE g_testByTest;

#define TEST_ENTITY_SET_NAME        ("SomeTrucks")
#define TEST_ENTITY_SET_NAMESPACE   ("WhiteTrucksDemo")

#define TEST_BUFFER_STORAGE_HANDLE_1 ((BUFFER_STORAGE_HANDLE)0x4242)
#define TEST_MODEL_HANDLE            ((SCHEMA_MODEL_TYPE_HANDLE)0x4242)

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
//
// We do this namespace redirection so that we don't have to repeat the utility string and buffer code in the mocks!
//
namespace BASEIMPLEMENTATION
{
#include "strings.c"
};

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
    MOCK_METHOD_END(SCHEMA_HANDLE, NULL)
    MOCK_STATIC_METHOD_1(, const char*, Schema_GetSchemaNamespace, SCHEMA_HANDLE, schemaHandle)
    MOCK_METHOD_END(const char*, NULL)

    /*Strings*/
    MOCK_STATIC_METHOD_0(, STRING_HANDLE, STRING_new)
    STRING_HANDLE result2;
    result2 = BASEIMPLEMENTATION::STRING_new();
    MOCK_METHOD_END(STRING_HANDLE, result2)

    MOCK_STATIC_METHOD_1(, STRING_HANDLE, STRING_new_quoted, const char*, source)
    STRING_HANDLE result2;
    result2 = BASEIMPLEMENTATION::STRING_new_quoted(source);
    MOCK_METHOD_END(STRING_HANDLE, result2)

    MOCK_STATIC_METHOD_1(, void, STRING_delete, STRING_HANDLE, s)
    BASEIMPLEMENTATION::STRING_delete(s);
    MOCK_VOID_METHOD_END()

    MOCK_STATIC_METHOD_2(, int, STRING_concat, STRING_HANDLE, s1, const char*, s2)
    MOCK_METHOD_END(int, BASEIMPLEMENTATION::STRING_concat(s1, s2));
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

    /* Device mocks */
    MOCK_STATIC_METHOD_3(, DEVICE_RESULT, Device_PublishTransacted, TRANSACTION_HANDLE, transactionHandle, const char*, propertyName, const AGENT_DATA_TYPE*, data)
    MOCK_METHOD_END(DEVICE_RESULT, DEVICE_OK);
    MOCK_STATIC_METHOD_1(, TRANSACTION_HANDLE, Device_StartTransaction, DEVICE_HANDLE, deviceHandle)
    MOCK_METHOD_END(TRANSACTION_HANDLE, (TRANSACTION_HANDLE)0x4455);
    MOCK_STATIC_METHOD_1(, DEVICE_RESULT, Device_EndTransaction, TRANSACTION_HANDLE, transactionHandle)
    MOCK_METHOD_END(DEVICE_RESULT, DEVICE_OK);
    MOCK_STATIC_METHOD_1(, DEVICE_RESULT, Device_CancelTransaction, TRANSACTION_HANDLE, transactionHandle)
    MOCK_METHOD_END(DEVICE_RESULT, DEVICE_OK);

    /* DataPublisher mocks */
    MOCK_STATIC_METHOD_1(, void, DataPublisher_SetMaxBufferSize, size_t, bytes)
    MOCK_VOID_METHOD_END()
};

DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubSchemaClientMocks, , CODEFIRST_RESULT, CodeFirst_Init, const char*, overrideSchemaNamespace);
DECLARE_GLOBAL_MOCK_METHOD_0(CIoTHubSchemaClientMocks, , void, CodeFirst_Deinit);

DECLARE_GLOBAL_MOCK_METHOD_0(CIoTHubSchemaClientMocks, , STRING_HANDLE, STRING_new);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubSchemaClientMocks, , STRING_HANDLE, STRING_new_quoted, const char*, source);
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

DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubSchemaClientMocks, , TRANSACTION_HANDLE, Device_StartTransaction, DEVICE_HANDLE, deviceHandle);
DECLARE_GLOBAL_MOCK_METHOD_3(CIoTHubSchemaClientMocks, , DEVICE_RESULT, Device_PublishTransacted, TRANSACTION_HANDLE, transactionHandle, const char*, propertyName, const AGENT_DATA_TYPE*, data);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubSchemaClientMocks, , DEVICE_RESULT, Device_EndTransaction, TRANSACTION_HANDLE, transactionHandle);
DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubSchemaClientMocks, , DEVICE_RESULT, Device_CancelTransaction, TRANSACTION_HANDLE, transactionHandle);

DECLARE_GLOBAL_MOCK_METHOD_1(CIoTHubSchemaClientMocks, , void, DataPublisher_SetMaxBufferSize, size_t, bytes);

/* Requirements tested by the virtue of using the exposed API:
Tests_SRS_SCHEMALIB_99_001:[ IotHub_Schema_Client shall expose the following API ... ] */

static MICROMOCK_GLOBAL_SEMAPHORE_HANDLE g_dllByDll;

BEGIN_TEST_SUITE(serializer_ut_without_init)

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
        }

        TEST_FUNCTION_CLEANUP(TestMethodCleanup)
        {
            if (!MicroMockReleaseMutex(g_testByTest))
            {
                ASSERT_FAIL("failure in test framework at ReleaseMutex");
            }
        }

        /* serializer_deinit */

        /* Tests_SRS_SCHEMALIB_99_044:[ If IotHub_Schema_Client is not initialized, serializer_deinit shall not attempt to de-initialize any module.] */
        TEST_FUNCTION(serializer_deinit_Does_Not_Deinitialize_Any_Module)
        {
            // arrange
            CIoTHubSchemaClientMocks serializerMocks;

            // act
            serializer_deinit();

            // assert
            // uMock checks the calls
        }

END_TEST_SUITE(serializer_ut_without_init)
