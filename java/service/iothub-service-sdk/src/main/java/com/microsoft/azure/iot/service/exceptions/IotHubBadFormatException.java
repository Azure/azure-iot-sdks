/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.exceptions;

/**
 * Create bad message format exception
 */
public class IotHubBadFormatException extends IotHubException 
{
    public IotHubBadFormatException()
    {
        super("Bad message format!");
    }
}
