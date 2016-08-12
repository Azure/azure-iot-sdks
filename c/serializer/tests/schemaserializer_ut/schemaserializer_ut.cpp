// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <cstdlib>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "testrunnerswitcher.h"
#include "micromock.h"
#include "micromockcharstararenullterminatedstrings.h"
#include "schemaserializer.h"
#include "azure_c_shared_utility/vector.h"
#include "azure_c_shared_utility/strings.h"
#include "schema.h"

TYPED_MOCK_CLASS(CSchemaSerializerMocks, CGlobalMock)
{
public:
    /* Schema mocks */
    MOCK_STATIC_METHOD_2(, SCHEMA_RESULT, Schema_GetModelActionCount, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle, size_t*, actionCount)
    MOCK_METHOD_END(SCHEMA_RESULT, SCHEMA_OK);
    MOCK_STATIC_METHOD_1(, const char*, Schema_GetModelActionName, SCHEMA_ACTION_HANDLE, actionHandle)
    MOCK_METHOD_END(const char*, NULL);
    MOCK_STATIC_METHOD_2(, SCHEMA_ACTION_HANDLE, Schema_GetModelActionByIndex, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle, size_t, index)
    MOCK_METHOD_END(SCHEMA_ACTION_HANDLE, NULL);
    MOCK_STATIC_METHOD_2(, SCHEMA_RESULT, Schema_GetModelActionArgumentCount, SCHEMA_ACTION_HANDLE, actionHandle, size_t*, argumentCount)
    MOCK_METHOD_END(SCHEMA_RESULT, SCHEMA_OK);
    MOCK_STATIC_METHOD_2(, SCHEMA_ACTION_ARGUMENT_HANDLE, Schema_GetModelActionArgumentByIndex, SCHEMA_ACTION_HANDLE, actionHandle, size_t, argumentIndex)
    MOCK_METHOD_END(SCHEMA_ACTION_ARGUMENT_HANDLE, NULL);
    MOCK_STATIC_METHOD_1(, const char*, Schema_GetActionArgumentName, SCHEMA_ACTION_ARGUMENT_HANDLE, actionArgumentHandle)
    MOCK_METHOD_END(const char*, NULL);
    MOCK_STATIC_METHOD_1(, const char*, Schema_GetActionArgumentType, SCHEMA_ACTION_ARGUMENT_HANDLE, actionArgumentHandle)
    MOCK_METHOD_END(const char*, NULL);

    /* Strings mocks */
    MOCK_STATIC_METHOD_2(, int, STRING_concat, STRING_HANDLE, s1, const char*, s2)
    MOCK_METHOD_END(int, 0);
};

DECLARE_GLOBAL_MOCK_METHOD_2(CSchemaSerializerMocks, , SCHEMA_RESULT, Schema_GetModelActionCount, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle, size_t*, actionCount);
DECLARE_GLOBAL_MOCK_METHOD_1(CSchemaSerializerMocks, , const char*, Schema_GetModelActionName, SCHEMA_ACTION_HANDLE, actionHandle);
DECLARE_GLOBAL_MOCK_METHOD_2(CSchemaSerializerMocks, , SCHEMA_ACTION_HANDLE, Schema_GetModelActionByIndex, SCHEMA_MODEL_TYPE_HANDLE, modelTypeHandle, size_t, index);
DECLARE_GLOBAL_MOCK_METHOD_2(CSchemaSerializerMocks, , SCHEMA_RESULT, Schema_GetModelActionArgumentCount, SCHEMA_ACTION_HANDLE, actionHandle, size_t*, argumentCount);
DECLARE_GLOBAL_MOCK_METHOD_2(CSchemaSerializerMocks, , SCHEMA_ACTION_ARGUMENT_HANDLE, Schema_GetModelActionArgumentByIndex, SCHEMA_ACTION_HANDLE, actionHandle, size_t, argumentIndex);
DECLARE_GLOBAL_MOCK_METHOD_1(CSchemaSerializerMocks, , const char*, Schema_GetActionArgumentName, SCHEMA_ACTION_ARGUMENT_HANDLE, actionArgumentHandle);
DECLARE_GLOBAL_MOCK_METHOD_1(CSchemaSerializerMocks, , const char*, Schema_GetActionArgumentType, SCHEMA_ACTION_ARGUMENT_HANDLE, actionArgumentHandle);
DECLARE_GLOBAL_MOCK_METHOD_2(CSchemaSerializerMocks, , int, STRING_concat, STRING_HANDLE, s1, const char*, s2);

#define TEST_STRING_HANDLE  (STRING_HANDLE)0x4242
#define TEST_MODEL_HANDLE   (SCHEMA_MODEL_TYPE_HANDLE)0x4243

#define TEST_ACTION_1       (SCHEMA_ACTION_HANDLE)0x44
#define TEST_ACTION_2       (SCHEMA_ACTION_HANDLE)0x45

#define TEST_ARG_1          (SCHEMA_ACTION_ARGUMENT_HANDLE)0x4042
#define TEST_ARG_2          (SCHEMA_ACTION_ARGUMENT_HANDLE)0x4043

static MICROMOCK_MUTEX_HANDLE g_testByTest;

DEFINE_MICROMOCK_ENUM_TO_STRING(SCHEMA_SERIALIZER_RESULT, SCHEMA_SERIALIZER_RESULT_VALUES);

static MICROMOCK_GLOBAL_SEMAPHORE_HANDLE g_dllByDll;

BEGIN_TEST_SUITE(SchemaSerializer_ut)

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
    }

    TEST_FUNCTION_CLEANUP(TestMethodCleanup)
    {
        if (!MicroMockReleaseMutex(g_testByTest))
        {
            ASSERT_FAIL("failure in test framework at ReleaseMutex");
        }
    }

    /* SchemaSerializer_SerializeCommandMetadata */

    /* Tests_SRS_SCHEMA_SERIALIZER_01_013: [If the modelHandle argument is NULL, SchemaSerializer_SerializeCommandMetadata shall return SCHEMA_SERIALIZER_INVALID_ARG.] */
    TEST_FUNCTION(SchemaSerializer_SerializeCommandMetadata_With_NULL_model_handle_fails)
    {
        // arrange
        CSchemaSerializerMocks mocks;

        // act
        SCHEMA_SERIALIZER_RESULT result = SchemaSerializer_SerializeCommandMetadata(NULL, TEST_STRING_HANDLE);

        // assert
        ASSERT_ARE_EQUAL(SCHEMA_SERIALIZER_RESULT, SCHEMA_SERIALIZER_INVALID_ARG, result);
    }

    /* Tests_SRS_SCHEMA_SERIALIZER_01_014: [If the schemaText argument is NULL, SchemaSerializer_SerializeCommandMetadata shall return SCHEMA_SERIALIZER_INVALID_ARG.] */
    TEST_FUNCTION(SchemaSerializer_SerializeCommandMetadata_With_NULL_string_handle_fails)
    {
        // arrange
        CSchemaSerializerMocks mocks;

        // act
        SCHEMA_SERIALIZER_RESULT result = SchemaSerializer_SerializeCommandMetadata(TEST_MODEL_HANDLE, NULL);

        // assert
        ASSERT_ARE_EQUAL(SCHEMA_SERIALIZER_RESULT, SCHEMA_SERIALIZER_INVALID_ARG, result);
    }

    /* Tests_SRS_SCHEMA_SERIALIZER_01_001: [SchemaSerializer_SerializeCommandMetadata shall serialize a specific model to a string using JSON as format.] */
    /* Tests_SRS_SCHEMA_SERIALIZER_01_002: [Only commands shall be serialized, the properties of a model shall be ignored.] */
    /* Tests_SRS_SCHEMA_SERIALIZER_01_003: [The output JSON shall have an array, where each array element shall represent a command.] */
    /* Tests_SRS_SCHEMA_SERIALIZER_01_011: [The JSON text shall be built into the string indicated by the schemaText argument by using String APIs.] */
    /* Tests_SRS_SCHEMA_SERIALIZER_01_012: [On success SchemaSerializer_SerializeCommandMetadata shall return SCHEMA_SERIALIZER_OK.] */
    /* Tests_SRS_SCHEMA_SERIALIZER_01_006: [The object for a command shall have a member named Name, whose value shall be the command name as obtained by using Schema APIs.] */
    TEST_FUNCTION(SchemaSerializer_SerializeCommandMetadata_When_Command_Count_Is_0_Should_Yield_An_Empty_Commands_Array)
    {
        // arrange
        CSchemaSerializerMocks mocks;
        size_t commandCount = 0;

        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "["));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionCount(TEST_MODEL_HANDLE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &commandCount, sizeof(commandCount));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "]"));

        // act
        SCHEMA_SERIALIZER_RESULT result = SchemaSerializer_SerializeCommandMetadata(TEST_MODEL_HANDLE, TEST_STRING_HANDLE);

        // assert
        ASSERT_ARE_EQUAL(SCHEMA_SERIALIZER_RESULT, SCHEMA_SERIALIZER_OK, result);
    }

    /* Tests_SRS_SCHEMA_SERIALIZER_01_001: [SchemaSerializer_SerializeCommandMetadata shall serialize a specific model to a string using JSON as format.] */
    /* Tests_SRS_SCHEMA_SERIALIZER_01_002: [Only commands shall be serialized, the properties of a model shall be ignored.] */
    /* Tests_SRS_SCHEMA_SERIALIZER_01_003: [The output JSON shall have an array, where each array element shall represent a command.] */
    /* Tests_SRS_SCHEMA_SERIALIZER_01_011: [The JSON text shall be built into the string indicated by the schemaText argument by using String APIs.] */
    /* Tests_SRS_SCHEMA_SERIALIZER_01_012: [On success SchemaSerializer_SerializeCommandMetadata shall return SCHEMA_SERIALIZER_OK.] */
    /* Tests_SRS_SCHEMA_SERIALIZER_01_006: [The object for a command shall have a member named Name, whose value shall be the command name as obtained by using Schema APIs.] */
    /* Tests_SRS_SCHEMA_SERIALIZER_01_005: [Each array element shall be a JSON object.] */
    /* Tests_SRS_SCHEMA_SERIALIZER_01_007: [The object for a command shall also have a "parameters" member.] */
    TEST_FUNCTION(SchemaSerializer_SerializeCommandMetadata_1_Command_With_No_Arguments_Yields_The_Proper_JSON)
    {
        // arrange
        CSchemaSerializerMocks mocks;
        size_t commandCount = 1;

        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "["));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionCount(TEST_MODEL_HANDLE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &commandCount, sizeof(commandCount));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "{ \"Name\":\""));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionByIndex(TEST_MODEL_HANDLE, 0))
            .SetReturn(TEST_ACTION_1);
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionName(TEST_ACTION_1))
            .SetReturn("Action1");
        size_t argCount = 0;
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionArgumentCount(TEST_ACTION_1, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &argCount, sizeof(argCount));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "Action1"));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "\", \"Parameters\":["));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "]}"));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "]"));

        // act
        SCHEMA_SERIALIZER_RESULT result = SchemaSerializer_SerializeCommandMetadata(TEST_MODEL_HANDLE, TEST_STRING_HANDLE);

        // assert
        ASSERT_ARE_EQUAL(SCHEMA_SERIALIZER_RESULT, SCHEMA_SERIALIZER_OK, result);
    }

    /* Tests_SRS_SCHEMA_SERIALIZER_01_001: [SchemaSerializer_SerializeCommandMetadata shall serialize a specific model to a string using JSON as format.] */
    /* Tests_SRS_SCHEMA_SERIALIZER_01_002: [Only commands shall be serialized, the properties of a model shall be ignored.] */
    /* Tests_SRS_SCHEMA_SERIALIZER_01_003: [The output JSON shall have an array, where each array element shall represent a command.] */
    /* Tests_SRS_SCHEMA_SERIALIZER_01_011: [The JSON text shall be built into the string indicated by the schemaText argument by using String APIs.] */
    /* Tests_SRS_SCHEMA_SERIALIZER_01_012: [On success SchemaSerializer_SerializeCommandMetadata shall return SCHEMA_SERIALIZER_OK.] */
    /* Tests_SRS_SCHEMA_SERIALIZER_01_006: [The object for a command shall have a member named Name, whose value shall be the command name as obtained by using Schema APIs.] */
    /* Tests_SRS_SCHEMA_SERIALIZER_01_005: [Each array element shall be a JSON object.] */
    /* Tests_SRS_SCHEMA_SERIALIZER_01_007: [The object for a command shall also have a "Parameters" member.] */
    /* Tests_SRS_SCHEMA_SERIALIZER_01_016: ["ascii_char_ptr" shall be translated to "string".] */
    /* Tests_SRS_SCHEMA_SERIALIZER_01_008: [The parameters member shall be an array, where each entry is a command parameter.]*/
    /* Tests_SRS_SCHEMA_SERIALIZER_01_009: [Each command parameter shall have a member named "Name", that should have as value the command argument name as obtained by using Schema APIs.] */
    /* Tests_SRS_SCHEMA_SERIALIZER_01_010: [Each command parameter shall have a member named "Type", that should have as value the command argument type as obtained by using Schema APIs.] */
    TEST_FUNCTION(SchemaSerializer_SerializeCommandMetadata_1_Command_With_1_Argument_Yields_The_Proper_JSON)
    {
        // arrange
        CSchemaSerializerMocks mocks;
        size_t commandCount = 1;

        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "["));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionCount(TEST_MODEL_HANDLE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &commandCount, sizeof(commandCount));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "{ \"Name\":\""));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionByIndex(TEST_MODEL_HANDLE, 0))
            .SetReturn(TEST_ACTION_1);
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionName(TEST_ACTION_1))
            .SetReturn("Action1");
        size_t argCount = 1;
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionArgumentCount(TEST_ACTION_1, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &argCount, sizeof(argCount));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "Action1"));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "\", \"Parameters\":["));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionArgumentByIndex(TEST_ACTION_1, 0))
            .SetReturn(TEST_ARG_1);
        STRICT_EXPECTED_CALL(mocks, Schema_GetActionArgumentName(TEST_ARG_1))
            .SetReturn("Argument1");
        STRICT_EXPECTED_CALL(mocks, Schema_GetActionArgumentType(TEST_ARG_1))
            .SetReturn("ascii_char_ptr");
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "{\"Name\":\""));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "Argument1"));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "\",\"Type\":\""));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "string"));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "\"}"));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "]}"));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "]"));

        // act
        SCHEMA_SERIALIZER_RESULT result = SchemaSerializer_SerializeCommandMetadata(TEST_MODEL_HANDLE, TEST_STRING_HANDLE);

        // assert
        ASSERT_ARE_EQUAL(SCHEMA_SERIALIZER_RESULT, SCHEMA_SERIALIZER_OK, result);
    }

    /* Tests_SRS_SCHEMA_SERIALIZER_01_001: [SchemaSerializer_SerializeCommandMetadata shall serialize a specific model to a string using JSON as format.] */
    /* Tests_SRS_SCHEMA_SERIALIZER_01_002: [Only commands shall be serialized, the properties of a model shall be ignored.] */
    /* Tests_SRS_SCHEMA_SERIALIZER_01_003: [The output JSON shall have an array, where each array element shall represent a command.] */
    /* Tests_SRS_SCHEMA_SERIALIZER_01_011: [The JSON text shall be built into the string indicated by the schemaText argument by using String APIs.] */
    /* Tests_SRS_SCHEMA_SERIALIZER_01_012: [On success SchemaSerializer_SerializeCommandMetadata shall return SCHEMA_SERIALIZER_OK.] */
    /* Tests_SRS_SCHEMA_SERIALIZER_01_006: [The object for a command shall have a member named Name, whose value shall be the command name as obtained by using Schema APIs.] */
    /* Tests_SRS_SCHEMA_SERIALIZER_01_005: [Each array element shall be a JSON object.] */
    /* Tests_SRS_SCHEMA_SERIALIZER_01_007: [The object for a command shall also have a "Parameters" member.] */
    /* Tests_SRS_SCHEMA_SERIALIZER_01_016: ["ascii_char_ptr" shall be translated to "string".] */
    /* Tests_SRS_SCHEMA_SERIALIZER_01_008: [The parameters member shall be an array, where each entry is a command parameter.]*/
    /* Tests_SRS_SCHEMA_SERIALIZER_01_009: [Each command parameter shall have a member named "Name", that should have as value the command argument name as obtained by using Schema APIs.] */
    /* Tests_SRS_SCHEMA_SERIALIZER_01_010: [Each command parameter shall have a member named "Type", that should have as value the command argument type as obtained by using Schema APIs.] */
    TEST_FUNCTION(SchemaSerializer_SerializeCommandMetadata_2_Commanda_With_1_Argument_Each_Yields_The_Proper_JSON)
    {
        // arrange
        CSchemaSerializerMocks mocks;
        size_t commandCount = 2;

        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "["));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionCount(TEST_MODEL_HANDLE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &commandCount, sizeof(commandCount));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "{ \"Name\":\""));

        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionByIndex(TEST_MODEL_HANDLE, 0))
            .SetReturn(TEST_ACTION_1);
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionName(TEST_ACTION_1))
            .SetReturn("Action1");
        size_t argCount = 1;
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionArgumentCount(TEST_ACTION_1, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &argCount, sizeof(argCount));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "Action1"));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "\", \"Parameters\":["));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionArgumentByIndex(TEST_ACTION_1, 0))
            .SetReturn(TEST_ARG_1);
        STRICT_EXPECTED_CALL(mocks, Schema_GetActionArgumentName(TEST_ARG_1))
            .SetReturn("Argument1");
        STRICT_EXPECTED_CALL(mocks, Schema_GetActionArgumentType(TEST_ARG_1))
            .SetReturn("ascii_char_ptr");
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "{\"Name\":\""));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "Argument1"));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "\",\"Type\":\""));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "string"));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "\"}"));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "]},"));

        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "{ \"Name\":\""));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionByIndex(TEST_MODEL_HANDLE, 1))
            .SetReturn(TEST_ACTION_2);
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionName(TEST_ACTION_2))
            .SetReturn("Action2");
        argCount = 1;
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionArgumentCount(TEST_ACTION_2, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &argCount, sizeof(argCount));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "Action2"));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "\", \"Parameters\":["));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionArgumentByIndex(TEST_ACTION_2, 0))
            .SetReturn(TEST_ARG_1);
        STRICT_EXPECTED_CALL(mocks, Schema_GetActionArgumentName(TEST_ARG_1))
            .SetReturn("Argument1");
        STRICT_EXPECTED_CALL(mocks, Schema_GetActionArgumentType(TEST_ARG_1))
            .SetReturn("ascii_char_ptr");
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "{\"Name\":\""));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "Argument1"));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "\",\"Type\":\""));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "string"));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "\"}"));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "]}"));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "]"));

        // act
        SCHEMA_SERIALIZER_RESULT result = SchemaSerializer_SerializeCommandMetadata(TEST_MODEL_HANDLE, TEST_STRING_HANDLE);

        // assert
        ASSERT_ARE_EQUAL(SCHEMA_SERIALIZER_RESULT, SCHEMA_SERIALIZER_OK, result);
    }

    /* Tests_SRS_SCHEMA_SERIALIZER_01_017: [All other types shall be kept as they are.]  */
    TEST_FUNCTION(SchemaSerializer_SerializeCommandMetadata_1_Command_With_1_Argument_Different_Than_String_Keeps_The_Same_Type)
    {
        // arrange
        CSchemaSerializerMocks mocks;
        size_t commandCount = 1;

        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "["));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionCount(TEST_MODEL_HANDLE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &commandCount, sizeof(commandCount));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "{ \"Name\":\""));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionByIndex(TEST_MODEL_HANDLE, 0))
            .SetReturn(TEST_ACTION_1);
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionName(TEST_ACTION_1))
            .SetReturn("Action1");
        size_t argCount = 1;
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionArgumentCount(TEST_ACTION_1, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &argCount, sizeof(argCount));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "Action1"));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "\", \"Parameters\":["));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionArgumentByIndex(TEST_ACTION_1, 0))
            .SetReturn(TEST_ARG_1);
        STRICT_EXPECTED_CALL(mocks, Schema_GetActionArgumentName(TEST_ARG_1))
            .SetReturn("Argument1");
        STRICT_EXPECTED_CALL(mocks, Schema_GetActionArgumentType(TEST_ARG_1))
            .SetReturn("pupu");
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "{\"Name\":\""));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "Argument1"));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "\",\"Type\":\""));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "pupu"));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "\"}"));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "]}"));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "]"));

        // act
        SCHEMA_SERIALIZER_RESULT result = SchemaSerializer_SerializeCommandMetadata(TEST_MODEL_HANDLE, TEST_STRING_HANDLE);

        // assert
        ASSERT_ARE_EQUAL(SCHEMA_SERIALIZER_RESULT, SCHEMA_SERIALIZER_OK, result);
    }

    /* Tests_SRS_SCHEMA_SERIALIZER_01_001: [SchemaSerializer_SerializeCommandMetadata shall serialize a specific model to a string using JSON as format.] */
    /* Tests_SRS_SCHEMA_SERIALIZER_01_002: [Only commands shall be serialized, the properties of a model shall be ignored.] */
    /* Tests_SRS_SCHEMA_SERIALIZER_01_003: [The output JSON shall have an array, where each array element shall represent a command.] */
    /* Tests_SRS_SCHEMA_SERIALIZER_01_011: [The JSON text shall be built into the string indicated by the schemaText argument by using String APIs.] */
    /* Tests_SRS_SCHEMA_SERIALIZER_01_012: [On success SchemaSerializer_SerializeCommandMetadata shall return SCHEMA_SERIALIZER_OK.] */
    /* Tests_SRS_SCHEMA_SERIALIZER_01_006: [The object for a command shall have a member named Name, whose value shall be the command name as obtained by using Schema APIs.] */
    /* Tests_SRS_SCHEMA_SERIALIZER_01_005: [Each array element shall be a JSON object.] */
    /* Tests_SRS_SCHEMA_SERIALIZER_01_007: [The object for a command shall also have a "Parameters" member.] */
    /* Tests_SRS_SCHEMA_SERIALIZER_01_016: ["ascii_char_ptr" shall be translated to "string".] */
    /* Tests_SRS_SCHEMA_SERIALIZER_01_008: [The parameters member shall be an array, where each entry is a command parameter.]*/
    /* Tests_SRS_SCHEMA_SERIALIZER_01_009: [Each command parameter shall have a member named "Name", that should have as value the command argument name as obtained by using Schema APIs.] */
    /* Tests_SRS_SCHEMA_SERIALIZER_01_010: [Each command parameter shall have a member named "Type", that should have as value the command argument type as obtained by using Schema APIs.] */
    TEST_FUNCTION(SchemaSerializer_SerializeCommandMetadata_1_Command_With_2_Arguments_Yields_The_Proper_JSON)
    {
        // arrange
        CSchemaSerializerMocks mocks;
        size_t commandCount = 1;

        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "["));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionCount(TEST_MODEL_HANDLE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &commandCount, sizeof(commandCount));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "{ \"Name\":\""));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionByIndex(TEST_MODEL_HANDLE, 0))
            .SetReturn(TEST_ACTION_1);
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionName(TEST_ACTION_1))
            .SetReturn("Action1");
        size_t argCount = 2;
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionArgumentCount(TEST_ACTION_1, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &argCount, sizeof(argCount));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "Action1"));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "\", \"Parameters\":["));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionArgumentByIndex(TEST_ACTION_1, 0))
            .SetReturn(TEST_ARG_1);
        STRICT_EXPECTED_CALL(mocks, Schema_GetActionArgumentName(TEST_ARG_1))
            .SetReturn("Argument1");
        STRICT_EXPECTED_CALL(mocks, Schema_GetActionArgumentType(TEST_ARG_1))
            .SetReturn("ascii_char_ptr");
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "{\"Name\":\""));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "Argument1"));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "\",\"Type\":\""));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "string"));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "\"},"));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionArgumentByIndex(TEST_ACTION_1, 1))
            .SetReturn(TEST_ARG_2);
        STRICT_EXPECTED_CALL(mocks, Schema_GetActionArgumentName(TEST_ARG_2))
            .SetReturn("Argument2");
        STRICT_EXPECTED_CALL(mocks, Schema_GetActionArgumentType(TEST_ARG_2))
            .SetReturn("ascii_char_ptr");
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "{\"Name\":\""));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "Argument2"));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "\",\"Type\":\""));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "string"));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "\"}"));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "]}"));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "]"));

        // act
        SCHEMA_SERIALIZER_RESULT result = SchemaSerializer_SerializeCommandMetadata(TEST_MODEL_HANDLE, TEST_STRING_HANDLE);

        // assert
        ASSERT_ARE_EQUAL(SCHEMA_SERIALIZER_RESULT, SCHEMA_SERIALIZER_OK, result);
    }

    /* Tests_SRS_SCHEMA_SERIALIZER_01_015: [If any of the Schema or String APIs fail then SchemaSerializer_SerializeCommandMetadata shall return SCHEMA_SERIALIZER_ERROR.] */
    TEST_FUNCTION(When_Concatenating_Arg_End_Fails_SchemaSerializer_SerializeCommandMetadata_fails)
    {
        // arrange
        CSchemaSerializerMocks mocks;
        size_t commandCount = 1;

        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "["));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionCount(TEST_MODEL_HANDLE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &commandCount, sizeof(commandCount));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "{ \"Name\":\""));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionByIndex(TEST_MODEL_HANDLE, 0))
            .SetReturn(TEST_ACTION_1);
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionName(TEST_ACTION_1))
            .SetReturn("Action1");
        size_t argCount = 2;
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionArgumentCount(TEST_ACTION_1, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &argCount, sizeof(argCount));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "Action1"));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "\", \"Parameters\":["));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionArgumentByIndex(TEST_ACTION_1, 0))
            .SetReturn(TEST_ARG_1);
        STRICT_EXPECTED_CALL(mocks, Schema_GetActionArgumentName(TEST_ARG_1))
            .SetReturn("Argument1");
        STRICT_EXPECTED_CALL(mocks, Schema_GetActionArgumentType(TEST_ARG_1))
            .SetReturn("ascii_char_ptr");
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "{\"Name\":\""));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "Argument1"));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "\",\"Type\":\""));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "string"));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "\"},"))
            .SetReturn(1);

        // act
        SCHEMA_SERIALIZER_RESULT result = SchemaSerializer_SerializeCommandMetadata(TEST_MODEL_HANDLE, TEST_STRING_HANDLE);

        // assert
        ASSERT_ARE_EQUAL(SCHEMA_SERIALIZER_RESULT, SCHEMA_SERIALIZER_ERROR, result);
    }

    /* Tests_SRS_SCHEMA_SERIALIZER_01_015: [If any of the Schema or String APIs fail then SchemaSerializer_SerializeCommandMetadata shall return SCHEMA_SERIALIZER_ERROR.] */
    TEST_FUNCTION(When_Concatenating_Arg_End_Fails_For_1_Arg_Command_SchemaSerializer_SerializeCommandMetadata_fails)
    {
        // arrange
        CSchemaSerializerMocks mocks;
        size_t commandCount = 1;

        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "["));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionCount(TEST_MODEL_HANDLE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &commandCount, sizeof(commandCount));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "{ \"Name\":\""));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionByIndex(TEST_MODEL_HANDLE, 0))
            .SetReturn(TEST_ACTION_1);
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionName(TEST_ACTION_1))
            .SetReturn("Action1");
        size_t argCount = 1;
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionArgumentCount(TEST_ACTION_1, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &argCount, sizeof(argCount));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "Action1"));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "\", \"Parameters\":["));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionArgumentByIndex(TEST_ACTION_1, 0))
            .SetReturn(TEST_ARG_1);
        STRICT_EXPECTED_CALL(mocks, Schema_GetActionArgumentName(TEST_ARG_1))
            .SetReturn("Argument1");
        STRICT_EXPECTED_CALL(mocks, Schema_GetActionArgumentType(TEST_ARG_1))
            .SetReturn("ascii_char_ptr");
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "{\"Name\":\""));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "Argument1"));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "\",\"Type\":\""));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "string"));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "\"}"))
            .SetReturn(1);

        // act
        SCHEMA_SERIALIZER_RESULT result = SchemaSerializer_SerializeCommandMetadata(TEST_MODEL_HANDLE, TEST_STRING_HANDLE);

        // assert
        ASSERT_ARE_EQUAL(SCHEMA_SERIALIZER_RESULT, SCHEMA_SERIALIZER_ERROR, result);
    }

    /* Tests_SRS_SCHEMA_SERIALIZER_01_015: [If any of the Schema or String APIs fail then SchemaSerializer_SerializeCommandMetadata shall return SCHEMA_SERIALIZER_ERROR.] */
    TEST_FUNCTION(When_Concatenating_Arg_Type_Fails_SchemaSerializer_SerializeCommandMetadata_fails)
    {
        // arrange
        CSchemaSerializerMocks mocks;
        size_t commandCount = 1;

        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "["));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionCount(TEST_MODEL_HANDLE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &commandCount, sizeof(commandCount));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "{ \"Name\":\""));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionByIndex(TEST_MODEL_HANDLE, 0))
            .SetReturn(TEST_ACTION_1);
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionName(TEST_ACTION_1))
            .SetReturn("Action1");
        size_t argCount = 2;
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionArgumentCount(TEST_ACTION_1, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &argCount, sizeof(argCount));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "Action1"));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "\", \"Parameters\":["));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionArgumentByIndex(TEST_ACTION_1, 0))
            .SetReturn(TEST_ARG_1);
        STRICT_EXPECTED_CALL(mocks, Schema_GetActionArgumentName(TEST_ARG_1))
            .SetReturn("Argument1");
        STRICT_EXPECTED_CALL(mocks, Schema_GetActionArgumentType(TEST_ARG_1))
            .SetReturn("ascii_char_ptr");
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "{\"Name\":\""));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "Argument1"));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "\",\"Type\":\""));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "string"))
            .SetReturn(1);

        // act
        SCHEMA_SERIALIZER_RESULT result = SchemaSerializer_SerializeCommandMetadata(TEST_MODEL_HANDLE, TEST_STRING_HANDLE);

        // assert
        ASSERT_ARE_EQUAL(SCHEMA_SERIALIZER_RESULT, SCHEMA_SERIALIZER_ERROR, result);
    }

    /* Tests_SRS_SCHEMA_SERIALIZER_01_015: [If any of the Schema or String APIs fail then SchemaSerializer_SerializeCommandMetadata shall return SCHEMA_SERIALIZER_ERROR.] */
    TEST_FUNCTION(When_Getting_Arg_Type_Fails_SchemaSerializer_SerializeCommandMetadata_fails)
    {
        // arrange
        CSchemaSerializerMocks mocks;
        size_t commandCount = 1;

        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "["));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionCount(TEST_MODEL_HANDLE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &commandCount, sizeof(commandCount));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "{ \"Name\":\""));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionByIndex(TEST_MODEL_HANDLE, 0))
            .SetReturn(TEST_ACTION_1);
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionName(TEST_ACTION_1))
            .SetReturn("Action1");
        size_t argCount = 2;
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionArgumentCount(TEST_ACTION_1, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &argCount, sizeof(argCount));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "Action1"));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "\", \"Parameters\":["));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionArgumentByIndex(TEST_ACTION_1, 0))
            .SetReturn(TEST_ARG_1);
        STRICT_EXPECTED_CALL(mocks, Schema_GetActionArgumentName(TEST_ARG_1))
            .SetReturn("Argument1");
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "{\"Name\":\""));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "Argument1"));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "\",\"Type\":\""));
        STRICT_EXPECTED_CALL(mocks, Schema_GetActionArgumentType(TEST_ARG_1))
            .SetReturn((const char*)NULL);

        // act
        SCHEMA_SERIALIZER_RESULT result = SchemaSerializer_SerializeCommandMetadata(TEST_MODEL_HANDLE, TEST_STRING_HANDLE);

        // assert
        ASSERT_ARE_EQUAL(SCHEMA_SERIALIZER_RESULT, SCHEMA_SERIALIZER_ERROR, result);
    }

    /* Tests_SRS_SCHEMA_SERIALIZER_01_015: [If any of the Schema or String APIs fail then SchemaSerializer_SerializeCommandMetadata shall return SCHEMA_SERIALIZER_ERROR.] */
    TEST_FUNCTION(When_Concatenating_ArgType_Member_Name_Fails_SchemaSerializer_SerializeCommandMetadata_fails)
    {
        // arrange
        CSchemaSerializerMocks mocks;
        size_t commandCount = 1;

        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "["));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionCount(TEST_MODEL_HANDLE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &commandCount, sizeof(commandCount));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "{ \"Name\":\""));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionByIndex(TEST_MODEL_HANDLE, 0))
            .SetReturn(TEST_ACTION_1);
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionName(TEST_ACTION_1))
            .SetReturn("Action1");
        size_t argCount = 2;
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionArgumentCount(TEST_ACTION_1, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &argCount, sizeof(argCount));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "Action1"));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "\", \"Parameters\":["));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionArgumentByIndex(TEST_ACTION_1, 0))
            .SetReturn(TEST_ARG_1);
        STRICT_EXPECTED_CALL(mocks, Schema_GetActionArgumentName(TEST_ARG_1))
            .SetReturn("Argument1");
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "{\"Name\":\""));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "Argument1"));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "\",\"Type\":\""))
            .SetReturn(1);

        // act
        SCHEMA_SERIALIZER_RESULT result = SchemaSerializer_SerializeCommandMetadata(TEST_MODEL_HANDLE, TEST_STRING_HANDLE);

        // assert
        ASSERT_ARE_EQUAL(SCHEMA_SERIALIZER_RESULT, SCHEMA_SERIALIZER_ERROR, result);
    }

    /* Tests_SRS_SCHEMA_SERIALIZER_01_015: [If any of the Schema or String APIs fail then SchemaSerializer_SerializeCommandMetadata shall return SCHEMA_SERIALIZER_ERROR.] */
    TEST_FUNCTION(When_Concatenating_ArgName_Fails_SchemaSerializer_SerializeCommandMetadata_fails)
    {
        // arrange
        CSchemaSerializerMocks mocks;
        size_t commandCount = 1;

        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "["));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionCount(TEST_MODEL_HANDLE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &commandCount, sizeof(commandCount));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "{ \"Name\":\""));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionByIndex(TEST_MODEL_HANDLE, 0))
            .SetReturn(TEST_ACTION_1);
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionName(TEST_ACTION_1))
            .SetReturn("Action1");
        size_t argCount = 2;
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionArgumentCount(TEST_ACTION_1, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &argCount, sizeof(argCount));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "Action1"));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "\", \"Parameters\":["));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionArgumentByIndex(TEST_ACTION_1, 0))
            .SetReturn(TEST_ARG_1);
        STRICT_EXPECTED_CALL(mocks, Schema_GetActionArgumentName(TEST_ARG_1))
            .SetReturn("Argument1");
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "{\"Name\":\""));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "Argument1"))
            .SetReturn(1);

        // act
        SCHEMA_SERIALIZER_RESULT result = SchemaSerializer_SerializeCommandMetadata(TEST_MODEL_HANDLE, TEST_STRING_HANDLE);

        // assert
        ASSERT_ARE_EQUAL(SCHEMA_SERIALIZER_RESULT, SCHEMA_SERIALIZER_ERROR, result);
    }

    /* Tests_SRS_SCHEMA_SERIALIZER_01_015: [If any of the Schema or String APIs fail then SchemaSerializer_SerializeCommandMetadata shall return SCHEMA_SERIALIZER_ERROR.] */
    TEST_FUNCTION(When_Getting_ArgName_Fails_SchemaSerializer_SerializeCommandMetadata_fails)
    {
        // arrange
        CSchemaSerializerMocks mocks;
        size_t commandCount = 1;

        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "["));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionCount(TEST_MODEL_HANDLE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &commandCount, sizeof(commandCount));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "{ \"Name\":\""));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionByIndex(TEST_MODEL_HANDLE, 0))
            .SetReturn(TEST_ACTION_1);
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionName(TEST_ACTION_1))
            .SetReturn("Action1");
        size_t argCount = 2;
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionArgumentCount(TEST_ACTION_1, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &argCount, sizeof(argCount));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "Action1"));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "\", \"Parameters\":["));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionArgumentByIndex(TEST_ACTION_1, 0))
            .SetReturn(TEST_ARG_1);
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "{\"Name\":\""));
        STRICT_EXPECTED_CALL(mocks, Schema_GetActionArgumentName(TEST_ARG_1))
            .SetReturn((const char*)NULL);

        // act
        SCHEMA_SERIALIZER_RESULT result = SchemaSerializer_SerializeCommandMetadata(TEST_MODEL_HANDLE, TEST_STRING_HANDLE);

        // assert
        ASSERT_ARE_EQUAL(SCHEMA_SERIALIZER_RESULT, SCHEMA_SERIALIZER_ERROR, result);
    }

    /* Tests_SRS_SCHEMA_SERIALIZER_01_015: [If any of the Schema or String APIs fail then SchemaSerializer_SerializeCommandMetadata shall return SCHEMA_SERIALIZER_ERROR.] */
    TEST_FUNCTION(When_Concatenating_argname_member_name_Fails_SchemaSerializer_SerializeCommandMetadata_fails)
    {
        // arrange
        CSchemaSerializerMocks mocks;
        size_t commandCount = 1;

        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "["));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionCount(TEST_MODEL_HANDLE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &commandCount, sizeof(commandCount));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "{ \"Name\":\""));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionByIndex(TEST_MODEL_HANDLE, 0))
            .SetReturn(TEST_ACTION_1);
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionName(TEST_ACTION_1))
            .SetReturn("Action1");
        size_t argCount = 2;
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionArgumentCount(TEST_ACTION_1, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &argCount, sizeof(argCount));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "Action1"));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "\", \"Parameters\":["));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionArgumentByIndex(TEST_ACTION_1, 0))
            .SetReturn(TEST_ARG_1);
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "{\"Name\":\""))
            .SetReturn(1);

        // act
        SCHEMA_SERIALIZER_RESULT result = SchemaSerializer_SerializeCommandMetadata(TEST_MODEL_HANDLE, TEST_STRING_HANDLE);

        // assert
        ASSERT_ARE_EQUAL(SCHEMA_SERIALIZER_RESULT, SCHEMA_SERIALIZER_ERROR, result);
    }

    /* Tests_SRS_SCHEMA_SERIALIZER_01_015: [If any of the Schema or String APIs fail then SchemaSerializer_SerializeCommandMetadata shall return SCHEMA_SERIALIZER_ERROR.] */
    TEST_FUNCTION(When_Getting_The_Arg_By_Index_Fails_SchemaSerializer_SerializeCommandMetadata_fails)
    {
        // arrange
        CSchemaSerializerMocks mocks;
        size_t commandCount = 1;

        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "["));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionCount(TEST_MODEL_HANDLE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &commandCount, sizeof(commandCount));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "{ \"Name\":\""));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionByIndex(TEST_MODEL_HANDLE, 0))
            .SetReturn(TEST_ACTION_1);
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionName(TEST_ACTION_1))
            .SetReturn("Action1");
        size_t argCount = 2;
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionArgumentCount(TEST_ACTION_1, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &argCount, sizeof(argCount));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "Action1"));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "\", \"Parameters\":["));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionArgumentByIndex(TEST_ACTION_1, 0))
            .SetReturn((SCHEMA_ACTION_ARGUMENT_HANDLE)NULL);

        // act
        SCHEMA_SERIALIZER_RESULT result = SchemaSerializer_SerializeCommandMetadata(TEST_MODEL_HANDLE, TEST_STRING_HANDLE);

        // assert
        ASSERT_ARE_EQUAL(SCHEMA_SERIALIZER_RESULT, SCHEMA_SERIALIZER_ERROR, result);
    }

    /* Tests_SRS_SCHEMA_SERIALIZER_01_015: [If any of the Schema or String APIs fail then SchemaSerializer_SerializeCommandMetadata shall return SCHEMA_SERIALIZER_ERROR.] */
    TEST_FUNCTION(When_Getting_The_Arg_Count_Fails_SchemaSerializer_SerializeCommandMetadata_fails)
    {
        // arrange
        CSchemaSerializerMocks mocks;
        size_t commandCount = 1;

        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "["));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionCount(TEST_MODEL_HANDLE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &commandCount, sizeof(commandCount));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "{ \"Name\":\""));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionByIndex(TEST_MODEL_HANDLE, 0))
            .SetReturn(TEST_ACTION_1);
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionName(TEST_ACTION_1))
            .SetReturn("Action1");
        size_t argCount = 2;
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "Action1"));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "\", \"Parameters\":["));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionArgumentCount(TEST_ACTION_1, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &argCount, sizeof(argCount))
            .SetReturn(SCHEMA_ERROR);

        // act
        SCHEMA_SERIALIZER_RESULT result = SchemaSerializer_SerializeCommandMetadata(TEST_MODEL_HANDLE, TEST_STRING_HANDLE);

        // assert
        ASSERT_ARE_EQUAL(SCHEMA_SERIALIZER_RESULT, SCHEMA_SERIALIZER_ERROR, result);
    }

    /* Tests_SRS_SCHEMA_SERIALIZER_01_015: [If any of the Schema or String APIs fail then SchemaSerializer_SerializeCommandMetadata shall return SCHEMA_SERIALIZER_ERROR.] */
    TEST_FUNCTION(When_Concatenating_the_parameters_field_name_Fails_SchemaSerializer_SerializeCommandMetadata_fails)
    {
        // arrange
        CSchemaSerializerMocks mocks;
        size_t commandCount = 1;

        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "["));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionCount(TEST_MODEL_HANDLE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &commandCount, sizeof(commandCount));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "{ \"Name\":\""));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionByIndex(TEST_MODEL_HANDLE, 0))
            .SetReturn(TEST_ACTION_1);
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionName(TEST_ACTION_1))
            .SetReturn("Action1");
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "Action1"));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "\", \"Parameters\":["))
            .SetReturn(1);

        // act
        SCHEMA_SERIALIZER_RESULT result = SchemaSerializer_SerializeCommandMetadata(TEST_MODEL_HANDLE, TEST_STRING_HANDLE);

        // assert
        ASSERT_ARE_EQUAL(SCHEMA_SERIALIZER_RESULT, SCHEMA_SERIALIZER_ERROR, result);
    }

    /* Tests_SRS_SCHEMA_SERIALIZER_01_015: [If any of the Schema or String APIs fail then SchemaSerializer_SerializeCommandMetadata shall return SCHEMA_SERIALIZER_ERROR.] */
    TEST_FUNCTION(When_Encoding_ActionName_Fails_SchemaSerializer_SerializeCommandMetadata_fails)
    {
        // arrange
        CSchemaSerializerMocks mocks;
        size_t commandCount = 1;

        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "["));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionCount(TEST_MODEL_HANDLE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &commandCount, sizeof(commandCount));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "{ \"Name\":\""));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionByIndex(TEST_MODEL_HANDLE, 0))
            .SetReturn(TEST_ACTION_1);
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionName(TEST_ACTION_1))
            .SetReturn("Action1");
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "Action1"))
            .SetReturn(1);

        // act
        SCHEMA_SERIALIZER_RESULT result = SchemaSerializer_SerializeCommandMetadata(TEST_MODEL_HANDLE, TEST_STRING_HANDLE);

        // assert
        ASSERT_ARE_EQUAL(SCHEMA_SERIALIZER_RESULT, SCHEMA_SERIALIZER_ERROR, result);
    }

    /* Tests_SRS_SCHEMA_SERIALIZER_01_015: [If any of the Schema or String APIs fail then SchemaSerializer_SerializeCommandMetadata shall return SCHEMA_SERIALIZER_ERROR.] */
    TEST_FUNCTION(When_getting_the_model_action_name_Fails_SchemaSerializer_SerializeCommandMetadata_fails)
    {
        // arrange
        CSchemaSerializerMocks mocks;
        size_t commandCount = 1;

        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "["));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionCount(TEST_MODEL_HANDLE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &commandCount, sizeof(commandCount));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "{ \"Name\":\""));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionByIndex(TEST_MODEL_HANDLE, 0))
            .SetReturn(TEST_ACTION_1);
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionName(TEST_ACTION_1))
            .SetReturn((const char*)NULL);

        // act
        SCHEMA_SERIALIZER_RESULT result = SchemaSerializer_SerializeCommandMetadata(TEST_MODEL_HANDLE, TEST_STRING_HANDLE);

        // assert
        ASSERT_ARE_EQUAL(SCHEMA_SERIALIZER_RESULT, SCHEMA_SERIALIZER_ERROR, result);
    }

    /* Tests_SRS_SCHEMA_SERIALIZER_01_015: [If any of the Schema or String APIs fail then SchemaSerializer_SerializeCommandMetadata shall return SCHEMA_SERIALIZER_ERROR.] */
    TEST_FUNCTION(When_concatenating_command_name_field_Fails_SchemaSerializer_SerializeCommandMetadata_fails)
    {
        // arrange
        CSchemaSerializerMocks mocks;
        size_t commandCount = 1;

        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "["));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionCount(TEST_MODEL_HANDLE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &commandCount, sizeof(commandCount));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionByIndex(TEST_MODEL_HANDLE, 0))
            .SetReturn(TEST_ACTION_1);
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "{ \"Name\":\""))
            .SetReturn(1);

        // act
        SCHEMA_SERIALIZER_RESULT result = SchemaSerializer_SerializeCommandMetadata(TEST_MODEL_HANDLE, TEST_STRING_HANDLE);

        // assert
        ASSERT_ARE_EQUAL(SCHEMA_SERIALIZER_RESULT, SCHEMA_SERIALIZER_ERROR, result);
    }

    /* Tests_SRS_SCHEMA_SERIALIZER_01_015: [If any of the Schema or String APIs fail then SchemaSerializer_SerializeCommandMetadata shall return SCHEMA_SERIALIZER_ERROR.] */
    TEST_FUNCTION(When_getting_the_model_action_Fails_SchemaSerializer_SerializeCommandMetadata_fails)
    {
        // arrange
        CSchemaSerializerMocks mocks;
        size_t commandCount = 1;

        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "["));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionCount(TEST_MODEL_HANDLE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &commandCount, sizeof(commandCount));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionByIndex(TEST_MODEL_HANDLE, 0))
            .SetReturn((SCHEMA_ACTION_HANDLE)NULL);

        // act
        SCHEMA_SERIALIZER_RESULT result = SchemaSerializer_SerializeCommandMetadata(TEST_MODEL_HANDLE, TEST_STRING_HANDLE);

        // assert
        ASSERT_ARE_EQUAL(SCHEMA_SERIALIZER_RESULT, SCHEMA_SERIALIZER_ERROR, result);
    }

    /* Tests_SRS_SCHEMA_SERIALIZER_01_015: [If any of the Schema or String APIs fail then SchemaSerializer_SerializeCommandMetadata shall return SCHEMA_SERIALIZER_ERROR.] */
    TEST_FUNCTION(When_getting_the_action_count_Fails_SchemaSerializer_SerializeCommandMetadata_fails)
    {
        // arrange
        CSchemaSerializerMocks mocks;
        size_t commandCount = 1;

        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "["));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionCount(TEST_MODEL_HANDLE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &commandCount, sizeof(commandCount))
            .SetReturn(SCHEMA_ERROR);

        // act
        SCHEMA_SERIALIZER_RESULT result = SchemaSerializer_SerializeCommandMetadata(TEST_MODEL_HANDLE, TEST_STRING_HANDLE);

        // assert
        ASSERT_ARE_EQUAL(SCHEMA_SERIALIZER_RESULT, SCHEMA_SERIALIZER_ERROR, result);
    }

    /* Tests_SRS_SCHEMA_SERIALIZER_01_015: [If any of the Schema or String APIs fail then SchemaSerializer_SerializeCommandMetadata shall return SCHEMA_SERIALIZER_ERROR.] */
    TEST_FUNCTION(When_concatenating_the_commands_node_Fails_SchemaSerializer_SerializeCommandMetadata_fails)
    {
        // arrange
        CSchemaSerializerMocks mocks;

        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "["))
            .SetReturn(1);

        // act
        SCHEMA_SERIALIZER_RESULT result = SchemaSerializer_SerializeCommandMetadata(TEST_MODEL_HANDLE, TEST_STRING_HANDLE);

        // assert
        ASSERT_ARE_EQUAL(SCHEMA_SERIALIZER_RESULT, SCHEMA_SERIALIZER_ERROR, result);
    }

    /* Tests_SRS_SCHEMA_SERIALIZER_01_015: [If any of the Schema or String APIs fail then SchemaSerializer_SerializeCommandMetadata shall return SCHEMA_SERIALIZER_ERROR.] */
    TEST_FUNCTION(When_Concatenating_End_Of_Commands_Object_Fails_SchemaSerializer_SerializeCommandMetadata_Fails)
    {
        // arrange
        CSchemaSerializerMocks mocks;
        size_t commandCount = 0;

        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "["));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionCount(TEST_MODEL_HANDLE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &commandCount, sizeof(commandCount));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "]"))
            .SetReturn(1);

        // act
        SCHEMA_SERIALIZER_RESULT result = SchemaSerializer_SerializeCommandMetadata(TEST_MODEL_HANDLE, TEST_STRING_HANDLE);

        // assert
        ASSERT_ARE_EQUAL(SCHEMA_SERIALIZER_RESULT, SCHEMA_SERIALIZER_ERROR, result);
    }

    /* Tests_SRS_SCHEMA_SERIALIZER_01_015: [If any of the Schema or String APIs fail then SchemaSerializer_SerializeCommandMetadata shall return SCHEMA_SERIALIZER_ERROR.] */
    TEST_FUNCTION(When_Concatenating_End_Of_Parameters_Array_Fails_SchemaSerializer_SerializeCommandMetadata_Fails)
    {
        // arrange
        CSchemaSerializerMocks mocks;
        size_t commandCount = 1;

        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "["));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionCount(TEST_MODEL_HANDLE, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &commandCount, sizeof(commandCount));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "{ \"Name\":\""));
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionByIndex(TEST_MODEL_HANDLE, 0))
            .SetReturn(TEST_ACTION_1);
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionName(TEST_ACTION_1))
            .SetReturn("Action1");
        size_t argCount = 0;
        STRICT_EXPECTED_CALL(mocks, Schema_GetModelActionArgumentCount(TEST_ACTION_1, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer(2, &argCount, sizeof(argCount));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "Action1"));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "\", \"Parameters\":["));
        STRICT_EXPECTED_CALL(mocks, STRING_concat(TEST_STRING_HANDLE, "]}"))
            .SetReturn(1);

        // act
        SCHEMA_SERIALIZER_RESULT result = SchemaSerializer_SerializeCommandMetadata(TEST_MODEL_HANDLE, TEST_STRING_HANDLE);

        // assert
        ASSERT_ARE_EQUAL(SCHEMA_SERIALIZER_RESULT, SCHEMA_SERIALIZER_ERROR, result);
    }

END_TEST_SUITE(SchemaSerializer_ut)
