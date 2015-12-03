// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "ctest.h"

#ifndef WINCE
int main(int argc, char* argv[])
#else
int main(int argc, wchar_t* argv[])
#endif
{
  size_t failedTests = 0;

  (void)(argc, argv);

  /* This first suite is ran without counting failed tests to prove that the argument is optional. */
  CTEST_RUN_TEST_SUITE(SimpleTestSuiteOneTest);

	CTEST_RUN_TEST_SUITE(SimpleTestSuiteOneTest, failedTests);
	CTEST_RUN_TEST_SUITE(SimpleTestSuiteTwoTests, failedTests);
  CTEST_RUN_TEST_SUITE(AssertFailureTests, failedTests);
  CTEST_RUN_TEST_SUITE(AssertSuccessTests, failedTests);
  CTEST_RUN_TEST_SUITE(TestSuiteInitializeCleanupTests, failedTests);
  failedTests -= 71; /*71 is the number of tests EXPECTED to fail in the previous unit. Because there should be exactly 0 failed tests...*/
  CTEST_RUN_TEST_SUITE(TestFunctionInitializeTests, failedTests);
  CTEST_RUN_TEST_SUITE(TestFunctionCleanupTests, failedTests);

  return failedTests;
}
