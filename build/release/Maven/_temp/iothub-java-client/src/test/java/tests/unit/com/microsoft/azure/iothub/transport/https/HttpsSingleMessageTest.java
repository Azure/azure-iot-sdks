// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

package tests.unit.com.microsoft.azure.iothub.transport.https;

import static org.hamcrest.CoreMatchers.is;
import static org.hamcrest.CoreMatchers.not;
import static org.junit.Assert.assertThat;

import com.microsoft.azure.iothub.Message;
import com.microsoft.azure.iothub.MessageProperty;
import com.microsoft.azure.iothub.transport.https.HttpsResponse;
import com.microsoft.azure.iothub.transport.https.HttpsSingleMessage;

import mockit.Mocked;
import mockit.NonStrictExpectations;
import mockit.Verifications;
import org.junit.Test;

import java.util.HashMap;
import java.util.Map;

/** Unit tests for HttpsSingleMessage. */
public class HttpsSingleMessageTest
{
    // Tests_SRS_HTTPSSINGLEMESSAGE_11_001: [The parsed HttpsSingleMessage shall have a copy of the original message body as its body.]
    @Test
    public void parseHttpsMessageFromMessageCopiesBody(
            @Mocked final Message mockMsg,
            @Mocked final MessageProperty mockProperty)
    {
        final byte[] body = { 0x61, 0x62, 0x63 };
        final MessageProperty[] properties = { mockProperty };
        final String propertyName = "test-property-name";
        final String propertyValue = "test-property-value";
        new NonStrictExpectations()
        {
            {
                mockMsg.getBytes();
                result = body;
                mockMsg.getProperties();
                result = properties;
                mockProperty.getName();
                result = propertyName;
                mockProperty.getValue();
                result = propertyValue;
            }
        };

        HttpsSingleMessage httpsMsg =
                HttpsSingleMessage.parseHttpsMessage(mockMsg);
        byte[] testBody = httpsMsg.getBody();

        byte[] expectedBody = body;
        assertThat(testBody, is(expectedBody));

        expectedBody[0] = 0x34;
        assertThat(testBody, is(not(expectedBody)));
    }

    // Tests_SRS_HTTPSSINGLEMESSAGE_11_003: [The parsed HttpsSingleMessage shall add the prefix 'iothub-app-' to each of the message properties.]
    @Test
    public void parseHttpsMessageFromMessageSavesPropertiesWithPrefix(
            @Mocked final Message mockMsg,
            @Mocked final MessageProperty mockProperty)
    {
        final byte[] body = { 0x61, 0x62, 0x63 };
        final MessageProperty[] properties = { mockProperty };
        final String propertyName = "test-property-name";
        final String propertyValue = "test-property-value";
        new NonStrictExpectations()
        {
            {
                mockMsg.getBytes();
                result = body;
                mockMsg.getProperties();
                result = properties;
                mockProperty.getName();
                result = propertyName;
                mockProperty.getValue();
                result = propertyValue;
            }
        };

        HttpsSingleMessage.parseHttpsMessage(mockMsg);

        final String expectedPrefix = "iothub-app-";
        final String expectedPropertyName = expectedPrefix + propertyName;
        final String expectedPropertyValue = propertyValue;
        new Verifications()
        {
            {
                new MessageProperty(expectedPropertyName,
                        expectedPropertyValue);
            }
        };
    }

    // Tests_SRS_HTTPSSINGLEMESSAGE_11_004: [The parsed HttpsSingleMessage shall have a copy of the original response body as its body.]
    @Test
    public void parseHttpsMessageFromResponseCopiesBody(
            @Mocked final HttpsResponse mockResponse,
            @Mocked final MessageProperty mockProperty)
    {
        final byte[] body = { 0x61, 0x62, 0x63 };
        final Map<String, String> headerFields = new HashMap<>();
        final String propertyName = "test-property-name";
        final String propertyValue = "test-property-value";
        headerFields.put(propertyName, propertyValue);
        new NonStrictExpectations()
        {
            {
                mockResponse.getBody();
                result = body;
                mockResponse.getHeaderFields();
                result = headerFields;
            }
        };

        HttpsSingleMessage httpsMsg =
                HttpsSingleMessage.parseHttpsMessage(mockResponse);
        byte[] testBody = httpsMsg.getBody();

        byte[] expectedBody = body;
        assertThat(testBody, is(expectedBody));

        expectedBody[0] = 0x34;
        assertThat(testBody, is(not(expectedBody)));
    }

    // Tests_SRS_HTTPSSINGLEMESSAGE_11_005: [The parsed HttpsSingleMessage shall not be Base64-encoded.]
    @Test
    public void parseHttpsMessageFromResponseDoesNotBase64EncodeBody(
            @Mocked final HttpsResponse mockResponse,
            @Mocked final MessageProperty mockProperty)
    {
        final byte[] body = { 0x61, 0x62, 0x63 };
        final Map<String, String> headerFields = new HashMap<>();
        final String propertyName = "test-property-name";
        final String propertyValue = "test-property-value";
        headerFields.put(propertyName, propertyValue);
        new NonStrictExpectations()
        {
            {
                mockResponse.getBody();
                result = body;
                mockResponse.getHeaderFields();
                result = headerFields;
            }
        };

        HttpsSingleMessage httpsMsg =
                HttpsSingleMessage.parseHttpsMessage(mockResponse);
        boolean testBase64Encoded = httpsMsg.isBase64Encoded();

        boolean expectedBase64Encoded = false;
        assertThat(testBase64Encoded, is(expectedBase64Encoded));
    }

    // Tests_SRS_HTTPSSINGLEMESSAGE_11_006: [The parsed HttpsSingleMessage shall include all valid HTTPS application-defined properties in the response header as message properties.]
    @Test
    public void parseHttpsMessageFromResponseDoesNotIncludeNonAppProperties(
            @Mocked final HttpsResponse mockResponse,
            @Mocked final MessageProperty mockProperty)
    {
        final byte[] body = { 0x61, 0x62, 0x63 };
        final Map<String, String> headerFields = new HashMap<>();
        final String propertyName = "test-property-name";
        final String propertyValue = "test-property-value";
        headerFields.put(propertyName, propertyValue);
        new NonStrictExpectations()
        {
            {
                mockResponse.getBody();
                result = body;
                mockResponse.getHeaderFields();
                result = headerFields;
            }
        };

        HttpsSingleMessage httpsMsg =
                HttpsSingleMessage.parseHttpsMessage(mockResponse);
        MessageProperty[] testProperties = httpsMsg.getProperties();

        MessageProperty[] expectedProperties = {};
        assertThat(testProperties, is(expectedProperties));
    }

    // Tests_SRS_HTTPSSINGLEMESSAGE_11_006: [The parsed HttpsSingleMessage shall include all valid HTTPS application-defined properties in the response header as message properties.]
    @Test
    public void parseHttpsMessageFromResponseIncludesAppProperties(
            @Mocked final HttpsResponse mockResponse,
            @Mocked final MessageProperty mockProperty)
    {
        final byte[] body = { 0x61, 0x62, 0x63 };
        final Map<String, String> headerFields = new HashMap<>();
        final String propertyName = "iothub-app-test-property-name";
        final String propertyValue = "test-property-value";
        headerFields.put(propertyName, propertyValue);
        new NonStrictExpectations()
        {
            {
                mockResponse.getBody();
                result = body;
                mockResponse.getHeaderFields();
                result = headerFields;
                MessageProperty.isValidAppProperty(
                        propertyName, propertyValue);
                result = true;
            }
        };

        HttpsSingleMessage.parseHttpsMessage(mockResponse);

        new Verifications()
        {
            {
                new MessageProperty(propertyName, propertyValue);
            }
        };
    }

    // Tests_SRS_HTTPSSINGLEMESSAGE_11_007: [The function shall return an IoT Hub message with a copy of the message body as its body.]
    @Test
    public void toMessageCopiesBody(@Mocked final HttpsResponse mockResponse,
            @Mocked final MessageProperty mockProperty,
            @Mocked final Message mockMsg)
    {
        final byte[] body = { 0x61, 0x62, 0x63 };
        final Map<String, String> headerFields = new HashMap<>();
        final String propertyName = "iothub-app-test-property-name";
        final String propertyValue = "test-property-value";
        headerFields.put(propertyName, propertyValue);
        new NonStrictExpectations()
        {
            {
                mockResponse.getBody();
                result = body;
                mockResponse.getHeaderFields();
                result = headerFields;
                MessageProperty.isValidAppProperty(
                        propertyName, propertyValue);
                result = true;
                new MessageProperty(propertyName, propertyValue);
                result = mockProperty;
                mockProperty.getName();
                result = propertyName;
                mockProperty.getValue();
                result = propertyValue;
                new Message(body);
                result = mockMsg;
            }
        };

        HttpsSingleMessage httpsMsg =
                HttpsSingleMessage.parseHttpsMessage(mockResponse);
        httpsMsg.toMessage();

        final byte[] expectedBody = body;
        new Verifications()
        {
            {
                new Message(expectedBody);
            }
        };
    }

    // Tests_SRS_HTTPSSINGLEMESSAGE_11_008: [The function shall return an IoT Hub message with application-defined properties that have the prefix 'iothub-app' removed.]
    @Test
    public void toMessageRemovesPrefixFromProperties(
            @Mocked final HttpsResponse mockResponse,
            @Mocked final MessageProperty mockProperty,
            @Mocked final Message mockMsg)
    {
        final byte[] body = { 0x61, 0x62, 0x63 };
        final Map<String, String> headerFields = new HashMap<>();
        final String propertyName = "iothub-app-test-property-name";
        final String propertyValue = "test-property-value";
        headerFields.put(propertyName, propertyValue);
        new NonStrictExpectations()
        {
            {
                mockResponse.getBody();
                result = body;
                mockResponse.getHeaderFields();
                result = headerFields;
                MessageProperty.isValidAppProperty(
                        propertyName, propertyValue);
                result = true;
                new MessageProperty(propertyName, propertyValue);
                result = mockProperty;
                mockProperty.getName();
                result = propertyName;
                mockProperty.getValue();
                result = propertyValue;
                new Message(body);
                result = mockMsg;
            }
        };

        HttpsSingleMessage httpsMsg =
                HttpsSingleMessage.parseHttpsMessage(mockResponse);
        httpsMsg.toMessage();

        final String expectedPropertyName = "test-property-name";
        final String expectedPropertyValue = propertyValue;
        new Verifications()
        {
            {
                mockMsg.setProperty(expectedPropertyName,
                        expectedPropertyValue);
            }
        };
    }

    // Tests_SRS_HTTPSSINGLEMESSAGE_11_009: [The function shall return a copy of the message body.]
    @Test
    public void getBodyReturnsCopyOfBody(
            @Mocked final Message mockMsg,
            @Mocked final MessageProperty mockProperty)
    {
        final byte[] body = { 0x61, 0x62, 0x63 };
        final MessageProperty[] properties = { mockProperty };
        final String propertyName = "test-property-name";
        final String propertyValue = "test-property-value";
        new NonStrictExpectations()
        {
            {
                mockMsg.getBytes();
                result = body;
                mockMsg.getProperties();
                result = properties;
                mockProperty.getName();
                result = propertyName;
                mockProperty.getValue();
                result = propertyValue;
            }
        };

        HttpsSingleMessage httpsMsg =
                HttpsSingleMessage.parseHttpsMessage(mockMsg);
        byte[] testBody = httpsMsg.getBody();

        byte[] expectedBody = body;
        assertThat(testBody, is(expectedBody));

        testBody[0] = 0x34;
        testBody = httpsMsg.getBody();
        assertThat(testBody, is(expectedBody));
    }

    // Tests_SRS_HTTPSSINGLEMESSAGE_11_010: [The function shall return the message body as a string encoded using charset UTF-8.]
    @Test
    public void getBodyAsStringsReturnsUtf8Body(
            @Mocked final Message mockMsg,
            @Mocked final MessageProperty mockProperty)
    {
        final byte[] body = { 0x61, 0x62, 0x63 };
        final MessageProperty[] properties = { mockProperty };
        final String propertyName = "test-property-name";
        final String propertyValue = "test-property-value";
        new NonStrictExpectations()
        {
            {
                mockMsg.getBytes();
                result = body;
                mockMsg.getProperties();
                result = properties;
                mockProperty.getName();
                result = propertyName;
                mockProperty.getValue();
                result = propertyValue;
            }
        };

        HttpsSingleMessage httpsMsg =
                HttpsSingleMessage.parseHttpsMessage(mockMsg);
        String testBody = httpsMsg.getBodyAsString();

        String expectedBody = "abc";
        assertThat(testBody, is(expectedBody));
    }

    // Tests_SRS_HTTPSSINGLEMESSAGE_11_011: [The function shall return the message content-type as 'binary/octet-stream'.]
    @Test
    public void getContentTypeReturnsCorrectContentType(
            @Mocked final Message mockMsg,
            @Mocked final MessageProperty mockProperty)
    {
        final byte[] body = { 0x61, 0x62, 0x63 };
        final boolean base64Encoded = false;
        final MessageProperty[] properties = { mockProperty };
        final String propertyName = "test-property-name";
        final String propertyValue = "test-property-value";
        new NonStrictExpectations()
        {
            {
                mockMsg.getBytes();
                result = body;
                mockMsg.getProperties();
                result = properties;
                mockProperty.getName();
                result = propertyName;
                mockProperty.getValue();
                result = propertyValue;
            }
        };

        HttpsSingleMessage httpsMsg =
                HttpsSingleMessage.parseHttpsMessage(mockMsg);
        String testContentType = httpsMsg.getContentType();

        String expectedContentType = "binary/octet-stream";
        assertThat(testContentType, is(expectedContentType));
    }

    // Tests_SRS_HTTPSSINGLEMESSAGE_11_013: [The function shall return a copy of the message properties.]
    @Test
    public void getPropertiesReturnsCopyOfProperties(
            @Mocked final Message mockMsg,
            @Mocked final MessageProperty mockProperty)
    {
        final byte[] body = { 0x61, 0x62, 0x63 };
        final MessageProperty[] properties = { mockProperty };
        final String propertyName = "test-property-name";
        final String httpsPropertyName = "iothub-app-test-property-name";
        final String propertyValue = "test-property-value";
        new NonStrictExpectations()
        {
            {
                mockMsg.getBytes();
                result = body;
                mockMsg.getProperties();
                result = properties;
                mockProperty.getName();
                result = propertyName;
                result = httpsPropertyName;
                mockProperty.getValue();
                result = propertyValue;
            }
        };

        HttpsSingleMessage httpsMsg = HttpsSingleMessage.parseHttpsMessage(mockMsg);
        MessageProperty[] testProperties = httpsMsg.getProperties();

        final MessageProperty[] expectedProperties = properties;
        assertThat(testProperties.length, is(expectedProperties.length));
        final String expectedPropertyName = httpsPropertyName;
        final String expectedPropertyValue = propertyValue;
        new Verifications()
        {
            {
                new MessageProperty(expectedPropertyName, expectedPropertyValue);
                times = 2;
            }
        };
    }
}
