// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.QueryExpressions
{
    using System;
    using Newtonsoft.Json;

    internal class AggregationProperty
    {
        [JsonProperty(PropertyName = "operator")]
        public AggregationOperatorType AggregationOperator { get; set; }

        [JsonProperty(PropertyName = "property")]
        public QueryProperty Property { get; set; }

        [JsonProperty(PropertyName = "columnName")]
        public string ResultColumnName { get; set; }

        internal void Validate()
        {
            if (this.AggregationOperator != AggregationOperatorType.Count)
            {
                if (this.Property == null)
                {
                    throw new ArgumentNullException(nameof(this.Property));
                }

                this.Property.Validate();
            }

            if (string.IsNullOrWhiteSpace(this.ResultColumnName))
            {
                throw new ArgumentNullException(nameof(this.ResultColumnName));
            }

            if (this.ResultColumnName.Contains("$"))
            {
                throw new ArgumentException(ApiResources.QueryExpressions_ReservedCharError, nameof(this.ResultColumnName));
            }
        }
    }
}
