// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

package com.microsoft.azure.iothub.auth;

/**
 * A signature that is used in the SAS token to authenticate the client.
 */
public final class Signature
{
    protected String sig;

    /**
     * Constructs a {@code Signature} instance from the given resource URI,
     * expiry time and device key.
     * @param resourceUri the resource URI.
     * @param expiryTime the time, as a UNIX timestamp, after which the token
     * will become invalid.
     * @param deviceKey the device key.
     */
    public Signature(String resourceUri, long expiryTime, String deviceKey)
    {
        // Codes_SRS_SIGNATURE_11_001: [The signature shall be computed from a composition of functions as such: encodeSignatureWebSafe(encodeSignatureUtf8(encodeSignatureBase64(encryptSignatureHmacSha256(buildRawSignature(scope, expiryTime))))).]
        byte[] rawSig = SignatureHelper.buildRawSignature(resourceUri,
                expiryTime);
        // Codes_SRS_SIGNATURE_11_002: [The device key shall be decoded using Base64 before the signature computation begins, excluding buildRawSignature().]
        byte[] decodedDeviceKey = SignatureHelper.decodeDeviceKeyBase64(
                deviceKey);
        byte[] encryptedSig =
                SignatureHelper.encryptSignatureHmacSha256(rawSig,
                        decodedDeviceKey);
        byte[] encryptedSigBase64 = SignatureHelper.encodeSignatureBase64(
                encryptedSig);
        // Codes_SRS_SIGNATURE_11_003: [The signature string shall be encoded using charset UTF-8.]
        String utf8Sig = SignatureHelper.encodeSignatureUtf8(encryptedSigBase64);
        this.sig = SignatureHelper.encodeSignatureWebSafe(utf8Sig);
    }

    /**
     * Returns the string representation of the signature.
     *
     * @return the string representation of the signature.
     */
    @Override
    public String toString()
    {
        // Codes_SRS_SIGNATURE_11_005: [The function shall return the string representation of the signature.]
        return this.sig;
    }
}
