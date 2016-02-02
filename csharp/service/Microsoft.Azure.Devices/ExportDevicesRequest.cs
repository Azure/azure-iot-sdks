// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices
{
    /// <summary>
    /// Used to provide parameters when requesting an export through the Resource Provider.
    /// </summary>
    public sealed class ExportDevicesRequest
    {
        public string ExportBlobContainerUri { get; set; }

        public bool ExcludeKeys { get; set; }
    }
}