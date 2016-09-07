/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.transport.amqps;

import com.microsoft.azure.iot.service.sdk.IotHubServiceClientProtocol;
import com.microsoft.azure.iot.service.sdk.Message;
import com.microsoft.azure.iot.service.sdk.Tools;
import org.apache.qpid.proton.Proton;
import org.apache.qpid.proton.engine.BaseHandler;
import org.apache.qpid.proton.engine.Event;
import org.apache.qpid.proton.reactor.Reactor;

import java.io.IOException;

/**
 * Instance of the QPID-Proton-J BaseHandler class
 * overriding the events what are needed to handle
 * high level open, close and send methods.
 * Initialize and use AmqpsSendHandler class for low level ampqs operations.
 */
public class AmqpSend extends BaseHandler
{
    protected final String hostName;
    protected final String userName;
    protected final String sasToken;
    protected Reactor reactor = null;
    protected AmqpSendHandler amqpSendHandler;
    protected IotHubServiceClientProtocol iotHubServiceClientProtocol;

    /**
     * Constructor to set up connection parameters
     * @param hostName The address string of the service (example: AAA.BBB.CCC)
     * @param userName The username string to use SASL authentication (example: user@sas.service)
     * @param sasToken The SAS token string
     */
    public AmqpSend(String hostName, String userName, String sasToken, IotHubServiceClientProtocol iotHubServiceClientProtocol)
    {
        // Codes_SRS_SERVICE_SDK_JAVA_AMQPSEND_12_001: [The constructor shall throw IllegalArgumentException if any of the input parameter is null or empty]
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

        // Codes_SRS_SERVICE_SDK_JAVA_AMQPSEND_12_002: [The constructor shall copy all input parameters to private member variables for event processing]
        this.hostName = hostName;
        this.userName = userName;
        this.sasToken = sasToken;
        this.iotHubServiceClientProtocol = iotHubServiceClientProtocol;
    }

    /**
     * Event handler for the reactor init event
     * @param event The proton event object
     */
    @Override
    public void onReactorInit(Event event) {
        // You can use the connection method to create AMQP connections.

        // This connection's handler is the AmqpSendHandler object. All the events
        // for this connection will go to the AmqpSendHandler object instead of
        // going to the reactor. If you were to omit the AmqpSendHandler object,
        // all the events would go to the reactor.

        // Codes_SRS_SERVICE_SDK_JAVA_AMQPSEND_12_003: [The event handler shall set the member AmqpsSendHandler object to handle the given connection events]
        event.getReactor().connection(amqpSendHandler);
    }

    /**
     * Create AmqpsSendHandler and store it in a member variable
     */
    public void open()
    {
        // Codes_SRS_SERVICE_SDK_JAVA_AMQPSEND_12_004: [The function shall create an AmqpsSendHandler object to handle reactor events]
        amqpSendHandler = new AmqpSendHandler(this.hostName, this.userName, this.sasToken, this.iotHubServiceClientProtocol);
    }

    /**
     * Invalidate AmqpsSendHandler member variable
     */
    public void close()
    {
        // Codes_SRS_SERVICE_SDK_JAVA_AMQPSEND_12_005: [The function shall invalidate the member AmqpsSendHandler object]
        amqpSendHandler = null;
    }

    /**
     * Create binary message
     * Initialize and start Proton reactor
     * Send the created message
     * @param deviceId The device name string
     * @param message The message to be sent
     * @throws IOException This exception is thrown if the AmqpSend object is not initialized
     */
    public void send(String deviceId, Message message) throws IOException
    {
        if  (amqpSendHandler != null)
        {
            // Codes_SRS_SERVICE_SDK_JAVA_AMQPSEND_12_006: [The function shall create a binary message with the given content]
            amqpSendHandler.createProtonMessage(deviceId, message);
            // Codes_SRS_SERVICE_SDK_JAVA_AMQPSEND_12_007: [The function shall initialize the Proton reactor object]
            this.reactor = Proton.reactor(this);
            // Codes_SRS_SERVICE_SDK_JAVA_AMQPSEND_12_008: [The function shall start the Proton reactor object]
            this.reactor.run();
            this.reactor.free();

        }
        else
        {
            // Codes_SRS_SERVICE_SDK_JAVA_AMQPSEND_12_009: [The function shall throw IOException if the send handler object is not initialized]
            throw new IOException("send handler is not initialized. call open before send");
        }

    }


}
