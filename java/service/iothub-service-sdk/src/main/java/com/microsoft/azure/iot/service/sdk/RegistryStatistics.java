/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.sdk;

import javax.json.Json;
import javax.json.JsonObject;
import javax.json.JsonReader;
import java.io.StringReader;

/**
 * Provide static function to parse Json string to RegistryStatistics object.
 */
public class RegistryStatistics
{
    protected long totalDeviceCount;
    protected long enabledDeviceCount;
    protected long disabledDeviceCount;

    private RegistryStatistics()
    {
        this.totalDeviceCount = 0;
        this.enabledDeviceCount = 0;
        this.disabledDeviceCount = 0;
    }

    /**
     * Static constructor
     * De-serializes a Device object from JSON string
     *
     * @param jsonString The string containing the JSON serialization of a Device object
     * @return RegistryStatistics object
     * @exception IllegalArgumentException This exception is thrown if {@code jsonString} is {@code null}.
     */
    public static RegistryStatistics parse(String jsonString)
    {
        // Codes_SRS_SERVICE_SDK_JAVA_REGISTRYSTATISTICS_12_001: [The function shall return an empty RegistryStatistics object if the input is empty or null]
        RegistryStatistics registryStatistics = new RegistryStatistics();
        if (!Tools.isNullOrEmpty(jsonString))
        {
            // Codes_SRS_SERVICE_SDK_JAVA_REGISTRYSTATISTICS_12_002: [The function shall throw an Exception if the parsing failed
            try (JsonReader jsonReader = Json.createReader(new StringReader(jsonString)))
            {
                JsonObject jsonObject = jsonReader.readObject();
                if ((jsonObject != JsonObject.NULL) && (jsonObject != null))
                {
                    // Codes_SRS_SERVICE_SDK_JAVA_REGISTRYSTATISTICS_12_003: [The function shall parse all the Json records to the RegistryStatistics object and return with it]
                    registryStatistics.totalDeviceCount = Tools.getNumberValueFromJsonObject(jsonObject, "totalDeviceCount");
                    registryStatistics.enabledDeviceCount = Tools.getNumberValueFromJsonObject(jsonObject, "enabledDeviceCount");
                    registryStatistics.disabledDeviceCount = Tools.getNumberValueFromJsonObject(jsonObject, "disabledDeviceCount");
                }
            }
        }
        return registryStatistics;
    }
}
