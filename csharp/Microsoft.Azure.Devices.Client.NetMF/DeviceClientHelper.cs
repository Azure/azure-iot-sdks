// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client
{
    using Microsoft.Azure.Devices.Client.Extensions;
    using System;
    
    /// <summary>
    /// Contains the implementation of methods that a device can use to send messages to and receive from the service.
    /// </summary>
    internal abstract class DeviceClientHelper
    {
        //bool openCalled;
        //bool closeCalled;
        //volatile TaskCompletionSource<object> openTaskCompletionSource;

        protected DeviceClientHelper()
        {
            this.ThisLock = new object();
            //this.openTaskCompletionSource = new TaskCompletionSource<object>(this);
        }

        protected abstract TimeSpan DefaultReceiveTimeout { get; set; }

        protected object ThisLock { get; private set; }

        ///// <summary>
        ///// Explicitly open the DeviceClient instance.
        ///// </summary>
        //public Task OpenAsync()
        //{
        //    return this.EnsureOpenedAsync(true);
        //}

        ///// <summary>
        ///// Close the DeviceClient instance
        ///// </summary>
        ///// <returns></returns>
        //public Task CloseAsync()
        //{
        //    TaskCompletionSource<object> localOpenTaskCompletionSource;
        //    lock (this.ThisLock)
        //    {
        //        if (this.closeCalled)
        //        {
        //            return TaskHelpers.CompletedTask;
        //        }

        //        localOpenTaskCompletionSource = this.openTaskCompletionSource;
        //        this.closeCalled = true;
        //    }

        //    if (localOpenTaskCompletionSource != null)
        //    {
        //        localOpenTaskCompletionSource.TrySetCanceled();
        //    }

        //    return this.OnCloseAsync();
        //}

        /// <summary>
        /// Receive a message from the device queue using the default timeout.
        /// </summary>
        /// <returns>The receive message or null if there was no message until the default timeout</returns>
        public Message Receive()
        {
            return this.Receive(this.DefaultReceiveTimeout);
        }

        /// <summary>
        /// Receive a message from the device queue with the specified timeout
        /// </summary>
        /// <returns>The receive message or null if there was no message until the specified time has elapsed</returns>
        public Message Receive(TimeSpan timeout)
        {
            //TimeoutHelper.ThrowIfNegativeArgument(timeout);
            this.EnsureOpened(false);
            return this.OnReceive(timeout);
        }

        ///// <summary>
        ///// Deletes a received message from the device queue
        ///// </summary>
        ///// <returns>The lock identifier for the previously received message</returns>
        //public async Task CompleteAsync(string lockToken)
        //{
        //    if (string.IsNullOrEmpty(lockToken))
        //    {
        //        throw Fx.Exception.ArgumentNull("lockToken");
        //    }

        //    await this.EnsureOpenedAsync(false);
        //    await this.OnCompleteAsync(lockToken);
        //}

        /// <summary>
        /// Deletes a received message from the device queue
        /// </summary>
        /// <returns>The previously received message</returns>
        public void Complete(Message message)
        {
            if (message == null)
            {
                //throw Fx.Exception.ArgumentNull("message");
            }

            this.EnsureOpened(false);
            this.OnCompleteAsync(message);
        }

        /// <summary>
        /// Puts a received message back onto the device queue
        /// </summary>
        /// <returns>The previously received message</returns>
        public void Abandon(string lockToken)
        {
            if (lockToken.IsNullOrEmpty())
            {
                throw new ArgumentNullException();
            }

            this.EnsureOpened(false);
            this.OnAbandon(lockToken);
        }

        /// <summary>
        /// Puts a received message back onto the device queue
        /// </summary>
        /// <returns>The lock identifier for the previously received message</returns>
        public void Abandon(Message message)
        {
            if (message == null)
            {
                throw new ArgumentNullException();
            }

            this.EnsureOpened(false);
            this.OnAbandon(message);
        }

        /// <summary>
        /// Deletes a received message from the device queue and indicates to the server that the message could not be processed.
        /// </summary>
        /// <returns>The previously received message</returns>
        public void Reject(string lockToken)
        {
            if (lockToken.IsNullOrEmpty())
            {
                throw new ArgumentNullException();
            }

            this.EnsureOpened(false);
            this.OnReject(lockToken);
        }

        /// <summary>
        /// Deletes a received message from the device queue and indicates to the server that the message could not be processed.
        /// </summary>
        /// <returns>The lock identifier for the previously received message</returns>
        public void Reject(Message message)
        {
            if (message == null)
            {
                throw new ArgumentNullException();
            }

            this.EnsureOpened(false);
            this.OnReject(message);
        }

        /// <summary>
        /// Sends an event to device hub
        /// </summary>
        /// <returns>The message containing the event</returns>
        public void SendEvent(Message message)
        {
            if (message == null)
            {
                throw new ArgumentNullException();
            }

            this.EnsureOpened(false);
            this.OnSendEvent(message);
        }

        ///// <summary>
        ///// Sends a batch of events to device hub
        ///// </summary>
        ///// <returns>The task containing the event</returns>
        //public async Task SendEventBatchAsync(IEnumerable<Message> messages)
        //{
        //    if (messages == null)
        //    {
        //        throw Fx.Exception.ArgumentNull("messages");
        //    }

        //    await this.EnsureOpenedAsync(false);
        //    await this.OnSendEventAsync(messages);
        //}

        protected void EnsureOpened(bool explicitOpen)
        {
            //if (this._amqpSender == null)
            //{
            //    this.StartSender();
            //}

            //bool needOpen = false;
            //Task openTask;
            //if (this.openTaskCompletionSource != null)
            //{
            //    lock (this.ThisLock)
            //    {
            //        if (this.openCalled)
            //        {
            //            if (this.openTaskCompletionSource == null)
            //            {
            //                // openTaskCompletionSource being null means open has finished completely
            //                openTask = TaskHelpers.CompletedTask;
            //            }
            //            else
            //            {
            //                openTask = this.openTaskCompletionSource.Task;
            //            }
            //        }
            //        else
            //        {
            //            // It's this call's job to kick off the open.
            //            this.openCalled = true;
            //            openTask = this.openTaskCompletionSource.Task;
            //            needOpen = true;
            //        }
            //    }
            //}
            //else
            //{
            //    // Open has already fully completed.
            //    openTask = TaskHelpers.CompletedTask;
            //}

            //if (needOpen)
            //{
            //    this.OnOpenAsync(explicitOpen).ContinueWith(
            //        t =>
            //        {
            //            var localOpenTaskCompletionSource = this.openTaskCompletionSource;
            //            lock (this.ThisLock)
            //            {
            //                if (!t.IsFaulted && !t.IsCanceled)
            //                {
            //                    // This lets future calls avoid the Open logic all together.
            //                    this.openTaskCompletionSource = null;
            //                }
            //                else
            //                {
            //                    // OpenAsync was cancelled or threw an exception, next time retry.
            //                    this.openCalled = false;
            //                    this.openTaskCompletionSource = new TaskCompletionSource<object>(this);
            //                }
            //            }

            //            // This completes anyone waiting for open to finish
            //            TaskHelpers.MarshalTaskResults(t, localOpenTaskCompletionSource);
            //        });
            //}

            //return openTask;
        }

        protected abstract void OnOpen(bool explicitOpen);

        //protected abstract Task OnCloseAsync();

        protected abstract Message OnReceive(TimeSpan timeout);

        protected virtual void OnCompleteAsync(Message message)
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

        //protected abstract Task OnSendEventAsync(IEnumerable<Message> messages);
    }
}
