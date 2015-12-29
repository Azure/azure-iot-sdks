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
    using System.Runtime.InteropServices.WindowsRuntime;
    using System.Threading.Tasks;
    using Windows.Networking.Sockets;
    using Windows.Storage.Streams;

    sealed class WebSocketTransport : IAsyncTransport
    {
        public const string WebSocketSubProtocol = "AMQPWSB10";
        public const string WebSockets = "WS";
        public const string SecureWebSockets = "WSS";
        const int WebSocketsPort = 80;
        const int SecureWebSocketsPort = 443;
        StreamWebSocket webSocket;
        Queue<ByteBuffer> writeQueue;
        Connection connection;

        public WebSocketTransport()
        {
            this.writeQueue = new Queue<ByteBuffer>();
        }

        public WebSocketTransport(StreamWebSocket webSocket)
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
            StreamWebSocket sws = new StreamWebSocket();
            sws.Control.SupportedProtocols.Add(WebSocketSubProtocol);
            sws.Closed += this.OnWebSocketClosed;

            Uri uri = new UriBuilder()
            {
                Scheme = address.Scheme,
                Port = GetDefaultPort(address.Scheme, address.Port),
                Host = address.Host,
                Path = address.Path
            }.Uri;

            await sws.ConnectAsync(uri);

            this.webSocket = sws;
        }

        void OnWebSocketClosed(IWebSocket sender, WebSocketClosedEventArgs args)
        {
            if (this.webSocket != null)
            {
                this.webSocket.Dispose();
            }

            Connection temp = this.connection;
            if (temp != null)
            {
                temp.OnIoException(new ObjectDisposedException(string.Format("WebSocket closed. code={0} reason={1}", args.Code, args.Reason)));
            }
        }

        void IAsyncTransport.SetConnection(Connection connection)
        {
            this.connection = connection;
        }

        async Task<int> IAsyncTransport.ReceiveAsync(byte[] buffer, int offset, int count)
        {
            var ret = await this.webSocket.InputStream.ReadAsync(buffer.AsBuffer(offset, count), (uint)count, InputStreamOptions.None);
            if (ret.Length == 0)
            {
                throw new ObjectDisposedException(this.GetType().Name);
            }

            return (int)ret.Length;
        }

        bool IAsyncTransport.SendAsync(ByteBuffer buffer, IList<ArraySegment<byte>> bufferList, int listSize)
        {
            if (buffer != null)
            {
                this.SendInternal(buffer);
            }
            else
            {
                for (int i = 0; i < bufferList.Count; i++)
                {
                    ArraySegment<byte> segment = bufferList[i];
                    this.SendInternal(new ByteBuffer(segment.Array, segment.Offset, segment.Count, segment.Count));
                }
            }

            return true;
        }

        void ITransport.Close()
        {
            this.SendInternal(null);
        }

        void ITransport.Send(ByteBuffer buffer)
        {
            this.SendInternal(buffer);
        }

        int ITransport.Receive(byte[] buffer, int offset, int count)
        {
            return ((IAsyncTransport)this).ReceiveAsync(buffer, offset, count).Result;
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

        void SendInternal(ByteBuffer buffer)
        {
            lock (this.writeQueue)
            {
                this.writeQueue.Enqueue(buffer);
                if (this.writeQueue.Count > 1)
                {
                    return;
                }
                else if (buffer == null)
                {
                    // close
                    this.CloseWebSocket(0, "normal");
                    return;
                }
            }

            this.SendAsync(buffer);
        }

        async void SendAsync(ByteBuffer buffer)
        {
            while (buffer != null)
            {
                try
                {
                    await this.webSocket.OutputStream.WriteAsync(buffer.Buffer.AsBuffer(buffer.Offset, buffer.Length));
                }
                catch (Exception exception)
                {
                    this.connection.OnIoException(exception);
                    this.CloseWebSocket(1, "fault");
                    break;
                }

                lock (this.writeQueue)
                {
                    this.writeQueue.Dequeue();
                    if (this.writeQueue.Count == 0)
                    {
                        buffer = null;
                    }
                    else
                    {
                        buffer = this.writeQueue.Peek();
                        if (buffer == null)
                        {
                            // delayed close
                            this.CloseWebSocket(0, "normal");
                        }
                    }
                }
            }
        }

        void CloseWebSocket(ushort code, string reason)
        {
            lock (this.writeQueue)
            {
                this.writeQueue.Clear();
            }

            if (this.webSocket != null)
            {
                this.webSocket.Close(code, reason);
            }
        }
    }
}