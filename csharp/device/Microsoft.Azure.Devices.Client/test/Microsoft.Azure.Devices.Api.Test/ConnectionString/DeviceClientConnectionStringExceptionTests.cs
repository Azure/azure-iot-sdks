//---------------------------------------------------------------
// Copyright (c) Microsoft Corporation. All rights reserved.
//---------------------------------------------------------------

namespace Microsoft.Azure.Devices.Client.Test.ConnectionString
{
    using System;

    using Microsoft.VisualStudio.TestTools.UnitTesting;
    using Microsoft.Azure.Devices.Client;

    [TestClass]
    public class DeviceClientConnectionStringExceptionTests
    {
        [TestMethod]
        [TestCategory("CIT")]
        [ExpectedException(typeof(ArgumentException))]
        public void DeviceClient_ConnectionString_DefaultScope_DefaultCredentialType_MissingEndpoint_ExceptionTest()
        {
            string connectionString = "SharedAccessKeyName=AllAccessKey;DeviceId=device1;SharedAccessKey=CQN2K33r45/0WeIjpqmErV5EIvX8JZrozt3NEHCEkG8=";
            var deviceClient = DeviceClient.CreateFromConnectionString(connectionString);
        }

        [TestMethod]
        [TestCategory("CIT")]
        [ExpectedException(typeof(ArgumentException))]
        public void DeviceClient_ConnectionString_DefaultScope_DefaultCredentialType_MissingDeviceId_ExceptionTest()
        {
            string connectionString = "HostName=acme.azure-devices.net;SharedAccessKey=CQN2K33r45/0WeIjpqmErV5EIvX8JZrozt3NEHCEkG8=";
            var deviceClient = DeviceClient.CreateFromConnectionString(connectionString);
        }

        [TestMethod]
        [TestCategory("CIT")]
        [ExpectedException(typeof(ArgumentException))]
        public void DeviceClient_ConnectionString_DefaultScope_DefaultCredentialType_MissingSharedAccessKey_ExceptionTest()
        {
            string connectionString = "HostName=acme.azure-devices.net;DeviceId=device1;SharedAccessKeyName=AllAccessKey"; 
            var deviceClient = DeviceClient.CreateFromConnectionString(connectionString);
        }

        [TestMethod]
        [TestCategory("CIT")]
        [ExpectedException(typeof(ArgumentException))]
        public void DeviceClient_ConnectionString_DefaultScope_DefaultCredentialType_MissingSharedAccessKeyNameAndKey_ExceptionTest()
        {
            string connectionString = "HostName=acme.azure-devices.net;DeviceId=device1";
            var deviceClient = DeviceClient.CreateFromConnectionString(connectionString);
        }

        [TestMethod]
        [TestCategory("CIT")]
        [ExpectedException(typeof(ArgumentException))]
        public void DeviceClient_ConnectionString_IotHubScope_SharedAccessSignatureCredentialType_MissingSharedAccessKey_ExceptionTest()
        {
            string connectionString = "HostName=acme.azure-devices.net;CredentialScope=IotHub;CredentialType=SharedAccessSignature;DeviceId=device1;SharedAccessKeyName=AllAccessKey";
            var deviceClient = DeviceClient.CreateFromConnectionString(connectionString);
        }

        [TestMethod]
        [TestCategory("CIT")]
        [ExpectedException(typeof(ArgumentException))]
        public void DeviceClient_ConnectionString_IotHubScope_SharedAccessKeyCredentialType_MissingSharedAccessKeyNameAndKey_ExceptionTest()
        {
            string connectionString = "HostName=acme.azure-devices.net;CredentialScope=IotHub;CredentialType=SharedAccessKey;DeviceId=device1";
            var deviceClient = DeviceClient.CreateFromConnectionString(connectionString);
        }

        [TestMethod]
        [TestCategory("CIT")]
        [ExpectedException(typeof(ArgumentException))]
        public void DeviceClient_ConnectionString_DeviceScope_SharedAccessKeyCredentialType_MissingSharedAccessKey_ExceptionTest()
        {
            string connectionString = "HostName=acme.azure-devices.net;CredentialScope=Device;CredentialType=SharedAccessKey;DeviceId=device1";
            var deviceClient = DeviceClient.CreateFromConnectionString(connectionString);
        }

        [TestMethod]
        [TestCategory("CIT")]
        [ExpectedException(typeof(ArgumentException))]
        public void DeviceClient_ConnectionString_DeviceScope_SharedAccessKeyCredentialType_NotAllowedSharedAccessKeyName_ExceptionTest()
        {
            string connectionString = "HostName=acme.azure-devices.net;CredentialScope=Device;CredentialType=SharedAccessKey;SharedAccessKeyName=AllAccessKey;DeviceId=device1";
            var deviceClient = DeviceClient.CreateFromConnectionString(connectionString);
        }

        [TestMethod]
        [TestCategory("CIT")]
        [ExpectedException(typeof(FormatException))]
        public void DeviceClient_ConnectionString_DeviceScope_SharedAccessSignatureCredentialType_InvalidSharedAccessKey_ExceptionTest()
        {
            string connectionString = "HostName=acme.azure-devices.net;CredentialScope=Device;CredentialType=SharedAccessKey;SharedAccessKey=INVALID;DeviceId=device1";
            var deviceClient = DeviceClient.CreateFromConnectionString(connectionString);
        }

        [TestMethod]
        [TestCategory("CIT")]
        public void DeviceClient_MalformedConnectionStringTest()
        {
            string connectionString = "TODO: IoT Hub connection string to connect to";
            try
            {
                var deviceClient = DeviceClient.CreateFromConnectionString(connectionString);
            }
            catch (FormatException fe)
            {
                Assert.IsTrue(fe.Message.Contains("Malformed Token"), "Exception should mention 'Malformed Token' Actual :" + fe.Message);             
            }
         }

        [TestMethod]
        [TestCategory("CIT")]
        [ExpectedException(typeof(FormatException))]
        public void DeviceClient_ConnectionString_DeviceScope_ImplicitSharedAccessSignatureCredentialType_InvalidSharedAccessSignature_ExceptionTest()
        {
            string connectionString = "HostName=acme.azure-devices.net;CredentialScope=Device;CredentialType=SharedAccessKey;SharedAccessSignature=INVALID;DeviceId=device1";
            var deviceClient = DeviceClient.CreateFromConnectionString(connectionString);
        }

        [TestMethod]
        [TestCategory("CIT")]
        [ExpectedException(typeof(ArgumentNullException))]
        public void DeviceClient_ConnectionString_EmptyConnectionString_ExceptionTest()
        {
            string connectionString = "";
            var deviceClient = DeviceClient.CreateFromConnectionString(connectionString);
        }

        [TestMethod]
        [TestCategory("CIT")]
        [ExpectedException(typeof(ArgumentNullException))]
        public void DeviceClient_ConnectionString_NullConnectionString_ExceptionTest()
        {
            string connectionString = null;
            var deviceClient = DeviceClient.CreateFromConnectionString(connectionString);
        }
    }
}

