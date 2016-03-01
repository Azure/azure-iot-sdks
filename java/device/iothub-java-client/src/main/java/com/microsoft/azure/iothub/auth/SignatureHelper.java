// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

package com.microsoft.azure.iothub.auth;

import java.io.UnsupportedEncodingException;
import java.net.URLEncoder;
import java.nio.charset.Charset;
import java.nio.charset.StandardCharsets;
import java.security.InvalidKeyException;
import java.security.NoSuchAlgorithmException;

import javax.crypto.Mac;
import javax.crypto.spec.SecretKeySpec;
import javax.xml.bind.DatatypeConverter;

/** Builds the authorization signature as a composition of functions. */
public final class SignatureHelper
{
    /**
     * The device ID will be the prefix. The expiry time, as a UNIX
     * timestamp, will be the suffix.
     */
    public static final String RAW_SIGNATURE_FORMAT = "%s\n%s";

    /** The charset used for the raw and hashed signature. */
    public static final Charset SIGNATURE_CHARSET = StandardCharsets.UTF_8;

    /**
     * Builds the raw signature.
     *
     * @param resourceUri the resource URI.
     * @param expiryTime the signature expiry time, as a UNIX timestamp.
     *
     * @return the raw signature.
     */
    public static byte[] buildRawSignature(String resourceUri, long expiryTime)
    {
        // Codes_SRS_SIGNATUREHELPER_11_001: [The function shall initialize the message being encoded as "<scope>\n<expiryTime>".]
        // Codes_SRS_SIGNATUREHELPER_11_002: [The function shall decode the message using the charset UTF-8.]
        return String.format(RAW_SIGNATURE_FORMAT, resourceUri, expiryTime)
                .getBytes(SIGNATURE_CHARSET);
    }

    /**
     * Decodes the deviceKey using Base64.
     *
     * @param deviceKey the device key.
     *
     * @return the Base64-decoded device key.
     */
    public static byte[] decodeDeviceKeyBase64(String deviceKey)
    {
        // Codes_SRS_SIGNATUREHELPER_11_003: [The function shall decode the device key using Base64.]
        return DatatypeConverter.parseBase64Binary(deviceKey);
    }

    /**
     * Encrypts the signature using HMAC-SHA256.
     *
     * @param sig the unencrypted signature.
     * @param deviceKey the Base64-decoded device key.
     *
     * @return the HMAC-SHA256 encrypted signature.
     */
    public static byte[] encryptSignatureHmacSha256(byte[] sig,
            byte[] deviceKey)
    {
        String hmacSha256 = "HmacSHA256";

        // Codes_SRS_SIGNATUREHELPER_11_005: [The function shall use the device key as the secret for the algorithm.]
        SecretKeySpec secretKey = new SecretKeySpec(deviceKey, hmacSha256);

        byte[] encryptedSig = null;
        try
        {
            // Codes_SRS_SIGNATUREHELPER_11_004: [The function shall encrypt the signature using the HMAC-SHA256 algorithm.]
            Mac hMacSha256 = Mac.getInstance(hmacSha256);
            hMacSha256.init(secretKey);
            encryptedSig = hMacSha256.doFinal(sig);
        }
        catch (NoSuchAlgorithmException e)
        {
            // should never happen, since the algorithm is hard-coded.
        }
        catch (InvalidKeyException e)
        {
            // should never happen, since the input key type is hard-coded.
        }

        return encryptedSig;
    }

    /**
     * Encodes the signature using Base64 and then further
     * encodes the resulting string using UTF-8 encoding.
     *
     * @param sig the HMAC-SHA256 encrypted signature.
     *
     * @return the Base64-encoded signature.
     */
    public static byte[] encodeSignatureBase64(byte[] sig)
    {
        // Codes_SRS_SIGNATUREHELPER_11_006: [The function shall encode the signature using Base64.]
        return DatatypeConverter.printBase64Binary(sig)
                .getBytes(SIGNATURE_CHARSET);
    }

    /**
     * Encodes the signature using charset UTF-8.
     *
     * @param sig the HMAC-SHA256 encrypted, Base64-encoded signature.
     *
     * @return the signature encoded using charset UTF-8.
     */
    public static String encodeSignatureUtf8(byte[] sig)
    {
        // Codes_SRS_SIGNATUREHELPER_11_010: [The function shall encode the signature using charset UTF-8.]
        return new String(sig, SIGNATURE_CHARSET);
    }


    /**
     * Safely escapes characters in the signature so that they can be
     * transmitted over the internet. Replaces unsafe characters with a '%'
     * followed by two hexadecimal digits (i.e. %2d).
     *
     * @param sig the HMAC-SHA256 encrypted, Base64-encoded, UTF-8 encoded
     * signature.
     *
     * @return the web-safe encoding of the signature.
     */
    public static String encodeSignatureWebSafe(String sig)
    {
        String strSig = "";
        try
        {
            // Codes_SRS_SIGNATUREHELPER_11_007: [The function shall replace web-unsafe characters in the signature with a '%' followed by two hexadecimal digits, where the hexadecimal digits are determined by the UTF-8 charset.]
            // Codes_SRS_SIGNATUREHELPER_11_008: [The function shall replace spaces with '+' signs.]
            strSig = URLEncoder.encode(sig, SIGNATURE_CHARSET.name());
        }
        catch (UnsupportedEncodingException e)
        {
            // should never happen, since the encoding is hard-coded.
            throw new IllegalStateException(e);
        }

        return strSig;
    }

    protected SignatureHelper()
    {
    }
}
