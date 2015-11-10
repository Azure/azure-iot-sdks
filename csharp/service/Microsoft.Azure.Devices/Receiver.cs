// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices
{
    using System;
    using System.Threading.Tasks;

    public abstract class Receiver<T>
    {
        /// <summary>
        /// Receive a message using the default timeout.
        /// </summary>
        /// <returns>The receive message or null if there was no message until the default timeout</returns>
        public abstract Task<T> ReceiveAsync();

        /// <summary>
        /// Receives a message
        /// </summary>
        /// <param name="timeout">The timeout for receiving a message</param>
        /// <returns>The receive message or null if there was no message until the specified timeout</returns>
        public abstract Task<T> ReceiveAsync(TimeSpan timeout);

        /// <summary>
        /// Deletes a received message from the queue
        /// </summary>
        public abstract Task CompleteAsync(T t);

        /// <summary>
        /// Puts a received message back into the queue
        /// </summary>
        public abstract Task AbandonAsync(T t);
    }
}
