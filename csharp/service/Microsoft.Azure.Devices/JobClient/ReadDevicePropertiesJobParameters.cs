// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices
{
    using System;
    using System.Collections.Generic;

    /// <summary>
    /// Input job parameters to schedule read properties on devices
    /// </summary>
    public class ReadDevicePropertiesJobParameters : DeviceJobParameters
    {
        /// <summary>
        ///     Parameters for read device properties job on a multiple on a single device
        /// </summary>
        public ReadDevicePropertiesJobParameters(IList<string> propertyNames, string deviceId)
            : this(propertyNames, new List<string>() { deviceId })
        {
        }

        /// <summary>
        ///     Parameters for read device properties job on a multiple on multiple devices
        /// </summary>
        public ReadDevicePropertiesJobParameters(IList<string> propertyNames, IEnumerable<string> deviceIds)
            : base(JobType.ReadDeviceProperties, deviceIds)
        {
            if (propertyNames == null || propertyNames.Count == 0)
            {
                throw new ArgumentException(ApiResources.DevicePropertiesJob_NoPropertiesSpecified, nameof(propertyNames));
            }

            this.DevicePropertyNames = propertyNames;
        }

        /// <summary>
        /// Device property names
        /// </summary>
        public IList<string> DevicePropertyNames { get; private set; }
    }
}
