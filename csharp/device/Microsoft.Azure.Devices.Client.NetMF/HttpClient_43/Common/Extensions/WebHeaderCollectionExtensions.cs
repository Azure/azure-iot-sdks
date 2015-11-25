// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client.Extensions
{
    using System.Net;

    static class WebHeaderCollectionExtensions
    {
        public static void TryGetValues(this WebHeaderCollection s, string name, out string[] values)
        {
            values = s.GetValues(name);
        }
    }
}
