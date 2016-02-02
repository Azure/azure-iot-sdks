// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices
{
    using System;
    using Newtonsoft.Json;

    /// <summary>
    /// Contains properties of an individual feedback record. 
    /// </summary>
    public class FeedbackRecord
    {
        [JsonProperty(PropertyName = "originalMessageId", DefaultValueHandling = DefaultValueHandling.Ignore)]
        public string OriginalMessageId { get; set; }

        [JsonProperty(PropertyName = "description", DefaultValueHandling = DefaultValueHandling.Ignore)]
        public string Description { get; set; }

        [JsonProperty(PropertyName = "deviceGenerationId", DefaultValueHandling = DefaultValueHandling.Ignore)]
        public string DeviceGenerationId { get; set; }

        [JsonProperty(PropertyName = "deviceId", DefaultValueHandling = DefaultValueHandling.Ignore)]
        public string DeviceId { get; set; }

        [JsonProperty(PropertyName = "enqueuedTimeUtc", DefaultValueHandling = DefaultValueHandling.Ignore)]
        public DateTime EnqueuedTimeUtc { get; set; }

        [JsonProperty(PropertyName = "statusCode", DefaultValueHandling = DefaultValueHandling.Ignore)]
        public FeedbackStatusCode StatusCode { get; set; }
    }
}
