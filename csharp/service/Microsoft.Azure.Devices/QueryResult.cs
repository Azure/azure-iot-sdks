// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices
{
    using System;
    using System.Collections.Generic;
    using System.Net.Http;
    using System.Threading.Tasks;
    using Microsoft.Azure.Devices.Common.Extensions;
    using Newtonsoft.Json;
    using Newtonsoft.Json.Converters;

    /// <summary>
    ///     Contains the result of a device, device job or device aggregate query
    /// </summary>
    class QueryResult
    {
        private const string ContinuationTokenHeader = "x-ms-continuation";
        private const string QueryResultTypeHeader = "x-ms-item-type";

        [JsonProperty(PropertyName = "type", Required = Required.Always)]
        [JsonConverter(typeof(StringEnumConverter))]
        public QueryResultType Type { get; set; }

        [JsonProperty(PropertyName = "items", Required = Required.Always)]
        public IEnumerable<object> Items { get; set; }

        [JsonProperty(PropertyName = "continuationToken", Required = Required.AllowNull)]
        public string ContinuationToken { get; set; }

        internal static async Task<QueryResult> FromHttpResponseAsync(HttpResponseMessage response)
        {
            return new QueryResult
            {
#if WINDOWS_UWP
                Items = Newtonsoft.Json.JsonConvert.DeserializeObject<IEnumerable<object>>(await response.Content.ReadAsStringAsync()),
#else
                Items = await response.Content.ReadAsAsync<IEnumerable<object>>(),
#endif
                Type = (QueryResultType)Enum.Parse(typeof(QueryResultType), response.Headers.GetFirstValueOrNull(QueryResultTypeHeader) ?? "unknown"),
                ContinuationToken = response.Headers.GetFirstValueOrNull(ContinuationTokenHeader)
            };
        }
    }
}