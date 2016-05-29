// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.Serialization;

namespace Microsoft.Azure.Devices.Client.Transport
{
    [DataContract]
    class FileUploadGetResponse
    {
        [DataMember]
        public string correlationId { get; set; }

        [DataMember]
        public string hostName { get; set; }

        [DataMember]
        public string containerName { get; set; }

        [DataMember]
        public string blobName { get; set; }

        [DataMember]
        public string sasToken { get; set; }
    }
}
