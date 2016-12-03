// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client
{
    using System;
    using System.Collections.Generic;
    using Microsoft.Azure.Devices.Common.Cloud;

    sealed class IotHubDeviceScopeConnectionPool
    {
        readonly IotHubConnectionCache cache;
        readonly Dictionary<long, Tuple<IotHubDeviceMuxConnection, uint>> connectionPool;
#if WINDOWS_UWP || PCL
        readonly Dictionary<IotHubDeviceMuxConnection, IOThreadTimerSlim> idleTimers;
#else
        readonly Dictionary<IotHubDeviceMuxConnection, IOThreadTimer> idleTimers;
#endif
        readonly AmqpTransportSettings amqpTransportSettings;
        readonly object thisLock;
        readonly TimeSpan idleTimeout;
        readonly IotHubConnectionString connectionString;

        public IotHubDeviceScopeConnectionPool(IotHubConnectionCache cache, IotHubConnectionString connectionString, AmqpTransportSettings amqpTransportSettings)
        {
            this.cache = cache;
            this.connectionString = connectionString;
            this.amqpTransportSettings = amqpTransportSettings;
            this.connectionPool = new Dictionary<long, Tuple<IotHubDeviceMuxConnection, uint>>();
#if WINDOWS_UWP || PCL
            this.idleTimers = new Dictionary<IotHubDeviceMuxConnection, IOThreadTimerSlim>();
#else
            this.idleTimers = new Dictionary<IotHubDeviceMuxConnection, IOThreadTimer>();
#endif
            this.idleTimeout = amqpTransportSettings.AmqpConnectionPoolSettings.ConnectionIdleTimeout;
            this.thisLock = new object();
        }

        public IotHubConnection GetConnection(string deviceId)
        {
            lock (this.thisLock)
            {
                var cacheIndex = this.HashDevice(deviceId);
                Tuple<IotHubDeviceMuxConnection, uint> selectedConnectionTuple;
                if (!this.connectionPool.TryGetValue(cacheIndex, out selectedConnectionTuple) &&
                    this.connectionPool.Count <= this.amqpTransportSettings.AmqpConnectionPoolSettings.MaxPoolSize)
                {
                    var newConnection = new IotHubDeviceMuxConnection(this, cacheIndex, this.connectionString, this.amqpTransportSettings);
                    selectedConnectionTuple = Tuple.Create(newConnection, (uint)0);
                    this.connectionPool[cacheIndex] = selectedConnectionTuple;
                    this.StartIdleConnectionTimer(newConnection);
                }

                if (selectedConnectionTuple == null)
                {
                    throw new InvalidOperationException("device scope connection pool capacity reached. Consider increasing AmqpConnectionSettings.MaxPoolSize");
                }

                var numDevices = selectedConnectionTuple.Item2;
                if (numDevices == 0)
                {
                    if (!this.TryCancelIdleTimer(selectedConnectionTuple.Item1))
                    {
                        return null;
                    }
                }

                if (++numDevices > AmqpConnectionPoolSettings.MaxDevicesPerConnection)
                {
                    throw new InvalidOperationException("device scope connection pool capacity reached. Consider increasing AmqpConnectionSettings.MaxPoolSize");
                }

                var updatedConnectionTuple = Tuple.Create(selectedConnectionTuple.Item1, numDevices);
                this.connectionPool[cacheIndex] = updatedConnectionTuple;

                return selectedConnectionTuple.Item1;
            }
        }

        public void RemoveDeviceFromConnection(IotHubDeviceMuxConnection iotHubDeviceMuxConnection, string deviceId)
        {
            lock (this.thisLock)
            {
                var cacheIndex = this.HashDevice(deviceId);
                Tuple<IotHubDeviceMuxConnection, uint> selectedConnectionTuple;
                if (!this.connectionPool.TryGetValue(cacheIndex, out selectedConnectionTuple))
                {
                    throw new InvalidOperationException("Unable to find iotHubMuxConnection to remove device from connection");
                }

                var numDevices = selectedConnectionTuple.Item2;
                var updatedConnectionTuple = Tuple.Create(selectedConnectionTuple.Item1, --numDevices);
                this.connectionPool[cacheIndex] = updatedConnectionTuple;

                if (numDevices == 0)
                {
                    this.StartIdleConnectionTimer(iotHubDeviceMuxConnection);
                }
            }
        }

        void RemoveConnection(IotHubDeviceMuxConnection iotHubDeviceMuxConnection)
        {
            lock (this.thisLock)
            {
                this.connectionPool.Remove(iotHubDeviceMuxConnection.GetCacheKey());

                this.idleTimers.Remove(iotHubDeviceMuxConnection);

                if (this.connectionPool.Count == 0)
                {
                    this.cache.RemoveDeviceScopeConnectionPool(this.connectionString);
                }
            }
        }

        // This method is only for unit tests
        internal int GetCount()
        {
            return this.connectionPool.Count;
        }

        long HashDevice(string deviceId)
        {
            return Math.Abs(PerfectHash.HashToLong(deviceId) % this.amqpTransportSettings.AmqpConnectionPoolSettings.MaxPoolSize);
        }

        /*
        * Only called under this.thisLock
        */
        bool TryCancelIdleTimer(IotHubDeviceMuxConnection iotHubDeviceMuxConnection)
        {
#if WINDOWS_UWP || PCL
            IOThreadTimerSlim idleTimer;
#else
            IOThreadTimer idleTimer;
#endif
            if (this.idleTimers.TryGetValue(iotHubDeviceMuxConnection, out idleTimer))
            {
                if (!idleTimer.Cancel())
                {
                    return false;
                }

                this.idleTimers.Remove(iotHubDeviceMuxConnection);
            }
            else
            {
               throw new InvalidOperationException("IdleTimer could not be found");
            }

            return true;
        }

        /*
        * Only called under this.thisLock
        */
        void StartIdleConnectionTimer(IotHubDeviceMuxConnection iotHubDeviceMuxConnection)
        {
#if WINDOWS_UWP || PCL
            var idleTimer = new IOThreadTimerSlim(this.IdleConnectionTimerCallback, iotHubDeviceMuxConnection, false);
#else
            var idleTimer = new IOThreadTimer(this.IdleConnectionTimerCallback, iotHubDeviceMuxConnection, false);
#endif
            this.idleTimers.Add(iotHubDeviceMuxConnection, idleTimer);
            idleTimer.Set(this.idleTimeout);
        }

        void IdleConnectionTimerCallback(object state)
        {
            var iotHubDeviceMuxConnection = (IotHubDeviceMuxConnection)state;
            this.RemoveConnection(iotHubDeviceMuxConnection);
            iotHubDeviceMuxConnection.CloseAsync().Fork();
        }
    }
        }