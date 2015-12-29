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

namespace Amqp
{
    using System;
    using System.Collections.Generic;
    using System.Globalization;
    using System.Net.WebSockets;
    using System.Threading;
    using System.Threading.Tasks;

    sealed class WebSocketTransport : IAsyncTransport
    {
        public const string WebSocketSubProtocol = "AMQPWSB10";
        public const string WebSockets = "WS";
        public const string SecureWebSockets = "WSS";
        const int WebSocketsPort = 80;
        const int SecureWebSocketsPort = 443;
        WebSocket webSocket;
        Queue<ByteBuffer> bufferQueue;
        bool writing;
        Connection connection;

        public WebSocketTransport()
        {
            this.bufferQueue = new Queue<ByteBuffer>();
        }

        public WebSocketTransport(WebSocket webSocket)
            : this()
        {
            this.webSocket = webSocket;
        }

        public static bool MatchScheme(string scheme)
        {
            return string.Equals(scheme, WebSockets, StringComparison.OrdinalIgnoreCase) ||
                string.Equals(scheme, SecureWebSockets, StringComparison.OrdinalIgnoreCase);
        }

        public async Task ConnectAsync(Address address)
        {
            Uri uri = new UriBuilder()
            {
                Scheme = address.Scheme,
                Port = GetDefaultPort(address.Scheme, address.Port),
                Host = address.Host,
                Path = address.Path
            }.Uri;

            ClientWebSocket cws = new ClientWebSocket();
            cws.Options.AddSubProtocol(WebSocketSubProtocol);
            await cws.ConnectAsync(uri, CancellationToken.None);
            if (cws.SubProtocol != WebSocketSubProtocol)
            {
                cws.Abort();

                throw new NotSupportedException(
                    string.Format(
                    CultureInfo.InvariantCulture,
                    "WebSocket SubProtocol used by the host is not the same that was requested: {0}",
                    cws.SubProtocol ?? "<null>"));
            }

            this.webSocket = cws;
        }

        void IAsyncTransport.SetConnection(Connection connection)
        {
            this.connection = connection;
        }

        async Task<int> IAsyncTransport.ReceiveAsync(byte[] buffer, int offset, int count)
        {
            if (this.webSocket.State != WebSocketState.Open)
            {
                return 0;
            }

            var result = await this.webSocket.ReceiveAsync(new ArraySegment<byte>(buffer, offset, count), CancellationToken.None);
            if (result.MessageType == WebSocketMessageType.Close)
            {
                return 0;
            }

            return result.Count;
        }

        bool IAsyncTransport.SendAsync(ByteBuffer buffer, IList<ArraySegment<byte>> bufferList, int listSize)
        {
            throw new InvalidOperationException();
        }

        void ITransport.Close()
        {
            this.webSocket.CloseAsync(WebSocketCloseStatus.NormalClosure, "close", CancellationToken.None).Wait();
        }

        void ITransport.Send(ByteBuffer buffer)
        {
            lock (this.bufferQueue)
            {
                if (this.writing)
                {
                    this.bufferQueue.Enqueue(buffer);
                    return;
                }

                this.writing = true;
            }

            Task task = this.WriteAsync(buffer);
        }

        int ITransport.Receive(byte[] buffer, int offset, int count)
        {
            throw new InvalidOperationException();
        }

        static int GetDefaultPort(string scheme, int port)
        {
            if (port < 0)
            {
                string temp = scheme.ToUpperInvariant();
                if (temp == WebSockets)
                {
                    port = WebSocketsPort;
                }
                else if (temp == SecureWebSockets)
                {
                    port = SecureWebSocketsPort;
                }
            }
            
            return port;
        }

        async Task WriteAsync(ByteBuffer buffer)
        {
            do
            {
                try
                {
                    await this.webSocket.SendAsync(new ArraySegment<byte>(buffer.Buffer, buffer.Offset, buffer.Length),
                        WebSocketMessageType.Binary, true, CancellationToken.None);
                }
                catch (Exception exception)
                {
                    this.connection.OnIoException(exception);
                    break;
                }

                lock (this.bufferQueue)
                {
                    if (this.bufferQueue.Count > 0)
                    {
                        buffer = this.bufferQueue.Dequeue();
                    }
                    else
                    {
                        this.writing = false;
                        buffer = null;
                    }
                }
            }
            while (buffer != null);
        }
    }
}