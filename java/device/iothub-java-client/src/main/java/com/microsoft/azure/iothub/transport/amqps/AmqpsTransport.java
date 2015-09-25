// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

package com.microsoft.azure.iothub.transport.amqps;

import com.microsoft.azure.iothub.DeviceClientConfig;
import com.microsoft.azure.iothub.Message;
import com.microsoft.azure.iothub.MessageCallback;
import com.microsoft.azure.iothub.IotHubMessageResult;
import com.microsoft.azure.iothub.IotHubStatusCode;
import com.microsoft.azure.iothub.IotHubEventCallback;
import com.microsoft.azure.iothub.transport.IotHubCallbackPacket;
import com.microsoft.azure.iothub.transport.IotHubOutboundPacket;
import com.microsoft.azure.iothub.transport.IotHubTransport;

import java.io.IOException;
import java.util.LinkedList;
import java.util.Queue;

/**
 * <p>
 * An AMQPS transport. Contains functionality for adding messages and sending
 * messages to an IoT Hub. Buffers unsent messages until they are received by an
 * IoT Hub. A transport is bound at construction to the following parameters:
 * IoT Hub name, device ID, device key, and message valid seconds.
 * </p>
 * The transport can also poll an IoT Hub for messages and invoke a
 * user-defined message callback if a message and callback is found.
 */
public final class AmqpsTransport implements IotHubTransport
{
    /** The state of the AMQPS transport. */
    protected enum AmqpsTransportState
    {
        OPEN, CRASHED, CLOSED
    }

    protected AmqpsTransportState state;

    /** The AMQPS session with an IoT Hub. */
    protected AmqpsIotHubSession session;

    /** Messages waiting to be sent to an IoT Hub. */
    protected final Queue<IotHubOutboundPacket> waitingList;
    /** Messages whose callbacks that are waiting to be invoked. */
    protected final Queue<IotHubCallbackPacket> callbackList;

    protected final DeviceClientConfig config;

    /**
     * Constructs an instance from the given {@link DeviceClientConfig}
     * object.
     *
     * @param config configuration parameters for an AMQPS session with an IoT
     * Hub.
     */
    public AmqpsTransport(DeviceClientConfig config)
    {
        // Codes_SRS_AMQPSTRANSPORT_11_001: [The function shall initialize an empty queue for messages waiting to be sent.]
        this.waitingList = new LinkedList<>();
        // Codes_SRS_AMQPSTRANSPORT_11_002: [The function shall initialize an empty queue for completed messages whose callbacks are waiting to be invoked.]
        this.callbackList = new LinkedList<>();
        this.config = config;

        this.state = AmqpsTransportState.CLOSED;
    }

    /**
     * Establishes a communication channel with an IoT Hub. If a channel is
     * already open, the function shall do nothing.
     *
     * @throws IOException if a communication channel cannot be established.
     */
    public void open() throws IOException
    {
        // Codes_SRS_AMQPSTRANSPORT_11_020: [If an AMQPS session is already open, the function shall do nothing.]
        if (this.state == AmqpsTransportState.OPEN)
        {
            return;
        }

        // Codes_SRS_AMQPSTRANSPORT_11_019: [The function shall open an AMQPS session with the IoT Hub given in the configuration.]
        this.session = new AmqpsIotHubSession(this.config);
        this.session.open();

        this.state = AmqpsTransportState.OPEN;
    }

    /**
     * Closes all resources used to communicate with an IoT Hub. Once {@code close()} is
     * called, the transport is no longer usable. If the transport is already
     * closed, the function shall do nothing.
     *
     * @throws IOException if an error occurs in closing the transport.
     */
    public void close() throws IOException
    {
        // Codes_SRS_AMQPSTRANSPORT_11_023: [If the AMQPS session is closed, the function shall do nothing.]
        if (this.state == AmqpsTransportState.CLOSED)
        {
            return;
        }

        // Codes_SRS_AMQPSTRANSPORT_11_021: [The function shall close an AMQPS session with the IoT Hub given in the configuration.]
        this.session.close();
        this.state = AmqpsTransportState.CLOSED;
    }

    /**
     * Adds a message to the transport queue.
     *
     * @param message the message to be sent.
     * @param callback the callback to be invoked when a response for the
     * message is received.
     * @param callbackContext the context to be passed in when the callback is
     * invoked.
     *
     * @throws IllegalStateException if the transport has not been opened or is
     * already closed.
     */
    public void addMessage(Message message,
            IotHubEventCallback callback,
            Object callbackContext)
    {
        // Codes_SRS_AMQPSTRANSPORT_11_025: [If the AMQPS session is closed, the function shall throw an IllegalStateException.]
        if (this.state == AmqpsTransportState.CLOSED)
        {
            throw new IllegalStateException(
                    "Cannot add a message to "
                            + "an AMQPS transport that is closed.");
        }

        // Codes_SRS_AMQPSTRANSPORT_11_003: [The function shall add a packet containing the message, callback, and callback context to the transport queue.]
        IotHubOutboundPacket packet =
                new IotHubOutboundPacket(message, callback, callbackContext);
        this.waitingList.add(packet);
    }

    /**
     * <p>
     * Sends all messages on the transport queue, one at a time. If a previous
     * send attempt had failed, the function will attempt to resend the messages
     * in the previous attempt.
     * </p>
     * If one has not already been created, the function will initialize an
     * AMQPS session with the IoT Hub specified in the configuration.
     *
     * @throws IOException if the server could not be reached.
     * @throws IllegalStateException if the transport has not been opened or is
     * already closed.
     */
    public void sendMessages() throws IOException
    {
        // Codes_SRS_AMQPSTRANSPORT_11_028: [If the AMQPS session is closed, the function shall throw an IllegalStateException.]
        if (this.state == AmqpsTransportState.CLOSED)
        {
            throw new IllegalStateException(
                    "AMQPS transport is already closed.");
        }
        if (this.waitingList.size() <= 0)
        {
            return;
        }
        // Codes_SRS_AMQPSTRANSPORT_11_026: [If the transport had previously encountered any exception or error while open, it shall reopen a new AMQPS session with the IoT Hub given in the configuration.]
        if (this.state == AmqpsTransportState.CRASHED)
        {
            // an AMQPS session must be manually managed,
            // so we reopen a new session to avoid encountering corrupted state.
            this.session = new AmqpsIotHubSession(this.config);
            this.session.open();
        }

        // Codes_SRS_AMQPSTRANSPORT_11_004: [The function shall attempt to send every message on its waiting list, one at a time.]
        try
        {
            while (!this.waitingList.isEmpty())
            {
                // Codes_SRS_AMQPSTRANSPORT_11_007: [If the IoT Hub could not be reached, the message shall be buffered to be sent again next time.]
                IotHubOutboundPacket packet = this.waitingList.peek();

                // Codes_SRS_AMQPSTRANSPORT_11_006: [For each message being sent, the function shall send the message and add the IoT Hub status code along with the callback and context to the callback list.]
                // Codes_SRS_AMQPSTRANSPORT_11_015: [If the IoT Hub could not be reached, the function shall throw an IOException.]
                // Codes_SRS_AMQPSTRANSPORT_11_005: [If no AMQPS session exists with the IoT Hub, the function shall establish one.]
                IotHubStatusCode status =
                        this.session.sendEvent(packet.getMessage());

                IotHubCallbackPacket callbackPacket =
                        new IotHubCallbackPacket(status, packet.getCallback(),
                                packet.getContext());
                this.waitingList.remove();
                this.callbackList.add(callbackPacket);
            }
        }
        catch (Throwable e)
        {
            // Codes_SRS_AMQPSTRANSPORT_11_034: [If the function throws any exception or error, it shall also close the AMQPS session.]
            this.session.close();
            this.state = AmqpsTransportState.CRASHED;
            throw e;
        }
    }

    /**
     * Invokes the callbacks for all completed requests.
     *
     * @throws IllegalStateException if the transport has not been opened or is
     * already closed.
     */
    public void invokeCallbacks()
    {
        // Codes_SRS_AMQPSTRANSPORT_11_030: [If the AMQPS session is closed, the function shall throw an IllegalStateException.]
        if (this.state == AmqpsTransportState.CLOSED)
        {
            throw new IllegalStateException(
                    "AMQPS transport is already closed.");
        }

        // Codes_SRS_AMQPSTRANSPORT_11_008: [The function shall invoke all callbacks on its callback queue.]
        while (!this.callbackList.isEmpty())
        {
            // Codes_SRS_AMQPSTRANSPORT_11_009: [If an exception is thrown during the callback, the function shall drop the callback from the queue.]
            IotHubCallbackPacket packet = this.callbackList.remove();

            IotHubStatusCode status = packet.getStatus();
            IotHubEventCallback callback = packet.getCallback();
            Object context = packet.getContext();

            callback.execute(status, context);
        }
    }

    /**
     * <p>
     * Invokes the message callback if a message is found and
     * responds to the IoT Hub on how the processed message should be
     * handled by the IoT Hub.
     * </p>
     * If no message callback is set, the function will do nothing.
     *
     * @throws IOException if the server could not be reached.
     * @throws IllegalStateException if the transport has not been opened or is
     * already closed.
     */
    public void handleMessage() throws IOException
    {
        // Codes_SRS_AMQPSTRANSPORT_11_033: [If the AMQPS session is closed, the function shall throw an IllegalStateException.]
        if (this.state == AmqpsTransportState.CLOSED)
        {
            throw new IllegalStateException(
                    "AMQPS transport is already closed.");
        }

        MessageCallback callback =
                this.config.getMessageCallback();
        Object context = this.config.getMessageContext();
        if (callback == null)
        {
            return;
        }
        // Codes_SRS_AMQPSTRANSPORT_11_031: [If the transport had previously encountered any exception or error while open, it shall reopen a new AMQPS session with the IoT Hub given in the configuration.]
        if (this.state == AmqpsTransportState.CRASHED)
        {
            // an AMQPS session must be manually managed,
            // so we reopen a new session to avoid encountering corrupted state.
            this.session = new AmqpsIotHubSession(this.config);
            this.session.open();
        }

        try
        {
            // Codes_SRS_AMQPSTRANSPORT_11_014: [If no AMQPS session exists with the IoT Hub, the function shall establish one.] 
            // Codes_SRS_AMQPSTRANSPORT_11_017: [If the IoT Hub could not be reached, the function shall throw an IOException.]
            // Codes_SRS_AMQPSTRANSPORT_11_010: [The function shall attempt to consume a message from the IoT Hub.]
            Message message = this.session.receiveMessage();
            // Codes_SRS_AMQPSTRANSPORT_11_011: [If a message is found and a message callback is registered, the function shall invoke the callback on the message.]
            if (message != null)
            {
                IotHubMessageResult result =
                        callback.execute(message, context);

                // Codes_SRS_AMQPSTRANSPORT_11_012: [The function shall return the message result (one of COMPLETE, ABANDON, or REJECT) to the IoT Hub.]
                this.session.sendMessageResult(result);
            }
        }
        catch (Throwable e)
        {
            // Codes_SRS_AMQPSTRANSPORT_11_035: [If the function throws any exception or error, it shall also close the AMQPS session.]
            this.session.close();
            this.state = AmqpsTransportState.CRASHED;
            throw e;
        }
    }

    /**
     * Returns {@code true} if the transport has no more messages to handle,
     * and {@code false} otherwise.
     *
     * @return {@code true} if the transport has no more messages to handle,
     * and {@code false} otherwise.
     */
    public boolean isEmpty()
    {
        // Codes_SRS_AMQPSTRANSPORT_11_013: [The function shall return true if the waiting list and callback list are all empty, and false otherwise.]
        if (this.waitingList.isEmpty() && this.callbackList.isEmpty())
        {
            return true;
        }

        return false;
    }

    protected AmqpsTransport()
    {
        this.waitingList = null;
        this.callbackList = null;
        this.config = null;
    }
}
