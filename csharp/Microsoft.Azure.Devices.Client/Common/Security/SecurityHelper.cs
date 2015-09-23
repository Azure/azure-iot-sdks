// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client
{
    using System;
    using System.Globalization;

    static class SecurityHelper
    {
        public static void ValidateIotHubHostName(string iotHubHostName, string iotHubName)
        {
            if (string.IsNullOrWhiteSpace(iotHubHostName))
            {
                throw new ArgumentNullException("iotHubHostName");
            }

            if (string.IsNullOrWhiteSpace(iotHubName))
            {
                throw new ArgumentNullException("iotHubName");
            }

            if (!iotHubHostName.StartsWith(string.Format(CultureInfo.InvariantCulture, "{0}.", iotHubName), StringComparison.OrdinalIgnoreCase))
            {
                throw new UnauthorizedAccessException("IOT hub does not correspond to host name");
            }
        }
    }
}
