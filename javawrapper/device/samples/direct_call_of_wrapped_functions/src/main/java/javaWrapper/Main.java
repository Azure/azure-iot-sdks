package javaWrapper;


import java.util.Arrays;
import java.util.List;

import com.sun.jna.Pointer;
import com.sun.jna.Structure;
import com.sun.jna.ptr.IntByReference;

import javaWrapper.Iothub_client_wrapperLibrary.AMQP_Protocol;
import javaWrapper.Iothub_client_wrapperLibrary.HTTP_Protocol;
import javaWrapper.Iothub_client_wrapperLibrary.IOTHUB_CLIENT_HANDLE;
import javaWrapper.Iothub_client_wrapperLibrary.IOTHUB_CLIENT_RESULT;
import javaWrapper.Iothub_client_wrapperLibrary.IOTHUB_MESSAGE_HANDLE;
import javaWrapper.Iothub_client_wrapperLibrary.IotHubEventCallback;
import javaWrapper.Iothub_client_wrapperLibrary.IotHubMessageCallback;
import javaWrapper.Iothub_client_wrapperLibrary.IotHubFileUploadCallback;
import javaWrapper.Iothub_client_wrapperLibrary.MAP_HANDLE;
import javaWrapper.Iothub_client_wrapperLibrary.MQTT_Protocol;


public class Main {

    public static int callbackCounter = 0;
    
    public static IOTHUB_CLIENT_HANDLE handle = new IOTHUB_CLIENT_HANDLE(null);
    public static IOTHUB_MESSAGE_HANDLE messageHandle;
    public static int IOTHUB_CLIENT_OK = 0;
    static final int MESSAGE_COUNT = 5;
    public static EVENT_INSTANCE[] messages = new EVENT_INSTANCE[MESSAGE_COUNT];
    static Pointer receiveContext;
    
    
    public static Iothub_client_wrapperLibrary IoTHub = Iothub_client_wrapperLibrary.INSTANCE;
    
    
    public static void main(String[] args) throws InterruptedException 
    {
        if (args.length == 0 || args.length >= 3)
        {
            System.out.format(
                    "Expected 1 or 2 arguments but received:\n%d. The program "
                            + "should be called with the: "
                            + "following args: \n"
                            + "[Device connection string] - String containing Hostname, Device Id & Device Key in one of the following formats: HostName=<iothub_host_name>;DeviceId=<device_id>;SharedAccessKey=<device_key>\n"
                            + "[Transport] (https | amqps | mqtt).\n",
                    args.length);
            return;
        }

        String connectionString = args[0];
        String protocolStr = args[1];

        if (args.length == 1)
        {
            AMQP_Protocol protocol = new AMQP_Protocol();
            handle = IoTHub.IoTHubClient_CreateFromConnectionString(connectionString, protocol);
        }
        else
        {
            if (protocolStr.equals("https"))
            {
                HTTP_Protocol protocol = new HTTP_Protocol();
                handle = IoTHub.IoTHubClient_CreateFromConnectionString(connectionString, protocol);
            }
            else if (protocolStr.equals("amqps"))
            {
                AMQP_Protocol protocol = new AMQP_Protocol();
                handle = IoTHub.IoTHubClient_CreateFromConnectionString(connectionString, protocol);
            }
            else if (protocolStr.equals("mqtt"))
            {
                MQTT_Protocol protocol = new MQTT_Protocol();
                handle = IoTHub.IoTHubClient_CreateFromConnectionString(connectionString, protocol);
            }
            else
            {
                System.out.format(
                        "Expected argument 1 to be one of 'https', 'amqps' or 'mqtt' "
                                + "but received %s. The program should be "
                                + "called with the: following args:\n"
                                + "[Device connection string] - String containing Hostname, Device Id & Device Key in one of the following formats: HostName=<iothub_host_name>;DeviceId=<device_id>;SharedAccessKey=<device_key>\n"
                                + "[Transport] (https | amqps | mqtt).\n"
                                + "\n",
                        protocolStr);
                return;
            }
        }
        
        System.out.println("Successfully read input parameters.");
        System.out.format("Using communication protocol %s.\n", protocolStr );
        
        System.out.println("library loaded");

        int status = IoTHub.platform_init();
        if (status != 0)
        {
        	 throw new IllegalStateException(
                     "Failed to initialize the platform. ");
        }
        
        if (protocolStr == "https")
        {
            IntByReference timeout = new IntByReference(241000);
            if (IoTHub.IoTHubClient_SetOption(handle.getPointer(), "timeout", timeout) != IOTHUB_CLIENT_OK)
            {
                System.out.println("failure to set option \"timeout\"\r\n");
            }
        
            IntByReference minimumPollingTime = new IntByReference(9);
            if (IoTHub.IoTHubClient_SetOption(handle.getPointer(), "MinimumPollingTime", minimumPollingTime.getPointer()) != IOTHUB_CLIENT_OK)
            {
                System.out.println("failure to set option \"MinimumPollingTime\"\r\n");
            }
        }
        
        if (IoTHub.IoTHubClient_SetMessageCallback(handle.getPointer(), ReceiveMessageCallback, receiveContext) != IOTHUB_CLIENT_OK)
        {
            System.out.println("ERROR: IoTHubClient_SetMessageCallback..........FAILED!\r\n");
        }
        else
        {
            System.out.println("Upload blob to file storage.");
            String filename= "hello_javawrapper_directcall_blob.txt";
            String content = "Hello World from JavaWrapper (Direct Call) Blob APi";
            Pointer userContext = new Pointer(1001);

            IoTHub.IoTHubClient_UploadToBlobAsync(handle.getPointer(), filename, content, content.length(), UploadConfirmationCallback, userContext);

            for (int i = 0; i < MESSAGE_COUNT; i++)
            {
                messages[i] = new EVENT_INSTANCE();
                String message = "test message";

                if ((messages[i].messageHandle = IoTHub.IoTHubMessage_CreateFromString(message)) == null)
                {
                    System.out.println("ERROR: iotHubMessageHandle is NULL!\r\n");
                }
                else
                {
                    MAP_HANDLE propMap;
                    
                    messages[i].messageTrackingId = i;
                    messages[i].write();
                    propMap = IoTHub.IoTHubMessage_Properties(messages[i].messageHandle);
                    String propText = String.format("PropMsg_%d", i);
                    if (IoTHub.Map_AddOrUpdate(propMap, "PropName", propText) != 0)
                    {
                        System.out.println("ERROR: Map_AddOrUpdate Failed!\r\n");
                    }
        
                    if (IoTHub.IoTHubClient_SendEventAsync(handle.getPointer(), messages[i].messageHandle, SendConfirmationCallback, messages[i].getPointer()) != IOTHUB_CLIENT_RESULT.IOTHUB_CLIENT_OK)
                    {
                        System.out.println("ERROR: IoTHubClient_SendEventAsync..........FAILED!\r\n");
                    }
                    else
                    {
                        System.out.println("IoTHubClient_SendEventAsync accepted message  for transmission to IoT Hub.\r\n");
                    }
                }
            }
        }
        
        while(true);
    }

    static IotHubFileUploadCallback UploadConfirmationCallback = new IotHubFileUploadCallback()
    {
        public void execute(int status, Pointer userContextCallback)
        {
            EVENT_INSTANCE eventInstance = new EVENT_INSTANCE(userContextCallback);

            System.out.printf("Confirmation[%d] received for upload, message tracking id = %d with result = %d\r\n", callbackCounter, eventInstance.messageTrackingId, status);

            IoTHub.IoTHubMessage_Destroy(messages[eventInstance.messageTrackingId].messageHandle);
            callbackCounter++;
        }
    };

    static IotHubEventCallback SendConfirmationCallback = new IotHubEventCallback()
    {
        public void execute(int status, Pointer userContextCallback)
        {
            EVENT_INSTANCE eventInstance = new EVENT_INSTANCE(userContextCallback);	
            
            System.out.printf("Confirmation[%d] received for message tracking id = %d with result = %d\r\n", callbackCounter, eventInstance.messageTrackingId, status);
            
            IoTHub.IoTHubMessage_Destroy(messages[eventInstance.messageTrackingId].messageHandle);
            callbackCounter++;
        }
    };
    
    static IotHubMessageCallback ReceiveMessageCallback = new IotHubMessageCallback()
    {
        public int execute(IOTHUB_MESSAGE_HANDLE message, Pointer userContextCallback)
        {
            IntByReference counter = new IntByReference();
            String messageRcvd;
            
            counter.setValue(userContextCallback.getInt(0));
            
            if ((messageRcvd = IoTHub.IoTHubMessage_GetString(messageHandle)) == null)
            {
                System.out.printf("unable to retrieve the message data\r\n");
            }
            else
            {
                System.out.printf("Received Message [%d] with Data: <<<%.*s>>> & Size=%d\r\n", counter.getValue(), messageRcvd.length(), messageRcvd, messageRcvd.length());
            }
            return 0;
            
        }
    };
    
    public static class EVENT_INSTANCE extends Structure 
    {
        public IOTHUB_MESSAGE_HANDLE messageHandle;
        public int messageTrackingId;
        public EVENT_INSTANCE (Pointer p)
        {
            super(p);
            read();
        }
        
        public EVENT_INSTANCE()
        {
            super();
        }
        @Override
        protected List<String> getFieldOrder() 
        {
            return Arrays.asList(new String[]{"messageHandle","messageTrackingId"});
        }
    }
}




