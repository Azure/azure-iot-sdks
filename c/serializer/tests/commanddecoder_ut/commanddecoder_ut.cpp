// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <cstdlib>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "azure_c_shared_utility/lock.h"
#include "testrunnerswitcher.h"
#include "micromock.h"
#include "micromockcharstararenullterminatedstrings.h"
#include "commanddecoder.h"
#include "multitree.h"
#include "schema.h"
#include "agenttypesystem.h"
#include "codefirst.h"
#include "jsondecoder.h"

#define GBALLOC_H

extern "C" int gballoc_init(void);
extern "C" void gballoc_deinit(void);
extern "C" void* gballoc_malloc(size_t size);
extern "C" void* gballoc_calloc(size_t nmemb, size_t size);
extern "C" void* gballoc_realloc(void* ptr, size_t size);
extern "C" void gballoc_free(void* ptr);


namespace BASEIMPLEMENTATION
{
    /*if malloc is defined as gballoc_malloc at this moment, there'd be serious trouble*/
#define Lock(x) (LOCK_OK + gballocState - gballocState) /*compiler warning about constant in if condition*/
#define Unlock(x) (LOCK_OK + gballocState - gballocState)
#define Lock_Init() (LOCK_HANDLE)0x42
#define Lock_Deinit(x) (LOCK_OK + gballocState - gballocState)
#include "gballoc.c"
#undef Lock
#undef Unlock
#undef Lock_Init
#undef Lock_Deinit
};

DEFINE_MICROMOCK_ENUM_TO_STRING(COMMANDDECODER_RESULT, COMMANDDECODER_RESULT_VALUES);
DEFINE_MICROMOCK_ENUM_TO_STRING(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_RESULT_VALUES);


static MICROMOCK_MUTEX_HANDLE g_testByTest;

static const SCHEMA_ACTION_HANDLE SetACStateActionHandle = (SCHEMA_ACTION_HANDLE)0x4242;

static const MULTITREE_HANDLE TEST_COMMAND_ROOT_NODE = (MULTITREE_HANDLE)0x4201;
static const MULTITREE_HANDLE TEST_COMMAND_NAME_NODE = (MULTITREE_HANDLE)0x4202;
static const MULTITREE_HANDLE TEST_COMMAND_ARGS_NODE = (MULTITREE_HANDLE)0x4202;
static const MULTITREE_HANDLE TEST_ARG1_NODE = (MULTITREE_HANDLE)0x4281;
static const MULTITREE_HANDLE TEST_ARG2_NODE = (MULTITREE_HANDLE)0x4282;
static const MULTITREE_HANDLE TEST_NESTED_STRUCT_NODE = (MULTITREE_HANDLE)0x4283;
static const SCHEMA_MODEL_TYPE_HANDLE TEST_MODEL_HANDLE = (SCHEMA_MODEL_TYPE_HANDLE)0x4301;
static const SCHEMA_MODEL_TYPE_HANDLE TEST_CHILD_MODEL_HANDLE = (SCHEMA_MODEL_TYPE_HANDLE)0x4302;
static const SCHEMA_HANDLE TEST_SCHEMA_HANDLE = (SCHEMA_HANDLE)0x4401;
static void* TEST_CALLBACK_CONTEXT_VALUE = (void*)0x4242;

// = { "State", "bool" }
static const char StateActionArgument_Name[] = "State";
static const char StateActionArgument_Type[] = "bool";
static const SCHEMA_ACTION_ARGUMENT_HANDLE StateActionArgument = (SCHEMA_ACTION_ARGUMENT_HANDLE)0x5252;
// = { "OtherArg", "ascii_char_ptr" }
static const char OtherArgActionArgument_Name[] = "OtherArg";
static const char OtherArgActionArgument_Type[] = "ascii_char_ptr";
static const SCHEMA_ACTION_ARGUMENT_HANDLE OtherArgActionArgument = (SCHEMA_ACTION_ARGUMENT_HANDLE)0x5253;

static AGENT_DATA_TYPE StateAgentDataType;
static AGENT_DATA_TYPE LatAgentDataType;
static AGENT_DATA_TYPE LongAgentDataType;
static AGENT_DATA_TYPE OtherArgAgentDataType;

static const char* quotedSetLocationName = "\"SetLocation\"";
static const char* setLocationName = "SetLocation";
static const char* quotedSetACStateName = "\"SetACState\"";
static const char* setACStateName = "SetACState";

#define DEFAULT_SCHEMA_NAME_SPACE "TruckDemo"

static const char OtherArgValue[] = "SomeString";
static const SCHEMA_STRUCT_TYPE_HANDLE TEST_STRUCT_1_HANDLE = (SCHEMA_STRUCT_TYPE_HANDLE)0x4301;
static const SCHEMA_STRUCT_TYPE_HANDLE TEST_STRUCT_2_HANDLE = (SCHEMA_STRUCT_TYPE_HANDLE)0x4302;

static const SCHEMA_ACTION_HANDLE SetLocationActionHandle = (SCHEMA_ACTION_HANDLE)0x4242;

// { "Location", "GeoLocation" }
static const char LocationActionArgument_Name[] = "Location";
static const char LocationActionArgument_Type[] = "GeoLocation";
static const SCHEMA_ACTION_ARGUMENT_HANDLE LocationActionArgument = (SCHEMA_ACTION_ARGUMENT_HANDLE)0x5254;

//  = { "Lat", "double" }
static const SCHEMA_PROPERTY_HANDLE memberProperty1 = (SCHEMA_PROPERTY_HANDLE)0x4401;
//  = { "Long", "double" }
static const SCHEMA_PROPERTY_HANDLE memberProperty2 = (SCHEMA_PROPERTY_HANDLE)0x4402;

//  = { "NestedLocation", "NestedGeoLocation" }
static const SCHEMA_PROPERTY_HANDLE memberNestedComplexTypeProperty = (SCHEMA_PROPERTY_HANDLE)0x4403;

static const MULTITREE_HANDLE TEST_MEMBER1_NODE = (MULTITREE_HANDLE)0x4401;
static const MULTITREE_HANDLE TEST_MEMBER2_NODE = (MULTITREE_HANDLE)0x4402;

static char lastMemberNames[100][100][100];
static size_t nCall = 0;

#define TEST_COMMAND \
    "{ "  \
    "\"Name\" : \"hagauaga\"," \
    "\"Parameters\":" \
    "{ "  \
    "  \"param1\" : \"42\"," \
    "} "  \
    " }"

static char TestCommand[] = TEST_COMMAND;

static const ACTION_CALLBACK_FUNC TEST_CALLBACK_PTR = (ACTION_CALLBACK_FUNC)0x4343;
static const char* TEST_IOTHUB_MESSAGE_HANDLE2 = TEST_COMMAND;
static const MULTITREE_HANDLE TEST_COMMANDS_ROOT_NODE = (MULTITREE_HANDLE)0x4201;
static const COMMAND_DECODER_HANDLE TEST_COMMAND_DECODER_HANDLE = (COMMAND_DECODER_HANDLE)0x4246;

static bool isIoTHubMessage_GetData_writing_to_outputs = true;

static size_t currentmalloc_call;
static size_t whenShallmalloc_fail;

static size_t currentrealloc_call;
static size_t whenShallrealloc_fail;


TYPED_MOCK_CLASS(CCommandDecoderMocks, CGlobalMock)
{
public:
    /* MultiTree mocks */
    MOCK_STATIC_METHOD_3(, MULTITREE_RESULT, MultiTree_GetChildByName, MULTITREE_HANDLE, treeHandle, const char*, childName, MULTITREE_HANDLE*, childHandle)
    MOCK_METHOD_END(MULTITREE_RESULT, MULTITREE_OK)
    MOCK_STATIC_METHOD_2(, MULTITREE_RESULT, MultiTree_GetValue, MULTITREE_HANDLE, treeHandle, const void**, destination)
    MOCK_METHOD_END(MULTITREE_RESULT, MULTITREE_OK)
    MOCK_STATIC_METHOD_2(, MULTITREE_RESULT, MultiTree_GetChildCount, MULTITREE_HANDLE, treeHandle, size_t*, count)
    MOCK_METHOD_END(MULTITREE_RESULT, MULTITREE_OK)
    MOCK_STATIC_METHOD_3(, MULTITREE_RESULT, MultiTree_GetChild, MULTITREE_HANDLE, treeHandle, size_t, index, MULTITREE_HANDLE*, childHandle)
    MOCK_METHOD_END(MULTITREE_RESULT, MULTITREE_OK)
    MOCK_STATIC_METHOD_1(, void, MultiTree_Destroy, MULTITREE_HANDLE, treeHandle)
    MOCK_VOID_METHOD_END()

    /* Action callback mock */
    MOCK_STATIC_METHOD_5(, EXECUTE_COMMAND_RESULT, ActionCallbackMock, void*, actionCallbackContext, const char*, relativeActionPath, const char*, actionName, size_t, parameterCount, const AGENT_DATA_TYPE*, parameterValues)
    MOCK_METHOD_END(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_SUCCESS)

    /* Schema mocks */
    MOCK_STATIC_METHOD_2(, SCHEMA_ACTION_HANDLE, Schema_GetModelActionByName, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle, const char*, actionName)
    MOCK_METHOD_END(SCHEMA_ACTION_HANDLE, (SCHEMA_ACTION_HANDLE)NULL)
    MOCK_STATIC_METHOD_2(, SCHEMA_RESULT, Schema_GetModelActionArgumentCount, SCHEMA_ACTION_HANDLE, actionHandle, size_t*, argumentCount)
    MOCK_METHOD_END(SCHEMA_RESULT, SCHEMA_OK)
    MOCK_STATIC_METHOD_2(, SCHEMA_ACTION_ARGUMENT_HANDLE, Schema_GetModelActionArgumentByIndex, SCHEMA_ACTION_HANDLE, modelActionHandle, size_t, argumentIndex)
    MOCK_METHOD_END(SCHEMA_ACTION_ARGUMENT_HANDLE, NULL)
    MOCK_STATIC_METHOD_1(, const char*, Schema_GetActionArgumentName, SCHEMA_ACTION_ARGUMENT_HANDLE, actionArgumentHandle)
    MOCK_METHOD_END(const char*, NULL)
    MOCK_STATIC_METHOD_1(, const char*, Schema_GetActionArgumentType, SCHEMA_ACTION_ARGUMENT_HANDLE, actionArgumentHandle)
    MOCK_METHOD_END(const char*, NULL)
    MOCK_STATIC_METHOD_2(, SCHEMA_STRUCT_TYPE_HANDLE, Schema_GetStructTypeByName, SCHEMA_HANDLE, schemaHandle, const char*, name)
    MOCK_METHOD_END(SCHEMA_STRUCT_TYPE_HANDLE, (SCHEMA_STRUCT_TYPE_HANDLE)NULL)
    MOCK_STATIC_METHOD_2(, SCHEMA_RESULT, Schema_GetStructTypePropertyCount, SCHEMA_STRUCT_TYPE_HANDLE, structTypeHandle, size_t*, propertyCount)
    MOCK_METHOD_END(SCHEMA_RESULT, SCHEMA_OK)
    MOCK_STATIC_METHOD_2(, SCHEMA_PROPERTY_HANDLE, Schema_GetStructTypePropertyByIndex, SCHEMA_STRUCT_TYPE_HANDLE, structTypeHandle, size_t, index)
    MOCK_METHOD_END(SCHEMA_PROPERTY_HANDLE, (SCHEMA_PROPERTY_HANDLE)NULL)
    MOCK_STATIC_METHOD_1(, SCHEMA_HANDLE, Schema_GetSchemaForModelType, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle)
    MOCK_METHOD_END(SCHEMA_HANDLE, TEST_SCHEMA_HANDLE)
    MOCK_STATIC_METHOD_1(, const char*, Schema_GetPropertyName, SCHEMA_PROPERTY_HANDLE, propertyHandle)
    MOCK_METHOD_END(const char*, NULL)
    MOCK_STATIC_METHOD_1(, const char*, Schema_GetPropertyType, SCHEMA_PROPERTY_HANDLE, propertyHandle)
    MOCK_METHOD_END(const char*, NULL)
    MOCK_STATIC_METHOD_1(, const char*, Schema_GetSchemaNamespace, SCHEMA_HANDLE, schemaHandle)
    MOCK_METHOD_END(const char*, DEFAULT_SCHEMA_NAME_SPACE)
    MOCK_STATIC_METHOD_2(, SCHEMA_MODEL_TYPE_HANDLE, Schema_GetModelModelByName, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle, const char*, propertyName)
    MOCK_METHOD_END(SCHEMA_MODEL_TYPE_HANDLE, TEST_CHILD_MODEL_HANDLE)

    /* AgentDataType mocks */
    MOCK_STATIC_METHOD_3(, AGENT_DATA_TYPES_RESULT, CreateAgentDataType_From_String, const char*, source, AGENT_DATA_TYPE_TYPE, type, AGENT_DATA_TYPE*, agentData)
    MOCK_METHOD_END(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK)
    MOCK_STATIC_METHOD_5(, AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_Members, AGENT_DATA_TYPE*, agentData, const char*, typeName, size_t, nMembers, const char* const *, memberNames, const AGENT_DATA_TYPE*, memberValues)
    {
        for (size_t i = 0; i < nMembers; i++)
        {
            strcpy(lastMemberNames[nCall][i], memberNames[i]);
        }
        nCall++;
    }
    MOCK_METHOD_END(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK)
        MOCK_STATIC_METHOD_1(, void, Destroy_AGENT_DATA_TYPE, AGENT_DATA_TYPE*, agentData)
        MOCK_VOID_METHOD_END()
        MOCK_STATIC_METHOD_1(, AGENT_DATA_TYPE_TYPE, CodeFirst_GetPrimitiveType, const char*, typeName)
        MOCK_METHOD_END(AGENT_DATA_TYPE_TYPE, EDM_NO_TYPE)

        ///* IoT Hub mocks */
    /*MOCK_STATIC_METHOD_3(, IOTHUB_MESSAGE_RESULT, IoTHubMessage_GetData, IOTHUB_MESSAGE_HANDLE, iotHubMessageHandle, const unsigned char**, buffer, size_t*, size)
        
    if (isIoTHubMessage_GetData_writing_to_outputs)
    {
        *buffer = (const unsigned char*)TEST_COMMAND;
        *size = strlen(TEST_COMMAND);
    }
        
    MOCK_METHOD_END(IOTHUB_MESSAGE_RESULT, IOTHUB_MESSAGE_OK)*/

    /* JSON Decoder mocks */
    MOCK_STATIC_METHOD_2(, JSON_DECODER_RESULT, JSONDecoder_JSON_To_MultiTree, char*, json, MULTITREE_HANDLE*, multiTreeHandle);
        *multiTreeHandle = TEST_COMMANDS_ROOT_NODE;
    MOCK_METHOD_END(JSON_DECODER_RESULT, JSON_DECODER_OK)


        MOCK_STATIC_METHOD_1(, void*, gballoc_malloc, size_t, size)
        void* result2;
    currentmalloc_call++;
    if (whenShallmalloc_fail>0)
    {
        if (currentmalloc_call == whenShallmalloc_fail)
        {
            result2 = (STRING_HANDLE)NULL;
        }
        else
        {
            result2 = BASEIMPLEMENTATION::gballoc_malloc(size);
        }
    }
    else
    {
        result2 = BASEIMPLEMENTATION::gballoc_malloc(size);
    }
    MOCK_METHOD_END(void*, result2);

    MOCK_STATIC_METHOD_2(, void*, gballoc_realloc, void*, ptr, size_t, size)
        void* result2;
    currentrealloc_call++;
    if (whenShallrealloc_fail>0)
    {
        if (currentrealloc_call == whenShallrealloc_fail)
        {
            result2 = (STRING_HANDLE)NULL;
        }
        else
        {
            result2 = BASEIMPLEMENTATION::gballoc_realloc(ptr, size);
        }
    }
    else
    {
        result2 = BASEIMPLEMENTATION::gballoc_realloc(ptr, size);
    }
    MOCK_METHOD_END(void*, result2);

    MOCK_STATIC_METHOD_1(, void, gballoc_free, void*, ptr)
        BASEIMPLEMENTATION::gballoc_free(ptr);
    MOCK_VOID_METHOD_END()

};

DECLARE_GLOBAL_MOCK_METHOD_3(CCommandDecoderMocks, , MULTITREE_RESULT, MultiTree_GetChildByName, MULTITREE_HANDLE, treeHandle, const char*, childName, MULTITREE_HANDLE*, childHandle);
DECLARE_GLOBAL_MOCK_METHOD_2(CCommandDecoderMocks, , MULTITREE_RESULT, MultiTree_GetValue, MULTITREE_HANDLE, treeHandle, const void**, destination);
DECLARE_GLOBAL_MOCK_METHOD_2(CCommandDecoderMocks, , MULTITREE_RESULT, MultiTree_GetChildCount, MULTITREE_HANDLE, treeHandle, size_t*, count);
DECLARE_GLOBAL_MOCK_METHOD_3(CCommandDecoderMocks, , MULTITREE_RESULT, MultiTree_GetChild, MULTITREE_HANDLE, treeHandle, size_t, index, MULTITREE_HANDLE*, childHandle);
DECLARE_GLOBAL_MOCK_METHOD_1(CCommandDecoderMocks, , void, MultiTree_Destroy, MULTITREE_HANDLE, treeHandle);

DECLARE_GLOBAL_MOCK_METHOD_5(CCommandDecoderMocks, , EXECUTE_COMMAND_RESULT, ActionCallbackMock, void*, actionCallbackContext, const char*, relativeActionPath, const char*, actionName, size_t, parameterCount, const AGENT_DATA_TYPE*, parameterValues);

DECLARE_GLOBAL_MOCK_METHOD_2(CCommandDecoderMocks, , SCHEMA_ACTION_HANDLE, Schema_GetModelActionByName, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle, const char*, actionName);
DECLARE_GLOBAL_MOCK_METHOD_2(CCommandDecoderMocks, , SCHEMA_RESULT, Schema_GetModelActionArgumentCount, SCHEMA_ACTION_HANDLE, actionHandle, size_t*, argumentCount);
DECLARE_GLOBAL_MOCK_METHOD_2(CCommandDecoderMocks, , SCHEMA_ACTION_ARGUMENT_HANDLE, Schema_GetModelActionArgumentByIndex, SCHEMA_ACTION_HANDLE, modelActionHandle, size_t, argumentIndex);
DECLARE_GLOBAL_MOCK_METHOD_1(CCommandDecoderMocks, , const char*, Schema_GetActionArgumentName, SCHEMA_ACTION_ARGUMENT_HANDLE, actionArgumentHandle);
DECLARE_GLOBAL_MOCK_METHOD_1(CCommandDecoderMocks, , const char*, Schema_GetActionArgumentType, SCHEMA_ACTION_ARGUMENT_HANDLE, actionArgumentHandle);
DECLARE_GLOBAL_MOCK_METHOD_2(CCommandDecoderMocks, , SCHEMA_STRUCT_TYPE_HANDLE, Schema_GetStructTypeByName, SCHEMA_HANDLE, schemaHandle, const char*, name);
DECLARE_GLOBAL_MOCK_METHOD_2(CCommandDecoderMocks, , SCHEMA_RESULT, Schema_GetStructTypePropertyCount, SCHEMA_STRUCT_TYPE_HANDLE, structTypeHandle, size_t*, propertyCount);
DECLARE_GLOBAL_MOCK_METHOD_2(CCommandDecoderMocks, , SCHEMA_PROPERTY_HANDLE, Schema_GetStructTypePropertyByIndex, SCHEMA_STRUCT_TYPE_HANDLE, structTypeHandle, size_t, index);
DECLARE_GLOBAL_MOCK_METHOD_1(CCommandDecoderMocks, , SCHEMA_HANDLE, Schema_GetSchemaForModelType, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle);
DECLARE_GLOBAL_MOCK_METHOD_1(CCommandDecoderMocks, , const char*, Schema_GetPropertyName, SCHEMA_PROPERTY_HANDLE, propertyHandle);
DECLARE_GLOBAL_MOCK_METHOD_1(CCommandDecoderMocks, , const char*, Schema_GetPropertyType, SCHEMA_PROPERTY_HANDLE, propertyHandle);
DECLARE_GLOBAL_MOCK_METHOD_1(CCommandDecoderMocks, , const char*, Schema_GetSchemaNamespace, SCHEMA_HANDLE, schemaHandle);
DECLARE_GLOBAL_MOCK_METHOD_2(CCommandDecoderMocks, , SCHEMA_MODEL_TYPE_HANDLE, Schema_GetModelModelByName, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle, const char*, propertyName);

DECLARE_GLOBAL_MOCK_METHOD_3(CCommandDecoderMocks, , AGENT_DATA_TYPES_RESULT, CreateAgentDataType_From_String, const char*, source, AGENT_DATA_TYPE_TYPE, type, AGENT_DATA_TYPE*, agentData);
DECLARE_GLOBAL_MOCK_METHOD_5(CCommandDecoderMocks, , AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_Members, AGENT_DATA_TYPE*, agentData, const char*, typeName, size_t, nMembers, const char* const *, memberNames, const AGENT_DATA_TYPE*, memberValues);
DECLARE_GLOBAL_MOCK_METHOD_1(CCommandDecoderMocks, , void, Destroy_AGENT_DATA_TYPE, AGENT_DATA_TYPE*, agentData);
DECLARE_GLOBAL_MOCK_METHOD_1(CCommandDecoderMocks, , AGENT_DATA_TYPE_TYPE, CodeFirst_GetPrimitiveType, const char*, typeName);

//
DECLARE_GLOBAL_MOCK_METHOD_2(CCommandDecoderMocks, , JSON_DECODER_RESULT, JSONDecoder_JSON_To_MultiTree, char*, json, MULTITREE_HANDLE*, multiTreeHandle);


DECLARE_GLOBAL_MOCK_METHOD_1(CCommandDecoderMocks, , void*, gballoc_malloc, size_t, size);
DECLARE_GLOBAL_MOCK_METHOD_2(CCommandDecoderMocks, , void*, gballoc_realloc, void*, ptr, size_t, size);
DECLARE_GLOBAL_MOCK_METHOD_1(CCommandDecoderMocks, , void, gballoc_free, void*, ptr);

/* Requirements tested by the virtue of invoking the public API */
/* Tests_SRS_COMMAND_DECODER_99_001:[ The CommandDecoder module shall expose the following API ... ] */

void SetupCommand(CCommandDecoderMocks* mocks, const char* quotedActionName, const char* actionName)
{
    (void)mocks;
    STRICT_EXPECTED_CALL((*mocks), JSONDecoder_JSON_To_MultiTree(TestCommand, IGNORED_PTR_ARG)).IgnoreArgument(2);
    STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildByName(TEST_COMMAND_ROOT_NODE, "Name", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TEST_COMMAND_NAME_NODE, sizeof(TEST_COMMAND_NAME_NODE));
    STRICT_EXPECTED_CALL((*mocks), MultiTree_GetValue(TEST_COMMAND_NAME_NODE, IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(2, &quotedActionName, sizeof(quotedActionName));
    STRICT_EXPECTED_CALL((*mocks), gballoc_malloc(IGNORED_NUM_ARG)) /*quotedActionName*/
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL((*mocks), gballoc_free(IGNORED_PTR_ARG))
        .IgnoreArgument(1);
    STRICT_EXPECTED_CALL((*mocks), Schema_GetSchemaForModelType(TEST_MODEL_HANDLE));
    STRICT_EXPECTED_CALL((*mocks), MultiTree_GetChildByName(TEST_COMMAND_ROOT_NODE, "Parameters", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TEST_COMMAND_ARGS_NODE, sizeof(TEST_COMMAND_ARGS_NODE));
    STRICT_EXPECTED_CALL((*mocks), Schema_GetModelActionByName(TEST_MODEL_HANDLE, actionName))
        .SetReturn(SetACStateActionHandle);
}

void SetupArgumentCalls(CCommandDecoderMocks* mocks, SCHEMA_ACTION_HANDLE actionHandle, size_t index, SCHEMA_ACTION_ARGUMENT_HANDLE argHandle, const char* argName, const char* argType)
{
    (void)mocks;
    STRICT_EXPECTED_CALL((*mocks), Schema_GetModelActionArgumentByIndex(actionHandle, index))
        .SetReturn(argHandle);
    STRICT_EXPECTED_CALL((*mocks), Schema_GetActionArgumentName(argHandle))
        .SetReturn(argName);
    STRICT_EXPECTED_CALL((*mocks), Schema_GetActionArgumentType(argHandle))
        .SetReturn(argType);
}

static MICROMOCK_GLOBAL_SEMAPHORE_HANDLE g_dllByDll;

BEGIN_TEST_SUITE(CommandDecoder_ut)

    TEST_SUITE_INITIALIZE(TestClassInitialize)
    {
        TEST_INITIALIZE_MEMORY_DEBUG(g_dllByDll);

        g_testByTest = MicroMockCreateMutex();
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

        nCall = 0;

        StateAgentDataType.type = EDM_BOOLEAN_TYPE;
        StateAgentDataType.value.edmBoolean.value = EDM_TRUE;

        StateAgentDataType.type = EDM_STRING_TYPE;
        StateAgentDataType.value.edmString.length = COUNT_OF(OtherArgValue);
        StateAgentDataType.value.edmString.chars = (char*)OtherArgValue;

        isIoTHubMessage_GetData_writing_to_outputs = true;

        currentmalloc_call = 0;
        whenShallmalloc_fail = 0;

        currentrealloc_call = 0;
        whenShallrealloc_fail = 0;
    }

    TEST_FUNCTION_CLEANUP(TestMethodCleanup)
    {
        if (!MicroMockReleaseMutex(g_testByTest))
        {
            ASSERT_FAIL("failure in test framework at ReleaseMutex");
        }
    }

    /* CommandDecoder_Create */

    /* Tests_SRS_COMMAND_DECODER_99_019:[ For all exposed APIs argument validity checks shall precede other checks.] */
    /* Tests_SRS_COMMAND_DECODER_01_003: [If any of the arguments modelHandle is NULL, CommandDecoder_Create shall return NULL.]*/
    TEST_FUNCTION(CommandDecoder_Create_With_NULL_Model_Handle_Fails)
    {
        // arrange
        CCommandDecoderMocks mocks;

        // act
        COMMAND_DECODER_HANDLE result = CommandDecoder_Create(NULL, ActionCallbackMock, TEST_CALLBACK_CONTEXT_VALUE);

        // assert
        ASSERT_IS_NULL(result);
    }

    /* Tests_SRS_COMMAND_DECODER_01_001: [CommandDecoder_Create shall create a new instance of a CommandDecoder.] */
    /* Tests_CommandDecoder_Create shall give its receive callback to the transport by calling IoTHubClient_SetMessageCallback when transportType is TRANSPORT_THREADED.*/
    /* Tests_SRS_COMMAND_DECODER_02_017: [If the receive callback is different than NULL, then CommandDecoder_Create shall call Transport_StartReceive.] */
    TEST_FUNCTION(CommandDecoder_Create_With_Non_NULL_Callback_Argument_Succeeds)
    {
        // arrange
        CCommandDecoderMocks mocks;

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);

        // act
        COMMAND_DECODER_HANDLE result = CommandDecoder_Create(TEST_MODEL_HANDLE, ActionCallbackMock, TEST_CALLBACK_CONTEXT_VALUE);

        // assert
        ASSERT_IS_NOT_NULL(result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CommandDecoder_Destroy(result);
    }

    /* Tests_SRS_COMMAND_DECODER_01_001: [CommandDecoder_Create shall create a new instance of a CommandDecoder.] */
    /* Tests_CommandDecoder_Create shall give its receive callback to the transport by calling IoTHubClient_SetMessageCallback when transportType is TRANSPORT_THREADED.*/
    /* Tests_SRS_COMMAND_DECODER_02_017: [If the receive callback is different than NULL, then CommandDecoder_Create shall call Transport_StartReceive.] */
    TEST_FUNCTION(CommandDecoder_Create_With_NULL_Callback_does_not_call_StartReceive)
    {
        // arrange
        CCommandDecoderMocks mocks;

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);

        // act
        COMMAND_DECODER_HANDLE result = CommandDecoder_Create(TEST_MODEL_HANDLE, NULL, TEST_CALLBACK_CONTEXT_VALUE);

        // assert
        ASSERT_IS_NOT_NULL(result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CommandDecoder_Destroy(result);
    }

    /*Tests_SRS_COMMAND_DECODER_02_018: [CommandDecoder_Create shall give its receive callback to the transport by calling IoTHubClient_LL_SetMessageCallback when transportType is TRANSPORT_LL.]*/
    TEST_FUNCTION(CommandDecoder_Create_TRANSPORT_TYPE_LL_succeeds)
    {
        // arrange
        CCommandDecoderMocks mocks;

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);

        // act
        COMMAND_DECODER_HANDLE result = CommandDecoder_Create(TEST_MODEL_HANDLE, NULL, TEST_CALLBACK_CONTEXT_VALUE);

        // assert
        ASSERT_IS_NOT_NULL(result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CommandDecoder_Destroy(result);
    }

    /* Tests_SRS_COMMAND_DECODER_01_004: [If any error is encountered during CommandDecoder_Create CommandDecoder_Create shall return NULL.] */
    TEST_FUNCTION(When_SetReceiveCallback_Fails_Then_gballoc_Fails)
    {
        // arrange
        CCommandDecoderMocks mocks;

        whenShallmalloc_fail = currentmalloc_call + 1;
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);

        // act
        COMMAND_DECODER_HANDLE result = CommandDecoder_Create(TEST_MODEL_HANDLE, ActionCallbackMock, TEST_CALLBACK_CONTEXT_VALUE);

        // assert
        ASSERT_IS_NULL(result);
    }

    /* CommandDecoder_Destroy */

    /* Tests_SRS_COMMAND_DECODER_01_007: [If CommandDecoder_Destroy is called with a NULL handle, CommandDecoder_Destroy shall do nothing.] */
    TEST_FUNCTION(CommandDecoder_Destroy_With_NULL_Handle_Does_Nothing)
    {
        // arrange
        CCommandDecoderMocks mocks;

        // act
        CommandDecoder_Destroy(NULL);

        // assert
        // no explicit asserts
    }

    /* Tests_SRS_COMMAND_DECODER_07_015: [CommandDecoder_Destroy shall remove the running thread by calling Transport_StopReceive.] */
    /* Tests_SRS_COMMAND_DECODER_01_005: [CommandDecoder_Destroy shall free all resources associated with the commandDecoderHandle instance.] */
    TEST_FUNCTION(CommandDecoder_Destroy_succeeds)
    {
        CCommandDecoderMocks mocks;

        COMMAND_DECODER_HANDLE commandDecoderHandle = CommandDecoder_Create(TEST_MODEL_HANDLE, ActionCallbackMock, TEST_CALLBACK_CONTEXT_VALUE);
        mocks.ResetAllCalls();

        // arrange

        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        // act
        CommandDecoder_Destroy(commandDecoderHandle);

        // assert
        // no explicit asserts
    }

    /* Tests_SRS_COMMAND_DECODER_01_010: [If either parameter is NULL, the processing shall stop and the command shall not be dispatched and it shall return EXECUTE_COMMAND_ERROR.] */
    TEST_FUNCTION(CommandDecoder_ExecuteCommand_with_NULL_messageHandle_fails)
    {
        /// arrange
        CCommandDecoderMocks mocks;
        COMMAND_DECODER_HANDLE commandDecoderHandle = CommandDecoder_Create(TEST_MODEL_HANDLE, ActionCallbackMock, TEST_CALLBACK_CONTEXT_VALUE);
        mocks.ResetAllCalls();

        /// act
        auto result = CommandDecoder_ExecuteCommand(NULL, "some command");

        // assert
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        /// cleanup
        CommandDecoder_Destroy(commandDecoderHandle);
    }

    /* Tests_SRS_COMMAND_DECODER_01_010: [If either parameter is NULL, the processing shall stop and the command shall not be dispatched and it shall return EXECUTE_COMMAND_ERROR.] */
    TEST_FUNCTION(CommandDecoder_ExecuteCommand_with_NULL_context_fails)
    {
        /// arrange
        CCommandDecoderMocks mocks;
        COMMAND_DECODER_HANDLE commandDecoderHandle = CommandDecoder_Create(TEST_MODEL_HANDLE, ActionCallbackMock, TEST_CALLBACK_CONTEXT_VALUE);
        mocks.ResetAllCalls();

        /// act
        auto result = CommandDecoder_ExecuteCommand(commandDecoderHandle, NULL);

        // assert
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        /// cleanup
        CommandDecoder_Destroy(commandDecoderHandle);
    }

    /*Tests_SRS_COMMAND_DECODER_01_011: [If the size of the command is 0 then the processing shall stop and the command shall not be dispatched and it shall return EXECUTE_COMMAND_ERROR.]*/
    TEST_FUNCTION(CommandDecoder_ExecuteCommand_with_zeroSize_message_fails)
    {
        /// arrange
        CCommandDecoderMocks mocks;
        COMMAND_DECODER_HANDLE commandDecoderHandle = CommandDecoder_Create(TEST_MODEL_HANDLE, ActionCallbackMock, TEST_CALLBACK_CONTEXT_VALUE);
        mocks.ResetAllCalls();

        /// act
        auto result = CommandDecoder_ExecuteCommand(commandDecoderHandle, "");

        // assert
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        /// cleanup
        CommandDecoder_Destroy(commandDecoderHandle);
    }

    /*Tests_SRS_COMMAND_DECODER_01_013: [If parsing the JSON to a multi tree fails, the processing shall stop and the command shall not be dispatched and it shall return EXECUTE_COMMAND_ERROR.]*/
    TEST_FUNCTION(When_copying_the_command_content_fails_returns_EXECUTE_COMMAND_ERROR)
    {
        // arrange
        CCommandDecoderMocks mocks;
        COMMAND_DECODER_HANDLE commandDecoderHandle = CommandDecoder_Create(TEST_MODEL_HANDLE, ActionCallbackMock, TEST_CALLBACK_CONTEXT_VALUE);
        mocks.ResetAllCalls();

        whenShallmalloc_fail = currentmalloc_call + 1;
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_COMMAND) + 1)); /*this creates a copy of the command that is given to JSON decoder*/

        // act
        auto result = CommandDecoder_ExecuteCommand(commandDecoderHandle, TEST_COMMAND);

        // assert
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CommandDecoder_Destroy(commandDecoderHandle);
    }

    /* Tests_SRS_COMMAND_DECODER_01_013: [If parsing the JSON to a multi tree fails, the processing shall stop and the command shall not be dispatched and it shall return EXECUTE_COMMAND_ERROR.]*/
    TEST_FUNCTION(When_Parsing_The_JSON_To_MultiTree_Fails_Then_No_Command_Is_Dispatched)
    {
        // arrange
        CCommandDecoderMocks mocks;
        COMMAND_DECODER_HANDLE commandDecoderHandle = CommandDecoder_Create(TEST_MODEL_HANDLE, ActionCallbackMock, TEST_CALLBACK_CONTEXT_VALUE);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_COMMAND) + 1)); /*this creates a copy of the command that is given to JSON decoder*/
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*free-ing the copy of the buffer*/
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, JSONDecoder_JSON_To_MultiTree(TestCommand, IGNORED_PTR_ARG)).IgnoreArgument(2)
            .SetReturn(JSON_DECODER_INVALID_ARG);

        // act
        auto result = CommandDecoder_ExecuteCommand(commandDecoderHandle, TEST_COMMAND);

        // assert
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CommandDecoder_Destroy(commandDecoderHandle);
    }



    /* Tests_SRS_COMMAND_DECODER_01_015: [If any MultiTree API call fails then the processing shall stop and the command shall not be dispatched and it shall return EXECUTE_COMMAND_ERROR.] */
    TEST_FUNCTION(When_Getting_The_Schema_For_The_Model_Fails_Then_No_Command_Is_Dispatched)
    {
        // arrange
        CCommandDecoderMocks mocks;
        COMMAND_DECODER_HANDLE commandDecoderHandle = CommandDecoder_Create(TEST_MODEL_HANDLE, ActionCallbackMock, TEST_CALLBACK_CONTEXT_VALUE);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_COMMAND) + 1)); /*this creates a copy of the command that is given to JSON decoder*/
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*free-ing the copy of the buffer*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, JSONDecoder_JSON_To_MultiTree(TestCommand, IGNORED_PTR_ARG)).IgnoreArgument(2);
        STRICT_EXPECTED_CALL(mocks, Schema_GetSchemaForModelType(TEST_MODEL_HANDLE))
            .SetReturn((SCHEMA_HANDLE)NULL);
        STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TEST_COMMANDS_ROOT_NODE));

        // act
        auto result = CommandDecoder_ExecuteCommand(commandDecoderHandle, TEST_COMMAND);

        // assert
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CommandDecoder_Destroy(commandDecoderHandle);
    }

    /* Tests_SRS_COMMAND_DECODER_01_015: [If any MultiTree API call fails then the processing shall stop and the command shall not be dispatched and it shall return EXECUTE_COMMAND_ERROR.] */
    TEST_FUNCTION(When_Getting_The_ActionName_Node_Fails_Then_No_Command_Is_Dispatched)
    {
        // arrange
        CCommandDecoderMocks mocks;
        COMMAND_DECODER_HANDLE commandDecoderHandle = CommandDecoder_Create(TEST_MODEL_HANDLE, ActionCallbackMock, TEST_CALLBACK_CONTEXT_VALUE);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_COMMAND) + 1)); /*this creates a copy of the command that is given to JSON decoder*/
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*free-ing the copy of the buffer*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, JSONDecoder_JSON_To_MultiTree(TestCommand, IGNORED_PTR_ARG)).IgnoreArgument(2);
        STRICT_EXPECTED_CALL(mocks, Schema_GetSchemaForModelType(TEST_MODEL_HANDLE));
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetChildByName(TEST_COMMAND_ROOT_NODE, "Name", IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(3, &TEST_COMMAND_NAME_NODE, sizeof(TEST_COMMAND_NAME_NODE))
            .SetReturn(MULTITREE_INVALID_ARG);
        STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TEST_COMMANDS_ROOT_NODE));

        // act
        auto result = CommandDecoder_ExecuteCommand(commandDecoderHandle, TEST_COMMAND);

        // assert
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CommandDecoder_Destroy(commandDecoderHandle);
    }

    /* Tests_SRS_COMMAND_DECODER_01_015: [If any MultiTree API call fails then the processing shall stop and the command shall not be dispatched and it shall return EXECUTE_COMMAND_ERROR.] */
    TEST_FUNCTION(When_Getting_The_ActionName_Fails_Then_No_Command_Is_Dispatched)
    {
        // arrange
        CCommandDecoderMocks mocks;
        COMMAND_DECODER_HANDLE commandDecoderHandle = CommandDecoder_Create(TEST_MODEL_HANDLE, ActionCallbackMock, TEST_CALLBACK_CONTEXT_VALUE);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_COMMAND) + 1)); /*this creates a copy of the command that is given to JSON decoder*/
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*free-ing the copy of the buffer*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, JSONDecoder_JSON_To_MultiTree(TestCommand, IGNORED_PTR_ARG)).IgnoreArgument(2);
        STRICT_EXPECTED_CALL(mocks, Schema_GetSchemaForModelType(TEST_MODEL_HANDLE));
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetChildByName(TEST_COMMAND_ROOT_NODE, "Name", IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(3, &TEST_COMMAND_NAME_NODE, sizeof(TEST_COMMAND_NAME_NODE));
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetValue(TEST_COMMAND_NAME_NODE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &quotedSetACStateName, sizeof(quotedSetACStateName))
            .SetReturn(MULTITREE_INVALID_ARG);
        STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TEST_COMMANDS_ROOT_NODE));

        // act
        auto result = CommandDecoder_ExecuteCommand(commandDecoderHandle, TEST_COMMAND);

        // assert
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CommandDecoder_Destroy(commandDecoderHandle);
    }

    /* Tests_SRS_COMMAND_DECODER_01_015: [If any MultiTree API call fails then the processing shall stop and the command shall not be dispatched and it shall return EXECUTE_COMMAND_ERROR.] */
    TEST_FUNCTION(When_Getting_The_Parameters_Node_Fails_Then_No_Command_Is_Dispatched)
    {
        // arrange
        CCommandDecoderMocks mocks;
        COMMAND_DECODER_HANDLE commandDecoderHandle = CommandDecoder_Create(TEST_MODEL_HANDLE, ActionCallbackMock, TEST_CALLBACK_CONTEXT_VALUE);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_COMMAND) + 1)); /*this creates a copy of the command that is given to JSON decoder*/
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*free-ing the copy of the buffer*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, JSONDecoder_JSON_To_MultiTree(TestCommand, IGNORED_PTR_ARG)).IgnoreArgument(2);
        STRICT_EXPECTED_CALL(mocks, Schema_GetSchemaForModelType(TEST_MODEL_HANDLE));
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetChildByName(TEST_COMMAND_ROOT_NODE, "Name", IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(3, &TEST_COMMAND_NAME_NODE, sizeof(TEST_COMMAND_NAME_NODE));
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetValue(TEST_COMMAND_NAME_NODE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &quotedSetACStateName, sizeof(quotedSetACStateName));
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is relativeActionPath*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetChildByName(TEST_COMMAND_ROOT_NODE, "Parameters", IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(3, &TEST_COMMAND_ARGS_NODE, sizeof(TEST_COMMAND_ARGS_NODE))
            .SetReturn(MULTITREE_INVALID_ARG);
        STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TEST_COMMANDS_ROOT_NODE));

        // act
        auto result = CommandDecoder_ExecuteCommand(commandDecoderHandle, TEST_COMMAND);

        // assert
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CommandDecoder_Destroy(commandDecoderHandle);
    }

    /* Tests_SRS_COMMAND_DECODER_01_015: [If any MultiTree API call fails then the processing shall stop and the command shall not be dispatched and it shall return EXECUTE_COMMAND_ERROR.] */
    TEST_FUNCTION(When_Copying_The_relativepath_fails_EXECUTE_COMMAND_ERROR_is_returned)
    {
        // arrange
        CCommandDecoderMocks mocks;
        COMMAND_DECODER_HANDLE commandDecoderHandle = CommandDecoder_Create(TEST_MODEL_HANDLE, ActionCallbackMock, TEST_CALLBACK_CONTEXT_VALUE);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_COMMAND) + 1)); /*this creates a copy of the command that is given to JSON decoder*/
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*free-ing the copy of the buffer*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, JSONDecoder_JSON_To_MultiTree(TestCommand, IGNORED_PTR_ARG)).IgnoreArgument(2);
        STRICT_EXPECTED_CALL(mocks, Schema_GetSchemaForModelType(TEST_MODEL_HANDLE));
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetChildByName(TEST_COMMAND_ROOT_NODE, "Name", IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(3, &TEST_COMMAND_NAME_NODE, sizeof(TEST_COMMAND_NAME_NODE));
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetValue(TEST_COMMAND_NAME_NODE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &quotedSetACStateName, sizeof(quotedSetACStateName));
        whenShallmalloc_fail = currentmalloc_call + 2;
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is relativeActionPath*/
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TEST_COMMANDS_ROOT_NODE));

        // act
        auto result = CommandDecoder_ExecuteCommand(commandDecoderHandle, TEST_COMMAND);

        // assert
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CommandDecoder_Destroy(commandDecoderHandle);
    }

    /* Tests_SRS_COMMAND_DECODER_99_010:[ If any Schema API fails then the command shall not be dispatched and it shall return EXECUTE_COMMAND_ERROR.]*/
    TEST_FUNCTION(When_GetModelActionByName_Fails_Then_No_Command_Is_Dispatched)
    {
        // arrange
        CCommandDecoderMocks mocks;
        COMMAND_DECODER_HANDLE commandDecoderHandle = CommandDecoder_Create(TEST_MODEL_HANDLE, ActionCallbackMock, TEST_CALLBACK_CONTEXT_VALUE);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_COMMAND) + 1)); /*this creates a copy of the command that is given to JSON decoder*/
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*free-ing the copy of the buffer*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, JSONDecoder_JSON_To_MultiTree(TestCommand, IGNORED_PTR_ARG)).IgnoreArgument(2);
        STRICT_EXPECTED_CALL(mocks, Schema_GetSchemaForModelType(TEST_MODEL_HANDLE));
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetChildByName(TEST_COMMAND_ROOT_NODE, "Name", IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(3, &TEST_COMMAND_NAME_NODE, sizeof(TEST_COMMAND_NAME_NODE));
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetValue(TEST_COMMAND_NAME_NODE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &quotedSetACStateName, sizeof(quotedSetACStateName));
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is relativeActionPath*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetChildByName(TEST_COMMAND_ROOT_NODE, "Parameters", IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(3, &TEST_COMMAND_ARGS_NODE, sizeof(TEST_COMMAND_ARGS_NODE));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionByName(TEST_MODEL_HANDLE, setACStateName))
            .SetReturn((SCHEMA_ACTION_HANDLE)NULL);

        STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TEST_COMMANDS_ROOT_NODE));

        // act
        auto result = CommandDecoder_ExecuteCommand(commandDecoderHandle, TEST_COMMAND);

        // assert
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CommandDecoder_Destroy(commandDecoderHandle);
    }

    /* Tests_SRS_COMMAND_DECODER_99_010:[ If any Schema API fails then the command shall not be dispatched and it shall return EXECUTE_COMMAND_ERROR.]*/
    TEST_FUNCTION(When_Getting_The_ArgCount_Fails_Then_No_Command_Is_Dispatched)
    {
        // arrange
        CCommandDecoderMocks mocks;
        COMMAND_DECODER_HANDLE commandDecoderHandle = CommandDecoder_Create(TEST_MODEL_HANDLE, ActionCallbackMock, TEST_CALLBACK_CONTEXT_VALUE);
        mocks.ResetAllCalls();

        const char* quotedActionName = "\"SetACState\"";
        const char* actionName = "SetACState";

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_COMMAND) + 1)); /*this creates a copy of the command that is given to JSON decoder*/
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*free-ing the copy of the buffer*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, JSONDecoder_JSON_To_MultiTree(TestCommand, IGNORED_PTR_ARG)).IgnoreArgument(2);
        STRICT_EXPECTED_CALL(mocks, Schema_GetSchemaForModelType(TEST_MODEL_HANDLE));
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetChildByName(TEST_COMMAND_ROOT_NODE, "Name", IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(3, &TEST_COMMAND_NAME_NODE, sizeof(TEST_COMMAND_NAME_NODE));
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetValue(TEST_COMMAND_NAME_NODE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &quotedActionName, sizeof(quotedActionName));
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is relativeActionPath*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1); 
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetChildByName(TEST_COMMAND_ROOT_NODE, "Parameters", IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(3, &TEST_COMMAND_ARGS_NODE, sizeof(TEST_COMMAND_ARGS_NODE));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionByName(TEST_MODEL_HANDLE, actionName))
            .SetReturn(SetACStateActionHandle);
        size_t argCount = 0;
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionArgumentCount(SetACStateActionHandle, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &argCount, sizeof(argCount))
            .SetReturn(SCHEMA_ERROR);

        STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TEST_COMMANDS_ROOT_NODE));

        // act

        auto result = CommandDecoder_ExecuteCommand(commandDecoderHandle, TEST_COMMAND);

        // assert
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CommandDecoder_Destroy(commandDecoderHandle);
    }

    TEST_FUNCTION(When_The_actionname_Contains_Only_One_Quote_No_Command_Is_Dispatched)
    {
        // arrange
        CCommandDecoderMocks mocks;
        COMMAND_DECODER_HANDLE commandDecoderHandle = CommandDecoder_Create(TEST_MODEL_HANDLE, ActionCallbackMock, TEST_CALLBACK_CONTEXT_VALUE);
        mocks.ResetAllCalls();

        const char* quotedActionName = "\"";

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_COMMAND) + 1)); /*this creates a copy of the command that is given to JSON decoder*/
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*free-ing the copy of the buffer*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, JSONDecoder_JSON_To_MultiTree(TestCommand, IGNORED_PTR_ARG)).IgnoreArgument(2);
        STRICT_EXPECTED_CALL(mocks, Schema_GetSchemaForModelType(TEST_MODEL_HANDLE));
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetChildByName(TEST_COMMAND_ROOT_NODE, "Name", IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(3, &TEST_COMMAND_NAME_NODE, sizeof(TEST_COMMAND_NAME_NODE));
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetValue(TEST_COMMAND_NAME_NODE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &quotedActionName, sizeof(quotedActionName));

        STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TEST_COMMANDS_ROOT_NODE));

        // act
        auto result = CommandDecoder_ExecuteCommand(commandDecoderHandle, TEST_COMMAND);

        // assert
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CommandDecoder_Destroy(commandDecoderHandle);
    }

    TEST_FUNCTION(When_The_actionname_Contains_Only_2_Quotes_No_Command_Is_Dispatched)
    {
        // arrange
        CCommandDecoderMocks mocks;
        COMMAND_DECODER_HANDLE commandDecoderHandle = CommandDecoder_Create(TEST_MODEL_HANDLE, ActionCallbackMock, TEST_CALLBACK_CONTEXT_VALUE);
        mocks.ResetAllCalls();

        const char* quotedActionName = "\"\"";

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_COMMAND) + 1)); /*this creates a copy of the command that is given to JSON decoder*/
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*free-ing the copy of the buffer*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, JSONDecoder_JSON_To_MultiTree(TestCommand, IGNORED_PTR_ARG)).IgnoreArgument(2);
        STRICT_EXPECTED_CALL(mocks, Schema_GetSchemaForModelType(TEST_MODEL_HANDLE));
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetChildByName(TEST_COMMAND_ROOT_NODE, "Name", IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(3, &TEST_COMMAND_NAME_NODE, sizeof(TEST_COMMAND_NAME_NODE));
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetValue(TEST_COMMAND_NAME_NODE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &quotedActionName, sizeof(quotedActionName));
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is relativeActionPath*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TEST_COMMANDS_ROOT_NODE));

        // act
        auto result = CommandDecoder_ExecuteCommand(commandDecoderHandle, TEST_COMMAND);

        // assert
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CommandDecoder_Destroy(commandDecoderHandle);
    }

    /*Tests_SRS_COMMAND_DECODER_99_021:[ If the parsing of the command fails for any other reason the command shall not be dispatched.]*/
    TEST_FUNCTION(CommandDecoder_ExecuteCommand_fails_when_array_of_args_fails)
    {
        // arrange
        CCommandDecoderMocks mocks;
        COMMAND_DECODER_HANDLE commandDecoderHandle = CommandDecoder_Create(TEST_MODEL_HANDLE, ActionCallbackMock, TEST_CALLBACK_CONTEXT_VALUE);
        mocks.ResetAllCalls();

        size_t argCount = 1;
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_COMMAND) + 1)); /*this creates a copy of the command that is given to JSON decoder*/
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*free-ing the copy of the buffer*/
            .IgnoreArgument(1);
        SetupCommand(&mocks, quotedSetACStateName, setACStateName);
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionArgumentCount(SetACStateActionHandle, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &argCount, sizeof(argCount));
        whenShallmalloc_fail = currentmalloc_call + 3;
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is allocating memory for the argument array*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TEST_COMMANDS_ROOT_NODE));

        // act
        auto result = CommandDecoder_ExecuteCommand(commandDecoderHandle, TEST_COMMAND);

        // assert
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CommandDecoder_Destroy(commandDecoderHandle);
    }

    /* Tests_SRS_COMMAND_DECODER_99_011:[ CommandDecoder shall attempt to extract the command arguments from the command JSON by looking them up under the node "Parameters".] */
    /* Tests_SRS_COMMAND_DECODER_99_027:[ The value for an argument of primitive type shall be decoded by using the CreateAgentDataType_From_String API.] */
    /* Tests_SRS_COMMAND_DECODER_01_014: [CommandDecoder shall use the MultiTree APIs to extract a specific element from the command JSON.] */
    /* Tests_SRS_COMMAND_DECODER_99_005:[ If an action is decoded successfully then the callback actionCallback shall be called, passing to it the callback action context, decoded name and arguments.] */
    /* Tests_SRS_COMMAND_DECODER_01_008: [Each argument shall be looked up as a field, member of the "Parameters" node.]  */
    TEST_FUNCTION(CommandDecoder_ExecuteCommand_With_Valid_Command_With_1_Arg_Decodes_The_Argument_And_Calls_The_ActionCallback)
    {
        // arrange
        CCommandDecoderMocks mocks;
        COMMAND_DECODER_HANDLE commandDecoderHandle = CommandDecoder_Create(TEST_MODEL_HANDLE, ActionCallbackMock, TEST_CALLBACK_CONTEXT_VALUE);
        mocks.ResetAllCalls();

        size_t argCount = 1;
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_COMMAND) + 1)); /*this creates a copy of the command that is given to JSON decoder*/
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*free-ing the copy of the buffer*/
            .IgnoreArgument(1);
        SetupCommand(&mocks, quotedSetACStateName, setACStateName);
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionArgumentCount(SetACStateActionHandle, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &argCount, sizeof(argCount));
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is allocating memory for the argument array*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        SetupArgumentCalls(&mocks, SetACStateActionHandle, 0, StateActionArgument, StateActionArgument_Name, StateActionArgument_Type);
        const char* stateValue = "true";
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetChildByName(TEST_COMMAND_ARGS_NODE, "State", IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(3, &TEST_ARG1_NODE, sizeof(TEST_ARG1_NODE));
        STRICT_EXPECTED_CALL(mocks, CodeFirst_GetPrimitiveType("bool"))
            .SetReturn(EDM_BOOLEAN_TYPE);
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetValue(TEST_ARG1_NODE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &stateValue, sizeof(stateValue));
        STRICT_EXPECTED_CALL(mocks, CreateAgentDataType_From_String(stateValue, EDM_BOOLEAN_TYPE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(3, &StateAgentDataType, sizeof(StateAgentDataType));

        STRICT_EXPECTED_CALL(mocks, ActionCallbackMock(TEST_CALLBACK_CONTEXT_VALUE, "", "SetACState", 1, IGNORED_PTR_ARG))
            .IgnoreArgument(5);

        EXPECTED_CALL(mocks, Destroy_AGENT_DATA_TYPE(IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TEST_COMMANDS_ROOT_NODE));

        // act
        auto result = CommandDecoder_ExecuteCommand(commandDecoderHandle, TEST_COMMAND);

        // assert
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_SUCCESS, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CommandDecoder_Destroy(commandDecoderHandle);
    }

    /* Tests_SRS_COMMAND_DECODER_99_010:[ If any Schema API fails then the command shall not be dispatched and it shall return EXECUTE_COMMAND_ERROR.]*/
    TEST_FUNCTION(CommandDecoder_When_GetModelActionArgumentByIndex_Fails_ExecuteCommand_Fails)
    {
        // arrange
        CCommandDecoderMocks mocks;
        COMMAND_DECODER_HANDLE commandDecoderHandle = CommandDecoder_Create(TEST_MODEL_HANDLE, ActionCallbackMock, TEST_CALLBACK_CONTEXT_VALUE);
        mocks.ResetAllCalls();

        size_t argCount = 1;
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_COMMAND) + 1)); /*this creates a copy of the command that is given to JSON decoder*/
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*free-ing the copy of the buffer*/
            .IgnoreArgument(1);
        SetupCommand(&mocks, quotedSetACStateName, setACStateName);
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionArgumentCount(SetACStateActionHandle, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &argCount, sizeof(argCount));
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is allocating memory for the argument array*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionArgumentByIndex(SetACStateActionHandle, 0))
            .SetReturn((SCHEMA_ACTION_ARGUMENT_HANDLE)NULL);
        STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TEST_COMMANDS_ROOT_NODE));

        // act
        auto result = CommandDecoder_ExecuteCommand(commandDecoderHandle, TEST_COMMAND);

        // assert
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CommandDecoder_Destroy(commandDecoderHandle);
    }

    /* Tests_SRS_COMMAND_DECODER_99_010:[ If any Schema API fails then the command shall not be dispatched and it shall return EXECUTE_COMMAND_ERROR.]*/
    TEST_FUNCTION(CommandDecoder_When_GetActionArgumentName_Fails_ExecuteCommand_Fails)
    {
        // arrange
        CCommandDecoderMocks mocks;
        COMMAND_DECODER_HANDLE commandDecoderHandle = CommandDecoder_Create(TEST_MODEL_HANDLE, ActionCallbackMock, TEST_CALLBACK_CONTEXT_VALUE);
        mocks.ResetAllCalls();
        
        size_t argCount = 1;
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_COMMAND) + 1)); /*this creates a copy of the command that is given to JSON decoder*/
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*free-ing the copy of the buffer*/
            .IgnoreArgument(1);
        SetupCommand(&mocks, quotedSetACStateName, setACStateName);
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionArgumentCount(SetACStateActionHandle, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &argCount, sizeof(argCount));
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is allocating memory for the argument array*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionArgumentByIndex(SetACStateActionHandle, 0))
            .SetReturn(StateActionArgument);
        STRICT_EXPECTED_CALL(mocks, Schema_GetActionArgumentName(StateActionArgument))
            .SetReturn((const char*)NULL);
        STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TEST_COMMANDS_ROOT_NODE));

        // act
        auto result = CommandDecoder_ExecuteCommand(commandDecoderHandle, TEST_COMMAND);

        // assert
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CommandDecoder_Destroy(commandDecoderHandle);
    }

    /* Tests_SRS_COMMAND_DECODER_99_010:[ If any Schema API fails then the command shall not be dispatched and it shall return EXECUTE_COMMAND_ERROR.]*/
    TEST_FUNCTION(CommandDecoder_When_GetActionArgumentType_Fails_ExecuteCommand_Fails)
    {
        // arrange
        CCommandDecoderMocks mocks;
        COMMAND_DECODER_HANDLE commandDecoderHandle = CommandDecoder_Create(TEST_MODEL_HANDLE, ActionCallbackMock, TEST_CALLBACK_CONTEXT_VALUE);
        mocks.ResetAllCalls();
        
        size_t argCount = 1;
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_COMMAND) + 1)); /*this creates a copy of the command that is given to JSON decoder*/
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*free-ing the copy of the buffer*/
            .IgnoreArgument(1);
        SetupCommand(&mocks, quotedSetACStateName, setACStateName);
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionArgumentCount(SetACStateActionHandle, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &argCount, sizeof(argCount));
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is allocating memory for the argument array*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionArgumentByIndex(SetACStateActionHandle, 0))
            .SetReturn(StateActionArgument);
        STRICT_EXPECTED_CALL(mocks, Schema_GetActionArgumentName(StateActionArgument))
            .SetReturn(StateActionArgument_Name);
        STRICT_EXPECTED_CALL(mocks, Schema_GetActionArgumentType(StateActionArgument))
            .SetReturn((const char*)NULL);
        STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TEST_COMMANDS_ROOT_NODE));

        // act
        auto result = CommandDecoder_ExecuteCommand(commandDecoderHandle, TEST_COMMAND);

        // assert
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CommandDecoder_Destroy(commandDecoderHandle);
    }

    /* Tests_SRS_COMMAND_DECODER_99_012:[ If any argument is missing in the command text then the command shall not be dispatched and it shall return EXECUTE_COMMAND_ERROR.] */
    TEST_FUNCTION(CommandDecoder_When_Getting_The_Argument_Node_Fails_ExecuteCommand_Fails)
    {
        // arrange
        CCommandDecoderMocks mocks;
        COMMAND_DECODER_HANDLE commandDecoderHandle = CommandDecoder_Create(TEST_MODEL_HANDLE, ActionCallbackMock, TEST_CALLBACK_CONTEXT_VALUE);
        mocks.ResetAllCalls();
        
        size_t argCount = 1;
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_COMMAND) + 1)); /*this creates a copy of the command that is given to JSON decoder*/
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*free-ing the copy of the buffer*/
            .IgnoreArgument(1);
        SetupCommand(&mocks, quotedSetACStateName, setACStateName);
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionArgumentCount(SetACStateActionHandle, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &argCount, sizeof(argCount));
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is allocating memory for the argument array*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        SetupArgumentCalls(&mocks, SetACStateActionHandle, 0, StateActionArgument, StateActionArgument_Name, StateActionArgument_Type);
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetChildByName(TEST_COMMAND_ARGS_NODE, "State", IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(3, &TEST_ARG1_NODE, sizeof(TEST_ARG1_NODE))
            .SetReturn(MULTITREE_INVALID_ARG);
        STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TEST_COMMANDS_ROOT_NODE));

        // act
        auto result = CommandDecoder_ExecuteCommand(commandDecoderHandle, TEST_COMMAND);

        // assert
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CommandDecoder_Destroy(commandDecoderHandle);
    }

    /* Tests_SRS_COMMAND_DECODER_01_015: [If any MultiTree API call fails then the processing shall stop and the command shall not be dispatched and it shall return EXECUTE_COMMAND_ERROR.] */
    TEST_FUNCTION(CommandDecoder_When_Getting_The_Argument_Node_Value_Fails_ExecuteCommand_Fails)
    {
        // arrange
        CCommandDecoderMocks mocks;
        COMMAND_DECODER_HANDLE commandDecoderHandle = CommandDecoder_Create(TEST_MODEL_HANDLE, ActionCallbackMock, TEST_CALLBACK_CONTEXT_VALUE);
        mocks.ResetAllCalls();
        
        size_t argCount = 1;
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_COMMAND) + 1)); /*this creates a copy of the command that is given to JSON decoder*/
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*free-ing the copy of the buffer*/
            .IgnoreArgument(1);
        SetupCommand(&mocks, quotedSetACStateName, setACStateName);
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionArgumentCount(SetACStateActionHandle, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &argCount, sizeof(argCount));
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is allocating memory for the argument array*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        SetupArgumentCalls(&mocks, SetACStateActionHandle, 0, StateActionArgument, StateActionArgument_Name, StateActionArgument_Type);
        const char* stateValue = "true";
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetChildByName(TEST_COMMAND_ARGS_NODE, "State", IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(3, &TEST_ARG1_NODE, sizeof(TEST_ARG1_NODE));
        STRICT_EXPECTED_CALL(mocks, CodeFirst_GetPrimitiveType("bool"))
            .SetReturn(EDM_BOOLEAN_TYPE);
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetValue(TEST_ARG1_NODE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &stateValue, sizeof(stateValue))
            .SetReturn(MULTITREE_INVALID_ARG);
        STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TEST_COMMANDS_ROOT_NODE));

        // act
        auto result = CommandDecoder_ExecuteCommand(commandDecoderHandle, TEST_COMMAND);

        // assert
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CommandDecoder_Destroy(commandDecoderHandle);
    }

    /* Tests_SRS_COMMAND_DECODER_99_012:[ If any argument is missing in the command text then the command shall not be dispatched and it shall return EXECUTE_COMMAND_ERROR.] */
    TEST_FUNCTION(CommandDecoder_When_Decoding_The_Argument_Value_Fails_ExecuteCommand_Fails)
    {
        // arrange
        CCommandDecoderMocks mocks;
        COMMAND_DECODER_HANDLE commandDecoderHandle = CommandDecoder_Create(TEST_MODEL_HANDLE, ActionCallbackMock, TEST_CALLBACK_CONTEXT_VALUE);
        mocks.ResetAllCalls();
        
        size_t argCount = 1;
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_COMMAND) + 1)); /*this creates a copy of the command that is given to JSON decoder*/
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*free-ing the copy of the buffer*/
            .IgnoreArgument(1);
        SetupCommand(&mocks, quotedSetACStateName, setACStateName);
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionArgumentCount(SetACStateActionHandle, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &argCount, sizeof(argCount));
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is allocating memory for the argument array*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        SetupArgumentCalls(&mocks, SetACStateActionHandle, 0, StateActionArgument, StateActionArgument_Name, StateActionArgument_Type);
        const char* stateValue = "true";
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetChildByName(TEST_COMMAND_ARGS_NODE, "State", IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(3, &TEST_ARG1_NODE, sizeof(TEST_ARG1_NODE));
        STRICT_EXPECTED_CALL(mocks, CodeFirst_GetPrimitiveType("bool"))
            .SetReturn(EDM_BOOLEAN_TYPE);
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetValue(TEST_ARG1_NODE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &stateValue, sizeof(stateValue));
        STRICT_EXPECTED_CALL(mocks, CreateAgentDataType_From_String(stateValue, EDM_BOOLEAN_TYPE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(3, &StateAgentDataType, sizeof(StateAgentDataType))
            .SetReturn(AGENT_DATA_TYPES_INVALID_ARG);
        STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TEST_COMMANDS_ROOT_NODE));

        // act
        auto result = CommandDecoder_ExecuteCommand(commandDecoderHandle, TEST_COMMAND);

        // assert
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CommandDecoder_Destroy(commandDecoderHandle);
    }

    /* Tests_SRS_COMMAND_DECODER_99_011:[ CommandDecoder shall attempt to extract from the command text the value for each action argument.] */
    /* Tests_SRS_COMMAND_DECODER_99_027:[ The value for an argument of primitive type shall be decoded by using the CreateAgentDataType_From_String API.] */
    /* Tests_SRS_COMMAND_DECODER_01_014: [CommandDecoder shall use the MultiTree APIs to extract a specific element from the command JSON.] */
    /* Tests_SRS_COMMAND_DECODER_99_005:[ If an action is decoded successfully then the callback actionCallback shall be called, passing to it the callback action context, decoded name and arguments.] */
    TEST_FUNCTION(CommandDecoder_ExecuteCommand_With_Valid_Command_With_2_Args_Decodes_The_Arguments_And_Calls_The_ActionCallback)
    {
        // arrange
        CCommandDecoderMocks mocks;
        COMMAND_DECODER_HANDLE commandDecoderHandle = CommandDecoder_Create(TEST_MODEL_HANDLE, ActionCallbackMock, TEST_CALLBACK_CONTEXT_VALUE);
        mocks.ResetAllCalls();

        /* arg 1 */
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_COMMAND) + 1)); /*this creates a copy of the command that is given to JSON decoder*/
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*free-ing the copy of the buffer*/
            .IgnoreArgument(1);
        SetupCommand(&mocks, quotedSetACStateName, setACStateName);

        size_t argCount = 2;
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionArgumentCount(SetACStateActionHandle, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &argCount, sizeof(argCount));
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is allocating memory for the argument array*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        SetupArgumentCalls(&mocks, SetACStateActionHandle, 0, StateActionArgument, StateActionArgument_Name, StateActionArgument_Type);

        const char* stateValue = "true";
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetChildByName(TEST_COMMAND_ARGS_NODE, "State", IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(3, &TEST_ARG1_NODE, sizeof(TEST_ARG1_NODE));
        STRICT_EXPECTED_CALL(mocks, CodeFirst_GetPrimitiveType("bool"))
            .SetReturn(EDM_BOOLEAN_TYPE);
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetValue(TEST_ARG1_NODE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &stateValue, sizeof(stateValue));
        STRICT_EXPECTED_CALL(mocks, CreateAgentDataType_From_String(stateValue, EDM_BOOLEAN_TYPE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(3, &StateAgentDataType, sizeof(StateAgentDataType));

        /* arg 2 */
        SetupArgumentCalls(&mocks, SetACStateActionHandle, 1, OtherArgActionArgument, OtherArgActionArgument_Name, OtherArgActionArgument_Type);
        const char* otherArgValue = OtherArgValue;
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetChildByName(TEST_COMMAND_ARGS_NODE, "OtherArg", IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(3, &TEST_ARG2_NODE, sizeof(TEST_ARG2_NODE));
        STRICT_EXPECTED_CALL(mocks, CodeFirst_GetPrimitiveType("ascii_char_ptr"))
            .SetReturn(EDM_STRING_TYPE);
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetValue(TEST_ARG2_NODE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &otherArgValue, sizeof(otherArgValue));
        STRICT_EXPECTED_CALL(mocks, CreateAgentDataType_From_String(otherArgValue, EDM_STRING_TYPE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(3, &OtherArgAgentDataType, sizeof(OtherArgAgentDataType));

        STRICT_EXPECTED_CALL(mocks, ActionCallbackMock(TEST_CALLBACK_CONTEXT_VALUE, "", "SetACState", 2, IGNORED_PTR_ARG))
            .IgnoreArgument(5);

        EXPECTED_CALL(mocks, Destroy_AGENT_DATA_TYPE(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, Destroy_AGENT_DATA_TYPE(IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TEST_COMMANDS_ROOT_NODE));

        // act
        auto result = CommandDecoder_ExecuteCommand(commandDecoderHandle, TEST_COMMAND);

        // assert
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_SUCCESS, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CommandDecoder_Destroy(commandDecoderHandle);
    }

    /* Tests_SRS_COMMAND_DECODER_99_010:[ If any Schema API fails then the command shall not be dispatched and it shall return EXECUTE_COMMAND_ERROR.]*/
    TEST_FUNCTION(CommandDecoder_When_GetArgument_For_The_2nd_Argument_Fails_Then_ExecuteCommand_Fails)
    {
        // arrange
        CCommandDecoderMocks mocks;
        COMMAND_DECODER_HANDLE commandDecoderHandle = CommandDecoder_Create(TEST_MODEL_HANDLE, ActionCallbackMock, TEST_CALLBACK_CONTEXT_VALUE);
        mocks.ResetAllCalls();
        
        /* arg 1 */
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_COMMAND) + 1)); /*this creates a copy of the command that is given to JSON decoder*/
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*free-ing the copy of the buffer*/
            .IgnoreArgument(1);
        SetupCommand(&mocks, quotedSetACStateName, setACStateName);

        size_t argCount = 2;
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionArgumentCount(SetACStateActionHandle, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &argCount, sizeof(argCount));
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is allocating memory for the argument array*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        SetupArgumentCalls(&mocks, SetACStateActionHandle, 0, StateActionArgument, StateActionArgument_Name, StateActionArgument_Type);
        const char* stateValue = "true";
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetChildByName(TEST_COMMAND_ARGS_NODE, "State", IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(3, &TEST_ARG1_NODE, sizeof(TEST_ARG1_NODE));
        STRICT_EXPECTED_CALL(mocks, CodeFirst_GetPrimitiveType("bool"))
            .SetReturn(EDM_BOOLEAN_TYPE);
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetValue(TEST_ARG1_NODE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &stateValue, sizeof(stateValue));
        STRICT_EXPECTED_CALL(mocks, CreateAgentDataType_From_String(stateValue, EDM_BOOLEAN_TYPE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(3, &StateAgentDataType, sizeof(StateAgentDataType));

        /* arg 2 */
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionArgumentByIndex(SetACStateActionHandle, 1))
            .SetReturn((SCHEMA_ACTION_ARGUMENT_HANDLE)NULL);

        EXPECTED_CALL(mocks, Destroy_AGENT_DATA_TYPE(IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TEST_COMMANDS_ROOT_NODE));

        // act
        auto result = CommandDecoder_ExecuteCommand(commandDecoderHandle, TEST_COMMAND);

        // assert
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CommandDecoder_Destroy(commandDecoderHandle);
    }

    /* Tests_SRS_COMMAND_DECODER_99_010:[ If any Schema API fails then the command shall not be dispatched and it shall return EXECUTE_COMMAND_ERROR.]*/
    TEST_FUNCTION(CommandDecoder_When_GetArgument_Name_For_The_2nd_Argument_Fails_Then_ExecuteCommand_Fails)
    {
        // arrange
        CCommandDecoderMocks mocks;
        COMMAND_DECODER_HANDLE commandDecoderHandle = CommandDecoder_Create(TEST_MODEL_HANDLE, ActionCallbackMock, TEST_CALLBACK_CONTEXT_VALUE);
        mocks.ResetAllCalls();
        
        /* arg 1 */
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_COMMAND) + 1)); /*this creates a copy of the command that is given to JSON decoder*/
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*free-ing the copy of the buffer*/
            .IgnoreArgument(1);

        SetupCommand(&mocks, quotedSetACStateName, setACStateName);
        size_t argCount = 2;
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionArgumentCount(SetACStateActionHandle, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &argCount, sizeof(argCount));
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is allocating memory for the argument array*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        SetupArgumentCalls(&mocks, SetACStateActionHandle, 0, StateActionArgument, StateActionArgument_Name, StateActionArgument_Type);
        const char* stateValue = "true";
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetChildByName(TEST_COMMAND_ARGS_NODE, "State", IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(3, &TEST_ARG1_NODE, sizeof(TEST_ARG1_NODE));
        STRICT_EXPECTED_CALL(mocks, CodeFirst_GetPrimitiveType("bool"))
            .SetReturn(EDM_BOOLEAN_TYPE);
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetValue(TEST_ARG1_NODE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &stateValue, sizeof(stateValue));
        STRICT_EXPECTED_CALL(mocks, CreateAgentDataType_From_String(stateValue, EDM_BOOLEAN_TYPE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(3, &StateAgentDataType, sizeof(StateAgentDataType));

        /* arg 2 */
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionArgumentByIndex(SetACStateActionHandle, 1))
            .SetReturn(OtherArgActionArgument);
        STRICT_EXPECTED_CALL(mocks, Schema_GetActionArgumentName(OtherArgActionArgument))
            .SetReturn((const char*)NULL);

        EXPECTED_CALL(mocks, Destroy_AGENT_DATA_TYPE(IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TEST_COMMANDS_ROOT_NODE));

        // act
        auto result = CommandDecoder_ExecuteCommand(commandDecoderHandle, TEST_COMMAND);

        // assert
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CommandDecoder_Destroy(commandDecoderHandle);
    }

    /* Tests_SRS_COMMAND_DECODER_99_010:[ If any Schema API fails then the command shall not be dispatched and it shall return EXECUTE_COMMAND_ERROR.]*/
    TEST_FUNCTION(CommandDecoder_When_GetArgument_Type_For_The_2nd_Argument_Fails_Then_ExecuteCommand_Fails)
    {
        // arrange
        CCommandDecoderMocks mocks;
        COMMAND_DECODER_HANDLE commandDecoderHandle = CommandDecoder_Create(TEST_MODEL_HANDLE, ActionCallbackMock, TEST_CALLBACK_CONTEXT_VALUE);
        mocks.ResetAllCalls();

        /* arg 1 */
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_COMMAND) + 1)); /*this creates a copy of the command that is given to JSON decoder*/
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*free-ing the copy of the buffer*/
            .IgnoreArgument(1);

        SetupCommand(&mocks, quotedSetACStateName, setACStateName);
        size_t argCount = 2;
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionArgumentCount(SetACStateActionHandle, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &argCount, sizeof(argCount));
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is allocating memory for the argument array*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        SetupArgumentCalls(&mocks, SetACStateActionHandle, 0, StateActionArgument, StateActionArgument_Name, StateActionArgument_Type);
        const char* stateValue = "true";
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetChildByName(TEST_COMMAND_ARGS_NODE, "State", IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(3, &TEST_ARG1_NODE, sizeof(TEST_ARG1_NODE));
        STRICT_EXPECTED_CALL(mocks, CodeFirst_GetPrimitiveType("bool"))
            .SetReturn(EDM_BOOLEAN_TYPE);
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetValue(TEST_ARG1_NODE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &stateValue, sizeof(stateValue));
        STRICT_EXPECTED_CALL(mocks, CreateAgentDataType_From_String(stateValue, EDM_BOOLEAN_TYPE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(3, &StateAgentDataType, sizeof(StateAgentDataType));

        /* arg 2 */
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionArgumentByIndex(SetACStateActionHandle, 1))
            .SetReturn(OtherArgActionArgument);
        STRICT_EXPECTED_CALL(mocks, Schema_GetActionArgumentName(OtherArgActionArgument))
            .SetReturn(OtherArgActionArgument_Name);
        STRICT_EXPECTED_CALL(mocks, Schema_GetActionArgumentType(OtherArgActionArgument))
            .SetReturn((const char*)NULL);

        EXPECTED_CALL(mocks, Destroy_AGENT_DATA_TYPE(IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TEST_COMMANDS_ROOT_NODE));

        // act
        auto result = CommandDecoder_ExecuteCommand(commandDecoderHandle, TEST_COMMAND);

        // assert
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CommandDecoder_Destroy(commandDecoderHandle);
    }

    /* Tests_SRS_COMMAND_DECODER_99_012:[ If any argument is missing in the command text then the command shall not be dispatched and it shall return EXECUTE_COMMAND_ERROR.] */
    TEST_FUNCTION(CommandDecoder_When_GetChildName_For_The_2nd_Argument_Fails_Then_ExecuteCommand_Fails)
    {
        // arrange
        CCommandDecoderMocks mocks;
        COMMAND_DECODER_HANDLE commandDecoderHandle = CommandDecoder_Create(TEST_MODEL_HANDLE, ActionCallbackMock, TEST_CALLBACK_CONTEXT_VALUE);
        mocks.ResetAllCalls();
    
        /* arg 1 */
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_COMMAND) + 1)); /*this creates a copy of the command that is given to JSON decoder*/
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*free-ing the copy of the buffer*/
            .IgnoreArgument(1);

        SetupCommand(&mocks, quotedSetACStateName, setACStateName);
        size_t argCount = 2;
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionArgumentCount(SetACStateActionHandle, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &argCount, sizeof(argCount));
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is allocating memory for the argument array*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        SetupArgumentCalls(&mocks, SetACStateActionHandle, 0, StateActionArgument, StateActionArgument_Name, StateActionArgument_Type);
        const char* stateValue = "true";
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetChildByName(TEST_COMMAND_ARGS_NODE, "State", IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(3, &TEST_ARG1_NODE, sizeof(TEST_ARG1_NODE));
        STRICT_EXPECTED_CALL(mocks, CodeFirst_GetPrimitiveType("bool"))
            .SetReturn(EDM_BOOLEAN_TYPE);
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetValue(TEST_ARG1_NODE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &stateValue, sizeof(stateValue));
        STRICT_EXPECTED_CALL(mocks, CreateAgentDataType_From_String(stateValue, EDM_BOOLEAN_TYPE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(3, &StateAgentDataType, sizeof(StateAgentDataType));

        /* arg 2 */
        SetupArgumentCalls(&mocks, SetACStateActionHandle, 1, OtherArgActionArgument, OtherArgActionArgument_Name, OtherArgActionArgument_Type);
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetChildByName(TEST_COMMAND_ARGS_NODE, "OtherArg", IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(3, &TEST_ARG2_NODE, sizeof(TEST_ARG2_NODE))
            .SetReturn(MULTITREE_INVALID_ARG);

        EXPECTED_CALL(mocks, Destroy_AGENT_DATA_TYPE(IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TEST_COMMANDS_ROOT_NODE));

        // act
        auto result = CommandDecoder_ExecuteCommand(commandDecoderHandle, TEST_COMMAND);

        // assert
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CommandDecoder_Destroy(commandDecoderHandle);
    }

    /* Tests_SRS_COMMAND_DECODER_01_015: [If any MultiTree API call fails then the processing shall stop and the command shall not be dispatched and it shall return EXECUTE_COMMAND_ERROR.] */
    TEST_FUNCTION(CommandDecoder_When_GetValue_For_The_2nd_Argument_Fails_Then_ExecuteCommand_Fails)
    {
        // arrange
        CCommandDecoderMocks mocks;
        COMMAND_DECODER_HANDLE commandDecoderHandle = CommandDecoder_Create(TEST_MODEL_HANDLE, ActionCallbackMock, TEST_CALLBACK_CONTEXT_VALUE);
        mocks.ResetAllCalls();

        /* arg 1 */
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_COMMAND) + 1)); /*this creates a copy of the command that is given to JSON decoder*/
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*free-ing the copy of the buffer*/
            .IgnoreArgument(1);

        SetupCommand(&mocks, quotedSetACStateName, setACStateName);
        size_t argCount = 2;
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionArgumentCount(SetACStateActionHandle, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &argCount, sizeof(argCount));
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is allocating memory for the argument array*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        SetupArgumentCalls(&mocks, SetACStateActionHandle, 0, StateActionArgument, StateActionArgument_Name, StateActionArgument_Type);
        const char* stateValue = "true";
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetChildByName(TEST_COMMAND_ARGS_NODE, "State", IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(3, &TEST_ARG1_NODE, sizeof(TEST_ARG1_NODE));
        STRICT_EXPECTED_CALL(mocks, CodeFirst_GetPrimitiveType("bool"))
            .SetReturn(EDM_BOOLEAN_TYPE);
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetValue(TEST_ARG1_NODE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &stateValue, sizeof(stateValue));
        STRICT_EXPECTED_CALL(mocks, CreateAgentDataType_From_String(stateValue, EDM_BOOLEAN_TYPE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(3, &StateAgentDataType, sizeof(StateAgentDataType));

        /* arg 2 */
        SetupArgumentCalls(&mocks, SetACStateActionHandle, 1, OtherArgActionArgument, OtherArgActionArgument_Name, OtherArgActionArgument_Type);
        const char* otherArgValue = OtherArgValue;
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetChildByName(TEST_COMMAND_ARGS_NODE, "OtherArg", IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(3, &TEST_ARG2_NODE, sizeof(TEST_ARG2_NODE));
        STRICT_EXPECTED_CALL(mocks, CodeFirst_GetPrimitiveType("ascii_char_ptr"))
            .SetReturn(EDM_STRING_TYPE);
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetValue(TEST_ARG2_NODE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &otherArgValue, sizeof(otherArgValue))
            .SetReturn(MULTITREE_INVALID_ARG);

        EXPECTED_CALL(mocks, Destroy_AGENT_DATA_TYPE(IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TEST_COMMANDS_ROOT_NODE));

        // act
        auto result = CommandDecoder_ExecuteCommand(commandDecoderHandle, TEST_COMMAND);

        // assert
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CommandDecoder_Destroy(commandDecoderHandle);
    }

    /* Tests_SRS_COMMAND_DECODER_99_012:[ If any argument is missing in the command text then the command shall not be dispatched and it shall return EXECUTE_COMMAND_ERROR.] */
    TEST_FUNCTION(CommandDecoder_When_Creating_The_Agent_Data_Type_For_The_2nd_Argument_Fails_Then_ExecuteCommand_Fails)
    {
        // arrange
        CCommandDecoderMocks mocks;
        COMMAND_DECODER_HANDLE commandDecoderHandle = CommandDecoder_Create(TEST_MODEL_HANDLE, ActionCallbackMock, TEST_CALLBACK_CONTEXT_VALUE);
        mocks.ResetAllCalls();

        /* arg 1 */
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_COMMAND) + 1)); /*this creates a copy of the command that is given to JSON decoder*/
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*free-ing the copy of the buffer*/
            .IgnoreArgument(1);
        
        SetupCommand(&mocks, quotedSetACStateName, setACStateName);
        size_t argCount = 2;
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionArgumentCount(SetACStateActionHandle, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &argCount, sizeof(argCount));
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is allocating memory for the argument array*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        SetupArgumentCalls(&mocks, SetACStateActionHandle, 0, StateActionArgument, StateActionArgument_Name, StateActionArgument_Type);
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetChildByName(TEST_COMMAND_ARGS_NODE, "State", IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(3, &TEST_ARG1_NODE, sizeof(TEST_ARG1_NODE));
        const char* stateValue = "true";
        STRICT_EXPECTED_CALL(mocks, CodeFirst_GetPrimitiveType("bool"))
            .SetReturn(EDM_BOOLEAN_TYPE);
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetValue(TEST_ARG1_NODE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &stateValue, sizeof(stateValue));
        STRICT_EXPECTED_CALL(mocks, CreateAgentDataType_From_String(stateValue, EDM_BOOLEAN_TYPE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(3, &StateAgentDataType, sizeof(StateAgentDataType));

        /* arg 2 */
        SetupArgumentCalls(&mocks, SetACStateActionHandle, 1, OtherArgActionArgument, OtherArgActionArgument_Name, OtherArgActionArgument_Type);
        const char* otherArgValue = OtherArgValue;
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetChildByName(TEST_COMMAND_ARGS_NODE, "OtherArg", IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(3, &TEST_ARG2_NODE, sizeof(TEST_ARG2_NODE));
        STRICT_EXPECTED_CALL(mocks, CodeFirst_GetPrimitiveType("ascii_char_ptr"))
            .SetReturn(EDM_STRING_TYPE);
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetValue(TEST_ARG2_NODE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &otherArgValue, sizeof(otherArgValue));
        STRICT_EXPECTED_CALL(mocks, CreateAgentDataType_From_String(otherArgValue, EDM_STRING_TYPE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(3, &OtherArgAgentDataType, sizeof(OtherArgAgentDataType))
            .SetReturn(AGENT_DATA_TYPES_INVALID_ARG);

        EXPECTED_CALL(mocks, Destroy_AGENT_DATA_TYPE(IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TEST_COMMANDS_ROOT_NODE));

        // act
        auto result = CommandDecoder_ExecuteCommand(commandDecoderHandle, TEST_COMMAND);

        // assert
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CommandDecoder_Destroy(commandDecoderHandle);
    }

    /* Tests_SRS_COMMAND_DECODER_99_029:[ If the argument type is complex then a complex type value shall be built from the child nodes.] */
    /* Tests_SRS_COMMAND_DECODER_99_030:[ For each child node a value shall be built by using AgentTypeSystem APIs.] */
    /* Tests_SRS_COMMAND_DECODER_99_031:[ The complex type value that aggregates the children shall be built by using the Create_AGENT_DATA_TYPE_from_Members.] */
    /* Tests_SRS_COMMAND_DECODER_99_033:[ In order to determine which are the members of a complex types, Schema APIs for structure types shall be used.] */
    /* Tests_SRS_COMMAND_DECODER_01_014: [CommandDecoder shall use the MultiTree APIs to extract a specific element from the command JSON.] */
    /* Tests_SRS_COMMAND_DECODER_99_005:[ If an action is decoded successfully then the callback actionCallback shall be called, passing to it the callback action context, decoded name and arguments.] */
    TEST_FUNCTION(CommandDecoder_When_The_Argument_Is_Complex_The_Nodes_Are_Scanned_To_Get_The_Members)
    {
        // arrange
        CCommandDecoderMocks mocks;
        COMMAND_DECODER_HANDLE commandDecoderHandle = CommandDecoder_Create(TEST_MODEL_HANDLE, ActionCallbackMock, TEST_CALLBACK_CONTEXT_VALUE);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_COMMAND) + 1)); /*this creates a copy of the command that is given to JSON decoder*/
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*free-ing the copy of the buffer*/
            .IgnoreArgument(1);

        SetupCommand(&mocks, quotedSetLocationName, setLocationName);
        size_t argCount = 1;
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionArgumentCount(SetLocationActionHandle, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &argCount, sizeof(argCount));
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is allocating memory for the argument array*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        SetupArgumentCalls(&mocks, SetACStateActionHandle, 0, LocationActionArgument, LocationActionArgument_Name, LocationActionArgument_Type);
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetChildByName(TEST_COMMAND_ARGS_NODE, "Location", IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(3, &TEST_ARG1_NODE, sizeof(TEST_ARG1_NODE));
        STRICT_EXPECTED_CALL(mocks, CodeFirst_GetPrimitiveType("GeoLocation"))
            .SetReturn(EDM_NO_TYPE);
        STRICT_EXPECTED_CALL(mocks, Schema_GetStructTypeByName(TEST_SCHEMA_HANDLE, "GeoLocation"))
            .SetReturn(TEST_STRUCT_1_HANDLE);
        size_t memberCount = 2;
        STRICT_EXPECTED_CALL(mocks, Schema_GetStructTypePropertyCount(TEST_STRUCT_1_HANDLE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &memberCount, sizeof(memberCount));
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is allocating the member values of the struct*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is allocating the member names of the struct*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        /* member 1 */
        STRICT_EXPECTED_CALL(mocks, Schema_GetStructTypePropertyByIndex(TEST_STRUCT_1_HANDLE, 0))
            .SetReturn(memberProperty1);
        STRICT_EXPECTED_CALL(mocks, Schema_GetPropertyName(memberProperty1))
            .SetReturn("Lat");
        STRICT_EXPECTED_CALL(mocks, Schema_GetPropertyType(memberProperty1))
            .SetReturn("double");
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetChildByName(TEST_ARG1_NODE, "Lat", IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(3, &TEST_MEMBER1_NODE, sizeof(TEST_MEMBER1_NODE));
        const char* latValue = "42.42";
        STRICT_EXPECTED_CALL(mocks, CodeFirst_GetPrimitiveType("double"))
            .SetReturn(EDM_DOUBLE_TYPE);
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetValue(TEST_MEMBER1_NODE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &latValue, sizeof(latValue));
        STRICT_EXPECTED_CALL(mocks, CreateAgentDataType_From_String(latValue, EDM_DOUBLE_TYPE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(3, &LatAgentDataType, sizeof(LatAgentDataType));

        /* member 2 */
        STRICT_EXPECTED_CALL(mocks, Schema_GetStructTypePropertyByIndex(TEST_STRUCT_1_HANDLE, 1))
            .SetReturn(memberProperty2);
        STRICT_EXPECTED_CALL(mocks, Schema_GetPropertyName(memberProperty2))
            .SetReturn("Long");
        STRICT_EXPECTED_CALL(mocks, Schema_GetPropertyType(memberProperty2))
            .SetReturn("double");
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetChildByName(TEST_ARG1_NODE, "Long", IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(3, &TEST_MEMBER2_NODE, sizeof(TEST_MEMBER2_NODE));
        const char * longValue = "1.2";
        STRICT_EXPECTED_CALL(mocks, CodeFirst_GetPrimitiveType("double"))
            .SetReturn(EDM_DOUBLE_TYPE);
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetValue(TEST_MEMBER2_NODE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &longValue, sizeof(longValue));
        STRICT_EXPECTED_CALL(mocks, CreateAgentDataType_From_String(longValue, EDM_DOUBLE_TYPE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(3, &LongAgentDataType, sizeof(LongAgentDataType));

        EXPECTED_CALL(mocks, Create_AGENT_DATA_TYPE_from_Members(IGNORED_PTR_ARG, "GeoLocation", 2, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .ValidateArgument(2).ValidateArgument(3);

        STRICT_EXPECTED_CALL(mocks, ActionCallbackMock(TEST_CALLBACK_CONTEXT_VALUE, "", "SetLocation", 1, IGNORED_PTR_ARG))
            .IgnoreArgument(5);

        EXPECTED_CALL(mocks, Destroy_AGENT_DATA_TYPE(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, Destroy_AGENT_DATA_TYPE(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, Destroy_AGENT_DATA_TYPE(IGNORED_PTR_ARG));

        STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TEST_COMMANDS_ROOT_NODE));

        // act
        auto result = CommandDecoder_ExecuteCommand(commandDecoderHandle, TEST_COMMAND);

        // assert
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_SUCCESS, result);
        mocks.AssertActualAndExpectedCalls();
        ASSERT_ARE_EQUAL(char_ptr, "Lat", lastMemberNames[0][0]);
        ASSERT_ARE_EQUAL(char_ptr, "Long", lastMemberNames[0][1]);

        // cleanup
        CommandDecoder_Destroy(commandDecoderHandle);
    }

    /*Tests_SRS_COMMAND_DECODER_99_021:[ If the parsing of the command fails for any other reason the command shall not be dispatched.]*/
    TEST_FUNCTION(CommandDecoder_When_The_Argument_Is_Complex_The_Nodes_Are_Scanned_To_Get_The_Members_and_fail_when_gbaloc_fails_1)
    {
        // arrange
        CCommandDecoderMocks mocks;
        COMMAND_DECODER_HANDLE commandDecoderHandle = CommandDecoder_Create(TEST_MODEL_HANDLE, ActionCallbackMock, TEST_CALLBACK_CONTEXT_VALUE);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_COMMAND) + 1)); /*this creates a copy of the command that is given to JSON decoder*/
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*free-ing the copy of the buffer*/
            .IgnoreArgument(1);

        SetupCommand(&mocks, quotedSetLocationName, setLocationName);
        size_t argCount = 1;
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionArgumentCount(SetLocationActionHandle, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &argCount, sizeof(argCount));
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is allocating memory for the argument array*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        SetupArgumentCalls(&mocks, SetACStateActionHandle, 0, LocationActionArgument, LocationActionArgument_Name, LocationActionArgument_Type);
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetChildByName(TEST_COMMAND_ARGS_NODE, "Location", IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(3, &TEST_ARG1_NODE, sizeof(TEST_ARG1_NODE));
        STRICT_EXPECTED_CALL(mocks, CodeFirst_GetPrimitiveType("GeoLocation"))
            .SetReturn(EDM_NO_TYPE);
        STRICT_EXPECTED_CALL(mocks, Schema_GetStructTypeByName(TEST_SCHEMA_HANDLE, "GeoLocation"))
            .SetReturn(TEST_STRUCT_1_HANDLE);
        size_t memberCount = 2;
        STRICT_EXPECTED_CALL(mocks, Schema_GetStructTypePropertyCount(TEST_STRUCT_1_HANDLE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &memberCount, sizeof(memberCount));
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is allocating the member values of the struct*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        whenShallmalloc_fail = currentmalloc_call + 5;
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is allocating the member names of the struct*/
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TEST_COMMANDS_ROOT_NODE));

        // act
        auto result = CommandDecoder_ExecuteCommand(commandDecoderHandle, TEST_COMMAND);

        // assert
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
        mocks.AssertActualAndExpectedCalls();
        ASSERT_ARE_EQUAL(char_ptr, "Lat", lastMemberNames[0][0]);
        ASSERT_ARE_EQUAL(char_ptr, "Long", lastMemberNames[0][1]);

        // cleanup
        CommandDecoder_Destroy(commandDecoderHandle);
    }

    /*Tests_SRS_COMMAND_DECODER_99_021:[ If the parsing of the command fails for any other reason the command shall not be dispatched.]*/
    TEST_FUNCTION(CommandDecoder_When_The_Argument_Is_Complex_The_Nodes_Are_Scanned_To_Get_The_Members_and_fail_when_gbaloc_fails_2)
    {
        // arrange
        CCommandDecoderMocks mocks;
        COMMAND_DECODER_HANDLE commandDecoderHandle = CommandDecoder_Create(TEST_MODEL_HANDLE, ActionCallbackMock, TEST_CALLBACK_CONTEXT_VALUE);
        mocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_COMMAND) + 1)); /*this creates a copy of the command that is given to JSON decoder*/
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*free-ing the copy of the buffer*/
            .IgnoreArgument(1);

        SetupCommand(&mocks, quotedSetLocationName, setLocationName);
        size_t argCount = 1;
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionArgumentCount(SetLocationActionHandle, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &argCount, sizeof(argCount));
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is allocating memory for the argument array*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        SetupArgumentCalls(&mocks, SetACStateActionHandle, 0, LocationActionArgument, LocationActionArgument_Name, LocationActionArgument_Type);
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetChildByName(TEST_COMMAND_ARGS_NODE, "Location", IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(3, &TEST_ARG1_NODE, sizeof(TEST_ARG1_NODE));
        STRICT_EXPECTED_CALL(mocks, CodeFirst_GetPrimitiveType("GeoLocation"))
            .SetReturn(EDM_NO_TYPE);
        STRICT_EXPECTED_CALL(mocks, Schema_GetStructTypeByName(TEST_SCHEMA_HANDLE, "GeoLocation"))
            .SetReturn(TEST_STRUCT_1_HANDLE);
        size_t memberCount = 2;
        STRICT_EXPECTED_CALL(mocks, Schema_GetStructTypePropertyCount(TEST_STRUCT_1_HANDLE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &memberCount, sizeof(memberCount));
        whenShallmalloc_fail = currentmalloc_call + 4;
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is allocating the member values of the struct*/
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TEST_COMMANDS_ROOT_NODE));

        // act
        auto result = CommandDecoder_ExecuteCommand(commandDecoderHandle, TEST_COMMAND);

        // assert
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
        mocks.AssertActualAndExpectedCalls();
        ASSERT_ARE_EQUAL(char_ptr, "Lat", lastMemberNames[0][0]);
        ASSERT_ARE_EQUAL(char_ptr, "Long", lastMemberNames[0][1]);

        // cleanup
        CommandDecoder_Destroy(commandDecoderHandle);
    }

    /* Tests_SRS_COMMAND_DECODER_99_029:[ If the argument type is complex then a complex type value shall be built from the child nodes.] */
    /* Tests_SRS_COMMAND_DECODER_99_010:[ If any Schema API fails then the command shall not be dispatched and it shall return EXECUTE_COMMAND_ERROR.]*/
    TEST_FUNCTION(CommandDecoder_When_Getting_The_Structure_Type_For_A_Complex_Type_Fails_Then_ExecuteCommand_Fails)
    {
        // arrange
        CCommandDecoderMocks mocks;
        COMMAND_DECODER_HANDLE commandDecoderHandle = CommandDecoder_Create(TEST_MODEL_HANDLE, ActionCallbackMock, TEST_CALLBACK_CONTEXT_VALUE);
        mocks.ResetAllCalls();
        
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_COMMAND) + 1)); /*this creates a copy of the command that is given to JSON decoder*/
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*free-ing the copy of the buffer*/
            .IgnoreArgument(1);

        SetupCommand(&mocks, quotedSetACStateName, setACStateName);
        size_t argCount = 1;
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionArgumentCount(SetLocationActionHandle, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &argCount, sizeof(argCount));
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is allocating memory for the argument array*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        SetupArgumentCalls(&mocks, SetACStateActionHandle, 0, LocationActionArgument, LocationActionArgument_Name, LocationActionArgument_Type);
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetChildByName(TEST_COMMAND_ARGS_NODE, "Location", IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(3, &TEST_ARG1_NODE, sizeof(TEST_ARG1_NODE));
        STRICT_EXPECTED_CALL(mocks, CodeFirst_GetPrimitiveType("GeoLocation"))
            .SetReturn(EDM_NO_TYPE);
        STRICT_EXPECTED_CALL(mocks, Schema_GetStructTypeByName(TEST_SCHEMA_HANDLE, "GeoLocation"))
            .SetReturn((SCHEMA_STRUCT_TYPE_HANDLE)NULL);
        STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TEST_COMMANDS_ROOT_NODE));

        // act
        auto result = CommandDecoder_ExecuteCommand(commandDecoderHandle, TEST_COMMAND);

        // assert
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CommandDecoder_Destroy(commandDecoderHandle);
    }

    /* Tests_SRS_COMMAND_DECODER_99_029:[ If the argument type is complex then a complex type value shall be built from the child nodes.] */
    /* Tests_SRS_COMMAND_DECODER_99_010:[ If any Schema API fails then the command shall not be dispatched and it shall return EXECUTE_COMMAND_ERROR.]*/
    TEST_FUNCTION(CommandDecoder_When_Getting_The_Structure_PropertyCount_For_A_Complex_Type_Fails_Then_ExecuteCommand_Fails)
    {
        // arrange
        CCommandDecoderMocks mocks;
        COMMAND_DECODER_HANDLE commandDecoderHandle = CommandDecoder_Create(TEST_MODEL_HANDLE, ActionCallbackMock, TEST_CALLBACK_CONTEXT_VALUE);
        mocks.ResetAllCalls();
        
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_COMMAND) + 1)); /*this creates a copy of the command that is given to JSON decoder*/
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*free-ing the copy of the buffer*/
            .IgnoreArgument(1);

        SetupCommand(&mocks, quotedSetACStateName, setACStateName);
        size_t argCount = 1;
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionArgumentCount(SetLocationActionHandle, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &argCount, sizeof(argCount));
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is allocating memory for the argument array*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        SetupArgumentCalls(&mocks, SetACStateActionHandle, 0, LocationActionArgument, LocationActionArgument_Name, LocationActionArgument_Type);
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetChildByName(TEST_COMMAND_ARGS_NODE, "Location", IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(3, &TEST_ARG1_NODE, sizeof(TEST_ARG1_NODE));
        STRICT_EXPECTED_CALL(mocks, CodeFirst_GetPrimitiveType("GeoLocation"))
            .SetReturn(EDM_NO_TYPE);
        STRICT_EXPECTED_CALL(mocks, Schema_GetStructTypeByName(TEST_SCHEMA_HANDLE, "GeoLocation"))
            .SetReturn(TEST_STRUCT_1_HANDLE);
        size_t memberCount = 1;
        STRICT_EXPECTED_CALL(mocks, Schema_GetStructTypePropertyCount(TEST_STRUCT_1_HANDLE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &memberCount, sizeof(memberCount))
            .SetReturn(SCHEMA_ERROR);
        STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TEST_COMMANDS_ROOT_NODE));

        // act
        auto result = CommandDecoder_ExecuteCommand(commandDecoderHandle, TEST_COMMAND);

        // assert
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CommandDecoder_Destroy(commandDecoderHandle);
    }

    /* Tests_SRS_COMMAND_DECODER_99_034:[ If Schema APIs indicate that a complex type has 0 members then the command shall not be dispatched and it shall return EXECUTE_COMMAND_ERROR.] */
    TEST_FUNCTION(CommandDecoder_When_The_Structure_PropertyCount_For_A_Complex_Type_Is_Zero_Then_ExecuteCommand_Fails)
    {
        // arrange
        CCommandDecoderMocks mocks;
        COMMAND_DECODER_HANDLE commandDecoderHandle = CommandDecoder_Create(TEST_MODEL_HANDLE, ActionCallbackMock, TEST_CALLBACK_CONTEXT_VALUE);
        mocks.ResetAllCalls();
        
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_COMMAND) + 1)); /*this creates a copy of the command that is given to JSON decoder*/
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*free-ing the copy of the buffer*/
            .IgnoreArgument(1);

        SetupCommand(&mocks, quotedSetACStateName, setACStateName);
        size_t argCount = 1;
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionArgumentCount(SetLocationActionHandle, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &argCount, sizeof(argCount));
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is allocating memory for the argument array*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        SetupArgumentCalls(&mocks, SetACStateActionHandle, 0, LocationActionArgument, LocationActionArgument_Name, LocationActionArgument_Type);
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetChildByName(TEST_COMMAND_ARGS_NODE, "Location", IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(3, &TEST_ARG1_NODE, sizeof(TEST_ARG1_NODE));
        STRICT_EXPECTED_CALL(mocks, CodeFirst_GetPrimitiveType("GeoLocation"))
            .SetReturn(EDM_NO_TYPE);
        STRICT_EXPECTED_CALL(mocks, Schema_GetStructTypeByName(TEST_SCHEMA_HANDLE, "GeoLocation"))
            .SetReturn(TEST_STRUCT_1_HANDLE);
        size_t memberCount = 0;
        STRICT_EXPECTED_CALL(mocks, Schema_GetStructTypePropertyCount(TEST_STRUCT_1_HANDLE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &memberCount, sizeof(memberCount));
        STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TEST_COMMANDS_ROOT_NODE));

        // act
        auto result = CommandDecoder_ExecuteCommand(commandDecoderHandle, TEST_COMMAND);

        // assert
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CommandDecoder_Destroy(commandDecoderHandle);
    }

    /* Tests_SRS_COMMAND_DECODER_99_010:[ If any Schema API fails then the command shall not be dispatched and it shall return EXECUTE_COMMAND_ERROR.]*/
    TEST_FUNCTION(CommandDecoder_When_Getting_The_Structure_Property_For_A_Complex_Type_Fails_Then_ExecuteCommand_Fails)
    {
        // arrange
        CCommandDecoderMocks mocks;
        COMMAND_DECODER_HANDLE commandDecoderHandle = CommandDecoder_Create(TEST_MODEL_HANDLE, ActionCallbackMock, TEST_CALLBACK_CONTEXT_VALUE);
        mocks.ResetAllCalls();
        
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_COMMAND) + 1)); /*this creates a copy of the command that is given to JSON decoder*/
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*free-ing the copy of the buffer*/
            .IgnoreArgument(1);

        SetupCommand(&mocks, quotedSetACStateName, setACStateName);
        size_t argCount = 1;
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionArgumentCount(SetLocationActionHandle, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &argCount, sizeof(argCount));
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is allocating memory for the argument array*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        SetupArgumentCalls(&mocks, SetACStateActionHandle, 0, LocationActionArgument, LocationActionArgument_Name, LocationActionArgument_Type);
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetChildByName(TEST_COMMAND_ARGS_NODE, "Location", IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(3, &TEST_ARG1_NODE, sizeof(TEST_ARG1_NODE));
        STRICT_EXPECTED_CALL(mocks, CodeFirst_GetPrimitiveType("GeoLocation"))
            .SetReturn(EDM_NO_TYPE);
        STRICT_EXPECTED_CALL(mocks, Schema_GetStructTypeByName(TEST_SCHEMA_HANDLE, "GeoLocation"))
            .SetReturn(TEST_STRUCT_1_HANDLE);
        size_t memberCount = 1;
        STRICT_EXPECTED_CALL(mocks, Schema_GetStructTypePropertyCount(TEST_STRUCT_1_HANDLE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &memberCount, sizeof(memberCount));
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is allocating the member values of the struct*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is allocating the member names of the struct*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        /* member 1 */
        STRICT_EXPECTED_CALL(mocks, Schema_GetStructTypePropertyByIndex(TEST_STRUCT_1_HANDLE, 0))
            .SetReturn((SCHEMA_PROPERTY_HANDLE)NULL);
        STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TEST_COMMANDS_ROOT_NODE));

        // act
        auto result = CommandDecoder_ExecuteCommand(commandDecoderHandle, TEST_COMMAND);

        // assert
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CommandDecoder_Destroy(commandDecoderHandle);
    }

    /* Tests_SRS_COMMAND_DECODER_99_010:[ If any Schema API fails then the command shall not be dispatched and it shall return EXECUTE_COMMAND_ERROR.]*/
    TEST_FUNCTION(CommandDecoder_When_Getting_The_Struct_Member_Name_Fails_Then_ExecuteCommand_Fails)
    {
        // arrange
        CCommandDecoderMocks mocks;
        COMMAND_DECODER_HANDLE commandDecoderHandle = CommandDecoder_Create(TEST_MODEL_HANDLE, ActionCallbackMock, TEST_CALLBACK_CONTEXT_VALUE);
        mocks.ResetAllCalls();
        
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_COMMAND) + 1)); /*this creates a copy of the command that is given to JSON decoder*/
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*free-ing the copy of the buffer*/
            .IgnoreArgument(1);

        SetupCommand(&mocks, quotedSetACStateName, setACStateName);
        size_t argCount = 1;
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionArgumentCount(SetLocationActionHandle, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &argCount, sizeof(argCount));
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is allocating memory for the argument array*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        SetupArgumentCalls(&mocks, SetACStateActionHandle, 0, LocationActionArgument, LocationActionArgument_Name, LocationActionArgument_Type);
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetChildByName(TEST_COMMAND_ARGS_NODE, "Location", IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(3, &TEST_ARG1_NODE, sizeof(TEST_ARG1_NODE));
        STRICT_EXPECTED_CALL(mocks, CodeFirst_GetPrimitiveType("GeoLocation"))
            .SetReturn(EDM_NO_TYPE);
        STRICT_EXPECTED_CALL(mocks, Schema_GetStructTypeByName(TEST_SCHEMA_HANDLE, "GeoLocation"))
            .SetReturn(TEST_STRUCT_1_HANDLE);
        size_t memberCount = 1;
        STRICT_EXPECTED_CALL(mocks, Schema_GetStructTypePropertyCount(TEST_STRUCT_1_HANDLE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &memberCount, sizeof(memberCount));
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is allocating the member values of the struct*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is allocating the member names of the struct*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        /* member 1 */
        STRICT_EXPECTED_CALL(mocks, Schema_GetStructTypePropertyByIndex(TEST_STRUCT_1_HANDLE, 0))
            .SetReturn(memberProperty1);
        STRICT_EXPECTED_CALL(mocks, Schema_GetPropertyName(memberProperty1))
            .SetReturn((const char*)NULL);
        STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TEST_COMMANDS_ROOT_NODE));

        // act
        auto result = CommandDecoder_ExecuteCommand(commandDecoderHandle, TEST_COMMAND);

        // assert
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CommandDecoder_Destroy(commandDecoderHandle);
    }

    /* Tests_SRS_COMMAND_DECODER_99_010:[ If any Schema API fails then the command shall not be dispatched and it shall return EXECUTE_COMMAND_ERROR.]*/
    TEST_FUNCTION(CommandDecoder_When_Getting_The_Struct_Member_Type_Fails_Then_ExecuteCommand_Fails)
    {
        // arrange
        CCommandDecoderMocks mocks;
        COMMAND_DECODER_HANDLE commandDecoderHandle = CommandDecoder_Create(TEST_MODEL_HANDLE, ActionCallbackMock, TEST_CALLBACK_CONTEXT_VALUE);
        mocks.ResetAllCalls();
        
        
        size_t argCount = 1;
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_COMMAND) + 1)); /*this creates a copy of the command that is given to JSON decoder*/
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*free-ing the copy of the buffer*/
            .IgnoreArgument(1);

        SetupCommand(&mocks, quotedSetACStateName, setACStateName);
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionArgumentCount(SetLocationActionHandle, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &argCount, sizeof(argCount));
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is allocating memory for the argument array*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        SetupArgumentCalls(&mocks, SetACStateActionHandle, 0, LocationActionArgument, LocationActionArgument_Name, LocationActionArgument_Type);
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetChildByName(TEST_COMMAND_ARGS_NODE, "Location", IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(3, &TEST_ARG1_NODE, sizeof(TEST_ARG1_NODE));
        STRICT_EXPECTED_CALL(mocks, CodeFirst_GetPrimitiveType("GeoLocation"))
            .SetReturn(EDM_NO_TYPE);
        STRICT_EXPECTED_CALL(mocks, Schema_GetStructTypeByName(TEST_SCHEMA_HANDLE, "GeoLocation"))
            .SetReturn(TEST_STRUCT_1_HANDLE);
        size_t memberCount = 1;
        STRICT_EXPECTED_CALL(mocks, Schema_GetStructTypePropertyCount(TEST_STRUCT_1_HANDLE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &memberCount, sizeof(memberCount));
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is allocating the member values of the struct*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is allocating the member names of the struct*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        /* member 1 */
        STRICT_EXPECTED_CALL(mocks, Schema_GetStructTypePropertyByIndex(TEST_STRUCT_1_HANDLE, 0))
            .SetReturn(memberProperty1);
        STRICT_EXPECTED_CALL(mocks, Schema_GetPropertyName(memberProperty1))
            .SetReturn("Lat");
        STRICT_EXPECTED_CALL(mocks, Schema_GetPropertyType(memberProperty1))
            .SetReturn((const char*)NULL);
        STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TEST_COMMANDS_ROOT_NODE));

        // act
        auto result = CommandDecoder_ExecuteCommand(commandDecoderHandle, TEST_COMMAND);

        // assert
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CommandDecoder_Destroy(commandDecoderHandle);
    }

    /* Tests_SRS_COMMAND_DECODER_01_015: [If any MultiTree API call fails then the processing shall stop and the command shall not be dispatched and it shall return EXECUTE_COMMAND_ERROR.] */
    TEST_FUNCTION(CommandDecoder_When_Getting_The_Child_Node_For_A_Member_Property_For_A_Complex_Type_Fails_Then_ExecuteCommand_Fails)
    {
        // arrange
        CCommandDecoderMocks mocks;
        COMMAND_DECODER_HANDLE commandDecoderHandle = CommandDecoder_Create(TEST_MODEL_HANDLE, ActionCallbackMock, TEST_CALLBACK_CONTEXT_VALUE);
        mocks.ResetAllCalls();
        
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_COMMAND) + 1)); /*this creates a copy of the command that is given to JSON decoder*/
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*free-ing the copy of the buffer*/
            .IgnoreArgument(1);

        SetupCommand(&mocks, quotedSetACStateName, setACStateName);
        size_t argCount = 1;
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionArgumentCount(SetLocationActionHandle, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &argCount, sizeof(argCount));
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is allocating memory for the argument array*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        SetupArgumentCalls(&mocks, SetACStateActionHandle, 0, LocationActionArgument, LocationActionArgument_Name, LocationActionArgument_Type);
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetChildByName(TEST_COMMAND_ARGS_NODE, "Location", IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(3, &TEST_ARG1_NODE, sizeof(TEST_ARG1_NODE));
        STRICT_EXPECTED_CALL(mocks, CodeFirst_GetPrimitiveType("GeoLocation"))
            .SetReturn(EDM_NO_TYPE);
        STRICT_EXPECTED_CALL(mocks, Schema_GetStructTypeByName(TEST_SCHEMA_HANDLE, "GeoLocation"))
            .SetReturn(TEST_STRUCT_1_HANDLE);
        size_t memberCount = 1;
        STRICT_EXPECTED_CALL(mocks, Schema_GetStructTypePropertyCount(TEST_STRUCT_1_HANDLE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &memberCount, sizeof(memberCount));
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is allocating the member values of the struct*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is allocating the member names of the struct*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        /* member 1 */
        STRICT_EXPECTED_CALL(mocks, Schema_GetStructTypePropertyByIndex(TEST_STRUCT_1_HANDLE, 0))
            .SetReturn(memberProperty1);
        STRICT_EXPECTED_CALL(mocks, Schema_GetPropertyName(memberProperty1))
            .SetReturn("Lat");
        STRICT_EXPECTED_CALL(mocks, Schema_GetPropertyType(memberProperty1))
            .SetReturn("double");
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetChildByName(TEST_ARG1_NODE, "Lat", IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(3, &TEST_MEMBER1_NODE, sizeof(TEST_MEMBER1_NODE))
            .SetReturn(MULTITREE_INVALID_ARG);
        STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TEST_COMMANDS_ROOT_NODE));

        // act
        auto result = CommandDecoder_ExecuteCommand(commandDecoderHandle, TEST_COMMAND);

        // assert
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CommandDecoder_Destroy(commandDecoderHandle);
    }

    /* Tests_SRS_COMMAND_DECODER_01_015: [If any MultiTree API call fails then the processing shall stop and the command shall not be dispatched and it shall return EXECUTE_COMMAND_ERROR.] */
    TEST_FUNCTION(CommandDecoder_When_Getting_The_Child_Value_For_A_Member_Property_For_A_Complex_Type_Fails_Then_ExecuteCommand_Fails)
    {
        // arrange
        CCommandDecoderMocks mocks;
        COMMAND_DECODER_HANDLE commandDecoderHandle = CommandDecoder_Create(TEST_MODEL_HANDLE, ActionCallbackMock, TEST_CALLBACK_CONTEXT_VALUE);
        mocks.ResetAllCalls();
        
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_COMMAND) + 1)); /*this creates a copy of the command that is given to JSON decoder*/
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*free-ing the copy of the buffer*/
            .IgnoreArgument(1);

        SetupCommand(&mocks, quotedSetACStateName, setACStateName);
        size_t argCount = 1;
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionArgumentCount(SetLocationActionHandle, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &argCount, sizeof(argCount));
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is allocating memory for the argument array*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        SetupArgumentCalls(&mocks, SetACStateActionHandle, 0, LocationActionArgument, LocationActionArgument_Name, LocationActionArgument_Type);
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetChildByName(TEST_COMMAND_ARGS_NODE, "Location", IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(3, &TEST_ARG1_NODE, sizeof(TEST_ARG1_NODE));
        STRICT_EXPECTED_CALL(mocks, CodeFirst_GetPrimitiveType("GeoLocation"))
            .SetReturn(EDM_NO_TYPE);
        STRICT_EXPECTED_CALL(mocks, Schema_GetStructTypeByName(TEST_SCHEMA_HANDLE, "GeoLocation"))
            .SetReturn(TEST_STRUCT_1_HANDLE);
        size_t memberCount = 1;
        STRICT_EXPECTED_CALL(mocks, Schema_GetStructTypePropertyCount(TEST_STRUCT_1_HANDLE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &memberCount, sizeof(memberCount));
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is allocating the member values of the struct*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is allocating the member names of the struct*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        /* member 1 */
        STRICT_EXPECTED_CALL(mocks, Schema_GetStructTypePropertyByIndex(TEST_STRUCT_1_HANDLE, 0))
            .SetReturn(memberProperty1);
        STRICT_EXPECTED_CALL(mocks, Schema_GetPropertyName(memberProperty1))
            .SetReturn("Lat");
        STRICT_EXPECTED_CALL(mocks, Schema_GetPropertyType(memberProperty1))
            .SetReturn("double");
        STRICT_EXPECTED_CALL(mocks, CodeFirst_GetPrimitiveType("double"))
            .SetReturn(EDM_DOUBLE_TYPE);
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetChildByName(TEST_ARG1_NODE, "Lat", IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(3, &TEST_MEMBER1_NODE, sizeof(TEST_MEMBER1_NODE));
        const char* latValue = "42.42";
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetValue(TEST_MEMBER1_NODE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &latValue, sizeof(latValue))
            .SetReturn(MULTITREE_INVALID_ARG);
        STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TEST_COMMANDS_ROOT_NODE));

        // act
        auto result = CommandDecoder_ExecuteCommand(commandDecoderHandle, TEST_COMMAND);

        // assert
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CommandDecoder_Destroy(commandDecoderHandle);
    }

    /* Tests_SRS_COMMAND_DECODER_99_028:[ If decoding the argument fails then the command shall not be dispatched and it shall return EXECUTE_COMMAND_ERROR.] */
    TEST_FUNCTION(CommandDecoder_When_Creating_The_Agent_Data_Type_Value_For_A_Member_Property_For_A_Complex_Type_Fails_Then_ExecuteCommand_Fails)
    {
        // arrange
        CCommandDecoderMocks mocks;
        COMMAND_DECODER_HANDLE commandDecoderHandle = CommandDecoder_Create(TEST_MODEL_HANDLE, ActionCallbackMock, TEST_CALLBACK_CONTEXT_VALUE);
        mocks.ResetAllCalls();
        
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_COMMAND) + 1)); /*this creates a copy of the command that is given to JSON decoder*/
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*free-ing the copy of the buffer*/
            .IgnoreArgument(1);

        SetupCommand(&mocks, quotedSetACStateName, setACStateName);
        size_t argCount = 1;
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionArgumentCount(SetLocationActionHandle, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &argCount, sizeof(argCount));
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is allocating memory for the argument array*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        SetupArgumentCalls(&mocks, SetACStateActionHandle, 0, LocationActionArgument, LocationActionArgument_Name, LocationActionArgument_Type);
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetChildByName(TEST_COMMAND_ARGS_NODE, "Location", IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(3, &TEST_ARG1_NODE, sizeof(TEST_ARG1_NODE));
        STRICT_EXPECTED_CALL(mocks, CodeFirst_GetPrimitiveType("GeoLocation"))
            .SetReturn(EDM_NO_TYPE);
        STRICT_EXPECTED_CALL(mocks, Schema_GetStructTypeByName(TEST_SCHEMA_HANDLE, "GeoLocation"))
            .SetReturn(TEST_STRUCT_1_HANDLE);
        size_t memberCount = 1;
        STRICT_EXPECTED_CALL(mocks, Schema_GetStructTypePropertyCount(TEST_STRUCT_1_HANDLE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &memberCount, sizeof(memberCount));
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is allocating the member values of the struct*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is allocating the member names of the struct*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        /* member 1 */
        STRICT_EXPECTED_CALL(mocks, Schema_GetStructTypePropertyByIndex(TEST_STRUCT_1_HANDLE, 0))
            .SetReturn(memberProperty1);
        STRICT_EXPECTED_CALL(mocks, Schema_GetPropertyName(memberProperty1))
            .SetReturn("Lat");
        STRICT_EXPECTED_CALL(mocks, Schema_GetPropertyType(memberProperty1))
            .SetReturn("double");
        STRICT_EXPECTED_CALL(mocks, CodeFirst_GetPrimitiveType("double"))
            .SetReturn(EDM_DOUBLE_TYPE);
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetChildByName(TEST_ARG1_NODE, "Lat", IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(3, &TEST_MEMBER1_NODE, sizeof(TEST_MEMBER1_NODE));
        const char* latValue = "42.42";
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetValue(TEST_MEMBER1_NODE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &latValue, sizeof(latValue));
        STRICT_EXPECTED_CALL(mocks, CreateAgentDataType_From_String(latValue, EDM_DOUBLE_TYPE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(3, &LatAgentDataType, sizeof(LatAgentDataType))
            .SetReturn(AGENT_DATA_TYPES_INVALID_ARG);
        STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TEST_COMMANDS_ROOT_NODE));

        // act
        auto result = CommandDecoder_ExecuteCommand(commandDecoderHandle, TEST_COMMAND);

        // assert
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CommandDecoder_Destroy(commandDecoderHandle);
    }

    /* Tests_SRS_COMMAND_DECODER_99_028:[ If decoding the argument fails then the command shall not be dispatched and it shall return EXECUTE_COMMAND_ERROR.] */
    TEST_FUNCTION(CommandDecoder_When_Creating_The_Complex_Type_Agent_Data_Type_Value_For_A_Complex_Type_Fails_Then_ExecuteCommand_Fails)
    {
        // arrange
        CCommandDecoderMocks mocks;
        COMMAND_DECODER_HANDLE commandDecoderHandle = CommandDecoder_Create(TEST_MODEL_HANDLE, ActionCallbackMock, TEST_CALLBACK_CONTEXT_VALUE);
        mocks.ResetAllCalls();
        
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_COMMAND) + 1)); /*this creates a copy of the command that is given to JSON decoder*/
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*free-ing the copy of the buffer*/
            .IgnoreArgument(1);

        SetupCommand(&mocks, quotedSetACStateName, setACStateName);
        size_t argCount = 1;
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionArgumentCount(SetLocationActionHandle, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &argCount, sizeof(argCount));
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is allocating memory for the argument array*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        SetupArgumentCalls(&mocks, SetACStateActionHandle, 0, LocationActionArgument, LocationActionArgument_Name, LocationActionArgument_Type);
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetChildByName(TEST_COMMAND_ARGS_NODE, "Location", IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(3, &TEST_ARG1_NODE, sizeof(TEST_ARG1_NODE));
        STRICT_EXPECTED_CALL(mocks, CodeFirst_GetPrimitiveType("GeoLocation"))
            .SetReturn(EDM_NO_TYPE);
        STRICT_EXPECTED_CALL(mocks, Schema_GetStructTypeByName(TEST_SCHEMA_HANDLE, "GeoLocation"))
            .SetReturn(TEST_STRUCT_1_HANDLE);
        size_t memberCount = 1;
        STRICT_EXPECTED_CALL(mocks, Schema_GetStructTypePropertyCount(TEST_STRUCT_1_HANDLE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &memberCount, sizeof(memberCount));
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is allocating the member values of the struct*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is allocating the member names of the struct*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        /* member 1 */
        STRICT_EXPECTED_CALL(mocks, Schema_GetStructTypePropertyByIndex(TEST_STRUCT_1_HANDLE, 0))
            .SetReturn(memberProperty1);
        STRICT_EXPECTED_CALL(mocks, Schema_GetPropertyName(memberProperty1))
            .SetReturn("Lat");
        STRICT_EXPECTED_CALL(mocks, Schema_GetPropertyType(memberProperty1))
            .SetReturn("double");
        STRICT_EXPECTED_CALL(mocks, CodeFirst_GetPrimitiveType("double"))
            .SetReturn(EDM_DOUBLE_TYPE);
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetChildByName(TEST_ARG1_NODE, "Lat", IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(3, &TEST_MEMBER1_NODE, sizeof(TEST_MEMBER1_NODE));
        const char* latValue = "42.42";
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetValue(TEST_MEMBER1_NODE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &latValue, sizeof(latValue));
        STRICT_EXPECTED_CALL(mocks, CreateAgentDataType_From_String(latValue, EDM_DOUBLE_TYPE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(3, &LatAgentDataType, sizeof(LatAgentDataType));

        EXPECTED_CALL(mocks, Create_AGENT_DATA_TYPE_from_Members(IGNORED_PTR_ARG, "GeoLocation", 1, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .ValidateArgument(2).ValidateArgument(3)
            .SetReturn(AGENT_DATA_TYPES_INVALID_ARG);

        EXPECTED_CALL(mocks, Destroy_AGENT_DATA_TYPE(IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TEST_COMMANDS_ROOT_NODE));

        // act
        auto result = CommandDecoder_ExecuteCommand(commandDecoderHandle, TEST_COMMAND);

        // assert
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CommandDecoder_Destroy(commandDecoderHandle);
    }

    /* Tests_SRS_COMMAND_DECODER_99_028:[ If decoding the argument fails then the command shall not be dispatched and it shall return EXECUTE_COMMAND_ERROR.] */
    TEST_FUNCTION(CommandDecoder_When_Decoding_The_2nd_Member_Of_A_Complex_Property_Fails_Execute_Command_Frees_The_Previously_Allocated_Member)
    {
        // arrange
        CCommandDecoderMocks mocks;
        COMMAND_DECODER_HANDLE commandDecoderHandle = CommandDecoder_Create(TEST_MODEL_HANDLE, ActionCallbackMock, TEST_CALLBACK_CONTEXT_VALUE);
        mocks.ResetAllCalls();
        
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_COMMAND) + 1)); /*this creates a copy of the command that is given to JSON decoder*/
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*free-ing the copy of the buffer*/
            .IgnoreArgument(1);

        SetupCommand(&mocks, quotedSetACStateName, setACStateName);
        size_t argCount = 1;
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionArgumentCount(SetLocationActionHandle, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &argCount, sizeof(argCount));
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is allocating memory for the argument array*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        SetupArgumentCalls(&mocks, SetACStateActionHandle, 0, LocationActionArgument, LocationActionArgument_Name, LocationActionArgument_Type);
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetChildByName(TEST_COMMAND_ARGS_NODE, "Location", IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(3, &TEST_ARG1_NODE, sizeof(TEST_ARG1_NODE));
        STRICT_EXPECTED_CALL(mocks, CodeFirst_GetPrimitiveType("GeoLocation"))
            .SetReturn(EDM_NO_TYPE);
        STRICT_EXPECTED_CALL(mocks, Schema_GetStructTypeByName(TEST_SCHEMA_HANDLE, "GeoLocation"))
            .SetReturn(TEST_STRUCT_1_HANDLE);
        size_t memberCount = 2;
        STRICT_EXPECTED_CALL(mocks, Schema_GetStructTypePropertyCount(TEST_STRUCT_1_HANDLE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &memberCount, sizeof(memberCount));
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is allocating the member values of the struct*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is allocating the member names of the struct*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        /* member 1 */
        STRICT_EXPECTED_CALL(mocks, Schema_GetStructTypePropertyByIndex(TEST_STRUCT_1_HANDLE, 0))
            .SetReturn(memberProperty1);
        STRICT_EXPECTED_CALL(mocks, Schema_GetPropertyName(memberProperty1))
            .SetReturn("Lat");
        STRICT_EXPECTED_CALL(mocks, Schema_GetPropertyType(memberProperty1))
            .SetReturn("double");
        STRICT_EXPECTED_CALL(mocks, CodeFirst_GetPrimitiveType("double"))
            .SetReturn(EDM_DOUBLE_TYPE);
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetChildByName(TEST_ARG1_NODE, "Lat", IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(3, &TEST_MEMBER1_NODE, sizeof(TEST_MEMBER1_NODE));
        const char* latValue = "42.42";
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetValue(TEST_MEMBER1_NODE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &latValue, sizeof(latValue));
        STRICT_EXPECTED_CALL(mocks, CreateAgentDataType_From_String(latValue, EDM_DOUBLE_TYPE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(3, &LatAgentDataType, sizeof(LatAgentDataType));

        /* member 2 */
        STRICT_EXPECTED_CALL(mocks, Schema_GetStructTypePropertyByIndex(TEST_STRUCT_1_HANDLE, 1))
            .SetReturn((SCHEMA_PROPERTY_HANDLE)NULL);

        EXPECTED_CALL(mocks, Destroy_AGENT_DATA_TYPE(IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TEST_COMMANDS_ROOT_NODE));

        // act
        auto result = CommandDecoder_ExecuteCommand(commandDecoderHandle, TEST_COMMAND);

        // assert
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CommandDecoder_Destroy(commandDecoderHandle);
    }

    /* Tests_SRS_COMMAND_DECODER_99_032:[ Nesting shall be supported for complex type.] */
    TEST_FUNCTION(CommandDecoder_Decoding_An_Action_With_Nested_Complex_Types_Succeeds)
    {
        // arrange
        CCommandDecoderMocks mocks;
        COMMAND_DECODER_HANDLE commandDecoderHandle = CommandDecoder_Create(TEST_MODEL_HANDLE, ActionCallbackMock, TEST_CALLBACK_CONTEXT_VALUE);
        mocks.ResetAllCalls();
        
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_COMMAND) + 1)); /*this creates a copy of the command that is given to JSON decoder*/
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*free-ing the copy of the buffer*/
            .IgnoreArgument(1);

        SetupCommand(&mocks, quotedSetLocationName, setLocationName);
        size_t argCount = 1;
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionArgumentCount(SetLocationActionHandle, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &argCount, sizeof(argCount));
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is allocating memory for the argument array*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        SetupArgumentCalls(&mocks, SetACStateActionHandle, 0, LocationActionArgument, LocationActionArgument_Name, LocationActionArgument_Type);
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetChildByName(TEST_COMMAND_ARGS_NODE, "Location", IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(3, &TEST_ARG1_NODE, sizeof(TEST_ARG1_NODE));
        STRICT_EXPECTED_CALL(mocks, CodeFirst_GetPrimitiveType("GeoLocation"))
            .SetReturn(EDM_NO_TYPE);
        STRICT_EXPECTED_CALL(mocks, Schema_GetStructTypeByName(TEST_SCHEMA_HANDLE, "GeoLocation"))
            .SetReturn(TEST_STRUCT_1_HANDLE);
        size_t memberCount = 1;
        STRICT_EXPECTED_CALL(mocks, Schema_GetStructTypePropertyCount(TEST_STRUCT_1_HANDLE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &memberCount, sizeof(memberCount));
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is allocating the member values of the struct*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is allocating the member names of the struct*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        /* member 1, nested complex type */
        STRICT_EXPECTED_CALL(mocks, Schema_GetStructTypePropertyByIndex(TEST_STRUCT_1_HANDLE, 0))
            .SetReturn(memberNestedComplexTypeProperty);
        STRICT_EXPECTED_CALL(mocks, Schema_GetPropertyName(memberNestedComplexTypeProperty))
            .SetReturn("NestedLocation");
        STRICT_EXPECTED_CALL(mocks, Schema_GetPropertyType(memberNestedComplexTypeProperty))
            .SetReturn("NestedGeoLocation");
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetChildByName(TEST_ARG1_NODE, "NestedLocation", IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(3, &TEST_NESTED_STRUCT_NODE, sizeof(TEST_NESTED_STRUCT_NODE));
        STRICT_EXPECTED_CALL(mocks, CodeFirst_GetPrimitiveType("NestedGeoLocation"))
            .SetReturn(EDM_NO_TYPE);
        STRICT_EXPECTED_CALL(mocks, Schema_GetStructTypeByName(TEST_SCHEMA_HANDLE, "NestedGeoLocation"))
            .SetReturn(TEST_STRUCT_2_HANDLE);
        memberCount = 2;
        STRICT_EXPECTED_CALL(mocks, Schema_GetStructTypePropertyCount(TEST_STRUCT_2_HANDLE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &memberCount, sizeof(memberCount));
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is allocating the member values of the struct*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is allocating the member names of the struct*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        /* member 1, nested */
        STRICT_EXPECTED_CALL(mocks, Schema_GetStructTypePropertyByIndex(TEST_STRUCT_2_HANDLE, 0))
            .SetReturn(memberProperty1);
        STRICT_EXPECTED_CALL(mocks, Schema_GetPropertyName(memberProperty1))
            .SetReturn("Lat");
        STRICT_EXPECTED_CALL(mocks, Schema_GetPropertyType(memberProperty1))
            .SetReturn("double");
        STRICT_EXPECTED_CALL(mocks, CodeFirst_GetPrimitiveType("double"))
            .SetReturn(EDM_DOUBLE_TYPE);
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetChildByName(TEST_NESTED_STRUCT_NODE, "Lat", IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(3, &TEST_MEMBER1_NODE, sizeof(TEST_MEMBER1_NODE));
        const char* latValue = "42.42";
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetValue(TEST_MEMBER1_NODE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &latValue, sizeof(latValue));
        STRICT_EXPECTED_CALL(mocks, CreateAgentDataType_From_String(latValue, EDM_DOUBLE_TYPE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(3, &LatAgentDataType, sizeof(LatAgentDataType));

        /* member 2, nested */
        STRICT_EXPECTED_CALL(mocks, Schema_GetStructTypePropertyByIndex(TEST_STRUCT_2_HANDLE, 1))
            .SetReturn(memberProperty2);
        STRICT_EXPECTED_CALL(mocks, Schema_GetPropertyName(memberProperty2))
            .SetReturn("Long");
        STRICT_EXPECTED_CALL(mocks, Schema_GetPropertyType(memberProperty2))
            .SetReturn("double");
        STRICT_EXPECTED_CALL(mocks, CodeFirst_GetPrimitiveType("double"))
            .SetReturn(EDM_DOUBLE_TYPE);
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetChildByName(TEST_NESTED_STRUCT_NODE, "Long", IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(3, &TEST_MEMBER2_NODE, sizeof(TEST_MEMBER2_NODE));
        const char* longValue = "1.2";
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetValue(TEST_MEMBER2_NODE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &longValue, sizeof(longValue));
        STRICT_EXPECTED_CALL(mocks, CreateAgentDataType_From_String(longValue, EDM_DOUBLE_TYPE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(3, &LongAgentDataType, sizeof(LongAgentDataType));

        EXPECTED_CALL(mocks, Create_AGENT_DATA_TYPE_from_Members(IGNORED_PTR_ARG, "NestedGeoLocation", 2, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .ValidateArgument(2).ValidateArgument(3);

        EXPECTED_CALL(mocks, Create_AGENT_DATA_TYPE_from_Members(IGNORED_PTR_ARG, "GeoLocation", 1, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .ValidateArgument(2).ValidateArgument(3);

        STRICT_EXPECTED_CALL(mocks, ActionCallbackMock(TEST_CALLBACK_CONTEXT_VALUE, "", "SetLocation", 1, IGNORED_PTR_ARG))
            .IgnoreArgument(5);

        EXPECTED_CALL(mocks, Destroy_AGENT_DATA_TYPE(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, Destroy_AGENT_DATA_TYPE(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, Destroy_AGENT_DATA_TYPE(IGNORED_PTR_ARG));
        EXPECTED_CALL(mocks, Destroy_AGENT_DATA_TYPE(IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TEST_COMMANDS_ROOT_NODE));

        // act
        auto result = CommandDecoder_ExecuteCommand(commandDecoderHandle, TEST_COMMAND);

        // assert
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_SUCCESS, result);
        mocks.AssertActualAndExpectedCalls();
        ASSERT_ARE_EQUAL(char_ptr, "Lat", lastMemberNames[0][0]);
        ASSERT_ARE_EQUAL(char_ptr, "Long", lastMemberNames[0][1]);
        ASSERT_ARE_EQUAL(char_ptr, "NestedLocation", lastMemberNames[1][0]);

        // cleanup
        CommandDecoder_Destroy(commandDecoderHandle);
    }

    /* Tests_SRS_COMMAND_DECODER_99_035:[ CommandDecoder_ExecuteCommand shall support paths to actions that are in child models (i.e. ChildModel/SomeAction.] */
    TEST_FUNCTION(CommandDecoder_ExecuteCommand_With_A_Command_In_A_Child_Model_Calls_The_ActionCallback)
    {
        // arrange
        CCommandDecoderMocks mocks;
        COMMAND_DECODER_HANDLE commandDecoderHandle = CommandDecoder_Create(TEST_MODEL_HANDLE, ActionCallbackMock, TEST_CALLBACK_CONTEXT_VALUE);
        mocks.ResetAllCalls();

        const char* quotedActionName = "\"ChildModel/SetACState\"";

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_COMMAND) + 1)); /*this creates a copy of the command that is given to JSON decoder*/
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*free-ing the copy of the buffer*/
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, JSONDecoder_JSON_To_MultiTree(TestCommand, IGNORED_PTR_ARG)).IgnoreArgument(2);
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetChildByName(TEST_COMMAND_ROOT_NODE, "Name", IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(3, &TEST_COMMAND_NAME_NODE, sizeof(TEST_COMMAND_NAME_NODE));
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetValue(TEST_COMMAND_NAME_NODE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &quotedActionName, sizeof(quotedActionName));
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is relativeActionPath*/ /*well - first part of it ="ChildModel"*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is relativeActionPath*/ /*well - last part of it ="SetACState"*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, Schema_GetSchemaForModelType(TEST_MODEL_HANDLE));
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetChildByName(TEST_COMMAND_ROOT_NODE, "Parameters", IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(3, &TEST_COMMAND_ARGS_NODE, sizeof(TEST_COMMAND_ARGS_NODE));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelModelByName(TEST_MODEL_HANDLE, "ChildModel"));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionByName(TEST_CHILD_MODEL_HANDLE, "SetACState"))
            .SetReturn(SetACStateActionHandle);
        size_t argCount = 0;
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionArgumentCount(SetACStateActionHandle, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &argCount, sizeof(argCount));
        STRICT_EXPECTED_CALL(mocks, ActionCallbackMock(TEST_CALLBACK_CONTEXT_VALUE, "ChildModel", "SetACState", 0, NULL));
        STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TEST_COMMANDS_ROOT_NODE));

        // act
        auto result = CommandDecoder_ExecuteCommand(commandDecoderHandle, TEST_COMMAND);

        // assert
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_SUCCESS, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CommandDecoder_Destroy(commandDecoderHandle);
    }

    /* Tests_SRS_COMMAND_DECODER_99_035:[ CommandDecoder_ExecuteCommand shall support paths to actions that are in child models (i.e. ChildModel/SomeAction.] */
    TEST_FUNCTION(CommandDecoder_ExecuteCommand_With_A_Command_In_A_Child_Model_Calls_The_ActionCallback_and_returns_REJECTED)
    {
        // arrange
        CCommandDecoderMocks mocks;
        COMMAND_DECODER_HANDLE commandDecoderHandle = CommandDecoder_Create(TEST_MODEL_HANDLE, ActionCallbackMock, TEST_CALLBACK_CONTEXT_VALUE);
        mocks.ResetAllCalls();

        const char* quotedActionName = "\"ChildModel/SetACState\"";

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_COMMAND) + 1)); /*this creates a copy of the command that is given to JSON decoder*/
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*free-ing the copy of the buffer*/
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, JSONDecoder_JSON_To_MultiTree(TestCommand, IGNORED_PTR_ARG)).IgnoreArgument(2);
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetChildByName(TEST_COMMAND_ROOT_NODE, "Name", IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(3, &TEST_COMMAND_NAME_NODE, sizeof(TEST_COMMAND_NAME_NODE));
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetValue(TEST_COMMAND_NAME_NODE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &quotedActionName, sizeof(quotedActionName));
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is relativeActionPath*/ /*well - first part of it ="ChildModel"*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is relativeActionPath*/ /*well - last part of it ="SetACState"*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, Schema_GetSchemaForModelType(TEST_MODEL_HANDLE));
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetChildByName(TEST_COMMAND_ROOT_NODE, "Parameters", IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(3, &TEST_COMMAND_ARGS_NODE, sizeof(TEST_COMMAND_ARGS_NODE));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelModelByName(TEST_MODEL_HANDLE, "ChildModel"));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionByName(TEST_CHILD_MODEL_HANDLE, "SetACState"))
            .SetReturn(SetACStateActionHandle);
        size_t argCount = 0;
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionArgumentCount(SetACStateActionHandle, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &argCount, sizeof(argCount));
        STRICT_EXPECTED_CALL(mocks, ActionCallbackMock(TEST_CALLBACK_CONTEXT_VALUE, "ChildModel", "SetACState", 0, NULL))
            .SetReturn(EXECUTE_COMMAND_FAILED);
        STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TEST_COMMANDS_ROOT_NODE));

        // act
        auto result = CommandDecoder_ExecuteCommand(commandDecoderHandle, TEST_COMMAND);

        // assert
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_FAILED, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CommandDecoder_Destroy(commandDecoderHandle);
    }

    /* Tests_SRS_COMMAND_DECODER_99_035:[ CommandDecoder_ExecuteCommand shall support paths to actions that are in child models (i.e. ChildModel/SomeAction.] */
    TEST_FUNCTION(CommandDecoder_ExecuteCommand_With_A_Command_In_A_Child_Model_Calls_The_ActionCallback_and_returns_ABANDONED)
    {
        // arrange
        CCommandDecoderMocks mocks;
        COMMAND_DECODER_HANDLE commandDecoderHandle = CommandDecoder_Create(TEST_MODEL_HANDLE, ActionCallbackMock, TEST_CALLBACK_CONTEXT_VALUE);
        mocks.ResetAllCalls();

        const char* quotedActionName = "\"ChildModel/SetACState\"";

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_COMMAND) + 1)); /*this creates a copy of the command that is given to JSON decoder*/
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*free-ing the copy of the buffer*/
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, JSONDecoder_JSON_To_MultiTree(TestCommand, IGNORED_PTR_ARG)).IgnoreArgument(2);
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetChildByName(TEST_COMMAND_ROOT_NODE, "Name", IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(3, &TEST_COMMAND_NAME_NODE, sizeof(TEST_COMMAND_NAME_NODE));
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetValue(TEST_COMMAND_NAME_NODE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &quotedActionName, sizeof(quotedActionName));
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is relativeActionPath*/ /*well - first part of it ="ChildModel"*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is relativeActionPath*/ /*well - last part of it ="SetACState"*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, Schema_GetSchemaForModelType(TEST_MODEL_HANDLE));
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetChildByName(TEST_COMMAND_ROOT_NODE, "Parameters", IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(3, &TEST_COMMAND_ARGS_NODE, sizeof(TEST_COMMAND_ARGS_NODE));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelModelByName(TEST_MODEL_HANDLE, "ChildModel"));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionByName(TEST_CHILD_MODEL_HANDLE, "SetACState"))
            .SetReturn(SetACStateActionHandle);
        size_t argCount = 0;
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionArgumentCount(SetACStateActionHandle, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &argCount, sizeof(argCount));
        STRICT_EXPECTED_CALL(mocks, ActionCallbackMock(TEST_CALLBACK_CONTEXT_VALUE, "ChildModel", "SetACState", 0, NULL))
            .SetReturn(EXECUTE_COMMAND_ERROR);
        STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TEST_COMMANDS_ROOT_NODE));

        // act
        auto result = CommandDecoder_ExecuteCommand(commandDecoderHandle, TEST_COMMAND);

        // assert
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CommandDecoder_Destroy(commandDecoderHandle);
    }

    /*Tests_SRS_COMMAND_DECODER_99_021:[ If the parsing of the command fails for any other reason the command shall not be dispatched.]*/
    TEST_FUNCTION(CommandDecoder_ExecuteCommand_With_A_Command_In_A_Child_Model_when_gballoc_fails_it_fails)
    {
        // arrange
        CCommandDecoderMocks mocks;
        COMMAND_DECODER_HANDLE commandDecoderHandle = CommandDecoder_Create(TEST_MODEL_HANDLE, ActionCallbackMock, TEST_CALLBACK_CONTEXT_VALUE);
        mocks.ResetAllCalls();

        const char* quotedActionName = "\"ChildModel/SetACState\"";

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_COMMAND) + 1)); /*this creates a copy of the command that is given to JSON decoder*/
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*free-ing the copy of the buffer*/
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, JSONDecoder_JSON_To_MultiTree(TestCommand, IGNORED_PTR_ARG)).IgnoreArgument(2);
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetChildByName(TEST_COMMAND_ROOT_NODE, "Name", IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(3, &TEST_COMMAND_NAME_NODE, sizeof(TEST_COMMAND_NAME_NODE));
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetValue(TEST_COMMAND_NAME_NODE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &quotedActionName, sizeof(quotedActionName));
        whenShallmalloc_fail = currentmalloc_call + 2;
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is relativeActionPath*/ /*well - first part of it ="ChildModel"*/
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(mocks, Schema_GetSchemaForModelType(TEST_MODEL_HANDLE));
        STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TEST_COMMANDS_ROOT_NODE));

        ASSERT_IS_NOT_NULL(CommandDecoder_ExecuteCommand);

        // act
        auto result = CommandDecoder_ExecuteCommand(commandDecoderHandle, TEST_COMMAND);

        // assert
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CommandDecoder_Destroy(commandDecoderHandle);
    }

    /* Tests_SRS_COMMAND_DECODER_99_036:[ If a child model cannot be found by using Schema APIs then the command shall not be dispatched and it shall return EXECUTE_COMMAND_ERROR.] */
    /* Tests_SRS_COMMAND_DECODER_99_037:[ The relative path passed to the actionCallback shall be in the format "childModel1/childModel2/.../childModelN".] */
    TEST_FUNCTION(CommandDecoder_ExecuteCommand_With_A_Command_In_A_Child_Model_And_The_Child_Model_Does_Not_Exist_Fails)
    {
        // arrange
        CCommandDecoderMocks mocks;
        COMMAND_DECODER_HANDLE commandDecoderHandle = CommandDecoder_Create(TEST_MODEL_HANDLE, ActionCallbackMock, TEST_CALLBACK_CONTEXT_VALUE);
        mocks.ResetAllCalls();

        const char* quotedActionName = "\"ChildModel/SetLocation\"";

        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(strlen(TEST_COMMAND) + 1)); /*this creates a copy of the command that is given to JSON decoder*/
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG)) /*free-ing the copy of the buffer*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, JSONDecoder_JSON_To_MultiTree(TestCommand, IGNORED_PTR_ARG)).IgnoreArgument(2);
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetChildByName(TEST_COMMAND_ROOT_NODE, "Name", IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(3, &TEST_COMMAND_NAME_NODE, sizeof(TEST_COMMAND_NAME_NODE));
        STRICT_EXPECTED_CALL(mocks, MultiTree_GetValue(TEST_COMMAND_NAME_NODE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &quotedActionName, sizeof(quotedActionName));
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG)) /*this is relativeActionPath*/ /*well - first part of it ="ChildModel"*/
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
                                                                    /*this is relativeActionPath*/ /*well - notice there's no last part of it not found*/

        STRICT_EXPECTED_CALL(mocks, Schema_GetSchemaForModelType(TEST_MODEL_HANDLE));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelModelByName(TEST_MODEL_HANDLE, "ChildModel"))
            .SetReturn((SCHEMA_MODEL_TYPE_HANDLE)NULL);
        STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TEST_COMMANDS_ROOT_NODE));

        // act
        auto result = CommandDecoder_ExecuteCommand(commandDecoderHandle, TEST_COMMAND);

        // assert
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
        mocks.AssertActualAndExpectedCalls();

        // cleanup
        CommandDecoder_Destroy(commandDecoderHandle);
    }

    END_TEST_SUITE(CommandDecoder_ut)
