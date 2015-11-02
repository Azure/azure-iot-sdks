/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.sdk;

import org.junit.Test;

import static org.junit.Assert.*;

/**
 *
 * @author zolvarga
 */
public class IotHubRegistryStatisticsTest
{
    /**
     * Test of default constructor , of class IotHubRegistryStatistics.
     */
    @Test
    public void testConstructor()
    {
        IotHubRegistryStatistics iotHubRegistryStatistics = new IotHubRegistryStatistics();
        assertEquals(0, iotHubRegistryStatistics.totalDeviceCount);
        assertEquals(0, iotHubRegistryStatistics.enabledDeviceCount);
        assertEquals(0, iotHubRegistryStatistics.disabledDeviceCount);
    }

    /**
     * Test of default constructor , of class IotHubRegistryStatistics.
     */
    @Test (expected = Exception.class)
    public void testConstructorJson()
    {
        IotHubRegistryStatistics iotHubRegistryStatistics = new IotHubRegistryStatistics(null);
    }

    /**
     * Test of clone method, of class IotHubRegistryStatistics.
     * Input: Bad Json
     * Expected: exception
     */
    @Test (expected = Exception.class)
    public void testBadJson()
    {
        String jsonString = "{\"totalDeviceCount\":\"42\",\"enabledDeviceCount\":\"34\",\"disabledDeviceCount\":\"8\"}";
        RegistryStatistics iotHubRegistryStatistics = RegistryStatistics.createFromJson(jsonString);
    }

    /**
     * Test of clone method, of class IotHubRegistryStatistics.
     * Input: Bad Json
     * Expected: exception
     */
    @Test
    public void testClone()
    {
        String jsonString = "{\"totalDeviceCount\":42,\"enabledDeviceCount\":34,\"disabledDeviceCount\":8}";
        RegistryStatistics iotHubRegistryStatistics1 = RegistryStatistics.createFromJson(jsonString);
        assertEquals(42, iotHubRegistryStatistics1.totalDeviceCount);
        assertEquals(34, iotHubRegistryStatistics1.enabledDeviceCount);
        assertEquals(8, iotHubRegistryStatistics1.disabledDeviceCount);
        RegistryStatistics iotHubRegistryStatistics2 = new IotHubRegistryStatistics();
        iotHubRegistryStatistics2.clone(iotHubRegistryStatistics1);
        assertEquals(42, iotHubRegistryStatistics2.totalDeviceCount);
        assertEquals(34, iotHubRegistryStatistics2.enabledDeviceCount);
        assertEquals(8, iotHubRegistryStatistics2.disabledDeviceCount);
    }
}
