
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifdef __cplusplus
#include <cstdlib>
#else
#include <stdlib.h>
#endif 

static void* my_gballoc_malloc(size_t size)
{
    return malloc(size);
}

static void my_gballoc_free(void* s)
{
    free(s);
}

#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "real_vector.h"
#include "real_crt_abstractions.h"

#include "macro_utils.h"
#include "testrunnerswitcher.h"

#include "umock_c.h"
#include "umocktypes_charptr.h"
#include "umocktypes_bool.h"
#include "umocktypes_stdint.h"
#include "umock_c_negative_tests.h"

#include "parson.h"

/*don't want any of the below mocks, so preemtpively #include the header will prohibit the mocks from being created*/

#define ENABLE_MOCKS
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "schema.h"
#include "codefirst.h"
#undef ENABLE_MOCKS

#include "commanddecoder.h"

#define ENABLE_MOCKS
#include "iothub_client.h"
#include "iothub_client_ll.h"
#include "azure_c_shared_utility/vector.h"
#include "parson.h"
#ifdef __cplusplus
extern "C"
{
#endif
    MOCKABLE_FUNCTION(, JSON_Value*, json_parse_string, const char *, string);
    MOCKABLE_FUNCTION(, const char*, json_object_get_string, const JSON_Object *, object, const char *, name);
    MOCKABLE_FUNCTION(, JSON_Object*, json_value_get_object, const JSON_Value *, value);
    MOCKABLE_FUNCTION(, double, json_object_get_number, const JSON_Object*, object, const char*, name);
    MOCKABLE_FUNCTION(, char*, json_serialize_to_string, const JSON_Value*, value);
    MOCKABLE_FUNCTION(, void, json_free_serialized_string, char*, string);
    MOCKABLE_FUNCTION(, const char*, json_object_dotget_string, const JSON_Object*, object, const char*, name);
    MOCKABLE_FUNCTION(, JSON_Status, json_object_set_string, JSON_Object*, object, const char*, name, const char*, string);
    MOCKABLE_FUNCTION(, JSON_Status, json_object_dotset_string, JSON_Object*, object, const char*, name, const char*, string);
    MOCKABLE_FUNCTION(, JSON_Value*, json_value_init_object);
    MOCKABLE_FUNCTION(, JSON_Array*, json_array_get_array, const JSON_Array*, array, size_t, index);
    MOCKABLE_FUNCTION(, JSON_Object*, json_array_get_object, const JSON_Array*, array, size_t, index);
    MOCKABLE_FUNCTION(, JSON_Array*, json_value_get_array, const JSON_Value*, value);
    MOCKABLE_FUNCTION(, size_t, json_array_get_count, const JSON_Array*, array);
    MOCKABLE_FUNCTION(, JSON_Status, json_array_clear, JSON_Array*, array);
    MOCKABLE_FUNCTION(, JSON_Status, json_object_clear, JSON_Object*, object);
    MOCKABLE_FUNCTION(, void, json_value_free, JSON_Value *, value);
    MOCKABLE_FUNCTION(, char *, json_serialize_to_string_pretty, const JSON_Value *, value);
    MOCKABLE_FUNCTION(, JSON_Status, json_object_dotset_value, JSON_Object *, object, const char *, name, JSON_Value *, value);
    MOCKABLE_FUNCTION(, JSON_Object *, json_object, const JSON_Value *, value);
    MOCKABLE_FUNCTION(, JSON_Object *, json_object_get_object, const JSON_Object *, object, const char *, name);
    MOCKABLE_FUNCTION(, JSON_Status, json_object_remove, JSON_Object *, object, const char *, name);
    MOCKABLE_FUNCTION(, JSON_Value  *, json_object_get_value, const JSON_Object *, object, const char *, name);
#ifdef __cplusplus
}
#endif

#undef ENABLE_MOCKS

#include "serializer_devicetwin.h"

static TEST_MUTEX_HANDLE g_testByTest;
static TEST_MUTEX_HANDLE g_dllByDll;

#ifdef CPP_UNITTEST
/*apparently CppUniTest.h does not define the below which is needed for int64_t asserts*/
template <> static std::wstring Microsoft::VisualStudio::CppUnitTestFramework::ToString < int64_t >(const int64_t& q)
{
    std::wstring result;
    std::wostringstream o;
    o << q;
    return o.str();
}
#endif

TEST_DEFINE_ENUM_TYPE(SERIALIZER_RESULT, SERIALIZER_RESULT_VALUES);
TEST_DEFINE_ENUM_TYPE(CODEFIRST_RESULT, CODEFIRST_RESULT_VALUES);
TEST_DEFINE_ENUM_TYPE(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_RESULT_VALUES);
IMPLEMENT_UMOCK_C_ENUM_TYPE(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_RESULT_VALUES);

DEFINE_ENUM_STRINGS(UMOCK_C_ERROR_CODE, UMOCK_C_ERROR_CODE_VALUES)
static void on_umock_c_error(UMOCK_C_ERROR_CODE error_code)
{
    char temp_str[256];
    (void)snprintf(temp_str, sizeof(temp_str), "umock_c reported error :%s", ENUM_TO_STRING(UMOCK_C_ERROR_CODE, error_code));
    ASSERT_FAIL(temp_str);
}

BEGIN_NAMESPACE(basic15)

DECLARE_DEVICETWIN_MODEL(basicModel_WithData15,
    WITH_DESIRED_PROPERTY(double, with_desired_property_double15, on_desired_property_double15),
    WITH_DESIRED_PROPERTY(int, with_desired_property_int15, on_desired_property_int15),
    WITH_DESIRED_PROPERTY(float, with_desired_property_float15, on_desired_property_float15),
    WITH_DESIRED_PROPERTY(long, with_desired_property_long15, on_desired_property_long15),
    WITH_DESIRED_PROPERTY(int8_t, with_desired_property_sint8_t15, on_desired_property_sint8_t15),
    WITH_DESIRED_PROPERTY(uint8_t, with_desired_property_uint8_t15, on_desired_property_uint8_t15),
    WITH_DESIRED_PROPERTY(int16_t, with_desired_property_int16_t15, on_desired_property_int16_t15),
    WITH_DESIRED_PROPERTY(int32_t, with_desired_property_int32_t15, on_desired_property_int32_t15),
    WITH_DESIRED_PROPERTY(int64_t, with_desired_property_int64_t15, on_desired_property_int64_t15),
    WITH_DESIRED_PROPERTY(bool, with_desired_property_bool15, on_desired_property_bool15),
    WITH_DESIRED_PROPERTY(ascii_char_ptr, with_desired_property_ascii_char_ptr15, on_desired_property_ascii_char_ptr15),
    WITH_DESIRED_PROPERTY(ascii_char_ptr_no_quotes, with_desired_property_ascii_char_ptr_no_quotes15, on_desired_property_ascii_char_ptr_no_quotes15),
    WITH_DESIRED_PROPERTY(EDM_DATE_TIME_OFFSET, with_desired_property_EdmDateTimeOffset15, on_desired_property_EdmDateTimeOffset15),
    WITH_DESIRED_PROPERTY(EDM_GUID, with_desired_property_EdmGuid15, on_desired_property_EdmGuid15),
    WITH_DESIRED_PROPERTY(EDM_BINARY, with_desired_property_EdmBinary15, on_desired_property_EdmBinary15)
)
END_NAMESPACE(basic15)

#define ENABLE_MOCKS
#include "azure_c_shared_utility/umock_c_prod.h"
MOCKABLE_FUNCTION(, void, on_desired_property_double15, void*, v);
    MOCKABLE_FUNCTION(, void, on_desired_property_int15, void*, v);
    MOCKABLE_FUNCTION(, void, on_desired_property_float15, void*, v);
    MOCKABLE_FUNCTION(, void, on_desired_property_long15, void*, v);
    MOCKABLE_FUNCTION(, void, on_desired_property_sint8_t15, void*, v);
    MOCKABLE_FUNCTION(, void, on_desired_property_uint8_t15, void*, v);
    MOCKABLE_FUNCTION(, void, on_desired_property_int16_t15, void*, v);
    MOCKABLE_FUNCTION(, void, on_desired_property_int32_t15, void*, v);
    MOCKABLE_FUNCTION(, void, on_desired_property_int64_t15, void*, v);
    MOCKABLE_FUNCTION(, void, on_desired_property_bool15, void*, v);
    MOCKABLE_FUNCTION(, void, on_desired_property_ascii_char_ptr15, void*, v);
    MOCKABLE_FUNCTION(, void, on_desired_property_ascii_char_ptr_no_quotes15, void*, v);
    MOCKABLE_FUNCTION(, void, on_desired_property_EdmDateTimeOffset15, void*, v);
    MOCKABLE_FUNCTION(, void, on_desired_property_EdmGuid15, void*, v);
    MOCKABLE_FUNCTION(, void, on_desired_property_EdmBinary15, void*, v);
#undef ENABLE_MOCKS

#define TEST_IOTHUB_CLIENT_HANDLE ((IOTHUB_CLIENT_HANDLE)0x42)
#define TEST_IOTHUB_CLIENT_LL_HANDLE ((IOTHUB_CLIENT_LL_HANDLE)0x432)
#define TEST_SCHEMA_HANDLE ((SCHEMA_HANDLE)(0x55))
#define TEST_SCHEMA_MODEL_TYPE_HANDLE ((SCHEMA_MODEL_TYPE_HANDLE )(0x66))
#define TEST_DEVICE_HANDLE ((void*)0x64)
#define TEST_SERIALIZER_INGEST_CONTEXT ((void*)(0xAA))
#define TEST_JSON_PARSE_STRING ((JSON_Value *)0xAC)
#define TEST_JSON_VALUE_GET_OBJECT ((JSON_Object *)0xAD)
#define TEST_JSON_OBJECT_GET_OBJECT ((JSON_Object *)0XAE)
#define TEST_JSON_OBJECT_GET_VALUE ((JSON_Value *)0xEF)
#define TEST_JSON_SERIALIZE_TO_STRING ((char*)("a"))

static IOTHUB_CLIENT_DEVICE_TWIN_CALLBACK g_SerializerIngest_fp = NULL;
static void* g_SerializerIngest_userContextCallback = NULL;

static IOTHUB_CLIENT_RESULT my_IoTHubClient_SetDeviceTwinCallback(IOTHUB_CLIENT_HANDLE iotHubClientHandle, IOTHUB_CLIENT_DEVICE_TWIN_CALLBACK deviceTwinCallback, void* userContextCallback)
{
    (void)(iotHubClientHandle);
    g_SerializerIngest_fp = deviceTwinCallback;
    g_SerializerIngest_userContextCallback = userContextCallback;
    return IOTHUB_CLIENT_OK;
}

static IOTHUB_CLIENT_RESULT my_IoTHubClient_LL_SetDeviceTwinCallback(IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle, IOTHUB_CLIENT_DEVICE_TWIN_CALLBACK deviceTwinCallback, void* userContextCallback)
{
    (void)(iotHubClientHandle);
    g_SerializerIngest_fp = deviceTwinCallback;
    g_SerializerIngest_userContextCallback = userContextCallback;
    return IOTHUB_CLIENT_OK;
}

static char* my_json_serialize_to_string(const JSON_Value *value)
{
    (void)value;
    char* temp = (char*)my_gballoc_malloc(2);
    temp[0] = '3';
    temp[1] = '\0';
    return temp;
}

BEGIN_TEST_SUITE(serializer_dt_ut)

    TEST_SUITE_INITIALIZE(TestClassInitialize)
    {
        TEST_INITIALIZE_MEMORY_DEBUG(g_dllByDll);
        g_testByTest = TEST_MUTEX_CREATE();
        ASSERT_IS_NOT_NULL(g_testByTest);

        (void)umock_c_init(on_umock_c_error);

        (void)umocktypes_bool_register_types();
        (void)umocktypes_charptr_register_types();
        (void)umocktypes_stdint_register_types();

        REGISTER_TYPE(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_RESULT);

        REGISTER_UMOCK_ALIAS_TYPE(IOTHUB_CLIENT_HANDLE, void*);
        REGISTER_UMOCK_ALIAS_TYPE(IOTHUB_CLIENT_LL_HANDLE, void*);
        REGISTER_UMOCK_ALIAS_TYPE(SCHEMA_HANDLE, void*);
        REGISTER_UMOCK_ALIAS_TYPE(SCHEMA_MODEL_TYPE_HANDLE, void*);
        REGISTER_UMOCK_ALIAS_TYPE(VECTOR_HANDLE, void*);
        REGISTER_UMOCK_ALIAS_TYPE(PREDICATE_FUNCTION, void*);
        REGISTER_UMOCK_ALIAS_TYPE(const VECTOR_HANDLE, void*);
        
        
        REGISTER_UMOCK_ALIAS_TYPE(IOTHUB_CLIENT_DEVICE_TWIN_CALLBACK, void*);
        
        REGISTER_GLOBAL_MOCK_HOOK(IoTHubClient_SetDeviceTwinCallback, my_IoTHubClient_SetDeviceTwinCallback);
        REGISTER_GLOBAL_MOCK_HOOK(IoTHubClient_LL_SetDeviceTwinCallback, my_IoTHubClient_LL_SetDeviceTwinCallback);

        REGISTER_GLOBAL_MOCK_RETURNS(Schema_GetSchemaForModel, TEST_SCHEMA_HANDLE, NULL);
        REGISTER_GLOBAL_MOCK_RETURN(Schema_GetMetadata, (void*)&ALL_REFLECTED(basic15));
        REGISTER_GLOBAL_MOCK_RETURNS(Schema_GetModelByName, TEST_SCHEMA_MODEL_TYPE_HANDLE, NULL);
        REGISTER_GLOBAL_MOCK_RETURNS(CodeFirst_CreateDevice, TEST_DEVICE_HANDLE, NULL);
        REGISTER_GLOBAL_MOCK_RETURNS(CodeFirst_IngestDesiredProperties, CODEFIRST_OK, CODEFIRST_ERROR);
        REGISTER_GLOBAL_MOCK_RETURNS(IoTHubClient_SetDeviceTwinCallback, IOTHUB_CLIENT_OK, IOTHUB_CLIENT_ERROR);
        REGISTER_GLOBAL_MOCK_RETURNS(IoTHubClient_LL_SetDeviceTwinCallback, IOTHUB_CLIENT_OK, IOTHUB_CLIENT_ERROR);

        REGISTER_GLOBAL_MOCK_HOOK(VECTOR_create, real_VECTOR_create);
        REGISTER_GLOBAL_MOCK_HOOK(VECTOR_destroy, real_VECTOR_destroy);
        REGISTER_GLOBAL_MOCK_HOOK(VECTOR_push_back, real_VECTOR_push_back);
        REGISTER_GLOBAL_MOCK_FAIL_RETURN(VECTOR_push_back, __LINE__);
        REGISTER_GLOBAL_MOCK_HOOK(VECTOR_erase, real_VECTOR_erase);
        REGISTER_GLOBAL_MOCK_HOOK(VECTOR_clear, real_VECTOR_clear);
        REGISTER_GLOBAL_MOCK_HOOK(VECTOR_element, real_VECTOR_element);
        REGISTER_GLOBAL_MOCK_FAIL_RETURN(VECTOR_element, NULL);
        REGISTER_GLOBAL_MOCK_HOOK(VECTOR_front, real_VECTOR_front);
        REGISTER_GLOBAL_MOCK_HOOK(VECTOR_back, real_VECTOR_back);
        REGISTER_GLOBAL_MOCK_HOOK(VECTOR_find_if, real_VECTOR_find_if);
        REGISTER_GLOBAL_MOCK_HOOK(VECTOR_size, real_VECTOR_size);

        REGISTER_GLOBAL_MOCK_HOOK(mallocAndStrcpy_s, real_mallocAndStrcpy_s);
        REGISTER_GLOBAL_MOCK_FAIL_RETURN(mallocAndStrcpy_s, __LINE__);
        REGISTER_GLOBAL_MOCK_HOOK(unsignedIntToString, real_unsignedIntToString);
        REGISTER_GLOBAL_MOCK_HOOK(size_tToString, real_size_tToString);

        REGISTER_GLOBAL_MOCK_HOOK(gballoc_malloc, my_gballoc_malloc);
        REGISTER_GLOBAL_MOCK_FAIL_RETURN(gballoc_malloc, NULL);
        REGISTER_GLOBAL_MOCK_HOOK(gballoc_free, my_gballoc_free);

        REGISTER_GLOBAL_MOCK_RETURNS(json_parse_string, TEST_JSON_PARSE_STRING, NULL);
        REGISTER_GLOBAL_MOCK_RETURNS(json_value_get_object, TEST_JSON_VALUE_GET_OBJECT, NULL);
        REGISTER_GLOBAL_MOCK_RETURNS(json_object_get_object, TEST_JSON_OBJECT_GET_OBJECT, NULL);
        REGISTER_GLOBAL_MOCK_RETURNS(json_object_get_value, TEST_JSON_OBJECT_GET_VALUE, NULL);
        REGISTER_GLOBAL_MOCK_FAIL_RETURN(json_serialize_to_string, NULL);
        REGISTER_GLOBAL_MOCK_HOOK(json_serialize_to_string, my_json_serialize_to_string);
        

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

        g_SerializerIngest_fp = NULL;
        g_SerializerIngest_userContextCallback = NULL;

    }

    TEST_FUNCTION_CLEANUP(TestMethodCleanup)
    {
        TEST_MUTEX_RELEASE(g_testByTest);
    }
   
    static void IoTHubDeviceTwin_CreatebasicModel_WithData15_inertPath(void)
    {
        STRICT_EXPECTED_CALL(Schema_GetSchemaForModel("basicModel_WithData15"));
        STRICT_EXPECTED_CALL(Schema_GetMetadata(TEST_SCHEMA_HANDLE));
        STRICT_EXPECTED_CALL(Schema_GetModelByName(TEST_SCHEMA_HANDLE, "basicModel_WithData15"));
        STRICT_EXPECTED_CALL(CodeFirst_CreateDevice(TEST_SCHEMA_MODEL_TYPE_HANDLE, &ALL_REFLECTED(basic15), sizeof(basicModel_WithData15), true));
        STRICT_EXPECTED_CALL(IoTHubClient_SetDeviceTwinCallback(TEST_IOTHUB_CLIENT_HANDLE, serializer_ingest, TEST_DEVICE_HANDLE));
        STRICT_EXPECTED_CALL(VECTOR_create(sizeof(SERIALIZER_DEVICETWIN_PROTOHANDLE)));
        STRICT_EXPECTED_CALL(VECTOR_push_back(IGNORED_PTR_ARG, IGNORED_PTR_ARG, 1))
            .IgnoreArgument_handle()
            .IgnoreArgument_elements();
    }

    /*this test wants to see that IoTHubDeviceTwin_CreatebasicModel_WithData15 doesn't fail*/
    /*Tests_SRS_SERIALIZERDEVICETWIN_02_009: [ IoTHubDeviceTwinCreate_Impl shall locate the model and the metadata for name by calling Schema_GetSchemaForModel/Schema_GetMetadata/Schema_GetModelByName. ]*/
    /*Tests_SRS_SERIALIZERDEVICETWIN_02_010: [ IoTHubDeviceTwinCreate_Impl shall call CodeFirst_CreateDevice. ]*/
    /*Tests_SRS_SERIALIZERDEVICETWIN_02_011: [ IoTHubDeviceTwinCreate_Impl shall set the device twin callback. ]*/
    /*Tests_SRS_SERIALIZERDEVICETWIN_02_012: [ IoTHubDeviceTwinCreate_Impl shall record the pair of (device, IoTHubClient(_LL)). ]*/
    /*Tests_SRS_SERIALIZERDEVICETWIN_02_013: [ If all operations complete successfully then IoTHubDeviceTwinCreate_Impl shall succeeds and return a non-NULL value. ]*/
    TEST_FUNCTION(IoTHubDeviceTwin_CreatebasicModel_WithData15_happy_path)
    {
        ///arrange
        (void)SERIALIZER_REGISTER_NAMESPACE(basic15);

        umock_c_reset_all_calls();

        IoTHubDeviceTwin_CreatebasicModel_WithData15_inertPath();
            
        ///act
        basicModel_WithData15* model = IoTHubDeviceTwin_CreatebasicModel_WithData15(TEST_IOTHUB_CLIENT_HANDLE);

        ///assert
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
        ASSERT_IS_NOT_NULL(model);

        ///clean
        IoTHubDeviceTwin_DestroybasicModel_WithData15(model);
        
    }

    /*this test wants to see that IoTHubDeviceTwin_CreatebasicModel_WithData15 doesn't fail*/
    /*Tests_SRS_SERIALIZERDEVICETWIN_02_014: [ Otherwise, IoTHubDeviceTwinCreate_Impl shall fail and return NULL. ]*/
    TEST_FUNCTION(IoTHubDeviceTwin_CreatebasicModel_WithData15_unhappy_paths)
    {
        ///arrange
        (void)SERIALIZER_REGISTER_NAMESPACE(basic15);

        umock_c_reset_all_calls();
        umock_c_negative_tests_init();

        IoTHubDeviceTwin_CreatebasicModel_WithData15_inertPath();

        umock_c_negative_tests_snapshot();

        for (size_t i = 0; i < umock_c_negative_tests_call_count(); i++)
        {
            umock_c_negative_tests_reset();
            umock_c_negative_tests_fail_call(i);

            
            if (
                (i != 1) /*Schema_GetMetadata*/
                )
            {
                /// act
                 basicModel_WithData15* model = IoTHubDeviceTwin_CreatebasicModel_WithData15(TEST_IOTHUB_CLIENT_HANDLE);

                /// assert
                ASSERT_IS_NULL(model);
            }
        }

        ///clean
        umock_c_negative_tests_deinit();
    }

    static void IoTHubDeviceTwin_LL_CreatebasicModel_WithData15_inertPath(void)
    {
        STRICT_EXPECTED_CALL(Schema_GetSchemaForModel("basicModel_WithData15"));
        STRICT_EXPECTED_CALL(Schema_GetMetadata(TEST_SCHEMA_HANDLE));
        STRICT_EXPECTED_CALL(Schema_GetModelByName(TEST_SCHEMA_HANDLE, "basicModel_WithData15"));
        STRICT_EXPECTED_CALL(CodeFirst_CreateDevice(TEST_SCHEMA_MODEL_TYPE_HANDLE, &ALL_REFLECTED(basic15), sizeof(basicModel_WithData15), true));
        STRICT_EXPECTED_CALL(IoTHubClient_LL_SetDeviceTwinCallback(TEST_IOTHUB_CLIENT_LL_HANDLE, serializer_ingest, TEST_DEVICE_HANDLE));
        STRICT_EXPECTED_CALL(VECTOR_create(sizeof(SERIALIZER_DEVICETWIN_PROTOHANDLE)));
        STRICT_EXPECTED_CALL(VECTOR_push_back(IGNORED_PTR_ARG, IGNORED_PTR_ARG, 1))
            .IgnoreArgument_handle()
            .IgnoreArgument_elements();
    }

    /*this test wants to see that IoTHubDeviceTwin_CreatebasicModel_WithData15 doesn't fail*/
    /*Tests_SRS_SERIALIZERDEVICETWIN_02_009: [ IoTHubDeviceTwinCreate_Impl shall locate the model and the metadata for name by calling Schema_GetSchemaForModel/Schema_GetMetadata/Schema_GetModelByName. ]*/
    /*Tests_SRS_SERIALIZERDEVICETWIN_02_010: [ IoTHubDeviceTwinCreate_Impl shall call CodeFirst_CreateDevice. ]*/
    /*Tests_SRS_SERIALIZERDEVICETWIN_02_011: [ IoTHubDeviceTwinCreate_Impl shall set the device twin callback. ]*/
    /*Tests_SRS_SERIALIZERDEVICETWIN_02_012: [ IoTHubDeviceTwinCreate_Impl shall record the pair of (device, IoTHubClient(_LL)). ]*/
    /*Tests_SRS_SERIALIZERDEVICETWIN_02_013: [ If all operations complete successfully then IoTHubDeviceTwinCreate_Impl shall succeeds and return a non-NULL value. ]*/
    TEST_FUNCTION(IoTHubDeviceTwin_LL_CreatebasicModel_WithData15_happy_path)
    {
        ///arrange
        (void)SERIALIZER_REGISTER_NAMESPACE(basic15);

        umock_c_reset_all_calls();

        IoTHubDeviceTwin_LL_CreatebasicModel_WithData15_inertPath();

        ///act
        basicModel_WithData15* model = IoTHubDeviceTwin_LL_CreatebasicModel_WithData15(TEST_IOTHUB_CLIENT_LL_HANDLE);

        ///assert
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
        ASSERT_IS_NOT_NULL(model);

        ///clean
        IoTHubDeviceTwin_LL_DestroybasicModel_WithData15(model);

    }

    /*this test wants to see that IoTHubDeviceTwin_CreatebasicModel_WithData15 doesn't fail*/
    /*Tests_SRS_SERIALIZERDEVICETWIN_02_014: [ Otherwise, IoTHubDeviceTwinCreate_Impl shall fail and return NULL. ]*/
    TEST_FUNCTION(IoTHubDeviceTwin_LL_CreatebasicModel_WithData15_unhappy_paths)
    {
        ///arrange
        (void)SERIALIZER_REGISTER_NAMESPACE(basic15);

        umock_c_reset_all_calls();
        umock_c_negative_tests_init();

        IoTHubDeviceTwin_LL_CreatebasicModel_WithData15_inertPath();

        umock_c_negative_tests_snapshot();

        for (size_t i = 0; i < umock_c_negative_tests_call_count(); i++)
        {
            umock_c_negative_tests_reset();
            umock_c_negative_tests_fail_call(i);

            if (
                (i != 1) /*Schema_GetMetadata*/
                )
            {
                /// act
                basicModel_WithData15* model = IoTHubDeviceTwin_LL_CreatebasicModel_WithData15(TEST_IOTHUB_CLIENT_LL_HANDLE);

                /// assert
                ASSERT_IS_NULL(model);
            }
        }

        ///clean
        umock_c_negative_tests_deinit();
    }

    void serializer_ingest_DEVICE_TWIN_UPDATE_COMPLETE_inert_path(size_t payloadSize)
    {
        STRICT_EXPECTED_CALL(gballoc_malloc(payloadSize + 1));
        STRICT_EXPECTED_CALL(json_parse_string(IGNORED_PTR_ARG))
            .IgnoreArgument_string();
        STRICT_EXPECTED_CALL(json_value_get_object(IGNORED_PTR_ARG))
            .IgnoreArgument_value();
        STRICT_EXPECTED_CALL(json_object_get_object(IGNORED_PTR_ARG, "desired"))
            .IgnoreArgument_object();
        STRICT_EXPECTED_CALL(json_object_remove(IGNORED_PTR_ARG, "$version"))
            .IgnoreArgument_object();
        STRICT_EXPECTED_CALL(json_object_get_value(IGNORED_PTR_ARG, "desired"))
            .IgnoreArgument_object();
        STRICT_EXPECTED_CALL(json_serialize_to_string(IGNORED_PTR_ARG))
            .IgnoreArgument_value();
        STRICT_EXPECTED_CALL(CodeFirst_IngestDesiredProperties(TEST_SERIALIZER_INGEST_CONTEXT, IGNORED_PTR_ARG))
            .IgnoreArgument_desiredProperties();
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument_ptr();
        STRICT_EXPECTED_CALL(json_value_free(IGNORED_PTR_ARG))
            .IgnoreArgument_value();
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument_ptr();
    }

    /*Tests_SRS_SERIALIZERDEVICETWIN_02_001: [ serializer_ingest shall clone the payload into a null terminated string. ]*/
    /*Tests_SRS_SERIALIZERDEVICETWIN_02_002: [ serializer_ingest shall parse the null terminated string into parson data types. ]*/
    /*Tests_SRS_SERIALIZERDEVICETWIN_02_003: [ If update_state is DEVICE_TWIN_UPDATE_COMPLETE then serializer_ingest shall locate "desired" json name. ]*/
    /*Tests_SRS_SERIALIZERDEVICETWIN_02_004: [ If "desired" contains "$version" then serializer_ingest shall remove it. ]*/
    /*Tests_SRS_SERIALIZERDEVICETWIN_02_005: [ The "desired" value shall be outputed to a null terminated string and serializer_ingest shall call CodeFirst_IngestDesiredProperties. ]*/
    TEST_FUNCTION(serializer_ingest_DEVICE_TWIN_UPDATE_COMPLETE_happy_path)
    {
        ///arrange
        unsigned char payload = (unsigned char)'p';
        size_t payloadSize = 1;

        serializer_ingest_DEVICE_TWIN_UPDATE_COMPLETE_inert_path(payloadSize);

        ///act
        serializer_ingest(DEVICE_TWIN_UPDATE_COMPLETE, &payload, payloadSize, TEST_SERIALIZER_INGEST_CONTEXT);

        ///assert
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///clean
    }

    /*Tests_SRS_SERIALIZERDEVICETWIN_02_008: [ If any of the above operations fail, then serializer_ingest shall return. ]*/
    TEST_FUNCTION(serializer_ingest_DEVICE_TWIN_UPDATE_COMPLETE_unhappy_path)
    {
        ///arrange
        unsigned char payload = (unsigned char)'p';
        size_t payloadSize = 1;

        umock_c_negative_tests_init();

        serializer_ingest_DEVICE_TWIN_UPDATE_COMPLETE_inert_path(payloadSize);

        umock_c_negative_tests_snapshot();

        for (size_t i = 0; i < umock_c_negative_tests_call_count(); i++)
        {
            umock_c_negative_tests_reset();
            umock_c_negative_tests_fail_call(i);

            if (
                (i != 8) && /*gballoc_free*/
                (i != 9) && /*json_value_free*/
                (i != 10)  /*gballoc_free*/
                )
            {
                /// act
                serializer_ingest(DEVICE_TWIN_UPDATE_COMPLETE, &payload, payloadSize, TEST_SERIALIZER_INGEST_CONTEXT);

                ///assert
                /// not much to assert, because serializer_ingest does not return - tests still ahve value tracking memory allocations etc
            }
        }

        ///clean
        umock_c_negative_tests_deinit();
    }

    void serializer_ingest_DEVICE_TWIN_UPDATE_PARTIAL_inert_path(size_t payloadSize)
    {
        STRICT_EXPECTED_CALL(gballoc_malloc(payloadSize + 1));
        STRICT_EXPECTED_CALL(json_parse_string(IGNORED_PTR_ARG))
            .IgnoreArgument_string();
        STRICT_EXPECTED_CALL(json_value_get_object(IGNORED_PTR_ARG))
            .IgnoreArgument_value();
        STRICT_EXPECTED_CALL(json_object_remove(IGNORED_PTR_ARG, "$version"))
            .IgnoreArgument_object();
        STRICT_EXPECTED_CALL(json_serialize_to_string(IGNORED_PTR_ARG))
            .IgnoreArgument_value();
        STRICT_EXPECTED_CALL(CodeFirst_IngestDesiredProperties(TEST_SERIALIZER_INGEST_CONTEXT, IGNORED_PTR_ARG))
            .IgnoreArgument_desiredProperties();
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument_ptr();
        STRICT_EXPECTED_CALL(json_value_free(IGNORED_PTR_ARG))
            .IgnoreArgument_value();
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument_ptr();
    }

    /*Tests_SRS_SERIALIZERDEVICETWIN_02_001: [ serializer_ingest shall clone the payload into a null terminated string. ]*/
    /*Tests_SRS_SERIALIZERDEVICETWIN_02_002: [ serializer_ingest shall parse the null terminated string into parson data types. ]*/
    /*Tests_SRS_SERIALIZERDEVICETWIN_02_006: [ If update_state is DEVICE_TWIN_UPDATE_PARTIAL then serializer_ingest shall remove "$version" (if it exists). ]*/
    /*Tests_SRS_SERIALIZERDEVICETWIN_02_007: [ The JSON shall be outputed to a null terminated string and serializer_ingest shall call CodeFirst_IngestDesiredProperties. ]*/
    TEST_FUNCTION(serializer_ingest_DEVICE_TWIN_UPDATE_PARTIAL_happy_path)
    {
        ///arrange
        unsigned char payload = (unsigned char)'p';
        size_t payloadSize = 1;

        serializer_ingest_DEVICE_TWIN_UPDATE_PARTIAL_inert_path(payloadSize);

        ///act
        serializer_ingest(DEVICE_TWIN_UPDATE_PARTIAL, &payload, payloadSize, TEST_SERIALIZER_INGEST_CONTEXT);

        ///assert
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///clean
    }

    /*Tests_SRS_SERIALIZERDEVICETWIN_02_008: [ If any of the above operations fail, then serializer_ingest shall return. ]*/
    TEST_FUNCTION(serializer_ingest_DEVICE_TWIN_UPDATE_PARTIAL_unhappy_path)
    {
        ///arrange
        unsigned char payload = (unsigned char)'p';
        size_t payloadSize = 1;

        umock_c_negative_tests_init();

        serializer_ingest_DEVICE_TWIN_UPDATE_PARTIAL_inert_path(payloadSize);

        umock_c_negative_tests_snapshot();

        for (size_t i = 0; i < umock_c_negative_tests_call_count(); i++)
        {
            umock_c_negative_tests_reset();
            umock_c_negative_tests_fail_call(i);

            if (
                (i != 8) && /*gballoc_free*/
                (i != 9) && /*json_value_free*/
                (i != 10)  /*gballoc_free*/
                )
            {
                /// act
                serializer_ingest(DEVICE_TWIN_UPDATE_PARTIAL, &payload, payloadSize, TEST_SERIALIZER_INGEST_CONTEXT);

                ///assert
                /// not much to assert, because serializer_ingest does not return - tests still ahve value tracking memory allocations etc
            }
        }

        ///clean
        umock_c_negative_tests_deinit();
    }
    
    /*Tests_SRS_SERIALIZERDEVICETWIN_02_020: [ If model is NULL then IoTHubDeviceTwin_Destroy_Impl shall return. ]*/
    TEST_FUNCTION(IoTHubDeviceTwin_Destroy_Impl_with_NULL_model_returns)
    {
        ///arrange

        ///act
        IoTHubDeviceTwin_Destroy_Impl(NULL);

        ///assert
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
    }

    /*Tests_SRS_SERIALIZERDEVICETWIN_02_015: [ IoTHubDeviceTwin_Destroy_Impl shall locate the saved handle belonging to model. ]*/
    /*Tests_SRS_SERIALIZERDEVICETWIN_02_016: [ IoTHubDeviceTwin_Destroy_Impl shall set the devicetwin callback to NULL. ]*/
    /*Tests_SRS_SERIALIZERDEVICETWIN_02_017: [ IoTHubDeviceTwin_Destroy_Impl shall call CodeFirst_DestroyDevice. ]*/
    /*Tests_SRS_SERIALIZERDEVICETWIN_02_018: [ IoTHubDeviceTwin_Destroy_Impl shall remove the IoTHubClient_Handle and the device handle from the recorded set. ]*/
    /*Tests_SRS_SERIALIZERDEVICETWIN_02_019: [ If the recorded set of IoTHubClient handles is zero size, then the set shall be destroyed. ]*/
    TEST_FUNCTION(IoTHubDeviceTwin_LL_Destroy_Impl_returns)
    {
        ///arrange
        (void)SERIALIZER_REGISTER_NAMESPACE(basic15);
        IoTHubDeviceTwin_LL_CreatebasicModel_WithData15_inertPath();
        basicModel_WithData15* model = IoTHubDeviceTwin_LL_CreatebasicModel_WithData15(TEST_IOTHUB_CLIENT_LL_HANDLE);
        umock_c_reset_all_calls();

        STRICT_EXPECTED_CALL(VECTOR_find_if(g_allProtoHandles, protoHandleHasDeviceStartAddress, model));
        STRICT_EXPECTED_CALL(IoTHubClient_LL_SetDeviceTwinCallback(TEST_IOTHUB_CLIENT_LL_HANDLE, NULL, NULL));
        STRICT_EXPECTED_CALL(CodeFirst_DestroyDevice(model));
        STRICT_EXPECTED_CALL(VECTOR_erase(g_allProtoHandles, IGNORED_PTR_ARG, 1))
            .IgnoreArgument_elements();
        STRICT_EXPECTED_CALL(VECTOR_size(g_allProtoHandles));
        STRICT_EXPECTED_CALL(VECTOR_destroy(g_allProtoHandles));

        ///act
        IoTHubDeviceTwin_LL_DestroybasicModel_WithData15(model);

        ///assert
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///clean

    }

    /*Tests_SRS_SERIALIZERDEVICETWIN_02_015: [ IoTHubDeviceTwin_Destroy_Impl shall locate the saved handle belonging to model. ]*/
    /*Tests_SRS_SERIALIZERDEVICETWIN_02_016: [ IoTHubDeviceTwin_Destroy_Impl shall set the devicetwin callback to NULL. ]*/
    /*Tests_SRS_SERIALIZERDEVICETWIN_02_017: [ IoTHubDeviceTwin_Destroy_Impl shall call CodeFirst_DestroyDevice. ]*/
    /*Tests_SRS_SERIALIZERDEVICETWIN_02_018: [ IoTHubDeviceTwin_Destroy_Impl shall remove the IoTHubClient_Handle and the device handle from the recorded set. ]*/
    /*Tests_SRS_SERIALIZERDEVICETWIN_02_019: [ If the recorded set of IoTHubClient handles is zero size, then the set shall be destroyed. ]*/
    TEST_FUNCTION(IoTHubDeviceTwin_Destroy_Impl_returns)
    {
        ///arrange
        (void)SERIALIZER_REGISTER_NAMESPACE(basic15);
        IoTHubDeviceTwin_CreatebasicModel_WithData15_inertPath();
        basicModel_WithData15* model = IoTHubDeviceTwin_CreatebasicModel_WithData15(TEST_IOTHUB_CLIENT_HANDLE);
        umock_c_reset_all_calls();

        STRICT_EXPECTED_CALL(VECTOR_find_if(g_allProtoHandles, protoHandleHasDeviceStartAddress, model));
        STRICT_EXPECTED_CALL(IoTHubClient_SetDeviceTwinCallback(TEST_IOTHUB_CLIENT_HANDLE, NULL, NULL));
        STRICT_EXPECTED_CALL(CodeFirst_DestroyDevice(model));
        STRICT_EXPECTED_CALL(VECTOR_erase(g_allProtoHandles, IGNORED_PTR_ARG, 1))
            .IgnoreArgument_elements();
        STRICT_EXPECTED_CALL(VECTOR_size(g_allProtoHandles));
        STRICT_EXPECTED_CALL(VECTOR_destroy(g_allProtoHandles));

        ///act
        IoTHubDeviceTwin_LL_DestroybasicModel_WithData15(model);

        ///assert
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///clean

    }
    
END_TEST_SUITE(serializer_dt_ut)
