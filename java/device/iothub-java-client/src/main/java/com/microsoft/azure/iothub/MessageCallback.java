// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

package com.microsoft.azure.iothub;

/**
 * An interface for an IoT Hub message callback.
 * <p>
 * The developer is expected to create an implementation of this interface,
 * and the transport will call {@link MessageCallback#execute(Message, Object)}
 * upon receiving a message from an IoT Hub.
 */
public interface MessageCallback
{
    /**
     * Executes the callback. The callback should return a response that
     * instructs an IoT Hub to complete, abandon, or reject the message.
     *
     * @param message the message.
     * @param callbackContext a custom context given by the developer.
     *
     * @return whether the IoT Hub should complete, abandon, or reject the
     * message.
     */
    IotHubMessageResult execute(Message message, Object callbackContext);
}
