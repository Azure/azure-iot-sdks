// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information

namespace Microsoft.Azure.Devices.Client.Test
{
    using System;
    using Microsoft.Azure.Devices.Client;
    using Microsoft.VisualStudio.TestTools.UnitTesting;

    [TestClass]
    public class TransportSettingsTests
    {
        [ExpectedException(typeof(ArgumentOutOfRangeException))]
        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("TransportSettings")]
        public void TransportSettingsTest_TransportType_Amqp()
        {
            var transportSetting = new AmqpTransportSettings(TransportType.Amqp);
        }

        [ExpectedException(typeof(ArgumentOutOfRangeException))]
        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("TransportSettings")]
        public void TransportSettingsTest_TransportType_Amqp_Http()
        {
            var transportSetting = new AmqpTransportSettings(TransportType.Http1);
        }

        [ExpectedException(typeof(ArgumentOutOfRangeException))]
        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("TransportSettings")]
        public void TransportSettingsTest_TransportType_AmqpTcp_Prefetch_0()
        {
            var amqpConnectionPoolSettings = new AmqpConnectionPoolSettings();
            var transportSetting = new AmqpTransportSettings(TransportType.Amqp_Tcp_Only, 0, amqpConnectionPoolSettings);
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("TransportSettings")]
        public void TransportSettingsTest_TransportType_Amqp_WebSocket()
        {
            var transportSetting = new AmqpTransportSettings(TransportType.Amqp_WebSocket_Only);
            Assert.IsTrue(transportSetting.GetTransportType() == TransportType.Amqp_WebSocket_Only, "Should be TransportType.Amqp_WebSocket_Only");
            Assert.IsTrue(transportSetting.PrefetchCount == 50, "Should be default value of 50");
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("TransportSettings")]
        public void TransportSettingsTest_TransportType_Amqp_WebSocket_Tcp()
        {
            var amqpConnectionPoolSettings = new AmqpConnectionPoolSettings();
            var transportSetting = new AmqpTransportSettings(TransportType.Amqp_Tcp_Only, 200, amqpConnectionPoolSettings);
            Assert.IsTrue(transportSetting.GetTransportType() == TransportType.Amqp_Tcp_Only, "Should be TransportType.Amqp_Tcp_Only");
            Assert.IsTrue(transportSetting.PrefetchCount == 200, "Should be value of 200");
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("TransportSettings")]
        public void TransportSettingsTest_TransportType_Http()
        {
            var transportSetting = new Http1TransportSettings();
            Assert.IsTrue(transportSetting.GetTransportType() == TransportType.Http1, "Should be TransportType.Http1");
        }
    }
}
