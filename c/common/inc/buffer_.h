// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef BUFFER_H
#define BUFFER_H

#ifdef __cplusplus
#include <cstddef>
extern "C"
{
#else
#include <stddef.h>
#endif

typedef void* BUFFER_HANDLE;

extern BUFFER_HANDLE BUFFER_new(void);
extern BUFFER_HANDLE BUFFER_create(const unsigned char* source, size_t size);
extern void BUFFER_delete(BUFFER_HANDLE handle);
extern int BUFFER_pre_build(BUFFER_HANDLE handle, size_t size);
extern int BUFFER_build(BUFFER_HANDLE handle, const unsigned char* source, size_t size);
extern int BUFFER_unbuild(BUFFER_HANDLE handle);
extern int BUFFER_enlarge(BUFFER_HANDLE handle, size_t enlargeSize);
extern int BUFFER_content(BUFFER_HANDLE handle, const unsigned char** content);
extern int BUFFER_size(BUFFER_HANDLE handle, size_t* size);
extern int BUFFER_append(BUFFER_HANDLE handle1, BUFFER_HANDLE handle2);
extern unsigned char* BUFFER_u_char(BUFFER_HANDLE handle);
extern size_t BUFFER_length(BUFFER_HANDLE handle);
extern BUFFER_HANDLE BUFFER_clone(BUFFER_HANDLE handle);

#ifdef __cplusplus
}
#endif


#endif  /* BUFFER_H */
