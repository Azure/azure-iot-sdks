// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifdef DONT_USE_UPLOADTOBLOB
#error "trying to compile main.c while DONT_USE_UPLOADTOBLOB is #define'd"
#else

#include "testrunnerswitcher.h"

#include <stddef.h>

int main(void)
{
    size_t failedTestCount = 0;

    RUN_TEST_SUITE(iothubclient_ll_uploadtoblob_ut, failedTestCount);
    return failedTestCount;
}

#endif /*DONT_USE_UPLOADTOBLOB*/
