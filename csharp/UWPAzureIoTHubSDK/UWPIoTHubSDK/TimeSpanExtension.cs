/*
Copyright (c) 2015 Paolo Patierno

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

using System;

namespace IoTHubAmqp
{
    /// <summary>
    /// Extension class for TimeSpan
    /// </summary>
    public static class TimeSpanExtension
    {
        public static double TotalSeconds(this TimeSpan timeSpan)
        {
            return timeSpan.Ticks * 1E-07;
        }

        public static double TotalMilliseconds(this TimeSpan timeSpan)
        {
            double num = timeSpan.Ticks * 0.0001;
            if (num > Int64.MaxValue)
            {
                return Int64.MaxValue;
            }
            if (num < Int64.MinValue)
            {
                return Int64.MinValue;
            }
            return num;
        }

        public static TimeSpan FromMilliseconds(double value)
        {
            return new TimeSpan((long)(value * 1E+04));
        }
    }
}
