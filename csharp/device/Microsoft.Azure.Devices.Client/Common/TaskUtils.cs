// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
namespace Microsoft.Azure.Devices.Client.Common
{
    using System;
    using System.Threading;
    using System.Threading.Tasks;

    /// <summary>
    /// Stephen Cleary approved
    /// http://stackoverflow.com/questions/15428604/how-to-run-a-task-on-a-custom-taskscheduler-using-await
    /// </summary>
    static class TaskUtils
    {
        static readonly TaskFactory MyTaskFactory = new TaskFactory( CancellationToken.None, TaskCreationOptions.DenyChildAttach, TaskContinuationOptions.None, TaskScheduler.Default);

        internal static Task RunOnDefaultScheduler(this Func<Task> func)
        {
            return MyTaskFactory.StartNew(func).Unwrap();
        }

        internal static Task<T> RunOnDefaultScheduler<T>(this Func<Task<T>> func)
        {
            return MyTaskFactory.StartNew(func).Unwrap();
        }

        internal static Task RunOnDefaultScheduler(this Action func)
        {
            return MyTaskFactory.StartNew(func);
        }

        internal static Task<T> RunOnDefaultScheduler<T>(Func<T> func) => MyTaskFactory.StartNew(func);
    }
}