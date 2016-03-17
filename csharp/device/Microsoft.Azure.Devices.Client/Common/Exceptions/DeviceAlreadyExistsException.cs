// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client.Exceptions
{
    using System;
    using System.Runtime.Serialization;
    using Microsoft.Azure.Devices.Client.Extensions;

    /// <summary>
    /// The exception that is thrown when an attempt to create a device fails because it already exists.
    /// </summary>
#if !WINDOWS_UWP && !PCL
    [Serializable]
#endif
#if !WINDOWS_UWP && !PCL // Exporting custom exception types is not allowed in WinRT
    public
#endif
    sealed class DeviceAlreadyExistsException : IotHubException
    {
        /// <summary>
        /// Initializes a new instance of the <see cref="DeviceAlreadyExistsException"/> class with the message string containing the identifier of the already existing device.
        /// </summary>
        /// <param name="deviceId">Device identifier that already exists.</param>
        public DeviceAlreadyExistsException(string deviceId)
            : base("Device {0} already registered".FormatInvariant(deviceId))
        {
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="DeviceAlreadyExistsException"/> class with the message string set to the device identifier of the already existing device and a reference to the inner exception that is the cause of this exception.
        /// </summary>
        /// <param name="message">A description of the error. The content of message is intended to be understood by humans. The caller of this constructor is required to ensure that this string has been localized for the current system culture.</param>
        /// <param name="innerException">The exception that is the cause of the current exception</param>
        public DeviceAlreadyExistsException(string message, Exception innerException)
            : base(message, innerException)
        {
        }

#if !WINDOWS_UWP && !PCL
        /// <summary>
        /// Initializes a new instance of the <see cref="DeviceAlreadyExistsException"/> class with the specified serialization and context information.
        /// </summary>
        /// <param name="info">An object that holds the serialized object data about the exception being thrown.</param>
        /// <param name="context">An object that contains contextual information about the source or destination.</param>
        public DeviceAlreadyExistsException(SerializationInfo info, StreamingContext context)
    : base(info, context)
        {
        } 
#endif

    }
}
