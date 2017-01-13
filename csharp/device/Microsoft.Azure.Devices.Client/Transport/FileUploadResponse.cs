// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information

namespace Microsoft.Azure.Devices.Client.Transport
{
    using Newtonsoft.Json;

    class FileUploadResponse
    {
        [JsonProperty(PropertyName = "correlationId")]
        public string CorrelationId { get; set; }

        [JsonProperty(PropertyName = "hostName")]
        public string HostName { get; set; }

        [JsonProperty(PropertyName = "containerName")]
        public string ContainerName { get; set; }

        [JsonProperty(PropertyName = "blobName")]
        public string BlobName { get; set; }

        [JsonProperty(PropertyName = "sasToken")]
        public string SasToken { get; set; }
    }
}
