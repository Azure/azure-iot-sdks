// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client
{
    using System;
    using System.Security.Cryptography.X509Certificates;
    using Microsoft.Azure.Devices.Client.Extensions;

    /// <summary>
    /// Authentication method that uses a X.509 certificate
    /// </summary>
    public sealed class DeviceAuthenticationWithX509Certificate : IAuthenticationMethod
    {
        string deviceId;
        X509Certificate2 certificate;

        /// <summary>
        /// Initializes a new instance of the <see cref="DeviceAuthenticationWithX509Certificate"/> class.
        /// </summary>
        /// <param name="deviceId">Device Identifier.</param>
        /// <param name="certificate">X.509 Certificate.</param>
        public DeviceAuthenticationWithX509Certificate(string deviceId, X509Certificate2 certificate)
        {
            this.SetDeviceId(deviceId);
            this.SetCertificate(certificate);
        }

        /// <summary>
        /// Gets or sets the device identifier.
        /// </summary>
        public string DeviceId
        {
            get { return this.deviceId; }
            set { this.SetDeviceId(value); }
        }

        /// <summary>
        /// Gets or sets the X.509 certificate associated with this device
        /// </summary>
        public X509Certificate2 Certificate
        {
            get { return this.certificate; }
            set { this.SetCertificate(value); }
        }

        /// <summary>
        /// Populates an <see cref="IotHubConnectionStringBuilder"/> instance based on the properties of the current instance.
        /// </summary>
        /// <param name="iotHubConnectionStringBuilder">Instance to populate.</param>
        /// <returns>The populated <see cref="IotHubConnectionStringBuilder"/> instance.</returns>
        public IotHubConnectionStringBuilder Populate(IotHubConnectionStringBuilder iotHubConnectionStringBuilder)
        {
            if (iotHubConnectionStringBuilder == null)
            {
                throw new ArgumentNullException("iotHubConnectionStringBuilder");
            }

            iotHubConnectionStringBuilder.DeviceId = this.DeviceId;
            iotHubConnectionStringBuilder.AuthMechanism = "X509Certificate";
            iotHubConnectionStringBuilder.SharedAccessSignature = null;
            iotHubConnectionStringBuilder.SharedAccessKey = null;
            iotHubConnectionStringBuilder.SharedAccessKeyName = null;

            return iotHubConnectionStringBuilder;
        }

        void SetDeviceId(string deviceId)
        {
            if (deviceId.IsNullOrWhiteSpace())
            {
                throw new ArgumentNullException("deviceId");
            }

            this.deviceId = deviceId;
        }

        void SetCertificate(X509Certificate2 certificate)
        {
            if (certificate == null)
            {
                throw new ArgumentNullException("certificate");
            }

            this.certificate = certificate;
        }
    }
}
