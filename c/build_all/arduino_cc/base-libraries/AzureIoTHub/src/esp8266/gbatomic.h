// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#if defined(ARDUINO_ARCH_ESP8266)
#ifndef GBATOMIC_H
#define GBATOMIC_H

#ifdef __cplusplus
#include <cstddef>
extern "C" {
#else
#include <stddef.h>
#endif

extern unsigned int __sync_add_and_fetch_4(volatile void* pVal, unsigned int inc);
extern unsigned int __sync_sub_and_fetch_4(volatile void* pVal, unsigned int inc);

#ifdef __cplusplus
}
#endif

#endif // GBATOMIC_H

#endif // #if defined(ARDUINO_ARCH_ESP8266)
