// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client
{
    using System;
    using System.Collections.Concurrent;
    using System.Collections.Generic;
    using System.Linq;

#if !WINDOWS_UWP
    sealed class IotHubConnectionCache
    {
        readonly ConcurrentDictionary<IotHubConnectionString, ConnectionReferenceCounter> connections;
        readonly AccessRights accessRights;

        public IotHubConnectionCache(AccessRights accessRights)
        {
            this.connections = new ConcurrentDictionary<IotHubConnectionString, ConnectionReferenceCounter>(new IotHubConnectionCacheStringComparer());
            this.accessRights = accessRights;
        }

        public IotHubConnection GetConnection(IotHubConnectionString connectionString, AmqpTransportSettings amqpTransportSettings)
        {
            if (connectionString.SharedAccessKeyName == null & amqpTransportSettings.AmqpConnectionPoolSettings.NumConnectionPools == 0)
            {
                // Connection pooling is turned off for device-scope connection strings
                return new IotHubMuxConnection(null, connectionString, this.accessRights, amqpTransportSettings);
            }

            // Use connection caching for both hub-scope and device-scope connection strings
            if (connectionString.SharedAccessKeyName != null)
            {
                ConnectionReferenceCounter connectionReferenceCounter;
                do
                {
                    connectionReferenceCounter = this.connections.GetOrAdd(connectionString, k => new ConnectionReferenceCounter(this, k, this.accessRights, amqpTransportSettings));
                }
                while (!connectionReferenceCounter.TryAddRef());

                return connectionReferenceCounter.Connection;
            }
            else
            {
                MuxConnectionPool muxConnectionPool;
                do
                {
                    muxConnectionPool = (MuxConnectionPool)this.connections.GetOrAdd(connectionString, k => new MuxConnectionPool(this, k, this.accessRights, amqpTransportSettings));
                }
                while (!muxConnectionPool.TryAddRef());

                return muxConnectionPool.Connection;
            }
        }

        // A comparer used to see if two IotHubConnectionStrings can share a common AmqpConnection between them.
        class IotHubConnectionCacheStringComparer : IEqualityComparer<IotHubConnectionString>
        {
            public bool Equals(IotHubConnectionString connectionString1, IotHubConnectionString connectionString2)
            {
                if (connectionString1 == null && connectionString2 == null)
                {
                    return true;
                }
                else if (connectionString1 == null || connectionString2 == null)
                {
                    return false;
                }

                if (connectionString1.SharedAccessKeyName != null || connectionString2.SharedAccessKeyName != null)
                {
                    // Hub-Scope connection strings must match fully
                    return connectionString1.IotHubName == connectionString2.IotHubName &&
                        connectionString1.AmqpEndpoint == connectionString2.AmqpEndpoint &&
                        connectionString1.SharedAccessKey == connectionString2.SharedAccessKey &&
                        connectionString1.SharedAccessKeyName == connectionString2.SharedAccessKeyName &&
                        connectionString1.SharedAccessSignature == connectionString2.SharedAccessSignature;
                }
                else
                {
                    // device-scope connection strings only need to match on IotHubName and Endpoint
                    return connectionString1.IotHubName == connectionString2.IotHubName &&
                        connectionString1.AmqpEndpoint == connectionString2.AmqpEndpoint;
                }

            }

            public int GetHashCode(IotHubConnectionString connectionString)
            {
                if (connectionString == null)
                {
                    return 0;
                }

                if (connectionString.SharedAccessKeyName != null)
                {
                    return HashCode.CombineHashCodes(
                        HashCode.SafeGet(connectionString.IotHubName),
                        HashCode.SafeGet(connectionString.AmqpEndpoint),
                        HashCode.SafeGet(connectionString.SharedAccessKey),
                        HashCode.SafeGet(connectionString.SharedAccessKeyName),
                        HashCode.SafeGet(connectionString.SharedAccessSignature));
                }
                else
                {
                    return HashCode.CombineHashCodes(
                        HashCode.SafeGet(connectionString.IotHubName),
                        HashCode.SafeGet(connectionString.AmqpEndpoint));
                }
            }
        }

        internal class ConnectionReferenceCounter
        {
            readonly IotHubConnectionCache cache;
            readonly IOThreadTimer idleTimer;
            readonly TimeSpan idleTimeout;

            int referenceCount;

            protected IotHubConnectionString ConnectionString { get; }

            public ConnectionReferenceCounter(IotHubConnectionCache cache, IotHubConnectionString connectionString, AccessRights accessRights, AmqpTransportSettings amqpTransportSettings)
            {
                this.cache = cache;
                this.ConnectionString = connectionString;

                if (this.ConnectionString.SharedAccessKeyName != null)
                {
                    this.Connection = new IotHubSingleTokenConnection(this, connectionString, accessRights, amqpTransportSettings);
                }

                this.ThisLock = new object();
                this.idleTimer = new IOThreadTimer(s => ((ConnectionReferenceCounter)s).IdleTimerCallback(), this, false);
                this.idleTimeout = amqpTransportSettings.AmqpConnectionPoolSettings.CacheIdleTimeout;
                this.idleTimer.Set(this.idleTimeout);
            }

            public virtual IotHubConnection Connection { get; }

            object ThisLock { get; set; }

            public bool TryAddRef()
            {
                lock (this.ThisLock)
                {
                    if (this.referenceCount == 0)
                    {
                        if (!this.idleTimer.Cancel())
                        {
                            return false;
                        }
                    }

                    ++this.referenceCount;
                    return true;
                }
            }

            public void RemoveRef()
            {
                lock (this.ThisLock)
                {
                    if (--this.referenceCount == 0)
                    {
                        this.idleTimer.Set(this.idleTimeout);
                    }

                    Fx.Assert(this.referenceCount >= 0, "Cached IotHubConnection's ref count should never be below 0!");
                }
            }

            protected virtual void IdleTimerCallback()
            {
                Fx.Assert(this.referenceCount == 0, "Cached IotHubConnection's ref count should be zero when idle timeout occurs!");
                ConnectionReferenceCounter connectionReferenceCounter;
                this.cache.connections.TryRemove(this.ConnectionString, out connectionReferenceCounter);
                this.Connection.CloseAsync().Fork();
            }
        }

        internal class MuxConnectionPool : ConnectionReferenceCounter
        {
            readonly HashSet<IotHubMuxConnection> fullyLoadedPools;
            readonly HashSet<IotHubMuxConnection> semiLoadedPools;
            readonly HashSet<IotHubMuxConnection> lightlyLoadedPools;
            readonly AccessRights accessRights;
            readonly AmqpTransportSettings amqpTransportSettings;

            public MuxConnectionPool(IotHubConnectionCache cache, IotHubConnectionString connectionString, AccessRights accessRights, AmqpTransportSettings amqpTransportSettings)
                : base(cache, connectionString, accessRights, amqpTransportSettings)
            {
                this.accessRights = accessRights;
                this.amqpTransportSettings = amqpTransportSettings;
                this.fullyLoadedPools = new HashSet<IotHubMuxConnection>();
                this.semiLoadedPools = new HashSet<IotHubMuxConnection>();
                this.lightlyLoadedPools = new HashSet<IotHubMuxConnection>();
                this.ThisLock = new object();
            }

            public override IotHubConnection Connection
            {
                get
                {
                    return this.PickConnectionPool();
                }
            }

            public bool IncrementDevices(IotHubMuxConnection iotHubMuxConnection)
            {
                lock (this.ThisLock)
                {
                    var numDevices = iotHubMuxConnection.IncrementNumberOfDevices();

                    if (numDevices > AmqpConnectionPoolSettings.DevicesPerConnectionLevel1 &&
                        numDevices <= AmqpConnectionPoolSettings.DevicesPerConnectionLevel2)
                    {
                        if (this.lightlyLoadedPools.Contains(iotHubMuxConnection))
                        {
                            this.lightlyLoadedPools.Remove(iotHubMuxConnection);
                            this.semiLoadedPools.Add(iotHubMuxConnection);
                        }
                    }
                    else if (numDevices > AmqpConnectionPoolSettings.DevicesPerConnectionLevel2)
                    {
                        if (this.semiLoadedPools.Contains(iotHubMuxConnection))
                        {
                            this.semiLoadedPools.Remove(iotHubMuxConnection);
                            this.fullyLoadedPools.Add(iotHubMuxConnection);
                        }
                    }
                }

                return true;
            }

            public void DecrementDevices(IotHubMuxConnection iotHubMuxConnection)
            {
                lock (this.ThisLock)
                {
                    var numDevices = iotHubMuxConnection.DecrementNumberOfDevices();

                    if (numDevices <= AmqpConnectionPoolSettings.DevicesPerConnectionLevel1)
                    {
                        if (this.semiLoadedPools.Contains(iotHubMuxConnection))
                        {
                            this.semiLoadedPools.Remove(iotHubMuxConnection);
                            this.lightlyLoadedPools.Add(iotHubMuxConnection);
                        }
                    }
                    else if (numDevices > AmqpConnectionPoolSettings.DevicesPerConnectionLevel1 &&
                             numDevices <= AmqpConnectionPoolSettings.DevicesPerConnectionLevel2)
                    {
                        if (this.fullyLoadedPools.Contains(iotHubMuxConnection))
                        {
                            this.fullyLoadedPools.Remove(iotHubMuxConnection);
                            this.semiLoadedPools.Add(iotHubMuxConnection);
                        }
                    }
                }
            }

            object ThisLock { get; }

            protected override void IdleTimerCallback()
            {
                lock (this.ThisLock)
                {
                    this.fullyLoadedPools.Clear();
                    this.semiLoadedPools.Clear();
                    this.lightlyLoadedPools.Clear();
                }

                base.IdleTimerCallback();
            }

            IotHubMuxConnection PickConnectionPool()
            {
                lock (this.ThisLock)
                {
                    IotHubMuxConnection selectedPool = null;
                    if (!this.lightlyLoadedPools.Any() && this.GetNumConnectionPools() < AmqpConnectionPoolSettings.MaxNumConnectionPools)
                    {
                        var iotHubMuxConnection = new IotHubMuxConnection(this, this.ConnectionString, this.accessRights, this.amqpTransportSettings);
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
                    else if (this.fullyLoadedPools.Any())
                    {
                        selectedPool = this.fullyLoadedPools.FirstOrDefault();
                    }

                    this.IncrementDevices(selectedPool);

                    return selectedPool;
                }
            }

            int GetNumConnectionPools()
            {
                return this.lightlyLoadedPools.Count + this.semiLoadedPools.Count + this.fullyLoadedPools.Count;
            }
        }
    }
#endif
}