// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

package samples.com.microsoft.azure.iothub;

import com.microsoft.azure.iothub.DeviceClient;
import com.microsoft.azure.iothub.IotHubClientProtocol;
import com.microsoft.azure.iothub.Message;
import com.microsoft.azure.iothub.IotHubStatusCode;
import com.microsoft.azure.iothub.IotHubEventCallback;

import java.io.IOException;
import java.net.URISyntaxException;
import java.security.InvalidKeyException;

import javax.naming.SizeLimitExceededException;


/** Sends a number of event messages to an IoT Hub. */
public class SendEvent
{
    protected static class EventCallback
            implements IotHubEventCallback
    {
        public void execute(IotHubStatusCode status, Object context)
        {
            Integer i = (Integer) context;
            System.out.println("IoT Hub responded to message " + i.toString()
                    + " with status " + status.name());
        }
    }

    /**
     * Sends a number of messages to an IoT Hub. Default protocol is to
     * use HTTPS transport.
     *
     * @param args args[0] = IoT Hub connection string; args[1] = number of
     * requests to send; args[2] = protocol (one of 'https' or 'amqps',
     * optional).
     */
    public static void main(String[] args)
            throws IOException, URISyntaxException
    {
        System.out.println("Starting...");
        System.out.println("Beginning setup.");

        if (args.length <= 1 || args.length >= 4)
        {
            System.out.format(
                    "Expected 2 or 3 arguments but received:\n%d. The program "
                            + "should be called with the: "
                            + "following args: [IoT Hub connection string] "
                            + "[number of requests to send] (https | amqps).\n",
                    args.length);
            return;
        }

        String connString = args[0];
        int numRequests;
        try
        {
            numRequests = Integer.parseInt(args[1]);
        }
        catch (NumberFormatException e)
        {
            System.out.format(
                    "Could not parse the number of requests to send. "
                            + "Expected an int but received:\n%s.\n", args[1]);
            return;
        }
        IotHubClientProtocol protocol;
        if (args.length == 2)
        {
            protocol = IotHubClientProtocol.HTTPS;
        }
        else
        {
            String protocolStr = args[2];
            if (protocolStr.equals("https"))
            {
                protocol = IotHubClientProtocol.HTTPS;
            }
            else if (protocolStr.equals("amqps"))
            {
                protocol = IotHubClientProtocol.AMQPS;
            }
            else
            {
                System.out.format(
                        "Expected argument 2 to be one of 'https' or 'amqps' "
                                + "but received %s. The program should be "
                                + "called with the: following args: "
                                + "[IoT Hub connection string] "
                                + "[number of requests to send] "
                                + "(https | amqps)."
                                + "\n",
                        protocolStr);
                return;
            }
        }

        System.out.println("Successfully read input parameters.");
        System.out.format("Using communication protocol %s.\n",
                protocol.name());

        DeviceClient client = new DeviceClient(connString, protocol);

        System.out.println("Successfully created an IoT Hub client.");

        client.open();

        System.out.println("Opened connection to IoT Hub.");
        System.out.println("Sending the "
                + "following event messages:");

        for (int i = 0; i < numRequests; ++i)
        {
            String msgStr = "Event Message " + Integer.toString(i);
            try
            {
                Message msg = new Message(msgStr);
                msg.setProperty("messageCount", Integer.toString(i));
                System.out.println(msgStr);

                EventCallback callback = new EventCallback();
                client.sendEventAsync(msg, callback, i);
            }
            catch (Exception e)
            {
            }
        }

        client.close();

        System.out.println("Shutting down...");
    }
}
