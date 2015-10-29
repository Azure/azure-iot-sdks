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
     * @param iotHubConnectionString Connection string object containing the connection parameters
     */
    public IotHubServiceSasToken(IotHubConnectionString iotHubConnectionString)
    {
        // Codes_SRS_SERVICE_SDK_JAVA_IOTHUBSERVICESASTOKEN_12_001: [The constructor shall throw IllegalArgumentException if the input object is null]
        if (iotHubConnectionString == null)
        {
            throw new IllegalArgumentException();
        }
        // Codes_SRS_SERVICE_SDK_JAVA_IOTHUBSERVICESASTOKEN_12_002: [The constructor shall create a target uri from the url encoded host name)]
        // Codes_SRS_SERVICE_SDK_JAVA_IOTHUBSERVICESASTOKEN_12_003: [The constructor shall create a string to sign by concatenating the target uri and the expiry time string (one year)]
        // Codes_SRS_SERVICE_SDK_JAVA_IOTHUBSERVICESASTOKEN_12_004: [The constructor shall create a key from the shared access key signing with HmacSHA256]
        // Codes_SRS_SERVICE_SDK_JAVA_IOTHUBSERVICESASTOKEN_12_005: [The constructor shall compute the final signature by url encoding the signed key]
        // Codes_SRS_SERVICE_SDK_JAVA_IOTHUBSERVICESASTOKEN_12_006: [The constructor shall concatenate the target uri, the signature, the expiry time and the key name using the format: "SharedAccessSignature sr=%s&sig=%s&se=%s&skn=%s"]
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
            // Codes_SRS_SERVICE_SDK_JAVA_IOTHUBSERVICESASTOKEN_12_002: [The constructor shall create a target uri from the url encoded host name)]
            targetUri = URLEncoder.encode(this.resourceUri.toLowerCase(), String.valueOf(StandardCharsets.UTF_8));
            // Codes_SRS_SERVICE_SDK_JAVA_IOTHUBSERVICESASTOKEN_12_003: [The constructor shall create a string to sign by concatenating the target uri and the expiry time string (one year)]
            String toSign = targetUri + "\n" + this.expiryTime;

            // Codes_SRS_SERVICE_SDK_JAVA_IOTHUBSERVICESASTOKEN_12_004: [The constructor shall create a key from the shared access key signing with HmacSHA256]
            // Get an hmac_sha1 key from the raw key bytes
            byte[] keyBytes = Base64.decodeBase64(this.keyValue.getBytes("UTF-8"));
            SecretKeySpec signingKey = new SecretKeySpec(keyBytes, "HmacSHA256");

            // Get an hmac_sha1 Mac instance and initialize with the signing key
            Mac mac = Mac.getInstance("HmacSHA256");
            mac.init(signingKey);

            // Codes_SRS_SERVICE_SDK_JAVA_IOTHUBSERVICESASTOKEN_12_005: [The constructor shall compute the final signature by url encoding the signed key]
            // Compute the hmac on input data bytes
            byte[] rawHmac = mac.doFinal(toSign.getBytes("UTF-8"));
            // Convert raw bytes to Hex
            String signature = URLEncoder.encode(
                    Base64.encodeBase64String(rawHmac), "UTF-8");

            // Codes_SRS_SERVICE_SDK_JAVA_IOTHUBSERVICESASTOKEN_12_006: [The constructor shall concatenate the target uri, the signature, the expiry time and the key name using the format: "SharedAccessSignature sr=%s&sig=%s&se=%s&skn=%s"]
            String token = String.format(TOKEN_FORMAT, targetUri, signature, this.expiryTime, this.keyName);

            return token;
        } catch (Exception e)
        {
            // Codes_SRS_SERVICE_SDK_JAVA_IOTHUBSERVICESASTOKEN_12_007: [The constructor shall throw Exception if building the token failed]
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
     * @return The string representation of the SAS token.
     */
    @Override
    public String toString()
    {
        // Codes_SRS_SERVICE_SDK_JAVA_IOTHUBSERVICESASTOKEN_12_008: [The constructor shall return with the generated token]
        return this.token;
    }
}

