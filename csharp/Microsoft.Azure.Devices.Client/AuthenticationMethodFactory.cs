// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client
{
    using System;
    using Microsoft.Azure.Devices.Client.Extensions;

    public sealed class AuthenticationMethodFactory
    {
        static internal IAuthenticationMethod GetAuthenticationMethod(IotHubConnectionStringBuilder iotHubConnectionStringBuilder)
        {
            if (iotHubConnectionStringBuilder.SharedAccessKeyName != null)
            {
                return new DeviceAuthenticationWithSharedAccessPolicyKey(
                    iotHubConnectionStringBuilder.DeviceId, iotHubConnectionStringBuilder.SharedAccessKeyName, iotHubConnectionStringBuilder.SharedAccessKey);
            }
            else if (iotHubConnectionStringBuilder.SharedAccessKey != null)
            {
                return new DeviceAuthenticationWithRegistrySymmetricKey(
                    iotHubConnectionStringBuilder.DeviceId, iotHubConnectionStringBuilder.SharedAccessKey);
            }       
            else if (iotHubConnectionStringBuilder.SharedAccessSignature != null)
            {
                return new DeviceAuthenticationWithToken(iotHubConnectionStringBuilder.DeviceId, iotHubConnectionStringBuilder.SharedAccessSignature);
            }

            throw new InvalidOperationException("Unsupported Authentication Method {0}".FormatInvariant(iotHubConnectionStringBuilder));
        }

        public static IAuthenticationMethod CreateAuthenticationWithSharedAccessPolicyKey(string deviceId, string policyName, string key)
        {
            return new DeviceAuthenticationWithSharedAccessPolicyKey(deviceId, policyName, key);
        }

        public static IAuthenticationMethod CreateAuthenticationWithToken(string deviceId, string token)
        {
            return new DeviceAuthenticationWithToken(deviceId, token);
        }

        public static IAuthenticationMethod CreateAuthenticationWithRegistrySymmetricKey(string deviceId, string key)
        {
            return new DeviceAuthenticationWithRegistrySymmetricKey(deviceId, key);
        }

    }
}
