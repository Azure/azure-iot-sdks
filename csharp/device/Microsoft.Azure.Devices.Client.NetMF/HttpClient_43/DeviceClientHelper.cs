// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client
{
    using System;
    using Microsoft.Azure.Devices.Client.Extensions;
    
    /// <summary>
    /// Contains the implementation of methods that a device can use to send messages to and receive from the service.
    /// </summary>
    internal abstract class DeviceClientHelper
    {
        bool openCalled;
        bool closeCalled;

        protected DeviceClientHelper()
        {
            this.ThisLock = new object();
        }

        protected abstract TimeSpan DefaultReceiveTimeout { get; set; }

        protected object ThisLock { get; private set; }
        
        /// <summary>
        /// Explicitly open the DeviceClient instance.
        /// </summary>
        public void Open()
        {
            this.EnsureOpened(true);
        }

        /// <summary>
        /// Close the DeviceClient instance
        /// </summary>
        /// <returns></returns>
        public void Close()
        {
            // Kept for API symmetry purposes. Since .NET MF supports only HTTP for now, there's really nothing to close.
        }

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
            this.EnsureOpened(false);
            return this.OnReceive(timeout);
        }

        /// <summary>
        /// Deletes a received message from the device queue
        /// </summary>
        /// <returns></returns>
        public void Complete(string lockToken)
        {
            this.EnsureOpened(false);
            this.OnComplete(lockToken);
        }

        /// <summary>
        /// Deletes a received message from the device queue
        /// </summary>
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

        /// <summary>
        /// Puts a received message back onto the device queue
        /// </summary>
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

        /// <summary>
        /// Puts a received message back onto the device queue
        /// </summary>
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

        /// <summary>
        /// Deletes a received message from the device queue and indicates to the server that the message could not be processed.
        /// </summary>
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
        public void SendEvent(Message message)
        {
            if (message == null)
            {
                throw new ArgumentNullException();
            }

            this.EnsureOpened(false);
            this.OnSendEvent(message);
        }

        protected void EnsureOpened(bool explicitOpen)
        {
            // Kept here for API symmetry purposes.
        }

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
    }
}
