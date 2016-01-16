/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.sdk;

import com.microsoft.azure.iot.service.auth.SymmetricKey;
import mockit.Expectations;
import mockit.NonStrictExpectations;
import org.junit.Test;

import javax.crypto.KeyGenerator;
import java.security.NoSuchAlgorithmException;

import static junit.framework.TestCase.assertEquals;
import static org.junit.Assert.assertNotEquals;

public class DeviceTest
{
    // Tests_SRS_SERVICE_SDK_JAVA_DEVICE_12_001: [The Device class has the following properties: Id, Etag, Authentication.SymmetricKey, State, StateReason, StateUpdatedTime, ConnectionState, ConnectionStateUpdatedTime, LastActivityTime]
    // Assert
    @Test (expected = IllegalArgumentException.class)
    public void device_get_all_properties() throws NoSuchAlgorithmException
    {
        // Arrange
        String id = "xxx-device";
        // Act
        Device device = Device.createFromId(id);
        device.getDeviceId();
        device.getGenerationId();
        device.getSymmetricKey();
        device.getPrimaryKey();
        device.getSecondaryKey();
        device.geteTag();
        device.getStatus();
        device.getStatusReason();
        device.getStatusUpdatedTime();
        device.getConnectionState();
        device.getConnectionStateUpdatedTime();
        device.getLastActivityTime();
        device.getCloudToDeviceMessageCount();
        device.setForceUpdate(true);
        device.setForceUpdate(null);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_DEVICE_12_002: [The constructor shall throw IllegalArgumentException if the input string is empty or null]
    // Assert
    @Test (expected = Exception.class)
    public void createFromId_input_null() throws NoSuchAlgorithmException
    {
        // Arrange
        String id = null;
        // Act
        Device.createFromId(id);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_DEVICE_12_002: [The constructor shall throw IllegalArgumentException if the input string is empty or null]
    // Assert
    @Test (expected = Exception.class)
    public void createFromId_input_empty() throws NoSuchAlgorithmException
    {
        // Arrange
        String id = "";
        // Act
        Device.createFromId(id);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_DEVICE_12_003: [The constructor shall create a new instance of Device using the given deviceId and return with it]
    @Test
    public void createFromId_good_case() throws NoSuchAlgorithmException
    {
        // Arrange
        String deviceId = "xxx-device";
        new Expectations()
        {
            {
                new Device(deviceId);
            }
        };
        // Act
        Device device = Device.createFromId(deviceId);
        // Assert
        assertNotEquals(device, null);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_DEVICE_12_004: [The constructor shall throw IllegalArgumentException if the input string is empty or null]
    // Assert
    @Test (expected = Exception.class)
    public void constructor_string_null() throws NoSuchAlgorithmException
    {
        // Arrange
        String deviceId = null;
        // Act
        Device device = new Device(deviceId);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_DEVICE_12_004: [The constructor shall throw IllegalArgumentException if the input string is empty or null]
    // Assert
    @Test (expected = Exception.class)
    public void constructor_string_empty() throws NoSuchAlgorithmException
    {
        // Arrange
        String deviceId = "";
        // Act
        Device device = new Device(deviceId);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_DEVICE_12_005: [The constructor shall create a new SymmetricKey instance using AES encryption and store it into a member variable]
    @Test
    public void constructor_create_symmetrickey() throws NoSuchAlgorithmException
    {
        // Arrange
        String encryptionMethod = "AES";
        String deviceId = "xxx-device";
        new NonStrictExpectations()
        {
            {
                SymmetricKey symmetricKey = new SymmetricKey();
                KeyGenerator.getInstance(encryptionMethod);
            }
        };
        // Act
        Device device = new Device(deviceId);
        // Assert
        assertNotEquals(device.getSymmetricKey(), null);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_DEVICE_12_006: [The constructor shall initialize all properties to default value]
    @Test
    public void constructor_initialize_properties() throws NoSuchAlgorithmException
    {
        // Arrange
        String utcTimeDefault = "0001-01-01T00:00:00";
        String jsonString = "xxx-device";
        // Act
        Device device = new Device(jsonString);
        // Assert
        assertNotEquals(null, device);
        assertNotEquals(device.getSymmetricKey(), null);

        assertEquals("xxx-device", device.getDeviceId());
        assertEquals("", device.getGenerationId());
        assertEquals("", device.geteTag());
        assertEquals(DeviceStatus.Disabled, device.getStatus());
        assertEquals("", device.getStatusReason());
        assertEquals(utcTimeDefault, device.getStatusUpdatedTime());
        assertEquals(DeviceConnectionState.Disconnected, device.getConnectionState());
        assertEquals(utcTimeDefault, device.getStatusUpdatedTime());
        assertEquals(utcTimeDefault, device.getConnectionStateUpdatedTime());
        assertEquals(utcTimeDefault, device.getLastActivityTime());
        assertEquals(0, device.getCloudToDeviceMessageCount());
    }
}