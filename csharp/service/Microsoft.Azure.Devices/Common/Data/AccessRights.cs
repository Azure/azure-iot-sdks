// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Common.Data
{
    using System;
    using System.Collections.Generic;
    using Newtonsoft.Json;
    using Newtonsoft.Json.Converters;

    [Flags]
    [JsonConverter(typeof(StringEnumConverter))]
    public enum AccessRights
    {
        RegistryRead = 1,
        RegistryWrite = RegistryRead | 2,
        ServiceConnect =  4,
        DeviceConnect = 8
    }

    static class AccessRightsHelper
    {
        public static string[] AccessRightsToStringArray(AccessRights accessRights)
        {
            var values = new List<string>(2);
            foreach (AccessRights right in Enum.GetValues(typeof(AccessRights)))
            {
                if (accessRights.HasFlag(right))
                {
                    values.Add(right.ToString());
                }
            }

            return values.ToArray();
        }
    }
}
