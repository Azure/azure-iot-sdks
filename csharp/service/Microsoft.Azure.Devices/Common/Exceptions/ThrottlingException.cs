// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Common.Exceptions
{
    using System;

#if !WINDOWS_UWP
    [Serializable]
#endif
    public class ThrottlingException : IotHubException
    {
        public ThrottlingException(string message)
            : base(message)
        {
        }

        public ThrottlingException(string message, Exception inner)
            : base(message, inner)
        {
        }
    }
}
