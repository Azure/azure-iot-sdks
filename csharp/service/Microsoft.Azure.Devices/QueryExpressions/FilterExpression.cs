// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.QueryExpressions
{
    using System;
    using Newtonsoft.Json;

    internal class FilterExpression
    {
        public FilterExpression() : this(FilterExpressionType.Comparison)
        {
        }

        public FilterExpression(FilterExpressionType type)
        {
            this.ExpressionType = type;
        }

        [JsonProperty(PropertyName = "type")]
        public FilterExpressionType ExpressionType { get; set; }

        internal virtual void Validate()
        {
        }
    }
}
