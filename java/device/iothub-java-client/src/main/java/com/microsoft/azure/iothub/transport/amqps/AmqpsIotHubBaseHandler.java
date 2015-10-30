/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iothub.transport.amqps;

import org.apache.qpid.proton.Proton;
import org.apache.qpid.proton.amqp.Binary;
import org.apache.qpid.proton.amqp.messaging.*;
import org.apache.qpid.proton.amqp.transport.ErrorCondition;
import org.apache.qpid.proton.engine.*;
import org.apache.qpid.proton.message.Message;
import org.apache.qpid.proton.reactor.FlowController;
import org.apache.qpid.proton.reactor.Handshaker;

import java.nio.BufferOverflowException;

public class AmqpsIotHubBaseHandler extends BaseHandler {
    public static final String SEND_TAG = "sender";
    public static final String RECEIVE_TAG = "receiver";
    public static final String PORT = ":5671";
    public static final String SEND_ENDPOINT_FORMAT = "/devices/%s/messages/events";
    public static final String RECEIVE_ENDPOINT_FORMAT = "/devices/%s/messages/devicebound";

    private final String hostName;
    private final String userName;
    private final String sasToken;
    private final String deviceID;
    private final String sendEndpoint;
    private final String receiveEndpoint;
    private final AmqpsComm parentComm;

    private Sender sender = null;
    private Receiver receiver = null;

    protected Message outgoingMessage;

    private int nextTag = 0;

    public AmqpsIotHubBaseHandler(String hostName, String userName, String sasToken, String deviceID, AmqpsComm parentComm){
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
        if(parentComm == null)
        {
            throw new IllegalArgumentException("The AmqpsComm cannot be null.");
        }

        this.hostName = hostName + PORT;
        this.userName = userName;
        this.deviceID = deviceID;
        this.sasToken = sasToken;
        this.parentComm = parentComm;

        this.sendEndpoint = String.format(SEND_ENDPOINT_FORMAT, this.deviceID);
        this.receiveEndpoint = String.format(RECEIVE_ENDPOINT_FORMAT, this.deviceID);

        add(new Handshaker());
        add(new FlowController());
    }

    //==============================================================================
    //Receiver Event Handlers
    //==============================================================================

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
            this.parentComm.addMessage(msg);
        }
    }

    /**
     * Event handler for the link local close event. If the link closes locally, close the session and connection.
     * @param event Proton Event object
     */
    @Override
    public void onLinkLocalClose(Event event){
        // Codes_SRS_AMQPSRECEIVERHANDLER_14_021: [The event handler shall close the Session and Connection (Proton) objects.]
        if(event.getLink().getName().equals(RECEIVE_TAG)) {
            event.getSession().close();
            event.getSession().getConnection().close();
        } else {
            this.parentComm.openSending();
        }
    }

    //==============================================================================
    //Sender Event Handlers
    //==============================================================================

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
                    length = outgoingMessage.encode(msgData, 0, msgData.length);
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
//            snd.getSession().close();
//            snd.getSession().getConnection().close();
        }
    }

    //==============================================================================
    //Common Event Handlers
    //==============================================================================

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
            transport.ssl(domain);
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
        this.receiver = ssn.receiver(RECEIVE_TAG);
        this.sender = ssn.sender(SEND_TAG);

        // Codes_SRS_AMQPSRECEIVERHANDLER_14_018: [The event handler shall open the Connection, Session, and Receiver objects.]
        conn.open();
        ssn.open();
        receiver.open();
        sender.open();
    }

    /**
     * Event handler for the link init event. Sets the proper target address on the link.
     * @param event Proton Event object
     */
    @Override
    public void onLinkInit(Event event){
        Link link = event.getLink();
        if(link.getName().equals(SEND_TAG)) {
            // Codes_SRS_AMQPSSENDERHANDLER_14_018: [The event handler shall create a new Target (Proton) object using the given endpoint address.]
            // Codes_SRS_AMQPSSENDERHANDLER_14_019: [The event handler shall get the Link (Proton) object and set its target.]
            Target t = new Target();
            t.setAddress(this.sendEndpoint);
            link.setTarget(t);
        } else {
            // Codes_SRS_AMQPSRECEIVERHANDLER_14_019: [The event handler shall create a new Source (Proton) object using the given endpoint address.]
            // Codes_SRS_AMQPSRECEIVERHANDLER_14_020: [The event handler shall get the Link (Proton) object and set its source to the created Source (Proton) object.]
            Source source = new Source();
            source.setAddress(this.receiveEndpoint);
            link.setSource(source);
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
            System.err.println("Error: " + condition.toString());
            System.err.println("Error: " + condition.getDescription());
        } else {
            System.err.println("Err (no description returned).");
        }
    }

    //==============================================================================
    //Helper Methods
    //==============================================================================

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
        this.parentComm.lockSending();

        // Codes_SRS_AMQPSSENDERHANDLER_14_006: [The function shall create a new Message (Proton) object.]
        this.outgoingMessage = Proton.message();

        // Codes_SRS_AMQPSSENDERHANDLER_14_007: [The function shall set the ‘to’ property on the Message object using the created event path.]
        // Codes_SRS_AMQPSSENDERHANDLER_14_026: [The function shall set the ‘messageId’ property on the Message object if the messageId is not null.]
        Properties properties = new Properties();
        properties.setTo(this.sendEndpoint);
        if(messageId != null){
            properties.setMessageId(messageId);
        }
        outgoingMessage.setProperties(properties);

        // Codes_SRS_AMQPSSENDERHANDLER_14_008: [The function shall create a Binary (Proton) object from the content array.]
        Binary binary = new Binary(content);
        // Codes_SRS_AMQPSSENDERHANDLER_14_009: [The function shall create a data Section (Proton) object from the Binary.]
        Section section = new Data(binary);
        // Codes_SRS_AMQPSSENDERHANDLER_14_010: [The function shall set the Message body to the created data section.]
        outgoingMessage.setBody(section);

        this.sender.open();
    }
}
