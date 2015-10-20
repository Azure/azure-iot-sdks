// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices
{
    sealed class IotHubApiResources : ApiResources
    {
        internal static string GetString(string value, params object[] args)
        {
            return string.Format(ApiResources.Culture, value, args);
        }
    }

}
