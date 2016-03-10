/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.sdk;

import com.google.gson.annotations.SerializedName;
import com.microsoft.azure.iot.service.auth.SymmetricKey;

import javax.crypto.KeyGenerator;
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
     * Creates device object using the given name.
     * If input device status and symmetric key are null then they will be auto generated.
     *
     * @param deviceId - String containing the device name
     * @param deviceId - Device status. If parameter is null, then the status will be set to Enabled.
     * @param symmetricKey - Device key. If parameter is null, then the key will be auto generated.
     * @return Device object
     * @throws IllegalArgumentException This exception is thrown if {@code deviceId} is {@code null} or empty.
     * @throws NoSuchAlgorithmException This exception is thrown if the encryption method is not supported by the keyGenerator
     */
    public static Device createFromId(String deviceId, DeviceStatus status, SymmetricKey symmetricKey)
            throws IllegalArgumentException, NoSuchAlgorithmException
    {
        // Codes_SRS_SERVICE_SDK_JAVA_DEVICE_12_002: [The function shall throw IllegalArgumentException if the input string is empty or null]
        if (Tools.isNullOrEmpty(deviceId))
        {
            throw new IllegalArgumentException(deviceId);
        }

        // Codes_SRS_SERVICE_SDK_JAVA_DEVICE_12_003: [The function shall create a new instance
        // of Device using the given deviceId and return it]
        Device device = new Device(deviceId, status, symmetricKey);
        return device;
    }

    /**
     * Create an Device instance using the given device name
     *
     * @param deviceId Name of the device (used as device id)
     * @param deviceId - Device status. If parameter is null, then the status will be set to Enabled.
     * @param symmetricKey - Device key. If parameter is null, then the key will be auto generated.
     * @throws NoSuchAlgorithmException This exception is thrown if the encryption method is not supported by the keyGenerator
     */
    protected Device(String deviceId, DeviceStatus status, SymmetricKey symmetricKey)
            throws NoSuchAlgorithmException, IllegalArgumentException
    {
        // Codes_SRS_SERVICE_SDK_JAVA_DEVICE_12_004: [The constructor shall throw IllegalArgumentException
        // if the input string is empty or null]
        if (Tools.isNullOrEmpty(deviceId))
        {
            throw new IllegalArgumentException();
        }

        // Codes_SRS_SERVICE_SDK_JAVA_DEVICE_15_007: [The constructor shall store
        // the input device status and symmetric key into a member variable]
        // Codes_SRS_SERVICE_SDK_JAVA_DEVICE_12_005: [If the input symmetric key is empty, the constructor shall create
        // a new SymmetricKey instance using AES encryption and store it into a member variable]
        KeyGenerator keyGenerator = KeyGenerator.getInstance(encryptionMethod);
        if (symmetricKey == null)
        {
            symmetricKey = new SymmetricKey();
            Base64.Encoder encoder = Base64.getEncoder();
            symmetricKey.setPrimaryKey(encoder.encodeToString(keyGenerator.generateKey().getEncoded()));
            symmetricKey.setSecondaryKey(encoder.encodeToString(keyGenerator.generateKey().getEncoded()));
        }
        this.symmetricKey = symmetricKey;

        // Codes_SRS_SERVICE_SDK_JAVA_DEVICE_12_006: [The constructor shall initialize all properties to default values]
        this.deviceId = deviceId;
        this.generationId = "";
        this.eTag = "";

        this.status = status != null ? status : DeviceStatus.Enabled;
        this.statusReason = "";
        this.statusUpdatedTime = utcTimeDefault;
        this.connectionState = DeviceConnectionState.Disconnected;
        this.connectionStateUpdatedTime = utcTimeDefault;
        this.lastActivityTime = utcTimeDefault;
        this.cloudToDeviceMessageCount = 0;
        this.setForceUpdate(false);
    }

    // Codes_SRS_SERVICE_SDK_JAVA_DEVICE_12_001: [The Device class has the following properties: Id, Etag,
    // Authentication.SymmetricKey, State, StateReason, StateUpdatedTime,
    // ConnectionState, ConnectionStateUpdatedTime, LastActivityTime]

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
    protected transient SymmetricKey symmetricKey;

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
     * Setter for SymmetricKey object
     *
     * @param symmetricKey
     */
    public void setSymmetricKey(SymmetricKey symmetricKey)
    {
        this.symmetricKey = symmetricKey;
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
    @SerializedName("etag")
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
     * Setter for DeviceStatus object
     *
     * @param status
     */
    public void setStatus(DeviceStatus status)
    {
        this.status = status;
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
    public void setForceUpdate(Boolean forceUpdate)
    {
        if (forceUpdate == null)
        {
            throw new IllegalArgumentException();
        }
        this.forceUpdate = forceUpdate;
    }
}
