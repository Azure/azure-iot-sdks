// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <cstdlib>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "testrunnerswitcher.h"
#include "schema.h"
#include "datamarshaller.h"
#include "datapublisher.h"
#include "schema.h"
#include "iothub_client.h"
#include "micromock.h"
#include "micromockcharstararenullterminatedstrings.h"


static MICROMOCK_MUTEX_HANDLE g_testByTest;

DEFINE_MICROMOCK_ENUM_TO_STRING(DATA_PUBLISHER_RESULT, DATA_PUBLISHER_RESULT_VALUES);

static AGENT_DATA_TYPE              data;
static const SCHEMA_PROPERTY_HANDLE g_ModelPropertyHandle = (SCHEMA_PROPERTY_HANDLE)0x4243;
static const DATA_MARSHALLER_HANDLE TEST_DATA_MARSHALLER_HANDLE = (DATA_MARSHALLER_HANDLE)0x4343;
static const IOTHUB_CLIENT_HANDLE       TEST_IOTHUB_CLIENT_HANDLE = (IOTHUB_CLIENT_HANDLE)0x4444;
static bool                         g_DataSentMatches;
static const DATA_MARSHALLER_VALUE* g_ExpectedDataSentValues;


static const SCHEMA_MODEL_TYPE_HANDLE TEST_MODEL_HANDLE = (SCHEMA_PROPERTY_HANDLE)0x4242;

static DATA_PUBLISHER_HANDLE g_handle = NULL;
static TRANSACTION_HANDLE g_myTransaction = NULL;

static size_t OriginalMaxBufferSize_()
{
    static size_t value = DataPublisher_GetMaxBufferSize();
    return value;
}

time_t currentTime;
int bufferStorageDataAmount = 0;


static bool operator==(_In_ const DATA_MARSHALLER_VALUE& lhs, _In_ const DATA_MARSHALLER_VALUE& rhs)
{
    return ((strcmp(lhs.PropertyPath, rhs.PropertyPath) == 0) &&
        (lhs.Value->type == rhs.Value->type) &&
        (lhs.Value->value.edmSingle.value == rhs.Value->value.edmSingle.value));
}

TYPED_MOCK_CLASS(CDataPublisherMock, CGlobalMock)
{
public:
    /* Tests_SRS_DATA_PUBLISHER_99_002:[ The DataPublisher module shall make use of the Schema module APIs to query schema information.] */
    MOCK_STATIC_METHOD_2(, bool, Schema_ModelPropertyByPathExists, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle, const char*, propertyPath);
    MOCK_METHOD_END(bool, true)

    /* Tests_SRS_DATA_PUBLISHER_99_039:[ The DataPublisher module shall make use of the DataPublisher module APIs to dispatch data to be published.] */
    MOCK_STATIC_METHOD_2(, DATA_MARSHALLER_HANDLE, DataMarshaller_Create, SCHEMA_MODEL_TYPE_HANDLE, modelHandle, bool, includePropertyPath);
    MOCK_METHOD_END(DATA_MARSHALLER_HANDLE, TEST_DATA_MARSHALLER_HANDLE)
    MOCK_STATIC_METHOD_1(, void, DataMarshaller_Destroy, DATA_MARSHALLER_HANDLE, dataMarshallerHandle);
    MOCK_VOID_METHOD_END()
    MOCK_STATIC_METHOD_5(, DATA_MARSHALLER_RESULT, DataMarshaller_SendData, DATA_MARSHALLER_HANDLE, dataMarshallerHandle, size_t, valueCount, const DATA_MARSHALLER_VALUE*, values, unsigned char**, destination, size_t*, destinationSize);
        if (g_ExpectedDataSentValues != NULL)
        {
            size_t i;

            g_DataSentMatches = true;
            for (i = 0; i < valueCount; i++)
            {
                if (!(values[i] == ((const DATA_MARSHALLER_VALUE*)g_ExpectedDataSentValues)[i]))
                {
                    g_DataSentMatches = false;
                }
            }
        }
    MOCK_METHOD_END(DATA_MARSHALLER_RESULT, DATA_MARSHALLER_OK)

    /* AgentTypeSystem mocks */
    MOCK_STATIC_METHOD_2(, AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_AGENT_DATA_TYPE, AGENT_DATA_TYPE*, dest, const AGENT_DATA_TYPE*, src)
        if (dest != NULL)
        {
            memcpy(dest, src, sizeof(AGENT_DATA_TYPE));
        }
    MOCK_METHOD_END(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK)
    MOCK_STATIC_METHOD_1(, void, Destroy_AGENT_DATA_TYPE, AGENT_DATA_TYPE*, agentData)
    MOCK_VOID_METHOD_END()
};

DECLARE_GLOBAL_MOCK_METHOD_2(CDataPublisherMock, , bool, Schema_ModelPropertyByPathExists, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle, const char*, propertyPath);

DECLARE_GLOBAL_MOCK_METHOD_2(CDataPublisherMock, , DATA_MARSHALLER_HANDLE, DataMarshaller_Create, SCHEMA_MODEL_TYPE_HANDLE, modelHandle, bool, includePropertyPath);
DECLARE_GLOBAL_MOCK_METHOD_1(CDataPublisherMock, , void, DataMarshaller_Destroy, DATA_MARSHALLER_HANDLE, dataMarshallerHandle);
DECLARE_GLOBAL_MOCK_METHOD_5(CDataPublisherMock, , DATA_MARSHALLER_RESULT, DataMarshaller_SendData, DATA_MARSHALLER_HANDLE, dataMarshallerHandle, size_t, valueCount, const DATA_MARSHALLER_VALUE*, values, unsigned char**, destination, size_t*, destinationSize);

DECLARE_GLOBAL_MOCK_METHOD_2(CDataPublisherMock, , AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_AGENT_DATA_TYPE, AGENT_DATA_TYPE*, dest, const AGENT_DATA_TYPE*, src);
DECLARE_GLOBAL_MOCK_METHOD_1(CDataPublisherMock, , void, Destroy_AGENT_DATA_TYPE, AGENT_DATA_TYPE*, agentData);

static bool operator==(_In_ const CMockValue<const DATA_MARSHALLER_VALUE*>& lhs, _In_ const CMockValue<const DATA_MARSHALLER_VALUE*>& rhs)
{
    const DATA_MARSHALLER_VALUE* lhsValue = lhs.GetValue();
    const DATA_MARSHALLER_VALUE* rhsValue = rhs.GetValue();
    if (lhsValue == NULL)
    {
        if (rhsValue == NULL)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        if (rhsValue == NULL)
        {
            return false;
        }
        else
        {
            return (*lhsValue == *rhsValue);
        }
    }
}

static const char* PropertyPath = "TestPropertyPath";
static const char* PropertyPath_2 = "Test42PropertyPath";

static MICROMOCK_GLOBAL_SEMAPHORE_HANDLE g_dllByDll;

BEGIN_TEST_SUITE(DataPublisher_ut)

        TEST_SUITE_INITIALIZE(TestClassInitialize)
        {
            TEST_INITIALIZE_MEMORY_DEBUG(g_dllByDll);
            g_testByTest = MicroMockCreateMutex();
            time(&currentTime);
            ASSERT_IS_NOT_NULL(g_testByTest);
        }

        TEST_SUITE_CLEANUP(TestClassCleanup)
        {
            MicroMockDestroyMutex(g_testByTest);
            TEST_DEINITIALIZE_MEMORY_DEBUG(g_dllByDll);
        }

        TEST_FUNCTION_INITIALIZE(TestMethodInitialize)
        {
            if (!MicroMockAcquireMutex(g_testByTest))
            {
                ASSERT_FAIL("our mutex is ABANDONED. Failure in test framework");
            }
            data.type = EDM_SINGLE_TYPE;
            data.value.edmSingle.value = 3.5f;
            g_ExpectedDataSentValues = NULL;

            DataPublisher_SetMaxBufferSize(OriginalMaxBufferSize_());
        }

        TEST_FUNCTION_CLEANUP(TestMethodCleanup)
        {
            if (!MicroMockReleaseMutex(g_testByTest))
            {
                ASSERT_FAIL("failure in test framework at ReleaseMutex");
            }
            if (g_myTransaction != NULL)
            {
                unsigned char* destination;
                size_t destinationSize;
                (void)DataPublisher_EndTransaction(g_myTransaction, &destination, &destinationSize);
                g_myTransaction = NULL;
            }
        }

        /* DataPublisher_Create */

        /* Tests_SRS_DATA_PUBLISHER_99_042:[ If a NULL argument is passed to it, DataPublisher_Create shall return NULL.] */
        TEST_FUNCTION(DataPublisher_Create_With_NULL_Model_Handle_Fails)
        {
            // arrange
            CDataPublisherMock dataPublisherMock;

            // act
            DATA_PUBLISHER_HANDLE result = DataPublisher_Create(NULL, true);

            // assert
            ASSERT_IS_NULL(result);
        }

        /* Tests_SRS_DATA_PUBLISHER_99_041:[ DataPublisher_Create shall create a new DataPublisher instance and return a non-NULL handle in case of success.] */
        /* Tests_SRS_DATA_PUBLISHER_99_043:[ DataPublisher_Create shall initialize and hold a handle to a DataMarshaller instance.] */
        /* Tests_SRS_DATA_PUBLISHER_01_001: [DataPublisher_Create shall pass the includePropertyPath argument to DataMarshaller_Create.] */
        TEST_FUNCTION(DataPublisher_Create_With_Valid_Arguments_Yields_A_non_NULL_Handle)
        {
            // arrange
            CDataPublisherMock dataPublisherMock;

            STRICT_EXPECTED_CALL(dataPublisherMock, DataMarshaller_Create(TEST_MODEL_HANDLE, true));

            // act
            DATA_PUBLISHER_HANDLE result = DataPublisher_Create(TEST_MODEL_HANDLE, true);

            // assert
            ASSERT_IS_NOT_NULL(result);
            dataPublisherMock.AssertActualAndExpectedCalls();

            // cleanup
            DataPublisher_Destroy(result);
        }

        /* Tests_SRS_DATA_PUBLISHER_01_001: [DataPublisher_Create shall pass the includePropertyPath argument to DataMarshaller_Create.] */
        TEST_FUNCTION(DataPublisher_Create_Passes_includePropertyPath_To_DataMarshaller_Create)
        {
            // arrange
            CDataPublisherMock dataPublisherMock;

            STRICT_EXPECTED_CALL(dataPublisherMock, DataMarshaller_Create(TEST_MODEL_HANDLE, false));

            // act
            DATA_PUBLISHER_HANDLE result = DataPublisher_Create(TEST_MODEL_HANDLE, false);

            // assert
            ASSERT_IS_NOT_NULL(result);
            dataPublisherMock.AssertActualAndExpectedCalls();

            // cleanup
            DataPublisher_Destroy(result);
        }

        /* Tests_SRS_DATA_PUBLISHER_99_041:[ DataPublisher_Create shall create a new DataPublisher instance and return a non-NULL handle in case of success.] */
        /* Tests_SRS_DATA_PUBLISHER_99_043:[ DataPublisher_Create shall initialize and hold a handle to a DataMarshaller instance.] */
        TEST_FUNCTION(DataPublisher_Create_2_Instances_Yields_Different_Handles)
        {
            // arrange
            CDataPublisherMock dataPublisherMock;

            STRICT_EXPECTED_CALL(dataPublisherMock, DataMarshaller_Create(TEST_MODEL_HANDLE, true));

            STRICT_EXPECTED_CALL(dataPublisherMock, DataMarshaller_Create(TEST_MODEL_HANDLE, true));

            DATA_PUBLISHER_HANDLE handle1 = DataPublisher_Create(TEST_MODEL_HANDLE, true);

            // act
            DATA_PUBLISHER_HANDLE handle2 = DataPublisher_Create(TEST_MODEL_HANDLE, true);

            // assert
            ASSERT_IS_NOT_NULL(handle1);
            ASSERT_IS_NOT_NULL(handle2);
            ASSERT_ARE_NOT_EQUAL(void_ptr, (void_ptr)handle1, (void_ptr)handle2);

            dataPublisherMock.AssertActualAndExpectedCalls();

            // cleanup
            DataPublisher_Destroy(handle1);
            DataPublisher_Destroy(handle2);
        }

        /* Tests_SRS_DATA_PUBLISHER_99_044:[ If the creation of the DataMarshaller instance fails, DataPublisher_Create shall return NULL.] */
        TEST_FUNCTION(DataPublisher_When_DataMarshaller_Create_Fails_DataPublisher_Create_Returns_NULL)
        {
            // arrange
            CDataPublisherMock dataPublisherMock;

            STRICT_EXPECTED_CALL(dataPublisherMock, DataMarshaller_Create(TEST_MODEL_HANDLE, true))
                .SetReturn((DATA_MARSHALLER_HANDLE)NULL);

            // act
            DATA_PUBLISHER_HANDLE handle = DataPublisher_Create(TEST_MODEL_HANDLE, true);

            // assert
            ASSERT_IS_NULL(handle);
        }

        /* DataPublisher_Destroy */

        /* Tests_SRS_DATA_PUBLISHER_99_045:[ DataPublisher_Destroy shall free all resources associated with a DataPublisher instance.] */
        TEST_FUNCTION(DataPublisher_Destroy_Releases_The_Data_Marshaller_Instance)
        {
            // arrange
            CDataPublisherMock dataPublisherMock;
            DATA_PUBLISHER_HANDLE handle = DataPublisher_Create(TEST_IOTHUB_CLIENT_HANDLE, true);


            dataPublisherMock.ResetAllCalls();

            STRICT_EXPECTED_CALL(dataPublisherMock, DataMarshaller_Destroy(TEST_DATA_MARSHALLER_HANDLE));

            // act
            DataPublisher_Destroy(handle);

            // assert
            // no explicit assert, uMock handles the call comparison
        }

        /* Tests_SRS_DATA_PUBLISHER_99_046:[ If a NULL argument is passed to it, DataPublisher_Destroy shall do nothing.] */
        /* Can't really test does nothing ... */
        TEST_FUNCTION(DataPublisher_Destroy_With_A_NULL_Handle_Does_Not_Call_DataMarshaller_Destroy)
        {
            // arrange
            CDataPublisherMock dataPublisherMock;

            // act
            DataPublisher_Destroy(NULL);

            // assert
            // no explicit assert, uMock handles the call comparison
        }

        /* DataPublisher_StartTransaction */

        /* Tests_SRS_DATA_PUBLISHER_99_007:[ A call to DataPublisher_StartBeginTransaction shall start a new transaction.] */
        /* Tests_SRS_DATA_PUBLISHER_99_008:[ DataPublisher_StartBeginTransaction shall return a non-NULL handle upon success.] */
        TEST_FUNCTION(DataPublisher_StartTransaction_Succeeds)
        {
            // arrange
            CDataPublisherMock dataPublisherMock;
            DATA_PUBLISHER_HANDLE handle = DataPublisher_Create(TEST_IOTHUB_CLIENT_HANDLE, true);
            unsigned char* destination;
            size_t destinationSize;
            dataPublisherMock.ResetAllCalls();

            // act
            auto result = DataPublisher_StartTransaction(handle);

            // assert
            ASSERT_IS_NOT_NULL(result);
            dataPublisherMock.AssertActualAndExpectedCalls();

            // cleanup
            (void)DataPublisher_EndTransaction(result, &destination, &destinationSize);
            DataPublisher_Destroy(handle);
        }

        /* Tests_SRS_DATA_PUBLISHER_99_038:[ If DataPublisher_StartTransaction is called with a NULL argument it shall return NULL.] */
        TEST_FUNCTION(DataPublisher_StartTransaction_With_NULL_Handle_Fails)
        {
            // arrange
            CDataPublisherMock dataPublisherMock;

            // act
            auto result = DataPublisher_StartTransaction(NULL);

            // assert
            ASSERT_IS_NULL(result);
        }

        /* DataPublisher_PublishTransacted */

        /* Tests_SRS_DATA_PUBLISHER_99_017:[ When one or more NULL parameter(s) are specified, DataPublisher_PublishTransacted is called with a NULL transactionHandle, it shall return DATA_PUBLISHER_INVALID_ARG.] */
        TEST_FUNCTION(DataPublisher_PublishTransacted_With_NULL_Transaction_Handle_Fails)
        {
            // arrange
            CDataPublisherMock dataPublisherMock;

            // act
            DATA_PUBLISHER_RESULT result = DataPublisher_PublishTransacted(NULL, PropertyPath, &data);

            // assert
            ASSERT_ARE_EQUAL(DATA_PUBLISHER_RESULT, DATA_PUBLISHER_INVALID_ARG, result);
        }

        /* Tests_SRS_DATA_PUBLISHER_99_017:[ When one or more NULL parameter(s) are specified, DataPublisher_PublishTransacted is called with a NULL transactionHandle, it shall return DATA_PUBLISHER_INVALID_ARG.] */
        TEST_FUNCTION(DataPublisher_PublishTransacted_With_NULL_PropertyPath_Fails)
        {
            // arrange
            CDataPublisherMock dataPublisherMock;
            DATA_PUBLISHER_HANDLE handle = DataPublisher_Create(TEST_IOTHUB_CLIENT_HANDLE, true);
            unsigned char* destination;
            size_t destinationSize;
            TRANSACTION_HANDLE transaction = DataPublisher_StartTransaction(handle);
            dataPublisherMock.ResetAllCalls();

            // act
            DATA_PUBLISHER_RESULT result = DataPublisher_PublishTransacted(transaction,  NULL, &data);

            (void)DataPublisher_EndTransaction(transaction, &destination, &destinationSize);

            // assert
            ASSERT_ARE_EQUAL(DATA_PUBLISHER_RESULT, DATA_PUBLISHER_INVALID_ARG, result);
            dataPublisherMock.AssertActualAndExpectedCalls();

            // cleanup
            DataPublisher_Destroy(handle);
        }

        /* Tests_SRS_DATA_PUBLISHER_99_017:[ When one or more NULL parameter(s) are specified, DataPublisher_PublishTransacted is called with a NULL transactionHandle, it shall return DATA_PUBLISHER_INVALID_ARG.] */
        TEST_FUNCTION(DataPublisher_PublishTransacted_With_NULL_Data_Payload_Fails)
        {
            // arrange
            CDataPublisherMock dataPublisherMock;
            DATA_PUBLISHER_HANDLE handle = DataPublisher_Create(TEST_IOTHUB_CLIENT_HANDLE, true);
            TRANSACTION_HANDLE transaction = DataPublisher_StartTransaction(handle);
            dataPublisherMock.ResetAllCalls();

            // act
            DATA_PUBLISHER_RESULT result = DataPublisher_PublishTransacted(transaction, PropertyPath, NULL);

            (void)DataPublisher_CancelTransaction(transaction);

            // assert
            ASSERT_ARE_EQUAL(DATA_PUBLISHER_RESULT, DATA_PUBLISHER_INVALID_ARG, result);
            dataPublisherMock.AssertActualAndExpectedCalls();

            // cleanup
            DataPublisher_Destroy(handle);
        }

        /* Tests_SRS_DATA_PUBLISHER_99_016:[ When DataPublisher_PublishTransacted is invoked, DataPublisher shall associate the data with the transaction identified by the transactionHandle argument and return DATA_PUBLISHER_OK. No data shall be dispatched at the time of the call.] */
        TEST_FUNCTION(DataPublisher_PublishTransacted_With_Valid_Data_Succeeds)
        {
            // arrange
            CDataPublisherMock dataPublisherMock;
            DATA_PUBLISHER_HANDLE handle = DataPublisher_Create(TEST_MODEL_HANDLE, true);
            TRANSACTION_HANDLE transaction = DataPublisher_StartTransaction(handle);
            dataPublisherMock.ResetAllCalls();

            STRICT_EXPECTED_CALL(dataPublisherMock, Schema_ModelPropertyByPathExists(TEST_MODEL_HANDLE, PropertyPath));
            STRICT_EXPECTED_CALL(dataPublisherMock, Create_AGENT_DATA_TYPE_from_AGENT_DATA_TYPE(IGNORED_PTR_ARG, &data))
                .IgnoreArgument(1);
            EXPECTED_CALL(dataPublisherMock, Destroy_AGENT_DATA_TYPE(IGNORED_PTR_ARG));

            // act
            DATA_PUBLISHER_RESULT result = DataPublisher_PublishTransacted(transaction, PropertyPath, &data);

            (void)DataPublisher_CancelTransaction(transaction);

            // assert
            ASSERT_ARE_EQUAL(DATA_PUBLISHER_RESULT, DATA_PUBLISHER_OK, result);
            dataPublisherMock.AssertActualAndExpectedCalls();

            // cleanup
            DataPublisher_Destroy(handle);
        }

        /* Tests_SRS_DATA_PUBLISHER_99_040:[ When PropertyPath does not exist in the supplied model, DataPublisher_Publish shall return DATA_PUBLISHER_SCHEMA_FAILED without dispatching data.] */
        TEST_FUNCTION(DataPublisher_When_GetModelProperty_Returns_NULL_Then_PublishTransacted_Fails)
        {
            // arrange
            CDataPublisherMock dataPublisherMock;
            DATA_PUBLISHER_HANDLE handle = DataPublisher_Create(TEST_MODEL_HANDLE, true);
            TRANSACTION_HANDLE transaction = DataPublisher_StartTransaction(handle);
            dataPublisherMock.ResetAllCalls();

            STRICT_EXPECTED_CALL(dataPublisherMock, Schema_ModelPropertyByPathExists(TEST_MODEL_HANDLE, PropertyPath))
                .SetReturn(false);

            // act
            DATA_PUBLISHER_RESULT result = DataPublisher_PublishTransacted(transaction, PropertyPath, &data);

            (void)DataPublisher_CancelTransaction(transaction);

            // assert
            ASSERT_ARE_EQUAL(DATA_PUBLISHER_RESULT, DATA_PUBLISHER_SCHEMA_FAILED, result);
            dataPublisherMock.AssertActualAndExpectedCalls();

            // cleanup
            DataPublisher_Destroy(handle);
        }

        /* Tests_SRS_DATA_PUBLISHER_99_028:[ If creating the copy fails then DATA_PUBLISHER_AGENT_DATA_TYPES_ERROR shall be returned.] */
        TEST_FUNCTION(DataPublisher_When_Cloning_The_Agent_Data_Type_Fails_Then_PublishTransacted_Fails)
        {
            // arrange
            CDataPublisherMock dataPublisherMock;
            DATA_PUBLISHER_HANDLE handle = DataPublisher_Create(TEST_MODEL_HANDLE, true);
            TRANSACTION_HANDLE transaction = DataPublisher_StartTransaction(handle);
            dataPublisherMock.ResetAllCalls();

            STRICT_EXPECTED_CALL(dataPublisherMock, Schema_ModelPropertyByPathExists(TEST_MODEL_HANDLE, PropertyPath));
            EXPECTED_CALL(dataPublisherMock, Create_AGENT_DATA_TYPE_from_AGENT_DATA_TYPE(IGNORED_PTR_ARG, &data))
                .SetReturn(AGENT_DATA_TYPES_ERROR);

            // act
            DATA_PUBLISHER_RESULT result = DataPublisher_PublishTransacted(transaction, PropertyPath, &data);

            (void)DataPublisher_CancelTransaction(transaction);

            // assert
            ASSERT_ARE_EQUAL(DATA_PUBLISHER_RESULT, DATA_PUBLISHER_AGENT_DATA_TYPES_ERROR, result);
            dataPublisherMock.AssertActualAndExpectedCalls();

            // cleanup
            DataPublisher_Destroy(handle);
        }

        /* Tests_SRS_DATA_PUBLISHER_99_027:[ DataPublisher shall make a copy of the data when associating it with the transaction by using AgentTypeSystem APIs.] */
        TEST_FUNCTION(DataPublisher_When_The_Value_Is_Destroyed_Before_End_Transaction_Cloned_Data_Is_Given_To_Data_Marshaller)
        {
            // arrange
            CDataPublisherMock dataPublisherMock;
            DATA_PUBLISHER_HANDLE handle = DataPublisher_Create(TEST_IOTHUB_CLIENT_HANDLE, true);
            unsigned char* destination;
            size_t destinationSize;
            TRANSACTION_HANDLE transaction = DataPublisher_StartTransaction(handle);
            dataPublisherMock.ResetAllCalls();
            AGENT_DATA_TYPE data2;

            data2.type = EDM_SINGLE_TYPE;
            data2.value.edmSingle.value = 3.5f;

            const DATA_MARSHALLER_VALUE value = { PropertyPath, &data };

            STRICT_EXPECTED_CALL(dataPublisherMock, Schema_ModelPropertyByPathExists(TEST_MODEL_HANDLE, PropertyPath));

            (void)DataPublisher_PublishTransacted(transaction, PropertyPath, &data2);

            (void)memset(&data2, 0, sizeof(data2));
            
            dataPublisherMock.ResetAllCalls();
            STRICT_EXPECTED_CALL(dataPublisherMock, DataMarshaller_SendData(TEST_DATA_MARSHALLER_HANDLE, 1, &value, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreArgument(4)
                .IgnoreArgument(5);
            EXPECTED_CALL(dataPublisherMock, Destroy_AGENT_DATA_TYPE(IGNORED_PTR_ARG));

            // act
            DATA_PUBLISHER_RESULT result = DataPublisher_EndTransaction(transaction, &destination, &destinationSize);

            // assert
            ASSERT_ARE_EQUAL(DATA_PUBLISHER_RESULT, DATA_PUBLISHER_OK, result);
            dataPublisherMock.AssertActualAndExpectedCalls();

            // cleanup
            DataPublisher_Destroy(handle);
        }

        /* DataPublisher_EndTransaction */

        /* Tests_SRS_DATA_PUBLISHER_99_011:[ If the transactionHandle argument is NULL, DataPublisher_EndTransaction shall return DATA_PUBLISHER_INVALID_ARG.] */
        TEST_FUNCTION(DataPublisher_EndTransaction_With_NULL_Handle_Fails)
        {
            // arrange
            CDataPublisherMock dataPublisherMock;
            unsigned char* destination;
            size_t destinationSize;

            // act
            DATA_PUBLISHER_RESULT result = DataPublisher_EndTransaction(NULL, &destination, &destinationSize);

            // assert
            ASSERT_ARE_EQUAL(DATA_PUBLISHER_RESULT, DATA_PUBLISHER_INVALID_ARG, result);
        }

        /*Tests_SRS_DATA_PUBLISHER_02_006: [If the destination argument is NULL, DataPublisher_EndTransaction shall return DATA_PUBLISHER_INVALID_ARG.] */
        TEST_FUNCTION(DataPublisher_EndTransaction_With_NULL_destination_Fails)
        {
            // arrange
            CDataPublisherMock dataPublisherMock;
            DATA_PUBLISHER_HANDLE handle = DataPublisher_Create(TEST_IOTHUB_CLIENT_HANDLE, true);
            size_t destinationSize;
            TRANSACTION_HANDLE transaction = DataPublisher_StartTransaction(handle);
            dataPublisherMock.ResetAllCalls();

            // act
            DATA_PUBLISHER_RESULT result = DataPublisher_EndTransaction(transaction, NULL, &destinationSize);

            // assert
            ASSERT_ARE_EQUAL(DATA_PUBLISHER_RESULT, DATA_PUBLISHER_INVALID_ARG, result);
            dataPublisherMock.AssertActualAndExpectedCalls();

            // cleanup
            unsigned char* destination;
            (void)DataPublisher_EndTransaction(transaction, &destination, &destinationSize);
            DataPublisher_Destroy(handle);
        }

        /*Tests_SRS_DATA_PUBLISHER_02_006: [If the destination argument is NULL, DataPublisher_EndTransaction shall return DATA_PUBLISHER_INVALID_ARG.] */
        TEST_FUNCTION(DataPublisher_EndTransaction_With_NULL_destinationSize_Fails)
        {
            // arrange
            CDataPublisherMock dataPublisherMock;
            DATA_PUBLISHER_HANDLE handle = DataPublisher_Create(TEST_IOTHUB_CLIENT_HANDLE, true);
            unsigned char* destination;
            TRANSACTION_HANDLE transaction = DataPublisher_StartTransaction(handle);
            dataPublisherMock.ResetAllCalls();

            // act
            DATA_PUBLISHER_RESULT result = DataPublisher_EndTransaction(transaction, &destination, NULL);

            // assert
            ASSERT_ARE_EQUAL(DATA_PUBLISHER_RESULT, DATA_PUBLISHER_INVALID_ARG, result);
            dataPublisherMock.AssertActualAndExpectedCalls();

            // cleanup
            size_t destinationSize;
            (void)DataPublisher_EndTransaction(transaction, &destination, &destinationSize);
            DataPublisher_Destroy(handle);
        }

        /* Tests_SRS_DATA_PUBLISHER_99_024:[ If no values have been associated with the transaction, no data shall be dispatched to DataMarshaller, the transaction shall be discarded and DataPublisher_EndTransaction shall return DATA_PUBLISHER_EMPTY_TRANSACTION.] */
        TEST_FUNCTION(DataPublisher_EndTransaction_With_Valid_Handle_An_Empty_Transaction_Fails)
        {
            // arrange
            CDataPublisherMock dataPublisherMock;
            DATA_PUBLISHER_HANDLE handle = DataPublisher_Create(TEST_IOTHUB_CLIENT_HANDLE, true);
            unsigned char* destination;
            size_t destinationSize;
            TRANSACTION_HANDLE transaction = DataPublisher_StartTransaction(handle);
            dataPublisherMock.ResetAllCalls();

            // act
            DATA_PUBLISHER_RESULT result = DataPublisher_EndTransaction(transaction, &destination, &destinationSize);

            // assert
            ASSERT_ARE_EQUAL(DATA_PUBLISHER_RESULT, DATA_PUBLISHER_EMPTY_TRANSACTION, result);
            dataPublisherMock.AssertActualAndExpectedCalls();

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
            CDataPublisherMock dataPublisherMock;
            DATA_PUBLISHER_HANDLE handle = DataPublisher_Create(TEST_IOTHUB_CLIENT_HANDLE, true);
            unsigned char* destination;
            size_t destinationSize;
            TRANSACTION_HANDLE transaction = DataPublisher_StartTransaction(handle);
            dataPublisherMock.ResetAllCalls();

            const DATA_MARSHALLER_VALUE value = { PropertyPath, &data };

            STRICT_EXPECTED_CALL(dataPublisherMock, Schema_ModelPropertyByPathExists(TEST_MODEL_HANDLE, PropertyPath));

            (void)DataPublisher_PublishTransacted(transaction, PropertyPath, &data);

            dataPublisherMock.ResetAllCalls();

            STRICT_EXPECTED_CALL(dataPublisherMock, DataMarshaller_SendData(TEST_DATA_MARSHALLER_HANDLE, 1, &value, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreArgument(4)
                .IgnoreArgument(5);
            EXPECTED_CALL(dataPublisherMock, Destroy_AGENT_DATA_TYPE(IGNORED_PTR_ARG));

            // act
            DATA_PUBLISHER_RESULT result = DataPublisher_EndTransaction(transaction, &destination, &destinationSize);

            // assert
            ASSERT_ARE_EQUAL(DATA_PUBLISHER_RESULT, DATA_PUBLISHER_OK, result);
            dataPublisherMock.AssertActualAndExpectedCalls();

            // cleanup
            DataPublisher_Destroy(handle);
        }

        /* Tests_SRS_DATA_PUBLISHER_99_025:[ When the DataMarshaller_SendData call fails, DataPublisher_EndTransaction shall return DATA_PUBLISHER_MARSHALLER_ERROR.] */
        TEST_FUNCTION(DataPublisher_When_DataMatshaller_SendData_Fails_Then_EndTransaction_Fails)
        {
            // arrange
            CDataPublisherMock dataPublisherMock;
            DATA_PUBLISHER_HANDLE handle = DataPublisher_Create(TEST_IOTHUB_CLIENT_HANDLE, true);
            unsigned char* destination;
            size_t destinationSize;
            TRANSACTION_HANDLE transaction = DataPublisher_StartTransaction(handle);
            dataPublisherMock.ResetAllCalls();

            const DATA_MARSHALLER_VALUE value = { PropertyPath, &data };

            STRICT_EXPECTED_CALL(dataPublisherMock, Schema_ModelPropertyByPathExists(TEST_MODEL_HANDLE, PropertyPath));

            (void)DataPublisher_PublishTransacted(transaction, PropertyPath, &data);

            dataPublisherMock.ResetAllCalls();
            STRICT_EXPECTED_CALL(dataPublisherMock, DataMarshaller_SendData(TEST_DATA_MARSHALLER_HANDLE, 1, &value, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreArgument(4)
                .IgnoreArgument(5)
                .SetReturn(DATA_MARSHALLER_ERROR);
            EXPECTED_CALL(dataPublisherMock, Destroy_AGENT_DATA_TYPE(IGNORED_PTR_ARG));

            // act
            DATA_PUBLISHER_RESULT result = DataPublisher_EndTransaction(transaction, &destination, &destinationSize);

            // assert
            ASSERT_ARE_EQUAL(DATA_PUBLISHER_RESULT, DATA_PUBLISHER_MARSHALLER_ERROR, result);
            dataPublisherMock.AssertActualAndExpectedCalls();

            // cleanup
            DataPublisher_Destroy(handle);
        }

        /* Tests_SRS_DATA_PUBLISHER_99_019:[ If the same property is associated twice with a transaction, then the last value shall be kept associated with the transaction.] */
        TEST_FUNCTION(DataPublisher_Adding_The_Same_Property_To_A_Transaction_Twice_Keeps_The_Last_Value_Only)
        {
            // arrange
            CDataPublisherMock dataPublisherMock;
            DATA_PUBLISHER_HANDLE handle = DataPublisher_Create(TEST_MODEL_HANDLE, true);
            unsigned char* destination;
            size_t destinationSize;
            TRANSACTION_HANDLE transaction = DataPublisher_StartTransaction(handle);
            dataPublisherMock.ResetAllCalls();

            AGENT_DATA_TYPE data2;

            data2.type = EDM_SINGLE_TYPE;
            data2.value.edmSingle.value = 3.7f;

            const DATA_MARSHALLER_VALUE value = { PropertyPath, &data2 };
            g_ExpectedDataSentValues = &value;

            STRICT_EXPECTED_CALL(dataPublisherMock, Schema_ModelPropertyByPathExists(TEST_MODEL_HANDLE, PropertyPath));
            EXPECTED_CALL(dataPublisherMock, Create_AGENT_DATA_TYPE_from_AGENT_DATA_TYPE(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
            EXPECTED_CALL(dataPublisherMock, Destroy_AGENT_DATA_TYPE(IGNORED_PTR_ARG));

            STRICT_EXPECTED_CALL(dataPublisherMock, Schema_ModelPropertyByPathExists(TEST_MODEL_HANDLE, PropertyPath));
            EXPECTED_CALL(dataPublisherMock, Create_AGENT_DATA_TYPE_from_AGENT_DATA_TYPE(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
            EXPECTED_CALL(dataPublisherMock, Destroy_AGENT_DATA_TYPE(IGNORED_PTR_ARG));

            STRICT_EXPECTED_CALL(dataPublisherMock, DataMarshaller_SendData(TEST_DATA_MARSHALLER_HANDLE, 1, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                .IgnoreArgument(4)
                .IgnoreArgument(5);

            (void)DataPublisher_PublishTransacted(transaction, PropertyPath, &data);
            (void)DataPublisher_PublishTransacted(transaction, PropertyPath, &data2);

            // act
            DATA_PUBLISHER_RESULT result = DataPublisher_EndTransaction(transaction, &destination, &destinationSize);

            // assert
            ASSERT_ARE_EQUAL(DATA_PUBLISHER_RESULT, DATA_PUBLISHER_OK, result);
            ASSERT_IS_TRUE(g_DataSentMatches);
            dataPublisherMock.AssertActualAndExpectedCalls();

            // cleanup
            DataPublisher_Destroy(handle);
        }

        /* Tests_SRS_DATA_PUBLISHER_99_016:[ When DataPublisher_PublishTransacted is invoked, DataPublisher shall associate the data with the transaction identified by the transactionHandle argument and return DATA_PUBLISHER_OK. No data shall be dispatched at the time of the call.] */
        /* Tests_SRS_DATA_PUBLISHER_99_010:[ A call to DataPublisher_EndTransaction shall mark the end of a transaction and trigger a dispatch of all the data grouped by that transaction.] */
        /* Tests_SRS_DATA_PUBLISHER_99_026:[ On success, DataPublisher_EndTransaction shall return DATA_PUBLISHER_OK.] */
        TEST_FUNCTION(DataPublisher_Adding_Two_Different_Properties_To_A_Transaction_Dispatches_Both_Values)
        {
            // arrange
            CDataPublisherMock dataPublisherMock;
            DATA_PUBLISHER_HANDLE handle = DataPublisher_Create(TEST_MODEL_HANDLE, true);
            unsigned char* destination;
            size_t destinationSize;
            TRANSACTION_HANDLE transaction = DataPublisher_StartTransaction(handle);
            dataPublisherMock.ResetAllCalls();

            AGENT_DATA_TYPE data2;

            data2.type = EDM_SINGLE_TYPE;
            data2.value.edmSingle.value = 3.7f;

            const DATA_MARSHALLER_VALUE values[] = {
                { PropertyPath, &data },
                { PropertyPath_2, &data2 },
            };

            g_ExpectedDataSentValues = values;

            STRICT_EXPECTED_CALL(dataPublisherMock, Schema_ModelPropertyByPathExists(TEST_MODEL_HANDLE, PropertyPath));
            EXPECTED_CALL(dataPublisherMock, Create_AGENT_DATA_TYPE_from_AGENT_DATA_TYPE(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
            EXPECTED_CALL(dataPublisherMock, Destroy_AGENT_DATA_TYPE(IGNORED_PTR_ARG));

            STRICT_EXPECTED_CALL(dataPublisherMock, Schema_ModelPropertyByPathExists(TEST_MODEL_HANDLE, PropertyPath_2));
            EXPECTED_CALL(dataPublisherMock, Create_AGENT_DATA_TYPE_from_AGENT_DATA_TYPE(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
            EXPECTED_CALL(dataPublisherMock, Destroy_AGENT_DATA_TYPE(IGNORED_PTR_ARG));

            STRICT_EXPECTED_CALL(dataPublisherMock, DataMarshaller_SendData(TEST_DATA_MARSHALLER_HANDLE, 2, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreArgument(3)
                .IgnoreArgument(4)
                .IgnoreArgument(5);

            (void)DataPublisher_PublishTransacted(transaction, PropertyPath, &data);
            (void)DataPublisher_PublishTransacted(transaction, PropertyPath_2, &data2);

            // act
            DATA_PUBLISHER_RESULT result = DataPublisher_EndTransaction(transaction, &destination, &destinationSize);

            // assert
            ASSERT_ARE_EQUAL(DATA_PUBLISHER_RESULT, DATA_PUBLISHER_OK, result);
            ASSERT_IS_TRUE(g_DataSentMatches);
            dataPublisherMock.AssertActualAndExpectedCalls();

            // cleanup
            DataPublisher_Destroy(handle);
        }

        /* Tests_SRS_DATA_PUBLISHER_99_040:[ When PropertyPath does not exist in the supplied model, DataPublisher_Publish shall return DATA_PUBLISHER_SCHEMA_FAILED without dispatching data.] */
        TEST_FUNCTION(DataPublisher_When_Getting_The_ModelProperty_For_The_Second_Property_Fails_Only_One_Property_Remains_Associated_With_The_Transaction)
        {
            // arrange
            CDataPublisherMock dataPublisherMock;
            DATA_PUBLISHER_HANDLE handle = DataPublisher_Create(TEST_MODEL_HANDLE, true);
            unsigned char* destination;
            size_t destinationSize;
            TRANSACTION_HANDLE transaction = DataPublisher_StartTransaction(handle);
            dataPublisherMock.ResetAllCalls();

            AGENT_DATA_TYPE data2;

            data2.type = EDM_SINGLE_TYPE;
            data2.value.edmSingle.value = 3.7f;

            const DATA_MARSHALLER_VALUE value = { PropertyPath, &data };

            STRICT_EXPECTED_CALL(dataPublisherMock, Schema_ModelPropertyByPathExists(TEST_MODEL_HANDLE, PropertyPath));
            STRICT_EXPECTED_CALL(dataPublisherMock, Create_AGENT_DATA_TYPE_from_AGENT_DATA_TYPE(IGNORED_PTR_ARG, &data))
                .IgnoreArgument(1);
            STRICT_EXPECTED_CALL(dataPublisherMock, Schema_ModelPropertyByPathExists(TEST_MODEL_HANDLE, PropertyPath_2))
                .SetReturn(false);
            STRICT_EXPECTED_CALL(dataPublisherMock, DataMarshaller_SendData(TEST_DATA_MARSHALLER_HANDLE, 1, &value, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
                .IgnoreArgument(4)
                .IgnoreArgument(5);
            EXPECTED_CALL(dataPublisherMock, Destroy_AGENT_DATA_TYPE(IGNORED_PTR_ARG));

            (void)DataPublisher_PublishTransacted(transaction, PropertyPath, &data);
            (void)DataPublisher_PublishTransacted(transaction, PropertyPath_2, &data2);

            // act
            DATA_PUBLISHER_RESULT result = DataPublisher_EndTransaction(transaction, &destination, &destinationSize);

            // assert
            ASSERT_ARE_EQUAL(DATA_PUBLISHER_RESULT, DATA_PUBLISHER_OK, result);
            dataPublisherMock.AssertActualAndExpectedCalls();

            // cleanup
            DataPublisher_Destroy(handle);
        }

        /* Tests_SRS_DATA_PUBLISHER_99_012:[ DataPublisher_EndTransaction shall dispose of any resources associated with the transaction.] */
        TEST_FUNCTION(DataPublisher_EndTransaction_Destroys_The_Cloned_Agent_Data_Type)
        {
            // arrange
            CDataPublisherMock dataPublisherMock;
            DATA_PUBLISHER_HANDLE handle = DataPublisher_Create(TEST_IOTHUB_CLIENT_HANDLE, true);
            unsigned char* destination;
            size_t destinationSize;
            TRANSACTION_HANDLE transaction = DataPublisher_StartTransaction(handle);
            dataPublisherMock.ResetAllCalls();

            STRICT_EXPECTED_CALL(dataPublisherMock, Schema_ModelPropertyByPathExists(TEST_MODEL_HANDLE, PropertyPath));

            DATA_PUBLISHER_RESULT result = DataPublisher_PublishTransacted(transaction, PropertyPath, &data);

            dataPublisherMock.ResetAllCalls();
            EXPECTED_CALL(dataPublisherMock, DataMarshaller_SendData(TEST_DATA_MARSHALLER_HANDLE, 1, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));

            EXPECTED_CALL(dataPublisherMock, Destroy_AGENT_DATA_TYPE(IGNORED_PTR_ARG));

            // act
            (void)DataPublisher_EndTransaction(transaction, &destination, &destinationSize);

            // assert
            ASSERT_ARE_EQUAL(DATA_PUBLISHER_RESULT, DATA_PUBLISHER_OK, result);
            dataPublisherMock.AssertActualAndExpectedCalls();

            // cleanup
            DataPublisher_Destroy(handle);
        }

        /* DataPublisher_CancelTransaction */

        /* Tests_SRS_DATA_PUBLISHER_99_013:[ A call to DataPublisher_CancelTransaction shall dispose of the transaction without dispatching the data to the DataMarshaller module and it shall return DATA_PUBLISHER_OK.] */
        TEST_FUNCTION(DataPublisher_CancelTransaction_Does_Not_Dispatch_Data)
        {
            // arrange
            CDataPublisherMock dataPublisherMock;
            DATA_PUBLISHER_HANDLE handle = DataPublisher_Create(TEST_IOTHUB_CLIENT_HANDLE, true);
            TRANSACTION_HANDLE transaction = DataPublisher_StartTransaction(handle);
            dataPublisherMock.ResetAllCalls();

            STRICT_EXPECTED_CALL(dataPublisherMock, Schema_ModelPropertyByPathExists(TEST_MODEL_HANDLE, PropertyPath));

            (void)DataPublisher_PublishTransacted(transaction, PropertyPath, &data);

            dataPublisherMock.ResetAllCalls();
            EXPECTED_CALL(dataPublisherMock, Destroy_AGENT_DATA_TYPE(IGNORED_PTR_ARG));

            // act
            DATA_PUBLISHER_RESULT result = DataPublisher_CancelTransaction(transaction);

            // assert
            ASSERT_ARE_EQUAL(DATA_PUBLISHER_RESULT, DATA_PUBLISHER_OK, result);
            dataPublisherMock.AssertActualAndExpectedCalls();

            // cleanup
            DataPublisher_Destroy(handle);
        }

        /* Tests_SRS_DATA_PUBLISHER_99_014:[ If the transactionHandle argument is NULL DataPublisher_CancelTransaction shall return DATA_PUBLISHER_INVALID_ARG.] */
        TEST_FUNCTION(DataPublisher_CancelTransaction_With_A_NULL_Transaction_Fails)
        {
            // arrange
            CDataPublisherMock dataPublisherMock;

            // act
            DATA_PUBLISHER_RESULT result = DataPublisher_CancelTransaction(NULL);

            // assert
            ASSERT_ARE_EQUAL(DATA_PUBLISHER_RESULT, DATA_PUBLISHER_INVALID_ARG, result);
        }

        /* Tests_SRS_DATA_PUBLISHER_99_015:[ DataPublisher_CancelTransaction shall dispose of any resources associated with the transaction.] */
        TEST_FUNCTION(DataPublisher_CancelTransaction_Destroys_The_Cloned_Agent_Data_Type)
        {
            // arrange
            CDataPublisherMock dataPublisherMock;
            DATA_PUBLISHER_HANDLE handle = DataPublisher_Create(TEST_IOTHUB_CLIENT_HANDLE, true);
            TRANSACTION_HANDLE transaction = DataPublisher_StartTransaction(handle);
            dataPublisherMock.ResetAllCalls();

            STRICT_EXPECTED_CALL(dataPublisherMock, Schema_ModelPropertyByPathExists(TEST_MODEL_HANDLE, PropertyPath));

            // act
            DATA_PUBLISHER_RESULT result = DataPublisher_PublishTransacted(transaction, PropertyPath, &data);

            dataPublisherMock.ResetAllCalls();
            EXPECTED_CALL(dataPublisherMock, Destroy_AGENT_DATA_TYPE(IGNORED_PTR_ARG));

            (void)DataPublisher_CancelTransaction(transaction);

            // assert
            ASSERT_ARE_EQUAL(DATA_PUBLISHER_RESULT, DATA_PUBLISHER_OK, result);
            dataPublisherMock.AssertActualAndExpectedCalls();

            // cleanup
            DataPublisher_Destroy(handle);
        }

        /* Tests_SRS_DATA_PUBLISHER_99_067:[ Before any call to DataPublisher_SetMaxBufferSize, the default max buffer size shall be equal to 10KB.] */
        TEST_FUNCTION(DataPublisher_default_max_buffer_size_should_be_10KB)
        {
            // arrange
            // act
            // assert
            ASSERT_ARE_EQUAL(size_t, 10 * 1024, OriginalMaxBufferSize_());
        }

        /* Tests_SRS_DATA_PUBLISHER_99_065:[ DataPublisher_SetMaxBufferSize shall directly update the value used to limit how much data (in bytes) can be buffered in the BufferStorage instance.] */
        /* Tests_SRS_DATA_PUBLISHER_99_069:[ DataMarshaller_GetMaxBufferSize shall return the current max buffer size value used by any new instance of DataMarshaller.] */
        TEST_FUNCTION(DataPublisher_SetMaxBufferSize_should_update_max_buffer_size_value)
        {
            // arrange
            // act
            DataPublisher_SetMaxBufferSize(42);

            // assert
            ASSERT_ARE_EQUAL(size_t, 42, DataPublisher_GetMaxBufferSize());
        }

END_TEST_SUITE(DataPublisher_ut)
