// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client.Exceptions
{
    using System;
    using System.Runtime.Serialization;
    using Microsoft.Azure.Devices.Client.Extensions;

#if !WINDOWS_UWP && !PCL // Exporting custom exception types is not allowed in WinRT
    [Serializable]
    public
#endif
    class IotHubSuspendedException : IotHubException
    {
        public IotHubSuspendedException(string iotHubName)
            : base("Iothub {0} is suspended".FormatInvariant(iotHubName))
        {
        }

        public IotHubSuspendedException(string iotHubName, string trackingId)
            : base("Iothub {0} is suspended".FormatInvariant(iotHubName), trackingId)
        {
        }

#if !WINDOWS_UWP && !PCL
        IotHubSuspendedException(SerializationInfo info, StreamingContext context)
            : base(info, context)
        {
        }
#endif
    }
}
