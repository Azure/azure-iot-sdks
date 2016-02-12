// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices
{
    using System;
    using Microsoft.Azure.Devices.Common;
    using Microsoft.Azure.Devices.Common.Security;
    using Newtonsoft.Json;

    /// <summary>
    /// primary and secondary symmetric keys of a device.
    /// </summary>
    public sealed class SymmetricKey
    {
        string primaryKey;
        string secondaryKey;

        [JsonProperty(PropertyName = "primaryKey")]
        public string PrimaryKey
        {
            get
            {
                return this.primaryKey;
            }

            set
            {
                ValidateDeviceAuthenticationKey(value, "PrimaryKey");
                this.primaryKey = value;
            }
        }

        [JsonProperty(PropertyName = "secondaryKey")]
        public string SecondaryKey
        {
            get
            {
                return this.secondaryKey;
            }

            set
            {
                ValidateDeviceAuthenticationKey(value, "SecondaryKey");
                this.secondaryKey = value;
            }
        }

        static void ValidateDeviceAuthenticationKey(string key, string paramName)
        {
            if (key != null)
            {
                int keyLength;
                if (!Utils.IsValidBase64(key, out keyLength))
                {
                    throw new ArgumentException(CommonResources.GetString(Resources.StringIsNotBase64, key), paramName);
                }

                if (keyLength < SecurityConstants.MinKeyLengthInBytes || keyLength > SecurityConstants.MaxKeyLengthInBytes)
                {
                    throw new ArgumentException(CommonResources.GetString(Resources.DeviceKeyLengthInvalid, SecurityConstants.MinKeyLengthInBytes, SecurityConstants.MaxKeyLengthInBytes));
                }
            }
        }
    }
}