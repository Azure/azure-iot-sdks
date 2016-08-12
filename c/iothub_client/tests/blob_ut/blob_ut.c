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

#define ENABLE_MOCKS
#include "azure_c_shared_utility/httpapiex.h"
#include "azure_c_shared_utility/buffer_.h"
#include "azure_c_shared_utility/strings.h"
#include "azure_c_shared_utility/base64.h"
#include "azure_c_shared_utility/httpheaders.h"
#include "azure_c_shared_utility/gballoc.h"
#undef ENABLE_MOCKS

#include "blob.h"
#include "testrunnerswitcher.h"
#include "umock_c.h"
#include "umocktypes_charptr.h"
#include "umock_c_negative_tests.h"

/*helps when enums are not matched*/
#ifdef malloc
#undef malloc
#endif

TEST_DEFINE_ENUM_TYPE(HTTPAPI_REQUEST_TYPE, HTTPAPI_REQUEST_TYPE_VALUES);
IMPLEMENT_UMOCK_C_ENUM_TYPE(HTTPAPI_REQUEST_TYPE, HTTPAPI_REQUEST_TYPE_VALUES);

TEST_DEFINE_ENUM_TYPE(HTTPAPIEX_RESULT, HTTPAPIEX_RESULT_VALUES);
IMPLEMENT_UMOCK_C_ENUM_TYPE(HTTPAPIEX_RESULT, HTTPAPIEX_RESULT_VALUES);

TEST_DEFINE_ENUM_TYPE(HTTP_HEADERS_RESULT, HTTP_HEADERS_RESULT_VALUES);
IMPLEMENT_UMOCK_C_ENUM_TYPE(HTTP_HEADERS_RESULT, HTTP_HEADERS_RESULT_VALUES);

static HTTPAPIEX_HANDLE my_HTTPAPIEX_Create(const char* hostName)
{
    (void)hostName;
    return (HTTPAPIEX_HANDLE)my_gballoc_malloc(1);
}

static void my_HTTPAPIEX_Destroy(HTTPAPIEX_HANDLE handle)
{
    my_gballoc_free(handle);
}

static BUFFER_HANDLE my_BUFFER_create(const unsigned char* source, size_t size)
{
    (void)source, size;
    return (BUFFER_HANDLE)my_gballoc_malloc(1);
}

static void my_BUFFER_delete(BUFFER_HANDLE h)
{
    my_gballoc_free(h);
}

static HTTP_HEADERS_HANDLE my_HTTPHeaders_Alloc(void)
{
    return (HTTP_HEADERS_HANDLE)my_gballoc_malloc(1);
}

static void my_HTTPHeaders_Free(HTTP_HEADERS_HANDLE h)
{
    my_gballoc_free(h);
}

static STRING_HANDLE my_STRING_construct(const char* psz)
{
    char* temp = (char*)my_gballoc_malloc(strlen(psz) + 1);
    ASSERT_IS_NOT_NULL(temp);
    memcpy(temp, psz, strlen(psz) + 1);
    return (STRING_HANDLE)temp;
}

static void my_STRING_delete(STRING_HANDLE h)
{
    my_gballoc_free((void*)h);
}

static STRING_HANDLE my_Base64_Encode_Bytes(const unsigned char* source, size_t size)
{
    (void)source, size;
    return (STRING_HANDLE)my_gballoc_malloc(1);
}

TEST_DEFINE_ENUM_TYPE(BLOB_RESULT, BLOB_RESULT_VALUES);

static TEST_MUTEX_HANDLE g_dllByDll;

#define TEST_HTTPCOLONBACKSLASHBACKSLACH "http://"
#define TEST_HOSTNAME_1 "host.name"
#define TEST_RELATIVE_PATH_1 "/here/follows/something?param1=value1&param2=value2"
#define TEST_VALID_SASURI_1 TEST_HTTPCOLONBACKSLASHBACKSLACH TEST_HOSTNAME_1 TEST_RELATIVE_PATH_1

#define X_MS_BLOB_TYPE "x-ms-blob-type"
#define BLOCK_BLOB "BlockBlob"

DEFINE_ENUM_STRINGS(UMOCK_C_ERROR_CODE, UMOCK_C_ERROR_CODE_VALUES)

static void on_umock_c_error(UMOCK_C_ERROR_CODE error_code)
{
    char temp_str[256];
    (void)snprintf(temp_str, sizeof(temp_str), "umock_c reported error :%s", ENUM_TO_STRING(UMOCK_C_ERROR_CODE, error_code));
    ASSERT_FAIL(temp_str);
}

static BUFFER_HANDLE testValidBufferHandle; /*assigned in TEST_SUITE_INITIALIZE*/
static unsigned int httpResponse; /*used as out parameter in every call to Blob_....*/
static const unsigned int TwoHundred = 200;
static const unsigned int FourHundredFour = 404;


BEGIN_TEST_SUITE(blob_ut)

TEST_SUITE_INITIALIZE(TestSuiteInitialize)
{
    TEST_INITIALIZE_MEMORY_DEBUG(g_dllByDll);
    (void)umock_c_init(on_umock_c_error);

    (void)umocktypes_charptr_register_types();

    REGISTER_GLOBAL_MOCK_HOOK(gballoc_malloc, my_gballoc_malloc);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(gballoc_malloc, NULL);
    REGISTER_GLOBAL_MOCK_HOOK(gballoc_free, my_gballoc_free);

    REGISTER_GLOBAL_MOCK_HOOK(HTTPAPIEX_Create, my_HTTPAPIEX_Create);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(HTTPAPIEX_Create, NULL);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(HTTPAPIEX_ExecuteRequest, HTTPAPIEX_ERROR);
    REGISTER_GLOBAL_MOCK_HOOK(HTTPAPIEX_Destroy, my_HTTPAPIEX_Destroy);

    REGISTER_GLOBAL_MOCK_HOOK(BUFFER_create, my_BUFFER_create);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(BUFFER_create, NULL);
    REGISTER_GLOBAL_MOCK_HOOK(BUFFER_delete, my_BUFFER_delete);

    REGISTER_GLOBAL_MOCK_HOOK(HTTPHeaders_Alloc, my_HTTPHeaders_Alloc);
    REGISTER_GLOBAL_MOCK_HOOK(HTTPHeaders_Free, my_HTTPHeaders_Free);

    REGISTER_GLOBAL_MOCK_HOOK(STRING_construct, my_STRING_construct);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(STRING_construct, NULL);
    REGISTER_GLOBAL_MOCK_HOOK(Base64_Encode_Bytes, my_Base64_Encode_Bytes);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(Base64_Encode_Bytes, NULL);

    REGISTER_GLOBAL_MOCK_FAIL_RETURN(STRING_concat, __LINE__);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(STRING_concat_with_STRING, __LINE__);

    REGISTER_GLOBAL_MOCK_RETURN(STRING_c_str, "a");
    REGISTER_GLOBAL_MOCK_HOOK(STRING_delete, my_STRING_delete);





    REGISTER_UMOCK_ALIAS_TYPE(HTTP_HEADERS_HANDLE, void*);
    REGISTER_UMOCK_ALIAS_TYPE(HTTPAPIEX_HANDLE, void*);

    REGISTER_UMOCK_ALIAS_TYPE(BUFFER_HANDLE, void*);
    REGISTER_UMOCK_ALIAS_TYPE(STRING_HANDLE, void*);

    REGISTER_TYPE(HTTPAPI_REQUEST_TYPE, HTTPAPI_REQUEST_TYPE);
    REGISTER_TYPE(HTTPAPIEX_RESULT, HTTPAPIEX_RESULT);
    REGISTER_TYPE(HTTP_HEADERS_RESULT, HTTP_HEADERS_RESULT);

    testValidBufferHandle = BUFFER_create((const unsigned char*)"a", 1);
    ASSERT_IS_NOT_NULL(testValidBufferHandle);
    umock_c_reset_all_calls();
}

TEST_SUITE_CLEANUP(TestClassCleanup)
{

    BUFFER_delete(testValidBufferHandle);

    umock_c_deinit();

    TEST_DEINITIALIZE_MEMORY_DEBUG(g_dllByDll);
}

TEST_FUNCTION_INITIALIZE(Setup)
{
    umock_c_reset_all_calls();
}

/*Tests_SRS_BLOB_02_001: [ If SASURI is NULL then Blob_UploadFromSasUri shall fail and return BLOB_INVALID_ARG. ]*/
TEST_FUNCTION(Blob_UploadFromSasUri_with_NULL_SasUri_fails)
{
    ///arrange
    unsigned char c = '3';

    ///act
    BLOB_RESULT result = Blob_UploadFromSasUri(NULL, &c, sizeof(c), &httpResponse, testValidBufferHandle);

    ///assert
    ASSERT_ARE_EQUAL(BLOB_RESULT, BLOB_INVALID_ARG, result);

    ///cleanup

}

/*Tests_SRS_BLOB_02_002: [ If source is NULL and size is not zero then Blob_UploadFromSasUri shall fail and return BLOB_INVALID_ARG. ]*/
TEST_FUNCTION(Blob_UploadFromSasUri_with_NULL_source_and_non_zero_size_fails)
{
    ///arrange

    ///act

    BLOB_RESULT result = Blob_UploadFromSasUri(TEST_VALID_SASURI_1, NULL, 1, &httpResponse, testValidBufferHandle);

    ///assert
    ASSERT_ARE_EQUAL(BLOB_RESULT, BLOB_INVALID_ARG, result);

    ///cleanup

}

/*Tests_SRS_BLOB_02_004: [ Blob_UploadFromSasUri shall copy from SASURI the hostname to a new const char*. ]*/
/*Tests_SRS_BLOB_02_006: [ Blob_UploadFromSasUri shall create a new HTTPAPI_EX_HANDLE by calling HTTPAPIEX_Create passing the hostname. ]*/
/*Tests_SRS_BLOB_02_008: [ Blob_UploadFromSasUri shall compute the relative path of the request from the SASURI parameter. ]*/
/*Tests_SRS_BLOB_02_009: [ Blob_UploadFromSasUri shall create an HTTP_HEADERS_HANDLE for the request HTTP headers carrying the following headers: ]*/
/*Tests_SRS_BLOB_02_010: [ Blob_UploadFromSasUri shall create a BUFFER_HANDLE from source and size parameters. ]*/
/*Tests_SRS_BLOB_02_012: [ Blob_UploadFromSasUri shall call HTTPAPIEX_ExecuteRequest passing the parameters previously build, httpStatus and httpResponse ]*/
/*Tests_SRS_BLOB_02_015: [ Otherwise, HTTPAPIEX_ExecuteRequest shall succeed and return BLOB_OK. ]*/
TEST_FUNCTION(Blob_UploadFromSasUri_happy_path)
{
    ///arrange
    unsigned char c = '3';

    STRICT_EXPECTED_CALL(gballoc_malloc(strlen(TEST_HOSTNAME_1) + 1));
    {
        STRICT_EXPECTED_CALL(HTTPAPIEX_Create(TEST_HOSTNAME_1));
        {
            STRICT_EXPECTED_CALL(BUFFER_create(&c, 1));
            {
                STRICT_EXPECTED_CALL(HTTPHeaders_Alloc());
                {
                    int responseCode = 200; /*everything is good*/
                    STRICT_EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, X_MS_BLOB_TYPE, BLOCK_BLOB))
                        .IgnoreArgument_httpHeadersHandle();

                    STRICT_EXPECTED_CALL(HTTPAPIEX_ExecuteRequest(IGNORED_PTR_ARG, HTTPAPI_REQUEST_PUT, TEST_RELATIVE_PATH_1, IGNORED_PTR_ARG, IGNORED_PTR_ARG, &httpResponse, NULL, testValidBufferHandle))
                        .IgnoreArgument_handle()
                        .IgnoreArgument_requestHttpHeadersHandle()
                        .IgnoreArgument_requestContent()
                        .CopyOutArgumentBuffer_statusCode(&responseCode, sizeof(responseCode))
                        .SetReturn(HTTPAPIEX_OK)
                        ;

                    STRICT_EXPECTED_CALL(HTTPHeaders_Free(IGNORED_PTR_ARG))
                        .IgnoreArgument_httpHeadersHandle();
                }
                STRICT_EXPECTED_CALL(BUFFER_delete(IGNORED_PTR_ARG))
                    .IgnoreArgument_handle();
            }
            STRICT_EXPECTED_CALL(HTTPAPIEX_Destroy(IGNORED_PTR_ARG))
                .IgnoreArgument_handle();
        }
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument_ptr();
    }

    ///act
    BLOB_RESULT result = Blob_UploadFromSasUri(TEST_VALID_SASURI_1, &c, sizeof(c), &httpResponse, testValidBufferHandle);

    ///assert
    ASSERT_ARE_EQUAL(BLOB_RESULT, BLOB_OK, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    ///cleanup
}

/*Tests_SRS_BLOB_02_015: [ Otherwise, HTTPAPIEX_ExecuteRequest shall succeed and return BLOB_OK. ]*/
TEST_FUNCTION(Blob_UploadFromSasUri_succeeds_when_HTTP_status_code_is_404)
{
    ///arrange
    unsigned char c = '3';

    STRICT_EXPECTED_CALL(gballoc_malloc(strlen(TEST_HOSTNAME_1) + 1));
    {
        STRICT_EXPECTED_CALL(HTTPAPIEX_Create(TEST_HOSTNAME_1));
        {
            STRICT_EXPECTED_CALL(BUFFER_create(&c, 1));
            {
                STRICT_EXPECTED_CALL(HTTPHeaders_Alloc());
                {
                    int responseCode = 404; /*not found*/
                    STRICT_EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, X_MS_BLOB_TYPE, BLOCK_BLOB))
                        .IgnoreArgument_httpHeadersHandle();

                    STRICT_EXPECTED_CALL(HTTPAPIEX_ExecuteRequest(IGNORED_PTR_ARG, HTTPAPI_REQUEST_PUT, TEST_RELATIVE_PATH_1, IGNORED_PTR_ARG, IGNORED_PTR_ARG, &httpResponse, NULL, testValidBufferHandle))
                        .IgnoreArgument_handle()
                        .IgnoreArgument_requestHttpHeadersHandle()
                        .IgnoreArgument_requestContent()
                        .CopyOutArgumentBuffer_statusCode(&responseCode, sizeof(responseCode))
                        .SetReturn(HTTPAPIEX_OK)
                        ;

                    STRICT_EXPECTED_CALL(HTTPHeaders_Free(IGNORED_PTR_ARG))
                        .IgnoreArgument_httpHeadersHandle();
                }
                STRICT_EXPECTED_CALL(BUFFER_delete(IGNORED_PTR_ARG))
                    .IgnoreArgument_handle();
            }
            STRICT_EXPECTED_CALL(HTTPAPIEX_Destroy(IGNORED_PTR_ARG))
                .IgnoreArgument_handle();
        }
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument_ptr();
    }

    ///act
    BLOB_RESULT result = Blob_UploadFromSasUri(TEST_VALID_SASURI_1, &c, sizeof(c), &httpResponse, testValidBufferHandle);

    ///assert
    ASSERT_ARE_EQUAL(BLOB_RESULT, BLOB_OK, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    ///cleanup
}

/*Tests_SRS_BLOB_02_013: [ If HTTPAPIEX_ExecuteRequest fails, then Blob_UploadFromSasUri shall fail and return BLOB_HTTP_ERROR. ]*/
TEST_FUNCTION(Blob_UploadFromSasUri_fails_when_HTTPAPIEX_ExecuteRequest_fails)
{
    ///arrange
    unsigned char c = '3';

    STRICT_EXPECTED_CALL(gballoc_malloc(strlen(TEST_HOSTNAME_1) + 1));
    {
        STRICT_EXPECTED_CALL(HTTPAPIEX_Create(TEST_HOSTNAME_1));
        {
            STRICT_EXPECTED_CALL(BUFFER_create(&c, 1));
            {
                STRICT_EXPECTED_CALL(HTTPHeaders_Alloc());
                {
                    int responseCode = 200; /*ok*/
                    STRICT_EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, X_MS_BLOB_TYPE, BLOCK_BLOB))
                        .IgnoreArgument_httpHeadersHandle();

                    STRICT_EXPECTED_CALL(HTTPAPIEX_ExecuteRequest(IGNORED_PTR_ARG, HTTPAPI_REQUEST_PUT, TEST_RELATIVE_PATH_1, IGNORED_PTR_ARG, IGNORED_PTR_ARG, &httpResponse, NULL, testValidBufferHandle))
                        .IgnoreArgument_handle()
                        .IgnoreArgument_requestHttpHeadersHandle()
                        .IgnoreArgument_requestContent()
                        .CopyOutArgumentBuffer_statusCode(&responseCode, sizeof(responseCode))
                        .SetReturn(HTTPAPIEX_ERROR)
                        ;

                    STRICT_EXPECTED_CALL(HTTPHeaders_Free(IGNORED_PTR_ARG))
                        .IgnoreArgument_httpHeadersHandle();
                }
                STRICT_EXPECTED_CALL(BUFFER_delete(IGNORED_PTR_ARG))
                    .IgnoreArgument_handle();
            }
            STRICT_EXPECTED_CALL(HTTPAPIEX_Destroy(IGNORED_PTR_ARG))
                .IgnoreArgument_handle();
        }
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument_ptr();
    }

    ///act
    BLOB_RESULT result = Blob_UploadFromSasUri(TEST_VALID_SASURI_1, &c, sizeof(c), &httpResponse, testValidBufferHandle);

    ///assert
    ASSERT_ARE_EQUAL(BLOB_RESULT, BLOB_HTTP_ERROR, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    ///cleanup
}

/*Tests_SRS_BLOB_02_011: [ If any of the previous steps related to building the HTTPAPI_EX_ExecuteRequest parameters fails, then Blob_UploadFromSasUri shall fail and return BLOB_ERROR. ]*/
TEST_FUNCTION(Blob_UploadFromSasUri_fails_when_HTTPHeaders_AddHeaderNameValuePair_fails)
{
    ///arrange
    unsigned char c = '3';

    STRICT_EXPECTED_CALL(gballoc_malloc(strlen(TEST_HOSTNAME_1) + 1));
    {
        STRICT_EXPECTED_CALL(HTTPAPIEX_Create(TEST_HOSTNAME_1));
        {
            STRICT_EXPECTED_CALL(BUFFER_create(&c, 1));
            {
                STRICT_EXPECTED_CALL(HTTPHeaders_Alloc());
                {
                    STRICT_EXPECTED_CALL(HTTPHeaders_AddHeaderNameValuePair(IGNORED_PTR_ARG, X_MS_BLOB_TYPE, BLOCK_BLOB))
                        .IgnoreArgument_httpHeadersHandle()
                        .SetReturn(HTTP_HEADERS_ERROR)
                        ;

                    STRICT_EXPECTED_CALL(HTTPHeaders_Free(IGNORED_PTR_ARG))
                        .IgnoreArgument_httpHeadersHandle();
                }
                STRICT_EXPECTED_CALL(BUFFER_delete(IGNORED_PTR_ARG))
                    .IgnoreArgument_handle();
            }
            STRICT_EXPECTED_CALL(HTTPAPIEX_Destroy(IGNORED_PTR_ARG))
                .IgnoreArgument_handle();
        }
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument_ptr();
    }

    ///act
    BLOB_RESULT result = Blob_UploadFromSasUri(TEST_VALID_SASURI_1, &c, sizeof(c), &httpResponse, testValidBufferHandle);

    ///assert
    ASSERT_ARE_EQUAL(BLOB_RESULT, BLOB_ERROR, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    ///cleanup
}

/*Tests_SRS_BLOB_02_011: [ If any of the previous steps related to building the HTTPAPI_EX_ExecuteRequest parameters fails, then Blob_UploadFromSasUri shall fail and return BLOB_ERROR. ]*/
TEST_FUNCTION(Blob_UploadFromSasUri_fails_when_HTTPHeaders_Alloc_fails)
{
    ///arrange
    unsigned char c = '3';

    STRICT_EXPECTED_CALL(gballoc_malloc(strlen(TEST_HOSTNAME_1) + 1));
    {
        STRICT_EXPECTED_CALL(HTTPAPIEX_Create(TEST_HOSTNAME_1));
        {
            STRICT_EXPECTED_CALL(BUFFER_create(&c, 1));
            {
                STRICT_EXPECTED_CALL(HTTPHeaders_Alloc())
                    .SetReturn(NULL)
                    ;

                STRICT_EXPECTED_CALL(BUFFER_delete(IGNORED_PTR_ARG))
                    .IgnoreArgument_handle();
            }
            STRICT_EXPECTED_CALL(HTTPAPIEX_Destroy(IGNORED_PTR_ARG))
                .IgnoreArgument_handle();
        }
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument_ptr();
    }

    ///act
    BLOB_RESULT result = Blob_UploadFromSasUri(TEST_VALID_SASURI_1, &c, sizeof(c), &httpResponse, testValidBufferHandle);

    ///assert
    ASSERT_ARE_EQUAL(BLOB_RESULT, BLOB_ERROR, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    ///cleanup
}

/*Tests_SRS_BLOB_02_011: [ If any of the previous steps related to building the HTTPAPI_EX_ExecuteRequest parameters fails, then Blob_UploadFromSasUri shall fail and return BLOB_ERROR. ]*/
TEST_FUNCTION(Blob_UploadFromSasUri_fails_when_BUFFER_create_fails)
{
    ///arrange
    unsigned char c = '3';

    STRICT_EXPECTED_CALL(gballoc_malloc(strlen(TEST_HOSTNAME_1) + 1));
    {
        STRICT_EXPECTED_CALL(HTTPAPIEX_Create(TEST_HOSTNAME_1));
        {
            STRICT_EXPECTED_CALL(BUFFER_create(&c, 1))
                .SetReturn(NULL)
                ;

            STRICT_EXPECTED_CALL(HTTPAPIEX_Destroy(IGNORED_PTR_ARG))
                .IgnoreArgument_handle();
        }
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument_ptr();
    }

    ///act
    BLOB_RESULT result = Blob_UploadFromSasUri(TEST_VALID_SASURI_1, &c, sizeof(c), &httpResponse, testValidBufferHandle);

    ///assert
    ASSERT_ARE_EQUAL(BLOB_RESULT, BLOB_ERROR, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    ///cleanup
}

/*Tests_SRS_BLOB_02_007: [ If HTTPAPIEX_Create fails then Blob_UploadFromSasUri shall fail and return BLOB_ERROR. ]*/
TEST_FUNCTION(Blob_UploadFromSasUri_fails_when_HTTPAPIEX_Create_fails)
{
    ///arrange
    unsigned char c = '3';

    STRICT_EXPECTED_CALL(gballoc_malloc(strlen(TEST_HOSTNAME_1) + 1));
    {
        STRICT_EXPECTED_CALL(HTTPAPIEX_Create(TEST_HOSTNAME_1))
            .SetReturn(NULL)
            ;
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument_ptr();
    }

    ///act
    BLOB_RESULT result = Blob_UploadFromSasUri(TEST_VALID_SASURI_1, &c, sizeof(c), &httpResponse, testValidBufferHandle);

    ///assert
    ASSERT_ARE_EQUAL(BLOB_RESULT, BLOB_ERROR, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    ///cleanup
}

/*Tests_SRS_BLOB_02_016: [ If the hostname copy cannot be made then then Blob_UploadFromSasUri shall fail and return BLOB_ERROR ]*/
TEST_FUNCTION(Blob_UploadFromSasUri_fails_when_malloc_fails)
{
    ///arrange
    unsigned char c = '3';

    STRICT_EXPECTED_CALL(gballoc_malloc(strlen(TEST_HOSTNAME_1) + 1))
        .SetReturn(NULL)
        ;

    ///act
    BLOB_RESULT result = Blob_UploadFromSasUri(TEST_VALID_SASURI_1, &c, sizeof(c), &httpResponse, testValidBufferHandle);

    ///assert
    ASSERT_ARE_EQUAL(BLOB_RESULT, BLOB_ERROR, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    ///cleanup
}

/*Tests_SRS_BLOB_02_005: [ If the hostname cannot be determined, then Blob_UploadFromSasUri shall fail and return BLOB_INVALID_ARG. ]*/
TEST_FUNCTION(Blob_UploadFromSasUri_when_SasUri_is_wrong_fails_1)
{
    ///arrange
    unsigned char c = '3';

    ///act
    BLOB_RESULT result = Blob_UploadFromSasUri("https:/h.h/doms", &c, sizeof(c), &httpResponse, testValidBufferHandle); /*wrong format for protocol, notice it is actually http:\h.h\doms (missing a \ from http)*/

    ///assert
    ASSERT_ARE_EQUAL(BLOB_RESULT, BLOB_INVALID_ARG, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    ///cleanup
}

/*Tests_SRS_BLOB_02_005: [ If the hostname cannot be determined, then Blob_UploadFromSasUri shall fail and return BLOB_INVALID_ARG. ]*/
TEST_FUNCTION(Blob_UploadFromSasUri_when_SasUri_is_wrong_fails_2)
{
    ///arrange
    unsigned char c = '3';

    ///act
    BLOB_RESULT result = Blob_UploadFromSasUri("https://h.h", &c, sizeof(c), &httpResponse, testValidBufferHandle); /*there's no relative path here*/

    ///assert
    ASSERT_ARE_EQUAL(BLOB_RESULT, BLOB_INVALID_ARG, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    ///cleanup
}

/*Tests_SRS_BLOB_02_017: [ Blob_UploadFromSasUri shall copy from SASURI the hostname to a new const char* ]*/
/*Tests_SRS_BLOB_02_018: [ Blob_UploadFromSasUri shall create a new HTTPAPI_EX_HANDLE by calling HTTPAPIEX_Create passing the hostname. ]*/
/*Tests_SRS_BLOB_02_019: [ Blob_UploadFromSasUri shall compute the base relative path of the request from the SASURI parameter. ]*/
/*Tests_SRS_BLOB_02_021: [ For every block of 4MB the following operations shall happen: ]*/
/*Tests_SRS_BLOB_02_020: [ Blob_UploadFromSasUri shall construct a BASE64 encoded string from the block ID (000000... 049999) ]*/
/*Tests_SRS_BLOB_02_022: [ Blob_UploadFromSasUri shall construct a new relativePath from following string: base relativePath + "&comp=block&blockid=BASE64 encoded string of blockId" ]*/
/*Tests_SRS_BLOB_02_023: [ Blob_UploadFromSasUri shall create a BUFFER_HANDLE from source and size parameters. ]*/
/*Tests_SRS_BLOB_02_024: [ Blob_UploadFromSasUri shall call HTTPAPIEX_ExecuteRequest with a PUT operation, passing httpStatus and httpResponse. ]*/
/*Tests_SRS_BLOB_02_025: [ If HTTPAPIEX_ExecuteRequest fails then Blob_UploadFromSasUri shall fail and return BLOB_HTTP_ERROR. ]*/
/*Tests_SRS_BLOB_02_027: [ Otherwise Blob_UploadFromSasUri shall continue execution. ]*/
/*Tests_SRS_BLOB_02_028: [ Blob_UploadFromSasUri shall construct an XML string with the following content: ]*/
/*Tests_SRS_BLOB_02_029: [ Blob_UploadFromSasUri shall construct a new relativePath from following string: base relativePath + "&comp=blocklist" ]*/
/*Tests_SRS_BLOB_02_030: [ Blob_UploadFromSasUri shall call HTTPAPIEX_ExecuteRequest with a PUT operation, passing the new relativePath, httpStatus and httpResponse and the XML string as content. ]*/
/*Tests_SRS_BLOB_02_032: [ Otherwise, Blob_UploadFromSasUri shall succeed and return BLOB_OK. ]*/
TEST_FUNCTION(Blob_UploadFromSasUri_various_sizes_happy_path)
{
    /*the following sizes have been identified as "important to be tested*/
    /*64MB, 64MB+1, 68MB-1, 68MB, 68MB+1*/
    size_t sizes[] = {

        64 * 1024 * 1024,
        64 * 1024 * 1024 + 1,

        68 * 1024 * 1024 - 1,
        68 * 1024 * 1024,
        68 * 1024 * 1024 + 1,
    };

    for (size_t iSize = 0; iSize < sizeof(sizes) / sizeof(sizes[0]);iSize++)
    {
        umock_c_reset_all_calls();
        ///arrange
        unsigned char * content = (unsigned char*)gballoc_malloc(sizes[iSize]);
        ASSERT_IS_NOT_NULL(content);

        umock_c_reset_all_calls();

        memset(content, '3', sizes[iSize]);
        content[0] = '0';
        content[sizes[iSize] - 1] = '4';

        STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG)) /*this is creating a copy of the hostname */
            .IgnoreArgument_size();

        STRICT_EXPECTED_CALL(HTTPAPIEX_Create("h.h")); /*this is creating the httpapiex handle to storage (it is always the same host)*/
        STRICT_EXPECTED_CALL(STRING_construct("<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n<BlockList>")); /*this is starting to build the XML used in Put Block List operation*/

        /*uploading blocks (Put Block)*/
        for (size_t blockNumber = 0;blockNumber < (sizes[iSize] - 1) / (4 * 1024 * 1024) + 1;blockNumber++)
        {
            /*here some sprintf happens and that produces a string in the form: 000000...049999*/
            STRICT_EXPECTED_CALL(Base64_Encode_Bytes(IGNORED_PTR_ARG, 6)) /*this is converting the produced blockID string to a base64 representation*/
                .IgnoreArgument_source();

            STRICT_EXPECTED_CALL(STRING_concat(IGNORED_PTR_ARG, "<Latest>")) /*this is building the XML*/
                .IgnoreArgument_handle();
            STRICT_EXPECTED_CALL(STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG)) /*this is building the XML*/
                .IgnoreArgument_s1()
                .IgnoreArgument_s2();
            STRICT_EXPECTED_CALL(STRING_concat(IGNORED_PTR_ARG, "</Latest>")) /*this is building the XML*/
                .IgnoreArgument_handle();
            STRICT_EXPECTED_CALL(STRING_construct("/something?a=b")); /*this is building the relativePath*/

            STRICT_EXPECTED_CALL(STRING_concat(IGNORED_PTR_ARG, "&comp=block&blockid=")) /*this is building the relativePath*/
                .IgnoreArgument_handle();
            STRICT_EXPECTED_CALL(STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG)) /*this is building the relativePath by adding the blockId (base64 encoded_*/
                .IgnoreArgument_s1()
                .IgnoreArgument_s2();

            STRICT_EXPECTED_CALL(BUFFER_create(content + blockNumber * 4 * 1024 * 1024,
                (blockNumber != (sizes[iSize] - 1) / (4 * 1024 * 1024)) ? 4 * 1024 * 1024 : (sizes[iSize] - 1) % (4 * 1024 * 1024) + 1 /*condition to take care of "the size of the last block*/
            )); /*this is the content to be uploaded by this call*/

            STRICT_EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG)) /*this is getting the relative path as const char* */
                .IgnoreArgument_handle();

            STRICT_EXPECTED_CALL(HTTPAPIEX_ExecuteRequest(IGNORED_PTR_ARG, HTTPAPI_REQUEST_PUT, IGNORED_PTR_ARG, NULL, IGNORED_PTR_ARG, &httpResponse, NULL, testValidBufferHandle))
                .IgnoreArgument_handle()
                .IgnoreArgument_relativePath()
                .IgnoreArgument_requestContent();

            STRICT_EXPECTED_CALL(BUFFER_delete(IGNORED_PTR_ARG)) /*this was the content to be uploaded*/
                .IgnoreArgument_handle();
            STRICT_EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG)) /*this is unbuilding the relativePath*/
                .IgnoreArgument_handle();
            STRICT_EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG)) /*this is unbuilding the blockID string to a base64 representation*/
                .IgnoreArgument_handle();
        }

        /*this part is Put Block list*/
        STRICT_EXPECTED_CALL(STRING_concat(IGNORED_PTR_ARG, "</BlockList>")) /*This is closing the XML*/
            .IgnoreArgument_handle();
        STRICT_EXPECTED_CALL(STRING_construct("/something?a=b")); /*this is building the relative path for the Put BLock list*/

        STRICT_EXPECTED_CALL(STRING_concat(IGNORED_PTR_ARG, "&comp=blocklist")) /*This is still building relative path for Put Block list*/
            .IgnoreArgument_handle();

        STRICT_EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG)) /*this is getting the XML as const char* so it can be passed to _ExecuteRequest*/
            .IgnoreArgument_handle();

        STRICT_EXPECTED_CALL(BUFFER_create(IGNORED_PTR_ARG, IGNORED_NUM_ARG)) /*this is creating the XML body as BUFFER_HANDLE*/
            .IgnoreAllArguments();

        STRICT_EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG)) /*this is getting the relative path*/
            .IgnoreArgument_handle();

        STRICT_EXPECTED_CALL(HTTPAPIEX_ExecuteRequest(
            IGNORED_PTR_ARG,
            HTTPAPI_REQUEST_PUT,
            IGNORED_PTR_ARG,
            NULL,
            IGNORED_PTR_ARG,
            &httpResponse,
            NULL,
            testValidBufferHandle
        ))
            .IgnoreArgument_handle()
            .IgnoreArgument_relativePath()
            .IgnoreArgument_requestContent()
            .CopyOutArgumentBuffer_statusCode(&TwoHundred, sizeof(TwoHundred))
            ;

        STRICT_EXPECTED_CALL(BUFFER_delete(IGNORED_PTR_ARG)) /*This is the XML as BUFFER_HANDLE*/
            .IgnoreArgument_handle();
        STRICT_EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG)) /*this is destroying the relative path for Put Block List*/
            .IgnoreArgument_handle();
        STRICT_EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG))/*this is the XML string used for Put Block List operation*/
            .IgnoreArgument_handle();
        STRICT_EXPECTED_CALL(HTTPAPIEX_Destroy(IGNORED_PTR_ARG)) /*this is the HTTPAPIEX handle*/
            .IgnoreArgument_handle();
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG)) /*this is freeing the copy of hte hostname*/
            .IgnoreArgument_ptr();

        ///act
        BLOB_RESULT result = Blob_UploadFromSasUri("https://h.h/something?a=b", content, sizes[iSize], &httpResponse, testValidBufferHandle);

        ///assert
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
        ASSERT_ARE_EQUAL(BLOB_RESULT, BLOB_OK, result);

        ///cleanup
        gballoc_free(content);
    }
}

/*Tests_SRS_BLOB_02_033: [ If any previous operation that doesn't have an explicit failure description fails then Blob_UploadFromSasUri shall fail and return BLOB_ERROR ]*/
TEST_FUNCTION(Blob_UploadFromSasUri_64MB_unhappy_path)
{
    size_t size = 64 * 1024 * 1024;

    size_t calls_that_cannot_fail[] =
    {
        13   ,/*BUFFER_delete*/
        26   ,/*BUFFER_delete*/
        39   ,/*BUFFER_delete*/
        52   ,/*BUFFER_delete*/
        65   ,/*BUFFER_delete*/
        78   ,/*BUFFER_delete*/
        91   ,/*BUFFER_delete*/
        104  ,/*BUFFER_delete*/
        117  ,/*BUFFER_delete*/
        130  ,/*BUFFER_delete*/
        143  ,/*BUFFER_delete*/
        156  ,/*BUFFER_delete*/
        169  ,/*BUFFER_delete*/
        182  ,/*BUFFER_delete*/
        195  ,/*BUFFER_delete*/
        208  ,/*BUFFER_delete*/
        11   ,/*STRING_c_str*/
        24   ,/*STRING_c_str*/
        37   ,/*STRING_c_str*/
        50   ,/*STRING_c_str*/
        63   ,/*STRING_c_str*/
        76   ,/*STRING_c_str*/
        89   ,/*STRING_c_str*/
        102  ,/*STRING_c_str*/
        115  ,/*STRING_c_str*/
        128  ,/*STRING_c_str*/
        141  ,/*STRING_c_str*/
        154  ,/*STRING_c_str*/
        167  ,/*STRING_c_str*/
        180  ,/*STRING_c_str*/
        193  ,/*STRING_c_str*/
        206  ,/*STRING_c_str*/
        14   ,/*STRING_delete*/
        27   ,/*STRING_delete*/
        40   ,/*STRING_delete*/
        53   ,/*STRING_delete*/
        66   ,/*STRING_delete*/
        79   ,/*STRING_delete*/
        92   ,/*STRING_delete*/
        105  ,/*STRING_delete*/
        118  ,/*STRING_delete*/
        131  ,/*STRING_delete*/
        144  ,/*STRING_delete*/
        157  ,/*STRING_delete*/
        170  ,/*STRING_delete*/
        183  ,/*STRING_delete*/
        196  ,/*STRING_delete*/
        209  ,/*STRING_delete*/
        15   ,/*STRING_delete*/
        28   ,/*STRING_delete*/
        41   ,/*STRING_delete*/
        54   ,/*STRING_delete*/
        67   ,/*STRING_delete*/
        80   ,/*STRING_delete*/
        93   ,/*STRING_delete*/
        106  ,/*STRING_delete*/
        119  ,/*STRING_delete*/
        132  ,/*STRING_delete*/
        145  ,/*STRING_delete*/
        158  ,/*STRING_delete*/
        171  ,/*STRING_delete*/
        184  ,/*STRING_delete*/
        197  ,/*STRING_delete*/
        210  ,/*STRING_delete*/


        214, /*STRING_c_str*/
        216, /*STRING_c_str*/
        218, /*BUFFER_delete*/
        219, /*STRING_delete*/
        220, /*STRING_delete*/
        221, /*HTTPAPIEX_Destroy*/
        222, /*gballoc_free*/
    };

    (void)umock_c_negative_tests_init();
    
    
    umock_c_reset_all_calls();
    ///arrange
    unsigned char * content = (unsigned char*)gballoc_malloc(size);
    ASSERT_IS_NOT_NULL(content);

    umock_c_reset_all_calls();

    memset(content, '3', size);
    content[0] = '0';
    content[size - 1] = '4';

    STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG)) /*this is creating a copy of the hostname */
        .IgnoreArgument_size();

    STRICT_EXPECTED_CALL(HTTPAPIEX_Create("h.h")); /*this is creating the httpapiex handle to storage (it is always the same host)*/
    STRICT_EXPECTED_CALL(STRING_construct("<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n<BlockList>")); /*this is starting to build the XML used in Put Block List operation*/

    /*uploading blocks (Put Block)*/
    for (size_t blockNumber = 0;blockNumber < (size - 1) / (4 * 1024 * 1024) + 1;blockNumber++)
    {
        /*here some sprintf happens and that produces a string in the form: 000000...049999*/
        STRICT_EXPECTED_CALL(Base64_Encode_Bytes(IGNORED_PTR_ARG, 6)) /*this is converting the produced blockID string to a base64 representation*/ /*3, 16, 29... (16 numbers)*/
            .IgnoreArgument_source();

        STRICT_EXPECTED_CALL(STRING_concat(IGNORED_PTR_ARG, "<Latest>")) /*this is building the XML*/
            .IgnoreArgument_handle();
        STRICT_EXPECTED_CALL(STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG)) /*this is building the XML*/
            .IgnoreArgument_s1()
            .IgnoreArgument_s2();
        STRICT_EXPECTED_CALL(STRING_concat(IGNORED_PTR_ARG, "</Latest>")) /*this is building the XML*/
            .IgnoreArgument_handle();
        STRICT_EXPECTED_CALL(STRING_construct("/something?a=b")); /*this is building the relativePath*/

        STRICT_EXPECTED_CALL(STRING_concat(IGNORED_PTR_ARG, "&comp=block&blockid=")) /*this is building the relativePath*/
            .IgnoreArgument_handle();
        STRICT_EXPECTED_CALL(STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG)) /*this is building the relativePath by adding the blockId (base64 encoded_*/
            .IgnoreArgument_s1()
            .IgnoreArgument_s2();

        STRICT_EXPECTED_CALL(BUFFER_create(content + blockNumber * 4 * 1024 * 1024,
            (blockNumber != (size - 1) / (4 * 1024 * 1024)) ? 4 * 1024 * 1024 : (size - 1) % (4 * 1024 * 1024) + 1 /*condition to take care of "the size of the last block*/
        )); /*this is the content to be uploaded by this call*/

        STRICT_EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG)) /*this is getting the relative path as const char* */ /*11, 24, 27...*/
            .IgnoreArgument_handle();

        STRICT_EXPECTED_CALL(HTTPAPIEX_ExecuteRequest(IGNORED_PTR_ARG, HTTPAPI_REQUEST_PUT, IGNORED_PTR_ARG, NULL, IGNORED_PTR_ARG, &httpResponse, NULL, testValidBufferHandle))
            .IgnoreArgument_handle()
            .IgnoreArgument_relativePath()
            .IgnoreArgument_requestContent()
            .CopyOutArgumentBuffer_statusCode(&TwoHundred, sizeof(TwoHundred))
            ;

        STRICT_EXPECTED_CALL(BUFFER_delete(IGNORED_PTR_ARG)) /*this was the content to be uploaded*/ /*13, 26, 39... (16 numbers)*/
            .IgnoreArgument_handle();
        STRICT_EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG)) /*this is unbuilding the relativePath*/ /*14, 27, 40...*/
            .IgnoreArgument_handle();
        STRICT_EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG)) /*this is unbuilding the blockID string to a base64 representation*/ /*15, 28, 41... 210*/
            .IgnoreArgument_handle();
    }

    /*this part is Put Block list*/
    STRICT_EXPECTED_CALL(STRING_concat(IGNORED_PTR_ARG, "</BlockList>")) /*This is closing the XML*/ /*211*/
        .IgnoreArgument_handle();
    STRICT_EXPECTED_CALL(STRING_construct("/something?a=b")); /*this is building the relative path for the Put BLock list*/

    STRICT_EXPECTED_CALL(STRING_concat(IGNORED_PTR_ARG, "&comp=blocklist")) /*This is still building relative path for Put Block list*/
        .IgnoreArgument_handle();

    STRICT_EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG)) /*this is getting the XML as const char* so it can be passed to _ExecuteRequest*/
        .IgnoreArgument_handle();

    STRICT_EXPECTED_CALL(BUFFER_create(IGNORED_PTR_ARG, IGNORED_NUM_ARG)) /*this is creating the XML body as BUFFER_HANDLE*/
        .IgnoreAllArguments();

    STRICT_EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG)) /*this is getting the relative path*/
        .IgnoreArgument_handle();

    STRICT_EXPECTED_CALL(HTTPAPIEX_ExecuteRequest(
        IGNORED_PTR_ARG,
        HTTPAPI_REQUEST_PUT,
        IGNORED_PTR_ARG,
        NULL,
        IGNORED_PTR_ARG,
        &httpResponse,
        NULL,
        testValidBufferHandle
    ))
        .IgnoreArgument_handle()
        .IgnoreArgument_relativePath()
        .IgnoreArgument_requestContent()
        .CopyOutArgumentBuffer_statusCode(&TwoHundred, sizeof(TwoHundred))
        ;

    STRICT_EXPECTED_CALL(BUFFER_delete(IGNORED_PTR_ARG)) /*This is the XML as BUFFER_HANDLE*/
        .IgnoreArgument_handle();
    STRICT_EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG)) /*this is destroying the relative path for Put Block List*/
        .IgnoreArgument_handle();
    STRICT_EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG))/*this is the XML string used for Put Block List operation*/
        .IgnoreArgument_handle();
    STRICT_EXPECTED_CALL(HTTPAPIEX_Destroy(IGNORED_PTR_ARG)) /*this is the HTTPAPIEX handle*/
        .IgnoreArgument_handle();
    STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG)) /*this is freeing the copy of hte hostname*/
        .IgnoreArgument_ptr();

    umock_c_negative_tests_snapshot();

    for (size_t i = 0; i < umock_c_negative_tests_call_count(); i++)
    {
        size_t j;
        umock_c_negative_tests_reset();
        
        for (j = 0;j<sizeof(calls_that_cannot_fail) / sizeof(calls_that_cannot_fail[0]);j++) /*not running the tests that have failed that cannot fail*/
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
            BLOB_RESULT result = Blob_UploadFromSasUri("https://h.h/something?a=b", content, size, &httpResponse, testValidBufferHandle);

            ///assert
            ASSERT_ARE_NOT_EQUAL_WITH_MSG(BLOB_RESULT, BLOB_OK, result, temp_str);
        }
    }

    umock_c_negative_tests_deinit();

    ///cleanup
    gballoc_free(content);
    
}

/*50000*4*1024*1024 is                   209715200000. 
UINT32_MAX is                              4294967295.
SIZE_MAX might be                          4294967295 or 
                                 18446744073709551615
                                 depending on platform
*/

/*run this test only on platforms where 50000*4*1024*1024 does not overflow. Note: code does not have this problem, as 50000 is written as 50000ULL...*/
#if SIZE_MAX > 4294967295
/*Tests_SRS_BLOB_02_034: [ If size is bigger than 50000*4*1024*1024 then Blob_UploadFromSasUri shall fail and return BLOB_INVALID_ARG. ]*/
TEST_FUNCTION(Blob_UploadFromSasUri_fails_when_size_is_exceeded)
{
    ///arrange
    size_t size = 50000ULL * 4 * 1024 * 1024 + 1;
    unsigned char c = 3;

    ///act
    BLOB_RESULT result = Blob_UploadFromSasUri("https://h.h", &c, size, &httpResponse, testValidBufferHandle);

    ///assert
    ASSERT_ARE_EQUAL(BLOB_RESULT, BLOB_INVALID_ARG, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    
    ///cleanup
}
#endif

/*Tests_SRS_BLOB_02_026: [ Otherwise, if HTTP response code is >=300 then Blob_UploadFromSasUri shall succeed and return BLOB_OK. ]*/
TEST_FUNCTION(Blob_UploadFromSasUri_when_http_code_is_404_it_immediately_succeeds)
{
    size_t size = 64 * 1024 * 1024;

    ///arrange
    unsigned char * content = (unsigned char*)gballoc_malloc(size);
    ASSERT_IS_NOT_NULL(content);

    umock_c_reset_all_calls();

    memset(content, '3', size);
    content[0] = '0';
    content[size - 1] = '4';

    STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG)) /*this is creating a copy of the hostname */
        .IgnoreArgument_size();

    STRICT_EXPECTED_CALL(HTTPAPIEX_Create("h.h")); /*this is creating the httpapiex handle to storage (it is always the same host)*/
    STRICT_EXPECTED_CALL(STRING_construct("<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n<BlockList>")); /*this is starting to build the XML used in Put Block List operation*/

    /*uploading blocks (Put Block)*/ /*this simply fails first block*/
    size_t blockNumber = 0;
    {
        /*here some sprintf happens and that produces a string in the form: 000000...049999*/
        STRICT_EXPECTED_CALL(Base64_Encode_Bytes(IGNORED_PTR_ARG, 6)) /*this is converting the produced blockID string to a base64 representation*/
            .IgnoreArgument_source();

        STRICT_EXPECTED_CALL(STRING_concat(IGNORED_PTR_ARG, "<Latest>")) /*this is building the XML*/
            .IgnoreArgument_handle();
        STRICT_EXPECTED_CALL(STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG)) /*this is building the XML*/
            .IgnoreArgument_s1()
            .IgnoreArgument_s2();
        STRICT_EXPECTED_CALL(STRING_concat(IGNORED_PTR_ARG, "</Latest>")) /*this is building the XML*/
            .IgnoreArgument_handle();
        STRICT_EXPECTED_CALL(STRING_construct("/something?a=b")); /*this is building the relativePath*/

        STRICT_EXPECTED_CALL(STRING_concat(IGNORED_PTR_ARG, "&comp=block&blockid=")) /*this is building the relativePath*/
            .IgnoreArgument_handle();
        STRICT_EXPECTED_CALL(STRING_concat_with_STRING(IGNORED_PTR_ARG, IGNORED_PTR_ARG)) /*this is building the relativePath by adding the blockId (base64 encoded_*/
            .IgnoreArgument_s1()
            .IgnoreArgument_s2();

        STRICT_EXPECTED_CALL(BUFFER_create(content + blockNumber * 4 * 1024 * 1024,
            (blockNumber != (size - 1) / (4 * 1024 * 1024)) ? 4 * 1024 * 1024 : (size - 1) % (4 * 1024 * 1024) + 1 /*condition to take care of "the size of the last block*/
        )); /*this is the content to be uploaded by this call*/

        STRICT_EXPECTED_CALL(STRING_c_str(IGNORED_PTR_ARG)) /*this is getting the relative path as const char* */
            .IgnoreArgument_handle();

        STRICT_EXPECTED_CALL(HTTPAPIEX_ExecuteRequest(IGNORED_PTR_ARG, HTTPAPI_REQUEST_PUT, IGNORED_PTR_ARG, NULL, IGNORED_PTR_ARG, &httpResponse, NULL, testValidBufferHandle))
            .IgnoreArgument_handle()
            .IgnoreArgument_relativePath()
            .IgnoreArgument_requestContent()
            .CopyOutArgumentBuffer_statusCode(&FourHundredFour, sizeof(FourHundredFour))
            ;

        STRICT_EXPECTED_CALL(BUFFER_delete(IGNORED_PTR_ARG)) /*this was the content to be uploaded*/
            .IgnoreArgument_handle();
        STRICT_EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG)) /*this is unbuilding the relativePath*/
            .IgnoreArgument_handle();
        STRICT_EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG)) /*this is unbuilding the blockID string to a base64 representation*/
            .IgnoreArgument_handle();
    }

    /*this part is Put Block list*/ /*notice: no op because it failed before with 404*/

    STRICT_EXPECTED_CALL(STRING_delete(IGNORED_PTR_ARG))/*this is the XML string used for Put Block List operation*/
        .IgnoreArgument_handle();
    STRICT_EXPECTED_CALL(HTTPAPIEX_Destroy(IGNORED_PTR_ARG)) /*this is the HTTPAPIEX handle*/
        .IgnoreArgument_handle();
    STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG)) /*this is freeing the copy of hte hostname*/
        .IgnoreArgument_ptr();

    ///act
    BLOB_RESULT result = Blob_UploadFromSasUri("https://h.h/something?a=b", content, size, &httpResponse, testValidBufferHandle);

    ///assert
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    ASSERT_ARE_EQUAL(BLOB_RESULT, BLOB_OK, result);

    ///cleanup
    gballoc_free(content);
    
}


END_TEST_SUITE(blob_ut);
