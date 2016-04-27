// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices
{
    using Newtonsoft.Json;

    /// <summary>
    /// X509 client certificate thumbprints of the device
    /// </summary>
    public sealed class X509Thumbprint
    {
        /// <summary>
        /// X509 client certificate primary thumbprint
        /// </summary>
        [JsonProperty(PropertyName = "primaryThumbprint")]
        public string PrimaryThumbprint { get; set; }

        /// <summary>
        /// X509 client certificate secondary thumbprint
        /// </summary>
        [JsonProperty(PropertyName = "secondaryThumbprint")]
        public string SecondaryThumbprint { get; set; }
    }
}