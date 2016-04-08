/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iothub.transport.amqps;

import com.microsoft.azure.iothub.IotHubClientProtocol;
import com.microsoft.azure.iothub.transport.TransportUtils;
import org.apache.qpid.proton.Proton;
import org.apache.qpid.proton.amqp.Binary;
import org.apache.qpid.proton.amqp.Symbol;
import org.apache.qpid.proton.amqp.messaging.*;
import org.apache.qpid.proton.amqp.transport.DeliveryState;
import org.apache.qpid.proton.amqp.transport.ErrorCondition;
import org.apache.qpid.proton.amqp.transport.SenderSettleMode;
import org.apache.qpid.proton.engine.*;
import org.apache.qpid.proton.engine.impl.WebSocketImpl;
import org.apache.qpid.proton.message.Message;
import org.apache.qpid.proton.reactor.FlowController;
import org.apache.qpid.proton.reactor.Handshaker;

import java.nio.BufferOverflowException;
import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.CompletableFuture;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.TimeoutException;

/**
 * Instance of the QPID-Proton-J BaseHandler class that overrides the events needed to handle receiving and sending operations.
 * Contains and sets connection parameters, maintains the layers of the AMQP protocol and creates and sets SASL authentication
 * for the transport.
 */
public final class AmqpsIotHubConnectionBaseHandler extends BaseHandler {
    //==============================================================================
    //Constant Variables
    //==============================================================================
    /** Default number of seconds to wait before assuming that the connection failed */
    private static final int DEFAULT_CONNECTION_WAIT_TIME_SECONDS = 10;
    /** Default number of seconds to wait before assuming that the Flow event has not happened and this BaseHandler will
     * not be informed of the amount of link credit available on the sender link.
     */
    private static final int DEFAULT_LINK_CREDIT_WAIT_TIME = 30;
    /** Constant name to assign to the Sender endpoint */
    public static final String SEND_TAG = "sender";
    /** Constant name to assign to the Receiver endpoint */
    public static final String RECEIVE_TAG = "receiver";
    /** AMQPS Port number */
    public static final int PORT_AMQP = 5671;
    /** AMQPS Port number */
    public static final int PORT_WEBSOCKET = 443;
    /** Sender endpoint format */
    public static final String SEND_ENDPOINT_FORMAT = "/devices/%s/messages/events";
    /** Receiver endpoint format */
    public static final String RECEIVE_ENDPOINT_FORMAT = "/devices/%s/messages/devicebound";

    /** WebSocket host name path extension */
    public static final String WEBSOCKET_PATH = "/$iothub/websocket";
    /** WebSocket sub-protocol */
    public static final String WEBSOCKET_SUB_PROTOCOL = "AMQPWSB10";

    //==============================================================================
    //Reactor Variables
    //==============================================================================
    /** The {@link Sender} link. */
    private Sender sender;
    /** The {@link Receiver} link. */
    private Receiver receiver;
    private Connection connection;
    private Session session;

    //==============================================================================
    //Connection Variables
    //==============================================================================
    /** The address string of the IoT Hub. */
    private final String hostName;
    /** The username string to use SASL authentication. */
    private final String userName;
    /** The SAS token string. */
    private final String sasToken;
    /** The ID for the associated device. */
    private final String deviceID;
    /** The Sender endpoint created from the {@link #SEND_ENDPOINT_FORMAT}. */
    private final String sendEndpoint;
    /** The Receiver endpoint created from the {@link #RECEIVE_ENDPOINT_FORMAT}. */
    private final String receiveEndpoint;
    /** Version identifier key */
    private static final String versionIdentifierKey = "com.microsoft:client-version";
    /** Indicates if use AMQP over WEBSOCKET or AMQP */
    private final IotHubClientProtocol iotHubClientProtocol;
    /** The address string of the IoT Hub without port */
    private final String webSocketHostName;

    //==============================================================================
    //Class Variables
    //==============================================================================
    /** The {@link AmqpsIotHubConnection} above this {@link AmqpsIotHubConnectionBaseHandler}. */
    private final AmqpsIotHubConnection parentIotHubConnection;
    /** The {@link CompletableFuture} representing the delivery status of the currently sent message.*/
    private CompletableFuture<Integer> currentSentMessageFuture;
    /** The current outgoing {@link Message}. */
    protected Message outgoingMessage;
    /** The {@link Delivery} tag. */
    private int nextTag = 0;
    /** The LinkFlow flag. */
    private boolean linkFlow;
    /** The Link Credit for the Sender link */
    private CompletableFuture<Integer> linkCredit;

    /**
     * Constructor to set up connection parameters and initialize handshaker and flow controller for the transport.
     * @param hostName The address string of the IoT Hub (example: AAA.BBB.CCC).
     * @param userName The username string to use SASL authentication (example: user@sas.[hostName]).
     * @param sasToken The SAS token string.
     * @param deviceID The ID for the associated device.
     * @param parentIotHubConnection The parent {@link AmqpsIotHubConnection} object.
     */
    public AmqpsIotHubConnectionBaseHandler(String hostName, String userName, String sasToken, String deviceID, IotHubClientProtocol iotHubClientProtocol, AmqpsIotHubConnection parentIotHubConnection){
        // Codes_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_001: [The constructor shall throw a new IllegalArgumentException if any of the input parameters is null or empty.]
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
        if(parentIotHubConnection == null)
        {
            throw new IllegalArgumentException("The AmqpsIotHubConnection cannot be null.");
        }

        // Codes_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_002: [The constructor shall copy all input parameters to private member variables for event processing.]
        // Codes_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_003: [The constructor shall concatenate the host name with the port.]
        this.iotHubClientProtocol = iotHubClientProtocol;
        this.webSocketHostName = hostName;
        if (this.iotHubClientProtocol == IotHubClientProtocol.AMQPS_WS)
        {
            this.hostName = String.format("%s:%d", hostName, PORT_WEBSOCKET);
        }
        else
        {
            this.hostName = String.format("%s:%d", hostName, PORT_AMQP);
        }
        this.userName = userName;
        this.deviceID = deviceID;
        this.sasToken = sasToken;
        this.parentIotHubConnection = parentIotHubConnection;
        this.linkFlow = false;
        this.linkCredit = new CompletableFuture<>();

        this.sender = null;
        this.receiver = null;
        this.session = null;
        this.connection = null;

        // Codes_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_004: [The constructor shall initialize the sender and receiver endpoint private member variables using the ENDPOINT_FORMAT constants and deviceID.]
        this.sendEndpoint = String.format(SEND_ENDPOINT_FORMAT, this.deviceID);
        this.receiveEndpoint = String.format(RECEIVE_ENDPOINT_FORMAT, this.deviceID);

        // Codes_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_005: [The constructor shall initialize a new Handshaker (Proton) object to handle communication handshake.]
        // Codes_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_006: [The constructor shall initialize a new FlowController (Proton) object to handle communication flow.]
        add(new Handshaker());
        add(new FlowController());
    }

    //==============================================================================
    //Receiver Event Handlers
    //==============================================================================

    /**
     * Event handler for the delivery event. Takes the message data, creates a new AmqpsMessage and gives the message
     * up to the parent receiver.
     * @param event The Proton Event object.
     */
    @Override
    public void onDelivery(Event event){
        if(event.getLink().getName().equals(RECEIVE_TAG)) {
            // Codes_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_007: [If this link is the Receiver link, the event handler shall get the Receiver and Delivery (Proton) objects from the event.]
            Receiver recv = (Receiver) event.getLink();
            Delivery delivery = recv.current();
            if (delivery.isReadable() && !delivery.isPartial()) {
                // Codes_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_008: [The event handler shall read the received buffer.]
                int size = delivery.pending();
                byte[] buffer = new byte[size];
                int read = recv.recv(buffer, 0, buffer.length);
                recv.advance();

                // Codes_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_009: [The event handler shall create an AmqpsMessage object from the decoded buffer.]
                // Codes_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_010: [The event handler shall set the AmqpsMessage Deliver (Proton) object.]
                Message msg = new AmqpsMessage();
                ((AmqpsMessage) msg).setDelivery(delivery);
                msg.decode(buffer, 0, read);

                // Codes_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_011: [The event handler shall give the message back to its parent AmqpsIotHubConnection.]
                this.parentIotHubConnection.addMessage(msg);
            }
        } else{
            //Sender specific section for dispositions it receives
            if(event.getType() == Event.Type.DELIVERY){
                // Codes_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_012: [If this link is not the Receiver link and the event type is DELIVERY, the event handler shall get the Delivery (Proton) object from the event.]
                Delivery d = event.getDelivery();
                DeliveryState remoteState = d.getRemoteState();

                // Codes_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_013: [If the event type is DELIVERY, the event handler shall note the remote delivery state and use it and the Delivery (Proton) hash code to inform the AmqpsIotHubConnection of the message receipt.]
                boolean state = remoteState.equals(Accepted.getInstance());
                this.parentIotHubConnection.acknowledge(d.hashCode(), state);
            }
        }
    }

    /**
     * Event handler for the link local close event. If the link closes locally, close the session and connection.
     * @param event The Proton Event object.
     */
    @Override
    public void onLinkLocalClose(Event event){

        // Codes_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_016: [If the link was locally closed before the current message is sent, the sent message CompletableFuture will be completed exceptionally with a new HandlerException.]
        if(currentSentMessageFuture != null && !this.currentSentMessageFuture.isDone()){
            this.currentSentMessageFuture.completeExceptionally(new HandlerException(this, new Throwable("Link closed before the message was sent.")));
        }

        // Codes_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_015: [The event handler shall close the Session and Connection (Proton) objects.]
        event.getSession().close();
        event.getSession().getConnection().close();
    }

    /**
     * Event handler for the link remote close event. If the link closes remotely, fail the parent connection.
     * @param event The Proton Event object.
     */
    @Override
    public void onLinkRemoteClose(Event event)
    {
        this.parentIotHubConnection.fail("Connection to the server closed");
    }

    //==============================================================================
    //Sender Event Handlers
    //==============================================================================

    /**
     * Event handler for the link flow event. Handles sending of a single message.
     * @param event The Proton Event object.
     */
    @Override
    public void onLinkFlow(Event event){
        Sender snd = (Sender)event.getLink();
        this.linkCredit.complete(snd.getCredit());
        this.linkFlow = true;
    }

    //==============================================================================
    //Common Event Handlers
    //==============================================================================

    /**
     * Event handler for the connection bound event. Sets Sasl authentication and proper authentication mode.
     * @param event The Proton Event object.
     */
    @Override
    public void onConnectionBound(Event event){
        // Codes_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_023: [The event handler shall get the Transport (Proton) object from the event.]
        Transport transport = event.getConnection().getTransport();
        if(transport != null){

            if (this.iotHubClientProtocol == IotHubClientProtocol.AMQPS_WS)
            {
                WebSocketImpl webSocket = (WebSocketImpl) transport.webSocket();
                webSocket.configure(this.webSocketHostName, WEBSOCKET_PATH, 0, WEBSOCKET_SUB_PROTOCOL, null, null);
            }

            // Codes_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_024: [The event handler shall set the SASL_PLAIN authentication on the transport using the given user name and sas token.]
            Sasl sasl = transport.sasl();
            sasl.plain(this.userName, this.sasToken);

            // Codes_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_025: [The event handler shall set ANONYMOUS_PEER authentication mode on the domain of the Transport.]
            SslDomain domain = makeDomain(SslDomain.Mode.CLIENT);
            domain.setPeerAuthentication(SslDomain.VerifyMode.ANONYMOUS_PEER);
            transport.ssl(domain);
        }
    }

    /**
     * Event handler for the connection init event
     * @param event The Proton Event object.
     */
    @Override
    public void onConnectionInit(Event event){
        // Codes_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_026: [The event handler shall get the Connection (Proton) object from the event handler and set the host name on the connection.]
        Connection conn = event.getConnection();
        conn.setHostname(this.hostName);

        // Codes_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_027: [The event handler shall create a Session (Proton) object from the connection.]
        Session ssn = conn.session();

        // Codes_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_028: [The event handler shall create a Receiver and Sender (Proton) object and set the protocol tag on them to a predefined constant.]
        Map<Symbol, Object> properties = new HashMap<>();
        properties.put(Symbol.getSymbol(versionIdentifierKey), TransportUtils.javaDeviceClientIdentifier + TransportUtils.clientVersion);

        // Codes_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_15_054: [The Receiver and Sender objects shall have the properties set to client version identifier.]
        this.receiver = ssn.receiver(RECEIVE_TAG);
        this.receiver.setProperties(properties);
        this.sender = ssn.sender(SEND_TAG);
        this.sender.setProperties(properties);
        this.session = ssn;
        this.connection = conn;

        // Codes_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_029: [The event handler shall open the Connection, Session, and Receiver objects.]
        conn.open();
        ssn.open();
        receiver.open();
        sender.open();
    }

    /**
     * Event handler for the connection local open event. If session is not opened remotely before the default connection wait time,
     * the parent {@link AmqpsIotHubConnection} will be informed that the connection was not opened in time.
     * @param event The Proton Event object.
     */
    @Override
    public void onConnectionLocalOpen(Event event){
        // Codes_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_037: [If the Connection and Session (Proton) are not opened before a constant number of seconds, this handler will inform the AmqpsIotHubConnection that the Connection was not opened in time.]
        new Thread(() -> {
            try {
                Thread.sleep(DEFAULT_CONNECTION_WAIT_TIME_SECONDS*1000);
                this.parentIotHubConnection.reactorReady.completeExceptionally(new Throwable("Connection unable to be opened in time."));
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }).start();
    }

    /**
     * Event handler for the connection remote close event.
     * @param event The Proton Event object.
     */
    @Override
    public void onConnectionRemoteClose(Event event){
        // Codes_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_035: [If the Connection was remotely closed abnormally, the event handler shall complete the sent message CompletableFuture with a new HandlerException.]
        if(event.getConnection().getCondition() != null)
        {
            this.currentSentMessageFuture.completeExceptionally(new HandlerException(this, new Throwable("Connected remotely closed due to error.")));
        }
    }

    /**
     * Event handler for the session remote open event. Informs the parent {@link AmqpsIotHubConnection} that the {@link org.apache.qpid.proton.reactor.Reactor} is ready.
     * @param event The Proton Event object.
     */
    @Override
    public void onLinkRemoteOpen(Event event) {
        // Codes_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_036: [The event handler shall inform the AmqpsIotHubConnection that the Reactor (Proton) is ready.]
        Link link = event.getLink();
        if (link.getName().equals(SEND_TAG)) {
            this.parentIotHubConnection.reactorReady.complete(new Boolean(true));
        }
    }

    /**
     * Event handler for the link init event. Sets the proper target address on the link.
     * @param event The Proton Event object.
     */
    @Override
    public void onLinkInit(Event event){
        Link link = event.getLink();
        if(link.getName().equals(SEND_TAG)) {
            // Codes_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_031: [If the link is the Sender link, the event handler shall create a new Target (Proton) object using the sender endpoint address member variable.]
            // Codes_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_033: [If the link is the Sender link, the event handler shall get the Link (Proton) object and set its target.]
            Target t = new Target();
            t.setAddress(this.sendEndpoint);
            link.setTarget(t);
            // Codes_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_034: [If the link is the Sender link, the event handler shall set the SenderSettleMode to UNSETTLED.]
            link.setSenderSettleMode(SenderSettleMode.UNSETTLED);
        } else {
            // Codes_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_030: [If the link is the Receiver link, the event handler shall create a new Source (Proton) object using the receiver endpoint address member variable.]
            // Codes_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_032: [If the link is the Receiver link, the event handler shall get the Link (Proton) object and set its source to the created Source (Proton) object.]
            Source source = new Source();
            source.setAddress(this.receiveEndpoint);
            link.setSource(source);
        }
    }

    /**
     * Event handler for the transport error event.
     * @param event The Proton Event object.
     */
    @Override
    public void onTransportError(Event event){
        // Codes_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_038: [The event handler shall log the error and description if there is a description accompanying the error.]
        ErrorCondition condition = event.getTransport().getCondition();
        if(condition != null){
            System.err.println("Error: " + condition.toString());
            System.err.println("Error: " + condition.getDescription());
        } else {
            System.err.println("Err (no description returned).");
        }
        // Codes_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_039: [The event handler shall cause the AmqpsIotHubConnection to fail.]
        this.parentIotHubConnection.fail(condition.toString());
    }

    public int getLinkCredit() throws ExecutionException, InterruptedException, TimeoutException {
        return this.linkCredit.get(DEFAULT_LINK_CREDIT_WAIT_TIME, TimeUnit.SECONDS);
    }

    //==============================================================================
    //Helper Methods
    //==============================================================================

    /**
     * Create Proton message using given content byte array and messageId
     * @param content The content byte array.
     * @param messageId The message ID of the message.
     *
     * @return a CompletableFuture for the sent message.
     */
    public synchronized CompletableFuture<Integer> createBinaryMessage(byte[] content, Object messageId){
        // Codes_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_040: [The function shall create a new Message (Proton) object.]
        this.outgoingMessage = Proton.message();

        // Codes_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_041: [The function shall set the ‘to’ property on the Message object using the created event path.]
        // Codes_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_042: [The function shall set the ‘messageId’ property on the Message object if the messageId is not null.]
        Properties properties = new Properties();
        properties.setTo(this.sendEndpoint);
        if(messageId != null){
            properties.setMessageId(messageId);
        }
        outgoingMessage.setProperties(properties);

        // Codes_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_043: [The function shall create a Binary (Proton) object from the content array.]
        Binary binary = new Binary(content);
        // Codes_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_044: [The function shall create a data Section (Proton) object from the Binary.]
        Section section = new Data(binary);
        // Codes_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_045: [The function shall set the Message body to the created data Section.]
        outgoingMessage.setBody(section);

        // Codes_SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_047: [The function shall return a new CompletableFuture for the sent message.]
        return this.send();
    }

    public void shutdown(){
		this.linkFlow = false;
        if (this.sender != null) {
            this.sender.close();
        }
        if (this.receiver != null) {
            this.receiver.close();
        }
        if (this.session != null) {
            this.session.close();
        }
        if (this.connection != null) {
            this.connection.close();
        }
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

    private CompletableFuture<Integer> send(){
        this.currentSentMessageFuture = new CompletableFuture<>();
        new Thread(() -> {
            if (this.linkFlow) {
                //Encode the message and copy the contents to the byte buffer
                if (this.sender.getCredit() > 0) {
                    byte[] msgData = new byte[1024];
                    int length;

                    while (true) {
                        try {
                            length = outgoingMessage.encode(msgData, 0, msgData.length);
                            break;
                        } catch (BufferOverflowException e) {
                            msgData = new byte[msgData.length * 2];
                        }
                    }
                    // Set the delivery tag
                    byte[] tag = String.valueOf(nextTag++).getBytes();
                    Delivery dlv = this.sender.delivery(tag);
                    // Send encoded bytes using the Sender link
                    this.sender.send(msgData, 0, length);

                    // Advance the sender and complete the future with the hash of the delivery object
                    this.sender.advance();
                    this.currentSentMessageFuture.complete(dlv.hashCode());
                }
            } else {
                this.currentSentMessageFuture.completeExceptionally(new Throwable("Link is currently closed. A new link will be opened and the message resent."));
            }
        }).start();
        return this.currentSentMessageFuture;
    }
}
