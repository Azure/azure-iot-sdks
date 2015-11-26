// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

package com.microsoft.azure.iothub.transport.mqtt;

import org.eclipse.paho.client.mqttv3.*;
import org.eclipse.paho.client.mqttv3.persist.MemoryPersistence;
import sun.reflect.generics.reflectiveObjects.NotImplementedException;

import java.io.IOException;

public class MqttIotHubConnectionHandler implements MqttCallback
{
    private MqttAsyncClient client;
    private String hostName;
    private String deviceId;
    private String publishTopic;
    private Object sendLock = new Object();
    private Object operationLock = new Object();
    private boolean deliveryComplete;
    private final MqttConnectOptions connectionOptions;
    private MemoryPersistence memoryPersistence;

    //mqtt connection options
    private static final int keepAliveInterval = 20;
    private static final int mqttVersion = 4;
    private static final boolean setCleanSession = true;
    private static final int qos = 1;


    public MqttIotHubConnectionHandler(String hostName, String deviceId, String sasToken)
            throws IllegalArgumentException
    {
        // Codes_SRS_MQTTIOTHUBCONNECTIONHANDLER_15_001: [The constructor shall throw a new IllegalArgumentException
        // if any of the input parameters is null or empty.]
        if(hostName == null || hostName.length() == 0)
        {
            throw new IllegalArgumentException("hostName cannot be null or empty.");
        }
        if (deviceId == null || deviceId.length() == 0)
        {
            throw new IllegalArgumentException("deviceId cannot be null or empty.");
        }
        if(sasToken == null || sasToken.length() == 0)
        {
            throw new IllegalArgumentException("sasToken cannot be null or empty.");
        }

        // Codes_SRS_MQTTIOTHUBCONNECTIONHANDLER_15_002: [The Constructor shall copy
        // all input parameters to private member variables.]
        this.hostName = hostName;
        this.deviceId = deviceId;

        // Codes_SRS_MQTTIOTHUBCONNECTIONHANDLER_15_015: [The constructor shall save the connection options
        // into a private variable which shall be used for publishing/receiving events/messages to/from IotHub]
        this.connectionOptions = new MqttConnectOptions();
        this.connectionOptions.setKeepAliveInterval(keepAliveInterval);
        this.connectionOptions.setCleanSession(setCleanSession);
        this.connectionOptions.setMqttVersion(mqttVersion);
        this.connectionOptions.setUserName(deviceId);
        this.connectionOptions.setPassword(sasToken.toCharArray());
        this.memoryPersistence = new MemoryPersistence();

        // Codes_SRS_MQTTIOTHUBCONNECTIONHANDLER_15_003: [The Constructor shall create the publish/subscribe
        // topics for the specified input parameters to private member variables.]
        this.publishTopic = "devices/" + this.deviceId + "/messages/events";
    }

    public void open() throws IOException
    {
        try
        {
            // Codes_SRS_MQTTIOTHUBCONNECTIONHANDLER_15_005: [The function shall initialize its MqttAsyncClient
            // using the saved host name and device id.]
            this.client = new MqttAsyncClient(hostName, deviceId, memoryPersistence);
            client.setCallback(this);
        }
        catch (MqttException e)
        {
            throw new IOException("Error initializing MQTT handler:" + e.getMessage());
        }
    }

    public void close()
    {
        synchronized (operationLock)
        {
            this.client = null;
        }
    }

    public void send(byte[] content) throws IOException
    {
        this.send(content, null);
    }

    public void send(byte[] content, Object messageId) throws IOException
    {
        synchronized (operationLock)
        {
            if (content == null || content.length == 0)
            {
                return;
            }

            deliveryComplete = false;

            // Codes_SRS_MQTTIOTHUBCONNECTIONHANDLER_15_007: [If the MqttAsyncClient has not been initialized,
            // the function shall throw a new IOException.]
            if (client == null)
            {
                throw new IOException("Connection handler is closed");
            }

            try
            {
                // Codes_SRS_MQTTIOTHUBCONNECTIONHANDLER_15_010: [The function shall connect to the IoT Hub broker
                // using the connection options saved previously.]
                IMqttToken connectToken = client.connect(connectionOptions);
                connectToken.waitForCompletion();

                MqttMessage message = new MqttMessage(content);
                message.setQos(qos);

                // Codes_SRS_MQTTIOTHUBCONNECTIONHANDLER_15_011: [The function shall publish the content
                // of the binary message to the specific device id topic]
                IMqttDeliveryToken publishToken = client.publish(publishTopic, message);
                publishToken.waitForCompletion();

                IMqttToken disconnectToken = client.disconnect();
                disconnectToken.waitForCompletion();
            }
            catch(MqttException e)
            {
                throw new IOException("Exception " + e.getMessage() + " while sending message with id " + messageId);
            }
        }
    }

    public MqttMessage consumeMessage() throws IOException
    {
        throw new NotImplementedException();
    }

    @Override
    public void connectionLost(Throwable throwable)
    {
        //todo iliel figure out what to do here
    }

    @Override
    public void messageArrived(String s, MqttMessage mqttMessage) throws Exception
    {

    }

    @Override
    public void deliveryComplete(IMqttDeliveryToken iMqttDeliveryToken)
    {

    }
}