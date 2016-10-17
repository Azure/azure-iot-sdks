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
    using Microsoft.Azure.Devices.Client;
    using Microsoft.VisualStudio.TestTools.UnitTesting;

    [TestClass]
    public class ClientWebSocketTransportTests
    {
        const string IotHubName = "localhost";
        const int Port = 12345;
        static HttpListener listener;
        static readonly Action<TransportAsyncCallbackArgs> onReadOperationComplete = OnReadOperationComplete;
        static readonly Action<TransportAsyncCallbackArgs> onWriteOperationComplete = OnWriteOperationComplete;
        static ClientWebSocketTransport clientWebSocketTransport;
        static LegacyClientWebSocketTransport legacyClientWebSocketTransport;
        static byte[] byteArray = new byte[10] { 0x5, 0x6, 0x7, 0x8, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF };
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
        public void ClientWebSocketTransportWriteWithoutConnectTest()
        {
            var websocket = new ClientWebSocket();
            clientWebSocketTransport = new ClientWebSocketTransport(websocket, null, null);
            var args = new TransportAsyncCallbackArgs();
            args.SetBuffer(byteArray, 0, byteArray.Length);
            clientWebSocketTransport.WriteAsync(args);
        }

        [ExpectedException(typeof(AmqpException))]
        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("WebSocket")]
        public async Task ClientWebSocketTransportReadWithoutConnectTest()
        {
            var websocket = new ClientWebSocket();
            clientWebSocketTransport = new ClientWebSocketTransport(websocket, null, null);
            var args = new TransportAsyncCallbackArgs();
            var byteArray = new byte[10];
            args.SetBuffer(byteArray, 0, 10);
            if (clientWebSocketTransport.ReadAsync(args))
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
            websocket.Options.AddSubProtocol(WebSocketConstants.SubProtocols.Amqpwsb10);
            Uri uri = new Uri("ws://" + IotHubName + ":" + Port + WebSocketConstants.UriSuffix);
            await websocket.ConnectAsync(uri, CancellationToken.None);
            clientWebSocketTransport = new ClientWebSocketTransport(websocket, null, null);

            // Test Write API
            var args = new TransportAsyncCallbackArgs();
            args.CompletedCallback = onWriteOperationComplete;
            args.SetBuffer(byteArray, 0, byteArray.Length);
            clientWebSocketTransport.WriteAsync(args);

            // Test Read API
            args.CompletedCallback = onReadOperationComplete;
            if (clientWebSocketTransport.ReadAsync(args))
            {
                while (!readComplete)
                {
                    Thread.Sleep(TimeSpan.FromSeconds(1));
                }
            }

            // Once Read operation is complete, close websocket transport
            // Test Close API
            await clientWebSocketTransport.CloseAsync(TimeSpan.FromSeconds(30));
        }

        [ExpectedException(typeof(ObjectDisposedException))]
        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("WebSocket")]
        public async Task ReadAfterCloseTest()
        {
            var websocket = new ClientWebSocket();
            // Set SubProtocol to AMQPWSB10
            websocket.Options.AddSubProtocol(WebSocketConstants.SubProtocols.Amqpwsb10);
            var uri = new Uri("ws://" + IotHubName + ":" + Port + WebSocketConstants.UriSuffix);
            await websocket.ConnectAsync(uri, CancellationToken.None);
            clientWebSocketTransport = new ClientWebSocketTransport(websocket, null, null);
            await clientWebSocketTransport.CloseAsync(TimeSpan.FromSeconds(30));

            var args = new TransportAsyncCallbackArgs();
            var byteArray = new byte[10];
            args.SetBuffer(byteArray, 0, 10);
            args.CompletedCallback = onReadOperationComplete;
            clientWebSocketTransport.ReadAsync(args);
        }

        [ExpectedException(typeof(ObjectDisposedException))]
        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("WebSocket")]
        public async Task WriteAfterCloseTest()
        {
            var websocket = new ClientWebSocket();
            // Set SubProtocol to AMQPWSB10
            websocket.Options.AddSubProtocol(WebSocketConstants.SubProtocols.Amqpwsb10);
            var uri = new Uri("ws://" + IotHubName + ":" + Port + WebSocketConstants.UriSuffix);
            await websocket.ConnectAsync(uri, CancellationToken.None);
            clientWebSocketTransport = new ClientWebSocketTransport(websocket, null, null);
            await clientWebSocketTransport.CloseAsync(TimeSpan.FromSeconds(30));

            var args = new TransportAsyncCallbackArgs();
            args.SetBuffer(byteArray, 0, byteArray.Length);
            args.CompletedCallback = onWriteOperationComplete;
            clientWebSocketTransport.WriteAsync(args);
        }

        [ExpectedException(typeof(ObjectDisposedException))]
        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("WebSocket")]
        public async Task ReadAfterAbortTest()
        {
            var websocket = new ClientWebSocket();
            // Set SubProtocol to AMQPWSB10
            websocket.Options.AddSubProtocol(WebSocketConstants.SubProtocols.Amqpwsb10);
            Uri uri = new Uri("ws://" + IotHubName + ":" + Port + WebSocketConstants.UriSuffix);
            await websocket.ConnectAsync(uri, CancellationToken.None);
            clientWebSocketTransport = new ClientWebSocketTransport(websocket, null, null);
            clientWebSocketTransport.Abort();
            var args = new TransportAsyncCallbackArgs();
            var byteArray = new byte[10];
            args.SetBuffer(byteArray, 0, 10);
            clientWebSocketTransport.ReadAsync(args);
        }

        [ExpectedException(typeof(ObjectDisposedException))]
        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("WebSocket")]
        public async Task WriteAfterAbortTest()
        {
            var websocket = new ClientWebSocket();
            // Set SubProtocol to AMQPWSB10
            websocket.Options.AddSubProtocol(WebSocketConstants.SubProtocols.Amqpwsb10);
            Uri uri = new Uri("ws://" + IotHubName + ":" + Port + WebSocketConstants.UriSuffix);
            await websocket.ConnectAsync(uri, CancellationToken.None);
            clientWebSocketTransport = new ClientWebSocketTransport(websocket, null, null);
            clientWebSocketTransport.Abort();
            var args = new TransportAsyncCallbackArgs();
            args.SetBuffer(byteArray, 0, byteArray.Length);
            clientWebSocketTransport.WriteAsync(args);
        }

        [ExpectedException(typeof(AmqpException))]
        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("WebSocket")]
        public void LegacyClientWebSocketTransportWriteWithoutConnectTest()
        {
            var websocket = new IotHubClientWebSocket(WebSocketConstants.SubProtocols.Amqpwsb10);
            var clientWebSocketTransport = new LegacyClientWebSocketTransport(websocket, TimeSpan.FromSeconds(60), null, null);
            var args = new TransportAsyncCallbackArgs();
            args.SetBuffer(byteArray, 0, byteArray.Length);
            clientWebSocketTransport.WriteAsync(args);
        }

        [ExpectedException(typeof(AmqpException))]
        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("WebSocket")]
        public async Task LegacyClientWebSocketTransportReadWithoutConnectTest()
        {
            var websocket = new IotHubClientWebSocket(WebSocketConstants.SubProtocols.Amqpwsb10);
            var clientWebSocketTransport = new LegacyClientWebSocketTransport(websocket, TimeSpan.FromSeconds(60), null, null);
            var args = new TransportAsyncCallbackArgs();
            var byteArray = new byte[10];
            args.SetBuffer(byteArray, 0, 10);
            if (clientWebSocketTransport.ReadAsync(args))
            {
                while (!readComplete)
                {
                    Thread.Sleep(TimeSpan.FromSeconds(1));
                }
            }

            await websocket.CloseAsync();
        }

        // The following tests can only be run in Administrator mode
        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("WebSocket")]
        public async Task LegacyWebSocketReadWriteTest()
        {
            var websocket = new IotHubClientWebSocket(WebSocketConstants.SubProtocols.Amqpwsb10);
            await websocket.ConnectAsync(IotHubName, Port, "ws://", TimeSpan.FromMinutes(1));

            legacyClientWebSocketTransport = new LegacyClientWebSocketTransport(websocket, TimeSpan.FromSeconds(60), null, null);

            // Test Write API
            TransportAsyncCallbackArgs args = new TransportAsyncCallbackArgs();
            args.CompletedCallback = onWriteOperationComplete;
            args.SetBuffer(byteArray, 0, byteArray.Length);
            legacyClientWebSocketTransport.WriteAsync(args);

            // Test Read API
            args.CompletedCallback = onReadOperationComplete;
            if (legacyClientWebSocketTransport.ReadAsync(args))
            {
                while (!readComplete)
                {
                    Thread.Sleep(TimeSpan.FromSeconds(1));
                }
            }

            // Once Read operation is complete, close websocket transport
            legacyClientWebSocketTransport.CloseAsync(TimeSpan.FromSeconds(30)).Wait(CancellationToken.None);
        }

        [ExpectedException(typeof(ObjectDisposedException))]
        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("WebSocket")]
        public async Task LegacyWebSocketReadAfterCloseTest()
        {
            var websocket = new IotHubClientWebSocket(WebSocketConstants.SubProtocols.Amqpwsb10);
            await websocket.ConnectAsync(IotHubName, Port, "ws://", TimeSpan.FromMinutes(1));
            legacyClientWebSocketTransport = new LegacyClientWebSocketTransport(websocket, TimeSpan.FromMinutes(1), null, null);
            await legacyClientWebSocketTransport.CloseAsync(TimeSpan.FromSeconds(30));
            var args = new TransportAsyncCallbackArgs();
            var byteArray = new byte[10];
            args.SetBuffer(byteArray, 0, 10);
            legacyClientWebSocketTransport.ReadAsync(args);
        }

        [ExpectedException(typeof(ObjectDisposedException))]
        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("WebSocket")]
        public async Task LegacyWebSocketWriteAfterCloseTest()
        {
            var websocket = new IotHubClientWebSocket(WebSocketConstants.SubProtocols.Amqpwsb10);
            await websocket.ConnectAsync(IotHubName, Port, "ws://", TimeSpan.FromMinutes(1));
            legacyClientWebSocketTransport = new LegacyClientWebSocketTransport(websocket, TimeSpan.FromMinutes(1), null, null);
            await legacyClientWebSocketTransport.CloseAsync(TimeSpan.FromSeconds(30));
            var args = new TransportAsyncCallbackArgs();
            args.SetBuffer(byteArray, 0, byteArray.Length);
            legacyClientWebSocketTransport.WriteAsync(args);
        }

        [ExpectedException(typeof(ObjectDisposedException))]
        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("WebSocket")]
        public async Task LegacyWebSocketReadAfterAbortTest()
        {
            var websocket = new IotHubClientWebSocket(WebSocketConstants.SubProtocols.Amqpwsb10);
            await websocket.ConnectAsync(IotHubName, Port, "ws://", TimeSpan.FromMinutes(1));
            legacyClientWebSocketTransport = new LegacyClientWebSocketTransport(websocket, TimeSpan.FromMinutes(1), null, null);
            legacyClientWebSocketTransport.Abort();

            var args = new TransportAsyncCallbackArgs();
            var byteArray = new byte[10];
            args.SetBuffer(byteArray, 0, 10);
            legacyClientWebSocketTransport.ReadAsync(args);
            Assert.Fail("Did not throw object disposed exception");
        }

        [ExpectedException(typeof(ObjectDisposedException))]
        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("WebSocket")]
        public async Task LegacyWebSocketWriteAfterAbortTest()
        {
            var websocket = new IotHubClientWebSocket(WebSocketConstants.SubProtocols.Amqpwsb10);
            await websocket.ConnectAsync(IotHubName, Port, "ws://", TimeSpan.FromMinutes(1));
            legacyClientWebSocketTransport = new LegacyClientWebSocketTransport(websocket, TimeSpan.FromMinutes(1), null, null);
            legacyClientWebSocketTransport.Abort();

            var args = new TransportAsyncCallbackArgs();
            args.SetBuffer(byteArray, 0, byteArray.Length);
            legacyClientWebSocketTransport.WriteAsync(args);
            Assert.Fail("Did not throw object disposed exception");
        }

        static void OnWriteOperationComplete(TransportAsyncCallbackArgs args)
        {
            if (args.BytesTransfered != byteArray.Length)
            {
                throw new InvalidOperationException("All the bytes sent were not transferred");
            }

            if (args.Exception != null)
            {
                throw args.Exception;
            }
        }

        static void OnReadOperationComplete(TransportAsyncCallbackArgs args)
        {
            if (args.Exception != null)
            {
                throw args.Exception;
            }

            // Verify that data matches what was sent
            if (byteArray.Length != args.Count)
            {
                throw new InvalidOperationException("Expected " + byteArray.Length + " bytes in response");
            }

            for (int i = 0; i < args.Count; i++)
            {
                if (byteArray[i] != args.Buffer[i])
                {
                    throw new InvalidOperationException("Response contents do not match what was sent");
                }
            }

            readComplete = true;
        }

        static public async Task RunWebSocketServer()
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

                    HttpListenerWebSocketContext webSocketContext = await context.AcceptWebSocketAsync(WebSocketConstants.SubProtocols.Amqpwsb10, 8 * 1024, TimeSpan.FromMinutes(5));

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
