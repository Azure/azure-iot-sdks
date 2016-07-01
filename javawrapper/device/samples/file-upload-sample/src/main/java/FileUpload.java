

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
import javaWrapper.Iothub_client_wrapperLibrary.IotHubFileUploadCallback;


/**
* Uploads a file to associated Azure Storage account.
* Default protocol is to use HTTPS transport.
*/
public class FileUpload
{

protected static class FileUploadCallback implements IotHubFileUploadCallback{
    public void execute(int status, Pointer context){
        Integer i = (int) Pointer.nativeValue(context);
        IotHubStatusCode status1 = IotHubStatusCode.values()[status];

        System.out.println("FileUploadCallback: IoT Hub responded to message "+i.toString()
                + " with status " + status1.name());
    }
}

 /**
  * Uploads a file to associated Azure Storage account.
  * Default protocol is to use HTTPS transport.
  *
  * @param args args[0] = IoT Hub connection string; args[1] = protocol (one
  * of 'https', 'amqps' or 'mqtt', optional).
  */
 public static void main(String[] args)
         throws IOException, URISyntaxException
 {
     System.out.println("Starting...");
     System.out.println("Beginning setup.");

     if (args.length <= 1 || args.length >= 3)
     {
         System.out.format(
                 "Expected 2 or 3 arguments but received:\n%d. The program "
                         + "should be called with the: "
                         + "following args: \n"
                         + "[Device connection string] - String containing Hostname, Device Id & Device Key in one of the following formats: HostName=<iothub_host_name>;DeviceId=<device_id>;SharedAccessKey=<device_key>\n"
                         + "(https | amqps | mqtt).\n",
                 args.length);
         return;
     }

     String connString = args[0];

     IotHubClientProtocol protocol;
     if (args.length == 1)
     {
         protocol = IotHubClientProtocol.AMQPS;
     }
     else
     {
         String protocolStr = args[1];
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
                             + "(https | amqps | mqtt).\n"
                             + "\n",
                     protocolStr);
             return;
         }
     }

     System.out.println("Successfully read input parameters.");
     System.out.format("Using communication protocol %s.\n", protocol.name());

     DeviceClient client = new DeviceClient(connString, protocol);

     System.out.println("Successfully created an IoT Hub client.");
     System.out.println("Opened connection to IoT Hub.");
     System.out.println("Start uploading blob.");

     String filename= "hello_javawrapper_blob.txt";

     String content = "Hello World from JavaWrapper Blob APi";
     FileUploadCallback fileUploadCallback = new FileUploadCallback();
     Pointer userContext = new Pointer(1001);

     client.uploadToBlobAsync(filename, content, content.length(), fileUploadCallback, userContext);

     System.out.println("Press any key to exit...");

     Scanner scanner = new Scanner(System.in);
     scanner.nextLine();

     System.out.println("Shutting down...");
     client.destroy();
 }
 
}

