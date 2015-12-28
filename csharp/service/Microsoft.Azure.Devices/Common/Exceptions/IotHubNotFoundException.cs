// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Common.Exceptions
{
    using System;
    using System.Runtime.Serialization;

    [Serializable]
    public class IotHubNotFoundException : IotHubException
    {
        public IotHubNotFoundException(string iotHubName)
            : base("IoT hub not found: {0}".FormatInvariant(iotHubName))
        {
        }

        public IotHubNotFoundException(string iotHubName, string trackingId)
            : base("IoT hub not found: {0}".FormatInvariant(iotHubName), trackingId)
        {
        }

        IotHubNotFoundException(SerializationInfo info, StreamingContext context)
            : base(info, context)
        {
        }
    }
}
