/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.auth;

/**
 * Store primary and secondary keys
 * Provide function for key length validation
 */
public class SymmetricKey
{
    private final int MinKeyLengthInBytes = 16;
    private final int MaxKeyLengthInBytes = 64;
    private final String DeviceKeyLengthInvalid = "DeviceKeyLengthInvalid";

    private String primaryKey;
    private String secondaryKey;

    /**
     * Constructor for initialization
     */
    public SymmetricKey()
    {
        this.primaryKey = "";
        this.secondaryKey = "";
    }

    /**
     * Getter for primary key
     * @return primary key
     */
    public String getPrimaryKey()
    {
        return primaryKey;
    }

    /**
     * Setter for primary key
     * Validates the length of the key
     *
     * @param primaryKey
     */
    public void setPrimaryKey(String primaryKey)
    {
        // Codes_SRS_SERVICE_SDK_JAVA_SYMMETRICKEY_12_001: [The function shall throw IllegalArgumentException if the length of the key less than 16 or greater than 64]
        validateDeviceAuthenticationKey(primaryKey);
        // Codes_SRS_SERVICE_SDK_JAVA_SYMMETRICKEY_12_002: [The function shall set the private primaryKey member to the given value if the length validation passed]
        this.primaryKey = primaryKey;
    }

    /**
     * Getter for secondary key
     * @return secondary key
     */
    public String getSecondaryKey()
    {
        return secondaryKey;
    }

    /**
     * Setter for secondary key
     * Validates the length of the key
     *
     * @param secondaryKey
     */
    public void setSecondaryKey(String secondaryKey)
    {
        // Codes_SRS_SERVICE_SDK_JAVA_SYMMETRICKEY_12_003: [The function shall throw IllegalArgumentException if the length of the key less than 16 or greater than 64]
        validateDeviceAuthenticationKey(secondaryKey);
        // Codes_SRS_SERVICE_SDK_JAVA_SYMMETRICKEY_12_003: [The function shall throw IllegalArgumentException if the length of the key less than 16 or greater than 64]
        this.secondaryKey = secondaryKey;
    }

    /**
     * Validate the length of the key
     * @param key
     */
    private void validateDeviceAuthenticationKey(String key)
    {
        if (key != null)
        {
            if ((key.length() < MinKeyLengthInBytes) || (key.length() > MaxKeyLengthInBytes))
            {
                throw new IllegalArgumentException(DeviceKeyLengthInvalid);
            }
        }
    }
}
