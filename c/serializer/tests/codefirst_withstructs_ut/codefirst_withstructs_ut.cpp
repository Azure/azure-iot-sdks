// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <cstdlib>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "testrunnerswitcher.h"
#include "codefirst.h"
#include "azure_c_shared_utility/macro_utils.h"
#include "micromock.h"
#include "micromockcharstararenullterminatedstrings.h"
#include "schema.h"
#include "iotdevice.h"
#include "serializer.h"
#include "iotdevice.h"


DEFINE_MICROMOCK_ENUM_TO_STRING(IOT_AGENT_RESULT, IOT_AGENT_RESULT_ENUM_VALUES)
DEFINE_MICROMOCK_ENUM_TO_STRING(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_RESULT_VALUES);

static const char TEST_MODEL_NAME[] = "SimpleDevice";

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

BEGIN_NAMESPACE(DummyDataProvider_WithStructs)

/*Tests_SRS_SERIALIZER_99_016:[ The second macro, DECLARE_STRUCT shall introduce a new struct  type having the fields field1name of type field1Type, field2name having the type field2type etc.]*/
DECLARE_STRUCT(GeoLocation,
double, Lat,
double, Long);

/*Tests_SRS_SERIALIZER_99_016:[ The second macro, DECLARE_STRUCT shall introduce a new struct  type having the fields field1name of type field1Type, field2name having the type field2type etc.]*/
DECLARE_STRUCT(CarLocation,
int, Alt,
/*Tests_SRS_SERIALIZER_99_017:[ These types can either be one of the types mentioned in WITH_DATA or it can be a type introduced by a previous DECLARE_STRUCT.]*/
GeoLocation, whereIsMyCar);

DECLARE_MODEL(TruckType,
WITH_DATA(double, this_is_double),
WITH_DATA(GeoLocation, whereIsMyDevice),
WITH_DATA(CarLocation, theCarIsBehindTheVan),

/*a simple action that takes as parameter a struct*/
WITH_ACTION(goToLocation, GeoLocation, XMarksTheSpot),

/*a more complex action that takes as parameter 1xsimple type, 1x complex type (that has another complex type embedded) and another simple type*/
WITH_ACTION(moveCarTo, int, speed, CarLocation, destination, bool, reverse),
WITH_ACTION(alwaysReject),
WITH_ACTION(alwaysAbandon)
);


END_NAMESPACE(DummyDataProvider_WithStructs)

BEGIN_NAMESPACE(TheSecondDummyDataProvider_WithStructs)

DECLARE_STRUCT(FamilyAccount,
int16_t, FatherMoney,
int8_t, MotherMoney);

DECLARE_STRUCT(MoneyInTheWorld,
FamilyAccount, moneyInCanada,
FamilyAccount, moneyInRomania,
FamilyAccount, moneyInUSA);

DECLARE_MODEL(someModel,
WITH_DATA(int8_t, myAccount),
WITH_DATA(FamilyAccount, UsMoney),
WITH_DATA(MoneyInTheWorld, allTheMoney)
);

END_NAMESPACE(TheSecondDummyDataProvider_WithStructs)

bool goToLocation_wasCalled;
double goToLocation_XMarksTheSpot_long;
double goToLocation_XMarksTheSpot_lat;
EXECUTE_COMMAND_RESULT goToLocation(TruckType* device, GeoLocation XMarksTheSpot)
{
    (void)device;

    goToLocation_wasCalled = true;
    goToLocation_XMarksTheSpot_long = XMarksTheSpot.Long;
    goToLocation_XMarksTheSpot_lat = XMarksTheSpot.Lat;
    return EXECUTE_COMMAND_SUCCESS; 

}

bool moveCarTo_wasCalled;
int moveCarTo_speed;
int moveCarTo_destination_Alt;
double moveCarTo_destination_whereIsMyCar_Lat;
double moveCarTo_destination_whereIsMyCar_Long;
bool moveCarTo_reverse;
EXECUTE_COMMAND_RESULT moveCarTo(TruckType* device, int speed, CarLocation destination, bool reverse)
{
    (void)device;
    moveCarTo_wasCalled = true;
    moveCarTo_speed = speed;
    moveCarTo_destination_Alt = destination.Alt;
    moveCarTo_destination_whereIsMyCar_Lat = destination.whereIsMyCar.Lat;
    moveCarTo_destination_whereIsMyCar_Long = destination.whereIsMyCar.Long;
    moveCarTo_reverse = reverse;
    return EXECUTE_COMMAND_SUCCESS;
}

EXECUTE_COMMAND_RESULT alwaysReject(TruckType* device)
{
    (void)(device);
    return EXECUTE_COMMAND_FAILED;
}
EXECUTE_COMMAND_RESULT alwaysAbandon(TruckType* device)
{
    (void)(device);
    return EXECUTE_COMMAND_ERROR;
}

static const SCHEMA_HANDLE TEST_SCHEMA_HANDLE = (SCHEMA_MODEL_TYPE_HANDLE)0x4242;
static const SCHEMA_MODEL_TYPE_HANDLE TEST_MODEL_HANDLE = (SCHEMA_MODEL_TYPE_HANDLE)0x4243;
static const SCHEMA_STRUCT_TYPE_HANDLE TEST_STRUCT_TYPE = (SCHEMA_STRUCT_TYPE_HANDLE)0x4244;
static const SCHEMA_ACTION_HANDLE TEST_ACTION_HANDLE = (SCHEMA_ACTION_HANDLE)0x4245;
static const DEVICE_HANDLE TEST_DEVICE_HANDLE = (DEVICE_HANDLE)0x4848;
static void* g_InvokeActionCallbackArgument;

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

static MICROMOCK_MUTEX_HANDLE g_testByTest;

#define MAX_NAME_LENGTH 100
typedef char someName[MAX_NAME_LENGTH];
#define MAX_RECORDINGS 100

static  AGENT_DATA_TYPE* Create_EDM_BOOLEAN_from_int_agentData = NULL;
static  AGENT_DATA_TYPE* Create_AGENT_DATA_TYPE_from_UINT8_agentData = NULL;
static  AGENT_DATA_TYPE* Create_AGENT_DATA_TYPE_from_DOUBLE_agentData[MAX_RECORDINGS];
static  double Create_AGENT_DATA_TYPE_from_DOUBLE_values[MAX_RECORDINGS];
static size_t nCreate_AGENT_DATA_TYPE_from_DOUBLE_agentData;
static  AGENT_DATA_TYPE* Create_AGENT_DATA_TYPE_from_SINT16_agentData = NULL;
static  AGENT_DATA_TYPE* Create_AGENT_DATA_TYPE_from_SINT32_agentData = NULL;
static  AGENT_DATA_TYPE* Create_AGENT_DATA_TYPE_from_SINT64_agentData = NULL;
static  AGENT_DATA_TYPE* Create_AGENT_DATA_TYPE_from_SINT8_agentData = NULL;
static  AGENT_DATA_TYPE* Create_AGENT_DATA_TYPE_from_FLOAT_agentData = NULL;
static  AGENT_DATA_TYPE* Create_AGENT_DATA_TYPE_from_charz_agentData = NULL;
static  AGENT_DATA_TYPE* Create_AGENT_DATA_TYPE_from_charz_agentData_no_quotes = NULL;
static AGENT_DATA_TYPE* Create_AGENT_DATA_TYPE_from_Members_agentData = NULL;
static someName Create_AGENT_DATA_TYPE_from_Members_names[MAX_RECORDINGS];
static const AGENT_DATA_TYPE* Create_AGENT_DATA_TYPE_from_Members_values[MAX_RECORDINGS];
static const AGENT_DATA_TYPE* Device_Publish_agentData = NULL;
static const AGENT_DATA_TYPE* Device_PublishTransacted_agentData = NULL;
static const AGENT_DATA_TYPE* Destroy_AGENT_DATA_TYPE_agentData[MAX_RECORDINGS];
static size_t nDestroy_AGENT_DATA_TYPE_agentData;
static  AGENT_DATA_TYPE* Create_AGENT_DATA_TYPE_from_EDM_DATE_TIME_OFFSET_agentData = NULL;
static  AGENT_DATA_TYPE* Create_AGENT_DATA_TYPE_from_EDM_GUID_agentData = NULL;
static  AGENT_DATA_TYPE* Create_AGENT_DATA_TYPE_from_EDM_BINARY_agentData = NULL;

TYPED_MOCK_CLASS(CCodeFirstMocks, CGlobalMock)
{
public:
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
        Create_AGENT_DATA_TYPE_from_DOUBLE_agentData[nCreate_AGENT_DATA_TYPE_from_DOUBLE_agentData] = agentData;
        Create_AGENT_DATA_TYPE_from_DOUBLE_values[nCreate_AGENT_DATA_TYPE_from_DOUBLE_agentData] = v;
        nCreate_AGENT_DATA_TYPE_from_DOUBLE_agentData++;
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
        Create_AGENT_DATA_TYPE_from_charz_agentData_no_quotes = agentData;
    }
    MOCK_METHOD_END(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK);

    MOCK_STATIC_METHOD_1(, void, Destroy_AGENT_DATA_TYPE, AGENT_DATA_TYPE*, agentData)
    {
        Destroy_AGENT_DATA_TYPE_agentData[nDestroy_AGENT_DATA_TYPE_agentData++] = agentData;
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

    MOCK_STATIC_METHOD_3(, DEVICE_RESULT, Device_PublishTransacted, TRANSACTION_HANDLE, transactionHandle, const char*, propertyName, const AGENT_DATA_TYPE*, data)
    {
        Device_PublishTransacted_agentData = data;
    }
    MOCK_METHOD_END(DEVICE_RESULT, DEVICE_OK);

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
    MOCK_METHOD_END(TRANSACTION_HANDLE, (TRANSACTION_HANDLE)0x4455);

    MOCK_STATIC_METHOD_3(, DEVICE_RESULT, Device_EndTransaction, TRANSACTION_HANDLE, transactionHandle, unsigned char**, destination, size_t*, destinationSize)
    {
    }
    MOCK_METHOD_END(DEVICE_RESULT, DEVICE_OK);

    MOCK_STATIC_METHOD_1(, DEVICE_RESULT, Device_CancelTransaction, TRANSACTION_HANDLE, transactionHandle)
    {
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
    MOCK_METHOD_END(SCHEMA_STRUCT_TYPE_HANDLE, TEST_STRUCT_TYPE);
    MOCK_STATIC_METHOD_3(, SCHEMA_RESULT, Schema_AddStructTypeProperty, SCHEMA_STRUCT_TYPE_HANDLE, structTypeHandle, const char*, propertyName, const char*, propertyType)
    MOCK_METHOD_END(SCHEMA_RESULT, SCHEMA_OK);
    MOCK_STATIC_METHOD_3(, SCHEMA_RESULT, Schema_AddModelProperty, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle, const char*, propertyName, const char*, propertyType)
    MOCK_METHOD_END(SCHEMA_RESULT, SCHEMA_OK);
    MOCK_STATIC_METHOD_2(, SCHEMA_ACTION_HANDLE, Schema_CreateModelAction, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle, const char*, actionName)
    MOCK_METHOD_END(SCHEMA_ACTION_HANDLE, TEST_ACTION_HANDLE);
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
    MOCK_STATIC_METHOD_1(, SCHEMA_RESULT, Schema_AddDeviceRef, SCHEMA_MODEL_TYPE_HANDLE, modelHandle)
    MOCK_METHOD_END(SCHEMA_RESULT, SCHEMA_OK);
    MOCK_STATIC_METHOD_1(, SCHEMA_HANDLE, Schema_GetSchemaForModelType, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle);
    MOCK_METHOD_END(SCHEMA_HANDLE, (SCHEMA_HANDLE)NULL);
    MOCK_STATIC_METHOD_1(, SCHEMA_RESULT, Schema_DestroyIfUnused, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle);
    MOCK_METHOD_END(SCHEMA_RESULT, SCHEMA_OK);
};

DECLARE_GLOBAL_MOCK_METHOD_2(CCodeFirstMocks, , AGENT_DATA_TYPES_RESULT, Create_EDM_BOOLEAN_from_int, AGENT_DATA_TYPE*, agentData, int, v);
DECLARE_GLOBAL_MOCK_METHOD_2(CCodeFirstMocks, , AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_UINT8, AGENT_DATA_TYPE*, agentData, uint8_t, v);
DECLARE_GLOBAL_MOCK_METHOD_2(CCodeFirstMocks, , AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_DOUBLE, AGENT_DATA_TYPE*, agentData, double, v);
DECLARE_GLOBAL_MOCK_METHOD_2(CCodeFirstMocks, , AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_SINT16, AGENT_DATA_TYPE*, agentData, int16_t, v);
DECLARE_GLOBAL_MOCK_METHOD_2(CCodeFirstMocks, , AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_SINT32, AGENT_DATA_TYPE*, agentData, int32_t, v);
DECLARE_GLOBAL_MOCK_METHOD_2(CCodeFirstMocks, , AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_SINT64, AGENT_DATA_TYPE*, agentData, int64_t, v);
DECLARE_GLOBAL_MOCK_METHOD_2(CCodeFirstMocks, , AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_SINT8, AGENT_DATA_TYPE*, agentData, int8_t, v);
DECLARE_GLOBAL_MOCK_METHOD_2(CCodeFirstMocks, , AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_FLOAT, AGENT_DATA_TYPE*, agentData, float, v);
DECLARE_GLOBAL_MOCK_METHOD_2(CCodeFirstMocks, , AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_charz, AGENT_DATA_TYPE*, agentData, const char*, v);
DECLARE_GLOBAL_MOCK_METHOD_2(CCodeFirstMocks, , AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_charz_no_quotes, AGENT_DATA_TYPE*, agentData, const char*, v);
DECLARE_GLOBAL_MOCK_METHOD_1(CCodeFirstMocks, , void, Destroy_AGENT_DATA_TYPE, AGENT_DATA_TYPE*, agentData);
DECLARE_GLOBAL_MOCK_METHOD_5(CCodeFirstMocks, , AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_Members, AGENT_DATA_TYPE*, agentData, const char*, typeName, size_t, nMembers, const char* const *, memberNames, const AGENT_DATA_TYPE*, memberValues);
DECLARE_GLOBAL_MOCK_METHOD_2(CCodeFirstMocks, , AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_EDM_DATE_TIME_OFFSET, AGENT_DATA_TYPE*, agentData, EDM_DATE_TIME_OFFSET, v);
DECLARE_GLOBAL_MOCK_METHOD_2(CCodeFirstMocks, , AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_EDM_GUID, AGENT_DATA_TYPE*, agentData, EDM_GUID, v);
DECLARE_GLOBAL_MOCK_METHOD_2(CCodeFirstMocks, , AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_EDM_BINARY, AGENT_DATA_TYPE*, agentData, EDM_BINARY, v);
DECLARE_GLOBAL_MOCK_METHOD_5(CCodeFirstMocks, , DEVICE_RESULT, Device_Create, SCHEMA_MODEL_TYPE_HANDLE, modelHandle, pPfDeviceActionCallback, deviceActionCallback, void*, callbackUserContext, bool, includePropertyPath, DEVICE_HANDLE*, deviceHandle);
DECLARE_GLOBAL_MOCK_METHOD_1(CCodeFirstMocks, , void, Device_Destroy, DEVICE_HANDLE, deviceHandle);
DECLARE_GLOBAL_MOCK_METHOD_3(CCodeFirstMocks, , DEVICE_RESULT, Device_PublishTransacted, TRANSACTION_HANDLE, transactionHandle, const char*, propertyName, const AGENT_DATA_TYPE*, data);
DECLARE_GLOBAL_MOCK_METHOD_1(CCodeFirstMocks, , TRANSACTION_HANDLE, Device_StartTransaction, SCHEMA_MODEL_TYPE_HANDLE, modelHandle);
DECLARE_GLOBAL_MOCK_METHOD_3(CCodeFirstMocks, , DEVICE_RESULT, Device_EndTransaction, TRANSACTION_HANDLE, transactionHandle, unsigned char**, destination, size_t*, destinationSize);
DECLARE_GLOBAL_MOCK_METHOD_1(CCodeFirstMocks, , DEVICE_RESULT, Device_CancelTransaction, TRANSACTION_HANDLE, transactionHandle);
DECLARE_GLOBAL_MOCK_METHOD_2(CCodeFirstMocks, , EXECUTE_COMMAND_RESULT, Device_ExecuteCommand, DEVICE_HANDLE, deviceHandle, const char*, command);
DECLARE_GLOBAL_MOCK_METHOD_1(CCodeFirstMocks, , DEVICE_RESULT, Device_SendAll, DEVICE_HANDLE, deviceHandle);
DECLARE_GLOBAL_MOCK_METHOD_1(CCodeFirstMocks, , DEVICE_RESULT, Device_DrainCommands, DEVICE_HANDLE, deviceHandle);

DECLARE_GLOBAL_MOCK_METHOD_1(CCodeFirstMocks, , SCHEMA_HANDLE, Schema_GetSchemaByNamespace, const char*, schemaNamespace);
DECLARE_GLOBAL_MOCK_METHOD_1(CCodeFirstMocks, , SCHEMA_HANDLE, Schema_Create, const char*, schemaNamespace);
DECLARE_GLOBAL_MOCK_METHOD_2(CCodeFirstMocks, , SCHEMA_MODEL_TYPE_HANDLE, Schema_CreateModelType, SCHEMA_HANDLE, schemaHandle, const char*, modelName);
DECLARE_GLOBAL_MOCK_METHOD_2(CCodeFirstMocks, , SCHEMA_STRUCT_TYPE_HANDLE, Schema_CreateStructType, SCHEMA_HANDLE, schemaHandle, const char*, structTypeName);
DECLARE_GLOBAL_MOCK_METHOD_3(CCodeFirstMocks, , SCHEMA_RESULT, Schema_AddStructTypeProperty, SCHEMA_STRUCT_TYPE_HANDLE, structTypeHandle, const char*, propertyName, const char*, propertyType);
DECLARE_GLOBAL_MOCK_METHOD_3(CCodeFirstMocks, , SCHEMA_RESULT, Schema_AddModelProperty, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle, const char*, propertyName, const char*, propertyType);
DECLARE_GLOBAL_MOCK_METHOD_2(CCodeFirstMocks, , SCHEMA_ACTION_HANDLE, Schema_CreateModelAction, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle, const char*, actionName);
DECLARE_GLOBAL_MOCK_METHOD_3(CCodeFirstMocks, , SCHEMA_RESULT, Schema_AddModelActionArgument, SCHEMA_ACTION_HANDLE, actionHandle, const char*, argumentName, const char*, argumentType);
DECLARE_GLOBAL_MOCK_METHOD_1(CCodeFirstMocks, , void, Schema_Destroy, SCHEMA_HANDLE, schemaHandle);
DECLARE_GLOBAL_MOCK_METHOD_1(CCodeFirstMocks, , const char*, Schema_GetModelName, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle);
DECLARE_GLOBAL_MOCK_METHOD_2(CCodeFirstMocks, , SCHEMA_MODEL_TYPE_HANDLE, Schema_GetModelByName, SCHEMA_HANDLE, schemaHandle, const char*, modelName);
DECLARE_GLOBAL_MOCK_METHOD_3(CCodeFirstMocks, , SCHEMA_RESULT, Schema_AddModelModel, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle, const char*, propertyName, SCHEMA_MODEL_TYPE_HANDLE, modelType);
DECLARE_GLOBAL_MOCK_METHOD_1(CCodeFirstMocks, , SCHEMA_RESULT, Schema_AddDeviceRef, SCHEMA_MODEL_TYPE_HANDLE, modelHandle);
DECLARE_GLOBAL_MOCK_METHOD_1(CCodeFirstMocks, , SCHEMA_RESULT, Schema_ReleaseDeviceRef, SCHEMA_MODEL_TYPE_HANDLE, modelHandle);
DECLARE_GLOBAL_MOCK_METHOD_1(CCodeFirstMocks, , SCHEMA_HANDLE, Schema_GetSchemaForModelType, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle);
DECLARE_GLOBAL_MOCK_METHOD_1(CCodeFirstMocks, , SCHEMA_RESULT, Schema_DestroyIfUnused, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle);


extern "C" DEVICE_HANDLE serializer_getdevicehandle(void) { return TEST_DEVICE_HANDLE; }

DEFINE_MICROMOCK_ENUM_TO_STRING(CODEFIRST_RESULT, CODEFIRST_ENUM_VALUES)

static STRING_HANDLE global_bufferTemp;
static MICROMOCK_GLOBAL_SEMAPHORE_HANDLE g_dllByDll;

BEGIN_TEST_SUITE(CodeFirst_ut_Two_Providers_With_Structs)

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
    CCodeFirstMocks mocks;
    mocks.SetPerformAutomaticCallComparison(AUTOMATIC_CALL_COMPARISON_OFF);
    nCreate_AGENT_DATA_TYPE_from_DOUBLE_agentData = 0;
    nDestroy_AGENT_DATA_TYPE_agentData = 0;

    goToLocation_wasCalled = false;
    goToLocation_XMarksTheSpot_long = -1.0;
    goToLocation_XMarksTheSpot_lat = -1.0;

    moveCarTo_wasCalled = false;
    moveCarTo_speed = -1;
    moveCarTo_destination_Alt = -1;
    moveCarTo_destination_whereIsMyCar_Lat = -1.0;
    moveCarTo_destination_whereIsMyCar_Long = -1.0;
    moveCarTo_reverse = false;

    (void)CodeFirst_Init(NULL);
    global_bufferTemp = STRING_new();
    if (!MicroMockAcquireMutex(g_testByTest))
    {
        ASSERT_FAIL("our mutex is ABANDONED. Failure in test framework");
    }
}

TEST_FUNCTION_CLEANUP(TestMethodCleanup)
{
    CCodeFirstMocks mocks;
    mocks.SetPerformAutomaticCallComparison(AUTOMATIC_CALL_COMPARISON_OFF);
    STRING_delete(global_bufferTemp);
    CodeFirst_Deinit();
    if (!MicroMockReleaseMutex(g_testByTest))
    {
        ASSERT_FAIL("failure in test framework at ReleaseMutex");
    }
}

/*Tests_SRS_CODEFIRST_99_002:[ CodeFirst_Init shall initialize the CodeFirst module. If initialization is successful, it shall return CODEFIRST_OK.]*/
TEST_FUNCTION(CodeFirst_Init_succeds)
{
    CNiceCallComparer<CCodeFirstMocks> mocks;

    ///arrange
    CODEFIRST_RESULT result;
    CodeFirst_Deinit();

    ///act
    result = CodeFirst_Init(NULL);

    ///assert
    ASSERT_ARE_EQUAL(CODEFIRST_RESULT, CODEFIRST_OK, result);
}

/*Tests_SRS_SERIALIZER_99_039:[ WITH_ACTION shall declare an action of the current data provider called as the first macro parameter (name) and having the first parameter called parameter1Name of type parameter1Type, the second parameter named parameter2Name having the type parameter2Type and so on. ]*/
/*Tests_SRS_SERIALIZER_99_042:[ The parameter types are either predefined parameter types (specs SRS_SERIALIZER_99_004-SRS_SERIALIZER_99_014) or a type introduced by DECLARE_STRUCT.]*/
TEST_FUNCTION(InvokeAction_goToLocation_succeeds)
{
    ///arrange
    CCodeFirstMocks mocks;
    void* device = CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &DummyDataProvider_WithStructs_allReflected, sizeof(TruckType), false);
    mocks.ResetAllCalls();
    AGENT_DATA_TYPE someLocation;

    AGENT_DATA_TYPE Lat;
    Lat.type = EDM_DOUBLE_TYPE;
    Lat.value.edmDouble.value = 3.0;

    AGENT_DATA_TYPE Long;
    Long.type = EDM_DOUBLE_TYPE;
    Long.value.edmDouble.value = 4.0;

    COMPLEX_TYPE_FIELD_TYPE fieldsOfSomeLocation[2] = { { "Lat", &Lat }, { "Long", &Long } };

    someLocation.type = EDM_COMPLEX_TYPE_TYPE;
    someLocation.value.edmComplexType.nMembers = 2;
    someLocation.value.edmComplexType.fields = fieldsOfSomeLocation;

    STRICT_EXPECTED_CALL(mocks, Schema_GetModelName(TEST_MODEL_HANDLE)).SetReturn("TruckType");

    ///act
   auto result = CodeFirst_InvokeAction(TEST_DEVICE_HANDLE, g_InvokeActionCallbackArgument, "", "goToLocation", 1, &someLocation);

    ///assert
   ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_SUCCESS, result);
    ASSERT_ARE_EQUAL(bool, true, goToLocation_wasCalled);
    ASSERT_ARE_EQUAL(double, 4.0, goToLocation_XMarksTheSpot_long);
    ASSERT_ARE_EQUAL(double, 3.0, goToLocation_XMarksTheSpot_lat);
    mocks.AssertActualAndExpectedCalls();

    // cleanup
    CodeFirst_DestroyDevice(device);
}

/*Tests_SRS_CODEFIRST_02_013: [The wrapper's return value shall be returned.] */
TEST_FUNCTION(InvokeAction_alwaysRejected_return_EXECUTE_COMMAND_FAILED)
{
    ///arrange
    CCodeFirstMocks mocks;
    void* device = CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &DummyDataProvider_WithStructs_allReflected, sizeof(TruckType), false);
    mocks.ResetAllCalls();
   
    STRICT_EXPECTED_CALL(mocks, Schema_GetModelName(TEST_MODEL_HANDLE)).SetReturn("TruckType");

    ///act
    auto result = CodeFirst_InvokeAction(TEST_DEVICE_HANDLE, g_InvokeActionCallbackArgument, "", "alwaysReject", 0, NULL);

    ///assert
    ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_FAILED, result);
    mocks.AssertActualAndExpectedCalls();

    // cleanup
    CodeFirst_DestroyDevice(device);
}

/*Tests_SRS_CODEFIRST_02_013: [The wrapper's return value shall be returned.] */
TEST_FUNCTION(InvokeAction_alwaysAbandon_return_EXECUTE_COMMAND_ERROR)
{
    ///arrange
    CCodeFirstMocks mocks;
    void* device = CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &DummyDataProvider_WithStructs_allReflected, sizeof(TruckType), false);
    mocks.ResetAllCalls();

    STRICT_EXPECTED_CALL(mocks, Schema_GetModelName(TEST_MODEL_HANDLE)).SetReturn("TruckType");

    ///act
    auto result = CodeFirst_InvokeAction(TEST_DEVICE_HANDLE, g_InvokeActionCallbackArgument, "", "alwaysAbandon", 0, NULL);

    ///assert
    ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
    mocks.AssertActualAndExpectedCalls();

    // cleanup
    CodeFirst_DestroyDevice(device);
}

/*the below tests are derived from the one above by trying to feed wrong data in the CodeFirst_InvokeAction*/

/*Tests_SRS_SERIALIZER_99_039:[ WITH_ACTION shall declare an action of the current data provider called as the first macro parameter (name) and having the first parameter called parameter1Name of type parameter1Type, the second parameter named parameter2Name having the type parameter2Type and so on. ]*/
TEST_FUNCTION(InvokeAction_goToLocation_with_a_struct_with_3_fields_fails)
{
    ///arrange
    CCodeFirstMocks mocks;
    void* device = CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &DummyDataProvider_WithStructs_allReflected, sizeof(TruckType), false);
    mocks.ResetAllCalls();
    AGENT_DATA_TYPE someLocation;

    AGENT_DATA_TYPE Lat;
    Lat.type = EDM_DOUBLE_TYPE;
    Lat.value.edmDouble.value = 3.0;

    AGENT_DATA_TYPE Long;
    Long.type = EDM_DOUBLE_TYPE;
    Long.value.edmDouble.value = 4.0;

    AGENT_DATA_TYPE Extra;
    Extra.type = EDM_DOUBLE_TYPE;
    Extra.value.edmDouble.value = 4.0;

    COMPLEX_TYPE_FIELD_TYPE fieldsOfSomeLocation[3] = { { "Lat", &Lat }, { "Long", &Long }, { "Extra", &Extra } };

    someLocation.type = EDM_COMPLEX_TYPE_TYPE;
    someLocation.value.edmComplexType.nMembers = 3;
    someLocation.value.edmComplexType.fields = fieldsOfSomeLocation;

    STRICT_EXPECTED_CALL(mocks, Schema_GetModelName(TEST_MODEL_HANDLE)).SetReturn("TruckType");

    ///act
   auto result = CodeFirst_InvokeAction(TEST_DEVICE_HANDLE, g_InvokeActionCallbackArgument, "", "goToLocation", 1, &someLocation);

    ///assert
   ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
    ASSERT_ARE_EQUAL(bool, false, goToLocation_wasCalled);
    mocks.AssertActualAndExpectedCalls();

    // cleanup
    CodeFirst_DestroyDevice(device);
}

/*Tests_SRS_SERIALIZER_99_039:[ WITH_ACTION shall declare an action of the current data provider called as the first macro parameter (name) and having the first parameter called parameter1Name of type parameter1Type, the second parameter named parameter2Name having the type parameter2Type and so on. ]*/
TEST_FUNCTION(InvokeAction_goToLocation_with_a_struct_with_1_field_fails)
{
    ///arrange
    CCodeFirstMocks mocks;
    void* device = CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &DummyDataProvider_WithStructs_allReflected, sizeof(TruckType), false);
    mocks.ResetAllCalls();
    AGENT_DATA_TYPE someLocation;

    AGENT_DATA_TYPE Lat;
    Lat.type = EDM_DOUBLE_TYPE;
    Lat.value.edmDouble.value = 3.0;

    COMPLEX_TYPE_FIELD_TYPE fieldsOfSomeLocation[1] = { { "Lat", &Lat } };

    someLocation.type = EDM_COMPLEX_TYPE_TYPE;
    someLocation.value.edmComplexType.nMembers = 1;
    someLocation.value.edmComplexType.fields = fieldsOfSomeLocation;

    STRICT_EXPECTED_CALL(mocks, Schema_GetModelName(TEST_MODEL_HANDLE)).SetReturn("TruckType");

    ///act
   auto result = CodeFirst_InvokeAction(TEST_DEVICE_HANDLE, g_InvokeActionCallbackArgument, "", "goToLocation", 1, &someLocation);

    ///assert
   ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
    ASSERT_ARE_EQUAL(bool, false, goToLocation_wasCalled);
    mocks.AssertActualAndExpectedCalls();

    // cleanup
    CodeFirst_DestroyDevice(device);
}

/*Tests_SRS_SERIALIZER_99_039:[ WITH_ACTION shall declare an action of the current data provider called as the first macro parameter (name) and having the first parameter called parameter1Name of type parameter1Type, the second parameter named parameter2Name having the type parameter2Type and so on. ]*/
TEST_FUNCTION(InvokeAction_goToLocation_with_a_non_struct_fails)
{
    ///arrange
    CCodeFirstMocks mocks;
    void* device = CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &DummyDataProvider_WithStructs_allReflected, sizeof(TruckType), false);
    mocks.ResetAllCalls();

    AGENT_DATA_TYPE Lat;
    Lat.type = EDM_DOUBLE_TYPE;
    Lat.value.edmDouble.value = 3.0;

    STRICT_EXPECTED_CALL(mocks, Schema_GetModelName(TEST_MODEL_HANDLE)).SetReturn("TruckType");

    ///act
   auto result = CodeFirst_InvokeAction(TEST_DEVICE_HANDLE, g_InvokeActionCallbackArgument, "", "goToLocation", 1, &Lat);

    ///assert
   ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
    ASSERT_ARE_EQUAL(bool, false, goToLocation_wasCalled);
    mocks.AssertActualAndExpectedCalls();

    // cleanup
    CodeFirst_DestroyDevice(device);
}

/*Tests_SRS_SERIALIZER_99_039:[ WITH_ACTION shall declare an action of the current data provider called as the first macro parameter (name) and having the first parameter called parameter1Name of type parameter1Type, the second parameter named parameter2Name having the type parameter2Type and so on. ]*/
TEST_FUNCTION(InvokeAction_goToLocation_with_a_struct_and_a_double_fails)
{
    ///arrange
    CCodeFirstMocks mocks;
    void* device = CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &DummyDataProvider_WithStructs_allReflected, sizeof(TruckType), false);
    mocks.ResetAllCalls();
    AGENT_DATA_TYPE someLocationAndDouble[2];

    AGENT_DATA_TYPE Lat;
    Lat.type = EDM_DOUBLE_TYPE;
    Lat.value.edmDouble.value = 3.0;

    AGENT_DATA_TYPE Long;
    Long.type = EDM_DOUBLE_TYPE;
    Long.value.edmDouble.value = 4.0;

    COMPLEX_TYPE_FIELD_TYPE someLocationAndDouble_1[2] = { { "Lat", &Lat }, { "Long", &Long } };

    someLocationAndDouble[0].type = EDM_COMPLEX_TYPE_TYPE;
    someLocationAndDouble[0].value.edmComplexType.nMembers = 2;
    someLocationAndDouble[0].value.edmComplexType.fields = someLocationAndDouble_1;

    someLocationAndDouble[1].type = EDM_DOUBLE_TYPE;
    someLocationAndDouble[1].value.edmDouble.value = EDM_DOUBLE_TYPE;

    STRICT_EXPECTED_CALL(mocks, Schema_GetModelName(TEST_MODEL_HANDLE)).SetReturn("TruckType");

    ///act
   auto result = CodeFirst_InvokeAction(TEST_DEVICE_HANDLE, g_InvokeActionCallbackArgument, "", "goToLocation", 2, someLocationAndDouble);

    ///assert
   ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
    ASSERT_ARE_EQUAL(bool, false, goToLocation_wasCalled);
    mocks.AssertActualAndExpectedCalls();

    // cleanup
    CodeFirst_DestroyDevice(device);
}

/*Tests_SRS_SERIALIZER_99_039:[ WITH_ACTION shall declare an action of the current data provider called as the first macro parameter (name) and having the first parameter called parameter1Name of type parameter1Type, the second parameter named parameter2Name having the type parameter2Type and so on. ]*/
TEST_FUNCTION(InvokeAction_goToLocation_with_a_double_and_a_struct_fails)
{
    ///arrange
    CCodeFirstMocks mocks;
    void* device = CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &DummyDataProvider_WithStructs_allReflected, sizeof(TruckType), false);
    mocks.ResetAllCalls();
    AGENT_DATA_TYPE someLocationAndDouble[2];

    AGENT_DATA_TYPE Lat;
    Lat.type = EDM_DOUBLE_TYPE;
    Lat.value.edmDouble.value = 3.0;

    AGENT_DATA_TYPE Long;
    Long.type = EDM_DOUBLE_TYPE;
    Long.value.edmDouble.value = 4.0;

    COMPLEX_TYPE_FIELD_TYPE someLocationAndDouble_1[2] = { { "Lat", &Lat }, { "Long", &Long } };

    someLocationAndDouble[1].type = EDM_COMPLEX_TYPE_TYPE;
    someLocationAndDouble[1].value.edmComplexType.nMembers = 2;
    someLocationAndDouble[1].value.edmComplexType.fields = someLocationAndDouble_1;

    someLocationAndDouble[0].type = EDM_DOUBLE_TYPE;
    someLocationAndDouble[0].value.edmDouble.value = EDM_DOUBLE_TYPE;

    STRICT_EXPECTED_CALL(mocks, Schema_GetModelName(TEST_MODEL_HANDLE)).SetReturn("TruckType");

    ///act
   auto result = CodeFirst_InvokeAction(TEST_DEVICE_HANDLE, g_InvokeActionCallbackArgument, "", "goToLocation", 2, someLocationAndDouble);

    ///assert
   ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
    ASSERT_ARE_EQUAL(bool, false, goToLocation_wasCalled);
    mocks.AssertActualAndExpectedCalls();

    // cleanup
    CodeFirst_DestroyDevice(device);
}

/*Tests_SRS_SERIALIZER_99_039:[ WITH_ACTION shall declare an action of the current data provider called as the first macro parameter (name) and having the first parameter called parameter1Name of type parameter1Type, the second parameter named parameter2Name having the type parameter2Type and so on. ]*/
TEST_FUNCTION(InvokeAction_goToLocation_with_a_struct_with_first_field_of_wrong_type_fails)
{
    ///arrange
    CCodeFirstMocks mocks;
    void* device = CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &DummyDataProvider_WithStructs_allReflected, sizeof(TruckType), false);
    mocks.ResetAllCalls();
    AGENT_DATA_TYPE someLocation;

    AGENT_DATA_TYPE Lat;
    Lat.type = EDM_SINGLE_TYPE;
    Lat.value.edmDouble.value = 3.0f;

    AGENT_DATA_TYPE Long;
    Long.type = EDM_DOUBLE_TYPE;
    Long.value.edmDouble.value = 4.0;

    COMPLEX_TYPE_FIELD_TYPE fieldsOfSomeLocation[2] = { { "Lat", &Lat }, { "Long", &Long } };

    someLocation.type = EDM_COMPLEX_TYPE_TYPE;
    someLocation.value.edmComplexType.nMembers = 2;
    someLocation.value.edmComplexType.fields = fieldsOfSomeLocation;

    STRICT_EXPECTED_CALL(mocks, Schema_GetModelName(TEST_MODEL_HANDLE)).SetReturn("TruckType");

    ///act
   auto result = CodeFirst_InvokeAction(TEST_DEVICE_HANDLE, g_InvokeActionCallbackArgument, "", "goToLocation", 1, &someLocation);

    ///assert
   ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
    ASSERT_ARE_EQUAL(bool, false, goToLocation_wasCalled);
    mocks.AssertActualAndExpectedCalls();

    // cleanup
    CodeFirst_DestroyDevice(device);
}

/*Tests_SRS_SERIALIZER_99_039:[ WITH_ACTION shall declare an action of the current data provider called as the first macro parameter (name) and having the first parameter called parameter1Name of type parameter1Type, the second parameter named parameter2Name having the type parameter2Type and so on. ]*/
TEST_FUNCTION(InvokeAction_goToLocation_with_a_struct_with_second_field_of_wrong_type_fails)
{
    ///arrange
    CCodeFirstMocks mocks;
    void* device = CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &DummyDataProvider_WithStructs_allReflected, sizeof(TruckType), false);
    mocks.ResetAllCalls();
    AGENT_DATA_TYPE someLocation;

    AGENT_DATA_TYPE Lat;
    Lat.type = EDM_DOUBLE_TYPE;
    Lat.value.edmDouble.value = 3.0;

    AGENT_DATA_TYPE Long;
    Long.type = EDM_SINGLE_TYPE;
    Long.value.edmDouble.value = 4.0f;

    COMPLEX_TYPE_FIELD_TYPE fieldsOfSomeLocation[2] = { { "Lat", &Lat }, { "Long", &Long } };

    someLocation.type = EDM_COMPLEX_TYPE_TYPE;
    someLocation.value.edmComplexType.nMembers = 2;
    someLocation.value.edmComplexType.fields = fieldsOfSomeLocation;

    STRICT_EXPECTED_CALL(mocks, Schema_GetModelName(TEST_MODEL_HANDLE)).SetReturn("TruckType");

    ///act
   auto result = CodeFirst_InvokeAction(TEST_DEVICE_HANDLE, g_InvokeActionCallbackArgument, "", "goToLocation", 1, &someLocation);

    ///assert
   ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
    ASSERT_ARE_EQUAL(bool, false, goToLocation_wasCalled);
    mocks.AssertActualAndExpectedCalls();

    // cleanup
    CodeFirst_DestroyDevice(device);
}

/*Tests_SRS_SERIALIZER_99_039:[ WITH_ACTION shall declare an action of the current data provider called as the first macro parameter (name) and having the first parameter called parameter1Name of type parameter1Type, the second parameter named parameter2Name having the type parameter2Type and so on. ]*/
/*Tests_SRS_SERIALIZER_99_042:[ The parameter types are either predefined parameter types (specs SRS_SERIALIZER_99_004-SRS_SERIALIZER_99_014) or a type introduced by DECLARE_STRUCT.]*/
TEST_FUNCTION(InvokeAction_moveCarTo_succeeds)
{
    ///arrange
    CCodeFirstMocks mocks;
    void* device = CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &DummyDataProvider_WithStructs_allReflected, sizeof(TruckType), false);
    mocks.ResetAllCalls();
    AGENT_DATA_TYPE parameters[3];

    parameters[0].type = EDM_INT32_TYPE;
    parameters[0].value.edmInt32.value = 2;

    AGENT_DATA_TYPE CarLocation_Alt;
    CarLocation_Alt.type = EDM_INT32_TYPE;
    CarLocation_Alt.value.edmInt32.value = 3;

    AGENT_DATA_TYPE CarLocation_whereIsMyCar_Lat;
    CarLocation_whereIsMyCar_Lat.type = EDM_DOUBLE_TYPE;
    CarLocation_whereIsMyCar_Lat.value.edmDouble.value = 4.0;

    AGENT_DATA_TYPE CarLocation_whereIsMyCar_Long;
    CarLocation_whereIsMyCar_Long.type = EDM_DOUBLE_TYPE;
    CarLocation_whereIsMyCar_Long.value.edmDouble.value = 5.0;

    COMPLEX_TYPE_FIELD_TYPE CarLocation_whereIsMyCarFields[2];
    CarLocation_whereIsMyCarFields[0].fieldName = "Lat";
    CarLocation_whereIsMyCarFields[0].value = &CarLocation_whereIsMyCar_Lat;
    CarLocation_whereIsMyCarFields[1].fieldName = "Long";
    CarLocation_whereIsMyCarFields[1].value = &CarLocation_whereIsMyCar_Long;

    AGENT_DATA_TYPE CarLocation_whereIsMyCar;
    CarLocation_whereIsMyCar.type = EDM_COMPLEX_TYPE_TYPE;
    CarLocation_whereIsMyCar.value.edmComplexType.nMembers = 2;
    CarLocation_whereIsMyCar.value.edmComplexType.fields = CarLocation_whereIsMyCarFields;

    COMPLEX_TYPE_FIELD_TYPE carLocationFields[2];
    carLocationFields[0].fieldName = "Alt";
    carLocationFields[0].value = &CarLocation_Alt;
    carLocationFields[1].fieldName = "whereIsMyCar";
    carLocationFields[1].value = &CarLocation_whereIsMyCar;

    parameters[1].type = EDM_COMPLEX_TYPE_TYPE;
    parameters[1].value.edmComplexType.nMembers = 2;
    parameters[1].value.edmComplexType.fields = carLocationFields;

    parameters[2].type = EDM_BOOLEAN_TYPE;
    parameters[2].value.edmBoolean.value = EDM_TRUE;

    STRICT_EXPECTED_CALL(mocks, Schema_GetModelName(TEST_MODEL_HANDLE)).SetReturn("TruckType");

    ///act
   auto result = CodeFirst_InvokeAction(TEST_DEVICE_HANDLE, g_InvokeActionCallbackArgument, "", "moveCarTo", 3, parameters);

    ///assert
   ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_SUCCESS, result);
    ASSERT_ARE_EQUAL(bool, true, moveCarTo_wasCalled);
    ASSERT_ARE_EQUAL(int, 2, moveCarTo_speed);
    ASSERT_ARE_EQUAL(int, 3, moveCarTo_destination_Alt);
    ASSERT_ARE_EQUAL(double, 4.0, moveCarTo_destination_whereIsMyCar_Lat);
    ASSERT_ARE_EQUAL(double, 5.0, moveCarTo_destination_whereIsMyCar_Long);
    ASSERT_ARE_EQUAL(bool, true, moveCarTo_reverse);
    mocks.AssertActualAndExpectedCalls();

    // cleanup
    CodeFirst_DestroyDevice(device);
}

/*the below tests are simple to make it fail*/

/*Tests_SRS_SERIALIZER_99_039:[ WITH_ACTION shall declare an action of the current data provider called as the first macro parameter (name) and having the first parameter called parameter1Name of type parameter1Type, the second parameter named parameter2Name having the type parameter2Type and so on. ]*/
TEST_FUNCTION(InvokeAction_moveCarTo_with_wrong_number_of_parameters_fails_1)
{
    ///arrange
    CCodeFirstMocks mocks;
    void* device = CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &DummyDataProvider_WithStructs_allReflected, sizeof(TruckType), false);
    mocks.ResetAllCalls();
    AGENT_DATA_TYPE parameters[3];

    parameters[0].type = EDM_INT32_TYPE;
    parameters[0].value.edmInt32.value = 2;

    AGENT_DATA_TYPE CarLocation_Alt;
    CarLocation_Alt.type = EDM_INT32_TYPE;
    CarLocation_Alt.value.edmInt32.value = 3;

    AGENT_DATA_TYPE CarLocation_whereIsMyCar_Lat;
    CarLocation_whereIsMyCar_Lat.type = EDM_DOUBLE_TYPE;
    CarLocation_whereIsMyCar_Lat.value.edmDouble.value = 4.0;

    AGENT_DATA_TYPE CarLocation_whereIsMyCar_Long;
    CarLocation_whereIsMyCar_Long.type = EDM_DOUBLE_TYPE;
    CarLocation_whereIsMyCar_Long.value.edmDouble.value = 5.0;

    COMPLEX_TYPE_FIELD_TYPE CarLocation_whereIsMyCarFields[2];
    CarLocation_whereIsMyCarFields[0].fieldName = "Lat";
    CarLocation_whereIsMyCarFields[0].value = &CarLocation_whereIsMyCar_Lat;
    CarLocation_whereIsMyCarFields[1].fieldName = "Long";
    CarLocation_whereIsMyCarFields[1].value = &CarLocation_whereIsMyCar_Long;

    AGENT_DATA_TYPE CarLocation_whereIsMyCar;
    CarLocation_whereIsMyCar.type = EDM_COMPLEX_TYPE_TYPE;
    CarLocation_whereIsMyCar.value.edmComplexType.nMembers = 2;
    CarLocation_whereIsMyCar.value.edmComplexType.fields = CarLocation_whereIsMyCarFields;

    COMPLEX_TYPE_FIELD_TYPE carLocationFields[2];
    carLocationFields[0].fieldName = "Alt";
    carLocationFields[0].value = &CarLocation_Alt;
    carLocationFields[1].fieldName = "whereIsMyCar";
    carLocationFields[1].value = &CarLocation_whereIsMyCar;

    parameters[1].type = EDM_COMPLEX_TYPE_TYPE;
    parameters[1].value.edmComplexType.nMembers = 2;
    parameters[1].value.edmComplexType.fields = carLocationFields;

    parameters[2].type = EDM_BOOLEAN_TYPE;
    parameters[2].value.edmBoolean.value = EDM_TRUE;

    STRICT_EXPECTED_CALL(mocks, Schema_GetModelName(TEST_MODEL_HANDLE)).SetReturn("TruckType");

    ///act
   auto result = CodeFirst_InvokeAction(TEST_DEVICE_HANDLE, g_InvokeActionCallbackArgument, "", "moveCarTo", 2, parameters);

    ///assert
   ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
    ASSERT_ARE_EQUAL(bool, false, moveCarTo_wasCalled);
    mocks.AssertActualAndExpectedCalls();

    // cleanup
    CodeFirst_DestroyDevice(device);
}

/*Tests_SRS_SERIALIZER_99_039:[ WITH_ACTION shall declare an action of the current data provider called as the first macro parameter (name) and having the first parameter called parameter1Name of type parameter1Type, the second parameter named parameter2Name having the type parameter2Type and so on. ]*/
TEST_FUNCTION(InvokeAction_moveCarTo_with_wrong_number_of_parameters_fails_2)
{
    ///arrange
    CCodeFirstMocks mocks;
    void* device = CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &DummyDataProvider_WithStructs_allReflected, sizeof(TruckType), false);
    mocks.ResetAllCalls();
    AGENT_DATA_TYPE parameters[3];

    parameters[0].type = EDM_INT32_TYPE;
    parameters[0].value.edmInt32.value = 2;

    AGENT_DATA_TYPE CarLocation_Alt;
    CarLocation_Alt.type = EDM_INT32_TYPE;
    CarLocation_Alt.value.edmInt32.value = 3;

    AGENT_DATA_TYPE CarLocation_whereIsMyCar_Lat;
    CarLocation_whereIsMyCar_Lat.type = EDM_DOUBLE_TYPE;
    CarLocation_whereIsMyCar_Lat.value.edmDouble.value = 4.0;

    AGENT_DATA_TYPE CarLocation_whereIsMyCar_Long;
    CarLocation_whereIsMyCar_Long.type = EDM_DOUBLE_TYPE;
    CarLocation_whereIsMyCar_Long.value.edmDouble.value = 5.0;

    COMPLEX_TYPE_FIELD_TYPE CarLocation_whereIsMyCarFields[2];
    CarLocation_whereIsMyCarFields[0].fieldName = "Lat";
    CarLocation_whereIsMyCarFields[0].value = &CarLocation_whereIsMyCar_Lat;
    CarLocation_whereIsMyCarFields[1].fieldName = "Long";
    CarLocation_whereIsMyCarFields[1].value = &CarLocation_whereIsMyCar_Long;

    AGENT_DATA_TYPE CarLocation_whereIsMyCar;
    CarLocation_whereIsMyCar.type = EDM_COMPLEX_TYPE_TYPE;
    CarLocation_whereIsMyCar.value.edmComplexType.nMembers = 2;
    CarLocation_whereIsMyCar.value.edmComplexType.fields = CarLocation_whereIsMyCarFields;

    COMPLEX_TYPE_FIELD_TYPE carLocationFields[2];
    carLocationFields[0].fieldName = "Alt";
    carLocationFields[0].value = &CarLocation_Alt;
    carLocationFields[1].fieldName = "whereIsMyCar";
    carLocationFields[1].value = &CarLocation_whereIsMyCar;

    parameters[1].type = EDM_COMPLEX_TYPE_TYPE;
    parameters[1].value.edmComplexType.nMembers = 2;
    parameters[1].value.edmComplexType.fields = carLocationFields;

    parameters[2].type = EDM_BOOLEAN_TYPE;
    parameters[2].value.edmBoolean.value = EDM_TRUE;

    STRICT_EXPECTED_CALL(mocks, Schema_GetModelName(TEST_MODEL_HANDLE)).SetReturn("TruckType");

    ///act
   auto result = CodeFirst_InvokeAction(TEST_DEVICE_HANDLE, g_InvokeActionCallbackArgument, "", "moveCarTo", 4, parameters);

    ///assert
   ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
    ASSERT_ARE_EQUAL(bool, false, moveCarTo_wasCalled);
    mocks.AssertActualAndExpectedCalls();

    // cleanup
    CodeFirst_DestroyDevice(device);
}

/*Tests_SRS_SERIALIZER_99_039:[ WITH_ACTION shall declare an action of the current data provider called as the first macro parameter (name) and having the first parameter called parameter1Name of type parameter1Type, the second parameter named parameter2Name having the type parameter2Type and so on. ]*/
TEST_FUNCTION(InvokeAction_moveCarTo_with_wrong_type_of_parameter_1_fails)
{
    ///arrange
    CCodeFirstMocks mocks;
    void* device = CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &DummyDataProvider_WithStructs_allReflected, sizeof(TruckType), false);
    mocks.ResetAllCalls();
    AGENT_DATA_TYPE parameters[3];

    parameters[0].type = EDM_INT64_TYPE;
    parameters[0].value.edmInt32.value = 2;

    AGENT_DATA_TYPE CarLocation_Alt;
    CarLocation_Alt.type = EDM_INT32_TYPE;
    CarLocation_Alt.value.edmInt32.value = 3;

    AGENT_DATA_TYPE CarLocation_whereIsMyCar_Lat;
    CarLocation_whereIsMyCar_Lat.type = EDM_DOUBLE_TYPE;
    CarLocation_whereIsMyCar_Lat.value.edmDouble.value = 4.0;

    AGENT_DATA_TYPE CarLocation_whereIsMyCar_Long;
    CarLocation_whereIsMyCar_Long.type = EDM_DOUBLE_TYPE;
    CarLocation_whereIsMyCar_Long.value.edmDouble.value = 5.0;

    COMPLEX_TYPE_FIELD_TYPE CarLocation_whereIsMyCarFields[2];
    CarLocation_whereIsMyCarFields[0].fieldName = "Lat";
    CarLocation_whereIsMyCarFields[0].value = &CarLocation_whereIsMyCar_Lat;
    CarLocation_whereIsMyCarFields[1].fieldName = "Long";
    CarLocation_whereIsMyCarFields[1].value = &CarLocation_whereIsMyCar_Long;

    AGENT_DATA_TYPE CarLocation_whereIsMyCar;
    CarLocation_whereIsMyCar.type = EDM_COMPLEX_TYPE_TYPE;
    CarLocation_whereIsMyCar.value.edmComplexType.nMembers = 2;
    CarLocation_whereIsMyCar.value.edmComplexType.fields = CarLocation_whereIsMyCarFields;

    COMPLEX_TYPE_FIELD_TYPE carLocationFields[2];
    carLocationFields[0].fieldName = "Alt";
    carLocationFields[0].value = &CarLocation_Alt;
    carLocationFields[1].fieldName = "whereIsMyCar";
    carLocationFields[1].value = &CarLocation_whereIsMyCar;

    parameters[1].type = EDM_COMPLEX_TYPE_TYPE;
    parameters[1].value.edmComplexType.nMembers = 2;
    parameters[1].value.edmComplexType.fields = carLocationFields;

    parameters[2].type = EDM_BOOLEAN_TYPE;
    parameters[2].value.edmBoolean.value = EDM_TRUE;

    STRICT_EXPECTED_CALL(mocks, Schema_GetModelName(TEST_MODEL_HANDLE)).SetReturn("TruckType");

    ///act
   auto result = CodeFirst_InvokeAction(TEST_DEVICE_HANDLE, g_InvokeActionCallbackArgument, "", "moveCarTo", 3, parameters);

    ///assert
   ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
    ASSERT_ARE_EQUAL(bool, false, moveCarTo_wasCalled);
    mocks.AssertActualAndExpectedCalls();

    // cleanup
    CodeFirst_DestroyDevice(device);
}

/*Tests_SRS_SERIALIZER_99_039:[ WITH_ACTION shall declare an action of the current data provider called as the first macro parameter (name) and having the first parameter called parameter1Name of type parameter1Type, the second parameter named parameter2Name having the type parameter2Type and so on. ]*/
TEST_FUNCTION(InvokeAction_moveCarTo_with_wrong_type_of_parameter_2_fails)
{
    ///arrange
    CCodeFirstMocks mocks;
    void* device = CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &DummyDataProvider_WithStructs_allReflected, sizeof(TruckType), false);
    mocks.ResetAllCalls();
    AGENT_DATA_TYPE parameters[3];

    parameters[0].type = EDM_INT32_TYPE;
    parameters[0].value.edmInt32.value = 2;

    AGENT_DATA_TYPE CarLocation_Alt;
    CarLocation_Alt.type = EDM_INT32_TYPE;
    CarLocation_Alt.value.edmInt32.value = 3;

    AGENT_DATA_TYPE CarLocation_whereIsMyCar_Lat;
    CarLocation_whereIsMyCar_Lat.type = EDM_DOUBLE_TYPE;
    CarLocation_whereIsMyCar_Lat.value.edmDouble.value = 4.0;

    AGENT_DATA_TYPE CarLocation_whereIsMyCar_Long;
    CarLocation_whereIsMyCar_Long.type = EDM_DOUBLE_TYPE;
    CarLocation_whereIsMyCar_Long.value.edmDouble.value = 5.0;

    COMPLEX_TYPE_FIELD_TYPE CarLocation_whereIsMyCarFields[2];
    CarLocation_whereIsMyCarFields[0].fieldName = "Lat";
    CarLocation_whereIsMyCarFields[0].value = &CarLocation_whereIsMyCar_Lat;
    CarLocation_whereIsMyCarFields[1].fieldName = "Long";
    CarLocation_whereIsMyCarFields[1].value = &CarLocation_whereIsMyCar_Long;

    AGENT_DATA_TYPE CarLocation_whereIsMyCar;
    CarLocation_whereIsMyCar.type = EDM_COMPLEX_TYPE_TYPE;
    CarLocation_whereIsMyCar.value.edmComplexType.nMembers = 2;
    CarLocation_whereIsMyCar.value.edmComplexType.fields = CarLocation_whereIsMyCarFields;

    COMPLEX_TYPE_FIELD_TYPE carLocationFields[2];
    carLocationFields[0].fieldName = "Alt";
    carLocationFields[0].value = &CarLocation_Alt;
    carLocationFields[1].fieldName = "whereIsMyCar";
    carLocationFields[1].value = &CarLocation_whereIsMyCar;

    parameters[1].type = EDM_INT64_TYPE;
    parameters[1].value.edmComplexType.nMembers = 2;
    parameters[1].value.edmComplexType.fields = carLocationFields;

    parameters[2].type = EDM_BOOLEAN_TYPE;
    parameters[2].value.edmBoolean.value = EDM_TRUE;

    STRICT_EXPECTED_CALL(mocks, Schema_GetModelName(TEST_MODEL_HANDLE)).SetReturn("TruckType");

    ///act
   auto result = CodeFirst_InvokeAction(TEST_DEVICE_HANDLE, g_InvokeActionCallbackArgument, "", "moveCarTo", 3, parameters);

    ///assert
   ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
    ASSERT_ARE_EQUAL(bool, false, moveCarTo_wasCalled);
    mocks.AssertActualAndExpectedCalls();

    // cleanup
    CodeFirst_DestroyDevice(device);
}

/*Tests_SRS_SERIALIZER_99_039:[ WITH_ACTION shall declare an action of the current data provider called as the first macro parameter (name) and having the first parameter called parameter1Name of type parameter1Type, the second parameter named parameter2Name having the type parameter2Type and so on. ]*/
TEST_FUNCTION(InvokeAction_moveCarTo_with_wrong_type_for_destination_Alt_fails)
{
    ///arrange
    CCodeFirstMocks mocks;
    void* device = CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &DummyDataProvider_WithStructs_allReflected, sizeof(TruckType), false);
    mocks.ResetAllCalls();
    AGENT_DATA_TYPE parameters[3];

    parameters[0].type = EDM_INT32_TYPE;
    parameters[0].value.edmInt32.value = 2;

    AGENT_DATA_TYPE CarLocation_Alt;
    CarLocation_Alt.type = EDM_INT64_TYPE;
    CarLocation_Alt.value.edmInt32.value = 3;

    AGENT_DATA_TYPE CarLocation_whereIsMyCar_Lat;
    CarLocation_whereIsMyCar_Lat.type = EDM_DOUBLE_TYPE;
    CarLocation_whereIsMyCar_Lat.value.edmDouble.value = 4.0;

    AGENT_DATA_TYPE CarLocation_whereIsMyCar_Long;
    CarLocation_whereIsMyCar_Long.type = EDM_DOUBLE_TYPE;
    CarLocation_whereIsMyCar_Long.value.edmDouble.value = 5.0;

    COMPLEX_TYPE_FIELD_TYPE CarLocation_whereIsMyCarFields[2];
    CarLocation_whereIsMyCarFields[0].fieldName = "Lat";
    CarLocation_whereIsMyCarFields[0].value = &CarLocation_whereIsMyCar_Lat;
    CarLocation_whereIsMyCarFields[1].fieldName = "Long";
    CarLocation_whereIsMyCarFields[1].value = &CarLocation_whereIsMyCar_Long;

    AGENT_DATA_TYPE CarLocation_whereIsMyCar;
    CarLocation_whereIsMyCar.type = EDM_COMPLEX_TYPE_TYPE;
    CarLocation_whereIsMyCar.value.edmComplexType.nMembers = 2;
    CarLocation_whereIsMyCar.value.edmComplexType.fields = CarLocation_whereIsMyCarFields;

    COMPLEX_TYPE_FIELD_TYPE carLocationFields[2];
    carLocationFields[0].fieldName = "Alt";
    carLocationFields[0].value = &CarLocation_Alt;
    carLocationFields[1].fieldName = "whereIsMyCar";
    carLocationFields[1].value = &CarLocation_whereIsMyCar;

    parameters[1].type = EDM_COMPLEX_TYPE_TYPE;
    parameters[1].value.edmComplexType.nMembers = 2;
    parameters[1].value.edmComplexType.fields = carLocationFields;

    parameters[2].type = EDM_BOOLEAN_TYPE;
    parameters[2].value.edmBoolean.value = EDM_TRUE;

    STRICT_EXPECTED_CALL(mocks, Schema_GetModelName(TEST_MODEL_HANDLE)).SetReturn("TruckType");

    ///act
   auto result = CodeFirst_InvokeAction(TEST_DEVICE_HANDLE, g_InvokeActionCallbackArgument, "", "moveCarTo", 3, parameters);

    ///assert
   ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
    ASSERT_ARE_EQUAL(bool, false, moveCarTo_wasCalled);
    mocks.AssertActualAndExpectedCalls();

    // cleanup
    CodeFirst_DestroyDevice(device);
}

/*Tests_SRS_SERIALIZER_99_039:[ WITH_ACTION shall declare an action of the current data provider called as the first macro parameter (name) and having the first parameter called parameter1Name of type parameter1Type, the second parameter named parameter2Name having the type parameter2Type and so on. ]*/
TEST_FUNCTION(InvokeAction_moveCarTo_with_wrong_type_for_destination_whereIsMyCar_fails)
{
    ///arrange
    CCodeFirstMocks mocks;
    void* device = CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &DummyDataProvider_WithStructs_allReflected, sizeof(TruckType), false);
    mocks.ResetAllCalls();
    AGENT_DATA_TYPE parameters[3];

    parameters[0].type = EDM_INT32_TYPE;
    parameters[0].value.edmInt32.value = 2;

    AGENT_DATA_TYPE CarLocation_Alt;
    CarLocation_Alt.type = EDM_INT32_TYPE;
    CarLocation_Alt.value.edmInt32.value = 3;

    AGENT_DATA_TYPE CarLocation_whereIsMyCar_Lat;
    CarLocation_whereIsMyCar_Lat.type = EDM_DOUBLE_TYPE;
    CarLocation_whereIsMyCar_Lat.value.edmDouble.value = 4.0;

    AGENT_DATA_TYPE CarLocation_whereIsMyCar_Long;
    CarLocation_whereIsMyCar_Long.type = EDM_DOUBLE_TYPE;
    CarLocation_whereIsMyCar_Long.value.edmDouble.value = 5.0;

    COMPLEX_TYPE_FIELD_TYPE CarLocation_whereIsMyCarFields[2];
    CarLocation_whereIsMyCarFields[0].fieldName = "Lat";
    CarLocation_whereIsMyCarFields[0].value = &CarLocation_whereIsMyCar_Lat;
    CarLocation_whereIsMyCarFields[1].fieldName = "Long";
    CarLocation_whereIsMyCarFields[1].value = &CarLocation_whereIsMyCar_Long;

    AGENT_DATA_TYPE CarLocation_whereIsMyCar;
    CarLocation_whereIsMyCar.type = EDM_INT64_TYPE;
    CarLocation_whereIsMyCar.value.edmComplexType.nMembers = 2;
    CarLocation_whereIsMyCar.value.edmComplexType.fields = CarLocation_whereIsMyCarFields;

    COMPLEX_TYPE_FIELD_TYPE carLocationFields[2];
    carLocationFields[0].fieldName = "Alt";
    carLocationFields[0].value = &CarLocation_Alt;
    carLocationFields[1].fieldName = "whereIsMyCar";
    carLocationFields[1].value = &CarLocation_whereIsMyCar;

    parameters[1].type = EDM_COMPLEX_TYPE_TYPE;
    parameters[1].value.edmComplexType.nMembers = 2;
    parameters[1].value.edmComplexType.fields = carLocationFields;

    parameters[2].type = EDM_BOOLEAN_TYPE;
    parameters[2].value.edmBoolean.value = EDM_TRUE;

    STRICT_EXPECTED_CALL(mocks, Schema_GetModelName(TEST_MODEL_HANDLE)).SetReturn("TruckType");

    ///act
   auto result = CodeFirst_InvokeAction(TEST_DEVICE_HANDLE, g_InvokeActionCallbackArgument, "", "moveCarTo", 3, parameters);

    ///assert
   ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
    ASSERT_ARE_EQUAL(bool, false, moveCarTo_wasCalled);
    mocks.AssertActualAndExpectedCalls();

    // cleanup
    CodeFirst_DestroyDevice(device);
}

/*Tests_SRS_SERIALIZER_99_039:[ WITH_ACTION shall declare an action of the current data provider called as the first macro parameter (name) and having the first parameter called parameter1Name of type parameter1Type, the second parameter named parameter2Name having the type parameter2Type and so on. ]*/
TEST_FUNCTION(InvokeAction_moveCarTo_with_wrong_type_for_destination_whereIsMyCar_Lat_fails)
{
    ///arrange
    CCodeFirstMocks mocks;
    void* device = CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &DummyDataProvider_WithStructs_allReflected, sizeof(TruckType), false);
    mocks.ResetAllCalls();
    AGENT_DATA_TYPE parameters[3];

    parameters[0].type = EDM_INT32_TYPE;
    parameters[0].value.edmInt32.value = 2;

    AGENT_DATA_TYPE CarLocation_Alt;
    CarLocation_Alt.type = EDM_INT32_TYPE;
    CarLocation_Alt.value.edmInt32.value = 3;

    AGENT_DATA_TYPE CarLocation_whereIsMyCar_Lat;
    CarLocation_whereIsMyCar_Lat.type = EDM_INT64_TYPE;
    CarLocation_whereIsMyCar_Lat.value.edmDouble.value = 4.0;

    AGENT_DATA_TYPE CarLocation_whereIsMyCar_Long;
    CarLocation_whereIsMyCar_Long.type = EDM_DOUBLE_TYPE;
    CarLocation_whereIsMyCar_Long.value.edmDouble.value = 5.0;

    COMPLEX_TYPE_FIELD_TYPE CarLocation_whereIsMyCarFields[2];
    CarLocation_whereIsMyCarFields[0].fieldName = "Lat";
    CarLocation_whereIsMyCarFields[0].value = &CarLocation_whereIsMyCar_Lat;
    CarLocation_whereIsMyCarFields[1].fieldName = "Long";
    CarLocation_whereIsMyCarFields[1].value = &CarLocation_whereIsMyCar_Long;

    AGENT_DATA_TYPE CarLocation_whereIsMyCar;
    CarLocation_whereIsMyCar.type = EDM_COMPLEX_TYPE_TYPE;
    CarLocation_whereIsMyCar.value.edmComplexType.nMembers = 2;
    CarLocation_whereIsMyCar.value.edmComplexType.fields = CarLocation_whereIsMyCarFields;

    COMPLEX_TYPE_FIELD_TYPE carLocationFields[2];
    carLocationFields[0].fieldName = "Alt";
    carLocationFields[0].value = &CarLocation_Alt;
    carLocationFields[1].fieldName = "whereIsMyCar";
    carLocationFields[1].value = &CarLocation_whereIsMyCar;

    parameters[1].type = EDM_COMPLEX_TYPE_TYPE;
    parameters[1].value.edmComplexType.nMembers = 2;
    parameters[1].value.edmComplexType.fields = carLocationFields;

    parameters[2].type = EDM_BOOLEAN_TYPE;
    parameters[2].value.edmBoolean.value = EDM_TRUE;

    STRICT_EXPECTED_CALL(mocks, Schema_GetModelName(TEST_MODEL_HANDLE)).SetReturn("TruckType");

    ///act
   auto result = CodeFirst_InvokeAction(TEST_DEVICE_HANDLE, g_InvokeActionCallbackArgument, "", "moveCarTo", 3, parameters);

    ///assert
   ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
    ASSERT_ARE_EQUAL(bool, false, moveCarTo_wasCalled);
    mocks.AssertActualAndExpectedCalls();

    // cleanup
    CodeFirst_DestroyDevice(device);
}

/*Tests_SRS_SERIALIZER_99_039:[ WITH_ACTION shall declare an action of the current data provider called as the first macro parameter (name) and having the first parameter called parameter1Name of type parameter1Type, the second parameter named parameter2Name having the type parameter2Type and so on. ]*/
TEST_FUNCTION(InvokeAction_moveCarTo_with_wrong_type_for_destination_whereIsMyCar_Long_fails)
{
    ///arrange
    CCodeFirstMocks mocks;
    void* device = CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &DummyDataProvider_WithStructs_allReflected, sizeof(TruckType), false);
    mocks.ResetAllCalls();
    AGENT_DATA_TYPE parameters[3];

    parameters[0].type = EDM_INT32_TYPE;
    parameters[0].value.edmInt32.value = 2;

    AGENT_DATA_TYPE CarLocation_Alt;
    CarLocation_Alt.type = EDM_INT32_TYPE;
    CarLocation_Alt.value.edmInt32.value = 3;

    AGENT_DATA_TYPE CarLocation_whereIsMyCar_Lat;
    CarLocation_whereIsMyCar_Lat.type = EDM_DOUBLE_TYPE;
    CarLocation_whereIsMyCar_Lat.value.edmDouble.value = 4.0;

    AGENT_DATA_TYPE CarLocation_whereIsMyCar_Long;
    CarLocation_whereIsMyCar_Long.type = EDM_INT64_TYPE;
    CarLocation_whereIsMyCar_Long.value.edmDouble.value = 5.0;

    COMPLEX_TYPE_FIELD_TYPE CarLocation_whereIsMyCarFields[2];
    CarLocation_whereIsMyCarFields[0].fieldName = "Lat";
    CarLocation_whereIsMyCarFields[0].value = &CarLocation_whereIsMyCar_Lat;
    CarLocation_whereIsMyCarFields[1].fieldName = "Long";
    CarLocation_whereIsMyCarFields[1].value = &CarLocation_whereIsMyCar_Long;

    AGENT_DATA_TYPE CarLocation_whereIsMyCar;
    CarLocation_whereIsMyCar.type = EDM_COMPLEX_TYPE_TYPE;
    CarLocation_whereIsMyCar.value.edmComplexType.nMembers = 2;
    CarLocation_whereIsMyCar.value.edmComplexType.fields = CarLocation_whereIsMyCarFields;

    COMPLEX_TYPE_FIELD_TYPE carLocationFields[2];
    carLocationFields[0].fieldName = "Alt";
    carLocationFields[0].value = &CarLocation_Alt;
    carLocationFields[1].fieldName = "whereIsMyCar";
    carLocationFields[1].value = &CarLocation_whereIsMyCar;

    parameters[1].type = EDM_COMPLEX_TYPE_TYPE;
    parameters[1].value.edmComplexType.nMembers = 2;
    parameters[1].value.edmComplexType.fields = carLocationFields;

    parameters[2].type = EDM_BOOLEAN_TYPE;
    parameters[2].value.edmBoolean.value = EDM_TRUE;

    STRICT_EXPECTED_CALL(mocks, Schema_GetModelName(TEST_MODEL_HANDLE)).SetReturn("TruckType");

    ///act
   auto result = CodeFirst_InvokeAction(TEST_DEVICE_HANDLE, g_InvokeActionCallbackArgument, "", "moveCarTo", 3, parameters);

    ///assert
   ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
    ASSERT_ARE_EQUAL(bool, false, moveCarTo_wasCalled);
    mocks.AssertActualAndExpectedCalls();

    // cleanup
    CodeFirst_DestroyDevice(device);
}

/*Tests_SRS_SERIALIZER_99_039:[ WITH_ACTION shall declare an action of the current data provider called as the first macro parameter (name) and having the first parameter called parameter1Name of type parameter1Type, the second parameter named parameter2Name having the type parameter2Type and so on. ]*/
TEST_FUNCTION(InvokeAction_moveCarTo_with_wrong_type_for_reverse_fails)
{
    ///arrange
    CCodeFirstMocks mocks;
    void* device = CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &DummyDataProvider_WithStructs_allReflected, sizeof(TruckType), false);
    mocks.ResetAllCalls();
    AGENT_DATA_TYPE parameters[3];

    parameters[0].type = EDM_INT32_TYPE;
    parameters[0].value.edmInt32.value = 2;

    AGENT_DATA_TYPE CarLocation_Alt;
    CarLocation_Alt.type = EDM_INT32_TYPE;
    CarLocation_Alt.value.edmInt32.value = 3;

    AGENT_DATA_TYPE CarLocation_whereIsMyCar_Lat;
    CarLocation_whereIsMyCar_Lat.type = EDM_DOUBLE_TYPE;
    CarLocation_whereIsMyCar_Lat.value.edmDouble.value = 4.0;

    AGENT_DATA_TYPE CarLocation_whereIsMyCar_Long;
    CarLocation_whereIsMyCar_Long.type = EDM_DOUBLE_TYPE;
    CarLocation_whereIsMyCar_Long.value.edmDouble.value = 5.0;

    COMPLEX_TYPE_FIELD_TYPE CarLocation_whereIsMyCarFields[2];
    CarLocation_whereIsMyCarFields[0].fieldName = "Lat";
    CarLocation_whereIsMyCarFields[0].value = &CarLocation_whereIsMyCar_Lat;
    CarLocation_whereIsMyCarFields[1].fieldName = "Long";
    CarLocation_whereIsMyCarFields[1].value = &CarLocation_whereIsMyCar_Long;

    AGENT_DATA_TYPE CarLocation_whereIsMyCar;
    CarLocation_whereIsMyCar.type = EDM_COMPLEX_TYPE_TYPE;
    CarLocation_whereIsMyCar.value.edmComplexType.nMembers = 2;
    CarLocation_whereIsMyCar.value.edmComplexType.fields = CarLocation_whereIsMyCarFields;

    COMPLEX_TYPE_FIELD_TYPE carLocationFields[2];
    carLocationFields[0].fieldName = "Alt";
    carLocationFields[0].value = &CarLocation_Alt;
    carLocationFields[1].fieldName = "whereIsMyCar";
    carLocationFields[1].value = &CarLocation_whereIsMyCar;

    parameters[1].type = EDM_COMPLEX_TYPE_TYPE;
    parameters[1].value.edmComplexType.nMembers = 2;
    parameters[1].value.edmComplexType.fields = carLocationFields;

    parameters[2].type = EDM_INT64_TYPE;
    parameters[2].value.edmBoolean.value = EDM_TRUE;

    STRICT_EXPECTED_CALL(mocks, Schema_GetModelName(TEST_MODEL_HANDLE)).SetReturn("TruckType");

    ///act
   auto result = CodeFirst_InvokeAction(TEST_DEVICE_HANDLE, g_InvokeActionCallbackArgument, "", "moveCarTo", 3, parameters);

    ///assert
   ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
    ASSERT_ARE_EQUAL(bool, false, moveCarTo_wasCalled);
    mocks.AssertActualAndExpectedCalls();

    // cleanup
    CodeFirst_DestroyDevice(device);
}

/*Tests_SRS_SERIALIZER_99_039:[ WITH_ACTION shall declare an action of the current data provider called as the first macro parameter (name) and having the first parameter called parameter1Name of type parameter1Type, the second parameter named parameter2Name having the type parameter2Type and so on. ]*/
TEST_FUNCTION(InvokeAction_moveCarTo_with_too_many_fields_in_CarLocation_fails)
{
    ///arrange
    CCodeFirstMocks mocks;
    void* device = CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &DummyDataProvider_WithStructs_allReflected, sizeof(TruckType), false);
    mocks.ResetAllCalls();
    AGENT_DATA_TYPE parameters[3];

    parameters[0].type = EDM_INT32_TYPE;
    parameters[0].value.edmInt32.value = 2;

    AGENT_DATA_TYPE CarLocation_Alt;
    CarLocation_Alt.type = EDM_INT32_TYPE;
    CarLocation_Alt.value.edmInt32.value = 3;

    AGENT_DATA_TYPE CarLocation_whereIsMyCar_Lat;
    CarLocation_whereIsMyCar_Lat.type = EDM_DOUBLE_TYPE;
    CarLocation_whereIsMyCar_Lat.value.edmDouble.value = 4.0;

    AGENT_DATA_TYPE CarLocation_whereIsMyCar_Long;
    CarLocation_whereIsMyCar_Long.type = EDM_DOUBLE_TYPE;
    CarLocation_whereIsMyCar_Long.value.edmDouble.value = 5.0;

    COMPLEX_TYPE_FIELD_TYPE CarLocation_whereIsMyCarFields[2];
    CarLocation_whereIsMyCarFields[0].fieldName = "Lat";
    CarLocation_whereIsMyCarFields[0].value = &CarLocation_whereIsMyCar_Lat;
    CarLocation_whereIsMyCarFields[1].fieldName = "Long";
    CarLocation_whereIsMyCarFields[1].value = &CarLocation_whereIsMyCar_Long;

    AGENT_DATA_TYPE CarLocation_whereIsMyCar;
    CarLocation_whereIsMyCar.type = EDM_COMPLEX_TYPE_TYPE;
    CarLocation_whereIsMyCar.value.edmComplexType.nMembers = 2;
    CarLocation_whereIsMyCar.value.edmComplexType.fields = CarLocation_whereIsMyCarFields;

    COMPLEX_TYPE_FIELD_TYPE carLocationFields[2];
    carLocationFields[0].fieldName = "Alt";
    carLocationFields[0].value = &CarLocation_Alt;
    carLocationFields[1].fieldName = "whereIsMyCar";
    carLocationFields[1].value = &CarLocation_whereIsMyCar;

    parameters[1].type = EDM_COMPLEX_TYPE_TYPE;
    parameters[1].value.edmComplexType.nMembers = 3;
    parameters[1].value.edmComplexType.fields = carLocationFields;

    parameters[2].type = EDM_BOOLEAN_TYPE;
    parameters[2].value.edmBoolean.value = EDM_TRUE;

    STRICT_EXPECTED_CALL(mocks, Schema_GetModelName(TEST_MODEL_HANDLE)).SetReturn("TruckType");

    ///act
   auto result = CodeFirst_InvokeAction(TEST_DEVICE_HANDLE, g_InvokeActionCallbackArgument, "", "moveCarTo", 3, parameters);

    ///assert
   ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
    ASSERT_ARE_EQUAL(bool, false, moveCarTo_wasCalled);
    mocks.AssertActualAndExpectedCalls();

    // cleanup
    CodeFirst_DestroyDevice(device);
}

/*Tests_SRS_SERIALIZER_99_039:[ WITH_ACTION shall declare an action of the current data provider called as the first macro parameter (name) and having the first parameter called parameter1Name of type parameter1Type, the second parameter named parameter2Name having the type parameter2Type and so on. ]*/
TEST_FUNCTION(InvokeAction_moveCarTo_with_too_few_fields_in_CarLocation_fails)
{
    ///arrange
    CCodeFirstMocks mocks;
    void* device = CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &DummyDataProvider_WithStructs_allReflected, sizeof(TruckType), false);
    mocks.ResetAllCalls();
    AGENT_DATA_TYPE parameters[3];

    parameters[0].type = EDM_INT32_TYPE;
    parameters[0].value.edmInt32.value = 2;

    AGENT_DATA_TYPE CarLocation_Alt;
    CarLocation_Alt.type = EDM_INT32_TYPE;
    CarLocation_Alt.value.edmInt32.value = 3;

    AGENT_DATA_TYPE CarLocation_whereIsMyCar_Lat;
    CarLocation_whereIsMyCar_Lat.type = EDM_DOUBLE_TYPE;
    CarLocation_whereIsMyCar_Lat.value.edmDouble.value = 4.0;

    AGENT_DATA_TYPE CarLocation_whereIsMyCar_Long;
    CarLocation_whereIsMyCar_Long.type = EDM_DOUBLE_TYPE;
    CarLocation_whereIsMyCar_Long.value.edmDouble.value = 5.0;

    COMPLEX_TYPE_FIELD_TYPE CarLocation_whereIsMyCarFields[2];
    CarLocation_whereIsMyCarFields[0].fieldName = "Lat";
    CarLocation_whereIsMyCarFields[0].value = &CarLocation_whereIsMyCar_Lat;
    CarLocation_whereIsMyCarFields[1].fieldName = "Long";
    CarLocation_whereIsMyCarFields[1].value = &CarLocation_whereIsMyCar_Long;

    AGENT_DATA_TYPE CarLocation_whereIsMyCar;
    CarLocation_whereIsMyCar.type = EDM_COMPLEX_TYPE_TYPE;
    CarLocation_whereIsMyCar.value.edmComplexType.nMembers = 2;
    CarLocation_whereIsMyCar.value.edmComplexType.fields = CarLocation_whereIsMyCarFields;

    COMPLEX_TYPE_FIELD_TYPE carLocationFields[2];
    carLocationFields[0].fieldName = "Alt";
    carLocationFields[0].value = &CarLocation_Alt;
    carLocationFields[1].fieldName = "whereIsMyCar";
    carLocationFields[1].value = &CarLocation_whereIsMyCar;

    parameters[1].type = EDM_COMPLEX_TYPE_TYPE;
    parameters[1].value.edmComplexType.nMembers = 1;
    parameters[1].value.edmComplexType.fields = carLocationFields;

    parameters[2].type = EDM_BOOLEAN_TYPE;
    parameters[2].value.edmBoolean.value = EDM_TRUE;

    STRICT_EXPECTED_CALL(mocks, Schema_GetModelName(TEST_MODEL_HANDLE)).SetReturn("TruckType");

    ///act
   auto result = CodeFirst_InvokeAction(TEST_DEVICE_HANDLE, g_InvokeActionCallbackArgument, "", "moveCarTo", 3, parameters);

    ///assert
   ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
    ASSERT_ARE_EQUAL(bool, false, moveCarTo_wasCalled);
    mocks.AssertActualAndExpectedCalls();

    // cleanup
    CodeFirst_DestroyDevice(device);
}

/*Tests_SRS_SERIALIZER_99_039:[ WITH_ACTION shall declare an action of the current data provider called as the first macro parameter (name) and having the first parameter called parameter1Name of type parameter1Type, the second parameter named parameter2Name having the type parameter2Type and so on. ]*/
TEST_FUNCTION(InvokeAction_moveCarTo_with_too_many_fields_in_CarLocation_whereIsMyCar_fails)
{
    ///arrange
    CCodeFirstMocks mocks;
    void* device = CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &DummyDataProvider_WithStructs_allReflected, sizeof(TruckType), false);
    mocks.ResetAllCalls();
    AGENT_DATA_TYPE parameters[3];

    parameters[0].type = EDM_INT32_TYPE;
    parameters[0].value.edmInt32.value = 2;

    AGENT_DATA_TYPE CarLocation_Alt;
    CarLocation_Alt.type = EDM_INT32_TYPE;
    CarLocation_Alt.value.edmInt32.value = 3;

    AGENT_DATA_TYPE CarLocation_whereIsMyCar_Lat;
    CarLocation_whereIsMyCar_Lat.type = EDM_DOUBLE_TYPE;
    CarLocation_whereIsMyCar_Lat.value.edmDouble.value = 4.0;

    AGENT_DATA_TYPE CarLocation_whereIsMyCar_Long;
    CarLocation_whereIsMyCar_Long.type = EDM_DOUBLE_TYPE;
    CarLocation_whereIsMyCar_Long.value.edmDouble.value = 5.0;

    COMPLEX_TYPE_FIELD_TYPE CarLocation_whereIsMyCarFields[2];
    CarLocation_whereIsMyCarFields[0].fieldName = "Lat";
    CarLocation_whereIsMyCarFields[0].value = &CarLocation_whereIsMyCar_Lat;
    CarLocation_whereIsMyCarFields[1].fieldName = "Long";
    CarLocation_whereIsMyCarFields[1].value = &CarLocation_whereIsMyCar_Long;

    AGENT_DATA_TYPE CarLocation_whereIsMyCar;
    CarLocation_whereIsMyCar.type = EDM_COMPLEX_TYPE_TYPE;
    CarLocation_whereIsMyCar.value.edmComplexType.nMembers = 3;
    CarLocation_whereIsMyCar.value.edmComplexType.fields = CarLocation_whereIsMyCarFields;

    COMPLEX_TYPE_FIELD_TYPE carLocationFields[2];
    carLocationFields[0].fieldName = "Alt";
    carLocationFields[0].value = &CarLocation_Alt;
    carLocationFields[1].fieldName = "whereIsMyCar";
    carLocationFields[1].value = &CarLocation_whereIsMyCar;

    parameters[1].type = EDM_COMPLEX_TYPE_TYPE;
    parameters[1].value.edmComplexType.nMembers = 2;
    parameters[1].value.edmComplexType.fields = carLocationFields;

    parameters[2].type = EDM_BOOLEAN_TYPE;
    parameters[2].value.edmBoolean.value = EDM_TRUE;

    STRICT_EXPECTED_CALL(mocks, Schema_GetModelName(TEST_MODEL_HANDLE)).SetReturn("TruckType");

    ///act
   auto result = CodeFirst_InvokeAction(TEST_DEVICE_HANDLE, g_InvokeActionCallbackArgument, "", "moveCarTo", 3, parameters);

    ///assert
   ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
    ASSERT_ARE_EQUAL(bool, false, moveCarTo_wasCalled);
    mocks.AssertActualAndExpectedCalls();

    // cleanup
    CodeFirst_DestroyDevice(device);
}

/*Tests_SRS_SERIALIZER_99_039:[ WITH_ACTION shall declare an action of the current data provider called as the first macro parameter (name) and having the first parameter called parameter1Name of type parameter1Type, the second parameter named parameter2Name having the type parameter2Type and so on. ]*/
TEST_FUNCTION(InvokeAction_moveCarTo_with_too_few_fields_in_CarLocation_whereIsMyCar_fails)
{
    ///arrange
    CCodeFirstMocks mocks;
    void* device = CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &DummyDataProvider_WithStructs_allReflected, sizeof(TruckType), false);
    mocks.ResetAllCalls();
    AGENT_DATA_TYPE parameters[3];

    parameters[0].type = EDM_INT32_TYPE;
    parameters[0].value.edmInt32.value = 2;

    AGENT_DATA_TYPE CarLocation_Alt;
    CarLocation_Alt.type = EDM_INT32_TYPE;
    CarLocation_Alt.value.edmInt32.value = 3;

    AGENT_DATA_TYPE CarLocation_whereIsMyCar_Lat;
    CarLocation_whereIsMyCar_Lat.type = EDM_DOUBLE_TYPE;
    CarLocation_whereIsMyCar_Lat.value.edmDouble.value = 4.0;

    AGENT_DATA_TYPE CarLocation_whereIsMyCar_Long;
    CarLocation_whereIsMyCar_Long.type = EDM_DOUBLE_TYPE;
    CarLocation_whereIsMyCar_Long.value.edmDouble.value = 5.0;

    COMPLEX_TYPE_FIELD_TYPE CarLocation_whereIsMyCarFields[2];
    CarLocation_whereIsMyCarFields[0].fieldName = "Lat";
    CarLocation_whereIsMyCarFields[0].value = &CarLocation_whereIsMyCar_Lat;
    CarLocation_whereIsMyCarFields[1].fieldName = "Long";
    CarLocation_whereIsMyCarFields[1].value = &CarLocation_whereIsMyCar_Long;

    AGENT_DATA_TYPE CarLocation_whereIsMyCar;
    CarLocation_whereIsMyCar.type = EDM_COMPLEX_TYPE_TYPE;
    CarLocation_whereIsMyCar.value.edmComplexType.nMembers = 1;
    CarLocation_whereIsMyCar.value.edmComplexType.fields = CarLocation_whereIsMyCarFields;

    COMPLEX_TYPE_FIELD_TYPE carLocationFields[2];
    carLocationFields[0].fieldName = "Alt";
    carLocationFields[0].value = &CarLocation_Alt;
    carLocationFields[1].fieldName = "whereIsMyCar";
    carLocationFields[1].value = &CarLocation_whereIsMyCar;

    parameters[1].type = EDM_COMPLEX_TYPE_TYPE;
    parameters[1].value.edmComplexType.nMembers = 2;
    parameters[1].value.edmComplexType.fields = carLocationFields;

    parameters[2].type = EDM_BOOLEAN_TYPE;
    parameters[2].value.edmBoolean.value = EDM_TRUE;

    STRICT_EXPECTED_CALL(mocks, Schema_GetModelName(TEST_MODEL_HANDLE)).SetReturn("TruckType");

    ///act
   auto result = CodeFirst_InvokeAction(TEST_DEVICE_HANDLE, g_InvokeActionCallbackArgument, "", "moveCarTo", 3, parameters);

    ///assert
   ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
    ASSERT_ARE_EQUAL(bool, false, moveCarTo_wasCalled);
    mocks.AssertActualAndExpectedCalls();

    // cleanup
    CodeFirst_DestroyDevice(device);
}

END_TEST_SUITE(CodeFirst_ut_Two_Providers_With_Structs);
