// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <cstdlib>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "testrunnerswitcher.h"
#include "micromock.h"
#include "micromockcharstararenullterminatedstrings.h"

#include "agenttime.h"



static MICROMOCK_MUTEX_HANDLE g_testByTest;


time_t my_time64(time_t * _Time)
{
    (void)_Time;
    return (int64_t)-1;
}

struct tm* my_gmtime64(time_t* _Time)
{
    (void)_Time;
    return NULL;
}

struct tm* my_localtime64(time_t * _Time)
{
    (void)_Time;
    return NULL;
}

time_t my_mktime64(struct tm* _Time)
{
    (void)_Time;
    return (int64_t)-1;
}

static MICROMOCK_GLOBAL_SEMAPHORE_HANDLE g_dllByDll;

BEGIN_TEST_SUITE(agenttime_unittests)

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
        }

        TEST_FUNCTION_CLEANUP(TestMethodCleanup)
        {
            if (!MicroMockReleaseMutex(g_testByTest))
            {
                ASSERT_FAIL("failure in test framework at ReleaseMutex");
            }
        }

        TEST_FUNCTION(get_time_succeed)
        {
            ///act
            time_t t;
            auto result = get_time(&t);

            ///assert
            ASSERT_ARE_EQUAL(int, (int)result, (int)t);
        }

        TEST_FUNCTION(get_gmtime_success)
        {
            ///act
            time_t now;
            struct tm* p;
            now = get_time(NULL);
            p = get_gmtime(&now);

            ///assert
            ASSERT_IS_NOT_NULL(p);
        }

        TEST_FUNCTION(get_difftime_success)
        {
            time_t now, sometimeAfterNow;
            now = get_time(NULL);
            sometimeAfterNow = now + 42; /*whatever this is*/
            double diff;

            ///act
            diff = get_difftime(sometimeAfterNow, now);
            
            ///assert
            ASSERT_ARE_EQUAL(double, difftime(sometimeAfterNow, now), diff);
        }


END_TEST_SUITE(agenttime_unittests)

