// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef C_SIZEOF_BOOL_H
#define C_SIZEOF_BOOL_H


#ifdef __cplusplus
#include <cstddef>
extern "C"
{
#else 
#include <stddef.h>
#endif

extern size_t sizeof_bool(void);

#ifdef __cplusplus
}
#endif

#endif