// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices
{
    /// <summary>
    /// Endpoints exposed by IOT hub
    /// </summary>
    public enum EndpointKind
    {
        /// <summary>
        /// sends commands to device
        /// </summary>
        DeviceBound,

        /// <summary>
        /// device telemetry ingress
        /// </summary>
        Events,                 

        /// <summary>
        /// inquiries from device -> cloud
        /// </summary>
        ServiceBound,

        /// <summary>
        /// acks/nacks from system -> cloud
        /// </summary>
        Feedback,

        /// <summary>
        /// correlated responses from device -> cloud
        /// </summary>
        Response,
        
        /// <summary>
        /// Notification messages for file uploads from devices
        /// </summary>
        FileNotification           

    }
}
