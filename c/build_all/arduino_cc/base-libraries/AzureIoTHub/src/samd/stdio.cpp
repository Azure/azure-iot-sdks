// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#if defined(ARDUINO_ARCH_SAMD)
#include <stdio.h>

#include "Arduino.h"

// these are needed by the serializer, so force link
asm(".global _printf_float");
asm(".global _scanf_float");

extern "C" {
    size_t _write(int handle, const unsigned char *buf, size_t bufSize)
    {
        /* Check for the command to flush all handles */
        if (handle == -1) {
            return 0;
        }

        /* Check for stdout and stderr (only necessary if FILE descriptors are enabled.) */
        if (handle != 1 && handle != 2) {
            return -1;
        }

        size_t nChars = 0;
        for (; bufSize > 0; --bufSize, ++buf, ++nChars) {
            Serial.write(*buf);
        }
        return nChars;
    }
}
#endif