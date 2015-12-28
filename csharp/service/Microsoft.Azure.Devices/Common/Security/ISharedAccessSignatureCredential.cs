// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Common.Security
{
    using System;

    using Microsoft.Azure.Devices.Common.Data;
      
    public interface ISharedAccessSignatureCredential
    {
        bool IsExpired();

        DateTime ExpiryTime();

        void Authenticate(SharedAccessSignatureAuthorizationRule sasAuthorizationRule);

        void Authorize(string hostName);

        void Authorize(Uri targetAddress);
    }
}
