// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client
{
    using System;
    using System.Collections.Generic;
    using System.Linq;

#if !WINDOWS_UWP
    sealed class IotHubDeviceScopeConnectionPool
    {
        readonly IotHubConnectionCache cache;
        readonly HashSet<IotHubMuxConnection> fullyLoadedPools;
        readonly HashSet<IotHubMuxConnection> semiLoadedPools;
        readonly HashSet<IotHubMuxConnection> lightlyLoadedPools;
        readonly AmqpTransportSettings amqpTransportSettings;
        readonly object thisLock;
        readonly IotHubConnectionString connectionString;

        public IotHubDeviceScopeConnectionPool(IotHubConnectionCache cache, IotHubConnectionString connectionString, AmqpTransportSettings amqpTransportSettings)
        {
            this.cache = cache;
            this.connectionString = connectionString;
            this.amqpTransportSettings = amqpTransportSettings;
            this.fullyLoadedPools = new HashSet<IotHubMuxConnection>();
            this.semiLoadedPools = new HashSet<IotHubMuxConnection>();
            this.lightlyLoadedPools = new HashSet<IotHubMuxConnection>();
            this.thisLock = new object();
        }

        public IotHubConnection GetConnection()
        {
            lock (this.thisLock)
            {
                IotHubMuxConnection selectedPool = null;
                if (!this.lightlyLoadedPools.Any() && this.GetNumConnectionPools() < AmqpConnectionPoolSettings.MaxNumberOfPools)
                {
                    var iotHubMuxConnection = new IotHubMuxConnection(this, this.connectionString, this.amqpTransportSettings);
                    this.lightlyLoadedPools.Add(iotHubMuxConnection);
                }

                if (this.lightlyLoadedPools.Any())
                {
                    selectedPool = this.lightlyLoadedPools.FirstOrDefault();
                }
                else if (this.semiLoadedPools.Any())
                {
                    selectedPool = this.semiLoadedPools.FirstOrDefault();
                }
                else
                {
                    throw new InvalidOperationException("Amqp device-scoped iothub connection pool capacity exhausted");
                }

                bool success = this.IncrementNumDevices(selectedPool);

                return success ? selectedPool : null;
            }
        }

        public void DecrementNumDevices(IotHubMuxConnection iotHubMuxConnection)
        {
            lock (this.thisLock)
            {
                var numDevices = iotHubMuxConnection.DecrementNumberOfDevices();
                switch (numDevices)
                {
                    case AmqpConnectionPoolSettings.DevicesPerConnectionLevel1:
                        if (this.semiLoadedPools.Contains(iotHubMuxConnection))
                        {
                            this.semiLoadedPools.Remove(iotHubMuxConnection);
                            this.lightlyLoadedPools.Add(iotHubMuxConnection);
                        }
                        break;
                    case AmqpConnectionPoolSettings.DevicesPerConnectionLevel2:
                        if (this.fullyLoadedPools.Contains(iotHubMuxConnection))
                        {
                            this.fullyLoadedPools.Remove(iotHubMuxConnection);
                            this.semiLoadedPools.Add(iotHubMuxConnection);
                        }
                        break;
                    default:
                        //do nothing
                        break;
                }
            }
        }

        public void Remove(IotHubMuxConnection iotHubMuxConnection)
        {
            lock (this.thisLock)
            {
                if (this.lightlyLoadedPools.Contains(iotHubMuxConnection))
                {
                    this.lightlyLoadedPools.Remove(iotHubMuxConnection);
                }
                else if (this.semiLoadedPools.Contains(iotHubMuxConnection))
                {
                    this.semiLoadedPools.Remove(iotHubMuxConnection);
                }
                else if (this.fullyLoadedPools.Contains(iotHubMuxConnection))
                {
                    this.fullyLoadedPools.Remove(iotHubMuxConnection);
                }

                if (this.GetNumConnectionPools() == 0)
                {
                    this.cache.RemoveDeviceScopeConnectionPool(this.connectionString);
                }
            }
        }

        /*
        * Only called under this.thisLock
        */
        bool IncrementNumDevices(IotHubMuxConnection iotHubMuxConnection)
        {
            uint numDevices;
            bool success = iotHubMuxConnection.IncrementNumberOfDevices(out numDevices);

            if (!success)
            {
                return false;
            }

            switch (numDevices)
            {
                case AmqpConnectionPoolSettings.DevicesPerConnectionLevel1:
                    if (this.lightlyLoadedPools.Contains(iotHubMuxConnection))
                    {
                        this.lightlyLoadedPools.Remove(iotHubMuxConnection);
                        this.semiLoadedPools.Add(iotHubMuxConnection);
                    }
                    break;
                case AmqpConnectionPoolSettings.DevicesPerConnectionLevel2:
                    if (this.semiLoadedPools.Contains(iotHubMuxConnection))
                    {
                        this.semiLoadedPools.Remove(iotHubMuxConnection);
                        this.fullyLoadedPools.Add(iotHubMuxConnection);
                    }
                    break;
                default:
                    // do nothing
                    break;
            }

            return true;
        }

        /*
        * Only called under this.thisLock
        */
        int GetNumConnectionPools()
        {
            return this.lightlyLoadedPools.Count + this.semiLoadedPools.Count + this.fullyLoadedPools.Count;
        }
    }
#endif
}