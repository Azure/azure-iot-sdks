// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client
{
    using System;
    using Microsoft.Azure.Devices.Client.Extensions;

#if !NETMF
    using System.Collections.Generic;
    using System.Threading.Tasks;
#endif

    // C# using aliases cannot name an unbound generic type declaration without supplying type arguments
    // Therefore, define a separate alias for each type argument
#if WINDOWS_UWP && !NETMF
    using AsyncTask = Windows.Foundation.IAsyncAction;
    using AsyncTaskOfMessage = Windows.Foundation.IAsyncOperation<Message>;
#elif !NETMF
    using AsyncTask = System.Threading.Tasks.Task;
    using AsyncTaskOfMessage = System.Threading.Tasks.Task<Message>;
#endif

    /// <summary>
    /// Contains the implementation of methods that a device can use to send messages to and receive from the service.
    /// </summary>
    internal abstract class DeviceClientHelper
    {
        bool openCalled;
        bool closeCalled;
#if !NETMF
        volatile TaskCompletionSource<object> openTaskCompletionSource;
#endif

        protected DeviceClientHelper()
        {
            this.ThisLock = new object();
#if !NETMF
            this.openTaskCompletionSource = new TaskCompletionSource<object>(this);
#endif
        }

        protected abstract TimeSpan DefaultReceiveTimeout { get; set; }

        protected object ThisLock { get; private set; }

        /// <summary>
        /// Explicitly open the DeviceClient instance.
        /// </summary>
#if NETMF
        public void Open()
        {
            this.EnsureOpened(true);
        }
#else
        public Task OpenAsync()
        {
            return this.EnsureOpenedAsync(true);
        }
#endif

        /// <summary>
        /// Close the DeviceClient instance
        /// </summary>
        /// <returns></returns>
#if NETMF
        public void Close()
        {

        }
#else
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
#endif

        /// <summary>
        /// Receive a message from the device queue using the default timeout.
        /// </summary>
        /// <returns>The receive message or null if there was no message until the default timeout</returns>
#if NETMF
        public Message Receive()
        {
            return this.Receive(this.DefaultReceiveTimeout);
        }
#else
        public Task<Message> ReceiveAsync()
        {
            return this.ReceiveAsync(this.DefaultReceiveTimeout);
        }
#endif

        /// <summary>
        /// Receive a message from the device queue with the specified timeout
        /// </summary>
        /// <returns>The receive message or null if there was no message until the specified time has elapsed</returns>
#if NETMF
        public Message Receive(TimeSpan timeout)
        {
            this.EnsureOpened(false);
            return this.OnReceive(timeout);
        }
#else
        public async Task<Message> ReceiveAsync(TimeSpan timeout)
        {
            TimeoutHelper.ThrowIfNegativeArgument(timeout);
            await this.EnsureOpenedAsync(false);
            return await this.OnReceiveAsync(timeout);
        }
#endif

        /// <summary>
        /// Deletes a received message from the device queue
        /// </summary>
#if NETMF
        /// <returns></returns>
        public void Complete(string lockToken)
        {
            this.EnsureOpened(false);
            this.OnComplete(lockToken);
        }
#else
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
#endif

        /// <summary>
        /// Deletes a received message from the device queue
        /// </summary>
#if NETMF
        /// <returns></returns>
        public void Complete(Message message)
        {
            if (message == null)
            {
                throw new ArgumentNullException();
            }

            this.EnsureOpened(false);
            this.OnComplete(message.LockToken);
        }
#else
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
#endif

        /// <summary>
        /// Puts a received message back onto the device queue
        /// </summary>
#if NETMF
        /// <returns></returns>
        public void Abandon(string lockToken)
        {
            if (lockToken.IsNullOrEmpty())
            {
                throw new ArgumentNullException();
            }

            this.EnsureOpened(false);
            this.OnAbandon(lockToken);
        }
#else
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
#endif

        /// <summary>
        /// Puts a received message back onto the device queue
        /// </summary>
#if NETMF
        /// <returns></returns>
        public void Abandon(Message message)
        {
            if (message == null)
            {
                throw new ArgumentNullException();
            }

            this.EnsureOpened(false);
            this.OnAbandon(message);
        }
#else
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
#endif

        /// <summary>
        /// Deletes a received message from the device queue and indicates to the server that the message could not be processed.
        /// </summary>
#if NETMF
        public void Reject(string lockToken)
        {
            if (lockToken.IsNullOrEmpty())
            {
                throw new ArgumentNullException();
            }

            this.EnsureOpened(false);
            this.OnReject(lockToken);
        }
#else
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
#endif

        /// <summary>
        /// Deletes a received message from the device queue and indicates to the server that the message could not be processed.
        /// </summary>
#if NETMF
        public void Reject(Message message)
        {
            if (message == null)
            {
                throw new ArgumentNullException();
            }

            this.EnsureOpened(false);
            this.OnReject(message);
        }
#else
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
#endif

        /// <summary>
        /// Sends an event to device hub
        /// </summary>
#if NETMF
        public void SendEvent(Message message)
        {
            if (message == null)
            {
                throw new ArgumentNullException();
            }

            this.EnsureOpened(false);
            this.OnSendEvent(message);
        }
#else
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
#endif

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

#if NETMF
        protected void EnsureOpened(bool explicitOpen)
        {

        }
#else
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
#endif

#if NETMF
        protected abstract void OnOpen(bool explicitOpen);

        protected abstract void OnClose();

        protected abstract Message OnReceive(TimeSpan timeout);

        protected virtual void OnComplete(Message message)
        {
            this.OnComplete(message.LockToken);
        }

        protected abstract void OnComplete(string lockToken);

        protected virtual void OnAbandon(Message message)
        {
            this.OnAbandon(message.LockToken);
        }

        protected abstract void OnAbandon(string lockToken);

        protected virtual void OnReject(Message message)
        {
            this.OnReject(message.LockToken);
        }

        protected abstract void OnReject(string lockToken);

        protected abstract void OnSendEvent(Message message);

        //protected abstract void OnSendEvent(IEnumerable<Message> messages);
#else
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
#endif

    }
}
