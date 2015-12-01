// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

package com.microsoft.azure.iothub.transport.mqtt;

import com.microsoft.azure.iothub.DeviceClientConfig;
import com.microsoft.azure.iothub.IotHubMessageResult;
import com.microsoft.azure.iothub.IotHubStatusCode;
import com.microsoft.azure.iothub.Message;
import com.microsoft.azure.iothub.auth.IotHubSasToken;
import com.microsoft.azure.iothub.transport.TransportUtils;
import sun.reflect.generics.reflectiveObjects.NotImplementedException;

import java.io.IOException;

public class MqttIotHubConnection {

    /** The MQTT connection lock. */
    protected static final Object MQTT_CONNECTION_LOCK = new Object();

    protected enum ConnectionState
    {
        OPEN, CLOSED
    }

    protected ConnectionState state;

    protected final DeviceClientConfig config;

    MqttIotHubConnectionHandler handler;
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
            // Codes_SRS_MQTTIOTHUBCONNECTION_15_018: [The constructor shall throw a new IllegalArgumentException
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
            this.state = ConnectionState.CLOSED;
        }
    }

    /**
     * Establishes a connection for the device and IoT Hub/Protocol Gateway given in the client
     * configuration. If the connection is already open, the function shall do
     * nothing.
     *
     * @throws IllegalArgumentException if connection parameters are null or empty.
     * @throws IOException if a connection could not to be established.
     */
    public void open() throws IOException, IllegalArgumentException
    {
        synchronized (MQTT_CONNECTION_LOCK)
        {
            //Codes_SRS_MQTTIOTHUBCONNECTION_15_004: [If the MQTT connection is already open,
            // the function shall do nothing.]
            if (this.state == ConnectionState.OPEN)
            {
                return;
            }

            String gatewayHostName = this.config.getGatewayHostName();
            String deviceId = this.config.getDeviceId();
            IotHubSasToken sasToken = TransportUtils.buildToken(this.config);

            // Codes_SRS_MQTTIOTHUBCONNECTION_15_002: [The function shall establish an MQTT connection
            // with an IoT Hub using the provided host name, user name, device ID, and sas token.]
            // Codes_SRS_MQTTIOTHUBCONNECTION_15_003: [If an MQTT connection is unable to be established
            // for any reason, the function shall throw an IOException.]

            this.handler = new MqttIotHubConnectionHandler(gatewayHostName, deviceId, sasToken.toString());
            this.handler.open();
            this.state = ConnectionState.OPEN;
        }
    }

    /**
     * Closes the connection. After the connection is closed, it is no longer usable.
     * If the connection is already closed, the function shall do nothing.
     *
     */
    public void close()
    {
        // Codes_SRS_MQTTIOTHUBCONNECTION_15_006: [If the MQTT session is closed, the function shall do nothing.]
        if (this.state == ConnectionState.CLOSED) {
            return;
        }

        // Codes_SRS_MQTTIOTHUBCONNECTION_15_005: [The function shall close the MQTT session.]
        this.handler.close();
        this.state = ConnectionState.CLOSED;
    }

    /**
     * Sends an event message.
     *
     * @param message the event message.
     *
     * @return the status code from sending the event message.
     *
     * @throws IllegalStateException if the MqttIotHubConnectionHandler is not open
     */
    public IotHubStatusCode sendEvent(Message message) throws IllegalStateException
    {
        synchronized (MQTT_CONNECTION_LOCK)
        {
            // Codes_SRS_MQTTIOTHUBCONNECTION_15_019: [If the message is null,
            // the function shall return status code BAD_FORMAT.]
            if (message == null)
            {
                return IotHubStatusCode.BAD_FORMAT;
            }

            // Codes_SRS_MQTTIOTHUBCONNECTION_15_012: [If the MQTT session is closed,
            // the function shall throw an IllegalStateException.]
            if (this.state == ConnectionState.CLOSED)
            {
                throw new IllegalStateException("Cannot send event using a closed MQTT session");
            }

            // Codes_SRS_MQTTIOTHUBCONNECTION_15_007: [The function shall send an event message
            // to the IoT Hub given in the configuration.]
            // Codes_SRS_MQTTIOTHUBCONNECTION_15_008: [The function shall send the message body.]
            // Codes_SRS_MQTTIOTHUBCONNECTION_15_010: [If the message was successfully received by the service,
            // the function shall return status code OK_EMPTY.]
            IotHubStatusCode result = IotHubStatusCode.OK_EMPTY;
            try
            {
                this.handler.send(message.getBytes(), message.messageId);
            }
            catch(Exception e)
            {
                // Codes_SRS_MQTTIOTHUBCONNECTION_15_011: [If the message was not successfully received by the service,
                // the function shall return status code ERROR.]
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
     * @throws IOException if the IoT Hub could not be reached.
     */
    public Message receiveMessage() throws IOException
    {
        // Codes_SRS_MQTTIOTHUBCONNECTION_15_013: [The function shall attempt to consume a message
        // from the IoT Hub given in the configuration.]
        // Codes_SRS_MQTTIOTHUBCONNECTION_15_014: [If a message is found, the function shall include
        // the message body in the returned message.]
        // Codes_SRS_MQTTIOTHUBCONNECTION_15_015: [If a message is found, the function shall include
        // all valid application properties in the returned message.]
        // Codes_SRS_MQTTIOTHUBCONNECTION_15_016: [If the IoT Hub could not be reached,
        // the function shall throw an IOException.]
        // Codes_SRS_MQTTIOTHUBCONNECTION_15_017: [If the MQTT session is closed,
        // the function shall throw an IllegalStateException.]

        //todo iliel implement real functionality

        throw new NotImplementedException();
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
    public void sendMessageResult(IotHubMessageResult result) throws IOException
    {
        // Codes_SRS_MQTTIOTHUBCONNECTION_15_018: [If the message result is COMPLETE,
        // the function shall acknowledge the last message message with acknowledgement type CONSUMED.]
        // Codes_SRS_MQTTIOTHUBCONNECTION_15_019: [If the message result is ABANDON,
        // the function shall acknowledge the last message message with acknowledgement type RELEASED.]
        // Codes_SRS_MQTTIOTHUBCONNECTION_15_020: [If the message result is REJECT,
        // the function shall acknowledge the last message message with acknowledgement type POISONED.]
        // Codes_SRS_MQTTIOTHUBCONNECTION_15_021: [If sendMessageResult(result) is called
        // before a message is received, the function shall throw an IllegalStateException.]
        // Codes_SRS_MQTTIOTHUBCONNECTION_15_022: [If the acknowledgement fails,
        // the function shall throw an IOException.]
        // Codes_SRS_MQTTIOTHUBCONNECTION_15_023: [If the MQTT session is closed,
        // the function shall throw an IllegalStateException.]

        //todo iliel implement real functionality

        throw new NotImplementedException();
    }
}
