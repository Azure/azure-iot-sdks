// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

package com.microsoft.azure.iothub;

/**
 * An interface for an IoT Hub event callback.
 *
 * Developers are expected to create an implementation of this interface,
 * and the transport will call {@link IotHubEventCallback#execute(IotHubStatusCode, Object)}
 * upon receiving a response from an IoT Hub.
 */
public interface IotHubEventCallback
{
    /**
     * Executes the callback.
     *
     * @param responseStatus the response status code.
     * @param callbackContext a custom context given by the developer.
     */
    void execute(IotHubStatusCode responseStatus, Object callbackContext);
}
