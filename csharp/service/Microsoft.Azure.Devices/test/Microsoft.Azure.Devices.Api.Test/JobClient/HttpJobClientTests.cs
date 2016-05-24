// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Api.Test.JobClient
{
    using System;
    using System.Collections.Generic;
    using System.Net;
    using System.Net.Http;
    using System.Threading;
    using System.Threading.Tasks;
    using Microsoft.VisualStudio.TestTools.UnitTesting;
    using Moq;
    using QueryExpressions;

    [TestClass]
    public class HttpJobClientTests
    {
        private readonly string jobId = "testJobId";
        private readonly string deviceId = "testDeviceId";
        private readonly List<string> deviceIds = new List<string>() { "testDeviceId", "testDeviceId2", "testDeviceId3", "testDeviceId4" };
        private readonly JobResponse expectedJobResponse = new JobResponse();
        private readonly string packageUri = "http://www.example.com/download";
        private readonly string configValue = "testConfigValue";
        private readonly TimeSpan timeout = TimeSpan.FromMinutes(1);

        private Mock<IHttpClientHelper> httpClientHelperMock;
        private readonly string iotHubName = "test";
        private HttpJobClient jobClient;

        [TestInitialize]
        public void Setup()
        {
            httpClientHelperMock = new Mock<IHttpClientHelper>();
            jobClient = new HttpJobClient(httpClientHelperMock.Object, iotHubName);
        }

        [TestMethod]
        public async Task ScheduleUpdateDeviceConfigurationAsyncTask()
        {
            this.ConfigurationUpdateJobParamTestSetup(configValue, CancellationToken.None);

            var response = await jobClient.ScheduleUpdateDeviceConfigurationAsync(jobId, deviceId, configValue);

            this.TestVerify(response);
        }

        [TestMethod]
        public async Task ScheduleUpdateDeviceConfigurationMultiDeviceAsyncTask()
        {
            this.ConfigurationUpdateJobParamMultiDeviceTestSetup(configValue, CancellationToken.None);

            var response = await jobClient.ScheduleUpdateDeviceConfigurationAsync(jobId, deviceIds, configValue);

            this.TestVerify(response);
        }

        [TestMethod]
        public async Task ScheduleFirmwareUpdateAsyncTest()
        {
            this.FirmwareUpdateJobParamTestSetup(packageUri, timeout, CancellationToken.None);

            // Null packageUri throws exception
            await TestAssert.ThrowsAsync<ArgumentNullException>(() => jobClient.ScheduleFirmwareUpdateAsync(jobId, deviceId, null, timeout));

            // timeout out of range throws exception
            await TestAssert.ThrowsAsync<ArgumentException>(() => jobClient.ScheduleFirmwareUpdateAsync(jobId, deviceId, packageUri, TimeSpan.Zero));
            await TestAssert.ThrowsAsync<ArgumentException>(() => jobClient.ScheduleFirmwareUpdateAsync(jobId, deviceId, packageUri, TimeSpan.FromDays(10)));

            // Success
            var response = await jobClient.ScheduleFirmwareUpdateAsync(jobId, deviceId, packageUri, timeout);

            this.TestVerify(response);
        }

        [TestMethod]
        public async Task ScheduleFirmwareUpdateAsyncWithCancellationTokenTest()
        {
            var cancellation = new CancellationToken();
            this.FirmwareUpdateJobParamTestSetup(packageUri, timeout, cancellation);

            var response =
                await jobClient.ScheduleFirmwareUpdateAsync(jobId, deviceId, packageUri, timeout, cancellation);

            this.TestVerify(response);
        }

        [TestMethod]
        public async Task ScheduleFactoryResetAsyncTest()
        {
            this.NoExtraJobParamTestSetup(JobType.FactoryResetDevice, CancellationToken.None);

            var response = await jobClient.ScheduleFactoryResetDeviceAsync(jobId, deviceId);

            this.TestVerify(response);
        }

        [TestMethod]
        public async Task ScheduleFactoryResetMultiDeviceAsyncTest()
        {
            this.NoExtraJobParamMultiDeviceTestSetup(JobType.FactoryResetDevice, CancellationToken.None);

            var response = await jobClient.ScheduleFactoryResetDeviceAsync(jobId, deviceIds);

            this.TestVerify(response);
        }

        [TestMethod]
        public async Task ScheduleFactoryResetAsyncWithCancellationTokenTest()
        {
            var cancellation = new CancellationToken();
            this.NoExtraJobParamTestSetup(JobType.FactoryResetDevice, cancellation);

            var response = await jobClient.ScheduleFactoryResetDeviceAsync(jobId, deviceId, cancellation);

            this.TestVerify(response);
        }

        [TestMethod]
        public async Task ScheduleFactoryResetMultiDeviceAsyncWithCancellationTokenTest()
        {
            var cancellation = new CancellationToken();
            this.NoExtraJobParamMultiDeviceTestSetup(JobType.FactoryResetDevice, cancellation);

            var response = await jobClient.ScheduleFactoryResetDeviceAsync(jobId, deviceIds, cancellation);

            this.TestVerify(response);
        }

        [TestMethod]
        public async Task ScheduleRebootAsyncTest()
        {
            this.NoExtraJobParamTestSetup(JobType.RebootDevice, CancellationToken.None);

            var response = await jobClient.ScheduleRebootDeviceAsync(jobId, deviceId);

            this.TestVerify(response);
        }

        [TestMethod]
        public async Task ScheduleRebootAsyncWithCancellationTokenTest()
        {
            var cancellation = new CancellationToken();
            this.NoExtraJobParamTestSetup(JobType.RebootDevice, cancellation);

            var response = await jobClient.ScheduleRebootDeviceAsync(jobId, deviceId, cancellation);

            this.TestVerify(response);
        }

        [TestMethod]
        public async Task JobHistoryQueryTest()
        {
            var jobResponseToReturn = new List<JobResponse>()
            { new JobResponse() };

            httpClientHelperMock.Setup(restOp => restOp.PostAsync<QueryExpression, 
                IEnumerable<JobResponse>>(
                It.IsAny<Uri>(), 
                It.IsAny<QueryExpression>(), 
                It.IsAny<IDictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>>>(), 
                null, 
                It.IsAny<CancellationToken>())
                ).ReturnsAsync(jobResponseToReturn);

            JobQueryResult results = await this.jobClient.QueryJobHistoryJsonAsync(string.Empty);

            Assert.AreEqual(new List<JobResponse>(results.Result).Count, 1);
        }

        private void ConfigurationUpdateJobParamTestSetup(string configValue, CancellationToken cancellationToken)
        {
            httpClientHelperMock.Setup(s => s.PutAsync<JobRequest, JobResponse>(
                It.IsAny<Uri>(),
                It.Is<JobRequest>(r =>
                    r.JobId == jobId &&
                    r.JobParameters.JobType == JobType.UpdateDeviceConfiguration &&
                    (r.JobParameters as UpdateDeviceConfigurationJobParameters).Value == configValue &&
                    (r.JobParameters as DeviceJobParameters).DeviceIds.Contains(deviceId)),
                It.IsAny<Dictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>>>(),
                It.Is<CancellationToken>(c => c == cancellationToken)))
                .Returns(Task.FromResult(expectedJobResponse));
        }

        private void ConfigurationUpdateJobParamMultiDeviceTestSetup(string configValue, CancellationToken cancellationToken)
        {
            httpClientHelperMock.Setup(s => s.PutAsync<JobRequest, JobResponse>(
                It.IsAny<Uri>(),
                It.Is<JobRequest>(r =>
                    r.JobId == jobId &&
                    r.JobParameters.JobType == JobType.UpdateDeviceConfiguration &&
                    (r.JobParameters as UpdateDeviceConfigurationJobParameters).Value == configValue &&
                    (r.JobParameters as DeviceJobParameters).DeviceIds.Contains(deviceId) &&
                    (r.JobParameters as DeviceJobParameters).DeviceIds.Count == deviceIds.Count),
                It.IsAny<Dictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>>>(),
                It.Is<CancellationToken>(c => c == cancellationToken)))
                .Returns(Task.FromResult(expectedJobResponse));
        }

        private void FirmwareUpdateJobParamTestSetup(string packageUri, TimeSpan timeOut, CancellationToken cancellationToken)
        {
            httpClientHelperMock.Setup(s => s.PutAsync<JobRequest, JobResponse>(
                It.IsAny<Uri>(),
                It.Is<JobRequest>(r =>
                    r.JobId == jobId &&
                    r.JobParameters.JobType == JobType.FirmwareUpdate &&
                    (r.JobParameters as FirmwareUpdateJobParameters).PackageUri == packageUri &&
                    (r.JobParameters as FirmwareUpdateJobParameters).Timeout == timeOut &&
                    (r.JobParameters as DeviceJobParameters).DeviceIds.Contains(deviceId)),
                It.IsAny<Dictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>>>(),
                It.Is<CancellationToken>(c => c == cancellationToken)))
                .Returns(Task.FromResult(expectedJobResponse));
        }

        private void NoExtraJobParamTestSetup(JobType jobType, CancellationToken cancellationToken)
        {
            httpClientHelperMock.Setup(s => s.PutAsync<JobRequest, JobResponse>(
                It.IsAny<Uri>(),
                It.Is<JobRequest>(
                    r =>
                        r.JobId == jobId && r.JobParameters.JobType == jobType &&
                        (r.JobParameters as DeviceJobParameters).DeviceIds.Contains(deviceId)),
                It.IsAny<Dictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>>>(),
                It.Is<CancellationToken>(c => c == cancellationToken)))
                .Returns(Task.FromResult(expectedJobResponse));
        }

        private void NoExtraJobParamMultiDeviceTestSetup(JobType jobType, CancellationToken cancellationToken)
        {
            httpClientHelperMock.Setup(s => s.PutAsync<JobRequest, JobResponse>(
                It.IsAny<Uri>(),
                It.Is<JobRequest>(
                    r =>
                        r.JobId == jobId && r.JobParameters.JobType == jobType &&
                        (r.JobParameters as DeviceJobParameters).DeviceIds.Contains(deviceId) &&
                        (r.JobParameters as DeviceJobParameters).DeviceIds.Count == deviceIds.Count),
                It.IsAny<Dictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>>>(),
                It.Is<CancellationToken>(c => c == cancellationToken)))
                .Returns(Task.FromResult(expectedJobResponse));
        }

        private void TestVerify(JobResponse actualJobResponse)
        {
            Assert.AreEqual(expectedJobResponse, actualJobResponse);

            httpClientHelperMock.Verify(v => v.PutAsync<JobRequest, JobResponse>(
                It.IsAny<Uri>(),
                It.IsAny<JobRequest>(),
                It.IsAny<Dictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>>>(),
                It.IsAny<CancellationToken>()), Times.Once);
        }
    }
}