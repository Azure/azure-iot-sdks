// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices
{
    using Newtonsoft.Json;

    /// <summary>
    /// Contains device registry statistics that can be retrieved from IotHub.
    /// </summary>
    public class RegistryStatistics
    {
        [JsonProperty(PropertyName = "totalDeviceCount")]
        public long TotalDeviceCount { get; set; }

        [JsonProperty(PropertyName = "enabledDeviceCount")]
        public long EnabledDeviceCount { get; set; }

        [JsonProperty(PropertyName = "disabledDeviceCount")]
        public long DisabledDeviceCount { get; set; }
    }
}