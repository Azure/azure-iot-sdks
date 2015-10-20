// ---------------------------------------------------------------
// Copyright (c) Microsoft Corporation. All rights reserved.
// ---------------------------------------------------------------

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
