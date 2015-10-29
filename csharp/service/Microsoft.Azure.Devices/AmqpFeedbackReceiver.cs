// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices
{
    using System;
    using System.Collections.Generic;
    using System.IO;
    using System.Text;
    using System.Threading.Tasks;
    using System.Web;
    using Microsoft.Azure.Amqp;
    using Microsoft.Azure.Amqp.Framing;
    using Newtonsoft.Json;

    using Microsoft.Azure.Devices.Common;
    using Microsoft.Azure.Devices.Common.Exceptions;

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
            this.receivingPath = GetReceivingPath(EndpointKind.Feedback);
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
            return this.GetReceivingLinkAsync();
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
                ReceivingAmqpLink receivingLink = await this.GetReceivingLinkAsync();
                AmqpMessage amqpMessage = await receivingLink.ReceiveMessageAsync(timeout);

                if (amqpMessage != null)
                {
                    using (amqpMessage)
                    {
                        string contentType = amqpMessage.Properties.ContentType.ToString();
                        if (!string.Equals(contentType, CommonConstants.BatchedFeedbackContentType, StringComparison.OrdinalIgnoreCase))
                        {
                            throw new InvalidOperationException("Unsupported content type: {0}".FormatInvariant(contentType));
                        }

                        using (var reader = new StreamReader(amqpMessage.BodyStream, Encoding.UTF8))
                        {
                            string jsonString = await reader.ReadToEndAsync();
                            var records = JsonConvert.DeserializeObject<IEnumerable<FeedbackRecord>>(jsonString);

                            return new FeedbackBatch
                            {
                                EnqueuedTime = (DateTime)amqpMessage.MessageAnnotations.Map[MessageSystemPropertyNames.EnqueuedTime],
                                LockToken = new Guid(amqpMessage.DeliveryTag.Array).ToString(),
                                Records = records,
                                UserId = Encoding.UTF8.GetString(amqpMessage.Properties.UserId.Array, amqpMessage.Properties.UserId.Offset, amqpMessage.Properties.UserId.Count)
                            };
                        }
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

        async Task<ReceivingAmqpLink> GetReceivingLinkAsync()
        {
            ReceivingAmqpLink receivingLink;
            if (!this.faultTolerantReceivingLink.TryGetOpenedObject(out receivingLink))
            {
                receivingLink = await this.faultTolerantReceivingLink.GetOrCreateAsync(this.OpenTimeout);
            }

            return receivingLink;
        }

        Task<ReceivingAmqpLink> CreateReceivingLinkAsync(TimeSpan timeout)
        {
            return this.iotHubConnection.CreateReceivingLink(this.receivingPath, timeout, 0);
        }

        static string GetReceivingPath(EndpointKind endpointKind)
        {
            string path;
            switch (endpointKind)
            {
                case EndpointKind.Feedback:
                    path = "/messages/serviceBound/feedback";
                    break;
                
                default:
                    throw new ArgumentException("Invalid endpoint kind to receive messages from Service endpoints", "endpointKind");
            }

            return path;
        }

        public override Task CompleteAsync(FeedbackBatch feedback)
        {
            return this.DisposeMessageAsync(feedback.LockToken, AmqpConstants.AcceptedOutcome);
        }

        public override Task AbandonAsync(FeedbackBatch feedback)
        {
             return this.DisposeMessageAsync(feedback.LockToken, AmqpConstants.ReleasedOutcome);
        }

        async Task DisposeMessageAsync(string lockToken, Outcome outcome)
        {
            var deliveryTag = IotHubConnection.ConvertToDeliveryTag(lockToken);

            Outcome disposeOutcome;
            try
            {
                ReceivingAmqpLink deviceBoundReceivingLink = await this.GetReceivingLinkAsync();
                disposeOutcome = await deviceBoundReceivingLink.DisposeMessageAsync(deliveryTag, outcome, batchable: true, timeout: this.OperationTimeout);
            }
            catch (Exception exception)
            {
                if (exception.IsFatal())
                {
                    throw;
                }

                throw AmqpClientHelper.ToIotHubClientContract(exception);
            }

            if (disposeOutcome.DescriptorCode != Accepted.Code)
            {
                throw AmqpErrorMapper.GetExceptionFromOutcome(disposeOutcome);
            }
        }
        
        /// <inheritdoc/>
        public void Dispose()
        {
            this.faultTolerantReceivingLink.Dispose();
        }
    }
}
