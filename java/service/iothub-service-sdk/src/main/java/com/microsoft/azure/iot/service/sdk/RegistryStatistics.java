/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.sdk;

public class RegistryStatistics
{
    protected long totalDeviceCount;
    protected long enabledDeviceCount;
    protected long disabledDeviceCount;

    private RegistryStatistics()
    {
        this.totalDeviceCount = 0;
        this.enabledDeviceCount = 0;
        this.disabledDeviceCount = 0;
    }
}
