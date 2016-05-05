// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Api.Test
{
    using System;
    using System.IO;
    using System.Net;
    using System.Net.Http;
    using System.Text;

    using Microsoft.Azure.Devices;
    using Microsoft.Azure.Devices.Common;
    using Microsoft.Azure.Devices.Common.Exceptions;
    using Microsoft.VisualStudio.TestTools.UnitTesting;

    [TestClass]
    public class MessageTests
    {
        [TestCategory("CIT")]
        [TestCategory("API")]
        [TestMethod]
        public void ConstructorTakingStreamTest()
        {
            var ms = new MemoryStream(Encoding.UTF8.GetBytes("Hello, World!"));
            var msg = new Message(ms);
            var stream = msg.GetBodyStream();
            Assert.AreSame(ms, stream);
        }

        [TestCategory("CIT")]
        [TestCategory("API")]
        [TestMethod]
        public void ConstructorTakingNullStreamTest()
        {
            var msg = new Message((Stream)null);
            var stream = msg.GetBodyStream();
            Assert.IsNotNull(stream);
            var ms = new MemoryStream();
            stream.CopyTo(ms);
            Assert.IsTrue(ms.GetBuffer().Length == 0);

            msg = new Message((Stream)null);
            var bytes = msg.GetBytes();
            Assert.AreEqual(0, bytes.Length);
        }

        [TestCategory("CIT")]
        [TestCategory("API")]
        [TestMethod]
        public void ConstructorTakingByteArrayTest()
        {
            const string MsgContents = "Hello, World!";
            var msg = new Message(Encoding.UTF8.GetBytes(MsgContents));
            var stream = msg.GetBodyStream();
            var sr = new StreamReader(stream);
            Assert.AreEqual(sr.ReadToEnd(), MsgContents);

            msg = new Message(Encoding.UTF8.GetBytes(MsgContents));
            var msgBytes = msg.GetBytes();
            Assert.AreEqual(Encoding.UTF8.GetString(msgBytes), MsgContents);
        }

        [TestCategory("CIT")]
        [TestCategory("API")]
        [TestMethod]
        [ExpectedException(typeof(ArgumentNullException))]
        public void ConstructorTakingNullByteArrayTest()
        {
            new Message((byte[])null);
        }

        [TestCategory("CIT")]
        [TestCategory("API")]
        [TestMethod]
        public void ConstructorTakingEmptyByteArrayTest()
        {
            var msg = new Message(new byte[0]);
            var stream = msg.GetBodyStream();
            Assert.IsNotNull(stream);
            var ms = new MemoryStream();
            stream.CopyTo(ms);
            Assert.IsTrue(ms.GetBuffer().Length == 0);

            msg = new Message(new byte[0]);
            var bytes = msg.GetBytes();
            Assert.AreEqual(0, bytes.Length);
        }

        [TestCategory("CIT")]
        [TestCategory("API")]
        [TestMethod]
        public void RetrievingMessageBytesAfterGetBodyStreamTest()
        {
            var msg = new Message(new byte[0]);
            msg.GetBodyStream();

            TestAssert.Throws<InvalidOperationException>(() => msg.GetBytes());
        }

        [TestCategory("CIT")]
        [TestCategory("API")]
        [TestMethod]
        public void RetrievingMessageBodyStreamAfterGetBytesTest()
        {
            var msg = new Message(new byte[0]);
            msg.GetBytes();

            TestAssert.Throws<InvalidOperationException>(() => msg.GetBodyStream());
        }

        [TestCategory("CIT")]
        [TestCategory("API")]
        [TestMethod]
        public void CallingGetBytesTwiceTest()
        {
            var msg = new Message(new byte[0]);
            msg.GetBytes();

            TestAssert.Throws<InvalidOperationException>(() => msg.GetBytes());
        }

        [TestCategory("CIT")]
        [TestCategory("API")]
        [TestMethod]
        public void CallingGetBodyStreamTwiceTest()
        {
            var msg = new Message(new byte[0]);
            msg.GetBodyStream();

            TestAssert.Throws<InvalidOperationException>(() => msg.GetBodyStream());
        }

        [TestCategory("CIT")]
        [TestCategory("API")]
        [TestMethod]
        public void DisposingOwnedStreamTest()
        {
            // ownStream = true
            var ms = new MemoryStream(Encoding.UTF8.GetBytes("Hello, World!"));
            var msg = new Message(ms, true);
            msg.Dispose();

            TestAssert.Throws<ObjectDisposedException>(() => ms.Write(Encoding.UTF8.GetBytes("howdy"), 0, 5));

            // ownStream = false
            ms = new MemoryStream(Encoding.UTF8.GetBytes("Hello, World!"));
            msg = new Message(ms, false);
            msg.Dispose();

            ms.Write(Encoding.UTF8.GetBytes("howdy"), 0, 5);
        }

        [TestCategory("CIT")]
        [TestMethod]
        public void HttpExceptionMappingTest_BulkRegistryOperationFailure()
        {
            var message = new HttpResponseMessage(HttpStatusCode.BadRequest);
            message.Headers.Add(CommonConstants.IotHubErrorCode, ErrorCode.BulkRegistryOperationFailure.ToString());
            bool isMappedToException = HttpClientHelper.IsMappedToException(message);
            Assert.IsFalse(isMappedToException, "BulkRegistryOperationFailures should not be mapped to exceptions");
        }
    }
}
