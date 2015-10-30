/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.sdk;

import org.junit.Test;

import javax.json.JsonObject;

import static org.junit.Assert.*;

public class DeviceTest
{
    /**
     * Test of createFromId method, of class Device.
     * Input: null string
     * Expected: exception
     */
    @Test (expected = Exception.class)
    public void testCreateFromIdNull()
    {
        String id = null;
        Device.createFromId(id);
    }

    /**
     * Test of createFromId method, of class Device.
     * Input: empty string
     * Expected: exception
     */
    @Test (expected = Exception.class)
    public void testCreateFromIdEmpty()
    {
        String id = "";
        Device.createFromId(id);
    }

    /**
     * Test of constructor, of class Device.
     * Input: empty string
     * Expected: exception
     */
    @Test (expected = Exception.class)
    public void testCreateEmpty()
    {
        String id = "";
        IotHubDevice device = new IotHubDevice("");
    }

    /**
     * Test of constructor, of class Device.
     * Input: null string
     * Expected: exception
     */
    @Test (expected = Exception.class)
    public void testCreateNull()
    {
        IotHubDevice device = new IotHubDevice((String)null);
    }


    /**
     * Test of constructor, of class Device.
     * Input: null JsonObject
     * Expected: exception
     */
    @Test (expected = Exception.class)
    public void testCreateJsonNull()
    {
        IotHubDevice device = new IotHubDevice((JsonObject) null);
    }

    /**
     * Test of createFromId method, of class Device.
     * Input: valid string
     * Expected: Device object created with default parameters
     */
    @Test
    public void testCreateFromIdGoodCase()
    {
        String id = "XXX";
        Device device = Device.createFromId(id);
        
        assertNotEquals(null, device);
        assertEquals("XXX", device.getId());
        assertEquals("XXX", device.getId());
        assertEquals("", device.getGenerationId());
        assertEquals("", device.geteTag());
        assertNotEquals("", device.getPrimaryKey());
        assertNotEquals("", device.getSecondaryKey());
        assertEquals(DeviceStatus.Disabled, device.getStatus());
        assertEquals("", device.getStatusReason());
        assertEquals("0001-01-01T00:00:00", device.getStatusUpdatedTime());
        assertEquals(DeviceConnectionState.Disconnected, device.getConnectionState());
        assertEquals("0001-01-01T00:00:00", device.getStatusUpdatedTime());
        assertEquals("0001-01-01T00:00:00", device.getConnectionStateUpdatedTime());
        assertEquals("0001-01-01T00:00:00", device.getLastActivityTime());
        assertEquals(0, device.getCloudToDeviceMessageCount());
    }
    
    /**
     * Test of createFromJson method, of class Device.
     */
    @Test (expected = Exception.class)
    public void testCreateFromJsonNull()
    {
        String jsonString = null;
        Device.createFromJson(jsonString);
    }
    
    /**
     * Test of createFromJson method, of class Device.
     */
    @Test (expected = Exception.class)
    public void testCreateFromJsonEmpty()
    {
        String jsonString = "";
        Device.createFromJson(jsonString);
    }
    
    /**
     * Test of createFromJson method, of class Device.
     */
    @Test
    public void testCreateFromJsonGoodCase()
    {
        String jsonString = "{\"deviceId\":\"XXX\",\"generationId\":null,\"etag\":null,\"connectionState\":\"Disconnected\",\"status\":\"enabled\",\"statusReason\":null,\"connectionStateUpdatedTime\":\"0001-01-01T00:00:00\",\"statusUpdatedTime\":\"0001-01-01T00:00:00\",\"lastActivityTime\":\"0001-01-01T00:00:00\",\"cloudToDeviceMessageCount\":0,\"authentication\":{\"symmetricKey\":{\"primaryKey\":\"Si1l8OZpfSdhTF5Cq5APrg3HThsRFhWciDS8alRsJmU=\",\"secondaryKey\":\"11BUdSwTcsDhiJoRMmQFF58qTaaq01CAVLroYq1GVAk=\"}}}";
        Device device = Device.createFromJson(jsonString);
        assertNotEquals(null, device);
        assertEquals("XXX", device.getId());
        assertEquals("XXX", device.getId());
        assertEquals("", device.getGenerationId());
        assertEquals("", device.geteTag());
        assertNotEquals("", device.getPrimaryKey());
        assertNotEquals("", device.getSecondaryKey());
        assertEquals(DeviceStatus.Enabled, device.getStatus());
        assertEquals("", device.getStatusReason());
        assertEquals("0001-01-01T00:00:00", device.getStatusUpdatedTime());
        assertEquals(DeviceConnectionState.Disconnected, device.getConnectionState());
        assertEquals("0001-01-01T00:00:00", device.getStatusUpdatedTime());
        assertEquals("0001-01-01T00:00:00", device.getConnectionStateUpdatedTime());
        assertEquals("0001-01-01T00:00:00", device.getLastActivityTime());
        assertEquals(0, device.getCloudToDeviceMessageCount());
    }
    
    /**
     * Test of clone method, of class Device.
     */
    @Test (expected = Exception.class)
    public void testCloneGoodCaseNull()
    {
        Device device2 = Device.createFromId("YYY");
        device2.clone(null);
    }

    /**
     * Test of clone method, of class Device.
     */
    @Test
    public void testCloneGoodCase()
    {
        String jsonString = "{\"deviceId\":\"XXX\",\"generationId\":\"generation1\",\"etag\":\"abcdefg\",\"connectionState\":\"Disconnected\",\"status\":\"enabled\",\"statusReason\":null,\"connectionStateUpdatedTime\":\"0001-01-01T00:00:00\",\"statusUpdatedTime\":\"0001-01-01T00:00:00\",\"lastActivityTime\":\"0001-01-01T00:00:00\",\"cloudToDeviceMessageCount\":0,\"authentication\":{\"symmetricKey\":{\"primaryKey\":\"Si1l8OZpfSdhTF5Cq5APrg3HThsRFhWciDS8alRsJmU=\",\"secondaryKey\":\"11BUdSwTcsDhiJoRMmQFF58qTaaq01CAVLroYq1GVAk=\"}}}";
        Device device1 = Device.createFromJson(jsonString);

        Device device2 = Device.createFromId("YYY");
        device2.clone(device1);
        
        assertEquals(device1.getId(), device2.getId());
        assertEquals(device1.getGenerationId(), device2.getGenerationId());
        assertEquals(device1.geteTag(), device2.geteTag());
        assertEquals(device1.getPrimaryKey(), device2.getPrimaryKey());
        assertEquals(device1.getSecondaryKey(), device2.getSecondaryKey());
        assertEquals(device1.getStatus(), device2.getStatus());
        assertEquals(device1.getStatusReason(), device2.getStatusReason());
        assertEquals(device1.getStatusUpdatedTime(), device2.getStatusUpdatedTime());
        assertEquals(device1.getConnectionState(), device2.getConnectionState());
        assertEquals(device1.getStatusUpdatedTime(), device1.getStatusUpdatedTime());
        assertEquals(device1.getConnectionStateUpdatedTime(), device1.getConnectionStateUpdatedTime());
        assertEquals(device1.getLastActivityTime(), device2.getLastActivityTime());
        assertEquals(device1.getCloudToDeviceMessageCount(), device2.getCloudToDeviceMessageCount());
    }

    /**
     * Test of setForceUpdate method, of class Device.
     */
    @Test (expected = Exception.class)
    public void testSetForceUpdate()
    {
        Boolean forceUpdate = null;
        Device device = Device.createFromId("XXX");
        device.setForceUpdate(forceUpdate);
    }

    /**
     * Test of serializeToJson method, of class Device.
     */
    @Test
    public void testSerializeToJson()
    {
        String jsonString1 = "{\"deviceId\":\"XXX\",\"generationId\":\"generation1\",\"etag\":\"abcdefg\",\"connectionState\":\"Disconnected\",\"connectionStateUpdatedTime\":\"0001-01-01T00:00:00\",\"status\":\"enabled\",\"statusReason\":null,\"statusUpdatedTime\":\"0001-01-01T00:00:00\",\"lastActivityTime\":\"0001-01-01T00:00:00\",\"cloudToDeviceMessageCount\":0,\"authentication\":{\"symmetricKey\":{\"primaryKey\":\"Si1l8OZpfSdhTF5Cq5APrg3HThsRFhWciDS8alRsJmU=\",\"secondaryKey\":\"11BUdSwTcsDhiJoRMmQFF58qTaaq01CAVLroYq1GVAk=\"}}}";
        Device device = Device.createFromJson(jsonString1);
        String jsonString2 = device.serializeToJson();
        assertEquals(jsonString1, jsonString2);
    }
}
