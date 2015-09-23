// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#include "gballoc.h"

#if defined(__cplusplus)
#include <cstdint>
#else
#include <stdint.h>
#endif

#include <thr/threads.h>
#include "threadapi.h"
#include "windows.h"
#include "iot_logging.h"

DEFINE_ENUM_STRINGS(THREADAPI_RESULT, THREADAPI_RESULT_VALUES);

THREADAPI_RESULT ThreadAPI_Create(THREAD_HANDLE* threadHandle, THREAD_START_FUNC func, void* arg)
{
    THREADAPI_RESULT result;
    int thrd_create_result = thrd_success;
    if ((threadHandle == NULL) ||
        (func == NULL))
    {
        result = THREADAPI_INVALID_ARG;
        LogError("(result = %s)\r\n", ENUM_TO_STRING(THREADAPI_RESULT, result));
    }
    else
    {
        thrd_t* thrd_t_ptr = (thrd_t*)malloc(sizeof(thrd_t));
        if (thrd_t_ptr == NULL)
        {
            result = THREADAPI_NO_MEMORY;
            LogError("(result = %s)\r\n", ENUM_TO_STRING(THREADAPI_RESULT, result));
        }
        else
        {
            thrd_create_result = thrd_create(thrd_t_ptr, func, arg);
            if (thrd_create_result != thrd_success)
            {
                free(thrd_t_ptr);
                result = THREADAPI_ERROR;
                LogError("(result = %s)\r\n", ENUM_TO_STRING(THREADAPI_RESULT, result));
            }
            else
            {
                result = THREADAPI_OK;
                *threadHandle = (THREAD_HANDLE)thrd_t_ptr;
            }
        }
    }

    return result;
}

THREADAPI_RESULT ThreadAPI_Join(THREAD_HANDLE threadHandle, int *res)
{
    THREADAPI_RESULT result;
    thrd_t* thrd_t_ptr = (thrd_t*)threadHandle;

    if (threadHandle == NULL)
    {
        result = THREADAPI_INVALID_ARG;
        LogError("(result = %s)\r\n", ENUM_TO_STRING(THREADAPI_RESULT, result));
    }
    else
    {
        switch (thrd_join(*thrd_t_ptr, res))
        {
        default:
        case thrd_error:
            result = THREADAPI_ERROR;
            LogError("(result = %s)\r\n", ENUM_TO_STRING(THREADAPI_RESULT, result));
            break;

        case thrd_success:
            result = THREADAPI_OK;
            break;

        case thrd_nomem:
            result = THREADAPI_NO_MEMORY;
            LogError("(result = %s)\r\n", ENUM_TO_STRING(THREADAPI_RESULT, result));
            break;
        }

        free(thrd_t_ptr);
    }

    return result;
}

void ThreadAPI_Exit(int res)
{
    thrd_exit(res);
}

void ThreadAPI_Sleep(unsigned int milliseconds)
{
    HANDLE handle = CreateEventEx(NULL, NULL, 0, EVENT_ALL_ACCESS);

    if (handle != NULL)
    {
        /*
         * Have to use at least 1 to cause a thread yield in case 0 is passed
         */
        (void)WaitForSingleObjectEx(handle, milliseconds == 0 ? 1 : milliseconds, FALSE);
        (void)CloseHandle(handle);
    }
}
