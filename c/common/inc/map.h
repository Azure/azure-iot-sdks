// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef MAP_H
#define MAP_H

#ifdef __cplusplus
#include <cstddef>
extern "C"
{
#else
#include <stddef.h>
#endif


#include "macro_utils.h"
#include "strings.h"
#include "crt_abstractions.h"

#define MAP_RESULT_VALUES \
    MAP_OK, \
    MAP_ERROR, \
    MAP_INVALIDARG, \
    MAP_KEYEXISTS, \
    MAP_KEYNOTFOUND, \
    MAP_FILTER_REJECT

DEFINE_ENUM(MAP_RESULT, MAP_RESULT_VALUES);

typedef void* MAP_HANDLE;

typedef int (*MAP_FILTER_CALLBACK)(const char* mapProperty, const char* mapValue);

extern MAP_HANDLE Map_Create(MAP_FILTER_CALLBACK mapFilterFunc);
extern void Map_Destroy(MAP_HANDLE handle);
extern MAP_HANDLE Map_Clone(MAP_HANDLE handle);

extern MAP_RESULT Map_Add(MAP_HANDLE handle, const char* key, const char* value);
extern MAP_RESULT Map_AddOrUpdate(MAP_HANDLE handle, const char* key, const char* value);
extern MAP_RESULT Map_Delete(MAP_HANDLE handle, const char* key);

extern MAP_RESULT Map_ContainsKey(MAP_HANDLE handle, const char* key, bool* keyExists);
extern MAP_RESULT Map_ContainsValue(MAP_HANDLE handle, const char* value, bool* valueExists);
extern const char* Map_GetValueFromKey(MAP_HANDLE handle, const char* key);

extern MAP_RESULT Map_GetInternals(MAP_HANDLE handle, const char*const** keys, const char*const** values, size_t* count);

#ifdef __cplusplus
}
#endif

#endif /* MAP_H */
