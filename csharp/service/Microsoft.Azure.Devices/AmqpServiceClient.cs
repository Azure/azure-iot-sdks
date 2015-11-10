﻿// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices
{
    using System;
    using System.Collections.Generic;
    using System.Net;
    using System.Net.Http;
    using System.Threading;
    using System.Threading.Tasks;
    using System.Web;
    using Microsoft.Azure.Amqp;
    using Microsoft.Azure.Amqp.Framing;
    using Microsoft.Azure.Devices.Common;
    using Microsoft.Azure.Devices.Common.Data;
    using Microsoft.Azure.Devices.Common.Exceptions;

    sealed class AmqpServiceClient : ServiceClient, IDisposable
    {
        static readonly TimeSpan DefaultOperationTimeout = TimeSpan.FromSeconds(100);
        const string StatisticsUriFormat = "/statistics/service?" + ClientApiVersionHelper.ApiVersionQueryString;

        readonly IotHubConnection iotHubConnection;
        readonly TimeSpan openTimeout;
        readonly TimeSpan operationTimeout;
        readonly FaultTolerantAmqpObject<SendingAmqpLink> faultTolerantSendingLink;
        readonly string sendingPath;
        readonly string receivingPath;
        readonly AmqpFeedbackReceiver feedbackReceiver;
        readonly IHttpClientHelper httpClientHelper;
        readonly string iotHubName;

        int sendingDeliveryTag;

        public AmqpServiceClient(IotHubConnectionString iotHubConnectionString)
        {
            var iotHubConnection = new IotHubConnection(iotHubConnectionString, AccessRights.ServiceConnect);
            this.iotHubConnection = iotHubConnection;
            this.openTimeout = IotHubConnection.DefaultOpenTimeout;
            this.operationTimeout = IotHubConnection.DefaultOperationTimeout;
            this.sendingPath = "/messages/deviceBound";
            this.faultTolerantSendingLink = new FaultTolerantAmqpObject<SendingAmqpLink>(this.CreateSendingLinkAsync, this.iotHubConnection.CloseLink);
            this.feedbackReceiver = new AmqpFeedbackReceiver(this.iotHubConnection);
            this.iotHubName = iotHubConnectionString.IotHubName;
            this.httpClientHelper = new HttpClientHelper(
                iotHubConnectionString.HttpsEndpoint,
                iotHubConnectionString,
                ExceptionHandlingHelper.GetDefaultErrorMapping(),
                DefaultOperationTimeout,
                client => {});
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

        public SendingAmqpLink SendingLink
        {
            get
            {
                SendingAmqpLink sendingLink;
                this.faultTolerantSendingLink.TryGetOpenedObject(out sendingLink);
                return sendingLink;
            }
        }

        public override async Task OpenAsync()
        {
            await this.GetSendingLinkAsync();
            await this.feedbackReceiver.OpenAsync();
        }

        public async override Task CloseAsync()
        {
            await this.feedbackReceiver.CloseAsync();
            await this.iotHubConnection.CloseAsync();
        }

        public async override Task SendAsync(string deviceId, Message message)
        {
            if (string.IsNullOrWhiteSpace(deviceId))
            {
                throw new ArgumentException("Value should be non null and non empty", "deviceId");
            }

            if (message == null)
            {
                throw new ArgumentNullException("message");
            }

            Outcome outcome;
            using (AmqpMessage amqpMessage = message.ToAmqpMessage())
            {
                amqpMessage.Properties.To = "/devices/" + HttpUtility.UrlEncode(deviceId) + "/messages/deviceBound";
                try
                {
                    SendingAmqpLink sendingLink = await this.GetSendingLinkAsync();
                    outcome = await sendingLink.SendMessageAsync(amqpMessage, IotHubConnection.GetNextDeliveryTag(ref this.sendingDeliveryTag), AmqpConstants.NullBinary, this.OperationTimeout);
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

            if (outcome.DescriptorCode != Accepted.Code)
            {
                throw AmqpErrorMapper.GetExceptionFromOutcome(outcome);
            }
        }

        public override FeedbackReceiver<FeedbackBatch> GetFeedbackReceiver()
        {
            return this.feedbackReceiver;
        }

        public override Task<ServiceStatistics> GetServiceStatisticsAsync()
        {
            return this.GetServiceStatisticsAsync(CancellationToken.None);
        }

        public override Task<ServiceStatistics> GetServiceStatisticsAsync(CancellationToken cancellationToken)
        {
            var errorMappingOverrides = new Dictionary<HttpStatusCode, Func<HttpResponseMessage, Task<Exception>>>();
            errorMappingOverrides.Add(HttpStatusCode.NotFound, responseMessage => Task.FromResult((Exception)new IotHubNotFoundException(this.iotHubName)));
            return this.httpClientHelper.GetAsync<ServiceStatistics>(GetStatisticsUri(), errorMappingOverrides, null, cancellationToken);
        }

        async Task<SendingAmqpLink> GetSendingLinkAsync()
        {
            SendingAmqpLink sendingLink;
            if (!this.faultTolerantSendingLink.TryGetOpenedObject(out sendingLink))
            {
                sendingLink = await this.faultTolerantSendingLink.GetOrCreateAsync(this.OpenTimeout);
            }

            return sendingLink;
        }

        Task<SendingAmqpLink> CreateSendingLinkAsync(TimeSpan timeout)
        {
            return this.iotHubConnection.CreateSendingLinkAsync(this.sendingPath, timeout);
        }

        /// <inheritdoc/>
        public void Dispose()
        {
            this.faultTolerantSendingLink.Dispose();
            this.feedbackReceiver.Dispose();
            this.httpClientHelper.Dispose();
        }

        static Uri GetStatisticsUri()
        {
            return new Uri(StatisticsUriFormat, UriKind.Relative);
        }
    }
}
