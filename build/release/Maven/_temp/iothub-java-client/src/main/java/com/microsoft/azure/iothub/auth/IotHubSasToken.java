// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

package com.microsoft.azure.iothub.auth;

/** Grants device access to an IoT Hub for the specified amount of time. */
public final class IotHubSasToken
{
    /**
     * The SAS token format. The parameters to be interpolated are, in order:
     * the signature, the expiry time, the key name (device ID), and the
     * resource URI.
     */
    public static final String TOKEN_FORMAT =
            "SharedAccessSignature sig=%s&se=%s&skn=%s&sr=%s";

    /** Components of the SAS token. */
    protected final String signature;
    /** The time, as a UNIX timestamp, before which the token is valid. */
    protected final long expiryTime;
    protected final String keyName;
    /**
     * The URI for a connection from a device to an IoT Hub. Does not include a
     * protocol.
     */
    protected final String resourceUri;

    public boolean appendSknValue;

    /**
     * Constructor. Generates a SAS token that grants access to an IoT Hub for
     * the specified amount of time.
     *
     * @param resourceUri the resource URI.
     * @param deviceId the device ID.
     * @param deviceKey the device key.
     * @param expiryTime the time, as a UNIX timestamp, after which the token
     * will become invalid.
     */
    public IotHubSasToken(String resourceUri, String deviceId, String deviceKey,
            long expiryTime)
    {
        // Codes_SRS_IOTHUBSASTOKEN_11_002: [The expiry time shall be the given expiry time, where it is a UNIX timestamp and indicates the time after which the token becomes invalid.]
        this.expiryTime = expiryTime;

        // Codes_SRS_IOTHUBSASTOKEN_11_003: [The key name shall be the device ID.]
        this.keyName = deviceId;
        // Codes_SRS_IOTHUBSASTOKEN_11_004: [The resource URI shall be the given resource URI.]
        this.resourceUri = resourceUri;

        // Codes_SRS_IOTHUBSASTOKEN_11_005: [The signature shall be correctly computed and set.]
        Signature sig = new Signature(this.resourceUri, this.expiryTime, deviceKey);
        this.signature = sig.toString();

        // Codes_SRS_IOTHUBSASTOKEN_08_012: [The skn key value shall be empty if connecting to IoT Hub, or the device Id if connecting to the Event Hub directly]
        this.appendSknValue = true;
    }

    /**
     * Returns the string representation of the SAS token.
     *
     * @return the string representation of the SAS token.
     */
    @Override
    public String toString()
    {
        // Tests_SRS_IOTHUBSASTOKEN_11_006: [The function shall return the string representation of the SAS token.]
        return buildSasToken();
    }


    /*
     * For future use, when the device ID (keyName) will possibly be passed on the SAS token string.
     */
    protected String buildSasToken() {
        String keyName = "";

        // Codes_SRS_IOTHUBSASTOKEN_08_012: [The skn key value shall be empty if connecting to IoT Hub, or the device Id if connecting to the Event Hub directly]
        if (appendSknValue == true) {
            keyName = this.keyName;
        }
        
        // Codes_SRS_IOTHUBSASTOKEN_11_001: [The SAS token shall have the format "SharedAccessSignature sig=<signature >&se=<expiryTime>&skn=&sr=<resourceURI>". The params can be in any order.]
        return String.format(TOKEN_FORMAT, this.signature, this.expiryTime, keyName, this.resourceUri);
    }

    protected IotHubSasToken()
    {
        this.signature = null;
        this.expiryTime = 0l;
        this.keyName = null;
        this.appendSknValue = true;
        this.resourceUri = null;
    }
}
