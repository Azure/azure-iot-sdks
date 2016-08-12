// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <cstdlib>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "testrunnerswitcher.h"
#include "micromock.h"
#include "micromockcharstararenullterminatedstrings.h"

#include "connection_string_parser.h"

#include "connection_string_parser.h"
#include "azure_c_shared_utility/strings.h"
#include "azure_c_shared_utility/map.h"
#include "azure_c_shared_utility/string_tokenizer.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/lock.h"

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
    #include "vector.c"
    #include "strings.c"
    #undef Lock
    #undef Unlock
    #undef Lock_Init
    #undef Lock_Deinit
};

static const char TEST_CONST_CHAR[] = "a";
static STRING_HANDLE DUMMY_STRING_HANDLE = (STRING_HANDLE)0x4242;
static MAP_HANDLE TEST_MAP_HANDLE = (MAP_HANDLE)0x4243;
static STRING_HANDLE TEST_CONNECTION_STRING = (STRING_HANDLE)0x4244;
static STRING_TOKENIZER_HANDLE TEST_STRING_TOKENIZER_HANDLE = (STRING_TOKENIZER_HANDLE)0x4245;
static STRING_HANDLE TEST_KEY_STRING_HANDLE = (STRING_HANDLE)0x4246;
static STRING_HANDLE TEST_VALUE_STRING_HANDLE = (STRING_HANDLE)0x4247;

static MICROMOCK_MUTEX_HANDLE g_testByTest;

typedef struct LOCK_TEST_STRUCT_TAG
{
    char* dummy;
} LOCK_TEST_STRUCT;

// ** Mocks **
TYPED_MOCK_CLASS(CConnectionStringParserMocks, CGlobalMock)
{
public:
    /* Map mocks */
    MOCK_STATIC_METHOD_1(, MAP_HANDLE, Map_Create, MAP_FILTER_CALLBACK, mapFilterFunc)
    MOCK_METHOD_END(MAP_HANDLE, TEST_MAP_HANDLE)
    MOCK_STATIC_METHOD_1(, void, Map_Destroy, MAP_HANDLE, handle)
    MOCK_VOID_METHOD_END()
    MOCK_STATIC_METHOD_3(, MAP_RESULT, Map_Add, MAP_HANDLE, handle, const char*, key, const char*, value)
    MOCK_METHOD_END(MAP_RESULT, MAP_OK)

    /* STRING mocks */
    MOCK_STATIC_METHOD_1(, void, STRING_delete, STRING_HANDLE, handle)
    MOCK_VOID_METHOD_END()
    MOCK_STATIC_METHOD_1(, const char*, STRING_c_str, STRING_HANDLE, s)
    MOCK_METHOD_END(const char*, TEST_CONST_CHAR)
    MOCK_STATIC_METHOD_0(, STRING_HANDLE, STRING_new)
    MOCK_METHOD_END(STRING_HANDLE, DUMMY_STRING_HANDLE);

    /* STRING_Tokenizer mocks */
    MOCK_STATIC_METHOD_1(, STRING_TOKENIZER_HANDLE, STRING_TOKENIZER_create, STRING_HANDLE, handle);
    MOCK_METHOD_END(STRING_TOKENIZER_HANDLE, TEST_STRING_TOKENIZER_HANDLE)
    MOCK_STATIC_METHOD_3(, int, STRING_TOKENIZER_get_next_token, STRING_TOKENIZER_HANDLE, t, STRING_HANDLE, output, const char*, delimiters);
	MOCK_METHOD_END(int, 0);
    MOCK_STATIC_METHOD_1(, void, STRING_TOKENIZER_destroy, STRING_TOKENIZER_HANDLE, t);
    MOCK_VOID_METHOD_END();
};

DECLARE_GLOBAL_MOCK_METHOD_1(CConnectionStringParserMocks, , MAP_HANDLE, Map_Create, MAP_FILTER_CALLBACK, mapFilterFunc);
DECLARE_GLOBAL_MOCK_METHOD_1(CConnectionStringParserMocks, , void, Map_Destroy, MAP_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_3(CConnectionStringParserMocks, , MAP_RESULT, Map_Add, MAP_HANDLE, handle, const char*, key, const char*, value);

DECLARE_GLOBAL_MOCK_METHOD_1(CConnectionStringParserMocks, , void, STRING_delete, STRING_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_1(CConnectionStringParserMocks, , const char*, STRING_c_str, STRING_HANDLE, s);
DECLARE_GLOBAL_MOCK_METHOD_0(CConnectionStringParserMocks, , STRING_HANDLE, STRING_new);

DECLARE_GLOBAL_MOCK_METHOD_1(CConnectionStringParserMocks, , STRING_TOKENIZER_HANDLE, STRING_TOKENIZER_create, STRING_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_3(CConnectionStringParserMocks, , int, STRING_TOKENIZER_get_next_token, STRING_TOKENIZER_HANDLE, t, STRING_HANDLE, output, const char*, delimiters);
DECLARE_GLOBAL_MOCK_METHOD_1(CConnectionStringParserMocks, , void, STRING_TOKENIZER_destroy, STRING_TOKENIZER_HANDLE, t);

static MICROMOCK_GLOBAL_SEMAPHORE_HANDLE g_dllByDll;

BEGIN_TEST_SUITE(connectionstringparser_ut)

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

/* connectionstringparser_parse */

/* Tests_SRS_CONNECTIONSTRINGPARSER_01_001: [connectionstringparser_parse shall parse all key value pairs from the connection_string passed in as argument and return a new map that holds the key/value pairs.]  */
/* Tests_SRS_CONNECTIONSTRINGPARSER_01_003: [connectionstringparser_parse shall create a STRING tokenizer to be used for parsing the connection string, by calling STRING_TOKENIZER_create.] */
/* Tests_SRS_CONNECTIONSTRINGPARSER_01_004: [connectionstringparser_parse shall start scanning at the beginning of the connection string.] */
/* Tests_SRS_CONNECTIONSTRINGPARSER_01_016: [2 STRINGs shall be allocated in order to hold the to be parsed key and value tokens.] */
/* Tests_SRS_CONNECTIONSTRINGPARSER_01_005: [The following actions shall be repeated until parsing is complete:] */
/* Tests_SRS_CONNECTIONSTRINGPARSER_01_006: [connectionstringparser_parse shall find a token (the key of the key/value pair) delimited by the "=" character, by calling STRING_TOKENIZER_get_next_token.] */
/* Tests_SRS_CONNECTIONSTRINGPARSER_01_007: [If STRING_TOKENIZER_get_next_token fails, parsing shall be considered complete.] */
/* Tests_SRS_CONNECTIONSTRINGPARSER_01_014: [After the parsing is complete the previously allocated STRINGs and STRING tokenizer shall be freed by calling STRING_TOKENIZER_destroy.] */
TEST_FUNCTION(connectionstringparser_parse_with_an_empty_string_yields_an_empty_map)
{
    // arrange
    CConnectionStringParserMocks mocks;

    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_create(TEST_CONNECTION_STRING));
    STRICT_EXPECTED_CALL(mocks, STRING_new())
        .SetReturn(TEST_KEY_STRING_HANDLE);
    STRICT_EXPECTED_CALL(mocks, STRING_new())
        .SetReturn(TEST_VALUE_STRING_HANDLE);
    STRICT_EXPECTED_CALL(mocks, Map_Create(NULL));
    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_KEY_STRING_HANDLE, "="))
        .SetReturn(1);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_KEY_STRING_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_VALUE_STRING_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_destroy(TEST_STRING_TOKENIZER_HANDLE));

    // act
    MAP_HANDLE result = connectionstringparser_parse(TEST_CONNECTION_STRING);

    // assert
    ASSERT_ARE_EQUAL(void_ptr, TEST_MAP_HANDLE, result);
}

/* Tests_SRS_CONNECTIONSTRINGPARSER_01_002: [If connection_string is NULL then connectionstringparser_parse shall fail and return NULL.] */
TEST_FUNCTION(connectionstringparser_parse_with_NULL_connection_string_fails)
{
    // arrange
    CConnectionStringParserMocks mocks;

    // act
    MAP_HANDLE result = connectionstringparser_parse(NULL);

    // assert
    ASSERT_ARE_EQUAL(void_ptr, NULL, result);
}

/* Tests_SRS_CONNECTIONSTRINGPARSER_01_015: [If STRING_TOKENIZER_create fails, connectionstringparser_parse shall fail and return NULL.] */
TEST_FUNCTION(when_creating_the_string_tokenizer_fails_then_connectionstringparser_fails)
{
    // arrange
    CConnectionStringParserMocks mocks;

    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_create(TEST_CONNECTION_STRING))
        .SetReturn((STRING_TOKENIZER_HANDLE)NULL);

    // act
    MAP_HANDLE result = connectionstringparser_parse(TEST_CONNECTION_STRING);

    // assert
    ASSERT_ARE_EQUAL(void_ptr, NULL, result);
}

/* Tests_SRS_CONNECTIONSTRINGPARSER_01_017: [If allocating the STRINGs fails connectionstringparser_parse shall fail and return NULL.] */
TEST_FUNCTION(when_allocating_the_key_token_string_fails_then_connectionstringparser_fails)
{
    // arrange
    CConnectionStringParserMocks mocks;

    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_create(TEST_CONNECTION_STRING));
    STRICT_EXPECTED_CALL(mocks, STRING_new())
        .SetReturn((STRING_HANDLE)NULL);
    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_destroy(TEST_STRING_TOKENIZER_HANDLE));

    // act
    MAP_HANDLE result = connectionstringparser_parse(TEST_CONNECTION_STRING);

    // assert
    ASSERT_ARE_EQUAL(void_ptr, NULL, result);
}

/* Tests_SRS_CONNECTIONSTRINGPARSER_01_017: [If allocating the STRINGs fails connectionstringparser_parse shall fail and return NULL.] */
TEST_FUNCTION(when_allocating_the_value_token_string_fails_then_connectionstringparser_fails)
{
    // arrange
    CConnectionStringParserMocks mocks;

    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_create(TEST_CONNECTION_STRING));
    STRICT_EXPECTED_CALL(mocks, STRING_new())
        .SetReturn(TEST_KEY_STRING_HANDLE);
    STRICT_EXPECTED_CALL(mocks, STRING_new())
        .SetReturn((STRING_HANDLE)NULL);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_KEY_STRING_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_destroy(TEST_STRING_TOKENIZER_HANDLE));

    // act
    MAP_HANDLE result = connectionstringparser_parse(TEST_CONNECTION_STRING);

    // assert
    ASSERT_ARE_EQUAL(void_ptr, NULL, result);
}

/* Tests_SRS_CONNECTIONSTRINGPARSER_01_018: [If creating the result map fails, then connectionstringparser_parse shall return NULL.] */
TEST_FUNCTION(when_allocating_the_result_map_fails_then_connectionstringparser_fails)
{
    // arrange
    CConnectionStringParserMocks mocks;

    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_create(TEST_CONNECTION_STRING));
    STRICT_EXPECTED_CALL(mocks, STRING_new())
        .SetReturn(TEST_KEY_STRING_HANDLE);
    STRICT_EXPECTED_CALL(mocks, STRING_new())
        .SetReturn(TEST_VALUE_STRING_HANDLE);
    STRICT_EXPECTED_CALL(mocks, Map_Create(NULL))
        .SetReturn((MAP_HANDLE)NULL);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_KEY_STRING_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_VALUE_STRING_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_destroy(TEST_STRING_TOKENIZER_HANDLE));

    // act
    MAP_HANDLE result = connectionstringparser_parse(TEST_CONNECTION_STRING);

    // assert
    ASSERT_ARE_EQUAL(void_ptr, NULL, result);
}

/* Tests_SRS_CONNECTIONSTRINGPARSER_01_001: [connectionstringparser_parse shall parse all key value pairs from the connection_string passed in as argument and return a new map that holds the key/value pairs.]  */
/* Tests_SRS_CONNECTIONSTRINGPARSER_01_003: [connectionstringparser_parse shall create a STRING tokenizer to be used for parsing the connection string, by calling STRING_TOKENIZER_create.] */
/* Tests_SRS_CONNECTIONSTRINGPARSER_01_004: [connectionstringparser_parse shall start scanning at the beginning of the connection string.] */
/* Tests_SRS_CONNECTIONSTRINGPARSER_01_016: [2 STRINGs shall be allocated in order to hold the to be parsed key and value tokens.] */
/* Tests_SRS_CONNECTIONSTRINGPARSER_01_005: [The following actions shall be repeated until parsing is complete:] */
/* Tests_SRS_CONNECTIONSTRINGPARSER_01_006: [connectionstringparser_parse shall find a token (the key of the key/value pair) delimited by the "=" character, by calling STRING_TOKENIZER_get_next_token.] */
/* Tests_SRS_CONNECTIONSTRINGPARSER_01_007: [If STRING_TOKENIZER_get_next_token fails, parsing shall be considered complete.] */
/* Tests_SRS_CONNECTIONSTRINGPARSER_01_014: [After the parsing is complete the previously allocated STRINGs and STRING tokenizer shall be freed by calling STRING_TOKENIZER_destroy.] */
/* Tests_SRS_CONNECTIONSTRINGPARSER_01_008: [connectionstringparser_parse shall find a token (the value of the key/value pair) delimited by the ";" character, by calling STRING_TOKENIZER_get_next_token.] */
/* Tests_SRS_CONNECTIONSTRINGPARSER_01_010: [The key and value shall be added to the result map by using Map_Add.] */
/* Tests_SRS_CONNECTIONSTRINGPARSER_01_011: [The C strings for the key and value shall be extracted from the previously parsed STRINGs by using STRING_c_str.] */
TEST_FUNCTION(connectionstringparser_parse_with_a_key_value_pair_adds_it_to_the_result_map)
{
    // arrange
    CConnectionStringParserMocks mocks;

    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_create(TEST_CONNECTION_STRING));
    STRICT_EXPECTED_CALL(mocks, STRING_new())
        .SetReturn(TEST_KEY_STRING_HANDLE);
    STRICT_EXPECTED_CALL(mocks, STRING_new())
        .SetReturn(TEST_VALUE_STRING_HANDLE);
    STRICT_EXPECTED_CALL(mocks, Map_Create(NULL));
    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_KEY_STRING_HANDLE, "="));
    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_VALUE_STRING_HANDLE, ";"));
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(TEST_KEY_STRING_HANDLE))
        .SetReturn("key1");
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(TEST_VALUE_STRING_HANDLE))
        .SetReturn("value1");
    STRICT_EXPECTED_CALL(mocks, Map_Add(TEST_MAP_HANDLE, "key1", "value1"));
    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_KEY_STRING_HANDLE, "="))
        .SetReturn(1);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_KEY_STRING_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_VALUE_STRING_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_destroy(TEST_STRING_TOKENIZER_HANDLE));

    // act
    MAP_HANDLE result = connectionstringparser_parse(TEST_CONNECTION_STRING);

    // assert
    ASSERT_ARE_EQUAL(void_ptr, TEST_MAP_HANDLE, result);
}

/* Tests_SRS_CONNECTIONSTRINGPARSER_01_009: [If STRING_TOKENIZER_get_next_token fails, connectionstringparser_parse shall fail and return NULL (freeing the allocated result map).] */
TEST_FUNCTION(when_getting_the_value_token_fails_then_connectionstringparser_parse_fails)
{
    // arrange
    CConnectionStringParserMocks mocks;

    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_create(TEST_CONNECTION_STRING));
    STRICT_EXPECTED_CALL(mocks, STRING_new())
        .SetReturn(TEST_KEY_STRING_HANDLE);
    STRICT_EXPECTED_CALL(mocks, STRING_new())
        .SetReturn(TEST_VALUE_STRING_HANDLE);
    STRICT_EXPECTED_CALL(mocks, Map_Create(NULL));
    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_KEY_STRING_HANDLE, "="));
    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_VALUE_STRING_HANDLE, ";"))
        .SetReturn(1);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_KEY_STRING_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_VALUE_STRING_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_destroy(TEST_STRING_TOKENIZER_HANDLE));
    STRICT_EXPECTED_CALL(mocks, Map_Destroy(TEST_MAP_HANDLE));

    // act
    MAP_HANDLE result = connectionstringparser_parse(TEST_CONNECTION_STRING);

    // assert
    ASSERT_ARE_EQUAL(void_ptr, NULL, result);
}

/* Tests_SRS_CONNECTIONSTRINGPARSER_01_019: [If the key length is zero then connectionstringparser_parse shall fail and return NULL (freeing the allocated result map).] */
TEST_FUNCTION(when_the_key_is_zero_length_then_connectionstringparser_parse_fails)
{
    // arrange
    CConnectionStringParserMocks mocks;

    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_create(TEST_CONNECTION_STRING));
    STRICT_EXPECTED_CALL(mocks, STRING_new())
        .SetReturn(TEST_KEY_STRING_HANDLE);
    STRICT_EXPECTED_CALL(mocks, STRING_new())
        .SetReturn(TEST_VALUE_STRING_HANDLE);
    STRICT_EXPECTED_CALL(mocks, Map_Create(NULL));
    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_KEY_STRING_HANDLE, "="));
    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_VALUE_STRING_HANDLE, ";"));
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(TEST_KEY_STRING_HANDLE))
        .SetReturn("");
    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_KEY_STRING_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_VALUE_STRING_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_destroy(TEST_STRING_TOKENIZER_HANDLE));
    STRICT_EXPECTED_CALL(mocks, Map_Destroy(TEST_MAP_HANDLE));

    // act
    MAP_HANDLE result = connectionstringparser_parse(TEST_CONNECTION_STRING);

    // assert
    ASSERT_ARE_EQUAL(void_ptr, NULL, result);
}

/* Tests_SRS_CONNECTIONSTRINGPARSER_01_012: [If Map_Add fails connectionstringparser_parse shall fail and return NULL (freeing the allocated result map).] */
TEST_FUNCTION(when_adding_the_key_value_pair_to_the_map_fails_then_connectionstringparser_parse_fails)
{
    // arrange
    CConnectionStringParserMocks mocks;

    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_create(TEST_CONNECTION_STRING));
    STRICT_EXPECTED_CALL(mocks, STRING_new())
        .SetReturn(TEST_KEY_STRING_HANDLE);
    STRICT_EXPECTED_CALL(mocks, STRING_new())
        .SetReturn(TEST_VALUE_STRING_HANDLE);
    STRICT_EXPECTED_CALL(mocks, Map_Create(NULL));
    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_KEY_STRING_HANDLE, "="));
    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_VALUE_STRING_HANDLE, ";"));
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(TEST_KEY_STRING_HANDLE))
        .SetReturn("key1");
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(TEST_VALUE_STRING_HANDLE))
        .SetReturn("value1");
    STRICT_EXPECTED_CALL(mocks, Map_Add(TEST_MAP_HANDLE, "key1", "value1"))
        .SetReturn(MAP_ERROR);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_KEY_STRING_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_VALUE_STRING_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_destroy(TEST_STRING_TOKENIZER_HANDLE));
    STRICT_EXPECTED_CALL(mocks, Map_Destroy(TEST_MAP_HANDLE));

    // act
    MAP_HANDLE result = connectionstringparser_parse(TEST_CONNECTION_STRING);

    // assert
    ASSERT_ARE_EQUAL(void_ptr, NULL, result);
}

/* Tests_SRS_CONNECTIONSTRINGPARSER_01_013: [If STRING_c_str fails then connectionstringparser_parse shall fail and return NULL (freeing the allocated result map).] */
TEST_FUNCTION(when_getting_the_C_string_for_the_key_fails_then_connectionstringparser_parse_fails)
{
    // arrange
    CConnectionStringParserMocks mocks;

    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_create(TEST_CONNECTION_STRING));
    STRICT_EXPECTED_CALL(mocks, STRING_new())
        .SetReturn(TEST_KEY_STRING_HANDLE);
    STRICT_EXPECTED_CALL(mocks, STRING_new())
        .SetReturn(TEST_VALUE_STRING_HANDLE);
    STRICT_EXPECTED_CALL(mocks, Map_Create(NULL));
    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_KEY_STRING_HANDLE, "="));
    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_VALUE_STRING_HANDLE, ";"));
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(TEST_KEY_STRING_HANDLE))
        .SetReturn((const char*)NULL);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_KEY_STRING_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_VALUE_STRING_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_destroy(TEST_STRING_TOKENIZER_HANDLE));
    STRICT_EXPECTED_CALL(mocks, Map_Destroy(TEST_MAP_HANDLE));

    // act
    MAP_HANDLE result = connectionstringparser_parse(TEST_CONNECTION_STRING);

    // assert
    ASSERT_ARE_EQUAL(void_ptr, NULL, result);
}

/* Tests_SRS_CONNECTIONSTRINGPARSER_01_013: [If STRING_c_str fails then connectionstringparser_parse shall fail and return NULL (freeing the allocated result map).] */
TEST_FUNCTION(when_getting_the_C_string_for_the_value_fails_then_connectionstringparser_parse_fails)
{
    // arrange
    CConnectionStringParserMocks mocks;

    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_create(TEST_CONNECTION_STRING));
    STRICT_EXPECTED_CALL(mocks, STRING_new())
        .SetReturn(TEST_KEY_STRING_HANDLE);
    STRICT_EXPECTED_CALL(mocks, STRING_new())
        .SetReturn(TEST_VALUE_STRING_HANDLE);
    STRICT_EXPECTED_CALL(mocks, Map_Create(NULL));
    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_KEY_STRING_HANDLE, "="));
    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_VALUE_STRING_HANDLE, ";"));
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(TEST_KEY_STRING_HANDLE))
        .SetReturn("key1");
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(TEST_VALUE_STRING_HANDLE))
        .SetReturn((const char*)NULL);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_KEY_STRING_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_VALUE_STRING_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_destroy(TEST_STRING_TOKENIZER_HANDLE));
    STRICT_EXPECTED_CALL(mocks, Map_Destroy(TEST_MAP_HANDLE));

    // act
    MAP_HANDLE result = connectionstringparser_parse(TEST_CONNECTION_STRING);

    // assert
    ASSERT_ARE_EQUAL(void_ptr, NULL, result);
}

/* Tests_SRS_CONNECTIONSTRINGPARSER_01_001: [connectionstringparser_parse shall parse all key value pairs from the connection_string passed in as argument and return a new map that holds the key/value pairs.]  */
/* Tests_SRS_CONNECTIONSTRINGPARSER_01_003: [connectionstringparser_parse shall create a STRING tokenizer to be used for parsing the connection string, by calling STRING_TOKENIZER_create.] */
/* Tests_SRS_CONNECTIONSTRINGPARSER_01_004: [connectionstringparser_parse shall start scanning at the beginning of the connection string.] */
/* Tests_SRS_CONNECTIONSTRINGPARSER_01_016: [2 STRINGs shall be allocated in order to hold the to be parsed key and value tokens.] */
/* Tests_SRS_CONNECTIONSTRINGPARSER_01_005: [The following actions shall be repeated until parsing is complete:] */
/* Tests_SRS_CONNECTIONSTRINGPARSER_01_006: [connectionstringparser_parse shall find a token (the key of the key/value pair) delimited by the "=" character, by calling STRING_TOKENIZER_get_next_token.] */
/* Tests_SRS_CONNECTIONSTRINGPARSER_01_007: [If STRING_TOKENIZER_get_next_token fails, parsing shall be considered complete.] */
/* Tests_SRS_CONNECTIONSTRINGPARSER_01_014: [After the parsing is complete the previously allocated STRINGs and STRING tokenizer shall be freed by calling STRING_TOKENIZER_destroy.] */
/* Tests_SRS_CONNECTIONSTRINGPARSER_01_008: [connectionstringparser_parse shall find a token (the value of the key/value pair) delimited by the ";" character, by calling STRING_TOKENIZER_get_next_token.] */
/* Tests_SRS_CONNECTIONSTRINGPARSER_01_010: [The key and value shall be added to the result map by using Map_Add.] */
/* Tests_SRS_CONNECTIONSTRINGPARSER_01_011: [The C strings for the key and value shall be extracted from the previously parsed STRINGs by using STRING_c_str.] */
TEST_FUNCTION(connectionstringparser_parse_with_2_key_value_pairs_adds_them_to_the_result_map)
{
    // arrange
    CConnectionStringParserMocks mocks;

    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_create(TEST_CONNECTION_STRING));
    STRICT_EXPECTED_CALL(mocks, STRING_new())
        .SetReturn(TEST_KEY_STRING_HANDLE);
    STRICT_EXPECTED_CALL(mocks, STRING_new())
        .SetReturn(TEST_VALUE_STRING_HANDLE);
    STRICT_EXPECTED_CALL(mocks, Map_Create(NULL));

    // 1st kvp
    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_KEY_STRING_HANDLE, "="));
    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_VALUE_STRING_HANDLE, ";"));
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(TEST_KEY_STRING_HANDLE))
        .SetReturn("key1");
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(TEST_VALUE_STRING_HANDLE))
        .SetReturn("value1");
    STRICT_EXPECTED_CALL(mocks, Map_Add(TEST_MAP_HANDLE, "key1", "value1"));

    // 2nd kvp
    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_KEY_STRING_HANDLE, "="));
    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_VALUE_STRING_HANDLE, ";"));
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(TEST_KEY_STRING_HANDLE))
        .SetReturn("key2");
    STRICT_EXPECTED_CALL(mocks, STRING_c_str(TEST_VALUE_STRING_HANDLE))
        .SetReturn("value2");
    STRICT_EXPECTED_CALL(mocks, Map_Add(TEST_MAP_HANDLE, "key2", "value2"));

    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_get_next_token(TEST_STRING_TOKENIZER_HANDLE, TEST_KEY_STRING_HANDLE, "="))
        .SetReturn(1);
    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_KEY_STRING_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_delete(TEST_VALUE_STRING_HANDLE));
    STRICT_EXPECTED_CALL(mocks, STRING_TOKENIZER_destroy(TEST_STRING_TOKENIZER_HANDLE));

    // act
    MAP_HANDLE result = connectionstringparser_parse(TEST_CONNECTION_STRING);

    // assert
    ASSERT_ARE_EQUAL(void_ptr, TEST_MAP_HANDLE, result);
}

END_TEST_SUITE(connectionstringparser_ut)
