// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices
{
    using System;
    using Microsoft.Azure.Devices.Common.Security;

    /// <summary>
    /// Authentication method that uses a shared access policy key. 
    /// </summary>
    public sealed class ServiceAuthenticationWithSharedAccessPolicyKey : IAuthenticationMethod
    {
        string policyName;
        string key;

        /// <summary>
        /// Initializes a new instance of the <see cref="ServiceAuthenticationWithSharedAccessPolicyKey"/> class.
        /// </summary>
        /// <param name="policyName">Name of the shared access policy to use.</param>
        /// <param name="key">Key associated with the shared access policy.</param>
        public ServiceAuthenticationWithSharedAccessPolicyKey(string policyName, string key)
        {
            this.SetPolicyName(policyName);
            this.SetKey(key);
        }

        public string PolicyName
        {
            get { return this.policyName; }
            set { this.SetPolicyName(value);}
        }

        public string Key
        {
            get { return this.key; }
            set { this.SetKey(value); }
        }

        public IotHubConnectionStringBuilder Populate(IotHubConnectionStringBuilder iotHubConnectionStringBuilder)
        {
            if (iotHubConnectionStringBuilder == null)
            {
                throw new ArgumentNullException("iotHubConnectionStringBuilder");
            }

            iotHubConnectionStringBuilder.SharedAccessKey = this.Key;
            iotHubConnectionStringBuilder.SharedAccessKeyName = this.PolicyName;
            iotHubConnectionStringBuilder.SharedAccessSignature = null;

            return iotHubConnectionStringBuilder;
        }

        void SetPolicyName(string policyName)
        {
            if (string.IsNullOrWhiteSpace(policyName))
            {
                throw new ArgumentNullException("policyName");
            }

            this.policyName = policyName;
        }

        void SetKey(string key)
        {
            if (string.IsNullOrWhiteSpace(key))
            {
                throw new ArgumentNullException("key");
            }

            if (!StringValidationHelper.IsBase64String(key))
            {
                throw new ArgumentException("Key must be Base64 encoded");
            }

            this.key = key;
        }
    }
}