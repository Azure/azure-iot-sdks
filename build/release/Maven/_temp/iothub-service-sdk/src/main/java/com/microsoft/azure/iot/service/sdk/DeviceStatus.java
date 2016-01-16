/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.sdk;

import com.google.gson.annotations.SerializedName;

/**
 * Enum for device status
 */
public enum DeviceStatus
{
    @SerializedName("enabled")
    Enabled,

    @SerializedName("disabled")
    Disabled
}
