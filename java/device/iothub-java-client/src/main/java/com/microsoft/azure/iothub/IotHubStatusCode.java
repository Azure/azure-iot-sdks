// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

package com.microsoft.azure.iothub;

/**
 * An IoT Hub status code. Included in a message from an IoT Hub to a device.
 */
public enum IotHubStatusCode
{
    OK, OK_EMPTY, BAD_FORMAT, UNAUTHORIZED, TOO_MANY_DEVICES,
    HUB_OR_DEVICE_ID_NOT_FOUND,
    PRECONDITION_FAILED, REQUEST_ENTITY_TOO_LARGE, THROTTLED,
    INTERNAL_SERVER_ERROR, SERVER_BUSY, ERROR;

    /**
     * Returns the IoT Hub status code referenced by the HTTPS status code.
     *
     * @param httpsStatus the HTTPS status code.
     *
     * @return the corresponding IoT Hub status code.
     *
     * @throws IllegalArgumentException if the HTTPS status code does not map to
     * an IoT Hub status code.
     */
    public static IotHubStatusCode getIotHubStatusCode(int httpsStatus)
    {
        // Codes_SRS_IOTHUBSTATUSCODE_11_001: [The function shall convert the given HTTPS status code to the corresponding IoT Hub status code.]
        IotHubStatusCode iotHubStatus;
        switch (httpsStatus)
        {
            case 200:
                iotHubStatus = OK;
                break;
            case 204:
                iotHubStatus = OK_EMPTY;
                break;
            case 400:
                iotHubStatus = BAD_FORMAT;
                break;
            case 401:
                iotHubStatus = UNAUTHORIZED;
                break;
            case 403:
                iotHubStatus = TOO_MANY_DEVICES;
                break;
            case 404:
                iotHubStatus = HUB_OR_DEVICE_ID_NOT_FOUND;
                break;
            case 412:
                iotHubStatus = PRECONDITION_FAILED;
                break;
            case 413:
                iotHubStatus = REQUEST_ENTITY_TOO_LARGE;
                break;
            case 429:
                iotHubStatus = THROTTLED;
                break;
            case 500:
                iotHubStatus = INTERNAL_SERVER_ERROR;
                break;
            case 503:
                iotHubStatus = SERVER_BUSY;
                break;
            default:
                // Codes_SRS_IOTHUBSTATUSCODE_11_002: [If the given HTTPS status code does not map to an IoT Hub status code, the function return status code ERROR.]
                iotHubStatus = ERROR;
        }

        return iotHubStatus;
    }
}
