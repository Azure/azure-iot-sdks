// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.QueryExpressions
{
    using System;
    using System.Collections.Generic;
    using Microsoft.Azure.Devices.Common;
    using Newtonsoft.Json;

    internal class QueryExpression
    {
        [JsonProperty(PropertyName = "filter")]
        [JsonConverter(typeof(FilterExpressionConverter))]
        public FilterExpression Filter { get; set; }

        [JsonProperty(PropertyName = "project")]
        public ProjectionExpression Projection { get; set; }

        [JsonProperty(PropertyName = "aggregate")]
        public AggregationExpression Aggregate { get; set; }

        [JsonProperty(PropertyName = "sort")]
        public List<SortExpression> Sort { get; set; }

        internal void Validate()
        {
            if (this.Projection != null && this.Aggregate != null)
            {
                throw new ArgumentException(ApiResources.QueryExpressions_InvalidQueryExpr);
            }

            this.Filter?.Validate();
            this.Projection?.Validate();
            this.Aggregate?.Validate();
            this.Sort?.ForEach(s =>
            {
                if (s == null)
                {
                    throw new ArgumentException(ApiResources.QueryExpressions_NullListValues.FormatInvariant(nameof(this.Sort)), nameof(this.Sort));
                }
                s.Validate();
            });
        }
    }
}
