// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices
{
    using Newtonsoft.Json;
    using Newtonsoft.Json.Converters;

    using System;

    /// <summary>
    /// Contains DeviceJob properties and their accessors.
    /// </summary>
    public class DeviceJob
    {
        /// <summary>
        /// Device ID
        /// </summary>
        [JsonProperty(PropertyName = "deviceId", NullValueHandling = NullValueHandling.Ignore)]
        public string DeviceId { get; set; }

        /// <summary>
        /// Job Id
        /// </summary>
        [JsonProperty(PropertyName = "jobId", NullValueHandling = NullValueHandling.Ignore)]
        public string JobId { get; set; }

        /// <summary>
        /// Device Job type
        /// </summary>
        [JsonProperty(PropertyName = "jobType", NullValueHandling = NullValueHandling.Ignore)]
        [JsonConverter(typeof(StringEnumConverter))]
        public DeviceJobType JobType { get; set; }

        /// <summary>
        /// Device Job Status
        /// </summary>
        [JsonProperty(PropertyName = "status", Required = Required.Always)]
        [JsonConverter(typeof(StringEnumConverter))]
        public DeviceJobStatus Status { get; set; }

        /// <summary>
        /// Device Job Start Time
        /// </summary>
        [JsonProperty(PropertyName = "startTimeUtc")]
        public DateTime StartTimeUtc { get; set; }

        /// <summary>
        /// Device Job End Time
        /// </summary>
        [JsonProperty(PropertyName = "endTimeUtc")]
        public DateTime EndTimeUtc { get; set; }

        /// <summary>
        /// Device Job Creation time
        /// </summary>
        [JsonProperty(PropertyName = "createdDateTimeUtc")]
        public DateTime CreatedDateTimeUtc { get; set; }

        /// <summary>
        /// Device Job last updated time
        /// </summary>
        [JsonProperty(PropertyName = "lastUpdatedDateTimeUtc")]
        public DateTime LastUpdatedDateTimeUtc { get; set; }

        /// <summary>
        /// Outcome for the device job continain job type specifics
        /// </summary>
        [JsonProperty(PropertyName = "outcome", NullValueHandling = NullValueHandling.Ignore)]
        public DeviceJobOutcome Outcome { get; set; }

        /// <summary>
        /// Device Job Error
        /// </summary>
        [JsonProperty(PropertyName = "error", NullValueHandling = NullValueHandling.Ignore)]
        public DeviceJobError Error { get; set; }
    }
}
