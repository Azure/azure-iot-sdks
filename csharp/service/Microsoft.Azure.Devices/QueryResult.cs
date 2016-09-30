// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

using Newtonsoft.Json;
using Newtonsoft.Json.Converters;

namespace Microsoft.Azure.Devices
{
    using System.Collections.Generic;

    /// <summary>
    ///     Contains the result of a device, device job or device aggregate query
    /// </summary>
    class QueryResult
    {
        [JsonProperty(PropertyName = "type", Required = Required.Always)]
        [JsonConverter(typeof(StringEnumConverter))]
        public QueryResultType Type { get; set; }

        [JsonProperty(PropertyName = "items", Required = Required.Always)]
        public IEnumerable<object> Items { get; set; }

        [JsonProperty(PropertyName = "continuationToken", Required = Required.AllowNull)]
        public string ContinuationToken { get; set; }
    }
}