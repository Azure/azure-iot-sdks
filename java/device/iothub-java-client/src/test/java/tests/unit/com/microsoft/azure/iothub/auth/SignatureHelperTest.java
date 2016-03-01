// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

package tests.unit.com.microsoft.azure.iothub.auth;

import static org.junit.Assert.assertThat;
import static org.hamcrest.CoreMatchers.is;
import static org.hamcrest.CoreMatchers.anyOf;
import static org.hamcrest.CoreMatchers.equalTo;

import com.microsoft.azure.iothub.auth.SignatureHelper;

import org.junit.Test;

import java.nio.charset.Charset;
import java.nio.charset.StandardCharsets;

/** Unit tests for SignatureHelper. */
public class SignatureHelperTest
{
    protected static Charset UTF8 = StandardCharsets.UTF_8;

    // Tests_SRS_SIGNATUREHELPER_11_001: [The function shall initialize the message being encoded as "<scope>\n<expiryTime>".]
    // Tests_SRS_SIGNATUREHELPER_11_002: [The function shall decode the message using the charset UTF-8.]
    @Test
    public void rawSignatureHasCorrectFormat()
    {
        final String resourceUri = "test-resource-uri";
        final long expiryTime = 101l;

        byte[] testRawSig =
                SignatureHelper.buildRawSignature(resourceUri, expiryTime);

        final byte[] expectedRawSig = "test-resource-uri\n101".getBytes(UTF8);
        assertThat(testRawSig, is(expectedRawSig));
    }

    // Tests_SRS_SIGNATUREHELPER_11_003: [The function shall decode the device key using Base64.]
    @Test
    public void deviceKeyDecodedUsingBase64()
    {
        final String deviceKey = "Zm9vYmFy";

        byte[] testDecodedDeviceKey =
                SignatureHelper.decodeDeviceKeyBase64(deviceKey);

        final byte[] expectedDecodedDeviceKey =
                { 0x66, 0x6F, 0x6F, 0x62, 0x61, 0x72 };
        assertThat(testDecodedDeviceKey, is(expectedDecodedDeviceKey));
    }

    // Tests_SRS_SIGNATUREHELPER_11_004: [The function shall encrypt the signature using the HMAC-SHA256 algorithm.]
    // Tests_SRS_SIGNATUREHELPER_11_005: [The function shall use the device key as the secret for the algorithm.]
    @Test
    public void encryptSignatureUsingHmacSha256()
    {
        final byte[] testSig = "what do ya want for nothing?".getBytes(UTF8);
        final byte[] deviceKey = "Jefe".getBytes(UTF8);

        byte[] testEncryptedSig =
                SignatureHelper.encryptSignatureHmacSha256(testSig, deviceKey);

        final byte[] expectedEncryptedSig =
                { 0x5b, (byte) 0xdc, (byte) 0xc1, 0x46, (byte) 0xbf, 0x60, 0x75,
                        0x4e, 0x6a, 0x04, 0x24, 0x26, 0x08, (byte) 0x95, 0x75,
                        (byte) 0xc7, 0x5a, 0x00, 0x3f, 0x08, (byte) 0x9d, 0x27,
                        0x39, (byte) 0x83, (byte) 0x9d, (byte) 0xec, 0x58,
                        (byte) 0xb9, 0x64, (byte) 0xec, 0x38, 0x43 };
        assertThat(testEncryptedSig, is(expectedEncryptedSig));
    }

    // Tests_SRS_SIGNATUREHELPER_11_006: [The function shall encode the signature using Base64.]
    @Test
    public void encodeSignatureUsingBase64()
    {
        final byte[] sig = "foobar".getBytes(UTF8);

        byte[] testBase64Sig =
                SignatureHelper.encodeSignatureBase64(sig);

        final byte[] expectedBase64Sig =
                { 0x5A, 0x6D, 0x39, 0x76, 0x59, 0x6D, 0x46, 0x79 };
        assertThat(testBase64Sig, is(expectedBase64Sig));
    }

    // Tests_SRS_SIGNATUREHELPER_11_010: [The function shall encode the signature using charset UTF-8.]
    @Test
    public void encodeSignatureUsingUtf8()
    {
        final byte[] sig = { 0x61, 0x62, 0x63 };

        String testUtf8Sig = SignatureHelper.encodeSignatureUtf8(sig);

        String expectedUtf8Sig = "abc";
        assertThat(testUtf8Sig, is(expectedUtf8Sig));
    }

    // Tests_SRS_SIGNATUREHELPER_11_007: [The function shall replace web-unsafe characters in the signature with a '%' followed by two hexadecimal digits, where the hexadecimal digits are determined by the UTF-8 charset.]
    @Test
    public void encodeSignatureEscapesWithUtf8Charset()
    {
        final String sig = "üm@?";

        String testWebSafeSig = SignatureHelper.encodeSignatureWebSafe(sig);

        final String expectedSigUpperHex = "%C3%BCm%40%3F";
        final String expectedSigLowerHex = "%c3%BCm%40%3f";
        assertThat(testWebSafeSig, is(anyOf(equalTo(expectedSigUpperHex),
                equalTo(expectedSigLowerHex))));
    }

    // Tests_SRS_SIGNATUREHELPER_11_008: [The function shall replace spaces with '+' signs.]
    @Test
    public void encodeSignatureReplacesSpaceWithPlus()
    {
        final String sig = "m m";

        String testWebSafeSig =
                SignatureHelper.encodeSignatureWebSafe(sig);

        final String expectedWebSafeSig = "m+m";
        assertThat(testWebSafeSig, is(expectedWebSafeSig));
    }
}
