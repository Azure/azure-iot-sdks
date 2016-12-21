using Amqp;
using Amqp.Framing;
using Amqp.Types;
using IoTHubAmqp;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace UWPIoTHubSDK
{
    public class IoTHub
    {
        private string HOST = null; 
        private  int PORT = 5671; // Default port calue for amqps
        private static string DEVICE_ID = null;
        private  string SHARED_ACCESS_KEY_NAME = null;
        private  string SHARED_ACCESS_KEY = null;
        private static Address address;
        private static Connection connection;
        private static Session session;
        private bool g_bThreadActive = true;

        const long epochTicks = 621355968000000000; // 1970-1-1 00:00:00 UTC
        const long ticksPerMillisecond = 10000;

        //Delegate for Receiving the data in the application
        public delegate void ReceivedData(byte[] data, string devid);
        //Delegate for Receiving the feedback for the data sent in the application
        public delegate void ReceivedFeedBackData(string FeedBack);

        string sbNamespace = null;
        string keyName = null;
        string keyValue = null;
        string entity = null;
        static object SendSync = new object();
        // Constructor for IoT hub
        public IoTHub(string NameSpace, string EventCompatibleNameSpace , string DeviceName, string KeyName , string KeyValue)
        {
            // Namespace refers to the string before the .azure-devices.net url
            HOST = NameSpace + ".azure-devices.net";
            // AMQPS port number
            PORT = 5671; 
            // DeviceName refers to the name of the device in the IoTHub
            DEVICE_ID = DeviceName;
            // KeyName and Key Value refers to the Access key name and Access key in the Azure portal
            SHARED_ACCESS_KEY_NAME = KeyName;
            SHARED_ACCESS_KEY = KeyValue;
            // Eventhubcompatible name space is the name space provided in the azure IoT Hub portal
            sbNamespace = EventCompatibleNameSpace;

            keyName = SHARED_ACCESS_KEY_NAME;
            keyValue = SHARED_ACCESS_KEY;
            entity = NameSpace;
        }
        //This Methods sends the given string to the device end point
        public void SendCloudToDeviceMessage(String data, ReceivedFeedBackData callback = null)
        {
            Task taskB = new Task(() =>
            {
                SendCommand(data,callback);
            });
            taskB.Start();
          //  taskB.Wait();
        }
        // This function is used to convert the give Date time to milliseconds
        private static long DateTimeToTimestamp(DateTime dateTime)
        {
            return (long)((dateTime.ToUniversalTime().Ticks - epochTicks) / ticksPerMillisecond);
        }
        public void ReceiveMessagesFromDevice(string partitionId, DateTime StartingTime, ReceivedData callback)
        {
            ReceiveMessagesWithTimestampFilter(partitionId, DateTimeToTimestamp(StartingTime), callback);
        }
        // This function receives the messages that are sent after the time stamp mentioned
        private void ReceiveMessagesWithTimestampFilter(string partitionId, long timestamp, ReceivedData callback)
        {
            ReceiveMessages("receive from timestamp checkpoint", 2, "amqp.annotation.x-opt-enqueuedtimeutc > " + timestamp, partitionId, callback);
        }
        // This functionis used to query the Management data
        private Message QueryManagementData()
        {
            Address address = new Address(sbNamespace, PORT, keyName, keyValue);
            Connection connection = new Connection(address);

            Trace.WriteLine(Amqp.TraceLevel.Information, "Creating a session...");
            Session session = new Session(connection);

            // create a pair of links for request/response
            Trace.WriteLine(Amqp.TraceLevel.Information, "Creating a request and a response link...");
            string clientNode = "client-temp-node";
            SenderLink sender = new SenderLink(session, "mgmt-sender", "$management");
            ReceiverLink receiver = new ReceiverLink(
                session,
                "mgmt-receiver",
                new Attach()
                {
                    Source = new Source() { Address = "$management" },
                    Target = new Target() { Address = clientNode }
                },
                null);

            Message request = new Message();
            request.Properties = new Properties() { MessageId = "request1", ReplyTo = clientNode };
            request.ApplicationProperties = new ApplicationProperties();
            request.ApplicationProperties["operation"] = "READ";
            request.ApplicationProperties["name"] = entity;
            request.ApplicationProperties["type"] = "com.microsoft:eventhub";
            sender.Send(request, null, null);

            Message response = receiver.Receive();
            if (response == null)
            {
                throw new Exception("No response was received.");
            }

            receiver.Accept(response);
            receiver.Close();
            sender.Close();
            connection.Close();
            return response;
        }
        public string[] GetPartitions()
        {
            Message response = QueryManagementData();

            Trace.WriteLine(Amqp.TraceLevel.Information, "Partition info {0}", response.Body.ToString());
            string[] partitions = (string[])((Map)response.Body)["partition_ids"];
            Trace.WriteLine(Amqp.TraceLevel.Information, "Partitions {0}", string.Join(",", partitions));
            Trace.WriteLine(Amqp.TraceLevel.Information, "");

            return partitions;
        }
        
        // Closes all the active threads for receive 
        public void CleanUp()
        {   
            g_bThreadActive = false;
        }

        // This function is the core receive function
        void ReceiveMessages(string scenario, int count, string filter, string partition,ReceivedData callback)
        {
            Trace.WriteLine(Amqp.TraceLevel.Information, "Running scenario '{0}', filter '{1}'...", scenario, filter);

            Trace.WriteLine(Amqp.TraceLevel.Information, "Establishing a connection...");
            Address address = new Address(sbNamespace, PORT, keyName, keyValue);
            Connection connection = new Connection(address);

            Trace.WriteLine(Amqp.TraceLevel.Information, "Creating a session...");
            Session session = new Session(connection);

            Trace.WriteLine(Amqp.TraceLevel.Information, "Creating a receiver link on partition {0}...", partition);
            string partitionAddress = entity + "/ConsumerGroups/$default/Partitions/" + partition;
            // Form the filter for receiving message based on the time stamp 
            Map filters = new Map();
            if (filter != null)
            {
                filters.Add(new Amqp.Types.Symbol("apache.org:selector-filter:string"),
                    new DescribedValue(new Amqp.Types.Symbol("apache.org:selector-filter:string"), filter));
            }

            string lastOffset = "-1";
            long lastSeqNumber = -1;
            DateTime lastEnqueueTime = DateTime.MinValue;

            ReceiverLink receiver = new ReceiverLink(
                session,
                "receiver-" + partition,
                new Source() { Address = partitionAddress, FilterSet = filters },
                null);
            // loop runs until clean up is called
            for (; g_bThreadActive; )
            {
                Message message = receiver.Receive(5000);
                if (message == null)
                {
                    continue;
                }

                receiver.Accept(message);
                Data data = (Data)message.BodySection;
                // Get the data such as offset, Seq number and Devicename from the response header
                lastOffset = (string)message.MessageAnnotations[new Amqp.Types.Symbol("x-opt-offset")];
                lastSeqNumber = (long)message.MessageAnnotations[new Amqp.Types.Symbol("x-opt-sequence-number")];
                lastEnqueueTime = (DateTime)message.MessageAnnotations[new Amqp.Types.Symbol("x-opt-enqueued-time")];
               
                var DeviceName = (string)message.MessageAnnotations[new Amqp.Types.Symbol("iothub-connection-device-id")];
                callback(data.Binary, DeviceName);
            }

            receiver.Close();
            session.Close();
            connection.Close();

        }
        // this function is core function that sends the message to the device
        private void SendCommand(string Message, ReceivedFeedBackData callback = null)
        {
            try
            {
                Debug.WriteLine("Send " + Message + " " + DEVICE_ID);
                long time = DateTimeToTimestamp(DateTime.UtcNow);
                lock (SendSync)
                {
                    address = new Address(HOST, PORT, null, null);
                    connection = new Connection(address);
                    session = new Session(connection);
                    string audience = Fx.Format("{0}/messages/devicebound", HOST);
                    string resourceUri = Fx.Format("{0}/messages/devicebound", HOST);
                    //We send the SAS token to the CBS node before sending the actual data
                    string sasToken = GetSharedAccessSignature(SHARED_ACCESS_KEY_NAME, SHARED_ACCESS_KEY, resourceUri, new TimeSpan(1, 0, 0));
                    bool cbs = PutCbsToken(connection, HOST, sasToken, audience);

                    if (cbs)
                    {
                        // nodes to send the data
                        string to = Fx.Format("/devices/{0}/messages/devicebound", DEVICE_ID);
                        string entity = "/messages/devicebound";

                        SenderLink senderLink = new SenderLink(session, "sender-link", entity);

                        var messageValue = Encoding.UTF8.GetBytes(Message);
                        Message message = new Message()
                        {
                            BodySection = new Data() { Binary = messageValue }
                        };
                        message.Properties = new Properties();
                        message.Properties.To = to;
                        message.Properties.MessageId = Guid.NewGuid().ToString();
                        message.ApplicationProperties = new ApplicationProperties();
                        message.ApplicationProperties["iothub-ack"] = "full";

                        senderLink.Send(message);
                        
                        
                        if (callback != null)
                        {
                            ReceiveFeedback(callback, "amqp.annotation.x-opt-enqueuedtimeutc > " + time);
                        }
                        senderLink.Close();
                        session.Close();
                        connection.Close();
                    }
                }
            }
            catch (Exception)
            {

                throw;
            }
        }
        // this function is used to receive the feed back for the message sent
        private void ReceiveFeedback(ReceivedFeedBackData callback,string filter)
        {
            string audience = Fx.Format("{0}/messages/servicebound/feedback", HOST);
            string resourceUri = Fx.Format("{0}/messages/servicebound/feedback", HOST);
            string sasToken = GetSharedAccessSignature(SHARED_ACCESS_KEY_NAME, SHARED_ACCESS_KEY, resourceUri, new TimeSpan(1, 0, 0));
            bool cbs = PutCbsToken(connection, HOST, sasToken, audience);
            if (cbs)
            {
                string entity = "/messages/servicebound/feedback";
                // Form the filter for receiving message based on the time stamp 
                Map filters = new Map();
                if (filter != null)
                {
                    filters.Add(new Amqp.Types.Symbol("apache.org:selector-filter:string"),
                        new DescribedValue(new Amqp.Types.Symbol("apache.org:selector-filter:string"), filter));
                }
                ReceiverLink receiveLink = new ReceiverLink(session, "receive-link", new Source() { Address = entity, FilterSet = filters },
                null);
                Message received = receiveLink.Receive();
                if (received != null)
                {
                    receiveLink.Accept(received);
                    Data data = (Data)received.BodySection;
                    callback(Encoding.UTF8.GetString(data.Binary));
                    Debug.WriteLine(Encoding.UTF8.GetString(data.Binary));
                }
                receiveLink.Close();
            }
           
        }
        // This function is used to send the SAS signature to the CBS node
        static private bool PutCbsToken(Connection connection, string host, string shareAccessSignature, string audience)
        {
            bool result = true;
            Session session = new Session(connection);

            string cbsReplyToAddress = "cbs-reply-to";
            var cbsSender = new SenderLink(session, "cbs-sender", "$cbs");
            var cbsReceiver = new ReceiverLink(session, cbsReplyToAddress, "$cbs");

            // construct the put-token message
            var request = new Message(shareAccessSignature);
            request.Properties = new Properties();
            request.Properties.MessageId = Guid.NewGuid().ToString();
            request.Properties.ReplyTo = cbsReplyToAddress;
            request.ApplicationProperties = new ApplicationProperties();
            request.ApplicationProperties["operation"] = "put-token";
            request.ApplicationProperties["type"] = "azure-devices.net:sastoken";
            request.ApplicationProperties["name"] = audience;
            cbsSender.Send(request);

            // receive the response
            var response = cbsReceiver.Receive();
            if (response == null || response.Properties == null || response.ApplicationProperties == null)
            {
                result = false;
            }
            else
            {
                int statusCode = (int)response.ApplicationProperties["status-code"];
                string statusCodeDescription = (string)response.ApplicationProperties["status-description"];
                if (statusCode != (int)202 && statusCode != (int)200) // !Accepted && !OK
                {
                    result = false;
                }
            }

            // the sender/receiver may be kept open for refreshing tokens
            cbsSender.Close();
            cbsReceiver.Close();
            session.Close();

            return result;
        }

        private static readonly long UtcReference = (new DateTime(1970, 1, 1, 0, 0, 0, 0)).Ticks;

        // this function is used to form the secured access signature
        static string GetSharedAccessSignature(string keyName, string sharedAccessKey, string resource, TimeSpan tokenTimeToLive)
        {
            // http://msdn.microsoft.com/en-us/library/azure/dn170477.aspx
            // the canonical Uri scheme is http because the token is not amqp specific
            // signature is computed from joined encoded request Uri string and expiry string

#if NETMF
            // needed in .Net Micro Framework to use standard RFC4648 Base64 encoding alphabet
            System.Convert.UseRFC4648Encoding = true;
#endif
            string expiry = ((long)(DateTime.UtcNow - new DateTime(UtcReference, DateTimeKind.Utc) + tokenTimeToLive).TotalSeconds()).ToString();
            string encodedUri = HttpUtility.UrlEncode(resource);

            byte[] hmac = SHA.computeHMAC_SHA256(Convert.FromBase64String(sharedAccessKey), Encoding.UTF8.GetBytes(encodedUri + "\n" + expiry));
            string sig = Convert.ToBase64String(hmac);

            if (keyName != null)
            {
                return Fx.Format(
                "SharedAccessSignature sr={0}&sig={1}&se={2}&skn={3}",
                encodedUri,
                HttpUtility.UrlEncode(sig),
                HttpUtility.UrlEncode(expiry),
                HttpUtility.UrlEncode(keyName));
            }
            else
            {
                return Fx.Format(
                    "SharedAccessSignature sr={0}&sig={1}&se={2}",
                    encodedUri,
                    HttpUtility.UrlEncode(sig),
                    HttpUtility.UrlEncode(expiry));
            }
        }
    }
}
