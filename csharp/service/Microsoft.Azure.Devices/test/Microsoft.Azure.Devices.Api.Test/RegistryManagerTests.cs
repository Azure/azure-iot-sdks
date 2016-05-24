// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Api.Test
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Net;
    using System.Net.Http;
    using System.Threading;
    using System.Threading.Tasks;
    using Microsoft.Azure.Devices;
    using Microsoft.VisualStudio.TestTools.UnitTesting;
    using Moq;
    using Newtonsoft.Json;
    using QueryExpressions;

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
        public async Task QueryDevicesAsyncTestWithNullTags()
        {
            var restOpMock = new Mock<IHttpClientHelper>();
            var registryManager = new HttpRegistryManager(restOpMock.Object, IotHubName);

            await TestAssert.ThrowsAsync<ArgumentException>(() => registryManager.QueryDevicesAsync(null, 1));
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("API")]
        public async Task QueryDevicesAsyncTestWithEmptyTags()
        {
            var restOpMock = new Mock<IHttpClientHelper>();
            var registryManager = new HttpRegistryManager(restOpMock.Object, IotHubName);

            await TestAssert.ThrowsAsync<ArgumentException>(() => registryManager.QueryDevicesAsync(new List<string>(), 1));
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("API")]
        public async Task QueryDevicesAsyncTest()
        {
            List<Device> devicesToReturn = new List<Device>();
            devicesToReturn.Add(new Device("a") { ConnectionState = DeviceConnectionState.Connected });

            var restOpMock = new Mock<IHttpClientHelper>();
            restOpMock.Setup(restOp => restOp.PostAsync<object, IEnumerable<Device>>(It.IsAny<Uri>(), null, It.IsAny<IDictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>>>(), null, It.IsAny<CancellationToken>())).ReturnsAsync(devicesToReturn);

            var registryManager = new HttpRegistryManager(restOpMock.Object, IotHubName);

            var returnedDevices = await registryManager.QueryDevicesAsync(new List<string>() { "a", "b" }, 1);

            Assert.AreSame(devicesToReturn, returnedDevices);
            Assert.AreSame(devicesToReturn[0], returnedDevices.First());
            restOpMock.VerifyAll();
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("API")]
        public async Task QueryDeviceExpressionAsyncTest()
        {
            List<Device> devicesToReturn = new List<Device>();
            devicesToReturn.Add(new Device("a") { ConnectionState = DeviceConnectionState.Connected });

            var queryResultToReturn = new DeviceQueryResult
            {
                Result = devicesToReturn
            };

            var restOpMock = new Mock<IHttpClientHelper>();
            restOpMock.Setup(restOp => restOp.PostAsync<QueryExpression, DeviceQueryResult>(It.IsAny<Uri>(), It.IsAny<QueryExpression>(), It.IsAny<IDictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>>>(), null, It.IsAny<CancellationToken>())).ReturnsAsync(queryResultToReturn);

            var registryManager = new HttpRegistryManager(restOpMock.Object, IotHubName);

            // Valid JSON
            var queryJson = JsonConvert.SerializeObject(this.GetQueryExpression());

            var returnedDevices = (await registryManager.QueryDevicesJsonAsync(queryJson)).Result;

            Assert.AreSame(devicesToReturn, returnedDevices);
            Assert.AreSame(devicesToReturn[0], returnedDevices.First());
            restOpMock.VerifyAll();

            // Invalid JSON
            queryJson = "{ filter: { { } }";
            await TestAssert.ThrowsAsync<Exception>(() => registryManager.QueryDevicesJsonAsync(queryJson));
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("API")]
        public async Task GetDevicesAsyncTest()
        {
            List<Device> devicesToReturn = new List<Device>();
            devicesToReturn.Add(new Device("a") { ConnectionState = DeviceConnectionState.Connected });

            var restOpMock = new Mock<IHttpClientHelper>();
            restOpMock.Setup(restOp => restOp.GetAsync<IEnumerable<Device>>(It.IsAny<Uri>(), It.IsAny<IDictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>>>(), null, It.IsAny<CancellationToken>())).ReturnsAsync(devicesToReturn);

            var registryManager = new HttpRegistryManager(restOpMock.Object, IotHubName);

            var returnedDevices = await registryManager.GetDevicesAsync(1);

            Assert.AreSame(devicesToReturn, returnedDevices);
            Assert.AreSame(devicesToReturn[0], returnedDevices.First());
            restOpMock.VerifyAll();
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
        public async Task RegisterDeviceAsyncWithInvalidDeviceIdTest()
        {
            var deviceToReturn = new Device("/baddevice") { ConnectionState = DeviceConnectionState.Connected, ETag = "123" };
            var restOpMock = new Mock<IHttpClientHelper>();
            var registryManager = new HttpRegistryManager(restOpMock.Object, IotHubName);
            await registryManager.AddDeviceAsync(deviceToReturn);
            Assert.Fail("RegisterDevice API did not throw exception when bad deviceid was used.");
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
        [ExpectedException(typeof(ArgumentException))]
        public async Task RegisterDevicesAsyncWithInvalidDeviceIdTest()
        {
            var goodDevice = new Device("123") { ConnectionState = DeviceConnectionState.Connected };
            // '/' is not a valid character in DeviceId
            var badDevice = new Device("/baddevice") { ConnectionState = DeviceConnectionState.Connected };
            var restOpMock = new Mock<IHttpClientHelper>();
            var registryManager = new HttpRegistryManager(restOpMock.Object, IotHubName);
            await registryManager.AddDevicesAsync(new List<Device>() {goodDevice, badDevice});
            Assert.Fail("RegisterDevices API did not throw exception when bad deviceid was used.");
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("API")]
        [ExpectedException(typeof(ArgumentException))]
        public async Task RegisterDevices2AsyncWithInvalidDeviceIdTest()
        {
            var goodDevice = new Device("123") { ConnectionState = DeviceConnectionState.Connected };
            // '/' is not a valid character in DeviceId
            var badDevice = new Device("/baddevice") { ConnectionState = DeviceConnectionState.Connected };
            var restOpMock = new Mock<IHttpClientHelper>();
            var registryManager = new HttpRegistryManager(restOpMock.Object, IotHubName);
            await registryManager.AddDevices2Async(new List<Device>() { goodDevice, badDevice });
            Assert.Fail("RegisterDevices API did not throw exception when bad deviceid was used.");
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("API")]
        [ExpectedException(typeof(ArgumentException))]
        public async Task RegisterDevicesAsyncWithETagsSetTest()
        {
            var goodDevice = new Device("123") { ConnectionState = DeviceConnectionState.Connected };
            var badDevice = new Device("234") { ConnectionState = DeviceConnectionState.Connected, ETag = "234" };
            var restOpMock = new Mock<IHttpClientHelper>();
            var registryManager = new HttpRegistryManager(restOpMock.Object, IotHubName);
            await registryManager.AddDevicesAsync(new List<Device>() { goodDevice, badDevice });
            Assert.Fail("RegisterDevices API did not throw exception when ETag was used.");
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("API")]
        [ExpectedException(typeof(ArgumentException))]
        public async Task RegisterDevices2AsyncWithETagsSetTest()
        {
            var goodDevice = new Device("123") { ConnectionState = DeviceConnectionState.Connected };
            var badDevice = new Device("234") { ConnectionState = DeviceConnectionState.Connected, ETag = "234" };
            var restOpMock = new Mock<IHttpClientHelper>();
            var registryManager = new HttpRegistryManager(restOpMock.Object, IotHubName);
            await registryManager.AddDevices2Async(new List<Device>() { goodDevice, badDevice });
            Assert.Fail("RegisterDevices API did not throw exception when ETag was used.");
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("API")]
        [ExpectedException(typeof(ArgumentNullException))]
        public async Task RegisterDevicesAsyncWithNullDeviceTest()
        {
            var goodDevice = new Device("123") { ConnectionState = DeviceConnectionState.Connected };
            Device badDevice = null;
            var restOpMock = new Mock<IHttpClientHelper>();
            var registryManager = new HttpRegistryManager(restOpMock.Object, IotHubName);
            await registryManager.AddDevicesAsync(new List<Device>() { goodDevice, badDevice });
            Assert.Fail("RegisterDevices API did not throw exception when Null device was used.");
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("API")]
        [ExpectedException(typeof(ArgumentNullException))]
        public async Task RegisterDevices2AsyncWithNullDeviceTest()
        {
            var goodDevice = new Device("123") { ConnectionState = DeviceConnectionState.Connected };
            Device badDevice = null;
            var restOpMock = new Mock<IHttpClientHelper>();
            var registryManager = new HttpRegistryManager(restOpMock.Object, IotHubName);
            await registryManager.AddDevices2Async(new List<Device>() { goodDevice, badDevice });
            Assert.Fail("RegisterDevices API did not throw exception when Null device was used.");
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("API")]
        [ExpectedException(typeof(ArgumentException))]
        public async Task RegisterDevicesAsyncWithNullDeviceListTest()
        {
            var restOpMock = new Mock<IHttpClientHelper>();
            var registryManager = new HttpRegistryManager(restOpMock.Object, IotHubName);
            await registryManager.AddDevicesAsync(new List<Device>());
            Assert.Fail("RegisterDevices API did not throw exception when Null device list was used.");
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("API")]
        [ExpectedException(typeof(ArgumentException))]
        public async Task RegisterDevices2AsyncWithNullDeviceListTest()
        {
            var restOpMock = new Mock<IHttpClientHelper>();
            var registryManager = new HttpRegistryManager(restOpMock.Object, IotHubName);
            await registryManager.AddDevices2Async(new List<Device>());
            Assert.Fail("RegisterDevices API did not throw exception when Null device list was used.");
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("API")]
        [ExpectedException(typeof(ArgumentException))]
        public async Task RegisterDevicesAsyncWithDeviceIdNullTest()
        {
            var goodDevice = new Device("123") { ConnectionState = DeviceConnectionState.Connected };
            var badDevice = new Device();
            var restOpMock = new Mock<IHttpClientHelper>();
            var registryManager = new HttpRegistryManager(restOpMock.Object, IotHubName);
            await registryManager.AddDevicesAsync(new List<Device>() { goodDevice, badDevice });
            Assert.Fail("RegisterDevices API did not throw exception when deviceId was null.");
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("API")]
        [ExpectedException(typeof(ArgumentException))]
        public async Task RegisterDevices2AsyncWithDeviceIdNullTest()
        {
            var goodDevice = new Device("123") { ConnectionState = DeviceConnectionState.Connected };
            var badDevice = new Device();
            var restOpMock = new Mock<IHttpClientHelper>();
            var registryManager = new HttpRegistryManager(restOpMock.Object, IotHubName);
            await registryManager.AddDevices2Async(new List<Device>() { goodDevice, badDevice });
            Assert.Fail("RegisterDevices API did not throw exception when deviceId was null.");
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

        private Device PrepareTestDevice(int batteryLevel, string firmwareVersion)
        {
            Device deviceToReturn = new Device("Device123");

            deviceToReturn.DeviceProperties = new Dictionary<string, DevicePropertyValue>() {
                { DevicePropertyNames.BatteryLevel,
                    new DevicePropertyValue() { LastUpdatedTime= DateTime.Today, Value = batteryLevel } },
                { DevicePropertyNames.FirmwareVersion,
                    new DevicePropertyValue() { LastUpdatedTime= DateTime.Today, Value = firmwareVersion }
                }
            };


            return deviceToReturn;
        }
        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("API")]
        public async Task UpdateManagedDevicePropertiesAsyncTest()
        {
            // Setup for Test

            Device deviceToReturn = PrepareTestDevice(batteryLevel: 10, firmwareVersion: "1.0");
            deviceToReturn.ServiceProperties = new ServiceProperties() { Tags = new List<string>() { "Building109" } };

            
            JobResponse jobCurrentTimeToReturn = new JobResponse() { Status = JobStatus.Running };

            var restOpMock = new Mock<IHttpClientHelper>();
            restOpMock.Setup(restOp => restOp.PutAsync(It.IsAny<Uri>(), It.IsAny<Device>(), It.IsAny<PutOperationType>(), It.IsAny<IDictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>>>(), It.IsAny<CancellationToken>())).ReturnsAsync(deviceToReturn);
            restOpMock.Setup(restOp => restOp.PutAsync<JobRequest,JobResponse>(It.IsAny<Uri>(), It.IsAny<JobRequest>(), It.IsAny<IDictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>>>(), It.IsAny<CancellationToken>())).ReturnsAsync(jobCurrentTimeToReturn);

            // Create a new Device with the Tags
            Device myDevice = new Device("Device123");

            var registryManager = new HttpRegistryManager(restOpMock.Object, IotHubName);
            Device createdDevice = await registryManager.AddDeviceAsync(myDevice);

            // Get the Device and check for the Managed Device Properties

            string manufacturer = null;
            try
            {
                manufacturer = createdDevice.DeviceProperties[DevicePropertyNames.Manufacturer].ToString();
            }
            catch (KeyNotFoundException)
            { }

            Assert.AreEqual(manufacturer, null);

            // Update the Device Property for Manufacturer . Schedules the Job and Returns the Job Id to poll
            var jobClient = new HttpJobClient(restOpMock.Object, IotHubName);
            JobResponse jobCurrentTime = await jobClient.ScheduleDevicePropertyWriteAsync(Guid.NewGuid().ToString(), myDevice.Id, DevicePropertyNames.CurrentTime, DateTime.UtcNow);

            Assert.AreEqual(jobCurrentTime.Status, jobCurrentTimeToReturn.Status);

            // Poll for the Job Status ...
            jobCurrentTime = await jobClient.GetJobAsync(jobCurrentTime.JobId);
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("API")]
        public async Task UpdateManagedDeviceConfigurationAsyncTest()
        {
            // Setup for Test
            Device deviceToReturn = PrepareTestDevice(batteryLevel: 10, firmwareVersion: "1.0");
            deviceToReturn.ServiceProperties = new ServiceProperties() { Tags = new List<string>() { "Building109" } };

            JobResponse configurationJobPropertyToReturn = new JobResponse() { Status = JobStatus.Running };

            var restOpMock = new Mock<IHttpClientHelper>();
            restOpMock.Setup(restOp => restOp.PutAsync(It.IsAny<Uri>(), It.IsAny<Device>(), It.IsAny<PutOperationType>(), It.IsAny<IDictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>>>(), It.IsAny<CancellationToken>())).ReturnsAsync(deviceToReturn);
            restOpMock.Setup(restOp => restOp.PutAsync<JobRequest, JobResponse>(It.IsAny<Uri>(), It.IsAny<JobRequest>(), It.IsAny<IDictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>>>(), It.IsAny<CancellationToken>())).ReturnsAsync(configurationJobPropertyToReturn);

            // Create a new Device with the Tags
            Device myDevice = new Device("Device123");

            var registryManager = new HttpRegistryManager(restOpMock.Object, IotHubName);
            Device createdDevice = await registryManager.AddDeviceAsync(myDevice);


            // Update the Device Property for Manufacturer . Schedules the Job and Returns the Job Id to poll
            var jobClient = new HttpJobClient(restOpMock.Object, IotHubName);
            JobResponse configJobProperties = await jobClient.ScheduleUpdateDeviceConfigurationAsync(Guid.NewGuid().ToString(), myDevice.Id, "New Configuration Value");

            Assert.AreEqual(configJobProperties.Status, configurationJobPropertyToReturn.Status);

            // Poll for the Job Status ...
            configJobProperties = await jobClient.GetJobAsync(configJobProperties.JobId);
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("API")]
        public async Task UpdateManagedDeviceTagsAsyncTest()
        {
            // Setup for Test
            Device createdDeviceToReturn = PrepareTestDevice(batteryLevel: 10, firmwareVersion: "1.0");
            createdDeviceToReturn.ServiceProperties = new ServiceProperties() { Tags = new List<string>() { "Building109" } };

            Device updatedDeviceToReturn = new Device("Device123");
            updatedDeviceToReturn.ServiceProperties = new ServiceProperties() { Tags = new List<string>() { "My New Tag" } };

            var restOpMock = new Mock<IHttpClientHelper>();
            restOpMock.Setup(restOp => restOp.PutAsync(It.IsAny<Uri>(), It.IsAny<Device>(), It.IsAny<PutOperationType>(), It.IsAny<IDictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>>>(), It.IsAny<CancellationToken>())).ReturnsAsync(createdDeviceToReturn);
            restOpMock.Setup(restOp => restOp.PostAsync<ServiceProperties, Device>(It.IsAny<Uri>(), It.IsAny<ServiceProperties>(), It.IsAny<IDictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>>>(), null, It.IsAny<CancellationToken>())).ReturnsAsync(updatedDeviceToReturn);

            // Create a new Device with the Tags
            Device myDevice = new Device("Device123");

            var registryManager = new HttpRegistryManager(restOpMock.Object, IotHubName);
            Device createdDevice = await registryManager.AddDeviceAsync(myDevice);

            // Updates the Tags
            createdDevice.ServiceProperties.Tags.Add("My New Tag");

            restOpMock.Setup(restOp => restOp.PutAsync(It.IsAny<Uri>(), It.IsAny<ServiceProperties>(), It.IsAny<PutOperationType>(), It.IsAny<IDictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>>>(), It.IsAny<CancellationToken>())).ReturnsAsync(createdDevice.ServiceProperties);

            ServiceProperties serviceProperties = await registryManager.SetServicePropertiesAsync(createdDevice.Id, createdDevice.ServiceProperties);

            Assert.AreEqual(true, serviceProperties.Tags.Contains("My New Tag"));
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


        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("API")]
        [ExpectedException(typeof(ArgumentException))]
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
        public async Task UpdateDeviceWithInvalidDeviceIdTest()
        {
            var restOpMock = new Mock<IHttpClientHelper>();
            var registryManager = new HttpRegistryManager(restOpMock.Object, IotHubName);
            // '/' is not a valid char in DeviceId
            await registryManager.UpdateDeviceAsync(new Device("/baddevice") { ETag = "*" });
            Assert.Fail("UpdateDevice api did not throw exception when the deviceid was invalid.");
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("API")]
        [ExpectedException(typeof(ArgumentException))]
        public async Task UpdateDevicesAsyncWithInvalidDeviceIdTest()
        {
            var goodDevice = new Device("123") { ConnectionState = DeviceConnectionState.Connected };
            var badDevice = new Device("/baddevice") { ConnectionState = DeviceConnectionState.Connected };
            var restOpMock = new Mock<IHttpClientHelper>();
            var registryManager = new HttpRegistryManager(restOpMock.Object, IotHubName);
            await registryManager.UpdateDevicesAsync(new List<Device>() { goodDevice, badDevice });
            Assert.Fail("UpdateDevices API did not throw exception when bad deviceid was used.");
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("API")]
        [ExpectedException(typeof(ArgumentException))]
        public async Task UpdateDevices2AsyncWithInvalidDeviceIdTest()
        {
            var goodDevice = new Device("123") { ConnectionState = DeviceConnectionState.Connected };
            var badDevice = new Device("/baddevice") { ConnectionState = DeviceConnectionState.Connected };
            var restOpMock = new Mock<IHttpClientHelper>();
            var registryManager = new HttpRegistryManager(restOpMock.Object, IotHubName);
            await registryManager.UpdateDevices2Async(new List<Device>() { goodDevice, badDevice });
            Assert.Fail("UpdateDevices API did not throw exception when bad deviceid was used.");
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("API")]
        [ExpectedException(typeof(ArgumentException))]
        public async Task UpdateDevicesAsyncWithETagMissingTest()
        {
            var goodDevice = new Device("123") { ConnectionState = DeviceConnectionState.Connected, ETag = "234" };
            var badDevice = new Device("234") { ConnectionState = DeviceConnectionState.Connected };
            var restOpMock = new Mock<IHttpClientHelper>();
            var registryManager = new HttpRegistryManager(restOpMock.Object, IotHubName);
            await registryManager.UpdateDevicesAsync(new List<Device>() { goodDevice, badDevice });
            Assert.Fail("UpdateDevices API did not throw exception when ETag was null.");
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("API")]
        [ExpectedException(typeof(ArgumentException))]
        public async Task UpdateDevices2AsyncWithETagMissingTest()
        {
            var goodDevice = new Device("123") { ConnectionState = DeviceConnectionState.Connected, ETag = "234" };
            var badDevice = new Device("234") { ConnectionState = DeviceConnectionState.Connected };
            var restOpMock = new Mock<IHttpClientHelper>();
            var registryManager = new HttpRegistryManager(restOpMock.Object, IotHubName);
            await registryManager.UpdateDevices2Async(new List<Device>() { goodDevice, badDevice });
            Assert.Fail("UpdateDevices API did not throw exception when ETag was null.");
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("API")]
        [ExpectedException(typeof(ArgumentNullException))]
        public async Task UpdateDevicesAsyncWithNullDeviceTest()
        {
            var goodDevice = new Device("123") { ConnectionState = DeviceConnectionState.Connected, ETag = "234" };
            Device badDevice = null;
            var restOpMock = new Mock<IHttpClientHelper>();
            var registryManager = new HttpRegistryManager(restOpMock.Object, IotHubName);
            await registryManager.UpdateDevicesAsync(new List<Device>() { goodDevice, badDevice });
            Assert.Fail("UpdateDevices API did not throw exception when Null device was used.");
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("API")]
        [ExpectedException(typeof(ArgumentNullException))]
        public async Task UpdateDevices2AsyncWithNullDeviceTest()
        {
            var goodDevice = new Device("123") { ConnectionState = DeviceConnectionState.Connected, ETag = "234" };
            Device badDevice = null;
            var restOpMock = new Mock<IHttpClientHelper>();
            var registryManager = new HttpRegistryManager(restOpMock.Object, IotHubName);
            await registryManager.UpdateDevices2Async(new List<Device>() { goodDevice, badDevice });
            Assert.Fail("UpdateDevices API did not throw exception when Null device was used.");
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("API")]
        [ExpectedException(typeof(ArgumentException))]
        public async Task UpdateDevicesAsyncWithNullDeviceListTest()
        {
            var restOpMock = new Mock<IHttpClientHelper>();
            var registryManager = new HttpRegistryManager(restOpMock.Object, IotHubName);
            await registryManager.UpdateDevicesAsync(new List<Device>());
            Assert.Fail("UpdateDevices API did not throw exception when Null device list was used.");
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("API")]
        [ExpectedException(typeof(ArgumentException))]
        public async Task UpdateDevices2AsyncWithNullDeviceListTest()
        {
            var restOpMock = new Mock<IHttpClientHelper>();
            var registryManager = new HttpRegistryManager(restOpMock.Object, IotHubName);
            await registryManager.UpdateDevices2Async(new List<Device>());
            Assert.Fail("UpdateDevices API did not throw exception when Null device list was used.");
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("API")]
        [ExpectedException(typeof(ArgumentException))]
        public async Task UpdateDevicesAsyncWithDeviceIdNullTest()
        {
            var goodDevice = new Device("123") { ConnectionState = DeviceConnectionState.Connected, ETag = "234" };
            var badDevice = new Device();
            var restOpMock = new Mock<IHttpClientHelper>();
            var registryManager = new HttpRegistryManager(restOpMock.Object, IotHubName);
            await registryManager.UpdateDevicesAsync(new List<Device>() { goodDevice, badDevice });
            Assert.Fail("UpdateDevices API did not throw exception when deviceId was null.");
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("API")]
        [ExpectedException(typeof(ArgumentException))]
        public async Task UpdateDevices2AsyncWithDeviceIdNullTest()
        {
            var goodDevice = new Device("123") { ConnectionState = DeviceConnectionState.Connected, ETag = "234" };
            var badDevice = new Device();
            var restOpMock = new Mock<IHttpClientHelper>();
            var registryManager = new HttpRegistryManager(restOpMock.Object, IotHubName);
            await registryManager.UpdateDevices2Async(new List<Device>() { goodDevice, badDevice });
            Assert.Fail("UpdateDevices API did not throw exception when deviceId was null.");
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("API")]
        public async Task UpdateDevicesAsyncForceUpdateTest()
        {
            var goodDevice1 = new Device("123") { ConnectionState = DeviceConnectionState.Connected };
            var goodDevice2 = new Device("234") { ConnectionState = DeviceConnectionState.Connected };
            var restOpMock = new Mock<IHttpClientHelper>();
            restOpMock.Setup(restOp => restOp.PostAsync<IEnumerable<ExportImportDevice>, Task<string[]>>(It.IsAny<Uri>(), It.IsAny<IEnumerable<ExportImportDevice>>(), It.IsAny<IDictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>>>(), It.IsAny<IDictionary<string, string>>(), It.IsAny<CancellationToken>())).ReturnsAsync(null);

            var registryManager = new HttpRegistryManager(restOpMock.Object, IotHubName);
            await registryManager.UpdateDevicesAsync(new List<Device>() { goodDevice1, goodDevice2 }, true, CancellationToken.None);
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("API")]
        public async Task UpdateDevices2AsyncForceUpdateTest()
        {
            var goodDevice1 = new Device("123") { ConnectionState = DeviceConnectionState.Connected };
            var goodDevice2 = new Device("234") { ConnectionState = DeviceConnectionState.Connected };
            var restOpMock = new Mock<IHttpClientHelper>();
            restOpMock.Setup(restOp => restOp.PostAsync<IEnumerable<ExportImportDevice>, Task<BulkRegistryOperationResult>>(It.IsAny<Uri>(), It.IsAny<IEnumerable<ExportImportDevice>>(), It.IsAny<IDictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>>>(), It.IsAny<IDictionary<string, string>>(), It.IsAny<CancellationToken>())).ReturnsAsync(null);

            var registryManager = new HttpRegistryManager(restOpMock.Object, IotHubName);
            await registryManager.UpdateDevices2Async(new List<Device>() { goodDevice1, goodDevice2 }, true, CancellationToken.None);
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("API")]
        [ExpectedException(typeof(ArgumentException))]
        public async Task UpdateDevicesAsyncForceUpdateMissingETagTest()
        {
            var badDevice1 = new Device("123") { ConnectionState = DeviceConnectionState.Connected };
            var badDevice2 = new Device("234") { ConnectionState = DeviceConnectionState.Connected };
            var restOpMock = new Mock<IHttpClientHelper>();
            restOpMock.Setup(restOp => restOp.PostAsync<IEnumerable<ExportImportDevice>, Task<string[]>>(It.IsAny<Uri>(), It.IsAny<IEnumerable<ExportImportDevice>>(), It.IsAny<IDictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>>>(), It.IsAny<IDictionary<string, string>>(), It.IsAny<CancellationToken>())).ReturnsAsync(null);

            var registryManager = new HttpRegistryManager(restOpMock.Object, IotHubName);
            await registryManager.UpdateDevicesAsync(new List<Device>() { badDevice1, badDevice2 }, false, CancellationToken.None);
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("API")]
        [ExpectedException(typeof(ArgumentException))]
        public async Task UpdateDevices2AsyncForceUpdateMissingETagTest()
        {
            var badDevice1 = new Device("123") { ConnectionState = DeviceConnectionState.Connected };
            var badDevice2 = new Device("234") { ConnectionState = DeviceConnectionState.Connected };
            var restOpMock = new Mock<IHttpClientHelper>();
            restOpMock.Setup(restOp => restOp.PostAsync<IEnumerable<ExportImportDevice>, Task<BulkRegistryOperationResult>>(It.IsAny<Uri>(), It.IsAny<IEnumerable<ExportImportDevice>>(), It.IsAny<IDictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>>>(), It.IsAny<IDictionary<string, string>>(), It.IsAny<CancellationToken>())).ReturnsAsync(null);

            var registryManager = new HttpRegistryManager(restOpMock.Object, IotHubName);
            await registryManager.UpdateDevices2Async(new List<Device>() { badDevice1, badDevice2 }, false, CancellationToken.None);
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("API")]
        public async Task UpdateDevicesAsyncForceUpdateFalseTest()
        {
            var goodDevice1 = new Device("123") { ConnectionState = DeviceConnectionState.Connected, ETag = "234" };
            var goodDevice2 = new Device("234") { ConnectionState = DeviceConnectionState.Connected, ETag = "123" };
            var restOpMock = new Mock<IHttpClientHelper>();
            restOpMock.Setup(restOp => restOp.PostAsync<IEnumerable<ExportImportDevice>, Task<string[]>>(It.IsAny<Uri>(), It.IsAny<IEnumerable<ExportImportDevice>>(), It.IsAny<IDictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>>>(), It.IsAny<IDictionary<string, string>>(), It.IsAny<CancellationToken>())).ReturnsAsync(null);

            var registryManager = new HttpRegistryManager(restOpMock.Object, IotHubName);
            await registryManager.UpdateDevicesAsync(new List<Device>() { goodDevice1, goodDevice2 }, false, CancellationToken.None);
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("API")]
        public async Task UpdateDevices2AsyncForceUpdateFalseTest()
        {
            var goodDevice1 = new Device("123") { ConnectionState = DeviceConnectionState.Connected, ETag = "234" };
            var goodDevice2 = new Device("234") { ConnectionState = DeviceConnectionState.Connected, ETag = "123" };
            var restOpMock = new Mock<IHttpClientHelper>();
            restOpMock.Setup(restOp => restOp.PostAsync<IEnumerable<ExportImportDevice>, Task<BulkRegistryOperationResult>>(It.IsAny<Uri>(), It.IsAny<IEnumerable<ExportImportDevice>>(), It.IsAny<IDictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>>>(), It.IsAny<IDictionary<string, string>>(), It.IsAny<CancellationToken>())).ReturnsAsync(null);

            var registryManager = new HttpRegistryManager(restOpMock.Object, IotHubName);
            await registryManager.UpdateDevices2Async(new List<Device>() { goodDevice1, goodDevice2 }, false, CancellationToken.None);
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("API")]
        [ExpectedException(typeof(ArgumentException))]
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

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("API")]
        [ExpectedException(typeof(ArgumentException))]
        public async Task DeleteDevicesAsyncWithInvalidDeviceIdTest()
        {
            var goodDevice = new Device("123") { ConnectionState = DeviceConnectionState.Connected };
            var badDevice = new Device("/baddevice") { ConnectionState = DeviceConnectionState.Connected };
            var restOpMock = new Mock<IHttpClientHelper>();
            var registryManager = new HttpRegistryManager(restOpMock.Object, IotHubName);
            await registryManager.RemoveDevicesAsync(new List<Device>() { goodDevice, badDevice });
            Assert.Fail("DeleteDevices API did not throw exception when bad deviceid was used.");
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("API")]
        [ExpectedException(typeof(ArgumentException))]
        public async Task DeleteDevices2AsyncWithInvalidDeviceIdTest()
        {
            var goodDevice = new Device("123") { ConnectionState = DeviceConnectionState.Connected };
            var badDevice = new Device("/baddevice") { ConnectionState = DeviceConnectionState.Connected };
            var restOpMock = new Mock<IHttpClientHelper>();
            var registryManager = new HttpRegistryManager(restOpMock.Object, IotHubName);
            await registryManager.RemoveDevices2Async(new List<Device>() { goodDevice, badDevice });
            Assert.Fail("DeleteDevices API did not throw exception when bad deviceid was used.");
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("API")]
        [ExpectedException(typeof(ArgumentException))]
        public async Task DeleteDevicesAsyncWithETagMissingTest()
        {
            var goodDevice = new Device("123") { ConnectionState = DeviceConnectionState.Connected, ETag = "234" };
            var badDevice = new Device("234") { ConnectionState = DeviceConnectionState.Connected };
            var restOpMock = new Mock<IHttpClientHelper>();
            var registryManager = new HttpRegistryManager(restOpMock.Object, IotHubName);
            await registryManager.RemoveDevicesAsync(new List<Device>() { goodDevice, badDevice });
            Assert.Fail("DeleteDevices API did not throw exception when ETag was null.");
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("API")]
        [ExpectedException(typeof(ArgumentException))]
        public async Task DeleteDevices2AsyncWithETagMissingTest()
        {
            var goodDevice = new Device("123") { ConnectionState = DeviceConnectionState.Connected, ETag = "234" };
            var badDevice = new Device("234") { ConnectionState = DeviceConnectionState.Connected };
            var restOpMock = new Mock<IHttpClientHelper>();
            var registryManager = new HttpRegistryManager(restOpMock.Object, IotHubName);
            await registryManager.RemoveDevices2Async(new List<Device>() { goodDevice, badDevice });
            Assert.Fail("DeleteDevices API did not throw exception when ETag was null.");
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("API")]
        [ExpectedException(typeof(ArgumentNullException))]
        public async Task DeleteDevicesAsyncWithNullDeviceTest()
        {
            var goodDevice = new Device("123") { ConnectionState = DeviceConnectionState.Connected, ETag = "234" };
            Device badDevice = null;
            var restOpMock = new Mock<IHttpClientHelper>();
            var registryManager = new HttpRegistryManager(restOpMock.Object, IotHubName);
            await registryManager.RemoveDevicesAsync(new List<Device>() { goodDevice, badDevice });
            Assert.Fail("DeleteDevices API did not throw exception when Null device was used.");
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("API")]
        [ExpectedException(typeof(ArgumentNullException))]
        public async Task DeleteDevices2AsyncWithNullDeviceTest()
        {
            var goodDevice = new Device("123") { ConnectionState = DeviceConnectionState.Connected, ETag = "234" };
            Device badDevice = null;
            var restOpMock = new Mock<IHttpClientHelper>();
            var registryManager = new HttpRegistryManager(restOpMock.Object, IotHubName);
            await registryManager.RemoveDevices2Async(new List<Device>() { goodDevice, badDevice });
            Assert.Fail("DeleteDevices API did not throw exception when Null device was used.");
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("API")]
        [ExpectedException(typeof(ArgumentException))]
        public async Task DeleteDevicesAsyncWithNullDeviceListTest()
        {
            var restOpMock = new Mock<IHttpClientHelper>();
            var registryManager = new HttpRegistryManager(restOpMock.Object, IotHubName);
            await registryManager.RemoveDevicesAsync(new List<Device>());
            Assert.Fail("DeleteDevices API did not throw exception when Null device list was used.");
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("API")]
        [ExpectedException(typeof(ArgumentException))]
        public async Task DeleteDevices2AsyncWithNullDeviceListTest()
        {
            var restOpMock = new Mock<IHttpClientHelper>();
            var registryManager = new HttpRegistryManager(restOpMock.Object, IotHubName);
            await registryManager.RemoveDevices2Async(new List<Device>());
            Assert.Fail("DeleteDevices API did not throw exception when Null device list was used.");
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("API")]
        [ExpectedException(typeof(ArgumentException))]
        public async Task DeleteDevicesAsyncWithDeviceIdNullTest()
        {
            var goodDevice = new Device("123") { ConnectionState = DeviceConnectionState.Connected, ETag = "234" };
            var badDevice = new Device();
            var restOpMock = new Mock<IHttpClientHelper>();
            var registryManager = new HttpRegistryManager(restOpMock.Object, IotHubName);
            await registryManager.RemoveDevicesAsync(new List<Device>() { goodDevice, badDevice });
            Assert.Fail("DeleteDevices API did not throw exception when deviceId was null.");
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("API")]
        [ExpectedException(typeof(ArgumentException))]
        public async Task DeleteDevices2AsyncWithDeviceIdNullTest()
        {
            var goodDevice = new Device("123") { ConnectionState = DeviceConnectionState.Connected, ETag = "234" };
            var badDevice = new Device();
            var restOpMock = new Mock<IHttpClientHelper>();
            var registryManager = new HttpRegistryManager(restOpMock.Object, IotHubName);
            await registryManager.RemoveDevices2Async(new List<Device>() { goodDevice, badDevice });
            Assert.Fail("DeleteDevices API did not throw exception when deviceId was null.");
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("API")]
        public async Task DeleteDevicesAsyncForceDeleteTest()
        {
            var goodDevice1 = new Device("123") { ConnectionState = DeviceConnectionState.Connected };
            var goodDevice2 = new Device("234") { ConnectionState = DeviceConnectionState.Connected };
            var restOpMock = new Mock<IHttpClientHelper>();
            restOpMock.Setup(restOp => restOp.PostAsync<IEnumerable<ExportImportDevice>,Task <string[]>>(It.IsAny<Uri>(), It.IsAny<IEnumerable<ExportImportDevice>>(), It.IsAny<IDictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>>>(), It.IsAny<IDictionary<string, string>>(), It.IsAny<CancellationToken>())).ReturnsAsync(null);

            var registryManager = new HttpRegistryManager(restOpMock.Object, IotHubName);
            await registryManager.RemoveDevicesAsync(new List<Device>() { goodDevice1, goodDevice2 }, true, CancellationToken.None);
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("API")]
        public async Task DeleteDevices2AsyncForceDeleteTest()
        {
            var goodDevice1 = new Device("123") { ConnectionState = DeviceConnectionState.Connected };
            var goodDevice2 = new Device("234") { ConnectionState = DeviceConnectionState.Connected };
            var restOpMock = new Mock<IHttpClientHelper>();
            restOpMock.Setup(restOp => restOp.PostAsync<IEnumerable<ExportImportDevice>, Task<BulkRegistryOperationResult>>(It.IsAny<Uri>(), It.IsAny<IEnumerable<ExportImportDevice>>(), It.IsAny<IDictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>>>(), It.IsAny<IDictionary<string, string>>(), It.IsAny<CancellationToken>())).ReturnsAsync(null);

            var registryManager = new HttpRegistryManager(restOpMock.Object, IotHubName);
            await registryManager.RemoveDevices2Async(new List<Device>() { goodDevice1, goodDevice2 }, true, CancellationToken.None);
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("API")]
        [ExpectedException(typeof(ArgumentException))]
        public async Task DeleteDevicesAsyncForceDeleteFalseMissingETagTest()
        {
            var badDevice1 = new Device("123") { ConnectionState = DeviceConnectionState.Connected};
            var badDevice2 = new Device("234") { ConnectionState = DeviceConnectionState.Connected};
            var restOpMock = new Mock<IHttpClientHelper>();
            restOpMock.Setup(restOp => restOp.PostAsync<IEnumerable<ExportImportDevice>, Task<string[]>>(It.IsAny<Uri>(), It.IsAny<IEnumerable<ExportImportDevice>>(), It.IsAny<IDictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>>>(), It.IsAny<IDictionary<string, string>>(), It.IsAny<CancellationToken>())).ReturnsAsync(null);

            var registryManager = new HttpRegistryManager(restOpMock.Object, IotHubName);
            await registryManager.RemoveDevicesAsync(new List<Device>() { badDevice1, badDevice2 }, false, CancellationToken.None);
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("API")]
        [ExpectedException(typeof(ArgumentException))]
        public async Task DeleteDevices2AsyncForceDeleteFalseMissingETagTest()
        {
            var badDevice1 = new Device("123") { ConnectionState = DeviceConnectionState.Connected };
            var badDevice2 = new Device("234") { ConnectionState = DeviceConnectionState.Connected };
            var restOpMock = new Mock<IHttpClientHelper>();
            restOpMock.Setup(restOp => restOp.PostAsync<IEnumerable<ExportImportDevice>, Task<BulkRegistryOperationResult>>(It.IsAny<Uri>(), It.IsAny<IEnumerable<ExportImportDevice>>(), It.IsAny<IDictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>>>(), It.IsAny<IDictionary<string, string>>(), It.IsAny<CancellationToken>())).ReturnsAsync(null);

            var registryManager = new HttpRegistryManager(restOpMock.Object, IotHubName);
            await registryManager.RemoveDevices2Async(new List<Device>() { badDevice1, badDevice2 }, false, CancellationToken.None);
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("API")]
        public async Task DeleteDevicesAsyncForceDeleteFalseTest()
        {
            var goodDevice1 = new Device("123") { ConnectionState = DeviceConnectionState.Connected, ETag = "234" };
            var goodDevice2 = new Device("234") { ConnectionState = DeviceConnectionState.Connected, ETag = "123" };
            var restOpMock = new Mock<IHttpClientHelper>();
            restOpMock.Setup(restOp => restOp.PostAsync<IEnumerable<ExportImportDevice>, Task<string[]>>(It.IsAny<Uri>(), It.IsAny<IEnumerable<ExportImportDevice>>(), It.IsAny<IDictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>>>(), It.IsAny<IDictionary<string, string>>(), It.IsAny<CancellationToken>())).ReturnsAsync(null);

            var registryManager = new HttpRegistryManager(restOpMock.Object, IotHubName);
            await registryManager.RemoveDevicesAsync(new List<Device>() { goodDevice1, goodDevice2 }, false, CancellationToken.None);
        }

        [TestMethod]
        [TestCategory("CIT")]
        [TestCategory("API")]
        public async Task DeleteDevices2AsyncForceDeleteFalseTest()
        {
            var goodDevice1 = new Device("123") { ConnectionState = DeviceConnectionState.Connected, ETag = "234" };
            var goodDevice2 = new Device("234") { ConnectionState = DeviceConnectionState.Connected, ETag = "123" };
            var restOpMock = new Mock<IHttpClientHelper>();
            restOpMock.Setup(restOp => restOp.PostAsync<IEnumerable<ExportImportDevice>, Task<BulkRegistryOperationResult>>(It.IsAny<Uri>(), It.IsAny<IEnumerable<ExportImportDevice>>(), It.IsAny<IDictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>>>(), It.IsAny<IDictionary<string, string>>(), It.IsAny<CancellationToken>())).ReturnsAsync(null);

            var registryManager = new HttpRegistryManager(restOpMock.Object, IotHubName);
            await registryManager.RemoveDevices2Async(new List<Device>() { goodDevice1, goodDevice2 }, false, CancellationToken.None);
        }
        
        private QueryExpression GetQueryExpression()
        {
            return new QueryExpression
            {
                Filter = new LogicalExpression
                {
                    LogicalOperator = LogicalOperatorType.And,
                    FilterExpressions = new List<FilterExpression>
                    {
                        new ComparisonExpression
                        {
                            ComparisonOperator = ComparisonOperatorType.GreaterThan,
                            Property = new QueryProperty
                            {
                                PropertyName = "Temperature",
                                PropertyType = PropertyType.DeviceProperty
                            },
                            Value = 70
                        },
                        new LogicalExpression
                        {
                            LogicalOperator = LogicalOperatorType.Or,
                            FilterExpressions = new List<FilterExpression>
                            {
                                new ComparisonExpression
                                {
                                    ComparisonOperator = ComparisonOperatorType.LessThan,
                                    Property = new QueryProperty
                                    {
                                        PropertyName = "Humidity",
                                        PropertyType = PropertyType.DeviceProperty
                                    },
                                    Value = 40
                                },
                                new ComparisonExpression
                                {
                                    ComparisonOperator = ComparisonOperatorType.GreaterThan,
                                    Property = new QueryProperty
                                    {
                                        PropertyName = "Humidity",
                                        PropertyType = PropertyType.DeviceProperty
                                    },
                                    Value = 60
                                }
                            }
                        }
                    }
                },
                Projection = new ProjectionExpression
                {
                    AllProperties = false,
                    ProjectedProperties = new List<QueryProperty>
                    {
                        new QueryProperty
                        {
                            PropertyName = "Temperature",
                            PropertyType = PropertyType.DeviceProperty
                        },
                        new QueryProperty
                        {
                            PropertyName = "Humidity",
                            PropertyType = PropertyType.DeviceProperty
                        }
                    }
                },
                Sort = new List<SortExpression>
                {
                    new SortExpression
                    {
                        Order = SortOrder.Ascending,
                        Property = new QueryProperty
                        {
                            PropertyName = "AvgHumidity",
                            PropertyType = PropertyType.AggregatedProperty
                        }
                    },
                    new SortExpression
                    {
                        Order = SortOrder.Ascending,
                        Property = new QueryProperty
                        {
                            PropertyName = "DeviceId"
                        }
                    }
                }
            };
        }
    }
}
