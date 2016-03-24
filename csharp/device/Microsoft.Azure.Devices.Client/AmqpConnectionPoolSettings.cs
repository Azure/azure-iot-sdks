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
        const uint DefaultPoolSize = 1;
        internal const uint MaxNumberOfPools = ushort.MaxValue;
        internal const uint DevicesPerConnectionLevel1 = 500;
        internal const uint DevicesPerConnectionLevel2 = 999;

        uint maxPoolSize;
        TimeSpan connectionIdleTimeout;

        public AmqpConnectionPoolSettings()
        {
            this.maxPoolSize = DefaultPoolSize;
            this.Pooling = true;
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
                    throw new ArgumentOutOfRangeException("value");
                }
            }
        }

        public bool Pooling { get; set; }

        public TimeSpan ConnectionIdleTimeout
        {
            get { return this.connectionIdleTimeout; }

            set
            {
                if (value > TimeSpan.Zero)
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
