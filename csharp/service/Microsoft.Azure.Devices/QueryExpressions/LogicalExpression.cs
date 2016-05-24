// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.QueryExpressions
{
    using System;
    using System.Collections.Generic;
    using Microsoft.Azure.Devices.Common;
    using Newtonsoft.Json;

    /// <summary>
    /// TODO: [Sridhar] Add comments to all Query classes and move Error Strings to the RESX Files
    /// </summary>
    internal class LogicalExpression : FilterExpression
    {
        public LogicalExpression() : base(FilterExpressionType.Logical)
        {
        }

        [JsonProperty(PropertyName = "logicalOperator")]
        public LogicalOperatorType LogicalOperator { get; set; }

        [JsonProperty(PropertyName = "filters")]
        public List<FilterExpression> FilterExpressions { get; set; }

        internal override void Validate()
        {
            if (this.FilterExpressions == null)
            {
                throw new ArgumentNullException(nameof(this.FilterExpressions));
            }

            if (this.FilterExpressions.Count == 0)
            {
                throw new ArgumentException(ApiResources.QueryExpressions_InvalidLogicalExpr, nameof(this.FilterExpressions));
            }

            if (this.LogicalOperator == LogicalOperatorType.Not &&
                this.FilterExpressions.Count != 1)
            {
                throw new ArgumentException(ApiResources.QueryExpressions_InvalidLogicalNotExpr, nameof(this.FilterExpressions));
            }
            
            foreach (FilterExpression filterExpression in this.FilterExpressions)
            {
                if (filterExpression == null)
                {
                    throw new ArgumentException(ApiResources.QueryExpressions_NullListValues.FormatInvariant(nameof(this.FilterExpressions)), nameof(this.FilterExpressions));
                }

                filterExpression.Validate();
            }
        }
    }
}
