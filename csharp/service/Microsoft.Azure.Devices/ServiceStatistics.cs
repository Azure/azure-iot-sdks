// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices
{
    using Newtonsoft.Json;

    public class ServiceStatistics
    {
        [JsonProperty(PropertyName = "connectedDeviceCount")]
        public long ConnectedDeviceCount { get; set; }
    }
}