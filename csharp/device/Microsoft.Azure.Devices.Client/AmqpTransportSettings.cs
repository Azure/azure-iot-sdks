// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client
{
    using System;

    /// <summary>
    /// contains Amqp transport-specific settings for DeviceClient
    /// </summary>
    public sealed class AmqpTransportSettings : ITransportSettings
    {
        const uint DefaultPrefetchCount = 50;
        readonly TransportType transportType;

        public AmqpTransportSettings(TransportType transportType)
            : this(transportType, DefaultPrefetchCount)
        {
        }

        public AmqpTransportSettings(TransportType transportType, uint prefetchCount)
        {
            if (prefetchCount <= 0)
            {
                throw new ArgumentOutOfRangeException("prefetchCount", "Must be greater than zero");
            }

            switch (transportType)
            {
                case TransportType.Amqp_WebSocket_Only:
                case TransportType.Amqp_Tcp_Only:
                    this.transportType = transportType;
                    break;
                case TransportType.Amqp:
                    throw new ArgumentOutOfRangeException(nameof(transportType), transportType, "Must specify Amqp_WebSocket_Only or Amqp_Tcp_Only");
                default:
                    throw new ArgumentOutOfRangeException(nameof(transportType), transportType, null);
            }

            this.PrefetchCount = prefetchCount;
        }

        public TransportType GetTransportType()
        {
            return this.transportType;
        }

        public uint PrefetchCount { get; set; }
    }
}
