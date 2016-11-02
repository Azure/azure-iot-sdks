// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.


namespace Microsoft.Azure.Devices
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using Microsoft.Azure.Devices.Common;

    /// <summary>
    ///     Extend JobParameters with device Ids
    /// </summary>
    public class DeviceJobParameters : JobParameters
    {
        /// <summary>
        ///     Parameters for parameterless device job on a single device
        /// </summary>        
        public DeviceJobParameters(JobType jobType, string deviceId)
            : this(jobType, new List<string>() { deviceId})
        {
        }

        /// <summary>
        ///     Parameters for parameterless device job on multiple devices
        /// </summary>        
        public DeviceJobParameters(JobType jobType, IEnumerable<string> deviceIds)
            : base(jobType)
        {
            IList<string> deviceList = deviceIds?.ToListSlim();
            if (deviceList == null || deviceList.Count == 0)
            {
                throw new ArgumentException(ApiResources.DeviceJobParametersNullOrEmptyDeviceList, nameof(deviceIds));
            }

            if ((from deviceId in deviceList where string.IsNullOrWhiteSpace(deviceId) select deviceId).Any())
            {
                throw new ArgumentException(ApiResources.DeviceJobParametersNullOrEmptyDeviceListEntries, nameof(deviceIds));
            }

            DeviceIds = deviceList;
        }

        /// <summary>
        ///     Ids of target devices
        /// </summary>
        public IList<string> DeviceIds { get; private set; }
    }
}