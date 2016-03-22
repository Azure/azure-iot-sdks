namespace Microsoft.Azure.Devices.Client
{
    using System;
    using System.Threading.Tasks;
    using Microsoft.Azure.Amqp;
    using Microsoft.Azure.Amqp.Framing;
    using Microsoft.Azure.Devices.Client.Extensions;

    sealed class IotHubSingleTokenConnection : IotHubConnection
    {
        IotHubTokenRefresher iotHubTokenRefresher;

        public IotHubSingleTokenConnection(IotHubConnectionCache.ConnectionReferenceCounter connectionReferenceCounter, IotHubConnectionString connectionString, AccessRights accessRights, AmqpTransportSettings amqpTransportSettings)
            :base(connectionReferenceCounter, connectionString, accessRights, amqpTransportSettings)
        {
            this.FaultTolerantSession = new FaultTolerantAmqpObject<AmqpSession>(this.CreateSessionAsync, this.CloseConnection);
        }

        public override Task OpenAsync(TimeSpan timeout)
        {
            return this.FaultTolerantSession.GetOrCreateAsync(timeout);
        }

        public override Task CloseAsync()
        {
            return this.FaultTolerantSession.CloseAsync();
        }

        public override void SafeClose(Exception exception)
        {
            this.FaultTolerantSession.Close();
        }

        public override int GetNumberOfLinks()
        {
            return 1;
        }

        public override async Task<SendingAmqpLink> CreateSendingLinkAsync(string path, IotHubConnectionString doNotUse, TimeSpan timeout)
        {
            var timeoutHelper = new TimeoutHelper(timeout);

            AmqpSession session;
            if (!this.FaultTolerantSession.TryGetOpenedObject(out session))
            {
                session = await this.FaultTolerantSession.GetOrCreateAsync(timeoutHelper.RemainingTime());
            }

            var linkAddress = this.ConnectionString.BuildLinkAddress(path);

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

        public override async Task<ReceivingAmqpLink> CreateReceivingLinkAsync(string path, IotHubConnectionString doNotUse, TimeSpan timeout, uint prefetchCount)
        {
            var timeoutHelper = new TimeoutHelper(timeout);

            AmqpSession session;
            if (!this.FaultTolerantSession.TryGetOpenedObject(out session))
            {
                session = await this.FaultTolerantSession.GetOrCreateAsync(timeoutHelper.RemainingTime());
            }

            var linkAddress = this.ConnectionString.BuildLinkAddress(path);

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

        protected override async Task<AmqpSession> CreateSessionAsync(TimeSpan timeout)
        {
            var timeoutHelper = new TimeoutHelper(timeout);

            if (this.iotHubTokenRefresher != null)
            {
                this.iotHubTokenRefresher.Cancel();
                this.iotHubTokenRefresher = null;
            }

            AmqpSession amqpSession = await base.CreateSessionAsync(timeoutHelper.RemainingTime());
            this.iotHubTokenRefresher = new IotHubTokenRefresher(
                amqpSession, 
                this.ConnectionString, 
                this.ConnectionString.AmqpEndpoint.AbsoluteUri, 
                this.AmqpTransportSettings.OperationTimeout,
                this.AccessRights);

            // Send Cbs token for new connection first
            await this.iotHubTokenRefresher.SendCbsTokenAsync(timeoutHelper.RemainingTime());

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
    }
}