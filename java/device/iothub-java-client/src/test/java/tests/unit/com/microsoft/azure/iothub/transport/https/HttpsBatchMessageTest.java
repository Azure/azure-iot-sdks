// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

package tests.unit.com.microsoft.azure.iothub.transport.https;

import static org.hamcrest.CoreMatchers.containsString;
import static org.hamcrest.CoreMatchers.is;
import static org.hamcrest.Matchers.lessThan;
import static org.junit.Assert.assertThat;

import com.microsoft.azure.iothub.Message;
import com.microsoft.azure.iothub.MessageProperty;
import com.microsoft.azure.iothub.transport.https.HttpsSingleMessage;
import com.microsoft.azure.iothub.transport.https.HttpsBatchMessage;

import mockit.Mocked;
import mockit.NonStrictExpectations;
import org.junit.Test;

import java.nio.charset.Charset;
import java.nio.charset.StandardCharsets;

import javax.naming.SizeLimitExceededException;

/** Unit tests for HttpsBatchMessage. */
public class HttpsBatchMessageTest
{
    protected static Charset UTF8 = StandardCharsets.UTF_8;
    protected static int SERVICEBOUND_MESSAGE_MAX_SIZE_BYTES = 256 * 1024;

    // Tests_SRS_HTTPSBATCHMESSAGE_11_001: [The constructor shall initialize the batch message with the body as an empty JSON array.]
    // Tests_SRS_HTTPSBATCHMESSAGE_11_006: [The function shall return the current batch message body as a byte array.]
    // Tests_SRS_HTTPSBATCHMESSAGE_11_007: [The batch message body shall be encoded using UTF-8.]
    @Test
    public void constructorInitializesEmptyArrayBody()
    {
        HttpsBatchMessage batchMsg = new HttpsBatchMessage();
        String testBatchBody = new String(batchMsg.getBody(), UTF8);

        final String expectedBatchBody = "[]";
        assertThat(testBatchBody, is(expectedBatchBody));
    }

    // Tests_SRS_HTTPSBATCHMESSAGE_11_002: [The function shall add the message as a JSON object appended to the current JSON array.]
    // Tests_SRS_HTTPSBATCHMESSAGE_11_003: [The JSON object shall have the field "body" set to the raw message.]
    @Test
    public void addMessageSetsBodyCorrectly(
            @Mocked final HttpsSingleMessage mockMsg) throws
            SizeLimitExceededException
    {
        final String msgBody = "test-msg-body";
        final boolean isBase64Encoded = false;
        new NonStrictExpectations()
        {
            {
                mockMsg.getBodyAsString();
                result = msgBody;
                mockMsg.isBase64Encoded();
                result = isBase64Encoded;
            }
        };

        HttpsBatchMessage batchMsg = new HttpsBatchMessage();
        batchMsg.addMessage(mockMsg);
        String testBatchBody =
                new String(batchMsg.getBody(), UTF8).replaceAll("\\s", "");

        final String expectedMsgBody = "\"body\":\"" + msgBody + "\"";
        assertThat(testBatchBody, containsString(expectedMsgBody));
    }

    // Tests_SRS_HTTPSBATCHMESSAGE_11_004: [The JSON object shall have the field "base64Encoded" set to whether the raw message was Base64-encoded.]
    @Test
    public void addMessageSetsBase64Correctly(
            @Mocked final HttpsSingleMessage mockMsg) throws
            SizeLimitExceededException
    {
        final String msgBody = "test-msg-body";
        final boolean isBase64Encoded = false;
        new NonStrictExpectations()
        {
            {
                mockMsg.getBodyAsString();
                result = msgBody;
                mockMsg.isBase64Encoded();
                result = isBase64Encoded;
            }
        };

        HttpsBatchMessage batchMsg = new HttpsBatchMessage();
        batchMsg.addMessage(mockMsg);
        String testBatchBody =
                new String(batchMsg.getBody(), UTF8).replaceAll("\\s", "");

        final String expectedMsgBase64 =
                "\"base64Encoded\":" + Boolean.toString(isBase64Encoded);
        assertThat(testBatchBody, containsString(expectedMsgBase64));
    }

    // Tests_SRS_HTTPSBATCHMESSAGE_11_005: [The JSON object shall have the field "properties" set to a JSON object which has a key-value pair for each message property, where the key is the HTTPS property name and the value is the property value.]
    @Test
    public void addMessageSetsPropertiesCorrectly(
            @Mocked final HttpsSingleMessage mockMsg,
            @Mocked final MessageProperty mockProperty) throws
            SizeLimitExceededException
    {
        final String msgBody = "test-msg-body";
        final boolean isBase64Encoded = false;
        final String propertyHttpsName = "test-property-name";
        final String propertyValue = "test-property-value";
        final MessageProperty[] properties = { mockProperty };
        new NonStrictExpectations()
        {
            {
                mockMsg.getBodyAsString();
                result = msgBody;
                mockMsg.getProperties();
                result = properties;
                mockProperty.getName();
                result = propertyHttpsName;
                mockProperty.getValue();
                result = propertyValue;
                mockMsg.isBase64Encoded();
                result = isBase64Encoded;
            }
        };

        HttpsBatchMessage batchMsg = new HttpsBatchMessage();
        batchMsg.addMessage(mockMsg);
        String testBatchBody =
                new String(batchMsg.getBody(), UTF8).replaceAll("\\s", "");

        final String expectedMsgProperties =
                "\"properties\":{\""
                        + propertyHttpsName + "\":\"" + propertyValue
                        + "\"}";
        assertThat(testBatchBody, containsString(expectedMsgProperties));
    }

    // Tests_SRS_HTTPSBATCHMESSAGE_11_009: [If the function throws a SizeLimitExceededException, the batched message shall remain as if the message was never added.]
    @Test
    public void addMessageRejectsOverflowingMessageAndPreservesOldBatchState(
            @Mocked final HttpsSingleMessage mockMsg) throws
            SizeLimitExceededException
    {
        final int msgBodySize = SERVICEBOUND_MESSAGE_MAX_SIZE_BYTES / 2 + 1;
        final byte[] msgBodyBytes = new byte[msgBodySize];
        final String msgBody = new String(msgBodyBytes, UTF8);
        final boolean isBase64Encoded = false;
        new NonStrictExpectations()
        {
            {
                mockMsg.getBodyAsString();
                result = msgBody;
                mockMsg.isBase64Encoded();
                result = isBase64Encoded;
            }
        };

        HttpsBatchMessage batchMsg = new HttpsBatchMessage();
        batchMsg.addMessage(mockMsg);
        try
        {
            batchMsg.addMessage(mockMsg);
        }
        catch (SizeLimitExceededException e)
        {
            final int expectedTwoMsgBodySize = 2 * msgBodySize;
            assertThat(batchMsg.getBody().length,
                    is(lessThan(expectedTwoMsgBodySize)));
        }
    }

    // Tests_SRS_HTTPSBATCHMESSAGE_11_011: [The function shall return 'application/vnd.microsoft.iothub.json'.]
    @Test
    public void getContentTypeReturnsCorrectContentType()
    {
        HttpsBatchMessage batchMsg = new HttpsBatchMessage();
        String testContentType = batchMsg.getContentType();

        final String expectedContentType =
                "application/vnd.microsoft.iothub.json";
        assertThat(testContentType, is(expectedContentType));
    }

    // Tests_SRS_HTTPSBATCHMESSAGE_11_012: [The function shall return an empty array.]
    @Test
    public void getPropertiesReturnsNoProperties()
    {
        HttpsBatchMessage batchMsg = new HttpsBatchMessage();
        MessageProperty[] testProperties = batchMsg.getProperties();

        MessageProperty[] expectedProperties =
                new MessageProperty[0];
        assertThat(testProperties, is(expectedProperties));
    }

    // Tests_SRS_HTTPSBATCHMESSAGE_11_010: [The function shall return the number of messages currently in the batch.]
    @Test
    public void numMessagesInitializedCorrectly(
            @Mocked final HttpsSingleMessage mockMsg) throws
            SizeLimitExceededException
    {
        HttpsBatchMessage batchMsg = new HttpsBatchMessage();
        int testNumMessages = batchMsg.numMessages();

        final int expectedNumMessages = 0;
        assertThat(testNumMessages, is(expectedNumMessages));
    }

    // Tests_SRS_HTTPSBATCHMESSAGE_11_010: [The function shall return the number of messages currently in the batch.]
    @Test
    public void numMessagesIncrementedCorrectly(
            @Mocked final HttpsSingleMessage mockMsg) throws
            SizeLimitExceededException
    {
        final String msgBody = "test-msg-body";
        final boolean isBase64Encoded = false;
        new NonStrictExpectations()
        {
            {
                mockMsg.getBodyAsString();
                result = msgBody;
                mockMsg.isBase64Encoded();
                result = isBase64Encoded;
            }
        };

        HttpsBatchMessage batchMsg = new HttpsBatchMessage();
        batchMsg.addMessage(mockMsg);
        batchMsg.addMessage(mockMsg);
        batchMsg.addMessage(mockMsg);
        int testNumMessages = batchMsg.numMessages();

        final int expectedNumMessages = 3;
        assertThat(testNumMessages, is(expectedNumMessages));
    }

    // Tests_SRS_HTTPSBATCHMESSAGE_11_008: [If adding the message causes the batched message to exceed 256 kb in size, the function shall throw a SizeLimitExceededException.]
    // Tests_SRS_HTTPSBATCHMESSAGE_11_009: [If the function throws a SizeLimitExceedException, the batched message shall remain as if the message was never added.]
    @Test
    public void testAddMessage(
            @Mocked final HttpsSingleMessage mockMsg) throws
            SizeLimitExceededException {

        // Note: this will currently result on a message size of 261154 bytes, considering the extra attributes contained on the json-serialized message.
        // Note: so the current body size limit alone actually is (255 * 1024  - 36) bytes.
        final byte[] validSizeBody = new byte[255 * 1024 - 1];

        new NonStrictExpectations()
        {
            {
                mockMsg.getBodyAsString();
                result = new String(validSizeBody, Message.DEFAULT_IOTHUB_MESSAGE_CHARSET);
            }
        };

        boolean httpsBatchMessageSizeLimitVerified = false;

        try {
            HttpsBatchMessage batchMsg = new HttpsBatchMessage();
            batchMsg.addMessage(mockMsg);
        } catch (SizeLimitExceededException ex) {
            httpsBatchMessageSizeLimitVerified = true;
        }

        assertThat(httpsBatchMessageSizeLimitVerified, is(true));
    }
}
