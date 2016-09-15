// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

package com.microsoft.azure.iothub.transport.https;

import com.microsoft.azure.iothub.*;
import com.microsoft.azure.iothub.MessageCallback;
import com.microsoft.azure.iothub.transport.IotHubCallbackPacket;
import com.microsoft.azure.iothub.transport.IotHubOutboundPacket;
import com.microsoft.azure.iothub.transport.IotHubTransport;

import java.io.IOException;
import java.util.LinkedList;
import java.util.NoSuchElementException;
import java.util.Queue;
import java.util.concurrent.LinkedBlockingDeque;

import javax.naming.SizeLimitExceededException;

/**
 * <p>
 * An HTTPS transport. Contains functionality for adding messages and sending
 * batched messages to an IoT Hub. Buffers unsent messages until they are
 * received by an IoT Hub. A transport is bound at construction to the following
 * parameters: IoT Hub name, device ID, device key, and message valid seconds.
 * </p>
 * <p>
 * The transport can also poll an IoT Hub for messages and invoke a
 * user-defined message callback if a message and callback is found.
 * </p>
 */
public final class HttpsTransport implements IotHubTransport
{
    /** The state of the HTTPS transport. */
    protected enum HttpsTransportState
    {
        OPEN, CLOSED
    }

    protected HttpsTransportState state;

    /** The underlying HTTPS connection. */
    protected HttpsIotHubConnection connection;

    /** Messages waiting to be sent to an IoT Hub. */
    protected final Queue<IotHubOutboundPacket> waitingList;
    /** Messages that are waiting for a response from an IoT Hub. */
    protected final Queue<IotHubOutboundPacket> inProgressList;
    /** Messages whose callbacks that are waiting to be invoked. */
    protected final Queue<IotHubCallbackPacket> callbackList;

    protected final DeviceClientConfig config;

    /**
     * Constructs an instance from the given {@link DeviceClientConfig}
     * object.
     *
     * @param config configuration parameters for an IoT Hub connection.
     */
    public HttpsTransport(DeviceClientConfig config)
    {
        // Codes_SRS_HTTPSTRANSPORT_11_001: [The constructor shall initialize an empty transport queue for adding messages to be sent as a batch.]
        this.waitingList = new LinkedList<>();
        this.inProgressList = new LinkedList<>();
        // Codes_SRS_HTTPSTRANSPORT_11_002: [The constructor shall initialize an empty queue for adding callbacks waiting to be invoked.]
        this.callbackList = new LinkedBlockingDeque<>();

        this.config = config;

        this.state = HttpsTransportState.CLOSED;
    }

    /**
     * Establishes a communication channel with an IoT Hub. If a channel is
     * already open, the function shall do nothing.
     *
     * @throws IOException if a communication channel cannot be established.
     */
    public void open() throws IOException
    {
        // Codes_SRS_HTTPSTRANSPORT_11_022: [If the transport is already open, the function shall do nothing.]
        if (this.state == HttpsTransportState.OPEN)
        {
            return;
        }

        // Codes_SRS_HTTPSTRANSPORT_11_023: [If the transport is already closed, the function shall throw an IllegalStateException.]
        // Codes_SRS_HTTPSTRANSPORT_11_021: [The function shall establish an HTTPS connection with the IoT Hub given in the configuration.]
        this.connection = new HttpsIotHubConnection(this.config);
        this.state = HttpsTransportState.OPEN;
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
        // the HTTPS connection does not contain state
        // that needs to be explicitly destroyed.
        // Codes_SRS_HTTPSTRANSPORT_11_035: [The function shall mark the transport as being closed.]
        this.state = HttpsTransportState.CLOSED;
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
        // Codes_SRS_HTTPSTRANSPORT_11_027: [If the transport is closed, the function shall throw an IllegalStateException.]
        if (this.state == HttpsTransportState.CLOSED) {
            throw new IllegalStateException("Cannot add a message to an HTTPS transport that is closed.");
        }

        // Codes_SRS_HTTPSTRANSPORT_11_003: [The function shall add a packet containing the message, callback, and callback context to the transport queue.]
        IotHubOutboundPacket packet = new IotHubOutboundPacket(message, callback, callbackContext);
        this.waitingList.add(packet);
    }

    /**
     * Sends all messages on the transport queue as a batched request.
     *
     * @throws IOException if the server could not be reached.
     * @throws IllegalStateException if the transport has not been opened or is
     * already closed.
     */
    public void sendMessages() throws IOException
    {
        // Codes_SRS_HTTPSTRANSPORT_11_029: [If the transport is closed, the function shall throw an IllegalStateException.]
        if (this.state == HttpsTransportState.CLOSED)
        {
            throw new IllegalStateException(
                    "Cannot send messages from an "
                            + "HTTPS transport that is closed.");
        }

        // Codes_SRS_HTTPSTRANSPORT_11_004: [If no previous send request had failed while in progress, the function shall batch as many messages as possible such that the batched message body is of size at most 256 kb.]
        // Codes_SRS_HTTPSTRANSPORT_11_012: [If a previous send request had failed while in progress, the function shall resend the request.]
        if (this.inProgressList.size() <= 0)
        {
            this.moveWaitingListToInProgressList();
        }
        if (this.inProgressList.size() <= 0)
        {
            return;
        }

        HttpsMessage msg;
        try
        {
            // Codes_SRS_HTTPSTRANSPORT_11_013: [If no messages fit using the batch format, the function shall send a single message without the batch format.] 
            msg = this.inProgressListToMessage();
        }
        catch (SizeLimitExceededException e)
        {
            // should never happen, since inProgressList is populated with
            // either a single message to be sent in unbatched format or
            // multiple messages that fit in a batch.
            throw new IllegalStateException(e);
        }

        // Codes_SRS_HTTPSTRANSPORT_11_008: [The request shall be sent to the IoT Hub given in the configuration from the constructor.]
        // Codes_SRS_HTTPSTRANSPORT_11_005: [The function shall configure a valid HTTPS request and send it to the IoT Hub.]
        // Codes_SRS_HTTPSTRANSPORT_11_014: [If the send request fails while in progress, the function shall throw an IOException.]
        // Codes_SRS_HTTPSTRANSPORT_11_017: [If an invalid URI is generated from the configuration given in the constructor, the function shall throw a URISyntaxException.]
        IotHubStatusCode status = this.connection.sendEvent(msg);

        // Codes_SRS_HTTPSTRANSPORT_11_006: [The function shall add a packet containing the callbacks, contexts, and response for all sent messages to the callback queue.]
        this.moveInProgressListToCallbackList(status);
    }

    /**
     * Invokes the callbacks for all completed requests.
     *
     * @throws IllegalStateException if the transport has not been opened or is
     * already closed.
     */
    public void invokeCallbacks()
    {
        // Codes_SRS_HTTPSTRANSPORT_11_031: [If the transport is closed, the function shall throw an IllegalStateException.]
        if (this.state == HttpsTransportState.CLOSED)
        {
            throw new IllegalStateException(
                    "Cannot invoke callbacks from "
                            + "an HTTPS transport that is closed.");
        }

        // Codes_SRS_HTTPSTRANSPORT_11_007: [The function shall invoke all callbacks on the callback queue.]
        while (!this.callbackList.isEmpty())
        {
            // Codes_SRS_HTTPSTRANSPORT_11_016: [If an exception is thrown during the callback, the function shall drop the callback from the queue.]
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
     * @throws IOException if the server could not be reached, or if the
     * response from sending the IoT Hub message result does not have
     * status code {@link IotHubStatusCode#OK_EMPTY}.
     * @throws IllegalStateException if the transport has not been opened or is
     * already closed.
     */
    public void handleMessage() throws IOException
    {
        // Codes_SRS_HTTPSTRANSPORT_11_033: [If the transport is closed, the function shall throw an IllegalStateException.]
        if (this.state == HttpsTransportState.CLOSED)
        {
            throw new IllegalStateException(
                    "Cannot handle a message using "
                            + "an HTTPS transport that is already closed.");
        }

        MessageCallback callback =
                this.config.getMessageCallback();
        Object context = this.config.getMessageContext();
        if (callback == null)
        {
            return;
        }

        // Codes_SRS_HTTPSTRANSPORT_11_009: [The function shall poll the IoT Hub for messages.]
        // Codes_SRS_HTTPSTRANSPORT_11_010: [If a message is found and a message callback is registered, the function shall invoke the callback on the message.] 
        // Codes_SRS_HTTPSTRANSPORT_11_018: [If an invalid URI is generated from the configuration given in the constructor, the function shall throw a URISyntaxException.]
        // Codes_SRS_HTTPSTRANSPORT_11_019: [If the IoT Hub could not be reached, the function shall throw an IOException.]
        Message message = this.connection.receiveMessage();
        if (message != null)
        {
            IotHubMessageResult result = callback.execute(message, context);

            // Codes_SRS_HTTPSTRANSPORT_11_011: [The function shall return the message result (one of COMPLETE, ABANDON, or REJECT) to the IoT Hub.]
            // Codes_SRS_HTTPSTRANSPORT_11_020: [If the response from sending the IoT Hub message result does not have status code OK_EMPTY, the function shall throw an IOException.] 
            this.connection.sendMessageResult(result);
        }
    }

    /**
     * Returns true if the transport has no more messages to handle, and false
     * otherwise.
     *
     * @return true if the transport has no more messages to handle, and false
     * otherwise.
     */
    public boolean isEmpty()
    {
        // Codes_SRS_HTTPSTRANSPORT_11_015: [The function shall return true if the waiting list, in progress list, and callback list are all empty, and false otherwise.]
        return this.waitingList.isEmpty() && this.inProgressList.isEmpty() && this.callbackList.isEmpty();
    }

    /**
     * Moves as many messages as can be sent in one HTTPS request from the
     * waiting list to the in-progress list. If a single message is moved to the
     * in-progress list, this indicates that the message is to be sent in the
     * un-batched message format.
     */
    protected void moveWaitingListToInProgressList()
    {
        HttpsBatchMessage batch = new HttpsBatchMessage();
        while (!this.waitingList.isEmpty())
        {
            IotHubOutboundPacket packet = this.waitingList.peek();
            try
            {
                HttpsSingleMessage httpsMsg =
                        HttpsSingleMessage.parseHttpsMessage(
                                packet.getMessage());
                batch.addMessage(httpsMsg);
            }
            catch (SizeLimitExceededException e)
            {
                break;
            }

            this.waitingList.remove();
            this.inProgressList.add(packet);
        }

        if (!this.waitingList.isEmpty() && batch.numMessages() <= 0)
        {
            IotHubOutboundPacket packet = this.waitingList.remove();
            this.inProgressList.add(packet);
        }
    }

    /**
     * Generates the message, possibly batched, to be sent using the messages in
     * the in-progress list.
     *
     * @return the message to be sent.
     *
     * @throws SizeLimitExceededException if the messages in {@code inProgressList}
     * exceed the service-bound message size limit of 255 kb.
     * @throws NoSuchElementException if {@code inProgressList} is empty.
     */
    protected HttpsMessage inProgressListToMessage()
            throws SizeLimitExceededException
    {
        HttpsMessage msg = null;
        int inProgressListSize = this.inProgressList.size();
        if (inProgressListSize < 1)
        {
            throw new NoSuchElementException(
                    "Cannot convert an empty in progress list to a message.");
        }
        if (inProgressListSize == 1)
        {
            msg = HttpsSingleMessage.parseHttpsMessage(
                    this.inProgressList.peek().getMessage());
        }
        else if (this.inProgressList.size() > 1)
        {
            HttpsBatchMessage batch = new HttpsBatchMessage();
            for (IotHubOutboundPacket packet : this.inProgressList)
            {
                HttpsSingleMessage httpsMsg =
                        HttpsSingleMessage.parseHttpsMessage(
                                packet.getMessage());
                batch.addMessage(httpsMsg);
            }

            msg = batch;
        }

        return msg;
    }

    /**
     * Moves messages from the in progress list to the callback list, updating
     * the message packets with the status code from the response.
     *
     * @param status the status code returned by the IoT Hub.
     */
    protected void moveInProgressListToCallbackList(IotHubStatusCode status)
    {
        while (!this.inProgressList.isEmpty())
        {
            IotHubOutboundPacket packet = this.inProgressList.remove();

            IotHubEventCallback callback = packet.getCallback();
            if (callback != null)
            {
                IotHubCallbackPacket callbackPacket =
                        new IotHubCallbackPacket(status,
                                packet.getCallback(),
                                packet.getContext());

                this.callbackList.add(callbackPacket);
            }
        }
    }
}
