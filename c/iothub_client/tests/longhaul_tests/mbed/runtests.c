// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stddef.h>
#include "testrunnerswitcher.h"
#include "runtests.h"

int run_tests(void)
{
        size_t failedTestCount = 0;
        RUN_TEST_SUITE(longhaul_tests, failedTestCount);
        return (int)failedTestCount;
}
