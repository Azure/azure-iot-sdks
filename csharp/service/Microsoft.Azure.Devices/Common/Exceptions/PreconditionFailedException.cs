// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Common.Exceptions
{
    using System;
    using System.Runtime.Serialization;
#if !WINDOWS_UWP
    using System.Web.UI;
#endif

#if !WINDOWS_UWP
    [Serializable]
#endif
    public sealed class PreconditionFailedException : IotHubException
    {
        public PreconditionFailedException(string message)
            : this(message, string.Empty)
        {
        }

        public PreconditionFailedException(string message, string trackingId)
            : base("Precondition failed: {0}".FormatInvariant(message), trackingId)
        {
        }

        public PreconditionFailedException(string message, Exception innerException)
            : base(message, innerException)
        {
        }

#if !WINDOWS_UWP
        PreconditionFailedException(SerializationInfo info, StreamingContext context)
            : base(info, context)
        {
        }
#endif
    }
}
