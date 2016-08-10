// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <cstdlib>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "testrunnerswitcher.h"
#include "micromock.h"
#include "micromockcharstararenullterminatedstrings.h"
#include "multitree.h"

/*this is what we test*/
#include "jsondecoder.h"


static const MULTITREE_HANDLE TestMultiTreeHandle = (MULTITREE_HANDLE)0x4242;
static const MULTITREE_HANDLE TestChildHandle1 = (MULTITREE_HANDLE)0x4243;
static const MULTITREE_HANDLE TestChildHandle2 = (MULTITREE_HANDLE)0x4244;
static const MULTITREE_HANDLE TestChildHandle3 = (MULTITREE_HANDLE)0x4245;

TYPED_MOCK_CLASS(CJSONDecoderMocks, CGlobalMock)
{
public:
    /* MultiTree mocks */
    MOCK_STATIC_METHOD_2(, MULTITREE_HANDLE, MultiTree_Create, MULTITREE_CLONE_FUNCTION, cloneFunction, MULTITREE_FREE_FUNCTION, freeFunction)
    MOCK_METHOD_END(MULTITREE_HANDLE, TestMultiTreeHandle)
    MOCK_STATIC_METHOD_1(, void, MultiTree_Destroy, MULTITREE_HANDLE, treeHandle)
    MOCK_VOID_METHOD_END()
    MOCK_STATIC_METHOD_3(, MULTITREE_RESULT, MultiTree_AddChild, MULTITREE_HANDLE, treeHandle, const char*, childName, MULTITREE_HANDLE*, childHandle)
    MOCK_METHOD_END(MULTITREE_RESULT, MULTITREE_OK)
    MOCK_STATIC_METHOD_2(, MULTITREE_RESULT, MultiTree_SetValue, MULTITREE_HANDLE, treeHandle, void*, value)
    MOCK_METHOD_END(MULTITREE_RESULT, MULTITREE_OK)
};

DECLARE_GLOBAL_MOCK_METHOD_2(CJSONDecoderMocks, , MULTITREE_HANDLE, MultiTree_Create, MULTITREE_CLONE_FUNCTION, cloneFunction, MULTITREE_FREE_FUNCTION, freeFunction);
DECLARE_GLOBAL_MOCK_METHOD_1(CJSONDecoderMocks, , void, MultiTree_Destroy, MULTITREE_HANDLE, treeHandle);
DECLARE_GLOBAL_MOCK_METHOD_3(CJSONDecoderMocks, , MULTITREE_RESULT, MultiTree_AddChild, MULTITREE_HANDLE, treeHandle, const char*, childName, MULTITREE_HANDLE*, childHandle);
DECLARE_GLOBAL_MOCK_METHOD_2(CJSONDecoderMocks, , MULTITREE_RESULT, MultiTree_SetValue, MULTITREE_HANDLE, treeHandle, void*, value);

static const char* emptyJSONobject = "{}";

MICROMOCK_ENUM_TO_STRING(JSON_DECODER_RESULT_TAG,
    L"JSON_DECODER_OK",
    L"JSON_DECODER_INVALID_ARG",
    L"JSON_DECODER_PARSE_ERROR",
    L"JSON_DECODER_MULTITREE_FAILED",
    L"JSON_DECODER_ERROR");

static MICROMOCK_MUTEX_HANDLE g_testByTest;

static MICROMOCK_GLOBAL_SEMAPHORE_HANDLE g_dllByDll;

BEGIN_TEST_SUITE(JSONDecoder_ut)

TEST_SUITE_INITIALIZE(BeforeSuite)
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

/* Tests_SRS_JSON_DECODER_99_001:[ If any of the parameters passed to the JSONDecoder_JSON_To_MultiTree function is NULL then the function call shall return JSON_DECODER_INVALID_ARG.] */
TEST_FUNCTION(JSONDecoder_With_NULL_json_argument_Fails)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(NULL, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_INVALID_ARG, result);
}

/* Tests_SRS_JSON_DECODER_99_001:[ If any of the parameters passed to the JSONDecoder_JSON_To_MultiTree function is NULL then the function call shall return JSON_DECODER_INVALID_ARG.] */
TEST_FUNCTION(JSONDecoder_With_NULL_MultiTreeHandle_argument_Fails)
{
    ///arrange
    CJSONDecoderMocks mocks;
    char jsonString[] = "[]";

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(jsonString, NULL);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_INVALID_ARG, result);
}

/* Tests_SRS_JSON_DECODER_99_007:[ If parsing the JSON fails due to the JSON string being malformed, JSONDecoder_JSON_To_MultiTree shall return JSON_DECODER_PARSE_ERROR.] */
/* Tests_SRS_JSON_DECODER_99_012:[ A JSON text is a serialized object or array.] */
TEST_FUNCTION(JSONDecoder_With_A_Space_As_json_argument_Fails)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TestMultiTreeHandle));

    char jsonString[] = " ";
    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(jsonString, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_PARSE_ERROR, result);
}

/* Tests_SRS_JSON_DECODER_99_007:[ If parsing the JSON fails due to the JSON string being malformed, JSONDecoder_JSON_To_MultiTree shall return JSON_DECODER_PARSE_ERROR.] */
/* Tests_SRS_JSON_DECODER_99_012:[ A JSON text is a serialized object or array.] */
TEST_FUNCTION(JSONDecoder_With_An_Empty_String_JSON_Fails)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;

    char jsonString[] = "";
    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(jsonString, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_PARSE_ERROR, result);
}

/* Tests_SRS_JSON_DECODER_99_007:[ If parsing the JSON fails due to the JSON string being malformed, JSONDecoder_JSON_To_MultiTree shall return JSON_DECODER_PARSE_ERROR.] */
/* Tests_SRS_JSON_DECODER_99_012:[ A JSON text is a serialized object or array.] */
TEST_FUNCTION(JSONDecoder_When_The_JSON_Starts_With_An_Unexpected_Character_Fails)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TestMultiTreeHandle));

    char jsonString[] = "a";
    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(jsonString, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_PARSE_ERROR, result);
}

/* Tests_SRS_JSON_DECODER_99_007:[ If parsing the JSON fails due to the JSON string being malformed, JSONDecoder_JSON_To_MultiTree shall return JSON_DECODER_PARSE_ERROR.] */
/* Tests_SRS_JSON_DECODER_99_012:[ A JSON text is a serialized object or array.] */
TEST_FUNCTION(JSONDecoder_When_The_JSON_Is_Made_Of_Only_A_Left_Square_Bracket_Decoding_Fails)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TestMultiTreeHandle));

    char jsonString[] = "[";
    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(jsonString, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_PARSE_ERROR, result);
}

/* Tests_SRS_JSON_DECODER_99_007:[ If parsing the JSON fails due to the JSON string being malformed, JSONDecoder_JSON_To_MultiTree shall return JSON_DECODER_PARSE_ERROR.] */
/* Tests_SRS_JSON_DECODER_99_012:[ A JSON text is a serialized object or array.] */
TEST_FUNCTION(JSONDecoder_When_The_JSON_Is_Made_Of_Only_A_Left_Curly_Brace_Decoding_Fails)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TestMultiTreeHandle));
    char jsonString[] = "{";

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(jsonString, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_PARSE_ERROR, result);
}

/* Tests_SRS_JSON_DECODER_99_007:[ If parsing the JSON fails due to the JSON string being malformed, JSONDecoder_JSON_To_MultiTree shall return JSON_DECODER_PARSE_ERROR.] */
/* Tests_SRS_JSON_DECODER_99_012:[ A JSON text is a serialized object or array.] */
TEST_FUNCTION(JSONDecoder_When_The_JSON_Is_Made_Of_Only_A_Right_Square_Bracket_Decoding_Fails)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TestMultiTreeHandle));
    char jsonString[] = "]";
    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(jsonString, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_PARSE_ERROR, result);
}

/* Tests_SRS_JSON_DECODER_99_007:[ If parsing the JSON fails due to the JSON string being malformed, JSONDecoder_JSON_To_MultiTree shall return JSON_DECODER_PARSE_ERROR.] */
/* Tests_SRS_JSON_DECODER_99_012:[ A JSON text is a serialized object or array.] */
TEST_FUNCTION(JSONDecoder_When_The_JSON_Is_Made_Of_Only_A_Right_Curly_Brace_Decoding_Fails)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TestMultiTreeHandle));
    char jsonString[] = "}";
    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(jsonString, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_PARSE_ERROR, result);
}

/* Tests_SRS_JSON_DECODER_99_007:[ If parsing the JSON fails due to the JSON string being malformed, JSONDecoder_JSON_To_MultiTree shall return JSON_DECODER_PARSE_ERROR.] */
/* Tests_SRS_JSON_DECODER_99_012:[ A JSON text is a serialized object or array.] */
TEST_FUNCTION(JSONDecoder_When_The_JSON_Is_Made_Of_Only_A_Colon_Decoding_Fails)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TestMultiTreeHandle));
    char jsonString[] = ":";
    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(jsonString, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_PARSE_ERROR, result);
}

/* Tests_SRS_JSON_DECODER_99_007:[ If parsing the JSON fails due to the JSON string being malformed, JSONDecoder_JSON_To_MultiTree shall return JSON_DECODER_PARSE_ERROR.] */
/* Tests_SRS_JSON_DECODER_99_012:[ A JSON text is a serialized object or array.] */
TEST_FUNCTION(JSONDecoder_When_The_JSON_Is_Made_Of_Only_A_Comma_Decoding_Fails)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TestMultiTreeHandle));
    char jsonString[] = ",";
    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(jsonString, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_PARSE_ERROR, result);
}

/* Tests_SRS_JSON_DECODER_99_012:[ A JSON text is a serialized object or array.] */
/* Tests_SRS_JSON_DECODER_99_021:[    An object structure is represented as a pair of curly brackets surrounding zero or more name/value pairs (or members).] */
TEST_FUNCTION(JSONDecoder_When_The_JSON_Is_An_Empty_Object_Decoding_Succeeds)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    char jsonString[] = "{}";
    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(jsonString, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_OK, result);
}

/* Tests_SRS_JSON_DECODER_99_012:[ A JSON text is a serialized object or array.] */
/* Tests_SRS_JSON_DECODER_99_021:[    An object structure is represented as a pair of curly brackets surrounding zero or more name/value pairs (or members).] */
/* Tests_SRS_JSON_DECODER_99_007:[ If parsing the JSON fails due to the JSON string being malformed, JSONDecoder_JSON_To_MultiTree shall return JSON_DECODER_PARSE_ERROR.] */
TEST_FUNCTION(JSONDecoder_When_The_JSON_Is_An_Empty_Object_Followed_By_Another_Object_Begin_Decoding_Fails)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TestMultiTreeHandle));
    char jsonString[] = "{}{";
    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(jsonString, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_PARSE_ERROR, result);
}

/* Tests_SRS_JSON_DECODER_99_012:[ A JSON text is a serialized object or array.] */
/* Tests_SRS_JSON_DECODER_99_021:[    An object structure is represented as a pair of curly brackets surrounding zero or more name/value pairs (or members).] */
/* Tests_SRS_JSON_DECODER_99_007:[ If parsing the JSON fails due to the JSON string being malformed, JSONDecoder_JSON_To_MultiTree shall return JSON_DECODER_PARSE_ERROR.] */
TEST_FUNCTION(JSONDecoder_When_The_JSON_Is_Made_Of_2_Empty_Objects_At_Root_Decoding_Fails)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TestMultiTreeHandle));
    char jsonString[] = "{}{}";
    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(jsonString, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_PARSE_ERROR, result);
}

/* Tests_SRS_JSON_DECODER_99_012:[ A JSON text is a serialized object or array.] */
/* Tests_SRS_JSON_DECODER_99_021:[    An object structure is represented as a pair of curly brackets surrounding zero or more name/value pairs (or members).] */
/* Tests_SRS_JSON_DECODER_99_022:[ A name is a string.] */
/* Tests_SRS_JSON_DECODER_99_023:[  A single colon comes after each name, separating the name from the value.] */
/* Tests_SRS_JSON_DECODER_99_028:[ A string begins and ends with quotation marks.] */
TEST_FUNCTION(JSONDecoder_When_The_JSON_Is_Made_Of_An_Object_With_One_Element_Decoding_Succeeds)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "{\"member1\":\"a\"}";
    void* memberValue = strstr(json, "\"a\"");

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "member1", IGNORED_PTR_ARG)).CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));
    STRICT_EXPECTED_CALL(mocks, MultiTree_SetValue(TestChildHandle1, memberValue));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_OK, result);
}

/* Tests_SRS_JSON_DECODER_99_012:[ A JSON text is a serialized object or array.] */
/* Tests_SRS_JSON_DECODER_99_021:[    An object structure is represented as a pair of curly brackets surrounding zero or more name/value pairs (or members).] */
/* Tests_SRS_JSON_DECODER_99_022:[ A name is a string.] */
/* Tests_SRS_JSON_DECODER_99_023:[  A single colon comes after each name, separating the name from the value.] */
/* Tests_SRS_JSON_DECODER_99_024:[ A single comma separates a value from a following name.] */
/* Tests_SRS_JSON_DECODER_99_028:[ A string begins and ends with quotation marks.] */
/* Tests_SRS_JSON_DECODER_99_002:[ JSONDecoder_JSON_To_MultiTree shall use the MultiTree APIs to create the multi tree and add leafs to the multi tree.] */
/* Tests_SRS_JSON_DECODER_99_003:[ When a JSON element is decoded from the JSON object then a leaf shall be added to the MultiTree.] */
/* Tests_SRS_JSON_DECODER_99_004:[ The leaf node name in the multi tree shall be the JSON element name.] */
/* Tests_SRS_JSON_DECODER_99_005:[ The leaf node added in the multi tree shall have the value the string value of the JSON element as parsed from the JSON object.] */
/* Tests_SRS_JSON_DECODER_99_009:[ On success, JSONDecoder_JSON_To_MultiTree shall return a handle to the multi tree it created in the multiTreeHandle argument and it shall return JSON_DECODER_OK.] */
/* Tests_SRS_JSON_DECODER_99_008:[ JSONDecoder_JSON_To_MultiTree shall create a multi tree based on the json string argument.] */
TEST_FUNCTION(JSONDecoder_When_The_JSON_Is_Made_Of_An_Object_With_2_Elements_Decoding_Succeeds)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "{\"member1\":\"a\",\"member2\":\"b\"}";
    void* member1Value = strstr(json, "\"a\"");
    void* member2Value = strstr(json, "\"b\"");

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "member1", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));
    STRICT_EXPECTED_CALL(mocks, MultiTree_SetValue(TestChildHandle1, member1Value));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "member2", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle2, sizeof(TestChildHandle2));
    STRICT_EXPECTED_CALL(mocks, MultiTree_SetValue(TestChildHandle2, member2Value));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_OK, result);
}

/* Tests_SRS_JSON_DECODER_99_022:[ A name is a string.] */
/* Tests_SRS_JSON_DECODER_99_007:[ If parsing the JSON fails due to the JSON string being malformed, JSONDecoder_JSON_To_MultiTree shall return JSON_DECODER_PARSE_ERROR.] */
TEST_FUNCTION(JSONDecoder_When_The_JSON_Ends_After_The_Member_Name_Quote_Decoding_Fails)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "{\"";

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TestMultiTreeHandle));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_PARSE_ERROR, result);
}

/* Tests_SRS_JSON_DECODER_99_022:[ A name is a string.] */
/* Tests_SRS_JSON_DECODER_99_007:[ If parsing the JSON fails due to the JSON string being malformed, JSONDecoder_JSON_To_MultiTree shall return JSON_DECODER_PARSE_ERROR.] */
TEST_FUNCTION(JSONDecoder_When_The_JSON_Ends_After_The_First_Char_Of_The_Member_Name_Decoding_Fails)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "{\"m";

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TestMultiTreeHandle));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_PARSE_ERROR, result);
}

/* Tests_SRS_JSON_DECODER_99_022:[ A name is a string.] */
/* Tests_SRS_JSON_DECODER_99_007:[ If parsing the JSON fails due to the JSON string being malformed, JSONDecoder_JSON_To_MultiTree shall return JSON_DECODER_PARSE_ERROR.] */
TEST_FUNCTION(JSONDecoder_When_The_JSON_Ends_After_The_Ending_Quote_Of_The_Member_Name_Decoding_Fails)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "{\"m\"";

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TestMultiTreeHandle));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_PARSE_ERROR, result);
}

/* Tests_SRS_JSON_DECODER_99_022:[ A name is a string.] */
/* Tests_SRS_JSON_DECODER_99_007:[ If parsing the JSON fails due to the JSON string being malformed, JSONDecoder_JSON_To_MultiTree shall return JSON_DECODER_PARSE_ERROR.] */
TEST_FUNCTION(JSONDecoder_When_The_JSON_Ends_After_The_Colon_Decoding_Fails)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "{\"m\":";

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));
    STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TestMultiTreeHandle));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_PARSE_ERROR, result);
}

/* Tests_SRS_JSON_DECODER_99_022:[ A name is a string.] */
/* Tests_SRS_JSON_DECODER_99_007:[ If parsing the JSON fails due to the JSON string being malformed, JSONDecoder_JSON_To_MultiTree shall return JSON_DECODER_PARSE_ERROR.] */
TEST_FUNCTION(JSONDecoder_When_The_JSON_Ends_After_The_Begining_Value_Quote_Decoding_Fails)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "{\"member1\":\"";

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));
    STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TestMultiTreeHandle));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_PARSE_ERROR, result);
}

/* Tests_SRS_JSON_DECODER_99_022:[ A name is a string.] */
/* Tests_SRS_JSON_DECODER_99_007:[ If parsing the JSON fails due to the JSON string being malformed, JSONDecoder_JSON_To_MultiTree shall return JSON_DECODER_PARSE_ERROR.] */
TEST_FUNCTION(JSONDecoder_When_The_JSON_Ends_In_The_Middle_Of_A_String_Value_Decoding_Fails)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "{\"member1\":\"a";

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, IGNORED_PTR_ARG, IGNORED_PTR_ARG)).CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));
    STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TestMultiTreeHandle));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_PARSE_ERROR, result);
}

/* Tests_SRS_JSON_DECODER_99_022:[ A name is a string.] */
/* Tests_SRS_JSON_DECODER_99_007:[ If parsing the JSON fails due to the JSON string being malformed, JSONDecoder_JSON_To_MultiTree shall return JSON_DECODER_PARSE_ERROR.] */
TEST_FUNCTION(JSONDecoder_When_The_JSON_Ends_After_The_Ending_Quote_Of_A_Value_Decoding_Fails)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "{\"member1\":\"a\"";

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, IGNORED_PTR_ARG, IGNORED_PTR_ARG)).CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));
    EXPECTED_CALL(mocks, MultiTree_SetValue(TestChildHandle1, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TestMultiTreeHandle));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_PARSE_ERROR, result);
}

/* Tests_SRS_JSON_DECODER_99_022:[ A name is a string.] */
/* Tests_SRS_JSON_DECODER_99_007:[ If parsing the JSON fails due to the JSON string being malformed, JSONDecoder_JSON_To_MultiTree shall return JSON_DECODER_PARSE_ERROR.] */
TEST_FUNCTION(JSONDecoder_When_The_JSON_Ends_After_A_Comma_Decoding_Fails)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "{\"member1\":\"a\",";

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, IGNORED_PTR_ARG, IGNORED_PTR_ARG)).CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));
    EXPECTED_CALL(mocks, MultiTree_SetValue(TestChildHandle1, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TestMultiTreeHandle));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_PARSE_ERROR, result);
}

/* Tests_SRS_JSON_DECODER_99_022:[ A name is a string.] */
/* Tests_SRS_JSON_DECODER_99_007:[ If parsing the JSON fails due to the JSON string being malformed, JSONDecoder_JSON_To_MultiTree shall return JSON_DECODER_PARSE_ERROR.] */
TEST_FUNCTION(JSONDecoder_When_The_Starting_Quote_Of_The_Name_Is_Missing_Decoding_Fails)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "{member1\":\"a\"}";

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TestMultiTreeHandle));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_PARSE_ERROR, result);
}

/* Tests_SRS_JSON_DECODER_99_022:[ A name is a string.] */
/* Tests_SRS_JSON_DECODER_99_007:[ If parsing the JSON fails due to the JSON string being malformed, JSONDecoder_JSON_To_MultiTree shall return JSON_DECODER_PARSE_ERROR.] */
TEST_FUNCTION(JSONDecoder_When_The_Ending_Quote_Of_The_Name_Is_Missing_Decoding_Fails)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "{\"member1:\"a\"}";

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TestMultiTreeHandle));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_PARSE_ERROR, result);
}

/* Tests_SRS_JSON_DECODER_99_023:[  A single colon comes after each name, separating the name from the value.] */
/* Tests_SRS_JSON_DECODER_99_007:[ If parsing the JSON fails due to the JSON string being malformed, JSONDecoder_JSON_To_MultiTree shall return JSON_DECODER_PARSE_ERROR.] */
TEST_FUNCTION(JSONDecoder_When_The_Colon_Is_Missing_In_A_Name_Value_Pair_Decoding_Fails)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "{\"member1\"\"a\"}";

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TestMultiTreeHandle));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_PARSE_ERROR, result);
}

/* Tests_SRS_JSON_DECODER_99_028:[ A string begins and ends with quotation marks.] */
/* Tests_SRS_JSON_DECODER_99_007:[ If parsing the JSON fails due to the JSON string being malformed, JSONDecoder_JSON_To_MultiTree shall return JSON_DECODER_PARSE_ERROR.] */
TEST_FUNCTION(JSONDecoder_When_The_Starting_Quote_Of_A_String_Value_Is_Missing_Decoding_Fails)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "{\"member1\":a\"}";

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, IGNORED_PTR_ARG, IGNORED_PTR_ARG)).CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));
    STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TestMultiTreeHandle));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_PARSE_ERROR, result);
}

/* Tests_SRS_JSON_DECODER_99_028:[ A string begins and ends with quotation marks.] */
/* Tests_SRS_JSON_DECODER_99_007:[ If parsing the JSON fails due to the JSON string being malformed, JSONDecoder_JSON_To_MultiTree shall return JSON_DECODER_PARSE_ERROR.] */
TEST_FUNCTION(JSONDecoder_When_The_Ending_Quote_Of_A_String_Value_Is_Missing_Decoding_Fails)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "{\"member1\":a\"}";

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, IGNORED_PTR_ARG, IGNORED_PTR_ARG)).CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));
    STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TestMultiTreeHandle));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_PARSE_ERROR, result);
}

/* Tests_SRS_JSON_DECODER_99_024:[ A single comma separates a value from a following name.] */
/* Tests_SRS_JSON_DECODER_99_007:[ If parsing the JSON fails due to the JSON string being malformed, JSONDecoder_JSON_To_MultiTree shall return JSON_DECODER_PARSE_ERROR.] */
TEST_FUNCTION(JSONDecoder_When_The_Comma_Separator_Is_Missing_Decoding_Fails)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "{\"member1\":\"a\"\"member2\":\"b\"}";

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, IGNORED_PTR_ARG, IGNORED_PTR_ARG)).CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));
    EXPECTED_CALL(mocks, MultiTree_SetValue(TestChildHandle1, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TestMultiTreeHandle));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_PARSE_ERROR, result);
}

/* Tests_SRS_JSON_DECODER_99_025:[ The names within an object SHOULD be unique.] */
/* Tests_SRS_JSON_DECODER_99_038:[ If any MultiTree API fails, JSONDecoder_JSON_To_MultiTree shall return JSON_DECODER_MULTITREE_FAILED.] */
TEST_FUNCTION(JSONDecoder_When_The_AddChild_Fails_Decoding_Fails)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "{\"member1\":\"a\",\"member1\":\"b\"}";

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, IGNORED_PTR_ARG, IGNORED_PTR_ARG)).SetReturn(MULTITREE_INVALID_ARG);
    STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TestMultiTreeHandle));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_MULTITREE_FAILED, result);
}

/* Tests_SRS_JSON_DECODER_99_026:[ An array structure is represented as square brackets surrounding zero or more values (or elements).] */
TEST_FUNCTION(JSONDecoder_When_The_JSON_Contains_An_Empty_Array_Decoding_Succeeds)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "[]";

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_OK, result);
}

/* Tests_SRS_JSON_DECODER_99_026:[ An array structure is represented as square brackets surrounding zero or more values (or elements).] */
/* Tests_SRS_JSON_DECODER_99_007:[ If parsing the JSON fails due to the JSON string being malformed, JSONDecoder_JSON_To_MultiTree shall return JSON_DECODER_PARSE_ERROR.] */
TEST_FUNCTION(JSONDecoder_When_The_JSON_Contains_Only_The_Opening_Array_Square_Bracket_Decoding_Fails)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "[";

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TestMultiTreeHandle));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_PARSE_ERROR, result);
}

/* Tests_SRS_JSON_DECODER_99_026:[ An array structure is represented as square brackets surrounding zero or more values (or elements).] */
/* Tests_SRS_JSON_DECODER_99_007:[ If parsing the JSON fails due to the JSON string being malformed, JSONDecoder_JSON_To_MultiTree shall return JSON_DECODER_PARSE_ERROR.] */
TEST_FUNCTION(JSONDecoder_When_The_JSON_Contains_Only_The_Closing_Array_Square_Bracket_Decoding_Fails)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "]";

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TestMultiTreeHandle));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_PARSE_ERROR, result);
}

/* Tests_SRS_JSON_DECODER_99_026:[ An array structure is represented as square brackets surrounding zero or more values (or elements).] */
/* Tests_SRS_JSON_DECODER_99_039:[ For array elements the multi tree node name shall be the string representation of the array index.] */
TEST_FUNCTION(JSONDecoder_When_The_JSON_Contains_One_String_Value_Decoding_Succeeds)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "[\"a\"]";
    void* value1Ptr = &json[1];

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "0", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));
    STRICT_EXPECTED_CALL(mocks, MultiTree_SetValue(TestChildHandle1, value1Ptr));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_OK, result);
}

/* Tests_SRS_JSON_DECODER_99_026:[ An array structure is represented as square brackets surrounding zero or more values (or elements).] */
/* Tests_SRS_JSON_DECODER_99_027:[ Elements are separated by commas.] */
/* Tests_SRS_JSON_DECODER_99_039:[ For array elements the multi tree node name shall be the string representation of the array index.] */
TEST_FUNCTION(JSONDecoder_When_The_JSON_Contains_An_Array_With_2_String_Values_Decoding_Succeeds)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "[\"a\",\"b\"]";
    void* value1Ptr = &json[1];
    void* value2Ptr = &json[5];

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "0", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));
    STRICT_EXPECTED_CALL(mocks, MultiTree_SetValue(TestChildHandle1, value1Ptr));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "1", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle2, sizeof(TestChildHandle2));
    STRICT_EXPECTED_CALL(mocks, MultiTree_SetValue(TestChildHandle2, value2Ptr));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_OK, result);
}

/* Tests_SRS_JSON_DECODER_99_026:[ An array structure is represented as square brackets surrounding zero or more values (or elements).] */
/* Tests_SRS_JSON_DECODER_99_007:[ If parsing the JSON fails due to the JSON string being malformed, JSONDecoder_JSON_To_MultiTree shall return JSON_DECODER_PARSE_ERROR.] */
TEST_FUNCTION(JSONDecoder_When_The_JSON_Ends_After_The_Begginning_Quote_Of_The_Array_First_String_Decoding_Fails)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "[\"";

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "0", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));
    STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TestMultiTreeHandle));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_PARSE_ERROR, result);
}

/* Tests_SRS_JSON_DECODER_99_026:[ An array structure is represented as square brackets surrounding zero or more values (or elements).] */
/* Tests_SRS_JSON_DECODER_99_007:[ If parsing the JSON fails due to the JSON string being malformed, JSONDecoder_JSON_To_MultiTree shall return JSON_DECODER_PARSE_ERROR.] */
TEST_FUNCTION(JSONDecoder_When_The_JSON_Ends_In_The_Middle_Of_The_Array_First_String_Decoding_Fails)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "[\"a";

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "0", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));
    STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TestMultiTreeHandle));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_PARSE_ERROR, result);
}

/* Tests_SRS_JSON_DECODER_99_026:[ An array structure is represented as square brackets surrounding zero or more values (or elements).] */
/* Tests_SRS_JSON_DECODER_99_007:[ If parsing the JSON fails due to the JSON string being malformed, JSONDecoder_JSON_To_MultiTree shall return JSON_DECODER_PARSE_ERROR.] */
TEST_FUNCTION(JSONDecoder_When_The_JSON_Ends_After_The_Ending_Quote_Of_The_Array_First_String_Decoding_Fails)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "[\"a\"";
    void* value1Ptr = &json[1];

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "0", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));
    STRICT_EXPECTED_CALL(mocks, MultiTree_SetValue(TestChildHandle1, value1Ptr));
    STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TestMultiTreeHandle));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_PARSE_ERROR, result);
}

/* Tests_SRS_JSON_DECODER_99_026:[ An array structure is represented as square brackets surrounding zero or more values (or elements).] */
/* Tests_SRS_JSON_DECODER_99_007:[ If parsing the JSON fails due to the JSON string being malformed, JSONDecoder_JSON_To_MultiTree shall return JSON_DECODER_PARSE_ERROR.] */
TEST_FUNCTION(JSONDecoder_When_The_JSON_Ends_After_The_Comma_Of_The_Array_Decoding_Fails)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "[\"a\",";
    void* value1Ptr = &json[1];

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "0", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));
    STRICT_EXPECTED_CALL(mocks, MultiTree_SetValue(TestChildHandle1, value1Ptr));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "1", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle2, sizeof(TestChildHandle2));
    STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TestMultiTreeHandle));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_PARSE_ERROR, result);
}

/* Tests_SRS_JSON_DECODER_99_018:[ A JSON value MUST be an object, array, number, or string, or one of the following three literal names: false null true] */
TEST_FUNCTION(JSONDecoder_When_The_JSON_Contains_An_Array_With_A_false_value_Decoding_Succeeds)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "[false]";
    void* value1Ptr = &json[1];

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "0", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));
    STRICT_EXPECTED_CALL(mocks, MultiTree_SetValue(TestChildHandle1, value1Ptr));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_OK, result);
}

/* Tests_SRS_JSON_DECODER_99_018:[ A JSON value MUST be an object, array, number, or string, or one of the following three literal names: false null true] */
TEST_FUNCTION(JSONDecoder_When_The_JSON_Contains_An_Array_With_A_true_value_Decoding_Succeeds)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "[true]";
    void* value1Ptr = &json[1];

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "0", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));
    STRICT_EXPECTED_CALL(mocks, MultiTree_SetValue(TestChildHandle1, value1Ptr));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_OK, result);
}

/* Tests_SRS_JSON_DECODER_99_018:[ A JSON value MUST be an object, array, number, or string, or one of the following three literal names: false null true] */
TEST_FUNCTION(JSONDecoder_When_The_JSON_Contains_An_Array_With_A_null_value_Decoding_Succeeds)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "[null]";
    void* value1Ptr = &json[1];

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "0", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));
    STRICT_EXPECTED_CALL(mocks, MultiTree_SetValue(TestChildHandle1, value1Ptr));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_OK, result);
}

/* Tests_SRS_JSON_DECODER_99_019:[ The literal names MUST be lowercase.] */
/* Tests_SRS_JSON_DECODER_99_007:[ If parsing the JSON fails due to the JSON string being malformed, JSONDecoder_JSON_To_MultiTree shall return JSON_DECODER_PARSE_ERROR.] */
TEST_FUNCTION(JSONDecoder_When_The_JSON_Contains_An_Array_With_A_false_But_Not_All_Chars_Are_lowercase_Decoding_Fails)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "[fAlse]";

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "0", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));
    STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TestMultiTreeHandle));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_PARSE_ERROR, result);
}

/* Tests_SRS_JSON_DECODER_99_019:[ The literal names MUST be lowercase.] */
/* Tests_SRS_JSON_DECODER_99_007:[ If parsing the JSON fails due to the JSON string being malformed, JSONDecoder_JSON_To_MultiTree shall return JSON_DECODER_PARSE_ERROR.] */
TEST_FUNCTION(JSONDecoder_When_The_JSON_Contains_An_Array_With_A_true_But_Not_All_Chars_Are_lowercase_Decoding_Fails)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "[trUe]";

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "0", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));
    STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TestMultiTreeHandle));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_PARSE_ERROR, result);
}

/* Tests_SRS_JSON_DECODER_99_019:[ The literal names MUST be lowercase.] */
/* Tests_SRS_JSON_DECODER_99_007:[ If parsing the JSON fails due to the JSON string being malformed, JSONDecoder_JSON_To_MultiTree shall return JSON_DECODER_PARSE_ERROR.] */
TEST_FUNCTION(JSONDecoder_When_The_JSON_Contains_An_Array_With_A_null_But_Not_All_Chars_Are_lowercase_Decoding_Fails)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "[Null]";

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "0", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));
    STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TestMultiTreeHandle));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_PARSE_ERROR, result);
}

/* Tests_SRS_JSON_DECODER_99_020:[ No other literal names are allowed.] */
/* Checking only for one literal name that's not in the spec */
TEST_FUNCTION(JSONDecoder_When_The_JSON_Contains_An_Array_With_A_Literal_Name_That_Is_Not_Accepted_Decoding_Fails)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "[hagauaga]";

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "0", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));
    STRICT_EXPECTED_CALL(mocks, MultiTree_Destroy(TestMultiTreeHandle));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_PARSE_ERROR, result);
}

/* Tests_SRS_JSON_DECODER_99_037:[    These are the six structural characters: ... ] */
/* Tests_SRS_JSON_DECODER_99_016:[    Insignificant whitespace is allowed before or after any of the six structural characters.] */
/* Tests_SRS_JSON_DECODER_99_017:[ %x20 /              ; Space ] */
TEST_FUNCTION(JSONDecoder_Space_Before_Array_Begin_Is_Allowed_And_Decoding_Succeeds)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = " [true]";
    void* value1Ptr = &json[2];

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "0", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));
    STRICT_EXPECTED_CALL(mocks, MultiTree_SetValue(TestChildHandle1, value1Ptr));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_OK, result);
}

/* Tests_SRS_JSON_DECODER_99_037:[    These are the six structural characters: ... ] */
/* Tests_SRS_JSON_DECODER_99_016:[    Insignificant whitespace is allowed before or after any of the six structural characters.] */
/* Tests_SRS_JSON_DECODER_99_042:[ %x0D                ; Carriage return] */
TEST_FUNCTION(JSONDecoder_CR_Before_Array_Begin_Is_Allowed_And_Decoding_Succeeds)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "\r[true]";
    void* value1Ptr = &json[2];

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "0", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));
    STRICT_EXPECTED_CALL(mocks, MultiTree_SetValue(TestChildHandle1, value1Ptr));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_OK, result);
}

/* Tests_SRS_JSON_DECODER_99_037:[    These are the six structural characters: ... ] */
/* Tests_SRS_JSON_DECODER_99_016:[    Insignificant whitespace is allowed before or after any of the six structural characters.] */
/* Tests_SRS_JSON_DECODER_99_041:[ %x0A /              ; Line feed or New line] */
TEST_FUNCTION(JSONDecoder_NL_Before_Array_Begin_Is_Allowed_And_Decoding_Succeeds)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "\n[true]";
    void* value1Ptr = &json[2];

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "0", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));
    STRICT_EXPECTED_CALL(mocks, MultiTree_SetValue(TestChildHandle1, value1Ptr));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_OK, result);
}

/* Tests_SRS_JSON_DECODER_99_037:[    These are the six structural characters: ... ] */
/* Tests_SRS_JSON_DECODER_99_016:[    Insignificant whitespace is allowed before or after any of the six structural characters.] */
/* Tests_SRS_JSON_DECODER_99_040:[ %x09 /              ; Horizontal tab] */
TEST_FUNCTION(JSONDecoder_Tab_Before_Array_Begin_Is_Allowed_And_Decoding_Succeeds)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "\t[true]";
    void* value1Ptr = &json[2];

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "0", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));
    STRICT_EXPECTED_CALL(mocks, MultiTree_SetValue(TestChildHandle1, value1Ptr));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_OK, result);
}

/* Tests_SRS_JSON_DECODER_99_037:[    These are the six structural characters: ... ] */
/* Tests_SRS_JSON_DECODER_99_016:[    Insignificant whitespace is allowed before or after any of the six structural characters.] */
TEST_FUNCTION(JSONDecoder_Multiple_Whitespaces_Before_Array_Begin_Is_Allowed_And_Decoding_Succeeds)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = " \t\r\n[true]";
    void* value1Ptr = &json[5];

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "0", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));
    STRICT_EXPECTED_CALL(mocks, MultiTree_SetValue(TestChildHandle1, value1Ptr));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_OK, result);
}

/* Tests_SRS_JSON_DECODER_99_037:[    These are the six structural characters: ... ] */
/* Tests_SRS_JSON_DECODER_99_016:[    Insignificant whitespace is allowed before or after any of the six structural characters.] */
/* Tests_SRS_JSON_DECODER_99_017:[ %x20 /              ; Space ] */
TEST_FUNCTION(JSONDecoder_Space_After_Array_Begin_Is_Allowed_And_Decoding_Succeeds)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "[ true]";
    void* value1Ptr = &json[2];

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "0", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));
    STRICT_EXPECTED_CALL(mocks, MultiTree_SetValue(TestChildHandle1, value1Ptr));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_OK, result);
}

/* Tests_SRS_JSON_DECODER_99_037:[    These are the six structural characters: ... ] */
/* Tests_SRS_JSON_DECODER_99_016:[    Insignificant whitespace is allowed before or after any of the six structural characters.] */
/* Tests_SRS_JSON_DECODER_99_042:[ %x0D                ; Carriage return] */
TEST_FUNCTION(JSONDecoder_CR_After_Array_Begin_Is_Allowed_And_Decoding_Succeeds)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "[\rtrue]";
    void* value1Ptr = &json[2];

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "0", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));
    STRICT_EXPECTED_CALL(mocks, MultiTree_SetValue(TestChildHandle1, value1Ptr));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_OK, result);
}

/* Tests_SRS_JSON_DECODER_99_037:[    These are the six structural characters: ... ] */
/* Tests_SRS_JSON_DECODER_99_016:[    Insignificant whitespace is allowed before or after any of the six structural characters.] */
/* Tests_SRS_JSON_DECODER_99_041:[ %x0A /              ; Line feed or New line] */
TEST_FUNCTION(JSONDecoder_NL_After_Array_Begin_Is_Allowed_And_Decoding_Succeeds)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "[\ntrue]";
    void* value1Ptr = &json[2];

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "0", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));
    STRICT_EXPECTED_CALL(mocks, MultiTree_SetValue(TestChildHandle1, value1Ptr));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_OK, result);
}

/* Tests_SRS_JSON_DECODER_99_037:[    These are the six structural characters: ... ] */
/* Tests_SRS_JSON_DECODER_99_016:[    Insignificant whitespace is allowed before or after any of the six structural characters.] */
/* Tests_SRS_JSON_DECODER_99_040:[ %x09 /              ; Horizontal tab] */
TEST_FUNCTION(JSONDecoder_Tab_After_Array_Begin_Is_Allowed_And_Decoding_Succeeds)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "[\ttrue]";
    void* value1Ptr = &json[2];

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "0", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));
    STRICT_EXPECTED_CALL(mocks, MultiTree_SetValue(TestChildHandle1, value1Ptr));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_OK, result);
}

/* Tests_SRS_JSON_DECODER_99_037:[    These are the six structural characters: ... ] */
/* Tests_SRS_JSON_DECODER_99_016:[    Insignificant whitespace is allowed before or after any of the six structural characters.] */
TEST_FUNCTION(JSONDecoder_Multiple_Whitespaces_After_Array_Begin_Is_Allowed_And_Decoding_Succeeds)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "[ \t\r\ntrue]";
    void* value1Ptr = &json[5];

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "0", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));
    STRICT_EXPECTED_CALL(mocks, MultiTree_SetValue(TestChildHandle1, value1Ptr));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_OK, result);
}

/* Tests_SRS_JSON_DECODER_99_037:[    These are the six structural characters: ... ] */
/* Tests_SRS_JSON_DECODER_99_016:[    Insignificant whitespace is allowed before or after any of the six structural characters.] */
TEST_FUNCTION(JSONDecoder_Multiple_Whitespaces_Before_Array_End_Is_Allowed_And_Decoding_Succeeds)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "[true \t\r\n]";
    void* value1Ptr = &json[1];

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "0", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));
    STRICT_EXPECTED_CALL(mocks, MultiTree_SetValue(TestChildHandle1, value1Ptr));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_OK, result);
}

/* Tests_SRS_JSON_DECODER_99_037:[    These are the six structural characters: ... ] */
/* Tests_SRS_JSON_DECODER_99_016:[    Insignificant whitespace is allowed before or after any of the six structural characters.] */
TEST_FUNCTION(JSONDecoder_Multiple_Whitespaces_After_Array_End_Is_Allowed_And_Decoding_Succeeds)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "[true] \t\r\n";
    void* value1Ptr = &json[1];

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "0", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));
    STRICT_EXPECTED_CALL(mocks, MultiTree_SetValue(TestChildHandle1, value1Ptr));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_OK, result);
}

/* Tests_SRS_JSON_DECODER_99_037:[    These are the six structural characters: ... ] */
/* Tests_SRS_JSON_DECODER_99_016:[    Insignificant whitespace is allowed before or after any of the six structural characters.] */
TEST_FUNCTION(JSONDecoder_Multiple_Whitespaces_Before_Comma_In_Array_Is_Allowed_And_Decoding_Succeeds)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "[true \t\r\n,false]";
    void* value1Ptr = &json[1];
    void* value2Ptr = &json[10];

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "0", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));
    STRICT_EXPECTED_CALL(mocks, MultiTree_SetValue(TestChildHandle1, value1Ptr));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "1", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle2, sizeof(TestChildHandle2));
    STRICT_EXPECTED_CALL(mocks, MultiTree_SetValue(TestChildHandle2, value2Ptr));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_OK, result);
}

/* Tests_SRS_JSON_DECODER_99_037:[    These are the six structural characters: ... ] */
/* Tests_SRS_JSON_DECODER_99_016:[    Insignificant whitespace is allowed before or after any of the six structural characters.] */
TEST_FUNCTION(JSONDecoder_Multiple_Whitespaces_After_Comma_In_Array_Is_Allowed_And_Decoding_Succeeds)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "[true, \t\r\nfalse]";
    void* value1Ptr = &json[1];
    void* value2Ptr = &json[10];

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "0", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));
    STRICT_EXPECTED_CALL(mocks, MultiTree_SetValue(TestChildHandle1, value1Ptr));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "1", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle2, sizeof(TestChildHandle2));
    STRICT_EXPECTED_CALL(mocks, MultiTree_SetValue(TestChildHandle2, value2Ptr));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_OK, result);
}

/* Tests_SRS_JSON_DECODER_99_037:[    These are the six structural characters: ... ] */
/* Tests_SRS_JSON_DECODER_99_016:[    Insignificant whitespace is allowed before or after any of the six structural characters.] */
TEST_FUNCTION(JSONDecoder_Multiple_Whitespaces_Before_Object_Begin_Is_Allowed_And_Decoding_Succeeds)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = " \t\r\n{\"a\":true}";
    void* value1Ptr = &json[9];

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "a", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));
    STRICT_EXPECTED_CALL(mocks, MultiTree_SetValue(TestChildHandle1, value1Ptr));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_OK, result);
}

/* Tests_SRS_JSON_DECODER_99_037:[    These are the six structural characters: ... ] */
/* Tests_SRS_JSON_DECODER_99_016:[    Insignificant whitespace is allowed before or after any of the six structural characters.] */
TEST_FUNCTION(JSONDecoder_Multiple_Whitespaces_After_Object_Begin_Is_Allowed_And_Decoding_Succeeds)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "{ \t\r\n\"a\":true}";
    void* value1Ptr = &json[9];

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "a", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));
    STRICT_EXPECTED_CALL(mocks, MultiTree_SetValue(TestChildHandle1, value1Ptr));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_OK, result);
}

/* Tests_SRS_JSON_DECODER_99_037:[    These are the six structural characters: ... ] */
/* Tests_SRS_JSON_DECODER_99_016:[    Insignificant whitespace is allowed before or after any of the six structural characters.] */
TEST_FUNCTION(JSONDecoder_Multiple_Whitespaces_Before_Object_End_Is_Allowed_And_Decoding_Succeeds)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "{\"a\":true \t\r\n}";
    void* value1Ptr = &json[5];

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "a", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));
    STRICT_EXPECTED_CALL(mocks, MultiTree_SetValue(TestChildHandle1, value1Ptr));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_OK, result);
}

/* Tests_SRS_JSON_DECODER_99_037:[    These are the six structural characters: ... ] */
/* Tests_SRS_JSON_DECODER_99_016:[    Insignificant whitespace is allowed before or after any of the six structural characters.] */
TEST_FUNCTION(JSONDecoder_Multiple_Whitespaces_After_Object_End_Is_Allowed_And_Decoding_Succeeds)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "{\"a\":true} \t\r\n";
    void* value1Ptr = &json[5];

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "a", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));
    STRICT_EXPECTED_CALL(mocks, MultiTree_SetValue(TestChildHandle1, value1Ptr));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_OK, result);
}

/* Tests_SRS_JSON_DECODER_99_037:[    These are the six structural characters: ... ] */
/* Tests_SRS_JSON_DECODER_99_016:[    Insignificant whitespace is allowed before or after any of the six structural characters.] */
TEST_FUNCTION(JSONDecoder_Multiple_Whitespaces_Before_Colon_Is_Allowed_And_Decoding_Succeeds)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "{\"a\" \t\r\n:true}";
    void* value1Ptr = &json[9];

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "a", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));
    STRICT_EXPECTED_CALL(mocks, MultiTree_SetValue(TestChildHandle1, value1Ptr));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_OK, result);
}

/* Tests_SRS_JSON_DECODER_99_037:[    These are the six structural characters: ... ] */
/* Tests_SRS_JSON_DECODER_99_016:[    Insignificant whitespace is allowed before or after any of the six structural characters.] */
TEST_FUNCTION(JSONDecoder_Multiple_Whitespaces_After_Colon_Is_Allowed_And_Decoding_Succeeds)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "{\"a\": \t\r\ntrue}";
    void* value1Ptr = &json[9];

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "a", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));
    STRICT_EXPECTED_CALL(mocks, MultiTree_SetValue(TestChildHandle1, value1Ptr));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_OK, result);
}

/* Tests_SRS_JSON_DECODER_99_037:[    These are the six structural characters: ... ] */
/* Tests_SRS_JSON_DECODER_99_016:[    Insignificant whitespace is allowed before or after any of the six structural characters.] */
TEST_FUNCTION(JSONDecoder_Multiple_Whitespaces_Before_Comma_In_Object_Is_Allowed_And_Decoding_Succeeds)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "{\"a\":true \t\r\n,\"b\":false}";
    void* value1Ptr = &json[5];
    void* value2Ptr = &json[18];

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "a", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));
    STRICT_EXPECTED_CALL(mocks, MultiTree_SetValue(TestChildHandle1, value1Ptr));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "b", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle2, sizeof(TestChildHandle2));
    STRICT_EXPECTED_CALL(mocks, MultiTree_SetValue(TestChildHandle2, value2Ptr));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_OK, result);
}

/* Tests_SRS_JSON_DECODER_99_037:[    These are the six structural characters: ... ] */
/* Tests_SRS_JSON_DECODER_99_016:[    Insignificant whitespace is allowed before or after any of the six structural characters.] */
TEST_FUNCTION(JSONDecoder_Multiple_Whitespaces_After_Comma_In_Object_Is_Allowed_And_Decoding_Succeeds)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "{\"a\":true, \t\r\n\"b\":false}";
    void* value1Ptr = &json[5];
    void* value2Ptr = &json[18];

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "a", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));
    STRICT_EXPECTED_CALL(mocks, MultiTree_SetValue(TestChildHandle1, value1Ptr));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "b", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle2, sizeof(TestChildHandle2));
    STRICT_EXPECTED_CALL(mocks, MultiTree_SetValue(TestChildHandle2, value2Ptr));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_OK, result);
}

/* Tests_SRS_JSON_DECODER_99_018:[ A JSON value MUST be an object, array, number, or string, or one of the following three literal names: false null true] */
TEST_FUNCTION(JSONDecoder_Nested_Array_Are_Allowed_And_Decoding_Succeeds)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "[[]]";

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "0", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_OK, result);
}

/* Tests_SRS_JSON_DECODER_99_018:[ A JSON value MUST be an object, array, number, or string, or one of the following three literal names: false null true] */
TEST_FUNCTION(JSONDecoder_Spaces_Before_Nested_Array_Are_Allowed_And_Decoding_Succeeds)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "[ \t\r\n[]]";

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "0", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_OK, result);
}

/* Tests_SRS_JSON_DECODER_99_018:[ A JSON value MUST be an object, array, number, or string, or one of the following three literal names: false null true] */
TEST_FUNCTION(JSONDecoder_Spaces_In_Nested_Array_Are_Allowed_And_Decoding_Succeeds)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "[[ \t\r\n]]";

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "0", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_OK, result);
}

/* Tests_SRS_JSON_DECODER_99_018:[ A JSON value MUST be an object, array, number, or string, or one of the following three literal names: false null true] */
TEST_FUNCTION(JSONDecoder_Spaces_After_Nested_Array_Are_Allowed_And_Decoding_Succeeds)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "[[ \t\r\n]]";

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "0", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_OK, result);
}

/* Tests_SRS_JSON_DECODER_99_018:[ A JSON value MUST be an object, array, number, or string, or one of the following three literal names: false null true] */
TEST_FUNCTION(JSONDecoder_Object_In_Array_Is_Allowed_And_Decoding_Succeeds)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "[{}]";

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "0", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_OK, result);
}

/* Tests_SRS_JSON_DECODER_99_018:[ A JSON value MUST be an object, array, number, or string, or one of the following three literal names: false null true] */
TEST_FUNCTION(JSONDecoder_Spaces_Before_Object_In_Array_Are_Allowed_And_Decoding_Succeeds)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "[ \t\r\n{}]";

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "0", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_OK, result);
}

/* Tests_SRS_JSON_DECODER_99_018:[ A JSON value MUST be an object, array, number, or string, or one of the following three literal names: false null true] */
TEST_FUNCTION(JSONDecoder_Spaces_In_Object_In_Array_Are_Allowed_And_Decoding_Succeeds)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "[{ \t\r\n}]";

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "0", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_OK, result);
}

/* Tests_SRS_JSON_DECODER_99_018:[ A JSON value MUST be an object, array, number, or string, or one of the following three literal names: false null true] */
TEST_FUNCTION(JSONDecoder_Spaces_After_Object_In_Array_Are_Allowed_And_Decoding_Succeeds)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "[{} \t\r\n]";

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "0", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_OK, result);
}

/* Tests_SRS_JSON_DECODER_99_018:[ A JSON value MUST be an object, array, number, or string, or one of the following three literal names: false null true] */
TEST_FUNCTION(JSONDecoder_Value_In_Object_In_Array_Are_Allowed_And_Decoding_Succeeds)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "[{\"member1\":\"a\"}]";
    void* value1Ptr = &json[12];

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "0", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestChildHandle1, "member1", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle2, sizeof(TestChildHandle2));
    STRICT_EXPECTED_CALL(mocks, MultiTree_SetValue(TestChildHandle2, value1Ptr));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_OK, result);
}

/* Tests_SRS_JSON_DECODER_99_018:[ A JSON value MUST be an object, array, number, or string, or one of the following three literal names: false null true] */
TEST_FUNCTION(JSONDecoder_Spaces_Before_Member_Name_In_Object_In_Array_Are_Allowed_And_Decoding_Succeeds)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "[{ \r\n\t\"member1\":\"a\"}]";
    void* value1Ptr = &json[16];

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "0", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestChildHandle1, "member1", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle2, sizeof(TestChildHandle2));
    STRICT_EXPECTED_CALL(mocks, MultiTree_SetValue(TestChildHandle2, value1Ptr));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_OK, result);
}

/* Tests_SRS_JSON_DECODER_99_018:[ A JSON value MUST be an object, array, number, or string, or one of the following three literal names: false null true] */
TEST_FUNCTION(JSONDecoder_Spaces_Before_Colon_In_Object_In_Array_Are_Allowed_And_Decoding_Succeeds)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "[{\"member1\" \r\n\t:\"a\"}]";
    void* value1Ptr = &json[16];

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "0", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestChildHandle1, "member1", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle2, sizeof(TestChildHandle2));
    STRICT_EXPECTED_CALL(mocks, MultiTree_SetValue(TestChildHandle2, value1Ptr));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_OK, result);
}

/* Tests_SRS_JSON_DECODER_99_018:[ A JSON value MUST be an object, array, number, or string, or one of the following three literal names: false null true] */
TEST_FUNCTION(JSONDecoder_Spaces_After_Colon_In_Object_In_Array_Are_Allowed_And_Decoding_Succeeds)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "[{\"member1\": \r\n\t\"a\"}]";
    void* value1Ptr = &json[16];

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "0", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestChildHandle1, "member1", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle2, sizeof(TestChildHandle2));
    STRICT_EXPECTED_CALL(mocks, MultiTree_SetValue(TestChildHandle2, value1Ptr));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_OK, result);
}

/* Tests_SRS_JSON_DECODER_99_018:[ A JSON value MUST be an object, array, number, or string, or one of the following three literal names: false null true] */
TEST_FUNCTION(JSONDecoder_Spaces_After_Value_In_Object_In_Array_Are_Allowed_And_Decoding_Succeeds)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "[{\"member1\":\"a\" \r\n\t}]";
    void* value1Ptr = &json[12];

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "0", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestChildHandle1, "member1", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle2, sizeof(TestChildHandle2));
    STRICT_EXPECTED_CALL(mocks, MultiTree_SetValue(TestChildHandle2, value1Ptr));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_OK, result);
}

/* Tests_SRS_JSON_DECODER_99_018:[ A JSON value MUST be an object, array, number, or string, or one of the following three literal names: false null true] */
TEST_FUNCTION(JSONDecoder_Spaces_Before_Comma_In_Object_In_Array_Are_Allowed_And_Decoding_Succeeds)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "[{\"member1\":\"a\" \r\n\t,\"member2\":\"b\"}]";
    void* value1Ptr = &json[12];
    void* value2Ptr = &json[30];

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "0", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestChildHandle1, "member1", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle2, sizeof(TestChildHandle2));
    STRICT_EXPECTED_CALL(mocks, MultiTree_SetValue(TestChildHandle2, value1Ptr));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestChildHandle1, "member2", IGNORED_PTR_ARG)
        .CopyOutArgumentBuffer(3, &TestChildHandle3, sizeof(TestChildHandle3)));
    STRICT_EXPECTED_CALL(mocks, MultiTree_SetValue(TestChildHandle3, value2Ptr));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_OK, result);
}

/* Tests_SRS_JSON_DECODER_99_018:[ A JSON value MUST be an object, array, number, or string, or one of the following three literal names: false null true] */
TEST_FUNCTION(JSONDecoder_Spaces_After_Comma_In_Object_In_Array_Are_Allowed_And_Decoding_Succeeds)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "[{\"member1\":\"a\", \r\n\t\"member2\":\"b\"}]";
    void* value1Ptr = &json[12];
    void* value2Ptr = &json[30];

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "0", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestChildHandle1, "member1", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle2, sizeof(TestChildHandle2));
    STRICT_EXPECTED_CALL(mocks, MultiTree_SetValue(TestChildHandle2, value1Ptr));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestChildHandle1, "member2", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle3, sizeof(TestChildHandle3));
    STRICT_EXPECTED_CALL(mocks, MultiTree_SetValue(TestChildHandle3, value2Ptr));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_OK, result);
}

/* Tests_SRS_JSON_DECODER_99_018:[ A JSON value MUST be an object, array, number, or string, or one of the following three literal names: false null true] */
TEST_FUNCTION(JSONDecoder_Spaces_Before_Value_In_Array_In_Array_Are_Allowed_And_Decoding_Succeeds)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "[[ \r\n\t\"a\"]]";
    void* value1Ptr = &json[6];

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "0", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestChildHandle1, "0", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle2, sizeof(TestChildHandle2));
    STRICT_EXPECTED_CALL(mocks, MultiTree_SetValue(TestChildHandle2, value1Ptr));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_OK, result);
}

/* Tests_SRS_JSON_DECODER_99_018:[ A JSON value MUST be an object, array, number, or string, or one of the following three literal names: false null true] */
TEST_FUNCTION(JSONDecoder_Spaces_After_Value_In_Array_In_Array_Are_Allowed_And_Decoding_Succeeds)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "[[\"a\" \r\n\t]]";
    void* value1Ptr = &json[2];

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "0", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestChildHandle1, "0", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle2, sizeof(TestChildHandle2));
    STRICT_EXPECTED_CALL(mocks, MultiTree_SetValue(TestChildHandle2, value1Ptr));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_OK, result);
}

/* Tests_SRS_JSON_DECODER_99_018:[ A JSON value MUST be an object, array, number, or string, or one of the following three literal names: false null true] */
TEST_FUNCTION(JSONDecoder_Spaces_Before_Comma_In_Array_In_Array_Are_Allowed_And_Decoding_Succeeds)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "[[\"a\" \r\n\t,\"b\"]]";
    void* value1Ptr = &json[2];
    void* value2Ptr = &json[10];

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "0", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestChildHandle1, "0", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle2, sizeof(TestChildHandle2));
    STRICT_EXPECTED_CALL(mocks, MultiTree_SetValue(TestChildHandle2, value1Ptr));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestChildHandle1, "1", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle3, sizeof(TestChildHandle3));
    STRICT_EXPECTED_CALL(mocks, MultiTree_SetValue(TestChildHandle3, value2Ptr));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_OK, result);
}

/* Tests_SRS_JSON_DECODER_99_043:[ A number contains an integer component that may be prefixed with an optional minus sign, which may be followed by a fraction part and/or an exponent part.] */
TEST_FUNCTION(JSONDecoder_Number_Without_Sign_Is_Decoded_Succesfully)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "[1]";
    void* value1Ptr = &json[1];

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "0", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));
    STRICT_EXPECTED_CALL(mocks, MultiTree_SetValue(TestChildHandle1, value1Ptr));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_OK, result);
}

/* Tests_SRS_JSON_DECODER_99_043:[ A number contains an integer component that may be prefixed with an optional minus sign, which may be followed by a fraction part and/or an exponent part.] */
TEST_FUNCTION(JSONDecoder_Number_Made_Out_Of_Several_Digits_Without_Sign_Is_Decoded_Succesfully)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "[4242]";
    void* value1Ptr = &json[1];

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "0", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));
    STRICT_EXPECTED_CALL(mocks, MultiTree_SetValue(TestChildHandle1, value1Ptr));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_OK, result);
}

/* Tests_SRS_JSON_DECODER_99_043:[ A number contains an integer component that may be prefixed with an optional minus sign, which may be followed by a fraction part and/or an exponent part.] */
TEST_FUNCTION(JSONDecoder_Number_With_Minus_Sign_Is_Decoded_Succesfully)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "[-4242]";
    void* value1Ptr = &json[1];

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "0", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));
    STRICT_EXPECTED_CALL(mocks, MultiTree_SetValue(TestChildHandle1, value1Ptr));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_OK, result);
}

/* Tests_SRS_JSON_DECODER_99_043:[ A number contains an integer component that may be prefixed with an optional minus sign, which may be followed by a fraction part and/or an exponent part.] */
TEST_FUNCTION(JSONDecoder_Number_With_2_Minus_Sign_Fails)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "[--4242]";

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "0", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));
    EXPECTED_CALL(mocks, MultiTree_Destroy(TestMultiTreeHandle));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_PARSE_ERROR, result);
}

/* Tests_SRS_JSON_DECODER_99_043:[ A number contains an integer component that may be prefixed with an optional minus sign, which may be followed by a fraction part and/or an exponent part.] */
TEST_FUNCTION(JSONDecoder_Minus_In_The_Middle_Of_Number_Fails)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "[42-42]";
    void* value1Ptr = &json[1];

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "0", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));
    STRICT_EXPECTED_CALL(mocks, MultiTree_SetValue(TestChildHandle1, value1Ptr));
    EXPECTED_CALL(mocks, MultiTree_Destroy(TestMultiTreeHandle));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_PARSE_ERROR, result);
}

/* Tests_SRS_JSON_DECODER_99_043:[ A number contains an integer component that may be prefixed with an optional minus sign, which may be followed by a fraction part and/or an exponent part.] */
/* Tests_SRS_JSON_DECODER_99_046:[ A fraction part is a decimal point followed by one or more digits.] */
TEST_FUNCTION(JSONDecoder_Fraction_Part_Without_Integer_Fails)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "[.1]";

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "0", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));
    EXPECTED_CALL(mocks, MultiTree_Destroy(TestMultiTreeHandle));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_PARSE_ERROR, result);
}

/* Tests_SRS_JSON_DECODER_99_043:[ A number contains an integer component that may be prefixed with an optional minus sign, which may be followed by a fraction part and/or an exponent part.] */
/* Tests_SRS_JSON_DECODER_99_046:[ A fraction part is a decimal point followed by one or more digits.] */
TEST_FUNCTION(JSONDecoder_No_Fraction_Part_Digits_Fails)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "[1.]";

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "0", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));
    EXPECTED_CALL(mocks, MultiTree_Destroy(TestMultiTreeHandle));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_PARSE_ERROR, result);
}

/* Tests_SRS_JSON_DECODER_99_043:[ A number contains an integer component that may be prefixed with an optional minus sign, which may be followed by a fraction part and/or an exponent part.] */
/* Tests_SRS_JSON_DECODER_99_046:[ A fraction part is a decimal point followed by one or more digits.] */
TEST_FUNCTION(JSONDecoder_Integer_With_Fraction_Part_Succeeds)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "[1.1]";
    void* value1Ptr = &json[1];

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "0", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));
    STRICT_EXPECTED_CALL(mocks, MultiTree_SetValue(TestChildHandle1, value1Ptr));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_OK, result);
}

/* Tests_SRS_JSON_DECODER_99_043:[ A number contains an integer component that may be prefixed with an optional minus sign, which may be followed by a fraction part and/or an exponent part.] */
/* Tests_SRS_JSON_DECODER_99_047:[ An exponent part begins with the letter E in upper or lowercase, which may be followed by a plus or minus sign.] */
/* Tests_SRS_JSON_DECODER_99_048:[ The E and optional sign are followed by one or more digits.] */
TEST_FUNCTION(JSONDecoder_Integer_With_e_And_Following_Digits_Succeeds)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "[1e1]";
    void* value1Ptr = &json[1];

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "0", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));
    STRICT_EXPECTED_CALL(mocks, MultiTree_SetValue(TestChildHandle1, value1Ptr));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_OK, result);
}

/* Tests_SRS_JSON_DECODER_99_043:[ A number contains an integer component that may be prefixed with an optional minus sign, which may be followed by a fraction part and/or an exponent part.] */
/* Tests_SRS_JSON_DECODER_99_047:[ An exponent part begins with the letter E in upper or lowercase, which may be followed by a plus or minus sign.] */
/* Tests_SRS_JSON_DECODER_99_048:[ The E and optional sign are followed by one or more digits.] */
TEST_FUNCTION(JSONDecoder_Integer_With_e_And_2_Digits_Succeeds)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "[1e42]";
    void* value1Ptr = &json[1];

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "0", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));
    STRICT_EXPECTED_CALL(mocks, MultiTree_SetValue(TestChildHandle1, value1Ptr));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_OK, result);
}

/* Tests_SRS_JSON_DECODER_99_043:[ A number contains an integer component that may be prefixed with an optional minus sign, which may be followed by a fraction part and/or an exponent part.] */
/* Tests_SRS_JSON_DECODER_99_047:[ An exponent part begins with the letter E in upper or lowercase, which may be followed by a plus or minus sign.] */
/* Tests_SRS_JSON_DECODER_99_048:[ The E and optional sign are followed by one or more digits.] */
TEST_FUNCTION(JSONDecoder_Integer_With_e_And_Minus_And_2_Digits_Succeeds)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "[1e-42]";
    void* value1Ptr = &json[1];

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "0", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));
    STRICT_EXPECTED_CALL(mocks, MultiTree_SetValue(TestChildHandle1, value1Ptr));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_OK, result);
}

/* Tests_SRS_JSON_DECODER_99_043:[ A number contains an integer component that may be prefixed with an optional minus sign, which may be followed by a fraction part and/or an exponent part.] */
/* Tests_SRS_JSON_DECODER_99_047:[ An exponent part begins with the letter E in upper or lowercase, which may be followed by a plus or minus sign.] */
/* Tests_SRS_JSON_DECODER_99_048:[ The E and optional sign are followed by one or more digits.] */
TEST_FUNCTION(JSONDecoder_Integer_With_e_And_Plux_And_2_Digits_Succeeds)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "[1e+42]";
    void* value1Ptr = &json[1];

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "0", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));
    STRICT_EXPECTED_CALL(mocks, MultiTree_SetValue(TestChildHandle1, value1Ptr));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_OK, result);
}

/* Tests_SRS_JSON_DECODER_99_043:[ A number contains an integer component that may be prefixed with an optional minus sign, which may be followed by a fraction part and/or an exponent part.] */
/* Tests_SRS_JSON_DECODER_99_047:[ An exponent part begins with the letter E in upper or lowercase, which may be followed by a plus or minus sign.] */
/* Tests_SRS_JSON_DECODER_99_048:[ The E and optional sign are followed by one or more digits.] */
TEST_FUNCTION(JSONDecoder_Integer_With_E_And_Following_Digits_Succeeds)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "[1E1]";
    void* value1Ptr = &json[1];

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "0", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));
    STRICT_EXPECTED_CALL(mocks, MultiTree_SetValue(TestChildHandle1, value1Ptr));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_OK, result);
}

/* Tests_SRS_JSON_DECODER_99_043:[ A number contains an integer component that may be prefixed with an optional minus sign, which may be followed by a fraction part and/or an exponent part.] */
/* Tests_SRS_JSON_DECODER_99_047:[ An exponent part begins with the letter E in upper or lowercase, which may be followed by a plus or minus sign.] */
/* Tests_SRS_JSON_DECODER_99_048:[ The E and optional sign are followed by one or more digits.] */
TEST_FUNCTION(JSONDecoder_Integer_With_E_And_2_Digits_Succeeds)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "[1E42]";
    void* value1Ptr = &json[1];

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "0", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));
    STRICT_EXPECTED_CALL(mocks, MultiTree_SetValue(TestChildHandle1, value1Ptr));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_OK, result);
}

/* Tests_SRS_JSON_DECODER_99_043:[ A number contains an integer component that may be prefixed with an optional minus sign, which may be followed by a fraction part and/or an exponent part.] */
/* Tests_SRS_JSON_DECODER_99_047:[ An exponent part begins with the letter E in upper or lowercase, which may be followed by a plus or minus sign.] */
/* Tests_SRS_JSON_DECODER_99_048:[ The E and optional sign are followed by one or more digits.] */
TEST_FUNCTION(JSONDecoder_Integer_With_E_And_Minus_And_2_Digits_Succeeds)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "[1E-42]";
    void* value1Ptr = &json[1];

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "0", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));
    STRICT_EXPECTED_CALL(mocks, MultiTree_SetValue(TestChildHandle1, value1Ptr));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_OK, result);
}

/* Tests_SRS_JSON_DECODER_99_043:[ A number contains an integer component that may be prefixed with an optional minus sign, which may be followed by a fraction part and/or an exponent part.] */
/* Tests_SRS_JSON_DECODER_99_047:[ An exponent part begins with the letter E in upper or lowercase, which may be followed by a plus or minus sign.] */
/* Tests_SRS_JSON_DECODER_99_048:[ The E and optional sign are followed by one or more digits.] */
TEST_FUNCTION(JSONDecoder_Integer_With_E_And_Plux_And_2_Digits_Succeeds)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "[1E+42]";
    void* value1Ptr = &json[1];

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "0", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));
    STRICT_EXPECTED_CALL(mocks, MultiTree_SetValue(TestChildHandle1, value1Ptr));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_OK, result);
}

/* Tests_SRS_JSON_DECODER_99_043:[ A number contains an integer component that may be prefixed with an optional minus sign, which may be followed by a fraction part and/or an exponent part.] */
/* Tests_SRS_JSON_DECODER_99_047:[ An exponent part begins with the letter E in upper or lowercase, which may be followed by a plus or minus sign.] */
/* Tests_SRS_JSON_DECODER_99_048:[ The E and optional sign are followed by one or more digits.] */
TEST_FUNCTION(JSONDecoder_Integer_With_e_With_No_Digits_Fails)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "[1e]";

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "0", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));
    EXPECTED_CALL(mocks, MultiTree_Destroy(TestMultiTreeHandle));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_PARSE_ERROR, result);
}

/* Tests_SRS_JSON_DECODER_99_043:[ A number contains an integer component that may be prefixed with an optional minus sign, which may be followed by a fraction part and/or an exponent part.] */
/* Tests_SRS_JSON_DECODER_99_047:[ An exponent part begins with the letter E in upper or lowercase, which may be followed by a plus or minus sign.] */
/* Tests_SRS_JSON_DECODER_99_048:[ The E and optional sign are followed by one or more digits.] */
TEST_FUNCTION(JSONDecoder_Integer_With_E_With_No_Digits_Fails)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "[1E]";

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "0", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));
    EXPECTED_CALL(mocks, MultiTree_Destroy(TestMultiTreeHandle));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_PARSE_ERROR, result);
}

/* Tests_SRS_JSON_DECODER_99_043:[ A number contains an integer component that may be prefixed with an optional minus sign, which may be followed by a fraction part and/or an exponent part.] */
/* Tests_SRS_JSON_DECODER_99_047:[ An exponent part begins with the letter E in upper or lowercase, which may be followed by a plus or minus sign.] */
/* Tests_SRS_JSON_DECODER_99_048:[ The E and optional sign are followed by one or more digits.] */
TEST_FUNCTION(JSONDecoder_Integer_With_e_Sign_And_With_No_Digits_Fails)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "[1e-]";

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "0", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));
    EXPECTED_CALL(mocks, MultiTree_Destroy(TestMultiTreeHandle));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_PARSE_ERROR, result);
}

/* Tests_SRS_JSON_DECODER_99_043:[ A number contains an integer component that may be prefixed with an optional minus sign, which may be followed by a fraction part and/or an exponent part.] */
/* Tests_SRS_JSON_DECODER_99_047:[ An exponent part begins with the letter E in upper or lowercase, which may be followed by a plus or minus sign.] */
/* Tests_SRS_JSON_DECODER_99_048:[ The E and optional sign are followed by one or more digits.] */
TEST_FUNCTION(JSONDecoder_Integer_With_E_Sign_And_With_No_Digits_Fails)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "[1E-]";

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "0", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));
    EXPECTED_CALL(mocks, MultiTree_Destroy(TestMultiTreeHandle));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_PARSE_ERROR, result);
}

/* Tests_SRS_JSON_DECODER_99_045:[ Leading zeros are not allowed.] */
TEST_FUNCTION(JSONDecoder_Integer_With_A_Leading_Zero_Fails)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "[01]";

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "0", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));
    EXPECTED_CALL(mocks, MultiTree_Destroy(TestMultiTreeHandle));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_PARSE_ERROR, result);
}

/* Tests_SRS_JSON_DECODER_99_045:[ Leading zeros are not allowed.] */
TEST_FUNCTION(JSONDecoder_Integer_With_2_Leading_Zero_Fails)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "[001]";

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "0", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));
    EXPECTED_CALL(mocks, MultiTree_Destroy(TestMultiTreeHandle));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_PARSE_ERROR, result);
}

/* Tests_SRS_JSON_DECODER_99_045:[ Leading zeros are not allowed.] */
TEST_FUNCTION(JSONDecoder_Integer_With_The_Value_0_Succeeds)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "[0]";
    void* value1Ptr = &json[1];

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "0", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));
    STRICT_EXPECTED_CALL(mocks, MultiTree_SetValue(TestChildHandle1, value1Ptr));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_OK, result);
}

/* Tests_SRS_JSON_DECODER_99_045:[ Leading zeros are not allowed.] */
TEST_FUNCTION(JSONDecoder_Integer_With_The_Value_101_Succeeds)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "[101]";
    void* value1Ptr = &json[1];

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "0", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));
    STRICT_EXPECTED_CALL(mocks, MultiTree_SetValue(TestChildHandle1, value1Ptr));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_OK, result);
}

/* Tests_SRS_JSON_DECODER_99_044:[ Octal and hex forms are not allowed.] */
TEST_FUNCTION(JSONDecoder_Integer_With_A_Hex_Number_Fails)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "[FF]";

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "0", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));
    EXPECTED_CALL(mocks, MultiTree_Destroy(TestMultiTreeHandle));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_PARSE_ERROR, result);
}

/* Tests_SRS_JSON_DECODER_99_018:[ A JSON value MUST be an object, array, number, or string, or one of the following three literal names: false null true] */
TEST_FUNCTION(JSONDecoder_Decoding_falseahbjkfsdhjkfhks_Fails)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "[falseahbjkfsdhjkfhks]";

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "0", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));
    EXPECTED_CALL(mocks, MultiTree_SetValue(TestChildHandle1, IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, MultiTree_Destroy(TestMultiTreeHandle));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_PARSE_ERROR, result);
}

/* Tests_SRS_JSON_DECODER_99_018:[ A JSON value MUST be an object, array, number, or string, or one of the following three literal names: false null true] */
TEST_FUNCTION(JSONDecoder_Decoding_falsetrue_Fails)
{
    ///arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    char json[] = "[falsetrue]";

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "0", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));
    EXPECTED_CALL(mocks, MultiTree_SetValue(TestChildHandle1, IGNORED_PTR_ARG));
    EXPECTED_CALL(mocks, MultiTree_Destroy(TestMultiTreeHandle));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_PARSE_ERROR, result);
}

void TestSpecialCharacter_Success(char* json)
{
    // arrange
    CJSONDecoderMocks mocks;
    MULTITREE_HANDLE multiTree;
    void* value1Ptr = &json[1];

    EXPECTED_CALL(mocks, MultiTree_Create(IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    STRICT_EXPECTED_CALL(mocks, MultiTree_AddChild(TestMultiTreeHandle, "0", IGNORED_PTR_ARG))
        .CopyOutArgumentBuffer(3, &TestChildHandle1, sizeof(TestChildHandle1));
    STRICT_EXPECTED_CALL(mocks, MultiTree_SetValue(TestChildHandle1, value1Ptr));

    ///act
    JSON_DECODER_RESULT result = JSONDecoder_JSON_To_MultiTree(json, &multiTree);

    ///assert
    ASSERT_ARE_EQUAL(JSON_DECODER_RESULT_TAG, JSON_DECODER_OK, result);
}

/* Tests_SRS_JSON_DECODER_99_033:[ Alternatively, there are two-character sequence escape  representations of some popular characters.  So, for example, a string containing only a single reverse solidus character may be represented more compactly as "\\".] */
/* Tests_SRS_JSON_DECODER_99_051:[ %x5C /          ; \    reverse solidus U+005C] */
TEST_FUNCTION(JSONDecoder_When_String_With_A_BackSlash_Value_Is_In_JSON_Decoding_Succeeds)
{
    char json[] = "[\"\\\\\"]";
    TestSpecialCharacter_Success(json);
}

/* Tests_SRS_JSON_DECODER_99_033:[ Alternatively, there are two-character sequence escape  representations of some popular characters.  So, for example, a string containing only a single reverse solidus character may be represented more compactly as "\\".] */
/* Tests_SRS_JSON_DECODER_99_050:[ %x22 /          ; "    quotation mark  U+0022] */
TEST_FUNCTION(JSONDecoder_When_String_With_A_Quote_Value_Is_In_JSON_Decoding_Succeeds)
{
    char json[] = "[\"\\\"\"]";
    TestSpecialCharacter_Success(json);
}

/* Tests_SRS_JSON_DECODER_99_033:[ Alternatively, there are two-character sequence escape  representations of some popular characters.  So, for example, a string containing only a single reverse solidus character may be represented more compactly as "\\".] */
/* Tests_SRS_JSON_DECODER_99_052:[ %x2F /          ; /    solidus         U+002F] */
TEST_FUNCTION(JSONDecoder_When_String_With_A_Slash_Value_Is_In_JSON_Decoding_Succeeds)
{
    char json[] = "[\"\\/\"]";
    TestSpecialCharacter_Success(json);
}

/* Tests_SRS_JSON_DECODER_99_033:[ Alternatively, there are two-character sequence escape  representations of some popular characters.  So, for example, a string containing only a single reverse solidus character may be represented more compactly as "\\".] */
/* Tests_SRS_JSON_DECODER_99_053:[ %x62 /          ; b    backspace       U+0008] */
TEST_FUNCTION(JSONDecoder_When_String_With_A_Backspace_Value_Is_In_JSON_Decoding_Succeeds)
{
    char json[] = "[\"\\b\"]";
    TestSpecialCharacter_Success(json);
}

/* Tests_SRS_JSON_DECODER_99_033:[ Alternatively, there are two-character sequence escape  representations of some popular characters.  So, for example, a string containing only a single reverse solidus character may be represented more compactly as "\\".] */
/* Tests_SRS_JSON_DECODER_99_054:[ %x66 /          ; f    form feed       U+000C] */
TEST_FUNCTION(JSONDecoder_When_String_With_A_FormFeed_Value_Is_In_JSON_Decoding_Succeeds)
{
    char json[] = "[\"\\f\"]";
    TestSpecialCharacter_Success(json);
}

/* Tests_SRS_JSON_DECODER_99_033:[ Alternatively, there are two-character sequence escape  representations of some popular characters.  So, for example, a string containing only a single reverse solidus character may be represented more compactly as "\\".] */
/* Tests_SRS_JSON_DECODER_99_055:[ %x6E /          ; n    line feed       U+000A] */
TEST_FUNCTION(JSONDecoder_When_String_With_A_LineFeed_Value_Is_In_JSON_Decoding_Succeeds)
{
    char json[] = "[\"\\n\"]";
    TestSpecialCharacter_Success(json);
}

/* Tests_SRS_JSON_DECODER_99_033:[ Alternatively, there are two-character sequence escape  representations of some popular characters.  So, for example, a string containing only a single reverse solidus character may be represented more compactly as "\\".] */
/* Tests_SRS_JSON_DECODER_99_056:[ %x72 /          ; r    carriage return U+000D] */
TEST_FUNCTION(JSONDecoder_When_String_With_A_CR_Value_Is_In_JSON_Decoding_Succeeds)
{
    char json[] = "[\"\\r\"]";
    TestSpecialCharacter_Success(json);
}

/* Tests_SRS_JSON_DECODER_99_033:[ Alternatively, there are two-character sequence escape  representations of some popular characters.  So, for example, a string containing only a single reverse solidus character may be represented more compactly as "\\".] */
/* Tests_SRS_JSON_DECODER_99_057:[ %x74 /          ; t    tab             U+0009] */
TEST_FUNCTION(JSONDecoder_When_String_With_A_Tab_Value_Is_In_JSON_Decoding_Succeeds)
{
    char json[] = "[\"\\t\"]";
    TestSpecialCharacter_Success(json);
}

/* Tests_SRS_JSON_DECODER_99_030:[ Any character may be escaped.]  */
TEST_FUNCTION(JSONDecoder_When_String_Has_Several_Escaped_Chars_Decoding_Succeeds)
{
    char json[] = "[\"Test\\t\\r\\nEndOfValue\"]";
    TestSpecialCharacter_Success(json);
}

END_TEST_SUITE(JSONDecoder_ut)
