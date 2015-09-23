// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#include "stdlib.h"

#include "macro_utils.h"
#include "lock.h"
#include <thr/threads.h>
#include "iot_logging.h"

DEFINE_ENUM_STRINGS(LOCK_RESULT, LOCK_RESULT_VALUES);

/*SRS_LOCK_99_002:[ This API on success will return a valid lock handle which should be a non NULL value]*/
LOCK_HANDLE Lock_Init(void)
{
    mtx_t* lock_mtx = (mtx_t*)malloc(sizeof(mtx_t));
    if (NULL != lock_mtx)
    {
        if (mtx_init(lock_mtx, mtx_plain) != thrd_success)
        {
            /*SRS_LOCK_99_003:[ On Error Should return NULL]*/
            free(lock_mtx);
            lock_mtx = NULL;
            LogError("Failed to initialize mutex\r\n");
        }
    }
    
    return (LOCK_HANDLE)lock_mtx;
}


LOCK_RESULT Lock(LOCK_HANDLE handle)
{
    LOCK_RESULT result;
    if (handle == NULL)
    {
        /*SRS_LOCK_99_007:[ This API on NULL handle passed returns LOCK_ERROR]*/
        result = LOCK_ERROR;
        LogError("(result = %s)\r\n", ENUM_TO_STRING(LOCK_RESULT, result));
    }
    else
    {
        if (mtx_lock((mtx_t*)handle) == thrd_success)
        {
            /*SRS_LOCK_99_005:[ This API on success should return LOCK_OK]*/
            result = LOCK_OK;
        }
        else
        {
            /*SRS_LOCK_99_006:[ This API on error should return LOCK_ERROR]*/
            result = LOCK_ERROR;
            LogError("(result = %s)\r\n", ENUM_TO_STRING(LOCK_RESULT, result));
        }
    }
    return result;
}
LOCK_RESULT Unlock(LOCK_HANDLE handle)
{
    LOCK_RESULT result;
    if (handle == NULL)
    {
        /*SRS_LOCK_99_011:[ This API on NULL handle passed returns LOCK_ERROR]*/
        result = LOCK_ERROR;
        LogError("(result = %s)\r\n", ENUM_TO_STRING(LOCK_RESULT, result));
    }
    else
    {
        if (mtx_unlock((mtx_t*)handle) == thrd_success)
        {
            /*SRS_LOCK_99_009:[ This API on success should return LOCK_OK]*/
            result = LOCK_OK;
        }
        else
        {
            /*SRS_LOCK_99_010:[ This API on error should return LOCK_ERROR]*/
            result = LOCK_ERROR;
            LogError("(result = %s)\r\n", ENUM_TO_STRING(LOCK_RESULT, result));
        }
    }
    return result;
}

LOCK_RESULT Lock_Deinit(LOCK_HANDLE handle)
{
    LOCK_RESULT result=LOCK_OK ;
    if (NULL == handle)
    {
        /*SRS_LOCK_99_013:[ This API on NULL handle passed returns LOCK_ERROR]*/
        result = LOCK_ERROR;
        LogError("(result = %s)\r\n", ENUM_TO_STRING(LOCK_RESULT, result));
    }
    else
    {
        /*SRS_LOCK_99_012:[ This API frees the memory pointed by handle]*/
        (void)mtx_destroy((mtx_t*)handle);
        free(handle);
        handle = NULL;
    }
    
    return result;
}
