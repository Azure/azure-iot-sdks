// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.QueryExpressions
{
    using System;
    using System.Collections.Generic;
    using Microsoft.Azure.Devices.Common;
    using Newtonsoft.Json;

    internal class AggregationExpression
    {
        [JsonProperty(PropertyName = "keys")]
        public List<QueryProperty> Keys { get; set; }

        [JsonProperty(PropertyName = "properties")]
        public List<AggregationProperty> AggregatedProperties { get; set; }

        internal void Validate()
        {
            if (this.Keys == null)
            {
                throw new ArgumentNullException(nameof(this.Keys));
            }

            if (this.AggregatedProperties == null)
            {
                throw new ArgumentNullException(nameof(this.AggregatedProperties));
            }

            if (this.Keys.Count == 0 || this.AggregatedProperties.Count == 0)
            {
                throw new ArgumentException(ApiResources.QueryExpressions_InvalidAggregationExpr, nameof(this.AggregatedProperties));
            }

            foreach (QueryProperty key in this.Keys)
            {
                if (key == null)
                {
                    throw new ArgumentException(ApiResources.QueryExpressions_NullListValues.FormatInvariant(nameof(this.Keys)), nameof(this.Keys));
                }

                key.Validate();
            }

            foreach (AggregationProperty aggregationProperty in this.AggregatedProperties)
            {
                if (aggregationProperty == null)
                {
                    throw new ArgumentException(ApiResources.QueryExpressions_NullListValues.FormatInvariant(nameof(this.AggregatedProperties)), nameof(this.AggregatedProperties));
                }

                aggregationProperty.Validate();
            }
        }
    }
}
