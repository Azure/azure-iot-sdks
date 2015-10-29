// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client
{
    using System;
    
    public sealed class DeviceAuthenticationWithRegistrySymmetricKey : IAuthenticationMethod
    {
        string deviceId;
        byte[] key;

        public DeviceAuthenticationWithRegistrySymmetricKey(string deviceId, string key)
        {
            this.SetDeviceId(deviceId);
            this.SetKeyFromBase64String(key);
        }

        public string DeviceId
        {
            get { return this.deviceId; }
            set { this.SetDeviceId(value); }
        }

        public byte[] Key 
        { 
            get { return this.key; }
            set { this.SetKey(value); } 
        }

        public string KeyAsBase64String
        {
            get { return Convert.ToBase64String(this.Key); }
            set { this.SetKeyFromBase64String(value);}
        }

        public IotHubConnectionStringBuilder Populate(IotHubConnectionStringBuilder iotHubConnectionStringBuilder)
        {
            if (iotHubConnectionStringBuilder == null)
            {
                throw new ArgumentNullException("iotHubConnectionStringBuilder");
            }

            iotHubConnectionStringBuilder.DeviceId = this.DeviceId;
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
            if (string.IsNullOrWhiteSpace(key))
            {
                throw new ArgumentNullException("key");
            }

            if (!StringValidationHelper.IsBase64String(key))
            {
                throw new ArgumentException("Key must be base64 encoded");
            }

            this.key = Convert.FromBase64String(key);
        }

        void SetDeviceId(string deviceId)
        {
            if (string.IsNullOrWhiteSpace(deviceId))
            {
                throw new ArgumentNullException("deviceId");
            }

            this.deviceId = deviceId;
        }
    }
}
