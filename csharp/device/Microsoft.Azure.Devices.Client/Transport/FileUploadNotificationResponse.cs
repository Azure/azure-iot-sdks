// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information

namespace Microsoft.Azure.Devices.Client.Transport
{
    using Newtonsoft.Json;

    class FileUploadNotificationResponse
    {
        [JsonProperty(PropertyName = "correlationId")]
        public string CorrelationId { get; set; }

        [JsonProperty(PropertyName = "isSuccess")]
        public bool IsSuccess { get; set; }

        [JsonProperty(PropertyName = "statusCode")]
        public int StatusCode { get; set; }

        [JsonProperty(PropertyName = "statusDescription")]
        public string StatusDescription { get; set; }
    }
}
