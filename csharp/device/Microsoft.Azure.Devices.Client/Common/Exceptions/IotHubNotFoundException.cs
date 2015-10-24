// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client.Exceptions
{
    using System;
    using System.Runtime.Serialization;
    using Microsoft.Azure.Devices.Client.Extensions;

#if !WINDOWS_UWP
    [Serializable]
#endif
#if !WINDOWS_UWP // Exporting custom exception types is not allowed in WinRT
    public
#endif
    class IotHubNotFoundException : IotHubException
    {
        public IotHubNotFoundException(string iotHubName)
            : base("IoT hub not found: {0}".FormatInvariant(iotHubName))
        {
        }

        public IotHubNotFoundException(string iotHubName, string trackingId)
            : base("IoT hub not found: {0}".FormatInvariant(iotHubName), trackingId)
        {
        }

#if !WINDOWS_UWP
        IotHubNotFoundException(SerializationInfo info, StreamingContext context)
            : base(info, context)
        {
        }
#endif
    }
}
