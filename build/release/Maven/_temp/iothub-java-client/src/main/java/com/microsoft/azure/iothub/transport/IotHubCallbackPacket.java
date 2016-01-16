// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

package com.microsoft.azure.iothub.transport;

import com.microsoft.azure.iothub.IotHubStatusCode;
import com.microsoft.azure.iothub.IotHubEventCallback;

/**
 * A packet containing the data needed for an IoT Hub transport to invoke a
 * callback.
 */
public final class IotHubCallbackPacket
{
    protected final IotHubStatusCode status;
    protected final IotHubEventCallback callback;
    protected final Object callbackContext;

    /**
     * Constructor.
     *
     * @param status the response status code.
     * @param callback the callback to be invoked for the completed request.
     * @param callbackContext the context to be passed in to the callback.
     */
    public IotHubCallbackPacket(IotHubStatusCode status,
            IotHubEventCallback callback,
            Object callbackContext)
    {
        // Codes_SRS_IOTHUBCALLBACKPACKET_11_001: [The constructor shall save the status, callback, and callback context.]
        this.status = status;
        this.callback = callback;
        this.callbackContext = callbackContext;
    }

    /**
     * Getter for the response status code.
     *
     * @return the response status code.
     */
    public IotHubStatusCode getStatus()
    {
        // Codes_SRS_IOTHUBCALLBACKPACKET_11_002: [The function shall return the status given in the constructor.]
        return status;
    }

    /**
     * Getter for the callback to be invoked for the completed request.
     *
     * @return the callback function.
     */
    public IotHubEventCallback getCallback()
    {
        // Codes_SRS_IOTHUBCALLBACKPACKET_11_003: [The function shall return the callback given in the constructor.]
        return callback;
    }

    /**
     * Getter for the context to be passed in to the callback when it is
     * invoked.
     *
     * @return the callback context.
     */
    public Object getContext()
    {
        // Codes_SRS_IOTHUBCALLBACKPACKET_11_004: [The function shall return the callback context given in the constructor.]
        return callbackContext;
    }
}
