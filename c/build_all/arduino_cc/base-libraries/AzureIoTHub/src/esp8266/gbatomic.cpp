// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#if defined(ARDUINO_ARCH_ESP8266)

#include "gbatomic.h"
#include <Arduino.h>
#include <cstddef>
 
#define saveInterrupt(interruptsState_addr) __asm__ __volatile__("rsil %0,15" : "=a" (interruptsState_addr))
#define restoreInterrupt(interruptsState) __asm__ __volatile__("wsr %0,ps; isync" :: "a" (interruptsState) : "memory")

static unsigned int xadd_4(volatile void* pVal, unsigned int value)
{
    unsigned int* pValInt = (unsigned int*)pVal;
    uint32_t savedInterrupts;
    uint32_t* savedInterruptsAddress = &savedInterrupts;

    saveInterrupt(savedInterruptsAddress);
    __asm__ __volatile__("addi %0, %1; isync"
        : "+r" (value), "+m" (pValInt)
        :
        : "memory");
    restoreInterrupt(savedInterrupts);

    return value;
}

unsigned int __sync_add_and_fetch_4(volatile void* pVal, unsigned int inc)
{
    return (xadd_4(pVal, inc) + inc);
}

unsigned int __sync_sub_and_fetch_4(volatile void* pVal, unsigned int inc)
{
    return (xadd_4(pVal, -inc) - inc);
}

#endif // #if defined(ARDUINO_ARCH_ESP8266)
