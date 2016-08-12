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
            TaskCompletionSource<int> openCompletionBeforeOperationStarted = this.openCompletion;
            IDelegatingHandler handlerBeforeOperationStarted = this.InnerHandler;

            if (openCompletionBeforeOperationStarted == null)
            {
                openCompletionBeforeOperationStarted = new TaskCompletionSource<int>();
#pragma warning disable 420 //Reference to volitile variable will not be treated as volatile which is not quite true in this case.
                TaskCompletionSource<int> currentOpenPromise;
                if ((currentOpenPromise = Interlocked.CompareExchange(ref this.openCompletion, openCompletionBeforeOperationStarted, null)) == null)
#pragma warning restore 420
                {
                    this.InnerHandler = this.handlerFactory();
                    try
                    {
                        await this.ExecuteWithErrorHandlingAsync(() => base.OpenAsync(explicitOpen), false);
                        openCompletionBeforeOperationStarted.TrySetResult(0);
                    }
                    catch (Exception ex) when (IsTransportHandlerStillUsable(ex))
                    {
                        this.Reset(openCompletionBeforeOperationStarted, handlerBeforeOperationStarted);
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
                await openCompletionBeforeOperationStarted.Task;
            }
        }

        public override Task<Message> ReceiveAsync()
        {
            return this.ExecuteWithErrorHandlingAsync(() => base.ReceiveAsync(), true);
        }

        public override Task<Message> ReceiveAsync(TimeSpan timeout)
        {
            return this.ExecuteWithErrorHandlingAsync(() => base.ReceiveAsync(timeout), true);
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

        Task ExecuteWithErrorHandlingAsync(Func<Task> asyncOperation, bool ensureOpen)
        {
            return ExecuteWithErrorHandlingAsync(async () => { await asyncOperation(); return 0; }, ensureOpen);
        }

        async Task<T> ExecuteWithErrorHandlingAsync<T>(Func<Task<T>> asyncOperation, bool ensureOpen)
        {
            if (ensureOpen)
            {
                await this.EnsureOpenAsync();
            }

            TaskCompletionSource<int> openCompletionBeforeOperationStarted = this.openCompletion;
            IDelegatingHandler handlerBeforeOperationStarted = this.InnerHandler;

            try
            {
                return await asyncOperation();
            }
            catch (Exception ex) when (!ex.IsFatal())
            {
                if (IsTransient(ex))
                {
                    if (IsTransportHandlerStillUsable(ex))
                    {
                        if (ex is IotHubClientTransientException)
                        {
                            throw;
                        }
                        throw new IotHubClientTransientException("Transient error occured, please retry.", ex);
                    }
                    this.Reset(openCompletionBeforeOperationStarted, handlerBeforeOperationStarted);
                    if (ex is IotHubClientTransientException)
                    {
                        throw;
                    }
                    throw new IotHubClientTransientException("Transient error occured, please retry.", ex);
                }
                else
                {
                    this.Reset(openCompletionBeforeOperationStarted, handlerBeforeOperationStarted);
                    throw;
                }
            }
        }

        Task EnsureOpenAsync()
        {
            return this.OpenAsync(false);
        }

        static bool IsTransportHandlerStillUsable(Exception exception)
        {
            return exception.Unwind(true).Any(e => TransportTransientExceptions.Contains(e.GetType()));
        }

        static bool IsTransient(Exception exception)
        {
            return exception.Unwind(true).Any(e => TransientExceptions.Contains(e.GetType()));
        }

        void Reset(TaskCompletionSource<int> openCompletionBeforeOperationStarted, IDelegatingHandler handlerBeforeOperationStarted)
        {
            if (openCompletionBeforeOperationStarted == this.openCompletion)
            {
#pragma warning disable 420 //Reference to volitile variable will not be treated as volatile which is not quite true in this case.
                if (Interlocked.CompareExchange(ref this.openCompletion, null, openCompletionBeforeOperationStarted) == openCompletionBeforeOperationStarted)
#pragma warning restore 420
                {
                    Cleanup(handlerBeforeOperationStarted);
                }
            }
        }

        static async void Cleanup(IDelegatingHandler handler)
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