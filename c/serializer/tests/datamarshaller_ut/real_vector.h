// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef REAL_VECTOR_H
#define REAL_VECTOR_H

#define VECTOR_create           real_VECTOR_create
#define VECTOR_destroy          real_VECTOR_destroy
#define VECTOR_push_back        real_VECTOR_push_back
#define VECTOR_erasetty         real_VECTOR_erasetty
#define VECTOR_clear            real_VECTOR_clear
#define VECTOR_element          real_VECTOR_element
#define VECTOR_front            real_VECTOR_front
#define VECTOR_back             real_VECTOR_back
#define VECTOR_find_if          real_VECTOR_find_if
#define VECTOR_size             real_VECTOR_size
#define VECTOR_erase            real_VECTOR_erase

#include "vector.h"

#ifndef COMPILING_REAL_VECTOR_C
#undef VECTOR_create   
#undef VECTOR_destroy  
#undef VECTOR_push_back
#undef VECTOR_erasetty 
#undef VECTOR_clear    
#undef VECTOR_element  
#undef VECTOR_front    
#undef VECTOR_back     
#undef VECTOR_find_if  
#undef VECTOR_size  
#undef VECTOR_erase   
#endif

#undef VECTOR_H

#endif