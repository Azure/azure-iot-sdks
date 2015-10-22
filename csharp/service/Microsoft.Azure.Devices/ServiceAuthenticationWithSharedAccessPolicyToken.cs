// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices
{
    using System;
    using Microsoft.Azure.Devices.Common.Security;

    public sealed class ServiceAuthenticationWithSharedAccessPolicyToken : IAuthenticationMethod
    {
        string policyName;
        string token;

        public ServiceAuthenticationWithSharedAccessPolicyToken(string policyName, string token)
        {
            this.SetPolicyName(policyName);
            this.SetToken(token);
        }

        public string PolicyName
        {
            get { return this.policyName; }
            set { this.SetPolicyName(value); }
        }

        public string Token
        {
            get { return this.token; }
            set { this.SetToken(value); }
        }

        public IotHubConnectionStringBuilder Populate(IotHubConnectionStringBuilder iotHubConnectionStringBuilder)
        {
            if (iotHubConnectionStringBuilder == null)
            {
                throw new ArgumentNullException("iotHubConnectionStringBuilder");
            }

            iotHubConnectionStringBuilder.SharedAccessKeyName = this.PolicyName;
            iotHubConnectionStringBuilder.SharedAccessSignature = this.Token;
            iotHubConnectionStringBuilder.SharedAccessKey = null;

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

        void SetToken(string token)
        {
            if (string.IsNullOrWhiteSpace(token))
            {
                throw new ArgumentNullException("token");
            }

            if (!token.StartsWith(SharedAccessSignatureConstants.SharedAccessSignature, StringComparison.OrdinalIgnoreCase))
            {
                throw new ArgumentException("Token must be of type SharedAccessSignature");
            }

            this.token = token;
        }
    }
}
