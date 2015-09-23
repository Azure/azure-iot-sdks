// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <cstdlib>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "testrunnerswitcher.h"
#include "hmacsha256.h"
#include "strings.h"
#include "micromock.h"
#include "micromockcharstararenullterminatedstrings.h"


static MICROMOCK_MUTEX_HANDLE g_testByTest;

DEFINE_MICROMOCK_ENUM_TO_STRING(HMACSHA256_RESULT, HMACSHA256_RESULT_VALUES);

static BUFFER_HANDLE hash;

static MICROMOCK_GLOBAL_SEMAPHORE_HANDLE g_dllByDll;

BEGIN_TEST_SUITE(HMACSHA256_UnitTests)

TEST_SUITE_INITIALIZE(TestClassInitialize)
{
    INITIALIZE_MEMORY_DEBUG(g_dllByDll);

    g_testByTest = MicroMockCreateMutex();
    ASSERT_IS_NOT_NULL(g_testByTest);
}

TEST_SUITE_CLEANUP(TestClassCleanup)
{
    MicroMockDestroyMutex(g_testByTest);
    DEINITIALIZE_MEMORY_DEBUG(g_dllByDll);

}

TEST_FUNCTION_INITIALIZE(TestMethodInitialize)
{
    if (!MicroMockAcquireMutex(g_testByTest))
    {
        ASSERT_FAIL("our mutex is ABANDONED. Failure in test framework");
    }
    hash = BUFFER_new();
}

TEST_FUNCTION_CLEANUP(TestMethodCleanup)
{
    BUFFER_delete(hash);
    if (!MicroMockReleaseMutex(g_testByTest))
    {
        ASSERT_FAIL("failure in test framework at ReleaseMutex");
    }
}

/* HMACSHA256_ComputeHash */

TEST_FUNCTION(HMACSHA256_ComputeHash_With_NULL_Key_Fails)
{
    // arrange
    static const unsigned char key[] = "key";
    static const unsigned char buffer[] = "testPayload";

    // act
    HMACSHA256_RESULT result = HMACSHA256_ComputeHash(NULL, sizeof(key) - 1, buffer, sizeof(buffer) - 1, hash);

    // assert
    ASSERT_ARE_EQUAL(HMACSHA256_RESULT, HMACSHA256_INVALID_ARG, result);
}

TEST_FUNCTION(HMACSHA256_ComputeHash_With_Zero_Key_Buffer_Size_Fails)
{
    // arrange
    static const unsigned char key[] = "key";
    static const unsigned char buffer[] = "testPayload";

    // act
    HMACSHA256_RESULT result = HMACSHA256_ComputeHash(key, 0, buffer, sizeof(buffer) - 1, hash);

    // assert
    ASSERT_ARE_EQUAL(HMACSHA256_RESULT, HMACSHA256_INVALID_ARG, result);
}

TEST_FUNCTION(HMACSHA256_ComputeHash_With_NULL_Payload_Fails)
{
    // arrange
    static const unsigned char key[] = "key";
    static const unsigned char buffer[] = "testPayload";

    // act
    HMACSHA256_RESULT result = HMACSHA256_ComputeHash(key, sizeof(key) - 1, NULL, sizeof(buffer) - 1, hash);

    // assert
    ASSERT_ARE_EQUAL(HMACSHA256_RESULT, HMACSHA256_INVALID_ARG, result);
}

TEST_FUNCTION(HMACSHA256_ComputeHash_With_Zero_Payload_Buffer_Size_Fails)
{
    // arrange
    static const unsigned char key[] = "key";
    static const unsigned char buffer[] = "testPayload";

    // act
    HMACSHA256_RESULT result = HMACSHA256_ComputeHash(key, sizeof(key) - 1, buffer, 0, hash);

    // assert
    ASSERT_ARE_EQUAL(HMACSHA256_RESULT, HMACSHA256_INVALID_ARG, result);
}

TEST_FUNCTION(HMACSHA256_ComputeHash_With_NULL_Hash_Fails)
{
    // arrange
    static const unsigned char key[] = "key";
    static const unsigned char buffer[] = "testPayload";

    // act
    HMACSHA256_RESULT result = HMACSHA256_ComputeHash(key, sizeof(key) - 1, buffer, sizeof(buffer) - 1, NULL);

    // assert
    ASSERT_ARE_EQUAL(HMACSHA256_RESULT, HMACSHA256_INVALID_ARG, result);
}

TEST_FUNCTION(HMACSHA256_ComputeHash_Succeeds)
{
    // arrange
    static const unsigned char key[] = "key";
    static const unsigned char buffer[] = "testPayload";
    unsigned char expectedHash[32] = { 108, 7, 130, 47, 104, 233, 39, 188, 126, 122, 134, 187, 63, 19, 52, 120, 172, 7, 43, 25, 133, 60, 92, 217, 59, 59, 69, 116, 85, 104, 55, 224 };

    // act
    HMACSHA256_RESULT result = HMACSHA256_ComputeHash(key, sizeof(key) - 1, buffer, sizeof(buffer) - 1, hash);

    // assert
    ASSERT_ARE_EQUAL(HMACSHA256_RESULT, HMACSHA256_OK, result);
    ASSERT_ARE_EQUAL(int, 0, memcmp(BUFFER_u_char(hash), expectedHash, 8));
}

END_TEST_SUITE(HMACSHA256_UnitTests)
