/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.sdk;

import mockit.Deencapsulation;
import org.junit.Test;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotEquals;

public class RegistryStatisticsTest
{
    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYSTATISTICS_12_001: [The constructor shall return an empty RegistryStatistics object if the input is empty or null]
    @Test
    public void parse_input_null()
    {
        RegistryStatistics registryStatistics = RegistryStatistics.parse(null);
        assertNotEquals(null, registryStatistics);
        assertEquals(0, (long)Deencapsulation.getField(registryStatistics, "totalDeviceCount"));
        assertEquals(0, (long)Deencapsulation.getField(registryStatistics, "enabledDeviceCount"));
        assertEquals(0, (long)Deencapsulation.getField(registryStatistics, "disabledDeviceCount"));
    }

    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYSTATISTICS_12_001: [The constructor shall return an empty RegistryStatistics object if the input is empty or null]
    @Test
    public void parse_input_empty()
    {
        RegistryStatistics registryStatistics = RegistryStatistics.parse("");
        assertNotEquals(null, registryStatistics);
        assertEquals(0, (long)Deencapsulation.getField(registryStatistics, "totalDeviceCount"));
        assertEquals(0, (long)Deencapsulation.getField(registryStatistics, "enabledDeviceCount"));
        assertEquals(0, (long)Deencapsulation.getField(registryStatistics, "disabledDeviceCount"));
    }

    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYSTATISTICS_12_002: [The constructor shall throw an Exception if the parsing failed]
    @Test (expected = Exception.class)
    public void parse_invalid_json()
    {
        String jsonString = "{";
        RegistryStatistics registryStatistics = RegistryStatistics.parse(jsonString);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYSTATISTICS_12_002: [The constructor shall throw an Exception if the parsing failed]
    @Test (expected = Exception.class)
    public void parse_invalid_json_values()
    {
        String jsonString = "{\"totalDeviceCount\":\"42\",\"enabledDeviceCount\":34,\"disabledDeviceCount\":8}";
        RegistryStatistics registryStatistics = RegistryStatistics.parse(jsonString);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYSTATISTICS_12_003: [The constructor shall parse all the Json records to the RegistryStatistics object and return with it]
    @Test
    public void parse_good_case()
    {
        String jsonString = "{\"totalDeviceCount\":42,\"enabledDeviceCount\":34,\"disabledDeviceCount\":8}";
        RegistryStatistics registryStatistics = RegistryStatistics.parse(jsonString);
        assertNotEquals(null, registryStatistics);
        assertEquals(42, (long)Deencapsulation.getField(registryStatistics, "totalDeviceCount"));
        assertEquals(34, (long)Deencapsulation.getField(registryStatistics, "enabledDeviceCount"));
        assertEquals(8, (long)Deencapsulation.getField(registryStatistics, "disabledDeviceCount"));
    }
}
