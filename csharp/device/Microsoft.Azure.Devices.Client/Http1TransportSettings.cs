// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client
{
    using System;

    /// <summary>
    /// contains Http1 transport-specific settings for DeviceClient
    /// </summary>
    public sealed class Http1TransportSettings : ITransportSettings
    {
        static readonly TimeSpan DefaultOperationTimeout = TimeSpan.FromSeconds(60);

        public TransportType GetTransportType()
        {
            return TransportType.Http1;
        }

        public TimeSpan DefaultReceiveTimeout => DefaultOperationTimeout;
    }
}
