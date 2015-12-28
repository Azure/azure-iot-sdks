// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Api.Test.ConnectionString
{
    using System;

    using Microsoft.Azure.Devices;
    using Microsoft.VisualStudio.TestTools.UnitTesting;

    [TestClass]
    public class ServiceClientConnectionStringExceptionTests
    {
        [TestMethod]
        [TestCategory("CIT")]
        [ExpectedException(typeof(ArgumentException))]
        public void ServiceClient_ConnectionString_DefaultScope_DefaultCredentialType_MissingEndpoint_ExceptionTest()
        {
            string connectionString = "SharedAccessKeyName=AllAccessKey;SharedAccessKey=CQN2K33r45/0WeIjpqmErV5EIvX8JZrozt3NEHCEkG8=";
            var serviceClient = ServiceClient.CreateFromConnectionString(connectionString);
        }

        [TestMethod]
        [TestCategory("CIT")]
        [ExpectedException(typeof(ArgumentException))]
        public void ServiceClient_ConnectionString_DefaultScope_DefaultCredentialType_MissingSharedAccessKeyName_ExceptionTest()
        {
            string connectionString = "HostName=acme.azure-devices.net;SharedAccessKey=CQN2K33r45/0WeIjpqmErV5EIvX8JZrozt3NEHCEkG8=";
            var serviceClient = ServiceClient.CreateFromConnectionString(connectionString);
        }

        [TestMethod]
        [TestCategory("CIT")]
        [ExpectedException(typeof(ArgumentException))]
        public void ServiceClient_ConnectionString_DefaultScope_DefaultCredentialType_MissingSharedAccessKey_ExceptionTest()
        {
            string connectionString = "HostName=acme.azure-devices.net;SharedAccessKeyName=AllAccessKey"; 
            var serviceClient = ServiceClient.CreateFromConnectionString(connectionString);
        }

        [TestMethod]
        [TestCategory("CIT")]
        [ExpectedException(typeof(ArgumentException))]
        public void ServiceClient_ConnectionString_DefaultScope_DefaultCredentialType_MissingSharedAccessKeyNameAndKey_ExceptionTest()
        {
            string connectionString = "HostName=acme.azure-devices.net";
            var serviceClient = ServiceClient.CreateFromConnectionString(connectionString);
        }

        [TestMethod]
        [TestCategory("CIT")]
        [ExpectedException(typeof(ArgumentException))]
        public void ServiceClient_ConnectionString_IotHubScope_SharedAccessSignatureCredentialType_MissingSharedAccessKeyName_ExceptionTest()
        {
            string connectionString = "HostName=acme.azure-devices.net;CredentialScope=IotHub;CredentialType=SharedAccessSignature;SharedAccessKey=CQN2K33r45/0WeIjpqmErV5EIvX8JZrozt3NEHCEkG8=";
            var serviceClient = ServiceClient.CreateFromConnectionString(connectionString);
        }

        [TestMethod]
        [TestCategory("CIT")]
        [ExpectedException(typeof(ArgumentException))]
        public void ServiceClient_ConnectionString_IotHubScope_SharedAccessSignatureCredentialType_MissingSharedAccessKey_ExceptionTest()
        {
            string connectionString = "HostName=acme.azure-devices.net;CredentialScope=IotHub;CredentialType=SharedAccessSignature;SharedAccessKeyName=AllAccessKey";
            var serviceClient = ServiceClient.CreateFromConnectionString(connectionString);
        }

        [TestMethod]
        [TestCategory("CIT")]
        [ExpectedException(typeof(ArgumentException))]
        public void ServiceClient_ConnectionString_IotHubScope_SharedAccessKeyCredentialType_MissingSharedAccessKeyNameAndKey_ExceptionTest()
        {
            string connectionString = "HostName=acme.azure-devices.net;CredentialScope=IotHub;CredentialType=SharedAccessKey";
            var serviceClient = ServiceClient.CreateFromConnectionString(connectionString);
        }

        [TestMethod]
        [TestCategory("CIT")]
        [ExpectedException(typeof(ArgumentException))]
        public void ServiceClient_ConnectionString_DeviceScope_SharedAccessKeyCredentialType_MissingSharedAccessKey_ExceptionTest()
        {
            string connectionString = "HostName=acme.azure-devices.net;CredentialScope=IotHub;CredentialType=SharedAccessKey";
            var serviceClient = ServiceClient.CreateFromConnectionString(connectionString);
        }

        [TestMethod]
        [TestCategory("CIT")]
        [ExpectedException(typeof(ArgumentException))]
        public void ServiceClient_ConnectionString_DeviceScope_SharedAccessKeyCredentialType_NotAllowedSharedAccessKeyName_ExceptionTest()
        {
            string connectionString = "HostName=acme.azure-devices.net;CredentialScope=IotHub;CredentialType=SharedAccessKey;SharedAccessKeyName=AllAccessKey";
            var serviceClient = ServiceClient.CreateFromConnectionString(connectionString);
        }

        [TestMethod]
        [TestCategory("CIT")]
        [ExpectedException(typeof(FormatException))]
        public void ServiceClient_ConnectionString_DeviceScope_SharedAccessKeyCredentialType_InvalidSharedAccessKey_ExceptionTest()
        {
            string connectionString = "HostName=acme.azure-devices.net;CredentialScope=IotHub;CredentialType=SharedAccessKey;SharedAccessKeyName=blah;SharedAccessKey=INVALID";
            var serviceClient = ServiceClient.CreateFromConnectionString(connectionString);
        }

        [TestMethod]
        [TestCategory("CIT")]
        [ExpectedException(typeof(FormatException))]
        public void ServiceClient_ConnectionString_DeviceScope_ImplicitSharedAccessKeyCredentialType_InvalidSharedAccessSignature_ExceptionTest()
        {
            string connectionString = "HostName=acme.azure-devices.net;CredentialScope=IotHub;CredentialType=SharedAccessKey;SharedAccessKeyName=blah;SharedAccessSignature=INVALID";
            var serviceClient = ServiceClient.CreateFromConnectionString(connectionString);
        }

        [TestMethod]
        [TestCategory("CIT")]
        [ExpectedException(typeof(ArgumentNullException))]
        public void ServiceClient_ConnectionString_EmptyConnectionString_ExceptionTest()
        {
            string connectionString = "";
            var serviceClient = ServiceClient.CreateFromConnectionString(connectionString);
        }

        [TestMethod]
        [TestCategory("CIT")]
        [ExpectedException(typeof(ArgumentNullException))]
        public void ServiceClient_ConnectionString_NullConnectionString_ExceptionTest()
        {
            string connectionString = null;
            var serviceClient = ServiceClient.CreateFromConnectionString(connectionString);
        }
    }
}

