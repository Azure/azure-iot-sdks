// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client
{
    using System.Threading.Tasks;

    static class DotNetHelpers
    {
        public static Task<T> AsTaskOrAsyncOp<T>(this Task<T> op) { return op; }
        public static Task AsTaskOrAsyncOp(this Task op) { return op; }
    }
}
