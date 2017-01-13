/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package tests.unit.com.microsoft.azure.iothub.transport.amqps;

import com.microsoft.azure.iothub.DeviceClientConfig;
import com.microsoft.azure.iothub.IotHubMessageResult;
import com.microsoft.azure.iothub.auth.IotHubSasToken;
import com.microsoft.azure.iothub.net.IotHubUri;
import com.microsoft.azure.iothub.transport.State;
import com.microsoft.azure.iothub.transport.amqps.AmqpsIotHubConnection;
import com.microsoft.azure.iothub.transport.amqps.AmqpsMessage;
import com.microsoft.azure.iothub.transport.amqps.IotHubReactor;
import com.microsoft.azure.iothub.transport.amqps.ServerListener;
import mockit.*;
import org.apache.qpid.proton.Proton;
import org.apache.qpid.proton.amqp.Symbol;
import org.apache.qpid.proton.amqp.messaging.Accepted;
import org.apache.qpid.proton.amqp.messaging.Source;
import org.apache.qpid.proton.amqp.messaging.Target;
import org.apache.qpid.proton.amqp.transport.SenderSettleMode;
import org.apache.qpid.proton.engine.*;
import org.apache.qpid.proton.engine.impl.WebSocketImpl;
import org.apache.qpid.proton.message.Message;
import org.apache.qpid.proton.reactor.FlowController;
import org.apache.qpid.proton.reactor.Handshaker;
import org.apache.qpid.proton.reactor.Reactor;
import org.junit.Test;

import java.io.IOException;
import java.util.Map;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Future;

import static org.junit.Assert.assertEquals;

public class AmqpsIotHubConnectionTest {

    final String hostName = "test.host.name";
    final String hubName = "test.iothub";
    final String deviceId = "test-deviceId";
    final String deviceKey = "test-devicekey?&test";
    final String amqpPort = "5671";
    final String amqpWebSocketPort = "443";

    @Mocked
    protected Handshaker mockHandshaker;

    @Mocked
    protected FlowController mockFlowController;

    @Mocked
    protected Proton mockProton;

    @Mocked
    protected IotHubReactor mockIotHubReactor;

    @Mocked
    protected Reactor mockReactor;

    @Mocked
    protected DeviceClientConfig mockConfig;

    @Mocked
    protected IotHubUri mockIotHubUri;

    @Mocked
    protected IotHubSasToken mockToken;

    @Mocked
    protected Message mockProtonMessage;

    @Mocked
    protected AmqpsMessage mockAmqpsMessage;

    @Mocked
    protected IotHubSasToken mockSasToken;

    @Mocked
    protected Sender mockSender;

    @Mocked
    protected Receiver mockReceiver;

    @Mocked
    protected Connection mockConnection;

    @Mocked
    protected Session mockSession;

    @Mocked
    protected Event mockEvent;

    @Mocked
    protected Future mockReactorFuture;

    @Mocked
    protected ExecutorService mockExecutorService;

    @Mocked
    protected Delivery mockDelivery;

    @Mocked
    protected Transport mockTransport;

    @Mocked
    protected Sasl mockSasl;

    @Mocked
    protected SslDomain mockSslDomain;

    @Mocked
    protected String mockCertPath;

    @Mocked
    WebSocketImpl mockWebSocket;

    @Mocked
    ServerListener mockServerListener;

    @Mocked
    Target mockTarget;

    @Mocked
    Source mockSource;

    // Tests_SRS_AMQPSIOTHUBCONNECTION_15_001: [The constructor shall throw IllegalArgumentException if
    // any of the parameters of the configuration is null or empty.]
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

        new AmqpsIotHubConnection(mockConfig, false);
    }

    // Tests_SRS_AMQPSIOTHUBCONNECTION_15_001: [The constructor shall throw IllegalArgumentException if
    // any of the parameters of the configuration is null or empty.]
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

        new AmqpsIotHubConnection(mockConfig, false);
    }

    // Tests_SRS_AMQPSIOTHUBCONNECTION_15_001: [The constructor shall throw IllegalArgumentException if
    // any of the parameters of the configuration is null or empty.]
    @Test(expected = IllegalArgumentException.class)
    public void constructorThrowsIllegalArgumentExceptionIfDeviceIdIsEmpty(){
        new NonStrictExpectations() {
            {
                mockConfig.getIotHubHostname();
                result = hostName;
                mockConfig.getIotHubName();
                result = hubName;
                mockConfig.getDeviceId();
                result = "";
                mockConfig.getDeviceKey();
                result = deviceKey;
            }
        };

        new AmqpsIotHubConnection(mockConfig, false);
    }

    // Tests_SRS_AMQPSIOTHUBCONNECTION_15_001: [The constructor shall throw IllegalArgumentException if
    // any of the parameters of the configuration is null or empty.]
    @Test(expected = IllegalArgumentException.class)
    public void constructorThrowsIllegalArgumentExceptionIfDeviceIdIsNull(){
        new NonStrictExpectations() {
            {
                mockConfig.getIotHubHostname();
                result = hostName;
                mockConfig.getIotHubName();
                result = hubName;
                mockConfig.getDeviceId();
                result = null;
                mockConfig.getDeviceKey();
                result = deviceKey;
            }
        };

        new AmqpsIotHubConnection(mockConfig, false);
    }

    // Tests_SRS_AMQPSIOTHUBCONNECTION_15_001: [The constructor shall throw IllegalArgumentException if
    // any of the parameters of the configuration is null or empty.]
    @Test(expected = IllegalArgumentException.class)
    public void constructorThrowsIllegalArgumentExceptionIfUserNameIsEmpty(){
        new NonStrictExpectations() {
            {
                mockConfig.getIotHubHostname();
                result = hostName;
                mockConfig.getIotHubName();
                result = hubName;
                mockConfig.getDeviceId();
                result = deviceId;
                mockConfig.getDeviceKey();
                result = "";
            }
        };

        new AmqpsIotHubConnection(mockConfig, false);
    }

    // Tests_SRS_AMQPSIOTHUBCONNECTION_15_001: [The constructor shall throw IllegalArgumentException if
    // any of the parameters of the configuration is null or empty.]
    @Test(expected = IllegalArgumentException.class)
    public void constructorThrowsIllegalArgumentExceptionIfUserNameIsNull(){
        new NonStrictExpectations() {
            {
                mockConfig.getIotHubHostname();
                result = hostName;
                mockConfig.getIotHubName();
                result = hubName;
                mockConfig.getDeviceId();
                result = deviceId;
                mockConfig.getDeviceKey();
                result = null;
            }
        };

        new AmqpsIotHubConnection(mockConfig, false);
    }

    // Tests_SRS_AMQPSIOTHUBCONNECTION_15_001: [The constructor shall throw IllegalArgumentException if
    // any of the parameters of the configuration is null or empty.]
    @Test(expected = IllegalArgumentException.class)
    public void constructorThrowsIllegalArgumentExceptionIfHubNameIsEmpty(){
        new NonStrictExpectations() {
            {
                mockConfig.getIotHubHostname();
                result = hostName;
                mockConfig.getIotHubName();
                result = "";
                mockConfig.getDeviceId();
                result = deviceId;
                mockConfig.getDeviceKey();
                result = deviceKey;
            }
        };

        new AmqpsIotHubConnection(mockConfig, false);
    }

    // Tests_SRS_AMQPSIOTHUBCONNECTION_15_001: [The constructor shall throw IllegalArgumentException if
    // any of the parameters of the configuration is null or empty.]
    @Test(expected = IllegalArgumentException.class)
    public void constructorThrowsIllegalArgumentExceptionIfHubNameIsNull(){
        new NonStrictExpectations() {
            {
                mockConfig.getIotHubHostname();
                result = hostName;
                mockConfig.getIotHubName();
                result = null;
                mockConfig.getDeviceId();
                result = deviceId;
                mockConfig.getDeviceKey();
                result = deviceKey;
            }
        };

        new AmqpsIotHubConnection(mockConfig, false);
    }

    // Tests_SRS_AMQPSIOTHUBCONNECTION_15_002: [The constructor shall save the configuration into private member variables.]
    // Tests_SRS_AMQPSIOTHUBCONNECTION_15_003: [The constructor shall initialize the sender and receiver
    // endpoint private member variables using the send/receiveEndpointFormat constants and device id.]
    // Tests_SRS_AMQPSIOTHUBCONNECTION_15_004: [The constructor shall initialize a new Handshaker
    // (Proton) object to handle communication handshake.]
    // Tests_SRS_AMQPSIOTHUBCONNECTION_15_005: [The constructor shall initialize a new FlowController
    // (Proton) object to handle communication flow.]
    // Tests_SRS_AMQPSIOTHUBCONNECTION_15_006: [The constructor shall set its state to CLOSED.]
    @Test
    public void constructorCopiesAllData()
    {
        baseExpectations();

        AmqpsIotHubConnection connection = new AmqpsIotHubConnection(mockConfig, false);

        DeviceClientConfig actualConfig = Deencapsulation.getField(connection, "config");
        String actualHostName = Deencapsulation.getField(connection, "hostName");
        String actualUserName = Deencapsulation.getField(connection, "userName");
        String actualSendEndpoint = Deencapsulation.getField(connection, "sendEndpoint");
        String actualReceiveEndpoint = Deencapsulation.getField(connection, "receiveEndpoint");

        assertEquals(mockConfig, actualConfig);
        assertEquals(hostName + ":" + amqpPort, actualHostName);
        assertEquals(deviceId + "@sas." + hubName, actualUserName);

        String expectedSendEndpoint = "/devices/test-deviceId/messages/events";
        assertEquals(expectedSendEndpoint, actualSendEndpoint);

        String expectedReceiveEndpoint = "/devices/test-deviceId/messages/devicebound";
        assertEquals(expectedReceiveEndpoint, actualReceiveEndpoint);

        new Verifications()
        {
            {
                new Handshaker();
                times = 1;
                new FlowController();
                times = 1;
            }
        };

        State actualState = Deencapsulation.getField(connection, "state");
        assertEquals(State.CLOSED, actualState);
    }

    @Test
    public void constructorSetsHostNameCorrectlyWhenWebSocketsAreEnabled()
    {
        baseExpectations();

        AmqpsIotHubConnection connection = new AmqpsIotHubConnection(mockConfig, true);

        String actualHostName = Deencapsulation.getField(connection, "hostName");
        assertEquals(hostName + ":" + amqpWebSocketPort, actualHostName);
    }

    // Tests_SRS_AMQPSIOTHUBCONNECTION_15_007: [If the AMQPS connection is already open, the function shall do nothing.]
    @Test
    public void openDoesNothingIfTheConnectionIsAlreadyOpen() throws IOException
    {
        baseExpectations();

        AmqpsIotHubConnection connection = new AmqpsIotHubConnection(mockConfig, false);

        Deencapsulation.setField(connection, "state", State.OPEN);

        connection.open();

        new Verifications()
        {
            {
                new IotHubSasToken(mockConfig, anyLong);
                times = 0;
            }
        };
    }


    // Tests_SRS_AMQPSIOTHUBCONNECTION_15_008: [The function shall create a new sasToken valid for the duration
    // specified in config to be used for the communication with IoTHub.]
    @Test
    public void openCreatesSasToken() throws IOException
    {
        baseExpectations();

        new MockUp<AmqpsIotHubConnection>() {
            @Mock
            Future startReactorAsync()
            {
                return null;
            }

            @Mock
            void connectionReady()
            {
            }
        };

        AmqpsIotHubConnection connection = new AmqpsIotHubConnection(mockConfig, false);

        connection.open();

        new Verifications()
        {
            {
                new IotHubSasToken(mockConfig, anyLong);
                times = 1;
            }
        };
    }

    // Tests_SRS_AMQPSIOTHUBCONNECTION_15_009: [The function shall trigger the Reactor (Proton) to begin running.]
    @Test
    public void openTriggersProtonReactor() throws IOException
    {
        baseExpectations();

        new NonStrictExpectations() {
            {
                new IotHubReactor((Reactor) any);
                result = mockIotHubReactor;
                mockIotHubReactor.run();
            }
        };

        new MockUp<AmqpsIotHubConnection>() {
            @Mock
            void connectionReady()
            {
            }
        };

        AmqpsIotHubConnection connection = new AmqpsIotHubConnection(mockConfig, false);

        connection.open();

        new Verifications()
        {
            {
                new IotHubSasToken(mockConfig, anyLong);
                times = 1;
                new IotHubReactor((Reactor)any);
                times = 1;
            }
        };
    }

    // Tests_SRS_AMQPSIOTHUBCONNECTION_15_010: [The function shall wait for the reactor to be ready and for
    // enough link credit to become available.]
    @Test
    public void openWaitsForReactorToBeReadyAndForEnoughLinkCreditToBeAvailable() throws IOException
    {
        baseExpectations();

        final AmqpsIotHubConnection connection = new AmqpsIotHubConnection(mockConfig, false);

        new MockUp<AmqpsIotHubConnection>() {
            @Mock
            Future startReactorAsync()
            {
                Deencapsulation.setField(connection, "state", State.OPEN);
                Deencapsulation.setField(connection, "linkCredit", 1000);
                return null;
            }
        };

        connection.open();
    }

    // Tests_SRS_AMQPSIOTHUBCONNECTION_15_011: [If any exception is thrown while attempting to trigger
    // the reactor, the function shall close the connection and throw an IOException.]
    @Test(expected = IOException.class)
    public void openFailsIfConnectionIsNotOpenedInTime() throws IOException
    {
        baseExpectations();

        final AmqpsIotHubConnection connection = new AmqpsIotHubConnection(mockConfig, false);
        final Boolean[] closeCalled = { false };

        new MockUp<AmqpsIotHubConnection>() {
            @Mock
            Future startReactorAsync()
            {
                Deencapsulation.setField(connection, "linkCredit", 1000);
                return null;
            }

            @Mock
            void close()
            {
                closeCalled[0] = true;
            }
        };

        Deencapsulation.setField(connection, "maxWaitTimeForOpeningConnection", 100);

        try
        {
            connection.open();
        }
        catch(IOException e)
        {
            assertEquals(true, closeCalled[0]);
            throw e;
        }
    }

    // Tests_SRS_AMQPSIOTHUBCONNECTION_15_048 [If the AMQPS connection is already closed, the function shall do nothing.]
    @Test
    public void closeDoesNothingIfTheConnectionWasNeverOpened()
    {
        baseExpectations();

        final AmqpsIotHubConnection connection = new AmqpsIotHubConnection(mockConfig, false);

        connection.close();

        new Verifications()
        {
            {
                mockSender.close();
                times = 0;
                mockReceiver.close();
                times = 0;
                mockSession.close();
                times = 0;
                mockConnection.close();
                times = 0;
                mockReactorFuture.cancel(true);
                times = 0;
                mockExecutorService.shutdown();
                times = 0;
            }
        };
    }

    // Tests_SRS_AMQPSIOTHUBCONNECTION_15_012: [The function shall set the status of the AMQPS connection to CLOSED.]
    // Tests_SRS_AMQPSIOTHUBCONNECTION_15_013: [The function shall close the AMQPS sender and receiver links,
    // the AMQPS session and the AMQPS connection.]
    // Tests_SRS_AMQPSIOTHUBCONNECTION_15_014: [The function shall stop the Proton reactor.]
    @Test
    public void closeClosesAllProtonVariablesAndStopsProtonReactor() throws IOException
    {
        baseExpectations();

        new NonStrictExpectations()
        {
            {
                mockReactorFuture.cancel(true);
                mockExecutorService.shutdown();
            }
        };

        new MockUp<AmqpsIotHubConnection>() {
            @Mock
            void open()
            {
            }
        };

        final AmqpsIotHubConnection connection = new AmqpsIotHubConnection(mockConfig, false);

        Deencapsulation.setField(connection, "state", State.OPEN);
        Deencapsulation.setField(connection, "sender", mockSender);
        Deencapsulation.setField(connection, "receiver", mockReceiver);
        Deencapsulation.setField(connection, "session", mockSession);
        Deencapsulation.setField(connection, "connection", mockConnection);
        Deencapsulation.setField(connection, "reactorFuture", mockReactorFuture);
        Deencapsulation.setField(connection, "executorService", mockExecutorService);

        connection.close();

        State actualState = Deencapsulation.getField(connection, "state");
        assertEquals(State.CLOSED, actualState);

        new Verifications()
        {
            {
                mockSender.close();
                times = 1;
                mockReceiver.close();
                times = 1;
                mockSession.close();
                times = 1;
                mockConnection.close();
                times = 1;
                mockReactorFuture.cancel(true);
                times = 1;
                mockExecutorService.shutdown();
                times = 1;
            }
        };
    }

    // Tests_SRS_AMQPSIOTHUBCONNECTION_15_015: [If the state of the connection is CLOSED or there is not enough
    // credit, the function shall return -1.]
    @Test
    public void sendMessageDoesNothingIfConnectionIsClosed()
    {
        baseExpectations();

        final AmqpsIotHubConnection connection = new AmqpsIotHubConnection(mockConfig, false);

        Deencapsulation.setField(connection, "state", State.CLOSED);
        Deencapsulation.setField(connection, "linkCredit", 100);

        Integer expectedDeliveryHash = -1;
        Integer actualDeliveryHash = connection.sendMessage(Message.Factory.create());

        assertEquals(expectedDeliveryHash, actualDeliveryHash);
    }

    // Tests_SRS_AMQPSIOTHUBCONNECTION_15_015: [If the state of the connection is CLOSED or there is not enough
    // credit, the function shall return -1.]
    @Test
    public void sendMessageDoesNothingIfNotEnoughLinkCredit()
    {
        baseExpectations();

        final AmqpsIotHubConnection connection = new AmqpsIotHubConnection(mockConfig, false);

        Deencapsulation.setField(connection, "state", State.OPEN);
        Deencapsulation.setField(connection, "linkCredit", -1);

        Integer expectedDeliveryHash = -1;
        Integer actualDeliveryHash = connection.sendMessage(Message.Factory.create());

        assertEquals(expectedDeliveryHash, actualDeliveryHash);
    }

    // Tests_SRS_AMQPSIOTHUBCONNECTION_15_016: [The function shall encode the message and copy the contents to the byte buffer.]
    // Tests_SRS_AMQPSIOTHUBCONNECTION_15_017: [The function shall set the delivery tag for the sender.]
    // Tests_SRS_AMQPSIOTHUBCONNECTION_15_018: [The function shall attempt to send the message using the sender link.]
    // Tests_SRS_AMQPSIOTHUBCONNECTION_15_019: [The function shall advance the sender link.]
    // Tests_SRS_AMQPSIOTHUBCONNECTION_15_020: [The function shall set the delivery hash to the value returned by the sender link.]
    // Tests_SRS_AMQPSIOTHUBCONNECTION_15_021: [The function shall return the delivery hash.]
    @Test
    public void sendMessage() throws IOException
    {
        baseExpectations();

        new NonStrictExpectations()
        {
            {
                mockProtonMessage.encode((byte[]) any, anyInt, anyInt);
                mockSender.delivery((byte[]) any);
                result = mockDelivery;
            }
        };

        final AmqpsIotHubConnection connection = new AmqpsIotHubConnection(mockConfig, false);

        Deencapsulation.setField(connection, "state", State.OPEN);
        Deencapsulation.setField(connection, "linkCredit", 100);
        Deencapsulation.setField(connection, "sender", mockSender);

        Integer expectedDeliveryHash = mockDelivery.hashCode();
        Integer actualDeliveryHash = connection.sendMessage(mockProtonMessage);

        assertEquals(expectedDeliveryHash, actualDeliveryHash);

        new Verifications()
        {
            {
                mockProtonMessage.encode((byte[]) any, anyInt, anyInt);
                times = 1;
                mockSender.delivery((byte[]) any);
                times = 1;
                mockSender.send((byte[]) any, anyInt, anyInt);
                times = 1;
                mockSender.advance();
                times = 1;
                mockDelivery.hashCode();
                times = 1;
            }
        };
    }

    // Tests_SRS_AMQPSIOTHUBCONNECTION_15_022: [If the AMQPS Connection is closed, the function shall return false.]
    @Test
    public void sendMessageReturnsFalseIfConnectionIsClosed()
    {
        baseExpectations();

        final AmqpsIotHubConnection connection = new AmqpsIotHubConnection(mockConfig, false);

        Deencapsulation.setField(connection, "state", State.CLOSED);

        Boolean expectedResult = false;
        Boolean actualResult = connection.sendMessageResult(mockAmqpsMessage, IotHubMessageResult.COMPLETE);

        assertEquals(expectedResult, actualResult);
    }

    // Tests_SRS_AMQPSIOTHUBCONNECTION_15_023: [If the message result is COMPLETE, ABANDON, or REJECT,
    // the function shall acknowledge the last message with acknowledgement type COMPLETE, ABANDON, or REJECT respectively.]
    // Tests_SRS_AMQPSIOTHUBCONNECTION_15_024: [The function shall return true after the message was acknowledged.]
    @Test
    public void sendMessageAcknowledgesProperlyBasedOnMessageResult()
    {
        baseExpectations();

        new NonStrictExpectations()
        {
            {
                mockAmqpsMessage.acknowledge(AmqpsMessage.ACK_TYPE.COMPLETE);
            }
        };

        final AmqpsIotHubConnection connection = new AmqpsIotHubConnection(mockConfig, false);

        Deencapsulation.setField(connection, "state", State.OPEN);

        for(final AmqpsMessage.ACK_TYPE ackType : AmqpsMessage.ACK_TYPE.values())
        {
            Boolean expectedResult = true;
            Boolean actualResult = connection.sendMessageResult(mockAmqpsMessage, IotHubMessageResult.valueOf(ackType.toString()));

            assertEquals(expectedResult, actualResult);

            new Verifications()
            {
                {
                    mockAmqpsMessage.acknowledge(ackType);
                    times = 1;
                }
            };
        }
    }

    // Tests_SRS_AMQPSIOTHUBCONNECTION_15_025: [The event handler shall get the Connection (Proton) object from the event handler and set the host name on the connection.]
    // Tests_SRS_AMQPSIOTHUBCONNECTION_15_026: [The event handler shall create a Session (Proton) object from the connection.]
    // Tests_SRS_AMQPSIOTHUBCONNECTION_15_027: [The event handler shall create a Receiver and Sender (Proton) links and set the protocol tag on them to a predefined constant.]
    // Tests_SRS_AMQPSIOTHUBCONNECTION_15_028: [The Receiver and Sender links shall have the properties set to client version identifier.]
    // Tests_SRS_AMQPSIOTHUBCONNECTION_15_029: [The event handler shall open the connection, session, sender and receiver objects.]
    @Test
    public void onConnectionInit()
    {
        baseExpectations();

        new NonStrictExpectations()
        {
            {
                mockEvent.getConnection();
                result = mockConnection;
                mockConnection.session();
                result = mockSession;
                mockSession.receiver("receiver");
                result = mockReceiver;
                mockSession.sender("sender");
                result = mockSender;
                mockConnection.open();
                mockSession.open();
                mockReceiver.open();
                mockSender.open();
            }
        };

        final AmqpsIotHubConnection connection = new AmqpsIotHubConnection(mockConfig, false);

        connection.onConnectionInit(mockEvent);

        new Verifications()
        {
            {
                mockEvent.getConnection();
                times = 1;
                mockConnection.setHostname(hostName + ":" + amqpPort);
                times = 1;
                mockConnection.session();
                times = 1;
                mockSession.receiver("receiver");
                times = 1;
                mockSession.sender("sender");
                times = 1;
                mockReceiver.setProperties((Map<Symbol, Object>) any);
                times = 1;
                mockSender.setProperties((Map<Symbol, Object>) any);
                times = 1;
                mockConnection.open();
                times = 1;
                mockSession.open();
                times = 1;
                mockReceiver.open();
                times = 1;
                mockSender.open();
                times = 1;
            }
        };
    }

    // Tests_SRS_AMQPSIOTHUBCONNECTION_15_030: [The event handler shall get the Transport (Proton) object from the event.]
    // Tests_SRS_AMQPSIOTHUBCONNECTION_15_031: [The event handler shall set the SASL_PLAIN authentication on the transport using the given user name and sas token.]
    // Tests_SRS_AMQPSIOTHUBCONNECTION_15_032: [The event handler shall set VERIFY_PEER authentication mode on the domain of the Transport.]
    @Test
    public void onConnectionBoundNoWebSockets()
    {
        baseExpectations();

        new NonStrictExpectations()
        {
            {
                mockEvent.getConnection();
                result = mockConnection;
                mockConnection.getTransport();
                result = mockTransport;
                mockTransport.sasl();
                result = mockSasl;
                mockSasl.plain(anyString, anyString);
                mockSslDomain.setTrustedCaDb(mockCertPath);
                mockSslDomain.getTrustedCaDb();
                result = mockCertPath;
                mockSslDomain.setPeerAuthentication(SslDomain.VerifyMode.VERIFY_PEER);
                mockTransport.ssl(mockSslDomain);
            }
        };

        final AmqpsIotHubConnection connection = new AmqpsIotHubConnection(mockConfig, false);

        new MockUp<AmqpsIotHubConnection>() {
            @Mock
            String getPemFormat(String certPath)
            {
                return mockCertPath;
            }
        };

        connection.onConnectionBound(mockEvent);

        new Verifications()
        {
            {
                mockEvent.getConnection();
                times = 1;
                mockConnection.getTransport();
                times = 1;
                mockTransport.sasl();
                times = 1;
                mockSasl.plain(anyString, anyString);
                times = 1;
                mockSslDomain.setTrustedCaDb(mockCertPath);
                times = 1;
                mockSslDomain.setPeerAuthentication(SslDomain.VerifyMode.VERIFY_PEER);
                times = 1;
                mockTransport.ssl(mockSslDomain);
                times = 1;
            }
        };
    }

    // Tests_SRS_AMQPSIOTHUBCONNECTION_15_030: [The event handler shall get the Transport (Proton) object from the event.]
    // Tests_SRS_AMQPSIOTHUBCONNECTION_15_031: [The event handler shall set the SASL_PLAIN authentication on the transport using the given user name and sas token.]
    // Tests_SRS_AMQPSIOTHUBCONNECTION_15_032: [The event handler shall set VERIFY_PEER authentication mode on the domain of the Transport.]
    @Test
    public void onConnectionBoundWebSockets()
    {
        baseExpectations();

        new NonStrictExpectations()
        {
            {
                mockEvent.getConnection();
                result = mockConnection;
                mockConnection.getTransport();
                result = mockTransport;
                mockTransport.webSocket();
                result = mockWebSocket;
                mockWebSocket.configure(anyString, anyString, anyInt, anyString, (Map<String, String>) any, (WebSocketHandler) any);
                mockTransport.sasl();
                result = mockSasl;
                mockSasl.plain(anyString, anyString);
                mockSslDomain.setTrustedCaDb(mockCertPath);
                mockSslDomain.getTrustedCaDb();
                result = mockCertPath;
                mockSslDomain.setPeerAuthentication(SslDomain.VerifyMode.VERIFY_PEER);
                mockTransport.ssl(mockSslDomain);
            }
        };

        final AmqpsIotHubConnection connection = new AmqpsIotHubConnection(mockConfig, false);

        new MockUp<AmqpsIotHubConnection>() {
            @Mock
            String getPemFormat(String certPath)
            {
                return mockCertPath;
            }
        };

        Deencapsulation.setField(connection, "useWebSockets", true);

        connection.onConnectionBound(mockEvent);

        new Verifications()
        {
            {
                mockEvent.getConnection();
                times = 1;
                mockConnection.getTransport();
                times = 1;
                mockTransport.webSocket();
                times = 1;
                mockWebSocket.configure(hostName + ":" + amqpPort, "/$iothub/websocket", 0, "AMQPWSB10", null, null);
                times = 1;
                mockTransport.sasl();
                times = 1;
                mockSasl.plain(deviceId + "@sas." + hubName, anyString);
                times = 1;
                mockSslDomain.setTrustedCaDb(mockCertPath);
                times = 1;
                mockSslDomain.setPeerAuthentication(SslDomain.VerifyMode.VERIFY_PEER);
                times = 1;
                mockTransport.ssl(mockSslDomain);
                times = 1;
            }
        };
    }

    // Tests_SRS_AMQPSIOTHUBCONNECTION_15_033: [The event handler shall set the current handler to handle the connection events.]
    @Test
    public void onReactorInit()
    {
        baseExpectations();

        new NonStrictExpectations()
        {
            {
                mockEvent.getReactor();
                result = mockReactor;
                mockReactor.connection((Handler) any);
            }
        };

        final AmqpsIotHubConnection connection = new AmqpsIotHubConnection(mockConfig, false);

        connection.onReactorInit(mockEvent);

        new Verifications()
        {
            {
                mockEvent.getReactor();
                times = 1;
                mockReactor.connection(connection);
                times = 1;
            }
        };
    }

    // Tests_SRS_AMQPSIOTHUBCONNECTION_15_034: [If this link is the Receiver link, the event handler shall get the Receiver and Delivery (Proton) objects from the event.]
    // Tests_SRS_AMQPSIOTHUBCONNECTION_15_035: [The event handler shall read the received buffer.]
    // Tests_SRS_AMQPSIOTHUBCONNECTION_15_036: [The event handler shall create an AmqpsMessage object from the decoded buffer.]
    // Tests_SRS_AMQPSIOTHUBCONNECTION_15_037: [The event handler shall set the AmqpsMessage Deliver (Proton) object.]
    // Tests_SRS_AMQPSIOTHUBCONNECTION_15_049: [All the listeners shall be notified that a message was received from the server.]
    @Test
    public void onDeliveryReceive()
    {
        baseExpectations();

        new NonStrictExpectations()
        {
            {
                mockEvent.getLink();
                result = mockReceiver;
                mockReceiver.getName();
                result = "receiver";
                mockReceiver.current();
                result = mockDelivery;
                mockDelivery.isReadable();
                result = true;
                mockDelivery.isPartial();
                result = false;
                mockDelivery.pending();
                result = 10;
                mockReceiver.recv((byte[]) any, anyInt, anyInt);
                result = 10;
                mockReceiver.advance();
                new AmqpsMessage();
                result = mockAmqpsMessage;
                mockAmqpsMessage.setDelivery(mockDelivery);
                mockAmqpsMessage.decode((byte[]) any, anyInt, anyInt);
                mockServerListener.messageReceived(mockAmqpsMessage);
            }
        };

        final AmqpsIotHubConnection connection = new AmqpsIotHubConnection(mockConfig, false);
        connection.addListener(mockServerListener);
        connection.onDelivery(mockEvent);

        new Verifications()
        {
            {
                mockEvent.getLink();
                times = 2;
                mockReceiver.getName();
                times = 1;
                mockReceiver.current();
                times = 1;
                mockDelivery.isReadable();
                times = 1;
                mockDelivery.isPartial();
                times = 1;
                mockDelivery.pending();
                times = 1;
                mockReceiver.recv((byte[]) any, anyInt, anyInt);
                times = 1;
                mockReceiver.advance();
                times = 1;
                mockAmqpsMessage.setDelivery(mockDelivery);
                times = 1;
                mockAmqpsMessage.decode((byte[]) any, anyInt, anyInt);
                times = 1;
                mockServerListener.messageReceived(mockAmqpsMessage);
                times = 1;
            }
        };
    }

    // Tests_SRS_AMQPSIOTHUBCONNECTION_15_038: [If this link is the Sender link and the event type is DELIVERY, the event handler shall get the Delivery (Proton) object from the event.]
    // Tests_SRS_AMQPSIOTHUBCONNECTION_15_039: [The event handler shall note the remote delivery state and use it and the Delivery (Proton) hash code to inform the AmqpsIotHubConnection of the message receipt.]
    @Test
    public void onDeliverySend()
    {
        baseExpectations();

        new NonStrictExpectations()
        {
            {
                mockEvent.getLink();
                result = mockReceiver;
                mockReceiver.getName();
                result = "sender";
                mockEvent.getType();
                result = Event.Type.DELIVERY;
                mockEvent.getDelivery();
                result = mockDelivery;
                mockDelivery.getRemoteState();
                result = Accepted.getInstance();
                mockServerListener.messageSent(anyInt, true);
            }
        };

        final AmqpsIotHubConnection connection = new AmqpsIotHubConnection(mockConfig, false);
        connection.addListener(mockServerListener);
        connection.onDelivery(mockEvent);

        new Verifications()
        {
            {
                mockEvent.getLink();
                times = 1;
                mockReceiver.getName();
                times = 1;
                mockEvent.getType();
                times = 1;
                mockEvent.getDelivery();
                times = 1;
                mockDelivery.getRemoteState();
                times = 1;
                mockServerListener.messageSent(mockDelivery.hashCode(), true);
                times = 1;
            }
        };
    }

    // Tests_SRS_AMQPSIOTHUBCONNECTION_15_040: [The event handler shall save the remaining link credit.]
    @Test
    public void onLinkFlow()
    {
        baseExpectations();

        new NonStrictExpectations()
        {
            {
                mockEvent.getLink();
                result = mockSender;
                mockSender.getCredit();
                result = 100;
            }
        };

        final AmqpsIotHubConnection connection = new AmqpsIotHubConnection(mockConfig, false);
        connection.onLinkFlow(mockEvent);

        Integer expectedLinkCredit = 100;
        Integer actualLinkCredit = Deencapsulation.getField(connection, "linkCredit");

        assertEquals(expectedLinkCredit, actualLinkCredit);

        new Verifications()
        {
            {
                mockEvent.getLink();
                times = 1;
                mockSender.getCredit();
                times = 1;
            }
        };
    }

    // Tests_SRS_AMQPSIOTHUBCONNECTION_15_041: [The connection state shall be considered OPEN when the sender link is open remotely.]
    @Test
    public void onLinkRemoteOpen()
    {
        baseExpectations();

        new NonStrictExpectations()
        {
            {
                mockEvent.getLink();
                result = mockSender;
                mockSender.getName();
                result = "sender";
            }
        };

        final AmqpsIotHubConnection connection = new AmqpsIotHubConnection(mockConfig, false);
        connection.onLinkRemoteOpen(mockEvent);

        State expectedState = State.OPEN;
        State actualState = Deencapsulation.getField(connection, "state");

        assertEquals(expectedState, actualState);

        new Verifications()
        {
            {
                mockEvent.getLink();
                times = 1;
                mockSender.getName();
                times = 1;
            }
        };
    }

    // Tests_SRS_AMQPSIOTHUBCONNECTION_15_042 [The event handler shall attempt to reconnect to the IoTHub.]
    @Test
    public void onLinkRemoteClose()
    {
        baseExpectations();

        new NonStrictExpectations()
        {
            {
                mockEvent.getLink();
                result = mockSender;
                mockSender.getName();
                result = "sender";
                mockServerListener.connectionLost();
            }
        };

        final AmqpsIotHubConnection connection = new AmqpsIotHubConnection(mockConfig, false);

        final Boolean[] openCalled = { false };
        final Boolean[] closeCalled = { false };

        new MockUp<AmqpsIotHubConnection>()
        {
            @Mock
            void open()
            {
                openCalled[0] = true;
                Deencapsulation.setField(connection, "state", State.OPEN);
            }

            @Mock
            void close()
            {
                closeCalled[0] = true;
                Deencapsulation.setField(connection, "state", State.CLOSED);
            }
        };

        connection.addListener(mockServerListener);
        connection.onLinkRemoteClose(mockEvent);

        assertEquals(true, closeCalled[0]);
        assertEquals(true, openCalled[0]);

        new Verifications()
        {
            {
                mockEvent.getLink();
                times = 1;
                mockSender.getName();
                times = 1;
                mockServerListener.connectionLost();
                times = 1;
            }
        };
    }

    // Tests_SRS_AMQPSIOTHUBCONNECTION_15_043: [If the link is the Sender link, the event handler shall create a new Target (Proton) object using the sender endpoint address member variable.]
    // Tests_SRS_AMQPSIOTHUBCONNECTION_15_044: [If the link is the Sender link, the event handler shall set its target to the created Target (Proton) object.]
    // Tests_SRS_AMQPSIOTHUBCONNECTION_14_045: [If the link is the Sender link, the event handler shall set the SenderSettleMode to UNSETTLED.]
    @Test
    public void onLinkInitSend()
    {
        baseExpectations();

        new NonStrictExpectations()
        {
            {
                mockEvent.getLink();
                result = mockSender;
                mockSender.getName();
                result = "sender";
                new Target();
                result = mockTarget;
                mockTarget.setAddress(anyString);
                mockSender.setTarget(mockTarget);
                mockSender.setSenderSettleMode(SenderSettleMode.UNSETTLED);
            }
        };

        final AmqpsIotHubConnection connection = new AmqpsIotHubConnection(mockConfig, false);

        connection.onLinkInit(mockEvent);

        new Verifications()
        {
            {
                mockEvent.getLink();
                times = 1;
                mockSender.getName();
                times = 1;
                new Target();
                times = 1;
                mockTarget.setAddress(anyString);
                times = 1;
                mockSender.setTarget((org.apache.qpid.proton.amqp.transport.Target) any);
                times = 1;
                mockSender.setSenderSettleMode(SenderSettleMode.UNSETTLED);
                times = 1;
            }
        };
    }

    // Tests_SRS_AMQPSIOTHUBCONNECTION_14_046: [If the link is the Receiver link, the event handler shall create a new Source (Proton) object using the receiver endpoint address member variable.]
    // Tests_SRS_AMQPSIOTHUBCONNECTION_14_047: [If the link is the Receiver link, the event handler shall set its source to the created Source (Proton) object.]
    @Test
    public void onLinkInitReceive()
    {
        baseExpectations();

        new NonStrictExpectations()
        {
            {
                mockEvent.getLink();
                result = mockReceiver;
                mockReceiver.getName();
                result = "receiver";
                new Source();
                result = mockSource;
                mockSource.setAddress(anyString);
                mockReceiver.setSource(mockSource);
            }
        };

        final AmqpsIotHubConnection connection = new AmqpsIotHubConnection(mockConfig, false);

        connection.onLinkInit(mockEvent);

        new Verifications()
        {
            {
                mockEvent.getLink();
                times = 1;
                mockReceiver.getName();
                times = 1;
                new Source();
                times = 1;
                mockSource.setAddress(anyString);
                times = 1;
                mockReceiver.setSource((org.apache.qpid.proton.amqp.transport.Source) any);
                times = 1;
            }
        };
    }

    // Tests_SRS_AMQPSIOTHUBCONNECTION_15_048: [The event handler shall attempt to reconnect to IoTHub.]
    @Test
    public void onTransportError()
    {
        baseExpectations();

        new NonStrictExpectations()
        {
            {
                mockServerListener.connectionLost();
            }
        };

        final AmqpsIotHubConnection connection = new AmqpsIotHubConnection(mockConfig, false);

        final Boolean[] openCalled = { false };
        final Boolean[] closeCalled = { false };

        new MockUp<AmqpsIotHubConnection>()
        {
            @Mock
            void open()
            {
                openCalled[0] = true;
                Deencapsulation.setField(connection, "state", State.OPEN);
            }

            @Mock
            void close()
            {
                closeCalled[0] = true;
                Deencapsulation.setField(connection, "state", State.CLOSED);
            }
        };

        connection.addListener(mockServerListener);
        connection.onTransportError(mockEvent);

        assertEquals(true, closeCalled[0]);
        assertEquals(true, openCalled[0]);

        new Verifications()
        {
            {
                mockServerListener.connectionLost();
                times = 1;
            }
        };
    }

    private void baseExpectations()
    {
        new NonStrictExpectations() {
            {
                mockConfig.getIotHubHostname();
                result = hostName;
                mockConfig.getIotHubName();
                result = hubName;
                mockConfig.getDeviceId();
                result = deviceId;
                mockConfig.getDeviceKey();
                result = deviceKey;
            }
        };
    }
}