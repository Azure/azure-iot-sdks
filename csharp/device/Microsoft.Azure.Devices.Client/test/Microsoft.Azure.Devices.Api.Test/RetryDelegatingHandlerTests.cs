// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
namespace Microsoft.Azure.Devices.Client.Test
{
    using System;
    using System.Collections.Generic;
    using System.IO;
    using System.Linq;
    using System.Threading.Tasks;
    using Microsoft.Azure.Devices.Client.Common;
    using Microsoft.Azure.Devices.Client.Exceptions;
    using Microsoft.Azure.Devices.Client.Transport;
    using Microsoft.VisualStudio.TestTools.UnitTesting;
    using NSubstitute;

    [TestClass]
    public class RetryDelegatingHandlerTests
    {
        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("DelegatingHandlers")]
        [TestCategory("Owner [mtuchkov]")]
        public async Task Retry_TransientErrorOccured_Retried()
        {
            int callCounter = 0;

            var innerHandlerMock = Substitute.For<IDelegatingHandler>();
            innerHandlerMock.OpenAsync(true).Returns(t =>
            {
                callCounter++;

                if (callCounter == 1)
                {
                    throw new IotHubClientTransientException("");
                }
                return TaskConstants.Completed;
            });

            var sut = new RetryDelegatingHandler(innerHandlerMock);
            await sut.OpenAsync(true);

            Assert.AreEqual(2, callCounter);
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("DelegatingHandlers")]
        [TestCategory("Owner [mtuchkov]")]
        public async Task Retry_MessageHasBeenTouchedTransientExceptionOccured_Success()
        {
            int callCounter = 0;

            var innerHandlerMock = Substitute.For<IDelegatingHandler>();
            var message = new Message(new MemoryStream(new byte[] {1,2,3}));
            innerHandlerMock.SendEventAsync(Arg.Is(message)).Returns(t =>
            {
                callCounter++;

                var m = t.Arg<Message>();
                Stream stream = m.BodyStream;
                if (callCounter == 1)
                {
                    throw new IotHubClientTransientException("");
                }
                var buffer = new byte[3];
                stream.Read(buffer, 0, 3);
                return TaskConstants.Completed;
            });

            var sut = new RetryDelegatingHandler(innerHandlerMock);
            await sut.SendEventAsync(message);

            Assert.AreEqual(2, callCounter);
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("DelegatingHandlers")]
        [TestCategory("Owner [mtuchkov]")]
        public async Task Retry_MessageHasBeenReadTransientExceptionOccured_Throws()
        {
            int callCounter = 0;

            var innerHandlerMock = Substitute.For<IDelegatingHandler>();
            var memoryStream = new NotSeekableStream(new byte[] {1,2,3});
            var message = new Message(memoryStream);
            innerHandlerMock.SendEventAsync(Arg.Is(message)).Returns(t =>
            {
                callCounter++;
                var m = t.Arg<Message>();
                Stream stream = m.BodyStream;
                var buffer = new byte[3];
                stream.Read(buffer, 0, 3);
                throw new IotHubClientTransientException("");
            });

            var sut = new RetryDelegatingHandler(innerHandlerMock);
            await sut.SendEventAsync(message).ExpectedAsync<IotHubClientTransientException>();

            Assert.AreEqual(callCounter, 1);
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("DelegatingHandlers")]
        [TestCategory("Owner [mtuchkov]")]
        public async Task Retry_OneMessageHasBeenTouchedTransientExceptionOccured_Success()
        {
            int callCounter = 0;

            var innerHandlerMock = Substitute.For<IDelegatingHandler>();
            var message = new Message(new MemoryStream(new byte[] {1,2,3}));
            IEnumerable<Message> messages = new[] { message };
            innerHandlerMock.SendEventAsync(Arg.Is(messages)).Returns(t =>
            {
                callCounter++;

                Message m = t.Arg<IEnumerable<Message>>().First();
                Stream stream = m.BodyStream;
                if (callCounter == 1)
                {
                    throw new IotHubClientTransientException("");
                }
                var buffer = new byte[3];
                stream.Read(buffer, 0, 3);
                return TaskConstants.Completed;
            });

            var sut = new RetryDelegatingHandler(innerHandlerMock);
            await sut.SendEventAsync(messages);

            Assert.AreEqual(2, callCounter);
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("DelegatingHandlers")]
        [TestCategory("Owner [mtuchkov]")]
        public async Task Retry_OneMessageHasBeenReadTransientExceptionOccured_Throws()
        {
            int callCounter = 0;

            var innerHandlerMock = Substitute.For<IDelegatingHandler>();
            var memoryStream = new NotSeekableStream(new byte[] {1,2,3});
            var message = new Message(memoryStream);
            var messages = new[] { message };
            innerHandlerMock.SendEventAsync(Arg.Is(messages)).Returns(t =>
            {
                callCounter++;
                Message m = t.Arg<IEnumerable<Message>>().First();
                Stream stream = m.BodyStream;
                var buffer = new byte[3];
                stream.Read(buffer, 0, 3);
                throw new IotHubClientTransientException("");
            });

            var sut = new RetryDelegatingHandler(innerHandlerMock);
            await sut.SendEventAsync(messages).ExpectedAsync<IotHubClientTransientException>();

            Assert.AreEqual(callCounter, 1);
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("DelegatingHandlers")]
        [TestCategory("Owner [mtuchkov]")]
        public async Task Retry_MessageWithSeekableStreamHasBeenReadTransientExceptionOccured_Throws()
        {
            int callCounter = 0;

            var innerHandlerMock = Substitute.For<IDelegatingHandler>();
            var message = new Message(new MemoryStream(new byte[] {1,2,3}));
            innerHandlerMock.SendEventAsync(Arg.Is(message)).Returns(t =>
            {
                callCounter++;
                var m = t.Arg<Message>();
                Stream stream = m.BodyStream;
                var buffer = new byte[3];
                stream.Read(buffer, 0, 3);
                if (callCounter == 1)
                {
                    throw new IotHubClientTransientException("");
                }
                return TaskConstants.Completed;
            });

            var sut = new RetryDelegatingHandler(innerHandlerMock);
            await sut.SendEventAsync(message);

            Assert.AreEqual(callCounter, 2);
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("DelegatingHandlers")]
        [TestCategory("Owner [mtuchkov]")]
        public async Task Retry_NonTransientErrorThrown_Throws()
        {
            int callCounter = 0;

            var innerHandlerMock = Substitute.For<IDelegatingHandler>();
            innerHandlerMock.OpenAsync(true).Returns(t =>
            {
                callCounter++;

                if (callCounter == 1)
                {
                    throw new InvalidOperationException("");
                }
                return TaskConstants.Completed;
            });

            var sut = new RetryDelegatingHandler(innerHandlerMock);

            await sut.OpenAsync(true).ExpectedAsync<InvalidOperationException>();

            Assert.AreEqual(callCounter, 1);
        }

        [TestMethod]
        [TestCategory("LongRunning")]
        [TestCategory("DelegatingHandlers")]
        [TestCategory("Owner [mtuchkov]")]
        public async Task Retry_TransientErrorThrownAfterNumberOfRetries_Throws()
        {
            RetryDelegatingHandler.RetryCount = 2;

            var innerHandlerMock = Substitute.For<IDelegatingHandler>();
            innerHandlerMock.OpenAsync(Arg.Is(true)).Returns(t =>
            {
                throw new IotHubClientTransientException("");
            });

            var sut = new RetryDelegatingHandler(innerHandlerMock);

            await sut.OpenAsync(true).ExpectedAsync<IotHubClientTransientException>();
        }

        class NotSeekableStream : MemoryStream
        {
            public override bool CanSeek => false;

            public NotSeekableStream(byte[] buffer):base(buffer)
            {
                
            }
        }
    }
}