// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

package tests.unit.com.microsoft.azure.iothub.auth;

import static org.junit.Assert.assertThat;
import static org.hamcrest.CoreMatchers.is;

import com.microsoft.azure.iothub.auth.SignatureHelper;
import mockit.Expectations;
import mockit.Mocked;
import mockit.VerificationsInOrder;
import org.junit.Test;

import com.microsoft.azure.iothub.auth.Signature;

/** Unit tests for Signature. */
public class SignatureTest
{
    @Mocked SignatureHelper mockSigHelper;

    // Tests_SRS_SIGNATURE_11_001: [The signature shall be computed from a composition of functions as such: convertSignatureHexDigitsToLowercase(encodeSignatureWebSafe(encodeSignatureUtf8(encodeSignatureBase64(encryptSignatureHmacSha256(buildRawSignature(resourceUri, expiryTime)))))).]
    @Test
    public void signatureComputedInOrder()
    {
        final String resourceUri = "test-resource-uri";
        final String deviceKey = "test-device-key";
        final long expiryTime = 101l;

        new Signature(resourceUri, expiryTime, deviceKey);

        new VerificationsInOrder()
        {
            {
                SignatureHelper.buildRawSignature(anyString, anyLong);
                SignatureHelper
                        .encryptSignatureHmacSha256((byte[]) any, (byte[]) any);
                SignatureHelper.encodeSignatureBase64((byte[]) any);
                SignatureHelper.encodeSignatureUtf8((byte[]) any);
                SignatureHelper.encodeSignatureWebSafe(anyString);
            }
        };
    }

    // Tests_SRS_SIGNATURE_11_002: [The device key shall be decoded using Base64 before the signature computation begins, excluding buildRawSignature().]
    // Tests_SRS_SIGNATURE_11_003: [The signature string shall be encoded using charset UTF-8.]
    @Test
    public void deviceKeyDecodedBeforeSignatureComputation()
    {
        final String resourceUri = "test-resource-uri";
        final String deviceKey = "test-device-key";
        final long expiryTime = 101l;

        new Signature(resourceUri, expiryTime, deviceKey);

        new VerificationsInOrder()
        {
            {
                SignatureHelper.decodeDeviceKeyBase64(anyString);
                SignatureHelper
                        .encryptSignatureHmacSha256((byte[]) any, (byte[]) any);
                SignatureHelper.encodeSignatureBase64((byte[]) any);
                SignatureHelper.encodeSignatureUtf8((byte[]) any);
                SignatureHelper.encodeSignatureWebSafe(anyString);
            }
        };
    }

    // Tests_SRS_SIGNATURE_11_005: [The function shall return the string representation of the signature.]
    @Test
    public void toStringReturnsCorrectString()
    {
        final String resourceUri = "test-resource-uri";
        final String deviceKey = "test-device-key";
        final long expiryTime = 101l;
        final String sigStr = "test-signature";
        new Expectations()
        {
            {
                // this should be the last step of the signature computation.
                SignatureHelper
                        .encodeSignatureWebSafe(anyString);
                result = sigStr;
            }
        };

        Signature sig =
                new Signature(resourceUri, expiryTime, deviceKey);
        String testSigStr = sig.toString();

        final String expectedSigStr = sigStr;
        assertThat(testSigStr, is(expectedSigStr));
    }
}
