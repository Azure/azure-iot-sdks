// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef STRINGS_H
#define STRINGS_H

#ifdef __cplusplus
#include <cstddef>
extern "C"
{
#else
#include <stddef.h>
#endif

typedef void* STRING_HANDLE;

extern STRING_HANDLE STRING_new(void);
extern STRING_HANDLE STRING_clone(STRING_HANDLE handle);
extern STRING_HANDLE STRING_construct(const char* psz);
extern STRING_HANDLE STRING_construct_n(const char* psz, size_t n);
extern STRING_HANDLE STRING_new_with_memory(const char* memory);
extern STRING_HANDLE STRING_new_quoted(const char* source);
extern STRING_HANDLE STRING_new_JSON(const char* source);
extern void STRING_delete(STRING_HANDLE handle);
extern int STRING_concat(STRING_HANDLE handle, const char* s2);
extern int STRING_concat_with_STRING(STRING_HANDLE s1, STRING_HANDLE s2);
extern int STRING_quote(STRING_HANDLE handle);
extern int STRING_copy(STRING_HANDLE s1, const char* s2);
extern int STRING_copy_n(STRING_HANDLE s1, const char* s2, size_t n);
extern const char* STRING_c_str(STRING_HANDLE handle);
extern int STRING_empty(STRING_HANDLE handle);
extern size_t STRING_length(STRING_HANDLE handle);
extern int STRING_compare(STRING_HANDLE s1, STRING_HANDLE s2);


#ifdef __cplusplus
}
#else
#endif

#endif  /*STRINGS_H*/
