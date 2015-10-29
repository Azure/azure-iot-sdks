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
    class DeviceMessageLockLostException : IotHubException
    {
        public DeviceMessageLockLostException(string message)
            : base(message)
        {
        }

        public DeviceMessageLockLostException(string deviceId, Guid messageId)
            : base("Message {0} lock was lost for Device {1}".FormatInvariant(messageId, deviceId))
        {
        }

#if !WINDOWS_UWP
        DeviceMessageLockLostException(SerializationInfo info, StreamingContext context)
            : base(info, context)
        {
        }
#endif
    }
}
