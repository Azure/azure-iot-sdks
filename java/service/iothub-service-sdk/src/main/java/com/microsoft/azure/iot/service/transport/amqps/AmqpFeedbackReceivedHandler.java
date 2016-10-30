/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.transport.amqps;

import com.microsoft.azure.iot.service.sdk.IotHubServiceClientProtocol;
import com.microsoft.azure.iot.service.transport.TransportUtils;
import com.microsoft.azure.iothub.ws.impl.WebSocketImpl;
import org.apache.qpid.proton.Proton;
import org.apache.qpid.proton.amqp.Symbol;
import org.apache.qpid.proton.amqp.messaging.Source;
import org.apache.qpid.proton.amqp.messaging.Target;
import org.apache.qpid.proton.engine.*;
import org.apache.qpid.proton.engine.impl.TransportInternal;
import org.apache.qpid.proton.engine.impl.TransportLayer;
import org.apache.qpid.proton.reactor.FlowController;
import org.apache.qpid.proton.reactor.Handshaker;

import java.util.HashMap;
import java.util.Map;

/**
 * Instance of the QPID-Proton-J BaseHandler class to override
 * the events what are needed to handle the receive operation
 * Contains and sets connection parameters (path, port, endpoint)
 * Maintains the layers of AMQP protocol (Link, Session, Connection, Transport)
 * Creates and sets SASL authentication for transport
 */
public class AmqpFeedbackReceivedHandler extends BaseHandler
{
    public static final String RECEIVE_TAG = "receiver";
    public static final String SEND_PORT_AMQPS = ":5671";
    public static final String SEND_PORT_AMQPS_WS = ":443";
    public static final String ENDPOINT = "/messages/servicebound/feedback";
    public static final String WEBSOCKET_PATH = "/$iothub/websocket";
    public static final String WEBSOCKET_SUB_PROTOCOL = "AMQPWSB10";

    private final String hostName;
    private final String userName;
    private final String sasToken;

    protected final IotHubServiceClientProtocol iotHubServiceClientProtocol;
    protected final String webSocketHostName;

    private AmqpFeedbackReceivedEvent amqpFeedbackReceivedEvent;

    /**
     * Constructor to set up connection parameters and initialize
     * handshaker and flow controller for transport
     * @param hostName The address string of the service (example: AAA.BBB.CCC)
     * @param userName The username string to use SASL authentication (example: user@sas.service)
     * @param sasToken The SAS token string
     * @param amqpFeedbackReceivedEvent callback to delegate the received message to the user API
     */
    public AmqpFeedbackReceivedHandler(String hostName, String userName, String sasToken, IotHubServiceClientProtocol iotHubServiceClientProtocol, AmqpFeedbackReceivedEvent amqpFeedbackReceivedEvent)
    {
        // Codes_SRS_SERVICE_SDK_JAVA_AMQPFEEDBACKRECEIVEDHANDLER_12_001: [The constructor shall copy all input parameters to private member variables for event processing]
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

        this.userName = userName;
        this.sasToken = sasToken;
        this.amqpFeedbackReceivedEvent = amqpFeedbackReceivedEvent;

        // Add a child handler that performs some default handshaking
        // behaviour.

        // Codes_SRS_SERVICE_SDK_JAVA_AMQPFEEDBACKRECEIVEDHANDLER_12_002: [The constructor shall initialize a new Handshaker (Proton) object to handle communication handshake]
        add(new Handshaker());
        // Codes_SRS_SERVICE_SDK_JAVA_AMQPFEEDBACKRECEIVEDHANDLER_12_003: [The constructor shall initialize a new FlowController (Proton) object to handle communication handshake]
        add(new FlowController());
    }

    /**
     * Create Proton SslDomain object from Address using the given Ssl mode
     * @param mode Proton enum value of requested Ssl mode
     * @return The created Ssl domain
     */
    private SslDomain makeDomain(SslDomain.Mode mode)
    {
        SslDomain domain = Proton.sslDomain();
        domain.init(mode);

        return domain;
    }

    /**
     * Event handler for the on delivery event
     * @param event The proton event object
     */
    @Override
    public void onDelivery(Event event)
    {
        // Codes_SRS_SERVICE_SDK_JAVA_AMQPFEEDBACKRECEIVEDHANDLER_12_004: [The event handler shall get the Link, Receiver and Delivery (Proton) objects from the event]
        Receiver recv = (Receiver)event.getLink();
        Delivery delivery = recv.current();
        if (delivery.isReadable() && !delivery.isPartial())
        {
            // Codes_SRS_SERVICE_SDK_JAVA_AMQPFEEDBACKRECEIVEDHANDLER_12_005: [The event handler shall read the received buffer]            int size = delivery.pending();
            int size = delivery.pending();
            byte[] buffer = new byte[size];
            int read = recv.recv(buffer, 0, buffer.length);
            recv.advance();

            // Codes_SRS_SERVICE_SDK_JAVA_AMQPFEEDBACKRECEIVEDHANDLER_12_006: [The event handler shall create a Message (Proton) object from the decoded buffer]
            org.apache.qpid.proton.message.Message msg = Proton.message();
            msg.decode(buffer, 0, read);

            // Codes_SRS_SERVICE_SDK_JAVA_AMQPFEEDBACKRECEIVEDHANDLER_12_007: [The event handler shall close the Session and Connection (Proton)]
            recv.getSession().close();
            recv.getSession().getConnection().close();

            // Codes_SRS_SERVICE_SDK_JAVA_AMQPFEEDBACKRECEIVEDHANDLER_12_008: [The event handler shall call the FeedbackReceived callback if it has been initialized]
            if (amqpFeedbackReceivedEvent != null)
            {
                amqpFeedbackReceivedEvent.onFeedbackReceived(msg.getBody().toString());
            }
        }
    }

    @Override
    public void onConnectionBound(Event event)
    {
        // Codes_SRS_SERVICE_SDK_JAVA_AMQPFEEDBACKRECEIVEDHANDLER_12_009: [The event handler shall set the SASL PLAIN authentication on the Transport using the given user name and sas token]
        // Codes_SRS_SERVICE_SDK_JAVA_AMQPFEEDBACKRECEIVEDHANDLER_12_010: [The event handler shall set ANONYMUS_PEER authentication mode on the domain of the Transport]
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

            SslDomain domain = makeDomain(SslDomain.Mode.CLIENT);
            domain.setPeerAuthentication(SslDomain.VerifyMode.ANONYMOUS_PEER);
            Ssl ssl = transport.ssl(domain);
        }
    }

    @Override
    public void onConnectionInit(Event event)
    {
        // Codes_SRS_SERVICE_SDK_JAVA_AMQPFEEDBACKRECEIVEDHANDLER_12_011: [The event handler shall set the host name on the connection]
        Connection conn = event.getConnection();
        conn.setHostname(hostName);

        // Every session or link could have their own handler(s) if we
        // wanted simply by adding the handler to the given session
        // or link

        // Codes_SRS_SERVICE_SDK_JAVA_AMQPFEEDBACKRECEIVEDHANDLER_12_012: [The event handler shall create a Session (Proton) object from the connection]
        Session ssn = conn.session();

        // If a link doesn't have an event handler, the events go to
        // its parent session. If the session doesn't have a handler
        // the events go to its parent connection. If the connection
        // doesn't have a handler, the events go to the reactor.

        // Codes_SRS_SERVICE_SDK_JAVA_AMQPFEEDBACKRECEIVEDHANDLER_12_013: [The event handler shall create a Receiver (Proton) object and set the protocol tag on it to a predefined constant]
        // Codes_SRS_SERVICE_SDK_JAVA_AMQPFEEDBACKRECEIVEDHANDLER_15_017: [The Receiver object shall have the properties set to service client version identifier.]
        Map<Symbol, Object> properties = new HashMap<>();
        properties.put(Symbol.getSymbol(TransportUtils.versionIdentifierKey), TransportUtils.javaServiceClientIdentifier + TransportUtils.serviceVersion);
        Receiver receiver = ssn.receiver(RECEIVE_TAG);
        receiver.setProperties(properties);

        // Codes_SRS_SERVICE_SDK_JAVA_AMQPFEEDBACKRECEIVEDHANDLER_12_014: [The event handler shall open the Connection, the Session and the Receiver object]
        conn.open();
        ssn.open();
        receiver.open();
    }

    @Override
    public void onLinkInit(Event event)
    {
        // Codes_SRS_SERVICE_SDK_JAVA_AMQPFEEDBACKRECEIVEDHANDLER_12_015: [The event handler shall create a new Target (Proton) object using the given endpoint address]
        // Codes_SRS_SERVICE_SDK_JAVA_AMQPFEEDBACKRECEIVEDHANDLER_12_016: [The event handler shall get the Link (Proton) object and set its target to the created Target (Proton) object]
        Link link = event.getLink();
        Target t = new Target();
        t.setAddress(ENDPOINT);
        Source source = new Source();
        source.setAddress(ENDPOINT);
        link.setTarget(t);
        link.setSource(source);
    }
}