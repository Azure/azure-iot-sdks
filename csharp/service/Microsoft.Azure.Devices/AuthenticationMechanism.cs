// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices
{
    using Newtonsoft.Json;

    /// <summary>
    /// Used to specify the authentication mechanism used by a device.
    /// </summary>
    public sealed class AuthenticationMechanism
    {
        /// <summary>
        /// default ctor
        /// </summary>
        public AuthenticationMechanism()
        {
            this.SymmetricKey = new SymmetricKey();
            this.X509Thumbprint = new X509Thumbprint();
        }

        /// <summary>
        /// Gets or sets the <see cref="SymmetricKey"/> used for Authentication
        /// </summary>
        [JsonProperty(PropertyName = "symmetricKey")]
        public SymmetricKey SymmetricKey { get; set; }

        /// <summary>
        /// X509 client certificate thumbprints
        /// </summary>
        [JsonProperty(PropertyName = "x509Thumbprint")]
        public X509Thumbprint X509Thumbprint { get; set; }
    }
}
