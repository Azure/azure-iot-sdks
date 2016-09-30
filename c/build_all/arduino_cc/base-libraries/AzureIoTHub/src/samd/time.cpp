// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#if defined(ARDUINO_ARCH_SAMD)
#include <time.h>
#include <sys/time.h>

#include <RTCZero.h>

RTCZero rtc;

extern "C" {
    int _gettimeofday(struct timeval* tp, void* /*tzvp*/)
    {
        tp->tv_sec = rtc.getEpoch();
        tp->tv_usec = 0;

        return 0;
    }

    int settimeofday(const struct timeval* tp, const struct timezone* /*tzp*/)
    {
        rtc.begin();
        rtc.setEpoch(tp->tv_sec);

        return 0;
    }
}
#endif