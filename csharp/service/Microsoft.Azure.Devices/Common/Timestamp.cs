// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Common
{
    using System;
    using System.Diagnostics;

    // This is meant to replace DateTime when it is primarily used to determine elapsed time. DateTime is vulnerable to clock jump when
    // system wall clock is reset. Stopwatch can be used in similar scenario but it is not optimized for memory foot-print.
    //
    // This class is immune to clock jump with the following two exceptions:
    //  - When multi-processor machine has a bug in BIOS/HAL that returns inconsistent clock tick for different processor.
    //  - When the machine does not support high frequency CPU tick.
    struct Timestamp : IComparable<Timestamp>, IEquatable<Timestamp>
    {
        static readonly double TickFrequency = 10000000.0 / Stopwatch.Frequency;
        readonly long timestamp;

        public Timestamp(long timestamp)
        {
            this.timestamp = timestamp;
        }

        public static Timestamp Now
        {
            get { return new Timestamp(Stopwatch.GetTimestamp()); }
        }

        public TimeSpan Elapsed
        {
            get { return new TimeSpan(this.GetElapsedDateTimeTicks()); }
        }

        public long ElapsedTicks
        {
            get
            {
                return this.GetElapsedDateTimeTicks();
            }
        }

        static long ConvertRawTicksToTicks(long rawTicks)
        {
            if (Stopwatch.IsHighResolution)
            {
                double elapsedTicks = rawTicks * TickFrequency;
                return (long)elapsedTicks;
            }

            return rawTicks;
        }

        long GetRawElapsedTicks()
        {
            return Stopwatch.GetTimestamp() - this.timestamp;
        }

        long GetElapsedDateTimeTicks()
        {
            long rawElapsedTicks = this.GetRawElapsedTicks();
            return ConvertRawTicksToTicks(rawElapsedTicks);
        }

        public int CompareTo(Timestamp other)
        {
            return this.timestamp.CompareTo(other.timestamp);
        }

        public bool Equals(Timestamp other)
        {
            return this.timestamp == other.timestamp;
        }

        public override int GetHashCode()
        {
            return this.timestamp.GetHashCode();
        }

        public override bool Equals(object obj)
        {
            if (obj is Timestamp)
            {
                return this.Equals((Timestamp)obj);
            }

            return false;
        }

        public static bool operator ==(Timestamp t1, Timestamp t2)
        {
            return t1.timestamp == t2.timestamp;
        }

        public static bool operator !=(Timestamp t1, Timestamp t2)
        {
            return t1.timestamp != t2.timestamp;
        }

        public static bool operator >(Timestamp t1, Timestamp t2)
        {
            return t1.timestamp > t2.timestamp;
        }

        public static bool operator <(Timestamp t1, Timestamp t2)
        {
            return t1.timestamp < t2.timestamp;
        }

        public static bool operator >=(Timestamp t1, Timestamp t2)
        {
            return t1.timestamp >= t2.timestamp;
        }

        public static bool operator <=(Timestamp t1, Timestamp t2)
        {
            return t1.timestamp <= t2.timestamp;
        }

        public static Timestamp operator +(Timestamp t, TimeSpan duration)
        {
            long timestamp = (long)(t.timestamp + duration.Ticks / TickFrequency);
            return new Timestamp(timestamp);
        }

        public static Timestamp operator -(Timestamp t, TimeSpan duration)
        {
            long timestamp = (long)(t.timestamp - duration.Ticks / TickFrequency);
            return new Timestamp(timestamp);
        }

        public static TimeSpan operator -(Timestamp t1, Timestamp t2)
        {
            long rawTicks = t1.timestamp - t2.timestamp;
            long ticks = ConvertRawTicksToTicks(rawTicks);
            return new TimeSpan(ticks);
        }
    }
}
