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
        readonly ConcurrentDictionary<string, IotHubLinkRefreshTokenTimer> iotHubLinkRefreshTokenTimers; // There can be multiple device-scope connection strings per IotHubConnection instance
        readonly uint maxNumberOfLinks;

        public IotHubMuxConnection(IotHubConnectionCache.ConnectionReferenceCounter connectionReferenceCounter, IotHubConnectionString connectionString, AccessRights accessRights, AmqpTransportSettings amqpTransportSettings)
            : base(connectionReferenceCounter, connectionString, accessRights, amqpTransportSettings)
        {
            this.FaultTolerantSession = new FaultTolerantAmqpObject<AmqpSession>(this.CreateSessionAsync, this.CloseConnection);
            this.iotHubLinkRefreshTokenTimers = new ConcurrentDictionary<string, IotHubLinkRefreshTokenTimer>();
            this.maxNumberOfLinks = AmqpConnectionPoolSettings.MaxLinksPerConnection;
        }

        public override Task OpenAsync(TimeSpan timeout)
        {
            return this.FaultTolerantSession.GetOrCreateAsync(timeout);
        }

        public override Task CloseAsync()
        {
            this.CancelRefreshTokenTimers();
            return this.FaultTolerantSession.CloseAsync();
        }

        public override void SafeClose(Exception exception)
        {
            this.CancelRefreshTokenTimers();
            this.FaultTolerantSession.Close();
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

        public override async Task<RequestResponseAmqpLink> CreateRequestResponseLinkAsync(string path, IotHubConnectionString connectionString, TimeSpan timeout)
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
                TotalLinkCredit = 0,
                AutoSendFlow = false,
                Source = new Source() { Address = linkAddress.AbsoluteUri },
                SettleType = SettleMode.SettleOnDispose,
                LinkName = Guid.NewGuid().ToString("N") // Use a human readable link name to help with debugging
            };

            SetLinkSettingsCommonProperties(linkSettings, timeoutHelper.RemainingTime());

            var link = new RequestResponseAmqpLink(session, linkSettings);

            string audience = connectionString.Audience + path;
            await this.OpenLinkAsync(link, connectionString, audience, timeoutHelper.RemainingTime());

            return link;
        }

        protected async override Task<AmqpSession> CreateSessionAsync(TimeSpan timeout)
        {
            var timeoutHelper = new TimeoutHelper(timeout);

            // Cleanup any lingering link refresh token timers
            this.CancelRefreshTokenTimers();

            return await base.CreateSessionAsync(timeoutHelper.RemainingTime());
        }

        async Task OpenLinkAsync(AmqpObject link, IotHubConnectionString connectionString, string audience, TimeSpan timeout)
        {
            var timeoutHelper = new TimeoutHelper(timeout);

			if (this.iotHubLinkRefreshTokenTimers.Count == this.maxNumberOfLinks)
			{
			   throw new ArgumentOutOfRangeException("IotHubConnection can support a maximum of {0}".FormatInvariant(this.maxNumberOfLinks));
			}

            try
            {
                // this is a device-scope connection string. We need to send a CBS token for this specific link before opening it.
                var iotHubLinkRefreshTokenTimer = new IotHubLinkRefreshTokenTimer(this, connectionString, audience);
                link.SafeAddClosed((s, e) =>
                {
                    if (this.iotHubLinkRefreshTokenTimers.TryRemove(audience, out iotHubLinkRefreshTokenTimer))
                    {
                        iotHubLinkRefreshTokenTimer.Cancel();
                    }
                });

                if (this.iotHubLinkRefreshTokenTimers.TryAdd(audience, iotHubLinkRefreshTokenTimer))
                {
                    // Send Cbs token for new link first
                    await iotHubLinkRefreshTokenTimer.SendLinkCbsTokenAsync(timeoutHelper.RemainingTime());
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

            this.CancelRefreshTokenTimers();
        }

        void CancelRefreshTokenTimers()
        {
            foreach (var iotHubLinkRefreshTokenTimer in this.iotHubLinkRefreshTokenTimers.Values)
            {
                iotHubLinkRefreshTokenTimer.Cancel();
            }

            this.iotHubLinkRefreshTokenTimers.Clear();
        }

        class IotHubLinkRefreshTokenTimer
        {
            readonly IotHubMuxConnection connection;
            readonly IotHubConnectionString connectionString;
            readonly string audience;
            readonly IOThreadTimer refreshTokenTimer;

            public IotHubLinkRefreshTokenTimer(IotHubMuxConnection connection, IotHubConnectionString connectionString, string audience)
            {
                this.connection = connection;
                this.connectionString = connectionString;
                this.audience = audience;
                this.refreshTokenTimer = new IOThreadTimer(s => ((IotHubLinkRefreshTokenTimer)s).OnLinkRefreshTokenAsync(), this, false);
            }

            public void Cancel()
            {
                this.refreshTokenTimer.Cancel();
            }

            public async Task SendLinkCbsTokenAsync(TimeSpan timeout)
            {
                AmqpSession amqpSession = this.connection.FaultTolerantSession.Value;
                if (amqpSession != null && !amqpSession.IsClosing())
                {
                    var cbsLink = amqpSession.Connection.Extensions.Find<AmqpCbsLink>();
                    if (cbsLink != null)
                    {
                        string resource = this.connectionString.AmqpEndpoint.AbsoluteUri;
                        var expiresAtUtc = await cbsLink.SendTokenAsync(
                            this.connectionString,
                            this.connectionString.AmqpEndpoint,
                            this.audience,
                            resource,
                            AccessRightsHelper.AccessRightsToStringArray(this.connection.AccessRights),
                            timeout);
                        this.ScheduleLinkTokenRefresh(expiresAtUtc);
                    }
                    else
                    {
                        // TODO: This is an error
                    }
                }
            }

            void ScheduleLinkTokenRefresh(DateTime expiresAtUtc)
            {
                if (expiresAtUtc == DateTime.MaxValue)
                {
                    return;
                }

                TimeSpan timeFromNow = expiresAtUtc.Subtract(RefreshTokenBuffer).Subtract(DateTime.UtcNow);
                if (timeFromNow > TimeSpan.Zero)
                {
                    this.refreshTokenTimer.Set(timeFromNow);
                }
            }

            async void OnLinkRefreshTokenAsync()
            {
                try
                {
                    await this.SendLinkCbsTokenAsync(IotHubConnection.DefaultOperationTimeout);
                }
                catch (Exception exception)
                {
                    if (Fx.IsFatal(exception))
                    {
                        throw;
                    }

                    this.refreshTokenTimer.Set(RefreshTokenRetryInterval);
                }
            }
        }
#endif
    }
}
