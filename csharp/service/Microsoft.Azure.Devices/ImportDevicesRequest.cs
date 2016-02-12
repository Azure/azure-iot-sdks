// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices
{
    /// <summary>
    /// Used to provide parameters when requesting an import through the Resource Provider.
    /// </summary>
    public sealed class ImportDevicesRequest
    {
        public string InputBlobContainerUri { get; set; }

        public string OutputBlobContainerUri { get; set; }
    }
}
