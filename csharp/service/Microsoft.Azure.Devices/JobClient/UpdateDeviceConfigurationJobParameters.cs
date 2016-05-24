// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices
{
    using System;
    using System.Collections.Generic;

    /// <summary>
    /// Input job parameters to schedule config update on devices
    /// </summary>
    public class UpdateDeviceConfigurationJobParameters : DeviceJobParameters
    {
        /// <summary>
        ///     Parameters for update device properties job on a multiple on a single device
        /// </summary>
        public UpdateDeviceConfigurationJobParameters(string value, string deviceId)
            : this(value, new List<string>() { deviceId} )
        {
        }

        /// <summary>
        ///     Parameters for update device properties job on a multiple on multiple devices
        /// </summary>
        public UpdateDeviceConfigurationJobParameters(string value, IEnumerable<string> deviceIds)
            : base(JobType.UpdateDeviceConfiguration, deviceIds)
        {
            if (value == null)
            {
                throw new ArgumentNullException(nameof(value));
            }

            this.Value = value;
        }

        /// <summary>
        /// Configuration value
        /// </summary>
        public string Value { get; private set; }
    }
}
