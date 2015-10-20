// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices
{
    using System.Threading;
    using System.Threading.Tasks;
    
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
            var iotHubConnectionString = IotHubConnectionString.Parse(connectionString);
            var serviceClient = new AmqpServiceClient(iotHubConnectionString);
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
        /// Get the FeedbackReceiver
        /// </summary>
        /// <returns>An instance of the FeedbackReceiver</returns>
        public abstract FeedbackReceiver<FeedbackBatch> GetFeedbackReceiver();

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
    }
}
