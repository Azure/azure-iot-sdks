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
    using System.Net;
    using System.Net.WebSockets;
    using System.Security.Cryptography.X509Certificates;
    using Microsoft.Azure.Amqp;
    using Microsoft.Azure.Amqp.Framing;
    using Microsoft.Azure.Amqp.Transport;
#endif

    abstract class IotHubConnection
    {
        internal static readonly TimeSpan DefaultOperationTimeout = TimeSpan.FromMinutes(1);
        internal static readonly TimeSpan DefaultOpenTimeout = TimeSpan.FromMinutes(1);
        protected static readonly TimeSpan RefreshTokenBuffer = TimeSpan.FromMinutes(2);
        protected static readonly TimeSpan RefreshTokenRetryInterval = TimeSpan.FromSeconds(30);

#if !WINDOWS_UWP
        protected readonly AccessRights accessRights;
        protected FaultTolerantAmqpObject<AmqpSession> faultTolerantSession;
        protected readonly IotHubConnectionString connectionString;
        protected readonly AmqpTransportSettings amqpTransportSettings;
        readonly IotHubConnectionCache.CachedConnection cachedConnection;

        static readonly AmqpVersion AmqpVersion_1_0_0 = new AmqpVersion(1, 0, 0);
        const string DisableServerCertificateValidationKeyName = "Microsoft.Azure.Devices.DisableServerCertificateValidation";
        readonly static Lazy<bool> DisableServerCertificateValidation = new Lazy<bool>(InitializeDisableServerCertificateValidation);

        protected IotHubConnection(IotHubConnectionCache.CachedConnection cachedConnection, IotHubConnectionString connectionString, AccessRights accessRights, AmqpTransportSettings amqpTransportSettings)
        {
            this.cachedConnection = cachedConnection;
            this.connectionString = connectionString;
            this.accessRights = accessRights;
            this.amqpTransportSettings = amqpTransportSettings;
        }

        // In case of device-scope connection strings, this connectionString would represent only one of many devices sharing the connection
        public IotHubConnectionString ConnectionString
        {
            get
            {
                return this.connectionString;
            }
        }

        public abstract Task OpenAsync(TimeSpan timeout);

        public abstract Task CloseAsync();

        public abstract void SafeClose(Exception exception);

        public abstract Task<SendingAmqpLink> CreateSendingLinkAsync(string path, IotHubConnectionString connectionString, TimeSpan timeout);

        public abstract Task<ReceivingAmqpLink> CreateReceivingLinkAsync(string path, IotHubConnectionString connectionString, TimeSpan timeout, uint prefetchCount);

        public abstract Task<RequestResponseAmqpLink> CreateRequestResponseLinkAsync(string path, IotHubConnectionString connectionString, TimeSpan timeout);

        public void CloseLink(AmqpLink link)
        {
            link.SafeClose();
        }

        public void Release()
        {
            if (this.cachedConnection != null)
            {
                this.cachedConnection.Release();
            }
        }

        protected static bool InitializeDisableServerCertificateValidation()
        {
            string value = ConfigurationManager.AppSettings[DisableServerCertificateValidationKeyName];
            if (!string.IsNullOrEmpty(value))
            {
                return bool.Parse(value);
            }

            return false;
        }

        protected virtual async Task<AmqpSession> CreateSessionAsync(TimeSpan timeout)
        {
            var timeoutHelper = new TimeoutHelper(timeout);

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
            return amqpSession;
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
            Uri websocketUri = new Uri(WebSocketConstants.Scheme + this.ConnectionString.HostName + ":" + WebSocketConstants.SecurePort + WebSocketConstants.UriSuffix);
            var websocket = await CreateClientWebSocketAsync(websocketUri, timeoutHelper.RemainingTime());
            return new ClientWebSocketTransport(
                websocket,
                null,
                null);
        }

        static AmqpSettings CreateAmqpSettings()
        {
            var amqpSettings = new AmqpSettings();          

            var amqpTransportProvider = new AmqpTransportProvider();
            amqpTransportProvider.Versions.Add(AmqpVersion_1_0_0);
            amqpSettings.TransportProviders.Add(amqpTransportProvider);

            return amqpSettings;
        }

        protected static AmqpLinkSettings SetLinkSettingsCommonProperties(AmqpLinkSettings linkSettings, TimeSpan timeSpan)
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
                CertificateValidationCallback = OnRemoteCertificateValidation
            };

            return tlsTransportSettings;
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

#endif
    }
}
