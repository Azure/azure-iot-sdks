/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.sdk;

import com.google.gson.*;
import com.microsoft.azure.iot.service.auth.SymmetricKey;

import java.lang.reflect.Type;

public class DeviceSerializer implements JsonSerializer<Device>
{
    private class Authentication
    {
        private SymmetricKey symmetricKey;

        public Authentication(SymmetricKey symmetricKey)
        {
            this.symmetricKey = symmetricKey;
        }
    }

    Gson gson = new Gson();
    @Override
    public JsonElement serialize(Device device, Type type, JsonSerializationContext jsonSerializationContext)
    {
        // CODES_SRS_SERVICE_SDK_JAVA_DEVICE_SERIALIZER_15_001: [The function shall serialize a Device object
        // into a JSON string recognized by the service]
        Authentication authentication = new Authentication(device.symmetricKey);
        JsonElement authenticationJsonElement = gson.toJsonTree(authentication, Authentication.class);
        JsonElement finalJson = gson.toJsonTree(device, type);
        finalJson.getAsJsonObject().add("authentication", authenticationJsonElement);

        return finalJson;
    }
}
