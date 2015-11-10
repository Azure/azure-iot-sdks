// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client
{
    using Microsoft.Azure.Devices.Client.Extensions;
    using System;

#if WINDOWS_UWP
    internal
#else
    public
#endif
    sealed class DeviceAuthenticationWithSharedAccessPolicyKey : IAuthenticationMethod
    {
        string deviceId;
        string policyName;
        string key;

        public DeviceAuthenticationWithSharedAccessPolicyKey(string deviceId, string policyName, string key)
        {
            this.SetDeviceId(deviceId);
            this.SetKey(key);
            this.SetPolicyName(policyName);
        }

        public string DeviceId
        {
            get { return this.deviceId; }
            set { this.SetDeviceId(value); }
        }

        public string Key
        {
            get { return this.key; }
            set { this.SetKey(value); }
        }

        public string PolicyName
        {
            get { return this.policyName; }
            set { this.SetPolicyName(value); }
        }

        public IotHubConnectionStringBuilder Populate(IotHubConnectionStringBuilder iotHubConnectionStringBuilder)
        {
            if (iotHubConnectionStringBuilder == null)
            {
                throw new ArgumentNullException("iotHubConnectionStringBuilder");
            }

            iotHubConnectionStringBuilder.DeviceId = this.DeviceId;
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
