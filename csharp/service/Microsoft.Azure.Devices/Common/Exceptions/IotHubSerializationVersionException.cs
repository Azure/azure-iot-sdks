// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Common.Exceptions
{
    using System;
    using System.Runtime.Serialization;

    [Serializable]
    public class IotHubSerializationVersionException : IotHubSerializationException
    {
        public IotHubSerializationVersionException(int receivedVersion)
            : base("Unrecognized Serialization Version: {0}".FormatInvariant(receivedVersion))
        {
        }

        IotHubSerializationVersionException(SerializationInfo info, StreamingContext context)
            : base(info, context)
        {
        }
    }
}
