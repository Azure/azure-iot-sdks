// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information

namespace Microsoft.Azure.Devices.Client.Test
{
    using System;
    using System.Diagnostics.Contracts;
    using System.Net;
    using System.Net.WebSockets;
    using System.Threading;
    using System.Threading.Tasks;
    using DotNetty.Buffers;
    using DotNetty.Transport.Channels;

    public class ServerWebSocketChannel : AbstractChannel
    {
        readonly WebSocket webSocket;
        readonly CancellationTokenSource writeCancellationTokenSource;
        bool active;

        internal bool ReadPending { get; set; }

        internal bool WriteInProgress { get; set; }

        public ServerWebSocketChannel(IChannel parent, WebSocket webSocket, EndPoint remoteEndPoint)
            : base(parent)
        {
            this.webSocket = webSocket;
            this.RemoteAddressInternal = remoteEndPoint;
            this.active = true;
            this.Metadata = new ChannelMetadata(false, 16);
            this.Configuration = new ServerWebSocketChannelConfig();
            this.writeCancellationTokenSource = new CancellationTokenSource();
        }

        public override IChannelConfiguration Configuration { get; }

        public override bool Open => this.webSocket.State == WebSocketState.Open;

        public override bool Active => this.active;

        public override ChannelMetadata Metadata { get; }

        public ServerWebSocketChannel Option<T>(ChannelOption<T> option, T value)
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

            public override Task ConnectAsync(EndPoint remoteAddress, EndPoint localAddress)
            {
                throw new NotSupportedException("ServerWebSocketChannel does not support BindAsync()");
            }

            protected override void Flush0()
            {
                // Flush immediately only when there's no pending flush.
                // If there's a pending flush operation, event loop will call FinishWrite() later,
                // and thus there's no need to call it now.
                if (((ServerWebSocketChannel)this.channel).WriteInProgress)
                {
                    return;
                }

                base.Flush0();
            }
        }

        protected override bool IsCompatible(IEventLoop eventLoop) => true;

        protected override void DoBind(EndPoint localAddress)
        {
            throw new NotSupportedException("ServerWebSocketChannel does not support DoBind()");
        }

        protected override void DoDisconnect()
        {
            throw new NotSupportedException("ServerWebSocketChannel does not support DoDisconnect()");
        }

        protected override async void DoClose()
        {
            try
            {
                WebSocketState webSocketState = this.webSocket.State;
                if (webSocketState != WebSocketState.Closed && webSocketState != WebSocketState.Aborted)
                {
                    // Cancel any pending write
                    this.CancelPendingWrite();
                    this.active = false;

                    using (var cancellationTokenSource = new CancellationTokenSource(TimeSpan.FromSeconds(30)))
                    {
                        await this.webSocket.CloseAsync(WebSocketCloseStatus.NormalClosure, string.Empty, cancellationTokenSource.Token);
                    }
                }
            }
            catch (Exception)// when (!e.IsFatal())
            {
                this.Abort();
            }
        }

        protected override async void DoBeginRead()
        {
            IByteBuffer byteBuffer = null;
            IRecvByteBufAllocatorHandle allocHandle = null;
            bool close = false;
            try
            {
                if (!this.Open || this.ReadPending)
                {
                    return;
                }

                this.ReadPending = true;
                IByteBufferAllocator allocator = this.Configuration.Allocator;
                allocHandle = this.Configuration.RecvByteBufAllocator.NewHandle();
                allocHandle.Reset(this.Configuration);
                do
                {
                    byteBuffer = allocHandle.Allocate(allocator);
                    allocHandle.LastBytesRead = await this.DoReadBytes(byteBuffer);
                    if (allocHandle.LastBytesRead <= 0)
                    {
                        // nothing was read -> release the buffer.
                        byteBuffer.Release();
                        byteBuffer = null;
                        close = allocHandle.LastBytesRead < 0;
                        break;
                    }

                    this.Pipeline.FireChannelRead(byteBuffer);
                    allocHandle.IncMessagesRead(1);
                }
                while (allocHandle.ContinueReading());

                allocHandle.ReadComplete();
                this.ReadPending = false;
                this.Pipeline.FireChannelReadComplete();
            }
            catch (Exception e) //when (!e.IsFatal())
            {
                // Since this method returns void, all exceptions must be handled here.
                byteBuffer?.Release();
                allocHandle?.ReadComplete();
                this.ReadPending = false;
                this.Pipeline.FireChannelReadComplete();
                this.Pipeline.FireExceptionCaught(e);
                close = true;
            }

            if (close)
            {
                if (this.Active)
                {
                    await this.HandleCloseAsync();
                }
            }
        }

        protected override async void DoWrite(ChannelOutboundBuffer channelOutboundBuffer)
        {
            try
            {
                this.WriteInProgress = true;
                while (true)
                {
                    object currentMessage = channelOutboundBuffer.Current;
                    if (currentMessage == null)
                    {
                        // Wrote all messages.
                        break;
                    }

                    var byteBuffer = currentMessage as IByteBuffer;
                    Fx.AssertAndThrow(byteBuffer != null, "channelOutBoundBuffer contents must be of type IByteBuffer");

                    if (byteBuffer.ReadableBytes == 0)
                    {
                        channelOutboundBuffer.Remove();
                        continue;
                    }

                    await this.webSocket.SendAsync(byteBuffer.GetIoBuffer(), WebSocketMessageType.Binary, true, this.writeCancellationTokenSource.Token);
                    channelOutboundBuffer.Remove();
                }

                this.WriteInProgress = false;
            }
            catch (Exception e) //when (!e.IsFatal())
            {
                // Since this method returns void, all exceptions must be handled here.

                this.WriteInProgress = false;
                this.Pipeline.FireExceptionCaught(e);
                await this.HandleCloseAsync();
            }
        }

        async Task<int> DoReadBytes(IByteBuffer byteBuffer)
        {
            WebSocketReceiveResult receiveResult = await this.webSocket.ReceiveAsync(new ArraySegment<byte>(byteBuffer.Array, byteBuffer.ArrayOffset + byteBuffer.WriterIndex, byteBuffer.WritableBytes), CancellationToken.None);
            if (receiveResult.MessageType == WebSocketMessageType.Text)
            {
                throw new ProtocolViolationException("Mqtt over WS message cannot be in text");
            }

            // Check if client closed WebSocket
            if (receiveResult.MessageType == WebSocketMessageType.Close)
            {
                return -1;
            }

            byteBuffer.SetWriterIndex(byteBuffer.WriterIndex + receiveResult.Count);
            return receiveResult.Count;
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

        async Task HandleCloseAsync()
        {
            try
            {
                await this.CloseAsync();
            }
            catch (Exception) //when (!ex.IsFatal())
            {
                this.Abort();
            }
        }

        void Abort()
        {
            this.webSocket.Abort();
            this.webSocket.Dispose();
            this.writeCancellationTokenSource.Dispose();
        }
    }

    public class ServerWebSocketChannelConfig : IChannelConfiguration
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
