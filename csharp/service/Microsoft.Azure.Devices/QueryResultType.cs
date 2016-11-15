// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices
{
    using System.Runtime.Serialization;

    enum QueryResultType
    {
        [EnumMember(Value = "unknown")]
        Unknown = 0,

        [EnumMember(Value = "twin")]
        Twin = 1,

        [EnumMember(Value = "deviceJob")]
        DeviceJob = 2,

        [EnumMember(Value = "jobResponse")]
        JobResponse = 3,

        [EnumMember(Value = "raw")]
        Raw = 4
    }
}