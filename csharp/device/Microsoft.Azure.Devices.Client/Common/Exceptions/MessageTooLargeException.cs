// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client.Exceptions
{
    using System;
    using System.Runtime.Serialization;
    using Microsoft.Azure.Devices.Client.Extensions;

#if !WINDOWS_UWP
    [Serializable]
#endif
#if !WINDOWS_UWP // Exporting custom exception types is not allowed in WinRT
    public
#endif
    sealed class MessageTooLargeException : IotHubException
    {
        public MessageTooLargeException(int maximumMessageSizeInBytes)
            : base("Message size cannot exceed {0} bytes".FormatInvariant(maximumMessageSizeInBytes))
        {
            this.MaximumMessageSizeInBytes = maximumMessageSizeInBytes;
        }

        public MessageTooLargeException(string message)
            : base(message)
        {
        }

        public MessageTooLargeException(string message, Exception innerException)
            : base(message, innerException)
        {
        }

#if !WINDOWS_UWP
        MessageTooLargeException(SerializationInfo info, StreamingContext context)
            : base(info, context)
        {
        }
#endif
        internal int MaximumMessageSizeInBytes
        {
            get;
            private set;
        }
    }
}
