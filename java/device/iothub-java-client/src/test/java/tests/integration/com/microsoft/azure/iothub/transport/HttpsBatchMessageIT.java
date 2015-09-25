// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

package tests.integration.com.microsoft.azure.iothub.transport;

import static org.hamcrest.CoreMatchers.is;
import static org.junit.Assert.assertThat;

import com.microsoft.azure.iothub.Message;
import com.microsoft.azure.iothub.transport.https.HttpsSingleMessage;
import com.microsoft.azure.iothub.transport.https.HttpsBatchMessage;

import org.junit.Test;

import java.nio.charset.Charset;
import java.nio.charset.StandardCharsets;

import javax.naming.SizeLimitExceededException;

/** Integration tests for HttpsBatchMessage. */
public class HttpsBatchMessageIT
{
    protected Charset UTF8 = StandardCharsets.UTF_8;

    @Test
    public void batchMessageSetsFieldsCorrectly() throws
            SizeLimitExceededException
    {
        String msgBytes0 = "abc";
        Message msg0 = new Message(msgBytes0);
        msg0.setProperty("prop-0", "value-0");
        HttpsSingleMessage httpsMsg0 = HttpsSingleMessage.parseHttpsMessage(msg0);

        byte[] msgBytes1 = { 48, 49, 50 };
        Message msg1 = new Message(msgBytes1);
        msg1.setProperty("prop-1", "value-1");
        HttpsSingleMessage httpsMsg1 = HttpsSingleMessage.parseHttpsMessage(msg1);
        HttpsBatchMessage batch = new HttpsBatchMessage();
        batch.addMessage(httpsMsg0);
        batch.addMessage(httpsMsg1);

        // JSON body with whitespace removed.
        String testBatchBody = new String(batch.getBody(), UTF8).replaceAll("\\s", "");

        String expectedBatchBody = "["
                + "{\"body\":\"abc\","
                + "\"base64Encoded\":false,"
                + "\"properties\":{\"iothub-app-prop-0\":\"value-0\"}},"
                + "{\"body\":\"012\","
                + "\"base64Encoded\":false,"
                + "\"properties\":{\"iothub-app-prop-1\":\"value-1\"}}"
                + "]";


        assertThat(testBatchBody, is(expectedBatchBody));
    }
}
