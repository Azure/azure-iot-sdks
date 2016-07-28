// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

package samples.com.microsoft.azure.iothub;

import com.microsoft.azure.iothub.*;

import java.io.IOException;
import java.net.URISyntaxException;
import java.util.Scanner;


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
     * requests to send; args[2] = protocol (one of 'https' or 'amqps' or 'mqtt' or 'amqps_ws',
     * optional); args[3] = path to certificate to enable one-way authentication over ssl for amqps
     * (optional, default shall be used if unspecified).
     */
    public static void main(String[] args)
            throws IOException, URISyntaxException
    {
        System.out.println("Starting...");
        System.out.println("Beginning setup.");

        if (args.length <= 1 || args.length >= 5)
        {
            System.out.format(
                    "Expected 2 or 3 arguments but received:\n%d. The program "
                            + "should be called with the: "
                            + "following args: \n"
                            + "[Device connection string] - String containing Hostname, Device Id & Device Key in one of the following formats: HostName=<iothub_host_name>;DeviceId=<device_id>;SharedAccessKey=<device_key>\n"
                            + "[number of requests to send] (https | amqps | mqtt | amqps_ws).\n"
                            + "path to certificate to enable one-way authentication over ssl for amqps. \n",
                    args.length);
            return;
        }

        String connString = args[0];
        int numRequests;
        String pathToCertificate = null;
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
            else if (protocolStr.equals("mqtt"))
            {
                protocol = IotHubClientProtocol.MQTT;
            }
            else if (protocolStr.equals("amqps_ws"))
            {
                protocol = IotHubClientProtocol.AMQPS_WS;
            }
            else
            {
                System.out.format(
                        "Expected argument 2 to be one of 'https', 'amqps' or 'mqtt'or 'amqps_ws'"
                                + "but received %s. The program should be "
                                + "called with the: following args:\n"
                                + "[Device connection string] - String containing Hostname, Device Id & Device Key in one of the following formats: HostName=<iothub_host_name>;DeviceId=<device_id>;SharedAccessKey=<device_key>\n"
                                + "[number of requests to send]\n"
                                + "(https | amqps | mqtt | amqps_ws).\n"
                                + "\n",
                        protocolStr);
                return;
            }

            if (args.length == 3)
            {
                pathToCertificate = null;
            }
            else
            {
                pathToCertificate = args[3];
            }
        }


        System.out.println("Successfully read input parameters.");
        System.out.format("Using communication protocol %s.\n",
                protocol.name());

        DeviceClient client = new DeviceClient(connString, protocol);
        if (pathToCertificate != null )
        {
            client.setOption("SetCertificatePath", pathToCertificate );
        }

        System.out.println("Successfully created an IoT Hub client.");

        // Set your token expiry time limit here
        long time = 2400;
        client.setOption("SetSASTokenExpiryTime", time);
        System.out.println("Updated token expiry time to " + time);

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
                msg.setExpiryTime(5000);
                System.out.println(msgStr);

                EventCallback callback = new EventCallback();
                client.sendEventAsync(msg, callback, i);
            }
            catch (Exception e)
            {
            }
            try {
                Thread.sleep(200);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }


        Scanner scanner = new Scanner(System.in);
        scanner.nextLine();

        client.close();

        System.out.println("Shutting down...");

    }
}
