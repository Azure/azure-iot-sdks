// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices
{
    using System.Collections.Generic;

    /// <summary>
    /// Contains the result of a Job query
    /// </summary>
    public class JobQueryResult
    {
        /// <summary>
        /// Gets or sets the Result of a Job query that is not an aggregation
        /// </summary>
        public IEnumerable<JobResponse> Result { get; set; }

        /// <summary>
        /// Gets or sets the Result of a Job query that is an aggregation
        /// </summary>
        public IEnumerable<Dictionary<string, object>> AggregateResult { get; set; }
    }
}
