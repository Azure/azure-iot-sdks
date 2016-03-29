// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information

namespace Microsoft.Azure.Devices.Client.Test
{
    using System;
    using Microsoft.Azure.Devices.Client;
    using Microsoft.VisualStudio.TestTools.UnitTesting;
    using Moq;

    [TestClass]
    public class ConnectionPoolTests
    {
        [ExpectedException(typeof(ArgumentOutOfRangeException))]
        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("ConnectionPool")]
        public void ConnectionPoolTest1()
        {
            var ampqConnectionPoolSettings = new AmqpConnectionPoolSettings();
            var amqpTransportSettings = new AmqpTransportSettings(TransportType.Amqp_Tcp_Only, 200, ampqConnectionPoolSettings);
            string connectionString = "HostName=acme.azure-devices.net;DeviceId=device1;SharedAccessKey=CQN2K33r45/0WeIjpqmErV5EIvX8JZrozt3NEHCEkG8=";
            var iotHubConnectionString = IotHubConnectionStringBuilder.Create(connectionString).ToIotHubConnectionString();
            var hubConnection = new Mock<IotHubConnection>();

       //     connectionCache.Setup(cache => cache.GetConnection(It.IsAny<IotHubConnectionString>(), It.IsAny<AmqpTransportSettings>()).);
         //   var connectionPool = new Mock<IotHubDeviceScopeConnectionPool>();
        //    var connection = connectionCache.Object.GetConnection(iotHubConnectionString, amqpTransportSettings);
        }
    }
}
