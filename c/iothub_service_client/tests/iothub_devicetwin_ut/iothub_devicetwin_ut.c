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

#include "iothub_devicetwin.h"
#include "iothub_service_client_auth.h"

typedef struct IOTHUB_SERVICE_CLIENT_DEVICE_TWIN_TAG
{
    char* hostname;
    char* sharedAccessKey;
    char* keyName;
} IOTHUB_SERVICE_CLIENT_DEVICE_TWIN;

static IOTHUB_SERVICE_CLIENT_AUTH TEST_IOTHUB_SERVICE_CLIENT_AUTH;
static IOTHUB_SERVICE_CLIENT_AUTH_HANDLE TEST_IOTHUB_SERVICE_CLIENT_AUTH_HANDLE = &TEST_IOTHUB_SERVICE_CLIENT_AUTH;

static IOTHUB_SERVICE_CLIENT_DEVICE_TWIN TEST_IOTHUB_SERVICE_CLIENT_DEVICE_TWIN;
static IOTHUB_SERVICE_CLIENT_DEVICE_TWIN_HANDLE TEST_IOTHUB_SERVICE_CLIENT_DEVICE_TWIN_HANDLE = &TEST_IOTHUB_SERVICE_CLIENT_DEVICE_TWIN;

static const char* TEST_STRING_VALUE = "Test string value";

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

BEGIN_TEST_SUITE(iothub_devicetwin_ut)

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

    REGISTER_GLOBAL_MOCK_HOOK(gballoc_malloc, my_gballoc_malloc);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(gballoc_malloc, NULL);

    REGISTER_GLOBAL_MOCK_HOOK(gballoc_free, my_gballoc_free);

    REGISTER_GLOBAL_MOCK_HOOK(mallocAndStrcpy_s, my_mallocAndStrcpy_s);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(mallocAndStrcpy_s, 42);

    REGISTER_GLOBAL_MOCK_HOOK(STRING_construct, my_STRING_construct);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(STRING_construct, NULL);

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

/*Tests_SRS_IOTHUBDEVICETWIN_12_001: [ If the serviceClientHandle input parameter is NULL IoTHubDeviceTwin_Create shall return NULL ]*/
TEST_FUNCTION(IoTHubDeviceTwin_Create_return_null_if_input_parameter_serviceClientHandle_is_NULL)
{
    ///arrange

    ///act
    IOTHUB_SERVICE_CLIENT_DEVICE_TWIN_HANDLE result = IoTHubDeviceTwin_Create(NULL);

    ///assert
    ASSERT_IS_NULL(result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
}

/*Tests_SRS_IOTHUBDEVICETWIN_12_002: [ If any member of the serviceClientHandle input parameter is NULL IoTHubDeviceTwin_Create shall return NULL ]*/
TEST_FUNCTION(IoTHubDeviceTwin_Create_return_null_if_input_parameter_serviceClientHandle_hostName_is_NULL)
{
    // arrange
    TEST_IOTHUB_SERVICE_CLIENT_AUTH.hostname = NULL;

    // act
    IOTHUB_SERVICE_CLIENT_DEVICE_TWIN_HANDLE result = IoTHubDeviceTwin_Create(TEST_IOTHUB_SERVICE_CLIENT_AUTH_HANDLE);

    // assert
    ASSERT_IS_NULL(result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
}

/*Tests_SRS_IOTHUBDEVICETWIN_12_002: [ If any member of the serviceClientHandle input parameter is NULL IoTHubDeviceTwin_Create shall return NULL ]*/
TEST_FUNCTION(IoTHubDeviceTwin_Create_return_null_if_input_parameter_serviceClientHandle_iothubName_is_NULL)
{
    // arrange
    TEST_IOTHUB_SERVICE_CLIENT_AUTH.iothubName = NULL;

    // act
    IOTHUB_SERVICE_CLIENT_DEVICE_TWIN_HANDLE result = IoTHubDeviceTwin_Create(TEST_IOTHUB_SERVICE_CLIENT_AUTH_HANDLE);

    // assert
    ASSERT_IS_NULL(result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
}

/*Tests_SRS_IOTHUBDEVICETWIN_12_002: [ If any member of the serviceClientHandle input parameter is NULL IoTHubDeviceTwin_Create shall return NULL ]*/
TEST_FUNCTION(IoTHubDeviceTwin_Create_return_null_if_input_parameter_serviceClientHandle_iothubSuffix_is_NULL)
{
    // arrange
    TEST_IOTHUB_SERVICE_CLIENT_AUTH.iothubSuffix = NULL;

    // act
    IOTHUB_SERVICE_CLIENT_DEVICE_TWIN_HANDLE result = IoTHubDeviceTwin_Create(TEST_IOTHUB_SERVICE_CLIENT_AUTH_HANDLE);

    // assert
    ASSERT_IS_NULL(result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
}

/*Tests_SRS_IOTHUBDEVICETWIN_12_002: [ If any member of the serviceClientHandle input parameter is NULL IoTHubDeviceTwin_Create shall return NULL ]*/
TEST_FUNCTION(IoTHubDeviceTwin_Create_return_null_if_input_parameter_serviceClientHandle_keyName_is_NULL)
{
    // arrange
    TEST_IOTHUB_SERVICE_CLIENT_AUTH.keyName = NULL;

    // act
    IOTHUB_SERVICE_CLIENT_DEVICE_TWIN_HANDLE result = IoTHubDeviceTwin_Create(TEST_IOTHUB_SERVICE_CLIENT_AUTH_HANDLE);

    // assert
    ASSERT_IS_NULL(result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
}

/*Tests_SRS_IOTHUBDEVICETWIN_12_002: [ If any member of the serviceClientHandle input parameter is NULL IoTHubDeviceTwin_Create shall return NULL ]*/
TEST_FUNCTION(IoTHubDeviceTwin_Create_return_null_if_input_parameter_serviceClientHandle_sharedAccessKey_is_NULL)
{
    // arrange
    TEST_IOTHUB_SERVICE_CLIENT_AUTH.sharedAccessKey = NULL;

    // act
    IOTHUB_SERVICE_CLIENT_DEVICE_TWIN_HANDLE result = IoTHubDeviceTwin_Create(TEST_IOTHUB_SERVICE_CLIENT_AUTH_HANDLE);

    // assert
    ASSERT_IS_NULL(result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
}

/*Tests_SRS_IOTHUBDEVICETWIN_12_003: [ IoTHubDeviceTwin_Create shall allocate memory for a new IOTHUB_SERVICE_CLIENT_DEVICE_TWIN_HANDLE instance ]*/
/*Tests_SRS_IOTHUBDEVICETWIN_12_005: [ If the allocation successful, IoTHubDeviceTwin_Create shall create a IOTHUB_SERVICE_CLIENT_DEVICE_TWIN_HANDLE from the given IOTHUB_SERVICE_CLIENT_AUTH_HANDLE and return with it ]*/
/*Tests_SRS_IOTHUBDEVICETWIN_12_006: [ IoTHubDeviceTwin_Create shall allocate memory and copy hostName to result-hostName by calling mallocAndStrcpy_s. ]*/
/*Tests_SRS_IOTHUBDEVICETWIN_12_008: [ IoTHubDeviceTwin_Create shall allocate memory and copy iothubName to result->iothubName by calling mallocAndStrcpy_s. ]*/
/*Tests_SRS_IOTHUBDEVICETWIN_12_010: [ IoTHubDeviceTwin_Create shall allocate memory and copy iothubSuffix to result->iothubSuffix by calling mallocAndStrcpy_s. ]*/
/*Tests_SRS_IOTHUBDEVICETWIN_12_012: [ IoTHubDeviceTwin_Create shall allocate memory and copy sharedAccessKey to result->sharedAccessKey by calling mallocAndStrcpy_s. ]*/
/*Tests_SRS_IOTHUBDEVICETWIN_12_014: [ IoTHubDeviceTwin_Create shall allocate memory and copy keyName to `result->keyName` by calling mallocAndStrcpy_s. ]*/
TEST_FUNCTION(IoTHubDeviceTwin_Create_happy_path)
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
    IOTHUB_SERVICE_CLIENT_DEVICE_TWIN_HANDLE result = IoTHubDeviceTwin_Create(TEST_IOTHUB_SERVICE_CLIENT_AUTH_HANDLE);
    
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

/*Tests_SRS_IOTHUBDEVICETWIN_12_004: [ If the allocation failed, IoTHubDeviceTwin_Create shall return NULL ]*/
/*Tests_SRS_IOTHUBDEVICETWIN_12_007: [ If the mallocAndStrcpy_s fails, IoTHubDeviceTwin_Create shall do clean up and return NULL. ]*/
/*Tests_SRS_IOTHUBDEVICETWIN_12_009: [ If the mallocAndStrcpy_s fails, IoTHubDeviceTwin_Create shall do clean up and return NULL. ]*/
/*Tests_SRS_IOTHUBDEVICETWIN_12_011: [ If the mallocAndStrcpy_s fails, IoTHubDeviceTwin_Create shall do clean up and return NULL. ]*/
/*Tests_SRS_IOTHUBDEVICETWIN_12_013: [ If the mallocAndStrcpy_s fails, IoTHubDeviceTwin_Create shall do clean up and return NULL. ]*/
/*Tests_SRS_IOTHUBDEVICETWIN_12_015: [ If the mallocAndStrcpy_s fails, IoTHubDeviceTwin_Create shall do clean up and return NULL. ]*/
TEST_FUNCTION(IoTHubDeviceTwin_Create_non_happy_path)
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
        IOTHUB_SERVICE_CLIENT_DEVICE_TWIN_HANDLE result = IoTHubDeviceTwin_Create(TEST_IOTHUB_SERVICE_CLIENT_AUTH_HANDLE);

        /// assert
        ASSERT_ARE_EQUAL(void_ptr, NULL, result);

        ///cleanup
    }
    umock_c_negative_tests_deinit();

}

/*Tests_SRS_IOTHUBDEVICETWIN_12_016: [ If the serviceClientDeviceTwinHandle input parameter is NULL IoTHubDeviceTwin_Destroy shall return ]*/
TEST_FUNCTION(IoTHubDeviceTwin_Destroy_return_if_input_parameter_serviceClientDeviceTwinHandle_is_NULL)
{
    // arrange

    // act
    IoTHubDeviceTwin_Destroy(NULL);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
}

/*Tests_SRS_IOTHUBDEVICETWIN_12_017: [ If the serviceClientDeviceTwinHandle input parameter is not NULL IoTHubDeviceTwin_Destroy shall free the memory of it and return ]*/
TEST_FUNCTION(IoTHubDeviceTwin_Destroy_do_clean_up_and_return_if_input_parameter_serviceClientDeviceTwinHandle_is_not_NULL)
{
    // arrange
    IOTHUB_SERVICE_CLIENT_DEVICE_TWIN_HANDLE handle = IoTHubDeviceTwin_Create(TEST_IOTHUB_SERVICE_CLIENT_AUTH_HANDLE);

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
    IoTHubDeviceTwin_Destroy(handle);

    // assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
}

/*Tests_SRS_IOTHUBDEVICETWIN_12_018: [ IoTHubDeviceTwin_GetTwin shall verify the input parameters and if any of them are NULL then return NULL ]*/
TEST_FUNCTION(IoTHubDeviceTwin_GetTwin_return_NULL_if_input_parameter_serviceClientDeviceTwinHandle_is_NULL)
{
    // arrange

    // act
    const char* deviceId = " ";
    char* result = IoTHubDeviceTwin_GetTwin(NULL, deviceId);

    // assert
    ASSERT_IS_NULL(result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
}

/*Tests_SRS_IOTHUBDEVICETWIN_12_018: [ IoTHubDeviceTwin_GetTwin shall verify the input parameters and if any of them are NULL then return NULL ]*/
TEST_FUNCTION(IoTHubDeviceTwin_GetTwin_return_NULL_if_input_parameter_deviceId_is_NULL)
{
    // arrange

    // act
    char* result = IoTHubDeviceTwin_GetTwin(TEST_IOTHUB_SERVICE_CLIENT_DEVICE_TWIN_HANDLE, NULL);

    // assert
    ASSERT_IS_NULL(result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
}

/*Tests_SRS_IOTHUBDEVICETWIN_12_019: [ IoTHubDeviceTwin_GetTwin shall create HTTP GET request URL using the given deviceId using the following format: url/twins/[deviceId] ]*/
/*Tests_SRS_IOTHUBDEVICETWIN_12_020: [ IoTHubDeviceTwin_GetTwin shall add the following headers to the created HTTP GET request: authorization=sasToken,Request-Id=1001,Accept=application/json,Content-Type=application/json,charset=utf-8 ]*/
/*Tests_SRS_IOTHUBDEVICETWIN_12_021: [ IoTHubDeviceTwin_GetTwin shall create an HTTPAPIEX_SAS_HANDLE handle by calling HTTPAPIEX_SAS_Create ]*/
/*Tests_SRS_IOTHUBDEVICETWIN_12_022: [ IoTHubDeviceTwin_GetTwin shall create an HTTPAPIEX_HANDLE handle by calling HTTPAPIEX_Create ]*/
/*Tests_SRS_IOTHUBDEVICETWIN_12_023: [ IoTHubDeviceTwin_GetTwin shall execute the HTTP GET request by calling HTTPAPIEX_ExecuteRequest ]*/
/*Tests_SRS_IOTHUBDEVICETWIN_12_030: [ Otherwise IoTHubDeviceTwin_GetTwin shall save the received deviceTwin to the out parameter and return with it ]*/
TEST_FUNCTION(IoTHubDeviceTwin_GetTwin_happy_path_status_code_200)
{
    // arrange
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

    EXPECTED_CALL(BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));

    EXPECTED_CALL(BUFFER_u_char(IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .SetReturn(TEST_UNSIGNED_CHAR_PTR);

    STRICT_EXPECTED_CALL(BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    // act
    const char* deviceId = " ";
    char* result = IoTHubDeviceTwin_GetTwin(TEST_IOTHUB_SERVICE_CLIENT_DEVICE_TWIN_HANDLE, deviceId);

    // assert
    ASSERT_IS_NOT_NULL(result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    free((void*)result);
}

/*Tests_SRS_IOTHUBDEVICETWIN_12_019: [ IoTHubDeviceTwin_GetTwin shall create HTTP GET request URL using the given deviceId using the following format: url/twins/[deviceId] ]*/
/*Tests_SRS_IOTHUBDEVICETWIN_12_020: [ IoTHubDeviceTwin_GetTwin shall add the following headers to the created HTTP GET request: authorization=sasToken,Request-Id=1001,Accept=application/json,Content-Type=application/json,charset=utf-8 ]*/
/*Tests_SRS_IOTHUBDEVICETWIN_12_021: [ IoTHubDeviceTwin_GetTwin shall create an HTTPAPIEX_SAS_HANDLE handle by calling HTTPAPIEX_SAS_Create ]*/
/*Tests_SRS_IOTHUBDEVICETWIN_12_022: [ IoTHubDeviceTwin_GetTwin shall create an HTTPAPIEX_HANDLE handle by calling HTTPAPIEX_Create ]*/
/*Tests_SRS_IOTHUBDEVICETWIN_12_023: [ IoTHubDeviceTwin_GetTwin shall execute the HTTP GET request by calling HTTPAPIEX_ExecuteRequest ]*/
/*Tests_SRS_IOTHUBDEVICETWIN_12_030: [ Otherwise IoTHubDeviceTwin_GetTwin shall save the received deviceTwin to the out parameter and return with it ]*/
TEST_FUNCTION(IoTHubDeviceTwin_GetTwin_happy_path_status_code_400)
{
    // arrange
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

    EXPECTED_CALL(gballoc_free(IGNORED_NUM_ARG));

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

    // act
    const char* deviceId = " ";
    char* result = IoTHubDeviceTwin_GetTwin(TEST_IOTHUB_SERVICE_CLIENT_DEVICE_TWIN_HANDLE, deviceId);

    // assert
    ASSERT_IS_NULL(result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
}

/*Tests_SRS_IOTHUBDEVICETWIN_12_024: [ If any of the call fails during the HTTP creation IoTHubDeviceTwin_GetTwin shall fail and return NULL ]*/
/*Tests_SRS_IOTHUBDEVICETWIN_12_025: [ If any of the HTTPAPI call fails IoTHubDeviceTwin_GetTwin shall fail and return NULL ]*/
/*Tests_SRS_IOTHUBDEVICETWIN_12_026: [ IoTHubDeviceTwin_GetTwin shall verify the received HTTP status code and if it is not equal to 200 then return NULL ]*/
TEST_FUNCTION(IoTHubDeviceTwin_GetTwin_non_happy_path)
{
    // arrange
    int umockc_result = umock_c_negative_tests_init();
    ASSERT_ARE_EQUAL(int, 0, umockc_result);

    EXPECTED_CALL(BUFFER_new());

    EXPECTED_CALL(STRING_construct(TEST_HOSTNAME));
    EXPECTED_CALL(STRING_construct(TEST_SHAREDACCESSKEY));
    EXPECTED_CALL(STRING_construct(TEST_SHAREDACCESSKEYNAME));

    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));
    EXPECTED_CALL(UniqueId_Generate(IGNORED_PTR_ARG, IGNORED_NUM_ARG));

    EXPECTED_CALL(HTTPHeaders_Alloc());
    EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_AUTHORIZATION, TEST_HTTP_HEADER_VAL_AUTHORIZATION))
        .IgnoreArgument(1);

    EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_REQUEST_ID, TEST_HTTP_HEADER_VAL_REQUEST_ID))
        .IgnoreArgument(1);
    EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_USER_AGENT, IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_ACCEPT, TEST_HTTP_HEADER_VAL_ACCEPT))
        .IgnoreArgument(1);

    EXPECTED_CALL(gballoc_free(IGNORED_NUM_ARG));

    EXPECTED_CALL(HTTPAPIEX_SAS_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreAllArguments();
    EXPECTED_CALL(HTTPAPIEX_Create(TEST_HOSTNAME));

    EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG));

    EXPECTED_CALL(HTTPAPIEX_SAS_ExecuteRequest(IGNORED_PTR_ARG, IGNORED_PTR_ARG, HTTPAPI_REQUEST_GET, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
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

    EXPECTED_CALL(BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
	    .SetReturn(NULL);

    EXPECTED_CALL(BUFFER_u_char(IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .SetReturn(TEST_UNSIGNED_CHAR_PTR);

    STRICT_EXPECTED_CALL(BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    // act
    umock_c_negative_tests_snapshot();

    ///act
    for (size_t i = 0; i < umock_c_negative_tests_call_count(); i++)
    {
        /// arrange
        umock_c_negative_tests_reset();
        umock_c_negative_tests_fail_call(i);

        /// act
        if (
            (i != 7) && /*gballoc_malloc*/
            (i != 13) && /*STRING_c_str*/
            (i != 15) && /*HTTPHeaders_Free*/
            (i != 16) && /*HTTPAPIEX_Destroy*/
            (i != 17) && /*HTTPAPIEX_SAS_Destroy*/
            (i != 18) && /*STRING_delete*/
            (i != 19) && /*STRING_delete*/
            (i != 20) && /*STRING_delete*/
            (i != 21) && /*BUFFER_u_char*/
            (i != 22) && /*mallocAndStrcpy_s*/
            (i != 23)    /*BUFFER_delete*/
            )
        {
            const char* deviceId = " ";
            char* result = IoTHubDeviceTwin_GetTwin(TEST_IOTHUB_SERVICE_CLIENT_DEVICE_TWIN_HANDLE, deviceId);

            /// assert
            ASSERT_IS_NULL(result);
        }

        ///cleanup
    }
    umock_c_negative_tests_deinit();
}

/*Tests_SRS_IOTHUBDEVICETWIN_12_031: [ IoTHubDeviceTwin_UpdateTwin shall verify the input parameters and if any of them are NULL then return NULL ]*/
TEST_FUNCTION(IoTHubDeviceTwin_UpdateTwin_return_NULL_if_input_parameter_serviceClientDeviceTwinHandle_is_NULL)
{
    // arrange

    // act
    const char* deviceId = " ";
    const char* deviceTwinJson = " ";
    char* result = IoTHubDeviceTwin_UpdateTwin(NULL, deviceId, deviceTwinJson);

    // assert
    ASSERT_IS_NULL(result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
}

/*Tests_SRS_IOTHUBDEVICETWIN_12_031: [ IoTHubDeviceTwin_UpdateTwin shall verify the input parameters and if any of them are NULL then return NULL ]*/
TEST_FUNCTION(IoTHubDeviceTwin_UpdateTwin_return_NULL_if_input_parameter_deviceId_is_NULL)
{
    // arrange

    // act
    const char* deviceTwinJson = " ";
    char* result = IoTHubDeviceTwin_UpdateTwin(TEST_IOTHUB_SERVICE_CLIENT_DEVICE_TWIN_HANDLE, NULL, deviceTwinJson);

    // assert
    ASSERT_IS_NULL(result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
}

/*Tests_SRS_IOTHUBDEVICETWIN_12_031: [ IoTHubDeviceTwin_UpdateTwin shall verify the input parameters and if any of them are NULL then return NULL ]*/
TEST_FUNCTION(IoTHubDeviceTwin_UpdateTwin_return_NULL_if_input_parameter_deviceTwinJson_is_NULL)
{
    // arrange

    // act
    const char* deviceId = " ";
    char* result = IoTHubDeviceTwin_UpdateTwin(TEST_IOTHUB_SERVICE_CLIENT_DEVICE_TWIN_HANDLE, deviceId, NULL);

    // assert
    ASSERT_IS_NULL(result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
}

/*Tests_SRS_IOTHUBDEVICETWIN_12_034: [ IoTHubDeviceTwin_UpdateTwin shall allocate memory for response buffer by calling BUFFER_new ]*/
/*Tests_SRS_IOTHUBDEVICETWIN_12_039: [ IoTHubDeviceTwin_UpdateTwin shall create an HTTP PATCH request using deviceTwinJson ]*/
/*Tests_SRS_IOTHUBDEVICETWIN_12_040: [ IoTHubDeviceTwin_UpdateTwin shall create an HTTP PATCH request using the createdfollowing HTTP headers: authorization=sasToken,Request-Id=1001,Accept=application/json,Content-Type=application/json,charset=utf-8 ]*/
/*Tests_SRS_IOTHUBDEVICETWIN_12_041: [ IoTHubDeviceTwin_UpdateTwin shall create an HTTPAPIEX_SAS_HANDLE handle by calling HTTPAPIEX_SAS_Create ]*/
/*Tests_SRS_IOTHUBDEVICETWIN_12_042: [ IoTHubDeviceTwin_UpdateTwin shall create an HTTPAPIEX_HANDLE handle by calling HTTPAPIEX_Create ]*/
/*Tests_SRS_IOTHUBDEVICETWIN_12_043: [ IoTHubDeviceTwin_UpdateTwin shall execute the HTTP PATCH request by calling HTTPAPIEX_ExecuteRequest ]*/
/*Tests_SRS_IOTHUBDEVICETWIN_12_047: [ Otherwise IoTHubDeviceTwin_UpdateTwin shall save the received updated device twin to the out parameter and return with it ]*/
TEST_FUNCTION(IoTHubDeviceTwin_UpdateTwin_happy_path_status_code_200)
{
    // arrange
    EXPECTED_CALL(BUFFER_create(IGNORED_PTR_ARG, IGNORED_NUM_ARG))
        .IgnoreAllArguments();

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
    EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_ACCEPT, TEST_HTTP_HEADER_VAL_ACCEPT))
        .IgnoreArgument(1);

    EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));

    EXPECTED_CALL(HTTPAPIEX_SAS_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreAllArguments();
    EXPECTED_CALL(HTTPAPIEX_Create(TEST_HOSTNAME));

    EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG));

    EXPECTED_CALL(HTTPAPIEX_SAS_ExecuteRequest(IGNORED_PTR_ARG, IGNORED_PTR_ARG, HTTPAPI_REQUEST_GET, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
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

    EXPECTED_CALL(BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG));

    EXPECTED_CALL(BUFFER_u_char(IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .SetReturn(TEST_UNSIGNED_CHAR_PTR);

    STRICT_EXPECTED_CALL(BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    // act
    const char* deviceId = " ";
    const char* deviceTwinJson = " ";
    char* result = IoTHubDeviceTwin_UpdateTwin(TEST_IOTHUB_SERVICE_CLIENT_DEVICE_TWIN_HANDLE, deviceId, deviceTwinJson);

    // assert
    ASSERT_IS_NOT_NULL(result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    // cleanup
    free((void*)result);
}

/*Tests_SRS_IOTHUBDEVICETWIN_12_034: [ IoTHubDeviceTwin_UpdateTwin shall allocate memory for response buffer by calling BUFFER_new ]*/
/*Tests_SRS_IOTHUBDEVICETWIN_12_039: [ IoTHubDeviceTwin_UpdateTwin shall create an HTTP PATCH request using deviceTwinJson ]*/
/*Tests_SRS_IOTHUBDEVICETWIN_12_040: [ IoTHubDeviceTwin_UpdateTwin shall create an HTTP PATCH request using the createdfollowing HTTP headers: authorization=sasToken,Request-Id=1001,Accept=application/json,Content-Type=application/json,charset=utf-8 ]*/
/*Tests_SRS_IOTHUBDEVICETWIN_12_041: [ IoTHubDeviceTwin_UpdateTwin shall create an HTTPAPIEX_SAS_HANDLE handle by calling HTTPAPIEX_SAS_Create ]*/
/*Tests_SRS_IOTHUBDEVICETWIN_12_042: [ IoTHubDeviceTwin_UpdateTwin shall create an HTTPAPIEX_HANDLE handle by calling HTTPAPIEX_Create ]*/
/*Tests_SRS_IOTHUBDEVICETWIN_12_043: [ IoTHubDeviceTwin_UpdateTwin shall execute the HTTP PATCH request by calling HTTPAPIEX_ExecuteRequest ]*/
/*Tests_SRS_IOTHUBDEVICETWIN_12_047: [ Otherwise IoTHubDeviceTwin_UpdateTwin shall save the received updated device twin to the out parameter and return with it ]*/
TEST_FUNCTION(IoTHubDeviceTwin_UpdateTwin_happy_path_status_code_400)
{
    // arrange
    EXPECTED_CALL(BUFFER_create(IGNORED_PTR_ARG, IGNORED_NUM_ARG))
        .IgnoreAllArguments();

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
    const char* deviceId = " ";
    const char* deviceTwinJson = " ";
    char* result = IoTHubDeviceTwin_UpdateTwin(TEST_IOTHUB_SERVICE_CLIENT_DEVICE_TWIN_HANDLE, deviceId, deviceTwinJson);

    // assert
    ASSERT_IS_NULL(result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
}

/*Tests_SRS_IOTHUBDEVICETWIN_12_033: [ If the creation fails, IoTHubDeviceTwin_UpdateTwin shall return NULL ]*/
/*Tests_SRS_IOTHUBDEVICETWIN_12_035: [ If the allocation failed, IoTHubDeviceTwin_UpdateTwin shall return NULL ]*/
/*Tests_SRS_IOTHUBDEVICETWIN_12_044: [ If any of the call fails during the HTTP creation IoTHubDeviceTwin_UpdateTwin shall fail and return NULL ]*/
/*Tests_SRS_IOTHUBDEVICETWIN_12_045: [ If any of the HTTPAPI call fails IoTHubDeviceTwin_UpdateTwin shall fail and return NULL ]*/
/*Tests_SRS_IOTHUBDEVICETWIN_12_046: [ IoTHubDeviceTwin_UpdateTwin shall verify the received HTTP status code and if it is not equal to 200 then return NULL ]*/
TEST_FUNCTION(IoTHubDeviceTwin_UpdateTwin_non_happy_path)
{
    // arrange
    int umockc_result = umock_c_negative_tests_init();
    ASSERT_ARE_EQUAL(int, 0, umockc_result);

    EXPECTED_CALL(BUFFER_create(IGNORED_PTR_ARG, IGNORED_NUM_ARG))
        .IgnoreAllArguments();

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
    EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, TEST_HTTP_HEADER_KEY_ACCEPT, TEST_HTTP_HEADER_VAL_ACCEPT))
        .IgnoreArgument(1);

    EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG));

    EXPECTED_CALL(HTTPAPIEX_SAS_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .IgnoreAllArguments();
    EXPECTED_CALL(HTTPAPIEX_Create(TEST_HOSTNAME));

    EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG));

    EXPECTED_CALL(HTTPAPIEX_SAS_ExecuteRequest(IGNORED_PTR_ARG, IGNORED_PTR_ARG, HTTPAPI_REQUEST_GET, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
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


    EXPECTED_CALL(BUFFER_length(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG)
	    .SetReturn(NULL));

    EXPECTED_CALL(BUFFER_u_char(IGNORED_PTR_ARG))
        .IgnoreArgument(1)
        .SetReturn(TEST_UNSIGNED_CHAR_PTR);

    STRICT_EXPECTED_CALL(BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    STRICT_EXPECTED_CALL(BUFFER_delete(IGNORED_PTR_ARG))
        .IgnoreArgument(1);

    // act
    umock_c_negative_tests_snapshot();

    ///act
    for (size_t i = 0; i < umock_c_negative_tests_call_count(); i++)
    {
        /// arrange
        umock_c_negative_tests_reset();
        umock_c_negative_tests_fail_call(i);

        /// act
        if (
            (i != 7)  && /*gballoc_malloc*/
            (i != 8)  && /*UniqueId_Generate*/
            (i != 13) && /*gballoc_free*/
            (i != 16) && /*STRING_c_str*/
            (i != 18) && /*STRING_delete*/
            (i != 19) && /*STRING_delete*/
            (i != 20) && /*STRING_delete*/
            (i != 21) && /*BUFFER_u_char*/
            (i != 22) && /*mallocAndStrcpy_s*/
            (i != 23) && /*BUFFER_delete*/
            (i != 24) && /*BUFFER_delete*/
            (i != 25) && /*BUFFER_delete*/
            (i != 27) && /*BUFFER_delete*/
            (i != 28)    /*BUFFER_delete*/
            )
        {
            const char* deviceId = " ";
            const char* deviceTwinJson = " ";
            char* result = IoTHubDeviceTwin_UpdateTwin(TEST_IOTHUB_SERVICE_CLIENT_DEVICE_TWIN_HANDLE, deviceId, deviceTwinJson);

            /// assert
            ASSERT_IS_NULL(result);
        }

        ///cleanup
    }
    umock_c_negative_tests_deinit();
}

END_TEST_SUITE(iothub_devicetwin_ut)
