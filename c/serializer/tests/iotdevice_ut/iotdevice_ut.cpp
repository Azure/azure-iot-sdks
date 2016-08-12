// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <cstdlib>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include <cstdbool>
#include "testrunnerswitcher.h"
#include "micromock.h"
#include "micromockenumtostring.h"
#include "micromockcharstararenullterminatedstrings.h"

#include "iotdevice.h"
#include "schema.h"
#include "commanddecoder.h"
#include "datapublisher.h"


DEFINE_MICROMOCK_ENUM_TO_STRING(DEVICE_RESULT, DEVICE_RESULT_VALUES);
DEFINE_MICROMOCK_ENUM_TO_STRING(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_RESULT_VALUES)


static const DATA_PUBLISHER_HANDLE TEST_DATA_PUBLISHER_HANDLE = (DATA_PUBLISHER_HANDLE)0x4444;
static const SCHEMA_HANDLE TEST_SCHEMA_HANDLE = (SCHEMA_HANDLE)0x4245;
static const TRANSACTION_HANDLE TEST_TRANSACTION_HANDLE = (TRANSACTION_HANDLE)0x4246;
static const COMMAND_DECODER_HANDLE TEST_COMMAND_DECODER_HANDLE = (COMMAND_DECODER_HANDLE)0x4247;
static const char* TEST_MODEL_NAME = "Photo_Model";

#define TEST_ENTITY_SET_NAME            "SomeTrucks"
#define TEST_ENTITY_SET_NAMESPACE       "WhiteTrucksDemo"

#define TEST_CALLBACK_CONTEXT           (void*)0x4246

static ACTION_CALLBACK_FUNC ActionCallbackCalledByCommandDecoder;

TYPED_MOCK_CLASS(CDeviceMocks, CGlobalMock)
{
public:
    /* DataPublisher mocks */
    MOCK_STATIC_METHOD_2(, DATA_PUBLISHER_HANDLE, DataPublisher_Create, SCHEMA_MODEL_TYPE_HANDLE, modelHandle, bool, includePropertyPath)
    MOCK_METHOD_END(DATA_PUBLISHER_HANDLE, TEST_DATA_PUBLISHER_HANDLE)
    MOCK_STATIC_METHOD_1(, void, DataPublisher_Destroy, DATA_PUBLISHER_HANDLE, dataPublisherHandle)
    MOCK_VOID_METHOD_END()

    /* CommandDecoder mocks */
    MOCK_STATIC_METHOD_3(, COMMAND_DECODER_HANDLE, CommandDecoder_Create, SCHEMA_MODEL_TYPE_HANDLE, modelHandle, ACTION_CALLBACK_FUNC, actionCallback, void*, actionCallbackContext)
        ActionCallbackCalledByCommandDecoder = actionCallback;
    MOCK_METHOD_END(COMMAND_DECODER_HANDLE, TEST_COMMAND_DECODER_HANDLE)
    MOCK_STATIC_METHOD_2(, EXECUTE_COMMAND_RESULT, CommandDecoder_ExecuteCommand, COMMAND_DECODER_HANDLE, handle, const char*, command)
    MOCK_METHOD_END(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_SUCCESS)

    MOCK_STATIC_METHOD_1(, void, CommandDecoder_Destroy, COMMAND_DECODER_HANDLE, commandDecoderHandle)
    MOCK_VOID_METHOD_END()

    /* Device action callback mock */
    MOCK_STATIC_METHOD_6(, EXECUTE_COMMAND_RESULT, DeviceActionCallback, DEVICE_HANDLE, deviceHandle, void*, callbackUserContext, const char*, relativeActionPath, const char*, actionName, size_t, argCount, const AGENT_DATA_TYPE*, arguments)
    MOCK_METHOD_END(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_SUCCESS)

    /* DataPublisher mocks */
    MOCK_STATIC_METHOD_1(, TRANSACTION_HANDLE, DataPublisher_StartTransaction, SCHEMA_MODEL_TYPE_HANDLE, modelHandle)
    MOCK_METHOD_END(TRANSACTION_HANDLE, TEST_TRANSACTION_HANDLE);
    MOCK_STATIC_METHOD_3(, DATA_PUBLISHER_RESULT, DataPublisher_EndTransaction, TRANSACTION_HANDLE, transactionHandle, unsigned char**, destination, size_t*, destinationSize)
    MOCK_METHOD_END(DATA_PUBLISHER_RESULT, DATA_PUBLISHER_OK);
    MOCK_STATIC_METHOD_1(, DATA_PUBLISHER_RESULT, DataPublisher_CancelTransaction, TRANSACTION_HANDLE, transactionHandle)
    MOCK_METHOD_END(DATA_PUBLISHER_RESULT, DATA_PUBLISHER_OK);
    MOCK_STATIC_METHOD_3(, DATA_PUBLISHER_RESULT, DataPublisher_PublishTransacted, TRANSACTION_HANDLE, transactionHandle, const char*, propertyPath, const AGENT_DATA_TYPE*, data)
    MOCK_METHOD_END(DATA_PUBLISHER_RESULT, DATA_PUBLISHER_OK);
};

DECLARE_GLOBAL_MOCK_METHOD_2(CDeviceMocks, , DATA_PUBLISHER_HANDLE, DataPublisher_Create, SCHEMA_MODEL_TYPE_HANDLE, modelHandle, bool, includePropertyPath);
DECLARE_GLOBAL_MOCK_METHOD_1(CDeviceMocks, , void, DataPublisher_Destroy, DATA_PUBLISHER_HANDLE, dataPublisherHandle);

DECLARE_GLOBAL_MOCK_METHOD_3(CDeviceMocks, , COMMAND_DECODER_HANDLE, CommandDecoder_Create, SCHEMA_MODEL_TYPE_HANDLE, modelHandle, ACTION_CALLBACK_FUNC, actionCallback, void*, actionCallbackContext);
DECLARE_GLOBAL_MOCK_METHOD_2(CDeviceMocks, ,EXECUTE_COMMAND_RESULT, CommandDecoder_ExecuteCommand, COMMAND_DECODER_HANDLE, handle, const char*, command)
DECLARE_GLOBAL_MOCK_METHOD_1(CDeviceMocks, , void, CommandDecoder_Destroy, COMMAND_DECODER_HANDLE, commandDecoderHandle)

DECLARE_GLOBAL_MOCK_METHOD_6(CDeviceMocks, , EXECUTE_COMMAND_RESULT, DeviceActionCallback, DEVICE_HANDLE, deviceHandle, void*, callbackUserContext, const char*, relativeActionPath, const char*, actionName, size_t, argCount, const AGENT_DATA_TYPE*, arguments);

DECLARE_GLOBAL_MOCK_METHOD_1(CDeviceMocks, , TRANSACTION_HANDLE, DataPublisher_StartTransaction, SCHEMA_MODEL_TYPE_HANDLE, modelHandle)
DECLARE_GLOBAL_MOCK_METHOD_3(CDeviceMocks, , DATA_PUBLISHER_RESULT, DataPublisher_EndTransaction, TRANSACTION_HANDLE, transactionHandle, unsigned char**, destination, size_t*, destinationSize)
DECLARE_GLOBAL_MOCK_METHOD_1(CDeviceMocks, , DATA_PUBLISHER_RESULT, DataPublisher_CancelTransaction, TRANSACTION_HANDLE, transactionHandle)
DECLARE_GLOBAL_MOCK_METHOD_3(CDeviceMocks, , DATA_PUBLISHER_RESULT, DataPublisher_PublishTransacted, TRANSACTION_HANDLE, transactionHandle, const char*, propertyPath, const AGENT_DATA_TYPE*, data)

namespace
{
    SCHEMA_MODEL_TYPE_HANDLE irrelevantModel = (SCHEMA_MODEL_TYPE_HANDLE)0x1;
    const char* irrelevantName = "name";

    DEVICE_HANDLE CreateDevice_(SCHEMA_MODEL_TYPE_HANDLE model)
    {
        DEVICE_HANDLE h;
        (void)Device_Create(model, DeviceActionCallback, TEST_CALLBACK_CONTEXT, false, &h);
        return h;
    }

    DEVICE_HANDLE CreateDeviceWithName_(void)
    {
        return CreateDevice_(irrelevantModel);
    }

    class AutoDevice
    {
        DEVICE_HANDLE h_;

    public:
        explicit AutoDevice(DEVICE_HANDLE deviceHandle) : h_(deviceHandle) {}
        ~AutoDevice() { if (h_) Device_Destroy(h_); }
        DEVICE_HANDLE Handle() { return h_; }
    };
}

static MICROMOCK_MUTEX_HANDLE g_testByTest;
static MICROMOCK_GLOBAL_SEMAPHORE_HANDLE g_dllByDll;

BEGIN_TEST_SUITE(IoTDevice_ut)

    TEST_SUITE_INITIALIZE(BeforeSuite)
    {
        TEST_INITIALIZE_MEMORY_DEBUG(g_dllByDll);
        g_testByTest = MicroMockCreateMutex();
        ASSERT_IS_NOT_NULL(g_testByTest);
    }

    TEST_SUITE_CLEANUP(AfterEachSuite)
    {
        MicroMockDestroyMutex(g_testByTest);
        TEST_DEINITIALIZE_MEMORY_DEBUG(g_dllByDll);
    }

    /* Tests_SRS_DEVICE_05_014: [If any of the modelHandle, deviceHandle or deviceActionCallback arguments are NULL, Device_Create shall return DEVICE_INVALID_ARG.]*/
    TEST_FUNCTION(Device_Create_with_NULL_model_handle_fails)
    {
        // arrange
        CDeviceMocks deviceMocks;
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
        CDeviceMocks deviceMocks;
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
        CDeviceMocks deviceMocks;

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
        CDeviceMocks deviceMocks;
        DEVICE_HANDLE h = NULL;

        STRICT_EXPECTED_CALL(deviceMocks, DataPublisher_Create(irrelevantModel, true));
        STRICT_EXPECTED_CALL(deviceMocks, CommandDecoder_Create(irrelevantModel, NULL, NULL))
            .IgnoreArgument(2).IgnoreArgument(3);

        // act
        DEVICE_RESULT res = Device_Create(irrelevantModel, DeviceActionCallback, TEST_CALLBACK_CONTEXT, true, &h);

        // assert
        ASSERT_ARE_EQUAL(DEVICE_RESULT, DEVICE_OK, res);
        ASSERT_IS_NOT_NULL(h);
        deviceMocks.AssertActualAndExpectedCalls();

        // cleanup
        Device_Destroy(h);
    }

    /* Tests_SRS_DEVICE_01_004: [DeviceCreate shall pass to DataPublisher_create the includePropertyPath argument.] */
    TEST_FUNCTION(Device_Create_passes_includePropertyPath_false_to_DataPublisher_Create)
    {
        // arrange
        CDeviceMocks deviceMocks;
        DEVICE_HANDLE h = NULL;

        STRICT_EXPECTED_CALL(deviceMocks, DataPublisher_Create(irrelevantModel, false));
        STRICT_EXPECTED_CALL(deviceMocks, CommandDecoder_Create(irrelevantModel, NULL, NULL))
            .IgnoreArgument(2).IgnoreArgument(3);

        // act
        DEVICE_RESULT res = Device_Create(irrelevantModel, DeviceActionCallback, TEST_CALLBACK_CONTEXT, false, &h);

        // assert
        ASSERT_ARE_EQUAL(DEVICE_RESULT, DEVICE_OK, res);
        ASSERT_IS_NOT_NULL(h);
        deviceMocks.AssertActualAndExpectedCalls();

        // cleanup
        Device_Destroy(h);
    }

    /* Tests_SRS_DEVICE_01_019: [If creating the DataPublisher instance fails, Device_Create shall return DEVICE_DATA_PUBLISHER_FAILED.] */
    TEST_FUNCTION(When_DataPublisher_Create_Fails_Then_Device_Create_Fails)
    {
        // arrange
        CNiceCallComparer<CDeviceMocks> deviceMocks;
        DEVICE_HANDLE h = NULL;

        STRICT_EXPECTED_CALL(deviceMocks, DataPublisher_Create(irrelevantModel, true))
            .SetReturn((DATA_PUBLISHER_HANDLE)NULL);

        // act
        DEVICE_RESULT res = Device_Create(irrelevantModel, DeviceActionCallback, TEST_CALLBACK_CONTEXT, true, &h);

        // assert
        ASSERT_ARE_EQUAL(DEVICE_RESULT, DEVICE_DATA_PUBLISHER_FAILED, res);
        ASSERT_IS_NULL(h);
    }


    /* Tests_SRS_DEVICE_01_003: [If CommandDecoder_Create fails, Device_Create shall return DEVICE_COMMAND_DECODER_FAILED.] */
    TEST_FUNCTION(When_CommandDecoder_Create_Fails_Device_Create_Fails)
    {
        // arrange
        CDeviceMocks deviceMocks;
        DEVICE_HANDLE h = NULL;

        STRICT_EXPECTED_CALL(deviceMocks, DataPublisher_Create(irrelevantModel, true));
        STRICT_EXPECTED_CALL(deviceMocks, CommandDecoder_Create(irrelevantModel, NULL, NULL))
            .IgnoreArgument(2).IgnoreArgument(3).SetReturn((COMMAND_DECODER_HANDLE)NULL);

        // act
        DEVICE_RESULT res = Device_Create(irrelevantModel, DeviceActionCallback, TEST_CALLBACK_CONTEXT, true, &h);

        // assert
        ASSERT_ARE_EQUAL(DEVICE_RESULT, DEVICE_COMMAND_DECODER_FAILED, res);
        ASSERT_IS_NULL(h);
        deviceMocks.AssertActualAndExpectedCalls();
    }

    /* Device_Destroy */

    /* Tests_SRS_DEVICE_03_007: [Device_Destroy will not do anything if deviceHandle is NULL.] */
    TEST_FUNCTION(Device_Destroy_with_a_NULL_handle_raises_no_exceptions)
    {
        // arrange
        CNiceCallComparer<CDeviceMocks> deviceMocks;

        // act
        Device_Destroy(NULL);

        // assert
        // no explicit assert; the test runner will treat a system null-dereference exception as a failure
    }

    /* Tests_SRS_DEVICE_03_006: [Device_Destroy shall free all resources associated with a device.] */
    TEST_FUNCTION(Device_Destroy_with_a_Valid_handle_frees_all_underlying_modules)
    {
        // arrange
        CDeviceMocks deviceMocks;
        DEVICE_HANDLE h = NULL;
        (void)Device_Create(irrelevantModel, DeviceActionCallback, TEST_CALLBACK_CONTEXT, false, &h);
        deviceMocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(deviceMocks, DataPublisher_Destroy(TEST_DATA_PUBLISHER_HANDLE));
        STRICT_EXPECTED_CALL(deviceMocks, CommandDecoder_Destroy(TEST_COMMAND_DECODER_HANDLE));

        // act
        Device_Destroy(h);

        // assert
        // uMock checks the calls
    }

    /* Device_StartTransaction */

    /* Tests_SRS_DEVICE_01_034: [Device_StartTransaction shall invoke DataPublisher_StartTransaction for the DataPublisher handle associated with the deviceHandle argument.] */
    /* Tests_SRS_DEVICE_01_043: [On success, Device_StartTransaction shall return a non NULL handle.] */
    TEST_FUNCTION(Device_StartTransaction_Calls_DataPublisher_And_Succeeds)
    {
        // arrange
        CDeviceMocks deviceMocks;
        AutoDevice device(CreateDeviceWithName_());
        deviceMocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(deviceMocks, DataPublisher_StartTransaction(TEST_DATA_PUBLISHER_HANDLE));

        // act
        TRANSACTION_HANDLE result = Device_StartTransaction(device.Handle());

        // assert
        ASSERT_IS_NOT_NULL(result);
        deviceMocks.AssertActualAndExpectedCalls();
    }

    /* Tests_SRS_DEVICE_01_035: [If any argument is NULL, Device_StartTransaction shall return NULL.] */
    TEST_FUNCTION(Device_StartTransaction_Called_With_NULL_Handle_Fails)
    {
        // arrange
        CDeviceMocks deviceMocks;

        // act
        TRANSACTION_HANDLE result = Device_StartTransaction(NULL);

        // assert
        ASSERT_IS_NULL(result);
    }

    /* Tests_SRS_DEVICE_01_048: [When DataPublisher_StartTransaction fails, Device_StartTransaction shall return NULL.] */
    TEST_FUNCTION(When_DataPublisher_StartTransaction_Fails_Then_Device_StartTransaction_Fails)
    {
        // arrange
        CDeviceMocks deviceMocks;
        AutoDevice device(CreateDeviceWithName_());
        deviceMocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(deviceMocks, DataPublisher_StartTransaction(TEST_DATA_PUBLISHER_HANDLE))
            .SetReturn((TRANSACTION_HANDLE)NULL);

        // act
        TRANSACTION_HANDLE result = Device_StartTransaction(device.Handle());

        // assert
        ASSERT_IS_NULL(result);
        deviceMocks.AssertActualAndExpectedCalls();
    }

    /* Device_PublishTransacted */

    /* Tests_SRS_DEVICE_01_036: [Device_PublishTransacted shall invoke DataPublisher_PublishTransacted.] */
    /* Tests_SRS_DEVICE_01_044: [On success, Device_PublishTransacted shall return DEVICE_OK.] */
    TEST_FUNCTION(Device_PublishTransacted_Calls_DataPublisher_And_Succeeds)
    {
        // arrange
        CDeviceMocks deviceMocks;
        AGENT_DATA_TYPE ag;
        AutoDevice device(CreateDeviceWithName_());
        TRANSACTION_HANDLE transaction = DataPublisher_StartTransaction(device.Handle());
        deviceMocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(deviceMocks, DataPublisher_PublishTransacted(transaction, "p", &ag));

        // act
        DEVICE_RESULT result = Device_PublishTransacted(transaction, "p", &ag);

        // assert
        ASSERT_ARE_EQUAL(DEVICE_RESULT, DEVICE_OK, result);
        deviceMocks.AssertActualAndExpectedCalls();

        Device_CancelTransaction(device.Handle());
    }

    /* Tests_SRS_DEVICE_01_037: [If any argument is NULL, Device_PublishTransacted shall return DEVICE_INVALID_ARG.] */
    TEST_FUNCTION(Device_PublishTransacted_Called_With_NULL_Handle_Fails)
    {
        // arrange
        CDeviceMocks deviceMocks;
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
        CDeviceMocks deviceMocks;
        AGENT_DATA_TYPE ag;
        AutoDevice device(CreateDeviceWithName_());
        TRANSACTION_HANDLE transaction = DataPublisher_StartTransaction(device.Handle());
        deviceMocks.ResetAllCalls();

        // act
        DEVICE_RESULT result = Device_PublishTransacted(transaction, NULL, &ag);

        // assert
        ASSERT_ARE_EQUAL(DEVICE_RESULT, DEVICE_INVALID_ARG, result);
        deviceMocks.AssertActualAndExpectedCalls();

        Device_CancelTransaction(device.Handle());
    }

    /* Tests_SRS_DEVICE_01_037: [If any argument is NULL, Device_PublishTransacted shall return DEVICE_INVALID_ARG.] */
    TEST_FUNCTION(Device_PublishTransacted_Called_With_NULL_Value_Fails)
    {
        // arrange
        CDeviceMocks deviceMocks;
        AutoDevice device(CreateDeviceWithName_());
        TRANSACTION_HANDLE transaction = DataPublisher_StartTransaction(device.Handle());
        deviceMocks.ResetAllCalls();

        // act
        DEVICE_RESULT result = Device_PublishTransacted(transaction, "p", NULL);

        // assert
        ASSERT_ARE_EQUAL(DEVICE_RESULT, DEVICE_INVALID_ARG, result);
        deviceMocks.AssertActualAndExpectedCalls();

        Device_CancelTransaction(device.Handle());
    }

    /* Tests_SRS_DEVICE_01_049: [When DataPublisher_PublishTransacted fails, Device_PublishTransacted shall return DEVICE_DATA_PUBLISHER_FAILED.] */
    TEST_FUNCTION(When_DataPublisher_PublishTransacted_Fails_Then_Device_PublishTransacted_Fails)
    {
        // arrange
        CDeviceMocks deviceMocks;
        AGENT_DATA_TYPE ag;
        AutoDevice device(CreateDeviceWithName_());
        TRANSACTION_HANDLE transaction = DataPublisher_StartTransaction(device.Handle());
        deviceMocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(deviceMocks, DataPublisher_PublishTransacted(transaction, "p", &ag))
            .SetReturn(DATA_PUBLISHER_ERROR);

        // act
        DEVICE_RESULT result = Device_PublishTransacted(transaction, "p", &ag);

        // assert
        ASSERT_ARE_EQUAL(DEVICE_RESULT, DEVICE_DATA_PUBLISHER_FAILED, result);
        deviceMocks.AssertActualAndExpectedCalls();

        Device_CancelTransaction(device.Handle());
    }

    /* Device_EndTransaction */

    /* Tests_SRS_DEVICE_01_038: [Device_EndTransaction shall invoke DataPublisher_EndTransaction.] */
    /* Tests_SRS_DEVICE_01_045: [On success, Device_EndTransaction shall return DEVICE_OK.] */
    TEST_FUNCTION(Device_EndTransaction_Calls_DataPublisher_And_Succeeds)
    {
        // arrange
        CDeviceMocks deviceMocks;
        unsigned char* destination;
        size_t destinationSize;

        STRICT_EXPECTED_CALL(deviceMocks, DataPublisher_EndTransaction(TEST_TRANSACTION_HANDLE, &destination, &destinationSize));

        // act
        DEVICE_RESULT result = Device_EndTransaction(TEST_TRANSACTION_HANDLE, &destination, &destinationSize);

        // assert
        ASSERT_ARE_EQUAL(DEVICE_RESULT, DEVICE_OK, result);
    }

    /* Tests_SRS_DEVICE_01_039: [If transactionHandle is NULL, Device_EndTransaction shall return DEVICE_INVALID_ARG.]    */
    TEST_FUNCTION(Device_EndTransaction_Called_With_NULL_Handle_Fails)
    {
        // arrange
        CDeviceMocks deviceMocks;

        // act
        DEVICE_RESULT result = Device_EndTransaction(NULL, NULL, NULL);

        // assert
        ASSERT_ARE_EQUAL(DEVICE_RESULT, DEVICE_INVALID_ARG, result);
    }

    /* Tests_SRS_DEVICE_01_050: [When DataPublisher_EndTransaction fails, Device_EndTransaction shall return DEVICE_DATA_PUBLISHER_FAILED.] */
    TEST_FUNCTION(When_DataPublisher_EndTransaction_Fails_Then_Device_EndTransaction_Fails)
    {
        // arrange
        CDeviceMocks deviceMocks;
        AutoDevice device(CreateDeviceWithName_());
        unsigned char* destination;
        size_t destinationSize;
        deviceMocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(deviceMocks, DataPublisher_EndTransaction(TEST_TRANSACTION_HANDLE, &destination, &destinationSize))
            .SetReturn(DATA_PUBLISHER_ERROR);

        // act
        DEVICE_RESULT result = Device_EndTransaction(TEST_TRANSACTION_HANDLE, &destination, &destinationSize);

        // assert
        ASSERT_ARE_EQUAL(DEVICE_RESULT, DEVICE_DATA_PUBLISHER_FAILED, result);
        deviceMocks.AssertActualAndExpectedCalls();
    }

    /* Tests_SRS_DEVICE_01_038: [Device_EndTransaction shall invoke DataPublisher_EndTransaction.] */
    TEST_FUNCTION(Device_EndTransaction_Calls_DataPublisher_passingCallBackAndNullContext_Succeed)
    {
        // arrange
        CDeviceMocks deviceMocks;
        unsigned char* destination;
        size_t destinationSize;

        STRICT_EXPECTED_CALL(deviceMocks, DataPublisher_EndTransaction(TEST_TRANSACTION_HANDLE, &destination, &destinationSize));

        // act
        DEVICE_RESULT result = Device_EndTransaction(TEST_TRANSACTION_HANDLE, &destination, &destinationSize);

        // assert
        ASSERT_ARE_EQUAL(DEVICE_RESULT, DEVICE_OK, result);
    }

    /* Tests_SRS_DEVICE_01_038: [Device_EndTransaction shall invoke DataPublisher_EndTransaction.] */
    TEST_FUNCTION(Device_EndTransaction_Calls_DataPublisher_passingCallBackAndAndContext_Succeed)
    {
        // arrange
        CDeviceMocks deviceMocks;
        unsigned char* destination;
        size_t destinationSize;

        STRICT_EXPECTED_CALL(deviceMocks, DataPublisher_EndTransaction(TEST_TRANSACTION_HANDLE, &destination, &destinationSize));

        // act
        DEVICE_RESULT result = Device_EndTransaction(TEST_TRANSACTION_HANDLE, &destination, &destinationSize);

        // assert
        ASSERT_ARE_EQUAL(DEVICE_RESULT, DEVICE_OK, result);
    }

    /* Device_CancelTransaction */

    /* Tests_SRS_DEVICE_01_040: [Device_CancelTransaction shall invoke DataPublisher_CancelTransaction.] */
    /* Tests_SRS_DEVICE_01_046: [On success, Device_PublishTransacted shall return DEVICE_OK.] */
    TEST_FUNCTION(Device_CancelTransaction_Calls_DataPublisher_And_Succeeds)
    {
        // arrange
        CDeviceMocks deviceMocks;

        STRICT_EXPECTED_CALL(deviceMocks, DataPublisher_CancelTransaction(TEST_TRANSACTION_HANDLE));

        // act
        DEVICE_RESULT result = Device_CancelTransaction(TEST_TRANSACTION_HANDLE);

        // assert
        ASSERT_ARE_EQUAL(DEVICE_RESULT, DEVICE_OK, result);
    }

    /* Tests_SRS_DEVICE_01_041: [If any argument is NULL, Device_CancelTransaction shall return DEVICE_INVALID_ARG.] */
    TEST_FUNCTION(Device_CancelTransaction_Called_With_NULL_Handle_Fails)
    {
        // arrange
        CDeviceMocks deviceMocks;

        // act
        DEVICE_RESULT result = Device_CancelTransaction(NULL);

        // assert
        ASSERT_ARE_EQUAL(DEVICE_RESULT, DEVICE_INVALID_ARG, result);
    }

    /* Tests_SRS_DEVICE_01_051: [When DataPublisher_CancelTransaction fails, Device_CancelTransaction shall return DEVICE_DATA_PUBLISHER_FAILED.] */
    TEST_FUNCTION(When_DataPublisher_CancelTransaction_Fails_Then_Device_CancelTransaction_Fails)
    {
        // arrange
        CDeviceMocks deviceMocks;
        AutoDevice device(CreateDeviceWithName_());
        deviceMocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(deviceMocks, DataPublisher_CancelTransaction(TEST_TRANSACTION_HANDLE))
            .SetReturn(DATA_PUBLISHER_ERROR);

        // act
        DEVICE_RESULT result = Device_CancelTransaction(TEST_TRANSACTION_HANDLE);

        // assert
        ASSERT_ARE_EQUAL(DEVICE_RESULT, DEVICE_DATA_PUBLISHER_FAILED, result);
        deviceMocks.AssertActualAndExpectedCalls();
    }

    /* Action callback */

    /*Tests_SRS_DEVICE_02_011: [If the parameter actionCallbackContent passed the callback is NULL then the callback shall return EXECUTION_COMMAND_ERROR.] */
    TEST_FUNCTION(When_Action_Callback_Is_Invoked_with_NULL_handle_returns_ABANDONED)
    {
        /// arrange
        CDeviceMocks deviceMocks;
        AutoDevice device(CreateDeviceWithName_());
        deviceMocks.ResetAllCalls();

        /// act
        EXECUTE_COMMAND_RESULT result = ActionCallbackCalledByCommandDecoder(NULL, "", "testAction", 0, NULL);

        /// assert
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
        deviceMocks.AssertActualAndExpectedCalls();
    }

    /* Tests_SRS_DEVICE_01_052: [When the action callback passed to CommandDecoder is called, Device shall call the appropriate user callback associated with the device handle.] */
    /* Tests_SRS_DEVICE_01_053: [The action name, argument count and action arguments shall be passed to the user callback.] */
    /* Tests_SRS_DEVICE_01_055: [The value passed in callbackUserContext when creating the device shall be passed to the callback as the value for the callbackUserContext argument.] */
    TEST_FUNCTION(When_Action_Callback_Is_Invoked_The_User_Callback_Is_Invoked)
    {
        // arrange
        CDeviceMocks deviceMocks;
        AutoDevice device(CreateDeviceWithName_());
        deviceMocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(deviceMocks, DeviceActionCallback(device.Handle(), TEST_CALLBACK_CONTEXT, "", "testAction", 0, NULL));

        // act
        EXECUTE_COMMAND_RESULT result = ActionCallbackCalledByCommandDecoder(device.Handle(), "", "testAction", 0, NULL);

        // assert
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_SUCCESS, result);
        deviceMocks.AssertActualAndExpectedCalls();
    }

    /* Tests_SRS_DEVICE_01_052: [When the action callback passed to CommandDecoder is called, Device shall call the appropriate user callback associated with the device handle.] */
    /* Tests_SRS_DEVICE_01_053: [The action name, argument count and action arguments shall be passed to the user callback.] */
    /* Tests_SRS_DEVICE_01_055: [The value passed in callbackUserContext when creating the device shall be passed to the callback as the value for the callbackUserContext argument.] */
    TEST_FUNCTION(When_Action_Callback_Is_Invoked_With_1_Arg_The_User_Callback_Is_Invoked_With_Same_Values)
    {
        // arrange
        CDeviceMocks deviceMocks;
        AGENT_DATA_TYPE ag;
        AutoDevice device(CreateDeviceWithName_());
        deviceMocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(deviceMocks, DeviceActionCallback(device.Handle(), TEST_CALLBACK_CONTEXT, "", "testAction", 1, &ag));

        // act
        EXECUTE_COMMAND_RESULT result = ActionCallbackCalledByCommandDecoder(device.Handle(), "", "testAction", 1, &ag);

        // assert
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_SUCCESS, result);
        deviceMocks.AssertActualAndExpectedCalls();
    }

    /* Tests_SRS_DEVICE_01_054: [If the user callback returns a non-zero value, a non-zero value shall be returned to CommandReader.] */
    TEST_FUNCTION(When_Action_Callback_Is_Invoked_And_User_Callback_Fails_Action_Callback_Fails_Too)
    {
        // arrange
        CDeviceMocks deviceMocks;
        AGENT_DATA_TYPE ag;
        AutoDevice device(CreateDeviceWithName_());
        deviceMocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(deviceMocks, DeviceActionCallback(device.Handle(), TEST_CALLBACK_CONTEXT, "", "testAction", 1, &ag))
            .SetReturn(EXECUTE_COMMAND_FAILED);

        // act
        EXECUTE_COMMAND_RESULT result = ActionCallbackCalledByCommandDecoder(device.Handle(), "", "testAction", 1, &ag);

        // assert
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_FAILED, result);
        deviceMocks.AssertActualAndExpectedCalls();
    }

    /*Tests_SRS_DEVICE_02_012: [If any parameters are NULL, then Device_ExecuteCommand shall return EXECUTE_COMMAND_ERROR.]*/
    TEST_FUNCTION(Device_ExecuteCommand_with_NULL_handle_returns_EXECUTE_COMMAND_ERROR)
    {
        ///arrange
        CDeviceMocks deviceMocks;

        ///act
        EXECUTE_COMMAND_RESULT result = Device_ExecuteCommand(NULL, "some command");

        ///assert
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
        deviceMocks.AssertActualAndExpectedCalls();

        ///cleanup
    }

    /*Tests_SRS_DEVICE_02_012: [If any parameters are NULL, then Device_ExecuteCommand shall return EXECUTE_COMMAND_ERROR.]*/
    TEST_FUNCTION(Device_ExecuteCommand_with_NULL_command_returns_EXECUTE_COMMAND_ERROR)
    {
        ///arrange
        CDeviceMocks deviceMocks;
        DEVICE_HANDLE h;
        Device_Create(irrelevantModel, DeviceActionCallback, TEST_CALLBACK_CONTEXT, false, &h);
        deviceMocks.ResetAllCalls();

        ///act
        EXECUTE_COMMAND_RESULT result = Device_ExecuteCommand(h,NULL);

        ///assert
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
        deviceMocks.AssertActualAndExpectedCalls();

        ///cleanup
        Device_Destroy(h);
    }

    /*Tests_SRS_DEVICE_02_013: [Otherwise, Device_ExecuteCommand shall call CommandDecoder_ExecuteCommand and return what CommandDecoder_ExecuteCommand is returning.]*/
    TEST_FUNCTION(Device_ExecuteCommand_returns_what_CommandDecoder_ExecuteCommand_returns_EXECUTE_COMMAND_SUCCESS)
    {
        ///arrange
        CDeviceMocks deviceMocks;
        DEVICE_HANDLE h;
        Device_Create(irrelevantModel, DeviceActionCallback, TEST_CALLBACK_CONTEXT, false, &h);
        deviceMocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(deviceMocks, CommandDecoder_ExecuteCommand(IGNORED_PTR_ARG, "some command"))
            .IgnoreArgument(1);

        ///act
        EXECUTE_COMMAND_RESULT result = Device_ExecuteCommand(h, "some command");

        ///assert
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_SUCCESS, result);
        deviceMocks.AssertActualAndExpectedCalls();

        ///cleanup
        Device_Destroy(h);
    }

    /*Tests_SRS_DEVICE_02_013: [Otherwise, Device_ExecuteCommand shall call CommandDecoder_ExecuteCommand and return what CommandDecoder_ExecuteCommand is returning.]*/
    TEST_FUNCTION(Device_ExecuteCommand_returns_what_CommandDecoder_ExecuteCommand_returns_EXECUTE_COMMAND_FAILED)
    {
        ///arrange
        CDeviceMocks deviceMocks;
        DEVICE_HANDLE h;
        Device_Create(irrelevantModel, DeviceActionCallback, TEST_CALLBACK_CONTEXT, false, &h);
        deviceMocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(deviceMocks, CommandDecoder_ExecuteCommand(IGNORED_PTR_ARG, "some command"))
            .IgnoreArgument(1)
            .SetReturn(EXECUTE_COMMAND_FAILED);

        ///act
        EXECUTE_COMMAND_RESULT result = Device_ExecuteCommand(h, "some command");

        ///assert
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_FAILED, result);
        deviceMocks.AssertActualAndExpectedCalls();

        ///cleanup
        Device_Destroy(h);
    }

    /*Tests_SRS_DEVICE_02_013: [Otherwise, Device_ExecuteCommand shall call CommandDecoder_ExecuteCommand and return what CommandDecoder_ExecuteCommand is returning.]*/
    TEST_FUNCTION(Device_ExecuteCommand_returns_what_CommandDecoder_ExecuteCommand_returns_EXECUTE_COMMAND_ERROR)
    {
        ///arrange
        CDeviceMocks deviceMocks;
        DEVICE_HANDLE h;
        Device_Create(irrelevantModel, DeviceActionCallback, TEST_CALLBACK_CONTEXT, false, &h);
        deviceMocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(deviceMocks, CommandDecoder_ExecuteCommand(IGNORED_PTR_ARG, "some command"))
            .IgnoreArgument(1)
            .SetReturn(EXECUTE_COMMAND_ERROR);

        ///act
        EXECUTE_COMMAND_RESULT result = Device_ExecuteCommand(h, "some command");

        ///assert
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
        deviceMocks.AssertActualAndExpectedCalls();

        ///cleanup
        Device_Destroy(h);
    }

END_TEST_SUITE(IoTDevice_ut)
