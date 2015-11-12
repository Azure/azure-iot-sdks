// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Common.Exceptions
{
    using System;
    using System.Runtime.Serialization;

    [Serializable]
    public sealed class IotHubThrottledException : IotHubException
    {
        public IotHubThrottledException(int maximumBatchCount)
            : base("Device Container has exceeded maximum number of allowed active requests: {0}".FormatInvariant(maximumBatchCount))
        {
        }

        public IotHubThrottledException(string message, Exception innerException)
            : base(message, innerException)
        {
        }

        IotHubThrottledException(SerializationInfo info, StreamingContext context)
            : base(info, context)
        {
        }
    }
}
