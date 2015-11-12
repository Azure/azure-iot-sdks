// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Common.Exceptions
{
    using System;
    using System.Runtime.Serialization;

    [Serializable]
    public class DeviceMessageLockLostException : IotHubException
    {
        public DeviceMessageLockLostException(string message)
            : base(message)
        {
        }

        public DeviceMessageLockLostException(string deviceId, Guid messageId)
            : this(deviceId, messageId, null)
        {
        }

        public DeviceMessageLockLostException(string deviceId, Guid messageId, string trackingId)
            : base("Message {0} lock was lost for Device {1}".FormatInvariant(messageId, deviceId), trackingId)
        {
        }

        DeviceMessageLockLostException(SerializationInfo info, StreamingContext context)
            : base(info, context)
        {
        }
    }
}
