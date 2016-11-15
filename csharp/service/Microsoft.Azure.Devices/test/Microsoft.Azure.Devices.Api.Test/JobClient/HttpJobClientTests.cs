// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

using System.Linq;

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

    [TestClass]
    public class HttpJobClientTests
    {
        private readonly string jobId = "testJobId";
        private readonly string deviceId = "testDeviceId";
        private readonly List<string> deviceIds = new List<string>() { "testDeviceId", "testDeviceId2", "testDeviceId3", "testDeviceId4" };
        private readonly JobResponse expectedJobResponse = new JobResponse();
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

        private void NoExtraJobParamTestSetup(JobType jobType, CancellationToken cancellationToken)
        {
            httpClientHelperMock.Setup(s => s.PutAsync<JobRequest, JobResponse>(
                It.IsAny<Uri>(),
                It.Is<JobRequest>(
                    r =>
                        r.JobId == jobId && r.JobType == jobType),
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
                        r.JobId == jobId && r.JobType == jobType),
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