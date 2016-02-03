// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices
{
    using Newtonsoft.Json;
    using Newtonsoft.Json.Converters;

    /// <summary>
    /// Specifies the various feedback status codes for a command sent to a device.
    /// </summary>
    [JsonConverter(typeof(StringEnumConverter))]
    public enum FeedbackStatusCode
    {
        Success = 0,
        Expired = 1,
        DeliveryCountExceeded = 2,
        Rejected = 3
    }
}
