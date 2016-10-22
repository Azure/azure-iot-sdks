// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef REAL_CRT_ABSTRACTIONS_H
#define REAL_CRT_ABSTRACTIONS_H

#define mallocAndStrcpy_s       real_mallocAndStrcpy_s
#define unsignedIntToString     real_unsignedIntToString
#define size_tToString          real_size_tToString 

#undef CRT_ABSTRACTIONS_H
#include "crt_abstractions.h"

#ifndef COMPILING_REAL_CRT_ABSTRACTIONS_C
#undef mallocAndStrcpy_s  
#undef unsignedIntToString
#undef size_tToString     
#endif

#undef CRT_ABSTRACTIONS_H

#endif