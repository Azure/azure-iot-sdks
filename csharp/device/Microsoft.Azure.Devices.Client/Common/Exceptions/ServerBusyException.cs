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
    sealed class ServerBusyException : IotHubException
    {
        public ServerBusyException(string message)
            : this(message, null)
        {
        }

        public ServerBusyException(string message, Exception innerException)
            : base(message, innerException, isTransient: true)
        {
        }

#if !WINDOWS_UWP
        ServerBusyException(SerializationInfo info, StreamingContext context)
            : base(info, context)
        {
        }
#endif
    }
}
