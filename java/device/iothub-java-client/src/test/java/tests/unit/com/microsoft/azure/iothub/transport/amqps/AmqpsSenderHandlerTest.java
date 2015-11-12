/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */
package tests.unit.com.microsoft.azure.iothub.transport.amqps;

import com.microsoft.azure.iothub.transport.amqps.AmqpsSenderHandler;
import mockit.Deencapsulation;
import mockit.Mocked;
import mockit.NonStrictExpectations;
import mockit.Verifications;
import org.apache.qpid.proton.Proton;
import org.apache.qpid.proton.amqp.Binary;
import org.apache.qpid.proton.amqp.messaging.Data;
import org.apache.qpid.proton.amqp.messaging.Properties;
import org.apache.qpid.proton.amqp.messaging.Section;
import org.apache.qpid.proton.amqp.messaging.Target;
import org.apache.qpid.proton.engine.*;
import org.apache.qpid.proton.message.Message;
import org.apache.qpid.proton.messenger.impl.Address;
import org.apache.qpid.proton.reactor.Handshaker;
import org.junit.Test;

import static org.junit.Assert.assertArrayEquals;
import static org.junit.Assert.assertEquals;

public class AmqpsSenderHandlerTest {

    public final String PORT = ":5671";

    @Mocked
    protected Handshaker mockHandshaker;
    @Mocked
    protected Proton mockProton;
    @Mocked
    protected Message mockMessage;
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
    protected Link mockLink;
    @Mocked
    protected Target mockTarget;
    @Mocked
    protected Delivery mockDelivery;

    // Tests_SRS_AMQPSSENDERHANDLER_14_002: [The constructor shall copy all input parameters to private member variables for event processing.]
    // Tests_SRS_AMQPSSENDERHANDLER_14_003: [The constructor shall concatenate the host name with the port.]
    // Tests_SRS_AMQPSSENDERHANDLER_14_004: [The constructor shall initialize the endpoint private member variable using the ENDPOINT_FORMAT and deviceID.]
    // Tests_SRS_AMQPSSENDERHANDLER_14_005: [The constructor shall initialize a new Handshaker (Proton) object to handle communication handshake.]
    @Test
    public void constructorCopiesAllData(){
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";

        final AmqpsSenderHandler handler = new AmqpsSenderHandler(hostName, userName, deviceId, sasToken);

        final String expectedEndpoint = "/devices/test-deviceId/messages/events";

        String actualHostName = Deencapsulation.getField(handler, "hostName");
        String actualDeviceId = Deencapsulation.getField(handler, "deviceID");
        String actualUserName = Deencapsulation.getField(handler, "userName");
        String actualSasToken = Deencapsulation.getField(handler, "sasToken");
        String actualEndpoint = Deencapsulation.getField(handler, "endpoint");

        assertEquals(hostName+PORT, actualHostName);
        assertEquals(userName, actualUserName);
        assertEquals(deviceId, actualDeviceId);
        assertEquals(sasToken, actualSasToken);
        assertEquals(expectedEndpoint, actualEndpoint);

        new Verifications()
        {
            {
                mockHandshaker = new Handshaker();
            }
        };
    }

    // Tests_SRS_AMQPSSENDERHANDLER_14_001: [The constructor shall throw a new IllegalArgumentException if any of the input parameters is null or empty.]
    @Test(expected = IllegalArgumentException.class)
    public void constructorThrowsIllegalArgumentExceptionIfHostNameIsEmpty(){
        final String hostName = "";
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";

        AmqpsSenderHandler handler = new AmqpsSenderHandler(hostName, userName, deviceId, sasToken);
    }

    // Tests_SRS_AMQPSSENDERHANDLER_14_001: [The constructor shall throw a new IllegalArgumentException if any of the input parameters is null or empty.]
    @Test(expected = IllegalArgumentException.class)
    public void constructorThrowsIllegalArgumentExceptionIfHostNameIsNull(){
        final String hostName = null;
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";

        AmqpsSenderHandler handler = new AmqpsSenderHandler(hostName, userName, deviceId, sasToken);
    }

    // Tests_SRS_AMQPSSENDERHANDLER_14_001: [The constructor shall throw a new IllegalArgumentException if any of the input parameters is null or empty.]
    @Test(expected = IllegalArgumentException.class)
    public void constructorThrowsIllegalArgumentExceptionIfDeviceIDIsEmpty(){
        final String hostName = "test.host.name";
        final String deviceId = "";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";

        AmqpsSenderHandler handler = new AmqpsSenderHandler(hostName, userName, deviceId, sasToken);
    }

    // Tests_SRS_AMQPSSENDERHANDLER_14_001: [The constructor shall throw a new IllegalArgumentException if any of the input parameters is null or empty.]
    @Test(expected = IllegalArgumentException.class)
    public void constructorThrowsIllegalArgumentExceptionIfDeviceIDIsNull(){
        final String hostName = "test.host.name";
        final String deviceId = null;
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";

        AmqpsSenderHandler handler = new AmqpsSenderHandler(hostName, userName, deviceId, sasToken);
    }

    // Tests_SRS_AMQPSSENDERHANDLER_14_001: [The constructor shall throw a new IllegalArgumentException if any of the input parameters is null or empty.]
    @Test(expected = IllegalArgumentException.class)
    public void constructorThrowsIllegalArgumentExceptionIfUserNameIsEmpty(){
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = "";
        final String sasToken = "test-token";

        AmqpsSenderHandler handler = new AmqpsSenderHandler(hostName, userName, deviceId, sasToken);
    }

    // Tests_SRS_AMQPSSENDERHANDLER_14_001: [The constructor shall throw a new IllegalArgumentException if any of the input parameters is null or empty.]
    @Test(expected = IllegalArgumentException.class)
    public void constructorThrowsIllegalArgumentExceptionIfUserNameIsNull(){
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = null;
        final String sasToken = "test-token";

        AmqpsSenderHandler handler = new AmqpsSenderHandler(hostName, userName, deviceId, sasToken);
    }

    // Tests_SRS_AMQPSSENDERHANDLER_14_001: [The constructor shall throw a new IllegalArgumentException if any of the input parameters is null or empty.]
    @Test(expected = IllegalArgumentException.class)
    public void constructorThrowsIllegalArgumentExceptionIfSasTokenIsEmpty(){
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "";

        AmqpsSenderHandler handler = new AmqpsSenderHandler(hostName, userName, deviceId, sasToken);
    }

    // Tests_SRS_AMQPSSENDERHANDLER_14_001: [The constructor shall throw a new IllegalArgumentException if any of the input parameters is null or empty.]
    @Test(expected = IllegalArgumentException.class)
    public void constructorThrowsIllegalArgumentExceptionIfSasTokenIsNull(){
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = null;

        AmqpsSenderHandler handler = new AmqpsSenderHandler(hostName, userName, deviceId, sasToken);
    }

    // Tests_SRS_AMQPSSENDERHANDLER_14_006: [The function shall create a new Message (Proton) object.]
    @Test
    public void createBinaryMessageCreatesNewProtonMessage(){
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";
        final byte[] msgBody = { 0x61, 0x62, 0x63 };

        AmqpsSenderHandler handler = new AmqpsSenderHandler(hostName, userName, deviceId, sasToken);

        handler.createBinaryMessage(msgBody, null);

        new Verifications()
        {
            {
                mockMessage = mockProton.message();
            }
        };
    }

    // Tests_SRS_AMQPSSENDERHANDLER_14_007: [The function shall set the ‘to’ property on the Message object using the created event path.]
    // Tests_SRS_AMQPSSENDERHANDLER_14_026: [The function shall set the ‘messageId’ property on the Message object if the messageId is not null.]
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

        final AmqpsSenderHandler handler = new AmqpsSenderHandler(hostName, userName, deviceId, sasToken);

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

    // Tests_SRS_AMQPSSENDERHANDLER_14_008: [The function shall create a Binary (Proton) object from the content array.]
    // Tests_SRS_AMQPSSENDERHANDLER_14_009: [The function shall create a data Section (Proton) object from the Binary.]
    // Tests_SRS_AMQPSSENDERHANDLER_14_010: [The function shall set the Message body to the created data section.]
    @Test
    public void createBinaryMessageSetsBodyFromContent(){
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";
        final byte[] msgBody = { 0x61, 0x62, 0x63 };
        final Object messageId = "123";

        final AmqpsSenderHandler handler = new AmqpsSenderHandler(hostName, userName, deviceId, sasToken);

        handler.createBinaryMessage(msgBody, messageId);

        new Verifications()
        {
            {
                mockBinary = new Binary(msgBody);
                mockSection = new Data(mockBinary);
                Message m = Deencapsulation.getField(handler, "message");
                m.setBody(mockSection);
            }
        };
    }

    // Tests_SRS_AMQPSSENDERHANDLER_14_011: [The event handler shall get the Transport (Proton) object from the event.]
    @Test
    public void onConnectionBoundGetsTransport(){
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";

        AmqpsSenderHandler handler = new AmqpsSenderHandler(hostName, userName, deviceId, sasToken);
        handler.onConnectionBound(mockEvent);

        new Verifications()
        {
            {
                mockConnection = mockEvent.getConnection();
                mockConnection.getTransport();
            }
        };
    }

    // Tests_SRS_AMQPSSENDERHANDLER_14_012: [The event handler shall set the SASL_PLAIN authentication on the transport using the given user name and sas token.]
    @Test
    public void onConnectionBoundSetsProperAuthenticationUsingUserNameAndSasToken(){
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";

        AmqpsSenderHandler handler = new AmqpsSenderHandler(hostName, userName, deviceId, sasToken);
        handler.onConnectionBound(mockEvent);

        new Verifications()
        {
            {
                mockSasl = mockTransport.sasl();
                mockSasl.plain(userName, sasToken);
            }
        };
    }

    // Tests_SRS_AMQPSSENDERHANDLER_14_013: [The event handler shall set ANONYMOUS_PEER authentication mode on the domain of the Transport.]
    @Test
    public void onConnectionBoundSetsAuthenticationModeOnDomain(){
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";

        AmqpsSenderHandler handler = new AmqpsSenderHandler(hostName, userName, deviceId, sasToken);
        handler.onConnectionBound(mockEvent);

        new Verifications()
        {
            {
                mockSslDomain.setPeerAuthentication(SslDomain.VerifyMode.ANONYMOUS_PEER);
                mockTransport.ssl(mockSslDomain);
            }
        };
    }

    // Tests_SRS_AMQPSSENDERHANDLER_14_014: [The event handler shall get the Connection (Proton) object from the event handler and set the host name on the connection.]
    // Tests_SRS_AMQPSSENDERHANDLER_14_015: [The event handler shall create a Session (Proton) object from the connection.]
    // Tests_SRS_AMQPSSENDERHANDLER_14_016: [The event handler shall create a Sender (Proton) object and set the protocol tag on it to a predefined constant.]
    // Tests_SRS_AMQPSSENDERHANDLER_14_017: [The event handler shall open the Connection, Session, and Sender objects.]
    @Test
    public void onConnectionInitCreatesSession(){
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";
        final String tag = "sender";

        AmqpsSenderHandler handler = new AmqpsSenderHandler(hostName, userName, deviceId, sasToken);
        handler.onConnectionInit(mockEvent);

        final String expectedHostName = hostName+PORT;

        new Verifications()
        {
            {
                mockConnection = mockEvent.getConnection();
                mockConnection.setHostname(expectedHostName);
                mockSession = mockConnection.session();
                mockSender = mockSession.sender(tag);
                mockConnection.open();
                mockSession.open();
                mockSender.open();
            }
        };
    }

    // Tests_SRS_AMQPSSENDERHANDLER_14_018: [The event handler shall create a new Target (Proton) object using the given endpoint address.]
    // Tests_SRS_AMQPSSENDERHANDLER_14_019: [The event handler shall get the Link (Proton) object and set its target.]
    @Test
    public void onLinkInitCreatesTargetAndSetOnLink(){
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";

        new NonStrictExpectations()
        {
            {
                new Target();
                result = mockTarget;
            }
        };

        AmqpsSenderHandler handler = new AmqpsSenderHandler(hostName, userName, deviceId, sasToken);
        handler.onLinkInit(mockEvent);

        final String expectedEndpoint = "/devices/test-deviceId/messages/events";

        new Verifications()
        {
            {
                mockTarget.setAddress(expectedEndpoint);
                mockLink.setTarget(mockTarget);
            }
        };
    }

    // Tests_SRS_AMQPSSENDERHANDLER_14_020: [The event handler shall get the Sender (Proton) object from the link.]
    // Tests_SRS_AMQPSSENDERHANDLER_14_021: [The event handler shall encode the message and copy the contents to the byte buffer.]
    // Tests_SRS_AMQPSSENDERHANDLER_14_022: [The event handler shall set the delivery tag on the Sender (Proton) object.]
    // Tests_SRS_AMQPSSENDERHANDLER_14_023: [The event handler shall send the encoded bytes.]
    // Tests_SRS_AMQPSSENDERHANDLER_14_024: [The event handler shall close the Sender, Session, and Connection objects.]
    @Test
    public void onLinkFlowFullTest(){
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
            }
        };

        AmqpsSenderHandler handler = new AmqpsSenderHandler(hostName, userName, deviceId, sasToken);

        handler.createBinaryMessage(msgBody, null);
        handler.onLinkFlow(mockEvent);

        new Verifications()
        {
            {
                mockEvent.getLink();
                byte[] buffer = new byte[1024];
                int length = mockMessage.encode(buffer, 0, buffer.length);
                byte[] deliveryTag = String.valueOf(tag).getBytes();
                mockSender.delivery(deliveryTag);
                mockSender.send((byte[])any, 0, length);

                mockSender.advance();
                mockSender.close();
                mockSender.getSession().close();
                mockSender.getSession().getConnection().close();
            }
        };
    }
}
