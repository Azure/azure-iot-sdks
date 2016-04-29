// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

package com.microsoft.azure.iothub.transport.amqps;

/**
 * Defines methods which are triggered by interactions with the IoTHub.
 */
public interface ServerListener
{
    /**
     * Method executed when a message was acknowledged by IoTHub.
     * @param messageHash The hash of the message.
     * @param deliveryState The state of the delivery.
     */
    void messageSent(Integer messageHash, Boolean deliveryState);

    /**
     * Method executed when a message was received from IoTHub.
     * @param message The message received.
     */
    void messageReceived(AmqpsMessage message);

    /**
     * Method executed when the connection with the IoTHub is lost.
     */
    void connectionLost();
}
