// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

package com.microsoft.azure.iothub;

import java.net.URI;
import java.net.URISyntaxException;

/**
 * Configuration settings for an IoT Hub client. Validates all user-defined
 * settings.
 */
public final class DeviceClientConfig
{
    /**
     * The number of seconds after which the generated SAS token for a message
     * will become invalid. We also use the expiry time, which is computed as
     * {@code currentTime() + DEVICE_KEY_VALID_LENGTH}, as a salt when generating our
     * SAS token. Use {@link #getTokenValidSecs()} instead in case the field becomes
     * configurable later on.
     */
    private long tokenValidSecs = 3600;

    /** The default value for readTimeoutMillis. */
    public static final int DEFAULT_READ_TIMEOUT_MILLIS = 240000;
    /** The default value for messageLockTimeoutSecs. */
    public static final int DEFAULT_MESSAGE_LOCK_TIMEOUT_SECS = 180;

    protected final String iotHubHostname;
    protected final String iotHubName;
    protected final String deviceId;
    protected final String deviceKey;
    protected final String sharedAccessToken;
    protected String pathToCertificate;

    /**
     * The callback to be invoked if a message is received.
     */
    protected MessageCallback messageCallback;
    /** The context to be passed in to the message callback. */
    protected Object messageContext;

    /**
     * Constructor.
     *
     * @param iotHubHostname the IoT Hub hostname.
     * @param deviceId the device ID.
     * @param deviceKey the device key.
     * @param sharedAccessToken the shared access token.
     *
     *
     * @throws URISyntaxException if the IoT Hub hostname does not conform to RFC 3986.
     * @throws IllegalArgumentException if the IoT Hub hostname does not contain
     * a valid IoT Hub name as its prefix.
     */
    public DeviceClientConfig(String iotHubHostname, String deviceId,
                              String deviceKey, String sharedAccessToken) throws URISyntaxException
    {
        // Codes_SRS_DEVICECLIENTCONFIG_11_014: [If the IoT Hub hostname is
        // not valid URI, the constructor shall throw a URISyntaxException.]
        new URI(iotHubHostname);

        // Codes_SRS_DEVICECLIENTCONFIG_11_015: [If the IoT Hub hostname does not contain a '.', the function shall throw an IllegalArgumentException.]
        int iotHubNameEndIdx = iotHubHostname.indexOf(".");
        if (iotHubNameEndIdx == -1)
        {
            String errStr = String.format(
                    "%s did not include a valid IoT Hub name as its prefix. "
                            + "An IoT Hub hostname has the following format: "
                            + "[iotHubName].[valid HTML chars]+",
                    iotHubHostname);
            throw new IllegalArgumentException(errStr);
        }

        // Codes_SRS_DEVICECLIENTCONFIG_11_001: [The constructor shall save the IoT Hub hostname, device ID, and device key.]
        this.iotHubHostname = iotHubHostname;
        this.iotHubName = iotHubHostname.substring(0, iotHubNameEndIdx);
        this.deviceId = deviceId;
        this.deviceKey = deviceKey;
        // Codes_SRS_DEVICECLIENTCONFIG_25_017: [**The constructor shall save sharedAccessToken.**] **
        this.sharedAccessToken = sharedAccessToken;

    }

    /**
     * Setter for the providing trusted certificate.
     *
     * @param pathToCertificate path to the certificate for one way authentication.
     */
    public void setPathToCert(String pathToCertificate)
    {
        this.pathToCertificate = pathToCertificate;
    }

    /**
     * Setter for the message callback. Can be {@code null}.
     *
     * @param callback the message callback. Can be {@code null}.
     * @param context the context to be passed in to the callback.
     */
    public void setMessageCallback(MessageCallback callback,
            Object context)
    {
        // Codes_SRS_DEVICECLIENTCONFIG_11_006: [The function shall set the message callback, with its associated context.]
        this.messageCallback = callback;
        this.messageContext = context;
    }

    /**
     * Getter for the IoT Hub hostname.
     *
     * @return the IoT Hub hostname.
     */
    public String getIotHubHostname()
    {
        // Codes_SRS_DEVICECLIENTCONFIG_11_002: [The function shall return the IoT Hub hostname given in the constructor.]
        return this.iotHubHostname;
    }

    /**
     * Getter for the IoT Hub name.
     *
     * @return the IoT Hub name.
     */
    public String getIotHubName()
    {
        // Codes_SRS_DEVICECLIENTCONFIG_11_002: [The function shall return the IoT Hub name given in the constructor, where the IoT Hub name is embedded in the IoT Hub hostname as follows: [IoT Hub name].[valid HTML chars]+.]
        return this.iotHubName;
    }

    /**
     * Getter for the device ID.
     *
     * @return the device ID.
     */
    public String getDeviceId()
    {
        // Codes_SRS_DEVICECLIENTCONFIG_11_003: [The function shall return the device ID given in the constructor.]
        return this.deviceId;
    }

    /**
     * Getter for the device key.
     *
     * @return the device key.
     */
    public String getDeviceKey()
    {
        // Codes_SRS_DEVICECLIENTCONFIG_11_004: [The function shall return the device key given in the constructor.]
        return this.deviceKey;
    }

    /**
     * Getter for the shared access signature.
     *
     * @return the shared access signature.
     */
    public String getSharedAccessToken()
    {
        // Codes_SRS_DEVICECLIENTCONFIG_25_018: [**The function shall return the SharedAccessToken given in the constructor.**] **
        return this.sharedAccessToken;
    }

    /**
     * Getter for the number of seconds a SAS token should be valid for. A
     * message that arrives at an IoT Hub in time of length greater than this
     * value will be rejected by the IoT Hub.
     *
     * @return the number of seconds a message in transit to an IoT Hub is valid
     * for.
     */
    public long getTokenValidSecs()
    {
        // Codes_SRS_DEVICECLIENTCONFIG_11_005: [The function shall return the value of tokenValidSecs.]
        return this.tokenValidSecs;
    }

    /**
     * Setter for the number of seconds a SAS token should be valid for. A
     * message that arrives at an IoT Hub in time of length greater than this
     * value will be rejected by the IoT Hub.
     */
    public void setTokenValidSecs(long expiryTime)
    {
        // Codes_SRS_DEVICECLIENTCONFIG_25_016: [The function shall set the value of tokenValidSecs.]
        this.tokenValidSecs = expiryTime;
    }

    /**
     * Getter for the timeout, in milliseconds, after a connection is
     * established for the server to respond to the request.
     *
     * @return the timeout, in milliseconds, after a connection is established
     * for the server to respond to the request.
     */
    public int getReadTimeoutMillis()
    {
        // Codes_SRS_DEVICECLIENTCONFIG_11_012: [The function shall return 240000ms.]
        return DEFAULT_READ_TIMEOUT_MILLIS;
    }

    /**
     * Getter for the message callback.
     *
     * @return the message callback.
     */
    public MessageCallback getMessageCallback()
    {
        // Codes_SRS_DEVICECLIENTCONFIG_11_010: [The function shall return the current message callback.]
        return this.messageCallback;
    }

    /**
     * Getter for the context to be passed in to the message callback.
     *
     * @return the message context.
     */
    public Object getMessageContext()
    {
        // Codes_SRS_DEVICECLIENTCONFIG_11_011: [The function shall return the current message context.]
        return this.messageContext;
    }

    /**
     * Getter for the timeout, in seconds, for the lock that the client has on a
     * received message.
     *
     * @return the timeout, in seconds, for a received message lock.
     */
    public int getMessageLockTimeoutSecs()
    {
        // Codes_SRS_DEVICECLIENTCONFIG_11_013: [The function shall return 180s.]
        return DEFAULT_MESSAGE_LOCK_TIMEOUT_SECS;
    }

    /**
     * Getter for the path to the certificate.
     *
     * @return the path to certificate.
     */
    public String getPathToCertificate()
    {
        if (this.pathToCertificate  == null) {
            DefaultCertificate cert = new DefaultCertificate();
            this.pathToCertificate = cert.getDefaultCertificate();
        }
        return this.pathToCertificate;
    }

    protected DeviceClientConfig()
    {
        this.iotHubHostname = null;
        this.iotHubName = null;
        this.deviceId = null;
        this.deviceKey = null;
        this.sharedAccessToken = null;
        this.pathToCertificate = null;
    }

}
