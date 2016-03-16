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
        public const uint ConnectionTokenLimit = 1000;
        public const uint MaxNumConnectionPools = 100;
        const uint DefaultMaxLinksPerConnection = 256;
        const uint DefaultNumConnectionPools = 1;

        uint maxLinksPerConnection;
        uint numConnectionPools;

        public AmqpConnectionPoolSettings()
        {
            this.maxLinksPerConnection = DefaultMaxLinksPerConnection;
            this.numConnectionPools = DefaultNumConnectionPools;
        }

        public uint LinksPerConnection
        {
            get { return this.maxLinksPerConnection; }

            set { this.SetLinksPerConnection(value); }
        }

        public uint NumConnectionPools
        {
            get { return this.numConnectionPools; }

            set { this.SetLinksPerConnection(value); }
        }

        void SetLinksPerConnection(uint maxLinks)
        {
            if (maxLinks <= ConnectionTokenLimit)
            {
                this.maxLinksPerConnection = maxLinks;
            }
            else
            {
                throw new ArgumentOutOfRangeException("maxLinks");
            }
        }

        void SetNumberOfConnectionPools(uint numPools)
        {
            if (numPools <= MaxNumConnectionPools)
            {
                this.numConnectionPools = numPools;
            }
            else
            {
                throw new ArgumentOutOfRangeException("numPools");
            }
        }
    }
}
