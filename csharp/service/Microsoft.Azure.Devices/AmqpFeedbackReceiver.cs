// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices
{
    using System;
    using System.Collections.Generic;
    using System.Text;
    using System.Threading.Tasks;
    using Microsoft.Azure.Amqp;
    using Microsoft.Azure.Devices.Common;
    using Microsoft.Azure.Devices.Common.Extensions;

    sealed class AmqpFeedbackReceiver : FeedbackReceiver<FeedbackBatch>, IDisposable
    {
        readonly IotHubConnection iotHubConnection;
        readonly TimeSpan openTimeout;
        readonly TimeSpan operationTimeout;
        readonly FaultTolerantAmqpObject<ReceivingAmqpLink> faultTolerantReceivingLink;
        readonly string receivingPath;

        public AmqpFeedbackReceiver(IotHubConnection iotHubConnection)
        {
            this.iotHubConnection = iotHubConnection;
            this.openTimeout = IotHubConnection.DefaultOpenTimeout;
            this.operationTimeout = IotHubConnection.DefaultOperationTimeout;
            this.receivingPath = AmqpClientHelper.GetReceivingPath(EndpointKind.Feedback);
            this.faultTolerantReceivingLink = new FaultTolerantAmqpObject<ReceivingAmqpLink>(this.CreateReceivingLinkAsync, this.iotHubConnection.CloseLink);
        }

        public TimeSpan OpenTimeout
        {
            get
            {
                return this.openTimeout;
            }
        }

        public TimeSpan OperationTimeout
        {
            get
            {
                return this.operationTimeout;
            }
        }

        public IotHubConnection Connection
        {
            get
            {
                return this.iotHubConnection;
            }
        }

        public Task OpenAsync()
        {
            return this.faultTolerantReceivingLink.GetReceivingLinkAsync();
        }

        public Task CloseAsync()
        {
            return this.faultTolerantReceivingLink.CloseAsync();
        }

        public override Task<FeedbackBatch> ReceiveAsync()
        {
            return this.ReceiveAsync(this.OperationTimeout);
        }

        public override async Task<FeedbackBatch> ReceiveAsync(TimeSpan timeout)
        {
            try
            {
                ReceivingAmqpLink receivingLink = await this.faultTolerantReceivingLink.GetReceivingLinkAsync();
                AmqpMessage amqpMessage = await receivingLink.ReceiveMessageAsync(timeout);

                if (amqpMessage != null)
                {
                    using (amqpMessage)
                    {
                        AmqpClientHelper.ValidateContentType(amqpMessage, CommonConstants.BatchedFeedbackContentType);
                        var records = await AmqpClientHelper.GetObjectFromAmqpMessageAsync<IEnumerable<FeedbackRecord>>(amqpMessage);

                        return new FeedbackBatch
                        {
                            EnqueuedTime = (DateTime)amqpMessage.MessageAnnotations.Map[MessageSystemPropertyNames.EnqueuedTime],
                            LockToken = new Guid(amqpMessage.DeliveryTag.Array).ToString(),
                            Records = records,
                            UserId = Encoding.UTF8.GetString(amqpMessage.Properties.UserId.Array, amqpMessage.Properties.UserId.Offset, amqpMessage.Properties.UserId.Count)
                        };
                    }
                }

                return null;
            }
            catch (Exception exception)
            {
                if (exception.IsFatal())
                {
                    throw;
                }

                throw AmqpClientHelper.ToIotHubClientContract(exception);
            }
        }

        Task<ReceivingAmqpLink> CreateReceivingLinkAsync(TimeSpan timeout)
        {
            return this.iotHubConnection.CreateReceivingLink(this.receivingPath, timeout, 0);
        }

        public override Task CompleteAsync(FeedbackBatch feedback)
        {
            return AmqpClientHelper.DisposeMessageAsync(
                this.faultTolerantReceivingLink,
                feedback.LockToken,
                AmqpConstants.AcceptedOutcome,
                true);
        }

        public override Task AbandonAsync(FeedbackBatch feedback)
        {
            return AmqpClientHelper.DisposeMessageAsync(
                this.faultTolerantReceivingLink,
                feedback.LockToken,
                AmqpConstants.ReleasedOutcome,
                true);
        }
        
        /// <inheritdoc/>
        public void Dispose()
        {
            this.faultTolerantReceivingLink.Dispose();
        }
    }
}
