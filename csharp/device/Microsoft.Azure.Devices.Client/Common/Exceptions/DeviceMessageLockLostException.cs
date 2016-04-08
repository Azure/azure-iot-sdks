// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client.Exceptions
{
    using System;
    using System.Runtime.Serialization;
    using Microsoft.Azure.Devices.Client.Extensions;

    /// <summary>
    /// The exception that is thrown when an attempt to communicate with a device fails because the lock token was lost (if the connection is lost and regained for example). This timeout has the same effect as if the message was abandonned.
    /// </summary>
    /// <remarks>
    /// An abandoned message will be re-enqueued in the per-device queue, and the <see cref="DeviceClient"/> instance will receive it again. A rejected message will be deleted from the queue and not received again by the device.
    /// </remarks>
#if !WINDOWS_UWP && !PCL
    [Serializable]
#endif
#if !WINDOWS_UWP && !PCL // Exporting custom exception types is not allowed in WinRT
    public
#endif
    class DeviceMessageLockLostException : IotHubException
    {
        /// <summary>
        /// Initializes a new instance of the <see cref="DeviceMessageLockLostException"/> class with the message string containing the device identifier that could not be found.
        /// </summary>
        /// <param name="message">A description of the error. The content of message is intended to be understood by humans. The caller of this constructor is required to ensure that this string has been localized for the current system culture.</param>
        public DeviceMessageLockLostException(string message)
            : base(message)
        {
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="DeviceMessageLockLostException"/> class with the message string containing the device identifier that could not be found.
        /// </summary>
        /// <param name="deviceId">Device identifier.</param>
        /// <param name="messageId">Message identifier.</param>
        public DeviceMessageLockLostException(string deviceId, Guid messageId)
            : base("Message {0} lock was lost for Device {1}".FormatInvariant(messageId, deviceId))
        {
        }

#if !WINDOWS_UWP && !PCL
        /// <summary>
        /// Initializes a new instance of the <see cref="DeviceMessageLockLostException"/> class with the specified serialization and context information.
        /// </summary>
        /// <param name="info">An object that holds the serialized object data about the exception being thrown.</param>
        /// <param name="context">An object that contains contextual information about the source or destination.</param>
        DeviceMessageLockLostException(SerializationInfo info, StreamingContext context)
            : base(info, context)
        {
        }
#endif
    }
}
