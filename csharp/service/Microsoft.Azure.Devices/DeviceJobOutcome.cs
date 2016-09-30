// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

using Newtonsoft.Json;

namespace Microsoft.Azure.Devices
{
    /// <summary>
    /// Contains the DeviceJob outcome
    /// </summary>    
    public class DeviceJobOutcome
    {
        /// <summary>
        /// Outcome for a device method job
        /// </summary>
        [JsonProperty(PropertyName = "deviceMethodResponse")]
        public CloudToDeviceMethodResult DeviceMethodResponse { get; set; }
    }
}
