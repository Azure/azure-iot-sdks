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

static void my_gballoc_free(void* ptr)
{
    free(ptr);
}

void* my_gballoc_realloc(void* ptr, size_t size)
{
    return realloc(ptr, size);
}

static int my_mallocAndStrcpy_s(char** destination, const char* source)
{
    (void)source;
    size_t l = strlen(source);
    *destination = (char*)my_gballoc_malloc(l + 1);
    strcpy(*destination, source);
    return 0;
}

#include "testrunnerswitcher.h"
#include "umock_c.h"
#include "umock_c_negative_tests.h"
#include "umocktypes_charptr.h"
#include "umocktypes_bool.h"
#include "umocktypes_stdint.h"

#define ENABLE_MOCKS

#include "azure_c_shared_utility/gballoc.h"

#include "iothub_client_private.h"
#include "iothub_client_options.h"

#include "azure_c_shared_utility/platform.h"

#include "azure_c_shared_utility/httpheaders.h"
#include "azure_c_shared_utility/httpapiex.h"
#include "azure_c_shared_utility/httpapiexsas.h"
#include "azure_c_shared_utility/uniqueid.h"
#include "parson.h"

MOCKABLE_FUNCTION(, JSON_Value*, json_parse_string, const char *, string);
MOCKABLE_FUNCTION(, JSON_Object*, json_value_get_object, const JSON_Value *, value);
MOCKABLE_FUNCTION(, JSON_Value*, json_object_get_value, const JSON_Object *, object, const char *, name);
MOCKABLE_FUNCTION(, const char*, json_object_get_string, const JSON_Object*, object, const char *, name);
MOCKABLE_FUNCTION(, const char*, json_value_get_string, const JSON_Value*, value);
MOCKABLE_FUNCTION(, double, json_value_get_number, const JSON_Value*, value);
MOCKABLE_FUNCTION(, JSON_Value_Type, json_value_get_type, const JSON_Value*, value);

MOCKABLE_FUNCTION(, JSON_Status, json_object_clear, JSON_Object*, object);
MOCKABLE_FUNCTION(, void, json_value_free, JSON_Value *, value);
MOCKABLE_FUNCTION(, char*, json_serialize_to_string, const JSON_Value*, value);

#undef ENABLE_MOCKS

#include "azure_c_shared_utility/strings.h"

TEST_DEFINE_ENUM_TYPE(HTTPAPI_RESULT, HTTPAPI_RESULT_VALUES);
IMPLEMENT_UMOCK_C_ENUM_TYPE(HTTPAPI_RESULT, HTTPAPI_RESULT_VALUES);
TEST_DEFINE_ENUM_TYPE(HTTPAPIEX_RESULT, HTTPAPIEX_RESULT_VALUES);
IMPLEMENT_UMOCK_C_ENUM_TYPE(HTTPAPIEX_RESULT, HTTPAPIEX_RESULT_VALUES);
TEST_DEFINE_ENUM_TYPE(HTTP_HEADERS_RESULT, HTTP_HEADERS_RESULT_VALUES);
IMPLEMENT_UMOCK_C_ENUM_TYPE(HTTP_HEADERS_RESULT, HTTP_HEADERS_RESULT_VALUES);
TEST_DEFINE_ENUM_TYPE(HTTPAPI_REQUEST_TYPE, HTTPAPI_REQUEST_TYPE_VALUES);
IMPLEMENT_UMOCK_C_ENUM_TYPE(HTTPAPI_REQUEST_TYPE, HTTPAPI_REQUEST_TYPE_VALUES);

static unsigned char* TEST_UNSIGNED_CHAR_PTR = (unsigned char*)"TestString";

static TEST_MUTEX_HANDLE g_testByTest;
static TEST_MUTEX_HANDLE g_dllByDll;

static void on_umock_c_error(UMOCK_C_ERROR_CODE error_code)
{
    (void)error_code;
    ASSERT_FAIL("umock_c reported error");
}

STRING_HANDLE my_STRING_construct(const char* psz)
{
    (void)psz;
    return (STRING_HANDLE)my_gballoc_malloc(1);
}

STRING_HANDLE my_STRING_construct_n(const char* psz, size_t n)
{
    (void)psz;
    n = 0;
    return (STRING_HANDLE)my_gballoc_malloc(1);
}

STRING_HANDLE my_STRING_from_byte_array(const unsigned char* psz, size_t n)
{
    (void)psz;
    n = 0;
    return (STRING_HANDLE)my_gballoc_malloc(1);
}

void my_STRING_delete(STRING_HANDLE handle)
{
    my_gballoc_free(handle);
}

HTTP_HEADERS_HANDLE my_HTTPHeaders_Alloc(void)
{
    return (HTTP_HEADERS_HANDLE)my_gballoc_malloc(1);
}

void my_HTTPHeaders_Free(HTTP_HEADERS_HANDLE handle)
{
    my_gballoc_free(handle);
}

BUFFER_HANDLE my_BUFFER_new(void)
{
    return (BUFFER_HANDLE)my_gballoc_malloc(1);
}

BUFFER_HANDLE my_BUFFER_create(const unsigned char* source, size_t size)
{
    (void)source, size;
    return (BUFFER_HANDLE)my_gballoc_malloc(1);
}

void my_BUFFER_delete(BUFFER_HANDLE handle)
{
    my_gballoc_free(handle);
}

HTTPAPIEX_HANDLE my_HTTPAPIEX_Create(const char* hostName)
{
    (void)hostName;
    return (HTTPAPIEX_HANDLE)my_gballoc_malloc(1);
}

void my_HTTPAPIEX_Destroy(HTTPAPIEX_HANDLE handle)
{
    my_gballoc_free(handle);
}

HTTPAPIEX_SAS_HANDLE my_HTTPAPIEX_SAS_Create(STRING_HANDLE key, STRING_HANDLE uriResource, STRING_HANDLE keyName)
{
    (void)key, uriResource, keyName;
    return (HTTPAPIEX_SAS_HANDLE)my_gballoc_malloc(1);
}

void my_HTTPAPIEX_SAS_Destroy(HTTPAPIEX_SAS_HANDLE handle)
{
    my_gballoc_free(handle);
}

char* my_json_serialize_to_string(const JSON_Value *value)
{
    (void)value;
    char* s = (char*)my_gballoc_malloc(1);
    *s=0;
    return s;
}

JSON_Value *my_json_parse_string(const char *string)
{
    (void)string;
    return (JSON_Value*)my_gballoc_malloc(1);
}

void my_json_value_free(JSON_Value *value)
{
    my_gballoc_free(value);
}


#include "iothub_devicemethod.h"
#include "iothub_service_client_auth.h"

typedef struct IOTHUB_SERVICE_CLIENT_DEVICE_METHOD_TAG
{
    char* hostname;
    char* sharedAccessKey;
    char* keyName;
} IOTHUB_SERVICE_CLIENT_DEVICE_METHOD;

static IOTHUB_SERVICE_CLIENT_AUTH TEST_IOTHUB_SERVICE_CLIENT_AUTH;
static IOTHUB_SERVICE_CLIENT_AUTH_HANDLE TEST_IOTHUB_SERVICE_CLIENT_AUTH_HANDLE = &TEST_IOTHUB_SERVICE_CLIENT_AUTH;

static IOTHUB_SERVICE_CLIENT_DEVICE_METHOD TEST_IOTHUB_SERVICE_CLIENT_DEVICE_METHOD;
static IOTHUB_SERVICE_CLIENT_DEVICE_METHOD_HANDLE TEST_IOTHUB_SERVICE_CLIENT_DEVICE_METHOD_HANDLE = &TEST_IOTHUB_SERVICE_CLIENT_DEVICE_METHOD;

static const char* TEST_STRING_VALUE = "Test string value";
static const char* TEST_CONST_CHAR_PTR = "TestConstChar";

static char* TEST_HOSTNAME = "theHostName";
static char* TEST_IOTHUBNAME = "theIotHubName";
static char* TEST_IOTHUBSUFFIX = "theIotHubSuffix";
static char* TEST_SHAREDACCESSKEY = "theSharedAccessKey";
static char* TEST_SHAREDACCESSKEYNAME = "theSharedAccessKeyName";

static const HTTP_HEADERS_HANDLE TEST_HTTP_HEADERS_HANDLE = (HTTP_HEADERS_HANDLE)0x4545;

static const unsigned int httpStatusCodeOk = 200;
static const unsigned int httpStatusCodeBadRequest = 400;

static const char* TEST_HTTP_HEADER_KEY_AUTHORIZATION = "Authorization";
static const char* TEST_HTTP_HEADER_VAL_AUTHORIZATION = " ";
static const char* TEST_HTTP_HEADER_KEY_REQUEST_ID = "Request-Id";
static const char* TEST_HTTP_HEADER_VAL_REQUEST_ID = "1001";
static const char* TEST_HTTP_HEADER_KEY_USER_AGENT = "User-Agent";
static const char* TEST_HTTP_HEADER_KEY_ACCEPT = "Accept";
static const char* TEST_HTTP_HEADER_VAL_ACCEPT = "application/json";
static const char* TEST_HTTP_HEADER_KEY_CONTENT_TYPE = "Content-Type";
static const char* TEST_HTTP_HEADER_VAL_CONTENT_TYPE = "application/json; charset=utf-8";
static const char* TEST_HTTP_HEADER_KEY_IFMATCH = "If-Match";
static const char* TEST_HTTP_HEADER_VAL_IFMATCH = "*";

static JSON_Value* TEST_JSON_VALUE = (JSON_Value*)0x5050;
static JSON_Object* TEST_JSON_OBJECT = (JSON_Object*)0x5151;
static JSON_Status TEST_JSON_STATUS = 0;

#ifdef __cplusplus
extern "C"
{
#endif
    int STRING_sprintf(STRING_HANDLE handle, const char* format, ...);
    STRING_HANDLE STRING_construct_sprintf(const char* format, ...);

    int STRING_sprintf(STRING_HANDLE handle, const char* format, ...)
    {
        (void)handle;
        (void)format;
        return 0;
    }

    STRING_HANDLE STRING_construct_sprintf(const char* format, ...)
    {
        (void)format;
        return (STRING_HANDLE)my_gballoc_malloc(1);
    }

    const char* my_STRING_c_str(STRING_HANDLE handle)
    {
        (void)handle;
        return TEST_STRING_VALUE;
    }

#ifdef __cplusplus
}
#endif

BEGIN_TEST_SUITE(iothub_devicemethod_ut)

TEST_SUITE_INITIALIZE(TestClassInitialize)
{
    TEST_INITIALIZE_MEMORY_DEBUG(g_dllByDll);
    g_testByTest = TEST_MUTEX_CREATE();
    ASSERT_IS_NOT_NULL(g_testByTest);

    umock_c_init(on_umock_c_error);

    int result = umocktypes_charptr_register_types();
    ASSERT_ARE_EQUAL(int, 0, result);

    REGISTER_TYPE(HTTPAPI_RESULT, HTTPAPI_RESULT);
    REGISTER_TYPE(HTTPAPIEX_RESULT, HTTPAPIEX_RESULT);
    REGISTER_TYPE(HTTP_HEADERS_RESULT, HTTP_HEADERS_RESULT);
    REGISTER_TYPE(HTTPAPI_REQUEST_TYPE, HTTPAPI_REQUEST_TYPE);
    REGISTER_UMOCK_ALIAS_TYPE(VECTOR_HANDLE, void*);
    REGISTER_UMOCK_ALIAS_TYPE(const VECTOR_HANDLE, void*);
    REGISTER_UMOCK_ALIAS_TYPE(PREDICATE_FUNCTION, void*);
    REGISTER_UMOCK_ALIAS_TYPE(BUFFER_HANDLE, void*);
    REGISTER_UMOCK_ALIAS_TYPE(STRING_HANDLE, void*);
    REGISTER_UMOCK_ALIAS_TYPE(HTTP_HEADERS_HANDLE, void*);
    REGISTER_UMOCK_ALIAS_TYPE(HTTP_HANDLE, void*);
    REGISTER_UMOCK_ALIAS_TYPE(HTTPAPIEX_HANDLE, void*);
    REGISTER_UMOCK_ALIAS_TYPE(HTTPAPIEX_SAS_HANDLE, void*);
    REGISTER_UMOCK_ALIAS_TYPE(JSON_Value_Type, int);


    REGISTER_GLOBAL_MOCK_HOOK(gballoc_malloc, my_gballoc_malloc);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(gballoc_malloc, NULL);

    REGISTER_GLOBAL_MOCK_HOOK(gballoc_free, my_gballoc_free);

    REGISTER_GLOBAL_MOCK_HOOK(mallocAndStrcpy_s, my_mallocAndStrcpy_s);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(mallocAndStrcpy_s, 42);

    REGISTER_GLOBAL_MOCK_HOOK(STRING_construct, my_STRING_construct);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(STRING_construct, NULL);

    REGISTER_GLOBAL_MOCK_HOOK(STRING_construct_n, my_STRING_construct_n);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(STRING_construct_n, NULL);

    REGISTER_GLOBAL_MOCK_HOOK(STRING_from_byte_array, my_STRING_from_byte_array);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(STRING_from_byte_array, NULL);

    REGISTER_GLOBAL_MOCK_HOOK(STRING_c_str, my_STRING_c_str);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(STRING_c_str, NULL);

    REGISTER_GLOBAL_MOCK_HOOK(STRING_delete, my_STRING_delete);

    REGISTER_GLOBAL_MOCK_HOOK(BUFFER_new, my_BUFFER_new);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(BUFFER_new, NULL);

    REGISTER_GLOBAL_MOCK_HOOK(BUFFER_create, my_BUFFER_create);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(BUFFER_create, NULL);

    REGISTER_GLOBAL_MOCK_HOOK(BUFFER_delete, my_BUFFER_delete);

    REGISTER_GLOBAL_MOCK_HOOK(HTTPHeaders_Alloc, my_HTTPHeaders_Alloc);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(HTTPHeaders_Alloc, NULL);

    REGISTER_GLOBAL_MOCK_HOOK(HTTPHeaders_Free, my_HTTPHeaders_Free);
    REGISTER_GLOBAL_MOCK_RETURN(HTTPHeaders_AddHeaderNameValuePair, HTTP_HEADERS_OK);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(HTTPHeaders_AddHeaderNameValuePair, HTTP_HEADERS_ERROR);

    REGISTER_GLOBAL_MOCK_HOOK(HTTPAPIEX_Create, my_HTTPAPIEX_Create);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(HTTPAPIEX_Create, NULL);

    REGISTER_GLOBAL_MOCK_HOOK(HTTPAPIEX_Destroy, my_HTTPAPIEX_Destroy);

    REGISTER_GLOBAL_MOCK_HOOK(HTTPAPIEX_SAS_Create, my_HTTPAPIEX_SAS_Create);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(HTTPAPIEX_SAS_Create, NULL);

    REGISTER_GLOBAL_MOCK_HOOK(HTTPAPIEX_SAS_Destroy, my_HTTPAPIEX_SAS_Destroy);

    REGISTER_GLOBAL_MOCK_RETURN(HTTPAPIEX_SAS_ExecuteRequest, HTTPAPIEX_OK);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(HTTPAPIEX_SAS_ExecuteRequest, HTTPAPIEX_ERROR);

    REGISTER_GLOBAL_MOCK_HOOK(json_parse_string, my_json_parse_string);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(json_parse_string, NULL);

    REGISTER_GLOBAL_MOCK_HOOK(json_value_free, my_json_value_free);

    REGISTER_GLOBAL_MOCK_RETURN(json_value_get_object, TEST_JSON_OBJECT);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(json_value_get_object, NULL);

    REGISTER_GLOBAL_MOCK_RETURN(json_object_get_value, TEST_JSON_VALUE);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(json_object_get_value, NULL);

    REGISTER_GLOBAL_MOCK_RETURN(json_object_get_string, TEST_CONST_CHAR_PTR);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(json_object_get_string, NULL);

    REGISTER_GLOBAL_MOCK_RETURN(json_value_get_string, TEST_CONST_CHAR_PTR);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(json_value_get_string, NULL);

    REGISTER_GLOBAL_MOCK_RETURN(json_value_get_number, 42);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(json_value_get_number, 0);

    REGISTER_GLOBAL_MOCK_HOOK(json_serialize_to_string, my_json_serialize_to_string);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(json_serialize_to_string, NULL);
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

    TEST_IOTHUB_SERVICE_CLIENT_AUTH.hostname = TEST_HOSTNAME;
    TEST_IOTHUB_SERVICE_CLIENT_AUTH.iothubName = TEST_IOTHUBNAME;
    TEST_IOTHUB_SERVICE_CLIENT_AUTH.iothubSuffix = TEST_IOTHUBSUFFIX;
    TEST_IOTHUB_SERVICE_CLIENT_AUTH.keyName = TEST_SHAREDACCESSKEYNAME;
    TEST_IOTHUB_SERVICE_CLIENT_AUTH.sharedAccessKey = TEST_SHAREDACCESSKEY;

}

TEST_FUNCTION_CLEANUP(TestMethodCleanup)
{
    umock_c_negative_tests_deinit();
    TEST_MUTEX_RELEASE(g_testByTest);
}

/*Tests_SRS_IOTHUBDEVICEMETHOD_12_001: [ If the serviceClientHandle input parameter is NULL IoTHubDeviceMethod_Create shall return NULL ]*/
TEST_FUNCTION(IoTHubDeviceMethod_Create_return_null_if_input_parameter_serviceClientHandle_is_NULL)
{
    ///arrange

    ///act
    IOTHUB_SERVICE_CLIENT_DEVICE_METHOD_HANDLE result = IoTHubDeviceMethod_Create(NULL);

    ///assert
    ASSERT_IS_NULL(result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
}

/*Tests_SRS_IOTHUBDEVICEMETHOD_12_002: [ If any member of the serviceClientHandle input parameter is NULL IoTHubDeviceMethod_Create shall return NULL ]*/
TEST_FUNCTION(IoTHubDeviceMethod_Create_return_null_if_input_parameter_serviceClientHandle_hostName_is_NULL)
{
    // arrange
    TEST_IOTHUB_SERVICE_CLIENT_AUTH.hostname = NULL;

    // act
    IOTHUB_SERVICE_CLIENT_DEVICE_METHOD_HANDLE result = IoTHubDeviceMethod_Create(TEST_IOTHUB_SERVICE_CLIENT_AUTH_HANDLE);

    // assert
    ASSERT_IS_NULL(result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
}

/*Tests_SRS_IOTHUBDEVICEMETHOD_12_002: [ If any member of the serviceClientHandle input parameter is NULL IoTHubDeviceMethod_Create shall return NULL ]*/
TEST_FUNCTION(IoTHubDeviceMethod_Create_return_null_if_input_parameter_serviceClientHandle_iothubName_is_NULL)
{
    // arrange
    TEST_IOTHUB_SERVICE_CLIENT_AUTH.iothubName = NULL;

    // act
    IOTHUB_SERVICE_CLIENT_DEVICE_METHOD_HANDLE result = IoTHubDeviceMethod_Create(TEST_IOTHUB_SERVICE_CLIENT_AUTH_HANDLE);

    // assert
    ASSERT_IS_NULL(result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
}

/*Tests_SRS_IOTHUBDEVICEMETHOD_12_002: [ If any member of the serviceClientHandle input parameter is NULL IoTHubDeviceMethod_Create shall return NULL ]*/
TEST_FUNCTION(IoTHubDeviceMethod_Create_return_null_if_input_parameter_serviceClientHandle_iothubSuffix_is_NULL)
{
    // arrange
    TEST_IOTHUB_SERVICE_CLIENT_AUTH.iothubSuffix = NULL;

    // act
    IOTHUB_SERVICE_CLIENT_DEVICE_METHOD_HANDLE result = IoTHubDeviceMethod_Create(TEST_IOTHUB_SERVICE_CLIENT_AUTH_HANDLE);

    // assert
    ASSERT_IS_NULL(result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
}

/*Tests_SRS_IOTHUBDEVICEMETHOD_12_002: [ If any member of the serviceClientHandle input parameter is NULL IoTHubDeviceMethod_Create shall return NULL ]*/
TEST_FUNCTION(IoTHubDeviceMethod_Create_return_null_if_input_parameter_serviceClientHandle_keyName_is_NULL)
{
    // arrange
    TEST_IOTHUB_SERVICE_CLIENT_AUTH.keyName = NULL;

    // act
    IOTHUB_SERVICE_CLIENT_DEVICE_METHOD_HANDLE result = IoTHubDeviceMethod_Create(TEST_IOTHUB_SERVICE_CLIENT_AUTH_HANDLE);

    // assert
    ASSERT_IS_NULL(result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
}

/*Tests_SRS_IOTHUBDEVICEMETHOD_12_002: [ If any member of the serviceClientHandle input parameter is NULL IoTHubDeviceMethod_Create shall return NULL ]*/
TEST_FUNCTION(IoTHubDeviceMethod_Create_return_null_if_input_parameter_serviceClientHandle_sharedAccessKey_is_NULL)
{
    // arrange
    TEST_IOTHUB_SERVICE_CLIENT_AUTH.sharedAccessKey = NULL;

    // act
    IOTHUB_SERVICE_CLIENT_DEVICE_METHOD_HANDLE result = IoTHubDeviceMethod_Create(TEST_IOTHUB_SERVICE_CLIENT_AUTH_HANDLE);

    // assert
    ASSERT_IS_NULL(result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
}

/*Tests_SRS_IOTHUBDEVICEMETHOD_12_003: [ IoTHubDeviceMethod_Create shall allocate memory for a new IOTHUB_SERVICE_CLIENT_DEVICE_METHOD_HANDLE instance ]*/
/*Tests_SRS_IOTHUBDEVICEMETHOD_12_005: [ If the allocation successful, IoTHubDeviceMethod_Create shall create a IOTHUB_SERVICE_CLIENT_DEVICE_METHOD_HANDLE from the given IOTHUB_SERVICE_CLIENT_AUTH_HANDLE and return with it ]*/
/*Tests_SRS_IOTHUBDEVICEMETHOD_12_006: [ IoTHubDeviceMethod_Create shall allocate memory and copy hostName to result-hostName by calling mallocAndStrcpy_s. ]*/
/*Tests_SRS_IOTHUBDEVICEMETHOD_12_008: [ IoTHubDeviceMethod_Create shall allocate memory and copy iothubName to result->iothubName by calling mallocAndStrcpy_s. ]*/
/*Tests_SRS_IOTHUBDEVICEMETHOD_12_010: [ IoTHubDeviceMethod_Create shall allocate memory and copy iothubSuffix to result->iothubSuffix by calling mallocAndStrcpy_s. ]*/
/*Tests_SRS_IOTHUBDEVICEMETHOD_12_012: [ IoTHubDeviceMethod_Create shall allocate memory and copy sharedAccessKey to result->sharedAccessKey by calling mallocAndStrcpy_s. ]*/
/*Tests_SRS_IOTHUBDEVICEMETHOD_12_014: [ IoTHubDeviceMethod_Create shall allocate memory and copy keyName to `result->keyName` by calling mallocAndStrcpy_s. ]*/
TEST_FUNCTION(IoTHubDeviceMethod_Create_happy_path)
{
    // arrange
    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
        .IgnoreArgument(1);
    
    EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreAllArguments();
    
    EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreAllArguments();
    
    EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreAllArguments();
    
    // act
    IOTHUB_SERVICE_CLIENT_DEVICE_METHOD_HANDLE result = IoTHubDeviceMethod_Create(TEST_IOTHUB_SERVICE_CLIENT_AUTH_HANDLE);
    
    // assert
    ASSERT_IS_NOT_NULL(result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    
    ///cleanup
    if (result != NULL)
    {
        free(result->hostname);
        free(result->keyName);
        free(result->sharedAccessKey);
        free(result);
        result = NULL;
    }
}

/*Tests_SRS_IOTHUBDEVICEMETHOD_12_004: [ If the allocation failed, IoTHubDeviceMethod_Create shall return NULL ]*/
/*Tests_SRS_IOTHUBDEVICEMETHOD_12_007: [ If the mallocAndStrcpy_s fails, IoTHubDeviceMethod_Create shall do clean up and return NULL. ]*/
/*Tests_SRS_IOTHUBDEVICEMETHOD_12_009: [ If the mallocAndStrcpy_s fails, IoTHubDeviceMethod_Create shall do clean up and return NULL. ]*/
/*Tests_SRS_IOTHUBDEVICEMETHOD_12_011: [ If the mallocAndStrcpy_s fails, IoTHubDeviceMethod_Create shall do clean up and return NULL. ]*/
/*Tests_SRS_IOTHUBDEVICEMETHOD_12_013: [ If the mallocAndStrcpy_s fails, IoTHubDeviceMethod_Create shall do clean up and return NULL. ]*/
/*Tests_SRS_IOTHUBDEVICEMETHOD_12_015: [ If the mallocAndStrcpy_s fails, IoTHubDeviceMethod_Create shall do clean up and return NULL. ]*/
TEST_FUNCTION(IoTHubDeviceMethod_Create_non_happy_path)
{
    // arrange
    int umockc_result = umock_c_negative_tests_init();
    ASSERT_ARE_EQUAL(int, 0, umockc_result);

    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
        .IgnoreArgument(1);

    EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, (const char*)(TEST_IOTHUB_SERVICE_CLIENT_AUTH_HANDLE->hostname)))
        .IgnoreArgument(1);

    EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, (const char*)(TEST_IOTHUB_SERVICE_CLIENT_AUTH_HANDLE->iothubName)))
        .IgnoreArgument(1);

    EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, (const char*)(TEST_IOTHUB_SERVICE_CLIENT_AUTH_HANDLE->keyName)))
        .IgnoreArgument(1);


    umock_c_negative_tests_snapshot();

    ///act
    for (size_t i = 0; i < umock_c_negative_tests_call_count(); i++)
    {
        /// arrange
        umock_c_negative_tests_reset();
        umock_c_negative_tests_fail_call(i);

        /// act
        IOTHUB_SERVICE_CLIENT_DEVICE_METHOD_HANDLE result = IoTHubDeviceMethod_Create(TEST_IOTHUB_SERVICE_CLIENT_AUTH_HANDLE);

        /// assert
        ASSERT_ARE_EQUAL(void_ptr, NULL, result);

        ///cleanup
    }
    umock_c_negative_tests_deinit();

}

/*Tests_SRS_IOTHUBDEVICEMETHOD_12_016: [ If the serviceClientdevicemethodHandle input parameter is NULL IoTHubDeviceMethod_Destroy shall return ]*/
TEST_FUNCTION(IoTHubDeviceMethod_Destroy_return_if_input_parameter_serviceClientdevicemethodHandle_is_NULL)
{
    // arrange

    // act
    IoTHubDeviceMethod_Destroy(NULL);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
}

/*Tests_SRS_IOTHUBDEVICEMETHOD_12_017: [ If the serviceClientdevicemethodHandle input parameter is not NULL IoTHubDeviceMethod_Destroy shall free the memory of it and return ]*/
TEST_FUNCTION(IoTHubDeviceMethod_Destroy_do_clean_up_and_return_if_input_parameter_serviceClientdevicemethodHandle_is_not_NULL)

{
    // arrange
    IOTHUB_SERVICE_CLIENT_DEVICE_METHOD_HANDLE handle = IoTHubDeviceMethod_Create(TEST_IOTHUB_SERVICE_CLIENT_AUTH_HANDLE);

    umock_c_reset_all_calls();

    EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    // act
    IoTHubDeviceMethod_Destroy(handle);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
}
//

/*Tests_SRS_IOTHUBDEVICEMETHOD_12_031: [ IoTHubDeviceMethod_Invoke shall verify the input parameters and if any of them (except the timeout) are NULL then return IOTHUB_DEVICE_METHOD_INVALID_ARG ]*/
TEST_FUNCTION(IoTHubDeviceMethod_Invoke_return_NULL_if_input_parameter_serviceClientdevicemethodHandle_is_NULL)
{
    // arrange

    // act
    const char* deviceId = " ";
    const char* methodName = " ";
    const char* methodPayload = " ";
    unsigned int timeout = 1;
    int responseStatus;
    unsigned char* responsePayload;
    size_t responsePayloadSize;

    IOTHUB_DEVICE_METHOD_RESULT result = IoTHubDeviceMethod_Invoke(NULL, deviceId, methodName, methodPayload, timeout, &responseStatus, &responsePayload, &responsePayloadSize);

    // assert
    ASSERT_ARE_EQUAL(int, result, IOTHUB_DEVICE_METHOD_INVALID_ARG);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
}

/*Tests_SRS_IOTHUBDEVICEMETHOD_12_031: [ IoTHubDeviceMethod_Invoke shall verify the input parameters and if any of them (except the timeout) are NULL then return IOTHUB_DEVICE_METHOD_INVALID_ARG ]*/
TEST_FUNCTION(IoTHubDeviceMethod_Invoke_return_NULL_if_input_parameter_deviceId_is_NULL)
{
    // arrange

    // act
    const char* methodName = " ";
    const char* methodPayload = " ";
    unsigned int timeout = 1;
    int responseStatus;
    unsigned char* responsePayload;
    size_t responsePayloadSize;

    IOTHUB_DEVICE_METHOD_RESULT result = IoTHubDeviceMethod_Invoke(TEST_IOTHUB_SERVICE_CLIENT_DEVICE_METHOD_HANDLE, NULL, methodName, methodPayload, timeout, &responseStatus, &responsePayload, &responsePayloadSize);

    // assert
    ASSERT_ARE_EQUAL(int, result, IOTHUB_DEVICE_METHOD_INVALID_ARG);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
}

/*Tests_SRS_IOTHUBDEVICEMETHOD_12_031: [ IoTHubDeviceMethod_Invoke shall verify the input parameters and if any of them (except the timeout) are NULL then return IOTHUB_DEVICE_METHOD_INVALID_ARG ]*/
TEST_FUNCTION(IoTHubDeviceMethod_Invoke_return_NULL_if_input_parameter_methodName_is_NULL)
{
    // arrange

    // act
    const char* deviceId = " ";
    const char* methodPayload = " ";
    unsigned int timeout = 1;
    int responseStatus;
    unsigned char* responsePayload;
    size_t responsePayloadSize;

    IOTHUB_DEVICE_METHOD_RESULT result = IoTHubDeviceMethod_Invoke(TEST_IOTHUB_SERVICE_CLIENT_DEVICE_METHOD_HANDLE, deviceId, NULL, methodPayload, timeout, &responseStatus, &responsePayload, &responsePayloadSize);

    // assert
    ASSERT_ARE_EQUAL(int, result, IOTHUB_DEVICE_METHOD_INVALID_ARG);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
}

/*Tests_SRS_IOTHUBDEVICEMETHOD_12_031: [ IoTHubDeviceMethod_Invoke shall verify the input parameters and if any of them (except the timeout) are NULL then return IOTHUB_DEVICE_METHOD_INVALID_ARG ]*/
TEST_FUNCTION(IoTHubDeviceMethod_Invoke_return_NULL_if_input_parameter_methodPayload_is_NULL)
{
    // arrange

    // act
    const char* deviceId = " ";
    const char* methodName = " ";
    unsigned int timeout = 1;
    int responseStatus;
    unsigned char* responsePayload;
    size_t responsePayloadSize;

    IOTHUB_DEVICE_METHOD_RESULT result = IoTHubDeviceMethod_Invoke(TEST_IOTHUB_SERVICE_CLIENT_DEVICE_METHOD_HANDLE, deviceId, methodName, NULL, timeout, &responseStatus, &responsePayload, &responsePayloadSize);

    // assert
    ASSERT_ARE_EQUAL(int, result, IOTHUB_DEVICE_METHOD_INVALID_ARG);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
}

/*Tests_SRS_IOTHUBDEVICEMETHOD_12_031: [ IoTHubDeviceMethod_Invoke shall verify the input parameters and if any of them (except the timeout) are NULL then return IOTHUB_DEVICE_METHOD_INVALID_ARG ]*/
TEST_FUNCTION(IoTHubDeviceMethod_Invoke_return_NULL_if_input_parameter_responseStatus_is_NULL)
{
    // arrange

    // act
    const char* deviceId = " ";
    const char* methodName = " ";
    const char* methodPayload = " ";
    unsigned int timeout = 1;
    unsigned char* responsePayload;
    size_t responsePayloadSize;

    IOTHUB_DEVICE_METHOD_RESULT result = IoTHubDeviceMethod_Invoke(TEST_IOTHUB_SERVICE_CLIENT_DEVICE_METHOD_HANDLE, deviceId, methodName, methodPayload, timeout, NULL, &responsePayload, &responsePayloadSize);

    // assert
    ASSERT_ARE_EQUAL(int, result, IOTHUB_DEVICE_METHOD_INVALID_ARG);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
}

/*Tests_SRS_IOTHUBDEVICEMETHOD_12_031: [ IoTHubDeviceMethod_Invoke shall verify the input parameters and if any of them (except the timeout) are NULL then return IOTHUB_DEVICE_METHOD_INVALID_ARG ]*/
TEST_FUNCTION(IoTHubDeviceMethod_Invoke_return_NULL_if_input_parameter_responsePayload_is_NULL)
{
    // arrange

    // act
    const char* deviceId = " ";
    const char* methodName = " ";
    const char* methodPayload = " ";
    unsigned int timeout = 1;
    int responseStatus;
    size_t responsePayloadSize;

    IOTHUB_DEVICE_METHOD_RESULT result = IoTHubDeviceMethod_Invoke(TEST_IOTHUB_SERVICE_CLIENT_DEVICE_METHOD_HANDLE, deviceId, methodName, methodPayload, timeout, &responseStatus, NULL, &responsePayloadSize);

    // assert
    ASSERT_ARE_EQUAL(int, result, IOTHUB_DEVICE_METHOD_INVALID_ARG);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
}

/*Tests_SRS_IOTHUBDEVICEMETHOD_12_031: [ IoTHubDeviceMethod_Invoke shall verify the input parameters and if any of them (except the timeout) are NULL then return IOTHUB_DEVICE_METHOD_INVALID_ARG ]*/
TEST_FUNCTION(IoTHubDeviceMethod_Invoke_return_NULL_if_input_parameter_responsePayloadSize_is_NULL)
{
    // arrange

    // act
    const char* deviceId = " ";
    const char* methodName = " ";
    const char* methodPayload = " ";
    unsigned int timeout = 1;
    int responseStatus;
    unsigned char* responsePayload;

    IOTHUB_DEVICE_METHOD_RESULT result = IoTHubDeviceMethod_Invoke(TEST_IOTHUB_SERVICE_CLIENT_DEVICE_METHOD_HANDLE, deviceId, methodName, methodPayload, timeout, &responseStatus, &responsePayload, NULL);

    // assert
    ASSERT_ARE_EQUAL(int, result, IOTHUB_DEVICE_METHOD_INVALID_ARG);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
}

/*Tests_SRS_IOTHUBDEVICEMETHOD_12_032: [ IoTHubDeviceMethod_Invoke shall create a BUFFER_HANDLE from methodName, timeout and methodPayload by calling BUFFER_create ]*/
/*Tests_SRS_IOTHUBDEVICEMETHOD_12_034: [ IoTHubDeviceMethod_Invoke shall allocate memory for response buffer by calling BUFFER_new ]*/
/*Tests_SRS_IOTHUBDEVICEMETHOD_12_039: [ IoTHubDeviceMethod_Invoke shall create an HTTP POST request using methodPayloadBuffer ]*/
/*Tests_SRS_IOTHUBDEVICEMETHOD_12_040: [ IoTHubDeviceMethod_Invoke shall create an HTTP POST request using the following HTTP headers: authorization=sasToken,Request-Id=1001,Accept=application/json,Content-Type=application/json,charset=utf-8 ]*/
/*Tests_SRS_IOTHUBDEVICEMETHOD_12_041: [ IoTHubDeviceMethod_Invoke shall create an HTTPAPIEX_SAS_HANDLE handle by calling HTTPAPIEX_SAS_Create ]*/
/*Tests_SRS_IOTHUBDEVICEMETHOD_12_042: [ IoTHubDeviceMethod_Invoke shall create an HTTPAPIEX_HANDLE handle by calling HTTPAPIEX_Create ]*/
/*Tests_SRS_IOTHUBDEVICEMETHOD_12_043: [ IoTHubDeviceMethod_Invoke shall execute the HTTP POST request by calling HTTPAPIEX_ExecuteRequest ]*/
/*Tests_SRS_IOTHUBDEVICEMETHOD_12_049: [ Otherwise IoTHubDeviceMethod_Invoke shall save the received status and payload to the corresponding out parameter and return with IOTHUB_DEVICE_METHOD_OK ]*/
TEST_FUNCTION(IoTHubDeviceMethod_Invoke_happy_path)
{
    // arrange
    EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG));
    EXPECTED_CALL(BUFFER_create(IGNORED_PTR_ARG, IGNORED_NUM_ARG))
        .IgnoreAllArguments();
    EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    EXPECTED_CALL(BUFFER_new());

    EXPECTED_CALL(STRING_construct(TEST_HOSTNAME));
    EXPECTED_CALL(STRING_construct(TEST_SHAREDACCESSKEY));
    EXPECTED_CALL(STRING_construct(TEST_SHAREDACCESSKEYNAME));

    EXPECTED_CALL(HTTPHeaders_Alloc());
    EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_AUTHORIZATION, TEST_HTTP_HEADER_VAL_AUTHORIZATION))
        .IgnoreArgument(1);
    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(UniqueId_Generate(IGNORED_PTR_ARG, IGNORED_NUM_ARG));

    EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_REQUEST_ID, TEST_HTTP_HEADER_VAL_REQUEST_ID))
        .IgnoreArgument(1);
    EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_USER_AGENT, IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_ACCEPT, TEST_HTTP_HEADER_VAL_ACCEPT))
        .IgnoreArgument(1);

    EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));

    EXPECTED_CALL(HTTPAPIEX_SAS_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreAllArguments();
    EXPECTED_CALL(HTTPAPIEX_Create(TEST_HOSTNAME));

    EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG));

    EXPECTED_CALL(HTTPAPIEX_SAS_ExecuteRequest(IGNORED_PTR_ARG, IGNORED_PTR_ARG, HTTPAPI_REQUEST_POST, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreAllArguments()
        .CopyOutArgumentBuffer_statusCode(&httpStatusCodeOk, sizeof(httpStatusCodeOk))
        .SetReturn(HTTPAPIEX_OK);

    EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    EXPECTED_CALL(HTTPAPIEX_Destroy(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    EXPECTED_CALL(HTTPAPIEX_SAS_Destroy(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    EXPECTED_CALL(HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    EXPECTED_CALL(BUFFER_u_char(IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .SetReturn(TEST_UNSIGNED_CHAR_PTR);
    EXPECTED_CALL(BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    EXPECTED_CALL(STRING_from_byte_array(IGNORED_PTR_ARG, IGNORED_NUM_ARG))
        .IgnoreAllArguments();
    EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    EXPECTED_CALL(json_parse_string(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    EXPECTED_CALL(json_value_get_object(TEST_JSON_VALUE));
    EXPECTED_CALL(json_object_get_value(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreAllArguments();
    EXPECTED_CALL(json_object_get_value(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreAllArguments();

    EXPECTED_CALL(json_serialize_to_string(IGNORED_PTR_ARG))
        .IgnoreAllArguments();
    EXPECTED_CALL(json_value_get_number(IGNORED_PTR_ARG))
        .IgnoreAllArguments();

    EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    EXPECTED_CALL(json_value_free(IGNORED_PTR_ARG))
        .IgnoreAllArguments();

    EXPECTED_CALL(BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    EXPECTED_CALL(BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    // act
    const char* deviceId = "deviceId";
    const char* methodName = "methodName";
    const char* methodPayload = "methodPayload";
    unsigned int timeout = 1;
    int responseStatus;
    unsigned char* responsePayload;
    size_t responsePayloadSize;

    IOTHUB_DEVICE_METHOD_RESULT result = IoTHubDeviceMethod_Invoke(TEST_IOTHUB_SERVICE_CLIENT_DEVICE_METHOD_HANDLE, deviceId, methodName, methodPayload, timeout, &responseStatus, &responsePayload, &responsePayloadSize);

    // assert
    ASSERT_ARE_EQUAL(int, result, IOTHUB_DEVICE_METHOD_OK);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    free((void*)responsePayload);
}

/*Tests_SRS_IOTHUBDEVICEMETHOD_12_032: [ IoTHubDeviceMethod_Invoke shall create a BUFFER_HANDLE from methodName, timeout and methodPayload by calling BUFFER_create ]*/
/*Tests_SRS_IOTHUBDEVICEMETHOD_12_034: [ IoTHubDeviceMethod_Invoke shall allocate memory for response buffer by calling BUFFER_new ]*/
/*Tests_SRS_IOTHUBDEVICEMETHOD_12_039: [ IoTHubDeviceMethod_Invoke shall create an HTTP POST request using methodPayloadBuffer ]*/
/*Tests_SRS_IOTHUBDEVICEMETHOD_12_040: [ IoTHubDeviceMethod_Invoke shall create an HTTP POST request using the following HTTP headers: authorization=sasToken,Request-Id=1001,Accept=application/json,Content-Type=application/json,charset=utf-8 ]*/
/*Tests_SRS_IOTHUBDEVICEMETHOD_12_041: [ IoTHubDeviceMethod_Invoke shall create an HTTPAPIEX_SAS_HANDLE handle by calling HTTPAPIEX_SAS_Create ]*/
/*Tests_SRS_IOTHUBDEVICEMETHOD_12_042: [ IoTHubDeviceMethod_Invoke shall create an HTTPAPIEX_HANDLE handle by calling HTTPAPIEX_Create ]*/
/*Tests_SRS_IOTHUBDEVICEMETHOD_12_043: [ IoTHubDeviceMethod_Invoke shall execute the HTTP POST request by calling HTTPAPIEX_ExecuteRequest ]*/
/*Tests_SRS_IOTHUBDEVICEMETHOD_12_049: [ Otherwise IoTHubDeviceMethod_Invoke shall save the received status and payload to the corresponding out parameter and return with IOTHUB_DEVICE_METHOD_OK ]*/
TEST_FUNCTION(IoTHubDeviceMethod_Invoke_happy_path_http_return_not_equal_200)
{
    // arrange
    EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG));
    EXPECTED_CALL(BUFFER_create(IGNORED_PTR_ARG, IGNORED_NUM_ARG))
        .IgnoreAllArguments();
    EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    EXPECTED_CALL(BUFFER_new());

    EXPECTED_CALL(STRING_construct(TEST_HOSTNAME));
    EXPECTED_CALL(STRING_construct(TEST_SHAREDACCESSKEY));
    EXPECTED_CALL(STRING_construct(TEST_SHAREDACCESSKEYNAME));

    EXPECTED_CALL(HTTPHeaders_Alloc());
    EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_AUTHORIZATION, TEST_HTTP_HEADER_VAL_AUTHORIZATION))
        .IgnoreArgument(1);
    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(UniqueId_Generate(IGNORED_PTR_ARG, IGNORED_NUM_ARG));

    EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_REQUEST_ID, TEST_HTTP_HEADER_VAL_REQUEST_ID))
        .IgnoreArgument(1);
    EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_USER_AGENT, IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_ACCEPT, TEST_HTTP_HEADER_VAL_ACCEPT))
        .IgnoreArgument(1);

    EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));

    EXPECTED_CALL(HTTPAPIEX_SAS_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreAllArguments();
    EXPECTED_CALL(HTTPAPIEX_Create(TEST_HOSTNAME));

    EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG));

    EXPECTED_CALL(HTTPAPIEX_SAS_ExecuteRequest(IGNORED_PTR_ARG, IGNORED_PTR_ARG, HTTPAPI_REQUEST_GET, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreAllArguments()
        .CopyOutArgumentBuffer_statusCode(&httpStatusCodeBadRequest, sizeof(httpStatusCodeBadRequest))
        .SetReturn(HTTPAPIEX_OK);

    EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    EXPECTED_CALL(HTTPAPIEX_Destroy(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    EXPECTED_CALL(HTTPAPIEX_SAS_Destroy(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    EXPECTED_CALL(HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    EXPECTED_CALL(BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    EXPECTED_CALL(BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    // act
    const char* deviceId = "deviceId";
    const char* methodName = "methodName";
    const char* methodPayload = "methodPayload";
    unsigned int timeout = 1;
    int responseStatus;
    unsigned char* responsePayload;
    size_t responsePayloadSize;

    IOTHUB_DEVICE_METHOD_RESULT result = IoTHubDeviceMethod_Invoke(TEST_IOTHUB_SERVICE_CLIENT_DEVICE_METHOD_HANDLE, deviceId, methodName, methodPayload, timeout, &responseStatus, &responsePayload, &responsePayloadSize);

    // assert
    ASSERT_ARE_EQUAL(int, result, IOTHUB_DEVICE_METHOD_ERROR);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
}

/*Tests_SRS_IOTHUBDEVICEMETHOD_12_033: [ If the creation fails, IoTHubDeviceMethod_Invoke shall return IOTHUB_DEVICE_METHOD_ERROR ]*/
/*Tests_SRS_IOTHUBDEVICEMETHOD_12_035: [ If the allocation failed, IoTHubDeviceMethod_Invoke shall return IOTHUB_DEVICE_METHOD_ERROR ]*/
/*Tests_SRS_IOTHUBDEVICEMETHOD_12_044: [ If any of the call fails during the HTTP creation IoTHubDeviceMethod_Invoke shall fail and return IOTHUB_DEVICE_METHOD_ERROR ]*/
/*Tests_SRS_IOTHUBDEVICEMETHOD_12_045: [ If any of the HTTPAPI call fails IoTHubDeviceMethod_Invoke shall fail and return IOTHUB_DEVICE_METHOD_ERROR ]*/
/*Tests_SRS_IOTHUBDEVICEMETHOD_12_046: [ IoTHubDeviceMethod_Invoke shall verify the received HTTP status code and if it is not equal to 200 then return IOTHUB_DEVICE_METHOD_ERROR ]*/
/*Tests_SRS_IOTHUBDEVICEMETHOD_12_048: [ If memory allocation for output paramater fails IoTHubDeviceMethod_Invoke shall return IOTHUB_DEVICE_METHOD_ERROR ]*/
/*Tests_SRS_IOTHUBDEVICEMETHOD_12_047: [ If parsing the response fails IoTHubDeviceMethod_Invoke shall return IOTHUB_DEVICE_METHOD_ERROR ]*/
TEST_FUNCTION(IoTHubDeviceMethod_Invoke_non_happy_path)
{
    IOTHUB_DEVICE_METHOD_RESULT result;

    // arrange
    int umockc_result = umock_c_negative_tests_init();
    ASSERT_ARE_EQUAL(int, 0, umockc_result);

    EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG));
    EXPECTED_CALL(BUFFER_create(IGNORED_PTR_ARG, IGNORED_NUM_ARG))
        .IgnoreAllArguments();
    EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    EXPECTED_CALL(BUFFER_new());

    EXPECTED_CALL(STRING_construct(TEST_HOSTNAME));
    EXPECTED_CALL(STRING_construct(TEST_SHAREDACCESSKEY));
    EXPECTED_CALL(STRING_construct(TEST_SHAREDACCESSKEYNAME));

    EXPECTED_CALL(HTTPHeaders_Alloc());
    EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_AUTHORIZATION, TEST_HTTP_HEADER_VAL_AUTHORIZATION))
        .IgnoreArgument(1);
    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(UniqueId_Generate(IGNORED_PTR_ARG, IGNORED_NUM_ARG));

    EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_REQUEST_ID, TEST_HTTP_HEADER_VAL_REQUEST_ID))
        .IgnoreArgument(1);
    EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_USER_AGENT, IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_ACCEPT, TEST_HTTP_HEADER_VAL_ACCEPT))
        .IgnoreArgument(1);

    EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));

    EXPECTED_CALL(HTTPAPIEX_SAS_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreAllArguments();
    EXPECTED_CALL(HTTPAPIEX_Create(TEST_HOSTNAME));

    EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG));

    EXPECTED_CALL(HTTPAPIEX_SAS_ExecuteRequest(IGNORED_PTR_ARG, IGNORED_PTR_ARG, HTTPAPI_REQUEST_POST, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreAllArguments()
        .CopyOutArgumentBuffer_statusCode(&httpStatusCodeOk, sizeof(httpStatusCodeOk))
        .SetReturn(HTTPAPIEX_OK);

    EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    EXPECTED_CALL(HTTPAPIEX_Destroy(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    EXPECTED_CALL(HTTPAPIEX_SAS_Destroy(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    EXPECTED_CALL(HTTPHeaders_Free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    EXPECTED_CALL(BUFFER_u_char(IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .SetReturn(TEST_UNSIGNED_CHAR_PTR);
    EXPECTED_CALL(BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    EXPECTED_CALL(STRING_from_byte_array(IGNORED_PTR_ARG, IGNORED_NUM_ARG))
        .IgnoreAllArguments();

    EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    EXPECTED_CALL(json_parse_string(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    EXPECTED_CALL(json_value_get_object(TEST_JSON_VALUE));
    EXPECTED_CALL(json_object_get_value(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreAllArguments();
    EXPECTED_CALL(json_object_get_value(IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreAllArguments();

    EXPECTED_CALL(json_serialize_to_string(IGNORED_PTR_ARG))
        .IgnoreAllArguments();
    EXPECTED_CALL(json_value_get_number(IGNORED_PTR_ARG))
        .IgnoreAllArguments();

    EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    EXPECTED_CALL(json_value_free(IGNORED_PTR_ARG))
        .IgnoreAllArguments();

    EXPECTED_CALL(BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    EXPECTED_CALL(BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    const char* deviceId = "deviceId";
    const char* methodName = "methodName";
    const char* methodPayload = "methodPayload";
    unsigned int timeout = 1;
    int responseStatus;
    unsigned char* responsePayload;
    size_t responsePayloadSize;


    // act
    umock_c_negative_tests_snapshot();

    // first make sure our expected calls are correct.
    result = IoTHubDeviceMethod_Invoke(TEST_IOTHUB_SERVICE_CLIENT_DEVICE_METHOD_HANDLE, deviceId, methodName, methodPayload, timeout, &responseStatus, &responsePayload, &responsePayloadSize);

    ASSERT_ARE_EQUAL(int, result, IOTHUB_DEVICE_METHOD_OK);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    free((void*)responsePayload);

    for (size_t i = 0; i < umock_c_negative_tests_call_count(); i++)
    {
        /// arrange
        umock_c_negative_tests_reset();
        umock_c_negative_tests_fail_call(i);

        /// act
        if (
            (i != 2)  && /*STRING_delete*/
            (i != 10) && /*UniqueId_Generate*/
            (i != 14) && /*gballoc_free*/
            (i != 17) && /*STRING_c_str*/
            (i != 19) && /*STRING_delete*/
            (i != 20) && /*HTTPAPIEX_Destroy*/
            (i != 21) && /*HTTPAPIEX_SAS_Destroy*/
            (i != 22) && /*HTTPHeaders_Free*/
            (i != 23) && /*STRING_delete*/
            (i != 24) && /*STRING_delete*/
            (i != 25) && /*STRING_delete*/
            (i != 27) && /*BUFFER_length*/
            (i != 35) && /*json_value_get_number*/
            (i != 36) && /*STRING_delete*/
            (i != 37) && /*json_value_free*/
            (i != 38) && /*BUFFER_delete*/
            (i != 39)    /*BUFFER_delete*/
            )
        {
            result = IoTHubDeviceMethod_Invoke(TEST_IOTHUB_SERVICE_CLIENT_DEVICE_METHOD_HANDLE, deviceId, methodName, methodPayload, timeout, &responseStatus, &responsePayload, &responsePayloadSize);

            /// assert
            ASSERT_ARE_NOT_EQUAL(int, result, IOTHUB_DEVICE_METHOD_OK);
        }
        /// cleanup
    }
    umock_c_negative_tests_deinit();
}

END_TEST_SUITE(iothub_devicemethod_ut)
