// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information

namespace Microsoft.Azure.Devices.Client.Test
{
    using System;
    using Microsoft.Azure.Devices.Client;
    using Microsoft.Azure.Devices.Client.ApiTest;
    using Microsoft.Azure.Devices.Client.Transport.Mqtt;
    using Microsoft.VisualStudio.TestTools.UnitTesting;

    [TestClass]
    public class TransportSettingsTests
    {
        const string LocalCertFilename = "..\\..\\Microsoft.Azure.Devices.Api.Test\\LocalNoChain.pfx";
        const string LocalCertPasswordFile = "..\\..\\Microsoft.Azure.Devices.Api.Test\\TestCertsPassword.txt";

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
        public void TransportSettingsTest_TransportType_Mqtt_Tcp_Only()
        {
            var transportSetting = new MqttTransportSettings(TransportType.Mqtt_Tcp_Only);
            Assert.IsTrue(transportSetting.GetTransportType() == TransportType.Mqtt_Tcp_Only, "Should be TransportType.Mqtt_Tcp_Only");
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("TransportSettings")]
        public void TransportSettingsTest_TransportType_Mqtt_WebSocket_Only()
        {
            var transportSetting = new MqttTransportSettings(TransportType.Mqtt_WebSocket_Only);
            Assert.IsTrue(transportSetting.GetTransportType() == TransportType.Mqtt_WebSocket_Only, "Should be TransportType.Mqtt_WebSocket_Only");
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("TransportSettings")]
        [ExpectedException(typeof(ArgumentOutOfRangeException))]
        public void TransportSettingsTest_TransportType_Mqtt()
        {
            new MqttTransportSettings(TransportType.Mqtt);
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
        public void ConnectionPoolSettingsTest_4SecsIdleTimeout()
        {
            var connectionPoolSettings = new AmqpConnectionPoolSettings();
            connectionPoolSettings.ConnectionIdleTimeout = TimeSpan.FromSeconds(4);
            var transportSetting = new AmqpTransportSettings(TransportType.Amqp, 200, connectionPoolSettings);
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("TransportSettings")]
        public void ConnectionPoolSettingsTest_MaxPoolSizeTest()
        {
            var connectionPoolSettings = new AmqpConnectionPoolSettings();
            connectionPoolSettings.MaxPoolSize = ushort.MaxValue;
            var transportSetting = new AmqpTransportSettings(TransportType.Amqp_Tcp_Only, 200, connectionPoolSettings);
            Assert.IsTrue(transportSetting.AmqpConnectionPoolSettings.MaxPoolSize == ushort.MaxValue, "MaxPoolSize should be 64K");
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("TransportSettings")]
        public void ConnectionPoolSettingsTest_PoolingOff()
        {
            var connectionPoolSettings = new AmqpConnectionPoolSettings();
            connectionPoolSettings.Pooling = false;
            var transportSetting = new AmqpTransportSettings(TransportType.Amqp_Tcp_Only, 200, connectionPoolSettings);
            Assert.IsTrue(transportSetting.AmqpConnectionPoolSettings.Pooling == false, "Pooling should be off");
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("TransportSettings")]
        public void X509Certificate_AmqpTransportSettingsTest()
        {
            string hostName = "acme.azure-devices.net";
            var cert = CertificateHelper.InstallCertificateFromFile(LocalCertFilename, LocalCertPasswordFile);
            var authMethod = new DeviceAuthenticationWithX509Certificate("device1", cert);

            var deviceClient = DeviceClient.Create(hostName, authMethod, new ITransportSettings[] { new AmqpTransportSettings(TransportType.Amqp_Tcp_Only, 100) });
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("TransportSettings")]
        public void X509Certificate_Http1TransportSettingsTest()
        {
            string hostName = "acme.azure-devices.net";
            var cert = CertificateHelper.InstallCertificateFromFile(LocalCertFilename, LocalCertPasswordFile);
            var authMethod = new DeviceAuthenticationWithX509Certificate("device1", cert);

            var deviceClient = DeviceClient.Create(hostName, authMethod, new ITransportSettings[] { new Http1TransportSettings()});
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("TransportSettings")]
        public void X509Certificate_MqttTransportSettingsTest()
        {
            string hostName = "acme.azure-devices.net";
            var cert = CertificateHelper.InstallCertificateFromFile(LocalCertFilename, LocalCertPasswordFile);
            var authMethod = new DeviceAuthenticationWithX509Certificate("device1", cert);

            var deviceClient = DeviceClient.Create(hostName, authMethod, new ITransportSettings[]
            {
                new MqttTransportSettings(TransportType.Mqtt_Tcp_Only)
                {
                    ClientCertificate = cert,
                    RemoteCertificateValidationCallback = (a, b, c, d) => true
                },
                new MqttTransportSettings(TransportType.Mqtt_WebSocket_Only)
                {
                    ClientCertificate = cert,
                    RemoteCertificateValidationCallback = (a, b, c, d) => true
                }
            });
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("TransportSettings")]
        [ExpectedException(typeof(ArgumentException))]
        public void NullX509Certificate_AmqpTransportSettingsTest()
        {
            string hostName = "acme.azure-devices.net";
            var authMethod = new DeviceAuthenticationWithX509Certificate("device1", null);

            var deviceClient = DeviceClient.Create(hostName, authMethod, new ITransportSettings[] { new AmqpTransportSettings(TransportType.Amqp_Tcp_Only, 100) });
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("TransportSettings")]
        public void X509Certificate_MutipleClientAuthMechanism()
        {
            string hostName = "acme.azure-devices.net";
            var amqpConnectionPoolSettings = new AmqpConnectionPoolSettings();
            var transportSetting = new AmqpTransportSettings(TransportType.Amqp_Tcp_Only, 200, amqpConnectionPoolSettings);
            var authMethod1 = new DeviceAuthenticationWithRegistrySymmetricKey("device1", "CQN2K33r45/0WeIjpqmErV5EIvX8JZrozt3NEHCEkG8=");
            var deviceClient = DeviceClient.Create(hostName, authMethod1, new ITransportSettings[] { transportSetting });

            var cert = CertificateHelper.InstallCertificateFromFile(LocalCertFilename, LocalCertPasswordFile);
            var authMethod2 = new DeviceAuthenticationWithX509Certificate("device2", cert);
            var device2Client = DeviceClient.Create(hostName, authMethod2, new ITransportSettings[] { new AmqpTransportSettings(TransportType.Amqp_Tcp_Only, 100) });
        }
    }
}