// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client
{
    using System;
    using Microsoft.Azure.Devices.Client.Extensions;
    
    /// <summary>
    /// Authentication method that uses the symmetric key associated with the device in the device registry. 
    /// </summary>
    public sealed class DeviceAuthenticationWithRegistrySymmetricKey : IAuthenticationMethod
    {
        string deviceId;
        byte[] key;

        /// <summary>
        /// Initializes a new instance of the <see cref="DeviceAuthenticationWithRegistrySymmetricKey"/> class.
        /// </summary>
        /// <param name="deviceId">Device identifier.</param>
        /// <param name="key">Symmetric key associated with the device.</param>
        public DeviceAuthenticationWithRegistrySymmetricKey(string deviceId, string key)
        {
            this.SetDeviceId(deviceId);
            this.SetKeyFromBase64String(key);
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
        /// Gets or sets the key associated with the device.
        /// </summary>
        public byte[] Key 
        { 
            get { return this.key; }
            set { this.SetKey(value); } 
        }

        /// <summary>
        /// Gets or sets the Base64 formatted key associated with the device.
        /// </summary>
        public string KeyAsBase64String
        {
            get { return Convert.ToBase64String(this.Key); }
            set { this.SetKeyFromBase64String(value);}
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
            iotHubConnectionStringBuilder.AuthScheme = AuthenticationScheme.SAS;
            iotHubConnectionStringBuilder.SharedAccessKey = this.KeyAsBase64String;
            iotHubConnectionStringBuilder.SharedAccessKeyName = null;
            iotHubConnectionStringBuilder.SharedAccessSignature = null;

            return iotHubConnectionStringBuilder;
        }

        void SetKey(byte[] key)
        {
            if (key == null)
            {
                throw new ArgumentNullException("key");
            }

            this.key = key;
        }

        void SetKeyFromBase64String(string key)
        {
            if (key.IsNullOrWhiteSpace())
            {
                throw new ArgumentNullException("key");
            }

#if !NETMF
            if (!StringValidationHelper.IsBase64String(key))
            {
                throw new ArgumentException("Key must be base64 encoded");
            }
#endif

            this.key = Convert.FromBase64String(key);
        }

        void SetDeviceId(string deviceId)
        {
            if (deviceId.IsNullOrWhiteSpace())
            {
                throw new ArgumentNullException("deviceId");
            }

            this.deviceId = deviceId;
        }
    }
}
