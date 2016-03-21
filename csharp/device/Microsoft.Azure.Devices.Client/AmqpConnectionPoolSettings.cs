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
        internal const uint MaxNumConnectionPools = ushort.MaxValue;
        const uint DefaultNumConnectionPools = 1;
        internal const uint MaxLinksPerConnection = 256;

        uint maxLinksPerConnection;
        uint numConnectionPools;

        public AmqpConnectionPoolSettings()
        {
            this.maxLinksPerConnection = MaxLinksPerConnection;
            this.numConnectionPools = DefaultNumConnectionPools;
        }

        public uint NumConnectionPools
        {
            get { return this.numConnectionPools; }

            set { this.SetNumberOfConnectionPools(value); }
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
