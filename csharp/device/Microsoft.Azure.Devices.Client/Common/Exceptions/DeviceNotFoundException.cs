// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client.Exceptions
{
    using System;
    using System.Runtime.Serialization;
    using Microsoft.Azure.Devices.Client.Extensions;

#if !WINDOWS_UWP
    [Serializable]
#endif
#if !WINDOWS_UWP // Exporting custom exception types is not allowed in WinRT
    public
#endif
    sealed class DeviceNotFoundException : IotHubException
    {
        public DeviceNotFoundException(string deviceId)
            : this(deviceId, null, null)
        {
        }

        public DeviceNotFoundException(string deviceId, string iotHubName)
            : this(deviceId, iotHubName, null)
        {
        }

        public DeviceNotFoundException(string deviceId, string iotHubName, string trackingId)
            : base(!string.IsNullOrEmpty(iotHubName) ? "Device {0} at IotHub {1} not registered".FormatInvariant(deviceId, iotHubName) : "Device {0} not registered".FormatInvariant(deviceId), trackingId)
        {
        }

        public DeviceNotFoundException(string message, Exception innerException)
            : base(message, innerException)
        {
        }

#if !WINDOWS_UWP
        public DeviceNotFoundException(SerializationInfo info, StreamingContext context)
            : base(info, context)
        {
        }
#endif
    }
}
