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
    using System.Collections.Concurrent;
    using System.Net;
    using System.Net.WebSockets;
    using System.Security.Cryptography.X509Certificates;
    using Microsoft.Azure.Amqp;
    using Microsoft.Azure.Amqp.Framing;
    using Microsoft.Azure.Amqp.Transport;
    using Microsoft.Azure.Devices.Client.Extensions;
#endif

    sealed class IotHubConnection
    {
        internal static readonly TimeSpan DefaultOperationTimeout = TimeSpan.FromMinutes(1);
        internal static readonly TimeSpan DefaultOpenTimeout = TimeSpan.FromMinutes(1);
        static readonly TimeSpan RefreshTokenBuffer = TimeSpan.FromMinutes(2);
        static readonly TimeSpan RefreshTokenRetryInterval = TimeSpan.FromSeconds(30);

#if !WINDOWS_UWP
        static readonly AmqpVersion AmqpVersion_1_0_0 = new AmqpVersion(1, 0, 0);
        const string DisableServerCertificateValidationKeyName = "Microsoft.Azure.Devices.DisableServerCertificateValidation";
        readonly static Lazy<bool> DisableServerCertificateValidation = new Lazy<bool>(InitializeDisableServerCertificateValidation);
        readonly IotHubConnectionString connectionString;
        readonly AccessRights accessRights;
        readonly FaultTolerantAmqpObject<AmqpSession> faultTolerantSession;
        readonly ConcurrentDictionary<string, IotHubLinkRefreshTokenTimer> iotHubLinkRefreshTokenTimers;
        readonly bool useWebSocketOnly;

        public IotHubConnection(IotHubConnectionString connectionString, AccessRights accessRights, bool useWebSocketOnly)
        {
            this.connectionString = connectionString;
            this.accessRights = accessRights;
            this.faultTolerantSession = new FaultTolerantAmqpObject<AmqpSession>(this.CreateSessionAsync, this.CloseConnection);
            this.iotHubLinkRefreshTokenTimers = new ConcurrentDictionary<string, IotHubLinkRefreshTokenTimer>();
            this.useWebSocketOnly = useWebSocketOnly;
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
            this.CancelRefreshTokenTimers();
            return this.faultTolerantSession.CloseAsync();
        }

        public void SafeClose(Exception exception)
        {
            this.CancelRefreshTokenTimers();
            this.faultTolerantSession.Close();
        }

        public async Task<SendingAmqpLink> CreateSendingLinkAsync(string path, IotHubConnectionString connectionString, string deviceId, TimeSpan timeout)
        {
            var timeoutHelper = new TimeoutHelper(timeout);
            AmqpSession session;
            if (!this.faultTolerantSession.TryGetOpenedObject(out session))
            {
                session = await this.faultTolerantSession.GetOrCreateAsync(timeoutHelper.RemainingTime());
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

            await OpenLinkAsync(link, connectionString, deviceId, session, timeoutHelper.RemainingTime());

            return link;
        }

        public async Task<ReceivingAmqpLink> CreateReceivingLink(string path, IotHubConnectionString connectionString, string deviceId, TimeSpan timeout, uint prefetchCount)
        {
            var timeoutHelper = new TimeoutHelper(timeout);

            AmqpSession session;
            if (!this.faultTolerantSession.TryGetOpenedObject(out session))
            {
                session = await this.faultTolerantSession.GetOrCreateAsync(timeoutHelper.RemainingTime());
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

            await OpenLinkAsync(link, connectionString, deviceId, session, timeoutHelper.RemainingTime());

            return link;
        }

        public async Task<RequestResponseAmqpLink> CreateRequestResponseLink(string path, IotHubConnectionString connectionString, string deviceId, TimeSpan timeout)
        {
            var timeoutHelper = new TimeoutHelper(timeout);

            AmqpSession session;
            if (!this.faultTolerantSession.TryGetOpenedObject(out session))
            {
                session = await this.faultTolerantSession.GetOrCreateAsync(timeoutHelper.RemainingTime());
            }

            var linkAddress = connectionString.BuildLinkAddress(path);

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

            await OpenLinkAsync(link, connectionString, deviceId, session, timeoutHelper.RemainingTime());

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

            // Cleanup any lingering link refresh token timers
            this.CancelRefreshTokenTimers();

            var amqpSettings = this.CreateAmqpSettings();
            var tlsTransportSettings = this.CreateTlsTransportSettings();

            var amqpTransportInitiator = new AmqpTransportInitiator(amqpSettings, tlsTransportSettings);
            TransportBase transport;
            if (this.useWebSocketOnly)
            {
                // Try only Amqp transport over WebSocket
                transport = await this.CreateClientWebSocketTransport(timeoutHelper.RemainingTime());
            }
            else
            {
                try
                {
                    transport = await amqpTransportInitiator.ConnectTaskAsync(timeoutHelper.RemainingTime());
                }
                catch (Exception e)
                {
                    if (Fx.IsFatal(e))
                    {
                        throw;
                    }

                    // Amqp transport over TCP failed. Retry Amqp transport over WebSocket
                    if (timeoutHelper.RemainingTime() != TimeSpan.Zero)
                    {
                        transport = await this.CreateClientWebSocketTransport(timeoutHelper.RemainingTime());
                    }
                    else
                    {
                        throw;
                    }
                }
            }

            AmqpConnectionSettings amqpConnectionSettings = new AmqpConnectionSettings()
            {
                MaxFrameSize = AmqpConstants.DefaultMaxFrameSize,
                ContainerId = Guid.NewGuid().ToString("N"),
                HostName = connectionString.AmqpEndpoint.Host
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

            // Cbs tokens are per link now. Let each link renew its own token
            return amqpSession;
        }

        async Task OpenLinkAsync(AmqpObject link, IotHubConnectionString connectionString, string deviceId, AmqpSession session, TimeSpan timeout)
        {
            var timeoutHelper = new TimeoutHelper(timeout);
            try
            {
                var iotHubLinkRefreshTokenTimer = new IotHubLinkRefreshTokenTimer(this, connectionString, deviceId, link);
                // Send Cbs token for new link first
                await iotHubLinkRefreshTokenTimer.SendLinkCbsTokenAsync(timeoutHelper.RemainingTime());
                // Open Amqp Link
                await link.OpenAsync(timeoutHelper.RemainingTime());
                if (this.iotHubLinkRefreshTokenTimers.TryAdd(connectionString.ToString(), iotHubLinkRefreshTokenTimer))
                {
                    link.SafeAddClosed((s, e) =>
                    {
                        if (this.iotHubLinkRefreshTokenTimers.TryRemove(connectionString.ToString(), out iotHubLinkRefreshTokenTimer))
                        {
                            iotHubLinkRefreshTokenTimer.Cancel();
                        }

                    });
                }
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
            linkSettings.AddProperty(IotHubAmqpProperty.ClientVersion, Utils.GetClientVersion());

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
                Certificate = null, // TODO: add client cert support
                CertificateValidationCallback = this.OnRemoteCertificateValidation
            };

            return tlsTransportSettings;
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

        void CancelRefreshTokenTimers()
        {
            foreach (var iotHubLinkRefreshTokenTimer in this.iotHubLinkRefreshTokenTimers)
            {
                iotHubLinkRefreshTokenTimer.Value.Cancel();
            }

            this.iotHubLinkRefreshTokenTimers.Clear();
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

        class IotHubLinkRefreshTokenTimer
        {
            readonly IotHubConnection connection;
            readonly IotHubConnectionString connectionString;
            readonly string deviceId;
            readonly IOThreadTimer refreshTokenTimer;

            public IotHubLinkRefreshTokenTimer(IotHubConnection connection, IotHubConnectionString connectionString, string deviceId, AmqpObject link)
            {
                this.connection = connection;
                this.connectionString = connectionString;
                this.deviceId = deviceId;
                this.refreshTokenTimer = new IOThreadTimer(s => ((IotHubLinkRefreshTokenTimer)s).OnLinkRefreshToken(), this, false);
            }

            public void Cancel()
            {
                this.refreshTokenTimer.Cancel();
            }

            public async Task SendLinkCbsTokenAsync(TimeSpan timeout)
            {
                AmqpSession amqpSession = this.connection.faultTolerantSession.Value;
                if (amqpSession != null && !amqpSession.IsClosing())
                {
                    var cbsLink = amqpSession.Connection.Extensions.Find<AmqpCbsLink>();
                    if (cbsLink != null)
                    {
                        string audience = this.connectionString.SharedAccessKeyName != null ?
                            this.connectionString.AmqpEndpoint.AbsoluteUri : "{0}/devices/{1}".FormatInvariant(this.connectionString.AmqpEndpoint.AbsoluteUri, WebUtility.UrlEncode(this.deviceId));
                        string resource = this.connectionString.AmqpEndpoint.AbsoluteUri;
                        var expiresAtUtc = await cbsLink.SendTokenAsync(
                            this.connectionString,
                            this.connectionString.AmqpEndpoint,
                            audience,
                            resource,
                            AccessRightsHelper.AccessRightsToStringArray(this.connection.accessRights),
                            timeout);
                        this.ScheduleLinkTokenRefresh(expiresAtUtc);
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

            async void OnLinkRefreshToken()
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
