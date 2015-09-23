// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

package tests.unit.com.microsoft.azure.iothub;

import com.microsoft.azure.iothub.IotHubClient;
import com.microsoft.azure.iothub.IotHubClientConfig;
import com.microsoft.azure.iothub.IotHubClientProtocol;
import com.microsoft.azure.iothub.IotHubServiceboundMessage;
import com.microsoft.azure.iothub.IotHubMessageCallback;
import com.microsoft.azure.iothub.IotHubEventCallback;
import com.microsoft.azure.iothub.transport.amqps.AmqpsTransport;
import com.microsoft.azure.iothub.transport.https.HttpsTransport;

import com.microsoft.azure.iothub.transport.IotHubReceiveTask;
import com.microsoft.azure.iothub.transport.IotHubSendTask;
import mockit.Mocked;
import mockit.NonStrictExpectations;
import mockit.StrictExpectations;
import mockit.Verifications;
import org.junit.Assert;
import org.junit.Test;

import java.io.IOException;
import java.net.URISyntaxException;
import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;

/** Unit tests for IotHubClient. */
public class IotHubClientTest
{
    @Mocked
    IotHubClientConfig mockConfig;

    @Mocked
    Executors mockExecutor;

    // Tests_SRS_IOTHUBCLIENT_11_042: [The constructor shall interpret the connection string as a set of key-value pairs delimited by ';', with keys and values separated by '='.]
    // Tests_SRS_IOTHUBCLIENT_11_043: [The constructor shall save the IoT Hub hostname as the value of 'HostName' in the connection string.]
    @Test
    public void connStringConstructorSavesHostname(
            @Mocked final AmqpsTransport mockTransport)
            throws URISyntaxException
    {
        final String connString =
                "HostName=iothub.device.com;CredentialType=SharedAccessKey;CredentialScope=Device;DeviceId=testdevice;SharedAccessKey=adjkl234j52=;";
        final IotHubClientProtocol protocol = IotHubClientProtocol.AMQPS;

        new IotHubClient(connString, protocol);

        final String expectedHostname = "iothub.device.com";
        new Verifications()
        {
            {
                new IotHubClientConfig(expectedHostname, anyString, anyString);
            }
        };
    }

    // Tests_SRS_IOTHUBCLIENT_11_044: [The constructor shall save the device ID as the UTF-8 URL-decoded value of 'DeviceId' in the connection string.]
    @Test
    public void connStringConstructorSavesDeviceId(
            @Mocked final AmqpsTransport mockTransport)
            throws URISyntaxException
    {
        final String connString =
                "HostName=iothub.device.com;CredentialType=SharedAccessKey;CredentialScope=Device;DeviceId=test%3Bdevice;SharedAccessKey=adjkl234j52=;";
        final IotHubClientProtocol protocol = IotHubClientProtocol.AMQPS;

        new IotHubClient(connString, protocol);

        final String expectedDeviceId = "test;device";
        new Verifications()
        {
            {
                new IotHubClientConfig(anyString, expectedDeviceId, anyString);
            }
        };
    }

    // Tests_SRS_IOTHUBCLIENT_11_045: [The constructor shall save the device key as the value of 'SharedAccessKey' in the connection string.]
    @Test
    public void connStringConstructorSavesSharedAccessKey(
            @Mocked final AmqpsTransport mockTransport)
            throws URISyntaxException
    {
        final String connString =
                "HostName=iothub.device.com;CredentialType=SharedAccessKey;CredentialScope=Device;DeviceId=testdevice;SharedAccessKey=adjkl234j52=;";
        final IotHubClientProtocol protocol = IotHubClientProtocol.AMQPS;

        new IotHubClient(connString, protocol);

        final String expectedSharedAccessKey = "adjkl234j52=";
        new Verifications()
        {
            {
                new IotHubClientConfig(anyString, anyString,
                        expectedSharedAccessKey);
            }
        };
    }

    // Tests_SRS_IOTHUBCLIENT_11_046: [The constructor shall initialize the IoT Hub transport that uses the protocol specified.]
    @Test
    public void connStringConstructorInitializesHttpsTransport(
            @Mocked final HttpsTransport mockTransport)
            throws URISyntaxException
    {
        final String connString =
                "HostName=iothub.device.com;CredentialType=SharedAccessKey;CredentialScope=Device;DeviceId=testdevice;SharedAccessKey=adjkl234j52=;";
        final IotHubClientProtocol protocol = IotHubClientProtocol.HTTPS;

        new IotHubClient(connString, protocol);

        new Verifications()
        {
            {
                new HttpsTransport((IotHubClientConfig) any);
            }
        };
    }

    // Tests_SRS_IOTHUBCLIENT_11_046: [The constructor shall initialize the IoT Hub transport that uses the protocol specified.]
    @Test
    public void connStringConstructorInitializesAmqpsTransport(
            @Mocked final AmqpsTransport mockTransport)
            throws URISyntaxException
    {
        final String connString =
                "HostName=iothub.device.com;CredentialType=SharedAccessKey;CredentialScope=Device;DeviceId=testdevice;SharedAccessKey=adjkl234j52=;";
        final IotHubClientProtocol protocol = IotHubClientProtocol.AMQPS;

        new IotHubClient(connString, protocol);

        new Verifications()
        {
            {
                new AmqpsTransport((IotHubClientConfig) any);
            }
        };
    }

    // Tests_SRS_IOTHUBCLIENT_11_047: [If the connection string is null, the function shall throw an IllegalArgumentException.]
    @Test(expected = IllegalArgumentException.class)
    public void connStringConstructorFailsForNullConnString() throws URISyntaxException
    {
        new IotHubClient(null, IotHubClientProtocol.AMQPS);
    }

    // Tests_SRS_IOTHUBCLIENT_11_048: [If no value for 'HostName' is found in the connection string, the function shall throw an IllegalArgumentException.]
    @Test(expected = IllegalArgumentException.class)
    public void connStringConstructorFailsIfConnStringMissingHostname(
            @Mocked final AmqpsTransport mockTransport)
            throws URISyntaxException
    {
        final String connString =
                "CredentialType=SharedAccessKey;CredentialScope=Device;DeviceId=testdevice;SharedAccessKey=adjkl234j52=;";
        final IotHubClientProtocol protocol = IotHubClientProtocol.AMQPS;

        new IotHubClient(connString, protocol);
    }

    // Tests_SRS_IOTHUBCLIENT_11_049: [If no value for 'DeviceId' is found in the connection string, the function shall throw an IllegalArgumentException.]
    @Test(expected = IllegalArgumentException.class)
    public void connStringConstructorFailsIfConnStringMissingDeviceId(
            @Mocked final AmqpsTransport mockTransport)
            throws URISyntaxException
    {
        final String connString =
                "HostName=iothub.device.com;CredentialType=SharedAccessKey;CredentialScope=Device;SharedAccessKey=adjkl234j52=;";
        final IotHubClientProtocol protocol = IotHubClientProtocol.AMQPS;

        new IotHubClient(connString, protocol);
    }

    // Tests_SRS_IOTHUBCLIENT_11_050: [If no argument for 'SharedAccessKey' is found in the connection string, the function shall throw an IllegalArgumentException.]
    @Test(expected = IllegalArgumentException.class)
    public void connStringConstructorFailsIfConnStringMissingSharedAccessKey(
            @Mocked final AmqpsTransport mockTransport)
            throws URISyntaxException
    {
        final String connString =
                "HostName=iothub.device.com;CredentialType=SharedAccessKey;CredentialScope=Device;DeviceId=testdevice;";
        final IotHubClientProtocol protocol = IotHubClientProtocol.AMQPS;

        new IotHubClient(connString, protocol);
    }

    // Tests_SRS_IOTHUBCLIENT_11_051: [If protocol is null, the function shall throw an IllegalArgumentException.]
    @Test(expected = IllegalArgumentException.class)
    public void connStringConstructorFailsIfNullProtocol()
            throws URISyntaxException
    {
        final String connString =
                "HostName=iothub.device.com;CredentialType=SharedAccessKey;CredentialScope=Device;DeviceId=testdevice;SharedAccessKey=adjkl234j52=;";

        new IotHubClient(connString, null);
    }

    // Tests_SRS_IOTHUBCLIENT_11_052: [The constructor shall save the IoT Hub hostname, device ID, and device key as configuration parameters.]
    @Test
    public void constructorSavesConfigParams(
            @Mocked final AmqpsTransport mockTransport)
            throws URISyntaxException
    {
        final String iotHubHostname = "test-iothubhostname";
        final String deviceId = "test-deviceid";
        final String deviceKey = "test-devicekey";
        final IotHubClientProtocol protocol = IotHubClientProtocol.AMQPS;

        new IotHubClient(iotHubHostname, deviceId, deviceKey, protocol);

        new Verifications()
        {
            {
                new IotHubClientConfig(iotHubHostname, deviceId,
                        deviceKey);
            }
        };
    }

    // Tests_SRS_IOTHUBCLIENT_11_004: [The constructor shall initialize the IoT Hub transport that uses the protocol specified.]
    @Test
    public void constructorInitializesHttpsTransport(
            @Mocked final HttpsTransport mockTransport)
            throws URISyntaxException
    {
        final String iotHubHostname = "test-iothubhostname";
        final String deviceId = "test-deviceid";
        final String deviceKey = "test-devicekey";
        final IotHubClientProtocol protocol = IotHubClientProtocol.HTTPS;

        new IotHubClient(iotHubHostname, deviceId, deviceKey, protocol);

        new Verifications()
        {
            {
                new HttpsTransport((IotHubClientConfig) any);
            }
        };
    }

    // Tests_SRS_IOTHUBCLIENT_11_004: [The constructor shall initialize the IoT Hub transport that uses the protocol specified.]
    @Test
    public void constructorInitializesAmqpsTransport(
            @Mocked final AmqpsTransport mockTransport)
            throws URISyntaxException
    {
        final String iotHubHostname = "test-iothubhostname";
        final String deviceId = "test-deviceid";
        final String deviceKey = "test-devicekey";
        final IotHubClientProtocol protocol = IotHubClientProtocol.AMQPS;

        new IotHubClient(iotHubHostname, deviceId, deviceKey, protocol);

        new Verifications()
        {
            {
                new AmqpsTransport((IotHubClientConfig) any);
            }
        };
    }

    // Tests_SRS_IOTHUBCLIENT_11_025: [If iotHubHostname is null, the constructor shall throw an IllegalArgumentException.]
    @Test(expected = IllegalArgumentException.class)
    public void constructorRejectsNullIotHubHostname(
            @Mocked final AmqpsTransport mockTransport)
            throws URISyntaxException
    {
        final String iotHubHostname = null;
        final String deviceId = "test-deviceid";
        final String deviceKey = "test-devicekey";
        final IotHubClientProtocol protocol = IotHubClientProtocol.AMQPS;

        new IotHubClient(iotHubHostname, deviceId, deviceKey, protocol);
    }

    // Tests_SRS_IOTHUBCLIENT_11_026: [If deviceId is null, the constructor shall throw an IllegalArgumentException.]
    @Test(expected = IllegalArgumentException.class)
    public void constructorRejectsNullDeviceId(
            @Mocked final AmqpsTransport mockTransport)
            throws URISyntaxException
    {
        final String iotHubHostname = "test-iothubhostname";
        final String deviceId = null;
        final String deviceKey = "test-devicekey";
        final IotHubClientProtocol protocol = IotHubClientProtocol.AMQPS;

        new IotHubClient(iotHubHostname, deviceId, deviceKey, protocol);
    }

    // Tests_SRS_IOTHUBCLIENT_11_027: [If deviceKey is null, the constructor shall throw an IllegalArgumentException.]
    @Test(expected = IllegalArgumentException.class)
    public void constructorRejectsNullDeviceKey(
            @Mocked final AmqpsTransport mockTransport)
            throws URISyntaxException
    {
        final String iotHubHostname = "test-iothubhostname";
        final String deviceId = "test-deviceid";
        final String deviceKey = null;
        final IotHubClientProtocol protocol = IotHubClientProtocol.AMQPS;

        new IotHubClient(iotHubHostname, deviceId, deviceKey, protocol);
    }

    // Tests_SRS_IOTHUBCLIENT_11_034: [If protocol is null, the constructor shall throw an IllegalArgumentException.]
    @Test(expected = IllegalArgumentException.class)
    public void constructorRejectsNullProtocol(
            @Mocked final AmqpsTransport mockTransport)
            throws URISyntaxException
    {
        final String iotHubHostname = "test-iothub-name";
        final String deviceId = "test-deviceid";
        final String deviceKey = "test-devicekey";
        final IotHubClientProtocol protocol = null;

        new IotHubClient(iotHubHostname, deviceId, deviceKey, protocol);
    }

    // Tests_SRS_IOTHUBCLIENT_11_035: [The function shall open the transport to communicate with an IoT Hub.]
    @Test
    public void openOpensTransport(
            @Mocked final ScheduledExecutorService mockScheduler,
            @Mocked final AmqpsTransport mockTransport,
            @Mocked final IotHubServiceboundMessage mockMsg,
            @Mocked final IotHubSendTask mockTask)
            throws IOException, URISyntaxException
    {
        final String iotHubHostname = "test-iothubhostname";
        final String deviceId = "test-deviceid";
        final String deviceKey = "test-devicekey";
        final IotHubClientProtocol protocol = IotHubClientProtocol.AMQPS;

        IotHubClient client =
                new IotHubClient(iotHubHostname, deviceId, deviceKey, protocol);
        client.open();

        final AmqpsTransport expectedTransport = mockTransport;
        new Verifications()
        {
            {
                expectedTransport.open();
            }
        };
    }

    // Tests_SRS_IOTHUBCLIENT_11_023: [The function shall schedule send tasks to run every 5000 milliseconds.]
    @Test
    public void openStartsSendTask(
            @Mocked final ScheduledExecutorService mockScheduler,
            @Mocked final AmqpsTransport mockTransport,
            @Mocked final IotHubServiceboundMessage mockMsg,
            @Mocked final IotHubSendTask mockTask)
            throws IOException, URISyntaxException
    {
        final String iotHubHostname = "test-iothubhostname";
        final String deviceId = "test-deviceid";
        final String deviceKey = "test-devicekey";
        final IotHubClientProtocol protocol = IotHubClientProtocol.AMQPS;

        IotHubClient client =
                new IotHubClient(iotHubHostname, deviceId, deviceKey, protocol);
        client.open();

        final long expectedSendPeriodMillis = 5000l;
        new Verifications()
        {
            {
                mockScheduler.scheduleAtFixedRate((IotHubSendTask) any,
                        anyLong, expectedSendPeriodMillis,
                        TimeUnit.MILLISECONDS);
            }
        };
    }

    // Tests_SRS_IOTHUBCLIENT_11_024: [The function shall schedule receive tasks to run every 5000 milliseconds.]
    @Test
    public void openStartsReceiveTask(
            @Mocked final ScheduledExecutorService mockScheduler,
            @Mocked final AmqpsTransport mockTransport,
            @Mocked final IotHubServiceboundMessage mockMsg,
            @Mocked final IotHubSendTask mockTask)
            throws IOException, URISyntaxException
    {
        final String iotHubHostname = "test-iothubhostname";
        final String deviceId = "test-deviceid";
        final String deviceKey = "test-devicekey";
        final IotHubClientProtocol protocol = IotHubClientProtocol.AMQPS;

        IotHubClient client =
                new IotHubClient(iotHubHostname, deviceId, deviceKey, protocol);
        client.open();

        final long expectedReceivePeriodMillis = 5000l;
        new Verifications()
        {
            {
                mockScheduler.scheduleAtFixedRate((IotHubReceiveTask) any,
                        anyLong, expectedReceivePeriodMillis,
                        TimeUnit.MILLISECONDS);
            }
        };
    }

    // Tests_SRS_IOTHUBCLIENT_11_036: [If an error occurs in opening the transport, the function shall throw an IOException.]
    @Test(expected = IOException.class)
    public void openThrowsIOExceptionIfTransportOpenFails(
            @Mocked final ScheduledExecutorService mockScheduler,
            @Mocked final AmqpsTransport mockTransport,
            @Mocked final IotHubServiceboundMessage mockMsg,
            @Mocked final IotHubSendTask mockTask)
            throws IOException, URISyntaxException
    {
        final String iotHubHostname = "test-iothubhostname";
        final String deviceId = "test-deviceid";
        final String deviceKey = "test-devicekey";
        final IotHubClientProtocol protocol = IotHubClientProtocol.AMQPS;
        new NonStrictExpectations()
        {
            {
                mockTransport.open();
                result = new IOException();
            }
        };

        IotHubClient client =
                new IotHubClient(iotHubHostname, deviceId, deviceKey, protocol);
        client.open();
    }

    // Tests_SRS_IOTHUBCLIENT_11_028: [If the client is already open, the function shall do nothing.]
    @Test
    public void openDoesNothingIfCalledTwice(
            @Mocked final ScheduledExecutorService mockScheduler,
            @Mocked final AmqpsTransport mockTransport,
            @Mocked final IotHubServiceboundMessage mockMsg,
            @Mocked final IotHubSendTask mockTask)
            throws IOException, URISyntaxException
    {
        final String iotHubHostname = "test-iothubhostname";
        final String deviceId = "test-deviceid";
        final String deviceKey = "test-devicekey";
        final IotHubClientProtocol protocol = IotHubClientProtocol.AMQPS;

        IotHubClient client =
                new IotHubClient(iotHubHostname, deviceId, deviceKey, protocol);
        client.open();
        client.open();

        final AmqpsTransport expectedTransport = mockTransport;
        new Verifications()
        {
            {
                expectedTransport.open();
                times = 1;
            }
        };
    }

    // Tests_SRS_IOTHUBCLIENT_11_010: [The function shall finish all ongoing tasks.]
    // Tests_SRS_IOTHUBCLIENT_11_011: [The function shall cancel all recurring tasks.]
    @Test
    public void closeWaitsForTasksToFinish(
            @Mocked final ScheduledExecutorService mockScheduler,
            @Mocked final HttpsTransport mockTransport,
            @Mocked final IotHubSendTask mockSendTask,
            @Mocked final IotHubReceiveTask mockReceiveTask)
            throws IOException, URISyntaxException
    {
        final String iotHubHostname = "test-iothubhostname";
        final String deviceId = "test-deviceid";
        final String deviceKey = "test-devicekey";
        final IotHubClientProtocol protocol = IotHubClientProtocol.HTTPS;
        new NonStrictExpectations()
        {
            {
                mockTransport.isEmpty();
                returns(false, false, true);
            }
        };

        IotHubClient client =
                new IotHubClient(iotHubHostname, deviceId, deviceKey, protocol);
        client.open();
        client.close();

        new Verifications()
        {
            {
                mockTransport.isEmpty();
                times = 3;
                mockScheduler.shutdown();
            }
        };
    }

    // Tests_SRS_IOTHUBCLIENT_11_037: [The function shall close the transport.]
    @Test
    public void closeClosesTransport(
            @Mocked final ScheduledExecutorService mockScheduler,
            @Mocked final HttpsTransport mockTransport,
            @Mocked final IotHubSendTask mockSendTask,
            @Mocked final IotHubReceiveTask mockReceiveTask)
            throws IOException, URISyntaxException
    {
        final String iotHubHostname = "test-iothubhostname";
        final String deviceId = "test-deviceid";
        final String deviceKey = "test-devicekey";
        final IotHubClientProtocol protocol = IotHubClientProtocol.HTTPS;
        new NonStrictExpectations()
        {
            {
                mockTransport.isEmpty();
                result = true;
            }
        };

        IotHubClient client =
                new IotHubClient(iotHubHostname, deviceId, deviceKey, protocol);
        client.open();
        client.close();

        final HttpsTransport expectedTransport = mockTransport;
        new Verifications()
        {
            {
                expectedTransport.close();
            }
        };
    }

    // Tests_SRS_IOTHUBCLIENT_11_031: [If the client is already closed, the function shall do nothing.]
    @Test
    public void closeDoesNothingOnUnopenedClient(
            @Mocked final ScheduledExecutorService mockScheduler,
            @Mocked final AmqpsTransport mockTransport,
            @Mocked final IotHubSendTask mockSendTask,
            @Mocked final IotHubReceiveTask mockReceiveTask)
            throws IOException, URISyntaxException
    {
        final String iotHubHostname = "test-iothubhostname";
        final String deviceId = "test-deviceid";
        final String deviceKey = "test-devicekey";
        final IotHubClientProtocol protocol = IotHubClientProtocol.AMQPS;
        new NonStrictExpectations()
        {
            {
                mockTransport.isEmpty();
                result = true;
            }
        };

        IotHubClient client =
                new IotHubClient(iotHubHostname, deviceId, deviceKey, protocol);
        client.close();

        final AmqpsTransport expectedTransport = mockTransport;
        new Verifications()
        {
            {
                expectedTransport.close();
                times = 0;
            }
        };
    }

    // Tests_SRS_IOTHUBCLIENT_11_031: [If the client is already closed, the function shall do nothing.]
    @Test
    public void closeDoesNothingOnClosedClient(
            @Mocked final ScheduledExecutorService mockScheduler,
            @Mocked final AmqpsTransport mockTransport,
            @Mocked final IotHubSendTask mockSendTask,
            @Mocked final IotHubReceiveTask mockReceiveTask)
            throws IOException, URISyntaxException
    {
        final String iotHubHostname = "test-iothubhostname";
        final String deviceId = "test-deviceid";
        final String deviceKey = "test-devicekey";
        final IotHubClientProtocol protocol = IotHubClientProtocol.AMQPS;
        new NonStrictExpectations()
        {
            {
                mockTransport.isEmpty();
                result = true;
            }
        };

        IotHubClient client =
                new IotHubClient(iotHubHostname, deviceId, deviceKey, protocol);
        client.open();
        client.close();
        client.close();

        final AmqpsTransport expectedTransport = mockTransport;
        new Verifications()
        {
            {
                expectedTransport.close();
                times = 1;
            }
        };
    }

    // Tests_SRS_IOTHUBCLIENT_11_006: [The function shall add the message, with its associated callback and callback context, to the transport.]
    @Test
    public void sendEventAsyncAddsMessageToTransport(
            @Mocked final ScheduledExecutorService mockScheduler,
            @Mocked final AmqpsTransport mockTransport,
            @Mocked final IotHubServiceboundMessage mockMsg,
            @Mocked final IotHubEventCallback mockCallback)
            throws IOException, URISyntaxException
    {
        final String iotHubHostname = "test-iothubhostname";
        final String deviceId = "test-deviceid";
        final String deviceKey = "test-devicekey";
        final IotHubClientProtocol protocol = IotHubClientProtocol.AMQPS;
        final Map<String, Object> context = new HashMap<>();

        IotHubClient client =
                new IotHubClient(iotHubHostname, deviceId, deviceKey, protocol);
        client.open();
        client.sendEventAsync(mockMsg, mockCallback, context);

        new Verifications()
        {
            {
                mockTransport.addMessage(mockMsg, mockCallback, context);
            }
        };
    }

    // Tests_SRS_IOTHUBCLIENT_11_033: [If the message given is null, the function shall throw an IllegalArgumentException.]
    @Test(expected = IllegalArgumentException.class)
    public void sendEventAsyncRejectsNullMessage(
            @Mocked final ScheduledExecutorService mockScheduler,
            @Mocked final AmqpsTransport mockTransport,
            @Mocked final IotHubEventCallback mockCallback)
            throws IOException, URISyntaxException
    {
        final String iotHubHostname = "test-iothubhostname";
        final String deviceId = "test-deviceid";
        final String deviceKey = "test-devicekey";
        final IotHubClientProtocol protocol = IotHubClientProtocol.AMQPS;
        final Map<String, Object> context = new HashMap<>();

        IotHubClient client =
                new IotHubClient(iotHubHostname, deviceId, deviceKey, protocol);
        client.open();
        client.sendEventAsync(null, mockCallback, context);
    }

    // Tests_SRS_IOTHUBCLIENT_11_039: [If the client is closed, the function shall throw an IllegalStateException.]
    @Test(expected = IllegalStateException.class)
    public void sendEventAsyncFailsIfClientNotOpened(
            @Mocked final ScheduledExecutorService mockScheduler,
            @Mocked final AmqpsTransport mockTransport,
            @Mocked final IotHubServiceboundMessage mockMsg,
            @Mocked final IotHubEventCallback mockCallback)
            throws IOException, URISyntaxException
    {
        final String iotHubHostname = "test-iothubhostname";
        final String deviceId = "test-deviceid";
        final String deviceKey = "test-devicekey";
        final IotHubClientProtocol protocol = IotHubClientProtocol.AMQPS;
        final Map<String, Object> context = new HashMap<>();

        IotHubClient client =
                new IotHubClient(iotHubHostname, deviceId, deviceKey, protocol);
        client.sendEventAsync(mockMsg, mockCallback, context);
    }

    // Tests_SRS_IOTHUBCLIENT_11_039: [If the client is closed, the function shall throw an IllegalStateException.]
    @Test(expected = IllegalStateException.class)
    public void sendEventAsyncFailsIfClientAlreadyClosed(
            @Mocked final ScheduledExecutorService mockScheduler,
            @Mocked final AmqpsTransport mockTransport,
            @Mocked final IotHubServiceboundMessage mockMsg,
            @Mocked final IotHubEventCallback mockCallback)
            throws IOException, URISyntaxException
    {
        final String iotHubHostname = "test-iothubhostname";
        final String deviceId = "test-deviceid";
        final String deviceKey = "test-devicekey";
        final IotHubClientProtocol protocol = IotHubClientProtocol.AMQPS;
        final Map<String, Object> context = new HashMap<>();
        new NonStrictExpectations()
        {
            {
                mockTransport.isEmpty();
                result = true;
            }
        };

        IotHubClient client =
                new IotHubClient(iotHubHostname, deviceId, deviceKey, protocol);
        client.open();
        client.close();
        client.sendEventAsync(mockMsg, mockCallback, context);
    }

    // Tests_SRS_IOTHUBCLIENT_11_012: [The function shall set the message callback, with its associated context.]
    @Test
    public void setMessageCallbackSetsMessageCallback(
            @Mocked final ScheduledExecutorService mockScheduler,
            @Mocked final AmqpsTransport mockTransport,
            @Mocked final IotHubMessageCallback mockCallback)
            throws IOException, URISyntaxException
    {
        final String iotHubHostname = "test-iothubhostname";
        final String deviceId = "test-deviceid";
        final String deviceKey = "test-devicekey";
        final IotHubClientProtocol protocol = IotHubClientProtocol.AMQPS;
        final Map<String, Object> context = new HashMap<>();

        IotHubClient client =
                new IotHubClient(iotHubHostname, deviceId, deviceKey, protocol);
        client.setMessageCallback(mockCallback, context);

        new Verifications()
        {
            {
                mockConfig.setMessageCallback(mockCallback, any);
            }
        };
    }

    // Tests_SRS_IOTHUBCLIENT_11_012: [The function shall set the message callback, with its associated context.]
    @Test
    public void setMessageCallbackSetsCallbackContext(
            @Mocked final ScheduledExecutorService mockScheduler,
            @Mocked final AmqpsTransport mockTransport,
            @Mocked final IotHubMessageCallback mockCallback)
            throws IOException, URISyntaxException
    {
        final String iotHubHostname = "test-iothubhostname";
        final String deviceId = "test-deviceid";
        final String deviceKey = "test-devicekey";
        final IotHubClientProtocol protocol = IotHubClientProtocol.AMQPS;
        final Map<String, Object> context = new HashMap<>();

        IotHubClient client =
                new IotHubClient(iotHubHostname, deviceId, deviceKey, protocol);
        client.setMessageCallback(mockCallback, context);

        new Verifications()
        {
            {
                mockConfig.setMessageCallback(
                        (IotHubMessageCallback) any, context);
            }
        };
    }

    // Tests_SRS_IOTHUBCLIENT_11_032: [If the callback is null but the context is non-null, the function shall throw an IllegalArgumentException.]
    @Test(expected = IllegalArgumentException.class)
    public void setMessageCallbackRejectsNullCallbackAndNonnullContext(
            @Mocked final ScheduledExecutorService mockScheduler,
            @Mocked final AmqpsTransport mockTransport)
            throws IOException, URISyntaxException
    {
        final String iotHubHostname = "test-iothubhostname";
        final String deviceId = "test-deviceid";
        final String deviceKey = "test-devicekey";
        final IotHubClientProtocol protocol = IotHubClientProtocol.AMQPS;
        final Map<String, Object> context = new HashMap<>();

        IotHubClient client =
                new IotHubClient(iotHubHostname, deviceId, deviceKey, protocol);
        client.setMessageCallback(null, context);
    }

    // Tests_SRS_IOTHUBCLIENT_02_001: [If optionName is null or not an option handled by the client, then it shall throw IllegalArgumentException.]
    @Test(expected = IllegalArgumentException.class)
    public void setOptionWithnullOptionNameThrows(
            @Mocked final ScheduledExecutorService mockScheduler,
            @Mocked final AmqpsTransport mockTransport)
            throws IOException, URISyntaxException
    {
        final String iotHubHostname = "test-iothubhostname";
        final String deviceId = "test-deviceid";
        final String deviceKey = "test-devicekey";
        final IotHubClientProtocol protocol = IotHubClientProtocol.HTTPS;

        IotHubClient client =
                new IotHubClient(iotHubHostname, deviceId, deviceKey, protocol);

        long someMiliseconds = 4;
        client.setOption(null, someMiliseconds);
    }

    // Tests_SRS_IOTHUBCLIENT_02_001: [If optionName is null or not an option handled by the client, then it shall throw IllegalArgumentException.]
    @Test(expected = IllegalArgumentException.class)
    public void setOptionWithUnknownOptionNameThrows(
            @Mocked final ScheduledExecutorService mockScheduler,
            @Mocked final HttpsTransport mockTransport)
            throws IOException, URISyntaxException
    {
        final String iotHubHostname = "test-iothubhostname";
        final String deviceId = "test-deviceid";
        final String deviceKey = "test-devicekey";
        final IotHubClientProtocol protocol = IotHubClientProtocol.HTTPS;

        IotHubClient client =
                new IotHubClient(iotHubHostname, deviceId, deviceKey, protocol);

        long someMiliseconds = 4;
        client.setOption("thisIsNotAHandledOption", someMiliseconds);
    }

    //Tests_SRS_IOTHUBCLIENT_02_003: [Available only for HTTP.]
    @Test(expected = IllegalArgumentException.class)
    public void setOptionMinimumPollingIntervalWithAMQPfails(
            @Mocked final ScheduledExecutorService mockScheduler,
            @Mocked final AmqpsTransport mockTransport)
            throws IOException, URISyntaxException
    {
        final String iotHubHostname = "test-iothubhostname";
        final String deviceId = "test-deviceid";
        final String deviceKey = "test-devicekey";
        final IotHubClientProtocol protocol = IotHubClientProtocol.AMQPS;
        long someMiliseconds = 4;

        IotHubClient client =
                new IotHubClient(iotHubHostname, deviceId, deviceKey, protocol);

            client.setOption("SetMinimumPollingInterval", someMiliseconds);

    }

    //Tests_SRS_IOTHUBCLIENT_02_004: [Value needs to have type long].
    @Test(expected = IllegalArgumentException.class)
    public void setOptionMinimumPollingIntervalWithStringInsteadOfLongFails(
            @Mocked final ScheduledExecutorService mockScheduler,
            @Mocked final HttpsTransport mockTransport)
            throws IOException, URISyntaxException
    {
        final String iotHubHostname = "test-iothubhostname";
        final String deviceId = "test-deviceid";
        final String deviceKey = "test-devicekey";
        final IotHubClientProtocol protocol = IotHubClientProtocol.HTTPS;

        IotHubClient client =
                new IotHubClient(iotHubHostname, deviceId, deviceKey, protocol);
        client.setOption("SetMinimumPollingInterval", "thisIsNotALong");
    }

    //Tests_SRS_IOTHUBCLIENT_02_005: [Setting the option can only be done before open call.]
    @Test(expected = IllegalStateException.class)
    public void setOptionMinimumPollingIntervalAfterOpenFails(
            @Mocked final ScheduledExecutorService mockScheduler,
            @Mocked final HttpsTransport mockTransport)
            throws IOException, URISyntaxException
    {
        final String iotHubHostname = "test-iothubhostname";
        final String deviceId = "test-deviceid";
        final String deviceKey = "test-devicekey";
        final IotHubClientProtocol protocol = IotHubClientProtocol.HTTPS;

        IotHubClient client =
                new IotHubClient(iotHubHostname, deviceId, deviceKey, protocol);
        client.open();
        long value = 3;
        client.setOption("SetMinimumPollingInterval", value);
    }

    //Tests_SRS_IOTHUBCLIENT_02_002: ["SetMinimumPollingInterval" - time in miliseconds between 2 consecutive polls.] */
    @Test
    public void setOptionMinimumPollingIntervalSucceeds(
            @Mocked final ScheduledExecutorService mockScheduler,
            @Mocked final HttpsTransport mockTransport)
            throws IOException, URISyntaxException
    {
        final String iotHubHostname = "test-iothubhostname";
        final String deviceId = "test-deviceid";
        final String deviceKey = "test-devicekey";
        final IotHubClientProtocol protocol = IotHubClientProtocol.HTTPS;

        IotHubClient client =
                new IotHubClient(iotHubHostname, deviceId, deviceKey, protocol);
        long value = 3;
        client.setOption("SetMinimumPollingInterval", value);

        client.open();

        new Verifications()
        {
            {
               mockScheduler.scheduleAtFixedRate((IotHubSendTask) any,
                        anyLong, 3,
                        TimeUnit.MILLISECONDS);
            }
        };
    }
}
