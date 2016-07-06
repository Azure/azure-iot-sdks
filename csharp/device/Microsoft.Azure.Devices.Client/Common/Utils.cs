// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Reflection;
    using System.Runtime.Serialization;
    using Microsoft.Azure.Devices.Client.Common;
    using Microsoft.Azure.Devices.Client.Extensions;

    static class Utils
    {
        static readonly Dictionary<DeliveryAcknowledgement, string> AckTypeMap = new Dictionary<DeliveryAcknowledgement, string>();
        static readonly Dictionary<string, DeliveryAcknowledgement> AckTypeReverseMap = new Dictionary<string, DeliveryAcknowledgement>(StringComparer.OrdinalIgnoreCase);

        static Utils()
        {
#if PCL
            IEnumerable<FieldInfo> fields = typeof(DeliveryAcknowledgement).GetRuntimeFields().Where(field => !field.IsStatic);
#else
            FieldInfo[] fields = typeof(DeliveryAcknowledgement).GetFields(BindingFlags.Public | BindingFlags.Static);
#endif

            foreach (FieldInfo field in fields)
            {
                string memberName = field.GetCustomAttributes().OfType<EnumMemberAttribute>().Single().Value;
                AckTypeMap.Add((DeliveryAcknowledgement)field.GetValue(null), memberName);
                AckTypeReverseMap.Add(memberName, (DeliveryAcknowledgement)field.GetValue(null));
            }
        }


#if !PCL && !WINDOWS_UWP
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
        public static DeliveryAcknowledgement ConvertDeliveryAckTypeFromString(string value)
        {
            DeliveryAcknowledgement result;
            if (AckTypeReverseMap.TryGetValue(value, out result))
            {
                return result;
            }
            throw new NotSupportedException($"Unknown value: '{value}'");
        }

        public static string ConvertDeliveryAckTypeToString(DeliveryAcknowledgement value)
        {
            string result;
            if (AckTypeMap.TryGetValue(value, out result))
            {
                return result;
            }
            throw new NotSupportedException($"Unknown value: '{value}'");
        }
    }
}