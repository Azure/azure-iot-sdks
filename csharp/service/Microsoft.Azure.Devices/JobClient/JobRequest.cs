// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices
{
    using Newtonsoft.Json;

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
        /// Job parameters required to execute job
        /// </summary>
        [JsonProperty(PropertyName = "jobParameters", Required = Required.Always)]
        public JobParameters JobParameters { get; set; }
    }
}
