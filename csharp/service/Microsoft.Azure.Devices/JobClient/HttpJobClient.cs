// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices
{
    using System;
    using System.Collections.Generic;
    using System.Net;
    using System.Net.Http;
    using System.Text;
    using System.Threading;
    using System.Threading.Tasks;
    using Microsoft.Azure.Devices.Common;
    using Microsoft.Azure.Devices.Common.Exceptions;

    class HttpJobClient : JobClient, IDisposable
    {
        const string JobsUriFormat = "/jobs/v2/{0}?{1}";
        const string JobsQueryFormat = "/jobs/v2/query?{0}";
        const string CancelJobUriFormat = "/jobs/v2/{0}/cancel?{1}";

        const string ContinuationTokenHeader = "x-ms-continuation";
        const string PageSizeHeader = "x-ms-max-item-count";

        static readonly TimeSpan DefaultOperationTimeout = TimeSpan.FromSeconds(100);

        IHttpClientHelper httpClientHelper;
        readonly string iotHubName;

        internal HttpJobClient(IotHubConnectionString connectionString)
        {
            this.iotHubName = connectionString.IotHubName;
            this.httpClientHelper = new HttpClientHelper(
                connectionString.HttpsEndpoint,
                connectionString,
                ExceptionHandlingHelper.GetDefaultErrorMapping(),
                DefaultOperationTimeout,
                client => { });
        }

        // internal test helper
        internal HttpJobClient(IHttpClientHelper httpClientHelper, string iotHubName)
        {
            if (httpClientHelper == null)
            {
                throw new ArgumentNullException(nameof(httpClientHelper));
            }

            this.iotHubName = iotHubName;
            this.httpClientHelper = httpClientHelper;
        }

        public override Task OpenAsync()
        {
            return TaskHelpers.CompletedTask;
        }

        public override Task CloseAsync()
        {
            if (this.httpClientHelper != null)
            {
                this.httpClientHelper.Dispose();
                this.httpClientHelper = null;
            }

            return TaskHelpers.CompletedTask;
        }

        /// <inheritdoc/>
        public void Dispose()
        {
            this.Dispose(true);
            GC.SuppressFinalize(this);
        }

        /// <summary>
        /// Dispose resources
        /// </summary>
        /// <param name="disposing">
        /// Governs disposable of managed and native resources
        /// </param>
        protected virtual void Dispose(bool disposing)
        {
            if (disposing)
            {
                if (this.httpClientHelper != null)
                {
                    this.httpClientHelper.Dispose();
                    this.httpClientHelper = null;
                }
            }
        }

        public override Task<JobResponse> GetJobAsync(string jobId)
        {
            return this.GetJobAsync(jobId, CancellationToken.None);
        }

        public override IQuery CreateQuery()
        {
            return CreateQuery(null, null, null);
        }

        public override IQuery CreateQuery(int? pageSize)
        {
            return CreateQuery(null, null, pageSize);
        }

        public override IQuery CreateQuery(JobType? jobType, JobStatus? jobStatus)
        {
            return CreateQuery(jobType, jobStatus, null);
        }

        public override IQuery CreateQuery(JobType? jobType, JobStatus? jobStatus, int? pageSize)
        {
            return new Query((token) => this.GetJobsAsync(jobType, jobStatus, pageSize, token, CancellationToken.None));
        }

        public override Task<JobResponse> CancelJobAsync(string jobId)
        {
            return this.CancelJobAsync(jobId, CancellationToken.None);
        }

        public override Task<JobResponse> GetJobAsync(string jobId, CancellationToken cancellationToken)
        {
            this.EnsureInstanceNotClosed();

            var errorMappingOverrides = new Dictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>>
            {
                {
                    HttpStatusCode.NotFound,
                    responseMessage => Task.FromResult((Exception) new JobNotFoundException(jobId))
                }
            };

            return this.httpClientHelper.GetAsync<JobResponse>(
                GetJobUri(jobId),
                errorMappingOverrides,
                null,
                cancellationToken);
        }

        public override Task<JobResponse> CancelJobAsync(string jobId, CancellationToken cancellationToken)
        {
            this.EnsureInstanceNotClosed();

            return this.httpClientHelper.PostAsync<string, JobResponse>(
                new Uri(CancelJobUriFormat.FormatInvariant(jobId, ClientApiVersionHelper.ApiVersionQueryString), UriKind.Relative),
                null,
                null,
                null,
                cancellationToken);
        }

        /// <inheritdoc/>
        public override Task<JobResponse> ScheduleDeviceMethodAsync(
            string jobId, 
            string queryCondition,
            CloudToDeviceMethod methodCall, 
            DateTime startTimeUtc, 
            long maxExecutionTimeInSeconds)
        {
            return this.ScheduleDeviceMethodAsync(jobId, queryCondition, methodCall, startTimeUtc, maxExecutionTimeInSeconds, CancellationToken.None);
        }

        /// <inheritdoc/>
        public override Task<JobResponse> ScheduleDeviceMethodAsync(
            string jobId, 
            string queryCondition,
            CloudToDeviceMethod cloudToDeviceMethod, 
            DateTime startTimeUtc,
            long maxExecutionTimeInSeconds,
            CancellationToken cancellationToken)
        {
            this.EnsureInstanceNotClosed();

            var jobRequest = new JobRequest()
            {
                JobId = jobId,
                JobType = JobType.ScheduleDeviceMethod,
                CloudToDeviceMethod = cloudToDeviceMethod,
                QueryCondition = queryCondition,
                StartTime = startTimeUtc,
                MaxExecutionTimeInSeconds = maxExecutionTimeInSeconds
            };

            return this.CreateJobAsync(jobRequest, cancellationToken);
        }

        public override Task<JobResponse> ScheduleTwinUpdateAsync(
            string jobId, 
            string queryCondition,
            Twin twin, 
            DateTime startTimeUtc,
            long maxExecutionTimeInSeconds)
        {
            return this.ScheduleTwinUpdateAsync(jobId, queryCondition, twin, startTimeUtc, maxExecutionTimeInSeconds, CancellationToken.None);
        }

        public override Task<JobResponse> ScheduleTwinUpdateAsync(
            string jobId, 
            string queryCondition,
            Twin twin, 
            DateTime startTimeUtc,
            long maxExecutionTimeInSeconds,
            CancellationToken cancellationToken)
        {
            this.EnsureInstanceNotClosed();

            var jobRequest = new JobRequest()
            {
                JobId = jobId,
                JobType = JobType.ScheduleUpdateTwin,
                UpdateTwin = twin,
                QueryCondition = queryCondition,
                StartTime = startTimeUtc,
                MaxExecutionTimeInSeconds = maxExecutionTimeInSeconds
            };

            return this.CreateJobAsync(jobRequest, cancellationToken);
        }

        Task<JobResponse> CreateJobAsync(JobRequest jobRequest, CancellationToken cancellationToken)
        {
            var errorMappingOverrides = new Dictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>>
            {
                {
                    HttpStatusCode.PreconditionFailed,
                    async (responseMessage) =>
                        new PreconditionFailedException(
                            await ExceptionHandlingHelper.GetExceptionMessageAsync(responseMessage))
                },
                {
                    HttpStatusCode.NotFound, async responseMessage =>
                    {
                        var responseContent = await ExceptionHandlingHelper.GetExceptionMessageAsync(responseMessage);
                        return (Exception) new DeviceNotFoundException(responseContent, (Exception) null);
                    }
                }
            };

            return this.httpClientHelper.PutAsync<JobRequest, JobResponse>(
                GetJobUri(jobRequest.JobId),
                jobRequest,
                errorMappingOverrides,
                cancellationToken);
        }

        void EnsureInstanceNotClosed()
        {
            if (this.httpClientHelper == null)
            {
                throw new ObjectDisposedException("JobClient", ApiResources.JobClientInstanceAlreadyClosed);
            }
        }

        async Task<QueryResult> GetJobsAsync(JobType? jobType, JobStatus? jobStatus, int? pageSize, string continuationToken, CancellationToken cancellationToken)
        {
            this.EnsureInstanceNotClosed();

            var customHeaders = new Dictionary<string, string>();
            if (!string.IsNullOrWhiteSpace(continuationToken))
            {
                customHeaders.Add(ContinuationTokenHeader, continuationToken);
            }

            if (pageSize != null)
            {
                customHeaders.Add(PageSizeHeader, pageSize.ToString());
            }

            HttpResponseMessage response = await this.httpClientHelper.GetAsync<HttpResponseMessage>(
                BuildQueryJobUri(jobType, jobStatus),
                null,
                customHeaders,
                cancellationToken);

            return await QueryResult.FromHttpResponseAsync(response);
        }

        Uri BuildQueryJobUri(JobType? jobType, JobStatus? jobStatus)
        {
            StringBuilder stringBuilder = new StringBuilder(JobsQueryFormat.FormatInvariant(ClientApiVersionHelper.ApiVersionQueryString));

            if (jobType != null)
            {
                stringBuilder.Append("&jobType={0}".FormatInvariant(WebUtility.UrlEncode(jobType.ToString())));
            }

            if (jobStatus != null)
            {
                stringBuilder.Append("&jobStatus={0}".FormatInvariant(WebUtility.UrlEncode(jobStatus.ToString())));
            }

            return new Uri(stringBuilder.ToString(), UriKind.Relative);
        }

        static Uri GetJobUri(string jobId)
        {
            return new Uri(JobsUriFormat.FormatInvariant(jobId ?? string.Empty, ClientApiVersionHelper.ApiVersionQueryString), UriKind.Relative);
        }
    }
}
