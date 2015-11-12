// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client
{
    using System;
    using Microsoft.Azure.Devices.Client.Extensions;

    public sealed class DeviceAuthenticationWithToken : IAuthenticationMethod
    {
        string deviceId;
        string token;

        public DeviceAuthenticationWithToken(string deviceId, string token)
        {
            this.SetDeviceId(deviceId);
            this.SetToken(token);
        }

        public string DeviceId
        {
            get { return this.deviceId; }
            set { this.SetDeviceId(value); }
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

            iotHubConnectionStringBuilder.DeviceId = this.DeviceId;
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

            if (token.IndexOf(SharedAccessSignatureConstants.SharedAccessSignature) != 0)
            {
                throw new ArgumentException("Token must be of type SharedAccessSignature");
            }

            this.token = token;
        }
    }
}
