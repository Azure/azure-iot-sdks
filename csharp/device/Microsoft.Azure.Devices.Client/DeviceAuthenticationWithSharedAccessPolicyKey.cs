// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client
{
    using Microsoft.Azure.Devices.Client.Extensions;
    using System;

    /// <summary>
    /// Authentication method that uses a shared access policy key. 
    /// </summary>
    public sealed class DeviceAuthenticationWithSharedAccessPolicyKey : IAuthenticationMethod
    {
        string deviceId;
        string policyName;
        string key;

        /// <summary>
        /// Initializes a new instance of the <see cref="DeviceAuthenticationWithSharedAccessPolicyKey"/> class.
        /// </summary>
        /// <param name="deviceId">Device identifier.</param>
        /// <param name="policyName">Name of the shared access policy to use.</param>
        /// <param name="key">Key associated with the shared access policy.</param>
        public DeviceAuthenticationWithSharedAccessPolicyKey(string deviceId, string policyName, string key)
        {
            this.SetDeviceId(deviceId);
            this.SetKey(key);
            this.SetPolicyName(policyName);
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
        /// Gets or sets the key associated with the shared policy.
        /// </summary>
        public string Key
        {
            get { return this.key; }
            set { this.SetKey(value); }
        }

        /// <summary>
        /// Name of the shared access policy.
        /// </summary>
        public string PolicyName
        {
            get { return this.policyName; }
            set { this.SetPolicyName(value); }
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
            iotHubConnectionStringBuilder.AuthScheme = AuthenticationScheme.SharedAccessKey;
            iotHubConnectionStringBuilder.SharedAccessKey = this.Key;
            iotHubConnectionStringBuilder.SharedAccessKeyName = this.PolicyName;
            iotHubConnectionStringBuilder.SharedAccessSignature = null;

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

        void SetKey(string key)
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

            this.key = key;
        }

        void SetPolicyName(string policyName)
        {
            if (policyName.IsNullOrWhiteSpace())
            {
                throw new ArgumentNullException("policyName");
            }

            this.policyName = policyName;
        }
    }
}
