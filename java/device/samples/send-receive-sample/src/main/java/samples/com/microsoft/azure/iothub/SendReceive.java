// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

package samples.com.microsoft.azure.iothub;

import com.microsoft.azure.iothub.*;

import java.io.IOException;
import java.net.URISyntaxException;
import java.util.Scanner;


/**
 * Handles messages from an IoT Hub. Default protocol is to use
 * HTTPS transport.
 */
public class SendReceive
{
    /** Used as a counter in the message callback. */
    protected static class Counter
    {
        protected int num;

        public Counter(int num)
        {
            this.num = num;
        }

        public int get()
        {
            return this.num;
        }

        public void increment()
        {
            this.num++;
        }

        @Override
        public String toString()
        {
            return Integer.toString(this.num);
        }
    }

    protected static class MessageCallback
            implements com.microsoft.azure.iothub.MessageCallback
    {
        public IotHubMessageResult execute(Message msg,
                Object context)
        {
            Counter counter = (Counter) context;
            System.out.println(
                    "Received message " + counter.toString()
                            + " with content: " + new String(msg.getBytes(), Message.DEFAULT_IOTHUB_MESSAGE_CHARSET));

            int switchVal = counter.get() % 3;
            IotHubMessageResult res;
            switch (switchVal)
            {
                case 0:
                    res = IotHubMessageResult.COMPLETE;
                    break;
                case 1:
                    res = IotHubMessageResult.ABANDON;
                    break;
                case 2:
                    res = IotHubMessageResult.REJECT;
                    break;
                default:
                    // should never happen.
                    throw new IllegalStateException(
                            "Invalid message result specified.");
            }

            System.out.println(
                    "Responding to message " + counter.toString()
                            + " with " + res.name());

            counter.increment();

            return res;
        }
    }

    protected static class EventCallback implements IotHubEventCallback{
        public void execute(IotHubStatusCode status, Object context){
            Integer i = (Integer) context;
            System.out.println("IoT Hub responded to message "+i.toString()
                + " with status " + status.name());
        }
    }

    /**
     * Receives requests from an IoT Hub. Default protocol is to use
     * HTTPS transport.
     *
     * @param args args[0] = IoT Hub connection string; args[1] = protocol (one
     * of 'https' or 'amqps', optional).
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

        MessageCallback callback = new MessageCallback();
        Counter counter = new Counter(0);
        client.setMessageCallback(callback, counter);

        System.out.println("Successfully set message callback.");

        client.open();

        System.out.println("Opened connection to IoT Hub.");

        System.out.println("Beginning to receive messages...");

        System.out.println("Sending the following event messages: ");

        new Thread(() -> {
            for (int i = 0; i < numRequests; ++i)
            {
                String msgStr = "Event Message " + Integer.toString(i);
                try
                {
                    Message msg = new Message(msgStr);
                    msg.setProperty("messageCount", Integer.toString(i));
                    System.out.println(msgStr);

                    EventCallback eventCallback = new EventCallback();
                    client.sendEventAsync(msg, eventCallback, i);
                }
                catch (Exception e)
                {
                }
                try {
                    Thread.sleep(2000);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }).start();

        System.out.println("Press any key to exit...");

        Scanner scanner = new Scanner(System.in);
        scanner.nextLine();

        client.close();

        System.out.println("Shutting down...");
    }
}
