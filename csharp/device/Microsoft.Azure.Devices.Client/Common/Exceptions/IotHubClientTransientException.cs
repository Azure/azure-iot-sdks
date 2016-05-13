// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client.Exceptions
{
    using System;
    using System.Runtime.Serialization;

    class IotHubClientTransientException : IotHubClientException
    {
        public IotHubClientTransientException(string message, string trackingId)
            : base(message, true, trackingId)
        {
        }

        public IotHubClientTransientException()
            : base("Transient exception occured. Please retry operation.", true)
        {
        }

        public IotHubClientTransientException(string message)
            : base(message, true)
        {
        }

        public IotHubClientTransientException(Exception innerException)
            : base(innerException)
        {
        }

        public IotHubClientTransientException(string message, Exception innerException)
            : base(message, innerException, true)
        {
        }

        public IotHubClientTransientException(string message, Exception innerException, string trackingId)
            : base(message, innerException, true, trackingId)
        {
        }
#if !PCL && !WINDOWS_UWP
        public IotHubClientTransientException(SerializationInfo info, StreamingContext context)
            : base(info, context)
        {
        }
#endif
    }
}