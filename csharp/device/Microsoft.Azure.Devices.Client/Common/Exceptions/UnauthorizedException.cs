// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client.Exceptions
{
    using System;
    using System.Runtime.Serialization;

#if !WINDOWS_UWP
    [Serializable]
#endif
#if !WINDOWS_UWP // Exporting custom exception types is not allowed in WinRT
    public
#endif
    sealed class UnauthorizedException : IotHubException
    {
        public UnauthorizedException(string message)
            : this(message, null)
        {
        }

        public UnauthorizedException(string message, Exception innerException)
            : base(message, innerException, isTransient: false)
        {
        }

#if !WINDOWS_UWP
        UnauthorizedException(SerializationInfo info, StreamingContext context)
            : base(info, context)
        {
        }
#endif
    }
}

