// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.QueryExpressions
{
    using System;
    using System.Collections;
    using Newtonsoft.Json;

    internal class ComparisonExpression : FilterExpression
    {
        public ComparisonExpression() : base(FilterExpressionType.Comparison)
        {
        }

        [JsonProperty(PropertyName = "property")]
        public QueryProperty Property { get; set; }

        [JsonProperty(PropertyName = "value")]
        public object Value { get; set; }
        
        [JsonProperty(PropertyName = "comparisonOperator")]
        public ComparisonOperatorType ComparisonOperator { get; set; }

        internal override void Validate()
        {
            if (this.Property == null)
            {
                throw new ArgumentNullException(nameof(this.Property));
            }

            this.Property.Validate();

            if (this.Property.PropertyType == PropertyType.Default &&
                this.Property.PropertyName.Equals("Tags", StringComparison.OrdinalIgnoreCase) &&
                this.ComparisonOperator != ComparisonOperatorType.All)
            {
                throw new ArgumentException(ApiResources.QueryExpressions_TagsOperatorError, nameof(this.ComparisonOperator));
            }

            if (this.ComparisonOperator == ComparisonOperatorType.In ||
                this.ComparisonOperator == ComparisonOperatorType.NotIn ||
                this.ComparisonOperator == ComparisonOperatorType.All)
            {
                if (!(this.Value is IEnumerable))
                {
                    throw new ArgumentException(ApiResources.QueryExpressions_RequiresEnumerableType, nameof(this.Value));
                }
            }
        }
    }
}
