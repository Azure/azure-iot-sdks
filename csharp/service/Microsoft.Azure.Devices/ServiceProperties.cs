// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices
{
    using System.Collections.Generic;
    using Microsoft.Azure.Devices.Common;
    using Newtonsoft.Json;

    /// <summary>
    ///     Properties that are managed by the Service.
    ///     The Contents cannot be changed once created
    /// </summary>
    public sealed class ServiceProperties : IETagHolder
    {
        /// <summary>
        /// Initializes a new instance of <see cref="ServiceProperties"/>
        /// </summary>
        public ServiceProperties()
        {
            Properties = new Dictionary<string, object>();
            Tags = new List<string>();
        }

        /// <summary>
        /// Gets or sets the ETag
        /// </summary>
        [JsonProperty(PropertyName = "etag")]
        public string ETag
        {
            get; set;
        }

        /// <summary>
        ///     Custom list of Properties.
        /// </summary>
        [JsonProperty(PropertyName = "properties")]
        public Dictionary<string, object> Properties { get; set; }

        /// <summary>
        ///     Tags for the Device
        /// </summary>
        [JsonProperty(PropertyName = "tags")]
        public List<string> Tags { get; set; }
    }
}
