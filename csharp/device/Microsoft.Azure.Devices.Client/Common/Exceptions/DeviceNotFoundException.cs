// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client.Exceptions
{
    using System;
    using System.Runtime.Serialization;
    using Microsoft.Azure.Devices.Client.Extensions;

    /// <summary>
    /// The exception that is thrown when an attempt to communicate with a device fails because the given device identifier cannot be found.
    /// </summary>
#if !WINDOWS_UWP
    [Serializable]
#endif
#if !WINDOWS_UWP // Exporting custom exception types is not allowed in WinRT
    public
#endif
    sealed class DeviceNotFoundException : IotHubException
    {
        /// <summary>
        /// Initializes a new instance of the <see cref="DeviceNotFoundException"/> class with the message string containing the device identifier that could not be found.
        /// </summary>
        /// <param name="deviceId">Device identifier that already exists.</param>
        public DeviceNotFoundException(string deviceId)
            : this(deviceId, null, null)
        {
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="DeviceNotFoundException"/> class with the message string containing the device identifier and the IoT hub instance that could not be found.
        /// </summary>
        /// <param name="deviceId">Device identifier that already exists.</param>
        /// <param name="iotHubName">Name of the IOT Hub instance.</param>
        public DeviceNotFoundException(string deviceId, string iotHubName)
            : this(deviceId, iotHubName, null)
        {
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="DeviceNotFoundException"/> class with the message string containing the device identifier that could not be found.
        /// </summary>
        /// <param name="deviceId">Device identifier that already exists.</param>
        /// <param name="iotHubName">Name of the IOT Hub instance.</param>
        /// <param name="trackingId">Tracking identifier that is used for telemetry purposes.</param>
        public DeviceNotFoundException(string deviceId, string iotHubName, string trackingId)
            : base(!string.IsNullOrEmpty(iotHubName) ? "Device {0} at IotHub {1} not registered".FormatInvariant(deviceId, iotHubName) : "Device {0} not registered".FormatInvariant(deviceId), trackingId)
        {
        }
        /// <summary>
        /// Initializes a new instance of the <see cref="DeviceNotFoundException"/> class with the message string containing the identifier of the device that could not be found and a reference to the inner exception that is the cause of this exception.
        /// </summary>
        /// <param name="message">A description of the error. The content of message is intended to be understood by humans. The caller of this constructor is required to ensure that this string has been localized for the current system culture.</param>
        /// <param name="innerException">The exception that is the cause of the current exception</param>
        public DeviceNotFoundException(string message, Exception innerException)
            : base(message, innerException)
        {
        }

#if !WINDOWS_UWP
        /// <summary>
        /// Initializes a new instance of the <see cref="DeviceNotFoundException"/> class with the specified serialization and context information.
        /// </summary>
        /// <param name="info">An object that holds the serialized object data about the exception being thrown.</param>
        /// <param name="context">An object that contains contextual information about the source or destination.</param>
        public DeviceNotFoundException(SerializationInfo info, StreamingContext context)
            : base(info, context)
        {
        }
#endif
    }
}
