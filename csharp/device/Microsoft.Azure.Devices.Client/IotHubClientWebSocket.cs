// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information. 

namespace Microsoft.Azure.Devices.Client
{
    using System;
    using System.Collections.Specialized;
    using System.Diagnostics.CodeAnalysis;
    using System.Globalization;
    using System.IO;
    using System.Net;
    using System.Net.Security;
    using System.Net.Sockets;
    using System.Security.Cryptography;
    using System.Text;
    using System.Threading;
    using System.Threading.Tasks;
    using Microsoft.Azure.Devices.Client.Common;
    using Microsoft.Azure.Devices.Client.Extensions;

    // http://tools.ietf.org/html/rfc6455
    [SuppressMessage(FxCop.Category.Design, FxCop.Rule.TypesThatOwnDisposableFieldsShouldBeDisposable, Justification = "Uses close/abort pattern")]
    class IotHubClientWebSocket
    {
        const string HttpGetHeaderFormat = "GET {0} HTTP/1.1\r\n";
        const string HttpConnectMethod = "CONNECT";
        const string Http10 = "HTTP/1.0";
        const string EndOfLineSuffix = "\r\n";
        const byte FIN = 0x80;
        const byte RSV = 0x00;
        const byte Mask = 0x80;
        const byte PayloadMask = 0x7F;
        const byte Continuation = 0x00;
        const byte Text = 0x01;
        const byte Binary = 0x02;
        const byte Close = 0x08;
        const byte Ping = 0x09;
        const byte Pong = 0x0A;
        const byte MediumSizeFrame = 126;
        const byte LargeSizeFrame = 127;

        const string HostHeaderPrefix = "Host: ";
        const string Separator = ": ";
        const string Upgrade = "Upgrade";
        const string Websocket = "websocket";
        const string ConnectionHeaderName = "Connection";
        const string FramingPrematureEOF = "More data was expected, but EOF was reached.";
        const string ClientWebSocketNotInOpenStateDuringReceive = "IotHubClientWebSocket not in Open State during Receive.";
        const string ClientWebSocketNotInOpenStateDuringSend = "IotHubClientWebSocket not in Open State during Send.";
        const string ServerRejectedUpgradeRequest = "The server rejected the upgrade request.";
        const string UpgradeProtocolNotSupported = "Protocol Type {0} was sent to a service that does not support that type of upgrade.";

        static readonly byte[] maskingKey = new byte[] { 0x00, 0x00, 0x00, 0x00 };
        static readonly SHA1CryptoServiceProvider sha1CryptoServiceProvider = InitCryptoServiceProvider();

        readonly string webSocketRole;   
        readonly string requestPath;
        string webSocketKey;
        string host;

        static class Headers
        {
            public const string SecWebSocketAccept = "Sec-WebSocket-Accept";
            public const string SecWebSocketProtocol = "Sec-WebSocket-Protocol";
            public const string SecWebSocketKey = "Sec-WebSocket-Key";
            public const string SecWebSocketVersion = "Sec-WebSocket-Version";
        }

        public IotHubClientWebSocket(string webSocketRole)
            : this(webSocketRole, WebSocketConstants.UriSuffix)
        {
        }

        public IotHubClientWebSocket(string webSocketRole, string requestPath)
        {
            this.State = WebSocketState.Initial;
            this.webSocketRole = webSocketRole;
            this.requestPath = requestPath;
        }

        public enum WebSocketMessageType
        {
            Binary,
            Close,
            Text
        }

        public enum WebSocketState
        {
            Initial,
            Connecting,
            Open,
            Closed,
            Aborted,
            Faulted
        }

        public EndPoint LocalEndpoint
        {
            get
            {
                return this.TcpClient?.Client?.LocalEndPoint;
            }
        }

        public EndPoint RemoteEndpoint
        {
            get
            {
                return this.TcpClient?.Client?.RemoteEndPoint;
            }
        }

        internal WebSocketState State { get; private set; }

        TcpClient TcpClient { get; set; }

        Stream WebSocketStream { get; set; }

        public void Abort()
        {
            if (this.State == WebSocketState.Aborted || this.State == WebSocketState.Closed || this.State == WebSocketState.Faulted)
            {
                return;
            }

            this.State = WebSocketState.Aborted;
            try
            {
                this.WebSocketStream?.Close(); // Ungraceful close

                this.WebSocketStream = null;

                this.TcpClient?.Close();

                this.TcpClient = null;
            }
            catch (Exception e)
            {
                if (Fx.IsFatal(e))
                {
                    throw;
                }

                // ignore non-fatal errors encountered during abort
                Fx.Exception.TraceHandled(e, "IotHubClientWebSocket.Abort");
            }
        }

        public async Task ConnectAsync(string host, int port, string scheme, TimeSpan timeout)
        {
            this.host = host;
            bool succeeded = false;
            try
            {
                // Connect without proxy
                this.TcpClient = new TcpClient();
                await this.TcpClient.ConnectAsync(host, port);

                if (string.Equals(WebSocketConstants.Scheme, scheme, StringComparison.OrdinalIgnoreCase))
                {
                    // In the real world, web-socket will happen over HTTPS
                    var sslStream = new SslStream(this.TcpClient.GetStream(), false, IotHubConnection.OnRemoteCertificateValidation);
                    await sslStream.AuthenticateAsClientAsync(host);
                    this.WebSocketStream = sslStream;
                }
                else
                {
                    this.WebSocketStream = this.TcpClient.GetStream();
                }

                var upgradeRequest = this.BuildUpgradeRequest();
                byte[] upgradeRequestBytes = Encoding.ASCII.GetBytes(upgradeRequest);

                this.TcpClient.Client.SendTimeout = GetSocketTimeoutInMilliSeconds(timeout);

                // Send WebSocket Upgrade request
                await this.WebSocketStream.WriteAsync(upgradeRequestBytes, 0, upgradeRequestBytes.Length);

                // receive WebSocket Upgrade response
                var responseBuffer = new byte[8 * 1024];

                var upgradeResponse = new HttpResponse(this.TcpClient, this.WebSocketStream, responseBuffer);

                await upgradeResponse.ReadAsync(timeout);

                if (upgradeResponse.StatusCode != HttpStatusCode.SwitchingProtocols)
                {
                    // the HTTP response code was not 101
                    if (this.TcpClient.Connected)
                    {
                        this.WebSocketStream.Close();
                        this.TcpClient.Close();
                    }

                    throw new IOException(ServerRejectedUpgradeRequest + " " + upgradeResponse);
                }

                if (!this.VerifyWebSocketUpgradeResponse(upgradeResponse.Headers))
                {
                    if (this.TcpClient.Connected)
                    {
                        this.WebSocketStream.Close();
                        this.TcpClient.Close();
                    }

                    throw new IOException(UpgradeProtocolNotSupported.FormatInvariant(WebSocketConstants.SubProtocols.Amqpwsb10));
                }

                this.State = WebSocketState.Open;
                succeeded = true;
            }
            finally
            {
                if (!succeeded)
                {
                    this.Abort();
                }
            }
        }

        public async Task<int> ReceiveAsync(byte[] buffer, int offset, int size, TimeSpan timeout)
        {
            byte[] header = new byte[2];

            Fx.AssertAndThrow(this.State == WebSocketState.Open, ClientWebSocketNotInOpenStateDuringReceive);
            this.TcpClient.ReceiveTimeout = TimeoutHelper.ToMilliseconds(timeout);

            bool succeeded = false;
            try
            {
                byte payloadLength;
                bool pongFrame;

                // TODO: rewrite this section to handle all control frames (including ping)
                int totalBytesRead;
                int bytesRead;
                do
                {
                    // Ignore pong frame and start over
                    totalBytesRead = 0;
                    do
                    {
                        bytesRead = await this.WebSocketStream.ReadAsync(header, totalBytesRead, header.Length - totalBytesRead);
                        if (bytesRead == 0)
                        {
                            throw new IOException(FramingPrematureEOF, new InvalidDataException("IotHubClientWebSocket was expecting more bytes"));
                        }

                        totalBytesRead += bytesRead;
                    }
                    while (totalBytesRead < header.Length);

                    if (!ParseWebSocketFrameHeader(header, out payloadLength, out pongFrame))
                    {
                        // Encountered a close frame or error in parsing frame from server. Close connection
                        var closeHeader = PrepareWebSocketHeader(0, WebSocketMessageType.Close);

                        await this.WebSocketStream.WriteAsync(closeHeader, 0, closeHeader.Length);

                        this.State = WebSocketState.Closed;
                        this.WebSocketStream?.Close();
                        this.TcpClient?.Close();
                        return 0;  // TODO: throw exception?
                    }

                    if (pongFrame && payloadLength > 0)
                    {
                        totalBytesRead = 0;
                        var tempBuffer = new byte[payloadLength];
                        while (totalBytesRead < payloadLength)
                        {
                            bytesRead = await this.WebSocketStream.ReadAsync(tempBuffer, totalBytesRead, payloadLength - totalBytesRead);
                            if (bytesRead == 0)
                            {
                                throw new IOException(FramingPrematureEOF, new InvalidDataException("IotHubClientWebSocket was expecting more bytes"));
                            }

                            totalBytesRead += bytesRead;
                        }
                    }
                }
                while (pongFrame);

                totalBytesRead = 0;

                if (buffer.Length < payloadLength)
                {
                    throw Fx.Exception.AsError(new InvalidOperationException(Resources.SizeExceedsRemainingBufferSpace));
                }

                if (payloadLength < MediumSizeFrame)
                {
                    while (totalBytesRead < payloadLength)
                    {
                        bytesRead = await this.WebSocketStream.ReadAsync(buffer, offset + totalBytesRead, payloadLength - totalBytesRead);

                        if (bytesRead == 0)
                        {
                            throw new IOException(FramingPrematureEOF, new InvalidDataException("IotHubClientWebSocket was expecting more bytes"));
                        }

                        totalBytesRead += bytesRead;
                    }
                }
                else
                {
                    switch (payloadLength)
                    {
                        case MediumSizeFrame:
                            // read payload length (< 64K)
                            do
                            {
                                bytesRead = await this.WebSocketStream.ReadAsync(header, totalBytesRead, header.Length - totalBytesRead);

                                if (bytesRead == 0)
                                {
                                    throw new IOException(FramingPrematureEOF, new InvalidDataException("IotHubClientWebSocket was expecting more bytes"));
                                }

                                totalBytesRead += bytesRead;
                            }
                            while (totalBytesRead < header.Length);

                            totalBytesRead = 0;
                            ushort extendedPayloadLength = (ushort)((header[0] << 8) | header[1]);

                            // read payload
                            if (buffer.Length >= extendedPayloadLength)
                            {
                                while (totalBytesRead < extendedPayloadLength)
                                {
                                    bytesRead = await this.WebSocketStream.ReadAsync(buffer, offset + totalBytesRead, extendedPayloadLength - totalBytesRead);

                                    if (bytesRead == 0)
                                    {
                                        throw new IOException(FramingPrematureEOF, new InvalidDataException("IotHubClientWebSocket was expecting more bytes"));
                                    }

                                    totalBytesRead += bytesRead;
                                }
                            }
                            else
                            {
                                throw Fx.Exception.AsError(new InvalidOperationException(Resources.SizeExceedsRemainingBufferSpace));
                            }

                            break;
                        case LargeSizeFrame:
                            // read payload length (>= 64K)
                            var payloadLengthBuffer = new byte[8];
                            do
                            {
                                bytesRead = await this.WebSocketStream.ReadAsync(payloadLengthBuffer, totalBytesRead, payloadLengthBuffer.Length - totalBytesRead);

                                if (bytesRead == 0)
                                {
                                    throw new IOException(FramingPrematureEOF, new InvalidDataException("IotHubClientWebSocket was expecting more bytes"));
                                }

                                totalBytesRead += bytesRead;
                            }
                            while (totalBytesRead < payloadLengthBuffer.Length);

                            totalBytesRead = 0;

                            // ignore bytes 0-3 - length cannot be larger than a 32-bit number
                            uint superExtendedPayloadLength = (uint)((payloadLengthBuffer[4] << 24) | (payloadLengthBuffer[5] << 16) | (payloadLengthBuffer[6] << 8) | payloadLengthBuffer[7]);

                            // read payload
                            if (buffer.Length >= superExtendedPayloadLength)
                            {
                                while (totalBytesRead < superExtendedPayloadLength)
                                {
                                    bytesRead = await this.WebSocketStream.ReadAsync(buffer, offset + totalBytesRead, (int)(superExtendedPayloadLength - totalBytesRead));

                                    if (bytesRead == 0)
                                    {
                                        throw new IOException(FramingPrematureEOF, new InvalidDataException("IotHubClientWebSocket was expecting more bytes"));
                                    }

                                    totalBytesRead += bytesRead;
                                }
                            }
                            else
                            {
                                throw Fx.Exception.AsError(new InvalidOperationException(Resources.SizeExceedsRemainingBufferSpace));
                            }

                            break;
                    }
                }

                succeeded = true;
                return totalBytesRead;
            }
            finally
            {
                if (!succeeded)
                {
                    this.Fault();
                }
            }
        }

        public async Task SendAsync(byte[] buffer, int offset, int size, WebSocketMessageType webSocketMessageType, TimeSpan timeout)
        {
            Fx.AssertAndThrow(this.State == WebSocketState.Open, ClientWebSocketNotInOpenStateDuringSend);
            this.TcpClient.Client.SendTimeout = TimeoutHelper.ToMilliseconds(timeout);
            bool succeeded = false;
            try
            {
                var webSocketHeader = PrepareWebSocketHeader(size, webSocketMessageType);
                await this.WebSocketStream.WriteAsync(webSocketHeader, 0, webSocketHeader.Length);
                MaskWebSocketData(buffer, offset, size);
                await this.WebSocketStream.WriteAsync(buffer, offset, size);
                succeeded = true;
            }
            finally
            {
                if (!succeeded)
                {
                    this.Fault();
                }
            }
        }

        public async Task CloseAsync()
        {
            this.State = WebSocketState.Closed;
            bool succeeded = false;
            try
            {
                if (this.TcpClient.Connected)
                {
                    var webSocketHeader = PrepareWebSocketHeader(0, WebSocketMessageType.Close);

                    await this.WebSocketStream.WriteAsync(webSocketHeader, 0, webSocketHeader.Length);

                    this.WebSocketStream?.Close();

                    this.TcpClient?.Close();
                }

                succeeded = true;
            }
            catch (Exception exception)
            {
                if (Fx.IsFatal(exception))
                {
                    throw;
                }

                // ignore exceptions during close
                Fx.Exception.TraceHandled(exception, "IotHubClientWebSocket.CloseAsync");
            }
            finally
            {
                if (!succeeded)
                {
                    this.Fault();
                }
            }
        }

        [SuppressMessage("Microsoft.Cryptographic.Standard", "CA5354:SHA1CannotBeUsed", Justification = "SHA-1 Hash mandated by RFC 6455")]
        static SHA1CryptoServiceProvider InitCryptoServiceProvider()
        {
            return new SHA1CryptoServiceProvider();
        }

        // Socket.ReceiveTimeout/SendTimeout 0 means infinite/no-timeout. When dealing with cascading timeouts
        // if the remaining time reaches TimeSpan.Zero we don't want to turn off timeouts on the socket, instead
        // we want to use a very small timeout.
        static int GetSocketTimeoutInMilliSeconds(TimeSpan timeout)
        {
            if (timeout == TimeSpan.MaxValue)
            {
                return Timeout.Infinite;
            }

            if (timeout == TimeSpan.Zero)
            {
                // Socket.ReceiveTimeout/SendTimeout 0 means no timeout which is not what we want.
                // Use a small number instead
                return 1;
            }

            long ticks = Ticks.FromTimeSpan(timeout);
            if (ticks / TimeSpan.TicksPerMillisecond > int.MaxValue)
            {
                return int.MaxValue;
            }
            return Ticks.ToMilliseconds(ticks);
        }

        static byte[] PrepareWebSocketHeader(int bufferLength, WebSocketMessageType webSocketMessageType)
        {
            byte[] octet;

            if (bufferLength < MediumSizeFrame)
            {
                // Handle small payloads and control frames
                octet = new byte[6];

                // Octet0
                octet[0] = PrepareOctet0(webSocketMessageType);

                // Octet 1
                octet[1] = (byte)(bufferLength | Mask);

                // Octets 2-5 (Masking Key)
                octet[2] = maskingKey[0];
                octet[3] = maskingKey[1];
                octet[4] = maskingKey[2];
                octet[5] = maskingKey[3];
            }
            else if (bufferLength <= UInt16.MaxValue)
            {
                // Handle medium payloads
                octet = new byte[8];

                // Octet 0
                octet[0] = PrepareOctet0(webSocketMessageType);

                // Octet 1
                octet[1] = MediumSizeFrame | Mask;

                // Octet 2-3 Payload Length
                octet[2] = (byte)((bufferLength >> 8) & 0x00FF);
                octet[3] = (byte)(bufferLength & 0x00FF);

                // Octets 4-7 (Masking Key)
                octet[4] = maskingKey[0];
                octet[5] = maskingKey[1];
                octet[6] = maskingKey[2];
                octet[7] = maskingKey[3];
            }
            else
            {
                // Handle large payloads
                octet = new byte[14];

                // Octet 0
                octet[0] = PrepareOctet0(webSocketMessageType);

                // Octet 1
                octet[1] = LargeSizeFrame | Mask;

                // Octet 2-9 Payload Length

                // ignore anything larger than a 32-bit number
                // octet[2] = octet[3] = octet[4] = octet[5] = 0; These are already set to 0
                octet[6] = (byte)((bufferLength >> 24) & 0x00FF);
                octet[7] = (byte)((bufferLength >> 16) & 0x00FF);
                octet[8] = (byte)((bufferLength >> 8) & 0x00FF);
                octet[9] = (byte)(bufferLength & 0x00FF);

                // Octets 10-13 (Masking Key)
                octet[10] = maskingKey[0];
                octet[11] = maskingKey[1];
                octet[12] = maskingKey[2];
                octet[13] = maskingKey[3];
            }

            return octet;
        }

        static byte PrepareOctet0(WebSocketMessageType webSocketMessageType)
        {
            byte octet0 = FIN | RSV;
            if (webSocketMessageType.Equals(WebSocketMessageType.Binary))
            {
                octet0 |= Binary;
            }
            else if (webSocketMessageType.Equals(WebSocketMessageType.Text))
            {
                octet0 |= Text;
            }
            else
            {
                octet0 |= Close;
            }

            return octet0;
        }

        static void MaskWebSocketData(byte[] buffer, int offset, int size)
        {
            Utils.ValidateBufferBounds(buffer, offset, size);

            for (int i = 0; i < size; i++)
            {
                buffer[i + offset] ^= maskingKey[i % 4];
            }
        }

        static bool ParseWebSocketFrameHeader(byte[] buffer, out byte payloadLength, out bool pongFrame)
        {
            payloadLength = 0;
            bool finalFragment;
            var fin = buffer[0] & FIN;
            if (fin == FIN)
            {
                // this is the final fragment
                finalFragment = true;
            }
            else
            {
                // TODO add fragmented message support
                throw Fx.Exception.AsError(new NotImplementedException("Client Websocket implementation lacks fragmentation support"));
            }

            // TODO: check RSV?
            var opcode = buffer[0] & 0x0F;

            pongFrame = false;

            switch (opcode)
            {
                case Continuation:
                    {
                        if (finalFragment)
                        {
                            return false; // This is a protocol violation. A final frame cannot also be a continuation frame
                        }

                        break;
                    }

                case Text:
                case Binary:
                    // WebSocket implementation can handle both text and binary messages
                    break;
                case Close:
                    return false;   // Close frame received - We can close the connection
                case Ping:
                    throw Fx.Exception.AsError(new NotImplementedException("Client Websocket implementation lacks ping message support"));

                // break;
                case Pong:
                    pongFrame = true;
                    break;
                default:
                    return false;
            }

            var mask = buffer[1] & Mask;
            if (mask == Mask)
            {
                // This is an error. We received a masked frame from server - Close connection as per RFC 6455
                return false;
            }

            payloadLength = (byte)(buffer[1] & PayloadMask);
            return true;
        }

        void Fault()
        {
            this.State = WebSocketState.Faulted;
            if (this.WebSocketStream != null)
            {
                this.WebSocketStream.Close();   // Ungraceful close
                this.WebSocketStream = null;
            }

            if (this.TcpClient != null)
            {
                this.TcpClient.Close();
                this.TcpClient = null;
            }
        }

        bool VerifyWebSocketUpgradeResponse(NameValueCollection webSocketHeaders)
        {
            // verify that Upgrade header is present with a value of websocket
            string upgradeHeaderValue;
            if (null == (upgradeHeaderValue = webSocketHeaders.Get(Upgrade)))
            {
                // Server did not respond with an upgrade header
                return false;
            }

            if (!string.Equals(upgradeHeaderValue, Websocket, StringComparison.OrdinalIgnoreCase))
            {
                // Server did not include the string websocket in the upgrade header
                return false;
            }

            // verify connection header is present with a value of Upgrade
            string connectionHeaderValue;
            if (null == (connectionHeaderValue = webSocketHeaders.Get(ConnectionHeaderName)))
            {
                // Server did not respond with an connection header
                return false;
            }

            if (!string.Equals(connectionHeaderValue, Upgrade, StringComparison.OrdinalIgnoreCase))
            {
                // Server did not include the string upgrade in the connection header
                return false;
            }

            // verify that a SecWebSocketAccept header is present with appropriate hash value string
            string secWebSocketAcceptHeaderValue;
            if (null == (secWebSocketAcceptHeaderValue = webSocketHeaders.Get(Headers.SecWebSocketAccept)))
            {
                // Server did not include the SecWebSocketAcceptHeader in the response
                return false;
            }

            if (!ComputeHash(this.webSocketKey).Equals(secWebSocketAcceptHeaderValue, StringComparison.Ordinal))
            {
                // Server Hash Value of Client's Nonce was invalid
                return false;
            }

            if (!string.IsNullOrEmpty(this.webSocketRole))
            {
                // verify SecWebSocketProtocol contents
                string secWebSocketProtocolHeaderValue;
                if (null != (secWebSocketProtocolHeaderValue = webSocketHeaders.Get(Headers.SecWebSocketProtocol)))
                {
                    // Check SecWebSocketProtocolHeader with requested protocol
                    if (!this.webSocketRole.Equals(secWebSocketProtocolHeaderValue))
                    {
                        return false;
                    }
                }
                else
                {
                    return false;
                }
            }

            return true;
        }

        string BuildUpgradeRequest()
        {
            this.webSocketKey = Convert.ToBase64String(Guid.NewGuid().ToByteArray());
            var sb = new StringBuilder();

            // GET {0} HTTP/1.1\r\n
            sb.AppendFormat(HttpGetHeaderFormat, this.requestPath);

            // Setup Host Header
            sb.Append(HostHeaderPrefix).Append(this.host).Append(EndOfLineSuffix);

            // Setup Upgrade Header
            sb.Append(Upgrade).Append(Separator).Append(Websocket).Append(EndOfLineSuffix);

            // Setup Connection Header
            sb.Append(ConnectionHeaderName).Append(Separator).Append(Upgrade).Append(EndOfLineSuffix);

            // Setup SecWebSocketKey Header
            sb.Append(Headers.SecWebSocketKey)
              .Append(Separator)
              .Append(this.webSocketKey)
              .Append(EndOfLineSuffix);

            if (!string.IsNullOrEmpty(this.webSocketRole))
            {
                // Setup SecWebSocketProtocol Header
                sb.Append(Headers.SecWebSocketProtocol)
                    .Append(Separator)
                    .Append(this.webSocketRole)
                    .Append(EndOfLineSuffix);
            }

            // Setup SecWebSocketVersion Header
            sb.Append(Headers.SecWebSocketVersion)
              .Append(Separator)
              .Append(WebSocketConstants.Version)
              .Append(EndOfLineSuffix);

            // Add an extra EndOfLine at the end
            sb.Append(EndOfLineSuffix);

            return sb.ToString();
        }

        static string ComputeHash(string key)
        {
            const string WebSocketGuid = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

            var modifiedString = key + WebSocketGuid;
            var modifiedStringBytes = Encoding.ASCII.GetBytes(modifiedString);

            byte[] hashBytes;
            lock (sha1CryptoServiceProvider)
            {
                hashBytes = sha1CryptoServiceProvider.ComputeHash(modifiedStringBytes);
            }

            return Convert.ToBase64String(hashBytes);
        }

        class HttpResponse
        {
            int bodyStartIndex;

            public HttpResponse(TcpClient tcpClient, Stream stream, byte[] buffer)
            {
                this.TcpClient = tcpClient;
                this.Stream = stream;
                this.Buffer = buffer;
            }

            TcpClient TcpClient { get; set; }

            Stream Stream { get; set; }

            byte[] Buffer { get; set; }

            int TotalBytesRead { get; set; }

            int bytesRead;

            public async Task ReadAsync(TimeSpan timeout)
            {
                TimeoutHelper timeoutHelper = new TimeoutHelper(timeout);
                do
                {
                    this.TcpClient.Client.ReceiveTimeout = GetSocketTimeoutInMilliSeconds(timeoutHelper.RemainingTime());
                    this.bytesRead = 0;

                    this.bytesRead = await this.Stream.ReadAsync(this.Buffer, this.TotalBytesRead, this.Buffer.Length - this.TotalBytesRead);

                    this.TotalBytesRead += this.bytesRead;
                    if (this.bytesRead == 0 || this.TryParseBuffer())
                    {
                        // exit the do/while loop
                        break;
                    }
                }
                while (true);

                if (this.TotalBytesRead == 0)
                {
                    var socketException = new SocketException((int)SocketError.ConnectionRefused);
                    throw Fx.Exception.AsWarning(new IOException(socketException.Message, socketException));
                }
            }

            // Commented out to avoid FxCop error for uncalled private code
            ////public string HttpVersion { get; private set; }

            public HttpStatusCode StatusCode { get; private set; }

            public string StatusDescription { get; private set; }

            public WebHeaderCollection Headers { get; private set; }

            public override string ToString()
            {
                // return a string like "407 Proxy Auth Required"
                return (int)this.StatusCode + " " + this.StatusDescription;
            }

            /// <summary>
            /// Parse the bytes received so far.
            /// If possible:
            ///    -Parse the Status line
            ///    -Parse the HTTP Headers
            ///    -if HTTP Headers Content-Length is present do we have that much content received?
            /// If all the above succeed then this method returns true, otherwise false (need to receive more data from network stream).
            /// </summary>
            bool TryParseBuffer()
            {
                if (this.bodyStartIndex == 0)
                {
                    int firstSpace = IndexOfAsciiChar(this.Buffer, 0, this.TotalBytesRead, ' ');
                    if (firstSpace == -1)
                    {
                        return false;
                    }

                    ////this.HttpVersion = Encoding.ASCII.GetString(array, arraySegment.Offset, firstSpace - arraySegment.Offset);
                    int secondSpace = IndexOfAsciiChar(this.Buffer, firstSpace + 1, this.TotalBytesRead - (firstSpace + 1), ' ');
                    if (secondSpace == -1)
                    {
                        return false;
                    }

                    var statusCodeString = Encoding.ASCII.GetString(this.Buffer, firstSpace + 1, secondSpace - (firstSpace + 1));
                    this.StatusCode = (HttpStatusCode)int.Parse(statusCodeString, CultureInfo.InvariantCulture);
                    int endOfLine = IndexOfAsciiChars(this.Buffer, secondSpace + 1, this.TotalBytesRead - (secondSpace + 1), '\r', '\n');
                    if (endOfLine == -1)
                    {
                        return false;
                    }

                    this.StatusDescription = Encoding.ASCII.GetString(this.Buffer, secondSpace + 1, endOfLine - (secondSpace + 1));

                    // Now parse the headers
                    this.Headers = new WebHeaderCollection();
                    while (true)
                    {
                        int startCurrentLine = endOfLine + 2;
                        if (startCurrentLine >= this.TotalBytesRead)
                        {
                            return false;
                        }
                        else if (this.Buffer[startCurrentLine] == '\r' && this.Buffer[startCurrentLine + 1] == '\n')
                        {
                            // \r\n\r\n indicates the end of the HTTP headers.
                            this.bodyStartIndex = startCurrentLine + 2;
                            break;
                        }

                        int separatorIndex = IndexOfAsciiChars(this.Buffer, startCurrentLine, this.TotalBytesRead - startCurrentLine, ':', ' ');
                        if (separatorIndex == -1)
                        {
                            return false;
                        }

                        string headerName = Encoding.ASCII.GetString(this.Buffer, startCurrentLine, separatorIndex - startCurrentLine);
                        endOfLine = IndexOfAsciiChars(this.Buffer, separatorIndex + 2, this.TotalBytesRead - (separatorIndex + 2), '\r', '\n');
                        if (endOfLine == -1)
                        {
                            return false;
                        }

                        string headerValue = Encoding.ASCII.GetString(this.Buffer, separatorIndex + 2, endOfLine - (separatorIndex + 2));
                        this.Headers.Add(headerName, headerValue);
                    }
                }

                // check to see if all the body bytes have been received.
                string contentLengthValue = this.Headers[HttpResponseHeader.ContentLength];
                if (!string.IsNullOrEmpty(contentLengthValue) && contentLengthValue != "0")
                {
                    int contentLength = int.Parse(contentLengthValue, CultureInfo.InvariantCulture);
                    if (contentLength > this.TotalBytesRead - this.bodyStartIndex)
                    {
                        return false;
                    }
                }

                return true;
            }
        }

        public static int IndexOfAsciiChar(byte[] array, int offset, int count, char asciiChar)
        {
            Fx.Assert(asciiChar <= byte.MaxValue, "asciiChar isn't valid ASCII!");
            Fx.Assert(offset + count <= array.Length, "offset + count > array.Length!");

            for (int i = offset; i < offset + count; i++)
            {
                if (array[i] == asciiChar)
                {
                    return i;
                }
            }

            return -1;
        }

        /// <summary>
        /// Check if the given buffer contains the 2 specified ascii characters (in sequence) without having to allocate or convert byte[] into string
        /// </summary>
        public static int IndexOfAsciiChars(byte[] array, int offset, int count, char asciiChar1, char asciiChar2)
        {
            Fx.Assert(asciiChar1 <= byte.MaxValue, "asciiChar1 isn't valid ASCII!");
            Fx.Assert(asciiChar2 <= byte.MaxValue, "asciiChar2 isn't valid ASCII!");
            Fx.Assert(offset + count <= array.Length, "offset + count > array.Length!");

            for (int i = offset; i < offset + count - 1; i++)
            {
                if (array[i] == asciiChar1 && array[i + 1] == asciiChar2)
                {
                    return i;
                }
            }

            return -1;
        }
    }
}