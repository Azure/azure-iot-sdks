// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client
{
    using System;
    using System.Collections.Concurrent;
    using System.Collections.Generic;

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

        public int GetNumConnections()
        {
            return this.connections.Count;
        }

        public IotHubConnection GetConnection(IotHubConnectionString connectionString, AmqpTransportSettings amqpTransportSettings)
        {
            if (connectionString.SharedAccessKeyName == null & amqpTransportSettings.AmqpConnectionPoolSettings.NumConnectionPools == 0)
            {
                // Connection pooling is turned off for device-scope connection strings
                return new IotHubMuxConnection(null, connectionString, this.accessRights, amqpTransportSettings);
            }

            // Use connection caching for both hub-scope and device-scope connection strings
            ConnectionReferenceCounter connectionReferenceCounter;
            do
            {
                connectionReferenceCounter = this.connections.GetOrAdd(connectionString, k => new ConnectionReferenceCounter(this, k, this.accessRights, amqpTransportSettings));
            }
            while (!connectionReferenceCounter.TryAddRef());

            return connectionReferenceCounter.Connection;
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
            readonly IotHubConnectionString connectionString;
            readonly IOThreadTimer idleTimer;
            readonly TimeSpan idleTimeout;
            int referenceCount;

            public ConnectionReferenceCounter(IotHubConnectionCache cache, IotHubConnectionString connectionString, AccessRights accessRights, AmqpTransportSettings amqpTransportSettings)
            {
                this.cache = cache;
                this.connectionString = connectionString;
                if (connectionString.SharedAccessKeyName != null)
                {
                    this.Connection = new IotHubSingleTokenConnection(this, connectionString, accessRights, amqpTransportSettings);
                }
                else
                {
                    this.Connection = new IotHubMuxConnection(this, connectionString, accessRights, amqpTransportSettings);
                }
                
                this.ThisLock = new object();
                this.idleTimer = new IOThreadTimer(s => ((ConnectionReferenceCounter)s).IdleTimerCallback(), this, false);
                this.idleTimeout = amqpTransportSettings.AmqpConnectionPoolSettings.CacheIdleTimeout;
                this.idleTimer.Set(this.idleTimeout);
            }

            internal IotHubConnection Connection { get; }

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

            void IdleTimerCallback()
            {
                Fx.Assert(this.referenceCount == 0, "Cached IotHubConnection's ref count should be zero when idle timeout occurs!");
                ConnectionReferenceCounter connectionReferenceCounter;
                this.cache.connections.TryRemove(this.connectionString, out connectionReferenceCounter);
                this.Connection.CloseAsync().Fork();
            }
        }
    }
#endif
}