// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices
{
    using System;
    using System.Collections.Generic;
    using System.Net;
    using System.Net.Http;
    using System.Threading;
    using System.Threading.Tasks;

    using Microsoft.Azure.Devices.Common;
    using Microsoft.Azure.Devices.Common.Exceptions;
    using Newtonsoft.Json;
    using QueryExpressions;

    class HttpJobClient : JobClient, IDisposable
    {
        const string JobsUriFormat = "/jobs/v2/{0}?{1}";
        const string JobsQueryExpressionUriFormat = "/jobs/v2/query?" + ClientApiVersionHelper.ApiVersionQueryString;

        static readonly TimeSpan DefaultOperationTimeout = TimeSpan.FromSeconds(100);
        private static readonly TimeSpan DefaultFirmwareUpdateJobTimeout = TimeSpan.FromMinutes(30);

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

        internal override Task<IEnumerable<JobResponse>> GetJobsAsync()
        {
            return this.GetJobsAsync(CancellationToken.None);
        }

        public override Task CancelJobAsync(string jobId)
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

        internal override Task<IEnumerable<JobResponse>> GetJobsAsync(CancellationToken cancellationToken)
        {
            this.EnsureInstanceNotClosed();

            return this.httpClientHelper.GetAsync<IEnumerable<JobResponse>>(
                GetJobUri(string.Empty),
                null,
                null,
                cancellationToken);
        }

        public override Task CancelJobAsync(string jobId, CancellationToken cancellationToken)
        {
            this.EnsureInstanceNotClosed();

            var errorMappingOverrides = new Dictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>>
            {
                {
                    HttpStatusCode.NotFound,
                    responseMessage => Task.FromResult((Exception) new JobNotFoundException(jobId))
                }
            };

            IETagHolder temp = null;
            return this.httpClientHelper.DeleteAsync(
                GetJobUri(jobId),
                temp,
                errorMappingOverrides,
                null,
                cancellationToken);
        }

        // ScheduleDevicePropertyRead
        public override Task<JobResponse> ScheduleDevicePropertyReadAsync(string jobId, string deviceId, string propertyName)
        {
            return this.ScheduleDevicePropertyReadAsync(jobId, deviceId, propertyName, CancellationToken.None);
        }

        public override Task<JobResponse> ScheduleDevicePropertyReadAsync(string jobId, string deviceId, string propertyName, CancellationToken cancellationToken)
        {
            return this.SchedulePropertyReadAsync(jobId, new List<string>() { deviceId }, propertyName, cancellationToken);
        }

        public override Task<JobResponse> ScheduleDevicePropertyReadAsync(string jobId, IEnumerable<string> deviceIds, string propertyName)
        {
            return this.ScheduleDevicePropertyReadAsync(jobId, deviceIds, propertyName, CancellationToken.None);
        }

        public override Task<JobResponse> ScheduleDevicePropertyReadAsync(string jobId, IEnumerable<string> deviceIds, string propertyName, CancellationToken cancellationToken)
        {
            return this.SchedulePropertyReadAsync(jobId, deviceIds, propertyName, cancellationToken);
        }

        // DevicePropertyWrite
        public override Task<JobResponse> ScheduleDevicePropertyWriteAsync(string jobId, string deviceId, string propertyName, object value)
        {
            return this.ScheduleDevicePropertyWriteAsync(jobId, deviceId, propertyName, value, CancellationToken.None);
        }

        public override Task<JobResponse> ScheduleDevicePropertyWriteAsync(string jobId, string deviceId, string propertyName, object value, CancellationToken cancellationToken)
        {
            return this.SchedulePropertyWriteAsync(jobId, new List<string>() { deviceId }, propertyName, value, cancellationToken);
        }

        public override Task<JobResponse> ScheduleDevicePropertyWriteAsync(string jobId, IEnumerable<string> deviceIds, string propertyName, object value)
        {
            return this.ScheduleDevicePropertyWriteAsync(jobId, deviceIds, propertyName, value, CancellationToken.None);
        }

        public override Task<JobResponse> ScheduleDevicePropertyWriteAsync(string jobId, IEnumerable<string> deviceIds, string propertyName, object value, CancellationToken cancellationToken)
        {
            return this.SchedulePropertyWriteAsync(jobId, deviceIds, propertyName, value, cancellationToken);
        }

        // UpdateDeviceConfiguration
        public override Task<JobResponse> ScheduleUpdateDeviceConfigurationAsync(string jobId, string deviceId, string value)
        {
            return this.ScheduleUpdateDeviceConfigurationAsync(jobId, new List<string>() { deviceId }, value, CancellationToken.None);
        }

        public override Task<JobResponse> ScheduleUpdateDeviceConfigurationAsync(string jobId, string deviceId, string value, CancellationToken cancellationToken)
        {
            return this.ScheduleUpdateDeviceConfigurationAsync(jobId, new List<string>() { deviceId }, value, cancellationToken);
        }

        public override Task<JobResponse> ScheduleUpdateDeviceConfigurationAsync(string jobId, IEnumerable<string> deviceIds, string value)
        {
            return this.ScheduleUpdateDeviceConfigurationAsync(jobId, deviceIds, value, CancellationToken.None);
        }

        public override Task<JobResponse> ScheduleUpdateDeviceConfigurationAsync(string jobId, IEnumerable<string> deviceIds, string value, CancellationToken cancellationToken)
        {
            this.EnsureInstanceNotClosed();

            var jobRequest = new JobRequest()
            {
                JobId = jobId,
                JobParameters = new UpdateDeviceConfigurationJobParameters(value, deviceIds)
            };

            return this.CreateJobAsync(jobRequest, cancellationToken);
        }

        // FirmwareUpdate
        public override Task<JobResponse> ScheduleFirmwareUpdateAsync(string jobId, string deviceId, string packageUri)
        {
            return this.ScheduleFirmwareUpdateAsync(jobId, new List<string>() { deviceId }, packageUri, DefaultFirmwareUpdateJobTimeout, CancellationToken.None);
        }

        public override Task<JobResponse> ScheduleFirmwareUpdateAsync(string jobId, string deviceId, string packageUri, CancellationToken cancellationToken)
        {
            return this.ScheduleFirmwareUpdateAsync(jobId, new List<string>() { deviceId }, packageUri, DefaultFirmwareUpdateJobTimeout, cancellationToken);
        }

        public override Task<JobResponse> ScheduleFirmwareUpdateAsync(string jobId, IEnumerable<string> deviceIds, string packageUri)
        {
            return this.ScheduleFirmwareUpdateAsync(jobId, deviceIds, packageUri, DefaultFirmwareUpdateJobTimeout, CancellationToken.None);
        }

        public override Task<JobResponse> ScheduleFirmwareUpdateAsync(string jobId, IEnumerable<string> deviceIds, string packageUri, CancellationToken cancellationToken)
        {
            return this.ScheduleFirmwareUpdateAsync(jobId, deviceIds, packageUri, DefaultFirmwareUpdateJobTimeout, cancellationToken);
        }

        public override Task<JobResponse> ScheduleFirmwareUpdateAsync(string jobId, string deviceId, string packageUri, TimeSpan timeout)
        {
            return this.ScheduleFirmwareUpdateAsync(jobId, new List<string>() { deviceId }, packageUri, timeout, CancellationToken.None);
        }

        public override Task<JobResponse> ScheduleFirmwareUpdateAsync(string jobId, string deviceId, string packageUri, TimeSpan timeout, CancellationToken cancellationToken)
        {
            return this.ScheduleFirmwareUpdateAsync(jobId, new List<string>() { deviceId }, packageUri, timeout, cancellationToken);
        }

        public override Task<JobResponse> ScheduleFirmwareUpdateAsync(string jobId, IEnumerable<string> deviceIds, string packageUri, TimeSpan timeout)
        {
            return this.ScheduleFirmwareUpdateAsync(jobId, deviceIds, packageUri, timeout, CancellationToken.None);
        }

        public override Task<JobResponse> ScheduleFirmwareUpdateAsync(string jobId, IEnumerable<string> deviceIds, string packageUri, TimeSpan timeout, CancellationToken cancellationToken)
        {
            this.EnsureInstanceNotClosed();

            if (string.IsNullOrWhiteSpace(packageUri))
            {
                throw new ArgumentNullException(nameof(packageUri));
            }

            if (timeout < TimeSpan.FromMinutes(1) || timeout > TimeSpan.FromDays(1))
            {
                throw new ArgumentException(ApiResources.FirmwareUpdateJob_InvalidTimeout, nameof(timeout));
            }

            var jobRequest = new JobRequest()
            {
                JobId = jobId,
                JobParameters = new FirmwareUpdateJobParameters(packageUri, timeout, deviceIds)
            };

            return this.CreateJobAsync(jobRequest, cancellationToken);
        }

        // ScheduleReboot
        public override Task<JobResponse> ScheduleRebootDeviceAsync(string jobId, string deviceId)
        {
            return this.ScheduleRebootDeviceAsync(jobId, new List<string>() { deviceId }, CancellationToken.None);
        }

        public override Task<JobResponse> ScheduleRebootDeviceAsync(string jobId, string deviceId, CancellationToken cancellationToken)
        {
            return this.ScheduleRebootDeviceAsync(jobId, new List<string>() { deviceId }, cancellationToken);
        }

        public override Task<JobResponse> ScheduleRebootDeviceAsync(string jobId, IEnumerable<string> deviceIds)
        {
            return this.ScheduleRebootDeviceAsync(jobId, deviceIds, CancellationToken.None);
        }

        public override Task<JobResponse> ScheduleRebootDeviceAsync(string jobId, IEnumerable<string> deviceIds, CancellationToken cancellationToken)
        {
            this.EnsureInstanceNotClosed();

            var jobRequest = new JobRequest()
            {
                JobId = jobId,
                JobParameters = new DeviceJobParameters(JobType.RebootDevice, deviceIds)
            };

            return this.CreateJobAsync(jobRequest, cancellationToken);
        }

        // FactoryReset
        public override Task<JobResponse> ScheduleFactoryResetDeviceAsync(string jobId, string deviceId)
        {
            return this.ScheduleFactoryResetDeviceAsync(jobId, new List<string>() { deviceId }, CancellationToken.None);
        }

        public override Task<JobResponse> ScheduleFactoryResetDeviceAsync(string jobId, string deviceId, CancellationToken cancellationToken)
        {
            return this.ScheduleFactoryResetDeviceAsync(jobId, new List<string>() { deviceId }, cancellationToken);
        }

        public override Task<JobResponse> ScheduleFactoryResetDeviceAsync(string jobId, IEnumerable<string> deviceIds)
        {
            return this.ScheduleFactoryResetDeviceAsync(jobId, deviceIds, CancellationToken.None);
        }

        public override Task<JobResponse> ScheduleFactoryResetDeviceAsync(string jobId, IEnumerable<string> deviceIds, CancellationToken cancellationToken)
        {
            this.EnsureInstanceNotClosed();

            var jobRequest = new JobRequest()
            {
                JobId = jobId,
                JobParameters = new DeviceJobParameters(JobType.FactoryResetDevice, deviceIds)
            };

            return this.CreateJobAsync(jobRequest, cancellationToken);
        }

        Task<JobResponse> SchedulePropertyReadAsync(
            string jobId,
            IEnumerable<string> deviceIds,
            string propertyName,
            CancellationToken cancellationToken)
        {
            this.EnsureInstanceNotClosed();

            var jobRequest = new JobRequest()
            {
                JobId = jobId,
                JobParameters = new ReadDevicePropertiesJobParameters(new List<string>() { propertyName }, deviceIds)
            };

            return this.CreateJobAsync(jobRequest, cancellationToken);
        }
        
        Task<JobResponse> SchedulePropertyWriteAsync(
            string jobId,
            IEnumerable<string> deviceIds,
            string propertyName,
            object value,
            CancellationToken cancellationToken)
        {
            this.EnsureInstanceNotClosed();

            if (string.IsNullOrEmpty(propertyName))
            {
                throw new ArgumentNullException(nameof(propertyName));
            }

            if (value == null)
            {
                throw new ArgumentNullException(nameof(value));
            }

            var jobRequest = new JobRequest()
            {
                JobId = jobId,
                JobParameters = new WriteDevicePropertiesJobParameters(new Dictionary<string, object>() { { propertyName, value } }, deviceIds)
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

        static Uri GetJobUri(string jobId)
        {
            return new Uri(JobsUriFormat.FormatInvariant(jobId ?? string.Empty, ClientApiVersionHelper.ApiVersionQueryString), UriKind.Relative);
        }

        public override Task<JobQueryResult> QueryJobHistoryJsonAsync(string queryJson)
        {
            return this.QueryJobHistoryJsonAsync(queryJson, CancellationToken.None);
        }

        public override Task<JobQueryResult> QueryJobHistoryJsonAsync(string queryJson, CancellationToken cancellationToken)
        {
            this.EnsureInstanceNotClosed();

            var queryExpression = string.IsNullOrWhiteSpace(queryJson) ? new QueryExpression() : JsonConvert.DeserializeObject<QueryExpression>(queryJson);
            queryExpression.Validate();

            return this.httpClientHelper.PostAsync<QueryExpression, JobQueryResult>(
                new Uri(JobsQueryExpressionUriFormat, UriKind.Relative),
                queryExpression,
                null,
                null,
                cancellationToken);
        }
    }
}
