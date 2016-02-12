// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices
{
    using System.Runtime.Serialization;
    using Newtonsoft.Json;
    using Newtonsoft.Json.Converters;

    /// <summary>
    /// Specifies the different states of a device.
    /// </summary>
    [JsonConverter(typeof(StringEnumConverter))]
    public enum DeviceStatus
    {
        [EnumMember(Value = "enabled")]
        Enabled = 0,

        [EnumMember(Value = "disabled")]
        Disabled,
    }
}