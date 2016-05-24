// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices
{
    using System;
    using Newtonsoft.Json;

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
        /// Required.
        /// The type of job to execute.
        /// </summary>
        [JsonProperty(PropertyName = "type", Required = Required.Always)]
        public JobType Type { get; internal set; }

        /// <summary>
        /// System generated.  Ignored at creation.
        /// </summary>
        [JsonProperty(PropertyName = "status", NullValueHandling = NullValueHandling.Ignore)]
        public JobStatus Status { get; internal set; }

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
}
