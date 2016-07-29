// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client
{
    using System;
#if !NETMF
    using System.Reflection;
    using Microsoft.Azure.Devices.Client.Common;
#endif
    using Extensions;
    using System.Collections;
#if NETMF
    using Exceptions;
#endif

    static class Utils
    {

#if PCL
        static readonly System.Collections.Generic.Dictionary<DeliveryAcknowledgement, string> AckTypeMap = new System.Collections.Generic.Dictionary<DeliveryAcknowledgement, string>();
        static readonly System.Collections.Generic.Dictionary<string, DeliveryAcknowledgement> AckTypeReverseMap = new System.Collections.Generic.Dictionary<string, DeliveryAcknowledgement>();
#elif !NETMF
        static readonly Hashtable AckTypeMap = new Hashtable();
        static readonly Hashtable AckTypeReverseMap = new Hashtable();
#endif

#if !NETMF
        static Utils()
        {
            AckTypeMap.Add(DeliveryAcknowledgement.NegativeOnly, "negative");
            AckTypeMap.Add(DeliveryAcknowledgement.PositiveOnly, "positive");
            AckTypeMap.Add(DeliveryAcknowledgement.Full, "full");
            AckTypeMap.Add(DeliveryAcknowledgement.None, "none");

            foreach (DeliveryAcknowledgement key in AckTypeMap.Keys)
            {
                AckTypeReverseMap.Add(AckTypeMap[key], key);
            }
        }
#endif

#if !PCL && !WINDOWS_UWP && !NETMF
        public static void ValidateBufferBounds(byte[] buffer, int offset, int size)
        {
            if (buffer == null)
            {
                throw new ArgumentNullException(nameof(buffer));
            }

            ValidateBufferBounds(buffer.Length, offset, size);
        }
        static void ValidateBufferBounds(int bufferSize, int offset, int size)
        {
            if (offset < 0)
            {
                throw new ArgumentOutOfRangeException(nameof(offset), offset, Resources.ArgumentMustBeNonNegative);
            }

            if (offset > bufferSize)
            {
                throw new ArgumentOutOfRangeException(nameof(offset), offset, Resources.OffsetExceedsBufferSize.FormatInvariant(bufferSize));
            }

            if (size <= 0)
            {
                throw new ArgumentOutOfRangeException(nameof(size), size, Resources.ArgumentMustBePositive);
            }

            int remainingBufferSpace = bufferSize - offset;
            if (size > remainingBufferSpace)
            {
                throw new ArgumentOutOfRangeException(nameof(size), size, Resources.SizeExceedsRemainingBufferSpace.FormatInvariant(remainingBufferSpace));
            }
        }

        public static string GetClientVersion()
        {
            Assembly a = Assembly.GetExecutingAssembly();
            var attribute = (AssemblyInformationalVersionAttribute)a.GetCustomAttributes(typeof(AssemblyInformationalVersionAttribute), true)[0];
            return a.GetName().Name + "/" + attribute.InformationalVersion;
        }

#endif

#if !NETMF

        public static string ConvertDeliveryAckTypeToString(DeliveryAcknowledgement value)
        {
#if PCL
            if (AckTypeMap.ContainsKey(value))
#else
            if (AckTypeMap.Contains(value))
#endif
            {
                return (string)AckTypeMap[value];
            }

            throw new NotSupportedException("Unknown value: " + value);
        }

        public static DeliveryAcknowledgement ConvertDeliveryAckTypeFromString(string value)
        {
#if PCL
            if (AckTypeReverseMap.ContainsKey(value))
#else
            if (AckTypeReverseMap.Contains(value))
#endif
            {
                return (DeliveryAcknowledgement)AckTypeReverseMap[value];
            }

            throw new NotSupportedException("Unknown value: " + value);
        }

#else

        /* 
         * Development notes: 
         * 1) NETMF doesn't have the same reflection goodies of the full framework to browse fields and such
         * 2) replacing the AckTypeMap and AckTypeReverseMap Dictionaries with an Hastable could work but it takes too much RAM (which is at premium in NETMF)
         * 3) the best approach is to do this the 'hard' way 
        */

        public static DeliveryAcknowledgement ConvertDeliveryAckTypeFromString(string value)
        {
            switch (value)
            {
                case "none":
                    return DeliveryAcknowledgement.None;
                case "negative":
                    return DeliveryAcknowledgement.NegativeOnly;
                case "positive":
                    return DeliveryAcknowledgement.PositiveOnly;
                case "full":
                    return DeliveryAcknowledgement.Full;

                default:
                    throw new NotSupportedException("Unknown value: '" + value + "'");
            }
        }

        public static string ConvertDeliveryAckTypeToString(DeliveryAcknowledgement value)
        {
            switch (value)
            {
                case DeliveryAcknowledgement.None:
                    return "none";
                case DeliveryAcknowledgement.NegativeOnly:
                    return "negative";
                case DeliveryAcknowledgement.PositiveOnly:
                    return "positive";
                case DeliveryAcknowledgement.Full:
                    return "full";

                default:
                    throw new NotSupportedException("Unknown value: '" + value + "'");
            }
        }

#endif

    }
}
