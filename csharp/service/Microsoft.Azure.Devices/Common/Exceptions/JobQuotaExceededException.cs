// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Common.Exceptions
{
    using System;

#if !WINDOWS_UWP
    [Serializable]
#endif
    public sealed class JobQuotaExceededException : IotHubException
    {
        public JobQuotaExceededException()
            : base("Job quota has been exceeded")
        {

        }
    }
}
