// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

package com.microsoft.azure.iothub.transport.amqps;

import com.microsoft.azure.iothub.*;
import com.microsoft.azure.iothub.Message;
import com.microsoft.azure.iothub.auth.IotHubSasToken;
import com.microsoft.azure.iothub.net.IotHubMessageUri;
import com.microsoft.azure.iothub.net.IotHubEventUri;
import com.microsoft.azure.iothub.net.IotHubUri;

import org.apache.qpid.jms.JmsConnectionFactory;
import org.apache.qpid.jms.JmsQueue;
import org.apache.qpid.jms.message.JmsAmqpMessage;
import org.apache.qpid.jms.provider.ProviderConstants;

import java.io.IOException;
import java.util.Enumeration;

import javax.jms.BytesMessage;
import javax.jms.Connection;
import javax.jms.ConnectionFactory;
import javax.jms.Destination;
import javax.jms.JMSException;
import javax.jms.MessageConsumer;
import javax.jms.MessageProducer;
import javax.jms.Session;

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
    /** The underlying AMQPS connection. */
    protected Connection connection;
    /** The AMQPS session. */
    protected Session session;

    /** The last AMQPS message received by the device. */
    protected JmsAmqpMessage lastMessage;


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

            String iotHubUrl = "amqps://" + iotHubHostname;

            String iotHubUser = deviceId + "@sas." + iotHubName;

            IotHubSasToken sasToken = this.buildToken();

            try
            {
                // Codes_SRS_AMQPSIOTHUBSESSION_11_001: [The function shall establish an AMQPS session with an IoT Hub with the resource URI 'amqps://[urlEncodedDeviceId]@sas.[urlEncodedIotHubName]:[urlEncodedSasToken]@[urlEncodedIotHubHostname].]
                // Codes_SRS_AMQPSIOTHUBSESSION_11_003: [If an AMQPS session is unable to be established, the function shall throw an IOException.]
                // JmsConnectionFactory URL-escapes the username and password.
                ConnectionFactory connFactory = new JmsConnectionFactory(
                        iotHubUser, sasToken.toString(), iotHubUrl);
                this.connection = connFactory.createConnection();
                this.connection.start();
                // the session will not use transactions.
                this.session = this.connection.createSession(
                        false, Session.CLIENT_ACKNOWLEDGE);

                this.state = AmqpsIotHubSessionState.OPEN;
            }
            // hide implementation-specific exception.
            catch (JMSException e)
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
                this.connection.close();
                this.state = AmqpsIotHubSessionState.CLOSED;
            }
            // hide implementation-specific exception.
            catch (JMSException e)
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
     * @throws IOException if a message producer could not be created.
     */
    public IotHubStatusCode sendEvent(Message message)
            throws IOException
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
            IotHubEventUri eventUri =
                    new IotHubEventUri(this.config.getIotHubHostname(),
                            this.config.getDeviceId());
            String eventPath = eventUri.getPath();
            Destination eventDest = new JmsQueue(eventPath);

            MessageProducer msgProducer;
            BytesMessage eventMsg;
            try
            {
                // Codes_SRS_AMQPSIOTHUBSESSION_11_010: [If a message producer could not be successfully registered, the function shall throw an IOException.]
                msgProducer = this.session.createProducer(eventDest);
                msgProducer.setTimeToLive(config.getReadTimeoutMillis());

                eventMsg = this.session.createBytesMessage();
                // Codes_SRS_AMQPSIOTHUBSESSION_11_005: [The function shall send the message body.]
                eventMsg.writeBytes(message.getBytes());
                // Codes_SRS_AMQPSIOTHUBSESSION_11_007: [The function shall send all message properties.]
                for (MessageProperty property : message.getProperties())
                {
                    eventMsg.setStringProperty(property.getName(),
                            property.getValue());
                }
                // Codes_SRS_AMQPSIOTHUBSESSION_11_006: [The function shall set the property 'to' to '/devices/[deviceId]/messages/events'.]
                eventMsg.setStringProperty("to", eventPath);
            }
            // hide implementation-specific exception.
            catch (JMSException e)
            {
                throw new IOException(e);
            }

            // Codes_SRS_AMQPSIOTHUBSESSION_11_008: [If the message was successfully received by the service, the function shall return status code OK_EMPTY.]
            IotHubStatusCode result = IotHubStatusCode.OK_EMPTY;
            try
            {

                msgProducer.send(eventMsg);
            }
            catch (JMSException e)
            {
                System.out.println(e.getMessage());
                // Codes_SRS_AMQPSIOTHUBSESSION_11_009: [If the message was not successfully received by the service, the function shall return status code ERROR.]
                result = IotHubStatusCode.ERROR;
            }
            finally
            {
                try
                {
                    msgProducer.close();
                } catch (JMSException e)
                {
                    throw new IOException(e);
                }
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

            // Codes_SRS_AMQPSIOTHUBSESSION_11_011: [The function shall attempt to consume a message from the IoT Hub given in the configuration.]
            IotHubMessageUri messageUri =
                    new IotHubMessageUri(this.config.getIotHubHostname(),
                            this.config.getDeviceId());
            String messagePath = messageUri.getPath();
            Destination messageDest = new JmsQueue(messagePath);

            // Codes_SRS_AMQPSIOTHUBSESSION_11_014: [If the IoT Hub could not be reached, the function shall throw an IOException.]
            MessageConsumer msgConsumer = null;
            Message msg = null;
            try
            {
                msgConsumer = this.session.createConsumer(messageDest);
                JmsAmqpMessage message =
                        (JmsAmqpMessage) msgConsumer.receive(
                                AMQPS_RECEIVE_TIMEOUT_MILLIS);
                if (message != null)
                {
                    this.lastMessage = message;
                    msg = amqpsMessageToMessage(
                            (BytesMessage) message);
                }
            }
            // hide implementation-specific exception.
            catch (JMSException e)
            {
                throw new IOException(e);
            }
            finally
            {
                if (msgConsumer != null)
                {
                    try
                    {
                        msgConsumer.close();
                    } catch (JMSException e)
                    {
                        e.printStackTrace();
                    }
                }
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
                        this.lastMessage.acknowledge(
                                ProviderConstants.ACK_TYPE.CONSUMED);
                        break;
                    // Codes_SRS_AMQPSIOTHUBSESSION_11_016: [If the message result is ABANDON, the function shall acknowledge the last message with acknowledgement type RELEASED.]
                    case ABANDON:
                        this.lastMessage.acknowledge(
                                ProviderConstants.ACK_TYPE.RELEASED);
                        break;
                    // Codes_SRS_AMQPSIOTHUBSESSION_11_017: [If the message result is REJECT, the function shall acknowledge the last message with acknowledgement type POISONED.]
                    case REJECT:
                        this.lastMessage.acknowledge(
                                ProviderConstants.ACK_TYPE.POISONED);
                        break;
                    default:
                        // should never happen.
                        throw new IllegalStateException(
                                "Invalid IoT Hub message result.");
                }
            }
            // hide implementation-specific exception.
            catch (JMSException e)
            {
                throw new IOException(e);
            }
        }
    }

    /**
     * Converts an AMQPS message to a corresponding IoT Hub message.
     *
     * @param amqpsMsg the AMQPS message.
     *
     * @return the corresponding IoT Hub message.
     *
     * @throws JMSException if the AMQPS message could not be parsed.
     */
    protected static Message amqpsMessageToMessage(BytesMessage amqpsMsg)
            throws JMSException
    {
        // Codes_SRS_AMQPSIOTHUBSESSION_11_012: [If a message is found, the function shall include the message body in the returned message.]
        byte[] msgBody = new byte[(int) amqpsMsg.getBodyLength()];
        amqpsMsg.readBytes(msgBody);
        Message msg = new Message(msgBody);
        // Codes_SRS_AMQPSIOTHUBSESSION_11_013: [If a message is found, the function shall include all valid application properties in the returned message.]
        Enumeration amqpsMsgProperties = amqpsMsg.getPropertyNames();
        while (amqpsMsgProperties.hasMoreElements())
        {
            String propertyName = (String) amqpsMsgProperties.nextElement();
            String propertyValue = amqpsMsg.getStringProperty(propertyName);
            if (MessageProperty.isValidAppProperty(
                    propertyName, propertyValue))
            {
                msg.setProperty(propertyName, propertyValue);
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
        this.connection = null;
        this.session = null;
    }
}
