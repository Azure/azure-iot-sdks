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
        static byte[] byteArray = new byte[10] { 0x5, 0x6, 0x7, 0x8, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF };

        [AssemblyInitialize()]
        public static void AssembyInitialize(TestContext testcontext)
        {
            listener = new HttpListener();
            listener.Prefixes.Add("http://+:" + Port + WebSocketConstants.UriSuffix + "/");
            listener.Start();
            RunWebSocketServer().Fork();
        }

        [AssemblyCleanup()]
        public static void AssemblyCleanup()
        {
            listener.Stop();
        }

        [ExpectedException(typeof(AmqpException))]
        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("WebSocket")]
        public void ClientWebSocketTransportWriteTest()
        {
            var websocket = new ClientWebSocket();
            var clientWebSocketTransport = new ClientWebSocketTransport(websocket, null, null);
            TransportAsyncCallbackArgs args = new TransportAsyncCallbackArgs();
            args.SetBuffer(byteArray, 0, byteArray.Length);
            clientWebSocketTransport.WriteAsync(args);
        }

        [ExpectedException(typeof(AmqpException))]
        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("WebSocket")]
        public void ClientWebSocketTransportReadTest()
        {
            var websocket = new ClientWebSocket();
            var clientWebSocketTransport = new ClientWebSocketTransport(websocket, null, null);
            TransportAsyncCallbackArgs args = new TransportAsyncCallbackArgs();
            byte[] byteArray = new byte[10];
            args.SetBuffer(byteArray, 0, 10);
            clientWebSocketTransport.ReadAsync(args);
        }

        // The following tests can only be run in Administrator mode
        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("WebSocket")]
        public void ReadWriteTest()
        {
            var websocket = new ClientWebSocket();
            // Set SubProtocol to AMQPWSB10
            websocket.Options.AddSubProtocol(WebSocketConstants.SubProtocols.Amqpwsb10);
            Uri uri = new Uri("ws://" + IotHubName + ":" + Port + WebSocketConstants.UriSuffix);
            websocket.ConnectAsync(uri, CancellationToken.None).Wait(CancellationToken.None);
            clientWebSocketTransport = new ClientWebSocketTransport(websocket, null, null);

            // Test Write API
            TransportAsyncCallbackArgs args = new TransportAsyncCallbackArgs();
            args.CompletedCallback = onWriteOperationComplete;
            args.SetBuffer(byteArray, 0, byteArray.Length);
            clientWebSocketTransport.WriteAsync(args);

            // Test Read API
            args.CompletedCallback = onReadOperationComplete;
            clientWebSocketTransport.ReadAsync(args);
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

            // Once Read operation is complete, close websocket transport
            // Test Close API
            clientWebSocketTransport.CloseAsync(TimeSpan.FromSeconds(30)).Wait(CancellationToken.None);
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("WebSocket")]
        public void ReadWriteAfterCloseTest()
        {
            var websocket = new ClientWebSocket();
            // Set SubProtocol to AMQPWSB10
            websocket.Options.AddSubProtocol(WebSocketConstants.SubProtocols.Amqpwsb10);
            Uri uri = new Uri("ws://" + IotHubName + ":" + Port + WebSocketConstants.UriSuffix);
            websocket.ConnectAsync(uri, CancellationToken.None).Wait(CancellationToken.None);
            clientWebSocketTransport = new ClientWebSocketTransport(websocket, null, null);
            clientWebSocketTransport.CloseAsync(TimeSpan.FromSeconds(30)).Wait(CancellationToken.None);

            try
            {
                TransportAsyncCallbackArgs args = new TransportAsyncCallbackArgs();
                args.SetBuffer(byteArray, 0, byteArray.Length);
                clientWebSocketTransport.WriteAsync(args);
                Assert.Fail("Did not throw object disposed exception");
            }
            catch (ObjectDisposedException)
            {

            }

            try
            {
                TransportAsyncCallbackArgs args = new TransportAsyncCallbackArgs();
                byte[] byteArray = new byte[10];
                args.SetBuffer(byteArray, 0, 10);
                clientWebSocketTransport.ReadAsync(args);
                Assert.Fail("Did not throw object disposed exception");
            }
            catch (ObjectDisposedException)
            {

            }
        }
        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("WebSocket")]
        public void ReadWriteAfterAbortTest()
        {
            var websocket = new ClientWebSocket();
            // Set SubProtocol to AMQPWSB10
            websocket.Options.AddSubProtocol(WebSocketConstants.SubProtocols.Amqpwsb10);
            Uri uri = new Uri("ws://" + IotHubName + ":" + Port + WebSocketConstants.UriSuffix);
            websocket.ConnectAsync(uri, CancellationToken.None).Wait(CancellationToken.None);
            clientWebSocketTransport = new ClientWebSocketTransport(websocket, null, null);
            clientWebSocketTransport.Abort();

            try
            {
                TransportAsyncCallbackArgs args = new TransportAsyncCallbackArgs();
                args.SetBuffer(byteArray, 0, byteArray.Length);
                clientWebSocketTransport.WriteAsync(args);
                Assert.Fail("Did not throw object disposed exception");
            }
            catch(ObjectDisposedException)
            {

            }

            try
            {
                TransportAsyncCallbackArgs args = new TransportAsyncCallbackArgs();
                byte[] byteArray = new byte[10];
                args.SetBuffer(byteArray, 0, 10);
                clientWebSocketTransport.ReadAsync(args);
                Assert.Fail("Did not throw object disposed exception");
            }
            catch (ObjectDisposedException)
            {

            }
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

                    byte[] buffer = new byte[1 * 1024];
                    ArraySegment<byte> arraySegment = new ArraySegment<byte>(buffer);
                    CancellationToken cancellationToken = new CancellationToken();
                    WebSocketReceiveResult receiveResult = await webSocketContext.WebSocket.ReceiveAsync(arraySegment, cancellationToken);

                    // Echo the data back to the client
                    CancellationToken responseCancellationToken = new CancellationToken();
                    byte[] responseBuffer = new byte[receiveResult.Count];
                    for (int i = 0; i < receiveResult.Count; i++)
                    {
                        responseBuffer[i] = arraySegment.Array[i];
                    }

                    ArraySegment<byte> responseSegment = new ArraySegment<byte>(responseBuffer);
                    await webSocketContext.WebSocket.SendAsync(responseSegment, WebSocketMessageType.Binary, true, responseCancellationToken);

                    // Have a pending read
                    CancellationTokenSource source = new CancellationTokenSource(TimeSpan.FromSeconds(60));
                    WebSocketReceiveResult result = await webSocketContext.WebSocket.ReceiveAsync(arraySegment, source.Token);
                    int bytes = result.Count;
                }
            }
            catch (TaskCanceledException)
            {
                return;
            }
       }
    }
}
