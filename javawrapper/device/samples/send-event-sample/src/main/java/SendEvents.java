package javaWrapper;

import java.io.IOException;
import java.net.URISyntaxException;
import java.util.Scanner;

import com.sun.jna.Pointer;

import javaWrapper.DeviceClient.IotHubClientProtocol;
import javaWrapper.Iothub_client_wrapperLibrary.IotHubEventCallback;

/** Sends a number of event messages to an IoT Hub. */
public class SendEvents
{
    protected static class EventCallback
            implements IotHubEventCallback
    {
        public void execute(int status, Pointer context)
        {
            Integer i = (int) Pointer.nativeValue(context);

            IotHubStatusCode status1 = IotHubStatusCode.values()[status];
            System.out.println("IoT Hub responded to message " + i.toString()
                    + " with status " + status1.name());
        }

    }
    /**
     * Sends a number of messages to an IoT Hub. Default protocol is to
     * use HTTPS transport.
     *
     * @param args args[0] = IoT Hub connection string; args[1] = number of
     * requests to send; args[2] = protocol (one of 'https' or 'amqps' or 'mqtt',
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
                            + "following args: \n"
                            + "[Device connection string] - String containing Hostname, Device Id & Device Key in one of the following formats: HostName=<iothub_host_name>;DeviceId=<device_id>;SharedAccessKey=<device_key>\n"
                            + "[number of requests to send] (https | amqps | mqtt).\n",
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
            else if (protocolStr.equals("mqtt"))
            {
                protocol = IotHubClientProtocol.MQTT;
            }
            else
            {
                System.out.format(
                        "Expected argument 2 to be one of 'https', 'amqps' or 'mqtt' "
                                + "but received %s. The program should be "
                                + "called with the: following args:\n"
                                + "[Device connection string] - String containing Hostname, Device Id & Device Key in one of the following formats: HostName=<iothub_host_name>;DeviceId=<device_id>;SharedAccessKey=<device_key>\n"
                                + "[number of requests to send]\n"
                                + "(https | amqps | mqtt).\n"
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

        System.out.println("Opened connection to IoT Hub.");
        System.out.println("Sending the "
                + "following event messages:");

        new Thread(() -> {
            
            for (int i = 0; i < numRequests; ++i)
            {
                String msgStr = "Event Message " + Integer.toString(i);
                try
                {
                    Message msg = new Message(msgStr);
                    msg.setProperty("messageCount", Integer.toString(i));
                    System.out.println(msgStr);

                    EventCallback callback = new EventCallback();
                   
                    Pointer ii = new Pointer(i);
                    client.sendEventAsync(msg, callback, ii);
                }
                catch (Exception e)
                {
                    System.out.println(e.getMessage());
                }
                try {
                    Thread.sleep(5000);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }).start();

        Scanner scanner = new Scanner(System.in);
        scanner.nextLine();

        System.out.println("Shutting down...");
    }
}

