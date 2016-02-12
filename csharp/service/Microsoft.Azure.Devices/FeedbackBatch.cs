// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices
{
    using System;
    using System.Collections.Generic;

    /// <summary>
    /// Contains a batch of feedback records.
    /// </summary>
    public class FeedbackBatch
    {
        public DateTime EnqueuedTime { get; set; }

        public IEnumerable<FeedbackRecord> Records { get; set; }

        public string UserId { get; set; }

        internal string LockToken { get; set; }
    }
}
