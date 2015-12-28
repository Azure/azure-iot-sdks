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
    using Windows.Networking;
    using Windows.Networking.Sockets;
    using Windows.Storage.Streams;

    sealed class TcpTransport : IAsyncTransport
    {
        readonly Queue<ByteBuffer> writeQueue;
        Connection connection;
        StreamSocket socket;

        public TcpTransport()
        {
            this.writeQueue = new Queue<ByteBuffer>();
        }

        public void Connect(Connection connection, Address address, bool noVerification)
        {
            this.connection = connection;
            var factory = new ConnectionFactory();
            this.ConnectAsync(address, factory).GetAwaiter().GetResult();
        }

        public void SetConnection(Connection connection)
        {
            this.connection = connection;
        }

        public async Task ConnectAsync(Address address, ConnectionFactory factory)
        {
            StreamSocket ss = new StreamSocket();
            await ss.ConnectAsync(
                new HostName(address.Host),
                address.Port.ToString(),
                address.UseSsl ? SocketProtectionLevel.Tls12 : SocketProtectionLevel.PlainSocket);

            this.socket = ss;
        }

        public async Task<int> ReceiveAsync(byte[] buffer, int offset, int count)
        {
            var ret = await this.socket.InputStream.ReadAsync(buffer.AsBuffer(offset, count), (uint)count, InputStreamOptions.None);
            if (ret.Length == 0)
            {
                throw new ObjectDisposedException(this.GetType().Name);
            }

            return (int)ret.Length;
        }

        public bool SendAsync(ByteBuffer buffer, IList<ArraySegment<byte>> bufferList, int listSize)
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

        public void Close()
        {
            this.SendInternal(null);
        }

        public void Send(ByteBuffer buffer)
        {
            this.SendInternal(buffer);
        }

        public int Receive(byte[] buffer, int offset, int count)
        {
            return this.ReceiveAsync(buffer, offset, count).Result;
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
                    this.CloseSocket();
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
                    await this.socket.OutputStream.WriteAsync(buffer.Buffer.AsBuffer(buffer.Offset, buffer.Length));
                }
                catch (Exception exception)
                {
                    this.connection.OnIoException(exception);
                    this.CloseSocket();
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
                            this.CloseSocket();
                        }
                    }
                }
            }
        }

        void CloseSocket()
        {
            lock (this.writeQueue)
            {
                this.writeQueue.Clear();
            }

            if (this.socket != null)
            {
                this.socket.Dispose();
            }
        }
    }
}