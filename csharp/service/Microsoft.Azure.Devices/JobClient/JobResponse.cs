// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices
{
    using System;
    using Newtonsoft.Json;
    using Newtonsoft.Json.Converters;

    /// <summary>
    /// Provides current job report when fetched
    /// </summary>
    public class JobResponse
    {
        /// <summary>
        /// System generated.  Ignored at creation.
        /// </summary>
        [JsonProperty(PropertyName = "jobId", NullValueHandling = NullValueHandling.Ignore)]
        public string JobId { get; internal set; }

        /// <summary>
        /// Device query condition.
        /// </summary>
        [JsonProperty(PropertyName = "queryCondition", NullValueHandling = NullValueHandling.Ignore)]
        public string QueryCondition { get; internal set; }

        /// <summary>
        /// Scheduled job start time in UTC.
        /// </summary>
        [JsonProperty(PropertyName = "createdTime", NullValueHandling = NullValueHandling.Ignore)]
        public DateTime? CreatedTimeUtc { get; internal set; }

        /// <summary>
        /// System generated.  Ignored at creation.
        /// </summary>
        [JsonProperty(PropertyName = "startTimeUtc", NullValueHandling = NullValueHandling.Ignore)]
        public DateTime? StartTimeUtc { get; internal set; }

        /// <summary>
        /// System generated.  Ignored at creation.
        /// Represents the time the job stopped processing.
        /// </summary>
        [JsonProperty(PropertyName = "endTimeUtc", NullValueHandling = NullValueHandling.Ignore)]
        public DateTime? EndTimeUtc { get; internal set; }

        /// <summary>
        /// Max execution time in secounds
        /// </summary>
        [JsonProperty(PropertyName = "maxExecutionTimeInSeconds", DefaultValueHandling = DefaultValueHandling.IgnoreAndPopulate)]
        public long MaxExecutionTimeInSeconds { get; set; }

        /// <summary>
        /// Required.
        /// The type of job to execute.
        /// </summary>
        [JsonProperty(PropertyName = "type", Required = Required.Always)]
        [JsonConverter(typeof(StringEnumConverter))]
        public JobType Type { get; internal set; }

        /// <summary>
        /// System generated.  Ignored at creation.
        /// </summary>
        [JsonProperty(PropertyName = "status", NullValueHandling = NullValueHandling.Ignore)]
        [JsonConverter(typeof(StringEnumConverter))]
        public JobStatus Status { get; internal set; }

        /// <summary>
        /// Required if jobType is cloudToDeviceMethod.
        /// The method type and parameters.
        /// </summary>
        [JsonProperty(PropertyName = "cloudToDeviceMethod", NullValueHandling = NullValueHandling.Ignore)]
        public CloudToDeviceMethod CloudToDeviceMethod { get; set; }

        /// <summary>
        /// Required if jobType is updateTwin.
        /// The Update Twin tags and desired properties.
        /// </summary>
        [JsonProperty(PropertyName = "updateTwin", NullValueHandling = NullValueHandling.Ignore)]
        public Twin UpdateTwin { get; set; }

        /// <summary>
        /// System generated.  Ignored at creation.
        /// If status == failure, this represents a string containing the reason.
        /// </summary>
        [JsonProperty(PropertyName = "failureReason", NullValueHandling = NullValueHandling.Ignore)]
        public string FailureReason { get; set; }

        /// <summary>
        /// System generated.  Ignored at creation.
        /// Represents a string containing a message with status about the job execution.
        /// </summary>
        [JsonProperty(PropertyName = "statusMessage", NullValueHandling = NullValueHandling.Ignore)]
        public string StatusMessage { get; set; }

        /// <summary>
        /// Different number of devices in the job
        /// </summary>
        [JsonProperty(PropertyName = "deviceJobStatistics", NullValueHandling = NullValueHandling.Ignore)]
        public DeviceJobStatistics DeviceJobStatistics { get; set; }

        /// <summary>
        /// The deviceId related to this response.
        /// It can be null (e.g. in case of a parent orchestration).
        /// </summary>
        [JsonProperty(PropertyName = "deviceId", NullValueHandling = NullValueHandling.Ignore)]
        public string DeviceId { get; set; }

        /// <summary>
        /// The jobId of the parent orchestration, if any.
        /// </summary>
        [JsonProperty(PropertyName = "parentJobId", NullValueHandling = NullValueHandling.Ignore)]
        public string ParentJobId { get; set; }
    }

    /// <summary>
    /// The job counts, e.g., number of failed/succeeded devices
    /// </summary>
    public class DeviceJobStatistics
    {
        /// <summary>
        /// Number of devices in the job
        /// </summary>
        [JsonProperty(PropertyName = "deviceCount")]
        public int DeviceCount { get; set; }

        /// <summary>
        /// The number of failed jobs
        /// </summary>
        [JsonProperty(PropertyName = "failedCount")]
        public int FailedCount { get; set; }

        /// <summary>
        /// The number of Successed jobs
        /// </summary>
        [JsonProperty(PropertyName = "succeededCount")]
        public int SucceededCount { get; set; }

        /// <summary>
        /// The number of running jobs
        /// </summary>
        [JsonProperty(PropertyName = "runningCount")]
        public int RunningCount { get; set; }

        /// <summary>
        /// The number of pending (scheduled) jobs
        /// </summary>
        [JsonProperty(PropertyName = "pendingCount")]
        public int PendingCount { get; set; }
    }
}
