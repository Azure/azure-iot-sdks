// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client
{
    using System;
    using System.Threading.Tasks;
    using Windows.Foundation;

    static class WinRTHelpers
    {
        public static IAsyncOperation<T> AsTaskOrAsyncOp<T>(this Task<T> op) { return op.AsAsyncOperation<T>(); }
        public static IAsyncAction AsTaskOrAsyncOp(this Task op) { return op.AsAsyncAction(); }
    }

    //
    // Summary:
    //     Identifies the type of event that has caused the trace.
    // Replacement for System.Diagnostics.TraceEventType
    public enum TraceEventType
    {
        //
        // Summary:
        //     Fatal error or application crash.
        Critical = 1,
        //
        // Summary:
        //     Recoverable error.
        Error = 2,
        //
        // Summary:
        //     Noncritical problem.
        Warning = 4,
        //
        // Summary:
        //     Informational message.
        Information = 8,
        //
        // Summary:
        //     Debugging trace.
        Verbose = 16,
        //
        // Summary:
        //     Starting of a logical operation.
        Start = 256,
        //
        // Summary:
        //     Stopping of a logical operation.
        Stop = 512,
        //
        // Summary:
        //     Suspension of a logical operation.
        Suspend = 1024,
        //
        // Summary:
        //     Resumption of a logical operation.
        Resume = 2048,
        //
        // Summary:
        //     Changing of correlation identity.
        Transfer = 4096
    }
}
