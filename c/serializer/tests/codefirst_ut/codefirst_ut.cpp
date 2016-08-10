// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <cstdlib>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "testrunnerswitcher.h"
#include "codefirst.h"
#include "macro_utils.h"
#include "micromock.h"
#include "micromockcharstararenullterminatedstrings.h"
#include "schema.h"
#include <cstdint>
#include "c_bool_size.h"
#include <string>
#include "azure_c_shared_utility/strings.h"
#include "serializer.h"


DEFINE_MICROMOCK_ENUM_TO_STRING(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_RESULT_VALUES);

BEGIN_NAMESPACE(DummyDataProvider)

DECLARE_MODEL(TruckType,

/*Tests_SRS_SERIALIZER_99_004:[ The propertyType can be any of the following data types: double]*/
WITH_DATA(double, this_is_double),

/*Tests_SRS_SERIALIZER_99_005:[ int], */
WITH_DATA(int, this_is_int),

/*Tests_SRS_SERIALIZER_99_006:[ float]*/
WITH_DATA(float, this_is_float),

/*Tests_SRS_SERIALIZER_99_007:[ long]*/
WITH_DATA(long, this_is_long),

/*Tests_SRS_SERIALIZER_99_008:[ int8_t]*/
WITH_DATA(int8_t, this_is_sint8_t),

/*Tests_SRS_SERIALIZER_99_009:[ uint8_t]*/
WITH_DATA(uint8_t, this_is_uint8_t),

/*Tests_SRS_SERIALIZER_99_010:[ int16_t]*/
WITH_DATA(int16_t, this_is_int16_t),

/*Tests_SRS_SERIALIZER_99_011:[ int32_t]*/
WITH_DATA(int32_t, this_is_int32_t),

/*Tests_SRS_SERIALIZER_99_012:[ int64_t]*/
WITH_DATA(int64_t, this_is_int64_t),

/*Tests_SRS_SERIALIZER_99_013:[ bool]*/
WITH_DATA(bool, this_is_bool),

/*Tests_SRS_SERIALIZER_99_014:[ ascii_char_ptr]*/
WITH_DATA(ascii_char_ptr, this_is_ascii_char_ptr),

/* Tests SRS_SERIALIZER_01_001: [ascii_char_ptr_no_quotes, ] */
WITH_DATA(ascii_char_ptr_no_quotes, this_is_ascii_char_ptr_no_quotes),

/*Tests_SRS_SERIALIZER_99_051:[ EDM_DATE_TIME_OFFSET]*/
WITH_DATA(EDM_DATE_TIME_OFFSET, this_is_EdmDateTimeOffset),

/*Tests_SRS_SERIALIZER_99_072:[ EDM_GUID]*/
WITH_DATA(EDM_GUID, this_is_EdmGuid),

/*Tests_SRS_SERIALIZER_99_074:[ EDM_BINARY]*/
WITH_DATA(EDM_BINARY, this_is_EdmBinary),

WITH_ACTION(reset),
WITH_ACTION(setSpeed, double, theSpeed),
WITH_ACTION(test1, double, P1, int, P2, float, P3, long, P4, int8_t, P5, uint8_t, P6, int16_t, P7, int32_t, P8, int64_t, P9, bool, P10, ascii_char_ptr, P11, EDM_DATE_TIME_OFFSET, P12, EDM_GUID, P13, EDM_BINARY, P14, ascii_char_ptr_no_quotes, P15)
);

END_NAMESPACE(DummyDataProvider)

static const char TEST_MODEL_NAME[] = "SimpleDevice";

bool DummyDataProvider_reset_wasCalled;
EXECUTE_COMMAND_RESULT reset(TruckType* device)
{
    (void)device;
    DummyDataProvider_reset_wasCalled = true;
    return EXECUTE_COMMAND_SUCCESS;
}

bool DummyDataProvider_setSpeed_wasCalled;
double DummyDataProvider_setSpeed_theSpeed;
EXECUTE_COMMAND_RESULT setSpeed(TruckType* device, double theSpeed)
{
    (void)device;
    DummyDataProvider_setSpeed_wasCalled = true;
    DummyDataProvider_setSpeed_theSpeed = theSpeed;
    return EXECUTE_COMMAND_SUCCESS;
}

bool                    DummyDataProvider_test1_wasCalled;
double                  DummyDataProvider_test1_P1;
int                     DummyDataProvider_test1_P2;
float                   DummyDataProvider_test1_P3;
long                    DummyDataProvider_test1_P4;
int8_t                  DummyDataProvider_test1_P5;
uint8_t                 DummyDataProvider_test1_P6;
int16_t                 DummyDataProvider_test1_P7;
int32_t                 DummyDataProvider_test1_P8;
int64_t                 DummyDataProvider_test1_P9;
bool                    DummyDataProvider_test1_P10;
#define                 P11_MAX_SIZE 100
char                    DummyDataProvider_test1_P11[P11_MAX_SIZE];
EDM_DATE_TIME_OFFSET    DummyDataProvider_test1_P12;
EDM_GUID                DummyDataProvider_test1_P13;
EDM_BINARY              DummyDataProvider_test1_P14 = { 0, NULL };
#define                 P15_MAX_SIZE 100
char                    DummyDataProvider_test1_P15[P15_MAX_SIZE];

EXECUTE_COMMAND_RESULT test1(TruckType* device, double P1, int P2, float P3, long P4, int8_t P5, uint8_t P6, int16_t P7, int32_t P8, int64_t P9, bool P10, ascii_char_ptr P11, EDM_DATE_TIME_OFFSET P12, EDM_GUID P13, EDM_BINARY P14, ascii_char_ptr_no_quotes P15)
{
    (void)device;
    DummyDataProvider_test1_wasCalled = true;
    DummyDataProvider_test1_P1 = P1;
    DummyDataProvider_test1_P2 = P2;
    DummyDataProvider_test1_P3 = P3;
    DummyDataProvider_test1_P4 = P4;
    DummyDataProvider_test1_P5 = P5;
    DummyDataProvider_test1_P6 = P6;
    DummyDataProvider_test1_P7 = P7;
    DummyDataProvider_test1_P8 = P8;
    DummyDataProvider_test1_P9 = P9;
    DummyDataProvider_test1_P10 = P10;
    strcpy_s(DummyDataProvider_test1_P11, P11_MAX_SIZE, P11);
    DummyDataProvider_test1_P12 = P12;
    DummyDataProvider_test1_P13 = P13;
    DummyDataProvider_test1_P14.size = P14.size;
    DummyDataProvider_test1_P14.data = (unsigned char*)malloc(P14.size);
    memcpy(DummyDataProvider_test1_P14.data, P14.data, P14.size);
    strcpy_s(DummyDataProvider_test1_P15, P15_MAX_SIZE, P15);
    return EXECUTE_COMMAND_SUCCESS;
}

#define TEST_DEFAULT_SCHEMA_NAMESPACE   ("Test.TruckDemo")
#define TEST_DEFAULT_MODEL_NAME         ("TruckType")

static const SCHEMA_HANDLE TEST_SCHEMA_HANDLE = (SCHEMA_MODEL_TYPE_HANDLE)0x4242;
static const SCHEMA_MODEL_TYPE_HANDLE TEST_MODEL_HANDLE = (SCHEMA_MODEL_TYPE_HANDLE)0x4243;
static const SCHEMA_MODEL_TYPE_HANDLE TEST_TRUCKTYPE_MODEL_HANDLE = (SCHEMA_MODEL_TYPE_HANDLE)0x4244;
static const DEVICE_HANDLE TEST_DEVICE_HANDLE = (DEVICE_HANDLE)0x4848;

static const SCHEMA_ACTION_HANDLE TEST1_ACTION_HANDLE = (SCHEMA_ACTION_HANDLE)0x5201;
static const SCHEMA_ACTION_HANDLE SETSPEED_ACTION_HANDLE = (SCHEMA_ACTION_HANDLE)0x5202;
static const SCHEMA_ACTION_HANDLE RESET_ACTION_HANDLE = (SCHEMA_ACTION_HANDLE)0x5202;
static const SCHEMA_STRUCT_TYPE_HANDLE TEST_STRUCT_TYPE_HANDLE = (SCHEMA_STRUCT_TYPE_HANDLE)0x6202;
static const TRANSACTION_HANDLE TEST_TRANSACTION_HANDLE = (TRANSACTION_HANDLE)0x6242;

#define MAX_NAME_LENGTH 100
typedef char someName[MAX_NAME_LENGTH];
#define MAX_RECORDINGS 100

static  AGENT_DATA_TYPE* Create_EDM_BOOLEAN_from_int_agentData = NULL;
static  AGENT_DATA_TYPE* Create_AGENT_DATA_TYPE_from_UINT8_agentData = NULL;
static  AGENT_DATA_TYPE* Create_AGENT_DATA_TYPE_from_DOUBLE_agentData = NULL;
static  AGENT_DATA_TYPE* Create_AGENT_DATA_TYPE_from_SINT16_agentData = NULL;
static  AGENT_DATA_TYPE* Create_AGENT_DATA_TYPE_from_SINT32_agentData = NULL;
static  AGENT_DATA_TYPE* Create_AGENT_DATA_TYPE_from_SINT64_agentData = NULL;
static  AGENT_DATA_TYPE* Create_AGENT_DATA_TYPE_from_SINT8_agentData = NULL;
static  AGENT_DATA_TYPE* Create_AGENT_DATA_TYPE_from_FLOAT_agentData = NULL;
static  AGENT_DATA_TYPE* Create_AGENT_DATA_TYPE_from_charz_agentData = NULL;
static  AGENT_DATA_TYPE* Create_AGENT_DATA_TYPE_from_charz_no_quotes_agentData = NULL;
static AGENT_DATA_TYPE* Create_AGENT_DATA_TYPE_from_Members_agentData = NULL;
static someName Create_AGENT_DATA_TYPE_from_Members_names[MAX_RECORDINGS];
static const AGENT_DATA_TYPE* Create_AGENT_DATA_TYPE_from_Members_values[MAX_RECORDINGS];
static  AGENT_DATA_TYPE* Create_AGENT_DATA_TYPE_from_EDM_DATE_TIME_OFFSET_agentData = NULL;
static  AGENT_DATA_TYPE* Create_AGENT_DATA_TYPE_from_EDM_GUID_agentData = NULL;
static  AGENT_DATA_TYPE* Create_AGENT_DATA_TYPE_from_EDM_BINARY_agentData = NULL;
static const AGENT_DATA_TYPE* Device_Publish_agentData = NULL;
static const AGENT_DATA_TYPE* Device_PublishTransacted_agentData = NULL;
static  AGENT_DATA_TYPE* Destroy_AGENT_DATA_TYPE_agentData = NULL;

#define TEST_CALLBACK_CONTEXT   ((void*)0x4247)
#define TEST_COMMAND "this be some command"

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
    (void)strcpy(string, val?"true":"false");
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

DEFINE_MICROMOCK_ENUM_TO_STRING(CODEFIRST_RESULT, CODEFIRST_ENUM_VALUES)

static MICROMOCK_MUTEX_HANDLE g_testByTest;

static char someChars[] = { "someChars" };
static char someMoreChars[] = { "someMoreChars" };
static AGENT_DATA_TYPE someDouble;
static AGENT_DATA_TYPE someInt32;
static AGENT_DATA_TYPE arrayOfAgentDataType[15];

static EDM_DATE_TIME_OFFSET someEdmDateTimeOffset;
static EDM_GUID someEdmGuid;
static EDM_BINARY someEdmBinary;
static void* g_InvokeActionCallbackArgument;

TYPED_MOCK_CLASS(CMocksForCodeFirst, CGlobalMock)
{
public:
    MOCK_STATIC_METHOD_2(, int, Create_AGENT_DATA_TYPE_From_Ptr_this_is_double, void*, param, AGENT_DATA_TYPE*, dest)
        Create_AGENT_DATA_TYPE_from_DOUBLE_agentData = dest;
    MOCK_METHOD_END(int, 0);

    MOCK_STATIC_METHOD_2(, int, Create_AGENT_DATA_TYPE_From_Ptr_this_is_int, void*, param, AGENT_DATA_TYPE*, dest)
        Create_AGENT_DATA_TYPE_from_SINT32_agentData = dest;
    MOCK_METHOD_END(int, 0);

    MOCK_STATIC_METHOD_2(, AGENT_DATA_TYPES_RESULT, Create_EDM_BOOLEAN_from_int, AGENT_DATA_TYPE*, agentData, int, v)
    {
        Create_EDM_BOOLEAN_from_int_agentData = agentData;
    }
    MOCK_METHOD_END(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK);

    MOCK_STATIC_METHOD_2(, AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_UINT8, AGENT_DATA_TYPE*, agentData, uint8_t, v)
    {
        Create_AGENT_DATA_TYPE_from_UINT8_agentData = agentData;
    }
    MOCK_METHOD_END(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK);

    MOCK_STATIC_METHOD_2(, AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_DOUBLE, AGENT_DATA_TYPE*, agentData, double, v)
    {
        Create_AGENT_DATA_TYPE_from_DOUBLE_agentData = agentData;
    }
    MOCK_METHOD_END(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK);

    MOCK_STATIC_METHOD_2(, AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_SINT16, AGENT_DATA_TYPE*, agentData, int16_t, v)
    {
        Create_AGENT_DATA_TYPE_from_SINT16_agentData = agentData;
    }
    MOCK_METHOD_END(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK);

    MOCK_STATIC_METHOD_2(, AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_SINT32, AGENT_DATA_TYPE*, agentData, int32_t, v)
    {
        Create_AGENT_DATA_TYPE_from_SINT32_agentData = agentData;
    }
    MOCK_METHOD_END(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK);

    MOCK_STATIC_METHOD_2(, AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_SINT64, AGENT_DATA_TYPE*, agentData, int64_t, v)
    {
        Create_AGENT_DATA_TYPE_from_SINT64_agentData = agentData;
    }
    MOCK_METHOD_END(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK);

    MOCK_STATIC_METHOD_2(, AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_SINT8, AGENT_DATA_TYPE*, agentData, int8_t, v)
    {
        Create_AGENT_DATA_TYPE_from_SINT8_agentData = agentData;
    }
    MOCK_METHOD_END(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK);

    MOCK_STATIC_METHOD_2(, AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_FLOAT, AGENT_DATA_TYPE*, agentData, float, v)
    {
        Create_AGENT_DATA_TYPE_from_FLOAT_agentData = agentData;
    }
    MOCK_METHOD_END(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK);

    MOCK_STATIC_METHOD_2(, AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_charz, AGENT_DATA_TYPE*, agentData, const char*, v)
    {
        Create_AGENT_DATA_TYPE_from_charz_agentData = agentData;
    }
    MOCK_METHOD_END(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK);

    MOCK_STATIC_METHOD_2(, AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_charz_no_quotes, AGENT_DATA_TYPE*, agentData, const char*, v)
    {
        Create_AGENT_DATA_TYPE_from_charz_no_quotes_agentData = agentData;
    }
    MOCK_METHOD_END(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK);

    MOCK_STATIC_METHOD_2(, AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_EDM_DATE_TIME_OFFSET, AGENT_DATA_TYPE*, agentData, EDM_DATE_TIME_OFFSET, v)
    {
        Create_AGENT_DATA_TYPE_from_EDM_DATE_TIME_OFFSET_agentData = agentData;
    }
    MOCK_METHOD_END(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK);

    MOCK_STATIC_METHOD_2(, AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_EDM_GUID, AGENT_DATA_TYPE*, agentData, EDM_GUID, v)
    {
        Create_AGENT_DATA_TYPE_from_EDM_GUID_agentData = agentData;
    }
    MOCK_METHOD_END(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK);

    MOCK_STATIC_METHOD_2(, AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_EDM_BINARY, AGENT_DATA_TYPE*, agentData, EDM_BINARY, v)
    {
        Create_AGENT_DATA_TYPE_from_EDM_BINARY_agentData = agentData;
    }
    MOCK_METHOD_END(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK);

    MOCK_STATIC_METHOD_1(, void, Destroy_AGENT_DATA_TYPE, AGENT_DATA_TYPE*, agentData)
    {
        Destroy_AGENT_DATA_TYPE_agentData = agentData;
    }
    MOCK_VOID_METHOD_END()

    MOCK_STATIC_METHOD_5(, AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_Members, AGENT_DATA_TYPE*, agentData, const char*, typeName, size_t, nMembers, const char* const *, memberNames, const AGENT_DATA_TYPE*, memberValues)
    {
        size_t i;
        Create_AGENT_DATA_TYPE_from_Members_agentData = agentData;
        //save the names
        //shall assume that no name is longer than 100 characters.
        for (i = 0; i < nMembers; i++)
        {
            strcpy(Create_AGENT_DATA_TYPE_from_Members_names[i], memberNames[i]);
            Create_AGENT_DATA_TYPE_from_Members_values[i] = memberValues + i;
        }
    }
    MOCK_METHOD_END(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK);

    /* Device mocks */
    MOCK_STATIC_METHOD_5(, DEVICE_RESULT, Device_Create, SCHEMA_MODEL_TYPE_HANDLE, modelHandle, pPfDeviceActionCallback, deviceActionCallback, void*, callbackUserContext, bool, includePropertyPath, DEVICE_HANDLE*, deviceHandle)
        *deviceHandle = TEST_DEVICE_HANDLE;
        g_InvokeActionCallbackArgument = callbackUserContext;
    MOCK_METHOD_END(DEVICE_RESULT, DEVICE_OK);
    MOCK_STATIC_METHOD_1(, void, Device_Destroy, DEVICE_HANDLE, deviceHandle)
    MOCK_VOID_METHOD_END();

    MOCK_STATIC_METHOD_1(, TRANSACTION_HANDLE, Device_StartTransaction, SCHEMA_MODEL_TYPE_HANDLE, modelHandle)
    {
    }
    MOCK_METHOD_END(TRANSACTION_HANDLE, TEST_TRANSACTION_HANDLE);

    MOCK_STATIC_METHOD_3(, DEVICE_RESULT, Device_EndTransaction, TRANSACTION_HANDLE, transactionHandle, unsigned char**, destination, size_t*, destinationSize)
    {
    }
    MOCK_METHOD_END(DEVICE_RESULT, DEVICE_OK);

    MOCK_STATIC_METHOD_1(, DEVICE_RESULT, Device_CancelTransaction, TRANSACTION_HANDLE, transactionHandle)
    {
    }
    MOCK_METHOD_END(DEVICE_RESULT, DEVICE_OK);

    MOCK_STATIC_METHOD_3(, DEVICE_RESULT, Device_PublishTransacted, TRANSACTION_HANDLE, transactionHandle, const char*, propertyName, const AGENT_DATA_TYPE*, data)
    {
        if (
            (
                (strcmp(propertyName, "this_is_double") == 0) &&
                (data != Create_AGENT_DATA_TYPE_from_DOUBLE_agentData)
            )
            /*the below cannot be checked easily because there are 2 types (int, int32_t) setting the same CReate_AGET_DATA_TYPE_from_SINT32_agentData*/
            /*same goes for long and sint64_t*/
            /*(
                (strcmp(propertyName, "this_is_int") == 0) &&
                (data != Create_AGENT_DATA_TYPE_from_DOUBLE_agentData)
            )*/
            ||
            (
                (strcmp(propertyName, "this_is_float") == 0) &&
                (data != Create_AGENT_DATA_TYPE_from_FLOAT_agentData)
            )
            ||
            (
            (strcmp(propertyName, "this_is_sint8_t") == 0) &&
            (data != Create_AGENT_DATA_TYPE_from_SINT8_agentData)
            )
            ||
            (
                (strcmp(propertyName, "this_is_uint8_t") == 0) &&
                (data != Create_AGENT_DATA_TYPE_from_UINT8_agentData)
            )
            ||
            (
                (strcmp(propertyName, "this_is_int16_t") == 0) &&
                (data != Create_AGENT_DATA_TYPE_from_SINT16_agentData)
            )
            ||
            (
                (strcmp(propertyName, "this_is_bool") == 0) &&
                (data != Create_EDM_BOOLEAN_from_int_agentData)
            )
            ||
            (
                (strcmp(propertyName, "this_is_ascii_char_ptr") == 0) &&
                (data != Create_AGENT_DATA_TYPE_from_charz_agentData)
            )
            ||
            (
            (strcmp(propertyName, "this_is_ascii_char_ptr_no_quotes") == 0) &&
            (data != Create_AGENT_DATA_TYPE_from_charz_no_quotes_agentData)
            )
            ||
            (
                (strcmp(propertyName, "this_is_EdmDateTimeOffset") == 0) &&
                (data != Create_AGENT_DATA_TYPE_from_EDM_DATE_TIME_OFFSET_agentData)
            )
        )
        {
            ASSERT_FAIL("where did you get that AGENT_DATA_TYPE?");
        }
        Device_PublishTransacted_agentData = data;
    }
    MOCK_METHOD_END(DEVICE_RESULT, DEVICE_OK);

    MOCK_STATIC_METHOD_2(, EXECUTE_COMMAND_RESULT, Device_ExecuteCommand, DEVICE_HANDLE, deviceHandle, const char*, command);
    MOCK_METHOD_END(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_SUCCESS);

    MOCK_STATIC_METHOD_1(, DEVICE_RESULT, Device_SendAll, DEVICE_HANDLE, deviceHandle)
    MOCK_METHOD_END(DEVICE_RESULT, DEVICE_OK);

    MOCK_STATIC_METHOD_1(, DEVICE_RESULT, Device_DrainCommands, DEVICE_HANDLE, deviceHandle)
    MOCK_METHOD_END(DEVICE_RESULT, DEVICE_OK);


    /* Schema mocks */
    MOCK_STATIC_METHOD_1(, SCHEMA_HANDLE, Schema_GetSchemaByNamespace, const char*, schemaNamespace)
    MOCK_METHOD_END(SCHEMA_HANDLE, NULL);
    MOCK_STATIC_METHOD_1(, SCHEMA_HANDLE, Schema_Create, const char*, schemaNamespace)
    MOCK_METHOD_END(SCHEMA_HANDLE, TEST_SCHEMA_HANDLE);
    MOCK_STATIC_METHOD_2(, SCHEMA_MODEL_TYPE_HANDLE, Schema_CreateModelType, SCHEMA_HANDLE, schemaHandle, const char*, modelName)
    MOCK_METHOD_END(SCHEMA_MODEL_TYPE_HANDLE, TEST_MODEL_HANDLE);
    MOCK_STATIC_METHOD_2(, SCHEMA_STRUCT_TYPE_HANDLE, Schema_CreateStructType, SCHEMA_HANDLE, schemaHandle, const char*, structTypeName)
    MOCK_METHOD_END(SCHEMA_STRUCT_TYPE_HANDLE, TEST_STRUCT_TYPE_HANDLE);
    MOCK_STATIC_METHOD_3(, SCHEMA_RESULT, Schema_AddStructTypeProperty, SCHEMA_STRUCT_TYPE_HANDLE, structTypeHandle, const char*, propertyName, const char*, propertyType)
    MOCK_METHOD_END(SCHEMA_RESULT, SCHEMA_OK);
    MOCK_STATIC_METHOD_3(, SCHEMA_RESULT, Schema_AddModelProperty, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle, const char*, propertyName, const char*, propertyType)
    MOCK_METHOD_END(SCHEMA_RESULT, SCHEMA_OK);
    MOCK_STATIC_METHOD_2(, SCHEMA_ACTION_HANDLE, Schema_CreateModelAction, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle, const char*, actionName)
    MOCK_METHOD_END(SCHEMA_ACTION_HANDLE, TEST1_ACTION_HANDLE);
    MOCK_STATIC_METHOD_3(, SCHEMA_RESULT, Schema_AddModelActionArgument, SCHEMA_ACTION_HANDLE, actionHandle, const char*, argumentName, const char*, argumentType)
    MOCK_METHOD_END(SCHEMA_RESULT, SCHEMA_OK);
    MOCK_STATIC_METHOD_1(, void, Schema_Destroy, SCHEMA_HANDLE, schemaHandle);
    MOCK_VOID_METHOD_END();
    MOCK_STATIC_METHOD_1(, const char*, Schema_GetModelName, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle);
    MOCK_METHOD_END(const char*, TEST_MODEL_NAME);
    MOCK_STATIC_METHOD_2(, SCHEMA_MODEL_TYPE_HANDLE, Schema_GetModelByName, SCHEMA_HANDLE, schemaHandle, const char*, modelName);
    MOCK_METHOD_END(SCHEMA_MODEL_TYPE_HANDLE, (SCHEMA_MODEL_TYPE_HANDLE)NULL);
    MOCK_STATIC_METHOD_3(, SCHEMA_RESULT, Schema_AddModelModel, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle, const char*, propertyName, SCHEMA_MODEL_TYPE_HANDLE, modelType)
    MOCK_METHOD_END(SCHEMA_RESULT, SCHEMA_OK);
    MOCK_STATIC_METHOD_1(, SCHEMA_RESULT, Schema_ReleaseDeviceRef, SCHEMA_MODEL_TYPE_HANDLE, modelHandle)
    MOCK_METHOD_END(SCHEMA_RESULT, SCHEMA_OK);
    MOCK_STATIC_METHOD_1(, SCHEMA_RESULT, Schema_AddDeviceRef, SCHEMA_MODEL_TYPE_HANDLE, schemaHandle)
    MOCK_METHOD_END(SCHEMA_RESULT, SCHEMA_OK);
    MOCK_STATIC_METHOD_1(, SCHEMA_HANDLE, Schema_GetSchemaForModelType, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle);
    MOCK_METHOD_END(SCHEMA_HANDLE, (SCHEMA_HANDLE)NULL);
    MOCK_STATIC_METHOD_1(, SCHEMA_RESULT, Schema_DestroyIfUnused, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle);
    MOCK_METHOD_END(SCHEMA_RESULT, SCHEMA_OK);
};

DECLARE_GLOBAL_MOCK_METHOD_2(CMocksForCodeFirst, , AGENT_DATA_TYPES_RESULT, Create_EDM_BOOLEAN_from_int, AGENT_DATA_TYPE*, agentData, int, v);
DECLARE_GLOBAL_MOCK_METHOD_2(CMocksForCodeFirst, , AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_UINT8, AGENT_DATA_TYPE*, agentData, uint8_t, v);
DECLARE_GLOBAL_MOCK_METHOD_2(CMocksForCodeFirst, , AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_DOUBLE, AGENT_DATA_TYPE*, agentData, double, v);
DECLARE_GLOBAL_MOCK_METHOD_2(CMocksForCodeFirst, , AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_SINT16, AGENT_DATA_TYPE*, agentData, int16_t, v);
DECLARE_GLOBAL_MOCK_METHOD_2(CMocksForCodeFirst, , AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_SINT32, AGENT_DATA_TYPE*, agentData, int32_t, v);
DECLARE_GLOBAL_MOCK_METHOD_2(CMocksForCodeFirst, , AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_SINT64, AGENT_DATA_TYPE*, agentData, int64_t, v);
DECLARE_GLOBAL_MOCK_METHOD_2(CMocksForCodeFirst, , AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_SINT8, AGENT_DATA_TYPE*, agentData, int8_t, v);
DECLARE_GLOBAL_MOCK_METHOD_2(CMocksForCodeFirst, , AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_FLOAT, AGENT_DATA_TYPE*, agentData, float, v);
DECLARE_GLOBAL_MOCK_METHOD_2(CMocksForCodeFirst, , AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_charz, AGENT_DATA_TYPE*, agentData, const char*, v);
DECLARE_GLOBAL_MOCK_METHOD_2(CMocksForCodeFirst, , AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_charz_no_quotes, AGENT_DATA_TYPE*, agentData, const char*, v);
DECLARE_GLOBAL_MOCK_METHOD_2(CMocksForCodeFirst, , AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_EDM_DATE_TIME_OFFSET, AGENT_DATA_TYPE*, agentData, EDM_DATE_TIME_OFFSET, v);
DECLARE_GLOBAL_MOCK_METHOD_2(CMocksForCodeFirst, , AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_EDM_GUID, AGENT_DATA_TYPE*, agentData, EDM_GUID, v);
DECLARE_GLOBAL_MOCK_METHOD_2(CMocksForCodeFirst, , AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_EDM_BINARY, AGENT_DATA_TYPE*, agentData, EDM_BINARY, v);
DECLARE_GLOBAL_MOCK_METHOD_1(CMocksForCodeFirst, , void, Destroy_AGENT_DATA_TYPE, AGENT_DATA_TYPE*, agentData);
DECLARE_GLOBAL_MOCK_METHOD_5(CMocksForCodeFirst, , AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_Members, AGENT_DATA_TYPE*, agentData, const char*, typeName, size_t, nMembers, const char* const *, memberNames, const AGENT_DATA_TYPE*, memberValues);

DECLARE_GLOBAL_MOCK_METHOD_5(CMocksForCodeFirst, , DEVICE_RESULT, Device_Create, SCHEMA_MODEL_TYPE_HANDLE, modelHandle, pPfDeviceActionCallback, deviceActionCallback, void*, callbackUserContext, bool, includePropertyPath, DEVICE_HANDLE*, deviceHandle);
DECLARE_GLOBAL_MOCK_METHOD_1(CMocksForCodeFirst, , void, Device_Destroy, DEVICE_HANDLE, deviceHandle);
DECLARE_GLOBAL_MOCK_METHOD_3(CMocksForCodeFirst, , DEVICE_RESULT, Device_PublishTransacted, TRANSACTION_HANDLE, transactionHandle, const char*, propertyName, const AGENT_DATA_TYPE*, data);
DECLARE_GLOBAL_MOCK_METHOD_1(CMocksForCodeFirst, , TRANSACTION_HANDLE, Device_StartTransaction, SCHEMA_MODEL_TYPE_HANDLE, modelHandle);
DECLARE_GLOBAL_MOCK_METHOD_3(CMocksForCodeFirst, , DEVICE_RESULT, Device_EndTransaction, TRANSACTION_HANDLE, transactionHandle, unsigned char**, destination, size_t*, destinationSize);
DECLARE_GLOBAL_MOCK_METHOD_1(CMocksForCodeFirst, , DEVICE_RESULT, Device_CancelTransaction, TRANSACTION_HANDLE, transactionHandle);
DECLARE_GLOBAL_MOCK_METHOD_2(CMocksForCodeFirst, , EXECUTE_COMMAND_RESULT, Device_ExecuteCommand, DEVICE_HANDLE, deviceHandle, const char*, command);
DECLARE_GLOBAL_MOCK_METHOD_1(CMocksForCodeFirst, , DEVICE_RESULT, Device_SendAll, DEVICE_HANDLE, deviceHandle);
DECLARE_GLOBAL_MOCK_METHOD_1(CMocksForCodeFirst, , DEVICE_RESULT, Device_DrainCommands, DEVICE_HANDLE, deviceHandle);

DECLARE_GLOBAL_MOCK_METHOD_1(CMocksForCodeFirst, , SCHEMA_HANDLE, Schema_GetSchemaByNamespace, const char*, schemaNamespace);
DECLARE_GLOBAL_MOCK_METHOD_1(CMocksForCodeFirst, , SCHEMA_HANDLE, Schema_Create, const char*, schemaNamespace);
DECLARE_GLOBAL_MOCK_METHOD_2(CMocksForCodeFirst, , SCHEMA_MODEL_TYPE_HANDLE, Schema_CreateModelType, SCHEMA_HANDLE, schemaHandle, const char*, modelName);
DECLARE_GLOBAL_MOCK_METHOD_2(CMocksForCodeFirst, , SCHEMA_STRUCT_TYPE_HANDLE, Schema_CreateStructType, SCHEMA_HANDLE, schemaHandle, const char*, structTypeName);
DECLARE_GLOBAL_MOCK_METHOD_3(CMocksForCodeFirst, , SCHEMA_RESULT, Schema_AddStructTypeProperty, SCHEMA_STRUCT_TYPE_HANDLE, structTypeHandle, const char*, propertyName, const char*, propertyType);
DECLARE_GLOBAL_MOCK_METHOD_3(CMocksForCodeFirst, , SCHEMA_RESULT, Schema_AddModelProperty, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle, const char*, propertyName, const char*, propertyType);
DECLARE_GLOBAL_MOCK_METHOD_2(CMocksForCodeFirst, , SCHEMA_ACTION_HANDLE, Schema_CreateModelAction, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle, const char*, actionName);
DECLARE_GLOBAL_MOCK_METHOD_3(CMocksForCodeFirst, , SCHEMA_RESULT, Schema_AddModelActionArgument, SCHEMA_ACTION_HANDLE, actionHandle, const char*, argumentName, const char*, argumentType);
DECLARE_GLOBAL_MOCK_METHOD_1(CMocksForCodeFirst, , void, Schema_Destroy, SCHEMA_HANDLE, schemaHandle);
DECLARE_GLOBAL_MOCK_METHOD_1(CMocksForCodeFirst, , const char*, Schema_GetModelName, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle);
DECLARE_GLOBAL_MOCK_METHOD_2(CMocksForCodeFirst, , SCHEMA_MODEL_TYPE_HANDLE, Schema_GetModelByName, SCHEMA_HANDLE, schemaHandle, const char*, modelName);
DECLARE_GLOBAL_MOCK_METHOD_3(CMocksForCodeFirst, , SCHEMA_RESULT, Schema_AddModelModel, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle, const char*, propertyName, SCHEMA_MODEL_TYPE_HANDLE, modelType);
DECLARE_GLOBAL_MOCK_METHOD_1(CMocksForCodeFirst, , SCHEMA_RESULT, Schema_AddDeviceRef, SCHEMA_MODEL_TYPE_HANDLE, modelHandle);
DECLARE_GLOBAL_MOCK_METHOD_1(CMocksForCodeFirst, , SCHEMA_RESULT, Schema_ReleaseDeviceRef, SCHEMA_MODEL_TYPE_HANDLE, modelHandle);
DECLARE_GLOBAL_MOCK_METHOD_1(CMocksForCodeFirst, , SCHEMA_HANDLE, Schema_GetSchemaForModelType, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle);
DECLARE_GLOBAL_MOCK_METHOD_1(CMocksForCodeFirst, , SCHEMA_RESULT, Schema_DestroyIfUnused, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle);


typedef struct SimpleDevice_TAG
{
    unsigned char __SimpleDevice_begin;
    double this_is_double;
    int this_is_int;
} SimpleDevice;

static const WRAPPER_ARGUMENT setSpeedArguments[] = { { "double", "theSpeed" } };

static const REFLECTED_SOMETHING this_is_double_Property = { REFLECTION_PROPERTY_TYPE, NULL, { { 0 }, { 0 }, { "this_is_double", "double", Create_AGENT_DATA_TYPE_From_Ptr_this_is_double, offsetof(SimpleDevice, this_is_double), sizeof(double), "SimpleDevice"}, { 0 }, { 0 }} };
static const REFLECTED_SOMETHING this_is_int_Property = { REFLECTION_PROPERTY_TYPE, &this_is_double_Property, { { 0 }, { 0 }, { "this_is_int", "int", Create_AGENT_DATA_TYPE_From_Ptr_this_is_int, offsetof(SimpleDevice, this_is_int), sizeof(int), "SimpleDevice"}, { 0 }, { 0 }} };
static const REFLECTED_SOMETHING SimpleDevice_Model = { REFLECTION_MODEL_TYPE, &this_is_int_Property, { { 0 }, { 0 }, { 0 }, { 0 }, { "SimpleDevice"}} };
static const REFLECTED_SOMETHING whereIsMyDevice_Struct = { REFLECTION_STRUCT_TYPE, &SimpleDevice_Model, { { "GeoLocation" }, { 0 }, { 0 }, { 0 }, { 0 }} };
static const REFLECTED_SOMETHING Lat_Field = { REFLECTION_FIELD_TYPE, &whereIsMyDevice_Struct, { { 0 }, { "Lat", "double", "GeoLocation" }, { 0 }, { 0 }, { 0 }} };
static const REFLECTED_SOMETHING Long_Field = { REFLECTION_FIELD_TYPE, &Lat_Field, { { 0 }, { "Long", "double", "GeoLocation" }, { 0 }, { 0 }, { 0 }} };
static const REFLECTED_SOMETHING theCarIsBehindTheVan_Struct = { REFLECTION_STRUCT_TYPE, &Long_Field, { { "theCarIsBehindTheVan" }, { 0 }, { 0 }, { 0 }, { 0 } } };
static const REFLECTED_SOMETHING Alt_Field = { REFLECTION_FIELD_TYPE, &theCarIsBehindTheVan_Struct, { { 0 }, { "Alt", "int", "theCarIsBehindTheVan" }, { 0 }, { 0 }, { 0 }} };
static const REFLECTED_SOMETHING whereIsMyCar_Field = { REFLECTION_FIELD_TYPE, &Alt_Field, { { 0 }, { "whereIsMyCar", "GeoLocation", "theCarIsBehindTheVan" }, { 0 }, { 0 }, { 0 }} };
static const REFLECTED_SOMETHING setSpeed_Action = { REFLECTION_ACTION_TYPE, &whereIsMyCar_Field, { { 0 }, { 0 }, { 0 }, { "setSpeed", 1, setSpeedArguments, NULL, "TruckType" }, { 0 }} };
static const REFLECTED_SOMETHING reset_Action = { REFLECTION_ACTION_TYPE, &setSpeed_Action, { { 0 }, { 0 }, { 0 }, { "reset", 0, NULL, NULL, "TruckType" }, { 0 }} };
static const REFLECTED_SOMETHING truckType_Model = { REFLECTION_MODEL_TYPE, &reset_Action, { { 0 }, { 0 }, { 0 }, { 0 }, { "TruckType"}} };
const REFLECTED_DATA_FROM_DATAPROVIDER testReflectedData = { &truckType_Model };


typedef struct InnerType_TAG
{
    unsigned char __InnerType_begin;
    double this_is_double;
    int this_is_int;
} InnerType;

typedef struct OuterType_TAG
{
    unsigned char __OuterType_begin;
    InnerType Inner;
    int this_is_int;
} OuterType;

void* OuterType_reset_device;
EXECUTE_COMMAND_RESULT OuterType_reset(void* device, size_t ParameterCount, const AGENT_DATA_TYPE* values)
{
    (void)ParameterCount;
    (void)values;
    OuterType_reset_device = device;
    return EXECUTE_COMMAND_SUCCESS;
}

void* InnerType_reset_device;
EXECUTE_COMMAND_RESULT InnerType_reset(void* device, size_t ParameterCount, const AGENT_DATA_TYPE* values)
{
    (void)ParameterCount;
    (void)values;
    InnerType_reset_device = device;
    return EXECUTE_COMMAND_SUCCESS;
}

static const SCHEMA_MODEL_TYPE_HANDLE TEST_INNERTYPE_MODEL_HANDLE = (SCHEMA_MODEL_TYPE_HANDLE)0x5555;
static const SCHEMA_MODEL_TYPE_HANDLE TEST_OUTERTYPE_MODEL_HANDLE = (SCHEMA_MODEL_TYPE_HANDLE)0x5556;

static const REFLECTED_SOMETHING OuterType_reset_Action = { REFLECTION_ACTION_TYPE, NULL, { { 0 }, { 0 }, { 0 }, { "reset", 0, NULL, OuterType_reset, "OuterType" }, { 0 } } };
static const REFLECTED_SOMETHING InnerType_reset_Action = { REFLECTION_ACTION_TYPE, &OuterType_reset_Action, { { 0 }, { 0 }, { 0 }, { "reset", 0, NULL, InnerType_reset, "InnerType" }, { 0 } } };
static const REFLECTED_SOMETHING this_is_int_Property_2 = { REFLECTION_PROPERTY_TYPE, &InnerType_reset_Action, { { 0 }, { 0 }, { "this_is_int", "int", Create_AGENT_DATA_TYPE_From_Ptr_this_is_int, offsetof(InnerType, this_is_int), sizeof(int), "InnerType" }, { 0 }, { 0 } } };
static const REFLECTED_SOMETHING this_is_double_Property_2 = { REFLECTION_PROPERTY_TYPE, &this_is_int_Property_2, { { 0 }, { 0 }, { "this_is_double", "double", Create_AGENT_DATA_TYPE_From_Ptr_this_is_double, offsetof(InnerType, this_is_double), sizeof(double), "InnerType"}, { 0 }, { 0 }} };
static const REFLECTED_SOMETHING InnerType_Model = { REFLECTION_MODEL_TYPE, &this_is_double_Property_2, { { 0 }, { 0 }, { 0 }, { 0 }, { "InnerType"}} };
static const REFLECTED_SOMETHING Inner_Property = { REFLECTION_PROPERTY_TYPE, &InnerType_Model, { { 0 }, { 0 }, { "Inner", "InnerType", Create_AGENT_DATA_TYPE_From_Ptr_this_is_double, offsetof(OuterType, Inner), sizeof(InnerType), "OuterType"}, { 0 }, { 0 }} };
static const REFLECTED_SOMETHING OuterType_Model = { REFLECTION_MODEL_TYPE, &Inner_Property, { { 0 }, { 0 }, { 0 }, { 0 }, { "OuterType"}} };
const REFLECTED_DATA_FROM_DATAPROVIDER testModelInModelReflectedData = { &OuterType_Model };


static unsigned char edmBinarySource[] = { 1, 42, 43, 44, 1 };

static MICROMOCK_GLOBAL_SEMAPHORE_HANDLE g_dllByDll;

BEGIN_TEST_SUITE(CodeFirst_ut_Dummy_Data_Provider)

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

        someEdmDateTimeOffset.dateTime.tm_year = 2014 - 1900;
        someEdmDateTimeOffset.dateTime.tm_mon = 1 - 1;
        someEdmDateTimeOffset.dateTime.tm_mday = 2;
        someEdmDateTimeOffset.dateTime.tm_hour = 3;
        someEdmDateTimeOffset.dateTime.tm_min = 4;
        someEdmDateTimeOffset.dateTime.tm_sec = 5;
        someEdmDateTimeOffset.hasFractionalSecond = 1;
        someEdmDateTimeOffset.fractionalSecond = 666666666666;
        someEdmDateTimeOffset.hasTimeZone = 1;
        someEdmDateTimeOffset.timeZoneHour = 7;
        someEdmDateTimeOffset.timeZoneMinute = 8;

        const EDM_GUID source= { { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 } };
        memcpy(someEdmGuid.GUID, source.GUID, 16);

        
        someEdmBinary.size = 5;
        someEdmBinary.data = edmBinarySource;

        someDouble.type = EDM_DOUBLE_TYPE;
        someDouble.value.edmDouble.value = 0.42;

        someInt32.type = EDM_INT32_TYPE;
        someInt32.value.edmInt32.value = -34;

        InnerType_reset_device = NULL;
        OuterType_reset_device = NULL;

        DummyDataProvider_reset_wasCalled = false;

        DummyDataProvider_setSpeed_wasCalled = false;
        DummyDataProvider_setSpeed_theSpeed = -1.0;

        DummyDataProvider_test1_wasCalled = false;
        DummyDataProvider_test1_P1 = -1.0;
        DummyDataProvider_test1_P2 = -1;
        DummyDataProvider_test1_P3 = -1.0f;
        DummyDataProvider_test1_P4 = -1;
        DummyDataProvider_test1_P5 = -1;
        DummyDataProvider_test1_P6 = 255;
        DummyDataProvider_test1_P7 = -1;
        DummyDataProvider_test1_P8 = -1;
        DummyDataProvider_test1_P9 = -1;
        memset(DummyDataProvider_test1_P11, 0, P11_MAX_SIZE);
        DummyDataProvider_test1_P10 = false;
        free(DummyDataProvider_test1_P14.data);
        DummyDataProvider_test1_P14.data = NULL;
        DummyDataProvider_test1_P14.size= 0;

        arrayOfAgentDataType[0].type = EDM_DOUBLE_TYPE;
        arrayOfAgentDataType[0].value.edmDouble.value = 42.0;
        arrayOfAgentDataType[1].type = EDM_INT32_TYPE;
        arrayOfAgentDataType[1].value.edmInt32.value = 4;
        arrayOfAgentDataType[2].type = EDM_SINGLE_TYPE;
        arrayOfAgentDataType[2].value.edmSingle.value = 24.0f;
        arrayOfAgentDataType[3].type = EDM_INT64_TYPE;
        arrayOfAgentDataType[3].value.edmInt32.value = 2;
        arrayOfAgentDataType[4].type = EDM_SBYTE_TYPE;
        arrayOfAgentDataType[4].value.edmSbyte.value = 3;
        arrayOfAgentDataType[5].type = EDM_BYTE_TYPE;
        arrayOfAgentDataType[5].value.edmByte.value = 4;
        arrayOfAgentDataType[6].type = EDM_INT16_TYPE;
        arrayOfAgentDataType[6].value.edmInt16.value = 5;
        arrayOfAgentDataType[7].type = EDM_INT32_TYPE;
        arrayOfAgentDataType[7].value.edmInt32.value = 6;
        arrayOfAgentDataType[8].type = EDM_INT64_TYPE;
        arrayOfAgentDataType[8].value.edmInt64.value = 7;
        arrayOfAgentDataType[9].type = EDM_BOOLEAN_TYPE;
        arrayOfAgentDataType[9].value.edmBoolean.value = EDM_TRUE;
        arrayOfAgentDataType[10].type = EDM_STRING_TYPE;
        arrayOfAgentDataType[10].value.edmString.chars = someChars;
        arrayOfAgentDataType[14].value.edmString.length = strlen(someChars);
        arrayOfAgentDataType[11].type = EDM_DATE_TIME_OFFSET_TYPE;
        arrayOfAgentDataType[11].value.edmDateTimeOffset = someEdmDateTimeOffset;
        arrayOfAgentDataType[12].type = EDM_GUID_TYPE;
        arrayOfAgentDataType[12].value.edmGuid = someEdmGuid;
        arrayOfAgentDataType[13].type = EDM_BINARY_TYPE;
        arrayOfAgentDataType[13].value.edmBinary = someEdmBinary;
        arrayOfAgentDataType[14].type = EDM_STRING_NO_QUOTES_TYPE;
        arrayOfAgentDataType[14].value.edmStringNoQuotes.chars = someMoreChars;
        arrayOfAgentDataType[14].value.edmStringNoQuotes.length = strlen(someMoreChars);

        CMocksForCodeFirst mocks;
        mocks.SetPerformAutomaticCallComparison(AUTOMATIC_CALL_COMPARISON_OFF);
        (void)CodeFirst_Init(NULL);

    }

    TEST_FUNCTION_CLEANUP(TestMethodCleanup)
    {
        free(DummyDataProvider_test1_P14.data);
        DummyDataProvider_test1_P14.data = NULL;
        DummyDataProvider_test1_P14.size = 0;

        CMocksForCodeFirst mocks;
        mocks.SetPerformAutomaticCallComparison(AUTOMATIC_CALL_COMPARISON_OFF);
        CodeFirst_Deinit();
        if (!MicroMockReleaseMutex(g_testByTest))
        {
            ASSERT_FAIL("failure in test framework at ReleaseMutex");
        }
    }

    TEST_FUNCTION(C_CPLUSPLUS_SANITY_CHECKS)
    {
        ///arrange

        ///act
        size_t C_has_sizeof_bool = sizeof_bool();

        ///assert
        ASSERT_ARE_EQUAL(size_t, sizeof(bool), C_has_sizeof_bool);
    }

    /*Tests_CODEFIRST_002:[ CodeFirst_RegisterSchema shall create the schema information and give it to the Schema module for one schema, identified by the metadata argument. On success, it shall return a handle to the model.]*/
    TEST_FUNCTION(CodeFirst_Init_succeds)
    {
        CMocksForCodeFirst mocks;

        ///arrange
        CODEFIRST_RESULT result;
        CodeFirst_Deinit();
        mocks.ResetAllCalls();

        ///act
        result = CodeFirst_Init(NULL);

        ///assert
        ASSERT_ARE_EQUAL(CODEFIRST_RESULT, CODEFIRST_OK, result);
    }

    /*Tests_SRS_CODEFIRST_99_003:[ If the module is already initialized, the initialization shall fail and the return value shall be CODEFIRST_ALREADY_INIT.]*/
    TEST_FUNCTION(CodeFirst_Init_after_Init_fails)
    {
        ///arrange
        CMocksForCodeFirst mocks;
        CODEFIRST_RESULT result;
        (void)CodeFirst_Init(NULL);

        mocks.ResetAllCalls();

        ///act
        result = CodeFirst_Init(NULL);

        ///assert
        ASSERT_ARE_EQUAL(CODEFIRST_RESULT, CODEFIRST_ALREADY_INIT, result);
    }

    /* CodeFirst_InvokeAction */

    /*Tests_SRS_CODEFIRST_99_066:[ If actionName, relativeActionPath or deviceHandle is NULL then EXECUTE_COMMAND_ERROR shall be returned*/
    TEST_FUNCTION(CodeFirst_InvokeAction_with_NULL_action_fails)
    {
        ///arrange
        CMocksForCodeFirst mocks;
        (void)CodeFirst_Init(NULL);
        mocks.ResetAllCalls();

        ///act
       auto result = CodeFirst_InvokeAction(TEST_DEVICE_HANDLE, NULL, "",  NULL, 1, &someDouble); 

        ///assert
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
    }

    /*Tests_SRS_CODEFIRST_99_066:[ If actionName, relativeActionPath or deviceHandle is NULL then EXECUTE_COMMAND_ERROR shall be returned*/
    TEST_FUNCTION(CodeFirst_InvokeAction_with_NULL_DeviceHandle_fails)
    {
        ///arrange
        CMocksForCodeFirst mocks;
        (void)CodeFirst_Init(NULL);
        mocks.ResetAllCalls();

        ///act
       auto result = CodeFirst_InvokeAction(NULL, NULL, "", "reset", 1, &someDouble);

        ///assert
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
    }

    /*Tests_SRS_CODEFIRST_99_066:[ If actionName, relativeActionPath or deviceHandle is NULL then EXECUTE_COMMAND_ERROR shall be returned*/
    TEST_FUNCTION(CodeFirst_InvokeAction_With_NULL_Relative_Path_Fails)
    {
        ///arrange
        CMocksForCodeFirst mocks;
        (void)CodeFirst_Init(NULL);
        void* device = CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &DummyDataProvider_allReflected, sizeof(TruckType), false);
        mocks.ResetAllCalls();

        ///act
       auto result = CodeFirst_InvokeAction(TEST_DEVICE_HANDLE, g_InvokeActionCallbackArgument, NULL, "reset", 0, NULL);

        ///assert
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CodeFirst_DestroyDevice(device);
    }

    /*Tests_SRS_CODEFIRST_99_068:[ If the function is called before CodeFirst is initialized then EXECUTE_COMMAND_ERROR shall be returned.] */
    TEST_FUNCTION(CodeFirst_InvokeAction_When_Not_Initialized_Fails)
    {
        ///arrange
        CMocksForCodeFirst mocks;
        CodeFirst_Deinit();
        mocks.ResetAllCalls();

        ///act
       auto result = CodeFirst_InvokeAction(TEST_DEVICE_HANDLE, NULL, "", "reset", 1, &someDouble);

        ///assert
       ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
    }

    /*Tests_SRS_CODEFIRST_99_067:[ If parameterCount is greater than zero and parameterValues is NULL then EXECUTE_COMMAND_ERROR shall be returned.]*/
    /*Tests_SRS_SERIALIZER_99_039:[ WITH_ACTION shall declare an action of the current data provider called as the first macro parameter (name) and having the first parameter called parameter1Name of type parameter1Type, the second parameter named parameter2Name having the type parameter2Type and so on. ]*/
    TEST_FUNCTION(CodeFirst_InvokeAction_with_NULL_parameterValues_and_positive_count_fails)
    {
        ///arrange
        CMocksForCodeFirst mocks;
        (void)CodeFirst_Init(NULL);
        mocks.ResetAllCalls();

        ///act
       auto result = CodeFirst_InvokeAction(TEST_DEVICE_HANDLE, NULL, "", "reset", 1, NULL);

        ///assert
       ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
    }

    /*Tests_SRS_CODEFIRST_99_078:[If such a function is not found then the function shall return EXECUTE_COMMAND_ERROR.]*/
    TEST_FUNCTION(CodeFirst_InvokeAction_with_unknown_actions_fails)
    {
        ///arrange
        CMocksForCodeFirst mocks;
        (void)CodeFirst_Init(NULL);
        void* device = CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &DummyDataProvider_allReflected, sizeof(TruckType), false);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Schema_GetModelName(TEST_MODEL_HANDLE)).SetReturn("TruckType");

        ///act
       auto result = CodeFirst_InvokeAction(TEST_DEVICE_HANDLE, g_InvokeActionCallbackArgument, "", "asjnhfaslk", 1, &someDouble);

        ///assert
       ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CodeFirst_DestroyDevice(device);
    }

    /*Tests_SRS_CODEFIRST_99_063:[ If the function is found, then CodeFirst shall call the wrapper of the found function inside the data provider. The wrapper is linked in the reflected data to the function name. The wrapper shall be called with the same arguments as CodeFirst_InvokeAction has been called.]*/
    /*Tests_SRS_SERIALIZER_99_039:[ WITH_ACTION shall declare an action of the current data provider called as the first macro parameter (name) and having the first parameter called parameter1Name of type parameter1Type, the second parameter named parameter2Name having the type parameter2Type and so on. ]*/
    /*Tests_SRS_SERIALIZER_99_040:[ In addition to declaring the function, DECLARE_IOT_METHOD shall provide a definition for a wrapper that takes as parameters a size_t parameterCount and const AGENT_DATA_TYPE*.] */
    /*Tests_SRS_SERIALIZER_99_041:[ This wrapper shall convert all the arguments to predefined types and then call the function written by the data provider developer.]*/
    /*Tests_SRS_SERIALIZER_99_043:[ It is valid for a method not to have any parameters.]*/
    TEST_FUNCTION(CodeFirst_InvokeAction_calls_reset_action_succeeds)
    {
        ///arrange
        CMocksForCodeFirst mocks;
        (void)CodeFirst_Init(NULL);
        void* device = CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &DummyDataProvider_allReflected, sizeof(TruckType), false);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Schema_GetModelName(TEST_MODEL_HANDLE)).SetReturn("TruckType");

        ///act
       auto result = CodeFirst_InvokeAction(TEST_DEVICE_HANDLE, g_InvokeActionCallbackArgument, "", "reset", 0, NULL);

        ///assert
       ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_SUCCESS, result);
        ASSERT_ARE_EQUAL(bool, true, DummyDataProvider_reset_wasCalled);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CodeFirst_DestroyDevice(device);
    }

    /*Tests_SRS_CODEFIRST_99_063:[ If the function is found, then CodeFirst shall call the wrapper of the found function inside the data provider. The wrapper is linked in the reflected data to the function name. The wrapper shall be called with the same arguments as CodeFirst_InvokeAction has been called.]*/
    /*Tests_SRS_SERIALIZER_99_045:[ If the number of passed parameters doesn't match the number of declared parameters, wrapper execution shall fail and return DATA_PROVIDER_INVALID_ARG;]*/
    /*Tests_SRS_CODEFIRST_99_065:[ For all the other return values CODEFIRST_ACTION_EXECUTION_ERROR shall be returned.]*/
    /*Tests_SRS_SERIALIZER_99_039:[ WITH_ACTION shall declare an action of the current data provider called as the first macro parameter (name) and having the first parameter called parameter1Name of type parameter1Type, the second parameter named parameter2Name having the type parameter2Type and so on. ]*/
    TEST_FUNCTION(CodeFirst_InvokeAction_calling_setSpeed_action_with_insuficcient_parameters_fails)
    {
        ///arrange
        CMocksForCodeFirst mocks;
        (void)CodeFirst_Init(NULL);
        void* device = CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &DummyDataProvider_allReflected, sizeof(TruckType), false);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Schema_GetModelName(TEST_MODEL_HANDLE)).SetReturn("TruckType");

        ///act
       auto result = CodeFirst_InvokeAction(TEST_DEVICE_HANDLE, g_InvokeActionCallbackArgument, "", "setSpeed", 0, NULL);

        ///assert
        ASSERT_ARE_EQUAL(bool, false, DummyDataProvider_setSpeed_wasCalled);
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CodeFirst_DestroyDevice(device);
    }

    /*Tests_SRS_CODEFIRST_99_063:[ If the function is found, then CodeFirst shall call the wrapper of the found function inside the data provider. The wrapper is linked in the reflected data to the function name. The wrapper shall be called with the same arguments as CodeFirst_InvokeAction has been called.]*/
    /*Tests_SRS_SERIALIZER_99_045:[ If the number of passed parameters doesn't match the number of declared parameters, wrapper execution shall fail and return DATA_PROVIDER_INVALID_ARG;]*/
    /*Tests_SRS_CODEFIRST_99_065:[ For all the other return values CODEFIRST_ACTION_EXECUTION_ERROR shall be returned.]*/
    /*Tests_SRS_SERIALIZER_99_039:[ WITH_ACTION shall declare an action of the current data provider called as the first macro parameter (name) and having the first parameter called parameter1Name of type parameter1Type, the second parameter named parameter2Name having the type parameter2Type and so on. ]*/
    TEST_FUNCTION(CodeFirst_InvokeAction_calling_setSpeed_action_with_NULL_ParameterValues_fails)
    {
        ///arrange
        CMocksForCodeFirst mocks;
        (void)CodeFirst_Init(NULL);
        void* device = CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &DummyDataProvider_allReflected, sizeof(TruckType), false);
        mocks.ResetAllCalls();

        ///act
       auto result = CodeFirst_InvokeAction(TEST_DEVICE_HANDLE, g_InvokeActionCallbackArgument, "", "setSpeed", 1, NULL);

        ///assert
        ASSERT_ARE_EQUAL(bool, false, DummyDataProvider_setSpeed_wasCalled);
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CodeFirst_DestroyDevice(device);
    }

    /*Tests_SRS_CODEFIRST_99_063:[ If the function is found, then CodeFirst shall call the wrapper of the found function inside the data provider. The wrapper is linked in the reflected data to the function name. The wrapper shall be called with the same arguments as CodeFirst_InvokeAction has been called.]*/
    /*Tests_SRS_SERIALIZER_99_045:[ If the number of passed parameters doesn't match the number of declared parameters, wrapper execution shall fail and return DATA_PROVIDER_INVALID_ARG;]*/
    /*Tests_SRS_CODEFIRST_99_065:[ For all the other return values CODEFIRST_ACTION_EXECUTION_ERROR shall be returned.]*/
    /*Tests_SRS_SERIALIZER_99_039:[ WITH_ACTION shall declare an action of the current data provider called as the first macro parameter (name) and having the first parameter called parameter1Name of type parameter1Type, the second parameter named parameter2Name having the type parameter2Type and so on. ]*/
    TEST_FUNCTION(CodeFirst_InvokeAction_calling_setSpeed_action_with_too_many_parameters_fails)
    {
        ///arrange
        CMocksForCodeFirst mocks;
        (void)CodeFirst_Init(NULL);
        void* device = CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &DummyDataProvider_allReflected, sizeof(TruckType), false);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Schema_GetModelName(TEST_MODEL_HANDLE)).SetReturn("TruckType");

        ///act
       auto result = CodeFirst_InvokeAction(TEST_DEVICE_HANDLE, g_InvokeActionCallbackArgument, "", "setSpeed", 2, arrayOfAgentDataType);

        ///assert
        ASSERT_ARE_EQUAL(bool, false, DummyDataProvider_setSpeed_wasCalled);
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CodeFirst_DestroyDevice(device);
    }

    /*Tests_SRS_CODEFIRST_99_063:[ If the function is found, then CodeFirst shall call the wrapper of the found function inside the data provider. The wrapper is linked in the reflected data to the function name. The wrapper shall be called with the same arguments as CodeFirst_InvokeAction has been called.]*/
    /*Tests_SRS_SERIALIZER_99_045:[ If the number of passed parameters doesn't match the number of declared parameters, wrapper execution shall fail and return DATA_PROVIDER_INVALID_ARG;]*/
    /*Tests_SRS_CODEFIRST_99_065:[ For all the other return values CODEFIRST_ACTION_EXECUTION_ERROR shall be returned.]*/
    /*Tests_SRS_SERIALIZER_99_046:[ If the types of the parameters do not match the declared types, DATAPROVIDER_INVALID_ARG shall be returned.]*/
    /*Tests_SRS_SERIALIZER_99_039:[ WITH_ACTION shall declare an action of the current data provider called as the first macro parameter (name) and having the first parameter called parameter1Name of type parameter1Type, the second parameter named parameter2Name having the type parameter2Type and so on. ]*/
    TEST_FUNCTION(CodeFirst_InvokeAction_calling_setSpeed_with_invalid_type_for_Parameter1_fails)
    {
        ///arrange
        CMocksForCodeFirst mocks;
        (void)CodeFirst_Init(NULL);
        void* device = CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &DummyDataProvider_allReflected, sizeof(TruckType), false);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Schema_GetModelName(TEST_MODEL_HANDLE)).SetReturn("TruckType");

        ///act
       auto result = CodeFirst_InvokeAction(TEST_DEVICE_HANDLE, g_InvokeActionCallbackArgument, "", "setSpeed", 1, &someInt32);

        ///assert
        ASSERT_ARE_EQUAL(bool, false, DummyDataProvider_setSpeed_wasCalled);
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CodeFirst_DestroyDevice(device);
    }

    /*Tests_SRS_CODEFIRST_99_063:[ If the function is found, then CodeFirst shall call the wrapper of the found function inside the data provider. The wrapper is linked in the reflected data to the function name. The wrapper shall be called with the same arguments as CodeFirst_InvokeAction has been called.]*/
    /*Tests_SRS_SERIALIZER_99_045:[ If the number of passed parameters doesn't match the number of declared parameters, wrapper execution shall fail and return DATA_PROVIDER_INVALID_ARG;]*/
    /*Tests_SRS_CODEFIRST_99_065:[ For all the other return values CODEFIRST_ACTION_EXECUTION_ERROR shall be returned.]*/
    /*Tests_SRS_CODEFIRST_99_064:[ If the wrapper call succeeds then CODEFIRST_OK shall be returned. ]*/
    /*Tests_SRS_SERIALIZER_99_039:[ WITH_ACTION shall declare an action of the current data provider called as the first macro parameter (name) and having the first parameter called parameter1Name of type parameter1Type, the second parameter named parameter2Name having the type parameter2Type and so on. ]*/
    /*Tests_SRS_SERIALIZER_99_040:[ In addition to declaring the function, DECLARE_IOT_METHOD shall provide a definition for a wrapper that takes as parameters a size_t parameterCount and const AGENT_DATA_TYPE*.] */
    /*Tests_SRS_SERIALIZER_99_041:[ This wrapper shall convert all the arguments to predefined types and then call the function written by the data provider developer.]*/
    TEST_FUNCTION(CodeFirst_InvokeAction_calling_setSpeed_succeeds)
    {
        ///arrange
        CMocksForCodeFirst mocks;
        (void)CodeFirst_Init(NULL);
        void* device = CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &DummyDataProvider_allReflected, sizeof(TruckType), false);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Schema_GetModelName(TEST_MODEL_HANDLE)).SetReturn("TruckType");

        ///act
       auto result = CodeFirst_InvokeAction(TEST_DEVICE_HANDLE, g_InvokeActionCallbackArgument, "", "setSpeed", 1, &someDouble);

        ///assert
        ASSERT_ARE_EQUAL(bool, true, DummyDataProvider_setSpeed_wasCalled);
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_SUCCESS, result);
        ASSERT_ARE_EQUAL(double, 0.42, DummyDataProvider_setSpeed_theSpeed);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CodeFirst_DestroyDevice(device);
    }

    /*from here on there are tests for "test1"*/
    /*Tests_SRS_SERIALIZER_99_042:[The parameter types are either predefined parameter types(specs SRS_SERIALIZER_99_004 - SRS_SERIALIZER_99_014)*/
    /*Tests_SRS_SERIALIZER_99_039:[ WITH_ACTION shall declare an action of the current data provider called as the first macro parameter (name) and having the first parameter called parameter1Name of type parameter1Type, the second parameter named parameter2Name having the type parameter2Type and so on. ]*/
    TEST_FUNCTION(CodeFirst_InvokeAction_calling_test1_with_insufficient_parameters_fails)
    {
        ///arrange
        CMocksForCodeFirst mocks;
        (void)CodeFirst_Init(NULL);
        void* device = CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &DummyDataProvider_allReflected, sizeof(TruckType), false);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Schema_GetModelName(TEST_MODEL_HANDLE)).SetReturn("TruckType");

        ///act
       auto result = CodeFirst_InvokeAction(TEST_DEVICE_HANDLE, g_InvokeActionCallbackArgument, "", "test1", 13, arrayOfAgentDataType);

        ///assert
        ASSERT_ARE_EQUAL(bool, false, DummyDataProvider_test1_wasCalled);
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CodeFirst_DestroyDevice(device);
    }

    /*Tests_SRS_SERIALIZER_99_042:[The parameter types are either predefined parameter types(specs SRS_SERIALIZER_99_004 - SRS_SERIALIZER_99_014)*/
    /*Tests_SRS_SERIALIZER_99_039:[ WITH_ACTION shall declare an action of the current data provider called as the first macro parameter (name) and having the first parameter called parameter1Name of type parameter1Type, the second parameter named parameter2Name having the type parameter2Type and so on. ]*/
    TEST_FUNCTION(CodeFirst_InvokeAction_calling_test1_with_too_many_parameters_fails)
    {
        ///arrange
        CMocksForCodeFirst mocks;
        (void)CodeFirst_Init(NULL);
        void* device = CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &DummyDataProvider_allReflected, sizeof(TruckType), false);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Schema_GetModelName(TEST_MODEL_HANDLE)).SetReturn("TruckType");

        ///act
       auto result = CodeFirst_InvokeAction(TEST_DEVICE_HANDLE, g_InvokeActionCallbackArgument, "", "test1", 16, arrayOfAgentDataType);

        ///assert
        ASSERT_ARE_EQUAL(bool, false, DummyDataProvider_test1_wasCalled);
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CodeFirst_DestroyDevice(device);
    }

    /*Tests_SRS_SERIALIZER_99_042:[The parameter types are either predefined parameter types(specs SRS_SERIALIZER_99_004 - SRS_SERIALIZER_99_014)*/
    /*Tests_SRS_SERIALIZER_99_039:[ WITH_ACTION shall declare an action of the current data provider called as the first macro parameter (name) and having the first parameter called parameter1Name of type parameter1Type, the second parameter named parameter2Name having the type parameter2Type and so on. ]*/
    TEST_FUNCTION(CodeFirst_InvokeAction_calling_test1_with_NULL_parameterValues_fails)
    {
        ///arrange
        CMocksForCodeFirst mocks;
        (void)CodeFirst_Init(NULL);
        void* device = CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &DummyDataProvider_allReflected, sizeof(TruckType), false);
        mocks.ResetAllCalls();

        ///act
       auto result = CodeFirst_InvokeAction(TEST_DEVICE_HANDLE, g_InvokeActionCallbackArgument, "", "test1", 11, NULL);

        ///assert
        ASSERT_ARE_EQUAL(bool, false, DummyDataProvider_test1_wasCalled);
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CodeFirst_DestroyDevice(device);
    }

    /*Tests_SRS_SERIALIZER_99_042:[The parameter types are either predefined parameter types(specs SRS_SERIALIZER_99_004 - SRS_SERIALIZER_99_014)*/
    /*Tests_SRS_SERIALIZER_99_039:[ WITH_ACTION shall declare an action of the current data provider called as the first macro parameter (name) and having the first parameter called parameter1Name of type parameter1Type, the second parameter named parameter2Name having the type parameter2Type and so on. ]*/
    TEST_FUNCTION(CodeFirst_InvokeAction_calling_test1_with_parameter_1_of_wrong_type_fails)
    {
        ///arrange
        arrayOfAgentDataType[0].type = EDM_INT32_TYPE;
        CMocksForCodeFirst mocks;
        (void)CodeFirst_Init(NULL);
        void* device = CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &DummyDataProvider_allReflected, sizeof(TruckType), false);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Schema_GetModelName(TEST_MODEL_HANDLE)).SetReturn("TruckType");

        ///act
       auto result = CodeFirst_InvokeAction(TEST_DEVICE_HANDLE, g_InvokeActionCallbackArgument, "", "test1", 12, arrayOfAgentDataType);

        ///assert
        ASSERT_ARE_EQUAL(bool, false, DummyDataProvider_test1_wasCalled);
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CodeFirst_DestroyDevice(device);
    }

    /*Tests_SRS_SERIALIZER_99_042:[The parameter types are either predefined parameter types(specs SRS_SERIALIZER_99_004 - SRS_SERIALIZER_99_014)*/
    /*Tests_SRS_SERIALIZER_99_039:[ WITH_ACTION shall declare an action of the current data provider called as the first macro parameter (name) and having the first parameter called parameter1Name of type parameter1Type, the second parameter named parameter2Name having the type parameter2Type and so on. ]*/
    TEST_FUNCTION(CodeFirst_InvokeAction_calling_test1_with_parameter_2_of_wrong_type_fails)
    {
        ///arrange
        arrayOfAgentDataType[1].type = EDM_DOUBLE_TYPE;
        CMocksForCodeFirst mocks;
        (void)CodeFirst_Init(NULL);
        void* device = CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &DummyDataProvider_allReflected, sizeof(TruckType), false);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Schema_GetModelName(TEST_MODEL_HANDLE)).SetReturn("TruckType");

        ///act
       auto result = CodeFirst_InvokeAction(TEST_DEVICE_HANDLE, g_InvokeActionCallbackArgument, "", "test1", 12, arrayOfAgentDataType);

        ///assert
        ASSERT_ARE_EQUAL(bool, false, DummyDataProvider_test1_wasCalled);
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CodeFirst_DestroyDevice(device);
    }

    /*Tests_SRS_SERIALIZER_99_042:[The parameter types are either predefined parameter types(specs SRS_SERIALIZER_99_004 - SRS_SERIALIZER_99_014)*/
    /*Tests_SRS_SERIALIZER_99_039:[ WITH_ACTION shall declare an action of the current data provider called as the first macro parameter (name) and having the first parameter called parameter1Name of type parameter1Type, the second parameter named parameter2Name having the type parameter2Type and so on. ]*/
    TEST_FUNCTION(CodeFirst_InvokeAction_calling_test1_with_parameter_3_of_wrong_type_fails)
    {
        ///arrange
        arrayOfAgentDataType[2].type = EDM_DOUBLE_TYPE;
        CMocksForCodeFirst mocks;
        (void)CodeFirst_Init(NULL);
        void* device = CodeFirst_CreateDevice(TEST_MODEL_HANDLE,  &DummyDataProvider_allReflected, sizeof(TruckType), false);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Schema_GetModelName(TEST_MODEL_HANDLE)).SetReturn("TruckType");

        ///act
       auto result = CodeFirst_InvokeAction(TEST_DEVICE_HANDLE, g_InvokeActionCallbackArgument, "", "test1", 12, arrayOfAgentDataType);

        ///assert
        ASSERT_ARE_EQUAL(bool, false, DummyDataProvider_test1_wasCalled);
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CodeFirst_DestroyDevice(device);
    }

    /*Tests_SRS_SERIALIZER_99_042:[The parameter types are either predefined parameter types(specs SRS_SERIALIZER_99_004 - SRS_SERIALIZER_99_014)*/
    /*Tests_SRS_SERIALIZER_99_039:[ WITH_ACTION shall declare an action of the current data provider called as the first macro parameter (name) and having the first parameter called parameter1Name of type parameter1Type, the second parameter named parameter2Name having the type parameter2Type and so on. ]*/
    TEST_FUNCTION(CodeFirst_InvokeAction_calling_test1_with_parameter_4_of_wrong_type_fails)
    {
        ///arrange
        arrayOfAgentDataType[3].type = EDM_DOUBLE_TYPE;
        CMocksForCodeFirst mocks;
        (void)CodeFirst_Init(NULL);
        void* device = CodeFirst_CreateDevice(TEST_MODEL_HANDLE,  &DummyDataProvider_allReflected, sizeof(TruckType), false);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Schema_GetModelName(TEST_MODEL_HANDLE)).SetReturn("TruckType");

        ///act
       auto result = CodeFirst_InvokeAction(TEST_DEVICE_HANDLE, g_InvokeActionCallbackArgument, "", "test1", 12, arrayOfAgentDataType);

        ///assert
        ASSERT_ARE_EQUAL(bool, false, DummyDataProvider_test1_wasCalled);
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CodeFirst_DestroyDevice(device);
    }

    /*Tests_SRS_SERIALIZER_99_042:[The parameter types are either predefined parameter types(specs SRS_SERIALIZER_99_004 - SRS_SERIALIZER_99_014)*/
    /*Tests_SRS_SERIALIZER_99_039:[ WITH_ACTION shall declare an action of the current data provider called as the first macro parameter (name) and having the first parameter called parameter1Name of type parameter1Type, the second parameter named parameter2Name having the type parameter2Type and so on. ]*/
    TEST_FUNCTION(CodeFirst_InvokeAction_calling_test1_with_parameter_5_of_wrong_type_fails)
    {
        ///arrange
        arrayOfAgentDataType[4].type = EDM_DOUBLE_TYPE;
        CMocksForCodeFirst mocks;
        (void)CodeFirst_Init(NULL);
        void* device = CodeFirst_CreateDevice(TEST_MODEL_HANDLE,  &DummyDataProvider_allReflected, sizeof(TruckType), false);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Schema_GetModelName(TEST_MODEL_HANDLE)).SetReturn("TruckType");

        ///act
       auto result = CodeFirst_InvokeAction(TEST_DEVICE_HANDLE, g_InvokeActionCallbackArgument, "", "test1", 12, arrayOfAgentDataType);

        ///assert
        ASSERT_ARE_EQUAL(bool, false, DummyDataProvider_test1_wasCalled);
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CodeFirst_DestroyDevice(device);
    }

    /*Tests_SRS_SERIALIZER_99_042:[The parameter types are either predefined parameter types(specs SRS_SERIALIZER_99_004 - SRS_SERIALIZER_99_014)*/
    /*Tests_SRS_SERIALIZER_99_039:[ WITH_ACTION shall declare an action of the current data provider called as the first macro parameter (name) and having the first parameter called parameter1Name of type parameter1Type, the second parameter named parameter2Name having the type parameter2Type and so on. ]*/
    TEST_FUNCTION(CodeFirst_InvokeAction_calling_test1_with_parameter_6_of_wrong_type_fails)
    {
        ///arrange
        arrayOfAgentDataType[5].type = EDM_DOUBLE_TYPE;
        CMocksForCodeFirst mocks;
        (void)CodeFirst_Init(NULL);
        void* device = CodeFirst_CreateDevice(TEST_MODEL_HANDLE,  &DummyDataProvider_allReflected, sizeof(TruckType), false);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Schema_GetModelName(TEST_MODEL_HANDLE)).SetReturn("TruckType");

        ///act
       auto result = CodeFirst_InvokeAction(TEST_DEVICE_HANDLE, g_InvokeActionCallbackArgument, "", "test1", 12, arrayOfAgentDataType);

        ///assert
        ASSERT_ARE_EQUAL(bool, false, DummyDataProvider_test1_wasCalled);
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CodeFirst_DestroyDevice(device);
    }

    /*Tests_SRS_SERIALIZER_99_042:[The parameter types are either predefined parameter types(specs SRS_SERIALIZER_99_004 - SRS_SERIALIZER_99_014)*/
    /*Tests_SRS_SERIALIZER_99_039:[ WITH_ACTION shall declare an action of the current data provider called as the first macro parameter (name) and having the first parameter called parameter1Name of type parameter1Type, the second parameter named parameter2Name having the type parameter2Type and so on. ]*/
    TEST_FUNCTION(CodeFirst_InvokeAction_calling_test1_with_parameter_7_of_wrong_type_fails)
    {
        ///arrange
        arrayOfAgentDataType[6].type = EDM_DOUBLE_TYPE;
        CMocksForCodeFirst mocks;
        (void)CodeFirst_Init(NULL);
        void* device = CodeFirst_CreateDevice(TEST_MODEL_HANDLE,  &DummyDataProvider_allReflected, sizeof(TruckType), false);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Schema_GetModelName(TEST_MODEL_HANDLE)).SetReturn("TruckType");

        ///act
       auto result = CodeFirst_InvokeAction(TEST_DEVICE_HANDLE, g_InvokeActionCallbackArgument, "", "test1", 12, arrayOfAgentDataType);

        ///assert
        ASSERT_ARE_EQUAL(bool, false, DummyDataProvider_test1_wasCalled);
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CodeFirst_DestroyDevice(device);
    }

    /*Tests_SRS_SERIALIZER_99_042:[The parameter types are either predefined parameter types(specs SRS_SERIALIZER_99_004 - SRS_SERIALIZER_99_014)*/
    /*Tests_SRS_SERIALIZER_99_039:[ WITH_ACTION shall declare an action of the current data provider called as the first macro parameter (name) and having the first parameter called parameter1Name of type parameter1Type, the second parameter named parameter2Name having the type parameter2Type and so on. ]*/
    TEST_FUNCTION(CodeFirst_InvokeAction_calling_test1_with_parameter_8_of_wrong_type_fails)
    {
        ///arrange
        arrayOfAgentDataType[7].type = EDM_DOUBLE_TYPE;
        CMocksForCodeFirst mocks;
        (void)CodeFirst_Init(NULL);
        void* device = CodeFirst_CreateDevice(TEST_MODEL_HANDLE,  &DummyDataProvider_allReflected, sizeof(TruckType), false);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Schema_GetModelName(TEST_MODEL_HANDLE)).SetReturn("TruckType");

        ///act
       auto result = CodeFirst_InvokeAction(TEST_DEVICE_HANDLE, g_InvokeActionCallbackArgument, "", "test1", 12, arrayOfAgentDataType);

        ///assert
        ASSERT_ARE_EQUAL(bool, false, DummyDataProvider_test1_wasCalled);
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CodeFirst_DestroyDevice(device);
    }

    /*Tests_SRS_SERIALIZER_99_042:[The parameter types are either predefined parameter types(specs SRS_SERIALIZER_99_004 - SRS_SERIALIZER_99_014)*/
    /*Tests_SRS_SERIALIZER_99_039:[ WITH_ACTION shall declare an action of the current data provider called as the first macro parameter (name) and having the first parameter called parameter1Name of type parameter1Type, the second parameter named parameter2Name having the type parameter2Type and so on. ]*/
    TEST_FUNCTION(CodeFirst_InvokeAction_calling_test1_with_parameter_9_of_wrong_type_fails)
    {
        ///arrange
        arrayOfAgentDataType[8].type = EDM_DOUBLE_TYPE;
        CMocksForCodeFirst mocks;
        (void)CodeFirst_Init(NULL);
        void* device = CodeFirst_CreateDevice(TEST_MODEL_HANDLE,  &DummyDataProvider_allReflected, sizeof(TruckType), false);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Schema_GetModelName(TEST_MODEL_HANDLE)).SetReturn("TruckType");

        ///act
       auto result = CodeFirst_InvokeAction(TEST_DEVICE_HANDLE, g_InvokeActionCallbackArgument, "", "test1", 12, arrayOfAgentDataType);

        ///assert
        ASSERT_ARE_EQUAL(bool, false, DummyDataProvider_test1_wasCalled);
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CodeFirst_DestroyDevice(device);
    }

    /*Tests_SRS_SERIALIZER_99_042:[The parameter types are either predefined parameter types(specs SRS_SERIALIZER_99_004 - SRS_SERIALIZER_99_014)*/
    /*Tests_SRS_SERIALIZER_99_039:[ WITH_ACTION shall declare an action of the current data provider called as the first macro parameter (name) and having the first parameter called parameter1Name of type parameter1Type, the second parameter named parameter2Name having the type parameter2Type and so on. ]*/
    TEST_FUNCTION(CodeFirst_InvokeAction_calling_test1_with_parameter_10_of_wrong_type_fails)
    {
        ///arrange
        arrayOfAgentDataType[9].type = EDM_DOUBLE_TYPE;
        CMocksForCodeFirst mocks;
        (void)CodeFirst_Init(NULL);
        void* device = CodeFirst_CreateDevice(TEST_MODEL_HANDLE,  &DummyDataProvider_allReflected, sizeof(TruckType), false);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Schema_GetModelName(TEST_MODEL_HANDLE)).SetReturn("TruckType");

        ///act
       auto result = CodeFirst_InvokeAction(TEST_DEVICE_HANDLE, g_InvokeActionCallbackArgument, "", "test1", 12, arrayOfAgentDataType);

        ///assert
        ASSERT_ARE_EQUAL(bool, false, DummyDataProvider_test1_wasCalled);
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CodeFirst_DestroyDevice(device);
    }

    /*Tests_SRS_SERIALIZER_99_042:[The parameter types are either predefined parameter types(specs SRS_SERIALIZER_99_004 - SRS_SERIALIZER_99_014)*/
    /*Tests_SRS_SERIALIZER_99_039:[ WITH_ACTION shall declare an action of the current data provider called as the first macro parameter (name) and having the first parameter called parameter1Name of type parameter1Type, the second parameter named parameter2Name having the type parameter2Type and so on. ]*/
    TEST_FUNCTION(CodeFirst_InvokeAction_calling_test1_with_parameter_11_of_wrong_type_fails)
    {
        ///arrange
        arrayOfAgentDataType[10].type = EDM_DOUBLE_TYPE;
        CMocksForCodeFirst mocks;
        (void)CodeFirst_Init(NULL);
        void* device = CodeFirst_CreateDevice(TEST_MODEL_HANDLE,  &DummyDataProvider_allReflected, sizeof(TruckType), false);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Schema_GetModelName(TEST_MODEL_HANDLE)).SetReturn("TruckType");

        ///act
       auto result = CodeFirst_InvokeAction(TEST_DEVICE_HANDLE, g_InvokeActionCallbackArgument, "", "test1", 12, arrayOfAgentDataType);

        ///assert
        ASSERT_ARE_EQUAL(bool, false, DummyDataProvider_test1_wasCalled);
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CodeFirst_DestroyDevice(device);
    }

    /*Tests_SRS_SERIALIZER_99_042:[The parameter types are either predefined parameter types(specs SRS_SERIALIZER_99_004 - SRS_SERIALIZER_99_014)*/
    /*Tests_SRS_SERIALIZER_99_039:[ WITH_ACTION shall declare an action of the current data provider called as the first macro parameter (name) and having the first parameter called parameter1Name of type parameter1Type, the second parameter named parameter2Name having the type parameter2Type and so on. ]*/
    TEST_FUNCTION(CodeFirst_InvokeAction_calling_test1_with_parameter_12_of_wrong_type_fails)
    {
        ///arrange
        arrayOfAgentDataType[11].type = EDM_DOUBLE_TYPE;
        CMocksForCodeFirst mocks;
        (void)CodeFirst_Init(NULL);
        void* device = CodeFirst_CreateDevice(TEST_MODEL_HANDLE,  &DummyDataProvider_allReflected, sizeof(TruckType), false);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Schema_GetModelName(TEST_MODEL_HANDLE)).SetReturn("TruckType");

        ///act
       auto result = CodeFirst_InvokeAction(TEST_DEVICE_HANDLE, g_InvokeActionCallbackArgument, "", "test1", 12, arrayOfAgentDataType);

        ///assert
        ASSERT_ARE_EQUAL(bool, false, DummyDataProvider_test1_wasCalled);
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CodeFirst_DestroyDevice(device);
    }

    /*Tests_SRS_SERIALIZER_99_042:[The parameter types are either predefined parameter types(specs SRS_SERIALIZER_99_004 - SRS_SERIALIZER_99_014)*/
    /*Tests_SRS_SERIALIZER_99_039:[ WITH_ACTION shall declare an action of the current data provider called as the first macro parameter (name) and having the first parameter called parameter1Name of type parameter1Type, the second parameter named parameter2Name having the type parameter2Type and so on. ]*/
    /*Tests_SRS_SERIALIZER_99_040:[ In addition to declaring the function, DECLARE_IOT_METHOD shall provide a definition for a wrapper that takes as parameters a size_t parameterCount and const AGENT_DATA_TYPE*.] */
    /*Tests_SRS_SERIALIZER_99_041:[ This wrapper shall convert all the arguments to predefined types and then call the function written by the data provider developer.]*/
    TEST_FUNCTION(CodeFirst_InvokeAction_calling_test1_succeeds)
    {
        ///arrange
        CMocksForCodeFirst mocks;
        (void)CodeFirst_Init(NULL);
        void* device = CodeFirst_CreateDevice(TEST_MODEL_HANDLE,  &DummyDataProvider_allReflected, sizeof(TruckType), false);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Schema_GetModelName(TEST_MODEL_HANDLE)).SetReturn("TruckType");

        ///act
       auto result = CodeFirst_InvokeAction(TEST_DEVICE_HANDLE, g_InvokeActionCallbackArgument, "", "test1", 15, arrayOfAgentDataType);

        ///assert
        ASSERT_ARE_EQUAL(bool, true, DummyDataProvider_test1_wasCalled);
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_SUCCESS, result);
        ASSERT_ARE_EQUAL(double, 42.0, DummyDataProvider_test1_P1);
        ASSERT_ARE_EQUAL(int, 4, DummyDataProvider_test1_P2);
        ASSERT_ARE_EQUAL(float, 24.0f, DummyDataProvider_test1_P3);
        ASSERT_ARE_EQUAL(int, 2, DummyDataProvider_test1_P4);
        ASSERT_ARE_EQUAL(int, 3, DummyDataProvider_test1_P5);
        ASSERT_ARE_EQUAL(int, 4, DummyDataProvider_test1_P6);
        ASSERT_ARE_EQUAL(int, 5, DummyDataProvider_test1_P7);
        ASSERT_ARE_EQUAL(int, 6, DummyDataProvider_test1_P8);
        ASSERT_ARE_EQUAL(int, 7, (int)DummyDataProvider_test1_P9);
        ASSERT_ARE_EQUAL(bool, true, DummyDataProvider_test1_P10);
        ASSERT_ARE_EQUAL(char_ptr, "someChars", DummyDataProvider_test1_P11);
        ASSERT_ARE_EQUAL(EDM_DATE_TIME_OFFSET, someEdmDateTimeOffset, DummyDataProvider_test1_P12);
        ASSERT_ARE_EQUAL(EDM_GUID, someEdmGuid, DummyDataProvider_test1_P13);
        ASSERT_ARE_EQUAL(EDM_BINARY, someEdmBinary, DummyDataProvider_test1_P14);
        ASSERT_ARE_EQUAL(char_ptr, "someMoreChars", DummyDataProvider_test1_P15);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CodeFirst_DestroyDevice(device);
    }

    /* Tests_SRS_CODEFIRST_99_140:[CodeFirst_InvokeAction shall pass to the action wrapper that it calls a pointer to the model where the action is defined.] */
    /* Tests_SRS_CODEFIRST_99_139:[If the relativeActionPath is empty then the action shall be looked up in the device model.] */
    /* Tests_SRS_CODEFIRST_99_138:[The relativeActionPath argument shall be used by CodeFirst_InvokeAction to find the child model where the action is declared.] */
    /* Tests_SRS_CODEFIRST_99_142:[The relativeActionPath argument shall be in the format "childModel1/childModel2/.../childModelN".] */
    TEST_FUNCTION(CodeFirst_InvokeAction_For_A_Child_Model_Passes_The_OuterType_Instance_To_The_Callback)
    {
        ///arrange
        CMocksForCodeFirst mocks;
        (void)CodeFirst_Init(NULL);
        OuterType* device = (OuterType*)CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &testModelInModelReflectedData, sizeof(OuterType), false);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Schema_GetModelName(TEST_MODEL_HANDLE)).SetReturn("OuterType");

        ///act
       auto result = CodeFirst_InvokeAction(TEST_DEVICE_HANDLE, g_InvokeActionCallbackArgument, "", "reset", 0, NULL);

        ///assert
       ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_SUCCESS, result);
        ASSERT_ARE_EQUAL(void_ptr, device, OuterType_reset_device);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CodeFirst_DestroyDevice(device);
    }

    /* Tests_SRS_CODEFIRST_99_140:[CodeFirst_InvokeAction shall pass to the action wrapper that it calls a pointer to the model where the action is defined.] */
    /* Tests_SRS_CODEFIRST_99_139:[If the relativeActionPath is empty then the action shall be looked up in the device model.] */
    /* Tests_SRS_CODEFIRST_99_138:[The relativeActionPath argument shall be used by CodeFirst_InvokeAction to find the child model where the action is declared.] */
    /* Tests_SRS_CODEFIRST_99_142:[The relativeActionPath argument shall be in the format "childModel1/childModel2/.../childModelN".] */
    TEST_FUNCTION(CodeFirst_InvokeAction_For_A_Child_Model_Passes_The_InnerType_Instance_To_The_Callback)
    {
        ///arrange
        CMocksForCodeFirst mocks;
        (void)CodeFirst_Init(NULL);
        OuterType* device = (OuterType*)CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &testModelInModelReflectedData, sizeof(OuterType), false);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Schema_GetModelName(TEST_MODEL_HANDLE)).SetReturn("OuterType");

        ///act
       auto result = CodeFirst_InvokeAction(TEST_DEVICE_HANDLE, g_InvokeActionCallbackArgument, "Inner", "reset", 0, NULL);



        ///assert
       ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_SUCCESS, result);
        ASSERT_ARE_EQUAL(void_ptr, &device->Inner, InnerType_reset_device);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CodeFirst_DestroyDevice(device);
    }

    /* Tests_SRS_CODEFIRST_99_141:[If a child model specified in the relativeActionPath argument cannot be found by CodeFirst_InvokeAction, it shall return EXECUTE_COMMAND_ERROR.] */
    TEST_FUNCTION(CodeFirst_InvokeAction_For_A_Child_Model_And_The_Model_Is_Not_Found_Fails)
    {
        ///arrange
        CMocksForCodeFirst mocks;
        (void)CodeFirst_Init(NULL);
        OuterType* device = (OuterType*)CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &testModelInModelReflectedData, sizeof(OuterType), false);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Schema_GetModelName(TEST_MODEL_HANDLE)).SetReturn("OuterType");

        ///act
       auto result = CodeFirst_InvokeAction(TEST_DEVICE_HANDLE, g_InvokeActionCallbackArgument, "Inne", "reset", 0, NULL);

        ///assert
       ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CodeFirst_DestroyDevice(device);
    }

    /* Tests_SRS_CODEFIRST_99_141:[If a child model specified in the relativeActionPath argument cannot be found by CodeFirst_InvokeAction, it shall return EXECUTE_COMMAND_ERROR.] */
    TEST_FUNCTION(CodeFirst_InvokeAction_For_A_Child_Model_And_The_Model_Is_Not_Found_Because_Path_Too_Deep_Fails)
    {
        ///arrange
        CMocksForCodeFirst mocks;
        (void)CodeFirst_Init(NULL);
        OuterType* device = (OuterType*)CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &testModelInModelReflectedData, sizeof(OuterType), false);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Schema_GetModelName(TEST_MODEL_HANDLE)).SetReturn("OuterType");

        ///act
       auto result = CodeFirst_InvokeAction(TEST_DEVICE_HANDLE, g_InvokeActionCallbackArgument, "Inner/FakeInner", "reset", 0, NULL);

        ///assert
       ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CodeFirst_DestroyDevice(device);
    }

    /* Tests_SRS_CODEFIRST_99_141:[If a child model specified in the relativeActionPath argument cannot be found by CodeFirst_InvokeAction, it shall return EXECUTE_COMMAND_ERROR.] */
    TEST_FUNCTION(CodeFirst_InvokeAction_For_A_Child_Model_And_The_Action_Is_Not_Found_Fails)
    {
        ///arrange
        CMocksForCodeFirst mocks;
        (void)CodeFirst_Init(NULL);
        OuterType* device = (OuterType*)CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &testModelInModelReflectedData, sizeof(OuterType), false);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Schema_GetModelName(TEST_MODEL_HANDLE)).SetReturn("OuterType");

        ///act
       auto result = CodeFirst_InvokeAction(TEST_DEVICE_HANDLE, g_InvokeActionCallbackArgument, "Inner", "rst", 0, NULL);

        ///assert
       ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CodeFirst_DestroyDevice(device);
    }

    /* CodeFirst_CreateDevice */

    /* Tests_SRS_CODEFIRST_99_080:[If CodeFirst_CreateDevice is invoked with a NULL iotHubClientHandle or model, it shall return NULL.] */
    TEST_FUNCTION(CodeFirst_CreateDevice_With_NULL_Model_Fails)
    {
        // arrange
        CMocksForCodeFirst mocks;

        // act
        void* result = CodeFirst_CreateDevice(NULL, &DummyDataProvider_allReflected, 1, false);

        // assert
        ASSERT_IS_NULL(result);
    }

    /* Tests_SRS_CODEFIRST_99_079:[CodeFirst_CreateDevice shall create a device and allocate a memory block that should hold the device data.] */
    /* Tests_SRS_CODEFIRST_99_081:[CodeFirst_CreateDevice shall use Device_Create to create a device handle.] */
    /* Tests_SRS_CODEFIRST_99_082:[CodeFirst_CreateDevice shall pass to Device_Create the function CodeFirst_InvokeAction as action callback argument.] */
    /* Tests_SRS_CODEFIRST_99_101:[On success, CodeFirst_CreateDevice shall return a non NULL pointer to the device data.] */
    /* Tests_SRS_CODEFIRST_01_001: [CodeFirst_CreateDevice shall pass the includePropertyPath argument to Device_Create.] */
    TEST_FUNCTION(CodeFirst_CreateDevice_With_Valid_Arguments_and_includePropertyPath_false_Succeeds_1)
    {
        // arrange
        CMocksForCodeFirst mocks;

        STRICT_EXPECTED_CALL(mocks, Device_Create(TEST_MODEL_HANDLE, CodeFirst_InvokeAction, TEST_CALLBACK_CONTEXT, false, IGNORED_PTR_ARG))
            .IgnoreArgument(3).IgnoreArgument(5);
        STRICT_EXPECTED_CALL(mocks, Schema_AddDeviceRef(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        // act
        void* result = CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &DummyDataProvider_allReflected, 1, false);

        // assert
        ASSERT_IS_NOT_NULL(result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CodeFirst_DestroyDevice(result);
    }

    TEST_FUNCTION(CodeFirst_CreateDevice_With_Valid_Arguments_and_includePropertyPath_false_Succeeds_2)
    {
        // arrange
        CMocksForCodeFirst mocks;

        STRICT_EXPECTED_CALL(mocks, Device_Create(TEST_MODEL_HANDLE, CodeFirst_InvokeAction, TEST_CALLBACK_CONTEXT, false, IGNORED_PTR_ARG))
            .IgnoreArgument(3).IgnoreArgument(5);
        STRICT_EXPECTED_CALL(mocks, Schema_AddDeviceRef(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        // act
        void* result = CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &DummyDataProvider_allReflected, 1, false);

        // assert
        ASSERT_IS_NOT_NULL(result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CodeFirst_DestroyDevice(result);
    }

    /* Tests_SRS_CODEFIRST_01_001: [CodeFirst_CreateDevice shall pass the includePropertyPath argument to Device_Create.] */
    TEST_FUNCTION(CodeFirst_CreateDevice_With_Valid_Arguments_and_includePropertyPath_true_Succeeds)
    {
        // arrange
        CMocksForCodeFirst mocks;

        STRICT_EXPECTED_CALL(mocks, Device_Create(TEST_MODEL_HANDLE, CodeFirst_InvokeAction, TEST_CALLBACK_CONTEXT, true, IGNORED_PTR_ARG))
            .IgnoreArgument(3).IgnoreArgument(5);
        STRICT_EXPECTED_CALL(mocks, Schema_AddDeviceRef(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        // act
        void* result = CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &DummyDataProvider_allReflected, 1, true);

        // assert
        ASSERT_IS_NOT_NULL(result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CodeFirst_DestroyDevice(result);
    }

    /* Tests_SRS_CODEFIRST_99_084:[If Device_Create fails, CodeFirst_CreateDevice shall return NULL.] */
    TEST_FUNCTION(When_Device_Create_Fails_Then_CodeFirst_CreateDevice_Fails)
    {
        // arrange
        CMocksForCodeFirst mocks;

        STRICT_EXPECTED_CALL(mocks, Device_Create(TEST_MODEL_HANDLE, CodeFirst_InvokeAction, TEST_CALLBACK_CONTEXT, false, IGNORED_PTR_ARG))
            .IgnoreArgument(3).IgnoreArgument(5).SetReturn(DEVICE_ERROR);

        // act
        void* result = CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &DummyDataProvider_allReflected, 1, false);

        // assert
        ASSERT_IS_NULL(result);
    }

    /* Tests_SRS_CODEFIRST_99_106:[If CodeFirst_CreateDevice is called when the modules is not initialized is shall return NULL.] */
    TEST_FUNCTION(CodeFirst_CreateDevice_When_The_Module_Is_Not_Initialized_Fails)
    {
        // arrange
        CMocksForCodeFirst mocks;
        CodeFirst_Deinit();
        mocks.ResetAllCalls();

        // act
        void* result = CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &DummyDataProvider_allReflected, 1, false);

        // assert
        ASSERT_IS_NULL(result);
    }

    /* CodeFirst_DestroyDevice */

    /* Tests_SRS_CODEFIRST_99_086:[If the argument is NULL, CodeFirst_DestroyDevice shall do nothing.] */
    TEST_FUNCTION(CodeFirst_DestroyDevice_With_NULL_Argument_Does_Nothing)
    {
        // arrange
        CMocksForCodeFirst mocks;

        // act
        CodeFirst_DestroyDevice(NULL);

        // assert
        // no explicit assert, uMock checks the calls
    }

    /* Tests_SRS_CODEFIRST_99_085:[CodeFirst_DestroyDevice shall free all resources associated with a device.] */
    /* Tests_SRS_CODEFIRST_99_087:[In order to release the device handle, CodeFirst_DestroyDevice shall call Device_Destroy.] */
    TEST_FUNCTION(CodeFirst_DestroyDevice_With_Valid_Argument_Destroys_The_Device)
    {
        // arrange
        CMocksForCodeFirst mocks;

        void* device = CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &DummyDataProvider_allReflected, 1, false);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Device_Destroy(TEST_DEVICE_HANDLE));
        STRICT_EXPECTED_CALL(mocks, Schema_ReleaseDeviceRef(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, Schema_DestroyIfUnused(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        // act
        CodeFirst_DestroyDevice(device);

        // assert
        // no explicit assert, uMock checks the calls
    }

    /* CodeFirst_SendAsync */

    /* Tests_SRS_CODEFIRST_99_103:[If CodeFirst_SendAsync is called with numProperties being zero, CODEFIRST_INVALID_ARG shall be returned.] */
    TEST_FUNCTION(CodeFirst_SendAsync_With_0_NumProperties_Fails)
    {
        // arrange
        CMocksForCodeFirst mocks;
        void* device = CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &DummyDataProvider_allReflected, 1, false);
        unsigned char* destination;
        size_t destinationSize;
        mocks.ResetAllCalls();

        // act
        CODEFIRST_RESULT result = CodeFirst_SendAsync(&destination, &destinationSize, 0, device);

        // assert
        ASSERT_ARE_EQUAL(CODEFIRST_RESULT, CODEFIRST_INVALID_ARG, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CodeFirst_DestroyDevice(device);
    }

    /* Tests_SRS_CODEFIRST_99_088:[CodeFirst_SendAsync shall send to the Device module a set of properties.] */
    /* Tests_SRS_CODEFIRST_99_105:[The properties are passed as pointers to the memory locations where the data exists in the device block allocated by CodeFirst_CreateDevice.] */
    /* Tests_SRS_CODEFIRST_99_089:[The numProperties argument shall indicate how many properties are to be sent.] */
    /* Tests_SRS_CODEFIRST_99_090:[All the properties shall be sent together by using the transacted APIs of the device.] */
    /* Tests_SRS_CODEFIRST_99_091:[CodeFirst_SendAsync shall start a transaction by calling Device_StartTransaction.] */
    /* Tests_SRS_CODEFIRST_99_092:[CodeFirst shall publish each value by using Device_PublishTransacted.] */
    /* Tests_SRS_CODEFIRST_99_093:[After all values have been published, Device_EndTransaction shall be called.] */
    /* Tests_SRS_CODEFIRST_99_095:[For each value passed to it, CodeFirst_SendAsync shall look up to which device the value belongs.] */
    /* Tests_SRS_CODEFIRST_99_097:[For each value marshalling to AGENT_DATA_TYPE shall be performed.] */
    /* Tests_SRS_CODEFIRST_99_098:[The marshalling shall be done by calling the Create_AGENT_DATA_TYPE_from_Ptr function associated with the property.] */
    /* Tests_SRS_CODEFIRST_99_117:[On success, CodeFirst_SendAsync shall return CODEFIRST_OK.] */
    TEST_FUNCTION(CodeFirst_SendAsync_With_One_Property_Succeeds)
    {
        // arrange
        CMocksForCodeFirst mocks;
        SimpleDevice* device = (SimpleDevice*)CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &testReflectedData, sizeof(SimpleDevice), false);
        unsigned char* destination;
        size_t destinationSize;
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Device_StartTransaction(TEST_DEVICE_HANDLE));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelName(TEST_MODEL_HANDLE));
        EXPECTED_CALL(mocks, Create_AGENT_DATA_TYPE_from_DOUBLE(IGNORED_PTR_ARG, 0.0));
        STRICT_EXPECTED_CALL(mocks, Device_PublishTransacted(TEST_TRANSACTION_HANDLE, "this_is_double", IGNORED_PTR_ARG))
            .IgnoreArgument(3);
        EXPECTED_CALL(mocks, Destroy_AGENT_DATA_TYPE(IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(mocks, Device_EndTransaction(TEST_TRANSACTION_HANDLE, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(2)
            .IgnoreArgument(3);
        device->this_is_double = 42.0;

        // act
        CODEFIRST_RESULT result = CodeFirst_SendAsync(&destination, &destinationSize, 1, &device->this_is_double);

        // assert
        ASSERT_ARE_EQUAL(CODEFIRST_RESULT, CODEFIRST_OK, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CodeFirst_DestroyDevice(device);
    }

    /* Tests_SRS_CODEFIRST_99_088:[CodeFirst_SendAsync shall send to the Device module a set of properties.] */
    /* Tests_SRS_CODEFIRST_99_105:[The properties are passed as pointers to the memory locations where the data exists in the device block allocated by CodeFirst_CreateDevice.] */
    /* Tests_SRS_CODEFIRST_99_089:[The numProperties argument shall indicate how many properties are to be sent.] */
    /* Tests_SRS_CODEFIRST_99_090:[All the properties shall be sent together by using the transacted APIs of the device.] */
    /* Tests_SRS_CODEFIRST_99_091:[CodeFirst_SendAsync shall start a transaction by calling Device_StartTransaction.] */
    /* Tests_SRS_CODEFIRST_99_092:[CodeFirst shall publish each value by using Device_PublishTransacted.] */
    /* Tests_SRS_CODEFIRST_99_093:[After all values have been published, Device_EndTransaction shall be called.] */
    /* Tests_SRS_CODEFIRST_99_095:[For each value passed to it, CodeFirst_SendAsync shall look up to which device the value belongs.] */
    /* Tests_SRS_CODEFIRST_99_097:[For each value marshalling to AGENT_DATA_TYPE shall be performed.] */
    /* Tests_SRS_CODEFIRST_99_098:[The marshalling shall be done by calling the Create_AGENT_DATA_TYPE_from_Ptr function associated with the property] */
    /* Tests_SRS_CODEFIRST_99_117:[On success, CodeFirst_SendAsync shall return CODEFIRST_OK.] */
    TEST_FUNCTION(CodeFirst_SendAsync_2_Properties_Succeeds)
    {
        // arrange
        CMocksForCodeFirst mocks;
        SimpleDevice* device = (SimpleDevice*)CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &testReflectedData, sizeof(SimpleDevice), false);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Device_StartTransaction(TEST_DEVICE_HANDLE));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelName(TEST_MODEL_HANDLE));
        EXPECTED_CALL(mocks, Create_AGENT_DATA_TYPE_from_DOUBLE(IGNORED_PTR_ARG, 0.0));
        STRICT_EXPECTED_CALL(mocks, Device_PublishTransacted(TEST_TRANSACTION_HANDLE, "this_is_double", IGNORED_PTR_ARG))
            .IgnoreArgument(3);
        EXPECTED_CALL(mocks, Destroy_AGENT_DATA_TYPE(IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelName(TEST_MODEL_HANDLE));
        EXPECTED_CALL(mocks, Create_AGENT_DATA_TYPE_from_SINT32(IGNORED_PTR_ARG, 0));
        STRICT_EXPECTED_CALL(mocks, Device_PublishTransacted(TEST_TRANSACTION_HANDLE, "this_is_int", IGNORED_PTR_ARG))
            .IgnoreArgument(3);
        EXPECTED_CALL(mocks, Destroy_AGENT_DATA_TYPE(IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(mocks, Device_EndTransaction(TEST_TRANSACTION_HANDLE, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(2)
            .IgnoreArgument(3);
        device->this_is_double = 42.0;
        device->this_is_int = 1;
        unsigned char* destination;
        size_t destinationSize;

        // act
        CODEFIRST_RESULT result = CodeFirst_SendAsync(&destination, &destinationSize, 2, &device->this_is_double, &device->this_is_int);

        // assert
        ASSERT_ARE_EQUAL(CODEFIRST_RESULT, CODEFIRST_OK, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CodeFirst_DestroyDevice(device);
    }

    /* Tests_SRS_CODEFIRST_99_094:[If any Device API fail, CodeFirst_SendAsync shall return CODEFIRST_DEVICE_PUBLISH_FAILED.] */
    TEST_FUNCTION(When_StartTransaction_Fails_CodeFirst_SendAsync_Fails)
    {
        // arrange
        CMocksForCodeFirst mocks;
        SimpleDevice* device = (SimpleDevice*)CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &testReflectedData, sizeof(SimpleDevice), false);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Device_StartTransaction(TEST_DEVICE_HANDLE))
            .SetReturn((TRANSACTION_HANDLE)NULL);
        device->this_is_double = 42.0;
        unsigned char* destination;
        size_t destinationSize;

        // act
        CODEFIRST_RESULT result = CodeFirst_SendAsync(&destination, &destinationSize, 1, device);

        // assert
        ASSERT_ARE_EQUAL(CODEFIRST_RESULT, CODEFIRST_DEVICE_PUBLISH_FAILED, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CodeFirst_DestroyDevice(device);
    }

    /* Tests_SRS_CODEFIRST_99_094:[If any Device API fail, CodeFirst_SendAsync shall return CODEFIRST_DEVICE_PUBLISH_FAILED.] */
    TEST_FUNCTION(When_PublishTransacted_Fails_CodeFirst_SendAsync_Fails)
    {
        // arrange
        CMocksForCodeFirst mocks;
        SimpleDevice* device = (SimpleDevice*)CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &testReflectedData, sizeof(SimpleDevice), false);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Device_StartTransaction(TEST_DEVICE_HANDLE));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelName(TEST_MODEL_HANDLE));
        EXPECTED_CALL(mocks, Create_AGENT_DATA_TYPE_from_DOUBLE(IGNORED_PTR_ARG, 0.0));
        STRICT_EXPECTED_CALL(mocks, Device_PublishTransacted(TEST_TRANSACTION_HANDLE, "this_is_double", IGNORED_PTR_ARG))
            .IgnoreArgument(3).SetReturn(DEVICE_ERROR);
        EXPECTED_CALL(mocks, Destroy_AGENT_DATA_TYPE(IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(mocks, Device_CancelTransaction(TEST_TRANSACTION_HANDLE));
        device->this_is_double = 42.0;
        unsigned char* destination;
        size_t destinationSize;

        // act
        CODEFIRST_RESULT result = CodeFirst_SendAsync(&destination, &destinationSize, 1, &device->this_is_double);

        // assert
        ASSERT_ARE_EQUAL(CODEFIRST_RESULT, CODEFIRST_DEVICE_PUBLISH_FAILED, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CodeFirst_DestroyDevice(device);
    }

    /* Tests_SRS_CODEFIRST_99_094:[If any Device API fail, CodeFirst_SendAsync shall return CODEFIRST_DEVICE_PUBLISH_FAILED.] */
    TEST_FUNCTION(When_PublishTransacted_Fails_For_The_2nd_Property_CodeFirst_SendAsync_Fails)
    {
        // arrange
        CMocksForCodeFirst mocks;
        SimpleDevice* device = (SimpleDevice*)CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &testReflectedData, sizeof(SimpleDevice), false);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Device_StartTransaction(TEST_DEVICE_HANDLE));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelName(TEST_MODEL_HANDLE));
        EXPECTED_CALL(mocks, Create_AGENT_DATA_TYPE_from_DOUBLE(IGNORED_PTR_ARG, 0.0));
        STRICT_EXPECTED_CALL(mocks, Device_PublishTransacted(TEST_TRANSACTION_HANDLE, "this_is_double", IGNORED_PTR_ARG))
            .IgnoreArgument(3);
        EXPECTED_CALL(mocks, Destroy_AGENT_DATA_TYPE(IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelName(TEST_MODEL_HANDLE));
        EXPECTED_CALL(mocks, Create_AGENT_DATA_TYPE_from_SINT32(IGNORED_PTR_ARG, 0));
        STRICT_EXPECTED_CALL(mocks, Device_PublishTransacted(TEST_TRANSACTION_HANDLE, "this_is_int", IGNORED_PTR_ARG))
            .IgnoreArgument(3).SetReturn(DEVICE_ERROR);
        EXPECTED_CALL(mocks, Destroy_AGENT_DATA_TYPE(IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(mocks, Device_CancelTransaction(TEST_TRANSACTION_HANDLE));
        device->this_is_double = 42.0;
        device->this_is_int = 1;
        unsigned char* destination;
        size_t destinationSize;

        // act
        CODEFIRST_RESULT result = CodeFirst_SendAsync(&destination, &destinationSize, 2, &device->this_is_double, &device->this_is_int);

        // assert
        ASSERT_ARE_EQUAL(CODEFIRST_RESULT, CODEFIRST_DEVICE_PUBLISH_FAILED, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CodeFirst_DestroyDevice(device);
    }

    /* Tests_SRS_CODEFIRST_99_094:[If any Device API fail, CodeFirst_SendAsync shall return CODEFIRST_DEVICE_PUBLISH_FAILED.] */
    TEST_FUNCTION(When_EndTransacted_Fails_CodeFirst_SendAsync_Fails)
    {
        // arrange
        CMocksForCodeFirst mocks;
        SimpleDevice* device = (SimpleDevice*)CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &testReflectedData, sizeof(SimpleDevice), false);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Device_StartTransaction(TEST_DEVICE_HANDLE));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelName(TEST_MODEL_HANDLE));
        EXPECTED_CALL(mocks, Create_AGENT_DATA_TYPE_from_DOUBLE(IGNORED_PTR_ARG, 0.0));
        STRICT_EXPECTED_CALL(mocks, Device_PublishTransacted(TEST_TRANSACTION_HANDLE, "this_is_double", IGNORED_PTR_ARG))
            .IgnoreArgument(3);
        EXPECTED_CALL(mocks, Destroy_AGENT_DATA_TYPE(IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(mocks, Device_EndTransaction(TEST_TRANSACTION_HANDLE, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(2)
            .IgnoreArgument(3)
            .SetReturn(DEVICE_ERROR);
        device->this_is_double = 42.0;
        unsigned char* destination;
        size_t destinationSize;

        // act
        CODEFIRST_RESULT result = CodeFirst_SendAsync(&destination, &destinationSize, 1, &device->this_is_double);

        // assert
        ASSERT_ARE_EQUAL(CODEFIRST_RESULT, CODEFIRST_DEVICE_PUBLISH_FAILED, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CodeFirst_DestroyDevice(device);
    }

    /* Tests_SRS_CODEFIRST_99_104:[If a property cannot be associated with a device, CodeFirst_SendAsync shall return CODEFIRST_INVALID_ARG.] */
    TEST_FUNCTION(When_An_Invalid_Pointer_Is_Passed_CodeFirst_SendAsync_Fails)
    {
        // arrange
        CMocksForCodeFirst mocks;
        SimpleDevice* device = (SimpleDevice*)CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &testReflectedData, sizeof(SimpleDevice), false);
        mocks.ResetAllCalls();

        device->this_is_double = 42.0;
        unsigned char* destination;
        size_t destinationSize;

        // act
        CODEFIRST_RESULT result = CodeFirst_SendAsync(&destination, &destinationSize, 1, device - 1);

        // assert
        ASSERT_ARE_EQUAL(CODEFIRST_RESULT, CODEFIRST_INVALID_ARG, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CodeFirst_DestroyDevice(device);
    }

    /* Tests_SRS_CODEFIRST_99_104:[If a property cannot be associated with a device, CodeFirst_SendAsync shall return CODEFIRST_INVALID_ARG.] */
    TEST_FUNCTION(When_A_Pointer_Within_The_Device_Block_But_Mismatched_Is_Passed_CodeFirst_SendAsync_Fails)
    {
        // arrange
        CMocksForCodeFirst mocks;
        SimpleDevice* device = (SimpleDevice*)CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &testReflectedData, sizeof(SimpleDevice), false);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Device_StartTransaction(TEST_DEVICE_HANDLE));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelName(TEST_MODEL_HANDLE));
        STRICT_EXPECTED_CALL(mocks, Device_CancelTransaction(TEST_TRANSACTION_HANDLE));
        device->this_is_double = 42.0;
        unsigned char* destination;
        size_t destinationSize;

        // act
        CODEFIRST_RESULT result = CodeFirst_SendAsync(&destination, &destinationSize, 1, (unsigned char*)device + 1);

        // assert
        ASSERT_ARE_EQUAL(CODEFIRST_RESULT, CODEFIRST_INVALID_ARG, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CodeFirst_DestroyDevice(device);
    }

    /* Tests_SRS_CODEFIRST_99_099:[If Create_AGENT_DATA_TYPE_from_Ptr fails, CodeFirst_SendAsync shall return CODEFIRST_AGENT_DATA_TYPE_ERROR.] */
    TEST_FUNCTION(When_Creating_The_Agent_Data_Type_Fails_Then_CodeFirst_SendAsync_Fails)
    {
        // arrange
        CMocksForCodeFirst mocks;
        SimpleDevice* device = (SimpleDevice*)CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &testReflectedData, sizeof(SimpleDevice), false);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Device_StartTransaction(TEST_DEVICE_HANDLE));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelName(TEST_MODEL_HANDLE));
        EXPECTED_CALL(mocks, Create_AGENT_DATA_TYPE_from_DOUBLE(IGNORED_PTR_ARG, 0.0))
            .SetReturn(AGENT_DATA_TYPES_ERROR);
        STRICT_EXPECTED_CALL(mocks, Device_CancelTransaction(TEST_TRANSACTION_HANDLE));
        device->this_is_double = 42.0;
        unsigned char* destination;
        size_t destinationSize;

        // act
        CODEFIRST_RESULT result = CodeFirst_SendAsync(&destination, &destinationSize, 1, &device->this_is_double);

        // assert
        ASSERT_ARE_EQUAL(CODEFIRST_RESULT, CODEFIRST_AGENT_DATA_TYPE_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CodeFirst_DestroyDevice(device);
    }

    /* Tests_SRS_CODEFIRST_99_099:[If Create_AGENT_DATA_TYPE_from_Ptr fails, CodeFirst_SendAsync shall return CODEFIRST_AGENT_DATA_TYPE_ERROR.] */
    TEST_FUNCTION(When_Creating_The_Agent_Data_Type_Fails_For_The_2nd_Property_Then_CodeFirst_SendAsync_Fails)
    {
        // arrange
        CMocksForCodeFirst mocks;
        SimpleDevice* device = (SimpleDevice*)CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &testReflectedData, sizeof(SimpleDevice), false);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Device_StartTransaction(TEST_DEVICE_HANDLE));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelName(TEST_MODEL_HANDLE));
        EXPECTED_CALL(mocks, Create_AGENT_DATA_TYPE_from_DOUBLE(IGNORED_PTR_ARG, 0.0));
        STRICT_EXPECTED_CALL(mocks, Device_PublishTransacted(TEST_TRANSACTION_HANDLE, "this_is_double", IGNORED_PTR_ARG))
            .IgnoreArgument(3);
        EXPECTED_CALL(mocks, Destroy_AGENT_DATA_TYPE(IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelName(TEST_MODEL_HANDLE));
        EXPECTED_CALL(mocks, Create_AGENT_DATA_TYPE_from_SINT32(IGNORED_PTR_ARG, 0))
            .SetReturn(AGENT_DATA_TYPES_ERROR);
        STRICT_EXPECTED_CALL(mocks, Device_CancelTransaction(TEST_TRANSACTION_HANDLE));
        device->this_is_double = 42.0;
        unsigned char* destination;
        size_t destinationSize;

        // act
        CODEFIRST_RESULT result = CodeFirst_SendAsync(&destination, &destinationSize, 2, &device->this_is_double, &device->this_is_int);

        // assert
        ASSERT_ARE_EQUAL(CODEFIRST_RESULT, CODEFIRST_AGENT_DATA_TYPE_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CodeFirst_DestroyDevice(device);
    }

    /* Tests_SRS_CODEFIRST_99_096:[All values have to belong to the same device, otherwise CodeFirst_SendAsync shall return CODEFIRST_VALUES_FROM_DIFFERENT_DEVICES_ERROR.] */
    TEST_FUNCTION(Properties_From_2_Different_Devices_Make_CodeFirst_SendAsync_Fail)
    {
        // arrange
        CMocksForCodeFirst mocks;
        SimpleDevice* device1 = (SimpleDevice*)CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &testReflectedData, sizeof(SimpleDevice), false);
        SimpleDevice* device2 = (SimpleDevice*)CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &testReflectedData, sizeof(SimpleDevice), false);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Device_StartTransaction(TEST_DEVICE_HANDLE));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelName(TEST_MODEL_HANDLE));
        EXPECTED_CALL(mocks, Create_AGENT_DATA_TYPE_from_DOUBLE(IGNORED_PTR_ARG, 0.0));
        STRICT_EXPECTED_CALL(mocks, Device_PublishTransacted(TEST_TRANSACTION_HANDLE, "this_is_double", IGNORED_PTR_ARG))
            .IgnoreArgument(3);
        EXPECTED_CALL(mocks, Destroy_AGENT_DATA_TYPE(IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(mocks, Device_CancelTransaction(TEST_TRANSACTION_HANDLE));
        device1->this_is_double = 42.0;
        device2->this_is_double = 42.0;
        unsigned char* destination;
        size_t destinationSize;

        // act
        CODEFIRST_RESULT result = CodeFirst_SendAsync(&destination, &destinationSize, 2, &device1->this_is_double, &device2->this_is_double);

        // assert
        ASSERT_ARE_EQUAL(CODEFIRST_RESULT, CODEFIRST_VALUES_FROM_DIFFERENT_DEVICES_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CodeFirst_DestroyDevice(device1);
        CodeFirst_DestroyDevice(device2);
    }

    /* Tests_SRS_CODEFIRST_99_088:[CodeFirst_SendAsync shall send to the Device module a set of properties.] */
    /* Tests_SRS_CODEFIRST_99_105:[The properties are passed as pointers to the memory locations where the data exists in the device block allocated by CodeFirst_CreateDevice.] */
    /* Tests_SRS_CODEFIRST_99_089:[The numProperties argument shall indicate how many properties are to be sent.] */
    /* Tests_SRS_CODEFIRST_99_090:[All the properties shall be sent together by using the transacted APIs of the device.] */
    /* Tests_SRS_CODEFIRST_99_091:[CodeFirst_SendAsync shall start a transaction by calling Device_StartTransaction.] */
    /* Tests_SRS_CODEFIRST_99_092:[CodeFirst shall publish each value by using Device_PublishTransacted.] */
    /* Tests_SRS_CODEFIRST_99_093:[After all values have been published, Device_EndTransaction shall be called.] */
    /* Tests_SRS_CODEFIRST_99_095:[For each value passed to it, CodeFirst_SendAsync shall look up to which device the value belongs.] */
    /* Tests_SRS_CODEFIRST_99_097:[For each value marshalling to AGENT_DATA_TYPE shall be performed.] */
    /* Tests_SRS_CODEFIRST_99_098:[The marshalling shall be done by calling the Create_AGENT_DATA_TYPE_from_Ptr function associated with the property.] */
    /* Tests_SRS_CODEFIRST_99_117:[On success, CodeFirst_SendAsync shall return CODEFIRST_OK.] */
    TEST_FUNCTION(CodeFirst_SendAsync_With_One_Property_Succeeds_2)
    {
        // arrange
        CMocksForCodeFirst mocks;
        SimpleDevice* device = (SimpleDevice*)CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &testReflectedData, sizeof(SimpleDevice), false);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Device_StartTransaction(TEST_DEVICE_HANDLE));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelName(TEST_MODEL_HANDLE));
        EXPECTED_CALL(mocks, Create_AGENT_DATA_TYPE_from_DOUBLE(IGNORED_PTR_ARG, (double)(IGNORED_PTR_ARG)));
        STRICT_EXPECTED_CALL(mocks, Device_PublishTransacted(TEST_TRANSACTION_HANDLE, "this_is_double", IGNORED_PTR_ARG))
            .IgnoreArgument(3);
        EXPECTED_CALL(mocks, Destroy_AGENT_DATA_TYPE(IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(mocks, Device_EndTransaction(TEST_TRANSACTION_HANDLE, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(2)
            .IgnoreArgument(3);
        device->this_is_double = 42.0;
        unsigned char* destination;
        size_t destinationSize;

        // act
        CODEFIRST_RESULT result = CodeFirst_SendAsync(&destination, &destinationSize, 1, &device->this_is_double);

        // assert
        ASSERT_ARE_EQUAL(CODEFIRST_RESULT, CODEFIRST_OK, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CodeFirst_DestroyDevice(device);
    }

    /* Tests_SRS_CODEFIRST_99_088:[CodeFirst_SendAsync shall send to the Device module a set of properties.] */
    /* Tests_SRS_CODEFIRST_99_105:[The properties are passed as pointers to the memory locations where the data exists in the device block allocated by CodeFirst_CreateDevice.] */
    /* Tests_SRS_CODEFIRST_99_089:[The numProperties argument shall indicate how many properties are to be sent.] */
    /* Tests_SRS_CODEFIRST_99_090:[All the properties shall be sent together by using the transacted APIs of the device.] */
    /* Tests_SRS_CODEFIRST_99_091:[CodeFirst_SendAsync shall start a transaction by calling Device_StartTransaction.] */
    /* Tests_SRS_CODEFIRST_99_092:[CodeFirst shall publish each value by using Device_PublishTransacted.] */
    /* Tests_SRS_CODEFIRST_99_093:[After all values have been published, Device_EndTransaction shall be called.] */
    /* Tests_SRS_CODEFIRST_99_095:[For each value passed to it, CodeFirst_SendAsync shall look up to which device the value belongs.] */
    /* Tests_SRS_CODEFIRST_99_097:[For each value marshalling to AGENT_DATA_TYPE shall be performed.] */
    /* Tests_SRS_CODEFIRST_99_098:[The marshalling shall be done by calling the Create_AGENT_DATA_TYPE_from_Ptr function associated with the property.] */
    /* Tests_SRS_CODEFIRST_99_117:[On success, CodeFirst_SendAsync shall return CODEFIRST_OK.] */
    TEST_FUNCTION(CodeFirst_SendAsync_2_Properties_Succeeds_2)
    {
        // arrange
        CMocksForCodeFirst mocks;
        SimpleDevice* device = (SimpleDevice*)CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &testReflectedData, sizeof(SimpleDevice), false);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Device_StartTransaction(TEST_DEVICE_HANDLE));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelName(TEST_MODEL_HANDLE));
        EXPECTED_CALL(mocks, Create_AGENT_DATA_TYPE_from_DOUBLE(IGNORED_PTR_ARG, (double)(IGNORED_PTR_ARG)));
        STRICT_EXPECTED_CALL(mocks, Device_PublishTransacted(TEST_TRANSACTION_HANDLE, "this_is_double", IGNORED_PTR_ARG))
            .IgnoreArgument(3);
        EXPECTED_CALL(mocks, Destroy_AGENT_DATA_TYPE(IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelName(TEST_MODEL_HANDLE));
        EXPECTED_CALL(mocks, Create_AGENT_DATA_TYPE_from_SINT32(IGNORED_PTR_ARG, (int32_t)(IGNORED_PTR_ARG)));
        STRICT_EXPECTED_CALL(mocks, Device_PublishTransacted(TEST_TRANSACTION_HANDLE, "this_is_int", IGNORED_PTR_ARG))
            .IgnoreArgument(3);
        EXPECTED_CALL(mocks, Destroy_AGENT_DATA_TYPE(IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(mocks, Device_EndTransaction(TEST_TRANSACTION_HANDLE, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(2)
            .IgnoreArgument(3);
        device->this_is_double = 42.0;
        device->this_is_int = 1;
        unsigned char* destination;
        size_t destinationSize;

        // act
        CODEFIRST_RESULT result = CodeFirst_SendAsync(&destination, &destinationSize, 2, &device->this_is_double, &device->this_is_int);

        // assert
        ASSERT_ARE_EQUAL(CODEFIRST_RESULT, CODEFIRST_OK, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CodeFirst_DestroyDevice(device);
    }

    /* Tests_SRS_CODEFIRST_99_130:[If a pointer to the beginning of a device block is passed to CodeFirst_SendAsync instead of a pointer to a property, CodeFirst_SendAsync shall send all the properties that belong to that device.] */
    /* Tests_SRS_CODEFIRST_99_131:[The properties shall be given to Device as one transaction, as if they were all passed as individual arguments to Code_First.] */
    TEST_FUNCTION(CodeFirst_SendAsync_The_Entire_Device_State)
    {
        // arrange
        CMocksForCodeFirst mocks;
        SimpleDevice* device = (SimpleDevice*)CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &testReflectedData, sizeof(SimpleDevice), false);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Schema_GetModelName(TEST_MODEL_HANDLE));
        STRICT_EXPECTED_CALL(mocks, Device_StartTransaction(TEST_DEVICE_HANDLE));
        EXPECTED_CALL(mocks, Create_AGENT_DATA_TYPE_from_DOUBLE(IGNORED_PTR_ARG, (double)(IGNORED_PTR_ARG)));
        STRICT_EXPECTED_CALL(mocks, Device_PublishTransacted(TEST_TRANSACTION_HANDLE, "this_is_double", IGNORED_PTR_ARG))
            .IgnoreArgument(3);
        EXPECTED_CALL(mocks, Destroy_AGENT_DATA_TYPE(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, Create_AGENT_DATA_TYPE_from_SINT32(IGNORED_PTR_ARG, (int32_t)(IGNORED_PTR_ARG)));
        STRICT_EXPECTED_CALL(mocks, Device_PublishTransacted(TEST_TRANSACTION_HANDLE, "this_is_int", IGNORED_PTR_ARG))
            .IgnoreArgument(3);
        EXPECTED_CALL(mocks, Destroy_AGENT_DATA_TYPE(IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(mocks, Device_EndTransaction(TEST_TRANSACTION_HANDLE, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(2)
            .IgnoreArgument(3);
        device->this_is_double = 42.0;
        device->this_is_int = 1;
        unsigned char* destination;
        size_t destinationSize;

        // act
        CODEFIRST_RESULT result = CodeFirst_SendAsync(&destination, &destinationSize, 1, device);

        // assert
        ASSERT_ARE_EQUAL(CODEFIRST_RESULT, CODEFIRST_OK, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CodeFirst_DestroyDevice(device);
    }

    /* Tests_SRS_CODEFIRST_99_099:[If Create_AGENT_DATA_TYPE_from_Ptr fails, CodeFirst_SendAsync shall return CODEFIRST_AGENT_DATA_TYPE_ERROR.] */
    TEST_FUNCTION(CodeFirst_When_Create_Agent_Data_Type_For_The_First_Property_Fails_Send_The_Entire_Device_State_Fails)
    {
        // arrange
        CMocksForCodeFirst mocks;
        SimpleDevice* device = (SimpleDevice*)CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &testReflectedData, sizeof(SimpleDevice), false);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Schema_GetModelName(TEST_MODEL_HANDLE));
        STRICT_EXPECTED_CALL(mocks, Device_StartTransaction(TEST_DEVICE_HANDLE));
        EXPECTED_CALL(mocks, Create_AGENT_DATA_TYPE_from_SINT32(IGNORED_PTR_ARG, (int32_t)(IGNORED_PTR_ARG)))
            .SetReturn(AGENT_DATA_TYPES_ERROR);
        STRICT_EXPECTED_CALL(mocks, Device_CancelTransaction(TEST_TRANSACTION_HANDLE));
        device->this_is_double = 42.0;
        device->this_is_int = 1;
        unsigned char* destination;
        size_t destinationSize;

        // act
        CODEFIRST_RESULT result = CodeFirst_SendAsync(&destination, &destinationSize, 1, device);

        // assert
        ASSERT_ARE_EQUAL(CODEFIRST_RESULT, CODEFIRST_AGENT_DATA_TYPE_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CodeFirst_DestroyDevice(device);
    }

    /* Tests_SRS_CODEFIRST_99_094:[If any Device API fail, CodeFirst_SendAsync shall return CODEFIRST_DEVICE_PUBLISH_FAILED.] */
    TEST_FUNCTION(CodeFirst_When_Publish_For_The_First_Property_Fails_Send_The_Entire_Device_State_Fails)
    {
        // arrange
        CMocksForCodeFirst mocks;
        SimpleDevice* device = (SimpleDevice*)CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &testReflectedData, sizeof(SimpleDevice), false);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Schema_GetModelName(TEST_MODEL_HANDLE));
        STRICT_EXPECTED_CALL(mocks, Device_StartTransaction(TEST_DEVICE_HANDLE));
        EXPECTED_CALL(mocks, Create_AGENT_DATA_TYPE_from_SINT32(IGNORED_PTR_ARG, (int32_t)(IGNORED_PTR_ARG)));
        STRICT_EXPECTED_CALL(mocks, Device_PublishTransacted(TEST_TRANSACTION_HANDLE, "this_is_int", IGNORED_PTR_ARG))
            .IgnoreArgument(3)
            .SetReturn(DEVICE_ERROR);
        EXPECTED_CALL(mocks, Destroy_AGENT_DATA_TYPE(IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(mocks, Device_CancelTransaction(TEST_TRANSACTION_HANDLE));
        device->this_is_double = 42.0;
        device->this_is_int = 1;
        unsigned char* destination;
        size_t destinationSize;

        // act
        CODEFIRST_RESULT result = CodeFirst_SendAsync(&destination, &destinationSize, 1, device);

        // assert
        ASSERT_ARE_EQUAL(CODEFIRST_RESULT, CODEFIRST_DEVICE_PUBLISH_FAILED, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CodeFirst_DestroyDevice(device);
    }

    /* Tests_SRS_CODEFIRST_99_099:[If Create_AGENT_DATA_TYPE_from_Ptr fails, CodeFirst_SendAsync shall return CODEFIRST_AGENT_DATA_TYPE_ERROR.] */
    TEST_FUNCTION(CodeFirst_When_Create_Agent_Data_Type_For_The_Second_Property_Fails_Send_The_Entire_Device_State_Fails)
    {
        // arrange
        CMocksForCodeFirst mocks;
        SimpleDevice* device = (SimpleDevice*)CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &testReflectedData, sizeof(SimpleDevice), false);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Schema_GetModelName(TEST_MODEL_HANDLE));
        STRICT_EXPECTED_CALL(mocks, Device_StartTransaction(TEST_DEVICE_HANDLE));
        EXPECTED_CALL(mocks, Create_AGENT_DATA_TYPE_from_SINT32(IGNORED_PTR_ARG, (int32_t)(IGNORED_PTR_ARG)));
        STRICT_EXPECTED_CALL(mocks, Device_PublishTransacted(TEST_TRANSACTION_HANDLE, "this_is_int", IGNORED_PTR_ARG))
            .IgnoreArgument(3);
        EXPECTED_CALL(mocks, Destroy_AGENT_DATA_TYPE(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, Create_AGENT_DATA_TYPE_from_DOUBLE(IGNORED_PTR_ARG, (double)(IGNORED_PTR_ARG)))
            .SetReturn(AGENT_DATA_TYPES_ERROR);
        STRICT_EXPECTED_CALL(mocks, Device_CancelTransaction(TEST_TRANSACTION_HANDLE));
        device->this_is_double = 42.0;
        device->this_is_int = 1;
        unsigned char* destination;
        size_t destinationSize;

        // act
        CODEFIRST_RESULT result = CodeFirst_SendAsync(&destination, &destinationSize, 1, device);

        // assert
        ASSERT_ARE_EQUAL(CODEFIRST_RESULT, CODEFIRST_AGENT_DATA_TYPE_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CodeFirst_DestroyDevice(device);
    }

    /* Tests_SRS_CODEFIRST_99_094:[If any Device API fail, CodeFirst_SendAsync shall return CODEFIRST_DEVICE_PUBLISH_FAILED.] */
    TEST_FUNCTION(CodeFirst_When_Publish_For_The_Second_Property_Fails_Send_The_Entire_Device_State_Fails)
    {
        // arrange
        CMocksForCodeFirst mocks;
        SimpleDevice* device = (SimpleDevice*)CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &testReflectedData, sizeof(SimpleDevice), false);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Schema_GetModelName(TEST_MODEL_HANDLE));
        STRICT_EXPECTED_CALL(mocks, Device_StartTransaction(TEST_DEVICE_HANDLE));
        EXPECTED_CALL(mocks, Create_AGENT_DATA_TYPE_from_SINT32(IGNORED_PTR_ARG, (int32_t)(IGNORED_PTR_ARG)));
        STRICT_EXPECTED_CALL(mocks, Device_PublishTransacted(TEST_TRANSACTION_HANDLE, "this_is_int", IGNORED_PTR_ARG))
            .IgnoreArgument(3);
        EXPECTED_CALL(mocks, Destroy_AGENT_DATA_TYPE(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, Create_AGENT_DATA_TYPE_from_DOUBLE(IGNORED_PTR_ARG, (double)(IGNORED_PTR_ARG)));
        STRICT_EXPECTED_CALL(mocks, Device_PublishTransacted(TEST_TRANSACTION_HANDLE, "this_is_double", IGNORED_PTR_ARG))
            .IgnoreArgument(3)
            .SetReturn(DEVICE_ERROR);
        EXPECTED_CALL(mocks, Destroy_AGENT_DATA_TYPE(IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(mocks, Device_CancelTransaction(TEST_TRANSACTION_HANDLE));
        device->this_is_double = 42.0;
        device->this_is_int = 1;
        unsigned char* destination;
        size_t destinationSize;

        // act
        CODEFIRST_RESULT result = CodeFirst_SendAsync(&destination, &destinationSize, 1, device);

        // assert
        ASSERT_ARE_EQUAL(CODEFIRST_RESULT, CODEFIRST_DEVICE_PUBLISH_FAILED, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CodeFirst_DestroyDevice(device);
    }

    /* Tests_SRS_CODEFIRST_99_133:[CodeFirst_SendAsync shall allow sending of properties that are part of a child model.] */
    /* Tests_SRS_CODEFIRST_99_136:[CodeFirst_SendAsync shall build the full path for each property and then pass it to Device_PublishTransacted.] */
    TEST_FUNCTION(CodeFirst_CodeFirst_SendAsync_Can_Send_A_Property_From_A_Child_Model)
    {
        // arrange
        CMocksForCodeFirst mocks;
        OuterType* device = (OuterType*)CodeFirst_CreateDevice(TEST_OUTERTYPE_MODEL_HANDLE, &testModelInModelReflectedData, sizeof(OuterType), false);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Schema_GetModelName(TEST_OUTERTYPE_MODEL_HANDLE)).SetReturn("OuterType");
        STRICT_EXPECTED_CALL(mocks, Device_StartTransaction(TEST_DEVICE_HANDLE));
        EXPECTED_CALL(mocks, Create_AGENT_DATA_TYPE_from_DOUBLE(IGNORED_PTR_ARG, (double)(IGNORED_PTR_ARG)));
        STRICT_EXPECTED_CALL(mocks, Device_PublishTransacted(TEST_TRANSACTION_HANDLE, "Inner/this_is_double", IGNORED_PTR_ARG))
            .IgnoreArgument(3);
        EXPECTED_CALL(mocks, Destroy_AGENT_DATA_TYPE(IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(mocks, Device_EndTransaction(TEST_TRANSACTION_HANDLE, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(2)
            .IgnoreArgument(3);
        device->Inner.this_is_double = 42.0;
        unsigned char* destination;
        size_t destinationSize;

        // act
        CODEFIRST_RESULT result = CodeFirst_SendAsync(&destination, &destinationSize, 1, &device->Inner.this_is_double);

        // assert
        ASSERT_ARE_EQUAL(CODEFIRST_RESULT, CODEFIRST_OK, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CodeFirst_DestroyDevice(device);
    }

    /* Tests_SRS_CODEFIRST_99_133:[CodeFirst_SendAsync shall allow sending of properties that are part of a child model.] */
    /* Tests_SRS_CODEFIRST_99_136:[CodeFirst_SendAsync shall build the full path for each property and then pass it to Device_PublishTransacted.] */
    TEST_FUNCTION(CodeFirst_CodeFirst_SendAsync_Can_Send_The_Last_Property_From_A_Child_Model_With_2_Properties)
    {
        // arrange
        CMocksForCodeFirst mocks;
        OuterType* device = (OuterType*)CodeFirst_CreateDevice(TEST_OUTERTYPE_MODEL_HANDLE, &testModelInModelReflectedData, sizeof(OuterType), false);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Schema_GetModelName(TEST_OUTERTYPE_MODEL_HANDLE)).SetReturn("OuterType");
        STRICT_EXPECTED_CALL(mocks, Device_StartTransaction(TEST_DEVICE_HANDLE));
        EXPECTED_CALL(mocks, Create_AGENT_DATA_TYPE_from_SINT32(IGNORED_PTR_ARG, (int32_t)(IGNORED_PTR_ARG)));
        STRICT_EXPECTED_CALL(mocks, Device_PublishTransacted(TEST_TRANSACTION_HANDLE, "Inner/this_is_int", IGNORED_PTR_ARG))
            .IgnoreArgument(3);
        EXPECTED_CALL(mocks, Destroy_AGENT_DATA_TYPE(IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(mocks, Device_EndTransaction(TEST_TRANSACTION_HANDLE, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument(2)
            .IgnoreArgument(3);
        device->Inner.this_is_int = 1;
        unsigned char* destination;
        size_t destinationSize;

        // act
        CODEFIRST_RESULT result = CodeFirst_SendAsync(&destination, &destinationSize, 1, &device->Inner.this_is_int);

        // assert
        ASSERT_ARE_EQUAL(CODEFIRST_RESULT, CODEFIRST_OK, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CodeFirst_DestroyDevice(device);
    }

    /* Tests_SRS_CODEFIRST_04_002: [If CodeFirst_SendAsync receives destination or destinationSize NULL, CodeFirst_SendAsync shall return Invalid Argument.]*/
    TEST_FUNCTION(CodeFirst_SendAsync_With_NULL_destination_and_NonNulldestinationSize_Fails)
    {
        // arrange
        CMocksForCodeFirst mocks;
        SimpleDevice* device = (SimpleDevice*)CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &testReflectedData, sizeof(SimpleDevice), false);
        size_t destinationSize;
        mocks.ResetAllCalls();

        // act
        CODEFIRST_RESULT result = CodeFirst_SendAsync(NULL, &destinationSize, 1, &device->this_is_double);

        // assert
        ASSERT_ARE_EQUAL(CODEFIRST_RESULT, CODEFIRST_INVALID_ARG, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CodeFirst_DestroyDevice(device);
    }

    /* Tests_SRS_CODEFIRST_04_002: [If CodeFirst_SendAsync receives destination or destinationSize NULL, CodeFirst_SendAsync shall return Invalid Argument.]*/
    TEST_FUNCTION(CodeFirst_SendAsync_With_nonNULL_destination_and_NulldestinationSize_Fails)
    {
        // arrange
        CMocksForCodeFirst mocks;
        SimpleDevice* device = (SimpleDevice*)CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &testReflectedData, sizeof(SimpleDevice), false);
        unsigned char* destination;
        mocks.ResetAllCalls();

        // act
        CODEFIRST_RESULT result = CodeFirst_SendAsync(&destination, NULL, 1, &device->this_is_double);

        // assert
        ASSERT_ARE_EQUAL(CODEFIRST_RESULT, CODEFIRST_INVALID_ARG, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CodeFirst_DestroyDevice(device);
    }

    /*Tests_SRS_CODEFIRST_99_088:[CodeFirst_SendAsync shall send to the Device module a set of properties, a destination and a destinationSize.] */
    TEST_FUNCTION(CodeFirst_SendAsync_With_One_Property_CallBackAndUserContext_Succeeds)
    {
        // arrange
        CMocksForCodeFirst mocks;
        SimpleDevice* device = (SimpleDevice*)CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &testReflectedData, sizeof(SimpleDevice), false);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Device_StartTransaction(TEST_DEVICE_HANDLE));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelName(TEST_MODEL_HANDLE));
        EXPECTED_CALL(mocks, Create_AGENT_DATA_TYPE_from_DOUBLE(IGNORED_PTR_ARG, 0.0));
        STRICT_EXPECTED_CALL(mocks, Device_PublishTransacted(TEST_TRANSACTION_HANDLE, "this_is_double", IGNORED_PTR_ARG))
            .IgnoreArgument(3);
        EXPECTED_CALL(mocks, Destroy_AGENT_DATA_TYPE(IGNORED_PTR_ARG));
        unsigned char* destination;
        size_t destinationSize;
        STRICT_EXPECTED_CALL(mocks, Device_EndTransaction(TEST_TRANSACTION_HANDLE, &destination, &destinationSize));
        device->this_is_double = 42.0;


        // act
        CODEFIRST_RESULT result = CodeFirst_SendAsync(&destination, &destinationSize, 1, &device->this_is_double);

        // assert
        ASSERT_ARE_EQUAL(CODEFIRST_RESULT, CODEFIRST_OK, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CodeFirst_DestroyDevice(device);
    }

    /* CodeFirst_RegisterSchema */
    /* Tests_SRS_CODEFIRST_99_002:[ CodeFirst_RegisterSchema shall create the schema information and give it to the Schema module for one schema, identified by the metadata argument. On success, it shall return a handle to the model.] */
    TEST_FUNCTION(CodeFirst_RegisterSchema_succeeds)
    {
        CMocksForCodeFirst mocks;
        static const SCHEMA_STRUCT_TYPE_HANDLE TEST_CAR_BEHIND_VAN_HANDLE = (SCHEMA_STRUCT_TYPE_HANDLE)0x7001;

        ///arrange
        STRICT_EXPECTED_CALL(mocks, Schema_GetSchemaByNamespace("TestSchema"));
        STRICT_EXPECTED_CALL(mocks, Schema_Create("TestSchema"));
        STRICT_EXPECTED_CALL(mocks, Schema_CreateStructType(TEST_SCHEMA_HANDLE, "GeoLocation"));
        STRICT_EXPECTED_CALL(mocks, Schema_AddStructTypeProperty(TEST_STRUCT_TYPE_HANDLE, "Lat", "double"));
        STRICT_EXPECTED_CALL(mocks, Schema_AddStructTypeProperty(TEST_STRUCT_TYPE_HANDLE, "Long", "double"));
        STRICT_EXPECTED_CALL(mocks, Schema_CreateStructType(TEST_SCHEMA_HANDLE, "theCarIsBehindTheVan"))
            .SetReturn(TEST_CAR_BEHIND_VAN_HANDLE);
        STRICT_EXPECTED_CALL(mocks, Schema_AddStructTypeProperty(TEST_CAR_BEHIND_VAN_HANDLE, "Alt", "int"));
        STRICT_EXPECTED_CALL(mocks, Schema_AddStructTypeProperty(TEST_CAR_BEHIND_VAN_HANDLE, "whereIsMyCar", "GeoLocation"));
        STRICT_EXPECTED_CALL(mocks, Schema_CreateModelType(TEST_SCHEMA_HANDLE, "TruckType"))
            .SetReturn(TEST_TRUCKTYPE_MODEL_HANDLE);
        STRICT_EXPECTED_CALL(mocks, Schema_CreateModelType(TEST_SCHEMA_HANDLE, "SimpleDevice"));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelByName(TEST_SCHEMA_HANDLE, "TruckType")).SetReturn(TEST_TRUCKTYPE_MODEL_HANDLE);
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelByName(TEST_SCHEMA_HANDLE, "SimpleDevice")).SetReturn(TEST_MODEL_HANDLE);
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelByName(TEST_SCHEMA_HANDLE, "int")).SetReturn((SCHEMA_MODEL_TYPE_HANDLE)NULL);
        STRICT_EXPECTED_CALL(mocks, Schema_AddModelProperty(TEST_MODEL_HANDLE, "this_is_int", "int"));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelByName(TEST_SCHEMA_HANDLE, "double")).SetReturn((SCHEMA_MODEL_TYPE_HANDLE)NULL);
        STRICT_EXPECTED_CALL(mocks, Schema_AddModelProperty(TEST_MODEL_HANDLE, "this_is_double", "double"));
        STRICT_EXPECTED_CALL(mocks, Schema_CreateModelAction(TEST_TRUCKTYPE_MODEL_HANDLE, "setSpeed"))
            .SetReturn(SETSPEED_ACTION_HANDLE);
        STRICT_EXPECTED_CALL(mocks, Schema_AddModelActionArgument(SETSPEED_ACTION_HANDLE, "theSpeed", "double"));
        STRICT_EXPECTED_CALL(mocks, Schema_CreateModelAction(TEST_TRUCKTYPE_MODEL_HANDLE, "reset"))
            .SetReturn(RESET_ACTION_HANDLE);

        ///act
        SCHEMA_HANDLE result = CodeFirst_RegisterSchema("TestSchema", &testReflectedData);

        ///assert
        ASSERT_ARE_EQUAL(void_ptr, TEST_SCHEMA_HANDLE, result);
    }

    /* Tests_SRS_CODEFIRST_99_002:[ CodeFirst_RegisterSchema shall create the schema information and give it to the Schema module for one schema, identified by the metadata argument. On success, it shall return a handle to the model.] */
    TEST_FUNCTION(CodeFirst_RegisterSchema_With_Model_In_Model_succeeds)
    {
        CMocksForCodeFirst mocks;
        static const SCHEMA_STRUCT_TYPE_HANDLE TEST_CAR_BEHIND_VAN_HANDLE = (SCHEMA_STRUCT_TYPE_HANDLE)0x7001;

        ///arrange
        STRICT_EXPECTED_CALL(mocks, Schema_GetSchemaByNamespace("TestSchema"));
        STRICT_EXPECTED_CALL(mocks, Schema_Create("TestSchema"));
        STRICT_EXPECTED_CALL(mocks, Schema_CreateModelType(TEST_SCHEMA_HANDLE, "InnerType"))
            .SetReturn(TEST_INNERTYPE_MODEL_HANDLE);
        STRICT_EXPECTED_CALL(mocks, Schema_CreateModelType(TEST_SCHEMA_HANDLE, "OuterType"))
            .SetReturn(TEST_OUTERTYPE_MODEL_HANDLE);
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelByName(TEST_SCHEMA_HANDLE, "InnerType"))
            .SetReturn(TEST_INNERTYPE_MODEL_HANDLE);
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelByName(TEST_SCHEMA_HANDLE, "double"));
        STRICT_EXPECTED_CALL(mocks, Schema_AddModelProperty(TEST_INNERTYPE_MODEL_HANDLE, "this_is_double", "double"));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelByName(TEST_SCHEMA_HANDLE, "int"));
        STRICT_EXPECTED_CALL(mocks, Schema_AddModelProperty(TEST_INNERTYPE_MODEL_HANDLE, "this_is_int", "int"));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelByName(TEST_SCHEMA_HANDLE, "OuterType"))
            .SetReturn(TEST_OUTERTYPE_MODEL_HANDLE);
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelByName(TEST_SCHEMA_HANDLE, "InnerType"))
            .SetReturn(TEST_INNERTYPE_MODEL_HANDLE);
        STRICT_EXPECTED_CALL(mocks, Schema_AddModelModel(TEST_OUTERTYPE_MODEL_HANDLE, "Inner", TEST_INNERTYPE_MODEL_HANDLE));
        STRICT_EXPECTED_CALL(mocks, Schema_CreateModelAction(TEST_OUTERTYPE_MODEL_HANDLE, "reset"));
        STRICT_EXPECTED_CALL(mocks, Schema_CreateModelAction(TEST_INNERTYPE_MODEL_HANDLE, "reset"));

        ///act
        SCHEMA_HANDLE result = CodeFirst_RegisterSchema("TestSchema", &testModelInModelReflectedData);

        ///assert
        ASSERT_ARE_EQUAL(void_ptr, TEST_SCHEMA_HANDLE, result);
    }

    /* Tests_SRS_CODEFIRST_99_076:[If any Schema APIs fail, CodeFirst_RegisterSchema shall return NULL.] */
    TEST_FUNCTION(When_Schema_Create_Fails_Then_CodeFirst_RegisterSchema_Fails)
    {
        CMocksForCodeFirst mocks;

        ///arrange
        STRICT_EXPECTED_CALL(mocks, Schema_GetSchemaByNamespace("TestSchema"));
        STRICT_EXPECTED_CALL(mocks, Schema_Create("TestSchema"))
            .SetReturn((SCHEMA_HANDLE)NULL);

        ///act
        SCHEMA_HANDLE result = CodeFirst_RegisterSchema("TestSchema", &testReflectedData);

        ///assert
        ASSERT_IS_NULL(result);
    }

    /* Tests_SRS_CODEFIRST_99_076:[If any Schema APIs fail, CodeFirst_RegisterSchema shall return NULL.] */
    TEST_FUNCTION(When_Schema_CreateModelType_Fails_Then_CodeFirst_RegisterSchema_Fails)
    {
        CNiceCallComparer<CMocksForCodeFirst> mocks;

        ///arrange
        STRICT_EXPECTED_CALL(mocks, Schema_Create("TestSchema"));
        STRICT_EXPECTED_CALL(mocks, Schema_CreateModelType(TEST_SCHEMA_HANDLE, TEST_DEFAULT_MODEL_NAME))
            .SetReturn((SCHEMA_MODEL_TYPE_HANDLE)NULL);
        STRICT_EXPECTED_CALL(mocks, Schema_Destroy(TEST_SCHEMA_HANDLE));

        ///act
        SCHEMA_HANDLE result = CodeFirst_RegisterSchema("TestSchema", &testReflectedData);

        ///assert
        ASSERT_IS_NULL(result);
    }

    /* Tests_SRS_CODEFIRST_99_076:[If any Schema APIs fail, CodeFirst_RegisterSchema shall return NULL.] */
    TEST_FUNCTION(When_Schema_AddModelProperty_Fails_Then_CodeFirst_RegisterSchema_Fails)
    {
        CNiceCallComparer<CMocksForCodeFirst> mocks;

        ///arrange
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelByName(TEST_SCHEMA_HANDLE, "TruckType")).SetReturn(TEST_MODEL_HANDLE);
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelByName(TEST_SCHEMA_HANDLE, "SimpleDevice")).SetReturn(TEST_MODEL_HANDLE);
        STRICT_EXPECTED_CALL(mocks, Schema_AddModelProperty(TEST_MODEL_HANDLE, "this_is_int", "int"))
            .SetReturn(SCHEMA_ERROR);
        STRICT_EXPECTED_CALL(mocks, Schema_Destroy(TEST_SCHEMA_HANDLE));

        ///act
        SCHEMA_HANDLE result = CodeFirst_RegisterSchema("TestSchema", &testReflectedData);

        ///assert
        ASSERT_IS_NULL(result);
    }

    /* Tests_SRS_CODEFIRST_99_076:[If any Schema APIs fail, CodeFirst_RegisterSchema shall return NULL.] */
    TEST_FUNCTION(When_Schema_CreateModelAction_Fails_Then_CodeFirst_RegisterSchema_Fails)
    {
        CNiceCallComparer<CMocksForCodeFirst> mocks;

        ///arrange
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelByName(TEST_SCHEMA_HANDLE, "TruckType")).SetReturn(TEST_MODEL_HANDLE);
        STRICT_EXPECTED_CALL(mocks, Schema_CreateModelAction(TEST_MODEL_HANDLE, "setSpeed"))
            .SetReturn((SCHEMA_ACTION_HANDLE)NULL);
        STRICT_EXPECTED_CALL(mocks, Schema_Destroy(TEST_SCHEMA_HANDLE));

        ///act
        SCHEMA_HANDLE result = CodeFirst_RegisterSchema("TestSchema", &testReflectedData);

        ///assert
        ASSERT_IS_NULL(result);
    }

    /* Tests_SRS_CODEFIRST_99_076:[If any Schema APIs fail, CodeFirst_RegisterSchema shall return NULL.] */
    TEST_FUNCTION(When_Schema_AddModelActionArgument_Fails_Then_CodeFirst_RegisterSchema_Fails)
    {
        CNiceCallComparer<CMocksForCodeFirst> mocks;

        ///arrange
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelByName(TEST_SCHEMA_HANDLE, "TruckType")).SetReturn(TEST_MODEL_HANDLE);
        STRICT_EXPECTED_CALL(mocks, Schema_CreateModelAction(TEST_MODEL_HANDLE, "setSpeed"))
            .SetReturn(SETSPEED_ACTION_HANDLE);
        STRICT_EXPECTED_CALL(mocks, Schema_AddModelActionArgument(SETSPEED_ACTION_HANDLE, "theSpeed", "double"))
            .SetReturn(SCHEMA_ERROR);
        STRICT_EXPECTED_CALL(mocks, Schema_Destroy(TEST_SCHEMA_HANDLE));

        ///act
        SCHEMA_HANDLE result = CodeFirst_RegisterSchema("TestSchema", &testReflectedData);

        ///assert
        ASSERT_IS_NULL(result);
    }

    /* Tests_SRS_CODEFIRST_99_076:[If any Schema APIs fail, CodeFirst_RegisterSchema shall return NULL.] */
    TEST_FUNCTION(When_Schema_CreateStructType_Fails_Then_CodeFirst_RegisterSchema_Fails)
    {
        CNiceCallComparer<CMocksForCodeFirst> mocks;

        ///arrange
        STRICT_EXPECTED_CALL(mocks, Schema_Create("TestSchema"));
        STRICT_EXPECTED_CALL(mocks, Schema_CreateStructType(TEST_SCHEMA_HANDLE, "GeoLocation"))
            .SetReturn((SCHEMA_STRUCT_TYPE_HANDLE)NULL);
        STRICT_EXPECTED_CALL(mocks, Schema_Destroy(TEST_SCHEMA_HANDLE));

        ///act
        SCHEMA_HANDLE result = CodeFirst_RegisterSchema("TestSchema", &testReflectedData);

        ///assert
        ASSERT_IS_NULL(result);
    }

    /* Tests_SRS_CODEFIRST_99_076:[If any Schema APIs fail, CodeFirst_RegisterSchema shall return NULL.] */
    TEST_FUNCTION(When_Schema_AddStructTypeProperty_Fails_Then_CodeFirst_RegisterSchema_Fails)
    {
        CNiceCallComparer<CMocksForCodeFirst> mocks;

        ///arrange
        STRICT_EXPECTED_CALL(mocks, Schema_Create("TestSchema"));
        STRICT_EXPECTED_CALL(mocks, Schema_AddStructTypeProperty(TEST_STRUCT_TYPE_HANDLE, "Lat", "double"))
            .SetReturn(SCHEMA_ERROR);
        STRICT_EXPECTED_CALL(mocks, Schema_Destroy(TEST_SCHEMA_HANDLE));

        ///act
        SCHEMA_HANDLE result = CodeFirst_RegisterSchema("TestSchema", &testReflectedData);

        ///assert
        ASSERT_IS_NULL(result);
    }

    /* Tests_SRS_CODEFIRST_99_076:[If any Schema APIs fail, CodeFirst_RegisterSchema shall return NULL.] */
    TEST_FUNCTION(When_Schema_GetModelByName_Returns_NULL_RegisterSchema_Fails)
    {
        CNiceCallComparer<CMocksForCodeFirst> mocks;

        ///arrange
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelByName(TEST_SCHEMA_HANDLE, "TruckType"))
            .SetReturn((SCHEMA_MODEL_TYPE_HANDLE)NULL);
        STRICT_EXPECTED_CALL(mocks, Schema_Destroy(TEST_SCHEMA_HANDLE));

        ///act
        SCHEMA_HANDLE result = CodeFirst_RegisterSchema("TestSchema", &testReflectedData);

        ///assert
        ASSERT_IS_NULL(result);
    }

    /* Tests_SRS_CODEFIRST_99_121:[If the schema has already been registered, CodeFirst_RegisterSchema shall return its handle.] */
    TEST_FUNCTION(When_Schema_Was_Already_Registered_CodeFirst_Returns_Its_Handle)
    {
        CMocksForCodeFirst mocks;

        ///arrange
        STRICT_EXPECTED_CALL(mocks, Schema_GetSchemaByNamespace("TestSchema"))
            .SetReturn((SCHEMA_HANDLE)TEST_SCHEMA_HANDLE);

        ///act
        SCHEMA_HANDLE result = CodeFirst_RegisterSchema("TestSchema", &testReflectedData);

        ///assert
        ASSERT_ARE_EQUAL(void_ptr, TEST_SCHEMA_HANDLE, result);
    }

    /*Tests_SRS_CODEFIRST_02_014: [If parameter device or command is NULL then CodeFirst_ExecuteCommand shall return EXECUTE_COMMAND_ERROR.] */
    TEST_FUNCTION(CodeFirst_ExecuteCommand_With_NULL_device_fails)
    {
        ///arrange

        ///act
        EXECUTE_COMMAND_RESULT result = CodeFirst_ExecuteCommand(NULL, TEST_COMMAND);

        ///assert
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);

        ///cleanup
    }

    /*Tests_SRS_CODEFIRST_02_014: [If parameter device or command is NULL then CodeFirst_ExecuteCommand shall return EXECUTE_COMMAND_ERROR.] */
    TEST_FUNCTION(CodeFirst_ExecuteCommand_With_NULL_command_fails)
    {
        ///arrange
        CMocksForCodeFirst mocks;
        void* device = CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &DummyDataProvider_allReflected, sizeof(TruckType), false);
        mocks.ResetAllCalls();

        ///act
        auto result = CodeFirst_ExecuteCommand(device, NULL);

        ///assert
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        CodeFirst_DestroyDevice(device);
    }

    /*Tests_SRS_CODEFIRST_02_015: [CodeFirst_ExecuteCommand shall find the device.]*/
    /*Tests_SRS_CODEFIRST_02_016: [If finding the device fails, then CodeFirst_ExecuteCommand shall return EXECUTE_COMMAND_ERROR.] */
    TEST_FUNCTION(CodeFirst_ExecuteCommand_fails_when_it_does_not_find_the_device)
    {
        ///arrange
        CMocksForCodeFirst mocks;
        mocks.ResetAllCalls();

        ///act
        auto result = CodeFirst_ExecuteCommand((unsigned char*)NULL +1, TEST_COMMAND);

        ///assert
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
    }

    /*Tests_SRS_CODEFIRST_02_017: [Otherwise CodeFirst_ExecuteCommand shall call Device_ExecuteCommand and return what Device_ExecuteCommand is returning.] */
    TEST_FUNCTION(CodeFirst_ExecuteCommand_calls_Device_ExecuteCommand)
    {
        ///arrange
        CMocksForCodeFirst mocks;
        void* device = CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &DummyDataProvider_allReflected, sizeof(TruckType), false);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Device_ExecuteCommand(IGNORED_PTR_ARG, TEST_COMMAND))
            .IgnoreArgument(1);

        ///act
        auto result = CodeFirst_ExecuteCommand(device, TEST_COMMAND);

        ///assert
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_SUCCESS, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        CodeFirst_DestroyDevice(device);
    }

    /*Tests_SRS_CODEFIRST_02_017: [Otherwise CodeFirst_ExecuteCommand shall call Device_ExecuteCommand and return what Device_ExecuteCommand is returning.] */
    TEST_FUNCTION(CodeFirst_ExecuteCommand_calls_Device_ExecuteCommand_can_returns_EXECUTE_COMMAND_FAILED)
    {
        ///arrange
        CMocksForCodeFirst mocks;
        void* device = CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &DummyDataProvider_allReflected, sizeof(TruckType), false);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Device_ExecuteCommand(IGNORED_PTR_ARG, TEST_COMMAND))
            .IgnoreArgument(1)
            .SetReturn(EXECUTE_COMMAND_FAILED);

        ///act
        auto result = CodeFirst_ExecuteCommand(device, TEST_COMMAND);

        ///assert
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_FAILED, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        CodeFirst_DestroyDevice(device);
    }

    /*Tests_SRS_CODEFIRST_02_017: [Otherwise CodeFirst_ExecuteCommand shall call Device_ExecuteCommand and return what Device_ExecuteCommand is returning.] */
    TEST_FUNCTION(CodeFirst_ExecuteCommand_calls_Device_ExecuteCommand_can_returns_EXECUTE_COMMAND_ERROR)
    {
        ///arrange
        CMocksForCodeFirst mocks;
        void* device = CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &DummyDataProvider_allReflected, sizeof(TruckType), false);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, Device_ExecuteCommand(IGNORED_PTR_ARG, TEST_COMMAND))
            .IgnoreArgument(1)
            .SetReturn(EXECUTE_COMMAND_ERROR);

        ///act
        auto result = CodeFirst_ExecuteCommand(device, TEST_COMMAND);

        ///assert
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        ///cleanup
        CodeFirst_DestroyDevice(device);
    }

END_TEST_SUITE(CodeFirst_ut_Dummy_Data_Provider);
