// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client
{
#if !NETMF
    // NETMF doesn't implement EnumMember attribute
    using System.Runtime.Serialization;
#endif

    ///////////////////////////////////////////////////////////////////
    /* 
     * NOTE: when adding new items to this enum, make sure to update 
     * accordingly the NETMF versions of the following methods:
     * Utils.ConvertDeliveryAckTypeFromString() 
     * Utils.ConvertDeliveryAckTypeToString()
    */
    ///////////////////////////////////////////////////////////////////

    /// <summary>
    /// Specifies the different acknowledgment levels for message delivery.
    /// </summary>
    public enum DeliveryAcknowledgement
    {
        /// <summary>
        /// Acknowledgment is NOT sent on delivery or failure.
        /// </summary>
#if !NETMF
    [EnumMember(Value = "none")]
#endif
        None,

        /// <summary>
        /// Acknowledgment is sent only if delivery fails.
        /// </summary>
#if !NETMF
        [EnumMember(Value = "negative")]
#endif
        NegativeOnly,

        /// <summary>
        /// Acknowledgment is sent only on delivery succeeds.
        /// </summary>
#if !NETMF
        [EnumMember(Value = "positive")]
#endif
        PositiveOnly,

        /// <summary>
        /// An acknowledgment is sent on delivery success or failure.
        /// </summary>
#if !NETMF
        [EnumMember(Value = "full")]
#endif
        Full
    }
}
