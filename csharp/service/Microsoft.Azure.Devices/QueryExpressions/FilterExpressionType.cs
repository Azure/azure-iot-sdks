// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.QueryExpressions
{
    using System.Runtime.Serialization;
    using Newtonsoft.Json;
    using Newtonsoft.Json.Converters;

    [JsonConverter(typeof(StringEnumConverter))]
    internal enum FilterExpressionType
    {
        [EnumMember(Value = "comparison")]
        Comparison = 0,

        [EnumMember(Value = "logical")]
        Logical
    }
}
