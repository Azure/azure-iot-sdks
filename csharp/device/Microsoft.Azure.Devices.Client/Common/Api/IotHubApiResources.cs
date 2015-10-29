// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

using Microsoft.Azure.Devices.Client.Common.Api;
namespace Microsoft.Azure.Devices.Client
{
    sealed class IotHubApiResources : ApiResources
    {
        internal static string GetString(string value, params object[] args)
        {
#if !WINDOWS_UWP
            return string.Format(ApiResources.Culture, value, args);
#else
            return string.Format(value, args);
#endif
        }
    }

}
