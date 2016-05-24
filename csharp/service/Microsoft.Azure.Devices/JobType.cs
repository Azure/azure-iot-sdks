// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices
{
    using System.Runtime.Serialization;
    using Newtonsoft.Json;
    using Newtonsoft.Json.Converters;

    /// <summary>
    /// Used to delineate job actions.
    /// </summary>
    [JsonConverter(typeof(StringEnumConverter))]
    public enum JobType
    {
        /// <summary>
        /// Unknown
        /// </summary>
        [EnumMember(Value = "unknown")]
        Unknown = 0,

        /// <summary>
        /// Indicates an ExportDevices job
        /// </summary>
        [EnumMember(Value = "export")]
        ExportDevices,

        /// <summary>
        /// Indicates an ImportDevices job
        /// </summary>
        [EnumMember(Value = "import")]
        ImportDevices,

        /// <summary>
        /// Indicates a ReadDeviceProperties job
        /// </summary>
        [EnumMember(Value = "readDeviceProperties")]
        ReadDeviceProperties,

        /// <summary>
        /// Indicates a WriteDeviceProperties job
        /// </summary>
        [EnumMember(Value = "writeDeviceProperties")]
        WriteDeviceProperties,

        /// <summary>
        /// Indicates a UpdateDeviceConfiguration job
        /// </summary>
        [EnumMember(Value = "updateDeviceConfiguration")]
        UpdateDeviceConfiguration,

        /// <summary>
        /// Indicates a RebootDevice job
        /// </summary>
        [EnumMember(Value = "rebootDevice")]
        RebootDevice,

        /// <summary>
        /// Indicates a FactoryResetDevice job
        /// </summary>
        [EnumMember(Value = "factoryResetDevice")]
        FactoryResetDevice,

        /// <summary>
        /// Indicates a FirmwareUpdate job
        /// </summary>
        [EnumMember(Value = "firmwareUpdate")]
        FirmwareUpdate,
    }
}
