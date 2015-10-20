// ---------------------------------------------------------------
// Copyright (c) Microsoft Corporation. All rights reserved.
// ---------------------------------------------------------------

namespace Microsoft.Azure.Devices.Common.Exceptions
{
    using System;
    using System.Runtime.Serialization;

    [Serializable]
    public class IotHubSerializationException : IotHubException
    {
        public IotHubSerializationException(string message)
            : base(message)
        {
        }

        protected IotHubSerializationException(SerializationInfo info, StreamingContext context)
            : base(info, context)
        {
        }
    }
}
