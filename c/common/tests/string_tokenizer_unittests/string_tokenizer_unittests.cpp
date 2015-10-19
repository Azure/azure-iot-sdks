// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

//
// PUT NO INCLUDES BEFORE HERE !!!!
//
#include <cstdlib>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include <stddef.h>


//
// PUT NO CLIENT LIBRARY INCLUDES BEFORE HERE !!!!
//
#include "testrunnerswitcher.h"
#include "string_tokenizer.h"
#include "strings.h"
#include "micromock.h"
#include "lock.h"



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

#ifdef _MSC_VER
#pragma warning(disable:4505)
#endif

static MICROMOCK_GLOBAL_SEMAPHORE_HANDLE g_dllByDll;
static MICROMOCK_MUTEX_HANDLE g_testByTest;

#define TEST_STRING_HANDLE (STRING_HANDLE)0x42
#define FAKE_LOCK_HANDLE (LOCK_HANDLE)0x4f

static size_t currentmalloc_call;
static size_t whenShallmalloc_fail;


// ** Mocks **
TYPED_MOCK_CLASS(CStringTokenizerMocks, CGlobalMock)
{
public:
        /*Memory allocation*/
        MOCK_STATIC_METHOD_1(, void*, gballoc_malloc, size_t, size)
        void* result2;
        currentmalloc_call++;
        if (whenShallmalloc_fail>0)
        {
            if (currentmalloc_call == whenShallmalloc_fail)
            {
                result2 = NULL;
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
            MOCK_METHOD_END(void*, BASEIMPLEMENTATION::gballoc_realloc(ptr, size));

        MOCK_STATIC_METHOD_1(, void, gballoc_free, void*, ptr)
            BASEIMPLEMENTATION::gballoc_free(ptr);
        MOCK_VOID_METHOD_END();

        MOCK_STATIC_METHOD_0(, LOCK_HANDLE, Lock_Init)
        MOCK_METHOD_END(LOCK_HANDLE, FAKE_LOCK_HANDLE);
        MOCK_STATIC_METHOD_1(, LOCK_RESULT, Lock, LOCK_HANDLE, handle)
            MOCK_METHOD_END(LOCK_RESULT, LOCK_OK);
        MOCK_STATIC_METHOD_1(, LOCK_RESULT, Unlock, LOCK_HANDLE, handle)
            MOCK_METHOD_END(LOCK_RESULT, LOCK_OK);
        MOCK_STATIC_METHOD_1(, LOCK_RESULT, Lock_Deinit, LOCK_HANDLE, handle)
            MOCK_METHOD_END(LOCK_RESULT, LOCK_OK);
};

DECLARE_GLOBAL_MOCK_METHOD_1(CStringTokenizerMocks, , void*, gballoc_malloc, size_t, size);
DECLARE_GLOBAL_MOCK_METHOD_2(CStringTokenizerMocks, , void*, gballoc_realloc, void*, ptr, size_t, size);
DECLARE_GLOBAL_MOCK_METHOD_1(CStringTokenizerMocks, , void, gballoc_free, void*, ptr)
DECLARE_GLOBAL_MOCK_METHOD_0(CStringTokenizerMocks, , LOCK_HANDLE, Lock_Init);
DECLARE_GLOBAL_MOCK_METHOD_1(CStringTokenizerMocks, , LOCK_RESULT, Lock, LOCK_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_1(CStringTokenizerMocks, , LOCK_RESULT, Unlock, LOCK_HANDLE, handle);
DECLARE_GLOBAL_MOCK_METHOD_1(CStringTokenizerMocks, , LOCK_RESULT, Lock_Deinit, LOCK_HANDLE, handle)

BEGIN_TEST_SUITE(string_tokenizer_unittests)

    TEST_SUITE_CLEANUP(TestClassCleanup)
    {
        DEINITIALIZE_MEMORY_DEBUG(g_dllByDll);
    }

    TEST_SUITE_INITIALIZE(setsBufferTempSize)
    {
        INITIALIZE_MEMORY_DEBUG(g_dllByDll);
        g_testByTest = MicroMockCreateMutex();
        ASSERT_IS_NOT_NULL(g_testByTest);
    }

    TEST_FUNCTION_INITIALIZE(TestMethodInitialize)
    {
        if (!MicroMockAcquireMutex(g_testByTest))
        {
            ASSERT_FAIL("our mutex is ABANDONED. Failure in test framework");
        }

        currentmalloc_call = 0;
        whenShallmalloc_fail = 0;
    }

    TEST_FUNCTION_CLEANUP(TestMethodCleanup)
    {
        if (!MicroMockReleaseMutex(g_testByTest))
        {
            ASSERT_FAIL("failure in test framework at ReleaseMutex");
        }
    }

    /* STRING_TOKENIZER_Tests BEGIN */

    /* STRING_TOKENIZER_CREATE */
    /* Tests_SRS_STRING_04_001: [STRING_TOKENIZER_create shall return an NULL STRING_TOKENIZER_HANDLE if parameter handle is NULL]*/
    TEST_FUNCTION(STRING_TOKENIZER_create_with_null_handle_fail)
    {
        ///arrange

        ///act
        STRING_TOKENIZER_HANDLE t = STRING_TOKENIZER_create(NULL);

        ///assert
        ASSERT_IS_NULL(t);
    }

    /* Tests_SRS_STRING_04_002: [STRING_TOKENIZER_create shall allocate a new STRING_TOKENIZER _HANDLE having the content of the STRING_HANDLE copied and current position pointing at the beginning of the string] */
    TEST_FUNCTION(STRING_TOKENIZER_create_succeed)
    {
        ///arrange
        CStringTokenizerMocks stMocks;
        const char* inputString = "Pirlimpimpim";

        STRING_HANDLE input_string_handle = STRING_construct(inputString);

        stMocks.ResetAllCalls();
        ///act

        STRICT_EXPECTED_CALL(stMocks, gballoc_malloc(0))  //Token Allocation.
            .IgnoreArgument(1);

        STRICT_EXPECTED_CALL(stMocks, gballoc_malloc(0))  //Token Content Allocation.
            .IgnoreArgument(1);


        STRING_TOKENIZER_HANDLE t = STRING_TOKENIZER_create(input_string_handle);

        stMocks.AssertActualAndExpectedCalls();
        ///assert
        ASSERT_IS_NOT_NULL(t);

        //Cleanup
        STRING_TOKENIZER_destroy(t);
        STRING_delete(input_string_handle);
    }

    /* Test_SRS_STRING_04_003: [STRING_TOKENIZER_create shall return an NULL STRING_TOKENIZER _HANDLE on any error that is encountered] */
    TEST_FUNCTION(STRING_TOKENIZER_create_when_malloc_fails_then_string_tokenizer_create_fails)
    {
        ///arrange
        CStringTokenizerMocks stMocks;
        const char* inputString = "Pirlimpimpim";

        STRING_HANDLE input_string_handle = STRING_construct(inputString);

        stMocks.ResetAllCalls();
        ///act

        
        EXPECTED_CALL(stMocks, gballoc_malloc(0));  //Token Allocation.


        whenShallmalloc_fail = currentmalloc_call + 1;
        STRING_TOKENIZER_HANDLE t = STRING_TOKENIZER_create(input_string_handle);

        ///assert
        ASSERT_IS_NULL(t);
        stMocks.AssertActualAndExpectedCalls();

        //Cleanup
        STRING_delete(input_string_handle);
    }
    /* STRING_TOKENIZER_get_next_token */
    /* Tests_SRS_STRING_04_004: [STRING_TOKENIZER_get_next_token shall return a nonzero value if any of the 3 parameters is NULL] */
    TEST_FUNCTION(STRING_TOKENIZER_get_next_token_handle_NULL_Fail)
    {
        ///arrange
        CStringTokenizerMocks stMocks;

        STRING_HANDLE output_string_handle = STRING_new();

        stMocks.ResetAllCalls();

        ///act
        int r = STRING_TOKENIZER_get_next_token(NULL, output_string_handle, "m");

        ///assert
        ASSERT_ARE_NOT_EQUAL(int, r, 0);
        stMocks.AssertActualAndExpectedCalls();
        //Cleanup
        STRING_delete(output_string_handle);
    }

    /* Tests_SRS_STRING_04_004: [STRING_TOKENIZER_get_next_token shall return a nonzero value if any of the 3 parameters is NULL] */
    TEST_FUNCTION(STRING_TOKENIZER_get_next_token_output_NULL_Fail)
    {
        ///arrange
        CStringTokenizerMocks stMocks;
        const char* inputString = "Pirlimpimpim";

        STRING_HANDLE input_string_handle = STRING_construct(inputString);

        STRING_TOKENIZER_HANDLE t = STRING_TOKENIZER_create(input_string_handle);

        stMocks.ResetAllCalls();

        ///act
        int r = STRING_TOKENIZER_get_next_token(t, NULL, "m");

        ///assert
        ASSERT_ARE_NOT_EQUAL(int, r, 0);
        stMocks.AssertActualAndExpectedCalls();
        //Cleanup
        STRING_TOKENIZER_destroy(t);
        STRING_delete(input_string_handle);
    }

    /* Tests_SRS_STRING_04_004: [STRING_TOKENIZER_get_next_token shall return a nonzero value if any of the 3 parameters is NULL] */
    TEST_FUNCTION(STRING_TOKENIZER_get_next_token_delimiters_NULL_Fail)
    {
        ///arrange
        CStringTokenizerMocks stMocks;
        const char* inputString = "Pirlimpimpim";

        STRING_HANDLE input_string_handle = STRING_construct(inputString);
        STRING_HANDLE output_string_handle = STRING_new();

        STRING_TOKENIZER_HANDLE t = STRING_TOKENIZER_create(input_string_handle);

        stMocks.ResetAllCalls();

        ///act
        int r = STRING_TOKENIZER_get_next_token(t, output_string_handle, NULL);
        stMocks.AssertActualAndExpectedCalls();
        ///assert
        ASSERT_ARE_NOT_EQUAL(int, r, 0);
        //Cleanup
        STRING_TOKENIZER_destroy(t);
        STRING_delete(input_string_handle);
        STRING_delete(output_string_handle);
    }

    /* Tests_SRS_STRING_04_005: [STRING_TOKENIZER_get_next_token searches the string inside STRING_TOKENIZER_HANDLE for the first character that is NOT contained in the current delimiter] */
    TEST_FUNCTION(STRING_TOKENIZER_get_next_token_PIRLI_Succeed)
    {
        ///arrange
        CStringTokenizerMocks stMocks;
        const char* inputString = "Pirlimpimpim";

        STRING_HANDLE input_string_handle = STRING_construct(inputString);
        STRING_HANDLE output_string_handle = STRING_construct(inputString);

        STRING_TOKENIZER_HANDLE t = STRING_TOKENIZER_create(input_string_handle);

        stMocks.ResetAllCalls();

        EXPECTED_CALL(stMocks, gballoc_realloc(0, 0))  //Alloc memory to copy result. 
            .IgnoreArgument(1)
            .IgnoreArgument(2);

        ///act
        int r = STRING_TOKENIZER_get_next_token(t, output_string_handle, "m");

        ///assert
        ASSERT_ARE_EQUAL(char_ptr, "Pirli", STRING_c_str(output_string_handle));
        ASSERT_ARE_EQUAL(int, r, 0);
        stMocks.AssertActualAndExpectedCalls();
        //Cleanup
        STRING_TOKENIZER_destroy(t);
        STRING_delete(input_string_handle);
        STRING_delete(output_string_handle);
    }

    /* Tests_SRS_STRING_04_005: [STRING_TOKENIZER_get_next_token searches the string inside STRING_TOKENIZER_HANDLE for the first character that is NOT contained in the current delimiter] */
    TEST_FUNCTION(STRING_TOKENIZER_get_next_token_Start_With_Delimiter_Succeed)
    {
        ///arrange
        CStringTokenizerMocks stMocks;
        const char* inputString = "Pirlimpimpim";

        STRING_HANDLE input_string_handle = STRING_construct(inputString);
        STRING_HANDLE output_string_handle = STRING_construct(inputString);

        STRING_TOKENIZER_HANDLE t = STRING_TOKENIZER_create(input_string_handle);

        stMocks.ResetAllCalls();

        EXPECTED_CALL(stMocks, gballoc_realloc(0, 0))  //Alloc memory to copy result. 
            .IgnoreArgument(1)
            .IgnoreArgument(2);

        ///act
        int r = STRING_TOKENIZER_get_next_token(t, output_string_handle, "P");

        ///assert
        ASSERT_ARE_EQUAL(char_ptr, "irlimpimpim", STRING_c_str(output_string_handle));
        ASSERT_ARE_EQUAL(int, r, 0);
        stMocks.AssertActualAndExpectedCalls();
        //Cleanup
        STRING_TOKENIZER_destroy(t);
        STRING_delete(input_string_handle);
        STRING_delete(output_string_handle);
    }
    
    /* Tests_SRS_STRING_04_005: [STRING_TOKENIZER_get_next_token searches the string inside STRING_TOKENIZER_HANDLE for the first character that is NOT contained in the current delimiter] */
    /* Tests_SRS_STRING_04_008: [STRING_TOKENIZER_get_next_token than searches from the start of a token for a character that is contained in the delimiters string.] */
    TEST_FUNCTION(STRING_TOKENIZER_get_next_token_Start_And_End_With_Delimiter_Succeed)
    {
        ///arrange
        CStringTokenizerMocks stMocks;
        const char* inputString = "PirlimP";

        STRING_HANDLE input_string_handle = STRING_construct(inputString);
        STRING_HANDLE output_string_handle = STRING_construct(inputString);

        STRING_TOKENIZER_HANDLE t = STRING_TOKENIZER_create(input_string_handle);

        stMocks.ResetAllCalls();

        EXPECTED_CALL(stMocks, gballoc_realloc(0, 0))  //Alloc memory to copy result. 
            .IgnoreArgument(1)
            .IgnoreArgument(2);

        ///act
        int r = STRING_TOKENIZER_get_next_token(t, output_string_handle, "P");

        ///assert
        ASSERT_ARE_EQUAL(char_ptr, "irlim", STRING_c_str(output_string_handle));
        ASSERT_ARE_EQUAL(int, r, 0);
        stMocks.AssertActualAndExpectedCalls();
        //Cleanup
        STRING_TOKENIZER_destroy(t);
        STRING_delete(input_string_handle);
        STRING_delete(output_string_handle);
    }

    /* Tests_SRS_STRING_04_006: [If no such character is found, then STRING_TOKENIZER_get_next_token shall return a nonzero Value (You've reach the end of the string or the string consists with only delimiters).] */
    TEST_FUNCTION(STRING_TOKENIZER_get_next_token_with_All_token_String_Fail)
    {
        ///arrange
        CStringTokenizerMocks stMocks;
        const char* inputString = "PPPPPP";

        STRING_HANDLE input_string_handle = STRING_construct(inputString);
        STRING_HANDLE output_string_handle = STRING_construct(inputString);

        STRING_TOKENIZER_HANDLE t = STRING_TOKENIZER_create(input_string_handle);

        stMocks.ResetAllCalls();

        ///act
        int r = STRING_TOKENIZER_get_next_token(t, output_string_handle, "P");

        ///assert
        ASSERT_ARE_NOT_EQUAL(int, r, 0);
        stMocks.AssertActualAndExpectedCalls();
        //Cleanup
        STRING_TOKENIZER_destroy(t);
        STRING_delete(input_string_handle);
        STRING_delete(output_string_handle);
    }

    /* Tests_SRS_STRING_04_006: [If no such character is found, then STRING_TOKENIZER_get_next_token shall return a nonzero Value (You've reach the end of the string or the string consists with only delimiters).] */
    TEST_FUNCTION(STRING_TOKENIZER_get_next_token_2Times_With_Just_1Token_SecondCall_Fail)
    {
        ///arrange
        CStringTokenizerMocks stMocks;
        const char* inputString = "TestP";

        STRING_HANDLE input_string_handle = STRING_construct(inputString);
        STRING_HANDLE output_string_handle = STRING_construct(inputString);

        STRING_TOKENIZER_HANDLE t = STRING_TOKENIZER_create(input_string_handle);

        stMocks.ResetAllCalls();

        EXPECTED_CALL(stMocks, gballoc_realloc(0, 0))  //Alloc memory to copy result. 
            .IgnoreArgument(1)
            .IgnoreArgument(2);

        ///act1
        int r = STRING_TOKENIZER_get_next_token(t, output_string_handle, "P");

        ///Assert1
        ASSERT_ARE_EQUAL(char_ptr, "Test", STRING_c_str(output_string_handle));
        ASSERT_ARE_EQUAL(int, r, 0);

        ///act2
        r = STRING_TOKENIZER_get_next_token(t, output_string_handle, "P");

        ///assert
        ASSERT_ARE_NOT_EQUAL(int, r, 0);
        stMocks.AssertActualAndExpectedCalls();
        //Cleanup
        STRING_TOKENIZER_destroy(t);
        STRING_delete(input_string_handle);
        STRING_delete(output_string_handle);
    }

    /* Tests_SRS_STRING_04_007: [If such a character is found, STRING_TOKENIZER_get_next_token consider it as the start of a token.] */
    /* Tests_SRS_STRING_04_009: [If no such character is found, STRING_TOKENIZER_get_next_token extends the current token to the end of the string inside t, copies the token to output and returns 0.] */
    TEST_FUNCTION(STRING_TOKENIZER_get_next_token_2charactersToken_at_begin_of_input_call_1_Time_Succeed)
    {
        ///arrange
        CStringTokenizerMocks stMocks;
        const char* inputString = "??This is a Test";

        STRING_HANDLE input_string_handle = STRING_construct(inputString);
        STRING_HANDLE output_string_handle = STRING_construct(inputString);

        STRING_TOKENIZER_HANDLE t = STRING_TOKENIZER_create(input_string_handle);

        stMocks.ResetAllCalls();

        EXPECTED_CALL(stMocks, gballoc_realloc(0, 0))  //Alloc memory to copy result. 
            .IgnoreArgument(1)
            .IgnoreArgument(2);

        ///act1
        int r = STRING_TOKENIZER_get_next_token(t, output_string_handle, "?");

        ///Assert1
        ASSERT_ARE_EQUAL(char_ptr, "This is a Test", STRING_c_str(output_string_handle));
        ASSERT_ARE_EQUAL(int, r, 0);
        stMocks.AssertActualAndExpectedCalls();
        //Cleanup
        STRING_TOKENIZER_destroy(t);
        STRING_delete(input_string_handle);
        STRING_delete(output_string_handle);
    }

    /* Tests_SRS_STRING_04_010: [If such a character is found, STRING_TOKENIZER_get_next_token consider it the end of the token and copy it's content to output, updates the current position inside t to the next character and returns 0.] */
    /* Tests_SRS_STRING_04_011: [Each subsequent call to STRING_TOKENIZER_get_next_token starts searching from the saved position on t and behaves as described above.] */
    TEST_FUNCTION(STRING_TOKENIZER_get_next_token_stringWith3Tokens_Call3Times_Succeed)
    {
        ///arrange
        CStringTokenizerMocks stMocks;
        const char* inputString = "Test1PTest2PTest3";

        STRING_HANDLE input_string_handle = STRING_construct(inputString);
        STRING_HANDLE output_string_handle = STRING_construct(inputString);

        STRING_TOKENIZER_HANDLE t = STRING_TOKENIZER_create(input_string_handle);

        stMocks.ResetAllCalls();

        EXPECTED_CALL(stMocks, gballoc_realloc(0, 0))  //Alloc memory to copy result1. 
            .IgnoreArgument(1)
            .IgnoreArgument(2);
        EXPECTED_CALL(stMocks, gballoc_realloc(0, 0))  //Alloc memory to copy result2. 
            .IgnoreArgument(1)
            .IgnoreArgument(2);
        EXPECTED_CALL(stMocks, gballoc_realloc(0, 0))  //Alloc memory to copy result3. 
            .IgnoreArgument(1)
            .IgnoreArgument(2);

        ///act1
        int r = STRING_TOKENIZER_get_next_token(t, output_string_handle, "P");
        
        ///Assert1
        ASSERT_ARE_EQUAL(char_ptr, "Test1", STRING_c_str(output_string_handle));
        ASSERT_ARE_EQUAL(int, r, 0);

        ///act2
        r = STRING_TOKENIZER_get_next_token(t, output_string_handle, "P");

        ///Assert2
        ASSERT_ARE_EQUAL(char_ptr,"Test2", STRING_c_str(output_string_handle));
        ASSERT_ARE_EQUAL(int, r, 0);

        ///act3
        r = STRING_TOKENIZER_get_next_token(t, output_string_handle, "P");

        ///Assert3
        ASSERT_ARE_EQUAL(char_ptr,"Test3", STRING_c_str(output_string_handle));
        ASSERT_ARE_EQUAL(int, r, 0);
        stMocks.AssertActualAndExpectedCalls();
        //Cleanup
        STRING_TOKENIZER_destroy(t);
        STRING_delete(input_string_handle);
        STRING_delete(output_string_handle);
    }

    /* Tests_SRS_STRING_TOKENIZER_04_014: [STRING_TOKENIZER_get_next_token shall return nonzero value if t contains an empty string.] */
    TEST_FUNCTION(STRING_TOKENIZER_get_next_token_inputEmptyString_Fail)
    {
        ///arrange
        CStringTokenizerMocks stMocks;
        const char* inputString = "";

        STRING_HANDLE input_string_handle = STRING_construct(inputString);
        STRING_HANDLE output_string_handle = STRING_construct(inputString);

        STRING_TOKENIZER_HANDLE t = STRING_TOKENIZER_create(input_string_handle);

        stMocks.ResetAllCalls();

        ///act
        int r = STRING_TOKENIZER_get_next_token(t, output_string_handle, "P");

        ///Assert
        ASSERT_ARE_NOT_EQUAL(int, r, 0);
        stMocks.AssertActualAndExpectedCalls();
        //Cleanup
        STRING_TOKENIZER_destroy(t);
        STRING_delete(input_string_handle);
        STRING_delete(output_string_handle);
    }

    /* Tests_SRS_STRING_04_010: [If such a character is found, STRING_TOKENIZER_get_next_token consider it the end of the token and copy it's content to output, updates the current position inside t to the next character and returns 0.] */
    /* Tests_SRS_STRING_04_011: [Each subsequent call to STRING_TOKENIZER_get_next_token starts searching from the saved position on t and behaves as described above.] */
    TEST_FUNCTION(STRING_TOKENIZER_get_next_token_multipleCalls_DifferentToken_Succeed)
    {
        ///arrange
        CStringTokenizerMocks stMocks;
        const char* inputString = "?a???b,,,#c";

        STRING_HANDLE input_string_handle = STRING_construct(inputString);
        STRING_HANDLE output_string_handle = STRING_construct(inputString);

        STRING_TOKENIZER_HANDLE t = STRING_TOKENIZER_create(input_string_handle);

        stMocks.ResetAllCalls();

        ///act1
        EXPECTED_CALL(stMocks, gballoc_realloc(0, 0))  //Alloc memory to copy result. 
            .IgnoreArgument(1)
            .IgnoreArgument(2);


        int r = STRING_TOKENIZER_get_next_token(t, output_string_handle, "?");

        ///Assert1
        ASSERT_ARE_EQUAL(char_ptr,"a", STRING_c_str(output_string_handle));
        ASSERT_ARE_EQUAL(int, r, 0);

        ///act2
        EXPECTED_CALL(stMocks, gballoc_realloc(0, 0))  //Alloc memory to copy result. 
            .IgnoreArgument(1)
            .IgnoreArgument(2);


        r = STRING_TOKENIZER_get_next_token(t, output_string_handle, ",");

        ///Assert2
        ASSERT_ARE_EQUAL(char_ptr,"??b", STRING_c_str(output_string_handle));
        ASSERT_ARE_EQUAL(int, r, 0);

        ///act3
        EXPECTED_CALL(stMocks, gballoc_realloc(0, 0))  //Alloc memory to copy result. 
            .IgnoreArgument(1)
            .IgnoreArgument(2);

        r = STRING_TOKENIZER_get_next_token(t, output_string_handle, "#,");

        ///Assert3
        ASSERT_ARE_EQUAL(char_ptr,"c", STRING_c_str(output_string_handle));
        ASSERT_ARE_EQUAL(int, r, 0);

        ///act4
        r = STRING_TOKENIZER_get_next_token(t, output_string_handle, "?");

        ///Assert4
        ASSERT_ARE_NOT_EQUAL(int, r, 0);
        stMocks.AssertActualAndExpectedCalls();
        ///Cleanup
        STRING_TOKENIZER_destroy(t);
        STRING_delete(input_string_handle);
        STRING_delete(output_string_handle);
    }

    /* Tests_SRS_STRING_04_010: [If such a character is found, STRING_TOKENIZER_get_next_token consider it the end of the token and copy it's content to output, updates the current position inside t to the next character and returns 0.] */
    /* Tests_SRS_STRING_04_011: [Each subsequent call to STRING_TOKENIZER_get_next_token starts searching from the saved position on t and behaves as described above.] */
    TEST_FUNCTION(STRING_TOKENIZER_get_next_token_inputString_with_SingleCharacter_call2Times_succeed)
    {
        ///arrange
        CStringTokenizerMocks stMocks;
        const char* inputString = "c";

        STRING_HANDLE input_string_handle = STRING_construct(inputString);
        STRING_HANDLE output_string_handle = STRING_construct(inputString);

        STRING_TOKENIZER_HANDLE t = STRING_TOKENIZER_create(input_string_handle);

        stMocks.ResetAllCalls();

        ///act1
        EXPECTED_CALL(stMocks, gballoc_realloc(0, 0))  //Alloc memory to copy result. 
            .IgnoreArgument(1)
            .IgnoreArgument(2);


        int r = STRING_TOKENIZER_get_next_token(t, output_string_handle, "?");

        ///Assert1
        ASSERT_ARE_EQUAL(char_ptr,"c", STRING_c_str(output_string_handle));
        ASSERT_ARE_EQUAL(int, r, 0);

        ///act2
        r = STRING_TOKENIZER_get_next_token(t, output_string_handle, "?");

        ///Assert2
        ASSERT_ARE_NOT_EQUAL(int, r, 0);
        stMocks.AssertActualAndExpectedCalls();
        ///Clean Up
        STRING_TOKENIZER_destroy(t);
        STRING_delete(input_string_handle);
        STRING_delete(output_string_handle);
    }

    /* Tests_SRS_STRING_04_010: [If such a character is found, STRING_TOKENIZER_get_next_token consider it the end of the token and copy it's content to output, updates the current position inside t to the next character and returns 0.] */
    /* Tests_SRS_STRING_04_011: [Each subsequent call to STRING_TOKENIZER_get_next_token starts searching from the saved position on t and behaves as described above.] */
    TEST_FUNCTION(STRING_TOKENIZER_get_next_token_inputString_with_NULL_in_the_Middle_succeed)
    {
        ///arrange
        CStringTokenizerMocks stMocks;
        const char* inputString = "This is a Test \0 1234";

        STRING_HANDLE input_string_handle = STRING_construct(inputString);
        STRING_HANDLE output_string_handle = STRING_construct(inputString);

        STRING_TOKENIZER_HANDLE t = STRING_TOKENIZER_create(input_string_handle);

        stMocks.ResetAllCalls();

        ///act1
        EXPECTED_CALL(stMocks, gballoc_realloc(0, 0))  //Alloc memory to copy result. 
            .IgnoreArgument(1)
            .IgnoreArgument(2);


        int r = STRING_TOKENIZER_get_next_token(t, output_string_handle, "1");

        ///Assert1
        ASSERT_ARE_EQUAL(char_ptr,"This is a Test ", STRING_c_str(output_string_handle));
        ASSERT_ARE_EQUAL(int, r, 0);

        stMocks.AssertActualAndExpectedCalls();
        ///Clean Up
        STRING_TOKENIZER_destroy(t);
        STRING_delete(input_string_handle);
        STRING_delete(output_string_handle);
    }

    /* Tests_SRS_STRING_04_010: [If such a character is found, STRING_TOKENIZER_get_next_token consider it the end of the token and copy it's content to output, updates the current position inside t to the next character and returns 0.] */
    /* Tests_SRS_STRING_04_011: [Each subsequent call to STRING_TOKENIZER_get_next_token starts searching from the saved position on t and behaves as described above.] */
    TEST_FUNCTION(STRING_TOKENIZER_get_next_token_inputString_with_specialCharacters_succeed)
    {
        ///arrange
        CStringTokenizerMocks stMocks;
        const char* inputString = "This is a Test \r\n 1234 \r\n\t 12345";

        STRING_HANDLE input_string_handle = STRING_construct(inputString);
        STRING_HANDLE output_string_handle = STRING_construct(inputString);

        STRING_TOKENIZER_HANDLE t = STRING_TOKENIZER_create(input_string_handle);

        stMocks.ResetAllCalls();

        ///act1
        EXPECTED_CALL(stMocks, gballoc_realloc(0, 0))  //Alloc memory to copy result. 
            .IgnoreArgument(1)
            .IgnoreArgument(2);


        int r = STRING_TOKENIZER_get_next_token(t, output_string_handle, "\r\n");

        ///Assert1
        ASSERT_ARE_EQUAL(char_ptr,"This is a Test ", STRING_c_str(output_string_handle));
        ASSERT_ARE_EQUAL(int, r, 0);

        ///act2
        EXPECTED_CALL(stMocks, gballoc_realloc(0, 0))  //Alloc memory to copy result. 
            .IgnoreArgument(1)
            .IgnoreArgument(2);


        r = STRING_TOKENIZER_get_next_token(t, output_string_handle, "\r\n");

        ///Assert2
        ASSERT_ARE_EQUAL(char_ptr," 1234 ", STRING_c_str(output_string_handle));
        ASSERT_ARE_EQUAL(int, r, 0);

        ///act3
        EXPECTED_CALL(stMocks, gballoc_realloc(0, 0))  //Alloc memory to copy result. 
            .IgnoreArgument(1)
            .IgnoreArgument(2);


        r = STRING_TOKENIZER_get_next_token(t, output_string_handle, "\r\n\t");

        ///Assert3
        ASSERT_ARE_EQUAL(char_ptr," 12345", STRING_c_str(output_string_handle));
        ASSERT_ARE_EQUAL(int, r, 0);

        stMocks.AssertActualAndExpectedCalls();
        ///Clean Up
        STRING_TOKENIZER_destroy(t);
        STRING_delete(input_string_handle);
        STRING_delete(output_string_handle);
    }
    /* STRING_TOKENIZER_delete */
    /*Test_SRS_STRING_TOKENIZER_04_012: [STRING_TOKENIZER_destroy shall free the memory allocated by the STRING_TOKENIZER_create ] */
    TEST_FUNCTION(STRING_TOKENIZER_DESTROY_Succeed)
    {
        ///arrange
        CStringTokenizerMocks stMocks;
        const char* inputString = "Pirlimpimpim";

        STRING_HANDLE input_string_handle = STRING_construct(inputString);
        STRING_TOKENIZER_HANDLE t = STRING_TOKENIZER_create(input_string_handle);

        stMocks.ResetAllCalls();
        ///act

        EXPECTED_CALL(stMocks, gballoc_free(0))  //Free Token content.
            .IgnoreArgument(1);

        EXPECTED_CALL(stMocks, gballoc_free(0))  //Free Token.
            .IgnoreArgument(1);


        STRING_TOKENIZER_destroy(t);

        ///assert
        stMocks.AssertActualAndExpectedCalls();
        //Cleanup
        STRING_delete(input_string_handle);
    }

    /*Tests_SRS_STRING_TOKENIZER_04_013: [When the t argument is NULL, then STRING_TOKENIZER_destroy shall not attempt to free] */
    TEST_FUNCTION(STRING_TOKENIZER_DESTROY_WITH_NULL_HANDLE_NO_FREE)
    {
        ///arrange
        STRING_TOKENIZER_destroy(NULL);
        ///assert
    }

END_TEST_SUITE(string_tokenizer_unittests)