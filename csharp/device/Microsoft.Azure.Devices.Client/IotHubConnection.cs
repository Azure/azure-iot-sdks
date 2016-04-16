// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client
{
    using System;
    using System.Net.Security;
    using System.Threading;
    using System.Threading.Tasks;

#if !WINDOWS_UWP
    using System.Configuration;
    using System.Net.WebSockets;
    using System.Security.Cryptography.X509Certificates;
#endif
    using System.Net;
    using Microsoft.Azure.Amqp;
    using Microsoft.Azure.Amqp.Framing;
    using Microsoft.Azure.Amqp.Sasl;
    using Microsoft.Azure.Amqp.Transport;
    using Microsoft.Azure.Devices.Client.Exceptions;
    using Microsoft.Azure.Devices.Client.Extensions;

    sealed class IotHubConnection
    {
        internal static readonly TimeSpan DefaultOperationTimeout = TimeSpan.FromMinutes(1);
        internal static readonly TimeSpan DefaultOpenTimeout = TimeSpan.FromMinutes(1);
        static readonly TimeSpan RefreshTokenBuffer = TimeSpan.FromMinutes(2);
        static readonly TimeSpan RefreshTokenRetryInterval = TimeSpan.FromSeconds(30);
        static readonly AmqpVersion AmqpVersion_1_0_0 = new AmqpVersion(1, 0, 0);
        const string DisableServerCertificateValidationKeyName = "Microsoft.Azure.Devices.DisableServerCertificateValidation";
        readonly static Lazy<bool> DisableServerCertificateValidation = new Lazy<bool>(InitializeDisableServerCertificateValidation);
        readonly IotHubConnectionString connectionString;
        readonly AccessRights accessRights;
        readonly FaultTolerantAmqpObject<AmqpSession> faultTolerantSession;
#if WINDOWS_UWP
        readonly IOThreadTimerSlim refreshTokenTimer;
#else
        readonly IOThreadTimer refreshTokenTimer;
#endif
        readonly AmqpTransportSettings amqpTransportSettings;

        public IotHubConnection(IotHubConnectionString connectionString, AccessRights accessRights, AmqpTransportSettings amqpTransportSettings)
        {
            this.connectionString = connectionString;
            this.accessRights = accessRights;
            this.faultTolerantSession = new FaultTolerantAmqpObject<AmqpSession>(this.CreateSessionAsync, this.CloseConnection);

#if WINDOWS_UWP
            this.refreshTokenTimer = new IOThreadTimerSlim(s => ((IotHubConnection)s).OnRefreshToken(), this, false);
#else
            this.refreshTokenTimer = new IOThreadTimer(s => ((IotHubConnection)s).OnRefreshToken(), this, false);
#endif

            this.amqpTransportSettings = amqpTransportSettings;
        }

        public IotHubConnectionString ConnectionString
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

            SetLinkSettingsCommonProperties(linkSettings, timeoutHelper.RemainingTime());

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

            SetLinkSettingsCommonProperties(linkSettings, timeoutHelper.RemainingTime());

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

            SetLinkSettingsCommonProperties(linkSettings, timeoutHelper.RemainingTime());

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
#if !WINDOWS_UWP // No System.Configuration.ConfigurationManager in UWP
            string value = ConfigurationManager.AppSettings[DisableServerCertificateValidationKeyName];
            if (!string.IsNullOrEmpty(value))
            {
                return bool.Parse(value);
            }
#endif
            return false;
        }

        async Task<AmqpSession> CreateSessionAsync(TimeSpan timeout)
        {
            var timeoutHelper = new TimeoutHelper(timeout);
            this.refreshTokenTimer.Cancel();

            AmqpSettings amqpSettings = this.CreateAmqpSettings();
            TlsTransportSettings tlsTransportSettings = this.CreateTlsTransportSettings();

            var amqpTransportInitiator = new AmqpTransportInitiator(amqpSettings, tlsTransportSettings);
            TransportBase transport;

            switch (this.amqpTransportSettings.GetTransportType())
            {
#if !WINDOWS_UWP
                case TransportType.Amqp_WebSocket_Only:
                transport = await this.CreateClientWebSocketTransport(timeoutHelper.RemainingTime());
                    break;
#endif
                case TransportType.Amqp_Tcp_Only:
                    transport = await amqpTransportInitiator.ConnectTaskAsync(timeoutHelper.RemainingTime());
                    break;
                default:
                    throw new InvalidOperationException("AmqpTransportSettings must specify WebSocketOnly or TcpOnly");
            }

            var amqpConnectionSettings = new AmqpConnectionSettings()
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

        void CloseConnection(AmqpSession amqpSession)
        {
            // Closing the connection also closes any sessions.
            amqpSession.Connection.SafeClose();
        }

#if !WINDOWS_UWP
        async Task<ClientWebSocket> CreateClientWebSocket(Uri websocketUri, TimeSpan timeout)
        {
            var websocket = new ClientWebSocket();

            // Set SubProtocol to AMQPWSB10
            websocket.Options.AddSubProtocol(WebSocketConstants.SubProtocols.Amqpwsb10);

            // Check if we're configured to use a proxy server
            IWebProxy webProxy = WebRequest.DefaultWebProxy;
            Uri proxyAddress = webProxy != null ? webProxy.GetProxy(websocketUri) : null;
            if (!websocketUri.Equals(proxyAddress))
            {
                // Configure proxy server
                websocket.Options.Proxy = webProxy;
            }

            websocket.Options.UseDefaultCredentials = true;

            using (var cancellationTokenSource = new CancellationTokenSource(timeout))
            {
                await websocket.ConnectAsync(websocketUri, cancellationTokenSource.Token);
            }

            return websocket;
        }

        async Task<TransportBase> CreateClientWebSocketTransport(TimeSpan timeout)
        {
            TimeoutHelper timeoutHelper = new TimeoutHelper(timeout);
            Uri websocketUri = new Uri(WebSocketConstants.Scheme + this.ConnectionString.HostName + ":" + WebSocketConstants.SecurePort + WebSocketConstants.UriSuffix);
            var websocket = await this.CreateClientWebSocket(websocketUri, timeoutHelper.RemainingTime());
            return new ClientWebSocketTransport(
                websocket,
                this.connectionString.IotHubName,
                null,
                null);
        }
#endif

        AmqpSettings CreateAmqpSettings()
        {
            var amqpSettings = new AmqpSettings();          

            var amqpTransportProvider = new AmqpTransportProvider();
            amqpTransportProvider.Versions.Add(AmqpVersion_1_0_0);
            amqpSettings.TransportProviders.Add(amqpTransportProvider);

            return amqpSettings;
        }

        static AmqpLinkSettings SetLinkSettingsCommonProperties(AmqpLinkSettings linkSettings, TimeSpan timeSpan)
        {
            linkSettings.AddProperty(IotHubAmqpProperty.TimeoutName, timeSpan.TotalMilliseconds);
#if WINDOWS_UWP
            // System.Reflection.Assembly.GetExecutingAssembly() does not exist for UWP, therefore use a hard-coded version name
            // (This string is picked up by the bump_version script, so don't change the line below)
            var UWPAssemblyVersion = "1.0.5";
            linkSettings.AddProperty(IotHubAmqpProperty.ClientVersion, UWPAssemblyVersion);
#else
            linkSettings.AddProperty(IotHubAmqpProperty.ClientVersion, Utils.GetClientVersion());
#endif

            return linkSettings;
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
#if !WINDOWS_UWP // Not supported in UWP
                CertificateValidationCallback = this.OnRemoteCertificateValidation
#endif
            };

#if !WINDOWS_UWP
            if (this.amqpTransportSettings.ClientCertificate != null)
            {
                tlsTransportSettings.Certificate = this.amqpTransportSettings.ClientCertificate;
            }
#endif

            return tlsTransportSettings;
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

#if !WINDOWS_UWP // Not supported in UWP
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
#endif

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
