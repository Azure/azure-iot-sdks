// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices
{
    using Common;
    using Converters;
    using Newtonsoft.Json;

    /// <summary>
    /// Twin Representation
    /// </summary>
    [JsonConverter(typeof(TwinJsonConverter))]
    public class Twin : IETagHolder
    {
        /// <summary>
        /// Creates an instance of <see cref="Twin"/>
        /// </summary>
        public Twin()
        {
            this.Tags = new TwinCollection();
            this.Properties = new TwinProperties();
        }

        /// <summary>
        /// Creates an instance of <see cref="Twin"/>
        /// </summary>
        /// <param name="deviceId">Device Id</param>
        public Twin(string deviceId) : this()
        {
            this.DeviceId = deviceId;
        }

        /// <summary>
        /// Gets and sets the <see cref="Twin"/> Id.
        /// </summary>
        public string DeviceId { get; set; }

        /// <summary>
        /// Gets and sets the <see cref="Twin"/> tags.
        /// </summary>
        public TwinCollection Tags { get; set; }

        /// <summary>
        /// Gets and sets the <see cref="Twin"/> properties.
        /// </summary>
        public TwinProperties Properties { get; set; }

        /// <summary>
        /// Twin's ETag
        /// </summary>
        public string ETag { get; set; }

        /// <summary>
        /// Gets the Twin as a JSON string
        /// </summary>
        /// <param name="formatting">Optional. Formatting for the output JSON string.</param>
        /// <returns>JSON string</returns>
        public string ToJson(Formatting formatting = Formatting.None)
        {
            return JsonConvert.SerializeObject(this, formatting);
        }
    }
}