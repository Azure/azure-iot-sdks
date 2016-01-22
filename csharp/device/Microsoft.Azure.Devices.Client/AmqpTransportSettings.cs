// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client
{
    using System;

    public sealed class AmqpTransportSettings : ITransportSettings
    {
        const uint DefaultPrefetchCount = 50;
        readonly TransportType transportType;

        public AmqpTransportSettings(TransportType transportType)
        {
            switch (transportType)
            {
                case TransportType.Amqp:
                    this.UseWebSocketOnly = false;
                    this.UseTcpOnly = false;
                    break;
                case TransportType.Amqp_WebSocket:
                    this.UseWebSocketOnly = true;
                    this.UseTcpOnly = false;
                    break;
                case TransportType.Amqp_Tcp:
                    this.UseWebSocketOnly = false;
                    this.UseTcpOnly = true;
                    break;
                default:
                    throw new ArgumentOutOfRangeException(nameof(transportType), transportType, null);
            }

            this.transportType = transportType;
            this.PrefetchCount = DefaultPrefetchCount;
        }

        public TransportType GetTransportType()
        {
            return this.transportType;
        }

        public bool UseWebSocketOnly { get; }

        public bool UseTcpOnly { get; }

        public uint PrefetchCount { get; set; }
    }
}
