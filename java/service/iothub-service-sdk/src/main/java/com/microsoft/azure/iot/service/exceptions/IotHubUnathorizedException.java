/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.exceptions;

/**
 * Create unauthorized exception
 */
public class IotHubUnathorizedException extends IotHubException
{
    public IotHubUnathorizedException()
    {
        super("Unauthorized!");
    }
}
