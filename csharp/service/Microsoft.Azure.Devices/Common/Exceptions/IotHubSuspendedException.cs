// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Common.Exceptions
{
    using System;
    using System.Runtime.Serialization;

    [Serializable]
    public class IotHubSuspendedException : IotHubException
    {
        public IotHubSuspendedException(string iotHubName)
            : base(Resources.IotHubSuspendedException.FormatInvariant(iotHubName))
        {
        }

        public IotHubSuspendedException(string iotHubName, string trackingId)
            : base(Resources.IotHubSuspendedException.FormatInvariant(iotHubName), trackingId)
        {
        }

        public IotHubSuspendedException(SerializationInfo info, StreamingContext context)
            : base(info, context)
        {
        }
    }
}
