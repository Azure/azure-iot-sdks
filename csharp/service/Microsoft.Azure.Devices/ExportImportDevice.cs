// ---------------------------------------------------------------
// Copyright (c) Microsoft Corporation. All rights reserved.
// ---------------------------------------------------------------

namespace Microsoft.Azure.Devices
{
    using System;
    using Newtonsoft.Json;

    /// <summary>
    ///  contains device properties specified during export/import operation
    /// </summary>
    public sealed class ExportImportDevice
    {
        string eTag;

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
            this.SetETag(device.ETag);
            this.ImportMode = importmode;
            this.Status = device.Status;
            this.StatusReason = device.StatusReason;
            this.Authentication = device.Authentication;
        }

        /// <summary>
        /// Id of the device
        /// </summary>
        [JsonProperty(PropertyName = "id", Required = Required.Always)]
        public string Id { get; set; }

        /// <summary>
        /// ETag of the device
        /// </summary>
        [JsonProperty(PropertyName = "eTag", NullValueHandling = NullValueHandling.Ignore)]
        public string ETag {
            get
            {
                return this.eTag;
            }
            set
            {
                this.SetETag(value);
            }
        }

        /// <summary>
        /// ImportMode of the device
        /// </summary>
        [JsonProperty(PropertyName = "importMode", DefaultValueHandling = DefaultValueHandling.IgnoreAndPopulate)]
        public ImportMode ImportMode { get; set; }

        /// <summary>
        /// Status of the device
        /// </summary>
        [JsonProperty(PropertyName = "status", Required = Required.Always)]
        public DeviceStatus Status { get; set; }

        /// <summary>
        /// StatusReason of the device
        /// </summary>
        [JsonProperty(PropertyName = "statusReason", NullValueHandling = NullValueHandling.Ignore)]
        public string StatusReason { get; set; }

        /// <summary>
        /// AuthenticationMechanism of the device
        /// </summary>
        [JsonProperty(PropertyName = "authentication")]
        public AuthenticationMechanism Authentication { get; set; }

        void SetETag(string eTag)
        {
            if (!string.IsNullOrWhiteSpace(eTag))
            {
                string localETag = eTag;

                if (eTag.StartsWith("\"", StringComparison.OrdinalIgnoreCase))
                {
                    // remove only the first char
                    localETag = eTag.Substring(1);
                }

                if (localETag.EndsWith("\"", StringComparison.OrdinalIgnoreCase))
                {
                    // remove only the last char
                    localETag = localETag.Remove(localETag.Length - 1);
                }

                this.eTag = localETag;
            }
            else
            {
                // in case it is empty or contains whitespace
                this.eTag = eTag;
            }
        }
    }
}