// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices
{
    using System;
    using System.Collections.Generic;
    using System.Threading;
    using System.Threading.Tasks;

    /// <summary>
    /// Job management
    /// </summary>
    public abstract class JobClient
    {
        /// <summary>
        /// Creates a JobClient from the Iot Hub connection string.
        /// </summary>
        /// <param name="connectionString"> The Iot Hub connection string.</param>
        /// <returns> A JobClient instance. </returns>
        public static JobClient CreateFromConnectionString(string connectionString)
        {
            var iotHubConnectionString = IotHubConnectionString.Parse(connectionString);
            return new HttpJobClient(iotHubConnectionString);
        }

        /// <summary>
        /// Explicitly open the JobClient instance.
        /// </summary>
        public abstract Task OpenAsync();

        /// <summary>
        /// Closes the JobClient instance and disposes its resources.
        /// </summary>
        public abstract Task CloseAsync();

        /// <summary>
        /// Gets the job with the specified ID.
        /// </summary>
        /// <param name="jobId">Id of the Job to retrieve</param>
        /// <returns>The matching JobResponse object</returns>
        public abstract Task<JobResponse> GetJobAsync(string jobId);

        /// <summary>
        /// Gets the job with the specified ID.
        /// </summary>
        /// <param name="jobId">Id of the job to retrieve</param>
        /// <param name="cancellationToken">Task cancellation token</param>
        /// <returns>The matching JobResponse object</returns>
        public abstract Task<JobResponse> GetJobAsync(string jobId, CancellationToken cancellationToken);

        /// <summary>
        /// List all jobs for the IoT Hub.
        /// </summary>
        /// <returns>List of JobResponse objects</returns>
        internal abstract Task<IEnumerable<JobResponse>> GetJobsAsync();

        /// <summary>
        /// List all jobs for the IoT Hub.
        /// </summary>
        /// <param name="cancellationToken">Task cancellation token</param>
        /// <returns>List of JobResponse objects</returns>
        internal abstract Task<IEnumerable<JobResponse>> GetJobsAsync(CancellationToken cancellationToken);

        /// <summary>
        /// Gets the jobs that meet the query criteria
        /// </summary>
        /// <param name="queryJson">JSON query string</param>
        /// <returns>Results of the specified query</returns>
        public abstract Task<JobQueryResult> QueryJobHistoryJsonAsync(string queryJson);

        /// <summary>
        /// Gets the jobs that meet the query criteria
        /// </summary>
        /// <param name="queryJson">JSON query string</param>
        /// <param name="cancellationToken">Task cancellation token</param>
        /// <returns>Results of the specified query</returns>
        public abstract Task<JobQueryResult> QueryJobHistoryJsonAsync(string queryJson, CancellationToken cancellationToken);

        /// <summary>
        /// Cancels/Deletes the job with the specified ID.
        /// </summary>
        /// <param name="jobId">Id of the Job to cancel</param>
        public abstract Task CancelJobAsync(string jobId);

        /// <summary>
        /// Cancels/Deletes the job with the specified ID.
        /// </summary>
        /// <param name="jobId">Id of the job to cancel</param>
        /// <param name="cancellationToken">Task cancellation token</param>
        public abstract Task CancelJobAsync(string jobId, CancellationToken cancellationToken);

        /// <summary>
        /// Schedules a Job to read the System defined Device Property value from a Device
        /// </summary>
        /// <param name="jobId">Job Id </param>
        /// <param name="deviceId">Device Id </param>
        /// <param name="propertyName">Property to read</param>
        /// <returns>Job to poll for status</returns>
        public abstract Task<JobResponse> ScheduleDevicePropertyReadAsync(string jobId, string deviceId, string propertyName);

        /// <summary>
        /// Schedules a Job to read the System defined Device Property value from a Device
        /// </summary>
        /// <param name="jobId">Job Id </param>
        /// <param name="deviceId">Device Id </param>
        /// <param name="propertyName">Property to read</param>
        /// <param name="cancellationToken"> The token which allows the the operation to be cancelled.</param>
        /// <returns>Job to poll for status</returns>
        public abstract Task<JobResponse> ScheduleDevicePropertyReadAsync(string jobId, string deviceId, string propertyName, CancellationToken cancellationToken);

        /// <summary>
        /// Schedules a Job to read the System defined Device Property value from a Device
        /// </summary>
        /// <param name="jobId">Job Id </param>
        /// <param name="deviceIds">Device Ids </param>
        /// <param name="propertyName">Property to read</param>
        /// <returns>Job to poll for status</returns>
        public abstract Task<JobResponse> ScheduleDevicePropertyReadAsync(string jobId, IEnumerable<string> deviceIds, string propertyName);

        /// <summary>
        /// Schedules a Job to read the System defined Device Property value from a Device
        /// </summary>
        /// <param name="jobId">Job Id </param>
        /// <param name="deviceIds">Device Ids </param>
        /// <param name="propertyName">Property to read</param>
        /// <param name="cancellationToken"> The token which allows the the operation to be cancelled.</param>
        /// <returns>Job to poll for status</returns>
        public abstract Task<JobResponse> ScheduleDevicePropertyReadAsync(string jobId, IEnumerable<string> deviceIds, string propertyName, CancellationToken cancellationToken);

        /// <summary>
        /// Schedules a Job to update the System defined Device Property value for a Device
        /// </summary>
        /// <param name="jobId">Job id</param>
        /// <param name="deviceId">Device Id </param>
        /// <param name="propertyName">Property to Update</param>
        /// <param name="value">Value to be updated to</param>
        /// <returns>Job to poll for status</returns>
        public abstract Task<JobResponse> ScheduleDevicePropertyWriteAsync(string jobId, string deviceId, string propertyName, object value);

        /// <summary>
        /// Schedules a Job to update the System defined Device Property value for a Device
        /// </summary>
        /// <param name="jobId">Job Id </param>
        /// <param name="deviceId">Device Id </param>
        /// <param name="propertyName">Property to Update</param>
        /// <param name="value">Value to be updated to</param>
        /// <param name="cancellationToken"> The token which allows the the operation to be cancelled.</param>
        /// <returns>Job to poll for status</returns>
        public abstract Task<JobResponse> ScheduleDevicePropertyWriteAsync(string jobId, string deviceId, string propertyName, object value, CancellationToken cancellationToken);

        /// <summary>
        /// Schedules a Job to update the System defined Device Property value for a Device
        /// </summary>
        /// <param name="jobId">job id</param>
        /// <param name="deviceIds">Device Ids </param>
        /// <param name="propertyName">Property to Update</param>
        /// <param name="value">Value to be updated to</param>
        /// <returns>Job to poll for status</returns>
        public abstract Task<JobResponse> ScheduleDevicePropertyWriteAsync(string jobId, IEnumerable<string> deviceIds, string propertyName, object value);

        /// <summary>
        /// Schedules a Job to update the System defined Device Property value for a Device
        /// </summary>
        /// <param name="jobId">Job Id </param>
        /// <param name="deviceIds">Device Ids </param>
        /// <param name="propertyName">Property to Update</param>
        /// <param name="value">Value to be updated to</param>
        /// <param name="cancellationToken"> The token which allows the the operation to be cancelled.</param>
        /// <returns>Job to poll for status</returns>
        public abstract Task<JobResponse> ScheduleDevicePropertyWriteAsync(string jobId, IEnumerable<string> deviceIds, string propertyName, object value, CancellationToken cancellationToken);

        /// <summary>
        /// Schedules a Job to update the Configuration value for a Device
        /// </summary>
        /// <param name="jobId">Job Id </param>
        /// <param name="deviceId">Device Id </param>
        /// <param name="value">Value to be updated to</param>
        /// <returns>Job to poll for status</returns>
        public abstract Task<JobResponse> ScheduleUpdateDeviceConfigurationAsync(string jobId, string deviceId, string value);

        /// <summary>
        /// Schedules a Job to update the Configuration value for a Device
        /// </summary>
        /// <param name="jobId">Job Id </param>
        /// <param name="deviceId">Device Id </param>
        /// <param name="value">Value to be updated to</param>
        /// <param name="cancellationToken"> The token which allows the the operation to be cancelled.</param>
        /// <returns>Job to poll for status</returns>
        public abstract Task<JobResponse> ScheduleUpdateDeviceConfigurationAsync(string jobId, string deviceId, string value, CancellationToken cancellationToken);

        /// <summary>
        /// Schedules a Job to update the Configuration value for a Device
        /// </summary>
        /// <param name="jobId">Job Id </param>
        /// <param name="deviceIds">Device Ids </param>
        /// <param name="value">Value to be updated to</param>
        /// <returns>Job to poll for status</returns>
        public abstract Task<JobResponse> ScheduleUpdateDeviceConfigurationAsync(string jobId, IEnumerable<string> deviceIds, string value);

        /// <summary>
        /// Schedules a Job to update the Configuration value for a Device
        /// </summary>
        /// <param name="jobId">Job Id </param>
        /// <param name="deviceIds">Device Ids </param>
        /// <param name="value">Value to be updated to</param>
        /// <param name="cancellationToken"> The token which allows the the operation to be cancelled.</param>
        /// <returns>Job to poll for status</returns>
        public abstract Task<JobResponse> ScheduleUpdateDeviceConfigurationAsync(string jobId, IEnumerable<string> deviceIds, string value, CancellationToken cancellationToken);

        /// <summary>
        /// Schedules a job to update the firmware of a Device
        /// </summary>
        /// <param name="jobId">Job Id</param>
        /// <param name="deviceId">Device Id</param>
        /// <param name="packageUri">Uri where the package should be downloaded from</param>
        /// <returns>Job to poll for status</returns>
        public abstract Task<JobResponse> ScheduleFirmwareUpdateAsync(string jobId, string deviceId, string packageUri);

        /// <summary>
        /// Schedules a job to update the firmware of a Device
        /// </summary>
        /// <param name="jobId">Job Id</param>
        /// <param name="deviceId">Device Id</param>
        /// <param name="packageUri">Uri where the package should be downloaded from</param>
        /// <param name="cancellationToken">The token which allows the operation to be cancelled.</param>
        /// <returns>Job to poll for status</returns>
        public abstract Task<JobResponse> ScheduleFirmwareUpdateAsync(string jobId, string deviceId, string packageUri, CancellationToken cancellationToken);


        /// <summary>
        /// Schedules a job to update the firmware of a Device
        /// </summary>
        /// <param name="jobId">Job Id</param>
        /// <param name="deviceIds">Device Ids</param>
        /// <param name="packageUri">Uri where the package should be downloaded from</param>
        /// <returns>Job to poll for status</returns>
        public abstract Task<JobResponse> ScheduleFirmwareUpdateAsync(string jobId, IEnumerable<string> deviceIds, string packageUri);

        /// <summary>
        /// Schedules a job to update the firmware of a Device
        /// </summary>
        /// <param name="jobId">Job Id</param>
        /// <param name="deviceIds">Device Ids</param>
        /// <param name="packageUri">Uri where the package should be downloaded from</param>
        /// <param name="cancellationToken">The token which allows the operation to be cancelled.</param>
        /// <returns>Job to poll for status</returns>
        public abstract Task<JobResponse> ScheduleFirmwareUpdateAsync(string jobId, IEnumerable<string> deviceIds, string packageUri, CancellationToken cancellationToken);

        /// <summary>
        /// Schedules a job to update the firmware of a Device
        /// </summary>
        /// <param name="jobId">Job Id</param>
        /// <param name="deviceId">Device Id</param>
        /// <param name="packageUri">Uri where the package should be downloaded from</param>
        /// <param name="timeout">Job timeout, with a lower bound of 1 minute and an upper bound of 1 day</param>
        /// <returns>Job to poll for status</returns>
        public abstract Task<JobResponse> ScheduleFirmwareUpdateAsync(string jobId, string deviceId, string packageUri, TimeSpan timeout);

        /// <summary>
        /// Schedules a job to update the firmware of a Device
        /// </summary>
        /// <param name="jobId">Job Id</param>
        /// <param name="deviceId">Device Id</param>
        /// <param name="packageUri">Uri where the package should be downloaded from</param>
        /// <param name="timeout">Job timeout, with a lower bound of 1 minute and an upper bound of 1 day</param>
        /// <param name="cancellationToken">The token which allows the operation to be cancelled.</param>
        /// <returns>Job to poll for status</returns>
        public abstract Task<JobResponse> ScheduleFirmwareUpdateAsync(string jobId, string deviceId, string packageUri, TimeSpan timeout, CancellationToken cancellationToken);


        /// <summary>
        /// Schedules a job to update the firmware of a Device
        /// </summary>
        /// <param name="jobId">Job Id</param>
        /// <param name="deviceIds">Device Ids</param>
        /// <param name="packageUri">Uri where the package should be downloaded from</param>
        /// <param name="timeout">Job timeout, with a lower bound of 1 minute and an upper bound of 1 day</param>
        /// <returns>Job to poll for status</returns>
        public abstract Task<JobResponse> ScheduleFirmwareUpdateAsync(string jobId, IEnumerable<string> deviceIds, string packageUri, TimeSpan timeout);

        /// <summary>
        /// Schedules a job to update the firmware of a Device
        /// </summary>
        /// <param name="jobId">Job Id</param>
        /// <param name="deviceIds">Device Ids</param>
        /// <param name="packageUri">Uri where the package should be downloaded from</param>
        /// <param name="timeout">Job timeout, with a lower bound of 1 minute and an upper bound of 1 day</param>
        /// <param name="cancellationToken">The token which allows the operation to be cancelled.</param>
        /// <returns>Job to poll for status</returns>
        public abstract Task<JobResponse> ScheduleFirmwareUpdateAsync(string jobId, IEnumerable<string> deviceIds, string packageUri, TimeSpan timeout, CancellationToken cancellationToken);

        /// <summary>
        /// Schedule a job to reboot a Device
        /// </summary>
        /// <param name="jobId">Job Id</param>
        /// <param name="deviceId">Device Id</param>
        /// <returns>Job to poll for status</returns>
        public abstract Task<JobResponse> ScheduleRebootDeviceAsync(string jobId, string deviceId);

        /// <summary>
        /// Schedule a job to reboot a Device
        /// </summary>
        /// <param name="jobId">Job Id</param>
        /// <param name="deviceId">Device Ids</param>
        /// <param name="cancellationToken">The token which allows the operation to be cancelled.</param>
        /// <returns>Job to poll for status</returns>
        public abstract Task<JobResponse> ScheduleRebootDeviceAsync(string jobId, string deviceId, CancellationToken cancellationToken);

        /// <summary>
        /// Schedule a job to reboot a Device
        /// </summary>
        /// <param name="jobId">Job Id</param>
        /// <param name="deviceIds">Device Ids</param>
        /// <returns>Job to poll for status</returns>
        public abstract Task<JobResponse> ScheduleRebootDeviceAsync(string jobId, IEnumerable<string> deviceIds);

        /// <summary>
        /// Schedule a job to reboot a Device
        /// </summary>
        /// <param name="jobId">Job Id</param>
        /// <param name="deviceIds">Device Ids</param>
        /// <param name="cancellationToken">The token which allows the operation to be cancelled.</param>
        /// <returns>Job to poll for status</returns>
        public abstract Task<JobResponse> ScheduleRebootDeviceAsync(string jobId, IEnumerable<string> deviceIds, CancellationToken cancellationToken);


        /// <summary>
        /// Schedule a job to factory reset a Device
        /// </summary>
        /// <param name="jobId">Job Id</param>
        /// <param name="deviceId">Device Id</param>
        /// <returns>Job to poll for status</returns>
        public abstract Task<JobResponse> ScheduleFactoryResetDeviceAsync(string jobId, string deviceId);

        /// <summary>
        /// Schedule a job to factory reset a Device
        /// </summary>
        /// <param name="jobId">Job Id</param>
        /// <param name="deviceId">Device Id</param>
        /// <param name="cancellationToken">The token which allows the operation to be cancelled.</param>
        /// <returns>Job to poll for status</returns>
        public abstract Task<JobResponse> ScheduleFactoryResetDeviceAsync(string jobId, string deviceId, CancellationToken cancellationToken);

        /// <summary>
        /// Schedule a job to factory reset a Device
        /// </summary>
        /// <param name="jobId">Job Id</param>
        /// <param name="deviceIds">Device Id</param>
        /// <returns>Job to poll for status</returns>
        public abstract Task<JobResponse> ScheduleFactoryResetDeviceAsync(string jobId, IEnumerable<string> deviceIds);

        /// <summary>
        /// Schedule a job to factory reset a Device
        /// </summary>
        /// <param name="jobId">Job Id</param>
        /// <param name="deviceIds">Device Id</param>
        /// <param name="cancellationToken">The token which allows the operation to be cancelled.</param>
        /// <returns>Job to poll for status</returns>
        public abstract Task<JobResponse> ScheduleFactoryResetDeviceAsync(string jobId, IEnumerable<string> deviceIds, CancellationToken cancellationToken);
    }
}
