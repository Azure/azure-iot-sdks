// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices
{
    /// <summary>
    /// Used to provide storage parameters when making an export or import request.
    /// </summary>
    public sealed class ExportImportRequest
    {
        public string StorageConnectionString;

        public string ContainerName;
    }
}
