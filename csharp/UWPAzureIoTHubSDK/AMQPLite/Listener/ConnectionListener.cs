//  ------------------------------------------------------------------------------------
//  Copyright (c) Microsoft Corporation
//  All rights reserved. 
//  
//  Licensed under the Apache License, Version 2.0 (the ""License""); you may not use this 
//  file except in compliance with the License. You may obtain a copy of the License at 
//  http://www.apache.org/licenses/LICENSE-2.0  
//  
//  THIS CODE IS PROVIDED *AS IS* BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, 
//  EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION ANY IMPLIED WARRANTIES OR 
//  CONDITIONS OF TITLE, FITNESS FOR A PARTICULAR PURPOSE, MERCHANTABLITY OR 
//  NON-INFRINGEMENT. 
// 
//  See the Apache Version 2.0 License for specific language governing permissions and 
//  limitations under the License.
//  ------------------------------------------------------------------------------------

namespace Amqp.Listener
{
    using System;
    using System.Collections.Generic;
    using System.Net;
    using System.Net.Security;
    using System.Net.Sockets;
    using System.Net.WebSockets;
    using System.Security.Authentication;
    using System.Security.Cryptography.X509Certificates;
    using System.Threading.Tasks;
    using Amqp.Framing;
    using Amqp.Sasl;
    using Amqp.Types;

    /// <summary>
    /// The conneciton listener accepts AMQP connections from an address.
    /// </summary>
    public class ConnectionListener : ConnectionFactoryBase
    {
        readonly IContainer container;
        readonly HashSet<Connection> connections;
        readonly Address address;
        TransportListener listener;
        SslSettings sslSettings;
        SaslSettings saslSettings;

        /// <summary>
        /// Initializes the connection listener object.
        /// </summary>
        /// <param name="addressUri"></param>
        /// <param name="userInfo"></param>
        /// <param name="container"></param>
        public ConnectionListener(Uri addressUri, string userInfo, IContainer container)
            : base()
        {
            this.connections = new HashSet<Connection>();
            this.container = container;

            string userName = null;
            string password = null;
            if (userInfo != null)
            {
                string[] creds = userInfo.Split(':');
                if (creds.Length != 2)
                {
                    throw new ArgumentException("userInfo");
                }
                userName = Uri.UnescapeDataString(creds[0]);
                password = creds.Length == 1 ? string.Empty : Uri.UnescapeDataString(creds[1]);
            }

            this.address = new Address(addressUri.Host, addressUri.Port, userName, password, "/", addressUri.Scheme);
        }

        /// <summary>
        /// Gets the AMQP container.
        /// </summary>
        public IContainer Container
        {
            get { return this.container; }
        }

        /// <summary>
        /// Gets the address the listener is listening on.
        /// </summary>
        public Address Address
        {
            get { return this.address; }
        }

        /// <summary>
        /// Gets the TLS/SSL settings on the listener.
        /// </summary>
        public SslSettings SSL
        {
            get
            {
                return this.sslSettings ?? (this.sslSettings = new SslSettings());
            }
        }

        /// <summary>
        /// Gets the SASL settings on the listener.
        /// </summary>
        public SaslSettings SASL
        {
            get
            {
                return this.saslSettings ?? (this.saslSettings = new SaslSettings());
            }
        }

        /// <summary>
        /// Opens the listener.
        /// </summary>
        public void Open()
        {
            if (this.address.Scheme.Equals(Address.Amqp, StringComparison.OrdinalIgnoreCase))
            {
                this.listener = new TcpTransportListener(this, this.address.Host, this.address.Port);
            }
            else if (this.address.Scheme.Equals(Address.Amqps, StringComparison.OrdinalIgnoreCase))
            {
                this.listener = new TlsTransportListener(this, this.address.Host, this.address.Port, this.GetServiceCertificate());
            }
            else if (this.address.Scheme.Equals(WebSocketTransport.WebSockets, StringComparison.OrdinalIgnoreCase))
            {
                this.listener = new WebSocketTransportListener(this, this.address.Host, address.Port, address.Path, null);
            }
            else if (this.address.Scheme.Equals(WebSocketTransport.SecureWebSockets, StringComparison.OrdinalIgnoreCase))
            {
                this.listener = new WebSocketTransportListener(this, this.address.Host, address.Port, address.Path, this.GetServiceCertificate());
            }
            else
            {
                throw new NotSupportedException(this.address.Scheme);
            }

            if (this.address.User != null)
            {
                this.SASL.EnablePlainMechanism(this.address.User, this.address.Password);
            }

            this.listener.Open();
        }

        /// <summary>
        /// Closes the listener.
        /// </summary>
        public void Close()
        {
            this.listener.Close();
        }

        X509Certificate2 GetServiceCertificate()
        {
            if (this.sslSettings != null && this.sslSettings.Certificate != null)
            {
                return this.sslSettings.Certificate;
            }
            else if (this.container.ServiceCertificate != null)
            {
                return this.container.ServiceCertificate;
            }

            throw new ArgumentNullException("certificate");
        }

        async Task HandleTransportAsync(IAsyncTransport transport)
        {
            if (this.saslSettings != null)
            {
                ListenerSasProfile profile = new ListenerSasProfile(this);
                transport = await profile.OpenAsync(null, transport);
            }

            Connection connection = new ListenerConnection(this, this.address, transport);
            connection.Closed += this.OnConnectionClosed;
            lock (this.connections)
            {
                this.connections.Add(connection);
            }

            AsyncPump pump = new AsyncPump(transport);
            pump.Start(connection);
        }

        void OnConnectionClosed(AmqpObject sender, Error error)
        {
            lock (this.connections)
            {
                this.connections.Remove((Connection)sender);
            }
        }

        /// <summary>
        /// Contains the TLS/SSL settings for a connection.
        /// </summary>
        public class SslSettings
        {
            internal SslSettings()
            {
                this.Protocols = SslProtocols.Default;
            }

            /// <summary>
            /// Gets or sets the listener certificate.
            /// </summary>
            public X509Certificate2 Certificate
            {
                get;
                set;
            }

            /// <summary>
            /// Gets or sets a a Boolean value that specifies whether the client must supply a certificate for authentication.
            /// </summary>
            public bool ClientCertificateRequired
            {
                get;
                set;
            }

            /// <summary>
            /// Gets or sets the supported protocols to use.
            /// </summary>
            public SslProtocols Protocols
            {
                get;
                set;
            }

            /// <summary>
            /// Specifies whether certificate revocation should be performed during handshake.
            /// </summary>
            public bool CheckCertificateRevocation
            {
                get;
                set;
            }

            /// <summary>
            /// Gets or sets a certificate validation callback to validate remote certificate.
            /// </summary>
            public RemoteCertificateValidationCallback RemoteCertificateValidationCallback
            {
                get;
                set;
            }
        }

        /// <summary>
        /// Contains the SASL settings for a connection.
        /// </summary>
        public class SaslSettings
        {
            Dictionary<Symbol, SaslMechanism> mechanisms;

            internal SaslSettings()
            {
                this.mechanisms = new Dictionary<Symbol, SaslMechanism>();
            }

            internal Symbol[] Mechanisms
            {
                get
                {
                    return new List<Symbol>(this.mechanisms.Keys).ToArray();
                }
            }

            /// <summary>
            /// Gets or sets a value indicating if SASL EXTERNAL mechanism is enabled.
            /// </summary>
            public bool EnableExternalMechanism
            {
                get
                {
                    return this.mechanisms.ContainsKey(SaslExternalProfile.Name);
                }

                set
                {
                    if (value)
                    {
                        this.mechanisms[SaslExternalProfile.Name] = SaslMechanism.External;
                    }
                    else
                    {
                        this.mechanisms.Remove(SaslExternalProfile.Name);
                    }
                }
            }

            /// <summary>
            /// Enables SASL PLAIN mechanism.
            /// </summary>
            /// <param name="userName"></param>
            /// <param name="password"></param>
            public void EnablePlainMechanism(string userName, string password)
            {
                this.mechanisms[SaslPlainProfile.Name] = new SaslPlainMechanism(userName, password);
            }

            internal bool TryGetMechanism(Symbol name, out SaslMechanism mechanism)
            {
                return this.mechanisms.TryGetValue(name, out mechanism);
            }
        }

        class ListenerSasProfile : SaslProfile
        {
            readonly ConnectionListener listener;
            SaslProfile innerProfile;

            public ListenerSasProfile(ConnectionListener listener)
            {
                this.listener = listener;
            }

            public SaslProfile InnerProfile
            {
                get { return this.innerProfile; }
            }

            protected override ITransport UpgradeTransport(ITransport transport)
            {
                return transport;
            }

            protected override DescribedList GetStartCommand(string hostname)
            {
                Symbol[] symbols = this.listener.saslSettings.Mechanisms;
                return new SaslMechanisms() { SaslServerMechanisms = symbols };
            }

            protected override DescribedList OnCommand(DescribedList command)
            {
                if (this.innerProfile == null)
                {
                    if (command.Descriptor.Code == Codec.SaslInit.Code)
                    {
                        var init = (SaslInit)command;
                        SaslMechanism saslMechanism;
                        if (!this.listener.saslSettings.TryGetMechanism(init.Mechanism, out saslMechanism))
                        {
                            throw new AmqpException(ErrorCode.NotImplemented, init.Mechanism);
                        }

                        this.innerProfile = saslMechanism.CreateProfile();
                    }
                    else
                    {
                        throw new AmqpException(ErrorCode.NotAllowed, command.Descriptor.Name);
                    }
                }

                return this.innerProfile.OnCommandInternal(command);
            }
        }

        abstract class TransportListener
        {
            protected bool closed;

            protected ConnectionListener Listener
            {
                get;
                set;
            }

            public abstract void Open();

            public abstract void Close();
        }

        class TcpTransportListener : TransportListener
        {
            Socket[] listenSockets;

            public TcpTransportListener(ConnectionListener listener, string host, int port)
            {
                this.Listener = listener;

                List<IPAddress> addresses = new List<IPAddress>();
                IPAddress ipAddress;
                if (host.Equals("localhost", StringComparison.OrdinalIgnoreCase) ||
                    host.Equals(Environment.MachineName, StringComparison.OrdinalIgnoreCase) ||
                    host.Equals(Dns.GetHostEntry(string.Empty).HostName, StringComparison.OrdinalIgnoreCase))
                {
                    if (Socket.OSSupportsIPv4)
                    {
                        addresses.Add(IPAddress.Any);
                    }

                    if (Socket.OSSupportsIPv6)
                    {
                        addresses.Add(IPAddress.IPv6Any);
                    }
                }
                else if (IPAddress.TryParse(host, out ipAddress))
                {
                    addresses.Add(ipAddress);
                }
                else
                {
                    addresses.AddRange(Dns.GetHostAddresses(host));
                }

                this.listenSockets = new Socket[addresses.Count];
                for (int i = 0; i < addresses.Count; ++i)
                {
                    this.listenSockets[i] = new Socket(addresses[i].AddressFamily, SocketType.Stream, ProtocolType.Tcp) { NoDelay = true };
                    this.listenSockets[i].Bind(new IPEndPoint(addresses[i], port));
                    this.listenSockets[i].Listen(20);
                }
            }

            public override void Open()
            {
                for (int i = 0; i < this.listenSockets.Length; i++)
                {
                    var task = this.AcceptAsync(this.listenSockets[i]);
                }
            }

            public override void Close()
            {
                this.closed = true;
                if (this.listenSockets != null)
                {
                    for (int i = 0; i < this.listenSockets.Length; i++)
                    {
                        if (this.listenSockets[i] != null)
                        {
                            this.listenSockets[i].Close();
                        }
                    }
                }
            }

            protected async Task HandleSocketAsync(Socket socket)
            {
                try
                {
                    if (this.Listener.tcpSettings != null)
                    {
                        this.Listener.tcpSettings.Configure(socket);
                    }

                    IAsyncTransport transport = await this.CreateTransportAsync(socket);

                    await this.Listener.HandleTransportAsync(transport);
                }
                catch (Exception exception)
                {
                    Trace.WriteLine(TraceLevel.Error, exception.ToString());
                    socket.Close();
                }
            }

            protected virtual Task<IAsyncTransport> CreateTransportAsync(Socket socket)
            {
                return Task.FromResult<IAsyncTransport>(new TcpTransport(socket));
            }

            async Task AcceptAsync(Socket socket)
            {
                while (!this.closed)
                {
                    try
                    {
                        Socket acceptSocket = await Task.Factory.FromAsync(
                            (c, s) => ((Socket)s).BeginAccept(c, s),
                            (r) => ((Socket)r.AsyncState).EndAccept(r),
                            socket);

                        var task = this.HandleSocketAsync(acceptSocket);
                    }
                    catch (ObjectDisposedException)
                    {
                        // listener is closed
                    }
                    catch (Exception exception)
                    {
                        Trace.WriteLine(TraceLevel.Warning, exception.ToString());
                    }
                }

                socket.Close();
            }
        }

        class TlsTransportListener : TcpTransportListener
        {
            readonly X509Certificate2 certificate;

            public TlsTransportListener(ConnectionListener listener, string host, int port, X509Certificate2 certificate)
                : base(listener, host, port)
            {
                this.certificate = certificate;
            }

            protected override async Task<IAsyncTransport> CreateTransportAsync(Socket socket)
            {
                SslStream sslStream;
                if (this.Listener.sslSettings == null)
                {
                    sslStream = new SslStream(new NetworkStream(socket));
                    await sslStream.AuthenticateAsServerAsync(this.certificate);
                }
                else
                {
                    sslStream = new SslStream(new NetworkStream(socket), false,
                        this.Listener.sslSettings.RemoteCertificateValidationCallback);

                    await sslStream.AuthenticateAsServerAsync(this.certificate, this.Listener.sslSettings.ClientCertificateRequired,
                        this.Listener.sslSettings.Protocols, this.Listener.sslSettings.CheckCertificateRevocation);
                }

                return new TcpTransport(sslStream);
            }
        }

        class WebSocketTransportListener : TransportListener
        {
            readonly ConnectionListener listener;
            HttpListener httpListener;

            public WebSocketTransportListener(ConnectionListener listener, string host, int port, string path, X509Certificate2 certificate)
            {
                this.listener = listener;

                // if certificate is set, it must be bound to host:port by netsh http command
                string address = string.Format("{0}://{1}:{2}{3}", certificate == null ? "http" : "https", host, port, path);
                this.httpListener = new HttpListener();
                this.httpListener.Prefixes.Add(address);
            }

            public override void Open()
            {
                this.httpListener.Start();
                var task = this.AcceptListenerContextLoop();
            }

            public override void Close()
            {
                this.closed = true;
                this.httpListener.Stop();
                this.httpListener.Close();
            }

            async Task HandleListenerContextAsync(HttpListenerContext context)
            {
                WebSocket webSocket = null;
                try
                {
                    var wsContext = await context.AcceptWebSocketAsync(WebSocketTransport.WebSocketSubProtocol);
                    var wsTransport = new WebSocketTransport(wsContext.WebSocket);
                    await this.listener.HandleTransportAsync(wsTransport);
                }
                catch(Exception exception)
                {
                    Trace.WriteLine(TraceLevel.Error, exception.ToString());
                    if (webSocket != null)
                    {
                        webSocket.Abort();
                    }
                }
            }

            async Task AcceptListenerContextLoop()
            {
                while (!this.closed)
                {
                    try
                    {
                        HttpListenerContext context = await this.httpListener.GetContextAsync();

                        var task = this.HandleListenerContextAsync(context);
                    }
                    catch (Exception exception)
                    {
                        Trace.WriteLine(TraceLevel.Error, exception.ToString());
                    }
                }
            }
        }
    }
}
