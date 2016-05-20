// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client
{
    using System;

    /// <summary>
    /// Interface used to define various transport-specific settings for DeviceClient
    /// </summary>
    public interface ITransportSettings
    {
        TransportType GetTransportType();

        TimeSpan DefaultReceiveTimeout { get; }
    }
}