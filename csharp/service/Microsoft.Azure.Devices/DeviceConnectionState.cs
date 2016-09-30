// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices
{
    /// <summary>
    /// Specifies the different connection states of a device.
    /// </summary>
    public enum DeviceConnectionState
    {
        /// <summary>
        /// Represents a <see cref="Device"/> in the Disconnected state
        /// </summary>
        Disconnected = 0,

        /// <summary>
        /// Represents a <see cref="Device"/> in the Connected state
        /// </summary>
        Connected = 1
    }
}