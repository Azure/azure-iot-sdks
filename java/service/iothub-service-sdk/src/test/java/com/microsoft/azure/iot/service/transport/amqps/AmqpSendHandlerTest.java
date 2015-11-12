/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.transport.amqps;

import mockit.Expectations;
import mockit.Mocked;
import mockit.integration.junit4.JMockit;
import org.apache.qpid.proton.Proton;
import org.apache.qpid.proton.amqp.Binary;
import org.apache.qpid.proton.amqp.messaging.*;
import org.apache.qpid.proton.amqp.transport.ErrorCondition;
import org.apache.qpid.proton.amqp.transport.ReceiverSettleMode;
import org.apache.qpid.proton.amqp.transport.SenderSettleMode;
import org.apache.qpid.proton.amqp.transport.Source;
import org.apache.qpid.proton.engine.*;
import org.apache.qpid.proton.message.Message;
import org.apache.qpid.proton.message.MessageError;
import org.apache.qpid.proton.message.MessageFormat;
import org.apache.qpid.proton.messenger.impl.Address;
import org.apache.qpid.proton.reactor.Handshaker;
import org.apache.qpid.proton.reactor.Reactor;
import org.apache.qpid.proton.reactor.Selectable;
import org.apache.qpid.proton.reactor.Task;
import org.junit.Test;
import org.junit.runner.RunWith;

import java.nio.BufferOverflowException;
import java.util.EnumSet;

import static org.junit.Assert.assertEquals;

/** Unit tests for AmqpSendHandler */
@RunWith(JMockit.class)
public class AmqpSendHandlerTest
{
    private Integer exceptionCount = 0;

    @Mocked Handshaker handshaker;
    @Mocked Proton proton;
    @Mocked Message message;
    @Mocked Message messageWithException;
    @Mocked Properties properties;
    @Mocked Binary binary;
    @Mocked Section section;
    @Mocked Data data;
    @Mocked Event event;
    @Mocked Transport transport;
    @Mocked Connection connection;
    @Mocked Address address;
    @Mocked Sasl sasl;
    @Mocked SslDomain sslDomain;
    @Mocked Session session;
    @Mocked Sender sender;
    @Mocked Link link;
    @Mocked Target target;
    @Mocked Delivery delivery;

    // Test_SRS_SERVICE_SDK_JAVA_AMQPSENDHANDLER_12_001: [The constructor shall copy all input parameters to private member variables for event processing]
    // Test_SRS_SERVICE_SDK_JAVA_AMQPSENDHANDLER_12_002: [The constructor shall concatenate the host name with the port]
    // Test_SRS_SERVICE_SDK_JAVA_AMQPSENDHANDLER_12_003: [The constructor shall initialize a new Handshaker (Proton) object to handle communication handshake]
    @Test
    public void constructor_copies_params_to_members()
    {
        // Arrange
        String hostName = "aaa";
        String userName = "bbb";
        String sasToken = "ccc";
        new Expectations()
        {
            {
                handshaker = new Handshaker();
            }
        };
        // Act
        AmqpSendHandler amqpSendHandler = new AmqpSendHandler(hostName, userName, sasToken);
        String _hostName = amqpSendHandler.hostName;
        String _userName = amqpSendHandler.userName;
        String _sasToken = amqpSendHandler.sasToken;
        // Assert
        assertEquals(hostName + ":5671", _hostName);
        assertEquals(userName, _userName);
        assertEquals(sasToken, _sasToken);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_AMQPSENDHANDLER_12_001: [The constructor shall throw IllegalArgumentException if any of the input parameter is null or empty]
    // Assert
    @Test (expected = IllegalArgumentException.class)
    public void constructor_checks_if_hostName_null()
    {
        // Arrange
        String hostName = null;
        String userName = "bbb";
        String sasToken = "ccc";
        // Act
        AmqpSendHandler amqpSend = new AmqpSendHandler(hostName, userName, sasToken);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_AMQPSENDHANDLER_12_001: [The constructor shall throw IllegalArgumentException if any of the input parameter is null or empty]
    // Assert
    @Test (expected = IllegalArgumentException.class)
    public void constructor_checks_if_hostName_empty()
    {
        // Arrange
        String hostName = "";
        String userName = "bbb";
        String sasToken = "ccc";
        // Act
        AmqpSendHandler amqpSend = new AmqpSendHandler(hostName, userName, sasToken);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_AMQPSENDHANDLER_12_001: [The constructor shall throw IllegalArgumentException if any of the input parameter is null or empty]
    // Assert
    @Test (expected = IllegalArgumentException.class)
    public void constructor_checks_if_userName_null()
    {
        // Arrange
        String hostName = "aaa";
        String userName = null;
        String sasToken = "ccc";
        // Act
        AmqpSendHandler amqpSend = new AmqpSendHandler(hostName, userName, sasToken);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_AMQPSENDHANDLER_12_001: [The constructor shall throw IllegalArgumentException if any of the input parameter is null or empty]
    // Assert
    @Test (expected = IllegalArgumentException.class)
    public void constructor_checks_if_userName_empty()
    {
        // Arrange
        String hostName = "aaa";
        String userName = "";
        String sasToken = "ccc";
        // Act
        AmqpSendHandler amqpSend = new AmqpSendHandler(hostName, userName, sasToken);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_AMQPSENDHANDLER_12_001: [The constructor shall throw IllegalArgumentException if any of the input parameter is null or empty]
    // Assert
    @Test (expected = IllegalArgumentException.class)
    public void constructor_checks_if_sasToken_null()
    {
        // Arrange
        String hostName = "aaa";
        String userName = "bbb";
        String sasToken = null;
        // Act
        AmqpSendHandler amqpSend = new AmqpSendHandler(hostName, userName, sasToken);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_AMQPSENDHANDLER_12_001: [The constructor shall throw IllegalArgumentException if any of the input parameter is null or empty]
    // Assert
    @Test (expected = IllegalArgumentException.class)
    public void constructor_checks_if_sasToken_empty()
    {
        // Arrange
        String hostName = "aaa";
        String userName = "bbb";
        String sasToken = "";
        // Act
        AmqpSendHandler amqpSend = new AmqpSendHandler(hostName, userName, sasToken);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_AMQPSENDHANDLER_12_004: [The function shall create a new Message (Proton) object]
    // Tests_SRS_SERVICE_SDK_JAVA_AMQPSENDHANDLER_12_005: [The function shall set the “to” property on the Message object using the created device path]
    // Tests_SRS_SERVICE_SDK_JAVA_AMQPSENDHANDLER_12_006: [The function shall create a Binary (Proton) object from the content string]
    // Tests_SRS_SERVICE_SDK_JAVA_AMQPSENDHANDLER_12_007: [The function shall create a data Section (Proton) object from the Binary]
    // Tests_SRS_SERVICE_SDK_JAVA_AMQPSENDHANDLER_12_008: [The function shall set the Message body to the created data section]
    @Test
    public void createBinaryMessage_creates_Message_and_sets_To()
    {
        // Arrange
        String hostName = "aaa";
        String userName = "bbb";
        String sasToken = "ccc";
        String deviceId = "deviceId";
        String content = "abcdefghijklmnopqrst";
        String toProperty = "/devices/deviceId/messages/devicebound";
        AmqpSendHandler amqpSendHandler = new AmqpSendHandler(hostName, userName, sasToken);
        // Assert
        new Expectations()
        {
            {
                message = Proton.message();
                properties = new Properties();
                properties.setTo(toProperty);
                message.setProperties(properties);
                binary = new Binary(content.getBytes());
                section = new Data(binary);
                message.setBody(section);
            }
        };
        // Act
        amqpSendHandler.createBinaryMessage(deviceId, content);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_AMQPSENDHANDLER_12_009: [The event handler shall set the SASL PLAIN authentication on the Transport using the given user name and sas token]
    // Tests_SRS_SERVICE_SDK_JAVA_AMQPSENDHANDLER_12_010: [The event handler shall set ANONYMUS_PEER authentication mode on the domain of the Transport]
    @Test
    public void onConnectionBound_sets_Sasl_Plain()
    {
        // Arrange
        String hostName = "aaa";
        String userName = "bbb";
        String sasToken = "ccc";
        String hostAddr = hostName + ":5671";
        AmqpSendHandler amqpSendHandler = new AmqpSendHandler(hostName, userName, sasToken);
        // Assert
        new Expectations()
        {
            {
                connection = event.getConnection();
                transport = connection.getTransport();
                sasl.plain(anyString, anyString);
                address = new Address(hostAddr);
                sslDomain = Proton.sslDomain();
                sslDomain.init(SslDomain.Mode.CLIENT);
                sslDomain.setPeerAuthentication(SslDomain.VerifyMode.ANONYMOUS_PEER);
                transport.ssl(sslDomain);
            }
        };
        // Act
        amqpSendHandler.onConnectionBound(event);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_AMQPSENDHANDLER_12_011: [The event handler shall set the host name on the connection]
    // Tests_SRS_SERVICE_SDK_JAVA_AMQPSENDHANDLER_12_012: [The event handler shall create a Session (Proton) object from the connection]
    // Tests_SRS_SERVICE_SDK_JAVA_AMQPSENDHANDLER_12_013: [The event handler shall create a Sender (Proton) object and set the protocol tag on it to a predefined constant]
    // Tests_SRS_SERVICE_SDK_JAVA_AMQPSENDHANDLER_12_014: [The event handler shall open the Connection, the Session and the Sender object]
    @Test
    public void onConnectionInit_creates_Session_and_open_Connection()
    {
        // Arrange
        String hostName = "aaa";
        String userName = "bbb";
        String sasToken = "ccc";
        String hostAddr = hostName + ":5671";
        AmqpSendHandler amqpSendHandler = new AmqpSendHandler(hostName, userName, sasToken);
        // Assert
        new Expectations()
        {
            {
                connection = event.getConnection();
                connection.setHostname(hostAddr);
                session = connection.session();
                sender = session.sender(anyString);
                connection.open();
                session.open();
                sender.open();
            }
        };
        // Act
        amqpSendHandler.onConnectionInit(event);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_AMQPSENDHANDLER_12_015: [The event handler shall create a new Target (Proton) object using the given endpoint address]
    // Tests_SRS_SERVICE_SDK_JAVA_AMQPSENDHANDLER_12_016: [The event handler shall get the Link (Proton) object and set its target to the created Target (Proton) object]
    @Test
    public void onLinkInit_call_flow_ok()
    {
        // Arrange
        String hostName = "aaa";
        String userName = "bbb";
        String sasToken = "ccc";
        String hostAddr = hostName + ":5671";
        String endpoint = "/messages/devicebound";
        AmqpSendHandler amqpSendHandler = new AmqpSendHandler(hostName, userName, sasToken);
        // Assert
        new Expectations()
        {
            {
                link = event.getLink();
                target = new Target();
                target.setAddress(endpoint);
            }
        };
        // Act
        amqpSendHandler.onLinkInit(event);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_AMQPSENDHANDLER_12_017: [The event handler shall get the Sender (Proton) object from the link]
    // Tests_SRS_SERVICE_SDK_JAVA_AMQPSENDHANDLER_12_018: [The event handler shall encode the message and copy to the byte buffer]
    // Tests_SRS_SERVICE_SDK_JAVA_AMQPSENDHANDLER_12_019: [The event handler shall set the delivery tag on the Sender (Proton) object]
    // Tests_SRS_SERVICE_SDK_JAVA_AMQPSENDHANDLER_12_020: [The event handler shall send the encoded bytes]
    // Tests_SRS_SERVICE_SDK_JAVA_AMQPSENDHANDLER_12_021: [The event handler shall close the Sender, Session and Connection]
    @Test
    public void onLinkFlow_call_flow_ok()
    {
        // Arrange
        String hostName = "aaa";
        String userName = "bbb";
        String sasToken = "ccc";
        String hostAddr = hostName + ":5671";
        String deviceId = "deviceId";
        String content = "abcdefghijklmnopqrst";
        String endpoint = "/messages/devicebound";
        createProtonObjects();
        AmqpSendHandler amqpSendHandler = new AmqpSendHandler(hostName, userName, sasToken);
        amqpSendHandler.createBinaryMessage(deviceId, content);
        // Assert
        new Expectations()
        {
            {
                link = event.getLink();
                link.getCredit();
                byte[] buffer = new byte[1024];
                message.encode(buffer, 0, 1024);
                sender.close();
                session = sender.getSession();
                connection = session.getConnection();
                connection.close();
            }
        };
        // Act
        amqpSendHandler.onLinkFlow(event);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_AMQPSENDHANDLER_12_017: [The event handler shall get the Sender (Proton) object from the link]
    // Tests_SRS_SERVICE_SDK_JAVA_AMQPSENDHANDLER_12_018: [The event handler shall encode the message and copy to the byte buffer]
    // Tests_SRS_SERVICE_SDK_JAVA_AMQPSENDHANDLER_12_019: [The event handler shall set the delivery tag on the Sender (Proton) object]
    // Tests_SRS_SERVICE_SDK_JAVA_AMQPSENDHANDLER_12_020: [The event handler shall send the encoded bytes]
    // Tests_SRS_SERVICE_SDK_JAVA_AMQPSENDHANDLER_12_021: [The event handler shall close the Sender, Session and Connection]
    @Test
    public void onLinkFlowBufferOverflow_call_flow_ok()
    {
        // Arrange
        String hostName = "aaa";
        String userName = "bbb";
        String sasToken = "ccc";
        String hostAddr = hostName + ":5671";
        String deviceId = "deviceId";
        String content = "abcdefghijklmnopqrst";
        String endpoint = "/messages/devicebound";
        exceptionCount = 0;
        createProtonObjects();
        AmqpSendHandler amqpSendHandler = new AmqpSendHandler(hostName, userName, sasToken);
        // Assert
        new Expectations()
        {
            {
                amqpSendHandler.message = messageWithException;
                link = event.getLink();
                link.getCredit();
            }
        };
        // Act
        amqpSendHandler.onLinkFlow(event);
    }

    private void createProtonObjects()
    {
        String exceptionMessage = "Not expected function called";

        message = Proton.message();

        messageWithException = new Message()
        {
            @Override
            public boolean isDurable()
            { return false; }

            @Override
            public long getDeliveryCount()
            { return 0; }

            @Override
            public short getPriority()
            { return 0; }

            @Override
            public boolean isFirstAcquirer()
            { return false; }

            @Override
            public long getTtl()
            { return 0; }

            @Override
            public void setDurable(boolean b)
            { }

            @Override
            public void setTtl(long l)
            { }

            @Override public void setDeliveryCount(long l)
            { }

            @Override
            public void setFirstAcquirer(boolean b)
            { }

            @Override
            public void setPriority(short i)
            { }

            @Override
            public Object getMessageId()
            { return null; }

            @Override
            public long getGroupSequence()
            { return 0; }

            @Override
            public String getReplyToGroupId()
            { return null; }

            @Override
            public long getCreationTime()
            { return 0; }

            @Override
            public String getAddress()
            { return null; }

            @Override
            public byte[] getUserId()
            { return new byte[0]; }

            @Override
            public String getReplyTo()
            { return null; }

            @Override
            public String getGroupId()
            { return null; }

            @Override
            public String getContentType()
            { return null; }

            @Override
            public long getExpiryTime()
            { return 0; }

            @Override
            public Object getCorrelationId()
            { return null; }

            @Override
            public String getContentEncoding()
            { return null; }

            @Override
            public String getSubject()
            { return null; }

            @Override
            public void setGroupSequence(long l)
            { }

            @Override
            public void setUserId(byte[] bytes)
            { }

            @Override
            public void setCreationTime(long l)
            { }

            @Override
            public void setSubject(String s)
            { }

            @Override
            public void setGroupId(String s)
            { }

            @Override
            public void setAddress(String s)
            { }

            @Override
            public void setExpiryTime(long l)
            { }

            @Override
            public void setReplyToGroupId(String s)
            { }

            @Override
            public void setContentEncoding(String s)
            { }

            @Override
            public void setContentType(String s)
            { }

            @Override
            public void setReplyTo(String s)
            { }

            @Override
            public void setCorrelationId(Object o)
            { }

            @Override
            public void setMessageId(Object o)
            { }

            @Override
            public Header getHeader()
            { return null; }

            @Override
            public DeliveryAnnotations getDeliveryAnnotations()
            { return null; }

            @Override
            public MessageAnnotations getMessageAnnotations()
            { return null; }

            @Override
            public Properties getProperties()
            { return null; }

            @Override
            public ApplicationProperties getApplicationProperties()
            { return null; }

            @Override
            public Section getBody()
            { return null; }

            @Override
            public Footer getFooter()
            { return null; }

            @Override
            public void setHeader(Header header)
            { }

            @Override
            public void setDeliveryAnnotations(DeliveryAnnotations deliveryAnnotations)
            { }

            @Override
            public void setMessageAnnotations(MessageAnnotations messageAnnotations)
            { }

            @Override
            public void setProperties(Properties properties)
            { }

            @Override
            public void setApplicationProperties(ApplicationProperties applicationProperties)
            { }

            @Override
            public void setBody(Section section)
            { }

            @Override
            public void setFooter(Footer footer)
            { }

            @Override
            public int decode(byte[] bytes, int i, int i1)
            { return 0; }

            @Override
            public int encode(byte[] bytes, int i, int i1)
            {
                if (exceptionCount == 0)
                {
                    exceptionCount++;
                    throw new BufferOverflowException();
                }
                else
                {
                    return 0;
                }
            }

            @Override
            public void load(Object o)
            { }

            @Override
            public Object save()
            { return null; }

            @Override
            public String toAMQPFormat(Object o)
            { return null; }

            @Override
            public Object parseAMQPFormat(String s)
            { return null; }

            @Override
            public void setMessageFormat(MessageFormat messageFormat)
            { } @Override
            public MessageFormat getMessageFormat()
            { return null; }

            @Override
            public void clear()
            { }

            @Override
            public MessageError getError()
            { return null; }
        };

        sender = new Sender()
        {
            @Override
            public Record attachments()
            { return null; }

            @Override
            public EndpointState getLocalState()
            { return null; }

            @Override
            public EndpointState getRemoteState()
            { return null; }

            @Override
            public ErrorCondition getCondition()
            { return null; }

            @Override
            public void setCondition(ErrorCondition errorCondition)
            { }

            @Override
            public ErrorCondition getRemoteCondition()
            { return null; }

            @Override
            public void free()
            { }

            @Override
            public void open()
            { }

            @Override
            public void close()
            { }

            @Override
            public void setContext(Object o)
            { }

            @Override
            public Object getContext()
            { return null; }

            @Override
            public void offer(int i)
            { }

            @Override
            public int send(byte[] bytes, int i, int i1)
            { return 0; }

            @Override
            public void abort()
            { }

            @Override
            public String getName()
            { return null; }

            @Override
            public Delivery delivery(byte[] bytes)
            { return delivery; }

            @Override
            public Delivery delivery(byte[] bytes, int i, int i1)
            { return null; }

            @Override
            public Delivery head()
            { return null; }

            @Override
            public Delivery current()
            { return null; }

            @Override
            public boolean advance()
            { return false; }

            @Override
            public Source getSource()
            { return null; }

            @Override
            public org.apache.qpid.proton.amqp.transport.Target getTarget()
            { return null; }

            @Override
            public void setSource(Source source)
            { }

            @Override
            public void setTarget(org.apache.qpid.proton.amqp.transport.Target target)
            { }

            @Override
            public Source getRemoteSource()
            { return null; }

            @Override
            public org.apache.qpid.proton.amqp.transport.Target getRemoteTarget()
            { return null; }

            @Override
            public Link next(EnumSet<EndpointState> enumSet, EnumSet<EndpointState> enumSet1)
            { return null; }

            @Override
            public int getCredit()
            { return 1; }

            @Override
            public int getQueued()
            { return 0; }

            @Override
            public int getUnsettled()
            { return 0; }

            @Override
            public Session getSession()
            { return session; }

            @Override
            public SenderSettleMode getSenderSettleMode()
            { return null; }

            @Override
            public void setSenderSettleMode(SenderSettleMode senderSettleMode)
            { }

            @Override
            public SenderSettleMode getRemoteSenderSettleMode()
            { return null; }

            @Override
            public ReceiverSettleMode getReceiverSettleMode()
            { return null; }

            @Override
            public void setReceiverSettleMode(ReceiverSettleMode receiverSettleMode)
            { }

            @Override
            public ReceiverSettleMode getRemoteReceiverSettleMode()
            { return null; }

            @Override
            public void setRemoteSenderSettleMode(SenderSettleMode senderSettleMode)
            { }

            @Override
            public int drained()
            { return 0; }

            @Override
            public int getRemoteCredit()
            { return 0; }

            @Override
            public boolean getDrain()
            { return false; }

            @Override
            public void detach()
            { }

            @Override
            public boolean detached()
            { return false; }
        };

        event = new Event()
        {

            @Override public Event.Type getType()
            { throw new UnsupportedOperationException(exceptionMessage); }

            @Override public Object getContext()
            { throw new UnsupportedOperationException(exceptionMessage); }

            @Override public void dispatch(Handler hndlr) throws HandlerException
            { throw new UnsupportedOperationException(exceptionMessage); }

            @Override public Connection getConnection()
            { return connection; }

            @Override public Session getSession()
            { throw new UnsupportedOperationException(exceptionMessage); }

            @Override
            public Link getLink()
            { return sender; }

            @Override
            public Delivery getDelivery()
            { throw new UnsupportedOperationException(exceptionMessage); }

            @Override
            public Transport getTransport()
            { throw new UnsupportedOperationException(exceptionMessage); }

            @Override
            public Reactor getReactor()
            { throw new UnsupportedOperationException(exceptionMessage); }

            @Override
            public Selectable getSelectable()
            { throw new UnsupportedOperationException(exceptionMessage); }

            @Override
            public Task getTask()
            { throw new UnsupportedOperationException(exceptionMessage);}

            @Override
            public Event copy()
            { throw new UnsupportedOperationException(exceptionMessage); }

            @Override
            public Record attachments()
            { throw new UnsupportedOperationException(exceptionMessage); }
        };
    }
}
