/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.sdk;

public enum DeliveryAcknowledgement
{
    /**
    * acknowledgment is NOT sent on delivery or failure.
    **/
    None,

    /**
    * acknowledgment is sent only if delivery fails.
    **/
    NegativeOnly,

    /**
    * acknowledgment is sent only on delivery succeeds.
    **/
    PositiveOnly,

    /**
    * acknowledgment is sent on delivery success or failure.
    **/
    Full    
}
