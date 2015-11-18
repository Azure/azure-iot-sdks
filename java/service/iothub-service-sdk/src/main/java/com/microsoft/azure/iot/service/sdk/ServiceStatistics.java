/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.sdk;

/**
 * Data structure for service statistics received in Json array
 * Provide getters and setters for statistics properties
 */
public class ServiceStatistics
{
    private long ConnectedDeviceCount;

    public long getConnectedDeviceCount()
    {
        return ConnectedDeviceCount;
    }

    public void setConnectedDeviceCount(long ConnectedDeviceCount)
    {
        this.ConnectedDeviceCount = ConnectedDeviceCount;
    }
}
