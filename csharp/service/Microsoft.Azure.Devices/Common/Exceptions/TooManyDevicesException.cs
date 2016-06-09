// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Common.Exceptions
{
    using System;
    using System.Runtime.Serialization;

    /// <summary>
    /// Exception thrown when the list of input devices is too large for an operation 
    /// </summary>
    /// 
#if !WINDOWS_UWP
    [Serializable]
#endif
    public sealed class TooManyDevicesException : IotHubException
    {
        /// <summary>
        /// ctor which takes an error message
        /// </summary>
        /// <param name="message"></param>
        public TooManyDevicesException(string message)
            : this(message, string.Empty)
        {
        }

        /// <summary>
        /// ctor which takes an error message and a tracking id
        /// </summary>
        /// <param name="message"></param>
        /// <param name="trackingId"></param>
        public TooManyDevicesException(string message, string trackingId)
            : base(message, trackingId)
        {
        }

        /// <summary>
        /// ctor which takes an error message alongwith an inner exception
        /// </summary>
        /// <param name="message"></param>
        /// <param name="innerException"></param>
        public TooManyDevicesException(string message, Exception innerException)
            : base(message, innerException)
        {
        }

#if !WINDOWS_UWP
        TooManyDevicesException(SerializationInfo info, StreamingContext context)
            : base(info, context)
        {
        }
#endif
    }
}
