/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.exceptions;

public class IotHubTooManyRequestsException extends IotHubException 
{
    public IotHubTooManyRequestsException()
    {
        super("Too many requests (throttled)!");
    }
}
