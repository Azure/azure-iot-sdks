/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.transport.amqps;

import com.microsoft.azure.iot.service.sdk.IotHubServiceClientProtocol;
import com.microsoft.azure.iot.service.sdk.Message;
import com.microsoft.azure.iot.service.sdk.Tools;
import com.microsoft.azure.iot.service.transport.TransportUtils;
import com.microsoft.azure.iothub.ws.impl.WebSocketImpl;
import org.apache.qpid.proton.Proton;
import org.apache.qpid.proton.amqp.Binary;
import org.apache.qpid.proton.amqp.Symbol;
import org.apache.qpid.proton.amqp.messaging.*;
import org.apache.qpid.proton.engine.*;
import org.apache.qpid.proton.engine.impl.TransportInternal;
import org.apache.qpid.proton.engine.impl.TransportLayer;
import org.apache.qpid.proton.messenger.impl.Address;
import org.apache.qpid.proton.reactor.Handshaker;

import java.nio.BufferOverflowException;
import java.util.HashMap;
import java.util.Map;

/**
 * Instance of the QPID-Proton-J BaseHandler class to override
 * the events what are needed to handle the send operation
 * Contains and sets connection parameters (path, port, endpoint)
 * Maintains the layers of AMQP protocol (Link, Session, Connection, Transport)
 * Creates and sets SASL authentication for transport
 */
public class AmqpSendHandler extends BaseHandler
{
    public static final String SEND_TAG = "sender";
    public static final String SEND_PORT_AMQPS = ":5671";
    public static final String SEND_PORT_AMQPS_WS = ":443";
    public static final String ENDPOINT = "/messages/devicebound";
    public static final String DEVICE_PATH_FORMAT = "/devices/%s/messages/devicebound";
    public static final String WEBSOCKET_PATH = "/$iothub/websocket";
    public static final String WEBSOCKET_SUB_PROTOCOL = "AMQPWSB10";

    protected final String hostName;
    protected final String userName;
    protected final String sasToken;
    protected org.apache.qpid.proton.message.Message protonMessage;
    private int nextTag = 0;

    protected final IotHubServiceClientProtocol iotHubServiceClientProtocol;
    protected final String webSocketHostName;

    /**
     * Constructor to set up connection parameters and initialize handshaker for transport
     *
     * @param hostName The address string of the service (example: AAA.BBB.CCC)
     * @param userName The username string to use SASL authentication (example: user@sas.service)
     * @param sasToken The SAS token string
     */
    public AmqpSendHandler(String hostName, String userName, String sasToken, IotHubServiceClientProtocol iotHubServiceClientProtocol)
    {
        // Codes_SRS_SERVICE_SDK_JAVA_AMQPSENDHANDLER_12_001: [The constructor shall throw IllegalArgumentException if any of the input parameter is null or empty]
        if (Tools.isNullOrEmpty(hostName))
        {
            throw new IllegalArgumentException("hostName can not be null or empty");
        }
        if (Tools.isNullOrEmpty(userName))
        {
            throw new IllegalArgumentException("userName can not be null or empty");
        }
        if (Tools.isNullOrEmpty(sasToken))
        {
            throw new IllegalArgumentException("sasToken can not be null or empty");
        }

        this.iotHubServiceClientProtocol = iotHubServiceClientProtocol;
        this.webSocketHostName = hostName;
        if (this.iotHubServiceClientProtocol == IotHubServiceClientProtocol.AMQPS_WS)
        {
            this.hostName = hostName + SEND_PORT_AMQPS_WS;
        }
        else
        {
            this.hostName = hostName + SEND_PORT_AMQPS;
        }

        // Codes_SRS_SERVICE_SDK_JAVA_AMQPSENDHANDLER_12_002: [The constructor shall copy all input parameters to private member variables for event processing]
        // Codes_SRS_SERVICE_SDK_JAVA_AMQPSENDHANDLER_12_003: [The constructor shall concatenate the host name with the port]
        this.userName = userName;
        this.sasToken = sasToken;

        // Add a child handler that performs some default handshaking behaviour.
        // Codes_SRS_SERVICE_SDK_JAVA_AMQPSENDHANDLER_12_004: [The constructor shall initialize a new Handshaker (Proton) object to handle communication handshake]
        add(new Handshaker());
    }

    /**
     * Create "to" parameter for AMQP message to address the device
     * @param deviceId device name string
     * @return full path tot he device
     */
    private String buildToDevicePath(String deviceId)
    {
        return String.format(DEVICE_PATH_FORMAT, deviceId);
    }

    /**
     * Create Proton message from deviceId and content string
     * @param deviceId The device name string
     * @param message The message to be sent
     */
    public void createProtonMessage(String deviceId, Message message)
    {
        // Codes_SRS_SERVICE_SDK_JAVA_AMQPSENDHANDLER_12_005: [The function shall create a new Message (Proton) object]
        this.protonMessage = Proton.message();

        // Codes_SRS_SERVICE_SDK_JAVA_AMQPSENDHANDLER_12_006: [The function shall set
        // the standard properties on the Proton Message object]
        Properties properties = new Properties();
        properties.setMessageId(message.getMessageId());
        properties.setTo(buildToDevicePath(deviceId));
        properties.setAbsoluteExpiryTime(message.getExpiryTimeUtc());
        properties.setCorrelationId(message.getCorrelationId());
        if (message.getUserId() != null)
        {
            properties.setUserId(new Binary(message.getUserId().getBytes()));
        }
        this.protonMessage.setProperties(properties);

        // Codes_SRS_SERVICE_SDK_JAVA_AMQPSENDHANDLER_12_023: [The function shall set
        // the application properties on the Proton Message object]
        if (message.getProperties() != null && message.getProperties().size() > 0)
        {
            Map<String, String> applicationPropertiesMap = new HashMap<>(message.getProperties().size());
            for(Map.Entry<String, String> entry : message.getProperties().entrySet())
            {
                applicationPropertiesMap.put(entry.getKey(), entry.getValue());
            }
            ApplicationProperties applicationProperties = new ApplicationProperties(applicationPropertiesMap);
            this.protonMessage.setApplicationProperties(applicationProperties);
        }

        // Codes_SRS_SERVICE_SDK_JAVA_AMQPSENDHANDLER_12_007: [The function shall create a Binary (Proton) object from the content string]
        Binary binary = new Binary(message.getBytes());
        // Codes_SRS_SERVICE_SDK_JAVA_AMQPSENDHANDLER_12_008: [The function shall create a data Section (Proton) object from the Binary]
        Section section = new Data(binary);
        // Codes_SRS_SERVICE_SDK_JAVA_AMQPSENDHANDLER_12_009: [The function shall set the Message body to the created data section]
        this.protonMessage.setBody(section);
    }

    /**
     * Create Proton SslDomain object from Address using the given Ssl mode
     * @param address The proton address object
     * @param mode The proton enum value of requested Ssl mode
     * @return The created Ssl domain
     */
    private SslDomain makeDomain(Address address, SslDomain.Mode mode)
    {
        SslDomain domain = Proton.sslDomain();
        domain.init(mode);

        return domain;
    }

    /**
     * Event handler for the connection bound event
     * @param event The proton event object
     */
    @Override
    public void onConnectionBound(Event event)
    {
        // Codes_SRS_SERVICE_SDK_JAVA_AMQPSENDHANDLER_12_010: [The event handler shall set the SASL PLAIN authentication on the Transport using the given user name and sas token]
        // Codes_SRS_SERVICE_SDK_JAVA_AMQPSENDHANDLER_12_011: [The event handler shall set ANONYMUS_PEER authentication mode on the domain of the Transport]
        Transport transport = event.getConnection().getTransport();
        if (transport != null)
        {
            if (this.iotHubServiceClientProtocol == IotHubServiceClientProtocol.AMQPS_WS)
            {
                WebSocketImpl webSocket = new WebSocketImpl();
                webSocket.configure(this.webSocketHostName, WEBSOCKET_PATH, 0, WEBSOCKET_SUB_PROTOCOL, null, null);
                ((TransportInternal)transport).addTransportLayer(webSocket);
            }
            Sasl sasl = transport.sasl();
            sasl.plain(this.userName, this.sasToken);

            Address address = new Address(this.hostName);

            SslDomain domain = makeDomain(address, SslDomain.Mode.CLIENT);
            domain.setPeerAuthentication(SslDomain.VerifyMode.ANONYMOUS_PEER);
            Ssl ssl = transport.ssl(domain);
        }
    }

    /**
     * Event handler for the connection init event
     * @param event The proton event object
     */
    @Override
    public void onConnectionInit(Event event)
    {
        // Codes_SRS_SERVICE_SDK_JAVA_AMQPSENDHANDLER_12_012: [The event handler shall set the host name on the connection]
        Connection conn = event.getConnection();
        conn.setHostname(hostName);

        // Every session or link could have their own handler(s) if we
        // wanted simply by adding the handler to the given session
        // or link

        // Codes_SRS_SERVICE_SDK_JAVA_AMQPSENDHANDLER_12_013: [The event handler shall create a Session (Proton) object from the connection]
        Session ssn = conn.session();

        // If a link doesn't have an event handler, the events go to
        // its parent session. If the session doesn't have a handler
        // the events go to its parent connection. If the connection
        // doesn't have a handler, the events go to the reactor.

        // Codes_SRS_SERVICE_SDK_JAVA_AMQPSENDHANDLER_12_014: [The event handler shall create a Sender (Proton) object and set the protocol tag on it to a predefined constant]
        // Codes_SRS_SERVICE_SDK_JAVA_AMQPSENDHANDLER_15_023: [The Sender object shall have the properties set to service client version identifier.]
        Map<Symbol, Object> properties = new HashMap<>();
        properties.put(Symbol.getSymbol(TransportUtils.versionIdentifierKey), TransportUtils.javaServiceClientIdentifier + TransportUtils.serviceVersion);
        Sender snd = ssn.sender(SEND_TAG);
        snd.setProperties(properties);

        // Codes_SRS_SERVICE_SDK_JAVA_AMQPSENDHANDLER_12_015: [The event handler shall open the Connection, the Session and the Sender object]
        conn.open();
        ssn.open();
        snd.open();
    }

    /**
     * Event handler for the link init event
     * @param event The proton event object
     */
    @Override
    public void onLinkInit(Event event)
    {
        // Codes_SRS_SERVICE_SDK_JAVA_AMQPSENDHANDLER_12_016: [The event handler shall create a new Target (Proton) object using the given endpoint address]
        // Codes_SRS_SERVICE_SDK_JAVA_AMQPSENDHANDLER_12_017: [The event handler shall get the Link (Proton) object and set its target to the created Target (Proton) object]
        Link link = event.getLink();
        Target t = new Target();
        t.setAddress(ENDPOINT);
        link.setTarget(t);
    }

    /**
     * Event handler for the link flow event
     * @param event The proton event object
     */
    @Override
    public void onLinkFlow(Event event)
    {
        // Codes_SRS_SERVICE_SDK_JAVA_AMQPSENDHANDLER_12_018: [The event handler shall get the Sender (Proton) object from the link]
        Sender snd = (Sender)event.getLink();
        // Codes_SRS_SERVICE_SDK_JAVA_AMQPSENDHANDLER_12_019: [The event handler shall encode the message and copy to the byte buffer]
        if (snd.getCredit() > 0)
        {
            byte[] msgData = new byte[1024];
            int length;
            while(true)
            {
                try
                {
                    length = this.protonMessage.encode(msgData, 0, msgData.length);
                    break;
                } catch(BufferOverflowException e)
                {
                    msgData = new byte[msgData.length * 2];
                }
            }
            // Codes_SRS_SERVICE_SDK_JAVA_AMQPSENDHANDLER_12_020: [The event handler shall set the delivery tag on the Sender (Proton) object]
            byte[] tag = String.valueOf(nextTag++).getBytes();
            Delivery dlv = snd.delivery(tag);
            // Codes_SRS_SERVICE_SDK_JAVA_AMQPSENDHANDLER_12_021: [The event handler shall send the encoded bytes]
            snd.send(msgData, 0, length);
            // Codes_SRS_SERVICE_SDK_JAVA_AMQPSENDHANDLER_12_022: [The event handler shall close the Sender, Session and Connection]
            dlv.settle();
            snd.advance();
            snd.close();
            snd.getSession().close();
            snd.getSession().getConnection().close();
        }
    }
}
