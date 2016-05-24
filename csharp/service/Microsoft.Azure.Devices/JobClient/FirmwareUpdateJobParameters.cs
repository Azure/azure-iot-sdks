// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.


namespace Microsoft.Azure.Devices
{
    using System;
    using System.Collections.Generic;

    /// <summary>
    ///     Input job parameters to schedule firmware update on one or many devices
    /// </summary>
    public class FirmwareUpdateJobParameters : DeviceJobParameters
    {
        /// <summary>
        ///     Parameters for firmware update job on a single device
        /// </summary>
        public FirmwareUpdateJobParameters(string packageUri, TimeSpan timeOut, string deviceId)
            : this(packageUri, timeOut, new List<string>() { deviceId })
        {
        }

        /// <summary>
        ///     Parameters for firmware update job on a multiple devices 
        /// </summary>
        public FirmwareUpdateJobParameters(string packageUri, TimeSpan timeOut, IEnumerable<string> deviceIds)
            : base(JobType.FirmwareUpdate, deviceIds)
        {
            if (string.IsNullOrEmpty(packageUri))
            {
                throw new ArgumentException(ApiResources.FirmwareUpdateJob_NullOrEmptyPackageUri, packageUri);                
            }

            if (timeOut == null)
            {
                throw new ArgumentNullException(nameof(timeOut));
            }

            PackageUri = packageUri;
            Timeout = timeOut;
        }

        /// <summary>
        ///     Firmware package URI
        /// </summary>
        public string PackageUri { get; private set; }

        /// <summary>
        ///     Job timeout
        /// </summary>
        public TimeSpan Timeout { get; private set; }
    }
}