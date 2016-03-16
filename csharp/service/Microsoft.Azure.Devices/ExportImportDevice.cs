// ---------------------------------------------------------------
// Copyright (c) Microsoft Corporation. All rights reserved.
// ---------------------------------------------------------------

namespace Microsoft.Azure.Devices
{
    using Newtonsoft.Json;

    /// <summary>
    ///  contains device properties specified during export/import operation
    /// </summary>
    public sealed class ExportImportDevice
    {
        /// <summary>
        /// making default ctor public
        /// </summary>
        public ExportImportDevice()
        {
        }

        /// <summary>
        /// ctor which takes a Device object along with import mode
        /// </summary>
        /// <param name="device"></param>
        /// <param name="importmode"></param>
        public ExportImportDevice(Device device, ImportMode importmode)
        {
            this.Id = device.Id;
            this.ETag = device.ETag;
            this.ImportMode = importmode;
            this.Status = device.Status;
            this.StatusReason = device.StatusReason;
            this.Authentication = device.Authentication;
        }

        [JsonProperty(PropertyName = "id", Required = Required.Always)]
        public string Id { get; set; }

        [JsonProperty(PropertyName = "eTag", NullValueHandling = NullValueHandling.Ignore)]
        public string ETag { get; set; }

        [JsonProperty(PropertyName = "importMode", DefaultValueHandling = DefaultValueHandling.IgnoreAndPopulate)]
        public ImportMode ImportMode { get; set; }

        [JsonProperty(PropertyName = "status", Required = Required.Always)]
        public DeviceStatus Status { get; set; }

        [JsonProperty(PropertyName = "statusReason", NullValueHandling = NullValueHandling.Ignore)]
        public string StatusReason { get; set; }

        [JsonProperty(PropertyName = "authentication")]
        public AuthenticationMechanism Authentication { get; set; }
    }
}