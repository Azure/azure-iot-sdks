// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices
{
    using System;
    using System.Collections.Generic;    
    using Newtonsoft.Json;
    using Microsoft.Azure.Devices.Common;

    /// <summary>
    /// Contains properties of a Job.
    /// </summary>
    public class JobProperties
    {
        public JobProperties()
        {
            this.JobId = string.Empty;
        }

        /// <summary>
        /// System generated.  Ignored at creation.
        /// </summary>
        [JsonProperty(PropertyName = "jobId", NullValueHandling = NullValueHandling.Ignore)]
        public string JobId { get; set; }

        /// <summary>
        /// System generated.  Ignored at creation.
        /// </summary>
        [JsonProperty(PropertyName = "startTimeUtc", NullValueHandling = NullValueHandling.Ignore)]
        public DateTime? StartTimeUtc { get; set; }

        /// <summary>
        /// System generated.  Ignored at creation.
        /// Represents the time the job stopped processing.
        /// </summary>
        [JsonProperty(PropertyName = "endTimeUtc", NullValueHandling = NullValueHandling.Ignore)]
        public DateTime? EndTimeUtc { get; set; }

        /// <summary>
        /// Required.
        /// The type of job to execute.
        /// </summary>
        [JsonProperty(PropertyName = "type", Required = Required.Always)]
        public JobType Type { get; set; }

        /// <summary>
        /// System generated.  Ignored at creation.
        /// </summary>
        [JsonProperty(PropertyName = "status", NullValueHandling = NullValueHandling.Ignore)]
        public JobStatus Status { get; set; }

        /// <summary>
        /// System generated.  Ignored at creation.
        /// Represents the percentage of completion.
        /// </summary>
        [JsonProperty(PropertyName = "progress", NullValueHandling = NullValueHandling.Ignore)]
        public int Progress { get; set; }

        /// <summary>
        /// URI containing SAS token to a blob container that contains registry data to sync.
        /// </summary>
        [JsonProperty(PropertyName = "inputBlobContainerUri", NullValueHandling = NullValueHandling.Ignore)]
        public string InputBlobContainerUri { get; set; }

        /// <summary>
        /// URI containing SAS token to a blob container.  This is used to output the status of the job and the results.
        /// </summary>
        [JsonProperty(PropertyName = "outputBlobContainerUri", Required = Required.Always)]
        public string OutputBlobContainerUri { get; set; }

        /// <summary>
        /// Optional for export jobs; ignored for other jobs.  Default: false.  If false, authorization keys are included
        /// in export output.  Keys are exported as null otherwise.
        /// </summary>
        [JsonProperty(PropertyName = "excludeKeysInExport", NullValueHandling = NullValueHandling.Ignore)]
        public bool ExcludeKeysInExport { get; set; }

        /// <summary>
        /// System genereated.  Ignored at creation.
        /// If status == failure, this represents a string containing the reason.
        /// </summary>
        [JsonProperty(PropertyName = "failureReason", NullValueHandling = NullValueHandling.Ignore)]
        public string FailureReason { get; set; }
    }
}
