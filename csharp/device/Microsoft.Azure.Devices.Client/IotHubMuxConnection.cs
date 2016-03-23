// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client
{
    using System;
    using System.Threading.Tasks;

#if !WINDOWS_UWP
    using System.Collections.Concurrent;
    using Microsoft.Azure.Amqp;
    using Microsoft.Azure.Amqp.Framing;
    using Microsoft.Azure.Devices.Client.Extensions;
#endif

    sealed class IotHubMuxConnection : IotHubConnection
    {
#if !WINDOWS_UWP
        readonly IotHubConnectionCache.MuxConnectionPool muxConnectionPool;
        readonly ConcurrentDictionary<AmqpObject, IotHubTokenRefresher> iotHubTokenRefreshers;
        int deviceCount;

        object ThisLock { get; set; }

        public IotHubMuxConnection(IotHubConnectionCache.MuxConnectionPool muxConnectionPool, IotHubConnectionString connectionString, AccessRights accessRights, AmqpTransportSettings amqpTransportSettings)
            : base(connectionString, accessRights, amqpTransportSettings)
        {
            this.muxConnectionPool = muxConnectionPool;
            this.FaultTolerantSession = new FaultTolerantAmqpObject<AmqpSession>(this.CreateSessionAsync, this.CloseConnection);
            this.iotHubTokenRefreshers = new ConcurrentDictionary<AmqpObject, IotHubTokenRefresher>();
            this.ThisLock = new object();
        }

        public override Task CloseAsync()
        {
            this.CancelTokenRefreshers();
            return this.FaultTolerantSession.CloseAsync();
        }

        public override void SafeClose(Exception exception)
        {
            this.CancelTokenRefreshers();
            this.FaultTolerantSession.Close();
        }

        public override void Release()
        {
            if (this.muxConnectionPool != null)
            {
                this.muxConnectionPool.DecrementDevices(this);
            }
            else
            {
                this.CloseAsync();
            }
        }

        public int IncrementNumberOfDevices()
        {
            lock (this.ThisLock)
            {
                return ++this.deviceCount;
            }
        }

        public int DecrementNumberOfDevices()
        {
            lock (this.ThisLock)
            {
                return --this.deviceCount;
            }
        }

        /**
         The input connection string can only be a device-scope connection string
        **/
        public override async Task<SendingAmqpLink> CreateSendingLinkAsync(string path, IotHubConnectionString connectionString, TimeSpan timeout)
        {
            if (connectionString.SharedAccessKeyName != null)
            {
                throw new ArgumentException("Must provide a device-scope connection string", "connectionString");
            }

            var timeoutHelper = new TimeoutHelper(timeout);
            AmqpSession session;
            if (!this.FaultTolerantSession.TryGetOpenedObject(out session))
            {
                session = await this.FaultTolerantSession.GetOrCreateAsync(timeoutHelper.RemainingTime());
            }

            var linkAddress = connectionString.BuildLinkAddress(path);

            var linkSettings = new AmqpLinkSettings()
            {
                Role = false,
                InitialDeliveryCount = 0,
                Target = new Target() { Address = linkAddress.AbsoluteUri },
                SndSettleMode = null, // SenderSettleMode.Unsettled (null as it is the default and to avoid bytes on the wire)
                RcvSettleMode = null, // (byte)ReceiverSettleMode.First (null as it is the default and to avoid bytes on the wire)
                LinkName = Guid.NewGuid().ToString("N") // Use a human readable link name to help with debugging
            };

            SetLinkSettingsCommonProperties(linkSettings, timeoutHelper.RemainingTime());

            var link = new SendingAmqpLink(linkSettings);
            link.AttachTo(session);

            string audience = connectionString.Audience + path;
            await this.OpenLinkAsync(link, connectionString, audience, timeoutHelper.RemainingTime());

            return link;
        }

        public override async Task<ReceivingAmqpLink> CreateReceivingLinkAsync(string path, IotHubConnectionString connectionString, TimeSpan timeout, uint prefetchCount)
        {
            if (connectionString.SharedAccessKeyName != null)
            {
                throw new ArgumentException("Must provide a device-scope connection string", "connectionString");
            }

            var timeoutHelper = new TimeoutHelper(timeout);

            AmqpSession session;
            if (!this.FaultTolerantSession.TryGetOpenedObject(out session))
            {
                session = await this.FaultTolerantSession.GetOrCreateAsync(timeoutHelper.RemainingTime());
            }

            var linkAddress = connectionString.BuildLinkAddress(path);

            var linkSettings = new AmqpLinkSettings()
            {
                Role = true,
                TotalLinkCredit = prefetchCount,
                AutoSendFlow = prefetchCount > 0,
                Source = new Source() { Address = linkAddress.AbsoluteUri },
                SndSettleMode = null, // SenderSettleMode.Unsettled (null as it is the default and to avoid bytes on the wire)
                RcvSettleMode = (byte)ReceiverSettleMode.Second, 
                LinkName = Guid.NewGuid().ToString("N") // Use a human readable link name to help with debuggin
            };

            SetLinkSettingsCommonProperties(linkSettings, timeoutHelper.RemainingTime());

            var link = new ReceivingAmqpLink(linkSettings);
            link.AttachTo(session);

            string audience = connectionString.Audience + path;
            await this.OpenLinkAsync(link, connectionString, audience, timeoutHelper.RemainingTime());

            return link;
        }

        protected async override Task<AmqpSession> CreateSessionAsync(TimeSpan timeout)
        {
            var timeoutHelper = new TimeoutHelper(timeout);

            // Cleanup any lingering link refresh token timers
            this.CancelTokenRefreshers();

            return await base.CreateSessionAsync(timeoutHelper.RemainingTime());
        }

        async Task OpenLinkAsync(AmqpObject link, IotHubConnectionString connectionString, string audience, TimeSpan timeout)
        {
            var timeoutHelper = new TimeoutHelper(timeout);
       
            try
            {
                // this is a device-scope connection string. We need to send a CBS token for this specific link before opening it.
                var iotHubLinkTokenRefresher = new IotHubTokenRefresher(
                    this.FaultTolerantSession.Value, 
                    connectionString, 
                    audience, 
                    this.AmqpTransportSettings.OperationTimeout,
                    this.AccessRights
                    );

                link.SafeAddClosed((s, e) =>
                {
                    if (this.iotHubTokenRefreshers.TryRemove(link, out iotHubLinkTokenRefresher))
                    {
                        iotHubLinkTokenRefresher.Cancel();
                    }
                });

                if (this.iotHubTokenRefreshers.TryAdd(link, iotHubLinkTokenRefresher))
                {
                    // Send Cbs token for new link first
                    await iotHubLinkTokenRefresher.SendCbsTokenAsync(timeoutHelper.RemainingTime());
                }

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
