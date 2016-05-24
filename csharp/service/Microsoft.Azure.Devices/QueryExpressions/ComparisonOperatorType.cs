// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.QueryExpressions
{
    using System.Runtime.Serialization;
    using Newtonsoft.Json;
    using Newtonsoft.Json.Converters;

    [JsonConverter(typeof(StringEnumConverter))]
    internal enum ComparisonOperatorType
    {
        [EnumMember(Value = "eq")]
        Equals = 0,

        [EnumMember(Value = "ne")]
        NotEquals,

        [EnumMember(Value = "gt")]
        GreaterThan,

        [EnumMember(Value = "gte")]
        GreaterThanEquals,

        [EnumMember(Value = "lt")]
        LessThan,

        [EnumMember(Value = "lte")]
        LessThanEquals,

        [EnumMember(Value = "in")]
        In,

        [EnumMember(Value = "nin")]
        NotIn,

        [EnumMember(Value = "all")]
        All
    }
}
