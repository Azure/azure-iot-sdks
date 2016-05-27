// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices
{
    using System;
    using System.Threading.Tasks;
    using Microsoft.Azure.Amqp;
    using Microsoft.Azure.Devices.Common;
    using Microsoft.Azure.Devices.Common.Extensions;

    sealed class AmqpFileNotificationReceiver : FileNotificationReceiver<FileNotification>, IDisposable
    {
        readonly FaultTolerantAmqpObject<ReceivingAmqpLink> faultTolerantReceivingLink;
        readonly string receivingPath;

        public AmqpFileNotificationReceiver(IotHubConnection iotHubConnection)
        {
            this.Connection = iotHubConnection;
            this.OpenTimeout = IotHubConnection.DefaultOpenTimeout;
            this.OperationTimeout = IotHubConnection.DefaultOperationTimeout;
            this.receivingPath = AmqpClientHelper.GetReceivingPath(EndpointKind.FileNotification);
            this.faultTolerantReceivingLink = new FaultTolerantAmqpObject<ReceivingAmqpLink>(this.CreateReceivingLinkAsync, this.Connection.CloseLink);
        }

        public TimeSpan OpenTimeout { get; }

        public TimeSpan OperationTimeout { get; }

        public IotHubConnection Connection { get; }

        public Task OpenAsync()
        {
            return this.faultTolerantReceivingLink.GetReceivingLinkAsync();
        }

        public Task CloseAsync()
        {
            return this.faultTolerantReceivingLink.CloseAsync();
        }

        public override Task<FileNotification> ReceiveAsync()
        {
            return this.ReceiveAsync(this.OperationTimeout);
        }

        public override async Task<FileNotification> ReceiveAsync(TimeSpan timeout)
        {
            try
            {
                ReceivingAmqpLink receivingLink = await this.faultTolerantReceivingLink.GetReceivingLinkAsync();
                AmqpMessage amqpMessage = await receivingLink.ReceiveMessageAsync(timeout);

                if (amqpMessage != null)
                {
                    using (amqpMessage)
                    {
                        AmqpClientHelper.ValidateContentType(amqpMessage, CommonConstants.FileNotificationContentType);

                        var fileNotification = await AmqpClientHelper.GetObjectFromAmqpMessageAsync<FileNotification>(amqpMessage);
                        fileNotification.LockToken = new Guid(amqpMessage.DeliveryTag.Array).ToString();

                        return fileNotification;
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
            return this.Connection.CreateReceivingLink(this.receivingPath, timeout, 0);
        }

        public override Task CompleteAsync(FileNotification fileNotification)
        {
            return AmqpClientHelper.DisposeMessageAsync(
                this.faultTolerantReceivingLink,
                fileNotification.LockToken,
                AmqpConstants.AcceptedOutcome,
                false);
        }

        public override Task AbandonAsync(FileNotification fileNotification)
        {
            return AmqpClientHelper.DisposeMessageAsync(
                this.faultTolerantReceivingLink, 
                fileNotification.LockToken,
                AmqpConstants.ReleasedOutcome, 
                false);
        }
        
        /// <inheritdoc/>
        public void Dispose()
        {
            this.faultTolerantReceivingLink.Dispose();
        }
    }
}
