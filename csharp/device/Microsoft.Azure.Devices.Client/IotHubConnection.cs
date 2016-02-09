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
    using Microsoft.Azure.Devices.Client.Exceptions;
    using Microsoft.Azure.Devices.Client.Extensions;
#endif

    sealed class IotHubConnection
    {
        internal static readonly TimeSpan DefaultOperationTimeout = TimeSpan.FromMinutes(1);
        internal static readonly TimeSpan DefaultOpenTimeout = TimeSpan.FromMinutes(1);
        static readonly TimeSpan RefreshTokenBuffer = TimeSpan.FromMinutes(2);
        static readonly TimeSpan RefreshTokenRetryInterval = TimeSpan.FromSeconds(30);
        const int MaxNumberOfLinks = 256;

#if !WINDOWS_UWP
        static readonly AmqpVersion AmqpVersion_1_0_0 = new AmqpVersion(1, 0, 0);
        const string DisableServerCertificateValidationKeyName = "Microsoft.Azure.Devices.DisableServerCertificateValidation";
        readonly static Lazy<bool> DisableServerCertificateValidation = new Lazy<bool>(InitializeDisableServerCertificateValidation);
        readonly AccessRights accessRights;
        readonly FaultTolerantAmqpObject<AmqpSession> faultTolerantSession;
        readonly IOThreadTimer hubScopeRefreshTokenTimer;         // There can only be one HubScopeConnectionString per IotHubConnection instance
        readonly ConcurrentDictionary<string, IotHubLinkRefreshTokenTimer> iotHubLinkRefreshTokenTimers; // There can be multiple device-scope connection strings per IotHubConnection instance
        readonly string hostName;
        readonly int port;
        readonly AmqpTransportSettings amqpTransportSettings;
        readonly IotHubConnectionCache.CachedConnection cachedConnection;

        public IotHubConnection(IotHubConnectionCache.CachedConnection cachedConnection, IotHubConnectionString connectionString, AccessRights accessRights, AmqpTransportSettings amqpTransportSettings)
        {
            if (connectionString.SharedAccessKeyName != null)
            {
                this.HubScopeConnectionString = connectionString;
            }

            this.accessRights = accessRights;
            this.hostName = connectionString.HostName;
            this.port = connectionString.AmqpEndpoint.Port;
            this.cachedConnection = cachedConnection;
            this.faultTolerantSession = new FaultTolerantAmqpObject<AmqpSession>(this.CreateSessionAsync, this.CloseConnection);
            this.hubScopeRefreshTokenTimer = new IOThreadTimer(s => ((IotHubConnection)s).OnRefreshTokenAsync(), this, false);
            this.iotHubLinkRefreshTokenTimers = new ConcurrentDictionary<string, IotHubLinkRefreshTokenTimer>();
            this.amqpTransportSettings = amqpTransportSettings;
        }

        public IotHubConnectionString HubScopeConnectionString { get; private set; }

        public Task OpenAsync(TimeSpan timeout)
        {
            return this.faultTolerantSession.GetOrCreateAsync(timeout);
        }

        public Task CloseAsync()
        {
            this.CancelRefreshTokenTimers();
            this.hubScopeRefreshTokenTimer.Cancel();
            return this.faultTolerantSession.CloseAsync();
        }

        public void SafeClose(Exception exception)
        {
            this.CancelRefreshTokenTimers();
            this.hubScopeRefreshTokenTimer.Cancel();
            this.faultTolerantSession.Close();
        }

        public void Release()
        {
            if (cachedConnection != null)
            {
                cachedConnection.Release();
            }
        }

        /**
         The input connection string can be a device-scope connection string or a hub-scope connection string
        **/
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

            string audience = connectionString.Audience + CommonConstants.DeviceEventPathTemplate.FormatInvariant(WebUtility.UrlEncode(deviceId));
            await OpenLinkAsync(link, connectionString, audience, timeoutHelper.RemainingTime());

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

            string audience = connectionString.Audience + CommonConstants.DeviceBoundPathTemplate.FormatInvariant(WebUtility.UrlEncode(deviceId));
            await OpenLinkAsync(link, connectionString, audience, timeoutHelper.RemainingTime());

            return link;
        }

        public async Task<RequestResponseAmqpLink> CreateRequestResponseLinkAsync(string path, IotHubConnectionString connectionString, string deviceId, TimeSpan timeout)
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

            string audience = connectionString.Audience + CommonConstants.DeviceBoundPathTemplate.FormatInvariant(WebUtility.UrlEncode(deviceId));
            await OpenLinkAsync(link, connectionString, audience, timeoutHelper.RemainingTime());

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
            var timeoutHelper = new TimeoutHelper(timeout);
            this.hubScopeRefreshTokenTimer.Cancel();

            // Cleanup any lingering link refresh token timers
            this.CancelRefreshTokenTimers();
            AmqpSettings amqpSettings = CreateAmqpSettings();
            TlsTransportSettings tlsTransportSettings = this.CreateTlsTransportSettings();

            var amqpTransportInitiator = new AmqpTransportInitiator(amqpSettings, tlsTransportSettings);
            TransportBase transport;

            switch (this.amqpTransportSettings.GetTransportType())
            {
                case TransportType.Amqp_WebSocket_Only:
                transport = await this.CreateClientWebSocketTransportAsync(timeoutHelper.RemainingTime());
                    break;
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
                HostName = this.hostName
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

        async Task OpenLinkAsync(AmqpObject link, IotHubConnectionString connectionString, string audience, TimeSpan timeout)
        {
            var timeoutHelper = new TimeoutHelper(timeout);
            if (connectionString.SharedAccessKeyName != null)
            {
                // this is a hub-scope connection string. No need to send a CBS token for this link
                this.HubScopeConnectionString = connectionString;
            }
            else
            {
                if (this.iotHubLinkRefreshTokenTimers.Count == MaxNumberOfLinks)
                {
                   throw new ArgumentOutOfRangeException("IotHubConnection can support a maximum of {0}".FormatInvariant(MaxNumberOfLinks));
                }

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
            }

            try
            {
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

        static async Task<ClientWebSocket> CreateClientWebSocketAsync(Uri websocketUri, TimeSpan timeout)
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

        async Task<TransportBase> CreateClientWebSocketTransportAsync(TimeSpan timeout)
        {
            var timeoutHelper = new TimeoutHelper(timeout);
            var websocketUri = new Uri(WebSocketConstants.Scheme + this.hostName + ":" + WebSocketConstants.SecurePort + WebSocketConstants.UriSuffix);
            var websocket = await CreateClientWebSocketAsync(websocketUri, timeoutHelper.RemainingTime());
            return new ClientWebSocketTransport(websocket, null, null);
        }

        static AmqpSettings CreateAmqpSettings()
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
                Host = this.hostName,
                Port = this.port
            };

            var tlsTransportSettings = new TlsTransportSettings(tcpTransportSettings)
            {
                TargetHost = this.hostName,
                Certificate = null, // TODO: add client cert support
                CertificateValidationCallback = OnRemoteCertificateValidation
            };

            return tlsTransportSettings;
        }

        public async Task SendCbsTokenAsync(AmqpCbsLink cbsLink, TimeSpan timeout)
        {
            if (this.HubScopeConnectionString != null)
        {
                string audience = this.HubScopeConnectionString.AmqpEndpoint.DnsSafeHost;
                string resource = this.HubScopeConnectionString.AmqpEndpoint.AbsoluteUri;
            var expiresAtUtc = await cbsLink.SendTokenAsync(
                    this.HubScopeConnectionString,
                    this.HubScopeConnectionString.AmqpEndpoint,
                audience,
                resource,
                AccessRightsHelper.AccessRightsToStringArray(this.accessRights),
                timeout);
            this.ScheduleTokenRefresh(expiresAtUtc);
        }
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

                        this.hubScopeRefreshTokenTimer.Set(RefreshTokenRetryInterval);
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
                this.hubScopeRefreshTokenTimer.Set(timeFromNow);
            }
        }

        static bool OnRemoteCertificateValidation(object sender, X509Certificate certificate, X509Chain chain, SslPolicyErrors sslPolicyErrors)
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
            foreach (var iotHubLinkRefreshTokenTimer in this.iotHubLinkRefreshTokenTimers.Values)
            {
                iotHubLinkRefreshTokenTimer.Cancel();
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
                throw new ArgumentNullException(nameof(lockToken));
            }

            Guid lockTokenGuid;
            if (!Guid.TryParse(lockToken, out lockTokenGuid))
            {
                throw new ArgumentException("Should be a valid Guid", nameof(lockToken));
            }

            var deliveryTag = new ArraySegment<byte>(lockTokenGuid.ToByteArray());
            return deliveryTag;
        }

        class IotHubLinkRefreshTokenTimer
        {
            readonly IotHubConnection connection;
            readonly IotHubConnectionString connectionString;
            readonly string audience;
            readonly IOThreadTimer refreshTokenTimer;

            public IotHubLinkRefreshTokenTimer(IotHubConnection connection, IotHubConnectionString connectionString, string audience)
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
                AmqpSession amqpSession = this.connection.faultTolerantSession.Value;
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
