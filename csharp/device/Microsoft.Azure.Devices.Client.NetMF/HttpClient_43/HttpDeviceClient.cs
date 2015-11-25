// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client
{
    using System;
    using System.IO;
    using System.Net;

    using Microsoft.Azure.Devices.Client.Extensions;
    using System.Collections;
    using System.Text;

    sealed class HttpDeviceClient : DeviceClientHelper
    {
        static readonly TimeSpan DefaultOperationTimeout = new TimeSpan(0, 0, 60);
        static readonly TimeSpan DefaultReceiveTimeoutInSeconds = new TimeSpan(0);
        static Hashtable MapMessageProperties2HttpHeaders = new Hashtable();

        readonly HttpClientHelper httpClientHelper;

        readonly string deviceId;

        internal HttpDeviceClient(IotHubConnectionString iotHubConnectionString)
        {
            this.deviceId = iotHubConnectionString.DeviceId;
            this.httpClientHelper = new HttpClientHelper(
                iotHubConnectionString.HttpsEndpoint,
                iotHubConnectionString,
                DefaultOperationTimeout);
            this.DefaultReceiveTimeout = DefaultReceiveTimeoutInSeconds;
            
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
        
        protected override void OnOpen(bool explicitOpen)
        {
            return;
        }

        protected override void OnClose()
        {
        }

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
    }
}