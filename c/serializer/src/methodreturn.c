// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#include "azure_c_shared_utility/gballoc.h"

#include "methodreturn.h"

typedef struct METHODRETURN_HANDLE_DATA_TAG
{
    int statusCode;
    const char* jsonValue;
}METHODRETURN_HANDLE_DATA;

METHODRETURN_HANDLE MethodReturn_Create(int statusCode, const char* jsonValue)
{
    (void)(statusCode, jsonValue);
    return NULL;
}

void MethodReturn_Destroy(METHODRETURN_HANDLE handle)
{

}

int MethodReturn_GetStatusCode(METHODRETURN_HANDLE handle)
{

}

const char* MethodReturn_GetJsonValue(METHODRETURN_HANDLE handle)
{

}

