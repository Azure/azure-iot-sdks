/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.sdk;

import com.google.gson.*;
import com.microsoft.azure.iot.service.auth.SymmetricKey;

import java.lang.reflect.Type;

public class DeviceDeserializer implements JsonDeserializer<Device>{

    private Device device;
    private Gson gson = new Gson();

    @Override
    public Device deserialize(JsonElement jsonElement, Type type, JsonDeserializationContext jsonDeserializationContext) throws JsonParseException
    {
        // CODES_SRS_SERVICE_SDK_JAVA_DEVICE_DESERIALIZER_15_001: The function shall deserialize the JSON into a Device object
        JsonElement deviceElement = jsonElement.getAsJsonObject();
        device = gson.fromJson(deviceElement, Device.class);
        JsonElement authenticationElement = jsonElement.getAsJsonObject().get("authentication");
        JsonElement symmetricKeyElement = authenticationElement.getAsJsonObject().get("symmetricKey");
        SymmetricKey symmetricKey = gson.fromJson(symmetricKeyElement, SymmetricKey.class);
        device.symmetricKey = symmetricKey;

        return device;
    }
}
