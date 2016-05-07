// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#include <stddef.h>
#include <string.h>




#define ENABLE_MOCKS
#include "azure_c_shared_utility\gballoc.h"
#include "azure_c_shared_utility\httpapiex.h"
#include "azure_c_shared_utility\buffer_.h"
#include "azure_c_shared_utility\httpheaders.h"
#undef ENABLE_MOCKS

#include "blob.h"

#include "testrunnerswitcher.h"
#include "umock_c.h"
#include "umocktypes_charptr.h"

static void* my_gballoc_malloc(size_t size)
{
    return malloc(size);
}

static void my_gballoc_free(void* s)
{
    free(s);
}

TEST_DEFINE_ENUM_TYPE(BLOB_RESULT, BLOB_RESULT_VALUES);

static TEST_MUTEX_HANDLE g_dllByDll;

#define TEST_VALID_SASURI "http:\\\\host.name\\here\\follows\\something?param1=value1&param2=value2"

static void on_umock_c_error(UMOCK_C_ERROR_CODE error_code)
{
    ASSERT_FAIL("umock_c reported error");
}

BEGIN_TEST_SUITE(blob_unittests)

TEST_SUITE_INITIALIZE(TestSuiteInitialize)
{
    TEST_INITIALIZE_MEMORY_DEBUG(g_dllByDll);
    umock_c_init(on_umock_c_error);

    REGISTER_GLOBAL_MOCK_HOOK(gballoc_malloc, my_gballoc_malloc);
    REGISTER_GLOBAL_MOCK_HOOK(gballoc_free, my_gballoc_free);
}

TEST_SUITE_CLEANUP(TestClassCleanup)
{
    umock_c_deinit();

    TEST_DEINITIALIZE_MEMORY_DEBUG(g_dllByDll);
}

/*Tests_SRS_BLOB_02_001: [ If SASURI is NULL then Blob_UploadFromSasUri shall fail and return BLOB_INVALID_ARG. ]*/
TEST_FUNCTION(Blob_UploadFromSasUri_with_NULL_SasUri_fails)
{
    ///arrange
    unsigned char c = '3';

    ///act
    BLOB_RESULT result = Blob_UploadFromSasUri(NULL, &c, sizeof(c));

    ///assert
    ASSERT_ARE_EQUAL(BLOB_RESULT, BLOB_INVALID_ARG, result);

    ///cleanup
    
}

/*Tests_SRS_BLOB_02_002: [ If source is NULL and size is not zero then Blob_UploadFromSasUri shall fail and return BLOB_INVALID_ARG. ]*/
TEST_FUNCTION(Blob_UploadFromSasUri_with_NULL_source_and_non_zero_size_fails)
{
    ///arrange

    ///act

    BLOB_RESULT result = Blob_UploadFromSasUri(TEST_VALID_SASURI, NULL, 1);

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

    BLOB_RESULT result = Blob_UploadFromSasUri(TEST_VALID_SASURI, &c, 64*1024*1024);

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

    BLOB_RESULT result = Blob_UploadFromSasUri(TEST_VALID_SASURI, &c, 64 * 1024 * 1024 + 1);

    ///assert
    ASSERT_ARE_EQUAL(BLOB_RESULT, BLOB_NOT_IMPLEMENTED, result);

    ///cleanup

}

/*Tests_SRS_BLOB_02_004: [ Blob_UploadFromSasUri shall copy from SASURI the hostname to a new const char*. ]*/
/*Tests_SRS_BLOB_02_006: [ Blob_UploadFromSasUri shall create a new HTTPAPI_EX_HANDLE by calling HTTPAPIEX_Create passing the hostname. ]*/
/*Tests_SRS_BLOB_02_008: [ Blob_UploadFromSasUri shall compute the relative path of the request from the SASURI parameter. ]*/
/*Tests_SRS_BLOB_02_009: [ Blob_UploadFromSasUri shall create an HTTP_HEADERS_HANDLE for the request HTTP headers carrying the following headers: ]*/
/*Tests_SRS_BLOB_02_010: [ Blob_UploadFromSasUri shall create a BUFFER_HANDLE from source and size parameters. ]*/
/*Tests_SRS_BLOB_02_012: [ Blob_UploadFromSasUri shall call HTTPAPIEX_ExecuteRequest passing the parameters previously build. ]*/
/*Tests_SRS_BLOB_02_015: [ Otherwise, HTTPAPIEX_ExecuteRequest shall succeed and return BLOB_OK. ]*/
TEST_FUNCTION(Blob_UploadFromSasUri_happy_path)
{
    ///arrange
    unsigned char c = '3';
    
    //STRICT_EXPECTED_CALL()

    ///act
    BLOB_RESULT result = Blob_UploadFromSasUri(TEST_VALID_SASURI, &c, sizeof(c));

    ///assert
    ASSERT_ARE_EQUAL(BLOB_RESULT, BLOB_OK, result);

    ///cleanup

}
END_TEST_SUITE(blob_unittests);
