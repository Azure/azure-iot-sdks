/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.sdk;

public enum DeliveryAcknowledgement
{
    /**
    * Acknowledgement is NOT sent on delivery or failure.
    **/
    None,

    /**
    * Acknowledgement is sent only if delivery fails.
    **/
    NegativeOnly,

    /**
    * Acknowledgement is sent only on delivery succeeds.
    **/
    PositiveOnly,

    /**
    * An acknowledgement is sent on delivery success or failure.
    **/
    Full
}
