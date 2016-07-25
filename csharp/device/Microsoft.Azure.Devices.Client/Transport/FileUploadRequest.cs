// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information

namespace Microsoft.Azure.Devices.Client.Transport
{
    using Newtonsoft.Json;

    class FileUploadRequest
    {
        [JsonProperty(PropertyName = "blobName")]
        public string BlobName { get; set; }
    }
}