/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package tests.unit.com.microsoft.azure.iothub.transport.amqps;

import com.microsoft.azure.iothub.transport.amqps.AmqpsMessage;
import com.microsoft.azure.iothub.transport.amqps.AmqpsReceiver;
import com.microsoft.azure.iothub.transport.amqps.AmqpsReceiverHandler;
import mockit.Deencapsulation;
import mockit.Mocked;
import mockit.NonStrictExpectations;
import mockit.Verifications;
import org.apache.qpid.proton.Proton;
import org.apache.qpid.proton.amqp.messaging.Properties;
import org.apache.qpid.proton.amqp.messaging.Source;
import org.apache.qpid.proton.amqp.messaging.Target;
import org.apache.qpid.proton.engine.*;
import org.apache.qpid.proton.message.Message;
import org.apache.qpid.proton.messenger.impl.Address;
import org.apache.qpid.proton.reactor.FlowController;
import org.apache.qpid.proton.reactor.Handshaker;
import org.junit.Test;

import static org.junit.Assert.assertEquals;

public class AmqpsReceiverHandlerTest {

    public static final String PORT = ":5671";

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
    protected Event mockEvent;
    @Mocked
    protected Receiver mockReceiver;
    @Mocked
    protected Delivery mockDelivery;
    @Mocked
    protected AmqpsReceiver mockAmqpsReceiver;
    @Mocked
    protected Transport mockTransport;
    @Mocked
    protected Connection mockConnection;
    @Mocked
    protected Sasl mockSasl;
    @Mocked
    protected SslDomain mockSslDomain;
    @Mocked
    protected Session mockSession;
    @Mocked
    protected Link mockLink;
    @Mocked
    protected Source mockSource;

    // Codes_SRS_AMQPSRECEIVERHANDLER_14_002: [The constructor shall copy all input parameters to private member variables for event processing.]
    // Codes_SRS_AMQPSRECEIVERHANDLER_14_003: [The constructor shall concatenate the host name with the port.]
    // Codes_SRS_AMQPSRECEIVERHANDLER_14_004: [The constructor shall initialize the endpoint private member variable using the ENDPOINT_FORMAT and deviceID.]
    // Codes_SRS_AMQPSRECEIVERHANDLER_14_005: [The constructor shall initialize a new Handshaker (Proton) object to handle communication handshake.]
    // Codes_SRS_AMQPSRECEIVERHANDLER_14_006: [The constructor shall initialize a new FlowController (Proton) object to handle communication handshake.]
    @Test
    public void constructorCopiesAllDataAndAddsHandlers(){
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";

        final AmqpsReceiverHandler handler = new AmqpsReceiverHandler(hostName, userName, deviceId, sasToken, mockAmqpsReceiver);

        final String expectedEndpoint = "/devices/test-deviceId/messages/devicebound";

        String actualHostName = Deencapsulation.getField(handler, "hostName");
        String actualDeviceId = Deencapsulation.getField(handler, "deviceID");
        String actualUserName = Deencapsulation.getField(handler, "userName");
        String actualSasToken = Deencapsulation.getField(handler, "sasToken");
        String actualEndpoint = Deencapsulation.getField(handler, "endpoint");
        AmqpsReceiver actualReceiver = Deencapsulation.getField(handler, "parentReceiver");

        assertEquals(hostName+PORT, actualHostName);
        assertEquals(userName, actualUserName);
        assertEquals(deviceId, actualDeviceId);
        assertEquals(sasToken, actualSasToken);
        assertEquals(expectedEndpoint, actualEndpoint);
        assertEquals(mockAmqpsReceiver, actualReceiver);

        new Verifications()
        {
            {
                mockHandshaker = new Handshaker();
                mockFlowController = new FlowController();
            }
        };
    }

    // Codes_SRS_AMQPSRECEIVERHANDLER_14_001: [The constructor shall throw a new IllegalArgumentException if any of the input parameters is null or empty.]
    @Test(expected = IllegalArgumentException.class)
    public void constructorThrowsIllegalArgumentExceptionIfHostNameIsEmpty(){
        final String hostName = "";
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";

        AmqpsReceiverHandler handler = new AmqpsReceiverHandler(hostName, userName, deviceId, sasToken, mockAmqpsReceiver);
    }

    // Codes_SRS_AMQPSRECEIVERHANDLER_14_001: [The constructor shall throw a new IllegalArgumentException if any of the input parameters is null or empty.]
    @Test(expected = IllegalArgumentException.class)
    public void constructorThrowsIllegalArgumentExceptionIfHostNameIsNull(){
        final String hostName = null;
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";

        AmqpsReceiverHandler handler = new AmqpsReceiverHandler(hostName, userName, deviceId, sasToken, mockAmqpsReceiver);
    }

    // Codes_SRS_AMQPSRECEIVERHANDLER_14_001: [The constructor shall throw a new IllegalArgumentException if any of the input parameters is null or empty.]
    @Test(expected = IllegalArgumentException.class)
    public void constructorThrowsIllegalArgumentExceptionIfDeviceIDIsEmpty(){
        final String hostName = "test.host.name";
        final String deviceId = "";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";

        AmqpsReceiverHandler handler = new AmqpsReceiverHandler(hostName, userName, deviceId, sasToken, mockAmqpsReceiver);
    }

    // Codes_SRS_AMQPSRECEIVERHANDLER_14_001: [The constructor shall throw a new IllegalArgumentException if any of the input parameters is null or empty.]
    @Test(expected = IllegalArgumentException.class)
    public void constructorThrowsIllegalArgumentExceptionIfDeviceIDIsNull(){
        final String hostName = "test.host.name";
        final String deviceId = null;
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";

        AmqpsReceiverHandler handler = new AmqpsReceiverHandler(hostName, userName, deviceId, sasToken, mockAmqpsReceiver);
    }

    // Codes_SRS_AMQPSRECEIVERHANDLER_14_001: [The constructor shall throw a new IllegalArgumentException if any of the input parameters is null or empty.]
    @Test(expected = IllegalArgumentException.class)
    public void constructorThrowsIllegalArgumentExceptionIfUserNameIsEmpty(){
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = "";
        final String sasToken = "test-token";

        AmqpsReceiverHandler handler = new AmqpsReceiverHandler(hostName, userName, deviceId, sasToken, mockAmqpsReceiver);
    }

    // Codes_SRS_AMQPSRECEIVERHANDLER_14_001: [The constructor shall throw a new IllegalArgumentException if any of the input parameters is null or empty.]
    @Test(expected = IllegalArgumentException.class)
    public void constructorThrowsIllegalArgumentExceptionIfUserNameIsNull(){
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = null;
        final String sasToken = "test-token";

        AmqpsReceiverHandler handler = new AmqpsReceiverHandler(hostName, userName, deviceId, sasToken, mockAmqpsReceiver);
    }

    // Codes_SRS_AMQPSRECEIVERHANDLER_14_001: [The constructor shall throw a new IllegalArgumentException if any of the input parameters is null or empty.]
    @Test(expected = IllegalArgumentException.class)
    public void constructorThrowsIllegalArgumentExceptionIfSasTokenIsEmpty(){
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "";

        AmqpsReceiverHandler handler = new AmqpsReceiverHandler(hostName, userName, deviceId, sasToken, mockAmqpsReceiver);
    }

    // Codes_SRS_AMQPSRECEIVERHANDLER_14_001: [The constructor shall throw a new IllegalArgumentException if any of the input parameters is null or empty.]
    @Test(expected = IllegalArgumentException.class)
    public void constructorThrowsIllegalArgumentExceptionIfSasTokenIsNull(){
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = null;

        AmqpsReceiverHandler handler = new AmqpsReceiverHandler(hostName, userName, deviceId, sasToken, mockAmqpsReceiver);
    }

    // Codes_SRS_AMQPSRECEIVERHANDLER_14_001: [The constructor shall throw a new IllegalArgumentException if any of the input parameters is null or empty.]
    @Test(expected = IllegalArgumentException.class)
    public void constructoThrowsIllegalArgumentExceptionIfReceiverIsNull(){
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";

        AmqpsReceiverHandler handler = new AmqpsReceiverHandler(hostName, userName, deviceId, sasToken, null);
    }

    // Codes_SRS_AMQPSRECEIVERHANDLER_14_007: [The event handler shall get the Receiver and Delivery (Proton) objects from the event.]
    @Test
    public void onDeliveryGetsReceiverAndDelivery(){
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";

        new NonStrictExpectations()
        {
            {
                mockEvent.getLink();
                result = mockReceiver;
                mockReceiver.current();
                result = mockDelivery;
            }
        };

        AmqpsReceiverHandler handler = new AmqpsReceiverHandler(hostName, userName, deviceId, sasToken, mockAmqpsReceiver);

        handler.onDelivery(mockEvent);

        new Verifications()
        {
            {
                mockEvent.getLink();
                mockReceiver.current();
            }
        };
    }

    // Codes_SRS_AMQPSRECEIVERHANDLER_14_008: [The event handler shall read the received buffer.]
    @Test
    public void onDeliveryReadsReceivedBuffer(){
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";
        final int pendingDeliverySize = 10;

        new NonStrictExpectations()
        {
            {
                mockEvent.getLink();
                result = mockReceiver;
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

        AmqpsReceiverHandler handler = new AmqpsReceiverHandler(hostName, userName, deviceId, sasToken, mockAmqpsReceiver);

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

    // Codes_SRS_AMQPSRECEIVERHANDLER_14_009: [The event handler shall create an AmqpsMessage object from the decoded buffer.]
    // Codes_SRS_AMQPSRECEIVERHANDLER_14_010: [The event handler shall set the AmqpsMessage Delivery (Proton) object.]
    @Test
    public void onDeliveryCreatesAndSetsAmqpsMessage(){
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";
        final int pendingDeliverySize = 10;
        final byte[] buffer = new byte[pendingDeliverySize];

        new NonStrictExpectations()
        {
            {
                mockEvent.getLink();
                result = mockReceiver;
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

        AmqpsReceiverHandler handler = new AmqpsReceiverHandler(hostName, userName, deviceId, sasToken, mockAmqpsReceiver);

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

    // Codes_SRS_AMQPSRECEIVERHANDLER_14_011: [The event handler shall give the message back to its parent AmqpsReceiver.]
    @Test
    public void onDeliveryGivesMessageToParentAmqpsReceiver(){
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";
        final int pendingDeliverySize = 10;
        final byte[] buffer = new byte[pendingDeliverySize];

        new NonStrictExpectations()
        {
            {
                mockEvent.getLink();
                result = mockReceiver;
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

        AmqpsReceiverHandler handler = new AmqpsReceiverHandler(hostName, userName, deviceId, sasToken, mockAmqpsReceiver);

        handler.onDelivery(mockEvent);

        new Verifications()
        {
            {
                Deencapsulation.invoke(mockAmqpsReceiver, "addMessage", mockAmqpsMessage);
            }
        };
    }

    // Codes_SRS_AMQPSRECEIVERHANDLER_14_012: [The event handler shall get the Transport (Proton) object from the event.]
    @Test
    public void onConnectionBoundGetsTransport(){
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";

        AmqpsReceiverHandler handler = new AmqpsReceiverHandler(hostName, userName, deviceId, sasToken, mockAmqpsReceiver);
        handler.onConnectionBound(mockEvent);

        new Verifications()
        {
            {
                mockEvent.getConnection();
                mockConnection.getTransport();
            }
        };
    }

    // Codes_SRS_AMQPSRECEIVERHANDLER_14_013: [The event handler shall set the SASL_PLAIN authentication on the transport using the given user name and sas token.]
    @Test
    public void onConnectionBoundSetsProperAuthenticationUsingUserNameAndSasToken(){
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";

        AmqpsReceiverHandler handler = new AmqpsReceiverHandler(hostName, userName, deviceId, sasToken, mockAmqpsReceiver);
        handler.onConnectionBound(mockEvent);

        new Verifications()
        {
            {
                mockTransport.sasl();
                mockSasl.plain(userName, sasToken);
            }
        };
    }

    // Codes_SRS_AMQPSRECEIVERHANDLER_14_014: [The event handler shall set ANONYMOUS_PEER authentication mode on the domain of the Transport.]
    @Test
    public void onConnectionBoundSetsAuthenticationModeOnDomain(){
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";

        AmqpsReceiverHandler handler = new AmqpsReceiverHandler(hostName, userName, deviceId, sasToken, mockAmqpsReceiver);
        handler.onConnectionBound(mockEvent);

        new Verifications()
        {
            {
                mockSslDomain.setPeerAuthentication(SslDomain.VerifyMode.ANONYMOUS_PEER);
                mockTransport.ssl(mockSslDomain);
            }
        };
    }

    // Codes_SRS_AMQPSRECEIVERHANDLER_14_015: [The event handler shall get the Connection (Proton) object from the event handler and set the host name on the connection.]
    // Codes_SRS_AMQPSRECEIVERHANDLER_14_016: [The event handler shall create a Session (Proton) object from the connection.]
    // Codes_SRS_AMQPSRECEIVERHANDLER_14_017: [The event handler shall create a Receiver (Proton) object and set the protocol tag on it to a predefined constant.]
    // Codes_SRS_AMQPSRECEIVERHANDLER_14_018: [The event handler shall open the Connection, Session, and Receiver objects.]
    @Test
    public void onConnectionInitCreatesSession(){
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";
        final String tag = "receiver";

        AmqpsReceiverHandler handler = new AmqpsReceiverHandler(hostName, userName, deviceId, sasToken, mockAmqpsReceiver);
        handler.onConnectionInit(mockEvent);

        final String expectedHostName = hostName+PORT;

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

    // Codes_SRS_AMQPSRECEIVERHANDLER_14_019: [The event handler shall create a new Source (Proton) object using the given endpoint address.]
    // Codes_SRS_AMQPSRECEIVERHANDLER_14_020: [The event handler shall get the Link (Proton) object and set its source to the created Source (Proton) object.]
    @Test
    public void onLinkInitCreatesSourceAndSetsOnLink(){
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";

        new NonStrictExpectations()
        {
            {
                new Source();
                result = mockSource;
            }
        };

        AmqpsReceiverHandler handler = new AmqpsReceiverHandler(hostName, userName, deviceId, sasToken, mockAmqpsReceiver);
        handler.onLinkInit(mockEvent);

        final String expectedEndpoint = "/devices/test-deviceId/messages/devicebound";

        new Verifications()
        {
            {
                mockSource.setAddress(expectedEndpoint);
                mockLink.setSource(mockSource);
            }
        };
    }

    // Codes_SRS_AMQPSRECEIVERHANDLER_14_021: [The event handler shall close the Session and Connection (Proton) objects.]
    @Test
    public void onLinkLocalCloseClosesSessionAndConnection(){
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";

        AmqpsReceiverHandler handler = new AmqpsReceiverHandler(hostName, userName, deviceId, sasToken, mockAmqpsReceiver);
        handler.onLinkLocalClose(mockEvent);

        new Verifications()
        {
            {
                mockEvent.getSession().close();
                mockEvent.getSession().getConnection().close();
            }
        };
    }
}
