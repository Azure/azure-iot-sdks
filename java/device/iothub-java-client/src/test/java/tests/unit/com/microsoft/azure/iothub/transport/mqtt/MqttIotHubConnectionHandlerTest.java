// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

package tests.unit.com.microsoft.azure.iothub.transport.mqtt;

import com.microsoft.azure.iothub.transport.mqtt.MqttIotHubConnectionHandler;
import mockit.*;
import org.eclipse.paho.client.mqttv3.*;
import org.eclipse.paho.client.mqttv3.persist.MemoryPersistence;
import org.junit.Test;

import java.io.IOException;

public class MqttIotHubConnectionHandlerTest
{
    @Mocked
    MqttAsyncClient mockClient;

    @Mocked
    MqttConnectOptions mockConnectionOptions;

    @Mocked
    IMqttDeliveryToken mockMqttToken;

    @Mocked
    MemoryPersistence mockMemoryPersistence;

    // Tests_SRS_MQTTIOTHUBCONNECTIONHANDLER_15_001: [The constructor shall throw a new IllegalArgumentException
    // if any of the input parameters is null or empty.]
    @Test(expected = IllegalArgumentException.class)
    public void constructorThrowsIllegalArgumentExceptionIfHostNameIsEmpty(){
        final String hostName = "";
        final String deviceId = "test-deviceId";
        final String sasToken = "test-token";

        new MqttIotHubConnectionHandler(hostName,  deviceId, sasToken);
    }

    // Tests_SRS_MQTTIOTHUBCONNECTIONHANDLER_15_001: [The constructor shall throw a new IllegalArgumentException
    // if any of the input parameters is null or empty.]
    @Test(expected = IllegalArgumentException.class)
    public void constructorThrowsIllegalArgumentExceptionIfHostNameIsNull(){
        final String hostName = null;
        final String deviceId = "test-deviceId";
        final String sasToken = "test-token";

        new MqttIotHubConnectionHandler(hostName, deviceId, sasToken);
    }

    // Tests_SRS_MQTTIOTHUBCONNECTIONHANDLER_15_001: [The constructor shall throw a new IllegalArgumentException
    // if any of the input parameters is null or empty.]
    @Test(expected = IllegalArgumentException.class)
    public void constructorThrowsIllegalArgumentExceptionIfDeviceIDIsEmpty(){
        final String hostName = "test.host.name";
        final String deviceId = "";
        final String sasToken = "test-token";

        new MqttIotHubConnectionHandler(hostName, deviceId, sasToken);
    }

    // Tests_SRS_MQTTIOTHUBCONNECTIONHANDLER_15_001: [The constructor shall throw a new IllegalArgumentException
    // if any of the input parameters is null or empty.]
    @Test(expected = IllegalArgumentException.class)
    public void constructorThrowsIllegalArgumentExceptionIfDeviceIDIsNull(){
        final String hostName = "test.host.name";
        final String deviceId = null;
        final String sasToken = "test-token";

        new MqttIotHubConnectionHandler(hostName, deviceId, sasToken);
    }

    // Tests_SRS_MQTTIOTHUBCONNECTIONHANDLER_15_001: [The constructor shall throw a new IllegalArgumentException
    // if any of the input parameters is null or empty.]
    @Test(expected = IllegalArgumentException.class)
    public void constructorThrowsIllegalArgumentExceptionIfSasTokenIsEmpty(){
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String sasToken = "";

        new MqttIotHubConnectionHandler(hostName, deviceId, sasToken);
    }

    // Tests_SRS_MQTTIOTHUBCONNECTIONHANDLER_15_001: [The constructor shall throw a new IllegalArgumentException
    // if any of the input parameters is null or empty.]
    @Test(expected = IllegalArgumentException.class)
    public void constructorThrowsIllegalArgumentExceptionIfSasTokenIsNull(){
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String sasToken = null;

        new MqttIotHubConnectionHandler(hostName, deviceId, sasToken);
    }

    // Tests_SRS_MQTTIOTHUBCONNECTIONHANDLER_15_002: [The Constructor shall copy
    // all input parameters to private member variables.]
    // Tests_SRS_MQTTIOTHUBCONNECTIONHANDLER_15_005: [The function shall initialize its MqttAsyncClient
    // using the saved host name, device id and sas token.]
    @Test
    public void constructorCopiesAllData()
    {
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String sasToken = "test-token";

        MqttIotHubConnectionHandler sender = new MqttIotHubConnectionHandler(hostName, deviceId, sasToken);
        try
        {
            sender.open();
        }
        catch (IOException e)
        {
            e.printStackTrace();
        }

        new Verifications()
        {
            {
                new MqttIotHubConnectionHandler(hostName, deviceId, sasToken);
            }
        };
    }

    // Tests_SRS_MQTTIOTHUBCONNECTIONHANDLER_15_006: [The function shall invalidate its MqttAsyncClient.]
    // Tests_SRS_MQTTIOTHUBCONNECTIONHANDLER_15_007: [If the MqttAsyncClient has not been initialized,
    // the function shall throw a new IOException.]
    @Test(expected = IOException.class)
    public void closeInvalidatesSenderHandlerObject() throws IOException{
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String sasToken = "test-token";
        final byte[] msgBody = { 0x61, 0x62, 0x63 };

        MqttIotHubConnectionHandler handler = new MqttIotHubConnectionHandler(hostName, deviceId, sasToken);
        handler.open();
        handler.close();
        handler.send(msgBody);
    }

    // Tests_SRS_MQTTIOTHUBCONNECTIONHANDLER_15_010: [The function shall connect to the IoT Hub broker.]
    // Tests_SRS_MQTTIOTHUBCONNECTIONHANDLER_15_011: [The function shall publish the content of the message
    // to the specific device id topic]
    @Test
    public void sendPublishesOneMessageCorrectly() throws Exception{
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String sasToken = "test-token";
        final byte[] msgBody = { 0x61, 0x62, 0x63 };
        final MqttMessage mqttMessage = new MqttMessage(msgBody);
        mqttMessage.setQos(1);

        MqttIotHubConnectionHandler handler = new MqttIotHubConnectionHandler(hostName, deviceId, sasToken);

        handler.open();
        handler.send(msgBody, 1);

        new VerificationsInOrder()
        {
            {
                mockClient.connect((MqttConnectOptions) any);
                mockClient.publish(anyString, (MqttMessage) any);
                mockClient.disconnect();
            }
        };
    }

    // Tests_SRS_MQTTIOTHUBCONNECTIONHANDLER_15_007: [If the MqttAsyncClient has not been initialized,
    // the function shall throw a new IOException.]
    @Test(expected = IOException.class)
    public void sendThrowsExceptionIfClientNotInitialized() throws Exception{
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String sasToken = "test-token";
        final byte[] msgBody = { 0x61, 0x62, 0x63 };
        final MqttMessage mqttMessage = new MqttMessage(msgBody);
        mqttMessage.setQos(1);

        MqttIotHubConnectionHandler handler = new MqttIotHubConnectionHandler(hostName, deviceId, sasToken);

        handler.send(msgBody, 1);
    }
}