/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iothub.transport.amqps;

import com.sun.org.apache.xpath.internal.operations.Bool;
import org.apache.qpid.proton.Proton;
import org.apache.qpid.proton.engine.BaseHandler;
import org.apache.qpid.proton.engine.Event;
import org.apache.qpid.proton.engine.HandlerException;
import org.apache.qpid.proton.message.Message;
import org.apache.qpid.proton.reactor.Reactor;

import java.io.IOException;
import java.util.NoSuchElementException;
import java.util.Queue;
import java.util.concurrent.CompletableFuture;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.Semaphore;

/**
 * Instance of the QPID-Proton-J BaseHandler class that overrides the callbacks needed to handle high level open,
 * close, and message consumption. Initialize and use the AmqpsReceiverHandler class for low level amqps operations.
 */
public class AmqpsReceiver extends BaseHandler{

    private final String hostName;
    private final String userName;
    private final String sasToken;
    private final String deviceID;
    private Queue<Message> messageQueue;
    private AmqpsReceiverHandler amqpsReceiverHandler;
    private Reactor reactor;
    private Semaphore reactorSemaphore = new Semaphore(1);

    /**
     * Constructor to set up connection parameters
     * @param hostName address string of the IoT Hub (example: AAA.BBB.CCC)
     * @param userName username string to use SASL authentication (example: user@sas.[hostName])
     * @param deviceID ID from which this receiver communicates
     * @param sasToken  SAS token string
     */
    public AmqpsReceiver(String hostName, String userName, String deviceID, String sasToken){
        // Codes_SRS_AMQPSRECEIVER_14_001: [The constructor shall throw a new IllegalArgumentException if any of the input parameters is null or empty.]
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

        // Codes_SRS_AMQPSRECEIVER_14_002: [The constructor shall copy all input parameters to private member variables.]
        this.hostName = hostName;
        this.userName = userName;
        this.deviceID = deviceID;
        this.sasToken = sasToken;

        // Codes_SRS_AMQPSRECEIVER_14_003: [The constructor shall initialize a new private queue for messages.]
        messageQueue = new LinkedBlockingQueue<>();
    }

    /**
     * Creates an AmqpsReceiverHandler and stores it as a member variable
     * @throws IOException
     * @throws InterruptedException
     */
    public CompletableFuture<Boolean> open() throws IOException, InterruptedException {
        // Codes_SRS_AMQPSRECEIVER_14_004: [The function shall initialize it’s AmqpsReceiverHandler using the saved host name, user name, device ID and sas token.]
        // Codes_SRS_AMQPSRECEIVER_14_005: [The function shall open the Amqps connection and trigger the Reactor (Proton) to begin receiving.]
        this.amqpsReceiverHandler = new AmqpsReceiverHandler(this.hostName, this.userName, this.deviceID, this.sasToken, this);
        return this.beginReceive();
    }

    /**
     * Invalidates the AmqpsReceiverHandler member variable
     */
    public void close(){
        // Codes_SRS_AMQPSRECEIVER_14_006: [The function shall invalidate its AmqpsReceiverHandler.]
        this.amqpsReceiverHandler = null;
    }

    /**
     * Asynchronously runs the Proton Reactor accepting messages. Any other call to this method on the Receiver will
     * block until the reactor terminates and this receiver closes.
     * Normally, this method should only be called once by the open method until the Receiver has been closed.
     * @throws IOException
     * @throws InterruptedException
     */
    public synchronized CompletableFuture<Boolean> beginReceive() throws IOException, InterruptedException {
        //Acquire permit to continue execution of this method and spawn a new thread.
        // Codes_SRS_AMQPSRECEIVER_14_008: [The function shall attempt to acquire a permit from the semaphore.]
        reactorSemaphore.acquire();
        final CompletableFuture<Boolean> future = new CompletableFuture<Boolean>();
        if(this.amqpsReceiverHandler != null) {
            // Codes_SRS_AMQPSRECEIVER_14_009: [The function shall initialize its Reactor object using itself as the handler.]
            this.reactor = Proton.reactor(this);

            // Codes_SRS_AMQPSRECEIVER_14_010: [The function shall asynchronously run the reactor.]
            // Codes_SRS_AMQPSRECEIVER_14_011: [The function shall asynchronously close the Receiver after running the Reactor.]
            // Codes_SRS_AMQPSRECEIVER_14_012: [The function shall asynchronously release the semaphore.]
            new Thread(new Runnable(){
                @Override
                public void run(){
                    try {
                        reactor.run();

                        //Closing here should be okay. The reactor will only stop running if the connection is remotely
                        //or locally closed. The transport will attempt to receive a message and will get an exception
                        //causing it to create a new AmqpsIoTHubSession which will open a new AmqpsReceiver.
                        close();

                        //Release the semaphore and make permit available allowing for the next reactor thread to spawn.
                        reactorSemaphore.release();
                        future.complete(Boolean.TRUE);
                    } catch(HandlerException e){
                        // Codes_SRS_AMQPSRECEIVER_14_013: [If the Reactor throws a HandlerException while running, the thread shall release the semaphore and close the Receiver.]
                        close();
                        reactorSemaphore.release();
                        future.complete(Boolean.FALSE);
                    }
                }
            }).start();
        } else {
            // Codes_SRS_AMQPSRECEIVER_14_007: [If the AmqpsReceiverHandler has not been initialized, the function shall throw a new IOException.]
            throw new IOException("The Receiver Handler has not been initialized. Call open before receiving.");
        }
        return future;
    }

    /**
     * Pulls a message from the Receiver if there is one to receive.
     * @return the first received/unconsumed message. Null if there are no messages to consume.
     * @throws IOException
     */
    public Message consumeMessage() throws IOException {
        if(this.amqpsReceiverHandler!=null) {
            Message m = null;

            // Codes_SRS_AMQPSRECEIVER_14_015: [The function shall attempt to remove a message from the message queue.]
            try {
                m = this.messageQueue.remove();
            }
            // Codes_SRS_AMQPSRECEIVER_14_016: [The function shall return the message if one was pulled from the queue, otherwise it shall return null.]
            catch (NoSuchElementException e) {
                return null;
            }
            return m;
        } else {
            // Codes_SRS_AMQPSRECEIVER_14_014: [If the AmqpsReceiverHandler has not been initialized, the function shall throw a new IOException.]
            throw new IOException("The Receiver Handler is not initialized.");
        }
    }

    /**
     * Adds a message to the Receivers message queue
     * @param m
     */
    protected void addMessage(Message m){
        this.messageQueue.add(m);
    }

    /**
     * Event handler for reactor init event.
     * @param event Proton Event object
     */
    @Override
    public void onReactorInit(Event event){
        // Codes_SRS_AMQPSRECEIVER_14_017: [The event handler shall set the member AmqpsReceiverHandler object to handle the connection events.]
        event.getReactor().connection(this.amqpsReceiverHandler);
    }
}
