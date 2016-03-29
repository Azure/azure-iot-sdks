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

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("TransportSettings")]
        [ExpectedException(typeof(ArgumentOutOfRangeException))]
        public void TransportSettingsTest_ZeroOperationTimeout()
        {
            var amqpConnectionPoolSettings = new AmqpConnectionPoolSettings();
            var transportSetting = new AmqpTransportSettings(TransportType.Amqp, 200, amqpConnectionPoolSettings);
            transportSetting.OperationTimeout = TimeSpan.Zero;
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("TransportSettings")]
        [ExpectedException(typeof(ArgumentOutOfRangeException))]
        public void TransportSettingsTest_ZeroOpenTimeout()
        {
            var amqpConnectionPoolSettings = new AmqpConnectionPoolSettings();
            var transportSetting = new AmqpTransportSettings(TransportType.Amqp, 200, amqpConnectionPoolSettings);
            transportSetting.OpenTimeout = TimeSpan.Zero;
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("TransportSettings")]
        public void TransportSettingsTest_Timeouts()
        {
            var amqpConnectionPoolSettings = new AmqpConnectionPoolSettings();
            var transportSetting = new AmqpTransportSettings(TransportType.Amqp_WebSocket_Only, 200, amqpConnectionPoolSettings);
            transportSetting.OpenTimeout = TimeSpan.FromMinutes(5);
            transportSetting.OperationTimeout = TimeSpan.FromMinutes(10);
            Assert.IsTrue(transportSetting.OpenTimeout == TimeSpan.FromMinutes(5), "OpenTimeout not set correctly");
            Assert.IsTrue(transportSetting.OperationTimeout == TimeSpan.FromMinutes(10), "OperationTimeout not set correctly");
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("TransportSettings")]
        [ExpectedException(typeof(ArgumentOutOfRangeException))]
        public void ConnectionPoolSettingsTest_ZeroPoolSize()
        {
            var connectionPoolSettings = new AmqpConnectionPoolSettings();
            connectionPoolSettings.MaxPoolSize = 0;
            var transportSetting = new AmqpTransportSettings(TransportType.Amqp, 200, connectionPoolSettings);
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("TransportSettings")]
        [ExpectedException(typeof(ArgumentOutOfRangeException))]
        public void ConnectionPoolSettingsTest_ZeroIdleTimeout()
        {
            var connectionPoolSettings = new AmqpConnectionPoolSettings();
            connectionPoolSettings.ConnectionIdleTimeout = TimeSpan.Zero;
            var transportSetting = new AmqpTransportSettings(TransportType.Amqp, 200, connectionPoolSettings);
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("TransportSettings")]
        public void ConnectionPoolSettingsTest()
        {
            var connectionPoolSettings = new AmqpConnectionPoolSettings();
            connectionPoolSettings.MaxPoolSize = ushort.MaxValue;
            var transportSetting = new AmqpTransportSettings(TransportType.Amqp_Tcp_Only, 200, connectionPoolSettings);
            Assert.IsTrue(transportSetting.AmqpConnectionPoolSettings.MaxPoolSize == ushort.MaxValue, "MaxPoolSize should be 64K");
        }
    }
}
