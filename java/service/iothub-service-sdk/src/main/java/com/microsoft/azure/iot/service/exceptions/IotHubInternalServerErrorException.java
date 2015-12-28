/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.exceptions;

/**
 * Create internal server error exception
 */
public class IotHubInternalServerErrorException extends IotHubException
{
    public IotHubInternalServerErrorException()
    {
        super("Internal server error!");
    }
}
