// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Common.Exceptions
{
    using System;
    using System.Runtime.Serialization;

#if !WINDOWS_UWP
    [Serializable]
#endif
    public sealed class MessageTooLargeException : IotHubException
    {
        public MessageTooLargeException(int maximumMessageSizeInBytes)
            : this(maximumMessageSizeInBytes, string.Empty)
        {
        }

        public MessageTooLargeException(int maximumMessageSizeInBytes, string trackingId)
            : base("Message size cannot exceed {0} bytes".FormatInvariant(maximumMessageSizeInBytes), trackingId)
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
            this.MaximumMessageSizeInBytes = info.GetInt32("MaximumMessageSizeInBytes");
        }
#endif

        internal int MaximumMessageSizeInBytes
        {
            get;
            private set;
        }

#if !WINDOWS_UWP
        /// <inheritdoc />
        public override void GetObjectData(SerializationInfo info, StreamingContext context)
        {
            base.GetObjectData(info, context);
            info.AddValue("MaximumMessageSizeInBytes", this.MaximumMessageSizeInBytes);
        }
#endif
    }
}
