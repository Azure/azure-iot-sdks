// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client
{
    using System;

    /// <summary>
    /// contains Amqp Connection Pool settings for DeviceClient
    /// </summary>

    public sealed class AmqpConnectionPoolSettings
    {
        static readonly TimeSpan DefaultConnectionIdleTimeout = TimeSpan.FromMinutes(2);
        static readonly TimeSpan MinConnectionIdleTimeout = TimeSpan.FromSeconds(5);
        const uint DefaultPoolSize = 100; // This will allow upto 100000 devices
        const uint MaxNumberOfPools = ushort.MaxValue;
        internal const uint MaxDevicesPerConnection = 995; // IotHub allows upto 1000 tokens per connection. Setting the threshold just below that.

        uint maxPoolSize;
        TimeSpan connectionIdleTimeout;

        public AmqpConnectionPoolSettings()
        {
            this.maxPoolSize = DefaultPoolSize;
            this.Pooling = false;
            this.connectionIdleTimeout = DefaultConnectionIdleTimeout;
        }

        public uint MaxPoolSize
        {
            get { return this.maxPoolSize; }

            set
            {
                if (value > 0 && value <= MaxNumberOfPools)
                {
                    this.maxPoolSize = value;
                }
                else
                {
                    throw new ArgumentOutOfRangeException(nameof(value));
                }
            }
        }

        public bool Pooling { get; set; }

        public TimeSpan ConnectionIdleTimeout
        {
            get { return this.connectionIdleTimeout; }

            set
            {
                if (value >= MinConnectionIdleTimeout)
                {
                    this.connectionIdleTimeout = value;
                }
                else
                {
                    throw new ArgumentOutOfRangeException("value");
                }
            }
        }
    }
}
