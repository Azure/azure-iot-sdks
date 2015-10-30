/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.sdk;

import javax.json.Json;
import javax.json.JsonObject;
import javax.json.JsonReader;
import java.io.StringReader;

public abstract class RegistryStatistics
{
    protected long totalDeviceCount;
    protected long enabledDeviceCount;
    protected long disabledDeviceCount;

    /**
     * Static create function
     * De-serializes a Device object from JSON string
     *
     * @param jsonString - String containing the JSON serialization of a Device object
     * @return RegistryStatistics object
     * @exception IllegalArgumentException This exception is thrown if
     * {@code jsonString} is {@code null}.
     */
    public static RegistryStatistics createFromJson(String jsonString)
    {
        if (jsonString == null)
        {
            throw new IllegalArgumentException();
        }

        try (JsonReader jsonReader = Json.createReader(new StringReader(jsonString)))
        {
            JsonObject jsonObject = jsonReader.readObject();
            IotHubRegistryStatistics iotRegistryStatistics = new IotHubRegistryStatistics(jsonObject);
            return iotRegistryStatistics;
        }

    }

    protected abstract void clone(RegistryStatistics registryStatistics);
}
