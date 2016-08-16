// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

package tests.unit.com.microsoft.azure.iothub.transport.mqtt;

import com.microsoft.azure.iothub.DeviceClientConfig;
import com.microsoft.azure.iothub.IotHubStatusCode;
import com.microsoft.azure.iothub.Message;
import com.microsoft.azure.iothub.auth.IotHubSasToken;
import com.microsoft.azure.iothub.net.IotHubUri;
import com.microsoft.azure.iothub.transport.State;
import com.microsoft.azure.iothub.transport.TransportUtils;
import com.microsoft.azure.iothub.transport.mqtt.MqttIotHubConnection;
import mockit.Deencapsulation;
import mockit.Mocked;
import mockit.NonStrictExpectations;
import mockit.Verifications;
import org.eclipse.paho.client.mqttv3.*;
import org.junit.Test;

import java.io.IOException;
import java.net.URLEncoder;
import java.util.Queue;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNull;

/** Unit tests for MqttIotHubConnection. */
public class MqttIotHubConnectionTest
{
    private static String sslPrefix = "ssl://";
    private static String sslPortSuffix = ":8883";
    final String iotHubHostName = "test.host.name";
    final String hubName = "test.iothub";
    final String deviceId = "test-deviceId";
    final String deviceKey = "test-devicekey?&test";
    final String resourceUri = "test-resource-uri";
    final int qos = 1;
    final String publishTopic = "devices/test-deviceId/messages/events/";
    final String subscribeTopic = "devices/test-deviceId/messages/devicebound/#";

    @Mocked
    protected DeviceClientConfig mockConfig;

    @Mocked
    protected MqttAsyncClient mockMqttAsyncClient;

    @Mocked
    protected IMqttToken mockMqttToken;

    @Mocked
    protected IMqttDeliveryToken mockMqttDeliveryToken;

    @Mocked
    protected IotHubSasToken mockToken;

    @Mocked
    protected MqttMessage mockMqttMessage;

    @Mocked
    IotHubUri mockIotHubUri;

    // Tests_SRS_MQTTIOTHUBCONNECTION_15_001: [The constructor shall save the configuration.]
    @Test
    public void constructorSavesCorrectConfig() throws IOException {

        baseExpectations();

        MqttIotHubConnection connection = new MqttIotHubConnection(mockConfig);

        DeviceClientConfig actualClientConfig = Deencapsulation.getField(connection, "config");
        DeviceClientConfig expectedClientConfig = mockConfig;

        assertEquals(expectedClientConfig, actualClientConfig);
    }

    // Tests_SRS_MQTTIOTHUBCONNECTION_15_002: [The constructor shall create the publish and subscribe
    // topic for the specified device id.]
    @Test
    public void constructorCreatesPublishAndSubscribeTopics(){
        this.baseExpectations();

        MqttIotHubConnection connection =  new MqttIotHubConnection(mockConfig);

        String publishTopic = Deencapsulation.getField(connection, "publishTopic");
        String subscribeTopic = Deencapsulation.getField(connection, "subscribeTopic");

        assertEquals(this.publishTopic, publishTopic);
        assertEquals(this.subscribeTopic, subscribeTopic);
    }

    // Tests_SRS_MQTTIOTHUBCONNECTION_15_003: [The constructor shall throw a new IllegalArgumentException
    // if any of the parameters of the configuration is null or empty.]
    @Test(expected = IllegalArgumentException.class)
    public void constructorThrowsIllegalArgumentExceptionIfHostNameIsEmpty(){
        new NonStrictExpectations() {
            {
                mockConfig.getIotHubHostname();
                result = "";
                mockConfig.getIotHubName();
                result = hubName;
                mockConfig.getDeviceId();
                result = deviceId;
                mockConfig.getDeviceKey();
                result = deviceKey;
            }
        };

        new MqttIotHubConnection(mockConfig);
    }

    // Tests_SRS_MQTTIOTHUBCONNECTION_15_003: [The constructor shall throw a new IllegalArgumentException
    // if any of the parameters of the configuration is null or empty.]
    @Test(expected = IllegalArgumentException.class)
    public void constructorThrowsIllegalArgumentExceptionIfHostNameIsNull(){
        new NonStrictExpectations() {
            {
                mockConfig.getIotHubHostname();
                result = null;
                mockConfig.getIotHubName();
                result = hubName;
                mockConfig.getDeviceId();
                result = deviceId;
                mockConfig.getDeviceKey();
                result = deviceKey;
            }
        };

        new MqttIotHubConnection(mockConfig);
    }

    // Tests_SRS_MQTTIOTHUBCONNECTION_15_003: [The constructor shall throw a new IllegalArgumentException
    // if any of the parameters of the configuration is null or empty.]
    @Test(expected = IllegalArgumentException.class)
    public void constructorThrowsIllegalArgumentExceptionIfDeviceIDIsEmpty(){
        new NonStrictExpectations() {
            {
                mockConfig.getIotHubHostname();
                result = iotHubHostName;
                mockConfig.getIotHubName();
                result = hubName;
                mockConfig.getDeviceId();
                result = "";
                mockConfig.getDeviceKey();
                result = deviceKey;
            }
        };

        new MqttIotHubConnection(mockConfig);
    }

    // Tests_SRS_MQTTIOTHUBCONNECTION_15_003: [The constructor shall throw a new IllegalArgumentException
    // if any of the parameters of the configuration is null or empty.]
    @Test(expected = IllegalArgumentException.class)
    public void constructorThrowsIllegalArgumentExceptionIfDeviceIDIsNull(){
        new NonStrictExpectations() {
            {
                mockConfig.getIotHubHostname();
                result = iotHubHostName;
                mockConfig.getIotHubName();
                result = hubName;
                mockConfig.getDeviceId();
                result = null;
                mockConfig.getDeviceKey();
                result = deviceKey;
            }
        };

        new MqttIotHubConnection(mockConfig);
    }

    // Tests_SRS_MQTTIOTHUBCONNECTION_15_003: [The constructor shall throw a new IllegalArgumentException
    // if any of the parameters of the configuration is null or empty.]
    @Test(expected = IllegalArgumentException.class)
    public void constructorThrowsIllegalArgumentExceptionIfUserNameIsEmpty(){
        new NonStrictExpectations() {
            {
                mockConfig.getIotHubHostname();
                result = iotHubHostName;
                mockConfig.getIotHubName();
                result = hubName;
                mockConfig.getDeviceId();
                result = deviceId;
                mockConfig.getDeviceKey();
                result = "";
            }
        };

        new MqttIotHubConnection(mockConfig);
    }

    // Tests_SRS_MQTTIOTHUBCONNECTION_15_003: [The constructor shall throw a new IllegalArgumentException
    // if any of the parameters of the configuration is null or empty.]
    @Test(expected = IllegalArgumentException.class)
    public void constructorThrowsIllegalArgumentExceptionIfUserNameIsNull(){
        new NonStrictExpectations() {
            {
                mockConfig.getIotHubHostname();
                result = iotHubHostName;
                mockConfig.getIotHubName();
                result = hubName;
                mockConfig.getDeviceId();
                result = deviceId;
                mockConfig.getDeviceKey();
                result = null;
            }
        };

        new MqttIotHubConnection(mockConfig);
    }

    // Tests_SRS_MQTTIOTHUBCONNECTION_15_003: [The constructor shall throw a new IllegalArgumentException
    // if any of the parameters of the configuration is null or empty.]
    @Test(expected = IllegalArgumentException.class)
    public void constructorThrowsIllegalArgumentExceptionIfSasTokenIsEmpty(){
        new NonStrictExpectations() {
            {
                mockConfig.getIotHubHostname();
                result = iotHubHostName;
                mockConfig.getIotHubName();
                result = "";
                mockConfig.getDeviceId();
                result = deviceId;
                mockConfig.getDeviceKey();
                result = deviceKey;
            }
        };

        new MqttIotHubConnection(mockConfig);
    }

    // Tests_SRS_MQTTIOTHUBCONNECTION_15_003: [The constructor shall throw a new IllegalArgumentException
    // if any of the parameters of the configuration is null or empty.]
    @Test(expected = IllegalArgumentException.class)
    public void constructorThrowsIllegalArgumentExceptionIfSasTokenIsNull(){
        new NonStrictExpectations() {
            {
                mockConfig.getIotHubHostname();
                result = iotHubHostName;
                mockConfig.getIotHubName();
                result = null;
                mockConfig.getDeviceId();
                result = deviceId;
                mockConfig.getDeviceKey();
                result = deviceKey;
            }
        };

        new MqttIotHubConnection(mockConfig);
    }

    // Tests_SRS_MQTTIOTHUBCONNECTION_15_004: [The function shall establish an MQTT connection with an IoT Hub
    // using the provided host name, user name, device ID, and sas token.]
    @Test
    public void openEstablishesConnectionUsingCorrectConfig() throws IOException, MqttException
    {
        baseExpectations();
        openExpectations();

        MqttIotHubConnection connection = new MqttIotHubConnection(mockConfig);
        connection.open();

        final MqttConnectOptions actualConnectionOptions = Deencapsulation.getField(connection, "connectionOptions");

        String clientIdentifier = "DeviceClientType=" + URLEncoder.encode(TransportUtils.javaDeviceClientIdentifier + TransportUtils.clientVersion, "UTF-8");
        assertEquals(iotHubHostName + "/" + deviceId + "/" + clientIdentifier, actualConnectionOptions.getUserName());

        String expectedSasToken = mockToken.toString();
        String actualUserPassword = new String(actualConnectionOptions.getPassword());

        assertEquals(expectedSasToken, actualUserPassword);

        State expectedState = State.OPEN;
        State actualState =  Deencapsulation.getField(connection, "state");
        assertEquals(expectedState, actualState);

        new Verifications()
        {
            {
                new MqttAsyncClient(sslPrefix + iotHubHostName + sslPortSuffix, deviceId, (MqttClientPersistence) any);
                new MqttConnectOptions();
                mockMqttAsyncClient.connect(actualConnectionOptions);
                mockMqttAsyncClient.subscribe(subscribeTopic, qos);

            }
        };
    }

    // Tests_SRS_MQTTIOTHUBCONNECTION_15_005: [If an MQTT connection is unable to be established for any reason,
    // the function shall throw an IOException.]
    @Test(expected = IOException.class)
    public void openThrowsIOExceptionIfConnectionFails() throws IOException, MqttException {
        baseExpectations();

        new NonStrictExpectations()
        {
            {
                new IotHubSasToken(mockConfig, anyLong);
                result = mockToken;
                new MqttAsyncClient(sslPrefix + iotHubHostName + sslPortSuffix, deviceId, (MqttClientPersistence) any);
                result = new MqttException(anyInt);
            }
        };

        MqttIotHubConnection connection = new MqttIotHubConnection(mockConfig);
        connection.open();
    }

    // Tests_SRS_MQTTIOTHUBCONNECTION_15_006: [If the MQTT connection is already open, the function shall do nothing.]
    @Test
    public void openDoesNothingIfAlreadyOpened() throws IOException, MqttException
    {
        baseExpectations();
        openExpectations();

        MqttIotHubConnection connection = new MqttIotHubConnection(mockConfig);
        connection.open();
        connection.open();

        new Verifications()
        {
            {
                new MqttAsyncClient(sslPrefix + iotHubHostName + sslPortSuffix, deviceId, (MqttClientPersistence) any); times = 1;
            }
        };
    }

    // Tests_SRS_MQTTIOTHUBCONNECTION_15_005: [The function shall close the MQTT connection.]
    @Test
    public void closeClosesMqttConnection() throws Exception
    {
        baseExpectations();
        openExpectations();
        new NonStrictExpectations()
        {
            {
                mockMqttAsyncClient.unsubscribe(subscribeTopic);
                result = mockMqttToken;
                result = null;
                mockMqttAsyncClient.disconnect();
                result = mockMqttToken;
                result = null;
            }
        };

        MqttIotHubConnection connection = new MqttIotHubConnection(mockConfig);
        connection.open();
        connection.close();

        State expectedState = State.CLOSED;
        State actualState =  Deencapsulation.getField(connection, "state");
        assertEquals(expectedState, actualState);

        MqttAsyncClient actualClient = Deencapsulation.getField(connection, "asyncClient");
        assertNull(actualClient);

        new Verifications()
        {
            {
                mockMqttAsyncClient.disconnect();
            }
        };
    }

    // Tests_SRS_MQTTIOTHUBCONNECTION_15_007: [If the MQTT connection is closed, the function shall do nothing.]
    @Test
    public void closeDoesNothingIfConnectionNotYetOpened() throws Exception
    {
        baseExpectations();
        new NonStrictExpectations()
        {
            {
                mockMqttAsyncClient.unsubscribe(subscribeTopic);
                result = mockMqttToken;
                result = null;
                mockMqttAsyncClient.disconnect();
                result = mockMqttToken;
                result = null;
            }
        };

        MqttIotHubConnection connection = new MqttIotHubConnection(mockConfig);
        connection.close();

        State expectedState = State.CLOSED;
        State actualState =  Deencapsulation.getField(connection, "state");
        assertEquals(expectedState, actualState);
    }

    // Tests_SRS_MQTTIOTHUBCONNECTION_15_007: [If the MQTT connection is closed, the function shall do nothing.]
    @Test
    public void closeDoesNothingIfConnectionAlreadyClosed() throws Exception
    {
        baseExpectations();
        openExpectations();
        new NonStrictExpectations()
        {
            {
                mockMqttAsyncClient.unsubscribe(subscribeTopic);
                result = mockMqttToken;
                result = null;
                mockMqttAsyncClient.disconnect();
                result = mockMqttToken;
                result = null;
            }
        };

        MqttIotHubConnection connection = new MqttIotHubConnection(mockConfig);
        connection.open();
        connection.close();
        connection.close();

        new Verifications()
        {
            {
                mockMqttAsyncClient.disconnect(); times = 1;
            }
        };
    }

    // Tests_SRS_MQTTIOTHUBCONNECTION_15_008: [The function shall send an event message to the IoT Hub
    // given in the configuration.]
    // Tests_SRS_MQTTIOTHUBCONNECTION_15_009: [The function shall send the message payload.]
    // Tests_SRS_MQTTIOTHUBCONNECTION_15_011: [If the message was successfully received by the service,
    // the function shall return status code OK_EMPTY.]
    @Test
    public void sendEventPublishesEventCorrectlyToIotHub(@Mocked final Message mockMsg) throws IOException, MqttException
    {
        baseExpectations();
        openExpectations();

        final byte[] msgBody = { 0x61, 0x62, 0x63 };
        new NonStrictExpectations()
        {
            {
                mockMsg.getBytes();
                result = msgBody;
                new MqttMessage(msgBody);
                result = mockMqttMessage;
                mockMqttAsyncClient.publish(publishTopic, mockMqttMessage);
                result = mockMqttDeliveryToken;
            }
        };

        MqttIotHubConnection connection = new MqttIotHubConnection(mockConfig);
        connection.open();
        IotHubStatusCode result = connection.sendEvent(mockMsg);

        assertEquals(IotHubStatusCode.OK_EMPTY, result);

        new Verifications()
        {
            {
                new MqttMessage(msgBody);
                mockMqttMessage.setQos(qos);
                mockMqttAsyncClient.publish(publishTopic, (MqttMessage) any);
            }
        };
    }

    // Tests_SRS_MQTTIOTHUBCONNECTION_15_010: [If the message is null or empty,
    // the function shall return status code BAD_FORMAT.]
    @Test
    public void sendEventReturnsBadFormatIfMessageIsNull() throws IOException, MqttException
    {
        baseExpectations();
        openExpectations();

        MqttIotHubConnection connection = new MqttIotHubConnection(mockConfig);
        connection.open();
        IotHubStatusCode result = connection.sendEvent(null);

        assertEquals(IotHubStatusCode.BAD_FORMAT, result);
    }

    // Tests_SRS_MQTTIOTHUBCONNECTION_15_010: [If the message is null or empty,
    // the function shall return status code BAD_FORMAT.]
    @Test
    public void sendEventReturnsBadFormatIfMessageHasNullBody(@Mocked final Message mockMsg) throws IOException, MqttException
    {
        baseExpectations();
        openExpectations();

        new NonStrictExpectations()
        {
            {
                mockMsg.getBytes();
                result = null;
            }
        };

        MqttIotHubConnection connection = new MqttIotHubConnection(mockConfig);
        connection.open();
        IotHubStatusCode result = connection.sendEvent(null);

        assertEquals(IotHubStatusCode.BAD_FORMAT, result);
    }

    // Tests_SRS_MQTTIOTHUBCONNECTION_15_010: [If the message is null or empty,
    // the function shall return status code BAD_FORMAT.]
    @Test
    public void sendEventReturnsBadFormatIfMessageHasEmptyBody(@Mocked final Message mockMsg) throws IOException, MqttException
    {
        baseExpectations();

        new NonStrictExpectations()
        {
            {
                mockMsg.getBytes();
                result = new byte[0];
            }
        };

        MqttIotHubConnection connection = new MqttIotHubConnection(mockConfig);
        connection.open();
        IotHubStatusCode result = connection.sendEvent(mockMsg);

        assertEquals(IotHubStatusCode.BAD_FORMAT, result);
    }

    // Tests_SRS_MQTTIOTHUBCONNECTION_15_013: [If the MQTT connection is closed,
    // the function shall throw an IllegalStateException.]
    @Test(expected = IllegalStateException.class)
    public void sendEventFailsIfConnectionNotYetOpened(@Mocked final Message mockMsg) throws IOException
    {
        baseExpectations();

        final byte[] msgBody = { 0x61, 0x62, 0x63 };
        new NonStrictExpectations()
        {
            {
                mockMsg.getBytes();
                result = msgBody;
            }
        };

        MqttIotHubConnection connection = new MqttIotHubConnection(mockConfig);
        connection.sendEvent(mockMsg);
    }

    // Tests_SRS_MQTTIOTHUBCONNECTION_15_013: [If the MQTT connection is closed,
    // the function shall throw an IllegalStateException.]
    @Test(expected = IllegalStateException.class)
    public void sendEventFailsIfConnectionClosed(@Mocked final Message mockMsg) throws IOException
    {
        baseExpectations();

        final byte[] msgBody = { 0x61, 0x62, 0x63 };
        new NonStrictExpectations()
        {
            {
                mockMsg.getBytes();
                result = msgBody;
            }
        };

        MqttIotHubConnection connection = new MqttIotHubConnection(mockConfig);
        connection.open();
        connection.close();
        connection.sendEvent(mockMsg);
    }

    // Tests_SRS_MQTTIOTHUBCONNECTION_15_012: [If the message was not successfully received by the service,
    // the function shall return status code ERROR.]
    @Test
    public void sendEventReturnsErrorIfMessageNotReceived(@Mocked final Message mockMsg) throws IOException, MqttException
    {
        baseExpectations();

        final byte[] msgBody = { 0x61, 0x62, 0x63 };
        new NonStrictExpectations()
        {
            {
                mockMsg.getBytes();
                result = msgBody;
                new MqttMessage(msgBody);
                result = mockMqttMessage;
                mockMqttAsyncClient.publish(publishTopic, mockMqttMessage);
                result = new MqttException(anyInt);
            }
        };

        MqttIotHubConnection connection = new MqttIotHubConnection(mockConfig);
        connection.open();
        IotHubStatusCode actualStatus = connection.sendEvent(mockMsg);

        IotHubStatusCode expectedStatus = IotHubStatusCode.ERROR;
        assertEquals(expectedStatus, actualStatus);
    }

    // Tests_SRS_MQTTIOTHUBCONNECTION_15_014: [The function shall attempt to consume a message
    // from the received messages queue.]
    @Test
    public void receiveMessageConsumesAMessageFromReceivedQueue() throws IOException, MqttException
    {
        baseExpectations();
        openExpectations();

        MqttIotHubConnection connection = new MqttIotHubConnection(mockConfig);
        connection.open();

        Queue<Message> receivedMessagesQueue =  Deencapsulation.getField(connection, "receivedMessagesQueue");
        byte[] expectedMessageBody = { 0x61, 0x62, 0x63 };
        receivedMessagesQueue.add(new Message(expectedMessageBody));
        Message message = connection.receiveMessage();
        byte[] actualMessageBody = message.getBytes();

        for (int i = 0; i < expectedMessageBody.length; i++)
        {
            assertEquals(expectedMessageBody[i], actualMessageBody[i]);
        }
    }

    // Tests_SRS_MQTTIOTHUBCONNECTION_15_015: [If the MQTT connection is closed,
    // the function shall throw an IllegalStateException.]
    @Test(expected = IllegalStateException.class)
    public void receiveMessageFailsIfConnectionClosed(@Mocked final Message mockMsg) throws IOException
    {
        baseExpectations();

        MqttIotHubConnection connection = new MqttIotHubConnection(mockConfig);
        connection.receiveMessage();
    }

    // Tests_SRS_MQTTIOTHUBCONNECTION_15_015: [If the MQTT connection is closed,
    // the function shall throw an IllegalStateException.]
    @Test(expected = IllegalStateException.class)
    public void receiveMessageFailsIfConnectionAlreadyClosed(@Mocked final Message mockMsg) throws IOException
    {
        baseExpectations();

        MqttIotHubConnection connection = new MqttIotHubConnection(mockConfig);
        connection.open();
        connection.close();
        connection.receiveMessage();
    }

    private void baseExpectations()
    {
        new NonStrictExpectations() {
            {
                mockConfig.getIotHubHostname(); result = iotHubHostName;
                mockConfig.getIotHubName(); result = hubName;
                mockConfig.getDeviceId(); result = deviceId;
                mockConfig.getDeviceKey(); result = deviceKey;
            }
        };
    }

    private void openExpectations() throws MqttException
    {
        new NonStrictExpectations()
        {
            {
                new IotHubSasToken(mockConfig, anyLong);
                result = mockToken;
                new MqttAsyncClient(sslPrefix + iotHubHostName + sslPortSuffix, deviceId, (MqttClientPersistence) any);
                result = mockMqttAsyncClient;
                mockMqttAsyncClient.connect();
                result = mockMqttToken;
                mockMqttAsyncClient.subscribe(anyString, anyInt);
                result = mockMqttToken;
            }
        };
    }
}