/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iothub.transport.amqps;

import org.apache.qpid.proton.Proton;
import org.apache.qpid.proton.amqp.Binary;
import org.apache.qpid.proton.amqp.messaging.Data;
import org.apache.qpid.proton.amqp.messaging.Properties;
import org.apache.qpid.proton.amqp.messaging.Section;
import org.apache.qpid.proton.amqp.messaging.Target;
import org.apache.qpid.proton.amqp.transport.ErrorCondition;
import org.apache.qpid.proton.engine.*;
import org.apache.qpid.proton.reactor.Handshaker;

import java.nio.BufferOverflowException;

/**
 * Instance of the QPID-Proton-J BaseHandler class overriding the events needed to handle sending operations. Contains
 * and sets connection parameters, maintains the layers of the AMQP protocol and creates and sets SASL authentication
 * for the transport.
 */
public class AmqpsSenderHandler extends BaseHandler {
    public static final String SEND_TAG = "sender";
    public static final String SEND_PORT = ":5671";
    public static final String ENDPOINT_FORMAT = "/devices/%s/messages/events";

    private final String hostName;
    private final String shortHostName;
    private final String userName;
    private final String sasToken;
    private final String endpoint;
    private final String deviceID;

    protected org.apache.qpid.proton.message.Message message;

    private int nextTag = 0;

    /**
     * Constructor to set up connection parameters and initialize handshaker for the transport.
     * @param hostName address string of the IoT Hub (example: AAA.BBB.CCC)
     * @param userName username string to use SASL authentication (example: user@sas.[hostName])
     * @param deviceID ID from which this senderHandler communicates
     * @param sasToken SAS token string
     */
    public AmqpsSenderHandler(String hostName, String userName, String deviceID, String sasToken){
        // Codes_SRS_AMQPSSENDERHANDLER_14_001: [The constructor shall throw a new IllegalArgumentException if any of the input parameters is null or empty.]
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

        // Codes_SRS_AMQPSSENDERHANDLER_14_002: [The constructor shall copy all input parameters to private member variables for event processing.]
        // Codes_SRS_AMQPSSENDERHANDLER_14_003: [The constructor shall concatenate the host name with the port.]
        this.hostName = hostName + SEND_PORT;
        this.shortHostName = hostName;
        this.userName = userName;
        this.sasToken = sasToken;
        this.deviceID = deviceID;

        // Codes_SRS_AMQPSSENDERHANDLER_14_004: [The constructor shall initialize the endpoint private member variable using the ENDPOINT_FORMAT and deviceID.]
        this.endpoint = String.format(ENDPOINT_FORMAT, this.deviceID);

        // Codes_SRS_AMQPSSENDERHANDLER_14_005: [The constructor shall initialize a new Handshaker (Proton) object to handle communication handshake.]
        add(new Handshaker());
    }

    /**
     * Create Proton message using given content byte array
     * @param content
     */
    public void createBinaryMessage(byte[] content){
        this.createBinaryMessage(content, null);
    }

    /**
     * Create Proton message using given content byte array and messageId
     * @param content
     * @param messageId
     */
    public void createBinaryMessage(byte[] content, Object messageId){
        // Codes_SRS_AMQPSSENDERHANDLER_14_006: [The function shall create a new Message (Proton) object.]
        this.message = Proton.message();

        // Codes_SRS_AMQPSSENDERHANDLER_14_007: [The function shall set the ‘to’ property on the Message object using the created event path.]
        // Codes_SRS_AMQPSSENDERHANDLER_14_026: [The function shall set the ‘messageId’ property on the Message object if the messageId is not null.]
        Properties properties = new Properties();
        properties.setTo(this.endpoint);
        if(messageId != null){
            properties.setMessageId(messageId);
        }
        message.setProperties(properties);

        // Codes_SRS_AMQPSSENDERHANDLER_14_008: [The function shall create a Binary (Proton) object from the content array.]
        Binary binary = new Binary(content);
        // Codes_SRS_AMQPSSENDERHANDLER_14_009: [The function shall create a data Section (Proton) object from the Binary.]
        Section section = new Data(binary);
        // Codes_SRS_AMQPSSENDERHANDLER_14_010: [The function shall set the Message body to the created data section.]
        message.setBody(section);
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
     * Event handler for the connection bound event. Sets Sasl authentication and proper authentication mode.
     * @param event Proton Event object
     */
    @Override
    public void onConnectionBound(Event event){
        // Codes_SRS_AMQPSSENDERHANDLER_14_011: [The event handler shall get the Transport (Proton) object from the event.]
        Transport transport = event.getConnection().getTransport();
        if(transport != null) {
            // Codes_SRS_AMQPSSENDERHANDLER_14_012: [The event handler shall set the SASL_PLAIN authentication on the transport using the given user name and sas token.]
            Sasl sasl = transport.sasl();
            sasl.plain(this.userName, this.sasToken);

            // Codes_SRS_AMQPSSENDERHANDLER_14_013: [The event handler shall set ANONYMOUS_PEER authentication mode on the domain of the Transport.]
            SslDomain domain = makeDomain(SslDomain.Mode.CLIENT);
            domain.setPeerAuthentication(SslDomain.VerifyMode.ANONYMOUS_PEER);
            transport.ssl(domain);
        }
    }

    /**
     * Event handler for the connection init event
     * @param event Proton Event object
     */
    @Override
    public void onConnectionInit(Event event){
        // Codes_SRS_AMQPSSENDERHANDLER_14_014: [The event handler shall get the Connection (Proton) object from the event handler and set the host name on the connection.]
        Connection conn = event.getConnection();
        conn.setHostname(this.hostName);

        // Every session or link could have their own handler(s) if we
        // wanted simply by adding the handler to the given session
        // or link

        // Codes_SRS_AMQPSSENDERHANDLER_14_015: [The event handler shall create a Session (Proton) object from the connection.]
        Session ssn = conn.session();

        // If a link doesn't have an event handler, the events go to
        // its parent session. If the session doesn't have a handler
        // the events go to its parent connection. If the connection
        // doesn't have a handler, the events go to the reactor.

        // Codes_SRS_AMQPSSENDERHANDLER_14_016: [The event handler shall create a Sender (Proton) object and set the protocol tag on it to a predefined constant.]
        Sender snd = ssn.sender(SEND_TAG);

        // Codes_SRS_AMQPSSENDERHANDLER_14_017: [The event handler shall open the Connection, Session, and Sender objects.]
        conn.open();
        ssn.open();
        snd.open();
    }

    /**
     * Event handler for the link init event. Sets the proper target address on the link.
     * @param event Proton Event object
     */
    @Override
    public void onLinkInit(Event event){
        // Codes_SRS_AMQPSSENDERHANDLER_14_018: [The event handler shall create a new Target (Proton) object using the given endpoint address.]
        // Codes_SRS_AMQPSSENDERHANDLER_14_019: [The event handler shall get the Link (Proton) object and set its target.]
        Link link = event.getLink();
        Target t = new Target();
        t.setAddress(this.endpoint);
        link.setTarget(t);
    }

    /**
     * Event handler for the link flow event. Handles sending of a single message.
     * @param event Proton Event object
     */
    @Override
    public void onLinkFlow(Event event){
        // Codes_SRS_AMQPSSENDERHANDLER_14_020: [The event handler shall get the Sender (Proton) object from the link.]
        Sender snd = (Sender)event.getLink();
        // Codes_SRS_AMQPSSENDERHANDLER_14_021: [The event handler shall encode the message and copy the contents to the byte buffer.]
        if (snd.getCredit() > 0) {
            byte[] msgData = new byte[1024];
            int length;
            while(true){
                try{
                    length = message.encode(msgData, 0, msgData.length);
                    break;
                } catch(BufferOverflowException e) {
                    msgData = new byte[msgData.length * 2];
                }
            }
            // Codes_SRS_AMQPSSENDERHANDLER_14_022: [The event handler shall set the delivery tag on the Sender (Proton) object.]
            byte[] tag = String.valueOf(nextTag++).getBytes();
            snd.delivery(tag);
            // Codes_SRS_AMQPSSENDERHANDLER_14_023: [The event handler shall send the encoded bytes.]
            snd.send(msgData, 0, length);
            //Receiving side should acknowledge
            //dlv.settle();
            // Codes_SRS_AMQPSSENDERHANDLER_14_024: [The event handler shall close the Sender, Session, and Connection objects.]
            snd.advance();
            snd.close();
            snd.getSession().close();
            snd.getSession().getConnection().close();
        }
    }

    /**
     * Event handler for the transport error event.
     * @param event Proton Event object
     */
    @Override
    public void onTransportError(Event event){
        // Codes_SRS_AMQPSSENDERHANDLER_14_025: [The function shall log the error and description if there is a description accompanying the error.]
        ErrorCondition condition = event.getTransport().getCondition();
        if(condition != null){
            System.err.println("Error: "+condition.toString());
            System.err.println("Error: "+condition.getDescription());
        } else {
            System.err.println("Err (no description returned).");
        }
    }
}
