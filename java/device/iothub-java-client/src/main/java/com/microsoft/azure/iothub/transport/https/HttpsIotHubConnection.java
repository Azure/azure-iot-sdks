// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

package com.microsoft.azure.iothub.transport.https;

import com.microsoft.azure.iothub.*;
import com.microsoft.azure.iothub.auth.IotHubSasToken;
import com.microsoft.azure.iothub.net.*;
import com.microsoft.azure.iothub.transport.TransportUtils;

import java.io.IOException;
import java.net.URL;

/**
 * An HTTPS connection between a device and an IoT Hub. Contains functionality
 * for synchronously connecting to the different IoT Hub resource URIs.
 */
public class HttpsIotHubConnection
{
    /** The HTTPS connection lock. */
    protected static final Object HTTPS_CONNECTION_LOCK = new Object();

    /** The client configuration. */
    protected final DeviceClientConfig config;
    /**
     * The message e-tag. Obtained when the device receives a
     * message and used when sending a message result back to
     * the IoT Hub.
     */
    protected String messageEtag;

    /**
     * Constructs an instance from the given {@link DeviceClientConfig}
     * object.
     *
     * @param config the client configuration.
     */
    public HttpsIotHubConnection(DeviceClientConfig config)
    {
        synchronized (HTTPS_CONNECTION_LOCK)
        {
            // Codes_SRS_HTTPSIOTHUBCONNECTION_11_001: [The constructor shall save the client configuration.]
            this.config = config;
        }
    }

    /**
     * Sends an event message.
     *
     * @param msg the event message.
     *
     * @return the status code from sending the event message.
     *
     * @throws IOException if the IoT Hub could not be reached.
     */
    public IotHubStatusCode sendEvent(HttpsMessage msg) throws IOException
    {
        synchronized (HTTPS_CONNECTION_LOCK)
        {
            String iotHubHostname = this.config.getIotHubHostname();
            String deviceId = this.config.getDeviceId();
            int readTimeoutMillis = this.config.getReadTimeoutMillis();

            // Codes_SRS_HTTPSIOTHUBCONNECTION_11_002: [The function shall send a request to the URL 'https://[iotHubHostname]/devices/[deviceId]/messages/events?api-version=2016-02-03'.]
            IotHubEventUri eventUri = new IotHubEventUri(iotHubHostname, deviceId);
            URL eventUrl = new URL("https://" + eventUri.toString());
            IotHubSasToken sasToken = new IotHubSasToken(this.config, System.currentTimeMillis() / 1000l +
                    this.config.getTokenValidSecs() + 1l);

            // Codes_SRS_HTTPSIOTHUBCONNECTION_11_003: [The function shall send a POST request.]
            // Codes_SRS_HTTPSIOTHUBCONNECTION_11_004: [The function shall set the request body to the message body.]
            HttpsRequest request =
                    new HttpsRequest(eventUrl, HttpsMethod.POST, msg.getBody());
            // Codes_SRS_HTTPSIOTHUBCONNECTION_11_005: [The function shall write each message property as a request header.]
            for (MessageProperty property : msg.getProperties())
            {
                request.setHeaderField(property.getName(),
                        property.getValue());
            }
            // Codes_SRS_HTTPSIOTHUBCONNECTION_11_006: [The function shall set the request read timeout to be the configuration parameter readTimeoutMillis.]
            request.setReadTimeoutMillis(readTimeoutMillis).
                    // Codes_SRS_HTTPSIOTHUBCONNECTION_11_007: [The function shall set the header field 'authorization' to be a valid SAS token generated from the configuration parameters.]
                            setHeaderField("authorization", sasToken.toString()).
                    // Codes_SRS_HTTPSIOTHUBCONNECTION_11_008: [The function shall set the header field 'iothub-to' to be '/devices/[deviceId]/messages/events'.]
                            setHeaderField("iothub-to", eventUri.getPath()).
                    // Codes_SRS_HTTPSIOTHUBCONNECTION_11_009: [The function shall set the header field 'content-type' to be the message content type.]
                            setHeaderField("content-type", msg.getContentType());

            // Codes_SRS_HTTPSIOTHUBCONNECTION_11_012: [If the IoT Hub could not be reached, the function shall throw an IOException.]
            HttpsResponse response = request.send();

            // Codes_SRS_HTTPSIOTHUBCONNECTION_11_010: [The function shall return the IoT Hub status code included in the response.]
            return IotHubStatusCode.getIotHubStatusCode(
                    response.getStatus());
        }
    }

    /**
     * Receives a message, if one exists.
     *
     * @return a message, or null if none exists.
     *
     * @throws IOException if the IoT Hub could not be reached.
     */
    public Message receiveMessage() throws IOException
    {
        synchronized (HTTPS_CONNECTION_LOCK)
        {
            String iotHubHostname = this.config.getIotHubHostname();
            String deviceId = this.config.getDeviceId();
            int readTimeoutMillis = this.config.getReadTimeoutMillis();
            int messageLockTimeoutSecs = this.config.getMessageLockTimeoutSecs();

            // Codes_SRS_HTTPSIOTHUBCONNECTION_11_013: [The function shall send a request to the URL 'https://[iotHubHostname]/devices/[deviceId]/messages/devicebound?api-version=2016-02-03'.]
            IotHubMessageUri messageUri = new IotHubMessageUri(iotHubHostname, deviceId);
            URL messageUrl = new URL("https://" + messageUri.toString());

            IotHubSasToken sasToken = new IotHubSasToken(this.config, System.currentTimeMillis() / 1000l +
                    this.config.getTokenValidSecs() + 1l);

            // Codes_SRS_HTTPSIOTHUBCONNECTION_11_014: [The function shall send a GET request.]
            HttpsRequest request =
                    new HttpsRequest(messageUrl, HttpsMethod.GET, new byte[0]).
                            // Codes_SRS_HTTPSIOTHUBCONNECTION_11_015: [The function shall set the request read timeout to be the configuration parameter readTimeoutMillis.]
                                    setReadTimeoutMillis(readTimeoutMillis).
                            // Codes_SRS_HTTPSIOTHUBCONNECTION_11_016: [The function shall set the header field 'authorization' to be a valid SAS token generated from the configuration parameters.]
                                    setHeaderField("authorization",
                                    sasToken.toString()).
                            // Codes_SRS_HTTPSIOTHUBCONNECTION_11_017: [The function shall set the header field 'iothub-to' to be '/devices/[deviceId]/messages/devicebound'.]
                                    setHeaderField("iothub-to",
                                    messageUri.getPath()).
                            // Codes_SRS_HTTPSIOTHUBCONNECTION_11_018: [The function shall set the header field 'iothub-messagelocktimeout' to be the configuration parameter messageLockTimeoutSecs.]
                                    setHeaderField("iothub-messagelocktimeout",
                                    Integer.toString(messageLockTimeoutSecs));

            // Codes_SRS_HTTPSIOTHUBCONNECTION_11_023: [If the IoT Hub could not be reached, the function shall throw an IOException.]
            HttpsResponse response = request.send();

            // Codes_SRS_HTTPSIOTHUBCONNECTION_11_021: [If a response with IoT Hub status code OK is not received, the function shall return null.]
            Message msg = null;
            // Codes_SRS_HTTPSIOTHUBCONNECTION_11_019: [If a response with IoT Hub status code OK is received, the function shall return the IoT Hub message included in the response.]
            IotHubStatusCode messageStatus = IotHubStatusCode.getIotHubStatusCode(response.getStatus());
            if (messageStatus == IotHubStatusCode.OK)
            {
                // Codes_SRS_HTTPSIOTHUBCONNECTION_11_020: [If a response with IoT Hub status code OK is received, the function shall save the response header field 'etag'.]
                this.messageEtag = sanitizeEtag(response.getHeaderField("etag"));

                HttpsSingleMessage httpsMsg = HttpsSingleMessage.parseHttpsMessage(response);
                msg = httpsMsg.toMessage();
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
        synchronized (HTTPS_CONNECTION_LOCK)
        {
            // Codes_SRS_HTTPSIOTHUBCONNECTION_11_039: [If the function is called before receiveMessage() returns a message, the function shall throw an IllegalStateException.]
            if (this.messageEtag == null)
            {
                throw new IllegalStateException("Cannot send a message "
                        + "result before a message is received.");
            }

            String iotHubHostname = this.config.getIotHubHostname();
            String deviceId = this.config.getDeviceId();
            int readTimeoutMillis = this.config.getReadTimeoutMillis();

            String resultUri = "https://";
            String resultPath;
            URL resultUrl;
            HttpsRequest request;
            switch (result)
            {
                case COMPLETE:
                    // Codes_SRS_HTTPSIOTHUBCONNECTION_11_024: [If the result is COMPLETE, the function shall send a request to the URL 'https://[iotHubHostname]/devices/[deviceId]/messages/devicebound/[eTag]?api-version=2016-02-03'.]
                    IotHubCompleteUri completeUri =
                            new IotHubCompleteUri(iotHubHostname, deviceId,
                                    this.messageEtag);
                    resultUri += completeUri.toString();
                    // Codes_SRS_HTTPSIOTHUBCONNECTION_11_026: [If the result is COMPLETE, the function shall set the header field 'iothub-to' to be '/devices/[deviceId]/messages/devicebound/[eTag]'.]
                    resultPath = completeUri.getPath();
                    resultUrl = new URL(resultUri);
                    // Codes_SRS_HTTPSIOTHUBCONNECTION_11_025: [If the result is COMPLETE, the function shall send a DELETE request.]
                    request = new HttpsRequest(resultUrl, HttpsMethod.DELETE,
                            new byte[0]);
                    break;
                case ABANDON:
                    // Codes_SRS_HTTPSIOTHUBCONNECTION_11_027: [If the result is ABANDON, the function shall send a request to the URL 'https://[iotHubHostname]/devices/[deviceId]/messages/devicebound/[eTag]/abandon?api-version=2016-02-03'.]
                    IotHubAbandonUri abandonUri =
                            new IotHubAbandonUri(iotHubHostname, deviceId,
                                    this.messageEtag);
                    resultUri += abandonUri.toString();
                    // Codes_SRS_HTTPSIOTHUBCONNECTION_11_029: [If the result is ABANDON, the function shall set the header field 'iothub-to' to be '/devices/[deviceId]/messages/devicebound/[eTag]/abandon'.]
                    resultPath = abandonUri.getPath();
                    resultUrl = new URL(resultUri);
                    // Codes_SRS_HTTPSIOTHUBCONNECTION_11_028: [If the result is ABANDON, the function shall send a POST request.]
                    // The IoT Hub service requires the content-length header to be
                    // set but the Java SE connection omits content-length
                    // if content-length == 0. We include a placeholder body to
                    // make the connection include a content-length.
                    request = new HttpsRequest(resultUrl, HttpsMethod.POST,
                            new byte[1]);
                    break;
                case REJECT:
                    // Codes_SRS_HTTPSIOTHUBCONNECTION_11_030: [If the result is REJECT, the function shall send a request to the URL 'https://[iotHubHostname]/devices/[deviceId]/messages/devicebound/[eTag]??reject=true&api-version=2016-02-03' (the query parameters can be in any order).]
                    IotHubRejectUri rejectUri =
                            new IotHubRejectUri(iotHubHostname, deviceId,
                                    this.messageEtag);
                    resultUri += rejectUri.toString();
                    // Codes_SRS_HTTPSIOTHUBCONNECTION_11_032: [If the result is REJECT, the function shall set the header field 'iothub-to' to be '/devices/[deviceId]/messages/devicebound/[eTag]'.]
                    resultPath = rejectUri.getPath();
                    resultUrl = new URL(resultUri);
                    // Codes_SRS_HTTPSIOTHUBCONNECTION_11_031: [If the result is REJECT, the function shall send a DELETE request.]
                    request = new HttpsRequest(resultUrl, HttpsMethod.DELETE,
                            new byte[0]);
                    break;
                default:
                    // should never happen.
                    throw new IllegalStateException(
                            "Invalid message result specified.");
            }

            IotHubSasToken sasToken = new IotHubSasToken(this.config, System.currentTimeMillis() / 1000l +
                                    this.config.getTokenValidSecs() + 1l);

            // Codes_SRS_HTTPSIOTHUBCONNECTION_11_033: [The function shall set the request read timeout to be the configuration parameter readTimeoutMillis.]
            request.setReadTimeoutMillis(readTimeoutMillis).
                    // Codes_SRS_HTTPSIOTHUBCONNECTION_11_034: [The function shall set the header field 'authorization' to be a valid SAS token generated from the configuration parameters.]
                            setHeaderField("authorization", sasToken.toString()).
                    setHeaderField("iothub-to", resultPath).
                    // Codes_SRS_HTTPSIOTHUBCONNECTION_11_035: [The function shall set the header field 'if-match' to be the e-tag saved when receiveMessage() was previously called.]
                            setHeaderField("if-match", this.messageEtag);

            // Codes_SRS_HTTPSIOTHUBCONNECTION_11_037: [If the IoT Hub could not be reached, the function shall throw an IOException.]
            HttpsResponse response = request.send();
            // Codes_SRS_HTTPSIOTHUBCONNECTION_11_038: [If the IoT Hub status code in the response is not OK_EMPTY, the function shall throw an IOException.]
            IotHubStatusCode resultStatus =
                    IotHubStatusCode.getIotHubStatusCode(
                            response.getStatus());
            if (resultStatus != IotHubStatusCode.OK_EMPTY)
            {
                String errMsg = String.format(
                        "Sending message result failed with status %s.\n",
                        resultStatus.name());
                throw new IOException(errMsg);
            }
        }
    }

    /**
     * Removes double quotes from the e-tag property.
     *
     * @param dirtyEtag the dirty e-tag value.
     *
     * @return the e-tag value with double quotes removed.
     */
    protected static String sanitizeEtag(String dirtyEtag)
    {
        return dirtyEtag.replace("\"", "");
    }
}
