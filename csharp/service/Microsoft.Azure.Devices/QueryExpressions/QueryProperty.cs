// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.QueryExpressions
{
    using System;
    using Newtonsoft.Json;

    internal class QueryProperty
    {
        [JsonProperty(PropertyName = "name")]
        public string PropertyName { get; set; }

        [JsonProperty(PropertyName = "type")]
        public PropertyType PropertyType { get; set; }

        internal void Validate()
        {
            if (string.IsNullOrWhiteSpace(this.PropertyName))
            {
                throw new ArgumentNullException(nameof(this.PropertyName));
            }

            if (this.PropertyName.Contains("$"))
            {
                throw new ArgumentException(ApiResources.QueryExpressions_ReservedCharError, nameof(this.PropertyName));
            }
        }
    }
}
