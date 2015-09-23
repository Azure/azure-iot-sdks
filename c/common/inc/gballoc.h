// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef GBALLOC_H
#define GBALLOC_H


#ifdef __cplusplus
#include <cstdlib>
extern "C"
{
#else
#include <stdlib.h>
#endif
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

/* all translation units that need memory measurement need to have GB_MEASURE_MEMORY_FOR_THIS defined */
/* GB_DEBUG_ALLOC is the switch that turns the measurement on/off, so that it is not on always */
#if defined(GB_DEBUG_ALLOC)

extern int gballoc_init(void);
extern void gballoc_deinit(void);
extern void* gballoc_malloc(size_t size);
extern void* gballoc_calloc(size_t nmemb, size_t size);
extern void* gballoc_realloc(void* ptr, size_t size);
extern void gballoc_free(void* ptr);

extern size_t gballoc_getMaximumMemoryUsed(void);
extern size_t gballoc_getCurrentMemoryUsed(void);

/* if GB_MEASURE_MEMORY_FOR_THIS is defined then we want to redirect memory allocation functions to gballoc_xxx functions */
#ifdef GB_MEASURE_MEMORY_FOR_THIS
#if defined(_CRTDBG_MAP_ALLOC) && defined(_DEBUG)
#undef _malloc_dbg
#undef _calloc_dbg
#undef _realloc_dbg
#undef _free_dbg
#define _malloc_dbg(size, ...) gballoc_malloc(size)
#define _calloc_dbg(nmemb, size, ...) gballoc_calloc(nmemb, size)
#define _realloc_dbg(ptr, size, ...) gballoc_realloc(ptr, size)
#define _free_dbg(ptr, ...) gballoc_free(ptr)
#else
#define malloc gballoc_malloc
#define calloc gballoc_calloc
#define realloc gballoc_realloc
#define free gballoc_free
#endif
#endif

#else /* GB_DEBUG_ALLOC */

#define gballoc_init() 0
#define gballoc_deinit() ((void)0)

#define gballoc_getMaximumMemoryUsed() SIZE_MAX
#define gballoc_getCurrentMemoryUsed() SIZE_MAX

#endif /* GB_DEBUG_ALLOC */

#ifdef __cplusplus
}
#endif

#endif /* GBALLOC_H */
