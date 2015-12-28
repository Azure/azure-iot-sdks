// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Common.WebApi
{
    using System.Collections.Generic;
    using System.Runtime.Serialization;
    using Newtonsoft.Json;

    public class ResourceRequest
    {
        [DataMember]
        [JsonProperty("name")]
        public string Name { get; set; }

        [DataMember]
        [JsonProperty("type")]
        public string Type { get; set; }

        /// <summary>
        /// TODO: hard code locations in enum
        /// </summary>
        [DataMember]
        [JsonProperty("location")]
        public string Location { get; set; }

        /// <summary>
        /// TODO: No more than 15 tags, max length per key is 512 chars
        /// </summary>
        [DataMember]
        [JsonProperty("tags")]
        public IDictionary<string, string> Tags { get; set; }

        [DataMember]
        [JsonProperty("properties")]
        public IDictionary<string, object> Properties { get; set; }
    }
}
