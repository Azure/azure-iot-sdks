// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

package tests.integration.com.microsoft.azure.iothub.transport;

import com.microsoft.azure.iot.service.exceptions.IotHubException;
import com.microsoft.azure.iot.service.sdk.Device;
import com.microsoft.azure.iot.service.sdk.RegistryManager;
import com.microsoft.azure.iothub.*;
import org.junit.*;
import tests.integration.com.microsoft.azure.iothub.DeviceConnectionString;
import tests.integration.com.microsoft.azure.iothub.EventCallback;
import tests.integration.com.microsoft.azure.iothub.Success;

import java.io.IOException;
import java.net.URISyntaxException;
import java.util.Map;
import java.util.UUID;

public class SendMessagesIT
{
    private static String iotHubonnectionStringEnvVarName = "IOTHUB_CONNECTION_STRING";
    private static String iotHubConnectionString = "";
    private static RegistryManager registryManager;
    private static Device deviceHttps;
    private static Device deviceAmqps;
    private static Device deviceMqtt;

    // How much to wait until a message makes it to the server, in milliseconds
    private Integer sendTimeout = 60000;

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
        }

        registryManager = RegistryManager.createFromConnectionString(iotHubConnectionString);
        String uuid = UUID.randomUUID().toString();
        String deviceIdHttps = "java-device-client-e2e-test-https".concat("-" + uuid);
        String deviceIdAmqps = "java-device-client-e2e-test-amqps".concat("-" + uuid);
        String deviceIdMqtt = "java-device-client-e2e-test-mqtt".concat("-" + uuid);

        deviceHttps = Device.createFromId(deviceIdHttps, null, null);
        deviceAmqps = Device.createFromId(deviceIdAmqps, null, null);
        deviceMqtt = Device.createFromId(deviceIdMqtt, null, null);

        registryManager.addDevice(deviceHttps);
        registryManager.addDevice(deviceAmqps);
        registryManager.addDevice(deviceMqtt);
    }

    @AfterClass
    public static void TearDown() throws IOException, IotHubException
    {
        registryManager.removeDevice(deviceHttps.getDeviceId());
        registryManager.removeDevice(deviceAmqps.getDeviceId());
        registryManager.removeDevice(deviceMqtt.getDeviceId());
    }

    @Test
    public void SendMessagesOverHttps() throws URISyntaxException, IOException
    {
        String messageString = "Java client e2e test message over Https protocol";
        Message msg = new Message(messageString);
        DeviceClient client = new DeviceClient(DeviceConnectionString.get(iotHubConnectionString, deviceHttps), IotHubClientProtocol.HTTPS);
        client.open();

        for (int i = 0; i < 3; ++i)
        {
            try
            {
                Success messageSent = new Success();
                EventCallback callback = new EventCallback();

                client.sendEventAsync(msg, callback, messageSent);

                Integer waitDuration = 0;
                while(true)
                {
                    Thread.sleep(100);
                    waitDuration += 100;
                    if (messageSent.getResult() || waitDuration > sendTimeout)
                    {
                        break;
                    }
                }

                if (!messageSent.getResult())
                {
                    Assert.fail("Sending message over HTTPS protocol failed");
                }
            }
            catch (Exception e)
            {
                Assert.fail("Sending message over HTTPS protocol failed");
            }
        }

        client.close();
    }

    @Test
    public void SendMessagesOverAmqps() throws URISyntaxException, IOException, InterruptedException
    {
        String messageString = "Java client e2e test message over Amqps protocol";
        Message msg = new Message(messageString);
        DeviceClient client = new DeviceClient(DeviceConnectionString.get(iotHubConnectionString, deviceAmqps), IotHubClientProtocol.AMQPS);
        client.open();

        for (int i = 0; i < 3; ++i)
        {
            try
            {
                Success messageSent = new Success();
                EventCallback callback = new EventCallback();
                client.sendEventAsync(msg, callback, messageSent);

                Integer waitDuration = 0;
                while(true)
                {
                    Thread.sleep(100);
                    if (messageSent.getResult() || waitDuration > sendTimeout)
                    {
                        break;
                    }
                }

                if (!messageSent.getResult())
                {
                    Assert.fail("Sending message over AMQPS protocol failed");
                }
            }
            catch (Exception e)
            {
                Assert.fail("Sending message over AMQPS protocol failed");
            }
        }

        client.close();
    }

    @Test
    public void SendMessagesOverMqtt() throws URISyntaxException, IOException
    {
        String messageString = "Java client e2e test message over Mqtt protocol";
        Message msg = new Message(messageString);
        DeviceClient client = new DeviceClient(DeviceConnectionString.get(iotHubConnectionString, deviceMqtt), IotHubClientProtocol.MQTT);
        client.open();

        for (int i = 0; i < 3; ++i)
        {
            try
            {
                Success messageSent = new Success();
                EventCallback callback = new EventCallback();
                client.sendEventAsync(msg, callback, messageSent);

                Integer waitDuration = 0;
                while(true)
                {
                    Thread.sleep(100);
                    if (messageSent.getResult() || waitDuration > sendTimeout)
                    {
                        break;
                    }
                }

                if (!messageSent.getResult())
                {
                    Assert.fail("Sending message over MQTT protocol failed");
                }
            }
            catch (Exception e)
            {
                Assert.fail("Sending message over MQTT protocol failed");
            }
        }

        client.close();
    }
}