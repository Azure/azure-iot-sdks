// Copyright(c) Microsoft.All rights reserved. 
// Licensed under the MIT license.See LICENSE file in the project root for full license information. 

package javaWrapper;

/**
 * An IoT Hub status code. Included in a message from an IoT Hub to a device.
 */
public enum IotHubStatusCode
{
    OK, OK_EMPTY, BAD_FORMAT, UNAUTHORIZED, TOO_MANY_DEVICES,
    HUB_OR_DEVICE_ID_NOT_FOUND,
    PRECONDITION_FAILED, REQUEST_ENTITY_TOO_LARGE, THROTTLED,
    INTERNAL_SERVER_ERROR, SERVER_BUSY, ERROR;
}

