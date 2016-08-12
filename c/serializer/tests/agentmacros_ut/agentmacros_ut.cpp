// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <cstdlib>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "testrunnerswitcher.h"
#include "micromock.h"
#include "micromockenumtostring.h"
#include "micromockcharstararenullterminatedstrings.h"
#include "codefirst.h"
#include "azure_c_shared_utility/strings.h"
#include "serializer.h"


DEFINE_MICROMOCK_ENUM_TO_STRING(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_RESULT_VALUES);
DEFINE_MICROMOCK_ENUM_TO_STRING(IOT_AGENT_RESULT, IOT_AGENT_RESULT_ENUM_VALUES);
DEFINE_MICROMOCK_ENUM_TO_STRING(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_RESULT_VALUES);


/* Stub unused functions that just need to link */

AGENT_DATA_TYPES_RESULT Create_EDM_BOOLEAN_from_int(AGENT_DATA_TYPE*, int) { return AGENT_DATA_TYPES_ERROR; }
AGENT_DATA_TYPES_RESULT Create_AGENT_DATA_TYPE_from_UINT8(AGENT_DATA_TYPE*, uint8_t) { return AGENT_DATA_TYPES_ERROR; }
AGENT_DATA_TYPES_RESULT Create_AGENT_DATA_TYPE_from_DOUBLE(AGENT_DATA_TYPE*, double) { return AGENT_DATA_TYPES_ERROR; }
AGENT_DATA_TYPES_RESULT Create_AGENT_DATA_TYPE_from_SINT16(AGENT_DATA_TYPE*, int16_t) { return AGENT_DATA_TYPES_ERROR; }
AGENT_DATA_TYPES_RESULT Create_AGENT_DATA_TYPE_from_SINT64(AGENT_DATA_TYPE*, int64_t) { return AGENT_DATA_TYPES_ERROR; }
AGENT_DATA_TYPES_RESULT Create_AGENT_DATA_TYPE_from_SINT8(AGENT_DATA_TYPE*, int8_t) { return AGENT_DATA_TYPES_ERROR; }
AGENT_DATA_TYPES_RESULT Create_AGENT_DATA_TYPE_from_EDM_DATE_TIME_OFFSET(AGENT_DATA_TYPE*, EDM_DATE_TIME_OFFSET) { return AGENT_DATA_TYPES_ERROR; }
AGENT_DATA_TYPES_RESULT Create_AGENT_DATA_TYPE_from_EDM_GUID(AGENT_DATA_TYPE*, EDM_GUID) { return AGENT_DATA_TYPES_ERROR; }
AGENT_DATA_TYPES_RESULT Create_AGENT_DATA_TYPE_from_EDM_BINARY(AGENT_DATA_TYPE*, EDM_BINARY) { return AGENT_DATA_TYPES_ERROR; }
AGENT_DATA_TYPES_RESULT Create_AGENT_DATA_TYPE_from_FLOAT(AGENT_DATA_TYPE*, float) { return AGENT_DATA_TYPES_ERROR; }

TRANSACTION_HANDLE Device_StartTransaction(DEVICE_HANDLE) { return NULL; }
DEVICE_RESULT Device_PublishTransacted(TRANSACTION_HANDLE, const char*, const AGENT_DATA_TYPE*) { return DEVICE_ERROR; }
DEVICE_RESULT Device_EndTransaction(TRANSACTION_HANDLE) { return DEVICE_ERROR; }
DEVICE_RESULT Device_CancelTransaction(TRANSACTION_HANDLE) { return DEVICE_ERROR; }

static const SCHEMA_HANDLE TEST_SCHEMA_HANDLE = (SCHEMA_HANDLE)0x4242;
static const SCHEMA_MODEL_TYPE_HANDLE TEST_MODEL_HANDLE = (SCHEMA_MODEL_TYPE_HANDLE)0x4243;

namespace BASEIMPLEMENTATION
{
    #include "strings.c"
};

static int bool_Compare(bool left, bool right)
{
    return left != right;
}

static void bool_ToString(char* string, size_t bufferSize, bool val)
{
    (void)bufferSize;
    (void)strcpy(string, val ? "true" : "false");
}


BEGIN_NAMESPACE(schemaWithModelAction)

DECLARE_MODEL(modelWithAction,
    WITH_ACTION(lotsOfAction, double, x, ascii_char_ptr, y)
)

END_NAMESPACE(schemaWithModelAction)

BEGIN_NAMESPACE(schemaWithModel)

DECLARE_MODEL(modelWithEachElement,
    WITH_DATA(int, simpleProperty),
    WITH_ACTION(simpleAction, int, actionArg1)
)

END_NAMESPACE(schemaWithModel);

BEGIN_NAMESPACE(schemaWithSpeed)

DECLARE_MODEL(SimpleDevice,
WITH_DATA(double, Speed),
WITH_DATA(double, moreSpeed)
)

END_NAMESPACE(schemaWithSpeed)

/* WITH_DATA's name argument shall be one of the following data types: */
/* Tests_SRS_SERIALIZER_99_133:[a model type introduced previously by DECLARE_MODEL] */
BEGIN_NAMESPACE(JukeBoxes)

DECLARE_MODEL(Song,
    WITH_DATA(ascii_char_ptr, songName),
    WITH_ACTION(Play))

DECLARE_MODEL(JukeBox,
    WITH_DATA(Song, bestSong),
    WITH_DATA(Song, worstSong),
    WITH_ACTION(Shuffle))

END_NAMESPACE(JukeBoxes)

EXECUTE_COMMAND_RESULT Play(Song* song)
{
    (void)song;
    return EXECUTE_COMMAND_SUCCESS;
}

EXECUTE_COMMAND_RESULT Shuffle(JukeBox* jukeBox)
{
    (void)jukeBox;
    return EXECUTE_COMMAND_SUCCESS;
}

static SimpleDevice TEST_DEVICE_DATA;
static modelWithAction TEST_DEVICE_WITH_ACTION;
static modelWithEachElement TEST_DEVICE_WITH_EACH_ELEMENT;
static JukeBox TEST_JUKEBOX_DEVICE;

/* Mocks */
static size_t currentSTRING_new_call;
static size_t whenShallSTRING_new_fail;

static size_t currentSTRING_clone_call;
static size_t whenShallSTRING_clone_fail;

static size_t currentSTRING_construct_call;
static size_t whenShallSTRING_construct_fail;

static size_t currentSTRING_concat_call;
static size_t whenShallSTRING_concat_fail;

static size_t currentSTRING_concat_with_STRING_call;
static size_t whenShallSTRING_concat_with_STRING_fail;
TYPED_MOCK_CLASS(AgentMacroMocks, CGlobalMock)
{
public:
    /* AgentTypeSystem mocks */
    MOCK_STATIC_METHOD_2(, AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_SINT32, AGENT_DATA_TYPE*, agentData, int32_t, v)
    MOCK_METHOD_END(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK)

    MOCK_STATIC_METHOD_2(, AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_charz, AGENT_DATA_TYPE*, agentData, const char*, v)
    MOCK_METHOD_END(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK)

    MOCK_STATIC_METHOD_2(, AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_charz_no_quotes, AGENT_DATA_TYPE*, agentData, const char*, v)
    MOCK_METHOD_END(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK)

    MOCK_STATIC_METHOD_5(, AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_Members, AGENT_DATA_TYPE*, agentData, const char*, typeName, size_t, nMembers, const char* const *, memberNames, const AGENT_DATA_TYPE*, memberValues)
    MOCK_METHOD_END(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK)

    MOCK_STATIC_METHOD_1(, void, Destroy_AGENT_DATA_TYPE, AGENT_DATA_TYPE*, agentData)
    MOCK_VOID_METHOD_END()

    /* IOT action functions */
    MOCK_STATIC_METHOD_3(, EXECUTE_COMMAND_RESULT, lotsOfAction, modelWithAction*, device, double, x, ascii_char_ptr, y)
    MOCK_METHOD_END(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_SUCCESS)
    MOCK_STATIC_METHOD_2(, EXECUTE_COMMAND_RESULT, simpleAction, modelWithEachElement*, device, int, actionArg1)
    MOCK_METHOD_END(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_SUCCESS)

    /* CodeFirst mocks */
    MOCK_STATIC_METHOD_2(, SCHEMA_HANDLE, CodeFirst_RegisterSchema, const char*, schemaNamespace, const REFLECTED_DATA_FROM_DATAPROVIDER*, metadata)
    MOCK_METHOD_END(SCHEMA_HANDLE, TEST_SCHEMA_HANDLE)
    MOCK_STATIC_METHOD_2(, EXECUTE_COMMAND_RESULT, CodeFirst_ExecuteCommand, void*, device, const char*, command)
    MOCK_METHOD_END(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_SUCCESS)
    MOCK_STATIC_METHOD_4(, void*, CodeFirst_CreateDevice, SCHEMA_MODEL_TYPE_HANDLE, model, const REFLECTED_DATA_FROM_DATAPROVIDER*, metadata, size_t, dataSize, bool, includePropertyPath)
    MOCK_METHOD_END(void*, (void*)&TEST_DEVICE_DATA)
    MOCK_STATIC_METHOD_1(, void, CodeFirst_DestroyDevice, void*, device)
    MOCK_VOID_METHOD_END()


    /* Schema mocks */
    MOCK_STATIC_METHOD_2(, SCHEMA_MODEL_TYPE_HANDLE, Schema_GetModelByName, SCHEMA_HANDLE, schemaHandle, const char*, modelName)
    MOCK_METHOD_END(SCHEMA_MODEL_TYPE_HANDLE, TEST_MODEL_HANDLE)

    MOCK_STATIC_METHOD_0(, STRING_HANDLE, STRING_new)
    STRING_HANDLE result2;
    currentSTRING_new_call++;
    if (whenShallSTRING_new_fail>0)
    {
        if (currentSTRING_new_call == whenShallSTRING_new_fail)
        {
            result2 = (STRING_HANDLE)NULL;
        }
        else
        {
            result2 = BASEIMPLEMENTATION::STRING_new();
        }
    }
    else
    {
        result2 = BASEIMPLEMENTATION::STRING_new();
    }
    MOCK_METHOD_END(STRING_HANDLE, result2)

        MOCK_STATIC_METHOD_1(, STRING_HANDLE, STRING_clone, STRING_HANDLE, handle)
        STRING_HANDLE result2;
    currentSTRING_clone_call++;
    if (whenShallSTRING_clone_fail>0)
    {
        if (currentSTRING_clone_call == whenShallSTRING_clone_fail)
        {
            result2 = (STRING_HANDLE)NULL;
        }
        else
        {
            result2 = BASEIMPLEMENTATION::STRING_clone(handle);
        }
    }
    else
    {
        result2 = BASEIMPLEMENTATION::STRING_clone(handle);
    }
    MOCK_METHOD_END(STRING_HANDLE, result2)

        MOCK_STATIC_METHOD_1(, STRING_HANDLE, STRING_construct, const char*, source)
        STRING_HANDLE result2;
    currentSTRING_construct_call++;
    if (whenShallSTRING_construct_fail>0)
    {
        if (currentSTRING_construct_call == whenShallSTRING_construct_fail)
        {
            result2 = (STRING_HANDLE)NULL;
        }
        else
        {
            result2 = BASEIMPLEMENTATION::STRING_construct(source);
        }
    }
    else
    {
        result2 = BASEIMPLEMENTATION::STRING_construct(source);
    }
    MOCK_METHOD_END(STRING_HANDLE, result2)

        MOCK_STATIC_METHOD_1(, void, STRING_delete, STRING_HANDLE, s)
        BASEIMPLEMENTATION::STRING_delete(s);
    MOCK_VOID_METHOD_END()

        MOCK_STATIC_METHOD_2(, int, STRING_concat, STRING_HANDLE, s1, const char*, s2)
        currentSTRING_concat_call++;
    MOCK_METHOD_END(int, (((whenShallSTRING_concat_fail>0) && (currentSTRING_concat_call == whenShallSTRING_concat_fail)) ? __LINE__ : BASEIMPLEMENTATION::STRING_concat(s1, s2)));

    MOCK_STATIC_METHOD_2(, int, STRING_concat_with_STRING, STRING_HANDLE, s1, STRING_HANDLE, s2)
        currentSTRING_concat_with_STRING_call++;
    MOCK_METHOD_END(int, (((whenShallSTRING_concat_with_STRING_fail>0) && (currentSTRING_concat_with_STRING_call == whenShallSTRING_concat_with_STRING_fail)) ? __LINE__ : BASEIMPLEMENTATION::STRING_concat_with_STRING(s1, s2)));

    MOCK_STATIC_METHOD_1(, const char*, STRING_c_str, STRING_HANDLE, s)
        MOCK_METHOD_END(const char*, BASEIMPLEMENTATION::STRING_c_str(s))
};

DECLARE_GLOBAL_MOCK_METHOD_2(AgentMacroMocks, , AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_SINT32, AGENT_DATA_TYPE*, agentData, int32_t, v);
DECLARE_GLOBAL_MOCK_METHOD_2(AgentMacroMocks, , AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_charz, AGENT_DATA_TYPE*, agentData, const char*, v);
DECLARE_GLOBAL_MOCK_METHOD_2(AgentMacroMocks, , AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_charz_no_quotes, AGENT_DATA_TYPE*, agentData, const char*, v);
DECLARE_GLOBAL_MOCK_METHOD_5(AgentMacroMocks, , AGENT_DATA_TYPES_RESULT, Create_AGENT_DATA_TYPE_from_Members, AGENT_DATA_TYPE*, agentData, const char*, typeName, size_t, nMembers, const char* const *, memberNames, const AGENT_DATA_TYPE*, memberValues);
DECLARE_GLOBAL_MOCK_METHOD_1(AgentMacroMocks, , void, Destroy_AGENT_DATA_TYPE, AGENT_DATA_TYPE*, agentData);
DECLARE_GLOBAL_MOCK_METHOD_3(AgentMacroMocks, , EXECUTE_COMMAND_RESULT, lotsOfAction, modelWithAction*, device, double, x, ascii_char_ptr, y);
DECLARE_GLOBAL_MOCK_METHOD_2(AgentMacroMocks, , EXECUTE_COMMAND_RESULT, simpleAction, modelWithEachElement*, device, int, actionArg1);
DECLARE_GLOBAL_MOCK_METHOD_2(AgentMacroMocks, , SCHEMA_HANDLE, CodeFirst_RegisterSchema, const char*, schemaNamespace, const REFLECTED_DATA_FROM_DATAPROVIDER*, metadata);
DECLARE_GLOBAL_MOCK_METHOD_2(AgentMacroMocks, , SCHEMA_MODEL_TYPE_HANDLE, Schema_GetModelByName, SCHEMA_HANDLE, schemaHandle, const char*, modelName);
DECLARE_GLOBAL_MOCK_METHOD_4(AgentMacroMocks, , void*, CodeFirst_CreateDevice, SCHEMA_MODEL_TYPE_HANDLE, model, const REFLECTED_DATA_FROM_DATAPROVIDER*, metadata, size_t, dataSize, bool, includePropertyPath);
DECLARE_GLOBAL_MOCK_METHOD_2(AgentMacroMocks, , EXECUTE_COMMAND_RESULT, CodeFirst_ExecuteCommand, void*, device, const char*, command)
DECLARE_GLOBAL_MOCK_METHOD_1(AgentMacroMocks, , void, CodeFirst_DestroyDevice, void*, device);

DECLARE_GLOBAL_MOCK_METHOD_0(AgentMacroMocks, , STRING_HANDLE, STRING_new);
DECLARE_GLOBAL_MOCK_METHOD_1(AgentMacroMocks, , STRING_HANDLE, STRING_clone, STRING_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_1(AgentMacroMocks, , STRING_HANDLE, STRING_construct, const char*, s);
DECLARE_GLOBAL_MOCK_METHOD_1(AgentMacroMocks, , void, STRING_delete, STRING_HANDLE, s);
DECLARE_GLOBAL_MOCK_METHOD_2(AgentMacroMocks, , int, STRING_concat, STRING_HANDLE, s1, const char*, s2);
DECLARE_GLOBAL_MOCK_METHOD_2(AgentMacroMocks, , int, STRING_concat_with_STRING, STRING_HANDLE, s1, STRING_HANDLE, s2);
DECLARE_GLOBAL_MOCK_METHOD_1(AgentMacroMocks, , const char*, STRING_c_str, STRING_HANDLE, s);

static size_t g_NumProperties;
static CODEFIRST_RESULT g_SendResult;
static DEVICE_HANDLE g_DeviceHandle = (DEVICE_HANDLE)0x434233;

CODEFIRST_RESULT CodeFirst_SendAsync(unsigned char** destination, size_t* destinationSize, size_t numProperties, ...)
{
    (void)(destination, destinationSize);

    g_NumProperties = numProperties;
    va_list argptr;
    va_start(argptr, numProperties);
    va_end(argptr);

    return g_SendResult;
}

/* Helpers */

namespace
{
    bool MetadataHasElement_(const REFLECTED_DATA_FROM_DATAPROVIDER* metadata, const char* elementName, REFLECTION_TYPE elementType)
    {
        const REFLECTED_SOMETHING* something;
        for (something = metadata->reflectedData; something != NULL; something = something->next)
        {
            if (something->type == elementType)
            {
                if (something->type == REFLECTION_NOTHING)
                {
                    return true;
                }

                const char* name;
                switch (elementType)
                {
                case REFLECTION_STRUCT_TYPE:
                    name = something->what.structure.name;
                    break;
                case REFLECTION_FIELD_TYPE:
                    name = something->what.field.fieldName;
                    break;
                case REFLECTION_PROPERTY_TYPE:
                    name = something->what.property.name;
                    break;
                case REFLECTION_ACTION_TYPE:
                    name = something->what.action.name;
                    break;
                case REFLECTION_MODEL_TYPE:
                    name = something->what.model.name;
                    break;
                default:
                    throw "unsupported element type";
                    break;
                }

                if (strcmp(elementName, name) == 0)
                {
                    return true;
                }
            }
        }

        return false;
    }

    size_t MetdataListSize_(const REFLECTED_DATA_FROM_DATAPROVIDER* list)
    {
        size_t size = 0;
        const REFLECTED_SOMETHING* something;
        for (something = list->reflectedData; something != NULL; something = something->next, ++size);
        return size;
    }

    template<class EdmType>
    AGENT_DATA_TYPE StructWithOneFieldAsAgentDataType_(AGENT_DATA_TYPE_TYPE fieldtype, const char* fieldName, EdmType fieldValue)
    {
        AGENT_DATA_TYPE* value = (AGENT_DATA_TYPE*)malloc(sizeof(AGENT_DATA_TYPE));
        value->type = fieldtype;
        *(EdmType*)&value->value.edmInt32 = fieldValue;

        COMPLEX_TYPE_FIELD_TYPE* field = (COMPLEX_TYPE_FIELD_TYPE*)malloc(sizeof(COMPLEX_TYPE_FIELD_TYPE)); // { fieldName, value };
        field->fieldName = fieldName;
        field->value = value;

        AGENT_DATA_TYPE src;
        src.type = EDM_COMPLEX_TYPE_TYPE;
        src.value.edmComplexType.nMembers = 1;
        src.value.edmComplexType.fields = field;
        return src;
    }

    class ComplexAgentDataTypeWithOneField
    {
        AGENT_DATA_TYPE data_;
    public:
        template<class EdmType>
        ComplexAgentDataTypeWithOneField(AGENT_DATA_TYPE_TYPE fieldtype, const char* fieldName, EdmType fieldValue)
            : data_(StructWithOneFieldAsAgentDataType_(fieldtype, fieldName, fieldValue))
        {}
        virtual ~ComplexAgentDataTypeWithOneField()
        {
            free(data_.value.edmComplexType.fields->value);
            free(data_.value.edmComplexType.fields);
        }
        operator const AGENT_DATA_TYPE*() const { return &data_; }
    };

    static ComplexAgentDataTypeWithOneField SimpleStructAsAgentDataType_(int value)
    {
        EDM_INT32 val = { value };
        return ComplexAgentDataTypeWithOneField(EDM_INT32_TYPE, "value", val);
    }

    ComplexAgentDataTypeWithOneField StructWithWrongFieldNameAsAgentDataType_()
    {
        EDM_INT32 val = { 0 };
        return ComplexAgentDataTypeWithOneField(EDM_INT32_TYPE, "other", val);
    }

    ComplexAgentDataTypeWithOneField StructWithWrongFieldTypeAsAgentDataType_()
    {
        EDM_DOUBLE val = { 0.0 };
        return ComplexAgentDataTypeWithOneField(EDM_DOUBLE_TYPE, "other", val);
    }

    AGENT_DATA_TYPE EmptyAgentDataType_()
    {
        EDM_COMPLEX_TYPE empty = { 0, NULL };
        AGENT_DATA_TYPE src = { EDM_COMPLEX_TYPE_TYPE };
        src.value.edmComplexType = empty;
        return src;
    }
}

/* Schemas */

BEGIN_NAMESPACE(emptySchema)
END_NAMESPACE(emptySchema)

BEGIN_NAMESPACE(schemaWithStruct)
    DECLARE_STRUCT(simpleStruct, int, value)
END_NAMESPACE(schemaWithStruct)

BEGIN_NAMESPACE(schemaWithMultifieldStruct)
    DECLARE_STRUCT(subStruct, int, value)
    DECLARE_STRUCT(multifieldStruct, ascii_char_ptr, name, int, value, subStruct, sub)
END_NAMESPACE(schemaWithMultifieldStruct)


static MICROMOCK_MUTEX_HANDLE g_testByTest;

static MICROMOCK_GLOBAL_SEMAPHORE_HANDLE g_dllByDll;

/* Tests */

BEGIN_TEST_SUITE(AgentMacros_ut)

    TEST_SUITE_INITIALIZE(BeforeSuite)
    {
        TEST_INITIALIZE_MEMORY_DEBUG(g_dllByDll);
        g_testByTest = MicroMockCreateMutex();
        ASSERT_IS_NOT_NULL(g_testByTest);
    }

    TEST_SUITE_CLEANUP(AfterSuite)
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

        currentSTRING_new_call = 0;
        whenShallSTRING_new_fail = 0;

        currentSTRING_clone_call = 0;
        whenShallSTRING_clone_fail = 0;

        currentSTRING_construct_call = 0;
        whenShallSTRING_construct_fail = 0;

        currentSTRING_concat_call = 0;
        whenShallSTRING_concat_fail = 0;

        currentSTRING_concat_with_STRING_call = 0;
        whenShallSTRING_concat_with_STRING_fail = 0;

        g_SendResult = CODEFIRST_OK;
    }

    TEST_FUNCTION_CLEANUP(TestMethodCleanup)
    {
        if (!MicroMockReleaseMutex(g_testByTest))
        {
            ASSERT_FAIL("failure in test framework at ReleaseMutex");
        }
    }

    /* Tests_SRS_SERIALIZER_99_001:[For each completed schema declaration block, a unique storage container for schema metadata shall be available in the translation unit at runtime.] */
    TEST_FUNCTION(Empty_schema_declaration_clause_should_create_empty_schema_metadata)
    {
        // arrange

        // act
        // see BEGIN_NAMESPACE/END_NAMESPACE blocks above...

        // assert
        ASSERT_IS_TRUE(MetadataHasElement_(&ALL_REFLECTED(emptySchema), NULL, REFLECTION_NOTHING));
        ASSERT_ARE_EQUAL(size_t, 1, MetdataListSize_(&ALL_REFLECTED(emptySchema)));
    }

    TEST_FUNCTION(Struct_declaration_should_insert_struct_into_schema_metadata)
    {
        // arrange

        // act
        // see BEGIN_NAMESPACE/END_NAMESPACE blocks above...

        // assert
        ASSERT_IS_TRUE(MetadataHasElement_(&ALL_REFLECTED(schemaWithStruct), "simpleStruct", REFLECTION_STRUCT_TYPE));
        ASSERT_IS_TRUE(MetadataHasElement_(&ALL_REFLECTED(schemaWithStruct), "value", REFLECTION_FIELD_TYPE));
    }


    TEST_FUNCTION(Model_declaration_should_insert_a_model_into_the_schema_metadata)
    {
        // arrange

        // act
        // see BEGIN_NAMESPACE/END_NAMESPACE blocks above...

        // assert
        ASSERT_IS_TRUE(MetadataHasElement_(&ALL_REFLECTED(schemaWithModel), "modelWithEachElement", REFLECTION_MODEL_TYPE));
    }

    TEST_FUNCTION(Property_declaration_should_insert_a_property_into_the_schema)
    {
        // arrange

        // act
        // see BEGIN_NAMESPACE/END_NAMESPACE blocks above...

        // assert
        ASSERT_IS_TRUE(MetadataHasElement_(&ALL_REFLECTED(schemaWithModel), "simpleProperty", REFLECTION_PROPERTY_TYPE));
    }

    TEST_FUNCTION(Action_declaration_should_insert_an_action_into_the_schema)
    {
        ASSERT_IS_TRUE(MetadataHasElement_(&ALL_REFLECTED(schemaWithModel), "simpleAction", REFLECTION_ACTION_TYPE));
    }

    TEST_FUNCTION(ToAGENT_DATA_TYPE_Struct_should_fail_when_ToAGENT_DATA_TYPE_for_a_field_fails)
    {
        // arrange
        CNiceCallComparer<AgentMacroMocks> macroMocks;
        simpleStruct simple = { 42 };
        AGENT_DATA_TYPE data;

        EXPECTED_CALL(macroMocks, Create_AGENT_DATA_TYPE_from_SINT32(NULL, 0))
            .SetReturn((AGENT_DATA_TYPES_RESULT)7777);
        EXPECTED_CALL(macroMocks, Create_AGENT_DATA_TYPE_from_Members(NULL, NULL, 0, NULL, NULL))
            .NeverInvoked();

        // act
        AGENT_DATA_TYPES_RESULT result = ToAGENT_DATA_TYPE_simpleStruct(&data, simple);

        // assert
        ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_ERROR, result);
    }

    TEST_FUNCTION(ToAGENT_DATA_TYPE_Struct_should_clean_up_as_many_field_AGENT_DATA_TYPEs_as_it_creates)
    {
        // arrange
        AgentMacroMocks macroMocks;
        multifieldStruct multi = { (char*)"hello", 77, { 42 } };
        AGENT_DATA_TYPE data;

        EXPECTED_CALL(macroMocks, Create_AGENT_DATA_TYPE_from_charz(NULL, multi.name))  // Create AGENT_DATA_TYPE for "hello"...
            .ValidateArgument(2);
        EXPECTED_CALL(macroMocks, Create_AGENT_DATA_TYPE_from_SINT32(NULL, 77))         // Create AGENT_DATA_TYPE for 77...
            .ValidateArgument(2);
        EXPECTED_CALL(macroMocks, Create_AGENT_DATA_TYPE_from_SINT32(NULL, 0))          // Fail to create AGENT_DATA_TYPE for 42...
            .SetReturn(AGENT_DATA_TYPES_ERROR);
        EXPECTED_CALL(macroMocks, Destroy_AGENT_DATA_TYPE(NULL))                        // Only clean up "hello" and 77.
            .ExpectedTimesExactly(2);

        // act
        AGENT_DATA_TYPES_RESULT result = ToAGENT_DATA_TYPE_multifieldStruct(&data, multi);

        // assert
        ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_ERROR, result);
    }

    TEST_FUNCTION(ToAGENT_DATA_TYPE_Struct_should_fail_when_Create_AGENT_DATA_TYPE_from_Members_fails)
    {
        // arrange
        CNiceCallComparer<AgentMacroMocks> macroMocks;
        simpleStruct simple = { 42 };
        AGENT_DATA_TYPE data;

        EXPECTED_CALL(macroMocks, Create_AGENT_DATA_TYPE_from_Members(NULL, NULL, 0, NULL, NULL))
            .SetReturn((AGENT_DATA_TYPES_RESULT)7777);

        // act
        AGENT_DATA_TYPES_RESULT result = ToAGENT_DATA_TYPE_simpleStruct(&data, simple);

        // assert
        ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_ERROR, result);
    }

    TEST_FUNCTION(ToAGENT_DATA_TYPE_Struct_with_valid_args_should_succeed)
    {
        // arrange
        AgentMacroMocks macroMocks;
        multifieldStruct multi = { (char*)"hello", 77, { 42 } };
        AGENT_DATA_TYPE data;

        STRICT_EXPECTED_CALL(macroMocks, Create_AGENT_DATA_TYPE_from_charz(NULL, multi.name))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(macroMocks, Create_AGENT_DATA_TYPE_from_SINT32(NULL, multi.value))
            .IgnoreArgument(1);
        // recurse to subStruct -->
        STRICT_EXPECTED_CALL(macroMocks, Create_AGENT_DATA_TYPE_from_SINT32(NULL, multi.sub.value))
            .IgnoreArgument(1);
        STRICT_EXPECTED_CALL(macroMocks, Create_AGENT_DATA_TYPE_from_Members(NULL, "subStruct", 1, NULL, NULL))
            .IgnoreArgument(1).IgnoreArgument(4).IgnoreArgument(5);
        STRICT_EXPECTED_CALL(macroMocks, Destroy_AGENT_DATA_TYPE(NULL))
            .IgnoreArgument(1);
        // <-- exit recursion
        STRICT_EXPECTED_CALL(macroMocks, Create_AGENT_DATA_TYPE_from_Members(&data, "multifieldStruct", 3, NULL, NULL))
            .IgnoreArgument(4).IgnoreArgument(5);
        STRICT_EXPECTED_CALL(macroMocks, Destroy_AGENT_DATA_TYPE(NULL))
            .IgnoreArgument(1)
            .ExpectedTimesExactly(3);

        // act
        AGENT_DATA_TYPES_RESULT result = ToAGENT_DATA_TYPE_multifieldStruct(&data, multi);

        // assert
        ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
    }

    TEST_FUNCTION(FromAGENT_DATA_TYPE_Struct_with_non_struct_input_arg_should_fail)
    {
        // arrange
        AGENT_DATA_TYPE input = { EDM_INT32_TYPE, { { 42 } } };
        simpleStruct output;

        // act
        AGENT_DATA_TYPES_RESULT result = FromAGENT_DATA_TYPE_simpleStruct(&input, &output);

        // assert
        ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
    }

    TEST_FUNCTION(FromAGENT_DATA_TYPE_Struct_with_input_arg_that_has_a_different_field_count_should_fail)
    {
        // arrange
        auto src = EmptyAgentDataType_();
        simpleStruct dest;

        // act
        AGENT_DATA_TYPES_RESULT result = FromAGENT_DATA_TYPE_simpleStruct(&src, &dest);

        // assert
        ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
    }

    TEST_FUNCTION(FromAGENT_DATA_TYPE_Struct_with_input_arg_that_has_different_field_names_should_fail)
    {
        // arrange
        auto src = StructWithWrongFieldNameAsAgentDataType_();
        simpleStruct dest;

        // act
        AGENT_DATA_TYPES_RESULT result = FromAGENT_DATA_TYPE_simpleStruct(src, &dest);

        // assert
        ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
    }

    TEST_FUNCTION(FromAGENT_DATA_TYPE_Struct_with_input_arg_that_has_different_field_types_should_fail)
    {
        // arrange
        auto src = StructWithWrongFieldTypeAsAgentDataType_();
        simpleStruct dest;

        // act
        AGENT_DATA_TYPES_RESULT result = FromAGENT_DATA_TYPE_simpleStruct(src, &dest);

        // assert
        ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_INVALID_ARG, result);
    }

    TEST_FUNCTION(FromAGENT_DATA_TYPE_Struct_with_valid_args_should_succeed)
    {
        // arrange
        auto src = SimpleStructAsAgentDataType_(42);
        simpleStruct dest;

        // act
        AGENT_DATA_TYPES_RESULT result = FromAGENT_DATA_TYPE_simpleStruct(src, &dest);

        // assert
        ASSERT_ARE_EQUAL(AGENT_DATA_TYPES_RESULT, AGENT_DATA_TYPES_OK, result);
        ASSERT_ARE_EQUAL(int, 42, dest.value);
    }

    TEST_FUNCTION(DECLARE_IOT_MODEL_should_create_a_C_struct_representing_the_model)
    {
        struct modelWithEachElement_replica
        {
            unsigned char __modelWithEachElement_replica;
            int simpleProperty;
        };

        ASSERT_ARE_EQUAL(size_t, sizeof(modelWithEachElement_replica), sizeof(modelWithEachElement));
        ASSERT_ARE_EQUAL(size_t, offsetof(modelWithEachElement_replica, simpleProperty), offsetof(modelWithEachElement, simpleProperty));
    }

    TEST_FUNCTION(Create_AGENT_DATA_TYPE_From_Ptr_Property_should_wrap_equivalent_AgentTypeSystem_conversion_function)
    {
        // arrange
        AgentMacroMocks macroMocks;
        AGENT_DATA_TYPE data;
        const int value = 42;

        STRICT_EXPECTED_CALL(macroMocks, Create_AGENT_DATA_TYPE_from_SINT32(NULL, value))
            .IgnoreArgument(1)
            .SetReturn((AGENT_DATA_TYPES_RESULT)7777);

        // act
        int result = Create_AGENT_DATA_TYPE_From_Ptr_simpleProperty((void*)&value, &data);

        // assert
        ASSERT_ARE_EQUAL(int, 7777, result);
    }

    TEST_FUNCTION(ActionWRAPPER_function_with_wrong_ParameterCount_arg_should_fail)
    {
        // arrange
        AGENT_DATA_TYPE data = { EDM_INT32_TYPE, { { 42 } } };

        // act
        EXECUTE_COMMAND_RESULT result = simpleActionWRAPPER(&TEST_DEVICE_DATA, 0, &data);

        // assert
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
    }

    TEST_FUNCTION(ActionWRAPPER_function_should_fail_when_the_AGENT_DATA_TYPE_type_does_not_match)
    {
        // arrange
        AGENT_DATA_TYPE data = { EDM_DOUBLE_TYPE, { { 42 } } }; // should be int, not double

        // act
        EXECUTE_COMMAND_RESULT result = simpleActionWRAPPER(&TEST_DEVICE_DATA, 1, &data);

        // assert
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
    }
    
    TEST_FUNCTION(ActionWRAPPER_function_should_not_call_the_action_if_it_cannot_convert_all_arguments)
    {
        // arrange
        AgentMacroMocks macroMocks;
        AGENT_DATA_TYPE data[] = { { EDM_DOUBLE_TYPE, { { 42 } } }, { EDM_INT32_TYPE, { { 0 } } } }; // 2nd type should be char*

        EXPECTED_CALL(macroMocks, lotsOfAction(&TEST_DEVICE_WITH_ACTION, 0, NULL))
            .NeverInvoked();

        // act
        EXECUTE_COMMAND_RESULT result = lotsOfActionWRAPPER(&TEST_DEVICE_WITH_ACTION, 2, data);

        // assert
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
    }
    
    TEST_FUNCTION(ActionWRAPPER_function_should_call_action_function_when_conversion_from_AGENT_DATA_TYPE_succeeds)
    {
        // arrange
        AgentMacroMocks macroMocks;
        AGENT_DATA_TYPE data = { EDM_INT32_TYPE, { { 42 } } };

        STRICT_EXPECTED_CALL(macroMocks, simpleAction(&TEST_DEVICE_WITH_EACH_ELEMENT, 42));

        // act
        EXECUTE_COMMAND_RESULT result = simpleActionWRAPPER(&TEST_DEVICE_WITH_EACH_ELEMENT, 1, &data);

        // assert
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_SUCCESS, result);
    }

    /* GET_MODEL_HANDLE */

    /* Tests_SRS_SERIALIZER_99_110:[ The GET_MODEL_HANDLE function macro shall first register the schema by calling CodeFirst_RegisterSchema, passing schemaNamespace and a pointer to the metadata generated in the schema declaration block.] */
    /* Tests_SRS_SERIALIZER_99_094:[ GET_MODEL_HANDLE shall then call Schema_GetModelByName, passing the schemaHandle obtained from CodeFirst_RegisterSchema and modelName arguments, to retrieve the SCHEMA_MODEL_TYPE_HANDLE corresponding to the modelName argument.] */
    /* Tests_SRS_SERIALIZER_99_112:[ GET_MODEL_HANDLE will return the handle for the named model.] */
    TEST_FUNCTION(GET_MODEL_HANDLE_Shall_Call_Register_SchemaAnd_Returns_The_Result_Of_GetModelByName)
    {
        // arrange
        AgentMacroMocks macroMocks;

        STRICT_EXPECTED_CALL(macroMocks, CodeFirst_RegisterSchema("schemaWithModel", &schemaWithModel_allReflected));
        STRICT_EXPECTED_CALL(macroMocks, Schema_GetModelByName(TEST_SCHEMA_HANDLE, "SimpleDevice"));

        // act
        SCHEMA_MODEL_TYPE_HANDLE modelHandle = GET_MODEL_HANDLE(schemaWithModel, SimpleDevice);

        // assert
        ASSERT_ARE_EQUAL(void_ptr, (void*)TEST_MODEL_HANDLE, modelHandle);
    }

    /* Tests_SRS_SERIALIZER_99_110:[ The GET_MODEL_HANDLE function macro shall first register the schema by calling CodeFirst_RegisterSchema, passing schemaNamespace and a pointer to the metadata generated in the schema declaration block.] */
    /* Tests_SRS_SERIALIZER_99_094:[ GET_MODEL_HANDLE shall then call Schema_GetModelByName, passing the schemaHandle obtained from CodeFirst_RegisterSchema and modelName arguments, to retrieve the SCHEMA_MODEL_TYPE_HANDLE corresponding to the modelName argument.] */
    /* Tests_SRS_SERIALIZER_99_112:[ GET_MODEL_HANDLE will return the handle for the named model.] */
    TEST_FUNCTION(When_Schema_Get_Model_By_Name_Returns_NULL_GET_MODEL_HANDLE_Returns_NULL)
    {
        // arrange
        AgentMacroMocks macroMocks;

        STRICT_EXPECTED_CALL(macroMocks, CodeFirst_RegisterSchema("schemaWithModel", &schemaWithModel_allReflected));
        STRICT_EXPECTED_CALL(macroMocks, Schema_GetModelByName(TEST_SCHEMA_HANDLE, "SimpleDevice"))
            .SetReturn((SCHEMA_MODEL_TYPE_HANDLE)NULL);

        // act
        SCHEMA_MODEL_TYPE_HANDLE modelHandle = GET_MODEL_HANDLE(schemaWithModel, SimpleDevice);

        // assert
        ASSERT_IS_NULL(modelHandle);
    }

    /* CREATE_MODEL_INSTANCE */

    /* Tests_SRS_SERIALIZER_99_104:[ CREATE_MODEL_INSTANCE shall call GET_MODEL_HANDLE, passing schemaNamespace and modelName, to get a model handle representing the model defined in the corresponding schema declaration block.] */
    /* Tests_SRS_SERIALIZER_99_106:[ CREATE_MODEL_INSTANCE shall call CodeFirst_CreateDevice, passing the model handle (SCHEMA_MODEL_TYPE_HANDLE]*/
    /* Tests_SRS_SERIALIZER_99_108:[ If CodeFirst_CreateDevice succeeds, CREATE_MODEL_INSTANCE shall return a pointer to an instance of the C struct representing the model for the device.] */
    /* Tests_SRS_SERIALIZER_01_003: [If the argument serializerIncludePropertyPath is not specified, CREATE_MODEL_INSTANCE shall pass false to CodeFirst_Create.] */
    TEST_FUNCTION(CREATE_DEVICE_calls_CodeFirst_Device_Create_With_The_Model_Handle_1)
    {
        // arrange
        AgentMacroMocks macroMocks;

        STRICT_EXPECTED_CALL(macroMocks, CodeFirst_RegisterSchema("schemaWithModel", &schemaWithModel_allReflected));
        STRICT_EXPECTED_CALL(macroMocks, Schema_GetModelByName(TEST_SCHEMA_HANDLE, "SimpleDevice"));
        STRICT_EXPECTED_CALL(macroMocks, CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &schemaWithModel_allReflected, sizeof(SimpleDevice), false));

        // act
        SimpleDevice* myDevice = CREATE_MODEL_INSTANCE(schemaWithModel, SimpleDevice);

        // assert
        ASSERT_IS_NOT_NULL(myDevice);
    }

    /* Tests_SRS_SERIALIZER_99_104:[ CREATE_MODEL_INSTANCE shall call GET_MODEL_HANDLE, passing schemaNamespace and modelName, to get a model handle representing the model defined in the corresponding schema declaration block.] */
    /* Tests_SRS_SERIALIZER_99_106:[ CREATE_MODEL_INSTANCE shall call CodeFirst_CreateDevice, passing the model handle (SCHEMA_MODEL_TYPE_HANDLE]*/
    /* Tests_SRS_SERIALIZER_99_108:[ If CodeFirst_CreateDevice succeeds, CREATE_MODEL_INSTANCE shall return a pointer to an instance of the C struct representing the model for the device.] */
    /* Tests_SRS_SERIALIZER_01_003: [If the argument serializerIncludePropertyPath is not specified, CREATE_MODEL_INSTANCE shall pass false to CodeFirst_Create.] */
    TEST_FUNCTION(CREATE_DEVICE_calls_CodeFirst_Device_Create_With_The_Model_Handle_2)
    {
        // arrange
        AgentMacroMocks macroMocks;

        STRICT_EXPECTED_CALL(macroMocks, CodeFirst_RegisterSchema("schemaWithModel", &schemaWithModel_allReflected));
        STRICT_EXPECTED_CALL(macroMocks, Schema_GetModelByName(TEST_SCHEMA_HANDLE, "SimpleDevice"));
        STRICT_EXPECTED_CALL(macroMocks, CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &schemaWithModel_allReflected, sizeof(SimpleDevice), false));

        // act
        SimpleDevice* myDevice = CREATE_MODEL_INSTANCE(schemaWithModel, SimpleDevice);

        // assert
        ASSERT_IS_NOT_NULL(myDevice);
    }

    /* Tests_SRS_SERIALIZER_99_104:[ CREATE_MODEL_INSTANCE shall call GET_MODEL_HANDLE, passing schemaNamespace and modelName, to get a model handle representing the model defined in the corresponding schema declaration block.] */
    /* Tests_SRS_SERIALIZER_99_106:[ CREATE_MODEL_INSTANCE shall call CodeFirst_CreateDevice, passing the model handle (SCHEMA_MODEL_TYPE_HANDLE]*/
    /* Tests_SRS_SERIALIZER_99_108:[ If CodeFirst_CreateDevice succeeds, CREATE_MODEL_INSTANCE shall return a pointer to an instance of the C struct representing the model for the device.] */
    /* Tests_SRS_SERIALIZER_01_003: [If the argument serializerIncludePropertyPath is not specified, CREATE_MODEL_INSTANCE shall pass false to CodeFirst_Create.] */
    TEST_FUNCTION(CREATE_DEVICE_When_GetModelByName_Returns_NULL_Then_NULL_Is_Passed_To_Create_Device)
    {
        // arrange
        AgentMacroMocks macroMocks;

        STRICT_EXPECTED_CALL(macroMocks, CodeFirst_RegisterSchema("schemaWithModel", &schemaWithModel_allReflected));
        STRICT_EXPECTED_CALL(macroMocks, Schema_GetModelByName(TEST_SCHEMA_HANDLE, "SimpleDevice"))
            .SetReturn((SCHEMA_MODEL_TYPE_HANDLE)NULL);
        STRICT_EXPECTED_CALL(macroMocks, CodeFirst_CreateDevice(NULL, &schemaWithModel_allReflected, sizeof(SimpleDevice), false));

        // act
        SimpleDevice* myDevice = CREATE_MODEL_INSTANCE(schemaWithModel, SimpleDevice);

        // assert
        ASSERT_IS_NOT_NULL(myDevice);
    }

    /* Tests_SRS_SERIALIZER_01_002: [If the argument serializerIncludePropertyPath is specified, its value shall be passed to CodeFirst_Create.] */
    TEST_FUNCTION(CREATE_DEVICE_with_includePropertyPath_true_passes_true_to_CodeFirst_CreateDevice)
    {
        // arrange
        AgentMacroMocks macroMocks;

        STRICT_EXPECTED_CALL(macroMocks, CodeFirst_RegisterSchema("schemaWithModel", &schemaWithModel_allReflected));
        STRICT_EXPECTED_CALL(macroMocks, Schema_GetModelByName(TEST_SCHEMA_HANDLE, "SimpleDevice"));
        STRICT_EXPECTED_CALL(macroMocks, CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &schemaWithModel_allReflected, sizeof(SimpleDevice), true));

        // act
        SimpleDevice* myDevice = CREATE_MODEL_INSTANCE(schemaWithModel, SimpleDevice, true);

        // assert
        ASSERT_IS_NOT_NULL(myDevice);
    }

    /* Tests_SRS_SERIALIZER_01_002: [If the argument serializerIncludePropertyPath is specified, its value shall be passed to CodeFirst_Create.] */
    TEST_FUNCTION(CREATE_DEVICE_with_includePropertyPath_true_passes_false_to_CodeFirst_CreateDevice)
    {
        // arrange
        AgentMacroMocks macroMocks;

        STRICT_EXPECTED_CALL(macroMocks, CodeFirst_RegisterSchema("schemaWithModel", &schemaWithModel_allReflected));
        STRICT_EXPECTED_CALL(macroMocks, Schema_GetModelByName(TEST_SCHEMA_HANDLE, "SimpleDevice"));
        STRICT_EXPECTED_CALL(macroMocks, CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &schemaWithModel_allReflected, sizeof(SimpleDevice), false));

        // act
        SimpleDevice* myDevice = CREATE_MODEL_INSTANCE(schemaWithModel, SimpleDevice, false);

        // assert
        ASSERT_IS_NOT_NULL(myDevice);
    }

    /* Tests_SRS_SERIALIZER_99_107:[ If CodeFirst_CreateDevice fails, CREATE_MODEL_INSTANCE shall return NULL.] */
    TEST_FUNCTION(When_CodeFirst_CreateDevice_Returns_NULL_Then_CREATE_DEVICE_Fails)
    {
        // arrange
        AgentMacroMocks macroMocks;

        STRICT_EXPECTED_CALL(macroMocks, CodeFirst_RegisterSchema("schemaWithModel", &schemaWithModel_allReflected));
        STRICT_EXPECTED_CALL(macroMocks, Schema_GetModelByName(TEST_SCHEMA_HANDLE, "SimpleDevice"));
        STRICT_EXPECTED_CALL(macroMocks, CodeFirst_CreateDevice(TEST_MODEL_HANDLE, &schemaWithModel_allReflected, sizeof(SimpleDevice), false))
            .SetReturn((void*)NULL);

        // act
        SimpleDevice* myDevice = CREATE_MODEL_INSTANCE(schemaWithModel, SimpleDevice);

        // assert
        ASSERT_IS_NULL(myDevice);
    }

    /* DESTROY_MODEL_INSTANCE */

    /* Tests_SRS_SERIALIZER_99_109:[ DESTROY_MODEL_INSTANCE shall call CodeFirst_DestroyDevice, passing the pointer returned from CREATE_MODEL_INSTANCE, to release all resources associated with the device.] */
    TEST_FUNCTION(DESTROY_DEVICE_Calls_CodeFirst_DestroyDevice)
    {
        // arrange
        AgentMacroMocks macroMocks;
        SimpleDevice* myDevice = CREATE_MODEL_INSTANCE(schemaWithModel, SimpleDevice);
        macroMocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(macroMocks, CodeFirst_DestroyDevice(&TEST_DEVICE_DATA));

        // act
        DESTROY_MODEL_INSTANCE(myDevice);

        // assert
        // uMock checks the calls
    }

    /* SEND */

    /* Tests_SRS_SERIALIZER_99_113:[ SERIALIZE shall call CodeFirst_SendAsync, passing a destination, destinationSize, the number of properties to publish, and pointers to the values for each property.] */
    /* Tests_SRS_SERIALIZER_99_117:[ If CodeFirst_SendAsync succeeds, SEND will return IOT_AGENT_OK.] */
    TEST_FUNCTION(SEND_With_One_Property_Succeeds)
    {
        // arrange
        AgentMacroMocks macroMocks;
        SimpleDevice* myDevice = CREATE_MODEL_INSTANCE(schemaWithModel, SimpleDevice);
        macroMocks.ResetAllCalls();

        // act
        IOT_AGENT_RESULT result = SERIALIZE(NULL, NULL, myDevice->Speed);

        // assert
        // uMock checks the calls
        ASSERT_ARE_EQUAL(IOT_AGENT_RESULT, IOT_AGENT_OK, result);
        ASSERT_ARE_EQUAL(size_t, 1, g_NumProperties);
        
    }

    /* Tests_SRS_SERIALIZER_99_113:[ SERIALIZE shall call CodeFirst_SendAsync, passing a destination, destinationSize, the number of properties to publish, and pointers to the values for each property.] */
    /* Tests_SRS_SERIALIZER_99_114:[ If CodeFirst_SendAsync fails, SEND shall return IOT_AGENT_SEND_FAILED.] */
    TEST_FUNCTION(When_CodeFirst_Send_Fails_SEND_Fails)
    {
        // arrange
        AgentMacroMocks macroMocks;
        SimpleDevice* myDevice = CREATE_MODEL_INSTANCE(schemaWithModel, SimpleDevice);
        macroMocks.ResetAllCalls();

        g_SendResult = CODEFIRST_ERROR;

        // act
        IOT_AGENT_RESULT result = SERIALIZE(NULL, NULL, myDevice->Speed);

        // assert
        // uMock checks the calls
        ASSERT_ARE_EQUAL(IOT_AGENT_RESULT, IOT_AGENT_SERIALIZE_FAILED, result);
        ASSERT_ARE_EQUAL(size_t, 1, g_NumProperties);

    }

    /* Tests_SRS_SERIALIZER_99_113:[ SERIALIZE shall call CodeFirst_SendAsync, passing a destination, destinationSize, the number of properties to publish, and pointers to the values for each property.] */
    /* Tests_SRS_SERIALIZER_99_117:[ If CodeFirst_SendAsync succeeds, SEND will return IOT_AGENT_OK.] */
    TEST_FUNCTION(SEND_With_2_Properties_Succeeds)
    {
        // arrange
        AgentMacroMocks macroMocks;
        SimpleDevice* myDevice = CREATE_MODEL_INSTANCE(schemaWithModel, SimpleDevice);
        macroMocks.ResetAllCalls();

        // act
        IOT_AGENT_RESULT result = SERIALIZE(NULL, NULL, myDevice->Speed, myDevice->moreSpeed);

        // assert
        // uMock checks the calls
        ASSERT_ARE_EQUAL(IOT_AGENT_RESULT, IOT_AGENT_OK, result);
        ASSERT_ARE_EQUAL(size_t, 2, g_NumProperties);
    }

    TEST_FUNCTION(SEND_With_Model_In_Model_Property_Succeeds)
    {
        // arrange
        AgentMacroMocks macroMocks;
        JukeBox* jukebox = CREATE_MODEL_INSTANCE(JukeBoxes, JukeBox);
        macroMocks.ResetAllCalls();

        // act
        IOT_AGENT_RESULT result = SERIALIZE(NULL, NULL, jukebox->bestSong);

        // assert
        // uMock checks the calls
        ASSERT_ARE_EQUAL(IOT_AGENT_RESULT, IOT_AGENT_OK, result);
        ASSERT_ARE_EQUAL(size_t, 1, g_NumProperties);
        macroMocks.AssertActualAndExpectedCalls();

        // cleanup
        DESTROY_MODEL_INSTANCE(jukebox);
    }

    /*Tests_SRS_SERIALIZER_02_018: [EXECUTE_COMMAND macro shall call CodeFirst_ExecuteCommand passing device, command.]*/
    TEST_FUNCTION(EXECUTE_COMMAND_calls_CodeFirst_ExecuteCommand_1)
    {
        /// arrange
        AgentMacroMocks macroMocks;
        JukeBox* jukebox = CREATE_MODEL_INSTANCE(JukeBoxes, JukeBox);
        macroMocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(macroMocks, CodeFirst_ExecuteCommand(jukebox, "\"Name\":\"Shuffle\",\"Parameters\":null"));

        /// act
        auto result = EXECUTE_COMMAND(jukebox, "\"Name\":\"Shuffle\",\"Parameters\":null");

        /// assert
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_SUCCESS, result);
        macroMocks.AssertActualAndExpectedCalls();

        /// cleanup
        DESTROY_MODEL_INSTANCE(jukebox);
    }

    /*Tests_SRS_SERIALIZER_02_018: [EXECUTE_COMMAND macro shall call CodeFirst_ExecuteCommand passing device, command.]*/
    TEST_FUNCTION(EXECUTE_COMMAND_calls_CodeFirst_ExecuteCommand_2)
    {
        /// arrange
        AgentMacroMocks macroMocks;
        JukeBox* jukebox = CREATE_MODEL_INSTANCE(JukeBoxes, JukeBox);
        macroMocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(macroMocks, CodeFirst_ExecuteCommand(jukebox, "\"Name\":\"Shuffle\",\"Parameters\":null"))
            .SetReturn(EXECUTE_COMMAND_FAILED);

        /// act
        auto result = EXECUTE_COMMAND(jukebox, "\"Name\":\"Shuffle\",\"Parameters\":null");

        /// assert
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_FAILED, result);
        macroMocks.AssertActualAndExpectedCalls();

        /// cleanup
        DESTROY_MODEL_INSTANCE(jukebox);
    }

    /*Tests_SRS_SERIALIZER_02_018: [EXECUTE_COMMAND macro shall call CodeFirst_ExecuteCommand passing device, command.]*/
    TEST_FUNCTION(EXECUTE_COMMAND_calls_CodeFirst_ExecuteCommand_3)
    {
        /// arrange
        AgentMacroMocks macroMocks;
        JukeBox* jukebox = CREATE_MODEL_INSTANCE(JukeBoxes, JukeBox);
        macroMocks.ResetAllCalls();

        STRICT_EXPECTED_CALL(macroMocks, CodeFirst_ExecuteCommand(jukebox, "\"Name\":\"Shuffle\",\"Parameters\":null"))
            .SetReturn(EXECUTE_COMMAND_ERROR);

        /// act
        auto result = EXECUTE_COMMAND(jukebox, "\"Name\":\"Shuffle\",\"Parameters\":null");

        /// assert
        ASSERT_ARE_EQUAL(EXECUTE_COMMAND_RESULT, EXECUTE_COMMAND_ERROR, result);
        macroMocks.AssertActualAndExpectedCalls();

        /// cleanup
        DESTROY_MODEL_INSTANCE(jukebox);
    }

END_TEST_SUITE(AgentMacros_ut)
