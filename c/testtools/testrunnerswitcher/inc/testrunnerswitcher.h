// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef TESTRUNNERSWITCHER_H
#define TESTRUNNERSWITCHER_H

#ifdef MBED_BUILD_TIMESTAMP
#define USE_CTEST
#endif

#ifdef USE_CTEST

#include "ctest.h"

#define BEGIN_TEST_SUITE(name)          CTEST_BEGIN_TEST_SUITE(name)
#define END_TEST_SUITE(name)            CTEST_END_TEST_SUITE(name)

#define TEST_SUITE_INITIALIZE(name)     CTEST_SUITE_INITIALIZE()
#define TEST_SUITE_CLEANUP(name)        CTEST_SUITE_CLEANUP()
#define TEST_FUNCTION_INITIALIZE(name)  CTEST_FUNCTION_INITIALIZE()
#define TEST_FUNCTION_CLEANUP(name)     CTEST_FUNCTION_CLEANUP()

#define TEST_FUNCTION(name)             CTEST_FUNCTION(name)

#define ASSERT_ARE_EQUAL                CTEST_ASSERT_ARE_EQUAL
#define ASSERT_ARE_EQUAL_WITH_MSG       CTEST_ASSERT_ARE_EQUAL_WITH_MSG
#define ASSERT_ARE_NOT_EQUAL            CTEST_ASSERT_ARE_NOT_EQUAL
#define ASSERT_ARE_NOT_EQUAL_WITH_MSG   CTEST_ASSERT_ARE_NOT_EQUAL_WITH_MSG
#define ASSERT_FAIL                     CTEST_ASSERT_FAIL
#define ASSERT_IS_NULL                  CTEST_ASSERT_IS_NULL
#define ASSERT_IS_NULL_WITH_MSG         CTEST_ASSERT_IS_NULL_WITH_MSG
#define ASSERT_IS_NOT_NULL              CTEST_ASSERT_IS_NOT_NULL
#define ASSERT_IS_NOT_NULL_WITH_MSG     CTEST_ASSERT_IS_NOT_NULL_WITH_MSG
#define ASSERT_IS_TRUE                  CTEST_ASSERT_IS_TRUE
#define ASSERT_IS_TRUE_WITH_MSG         CTEST_ASSERT_IS_TRUE_WITH_MSG
#define ASSERT_IS_FALSE                 CTEST_ASSERT_IS_FALSE
#define ASSERT_IS_FALSE_WITH_MSG        CTEST_ASSERT_IS_FALSE_WITH_MSG

#define RUN_TEST_SUITE(...)             CTEST_RUN_TEST_SUITE(__VA_ARGS__)

#elif defined CPP_UNITTEST

#include "CppUnitTest.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

typedef const char* char_ptr;
typedef void* void_ptr;

#define BEGIN_TEST_SUITE(name)          TEST_CLASS(name) {

#define END_TEST_SUITE(name)            };

#define TEST_SUITE_INITIALIZE(name)     TEST_CLASS_INITIALIZE(name)
#define TEST_SUITE_CLEANUP(name)        TEST_CLASS_CLEANUP(name)
#define TEST_FUNCTION_INITIALIZE(name)  TEST_METHOD_INITIALIZE(name)
#define TEST_FUNCTION_CLEANUP(name)     TEST_METHOD_CLEANUP(name)

#define TEST_FUNCTION(name)             TEST_METHOD(name)

#define ASSERT_ARE_EQUAL(type, A, B)                        Assert::AreEqual((type)A, (type)B)
#define ASSERT_ARE_EQUAL_WITH_MSG(type, A, B, message)      Assert::AreEqual((type)A, (type)B, ToString(message).c_str())
#define ASSERT_ARE_NOT_EQUAL(type, A, B)                    Assert::AreNotEqual((type)A, (type)B)
#define ASSERT_ARE_NOT_EQUAL_WITH_MSG(type, A, B, message)  Assert::AreNotEqual((type)A, (type)B, ToString(message).c_str())
#define ASSERT_FAIL(message)                                Assert::Fail(ToString(message).c_str())
#define ASSERT_IS_TRUE(expression)                          Assert::IsTrue(expression)
#define ASSERT_IS_TRUE_WITH_MSG(expression, message)        Assert::IsTrue(expression, ToString(message).c_str())
#define ASSERT_IS_FALSE(expression)                         Assert::IsFalse(expression)
#define ASSERT_IS_FALSE_WITH_MSG(expression, message)       Assert::IsFalse(expression, ToString(message).c_str())
#define ASSERT_IS_NOT_NULL(value)                           Assert::IsNotNull(value)
#define ASSERT_IS_NOT_NULL_WITH_MSG(value, message)         Assert::IsNotNull(value, ToString(message).c_str())
#define ASSERT_IS_NULL(value)                               Assert::IsNull(value)
#define ASSERT_IS_NULL_WITH_MSG(value, message)             Assert::IsNull(value, ToString(message).c_str())

#define RUN_TEST_SUITE(...)

#else
#error No test runner defined
#endif

#define SEMAPHORE_HIGH_WATER 1000000

#ifdef _CRTDBG_MAP_ALLOC

#ifdef CPP_UNITTEST
#define WRITE_LEAK_MESSAGE Logger::WriteMessage(" Memory Leak found!!! Run test in debug mode & review Debug output for details.");
#else
#define WRITE_LEAK_MESSAGE ((void)0);
#endif

#define REPORT_MEMORY_LEAKS if (_CrtDumpMemoryLeaks()) { WRITE_LEAK_MESSAGE }
#else
#define REPORT_MEMORY_LEAKS ((void)0);
#endif

#define INITIALIZE_MEMORY_DEBUG(semaphore) \
semaphore = MicroMockCreateGlobalSemaphore("MICROMOCK_DLL_BY_DLL", SEMAPHORE_HIGH_WATER); \
ASSERT_IS_NOT_NULL(semaphore); \
MicroMockAcquireGlobalSemaphore(semaphore) ; \

#define DEINITIALIZE_MEMORY_DEBUG(semaphore)\
if (MicroMockReleaseGlobalSemaphore(g_dllByDll) == SEMAPHORE_HIGH_WATER-1)\
{                                                        \
    REPORT_MEMORY_LEAKS;                                 \
}                                                        \
MicroMockDestroyGlobalSemaphore(g_dllByDll);             \

#endif
