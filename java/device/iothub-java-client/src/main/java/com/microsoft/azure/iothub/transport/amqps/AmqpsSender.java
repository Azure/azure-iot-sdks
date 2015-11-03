/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iothub.transport.amqps;

import org.apache.qpid.proton.Proton;
import org.apache.qpid.proton.engine.BaseHandler;
import org.apache.qpid.proton.engine.Event;
import org.apache.qpid.proton.engine.HandlerException;
import org.apache.qpid.proton.reactor.Reactor;

import java.io.IOException;
import java.nio.charset.StandardCharsets;

/**
 * Instance of the QPID-Proton-J BaseHandler class that overrides the callbacks needed to handle high level open,
 * close, and send functions. Initialize and use the AmqpsSenderHandler class for low level amqps operations.
 */
public class AmqpsSender extends BaseHandler {

    private final String hostName;
    private final String userName;
    private final String sasToken;
    private final String deviceID;
    private AmqpsSenderHandler amqpsSenderHandler;
    private Reactor reactor;

    /**
     * Constructor to set up connection parameters
     * @param hostName address string of the IoT Hub (example: AAA.BBB.CCC)
     * @param userName username string to use SASL authentication (example: user@sas.[hostName])
     * @param deviceID ID from which this sender communicates
     * @param sasToken  SAS token string
     */
    public AmqpsSender(String hostName, String userName, String deviceID, String sasToken){
        // Codes_SRS_AMQPSSENDER_14_001: [The constructor shall throw a new IllegalArgumentException if any of the input parameters is null or empty.]
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

        // Codes_SRS_AMQPSSENDER_14_002: [The Constructor shall copy all input parameters to private member variables.]
        this.hostName = hostName;
        this.userName = userName;
        this.sasToken = sasToken;
        this.deviceID = deviceID;
    }

    /**
     * Creates an AmqpsSenderHandler and stores it as a member variable
     */
    public void open(){
        // Codes_SRS_AMQPSSENDER_14_003: [The function shall initialize its AmqpsSenderHandler using the saved host name, user name, device ID and sas token.]
        this.amqpsSenderHandler = new AmqpsSenderHandler(hostName, userName, deviceID, sasToken);
    }

    /**
     * Invalidates the AmqpsSenderHandler member variables
     */
    public void close(){
        // Codes_SRS_AMQPSSENDER_14_004: [The function shall invalidate its AmqpsSenderHandler.]
        this.amqpsSenderHandler = null;
    }

    /**
     * Creates a binary message using the given content string. Initializes and starts the Proton reactor. Sends the created message.
     * @param content
     * @throws IOException
     */
    public void send(String content) throws IOException {
        this.send(content.getBytes(StandardCharsets.UTF_8));
    }

    /**
     * Creates a binary message using the given content array. Initializes and starts the Proton reactor. Sends the created message.
     * @param content content byte array of the message
     * @throws IOException
     * @throws HandlerException
     */
    public void send(byte[] content) throws IOException, HandlerException {
        // Codes_SRS_AMQPSSENDER_14_011: [If a messageId is not provided, the function shall create a binary message using the given content and a null messageId.]
        this.send(content, null);
    }

    /**
     * Creates a binary message using the given content array and messageId. Initializes and starts the Proton reactor. Sends the created message.
     * @param content content byte array of the message
     * @param messageId messageId of the message
     * @throws IOException
     * @throws HandlerException
     */
    public void send(byte[] content, Object messageId) throws IOException, HandlerException {
        if(amqpsSenderHandler != null) {
            // Codes_SRS_AMQPSSENDER_14_006: [The function shall create a binary message with the given content and messageId.]
            amqpsSenderHandler.createBinaryMessage(content, messageId);
            // Codes_SRS_AMQPSSENDER_14_007: [The function shall initialize the Reactor (Proton) object.]
            this.reactor = Proton.reactor(this);
            // Codes_SRS_AMQPSSENDER_14_008: [The function shall start running the Reactor (Proton) object.]
            this.reactor.run();
        } else {
            // Codes_SRS_AMQPSSENDER_14_005: [If the AmqpsSenderHandler has not been initialized, the function shall throw a new IOException.]
            throw new IOException("The Sender Handler has not been initialized. Call open before sending.");
        }
    }

    /**
     * Event handler for the reactor init event
     * @param event Proton Event object
     */
    @Override
    public void onReactorInit(Event event){
        // Codes_SRS_AMQPSSENDER_14_009: [The event handler shall set the member AmqpsSenderHandler object to handle the connection events.]
        event.getReactor().connection(this.amqpsSenderHandler);
    }
}
