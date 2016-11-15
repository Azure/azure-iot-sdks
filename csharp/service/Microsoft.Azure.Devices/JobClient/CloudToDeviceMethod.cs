// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices
{
    using System;
    using Newtonsoft.Json;
    using Newtonsoft.Json.Linq;

    /// <summary>
    /// Parameters to execute a direct method on the device
    /// </summary>
    public class CloudToDeviceMethod
    {
        internal CloudToDeviceMethod() { } // @ailn: for serialization only

        /// <summary>
        /// Creates an instance of CloudToDeviceMethod type
        /// </summary>
        /// <param name="methodName">Method name</param>
        /// <param name="responseTimeout">Method timeout</param>
        /// <param name="connectionTimeout">Device connection timeout</param>
        /// <exception cref="ArgumentException">If <b>methodName</b> is null or whitespace</exception>
        public CloudToDeviceMethod(string methodName, TimeSpan responseTimeout, TimeSpan connectionTimeout)
        {
            if (string.IsNullOrWhiteSpace(methodName))
            {
                throw new ArgumentException("Canot be empty", nameof(methodName));
            }

            this.MethodName = methodName;
            this.ResponseTimeout = responseTimeout;
            this.ConnectionTimeout = connectionTimeout;
        }

        /// <summary>
        /// Creates an instance of CloudToDeviceMethod type with Zero timeout
        /// </summary>
        /// <param name="methodName">Method name</param>
        /// <param name="responseTimeout">Method timeout</param>
        /// <exception cref="ArgumentException">If <b>methodName</b> is null or whitespace</exception>
        public CloudToDeviceMethod(string methodName, TimeSpan responseTimeout)
            : this(methodName, responseTimeout, TimeSpan.Zero)
        {
        }

        /// <summary>
        /// Creates an instance of CloudToDeviceMethod type with Zero timeout
        /// </summary>
        /// <param name="methodName">Method name</param>
        /// <exception cref="ArgumentException">If <b>methodName</b> is null or whitespace</exception>
        public CloudToDeviceMethod(string methodName)
            : this(methodName, TimeSpan.Zero, TimeSpan.Zero)
        {
        }

        /// <summary>
        /// Method to run
        /// </summary>
        [JsonProperty("methodName", Required = Required.Always)]
        public string MethodName { get; set; }

        /// <summary>
        /// Method timeout
        /// </summary>
        [JsonIgnore]
        public TimeSpan ResponseTimeout { get; set; }

        /// <summary>
        /// Timeout for device to come online
        /// </summary>
        [JsonIgnore]
        public TimeSpan ConnectionTimeout { get; set; }

        /// <summary>
        /// Set payload as json
        /// </summary>
        public CloudToDeviceMethod SetPayloadJson(string json)
        {
            if (json == null)
            {
                this.Payload = null;
            }
            else
            {
                try
                {
                    JToken.Parse(json); // @ailn: this is just a check for valid json as JRaw does not do the validation.
                    this.Payload = new JRaw(json);
                }
                catch (JsonException ex)
                {
                    throw new ArgumentException(ex.Message, nameof(json)); // @ailn: here we want to hide the fact we're using Json.net
                }
            }

            return this; // @ailn: it will allow such code: new CloudToDeviceMethod("c2dmethodname").SetPayloadJson("{ ... }");
        }

        /// <summary>
        /// Get payload as json
        /// </summary>
        public string GetPayloadAsJson()
        {
            // @ailn: 
            //  JRaw inherits from JToken which implements explicit string operator.
            //  It takes care of null ref and performs to string logic.
            return (string)this.Payload;
        }

        /// <summary>
        /// Method timeout in seconds
        /// </summary>
        [JsonProperty("responseTimeoutInSeconds", NullValueHandling = NullValueHandling.Ignore)]
        internal int? ResponseTimeoutInSeconds => this.ResponseTimeout <= TimeSpan.Zero ? (int?)null : (int)this.ResponseTimeout.TotalSeconds;

        /// <summary>
        /// Connection timeout in seconds
        /// </summary>
        [JsonProperty("connectTimeoutInSeconds", NullValueHandling = NullValueHandling.Ignore)]
        internal int? ConnectionTimeoutInSeconds => this.ConnectionTimeout <= TimeSpan.Zero ? (int?)null : (int)this.ConnectionTimeout.TotalSeconds;

        [JsonProperty("payload")]
        internal JRaw Payload { get; set; }
    }
}