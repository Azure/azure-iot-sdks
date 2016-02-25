﻿// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client.Transport
{
    using System;
    using System.Collections.Generic;
    using System.Globalization;
    using System.IO;
    using System.Linq;
    using System.Net;
    using System.Net.Http;
    using System.Text;
    using System.Threading;
    using System.Threading.Tasks;
    using Microsoft.Azure.Devices.Client.Exceptions;
    using Microsoft.Azure.Devices.Client.Extensions;
    using Newtonsoft.Json;

    sealed class HttpTransportHandler : TransportHandlerBase
    {
        static readonly TimeSpan DefaultOperationTimeout = TimeSpan.FromSeconds(60);
        static readonly TimeSpan DefaultReceiveTimeoutInSeconds = TimeSpan.FromSeconds(0);
        static readonly IDictionary<string, string> MapMessageProperties2HttpHeaders = new Dictionary<string, string>()
            {
                { MessageSystemPropertyNames.Ack, CustomHeaderConstants.Ack },
                { MessageSystemPropertyNames.CorrelationId, CustomHeaderConstants.CorrelationId },
                { MessageSystemPropertyNames.ExpiryTimeUtc, CustomHeaderConstants.ExpiryTimeUtc },
                { MessageSystemPropertyNames.MessageId, CustomHeaderConstants.MessageId },
                { MessageSystemPropertyNames.Operation, CustomHeaderConstants.Operation },
                { MessageSystemPropertyNames.To, CustomHeaderConstants.To },
                { MessageSystemPropertyNames.UserId, CustomHeaderConstants.UserId },
            };

        readonly IHttpClientHelper httpClientHelper;
        readonly string deviceId;

#if !WINDOWS_UWP
        readonly Http1TransportSettings transportSettings;
#endif

        internal HttpTransportHandler(IotHubConnectionString iotHubConnectionString)
        {
            this.deviceId = iotHubConnectionString.DeviceId;
            this.httpClientHelper = new HttpClientHelper(
                iotHubConnectionString.HttpsEndpoint,
                iotHubConnectionString,
                ExceptionHandlingHelper.GetDefaultErrorMapping(),
                DefaultOperationTimeout,
                null);
            this.DefaultReceiveTimeout = DefaultReceiveTimeoutInSeconds;
        }

#if !WINDOWS_UWP
        internal HttpTransportHandler(IotHubConnectionString iotHubConnectionString, Http1TransportSettings transportSettings)
            :this(iotHubConnectionString)
        {
            this.transportSettings = transportSettings;
        }
#endif

        /// <summary>
        /// Create a DeviceClient from individual parameters
        /// </summary>
        /// <param name="hostname">The fully-qualified DNS hostname of IoT Hub</param>
        /// <param name="authMethod">The authentication method that is used</param>
        /// <returns>DeviceClient</returns>
        public static HttpTransportHandler Create(string hostname, IAuthenticationMethod authMethod)
        {
            if (hostname == null)
            {
                throw new ArgumentNullException("hostname");
            }

            if (authMethod == null)
            {
                throw new ArgumentNullException("authMethod");
            }

            var connectionStringBuilder = IotHubConnectionStringBuilder.Create(hostname, authMethod);
            return CreateFromConnectionString(connectionStringBuilder.ToString());
        }

        /// <summary>
        /// Create DeviceClient from the specified connection string
        /// </summary>
        /// <param name="connectionString">Connection string for the IoT hub</param>
        /// <returns>DeviceClient</returns>
        public static HttpTransportHandler CreateFromConnectionString(string connectionString)
        {
            if (connectionString == null)
            {
                throw new ArgumentNullException("connectionString");
            }

            var iotHubConnectionString = IotHubConnectionString.Parse(connectionString);
            return new HttpTransportHandler(iotHubConnectionString);
        }

        protected override TimeSpan DefaultReceiveTimeout { get; set; }

        protected override Task OnOpenAsync(bool explicitOpen)
        {
            return TaskHelpers.CompletedTask;
        }

        protected override Task OnCloseAsync()
        {
            return TaskHelpers.CompletedTask;
        }

        protected override Task OnSendEventAsync(Message message)
        {
            if (message == null)
            {
                throw Fx.Exception.ArgumentNull("message");
            }
            
            var customHeaders = new Dictionary<string, string>(message.SystemProperties.Count + message.Properties.Count);
            foreach (var property in message.SystemProperties)
            {
                customHeaders.Add(MapMessageProperties2HttpHeaders[property.Key], property.Value.ToString());
            }

            foreach (var property in message.Properties)
            {
                customHeaders.Add(CustomHeaderConstants.HttpAppPropertyPrefix + property.Key, property.Value);
            }

            return this.httpClientHelper.PostAsync<byte[]>(
                GetRequestUri(this.deviceId, CommonConstants.DeviceEventPathTemplate, null), 
                message.GetBytes(), 
                ExceptionHandlingHelper.GetDefaultErrorMapping(), 
                customHeaders,
                CancellationToken.None);
        }

        protected override Task OnSendEventAsync(IEnumerable<Message> messages)
        {
            if (messages == null)
            {
                throw Fx.Exception.ArgumentNull("messages");
            }

            var customHeaders = PrepareCustomHeaders(CommonConstants.DeviceEventPathTemplate.FormatInvariant(this.deviceId), string.Empty, CommonConstants.DeviceToCloudOperation);

            string body = ToJson(messages);
            return this.httpClientHelper.PostAsync<string>(
                GetRequestUri(this.deviceId, CommonConstants.DeviceEventPathTemplate, null),
                body,
                ExceptionHandlingHelper.GetDefaultErrorMapping(),
                customHeaders,
                CancellationToken.None);
        }

        internal Task SendEventAsync(IEnumerable<string> messages)
        {
            var customHeaders = PrepareCustomHeaders(CommonConstants.DeviceEventPathTemplate.FormatInvariant(this.deviceId), string.Empty, CommonConstants.DeviceToCloudOperation);

            string body = ToJson(messages);
            return this.httpClientHelper.PostAsync<string>(
                GetRequestUri(this.deviceId, CommonConstants.DeviceEventPathTemplate, null),
                body,
                ExceptionHandlingHelper.GetDefaultErrorMapping(),
                customHeaders,
                CancellationToken.None);
        }

        internal Task SendEventAsync(IEnumerable<Tuple<string, IDictionary<string, string>>> messages)
        {
            var customHeaders = PrepareCustomHeaders(CommonConstants.DeviceEventPathTemplate.FormatInvariant(this.deviceId), string.Empty, CommonConstants.DeviceToCloudOperation);
            string body = ToJson(messages);
            return this.httpClientHelper.PostAsync<string>(
                GetRequestUri(this.deviceId, CommonConstants.DeviceEventPathTemplate, null),
                body,
                ExceptionHandlingHelper.GetDefaultErrorMapping(),
                customHeaders,
                CancellationToken.None);
        }

        protected async override Task<Message> OnReceiveAsync(TimeSpan timeout)
        {
            // Long-polling is not supported
            if (!TimeSpan.Zero.Equals(timeout))
            {
                throw new ArgumentOutOfRangeException("timeout", "Http Protocol does not support a non-zero receive timeout");
            }

            IDictionary<string, string> customHeaders = PrepareCustomHeaders(CommonConstants.DeviceBoundPathTemplate.FormatInvariant(this.deviceId), null, CommonConstants.CloudToDeviceOperation);
            IDictionary<string, string> queryValueDictionary =
                new Dictionary<string, string>() { { CustomHeaderConstants.MessageLockTimeout, DefaultOperationTimeout.TotalSeconds.ToString(CultureInfo.InvariantCulture) } };

            var responseMessage = await this.httpClientHelper.GetAsync<HttpResponseMessage>(
                GetRequestUri(this.deviceId, CommonConstants.DeviceBoundPathTemplate, queryValueDictionary),
                ExceptionHandlingHelper.GetDefaultErrorMapping(),
                customHeaders,
                true,
                CancellationToken.None);

            if (responseMessage.StatusCode == HttpStatusCode.NoContent)
            {
                return null;
            }

            IEnumerable<string> messageId;
            responseMessage.Headers.TryGetValues(CustomHeaderConstants.MessageId, out messageId);

            IEnumerable<string> lockToken;
            responseMessage.Headers.TryGetValues(HttpResponseHeader.ETag.ToString(), out lockToken);

            IEnumerable<string> enqueuedTime;
            responseMessage.Headers.TryGetValues(CustomHeaderConstants.EnqueuedTime, out enqueuedTime);

            IEnumerable<string> deliveryCountAsStr;
            responseMessage.Headers.TryGetValues(CustomHeaderConstants.DeliveryCount, out deliveryCountAsStr);

            IEnumerable<string> expiryTime;
            responseMessage.Headers.TryGetValues(CustomHeaderConstants.ExpiryTimeUtc, out expiryTime);

            IEnumerable<string> correlationId;
            responseMessage.Headers.TryGetValues(CustomHeaderConstants.CorrelationId, out correlationId);

            IEnumerable<string> sequenceNumber;
            responseMessage.Headers.TryGetValues(CustomHeaderConstants.SequenceNumber, out sequenceNumber);

            var byteContent = await responseMessage.Content.ReadAsByteArrayAsync();

            var message = byteContent != null ? new Message(byteContent) : new Message();

            message.MessageId = messageId != null ? messageId.First() : null;
            message.LockToken = lockToken != null ? lockToken.First().Trim('\"') : null;

            if (enqueuedTime != null)
            {
                DateTime enqueuedTimeUtc;
                if (DateTime.TryParse(enqueuedTime.First(), out enqueuedTimeUtc))
                {
                    message.EnqueuedTimeUtc = enqueuedTimeUtc;
                }
            }

            if (deliveryCountAsStr != null)
            {
                byte deliveryCount;
                if (byte.TryParse(deliveryCountAsStr.First(), out deliveryCount))
                {
                    message.DeliveryCount = deliveryCount;
                }
            }

            if (expiryTime != null)
            {
                DateTime absoluteExpiryTime;
                if (DateTime.TryParse(expiryTime.First(), out absoluteExpiryTime))
                {
                    message.ExpiryTimeUtc = absoluteExpiryTime;
                }
            }

            message.CorrelationId = correlationId != null ? correlationId.First() : null;
            message.SequenceNumber = sequenceNumber != null ? Convert.ToUInt64(sequenceNumber.First()) : 0;

            // Read custom headers and map them to properties.
            foreach (KeyValuePair<string, IEnumerable<string>> keyValue in responseMessage.Headers)
            {
                if (keyValue.Key.StartsWith(CustomHeaderConstants.HttpAppPropertyPrefix, StringComparison.OrdinalIgnoreCase))
                {
                    message.Properties.Add(keyValue.Key.Substring(CustomHeaderConstants.HttpAppPropertyPrefix.Length), keyValue.Value.First());
                }
            }

            return message;
        }

        protected override Task OnCompleteAsync(string lockToken)
        {
            var customHeaders = PrepareCustomHeaders(
                CommonConstants.DeviceBoundPathCompleteTemplate.FormatInvariant(this.deviceId, lockToken),
                null,
                CommonConstants.CloudToDeviceOperation);

            var eTag = new ETagHolder { ETag = lockToken };

            return this.httpClientHelper.DeleteAsync(
                GetRequestUri(this.deviceId, CommonConstants.DeviceBoundPathTemplate + "/{0}".FormatInvariant(lockToken), null),
                eTag,
                ExceptionHandlingHelper.GetDefaultErrorMapping(),
                customHeaders,
                CancellationToken.None);
        }

        protected override Task OnAbandonAsync(string lockToken)
        {
            var customHeaders = PrepareCustomHeaders(
                CommonConstants.DeviceBoundPathAbandonTemplate.FormatInvariant(this.deviceId, lockToken),
                null,
                CommonConstants.CloudToDeviceOperation);

            // Even though If-Match is not a customHeader, add it here for convenience
            customHeaders.Add(HttpRequestHeader.IfMatch.ToString(), lockToken);

            return this.httpClientHelper.PostAsync(
                GetRequestUri(this.deviceId, CommonConstants.DeviceBoundPathTemplate + "/{0}/abandon".FormatInvariant(lockToken), null),
                (Object)null,
                ExceptionHandlingHelper.GetDefaultErrorMapping(),
                customHeaders,
                CancellationToken.None);
        }

        protected override Task OnRejectAsync(string lockToken)
        {
            var customHeaders = PrepareCustomHeaders(
                CommonConstants.DeviceBoundPathRejectTemplate.FormatInvariant(this.deviceId, lockToken),
                null,
                CommonConstants.CloudToDeviceOperation);

            var eTag = new ETagHolder { ETag = lockToken };

            return this.httpClientHelper.DeleteAsync(
                GetRequestUri(this.deviceId, CommonConstants.DeviceBoundPathTemplate + "/{0}".FormatInvariant(lockToken), new Dictionary<string, string> { { "reject", null } }),
                eTag,
                ExceptionHandlingHelper.GetDefaultErrorMapping(),
                customHeaders,
                CancellationToken.None);
        }

        static IDictionary<string, string> PrepareCustomHeaders(string toHeader, string messageId, string operation)
        {
            var customHeaders = new Dictionary<string, string>
            {   
                { CustomHeaderConstants.To, toHeader },
                { CustomHeaderConstants.Operation, operation },
            };

            if (!string.IsNullOrEmpty(messageId))
            {
                customHeaders.Add(CustomHeaderConstants.MessageId, messageId);
            }

            return customHeaders;
        }

        static Uri GetRequestUri(string deviceId, string path, IDictionary<string, string> queryValueDictionary)
        {
            deviceId = WebUtility.UrlEncode(deviceId);

            var stringBuilder = new StringBuilder("{0}?{1}".FormatInvariant(path.FormatInvariant(deviceId), ClientApiVersionHelper.ApiVersionQueryString));

            if (queryValueDictionary != null)
            {
                foreach (var queryValue in queryValueDictionary)
                {
                    if (string.IsNullOrEmpty(queryValue.Value))
                    {
                        stringBuilder.Append("&{0}".FormatInvariant(queryValue.Key));
                    }
                    else
                    {
                        stringBuilder.Append("&{0}={1}".FormatInvariant(queryValue.Key, queryValue.Value));
                    }
                }
            }

            return new Uri(stringBuilder.ToString(), UriKind.Relative);
        }

        static string ToJson(IEnumerable<Message> messages)
        {
            using (var sw = new StringWriter())
            using (var writer = new JsonTextWriter(sw))
            {
                // [
                writer.WriteStartArray();

                foreach (Message message in messages)
                {
                    // {
                    writer.WriteStartObject();

                    // always encode body as Base64 string
                    writer.WritePropertyName("body");
                    writer.WriteValue(Convert.ToBase64String(message.GetBytes()));

                    // skip base64Encoded property since the default is true

                    // "properties" :
                    writer.WritePropertyName("properties");

                    // {
                    writer.WriteStartObject();

                    foreach (var property in message.SystemProperties)
                    {
                        writer.WritePropertyName(MapMessageProperties2HttpHeaders[property.Key]);
                        writer.WriteValue(property.Value);
                    }

                    foreach (var property in message.Properties)
                    {
                        writer.WritePropertyName(CustomHeaderConstants.HttpAppPropertyPrefix + property.Key);
                        writer.WriteValue(property.Value);
                    }

                    // }
                    writer.WriteEndObject();

                    // }
                    writer.WriteEndObject();
                }

                writer.WriteEndArray();
                // ]

                return sw.ToString();                
            }
        }

        static string ToJson(IEnumerable<string> messages)
        {
            using (var sw = new StringWriter())
            using (var writer = new JsonTextWriter(sw))
            {
                // [
                writer.WriteStartArray();

                foreach (var message in messages)
                {
                    // {
                    writer.WriteStartObject();

                    // "body" : "{\"my\": \"message\", \"is\": \"json\"}"
                    writer.WritePropertyName("body");
                    writer.WriteValue(message);

                    // "base64Encoded":false
                    writer.WritePropertyName("base64Encoded");
                    writer.WriteValue(false);

                    // }
                    writer.WriteEndObject();
                }

                writer.WriteEndArray();
                // ]

                return sw.ToString();
            }
        }

        static string ToJson(IEnumerable<Tuple<string, IDictionary<string, string>>> messages)
        {
            using (var sw = new StringWriter())
            using (var writer = new JsonTextWriter(sw))
            {
                // [
                writer.WriteStartArray();

                foreach (var message in messages)
                {
                    // {
                    writer.WriteStartObject();

                    // "body" : "{\"my\": \"message\", \"is\": \"json\"}"
                    writer.WritePropertyName("body");
                    writer.WriteValue(message.Item1);

                    // "base64Encoded":false
                    writer.WritePropertyName("base64Encoded");
                    writer.WriteValue(false);

                    // "properties" :
                    writer.WritePropertyName("properties");

                    // {
                    writer.WriteStartObject();

                    foreach (var property in message.Item2)
                    {
                        writer.WritePropertyName(property.Key); 
                        writer.WriteValue(property.Value);
                    }

                    // }
                    writer.WriteEndObject();

                    // }
                    writer.WriteEndObject();
                }

                writer.WriteEndArray();
                // ]

                return sw.ToString();
            }
        }
    }
}