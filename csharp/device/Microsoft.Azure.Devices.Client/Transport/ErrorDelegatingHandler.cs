// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client.Transport
{
    using System;
    using System.Collections.Generic;
    using System.IO;
    using System.Linq;
    using System.Threading;
    using System.Threading.Tasks;
#if !PCL && !WINDOWS_UWP
#endif
    using Microsoft.Azure.Devices.Client.Exceptions;
    using Microsoft.Azure.Devices.Client.Extensions;

    // Copyright (c) Microsoft. All rights reserved.
    // Licensed under the MIT license. See LICENSE file in the project root for full license information.

    sealed class ErrorDelegatingHandler : DefaultDelegatingHandler
    {

        internal static readonly HashSet<Type> TransientExceptions = new HashSet<Type>
        {
            typeof(IotHubClientTransientException),
            typeof(IotHubCommunicationException),
            typeof(ServerBusyException),
            typeof(IOException),
            typeof(TimeoutException),
            typeof(ObjectDisposedException),
            typeof(OperationCanceledException),
            typeof(TaskCanceledException),
#if !PCL && !WINDOWS_UWP
            typeof(System.Net.Sockets.SocketException),
#endif
        };

        internal static readonly HashSet<Type> TransportTransientExceptions = new HashSet<Type>
        {
            typeof(IotHubClientTransientException),
            typeof(ServerBusyException),
            typeof(OperationCanceledException),
            typeof(TaskCanceledException),
        };

        readonly Func<IDelegatingHandler> handlerFactory;

        volatile TaskCompletionSource<int> openCompletion;

        public ErrorDelegatingHandler(Func<IDelegatingHandler> handlerFactory)
        {
            this.handlerFactory = handlerFactory;
        }

        public override async Task OpenAsync(bool explicitOpen)
        {
            TaskCompletionSource<int> openPromise = this.openCompletion;
            IDelegatingHandler handler = this.InnerHandler;
            if (openPromise == null)
            {
                openPromise = new TaskCompletionSource<int>();
#pragma warning disable 420 //Reference to volitile variable will not be treated as volatile which is not quite true in this case.
                TaskCompletionSource<int> currentOpenPromise;
                if ((currentOpenPromise = Interlocked.CompareExchange(ref this.openCompletion, openPromise, null)) == null)
#pragma warning restore 420
                {
                    this.InnerHandler = this.handlerFactory();
                    try
                    {
                        await this.ExecuteWithErrorHandlingAsync(() => base.OpenAsync(explicitOpen), false);
                        openPromise.TrySetResult(0);
                    }
                    catch (Exception ex) when (this.IsTranportTransient(ex))
                    {
                        Reset(openPromise, handler);
                        throw;
                    }
                    catch (Exception ex) when (!ex.IsFatal())
                    {
                        throw;
                    }
                }
                else
                {
                    await currentOpenPromise.Task;
                }
            }
            else
            {
                await openPromise.Task;
            }
        }

        public override Task<Message> ReceiveAsync()
        {
            return this.ExecuteWithErrorHandlingAsync(() => base.ReceiveAsync());
        }

        public override Task<Message> ReceiveAsync(TimeSpan timeout)
        {
            return this.ExecuteWithErrorHandlingAsync(() => base.ReceiveAsync(timeout));
        }

        public override Task AbandonAsync(string lockToken)
        {
            return this.ExecuteWithErrorHandlingAsync(() => base.AbandonAsync(lockToken), true);
        }

        public override Task CompleteAsync(string lockToken)
        {
            return this.ExecuteWithErrorHandlingAsync(() => base.CompleteAsync(lockToken), true);
        }

        public override Task RejectAsync(string lockToken)
        {
            return this.ExecuteWithErrorHandlingAsync(() => base.RejectAsync(lockToken), true);
        }

        public override Task SendEventAsync(IEnumerable<Message> messages)
        {
            return this.ExecuteWithErrorHandlingAsync(() => base.SendEventAsync(messages), true);
        }

        public override Task SendEventAsync(Message message)
        {
            return this.ExecuteWithErrorHandlingAsync(() => base.SendEventAsync(message), true);
        }

        async Task<T> ExecuteWithErrorHandlingAsync<T>(Func<Task<T>> asyncOperation)
        {
            await this.EnsureOpenAsync();

            TaskCompletionSource<int> completedPromise = this.openCompletion;

            IDelegatingHandler handler = this.InnerHandler;
            try
            {
                return await asyncOperation();
            }
            catch (Exception ex) when (!ex.IsFatal())
            {
                if (this.IsTransient(ex))
                {
                    if (this.IsTranportTransient(ex))
                    {
                        if (ex is IotHubClientTransientException)
                        {
                            throw;
                        }
                        throw new IotHubClientTransientException("Transient error occured, please retry.", ex);
                    }
                    this.Reset(completedPromise, handler);
                    if (ex is IotHubClientTransientException)
                    {
                        throw;
                    }
                    throw new IotHubClientTransientException("Transient error occured, please retry.", ex);
                }
                this.Reset(completedPromise, handler);
                throw;
            }
        }

        async Task ExecuteWithErrorHandlingAsync(Func<Task> asyncOperation, bool ensureOpen)
        {
            if (ensureOpen)
            {
                await this.EnsureOpenAsync();
            }

            TaskCompletionSource<int> completedPromise = this.openCompletion;
            IDelegatingHandler handler = this.InnerHandler;

            try
            {
                await asyncOperation();
            }
            catch (Exception ex) when (!ex.IsFatal())
            {
                if (this.IsTransient(ex))
                {
                    if (this.IsTranportTransient(ex))
                    {
                        if (ex is IotHubClientTransientException)
                        {
                            throw;
                        }
                        throw new IotHubClientTransientException("Transient error occured, please retry.", ex);
                    }
                    this.Reset(completedPromise, handler);
                    if (ex is IotHubClientTransientException)
                    {
                        throw;
                    }
                    throw new IotHubClientTransientException("Transient error occured, please retry.", ex);
                }
                this.Reset(completedPromise, handler);
                throw;
            }
        }

        Task EnsureOpenAsync()
        {
            return this.OpenAsync(false);
        }

        bool IsTranportTransient(Exception exception)
        {
            return exception.Unwind(true).Any(e => TransportTransientExceptions.Contains(e.GetType()));
        }

        bool IsTransient(Exception exception)
        {
            return exception.Unwind(true).Any(e => TransientExceptions.Contains(e.GetType()));
        }

        void Reset(TaskCompletionSource<int> completion, IDelegatingHandler handler)
        {
            if (completion == this.openCompletion)
            {
#pragma warning disable 420 //Reference to volitile variable will not be treated as volatile which is not quite true in this case.
                if (Interlocked.CompareExchange(ref this.openCompletion, null, completion) == completion)
#pragma warning restore 420
                {
                    if (handler == this.InnerHandler)
                    {
                        this.Cleanup(handler);
                    }
                }
            }
        }

        async void Cleanup(IDelegatingHandler handler)
        {
            try
            {
                if (handler != null)
                {
                    await handler.CloseAsync();
                }
            }
            catch (Exception ex) when (!ex.IsFatal())
            {
                //unexpected behaviour - ignore. LOG?
            }
        }
    }
}