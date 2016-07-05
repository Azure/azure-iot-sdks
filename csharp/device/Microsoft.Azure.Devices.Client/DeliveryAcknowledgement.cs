// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client
{
    using System.Runtime.Serialization;
    /// <summary>
    /// Specifies the different acknowledgement levels for message delivery.
    /// </summary>
    public enum DeliveryAcknowledgement
    {
        /// <summary>
        /// Acknowledgement is NOT sent on delivery or failure.
        /// </summary>
        [EnumMember(Value = "none")]
        None,

        /// <summary>
        /// Acknowledgement is sent only if delivery fails.
        /// </summary>
        [EnumMember(Value = "negative")]
        NegativeOnly,

        /// <summary>
        /// Acknowledgement is sent only on delivery succeeds.
        /// </summary>
        [EnumMember(Value = "positive")]
        PositiveOnly,

        /// <summary>
        /// An acknowledgement is sent on delivery success or failure.
        /// </summary>
        [EnumMember(Value = "full")]
        Full
    }
}
