// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.QueryExpressions
{
    using System;
    using Newtonsoft.Json;

    internal class SortExpression
    {
        [JsonProperty(PropertyName = "order")]
        public SortOrder Order { get; set; }

        [JsonProperty(PropertyName = "property")]
        public QueryProperty Property { get; set; }

        internal void Validate()
        {
            if (this.Property == null)
            {
                throw new ArgumentNullException(nameof(this.Property));
            }

            this.Property.Validate();
        }
    }
}
