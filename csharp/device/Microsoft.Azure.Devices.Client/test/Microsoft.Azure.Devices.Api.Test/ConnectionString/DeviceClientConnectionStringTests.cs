// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client.Test.ConnectionString
{
    using System;
    using System.Security.Cryptography.X509Certificates;
    using Microsoft.Azure.Devices.Client;
    using Microsoft.Azure.Devices.Client.ApiTest;
    using Microsoft.Azure.Devices.Client.Transport;
    using Microsoft.VisualStudio.TestTools.UnitTesting;

    [TestClass]
    public class DeviceClientConnectionStringTests
    {
        [TestMethod]
        [TestCategory("CIT")]
        public void DeviceClient_Create_DeviceScope_SharedAccessSignature_Test()
        {
            string hostName = "acme.azure-devices.net";
            string password = "CQN2K33r45/0WeIjpqmErV5EIvX8JZrozt3NEHCEkG8=";
            var sasRule = new SharedAccessSignatureBuilder()
            {
                Key = password,
                Target = hostName + "/devices/" + "device1"
            };
            var authMethod = new DeviceAuthenticationWithToken("device1", sasRule.ToSignature());
            var deviceClient = AmqpTransportHandler.Create(hostName, authMethod);

            Assert.IsNotNull(deviceClient.IotHubConnection);
            Assert.IsNotNull(((IotHubSingleTokenConnection)deviceClient.IotHubConnection).ConnectionString);
        }

        [TestMethod]
        [TestCategory("CIT")]
        public void DeviceClient_ConnectionString_DefaultScope_DefaultCredentialType_Test()
        {
            string connectionString = "HostName=acme.azure-devices.net;SharedAccessKeyName=AllAccessKey;DeviceId=device1;SharedAccessKey=CQN2K33r45/0WeIjpqmErV5EIvX8JZrozt3NEHCEkG8=";
            var deviceClient = AmqpTransportHandler.CreateFromConnectionString(connectionString);

            Assert.IsNotNull(deviceClient.IotHubConnection);
            Assert.IsNotNull(((IotHubSingleTokenConnection)deviceClient.IotHubConnection).ConnectionString);
            var iotHubConnectionStringBuilder = IotHubConnectionStringBuilder.Create(connectionString);
        }

        [TestMethod]
        [Owner("HillaryC")]
        [TestCategory("CIT")]
        public void DeviceClient_ConnectionString_IotHubScope_ImplicitSharedAccessSignatureCredentialType_Test()
        {
            string connectionString = "HostName=acme.azure-devices.net;DeviceId=device1;SharedAccessKeyName=AllAccessKey;SharedAccessKey=CQN2K33r45/0WeIjpqmErV5EIvX8JZrozt3NEHCEkG8=";
            var deviceClient = AmqpTransportHandler.CreateFromConnectionString(connectionString);

            Assert.IsNotNull(deviceClient.IotHubConnection);
            Assert.IsNotNull(((IotHubSingleTokenConnection)deviceClient.IotHubConnection).ConnectionString);
        }

        [TestMethod]
        [TestCategory("CIT")]
        public void DeviceClient_ConnectionString_IotHubScope_ExplicitSharedAccessSignatureCredentialType_Test()
        {
            string connectionString = "HostName=acme.azure-devices.net;DeviceId=device1;SharedAccessSignature=SharedAccessSignature sr=dh%3a%2f%2facme.azure-devices.net&sig=poifbMLdBGtCJknubF2FW6FLn5vND5k1IKoeQ%2bONgkE%3d&se=87824124985&skn=AllAccessKey";
            var deviceClient = AmqpTransportHandler.CreateFromConnectionString(connectionString);

            Assert.IsNotNull(deviceClient.IotHubConnection);
            Assert.IsNotNull(((IotHubSingleTokenConnection)deviceClient.IotHubConnection).ConnectionString);
        }

        [TestMethod]
        [TestCategory("CIT")]
        public void DeviceClient_ConnectionString_IotHubScope_SharedAccessKeyCredentialType_Test()
        {
            string connectionString = "HostName=acme.azure-devices.net;DeviceId=device1;SharedAccessKeyName=AllAccessKey;SharedAccessKey=CQN2K33r45/0WeIjpqmErV5EIvX8JZrozt3NEHCEkG8=";
            var deviceClient = AmqpTransportHandler.CreateFromConnectionString(connectionString);

            Assert.IsNotNull(deviceClient.IotHubConnection);
            Assert.IsNotNull(((IotHubSingleTokenConnection)deviceClient.IotHubConnection).ConnectionString);
        }

        [TestMethod]
        [TestCategory("CIT")]
        public void DeviceClient_ConnectionString_X509Certificate_Test()
        {
            string hostName = "acme.azure-devices.net";
            string password = "CQN2K33r45/0WeIjpqmErV5EIvX8JZrozt3NEHCEkG8=";

            var cert = CertificateHelper.InstallCertificateFromFile("device.pfx", "device.txt");
            var authMethod = new DeviceAuthenticationWithX509Certificate("device1", cert);
            var deviceClient = AmqpTransportHandler.Create(hostName, authMethod);

            Assert.IsNotNull(deviceClient.Connection);
            Assert.IsNotNull(deviceClient.Connection.ConnectionString);
        }

        [TestMethod]
        [TestCategory("CIT")]
        public void DeviceClient_IotHubConnectionStringBuilder_Test()
        {
            string connectionString = "HostName=acme.azure-devices.net;SharedAccessKeyName=AllAccessKey;DeviceId=device1;SharedAccessKey=CQN2K33r45/0WeIjpqmErV5EIvX8JZrozt3NEHCEkG8=";
            var iotHubConnectionStringBuilder = IotHubConnectionStringBuilder.Create(connectionString);
            Assert.IsNotNull(iotHubConnectionStringBuilder.HostName);
            Assert.IsNotNull(iotHubConnectionStringBuilder.DeviceId);
            Assert.IsNotNull(iotHubConnectionStringBuilder.AuthenticationMethod);
            Assert.IsNotNull(iotHubConnectionStringBuilder.SharedAccessKey);
            Assert.IsNotNull(iotHubConnectionStringBuilder.SharedAccessKeyName);
            Assert.IsNull(iotHubConnectionStringBuilder.SharedAccessSignature);
            Assert.IsTrue(iotHubConnectionStringBuilder.AuthenticationMethod is DeviceAuthenticationWithSharedAccessPolicyKey);

            connectionString = "HostName=acme.azure-devices.net;CredentialType=SharedAccessSignature;DeviceId=device1;SharedAccessSignature=SharedAccessSignature sr=dh%3a%2f%2facme.azure-devices.net&sig=poifbMLdBGtCJknubF2FW6FLn5vND5k1IKoeQ%2bONgkE%3d&se=87824124985&skn=AllAccessKey";
            iotHubConnectionStringBuilder = IotHubConnectionStringBuilder.Create(connectionString);
            Assert.IsNotNull(iotHubConnectionStringBuilder.HostName);
            Assert.IsNotNull(iotHubConnectionStringBuilder.DeviceId);
            Assert.IsNotNull(iotHubConnectionStringBuilder.AuthenticationMethod);
            Assert.IsNull(iotHubConnectionStringBuilder.SharedAccessKey);
            Assert.IsNotNull(iotHubConnectionStringBuilder.SharedAccessSignature);
            Assert.IsTrue(iotHubConnectionStringBuilder.AuthenticationMethod is DeviceAuthenticationWithToken);

            connectionString = "HostName=acme.azure-devices.net;CredentialScope=Device;DeviceId=device1;SharedAccessKey=CQN2K33r45/0WeIjpqmErV5EIvX8JZrozt3NEHCEkG8=";
            iotHubConnectionStringBuilder = IotHubConnectionStringBuilder.Create(connectionString);
            Assert.IsNotNull(iotHubConnectionStringBuilder.HostName);
            Assert.IsNotNull(iotHubConnectionStringBuilder.DeviceId);
            Assert.IsNotNull(iotHubConnectionStringBuilder.AuthenticationMethod);
            Assert.IsNotNull(iotHubConnectionStringBuilder.SharedAccessKey);
            Assert.IsNull(iotHubConnectionStringBuilder.SharedAccessKeyName);
            Assert.IsNull(iotHubConnectionStringBuilder.SharedAccessSignature);
            Assert.IsTrue(iotHubConnectionStringBuilder.AuthenticationMethod is DeviceAuthenticationWithRegistrySymmetricKey);

            connectionString = "HostName=acme.azure-devices.net;CredentialScope=Device;DeviceId=device1;SharedAccessSignature=SharedAccessSignature sr=dh%3a%2f%2facme.azure-devices.net&sig=poifbMLdBGtCJknubF2FW6FLn5vND5k1IKoeQ%2bONgkE%3d&se=87824124985&skn=AllAccessKey";
            iotHubConnectionStringBuilder = IotHubConnectionStringBuilder.Create(connectionString);
            Assert.IsNotNull(iotHubConnectionStringBuilder.HostName);
            Assert.IsNotNull(iotHubConnectionStringBuilder.DeviceId);
            Assert.IsNotNull(iotHubConnectionStringBuilder.AuthenticationMethod);
            Assert.IsNull(iotHubConnectionStringBuilder.SharedAccessKey);
            Assert.IsNull(iotHubConnectionStringBuilder.SharedAccessKeyName);
            Assert.IsNotNull(iotHubConnectionStringBuilder.SharedAccessSignature);
            Assert.IsTrue(iotHubConnectionStringBuilder.AuthenticationMethod is DeviceAuthenticationWithToken);

            try
            {
                iotHubConnectionStringBuilder.HostName = "adshgfvyregferuehfiuehr";
                Assert.Fail("Expected FormatException");
            }
            catch (FormatException)
            {               
            }

            iotHubConnectionStringBuilder.HostName = "acme.azure-devices.net";
            iotHubConnectionStringBuilder.AuthenticationMethod = new DeviceAuthenticationWithRegistrySymmetricKey("Device1", "CQN2K33r45/0WeIjpqmErV5EIvX8JZrozt3NEHCEkG8=");
            Assert.IsTrue(iotHubConnectionStringBuilder.AuthenticationMethod is DeviceAuthenticationWithRegistrySymmetricKey);
            Assert.IsTrue(iotHubConnectionStringBuilder.SharedAccessSignature == null);
            Assert.IsTrue(iotHubConnectionStringBuilder.SharedAccessKey == "CQN2K33r45/0WeIjpqmErV5EIvX8JZrozt3NEHCEkG8=");
            Assert.IsTrue(iotHubConnectionStringBuilder.DeviceId=="Device1");

            iotHubConnectionStringBuilder.AuthenticationMethod = new DeviceAuthenticationWithToken("Device2", "SharedAccessSignature sr=dh%3a%2f%2facme.azure-devices.net&sig=poifbMLdBGtCJknubF2FW6FLn5vND5k1IKoeQ%2bONgkE%3d&se=87824124985&skn=AllAccessKey");
            Assert.IsTrue(iotHubConnectionStringBuilder.AuthenticationMethod is DeviceAuthenticationWithToken);
            Assert.IsTrue(iotHubConnectionStringBuilder.SharedAccessSignature == "SharedAccessSignature sr=dh%3a%2f%2facme.azure-devices.net&sig=poifbMLdBGtCJknubF2FW6FLn5vND5k1IKoeQ%2bONgkE%3d&se=87824124985&skn=AllAccessKey");
            Assert.IsTrue(iotHubConnectionStringBuilder.SharedAccessKey == null);
            Assert.IsTrue(iotHubConnectionStringBuilder.DeviceId == "Device2");

            iotHubConnectionStringBuilder.AuthenticationMethod = new DeviceAuthenticationWithSharedAccessPolicyKey("Device3", "AllAccess", "KQN2K33r45/0WeIjpqmErV5EIvX8JZrozt3NEHCEkG8=");
            Assert.IsTrue(iotHubConnectionStringBuilder.AuthenticationMethod is DeviceAuthenticationWithSharedAccessPolicyKey);
            Assert.IsTrue(iotHubConnectionStringBuilder.SharedAccessSignature == null);
            Assert.IsTrue(iotHubConnectionStringBuilder.SharedAccessKey == "KQN2K33r45/0WeIjpqmErV5EIvX8JZrozt3NEHCEkG8=");
            Assert.IsTrue(iotHubConnectionStringBuilder.DeviceId == "Device3");

            iotHubConnectionStringBuilder.AuthenticationMethod = new DeviceAuthenticationWithToken("Device4", "SharedAccessSignature sr=dh%3a%2f%2facme.azure-devices.net&sig=poifbMLdBGtCJknubF2FW6FLn5vND5k1IKoeQ%2bONgkE%3d&se=87824124985&skn=AllAccessKey");
            Assert.IsTrue(iotHubConnectionStringBuilder.AuthenticationMethod is DeviceAuthenticationWithToken);
            Assert.IsTrue(iotHubConnectionStringBuilder.SharedAccessSignature == "SharedAccessSignature sr=dh%3a%2f%2facme.azure-devices.net&sig=poifbMLdBGtCJknubF2FW6FLn5vND5k1IKoeQ%2bONgkE%3d&se=87824124985&skn=AllAccessKey");
            Assert.IsTrue(iotHubConnectionStringBuilder.SharedAccessKey == null);
            Assert.IsTrue(iotHubConnectionStringBuilder.DeviceId == "Device4");

            IAuthenticationMethod authMethod = AuthenticationMethodFactory.CreateAuthenticationWithToken("Device5", "SharedAccessSignature sr=dh%3a%2f%2facme.azure-devices.net&sig=poifbMLdBGtCJknubF2FW6FLn5vND5k1IKoeQ%2bONgkE%3d&se=87824124985&skn=AllAccessKey");
            iotHubConnectionStringBuilder = IotHubConnectionStringBuilder.Create("acme1.azure-devices.net", authMethod);
            Assert.IsTrue(iotHubConnectionStringBuilder.AuthenticationMethod is DeviceAuthenticationWithToken);
            Assert.IsTrue(iotHubConnectionStringBuilder.HostName == "acme1.azure-devices.net");
            Assert.IsTrue(iotHubConnectionStringBuilder.SharedAccessSignature == "SharedAccessSignature sr=dh%3a%2f%2facme.azure-devices.net&sig=poifbMLdBGtCJknubF2FW6FLn5vND5k1IKoeQ%2bONgkE%3d&se=87824124985&skn=AllAccessKey");
            Assert.IsTrue(iotHubConnectionStringBuilder.SharedAccessKey == null);
            Assert.IsTrue(iotHubConnectionStringBuilder.DeviceId == "Device5");

            authMethod = new DeviceAuthenticationWithSharedAccessPolicyKey("Device3", "AllAccess", "KQN2K33r45/0WeIjpqmErV5EIvX8JZrozt3NEHCEkG8=");
            iotHubConnectionStringBuilder = IotHubConnectionStringBuilder.Create("acme2.azure-devices.net", authMethod);
            Assert.IsTrue(iotHubConnectionStringBuilder.AuthenticationMethod is DeviceAuthenticationWithSharedAccessPolicyKey);
            Assert.IsTrue(iotHubConnectionStringBuilder.HostName == "acme2.azure-devices.net");
            Assert.IsTrue(iotHubConnectionStringBuilder.SharedAccessSignature == null);
            Assert.IsTrue(iotHubConnectionStringBuilder.SharedAccessKey == "KQN2K33r45/0WeIjpqmErV5EIvX8JZrozt3NEHCEkG8=");
            Assert.IsTrue(iotHubConnectionStringBuilder.DeviceId == "Device3");

            authMethod = new DeviceAuthenticationWithToken("Device2", "SharedAccessSignature sr=dh%3a%2f%2facme.azure-devices.net&sig=poifbMLdBGtCJknubF2FW6FLn5vND5k1IKoeQ%2bONgkE%3d&se=87824124985&skn=AllAccessKey");
            iotHubConnectionStringBuilder = IotHubConnectionStringBuilder.Create("acme3.azure-devices.net", authMethod);
            Assert.IsTrue(iotHubConnectionStringBuilder.AuthenticationMethod is DeviceAuthenticationWithToken);
            Assert.IsTrue(iotHubConnectionStringBuilder.HostName == "acme3.azure-devices.net");
            Assert.IsTrue(iotHubConnectionStringBuilder.SharedAccessSignature == "SharedAccessSignature sr=dh%3a%2f%2facme.azure-devices.net&sig=poifbMLdBGtCJknubF2FW6FLn5vND5k1IKoeQ%2bONgkE%3d&se=87824124985&skn=AllAccessKey");
            Assert.IsTrue(iotHubConnectionStringBuilder.SharedAccessKey == null);
            Assert.IsTrue(iotHubConnectionStringBuilder.DeviceId == "Device2");

            authMethod = new DeviceAuthenticationWithRegistrySymmetricKey("Device1", "CQN2K33r45/0WeIjpqmErV5EIvX8JZrozt3NEHCEkG8=");
            iotHubConnectionStringBuilder = IotHubConnectionStringBuilder.Create("acme4.azure-devices.net", authMethod);
            Assert.IsTrue(iotHubConnectionStringBuilder.AuthenticationMethod is DeviceAuthenticationWithRegistrySymmetricKey);
            Assert.IsTrue(iotHubConnectionStringBuilder.HostName == "acme4.azure-devices.net");
            Assert.IsTrue(iotHubConnectionStringBuilder.SharedAccessSignature == null);
            Assert.IsTrue(iotHubConnectionStringBuilder.SharedAccessKey == "CQN2K33r45/0WeIjpqmErV5EIvX8JZrozt3NEHCEkG8=");
            Assert.IsTrue(iotHubConnectionStringBuilder.DeviceId == "Device1");
        }
    }
}

