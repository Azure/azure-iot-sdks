/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.sdk;

import com.microsoft.azure.iot.service.auth.SymmetricKey;

import javax.json.Json;
import javax.json.JsonObject;
import javax.json.JsonReader;
import java.io.StringReader;

/**
 * Device class is the main DTO for the RegistryManager class
 */
public abstract class Device
{
    /**
     * Device name
     * A case-sensitive string (up to 128 char long)
     * of ASCII 7-bit alphanumeric chars
     * + {'-', ':', '.', '+', '%', '_', '#', '*', '?', '!', '(', ')', ',', '=', '@', ';', '$', '''}.
     */
    protected String id;

    public String getId()
    {
        return id;
    }

    /**
     * Device generation Id
     */
    protected String generationId;

    public String getGenerationId()
    {
        return generationId;
    }

    public Boolean getForceUpdate()
    {
        return forceUpdate;
    }

    /**
     * A string representing a weak ETAG version
     * of this JSON description. This is a hash.
     */
    protected String eTag;

    public SymmetricKey getSymmetricKey()
    {
        return symmetricKey;
    }

    public String getPrimaryKey()
    {
        return symmetricKey.getPrimaryKey();
    }

    public String getSecondaryKey()
    {
        return symmetricKey.getSecondaryKey();
    }

    /**
     * Symmetric key to be used to authorize this device.
     * This can also be an array (up to 2) of keys to do key rollover.
     * Format (same as device Id).
     */
    protected SymmetricKey symmetricKey;

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

    public DeviceStatus getStatus()
    {
        return status;
    }

    /**
     * A 128 char long string storing the reason of suspension.
     * (all UTF-8 chars allowed).
     */
    protected String statusReason;

    public String getStatusReason()
    {
        return statusReason;
    }

    /**
     * Datetime of last time the state was updated.
     */
    protected String statusUpdatedTime;

    public String getStatusUpdatedTime()
    {
        return statusUpdatedTime;
    }

    /**
     * Status of the device:
     * {"connected" | "disconnected"}
     */
    protected DeviceConnectionState connectionState;

    public DeviceConnectionState getConnectionState()
    {
        return connectionState;
    }

    /**
     * Datetime of last time the connection state was updated.
     */
    protected String connectionStateUpdatedTime;

    public String getConnectionStateUpdatedTime()
    {
        return connectionStateUpdatedTime;
    }


    /**
     * Datetime of last time the device authenticated, received, or sent a message.
     */
    protected String lastActivityTime;

    public String getLastActivityTime()
    {
        return lastActivityTime;
    }


    /**
     * Number of messages received by the device
     */
    protected long cloudToDeviceMessageCount;

    public long getCloudToDeviceMessageCount()
    {
        return cloudToDeviceMessageCount;
    }

    /**
     * Flip-flop helper for sending a forced update
     */
    protected Boolean forceUpdate;

    /**
     * Static create function
     * Creates device object using the given name
     * and initializing with default values
     *
     * @param id - String containing the device name
     * @return Device object
     * @throws IllegalArgumentException This exception is thrown if
     *                                  {@code id} is {@code null} or empty.
     */
    public static Device createFromId(String id)
    {
        if (Tools.isNullOrEmpty(id))
        {
            throw new IllegalArgumentException(id);
        }
        IotHubDevice iotHubDevice = new IotHubDevice(id);
        return iotHubDevice;
    }

    /**
     * Static create function
     * De-serializes a Device object from JSON string
     *
     * @param jsonString - String containing the JSON serialization of a Device object
     * @return Device object
     * @throws IllegalArgumentException This exception is thrown if
     *                                  {@code jsonString} is {@code null}.
     */
    public static Device createFromJson(String jsonString)
    {
        if (jsonString == null)
        {
            throw new IllegalArgumentException();
        }

        try (JsonReader jsonReader = Json.createReader(new StringReader(jsonString)))
        {
            JsonObject jsonObject = jsonReader.readObject();
            IotHubDevice iotHubDevice = new IotHubDevice(jsonObject);
            return iotHubDevice;
        }
    }

    /**
     * Abstract for cloning the device
     *
     * @param device - Device object cloning from
     */
    protected abstract void clone(Device device);

    /**
     * Abstract function to set forceUpdate
     *
     * @param forceUpdate - Boolean controlling if the update should be forced or not
     */
    protected abstract void setForceUpdate(Boolean forceUpdate);

    /**
     * Abstract for JSON serialization of the object
     *
     * @return String containing the serialized Device object
     */
    protected abstract String serializeToJson();
}
