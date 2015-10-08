// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client
{
    using System;

    /// <summary>
    /// Transport types supported by DeviceClient - AMQP and HTTP 1.1
    /// </summary>
    public enum TransportType
    {
        Amqp,
        Http1
    }

    /// <summary>
    /// Contains methods that a device can use to send messages to and receive from the service.
    /// </summary>
    public sealed class DeviceClient
    {
        const string DeviceId = "DeviceId";
        const string DeviceIdParameterPattern = @"(^\s*?|.*;\s*?)" + DeviceId + @"\s*?=.*";

        readonly DeviceClientHelper impl;

        DeviceClient(DeviceClientHelper impl)
        {
            this.impl = impl;
        }

        ///// <summary>
        ///// Create an Amqp DeviceClient from individual parameters
        ///// </summary>
        ///// <param name="hostname">The fully-qualified DNS hostname of IoT Hub</param>
        ///// <param name="authenticationMethod">The authentication method that is used</param>
        ///// <returns>DeviceClient</returns>
        //public static DeviceClient Create(string hostname, IAuthenticationMethod authenticationMethod)
        //{
        //    return Create(hostname, authenticationMethod, TransportType.Amqp);
        //}

        ///// <summary>
        ///// Create a DeviceClient from individual parameters
        ///// </summary>
        ///// <param name="hostname">The fully-qualified DNS hostname of IoT Hub</param>
        ///// <param name="authenticationMethod">The authentication method that is used</param>
        ///// <param name="transportType">The transportType used (Http1 or Amqp)</param>
        ///// <returns>DeviceClient</returns>
        //public static DeviceClient Create(string hostname, IAuthenticationMethod authenticationMethod, TransportType transportType)
        //{
        //    if (hostname == null)
        //    {
        //        throw new ArgumentNullException("hostname");
        //    }

        //    if (authenticationMethod == null)
        //    {
        //        throw new ArgumentNullException("authMethod");
        //    }

        //    var connectionStringBuilder = IotHubConnectionStringBuilder.Create(hostname, authenticationMethod);
        //    return CreateFromConnectionString(connectionStringBuilder.ToString(), transportType);
        //}

        /// <summary>
        /// Create a DeviceClient using HTTP transport from the specified connection string
        /// </summary>
        /// <param name="connectionString">Connection string for the IoT hub (including DeviceId)</param>
        /// <returns>DeviceClient</returns>
        public static DeviceClient CreateFromConnectionString(string connectionString)
        {
            return CreateFromConnectionString(connectionString, TransportType.Http1);
        }

        /// <summary>
        /// Create a DeviceClient using Http transport from the specified connection string
        /// </summary>
        /// <param name="connectionString">IoT Hub-Scope Connection string for the IoT hub (without DeviceId)</param>
        /// <param name="deviceId">Id of the Device</param>
        /// <returns>DeviceClient</returns>
        public static DeviceClient CreateFromConnectionString(string connectionString, string deviceId)
        {
            return CreateFromConnectionString(connectionString, deviceId, TransportType.Http1);
        }

        /// <summary>
        /// Create DeviceClient from the specified connection string using HTTP transport 
        /// </summary>
        /// <param name="connectionString">Connection string for the IoT hub (including DeviceId)</param>
        /// <param name="transportType">Specifies whether Amqp or Http transport is used</param>
        /// <returns>DeviceClient</returns>
        public static DeviceClient CreateFromConnectionString(string connectionString, TransportType transportType)
        {
            if (connectionString  == null)
            {
                throw new ArgumentNullException("connectionString");
            }

            var iotHubConnectionString = IotHubConnectionString.Parse(connectionString);
            if (transportType == TransportType.Amqp)
            {
                throw new NotImplementedException();
                //return new DeviceClient(new AmqpDeviceClient(iotHubConnectionString));
            }
            else if (transportType == TransportType.Http1)
            {
                return new DeviceClient(new HttpDeviceClient(iotHubConnectionString));
            }

            throw new InvalidOperationException("Unsupported Transport Type "+ transportType.ToString());
        }

        /// <summary>
        /// Create an DeviceClient from the specified connection string using the specified transport type
        /// </summary>
        /// <param name="connectionString">IoT Hub-Scope Connection string for the IoT hub (without DeviceId)</param>
        /// <param name="deviceId">Id of the device</param>
        /// <param name="transportType">The transportType used (Http1 or Amqp)</param>
        /// <returns>DeviceClient</returns>
        public static DeviceClient CreateFromConnectionString(string connectionString, string deviceId, TransportType transportType)
        {
            if (connectionString == null)
            {
                throw new ArgumentNullException("connectionString");
            }

            if (deviceId == null)
            {
                throw new ArgumentNullException("deviceId");
            }

            return CreateFromConnectionString(connectionString + ";" + DeviceId + "=" + deviceId, transportType);
        }

        ///// <summary>
        ///// Explicitly open the DeviceClient instance.
        ///// </summary>
        //public AsyncTask OpenAsync()
        //{
        //    return impl.OpenAsync().AsTaskOrAsyncOp();
        //}

        ///// <summary>
        ///// Close the DeviceClient instance
        ///// </summary>
        ///// <returns></returns>
        //public AsyncTask CloseAsync()
        //{
        //    return impl.CloseAsync().AsTaskOrAsyncOp();
        //}

        /// <summary>
        /// Receive a message from the device queue using the default timeout.
        /// </summary>
        /// <returns>The receive message or null if there was no message until the default timeout</returns>
        public Message Receive()
        {
            return impl.Receive();
        }

//        /// <summary>
//        /// Deletes a received message from the device queue
//        /// </summary>
//        /// <returns>The lock identifier for the previously received message</returns>
//        public AsyncTask CompleteAsync(string lockToken)
//        {
//            return impl.CompleteAsync(lockToken).AsTaskOrAsyncOp();
//        }

        /// <summary>
        /// Deletes a received message from the device queue
        /// </summary>
        /// <returns>The previously received message</returns>
        public void Complete(Message message)
        {
            impl.Complete(message);
        }

        /// <summary>
        /// Puts a received message back onto the device queue
        /// </summary>
        /// <returns>The previously received message</returns>
        public void Abandon(string lockToken)
        {
            impl.Abandon(lockToken);
        }

        /// <summary>
        /// Puts a received message back onto the device queue
        /// </summary>
        /// <returns>The lock identifier for the previously received message</returns>
        public void AbandonAsync(Message message)
        {
            impl.Abandon(message);
        }

        /// <summary>
        /// Deletes a received message from the device queue and indicates to the server that the message could not be processed.
        /// </summary>
        /// <returns>The previously received message</returns>
        public void Reject(string lockToken)
        {
            impl.Reject(lockToken);
        }

        /// <summary>
        /// Deletes a received message from the device queue and indicates to the server that the message could not be processed.
        /// </summary>
        /// <returns>The lock identifier for the previously received message</returns>
        public void Reject(Message message)
        {
            impl.Reject(message);
        }

        /// <summary>
        /// Sends an event to device hub
        /// </summary>
        /// <returns>The message containing the event</returns>
        public void SendEvent(Message message)
        {
            impl.SendEvent(message);
        }

//        /// <summary>
//        /// Sends a batch of events to device hub
//        /// </summary>
//        /// <returns>The task containing the event</returns>
//        public AsyncTask SendEventBatchAsync(IEnumerable<Message> messages)
//        {
//            return impl.SendEventBatchAsync(messages).AsTaskOrAsyncOp();
//        }
    }
}
