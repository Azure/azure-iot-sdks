/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.exceptions;

import com.microsoft.azure.iot.service.sdk.Tools;
import com.microsoft.azure.iot.service.transport.http.HttpResponse;

import javax.json.Json;
import javax.json.JsonObject;
import javax.json.JsonReader;
import java.io.StringReader;
import java.nio.charset.StandardCharsets;

/**
 * Provide static function to verify results and throw appropriate exception.
 */
public class IotHubExceptionManager
{
    /**
     * Verify Http response using response status
     *
     * @param httpResponse Http response object to verify
     * @throws IotHubBadFormatException This exception is thrown if the response status equal 400
     * @throws IotHubUnathorizedException This exception is thrown if the response status equal 401
     * @throws IotHubTooManyDevicesException This exception is thrown if the response status equal 403
     * @throws IotHubNotFoundException This exception is thrown if the response status equal 404
     * @throws IotHubPreconditionFailedException This exception is thrown if the response status equal 412
     * @throws IotHubTooManyRequestsException This exception is thrown if the response status equal 429
     * @throws IotHubInternalServerErrorException This exception is thrown if the response status equal 500
     * @throws IotHubServerBusyException This exception is thrown if the response status equal 503
     * @throws IotHubException This exception is thrown if the response status none of them above and greater then 300
     */
    public static void httpResponseVerification(HttpResponse httpResponse)
            throws 
            IotHubBadFormatException, 
            IotHubUnathorizedException, 
            IotHubTooManyDevicesException,
            IotHubPreconditionFailedException,
            IotHubTooManyRequestsException,
            IotHubInternalServerErrorException,
            IotHubServerBusyException,
            IotHubNotFoundException,
            IotHubException
    {
        int responseStatus = httpResponse.getStatus();
        // Codes_SRS_SERVICE_SDK_JAVA_IOTHUBEXCEPTIONMANAGER_12_001: [The function shall throw IotHubBadFormatException if the Http response status equal 400]
        if (400 == responseStatus)
        {
            throw new IotHubBadFormatException();
        }
        // Codes_SRS_SERVICE_SDK_JAVA_IOTHUBEXCEPTIONMANAGER_12_002: [The function shall throw IotHubUnathorizedException if the Http response status equal 401]
        else if (401 == responseStatus)
        {
            throw new IotHubUnathorizedException();
        }
        // Codes_SRS_SERVICE_SDK_JAVA_IOTHUBEXCEPTIONMANAGER_12_002: [The function shall throw IotHubTooManyDevicesException if the Http response status equal 403]
        else if (403 == responseStatus)
        {
            throw new IotHubTooManyDevicesException();
        }
        // Codes_SRS_SERVICE_SDK_JAVA_IOTHUBEXCEPTIONMANAGER_12_002: [The function shall throw IotHubNotFoundException if the Http response status equal 404]
        else if (404 == responseStatus)
        {
            throw new IotHubNotFoundException();
        }
        // Codes_SRS_SERVICE_SDK_JAVA_IOTHUBEXCEPTIONMANAGER_12_002: [The function shall throw IotHubPreconditionFailedException if the Http response status equal 412]
        else if (412 == responseStatus)
        {
            throw new IotHubPreconditionFailedException();
        }
        // Codes_SRS_SERVICE_SDK_JAVA_IOTHUBEXCEPTIONMANAGER_12_002: [The function shall throw IotHubTooManyRequestsException if the Http response status equal 429]
        else if (429 == responseStatus)
        {
            throw new IotHubTooManyRequestsException();
        }
        // Codes_SRS_SERVICE_SDK_JAVA_IOTHUBEXCEPTIONMANAGER_12_002: [The function shall throw IotHubInternalServerErrorException if the Http response status equal 500]
        else if (500 == responseStatus)
        {
            throw new IotHubInternalServerErrorException();
        }
        // Codes_SRS_SERVICE_SDK_JAVA_IOTHUBEXCEPTIONMANAGER_12_002: [The function shall throw IotHubServerBusyException if the Http response status equal 503]
        else if (503 == responseStatus)
        {
            throw new IotHubServerBusyException();
        }
        // Codes_SRS_SERVICE_SDK_JAVA_IOTHUBEXCEPTIONMANAGER_12_002: [The function shall throw IotHubException if the Http response status none of them above and greater than 300 copying the error Http reason to the exception]
        else if (responseStatus > 300)
        {
            try
            {
                String errorMessage = "";
                String jsonString = new String(httpResponse.getErrorReason(), StandardCharsets.UTF_8);
                try (JsonReader jsonReader = Json.createReader(new StringReader(jsonString)))
                {
                    JsonObject jsonObject = jsonReader.readObject();
                    if ((jsonObject != JsonObject.NULL) && (jsonObject != null))
                    {
                        errorMessage = Tools.getValueFromJsonObject(jsonObject, "ExceptionMessage");
                    }
                }
                throw new IotHubException(errorMessage);
            }
            catch (Exception e)
            {
                throw new IotHubException("Unknown error reason");
            }
        }
        // Codes_SRS_SERVICE_SDK_JAVA_IOTHUBEXCEPTIONMANAGER_12_010: [The function shall return without exception if the response status equal or less than 300]
    }
}
