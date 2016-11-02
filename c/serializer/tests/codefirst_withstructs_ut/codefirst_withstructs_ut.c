// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifdef __cplusplus
#include <cstdlib>
#else
#include <stdlib.h>
#endif

void* my_gballoc_malloc(size_t t)
{
    return malloc(t);
}

void my_gballoc_free(void * t)
{
    free(t);
}

#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "macro_utils.h"
#include "umock_c.h"
#include "umocktypes_charptr.h"
#include "umocktypes_bool.h"
#include "umocktypes_stdint.h"
#include "umock_c_negative_tests.h"

#define ENABLE_MOCKS
#include "agenttypesystem.h"
#include "schema.h"
#include "iotdevice.h"
#undef ENABLE_MOCKS

#include "testrunnerswitcher.h"
#include "codefirst.h"
#include "azure_c_shared_utility/macro_utils.h"

#include "serializer.h"

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

static const DEVICE_HANDLE TEST_DEVICE_HANDLE = (DEVICE_HANDLE)0x4848;
static const SCHEMA_MODEL_TYPE_HANDLE TEST_MODEL_HANDLE = (SCHEMA_MODEL_TYPE_HANDLE)0x4243;
static void* g_InvokeActionCallbackArgument;

#define MAX_RECORDINGS 100
#define MAX_NAME_LENGTH 100
typedef char someName[MAX_NAME_LENGTH];
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

static DEVICE_HANDLE serializer_getdevicehandle(void) { return TEST_DEVICE_HANDLE; }

static TEST_MUTEX_HANDLE g_testByTest;
static TEST_MUTEX_HANDLE g_dllByDll;

static STRING_HANDLE global_bufferTemp;

DEFINE_ENUM_STRINGS(UMOCK_C_ERROR_CODE, UMOCK_C_ERROR_CODE_VALUES)
static void on_umock_c_error(UMOCK_C_ERROR_CODE error_code)
{
    char temp_str[256];
    (void)snprintf(temp_str, sizeof(temp_str), "umock_c reported error :%s", ENUM_TO_STRING(UMOCK_C_ERROR_CODE, error_code));
    ASSERT_FAIL(temp_str);
}

TEST_DEFINE_ENUM_TYPE(CODEFIRST_RESULT, CODEFIRST_RESULT_VALUES);
IMPLEMENT_UMOCK_C_ENUM_TYPE(CODEFIRST_RESULT, CODEFIRST_RESULT_VALUES);
TEST_DEFINE_ENUM_TYPE(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_RESULT_VALUES);
IMPLEMENT_UMOCK_C_ENUM_TYPE(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_RESULT_VALUES);


static DEVICE_RESULT my_Device_Create(SCHEMA_MODEL_TYPE_HANDLE modelHandle, pfDeviceActionCallback deviceActionCallback, void* callbackUserContext, bool includePropertyPath, DEVICE_HANDLE* deviceHandle)
{
    (void)(modelHandle, deviceActionCallback, includePropertyPath);
    *deviceHandle = TEST_DEVICE_HANDLE;
    g_InvokeActionCallbackArgument = callbackUserContext;
    return DEVICE_OK;
}

static SCHEMA_RESULT my_Schema_GetModelDesiredPropertyCount(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, size_t* desiredPropertyCount)
{
    (void)modelTypeHandle;
    if (desiredPropertyCount != NULL)
    {
        *desiredPropertyCount = 0;
    }
    return SCHEMA_OK;
}

static SCHEMA_RESULT my_Schema_GetModelModelCount(SCHEMA_MODEL_TYPE_HANDLE modelTypeHandle, size_t* modelCount)
{
    (void)modelTypeHandle;
    if (modelCount != NULL)
    {
        *modelCount = 0;
    }
    return SCHEMA_OK;
}

BEGIN_TEST_SUITE(CodeFirst_ut_Two_Providers_With_Structs)

TEST_SUITE_INITIALIZE(TestClassInitialize)
{
    TEST_INITIALIZE_MEMORY_DEBUG(g_dllByDll);

    g_testByTest = TEST_MUTEX_CREATE();
    ASSERT_IS_NOT_NULL(g_testByTest);

    (void)umock_c_init(on_umock_c_error);

    (void)umocktypes_bool_register_types();
    (void)umocktypes_charptr_register_types();
    (void)umocktypes_stdint_register_types();

    REGISTER_GLOBAL_MOCK_HOOK(Device_Create, my_Device_Create);
    REGISTER_UMOCK_ALIAS_TYPE(SCHEMA_MODEL_TYPE_HANDLE, void*);
    REGISTER_UMOCK_ALIAS_TYPE(pfDeviceActionCallback, void*);
    REGISTER_UMOCK_ALIAS_TYPE(DEVICE_HANDLE, void*);
    REGISTER_UMOCK_ALIAS_TYPE(STRING_HANDLE, void*);
    
    REGISTER_GLOBAL_MOCK_HOOK(Schema_GetModelDesiredPropertyCount, my_Schema_GetModelDesiredPropertyCount);
    REGISTER_GLOBAL_MOCK_HOOK(Schema_GetModelModelCount, my_Schema_GetModelModelCount);

}

TEST_SUITE_CLEANUP(TestClassCleanup)
{
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

    global_bufferTemp = STRING_new();

}

TEST_FUNCTION_CLEANUP(TestMethodCleanup)
{
    STRING_delete(global_bufferTemp);

    TEST_MUTEX_RELEASE(g_testByTest);
}

/*Tests_SRS_CODEFIRST_99_002:[ CodeFirst_Init shall initialize the CodeFirst module. If initialization is successful, it shall return CODEFIRST_OK.]*/
TEST_FUNCTION(CodeFirst_Init_succeds)
{
    umock_c_reset_all_calls();

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
    void* device = CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &DummyDataProvider_WithStructs_allReflected, sizeof(TruckType), false);
    umock_c_reset_all_calls();
    AGENT_DATA_TYPE someLocation;

    static AGENT_DATA_TYPE Lat;
    Lat.type = EDM_DOUBLE_TYPE;
    Lat.value.edmDouble.value = 3.0;

    static AGENT_DATA_TYPE Long;
    Long.type = EDM_DOUBLE_TYPE;
    Long.value.edmDouble.value = 4.0;

    COMPLEX_TYPE_FIELD_TYPE fieldsOfSomeLocation[2] = { { "Lat", &Lat }, { "Long", &Long } };

    someLocation.type = EDM_COMPLEX_TYPE_TYPE;
    someLocation.value.edmComplexType.nMembers = 2;
    someLocation.value.edmComplexType.fields = fieldsOfSomeLocation;

    STRICT_EXPECTED_CALL(Schema_GetModelName(TEST_MODEL_HANDLE)).SetReturn("TruckType");

    ///act
    EXECUTE_COMMAND_RESULT result = CodeFirst_InvokeAction(TEST_DEVICE_HANDLE, g_InvokeActionCallbackArgument, "", "goToLocation", 1, &someLocation);

    ///assert
    ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_SUCCESS, result);
    ASSERT_IS_TRUE(goToLocation_wasCalled);
    ASSERT_ARE_EQUAL(double, 4.0, goToLocation_XMarksTheSpot_long);
    ASSERT_ARE_EQUAL(double, 3.0, goToLocation_XMarksTheSpot_lat);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    CodeFirst_DestroyDevice(device);
}

/*Tests_SRS_CODEFIRST_02_013: [The wrapper's return value shall be returned.] */
TEST_FUNCTION(InvokeAction_alwaysRejected_return_EXECUTE_COMMAND_FAILED)
{
    ///arrange
    void* device = CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &DummyDataProvider_WithStructs_allReflected, sizeof(TruckType), false);
    umock_c_reset_all_calls();
   
    STRICT_EXPECTED_CALL(Schema_GetModelName(TEST_MODEL_HANDLE)).SetReturn("TruckType");

    ///act
    EXECUTE_COMMAND_RESULT result = CodeFirst_InvokeAction(TEST_DEVICE_HANDLE, g_InvokeActionCallbackArgument, "", "alwaysReject", 0, NULL);

    ///assert
    ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_FAILED, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    CodeFirst_DestroyDevice(device);
}

/*Tests_SRS_CODEFIRST_02_013: [The wrapper's return value shall be returned.] */
TEST_FUNCTION(InvokeAction_alwaysAbandon_return_EXECUTE_COMMAND_ERROR)
{
    ///arrange
    void* device = CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &DummyDataProvider_WithStructs_allReflected, sizeof(TruckType), false);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(Schema_GetModelName(TEST_MODEL_HANDLE)).SetReturn("TruckType");

    ///act
    EXECUTE_COMMAND_RESULT result = CodeFirst_InvokeAction(TEST_DEVICE_HANDLE, g_InvokeActionCallbackArgument, "", "alwaysAbandon", 0, NULL);

    ///assert
    ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    CodeFirst_DestroyDevice(device);
}

/*the below tests are derived from the one above by trying to feed wrong data in the CodeFirst_InvokeAction*/

/*Tests_SRS_SERIALIZER_99_039:[ WITH_ACTION shall declare an action of the current data provider called as the first macro parameter (name) and having the first parameter called parameter1Name of type parameter1Type, the second parameter named parameter2Name having the type parameter2Type and so on. ]*/
TEST_FUNCTION(InvokeAction_goToLocation_with_a_struct_with_3_fields_fails)
{
    ///arrange
    void* device = CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &DummyDataProvider_WithStructs_allReflected, sizeof(TruckType), false);
    umock_c_reset_all_calls();
    static AGENT_DATA_TYPE someLocation;

    static AGENT_DATA_TYPE Lat;
    Lat.type = EDM_DOUBLE_TYPE;
    Lat.value.edmDouble.value = 3.0;

    static AGENT_DATA_TYPE Long;
    Long.type = EDM_DOUBLE_TYPE;
    Long.value.edmDouble.value = 4.0;

    static AGENT_DATA_TYPE Extra;
    Extra.type = EDM_DOUBLE_TYPE;
    Extra.value.edmDouble.value = 4.0;

    COMPLEX_TYPE_FIELD_TYPE fieldsOfSomeLocation[3] = { { "Lat", &Lat }, { "Long", &Long }, { "Extra", &Extra } };

    someLocation.type = EDM_COMPLEX_TYPE_TYPE;
    someLocation.value.edmComplexType.nMembers = 3;
    someLocation.value.edmComplexType.fields = fieldsOfSomeLocation;

    STRICT_EXPECTED_CALL(Schema_GetModelName(TEST_MODEL_HANDLE)).SetReturn("TruckType");

    ///act
    EXECUTE_COMMAND_RESULT result = CodeFirst_InvokeAction(TEST_DEVICE_HANDLE, g_InvokeActionCallbackArgument, "", "goToLocation", 1, &someLocation);

    ///assert
    ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
    ASSERT_IS_FALSE(goToLocation_wasCalled);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    CodeFirst_DestroyDevice(device);
}

/*Tests_SRS_SERIALIZER_99_039:[ WITH_ACTION shall declare an action of the current data provider called as the first macro parameter (name) and having the first parameter called parameter1Name of type parameter1Type, the second parameter named parameter2Name having the type parameter2Type and so on. ]*/
TEST_FUNCTION(InvokeAction_goToLocation_with_a_struct_with_1_field_fails)
{
    ///arrange
    void* device = CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &DummyDataProvider_WithStructs_allReflected, sizeof(TruckType), false);
    umock_c_reset_all_calls();
    static AGENT_DATA_TYPE someLocation;

    static AGENT_DATA_TYPE Lat;
    Lat.type = EDM_DOUBLE_TYPE;
    Lat.value.edmDouble.value = 3.0;

    COMPLEX_TYPE_FIELD_TYPE fieldsOfSomeLocation[1] = { { "Lat", &Lat } };

    someLocation.type = EDM_COMPLEX_TYPE_TYPE;
    someLocation.value.edmComplexType.nMembers = 1;
    someLocation.value.edmComplexType.fields = fieldsOfSomeLocation;

    STRICT_EXPECTED_CALL(Schema_GetModelName(TEST_MODEL_HANDLE)).SetReturn("TruckType");

    ///act
    EXECUTE_COMMAND_RESULT result = CodeFirst_InvokeAction(TEST_DEVICE_HANDLE, g_InvokeActionCallbackArgument, "", "goToLocation", 1, &someLocation);

    ///assert
    ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
    ASSERT_IS_FALSE(goToLocation_wasCalled);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    CodeFirst_DestroyDevice(device);
}

/*Tests_SRS_SERIALIZER_99_039:[ WITH_ACTION shall declare an action of the current data provider called as the first macro parameter (name) and having the first parameter called parameter1Name of type parameter1Type, the second parameter named parameter2Name having the type parameter2Type and so on. ]*/
TEST_FUNCTION(InvokeAction_goToLocation_with_a_non_struct_fails)
{
    ///arrange
    void* device = CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &DummyDataProvider_WithStructs_allReflected, sizeof(TruckType), false);
    umock_c_reset_all_calls();

    static AGENT_DATA_TYPE Lat;
    Lat.type = EDM_DOUBLE_TYPE;
    Lat.value.edmDouble.value = 3.0;

    STRICT_EXPECTED_CALL(Schema_GetModelName(TEST_MODEL_HANDLE)).SetReturn("TruckType");

    ///act
    EXECUTE_COMMAND_RESULT result = CodeFirst_InvokeAction(TEST_DEVICE_HANDLE, g_InvokeActionCallbackArgument, "", "goToLocation", 1, &Lat);

    ///assert
    ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
    ASSERT_IS_FALSE(goToLocation_wasCalled);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    CodeFirst_DestroyDevice(device);
}

/*Tests_SRS_SERIALIZER_99_039:[ WITH_ACTION shall declare an action of the current data provider called as the first macro parameter (name) and having the first parameter called parameter1Name of type parameter1Type, the second parameter named parameter2Name having the type parameter2Type and so on. ]*/
TEST_FUNCTION(InvokeAction_goToLocation_with_a_struct_and_a_double_fails)
{
    ///arrange
    void* device = CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &DummyDataProvider_WithStructs_allReflected, sizeof(TruckType), false);
    umock_c_reset_all_calls();
    AGENT_DATA_TYPE someLocationAndDouble[2];

    static AGENT_DATA_TYPE Lat;
    Lat.type = EDM_DOUBLE_TYPE;
    Lat.value.edmDouble.value = 3.0;

    static AGENT_DATA_TYPE Long;
    Long.type = EDM_DOUBLE_TYPE;
    Long.value.edmDouble.value = 4.0;

    COMPLEX_TYPE_FIELD_TYPE someLocationAndDouble_1[2] = { { "Lat", &Lat }, { "Long", &Long } };

    someLocationAndDouble[0].type = EDM_COMPLEX_TYPE_TYPE;
    someLocationAndDouble[0].value.edmComplexType.nMembers = 2;
    someLocationAndDouble[0].value.edmComplexType.fields = someLocationAndDouble_1;

    someLocationAndDouble[1].type = EDM_DOUBLE_TYPE;
    someLocationAndDouble[1].value.edmDouble.value = EDM_DOUBLE_TYPE;

    STRICT_EXPECTED_CALL(Schema_GetModelName(TEST_MODEL_HANDLE)).SetReturn("TruckType");

    ///act
    EXECUTE_COMMAND_RESULT result = CodeFirst_InvokeAction(TEST_DEVICE_HANDLE, g_InvokeActionCallbackArgument, "", "goToLocation", 2, someLocationAndDouble);

    ///assert
    ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
    ASSERT_IS_FALSE(goToLocation_wasCalled);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    CodeFirst_DestroyDevice(device);
}

/*Tests_SRS_SERIALIZER_99_039:[ WITH_ACTION shall declare an action of the current data provider called as the first macro parameter (name) and having the first parameter called parameter1Name of type parameter1Type, the second parameter named parameter2Name having the type parameter2Type and so on. ]*/
TEST_FUNCTION(InvokeAction_goToLocation_with_a_double_and_a_struct_fails)
{
    ///arrange
    void* device = CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &DummyDataProvider_WithStructs_allReflected, sizeof(TruckType), false);
    umock_c_reset_all_calls();
    AGENT_DATA_TYPE someLocationAndDouble[2];

    static AGENT_DATA_TYPE Lat;
    Lat.type = EDM_DOUBLE_TYPE;
    Lat.value.edmDouble.value = 3.0;

    static AGENT_DATA_TYPE Long;
    Long.type = EDM_DOUBLE_TYPE;
    Long.value.edmDouble.value = 4.0;

    COMPLEX_TYPE_FIELD_TYPE someLocationAndDouble_1[2] = { { "Lat", &Lat }, { "Long", &Long } };

    someLocationAndDouble[1].type = EDM_COMPLEX_TYPE_TYPE;
    someLocationAndDouble[1].value.edmComplexType.nMembers = 2;
    someLocationAndDouble[1].value.edmComplexType.fields = someLocationAndDouble_1;

    someLocationAndDouble[0].type = EDM_DOUBLE_TYPE;
    someLocationAndDouble[0].value.edmDouble.value = EDM_DOUBLE_TYPE;

    STRICT_EXPECTED_CALL(Schema_GetModelName(TEST_MODEL_HANDLE)).SetReturn("TruckType");

    ///act
   EXECUTE_COMMAND_RESULT result = CodeFirst_InvokeAction(TEST_DEVICE_HANDLE, g_InvokeActionCallbackArgument, "", "goToLocation", 2, someLocationAndDouble);

    ///assert
   ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
    ASSERT_IS_FALSE(goToLocation_wasCalled);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    CodeFirst_DestroyDevice(device);
}

/*Tests_SRS_SERIALIZER_99_039:[ WITH_ACTION shall declare an action of the current data provider called as the first macro parameter (name) and having the first parameter called parameter1Name of type parameter1Type, the second parameter named parameter2Name having the type parameter2Type and so on. ]*/
TEST_FUNCTION(InvokeAction_goToLocation_with_a_struct_with_first_field_of_wrong_type_fails)
{
    ///arrange
    void* device = CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &DummyDataProvider_WithStructs_allReflected, sizeof(TruckType), false);
    umock_c_reset_all_calls();
    AGENT_DATA_TYPE someLocation;

    static AGENT_DATA_TYPE Lat;
    Lat.type = EDM_SINGLE_TYPE;
    Lat.value.edmDouble.value = 3.0f;

    static AGENT_DATA_TYPE Long;
    Long.type = EDM_DOUBLE_TYPE;
    Long.value.edmDouble.value = 4.0;

    COMPLEX_TYPE_FIELD_TYPE fieldsOfSomeLocation[2] = { { "Lat", &Lat }, { "Long", &Long } };

    someLocation.type = EDM_COMPLEX_TYPE_TYPE;
    someLocation.value.edmComplexType.nMembers = 2;
    someLocation.value.edmComplexType.fields = fieldsOfSomeLocation;

    STRICT_EXPECTED_CALL(Schema_GetModelName(TEST_MODEL_HANDLE)).SetReturn("TruckType");

    ///act
   EXECUTE_COMMAND_RESULT result = CodeFirst_InvokeAction(TEST_DEVICE_HANDLE, g_InvokeActionCallbackArgument, "", "goToLocation", 1, &someLocation);

    ///assert
   ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
    ASSERT_IS_FALSE(goToLocation_wasCalled);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    CodeFirst_DestroyDevice(device);
}

/*Tests_SRS_SERIALIZER_99_039:[ WITH_ACTION shall declare an action of the current data provider called as the first macro parameter (name) and having the first parameter called parameter1Name of type parameter1Type, the second parameter named parameter2Name having the type parameter2Type and so on. ]*/
TEST_FUNCTION(InvokeAction_goToLocation_with_a_struct_with_second_field_of_wrong_type_fails)
{
    ///arrange
    void* device = CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &DummyDataProvider_WithStructs_allReflected, sizeof(TruckType), false);
    umock_c_reset_all_calls();
    AGENT_DATA_TYPE someLocation;

    static AGENT_DATA_TYPE Lat;
    Lat.type = EDM_DOUBLE_TYPE;
    Lat.value.edmDouble.value = 3.0;

    static AGENT_DATA_TYPE Long;
    Long.type = EDM_SINGLE_TYPE;
    Long.value.edmDouble.value = 4.0f;

    COMPLEX_TYPE_FIELD_TYPE fieldsOfSomeLocation[2] = { { "Lat", &Lat }, { "Long", &Long } };

    someLocation.type = EDM_COMPLEX_TYPE_TYPE;
    someLocation.value.edmComplexType.nMembers = 2;
    someLocation.value.edmComplexType.fields = fieldsOfSomeLocation;

    STRICT_EXPECTED_CALL(Schema_GetModelName(TEST_MODEL_HANDLE)).SetReturn("TruckType");

    ///act
    EXECUTE_COMMAND_RESULT result = CodeFirst_InvokeAction(TEST_DEVICE_HANDLE, g_InvokeActionCallbackArgument, "", "goToLocation", 1, &someLocation);

    ///assert
    ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
    ASSERT_IS_FALSE(goToLocation_wasCalled);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    CodeFirst_DestroyDevice(device);
}

/*Tests_SRS_SERIALIZER_99_039:[ WITH_ACTION shall declare an action of the current data provider called as the first macro parameter (name) and having the first parameter called parameter1Name of type parameter1Type, the second parameter named parameter2Name having the type parameter2Type and so on. ]*/
/*Tests_SRS_SERIALIZER_99_042:[ The parameter types are either predefined parameter types (specs SRS_SERIALIZER_99_004-SRS_SERIALIZER_99_014) or a type introduced by DECLARE_STRUCT.]*/
TEST_FUNCTION(InvokeAction_moveCarTo_succeeds)
{
    ///arrange
    void* device = CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &DummyDataProvider_WithStructs_allReflected, sizeof(TruckType), false);
    umock_c_reset_all_calls();
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

    STRICT_EXPECTED_CALL(Schema_GetModelName(TEST_MODEL_HANDLE)).SetReturn("TruckType");

    ///act
    EXECUTE_COMMAND_RESULT result = CodeFirst_InvokeAction(TEST_DEVICE_HANDLE, g_InvokeActionCallbackArgument, "", "moveCarTo", 3, parameters);

    ///assert
    ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_SUCCESS, result);
    ASSERT_IS_TRUE(moveCarTo_wasCalled);
    ASSERT_ARE_EQUAL(int, 2, moveCarTo_speed);
    ASSERT_ARE_EQUAL(int, 3, moveCarTo_destination_Alt);
    ASSERT_ARE_EQUAL(double, 4.0, moveCarTo_destination_whereIsMyCar_Lat);
    ASSERT_ARE_EQUAL(double, 5.0, moveCarTo_destination_whereIsMyCar_Long);
    ASSERT_IS_TRUE(moveCarTo_reverse);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    CodeFirst_DestroyDevice(device);
}

/*the below tests are simple to make it fail*/

/*Tests_SRS_SERIALIZER_99_039:[ WITH_ACTION shall declare an action of the current data provider called as the first macro parameter (name) and having the first parameter called parameter1Name of type parameter1Type, the second parameter named parameter2Name having the type parameter2Type and so on. ]*/
TEST_FUNCTION(InvokeAction_moveCarTo_with_wrong_number_of_parameters_fails_1)
{
    ///arrange
    void* device = CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &DummyDataProvider_WithStructs_allReflected, sizeof(TruckType), false);
    umock_c_reset_all_calls();
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

    STRICT_EXPECTED_CALL(Schema_GetModelName(TEST_MODEL_HANDLE)).SetReturn("TruckType");

    ///act
    EXECUTE_COMMAND_RESULT result = CodeFirst_InvokeAction(TEST_DEVICE_HANDLE, g_InvokeActionCallbackArgument, "", "moveCarTo", 2, parameters);

    ///assert
    ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
    ASSERT_IS_FALSE(moveCarTo_wasCalled);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    CodeFirst_DestroyDevice(device);
}

/*Tests_SRS_SERIALIZER_99_039:[ WITH_ACTION shall declare an action of the current data provider called as the first macro parameter (name) and having the first parameter called parameter1Name of type parameter1Type, the second parameter named parameter2Name having the type parameter2Type and so on. ]*/
TEST_FUNCTION(InvokeAction_moveCarTo_with_wrong_number_of_parameters_fails_2)
{
    ///arrange
    void* device = CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &DummyDataProvider_WithStructs_allReflected, sizeof(TruckType), false);
    umock_c_reset_all_calls();
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

    STRICT_EXPECTED_CALL(Schema_GetModelName(TEST_MODEL_HANDLE)).SetReturn("TruckType");

    ///act
   EXECUTE_COMMAND_RESULT result = CodeFirst_InvokeAction(TEST_DEVICE_HANDLE, g_InvokeActionCallbackArgument, "", "moveCarTo", 4, parameters);

    ///assert
   ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
    ASSERT_IS_FALSE(moveCarTo_wasCalled);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    CodeFirst_DestroyDevice(device);
}

/*Tests_SRS_SERIALIZER_99_039:[ WITH_ACTION shall declare an action of the current data provider called as the first macro parameter (name) and having the first parameter called parameter1Name of type parameter1Type, the second parameter named parameter2Name having the type parameter2Type and so on. ]*/
TEST_FUNCTION(InvokeAction_moveCarTo_with_wrong_type_of_parameter_1_fails)
{
    ///arrange
    void* device = CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &DummyDataProvider_WithStructs_allReflected, sizeof(TruckType), false);
    umock_c_reset_all_calls();
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

    STRICT_EXPECTED_CALL(Schema_GetModelName(TEST_MODEL_HANDLE)).SetReturn("TruckType");

    ///act
    EXECUTE_COMMAND_RESULT result = CodeFirst_InvokeAction(TEST_DEVICE_HANDLE, g_InvokeActionCallbackArgument, "", "moveCarTo", 3, parameters);

    ///assert
    ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
    ASSERT_IS_FALSE(moveCarTo_wasCalled);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    CodeFirst_DestroyDevice(device);
}

/*Tests_SRS_SERIALIZER_99_039:[ WITH_ACTION shall declare an action of the current data provider called as the first macro parameter (name) and having the first parameter called parameter1Name of type parameter1Type, the second parameter named parameter2Name having the type parameter2Type and so on. ]*/
TEST_FUNCTION(InvokeAction_moveCarTo_with_wrong_type_of_parameter_2_fails)
{
    ///arrange
    void* device = CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &DummyDataProvider_WithStructs_allReflected, sizeof(TruckType), false);
    umock_c_reset_all_calls();
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

    STRICT_EXPECTED_CALL(Schema_GetModelName(TEST_MODEL_HANDLE)).SetReturn("TruckType");

    ///act
    EXECUTE_COMMAND_RESULT result = CodeFirst_InvokeAction(TEST_DEVICE_HANDLE, g_InvokeActionCallbackArgument, "", "moveCarTo", 3, parameters);

    ///assert
    ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
    ASSERT_IS_FALSE(moveCarTo_wasCalled);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    CodeFirst_DestroyDevice(device);
}

/*Tests_SRS_SERIALIZER_99_039:[ WITH_ACTION shall declare an action of the current data provider called as the first macro parameter (name) and having the first parameter called parameter1Name of type parameter1Type, the second parameter named parameter2Name having the type parameter2Type and so on. ]*/
TEST_FUNCTION(InvokeAction_moveCarTo_with_wrong_type_for_destination_Alt_fails)
{
    ///arrange
    void* device = CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &DummyDataProvider_WithStructs_allReflected, sizeof(TruckType), false);
    umock_c_reset_all_calls();
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

    STRICT_EXPECTED_CALL(Schema_GetModelName(TEST_MODEL_HANDLE)).SetReturn("TruckType");

    ///act
    EXECUTE_COMMAND_RESULT result = CodeFirst_InvokeAction(TEST_DEVICE_HANDLE, g_InvokeActionCallbackArgument, "", "moveCarTo", 3, parameters);

    ///assert
    ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
    ASSERT_IS_FALSE(moveCarTo_wasCalled);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    CodeFirst_DestroyDevice(device);
}

/*Tests_SRS_SERIALIZER_99_039:[ WITH_ACTION shall declare an action of the current data provider called as the first macro parameter (name) and having the first parameter called parameter1Name of type parameter1Type, the second parameter named parameter2Name having the type parameter2Type and so on. ]*/
TEST_FUNCTION(InvokeAction_moveCarTo_with_wrong_type_for_destination_whereIsMyCar_fails)
{
    ///arrange
    void* device = CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &DummyDataProvider_WithStructs_allReflected, sizeof(TruckType), false);
    umock_c_reset_all_calls();
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

    STRICT_EXPECTED_CALL(Schema_GetModelName(TEST_MODEL_HANDLE)).SetReturn("TruckType");

    ///act
    EXECUTE_COMMAND_RESULT result = CodeFirst_InvokeAction(TEST_DEVICE_HANDLE, g_InvokeActionCallbackArgument, "", "moveCarTo", 3, parameters);

    ///assert
    ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
    ASSERT_IS_FALSE(moveCarTo_wasCalled);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    CodeFirst_DestroyDevice(device);
}

/*Tests_SRS_SERIALIZER_99_039:[ WITH_ACTION shall declare an action of the current data provider called as the first macro parameter (name) and having the first parameter called parameter1Name of type parameter1Type, the second parameter named parameter2Name having the type parameter2Type and so on. ]*/
TEST_FUNCTION(InvokeAction_moveCarTo_with_wrong_type_for_destination_whereIsMyCar_Lat_fails)
{
    ///arrange
    void* device = CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &DummyDataProvider_WithStructs_allReflected, sizeof(TruckType), false);
    umock_c_reset_all_calls();
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

    STRICT_EXPECTED_CALL(Schema_GetModelName(TEST_MODEL_HANDLE)).SetReturn("TruckType");

    ///act
    EXECUTE_COMMAND_RESULT result = CodeFirst_InvokeAction(TEST_DEVICE_HANDLE, g_InvokeActionCallbackArgument, "", "moveCarTo", 3, parameters);

    ///assert
    ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
    ASSERT_IS_FALSE(moveCarTo_wasCalled);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    CodeFirst_DestroyDevice(device);
}

/*Tests_SRS_SERIALIZER_99_039:[ WITH_ACTION shall declare an action of the current data provider called as the first macro parameter (name) and having the first parameter called parameter1Name of type parameter1Type, the second parameter named parameter2Name having the type parameter2Type and so on. ]*/
TEST_FUNCTION(InvokeAction_moveCarTo_with_wrong_type_for_destination_whereIsMyCar_Long_fails)
{
    ///arrange
    void* device = CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &DummyDataProvider_WithStructs_allReflected, sizeof(TruckType), false);
    umock_c_reset_all_calls();
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

    STRICT_EXPECTED_CALL(Schema_GetModelName(TEST_MODEL_HANDLE)).SetReturn("TruckType");

    ///act
    EXECUTE_COMMAND_RESULT result = CodeFirst_InvokeAction(TEST_DEVICE_HANDLE, g_InvokeActionCallbackArgument, "", "moveCarTo", 3, parameters);

    ///assert
    ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
    ASSERT_IS_FALSE(moveCarTo_wasCalled);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    CodeFirst_DestroyDevice(device);
}

/*Tests_SRS_SERIALIZER_99_039:[ WITH_ACTION shall declare an action of the current data provider called as the first macro parameter (name) and having the first parameter called parameter1Name of type parameter1Type, the second parameter named parameter2Name having the type parameter2Type and so on. ]*/
TEST_FUNCTION(InvokeAction_moveCarTo_with_wrong_type_for_reverse_fails)
{
    ///arrange
    void* device = CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &DummyDataProvider_WithStructs_allReflected, sizeof(TruckType), false);
    umock_c_reset_all_calls();
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

    STRICT_EXPECTED_CALL(Schema_GetModelName(TEST_MODEL_HANDLE)).SetReturn("TruckType");

    ///act
    EXECUTE_COMMAND_RESULT result = CodeFirst_InvokeAction(TEST_DEVICE_HANDLE, g_InvokeActionCallbackArgument, "", "moveCarTo", 3, parameters);

    ///assert
    ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
    ASSERT_IS_FALSE(moveCarTo_wasCalled);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    CodeFirst_DestroyDevice(device);
}

/*Tests_SRS_SERIALIZER_99_039:[ WITH_ACTION shall declare an action of the current data provider called as the first macro parameter (name) and having the first parameter called parameter1Name of type parameter1Type, the second parameter named parameter2Name having the type parameter2Type and so on. ]*/
TEST_FUNCTION(InvokeAction_moveCarTo_with_too_many_fields_in_CarLocation_fails)
{
    ///arrange
    void* device = CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &DummyDataProvider_WithStructs_allReflected, sizeof(TruckType), false);
    umock_c_reset_all_calls();
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

    STRICT_EXPECTED_CALL(Schema_GetModelName(TEST_MODEL_HANDLE)).SetReturn("TruckType");

    ///act
    EXECUTE_COMMAND_RESULT result = CodeFirst_InvokeAction(TEST_DEVICE_HANDLE, g_InvokeActionCallbackArgument, "", "moveCarTo", 3, parameters);

    ///assert
    ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
    ASSERT_IS_FALSE(moveCarTo_wasCalled);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    CodeFirst_DestroyDevice(device);
}

/*Tests_SRS_SERIALIZER_99_039:[ WITH_ACTION shall declare an action of the current data provider called as the first macro parameter (name) and having the first parameter called parameter1Name of type parameter1Type, the second parameter named parameter2Name having the type parameter2Type and so on. ]*/
TEST_FUNCTION(InvokeAction_moveCarTo_with_too_few_fields_in_CarLocation_fails)
{
    ///arrange
    void* device = CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &DummyDataProvider_WithStructs_allReflected, sizeof(TruckType), false);
    umock_c_reset_all_calls();
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

    STRICT_EXPECTED_CALL(Schema_GetModelName(TEST_MODEL_HANDLE)).SetReturn("TruckType");

    ///act
    EXECUTE_COMMAND_RESULT result = CodeFirst_InvokeAction(TEST_DEVICE_HANDLE, g_InvokeActionCallbackArgument, "", "moveCarTo", 3, parameters);

    ///assert
    ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
    ASSERT_IS_FALSE(moveCarTo_wasCalled);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    CodeFirst_DestroyDevice(device);
}

/*Tests_SRS_SERIALIZER_99_039:[ WITH_ACTION shall declare an action of the current data provider called as the first macro parameter (name) and having the first parameter called parameter1Name of type parameter1Type, the second parameter named parameter2Name having the type parameter2Type and so on. ]*/
TEST_FUNCTION(InvokeAction_moveCarTo_with_too_many_fields_in_CarLocation_whereIsMyCar_fails)
{
    ///arrange
    void* device = CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &DummyDataProvider_WithStructs_allReflected, sizeof(TruckType), false);
    umock_c_reset_all_calls();
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

    STRICT_EXPECTED_CALL(Schema_GetModelName(TEST_MODEL_HANDLE)).SetReturn("TruckType");

    ///act
    EXECUTE_COMMAND_RESULT result = CodeFirst_InvokeAction(TEST_DEVICE_HANDLE, g_InvokeActionCallbackArgument, "", "moveCarTo", 3, parameters);

    ///assert
    ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
    ASSERT_IS_FALSE(moveCarTo_wasCalled);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    CodeFirst_DestroyDevice(device);
}

/*Tests_SRS_SERIALIZER_99_039:[ WITH_ACTION shall declare an action of the current data provider called as the first macro parameter (name) and having the first parameter called parameter1Name of type parameter1Type, the second parameter named parameter2Name having the type parameter2Type and so on. ]*/
TEST_FUNCTION(InvokeAction_moveCarTo_with_too_few_fields_in_CarLocation_whereIsMyCar_fails)
{
    ///arrange
    void* device = CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &DummyDataProvider_WithStructs_allReflected, sizeof(TruckType), false);
    umock_c_reset_all_calls();
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

    STRICT_EXPECTED_CALL(Schema_GetModelName(TEST_MODEL_HANDLE)).SetReturn("TruckType");

    ///act
   EXECUTE_COMMAND_RESULT result = CodeFirst_InvokeAction(TEST_DEVICE_HANDLE, g_InvokeActionCallbackArgument, "", "moveCarTo", 3, parameters);

    ///assert
   ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
    ASSERT_IS_FALSE(moveCarTo_wasCalled);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    CodeFirst_DestroyDevice(device);
}

END_TEST_SUITE(CodeFirst_ut_Two_Providers_With_Structs);
