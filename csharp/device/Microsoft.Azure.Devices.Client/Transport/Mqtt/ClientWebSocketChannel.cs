// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client.Transport.Mqtt
{
    using System;
    using System.Diagnostics.Contracts;
    using System.Net;
    using System.Net.WebSockets;
    using System.Threading;
    using System.Threading.Tasks;
    using DotNetty.Buffers;
    using DotNetty.Transport.Channels;
    using Microsoft.Azure.Devices.Client.Extensions;

    public class ClientWebSocketChannel : AbstractChannel
    {
        readonly ClientWebSocket webSocket;
        bool active;
        volatile CancellationTokenSource writeCancellationTokenSource;

        internal bool ReadPending { get; set; }
   
        internal bool WriteInProgress { get; set; }

        public ClientWebSocketChannel(IChannel parent, ClientWebSocket webSocket)
            : base(parent)
        {
            this.webSocket = webSocket;
            this.Metadata = new ChannelMetadata(false, 1);
            this.Configuration = new ClientWebSocketChannelConfig();
            this.active = true;
            this.writeCancellationTokenSource = new CancellationTokenSource();
        }

        public override IChannelConfiguration Configuration { get; }

        public override bool Open => this.webSocket.State == WebSocketState.Open;

        public override bool Active => this.active;

        public override ChannelMetadata Metadata { get; }

        public ClientWebSocketChannel Option<T>(ChannelOption<T> option, T value)
        {
            Contract.Requires(option != null);

            this.Configuration.SetOption(option, value);
            return this;
        }

        protected override EndPoint LocalAddressInternal { get; }

        protected override EndPoint RemoteAddressInternal { get; }

        protected override IChannelUnsafe NewUnsafe() => new WebSocketChannelUnsafe(this);

        protected class WebSocketChannelUnsafe : AbstractUnsafe
        {
            public WebSocketChannelUnsafe(AbstractChannel channel)
                :base(channel)
            {
            }

            public override async Task ConnectAsync(EndPoint remoteAddress, EndPoint localAddress)
            {
                 throw new NotImplementedException();

                //this.webSocket = new ClientWebSocket();
                //this.webSocket.Options.AddSubProtocol(this.subProtocol);
                //// Uri uri = new Uri("ws://" + IotHubName + ":" + Port + "/$iothub/websocket");
                //var uri = new Uri("ws://" + ((IPEndPoint)remoteAddress).Address + ":" + ((IPEndPoint)remoteAddress).Port + "/$iothub/websocket");
                //await ((ClientWebSocketChannel)this.channel).webSocket.ConnectAsync(uri, CancellationToken.None);
                //((ClientWebSocketChannel)this.channel).Configuration.AutoRead = true;
            }

            protected override void Flush0()
            {
                // Flush immediately only when there's no pending flush.
                // If there's a pending flush operation, event loop will call FinishWrite() later,
                // and thus there's no need to call it now.
                if (((ClientWebSocketChannel)this.channel).WriteInProgress)
                {
                    return;
                }

                base.Flush0();
            }
        }

        protected override bool IsCompatible(IEventLoop eventLoop) => true;


        protected override void DoBind(EndPoint localAddress)
        {
            throw new NotImplementedException();
        }

        protected override void DoDisconnect()
        {
            if (this.Metadata.HasDisconnect)
            {
                this.DoClose();
            }
            else
            {
                throw new NotSupportedException("ClientWebSocketChannel does not support DoDisconnect()");
            }
        }

        protected override async void DoClose()
        {
            try
            {
                // Cancel any pending write
                this.CancelPendingWrite();
                this.active = false;

                using (var cancellationTokenSource = new CancellationTokenSource(TimeSpan.FromSeconds(30)))
                {
                    await this.webSocket.CloseAsync(WebSocketCloseStatus.NormalClosure, string.Empty, cancellationTokenSource.Token);
                }
            }
            catch (Exception e)  when (!e.IsFatal())
            {
                this.Abort();
            }
        }

        protected override async void DoBeginRead()
        {
            if (!this.Open)
            {
                return;
            }

            if (this.ReadPending)
            {
                return;
            }

            this.ReadPending = true;
            IByteBufferAllocator allocator = this.Configuration.Allocator;
            IRecvByteBufAllocatorHandle allocHandle = this.Configuration.RecvByteBufAllocator.NewHandle();
            allocHandle.Reset(this.Configuration);
            do
            {
                IByteBuffer byteBuf = allocHandle.Allocate(allocator);
                allocHandle.LastBytesRead = await this.DoReadBytes(byteBuf, allocHandle);
                if (allocHandle.LastBytesRead <= 0)
                {
                    // nothing was read -> release the buffer.
                    byteBuf.Release();
                    break;
                }

                this.Pipeline.FireChannelRead(byteBuf);

                allocHandle.IncMessagesRead(1);
                this.ReadPending = false;
            }
            while (allocHandle.ContinueReading());

            allocHandle.ReadComplete();
            this.Pipeline.FireChannelReadComplete();
        }

        protected override async void DoWrite(ChannelOutboundBuffer input)
        {
            this.ThrowIfNotOpen();

            this.WriteInProgress = true;

            while (true)
            {
                object msg = input.Current;
                if (msg == null)
                {
                    // Wrote all messages.
                    break;
                }

                var buf = msg as IByteBuffer;
                if (buf != null)
                {
                    int readableBytes = buf.ReadableBytes;
                    if (readableBytes == 0)
                    {
                        input.Remove();
                        continue;
                    }

                    await this.DoWriteBytes(buf, input);
                }
                else
                {
                    // TODO: throw exception?
                }
            }

            this.WriteInProgress = false;
        }

        /// <returns>the amount of written bytes</returns>
        async Task DoWriteBytes(IByteBuffer buf, ChannelOutboundBuffer outboundBuffer)
        {
            bool success = false;
            try
            {
                await this.webSocket.SendAsync(buf.GetIoBuffer(), WebSocketMessageType.Binary, true, this.writeCancellationTokenSource.Token);
                outboundBuffer.Remove();
                success = true;
            }
            catch (Exception e)  when (!e.IsFatal())
            {
                this.Pipeline.FireExceptionCaught(e);
                throw;
            }
            finally
            {
                if (!success)
                {
                    this.Abort();
                }
            }
        }

        async Task<int> DoReadBytes(IByteBuffer buf, IRecvByteBufAllocatorHandle allocHandle)
        {
            int readBytes = 0;
            try
            {
                WebSocketReceiveResult receiveResult = await this.webSocket.ReceiveAsync(new ArraySegment<byte>(buf.Array, buf.ArrayOffset + buf.WriterIndex, buf.WritableBytes), CancellationToken.None);
                if (receiveResult.MessageType == WebSocketMessageType.Text)
                {
                    throw new ProtocolViolationException("Mqtt over WS message cannot be in text");
                }

                buf.SetWriterIndex(buf.WriterIndex + receiveResult.Count);
                readBytes = receiveResult.Count;
            }
            catch (Exception e)  when (!e.IsFatal())
            {
                this.HandleReadException(buf, allocHandle, e);
            }
            finally
            {
                if (readBytes == 0)
                {
                    this.Abort();
                }
            }

            return readBytes;
        }

        void CancelPendingWrite()
        {
            try
            {
                this.writeCancellationTokenSource.Cancel();
            }
            catch (ObjectDisposedException)
            {
                // ignore this error
            }
        }

        void HandleReadException(IByteBuffer byteBuf, IRecvByteBufAllocatorHandle allocHandle, Exception cause)
        {
            if (byteBuf != null)
            {
                if (byteBuf.IsReadable())
                {
                    this.ReadPending = false;
                    this.Pipeline.FireChannelRead(byteBuf);
                }
                else
                {
                    byteBuf.Release();
                }
            }

            allocHandle.ReadComplete();
            this.Pipeline.FireChannelReadComplete();
            this.Pipeline.FireExceptionCaught(cause);
            //if (close || cause is SocketException)
            //{
            //    this.CloseOnRead();
            //}
        }

        void ThrowIfNotOpen()
        {
            var webSocketState = this.webSocket.State;
            if (webSocketState == WebSocketState.Open)
            {
                return;
            }

            if (webSocketState == WebSocketState.Aborted)
            {
                throw new ObjectDisposedException(this.GetType().Name);
            }

            if (webSocketState == WebSocketState.Closed ||
                webSocketState == WebSocketState.CloseReceived ||
                webSocketState == WebSocketState.CloseSent)
            {
                throw new ObjectDisposedException(this.GetType().Name);
            }

            throw new ProtocolViolationException("ClientWebSocketChannel websocket in invalid state");
        }

        void Abort()
        {
            this.active = false;
            this.webSocket.Abort();
            this.webSocket.Dispose();
        }

        public class ClientWebSocketChannelConfig : IChannelConfiguration
        {
            public T GetOption<T>(ChannelOption<T> option)
            {
                Contract.Requires(option != null);

                if (ChannelOption.ConnectTimeout.Equals(option))
                {
                    return (T)(object)this.ConnectTimeout; // no boxing will happen, compiler optimizes away such casts
                }
                if (ChannelOption.WriteSpinCount.Equals(option))
                {
                    return (T)(object)this.WriteSpinCount;
                }
                if (ChannelOption.Allocator.Equals(option))
                {
                    return (T)this.Allocator;
                }
                if (ChannelOption.RcvbufAllocator.Equals(option))
                {
                    return (T)this.RecvByteBufAllocator;
                }
                if (ChannelOption.AutoRead.Equals(option))
                {
                    return (T)(object)this.AutoRead;
                }
                if (ChannelOption.WriteBufferHighWaterMark.Equals(option))
                {
                    return (T)(object)this.WriteBufferHighWaterMark;
                }
                if (ChannelOption.WriteBufferLowWaterMark.Equals(option))
                {
                    return (T)(object)this.WriteBufferLowWaterMark;
                }
                if (ChannelOption.MessageSizeEstimator.Equals(option))
                {
                    return (T)this.MessageSizeEstimator;
                }
                return default(T);
            }

            public bool SetOption(ChannelOption option, object value) => option.Set(this, value);

            public bool SetOption<T>(ChannelOption<T> option, T value)
            {
                // this.Validate(option, value);

                if (ChannelOption.ConnectTimeout.Equals(option))
                {
                    this.ConnectTimeout = (TimeSpan)(object)value;
                }
                else if (ChannelOption.WriteSpinCount.Equals(option))
                {
                    this.WriteSpinCount = (int)(object)value;
                }
                else if (ChannelOption.Allocator.Equals(option))
                {
                    this.Allocator = (IByteBufferAllocator)value;
                }
                else if (ChannelOption.RcvbufAllocator.Equals(option))
                {
                    this.RecvByteBufAllocator = (IRecvByteBufAllocator)value;
                }
                else if (ChannelOption.AutoRead.Equals(option))
                {
                    this.AutoRead = (bool)(object)value;
                }
                else if (ChannelOption.WriteBufferHighWaterMark.Equals(option))
                {
                    this.WriteBufferHighWaterMark = (int)(object)value;
                }
                else if (ChannelOption.WriteBufferLowWaterMark.Equals(option))
                {
                    this.WriteBufferLowWaterMark = (int)(object)value;
                }
                else if (ChannelOption.MessageSizeEstimator.Equals(option))
                {
                    this.MessageSizeEstimator = (IMessageSizeEstimator)value;
                }
                else
                {
                    return false;
                }

                return true;
            }

            public TimeSpan ConnectTimeout { get; set; }

            public int WriteSpinCount { get; set; }

            public IByteBufferAllocator Allocator { get; set; }

            public IRecvByteBufAllocator RecvByteBufAllocator { get; set; }

            public bool AutoRead { get; set; }

            public int WriteBufferHighWaterMark { get; set; }

            public int WriteBufferLowWaterMark { get; set; }

            public IMessageSizeEstimator MessageSizeEstimator { get; set; }
        }
    }
}
