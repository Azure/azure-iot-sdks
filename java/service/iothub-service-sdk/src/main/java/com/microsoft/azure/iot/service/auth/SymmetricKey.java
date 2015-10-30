/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.auth;

public class SymmetricKey
{
    private final int MinKeyLengthInBytes = 16;
    private final int MaxKeyLengthInBytes = 64;
    private final String DeviceKeyLengthInvalid = "DeviceKeyLengthInvalid";

    private String primaryKey;
    private String secondaryKey;

    public SymmetricKey()
    {
        this.primaryKey = "";
        this.secondaryKey = "";
    }
    
    public String getPrimaryKey()
    {
        return primaryKey;
    }

    public void setPrimaryKey(String primaryKey)
    {
        validateDeviceAuthenticationKey(primaryKey, "PrimaryKey");
        this.primaryKey = primaryKey;
    }

    public String getSecondaryKey()
    {
        return secondaryKey;
    }

    public void setSecondaryKey(String secondaryKey)
    {
        validateDeviceAuthenticationKey(secondaryKey, "SecondaryKey");
        this.secondaryKey = secondaryKey;
    }

    private void validateDeviceAuthenticationKey(String key, String paramName)
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
