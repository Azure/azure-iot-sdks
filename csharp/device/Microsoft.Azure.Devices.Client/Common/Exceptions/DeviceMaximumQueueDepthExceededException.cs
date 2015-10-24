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
    sealed class DeviceMaximumQueueDepthExceededException : IotHubException
    {
        public DeviceMaximumQueueDepthExceededException(int maximumQueueDepth)
            : base("Device Queue depth cannot exceed {0} messages".FormatInvariant(maximumQueueDepth))
        {
            this.MaximumQueueDepth = maximumQueueDepth;
        }

        public DeviceMaximumQueueDepthExceededException(string message)
            : base(message)
        {
        }

        public DeviceMaximumQueueDepthExceededException(string message, Exception innerException)
            : base(message, innerException)
        {
        }
#if !WINDOWS_UWP
        DeviceMaximumQueueDepthExceededException(SerializationInfo info, StreamingContext context)
            : base(info, context)
        {
        }
#endif
        internal int MaximumQueueDepth
        {
            get;
            private set;
        }
    }
}
