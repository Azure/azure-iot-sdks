// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client
{
    /// <summary>
    /// Specifies the different acknowledgement levels for Method calls.
    /// </summary>
    public enum MethodStatusType
    {
        /// <summary>
        /// the specified method is implemented and the call was successfull.
        /// </summary>
        Success,

        /// <summary>
        /// the device experienced some error condition during the method call.
        /// </summary>
        ServerError,

        /// <summary>
        /// the method requested is not supported by this client 'type'.
        /// </summary>
        NotSupported,

        /// <summary>
        /// the method requested is not implemented by this client.
        /// </summary>
        NotImplemented
    }
}

