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

void* my_gballoc_realloc(void* ptr, size_t size)
{
    return realloc(ptr, size);
}

void my_gballoc_free(void * t)
{
    free(t);
}

#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

/*want crt_abstractions to use real malloc*/
#define GBALLOC_H 
#define mallocAndStrcpy_s real_mallocAndStrcpy_s
#define unsignedIntToString real_unsignedIntToString
#define size_tToString real_size_tToString 
#include "crt_abstractions.c"
#undef mallocAndStrcpy_s 
#undef unsignedIntToString 
#undef size_tToString 
#undef GBALLOC_H
#undef CRT_ABSTRACTIONS_H

#include "umock_c.h"
#include "umocktypes_charptr.h"
#include "umocktypes_bool.h"
#include "umocktypes_stdint.h"
#include "umock_c_negative_tests.h"

/*not very nice source level preprocessor mocking... */
#define GBALLOC_H
#define VECTOR_create real_VECTOR_create
#define VECTOR_destroy real_VECTOR_destroy
#define VECTOR_push_back real_VECTOR_push_back 
#define VECTOR_erase real_VECTOR_erase 
#define VECTOR_clear real_VECTOR_clear 
#define VECTOR_element real_VECTOR_element 
#define VECTOR_front real_VECTOR_front 
#define VECTOR_back real_VECTOR_back 
#define VECTOR_find_if real_VECTOR_find_if 
#define VECTOR_size real_VECTOR_size 
#include "../src/vector.c"
#undef VECTOR_create 
#undef VECTOR_destroy 
#undef VECTOR_push_back 
#undef VECTOR_erase 
#undef VECTOR_clear 
#undef VECTOR_element 
#undef VECTOR_front 
#undef VECTOR_back 
#undef VECTOR_find_if 
#undef VECTOR_size 
#undef VECTOR_H
#undef GBALLOC_H
#undef CRT_ABSTRACTIONS_H

#include "testrunnerswitcher.h"
#define ENABLE_MOCKS
#include "schema.h"
#include "datamarshaller.h"
#include "schema.h"
#include "iothub_client.h"
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/vector.h"
#undef ENABLE_MOCKS

#include "datapublisher.h"

static AGENT_DATA_TYPE              data;
static const DATA_MARSHALLER_VALUE* g_ExpectedDataSentValues;
static TRANSACTION_HANDLE g_myTransaction = NULL;
static const SCHEMA_MODEL_TYPE_HANDLE TEST_MODEL_HANDLE = (SCHEMA_MODEL_TYPE_HANDLE)0x4242;
static const SCHEMA_MODEL_TYPE_HANDLE       TEST_SCHEMA_MODEL_TYPE_HANDLE = (SCHEMA_MODEL_TYPE_HANDLE)0x4444;
static const char* PropertyPath = "TestPropertyPath";
static const char* PropertyPath_2 = "Test42PropertyPath";

TEST_DEFINE_ENUM_TYPE(DATA_PUBLISHER_RESULT, DATA_PUBLISHER_RESULT_VALUES);
IMPLEMENT_UMOCK_C_ENUM_TYPE(DATA_PUBLISHER_RESULT, DATA_PUBLISHER_RESULT_VALUES);

TEST_DEFINE_ENUM_TYPE(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_RESULT_VALUES);
IMPLEMENT_UMOCK_C_ENUM_TYPE(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_RESULT_VALUES);

TEST_DEFINE_ENUM_TYPE(DATA_MARSHALLER_RESULT, DATA_MARSHALLER_RESULT_VALUES);
IMPLEMENT_UMOCK_C_ENUM_TYPE(DATA_MARSHALLER_RESULT, DATA_MARSHALLER_RESULT_VALUES);

static TEST_MUTEX_HANDLE g_testByTest;
static TEST_MUTEX_HANDLE g_dllByDll;

DEFINE_ENUM_STRINGS(UMOCK_C_ERROR_CODE, UMOCK_C_ERROR_CODE_VALUES)

static void on_umock_c_error(UMOCK_C_ERROR_CODE error_code)
{
    char temp_str[256];
    (void)snprintf(temp_str, sizeof(temp_str), "umock_c reported error :%s", ENUM_TO_STRING(UMOCK_C_ERROR_CODE, error_code));
    ASSERT_FAIL(temp_str);
}

static DATA_MARSHALLER_HANDLE my_DataMarshaller_Create(SCHEMA_MODEL_TYPE_HANDLE modelHandle, bool includePropertyPath)
{
    (void)(modelHandle, includePropertyPath);
    return (DATA_MARSHALLER_HANDLE)my_gballoc_malloc(1);
}

static void my_DataMarshaller_Destroy(DATA_MARSHALLER_HANDLE h)
{
    my_gballoc_free(h);
}

static AGENT_DATA_TYPES_RESULT my_Create_AGENT_DATA_TYPE_from_AGENT_DATA_TYPE(AGENT_DATA_TYPE* dest, const AGENT_DATA_TYPE* src)
{
    if (dest != NULL)
    {
        memcpy(dest, src, sizeof(AGENT_DATA_TYPE));
    }
    return AGENT_DATA_TYPES_OK;
}
static DATA_MARSHALLER_RESULT my_DataMarshaller_SendData(DATA_MARSHALLER_HANDLE dataMarshallerHandle, size_t valueCount, const DATA_MARSHALLER_VALUE* values, unsigned char** destination, size_t* destinationSize)
{
    (void)(destination, destinationSize, values, valueCount, dataMarshallerHandle);
    return DATA_MARSHALLER_OK;
}

BEGIN_TEST_SUITE(DataPublisher_ut)

    TEST_SUITE_INITIALIZE(TestClassInitialize)
    {
        TEST_INITIALIZE_MEMORY_DEBUG(g_dllByDll);
        g_testByTest = TEST_MUTEX_CREATE();
        ASSERT_IS_NOT_NULL(g_testByTest);

        (void)umock_c_init(on_umock_c_error);

        (void)umocktypes_bool_register_types();
        (void)umocktypes_charptr_register_types();
        (void)umocktypes_stdint_register_types();

        REGISTER_UMOCK_ALIAS_TYPE(SCHEMA_MODEL_TYPE_HANDLE, void*);
        REGISTER_UMOCK_ALIAS_TYPE(DATA_MARSHALLER_HANDLE, void*);
        REGISTER_UMOCK_ALIAS_TYPE(VECTOR_HANDLE, void*);
        REGISTER_UMOCK_ALIAS_TYPE(PREDICATE_FUNCTION, void*);
        REGISTER_UMOCK_ALIAS_TYPE(const VECTOR_HANDLE, void*);
        
        REGISTER_UMOCK_ALIAS_TYPE(DATA_PUBLISHER_RESULT, int);
        REGISTER_UMOCK_ALIAS_TYPE(AGENT_DATA_TYPES_RESULT, int);
        REGISTER_UMOCK_ALIAS_TYPE(DATA_MARSHALLER_RESULT, int);
        

        REGISTER_GLOBAL_MOCK_HOOK(gballoc_malloc, my_gballoc_malloc);
        REGISTER_GLOBAL_MOCK_FAIL_RETURN(gballoc_malloc, NULL);
        REGISTER_GLOBAL_MOCK_HOOK(gballoc_realloc, my_gballoc_realloc);
        REGISTER_GLOBAL_MOCK_FAIL_RETURN(gballoc_realloc, NULL);
        REGISTER_GLOBAL_MOCK_HOOK(gballoc_free, my_gballoc_free);

        REGISTER_GLOBAL_MOCK_HOOK(DataMarshaller_Create, my_DataMarshaller_Create);
        REGISTER_GLOBAL_MOCK_HOOK(DataMarshaller_SendData, my_DataMarshaller_SendData);
        REGISTER_GLOBAL_MOCK_RETURN(DataMarshaller_SendData_ReportedProperties, DATA_MARSHALLER_OK);
        REGISTER_GLOBAL_MOCK_HOOK(DataMarshaller_Destroy, my_DataMarshaller_Destroy);

        REGISTER_GLOBAL_MOCK_RETURN(Schema_ModelPropertyByPathExists, true);
        REGISTER_GLOBAL_MOCK_FAIL_RETURN(Schema_ModelPropertyByPathExists, false);
        
        

        REGISTER_GLOBAL_MOCK_RETURN(Schema_ModelReportedPropertyByPathExists, true);
        REGISTER_GLOBAL_MOCK_FAIL_RETURN(Schema_ModelReportedPropertyByPathExists, false);

        REGISTER_GLOBAL_MOCK_HOOK(Create_AGENT_DATA_TYPE_from_AGENT_DATA_TYPE, my_Create_AGENT_DATA_TYPE_from_AGENT_DATA_TYPE);
        REGISTER_GLOBAL_MOCK_FAIL_RETURN(Create_AGENT_DATA_TYPE_from_AGENT_DATA_TYPE, AGENT_DATA_TYPES_ERROR);

        REGISTER_GLOBAL_MOCK_HOOK(mallocAndStrcpy_s, real_mallocAndStrcpy_s);
        REGISTER_GLOBAL_MOCK_FAIL_RETURN(mallocAndStrcpy_s, __LINE__);
        REGISTER_GLOBAL_MOCK_HOOK(unsignedIntToString, real_unsignedIntToString);
        REGISTER_GLOBAL_MOCK_HOOK(size_tToString, real_size_tToString);
        
        REGISTER_GLOBAL_MOCK_HOOK(VECTOR_create, real_VECTOR_create);
        REGISTER_GLOBAL_MOCK_FAIL_RETURN(VECTOR_create, NULL);

        REGISTER_GLOBAL_MOCK_HOOK(VECTOR_size, real_VECTOR_size);
        REGISTER_GLOBAL_MOCK_HOOK(VECTOR_push_back, real_VECTOR_push_back);
        REGISTER_GLOBAL_MOCK_FAIL_RETURN(VECTOR_push_back, __LINE__);

        REGISTER_GLOBAL_MOCK_HOOK(VECTOR_find_if, real_VECTOR_find_if);

        REGISTER_GLOBAL_MOCK_HOOK(VECTOR_element, real_VECTOR_element);

        REGISTER_GLOBAL_MOCK_HOOK(VECTOR_destroy, real_VECTOR_destroy);

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
        data.type = EDM_SINGLE_TYPE;
        data.value.edmSingle.value = 3.5f;
        g_ExpectedDataSentValues = NULL;
    }

    TEST_FUNCTION_CLEANUP(TestMethodCleanup)
    {
        if (g_myTransaction != NULL)
        {
            unsigned char* destination;
            size_t destinationSize;
            (void)DataPublisher_EndTransaction(g_myTransaction, &destination, &destinationSize);
            g_myTransaction = NULL;
        }
        TEST_MUTEX_RELEASE(g_testByTest);
    }

    /* DataPublisher_Create */

    /* Tests_SRS_DATA_PUBLISHER_99_042:[ If a NULL argument is passed to it, DataPublisher_Create shall return NULL.] */
    TEST_FUNCTION(DataPublisher_Create_With_NULL_Model_Handle_Fails)
    {
        // arrange

        // act
        DATA_PUBLISHER_HANDLE result = DataPublisher_Create(NULL, true);

        // assert
        ASSERT_IS_NULL(result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    }

    /* Tests_SRS_DATA_PUBLISHER_99_041:[ DataPublisher_Create shall create a new DataPublisher instance and return a non-NULL handle in case of success.] */
    /* Tests_SRS_DATA_PUBLISHER_99_043:[ DataPublisher_Create shall initialize and hold a handle to a DataMarshaller instance.] */
    /* Tests_SRS_DATA_PUBLISHER_01_001: [DataPublisher_Create shall pass the includePropertyPath argument to DataMarshaller_Create.] */
    TEST_FUNCTION(DataPublisher_Create_With_Valid_Arguments_Yields_A_non_NULL_Handle)
    {
        // arrange
        STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument_size();
        STRICT_EXPECTED_CALL(DataMarshaller_Create(TEST_MODEL_HANDLE, true));

        // act
        DATA_PUBLISHER_HANDLE result = DataPublisher_Create(TEST_MODEL_HANDLE, true);

        // assert
        ASSERT_IS_NOT_NULL(result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        // cleanup
        DataPublisher_Destroy(result);
    }

    /* Tests_SRS_DATA_PUBLISHER_01_001: [DataPublisher_Create shall pass the includePropertyPath argument to DataMarshaller_Create.] */
    TEST_FUNCTION(DataPublisher_Create_Passes_includePropertyPath_To_DataMarshaller_Create)
    {
        // arrange
        STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument_size();
        STRICT_EXPECTED_CALL(DataMarshaller_Create(TEST_MODEL_HANDLE, false));

        // act
        DATA_PUBLISHER_HANDLE result = DataPublisher_Create(TEST_MODEL_HANDLE, false);

        // assert
        ASSERT_IS_NOT_NULL(result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        // cleanup
        DataPublisher_Destroy(result);
    }

    /* Tests_SRS_DATA_PUBLISHER_99_041:[ DataPublisher_Create shall create a new DataPublisher instance and return a non-NULL handle in case of success.] */
    /* Tests_SRS_DATA_PUBLISHER_99_043:[ DataPublisher_Create shall initialize and hold a handle to a DataMarshaller instance.] */
    TEST_FUNCTION(DataPublisher_Create_2_Instances_Yields_Different_Handles)
    {
        // arrange
        STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument_size();
        STRICT_EXPECTED_CALL(DataMarshaller_Create(TEST_MODEL_HANDLE, true));

        STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument_size();
        STRICT_EXPECTED_CALL(DataMarshaller_Create(TEST_MODEL_HANDLE, true));

        DATA_PUBLISHER_HANDLE handle1 = DataPublisher_Create(TEST_MODEL_HANDLE, true);

        // act
        DATA_PUBLISHER_HANDLE handle2 = DataPublisher_Create(TEST_MODEL_HANDLE, true);

        // assert
        ASSERT_IS_NOT_NULL(handle1);
        ASSERT_IS_NOT_NULL(handle2);
        ASSERT_ARE_NOT_EQUAL(void_ptr, (void_ptr)handle1, (void_ptr)handle2);

        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        // cleanup
        DataPublisher_Destroy(handle1);
        DataPublisher_Destroy(handle2);
    }

    /* Tests_SRS_DATA_PUBLISHER_99_044:[ If the creation of the DataMarshaller instance fails, DataPublisher_Create shall return NULL.] */
    TEST_FUNCTION(DataPublisher_When_DataMarshaller_Create_Fails_DataPublisher_Create_Returns_NULL)
    {
        // arrange
        STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument_size();
        STRICT_EXPECTED_CALL(DataMarshaller_Create(TEST_MODEL_HANDLE, true))
            .SetReturn((DATA_MARSHALLER_HANDLE)NULL);
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument_ptr();

        // act
        DATA_PUBLISHER_HANDLE handle = DataPublisher_Create(TEST_MODEL_HANDLE, true);

        // assert
        ASSERT_IS_NULL(handle);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    }

    /* DataPublisher_Destroy */

    /* Tests_SRS_DATA_PUBLISHER_99_045:[ DataPublisher_Destroy shall free all resources associated with a DataPublisher instance.] */
    TEST_FUNCTION(DataPublisher_Destroy_Releases_The_Data_Marshaller_Instance)
    {
        // arrange
        DATA_PUBLISHER_HANDLE handle = DataPublisher_Create(TEST_SCHEMA_MODEL_TYPE_HANDLE, true);
        umock_c_reset_all_calls();
        STRICT_EXPECTED_CALL(DataMarshaller_Destroy(IGNORED_PTR_ARG))
            .IgnoreArgument_dataMarshallerHandle();
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument_ptr();

        // act
        DataPublisher_Destroy(handle);

        // assert
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    }

    /* Tests_SRS_DATA_PUBLISHER_99_046:[ If a NULL argument is passed to it, DataPublisher_Destroy shall do nothing.] */
    /* Can't really test does nothing ... */
    TEST_FUNCTION(DataPublisher_Destroy_With_A_NULL_Handle_Does_Not_Call_DataMarshaller_Destroy)
    {
        // arrange
        // act
        DataPublisher_Destroy(NULL);

        // assert
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    }

    /* DataPublisher_StartTransaction */

    /* Tests_SRS_DATA_PUBLISHER_99_007:[ A call to DataPublisher_StartBeginTransaction shall start a new transaction.] */
    /* Tests_SRS_DATA_PUBLISHER_99_008:[ DataPublisher_StartBeginTransaction shall return a non-NULL handle upon success.] */
    TEST_FUNCTION(DataPublisher_StartTransaction_Succeeds)
    {
        // arrange
        DATA_PUBLISHER_HANDLE handle = DataPublisher_Create(TEST_SCHEMA_MODEL_TYPE_HANDLE, true);
        unsigned char* destination;
        size_t destinationSize;
        umock_c_reset_all_calls();

        STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument_size();

        // act
        TRANSACTION_HANDLE result = DataPublisher_StartTransaction(handle);

        // assert
        ASSERT_IS_NOT_NULL(result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        // cleanup
        (void)DataPublisher_EndTransaction(result, &destination, &destinationSize);
        DataPublisher_Destroy(handle);
    }

    /* Tests_SRS_DATA_PUBLISHER_99_038:[ If DataPublisher_StartTransaction is called with a NULL argument it shall return NULL.] */
    TEST_FUNCTION(DataPublisher_StartTransaction_With_NULL_Handle_Fails)
    {
        // arrange
        // act
        TRANSACTION_HANDLE result = DataPublisher_StartTransaction(NULL);

        // assert
        ASSERT_IS_NULL(result);
    }

    /* DataPublisher_PublishTransacted */

    /* Tests_SRS_DATA_PUBLISHER_99_017:[ When one or more NULL parameter(s) are specified, DataPublisher_PublishTransacted is called with a NULL transactionHandle, it shall return DATA_PUBLISHER_INVALID_ARG.] */
    TEST_FUNCTION(DataPublisher_PublishTransacted_With_NULL_Transaction_Handle_Fails)
    {
        // arrange
        // act
        DATA_PUBLISHER_RESULT result = DataPublisher_PublishTransacted(NULL, PropertyPath, &data);

        // assert
        ASSERT_ARE_EQUAL(DATA_PUBLISHER_RESULT, DATA_PUBLISHER_INVALID_ARG, result);
    }

    /* Tests_SRS_DATA_PUBLISHER_99_017:[ When one or more NULL parameter(s) are specified, DataPublisher_PublishTransacted is called with a NULL transactionHandle, it shall return DATA_PUBLISHER_INVALID_ARG.] */
    TEST_FUNCTION(DataPublisher_PublishTransacted_With_NULL_PropertyPath_Fails)
    {
        // arrange
        DATA_PUBLISHER_HANDLE handle = DataPublisher_Create(TEST_SCHEMA_MODEL_TYPE_HANDLE, true);
        unsigned char* destination;
        size_t destinationSize;
        TRANSACTION_HANDLE transaction = DataPublisher_StartTransaction(handle);
        umock_c_reset_all_calls();

        // act
        DATA_PUBLISHER_RESULT result = DataPublisher_PublishTransacted(transaction, NULL, &data);
        
        // assert
        ASSERT_ARE_EQUAL(DATA_PUBLISHER_RESULT, DATA_PUBLISHER_INVALID_ARG, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        // cleanup
        (void)DataPublisher_EndTransaction(transaction, &destination, &destinationSize);
        DataPublisher_Destroy(handle);
    }

    /* Tests_SRS_DATA_PUBLISHER_99_017:[ When one or more NULL parameter(s) are specified, DataPublisher_PublishTransacted is called with a NULL transactionHandle, it shall return DATA_PUBLISHER_INVALID_ARG.] */
    TEST_FUNCTION(DataPublisher_PublishTransacted_With_NULL_Data_Payload_Fails)
    {
        // arrange
        DATA_PUBLISHER_HANDLE handle = DataPublisher_Create(TEST_SCHEMA_MODEL_TYPE_HANDLE, true);
        TRANSACTION_HANDLE transaction = DataPublisher_StartTransaction(handle);
        umock_c_reset_all_calls();

        // act
        DATA_PUBLISHER_RESULT result = DataPublisher_PublishTransacted(transaction, PropertyPath, NULL);

        // assert
        ASSERT_ARE_EQUAL(DATA_PUBLISHER_RESULT, DATA_PUBLISHER_INVALID_ARG, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        // cleanup
        (void)DataPublisher_CancelTransaction(transaction);
        DataPublisher_Destroy(handle);
    }

    /* Tests_SRS_DATA_PUBLISHER_99_016:[ When DataPublisher_PublishTransacted is invoked, DataPublisher shall associate the data with the transaction identified by the transactionHandle argument and return DATA_PUBLISHER_OK. No data shall be dispatched at the time of the call.] */
    TEST_FUNCTION(DataPublisher_PublishTransacted_With_Valid_Data_Succeeds)
    {
        // arrange
        DATA_PUBLISHER_HANDLE handle = DataPublisher_Create(TEST_MODEL_HANDLE, true);
        TRANSACTION_HANDLE transaction = DataPublisher_StartTransaction(handle);
        umock_c_reset_all_calls();

        STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, PropertyPath))
            .IgnoreArgument_destination();
        STRICT_EXPECTED_CALL(Schema_ModelPropertyByPathExists(TEST_MODEL_HANDLE, PropertyPath));
        STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument_size();
        STRICT_EXPECTED_CALL(Create_AGENT_DATA_TYPE_from_AGENT_DATA_TYPE(IGNORED_PTR_ARG, &data))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(gballoc_realloc(IGNORED_PTR_ARG, IGNORED_NUM_ARG))
            .IgnoreArgument_ptr()
            .IgnoreArgument_size();

        // act
        DATA_PUBLISHER_RESULT result = DataPublisher_PublishTransacted(transaction, PropertyPath, &data);

        // assert
        ASSERT_ARE_EQUAL(DATA_PUBLISHER_RESULT, DATA_PUBLISHER_OK, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        // cleanup
        (void)DataPublisher_CancelTransaction(transaction);
        DataPublisher_Destroy(handle);
    }

    /* Tests_SRS_DATA_PUBLISHER_99_040:[ When PropertyPath does not exist in the supplied model, DataPublisher_Publish shall return DATA_PUBLISHER_SCHEMA_FAILED without dispatching data.] */
    TEST_FUNCTION(DataPublisher_When_GetModelProperty_Returns_NULL_Then_PublishTransacted_Fails)
    {
        // arrange
        DATA_PUBLISHER_HANDLE handle = DataPublisher_Create(TEST_MODEL_HANDLE, true);
        TRANSACTION_HANDLE transaction = DataPublisher_StartTransaction(handle);
        umock_c_reset_all_calls();

        STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, PropertyPath))
            .IgnoreArgument_destination();
        STRICT_EXPECTED_CALL(Schema_ModelPropertyByPathExists(TEST_MODEL_HANDLE, PropertyPath))
            .SetReturn(false);
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument_ptr();

        // act
        DATA_PUBLISHER_RESULT result = DataPublisher_PublishTransacted(transaction, PropertyPath, &data);

        // assert
        ASSERT_ARE_EQUAL(DATA_PUBLISHER_RESULT, DATA_PUBLISHER_SCHEMA_FAILED, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        // cleanup
        (void)DataPublisher_CancelTransaction(transaction);
        DataPublisher_Destroy(handle);
    }

    /* Tests_SRS_DATA_PUBLISHER_99_028:[ If creating the copy fails then DATA_PUBLISHER_AGENT_DATA_TYPES_ERROR shall be returned.] */
    TEST_FUNCTION(DataPublisher_When_Cloning_The_Agent_Data_Type_Fails_Then_PublishTransacted_Fails)
    {
        // arrange
        DATA_PUBLISHER_HANDLE handle = DataPublisher_Create(TEST_MODEL_HANDLE, true);
        TRANSACTION_HANDLE transaction = DataPublisher_StartTransaction(handle);
        umock_c_reset_all_calls();

        STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, PropertyPath))
            .IgnoreArgument_destination();
        STRICT_EXPECTED_CALL(Schema_ModelPropertyByPathExists(TEST_MODEL_HANDLE, PropertyPath));
        STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument_size();
        STRICT_EXPECTED_CALL(Create_AGENT_DATA_TYPE_from_AGENT_DATA_TYPE(IGNORED_PTR_ARG, &data))
            .IgnoreArgument(1)
            .SetReturn(AGENT_DATA_TYPES_ERROR);
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument_ptr();
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument_ptr();

        // act
        DATA_PUBLISHER_RESULT result = DataPublisher_PublishTransacted(transaction, PropertyPath, &data);

        // assert
        ASSERT_ARE_EQUAL(DATA_PUBLISHER_RESULT, DATA_PUBLISHER_AGENT_DATA_TYPES_ERROR, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        // cleanup
        (void)DataPublisher_CancelTransaction(transaction);
        DataPublisher_Destroy(handle);
    }

    /* Tests_SRS_DATA_PUBLISHER_99_027:[ DataPublisher shall make a copy of the data when associating it with the transaction by using AgentTypeSystem APIs.] */
    TEST_FUNCTION(DataPublisher_When_The_Value_Is_Destroyed_Before_End_Transaction_Cloned_Data_Is_Given_To_Data_Marshaller)
    {
        // arrange
        DATA_PUBLISHER_HANDLE handle = DataPublisher_Create(TEST_SCHEMA_MODEL_TYPE_HANDLE, true);
        unsigned char* destination;
        size_t destinationSize;
        TRANSACTION_HANDLE transaction = DataPublisher_StartTransaction(handle);        
        AGENT_DATA_TYPE data2;

        data2.type = EDM_SINGLE_TYPE;
        data2.value.edmSingle.value = 3.5f;

        (void)DataPublisher_PublishTransacted(transaction, PropertyPath, &data2);

        (void)memset(&data2, 0, sizeof(data2));

        DATA_MARSHALLER_VALUE value;
        value.PropertyPath = PropertyPath;
        value.Value = &data;

        umock_c_reset_all_calls();
        
        STRICT_EXPECTED_CALL(DataMarshaller_SendData(IGNORED_PTR_ARG, 1, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument_dataMarshallerHandle()
            .IgnoreArgument_values()
            .IgnoreArgument(4)
            .IgnoreArgument(5);
        STRICT_EXPECTED_CALL(Destroy_AGENT_DATA_TYPE(IGNORED_PTR_ARG))
            .IgnoreArgument_agentData();
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument_ptr();
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument_ptr();
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument_ptr();
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument_ptr();

        // act
        DATA_PUBLISHER_RESULT result = DataPublisher_EndTransaction(transaction, &destination, &destinationSize);

        // assert
        ASSERT_ARE_EQUAL(DATA_PUBLISHER_RESULT, DATA_PUBLISHER_OK, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        // cleanup
        DataPublisher_Destroy(handle);
    }

    /* DataPublisher_EndTransaction */

    /* Tests_SRS_DATA_PUBLISHER_99_011:[ If the transactionHandle argument is NULL, DataPublisher_EndTransaction shall return DATA_PUBLISHER_INVALID_ARG.] */
    TEST_FUNCTION(DataPublisher_EndTransaction_With_NULL_Handle_Fails)
    {
        // arrange
        unsigned char* destination;
        size_t destinationSize;

        // act
        DATA_PUBLISHER_RESULT result = DataPublisher_EndTransaction(NULL, &destination, &destinationSize);

        // assert
        ASSERT_ARE_EQUAL(DATA_PUBLISHER_RESULT, DATA_PUBLISHER_INVALID_ARG, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    }

    /*Tests_SRS_DATA_PUBLISHER_02_006: [If the destination argument is NULL, DataPublisher_EndTransaction shall return DATA_PUBLISHER_INVALID_ARG.] */
    TEST_FUNCTION(DataPublisher_EndTransaction_With_NULL_destination_Fails)
    {
        // arrange
        DATA_PUBLISHER_HANDLE handle = DataPublisher_Create(TEST_SCHEMA_MODEL_TYPE_HANDLE, true);
        size_t destinationSize;
        TRANSACTION_HANDLE transaction = DataPublisher_StartTransaction(handle);
        umock_c_reset_all_calls();

        // act
        DATA_PUBLISHER_RESULT result = DataPublisher_EndTransaction(transaction, NULL, &destinationSize);

        // assert
        ASSERT_ARE_EQUAL(DATA_PUBLISHER_RESULT, DATA_PUBLISHER_INVALID_ARG, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        // cleanup
        unsigned char* destination;
        (void)DataPublisher_EndTransaction(transaction, &destination, &destinationSize);
        DataPublisher_Destroy(handle);
    }

    /*Tests_SRS_DATA_PUBLISHER_02_006: [If the destination argument is NULL, DataPublisher_EndTransaction shall return DATA_PUBLISHER_INVALID_ARG.] */
    TEST_FUNCTION(DataPublisher_EndTransaction_With_NULL_destinationSize_Fails)
    {
        // arrange
        DATA_PUBLISHER_HANDLE handle = DataPublisher_Create(TEST_SCHEMA_MODEL_TYPE_HANDLE, true);
        unsigned char* destination;
        TRANSACTION_HANDLE transaction = DataPublisher_StartTransaction(handle);
        umock_c_reset_all_calls();

        // act
        DATA_PUBLISHER_RESULT result = DataPublisher_EndTransaction(transaction, &destination, NULL);

        // assert
        ASSERT_ARE_EQUAL(DATA_PUBLISHER_RESULT, DATA_PUBLISHER_INVALID_ARG, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        // cleanup
        size_t destinationSize;
        (void)DataPublisher_EndTransaction(transaction, &destination, &destinationSize);
        DataPublisher_Destroy(handle);
    }

    /* Tests_SRS_DATA_PUBLISHER_99_024:[ If no values have been associated with the transaction, no data shall be dispatched to DataMarshaller, the transaction shall be discarded and DataPublisher_EndTransaction shall return DATA_PUBLISHER_EMPTY_TRANSACTION.] */
    TEST_FUNCTION(DataPublisher_EndTransaction_With_Valid_Handle_An_Empty_Transaction_Fails)
    {
        // arrange
        DATA_PUBLISHER_HANDLE handle = DataPublisher_Create(TEST_SCHEMA_MODEL_TYPE_HANDLE, true);
        unsigned char* destination;
        size_t destinationSize;
        TRANSACTION_HANDLE transaction = DataPublisher_StartTransaction(handle);
        umock_c_reset_all_calls();

        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument_ptr();
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument_ptr();

        // act
        DATA_PUBLISHER_RESULT result = DataPublisher_EndTransaction(transaction, &destination, &destinationSize);

        // assert
        ASSERT_ARE_EQUAL(DATA_PUBLISHER_RESULT, DATA_PUBLISHER_EMPTY_TRANSACTION, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        // cleanup
        DataPublisher_Destroy(handle);
    }

    /* Tests_SRS_DATA_PUBLISHER_99_016:[ When DataPublisher_PublishTransacted is invoked, DataPublisher shall associate the data with the transaction identified by the transactionHandle argument and return DATA_PUBLISHER_OK. No data shall be dispatched at the time of the call.] */
    /* Tests_SRS_DATA_PUBLISHER_99_010:[ A call to DataPublisher_EndTransaction shall mark the end of a transaction and trigger a dispatch of all the data grouped by that transaction.] */
    /* Tests_SRS_DATA_PUBLISHER_99_026:[ On success, DataPublisher_EndTransaction shall return DATA_PUBLISHER_OK.] */
    /* Tests_SRS_DATA_PUBLISHER_99_055:[ DataPublisher_EndTransaction shall get the Current time before sending the transaction to DataPublisher or storing it on Buffer Storage. ]*/
    TEST_FUNCTION(DataPublisher_EndTransaction_With_One_Value_Dispatches_The_Value)
    {
        // arrange
        DATA_PUBLISHER_HANDLE handle = DataPublisher_Create(TEST_SCHEMA_MODEL_TYPE_HANDLE, true);
        unsigned char* destination;
        size_t destinationSize;
        TRANSACTION_HANDLE transaction = DataPublisher_StartTransaction(handle);
        (void)DataPublisher_PublishTransacted(transaction, PropertyPath, &data);
        DATA_MARSHALLER_VALUE value;
        value.PropertyPath = PropertyPath;
        value.Value = &data;

        umock_c_reset_all_calls();

        STRICT_EXPECTED_CALL(DataMarshaller_SendData(IGNORED_PTR_ARG, 1, &value, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument_dataMarshallerHandle()
            .IgnoreArgument_values()
            .IgnoreArgument(4)
            .IgnoreArgument(5);
        EXPECTED_CALL(Destroy_AGENT_DATA_TYPE(IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument_ptr();
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument_ptr();
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument_ptr();
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument_ptr();

        // act
        DATA_PUBLISHER_RESULT result = DataPublisher_EndTransaction(transaction, &destination, &destinationSize);

        // assert
        ASSERT_ARE_EQUAL(DATA_PUBLISHER_RESULT, DATA_PUBLISHER_OK, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        // cleanup
        DataPublisher_Destroy(handle);
    }

    /* Tests_SRS_DATA_PUBLISHER_99_025:[ When the DataMarshaller_SendData call fails, DataPublisher_EndTransaction shall return DATA_PUBLISHER_MARSHALLER_ERROR.] */
    TEST_FUNCTION(DataPublisher_When_DataMatshaller_SendData_Fails_Then_EndTransaction_Fails)
    {
        // arrange
        DATA_PUBLISHER_HANDLE handle = DataPublisher_Create(TEST_SCHEMA_MODEL_TYPE_HANDLE, true);
        unsigned char* destination;
        size_t destinationSize;
        TRANSACTION_HANDLE transaction = DataPublisher_StartTransaction(handle);
        (void)DataPublisher_PublishTransacted(transaction, PropertyPath, &data);
        DATA_MARSHALLER_VALUE value;
        value.PropertyPath = PropertyPath;
        value.Value = &data;
        umock_c_reset_all_calls();

        STRICT_EXPECTED_CALL(DataMarshaller_SendData(IGNORED_PTR_ARG, 1, &value, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument_dataMarshallerHandle()
            .IgnoreArgument_values()
            .IgnoreArgument(4)
            .IgnoreArgument(5)
            .SetReturn(DATA_MARSHALLER_ERROR);
        EXPECTED_CALL(Destroy_AGENT_DATA_TYPE(IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument_ptr();
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument_ptr();
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument_ptr();
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument_ptr();

        // act
        DATA_PUBLISHER_RESULT result = DataPublisher_EndTransaction(transaction, &destination, &destinationSize);

        // assert
        ASSERT_ARE_EQUAL(DATA_PUBLISHER_RESULT, DATA_PUBLISHER_MARSHALLER_ERROR, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        // cleanup
        DataPublisher_Destroy(handle);
    }

    /* Tests_SRS_DATA_PUBLISHER_99_019:[ If the same property is associated twice with a transaction, then the last value shall be kept associated with the transaction.] */
    TEST_FUNCTION(DataPublisher_Adding_The_Same_Property_To_A_Transaction_Twice_Keeps_The_Last_Value_Only)
    {
        // arrange
        DATA_PUBLISHER_HANDLE handle = DataPublisher_Create(TEST_MODEL_HANDLE, true);
        unsigned char* destination;
        size_t destinationSize;
        TRANSACTION_HANDLE transaction = DataPublisher_StartTransaction(handle);
        AGENT_DATA_TYPE data2;

        data2.type = EDM_SINGLE_TYPE;
        data2.value.edmSingle.value = 3.7f;

        DATA_MARSHALLER_VALUE value;
        value.PropertyPath = PropertyPath;
        value.Value = &data2;

        g_ExpectedDataSentValues = &value;

        (void)DataPublisher_PublishTransacted(transaction, PropertyPath, &data);
        (void)DataPublisher_PublishTransacted(transaction, PropertyPath, &data2);
        umock_c_reset_all_calls();

        STRICT_EXPECTED_CALL(DataMarshaller_SendData(IGNORED_PTR_ARG, 1, IGNORED_PTR_ARG, &destination, &destinationSize))
            .IgnoreArgument_dataMarshallerHandle()
            .IgnoreArgument_values();
        EXPECTED_CALL(Destroy_AGENT_DATA_TYPE(IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument_ptr();
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument_ptr();
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument_ptr();
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument_ptr();

        // act
        DATA_PUBLISHER_RESULT result = DataPublisher_EndTransaction(transaction, &destination, &destinationSize);

        // assert
        ASSERT_ARE_EQUAL(DATA_PUBLISHER_RESULT, DATA_PUBLISHER_OK, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        // cleanup
        DataPublisher_Destroy(handle);
    }

    /* Tests_SRS_DATA_PUBLISHER_99_016:[ When DataPublisher_PublishTransacted is invoked, DataPublisher shall associate the data with the transaction identified by the transactionHandle argument and return DATA_PUBLISHER_OK. No data shall be dispatched at the time of the call.] */
    /* Tests_SRS_DATA_PUBLISHER_99_010:[ A call to DataPublisher_EndTransaction shall mark the end of a transaction and trigger a dispatch of all the data grouped by that transaction.] */
    /* Tests_SRS_DATA_PUBLISHER_99_026:[ On success, DataPublisher_EndTransaction shall return DATA_PUBLISHER_OK.] */
    TEST_FUNCTION(DataPublisher_Adding_Two_Different_Properties_To_A_Transaction_Dispatches_Both_Values)
    {
        // arrange
        DATA_PUBLISHER_HANDLE handle = DataPublisher_Create(TEST_MODEL_HANDLE, true);
        unsigned char* destination;
        size_t destinationSize;
        TRANSACTION_HANDLE transaction = DataPublisher_StartTransaction(handle);
        AGENT_DATA_TYPE data2;

        data2.type = EDM_SINGLE_TYPE;
        data2.value.edmSingle.value = 3.7f;

        (void)DataPublisher_PublishTransacted(transaction, PropertyPath, &data);
        (void)DataPublisher_PublishTransacted(transaction, PropertyPath_2, &data2);
        umock_c_reset_all_calls();

        STRICT_EXPECTED_CALL(DataMarshaller_SendData(IGNORED_PTR_ARG, 2, IGNORED_PTR_ARG, &destination, &destinationSize))
            .IgnoreArgument_dataMarshallerHandle()
            .IgnoreArgument_values();
        EXPECTED_CALL(Destroy_AGENT_DATA_TYPE(IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument_ptr();
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument_ptr();
        EXPECTED_CALL(Destroy_AGENT_DATA_TYPE(IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument_ptr();
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument_ptr();
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument_ptr();
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument_ptr();

        // act
        DATA_PUBLISHER_RESULT result = DataPublisher_EndTransaction(transaction, &destination, &destinationSize);

        // assert
        ASSERT_ARE_EQUAL(DATA_PUBLISHER_RESULT, DATA_PUBLISHER_OK, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        // cleanup
        DataPublisher_Destroy(handle);
    }

    /* Tests_SRS_DATA_PUBLISHER_99_040:[ When PropertyPath does not exist in the supplied model, DataPublisher_Publish shall return DATA_PUBLISHER_SCHEMA_FAILED without dispatching data.] */
    TEST_FUNCTION(DataPublisher_When_Getting_The_ModelProperty_For_The_Second_Property_Fails_Only_One_Property_Remains_Associated_With_The_Transaction)
    {
        // arrange
        DATA_PUBLISHER_HANDLE handle = DataPublisher_Create(TEST_MODEL_HANDLE, true);
        unsigned char* destination;
        size_t destinationSize;
        TRANSACTION_HANDLE transaction = DataPublisher_StartTransaction(handle);
        AGENT_DATA_TYPE data2;

        data2.type = EDM_SINGLE_TYPE;
        data2.value.edmSingle.value = 3.7f;

        DATA_MARSHALLER_VALUE value;
        value.PropertyPath = PropertyPath;
        value.Value = &data;

        (void)DataPublisher_PublishTransacted(transaction, PropertyPath, &data);
        umock_c_reset_all_calls();
        
        STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, PropertyPath_2))
            .IgnoreArgument_destination();
        STRICT_EXPECTED_CALL(Schema_ModelPropertyByPathExists(TEST_MODEL_HANDLE, PropertyPath_2))
            .SetReturn(false);
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument_ptr();

        STRICT_EXPECTED_CALL(DataMarshaller_SendData(IGNORED_PTR_ARG, 1, &value, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument_dataMarshallerHandle()
            .IgnoreArgument_values()
            .IgnoreArgument(4)
            .IgnoreArgument(5);
        (void)DataPublisher_PublishTransacted(transaction, PropertyPath_2, &data2);

        EXPECTED_CALL(Destroy_AGENT_DATA_TYPE(IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument_ptr();
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument_ptr();
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument_ptr();
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument_ptr();
        // act
        DATA_PUBLISHER_RESULT result = DataPublisher_EndTransaction(transaction, &destination, &destinationSize);

        // assert
        ASSERT_ARE_EQUAL(DATA_PUBLISHER_RESULT, DATA_PUBLISHER_OK, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        // cleanup
        DataPublisher_Destroy(handle);
    }

    /* Tests_SRS_DATA_PUBLISHER_99_012:[ DataPublisher_EndTransaction shall dispose of any resources associated with the transaction.] */
    TEST_FUNCTION(DataPublisher_EndTransaction_Destroys_The_Cloned_Agent_Data_Type)
    {
        // arrange
        DATA_PUBLISHER_HANDLE handle = DataPublisher_Create(TEST_SCHEMA_MODEL_TYPE_HANDLE, true);
        unsigned char* destination;
        size_t destinationSize;
        TRANSACTION_HANDLE transaction = DataPublisher_StartTransaction(handle);
        DATA_PUBLISHER_RESULT result = DataPublisher_PublishTransacted(transaction, PropertyPath, &data);
        umock_c_reset_all_calls();

        STRICT_EXPECTED_CALL(DataMarshaller_SendData(IGNORED_PTR_ARG, 1, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .IgnoreArgument_dataMarshallerHandle()
            .IgnoreArgument_values()
            .IgnoreArgument(4)
            .IgnoreArgument(5);

        EXPECTED_CALL(Destroy_AGENT_DATA_TYPE(IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument_ptr();
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument_ptr();
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument_ptr();
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument_ptr();

        // act
        (void)DataPublisher_EndTransaction(transaction, &destination, &destinationSize);

        // assert
        ASSERT_ARE_EQUAL(DATA_PUBLISHER_RESULT, DATA_PUBLISHER_OK, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        // cleanup
        DataPublisher_Destroy(handle);
    }

    /* DataPublisher_CancelTransaction */

    /* Tests_SRS_DATA_PUBLISHER_99_013:[ A call to DataPublisher_CancelTransaction shall dispose of the transaction without dispatching the data to the DataMarshaller module and it shall return DATA_PUBLISHER_OK.] */
    TEST_FUNCTION(DataPublisher_CancelTransaction_Does_Not_Dispatch_Data)
    {
        // arrange
        DATA_PUBLISHER_HANDLE handle = DataPublisher_Create(TEST_SCHEMA_MODEL_TYPE_HANDLE, true);
        TRANSACTION_HANDLE transaction = DataPublisher_StartTransaction(handle);
        (void)DataPublisher_PublishTransacted(transaction, PropertyPath, &data);
        umock_c_reset_all_calls();

        EXPECTED_CALL(Destroy_AGENT_DATA_TYPE(IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument_ptr();
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument_ptr();
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument_ptr();
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument_ptr();

        // act
        DATA_PUBLISHER_RESULT result = DataPublisher_CancelTransaction(transaction);

        // assert
        ASSERT_ARE_EQUAL(DATA_PUBLISHER_RESULT, DATA_PUBLISHER_OK, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        // cleanup
        DataPublisher_Destroy(handle);
    }

    /* Tests_SRS_DATA_PUBLISHER_99_014:[ If the transactionHandle argument is NULL DataPublisher_CancelTransaction shall return DATA_PUBLISHER_INVALID_ARG.] */
    TEST_FUNCTION(DataPublisher_CancelTransaction_With_A_NULL_Transaction_Fails)
    {
        // arrange

        // act
        DATA_PUBLISHER_RESULT result = DataPublisher_CancelTransaction(NULL);

        // assert
        ASSERT_ARE_EQUAL(DATA_PUBLISHER_RESULT, DATA_PUBLISHER_INVALID_ARG, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    }

    /* Tests_SRS_DATA_PUBLISHER_99_015:[ DataPublisher_CancelTransaction shall dispose of any resources associated with the transaction.] */
    TEST_FUNCTION(DataPublisher_CancelTransaction_Destroys_The_Cloned_Agent_Data_Type)
    {
        // arrange
        DATA_PUBLISHER_HANDLE handle = DataPublisher_Create(TEST_SCHEMA_MODEL_TYPE_HANDLE, true);
        TRANSACTION_HANDLE transaction = DataPublisher_StartTransaction(handle);
        (void)DataPublisher_PublishTransacted(transaction, PropertyPath, &data);
        umock_c_reset_all_calls();

        EXPECTED_CALL(Destroy_AGENT_DATA_TYPE(IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument_ptr();
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument_ptr();
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument_ptr();
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument_ptr();

        // act
        (void)DataPublisher_CancelTransaction(transaction);

        // assert
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        // cleanup
        DataPublisher_Destroy(handle);
    }

    /* Tests_SRS_DATA_PUBLISHER_99_067:[ Before any call to DataPublisher_SetMaxBufferSize, the default max buffer size shall be equal to 10KB.] */
    TEST_FUNCTION(DataPublisher_default_max_buffer_size_should_be_10KB)
    {
        // arrange

        // act
        size_t size = DataPublisher_GetMaxBufferSize();

        // assert
        ASSERT_ARE_EQUAL(size_t, 10 * 1024, size);
    }

    /* Tests_SRS_DATA_PUBLISHER_99_065:[ DataPublisher_SetMaxBufferSize shall directly update the value used to limit how much data (in bytes) can be buffered in the BufferStorage instance.] */
    /* Tests_SRS_DATA_PUBLISHER_99_069:[ DataMarshaller_GetMaxBufferSize shall return the current max buffer size value used by any new instance of DataMarshaller.] */
    TEST_FUNCTION(DataPublisher_SetMaxBufferSize_should_update_max_buffer_size_value)
    {
        // arrange
        DataPublisher_SetMaxBufferSize(42);

        // act
        size_t result = DataPublisher_GetMaxBufferSize();

        // assert
        ASSERT_ARE_EQUAL(size_t, 42, result);

        ///cleanup
        DataPublisher_SetMaxBufferSize(10*1024);
    }

    /*Tests_SRS_DATA_PUBLISHER_02_027: [ If argument dataPublisherHandle is NULL then DataPublisher_CreateTransaction_ReportedProperties shall fail and return NULL. ]*/
    TEST_FUNCTION(DataPublisher_CreateTransaction_ReportedProperties_with_NULL_dataPublisherHandle_fails)
    {
        ///arrange

        ///act
        REPORTED_PROPERTIES_TRANSACTION_HANDLE handle = DataPublisher_CreateTransaction_ReportedProperties(NULL);

        ///assert
        ASSERT_IS_NULL(handle);

        ///cleanup
    }

    void DataPublisher_CreateTransaction_ReportedProperties_inert_path(void)
    {
        STRICT_EXPECTED_CALL(gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument_size();
        STRICT_EXPECTED_CALL(VECTOR_create(sizeof(void*)));
    }

    /*Tests_SRS_DATA_PUBLISHER_02_028: [ DataPublisher_CreateTransaction_ReportedProperties shall create a VECTOR_HANDLE holding the individual elements of the transaction (DATA_MARSHALLER_VALUE). ]*/
    /*Tests_SRS_DATA_PUBLISHER_02_030: [ Otherwise DataPublisher_CreateTransaction_ReportedProperties shall succeed and return a non-NULL handle. ]*/
    TEST_FUNCTION(DataPublisher_CreateTransaction_ReportedProperties_succeeds)
    {
        ///arrange
        DATA_PUBLISHER_HANDLE dataPublisherHandle = DataPublisher_Create(TEST_SCHEMA_MODEL_TYPE_HANDLE, true);
        umock_c_reset_all_calls();

        DataPublisher_CreateTransaction_ReportedProperties_inert_path();

        ///act
        REPORTED_PROPERTIES_TRANSACTION_HANDLE handle = DataPublisher_CreateTransaction_ReportedProperties(dataPublisherHandle);

        ///assert
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
        ASSERT_IS_NOT_NULL(handle);
        
        ///cleanup
        DataPublisher_DestroyTransaction_ReportedProperties(handle);
        DataPublisher_Destroy(dataPublisherHandle);
    }

    /*Tests_SRS_DATA_PUBLISHER_02_029: [ If any error occurs then DataPublisher_CreateTransaction_ReportedProperties shall fail and return NULL. ]*/
    TEST_FUNCTION(DataPublisher_CreateTransaction_ReportedProperties_fails)
    {
        ///arrange
        DATA_PUBLISHER_HANDLE dataPublisherHandle = DataPublisher_Create(TEST_SCHEMA_MODEL_TYPE_HANDLE, true);
        umock_c_reset_all_calls();

        (void)umock_c_negative_tests_init();

        DataPublisher_CreateTransaction_ReportedProperties_inert_path();

        umock_c_negative_tests_snapshot();

        for (size_t i = 0; i < umock_c_negative_tests_call_count(); i++)
        {

            umock_c_negative_tests_reset();
            umock_c_negative_tests_fail_call(i);

            ///act
            char temp_str[128];
            (void)sprintf(temp_str, "On failed call %zu", i);

            ///act
            REPORTED_PROPERTIES_TRANSACTION_HANDLE handle = DataPublisher_CreateTransaction_ReportedProperties(dataPublisherHandle);

            ///assert
            ASSERT_IS_NULL_WITH_MSG(handle, temp_str);
        }

        ///cleanup
        DataPublisher_Destroy(dataPublisherHandle);
        umock_c_negative_tests_deinit();
    }

    /*Tests_SRS_DATA_PUBLISHER_02_009: [ If argument transactionHandle is NULL then DataPublisher_PublishTransacted_ReportedProperty shall fail and return DATA_PUBLISHER_INVALID_ARG. ]*/
    TEST_FUNCTION(DataPublisher_PublishTransacted_ReportedProperty_with_NULL_transactionHandle_fails)
    {
        ///arrange
        AGENT_DATA_TYPE ag;

        ///act
        DATA_PUBLISHER_RESULT result = DataPublisher_PublishTransacted_ReportedProperty(NULL, "a", &ag);

        ///assert
        ASSERT_ARE_EQUAL(DATA_PUBLISHER_RESULT, DATA_PUBLISHER_INVALID_ARG, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///clean
    }

    /*Tests_SRS_DATA_PUBLISHER_02_010: [ If argument reportedPropertyPath is NULL then DataPublisher_PublishTransacted_ReportedProperty shall fail and return DATA_PUBLISHER_INVALID_ARG. ]*/
    TEST_FUNCTION(DataPublisher_PublishTransacted_ReportedProperty_with_NULL_reportedPropertyPath_fails)
    {
        ///arrange
        AGENT_DATA_TYPE ag;
        ag.type = EDM_BYTE_TYPE;
        ag.value.edmByte.value = 1;
        DATA_PUBLISHER_HANDLE dataPublisherHandle = DataPublisher_Create(TEST_SCHEMA_MODEL_TYPE_HANDLE, true);
        REPORTED_PROPERTIES_TRANSACTION_HANDLE handle = DataPublisher_CreateTransaction_ReportedProperties(dataPublisherHandle);
        umock_c_reset_all_calls();

        ///act
        DATA_PUBLISHER_RESULT result = DataPublisher_PublishTransacted_ReportedProperty(handle, NULL, &ag);

        ///assert
        ASSERT_ARE_EQUAL(DATA_PUBLISHER_RESULT, DATA_PUBLISHER_INVALID_ARG, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///clean
        DataPublisher_DestroyTransaction_ReportedProperties(handle);
        DataPublisher_Destroy(dataPublisherHandle);
    }

    /*Tests_SRS_DATA_PUBLISHER_02_011: [ If argument data is NULL then DataPublisher_PublishTransacted_ReportedProperty shall fail and return DATA_PUBLISHER_INVALID_ARG. ]*/
    TEST_FUNCTION(DataPublisher_PublishTransacted_ReportedProperty_with_NULL_data_fails)
    {
        ///arrange
        DATA_PUBLISHER_HANDLE dataPublisherHandle = DataPublisher_Create(TEST_SCHEMA_MODEL_TYPE_HANDLE, true);
        REPORTED_PROPERTIES_TRANSACTION_HANDLE handle = DataPublisher_CreateTransaction_ReportedProperties(dataPublisherHandle);
        umock_c_reset_all_calls();

        ///act
        DATA_PUBLISHER_RESULT result = DataPublisher_PublishTransacted_ReportedProperty(handle, "A", NULL);

        ///assert
        ASSERT_ARE_EQUAL(DATA_PUBLISHER_RESULT, DATA_PUBLISHER_INVALID_ARG, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///clean
        DataPublisher_DestroyTransaction_ReportedProperties(handle);
        DataPublisher_Destroy(dataPublisherHandle);
    }

    void DataPublisher_PublishTransacted_ReportedProperty_new_property_inert_path(const char* reportedPropertyPath, AGENT_DATA_TYPE* ag)
    {
        STRICT_EXPECTED_CALL(Schema_ModelReportedPropertyByPathExists(TEST_SCHEMA_MODEL_TYPE_HANDLE, reportedPropertyPath));
        STRICT_EXPECTED_CALL(VECTOR_find_if(IGNORED_PTR_ARG, IGNORED_PTR_ARG, reportedPropertyPath))
            .IgnoreArgument_handle()
            .IgnoreArgument_pred();
        STRICT_EXPECTED_CALL(gballoc_malloc(sizeof(DATA_MARSHALLER_VALUE)));
        STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, reportedPropertyPath))
            .IgnoreArgument_destination();
        STRICT_EXPECTED_CALL(gballoc_malloc(sizeof(AGENT_DATA_TYPE)));
        STRICT_EXPECTED_CALL(Create_AGENT_DATA_TYPE_from_AGENT_DATA_TYPE(IGNORED_PTR_ARG, ag))
            .IgnoreArgument_dest();
        STRICT_EXPECTED_CALL(VECTOR_push_back(IGNORED_PTR_ARG, IGNORED_PTR_ARG, 1))
            .IgnoreArgument_elements()
            .IgnoreArgument_handle();
    }

    /*Tests_SRS_DATA_PUBLISHER_02_015: [ DataPublisher_PublishTransacted_ReportedProperty shall add a new DATA_MARSHALLER_VALUE to the VECTOR_HANDLE. ]*/
    /*Tests_SRS_DATA_PUBLISHER_02_017: [ Otherwise DataPublisher_PublishTransacted_ReportedProperty shall succeed and return DATA_PUBLISHER_OK. ]*/
    TEST_FUNCTION(DataPublisher_PublishTransacted_ReportedProperty_new_property_happy_path)
    {
        ///arrange
        AGENT_DATA_TYPE ag;
        ag.type = EDM_BYTE_TYPE;
        ag.value.edmByte.value = 1;
        DATA_PUBLISHER_HANDLE dataPublisherHandle = DataPublisher_Create(TEST_SCHEMA_MODEL_TYPE_HANDLE, true);
        REPORTED_PROPERTIES_TRANSACTION_HANDLE handle = DataPublisher_CreateTransaction_ReportedProperties(dataPublisherHandle);
        const char* reportedPropertyPath = "A";
        umock_c_reset_all_calls();

        DataPublisher_PublishTransacted_ReportedProperty_new_property_inert_path(reportedPropertyPath, &ag);

        ///act
        DATA_PUBLISHER_RESULT result = DataPublisher_PublishTransacted_ReportedProperty(handle, reportedPropertyPath, &ag);

        ///assert
        ASSERT_ARE_EQUAL(DATA_PUBLISHER_RESULT, DATA_PUBLISHER_OK, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///clean
        DataPublisher_DestroyTransaction_ReportedProperties(handle);
        DataPublisher_Destroy(dataPublisherHandle);
    }

    /*Tests_SRS_DATA_PUBLISHER_02_023: [ If any error occurs then DataPublisher_CommitTransaction_ReportedProperties shall fail and return DATA_PUBLISHER_ERROR. ]*/
    /*Tests_SRS_DATA_PUBLISHER_02_013: [ If a reported property with path reportedPropertyPath does not exist in the model then DataPublisher_PublishTransacted_ReportedProperty shall fail and return DATA_PUBLISHER_INVALID_ARG. ]*/
    TEST_FUNCTION(DataPublisher_PublishTransacted_ReportedProperty_new_property_unhappy_paths)
    {
        ///arrange
        AGENT_DATA_TYPE ag;
        ag.type = EDM_BYTE_TYPE;
        ag.value.edmByte.value = 1;
        DATA_PUBLISHER_HANDLE dataPublisherHandle = DataPublisher_Create(TEST_SCHEMA_MODEL_TYPE_HANDLE, true);
        REPORTED_PROPERTIES_TRANSACTION_HANDLE handle = DataPublisher_CreateTransaction_ReportedProperties(dataPublisherHandle);
        const char* reportedPropertyPath = "A";
        umock_c_reset_all_calls();
        umock_c_negative_tests_init();

        DataPublisher_PublishTransacted_ReportedProperty_new_property_inert_path(reportedPropertyPath, &ag);

        umock_c_negative_tests_snapshot();

        size_t calls_that_cannot_fail[] =
        {
            1, /*VECTOR_find_if*/
        };

        for (size_t i = 0; i < umock_c_negative_tests_call_count(); i++)
        {
            size_t j;
            for (j = 0;j < sizeof(calls_that_cannot_fail) / sizeof(calls_that_cannot_fail[0]);j++)
            {
                if (calls_that_cannot_fail[j] == i)
                {
                    goto next_fail;
                }
            }

            umock_c_negative_tests_reset();
            umock_c_negative_tests_fail_call(i);

            char temp_str[128];
            (void)sprintf(temp_str, "On failed call %zu", i);

            ///act
            DATA_PUBLISHER_RESULT result = DataPublisher_PublishTransacted_ReportedProperty(handle, reportedPropertyPath, &ag);

            ///assert
            ASSERT_ARE_NOT_EQUAL_WITH_MSG(DATA_PUBLISHER_RESULT, DATA_PUBLISHER_OK, result, temp_str);
next_fail:;
        }
        ///clean

        umock_c_negative_tests_deinit();
        DataPublisher_DestroyTransaction_ReportedProperties(handle);
        DataPublisher_Destroy(dataPublisherHandle);
    }

    void DataPublisher_PublishTransacted_ReportedProperty_new_property_after_property_inert_path(const char* reportedPropertyPath, AGENT_DATA_TYPE* ag)
    {
        STRICT_EXPECTED_CALL(Schema_ModelReportedPropertyByPathExists(TEST_SCHEMA_MODEL_TYPE_HANDLE, reportedPropertyPath));
        STRICT_EXPECTED_CALL(VECTOR_find_if(IGNORED_PTR_ARG, IGNORED_PTR_ARG, reportedPropertyPath))
            .IgnoreArgument_handle()
            .IgnoreArgument_pred();
        STRICT_EXPECTED_CALL(gballoc_malloc(sizeof(DATA_MARSHALLER_VALUE)));
        STRICT_EXPECTED_CALL(mallocAndStrcpy_s(IGNORED_PTR_ARG, reportedPropertyPath))
            .IgnoreArgument_destination();
        STRICT_EXPECTED_CALL(gballoc_malloc(sizeof(AGENT_DATA_TYPE)));
        STRICT_EXPECTED_CALL(Create_AGENT_DATA_TYPE_from_AGENT_DATA_TYPE(IGNORED_PTR_ARG, ag))
            .IgnoreArgument_dest();
        STRICT_EXPECTED_CALL(VECTOR_push_back(IGNORED_PTR_ARG, IGNORED_PTR_ARG, 1))
            .IgnoreArgument_elements()
            .IgnoreArgument_handle();
    }

    /*Tests_SRS_DATA_PUBLISHER_02_015: [ DataPublisher_PublishTransacted_ReportedProperty shall add a new DATA_MARSHALLER_VALUE to the VECTOR_HANDLE. ]*/
    /*Tests_SRS_DATA_PUBLISHER_02_017: [ Otherwise DataPublisher_PublishTransacted_ReportedProperty shall succeed and return DATA_PUBLISHER_OK. ]*/
    TEST_FUNCTION(DataPublisher_PublishTransacted_ReportedProperty_new_property_after_property_happy_path)
    {
        ///arrange
        AGENT_DATA_TYPE ag;
        ag.type = EDM_BYTE_TYPE;
        ag.value.edmByte.value = 1;
        DATA_PUBLISHER_HANDLE dataPublisherHandle = DataPublisher_Create(TEST_SCHEMA_MODEL_TYPE_HANDLE, true);
        REPORTED_PROPERTIES_TRANSACTION_HANDLE handle = DataPublisher_CreateTransaction_ReportedProperties(dataPublisherHandle);
        (void)DataPublisher_PublishTransacted_ReportedProperty(handle, "Z", &ag);
        const char* reportedPropertyPath = "A";
        umock_c_reset_all_calls();

        DataPublisher_PublishTransacted_ReportedProperty_new_property_after_property_inert_path(reportedPropertyPath, &ag);

        ///act
        DATA_PUBLISHER_RESULT result = DataPublisher_PublishTransacted_ReportedProperty(handle, reportedPropertyPath, &ag);

        ///assert
        ASSERT_ARE_EQUAL(DATA_PUBLISHER_RESULT, DATA_PUBLISHER_OK, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///clean
        DataPublisher_DestroyTransaction_ReportedProperties(handle);
        DataPublisher_Destroy(dataPublisherHandle);
    }

    /*Tests_SRS_DATA_PUBLISHER_02_023: [ If any error occurs then DataPublisher_CommitTransaction_ReportedProperties shall fail and return DATA_PUBLISHER_ERROR. ]*/
    /*Tests_SRS_DATA_PUBLISHER_02_013: [ If a reported property with path reportedPropertyPath does not exist in the model then DataPublisher_PublishTransacted_ReportedProperty shall fail and return DATA_PUBLISHER_INVALID_ARG. ]*/
    TEST_FUNCTION(DataPublisher_PublishTransacted_ReportedProperty_new_property_after_property_unhappy_paths)
    {
        ///arrange
        AGENT_DATA_TYPE ag;
        ag.type = EDM_BYTE_TYPE;
        ag.value.edmByte.value = 1;
        DATA_PUBLISHER_HANDLE dataPublisherHandle = DataPublisher_Create(TEST_SCHEMA_MODEL_TYPE_HANDLE, true);
        REPORTED_PROPERTIES_TRANSACTION_HANDLE handle = DataPublisher_CreateTransaction_ReportedProperties(dataPublisherHandle);
        const char* reportedPropertyPath = "A";
        umock_c_reset_all_calls();
        umock_c_negative_tests_init();

        DataPublisher_PublishTransacted_ReportedProperty_new_property_after_property_inert_path(reportedPropertyPath, &ag);

        umock_c_negative_tests_snapshot();

        size_t calls_that_cannot_fail[] =
        {
            1, /*VECTOR_find_if*/
        };

        for (size_t i = 0; i < umock_c_negative_tests_call_count(); i++)
        {
            size_t j;
            for (j = 0;j < sizeof(calls_that_cannot_fail) / sizeof(calls_that_cannot_fail[0]);j++)
            {
                if (calls_that_cannot_fail[j] == i)
                {
                    goto next_fail;
                }
            }

            umock_c_negative_tests_reset();
            umock_c_negative_tests_fail_call(i);

            char temp_str[128];
            (void)sprintf(temp_str, "On failed call %zu", i);

            ///act
            DATA_PUBLISHER_RESULT result = DataPublisher_PublishTransacted_ReportedProperty(handle, reportedPropertyPath, &ag);

            ///assert
            ASSERT_ARE_NOT_EQUAL_WITH_MSG(DATA_PUBLISHER_RESULT, DATA_PUBLISHER_OK, result, temp_str);
        next_fail:;
        }

        ///clean
        umock_c_negative_tests_deinit();
        DataPublisher_DestroyTransaction_ReportedProperties(handle);
        DataPublisher_Destroy(dataPublisherHandle);
    }

    void DataPublisher_PublishTransacted_ReportedProperty_same_reportedPropertyPath_updates_property_inert_path(const char* reportedPropertyPath, AGENT_DATA_TYPE* ag)
    {
        STRICT_EXPECTED_CALL(Schema_ModelReportedPropertyByPathExists(TEST_SCHEMA_MODEL_TYPE_HANDLE, reportedPropertyPath));
        STRICT_EXPECTED_CALL(VECTOR_find_if(IGNORED_PTR_ARG, IGNORED_PTR_ARG, reportedPropertyPath))
            .IgnoreArgument_handle()
            .IgnoreArgument_pred();
        STRICT_EXPECTED_CALL(gballoc_malloc(sizeof(AGENT_DATA_TYPE)));
        STRICT_EXPECTED_CALL(Create_AGENT_DATA_TYPE_from_AGENT_DATA_TYPE(IGNORED_PTR_ARG, ag))
            .IgnoreArgument_dest();
        STRICT_EXPECTED_CALL(Destroy_AGENT_DATA_TYPE(IGNORED_PTR_ARG))
            .IgnoreArgument_agentData();
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument_ptr();
    }

    /*Tests_SRS_DATA_PUBLISHER_02_014: [ If the same (by reportedPropertypath) reported property has already been added to the transaction, then DataPublisher_PublishTransacted_ReportedProperty shall overwrite the previous reported property. ]*/
    TEST_FUNCTION(DataPublisher_PublishTransacted_ReportedProperty_same_reportedPropertyPath_updates_property_happy_path)
    {
        ///arrange
        AGENT_DATA_TYPE ag1;
        ag1.type = EDM_BYTE_TYPE;
        ag1.value.edmByte.value = 1;
        AGENT_DATA_TYPE ag2;
        ag2.type = EDM_SBYTE_TYPE;
        ag2.value.edmSbyte.value = -31;
        DATA_PUBLISHER_HANDLE dataPublisherHandle = DataPublisher_Create(TEST_SCHEMA_MODEL_TYPE_HANDLE, true);
        const char* reportedPropertyPath = "A";
        REPORTED_PROPERTIES_TRANSACTION_HANDLE handle = DataPublisher_CreateTransaction_ReportedProperties(dataPublisherHandle);
        (void)DataPublisher_PublishTransacted_ReportedProperty(handle, reportedPropertyPath, &ag1);
        umock_c_reset_all_calls();

        DataPublisher_PublishTransacted_ReportedProperty_same_reportedPropertyPath_updates_property_inert_path(reportedPropertyPath, &ag2);

        ///act
        DATA_PUBLISHER_RESULT result = DataPublisher_PublishTransacted_ReportedProperty(handle, reportedPropertyPath, &ag2);

        ///assert
        ASSERT_ARE_EQUAL(DATA_PUBLISHER_RESULT, DATA_PUBLISHER_OK, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///clean
        DataPublisher_DestroyTransaction_ReportedProperties(handle);
        DataPublisher_Destroy(dataPublisherHandle);
    }

    /*Tests_SRS_DATA_PUBLISHER_02_016: [ If any error occurs then DataPublisher_PublishTransacted_ReportedProperty shall fail and return DATA_PUBLISHER_ERROR. ]*/
    TEST_FUNCTION(DataPublisher_PublishTransacted_ReportedProperty_same_reportedPropertyPath_updates_property_unhappy_paths)
    {
        ///arrange
        AGENT_DATA_TYPE ag1;
        ag1.type = EDM_BYTE_TYPE;
        ag1.value.edmByte.value = 1;
        AGENT_DATA_TYPE ag2;
        ag2.type = EDM_SBYTE_TYPE;
        ag2.value.edmSbyte.value = -31;
        DATA_PUBLISHER_HANDLE dataPublisherHandle = DataPublisher_Create(TEST_SCHEMA_MODEL_TYPE_HANDLE, true);
        const char* reportedPropertyPath = "A";
        REPORTED_PROPERTIES_TRANSACTION_HANDLE handle = DataPublisher_CreateTransaction_ReportedProperties(dataPublisherHandle);
        (void)DataPublisher_PublishTransacted_ReportedProperty(handle, reportedPropertyPath, &ag1);
        umock_c_reset_all_calls();

        umock_c_negative_tests_init();

        DataPublisher_PublishTransacted_ReportedProperty_same_reportedPropertyPath_updates_property_inert_path(reportedPropertyPath, &ag2);

        umock_c_negative_tests_snapshot();

        size_t calls_that_cannot_fail[] =
        {
            1, /*VECTOR_find_if*/
            4, /*Destroy_AGENT_DATA_TYPE*/
            5, /*gballoc_free*/
        };


        for (size_t i = 0; i < umock_c_negative_tests_call_count(); i++)
        {
            size_t j;
            for (j = 0;j < sizeof(calls_that_cannot_fail) / sizeof(calls_that_cannot_fail[0]);j++)
            {
                if (calls_that_cannot_fail[j] == i)
                {
                    goto next_fail;
                }
            }

            umock_c_negative_tests_reset();
            umock_c_negative_tests_fail_call(i);

            char temp_str[128];
            (void)sprintf(temp_str, "On failed call %zu", i);

            ///act
            DATA_PUBLISHER_RESULT result = DataPublisher_PublishTransacted_ReportedProperty(handle, reportedPropertyPath, &ag2);

            ///assert
            ASSERT_ARE_NOT_EQUAL_WITH_MSG(DATA_PUBLISHER_RESULT, DATA_PUBLISHER_OK, result, temp_str);
        next_fail:;
        }

        ///clean
        umock_c_negative_tests_deinit();
        DataPublisher_DestroyTransaction_ReportedProperties(handle);
        DataPublisher_Destroy(dataPublisherHandle);
    }

    /*Tests_SRS_DATA_PUBLISHER_02_019: [ If argument transactionHandle is NULL then DataPublisher_CommitTransaction_ReportedProperties shall fail and return DATA_PUBLISHER_INVALID_ARG. ]*/
    TEST_FUNCTION(DataPublisher_CommitTransaction_ReportedProperties_with_NULL_transactionHandle_fails)
    {
        ///arrange
        unsigned char* destination;
        size_t destinationSize;

        ///act
        DATA_PUBLISHER_RESULT result = DataPublisher_CommitTransaction_ReportedProperties(NULL, &destination, &destinationSize);

        ///assert
        ASSERT_ARE_EQUAL(DATA_PUBLISHER_RESULT, DATA_PUBLISHER_INVALID_ARG, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
    }

    /*Tests_SRS_DATA_PUBLISHER_02_020: [ If argument destination is NULL then DataPublisher_CommitTransaction_ReportedProperties shall fail and return DATA_PUBLISHER_INVALID_ARG. ]*/
    TEST_FUNCTION(DataPublisher_CommitTransaction_ReportedProperties_with_NULL_destination_fails)
    {
        ///arrange
        DATA_PUBLISHER_HANDLE dataPublisherHandle = DataPublisher_Create(TEST_SCHEMA_MODEL_TYPE_HANDLE, true);
        REPORTED_PROPERTIES_TRANSACTION_HANDLE handle = DataPublisher_CreateTransaction_ReportedProperties(dataPublisherHandle);
        size_t destinationSize;
        umock_c_reset_all_calls();

        ///act
        DATA_PUBLISHER_RESULT result = DataPublisher_CommitTransaction_ReportedProperties(handle, NULL, &destinationSize);

        ///assert
        ASSERT_ARE_EQUAL(DATA_PUBLISHER_RESULT, DATA_PUBLISHER_INVALID_ARG, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        DataPublisher_DestroyTransaction_ReportedProperties(handle);
        DataPublisher_Destroy(dataPublisherHandle);
    }

    /*Tests_SRS_DATA_PUBLISHER_02_021: [ If argument destinationSize NULL then DataPublisher_CommitTransaction_ReportedProperties shall fail and return DATA_PUBLISHER_INVALID_ARG. ]*/
    TEST_FUNCTION(DataPublisher_CommitTransaction_ReportedProperties_with_NULL_destinationSize_fails)
    {
        ///arrange
        DATA_PUBLISHER_HANDLE dataPublisherHandle = DataPublisher_Create(TEST_SCHEMA_MODEL_TYPE_HANDLE, true);
        REPORTED_PROPERTIES_TRANSACTION_HANDLE handle = DataPublisher_CreateTransaction_ReportedProperties(dataPublisherHandle);
        unsigned char* destination;
        umock_c_reset_all_calls();

        ///act
        DATA_PUBLISHER_RESULT result = DataPublisher_CommitTransaction_ReportedProperties(handle, &destination, NULL);

        ///assert
        ASSERT_ARE_EQUAL(DATA_PUBLISHER_RESULT, DATA_PUBLISHER_INVALID_ARG, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        DataPublisher_DestroyTransaction_ReportedProperties(handle);
        DataPublisher_Destroy(dataPublisherHandle);
    }

    /*Tests_SRS_DATA_PUBLISHER_02_031: [ If the transaction contains zero elements then DataPublisher_CommitTransaction_ReportedProperties shall fail and return DATA_PUBLISHER_INVALID_ARG. ]*/
    TEST_FUNCTION(DataPublisher_CommitTransaction_ReportedProperties_with_zero_elements_fails)
    {
        ///arrange
        DATA_PUBLISHER_HANDLE dataPublisherHandle = DataPublisher_Create(TEST_SCHEMA_MODEL_TYPE_HANDLE, true);
        REPORTED_PROPERTIES_TRANSACTION_HANDLE handle = DataPublisher_CreateTransaction_ReportedProperties(dataPublisherHandle);
        unsigned char* destination;
        size_t destinationSize;
        umock_c_reset_all_calls();

        STRICT_EXPECTED_CALL(VECTOR_size(IGNORED_PTR_ARG))
            .IgnoreArgument_handle();

        ///act
        DATA_PUBLISHER_RESULT result = DataPublisher_CommitTransaction_ReportedProperties(handle, &destination, &destinationSize);

        ///assert
        ASSERT_ARE_EQUAL(DATA_PUBLISHER_RESULT, DATA_PUBLISHER_INVALID_ARG, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        DataPublisher_DestroyTransaction_ReportedProperties(handle);
        DataPublisher_Destroy(dataPublisherHandle);
    }

    /*Tests_SRS_DATA_PUBLISHER_02_022: [ DataPublisher_CommitTransaction_ReportedProperties shall call DataMarshaller_SendData_ReportedProperties providing the VECTOR_HANDLE holding the transacted reported properties, destination and destinationSize. ]*/
    /*Tests_SRS_DATA_PUBLISHER_02_024: [ Otherwise DataPublisher_CommitTransaction_ReportedProperties shall succeed and return DATA_PUBLISHER_OK. ]*/
    TEST_FUNCTION(DataPublisher_CommitTransaction_ReportedProperties_succeeds)
    {
        ///arrange
        AGENT_DATA_TYPE ag1;
        ag1.type = EDM_BYTE_TYPE;
        ag1.value.edmByte.value = 1;
        DATA_PUBLISHER_HANDLE dataPublisherHandle = DataPublisher_Create(TEST_SCHEMA_MODEL_TYPE_HANDLE, true);
        REPORTED_PROPERTIES_TRANSACTION_HANDLE handle = DataPublisher_CreateTransaction_ReportedProperties(dataPublisherHandle);
        const char* reportedPropertyPath = "A";
        (void)DataPublisher_PublishTransacted_ReportedProperty(handle, reportedPropertyPath, &ag1);
        unsigned char* destination;
        size_t destinationSize;
        umock_c_reset_all_calls();

        STRICT_EXPECTED_CALL(VECTOR_size(IGNORED_PTR_ARG))
            .IgnoreArgument_handle();
        STRICT_EXPECTED_CALL(DataMarshaller_SendData_ReportedProperties(IGNORED_PTR_ARG, IGNORED_PTR_ARG, &destination, &destinationSize))
            .IgnoreArgument_dataMarshallerHandle()
            .IgnoreArgument_values();

        ///act
        DATA_PUBLISHER_RESULT result = DataPublisher_CommitTransaction_ReportedProperties(handle, &destination, &destinationSize);

        ///assert
        ASSERT_ARE_EQUAL(DATA_PUBLISHER_RESULT, DATA_PUBLISHER_OK, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        DataPublisher_DestroyTransaction_ReportedProperties(handle);
        DataPublisher_Destroy(dataPublisherHandle);
    }

    /*Tests_SRS_DATA_PUBLISHER_02_023: [ If any error occurs then DataPublisher_CommitTransaction_ReportedProperties shall fail and return DATA_PUBLISHER_ERROR. ]*/
    TEST_FUNCTION(DataPublisher_CommitTransaction_ReportedProperties_fails)
    {
        ///arrange
        AGENT_DATA_TYPE ag1;
        ag1.type = EDM_BYTE_TYPE;
        ag1.value.edmByte.value = 1;
        DATA_PUBLISHER_HANDLE dataPublisherHandle = DataPublisher_Create(TEST_SCHEMA_MODEL_TYPE_HANDLE, true);
        REPORTED_PROPERTIES_TRANSACTION_HANDLE handle = DataPublisher_CreateTransaction_ReportedProperties(dataPublisherHandle);
        const char* reportedPropertyPath = "A";
        (void)DataPublisher_PublishTransacted_ReportedProperty(handle, reportedPropertyPath, &ag1);
        unsigned char* destination;
        size_t destinationSize;
        umock_c_reset_all_calls();

        STRICT_EXPECTED_CALL(VECTOR_size(IGNORED_PTR_ARG))
            .IgnoreArgument_handle();
        STRICT_EXPECTED_CALL(DataMarshaller_SendData_ReportedProperties(IGNORED_PTR_ARG, IGNORED_PTR_ARG, &destination, &destinationSize))
            .IgnoreArgument_dataMarshallerHandle()
            .IgnoreArgument_values()
            .SetReturn(DATA_MARSHALLER_ERROR);

        ///act
        DATA_PUBLISHER_RESULT result = DataPublisher_CommitTransaction_ReportedProperties(handle, &destination, &destinationSize);

        ///assert
        ASSERT_ARE_EQUAL(DATA_PUBLISHER_RESULT, DATA_PUBLISHER_ERROR, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        DataPublisher_DestroyTransaction_ReportedProperties(handle);
        DataPublisher_Destroy(dataPublisherHandle);
    }

    /*Tests_SRS_DATA_PUBLISHER_02_025: [ If argument transactionHandle is NULL then DataPublisher_DestroyTransaction_ReportedProperties shall return. ]*/
    TEST_FUNCTION(DataPublisher_DestroyTransaction_ReportedProperties_with_NULL_transactionHandle_fails)
    {
        ///arrange

        ///act
        DataPublisher_DestroyTransaction_ReportedProperties(NULL);

        ///assert
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///clean
    }

    /*Tests_SRS_DATA_PUBLISHER_02_026: [ Otherwise DataPublisher_DestroyTransaction_ReportedProperties shall free all resources associated with the reported properties transactionHandle. ]*/
    TEST_FUNCTION(DataPublisher_DestroyTransaction_ReportedProperties__empty_transaction_succeeds)
    {
        ///arrange
        DATA_PUBLISHER_HANDLE dataPublisherHandle = DataPublisher_Create(TEST_SCHEMA_MODEL_TYPE_HANDLE, true);
        REPORTED_PROPERTIES_TRANSACTION_HANDLE handle = DataPublisher_CreateTransaction_ReportedProperties(dataPublisherHandle);
        umock_c_reset_all_calls();

        STRICT_EXPECTED_CALL(VECTOR_size(IGNORED_PTR_ARG))
            .IgnoreArgument_handle();
        STRICT_EXPECTED_CALL(VECTOR_destroy(IGNORED_PTR_ARG))
            .IgnoreArgument_handle();
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument_ptr();

        ///act
        DataPublisher_DestroyTransaction_ReportedProperties(handle);

        ///assert
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///clean
        DataPublisher_Destroy(dataPublisherHandle);
    }

    /*Tests_SRS_DATA_PUBLISHER_02_026: [ Otherwise DataPublisher_DestroyTransaction_ReportedProperties shall free all resources associated with the reported properties transactionHandle. ]*/
    TEST_FUNCTION(DataPublisher_DestroyTransaction_ReportedProperties_1_element_in_transaction_succeeds)
    {
        ///arrange
        AGENT_DATA_TYPE ag1;
        ag1.type = EDM_BYTE_TYPE;
        ag1.value.edmByte.value = 1;

        DATA_PUBLISHER_HANDLE dataPublisherHandle = DataPublisher_Create(TEST_SCHEMA_MODEL_TYPE_HANDLE, true);
        REPORTED_PROPERTIES_TRANSACTION_HANDLE handle = DataPublisher_CreateTransaction_ReportedProperties(dataPublisherHandle);
        (void)DataPublisher_PublishTransacted_ReportedProperty(handle, "AAA", &ag1);
        umock_c_reset_all_calls();

        STRICT_EXPECTED_CALL(VECTOR_size(IGNORED_PTR_ARG))
            .IgnoreArgument_handle();
        for (size_t i = 0;i < 1;i++)
        {
            STRICT_EXPECTED_CALL(VECTOR_element(IGNORED_PTR_ARG, i))
                .IgnoreArgument_handle();
            STRICT_EXPECTED_CALL(Destroy_AGENT_DATA_TYPE(IGNORED_PTR_ARG))
                .IgnoreArgument_agentData();
            STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
                .IgnoreArgument_ptr();
            STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
                .IgnoreArgument_ptr();
            STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
                .IgnoreArgument_ptr();
        }

        STRICT_EXPECTED_CALL(VECTOR_destroy(IGNORED_PTR_ARG))
            .IgnoreArgument_handle();
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument_ptr();

        ///act
        DataPublisher_DestroyTransaction_ReportedProperties(handle);

        ///assert
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///clean
        DataPublisher_Destroy(dataPublisherHandle);
    }

    /*Tests_SRS_DATA_PUBLISHER_02_026: [ Otherwise DataPublisher_DestroyTransaction_ReportedProperties shall free all resources associated with the reported properties transactionHandle. ]*/
    TEST_FUNCTION(DataPublisher_DestroyTransaction_ReportedProperties_2_elements_in_transaction_succeeds)
    {
        ///arrange
        AGENT_DATA_TYPE ag1;
        ag1.type = EDM_BYTE_TYPE;
        ag1.value.edmByte.value = 1;

        AGENT_DATA_TYPE ag2;
        ag2.type = EDM_SBYTE_TYPE;
        ag2.value.edmSbyte.value = -31;

        DATA_PUBLISHER_HANDLE dataPublisherHandle = DataPublisher_Create(TEST_SCHEMA_MODEL_TYPE_HANDLE, true);
        REPORTED_PROPERTIES_TRANSACTION_HANDLE handle = DataPublisher_CreateTransaction_ReportedProperties(dataPublisherHandle);
        (void)DataPublisher_PublishTransacted_ReportedProperty(handle, "AAA", &ag1);
        (void)DataPublisher_PublishTransacted_ReportedProperty(handle, "ZZZ", &ag2);
        umock_c_reset_all_calls();

        STRICT_EXPECTED_CALL(VECTOR_size(IGNORED_PTR_ARG))
            .IgnoreArgument_handle();
        for (size_t i = 0;i < 2;i++)
        {
            STRICT_EXPECTED_CALL(VECTOR_element(IGNORED_PTR_ARG, i))
                .IgnoreArgument_handle();
            STRICT_EXPECTED_CALL(Destroy_AGENT_DATA_TYPE(IGNORED_PTR_ARG))
                .IgnoreArgument_agentData();
            STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
                .IgnoreArgument_ptr();
            STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
                .IgnoreArgument_ptr();
            STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
                .IgnoreArgument_ptr();
        }
        STRICT_EXPECTED_CALL(VECTOR_destroy(IGNORED_PTR_ARG))
            .IgnoreArgument_handle();
        STRICT_EXPECTED_CALL(gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument_ptr();

        ///act
        DataPublisher_DestroyTransaction_ReportedProperties(handle);

        ///assert
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///clean
        DataPublisher_Destroy(dataPublisherHandle);
    }
END_TEST_SUITE(DataPublisher_ut)
