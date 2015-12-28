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
    using System.Net;
    using System.Net.Security;
    using System.Net.Sockets;
    using System.Threading.Tasks;

    sealed class TcpTransport : IAsyncTransport
    {
        static readonly RemoteCertificateValidationCallback noneCertValidator = (a, b, c, d) => true;
        Connection connection;
        Writer writer;
        IAsyncTransport socketTransport;

        public TcpTransport()
        {
        }

        // called by listener
        public TcpTransport(Socket socket)
        {
            this.socketTransport = new TcpSocket(this, socket);
            this.writer = new Writer(this, this.socketTransport);
        }

        // called by listener
        public TcpTransport(SslStream sslStream)
        {
            this.socketTransport = new SslSocket(this, sslStream);
            this.writer = new Writer(this, this.socketTransport);
        }

        public void Connect(Connection connection, Address address, bool noVerification)
        {
            this.connection = connection;
            var factory = new ConnectionFactory();
            if (noVerification)
            {
                factory.SSL.RemoteCertificateValidationCallback = noneCertValidator;
            }

            this.ConnectAsync(address, factory).GetAwaiter().GetResult();
        }

        public async Task ConnectAsync(Address address, ConnectionFactory factory)
        {
            Socket socket;
            IPAddress[] ipAddresses;
            IPAddress ipAddress;
            if (IPAddress.TryParse(address.Host, out ipAddress))
            {
                socket = new Socket(ipAddress.AddressFamily, SocketType.Stream, ProtocolType.Tcp);
                ipAddresses = new IPAddress[] { ipAddress };
            }
            else
            {
                socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
                ipAddresses = Dns.GetHostEntry(address.Host).AddressList;
            }

            if (factory.tcpSettings != null)
            {
                factory.tcpSettings.Configure(socket);
            }

            await Task.Factory.FromAsync(
                (c, s) => ((Socket)s).BeginConnect(ipAddresses, address.Port, c, s),
                (r) => ((Socket)r.AsyncState).EndConnect(r),
                socket);

            IAsyncTransport transport;
            if (address.UseSsl)
            {
                SslStream sslStream;
                var ssl = factory.SslInternal;
                if (ssl == null)
                {
                    sslStream = new SslStream(new NetworkStream(socket));
                    await sslStream.AuthenticateAsClientAsync(address.Host);
                }
                else
                {
                    sslStream = new SslStream(new NetworkStream(socket), false, ssl.RemoteCertificateValidationCallback);
                    await sslStream.AuthenticateAsClientAsync(address.Host, ssl.ClientCertificates,
                        ssl.Protocols, ssl.CheckCertificateRevocation);
                }

                transport = new SslSocket(this, sslStream);
            }
            else
            {
                transport = new TcpSocket(this, socket);
            }

            this.socketTransport = transport;
            this.writer = new Writer(this, this.socketTransport);
        }

        void IAsyncTransport.SetConnection(Connection connection)
        {
            this.connection = connection;
        }

        async Task<int> IAsyncTransport.ReceiveAsync(byte[] buffer, int offset, int count)
        {
            int received = await this.socketTransport.ReceiveAsync(buffer, offset, count);
            if (received == 0)
            {
                throw new SocketException((int)SocketError.ConnectionReset);
            }

            return received;
        }

        bool IAsyncTransport.SendAsync(ByteBuffer buffer, IList<ArraySegment<byte>> bufferList, int listSize)
        {
            throw new InvalidOperationException();
        }

        void ITransport.Close()
        {
            this.socketTransport.Close();
        }

        void ITransport.Send(ByteBuffer buffer)
        {
            this.writer.Write(buffer);
        }

        int ITransport.Receive(byte[] buffer, int offset, int count)
        {
            int received = this.socketTransport.Receive(buffer, offset, count);
            if (received == 0)
            {
                throw new SocketException((int)SocketError.ConnectionReset);
            }

            return received;
        }

        class TcpSocket : IAsyncTransport
        {
            readonly static EventHandler<SocketAsyncEventArgs> onWriteComplete = OnWriteComplete;
            readonly TcpTransport transport;
            readonly Socket socket;
            readonly SocketAsyncEventArgs args;
            readonly IopsTracker receiveTracker;
            ByteBuffer receiveBuffer;

            public TcpSocket(TcpTransport transport, Socket socket)
            {
                this.transport = transport;
                this.socket = socket;
                this.receiveTracker = new IopsTracker();
                this.args = new SocketAsyncEventArgs();
                this.args.Completed += onWriteComplete;
                this.args.UserToken = this;
            }

            static void OnWriteComplete(object sender, SocketAsyncEventArgs eventArgs)
            {
                var thisPtr = (TcpSocket)eventArgs.UserToken;
                if (eventArgs.SocketError != SocketError.Success)
                {
                    thisPtr.transport.connection.OnIoException(new SocketException((int)eventArgs.SocketError));
                }
                else
                {
                    thisPtr.transport.writer.ContinueWrite();
                }
            }

            void IAsyncTransport.SetConnection(Connection connection)
            {
                throw new NotImplementedException();
            }

            bool IAsyncTransport.SendAsync(ByteBuffer buffer, IList<ArraySegment<byte>> bufferList, int listSize)
            {
                if (buffer != null)
                {
                    this.args.BufferList = null;
                    this.args.SetBuffer(buffer.Buffer, buffer.Offset, buffer.Length);
                }
                else
                {
                    this.args.SetBuffer(null, 0, 0);
                    this.args.BufferList = bufferList;
                }

                return this.socket.SendAsync(this.args);
            }

            async Task<int> IAsyncTransport.ReceiveAsync(byte[] buffer, int offset, int count)
            {
                if (this.receiveBuffer != null && this.receiveBuffer.Length > 0)
                {
                    return ReceiveFromBuffer(this.receiveBuffer, buffer, offset, count);
                }

                if (this.receiveTracker.TrackOne())
                {
                    if (this.receiveTracker.Level > 0)
                    {
                        this.receiveBuffer = new ByteBuffer(8 * 1024, false);
                    }
                    else
                    {
                        this.receiveBuffer = null;
                    }
                }

                if (this.receiveBuffer == null)
                {
                    return await Task.Factory.FromAsync(
                        (c, s) => ((TcpSocket)s).socket.BeginReceive(buffer, offset, count, SocketFlags.None, c, s),
                        (r) => ((TcpSocket)r.AsyncState).socket.EndReceive(r),
                        this);
                }

                int bytes = await Task.Factory.FromAsync(
                    (c, s) =>
                    {
                        var thisPtr = (TcpSocket)s;
                        return thisPtr.socket.BeginReceive(thisPtr.receiveBuffer.Buffer, 0, thisPtr.receiveBuffer.Size, SocketFlags.None, c, s);
                    },
                    (r) => ((TcpSocket)r.AsyncState).socket.EndReceive(r),
                    this);

                this.receiveBuffer.Append(bytes);
                return ReceiveFromBuffer(this.receiveBuffer, buffer, offset, count);
            }

            void ITransport.Send(ByteBuffer buffer)
            {
                throw new InvalidOperationException();
            }

            int ITransport.Receive(byte[] buffer, int offset, int count)
            {
                if (this.receiveBuffer != null && this.receiveBuffer.Length > 0)
                {
                    return ReceiveFromBuffer(this.receiveBuffer, buffer, offset, count);
                }

                if (this.receiveTracker.TrackOne())
                {
                    if (this.receiveTracker.Level > 0)
                    {
                        this.receiveBuffer = new ByteBuffer(8 * 1024, false);
                    }
                    else
                    {
                        this.receiveBuffer = null;
                    }
                }

                if (this.receiveBuffer == null)
                {
                    return this.socket.Receive(buffer, offset, count, SocketFlags.None);
                }

                int bytes = this.socket.Receive(this.receiveBuffer.Buffer, this.receiveBuffer.Offset, this.receiveBuffer.Size, SocketFlags.None);
                this.receiveBuffer.Append(bytes);
                return ReceiveFromBuffer(this.receiveBuffer, buffer, offset, count);
            }

            static int ReceiveFromBuffer(ByteBuffer byteBuffer, byte[] buffer, int offset, int count)
            {
                int len = byteBuffer.Length;
                if (len <= count)
                {
                    Buffer.BlockCopy(byteBuffer.Buffer, byteBuffer.Offset, buffer, offset, len);
                    byteBuffer.Reset();
                    return len;
                }
                else
                {
                    Buffer.BlockCopy(byteBuffer.Buffer, byteBuffer.Offset, buffer, offset, count);
                    byteBuffer.Complete(count);
                    return count;
                }
            }

            void ITransport.Close()
            {
                this.socket.Close();
                this.args.Dispose();
            }
        }

        class SslSocket : IAsyncTransport
        {
            readonly TcpTransport transport;
            readonly SslStream sslStream;

            public SslSocket(TcpTransport transport, SslStream sslStream)
            {
                this.transport = transport;
                this.sslStream = sslStream;
            }

            void IAsyncTransport.SetConnection(Connection connection)
            {
                throw new NotImplementedException();
            }

            bool IAsyncTransport.SendAsync(ByteBuffer buffer, IList<ArraySegment<byte>> bufferList, int listSize)
            {
                ArraySegment<byte> writeBuffer;
                if (buffer != null)
                {
                    writeBuffer = new ArraySegment<byte>(buffer.Buffer, buffer.Offset, buffer.Length);
                }
                else
                {
                    byte[] temp = new byte[listSize];
                    int offset = 0;
                    for (int i = 0; i < bufferList.Count; i++)
                    {
                        ArraySegment<byte> segment = bufferList[i];
                        Buffer.BlockCopy(segment.Array, segment.Offset, temp, offset, segment.Count);
                        offset += segment.Count;
                    }

                    writeBuffer = new ArraySegment<byte>(temp, 0, listSize);
                }

                Task task = this.sslStream.WriteAsync(writeBuffer.Array, writeBuffer.Offset, writeBuffer.Count);
                bool pending = !task.IsCompleted;
                if (pending)
                {
                    task.ContinueWith(
                        (t, s) =>
                        {
                            var thisPtr = (SslSocket)s;
                            if (t.IsFaulted)
                            {
                                thisPtr.transport.connection.OnIoException(t.Exception.InnerException);
                            }
                            else
                            {
                                thisPtr.transport.writer.ContinueWrite();
                            }
                        },
                        this);
                }

                return pending;
            }

            Task<int> IAsyncTransport.ReceiveAsync(byte[] buffer, int offset, int count)
            {
                return this.sslStream.ReadAsync(buffer, offset, count);
            }

            void ITransport.Send(ByteBuffer buffer)
            {
                throw new InvalidOperationException();
            }

            int ITransport.Receive(byte[] buffer, int offset, int count)
            {
                return this.sslStream.Read(buffer, offset, count);
            }

            void ITransport.Close()
            {
                this.sslStream.Close();
            }
        }

        sealed class Writer
        {
            readonly TcpTransport owner;
            readonly IAsyncTransport transport;
            Queue<ByteBuffer> bufferQueue;
            bool writing;

            public Writer(TcpTransport owner, IAsyncTransport transport)
            {
                this.owner = owner;
                this.transport = transport;
                this.bufferQueue = new Queue<ByteBuffer>();
            }

            public void Write(ByteBuffer buffer)
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

                if (!this.transport.SendAsync(buffer, null, 0))
                {
                    this.ContinueWrite();
                }
            }

            public void ContinueWrite()
            {
                ByteBuffer buffer = null;
                IList<ArraySegment<byte>> buffers = null;
                int listSize = 0;
                do
                {
                    lock (this.bufferQueue)
                    {
                        int queueDepth = this.bufferQueue.Count;
                        if (queueDepth == 0)
                        {
                            this.writing = false;
                            return;
                        }
                        else if (queueDepth == 1)
                        {
                            buffer = this.bufferQueue.Dequeue();
                            buffers = null;
                        }
                        else
                        {
                            buffer = null;
                            listSize = 0;
                            buffers = new ArraySegment<byte>[queueDepth];
                            for (int i = 0; i < queueDepth; i++)
                            {
                                ByteBuffer item = this.bufferQueue.Dequeue();
                                buffers[i] = new ArraySegment<byte>(item.Buffer, item.Offset, item.Length);
                                listSize += item.Length;
                            }
                        }
                    }

                    try
                    {
                        if (this.transport.SendAsync(buffer, buffers, listSize))
                        {
                            break;
                        }
                    }
                    catch (Exception exception)
                    {
                        this.owner.connection.OnIoException(exception);
                        break;
                    }
                }
                while (true);
            }
        }

        class IopsTracker
        {
            const long WindowTicks = 6 * 1000 * 10000;
            static int[] thresholds = new int[] { 6000, 60000 };
            DateTime windowStart;
            int iops;
            byte level0;
            byte level1;

            public IopsTracker()
            {
                this.windowStart = DateTime.UtcNow;
            }

            public byte Level
            {
                get { return this.level0; }
            }

            public bool TrackOne()
            {
                this.iops++;
                var now = DateTime.UtcNow;
                bool changed = false;
                if (now.Ticks - this.windowStart.Ticks >= WindowTicks)
                {
                    int i = thresholds.Length;
                    while (i >= 1 && this.iops < thresholds[i - 1])
                    {
                        i--;
                    }

                    byte level = (byte)i;
                    if (this.level1 > this.level0)
                    {
                        if (level > this.level0)
                        {
                            this.level0 = Math.Min(level, this.level1);
                            changed = true;
                        }
                    }
                    else if (this.level1 < this.level0)
                    {
                        if (level < this.level0)
                        {
                            this.level0 = Math.Max(level0, this.level1);
                            changed = true;
                        }
                    }

                    this.level1 = level;
                    this.iops = 0;
                    this.windowStart = now;
                }

                return changed;
            }
        }
    }
}