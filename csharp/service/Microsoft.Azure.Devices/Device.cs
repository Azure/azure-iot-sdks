// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices
{
    using System;
    using System.Collections.Generic;
    using Microsoft.Azure.Devices.Common;
    using Newtonsoft.Json;
    using Newtonsoft.Json.Converters;

    /// <summary>
    /// Contains Device properties and their accessors.
    /// </summary>
    public class Device : IETagHolder
    {
        /// <summary>
        /// Creates a new instance of <see cref="Device"/>
        /// </summary>
        public Device()
        {
        }

        /// <summary>
        /// Creates a new instance of <see cref="Device"/>
        /// </summary>
        /// <param name="id">Device ID</param>
        public Device(string id)
        {
            this.Id = id;
            this.ConnectionState = DeviceConnectionState.Disconnected;
            this.LastActivityTime = this.StatusUpdatedTime = this.ConnectionStateUpdatedTime = DateTime.MinValue;
            this.ServiceProperties = new ServiceProperties();
        }

        /// <summary>
        /// Device ID
        /// </summary>
        [JsonProperty(PropertyName = "deviceId")]
        public string Id { get; internal set; }

        /// <summary>
        /// Device's Generation ID
        /// </summary>
        [JsonProperty(PropertyName = "generationId")]
        public string GenerationId { get; internal set; }

        /// <summary>
        /// Device's ETag
        /// </summary>
        [JsonProperty(PropertyName = "etag")]
        public string ETag
        {
            get;
            set;
        }

        /// <summary>
        /// Device's ConnectionState
        /// </summary>
        [JsonProperty(PropertyName = "connectionState")]
        [JsonConverter(typeof(StringEnumConverter))]
        public DeviceConnectionState ConnectionState { get; internal set; }

        /// <summary>
        /// Device's Status
        /// </summary>
        [JsonProperty(PropertyName = "status")]
        [JsonConverter(typeof(StringEnumConverter))]
        public DeviceStatus Status { get; set; }

        /// <summary>
        /// Reason, if any, for the Device to be in specified <see cref="Status"/>
        /// </summary>
        [JsonProperty(PropertyName = "statusReason")]
        public string StatusReason { get; set; }

        /// <summary>
        /// Time when the <see cref="ConnectionState"/> was last updated
        /// </summary>
        [JsonProperty(PropertyName = "connectionStateUpdatedTime")]
        public DateTime ConnectionStateUpdatedTime { get; internal set; }

        /// <summary>
        /// Time when the <see cref="Status"/> was last updated
        /// </summary>
        [JsonProperty(PropertyName = "statusUpdatedTime")]
        public DateTime StatusUpdatedTime { get; internal set; }

        /// <summary>
        /// Time when the <see cref="Device"/> was last active
        /// </summary>
        [JsonProperty(PropertyName = "lastActivityTime")]
        public DateTime LastActivityTime { get; internal set; }

        /// <summary>
        /// Number of messages sent to the Device from the Cloud
        /// </summary>
        [JsonProperty(PropertyName = "cloudToDeviceMessageCount")]
        public int CloudToDeviceMessageCount { get; internal set; }

        /// <summary>
        /// Device's authentication mechanism
        /// </summary>
        [JsonProperty(PropertyName = "authentication")]
        public AuthenticationMechanism Authentication { get; set; }

        /// <summary>
        /// Specifies whether this is a Managed Device
        /// </summary>
        [JsonProperty(PropertyName = "isManaged")]
        public bool IsManaged { get; internal set; }

        /// <summary>
        ///     Device Properties that are defined in the System Model. These are common for all the devices.
        /// </summary>
        [JsonProperty(PropertyName = "deviceProperties")]
        public IReadOnlyDictionary<string, DevicePropertyValue> DeviceProperties { get; internal set; }

        /// <summary>
        ///    Configuration value that is sent to the device
        /// </summary>
        [JsonProperty(PropertyName = "configuration")]
        public string Configuration { get; internal set; }

        /// <summary>
        ///     Properties that are mastered by the Service
        /// </summary>
        [JsonProperty(PropertyName = "serviceProperties")]
        public ServiceProperties ServiceProperties { get; internal set; }
    }
}