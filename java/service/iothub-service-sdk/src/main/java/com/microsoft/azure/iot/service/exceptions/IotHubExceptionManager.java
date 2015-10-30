/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.exceptions;

import com.microsoft.azure.iot.service.sdk.Tools;
import com.microsoft.azure.iot.service.transport.https.HttpsResponse;

import javax.json.Json;
import javax.json.JsonObject;
import javax.json.JsonReader;
import java.io.StringReader;
import java.nio.charset.StandardCharsets;

public class IotHubExceptionManager
{
    public static enum RegistryManagerAction
    {
        AddDevice,
        GetDevice,
        GetDevices,
        UpdateDevice,
        RemoveDevice,
        GetStatistics
    }
    
    public static void HttpResponseVerification(RegistryManagerAction registryManagerAction, HttpsResponse httpsResponse)
            throws 
            IotHubBadFormatException, 
            IotHubUnathorizedException, 
            IotHubTooManyDevicesException,
            IotHubPreconditionFailedException,
            IotHubTooManyRequestsException,
            IotHubInternalServerErrorException,
            IotHubServerBusyException,
            IotHubHubNotFoundException,
            IotHubException
    {
        int responseStatus = httpsResponse.getStatus();
        if (400 == responseStatus)
        {
            throw new IotHubBadFormatException();
        }
        else if (401 == responseStatus)
        {
            throw new IotHubUnathorizedException();
        }
        else if (403 == responseStatus)
        {
            throw new IotHubTooManyDevicesException();
        }
        else if (404 == responseStatus)
        {
            throw new IotHubHubNotFoundException();
        }
        else if (412 == responseStatus)
        {
            throw new IotHubPreconditionFailedException();
        }
        else if (429 == responseStatus)
        {
            throw new IotHubTooManyRequestsException();
        }
        else if (500 == responseStatus)
        {
            throw new IotHubInternalServerErrorException();
        }
        else if (503 == responseStatus)
        {
            throw new IotHubServerBusyException();
        }
        else if (responseStatus > 300)
        {
            String errorMessage = "";
            String jsonString = new String(httpsResponse.getErrorReason(), StandardCharsets.UTF_8);
            try (JsonReader jsonReader = Json.createReader(new StringReader(jsonString)))
            {
                JsonObject jsonObject = jsonReader.readObject();
                if ((jsonObject != JsonObject.NULL) && (jsonObject != null))
                {
                    errorMessage = Tools.getStringValueFromJsonObject(jsonObject, "ExceptionMessage");
                }
            }
            throw new IotHubException(errorMessage);
        }
    }
}
