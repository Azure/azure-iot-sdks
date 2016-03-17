// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client
{
    using System;

    static class Ticks
    {

#if !WINDOWS_UWP && !PCL
        public static long Now
        {
            [Fx.Tag.SecurityNote(Miscellaneous = "Why isn't the SuppressUnmanagedCodeSecurity attribute working in this case?")]
            get
            {
                long time;
#pragma warning disable 1634
#pragma warning suppress 56523 // function has no error return value
#pragma warning restore 1634
                UnsafeNativeMethods.GetSystemTimeAsFileTime(out time);
                return time;
            }
        }
#endif

        public static long FromMilliseconds(int milliseconds)
        {
            return checked((long)milliseconds * TimeSpan.TicksPerMillisecond);
        }

        public static int ToMilliseconds(long ticks)
        {
            return checked((int)(ticks / TimeSpan.TicksPerMillisecond));
        }

        public static long FromTimeSpan(TimeSpan duration)
        {
            return duration.Ticks;
        }

        public static TimeSpan ToTimeSpan(long ticks)
        {
            return new TimeSpan(ticks);
        }

        public static long Add(long firstTicks, long secondTicks)
        {
            if (firstTicks == long.MaxValue || firstTicks == long.MinValue)
            {
                return firstTicks;
            }

            if (secondTicks == long.MaxValue || secondTicks == long.MinValue)
            {
                return secondTicks;
            }

            if (firstTicks >= 0 && long.MaxValue - firstTicks <= secondTicks)
            {
                return long.MaxValue - 1;
            }

            if (firstTicks <= 0 && long.MinValue - firstTicks >= secondTicks)
            {
                return long.MinValue + 1;
            }

            return checked(firstTicks + secondTicks);
        }
    }
}
