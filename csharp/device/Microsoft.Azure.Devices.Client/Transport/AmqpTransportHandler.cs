﻿// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client.Transport
{
    using System;
    using System.Collections.Generic;
    using System.Globalization;
    using System.Threading.Tasks;
    using System.Web;
    using Microsoft.Azure.Amqp;
    using Microsoft.Azure.Amqp.Framing;
    using Microsoft.Azure.Devices.Client.Exceptions;
    using Microsoft.Azure.Devices.Client.Extensions;

    sealed class AmqpTransportHandler : TransportHandlerBase
    {
        static readonly IotHubConnectionCache tcpConnectionCache = new IotHubConnectionCache(AccessRights.DeviceConnect);
        static readonly IotHubConnectionCache wsConnectionCache = new IotHubConnectionCache(AccessRights.DeviceConnect);
        readonly string deviceId;
        readonly Client.FaultTolerantAmqpObject<SendingAmqpLink> faultTolerantEventSendingLink;
        readonly Client.FaultTolerantAmqpObject<ReceivingAmqpLink> faultTolerantDeviceBoundReceivingLink;
        readonly IotHubConnection IotHubConnection;
        readonly TimeSpan openTimeout;
        readonly TimeSpan operationTimeout;
        readonly uint prefetchCount;
        readonly TransportType transportType;

        int eventsDeliveryTag;

        public AmqpTransportHandler(IotHubConnectionString connectionString, AmqpTransportSettings transportSettings)
        {
            this.transportType = transportSettings.GetTransportType();
            switch (this.transportType)
            {
                case TransportType.Amqp_Tcp_Only:
                    this.IotHubConnection = tcpConnectionCache.GetConnection(connectionString, transportSettings);
                    break;
                case TransportType.Amqp_WebSocket_Only:
                    this.IotHubConnection = wsConnectionCache.GetConnection(connectionString, transportSettings);
                    break;
                default:
                    throw new InvalidOperationException("Invalid Transport Type {0}".FormatInvariant(this.transportType));
            }
            
            this.deviceId = connectionString.DeviceId;
            this.openTimeout = IotHubConnection.DefaultOpenTimeout;
            this.operationTimeout = IotHubConnection.DefaultOperationTimeout;
            this.DefaultReceiveTimeout = IotHubConnection.DefaultOperationTimeout;
            this.faultTolerantEventSendingLink = new Client.FaultTolerantAmqpObject<SendingAmqpLink>(this.CreateEventSendingLinkAsync, this.IotHubConnection.CloseLink);
            this.faultTolerantDeviceBoundReceivingLink = new Client.FaultTolerantAmqpObject<ReceivingAmqpLink>(this.CreateDeviceBoundReceivingLinkAsync, this.IotHubConnection.CloseLink);
            this.prefetchCount = transportSettings.PrefetchCount;
        }

        /// <summary>
        /// Create a DeviceClient from individual parameters
        /// </summary>
        /// <param name="hostname">The fully-qualified DNS hostname of IoT Hub</param>
        /// <param name="authenticationMethod">The authentication method that is used</param>
        /// <returns>DeviceClient</returns>
        public static AmqpTransportHandler Create(string hostname, IAuthenticationMethod authenticationMethod)
        {
            if (hostname == null)
            {
                throw new ArgumentNullException(nameof(hostname));
            }

            if (authenticationMethod == null)
            {
                throw new ArgumentNullException(nameof(authenticationMethod));
            }

            IotHubConnectionStringBuilder connectionStringBuilder = IotHubConnectionStringBuilder.Create(hostname, authenticationMethod);
            return CreateFromConnectionString(connectionStringBuilder.ToString());
        }

        /// <summary>
        /// Create DeviceClient from the specified connection string
        /// </summary>
        /// <param name="connectionString">Connection string for the IoT hub</param>
        /// <returns>DeviceClient</returns>
        public static AmqpTransportHandler CreateFromConnectionString(string connectionString)
        {
            if (connectionString == null)
            {
                throw new ArgumentNullException("connectionString");
            }

            var iotHubConnectionString = IotHubConnectionString.Parse(connectionString);
            return new AmqpTransportHandler(iotHubConnectionString, new AmqpTransportSettings(TransportType.Amqp_Tcp_Only));
        }

        // This Finalizer gets cancelled when/if the user calls CloseAsync.
        ~AmqpTransportHandler()
        {
            // If the user failed to call CloseAsync make sure the connection's reference count gets updated.
            this.CloseAsync().Fork();
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
                return this.IotHubConnection;
            }
        }

        public AmqpLink EventSendingLink
        {
            get
            {
                return this.faultTolerantEventSendingLink.Value;
            }
        }

        public AmqpLink DeviceBoundReceivingLink
        {
            get
            {
                return this.faultTolerantDeviceBoundReceivingLink.Value;
            }
        }

        protected override TimeSpan DefaultReceiveTimeout { get; set; }

        protected override async Task OnOpenAsync(bool explicitOpen)
        {
            if (!explicitOpen)
            {
                return;
            }

            try
            {
                await Task.WhenAll(
                    this.faultTolerantEventSendingLink.OpenAsync(this.OpenTimeout),
                    this.faultTolerantDeviceBoundReceivingLink.OpenAsync(this.OpenTimeout));
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

        protected override Task OnCloseAsync()
        {
            GC.SuppressFinalize(this);
            switch (this.transportType)
            {
                case TransportType.Amqp_Tcp_Only:
                    return tcpConnectionCache.ReleaseConnectionAsync(this.Connection);
                case TransportType.Amqp_WebSocket_Only:
                    return wsConnectionCache.ReleaseConnectionAsync(this.Connection);
                default:
                    throw new InvalidOperationException("Invalid Transport Type {0}".FormatInvariant(this.transportType));
            }
        }

        protected async override Task OnSendEventAsync(Message message)
        {
            Outcome outcome;
            using (AmqpMessage amqpMessage = message.ToAmqpMessage())
            {
                outcome = await this.SendAmqpMessageAsync(amqpMessage);
            }

            if (outcome.DescriptorCode != Accepted.Code)
            {
                throw AmqpErrorMapper.GetExceptionFromOutcome(outcome);
            }
        }

        protected override async Task OnSendEventAsync(IEnumerable<Message> messages)
        {
            // List to hold messages in Amqp friendly format
            var messageList = new List<Data>();

            foreach (var message in messages)
            {
                using (AmqpMessage amqpMessage = message.ToAmqpMessage())
                {
                    var data = new Data() { Value = MessageConverter.ReadStream(amqpMessage.ToStream()) };
                    messageList.Add(data);
                }
            }

            Outcome outcome;
            using (var amqpMessage = AmqpMessage.Create(messageList))
            {
                amqpMessage.MessageFormat = AmqpConstants.AmqpBatchedMessageFormat;
                outcome = await this.SendAmqpMessageAsync(amqpMessage);
            }

            if (outcome.DescriptorCode != Accepted.Code)
            {
                throw AmqpErrorMapper.GetExceptionFromOutcome(outcome);
            }
        }

        protected async override Task<Message> OnReceiveAsync(TimeSpan timeout)
        {
            AmqpMessage amqpMessage;
            try
            {
                ReceivingAmqpLink deviceBoundReceivingLink = await this.GetDeviceBoundReceivingLinkAsync();
                amqpMessage = await deviceBoundReceivingLink.ReceiveMessageAsync(timeout);
            }
            catch (Exception exception)
            {
                if (exception.IsFatal())
                {
                    throw;
                }

                throw AmqpClientHelper.ToIotHubClientContract(exception);
            }

            Message message;
            if (amqpMessage != null)
            {
                message = new Message(amqpMessage)
                {
                    LockToken = new Guid(amqpMessage.DeliveryTag.Array).ToString()
                };
            }
            else
            {
                message = null;
            }

            return message;
        }

        protected override Task OnCompleteAsync(string lockToken)
        {
            return this.DisposeMessageAsync(lockToken, AmqpConstants.AcceptedOutcome);
        }

        protected override Task OnAbandonAsync(string lockToken)
        {
            return this.DisposeMessageAsync(lockToken, AmqpConstants.ReleasedOutcome);
        }

        protected override Task OnRejectAsync(string lockToken)
        {
            return this.DisposeMessageAsync(lockToken, AmqpConstants.RejectedOutcome);
        }

        protected override Task OnRejectAsync(Message message)
        {
            if (message == null)
            {
                throw Fx.Exception.ArgumentNull("message");
            }

            return this.DisposeMessageAsync(message.LockToken, AmqpConstants.RejectedOutcome);
        }

       async Task<Outcome> SendAmqpMessageAsync(AmqpMessage amqpMessage)
       {
            Outcome outcome;
            try
            {
                SendingAmqpLink eventSendingLink = await this.GetEventSendingLinkAsync();
                outcome = await eventSendingLink.SendMessageAsync(amqpMessage, IotHubConnection.GetNextDeliveryTag(ref this.eventsDeliveryTag), AmqpConstants.NullBinary, this.OperationTimeout);
            }
            catch (Exception exception)
            {
                if (exception.IsFatal())
                {
                    throw;
                }

                throw AmqpClientHelper.ToIotHubClientContract(exception);
            }

            return outcome;
       } 

        async Task DisposeMessageAsync(string lockToken, Outcome outcome)
        {
            var deliveryTag = IotHubConnection.ConvertToDeliveryTag(lockToken);

            Outcome disposeOutcome;
            try
            {
                ReceivingAmqpLink deviceBoundReceivingLink = await this.GetDeviceBoundReceivingLinkAsync();
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
                if (disposeOutcome.DescriptorCode == Rejected.Code)
                {
                    var rejected = (Rejected)disposeOutcome;

                    // Special treatment for NotFound amqp rejected error code in case of DisposeMessage 
                    if (rejected.Error != null && rejected.Error.Condition.Equals(AmqpErrorCode.NotFound))
                    {
                        throw new DeviceMessageLockLostException(rejected.Error.Description);
                    }
                }

                throw AmqpErrorMapper.GetExceptionFromOutcome(disposeOutcome);
            }
        }

        async Task<SendingAmqpLink> GetEventSendingLinkAsync()
        {
            SendingAmqpLink eventSendingLink;
            if (!this.faultTolerantEventSendingLink.TryGetOpenedObject(out eventSendingLink))
            {
                eventSendingLink = await this.faultTolerantEventSendingLink.GetOrCreateAsync(this.OpenTimeout);
            }
            return eventSendingLink;
        }

        async Task<SendingAmqpLink> CreateEventSendingLinkAsync(TimeSpan timeout)
        {
            string path = string.Format(CultureInfo.InvariantCulture, "/devices/{0}/messages/events", HttpUtility.UrlEncode(this.deviceId));

            return await this.IotHubConnection.CreateSendingLinkAsync(path, timeout);
        }

        async Task<ReceivingAmqpLink> GetDeviceBoundReceivingLinkAsync()
        {
            ReceivingAmqpLink deviceBoundReceivingLink;
            if (!this.faultTolerantDeviceBoundReceivingLink.TryGetOpenedObject(out deviceBoundReceivingLink))
            {
                deviceBoundReceivingLink = await this.faultTolerantDeviceBoundReceivingLink.GetOrCreateAsync(this.OpenTimeout);
            }

            return deviceBoundReceivingLink;
        }

        async Task<ReceivingAmqpLink> CreateDeviceBoundReceivingLinkAsync(TimeSpan timeout)
        {
            string path = string.Format(CultureInfo.InvariantCulture, "/devices/{0}/messages/deviceBound", HttpUtility.UrlEncode(this.deviceId));

            return await this.IotHubConnection.CreateReceivingLink(path, timeout, this.prefetchCount);
        }
    }
}
