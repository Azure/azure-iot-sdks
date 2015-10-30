/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iothub.transport.amqps;

import org.apache.qpid.proton.Proton;
import org.apache.qpid.proton.engine.BaseHandler;
import org.apache.qpid.proton.engine.Event;
import org.apache.qpid.proton.engine.HandlerException;
import org.apache.qpid.proton.message.Message;
import org.apache.qpid.proton.reactor.Reactor;

import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.util.NoSuchElementException;
import java.util.Queue;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.Semaphore;

public class AmqpsComm extends BaseHandler {

    private final String hostName;
    private final String userName;
    private final String sasToken;
    private final String deviceID;
    private Queue<Message> receivedMessageQueue;
    private Queue<Tuple<byte[], Object>> toSendMessageQueue;
    private AmqpsIotHubBaseHandler amqpsHandler;
    private Reactor reactor;
    private Semaphore reactorSemaphore = new Semaphore(1);

    private boolean canSend = true;

    /**
     * Constructor to set up connection parameters
     * @param hostName address string of the IoT Hub (example: AAA.BBB.CCC)
     * @param userName username string to use SASL authentication (example: user@sas.[hostName])
     * @param deviceID ID from which this communicates
     * @param sasToken  SAS token string
     */
    public AmqpsComm(String hostName, String userName, String deviceID, String sasToken){
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

        this.hostName = hostName;
        this.userName = userName;
        this.deviceID = deviceID;
        this.sasToken = sasToken;

        receivedMessageQueue = new LinkedBlockingQueue<>();
        toSendMessageQueue = new LinkedBlockingQueue<>();
    }

    /**
     * Creates an AmqpsReceiverHandler and stores it as a member variable
     * @throws IOException
     * @throws InterruptedException
     */
    public void open() throws IOException, InterruptedException {
        this.amqpsHandler = new AmqpsIotHubBaseHandler(this.hostName, this.userName, this.deviceID, this.sasToken, this);
        this.begin();
    }

    /**
     * Invalidates the AmqpsReceiverHandler member variable
     */
    public void close(){
        this.receivedMessageQueue.clear();
        this.toSendMessageQueue.clear();
        this.amqpsHandler = null;
    }

    /**
     * Asynchronously runs the Proton Reactor accepting messages. Any other call to this method on the Receiver will
     * block until the reactor terminates and this receiver closes.
     * Normally, this method should only be called once by the open method until the Receiver has been closed.
     * @throws IOException
     * @throws InterruptedException
     */
    public synchronized void begin() throws IOException, InterruptedException {
        //Acquire permit to continue execution of this method and spawn a new thread.
        reactorSemaphore.acquire();

        if(this.amqpsHandler != null) {
            this.reactor = Proton.reactor(this);

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
                    } catch(HandlerException e){
                        close();
                        reactorSemaphore.release();
                    }
                }
            }).start();
        } else {
            throw new IOException("The Handler has not been initialized. Call open before receiving.");
        }
    }

    /**
     * Pulls a message from the Receiver if there is one to receive.
     * @return the first received/unconsumed message. Null if there are no messages to consume.
     * @throws IOException
     */
    public Message consumeMessage() throws IOException {
        if(this.amqpsHandler!=null) {
            Message m = null;

            // Codes_SRS_AMQPSRECEIVER_14_015: [The function shall attempt to remove a message from the message queue.]
            try {
                m = this.receivedMessageQueue.remove();
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

    protected void addMessage(Message m) {
        this.receivedMessageQueue.add(m);
    }

    /**
     * Creates a binary message using the given content string. Initializes and starts the Proton reactor. Sends the created message.
     * @param content
     * @throws IOException
     */
    public void scheduleSend(String content) throws IOException {
        this.scheduleSend(content.getBytes(StandardCharsets.UTF_8));
    }

    /**
     * Creates a binary message using the given content array. Initializes and starts the Proton reactor. Sends the created message.
     * @param content content byte array of the message
     * @throws IOException
     * @throws HandlerException
     */
    public void scheduleSend(byte[] content) throws IOException, HandlerException {
        // Codes_SRS_AMQPSSENDER_14_011: [If a messageId is not provided, the function shall create a binary message using the given content and a null messageId.]
        this.scheduleSend(content, null);
    }

    /**
     * Creates a binary message using the given content array and messageId. Initializes and starts the Proton reactor. Sends the created message.
     * @param content content byte array of the message
     * @param messageId messageId of the message
     * @throws IOException
     * @throws HandlerException
     */
    public void scheduleSend(byte[] content, Object messageId) throws IOException, HandlerException {
        if(amqpsHandler != null) {

            this.toSendMessageQueue.add(new Tuple(content, messageId));
            send();

        } else {
            throw new IOException("The Handler has not been initialized. Call open before sending.");
        }
    }

    protected void lockSending() {this.canSend = false;}
    protected void openSending() {this.canSend = true; send();}

    /**
     *
     */
    private void send(){
        if(canSend) {
            try {
                Tuple message = this.toSendMessageQueue.remove();
                amqpsHandler.createBinaryMessage((byte[]) message.o1, message.o2);
            } catch (NoSuchElementException e) {
                return;
            }
        }
    }

    /**
     * Event handler for reactor init event.
     * @param event Proton Event object
     */
    @Override
    public void onReactorInit(Event event){
        event.getReactor().connection(this.amqpsHandler);
    }

    public class Tuple<A,B>{
        public A o1;
        public B o2;

        public Tuple(A o1, B o2){
            this.o1 = o1;
            this.o2 = o2;
        }
    }
}
