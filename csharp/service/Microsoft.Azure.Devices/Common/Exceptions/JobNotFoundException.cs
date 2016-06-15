// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Common.Exceptions
{
    using System;

#if !WINDOWS_UWP
    [Serializable]
#endif
    public sealed class JobNotFoundException : IotHubException
    {
        public JobNotFoundException(string jobId)
            : base("Job with ID '{0}' not found".FormatInvariant(jobId))
        {

        }
    }
}
