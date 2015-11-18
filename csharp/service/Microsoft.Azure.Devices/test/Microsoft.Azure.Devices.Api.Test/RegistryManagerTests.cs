// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Api.Test
{
    using System;
    using System.Collections.Generic;
    using System.Net;
    using System.Net.Http;
    using System.Threading;
    using System.Threading.Tasks;
    using Microsoft.Azure.Devices;
    using Microsoft.VisualStudio.TestTools.UnitTesting;
    using Moq;

    [TestClass]
    public class RegistryManagerTests
    {
        const string IotHubName = "acme";

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("API")]
        [ExpectedException(typeof(ArgumentNullException))]
        public void ConstructorWithInvalidConnectionStringTest()
        {
            RegistryManager.CreateFromConnectionString(string.Empty);
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("API")]
        public void ConstructorWithValidConnectionStringTest()
        {
            var authMethod = new ServiceAuthenticationWithSharedAccessPolicyKey("test", "CQN2K33r45/0WeIjpqmErV5EIvX8JZrozt3NEHCEkG8=");
            var builder = IotHubConnectionStringBuilder.Create("acme.azure-devices.net", authMethod);
            var registryManager = RegistryManager.CreateFromConnectionString(builder.ToString());
            Assert.IsNotNull(registryManager);
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("API")]
        public async Task GetDeviceAsyncTest()
        {
            const string DeviceId = "123";
            var deviceToReturn = new Device(DeviceId) { ConnectionState = DeviceConnectionState.Connected };
            var restOpMock = new Mock<IHttpClientHelper>();
            restOpMock.Setup(restOp => restOp.GetAsync<Device>(It.IsAny<Uri>(), It.IsAny<IDictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>>>(), null, false, It.IsAny<CancellationToken>())).ReturnsAsync(deviceToReturn);

            var registryManager = new HttpRegistryManager(restOpMock.Object, IotHubName);
            var device = await registryManager.GetDeviceAsync(DeviceId);
            Assert.AreSame(deviceToReturn, device);
            restOpMock.VerifyAll();
        }

        [ExpectedException(typeof(ArgumentException))]
        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("API")]
        public async Task GetDeviceAsyncWithNullDeviceIdTest()
        {
            var restOpMock = new Mock<IHttpClientHelper>();
            var registryManager = new HttpRegistryManager(restOpMock.Object, IotHubName);
            await registryManager.GetDeviceAsync(null);
            Assert.Fail("Calling GetDeviceAsync with null device id did not throw an exception.");
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("API")]
        public async Task RegisterDeviceAsyncTest()
        {
            var deviceToReturn = new Device("123") { ConnectionState = DeviceConnectionState.Connected };
            var restOpMock = new Mock<IHttpClientHelper>();
            restOpMock.Setup(restOp => restOp.PutAsync(It.IsAny<Uri>(), It.IsAny<Device>(), It.IsAny<PutOperationType>(), It.IsAny<IDictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>>>(), It.IsAny<CancellationToken>())).ReturnsAsync(deviceToReturn);

            var registryManager = new HttpRegistryManager(restOpMock.Object, IotHubName);
            var returnedDevice = await registryManager.AddDeviceAsync(deviceToReturn);
            Assert.AreSame(deviceToReturn, returnedDevice);
            restOpMock.VerifyAll();
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("API")]
        [ExpectedException(typeof(ArgumentException))]
        public async Task RegisterDeviceAsyncWithETagSetTest()
        {
            var deviceToReturn = new Device("123") { ConnectionState = DeviceConnectionState.Connected, ETag = "123" };
            var restOpMock = new Mock<IHttpClientHelper>();
            var registryManager = new HttpRegistryManager(restOpMock.Object, IotHubName);
            await registryManager.AddDeviceAsync(deviceToReturn);
            Assert.Fail("RegisterDevice API did not throw exception when ETag was set.");
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("API")]
        [ExpectedException(typeof(ArgumentNullException))]
        public async Task RegisterDeviceAsyncWithNullDeviceTest()
        {
            var restOpMock = new Mock<IHttpClientHelper>();
            var registryManager = new HttpRegistryManager(restOpMock.Object, IotHubName);
            await registryManager.AddDeviceAsync(null);
            Assert.Fail("RegisterDevice API did not throw exception when the device parameter was null.");
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("API")]
        [ExpectedException(typeof(ArgumentException))]
        public async Task RegisterDeviceAsyncWithDeviceIdNullTest()
        {
            var restOpMock = new Mock<IHttpClientHelper>();
            var registryManager = new HttpRegistryManager(restOpMock.Object, IotHubName);
            await registryManager.AddDeviceAsync(new Device());
            Assert.Fail("RegisterDevice API did not throw exception when the device's id was not set.");
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("API")]
        public async Task UpdateDeviceAsyncTest()
        {
            var deviceToReturn = new Device("123") { ConnectionState = DeviceConnectionState.Connected, ETag = "123" };
            var restOpMock = new Mock<IHttpClientHelper>();
            restOpMock.Setup(restOp => restOp.PutAsync(It.IsAny<Uri>(), It.IsAny<Device>(), It.IsAny<PutOperationType>(), It.IsAny<IDictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>>>(), It.IsAny<CancellationToken>())).ReturnsAsync(deviceToReturn);

            var registryManager = new HttpRegistryManager(restOpMock.Object, IotHubName);
            var returnedDevice = await registryManager.UpdateDeviceAsync(deviceToReturn);
            Assert.AreSame(deviceToReturn, returnedDevice);
            restOpMock.VerifyAll();
        }

        [ExpectedException(typeof(ArgumentNullException))]
        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("API")]
        public async Task UpdateDeviceWithNullDeviceTest()
        {
            var restOpMock = new Mock<IHttpClientHelper>();
            var registryManager = new HttpRegistryManager(restOpMock.Object, IotHubName);
            await registryManager.UpdateDeviceAsync(null);
            Assert.Fail("UpdateDevice api did not throw exception when the device parameter was null.");
        }

        [ExpectedException(typeof(ArgumentException))]
        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("API")]
        public async Task UpdateDeviceWithDeviceIdNullTest()
        {
            var restOpMock = new Mock<IHttpClientHelper>();
            var registryManager = new HttpRegistryManager(restOpMock.Object, IotHubName);
            await registryManager.UpdateDeviceAsync(new Device() { ETag = "*" });
            Assert.Fail("UpdateDevice api did not throw exception when the device's id was null.");
        }

        [ExpectedException(typeof(ArgumentException))]
        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("API")]
        public async Task DeleteDeviceAsyncTest()
        {
            var restOpMock = new Mock<IHttpClientHelper>();
            var mockETag = new ETagHolder() { ETag = "*" };
            restOpMock.Setup(restOp => restOp.DeleteAsync(It.IsAny<Uri>(), mockETag, It.IsAny<IDictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>>>(), null, It.IsAny<CancellationToken>())).Returns(Task.FromResult(0));
            var registryManager = new HttpRegistryManager(restOpMock.Object, IotHubName);
            await registryManager.RemoveDeviceAsync(new Device());
            restOpMock.VerifyAll();
        }

        [ExpectedException(typeof(ArgumentException))]
        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("API")]
        public async Task DeleteDeviceAsyncWithNullIdTest()
        {
            var restOpMock = new Mock<IHttpClientHelper>();
            var registryManager = new HttpRegistryManager(restOpMock.Object, IotHubName);
            await registryManager.RemoveDeviceAsync(string.Empty);
            Assert.Fail("Delete API did not throw exception when the device id was null.");
        }
    }
}
