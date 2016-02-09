// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client.Exceptions
{
    using System;
    using System.Runtime.Serialization;

    class IotHubClientException : IotHubException
    {
        public IotHubClientException(string message)
            : base(message)
        {
        }

        public IotHubClientException(string message, string trackingId)
            : base(message, trackingId)
        {
        }

        public IotHubClientException(string message, bool isTransient, string trackingId)
            : base(message, isTransient, trackingId)
        {
        }

        public IotHubClientException(string message, bool isTransient)
            : base(message, isTransient)
        {
        }

        public IotHubClientException(Exception innerException)
            : base(innerException)
        {
        }

        public IotHubClientException(string message, Exception innerException)
            : base(message, innerException)
        {
        }

        public IotHubClientException(string message, Exception innerException, bool isTransient)
            : base(message, innerException, isTransient)
        {
        }

        public IotHubClientException(string message, Exception innerException, bool isTransient, string trackingId)
            : base(message, innerException, isTransient, trackingId)
        {
        }

        public IotHubClientException(SerializationInfo info, StreamingContext context)
            : base(info, context)
        {
        }
    }
}