// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client
{
    using System;
    using System.Reflection;
    using Microsoft.Azure.Devices.Client.Common;
    using Microsoft.Azure.Devices.Client.Extensions;

    public static class Utils
    {
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
                throw new ArgumentOutOfRangeException("offset", offset, Resources.ArgumentMustBeNonNegative);
            }

            if (offset > bufferSize)
            {
                throw new ArgumentOutOfRangeException("offset", offset, Resources.OffsetExceedsBufferSize.FormatInvariant(bufferSize));
            }

            if (size <= 0)
            {
                throw new ArgumentOutOfRangeException("size", size, Resources.ArgumentMustBePositive);
            }

            int remainingBufferSpace = bufferSize - offset;
            if (size > remainingBufferSpace)
            {
                throw new ArgumentOutOfRangeException("size", size, Resources.SizeExceedsRemainingBufferSpace.FormatInvariant(remainingBufferSpace));
            }
        }

        public static string GetClientVersion()
        {
            var a = Assembly.GetExecutingAssembly();
            var attribute = (AssemblyInformationalVersionAttribute)a.GetCustomAttributes(typeof(AssemblyInformationalVersionAttribute), true)[0];
            return a.GetName().Name + "/" + attribute.InformationalVersion + ";";
        }
    }
}
