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
    using Microsoft.Azure.Amqp.Transport;
    using Microsoft.Azure.Devices.Client.Extensions;
    using Microsoft.Azure.Devices.Client.Transport;

    abstract class IotHubConnection
    {
        readonly string hostName;
        readonly int port;

        static readonly AmqpVersion AmqpVersion_1_0_0 = new AmqpVersion(1, 0, 0);
        const string DisableServerCertificateValidationKeyName = "Microsoft.Azure.Devices.DisableServerCertificateValidation";
        static readonly Lazy<bool> DisableServerCertificateValidation = new Lazy<bool>(InitializeDisableServerCertificateValidation);

        protected IotHubConnection(string hostName, int port, AmqpTransportSettings amqpTransportSettings)
        {
            this.hostName = hostName;
            this.port = port;
            this.AmqpTransportSettings = amqpTransportSettings;
        }

        protected FaultTolerantAmqpObject<AmqpSession> FaultTolerantSession { get; set; }

        protected AmqpTransportSettings AmqpTransportSettings { get; }

        public abstract Task CloseAsync();

        public abstract void SafeClose(Exception exception);

        public async Task<SendingAmqpLink> CreateSendingLinkAsync(string path, IotHubConnectionString connectionString, TimeSpan timeout)
        {
            this.OnCreateSendingLink(connectionString);

            var timeoutHelper = new TimeoutHelper(timeout);

            AmqpSession session;
            if (!this.FaultTolerantSession.TryGetOpenedObject(out session))
            {
                session = await this.FaultTolerantSession.GetOrCreateAsync(timeoutHelper.RemainingTime());
            }

            var linkAddress = this.BuildLinkAddress(connectionString, path);

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

            var audience = this.BuildAudience(connectionString, path);
            await this.OpenLinkAsync(link, connectionString, audience, timeoutHelper.RemainingTime());

            return link;
        }

        public async Task<ReceivingAmqpLink> CreateReceivingLinkAsync(string path, IotHubConnectionString connectionString, TimeSpan timeout, uint prefetchCount)
        {
            this.OnCreateReceivingLink(connectionString);

            var timeoutHelper = new TimeoutHelper(timeout);

            AmqpSession session;
            if (!this.FaultTolerantSession.TryGetOpenedObject(out session))
            {
                session = await this.FaultTolerantSession.GetOrCreateAsync(timeoutHelper.RemainingTime());
            }

            var linkAddress = this.BuildLinkAddress(connectionString, path);

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

            var audience = this.BuildAudience(connectionString, path);
            await this.OpenLinkAsync(link, connectionString, audience, timeoutHelper.RemainingTime());

            return link;
        }

        public void CloseLink(AmqpLink link)
        {
            link.SafeClose();
        }

        public abstract void Release(string deviceId);

        protected abstract Uri BuildLinkAddress(IotHubConnectionString iotHubConnectionString, string path);

        protected abstract string BuildAudience(IotHubConnectionString iotHubConnectionString, string path);

        protected abstract Task OpenLinkAsync(AmqpObject link, IotHubConnectionString connectionString, string audience, TimeSpan timeout);

        protected static bool InitializeDisableServerCertificateValidation()
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

        protected virtual void OnCreateSendingLink(IotHubConnectionString connectionString)
        {
            // do nothing. Override in derived classes if necessary
        }

        protected virtual void OnCreateReceivingLink(IotHubConnectionString connectionString)
        {
            // do nothing. Override in derived classes if necessary
        }

        protected virtual async Task<AmqpSession> CreateSessionAsync(TimeSpan timeout)
        {
            this.OnCreateSession();

            var timeoutHelper = new TimeoutHelper(timeout);

            AmqpSettings amqpSettings = CreateAmqpSettings();
            TransportBase transport;

            switch (this.AmqpTransportSettings.GetTransportType())
            {
#if !WINDOWS_UWP
                case TransportType.Amqp_WebSocket_Only:
                    transport = await this.CreateClientWebSocketTransportAsync(timeoutHelper.RemainingTime());
                    break;
#endif
                case TransportType.Amqp_Tcp_Only:
                    TlsTransportSettings tlsTransportSettings = this.CreateTlsTransportSettings();
                    var amqpTransportInitiator = new AmqpTransportInitiator(amqpSettings, tlsTransportSettings);
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
            try
            {
                await amqpConnection.OpenAsync(timeoutHelper.RemainingTime());

                var sessionSettings = new AmqpSessionSettings()
                {
                    Properties = new Fields()
                };

                AmqpSession amqpSession = amqpConnection.CreateSession(sessionSettings);
                await amqpSession.OpenAsync(timeoutHelper.RemainingTime());

                // This adds itself to amqpConnection.Extensions
                var cbsLink = new AmqpCbsLink(amqpConnection);
                return amqpSession;
            }
            catch (Exception ex) when (!ex.IsFatal())
            {
                if (amqpConnection.TerminalException != null)
                {
                    throw AmqpClientHelper.ToIotHubClientContract(amqpConnection.TerminalException);
                }

                amqpConnection.SafeClose(ex);
                throw;
            }
        }

        protected virtual void OnCreateSession()
        {
            // do nothing. Override in derived classes if necessary
        }

#if !WINDOWS_UWP
        async Task<ClientWebSocket> CreateClientWebSocketAsync(Uri websocketUri, TimeSpan timeout)
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

            if (this.AmqpTransportSettings.ClientCertificate != null)
            {
                websocket.Options.ClientCertificates.Add(this.AmqpTransportSettings.ClientCertificate);
            }
            else
            {
                websocket.Options.UseDefaultCredentials = true;
            }

            using (var cancellationTokenSource = new CancellationTokenSource(timeout))
            {
                await websocket.ConnectAsync(websocketUri, cancellationTokenSource.Token);
            }

            return websocket;
        }

        async Task<TransportBase> CreateClientWebSocketTransportAsync(TimeSpan timeout)
        {
            var timeoutHelper = new TimeoutHelper(timeout);
            Uri websocketUri = new Uri(WebSocketConstants.Scheme + this.hostName + ":" + WebSocketConstants.SecurePort + WebSocketConstants.UriSuffix);

            // Use Legacy WebSocket if it is running on Windows 7 or older. Windows 7/Windows 2008 R2 is version 6.1
            if (Environment.OSVersion.Version.Major < 6 || (Environment.OSVersion.Version.Major == 6 && Environment.OSVersion.Version.Minor <= 1))
            {
                var websocket = await CreateLegacyClientWebSocketAsync(websocketUri, timeoutHelper.RemainingTime());
                return new LegacyClientWebSocketTransport(
                    websocket,
                    this.AmqpTransportSettings.OperationTimeout,
                    null,
                    null);
            }
            else
            {
                var websocket = await this.CreateClientWebSocketAsync(websocketUri, timeoutHelper.RemainingTime());
                return new ClientWebSocketTransport(
                    websocket,
                    null,
                    null);
            }
        }

        static async Task<IotHubClientWebSocket> CreateLegacyClientWebSocketAsync(Uri webSocketUri,  TimeSpan timeout)
        {
            var websocket = new IotHubClientWebSocket(WebSocketConstants.SubProtocols.Amqpwsb10);
            await websocket.ConnectAsync(webSocketUri.Host, webSocketUri.Port, WebSocketConstants.Scheme, timeout);
            return websocket;
        }
#endif

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
#if WINDOWS_UWP
            // System.Reflection.Assembly.GetExecutingAssembly() does not exist for UWP, therefore use a hard-coded version name
            // (This string is picked up by the bump_version script, so don't change the line below)
            var UWPAssemblyVersion = "1.0.19";
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
                Host = this.hostName,
                Port = this.port
            };

            var tlsTransportSettings = new TlsTransportSettings(tcpTransportSettings)
            {
                TargetHost = this.hostName,
#if !WINDOWS_UWP // Not supported in UWP
                Certificate = null,
                CertificateValidationCallback = OnRemoteCertificateValidation
#endif
            };

#if !WINDOWS_UWP
            if (this.AmqpTransportSettings.ClientCertificate != null)
            {
                tlsTransportSettings.Certificate = this.AmqpTransportSettings.ClientCertificate;
            }
#endif

            return tlsTransportSettings;
        }

#if !WINDOWS_UWP // Not supported in UWP
        public static bool OnRemoteCertificateValidation(object sender, X509Certificate certificate, X509Chain chain, SslPolicyErrors sslPolicyErrors)
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
    }
}
