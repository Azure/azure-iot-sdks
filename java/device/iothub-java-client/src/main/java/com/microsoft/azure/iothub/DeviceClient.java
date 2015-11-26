// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

package com.microsoft.azure.iothub;

import com.microsoft.azure.iothub.transport.amqps.AmqpsTransport;
import com.microsoft.azure.iothub.transport.https.HttpsTransport;
import com.microsoft.azure.iothub.transport.IotHubReceiveTask;
import com.microsoft.azure.iothub.transport.IotHubSendTask;
import com.microsoft.azure.iothub.transport.IotHubTransport;
import com.microsoft.azure.iothub.transport.mqtt.MqttTransport;

import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.net.URISyntaxException;
import java.net.URLDecoder;
import java.nio.charset.Charset;
import java.nio.charset.StandardCharsets;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;


/**
 * <p>
 * The public-facing API. Allows a single logical or physical device to connect
 * to an IoT Hub. The IoT Hub client supports sending events to and receiving
 * messages from an IoT Hub.
 * </p>
 * <p>
 * To support these workflows, the client library will provide the following
 * abstractions: a message, with its payload and associated properties; and a
 * client, which sends and receives messages.
 * </p>
 * <p>
 * The client buffers messages while the network is down, and re-sends them when
 * the network comes back online. It also batches messages to improve
 * communication efficiency (HTTPS only).
 * </p>
 * The client supports HTTPS 1.1 and AMQPS 1.0 transports.
 */
public final class DeviceClient
{
    /** The state of the IoT Hub client's connection with the IoT Hub. */
    protected enum IotHubClientState
    {
        OPEN, CLOSED
    }

    /**
     * The default number of milliseconds the transport will wait between
     * sending out messages.
     */
    public static final long DEFAULT_SEND_PERIOD_MILLIS = 5000l;
    /**
     * The default number of milliseconds the transport will wait between
     * polling for messages.
     */
    public static final long DEFAULT_RECEIVE_PERIOD_MILLIS_AMQPS = 5000l;
    public static final long DEFAULT_RECEIVE_PERIOD_MILLIS_MQTT = 5000l;
    public static final long DEFAULT_RECEIVE_PERIOD_MILLIS_HTTPS = 25*60*1000; /*25 minutes*/

    /** The hostname attribute name in a connection string. */
    public static final String HOSTNAME_ATTRIBUTE = "HostName=";
    /** The device ID attribute name in a connection string. */
    public static final String DEVICE_ID_ATTRIBUTE = "DeviceId=";
    /** The shared access key attribute name in a connection string. */
    public static final String SHARED_ACCESS_KEY_ATTRIBUTE = "SharedAccessKey=";
    /** The gateway hostname attribute name in a connection string. */
    public static final String GATEWAY_HOSTNAME_ATTRIBUTE = "GatewayHostName=";

    /**
     * The charset used for URL-encoding the device ID in the connection
     * string.
     */
    public static final Charset CONNECTION_STRING_CHARSET = StandardCharsets.UTF_8;

    protected DeviceClientConfig config;
    protected IotHubTransport transport;

    protected ScheduledExecutorService taskScheduler;
    protected IotHubClientState state;
    protected long current_receive_period_millis;

    /**
     * Constructor that takes a connection string as an argument.
     *
     * @param connString the connection string. The connection string is a set
     * of key-value pairs that are separated by ';', with the keys and values
     * separated by '='. It should contain values for the following keys:
     * {@code HostName}, {@code DeviceId}, and {@code SharedAccessKey}.
     * @param protocol the communication protocol used (i.e. HTTPS).
     *
     * @throws IllegalArgumentException if any of {@code connString} or
     * {@code protocol} are {@code null}; or if {@code connString} is missing
     * one of the following attributes:{@code HostName}, {@code DeviceId}, or
     * {@code SharedAccessKey}.
     * @throws URISyntaxException if the IoT hub hostname does not conform to
     * RFC 3986.
     */
    public DeviceClient(String connString, IotHubClientProtocol protocol)
            throws URISyntaxException
    {
        // Codes_SRS_DEVICECLIENT_11_047: [If the connection string is null, the function shall throw an IllegalArgumentException.]
        if (connString == null)
        {
            throw new IllegalArgumentException(
                    "IoT Hub connection string cannot be null.");
        }

        // Codes_SRS_DEVICECLIENT_11_042: [The constructor shall interpret the connection string as a set of key-value pairs delimited by ';', with keys and values separated by '='.]
        String[] connStringAttrs = connString.split(";");
        String hostname = null;
        String deviceId = null;
        String sharedAccessKey = null;
        String gatewayHostName = null;
        for (String attr : connStringAttrs)
        {
            // Codes_SRS_DEVICECLIENT_11_043: [The constructor shall save the IoT Hub hostname as the value of 'HostName' in the connection string.]
            if (attr.startsWith(HOSTNAME_ATTRIBUTE))
            {
                hostname = attr.substring(HOSTNAME_ATTRIBUTE.length());
            }
            // Codes_SRS_DEVICECLIENT_11_044: [The constructor shall save the device ID as the UTF-8 URL-decoded value of 'DeviceId' in the connection string.]
            else if (attr.startsWith(DEVICE_ID_ATTRIBUTE))
            {
                String urlEncodedDeviceId = attr.substring(DEVICE_ID_ATTRIBUTE.length());
                try
                {
                    deviceId = URLDecoder.decode(urlEncodedDeviceId,CONNECTION_STRING_CHARSET.name());
                }
                catch (UnsupportedEncodingException e)
                {
                    // should never happen, since the encoding is hard-coded.
                    throw new IllegalStateException(e);
                }
            }
            // Codes_SRS_DEVICECLIENT_11_045: [The constructor shall save the device key as the value of 'SharedAccessKey' in the connection string.]
            else if (attr.startsWith(SHARED_ACCESS_KEY_ATTRIBUTE))
            {
                sharedAccessKey = attr.substring(SHARED_ACCESS_KEY_ATTRIBUTE.length());
            }
            else if (attr.startsWith(GATEWAY_HOSTNAME_ATTRIBUTE))
            {
                gatewayHostName = attr.substring(GATEWAY_HOSTNAME_ATTRIBUTE.length());
            }
        }

        initIotHubClient(hostname, gatewayHostName, deviceId, sharedAccessKey, protocol);
    }

    /**
     * Starts asynchronously sending and receiving messages from an IoT Hub. If
     * the client is already open, the function shall do nothing.
     *
     * @throws IOException if a connection to an IoT Hub is cannot be
     * established.
     */
    public void open() throws IOException
    {
        // Codes_SRS_DEVICECLIENT_11_028: [If the client is already open, the function shall do nothing.]
        if (this.state == IotHubClientState.OPEN)
        {
            return;
        }

        // Codes_SRS_DEVICECLIENT_11_035: [The function shall open the transport to communicate with an IoT Hub.]
        // Codes_SRS_DEVICECLIENT_11_036: [If an error occurs in opening the transport, the function shall throw an IOException.]
        this.transport.open();

        IotHubSendTask sendTask = new IotHubSendTask(this.transport);
        IotHubReceiveTask receiveTask = new IotHubReceiveTask(this.transport);

        this.taskScheduler = Executors.newScheduledThreadPool(2);
        // the scheduler waits until each execution is finished before
        // scheduling the next one, so executions of a given task
        // will never overlap.
        // Codes_SRS_DEVICECLIENT_11_023: [The function shall schedule send tasks to run every 5000 milliseconds.]
        this.taskScheduler.scheduleAtFixedRate(sendTask, 0,
                DEFAULT_SEND_PERIOD_MILLIS, TimeUnit.MILLISECONDS);
        // Codes_SRS_DEVICECLIENT_11_024: [The function shall schedule receive tasks to run every 5000 milliseconds.]
        this.taskScheduler.scheduleAtFixedRate(receiveTask, 0,
                current_receive_period_millis, TimeUnit.MILLISECONDS);

        this.state = IotHubClientState.OPEN;
    }


    /**
     * Completes all current outstanding requests and closes the IoT Hub client.
     * Must be called to terminate the background thread that is sending data to
     * IoT Hub. After {@code close()} is called, the IoT Hub client is no longer
     *  usable. If the client is already closed, the function shall do nothing.
     *
     * @throws IOException if the connection to an IoT Hub cannot be closed.
     */
    public void close() throws IOException
    {
        // Codes_SRS_DEVICECLIENT_11_031: [If the client is already closed, the function shall do nothing.]
        if (this.state == IotHubClientState.CLOSED)
        {
            return;
        }

        // Codes_SRS_DEVICECLIENT_11_010: [The function shall finish all ongoing tasks.]
        // Codes_SRS_DEVICECLIENT_11_011: [The function shall cancel all recurring tasks.]
        while (!this.transport.isEmpty())
        {

        }
        this.taskScheduler.shutdown();
        // Codes_SRS_DEVICECLIENT_11_037: [The function shall close the transport.]
        this.transport.close();
        this.state = IotHubClientState.CLOSED;
    }

    /**
     * Asynchronously sends an event message to the IoT Hub.
     *
     * @param message the message to be sent.
     * @param callback the callback to be invoked when a response is received.
     * Can be {@code null}.
     * @param callbackContext a context to be passed to the callback. Can be
     * {@code null} if no callback is provided.
     *
     * @throws IllegalArgumentException if the message provided is {@code null}.
     * @throws IllegalStateException if the client has not been opened yet or is
     * already closed.
     */
    public void sendEventAsync(Message message,
            IotHubEventCallback callback,
            Object callbackContext)
    {
        // Codes_SRS_DEVICECLIENT_11_039: [If the client is closed, the function shall throw an IllegalStateException.]
        if (this.state == IotHubClientState.CLOSED)
        {
            throw new IllegalStateException(
                    "Cannot send event from "
                            + "an IoT Hub client that is closed.");
        }

        // Codes_SRS_DEVICECLIENT_11_033: [If the message given is null, the function shall throw an IllegalArgumentException.]
        if (message == null)
        {
            throw new IllegalArgumentException("Cannot send message 'null'.");
        }

        // Codes_SRS_DEVICECLIENT_11_006: [The function shall add the message, with its associated callback and callback context, to the transport.]
        transport.addMessage(message, callback, callbackContext);
    }

    /**
     * Sets the message callback.
     *
     * @param callback the message callback. Can be {@code null}.
     * @param context the context to be passed to the callback. Can be {@code null}.
     *
     * @return itself, for fluent setting.
     *
     * @throws IllegalArgumentException if the callback is {@code null} but a context is
     * passed in.
     * @throws IllegalStateException if the callback is set after the client is
     * closed.
     */
    public DeviceClient setMessageCallback(
            MessageCallback callback,
            Object context)
    {
        // Codes_SRS_DEVICECLIENT_11_032: [If the callback is null but the context is non-null, the function shall throw an IllegalArgumentException.]
        if (callback == null && context != null)
        {
            throw new IllegalArgumentException(
                    "Cannot give non-null context for a null callback.");
        }

        // Codes_SRS_DEVICECLIENT_11_012: [The function shall set the message callback, with its associated context.]
        this.config.setMessageCallback(callback, context);
        return this;
    }

    /**
     * Initializes an IoT Hub device client with the given parameters.
     *
     * @param iotHubHostname the IoT Hub hostname.
     * @param gatewayHostName the Protocol Gateway hostname.
     * @param deviceId the device ID.
     * @param deviceKey the device key.
     * @param protocol the communication protocol used (i.e. HTTPS).
     *
     * @throws IllegalArgumentException if any of {@code iotHubHostname}, {@code deviceId},
     * {@code deviceKey}, or {@code protocol} are {@code null}; or if {@code iotHubHostname}
     * does not contain a valid IoT Hub name as its prefix.
     * @throws URISyntaxException if the IoT Hub hostname does not conform to
     * RFC 3986.
     */
    protected void initIotHubClient(String iotHubHostname, String gatewayHostName, String deviceId,
            String deviceKey, IotHubClientProtocol protocol)
            throws URISyntaxException
    {
        // Codes_SRS_DEVICECLIENT_11_048: [If no value for 'HostName' is found in the connection string, the function shall throw an IllegalArgumentException.]
        // Codes_SRS_DEVICECLIENT_11_025: [If iotHubHostname is null, the constructor shall throw an IllegalArgumentException.]
        if (iotHubHostname == null)
        {
            throw new IllegalArgumentException(
                    "IoT Hub hostname cannot be null.");
        }
        // Codes_SRS_DEVICECLIENT_11_049: [If no value for 'DeviceId' is found in the connection string, the function shall throw an IllegalArgumentException.]
        // Codes_SRS_DEVICECLIENT_11_026: [If deviceId is null, the constructor shall throw an IllegalArgumentException.]
        if (deviceId == null)
        {
            throw new IllegalArgumentException("Device ID cannot be null.");
        }
        // Codes_SRS_DEVICECLIENT_11_050: [If no argument for 'SharedAccessKey' is found in the connection string, the function shall throw an IllegalArgumentException.]
        // Codes_SRS_DEVICECLIENT_11_027: [If deviceKey is null, the constructor shall throw an IllegalArgumentException.]
        if (deviceKey == null)
        {
            throw new IllegalArgumentException("Device key cannot be null.");
        }
        // Codes_SRS_DEVICECLIENT_11_051: [If protocol is null, the function shall throw an IllegalArgumentException.]
        // Codes_SRS_DEVICECLIENT_11_034: [If protocol is null, the constructor shall throw an IllegalArgumentException.]
        if (protocol == null)
        {
            throw new IllegalArgumentException("Protocol cannot be null.");
        }

        // Codes_SRS_DEVICECLIENT_11_052: [The constructor shall save the IoT Hub hostname, device ID, and device key as configuration parameters.]
        this.config = new DeviceClientConfig(iotHubHostname, gatewayHostName, deviceId, deviceKey);
        // Codes_SRS_DEVICECLIENT_11_046: [The constructor shall initialize the IoT Hub transport that uses the protocol specified.]
        // Codes_SRS_DEVICECLIENT_11_004: [The constructor shall initialize the IoT Hub transport that uses the protocol specified.]
        switch (protocol)
        {
            case HTTPS:
                this.transport = new HttpsTransport(this.config);
                current_receive_period_millis = DEFAULT_RECEIVE_PERIOD_MILLIS_HTTPS;
                break;
            case AMQPS:
                this.transport = new AmqpsTransport(this.config);
                current_receive_period_millis = DEFAULT_RECEIVE_PERIOD_MILLIS_AMQPS;
                break;
            case MQTT:
                this.transport = new MqttTransport(this.config);
                current_receive_period_millis = DEFAULT_RECEIVE_PERIOD_MILLIS_MQTT;
                break;
            default:
                // should never happen.
                throw new IllegalStateException(
                        "Invalid client protocol specified.");
        }

        this.state = IotHubClientState.CLOSED;
    }

    protected DeviceClient()
    {
        this.config = null;
        this.transport = null;
        this.taskScheduler = null;
    }

    /**
     * Sets a runtime option identified by parameter {@code optionName}
     * to {@code value}.
     *
     * The options that can be set via this API are:
     *	    - <b>SetMinimumPollingInterval</b> - this option is applicable only
     *	      when the transport configured with this client is HTTP. This
     *	      option specifies the interval in milliseconds between calls to
     *	      the service checking for availability of new messages. The value
     *	      is expected to be of type {@code long}.
     *
     * @param optionName the option name to modify
     * @param value an object of the appropriate type for the option's value
     */
    public void setOption(String optionName, Object value)
    {
        // Codes_SRS_DEVICECLIENT_02_001: [If optionName is null or not an option handled by the client, then it shall throw IllegalArgumentException.]
        if(optionName == null)
        {
            throw new IllegalArgumentException("optionName is null");
        }
        else
        {
            // Codes_SRS_DEVICECLIENT_02_003: [Available only for HTTP.]
            if(this.transport.getClass() == HttpsTransport.class)
            {
                if(optionName.equals("SetMinimumPollingInterval"))
                {
                    if(this.state!=IotHubClientState.CLOSED)
                    {
                        throw new IllegalStateException("setOption only works when the transport is closed");
                    }
                    else
                    {
                        // Codes_SRS_DEVICECLIENT_02_004: [Value needs to have type long].
                        if (value instanceof Long)
                        {
                            this.current_receive_period_millis = (long) value;
                        }
                        else
                        {
                            throw new IllegalArgumentException("value is not long = " + value);
                        }
                    }
                }
                else
                {
                        // Codes_SRS_DEVICECLIENT_02_001: [If optionName is null or not an option handled by the client, then it shall throw IllegalArgumentException.]
                    throw new IllegalArgumentException("optionName is unknown = "+optionName);
                }
            }
            else
            {
                throw new IllegalArgumentException("AMQP does not have setOption");
            }
        }
    }
}
