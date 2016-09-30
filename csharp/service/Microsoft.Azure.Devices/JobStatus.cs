// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices
{
    using System.Runtime.Serialization;
    using Newtonsoft.Json;
    using Newtonsoft.Json.Converters;

    /// <summary>
    /// Specifies the various job status for a job.
    /// </summary>
    [JsonConverter(typeof(StringEnumConverter))]
    public enum JobStatus
    {
        /// <summary>
        /// Unknown
        /// </summary>
        [EnumMember(Value = "unknown")]
        Unknown,

        /// <summary>
        /// Indicates that a Job is in the queue for execution
        /// </summary>
        [EnumMember(Value = "enqueued")]
        Enqueued,

        /// <summary>
        /// Indicates that a Job is running
        /// </summary>
        [EnumMember(Value = "running")]
        Running,

        /// <summary>
        /// Indicates that a Job execution is completed
        /// </summary>
        [EnumMember(Value = "completed")]
        Completed,

        /// <summary>
        /// Indicates that a Job execution failed
        /// </summary>
        [EnumMember(Value = "failed")]
        Failed,

        /// <summary>
        /// Indicates that a Job execution was cancelled
        /// </summary>
        [EnumMember(Value = "cancelled")]
        Cancelled,

        /// <summary>
        /// Indicates that a Job is scheduled for a future datetime
        /// </summary>
        [EnumMember(Value = "scheduled")]
        Scheduled,

        /// <summary>
        /// Indicates that a Job is in the queue for execution (synonym for enqueued to be depricated)
        /// </summary>
        [EnumMember(Value = "queued")]
        Queued
    }
}
