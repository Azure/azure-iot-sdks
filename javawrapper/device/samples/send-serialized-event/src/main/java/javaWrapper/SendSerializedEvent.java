package javaWrapper;


import java.io.IOException;
import java.util.Random;
import com.sun.jna.Pointer;
import com.sun.jna.ptr.IntByReference;

import javaWrapper.DeviceClient.IotHubClientProtocol;
import javaWrapper.Iothub_client_wrapperLibrary.IotHubEventCallback;

import com.google.gson.Gson;


public class SendSerializedEvent {

    private static String connString = "connection string";
    private static IotHubClientProtocol protocol = IotHubClientProtocol.AMQPS;
    private static boolean stopThread = false;
    
    private static class TelemetryDataPoint 
    {
        public String deviceId;
        public double windSpeed;

        public String serialize() 
        {
            Gson gson = new Gson();
            return gson.toJson(this);
        }
    }
    
    private static class MessageSender implements Runnable 
    {
        public volatile boolean stopThread = false;

        public void run()  
        {
            try 
            {
                double avgWindSpeed = 10; // m/s
                Random rand = new Random();
                DeviceClient client;
                client = new DeviceClient(connString, protocol);

                while (!stopThread) 
                {
                    double currentWindSpeed = avgWindSpeed + rand.nextDouble() * 4 - 2;
                    TelemetryDataPoint telemetryDataPoint = new TelemetryDataPoint();
                    telemetryDataPoint.deviceId = "myFirstDevice";
                    telemetryDataPoint.windSpeed = currentWindSpeed;

                    String msgStr = telemetryDataPoint.serialize();
                    Message msg = new Message(msgStr);
                    System.out.println(msgStr);

                    IntByReference data = new IntByReference();
                    Pointer lockobj = data.getPointer();
                    
                    EventCallback callback = new EventCallback();
                    client.sendEventAsync(msg, callback, lockobj);

                    Thread.sleep(1000);
                }
            } catch (Exception e) 
            {
                e.printStackTrace();
            }
        }
    }
    
    
    public static void main(String[] args) throws InterruptedException, IOException 
    {
        MessageSender ms0 = new MessageSender();
        Thread t0 = new Thread(ms0);
        t0.start(); 

        System.out.println("Press ENTER to exit.");
        System.in.read();
        ms0.stopThread = true;
    }

    static class EventCallback implements IotHubEventCallback
    {
        public void execute(int status, Pointer context)
        {
            IotHubStatusCode status1 = IotHubStatusCode.values()[status];
            
            System.out.println("IoT Hub responded to message with status " + status1.name());
         }
    };
}



