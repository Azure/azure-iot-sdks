// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client
{
    using System;
    using System.Collections.Concurrent;
    using System.Collections.Generic;
    using System.Threading.Tasks;

#if !WINDOWS_UWP
    sealed class IotHubConnectionCache
    {
        static readonly TimeSpan DefaultIdleTimeout = TimeSpan.FromMinutes(2);
        readonly ConcurrentDictionary<IotHubConnectionString, CachedConnection> connections;
        readonly AccessRights accessRights;

        public IotHubConnectionCache(AccessRights accessRights)
        {
            this.IdleTimeout = DefaultIdleTimeout;
            this.connections = new ConcurrentDictionary<IotHubConnectionString, CachedConnection>(new IotHubConnectionCacheStringComparer());
            this.accessRights = accessRights;
        }

        public TimeSpan IdleTimeout { get; set; }

        public IotHubConnection GetConnection(IotHubConnectionString connectionString, AmqpTransportSettings amqpTransportSettings)
        {
            // Use connection caching for both hub-scope and device-scope connection strings
            CachedConnection cachedConnection;
            do
            {
                cachedConnection = this.connections.GetOrAdd(connectionString, k => new CachedConnection(this, k, this.accessRights, amqpTransportSettings));
            }
            while (!cachedConnection.TryAddRef());

            return cachedConnection.Connection;
        }

        public Task ReleaseConnectionAsync(IotHubConnection connection)
        {
            connection.Release();
            return TaskHelpers.CompletedTask;
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

        internal class CachedConnection
        {
            readonly IotHubConnectionCache cache;
            readonly IotHubConnectionString connectionString;
            readonly IOThreadTimer idleTimer;
            int referenceCount;

            public CachedConnection(IotHubConnectionCache cache, IotHubConnectionString connectionString, AccessRights accessRights, AmqpTransportSettings amqpTransportSettings)
            {
                this.cache = cache;
                this.connectionString = connectionString;
                if (connectionString.SharedAccessKeyName != null)
                {
                    this.Connection = new IotHubDedicatedConnection(this, connectionString, accessRights, amqpTransportSettings);
                }
                else
                {
                    this.Connection = new IotHubMuxConnection(this, connectionString, accessRights, amqpTransportSettings);
                }
                
                this.ThisLock = new object();
                this.idleTimer = new IOThreadTimer(s => ((CachedConnection)s).IdleTimerCallback(), this, false);
                this.idleTimer.Set(this.cache.IdleTimeout);
            }

            public IotHubConnection Connection { get; private set; }

            object ThisLock { get; set; }

            public bool TryAddRef()
            {
                lock (this.ThisLock)
                {
                    if (this.referenceCount == 0)
                    {
                        bool stillAlive = this.idleTimer.Cancel();
                        if (stillAlive)
                        {
                            this.referenceCount++;
                        }

                        return stillAlive;
                    }
                    else
                    {
                        this.referenceCount++;
                        return true;
                    }
                }
            }

            public void Release()
            {
                lock (this.ThisLock)
                {
                    if (--this.referenceCount == 0)
                    {
                        this.idleTimer.Set(this.cache.IdleTimeout);
                    }

                    Fx.Assert(this.referenceCount >= 0, "Cached IotHubConnection's ref count should never be below 0!");
                }
            }

            void IdleTimerCallback()
            {
                Fx.Assert(this.referenceCount == 0, "Cached IotHubConnection's ref count should be zero when idle timeout occurs!");
                CachedConnection cachedConnection;
                this.cache.connections.TryRemove(this.connectionString, out cachedConnection);
                this.Connection.CloseAsync().Fork();
            }
        }
    }
#endif
}