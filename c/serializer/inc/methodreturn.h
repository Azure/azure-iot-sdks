// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef METHODRETURN_H
#define METHODRETURN_H

typedef struct METHODRETURN_HANDLE_DATA_TAG* METHODRETURN_HANDLE;

#include "azure_c_shared_utility/macro_utils.h"

/*the following macro expands to "const" if X is defined. If X is not defined, then it expands to nothing*/
#define CONST_BY_COMPILATION_UNIT(X) IF(COUNT_ARG(X),const,)

typedef struct METHODRETURN_DATA_TAG
{
    CONST_BY_COMPILATION_UNIT(METHODRETURN_C) int statusCode;
    CONST_BY_COMPILATION_UNIT(METHODRETURN_C) char* jsonValue;
}METHODRETURN_DATA;

#include "azure_c_shared_utility/umock_c_prod.h"

#ifdef __cplusplus
extern "C" 
{
#endif

MOCKABLE_FUNCTION(, METHODRETURN_HANDLE, MethodReturn_Create, int, statusCode, const char*, jsonValue);
MOCKABLE_FUNCTION(, void, MethodReturn_Destroy, METHODRETURN_HANDLE, handle);
MOCKABLE_FUNCTION(, const METHODRETURN_DATA*, MethodReturn_GetReturn, METHODRETURN_HANDLE, handle);

#ifdef __cplusplus
}
#endif


#endif /*METHODRETURN_H*/

