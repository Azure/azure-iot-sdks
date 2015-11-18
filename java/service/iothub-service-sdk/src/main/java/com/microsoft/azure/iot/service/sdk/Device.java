/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.sdk;

import com.microsoft.azure.iot.service.auth.SymmetricKey;

import javax.crypto.KeyGenerator;
import javax.json.Json;
import javax.json.JsonObject;
import javax.json.JsonReader;
import javax.json.stream.JsonParsingException;
import java.io.StringReader;
import java.security.NoSuchAlgorithmException;
import java.util.Base64;

/**
 * The Device class extends the Device class
 * implementing constructors and serialization functionality.
 */
public class Device
{
    protected final String encryptionMethod = "AES";
    protected final String utcTimeDefault = "0001-01-01T00:00:00";

    /**
     * Static create function
     * Creates device object using the given name
     * and initializing with default values
     *
     * @param deviceId - String containing the device name
     * @return Device object
     * @throws IllegalArgumentException This exception is thrown if {@code deviceId} is {@code null} or empty.
     * @throws NoSuchAlgorithmException This exception is thrown if the encryption method is not supported by the keyGenerator
     */
    public static Device createFromId(String deviceId) throws NoSuchAlgorithmException
    {
        // Codes_SRS_SERVICE_SDK_JAVA_DEVICE_12_002: [The function shall throw IllegalArgumentException if the input string is empty or null]
        if (Tools.isNullOrEmpty(deviceId))
        {
            throw new IllegalArgumentException(deviceId);
        }
        // Codes_SRS_SERVICE_SDK_JAVA_DEVICE_12_003: [The constructor shall create a new instance of Device using the given deviceId and return with it]
        Device device = new Device(deviceId);
        return device;
    }

    /**
     * Static create function
     * De-serializes a Device object from JSON string
     *
     * @param jsonString - String containing the JSON serialization of a Device object
     * @return Device object
     * @throws IllegalArgumentException This exception is thrown if {@code jsonString} is {@code null}.
     */
    public static Device createFromJson(String jsonString)
    {
        // Codes_SRS_SERVICE_SDK_JAVA_DEVICE_12_004: [The constructor shall throw IllegalArgumentException if the input string is empty or null]
        if (jsonString == null)
        {
            throw new IllegalArgumentException();
        }

        // Codes_SRS_SERVICE_SDK_JAVA_DEVICE_12_005: [The constructor shall create a JsonObject by parsing the given jsonString]
        try (JsonReader jsonReader = Json.createReader(new StringReader(jsonString)))
        {
            JsonObject jsonObject = jsonReader.readObject();
            // Codes_SRS_SERVICE_SDK_JAVA_DEVICE_12_006: [The constructor shall create a new instance of Device using the created JsonObject and return with it]
            Device device = new Device(jsonObject);
            return device;
        }
    }

    /**
     * Create an Device instance using the given device name
     *
     * @param deviceId Name of the device (used as device id)
     * @throws NoSuchAlgorithmException This exception is thrown if the encryption method is not supported by the keyGenerator
     */
    protected Device(String deviceId) throws NoSuchAlgorithmException
    {
        // Codes_SRS_SERVICE_SDK_JAVA_DEVICE_12_007: [The constructor shall throw IllegalArgumentException if the input string is empty or null]
        if (Tools.isNullOrEmpty(deviceId))
        {
            throw new IllegalArgumentException();
        }

        // Codes_SRS_SERVICE_SDK_JAVA_DEVICE_12_008: [The constructor shall create a new SymmetricKey instance and store it into a member variable]
        SymmetricKey symmKey = new SymmetricKey();
        KeyGenerator keyGenerator = null;
        keyGenerator = KeyGenerator.getInstance(encryptionMethod);
        symmKey.setPrimaryKey(Base64.getEncoder().encodeToString(keyGenerator.generateKey().getEncoded()));
        symmKey.setSecondaryKey(Base64.getEncoder().encodeToString(keyGenerator.generateKey().getEncoded()));
        this.symmetricKey = symmKey;

        // Codes_SRS_SERVICE_SDK_JAVA_DEVICE_12_009: [The constructor shall initialize all properties to default value]
        this.deviceId = deviceId;
        this.generationId = "";
        this.eTag = "";
        this.status = DeviceStatus.Disabled;
        this.statusReason = "";
        this.statusUpdatedTime = utcTimeDefault;
        this.connectionState = DeviceConnectionState.Disconnected;
        this.connectionStateUpdatedTime = utcTimeDefault;
        this.lastActivityTime = utcTimeDefault;
        this.cloudToDeviceMessageCount = 0;
        this.setForceUpdate(false);
    }

    /**
     * Create an Device instance using the given JsonObject
     * @param jsonObject Json object created by parsing device Json string
     *
     * @throws JsonParsingException This exception is thrown if the Json parsing failed
     * @throws IllegalArgumentException This exception is thrown if the input JsonObject is null
     */
    protected Device(JsonObject jsonObject) throws JsonParsingException, IllegalArgumentException
    {
        // Codes_SRS_SERVICE_SDK_JAVA_DEVICE_12_010: [The constructor shall throw IllegalArgumentException if the input object is null]
        if ((jsonObject == JsonObject.NULL) || (jsonObject == null))
        {
            throw new IllegalArgumentException();
        }
        // Codes_SRS_SERVICE_SDK_JAVA_DEVICE_12_011: [The constructor shall initialize all properties from the given Json object]
        this.deviceId = Tools.getValueFromJsonObject(jsonObject, "deviceId");
        if (Tools.isNullOrEmpty(this.deviceId))
        {
            // Codes_SRS_SERVICE_SDK_JAVA_DEVICE_12_0012: [The constructor shall throw IllegalArgumentException if the device deviceId is empty or null]
            throw new IllegalArgumentException("device deviceId cannot be empty or null");
        }
        this.generationId = Tools.getValueFromJsonObject(jsonObject, "generationId");
        this.eTag = Tools.getValueFromJsonObject(jsonObject, "etag");

        // Codes_SRS_SERVICE_SDK_JAVA_DEVICE_12_013: [The function shall throw Exception if the parsing failed]
        JsonObject authenticationObject = jsonObject.getJsonObject("authentication");
        JsonObject symmKeyObject = authenticationObject.getJsonObject("symmetricKey");
        String primKeyValue = Tools.getValueFromJsonObject(symmKeyObject, "primaryKey");
        String secKeyValue = Tools.getValueFromJsonObject(symmKeyObject, "secondaryKey");
        SymmetricKey symmKey = new SymmetricKey();
        symmKey.setPrimaryKey(primKeyValue);
        symmKey.setSecondaryKey(secKeyValue);
        this.symmetricKey = symmKey;

        String state = Tools.getValueFromJsonObject(jsonObject, "status");
        if ("enabled".equals(state.toLowerCase()))
            this.status = DeviceStatus.Enabled;
        else
            this.status = DeviceStatus.Disabled;

        this.statusReason = Tools.getValueFromJsonObject(jsonObject, "statusReason");
        this.statusUpdatedTime = Tools.getValueFromJsonObject(jsonObject, "statusUpdatedTime");

        String connState = Tools.getValueFromJsonObject(jsonObject, "connectionState");
        if ("connected".equals(connState.toLowerCase()))
            this.connectionState = DeviceConnectionState.Connected;
        else
            this.connectionState = DeviceConnectionState.Disconnected;

        this.connectionStateUpdatedTime = Tools.getValueFromJsonObject(jsonObject, "connectionStateUpdatedTime");
        this.lastActivityTime = Tools.getValueFromJsonObject(jsonObject, "lastActivityTime");

        this.cloudToDeviceMessageCount = Tools.getNumberValueFromJsonObject(jsonObject, "cloudToDeviceMessageCount");
    }

    /**
     * Serialize the object to Json format
     *
     * @return Json string containing this Device object
     */
    protected String serializeToJson()
    {
        // Codes_SRS_SERVICE_SDK_JAVA_DEVICE_12_014: [The function shall return with a proper Json string containing all properties]
        StringBuilder jsonBody = new StringBuilder();
        jsonBody.append("{");
        Tools.appendJsonAttribute(jsonBody, "deviceId", this.deviceId, true, false);
        Tools.appendJsonAttribute(jsonBody, "generationId", this.generationId, true, false);
        Tools.appendJsonAttribute(jsonBody, "etag", this.eTag, true, false);
        switch (this.connectionState)
        {
            case Connected:
                Tools.appendJsonAttribute(jsonBody, "connectionState", "Connected", true, false);
                break;
            case Disconnected:
                Tools.appendJsonAttribute(jsonBody, "connectionState", "Disconnected", true, false);
                break;
        }
        Tools.appendJsonAttribute(jsonBody, "connectionStateUpdatedTime", this.connectionStateUpdatedTime, true, false);
        switch (this.status)
        {
            case Enabled:
                Tools.appendJsonAttribute(jsonBody, "status", "enabled", true, false);
                break;
            case Disabled:
                Tools.appendJsonAttribute(jsonBody, "status", "disabled", true, false);
                break;
        }
        Tools.appendJsonAttribute(jsonBody, "statusReason", this.statusReason, true, false);
        Tools.appendJsonAttribute(jsonBody, "statusUpdatedTime", this.statusUpdatedTime, true, false);
        Tools.appendJsonAttribute(jsonBody, "lastActivityTime", this.lastActivityTime, true, false);
        Tools.appendJsonAttribute(jsonBody, "cloudToDeviceMessageCount", String.valueOf(this.cloudToDeviceMessageCount), false, true);

        if (this.symmetricKey != null)
        {
            jsonBody.append(",");

            jsonBody.append("\"");
            jsonBody.append("authentication");
            jsonBody.append("\"");
            jsonBody.append(":");
            jsonBody.append("{");

            jsonBody.append("\"");
            jsonBody.append("symmetricKey");
            jsonBody.append("\"");
            jsonBody.append(":");
            jsonBody.append("{");

            jsonBody.append("\"");
            jsonBody.append("primaryKey");
            jsonBody.append("\"");
            jsonBody.append(":");

            jsonBody.append("\"");
            jsonBody.append(this.symmetricKey.getPrimaryKey());
            jsonBody.append("\"");
            jsonBody.append(",");

            jsonBody.append("\"");
            jsonBody.append("secondaryKey");
            jsonBody.append("\"");
            jsonBody.append(":");

            jsonBody.append("\"");
            jsonBody.append(this.symmetricKey.getSecondaryKey());
            jsonBody.append("\"");

            jsonBody.append("}");
            jsonBody.append("}");
        }
        jsonBody.append("}");
        return jsonBody.toString();
    }

    // Codes_SRS_SERVICE_SDK_JAVA_DEVICE_12_001: [The Device class has the following properties: Id, Etag, Authentication.SymmetricKey, State, StateReason, StateUpdatedTime, ConnectionState, ConnectionStateUpdatedTime, LastActivityTime]

    /**
     * Device name
     * A case-sensitive string (up to 128 char long)
     * of ASCII 7-bit alphanumeric chars
     * + {'-', ':', '.', '+', '%', '_', '#', '*', '?', '!', '(', ')', ',', '=', '@', ';', '$', '''}.
     */
    protected String deviceId;

    /**
     * Getter for device name
     *
     * @return The deviceId string
     */
    public String getDeviceId()
    {
        return deviceId;
    }

    /**
     * Device generation Id
     */
    protected String generationId;

    /**
     * Getter for GenerationId
     *
     * @return The generationId string
     */
    public String getGenerationId()
    {
        return generationId;
    }

    /**
     * Symmetric key to be used to authorize this device.
     * This can also be an array (up to 2) of keys to do key rollover.
     * Format (same as device Id).
     */
    protected SymmetricKey symmetricKey;

    /**
     * Getter for SymmetricKey object
     *
     * @return The symmetricKey object
     */
    public SymmetricKey getSymmetricKey()
    {
        return symmetricKey;
    }

    /**
     * Getter for PrimaryKey part of the SymmetricKey
     *
     * @return The primaryKey string
     */
    public String getPrimaryKey()
    {
        return symmetricKey.getPrimaryKey();
    }

    /**
     * Getter for SecondaryKey part of the SymmetricKey
     *
     * @return The secondaryKey string
     */
    public String getSecondaryKey()
    {
        return symmetricKey.getSecondaryKey();
    }

    /**
     * A string representing a weak ETAG version
     * of this JSON description. This is a hash.
     */
    protected String eTag;

    /**
     * Getter for eTag
     *
     * @return The eTag string
     */
    public String geteTag()
    {
        return eTag;
    }

    /**
     * "Enabled", "Disabled".
     * If "Enabled", this device is authorized to connect.
     * If "Disabled" this device cannot receive or send messages, and statusReason must be set.
     */
    protected DeviceStatus status;

    /**
     * Getter for DeviceStatus object
     * @return The deviceStatus object
     */
    public DeviceStatus getStatus()
    {
        return status;
    }

    /**
     * A 128 char long string storing the reason of suspension.
     * (all UTF-8 chars allowed).
     */
    protected String statusReason;

    /**
     * Getter for status reason
     *
     * @return The statusReason string
     */
    public String getStatusReason()
    {
        return statusReason;
    }

    /**
     * Datetime of last time the state was updated.
     */
    protected String statusUpdatedTime;

    /**
     * Getter for status updated time string
     *
     * @return The string containing the time when the statusUpdated parameter was updated
     */
    public String getStatusUpdatedTime()
    {
        return statusUpdatedTime;
    }

    /**
     * Status of the device:
     * {"connected" | "disconnected"}
     */
    protected DeviceConnectionState connectionState;

    /**
     * Getter for connection state
     *
     * @return The connectionState string
     */
    public DeviceConnectionState getConnectionState()
    {
        return connectionState;
    }

    /**
     * Datetime of last time the connection state was updated.
     */
    protected String connectionStateUpdatedTime;

    /**
     * Getter for connection state updated time
     *
     * @return The string containing the time when the connectionState parameter was updated
     */
    public String getConnectionStateUpdatedTime()
    {
        return connectionStateUpdatedTime;
    }

    /**
     * Datetime of last time the device authenticated, received, or sent a message.
     */
    protected String lastActivityTime;

    /**
     * Getter for last activity time
     *
     * @return The string containing the time when the lastActivity parameter was updated
     */
    public String getLastActivityTime()
    {
        return lastActivityTime;
    }

    /**
     * Number of messages received by the device
     */
    protected long cloudToDeviceMessageCount;

    /**
     * Getter for cloud to device message count
     *
     * @return The string containing the time when the cloudToDeviceMessageCount parameter was updated
     */
    public long getCloudToDeviceMessageCount()
    {
        return cloudToDeviceMessageCount;
    }

    /**
     * Flip-flop helper for sending a forced update
     */
    private Boolean forceUpdate;

    /**
     * Setter for force update boolean
     *
     * @param forceUpdate - Boolean controlling if the update should be forced or not
     */
    protected void setForceUpdate(Boolean forceUpdate)
    {
        if (forceUpdate == null)
        {
            throw new IllegalArgumentException();
        }
        this.forceUpdate = forceUpdate;
    }
}
