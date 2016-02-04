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
        public AuthenticationMechanism()
        {
            this.SymmetricKey = new SymmetricKey();
        }

        [JsonProperty(PropertyName = "symmetricKey")]
        public SymmetricKey SymmetricKey { get; set; }
    }
}
