// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client
{
    using System;
#if !PCL
    sealed class IotHubScopeConnectionPool
    {
        readonly IotHubConnectionCache cache;
#if WINDOWS_UWP
        readonly IOThreadTimerSlim idleTimer;
#else
        readonly IOThreadTimer idleTimer;
#endif
        readonly TimeSpan idleTimeout;
        readonly object thisLock;
        int referenceCount;

        IotHubConnectionString ConnectionString { get; }

        public IotHubScopeConnectionPool(IotHubConnectionCache cache, IotHubConnectionString connectionString, AmqpTransportSettings amqpTransportSettings)
        {
            Fx.Assert(cache != null, "IotHubConnectionCache reference is null");
            this.cache = cache;
            this.ConnectionString = connectionString;
            this.Connection = new IotHubSingleTokenConnection(this, connectionString,  amqpTransportSettings);
            this.thisLock = new object();
#if WINDOWS_UWP
            this.idleTimer = new IOThreadTimerSlim(s => ((IotHubScopeConnectionPool)s).IdleTimerCallback(), this, false);
#else
            this.idleTimer = new IOThreadTimer(s => ((IotHubScopeConnectionPool)s).IdleTimerCallback(), this, false);
#endif
            this.idleTimeout = amqpTransportSettings.AmqpConnectionPoolSettings.ConnectionIdleTimeout;
            this.idleTimer.Set(this.idleTimeout);
        }

        public IotHubConnection Connection { get; }

        public bool TryAddRef()
        {
            lock (this.thisLock)
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
            lock (this.thisLock)
            {
                if (--this.referenceCount == 0)
                {
                    this.idleTimer.Set(this.idleTimeout);
                }

                Fx.Assert(this.referenceCount >= 0, "Cached IotHubConnection's ref count should never be below 0!");
            }
        }

        // This method is only for unit tests
        internal int GetCount()
        {
            return this.referenceCount;
        }

        void IdleTimerCallback()
        {
            Fx.Assert(this.referenceCount == 0, "Cached IotHubConnection's ref count should be zero when idle timeout occurs!");
            this.cache.RemoveHubScopeConnectionPool(this.ConnectionString);
            this.Connection.CloseAsync().Fork();
        }
    }
#endif
        }