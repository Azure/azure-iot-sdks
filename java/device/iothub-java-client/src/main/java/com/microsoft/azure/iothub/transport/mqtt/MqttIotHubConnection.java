// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

package com.microsoft.azure.iothub.transport.mqtt;

import com.microsoft.azure.iothub.DeviceClientConfig;
import com.microsoft.azure.iothub.IotHubStatusCode;
import com.microsoft.azure.iothub.Message;
import com.microsoft.azure.iothub.auth.IotHubSasToken;
import com.microsoft.azure.iothub.transport.TransportUtils;
import org.eclipse.paho.client.mqttv3.*;
import org.eclipse.paho.client.mqttv3.persist.MemoryPersistence;

import java.io.IOException;
import java.util.Queue;
import java.util.concurrent.LinkedBlockingQueue;

public class MqttIotHubConnection implements MqttCallback
{
    /** The MQTT connection lock. */
    protected static final Object MQTT_CONNECTION_LOCK = new Object();

    private MqttAsyncClient asyncClient;

    public enum ConnectionState
    {
        OPEN, CLOSED
    }

    protected final DeviceClientConfig config;
    private Queue<Message> receivedMessagesQueue  = new LinkedBlockingQueue<>();
    protected ConnectionState state = ConnectionState.CLOSED;

    private String publishTopic;
    private String subscribeTopic;
    private MqttConnectOptions connectionOptions = new MqttConnectOptions();


    //mqtt connection options
    private static final int keepAliveInterval = 20;
    private static final int mqttVersion = 4;
    private static final boolean setCleanSession = true;
    private static final int qos = 1;

    //string constants
    private static String sslPrefix = "ssl://";
    private static String sslPortSuffix = ":8883";

    /**
     * Constructs an instance from the given {@link DeviceClientConfig}
     * object.
     *
     * @param config the client configuration.
     */
    public MqttIotHubConnection(DeviceClientConfig config) throws IllegalArgumentException
    {
        synchronized (MQTT_CONNECTION_LOCK)
        {
            // Codes_SRS_MQTTIOTHUBCONNECTION_15_003: [The constructor shall throw a new IllegalArgumentException
            // if any of the parameters of the configuration is null or empty.]
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

            // Codes_SRS_MQTTIOTHUBCONNECTION_15_001: [The constructor shall save the configuration.]
            this.config = config;

            // Codes_SRS_MQTTIOTHUBCONNECTION_15_002: [The constructor shall create the publish and subscribe
            // topic for the specified device id.]
            this.publishTopic = "devices/" + this.config.getDeviceId() + "/messages/events/";
            this.subscribeTopic = "devices/" + this.config.getDeviceId() + "/messages/devicebound/#";
        }
    }

    /**
     * Establishes a connection for the device and IoT Hub given in the client
     * configuration. If the connection is already open, the function shall do
     * nothing.
     *
     * @throws IOException if a connection could not to be established.
     */
    public void open() throws IOException
    {
        synchronized (MQTT_CONNECTION_LOCK)
        {
            //Codes_SRS_MQTTIOTHUBCONNECTION_15_006: [If the MQTT connection is already open,
            // the function shall do nothing.]
            if (this.state == ConnectionState.OPEN)
            {
                return;
            }

            // Codes_SRS_MQTTIOTHUBCONNECTION_15_004: [The function shall establish an MQTT connection
            // with an IoT Hub using the provided host name, user name, device ID, and sas token.]
            try
            {
                IotHubSasToken sasToken = new IotHubSasToken(this.config);

                this.asyncClient = new MqttAsyncClient(sslPrefix + this.config.getIotHubHostname() + sslPortSuffix,
                        this.config.getDeviceId(), new MemoryPersistence());
                asyncClient.setCallback(this);

                this.updateConnectionOptions(this.config.getIotHubHostname() + "/" + this.config.getDeviceId(), sasToken.toString());
                this.connect(connectionOptions);

                this.subscribe();

                this.state = ConnectionState.OPEN;
            }
            // Codes_SRS_MQTTIOTHUBCONNECTION_15_005: [If an MQTT connection is unable to be established
            // for any reason, the function shall throw an IOException.]
            catch (MqttException e)
            {
                throw new IOException("Error initializing MQTT connection:" + e.getMessage());
            }
        }
    }

    /**
     * Closes the connection. After the connection is closed, it is no longer usable.
     * If the connection is already closed, the function shall do nothing.
     *
     */
    public void close()
    {
        synchronized (MQTT_CONNECTION_LOCK)
        {
            // Codes_SRS_MQTTIOTHUBCONNECTION_15_007: [If the MQTT session is closed, the function shall do nothing.]
            if (this.state == ConnectionState.CLOSED) {
                return;
            }

            // Codes_SRS_MQTTIOTHUBCONNECTION_15_006: [**The function shall close the MQTT connection.]
            try
            {
                IMqttToken disconnectToken = this.asyncClient.disconnect();
                disconnectToken.waitForCompletion();
            }
            catch (MqttException e)
            {
                //do nothing, since connection is closed anyway.
            }

            this.state = ConnectionState.CLOSED;
            this.asyncClient = null;
        }
    }

    /**
     * Sends an event message.
     *
     * @param message the event message.
     *
     * @return the status code from sending the event message.
     *
     * @throws IllegalStateException if the MqttIotHubConnection is not open
     */
    public IotHubStatusCode sendEvent(Message message) throws IllegalStateException
    {
        synchronized (MQTT_CONNECTION_LOCK)
        {
            // Codes_SRS_MQTTIOTHUBCONNECTION_15_010: [If the message is null or empty,
            // the function shall return status code BAD_FORMAT.]
            if (message == null || message.getBytes() == null || message.getBytes().length == 0)
            {
                return IotHubStatusCode.BAD_FORMAT;
            }

            // Codes_SRS_MQTTIOTHUBCONNECTION_15_013: [If the MQTT connection is closed,
            // the function shall throw an IllegalStateException.]
            if (this.state == ConnectionState.CLOSED)
            {
                throw new IllegalStateException("Cannot send event using a closed MQTT connection");
            }

            // Codes_SRS_MQTTIOTHUBCONNECTION_15_008: [The function shall send an event message
            // to the IoT Hub given in the configuration.]
            // Codes_SRS_MQTTIOTHUBCONNECTION_15_009: [The function shall send the message payload.]
            // Codes_SRS_MQTTIOTHUBCONNECTION_15_011: [If the message was successfully received by the service,
            // the function shall return status code OK_EMPTY.]
            IotHubStatusCode result = IotHubStatusCode.OK_EMPTY;

            try
            {
                MqttMessage mqttMessage = new MqttMessage(message.getBytes());
                mqttMessage.setQos(qos);

                IMqttDeliveryToken publishToken = asyncClient.publish(publishTopic, mqttMessage);
                publishToken.waitForCompletion();
            }
            // Codes_SRS_MQTTIOTHUBCONNECTION_15_012: [If the message was not successfully
            // received by the service, the function shall return status code ERROR.]
            catch(MqttException e)
            {
                result = IotHubStatusCode.ERROR;
            }

            return result;
        }
    }

    /**
     * Receives a message, if one exists.
     *
     * @return the message received, or null if none exists.
     *
     * @throws IllegalStateException if the connection state is currently closed.
     */
    public Message receiveMessage() throws IllegalStateException
    {
        // Codes_SRS_MQTTIOTHUBCONNECTION_15_015: [If the MQTT connection is closed,
        // the function shall throw an IllegalStateException.]
        if (this.state == ConnectionState.CLOSED)
        {
            throw new IllegalStateException("The MQTT connection is currently closed. Call open() before attempting" +
                    "to receive a message.");
        }

        Message message = null;

        // Codes_SRS_MQTTIOTHUBCONNECTION_15_014: [The function shall attempt to consume a message
        // from the received messages queue.]

        if (this.receivedMessagesQueue.size() > 0)
        {
            message = this.receivedMessagesQueue.remove();
        }

        return message;
    }

    /**
     * Event fired when the connection with the MQTT broker is lost.
     * @param throwable Reason for losing the connection.
     */
    @Override
    public void connectionLost(Throwable throwable)
    {
        synchronized (MQTT_CONNECTION_LOCK)
        {
            // Codes_SRS_MQTTIOTHUBCONNECTION_15_016: [The function shall attempt to reconnect to the IoTHub
            // in a loop with an exponential backoff until it succeeds]
            int currentReconnectionAttempt = 0;
            boolean connected = false;
            while (!connected)
            {
                try
                {
                    // Codes_SRS_MQTTIOTHUBCONNECTION_15_17: [The function shall generate a new sas token to be
                    // used for connecting to the mqtt broker.]
                    IotHubSasToken sasToken = new IotHubSasToken(this.config);

                    this.updateConnectionOptions(this.config.getDeviceId(), sasToken.toString());
                    this.connect(this.connectionOptions);
                    connected = true;
                }
                catch (MqttException connectionException)
                {
                    try
                    {
                        currentReconnectionAttempt++;

                        // Codes_SRS_MQTTIOTHUBCONNECTION_15_018: [The maximum wait interval
                        // until a reconnect is attempted shall be 60 seconds.]
                        Thread.sleep(generateSleepInterval(currentReconnectionAttempt));
                    }
                    catch (InterruptedException exception)
                    {
                        // do nothing, reconnection attempts will continue
                    }
                }
            }
        }
    }

    @Override
    public void messageArrived(String topic, MqttMessage mqttMessage)
    {
        // Codes_SRS_MQTTIOTHUBCONNECTION_15_019: [A new message is created using the payload
        // from the received MqttMessage]
        Message message = new Message(mqttMessage.getPayload());

        // Codes_SRS_MQTTIOTHUBCONNECTION_15_020: [The newly created message is added to the received messages queue.]

        this.receivedMessagesQueue.add(message);
    }

    @Override
    public void deliveryComplete(IMqttDeliveryToken iMqttDeliveryToken)
    {
        //there is no need for anything here, since we already call waitForCompletion after publishing a message.
    }

    private void connect(MqttConnectOptions connectionOptions) throws MqttException
    {
        if (!this.asyncClient.isConnected())
        {
            IMqttToken connectToken = this.asyncClient.connect(connectionOptions);
            connectToken.waitForCompletion();
        }
    }

    private void subscribe() throws MqttException
    {
        IMqttToken subToken = this.asyncClient.subscribe(this.subscribeTopic, qos);
        subToken.waitForCompletion();
    }

    /**
     * Generates the connection options for the mqtt broker connection.
     *
     * @param userName the user name for the mqtt broker connection.
     * @param userPassword the user password for the mqtt broker connection.
     */
    private void updateConnectionOptions(String userName, String userPassword)
    {
        this.connectionOptions.setKeepAliveInterval(keepAliveInterval);
        this.connectionOptions.setCleanSession(setCleanSession);
        this.connectionOptions.setMqttVersion(mqttVersion);
        this.connectionOptions.setUserName(userName);
        this.connectionOptions.setPassword(userPassword.toCharArray());
    }

    private static byte[] sleepIntervals = {1, 2, 4, 8, 16, 32, 60};
    /** Generates a reconnection time with an exponential backoff
     * and a maximum value of 60 seconds.
     *
     * @param currentAttempt the number of attempts
     * @return the sleep interval until the next attempt.
     */
    private byte generateSleepInterval(int currentAttempt)
    {
        if (currentAttempt > 7)
        {
            return sleepIntervals[6];
        }
        else if (currentAttempt > 0)
        {
            return sleepIntervals[currentAttempt - 1];
        }
        else
        {
            return 0;
        }
    }
}
