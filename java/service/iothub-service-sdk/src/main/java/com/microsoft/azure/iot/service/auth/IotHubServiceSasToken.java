/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.auth;

import com.microsoft.azure.iot.service.sdk.IotHubConnectionString;
import org.apache.commons.codec.binary.Base64;

import javax.crypto.Mac;
import javax.crypto.spec.SecretKeySpec;
import java.net.URLEncoder;
import java.nio.charset.StandardCharsets;

/** 
 * Grants device access to an IoT Hub for the specified amount of time. 
 */
public final class IotHubServiceSasToken
{
    long TOKEN_VALID_SECS = 365*24*60*60;

    /**
     * The SAS token format. The parameters to be interpolated are, in order:
     * the signature
     * the resource URI
     * the expiry time
     * the key name
     * Example: {@code SharedAccessSignature sr=IOTHUBURI&sig=SIGNATURE&se=EXPIRY&skn=SHAREDACCESSKEYNAME}
     */
    public static final String TOKEN_FORMAT = "SharedAccessSignature sr=%s&sig=%s&se=%s&skn=%s";

    /* The URI for a connection to an IoT Hub */
    protected final String resourceUri;
    /* The value of the SharedAccessKey */
    protected final String keyValue;
    /* The time, as a UNIX timestamp, before which the token is valid. */
    protected final long expiryTime;
    /* The value of SharedAccessKeyName */
    protected final String keyName;
    /* The SAS token that grants access. */
    protected final String token;

    /**
     * Constructor. Generates a SAS token that grants access to an IoT Hub for
     * the specified amount of time. (1 year specified in TOKEN_VALID_SECS)
     *
     * @param iotHubConnectionString connection string object
     * will become invalid.
     */
    public IotHubServiceSasToken(IotHubConnectionString iotHubConnectionString)
    {
        if (iotHubConnectionString == null)
        {
            throw new IllegalArgumentException();
        }
        this.resourceUri = iotHubConnectionString.getHostName();
        this.keyValue = iotHubConnectionString.getSharedAccessKey();
        this.keyName = iotHubConnectionString.getSharedAccessKeyName();
        this.expiryTime = buildExpiresOn();
        this.token =  buildToken();
    }

    /**
     * Helper function to build the token string
     *
     * @return Valid token string
     */
    private String buildToken()
    {
        String targetUri;
        try
        {
            targetUri = URLEncoder.encode(this.resourceUri.toLowerCase(), String.valueOf(StandardCharsets.UTF_8));
            String toSign = targetUri + "\n" + this.expiryTime;

            // Get an hmac_sha1 key from the raw key bytes
            byte[] keyBytes = Base64.decodeBase64(this.keyValue.getBytes("UTF-8"));

            SecretKeySpec signingKey = new SecretKeySpec(keyBytes, "HmacSHA256");

            // Get an hmac_sha1 Mac instance and initialize with the signing key
            Mac mac = Mac.getInstance("HmacSHA256");
            mac.init(signingKey);

            // Compute the hmac on input data bytes
            byte[] rawHmac = mac.doFinal(toSign.getBytes("UTF-8"));

            // Convert raw bytes to Hex
            String signature = URLEncoder.encode(
                    Base64.encodeBase64String(rawHmac), "UTF-8");

            String token = String.format(TOKEN_FORMAT, targetUri, signature, this.expiryTime, this.keyName);

            return token;
        } catch (Exception e) {
            throw new RuntimeException(e);
        }
    }

    /**
     * Helper function to calculate token expiry
     *
     * @return Seconds from now to expiry
     */
    private long buildExpiresOn()
    {
        long expiresOnDate = System.currentTimeMillis();
        expiresOnDate += TOKEN_VALID_SECS * 1000;
        return expiresOnDate / 1000;
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
        return this.token;
    }
}

