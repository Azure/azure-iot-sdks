// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices
{
    /// <summary>
    /// Contains methods that services can use to perform feedback receive operations.
    /// </summary>
    public abstract class FeedbackReceiver<T> : Receiver<T> where T: FeedbackBatch
    {
    }
}
