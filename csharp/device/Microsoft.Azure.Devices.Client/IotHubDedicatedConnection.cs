namespace Microsoft.Azure.Devices.Client
{
    using System;
    using System.Threading.Tasks;
    using Microsoft.Azure.Amqp;
    using Microsoft.Azure.Amqp.Framing;
    using Microsoft.Azure.Devices.Client.Extensions;

    sealed class IotHubDedicatedConnection : IotHubConnection
    {
        readonly IOThreadTimer refreshTokenTimer;

        public IotHubDedicatedConnection(IotHubConnectionCache.CachedConnection cachedConnection, IotHubConnectionString connectionString, AccessRights accessRights, AmqpTransportSettings amqpTransportSettings)
            :base(cachedConnection, connectionString, accessRights, amqpTransportSettings)
        {
            this.faultTolerantSession = new FaultTolerantAmqpObject<AmqpSession>(this.CreateSessionAsync, this.CloseConnection);
            this.refreshTokenTimer = new IOThreadTimer(s => ((IotHubDedicatedConnection)s).OnRefreshTokenAsync(), this, false);
        }

        public override Task OpenAsync(TimeSpan timeout)
        {
            return this.faultTolerantSession.GetOrCreateAsync(timeout);
        }

        public override Task CloseAsync()
        {
            return this.faultTolerantSession.CloseAsync();
        }

        public override void SafeClose(Exception exception)
        {
            this.faultTolerantSession.Close();
        }

        public override async Task<SendingAmqpLink> CreateSendingLinkAsync(string path, IotHubConnectionString connectionString, TimeSpan timeout)
        {
            // parameter connectionString is not used
            var timeoutHelper = new TimeoutHelper(timeout);

            AmqpSession session;
            if (!this.faultTolerantSession.TryGetOpenedObject(out session))
            {
                session = await this.faultTolerantSession.GetOrCreateAsync(timeoutHelper.RemainingTime());
            }

            var linkAddress = this.connectionString.BuildLinkAddress(path);

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

            await OpenLinkAsync(link, timeoutHelper.RemainingTime());

            return link;
        }

        public override async Task<ReceivingAmqpLink> CreateReceivingLinkAsync(string path, IotHubConnectionString connectionString, TimeSpan timeout, uint prefetchCount)
        {
            // parameter connectionString is not used
            var timeoutHelper = new TimeoutHelper(timeout);

            AmqpSession session;
            if (!this.faultTolerantSession.TryGetOpenedObject(out session))
            {
                session = await this.faultTolerantSession.GetOrCreateAsync(timeoutHelper.RemainingTime());
            }

            var linkAddress = this.connectionString.BuildLinkAddress(path);

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

            await OpenLinkAsync(link, timeoutHelper.RemainingTime());

            return link;
        }

        public override async Task<RequestResponseAmqpLink> CreateRequestResponseLinkAsync(string path, IotHubConnectionString connectionString, TimeSpan timeout)
        {
            // parameter connectionString is not used
            var timeoutHelper = new TimeoutHelper(timeout);

            AmqpSession session;
            if (!this.faultTolerantSession.TryGetOpenedObject(out session))
            {
                session = await this.faultTolerantSession.GetOrCreateAsync(timeoutHelper.RemainingTime());
            }

            var linkAddress = this.connectionString.BuildLinkAddress(path);

            var linkSettings = new AmqpLinkSettings()
            {
                TotalLinkCredit = 0,
                AutoSendFlow = false,
                Source = new Source() { Address = linkAddress.AbsoluteUri },
                SettleType = SettleMode.SettleOnDispose,
                LinkName = Guid.NewGuid().ToString("N") // Use a human readable link name to help with debuggin
            };

            SetLinkSettingsCommonProperties(linkSettings, timeoutHelper.RemainingTime());

            var link = new RequestResponseAmqpLink(session, linkSettings);
            await OpenLinkAsync(link, timeoutHelper.RemainingTime());

            return link;
        }

        protected override async Task<AmqpSession> CreateSessionAsync(TimeSpan timeout)
        {
            this.refreshTokenTimer.Cancel();
            var timeoutHelper = new TimeoutHelper(timeout);

            AmqpSession amqpSession = await base.CreateSessionAsync(timeoutHelper.RemainingTime());

            var cbsLink = amqpSession.Connection.Extensions.Find<AmqpCbsLink>();
            if (cbsLink != null)
            {
                await this.SendCbsTokenAsync(cbsLink, timeoutHelper.RemainingTime());
            }
            else
            {
                // TODO: This is an error
            }

            return amqpSession;
        }

        void CloseConnection(AmqpSession amqpSession)
        {
            // Closing the connection also closes any sessions.
            amqpSession.Connection.SafeClose();
        }

        static async Task OpenLinkAsync(AmqpObject link, TimeSpan timeout)
        {
            var timeoutHelper = new TimeoutHelper(timeout);
            try
            {
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

        async Task SendCbsTokenAsync(AmqpCbsLink cbsLink, TimeSpan timeout)
        {
            string audience = this.ConnectionString.AmqpEndpoint.AbsoluteUri;
            string resource = this.ConnectionString.AmqpEndpoint.AbsoluteUri;
            var expiresAtUtc = await cbsLink.SendTokenAsync(
                this.ConnectionString,
                this.ConnectionString.AmqpEndpoint,
                audience,
                resource,
                AccessRightsHelper.AccessRightsToStringArray(this.accessRights),
                timeout);
            this.ScheduleTokenRefresh(expiresAtUtc);
        }

        async void OnRefreshTokenAsync()
        {
            AmqpSession amqpSession = this.faultTolerantSession.Value;
            if (amqpSession != null && !amqpSession.IsClosing())
            {
                var cbsLink = amqpSession.Connection.Extensions.Find<AmqpCbsLink>();
                if (cbsLink != null)
                {
                    try
                    {
                        await this.SendCbsTokenAsync(cbsLink, DefaultOperationTimeout);
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
        }

        void ScheduleTokenRefresh(DateTime expiresAtUtc)
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
    }
}