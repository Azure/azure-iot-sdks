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

#include "real_parson.h"
#include "real_vector.h"
#include "real_crt_abstractions.h"
#include "real_strings.h"

#include "umock_c.h"
#include "umocktypes_charptr.h"
#include "umocktypes_bool.h"
#include "umocktypes_stdint.h"
#include "umock_c_negative_tests.h"

#define ENABLE_MOCKS
#include "jsonencoder.h"
#include "multitree.h"
#include "schema.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/strings.h"
#include "azure_c_shared_utility/vector.h"
#include "agenttypesystem.h"
#include "parson.h"
#include "azure_c_shared_utility/gballoc.h"
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


#include "datamarshaller.h"
#include "testrunnerswitcher.h"
#include "umock_c.h"
#include "umocktypes_charptr.h"
#include "umock_c_negative_tests.h"

DEFINE_ENUM_STRINGS(UMOCK_C_ERROR_CODE, UMOCK_C_ERROR_CODE_VALUES)

static TEST_MUTEX_HANDLE g_testByTest;
static TEST_MUTEX_HANDLE g_dllByDll;

static AGENT_DATA_TYPE floatValid;
static AGENT_DATA_TYPE intValid;
static AGENT_DATA_TYPE structTypeValue;
static AGENT_DATA_TYPE structTypeValue2Members;

static COMPLEX_TYPE_FIELD_TYPE members = { "x", &floatValid };
static COMPLEX_TYPE_FIELD_TYPE two_members[] = { { "x", &floatValid },{ "y", &intValid } };

static const SCHEMA_MODEL_TYPE_HANDLE TEST_MODEL_HANDLE = (SCHEMA_MODEL_TYPE_HANDLE)0x4242;
#define DEFAULT_PROPERTY_NAME "defaultPropertyName"
#define DEFAULT_PROPERTY_NAME_SLASHES_REPLACED_BY_DOTS "defaultPropertyName"

#define DEFAULT_PROPERTY_NAME_LEVEL2 "a/b"
#define DEFAULT_PROPERTY_NAME_LEVEL2_SLASHES_REPLACED_BY_DOTS "a.b"



TEST_DEFINE_ENUM_TYPE(DATA_MARSHALLER_RESULT, DATA_MARSHALLER_RESULT_VALUES);
IMPLEMENT_UMOCK_C_ENUM_TYPE(DATA_MARSHALLER_RESULT, DATA_MARSHALLER_RESULT_VALUES);

TEST_DEFINE_ENUM_TYPE(MULTITREE_RESULT, MULTITREE_RESULT_VALUES);
IMPLEMENT_UMOCK_C_ENUM_TYPE(MULTITREE_RESULT, MULTITREE_RESULT_VALUES);

TEST_DEFINE_ENUM_TYPE(JSON_ENCODER_RESULT, JSON_ENCODER_RESULT_VALUES);
IMPLEMENT_UMOCK_C_ENUM_TYPE(JSON_ENCODER_RESULT, JSON_ENCODER_RESULT_VALUES);

#define DEFAULT_PROPERTY_NAME_2 "blahBlah"

static MULTITREE_HANDLE my_MultiTree_Create(MULTITREE_CLONE_FUNCTION cloneFunction, MULTITREE_FREE_FUNCTION freeFunction)
{
    (void)(cloneFunction, freeFunction);
    return (MULTITREE_HANDLE)my_gballoc_malloc(1);
}

static void my_MultiTree_Destroy(MULTITREE_HANDLE treeHandle)
{
    my_gballoc_free(treeHandle);
}

static STRING_HANDLE my_STRING_new(void)
{
    return (STRING_HANDLE)my_gballoc_malloc(2);
}

static void my_STRING_delete(STRING_HANDLE handle)
{
    my_gballoc_free(handle);
}

static AGENT_DATA_TYPES_RESULT my_AgentDataTypes_ToString(STRING_HANDLE destination, const AGENT_DATA_TYPE* value)
{
    (void)value;
    (void)real_STRING_concat(destination, "2.4");
    return AGENT_DATA_TYPES_OK;
}

static void on_umock_c_error(UMOCK_C_ERROR_CODE error_code)
{
    char temp_str[256];
    (void)snprintf(temp_str, sizeof(temp_str), "umock_c reported error :%s", ENUM_TO_STRING(UMOCK_C_ERROR_CODE, error_code));
    ASSERT_FAIL(temp_str);
}

BEGIN_TEST_SUITE(DataMarshaller_ut)

    TEST_SUITE_INITIALIZE(TestClassInitialize)
    {
        TEST_INITIALIZE_MEMORY_DEBUG(g_dllByDll);
        g_testByTest = TEST_MUTEX_CREATE();
        ASSERT_IS_NOT_NULL(g_testByTest);

        (void)umock_c_init(on_umock_c_error);

        (void)umocktypes_bool_register_types();
        (void)umocktypes_charptr_register_types();
        (void)umocktypes_stdint_register_types();

        floatValid.type = EDM_SINGLE_TYPE;
        floatValid.value.edmSingle.value = 10.5;
        intValid.type = EDM_INT32_TYPE;
        intValid.value.edmInt32.value = 10;
        structTypeValue.type = EDM_COMPLEX_TYPE_TYPE;
        structTypeValue.value.edmComplexType.nMembers = 1;
        structTypeValue2Members.value.edmComplexType.fields = &members;
        structTypeValue2Members.type = EDM_COMPLEX_TYPE_TYPE;
        structTypeValue2Members.value.edmComplexType.nMembers = COUNT_OF(two_members);
        structTypeValue2Members.value.edmComplexType.fields = two_members;

        REGISTER_UMOCK_ALIAS_TYPE(MULTITREE_CLONE_FUNCTION, void*);
        REGISTER_UMOCK_ALIAS_TYPE(MULTITREE_FREE_FUNCTION, void*);
        REGISTER_UMOCK_ALIAS_TYPE(MULTITREE_HANDLE, void*);
        REGISTER_UMOCK_ALIAS_TYPE(STRING_HANDLE, void*);
        REGISTER_UMOCK_ALIAS_TYPE(JSON_ENCODER_TOSTRING_FUNC, void*);
        REGISTER_UMOCK_ALIAS_TYPE(VECTOR_HANDLE, void*);
        REGISTER_UMOCK_ALIAS_TYPE(const VECTOR_HANDLE, void*);
        
        REGISTER_UMOCK_ALIAS_TYPE(MULTITREE_RESULT, int);
        REGISTER_UMOCK_ALIAS_TYPE(DATA_MARSHALLER_RESULT, int);
        REGISTER_UMOCK_ALIAS_TYPE(JSON_ENCODER_RESULT, int);
            
        REGISTER_GLOBAL_MOCK_HOOK(MultiTree_Create, my_MultiTree_Create);
        REGISTER_GLOBAL_MOCK_HOOK(MultiTree_Destroy, my_MultiTree_Destroy);

        REGISTER_GLOBAL_MOCK_HOOK(STRING_new, real_STRING_new);
        REGISTER_GLOBAL_MOCK_HOOK(STRING_c_str, real_STRING_c_str);
        REGISTER_GLOBAL_MOCK_HOOK(STRING_delete, real_STRING_delete);

        REGISTER_GLOBAL_MOCK_HOOK(AgentDataTypes_ToString, my_AgentDataTypes_ToString);
        REGISTER_GLOBAL_MOCK_FAIL_RETURN(AgentDataTypes_ToString, AGENT_DATA_TYPES_ERROR);

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

        REGISTER_GLOBAL_MOCK_HOOK(json_value_init_object, real_json_value_init_object);
        REGISTER_GLOBAL_MOCK_FAIL_RETURN(json_value_init_object, NULL);
        REGISTER_GLOBAL_MOCK_HOOK(json_object, real_json_object);
        REGISTER_GLOBAL_MOCK_FAIL_RETURN(json_object, NULL);
        REGISTER_GLOBAL_MOCK_HOOK(json_parse_string, real_json_parse_string);
        REGISTER_GLOBAL_MOCK_FAIL_RETURN(json_parse_string, NULL);
        REGISTER_GLOBAL_MOCK_HOOK(json_object_dotset_value, real_json_object_dotset_value);
        REGISTER_GLOBAL_MOCK_FAIL_RETURN(json_object_dotset_value, JSONFailure);
        REGISTER_GLOBAL_MOCK_HOOK(json_value_free, real_json_value_free);
        REGISTER_GLOBAL_MOCK_HOOK(json_serialize_to_string_pretty, real_json_serialize_to_string_pretty);
        REGISTER_GLOBAL_MOCK_FAIL_RETURN(json_serialize_to_string_pretty, NULL);
            
            
            
        REGISTER_GLOBAL_MOCK_HOOK(gballoc_malloc, my_gballoc_malloc);
        REGISTER_GLOBAL_MOCK_FAIL_RETURN(gballoc_malloc, NULL);
        REGISTER_GLOBAL_MOCK_HOOK(gballoc_free, my_gballoc_free);

    }

    TEST_SUITE_CLEANUP(TestClassCleanup)
    {
        umock_c_deinit();

        TEST_MUTEX_DESTROY(g_testByTest);
        TEST_DEINITIALIZE_MEMORY_DEBUG(g_dllByDll);;
    }

    TEST_FUNCTION_INITIALIZE(TestMethodInitialize)
    {
        if (TEST_MUTEX_ACQUIRE(g_testByTest))
        {
            ASSERT_FAIL("our mutex is ABANDONED. Failure in test framework");
        }

        umock_c_reset_all_calls();

    }

    TEST_FUNCTION_CLEANUP(TestMethodCleanup)
    {
        TEST_MUTEX_RELEASE(g_testByTest);
    }

    /* DataMarshaller_Create */

    /*Tests_SRS_DATA_MARSHALLER_99_019:[ DataMarshaller_Create shall return NULL if any argument is NULL.]*/
    TEST_FUNCTION(DataMarshaller_Create_with_NULL_Model_Handle_fails)
    {
        ///arrange

        ///act
        DATA_MARSHALLER_HANDLE res = DataMarshaller_Create(NULL, true);

        ///assert
        ASSERT_IS_NULL(res);
    }

    /* Tests_SRS_DATA_MARSHALLER_99_018:[ DataMarshaller_Create shall create a new DataMarshaller instance and on success it shall return a non NULL handle.]*/
    /* Tests_SRS_DATA_MARSHALLER_99_051:[DataMarshaller_Create shall initialize a BufferProcess instance and associate it with the newly created DataMarshaller instance.] */
    TEST_FUNCTION(DataMarshaller_Create_succeeds)
    {
        ///arrange
        STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument_size();

        ///act
        DATA_MARSHALLER_HANDLE res = DataMarshaller_Create(TEST_MODEL_HANDLE, true);

        ///assert
        ASSERT_IS_NOT_NULL(res);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        DataMarshaller_Destroy(res);
    }

    /* Tests_SRS_DATA_MARSHALLER_99_018:[ DataMarshaller_Create shall create a new DataMarshaller instance and on success it shall return a non NULL handle.]*/

    /* Tests_SRS_DATA_MARSHALLER_99_051:[DataMarshaller_Create shall initialize a BufferProcess instance and associate it with the newly created DataMarshaller instance.] */
    TEST_FUNCTION(DataMarshaller_Create_Twice_Yields_2_Different_Handles)
    {
        ///arrange
        STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument_size();
        STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument_size();

        ///act
        DATA_MARSHALLER_HANDLE handle1 = DataMarshaller_Create(TEST_MODEL_HANDLE, true);
        DATA_MARSHALLER_HANDLE handle2 = DataMarshaller_Create(TEST_MODEL_HANDLE, true);

        ///assert
        ASSERT_IS_NOT_NULL(handle1);
        ASSERT_IS_NOT_NULL(handle2);
        ASSERT_ARE_NOT_EQUAL(void_ptr, (void_ptr)handle1, (void_ptr)handle2);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        DataMarshaller_Destroy(handle1);
        DataMarshaller_Destroy(handle2);
    }

    /* DataMarshaller_Destroy */

    /*Tests_SRS_DATA_MARSHALLER_99_022:[ DataMarshaller_Destroy shall free all resources associated with the dataMarshallerHandle argument.]*/
    TEST_FUNCTION(DataMarshaller_Destroy_succeeds_1)
    {
        ///arrange
        DATA_MARSHALLER_HANDLE handle = DataMarshaller_Create(TEST_MODEL_HANDLE, true);
        umock_c_reset_all_calls();

        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument_ptr();

        ///act
        DataMarshaller_Destroy(handle);

        ///assert
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    }

    /*Tests_SRS_DATA_MARSHALLER_99_024:[ When called with a NULL handle, DataMarshaller_Destroy shall do nothing.]*/
    TEST_FUNCTION(DataMarshaller_Destroy_With_A_NULL_Handle_Does_Nothing)
    {
        /*Comments: "shall do nothing" is impossible to test, but we can try something*/

        ///arrange

        ///act
        DataMarshaller_Destroy(NULL);

        ///assert
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    }

    /* DataMarshaller_SendData */

    /* Tests_SRS_DATA_MARSHALLER_99_033:[ DATA_MARSHALLER_INVALID_ARG shall be returned if the valueCount is zero.] */
    /* Tests_SRS_DATA_MARSHALLER_99_034:[All argument checks shall be performed before calling any other modules.] */
    TEST_FUNCTION(DataMarshaller_SendData_with_Zero_Value_Count_Fails)
    {
        ///arrange
        DATA_MARSHALLER_HANDLE handle = DataMarshaller_Create(TEST_MODEL_HANDLE, true);
        unsigned char* destination;
        size_t destinationSize;
        umock_c_reset_all_calls();

        const DATA_MARSHALLER_VALUE value = { DEFAULT_PROPERTY_NAME, &floatValid };

        ///act
        DATA_MARSHALLER_RESULT result = DataMarshaller_SendData(handle, 0, &value, &destination, &destinationSize);
            
        ///assert
        ASSERT_ARE_EQUAL(DATA_MARSHALLER_RESULT, DATA_MARSHALLER_INVALID_ARG, result);

        /// cleanup
        DataMarshaller_Destroy(handle);
    }

    /*Tests_SRS_DATA_MARSHALLER_99_007:[ DATA_MARSHALLER_INVALID_MODEL_PROPERTY shall be returned when any of the items in values contain invalid data]*/
    /*Tests_SRS_DATA_MARSHALLER_99_034:[All argument checks shall be performed before calling any other modules.]*/
    TEST_FUNCTION(DataMarshaller_SendData_with_PropertyName_NULL_fails)
    {
        ///arrange
        DATA_MARSHALLER_HANDLE handle = DataMarshaller_Create(TEST_MODEL_HANDLE, true);
        unsigned char* destination;
        size_t destinationSize;
        umock_c_reset_all_calls();

        DATA_MARSHALLER_VALUE value = { NULL, &floatValid };

        ///act
        DATA_MARSHALLER_RESULT result = DataMarshaller_SendData(handle, 1, &value, &destination, &destinationSize);
            
        ///assert
        ASSERT_ARE_EQUAL(DATA_MARSHALLER_RESULT, DATA_MARSHALLER_INVALID_MODEL_PROPERTY, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        DataMarshaller_Destroy(handle);
    }

    /*Tests_SRS_DATA_MARSHALLER_99_007:[ DATA_MARSHALLER_INVALID_MODEL_PROPERTY shall be returned when any of the items in values contain invalid data]*/
    /*Tests_SRS_DATA_MARSHALLER_99_034:[All argument checks shall be performed before calling any other modules.]*/
    TEST_FUNCTION(DataMarshaller_SendData_with_Value_NULL_In_The_DATA_MARSHALLER_VALUE_fails)
    {
        ///arrange
        DATA_MARSHALLER_HANDLE handle = DataMarshaller_Create(TEST_MODEL_HANDLE, true);
        unsigned char* destination;
        size_t destinationSize;
        umock_c_reset_all_calls();

        DATA_MARSHALLER_VALUE value = { DEFAULT_PROPERTY_NAME, NULL };

        ///act
        DATA_MARSHALLER_RESULT result = DataMarshaller_SendData(handle, 1, &value, &destination, &destinationSize);

        ///assert
        ASSERT_ARE_EQUAL(DATA_MARSHALLER_RESULT, DATA_MARSHALLER_INVALID_MODEL_PROPERTY, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        DataMarshaller_Destroy(handle);
    }

    /*Tests_SRS_DATA_MARSHALLER_99_007:[ DATA_MARSHALLER_INVALID_MODEL_PROPERTY shall be returned when any of the items in values contain invalid data]*/
    /*Tests_SRS_DATA_MARSHALLER_99_034:[All argument checks shall be performed before calling any other modules.]*/
    TEST_FUNCTION(DataMarshaller_SendData_with_Second_PropertyName_NULL_fails)
    {
        ///arrange
        DATA_MARSHALLER_HANDLE handle = DataMarshaller_Create(TEST_MODEL_HANDLE, true);
        unsigned char* destination;
        size_t destinationSize;
        umock_c_reset_all_calls();

        DATA_MARSHALLER_VALUE values[] = {
            { DEFAULT_PROPERTY_NAME, &floatValid },
            { NULL, &floatValid } };

        ///act
        DATA_MARSHALLER_RESULT result = DataMarshaller_SendData(handle, sizeof(values) / sizeof(values[0]), values, &destination, &destinationSize);

        ///assert
        ASSERT_ARE_EQUAL(DATA_MARSHALLER_RESULT, DATA_MARSHALLER_INVALID_MODEL_PROPERTY, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        DataMarshaller_Destroy(handle);
    }

    /*Tests_SRS_DATA_MARSHALLER_99_007:[ DATA_MARSHALLER_INVALID_MODEL_PROPERTY shall be returned when any of the items in values contain invalid data]*/
    /*Tests_SRS_DATA_MARSHALLER_99_034:[All argument checks shall be performed before calling any other modules.]*/
    TEST_FUNCTION(DataMarshaller_SendData_with_Second_Value_NULL_In_The_DATA_MARSHALLER_VALUE_fails)
    {
        ///arrange
        DATA_MARSHALLER_HANDLE handle = DataMarshaller_Create(TEST_MODEL_HANDLE, true);
        unsigned char* destination;
        size_t destinationSize;
        umock_c_reset_all_calls();

        DATA_MARSHALLER_VALUE values[] = {
            { DEFAULT_PROPERTY_NAME, &floatValid },
            { DEFAULT_PROPERTY_NAME_2, NULL } };

        ///act
        DATA_MARSHALLER_RESULT result = DataMarshaller_SendData(handle, sizeof(values) / sizeof(values[0]), values, &destination, &destinationSize);

        ///assert
        ASSERT_ARE_EQUAL(DATA_MARSHALLER_RESULT, DATA_MARSHALLER_INVALID_MODEL_PROPERTY, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        DataMarshaller_Destroy(handle);
    }

    /*Tests_SRS_DATA_MARSHALLER_99_004:[ DATA_MARSHALLER_INVALID_ARG shall be returned when the function has detected an invalid parameter (NULL) being passed to the function.]*/
    /*Tests_SRS_DATA_MARSHALLER_99_034:[All argument checks shall be performed before calling any other modules.]*/
    TEST_FUNCTION(DataMarshaller_SendData_with_NULL_values_fails)
    {
        ///arrange
        DATA_MARSHALLER_HANDLE handle = DataMarshaller_Create(TEST_MODEL_HANDLE, true);
        unsigned char* destination;
        size_t destinationSize;
        umock_c_reset_all_calls();

        ///act
        DATA_MARSHALLER_RESULT result = DataMarshaller_SendData(handle, 1, NULL, &destination, &destinationSize);

        ///assert
        umock_c_reset_all_calls(); /*because for this test we don't care about the calls in other layers*/
        ASSERT_ARE_EQUAL(DATA_MARSHALLER_RESULT, DATA_MARSHALLER_INVALID_ARG, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        DataMarshaller_Destroy(handle);
    }

    /*Tests_SRS_DATA_MARSHALLER_99_004:[ DATA_MARSHALLER_INVALID_ARG shall be returned when the function has detected an invalid parameter (NULL) being passed to the function.]*/
    TEST_FUNCTION(DataMarshaller_SendData_with_NULL_destination_fails)
    {
        ///arrange
        DATA_MARSHALLER_HANDLE handle = DataMarshaller_Create(TEST_MODEL_HANDLE, true);
        size_t destinationSize;
        DATA_MARSHALLER_VALUE value = { DEFAULT_PROPERTY_NAME, &floatValid };
        umock_c_reset_all_calls();

        ///act
        DATA_MARSHALLER_RESULT result = DataMarshaller_SendData(handle, 1, &value, NULL, &destinationSize);

        ///assert
        umock_c_reset_all_calls(); /*because for this test we don't care about the calls in other layers*/
        ASSERT_ARE_EQUAL(DATA_MARSHALLER_RESULT, DATA_MARSHALLER_INVALID_ARG, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        DataMarshaller_Destroy(handle);
    }

    /*Tests_SRS_DATA_MARSHALLER_99_004:[ DATA_MARSHALLER_INVALID_ARG shall be returned when the function has detected an invalid parameter (NULL) being passed to the function.]*/
    TEST_FUNCTION(DataMarshaller_SendData_with_NULL_destinationSize_fails)
    {
        ///arrange
        DATA_MARSHALLER_HANDLE handle = DataMarshaller_Create(TEST_MODEL_HANDLE, true);
        unsigned char* destination;
        DATA_MARSHALLER_VALUE value = { DEFAULT_PROPERTY_NAME, &floatValid };
        umock_c_reset_all_calls();

        ///act
        DATA_MARSHALLER_RESULT result = DataMarshaller_SendData(handle, 1, &value, &destination, NULL);

        ///assert
        umock_c_reset_all_calls(); /*because for this test we don't care about the calls in other layers*/
        ASSERT_ARE_EQUAL(DATA_MARSHALLER_RESULT, DATA_MARSHALLER_INVALID_ARG, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        DataMarshaller_Destroy(handle);
    }

    /* Tests_SRS_DATA_MARSHALLER_99_035:[DATA_MARSHALLER_MULTITREE_ERROR shall be returned in case any MultiTree API call fails.] */
    TEST_FUNCTION(DataMarshaller_SendData_When_MultiTree_Create_Fails_Then_Fails)
    {
        ///arrange
        DATA_MARSHALLER_HANDLE handle = DataMarshaller_Create(TEST_MODEL_HANDLE, true);
        unsigned char* destination;
        size_t destinationSize;
        umock_c_reset_all_calls();

        DATA_MARSHALLER_VALUE value = { DEFAULT_PROPERTY_NAME, &floatValid };

        EXPECTED_CALL(MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .SetReturn((MULTITREE_HANDLE)NULL);

        ///act
        DATA_MARSHALLER_RESULT result = DataMarshaller_SendData(handle, 1, &value, &destination, &destinationSize);

        ///assert
        ASSERT_ARE_EQUAL(DATA_MARSHALLER_RESULT, DATA_MARSHALLER_MULTITREE_ERROR, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        DataMarshaller_Destroy(handle);
    }

    /* Tests_SRS_DATA_MARSHALLER_99_035:[DATA_MARSHALLER_MULTITREE_ERROR shall be returned in case any MultiTree API call fails.] */
    TEST_FUNCTION(DataMarshaller_SendData_When_MultiTree_AddLeaf_With_Property_Value_Fails_Then_Fails)
    {
        ///arrange
        DATA_MARSHALLER_HANDLE handle = DataMarshaller_Create(TEST_MODEL_HANDLE, true);
        unsigned char* destination;
        size_t destinationSize;
        umock_c_reset_all_calls();

        DATA_MARSHALLER_VALUE value = { DEFAULT_PROPERTY_NAME, &floatValid };

        STRICT_EXPECTED_CALL(MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument_cloneFunction()
            .IgnoreArgument_freeFunction();

        STRICT_EXPECTED_CALL(MultiTree_AddLeaf(IGNORED_PTR_ARG, DEFAULT_PROPERTY_NAME, &floatValid))
            .IgnoreArgument_treeHandle()
            .SetReturn(MULTITREE_ERROR);

        STRICT_EXPECTED_CALL(MultiTree_Destroy(IGNORED_PTR_ARG))
            .IgnoreArgument_treeHandle();

        ///act
        DATA_MARSHALLER_RESULT result = DataMarshaller_SendData(handle, 1, &value, &destination, &destinationSize);

        ///assert
        ASSERT_ARE_EQUAL(DATA_MARSHALLER_RESULT, DATA_MARSHALLER_MULTITREE_ERROR, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        DataMarshaller_Destroy(handle);
    }

    /* Tests_SRS_DATA_MARSHALLER_99_035:[DATA_MARSHALLER_MULTITREE_ERROR shall be returned in case any MultiTree API call fails.] */
    TEST_FUNCTION(DataMarshaller_SendData_When_MultiTree_AddLeaf_With_The_Second_Property_Value_Fails_Then_Fails)
    {
        ///arrange
        DATA_MARSHALLER_HANDLE handle = DataMarshaller_Create(TEST_MODEL_HANDLE, true);
        unsigned char* destination;
        size_t destinationSize;
        umock_c_reset_all_calls();
        AGENT_DATA_TYPE floatValid2;

        DATA_MARSHALLER_VALUE values[2];
        values[0].PropertyPath = DEFAULT_PROPERTY_NAME;
        values[0].Value = &floatValid;

        values[1].PropertyPath = DEFAULT_PROPERTY_NAME_2;
        values[1].Value = &floatValid2;

        EXPECTED_CALL(MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));

        STRICT_EXPECTED_CALL(MultiTree_AddLeaf(IGNORED_PTR_ARG, DEFAULT_PROPERTY_NAME, &floatValid))
            .IgnoreArgument_treeHandle();
        STRICT_EXPECTED_CALL(MultiTree_AddLeaf(IGNORED_PTR_ARG, DEFAULT_PROPERTY_NAME_2, &floatValid2))
            .IgnoreArgument_treeHandle()
            .SetReturn(MULTITREE_ERROR);

        STRICT_EXPECTED_CALL(MultiTree_Destroy(IGNORED_PTR_ARG))
            .IgnoreArgument_treeHandle();

        ///act
        DATA_MARSHALLER_RESULT result = DataMarshaller_SendData(handle, sizeof(values) / sizeof(values[0]), values, &destination, &destinationSize);

        ///assert
        ASSERT_ARE_EQUAL(DATA_MARSHALLER_RESULT, DATA_MARSHALLER_MULTITREE_ERROR, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        DataMarshaller_Destroy(handle);
    }

    /* Tests_SRS_DATA_MARSHALLER_99_027:[ DATA_MARSHALLER_JSON_ENCODER_ERROR shall be returned when JSONEncoder returns an error code.] */
    TEST_FUNCTION(DataMarshaller_SendData_When_Encoding_The_Values_Tree_To_JSON_Fails_Then_Fails)
    {
        ///arrange
        DATA_MARSHALLER_HANDLE handle = DataMarshaller_Create(TEST_MODEL_HANDLE, true);
        umock_c_reset_all_calls();
        unsigned char* destination;
        size_t destinationSize;
        DATA_MARSHALLER_VALUE value = { DEFAULT_PROPERTY_NAME, &floatValid };

        EXPECTED_CALL(MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));

        STRICT_EXPECTED_CALL(MultiTree_AddLeaf(IGNORED_PTR_ARG, DEFAULT_PROPERTY_NAME, &floatValid))
            .IgnoreArgument_treeHandle();
        STRICT_EXPECTED_CALL(STRING_new());
        EXPECTED_CALL(JSONEncoder_EncodeTree(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument_treeHandle()
            .IgnoreArgument_destination()
            .IgnoreArgument_toStringFunc()
            .SetReturn(JSON_ENCODER_ERROR);

        STRICT_EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG))
            .IgnoreArgument_handle();

        STRICT_EXPECTED_CALL(MultiTree_Destroy(IGNORED_PTR_ARG))
            .IgnoreArgument_treeHandle();

        ///act
        DATA_MARSHALLER_RESULT result = DataMarshaller_SendData(handle, 1, &value, &destination, &destinationSize);

        ///assert
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
        ASSERT_ARE_EQUAL(DATA_MARSHALLER_RESULT, DATA_MARSHALLER_JSON_ENCODER_ERROR, result);
            

        ///cleanup
        DataMarshaller_Destroy(handle);
    }

    /* Tests_SRS_DATAMARSHALLER_01_002: [If the includePropertyPath argument passed to DataMarshaller_Create was false and the number of values passed to SendData is greater than 1 and at least one of them is a struct, DataMarshaller_SendData shall fallback to  including the complete property path in the output JSON.] */
    /*Tests_SRS_DATAMARSHALLER_02_007: [DataMarshaller_SendData shall copy in the output parameters *destination, *destinationSize the content and the content length of the encoded JSON tree.] */
    TEST_FUNCTION(when_includepropertypath_is_false_and_value_count_is_greater_than_1_and_one_of_them_is_a_struct_the_property_path_is_included)
    {
        ///arrange
        DATA_MARSHALLER_HANDLE handle = DataMarshaller_Create(TEST_MODEL_HANDLE, false);
        unsigned char* destination;
        size_t destinationSize;
        umock_c_reset_all_calls();
        DATA_MARSHALLER_VALUE value[] = { { DEFAULT_PROPERTY_NAME, &floatValid }, { DEFAULT_PROPERTY_NAME_2, &structTypeValue } };
        char json_payload[] = "Test";

        EXPECTED_CALL(MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));

        STRICT_EXPECTED_CALL(MultiTree_AddLeaf(IGNORED_PTR_ARG, DEFAULT_PROPERTY_NAME, &floatValid))
            .IgnoreArgument_treeHandle();
        STRICT_EXPECTED_CALL(MultiTree_AddLeaf(IGNORED_PTR_ARG, DEFAULT_PROPERTY_NAME_2, &structTypeValue))
            .IgnoreArgument_treeHandle();
        EXPECTED_CALL(STRING_new());
        EXPECTED_CALL(JSONEncoder_EncodeTree(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument_treeHandle()
            .IgnoreArgument_destination()
            .IgnoreArgument_toStringFunc();

        EXPECTED_CALL(STRING_length(IGNORED_PTR_ARG))
            .SetReturn(strlen(json_payload));

        STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument_size();

        EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG))
            .SetReturn(json_payload);

        EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(MultiTree_Destroy(IGNORED_PTR_ARG))
            .IgnoreArgument_treeHandle();

        ///act
        DATA_MARSHALLER_RESULT result = DataMarshaller_SendData(handle, 2, value, &destination, &destinationSize);

        ///assert
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
        ASSERT_ARE_EQUAL(DATA_MARSHALLER_RESULT, DATA_MARSHALLER_OK, result);
        ASSERT_ARE_EQUAL(size_t, strlen(json_payload), destinationSize);
        ASSERT_ARE_EQUAL(int, 0, memcmp(destination, json_payload, destinationSize));
            

        ///cleanup
        free(destination);
        DataMarshaller_Destroy(handle);
    }

    /*Tests_SRS_DATAMARSHALLER_02_006: [The complete JSON object shall be handed over to IoTHubClient by a call to IoTHubClient_LL_SendEventAsync if parameter transportType of _Create was TRANSPORT_LL.] */
    TEST_FUNCTION(DataMarshaller_SendData_sends_to_LL_layer_succeeds)
    {
        ///arrange
        DATA_MARSHALLER_HANDLE handle = DataMarshaller_Create(TEST_MODEL_HANDLE, false);
        unsigned char* destination;
        size_t destinationSize;
        umock_c_reset_all_calls();
        DATA_MARSHALLER_VALUE value[] = { { DEFAULT_PROPERTY_NAME, &floatValid }, { DEFAULT_PROPERTY_NAME_2, &structTypeValue } };
        char json_payload[] = "Test";

        EXPECTED_CALL(MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));

        STRICT_EXPECTED_CALL(MultiTree_AddLeaf(IGNORED_PTR_ARG, DEFAULT_PROPERTY_NAME, &floatValid))
            .IgnoreArgument_treeHandle();
        STRICT_EXPECTED_CALL(MultiTree_AddLeaf(IGNORED_PTR_ARG, DEFAULT_PROPERTY_NAME_2, &structTypeValue))
            .IgnoreArgument_treeHandle();
        EXPECTED_CALL(STRING_new());
        EXPECTED_CALL(JSONEncoder_EncodeTree(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument_treeHandle()
            .IgnoreArgument_destination()
            .IgnoreArgument_toStringFunc();

        EXPECTED_CALL(STRING_length(IGNORED_PTR_ARG))
            .SetReturn(strlen(json_payload));

        STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument_size();

        EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG))
            .SetReturn(json_payload);

        EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(MultiTree_Destroy(IGNORED_PTR_ARG))
            .IgnoreArgument_treeHandle();

        ///act
        DATA_MARSHALLER_RESULT result = DataMarshaller_SendData(handle, 2, value, &destination, &destinationSize);

        ///assert
        ASSERT_ARE_EQUAL(DATA_MARSHALLER_RESULT, DATA_MARSHALLER_OK, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        free(destination);
        DataMarshaller_Destroy(handle);
    }

    /* Tests_SRS_DATAMARSHALLER_01_002: [If the includePropertyPath argument passed to DataMarshaller_Create was false and the number of values passed to SendData is greater than 1 and at least one of them is a struct, DataMarshaller_SendData shall fallback to  including the complete property path in the output JSON.] */
    TEST_FUNCTION(when_includepropertypath_is_false_and_value_count_is_greater_than_1_and_one_but_no_structs_SendData_succeeds)
    {
        ///arrange
        DATA_MARSHALLER_HANDLE handle = DataMarshaller_Create(TEST_MODEL_HANDLE, false);
        unsigned char* destination;
        size_t destinationSize;
        umock_c_reset_all_calls();
        DATA_MARSHALLER_VALUE value[] = { { DEFAULT_PROPERTY_NAME, &floatValid }, { DEFAULT_PROPERTY_NAME_2, &floatValid } };
        char json_payload[] = "Test";

        EXPECTED_CALL(MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));

        STRICT_EXPECTED_CALL(MultiTree_AddLeaf(IGNORED_PTR_ARG, DEFAULT_PROPERTY_NAME, &floatValid))
            .IgnoreArgument_treeHandle();
        STRICT_EXPECTED_CALL(MultiTree_AddLeaf(IGNORED_PTR_ARG, DEFAULT_PROPERTY_NAME_2, &floatValid))
            .IgnoreArgument_treeHandle();
        EXPECTED_CALL(STRING_new());
        EXPECTED_CALL(JSONEncoder_EncodeTree(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument_treeHandle()
            .IgnoreArgument_destination()
            .IgnoreArgument_toStringFunc();

        EXPECTED_CALL(STRING_length(IGNORED_PTR_ARG))
            .SetReturn(strlen(json_payload));

        STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument_size();

        EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG))
            .SetReturn(json_payload);

        EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(MultiTree_Destroy(IGNORED_PTR_ARG))
            .IgnoreArgument_treeHandle();

        ///act
        DATA_MARSHALLER_RESULT result = DataMarshaller_SendData(handle, 2, value, &destination, &destinationSize);

        ///assert
        ASSERT_ARE_EQUAL(DATA_MARSHALLER_RESULT, DATA_MARSHALLER_OK, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        free(destination);
        DataMarshaller_Destroy(handle);
    }

    /* Tests_SRS_DATA_MARSHALLER_99_039:[ If the includePropertyPath argument passed to DataMarshaller_Create was true each property shall be placed in the appropriate position in the JSON according to its path in the model.] */
    TEST_FUNCTION(when_includePropertyPath_is_true_the_property_name_is_placed_in_the_JSON_and_SendAsync_is_called)
    {
        ///arrange
        DATA_MARSHALLER_HANDLE handle = DataMarshaller_Create(TEST_MODEL_HANDLE, true);
        unsigned char* destination;
        size_t destinationSize;
        umock_c_reset_all_calls();
        DATA_MARSHALLER_VALUE value = { DEFAULT_PROPERTY_NAME, &floatValid };
        char json_payload[] = "Test";

        EXPECTED_CALL(MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));

        STRICT_EXPECTED_CALL(MultiTree_AddLeaf(IGNORED_PTR_ARG, DEFAULT_PROPERTY_NAME, &floatValid))
            .IgnoreArgument_treeHandle();
        EXPECTED_CALL(STRING_new());
        EXPECTED_CALL(JSONEncoder_EncodeTree(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument_treeHandle()
            .IgnoreArgument_destination()
            .IgnoreArgument_toStringFunc();

        EXPECTED_CALL(STRING_length(IGNORED_PTR_ARG))
            .SetReturn(strlen(json_payload));

        STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument_size();

        EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG))
            .SetReturn(json_payload);

        EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(MultiTree_Destroy(IGNORED_PTR_ARG))
            .IgnoreArgument_treeHandle();

        ///act
        DATA_MARSHALLER_RESULT result = DataMarshaller_SendData(handle, 1, &value, &destination, &destinationSize);

        ///assert
        ASSERT_ARE_EQUAL(DATA_MARSHALLER_RESULT, DATA_MARSHALLER_OK, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        free(destination);
        DataMarshaller_Destroy(handle);
    }

    /* Tests_SRS_DATAMARSHALLER_01_001: [If the includePropertyPath argument passed to DataMarshaller_Create was false and only one struct is being sent, the relative path of the value passed to DataMarshaller_SendData - including property name - shall be ignored and the value shall be placed at JSON root.] */
    /* Tests_SRS_DATAMARSHALLER_01_004: [In this case the members of the struct shall be added as leafs into the MultiTree, each leaf having the name of the struct member.] */
    TEST_FUNCTION(when_includePropertyPath_is_false_and_one_struct_is_being_sent_the_property_name_is_not_placed_in_the_JSON_and_SendAsync_is_called)
    {
        ///arrange
        DATA_MARSHALLER_HANDLE handle = DataMarshaller_Create(TEST_MODEL_HANDLE, false);
        unsigned char* destination;
        size_t destinationSize;
        umock_c_reset_all_calls();
        DATA_MARSHALLER_VALUE value = { DEFAULT_PROPERTY_NAME, &structTypeValue2Members };
        char json_payload[] = "Test";

        EXPECTED_CALL(MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));

        STRICT_EXPECTED_CALL(MultiTree_AddLeaf(IGNORED_PTR_ARG, "x", structTypeValue2Members.value.edmComplexType.fields[0].value))
            .IgnoreArgument_treeHandle();
        STRICT_EXPECTED_CALL(MultiTree_AddLeaf(IGNORED_PTR_ARG, "y", structTypeValue2Members.value.edmComplexType.fields[1].value))
            .IgnoreArgument_treeHandle();
        EXPECTED_CALL(STRING_new());
        EXPECTED_CALL(JSONEncoder_EncodeTree(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument_treeHandle()
            .IgnoreArgument_destination()
            .IgnoreArgument_toStringFunc();

        EXPECTED_CALL(STRING_length(IGNORED_PTR_ARG))
            .SetReturn(strlen(json_payload));

        STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument_size();

        EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG))
            .SetReturn(json_payload);
        EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(MultiTree_Destroy(IGNORED_PTR_ARG))
            .IgnoreArgument_treeHandle();

        ///act
        DATA_MARSHALLER_RESULT result = DataMarshaller_SendData(handle, 1, &value, &destination, &destinationSize);

        ///assert
        ASSERT_ARE_EQUAL(DATA_MARSHALLER_RESULT, DATA_MARSHALLER_OK, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        free(destination);
        DataMarshaller_Destroy(handle);
    }

    /* Tests_SRS_DATA_MARSHALLER_99_035:[DATA_MARSHALLER_MULTITREE_ERROR shall be returned in case any MultiTree API call fails.] */
    TEST_FUNCTION(when_adding_the_first_member_of_the_struct_fails_then_senddata_fails)
    {
        ///arrange
        DATA_MARSHALLER_HANDLE handle = DataMarshaller_Create(TEST_MODEL_HANDLE, false);
        unsigned char* destination;
        size_t destinationSize;
        umock_c_reset_all_calls();
        DATA_MARSHALLER_VALUE value = { DEFAULT_PROPERTY_NAME, &structTypeValue2Members };

        EXPECTED_CALL(MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));

        STRICT_EXPECTED_CALL(MultiTree_AddLeaf(IGNORED_PTR_ARG, "x", structTypeValue2Members.value.edmComplexType.fields[0].value))
            .IgnoreArgument_treeHandle()
            .SetReturn(MULTITREE_ERROR);

        STRICT_EXPECTED_CALL(MultiTree_Destroy(IGNORED_PTR_ARG))
            .IgnoreArgument_treeHandle();

        ///act
        DATA_MARSHALLER_RESULT result = DataMarshaller_SendData(handle, 1, &value, &destination, &destinationSize);

        ///assert
        ASSERT_ARE_EQUAL(DATA_MARSHALLER_RESULT, DATA_MARSHALLER_MULTITREE_ERROR, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        DataMarshaller_Destroy(handle);
    }

    /* Tests_SRS_DATA_MARSHALLER_99_035:[DATA_MARSHALLER_MULTITREE_ERROR shall be returned in case any MultiTree API call fails.] */
    TEST_FUNCTION(when_adding_the_second_member_of_the_struct_fails_then_senddata_fails)
    {
        ///arrange
        DATA_MARSHALLER_HANDLE handle = DataMarshaller_Create(TEST_MODEL_HANDLE, false);
        unsigned char* destination;
        size_t destinationSize;
        umock_c_reset_all_calls();
        DATA_MARSHALLER_VALUE value = { DEFAULT_PROPERTY_NAME, &structTypeValue2Members };

        EXPECTED_CALL(MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));

        STRICT_EXPECTED_CALL(MultiTree_AddLeaf(IGNORED_PTR_ARG, "x", structTypeValue2Members.value.edmComplexType.fields[0].value))
            .IgnoreArgument_treeHandle();
        STRICT_EXPECTED_CALL(MultiTree_AddLeaf(IGNORED_PTR_ARG, "y", structTypeValue2Members.value.edmComplexType.fields[1].value))
            .IgnoreArgument_treeHandle()
            .SetReturn(MULTITREE_ERROR);

        STRICT_EXPECTED_CALL(MultiTree_Destroy(IGNORED_PTR_ARG))
            .IgnoreArgument_treeHandle();

        ///act
        DATA_MARSHALLER_RESULT result = DataMarshaller_SendData(handle, 1, &value, &destination, &destinationSize);

        ///assert
        ASSERT_ARE_EQUAL(DATA_MARSHALLER_RESULT, DATA_MARSHALLER_MULTITREE_ERROR, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        DataMarshaller_Destroy(handle);
    }

    /* Tests_SRS_DATAMARSHALLER_01_003: [DATA_MARSHALLER_ERROR shall be returned for any errors when calling IoTHubMessage APIs.] */
    TEST_FUNCTION(when_STRING_new_fails_SendData_Fails)
    {
        ///arrange
        DATA_MARSHALLER_HANDLE handle = DataMarshaller_Create(TEST_MODEL_HANDLE, false);
        unsigned char* destination;
        size_t destinationSize;
        umock_c_reset_all_calls();
        DATA_MARSHALLER_VALUE value = { DEFAULT_PROPERTY_NAME, &floatValid };

        EXPECTED_CALL(MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));

        STRICT_EXPECTED_CALL(MultiTree_AddLeaf(IGNORED_PTR_ARG, DEFAULT_PROPERTY_NAME, &floatValid))
            .IgnoreArgument_treeHandle();
        EXPECTED_CALL(STRING_new())
            .SetReturn(NULL);
        STRICT_EXPECTED_CALL(MultiTree_Destroy(IGNORED_PTR_ARG))
            .IgnoreArgument_treeHandle();

        ///act
        DATA_MARSHALLER_RESULT result = DataMarshaller_SendData(handle, 1, &value, &destination, &destinationSize);

        ///assert
        ASSERT_ARE_EQUAL(DATA_MARSHALLER_RESULT, DATA_MARSHALLER_ERROR, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        DataMarshaller_Destroy(handle);
    }

    /*Tests_SRS_DATA_MARSHALLER_02_021: [ If argument dataMarshallerHandle is NULL then DataMarshaller_SendData_ReportedProperties shall fail and return DATA_MARSHALLER_INVALID_ARG. ]*/
    TEST_FUNCTION(DataMarshaller_SendData_ReportedProperties_with_NULL_dataMarshallerHandle_fails)
    {
        ///arrange
        DATA_MARSHALLER_VALUE value = { DEFAULT_PROPERTY_NAME, &floatValid };
        VECTOR_HANDLE values = VECTOR_create(sizeof(DATA_MARSHALLER_VALUE));
        (void)VECTOR_push_back(values, &value, 1);
        unsigned char* destination;
        size_t destinationSize;
        umock_c_reset_all_calls();
        ///act

        DATA_MARSHALLER_RESULT result = DataMarshaller_SendData_ReportedProperties(NULL, values, &destination, &destinationSize);

        ///assert
        ASSERT_ARE_EQUAL(DATA_MARSHALLER_RESULT, DATA_MARSHALLER_INVALID_ARG, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///clean
        VECTOR_destroy(values);
    }

    /*Tests_SRS_DATA_MARSHALLER_02_008: [ If argument values is NULL then DataMarshaller_SendData_ReportedProperties shall fail and return DATA_MARSHALLER_INVALID_ARG. ]*/
    TEST_FUNCTION(DataMarshaller_SendData_ReportedProperties_with_NULL_values_fails)
    {
        ///arrange
        DATA_MARSHALLER_HANDLE handle = DataMarshaller_Create(TEST_MODEL_HANDLE, false);
        unsigned char* destination;
        size_t destinationSize;
        umock_c_reset_all_calls();
        ///act

        DATA_MARSHALLER_RESULT result = DataMarshaller_SendData_ReportedProperties(handle, NULL, &destination, &destinationSize);

        ///assert
        ASSERT_ARE_EQUAL(DATA_MARSHALLER_RESULT, DATA_MARSHALLER_INVALID_ARG, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///clean
        DataMarshaller_Destroy(handle);
    }

    /*Tests_SRS_DATA_MARSHALLER_02_009: [ If argument destination NULL then DataMarshaller_SendData_ReportedProperties shall fail and return DATA_MARSHALLER_INVALID_ARG. ]*/
    TEST_FUNCTION(DataMarshaller_SendData_ReportedProperties_with_NULL_destination_fails)
    {
        ///arrange
        DATA_MARSHALLER_HANDLE handle = DataMarshaller_Create(TEST_MODEL_HANDLE, false);
        DATA_MARSHALLER_VALUE value = { DEFAULT_PROPERTY_NAME, &floatValid };
        VECTOR_HANDLE values = VECTOR_create(sizeof(DATA_MARSHALLER_VALUE));
        (void)VECTOR_push_back(values, &value, 1);
        size_t destinationSize;
        umock_c_reset_all_calls();
        ///act

        DATA_MARSHALLER_RESULT result = DataMarshaller_SendData_ReportedProperties(handle, values, NULL, &destinationSize);

        ///assert
        ASSERT_ARE_EQUAL(DATA_MARSHALLER_RESULT, DATA_MARSHALLER_INVALID_ARG, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///clean
        VECTOR_destroy(values);
        DataMarshaller_Destroy(handle);
    }

    /*Tests_SRS_DATA_MARSHALLER_02_010: [ If argument destinationSize NULL then DataMarshaller_SendData_ReportedProperties shall fail and return DATA_MARSHALLER_INVALID_ARG. ]*/
    TEST_FUNCTION(DataMarshaller_SendData_ReportedProperties_with_NULL_destinationSize_fails)
    {
        ///arrange
        DATA_MARSHALLER_HANDLE handle = DataMarshaller_Create(TEST_MODEL_HANDLE, false);
        DATA_MARSHALLER_VALUE value = { DEFAULT_PROPERTY_NAME, &floatValid };
        VECTOR_HANDLE values = VECTOR_create(sizeof(DATA_MARSHALLER_VALUE));
        (void)VECTOR_push_back(values, &value, 1);
        unsigned char* destination;
        umock_c_reset_all_calls();
        ///act

        DATA_MARSHALLER_RESULT result = DataMarshaller_SendData_ReportedProperties(handle, values, &destination, NULL);

        ///assert
        ASSERT_ARE_EQUAL(DATA_MARSHALLER_RESULT, DATA_MARSHALLER_INVALID_ARG, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///clean
        VECTOR_destroy(values);
        DataMarshaller_Destroy(handle);
    }

    void DataMarshaller_SendData_ReportedProperties_inert_path(void)
    {
        STRICT_EXPECTED_CALL(json_value_init_object());
        STRICT_EXPECTED_CALL(json_object(IGNORED_PTR_ARG))
            .IgnoreArgument_value();

        STRICT_EXPECTED_CALL(VECTOR_size(IGNORED_PTR_ARG))
            .IgnoreArgument_handle();
        size_t i;
        for (i = 0;i < 1;i++) /*these is property indexer*/
        {
            STRICT_EXPECTED_CALL(VECTOR_element(IGNORED_PTR_ARG, i))
                .IgnoreArgument_handle();
            STRICT_EXPECTED_CALL(STRING_new());
            STRICT_EXPECTED_CALL(AgentDataTypes_ToString(IGNORED_PTR_ARG, &floatValid))
                .IgnoreArgument_destination();
            STRICT_EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG))
                .IgnoreArgument_handle();
            STRICT_EXPECTED_CALL(json_parse_string(IGNORED_PTR_ARG))
                .IgnoreArgument_string();
            STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, DEFAULT_PROPERTY_NAME))
                .IgnoreArgument_destination();

            STRICT_EXPECTED_CALL(json_object_dotset_value(IGNORED_PTR_ARG, DEFAULT_PROPERTY_NAME_SLASHES_REPLACED_BY_DOTS, IGNORED_PTR_ARG))
                .IgnoreArgument_object()
                .IgnoreArgument_value();

            STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
                .IgnoreArgument_ptr();
            STRICT_EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG))
                .IgnoreArgument_handle();
        }

        STRICT_EXPECTED_CALL(json_serialize_to_string_pretty(IGNORED_PTR_ARG))
            .IgnoreArgument_value();

        STRICT_EXPECTED_CALL(json_value_free(IGNORED_PTR_ARG))
            .IgnoreArgument_value();
    }

    /*Tests_SRS_DATA_MARSHALLER_02_011: [ DataMarshaller_SendData_ReportedProperties shall ignore the value of includePropertyPath and shall consider it to be true. ]*/
    /*Tests_SRS_DATA_MARSHALLER_02_012: [ DataMarshaller_SendData_ReportedProperties shall create an empty JSON_Value. ]*/
    /*Tests_SRS_DATA_MARSHALLER_02_013: [ DataMarshaller_SendData_ReportedProperties shall get the object behind the JSON_Value by calling json_object. ]*/
    /*Tests_SRS_DATA_MARSHALLER_02_014: [ For every reported property, DataMarshaller_SendData_ReportedProperties shall get the reported property's JSON value (as string) by calling AgentDataTypes_ToString. ]*/
    /*Tests_SRS_DATA_MARSHALLER_02_015: [ DataMarshaller_SendData_ReportedProperties shall import the JSON value (as string) by calling json_parse_string. ]*/
    /*Tests_SRS_DATA_MARSHALLER_02_016: [ DataMarshaller_SendData_ReportedProperties shall replace all the occurences of / with . in the reported property paths. ]*/
    /*Tests_SRS_DATA_MARSHALLER_02_017: [ DataMarshaller_SendData_ReportedProperties shall use json_object_dotset_value passing the reported property path and the imported json value. ]*/
    /*Tests_SRS_DATA_MARSHALLER_02_018: [ DataMarshaller_SendData_ReportedProperties shall use json_serialize_to_string_pretty to produce the output JSON string that fills out parameters destination and destionationSize. ]*/
    /*Tests_SRS_DATA_MARSHALLER_02_020: [ Otherwise DataMarshaller_SendData_ReportedProperties shall succeed and return DATA_MARSHALLER_OK. ]*/
    TEST_FUNCTION(DataMarshaller_SendData_ReportedProperties_happy_path)
    {
        ///arrange
        DATA_MARSHALLER_HANDLE handle = DataMarshaller_Create(TEST_MODEL_HANDLE, false);
        DATA_MARSHALLER_VALUE value = { DEFAULT_PROPERTY_NAME, &floatValid };
        DATA_MARSHALLER_VALUE* pvalue = &value;
        VECTOR_HANDLE values = VECTOR_create(sizeof(DATA_MARSHALLER_VALUE*));
        (void)VECTOR_push_back(values, &pvalue, 1);
        unsigned char* destination;
        size_t destinationSize;
        umock_c_reset_all_calls();
        
        DataMarshaller_SendData_ReportedProperties_inert_path();
        
        ///act
        DATA_MARSHALLER_RESULT result = DataMarshaller_SendData_ReportedProperties(handle, values, &destination, &destinationSize);

        ///assert
        ASSERT_ARE_EQUAL(DATA_MARSHALLER_RESULT, DATA_MARSHALLER_OK, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///clean
        free(destination);
        VECTOR_destroy(values);
        DataMarshaller_Destroy(handle);
    }

    /*Tests_SRS_DATA_MARSHALLER_02_019: [ If any failure occurs, DataMarshaller_SendData_ReportedProperties shall fail and return DATA_MARSHALLER_ERROR. ]*/
    TEST_FUNCTION(DataMarshaller_SendData_ReportedProperties_unhappy_paths)
    {
        ///arrange
        DATA_MARSHALLER_HANDLE handle = DataMarshaller_Create(TEST_MODEL_HANDLE, false);
        DATA_MARSHALLER_VALUE value = { DEFAULT_PROPERTY_NAME, &floatValid };
        DATA_MARSHALLER_VALUE* pvalue = &value;
        VECTOR_HANDLE values = VECTOR_create(sizeof(DATA_MARSHALLER_VALUE*));
        (void)VECTOR_push_back(values, &pvalue, 1);
        unsigned char* destination;
        size_t destinationSize;
        umock_c_reset_all_calls();

        umock_c_negative_tests_init();

        DataMarshaller_SendData_ReportedProperties_inert_path();
        
        umock_c_negative_tests_snapshot();
        
        size_t calls_that_cannot_fail[] =
        {
            2   ,/*VECTOR_size*/
            3, /*VECTOR_element*/
            6, /*STRING_c_str*/
            10, /*(gballoc_free*/
            11, /*STRING_delete*/
            13, /*json_value_free*/
        };

        for (size_t i = 0; i < umock_c_negative_tests_call_count(); i++)
        {
            size_t j;
            umock_c_negative_tests_reset();

            for (j = 0;j<sizeof(calls_that_cannot_fail) / sizeof(calls_that_cannot_fail[0]);j++) /*not running the tests that cannot fail*/
            {
                if (calls_that_cannot_fail[j] == i)
                    break;
            }

            if (j == sizeof(calls_that_cannot_fail) / sizeof(calls_that_cannot_fail[0]))
            {

                umock_c_negative_tests_fail_call(i);
                char temp_str[128];
                sprintf(temp_str, "On failed call %zu", i);

                ///act
                DATA_MARSHALLER_RESULT result = DataMarshaller_SendData_ReportedProperties(handle, values, &destination, &destinationSize);

                ///assert
                ASSERT_ARE_NOT_EQUAL(DATA_MARSHALLER_RESULT, DATA_MARSHALLER_OK, result);
            }
        }

        umock_c_negative_tests_deinit();

        ///clean
        VECTOR_destroy(values);
        DataMarshaller_Destroy(handle);
    }

    void DataMarshaller_SendData_ReportedProperties_of_model_in_model_inert_path(void)
    {
        STRICT_EXPECTED_CALL(json_value_init_object());
        STRICT_EXPECTED_CALL(json_object(IGNORED_PTR_ARG))
            .IgnoreArgument_value();

        STRICT_EXPECTED_CALL(VECTOR_size(IGNORED_PTR_ARG))
            .IgnoreArgument_handle();
        size_t i;
        for (i = 0;i < 1;i++) /*these is property indexer*/
        {
            STRICT_EXPECTED_CALL(VECTOR_element(IGNORED_PTR_ARG, i))
                .IgnoreArgument_handle();
            STRICT_EXPECTED_CALL(STRING_new());
            STRICT_EXPECTED_CALL(AgentDataTypes_ToString(IGNORED_PTR_ARG, &floatValid))
                .IgnoreArgument_destination();
            STRICT_EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG))
                .IgnoreArgument_handle();
            STRICT_EXPECTED_CALL(json_parse_string(IGNORED_PTR_ARG))
                .IgnoreArgument_string();
            STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, DEFAULT_PROPERTY_NAME_LEVEL2))
                .IgnoreArgument_destination();

            STRICT_EXPECTED_CALL(json_object_dotset_value(IGNORED_PTR_ARG, DEFAULT_PROPERTY_NAME_LEVEL2_SLASHES_REPLACED_BY_DOTS, IGNORED_PTR_ARG))
                .IgnoreArgument_object()
                .IgnoreArgument_value();

            STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
                .IgnoreArgument_ptr();
            STRICT_EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG))
                .IgnoreArgument_handle();
        }

        STRICT_EXPECTED_CALL(json_serialize_to_string_pretty(IGNORED_PTR_ARG))
            .IgnoreArgument_value();

        STRICT_EXPECTED_CALL(json_value_free(IGNORED_PTR_ARG))
            .IgnoreArgument_value();
    }

    /*Tests_SRS_DATA_MARSHALLER_02_016: [ DataMarshaller_SendData_ReportedProperties shall replace all the occurences of / with . in the reported property paths. ]*/
    TEST_FUNCTION(DataMarshaller_SendData_ReportedProperties_of_model_in_model_happy_path)
    {
        ///arrange
        DATA_MARSHALLER_HANDLE handle = DataMarshaller_Create(TEST_MODEL_HANDLE, false);
        DATA_MARSHALLER_VALUE value = { DEFAULT_PROPERTY_NAME_LEVEL2, &floatValid };
        DATA_MARSHALLER_VALUE* pvalue = &value;
        VECTOR_HANDLE values = VECTOR_create(sizeof(DATA_MARSHALLER_VALUE*));
        (void)VECTOR_push_back(values, &pvalue, 1);
        unsigned char* destination;
        size_t destinationSize;
        umock_c_reset_all_calls();

        DataMarshaller_SendData_ReportedProperties_of_model_in_model_inert_path();

        ///act
        DATA_MARSHALLER_RESULT result = DataMarshaller_SendData_ReportedProperties(handle, values, &destination, &destinationSize);

        ///assert
        ASSERT_ARE_EQUAL(DATA_MARSHALLER_RESULT, DATA_MARSHALLER_OK, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///clean
        free(destination);
        VECTOR_destroy(values);
        DataMarshaller_Destroy(handle);
    }

    /*the previous test has NO negative tests, and that is intended*/

END_TEST_SUITE(DataMarshaller_ut);
