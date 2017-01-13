// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Common
{
    using System;
    using System.Diagnostics;
    using System.IO;
    using System.Reflection;
    using System.Threading;
    using Newtonsoft.Json;
    using Newtonsoft.Json.Linq;

    static class Utils
    {
        public static bool IsValidBase64(string input, out int lengthInBytes)
        {
            lengthInBytes = 0;
            try
            {
                lengthInBytes = Convert.FromBase64String(input).Length;
                return true;
            }
            catch (FormatException)
            {
                return false;
            }
        }

        public static void ValidateBufferBounds(byte[] buffer, int offset, int size)
        {
            if (buffer == null)
            {
                throw new ArgumentNullException("buffer");
            }

            ValidateBufferBounds(buffer.Length, offset, size);
        }

        static void ValidateBufferBounds(int bufferSize, int offset, int size)
        {
            if (offset < 0)
            {
                throw new ArgumentOutOfRangeException("offset", offset, ApiResources.ArgumentMustBeNonNegative);
            }

            if (offset > bufferSize)
            {
                throw new ArgumentOutOfRangeException("offset", offset, ApiResources.OffsetExceedsBufferSize.FormatInvariant(bufferSize));
            }

            if (size <= 0)
            {
                throw new ArgumentOutOfRangeException("size", size, ApiResources.ArgumentMustBePositive);
            }

            int remainingBufferSpace = bufferSize - offset;
            if (size > remainingBufferSpace)
            {
                throw new ArgumentOutOfRangeException("size", size, ApiResources.SizeExceedsRemainingBufferSpace.FormatInvariant(remainingBufferSpace));
            }
        }

        public static string GetClientVersion()
        {
#if WINDOWS_UWP
            // System.Reflection.Assembly.GetExecutingAssembly() does not exist for UWP, therefore use a hard-coded version name
            // (This string is picked up by the bump_version script, so don't change the line below)
            var UWPAssemblyVersion = "1.0.15";
            return UWPAssemblyVersion;
#else
            var a = Assembly.GetExecutingAssembly();
            var attribute = (AssemblyInformationalVersionAttribute)a.GetCustomAttributes(typeof(AssemblyInformationalVersionAttribute), true)[0];
            return a.GetName().Name + "/" + attribute.InformationalVersion;
#endif
        }
    }
}
