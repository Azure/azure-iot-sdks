// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices
{
    using System.Threading;
    using System.Threading.Tasks;

    /// <summary>
    /// Transport types supported by ServiceClient - Amqp and Amqp over WebSocket only
    /// </summary>
    public enum TransportType
    {
        /// <summary>
        /// Advanced Message Queuing Protocol transport.
        /// </summary>
        Amqp,

        /// <summary>
        /// Advanced Message Queuing Protocol transport over WebSocket only.
        /// </summary>
        Amqp_WebSocket_Only
    }

    /// <summary>
    /// Contains methods that services can use to send messages to devices
    /// </summary>
    public abstract class ServiceClient
    {
        /// <summary>
        /// Make this constructor internal so that only this library may implement this abstract class.
        /// </summary>
        internal ServiceClient()
        {
        }

        /// <summary>
        /// Create ServiceClient from the specified connection string
        /// </summary>
        /// <param name="connectionString">Connection string for the iothub</param>
        /// <returns></returns>
        public static ServiceClient CreateFromConnectionString(string connectionString)
        {
            return CreateFromConnectionString(connectionString, TransportType.Amqp);
        }

        /// <summary>
        /// Create ServiceClient from the specified connection string using specified Transport Type
        /// </summary>
        /// <param name="connectionString">Connection string for the iothub</param>
        /// <param name="transportType">Specifies whether Amqp or Amqp over Websocket transport is used</param>
        /// <returns></returns>
        public static ServiceClient CreateFromConnectionString(string connectionString, TransportType transportType)
        {
            var iotHubConnectionString = IotHubConnectionString.Parse(connectionString);
            var serviceClient = new AmqpServiceClient(iotHubConnectionString, (transportType == TransportType.Amqp_WebSocket_Only) ? true : false);
            return serviceClient;
        }

        /// <summary>
        /// Open the ServiceClient instance
        /// </summary>
        /// <returns></returns>
        public abstract Task OpenAsync();

        /// <summary>
        /// Close the ServiceClient instance
        /// </summary>
        /// <returns></returns>
        public abstract Task CloseAsync();

        /// <summary>
        /// Send a one-way notification to the specified device
        /// </summary>
        /// <param name="deviceId">The device identifier for the target device</param>
        /// <param name="message">The message containing the notification</param>
        /// <returns></returns>
        public abstract Task SendAsync(string deviceId, Message message);

        /// <summary>
        /// Removes all messages from a device's queue.
        /// </summary>
        /// <param name="deviceId"></param>
        /// <returns></returns>
        public abstract Task<PurgeMessageQueueResult> PurgeMessageQueueAsync(string deviceId);

        /// <summary>
        /// Removes all messages from a device's queue.
        /// </summary>
        /// <param name="deviceId"></param>
        /// <param name="cancellationToken"></param>
        /// <returns></returns>
        public abstract Task<PurgeMessageQueueResult> PurgeMessageQueueAsync(string deviceId, CancellationToken cancellationToken);

        /// <summary>
        /// Get the FeedbackReceiver
        /// </summary>
        /// <returns>An instance of the FeedbackReceiver</returns>
        public abstract FeedbackReceiver<FeedbackBatch> GetFeedbackReceiver();

        /// <summary>
        /// Get the FeedbackReceiver
        /// </summary>
        /// <returns>An instance of the FeedbackReceiver</returns>
        public abstract FileNotificationReceiver<FileNotification> GetFileNotificationReceiver();

        /// <summary>
        /// Gets service statistics for the Iot Hub.
        /// </summary>
        /// <returns>returns ServiceStatistics object containing current service statistics</returns>
        public abstract Task<ServiceStatistics> GetServiceStatisticsAsync();

        /// <summary>
        /// Gets service statistics for the Iot Hub.
        /// </summary>
        /// <param name="cancellationToken">
        /// The token which allows the the operation to be cancelled.
        /// </param>
        /// <returns>returns ServiceStatistics object containing current service statistics</returns>
        public abstract Task<ServiceStatistics> GetServiceStatisticsAsync(CancellationToken cancellationToken);

        /// <summary>
        /// Interactively invokes a method on device
        /// </summary>
        /// <param name="deviceId">Device Id</param>
        /// <param name="cloudToDeviceMethod">Device method parameters (passthrough to device)</param>
        /// <returns>Method result</returns>
        public abstract Task<CloudToDeviceMethodResult> InvokeDeviceMethodAsync(string deviceId, CloudToDeviceMethod cloudToDeviceMethod);

        /// <summary>
        /// Interactively invokes a method on device
        /// </summary>
        /// <param name="deviceId">Device Id</param>
        /// <param name="cloudToDeviceMethod">Device method parameters (passthrough to device)</param>
        /// <param name="cancellationToken">Cancellation Token</param>
        /// <returns>Method result</returns>
        public abstract Task<CloudToDeviceMethodResult> InvokeDeviceMethodAsync(string deviceId, CloudToDeviceMethod cloudToDeviceMethod, CancellationToken cancellationToken);
    }
}
