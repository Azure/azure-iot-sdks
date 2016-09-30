// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information

namespace Microsoft.Azure.Devices.Client.Test
{
    using System;
    using System.Net;
    using System.Net.WebSockets;
    using System.Threading;
    using System.Threading.Tasks;
    using Microsoft.Azure.Amqp;
    using Microsoft.Azure.Amqp.Transport;
    using Microsoft.Azure.Devices.Client.Transport.Mqtt;
    using Microsoft.VisualStudio.TestTools.UnitTesting;

    [TestClass]
    public class ClientWebSocketChannelTests
    {
        const string IotHubName = "localhost";
        const int Port = 12345;
        static HttpListener listener;
        static ClientWebSocketChannel clientWebSocketChannel;
        static readonly byte[] byteArray = new byte[10] { 0x5, 0x6, 0x7, 0x8, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF };
        static volatile bool readComplete;

        [ClassInitialize()]
        public static void AssembyInitialize(TestContext testcontext)
        {
            listener = new HttpListener();
            listener.Prefixes.Add("http://+:" + Port + WebSocketConstants.UriSuffix + "/");
            listener.Start();
            RunWebSocketServer().Fork();
        }

        [ClassCleanup()]
        public static void AssemblyCleanup()
        {
            listener.Stop();
        }

        [ExpectedException(typeof(AmqpException))]
        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("WebSocket")]
        public void ClientWebSocketChannelWriteWithoutConnectTest()
        {
            var websocket = new ClientWebSocket();
            clientWebSocketChannel = new ClientWebSocketChannel(null, websocket);
            var args = new TransportAsyncCallbackArgs();
            args.SetBuffer(byteArray, 0, byteArray.Length);
            clientWebSocketChannel.WriteAsync(args);
        }

        [ExpectedException(typeof(AmqpException))]
        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("WebSocket")]
        public async Task ClientWebSocketChannelReadWithoutConnectTest()
        {
            var websocket = new ClientWebSocket();
            clientWebSocketChannel = new ClientWebSocketChannel(null, websocket);
            var args = new TransportAsyncCallbackArgs();
            var byteArray = new byte[10];
            args.SetBuffer(byteArray, 0, 10);
            if (clientWebSocketChannel.ReadAsync(args))
            {
                while (!readComplete)
                {
                    Thread.Sleep(TimeSpan.FromSeconds(1));
                }
            }

            await websocket.CloseAsync(WebSocketCloseStatus.NormalClosure, "", CancellationToken.None);
        }

        // The following tests can only be run in Administrator mode
        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("WebSocket")]
        public async Task ReadWriteTest()
        {
            var websocket = new ClientWebSocket();
            // Set SubProtocol to AMQPWSB10
            websocket.Options.AddSubProtocol(WebSocketConstants.SubProtocols.Mqtt);
            Uri uri = new Uri("ws://" + IotHubName + ":" + Port + WebSocketConstants.UriSuffix);
            await websocket.ConnectAsync(uri, CancellationToken.None);
            clientWebSocketChannel = new ClientWebSocketChannel(null, websocket);

            // Test Write API
            var args = new TransportAsyncCallbackArgs();
            args.SetBuffer(byteArray, 0, byteArray.Length);
            await clientWebSocketChannel.WriteAsync(args);

            // Test Read API
            if (clientWebSocketChannel.ReadAsync(args))
            {
                while (!readComplete)
                {
                    Thread.Sleep(TimeSpan.FromSeconds(1));
                }
            }

            // Once Read operation is complete, close websocket transport
            // Test Close API
            await clientWebSocketChannel.CloseAsync();
        }

        [ExpectedException(typeof(ObjectDisposedException))]
        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("WebSocket")]
        public async Task ReadAfterCloseTest()
        {
            var websocket = new ClientWebSocket();
            // Set SubProtocol to AMQPWSB10
            websocket.Options.AddSubProtocol(WebSocketConstants.SubProtocols.Mqtt);
            var uri = new Uri("ws://" + IotHubName + ":" + Port + WebSocketConstants.UriSuffix);
            await websocket.ConnectAsync(uri, CancellationToken.None);
            clientWebSocketChannel = new ClientWebSocketChannel(null, websocket);
            await clientWebSocketChannel.CloseAsync();

            var args = new TransportAsyncCallbackArgs();
            var byteArray = new byte[10];
            args.SetBuffer(byteArray, 0, 10);
            await clientWebSocketChannel.ReadAsync(args);
        }

        [ExpectedException(typeof(ObjectDisposedException))]
        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("WebSocket")]
        public async Task WriteAfterCloseTest()
        {
            var websocket = new ClientWebSocket();
            // Set SubProtocol to AMQPWSB10
            websocket.Options.AddSubProtocol(WebSocketConstants.SubProtocols.Mqtt);
            var uri = new Uri("ws://" + IotHubName + ":" + Port + WebSocketConstants.UriSuffix);
            await websocket.ConnectAsync(uri, CancellationToken.None);
            clientWebSocketChannel = new ClientWebSocketChannel(null, websocket);
            await clientWebSocketChannel.CloseAsync();

            var args = new TransportAsyncCallbackArgs();
            args.SetBuffer(byteArray, 0, byteArray.Length);
            await clientWebSocketChannel.WriteAsync(args);
        }

        [ExpectedException(typeof(ObjectDisposedException))]
        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("WebSocket")]
        public async Task ReadAfterAbortTest()
        {
            var websocket = new ClientWebSocket();
            // Set SubProtocol to AMQPWSB10
            websocket.Options.AddSubProtocol(WebSocketConstants.SubProtocols.Mqtt);
            Uri uri = new Uri("ws://" + IotHubName + ":" + Port + WebSocketConstants.UriSuffix);
            await websocket.ConnectAsync(uri, CancellationToken.None);
            clientWebSocketChannel = new ClientWebSocketChannel(null, websocket);
            clientWebSocketChannel.Abort();
            var args = new TransportAsyncCallbackArgs();
            var byteArray = new byte[10];
            args.SetBuffer(byteArray, 0, 10);
            await clientWebSocketChannel.ReadAsync();
        }

        [ExpectedException(typeof(ObjectDisposedException))]
        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("WebSocket")]
        public async Task WriteAfterAbortTest()
        {
            var websocket = new ClientWebSocket();
            // Set SubProtocol to AMQPWSB10
            websocket.Options.AddSubProtocol(WebSocketConstants.SubProtocols.Mqtt);
            Uri uri = new Uri("ws://" + IotHubName + ":" + Port + WebSocketConstants.UriSuffix);
            await websocket.ConnectAsync(uri, CancellationToken.None);
            clientWebSocketChannel = new ClientWebSocketChannel(null, websocket);
            clientWebSocketChannel.Abort();
            var args = new TransportAsyncCallbackArgs();
            args.SetBuffer(byteArray, 0, byteArray.Length);
            await clientWebSocketChannel.WriteAsync(args);
        }

        public static async Task RunWebSocketServer()
        {
            try
            {
                while (true)
                {
                    HttpListenerContext context = await listener.GetContextAsync();
                    if (!context.Request.IsWebSocketRequest)
                    {
                        context.Response.StatusCode = (int)HttpStatusCode.BadRequest;
                        context.Response.Close();
                    }

                    HttpListenerWebSocketContext webSocketContext = await context.AcceptWebSocketAsync(WebSocketConstants.SubProtocols.Mqtt, 8 * 1024, TimeSpan.FromMinutes(5));

                    var buffer = new byte[1 * 1024];
                    var arraySegment = new ArraySegment<byte>(buffer);
                    var cancellationToken = new CancellationToken();
                    WebSocketReceiveResult receiveResult = await webSocketContext.WebSocket.ReceiveAsync(arraySegment, cancellationToken);

                    // Echo the data back to the client
                    var responseCancellationToken = new CancellationToken();
                    var responseBuffer = new byte[receiveResult.Count];
                    for (int i = 0; i < receiveResult.Count; i++)
                    {
                        responseBuffer[i] = arraySegment.Array[i];
                    }

                    var responseSegment = new ArraySegment<byte>(responseBuffer);
                    await webSocketContext.WebSocket.SendAsync(responseSegment, WebSocketMessageType.Binary, true, responseCancellationToken);

                    // Have a pending read
                    var source = new CancellationTokenSource(TimeSpan.FromSeconds(60));
                    WebSocketReceiveResult result = await webSocketContext.WebSocket.ReceiveAsync(arraySegment, source.Token);
                    int bytes = result.Count;
                }
            }
            catch (TaskCanceledException)
            {
                return;
            }
            catch (ObjectDisposedException)
            {
                return;
            }
            catch (WebSocketException)
            {
                return;
            }
            catch (HttpListenerException)
            {
                return;
            }
        }
    }
}
