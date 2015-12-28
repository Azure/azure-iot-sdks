// ---------------------------------------------------------------
// Copyright (c) Microsoft Corporation. All rights reserved.
// ---------------------------------------------------------------

namespace Microsoft.Azure.Devices.Client
{
    class SasDeviceTokenAuthenticationHeaderProvider : IAuthorizationHeaderProvider
    {
        readonly string sasSignatureOrSharedAccessKey;

        public SasDeviceTokenAuthenticationHeaderProvider(IotHubConnectionString iotHubConnectionString)
        {
            if (iotHubConnectionString.CredentialScope == CredentialScope.Device
                && iotHubConnectionString.CredentialType == CredentialType.SharedAccessKey)
            {
                // Http Authorization header will not accept only a key as a value. It must have a prefix
                this.sasSignatureOrSharedAccessKey = SecurityConstants.SharedAccessKeyFullFieldName
                                                     + iotHubConnectionString.GetPassword();
            }
            else
            {
                this.sasSignatureOrSharedAccessKey = iotHubConnectionString.GetPassword();
            }
            
        }

        public string GetAuthorizationHeader()
        {
            return this.sasSignatureOrSharedAccessKey;
        }
    }
}
