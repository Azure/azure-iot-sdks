// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

package tests.unit.com.microsoft.azure.iothub.transport.https;

import static org.junit.Assert.assertThat;
import static org.hamcrest.CoreMatchers.is;

import com.microsoft.azure.iothub.DeviceClientConfig;
import com.microsoft.azure.iothub.Message;
import com.microsoft.azure.iothub.MessageProperty;
import com.microsoft.azure.iothub.IotHubMessageResult;
import com.microsoft.azure.iothub.IotHubStatusCode;
import com.microsoft.azure.iothub.auth.IotHubSasToken;
import com.microsoft.azure.iothub.transport.https.HttpsIotHubConnection;
import com.microsoft.azure.iothub.transport.https.HttpsMethod;
import com.microsoft.azure.iothub.transport.https.HttpsRequest;
import com.microsoft.azure.iothub.transport.https.HttpsResponse;

import com.microsoft.azure.iothub.net.IotHubAbandonUri;
import com.microsoft.azure.iothub.net.IotHubCompleteUri;
import com.microsoft.azure.iothub.net.IotHubMessageUri;
import com.microsoft.azure.iothub.net.IotHubRejectUri;
import com.microsoft.azure.iothub.net.IotHubEventUri;
import com.microsoft.azure.iothub.net.IotHubUri;
import com.microsoft.azure.iothub.transport.https.HttpsSingleMessage;
import mockit.Mocked;
import mockit.NonStrictExpectations;
import mockit.Verifications;
import org.junit.Test;

import java.io.IOException;
import java.net.URL;
import java.net.URLEncoder;

/** Unit tests for HttpsIotHubConnection. */
public class HttpsIotHubConnectionTest
{
    @Mocked
    DeviceClientConfig mockConfig;
    @Mocked
    IotHubUri mockIotHubUri;
    @Mocked
    URL mockUrl;
    @Mocked
    HttpsSingleMessage mockMsg;
    @Mocked
    IotHubSasToken mockToken;
    @Mocked
    HttpsRequest mockRequest;
    @Mocked
    HttpsResponse mockResponse;
    @Mocked
    IotHubStatusCode mockStatus;

    // Tests_SRS_HTTPSIOTHUBCONNECTION_11_002: [The function shall send a request to the URL 'https://[iotHubHostname]/devices/[deviceId]/messages/events?api-version=2016-02-03'.] 
    // Tests_SRS_HTTPSIOTHUBCONNECTION_11_001: [The constructor shall save the client configuration.]
    @Test
    public void sendEventHasCorrectUrl(
            @Mocked final IotHubEventUri mockUri) throws IOException
    {
        final String iotHubHostname = "test.iothub";
        final String deviceId = "test-device-id";
        final String eventUri = "test-event-uri";
        new NonStrictExpectations()
        {
            {
                mockConfig.getIotHubHostname();
                result = iotHubHostname;
                mockConfig.getDeviceId();
                result = deviceId;
                new IotHubEventUri(iotHubHostname, deviceId);
                result = mockUri;
                mockUri.toString();
                result = eventUri;
            }
        };

        HttpsIotHubConnection conn = new HttpsIotHubConnection(mockConfig);
        conn.sendEvent(mockMsg);

        final String expectedUrl = "https://" + eventUri;
        new Verifications()
        {
            {
                new URL(expectedUrl);
            }
        };
    }

    // Tests_SRS_HTTPSIOTHUBCONNECTION_11_003: [The function shall send a POST request.]
    @Test
    public void sendEventSendsPostRequest(
            @Mocked final IotHubEventUri mockUri) throws IOException
    {
        HttpsIotHubConnection conn = new HttpsIotHubConnection(mockConfig);
        conn.sendEvent(mockMsg);

        final HttpsMethod expectedMethod = HttpsMethod.POST;
        new Verifications()
        {
            {
                new HttpsRequest((URL) any, expectedMethod, (byte[]) any);
            }
        };
    }

    // Tests_SRS_HTTPSIOTHUBCONNECTION_11_004: [The function shall set the request body to the message body.]
    @Test
    public void sendEventSendsMessageBody(
            @Mocked final IotHubEventUri mockUri) throws IOException
    {
        final byte[] body = { 0x61, 0x62 };
        new NonStrictExpectations()
        {
            {
                mockMsg.getBody();
                result = body;
            }
        };

        HttpsIotHubConnection conn = new HttpsIotHubConnection(mockConfig);
        conn.sendEvent(mockMsg);

        final byte[] expectedBody = body;
        new Verifications()
        {
            {
                new HttpsRequest((URL) any, (HttpsMethod) any, expectedBody);
            }
        };
    }

    // Tests_SRS_HTTPSIOTHUBCONNECTION_11_005: [The function shall write each message property as a request header.]
    @Test
    public void sendEventSendsMessageProperties(
            @Mocked final IotHubEventUri mockUri,
            @Mocked final MessageProperty mockProperty) throws IOException
    {
        final MessageProperty[] properties = { mockProperty };
        final String propertyName = "test-property-name";
        final String propertyValue = "test-property-value";
        new NonStrictExpectations()
        {
            {
                mockMsg.getProperties();
                result = properties;
                mockProperty.getName();
                result = propertyName;
                mockProperty.getValue();
                result = propertyValue;
            }
        };

        HttpsIotHubConnection conn = new HttpsIotHubConnection(mockConfig);
        conn.sendEvent(mockMsg);

        final String expectedPropertyName = propertyName;
        final String expectedPropertyValue = propertyValue;
        new Verifications()
        {
            {
                mockRequest.setHeaderField(expectedPropertyName,
                        expectedPropertyValue);
            }
        };
    }

    // Tests_SRS_HTTPSIOTHUBCONNECTION_11_006: [The function shall set the request read timeout to be the configuration parameter readTimeoutMillis.]
    @Test
    public void sendEventSetsReadTimeout(@Mocked final IotHubEventUri mockUri) throws IOException
    {
        final int readTimeoutMillis = 10;
        new NonStrictExpectations()
        {
            {
                mockConfig.getReadTimeoutMillis();
                result = readTimeoutMillis;
            }
        };

        HttpsIotHubConnection conn = new HttpsIotHubConnection(mockConfig);
        conn.sendEvent(mockMsg);

        final int expectedReadTimeoutMillis = readTimeoutMillis;
        new Verifications()
        {
            {
                mockRequest.setReadTimeoutMillis(expectedReadTimeoutMillis);
            }
        };
    }

    // Tests_SRS_HTTPSIOTHUBCONNECTION_11_007: [The function shall set the header field 'authorization' to be a valid SAS token generated from the configuration parameters.]
    @Test
    public void sendEventSetsAuthToSasToken(@Mocked final IotHubEventUri mockUri) throws IOException
    {
        final String iotHubHostname = "test-iothubname";
        final String deviceId = "test-device-key";
        final String deviceKey = "test-device-key";
        final String tokenStr = "test-token-str";
        new NonStrictExpectations()
        {
            {
                mockConfig.getIotHubHostname();
                result = iotHubHostname;
                mockConfig.getDeviceId();
                result = deviceId;
                mockConfig.getDeviceKey();
                result = deviceKey;
                new IotHubSasToken(mockConfig, anyLong);
                result = mockToken;
                mockToken.toString();
                result = tokenStr;
            }
        };

        HttpsIotHubConnection conn = new HttpsIotHubConnection(mockConfig);
        conn.sendEvent(mockMsg);

        final String expectedTokenStr = tokenStr;
        new Verifications()
        {
            {
                mockRequest.setHeaderField(withMatch("(?i)authorization"),
                        expectedTokenStr);
            }
        };
    }

    // Tests_SRS_HTTPSIOTHUBCONNECTION_11_008: [The function shall set the header field 'iothub-to' to be '/devices/[deviceId]/messages/events'.]
    @Test
    public void sendEventSetsIotHubToToPath(@Mocked final IotHubEventUri mockUri) throws IOException
    {
        final String path = "test-path";
        new NonStrictExpectations()
        {
            {
                mockUri.getPath();
                result = path;
            }
        };

        HttpsIotHubConnection conn = new HttpsIotHubConnection(mockConfig);
        conn.sendEvent(mockMsg);

        final String expectedPath = path;
        new Verifications()
        {
            {
                mockRequest.setHeaderField(withMatch("(?i)iothub-to"),
                        expectedPath);
            }
        };
    }

    // Tests_SRS_HTTPSIOTHUBCONNECTION_11_009: [The function shall set the header field 'content-type' to be the message content type.]
    @Test
    public void sendEventSetsContentTypeCorrectly(@Mocked final IotHubEventUri mockUri) throws IOException
    {
        final String contentType = "test-content-type";
        new NonStrictExpectations()
        {
            {
                mockMsg.getContentType();
                result = contentType;
            }
        };

        HttpsIotHubConnection conn = new HttpsIotHubConnection(mockConfig);
        conn.sendEvent(mockMsg);

        final String expectedContentType = contentType;
        new Verifications()
        {
            {
                mockRequest.setHeaderField(withMatch("(?i)content-type"),
                        expectedContentType);
            }
        };
    }

    // Tests_SRS_HTTPSIOTHUBCONNECTION_11_010: [The function shall return the IoT Hub status code included in the response.]
    @Test
    public void sendEventReturnsCorrectStatusCode(@Mocked final IotHubEventUri mockUri) throws IOException
    {
        final int httpsStatus = 200;
        final IotHubStatusCode status = IotHubStatusCode.OK;
        new NonStrictExpectations()
        {
            {
                mockResponse.getStatus();
                result = httpsStatus;
                IotHubStatusCode.getIotHubStatusCode(httpsStatus);
                result = status;
            }
        };

        HttpsIotHubConnection conn = new HttpsIotHubConnection(mockConfig);
        IotHubStatusCode testStatus = conn.sendEvent(mockMsg);

        final IotHubStatusCode expectedStatus = status;
        assertThat(testStatus, is(expectedStatus));
    }

    // Tests_SRS_HTTPSIOTHUBCONNECTION_11_012: [If the IoT Hub could not be reached, the function shall throw an IOException.]
    @Test(expected = IOException.class)
    public void sendEventThrowsIOExceptionIfRequestFails(@Mocked final IotHubEventUri mockUri) throws IOException
    {
        new NonStrictExpectations()
        {
            {
                mockRequest.send();
                result = new IOException();
            }
        };

        HttpsIotHubConnection conn = new HttpsIotHubConnection(mockConfig);
        conn.sendEvent(mockMsg);
    }

    // Tests_SRS_HTTPSIOTHUBCONNECTION_11_013: [The function shall send a request to the URL 'https://[iotHubHostname]/devices/[deviceId]/messages/devicebound?api-version=2016-02-03'.]
    @Test
    public void receiveMessageHasCorrectUrl(@Mocked final IotHubMessageUri mockUri) throws IOException
    {
        final String iotHubHostname = "test.iothub";
        final String deviceId = "test-device-id";
        final String messageUri = "test-message-uri";
        new NonStrictExpectations()
        {
            {
                mockConfig.getIotHubHostname();
                result = iotHubHostname;
                mockConfig.getDeviceId();
                result = deviceId;
                new IotHubMessageUri(iotHubHostname, deviceId);
                result = mockUri;
                mockUri.toString();
                result = messageUri;
            }
        };

        HttpsIotHubConnection conn = new HttpsIotHubConnection(mockConfig);
        conn.receiveMessage();

        final String expectedUrl = "https://" + messageUri;
        new Verifications()
        {
            {
                new URL(expectedUrl);
            }
        };
    }

    // Tests_SRS_HTTPSIOTHUBCONNECTION_11_014: [The function shall send a GET request.]
    @Test
    public void receiveMessageSendsGetRequest(@Mocked final IotHubMessageUri mockUri) throws IOException
    {
        HttpsIotHubConnection conn = new HttpsIotHubConnection(mockConfig);
        conn.receiveMessage();

        final HttpsMethod expectedMethod = HttpsMethod.GET;
        new Verifications()
        {
            {
                new HttpsRequest((URL) any, expectedMethod, (byte[]) any);
            }
        };
    }

    // Tests_SRS_HTTPSIOTHUBCONNECTION_11_015: [The function shall set the request read timeout to be the configuration parameter readTimeoutMillis.]
    @Test
    public void receiveMessageSetsReadTimeout(@Mocked final IotHubMessageUri mockUri) throws IOException
    {
        final int readTimeoutMillis = 10;
        new NonStrictExpectations()
        {
            {
                mockConfig.getReadTimeoutMillis();
                result = readTimeoutMillis;
            }
        };

        HttpsIotHubConnection conn = new HttpsIotHubConnection(mockConfig);
        conn.receiveMessage();

        final int expectedReadTimeoutMillis = readTimeoutMillis;
        new Verifications()
        {
            {
                mockRequest.setReadTimeoutMillis(expectedReadTimeoutMillis);
            }
        };
    }

    // Tests_SRS_HTTPSIOTHUBCONNECTION_11_016: [The function shall set the header field 'authorization' to be a valid SAS token generated from the configuration parameters.]
    @Test
    public void receiveMessageSetsAuthToSasToken(@Mocked final IotHubMessageUri mockUri) throws IOException
    {
        final String iotHubHostname = "test-iothubname";
        final String deviceId = "test-device-key";
        final String deviceKey = "test-device-key";
        final String tokenStr = "test-token-str";
        new NonStrictExpectations()
        {
            {
                mockConfig.getIotHubHostname();
                result = iotHubHostname;
                mockConfig.getDeviceId();
                result = deviceId;
                mockConfig.getDeviceKey();
                result = deviceKey;
                new IotHubSasToken(mockConfig, anyLong);
                result = mockToken;
                mockToken.toString();
                result = tokenStr;
            }
        };

        HttpsIotHubConnection conn = new HttpsIotHubConnection(mockConfig);
        conn.receiveMessage();

        final String expectedTokenStr = tokenStr;
        new Verifications()
        {
            {
                mockRequest.setHeaderField(withMatch("(?i)authorization"),
                        expectedTokenStr);
            }
        };
    }

    // Tests_SRS_HTTPSIOTHUBCONNECTION_11_017: [The function shall set the header field 'iothub-to' to be '/devices/[deviceId]/messages/devicebound'.]
    @Test
    public void receiveMessageSetsIotHubToToPath(@Mocked final IotHubMessageUri mockUri) throws IOException
    {
        final String path = "test-path";
        new NonStrictExpectations()
        {
            {
                mockUri.getPath();
                result = path;
            }
        };

        HttpsIotHubConnection conn = new HttpsIotHubConnection(mockConfig);
        conn.receiveMessage();

        final String expectedPath = path;
        new Verifications()
        {
            {
                mockRequest.setHeaderField(withMatch("(?i)iothub-to"),
                        expectedPath);
            }
        };
    }

    // Tests_SRS_HTTPSIOTHUBCONNECTION_11_018: [The function shall set the header field 'iothub-messagelocktimeout' to be the configuration parameter messageLockTimeoutSecs.]
    @Test
    public void receiveMessageSetsMessageLockTimeout(@Mocked final IotHubMessageUri mockUri) throws IOException
    {
        final int messageLockTimeoutSecs = 24;
        new NonStrictExpectations()
        {
            {
                mockConfig.getMessageLockTimeoutSecs();
                result = messageLockTimeoutSecs;
            }
        };

        HttpsIotHubConnection conn = new HttpsIotHubConnection(mockConfig);
        conn.receiveMessage();

        final String expectedMessageLockTimeoutSecs =
                Integer.toString(messageLockTimeoutSecs);
        new Verifications()
        {
            {
                mockRequest.setHeaderField(
                        withMatch("(?i)iothub-messagelocktimeout"),
                        expectedMessageLockTimeoutSecs);
            }
        };
    }

    // Tests_SRS_HTTPSIOTHUBCONNECTION_11_019: [If a response with IoT Hub status code OK is received, the function shall return the IoT Hub message included in the response.]
    @Test
    public void receiveMessageReturnsMessageBody(@Mocked final IotHubMessageUri mockUri) throws IOException
    {
        final byte[] body = { 0x61, 0x62 };
        final String eTag = "test-etag";
        new NonStrictExpectations()
        {
            {
                mockResponse.getBody();
                result = body;
                mockResponse.getStatus();
                result = 200;
                IotHubStatusCode.getIotHubStatusCode(200);
                result = IotHubStatusCode.OK;
                mockResponse.getHeaderField(withMatch("(?i)etag"));
                result = eTag;
            }
        };

        HttpsIotHubConnection conn = new HttpsIotHubConnection(mockConfig);
        conn.receiveMessage();

        final byte[] expectedBody = body;
        new Verifications()
        {
            {
                new Message(expectedBody);
            }
        };
    }

    // Tests_SRS_HTTPSIOTHUBCONNECTION_11_020: [If a response with IoT Hub status code OK is received, the function shall save the response header field 'etag'.]
    @Test
    public void receiveMessageSavesEtag(@Mocked final IotHubMessageUri mockUri) throws IOException
    {
        final String eTag = "test-etag";
        new NonStrictExpectations()
        {
            {
                mockResponse.getStatus();
                result = 200;
                IotHubStatusCode.getIotHubStatusCode(200);
                result = IotHubStatusCode.OK;
                mockResponse.getHeaderField(withMatch("(?i)etag"));
                result = eTag;
            }
        };

        HttpsIotHubConnection conn = new HttpsIotHubConnection(mockConfig);
        conn.receiveMessage();

        new Verifications()
        {
            {
                mockResponse.getHeaderField(withMatch("(?i)etag"));
            }
        };
    }

    // Tests_SRS_HTTPSIOTHUBCONNECTION_11_021: [If a response with IoT Hub status code OK is not received, the function shall return null.]
    @Test
    public void receiveMessageReturnsNullIfNoMessageReceived(@Mocked final IotHubMessageUri mockUri) throws IOException
    {
        new NonStrictExpectations()
        {
            {
                mockResponse.getStatus();
                result = 204;
                IotHubStatusCode.getIotHubStatusCode(204);
                result = IotHubStatusCode.OK_EMPTY;
            }
        };

        HttpsIotHubConnection conn = new HttpsIotHubConnection(mockConfig);
        Message testMsg = conn.receiveMessage();

        Message expectedMsg = null;
        assertThat(testMsg, is(expectedMsg));
    }

    // Tests_SRS_HTTPSIOTHUBCONNECTION_11_023: [If the IoT Hub could not be reached, the function shall throw an IOException.]
    @Test(expected = IOException.class)
    public void receiveMessageThrowsIOExceptionIfRequestFails(@Mocked final IotHubMessageUri mockUri) throws IOException
    {
        new NonStrictExpectations()
        {
            {
                mockRequest.send();
                result = new IOException();
            }
        };

        HttpsIotHubConnection conn = new HttpsIotHubConnection(mockConfig);
        conn.receiveMessage();
    }

    // Tests_SRS_HTTPSIOTHUBCONNECTION_11_024: [If the result is COMPLETE, the function shall send a request to the URL 'https://[iotHubHostname]/devices/[deviceId]/messages/devicebound/[eTag]?api-version=2016-02-03'.]
    @Test
    public void sendMessageResultWhenCompleteUsesCompleteUrl(@Mocked final IotHubCompleteUri mockUri) throws IOException
    {
        final String iotHubHostname = "test.iothub";
        final String deviceId = "test-device-id";
        final String completeUri = "test-complete-uri";
        final String eTag = "test-etag";
        new NonStrictExpectations()
        {
            {
                mockResponse.getStatus();
                returns(200, 204);
                IotHubStatusCode.getIotHubStatusCode(200);
                result = IotHubStatusCode.OK;
                IotHubStatusCode.getIotHubStatusCode(204);
                result = IotHubStatusCode.OK_EMPTY;
                mockResponse.getHeaderField(withMatch("(?i)etag"));
                result = eTag;
                mockConfig.getIotHubHostname();
                result = iotHubHostname;
                mockConfig.getDeviceId();
                result = deviceId;
                new IotHubCompleteUri(iotHubHostname, deviceId, eTag);
                result = mockUri;
                mockUri.toString();
                result = completeUri;
            }
        };

        HttpsIotHubConnection conn = new HttpsIotHubConnection(mockConfig);
        conn.receiveMessage();
        conn.sendMessageResult(IotHubMessageResult.COMPLETE);

        final String expectedUrl = "https://" + completeUri;
        new Verifications()
        {
            {
                new URL(expectedUrl);
            }
        };
    }

    // Tests_SRS_HTTPSIOTHUBCONNECTION_11_025: [If the result is COMPLETE, the function shall send a DELETE request.]
    @Test
    public void sendMessageResultWhenCompleteSendsDeleteRequest(@Mocked final IotHubCompleteUri mockUri) throws IOException
    {
        final String eTag = "test-etag";
        new NonStrictExpectations()
        {
            {
                mockResponse.getStatus();
                returns(200, 204);
                IotHubStatusCode.getIotHubStatusCode(200);
                result = IotHubStatusCode.OK;
                IotHubStatusCode.getIotHubStatusCode(204);
                result = IotHubStatusCode.OK_EMPTY;
                mockResponse.getHeaderField(withMatch("(?i)etag"));
                result = eTag;
            }
        };

        HttpsIotHubConnection conn = new HttpsIotHubConnection(mockConfig);
        conn.receiveMessage();
        conn.sendMessageResult(IotHubMessageResult.COMPLETE);

        final HttpsMethod expectedMethod = HttpsMethod.DELETE;
        new Verifications()
        {
            {
                new HttpsRequest((URL) any, expectedMethod, (byte[]) any);
            }
        };
    }

    // Tests_SRS_HTTPSIOTHUBCONNECTION_11_026: [If the result is COMPLETE, the function shall set the header field 'iothub-to' to be '/devices/[deviceId]/messages/devicebound/[eTag]'.]
    @Test
    public void sendMessageResultWhenCompleteSetsIotHubToToPath(@Mocked final IotHubCompleteUri mockUri) throws IOException
    {
        final String iotHubHostname = "test.iothub";
        final String deviceId = "test-device-id";
        final String completeUri = "test-complete-uri";
        final String eTag = "test-etag";
        final String path = "test-path";
        new NonStrictExpectations()
        {
            {
                mockResponse.getStatus();
                returns(200, 204);
                IotHubStatusCode.getIotHubStatusCode(200);
                result = IotHubStatusCode.OK;
                IotHubStatusCode.getIotHubStatusCode(204);
                result = IotHubStatusCode.OK_EMPTY;
                mockResponse.getHeaderField(withMatch("(?i)etag"));
                result = eTag;
                mockConfig.getIotHubHostname();
                result = iotHubHostname;
                mockConfig.getDeviceId();
                result = deviceId;
                new IotHubCompleteUri(iotHubHostname, deviceId, eTag);
                result = mockUri;
                mockUri.toString();
                result = completeUri;
                mockUri.getPath();
                result = path;
            }
        };

        HttpsIotHubConnection conn = new HttpsIotHubConnection(mockConfig);
        conn.receiveMessage();
        conn.sendMessageResult(IotHubMessageResult.COMPLETE);

        final String expectedPath = path;
        new Verifications()
        {
            {
                mockRequest.setHeaderField(withMatch("(?i)iothub-to"),
                        expectedPath);
            }
        };
    }

    // Tests_SRS_HTTPSIOTHUBCONNECTION_11_027: [If the result is ABANDON, the function shall send a request to the URL 'https://[iotHubHostname]/devices/[deviceId]/messages/devicebound/[eTag]/abandon?api-version=2016-02-03'.]
    @Test
    public void sendMessageResultWhenAbandonUsesAbandonUrl(@Mocked final IotHubAbandonUri mockUri) throws IOException
    {
        final String iotHubHostname = "test.iothub";
        final String deviceId = "test-device-id";
        final String abandonUri = "test-abandon-uri";
        final String eTag = "test-etag";
        new NonStrictExpectations()
        {
            {
                mockResponse.getStatus();
                returns(200, 204);
                IotHubStatusCode.getIotHubStatusCode(200);
                result = IotHubStatusCode.OK;
                IotHubStatusCode.getIotHubStatusCode(204);
                result = IotHubStatusCode.OK_EMPTY;
                mockResponse.getHeaderField(withMatch("(?i)etag"));
                result = eTag;
                mockConfig.getIotHubHostname();
                result = iotHubHostname;
                mockConfig.getDeviceId();
                result = deviceId;
                new IotHubAbandonUri(iotHubHostname, deviceId, eTag);
                result = mockUri;
                mockUri.toString();
                result = abandonUri;
            }
        };

        HttpsIotHubConnection conn = new HttpsIotHubConnection(mockConfig);
        conn.receiveMessage();
        conn.sendMessageResult(IotHubMessageResult.ABANDON);

        final String expectedUrl = "https://" + abandonUri;
        new Verifications()
        {
            {
                new URL(expectedUrl);
            }
        };
    }

    // Tests_SRS_HTTPSIOTHUBCONNECTION_11_028: [If the result is ABANDON, the function shall send a POST request.]
    @Test
    public void sendMessageResultWhenAbandonSendsPostRequest(@Mocked final IotHubAbandonUri mockUri) throws IOException
    {
        final String eTag = "test-etag";
        new NonStrictExpectations()
        {
            {
                mockResponse.getStatus();
                returns(200, 204);
                IotHubStatusCode.getIotHubStatusCode(200);
                result = IotHubStatusCode.OK;
                IotHubStatusCode.getIotHubStatusCode(204);
                result = IotHubStatusCode.OK_EMPTY;
                mockResponse.getHeaderField(withMatch("(?i)etag"));
                result = eTag;
            }
        };

        HttpsIotHubConnection conn = new HttpsIotHubConnection(mockConfig);
        conn.receiveMessage();
        conn.sendMessageResult(IotHubMessageResult.ABANDON);

        final HttpsMethod expectedMethod = HttpsMethod.POST;
        new Verifications()
        {
            {
                new HttpsRequest((URL) any, expectedMethod, (byte[]) any);
            }
        };
    }

    // Tests_SRS_HTTPSIOTHUBCONNECTION_11_029: [If the result is ABANDON, the function shall set the header field 'iothub-to' to be '/devices/[deviceId]/messages/devicebound/[eTag]/abandon'.]
    @Test
    public void sendMessageResultWhenAbandonSetsIotHubToToPath(@Mocked final IotHubAbandonUri mockUri) throws IOException
    {
        final String iotHubHostname = "test.iothub";
        final String deviceId = "test-device-id";
        final String abandonUri = "test-abandon-uri";
        final String eTag = "test-etag";
        final String path = "test-path";
        new NonStrictExpectations()
        {
            {
                mockResponse.getStatus();
                returns(200, 204);
                IotHubStatusCode.getIotHubStatusCode(200);
                result = IotHubStatusCode.OK;
                IotHubStatusCode.getIotHubStatusCode(204);
                result = IotHubStatusCode.OK_EMPTY;
                mockResponse.getHeaderField(withMatch("(?i)etag"));
                result = eTag;
                mockConfig.getIotHubHostname();
                result = iotHubHostname;
                mockConfig.getDeviceId();
                result = deviceId;
                new IotHubAbandonUri(iotHubHostname, deviceId, eTag);
                result = mockUri;
                mockUri.toString();
                result = abandonUri;
                mockUri.getPath();
                result = path;
            }
        };

        HttpsIotHubConnection conn = new HttpsIotHubConnection(mockConfig);
        conn.receiveMessage();
        conn.sendMessageResult(IotHubMessageResult.ABANDON);

        final String expectedPath = path;
        new Verifications()
        {
            {
                mockRequest.setHeaderField(withMatch("(?i)iothub-to"),
                        expectedPath);
            }
        };
    }

    // Tests_SRS_HTTPSIOTHUBCONNECTION_11_030: [If the result is REJECT, the function shall send a request to the URL 'https://[iotHubHostname]/devices/[deviceId]/messages/devicebound/[eTag]??reject=true&api-version=2016-02-03' (the query parameters can be in any order).]
    @Test
    public void sendMessageResultWhenRejectUsesRejectUrl(@Mocked final IotHubRejectUri mockUri) throws IOException
    {
        final String iotHubHostname = "test.iothub";
        final String deviceId = "test-device-id";
        final String rejectUri = "test-reject-uri";
        final String eTag = "test-etag";
        new NonStrictExpectations()
        {
            {
                mockResponse.getStatus();
                returns(200, 204);
                IotHubStatusCode.getIotHubStatusCode(200);
                result = IotHubStatusCode.OK;
                IotHubStatusCode.getIotHubStatusCode(204);
                result = IotHubStatusCode.OK_EMPTY;
                mockResponse.getHeaderField(withMatch("(?i)etag"));
                result = eTag;
                mockConfig.getIotHubHostname();
                result = iotHubHostname;
                mockConfig.getDeviceId();
                result = deviceId;
                new IotHubRejectUri(iotHubHostname, deviceId, eTag);
                result = mockUri;
                mockUri.toString();
                result = rejectUri;
            }
        };

        HttpsIotHubConnection conn = new HttpsIotHubConnection(mockConfig);
        conn.receiveMessage();
        conn.sendMessageResult(IotHubMessageResult.REJECT);

        final String expectedUrl = "https://" + rejectUri;
        new Verifications()
        {
            {
                new URL(expectedUrl);
            }
        };
    }

    // Tests_SRS_HTTPSIOTHUBCONNECTION_11_031: [If the result is REJECT, the function shall send a DELETE request.]
    @Test
    public void sendMessageResultWhenRejectSendsDeleteRequest(@Mocked final IotHubRejectUri mockUri) throws IOException
    {
        final String eTag = "test-etag";
        new NonStrictExpectations()
        {
            {
                mockResponse.getStatus();
                returns(200, 204);
                IotHubStatusCode.getIotHubStatusCode(200);
                result = IotHubStatusCode.OK;
                IotHubStatusCode.getIotHubStatusCode(204);
                result = IotHubStatusCode.OK_EMPTY;
                mockResponse.getHeaderField(withMatch("(?i)etag"));
                result = eTag;
            }
        };

        HttpsIotHubConnection conn = new HttpsIotHubConnection(mockConfig);
        conn.receiveMessage();
        conn.sendMessageResult(IotHubMessageResult.REJECT);

        final HttpsMethod expectedMethod = HttpsMethod.DELETE;
        new Verifications()
        {
            {
                new HttpsRequest((URL) any, expectedMethod, (byte[]) any);
            }
        };
    }

    // Tests_SRS_HTTPSIOTHUBCONNECTION_11_032: [If the result is REJECT, the function shall set the header field 'iothub-to' to be '/devices/[deviceId]/messages/devicebound/[eTag]'.]
    @Test
    public void sendMessageResultWhenRejectSetsIotHubToToPath(@Mocked final IotHubRejectUri mockUri) throws IOException
    {
        final String iotHubHostname = "test.iothub";
        final String deviceId = "test-device-id";
        final String rejectUri = "test-reject-uri";
        final String eTag = "test-etag";
        final String path = "test-path";
        new NonStrictExpectations()
        {
            {
                mockResponse.getStatus();
                returns(200, 204);
                IotHubStatusCode.getIotHubStatusCode(200);
                result = IotHubStatusCode.OK;
                IotHubStatusCode.getIotHubStatusCode(204);
                result = IotHubStatusCode.OK_EMPTY;
                mockResponse.getHeaderField(withMatch("(?i)etag"));
                result = eTag;
                mockConfig.getIotHubHostname();
                result = iotHubHostname;
                mockConfig.getDeviceId();
                result = deviceId;
                new IotHubRejectUri(iotHubHostname, deviceId, eTag);
                result = mockUri;
                mockUri.toString();
                result = rejectUri;
                mockUri.getPath();
                result = path;
            }
        };

        HttpsIotHubConnection conn = new HttpsIotHubConnection(mockConfig);
        conn.receiveMessage();
        conn.sendMessageResult(IotHubMessageResult.REJECT);

        final String expectedPath = path;
        new Verifications()
        {
            {
                mockRequest.setHeaderField(withMatch("(?i)iothub-to"),
                        expectedPath);
            }
        };
    }

    // Tests_SRS_HTTPSIOTHUBCONNECTION_11_033: [The function shall set the request read timeout to be the configuration parameter readTimeoutMillis.]
    @Test
    public void sendMessageResultSetsReadTimeout(@Mocked final IotHubRejectUri mockUri) throws IOException
    {
        final String eTag = "test-etag";
        final int readTimeoutMillis = 23;
        new NonStrictExpectations()
        {
            {
                mockResponse.getStatus();
                returns(200, 204);
                IotHubStatusCode.getIotHubStatusCode(200);
                result = IotHubStatusCode.OK;
                IotHubStatusCode.getIotHubStatusCode(204);
                result = IotHubStatusCode.OK_EMPTY;
                mockResponse.getHeaderField(withMatch("(?i)etag"));
                result = eTag;
                mockConfig.getReadTimeoutMillis();
                result = readTimeoutMillis;
            }
        };

        HttpsIotHubConnection conn = new HttpsIotHubConnection(mockConfig);
        conn.receiveMessage();
        conn.sendMessageResult(IotHubMessageResult.REJECT);

        final int expectedReadTimeoutMillis = readTimeoutMillis;
        new Verifications()
        {
            {
                mockRequest.setReadTimeoutMillis(expectedReadTimeoutMillis);
            }
        };
    }

    // Tests_SRS_HTTPSIOTHUBCONNECTION_11_034: [The function shall set the header field 'authorization' to be a valid SAS token generated from the configuration parameters.]
    @Test
    public void sendMessageResultSetsAuthToSasToken(@Mocked final IotHubRejectUri mockUri) throws IOException
    {
        final String eTag = "test-etag";
        final String iotHubHostname = "test-iothubname";
        final String deviceId = "test-device-key";
        final String deviceKey = "test-device-key";
        final String tokenStr = "test-token-str";
        new NonStrictExpectations()
        {
            {
                mockResponse.getStatus();
                returns(200, 204);
                IotHubStatusCode.getIotHubStatusCode(200);
                result = IotHubStatusCode.OK;
                IotHubStatusCode.getIotHubStatusCode(204);
                result = IotHubStatusCode.OK_EMPTY;
                mockResponse.getHeaderField(withMatch("(?i)etag"));
                result = eTag;
                mockConfig.getIotHubHostname();
                result = iotHubHostname;
                mockConfig.getDeviceId();
                result = deviceId;
                mockConfig.getDeviceKey();
                result = deviceKey;
                new IotHubSasToken(mockConfig, anyLong);
                result = mockToken;
                mockToken.toString();
                result = tokenStr;
            }
        };

        HttpsIotHubConnection conn = new HttpsIotHubConnection(mockConfig);
        conn.receiveMessage();
        conn.sendMessageResult(IotHubMessageResult.REJECT);

        final String expectedTokenStr = tokenStr;
        new Verifications()
        {
            {
                mockRequest.setHeaderField(withMatch("(?i)authorization"),
                        expectedTokenStr);
            }
        };
    }

    // Tests_SRS_HTTPSIOTHUBCONNECTION_11_035: [The function shall set the header field 'if-match' to be the e-tag saved when receiveMessage() was previously called.]
    @Test
    public void sendMessageResultSetsIfMatchToEtag(@Mocked final IotHubRejectUri mockUri) throws IOException
    {
        final String eTag = "test-etag";
        new NonStrictExpectations()
        {
            {
                mockResponse.getStatus();
                returns(200, 204);
                IotHubStatusCode.getIotHubStatusCode(200);
                result = IotHubStatusCode.OK;
                IotHubStatusCode.getIotHubStatusCode(204);
                result = IotHubStatusCode.OK_EMPTY;
                mockResponse.getHeaderField(withMatch("(?i)etag"));
                result = eTag;
            }
        };

        HttpsIotHubConnection conn = new HttpsIotHubConnection(mockConfig);
        conn.receiveMessage();
        conn.sendMessageResult(IotHubMessageResult.REJECT);

        final String expectedEtag = eTag;
        new Verifications()
        {
            {
                mockRequest.setHeaderField(withMatch("(?i)if-match"),
                        expectedEtag);
            }
        };
    }

    // Tests_SRS_HTTPSIOTHUBCONNECTION_11_037: [If the IoT Hub could not be reached, the function shall throw an IOException.]
    @Test(expected = IOException.class)
    public void sendMessageResultThrowsIOExceptionIfRequestFails(@Mocked final IotHubRejectUri mockUri) throws IOException
    {
        final String eTag = "test-etag";
        new NonStrictExpectations()
        {
            {
                mockResponse.getStatus();
                returns(200, 204);
                IotHubStatusCode.getIotHubStatusCode(200);
                result = IotHubStatusCode.OK;
                IotHubStatusCode.getIotHubStatusCode(204);
                result = IotHubStatusCode.OK_EMPTY;
                mockResponse.getHeaderField(withMatch("(?i)etag"));
                result = eTag;
                mockRequest.send();
                result = mockResponse;
                result = new IOException();
            }
        };

        HttpsIotHubConnection conn = new HttpsIotHubConnection(mockConfig);
        conn.receiveMessage();
        conn.sendMessageResult(IotHubMessageResult.REJECT);
    }

    // Tests_SRS_HTTPSIOTHUBCONNECTION_11_038: [If the IoT Hub status code in the response is not OK_EMPTY, the function shall throw an IOException.]
    @Test(expected = IOException.class)
    public void sendMessageResultThrowsIOExceptionIfBadResponseStatus(@Mocked final IotHubRejectUri mockUri) throws IOException
    {
        final String eTag = "test-etag";
        new NonStrictExpectations()
        {
            {
                mockResponse.getStatus();
                result = 200;
                IotHubStatusCode.getIotHubStatusCode(200);
                result = IotHubStatusCode.OK;
                mockResponse.getHeaderField(withMatch("(?i)etag"));
                result = eTag;
            }
        };

        HttpsIotHubConnection conn = new HttpsIotHubConnection(mockConfig);
        conn.receiveMessage();
        conn.sendMessageResult(IotHubMessageResult.REJECT);
    }

    // Tests_SRS_HTTPSIOTHUBCONNECTION_11_039: [If the function is called before receiveMessage() returns a message, the function shall throw an IllegalStateException.]
    @Test(expected = IllegalStateException.class)
    public void sendMessageResultFailsIfReceiveNotCalled(
            @Mocked final IotHubRejectUri mockUri) throws IOException
    {
        HttpsIotHubConnection conn = new HttpsIotHubConnection(mockConfig);
        conn.sendMessageResult(IotHubMessageResult.REJECT);
    }

    // Tests_SRS_HTTPSIOTHUBCONNECTION_11_039: [If the function is called before receiveMessage() returns a message, the function shall throw an IllegalStateException.]
    @Test(expected = IllegalStateException.class)
    public void sendMessageResultFailsIfNoMessageReceived(
            @Mocked final IotHubRejectUri mockUri) throws IOException
    {
        HttpsIotHubConnection conn = new HttpsIotHubConnection(mockConfig);
        conn.receiveMessage();
        conn.sendMessageResult(IotHubMessageResult.REJECT);
    }
}
