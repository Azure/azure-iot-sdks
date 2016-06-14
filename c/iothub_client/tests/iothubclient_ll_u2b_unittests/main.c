// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef USE_UPLOADTOBLOB
#error "trying to compile main.c without #define USE_UPLOADTOBLOB"
#else

#include "testrunnerswitcher.h"

#include <stddef.h>

int main(void)
{
    size_t failedTestCount = 0;

    RUN_TEST_SUITE(iothubclient_ll_uploadtoblob_unittests, failedTestCount);
    return failedTestCount;
}

#endif /*USE_UPLOADTOBLOB*/
