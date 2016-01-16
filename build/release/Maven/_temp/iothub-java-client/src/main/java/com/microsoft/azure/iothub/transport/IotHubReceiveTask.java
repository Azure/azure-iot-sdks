// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

package com.microsoft.azure.iothub.transport;

/**
 * Polls an IoT Hub for messages and invokes a callback if one is found.
 * Meant to be used with an executor that continuously calls run().
 */
public final class IotHubReceiveTask implements Runnable
{
    protected final IotHubTransport transport;

    public IotHubReceiveTask(IotHubTransport transport)
    {
        // Codes_SRS_IOTHUBRECEIVETASK_11_001: [The constructor shall save the transport.]
        this.transport = transport;
    }

    public void run()
    {
        try
        {
            // Codes_SRS_IOTHUBRECEIVETASK_11_002: [The function shall poll an IoT Hub for messages, invoke the message callback if one exists, and return one of COMPLETE, ABANDON, or REJECT to the IoT Hub.]
            this.transport.handleMessage();
        }
        // Codes_SRS_IOTHUBRECEIVETASK_11_004: [The function shall not crash because of an IOException thrown by the transport.]
        // Codes_SRS_IOTHUBRECEIVETASK_11_005: [The function shall not crash because of any error or exception thrown by the transport.]
        catch (Throwable e)
        {
            System.out.println(e.toString() + ": " + e.getMessage());
            for (StackTraceElement el : e.getStackTrace())
            {
                System.out.println(el);
            }
        }
    }
}