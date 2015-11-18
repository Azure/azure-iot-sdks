// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Api.Test.ConnectionString
{
    using System;
    using Microsoft.Azure.Devices;
    using Microsoft.VisualStudio.TestTools.UnitTesting;

    [TestClass]
    public class ServiceClientConnectionStringTests
    {
        class TestAuthenticationMethod : IAuthenticationMethod
        {
            public virtual IotHubConnectionStringBuilder Populate(IotHubConnectionStringBuilder iotHubConnectionStringBuilder)
            {
                // intentionally set SharedAccessKeyName to null;
                iotHubConnectionStringBuilder.SharedAccessKeyName = null;
                iotHubConnectionStringBuilder.SharedAccessKey = "CQN2K33r45/0WeIjpqmErV5EIvX8JZrozt3NEHCEkG8=";
                return iotHubConnectionStringBuilder;
            }
        }

        [TestMethod]
        [TestCategory("CIT")]
        public void ServiceClient_ConnectionString_DefaultScope_DefaultCredentialType_Test()
        {
            string connectionString = "HostName=acme.azure-devices.net;SharedAccessKeyName=AllAccessKey;SharedAccessKey=CQN2K33r45/0WeIjpqmErV5EIvX8JZrozt3NEHCEkG8=";
            var serviceClient = (AmqpServiceClient)ServiceClient.CreateFromConnectionString(connectionString);

            Assert.IsNotNull(serviceClient.Connection);
            Assert.IsNotNull(serviceClient.Connection.ConnectionString);
        }

        [TestMethod]
        [TestCategory("CIT")]
        public void ServiceClient_ConnectionString_IotHubScope_ImplicitSharedAccessSignatureCredentialType_Test()
        {
            string connectionString = "HostName=acme.azure-devices.net;CredentialScope=IotHub;CredentialType=SharedAccessSignature;SharedAccessKeyName=AllAccessKey;SharedAccessKey=CQN2K33r45/0WeIjpqmErV5EIvX8JZrozt3NEHCEkG8=";
            var serviceClient = (AmqpServiceClient)ServiceClient.CreateFromConnectionString(connectionString);

            Assert.IsNotNull(serviceClient.Connection);
            Assert.IsNotNull(serviceClient.Connection.ConnectionString);
        }

        [TestMethod]
        [TestCategory("CIT")]
        public void ServiceClient_ConnectionString_IotHubScope_ExplicitSharedAccessSignatureCredentialType_Test()
        {
            string connectionString = "HostName=acme.azure-devices.net;CredentialScope=IotHub;CredentialType=SharedAccessSignature;SharedAccessKeyName=AllAccessKey;SharedAccessSignature=SharedAccessSignature sr=dh%3a%2f%2facme.azure-devices.net&sig=poifbMLdBGtCJknubF2FW6FLn5vND5k1IKoeQ%2bONgkE%3d&se=87824124985&skn=AllAccessKey";
            var serviceClient = (AmqpServiceClient)ServiceClient.CreateFromConnectionString(connectionString);

            Assert.IsNotNull(serviceClient.Connection);
            Assert.IsNotNull(serviceClient.Connection.ConnectionString);
        }

        [TestMethod]
        [TestCategory("CIT")]
        public void ServiceClient_ConnectionString_IotHubScope_SharedAccessKeyCredentialType_Test()
        {
            string connectionString = "HostName=acme.azure-devices.net;CredentialScope=IotHub;CredentialType=SharedAccessKey;SharedAccessKeyName=AllAccessKey;SharedAccessKey=CQN2K33r45/0WeIjpqmErV5EIvX8JZrozt3NEHCEkG8=";
            var serviceClient = (AmqpServiceClient)ServiceClient.CreateFromConnectionString(connectionString);

            Assert.IsNotNull(serviceClient.Connection);
            Assert.IsNotNull(serviceClient.Connection.ConnectionString);
        }

        [TestMethod]
        [TestCategory("CIT")]
        public void ServiceClient_ConnectionString_DeviceScope_ImplicitSharedAccessSignatureCredentialType_Test()
        {
            string connectionString = "HostName=acme.azure-devices.net;CredentialScope=IotHub;CredentialType=SharedAccessSignature;SharedAccessKeyName=blah;SharedAccessKey=CQN2K33r45/0WeIjpqmErV5EIvX8JZrozt3NEHCEkG8=";
            var serviceClient = (AmqpServiceClient)ServiceClient.CreateFromConnectionString(connectionString);

            Assert.IsNotNull(serviceClient.Connection);
            Assert.IsNotNull(serviceClient.Connection.ConnectionString);
        }

        [TestMethod]
        [TestCategory("CIT")]
        public void ServiceClient_ConnectionString_DeviceScope_ExplicitSharedAccessSignatureCredentialType_Test()
        {
            string connectionString = "HostName=acme.azure-devices.net;CredentialScope=IotHub;CredentialType=SharedAccessSignature;SharedAccessKeyName=blah;SharedAccessSignature=SharedAccessSignature sr=dh%3a%2f%2facme.azure-devices.net&sig=poifbMLdBGtCJknubF2FW6FLn5vND5k1IKoeQ%2bONgkE%3d&se=87824124985&skn=AllAccessKey";
            var serviceClient = (AmqpServiceClient)ServiceClient.CreateFromConnectionString(connectionString);

            Assert.IsNotNull(serviceClient.Connection);
            Assert.IsNotNull(serviceClient.Connection.ConnectionString);
        }

        [TestMethod]
        [TestCategory("CIT")]
        public void ServiceClient_ConnectionString_DeviceScope_SharedAccessKeyCredentialType_Test()
        {
            string connectionString = "HostName=acme.azure-devices.net;CredentialScope=IotHub;CredentialType=SharedAccessKey;SharedAccessKeyName=blah;SharedAccessKey=CQN2K33r45/0WeIjpqmErV5EIvX8JZrozt3NEHCEkG8=";
            var serviceClient = (AmqpServiceClient)ServiceClient.CreateFromConnectionString(connectionString);

            Assert.IsNotNull(serviceClient.Connection);
            Assert.IsNotNull(serviceClient.Connection.ConnectionString);
        }

        [TestMethod]
        [TestCategory("CIT")]
        public void ServiceClient_IotHubConnectionStringBuilder_Test()
        {
            string connectionString = "HostName=acme.azure-devices.net;SharedAccessKeyName=AllAccessKey;SharedAccessKey=CQN2K33r45/0WeIjpqmErV5EIvX8JZrozt3NEHCEkG8=";
            var iotHubConnectionStringBuilder = IotHubConnectionStringBuilder.Create(connectionString);
            Assert.IsNotNull(iotHubConnectionStringBuilder.HostName);
            Assert.IsNotNull(iotHubConnectionStringBuilder.AuthenticationMethod);
            Assert.IsNotNull(iotHubConnectionStringBuilder.SharedAccessKey);
            Assert.IsNotNull(iotHubConnectionStringBuilder.SharedAccessKeyName);
            Assert.IsNull(iotHubConnectionStringBuilder.SharedAccessSignature);
            Assert.IsTrue(iotHubConnectionStringBuilder.AuthenticationMethod is ServiceAuthenticationWithSharedAccessPolicyKey);

            connectionString = "HostName=acme.azure-devices.net;CredentialType=SharedAccessSignature;SharedAccessKeyName=AllAccessKey;SharedAccessSignature=SharedAccessSignature sr=dh%3a%2f%2facme.azure-devices.net&sig=poifbMLdBGtCJknubF2FW6FLn5vND5k1IKoeQ%2bONgkE%3d&se=87824124985&skn=AllAccessKey";
            iotHubConnectionStringBuilder = IotHubConnectionStringBuilder.Create(connectionString);
            Assert.IsNotNull(iotHubConnectionStringBuilder.HostName);
            Assert.IsNotNull(iotHubConnectionStringBuilder.AuthenticationMethod);
            Assert.IsNull(iotHubConnectionStringBuilder.SharedAccessKey);
            Assert.IsNotNull(iotHubConnectionStringBuilder.SharedAccessKeyName);
            Assert.IsNotNull(iotHubConnectionStringBuilder.SharedAccessSignature);
            Assert.IsTrue(iotHubConnectionStringBuilder.AuthenticationMethod is ServiceAuthenticationWithSharedAccessPolicyToken);

            try
            {
                iotHubConnectionStringBuilder.HostName = "adshgfvyregferuehfiuehr";
                Assert.Fail("Expected FormatException");
            }
            catch (FormatException)
            {
            }

            try
            {
                iotHubConnectionStringBuilder.HostName = "acme.azure-devices.net";
                iotHubConnectionStringBuilder.AuthenticationMethod = new TestAuthenticationMethod();
                Assert.Fail("Expected ArgumentException");
            }
            catch (ArgumentException e)
            {
                Assert.IsTrue(e.Message.Contains("SharedAccessKey"));
            }

            iotHubConnectionStringBuilder.HostName = "acme.azure-devices.net";
            iotHubConnectionStringBuilder.AuthenticationMethod = new ServiceAuthenticationWithSharedAccessPolicyKey("AllAccessKey1", "CQN2K33r45/0WeIjpqmErV5EIvX8JZrozt3NEHCEkG8=");
            Assert.IsTrue(iotHubConnectionStringBuilder.AuthenticationMethod is ServiceAuthenticationWithSharedAccessPolicyKey);
            Assert.IsTrue(iotHubConnectionStringBuilder.SharedAccessSignature == null);
            Assert.IsTrue(iotHubConnectionStringBuilder.SharedAccessKeyName == "AllAccessKey1");
            Assert.IsTrue(iotHubConnectionStringBuilder.SharedAccessKey == "CQN2K33r45/0WeIjpqmErV5EIvX8JZrozt3NEHCEkG8=");

            iotHubConnectionStringBuilder.AuthenticationMethod = new ServiceAuthenticationWithSharedAccessPolicyToken("AllAccessKey2", "SharedAccessSignature sr=dh%3a%2f%2facme.azure-devices.net&sig=poifbMLdBGtCJknubF2FW6FLn5vND5k1IKoeQ%2bONgkE%3d&se=87824124985&skn=AllAccessKey");
            Assert.IsTrue(iotHubConnectionStringBuilder.AuthenticationMethod is ServiceAuthenticationWithSharedAccessPolicyToken);
            Assert.IsTrue(iotHubConnectionStringBuilder.SharedAccessKeyName == "AllAccessKey2");
            Assert.IsTrue(iotHubConnectionStringBuilder.SharedAccessSignature == "SharedAccessSignature sr=dh%3a%2f%2facme.azure-devices.net&sig=poifbMLdBGtCJknubF2FW6FLn5vND5k1IKoeQ%2bONgkE%3d&se=87824124985&skn=AllAccessKey");
            Assert.IsTrue(iotHubConnectionStringBuilder.SharedAccessKey == null);

            IAuthenticationMethod authMethod = new ServiceAuthenticationWithSharedAccessPolicyToken("AllAccess1", "SharedAccessSignature sr=dh%3a%2f%2facme.azure-devices.net&sig=poifbMLdBGtCJknubF2FW6FLn5vND5k1IKoeQ%2bONgkE%3d&se=87824124985&skn=AllAccessKey");
            iotHubConnectionStringBuilder = IotHubConnectionStringBuilder.Create("acme1.azure-devices.net", authMethod);
            Assert.IsTrue(iotHubConnectionStringBuilder.AuthenticationMethod is ServiceAuthenticationWithSharedAccessPolicyToken);
            Assert.IsTrue(iotHubConnectionStringBuilder.HostName == "acme1.azure-devices.net");
            Assert.IsTrue(iotHubConnectionStringBuilder.SharedAccessKeyName == "AllAccess1");
            Assert.IsTrue(iotHubConnectionStringBuilder.SharedAccessSignature == "SharedAccessSignature sr=dh%3a%2f%2facme.azure-devices.net&sig=poifbMLdBGtCJknubF2FW6FLn5vND5k1IKoeQ%2bONgkE%3d&se=87824124985&skn=AllAccessKey");
            Assert.IsTrue(iotHubConnectionStringBuilder.SharedAccessKey == null);

            authMethod = new ServiceAuthenticationWithSharedAccessPolicyKey("AllAccess2", "KQN2K33r45/0WeIjpqmErV5EIvX8JZrozt3NEHCEkG8=");
            iotHubConnectionStringBuilder = IotHubConnectionStringBuilder.Create("acme2.azure-devices.net", authMethod);
            Assert.IsTrue(iotHubConnectionStringBuilder.AuthenticationMethod is ServiceAuthenticationWithSharedAccessPolicyKey);
            Assert.IsTrue(iotHubConnectionStringBuilder.SharedAccessKeyName == "AllAccess2");
            Assert.IsTrue(iotHubConnectionStringBuilder.HostName == "acme2.azure-devices.net");
            Assert.IsTrue(iotHubConnectionStringBuilder.SharedAccessSignature == null);
            Assert.IsTrue(iotHubConnectionStringBuilder.SharedAccessKey == "KQN2K33r45/0WeIjpqmErV5EIvX8JZrozt3NEHCEkG8=");

            authMethod = AuthenticationMethodFactory.CreateAuthenticationWithSharedAccessPolicyKey("AllAccess2", "KQN2K33r45/0WeIjpqmErV5EIvX8JZrozt3NEHCEkG8=");
            iotHubConnectionStringBuilder = IotHubConnectionStringBuilder.Create("acme2.azure-devices.net", authMethod);
            Assert.IsTrue(iotHubConnectionStringBuilder.AuthenticationMethod is ServiceAuthenticationWithSharedAccessPolicyKey);
            Assert.IsTrue(iotHubConnectionStringBuilder.SharedAccessKeyName == "AllAccess2");
            Assert.IsTrue(iotHubConnectionStringBuilder.HostName == "acme2.azure-devices.net");
            Assert.IsTrue(iotHubConnectionStringBuilder.SharedAccessSignature == null);
            Assert.IsTrue(iotHubConnectionStringBuilder.SharedAccessKey == "KQN2K33r45/0WeIjpqmErV5EIvX8JZrozt3NEHCEkG8=");

            authMethod = AuthenticationMethodFactory.CreateAuthenticationWithSharedAccessPolicyToken("AllAccess1", "SharedAccessSignature sr=dh%3a%2f%2facme.azure-devices.net&sig=poifbMLdBGtCJknubF2FW6FLn5vND5k1IKoeQ%2bONgkE%3d&se=87824124985&skn=AllAccessKey");
            iotHubConnectionStringBuilder = IotHubConnectionStringBuilder.Create("acme1.azure-devices.net", authMethod);
            Assert.IsTrue(iotHubConnectionStringBuilder.AuthenticationMethod is ServiceAuthenticationWithSharedAccessPolicyToken);
            Assert.IsTrue(iotHubConnectionStringBuilder.HostName == "acme1.azure-devices.net");
            Assert.IsTrue(iotHubConnectionStringBuilder.SharedAccessKeyName == "AllAccess1");
            Assert.IsTrue(iotHubConnectionStringBuilder.SharedAccessSignature == "SharedAccessSignature sr=dh%3a%2f%2facme.azure-devices.net&sig=poifbMLdBGtCJknubF2FW6FLn5vND5k1IKoeQ%2bONgkE%3d&se=87824124985&skn=AllAccessKey");
            Assert.IsTrue(iotHubConnectionStringBuilder.SharedAccessKey == null);
        }
    }
}

