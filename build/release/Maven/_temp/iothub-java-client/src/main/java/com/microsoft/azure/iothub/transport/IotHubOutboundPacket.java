// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

package com.microsoft.azure.iothub.transport;

import com.microsoft.azure.iothub.IotHubEventCallback;
import com.microsoft.azure.iothub.Message;

/**
 * A packet containing the data needed for an IoT Hub transport to send a
 * message.
 */
public final class IotHubOutboundPacket
{
    protected final Message message;
    protected final IotHubEventCallback callback;
    protected final Object callbackContext;

    /**
     * Constructor.
     *
     * @param message the message to be sent.
     * @param callback the callback to be invoked when a response from the IoT
     * Hub is received.
     * @param callbackContext the context to be passed to the callback.
     */
    public IotHubOutboundPacket(Message message,
            IotHubEventCallback callback,
            Object callbackContext)
    {
        // Codes_SRS_IOTHUBOUTBOUNDPACKET_11_001: [The constructor shall save the message, callback, and callback context.]
        this.message = message;
        this.callback = callback;
        this.callbackContext = callbackContext;
    }

    /**
     * Getter for the message to be sent.
     *
     * @return the message to be sent.
     */
    public Message getMessage()
    {
        // Codes_SRS_IOTHUBOUTBOUNDPACKET_11_002: [The function shall return the message given in the constructor.]
        return message;
    }

    /**
     * Getter for the callback to be invoked when a response is received.
     *
     * @return the callback function.
     */
    public IotHubEventCallback getCallback()
    {
        // Codes_SRS_IOTHUBOUTBOUNDPACKET_11_003: [The function shall return the callback given in the constructor.]
        return callback;
    }

    /**
     * Getter for the context to be passed to the callback when it is invoked.
     *
     * @return the callback context.
     */
    public Object getContext()
    {
        // Codes_SRS_IOTHUBOUTBOUNDPACKET_11_004: [The function shall return the callback context given in the constructor.]
        return callbackContext;
    }
}
