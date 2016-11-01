using Microsoft.Azure.Devices.Client;
using Microsoft.SPOT;
using System;
using System.Text;
using System.Threading;

namespace MFTestApplication
{
    public class Program
    {
        // It is NOT a good practice to put device credentials in the code as shown below.
        // This is done in this sample for simplicity purposes.
        // Good practices such as the use of TPM or other secure storage on the device are recommended to store device credentials.
        // String containing Hostname, Device Id & Device Key in one of the following formats:
        //  "HostName=<iothub_host_name>;DeviceId=<device_id>;SharedAccessKey=<device_key>"
        //  "HostName=<iothub_host_name>;CredentialType=SharedAccessSignature;DeviceId=<device_id>;SharedAccessSignature=SharedAccessSignature sr=<iot_host>/devices/<device_id>&sig=<token>&se=<expiry_time>";
        private const string DeviceConnectionString = "<replace>";

        private static int MESSAGE_COUNT = 5;

        public static void Main()
        {
            try
            {
                DeviceClient deviceClient = DeviceClient.CreateFromConnectionString(DeviceConnectionString, TransportType.Http1);

                SendEvent(deviceClient);
                ReceiveCommands(deviceClient);

                Debug.Print("Done!\n");
            }

            catch { };
        }

        static void SendEvent(DeviceClient deviceClient)
        {
            string dataBuffer;

            Debug.Print("Device sending " + MESSAGE_COUNT + " messages to IoTHub...");

            for (int count = 0; count < MESSAGE_COUNT; count++)
            {
                dataBuffer = Guid.NewGuid().ToString();
                Message eventMessage = new Message(Encoding.UTF8.GetBytes(dataBuffer));
                Debug.Print(DateTime.Now.ToLocalTime() + "> Sending message: " + count + ", Data: [" + dataBuffer + "]");

                deviceClient.SendEvent(eventMessage);
            }
        }

        static void ReceiveCommands(DeviceClient deviceClient)
        {
            Debug.Print("Device waiting for commands from IoTHub...");
            Message receivedMessage;
            string messageData;

            while (true)
            {
                receivedMessage = deviceClient.Receive();

                if (receivedMessage != null)
                {
                    StringBuilder sb = new StringBuilder();

                    foreach (byte b in receivedMessage.GetBytes())
                    {
                        sb.Append((char)b);
                    }

                    messageData = sb.ToString();

                    // dispose string builder
                    sb = null;

                    Debug.Print(DateTime.Now.ToLocalTime() + "> Received message: " + messageData);

                    deviceClient.Complete(receivedMessage);
                }

                //  Note: In this sample, the polling interval is set to 
                //  10 seconds to enable you to see messages as they are sent.
                //  To enable an IoT solution to scale, you should extend this //  interval. For example, to scale to 1 million devices, set 
                //  the polling interval to 25 minutes.
                //  For further information, see
                //  https://azure.microsoft.com/documentation/articles/iot-hub-devguide/#messaging
                Thread.Sleep(10000);
            }
        }


    }
}
