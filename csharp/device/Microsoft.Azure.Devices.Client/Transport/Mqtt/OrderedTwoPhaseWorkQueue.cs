// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client.Transport.Mqtt
{
    using System;
    using System.Collections.Generic;
    using System.Threading.Tasks;
    using DotNetty.Common.Utilities;
    using DotNetty.Transport.Channels;
    using Microsoft.Azure.Devices.Client.Exceptions;

    class OrderedTwoPhaseWorkQueue<TWorkId, TWork> : SimpleWorkQueue<TWork> where TWorkId:IEquatable<TWorkId>
    {
        class IncompleteWorkItem
        {
            public IncompleteWorkItem(TWorkId id, TWork workItem)
            {
                this.WorkItem = workItem;
                this.Id = id;
            }

            public TWork WorkItem { get; }

            public TWorkId Id { get; }
        }

        readonly Func<TWork, TWorkId> getWorkId;
        readonly Func<IChannelHandlerContext, TWork, Task> completeWork;
        readonly Queue<IncompleteWorkItem> incompleteQueue = new Queue<IncompleteWorkItem>();

        public OrderedTwoPhaseWorkQueue(Func<IChannelHandlerContext, TWork, Task> worker, Func<TWork, TWorkId> getWorkId, Func<IChannelHandlerContext, TWork, Task> completeWork)
            : base(worker)
        {
            this.getWorkId = getWorkId;
            this.completeWork = completeWork;
        }

        public Task CompleteWorkAsync(IChannelHandlerContext context, TWorkId workId)
        {
            if (this.incompleteQueue.Count == 0)
            {
                throw new IotHubClientException("Nothing to complete.");
            }
            IncompleteWorkItem incompleteWorkItem = this.incompleteQueue.Peek();
            if (incompleteWorkItem.Id.Equals(workId))
            {
                this.incompleteQueue.Dequeue();
                return this.completeWork(context, incompleteWorkItem.WorkItem);
            }
            throw new IotHubClientException($"Work must be complete in the same order as it was started. Expected work id: '{incompleteWorkItem.Id}', actual work id: '{workId}'");
        }

        protected override async Task DoWorkAsync(IChannelHandlerContext context, TWork work)
        {
            await base.DoWorkAsync(context, work);
            this.incompleteQueue.Enqueue(new IncompleteWorkItem(this.getWorkId(work), work));
        }

        public override void Abort()
        {
            this.Abort(null);
        }

        public override void Abort(Exception exception)
        {
            States stateBefore = this.State;
            base.Abort(exception);
            if (stateBefore != this.State && this.State == States.Aborted)
            {
                Queue<IncompleteWorkItem> queue = this.incompleteQueue;
                while (queue.Count > 0)
                {
                    TWork workItem = queue.Dequeue().WorkItem;
                    if (exception == null)
                    {
                        (workItem as ICancellable)?.Cancel();
                    }
                    else
                    {
                        (workItem as ICancellable)?.Abort(exception);
                    }
                }
            }
        }
    }
}