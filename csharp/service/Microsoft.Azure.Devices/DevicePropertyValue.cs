// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices
{
    using System;
    using Newtonsoft.Json;

    /// <summary>
    /// Provides the value for a property
    /// </summary>
    public class DevicePropertyValue
    {
        /// <summary>
        /// Last time the property was updated by the device
        /// </summary>
        [JsonProperty(PropertyName ="lastUpdatedTime")]
        public DateTime LastUpdatedTime { get; internal set; }

        /// <summary>
        /// Value of the property. The object can tell the type of the object to cast to
        /// </summary>
        [JsonProperty(PropertyName = "value")]
        public object Value { get; internal set; }
    }
}
