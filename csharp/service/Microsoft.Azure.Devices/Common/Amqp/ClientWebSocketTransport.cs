// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Amqp.Transport
{
    using System;
    using System.IO;
    using System.Net;
    using System.Net.WebSockets;
    using System.Threading;
    using System.Threading.Tasks;

    using Microsoft.Azure.Devices.Common;

    sealed class ClientWebSocketTransport : TransportBase
    {
        static readonly AsyncCallback onReadComplete = OnReadComplete;
        static readonly AsyncCallback onWriteComplete = OnWriteComplete;

        readonly ClientWebSocket webSocket;
        readonly EndPoint localEndPoint;
        readonly EndPoint remoteEndPoint;
        readonly string iotHubName;
        volatile CancellationTokenSource writeCancellationTokenSource;

        public ClientWebSocketTransport(ClientWebSocket webSocket, string iotHubName, EndPoint localEndpoint, EndPoint remoteEndpoint)
            : base("clientwebsocket")
        {
            this.webSocket = webSocket;
            this.iotHubName = iotHubName;
            this.localEndPoint = localEndpoint;
            this.remoteEndPoint = remoteEndpoint;
            this.writeCancellationTokenSource = new CancellationTokenSource();
        }

        public override EndPoint LocalEndPoint
        {
            get { return this.localEndPoint; }
        }

        public override EndPoint RemoteEndPoint
        {
            get { return this.remoteEndPoint; }
        }

        public override bool RequiresCompleteFrames
        {
            get { return true; }
        }

        public override bool IsSecure
        {
            get { return true; }
        }

        public override bool WriteAsync(TransportAsyncCallbackArgs args)
        {
            this.ThrowIfNotOpen();

            Fx.AssertAndThrow(args.Buffer != null || args.ByteBufferList != null, "must have a buffer to write");
            Fx.AssertAndThrow(args.CompletedCallback != null, "must have a valid callback");
            args.Exception = null; // null out any exceptions

            Task taskResult = this.WriteAsyncCore(args);
            if (WriteTaskDone(taskResult, args))
            {
                return false;
            }

            taskResult.ToAsyncResult(onWriteComplete, args);
            return true;
        }

        async Task WriteAsyncCore(TransportAsyncCallbackArgs args)
        {
            bool succeeded = false;
            try
            {
                if (args.Buffer != null)
                {
                    var arraySegment = new ArraySegment<byte>(args.Buffer, args.Offset, args.Count);
                    await this.webSocket.SendAsync(arraySegment, WebSocketMessageType.Binary, true, this.writeCancellationTokenSource.Token);
                }
                else
                {
                    foreach (ByteBuffer byteBuffer in args.ByteBufferList)
                    {
                        await this.webSocket.SendAsync(new ArraySegment<byte>(byteBuffer.Buffer, byteBuffer.Offset, byteBuffer.Length),
                            WebSocketMessageType.Binary, true, this.writeCancellationTokenSource.Token);
                    }
                }

                succeeded = true;
            }
            catch (WebSocketException webSocketException)
            {
                throw new IOException(
                    webSocketException.InnerException != null ? webSocketException.InnerException.Message : webSocketException.Message,
                    webSocketException.InnerException ?? webSocketException);
            }
            catch (HttpListenerException httpListenerException)
            {
                throw new IOException(
                    httpListenerException.InnerException != null ? httpListenerException.InnerException.Message : httpListenerException.Message,
                    httpListenerException.InnerException ?? httpListenerException);
            }
            catch (TaskCanceledException taskCanceledException)
            {
                throw new TimeoutException(
                    taskCanceledException.InnerException != null ? taskCanceledException.InnerException.Message : taskCanceledException.Message,
                    taskCanceledException.InnerException ?? taskCanceledException);
            }
            finally
            {
                if (!succeeded)
                {
                    this.Abort();
                }
            }
        }

        public override bool ReadAsync(TransportAsyncCallbackArgs args)
        {
            this.ThrowIfNotOpen();

            // Read with buffer list not supported
            Fx.AssertAndThrow(args.Buffer != null, "must have buffer to read");
            Fx.AssertAndThrow(args.CompletedCallback != null, "must have a valid callback");

            //  ConnectionUtilities.ValidateBufferBounds(args.Buffer, args.Offset, args.Count);
            args.Exception = null; // null out any exceptions

            Task<int> taskResult = this.ReadAsyncCore(args);
            if (ReadTaskDone(taskResult, args))
            {
                return false;
            }

            taskResult.ToAsyncResult(onReadComplete, args);
            return true;
        }

        async Task<int> ReadAsyncCore(TransportAsyncCallbackArgs args)
        {
            bool succeeded = false;
            try
            {
                WebSocketReceiveResult receiveResult = await this.webSocket.ReceiveAsync(
                    new ArraySegment<byte>(args.Buffer, args.Offset, args.Count), CancellationToken.None);

                succeeded = true;
                return receiveResult.Count;
            }
            catch (WebSocketException webSocketException)
            {
                throw new IOException(
                    webSocketException.InnerException != null ? webSocketException.InnerException.Message : webSocketException.Message,
                    webSocketException.InnerException ?? webSocketException);
            }
            catch (HttpListenerException httpListenerException)
            {
                throw new IOException(
                    httpListenerException.InnerException != null ? httpListenerException.InnerException.Message : httpListenerException.Message,
                    httpListenerException.InnerException ?? httpListenerException); ;
            }
            catch (TaskCanceledException taskCanceledException)
            {
                throw new TimeoutException(
                    taskCanceledException.InnerException != null ? taskCanceledException.InnerException.Message : taskCanceledException.Message,
                    taskCanceledException.InnerException ?? taskCanceledException);
            }
            finally
            {
                if (!succeeded)
                {
                    this.Abort();
                }
            }
        }

        protected override bool OpenInternal()
        {
            this.ThrowIfNotOpen();

            return true;
        }

        protected override bool CloseInternal()
        {
            var webSocketState = this.webSocket.State;
            if (webSocketState != WebSocketState.Closed && webSocketState != WebSocketState.Aborted)
            {
                this.CloseInternalAsync(TimeSpan.FromSeconds(30)).Fork();
            }

            return true;
        }

        async Task CloseInternalAsync(TimeSpan timeout)
        {
            try
            {
                // Cancel any pending write
                this.CancelPendingWrite();

                using (var cancellationTokenSource = new CancellationTokenSource(timeout))
                {
                    await this.webSocket.CloseAsync(WebSocketCloseStatus.NormalClosure, string.Empty, cancellationTokenSource.Token);
                }
            }
            catch (Exception e)
            {
                if (Fx.IsFatal(e))
                {
                    throw;
                }
            }

            // Call Abort anyway to ensure that all WebSocket Resources are released 
            this.Abort();
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

        protected override void AbortInternal()
        {
            if (this.webSocket.State != WebSocketState.Aborted)
            {
                this.webSocket.Abort();
                this.webSocket.Dispose();
            }
        }

        static void OnReadComplete(IAsyncResult result)
        {
            if (result.CompletedSynchronously)
            {
                return;
            }

            HandleReadComplete(result);
        }

        static void HandleReadComplete(IAsyncResult result)
        {
            Task<int> taskResult = (Task<int>)result;
            var args = (TransportAsyncCallbackArgs)taskResult.AsyncState;

            ReadTaskDone(taskResult, args);
            args.CompletedCallback(args);
        }

        static bool ReadTaskDone(Task<int> taskResult, TransportAsyncCallbackArgs args)
        {
            IAsyncResult result = taskResult;
            if (taskResult.IsFaulted)
            {
                args.Exception = taskResult.Exception;
                return true;
            }
            else if (taskResult.IsCompleted)
            {
                args.BytesTransfered = taskResult.Result;
                args.CompletedSynchronously = result.CompletedSynchronously;
                return true;
            }
            else if (taskResult.IsCanceled)  // This should not happen since TaskCanceledException is handled in ReadAsyncCore.
            {
                args.BytesTransfered = 0;
                return true;
            }

            return false;
        }

        static void OnWriteComplete(IAsyncResult result)
        {
            if (result.CompletedSynchronously)
            {
                return;
            }

            HandleWriteComplete(result);
        }

        static void HandleWriteComplete(IAsyncResult result)
        {
            Task taskResult = (Task)result;
            var args = (TransportAsyncCallbackArgs)taskResult.AsyncState;
            WriteTaskDone(taskResult, args);
            args.CompletedCallback(args);
        }

        static bool WriteTaskDone(Task taskResult, TransportAsyncCallbackArgs args)
        {
            IAsyncResult result = taskResult;
            if (taskResult.IsFaulted)
            {
                args.Exception = taskResult.Exception;
                return true;
            }
            else if (taskResult.IsCompleted)
            {
                args.BytesTransfered = args.Count;
                args.CompletedSynchronously = result.CompletedSynchronously;
                return true;
            }
            else if (taskResult.IsCanceled)  // This should not happen since TaskCanceledException is handled in WriteAsyncCore.
            {
                args.BytesTransfered = 0;
                return true;
            }

            return false;
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

            throw new AmqpException(AmqpErrorCode.IllegalState, null);
        }
    }
}