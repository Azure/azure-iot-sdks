// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.QueryExpressions
{
    using System.Runtime.Serialization;
    using Newtonsoft.Json;
    using Newtonsoft.Json.Converters;

    [JsonConverter(typeof(StringEnumConverter))]
    internal enum PropertyType
    {
        [EnumMember(Value = "default")]
        Default,

        [EnumMember(Value = "device")]
        DeviceProperty,

        [EnumMember(Value = "custom")]
        CustomProperty,

        [EnumMember(Value = "service")]
        ServiceProperty,

        [EnumMember(Value = "aggregated")]
        AggregatedProperty
    }
}
