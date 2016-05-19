// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
namespace Microsoft.Azure.Devices.Client.Common
{
    using System.Threading.Tasks;

    static class TaskConstants
    {
        static readonly Task<bool> BooleanTrueTask = Task.FromResult(true);
        static readonly Task<int> IntNegativeOneTask = Task.FromResult(-1);

        /// <summary>
        ///     A task that has been completed with the value <c>true</c>.
        /// </summary>
        public static Task<bool> BooleanTrue
        {
            get { return BooleanTrueTask; }
        }

        /// <summary>
        ///     A task that has been completed with the value <c>false</c>.
        /// </summary>
        public static Task<bool> BooleanFalse
        {
            get { return TaskConstantsGeneric<bool>.Default; }
        }

        /// <summary>
        ///     A task that has been completed with the value <c>0</c>.
        /// </summary>
        public static Task<int> Int32Zero
        {
            get { return TaskConstantsGeneric<int>.Default; }
        }

        /// <summary>
        ///     A task that has been completed with the value <c>-1</c>.
        /// </summary>
        public static Task<int> Int32NegativeOne
        {
            get { return IntNegativeOneTask; }
        }

        /// <summary>
        ///     A <see cref="Task" /> that has been completed.
        /// </summary>
        public static Task Completed
        {
            get { return BooleanTrueTask; }
        }

        /// <summary>
        ///     A <see cref="Task" /> that will never complete.
        /// </summary>
        public static Task Never
        {
            get { return TaskConstantsGeneric<bool>.Never; }
        }

        /// <summary>
        ///     A task that has been canceled.
        /// </summary>
        public static Task Canceled
        {
            get { return TaskConstantsGeneric<bool>.Canceled; }
        }
    }

    /// <summary>
    ///     Provides completed task constants.
    /// </summary>
    /// <typeparam name="T">The type of the task result.</typeparam>
    static class TaskConstantsGeneric<T>
    {
        static readonly Task<T> defaultValue = Task.FromResult(default(T));

        static readonly Task<T> never = new TaskCompletionSource<T>().Task;

        static readonly Task<T> canceled = CanceledTask();

        static Task<T> CanceledTask()
        {
            var tcs = new TaskCompletionSource<T>();
            tcs.SetCanceled();
            return tcs.Task;
        }

        /// <summary>
        ///     A task that has been completed with the default value of <typeparamref name="T" />.
        /// </summary>
        public static Task<T> Default
        {
            get { return defaultValue; }
        }

        /// <summary>
        ///     A <see cref="Task" /> that will never complete.
        /// </summary>
        public static Task<T> Never
        {
            get { return never; }
        }

        /// <summary>
        ///     A task that has been canceled.
        /// </summary>
        public static Task<T> Canceled
        {
            get { return canceled; }
        }
    }
}