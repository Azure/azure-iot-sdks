// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#if defined(ARDUINO_ARCH_ESP8266)
#include "azcpgmspace.h"
#include <cstddef>
 
char* az_c_strncpy_P(char* dest, PGM_P src, size_t size) {
    return strncpy_P(dest, src, size);
}

size_t az_c_strlen_P(PGM_P s) {
    return strlen_P(s);
}

const char *__ctype_ptr__ = NULL;

unsigned long long strtoull(const char* nptr, char** endPtr, int base)
{
    return strtoull_s(nptr, endPtr, base);
}

float strtof(const char* nptr, char** endPtr)
{
    return strtof_s(nptr, endPtr);
}

long double strtold(const char* nptr, char** endPtr)
{
    return strtold_s(nptr, endPtr);
}

#endif
