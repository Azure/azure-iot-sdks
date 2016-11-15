// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include <stddef.h>
#include <string.h>

static void* my_gballoc_malloc(size_t size)
{
    return malloc(size);
}

static void my_gballoc_free(void* s)
{
    free(s);
}

#include "parson.h"

#define ENABLE_MOCKS
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/crt_abstractions.h"
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
#ifdef __cplusplus
}
#endif

#undef ENABLE_MOCKS



#include "methodreturn.h"
#include "testrunnerswitcher.h"

#include "umock_c.h"
#include "umocktypes_charptr.h"
#include "umock_c_negative_tests.h"

/*helps when enums are not matched*/
#ifdef malloc
#undef malloc
#endif

static TEST_MUTEX_HANDLE g_dllByDll;
DEFINE_ENUM_STRINGS(UMOCK_C_ERROR_CODE, UMOCK_C_ERROR_CODE_VALUES)

static void on_umock_c_error(UMOCK_C_ERROR_CODE error_code)
{
    char temp_str[256];
    (void)snprintf(temp_str, sizeof(temp_str), "umock_c reported error :%s", ENUM_TO_STRING(UMOCK_C_ERROR_CODE, error_code));
    ASSERT_FAIL(temp_str);
}

BEGIN_TEST_SUITE(methodreturn_ut)

static JSON_Value* my_json_parse_string(const char * string)
{
    (void)string;
    return (JSON_Value*)my_gballoc_malloc(1);
}
static void my_json_value_free(JSON_Value* value)
{
    my_gballoc_free(value);
}

static int my_mallocAndStrcpy_s(char** destination, const char* source)
{
    *destination = (char*)my_gballoc_malloc(strlen(source) + 1);
    memcpy(*destination, source, strlen(source) + 1);
    return 0;
}

TEST_SUITE_INITIALIZE(TestSuiteInitialize)
{
    TEST_INITIALIZE_MEMORY_DEBUG(g_dllByDll);
    (void)umock_c_init(on_umock_c_error);

    (void)umocktypes_charptr_register_types();

    REGISTER_GLOBAL_MOCK_HOOK(json_parse_string, my_json_parse_string);
    REGISTER_GLOBAL_MOCK_HOOK(json_value_free, my_json_value_free);

    REGISTER_GLOBAL_MOCK_HOOK(gballoc_malloc, my_gballoc_malloc);
    REGISTER_GLOBAL_MOCK_HOOK(gballoc_free, my_gballoc_free);
    REGISTER_GLOBAL_MOCK_HOOK(mallocAndStrcpy_s, my_mallocAndStrcpy_s);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(mallocAndStrcpy_s, __LINE__);


}

TEST_SUITE_CLEANUP(TestClassCleanup)
{
    umock_c_deinit();
    TEST_DEINITIALIZE_MEMORY_DEBUG(g_dllByDll);
}

TEST_FUNCTION_INITIALIZE(Setup)
{
    umock_c_reset_all_calls();
}

static void MethodReturn_Create_with_non_NULL_jsonValue_inert_path(const char* jsonValue)
{
    STRICT_EXPECTED_CALL(json_parse_string(jsonValue));
    STRICT_EXPECTED_CALL(json_value_free(IGNORED_PTR_ARG))
        .IgnoreArgument_value();
    STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
        .IgnoreArgument_size();
    STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, jsonValue))
        .IgnoreArgument_destination();
}

/*Tests_SRS_METHODRETURN_02_001: [ MethodReturn_Create shall create a non-NULL handle containing statusCode and a clone of jsonValue. ]*/
TEST_FUNCTION(MethodReturn_Create_succeeds_with_non_NULL_jsonValue_happy_path)
{
    ///arrange
    const char* jsonValue = "1";
    
    MethodReturn_Create_with_non_NULL_jsonValue_inert_path(jsonValue);

    ///act
    METHODRETURN_HANDLE h = MethodReturn_Create(1, jsonValue);

    ///assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_IS_NOT_NULL(h);

    ///cleanup
    MethodReturn_Destroy(h);
}

/*Tests_SRS_METHODRETURN_02_009: [ If jsonValue is not a JSON value then MethodReturn_Create shall fail and return NULL. ]*/
/*Tests_SRS_METHODRETURN_02_002: [ If any failure is encountered then MethodReturn_Create shall return NULL ]*/
TEST_FUNCTION(MethodReturn_Create_succeeds_with_non_NULL_jsonValue_unhappy_paths)
{
    ///arrange
    const char* jsonValue = "1";
    umock_c_negative_tests_init();
    MethodReturn_Create_with_non_NULL_jsonValue_inert_path(jsonValue);
    umock_c_negative_tests_snapshot();

    for (size_t i = 0; i < umock_c_negative_tests_call_count(); i++)
    {

        if (
            (i != 1) /*json_value_free*/
            )
        {
            umock_c_negative_tests_reset();

            umock_c_negative_tests_fail_call(i);
            char temp_str[128];
            sprintf(temp_str, "On failed call %zu", i);

            ///act
            METHODRETURN_HANDLE h = MethodReturn_Create(1, jsonValue);

            ///assert
            ASSERT_IS_NULL_WITH_MSG(h, temp_str);
        }
    }

    ///cleanup
    umock_c_negative_tests_deinit();
}

static void MethodReturn_Create_with_NULL_jsonValue_inert_path(const char* jsonValue)
{
    (void)(jsonValue);
    STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
        .IgnoreArgument_size();
}

/*Tests_SRS_METHODRETURN_02_001: [ MethodReturn_Create shall create a non-NULL handle containing statusCode and a clone of jsonValue. ]*/
TEST_FUNCTION(MethodReturn_Create_succeeds_with_NULL_jsonValue_happy_path)
{
    ///arrange
    const char* jsonValue = NULL;

    MethodReturn_Create_with_NULL_jsonValue_inert_path(jsonValue);

    ///act
    METHODRETURN_HANDLE h = MethodReturn_Create(1, jsonValue);

    ///assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_IS_NOT_NULL(h);

    ///cleanup
    MethodReturn_Destroy(h);
}

/*Tests_SRS_METHODRETURN_02_009: [ If jsonValue is not a JSON value then MethodReturn_Create shall fail and return NULL. ]*/
/*Tests_SRS_METHODRETURN_02_002: [ If any failure is encountered then MethodReturn_Create shall return NULL ]*/
TEST_FUNCTION(MethodReturn_Create_with_NULL_jsonValue_unhappy_paths)
{
    ///arrange
    const char* jsonValue = NULL;
    umock_c_negative_tests_init();
    MethodReturn_Create_with_NULL_jsonValue_inert_path(jsonValue);
    umock_c_negative_tests_snapshot();

    for (size_t i = 0; i < umock_c_negative_tests_call_count(); i++)
    {
        
        umock_c_negative_tests_reset();

        umock_c_negative_tests_fail_call(i);
        char temp_str[128];
        sprintf(temp_str, "On failed call %zu", i);

        ///act
        METHODRETURN_HANDLE h = MethodReturn_Create(1, jsonValue);

        ///assert
        ASSERT_IS_NULL_WITH_MSG(h, temp_str);
        
    }

    ///cleanup
    umock_c_negative_tests_deinit();
}

/*Tests_SRS_METHODRETURN_02_003: [ If handle is NULL then MethodReturn_Destroy shall return. ]*/
TEST_FUNCTION(MethodReturn_Destroy_with_NULL_handle_returns)
{
    ///arrange

    ///act
    MethodReturn_Destroy(NULL);

    ///assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
}

/*Tests_SRS_METHODRETURN_02_004: [ Otherwise, MethodReturn_Destroy shall free all used resources by handle. ]*/
TEST_FUNCTION(MethodReturn_Destroy_with_non_NULL_handle_returns)
{
    ///arrange
    const char* jsonValue = "1";
    METHODRETURN_HANDLE h = MethodReturn_Create(1, jsonValue);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
        .IgnoreArgument_ptr();

    STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
        .IgnoreArgument_ptr();

    ///act
    MethodReturn_Destroy(h);

    ///assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    ///clean
    /*already clean*/
}

/*Tests_SRS_METHODRETURN_02_010: [ If handle is NULL then MethodReturn_GetReturn shall fail and return NULL. ]*/
TEST_FUNCTION(MethodReturn_GetReturn_with_NULL_handle_returns_NULL)
{
    ///arrange

    ///act
    const METHODRETURN_DATA* data = MethodReturn_GetReturn(NULL);

    ///assert
    ASSERT_IS_NULL(data);
}

/*Tests_SRS_METHODRETURN_02_011: [ Otherwise, MethodReturn_GetReturn shall return a non-NULL const pointer to a METHODRETURN_DATA. ]*/
TEST_FUNCTION(MethodReturn_GetReturn_with_non_NULL_handle_succeeds)
{
    ///arrange
    const char* jsonValue = "1";
    METHODRETURN_HANDLE h = MethodReturn_Create(1, jsonValue);
    umock_c_reset_all_calls();

    ///act
    const METHODRETURN_DATA* data = MethodReturn_GetReturn(h);

    ///assert
    ASSERT_IS_NOT_NULL(data);
    ASSERT_ARE_EQUAL(int, 1, data->statusCode);
    ASSERT_ARE_EQUAL(char_ptr, "1", data->jsonValue);

    ///clean
    MethodReturn_Destroy(h);
}

END_TEST_SUITE(methodreturn_ut);
