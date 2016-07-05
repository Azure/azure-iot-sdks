// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Common.Exceptions
{
    using System;
    using System.Runtime.Serialization;

#if !WINDOWS_UWP
    [Serializable]
#endif
    public sealed class QuotaExceededException : IotHubException
    {
        public QuotaExceededException(string message)
            : this(message, null)
        {
        }

        public QuotaExceededException(string message, Exception innerException)
            : base(message, innerException, isTransient: true)
        {
        }

#if !WINDOWS_UWP
        QuotaExceededException(SerializationInfo info, StreamingContext context)
            : base(info, context)
        {
        }
#endif
    }
}
