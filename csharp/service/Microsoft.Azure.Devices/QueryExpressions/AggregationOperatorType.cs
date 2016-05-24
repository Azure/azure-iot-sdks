// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.QueryExpressions
{
    using System.Runtime.Serialization;
    using Newtonsoft.Json;
    using Newtonsoft.Json.Converters;

    [JsonConverter(typeof(StringEnumConverter))]
    internal enum AggregationOperatorType
    {
        [EnumMember(Value = "sum")]
        Sum = 0,

        [EnumMember(Value = "avg")]
        Average,

        [EnumMember(Value = "count")]
        Count,

        [EnumMember(Value = "min")]
        Min,

        [EnumMember(Value = "max")]
        Max
    }
}
