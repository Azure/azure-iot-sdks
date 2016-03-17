// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client.Exceptions
{
    using System;
    using System.Runtime.Serialization;
    using Microsoft.Azure.Devices.Client.Extensions;

#if !WINDOWS_UWP && !PCL
    [Serializable]
#endif
#if !WINDOWS_UWP && !PCL // Exporting custom exception types is not allowed in WinRT
    public
#endif
    sealed class DeviceDisabledException : IotHubException
    {
        public DeviceDisabledException(string deviceId)
            : base("Device {0} is disabled".FormatInvariant(deviceId))
        {
        }

        public DeviceDisabledException(string deviceId, string iotHubName)
            : this(deviceId, iotHubName, string.Empty)
        {
        }

        public DeviceDisabledException(string deviceId, string iotHubName, string trackingId)
            : base(!string.IsNullOrEmpty(iotHubName) ? "Device {0} at IotHub {1} is disabled".FormatInvariant(deviceId, iotHubName) : "Device {0} is disabled".FormatInvariant(deviceId), trackingId)
        {
        }

        public DeviceDisabledException(string message, Exception innerException)
            : base(message, innerException)
        {
        }

#if !WINDOWS_UWP && !PCL
        public DeviceDisabledException(SerializationInfo info, StreamingContext context)
            : base(info, context)
        {
        }
#endif
    }
}
