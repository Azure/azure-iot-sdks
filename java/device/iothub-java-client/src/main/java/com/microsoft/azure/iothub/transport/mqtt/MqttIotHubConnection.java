// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

package com.microsoft.azure.iothub.transport.mqtt;

import com.microsoft.azure.iothub.DeviceClientConfig;
import com.microsoft.azure.iothub.IotHubStatusCode;
import com.microsoft.azure.iothub.Message;
import com.microsoft.azure.iothub.auth.IotHubSasToken;
import com.microsoft.azure.iothub.transport.State;
import com.microsoft.azure.iothub.transport.TransportUtils;
import org.eclipse.paho.client.mqttv3.*;
import org.eclipse.paho.client.mqttv3.persist.MemoryPersistence;

import java.io.IOException;
import java.net.URLEncoder;
import java.util.Queue;
import java.util.concurrent.LinkedBlockingQueue;

public class MqttIotHubConnection implements MqttCallback
{
    /** The MQTT connection lock. */
    protected final Object MQTT_CONNECTION_LOCK = new Object();

    private MqttAsyncClient asyncClient;

    protected final DeviceClientConfig config;
    protected State state = State.CLOSED;

    // paho mqtt only supports 10 messages in flight at the same time
    private static final int maxInFlightCount = 10;

    private Queue<Message> receivedMessagesQueue  = new LinkedBlockingQueue<>();

    private String publishTopic;
    private String subscribeTopic;
    private MqttConnectOptions connectionOptions = new MqttConnectOptions();
    private String iotHubUserName;

    //mqtt connection options
    private static final int keepAliveInterval = 20;
    private static final int mqttVersion = 4;
    private static final boolean setCleanSession = false;
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
            if (config == null)
            {
                throw new IllegalArgumentException("The DeviceClientConfig cannot be null.");
            }
            if (config.getIotHubHostname() == null || config.getIotHubHostname().length() == 0)
            {
                throw new IllegalArgumentException("hostName cannot be null or empty.");
            }
            if (config.getDeviceId() == null || config.getDeviceId().length() == 0)
            {
                throw new IllegalArgumentException("deviceID cannot be null or empty.");
            }
            if (config.getIotHubName() == null || config.getIotHubName().length() == 0)
            {
                throw new IllegalArgumentException("hubName cannot be null or empty.");
            }
            if (config.getDeviceKey() == null || config.getDeviceKey().length() == 0)
            {
                if(config.getSharedAccessToken() == null || config.getSharedAccessToken().length() == 0)

                    throw new IllegalArgumentException("Both deviceKey and shared access signature cannot be null or empty.");
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
            if (this.state == State.OPEN)
            {
                return;
            }

            // Codes_SRS_MQTTIOTHUBCONNECTION_15_004: [The function shall establish an MQTT connection
            // with an IoT Hub using the provided host name, user name, device ID, and sas token.]
            try
            {
                IotHubSasToken sasToken = new IotHubSasToken(this.config, System.currentTimeMillis() / 1000l +
                        this.config.getTokenValidSecs() + 1l);


                this.asyncClient = new MqttAsyncClient(sslPrefix + this.config.getIotHubHostname() + sslPortSuffix,
                        this.config.getDeviceId(), new MemoryPersistence());
                asyncClient.setCallback(this);

                String clientIdentifier = "DeviceClientType=" + URLEncoder.encode(TransportUtils.javaDeviceClientIdentifier + TransportUtils.clientVersion, "UTF-8");
                this.iotHubUserName = this.config.getIotHubHostname() + "/" + this.config.getDeviceId() + "/" + clientIdentifier;

                this.updateConnectionOptions(this.iotHubUserName, sasToken.toString());
                this.connect(connectionOptions);

                this.subscribe();

                this.state = State.OPEN;
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
            if (this.state == State.CLOSED)
            {
                return;
            }

            // Codes_SRS_MQTTIOTHUBCONNECTION_15_006: [**The function shall close the MQTT connection.]
            try
            {
                IMqttToken disconnectToken = this.asyncClient.disconnect();
                disconnectToken.waitForCompletion();
            } catch (MqttException e)
            {
                //do nothing, since connection is closed anyway.
            }

            this.state = State.CLOSED;
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
            if (this.state == State.CLOSED)
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
                while (asyncClient.getPendingDeliveryTokens().length >= maxInFlightCount)
                {
                    Thread.sleep(10);
                }

                MqttMessage mqttMessage = new MqttMessage(message.getBytes());
                mqttMessage.setQos(qos);

                IMqttDeliveryToken publishToken = asyncClient.publish(publishTopic, mqttMessage);

            }
            // Codes_SRS_MQTTIOTHUBCONNECTION_15_012: [If the message was not successfully
            // received by the service, the function shall return status code ERROR.]
            catch (Exception e)
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
        if (this.state == State.CLOSED)
        {
            throw new IllegalStateException("The MQTT connection is currently closed. Call open() before attempting " +
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
            // The connection was closed by calling the close() method, meaning we don't want to re-establish it.
            if (this.asyncClient == null)
            {
                return;
            }

            // Codes_SRS_MQTTIOTHUBCONNECTION_15_016: [The function shall attempt to reconnect to the IoTHub
            // in a loop with an exponential backoff until it succeeds]
            this.state = State.CLOSED;
            int currentReconnectionAttempt = 0;
            while (this.state == State.CLOSED)
            {
                try
                {
                    this.open();
                } catch (IOException e)
                {
                    try
                    {
                        currentReconnectionAttempt++;

                        // Codes_SRS_MQTTIOTHUBCONNECTION_15_018: [The maximum wait interval
                        // until a reconnect is attempted shall be 60 seconds.]
                        Thread.sleep(TransportUtils.generateSleepInterval(currentReconnectionAttempt));
                    } catch (InterruptedException exception)
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
}
