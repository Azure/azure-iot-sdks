// ---------------------------------------------------------------
// Copyright (c) Microsoft Corporation. All rights reserved.
// ---------------------------------------------------------------

namespace Microsoft.Azure.Devices
{
    using Microsoft.Azure.Devices.Common.Exceptions;
    using Newtonsoft.Json;

    /// <summary>
    /// Encapsulates device registry operation error details.
    /// </summary>
    public sealed class DeviceRegistryOperationError
    {
        /// <summary>
        /// The ID of the device that indicated the error.
        /// </summary>
        [JsonProperty(PropertyName = "deviceId", NullValueHandling = NullValueHandling.Ignore)]
        public string DeviceId { get; set; }

        /// <summary>
        /// ErrorCode associated with the error.
        /// </summary>
        [JsonProperty(PropertyName = "errorCode", NullValueHandling = NullValueHandling.Ignore)]
        public ErrorCode ErrorCode { get; set; }

        /// <summary>
        /// Additional details associated with the error.
        /// </summary>
        [JsonProperty(PropertyName = "errorStatus", NullValueHandling = NullValueHandling.Ignore)]
        public string ErrorStatus { get; set; }
    }
}
