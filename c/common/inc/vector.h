// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef VECTOR_H
#define VECTOR_H

#include "crt_abstractions.h"

#ifdef __cplusplus
#include <cstddef>
extern "C"
{
#else
#include <stddef.h>
#endif

typedef void* VECTOR_HANDLE;

typedef bool(*PREDICATE_FUNCTION)(const void* element, const void* value);

/* creation */
extern VECTOR_HANDLE VECTOR_create(size_t elementSize);
extern void VECTOR_destroy(VECTOR_HANDLE handle);

/* insertion */
extern int VECTOR_push_back(VECTOR_HANDLE handle, const void* elements, size_t numElements);

/* removal */
extern void VECTOR_erase(VECTOR_HANDLE handle, void* elements, size_t numElements);
extern void VECTOR_clear(VECTOR_HANDLE handle);

/* access */
extern void* VECTOR_element(const VECTOR_HANDLE handle, size_t index);
extern void* VECTOR_front(const VECTOR_HANDLE handle);
extern void* VECTOR_back(const VECTOR_HANDLE handle);
extern void* VECTOR_find_if(const VECTOR_HANDLE handle, PREDICATE_FUNCTION pred, const void* value);

/* capacity */
extern size_t VECTOR_size(const VECTOR_HANDLE handle);

#ifdef __cplusplus
}
#else
#endif

#endif /* VECTOR_H */
