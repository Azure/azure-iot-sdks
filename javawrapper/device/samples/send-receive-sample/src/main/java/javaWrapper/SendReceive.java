

//Copyright (c) Microsoft. All rights reserved.
//Licensed under the MIT license. See LICENSE file in the project root for full license information.

package javaWrapper;

import java.io.IOException;
import java.net.URISyntaxException;
import java.util.Arrays;
import java.util.List;
import java.util.Scanner;

import com.sun.jna.Pointer;
import com.sun.jna.Structure;
import com.sun.jna.ptr.IntByReference;

import javaWrapper.DeviceClient.IotHubClientProtocol;
import javaWrapper.Iothub_client_wrapperLibrary.IOTHUB_MESSAGE_HANDLE;
import javaWrapper.Iothub_client_wrapperLibrary.IotHubEventCallback;
import javaWrapper.Iothub_client_wrapperLibrary.IotHubMessageCallback;


/**
* Handles messages from an IoT Hub. Default protocol is to use
* HTTPS transport.
*/
public class SendReceive
{
 /** Used as a counter in the message callback. */
 protected static class Counter extends Structure 
 {
     public int num;
     
     public Counter(int num)
     {
         this.num = num;
     }
     
     public Counter (Pointer p)
     {
         super(p);
         read();
     }
        
     public Counter()
     {
         super();
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
     
     @Override
     protected List<String> getFieldOrder() 
     {
         return Arrays.asList(new String[]{"num"});
     }
     
 }

 protected static class MessageCallback
         implements IotHubMessageCallback
 {
     public int execute(IOTHUB_MESSAGE_HANDLE msg,
             Pointer context)
     {
         Counter counter = new Counter(context);
         
         Message message = new Message();
         System.out.println(
                 "Received message " + counter.toString()
                 + " with content: " + message.getString());

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

         counter.increment();;
         counter.write();
         
         return res.ordinal();
     }
 }

 // Our MQTT doesn't support abandon/reject, so we will only display the messaged received
 // from IoTHub and return COMPLETE
 protected static class MessageCallbackMqtt implements IotHubMessageCallback
 {
     public int execute(IOTHUB_MESSAGE_HANDLE msg, Pointer context)
     {
         Counter counter = new Counter(context);
         
         Message message = new Message();
         System.out.println(
                 "Received message " + counter.toString()
                 + " with content: " + message.getString());

         counter.increment();;

         return IotHubMessageResult.COMPLETE.ordinal();
     }
 }

 protected static class EventCallback implements IotHubEventCallback{
     public void execute(int status, Pointer context){
         Integer i = (int) Pointer.nativeValue(context);  
         IotHubStatusCode status1 = IotHubStatusCode.values()[status];
         
         System.out.println("IoT Hub responded to message "+i.toString()
             + " with status " + status1.name());
     }
 }

 /**
  * Receives requests from an IoT Hub. Default protocol is to use
  * HTTPS transport.
  *
  * @param args args[0] = IoT Hub connection string; args[1] = protocol (one
  * of 'https', 'amqps' or 'mqtt', optional).
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
                     "Expected argument 2 to be one of 'https', 'amqps' or 'mqtt'"
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

     if (protocol == IotHubClientProtocol.MQTT)
     {
         MessageCallbackMqtt callback = new MessageCallbackMqtt();
         Counter counter = new Counter(0);
         client.setMessageCallback(callback, counter.getPointer());
     }
     else
     {
         MessageCallback callback = new MessageCallback();
         Counter counter = new Counter(0);
         client.setMessageCallback(callback, counter.getPointer());
     }

     System.out.println("Successfully set message callback.");

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
                 Pointer ii = new Pointer(i);
                 client.sendEventAsync(msg, eventCallback, ii);
             }
             catch (Exception e)
             {
                 e.printStackTrace();
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

     System.out.println("Shutting down...");
 }
 
}

