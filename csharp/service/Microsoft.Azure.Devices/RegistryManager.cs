// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices
{
    using System.Collections.Generic;
    using System.Threading;
    using System.Threading.Tasks;

    public abstract class RegistryManager
    {
        /// <summary>
        /// Creates an RegistryManager from the Iot Hub connection string.
        /// </summary>
        /// <param name="connectionString"> The Iot Hub connection string.</param>
        /// <returns> An RegistryManager instance. </returns>
        public static RegistryManager CreateFromConnectionString(string connectionString)
        {
            var iotHubConnectionString = IotHubConnectionString.Parse(connectionString);
            return new HttpRegistryManager(iotHubConnectionString);
        }

        /// <summary>
        /// Explicitly open the RegistryManager instance.
        /// </summary>
        public abstract Task OpenAsync();

        /// <summary>
        /// Closes the RegistryManager instance and disposes its resources.
        /// </summary>
        /// <returns></returns>
        public abstract Task CloseAsync();

        /// <summary>
        /// Register a new device with the system
        /// </summary>
        /// <param name="device">
        /// The Device object to be registered.
        /// </param>
        /// <returns>echoes back the Device object with the generated keys and etags</returns>
        public abstract Task<Device> AddDeviceAsync(Device device);

        /// <summary>
        /// Register a new device with the system
        /// </summary>
        /// <param name="device">
        /// The Device object to be registered.
        /// </param>
        /// <param name="cancellationToken">
        /// The token which allows the the operation to be cancelled.
        /// </param>
        /// <returns>echoes back the Device object with the generated keys and etags</returns>
        public abstract Task<Device> AddDeviceAsync(Device device, CancellationToken cancellationToken);

        /// <summary>
        /// Update the mutable fields of the device registration
        /// </summary>
        /// <param name="device">
        /// The Device object with updated fields.
        /// </param>
        /// <returns>echoes back the Device object with updated etags</returns>
        public abstract Task<Device> UpdateDeviceAsync(Device device);

        /// <summary>
        /// Update the mutable fields of the device registration
        /// </summary>
        /// <param name="device">
        /// The Device object with updated fields.
        /// </param>
        /// <param name="forceUpdate">
        /// Forces the device object to be replaced even if it was updated since it was retrieved last time.
        /// </param>
        /// <returns>echoes back the Device object with updated etags</returns>
        public abstract Task<Device> UpdateDeviceAsync(Device device, bool forceUpdate);

        /// <summary>
        /// Update the mutable fields of the device registration
        /// </summary>
        /// <param name="device">
        /// The Device object with updated fields.
        /// </param>
        /// <param name="cancellationToken">
        /// The token which allows the the operation to be cancelled.
        /// </param>
        /// <returns>echoes back the Device object with updated etags</returns>
        public abstract Task<Device> UpdateDeviceAsync(Device device, CancellationToken cancellationToken);

        /// <summary>
        /// Update the mutable fields of the device registration
        /// </summary>
        /// <param name="device">
        /// The Device object with updated fields.
        /// </param>
        /// <param name="forceUpdate">
        /// Forces the device object to be replaced even if it was updated since it was retrieved last time.
        /// </param>
        /// <param name="cancellationToken">
        /// The token which allows the the operation to be cancelled.
        /// </param>
        /// <returns>echoes back the Device object with updated etags</returns>
        public abstract Task<Device> UpdateDeviceAsync(Device device, bool forceUpdate, CancellationToken cancellationToken);

        /// <summary>
        /// Deletes a previously registered device from the system.
        /// </summary>
        /// <param name="deviceId">
        /// The id of the device to be deleted.
        /// </param>
        public abstract Task RemoveDeviceAsync(string deviceId);

        /// <summary>
        /// Deletes a previously registered device from the system.
        /// </summary>
        /// <param name="deviceId">
        /// The id of the device to be deleted.
        /// </param>
        /// <param name="cancellationToken">
        /// The token which allows the the operation to be cancelled.
        /// </param>
        public abstract Task RemoveDeviceAsync(string deviceId, CancellationToken cancellationToken);

        /// <summary>
        /// Deletes a previously registered device from the system.
        /// </summary>
        /// <param name="device">
        /// The device to be deleted.
        /// </param>
        public abstract Task RemoveDeviceAsync(Device device);

        /// <summary>
        /// Deletes a previously registered device from the system.
        /// </summary>
        /// <param name="device">
        /// The device to be deleted.
        /// </param>
        /// <param name="cancellationToken">
        /// The token which allows the the operation to be cancelled.
        /// </param>
        public abstract Task RemoveDeviceAsync(Device device, CancellationToken cancellationToken);

        /// <summary>
        /// Gets usage statistics for the Iot Hub.
        /// </summary>
        public abstract Task<RegistryStatistics> GetRegistryStatisticsAsync();

        /// <summary>
        /// Gets usage statistics for the Iot Hub.
        /// </summary>
        /// <param name="cancellationToken">
        /// The token which allows the the operation to be cancelled.
        /// </param>
        public abstract Task<RegistryStatistics> GetRegistryStatisticsAsync(CancellationToken cancellationToken);

        /// <summary>
        /// Retrieves the specified Device object.
        /// </summary>
        /// <param name="deviceId">
        /// The id of the device to be retrieved.
        /// </param>
        /// <returns>
        /// The Device object.
        /// </returns>
        public abstract Task<Device> GetDeviceAsync(string deviceId);

        /// <summary>
        /// Retrieves the specified Device object.
        /// </summary>
        /// <param name="deviceId">
        /// The id of the device to be retrieved.
        /// </param>
        /// <param name="cancellationToken">
        /// The token which allows the the operation to be cancelled.
        /// </param>
        /// <returns>
        /// The Device object.
        /// </returns>
        public abstract Task<Device> GetDeviceAsync(string deviceId, CancellationToken cancellationToken);

        /// <summary>
        /// Retrieves specified number of devices from every Iot Hub partition. 
        /// This is an approximation and not a definitive list. Results are not ordered.
        /// </summary>
        /// <returns>
        /// The list of devices
        /// </returns>
        public abstract Task<IEnumerable<Device>> GetDevicesAsync(int maxCount);

        /// <summary>
        /// Retrieves specified number of devices from every Iot hub partition. 
        /// This is an approximation and not a definitive list. Results are not ordered.
        /// </summary>
        /// <returns>
        /// The list of devices
        /// </returns>
        public abstract Task<IEnumerable<Device>> GetDevicesAsync(int maxCount, CancellationToken cancellationToken);

        /// <summary>
        /// Copies registered device data to a set of blobs in a specific container in a storage account. 
        /// </summary>
        internal abstract Task ExportRegistryAsync(string storageAccountConnectionString, string containerName);

        /// <summary>
        /// Copies registered device data to a set of blobs in a specific container in a storage account. 
        /// </summary>
        internal abstract Task ExportRegistryAsync(string storageAccountConnectionString, string containerName, CancellationToken cancellationToken);

        /// <summary>
        /// Imports registered device data from a set of blobs in a specific container in a storage account. 
        /// </summary>
        internal abstract Task ImportRegistryAsync(string storageAccountConnectionString, string containerName);

        /// <summary>
        /// Imports registered device data from a set of blobs in a specific container in a storage account. 
        /// </summary>
        internal abstract Task ImportRegistryAsync(string storageAccountConnectionString, string containerName, CancellationToken cancellationToken);

        /// <summary>
        /// Creates a new bulk job to export device registrations to the container specified by the provided URI.
        /// </summary>
        /// <param name="exportBlobContainerUri"></param>
        /// <param name="excludeKeys"></param>
        /// <returns></returns>
        public abstract Task<JobProperties> ExportDevicesAsync(string exportBlobContainerUri, bool excludeKeys);

        /// <summary>
        /// Creates a new bulk job to export device registrations to the container specified by the provided URI.
        /// </summary>
        /// <param name="exportBlobContainerUri"></param>
        /// <param name="excludeKeys"></param>
        /// <param name="cancellationToken"></param>
        /// <returns></returns>
        public abstract Task<JobProperties> ExportDevicesAsync(string exportBlobContainerUri, bool excludeKeys, CancellationToken cancellationToken);

        /// <summary>
        /// Gets the job with the specified ID.
        /// </summary>
        /// <param name="jobId"></param>
        /// <returns></returns>
        public abstract Task<JobProperties> GetJobAsync(string jobId);

        /// <summary>
        /// Gets the job with the specified ID.
        /// </summary>
        /// <param name="jobId"></param>
        /// <param name="cancellationToken"></param>
        /// <returns></returns>
        public abstract Task<JobProperties> GetJobAsync(string jobId, CancellationToken cancellationToken);

        /// <summary>
        /// List all jobs for the IoT Hub.
        /// </summary>
        /// <returns></returns>
        public abstract Task<IEnumerable<JobProperties>> GetJobsAsync();

        /// <summary>
        /// List all jobs for the IoT Hub.
        /// </summary>
        /// <param name="cancellationToken"></param>
        /// <returns></returns>
        public abstract Task<IEnumerable<JobProperties>> GetJobsAsync(CancellationToken cancellationToken);

        /// <summary>
        /// Cancels/Deletes the job with the specified ID.
        /// </summary>
        /// <param name="jobId"></param>
        /// <returns></returns>
        public abstract Task CancelJobAsync(string jobId);

        /// <summary>
        /// Cancels/Deletes the job with the specified ID.
        /// </summary>
        /// <param name="jobId"></param>
        /// <param name="cancellationToken"></param>
        /// <returns></returns>
        public abstract Task CancelJobAsync(string jobId, CancellationToken cancellationToken);
    }
}
