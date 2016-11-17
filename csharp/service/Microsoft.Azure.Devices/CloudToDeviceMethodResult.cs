// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices
{
    using Newtonsoft.Json;
    using Newtonsoft.Json.Linq;

    /// <summary>
    /// Represents the Device Method Invocation Results.
    /// </summary>
    public class CloudToDeviceMethodResult
    {
        /// <summary>
        /// Gets or sets the status of device method invocation.
        /// </summary>
        [JsonProperty("status")]
        public int Status { get; set; }

        /// <summary>
        /// Get payload as json
        /// </summary>
        public string GetPayloadAsJson()
        {
            return (string)this.Payload;
        }

        [JsonProperty("payload")]
        internal JRaw Payload { get; set; }
    }
}