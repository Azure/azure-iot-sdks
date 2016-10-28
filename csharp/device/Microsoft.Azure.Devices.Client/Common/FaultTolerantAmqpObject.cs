// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client
{
    using System;
    using System.Threading;
    using System.Threading.Tasks;
    using Microsoft.Azure.Amqp;

    sealed class FaultTolerantAmqpObject<T> : Singleton<T> where T : AmqpObject
    {
        readonly Func<TimeSpan, CancellationToken, Task<T>> createObjectAsync;
        readonly Action<T> closeObject;
        readonly EventHandler onObjectClosed;

        public FaultTolerantAmqpObject(Func<TimeSpan, CancellationToken, Task<T> > createObjectAsync, Action<T> closeObject)
        {
            this.createObjectAsync = createObjectAsync;
            this.closeObject = closeObject;
            this.onObjectClosed = new EventHandler(this.OnObjectClosed);
        }

        public bool TryGetOpenedObject(out T openedAmqpObject)
        {
            var taskCompletionSource = this.TaskCompletionSource;
            if (taskCompletionSource != null && taskCompletionSource.Task.Status == TaskStatus.RanToCompletion)
            {
                openedAmqpObject = taskCompletionSource.Task.Result;
                if (openedAmqpObject == null || openedAmqpObject.State != AmqpObjectState.Opened)
                {
                    openedAmqpObject = null;
                }
            }
            else
            {
                openedAmqpObject = null;
            }

            return openedAmqpObject != null;
        }

        protected override async Task<T> OnCreateAsync(TimeSpan timeout, CancellationToken cancellationToken)
        {
            T amqpObject = await this.createObjectAsync(timeout, cancellationToken);
            amqpObject.SafeAddClosed((s, e) => this.Invalidate(amqpObject));

            return amqpObject;
        }

        protected override void OnSafeClose(T value)
        {
            this.closeObject(value);
        }

        void OnObjectClosed(object sender, EventArgs e)
        {
            T instance = (T)sender;
            this.closeObject(instance); 
            this.Invalidate(instance);
        }
    }
}
