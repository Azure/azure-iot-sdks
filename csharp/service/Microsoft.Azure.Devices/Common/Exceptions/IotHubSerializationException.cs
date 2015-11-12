// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

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
