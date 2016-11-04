// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Api.Test
{
    using System.Collections.Generic;
    using System.Linq;
    using System.Threading.Tasks;
    using Microsoft.VisualStudio.TestTools.UnitTesting;
    using Newtonsoft.Json;

    [TestClass]
    public class QueryTests
    {
        [TestMethod]
        public void QueryResult_CastContentToTwin_NoContinuation_Test()
        {
            // simulate json serialize/deserialize
            var serverQueryResult = new QueryResult()
            {
                Type = QueryResultType.Twin,
                Items = new List<Twin>()
                {
                    new Twin()
                    {
                        DeviceId = "test",
                    }
                }
            };
            // serialize
            var jsonQueryResult = JsonConvert.SerializeObject(serverQueryResult);
            Assert.AreEqual("{\"type\":\"twin\",\"items\":[{\"deviceId\":\"test\",\"etag\":null,\"properties\":{\"desired\":{},\"reported\":{}}}],\"continuationToken\":null}", jsonQueryResult);

            // deserialize
            var clientQueryResult = JsonConvert.DeserializeObject<QueryResult>(jsonQueryResult);

            // test
            IQuery q = new Query((t) => Task.FromResult<QueryResult>(clientQueryResult));

            // validate
            Assert.IsTrue(q.HasMoreResults);

            IEnumerable<Twin> r = q.GetNextAsTwinAsync().Result;
            Assert.AreEqual(1, r.Count());
            Assert.IsInstanceOfType(r.ElementAt(0), typeof(Twin));
            Assert.AreEqual("test", r.ElementAt(0).DeviceId);
            Assert.IsFalse(q.HasMoreResults);
        }

        [TestMethod]
        public void QueryResult_CastContentToTwin_Continuation_Test()
        {
            // simulate json serialize/deserialize
            var serverQueryResult = new QueryResult()
            {
                Type = QueryResultType.Twin,
                Items = new List<Twin>()
                {
                    new Twin()
                    {
                        DeviceId = "test",
                    }
                },
                ContinuationToken = "GYUVJDBJFKJ"
            };
            // serialize
            var jsonQueryResult = JsonConvert.SerializeObject(serverQueryResult);
            Assert.AreEqual("{\"type\":\"twin\",\"items\":[{\"deviceId\":\"test\",\"etag\":null,\"properties\":{\"desired\":{},\"reported\":{}}}],\"continuationToken\":\"GYUVJDBJFKJ\"}", jsonQueryResult);
            // deserialize
            var clientQueryResult = JsonConvert.DeserializeObject<QueryResult>(jsonQueryResult);

            // test
            IQuery q = new Query((t) => Task.FromResult<QueryResult>(clientQueryResult));

            // validate
            Assert.IsTrue(q.HasMoreResults);

            IEnumerable<Twin> r = q.GetNextAsTwinAsync().Result;
            Assert.AreEqual(1, r.Count());
            Assert.IsInstanceOfType(r.ElementAt(0), typeof(Twin));
            Assert.AreEqual("test", r.ElementAt(0).DeviceId);
            Assert.IsTrue(q.HasMoreResults);
        }

        [TestMethod]
        public void QueryResult_CastContentToDeviceJob_Test()
        {
            // simulate json serialize/deserialize
            IEnumerable<DeviceJob> jobs = new List<DeviceJob>()
            {
                new DeviceJob()
                {
                    DeviceId = "123456",
                    JobId = "789",
                    JobType = DeviceJobType.ScheduleUpdateTwin,
                    Status = DeviceJobStatus.Running
                }
            };

            var serverQueryResult = new QueryResult()
            {
                Type = QueryResultType.DeviceJob,
                Items = jobs
            };

            // serialize
            var jsonQueryResult = JsonConvert.SerializeObject(serverQueryResult);
            Assert.AreEqual("{\"type\":\"deviceJob\",\"items\":[{\"deviceId\":\"123456\",\"jobId\":\"789\",\"jobType\":\"scheduleUpdateTwin\",\"status\":\"running\",\"startTimeUtc\":\"0001-01-01T00:00:00\",\"endTimeUtc\":\"0001-01-01T00:00:00\",\"createdDateTimeUtc\":\"0001-01-01T00:00:00\",\"lastUpdatedDateTimeUtc\":\"0001-01-01T00:00:00\"}],\"continuationToken\":null}", jsonQueryResult);

            // deserialize
            var clientQueryResult = JsonConvert.DeserializeObject<QueryResult>(jsonQueryResult);

            // test
            IQuery q = new Query((t) => Task.FromResult<QueryResult>(clientQueryResult));

            // validate
            Assert.IsTrue(q.HasMoreResults);
            IEnumerable<DeviceJob> content = q.GetNextAsDeviceJobAsync().Result;

            Assert.AreEqual(1, content.Count());
            Assert.IsInstanceOfType(content.ElementAt(0), typeof(DeviceJob));
            Assert.AreEqual("123456", content.ElementAt(0).DeviceId);
            Assert.AreEqual("789", content.ElementAt(0).JobId);
            Assert.AreEqual(DeviceJobStatus.Running, content.ElementAt(0).Status);
        }

        [TestMethod]
        public void QueryResult_CastContentToJobResponse_Test()
        {
            // simulate json serialize/deserialize
            IEnumerable<JobResponse> jobs = new List<JobResponse>()
            {
                new JobResponse()
                {
                    DeviceId = "123456",
                    JobId = "789",
                    Type = JobType.ScheduleUpdateTwin,
                    Status = JobStatus.Completed
                }
            };

            var serverQueryResult = new QueryResult()
            {
                Type = QueryResultType.JobResponse,
                Items = jobs
            };

            // serialize
            var jsonQueryResult = JsonConvert.SerializeObject(serverQueryResult);
            Assert.AreEqual("{\"type\":\"jobResponse\",\"items\":[{\"jobId\":\"789\",\"type\":\"scheduleUpdateTwin\",\"status\":\"completed\",\"deviceId\":\"123456\"}],\"continuationToken\":null}", jsonQueryResult);

            // deserialize
            var clientQueryResult = JsonConvert.DeserializeObject<QueryResult>(jsonQueryResult);

            // test
            IQuery q = new Query((t) => Task.FromResult<QueryResult>(clientQueryResult));

            // validate
            Assert.IsTrue(q.HasMoreResults);
            IEnumerable<JobResponse> content = q.GetNextAsJobResponseAsync().Result;

            Assert.AreEqual(1, content.Count());
            Assert.IsInstanceOfType(content.ElementAt(0), typeof(JobResponse));
            Assert.AreEqual("123456", content.ElementAt(0).DeviceId);
            Assert.AreEqual("789", content.ElementAt(0).JobId);
            Assert.AreEqual(JobType.ScheduleUpdateTwin, content.ElementAt(0).Type);
            Assert.AreEqual(JobStatus.Completed, content.ElementAt(0).Status);
        }

        [TestMethod]
        public void QueryResult_CastContentToJson_Test()
        {
            // simulate json serialize/deserialize
            IEnumerable<DeviceJob> jobs = new List<DeviceJob>()
            {
                new DeviceJob()
                {
                    DeviceId = "123456",
                    JobId = "789",
                    JobType = DeviceJobType.ScheduleDeviceMethod,
                    Status = DeviceJobStatus.Running
                }
            };

            var serverQueryResult = new QueryResult()
            {
                Type = QueryResultType.DeviceJob,
                Items = jobs
            };

            // serialize
            var jsonQueryResult = JsonConvert.SerializeObject(serverQueryResult);
            Assert.AreEqual("{\"type\":\"deviceJob\",\"items\":[{\"deviceId\":\"123456\",\"jobId\":\"789\",\"jobType\":\"scheduleDeviceMethod\",\"status\":\"running\",\"startTimeUtc\":\"0001-01-01T00:00:00\",\"endTimeUtc\":\"0001-01-01T00:00:00\",\"createdDateTimeUtc\":\"0001-01-01T00:00:00\",\"lastUpdatedDateTimeUtc\":\"0001-01-01T00:00:00\"}],\"continuationToken\":null}", jsonQueryResult);
            // deserialize
            var clientQueryResult = JsonConvert.DeserializeObject<QueryResult>(jsonQueryResult);

            // test
            IQuery q = new Query((t) => Task.FromResult<QueryResult>(clientQueryResult));

            // validate
            Assert.IsTrue(q.HasMoreResults);
            IEnumerable<DeviceJob> content = q.GetNextAsDeviceJobAsync().Result;

            Assert.AreEqual(1, content.Count());
            Assert.IsInstanceOfType(content.ElementAt(0), typeof(DeviceJob));
            Assert.AreEqual("123456", content.ElementAt(0).DeviceId);
            Assert.AreEqual("789", content.ElementAt(0).JobId);
            Assert.AreEqual(DeviceJobStatus.Running, content.ElementAt(0).Status);
        }

        [TestMethod]
        public void QueryResult_CallNextOutsideWhile_Test()
        {
            // simulate json serialize/deserialize
            IEnumerable<DeviceJob> jobs = new List<DeviceJob>()
            {
                new DeviceJob()
                {
                    DeviceId = "123456",
                    JobId = "789",
                    JobType = DeviceJobType.ScheduleDeviceMethod,
                    Status = DeviceJobStatus.Running
                }
            };

            var serverQueryResult = new QueryResult()
            {
                Type = QueryResultType.DeviceJob,
                Items = jobs
            };

            var jsonQueryResult = JsonConvert.SerializeObject(serverQueryResult);
            var clientQueryResult = JsonConvert.DeserializeObject<QueryResult>(jsonQueryResult);

            // test
            IQuery q = new Query((t) => Task.FromResult<QueryResult>(clientQueryResult));

            while (q.HasMoreResults)
            {
                q.GetNextAsJsonAsync().Wait();
            }

            // validate
            // after a while, q.HasMoreResult should be false and GetNext() should return an empty enumerable
            Assert.IsFalse(q.HasMoreResults);
            Assert.IsFalse(q.GetNextAsJsonAsync().Result.Any());
        }
    }
}