/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iothub.transport.amqps;

import org.apache.qpid.proton.Proton;
import org.apache.qpid.proton.amqp.messaging.Source;
import org.apache.qpid.proton.engine.*;
import org.apache.qpid.proton.message.Message;
import org.apache.qpid.proton.reactor.FlowController;
import org.apache.qpid.proton.reactor.Handshaker;

/**
 * Instance of the QPID-Proton-J BaseHandler class that overrides the events needed to handle receiving operations.
 * Contains and sets connection parameters, maintains the layers of the AMQP protocol and creates and sets SASL
 * authentication for the transport.
 */
public class AmqpsReceiverHandler extends BaseHandler{
    public static final String RECEIVE_TAG = "receiver";
    public static final String SEND_PORT = ":5671";
    public static final String ENDPOINT_FORMAT = "/devices/%s/messages/devicebound";


    private final String hostName;
    private final String userName;
    private final String sasToken;
    private final String deviceID;
    private final String endpoint;
    private final AmqpsReceiver parentReceiver;

    /**
     * Constructor to set up connection parameters initialize handshaker and flow controller for the transport.
     * @param hostName address string of the IoT Hub (example: AAA.BBB.CCC)
     * @param userName username string to use SASL authentication (example: user@sas.[hostName])
     * @param deviceID ID from which this sender communicates
     * @param sasToken  SAS token string
     * @param parentReceiver parent AmqpsReceiver object
     */
    public AmqpsReceiverHandler(String hostName, String userName, String deviceID, String sasToken, AmqpsReceiver parentReceiver){
        // Codes_SRS_AMQPSRECEIVERHANDLER_14_001: [The constructor shall throw a new IllegalArgumentException if any of the input parameters is null or empty.]
        if(hostName == null || hostName.length() == 0)
        {
            throw new IllegalArgumentException("hostName cannot be null or empty.");
        }
        if(userName == null || userName.length() == 0)
        {
            throw new IllegalArgumentException("userName cannot be null or empty.");
        }
        if (deviceID == null || deviceID.length() == 0)
        {
            throw new IllegalArgumentException("deviceID cannot be null or empty.");
        }
        if(sasToken == null || sasToken.length() == 0)
        {
            throw new IllegalArgumentException("sasToken cannot be null or empty.");
        }
        if(parentReceiver == null)
        {
            throw new IllegalArgumentException("The AmqpsReceiver cannot be null.");
        }

        // Codes_SRS_AMQPSRECEIVERHANDLER_14_002: [The constructor shall copy all input parameters to private member variables for event processing.]
        // Codes_SRS_AMQPSRECEIVERHANDLER_14_003: [The constructor shall concatenate the host name with the port.]
        this.hostName = hostName + SEND_PORT;
        this.userName = userName;
        this.deviceID = deviceID;
        this.sasToken = sasToken;
        this.parentReceiver = parentReceiver;

        // Codes_SRS_AMQPSRECEIVERHANDLER_14_004: [The constructor shall initialize the endpoint private member variable using the ENDPOINT_FORMAT and deviceID.]
        this.endpoint = String.format(ENDPOINT_FORMAT, this.deviceID);

        // Codes_SRS_AMQPSRECEIVERHANDLER_14_005: [The constructor shall initialize a new Handshaker (Proton) object to handle communication handshake.]
        // Codes_SRS_AMQPSRECEIVERHANDLER_14_006: [The constructor shall initialize a new FlowController (Proton) object to handle communication handshake.]
        add(new Handshaker());
        add(new FlowController());
    }

    /**
     * Create Proton SslDomain object from Address using the given Ssl mode
     * @param mode Proton enum value of requested Ssl mode
     * @return the created Ssl domain
     */
    private SslDomain makeDomain(SslDomain.Mode mode){
        SslDomain domain = Proton.sslDomain();
        domain.init(mode);

        return domain;
    }

    /**
     * Event handler for the delivery event. Takes the message data, creates a new AmqpsMessage and gives the message
     * up to the parent receiver.
     * @param event Proton Event object
     */
    @Override
    public void onDelivery(Event event){
        // Codes_SRS_AMQPSRECEIVERHANDLER_14_007: [The event handler shall get the Receiver and Delivery (Proton) objects from the event.]
        Receiver recv = (Receiver) event.getLink();
        Delivery delivery = recv.current();
        if (delivery.isReadable() && !delivery.isPartial()) {
            // Codes_SRS_AMQPSRECEIVERHANDLER_14_008: [The event handler shall read the received buffer.]
            int size = delivery.pending();
            byte[] buffer = new byte[size];
            int read = recv.recv(buffer, 0, buffer.length);
            recv.advance();

            // Codes_SRS_AMQPSRECEIVERHANDLER_14_009: [The event handler shall create an AmqpsMessage object from the decoded buffer.]
            // Codes_SRS_AMQPSRECEIVERHANDLER_14_010: [The event handler shall set the AmqpsMessage Delivery (Proton) object.]
            Message msg = new AmqpsMessage();
            ((AmqpsMessage) msg).setDelivery(delivery);
            msg.decode(buffer, 0, read);

            // Codes_SRS_AMQPSRECEIVERHANDLER_14_011: [The event handler shall give the message back to its parent AmqpsReceiver.]
            this.parentReceiver.addMessage(msg);
        }
    }

    /**
     * Event handler for the connection bound event. Sets Sasl authentication and proper authentication mode.
     * @param event Proton Event object
     */
    @Override
    public void onConnectionBound(Event event){
        // Codes_SRS_AMQPSRECEIVERHANDLER_14_012: [The event handler shall get the Transport (Proton) object from the event.]
        Transport transport = event.getConnection().getTransport();
        if(transport != null){
            // Codes_SRS_AMQPSRECEIVERHANDLER_14_013: [The event handler shall set the SASL_PLAIN authentication on the transport using the given user name and sas token.]
            Sasl sasl = transport.sasl();
            sasl.plain(this.userName, this.sasToken);

            // Codes_SRS_AMQPSRECEIVERHANDLER_14_014: [The event handler shall set ANONYMOUS_PEER authentication mode on the domain of the Transport.]
            SslDomain domain = makeDomain(SslDomain.Mode.CLIENT);
            domain.setPeerAuthentication(SslDomain.VerifyMode.ANONYMOUS_PEER);
            Ssl ssl = transport.ssl(domain);
        }
    }

    /**
     * Event handler for the connection init event
     * @param event Proton Event object
     */
    @Override
    public void onConnectionInit(Event event){
        // Codes_SRS_AMQPSRECEIVERHANDLER_14_015: [The event handler shall get the Connection (Proton) object from the event handler and set the host name on the connection.]
        Connection conn = event.getConnection();
        conn.setHostname(this.hostName);

        // Codes_SRS_AMQPSRECEIVERHANDLER_14_016: [The event handler shall create a Session (Proton) object from the connection.]
        Session ssn = conn.session();

        // Codes_SRS_AMQPSRECEIVERHANDLER_14_017: [The event handler shall create a Receiver (Proton) object and set the protocol tag on it to a predefined constant.]
        Receiver receiver = ssn.receiver(RECEIVE_TAG);

        // Codes_SRS_AMQPSRECEIVERHANDLER_14_018: [The event handler shall open the Connection, Session, and Receiver objects.]
        conn.open();
        ssn.open();
        receiver.open();
    }

    /**
     * Event handler for the link init event. Sets the proper Source address on the link.
     * @param event Proton Event object
     */
    @Override
    public void onLinkInit(Event event){
        // Codes_SRS_AMQPSRECEIVERHANDLER_14_019: [The event handler shall create a new Source (Proton) object using the given endpoint address.]
        // Codes_SRS_AMQPSRECEIVERHANDLER_14_020: [The event handler shall get the Link (Proton) object and set its source to the created Source (Proton) object.]
        Link link = event.getLink();
        Source source = new Source();
        source.setAddress(this.endpoint);
        link.setSource(source);
    }

    /**
     * Event handler for the link local close event. If the link closes locally, close the session and connection.
     * @param event Proton Event object
     */
    @Override
    public void onLinkLocalClose(Event event){
        // Codes_SRS_AMQPSRECEIVERHANDLER_14_021: [The event handler shall close the Session and Connection (Proton) objects.]
        event.getSession().close();
        event.getSession().getConnection().close();
    }
}
