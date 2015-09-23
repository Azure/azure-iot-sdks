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
    sealed class DeviceAlreadyExistsException : IotHubException
    {
        public DeviceAlreadyExistsException(string deviceId)
            : base("Device {0} already registered".FormatInvariant(deviceId))
        {
        }

        public DeviceAlreadyExistsException(string message, Exception innerException)
            : base(message, innerException)
        {
        }

#if !WINDOWS_UWP
        public DeviceAlreadyExistsException(SerializationInfo info, StreamingContext context)
    : base(info, context)
        {
        } 
#endif

    }
}
