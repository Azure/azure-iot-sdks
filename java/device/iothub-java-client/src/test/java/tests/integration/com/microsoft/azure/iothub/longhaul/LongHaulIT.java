// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

package tests.integration.com.microsoft.azure.iothub.longhaul;

import com.microsoft.azure.iot.service.exceptions.IotHubException;
import com.microsoft.azure.iot.service.sdk.*;
import com.microsoft.azure.iothub.*;
import com.microsoft.azure.iothub.Message;
import org.junit.*;
import org.junit.experimental.ParallelComputer;
import org.junit.runner.JUnitCore;
import tests.integration.com.microsoft.azure.iothub.DeviceConnectionString;
import tests.integration.com.microsoft.azure.iothub.EventCallback;
import tests.integration.com.microsoft.azure.iothub.Success;

import java.io.IOException;
import java.lang.reflect.Field;
import java.net.URISyntaxException;
import java.util.HashMap;
import java.util.Map;
import java.util.UUID;

public class LongHaulIT
{
    @Test
    public void runLongHaulTests()
    {
        Class<?>[] classes = { LongHaulMessageTests.class };

        JUnitCore.runClasses(new ParallelComputer(true, true), classes);
    }

    public static class LongHaulMessageTests
    {
        private static String iotHubonnectionStringEnvVarName = "IOTHUB_CONNECTION_STRING";
        private static String iotHubConnectionString = "";

        private static String longHaulDurationEnvVarName = "IOTHUB_DEVICE_LONGHAUL_DURATION_SECONDS";
        private static Long longHaulDuration = 0L;

        private static RegistryManager registryManager;
        private static Device deviceSendHttps;
        private static Device deviceSendAmqps;
        private static Device deviceSendMqtt;
        private static Device deviceReceiveHttps;
        private static Device deviceReceiveAmqps;
        private static Device deviceReceiveMqtt;

        static Map<String, String> messageProperties = new HashMap<>(3);

        // How much to wait until receiving a message from the server, in milliseconds
        private Integer receiveTimeout = 60000;

        // How much to wait until a message makes it to the server until marking it failed, in milliseconds
        private Integer sendTimeout = 60000;

        // How much to wait before attempting to send a new message
        private Integer nextMessageWaitDuration = 1000;

        @BeforeClass
        public static void setUp() throws Exception
        {
            Map<String, String> env = System.getenv();
            for (String envName : env.keySet())
            {
                if (envName.equals(iotHubonnectionStringEnvVarName))
                {
                    iotHubConnectionString = env.get(envName);
                }

                if (envName.equals(longHaulDurationEnvVarName))
                {
                    try
                    {
                        longHaulDuration = Long.parseLong(env.get(envName));
                    } catch (Exception e)
                    {
                        //do nothing, the tests won't be executed
                    }
                }
            }

            Assume.assumeTrue(longHaulDuration > 0);

            registryManager = RegistryManager.createFromConnectionString(iotHubConnectionString);
            String uuid = UUID.randomUUID().toString();

            String deviceIdSendHttps = "java-device-client-longhaul-send-test-https".concat("-" + uuid);
            String deviceIdSendAmqps = "java-device-client-longhaul-send-test-amqps".concat("-" + uuid);
            String deviceIdSendMqtt = "java-device-client-longhaul-send-test-mqtt".concat("-" + uuid);
            String deviceIdReceiveHttps = "java-device-client-longhaul-receive-test-https".concat("-" + uuid);
            String deviceIdReceiveAmqps = "java-device-client-longhaul-receive-test-amqps".concat("-" + uuid);
            String deviceIdReceiveMqtt = "java-device-client-longhaul-receive-test-mqtt".concat("-" + uuid);

            deviceSendHttps = Device.createFromId(deviceIdSendHttps, null, null);
            deviceSendAmqps = Device.createFromId(deviceIdSendAmqps, null, null);
            deviceSendMqtt = Device.createFromId(deviceIdSendMqtt, null, null);
            deviceReceiveHttps = Device.createFromId(deviceIdReceiveHttps, null, null);
            deviceReceiveAmqps = Device.createFromId(deviceIdReceiveAmqps, null, null);
            deviceReceiveMqtt = Device.createFromId(deviceIdReceiveMqtt, null, null);

            registryManager.addDevice(deviceSendHttps);
            registryManager.addDevice(deviceSendAmqps);
            registryManager.addDevice(deviceSendMqtt);
            registryManager.addDevice(deviceReceiveHttps);
            registryManager.addDevice(deviceReceiveAmqps);
            registryManager.addDevice(deviceReceiveMqtt);

            messageProperties = new HashMap<>(3);
            messageProperties.put("name1", "value1");
            messageProperties.put("name2", "value2");
            messageProperties.put("name3", "value3");
        }

        @AfterClass
        public static void TearDown() throws IOException, IotHubException
        {
            if (deviceSendHttps != null)
            {
                registryManager.removeDevice(deviceSendHttps.getDeviceId());
            }

            if (deviceSendAmqps != null)
            {
                registryManager.removeDevice(deviceSendAmqps.getDeviceId());
            }

            if (deviceSendMqtt != null)
            {
                registryManager.removeDevice(deviceSendMqtt.getDeviceId());
            }

            if (deviceReceiveHttps != null)
            {
                registryManager.removeDevice(deviceReceiveHttps.getDeviceId());
            }

            if (deviceReceiveAmqps != null)
            {
                registryManager.removeDevice(deviceReceiveAmqps.getDeviceId());
            }

            if (deviceReceiveAmqps != null)
            {
                registryManager.removeDevice(deviceReceiveMqtt.getDeviceId());
            }
        }

        @Test
        public void SendMessagesOverHttps() throws URISyntaxException, IOException, InterruptedException
        {
            System.out.println("Started Https send long haul test...");

            String messageString = "Java client e2e test message over Https protocol";
            Message msg = new Message(messageString);
            DeviceClient client = new DeviceClient(DeviceConnectionString.get(iotHubConnectionString, deviceSendHttps), IotHubClientProtocol.HTTPS);
            client.open();

            int messagesSentSuccessfully = 0;
            int messagesFailed = 0;

            Long endTime = System.currentTimeMillis() + longHaulDuration * 1000;

            while (System.currentTimeMillis() < endTime)
            {
                Success messageSent = new Success();
                EventCallback callback = new EventCallback();

                client.sendEventAsync(msg, callback, messageSent);

                Integer waitDuration = 0;
                while (true)
                {
                    Thread.sleep(100);
                    waitDuration += 100;
                    if (messageSent.getResult() || waitDuration > sendTimeout)
                    {
                        break;
                    }
                }

                if (messageSent.getResult())
                {
                    messagesSentSuccessfully++;
                }
                else
                {
                    messagesFailed++;
                }

                Thread.sleep(nextMessageWaitDuration);
            }

            System.out.println("Https send long haul test - messages sent successfully: " + messagesSentSuccessfully);
            System.out.println("Https send long haul test - messages failed: " + messagesFailed);

            client.close();
        }

        @Test
        public void SendMessagesOverAmqps() throws URISyntaxException, IOException, InterruptedException
        {
            System.out.println("Started Amqps send long haul test...");

            String messageString = "Java client e2e test message over Amqps protocol";
            Message msg = new Message(messageString);
            DeviceClient client = new DeviceClient(DeviceConnectionString.get(iotHubConnectionString, deviceSendAmqps), IotHubClientProtocol.AMQPS);
            client.open();

            int messagesSentSuccessfully = 0;
            int messagesFailed = 0;

            Long endTime = System.currentTimeMillis() + longHaulDuration * 1000;

            while (System.currentTimeMillis() < endTime)
            {
                Success messageSent = new Success();
                EventCallback callback = new EventCallback();
                client.sendEventAsync(msg, callback, messageSent);

                Integer waitDuration = 0;
                while (true)
                {
                    Thread.sleep(100);
                    if (messageSent.getResult() || waitDuration > sendTimeout)
                    {
                        break;
                    }
                }

                if (messageSent.getResult())
                {
                    messagesSentSuccessfully++;
                }
                else
                {
                    messagesFailed++;
                }

                Thread.sleep(nextMessageWaitDuration);
            }

            System.out.println("Amqps send long haul test - messages sent successfully: " + messagesSentSuccessfully);
            System.out.println("Amqps send long haul test - messages failed: " + messagesFailed);

            client.close();
        }

        @Test
        public void SendMessagesOverMqtt() throws URISyntaxException, IOException, InterruptedException
        {
            System.out.println("Started Mqtt send long haul test...");

            String messageString = "Java client e2e test message over Mqtt protocol";
            Message msg = new Message(messageString);
            DeviceClient client = new DeviceClient(DeviceConnectionString.get(iotHubConnectionString, deviceSendMqtt), IotHubClientProtocol.MQTT);
            client.open();

            int messagesSentSuccessfully = 0;
            int messagesFailed = 0;

            Long endTime = System.currentTimeMillis() + longHaulDuration * 1000;

            while (System.currentTimeMillis() < endTime)
            {
                Success messageSent = new Success();
                EventCallback callback = new EventCallback();
                client.sendEventAsync(msg, callback, messageSent);

                Integer waitDuration = 0;
                while (true)
                {
                    Thread.sleep(100);
                    if (messageSent.getResult() || waitDuration > sendTimeout)
                    {
                        break;
                    }
                }

                if (messageSent.getResult())
                {
                    messagesSentSuccessfully++;
                }
                else
                {
                    messagesFailed++;
                }

                Thread.sleep(nextMessageWaitDuration);
            }

            System.out.println("Mqtt send long haul test - messages sent successfully: " + messagesSentSuccessfully);
            System.out.println("Mqtt send long haul test - messages failed: " + messagesFailed);

            client.close();
        }

        @Test
        public void ReceiveMessagesOverHttpsIncludingProperties() throws Exception
        {
            System.out.println("Started Https receive long haul test...");

            ServiceClient serviceClient = ServiceClient.createFromConnectionString(iotHubConnectionString, IotHubServiceClientProtocol.AMQPS);
            serviceClient.open();

            DeviceClient client = new DeviceClient(DeviceConnectionString.get(iotHubConnectionString, deviceReceiveHttps), IotHubClientProtocol.HTTPS);
            Field receivePeriod = client.getClass().getDeclaredField("RECEIVE_PERIOD_MILLIS");
            receivePeriod.setAccessible(true);
            receivePeriod.set(client, 1000);
            client.open();

            int messagesReceivedSuccessfully = 0;
            int messagesFailed = 0;

            Long endTime = System.currentTimeMillis() + longHaulDuration * 1000;

            while (System.currentTimeMillis() < endTime)
            {
                Success messageReceived = new Success();
                com.microsoft.azure.iothub.MessageCallback callback = new MessageCallback();
                client.setMessageCallback(callback, messageReceived);

                String messageString = "Java service e2e test message to be received over Https protocol";
                com.microsoft.azure.iot.service.sdk.Message serviceMessage = new com.microsoft.azure.iot.service.sdk.Message(messageString);
                serviceMessage.setProperties(messageProperties);
                String deviceId = deviceReceiveHttps.getDeviceId();
                serviceClient.send(deviceId, serviceMessage);

                Integer waitDuration = 0;
                while (true)
                {
                    waitDuration += 100;
                    if (messageReceived.getResult() || waitDuration > receiveTimeout)
                    {
                        break;
                    }
                    Thread.sleep(100);
                }

                if (messageReceived.getResult())
                {
                    messagesReceivedSuccessfully++;
                }
                else
                {
                    messagesFailed++;
                }

                Thread.sleep(nextMessageWaitDuration);
            }

            System.out.println("Https receive long haul test - messages received successfully: " + messagesReceivedSuccessfully);
            System.out.println("Https receive long haul test - messages failed: " + messagesFailed);

            Thread.sleep(200);
            client.close();
            serviceClient.close();
        }

        @Test
        public void ReceiveMessagesOverAmqpsIncludingProperties() throws Exception
        {
            System.out.println("Started Amqps receive long haul test...");

            ServiceClient serviceClient = ServiceClient.createFromConnectionString(iotHubConnectionString, IotHubServiceClientProtocol.AMQPS);
            serviceClient.open();

            DeviceClient client = new DeviceClient(DeviceConnectionString.get(iotHubConnectionString, deviceReceiveAmqps), IotHubClientProtocol.AMQPS);
            client.open();

            int messagesReceivedSuccessfully = 0;
            int messagesFailed = 0;

            Long endTime = System.currentTimeMillis() + longHaulDuration * 1000;

            while (System.currentTimeMillis() < endTime)
            {
                Success messageReceived = new Success();
                com.microsoft.azure.iothub.MessageCallback callback = new MessageCallback();
                client.setMessageCallback(callback, messageReceived);

                String messageString = "Java service e2e test message to be received over Amqps protocol";
                com.microsoft.azure.iot.service.sdk.Message serviceMessage = new com.microsoft.azure.iot.service.sdk.Message(messageString);
                serviceMessage.setProperties(messageProperties);
                String deviceId = deviceReceiveAmqps.getDeviceId();
                serviceClient.send(deviceId, serviceMessage);

                Integer waitDuration = 0;
                while (true)
                {
                    waitDuration += 100;
                    if (messageReceived.getResult() || waitDuration > receiveTimeout)
                    {
                        break;
                    }
                    Thread.sleep(100);
                }

                if (messageReceived.getResult())
                {
                    messagesReceivedSuccessfully++;
                }
                else
                {
                    messagesFailed++;
                }

                Thread.sleep(nextMessageWaitDuration);
            }

            System.out.println("Amqps receive long haul test - messages received successfully: " + messagesReceivedSuccessfully);
            System.out.println("Amqps receive long haul test - messages failed: " + messagesFailed);

            Thread.sleep(200);
            client.close();
            serviceClient.close();
        }

        @Test
        public void ReceiveMessagesOverMqtt() throws Exception
        {
            System.out.println("Started Mqtt receive long haul test...");

            ServiceClient serviceClient = ServiceClient.createFromConnectionString(iotHubConnectionString, IotHubServiceClientProtocol.AMQPS);
            serviceClient.open();

            DeviceClient client = new DeviceClient(DeviceConnectionString.get(iotHubConnectionString, deviceReceiveMqtt), IotHubClientProtocol.MQTT);
            client.open();

            int messagesReceivedSuccessfully = 0;
            int messagesFailed = 0;

            Long endTime = System.currentTimeMillis() + longHaulDuration * 1000;

            while (System.currentTimeMillis() < endTime)
            {
                Success messageReceived = new Success();
                com.microsoft.azure.iothub.MessageCallback callback = new MessageCallbackMqtt();
                client.setMessageCallback(callback, messageReceived);

                String messageString = "Java service e2e test message to be received over Mqtt protocol";
                com.microsoft.azure.iot.service.sdk.Message serviceMessage = new com.microsoft.azure.iot.service.sdk.Message(messageString);
                String deviceId = deviceReceiveMqtt.getDeviceId();
                serviceClient.send(deviceId, serviceMessage);

                Integer waitDuration = 0;
                while (true)
                {
                    waitDuration += 100;
                    if (messageReceived.getResult() || waitDuration > receiveTimeout)
                    {
                        break;
                    }
                    Thread.sleep(100);
                }

                if (messageReceived.getResult())
                {
                    messagesReceivedSuccessfully++;
                }
                else
                {
                    messagesFailed++;
                }

                Thread.sleep(nextMessageWaitDuration);
            }

            System.out.println("Mqtt receive long haul test - messages received successfully: " + messagesReceivedSuccessfully);
            System.out.println("Mqtt receive long haul test - messages failed: " + messagesFailed);

            Thread.sleep(200);
            client.close();
            serviceClient.close();
        }

        protected static class MessageCallback implements com.microsoft.azure.iothub.MessageCallback
        {
            public IotHubMessageResult execute(Message msg, Object context)
            {
                Boolean resultValue = true;
                HashMap<String, String> messageProperties = (HashMap<String, String>) LongHaulMessageTests.messageProperties;
                Success messageReceived = (Success) context;
                MessageProperty[] messagePropertiesFromService = msg.getProperties();
                if (messageProperties.size() != messagePropertiesFromService.length)
                {
                    resultValue = false;
                } else
                {
                    for (int i = 0; i < 3; i++)
                    {
                        if (!messageProperties.containsKey(messagePropertiesFromService[i].getName())
                                || !messageProperties.containsValue(messagePropertiesFromService[i].getValue()))
                        {
                            resultValue = false;
                            break;
                        }
                    }
                }
                messageReceived.setResult(resultValue);
                return IotHubMessageResult.COMPLETE;
            }
        }
    }

    private static class MessageCallbackMqtt implements com.microsoft.azure.iothub.MessageCallback
    {
        public IotHubMessageResult execute(Message msg, Object context)
        {
            Success messageReceived = (Success)context;
            messageReceived.setResult(true);
            return IotHubMessageResult.COMPLETE;
        }
    }
}
