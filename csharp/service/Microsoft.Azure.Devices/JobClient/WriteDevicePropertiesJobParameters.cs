// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices
{
    using System;
    using System.Collections.Generic;

    /// <summary>
    /// Input job parameters to schedule write properties on devices
    /// </summary>
    public class WriteDevicePropertiesJobParameters : DeviceJobParameters
    {
        /// <summary>
        ///     Parameters for write device properties job on a multiple on a single device
        /// </summary>
        public WriteDevicePropertiesJobParameters(IDictionary<string, object> deviceProperties, string deviceId)
            : this(deviceProperties, new List<string>() { deviceId })
        {
        }

        /// <summary>
        ///     Parameters for write device properties job on a multiple on multiple devices
        /// </summary>
        public WriteDevicePropertiesJobParameters(IDictionary<string, object> deviceProperties, IEnumerable<string> deviceIds)
            : base(JobType.WriteDeviceProperties, deviceIds)
        {
            if (deviceProperties == null || deviceProperties.Count == 0)
            {
                throw new ArgumentException(ApiResources.DevicePropertiesJob_NoPropertiesSpecified, nameof(deviceProperties));
            }

            this.DeviceProperties = deviceProperties;
        }

        /// <summary>
        /// Device properties
        /// </summary>
        public IDictionary<string, object> DeviceProperties { get; private set; }
    }
}
