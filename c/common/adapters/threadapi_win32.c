// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#include "threadapi.h"
#include "windows.h"
#include "iot_logging.h"

DEFINE_ENUM_STRINGS(THREADAPI_RESULT, THREADAPI_RESULT_VALUES);

THREADAPI_RESULT ThreadAPI_Create(THREAD_HANDLE* threadHandle, THREAD_START_FUNC func, void* arg)
{
    THREADAPI_RESULT result;
    if ((threadHandle == NULL) ||
        (func == NULL))
    {
        result = THREADAPI_INVALID_ARG;
        LogError("(result = %s)\r\n", ENUM_TO_STRING(THREADAPI_RESULT, result));
    }
    else
    {
        *threadHandle = CreateThread(NULL, 0, func, arg, 0, NULL);
        if(threadHandle == NULL)
        {
            result = THREADAPI_ERROR;
            LogError("(result = %s)\r\n", ENUM_TO_STRING(THREADAPI_RESULT, result));
        }
        else
        {
            result = THREADAPI_OK;
        }
    }

    return result;
}

THREADAPI_RESULT ThreadAPI_Join(THREAD_HANDLE threadHandle, int *res)
{
    THREADAPI_RESULT result = THREADAPI_OK;

    if (threadHandle == NULL)
    {
        result = THREADAPI_INVALID_ARG;
        LogError("(result = %s)\r\n", ENUM_TO_STRING(THREADAPI_RESULT, result));
    }
    else
    {
        DWORD returnCode = WaitForSingleObject(threadHandle, INFINITE);
        
        if( returnCode != WAIT_OBJECT_0)
        {
            result = THREADAPI_ERROR;
            LogError("Error waiting for Single Object. Return Code: %d. Error Code: %d\r\n", returnCode);
        }
        else if((res != NULL) && !GetExitCodeThread(threadHandle, res)) //If thread end is signaled we need to get the Thread Exit Code;
        {
            DWORD errorCode = GetLastError();
            result = THREADAPI_ERROR;
            LogError("Error Getting Exit Code. Error Code: %d.\r\n", errorCode);
        }
        CloseHandle(threadHandle);
    }

    return result;
}

void ThreadAPI_Exit(int res)
{
    ExitThread(res);
}

void ThreadAPI_Sleep(unsigned int milliseconds)
{
    Sleep(milliseconds);
}
