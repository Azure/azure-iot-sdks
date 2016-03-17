// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client.Exceptions
{
    using System;
    using System.Runtime.Serialization;

#if !WINDOWS_UWP && !PCL
    [Serializable]
#endif
#if !WINDOWS_UWP && !PCL // Exporting custom exception types is not allowed in WinRT
    public
#endif
    sealed class ServerErrorException : IotHubException
    {
        public ServerErrorException(string message)
            : base(message, isTransient: true)
        {
        }

        public ServerErrorException(string message, Exception innerException)
            : base(message, innerException, isTransient: true)
        {
        }

#if !WINDOWS_UWP && !PCL
        ServerErrorException(SerializationInfo info, StreamingContext context)
            : base(info, context)
        {
        }
#endif

    }
}
