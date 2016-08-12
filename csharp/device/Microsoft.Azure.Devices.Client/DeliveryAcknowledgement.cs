// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client
{
    /*   
     * NOTE: when adding new items to this enum, make sure to update   
     * accordingly the following functions in Utils:
     *      Utils.ConvertDeliveryAckTypeFromString()   
     *      Utils.ConvertDeliveryAckTypeToString()  
     */  
    
        /// <summary>
    /// Specifies the different acknowledgement levels for message delivery.
    /// </summary>
    public enum DeliveryAcknowledgement
    {
        /// <summary>
        /// Acknowledgement is NOT sent on delivery or failure.
        /// </summary>
        None,

        /// <summary>
        /// Acknowledgement is sent only if delivery fails.
        /// </summary>
        NegativeOnly,

        /// <summary>
        /// Acknowledgement is sent only on delivery succeeds.
        /// </summary>
        PositiveOnly,

        /// <summary>
        /// An acknowledgement is sent on delivery success or failure.
        /// </summary>
        Full
    }
}
