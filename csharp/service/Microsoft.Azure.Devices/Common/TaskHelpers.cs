// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Common
{
    using System;
    using System.Runtime.InteropServices;
    using System.Threading;
    using System.Threading.Tasks;
#if !WINDOWS_UWP
    using System.Transactions;
#endif
    static class TaskHelpers
    {
        public static readonly Task CompletedTask = Task.FromResult(default(VoidTaskResult));

        /// <summary>
        /// Create a Task based on Begin/End IAsyncResult pattern.
        /// </summary>
        /// <param name="begin"></param>
        /// <param name="end"></param>
        /// <param name="state"> 
        /// This parameter helps reduce allocations by passing state to the Funcs. e.g.:
        ///  await TaskHelpers.CreateTask(
        ///      (c, s) => ((Transaction)s).BeginCommit(c, s),
        ///      (a) => ((Transaction)a.AsyncState).EndCommit(a),
        ///      transaction);
        /// </param>
        public static Task CreateTask(Func<AsyncCallback, object, IAsyncResult> begin, Action<IAsyncResult> end, object state = null)
        {
            Task retval;
            try
            {
                retval = Task.Factory.FromAsync(begin, end, state);
            }
            catch (Exception ex)
            {
                if (Fx.IsFatal(ex))
                {
                    throw;
                }

                var completionSource = new TaskCompletionSource<object>(state);
                completionSource.SetException(ex);
                retval = completionSource.Task;
            }

            return retval;
        }

        public static Task<T> CreateTask<T>(Func<AsyncCallback, object, IAsyncResult> begin, Func<IAsyncResult, T> end, object state = null)
        {
            Task<T> retval;
            try
            {
                retval = Task<T>.Factory.FromAsync(begin, end, state);
            }
            catch (Exception ex)
            {
                if (Fx.IsFatal(ex))
                {
                    throw;
                }

                var completionSource = new TaskCompletionSource<T>(state);
                completionSource.SetException(ex);
                retval = completionSource.Task;
            }

            return retval;
        }

#if !WINDOWS_UWP
        /// <summary>
        /// Create a Task based on Begin/End IAsyncResult pattern.
        /// </summary>
        /// <param name="transaction">The transaction (optional) to use.  If not null a TransactionScope will be used when calling the begin Func.</param>
        /// <param name="begin"></param>
        /// <param name="end"></param>
        /// <param name="state"> 
        /// This parameter helps reduce allocations by passing state to the Funcs. e.g.:
        ///  await TaskHelpers.CreateTask(
        ///      (c, s) => ((Transaction)s).BeginCommit(c, s),
        ///      (a) => ((Transaction)a.AsyncState).EndCommit(a),
        ///      transaction);
        /// </param>
        public static Task CreateTransactionalTask(Transaction transaction, Func<AsyncCallback, object, IAsyncResult> begin, Action<IAsyncResult> end, object state = null)
        {
            Task retval;
            TransactionScope scope = null;
            try
            {
                scope = Fx.CreateTransactionScope(transaction);
                retval = Task.Factory.FromAsync(begin, end, state);
                Fx.CompleteTransactionScope(ref scope);
            }
            catch (Exception ex)
            {
                if (Fx.IsFatal(ex))
                {
                    throw;
                }

                if (scope != null)
                {
                    scope.Dispose();
                }

                var completionSource = new TaskCompletionSource<object>(state);
                completionSource.SetException(ex);
                retval = completionSource.Task;
            }

            return retval;
        }

        public static Task<TResult> CreateTransactionalTask<TResult>(Transaction transaction, Func<AsyncCallback, object, IAsyncResult> begin, Func<IAsyncResult, TResult> end)
        {
            Task<TResult> retval;
            TransactionScope scope = null;
            try
            {
                scope = Fx.CreateTransactionScope(transaction);
                retval = Task.Factory.FromAsync(begin, end, null);
                Fx.CompleteTransactionScope(ref scope);
            }
            catch (Exception ex)
            {
                if (Fx.IsFatal(ex))
                {
                    throw;
                }

                if (scope != null)
                {
                    scope.Dispose();
                }

                var completionSource = new TaskCompletionSource<TResult>();
                completionSource.SetException(ex);
                retval = completionSource.Task;
            }

            return retval;
        }
#endif
        public static Task ExecuteAndGetCompletedTask(Action action)
        {
            TaskCompletionSource<object> completedTcs = new TaskCompletionSource<object>();

            try
            {
                action();
                completedTcs.SetResult(null);
            }
            catch (Exception e)
            {
                if (Fx.IsFatal(e))
                {
                    throw;
                }

                completedTcs.SetException(e);
            }

            return completedTcs.Task;
        }

        public static Task<TResult> ExecuteAndGetCompletedTask<TResult>(Func<TResult> function)
        {
            TaskCompletionSource<TResult> completedTcs = new TaskCompletionSource<TResult>();

            try
            {
                completedTcs.SetResult(function());
            }
            catch (Exception e)
            {
                if (Fx.IsFatal(e))
                {
                    throw;
                }

                completedTcs.SetException(e);
            }

            return completedTcs.Task;
        }

        public static void Fork(this Task thisTask)
        {
            Fork(thisTask, "TaskExtensions.Fork");
        }

        public static void Fork(this Task thisTask, string tracingInfo)
        {
            Fx.Assert(thisTask != null, "task is required!");
            thisTask.ContinueWith(t => Fx.Exception.TraceHandled(t.Exception, tracingInfo), TaskContinuationOptions.OnlyOnFaulted);
        }

        public static IAsyncResult ToAsyncResult(this Task task, AsyncCallback callback, object state)
        {
            if (task.AsyncState == state)
            {
                if (callback != null)
                {
                    task.ContinueWith(
                        t => callback(task),
                        TaskContinuationOptions.ExecuteSynchronously);
                }

                return task;
            }

            var tcs = new TaskCompletionSource<object>(state);
            task.ContinueWith(
                _ =>
                {
                    if (task.IsFaulted)
                    {
                        tcs.TrySetException(task.Exception.InnerExceptions);
                    }
                    else if (task.IsCanceled)
                    {
                        tcs.TrySetCanceled();
                    }
                    else
                    {
                        tcs.TrySetResult(null);
                    }

                    if (callback != null)
                    {
                        callback(tcs.Task);
                    }
                },
                TaskContinuationOptions.ExecuteSynchronously);

            return tcs.Task;
        }

        public static IAsyncResult ToAsyncResult<TResult>(this Task<TResult> task, AsyncCallback callback, object state)
        {
            if (task.AsyncState == state)
            {
                if (callback != null)
                {
                    task.ContinueWith(
                        t => callback(task),
                        TaskContinuationOptions.ExecuteSynchronously);
                }

                return task;
            }

            var tcs = new TaskCompletionSource<TResult>(state);
            task.ContinueWith(
                _ =>
                {
                    if (task.IsFaulted)
                    {
                        tcs.TrySetException(task.Exception.InnerExceptions);
                    }
                    else if (task.IsCanceled)
                    {
                        tcs.TrySetCanceled();
                    }
                    else
                    {
                        tcs.TrySetResult(task.Result);
                    }

                    if (callback != null)
                    {
                        callback(tcs.Task);
                    }
                },
                TaskContinuationOptions.ExecuteSynchronously);

            return tcs.Task;
        }

        public static void EndAsyncResult(IAsyncResult asyncResult)
        {
            Task task = asyncResult as Task;
            if (task == null)
            {
                throw Fx.Exception.AsError(new ArgumentException(CommonResources.InvalidAsyncResult));
            }

            try
            {
                task.Wait();
            }
            catch (AggregateException ae)
            {
                ExceptionDispatcher.Throw(ae.GetBaseException());
            }
        }

        public static TResult EndAsyncResult<TResult>(IAsyncResult asyncResult)
        {
            Task<TResult> task = asyncResult as Task<TResult>;
            if (task == null)
            {
                throw Fx.Exception.AsError(new ArgumentException(CommonResources.InvalidAsyncResult));
            }

            try
            {
                return task.Result;
            }
            catch (AggregateException ae)
            {
                ExceptionDispatcher.Throw(ae.GetBaseException());

                // Dummy Code
                throw ae.GetBaseException();
            }
        }

        internal static void MarshalTaskResults<TResult>(Task source, TaskCompletionSource<TResult> proxy)
        {
            Fx.Assert(source != null, "source Task is required!");
            Fx.Assert(proxy != null, "proxy TaskCompletionSource is required!");

            switch (source.Status)
            {
                case TaskStatus.Faulted:
                    var exception = source.Exception.GetBaseException();
                    proxy.TrySetException(exception);
                    break;
                case TaskStatus.Canceled:
                    proxy.TrySetCanceled();
                    break;
                case TaskStatus.RanToCompletion:
                    Task<TResult> castedSource = source as Task<TResult>;
                    proxy.TrySetResult(
                        castedSource == null ? default(TResult) : // source is a Task
                            castedSource.Result); // source is a Task<TResult>
                    break;
            }
        }

        public static Task WithTimeoutNoException(this Task task, TimeSpan timeout)
        {
            return WithTimeoutNoException(task, timeout, CancellationToken.None);
        }

        public static async Task WithTimeoutNoException(this Task task, TimeSpan timeout, CancellationToken token)
        {
            if (timeout == TimeSpan.MaxValue)
            {
                timeout = Timeout.InfiniteTimeSpan;
            }
            else if (timeout.TotalMilliseconds > Int32.MaxValue)
            {
                timeout = TimeSpan.FromMilliseconds(Int32.MaxValue);
            }
            else if (timeout < TimeSpan.Zero)
            {
                timeout = TimeSpan.Zero;
            }

            if (task.IsCompleted || (timeout == Timeout.InfiniteTimeSpan && token == CancellationToken.None))
            {
                await task;
                return;
            }

            using (var cts = CancellationTokenSource.CreateLinkedTokenSource(token))
            {
                if (task == await Task.WhenAny(task, CreateDelayTask(timeout, cts.Token)))
                {
                    cts.Cancel();
                    await task;
                }
            }
        }

        public static Task WithTimeout(this Task task, TimeSpan timeout, Func<string> errorMessage)
        {
            return WithTimeout(task, timeout, errorMessage, CancellationToken.None);
        }

        public static async Task WithTimeout(this Task task, TimeSpan timeout, Func<string> errorMessage, CancellationToken token)
        {
            if (timeout == TimeSpan.MaxValue)
            {
                timeout = Timeout.InfiniteTimeSpan;
            }
            else if (timeout.TotalMilliseconds > Int32.MaxValue)
            {
                timeout = TimeSpan.FromMilliseconds(Int32.MaxValue);
            }
            else if (timeout < TimeSpan.Zero)
            {
                timeout = TimeSpan.Zero;
            }

            if (task.IsCompleted || (timeout == Timeout.InfiniteTimeSpan && token == CancellationToken.None))
            {
                await task;
                return;
            }

            using (var cts = CancellationTokenSource.CreateLinkedTokenSource(token))
            {
                if (task == await Task.WhenAny(task, CreateDelayTask(timeout, cts.Token)))
                {
                    cts.Cancel();
                    await task;
                    return;
                }
            }

            throw new TimeoutException(errorMessage());
        }

        public static Task<T> WithTimeout<T>(this Task<T> task, TimeSpan timeout, Func<string> errorMessage)
        {
            return WithTimeout(task, timeout, errorMessage, CancellationToken.None);
        }

        public static async Task<T> WithTimeout<T>(this Task<T> task, TimeSpan timeout, Func<string> errorMessage, CancellationToken token)
        {
            if (timeout == TimeSpan.MaxValue)
            {
                timeout = Timeout.InfiniteTimeSpan;
            }
            else if (timeout.TotalMilliseconds > Int32.MaxValue)
            {
                timeout = TimeSpan.FromMilliseconds(Int32.MaxValue);
            }
            else if (timeout < TimeSpan.Zero)
            {
                timeout = TimeSpan.Zero;
            }

            if (task.IsCompleted || (timeout == Timeout.InfiniteTimeSpan && token == CancellationToken.None))
            {
                return await task;
            }

            using (var cts = CancellationTokenSource.CreateLinkedTokenSource(token))
            {
                if (task == await Task.WhenAny(task, CreateDelayTask(timeout, cts.Token)))
                {
                    cts.Cancel();
                    return await task;
                }
            }

            throw new TimeoutException(errorMessage());
        }

        static async Task CreateDelayTask(TimeSpan timeout, CancellationToken token)
        {
            try
            {
                await Task.Delay(timeout, token);
            }
            catch (TaskCanceledException)
            {
                // No need to throw. Caller is responsible for detecting
                // which task completed and throwing appropriate Timeout Exception
            }
        }

        [StructLayout(LayoutKind.Sequential, Size = 1)]
        internal struct VoidTaskResult
        {
        }
    }
}
