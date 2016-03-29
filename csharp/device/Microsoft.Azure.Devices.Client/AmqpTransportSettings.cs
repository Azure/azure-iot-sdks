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
        static readonly TimeSpan DefaultOperationTimeout = TimeSpan.FromMinutes(1);
        static readonly TimeSpan DefaultOpenTimeout = TimeSpan.FromMinutes(1);
        const uint DefaultPrefetchCount = 50;

        readonly TransportType transportType;
        TimeSpan operationTimeout;
        TimeSpan openTimeout;

        public AmqpTransportSettings(TransportType transportType)
            : this(transportType, DefaultPrefetchCount, new AmqpConnectionPoolSettings())
        {
            this.operationTimeout = DefaultOperationTimeout;
            this.openTimeout = DefaultOpenTimeout;
        }

        public AmqpTransportSettings(TransportType transportType, uint prefetchCount)
            :this(transportType, prefetchCount, new AmqpConnectionPoolSettings())
        {
        }

        public AmqpTransportSettings(TransportType transportType, uint prefetchCount, AmqpConnectionPoolSettings amqpConnectionPoolSettings)
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
            this.AmqpConnectionPoolSettings = amqpConnectionPoolSettings;
        }

        public TransportType GetTransportType()
        {
            return this.transportType;
        }

        public TimeSpan OperationTimeout {
            get { return this.operationTimeout; }
            set { this.SetOperationTimeout(value); }
        }

        public TimeSpan OpenTimeout
        {
            get { return this.openTimeout; }
            set { this.SetOpenTimeout(value); }
        }

        public uint PrefetchCount { get; set; }

        public AmqpConnectionPoolSettings AmqpConnectionPoolSettings { get; set; }

        void SetOperationTimeout(TimeSpan operationTimeout)
        {
            if (operationTimeout > TimeSpan.Zero)
            {
                this.operationTimeout = operationTimeout;
            }
            else
            {
                throw new ArgumentOutOfRangeException("operationTimeout");
            }
        }

        void SetOpenTimeout(TimeSpan openTimeout)
        {
            if (openTimeout > TimeSpan.Zero)
            {
                this.openTimeout = openTimeout;
            }
            else
            {
                throw new ArgumentOutOfRangeException("openTimeout");
            }
        }
    }
}
