#if defined(ARDUINO_ARCH_ESP8266)

#include <stddef.h>
#include <stdarg.h>
#include <stdio.h>

#include "ets_sys.h"
#include "os_type.h"
#include "osapi.h"
#include "mem.h"

int fprintf(FILE* file, const char* format, ...) {
    // if(file != stderr || file != stdout) {
    //     return -1;
    // }
    int ret;
    va_list arglist;
    va_start(arglist, format);
    ret = ets_vprintf(ets_putc, format, arglist);
    va_end(arglist);
    return ret;
}

double difftime (time_t time1, time_t time0)
{
    double t1 = time1, t0 = time0;
    return t1 - t0;
}

// The below code was taken from newlib source and altered
// to remove support for options and build environments we
// do not plan to support.
// Support removed: ALLOW_NEGATIVE_CTYPE_INDEX, _MB_CAPABLE, _MB_EXTENDED_CHARSETS_ISO
//                  _MB_EXTENDED_CHARSETS_WINDOWS
// Environments not supported: Cygwin

/*
 * Copyright (c) 1989 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <ctype.h>

#define _CTYPE_DATA_0_127 \
    _C,    _C,    _C,    _C,    _C,    _C,    _C,    _C, \
    _C,    _C|_S, _C|_S, _C|_S,    _C|_S,    _C|_S,    _C,    _C, \
    _C,    _C,    _C,    _C,    _C,    _C,    _C,    _C, \
    _C,    _C,    _C,    _C,    _C,    _C,    _C,    _C, \
    _S|_B,    _P,    _P,    _P,    _P,    _P,    _P,    _P, \
    _P,    _P,    _P,    _P,    _P,    _P,    _P,    _P, \
    _N,    _N,    _N,    _N,    _N,    _N,    _N,    _N, \
    _N,    _N,    _P,    _P,    _P,    _P,    _P,    _P, \
    _P,    _U|_X,    _U|_X,    _U|_X,    _U|_X,    _U|_X,    _U|_X,    _U, \
    _U,    _U,    _U,    _U,    _U,    _U,    _U,    _U, \
    _U,    _U,    _U,    _U,    _U,    _U,    _U,    _U, \
    _U,    _U,    _U,    _P,    _P,    _P,    _P,    _P, \
    _P,    _L|_X,    _L|_X,    _L|_X,    _L|_X,    _L|_X,    _L|_X,    _L, \
    _L,    _L,    _L,    _L,    _L,    _L,    _L,    _L, \
    _L,    _L,    _L,    _L,    _L,    _L,    _L,    _L, \
    _L,    _L,    _L,    _P,    _P,    _P,    _P,    _C

#define _CTYPE_DATA_128_255 \
    0,    0,    0,    0,    0,    0,    0,    0, \
    0,    0,    0,    0,    0,    0,    0,    0, \
    0,    0,    0,    0,    0,    0,    0,    0, \
    0,    0,    0,    0,    0,    0,    0,    0, \
    0,    0,    0,    0,    0,    0,    0,    0, \
    0,    0,    0,    0,    0,    0,    0,    0, \
    0,    0,    0,    0,    0,    0,    0,    0, \
    0,    0,    0,    0,    0,    0,    0,    0, \
    0,    0,    0,    0,    0,    0,    0,    0, \
    0,    0,    0,    0,    0,    0,    0,    0, \
    0,    0,    0,    0,    0,    0,    0,    0, \
    0,    0,    0,    0,    0,    0,    0,    0, \
    0,    0,    0,    0,    0,    0,    0,    0, \
    0,    0,    0,    0,    0,    0,    0,    0, \
    0,    0,    0,    0,    0,    0,    0,    0, \
    0,    0,    0,    0,    0,    0,    0,    0

_CONST char _ctype_[1 + 256] = {
    0,
    _CTYPE_DATA_0_127,
    _CTYPE_DATA_128_255
};

_CONST char *__ctype_ptr__ = (char *) _ctype_;

#endif // (ARDUINO_ARCH_ESP8266)