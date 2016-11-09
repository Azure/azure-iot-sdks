// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#define STRING_construct real_STRING_construct
#define STRING_delete real_STRING_delete
#define STRING_c_str real_STRING_c_str
#define STRING_new real_STRING_new
#define STRING_clone real_STRING_clone
#define STRING_construct_n real_STRING_construct_n
#define STRING_new_with_memory real_STRING_new_with_memory
#define STRING_new_quoted real_STRING_new_quoted
#define STRING_new_JSON real_STRING_new_JSON
#define STRING_from_byte_array real_STRING_from_byte_array
#define STRING_concat real_STRING_concat
#define STRING_quote real_STRING_quote
#define STRING_copy real_STRING_copy
#define STRING_copy_n real_STRING_copy_n
#define STRING_empty real_STRING_empty
#define STRING_concat_with_STRING real_STRING_concat_with_STRING
#define STRING_compare real_STRING_compare
#define STRING_length real_STRING_length

#define GBALLOC_H

#include "strings.c"
