/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.exceptions;

/**
 * Create precondition failed exception
 */
public class IotHubPreconditionFailedException extends IotHubException
{
    public IotHubPreconditionFailedException()
    {
        super("Precondition failed!");
    }
}
