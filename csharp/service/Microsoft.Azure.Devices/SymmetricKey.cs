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

        /// <summary>
        /// Gets or sets the PrimaryKey
        /// </summary>
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

        /// <summary>
        /// Gets or sets the SecondaryKey
        /// </summary>
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

        /// <summary>
        /// Checks if the contents are valid
        /// </summary>
        /// <param name="throwArgumentException"></param>
        /// <returns>bool</returns>
        public bool IsValid(bool throwArgumentException)
        {
            if (!this.IsEmpty())
            {
                // either both keys should be specified or neither once should be specified (in which case we will create both the keys in the service)
                // we do allow primary and secondary keys to be identical
                if (string.IsNullOrWhiteSpace(this.PrimaryKey) || string.IsNullOrWhiteSpace(this.SecondaryKey))
                {
                    if (throwArgumentException)
                    {
                        throw new ArgumentException(ApiResources.DeviceKeysInvalid);
                    }

                    return false;
                }

                return true;
            }

            return false;
        }

        /// <summary>
        /// Checks if the fields are populated
        /// </summary>
        /// <returns>bool</returns>
        public bool IsEmpty()
        {
            return string.IsNullOrWhiteSpace(this.PrimaryKey) && string.IsNullOrWhiteSpace(this.SecondaryKey);
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