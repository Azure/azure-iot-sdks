/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iothub.transport.amqps;

import com.microsoft.azure.iothub.DeviceClientConfig;
import com.microsoft.azure.iothub.IotHubClientProtocol;
import com.microsoft.azure.iothub.IotHubMessageResult;
import com.microsoft.azure.iothub.auth.IotHubSasToken;
import com.microsoft.azure.iothub.net.IotHubUri;
import com.microsoft.azure.iothub.transport.TransportUtils;
import org.apache.qpid.proton.*;
import org.apache.qpid.proton.engine.BaseHandler;
import org.apache.qpid.proton.engine.Event;
import org.apache.qpid.proton.message.Message;
import org.apache.qpid.proton.reactor.Reactor;

import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.util.*;
import java.util.concurrent.*;
import java.util.concurrent.TimeoutException;

/**
 * An AMQPS IotHub Session for a device and an IotHub. This class contains functionality for
 */
public final class AmqpsIotHubConnection extends BaseHandler {
    private static final int DEFAULT_DELIVERY_WAIT_TIME_SECONDS = 5;

    private IotHubReactor iotHubReactor;

    /** Future that gets completed once the reactor is ready to begin sending and receiving */
    protected CompletableFuture<Boolean> reactorReady;

    /** The state of the Reactor. */
    public enum ReactorState
    {
        OPEN, CLOSED
    } ReactorState state;

    /** Semaphore to restrict multiple calls to asynchronously run the Reactor. */
    private Semaphore reactorSemaphore = new Semaphore(1);

    //==============================================================================
    //Connection Variables
    //==============================================================================
    /** The address string of the IoT Hub. */
    private String hostName;
    /** The username string to use SASL authentication. */
    private String userName;
    /** The ID for the associated device. */
    private String deviceID;
    /** Indicates if use AMQP over WEBSOCKET or AMQP */
    private final IotHubClientProtocol iotHubClientProtocol;

    /** The {@link DeviceClientConfig} for the associated device. */
    protected DeviceClientConfig config;

    //==============================================================================
    //Sender Variables
    //==============================================================================
    /** Map of Proton-J {@link org.apache.qpid.proton.engine.Delivery} object hash codes to the corresponding message {@link Tuple}. */
    private Map<Integer, Tuple<CompletableFuture<Boolean>, byte[], Object>> inProgressMessageMap;
    /** Integer representing the maximum allowable size of the {@link AmqpsIotHubConnection#inProgressMessageMap}. This integer equals the AMQP link credit received when the Sender link is attached. */
	private int maxQueueSize;

    //==============================================================================
    //Receiver Variables
    //==============================================================================
    /** Queue of messaged received. */
    private Queue<Message> receivedMessageQueue;

    //==============================================================================
    //Class Variables
    //==============================================================================
    /** The underlying {@link AmqpsIotHubConnectionBaseHandler} implementing lower level callbacks.*/
    private AmqpsIotHubConnectionBaseHandler amqpsHandler;
    /** The completion status for this {@link AmqpsIotHubConnection}. */
    private CompletableFuture<Boolean> completionStatus;
    /** The last received {@link AmqpsMessage} from the {@link AmqpsIotHubConnectionBaseHandler}. */
    private AmqpsMessage lastMessage = null;

    /**
     * Constructor to set up connection parameters using the {@link DeviceClientConfig}.
     *
     * @param config The {@link DeviceClientConfig} corresponding to the device associated with this {@link com.microsoft.azure.iothub.DeviceClient}.
     */
    public AmqpsIotHubConnection(DeviceClientConfig config, IotHubClientProtocol iotHubClientProtocol){
        // Codes_SRS_AMQPSIOTHUBCONNECTION_14_002: [The constructor shall throw a new IllegalArgumentException if any of the parameters of the configuration is null or empty.]
        if(config == null){
            throw new IllegalArgumentException("The DeviceClientConfig cannot be null.");
        }
        if(config.getIotHubHostname() == null || config.getIotHubHostname().length() == 0)
        {
            throw new IllegalArgumentException("hostName cannot be null or empty.");
        }
        if (config.getDeviceId() == null || config.getDeviceId().length() == 0)
        {
            throw new IllegalArgumentException("deviceID cannot be null or empty.");
        }
        if(config.getIotHubName() == null || config.getIotHubName().length() == 0)
        {
            throw new IllegalArgumentException("hubName cannot be null or empty.");
        }
        if(config.getDeviceKey() == null || config.getDeviceKey().length() == 0)
        {
            throw new IllegalArgumentException("deviceKey cannot be null or empty.");
        }

        // Codes_SRS_AMQPSIOTHUBCONNECTION_14_001: [The constructor shall save the configuration.]
        this.config = config;

        String iotHubHostname = this.config.getIotHubHostname();
        String iotHubName = this.config.getIotHubName();
        String deviceId = this.config.getDeviceId();
        String iotHubUser = deviceId + "@sas." + iotHubName;

        // Codes_SRS_AMQPSIOTHUBCONNECTION_14_006: [The constructor shall initialize a new private map for messages that are in progress.]
        // Codes_SRS_AMQPSIOTHUBCONNECTION_14_007: [The constructor shall initialize new private Futures for the status of the Connection and Reactor.]
        this.hostName = iotHubHostname;
        this.userName = iotHubUser;
        this.deviceID = deviceId;
        this.iotHubClientProtocol = iotHubClientProtocol;

        // Codes_SRS_AMQPSIOTHUBCONNECTION_14_004: [The constructor shall set it’s state to CLOSED.]
        this.state = ReactorState.CLOSED;

        // Codes_SRS_AMQPSIOTHUBCONNECTION_14_005: [The constructor shall initialize a new private queue for received messages.]
        // Codes_SRS_AMQPSIOTHUBCONNECTION_14_006: [The constructor shall initialize a new private map for messages that are in progress.]
        receivedMessageQueue = new LinkedBlockingQueue<>();
        inProgressMessageMap = new HashMap<>();

        this.maxQueueSize = -1;

        // Codes_SRS_AMQPSIOTHUBCONNECTION_14_007: [The constructor shall initialize new private Futures for the status of the Connection and Reactor.]
        completionStatus = new CompletableFuture<>();
        reactorReady = new CompletableFuture<>();
    }

    /**
     * Opens the {@link AmqpsIotHubConnection} creating a new {@link AmqpsIotHubConnectionBaseHandler}.
     * <p>
     *     If the current {@link AmqpsIotHubConnection.ReactorState} is not OPEN, this method
     *     will create a new {@link IotHubSasToken} and use it to create a new {@link AmqpsIotHubConnectionBaseHandler}. This method will
     *     start the {@link Reactor}, set the current {@link AmqpsIotHubConnection.ReactorState}
     *     to OPEN, and open the {@link AmqpsIotHubConnection} for sending.
     * </p>
     *
     * @throws IOException if the {@link AmqpsIotHubConnectionBaseHandler} has not been initialized.
     * @throws InterruptedException if there is a problem acquiring the semaphore for the {@link Reactor}.
     * @throws ExecutionException If the {@link CompletableFuture} in {@link AmqpsIotHubConnection#reactorReady()} completed exceptionally
     */
    public void open() throws IOException, InterruptedException, ExecutionException {
        // Codes_SRS_AMQPSIOTHUBCONNECTION_14_011: [If the AMQPS connection is already open, the function shall do nothing.]
        if(this.state != ReactorState.OPEN)
        {
            IotHubSasToken sasToken = new IotHubSasToken(
                    IotHubUri.getResourceUri(this.config.getIotHubHostname(), this.config.getDeviceId()),
                    this.config.getDeviceKey(),
                    System.currentTimeMillis() / 1000l + this.config.getTokenValidSecs() + 1l);

            // Codes_SRS_AMQPSIOTHUBCONNECTION_14_008: [The function shall initialize its AmqpsIotHubConnectionBaseHandler
            // using the saved host name, user name, device ID and sas token.]
            this.amqpsHandler = new AmqpsIotHubConnectionBaseHandler(this.hostName,
                    this.userName, sasToken.toString(), this.deviceID, this.iotHubClientProtocol, this);

            // Codes_SRS_AMQPSIOTHUBCONNECTION_14_009: [The function shall open the Amqps connection and trigger the Reactor (Proton) to begin running.]
            // Codes_SRS_AMQPSIOTHUBCONNECTION_14_012: [If the AmqpsIotHubConnectionBaseHandler becomes invalidated before the Reactor (Proton) starts, the function shall throw an IOException.]
            this.startReactorAsync();

            // Codes_SRS_AMQPSIOTHUBCONNECTION_14_010: [Once the Reactor (Proton) is ready, the function shall set its state to OPEN.]
            // Codes_SRS_AMQPSIOTHUBCONNECTION_14_031: [ The function shall get the link credit from it's AmqpsIotHubConnectionBaseHandler and set the private maxQueueSize member variable. ]
			// Codes_SRS_AMQPSIOTHUBCONNECTION_14_032: [ The function shall successfully complete it’s CompletableFuture status member variable. ]
			try {
                this.reactorReady();
                //This is a blocking call, will return once the link credit is available from the AmqpsIotHubBaseHandler
                this.maxQueueSize = this.amqpsHandler.getLinkCredit();
            }catch(TimeoutException e){
                this.amqpsHandler.shutdown();
                throw new ExecutionException("The request to get the link credit from the AmqpsIotHubBaseHandler timed out.", e);
            }catch(Exception e){
                this.amqpsHandler.shutdown();
                throw e;
            }
            this.completionStatus.complete(new Boolean(true));
            this.state = ReactorState.OPEN;
        }
        //TODO: Should this wrap all exceptions in an IOException and only throw that?
    }

    /**
     * Closes the {@link AmqpsIotHubConnection} and invalidates the {@link Reactor} object.
     * <p>
     *     If the current {@link AmqpsIotHubConnection.ReactorState} is not CLOSED, this function
     *     will invalidate the current {@link Reactor} object, set the current {@link AmqpsIotHubConnection.ReactorState}
     *     to CLOSED, and free the current {@link AmqpsIotHubConnectionBaseHandler}.
     * </p>
     */
    public synchronized void close(){
        // Codes_SRS_AMQPSIOTHUBCONNECTION_14_013: [The function shall invalidate the private Reactor (Proton) member variable.]
        // Codes_SRS_AMQPSIOTHUBCONNECTION_14_014: [The function shall free the AmqpsIotHubConnectionBaseHandler.]
        // Codes_SRS_AMQPSIOTHUBCONNECTION_14_015: [The function shall close the AMQPS connection.]
        // Codes_SRS_AMQPSIOTHUBCONNECTION_14_016: [If the AMQPS connection is already closed, the function shall do nothing.]
		// Codes_SRS_AMQPSIOTHUBCONNECTION_14_033: [The function shall close the AmqpsIotHubConnectionBaseHandler.]
		// Codes_SRS_AMQPSIOTHUBCONNECTION_14_034: [The function shall exceptionally complete all remaining messages that are currently in progress and clear the queue.]
		if(this.state != ReactorState.CLOSED)
        {
            this.amqpsHandler.shutdown();
            this.clearInProgressMap();
            this.freeReactor();
            this.freeHandler();
            this.state = ReactorState.CLOSED;
            this.maxQueueSize = -1;
        }
    }

    /**
     * Pulls a message from the {@link AmqpsIotHubConnection} if there is one to receive.
     * @return the first received/unconsumed message. Null if there are no messages to consume.
     * @throws IOException if the {@link AmqpsIotHubConnectionBaseHandler} has not been initialized.
     */
    public Message consumeMessage() throws IOException {
        Message message = null;

        // Codes_SRS_AMQPSIOTHUBCONNECTION_14_019: [The function shall attempt to remove a message from the queue.]
        if (this.receivedMessageQueue.size() > 0)
        {
            message = this.receivedMessageQueue.remove();
            this.lastMessage = (AmqpsMessage) message;
        }
        // Codes_SRS_AMQPSIOTHUBCONNECTION_14_020: [The function shall return the message if one was pulled from the queue, otherwise it shall return null.]
        return message;
    }

    /**
     * Sends the message result for the previously received
     * message.
     *
     * @param result the message result (one of {@link IotHubMessageResult#COMPLETE},
     *               {@link IotHubMessageResult#ABANDON}, or {@link IotHubMessageResult#REJECT}).
     *
     * @throws IllegalStateException if {@code sendMessageResult} is called before
     * {@link #consumeMessage()} is called and has returned a {@link Message}.
     * @throws IOException if the IoT Hub could not be reached or if any other exception is thrown while attempting to acknowledge.
     */
    public void sendMessageResult(IotHubMessageResult result) throws IOException {
        // Codes_SRS_AMQPSIOTHUBCONNECTION_14_024: [If the AMQPS Connection is closed, the function shall throw an IllegalStateException.]
        if(this.state == ReactorState.CLOSED){
            throw new IllegalStateException("The AMQPS IotHub Connection is currently closed. Call open() before attempting to acknowledge a message.");
        }

        // Codes_SRS_AMQPSIOTHUBCONNECTION_14_022: [If sendMessageResult(result) is called before a message is received, the function shall throw an IllegalStateException.]
        if (this.lastMessage == null)
        {
            throw new IllegalStateException("Cannot send a message "
                    + "result before a message is received.");
        }

        try {
            // Codes_SRS_AMQPSIOTHUBCONNECTION_14_021: [If the message result is COMPLETE, ABANDON, or REJECT, the function shall acknowledge the last message with acknowledgement type COMPLETE, ABANDON, or REJECT respectively.]
            switch (result) {
                case COMPLETE:
                    this.lastMessage.acknowledge(AmqpsMessage.ACK_TYPE.COMPLETE);
                    break;
                case REJECT:
                    this.lastMessage.acknowledge(AmqpsMessage.ACK_TYPE.REJECT);
                    break;
                case ABANDON:
                    this.lastMessage.acknowledge(AmqpsMessage.ACK_TYPE.ABANDON);
                    break;
                default:
                    // should never happen.
                    throw new IllegalStateException(
                            "Invalid IoT Hub message result.");
            }
        } catch(Exception e){
            // Codes_SRS_AMQPSIOTHUBCONNECTION_14_023: [If the acknowledgement fails, the function shall throw an IOException.]
            throw new IOException(e);
        }
    }

    /**
     * Creates a binary message using the given content string and queues it for sending.
     * @param content The content string to send.
     * @return A {@link Boolean} {@link CompletableFuture} representing the whether the message has been sent and received.
     * @throws IOException If the current {@link AmqpsIotHubConnection.ReactorState} is CLOSED or if {@link AmqpsIotHubConnectionBaseHandler} has not been initialized.
     */
    public CompletableFuture<Boolean> scheduleSend(String content) throws IOException {
        return this.scheduleSend(content.getBytes(StandardCharsets.UTF_8));
    }

    /**
     * Creates a binary message using the given content array and queues it for sending.
     * @param content The content byte array of the message to send.
     * @return A {@link Boolean} {@link CompletableFuture} representing the whether the message has been sent and received.
     * @throws IOException If the current {@link AmqpsIotHubConnection.ReactorState} is CLOSED or if {@link AmqpsIotHubConnectionBaseHandler} has not been initialized.
     */
    public CompletableFuture<Boolean> scheduleSend(byte[] content) throws IOException {
        // Codes_SRS_AMQPSSENDER_14_011: [If a messageId is not provided, the function shall create a binary message using the given content and a null messageId.]
        return this.scheduleSend(content, null);
    }

    /**
     * Creates a binary message using the given content array and messageId. Initializes and starts the Proton reactor. Sends the created message.
     * @param content The content byte array of the message to send.
     * @param messageId The messageId of the message.
     * @return A {@link Boolean} {@link CompletableFuture} representing the whether the message has been sent and received.
     * @throws IOException If the current {@link AmqpsIotHubConnection.ReactorState} is CLOSED or if {@link AmqpsIotHubConnectionBaseHandler} has not been initialized.
     */
    public CompletableFuture<Boolean> scheduleSend(byte[] content, Object messageId) throws IOException {
        // Codes_SRS_AMQPSIOTHUBCONNECTION_14_029: [If the AMQPS Connection is closed, the function shall throw an IllegalStateException.]
        if(this.state == ReactorState.CLOSED){
            throw new IllegalStateException("The AMQPS IotHub Connection is currently closed. Call open() before attempting to schedule a message.");
        }

        if(amqpsHandler != null) {
            // Codes_SRS_AMQPSIOTHUBCONNECTION_14_026: [The function shall create a new CompletableFuture for the message acknowledgement.]
            // Codes_SRS_AMQPSIOTHUBCONNECTION_14_027: [The function shall create a new Tuple containing the CompletableFuture, message content, and message ID.
            // Codes_SRS_AMQPSIOTHUBCONNECTION_14_028: [The function shall acquire a lock and attempt to send the message.]
            CompletableFuture<Boolean> future = new CompletableFuture<>();
            Tuple<CompletableFuture<Boolean>, byte[], Object> message = new Tuple(future, content, messageId);
            send(message);
            return future;
        } else {
            // Codes_SRS_AMQPSIOTHUBCONNECTION_14_025: [If the AmqpsIotHubConnectionBaseHandler has not been initialized, the function shall throw a new IOException and exceptionally complete it’s CompletableFuture status member variable with the same exception.]
            this.completionStatus.completeExceptionally(new IOException("The Handler has not been initialized. Call open before sending."));
            throw new IOException("The Handler has not been initialized. Call open before sending.");
        }
    }

    //==============================================================================
    //Protected Methods
    //==============================================================================

    /**
     * This blocking function attempts to send the message passed in as a parameter.
     * This {@link AmqpsIotHubConnection} handles all calls to this method.
     *
     * <p>
     *     Only the first call to this method will result in an attempt to send. Until the message has been sent, all other
     *     calls to this method will block. Once a message has been sent and this method notified that it has been sent,
     *     this method will be invoked again if was previously another call to send a message.
     *
     *     If a message has been passed down to the handler for sending but the message isn't sent after a default constant number of
     *     seconds, the {@link AmqpsTransport} will set an ERROR status code on the message and it will placed back onto the queue.
     * </p>
     * @throws IOException If {@link AmqpsIotHubConnectionBaseHandler} has not been initialized.
     */
    protected synchronized void send(Tuple<CompletableFuture<Boolean>, byte[], Object> message) throws IOException
    {
        if(this.state == ReactorState.CLOSED)
        {
            throw new IllegalStateException("The AMQPS IotHub Connection is currently closed. Call open() before attempting to send a message.");
        }
        if(message != null)
        {
            if (this.inProgressMessageMap.size() >= this.maxQueueSize * 0.9)
            {
                message.V1.completeExceptionally(new Throwable("Insufficient link credit to send message."));
            }
            else
            {
                try
                {
                    //Use the content and ID fields of the input message to have the handler create and send the message
                    CompletableFuture<Integer> deliveryFuture = amqpsHandler.createBinaryMessage(message.V2, message.V3);

                    //Wait for a period of time before rejecting the message
                    new Thread(() ->
                    {
                        try
                        {
                            Thread.sleep(DEFAULT_DELIVERY_WAIT_TIME_SECONDS * 1000);
                            deliveryFuture.completeExceptionally(new Throwable("Default timeout exceeded before this message was sent."));
                        }
                        catch (InterruptedException e)
                        {
                            e.printStackTrace();
                        }
                    }).start();

                    //Wait for the deliveryFuture to be completed, providing the delivery hash code.
                    //When this future completes, the message has been SENT
                    Integer deliveryHash = deliveryFuture.get();
                    inProgressMessageMap.put(deliveryHash, message);
                }
                // There was a problem sending, exceptionally complete the future causing the message to be put back on the queue.
                catch (Exception e)
                {
                    if (message != null)
                    {
                        message.V1.completeExceptionally(e);
                    }
                    this.fail(e);
                }
            }
        }
        else
        {
            throw new IOException("Cannot send an unitialized message.");
        }
    }

    /**
     * Informs the {@link AmqpsTransport} that the message corresponding to the {@link org.apache.qpid.proton.engine.Delivery}
     * {@code hash} has or has not been successfully received by the service using the boolean {@code result}.
     *
     * @param hash The hash code of the {@link org.apache.qpid.proton.engine.Delivery} corresponding to the message.
     * @param result The boolean result status of the sent message.
     */
    protected synchronized void acknowledge(int hash, boolean result){
        Tuple<CompletableFuture<Boolean>, byte[], Object> item = inProgressMessageMap.remove(hash);
        item.V1.complete(new Boolean(result));
        //If the message queue has been successfully exhausted, complete the completionStatus future successfully.
    }

    /**
     * Adds a message to the received message queue.
     * @param m The received message.
     */
    protected void addMessage(Message m) {
        this.receivedMessageQueue.add(m);
    }

    /**
     * Invalidates the {@link Reactor} member variable.
     */
    protected void freeReactor(){
        this.iotHubReactor = null;
    }

    /**
     * Invalidates the {@link AmqpsIotHubConnectionBaseHandler} member variable.
     */
    protected void freeHandler(){
        this.amqpsHandler = null;
    }

    /**
     *
     */
    protected void clearInProgressMap(){
        for (Tuple<CompletableFuture<Boolean>, byte[], Object> item : this.inProgressMessageMap.values()) {
            item.V1.completeExceptionally(new Throwable("Connection closed before message acknowledgement received."));
        }
        this.inProgressMessageMap.clear();
    }

    /**
     * Every {@link AmqpsIotHubConnection} has a {@link CompletableFuture} {@code completionStatus} variable. This can be used by the {@link AmqpsTransport}
     * to know when/if the {@link AmqpsIotHubConnection} has completed work successfully or not.
     *
     * <p>
     *     The {@code completionStatus} variable is completed whenever the {@link AmqpsIotHubConnection} has exhausted the current message
     *     queue successfully or whenever there is an error sending a message.
     * </p>
     * @return The boolean value of the completion status.
     * @throws ExecutionException If this future completed exceptionally
     * @throws InterruptedException If the current thread was interrupted
     */
    protected boolean getCompletionStatus() throws ExecutionException, InterruptedException {
        boolean value = this.completionStatus.get().booleanValue();
        return value;
    }

    /**
     * Waits if necessary and returns once the reactor is ready.
     * @throws ExecutionException If this future completed exceptionally
     * @throws InterruptedException If the current thread was interrupted
     */
    protected void reactorReady() throws ExecutionException, InterruptedException{
        this.reactorReady.get();
    }

    /**
     * Completes the {@link CompletableFuture} {@code completionStatus} variable with an exception using the given string.
     * @param s The string content to place in the thrown exception.
     */
    protected void fail(String s) {
        //TODO: Should also exceptionally complete anything currently in the queue that hasn't been sent.
        this.completionStatus = new CompletableFuture<>();
        this.completionStatus.completeExceptionally(new Throwable(s));
    }

    protected void fail(Throwable t){
        this.completionStatus = new CompletableFuture<>();
        this.completionStatus.completeExceptionally(t);
    }

    //==============================================================================
    //Private Methods
    //==============================================================================

    /**
     * Asynchronously runs the Proton {@link Reactor} accepting and sending messages. Any other call to this method on this
     * {@link AmqpsIotHubConnection} will block until the {@link Reactor} terminates and this {@link AmqpsIotHubConnection} closes.
     * Normally, this method should only be called once by the {@link #open()} method until the {@link AmqpsIotHubConnection} has been closed.
     * @throws IOException if the {@link AmqpsIotHubConnectionBaseHandler} has not been initialized.
     * @throws InterruptedException if there is a problem acquiring the semaphore.
     */
    private synchronized void startReactorAsync() throws IOException, InterruptedException {
        //Acquire permit to continue execution of this method and spawn a new thread.
        reactorSemaphore.acquire();

        if(this.amqpsHandler != null) {
            Reactor reactor = Proton.reactor(this);
            this.iotHubReactor = new IotHubReactor(reactor);

            new Thread(() -> {
                try {
                    iotHubReactor.run();

                    //Release the semaphore and make permit available allowing for the next reactor thread to spawn.
                    reactorSemaphore.release();
                }
                catch(Exception e)
                {
                    reactorSemaphore.release();
                }
            }).start();
        } else {
            throw new IOException("The Handler has not been initialized. Ensure that the AmqpsIotHubConnection is in an OPEN state by calling open().");
        }
    }

    //==============================================================================
    //Reactor Overrides
    //==============================================================================

    /**
     * Event handler for reactor init event.
     * @param event Proton Event object
     */
    @Override
    public void onReactorInit(Event event){
        // Codes_SRS_AMQPSIOTHUBCONNECTION_14_030: [The event handler shall set the member AmqpsIotHubConnectionBaseHandler object to handle the connection events.]
        event.getReactor().connection(this.amqpsHandler);
    }

    //==============================================================================
    //Private Classes
    //==============================================================================

    private class Tuple<T1,T2,T3>{
        public T1 V1;
        public T2 V2;
        public T3 V3;

        public Tuple(T1 V1, T2 V2, T3 V3){
            this.V1 = V1;
            this.V2 = V2;
            this.V3 = V3;
        }
    }
}
