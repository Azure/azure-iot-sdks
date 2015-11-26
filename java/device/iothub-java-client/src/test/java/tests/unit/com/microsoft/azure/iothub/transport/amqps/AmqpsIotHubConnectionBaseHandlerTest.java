/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package tests.unit.com.microsoft.azure.iothub.transport.amqps;

import com.microsoft.azure.iothub.transport.amqps.AmqpsIotHubConnection;
import com.microsoft.azure.iothub.transport.amqps.AmqpsIotHubConnectionBaseHandler;
import com.microsoft.azure.iothub.transport.amqps.AmqpsMessage;
import mockit.Deencapsulation;
import mockit.Mocked;
import mockit.NonStrictExpectations;
import mockit.Verifications;
import org.apache.qpid.proton.Proton;
import org.apache.qpid.proton.amqp.Binary;
import org.apache.qpid.proton.amqp.messaging.*;
import org.apache.qpid.proton.amqp.transport.ErrorCondition;
import org.apache.qpid.proton.amqp.transport.SenderSettleMode;
import org.apache.qpid.proton.codec.transport.ErrorConditionType;
import org.apache.qpid.proton.engine.*;
import org.apache.qpid.proton.message.Message;
import org.apache.qpid.proton.messenger.impl.Address;
import org.apache.qpid.proton.reactor.FlowController;
import org.apache.qpid.proton.reactor.Handshaker;
import org.junit.Test;

import java.util.concurrent.CompletableFuture;
import java.util.concurrent.ExecutionException;

import static org.junit.Assert.assertEquals;

public class AmqpsIotHubConnectionBaseHandlerTest {

    public final int PORT = 5671;

    @Mocked
    protected Handshaker mockHandshaker;
    @Mocked
    protected FlowController mockFlowController;
    @Mocked
    protected Proton mockProton;
    @Mocked
    protected Message mockMessage;
    @Mocked
    protected AmqpsMessage mockAmqpsMessage;
    @Mocked
    protected Properties mockProperties;
    @Mocked
    protected Binary mockBinary;
    @Mocked
    protected Section mockSection;
    @Mocked
    protected Data mockData;
    @Mocked
    protected Event mockEvent;
    @Mocked
    protected Transport mockTransport;
    @Mocked
    protected Connection mockConnection;
    @Mocked
    protected Address mockAddress;
    @Mocked
    protected Sasl mockSasl;
    @Mocked
    protected SslDomain mockSslDomain;
    @Mocked
    protected Session mockSession;
    @Mocked
    protected Sender mockSender;
    @Mocked
    protected Receiver mockReceiver;
    @Mocked
    protected Link mockLink;
    @Mocked
    protected Target mockTarget;
    @Mocked
    protected Delivery mockDelivery;
    @Mocked
    protected Source mockSource;
    @Mocked
    protected AmqpsIotHubConnection mockIotHubConnection;

    // Tests_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_002: [The constructor shall copy all input parameters to private member variables for event processing.]
    // Tests_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_003: [The constructor shall concatenate the host name with the port.]
    // Tests_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_004: [The constructor shall initialize the sender and receiver endpoint private member variables using the ENDPOINT_FORMAT constants and deviceID.]
    // Tests_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_005: [The constructor shall initialize a new Handshaker (Proton) object to handle communication handshake.]
    // Tests_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_006: [The constructor shall initialize a new FlowController (Proton) object to handle communication flow.]
    @Test
    public void constructorCopiesAllData(){
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";

        final AmqpsIotHubConnectionBaseHandler handler = new AmqpsIotHubConnectionBaseHandler(hostName, userName, sasToken, deviceId, mockIotHubConnection);

        final String expectedSendEndpoint = "/devices/test-deviceId/messages/events";
        final String expectedReceiveEndpoint = "/devices/test-deviceId/messages/devicebound";

        String actualHostName = Deencapsulation.getField(handler, "hostName");
        String actualDeviceId = Deencapsulation.getField(handler, "deviceID");
        String actualUserName = Deencapsulation.getField(handler, "userName");
        String actualSasToken = Deencapsulation.getField(handler, "sasToken");
        String actualSendEndpoint = Deencapsulation.getField(handler, "sendEndpoint");
        String actualReceiveEndpoint = Deencapsulation.getField(handler, "receiveEndpoint");

        assertEquals(String.format("%s:%d", hostName, PORT), actualHostName);
        assertEquals(userName, actualUserName);
        assertEquals(deviceId, actualDeviceId);
        assertEquals(sasToken, actualSasToken);
        assertEquals(expectedSendEndpoint, actualSendEndpoint);
        assertEquals(expectedReceiveEndpoint, actualReceiveEndpoint);

        new Verifications()
        {
            {
                mockHandshaker = new Handshaker();
                mockFlowController = new FlowController();
            }
        };
    }

    // Tests_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_001: [The constructor shall throw a new IllegalArgumentException if any of the input parameters is null or empty.]
    @Test(expected = IllegalArgumentException.class)
    public void constructorThrowsIllegalArgumentExceptionIfHostNameIsEmpty(){
        final String hostName = "";
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";

        AmqpsIotHubConnectionBaseHandler handler = new AmqpsIotHubConnectionBaseHandler(hostName, userName, sasToken, deviceId, mockIotHubConnection);
    }

    // Tests_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_001: [The constructor shall throw a new IllegalArgumentException if any of the input parameters is null or empty.]
    @Test(expected = IllegalArgumentException.class)
    public void constructorThrowsIllegalArgumentExceptionIfHostNameIsNull(){
        final String hostName = null;
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";

        AmqpsIotHubConnectionBaseHandler handler = new AmqpsIotHubConnectionBaseHandler(hostName, userName, sasToken, deviceId, mockIotHubConnection);
    }

    // Tests_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_001: [The constructor shall throw a new IllegalArgumentException if any of the input parameters is null or empty.]
    @Test(expected = IllegalArgumentException.class)
    public void constructorThrowsIllegalArgumentExceptionIfDeviceIDIsEmpty(){
        final String hostName = "test.host.name";
        final String deviceId = "";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";

        AmqpsIotHubConnectionBaseHandler handler = new AmqpsIotHubConnectionBaseHandler(hostName, userName, sasToken, deviceId, mockIotHubConnection);
    }

    // Tests_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_001: [The constructor shall throw a new IllegalArgumentException if any of the input parameters is null or empty.]
    @Test(expected = IllegalArgumentException.class)
    public void constructorThrowsIllegalArgumentExceptionIfDeviceIDIsNull(){
        final String hostName = "test.host.name";
        final String deviceId = null;
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";

        AmqpsIotHubConnectionBaseHandler handler = new AmqpsIotHubConnectionBaseHandler(hostName, userName, sasToken, deviceId, mockIotHubConnection);
    }

    // Tests_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_001: [The constructor shall throw a new IllegalArgumentException if any of the input parameters is null or empty.]
    @Test(expected = IllegalArgumentException.class)
    public void constructorThrowsIllegalArgumentExceptionIfUserNameIsEmpty(){
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = "";
        final String sasToken = "test-token";

        AmqpsIotHubConnectionBaseHandler handler = new AmqpsIotHubConnectionBaseHandler(hostName, userName, sasToken, deviceId, mockIotHubConnection);
    }

    // Tests_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_001: [The constructor shall throw a new IllegalArgumentException if any of the input parameters is null or empty.]
    @Test(expected = IllegalArgumentException.class)
    public void constructorThrowsIllegalArgumentExceptionIfUserNameIsNull(){
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = null;
        final String sasToken = "test-token";

        AmqpsIotHubConnectionBaseHandler handler = new AmqpsIotHubConnectionBaseHandler(hostName, userName, sasToken, deviceId, mockIotHubConnection);
    }

    // Tests_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_001: [The constructor shall throw a new IllegalArgumentException if any of the input parameters is null or empty.]
    @Test(expected = IllegalArgumentException.class)
    public void constructorThrowsIllegalArgumentExceptionIfSasTokenIsEmpty(){
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "";

        AmqpsIotHubConnectionBaseHandler handler = new AmqpsIotHubConnectionBaseHandler(hostName, userName, sasToken, deviceId, mockIotHubConnection);
    }

    // Tests_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_001: [The constructor shall throw a new IllegalArgumentException if any of the input parameters is null or empty.]
    @Test(expected = IllegalArgumentException.class)
    public void constructorThrowsIllegalArgumentExceptionIfSasTokenIsNull(){
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = null;

        AmqpsIotHubConnectionBaseHandler handler = new AmqpsIotHubConnectionBaseHandler(hostName, userName, sasToken, deviceId, mockIotHubConnection);
    }

    // Tests_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_007: [If this link is the Receiver link, the event handler shall get the Receiver and Delivery (Proton) objects from the event.]
    @Test
    public void onDeliveryGetsReceiverAndDelivery(){
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";
        final String receiveTag = "receiver";

        new NonStrictExpectations()
        {
            {
                mockEvent.getLink();
                result = mockReceiver;
                mockReceiver.getName();
                result = receiveTag;
                mockReceiver.current();
                result = mockDelivery;
            }
        };

        AmqpsIotHubConnectionBaseHandler handler = new AmqpsIotHubConnectionBaseHandler(hostName, userName, sasToken, deviceId, mockIotHubConnection);

        handler.onDelivery(mockEvent);

        new Verifications()
        {
            {
                mockEvent.getLink();
                mockReceiver.current();
            }
        };
    }

    // Tests_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_008: [The event handler shall read the received buffer.]
    @Test
    public void onDeliveryReadsReceivedBuffer(){
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";
        final String receiveTag = "receiver";
        final int pendingDeliverySize = 10;

        new NonStrictExpectations()
        {
            {
                mockEvent.getLink();
                result = mockReceiver;
                mockReceiver.getName();
                result = receiveTag;
                mockReceiver.current();
                result = mockDelivery;
                mockDelivery.isReadable();
                result = true;
                mockDelivery.isPartial();
                result = false;
                mockDelivery.pending();
                result = pendingDeliverySize;
            }
        };

        AmqpsIotHubConnectionBaseHandler handler = new AmqpsIotHubConnectionBaseHandler(hostName, userName, sasToken, deviceId, mockIotHubConnection);

        handler.onDelivery(mockEvent);

        new Verifications()
        {
            {
                mockDelivery.pending();
                byte[] buffer = new byte[pendingDeliverySize];
                mockReceiver.recv(buffer, 0, pendingDeliverySize);
                mockReceiver.advance();
            }
        };
    }

    // Tests_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_009: [The event handler shall create an AmqpsMessage object from the decoded buffer.]
    // Tests_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_010: [The event handler shall set the AmqpsMessage Deliver (Proton) object.]
    @Test
    public void onDeliveryCreatesAndSetsAmqpsMessage(){
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";
        final String receiveTag = "receiver";
        final int pendingDeliverySize = 10;
        final byte[] buffer = new byte[pendingDeliverySize];

        new NonStrictExpectations()
        {
            {
                mockEvent.getLink();
                result = mockReceiver;
                mockReceiver.getName();
                result = receiveTag;
                mockReceiver.current();
                result = mockDelivery;
                mockDelivery.isReadable();
                result = true;
                mockDelivery.isPartial();
                result = false;
                mockDelivery.pending();
                result = pendingDeliverySize;
                new AmqpsMessage();
                result = mockAmqpsMessage;
            }
        };

        AmqpsIotHubConnectionBaseHandler handler = new AmqpsIotHubConnectionBaseHandler(hostName, userName, sasToken, deviceId, mockIotHubConnection);

        handler.onDelivery(mockEvent);

        new Verifications()
        {
            {
                new AmqpsMessage();
                mockAmqpsMessage.setDelivery(mockDelivery);
                mockAmqpsMessage.decode(buffer, 0, anyInt);
            }
        };
    }

    // Tests_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_011: [The event handler shall give the message back to its parent AmqpsIotHubConnection.]
    @Test
    public void onDeliveryGivesMessageToParentAmqpsReceiver(){
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";
        final String receiveTag = "receiver";
        final int pendingDeliverySize = 10;
        final byte[] buffer = new byte[pendingDeliverySize];

        new NonStrictExpectations()
        {
            {
                mockEvent.getLink();
                result = mockReceiver;
                mockReceiver.getName();
                result = receiveTag;
                mockReceiver.current();
                result = mockDelivery;
                mockDelivery.isReadable();
                result = true;
                mockDelivery.isPartial();
                result = false;
                new AmqpsMessage();
                result = mockAmqpsMessage;
            }
        };

        AmqpsIotHubConnectionBaseHandler handler = new AmqpsIotHubConnectionBaseHandler(hostName, userName, sasToken, deviceId, mockIotHubConnection);

        handler.onDelivery(mockEvent);

        new Verifications()
        {
            {
                Deencapsulation.invoke(mockIotHubConnection, "addMessage", mockAmqpsMessage);
            }
        };
    }

    // Tests_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_012: [If this link is not the Receiver link and the event type is DELIVERY, the event handler shall get the Delivery (Proton) object from the event.]
    @Test
    public void onDeliveryAckGetsDelivery(){
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";
        final String sendTag = "sender";

        new NonStrictExpectations()
        {
            {
                mockEvent.getLink();
                result = mockSender;
                mockSender.getName();
                result = sendTag;
                mockEvent.getType();
                result = Event.Type.DELIVERY;
            }
        };

        AmqpsIotHubConnectionBaseHandler handler = new AmqpsIotHubConnectionBaseHandler(hostName, userName, sasToken, deviceId, mockIotHubConnection);

        Deencapsulation.setField(handler, "sender", mockSender);

        handler.onDelivery(mockEvent);

        new Verifications()
        {
            {
                mockEvent.getDelivery();
            }
        };
    }

    // Tests_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_013: [If the event type is DELIVERY, the event handler shall note the remote delivery state and use it and the Delivery (Proton) hash code to inform the AmqpsIotHubConnection of the message receipt.]
    @Test
    public void onDeliveryAckToConnection(){
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";
        final String sendTag = "sender";
        final int hash = 12345;

        new NonStrictExpectations()
        {
            {
                mockEvent.getLink();
                result = mockSender;
                mockSender.getName();
                result = sendTag;
                mockEvent.getType();
                result = Event.Type.DELIVERY;
                mockEvent.getDelivery();
                result = mockDelivery;
                mockDelivery.getRemoteState();
                result = Accepted.getInstance();
            }
        };

        AmqpsIotHubConnectionBaseHandler handler = new AmqpsIotHubConnectionBaseHandler(hostName, userName, sasToken, deviceId, mockIotHubConnection);

        Deencapsulation.setField(handler, "sender", mockSender);

        handler.onDelivery(mockEvent);

        new Verifications()
        {
            {
                mockEvent.getDelivery();
                mockDelivery.getRemoteState();
                Deencapsulation.invoke(mockIotHubConnection, "acknowledge", mockDelivery.hashCode(), true);
            }
        };
    }

    // Tests_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_014: [If the event type is DELIVERY, the event handler shall close the Sender link.]
    @Test
    public void onDeliveryClosesSender(){
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";
        final String sendTag = "sender";

        new NonStrictExpectations()
        {
            {
                mockEvent.getLink();
                result = mockSender;
                mockSender.getName();
                result = sendTag;
                mockEvent.getType();
                result = Event.Type.DELIVERY;
                mockEvent.getDelivery();
                result = mockDelivery;
            }
        };

        AmqpsIotHubConnectionBaseHandler handler = new AmqpsIotHubConnectionBaseHandler(hostName, userName, sasToken, deviceId, mockIotHubConnection);

        Deencapsulation.setField(handler, "sender", mockSender);

        handler.onDelivery(mockEvent);

        new Verifications()
        {
            {
                mockSender.close();
            }
        };
    }

    // Tests_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_015: [If this link is the Receiver link, the event handler shall close the Session and Connection (Proton) objects.]
    @Test
    public void onLinkLocalCloseClosesSessionAndConnection(
            @Mocked final CompletableFuture<Integer> mockFuture)
    {
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";
        final String receiveTag = "receiver";

        new NonStrictExpectations()
        {
            {
                mockEvent.getLink();
                result = mockReceiver;
                mockReceiver.getName();
                result = receiveTag;
                mockFuture.isDone();
                result = true;
            }
        };

        AmqpsIotHubConnectionBaseHandler handler = new AmqpsIotHubConnectionBaseHandler(hostName, userName, sasToken, deviceId, mockIotHubConnection);

        Deencapsulation.setField(handler, "currentSentMessageFuture", mockFuture);
        handler.onLinkLocalClose(mockEvent);

        new Verifications()
        {
            {
                mockEvent.getSession().close();
                mockEvent.getSession().getConnection().close();
            }
        };
    }

    // Tests_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_016: [If the link was locally closed before the current message is sent, the sent message CompletableFuture will be completed exceptionally with a new HandlerException.]
    @Test
    public void onLinkLocalCloseClosesBeforeSendCompletesExceptionally(
            @Mocked final CompletableFuture<Integer> mockFuture)
    {
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";
        final String receiveTag = "receiver";

        new NonStrictExpectations()
        {
            {
                mockEvent.getLink();
                result = mockReceiver;
                mockReceiver.getName();
                result = receiveTag;
                mockFuture.isDone();
                result = false;
            }
        };

        AmqpsIotHubConnectionBaseHandler handler = new AmqpsIotHubConnectionBaseHandler(hostName, userName, sasToken, deviceId, mockIotHubConnection);

        Deencapsulation.setField(handler, "currentSentMessageFuture", mockFuture);
        handler.onLinkLocalClose(mockEvent);

        new Verifications()
        {
            {
                mockFuture.completeExceptionally((HandlerException) any);
            }
        };
    }

    // Tests_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_017: [The event handler shall get the Sender (Proton) object from the link.]
    // Tests_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_018: [The event handler shall encode the message and copy the contents to the byte buffer.]
    // Tests_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_019: [The event handler shall set the delivery tag on the Sender (Proton) object.]
    // Tests_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_020: [The event handler shall send the encoded bytes.]
    // Tests_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_021: [The event handler shall advance the Sender and complete the sent message CompletableFuture using the Delivery (Proton) object hash code.]
    @Test
    public void onLinkFlowFullTest(
            @Mocked final CompletableFuture<Integer> mockFuture)
    {
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";
        final byte[] msgBody = { 0x61, 0x62, 0x63 };
        final int tag = 0;

        new NonStrictExpectations()
        {
            {
                mockMessage.encode(msgBody, 0, msgBody.length);
                result = 3;
                mockEvent.getLink();
                result = mockSender;
                mockSender.getCredit();
                result = 1;
                mockSender.getUnsettled();
                result = 0;
            }
        };

        AmqpsIotHubConnectionBaseHandler handler = new AmqpsIotHubConnectionBaseHandler(hostName, userName, sasToken, deviceId, mockIotHubConnection);

        Deencapsulation.setField(handler, "sender", mockSender);
        Deencapsulation.setField(handler, "currentSentMessageFuture", mockFuture);
        handler.createBinaryMessage(msgBody, null);
        handler.onLinkFlow(mockEvent);

        new Verifications()
        {
            {
                mockEvent.getLink();
                byte[] buffer = new byte[1024];
                int length = mockMessage.encode(buffer, 0, buffer.length);
                byte[] deliveryTag = String.valueOf(tag).getBytes();
                Delivery d = mockSender.delivery(deliveryTag);
                mockSender.send((byte[]) any, 0, length);

                mockSender.advance();
                mockFuture.complete(d.hashCode());
            }
        };
    }

    // Tests_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_022: [If the link is the Sender link and the message was sent successfully over this link, the event handler shall reset the private Sender (Proton) member variable and open the AmqpsIotHubConnection for sending.]
    @Test
    public void onLinkRemoteCloseResetsSenderAndOpens(
            @Mocked final CompletableFuture<Integer> mockFuture)
    {
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";
        final String sendTag = "sender";

        new NonStrictExpectations()
        {
            {
                mockEvent.getLink();
                result = mockSender;
                mockSender.getName();
                result = sendTag;
                mockFuture.isCompletedExceptionally();
                result = false;
            }
        };

        AmqpsIotHubConnectionBaseHandler handler = new AmqpsIotHubConnectionBaseHandler(hostName, userName, sasToken, deviceId, mockIotHubConnection);
        Deencapsulation.setField(handler, "currentSentMessageFuture", mockFuture);
        handler.onLinkRemoteClose(mockEvent);

        new Verifications()
        {
            {
                mockEvent.getSession().sender(sendTag);
                Deencapsulation.invoke(mockIotHubConnection, "openSending");
            }
        };
    }

    // Tests_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_023: [The event handler shall get the Transport (Proton) object from the event.]
    @Test
    public void onConnectionBoundGetsTransport(){
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";

        AmqpsIotHubConnectionBaseHandler handler = new AmqpsIotHubConnectionBaseHandler(hostName, userName, sasToken, deviceId, mockIotHubConnection);
        handler.onConnectionBound(mockEvent);

        new Verifications()
        {
            {
                mockConnection = mockEvent.getConnection();
                mockConnection.getTransport();
            }
        };
    }

    // Tests_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_024: [The event handler shall set the SASL_PLAIN authentication on the transport using the given user name and sas token.]
    @Test
    public void onConnectionBoundSetsProperAuthenticationUsingUserNameAndSasToken(){
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";

        AmqpsIotHubConnectionBaseHandler handler = new AmqpsIotHubConnectionBaseHandler(hostName, userName, sasToken, deviceId, mockIotHubConnection);
        handler.onConnectionBound(mockEvent);

        new Verifications()
        {
            {
                mockSasl = mockTransport.sasl();
                mockSasl.plain(userName, sasToken);
            }
        };
    }

    // Tests_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_025: [The event handler shall set ANONYMOUS_PEER authentication mode on the domain of the Transport.]
    @Test
    public void onConnectionBoundSetsAuthenticationModeOnDomain(){
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";

        AmqpsIotHubConnectionBaseHandler handler = new AmqpsIotHubConnectionBaseHandler(hostName, userName, sasToken, deviceId, mockIotHubConnection);
        handler.onConnectionBound(mockEvent);

        new Verifications()
        {
            {
                mockSslDomain.setPeerAuthentication(SslDomain.VerifyMode.ANONYMOUS_PEER);
                mockTransport.ssl(mockSslDomain);
            }
        };
    }

    // Tests_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_026: [The event handler shall get the Connection (Proton) object from the event handler and set the host name on the connection.]
    // Tests_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_027: [The event handler shall create a Session (Proton) object from the connection.]
    // Tests_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_028: [The event handler shall create a Receiver and Sender (Proton) object and set the protocol tag on them to a predefined constant.]
    // Tests_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_029: [The event handler shall open the Connection, Session, and Receiver objects.]
    @Test
    public void onConnectionInitCreatesSession(){
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";
        final String tag = "receiver";

        AmqpsIotHubConnectionBaseHandler handler = new AmqpsIotHubConnectionBaseHandler(hostName, userName, sasToken, deviceId, mockIotHubConnection);
        handler.onConnectionInit(mockEvent);

        final String expectedHostName = String.format("%s:%d",hostName,PORT);

        new Verifications()
        {
            {
                mockEvent.getConnection();
                mockConnection.setHostname(expectedHostName);
                mockConnection.session();
                mockSession.receiver(tag);
                mockConnection.open();
                mockSession.open();
                mockReceiver.open();
            }
        };
    }

    // Tests_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_037: [If the Connection and Session (Proton) are not opened before a constant number of seconds, this handler will inform the AmqpsIotHubConnection that the Connection was not opened in time.]
    @Test(expected = ExecutionException.class)
    public void onConnectionLocalOpenTimesOut() throws ExecutionException, InterruptedException {
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";

        AmqpsIotHubConnectionBaseHandler handler = new AmqpsIotHubConnectionBaseHandler(hostName, userName, sasToken, deviceId, mockIotHubConnection);

        Deencapsulation.setField(mockIotHubConnection, "reactorReady", new CompletableFuture<Boolean>());
        handler.onConnectionLocalOpen(mockEvent);
        CompletableFuture<Boolean> future = Deencapsulation.getField(mockIotHubConnection, "reactorReady");

        future.get();
    }

    // Tests_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_035: [If the Connection was remotely closed abnormally, the event handler shall complete the sent message CompletableFuture with a new HandlerException.]
    @Test
    public void onConnectionRemoteCloseClosesAbnormallyCompletesMessageExceptionally(
            @Mocked final CompletableFuture<Integer> mockFuture)
    {
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";

        new NonStrictExpectations()
        {
            {
                mockEvent.getConnection();
                result = mockConnection;
                mockConnection.getCondition();
                result = new ErrorCondition();
            }
        };

        AmqpsIotHubConnectionBaseHandler handler = new AmqpsIotHubConnectionBaseHandler(hostName, userName, sasToken, deviceId, mockIotHubConnection);
        Deencapsulation.setField(handler, "currentSentMessageFuture", mockFuture);
        handler.onConnectionRemoteClose(mockEvent);

        new Verifications()
        {
            {
                mockFuture.completeExceptionally((HandlerException)any);
            }
        };
    }

    // Tests_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_036: [The event handler shall inform the AmqpsIotHubConnection that the Reactor (Proton) is ready.]
    @Test
    public void onSessionRemoteOpenInformsReactorReady(){
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";
        final CompletableFuture<Boolean> future = new CompletableFuture<>();

        AmqpsIotHubConnectionBaseHandler handler = new AmqpsIotHubConnectionBaseHandler(hostName, userName, sasToken, deviceId, mockIotHubConnection);

        Deencapsulation.setField(mockIotHubConnection, "reactorReady", future);
        handler.onSessionRemoteOpen(mockEvent);

        new Verifications()
        {
            {
                future.complete(new Boolean(true));
            }
        };
    }

    // Tests_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_030: [If the link is the Receiver link, the event handler shall create a new Source (Proton) object using the receiver endpoint address member variable.]
    // Tests_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_032: [If the link is the Receiver link, the event handler shall get the Link (Proton) object and set its source to the created Source (Proton) object.]
    @Test
    public void onLinkInitCreatesSourceAndSetsOnLinkForReceiver(){
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";
        final String receiveTag = "receiver";

        new NonStrictExpectations()
        {
            {
                mockEvent.getLink();
                result = mockReceiver;
                mockReceiver.getName();
                result = receiveTag;
                new Source();
                result = mockSource;
            }
        };

        AmqpsIotHubConnectionBaseHandler handler = new AmqpsIotHubConnectionBaseHandler(hostName, userName, sasToken, deviceId, mockIotHubConnection);
        handler.onLinkInit(mockEvent);

        final String expectedEndpoint = "/devices/test-deviceId/messages/devicebound";

        new Verifications()
        {
            {
                mockSource.setAddress(expectedEndpoint);
                mockReceiver.setSource(mockSource);
            }
        };
    }

    // Tests_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_031: [If the link is the Sender link, the event handler shall create a new Target (Proton) object using the sender endpoint address member variable.]
    // Tests_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_033: [If the link is the Sender link, the event handler shall get the Link (Proton) object and set its target.]
    // Tests_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_034: [If the link is the Sender link, the event handler shall set the SenderSettleMode to UNSETTLED.]
    @Test
    public void onLinkInitCreatesTargetAndSetsOnLinkForSender(){
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";
        final String sendTag = "sender";

        new NonStrictExpectations() {
            {
                mockEvent.getLink();
                result = mockSender;
                mockSender.getName();
                result = sendTag;
                new Target();
                result = mockTarget;
            }
        };

        AmqpsIotHubConnectionBaseHandler handler = new AmqpsIotHubConnectionBaseHandler(hostName, userName, sasToken, deviceId, mockIotHubConnection);
        handler.onLinkInit(mockEvent);

        final String expectedEndpoint = "/devices/test-deviceId/messages/events";

        new Verifications()
        {
            {
                mockSource.setAddress(expectedEndpoint);
                mockSender.setTarget(mockTarget);
                mockSender.setSenderSettleMode(SenderSettleMode.UNSETTLED);
            }
        };
    }

    // Tests_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_038: [The event handler shall log the error and description if there is a description accompanying the error.]
    // Tests_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_039: [The event handler shall cause the AmqpsIotHubConnection to fail.]
    @Test
    public void onTransportErrorLogsErrorAndCausesIotHubConnectionFail(
            @Mocked ErrorCondition mockCondition)
    {
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";
        final String errorString = "Mock Error";
        final String errorDescription = "Mock Description";

        new NonStrictExpectations()
        {
            {
                mockEvent.getTransport();
                result = mockTransport;
                mockTransport.getCondition();
                result = mockCondition;
                mockCondition.toString();
                result = errorString;
                mockCondition.getDescription();
                result = errorDescription;
            }
        };

        AmqpsIotHubConnectionBaseHandler handler = new AmqpsIotHubConnectionBaseHandler(hostName, userName, sasToken, deviceId, mockIotHubConnection);
        handler.onTransportError(mockEvent);

        new Verifications()
        {
            {
                System.err.println("Error: " + errorString);
                System.err.println("Error: " + errorDescription);
                Deencapsulation.invoke(mockIotHubConnection, "fail", errorString);
            }
        };
    }

    // Tests_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_040: [The function shall create a new Message (Proton) object.]
    @Test
    public void createBinaryMessageCreatesNewProtonMessage(){
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";
        final byte[] msgBody = { 0x61, 0x62, 0x63 };

        AmqpsIotHubConnectionBaseHandler handler = new AmqpsIotHubConnectionBaseHandler(hostName, userName, sasToken, deviceId, mockIotHubConnection);
        Deencapsulation.setField(handler, "sender", mockSender);
        handler.createBinaryMessage(msgBody, null);

        new Verifications()
        {
            {
                mockMessage = mockProton.message();
            }
        };
    }

    // Tests_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_041: [The function shall set the ‘to’ property on the Message object using the created event path.]
    // Tests_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_042: [The function shall set the ‘messageId’ property on the Message object if the messageId is not null.]
    @Test
    public void createBinaryMessageSetsToPropertyAndMessageIDIfNotNull(){
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";
        final byte[] msgBody = { 0x61, 0x62, 0x63 };
        final Object messageId = "123";
        final String expectedEndpoint = "/devices/test-deviceId/messages/events";

        new NonStrictExpectations()
        {
            {
                new Properties();
                result = mockProperties;
            }
        };

        AmqpsIotHubConnectionBaseHandler handler = new AmqpsIotHubConnectionBaseHandler(hostName, userName, sasToken, deviceId, mockIotHubConnection);
        Deencapsulation.setField(handler, "sender", mockSender);
        handler.createBinaryMessage(msgBody, null);
        handler.createBinaryMessage(msgBody, messageId);

        new Verifications()
        {
            {
                mockProperties.setTo(expectedEndpoint); times = 2;
                mockProperties.setMessageId(messageId); times = 1;
                mockMessage.setProperties(mockProperties); times = 2;
            }
        };
    }

    // Tests_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_043: [The function shall create a Binary (Proton) object from the content array.]
    // Tests_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_044: [The function shall create a data Section (Proton) object from the Binary.]
    // Tests_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_045: [The function shall set the Message body to the created data Section.]
    @Test
    public void createBinaryMessageSetsBodyFromContent(){
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";
        final byte[] msgBody = { 0x61, 0x62, 0x63 };
        final Object messageId = "123";

        AmqpsIotHubConnectionBaseHandler handler = new AmqpsIotHubConnectionBaseHandler(hostName, userName, sasToken, deviceId, mockIotHubConnection);
        Deencapsulation.setField(handler, "sender", mockSender);

        handler.createBinaryMessage(msgBody, messageId);

        new Verifications()
        {
            {
                mockBinary = new Binary(msgBody);
                mockSection = new Data(mockBinary);
                Message m = Deencapsulation.getField(handler, "outgoingMessage");
                m.setBody(mockSection);
            }
        };
    }

    // Tests_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_046: [The function shall open the Sender (Proton) link.]
    // Tests_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_047: [The function shall return a new CompletableFuture for the sent message.]
    // Tests_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_048: [The function shall lock sending on the AmqpsIotHubConnection.]
    @Test
    public void createBinaryMessageOpensSenderAndLocksSending(
            @Mocked final CompletableFuture<Integer> mockFuture)
    {
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";
        final byte[] msgBody = { 0x61, 0x62, 0x63 };
        final Object messageId = "123";

        new NonStrictExpectations()
        {
            {
                new CompletableFuture<Integer>();
                result = mockFuture;
            }
        };

        AmqpsIotHubConnectionBaseHandler handler = new AmqpsIotHubConnectionBaseHandler(hostName, userName, sasToken, deviceId, mockIotHubConnection);
        Deencapsulation.setField(handler, "sender", mockSender);

        CompletableFuture<Integer> actualFuture = handler.createBinaryMessage(msgBody, messageId);

        assertEquals(CompletableFuture.class, actualFuture.getClass());

        new Verifications()
        {
            {
                Deencapsulation.invoke(mockIotHubConnection, "lockSending");
                mockSender.open();
            }
        };
    }

}
