// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client
{
    ///////////////////////////////////////////////////////////////////
    /* 
     * NOTE: when adding new items to this enum, make sure to update 
     * accordingly the following in Utils:
     * 1) update items being added/removed to AckTypeMap in the Utils constructor
     * 2) the NETMF versions of the following methods:
     *    - Utils.ConvertDeliveryAckTypeFromString() 
     *    - Utils.ConvertDeliveryAckTypeToString()
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
        None,

        /// <summary>
        /// Acknowledgment is sent only if delivery fails.
        /// </summary>
        NegativeOnly,

        /// <summary>
        /// Acknowledgment is sent only on delivery succeeds.
        /// </summary>
        PositiveOnly,

        /// <summary>
        /// An acknowledgment is sent on delivery success or failure.
        /// </summary>
        Full
    }
}
