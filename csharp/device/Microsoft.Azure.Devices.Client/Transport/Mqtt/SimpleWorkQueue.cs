// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client.Transport.Mqtt
{
    using System;
    using System.Collections.Generic;
    using System.Threading.Tasks;
    using DotNetty.Common.Concurrency;
    using DotNetty.Common.Utilities;
    using DotNetty.Transport.Channels;

    /// <summary>
    /// Simple work queue with lifecycle state machine. 
    /// It is running work items if it is available; otherwise waits for new work item. 
    /// Worker will resume work as soon as new work has arrived.
    /// </summary>
    /// <typeparam name="TWork"></typeparam>
    class SimpleWorkQueue<TWork>
    {
        readonly Func<IChannelHandlerContext, TWork, Task> worker;

        readonly Queue<TWork> backlogQueue;
        readonly TaskCompletionSource completionSource;
        protected States State { get; set; }

        public SimpleWorkQueue(Func<IChannelHandlerContext, TWork, Task> worker)
        {
            this.worker = worker;
            this.completionSource = new TaskCompletionSource();
            this.backlogQueue = new Queue<TWork>();
        }

        public Task Completion => this.completionSource.Task;

        /// <summary>
        /// Current backlog size
        /// </summary>
        public int BacklogSize => this.backlogQueue.Count;

        /// <summary>
        /// Puts the new work to backlog queue and resume work if worker is idle.
        /// </summary>
        /// <param name="context"></param>
        /// <param name="workItem"></param>
        public virtual void Post(IChannelHandlerContext context, TWork workItem)
        {
            switch (this.State)
            {
                case States.Idle:
                    this.backlogQueue.Enqueue(workItem);
                    this.State = States.Processing;
                    this.StartWorkQueueProcessingAsync(context);
                    break;
                case States.Processing:
                case States.FinalProcessing:
                    this.backlogQueue.Enqueue(workItem);
                    break;
                case States.Aborted:
                    ReferenceCountUtil.Release(workItem);
                    break;
                default:
                    throw new ArgumentOutOfRangeException();
            }
        }

        /// <summary>
        /// Stops work gracefully. All backlog will be processed.
        /// </summary>
        public void Complete()
        {
            switch (this.State)
            {
                case States.Idle:
                    this.completionSource.TryComplete();
                    break;
                case States.Processing:
                    this.State = States.FinalProcessing;
                    break;
                case States.FinalProcessing:
                case States.Aborted:
                    break;
                default:
                    throw new ArgumentOutOfRangeException();
            }
        }


        public virtual void Abort()
        {
            Abort(null);
        }

        public virtual void Abort(Exception exception)
        {
            switch (this.State)
            {
                case States.Idle:
                case States.Processing:
                case States.FinalProcessing:
                    this.State = States.Aborted;

                    Queue<TWork> queue = this.backlogQueue;
                    while (queue.Count > 0)
                    {
                        TWork workItem = queue.Dequeue();
                        ReferenceCountUtil.Release(workItem);
                        if (exception == null)
                        {
                            (workItem as ICancellable)?.Cancel();
                        }
                        else
                        {
                            (workItem as ICancellable)?.Abort(exception);
                        }
                    }
                    break;
                case States.Aborted:
                    break;
                default:
                    throw new ArgumentOutOfRangeException();
            }
        }

        protected virtual Task DoWorkAsync(IChannelHandlerContext context, TWork work)
        {
            return this.worker(context, work);
        }

        async void StartWorkQueueProcessingAsync(IChannelHandlerContext context)
        {
            try
            {
                Queue<TWork> queue = this.backlogQueue;
                while (queue.Count > 0 && this.State != States.Aborted)
                {
                    TWork workItem = queue.Dequeue();
                    await this.DoWorkAsync(context, workItem).ConfigureAwait(false);
                }

                switch (this.State)
                {
                    case States.Processing:
                        this.State = States.Idle;
                        break;
                    case States.FinalProcessing:
                    case States.Aborted:
                        this.completionSource.TryComplete();
                        break;
                    default:
                        throw new ArgumentOutOfRangeException();
                }
            }
            catch (Exception ex)
            {
                this.Abort();
                this.completionSource.TrySetException(new ChannelMessageProcessingException(ex, context));
            }
        }

        protected enum States
        {
            Idle,
            Processing,
            FinalProcessing,
            Aborted
        }
    }
}