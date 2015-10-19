/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.sdk;

import javax.json.JsonObject;

public class IotHubRegistryStatistics extends RegistryStatistics
{
    public IotHubRegistryStatistics()
    {
        this.totalDeviceCount = 0;
        this.enabledDeviceCount = 0;
        this.disabledDeviceCount = 0;
    }

    public IotHubRegistryStatistics(JsonObject jsonObject)
    {
        if ((jsonObject == JsonObject.NULL) || (jsonObject == null))
        {
            throw new IllegalArgumentException();
        }
        try
        {
            this.totalDeviceCount = Tools.getNumberValueFromJsonObject(jsonObject, "totalDeviceCount");
            this.enabledDeviceCount = Tools.getNumberValueFromJsonObject(jsonObject, "enabledDeviceCount");
            this.disabledDeviceCount = Tools.getNumberValueFromJsonObject(jsonObject, "disabledDeviceCount");
        }
        catch (Exception e)
        {
            throw new IllegalArgumentException();
        }
    }

    @Override
    protected void clone(RegistryStatistics registryStatistics)
    {
        this.totalDeviceCount = registryStatistics.totalDeviceCount;
        this.enabledDeviceCount = registryStatistics.enabledDeviceCount;
        this.disabledDeviceCount = registryStatistics.disabledDeviceCount;
    }

    public long getTotalDeviceCount()
    {
        return totalDeviceCount;
    }

    public long getEnabledDeviceCount()
    {
        return enabledDeviceCount;
    }

    public long getDisabledDeviceCount()
    {
        return disabledDeviceCount;
    }
}
