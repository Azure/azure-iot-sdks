// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices
{
    using System;
    using Newtonsoft.Json;

    /// <summary>
    /// Contains properties for file upload notifications 
    /// </summary>
    public class FileNotification
    {
        [JsonProperty(PropertyName = "deviceId", DefaultValueHandling = DefaultValueHandling.Ignore)]
        public string DeviceId { get; set; }

        [JsonProperty(PropertyName = "blobUri", DefaultValueHandling = DefaultValueHandling.Ignore)]
        public string BlobUri { get; set; }

        [JsonProperty(PropertyName = "blobName", DefaultValueHandling = DefaultValueHandling.Ignore)]
        public string BlobName { get; set; }

        [JsonProperty(PropertyName = "lastUpdatedTime", DefaultValueHandling = DefaultValueHandling.Ignore)]
        public DateTimeOffset? LastUpdatedTime { get; set; }

        [JsonProperty(PropertyName = "blobSizeInBytes", DefaultValueHandling = DefaultValueHandling.Ignore)]
        public long BlobSizeInBytes { get; set; }

        [JsonProperty(PropertyName = "enqueuedTimeUtc", DefaultValueHandling = DefaultValueHandling.Ignore)]
        public DateTime EnqueuedTimeUtc { get; set; }

        [JsonIgnore]
        internal string LockToken { get; set; }
    }
}
