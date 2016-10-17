// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

package tests.integration.com.microsoft.azure.iothub.transport.https;

import static org.hamcrest.CoreMatchers.is;
import static org.hamcrest.Matchers.startsWith;
import static org.junit.Assert.assertThat;

import com.microsoft.azure.iothub.transport.https.HttpsMethod;
import com.microsoft.azure.iothub.transport.https.HttpsRequest;

import com.microsoft.azure.iothub.transport.https.HttpsResponse;
import org.junit.Test;

import java.io.IOException;
import java.net.URL;

/** Integration tests for HttpsRequest. */
public class HttpsRequestIT
{
    @Test
    public void sendHttpsRequestGetsCorrectResponse() throws IOException
    {
        URL url = new URL("https://fonts.googleapis.com/css?family=Inconsolata");
        HttpsMethod method = HttpsMethod.GET;
        byte[] body = new byte[0];
        String encoding = "UTF-8";
        HttpsRequest request = new HttpsRequest(url, method, body);
        request.setHeaderField("accept-charset", encoding);

        HttpsResponse response = request.send();
        int testStatus = response.getStatus();
        String testBody = new String(response.getBody(), encoding);

        int expectedStatus = 200;
        String expectedBodyPrefix = "@font-face {\n"
                + "  font-family: 'Inconsolata';";
        assertThat(testStatus, is(expectedStatus));
        assertThat(testBody, is(startsWith(expectedBodyPrefix)));
    }
}
