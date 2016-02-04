// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client.Transport
{
    // C# using aliases cannot name an unbound generic type declaration without supplying type arguments
    // Therefore, define a separate alias for each type argument
    using System;
    using System.Collections.Generic;
    using System.Threading.Tasks;

    /// <summary>
    /// Contains the implementation of methods that a device can use to send messages to and receive from the service.
    /// </summary>
    abstract class TransportHandlerBase
    {
        bool openCalled;
        bool closeCalled;
        volatile TaskCompletionSource<object> openTaskCompletionSource;

        protected TransportHandlerBase()
        {
            this.ThisLock = new object();
            this.openTaskCompletionSource = new TaskCompletionSource<object>(this);
        }

        protected abstract TimeSpan DefaultReceiveTimeout { get; set; }

        protected object ThisLock { get; private set; }

        /// <summary>
        /// Explicitly open the DeviceClient instance.
        /// </summary>
        public Task OpenAsync()
        {
            return this.EnsureOpenedAsync(true);
        }

        /// <summary>
        /// Close the DeviceClient instance
        /// </summary>
        /// <returns></returns>
        public Task CloseAsync()
        {
            TaskCompletionSource<object> localOpenTaskCompletionSource;
            lock (this.ThisLock)
            {
                if (this.closeCalled)
                {
                    return TaskHelpers.CompletedTask;
                }

                localOpenTaskCompletionSource = this.openTaskCompletionSource;
                this.closeCalled = true;
            }

            if (localOpenTaskCompletionSource != null)
            {
                localOpenTaskCompletionSource.TrySetCanceled();
            }

            return this.OnCloseAsync();
        }

        /// <summary>
        /// Receive a message from the device queue using the default timeout.
        /// </summary>
        /// <returns>The receive message or null if there was no message until the default timeout</returns>
        public Task<Message> ReceiveAsync()
        {
            return this.ReceiveAsync(this.DefaultReceiveTimeout);
        }

        /// <summary>
        /// Receive a message from the device queue with the specified timeout
        /// </summary>
        /// <returns>The receive message or null if there was no message until the specified time has elapsed</returns>
        public async Task<Message> ReceiveAsync(TimeSpan timeout)
        {
            TimeoutHelper.ThrowIfNegativeArgument(timeout);
            await this.EnsureOpenedAsync(false);
            return await this.OnReceiveAsync(timeout);
        }

        /// <summary>
        /// Deletes a received message from the device queue
        /// </summary>
        /// <returns>The lock identifier for the previously received message</returns>
        public async Task CompleteAsync(string lockToken)
        {
            if (string.IsNullOrEmpty(lockToken))
            {
                throw Fx.Exception.ArgumentNull("lockToken");
            }

            await this.EnsureOpenedAsync(false);
            await this.OnCompleteAsync(lockToken);
        }

        /// <summary>
        /// Deletes a received message from the device queue
        /// </summary>
        /// <returns>The previously received message</returns>
        public async Task CompleteAsync(Message message)
        {
            if (message == null)
            {
                throw Fx.Exception.ArgumentNull("message");
            }

            await this.EnsureOpenedAsync(false);
            await this.OnCompleteAsync(message);
        }

        /// <summary>
        /// Puts a received message back onto the device queue
        /// </summary>
        /// <returns>The previously received message</returns>
        public async Task AbandonAsync(string lockToken)
        {
            if (string.IsNullOrEmpty(lockToken))
            {
                throw Fx.Exception.ArgumentNull("lockToken");
            }

            await this.EnsureOpenedAsync(false);
            await this.OnAbandonAsync(lockToken);
        }

        /// <summary>
        /// Puts a received message back onto the device queue
        /// </summary>
        /// <returns>The lock identifier for the previously received message</returns>
        public async Task AbandonAsync(Message message)
        {
            if (message == null)
            {
                throw Fx.Exception.ArgumentNull("message");
            }

            await this.EnsureOpenedAsync(false);
            await this.OnAbandonAsync(message);
        }

        /// <summary>
        /// Deletes a received message from the device queue and indicates to the server that the message could not be processed.
        /// </summary>
        /// <returns>The previously received message</returns>
        public async Task RejectAsync(string lockToken)
        {
            if (string.IsNullOrEmpty(lockToken))
            {
                throw Fx.Exception.ArgumentNull("lockToken");
            }

            await this.EnsureOpenedAsync(false);
            await this.OnRejectAsync(lockToken);
        }

        /// <summary>
        /// Deletes a received message from the device queue and indicates to the server that the message could not be processed.
        /// </summary>
        /// <returns>The lock identifier for the previously received message</returns>
        public async Task RejectAsync(Message message)
        {
            if (message == null)
            {
                throw Fx.Exception.ArgumentNull("message");
            }

            await this.EnsureOpenedAsync(false);
            await this.OnRejectAsync(message);
        }

        /// <summary>
        /// Sends an event to device hub
        /// </summary>
        /// <returns>The message containing the event</returns>
        public async Task SendEventAsync(Message message)
        {
            if (message == null)
            {
                throw Fx.Exception.ArgumentNull("message");
            }

            await this.EnsureOpenedAsync(false);
            await this.OnSendEventAsync(message);
        }

        /// <summary>
        /// Sends a batch of events to device hub
        /// </summary>
        /// <returns>The task containing the event</returns>
        public async Task SendEventBatchAsync(IEnumerable<Message> messages)
        {
            if (messages == null)
            {
                throw Fx.Exception.ArgumentNull("messages");
            }

            await this.EnsureOpenedAsync(false);
            await this.OnSendEventAsync(messages);
        }

        protected Task EnsureOpenedAsync(bool explicitOpen)
        {
            bool needOpen = false;
            Task openTask;
            if (this.openTaskCompletionSource != null)
            {
                lock (this.ThisLock)
                {
                    if (this.openCalled)
                    {
                        if (this.openTaskCompletionSource == null)
                        {
                            // openTaskCompletionSource being null means open has finished completely
                            openTask = TaskHelpers.CompletedTask;
                        }
                        else
                        {
                            openTask = this.openTaskCompletionSource.Task;
                        }
                    }
                    else
                    {
                        // It's this call's job to kick off the open.
                        this.openCalled = true;
                        openTask = this.openTaskCompletionSource.Task;
                        needOpen = true;
                    }
                }
            }
            else
            {
                // Open has already fully completed.
                openTask = TaskHelpers.CompletedTask;
            }

            if (needOpen)
            {
                this.OnOpenAsync(explicitOpen).ContinueWith(
                    t =>
                    {
                        var localOpenTaskCompletionSource = this.openTaskCompletionSource;
                        lock (this.ThisLock)
                        {
                            if (!t.IsFaulted && !t.IsCanceled)
                            {
                                // This lets future calls avoid the Open logic all together.
                                this.openTaskCompletionSource = null;
                            }
                            else
                            {
                                // OpenAsync was cancelled or threw an exception, next time retry.
                                this.openCalled = false;
                                this.openTaskCompletionSource = new TaskCompletionSource<object>(this);
                            }
                        }

                        // This completes anyone waiting for open to finish
                        TaskHelpers.MarshalTaskResults(t, localOpenTaskCompletionSource);
                    });
            }

            return openTask;
        }

        protected abstract Task OnOpenAsync(bool explicitOpen);

        protected abstract Task OnCloseAsync();

        protected abstract Task<Message> OnReceiveAsync(TimeSpan timeout);

        protected virtual Task OnCompleteAsync(Message message)
        {
            return this.OnCompleteAsync(message.LockToken);
        }

        protected abstract Task OnCompleteAsync(string lockToken);

        protected virtual Task OnAbandonAsync(Message message)
        {
            return this.OnAbandonAsync(message.LockToken);
        }

        protected abstract Task OnAbandonAsync(string lockToken);

        protected virtual Task OnRejectAsync(Message message)
        {
            return this.OnRejectAsync(message.LockToken);
        }

        protected abstract Task OnRejectAsync(string lockToken);

        protected abstract Task OnSendEventAsync(Message message);

        protected abstract Task OnSendEventAsync(IEnumerable<Message> messages);

    }
}
