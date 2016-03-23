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
        static readonly TimeSpan DefaultCacheIdleTimeout = TimeSpan.FromMinutes(2);
        const uint DefaultNumConnectionPools = 1;
        internal const uint MaxNumConnectionPools = ushort.MaxValue;
        internal const uint DevicesPerConnectionLevel1 = 500;
        internal const uint DevicesPerConnectionLevel2 = 950;

        uint numConnectionPools;
        TimeSpan cacheIdleTimeout;

        public AmqpConnectionPoolSettings()
        {
            this.numConnectionPools = DefaultNumConnectionPools;
            this.cacheIdleTimeout = DefaultCacheIdleTimeout;
        }

        public uint NumConnectionPools
        {
            get { return this.numConnectionPools; }

            set { this.SetNumberOfConnectionPools(value); }
        }

        public TimeSpan CacheIdleTimeout
        {
            get { return this.cacheIdleTimeout; }

            set { this.SetCacheIdleTimeout(value); }
        }

        void SetNumberOfConnectionPools(uint numPools)
        {
            // zero is allowed. This disables connection pooling for device-scoped connection strings
            if (numPools <= MaxNumConnectionPools)
            {
                this.numConnectionPools = numPools;
            }
            else
            {
                throw new ArgumentOutOfRangeException("numPools");
            }
        }

        void SetCacheIdleTimeout(TimeSpan cacheIdleTimeout)
        {
            if (cacheIdleTimeout > TimeSpan.Zero)
            {
                this.cacheIdleTimeout = cacheIdleTimeout;
            }
            else
            {
                throw new ArgumentOutOfRangeException("cacheIdleTimeout");
            }
        }
    }
}
