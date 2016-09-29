// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

/** @file azcpgmspace.h
*	@brief Function prototypes for pgmspace functions that need extern-ing to C
*
*	@details These fucntions are just wrappers around existing ones in pgmspace.h that
*    are not defined in a way to make them linkable from c libs.
*/
#if defined(ARDUINO_ARCH_ESP8266)
#ifndef AZCPGMSPACE_H
#define AZCPGMSPACE_H

#include <pgmspace.h>
#include "azure_c_shared_utility\crt_abstractions.h"


#ifdef __cplusplus
#include <cstddef>
extern "C" {
#else
#include <stddef.h>
#endif
extern char* az_c_strncpy_P(char* dest, PGM_P src, size_t size);
extern size_t az_c_strlen_P(PGM_P s);
extern unsigned long long strtoull(const char* nptr, char** endPtr, int base);
extern float strtof(const char* nptr, char** endPtr);
extern long double strtold(const char* nptr, char** endPtr);
#ifdef __cplusplus
}
#endif

#endif // _AZCPGMSPACE_H

#endif // #if defined(ARDUINO_ARCH_ESP8266)
