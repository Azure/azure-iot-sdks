// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information

namespace Microsoft.Azure.Devices.Client.Test
{
    using System;
    using System.Threading;
    using System.Threading.Tasks;
    using Microsoft.Azure.Devices.Client;
    using Microsoft.VisualStudio.TestTools.UnitTesting;
    using Moq;

    [TestClass]
    public class ConnectionPoolTests
    {
        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("ConnectionPool")]
        public void DeviceScopeMuxConnection_PoolingOffReleaseTest()
        {
            // Arrange
            var amqpConnectionPoolSettings = new AmqpConnectionPoolSettings();
            amqpConnectionPoolSettings.Pooling = false;
            var amqpTransportSettings = new AmqpTransportSettings(TransportType.Amqp_Tcp_Only, 200, amqpConnectionPoolSettings);
            string connectionString = "HostName=acme.azure-devices.net;DeviceId=device1;SharedAccessKey=CQN2K33r45/0WeIjpqmErV5EIvX8JZrozt3NEHCEkG8=";
            var iotHubConnectionString = IotHubConnectionStringBuilder.Create(connectionString).ToIotHubConnectionString();
            var connectionCache = new Mock<IotHubConnectionCache>();
            // Pooling is off - pass null for ConnectionPoolCache
            var iotHubConnection = new IotHubDeviceMuxConnection(null, 1, iotHubConnectionString, amqpTransportSettings);
            connectionCache.Setup(cache => cache.GetConnection(It.IsAny<IotHubConnectionString>(), It.IsAny<AmqpTransportSettings>())).Returns(iotHubConnection);

            // Act
            var connection = connectionCache.Object.GetConnection(iotHubConnectionString, amqpTransportSettings);
            connection.Release("device"); // does not match "device1" above, However pooling is off. Thus, this iothubconnection object is closed

            // Success
        }

        [ExpectedException(typeof(InvalidOperationException))]
        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("ConnectionPool")]
        public void DeviceScopeMuxConnection_PoolingOnNegativeReleaseTest()
        {
            // Arrange
            var amqpConnectionPoolSettings = new AmqpConnectionPoolSettings();
            var amqpTransportSettings = new AmqpTransportSettings(TransportType.Amqp_Tcp_Only, 200, amqpConnectionPoolSettings);
            string connectionString = "HostName=acme.azure-devices.net;DeviceId=device1;SharedAccessKey=CQN2K33r45/0WeIjpqmErV5EIvX8JZrozt3NEHCEkG8=";
            var iotHubConnectionString = IotHubConnectionStringBuilder.Create(connectionString).ToIotHubConnectionString();
            var connectionCache = new Mock<IotHubConnectionCache>();
            var connectionPool = new IotHubDeviceScopeConnectionPool(connectionCache.Object, iotHubConnectionString, amqpTransportSettings);
            connectionCache.Setup(cache => cache.GetConnection(It.IsAny<IotHubConnectionString>(), It.IsAny<AmqpTransportSettings>())).Returns(connectionPool.GetConnection("device1"));

            // Act
            var connection = connectionCache.Object.GetConnection(iotHubConnectionString, amqpTransportSettings);

            // throw exception if you release a device that is not in the pool
            connection.Release("device2");
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("ConnectionPool")]
        public void DeviceScopeMuxConnection_PoolingOnPositiveReleaseTest()
        {
            // Arrange
            var amqpConnectionPoolSettings = new AmqpConnectionPoolSettings();
            var amqpTransportSettings = new AmqpTransportSettings(TransportType.Amqp_Tcp_Only, 200, amqpConnectionPoolSettings);
            string connectionString = "HostName=acme.azure-devices.net;DeviceId=device1;SharedAccessKey=CQN2K33r45/0WeIjpqmErV5EIvX8JZrozt3NEHCEkG8=";
            var iotHubConnectionString = IotHubConnectionStringBuilder.Create(connectionString).ToIotHubConnectionString();
            var connectionCache = new Mock<IotHubConnectionCache>();
            var connectionPool = new IotHubDeviceScopeConnectionPool(connectionCache.Object, iotHubConnectionString, amqpTransportSettings);
            connectionCache.Setup(
                cache => cache.GetConnection(It.IsAny<IotHubConnectionString>(), It.IsAny<AmqpTransportSettings>())).Returns(connectionPool.GetConnection("device1"));

            // Act
            var connection = connectionCache.Object.GetConnection(iotHubConnectionString, amqpTransportSettings);
            connection.Release("device1");

            // Success - Device1 was in the pool and released
        }

        [ExpectedException(typeof(InvalidOperationException))]
        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("ConnectionPool")]
        public void DeviceScopeMuxConnection_MaxDevicesPerConnectionTest()
        {
            // Arrange

            var amqpConnectionPoolSettings = new AmqpConnectionPoolSettings();
            // Reduce poolsize to 1. This will mux all devices onto one connection
            amqpConnectionPoolSettings.MaxPoolSize = 1;
            var amqpTransportSettings = new AmqpTransportSettings(TransportType.Amqp_Tcp_Only, 200, amqpConnectionPoolSettings);
            string connectionString = "HostName=acme.azure-devices.net;DeviceId=device1;SharedAccessKey=CQN2K33r45/0WeIjpqmErV5EIvX8JZrozt3NEHCEkG8=";
            var iotHubConnectionString = IotHubConnectionStringBuilder.Create(connectionString).ToIotHubConnectionString();
            var connectionCache = new Mock<IotHubConnectionCache>();
            var connectionPool = new IotHubDeviceScopeConnectionPool(connectionCache.Object, iotHubConnectionString, amqpTransportSettings);

            // Act

            // Create 995 Muxed Device Connections
            for (int i = 0; i < AmqpConnectionPoolSettings.MaxDevicesPerConnection; i++)
            {
                connectionPool.GetConnection(iotHubConnectionString + "DeviceId=" + Guid.NewGuid().ToString());
            }

            // try one more. This should throw invalid operation exception
            var connection = connectionPool.GetConnection(iotHubConnectionString + "DeviceId=" + Guid.NewGuid().ToString());
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("ConnectionPool")]
        public void DeviceScopeMuxConnection_NumberOfPoolsTest()
        {
            // Arrange

            var amqpConnectionPoolSettings = new AmqpConnectionPoolSettings();
            var amqpTransportSettings = new AmqpTransportSettings(TransportType.Amqp_Tcp_Only, 200, amqpConnectionPoolSettings);
            string connectionString = "HostName=acme.azure-devices.net;DeviceId=device1;SharedAccessKey=CQN2K33r45/0WeIjpqmErV5EIvX8JZrozt3NEHCEkG8=";
            var iotHubConnectionString = IotHubConnectionStringBuilder.Create(connectionString).ToIotHubConnectionString();
            var connectionCache = new Mock<IotHubConnectionCache>();
            var connectionPool = new IotHubDeviceScopeConnectionPool(connectionCache.Object, iotHubConnectionString, amqpTransportSettings);

            // Act

            // Create 10 Muxed Device Connections - these should hash into different mux connections
            for (int i = 0; i < 10; i++)
            {
                var connection = connectionPool.GetConnection(i.ToString());
            }

            // Assert
            Assert.IsTrue(connectionPool.GetCount() == 10, "Did not create 10 different Connection objects");
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("ConnectionPool")]
        public async Task DeviceScopeMuxConnection_ConnectionIdleTimeoutTest()
        {
            // Arrange

            var amqpConnectionPoolSettings = new AmqpConnectionPoolSettings();
            amqpConnectionPoolSettings.ConnectionIdleTimeout = TimeSpan.FromSeconds(5);
            var amqpTransportSettings = new AmqpTransportSettings(TransportType.Amqp_Tcp_Only, 200, amqpConnectionPoolSettings);
            string connectionString = "HostName=acme.azure-devices.net;DeviceId=device1;SharedAccessKey=CQN2K33r45/0WeIjpqmErV5EIvX8JZrozt3NEHCEkG8=";
            var iotHubConnectionString = IotHubConnectionStringBuilder.Create(connectionString).ToIotHubConnectionString();
            var connectionCache = new Mock<IotHubConnectionCache>();
            var connectionPool = new IotHubDeviceScopeConnectionPool(connectionCache.Object, iotHubConnectionString, amqpTransportSettings);

            // Act

            var connections = new IotHubDeviceMuxConnection[10];
            // Create 10 Muxed Device Connections - these should hash into different mux connections
            for (int i = 0; i < 10; i++)
            {
                connections[i] = (IotHubDeviceMuxConnection)connectionPool.GetConnection(i.ToString());
            }

            for (int j = 0; j < 10; j++)
            {
                connectionPool.RemoveDeviceFromConnection(connections[j], j.ToString());
            }

            await Task.Delay(TimeSpan.FromSeconds(6));

            // Assert
            Assert.IsTrue(connectionPool.GetCount() == 0, "Did not cleanup all Connection objects");
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("ConnectionPool")]
        [ExpectedException(typeof(ObjectDisposedException))]
        public async Task HubScopeMuxConnection_ConnectionIdleTimeoutTest()
        {
            // Arrange
            var amqpConnectionPoolSettings = new AmqpConnectionPoolSettings();
            amqpConnectionPoolSettings.ConnectionIdleTimeout = TimeSpan.FromSeconds(5);
            var amqpTransportSettings = new AmqpTransportSettings(TransportType.Amqp_Tcp_Only, 200, amqpConnectionPoolSettings);
            string connectionString = "HostName=acme.azure-devices.net;DeviceId=device1;SharedAccessKey=CQN2K33r45/0WeIjpqmErV5EIvX8JZrozt3NEHCEkG8=";
            var iotHubConnectionString = IotHubConnectionStringBuilder.Create(connectionString).ToIotHubConnectionString();
            var connectionCache = new Mock<IotHubConnectionCache>();
            connectionCache.Setup(cache => cache.RemoveHubScopeConnectionPool(It.IsAny<IotHubConnectionString>())).Returns(true);
            var hubscopeConnectionPool = new IotHubScopeConnectionPool(connectionCache.Object, iotHubConnectionString, amqpTransportSettings);

            // Act
            for (int i = 0; i < 10; i++)
            {
                hubscopeConnectionPool.TryAddRef();
            }

            // Assert
            Assert.IsTrue(hubscopeConnectionPool.GetCount() == 10, "Reference count should be ten");

            for (int i = 0; i < 10; i++)
            {
                hubscopeConnectionPool.RemoveRef();
            }

            // Assert
            Assert.IsTrue(hubscopeConnectionPool.GetCount() == 0, "Reference count should be zero");

            await Task.Delay(TimeSpan.FromSeconds(6));

            // Hacky way to verify that the SingleTokenConnection object has been closed.
            var singleConnection = (IotHubSingleTokenConnection)hubscopeConnectionPool.Connection;
            await singleConnection.CreateSendingLinkAsync("test", iotHubConnectionString, TimeSpan.FromMinutes(2), CancellationToken.None);
        }
    }
}
