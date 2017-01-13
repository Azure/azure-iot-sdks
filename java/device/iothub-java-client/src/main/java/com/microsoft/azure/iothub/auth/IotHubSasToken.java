// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

package com.microsoft.azure.iothub.auth;

import com.microsoft.azure.iothub.DeviceClientConfig;
import com.microsoft.azure.iothub.net.IotHubUri;

/** Grants device access to an IoT Hub for the specified amount of time. */
public final class IotHubSasToken
{
    /**
     * The SAS token format. The parameters to be interpolated are, in order:
     * the signature, the expiry time, the key name (device ID), and the
     * resource URI.
     */
    public static final String TOKEN_FORMAT = "SharedAccessSignature sig=%s&se=%s&sr=%s";
    protected String sasToken = null;

    /** Components of the SAS token. */
    protected String signature = null;
    /** The time, as a UNIX timestamp, before which the token is valid. */
    protected long expiryTime = 0l;
    /**
     * The URI for a connection from a device to an IoT Hub. Does not include a
     * protocol.
     */
    protected String scope = null;

    /**
     * Constructor. Generates a SAS token that grants access to an IoT Hub for
     * the specified amount of time.
     *
     * @param config the device client config.
     * @param expiryTime the time, as a UNIX timestamp, after which the token
     * will become invalid.
     */
    public IotHubSasToken(DeviceClientConfig config, long expiryTime)
    {

        // Codes_SRS_IOTHUBSASTOKEN_25_005: [**If device key is provided then the signature shall be correctly computed and set.**]**
        if (config.getDeviceKey() != null) {
            // Tests_SRS_IOTHUBSASTOKEN_11_002: [**The constructor shall save all input parameters to member variables.**]
            this.scope = IotHubUri.getResourceUri(config.getIotHubHostname(), config.getDeviceId());
            this.expiryTime = expiryTime;

            Signature sig = new Signature(this.scope, this.expiryTime, config.getDeviceKey());
            this.signature = sig.toString();
        }
        // Codes_SRS_IOTHUBSASTOKEN_25_007: [**If device key is not provided in config then the SASToken from config shall be used.**]**
        else if(config.getSharedAccessToken() != null)
        {
            this.sasToken = config.getSharedAccessToken();
            // Codes_SRS_IOTHUBSASTOKEN_25_008: [**The required format for the SAS Token shall be verified and IllegalArgumentException is thrown if unmatched.**]**
            if (!isSasFormat())
                throw new IllegalArgumentException("SasToken format is invalid");
        }
        else
        {
            this.signature = null;
            this.sasToken = null;
        }
    }

    /**
     * Returns the string representation of the SAS token.
     *
     * @return the string representation of the SAS token.
     */
    @Override
    public String toString()
    {
        // Codes_SRS_IOTHUBSASTOKEN_25_009: [**If SAS Token was provided by config it should be returned as string **]**
        if (this.sasToken != null)
        {
            if(isSasFormat())
                return this.sasToken;
            else
                throw new IllegalArgumentException("SasToken format is invalid");
        }
        else if(this.signature != null && this.expiryTime != 0l && this.scope!= null)
        {
            //Codes_SRS_IOTHUBSASTOKEN_25_010: [**If SAS Token was not provided by config it should be built and returned as string **]**
            return buildSasToken();
        }
        else
            return null;
    }

    protected boolean isSasFormat() {

        /**
         * The SAS token format. The parameters to be interpolated are, in any order:
         * the signature, the expiry time, and the resource URI.
         */
        if (this.sasToken != null)
        {
            if(this.sasToken.startsWith("SharedAccessSignature"))
            {
                if(this.sasToken.contains("sr=") && this.sasToken.contains("se=") &&
                        this.sasToken.contains("sig="))
                    return true;
            }

        }
        return false;
    }

    protected String buildSasToken() {
        // Codes_SRS_IOTHUBSASTOKEN_11_001: [The SAS token shall have the format "SharedAccessSignature sig=<signature >&se=<expiryTime>&sr=<resourceURI>". The params can be in any order.]
        return String.format(TOKEN_FORMAT, this.signature, this.expiryTime, this.scope);
    }

    protected IotHubSasToken()
    {
        this.signature = null;
        this.expiryTime = 0l;
        this.scope = null;
        this.sasToken = null;
    }
}
