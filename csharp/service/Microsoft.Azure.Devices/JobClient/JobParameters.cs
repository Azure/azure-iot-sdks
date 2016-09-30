// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

using System.Collections.Generic;

namespace Microsoft.Azure.Devices
{
    using Newtonsoft.Json;

    /// <summary>
    /// Input specific to a job used in JobRequest
    /// </summary>
    public class JobParameters
    {
        public JobParameters(JobType jobType)
        {
            this.JobType = jobType;
        }

        /// <summary>
        /// Required.
        /// The type of job to execute.
        /// </summary>
        [JsonProperty(PropertyName = "jobType", Required = Required.Always)]
        public JobType JobType { get; private set; }
    }
}
