// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.QueryExpressions
{
    using System;
    using System.Collections.Generic;
    using Microsoft.Azure.Devices.Common;
    using Newtonsoft.Json;

    internal class ProjectionExpression
    {
        [JsonProperty(PropertyName = "all")]
        public bool AllProperties { get; set; }

        [JsonProperty(PropertyName = "properties")]
        public List<QueryProperty> ProjectedProperties { get; set; }

        internal void Validate()
        {
            if (this.AllProperties)
            {
                return;
            }

            if (this.ProjectedProperties == null)
            {
                throw new ArgumentNullException(nameof(this.ProjectedProperties));
            }

            if (this.ProjectedProperties.Count == 0)
            {
                throw new ArgumentException(ApiResources.QueryExpressions_InvalidProjectionExpr, nameof(this.ProjectedProperties));
            }

            foreach (QueryProperty projectedProperty in this.ProjectedProperties)
            {
                if (projectedProperty == null)
                {
                    throw new ArgumentException(ApiResources.QueryExpressions_NullListValues.FormatInvariant(nameof(this.ProjectedProperties)), nameof(this.ProjectedProperties));
                }

                projectedProperty.Validate();
            }
        }
    }
}
