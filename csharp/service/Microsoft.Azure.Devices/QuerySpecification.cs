// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices
{
    using Newtonsoft.Json;

    /// <summary>
    ///     A Json query request
    /// </summary>
    internal class QuerySpecification
    {
        [JsonProperty(PropertyName = "query", Required = Required.Always)]
        public string Sql { get; set; }

        [JsonIgnore]
        public int? PageSize { get; set; }

        [JsonIgnore]
        public string ContinuationToken { get; set; }
    }
}