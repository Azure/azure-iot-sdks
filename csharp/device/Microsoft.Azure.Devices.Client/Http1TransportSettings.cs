// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client
{
#if !WINDOWS_UWP && !PCL
    using System.Security.Cryptography.X509Certificates;
#endif

    /// <summary>
    /// contains Http1 transport-specific settings for DeviceClient
    /// </summary>
    public sealed class Http1TransportSettings : ITransportSettings
    {
        public TransportType GetTransportType()
        {
            return TransportType.Http1;
        }

#if !WINDOWS_UWP && !PCL
        public X509Certificate2 ClientCertificate { get; set; }
#endif
    }
}
