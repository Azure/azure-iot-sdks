// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices
{
    using System;
    using System.IO;
    using System.Text;
    using System.Threading.Tasks;
    using Microsoft.Azure.Amqp;
    using Microsoft.Azure.Amqp.Framing;
    using Microsoft.Azure.Devices.Common;
    using Microsoft.Azure.Devices.Common.Exceptions;
    using Microsoft.Azure.Devices.Common.Extensions;
    using Newtonsoft.Json;

    class AmqpClientHelper
    {
        public delegate object ParseFunc<in T>(AmqpMessage amqpMessage, T data);

        public static Exception ToIotHubClientContract(Exception exception)
        {
            if (exception is TimeoutException)
            {
                return new IotHubCommunicationException(exception.Message);
            }
            else if (exception is UnauthorizedAccessException)
            {
                return new UnauthorizedException(exception.Message);
            }
            else
            {
                var amqpException = exception as AmqpException;
                if (amqpException != null)
                {
                    return AmqpErrorMapper.ToIotHubClientContract(amqpException.Error);
                }

                return exception;
            }
        }

        internal static string GetReceivingPath(EndpointKind endpointKind)
        {
            string path;
            switch (endpointKind)
            {
                case EndpointKind.Feedback:
                    path = "/messages/serviceBound/feedback";
                    break;

                case EndpointKind.FileNotification:
                    path = "/messages/serviceBound/filenotifications";
                    break;

                default:
                    throw new ArgumentException("Invalid endpoint kind to receive messages from Service endpoints", "endpointKind");
            }

            return path;
        }

        internal static async Task DisposeMessageAsync(FaultTolerantAmqpObject<ReceivingAmqpLink> faultTolerantReceivingLink, string lockToken, Outcome outcome, bool batchable)
        {
            var deliveryTag = IotHubConnection.ConvertToDeliveryTag(lockToken);

            Outcome disposeOutcome;
            try
            {
                ReceivingAmqpLink deviceBoundReceivingLink = await faultTolerantReceivingLink.GetReceivingLinkAsync().ConfigureAwait(false);
                disposeOutcome = await deviceBoundReceivingLink.DisposeMessageAsync(deliveryTag, outcome, batchable, IotHubConnection.DefaultOperationTimeout).ConfigureAwait(false);
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

        public static void ValidateContentType(AmqpMessage amqpMessage, string expectedContentType)
        {
            string contentType = amqpMessage.Properties.ContentType.ToString();
            if (!string.Equals(contentType, expectedContentType, StringComparison.OrdinalIgnoreCase))
            {
                throw new InvalidOperationException("Unsupported content type: {0}".FormatInvariant(contentType));
            }
        }

        public static async Task<T> GetObjectFromAmqpMessageAsync<T>(AmqpMessage amqpMessage)
        {
            using (var reader = new StreamReader(amqpMessage.BodyStream, Encoding.UTF8))
            {
                string jsonString = await reader.ReadToEndAsync().ConfigureAwait(false);
                return JsonConvert.DeserializeObject<T>(jsonString);
            }
        }
    }
}
