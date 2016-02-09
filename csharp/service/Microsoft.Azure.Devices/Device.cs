// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices
{
    using System; 
    using Newtonsoft.Json;
    using Newtonsoft.Json.Converters;
    using Microsoft.Azure.Devices.Common;

    /// <summary>
    /// contains device properties and their accessors.
    /// </summary>
    public class Device : IETagHolder
    {
        // only for deserialization
        public Device()
        {
        }

        public Device(string id)
        {
            this.Id = id;
            this.ConnectionState = DeviceConnectionState.Disconnected;
            this.LastActivityTime = this.StatusUpdatedTime = this.ConnectionStateUpdatedTime = DateTime.MinValue;
        }

        [JsonProperty(PropertyName = "deviceId")]
        public string Id { get; internal set; }

        [JsonProperty(PropertyName = "generationId")]
        public string GenerationId { get; internal set; }

        [JsonProperty(PropertyName = "etag")]
        public string ETag
        {
            get;
            set;
        }

        [JsonProperty(PropertyName = "connectionState")]
        [JsonConverter(typeof(StringEnumConverter))]
        public DeviceConnectionState ConnectionState { get; internal set; }

        [JsonProperty(PropertyName = "status")]
        [JsonConverter(typeof(StringEnumConverter))]
        public DeviceStatus Status { get; set; }

        [JsonProperty(PropertyName = "statusReason")]
        public string StatusReason { get; set; }

        [JsonProperty(PropertyName = "connectionStateUpdatedTime")]
        public DateTime ConnectionStateUpdatedTime { get; internal set; }

        [JsonProperty(PropertyName = "statusUpdatedTime")]
        public DateTime StatusUpdatedTime { get; internal set; }

        [JsonProperty(PropertyName = "lastActivityTime")]
        public DateTime LastActivityTime { get; internal set; }

        [JsonProperty(PropertyName = "cloudToDeviceMessageCount")]
        public int CloudToDeviceMessageCount { get; internal set; }

        [JsonProperty(PropertyName = "authentication")]
        public AuthenticationMechanism Authentication { get; set; }
    }
}