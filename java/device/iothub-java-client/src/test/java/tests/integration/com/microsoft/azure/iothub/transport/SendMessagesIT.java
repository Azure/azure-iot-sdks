// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

package tests.integration.com.microsoft.azure.iothub.transport;

import com.microsoft.azure.iot.service.exceptions.IotHubException;
import com.microsoft.azure.iot.service.sdk.Device;
import com.microsoft.azure.iot.service.sdk.RegistryManager;
import com.microsoft.azure.iothub.*;
import org.junit.After;
import org.junit.Assert;
import org.junit.Before;
import org.junit.Test;
import tests.integration.com.microsoft.azure.iothub.TestUtils.*;

import java.io.IOException;
import java.net.URISyntaxException;
import java.util.Map;
import java.util.UUID;
import java.util.concurrent.CompletableFuture;

public class SendMessagesIT
{
    private static String iotHubonnectionStringEnvVarName = "IOTHUB_CONNECTION_STRING";
    private static String iotHubConnectionString = "";
    private static RegistryManager registryManager;
    private Device deviceHttps;
    private Device deviceAmqps;
    private Device deviceMqtt;

    @Before
    public void setUp() throws Exception
    {
        Map<String, String> env = System.getenv();
        for (String envName : env.keySet())
        {
            if (envName.equals(iotHubonnectionStringEnvVarName.toString()))
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

    @After
    public void TearDown() throws IOException, IotHubException
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
                CompletableFuture<Boolean> messageSent = new CompletableFuture<>();
                EventCallback callback = new EventCallback();
                client.sendEventAsync(msg, callback, messageSent);
                Boolean result = messageSent.get();

                if (!result)
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
                CompletableFuture<Boolean> messageSent = new CompletableFuture<>();
                EventCallback callback = new EventCallback();
                client.sendEventAsync(msg, callback, messageSent);
                Boolean result = messageSent.get();

                if (!result)
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
                CompletableFuture<Boolean> messageSent = new CompletableFuture<>();
                EventCallback callback = new EventCallback();
                client.sendEventAsync(msg, callback, messageSent);
                Boolean result = messageSent.get();

                if (!result)
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

    public class EventCallback implements IotHubEventCallback
    {
        public void execute(IotHubStatusCode status, Object context)
        {
            CompletableFuture<Boolean> messageSent = (CompletableFuture<Boolean>)context;
            if (status.equals(IotHubStatusCode.OK_EMPTY))
            {
                messageSent.complete(true);
            }
            else
            {
                messageSent.complete(false);
            }
        }
    }
}
