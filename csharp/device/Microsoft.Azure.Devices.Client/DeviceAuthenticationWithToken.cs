// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client
{
    using System;
    using Microsoft.Azure.Devices.Client.Extensions;

    /// <summary>
    /// Authentication method that uses a shared access signature token. 
    /// </summary>
    public sealed class DeviceAuthenticationWithToken : IAuthenticationMethod
    {
        string deviceId;
        string token;

        /// <summary>
        /// Initializes a new instance of the <see cref="DeviceAuthenticationWithToken"/> class.
        /// </summary>
        /// <param name="deviceId">Device Identifier.</param>
        /// <param name="token">Security Token.</param>
        public DeviceAuthenticationWithToken(string deviceId, string token)
        {
            this.SetDeviceId(deviceId);
            this.SetToken(token);
        }

        /// <summary>
        /// Gets or sets the device identifier.
        /// </summary>
        public string DeviceId
        {
            get { return this.deviceId; }
            set { this.SetDeviceId(value); }
        }

        /// <summary>
        /// Gets or sets the security token associated with the device.
        /// </summary>
        public string Token
        {
            get { return this.token; }
            set { this.SetToken(value); }
        }

        /// <summary>
        /// Populates an <see cref="IotHubConnectionStringBuilder"/> instance based on the properties of the current instance.
        /// </summary>
        /// <param name="iotHubConnectionStringBuilder">Instance to populate.</param>
        /// <returns>The populated <see cref="IotHubConnectionStringBuilder"/> instance.</returns>
        public IotHubConnectionStringBuilder Populate(IotHubConnectionStringBuilder iotHubConnectionStringBuilder)
        {
            if (iotHubConnectionStringBuilder == null)
            {
                throw new ArgumentNullException("iotHubConnectionStringBuilder");
            }

            iotHubConnectionStringBuilder.DeviceId = this.DeviceId;
            iotHubConnectionStringBuilder.AuthScheme = AuthenticationScheme.SharedAccessSignature;
            iotHubConnectionStringBuilder.SharedAccessSignature = this.Token;
            iotHubConnectionStringBuilder.SharedAccessKey = null;
            iotHubConnectionStringBuilder.SharedAccessKeyName = null;

            return iotHubConnectionStringBuilder;
        }

        void SetDeviceId(string deviceId)
        {
            if (deviceId.IsNullOrWhiteSpace())
            {
                throw new ArgumentNullException("deviceId");
            }

            this.deviceId = deviceId;
        }

        void SetToken(string token)
        {
            if (token.IsNullOrWhiteSpace())
            {
                throw new ArgumentNullException("token");
            }

#if NETMF
            if (token.IndexOf(SharedAccessSignatureConstants.SharedAccessSignature) != 0)
            {
                throw new ArgumentException("Token must be of type SharedAccessSignature");
            }
#else
            if (!token.StartsWith(SharedAccessSignatureConstants.SharedAccessSignature, StringComparison.OrdinalIgnoreCase))
            {
                throw new ArgumentException("Token must be of type SharedAccessSignature");
            }
#endif

            this.token = token;
        }
    }
}
