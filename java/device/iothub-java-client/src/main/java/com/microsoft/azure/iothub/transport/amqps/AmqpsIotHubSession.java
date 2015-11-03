// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

package com.microsoft.azure.iothub.transport.amqps;

import com.microsoft.azure.iothub.*;
import com.microsoft.azure.iothub.auth.IotHubSasToken;
import com.microsoft.azure.iothub.net.IotHubUri;
import org.apache.qpid.proton.amqp.Binary;
import org.apache.qpid.proton.amqp.messaging.Data;
import org.apache.qpid.proton.amqp.messaging.Properties;
import org.apache.qpid.proton.engine.HandlerException;
import org.apache.qpid.proton.message.impl.MessageImpl;

import java.io.IOException;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.nio.ByteBuffer;

/**
 * An AMQPS session for a device and an IoT Hub. Contains functionality for
 * synchronously connecting to the different IoT Hub resource URIs. The AMQPS
 * session must be manually managed and needs to be explicitly closed when it is
 * no longer in use.
 */
public final class AmqpsIotHubSession
{
    /**
     * The time, in milliseconds, that a message consumer will wait when
     * receiving a message.
     */
    public static final int AMQPS_RECEIVE_TIMEOUT_MILLIS = 3000;

    /** The state of the AMQPS session. */
    protected enum AmqpsIotHubSessionState
    {
        OPEN, CLOSED
    }

    protected AmqpsIotHubSessionState state;

    /** The AMQPS session lock. */
    protected static final Object AMQPS_SESSION_LOCK = new Object();

    /** The client configuration. */
    protected final DeviceClientConfig config;

    protected AmqpsSender sender = null;
    protected AmqpsReceiver receiver = null;

    /** The last AMQPS message received by the device. */
    protected AmqpsMessage lastMessage;


    /**
     * Constructs an instance from the given {@link DeviceClientConfig}
     * object.
     *
     * @param config the client configuration.
     */
    public AmqpsIotHubSession(DeviceClientConfig config)
    {
        synchronized (AMQPS_SESSION_LOCK)
        {
            // Codes_SRS_AMQPSIOTHUBSESSION_11_022: [The constructor shall save the configuration.]
            this.config = config;
            this.state = AmqpsIotHubSessionState.CLOSED;
        }
    }

    /**
     * Establishes a session for the device and IoT Hub given in the client
     * configuration. If the session is already open, the function shall do
     * nothing.
     *
     * @throws IOException if a session could not to be established.
     */
    public void open() throws IOException
    {
        synchronized (AMQPS_SESSION_LOCK)
        {
            // Codes_SRS_AMQPSIOTHUBSESSION_11_020: [If the AMQPS session is already open, the function shall do nothing.]
            if (this.state == AmqpsIotHubSessionState.OPEN)
            {
                return;
            }

            String iotHubHostname = this.config.getIotHubHostname();
            String iotHubName = this.config.getIotHubName();
            String deviceId = this.config.getDeviceId();

            String iotHubUser = deviceId + "@sas." + iotHubName;

            IotHubSasToken sasToken = this.buildToken();

            try
            {
                // Codes_SRS_AMQPSIOTHUBSESSION_11_001: [The function shall establish an AMQPS session with an IoT Hub using the provided host name, user name, device ID, and sas token.]
                // Codes_SRS_AMQPSIOTHUBSESSION_11_003: [If an AMQPS session is unable to be established for any reason, the function shall throw an IOException.]

                this.sender = null;
                this.receiver = null;
                this.sender = new AmqpsSender(iotHubHostname, iotHubUser, deviceId, sasToken.toString());
                this.receiver = new AmqpsReceiver(iotHubHostname, iotHubUser, deviceId, sasToken.toString());
                this.sender.open();
                this.receiver.open();

                this.state = AmqpsIotHubSessionState.OPEN;
            }
            // hide implementation-specific exception.
            catch (Exception e)
            {
                throw new IOException(e);
            }
        }
    }

    /**
     * Closes the session. After the session is closed, it is no longer usable.
     * If the session is already closed, the function shall do nothing.
     *
     * @throws IOException In case an error occurs while closing the session.
     */
    public void close() throws IOException
    {
        synchronized (AMQPS_SESSION_LOCK)
        {
            // Codes_SRS_AMQPSIOTHUBSESSION_11_025: [If the AMQPS session is closed, the function shall do nothing.]
            if (this.state == AmqpsIotHubSessionState.CLOSED)
            {
                return;
            }

            try
            {
                // Codes_SRS_AMQPSIOTHUBSESSION_11_023: [The function shall close the AMQPS session.]
                this.sender.close();
                this.receiver.close();
                this.state = AmqpsIotHubSessionState.CLOSED;
            }
            // hide implementation-specific exception.
            catch (Exception e)
            {
                throw new IOException(e);
            }
        }
    }

    /**
     * Sends an event message.
     *
     * @param message the event message.
     *
     * @return the status code from sending the event message.
     *
     * @throws IOException if the AmqpsSender is not open
     */
    public IotHubStatusCode sendEvent(Message message) throws IOException
    {
        synchronized (AMQPS_SESSION_LOCK)
        {
            // Codes_SRS_AMQPSIOTHUBSESSION_11_027: [If the AMQPS session is closed, the function shall throw an IllegalStateException.]
            if (this.state == AmqpsIotHubSessionState.CLOSED)
            {
                throw new IllegalStateException(
                        "Cannot send event using "
                                + "an AMQPS session that is closed.");
            }
            
            // Codes_SRS_AMQPSIOTHUBSESSION_11_004: [The function shall send an event message to the IoT Hub given in the configuration.]
            // Codes_SRS_AMQPSIOTHUBSESSION_11_005: [The function shall send the message body.]
            // Codes_SRS_AMQPSIOTHUBSESSION_11_008: [If the message was successfully received by the service, the function shall return status code OK_EMPTY.]
            IotHubStatusCode result = IotHubStatusCode.OK_EMPTY;
            try
            {
                this.sender.send(message.getBytes(), message.messageId);

            }
            catch (HandlerException e)
            {
                System.out.println(e.getMessage());
                // Codes_SRS_AMQPSIOTHUBSESSION_11_009: [If the message was not successfully received by the service, the function shall return status code ERROR.]
                result = IotHubStatusCode.ERROR;
            }
            catch (Throwable e)
            {
                throw e;
            }

            return result;
        }
    }

    /**
     * Receives a message, if one exists.
     *
     * @return the message received, or null if none exists.
     *
     * @throws IOException if the IoT Hub could not be reached.
     */
    public Message receiveMessage() throws IOException
    {
        synchronized (AMQPS_SESSION_LOCK)
        {
            // Codes_SRS_AMQPSIOTHUBSESSION_11_029: [If the AMQPS session is closed, the function shall throw an IllegalStateException.]
            if (this.state == AmqpsIotHubSessionState.CLOSED)
            {
                throw new IllegalStateException(
                        "Cannot receive a message using "
                                + "an AMQPS session that is closed.");
            }

            Message msg = null;

            //Will be given a message if there is one to consume or null
            // Codes_SRS_AMQPSIOTHUBSESSION_11_011: [The function shall attempt to consume a message from the IoT Hub given in the configuration.]
            // Codes_SRS_AMQPSIOTHUBSESSION_11_014: [If the IoT Hub could not be reached, the function shall throw an IOException.]
            AmqpsMessage message = (AmqpsMessage) this.receiver.consumeMessage();
            if(message!=null) {
                msg = this.protonMessageToMessage(message);
                this.lastMessage = message;
            }

            return msg;
        }
    }

    /**
     * Sends the message result for the previously received
     * message.
     *
     * @param result the message result (one of {@link IotHubMessageResult#COMPLETE},
     *               {@link IotHubMessageResult#ABANDON}, or {@link IotHubMessageResult#REJECT}).
     *
     * @throws IllegalStateException if {@code sendMessageResult} is called before
     * {@link #receiveMessage()} is called.
     * @throws IOException if the IoT Hub could not be reached.
     */
    public void sendMessageResult(IotHubMessageResult result)
            throws IOException
    {
        synchronized (AMQPS_SESSION_LOCK)
        {
            // Codes_SRS_AMQPSIOTHUBSESSION_11_031: [If the AMQPS session is closed, the function shall throw an IllegalStateException.]
            if (this.state == AmqpsIotHubSessionState.CLOSED)
            {
                throw new IllegalStateException(
                        "Cannot send a message result using "
                                + "an AMQPS session that is closed.");
            }

            // Codes_SRS_AMQPSIOTHUBSESSION_11_018: [If sendMessageResult(result) is called before a message is received, the function shall throw an IllegalStateException.]
            if (this.lastMessage == null)
            {
                throw new IllegalStateException("Cannot send a message "
                        + "result before a message is received.");
            }

            // Codes_SRS_AMQPSIOTHUBSESSION_11_019: [If the acknowledgement fails, the function shall throw an IOException.]
            try
            {
                switch (result)
                {
                    // Codes_SRS_AMQPSIOTHUBSESSION_11_015: [If the message result is COMPLETE, the function shall acknowledge the last message with acknowledgement type CONSUMED.]
                    case COMPLETE:
                        this.lastMessage.acknowledge(AmqpsMessage.ACK_TYPE.COMPLETE);
                        break;
                    // Codes_SRS_AMQPSIOTHUBSESSION_11_016: [If the message result is ABANDON, the function shall acknowledge the last message with acknowledgement type RELEASED.]
                    case ABANDON:
                        this.lastMessage.acknowledge(AmqpsMessage.ACK_TYPE.ABANDON);
                        break;
                    // Codes_SRS_AMQPSIOTHUBSESSION_11_017: [If the message result is REJECT, the function shall acknowledge the last message with acknowledgement type POISONED.]
                    case REJECT:
                        this.lastMessage.acknowledge(AmqpsMessage.ACK_TYPE.REJECT);
                        break;
                    default:
                        // should never happen.
                        throw new IllegalStateException(
                                "Invalid IoT Hub message result.");
                }
            }
            // hide implementation-specific exception.
            catch (Exception e)
            {
                throw new IOException(e);
            }
        }
    }

    /**
     * Converts an AMQPS message to a corresponding IoT Hub message.
     *
     * @param protonMsg the AMQPS message.
     *
     * @return the corresponding IoT Hub message.
     */
    protected static Message protonMessageToMessage(MessageImpl protonMsg){
        Data d = (Data)protonMsg.getBody();
        Binary b = d.getValue();
        byte[] msgBody = new byte[b.getLength()];
        ByteBuffer buffer = b.asByteBuffer();
        buffer.get(msgBody);
        Message msg = new Message(msgBody);

        Properties properties = protonMsg.getProperties();

        //Call all of the getters for the Proton message Properties and set those properties
        //in the IoT Hub message properties if they exist.
        for(Method m : properties.getClass().getMethods()){
            if(m.getName().startsWith("get")){
                try {
                    String propertyName = Character.toLowerCase(m.getName().charAt(3))+ m.getName().substring(4);
                    Object value = m.invoke(properties);
                    if(value != null && !propertyName.equals("class")) {
                        String val = value.toString();

                        if (MessageProperty.isValidAppProperty(propertyName, val)) {
                            msg.setProperty(propertyName, val);
                        }
                    }
                } catch (IllegalAccessException e) {
                    System.err.println("Attempted to access private or protected member of class during message conversion.");
                } catch (InvocationTargetException e) {
                    System.err.println("Exception thrown while attempting to get member variable. See: " + e.getMessage());
                }
            }
        }
        return msg;
    }

    /**
     * Generates a valid SAS token from the client configuration.
     *
     * @return a SAS token that authenticates the device to the IoT Hub.
     */
    protected IotHubSasToken buildToken()
    {
        String iotHubHostname = this.config.getIotHubHostname();
        String deviceId = this.config.getDeviceId();
        String deviceKey = this.config.getDeviceKey();
        long tokenValidSecs = this.config.getTokenValidSecs();

        long msgExpiryTime = System.currentTimeMillis() / 1000l + tokenValidSecs + 1l;

        String resourceUri = IotHubUri.getResourceUri(iotHubHostname, deviceId);

        IotHubSasToken sasToken = new IotHubSasToken(resourceUri, deviceId, deviceKey, msgExpiryTime);

        if (this.config.targetHubType == AzureHubType.IoTHub) {
            sasToken.appendSknValue = false;
        } else if (this.config.targetHubType == AzureHubType.EventHub) {
            sasToken.appendSknValue = true;
        }

        return sasToken;
    }

    protected AmqpsIotHubSession()
    {
        this.config = null;
    }
}
