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
#include "azure_c_shared_utility/httpheaders.h"
#include "azure_c_shared_utility/gballoc.h"
#undef ENABLE_MOCKS

#include "blob.h"
#include "testrunnerswitcher.h"
#include "umock_c.h"
#include "umocktypes_charptr.h"

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
    return (HTTPAPIEX_HANDLE)my_gballoc_malloc(1);
}

static void my_HTTPAPIEX_Destroy(HTTPAPIEX_HANDLE handle)
{
    my_gballoc_free(handle);
}

static BUFFER_HANDLE my_BUFFER_create(const unsigned char* source, size_t size)
{
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


TEST_DEFINE_ENUM_TYPE(BLOB_RESULT, BLOB_RESULT_VALUES);

static TEST_MUTEX_HANDLE g_dllByDll;

#define TEST_HTTPCOLONBACKSLASHBACKSLACH "http://"
#define TEST_HOSTNAME_1 "host.name"
#define TEST_RELATIVE_PATH_1 "/here/follows/something?param1=value1&param2=value2"
#define TEST_VALID_SASURI_1 TEST_HTTPCOLONBACKSLASHBACKSLACH TEST_HOSTNAME_1 TEST_RELATIVE_PATH_1

#define X_MS_BLOB_TYPE "x-ms-blob-type"
#define BLOCK_BLOB "BlockBlob"

static void on_umock_c_error(UMOCK_C_ERROR_CODE error_code)
{
    ASSERT_FAIL("umock_c reported error");
}

static BUFFER_HANDLE testValidBufferHandle; /*assigned in TEST_SUITE_INITIALIZE*/
static unsigned int httpResponse; /*used as out parameter in every call to Blob_....*/

BEGIN_TEST_SUITE(blob_unittests)

TEST_SUITE_INITIALIZE(TestSuiteInitialize)
{
    TEST_INITIALIZE_MEMORY_DEBUG(g_dllByDll);
    (void)umock_c_init(on_umock_c_error);

    (void)umocktypes_charptr_register_types();

    REGISTER_GLOBAL_MOCK_HOOK(gballoc_malloc, my_gballoc_malloc);
    REGISTER_GLOBAL_MOCK_HOOK(gballoc_free, my_gballoc_free);

    REGISTER_GLOBAL_MOCK_HOOK(HTTPAPIEX_Create, my_HTTPAPIEX_Create);
    REGISTER_GLOBAL_MOCK_HOOK(HTTPAPIEX_Destroy, my_HTTPAPIEX_Destroy);

    REGISTER_GLOBAL_MOCK_HOOK(BUFFER_create, my_BUFFER_create);
    REGISTER_GLOBAL_MOCK_HOOK(BUFFER_delete, my_BUFFER_delete);

    REGISTER_GLOBAL_MOCK_HOOK(HTTPHeaders_Alloc, my_HTTPHeaders_Alloc);
    REGISTER_GLOBAL_MOCK_HOOK(HTTPHeaders_Free, my_HTTPHeaders_Free);

    REGISTER_UMOCK_ALIAS_TYPE(HTTP_HEADERS_HANDLE, void*);
    REGISTER_UMOCK_ALIAS_TYPE(HTTPAPIEX_HANDLE, void*);
    REGISTER_UMOCK_ALIAS_TYPE(BUFFER_HANDLE, void*);

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

/*Tests_SRS_BLOB_02_003: [ If size is bigger or equal to 64M then Blob_UploadFromSasUri shall fail and return BLOB_NOT_IMPLEMENTED. ]*/
TEST_FUNCTION(Blob_UploadFromSasUri_with_source_greater_than_or_equal_to_64M_fails_1)
{
    ///arrange
    unsigned char c = '3';
    ///act

    BLOB_RESULT result = Blob_UploadFromSasUri(TEST_VALID_SASURI_1, &c, 64*1024*1024, &httpResponse, testValidBufferHandle);

    ///assert
    ASSERT_ARE_EQUAL(BLOB_RESULT, BLOB_NOT_IMPLEMENTED, result);

    ///cleanup

}

/*Tests_SRS_BLOB_02_003: [ If size is bigger or equal to 64M then Blob_UploadFromSasUri shall fail and return BLOB_NOT_IMPLEMENTED. ]*/
TEST_FUNCTION(Blob_UploadFromSasUri_with_source_greater_than_or_equal_to_64M_fails_2)
{
    ///arrange
    unsigned char c = '3';
    ///act

    BLOB_RESULT result = Blob_UploadFromSasUri(TEST_VALID_SASURI_1, &c, 64 * 1024 * 1024 + 1, &httpResponse, testValidBufferHandle);

    ///assert
    ASSERT_ARE_EQUAL(BLOB_RESULT, BLOB_NOT_IMPLEMENTED, result);

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
TEST_FUNCTION(Blob_UploadFromSasUri_fails_when_SasUri_is_wrong_fails_1)
{
    ///arrange
    unsigned char c = '3';

    ///act
    BLOB_RESULT result = Blob_UploadFromSasUri("https:\\h.h\\doms", &c, sizeof(c), &httpResponse, testValidBufferHandle); /*wrong format for protocol, notice it is actually http:\h.h\doms (missing a \ from http)*/

    ///assert
    ASSERT_ARE_EQUAL(BLOB_RESULT, BLOB_INVALID_ARG, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    ///cleanup
}

/*Tests_SRS_BLOB_02_005: [ If the hostname cannot be determined, then Blob_UploadFromSasUri shall fail and return BLOB_INVALID_ARG. ]*/
TEST_FUNCTION(Blob_UploadFromSasUri_fails_when_SasUri_is_wrong_fails_2)
{
    ///arrange
    unsigned char c = '3';

    ///act
    BLOB_RESULT result = Blob_UploadFromSasUri("https:\\\\h.h", &c, sizeof(c), &httpResponse, testValidBufferHandle); /*there's no relative path here*/

    ///assert
    ASSERT_ARE_EQUAL(BLOB_RESULT, BLOB_INVALID_ARG, result);
    ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    ///cleanup
}



END_TEST_SUITE(blob_unittests);
