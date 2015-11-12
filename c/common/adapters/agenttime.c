// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifndef WINCE
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#endif
#include "gballoc.h"

#include "time.h"
#include "agenttime.h"

time_t get_time(time_t* p)
{
    return time(p);
}

struct tm* get_gmtime(time_t* currentTime)
{
    return gmtime(currentTime);
}

char* get_ctime(time_t* timeToGet)
{
    return ctime(timeToGet);
}

double get_difftime(time_t stopTime, time_t startTime)
{
    return difftime(stopTime, startTime);
}