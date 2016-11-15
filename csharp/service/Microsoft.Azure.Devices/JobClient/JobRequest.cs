// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices
{
    using System;
    using System.Collections.Generic;
    using Newtonsoft.Json;
    using Newtonsoft.Json.Converters;

    /// <summary>
    /// Job input
    /// </summary>
    public class JobRequest
    {
        /// <summary>
        /// Job identifier
        /// </summary>
        [JsonProperty(PropertyName = "jobId", Required = Required.Always)]
        public string JobId { get; set; }

        /// <summary>
        /// Required.
        /// The type of job to execute.
        /// </summary>
        [JsonProperty(PropertyName = "type", Required = Required.Always)]
        public JobType JobType { get; set; }

        /// <summary>
        /// Required if jobType is cloudToDeviceMethod.
        /// The method type and parameters.
        /// </summary>
        [JsonProperty(PropertyName = "cloudToDeviceMethod")]
        public CloudToDeviceMethod CloudToDeviceMethod { get; set; }

        /// <summary>
        /// Required if jobType is updateTwin.
        /// The Update Twin tags and desired properties.
        /// </summary>
        [JsonProperty(PropertyName = "updateTwin")]
        public Twin UpdateTwin { get; set; }

        /// <summary>
        /// Required if jobType is updateTwin or cloudToDeviceMethod.
        /// Condition for device query to get devices to execute the job on
        /// </summary>
        [JsonProperty(PropertyName = "queryCondition")]
        public string QueryCondition { get; set; }


        /// <summary>
        /// ISO 8601 date time to start the job
        /// </summary>
        [JsonProperty(PropertyName = "startTime")]
        [JsonConverter(typeof(IsoDateTimeConverter))]
        // TODO: siport : validate if we want utc on the internal propname but not on the external
        public DateTime StartTime { get; set; }

        /// <summary>
        /// Max execution time in seconds (ttl duration)
        /// </summary>
        [JsonProperty(PropertyName = "maxExecutionTimeInSeconds", DefaultValueHandling = DefaultValueHandling.IgnoreAndPopulate)]
        public long MaxExecutionTimeInSeconds { get; set; }
    }
}
