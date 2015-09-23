// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

package tests.integration.com.microsoft.azure.iothub.net;

import static org.hamcrest.CoreMatchers.is;
import static org.hamcrest.CoreMatchers.anyOf;
import static org.hamcrest.CoreMatchers.equalTo;
import static org.junit.Assert.assertThat;

import com.microsoft.azure.iothub.auth.Signature;
import org.junit.Test;

/** Integration tests for Signature. */
public class SignatureIT
{
    @Test
    public void signatureIsCorrect()
    {
        String resourceUri =
                "sdktesthub.private.azure-devices-int.net/devices/test8";
        String deviceKey =
                "q+DKVojmMz014RjAwpNGZbAkdOocSVWMKPOccLfUmLE=";
        long expiryTime = 1462333672;

        String testSig =
                new Signature(resourceUri, expiryTime, deviceKey).toString();

        String expectedSigUpperHex =
                "ZrTYgp6uRmbDl4lzzewxFD9FoaNKIK7G2bdivDbtc9A%3D";
        String expectedSigLowerHex =
                "ZrTYgp6uRmbDl4lzzewxFD9FoaNKIK7G2bdivDbtc9A%3d";
        assertThat(testSig, is(anyOf(equalTo(expectedSigUpperHex),
                equalTo(expectedSigLowerHex))));
    }
}
