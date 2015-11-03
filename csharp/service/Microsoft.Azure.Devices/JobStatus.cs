// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices
{
    using System.Runtime.Serialization;
    using Newtonsoft.Json;
    using Newtonsoft.Json.Converters;

    [JsonConverter(typeof(StringEnumConverter))]
    public enum JobStatus
    {
        [EnumMember(Value = "unknown")]
        Unknown,

        [EnumMember(Value = "enqueued")]
        Enqueued,

        [EnumMember(Value = "running")]
        Running,

        [EnumMember(Value = "completed")]
        Completed,

        [EnumMember(Value = "failed")]
        Failed,

        [EnumMember(Value = "cancelled")]
        Cancelled
    }
}
