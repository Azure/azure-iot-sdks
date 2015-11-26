// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client
{
    using System;
    using System.IO;
    using System.Net;

    using Microsoft.Azure.Devices.Client.Extensions;
    using Microsoft.Azure.Devices.Client.Exceptions;
#if NETMF
    using System.Collections;
    using System.Text;
#else
    using System.Collections.Generic;
    using System.Globalization;
    using System.Linq;
    using System.Net.Http;
    using System.Text;
    using System.Threading;
    using System.Threading.Tasks;
    using Newtonsoft.Json;
#endif

    sealed class HttpDeviceClient : DeviceClientHelper
    {
#if NETMF
        static readonly TimeSpan DefaultOperationTimeout = new TimeSpan(0, 0, 60);
        static readonly TimeSpan DefaultReceiveTimeoutInSeconds = new TimeSpan(0);
        static Hashtable MapMessageProperties2HttpHeaders = new Hashtable();

        readonly HttpClientHelper httpClientHelper;
#else
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
#endif

        readonly string deviceId;

        internal HttpDeviceClient(IotHubConnectionString iotHubConnectionString)
        {
            this.deviceId = iotHubConnectionString.DeviceId;
            this.httpClientHelper = new HttpClientHelper(
                iotHubConnectionString.HttpsEndpoint,
                iotHubConnectionString,
#if NETMF
                DefaultOperationTimeout);
#else
                ExceptionHandlingHelper.GetDefaultErrorMapping(),
                DefaultOperationTimeout,
                null);
#endif
            this.DefaultReceiveTimeout = DefaultReceiveTimeoutInSeconds;

#if NETMF
            // fill MapMessageProperties2HttpHeaders
            MapMessageProperties2HttpHeaders.Add(MessageSystemPropertyNames.Ack, CustomHeaderConstants.Ack);
            MapMessageProperties2HttpHeaders.Add(MessageSystemPropertyNames.CorrelationId, CustomHeaderConstants.CorrelationId);
            MapMessageProperties2HttpHeaders.Add(MessageSystemPropertyNames.ExpiryTimeUtc, CustomHeaderConstants.ExpiryTimeUtc);
            MapMessageProperties2HttpHeaders.Add(MessageSystemPropertyNames.MessageId, CustomHeaderConstants.MessageId);
            MapMessageProperties2HttpHeaders.Add(MessageSystemPropertyNames.Operation, CustomHeaderConstants.Operation);
            MapMessageProperties2HttpHeaders.Add(MessageSystemPropertyNames.To, CustomHeaderConstants.To);
            MapMessageProperties2HttpHeaders.Add(MessageSystemPropertyNames.UserId, CustomHeaderConstants.UserId);

            // make sure we are using RFC4648 encoding for Base64 (.Net MF default is not RFC4648)
            Convert.UseRFC4648Encoding = true;
#endif
        }

        /// <summary>
        /// Create a DeviceClient from individual parameters
        /// </summary>
        /// <param name="hostname">The fully-qualified DNS hostname of IoT Hub</param>
        /// <param name="authMethod">The authentication method that is used</param>
        /// <returns>DeviceClient</returns>
        public static HttpDeviceClient Create(string hostname, IAuthenticationMethod authMethod)
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
        public static HttpDeviceClient CreateFromConnectionString(string connectionString)
        {
            if (connectionString == null)
            {
                throw new ArgumentNullException("connectionString");
            }

            var iotHubConnectionString = IotHubConnectionString.Parse(connectionString);
            return new HttpDeviceClient(iotHubConnectionString);
        }

        protected override TimeSpan DefaultReceiveTimeout { get; set; }

#if NETMF
        protected override void OnOpen(bool explicitOpen)
        {
            return;
        }
#else
        protected override Task OnOpenAsync(bool explicitOpen)
        {
            return TaskHelpers.CompletedTask;
        }
#endif

#if NETMF
        protected override void OnClose()
        {
        }
#else
        protected override Task OnCloseAsync()
        {
            return TaskHelpers.CompletedTask;
        }
#endif

#if NETMF
        protected override void OnSendEvent(Message message)
        {
            if (message == null)
            {
                //throw Fx.Exception.ArgumentNull("message");
            }

            Hashtable customHeaders = new Hashtable(message.SystemProperties.Count + message.Properties.Count);
            foreach (var property in message.SystemProperties.Keys)
            {
                customHeaders.Add(MapMessageProperties2HttpHeaders[property], message.SystemProperties[property] as string);
            }

            foreach (var property in message.Properties.Keys)
            {
                customHeaders.Add(CustomHeaderConstants.HttpAppPropertyPrefix + property as string, message.Properties[property] as string);
            }

            this.httpClientHelper.Post(GetRequestUri(this.deviceId, CommonConstants.DeviceEventPathTemplate, null), message.BodyStream, customHeaders);
        }
#else
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
#endif

#if !NETMF
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
#endif

#if NETMF
        protected override Message OnReceive(TimeSpan timeout)
        {
            // Long-polling is not supported
            if (!TimeSpan.Zero.Equals(timeout))
            {
                throw new ArgumentOutOfRangeException("timeout", "Http Protocol does not support a non-zero receive timeout");
            }

            var pathTemplate = new StringBuilder(CommonConstants.DeviceBoundPathTemplate);

            Hashtable customHeaders = PrepareCustomHeaders(pathTemplate.Replace("{0}", this.deviceId).ToString(), null, CommonConstants.CloudToDeviceOperation);

            // not required anymore
            pathTemplate = null;

            Hashtable queryValueDictionary = new Hashtable();

            // need to perform the timespan calculation in Ticks because .NetMF doesn't implement TimeSpan.TotalSeconds
            queryValueDictionary.Add(CustomHeaderConstants.MessageLockTimeout, (DefaultOperationTimeout.Ticks / 10000000).ToString());


            var responseMessage = this.httpClientHelper.Get(
                GetRequestUri(this.deviceId, CommonConstants.DeviceBoundPathTemplate, queryValueDictionary),
                //ExceptionHandlingHelper.GetDefaultErrorMapping(),
                customHeaders,
                true);

            if (responseMessage.StatusCode == HttpStatusCode.NoContent)
            {
                // close the WebResponse now
                responseMessage.Close();

                // done here
                return null;
            }

            string[] messageId;
            responseMessage.Headers.TryGetValues(CustomHeaderConstants.MessageId, out messageId);

            string[] lockToken;
            responseMessage.Headers.TryGetValues("ETag", out lockToken);

            string[] enqueuedTime;
            responseMessage.Headers.TryGetValues(CustomHeaderConstants.EnqueuedTime, out enqueuedTime);

            string[] deliveryCountAsStr;
            responseMessage.Headers.TryGetValues(CustomHeaderConstants.DeliveryCount, out deliveryCountAsStr);

            string[] expiryTime;
            responseMessage.Headers.TryGetValues(CustomHeaderConstants.ExpiryTimeUtc, out expiryTime);

            string[] correlationId;
            responseMessage.Headers.TryGetValues(CustomHeaderConstants.CorrelationId, out correlationId);

            string[] sequenceNumber;
            responseMessage.Headers.TryGetValues(CustomHeaderConstants.SequenceNumber, out sequenceNumber);

            StreamReader reader = new StreamReader(responseMessage.GetResponseStream());

            byte[] byteContent = Encoding.UTF8.GetBytes(reader.ReadToEnd());

            // OK to close the WebResponse now
            responseMessage.Close();

            // dispose response
            responseMessage.Dispose();


            var message = byteContent != null ? new Message(byteContent) : new Message();

            message.MessageId = messageId != null ? messageId.GetValue(0) as string : null;
            message.LockToken = lockToken != null ? (lockToken.GetValue(0) as string).Trim('\"') : null;

            if (enqueuedTime != null)
            {
                DateTime enqueuedTimeUtc;
                if ((enqueuedTime.GetValue(0) as string).TryParseDateTime(out enqueuedTimeUtc))
                {
                    message.EnqueuedTimeUtc = enqueuedTimeUtc;
                }
            }

            if (deliveryCountAsStr != null)
            {
                // .NetMF doesn't implement TryParse so will wrap the Parse with a try/catch to achive the same goal
                try
                {
                    message.DeliveryCount = byte.Parse(deliveryCountAsStr.GetValue(0) as string);

                }
                catch { };
            }

            if (expiryTime != null)
            {
                DateTime absoluteExpiryTime;
                if ((expiryTime.GetValue(0) as string).TryParseDateTime(out absoluteExpiryTime))
                {
                    message.ExpiryTimeUtc = absoluteExpiryTime;
                }
            }

            message.CorrelationId = correlationId != null ? correlationId.GetValue(0) as string : null;
            message.SequenceNumber = sequenceNumber != null ? Convert.ToUInt64(sequenceNumber.GetValue(0) as string) : 0;

            return message;
        }
#else
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
#endif

#if NETMF
        protected override void OnComplete(string lockToken)
        {
            var pathTemplate = new StringBuilder(CommonConstants.DeviceBoundPathCompleteTemplate);

            Hashtable customHeaders = PrepareCustomHeaders(pathTemplate.Replace("{1}", lockToken).ToString(), null, CommonConstants.CloudToDeviceOperation);

            var eTag = new ETagHolder { ETag = lockToken };

            Hashtable fields = new Hashtable();
            fields.Add("reject", null);

            this.httpClientHelper.Delete(
                GetRequestUri(this.deviceId, CommonConstants.DeviceBoundPathTemplate + "/" + lockToken, fields),
                eTag,
                customHeaders);
        }
#else
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
#endif

#if NETMF
        protected override void OnAbandon(string lockToken)
        {
            var pathTemplate = new StringBuilder(CommonConstants.DeviceBoundPathAbandonTemplate);

            Hashtable customHeaders = PrepareCustomHeaders(pathTemplate.Replace("{1}", lockToken).ToString(), null, CommonConstants.CloudToDeviceOperation);

            // Even though If-Match is not a customHeader, add it here for convenience
            customHeaders.Add("IfMatch", lockToken);

            this.httpClientHelper.Post(
                GetRequestUri(this.deviceId, CommonConstants.DeviceBoundPathTemplate + "/" + lockToken, null),
                null,
                customHeaders);
        }
#else
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
#endif

#if NETMF
        protected override void OnReject(string lockToken)
        {
            var pathTemplate = new StringBuilder(CommonConstants.DeviceBoundPathRejectTemplate);

            Hashtable customHeaders = PrepareCustomHeaders(pathTemplate.Replace("{1}", lockToken).ToString(), null, CommonConstants.CloudToDeviceOperation);

            var eTag = new ETagHolder { ETag = lockToken };

            Hashtable fields = new Hashtable();
            fields.Add("reject", null);

            this.httpClientHelper.Delete(
                GetRequestUri(this.deviceId, CommonConstants.DeviceBoundPathTemplate + "/" + lockToken, fields),
                eTag,
                customHeaders);
        }
#else
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
#endif

#if NETMF
        static Hashtable PrepareCustomHeaders(string toHeader, string messageId, string operation)
        {
            var customHeaders = new Hashtable();

            customHeaders.Add(CustomHeaderConstants.To, toHeader);
            customHeaders.Add(CustomHeaderConstants.Operation, operation);

            if (!messageId.IsNullOrEmpty())
            {
                customHeaders.Add(CustomHeaderConstants.MessageId, messageId);
            }

            return customHeaders;
        }
#else
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
#endif

#if NETMF
        static string GetRequestUri(string deviceId, string path, Hashtable queryValueDictionary)
        {
            deviceId = WebUtility.UrlEncode(deviceId);

            // NetMF doesn't implement string.format so we need to use Replace of StringBuilder to replace it
            // this won't perform any validation on the format string, arguments and such
            // it should be OK as the path is a constant in the project and the {N} placeholders aren't valid chars for a path
            var stringBuilder = new StringBuilder(path).Replace("{0}", deviceId);
            stringBuilder.Append("?" + ClientApiVersionHelper.ApiVersionQueryString);

            if (queryValueDictionary != null)
            {
                foreach (var key in queryValueDictionary.Keys)
                {
                    if (queryValueDictionary[key] != null)
                    {
                        // key has value
                        stringBuilder.Append("&" + key + "=" + queryValueDictionary[key] as string);
                    }
                    else
                    {
                        // key doesn't have value, just add it to the query string
                        stringBuilder.Append("&" + key);
                    }
                }
            }

            // The NetMF implemention of Uri doesn't handle properly relative URLs so we need to return a string here. The complete URI will be assembled elsewhere.
            return stringBuilder.ToString();
        }
#else
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
#endif

#if !NETMF
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
#endif

    }
}