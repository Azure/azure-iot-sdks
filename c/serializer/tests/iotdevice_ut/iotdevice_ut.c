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

void my_gballoc_free(void * t)
{
    free(t);
}

#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#ifdef __cplusplus
#include <cstddef>
#include <cstdbool>
#else
#include <stdlib.h>
#include <stdbool.h>
#endif

#include "umock_c.h"
#include "umocktypes_charptr.h"
#include "umocktypes_bool.h"
#include "umocktypes_stdint.h"
#include "umock_c_negative_tests.h"

#include "agenttypesystem.h"

#define ENABLE_MOCKS
#include "schema.h"
#include "datapublisher.h"
#include "commanddecoder.h"
#undef ENABLE_MOCKS

#include "iotdevice.h"

#include "testrunnerswitcher.h"

TEST_DEFINE_ENUM_TYPE(DEVICE_RESULT, DEVICE_RESULT_VALUES);
IMPLEMENT_UMOCK_C_ENUM_TYPE(DEVICE_RESULT, DEVICE_RESULT_VALUES);

TEST_DEFINE_ENUM_TYPE(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_RESULT_VALUES);
IMPLEMENT_UMOCK_C_ENUM_TYPE(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_RESULT_VALUES);

TEST_DEFINE_ENUM_TYPE(DATA_PUBLISHER_RESULT, DATA_PUBLISHER_RESULT_VALUES);
IMPLEMENT_UMOCK_C_ENUM_TYPE(DATA_PUBLISHER_RESULT, DATA_PUBLISHER_RESULT_VALUES);

#define ENABLE_MOCKS
#include "azure_c_shared_utility/umock_c_prod.h"
MOCKABLE_FUNCTION(, EXECUTE_COMMAND_RESULT, DeviceActionCallback, DEVICE_HANDLE, deviceHandle, void*, callbackUserContext, const char*, relativeActionPath, const char*, actionName, size_t, argCount, const AGENT_DATA_TYPE*, arguments);
#undef ENABLE_MOCKS

DEFINE_ENUM_STRINGS(UMOCK_C_ERROR_CODE, UMOCK_C_ERROR_CODE_VALUES)

#define TEST_CALLBACK_CONTEXT           (void*)0x4246

static SCHEMA_MODEL_TYPE_HANDLE irrelevantModel = (SCHEMA_MODEL_TYPE_HANDLE)0x1;
static ACTION_CALLBACK_FUNC ActionCallbackCalledByCommandDecoder;

static TEST_MUTEX_HANDLE g_testByTest;
static TEST_MUTEX_HANDLE g_dllByDll;

static void on_umock_c_error(UMOCK_C_ERROR_CODE error_code)
{
    char temp_str[256];
    (void)snprintf(temp_str, sizeof(temp_str), "umock_c reported error :%s", ENUM_TO_STRING(UMOCK_C_ERROR_CODE, error_code));
    ASSERT_FAIL(temp_str);
}

static DATA_PUBLISHER_HANDLE my_DataPublisher_Create(SCHEMA_MODEL_TYPE_HANDLE modelHandle, bool includePropertyPath)
{
    (void)(modelHandle, includePropertyPath);
    return (DATA_PUBLISHER_HANDLE )my_gballoc_malloc(1);
}

static void my_DataPublisher_Destroy(DATA_PUBLISHER_HANDLE dataPublisherHandle)
{
    my_gballoc_free(dataPublisherHandle);
}

static COMMAND_DECODER_HANDLE my_CommandDecoder_Create(SCHEMA_MODEL_TYPE_HANDLE modelHandle, ACTION_CALLBACK_FUNC actionCallback, void* actionCallbackContext)
{
    (void)(modelHandle, actionCallbackContext);
    ActionCallbackCalledByCommandDecoder = actionCallback;
    return (COMMAND_DECODER_HANDLE)my_gballoc_malloc(1);
}

static void my_CommandDecoder_Destroy(COMMAND_DECODER_HANDLE commandDecoderHandle)
{
    my_gballoc_free(commandDecoderHandle);
}

static TRANSACTION_HANDLE my_DataPublisher_StartTransaction(DATA_PUBLISHER_HANDLE dataPublisherHandle)
{
    (void)(dataPublisherHandle);
    return (TRANSACTION_HANDLE)my_gballoc_malloc(2);
}

static DATA_PUBLISHER_RESULT my_DataPublisher_EndTransaction(TRANSACTION_HANDLE transactionHandle, unsigned char** destination, size_t* destinationSize)
{
    (void)(destination, destinationSize);
    my_gballoc_free(transactionHandle);
    return DATA_PUBLISHER_OK;
}

static DATA_PUBLISHER_RESULT my_DataPublisher_CancelTransaction(TRANSACTION_HANDLE transactionHandle)
{
    my_gballoc_free(transactionHandle);
    return DATA_PUBLISHER_OK;
}

static REPORTED_PROPERTIES_TRANSACTION_HANDLE my_DataPublisher_CreateTransaction_ReportedProperties(DATA_PUBLISHER_HANDLE dataPublisherHandle)
{
    (void)(dataPublisherHandle);
    return (REPORTED_PROPERTIES_TRANSACTION_HANDLE)my_gballoc_malloc(3);
}

void my_DataPublisher_DestroyTransaction_ReportedProperties(REPORTED_PROPERTIES_TRANSACTION_HANDLE transactionHandle)
{
    my_gballoc_free(transactionHandle);
}

static void* FAKE_DEVICE_START_ADDRESS = (void*)(0x42);

BEGIN_TEST_SUITE(IoTDevice_ut)

    TEST_SUITE_INITIALIZE(BeforeSuite)
    {
        TEST_INITIALIZE_MEMORY_DEBUG(g_dllByDll);

        g_testByTest = TEST_MUTEX_CREATE();
        ASSERT_IS_NOT_NULL(g_testByTest);

        (void)umock_c_init(on_umock_c_error);
        (void)umocktypes_bool_register_types();
        (void)umocktypes_charptr_register_types();
        (void)umocktypes_stdint_register_types();

        REGISTER_UMOCK_ALIAS_TYPE(SCHEMA_MODEL_TYPE_HANDLE, void*);
        REGISTER_UMOCK_ALIAS_TYPE(DATA_PUBLISHER_HANDLE, void*);
        REGISTER_UMOCK_ALIAS_TYPE(ACTION_CALLBACK_FUNC, void*);
        REGISTER_UMOCK_ALIAS_TYPE(COMMAND_DECODER_HANDLE, void*);
        REGISTER_UMOCK_ALIAS_TYPE(TRANSACTION_HANDLE, void*);
        REGISTER_UMOCK_ALIAS_TYPE(DEVICE_HANDLE, void*);
        REGISTER_UMOCK_ALIAS_TYPE(REPORTED_PROPERTIES_TRANSACTION_HANDLE, void*);
        
        REGISTER_UMOCK_ALIAS_TYPE(EXECUTE_COMMAND_RESULT, int);
        REGISTER_UMOCK_ALIAS_TYPE(DATA_PUBLISHER_RESULT, int);
        
        REGISTER_GLOBAL_MOCK_RETURN(DeviceActionCallback, EXECUTE_COMMAND_SUCCESS);

        REGISTER_GLOBAL_MOCK_HOOK(DataPublisher_Create, my_DataPublisher_Create);
        REGISTER_GLOBAL_MOCK_FAIL_RETURN(DataPublisher_Create, NULL);
        REGISTER_GLOBAL_MOCK_HOOK(DataPublisher_Destroy, my_DataPublisher_Destroy);


        
        REGISTER_GLOBAL_MOCK_HOOK(CommandDecoder_Create, my_CommandDecoder_Create);
        REGISTER_GLOBAL_MOCK_FAIL_RETURN(CommandDecoder_Create, NULL);
        REGISTER_GLOBAL_MOCK_RETURNS(CommandDecoder_ExecuteCommand, EXECUTE_COMMAND_SUCCESS, EXECUTE_COMMAND_ERROR);
        REGISTER_GLOBAL_MOCK_RETURNS(CommandDecoder_IngestDesiredProperties, EXECUTE_COMMAND_SUCCESS, EXECUTE_COMMAND_ERROR);
        REGISTER_GLOBAL_MOCK_HOOK(CommandDecoder_Destroy, my_CommandDecoder_Destroy);
        
        REGISTER_GLOBAL_MOCK_HOOK(DataPublisher_StartTransaction, my_DataPublisher_StartTransaction);
        REGISTER_GLOBAL_MOCK_FAIL_RETURN(DataPublisher_StartTransaction, NULL);
        REGISTER_GLOBAL_MOCK_HOOK(DataPublisher_EndTransaction, my_DataPublisher_EndTransaction);
        REGISTER_GLOBAL_MOCK_FAIL_RETURN(DataPublisher_EndTransaction, DATA_PUBLISHER_ERROR);
        REGISTER_GLOBAL_MOCK_HOOK(DataPublisher_CancelTransaction, my_DataPublisher_CancelTransaction);
        REGISTER_GLOBAL_MOCK_FAIL_RETURN(DataPublisher_CancelTransaction, DATA_PUBLISHER_ERROR);

        REGISTER_GLOBAL_MOCK_HOOK(DataPublisher_CreateTransaction_ReportedProperties, my_DataPublisher_CreateTransaction_ReportedProperties);
        REGISTER_GLOBAL_MOCK_HOOK(DataPublisher_DestroyTransaction_ReportedProperties, my_DataPublisher_DestroyTransaction_ReportedProperties);

    }

    TEST_SUITE_CLEANUP(AfterEachSuite)
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
    }

    TEST_FUNCTION_CLEANUP(TestMethodCleanup)
    {

        TEST_MUTEX_RELEASE(g_testByTest);
    }

    /* Tests_SRS_DEVICE_05_014: [If any of the modelHandle, deviceHandle or deviceActionCallback arguments are NULL, Device_Create shall return DEVICE_INVALID_ARG.]*/
    TEST_FUNCTION(Device_Create_with_NULL_model_handle_fails)
    {
        // arrange
        DEVICE_HANDLE h;

        // act
        DEVICE_RESULT res = Device_Create(NULL, DeviceActionCallback, TEST_CALLBACK_CONTEXT, false, &h);

        // assert
        ASSERT_ARE_EQUAL(DEVICE_RESULT, DEVICE_INVALID_ARG, res);
    }

    /* Tests_SRS_DEVICE_05_014: [If any of the iotHubClientHandle, modelHandle, deviceHandle or deviceActionCallback arguments are NULL, Device_Create shall return DEVICE_INVALID_ARG.] */
    TEST_FUNCTION(Device_Create_with_NULL_Action_Callback_fails)
    {
        // arrange
        DEVICE_HANDLE h;

        // act
        DEVICE_RESULT res = Device_Create(irrelevantModel, NULL, TEST_CALLBACK_CONTEXT, false, &h);

        // assert
        ASSERT_ARE_EQUAL(DEVICE_RESULT, DEVICE_INVALID_ARG, res);
    }

    /* Tests_SRS_DEVICE_05_014: [If any of the modelHandle, deviceHandle or deviceActionCallback arguments are NULL, Device_Create shall return DEVICE_INVALID_ARG.]*/
    TEST_FUNCTION(Device_Create_with_NULL_outparam_fails)
    {
        // arrange

        // act
        DEVICE_RESULT res = Device_Create(irrelevantModel, DeviceActionCallback, TEST_CALLBACK_CONTEXT, false, NULL);

        // assert
        ASSERT_ARE_EQUAL(DEVICE_RESULT, DEVICE_INVALID_ARG, res);
    }

    /* Tests_SRS_DEVICE_03_003: [The DEVICE_HANDLE shall be provided via the deviceHandle out argument.] */
    /* Tests_SRS_DEVICE_03_004: [Device_Create shall return DEVICE_OK upon success.] */
    /* Tests_SRS_DEVICE_01_018: [Device_Create shall create a DataPublisher instance by calling DataPublisher_Create.] */
    /* Tests_SRS_DEVICE_01_020: [Device_Create shall pass to DataPublisher_Create the FrontDoor instance obtained earlier.] */
    /* Tests_SRS_DEVICE_01_001: [Device_Create shall create a CommandDecoder instance by calling CommandDecoder_Create and passing to it the model handle.] */
    /* Tests_SRS_DEVICE_01_002: [Device_Create shall also pass to CommandDecoder_Create a callback to be invoked when a command is received and a context that shall be the device handle.]  */
    /* Tests_SRS_DEVICE_01_004: [DeviceCreate shall pass to DataPublisher_create the includePropertyPath argument.] */
    TEST_FUNCTION(Device_Create_can_return_a_device_handle)
    {
        // arrange
        DEVICE_HANDLE h = NULL;

        STRICT_EXPECTED_CALL(DataPublisher_Create(irrelevantModel, true));
        STRICT_EXPECTED_CALL(CommandDecoder_Create(irrelevantModel, NULL, NULL))
            .IgnoreArgument(2).IgnoreArgument(3);

        // act
        DEVICE_RESULT res = Device_Create(irrelevantModel, DeviceActionCallback, TEST_CALLBACK_CONTEXT, true, &h);

        // assert
        ASSERT_ARE_EQUAL(DEVICE_RESULT, DEVICE_OK, res);
        ASSERT_IS_NOT_NULL(h);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        // cleanup
        Device_Destroy(h);
    }


    /* Tests_SRS_DEVICE_01_004: [DeviceCreate shall pass to DataPublisher_create the includePropertyPath argument.] */
    TEST_FUNCTION(Device_Create_passes_includePropertyPath_false_to_DataPublisher_Create)
    {
        // arrange
        DEVICE_HANDLE h = NULL;

        STRICT_EXPECTED_CALL(DataPublisher_Create(irrelevantModel, false));
        STRICT_EXPECTED_CALL(CommandDecoder_Create(irrelevantModel, NULL, NULL))
            .IgnoreArgument(2).IgnoreArgument(3);

        // act
        DEVICE_RESULT res = Device_Create(irrelevantModel, DeviceActionCallback, TEST_CALLBACK_CONTEXT, false, &h);

        // assert
        ASSERT_ARE_EQUAL(DEVICE_RESULT, DEVICE_OK, res);
        ASSERT_IS_NOT_NULL(h);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        // cleanup
        Device_Destroy(h);
    }

    /* Tests_SRS_DEVICE_01_019: [If creating the DataPublisher instance fails, Device_Create shall return DEVICE_DATA_PUBLISHER_FAILED.] */
    TEST_FUNCTION(When_DataPublisher_Create_Fails_Then_Device_Create_Fails)
    {
        // arrange
        DEVICE_HANDLE h = NULL;

        STRICT_EXPECTED_CALL(DataPublisher_Create(irrelevantModel, true))
            .SetReturn((DATA_PUBLISHER_HANDLE)NULL);

        // act
        DEVICE_RESULT res = Device_Create(irrelevantModel, DeviceActionCallback, TEST_CALLBACK_CONTEXT, true, &h);

        // assert
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
        ASSERT_ARE_EQUAL(DEVICE_RESULT, DEVICE_DATA_PUBLISHER_FAILED, res);
        ASSERT_IS_NULL(h);
    }

    /* Tests_SRS_DEVICE_01_003: [If CommandDecoder_Create fails, Device_Create shall return DEVICE_COMMAND_DECODER_FAILED.] */
    TEST_FUNCTION(When_CommandDecoder_Create_Fails_Device_Create_Fails)
    {
        // arrange
        DEVICE_HANDLE h = NULL;

        STRICT_EXPECTED_CALL(DataPublisher_Create(irrelevantModel, true));
        STRICT_EXPECTED_CALL(CommandDecoder_Create(irrelevantModel, NULL, NULL))
            .IgnoreArgument(2).IgnoreArgument(3).SetReturn((COMMAND_DECODER_HANDLE)NULL);

        STRICT_EXPECTED_CALL(DataPublisher_Destroy(IGNORED_PTR_ARG))
            .IgnoreArgument_dataPublisherHandle();

        // act
        DEVICE_RESULT res = Device_Create(irrelevantModel, DeviceActionCallback, TEST_CALLBACK_CONTEXT, true, &h);

        // assert
        ASSERT_ARE_EQUAL(DEVICE_RESULT, DEVICE_COMMAND_DECODER_FAILED, res);
        ASSERT_IS_NULL(h);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    }

    /* Device_Destroy */

    /* Tests_SRS_DEVICE_03_007: [Device_Destroy will not do anything if deviceHandle is NULL.] */
    TEST_FUNCTION(Device_Destroy_with_a_NULL_handle_raises_no_exceptions)
    {
        // arrange

        // act
        Device_Destroy(NULL);

        // assert
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    }


    /* Tests_SRS_DEVICE_03_006: [Device_Destroy shall free all resources associated with a device.] */
    TEST_FUNCTION(Device_Destroy_with_a_Valid_handle_frees_all_underlying_modules)
    {
        // arrange
        DEVICE_HANDLE h = NULL;
        (void)Device_Create(irrelevantModel, DeviceActionCallback, TEST_CALLBACK_CONTEXT, false, &h);
        umock_c_reset_all_calls();

        STRICT_EXPECTED_CALL(DataPublisher_Destroy(IGNORED_PTR_ARG))
            .IgnoreArgument_dataPublisherHandle();
        STRICT_EXPECTED_CALL(CommandDecoder_Destroy(IGNORED_PTR_ARG))
            .IgnoreArgument_commandDecoderHandle();

        // act
        Device_Destroy(h);

        // assert
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    }

    /* Device_StartTransaction */

    /* Tests_SRS_DEVICE_01_034: [Device_StartTransaction shall invoke DataPublisher_StartTransaction for the DataPublisher handle associated with the deviceHandle argument.] */
    /* Tests_SRS_DEVICE_01_043: [On success, Device_StartTransaction shall return a non NULL handle.] */
    TEST_FUNCTION(Device_StartTransaction_Calls_DataPublisher_And_Succeeds)
    {
        // arrange
        DEVICE_HANDLE deviceHandle;
        (void)Device_Create(irrelevantModel, DeviceActionCallback, TEST_CALLBACK_CONTEXT, false, &deviceHandle);
        umock_c_reset_all_calls();

        STRICT_EXPECTED_CALL(DataPublisher_StartTransaction(IGNORED_PTR_ARG))
            .IgnoreArgument_dataPublisherHandle();

        // act
        TRANSACTION_HANDLE result = Device_StartTransaction(deviceHandle);

        // assert
        ASSERT_IS_NOT_NULL(result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///clean
        Device_CancelTransaction(result);
        Device_Destroy(deviceHandle);
    }

    /* Tests_SRS_DEVICE_01_035: [If any argument is NULL, Device_StartTransaction shall return NULL.] */
    TEST_FUNCTION(Device_StartTransaction_Called_With_NULL_Handle_Fails)
    {
        // arrange

        // act
        TRANSACTION_HANDLE result = Device_StartTransaction(NULL);

        // assert
        ASSERT_IS_NULL(result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    }


    /* Tests_SRS_DEVICE_01_048: [When DataPublisher_StartTransaction fails, Device_StartTransaction shall return NULL.] */
    TEST_FUNCTION(When_DataPublisher_StartTransaction_Fails_Then_Device_StartTransaction_Fails)
    {
        // arrange
        DEVICE_HANDLE deviceHandle;
        (void)Device_Create(irrelevantModel, DeviceActionCallback, TEST_CALLBACK_CONTEXT, false, &deviceHandle);
        umock_c_reset_all_calls();

        STRICT_EXPECTED_CALL(DataPublisher_StartTransaction(IGNORED_PTR_ARG))
            .IgnoreArgument_dataPublisherHandle()
            .SetReturn((TRANSACTION_HANDLE)NULL);

        // act
        TRANSACTION_HANDLE result = Device_StartTransaction(deviceHandle);

        // assert
        ASSERT_IS_NULL(result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///clean
        Device_Destroy(deviceHandle);
    }

    /* Device_PublishTransacted */

    /* Tests_SRS_DEVICE_01_036: [Device_PublishTransacted shall invoke DataPublisher_PublishTransacted.] */
    /* Tests_SRS_DEVICE_01_044: [On success, Device_PublishTransacted shall return DEVICE_OK.] */
    TEST_FUNCTION(Device_PublishTransacted_Calls_DataPublisher_And_Succeeds)
    {
        // arrange
        AGENT_DATA_TYPE ag;
        DEVICE_HANDLE deviceHandle;
        (void)Device_Create(irrelevantModel, DeviceActionCallback, TEST_CALLBACK_CONTEXT, false, &deviceHandle);
        TRANSACTION_HANDLE transaction = Device_StartTransaction(deviceHandle);
        umock_c_reset_all_calls();

        STRICT_EXPECTED_CALL(DataPublisher_PublishTransacted(transaction, "p", &ag));

        // act
        DEVICE_RESULT result = Device_PublishTransacted(transaction, "p", &ag);

        // assert
        ASSERT_ARE_EQUAL(DEVICE_RESULT, DEVICE_OK, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///clean
        Device_CancelTransaction(transaction);
        Device_Destroy(deviceHandle);
    }

    /* Tests_SRS_DEVICE_01_037: [If any argument is NULL, Device_PublishTransacted shall return DEVICE_INVALID_ARG.] */
    TEST_FUNCTION(Device_PublishTransacted_Called_With_NULL_Handle_Fails)
    {
        // arrange
        AGENT_DATA_TYPE ag;

        // act
        DEVICE_RESULT result = Device_PublishTransacted(NULL, "p", &ag);

        // assert
        ASSERT_ARE_EQUAL(DEVICE_RESULT, DEVICE_INVALID_ARG, result);
    }


    /* Tests_SRS_DEVICE_01_037: [If any argument is NULL, Device_PublishTransacted shall return DEVICE_INVALID_ARG.] */
    TEST_FUNCTION(Device_PublishTransacted_Called_With_NULL_Property_Fails)
    {
        // arrange
        AGENT_DATA_TYPE ag;
        DEVICE_HANDLE deviceHandle;
        (void)Device_Create(irrelevantModel, DeviceActionCallback, TEST_CALLBACK_CONTEXT, false, &deviceHandle);
        TRANSACTION_HANDLE transaction = Device_StartTransaction(deviceHandle);
        umock_c_reset_all_calls();

        // act
        DEVICE_RESULT result = Device_PublishTransacted(transaction, NULL, &ag);

        // assert
        ASSERT_ARE_EQUAL(DEVICE_RESULT, DEVICE_INVALID_ARG, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///clean
        Device_CancelTransaction(transaction);
        Device_Destroy(deviceHandle);
    }

    /* Tests_SRS_DEVICE_01_037: [If any argument is NULL, Device_PublishTransacted shall return DEVICE_INVALID_ARG.] */
    TEST_FUNCTION(Device_PublishTransacted_Called_With_NULL_Value_Fails)
    {
        // arrange
        DEVICE_HANDLE deviceHandle;
        (void)Device_Create(irrelevantModel, DeviceActionCallback, TEST_CALLBACK_CONTEXT, false, &deviceHandle);
        TRANSACTION_HANDLE transaction = Device_StartTransaction(deviceHandle);
        umock_c_reset_all_calls();

        // act
        DEVICE_RESULT result = Device_PublishTransacted(transaction, "p", NULL);

        // assert
        ASSERT_ARE_EQUAL(DEVICE_RESULT, DEVICE_INVALID_ARG, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///clean
        Device_CancelTransaction(transaction);
        Device_Destroy(deviceHandle);
    }

    /* Tests_SRS_DEVICE_01_049: [When DataPublisher_PublishTransacted fails, Device_PublishTransacted shall return DEVICE_DATA_PUBLISHER_FAILED.] */
    TEST_FUNCTION(When_DataPublisher_PublishTransacted_Fails_Then_Device_PublishTransacted_Fails)
    {
        // arrange
        AGENT_DATA_TYPE ag;
        DEVICE_HANDLE deviceHandle;
        (void)Device_Create(irrelevantModel, DeviceActionCallback, TEST_CALLBACK_CONTEXT, false, &deviceHandle);
        TRANSACTION_HANDLE transaction = Device_StartTransaction(deviceHandle);
        umock_c_reset_all_calls();

        STRICT_EXPECTED_CALL(DataPublisher_PublishTransacted(transaction, "p", &ag))
            .SetReturn(DATA_PUBLISHER_ERROR);

        // act
        DEVICE_RESULT result = Device_PublishTransacted(transaction, "p", &ag);

        // assert
        ASSERT_ARE_EQUAL(DEVICE_RESULT, DEVICE_DATA_PUBLISHER_FAILED, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///clean
        Device_CancelTransaction(transaction);
        Device_Destroy(deviceHandle);
    }

    /* Device_EndTransaction */

    /* Tests_SRS_DEVICE_01_038: [Device_EndTransaction shall invoke DataPublisher_EndTransaction.] */
    /* Tests_SRS_DEVICE_01_045: [On success, Device_EndTransaction shall return DEVICE_OK.] */
    TEST_FUNCTION(Device_EndTransaction_Calls_DataPublisher_And_Succeeds)
    {
        // arrange
        unsigned char* destination;
        size_t destinationSize;
        DEVICE_HANDLE deviceHandle;
        (void)Device_Create(irrelevantModel, DeviceActionCallback, TEST_CALLBACK_CONTEXT, false, &deviceHandle);
        TRANSACTION_HANDLE transaction = Device_StartTransaction(deviceHandle);
        umock_c_reset_all_calls();

        STRICT_EXPECTED_CALL(DataPublisher_EndTransaction(transaction, &destination, &destinationSize));

        // act
        DEVICE_RESULT result = Device_EndTransaction(transaction, &destination, &destinationSize);

        // assert
        ASSERT_ARE_EQUAL(DEVICE_RESULT, DEVICE_OK, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///clean
        Device_Destroy(deviceHandle);
    }

    /* Tests_SRS_DEVICE_01_039: [If transactionHandle is NULL, Device_EndTransaction shall return DEVICE_INVALID_ARG.]    */
    TEST_FUNCTION(Device_EndTransaction_Called_With_NULL_Handle_Fails)
    {
        // arrange

        // act
        DEVICE_RESULT result = Device_EndTransaction(NULL, NULL, NULL);

        // assert
        ASSERT_ARE_EQUAL(DEVICE_RESULT, DEVICE_INVALID_ARG, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    }

    /* Tests_SRS_DEVICE_01_050: [When DataPublisher_EndTransaction fails, Device_EndTransaction shall return DEVICE_DATA_PUBLISHER_FAILED.] */
    TEST_FUNCTION(When_DataPublisher_EndTransaction_Fails_Then_Device_EndTransaction_Fails)
    {
        // arrange
        DEVICE_HANDLE deviceHandle;
        (void)Device_Create(irrelevantModel, DeviceActionCallback, TEST_CALLBACK_CONTEXT, false, &deviceHandle);
        TRANSACTION_HANDLE transaction = Device_StartTransaction(deviceHandle);
        unsigned char* destination;
        size_t destinationSize;
        umock_c_reset_all_calls();

        STRICT_EXPECTED_CALL(DataPublisher_EndTransaction(transaction, &destination, &destinationSize))
            .SetReturn(DATA_PUBLISHER_ERROR);

        // act
        DEVICE_RESULT result = Device_EndTransaction(transaction, &destination, &destinationSize);

        // assert
        ASSERT_ARE_EQUAL(DEVICE_RESULT, DEVICE_DATA_PUBLISHER_FAILED, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///clean
        Device_CancelTransaction(transaction);
        Device_Destroy(deviceHandle);
    }

    /* Tests_SRS_DEVICE_01_038: [Device_EndTransaction shall invoke DataPublisher_EndTransaction.] */
    TEST_FUNCTION(Device_EndTransaction_Calls_DataPublisher_passingCallBackAndNullContext_Succeed)
    {
        // arrange
        DEVICE_HANDLE deviceHandle;
        (void)Device_Create(irrelevantModel, DeviceActionCallback, TEST_CALLBACK_CONTEXT, false, &deviceHandle);
        TRANSACTION_HANDLE transaction = Device_StartTransaction(deviceHandle);
        unsigned char* destination;
        size_t destinationSize;
        umock_c_reset_all_calls();

        STRICT_EXPECTED_CALL(DataPublisher_EndTransaction(transaction, &destination, &destinationSize));

        // act
        DEVICE_RESULT result = Device_EndTransaction(transaction, &destination, &destinationSize);

        // assert
        ASSERT_ARE_EQUAL(DEVICE_RESULT, DEVICE_OK, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///clean
        Device_Destroy(deviceHandle);
    }

    /* Tests_SRS_DEVICE_01_038: [Device_EndTransaction shall invoke DataPublisher_EndTransaction.] */
    TEST_FUNCTION(Device_EndTransaction_Calls_DataPublisher_passingCallBackAndAndContext_Succeed)
    {
        // arrange
        DEVICE_HANDLE deviceHandle;
        (void)Device_Create(irrelevantModel, DeviceActionCallback, TEST_CALLBACK_CONTEXT, false, &deviceHandle);
        TRANSACTION_HANDLE transaction = Device_StartTransaction(deviceHandle);
        unsigned char* destination;
        size_t destinationSize;
        umock_c_reset_all_calls();

        STRICT_EXPECTED_CALL(DataPublisher_EndTransaction(transaction, &destination, &destinationSize));

        // act
        DEVICE_RESULT result = Device_EndTransaction(transaction, &destination, &destinationSize);

        // assert
        ASSERT_ARE_EQUAL(DEVICE_RESULT, DEVICE_OK, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///clean
        Device_Destroy(deviceHandle);
    }

    /* Device_CancelTransaction */

    /* Tests_SRS_DEVICE_01_040: [Device_CancelTransaction shall invoke DataPublisher_CancelTransaction.] */
    /* Tests_SRS_DEVICE_01_046: [On success, Device_PublishTransacted shall return DEVICE_OK.] */
    TEST_FUNCTION(Device_CancelTransaction_Calls_DataPublisher_And_Succeeds)
    {
        // arrange
        DEVICE_HANDLE deviceHandle;
        (void)Device_Create(irrelevantModel, DeviceActionCallback, TEST_CALLBACK_CONTEXT, false, &deviceHandle);
        TRANSACTION_HANDLE transaction = Device_StartTransaction(deviceHandle);
        umock_c_reset_all_calls();

        STRICT_EXPECTED_CALL(DataPublisher_CancelTransaction(transaction));

        // act
        DEVICE_RESULT result = Device_CancelTransaction(transaction);

        // assert
        ASSERT_ARE_EQUAL(DEVICE_RESULT, DEVICE_OK, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///clean
        Device_Destroy(deviceHandle);
    }


    /* Tests_SRS_DEVICE_01_041: [If any argument is NULL, Device_CancelTransaction shall return DEVICE_INVALID_ARG.] */
    TEST_FUNCTION(Device_CancelTransaction_Called_With_NULL_Handle_Fails)
    {
        // arrange

        // act
        DEVICE_RESULT result = Device_CancelTransaction(NULL);

        // assert
        ASSERT_ARE_EQUAL(DEVICE_RESULT, DEVICE_INVALID_ARG, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    }


    /* Tests_SRS_DEVICE_01_051: [When DataPublisher_CancelTransaction fails, Device_CancelTransaction shall return DEVICE_DATA_PUBLISHER_FAILED.] */
    TEST_FUNCTION(When_DataPublisher_CancelTransaction_Fails_Then_Device_CancelTransaction_Fails)
    {
        // arrange
        DEVICE_HANDLE deviceHandle;
        (void)Device_Create(irrelevantModel, DeviceActionCallback, TEST_CALLBACK_CONTEXT, false, &deviceHandle);
        TRANSACTION_HANDLE transaction = Device_StartTransaction(deviceHandle);
        umock_c_reset_all_calls();

        STRICT_EXPECTED_CALL(DataPublisher_CancelTransaction(transaction))
            .SetReturn(DATA_PUBLISHER_ERROR);

        // act
        DEVICE_RESULT result = Device_CancelTransaction(transaction);

        // assert
        ASSERT_ARE_EQUAL(DEVICE_RESULT, DEVICE_DATA_PUBLISHER_FAILED, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///clean
        Device_CancelTransaction(transaction);
        Device_Destroy(deviceHandle);
    }

    /* Action callback */

    /*Tests_SRS_DEVICE_02_011: [If the parameter actionCallbackContent passed the callback is NULL then the callback shall return EXECUTION_COMMAND_ERROR.] */
    TEST_FUNCTION(When_Action_Callback_Is_Invoked_with_NULL_handle_returns_ABANDONED)
    {
        /// arrange
        DEVICE_HANDLE deviceHandle;
        (void)Device_Create(irrelevantModel, DeviceActionCallback, TEST_CALLBACK_CONTEXT, false, &deviceHandle);
        umock_c_reset_all_calls();

        /// act
        EXECUTE_COMMAND_RESULT result = ActionCallbackCalledByCommandDecoder(NULL, "", "testAction", 0, NULL);

        /// assert
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///clean
        Device_Destroy(deviceHandle);
    }

    /* Tests_SRS_DEVICE_01_052: [When the action callback passed to CommandDecoder is called, Device shall call the appropriate user callback associated with the device handle.] */
    /* Tests_SRS_DEVICE_01_053: [The action name, argument count and action arguments shall be passed to the user callback.] */
    /* Tests_SRS_DEVICE_01_055: [The value passed in callbackUserContext when creating the device shall be passed to the callback as the value for the callbackUserContext argument.] */
    TEST_FUNCTION(When_Action_Callback_Is_Invoked_The_User_Callback_Is_Invoked)
    {
        // arrange
        DEVICE_HANDLE deviceHandle;
        (void)Device_Create(irrelevantModel, DeviceActionCallback, TEST_CALLBACK_CONTEXT, false, &deviceHandle);
        umock_c_reset_all_calls();

        STRICT_EXPECTED_CALL(DeviceActionCallback(deviceHandle, TEST_CALLBACK_CONTEXT, "", "testAction", 0, NULL));

        // act
        EXECUTE_COMMAND_RESULT result = ActionCallbackCalledByCommandDecoder(deviceHandle, "", "testAction", 0, NULL);

        // assert
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_SUCCESS, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///clean
        Device_Destroy(deviceHandle);
    }

    /* Tests_SRS_DEVICE_01_052: [When the action callback passed to CommandDecoder is called, Device shall call the appropriate user callback associated with the device handle.] */
    /* Tests_SRS_DEVICE_01_053: [The action name, argument count and action arguments shall be passed to the user callback.] */
    /* Tests_SRS_DEVICE_01_055: [The value passed in callbackUserContext when creating the device shall be passed to the callback as the value for the callbackUserContext argument.] */
    TEST_FUNCTION(When_Action_Callback_Is_Invoked_With_1_Arg_The_User_Callback_Is_Invoked_With_Same_Values)
    {
        // arrange
        AGENT_DATA_TYPE ag;
        DEVICE_HANDLE deviceHandle;
        (void)Device_Create(irrelevantModel, DeviceActionCallback, TEST_CALLBACK_CONTEXT, false, &deviceHandle);
        umock_c_reset_all_calls();

        STRICT_EXPECTED_CALL(DeviceActionCallback(deviceHandle, TEST_CALLBACK_CONTEXT, "", "testAction", 1, &ag));

        // act
        EXECUTE_COMMAND_RESULT result = ActionCallbackCalledByCommandDecoder(deviceHandle, "", "testAction", 1, &ag);

        // assert
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_SUCCESS, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///clean
        Device_Destroy(deviceHandle);
    }

    /* Tests_SRS_DEVICE_01_054: [If the user callback returns a non-zero value, a non-zero value shall be returned to CommandReader.] */
    TEST_FUNCTION(When_Action_Callback_Is_Invoked_And_User_Callback_Fails_Action_Callback_Fails_Too)
    {
        // arrange
        AGENT_DATA_TYPE ag;
        DEVICE_HANDLE deviceHandle;
        (void)Device_Create(irrelevantModel, DeviceActionCallback, TEST_CALLBACK_CONTEXT, false, &deviceHandle);
        umock_c_reset_all_calls();

        STRICT_EXPECTED_CALL(DeviceActionCallback(deviceHandle, TEST_CALLBACK_CONTEXT, "", "testAction", 1, &ag))
            .SetReturn(EXECUTE_COMMAND_FAILED);

        // act
        EXECUTE_COMMAND_RESULT result = ActionCallbackCalledByCommandDecoder(deviceHandle, "", "testAction", 1, &ag);

        // assert
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_FAILED, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///clean
        Device_Destroy(deviceHandle);
    }

    /*Tests_SRS_DEVICE_02_012: [If any parameters are NULL, then Device_ExecuteCommand shall return EXECUTE_COMMAND_ERROR.]*/
    TEST_FUNCTION(Device_ExecuteCommand_with_NULL_handle_returns_EXECUTE_COMMAND_ERROR)
    {
        ///arrange

        ///act
        EXECUTE_COMMAND_RESULT result = Device_ExecuteCommand(NULL, "some command");

        ///assert
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
    }

    /*Tests_SRS_DEVICE_02_012: [If any parameters are NULL, then Device_ExecuteCommand shall return EXECUTE_COMMAND_ERROR.]*/
    TEST_FUNCTION(Device_ExecuteCommand_with_NULL_command_returns_EXECUTE_COMMAND_ERROR)
    {
        ///arrange
        DEVICE_HANDLE h;
        Device_Create(irrelevantModel, DeviceActionCallback, TEST_CALLBACK_CONTEXT, false, &h);
        umock_c_reset_all_calls();

        ///act
        EXECUTE_COMMAND_RESULT result = Device_ExecuteCommand(h,NULL);

        ///assert
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        Device_Destroy(h);
    }

    /*Tests_SRS_DEVICE_02_013: [Otherwise, Device_ExecuteCommand shall call CommandDecoder_ExecuteCommand and return what CommandDecoder_ExecuteCommand is returning.]*/
    TEST_FUNCTION(Device_ExecuteCommand_returns_what_CommandDecoder_ExecuteCommand_returns_EXECUTE_COMMAND_SUCCESS)
    {
        ///arrange
        DEVICE_HANDLE h;
        Device_Create(irrelevantModel, DeviceActionCallback, TEST_CALLBACK_CONTEXT, false, &h);
        umock_c_reset_all_calls();

        STRICT_EXPECTED_CALL(CommandDecoder_ExecuteCommand(IGNORED_PTR_ARG, "some command"))
            .IgnoreArgument(1);

        ///act
        EXECUTE_COMMAND_RESULT result = Device_ExecuteCommand(h, "some command");

        ///assert
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_SUCCESS, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        Device_Destroy(h);
    }

    /*Tests_SRS_DEVICE_02_013: [Otherwise, Device_ExecuteCommand shall call CommandDecoder_ExecuteCommand and return what CommandDecoder_ExecuteCommand is returning.]*/
    TEST_FUNCTION(Device_ExecuteCommand_returns_what_CommandDecoder_ExecuteCommand_returns_EXECUTE_COMMAND_FAILED)
    {
        ///arrange
        DEVICE_HANDLE h;
        Device_Create(irrelevantModel, DeviceActionCallback, TEST_CALLBACK_CONTEXT, false, &h);
        umock_c_reset_all_calls();

        STRICT_EXPECTED_CALL(CommandDecoder_ExecuteCommand(IGNORED_PTR_ARG, "some command"))
            .IgnoreArgument(1)
            .SetReturn(EXECUTE_COMMAND_FAILED);

        ///act
        EXECUTE_COMMAND_RESULT result = Device_ExecuteCommand(h, "some command");

        ///assert
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_FAILED, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        Device_Destroy(h);
    }

    /*Tests_SRS_DEVICE_02_013: [Otherwise, Device_ExecuteCommand shall call CommandDecoder_ExecuteCommand and return what CommandDecoder_ExecuteCommand is returning.]*/
    TEST_FUNCTION(Device_ExecuteCommand_returns_what_CommandDecoder_ExecuteCommand_returns_EXECUTE_COMMAND_ERROR)
    {
        ///arrange
        DEVICE_HANDLE h;
        Device_Create(irrelevantModel, DeviceActionCallback, TEST_CALLBACK_CONTEXT, false, &h);
        umock_c_reset_all_calls();

        STRICT_EXPECTED_CALL(CommandDecoder_ExecuteCommand(IGNORED_PTR_ARG, "some command"))
            .IgnoreArgument(1)
            .SetReturn(EXECUTE_COMMAND_ERROR);

        ///act
        EXECUTE_COMMAND_RESULT result = Device_ExecuteCommand(h, "some command");

        ///assert
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///cleanup
        Device_Destroy(h);
    }

    /*Tests_SRS_DEVICE_02_014: [ If argument deviceHandle is NULL then Device_CreateTransaction_ReportedProperties shall fail and return NULL. ]*/
    TEST_FUNCTION(Device_CreateTransaction_ReportedProperties_with_NULL_deviceHandle_fails)
    {
        ///arrange

        ///act
        REPORTED_PROPERTIES_TRANSACTION_HANDLE reportedPropertiesTransactionHandle = Device_CreateTransaction_ReportedProperties(NULL);

        ///assert
        ASSERT_IS_NULL(reportedPropertiesTransactionHandle);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///clean
    }

    /*Tests_SRS_DEVICE_02_015: [ Otherwise, Device_CreateTransaction_ReportedProperties shall call DataPublisher_CreateTransaction_ReportedProperties. ]*/
    /*Tests_SRS_DEVICE_02_017: [ Otherwise Device_CreateTransaction_ReportedProperties shall succeed and return a non-NULL value. ]*/
    TEST_FUNCTION(Device_CreateTransaction_ReportedProperties_succeeds)
    {
        ///arrange
        DEVICE_HANDLE h;
        Device_Create(irrelevantModel, DeviceActionCallback, TEST_CALLBACK_CONTEXT, false, &h);
        umock_c_reset_all_calls();

        STRICT_EXPECTED_CALL(DataPublisher_CreateTransaction_ReportedProperties(IGNORED_PTR_ARG))
            .IgnoreArgument_dataPublisherHandle();

        ///act
        REPORTED_PROPERTIES_TRANSACTION_HANDLE reportedPropertiesTransactionHandle = Device_CreateTransaction_ReportedProperties(h);

        ///assert
        ASSERT_IS_NOT_NULL(reportedPropertiesTransactionHandle);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///clean
        Device_DestroyTransaction_ReportedProperties(reportedPropertiesTransactionHandle);
        Device_Destroy(h);
    }

    /*Tests_SRS_DEVICE_02_016: [ If DataPublisher_CreateTransaction_ReportedProperties fails then Device_CreateTransaction_ReportedProperties shall fail and return NULL. ]*/
    TEST_FUNCTION(Device_CreateTransaction_ReportedProperties_fails)
    {
        ///arrange
        DEVICE_HANDLE h;
        Device_Create(irrelevantModel, DeviceActionCallback, TEST_CALLBACK_CONTEXT, false, &h);
        umock_c_reset_all_calls();

        STRICT_EXPECTED_CALL(DataPublisher_CreateTransaction_ReportedProperties(IGNORED_PTR_ARG))
            .IgnoreArgument_dataPublisherHandle()
            .SetReturn(NULL);

        ///act
        REPORTED_PROPERTIES_TRANSACTION_HANDLE reportedPropertiesTransactionHandle = Device_CreateTransaction_ReportedProperties(h);

        ///assert
        ASSERT_IS_NULL(reportedPropertiesTransactionHandle);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///clean
        Device_Destroy(h);
    }

    /*Tests_SRS_DEVICE_02_018: [ If argument transactionHandle is NULL then Device_PublishTransacted_ReportedProperty shall fail and return DEVICE_INVALID_ARG. ]*/
    TEST_FUNCTION(Device_PublishTransacted_ReportedProperty_with_NULL_transactionHandle_fails)
    {
        ///arrange
        AGENT_DATA_TYPE ag;

        ///act
        DEVICE_RESULT result = Device_PublishTransacted_ReportedProperty(NULL, "a", &ag);

        ///assert
        ASSERT_ARE_EQUAL(DEVICE_RESULT, DEVICE_INVALID_ARG, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///clean
    }

    /*Tests_SRS_DEVICE_02_019: [ If argument reportedPropertyPath is NULL then Device_PublishTransacted_ReportedProperty shall fail and return DEVICE_INVALID_ARG. ]*/
    TEST_FUNCTION(Device_PublishTransacted_ReportedProperty_with_NULL_reportedPropertyPath_fails)
    {
        ///arrange
        AGENT_DATA_TYPE ag;
        DEVICE_HANDLE h;
        Device_Create(irrelevantModel, DeviceActionCallback, TEST_CALLBACK_CONTEXT, false, &h);
        REPORTED_PROPERTIES_TRANSACTION_HANDLE reportedPropertiesTransactionHandle = Device_CreateTransaction_ReportedProperties(h);
        umock_c_reset_all_calls();

        ///act
        DEVICE_RESULT result = Device_PublishTransacted_ReportedProperty(reportedPropertiesTransactionHandle, NULL, &ag);

        ///assert
        ASSERT_ARE_EQUAL(DEVICE_RESULT, DEVICE_INVALID_ARG, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///clean
        Device_DestroyTransaction_ReportedProperties(reportedPropertiesTransactionHandle);
        Device_Destroy(h);
    }

    /*Tests_SRS_DEVICE_02_020: [ If argument data is NULL then Device_PublishTransacted_ReportedProperty shall fail and return DEVICE_INVALID_ARG. ]*/
    TEST_FUNCTION(Device_PublishTransacted_ReportedProperty_with_NULL_data_fails)
    {
        ///arrange
        DEVICE_HANDLE h;
        Device_Create(irrelevantModel, DeviceActionCallback, TEST_CALLBACK_CONTEXT, false, &h);
        REPORTED_PROPERTIES_TRANSACTION_HANDLE reportedPropertiesTransactionHandle = Device_CreateTransaction_ReportedProperties(h);
        umock_c_reset_all_calls();

        ///act
        DEVICE_RESULT result = Device_PublishTransacted_ReportedProperty(reportedPropertiesTransactionHandle, "a", NULL);

        ///assert
        ASSERT_ARE_EQUAL(DEVICE_RESULT, DEVICE_INVALID_ARG, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///clean
        Device_DestroyTransaction_ReportedProperties(reportedPropertiesTransactionHandle);
        Device_Destroy(h);
    }

    /*Tests_SRS_DEVICE_02_021: [ Device_PublishTransacted_ReportedProperty shall call DataPublisher_PublishTransacted_ReportedProperty. ]*/
    /*Tests_SRS_DEVICE_02_023: [ Otherwise, Device_PublishTransacted_ReportedProperty shall succeed and return DEVICE_OK. ]*/
    TEST_FUNCTION(Device_PublishTransacted_ReportedProperty_succeeds)
    {
        ///arrange
        AGENT_DATA_TYPE ag;
        DEVICE_HANDLE h;
        Device_Create(irrelevantModel, DeviceActionCallback, TEST_CALLBACK_CONTEXT, false, &h);
        REPORTED_PROPERTIES_TRANSACTION_HANDLE reportedPropertiesTransactionHandle = Device_CreateTransaction_ReportedProperties(h);
        umock_c_reset_all_calls();

        STRICT_EXPECTED_CALL(DataPublisher_PublishTransacted_ReportedProperty(IGNORED_PTR_ARG, "a", &ag))
            .IgnoreArgument_transactionHandle();

        ///act
        DEVICE_RESULT result = Device_PublishTransacted_ReportedProperty(reportedPropertiesTransactionHandle, "a", &ag);

        ///assert
        ASSERT_ARE_EQUAL(DEVICE_RESULT, DEVICE_OK, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///clean
        Device_DestroyTransaction_ReportedProperties(reportedPropertiesTransactionHandle);
        Device_Destroy(h);
    }

    /*Tests_SRS_DEVICE_02_022: [ If DataPublisher_PublishTransacted_ReportedProperty fails then Device_PublishTransacted_ReportedProperty shall fail and return DEVICE_DATA_PUBLISHER_FAILED. ]*/
    TEST_FUNCTION(Device_PublishTransacted_ReportedProperty_fails)
    {
        ///arrange
        AGENT_DATA_TYPE ag;
        DEVICE_HANDLE h;
        Device_Create(irrelevantModel, DeviceActionCallback, TEST_CALLBACK_CONTEXT, false, &h);
        REPORTED_PROPERTIES_TRANSACTION_HANDLE reportedPropertiesTransactionHandle = Device_CreateTransaction_ReportedProperties(h);
        umock_c_reset_all_calls();

        STRICT_EXPECTED_CALL(DataPublisher_PublishTransacted_ReportedProperty(IGNORED_PTR_ARG, "a", &ag))
            .IgnoreArgument_transactionHandle()
            .SetReturn(DATA_PUBLISHER_ERROR);

        ///act
        DEVICE_RESULT result = Device_PublishTransacted_ReportedProperty(reportedPropertiesTransactionHandle, "a", &ag);

        ///assert
        ASSERT_ARE_EQUAL(DEVICE_RESULT, DEVICE_DATA_PUBLISHER_FAILED, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///clean
        Device_DestroyTransaction_ReportedProperties(reportedPropertiesTransactionHandle);
        Device_Destroy(h);
    }

    /*Tests_SRS_DEVICE_02_024: [ If argument transactionHandle is NULL then Device_CommitTransaction_ReportedProperties shall fail and return DEVICE_INVALID_ARG. ]*/
    TEST_FUNCTION(Device_CommitTransaction_ReportedProperties_with_NULL_transactionHandle_fails)
    {
        ///arrange
        size_t destinationSize;
        unsigned char* destination;

        ///act
        DEVICE_RESULT result = Device_CommitTransaction_ReportedProperties(NULL, &destination, &destinationSize);

        ///assert
        ASSERT_ARE_EQUAL(DEVICE_RESULT, DEVICE_INVALID_ARG, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    }

    /*Tests_SRS_DEVICE_02_025: [ If argument destination is NULL then Device_CommitTransaction_ReportedProperties shall fail and return DEVICE_INVALID_ARG. ]*/
    TEST_FUNCTION(Device_CommitTransaction_ReportedProperties_with_NULL_destination_fails)
    {
        ///arrange
        DEVICE_HANDLE h;
        size_t destinationSize;
        Device_Create(irrelevantModel, DeviceActionCallback, TEST_CALLBACK_CONTEXT, false, &h);
        REPORTED_PROPERTIES_TRANSACTION_HANDLE reportedPropertiesTransactionHandle = Device_CreateTransaction_ReportedProperties(h);
        umock_c_reset_all_calls();

        ///act
        DEVICE_RESULT result = Device_CommitTransaction_ReportedProperties(reportedPropertiesTransactionHandle, NULL, &destinationSize);

        ///assert
        ASSERT_ARE_EQUAL(DEVICE_RESULT, DEVICE_INVALID_ARG, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///clean
        Device_DestroyTransaction_ReportedProperties(reportedPropertiesTransactionHandle);
        Device_Destroy(h);
        
    }

    /*Tests_SRS_DEVICE_02_026: [ If argument destinationSize is NULLthen Device_CommitTransaction_ReportedProperties shall fail and return DEVICE_INVALID_ARG. ]*/
    TEST_FUNCTION(Device_CommitTransaction_ReportedProperties_with_NULL_destinationSize_fails)
    {
        ///arrange
        DEVICE_HANDLE h;
        unsigned char* destination;
        Device_Create(irrelevantModel, DeviceActionCallback, TEST_CALLBACK_CONTEXT, false, &h);
        REPORTED_PROPERTIES_TRANSACTION_HANDLE reportedPropertiesTransactionHandle = Device_CreateTransaction_ReportedProperties(h);
        umock_c_reset_all_calls();

        ///act
        DEVICE_RESULT result = Device_CommitTransaction_ReportedProperties(reportedPropertiesTransactionHandle, &destination, NULL);

        ///assert
        ASSERT_ARE_EQUAL(DEVICE_RESULT, DEVICE_INVALID_ARG, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///clean
        Device_DestroyTransaction_ReportedProperties(reportedPropertiesTransactionHandle);
        Device_Destroy(h);
    }

    /*Tests_SRS_DEVICE_02_027: [ Device_CommitTransaction_ReportedProperties shall call DataPublisher_CommitTransaction_ReportedProperties. ]*/
    /*Tests_SRS_DEVICE_02_029: [ Otherwise Device_CommitTransaction_ReportedProperties shall succeed and return DEVICE_OK. ]*/
    TEST_FUNCTION(Device_CommitTransaction_ReportedProperties_succeeds)
    {
        ///arrange
        DEVICE_HANDLE h;
        unsigned char* destination;
        size_t destinationSize;
        Device_Create(irrelevantModel, DeviceActionCallback, TEST_CALLBACK_CONTEXT, false, &h);
        REPORTED_PROPERTIES_TRANSACTION_HANDLE reportedPropertiesTransactionHandle = Device_CreateTransaction_ReportedProperties(h);
        umock_c_reset_all_calls();

        STRICT_EXPECTED_CALL(DataPublisher_CommitTransaction_ReportedProperties(IGNORED_PTR_ARG, &destination, &destinationSize))
            .IgnoreArgument_transactionHandle();

        ///act
        DEVICE_RESULT result = Device_CommitTransaction_ReportedProperties(reportedPropertiesTransactionHandle, &destination, &destinationSize);

        ///assert
        ASSERT_ARE_EQUAL(DEVICE_RESULT, DEVICE_OK, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///clean
        Device_DestroyTransaction_ReportedProperties(reportedPropertiesTransactionHandle);
        Device_Destroy(h);
    }

    /*Tests_SRS_DEVICE_02_028: [ If DataPublisher_CommitTransaction_ReportedProperties fails then Device_CommitTransaction_ReportedProperties shall fail and return DEVICE_DATA_PUBLISHER_FAILED. ]*/
    TEST_FUNCTION(Device_CommitTransaction_ReportedProperties_fails)
    {
        ///arrange
        DEVICE_HANDLE h;
        unsigned char* destination;
        size_t destinationSize;
        Device_Create(irrelevantModel, DeviceActionCallback, TEST_CALLBACK_CONTEXT, false, &h);
        REPORTED_PROPERTIES_TRANSACTION_HANDLE reportedPropertiesTransactionHandle = Device_CreateTransaction_ReportedProperties(h);
        umock_c_reset_all_calls();

        STRICT_EXPECTED_CALL(DataPublisher_CommitTransaction_ReportedProperties(IGNORED_PTR_ARG, &destination, &destinationSize))
            .IgnoreArgument_transactionHandle()
            .SetReturn(DATA_PUBLISHER_ERROR);

        ///act
        DEVICE_RESULT result = Device_CommitTransaction_ReportedProperties(reportedPropertiesTransactionHandle, &destination, &destinationSize);

        ///assert
        ASSERT_ARE_EQUAL(DEVICE_RESULT, DEVICE_DATA_PUBLISHER_FAILED, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///clean
        Device_DestroyTransaction_ReportedProperties(reportedPropertiesTransactionHandle);
        Device_Destroy(h);
    }

    /*Tests_SRS_DEVICE_02_030: [ If argument transactionHandle is NULL then Device_DestroyTransaction_ReportedProperties shall return. ]*/
    TEST_FUNCTION(Device_DestroyTransaction_ReportedProperties_with_NULL_returns)
    {
        ///arrange
        
        ///act
        Device_DestroyTransaction_ReportedProperties(NULL);

        ///assert
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///clean
    }

    /*Tests_SRS_DEVICE_02_031: [ Otherwise Device_DestroyTransaction_ReportedProperties shall free all used resources. ]*/
    TEST_FUNCTION(Device_DestroyTransaction_ReportedProperties_succeeds_1)
    {
        ///arrange
        DEVICE_HANDLE h;
        Device_Create(irrelevantModel, DeviceActionCallback, TEST_CALLBACK_CONTEXT, false, &h);
        REPORTED_PROPERTIES_TRANSACTION_HANDLE reportedPropertiesTransactionHandle = Device_CreateTransaction_ReportedProperties(h);
        umock_c_reset_all_calls();

        STRICT_EXPECTED_CALL(DataPublisher_DestroyTransaction_ReportedProperties(IGNORED_PTR_ARG))
            .IgnoreArgument_transactionHandle();

        ///act
        Device_DestroyTransaction_ReportedProperties(reportedPropertiesTransactionHandle);

        ///assert
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///clean
        Device_Destroy(h);
    }

    /*Tests_SRS_DEVICE_02_031: [ Otherwise Device_DestroyTransaction_ReportedProperties shall free all used resources. ]*/
    TEST_FUNCTION(Device_DestroyTransaction_ReportedProperties_succeeds_2)
    {
        ///arrange
        AGENT_DATA_TYPE ag;
        DEVICE_HANDLE h;
        Device_Create(irrelevantModel, DeviceActionCallback, TEST_CALLBACK_CONTEXT, false, &h);
        REPORTED_PROPERTIES_TRANSACTION_HANDLE reportedPropertiesTransactionHandle = Device_CreateTransaction_ReportedProperties(h);
        (void)Device_PublishTransacted_ReportedProperty(reportedPropertiesTransactionHandle, "a", &ag);
        umock_c_reset_all_calls();

        STRICT_EXPECTED_CALL(DataPublisher_DestroyTransaction_ReportedProperties(IGNORED_PTR_ARG))
            .IgnoreArgument_transactionHandle();

        ///act
        Device_DestroyTransaction_ReportedProperties(reportedPropertiesTransactionHandle);

        ///assert
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///clean
        Device_Destroy(h);
    }

    /*Tests_SRS_DEVICE_02_031: [ Otherwise Device_DestroyTransaction_ReportedProperties shall free all used resources. ]*/
    TEST_FUNCTION(Device_DestroyTransaction_ReportedProperties_succeeds_3)
    {
        ///arrange
        AGENT_DATA_TYPE ag;
        DEVICE_HANDLE h;
        unsigned char* destination;
        size_t destinationSize;
        Device_Create(irrelevantModel, DeviceActionCallback, TEST_CALLBACK_CONTEXT, false, &h);
        REPORTED_PROPERTIES_TRANSACTION_HANDLE reportedPropertiesTransactionHandle = Device_CreateTransaction_ReportedProperties(h);
        (void)Device_PublishTransacted_ReportedProperty(reportedPropertiesTransactionHandle, "a", &ag);
        (void)Device_CommitTransaction_ReportedProperties(reportedPropertiesTransactionHandle, &destination, &destinationSize);
        umock_c_reset_all_calls();

        STRICT_EXPECTED_CALL(DataPublisher_DestroyTransaction_ReportedProperties(IGNORED_PTR_ARG))
            .IgnoreArgument_transactionHandle();

        ///act
        Device_DestroyTransaction_ReportedProperties(reportedPropertiesTransactionHandle);

        ///assert
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///clean
        Device_Destroy(h);
    }

    /*Tests_SRS_DEVICE_02_032: [ If deviceHandle is NULL then Device_IngestDesiredProperties shall fail and return DEVICE_INVALID_ARG. ]*/
    TEST_FUNCTION(Device_IngestDesiredProperties_with_NULL_deviceHandle_fails)
    {
        ///arrange

        ///act
        DEVICE_RESULT result = Device_IngestDesiredProperties(FAKE_DEVICE_START_ADDRESS, NULL, "{}");


        ///assert
        ASSERT_ARE_EQUAL(DEVICE_RESULT, DEVICE_INVALID_ARG, result);

        ///clean
    }

    /*Tests_SRS_DEVICE_02_033: [ If desiredProperties is NULL then Device_IngestDesiredProperties shall fail and return DEVICE_INVALID_ARG. ]*/
    TEST_FUNCTION(Device_IngestDesiredProperties_with_NULL_desiredProperties_fails)
    {        
        ///arrange
        DEVICE_HANDLE h;
        Device_Create(irrelevantModel, DeviceActionCallback, TEST_CALLBACK_CONTEXT, false, &h);
        umock_c_reset_all_calls();

        ///act
        DEVICE_RESULT result = Device_IngestDesiredProperties(FAKE_DEVICE_START_ADDRESS, NULL, "{}");

        ///assert
        ASSERT_ARE_EQUAL(DEVICE_RESULT, DEVICE_INVALID_ARG, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///clean
        Device_Destroy(h);
    }

    /*Tests_SRS_DEVICE_02_037: [ If startAddress is NULL then Device_IngestDesiredProperties shall fail and return DEVICE_INVALID_ARG. ]*/
    TEST_FUNCTION(Device_IngestDesiredProperties_with_NULL_startAddress_fails)
    {
        ///arrange
        DEVICE_HANDLE h;
        Device_Create(irrelevantModel, DeviceActionCallback, TEST_CALLBACK_CONTEXT, false, &h);
        umock_c_reset_all_calls();

        ///act
        DEVICE_RESULT result = Device_IngestDesiredProperties(NULL, h, "{}");

        ///assert
        ASSERT_ARE_EQUAL(DEVICE_RESULT, DEVICE_INVALID_ARG, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///clean
        Device_Destroy(h);
    }
    /*Tests_SRS_DEVICE_02_034: [ Device_IngestDesiredProperties shall call CommandDecoder_IngestDesiredProperties. ]*/
    /*Tests_SRS_DEVICE_02_036: [ Otherwise, Device_IngestDesiredProperties shall succeed and return DEVICE_OK. ]*/
    TEST_FUNCTION(Device_IngestDesiredProperties_succeeds)
    {
        ///arrange
        DEVICE_HANDLE h;
        Device_Create(irrelevantModel, DeviceActionCallback, TEST_CALLBACK_CONTEXT, false, &h);
        umock_c_reset_all_calls();

        STRICT_EXPECTED_CALL(CommandDecoder_IngestDesiredProperties(FAKE_DEVICE_START_ADDRESS, IGNORED_PTR_ARG, "{}"))
            .IgnoreArgument_handle();

        ///act
        DEVICE_RESULT result = Device_IngestDesiredProperties(FAKE_DEVICE_START_ADDRESS, h, "{}");

        ///assert
        ASSERT_ARE_EQUAL(DEVICE_RESULT, DEVICE_OK, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///clean
        Device_Destroy(h);
    }

    /*Tests_SRS_DEVICE_02_035: [ If any failure happens then Device_IngestDesiredProperties shall fail and return DEVICE_ERROR. ]*/
    TEST_FUNCTION(Device_IngestDesiredProperties_fails)
    {
        ///arrange
        DEVICE_HANDLE h;
        Device_Create(irrelevantModel, DeviceActionCallback, TEST_CALLBACK_CONTEXT, false, &h);
        umock_c_reset_all_calls();

        STRICT_EXPECTED_CALL(CommandDecoder_IngestDesiredProperties(FAKE_DEVICE_START_ADDRESS, IGNORED_PTR_ARG, "{}"))
            .IgnoreArgument_handle()
            .SetReturn(EXECUTE_COMMAND_FAILED);

        ///act
        DEVICE_RESULT result = Device_IngestDesiredProperties(FAKE_DEVICE_START_ADDRESS, h, "{}");

        ///assert
        ASSERT_ARE_EQUAL(DEVICE_RESULT, DEVICE_ERROR, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        ///clean
        Device_Destroy(h);
    }

END_TEST_SUITE(IoTDevice_ut)
