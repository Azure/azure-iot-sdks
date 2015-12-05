// ---------------------------------------------------------------
// Copyright (c) Microsoft Corporation. All rights reserved.
// ---------------------------------------------------------------

namespace Microsoft.Azure.Devices
{
    using System.Runtime.Serialization;
    using Newtonsoft.Json;
    using Newtonsoft.Json.Converters;

    /// <summary>
    /// Identifies the behavior when merging a device to the registry during import actions.
    /// </summary>
    [JsonConverter(typeof(StringEnumConverter))]
    public enum ImportMode
    {
        [EnumMember(Value = "createOrUpdate")]
        CreateOrUpdate = 0,

        [EnumMember(Value = "create")]
        Create = 1,

        [EnumMember(Value = "update")]
        Update = 2,

        [EnumMember(Value = "updateIfMatchETag")]
        UpdateIfMatchETag = 3,

        [EnumMember(Value = "createOrUpdateIfMatchETag")]
        CreateOrUpdateIfMatchETag = 4,

        [EnumMember(Value = "delete")]
        Delete = 5,

        [EnumMember(Value = "deleteIfMatchETag")]
        DeleteIfMatchETag = 6,
    }
}
