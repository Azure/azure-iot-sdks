// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client
{
    using System;
    using System.Threading;
    using System.Threading.Tasks;

#if !PCL
    using System.Collections.Concurrent;
    using Microsoft.Azure.Amqp;
    using Microsoft.Azure.Devices.Client.Extensions;
#endif

    sealed class IotHubDeviceMuxConnection : IotHubConnection
    {
#if !PCL
        readonly IotHubDeviceScopeConnectionPool deviceScopeConnectionPool;
        readonly ConcurrentDictionary<AmqpObject, IotHubTokenRefresher> iotHubTokenRefreshers;
        readonly long cacheKey;

        public IotHubDeviceMuxConnection(IotHubDeviceScopeConnectionPool deviceScopeConnectionPool, long cacheKey, IotHubConnectionString connectionString, AmqpTransportSettings amqpTransportSettings)
            : base(connectionString.HostName, connectionString.AmqpEndpoint.Port, amqpTransportSettings)
        {
            this.deviceScopeConnectionPool = deviceScopeConnectionPool;
            this.cacheKey = cacheKey;
            this.FaultTolerantSession = new FaultTolerantAmqpObject<AmqpSession>(this.CreateSessionAsync, this.CloseConnection);
            this.iotHubTokenRefreshers = new ConcurrentDictionary<AmqpObject, IotHubTokenRefresher>();
        }

        public override Task CloseAsync()
        {
            return this.FaultTolerantSession.CloseAsync();
        }

        public override void SafeClose(Exception exception)
        {
            this.FaultTolerantSession.Close();
        }

        public override void Release(string deviceId)
        {
            if (this.deviceScopeConnectionPool != null)
            {
                this.deviceScopeConnectionPool.RemoveDeviceFromConnection(this, deviceId);
            }
            else
            {
                this.CloseAsync();
            }
        }

        public long GetCacheKey()
        {
            return this.cacheKey;
        }

        protected override void OnCreateSession()
        {
            // Cleanup any lingering link refresh token timers
            this.CancelTokenRefreshers();
        }

        /**
          The input connection string can only be a device-scope connection string
         **/
        protected override void OnCreateSendingLink(IotHubConnectionString connectionString)
        {
            if (connectionString.SharedAccessKeyName != null)
            {
                throw new ArgumentException("Must provide a device-scope connection string", "connectionString");
            }
        }

        protected override void OnCreateReceivingLink(IotHubConnectionString connectionString)
        {
            if (connectionString.SharedAccessKeyName != null)
            {
                throw new ArgumentException("Must provide a device-scope connection string", "connectionString");
            }
        }

        protected override Uri BuildLinkAddress(IotHubConnectionString iotHubConnectionString, string path)
        {
            return iotHubConnectionString.BuildLinkAddress(path);
        }

        protected override string BuildAudience(IotHubConnectionString iotHubConnectionString, string path)
        {
            return iotHubConnectionString.Audience + path;
        }

        protected override async Task OpenLinkAsync(AmqpObject link, IotHubConnectionString connectionString, string audience, TimeSpan timeout, CancellationToken token)
        {
            var timeoutHelper = new TimeoutHelper(timeout);

            token.ThrowIfCancellationRequested();
            try
            {
                // this is a device-scope connection string. We need to send a CBS token for this specific link before opening it.
                var iotHubLinkTokenRefresher = new IotHubTokenRefresher(
                    this.FaultTolerantSession.Value, 
                    connectionString, 
                    audience
                    );

                if (this.iotHubTokenRefreshers.TryAdd(link, iotHubLinkTokenRefresher))
                {
                    link.SafeAddClosed((s, e) =>
                    {
                        if (this.iotHubTokenRefreshers.TryRemove(link, out iotHubLinkTokenRefresher))
                        {
                            iotHubLinkTokenRefresher.Cancel();
                        }
                    });

                    // Send Cbs token for new link first
                    await iotHubLinkTokenRefresher.SendCbsTokenAsync(timeoutHelper.RemainingTime());
                }

                token.ThrowIfCancellationRequested();
                // Open Amqp Link
                await link.OpenAsync(timeoutHelper.RemainingTime());
            }
            catch (Exception exception)
            {
                if (exception.IsFatal())
                {
                    throw;
                }

                link.SafeClose(exception);
                throw;
            }
        }

        void CloseConnection(AmqpSession amqpSession)
        {
            // Closing the connection also closes any sessions.
            amqpSession.Connection.SafeClose();

            this.CancelTokenRefreshers();
        }

        void CancelTokenRefreshers()
        {
            foreach (var iotHubLinkTokenRefresher in this.iotHubTokenRefreshers.Values)
            {
                iotHubLinkTokenRefresher.Cancel();
            }

            this.iotHubTokenRefreshers.Clear();
        }
#endif
    }
}
