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
import javax.json.Json;
import javax.json.JsonObject;
import javax.json.JsonReader;
import java.io.StringReader;
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
    public void createFromJson_input_null()
    {
        // Arrange
        String id = null;
        // Act
        Device.createFromJson(id);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_DEVICE_12_004: [The constructor shall throw IllegalArgumentException if the input string is empty or null]
    // Assert
    @Test (expected = Exception.class)
    public void createFromJson_input_empty()
    {
        // Arrange
        String id = "";
        // Act
        Device.createFromJson(id);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_DEVICE_12_005: [The constructor shall create a JsonObject by parsing the given jsonString]
    // Tests_SRS_SERVICE_SDK_JAVA_DEVICE_12_006: [The constructor shall create a new instance of Device using the created JsonObject and return with it]
    @Test
    public void createFromJson_good_case()
    {
        // Arrange
        String jsonString = "{\"deviceId\":\"xxx-device\",\"generationId\":\"111111111111111111\",\"etag\":\"MA==\",\"connectionState\":\"Disconnected\",\"status\":\"disabled\",\"statusReason\":null,\"connectionStateUpdatedTime\":\"0001-01-01T00:00:00\",\"statusUpdatedTime\":\"0001-01-01T00:00:00\",\"lastActivityTime\":\"0001-01-01T00:00:00\",\"cloudToDeviceMessageCount\":0,\"authentication\":{\"symmetricKey\":{\"primaryKey\":\"AAABBBCCC111222333444000\",\"secondaryKey\":\"111222333444555AAABBBCCC\"}}}";
        new Expectations()
        {
            {
                StringReader stringReader = new StringReader(jsonString);
                JsonReader jsonReader = Json.createReader(stringReader);
                JsonObject jsonObject = jsonReader.readObject();
                new Device(jsonObject);
            }
        };
        // Act
        Device device = Device.createFromJson(jsonString);
        // Assert
        assertNotEquals(device, null);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_DEVICE_12_007: [The constructor shall throw IllegalArgumentException if the input string is empty or null]
    // Assert
    @Test (expected = Exception.class)
    public void constructor_string_null() throws NoSuchAlgorithmException
    {
        // Arrange
        String deviceId = null;
        // Act
        Device device = new Device(deviceId);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_DEVICE_12_007: [The constructor shall throw IllegalArgumentException if the input string is empty or null]
    // Assert
    @Test (expected = Exception.class)
    public void constructor_string_empty() throws NoSuchAlgorithmException
    {
        // Arrange
        String deviceId = "";
        // Act
        Device device = new Device(deviceId);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_DEVICE_12_008: [The constructor shall create a new SymmetricKey instance using AES encryption and store it into a member variable]
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

    // Tests_SRS_SERVICE_SDK_JAVA_DEVICE_12_009: [The constructor shall initialize all properties to default value]
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

    // Tests_SRS_SERVICE_SDK_JAVA_DEVICE_12_010: [The constructor shall throw IllegalArgumentException if the input object is null]
    // Assert
    @Test (expected = Exception.class)
    public void constructor_jsonobject_null()
    {
        // Arrange
        JsonObject jsonObject = (JsonObject) null;
        // Act
        Device device = new Device(jsonObject);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_DEVICE_12_011: [The constructor shall initialize all properties from the given Json object]
    @Test
    public void constructor_json_initialize_properties_enabled()
    {
        // Arrange
        String jsonString = "{\"deviceId\":\"xxx-device\",\"generationId\":\"111111111111111111\",\"etag\":\"MA==\",\"connectionState\":\"Disconnected\",\"status\":\"Enabled\",\"statusReason\":\"justbecause\",\"connectionStateUpdatedTime\":\"1111-01-01T00:00:00\",\"statusUpdatedTime\":\"2222-01-01T00:00:00\",\"lastActivityTime\":\"3333-01-01T00:00:00\",\"cloudToDeviceMessageCount\":10,\"authentication\":{\"symmetricKey\":{\"primaryKey\":\"AAABBBCCC111222333444000\",\"secondaryKey\":\"111222333444555AAABBBCCC\"}}}";
        StringReader stringReader = new StringReader(jsonString);
        JsonReader jsonReader = Json.createReader(stringReader);
        JsonObject jsonObject = jsonReader.readObject();
        // Act
        Device device = new Device(jsonObject);
        // Assert
        assertNotEquals(null, device);
        assertEquals("xxx-device", device.getDeviceId());
        assertEquals("111111111111111111", device.getGenerationId());
        assertEquals("MA==", device.geteTag());

        assertEquals("AAABBBCCC111222333444000", device.getPrimaryKey());
        assertEquals("111222333444555AAABBBCCC", device.getSecondaryKey());

        assertEquals(DeviceStatus.Enabled, device.getStatus());
        assertEquals("justbecause", device.getStatusReason());
        assertEquals("2222-01-01T00:00:00", device.getStatusUpdatedTime());

        assertEquals(DeviceConnectionState.Disconnected, device.getConnectionState());
        assertEquals("1111-01-01T00:00:00", device.getConnectionStateUpdatedTime());
        assertEquals("3333-01-01T00:00:00", device.getLastActivityTime());
        assertEquals(10, device.getCloudToDeviceMessageCount());
    }

    // Tests_SRS_SERVICE_SDK_JAVA_DEVICE_12_011: [The constructor shall initialize all properties from the given Json object]
    @Test
    public void constructor_json_initialize_properties_disabled()
    {
        // Arrange
        String jsonString = "{\"deviceId\":\"xxx-device\",\"generationId\":\"111111111111111111\",\"etag\":\"MA==\",\"connectionState\":\"Disconnected\",\"status\":\"Disabled\",\"statusReason\":\"justbecause\",\"connectionStateUpdatedTime\":\"1111-01-01T00:00:00\",\"statusUpdatedTime\":\"2222-01-01T00:00:00\",\"lastActivityTime\":\"3333-01-01T00:00:00\",\"cloudToDeviceMessageCount\":10,\"authentication\":{\"symmetricKey\":{\"primaryKey\":\"AAABBBCCC111222333444000\",\"secondaryKey\":\"111222333444555AAABBBCCC\"}}}";
        StringReader stringReader = new StringReader(jsonString);
        JsonReader jsonReader = Json.createReader(stringReader);
        JsonObject jsonObject = jsonReader.readObject();
        // Act
        Device device = new Device(jsonObject);
        // Assert
        assertNotEquals(null, device);
        assertEquals("xxx-device", device.getDeviceId());
        assertEquals("111111111111111111", device.getGenerationId());
        assertEquals("MA==", device.geteTag());

        assertEquals("AAABBBCCC111222333444000", device.getPrimaryKey());
        assertEquals("111222333444555AAABBBCCC", device.getSecondaryKey());

        assertEquals(DeviceStatus.Disabled, device.getStatus());
        assertEquals("justbecause", device.getStatusReason());
        assertEquals("2222-01-01T00:00:00", device.getStatusUpdatedTime());

        assertEquals(DeviceConnectionState.Disconnected, device.getConnectionState());
        assertEquals("1111-01-01T00:00:00", device.getConnectionStateUpdatedTime());
        assertEquals("3333-01-01T00:00:00", device.getLastActivityTime());
        assertEquals(10, device.getCloudToDeviceMessageCount());
    }

    // Tests_SRS_SERVICE_SDK_JAVA_DEVICE_12_011: [The constructor shall initialize all properties from the given Json object]
    @Test
    public void constructor_json_initialize_properties_disconnected()
    {
        // Arrange
        String jsonString = "{\"deviceId\":\"xxx-device\",\"generationId\":\"111111111111111111\",\"etag\":\"MA==\",\"connectionState\":\"Disconnected\",\"status\":\"enabled\",\"statusReason\":\"justbecause\",\"connectionStateUpdatedTime\":\"1111-01-01T00:00:00\",\"statusUpdatedTime\":\"2222-01-01T00:00:00\",\"lastActivityTime\":\"3333-01-01T00:00:00\",\"cloudToDeviceMessageCount\":10,\"authentication\":{\"symmetricKey\":{\"primaryKey\":\"AAABBBCCC111222333444000\",\"secondaryKey\":\"111222333444555AAABBBCCC\"}}}";
        StringReader stringReader = new StringReader(jsonString);
        JsonReader jsonReader = Json.createReader(stringReader);
        JsonObject jsonObject = jsonReader.readObject();
        // Act
        Device device = new Device(jsonObject);
        // Assert
        assertNotEquals(null, device);
        assertEquals("xxx-device", device.getDeviceId());
        assertEquals("111111111111111111", device.getGenerationId());
        assertEquals("MA==", device.geteTag());

        assertEquals("AAABBBCCC111222333444000", device.getPrimaryKey());
        assertEquals("111222333444555AAABBBCCC", device.getSecondaryKey());

        assertEquals(DeviceStatus.Enabled, device.getStatus());
        assertEquals("justbecause", device.getStatusReason());
        assertEquals("2222-01-01T00:00:00", device.getStatusUpdatedTime());

        assertEquals(DeviceConnectionState.Disconnected, device.getConnectionState());
        assertEquals("1111-01-01T00:00:00", device.getConnectionStateUpdatedTime());
        assertEquals("3333-01-01T00:00:00", device.getLastActivityTime());
        assertEquals(10, device.getCloudToDeviceMessageCount());
    }

    // Tests_SRS_SERVICE_SDK_JAVA_DEVICE_12_011: [The constructor shall initialize all properties from the given Json object]
    @Test
    public void constructor_json_initialize_properties_connected()
    {
        // Arrange
        String jsonString = "{\"deviceId\":\"xxx-device\",\"generationId\":\"111111111111111111\",\"etag\":\"MA==\",\"connectionState\":\"Connected\",\"status\":\"enabled\",\"statusReason\":\"justbecause\",\"connectionStateUpdatedTime\":\"1111-01-01T00:00:00\",\"statusUpdatedTime\":\"2222-01-01T00:00:00\",\"lastActivityTime\":\"3333-01-01T00:00:00\",\"cloudToDeviceMessageCount\":10,\"authentication\":{\"symmetricKey\":{\"primaryKey\":\"AAABBBCCC111222333444000\",\"secondaryKey\":\"111222333444555AAABBBCCC\"}}}";
        StringReader stringReader = new StringReader(jsonString);
        JsonReader jsonReader = Json.createReader(stringReader);
        JsonObject jsonObject = jsonReader.readObject();
        // Act
        Device device = new Device(jsonObject);
        // Assert
        assertNotEquals(null, device);
        assertEquals("xxx-device", device.getDeviceId());
        assertEquals("111111111111111111", device.getGenerationId());
        assertEquals("MA==", device.geteTag());

        assertEquals("AAABBBCCC111222333444000", device.getPrimaryKey());
        assertEquals("111222333444555AAABBBCCC", device.getSecondaryKey());

        assertEquals(DeviceStatus.Enabled, device.getStatus());
        assertEquals("justbecause", device.getStatusReason());
        assertEquals("2222-01-01T00:00:00", device.getStatusUpdatedTime());

        assertEquals(DeviceConnectionState.Connected, device.getConnectionState());
        assertEquals("1111-01-01T00:00:00", device.getConnectionStateUpdatedTime());
        assertEquals("3333-01-01T00:00:00", device.getLastActivityTime());
        assertEquals(10, device.getCloudToDeviceMessageCount());
    }

    // Tests_SRS_SERVICE_SDK_JAVA_DEVICE_12_012: [The constructor shall throw IllegalArgumentException if the device deviceId is empty or null]
    // Assert
    @Test (expected = IllegalArgumentException.class)
    public void constructor_json_device_id_not_defined()
    {
        // Arrange
        String jsonString = "{\"generationId\":\"111111111111111111\",\"etag\":\"MA==\",\"connectionState\":\"Disconnected\",\"status\":\"enabled\",\"statusReason\":\"justbecause\",\"connectionStateUpdatedTime\":\"1111-01-01T00:00:00\",\"statusUpdatedTime\":\"2222-01-01T00:00:00\",\"lastActivityTime\":\"3333-01-01T00:00:00\",\"cloudToDeviceMessageCount\":10,\"authentication\":{\"symmetricKey\":{\"primaryKey\":\"AAABBBCCC111222333444000\",\"secondaryKey\":\"111222333444555AAABBBCCC\"}}}";
        StringReader stringReader = new StringReader(jsonString);
        JsonReader jsonReader = Json.createReader(stringReader);
        JsonObject jsonObject = jsonReader.readObject();
        // Act
        Device device = new Device(jsonObject);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_DEVICE_12_012: [The constructor shall throw IllegalArgumentException if the device deviceId is empty or null]
    // Assert
    @Test (expected = IllegalArgumentException.class)
    public void constructor_json_device_id_empty()
    {
        // Arrange
        String jsonString = "{\"deviceId\":\"\",\"generationId\":\"111111111111111111\",\"etag\":\"MA==\",\"connectionState\":\"Disconnected\",\"status\":\"enabled\",\"statusReason\":\"justbecause\",\"connectionStateUpdatedTime\":\"1111-01-01T00:00:00\",\"statusUpdatedTime\":\"2222-01-01T00:00:00\",\"lastActivityTime\":\"3333-01-01T00:00:00\",\"cloudToDeviceMessageCount\":10,\"authentication\":{\"symmetricKey\":{\"primaryKey\":\"AAABBBCCC111222333444000\",\"secondaryKey\":\"111222333444555AAABBBCCC\"}}}";
        StringReader stringReader = new StringReader(jsonString);
        JsonReader jsonReader = Json.createReader(stringReader);
        JsonObject jsonObject = jsonReader.readObject();
        // Act
        Device device = new Device(jsonObject);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_DEVICE_12_012: [The constructor shall throw IllegalArgumentException if the device deviceId is empty or null]
    // Assert
    @Test (expected = IllegalArgumentException.class)
    public void constructor_json_device_id_null()
    {
        // Arrange
        String jsonString = "{\"deviceId\":null,\"generationId\":\"111111111111111111\",\"etag\":\"MA==\",\"connectionState\":\"Disconnected\",\"status\":\"enabled\",\"statusReason\":\"justbecause\",\"connectionStateUpdatedTime\":\"1111-01-01T00:00:00\",\"statusUpdatedTime\":\"2222-01-01T00:00:00\",\"lastActivityTime\":\"3333-01-01T00:00:00\",\"cloudToDeviceMessageCount\":10,\"authentication\":{\"symmetricKey\":{\"primaryKey\":\"AAABBBCCC111222333444000\",\"secondaryKey\":\"111222333444555AAABBBCCC\"}}}";
        StringReader stringReader = new StringReader(jsonString);
        JsonReader jsonReader = Json.createReader(stringReader);
        JsonObject jsonObject = jsonReader.readObject();
        // Act
        Device device = new Device(jsonObject);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_DEVICE_12_013: [The constructor shall throw Exception if the device authentication key is empty or null]
    // Assert
    @Test (expected = Exception.class)
    public void constructor_json_authentication_empty()
    {
        // Arrange
        String jsonString = "{\"deviceId\":\"xxx-device\",\"generationId\":\"111111111111111111\",\"etag\":\"MA==\",\"connectionState\":\"Disconnected\",\"status\":\"enabled\",\"statusReason\":\"justbecause\",\"connectionStateUpdatedTime\":\"1111-01-01T00:00:00\",\"statusUpdatedTime\":\"2222-01-01T00:00:00\",\"lastActivityTime\":\"3333-01-01T00:00:00\",\"cloudToDeviceMessageCount\":10,\"authentication\"}";
        StringReader stringReader = new StringReader(jsonString);
        JsonReader jsonReader = Json.createReader(stringReader);
        JsonObject jsonObject = jsonReader.readObject();
        // Act
        Device device = new Device(jsonObject);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_DEVICE_12_013: [The constructor shall throw Exception if the device authentication key is empty or null]
    // Assert
    @Test (expected = Exception.class)
    public void constructor_json_authentication_null()
    {
        // Arrange
        String jsonString = "{\"deviceId\":\"xxx-device\",\"generationId\":\"111111111111111111\",\"etag\":\"MA==\",\"connectionState\":\"Disconnected\",\"status\":\"enabled\",\"statusReason\":\"justbecause\",\"connectionStateUpdatedTime\":\"1111-01-01T00:00:00\",\"statusUpdatedTime\":\"2222-01-01T00:00:00\",\"lastActivityTime\":\"3333-01-01T00:00:00\",\"cloudToDeviceMessageCount\":10,\"authentication\":null}";
        StringReader stringReader = new StringReader(jsonString);
        JsonReader jsonReader = Json.createReader(stringReader);
        JsonObject jsonObject = jsonReader.readObject();
        // Act
        Device device = new Device(jsonObject);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_DEVICE_12_014: [The function shall return with a proper Json string containing all properties]
    @Test
    public void serializeToJson_good_case_connected() throws NoSuchAlgorithmException
    {
        // Arrange
        String jsonString = "{\"deviceId\":\"xxx-device\",\"generationId\":\"111111111111111111\",\"etag\":\"MA==\",\"connectionState\":\"Connected\",\"connectionStateUpdatedTime\":\"1111-01-01T00:00:00\",\"status\":\"enabled\",\"statusReason\":\"justbecause\",\"statusUpdatedTime\":\"2222-01-01T00:00:00\",\"lastActivityTime\":\"3333-01-01T00:00:00\",\"cloudToDeviceMessageCount\":10,\"authentication\":{\"symmetricKey\":{\"primaryKey\":\"AAABBBCCC111222333444000\",\"secondaryKey\":\"111222333444555AAABBBCCC\"}}}";
        StringReader stringReader = new StringReader(jsonString);
        JsonReader jsonReader = Json.createReader(stringReader);
        JsonObject jsonObject = jsonReader.readObject();
        Device device = new Device(jsonObject);
        // Act
        String serializedJsonString = device.serializeToJson();
        // Assert
        assertEquals(jsonString, serializedJsonString);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_DEVICE_12_014: [The function shall return with a proper Json string containing all properties]
    @Test
    public void serializeToJson_good_case_disconnected() throws NoSuchAlgorithmException
    {
        // Arrange
        String jsonString = "{\"deviceId\":\"xxx-device\",\"generationId\":\"111111111111111111\",\"etag\":\"MA==\",\"connectionState\":\"Disconnected\",\"connectionStateUpdatedTime\":\"1111-01-01T00:00:00\",\"status\":\"enabled\",\"statusReason\":\"justbecause\",\"statusUpdatedTime\":\"2222-01-01T00:00:00\",\"lastActivityTime\":\"3333-01-01T00:00:00\",\"cloudToDeviceMessageCount\":10,\"authentication\":{\"symmetricKey\":{\"primaryKey\":\"AAABBBCCC111222333444000\",\"secondaryKey\":\"111222333444555AAABBBCCC\"}}}";
        StringReader stringReader = new StringReader(jsonString);
        JsonReader jsonReader = Json.createReader(stringReader);
        JsonObject jsonObject = jsonReader.readObject();
        Device device = new Device(jsonObject);
        // Act
        String serializedJsonString = device.serializeToJson();
        // Assert
        assertEquals(jsonString, serializedJsonString);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_DEVICE_12_014: [The function shall return with a proper Json string containing all properties]
    @Test
    public void serializeToJson_good_case_enabled() throws NoSuchAlgorithmException
    {
        // Arrange
        String jsonString = "{\"deviceId\":\"xxx-device\",\"generationId\":\"111111111111111111\",\"etag\":\"MA==\",\"connectionState\":\"Disconnected\",\"connectionStateUpdatedTime\":\"1111-01-01T00:00:00\",\"status\":\"enabled\",\"statusReason\":\"justbecause\",\"statusUpdatedTime\":\"2222-01-01T00:00:00\",\"lastActivityTime\":\"3333-01-01T00:00:00\",\"cloudToDeviceMessageCount\":10,\"authentication\":{\"symmetricKey\":{\"primaryKey\":\"AAABBBCCC111222333444000\",\"secondaryKey\":\"111222333444555AAABBBCCC\"}}}";
        StringReader stringReader = new StringReader(jsonString);
        JsonReader jsonReader = Json.createReader(stringReader);
        JsonObject jsonObject = jsonReader.readObject();
        Device device = new Device(jsonObject);
        // Act
        String serializedJsonString = device.serializeToJson();
        // Assert
        assertEquals(jsonString, serializedJsonString);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_DEVICE_12_014: [The function shall return with a proper Json string containing all properties]
    @Test
    public void serializeToJson_good_case_disabled() throws NoSuchAlgorithmException
    {
        // Arrange
        String jsonString = "{\"deviceId\":\"xxx-device\",\"generationId\":\"111111111111111111\",\"etag\":\"MA==\",\"connectionState\":\"Disconnected\",\"connectionStateUpdatedTime\":\"1111-01-01T00:00:00\",\"status\":\"disabled\",\"statusReason\":\"justbecause\",\"statusUpdatedTime\":\"2222-01-01T00:00:00\",\"lastActivityTime\":\"3333-01-01T00:00:00\",\"cloudToDeviceMessageCount\":10,\"authentication\":{\"symmetricKey\":{\"primaryKey\":\"AAABBBCCC111222333444000\",\"secondaryKey\":\"111222333444555AAABBBCCC\"}}}";
        StringReader stringReader = new StringReader(jsonString);
        JsonReader jsonReader = Json.createReader(stringReader);
        JsonObject jsonObject = jsonReader.readObject();
        Device device = new Device(jsonObject);
        // Act
        String serializedJsonString = device.serializeToJson();
        // Assert
        assertEquals(jsonString, serializedJsonString);
    }
}
