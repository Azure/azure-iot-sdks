// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices
{
    using System;
    using System.Configuration;
    using System.Net.Security;
    using System.Security.Cryptography.X509Certificates;
    using System.Threading;
    using System.Threading.Tasks;

    using Microsoft.Azure.Devices.Common;
    using Microsoft.Azure.Amqp;
    using Microsoft.Azure.Amqp.Framing;
    using Microsoft.Azure.Amqp.Transport;
    using Microsoft.Azure.Devices.Common.Client;
    using Microsoft.Azure.Devices.Common.Data;

    sealed class IotHubConnection : IDisposable
    {
        static readonly AmqpVersion AmqpVersion_1_0_0 = new AmqpVersion(1, 0, 0);
        internal static readonly TimeSpan DefaultOperationTimeout = TimeSpan.FromMinutes(1);
        internal static readonly TimeSpan DefaultOpenTimeout = TimeSpan.FromMinutes(1);
        static readonly TimeSpan RefreshTokenBuffer = TimeSpan.FromMinutes(2);
        static readonly TimeSpan RefreshTokenRetryInterval = TimeSpan.FromSeconds(30);
        const string DisableServerCertificateValidationKeyName = "Microsoft.Azure.Devices.DisableServerCertificateValidation";
        readonly static Lazy<bool> DisableServerCertificateValidation = new Lazy<bool>(InitializeDisableServerCertificateValidation);
        readonly IotHubConnectionString connectionString;
        readonly AccessRights accessRights;
        readonly FaultTolerantAmqpObject<AmqpSession> faultTolerantSession;
        readonly IOThreadTimer refreshTokenTimer;

        public IotHubConnection(IotHubConnectionString connectionString, AccessRights accessRights)
        {
            this.connectionString = connectionString;
            this.accessRights = accessRights;
            this.faultTolerantSession = new FaultTolerantAmqpObject<AmqpSession>(this.CreateSessionAsync, this.CloseConnection);
            this.refreshTokenTimer = new IOThreadTimer(s => ((IotHubConnection)s).OnRefreshToken(), this, false);
        }

        internal IotHubConnectionString ConnectionString
        {
            get
            {
                return this.connectionString;
            }
        }

        public Task OpenAsync(TimeSpan timeout)
        {
            return this.faultTolerantSession.GetOrCreateAsync(timeout);
        }

        public Task CloseAsync()
        {
            return this.faultTolerantSession.CloseAsync();
        }

        public void SafeClose(Exception exception)
        {
            this.faultTolerantSession.Close();
        }

        public async Task<SendingAmqpLink> CreateSendingLinkAsync(string path, TimeSpan timeout)
        {
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

            linkSettings.AddProperty(IotHubAmqpProperty.TimeoutName, timeoutHelper.RemainingTime().TotalMilliseconds);

            var link = new SendingAmqpLink(linkSettings);
            link.AttachTo(session);

            await OpenLinkAsync(link, timeoutHelper.RemainingTime());

            return link;
        }

        public async Task<ReceivingAmqpLink> CreateReceivingLink(string path, TimeSpan timeout, uint prefetchCount)
        {
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

            linkSettings.AddProperty(IotHubAmqpProperty.TimeoutName, timeoutHelper.RemainingTime().TotalMilliseconds);

            var link = new ReceivingAmqpLink(linkSettings);
            link.AttachTo(session);

            await OpenLinkAsync(link, timeoutHelper.RemainingTime());

            return link;
        }

        public async Task<RequestResponseAmqpLink> CreateRequestResponseLink(string path, TimeSpan timeout)
        {
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

            linkSettings.AddProperty(IotHubAmqpProperty.TimeoutName, timeoutHelper.RemainingTime().TotalMilliseconds);

            var link = new RequestResponseAmqpLink(session, linkSettings);

            await OpenLinkAsync(link, timeoutHelper.RemainingTime());

            return link;
        }

        public void CloseLink(AmqpLink link)
        {
            link.SafeClose();
        }

        static bool InitializeDisableServerCertificateValidation()
        {
            string value = ConfigurationManager.AppSettings[DisableServerCertificateValidationKeyName];
            if (!string.IsNullOrEmpty(value))
            {
                return bool.Parse(value);
            }

            return false;
        }

        async Task<AmqpSession> CreateSessionAsync(TimeSpan timeout)
        {
            TimeoutHelper timeoutHelper = new TimeoutHelper(timeout);
            this.refreshTokenTimer.Cancel();

            var amqpSettings = this.CreateAmqpSettings();
            var tlsTransportSettings = this.CreateTlsTransportSettings();

            var amqpTransportInitiator = new AmqpTransportInitiator(amqpSettings, tlsTransportSettings);
            var transport = await amqpTransportInitiator.ConnectTaskAsync(timeoutHelper.RemainingTime());

            AmqpConnectionSettings amqpConnectionSettings = new AmqpConnectionSettings()
            {
                MaxFrameSize = AmqpConstants.DefaultMaxFrameSize,
                ContainerId = Guid.NewGuid().ToString("N"),
                HostName = this.connectionString.AmqpEndpoint.Host
            };

            var amqpConnection = new AmqpConnection(transport, amqpSettings, amqpConnectionSettings);
            await amqpConnection.OpenAsync(timeoutHelper.RemainingTime());

            var sessionSettings = new AmqpSessionSettings()
            {
                Properties = new Fields()
            };

            var amqpSession = amqpConnection.CreateSession(sessionSettings);
            await amqpSession.OpenAsync(timeoutHelper.RemainingTime());

            // This adds itself to amqpConnection.Extensions
            var cbsLink = new AmqpCbsLink(amqpConnection);
            await this.SendCbsTokenAsync(cbsLink, timeoutHelper.RemainingTime());
            return amqpSession;
        }

        void CloseConnection(AmqpSession amqpSession)
        {
            // Closing the connection also closes any sessions.
            amqpSession.Connection.SafeClose();
        }

        AmqpSettings CreateAmqpSettings()
        {
            var amqpSettings = new AmqpSettings();

            var amqpTransportProvider = new AmqpTransportProvider();
            amqpTransportProvider.Versions.Add(AmqpVersion_1_0_0);
            amqpSettings.TransportProviders.Add(amqpTransportProvider);

            return amqpSettings;
        }

        TlsTransportSettings CreateTlsTransportSettings()
        {
            var tcpTransportSettings = new TcpTransportSettings()
            {
                Host = this.connectionString.HostName,
                Port = this.connectionString.AmqpEndpoint.Port
            };

            var tlsTransportSettings = new TlsTransportSettings(tcpTransportSettings)
            {
                TargetHost = this.connectionString.HostName,
                Certificate = null, // TODO: add client cert support
                CertificateValidationCallback = this.OnRemoteCertificateValidation
            };

            return tlsTransportSettings;
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

        async void OnRefreshToken()
        {
            AmqpSession amqpSession;
            if (this.faultTolerantSession.TryGetOpenedObject(out amqpSession) && amqpSession != null && !amqpSession.IsClosing())
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

        bool OnRemoteCertificateValidation(object sender, X509Certificate certificate, X509Chain chain, SslPolicyErrors sslPolicyErrors)
        {
            if (sslPolicyErrors == SslPolicyErrors.None)
            {
                return true;
            }

            if (DisableServerCertificateValidation.Value && sslPolicyErrors == SslPolicyErrors.RemoteCertificateNameMismatch)
            {
                return true;
            }

            return false;

        }

        public static ArraySegment<byte> GetNextDeliveryTag(ref int deliveryTag)
        {
            int nextDeliveryTag = Interlocked.Increment(ref deliveryTag);
            return new ArraySegment<byte>(BitConverter.GetBytes(nextDeliveryTag));
        }

        public static ArraySegment<byte> ConvertToDeliveryTag(string lockToken)
        {
            if (lockToken == null)
            {
                throw new ArgumentNullException("lockToken");
            }

            Guid lockTokenGuid;
            if (!Guid.TryParse(lockToken, out lockTokenGuid))
            {
                throw new ArgumentException("Should be a valid Guid", "lockToken");
            }

            var deliveryTag = new ArraySegment<byte>(lockTokenGuid.ToByteArray());
            return deliveryTag;
        }
        
        /// <inheritdoc/>
        public void Dispose()
        {
            this.faultTolerantSession.Dispose();
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
