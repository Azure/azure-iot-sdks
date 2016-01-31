// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

package tests.unit.com.microsoft.azure.iothub.transport.https;

import static org.junit.Assert.assertThat;
import static org.hamcrest.CoreMatchers.is;

import com.microsoft.azure.iothub.transport.TransportUtils;
import com.microsoft.azure.iothub.transport.https.HttpsConnection;
import com.microsoft.azure.iothub.transport.https.HttpsRequest;
import mockit.*;
import org.junit.Test;

import com.microsoft.azure.iothub.transport.https.HttpsMethod;
import com.microsoft.azure.iothub.transport.https.HttpsResponse;

import java.io.IOException;
import java.net.URL;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

/** Unit tests for HttpsRequest. */
public class HttpsRequestTest
{
    @Mocked URL mockUrl;

    // Tests_SRS_HTTPSREQUEST_11_001: [The function shall open a connection with the given URL as the endpoint.]
    @Test
    public void constructorOpensConnection(@Mocked final HttpsConnection mockConn) throws IOException
    {
        final HttpsMethod httpsMethod = HttpsMethod.GET;
        final byte[] body = new byte[0];
        new NonStrictExpectations()
        {
            {
                mockUrl.getProtocol();
                result = "https";
            }
        };

        new HttpsRequest(mockUrl, httpsMethod, body);

        new Verifications()
        {
            {
                new HttpsConnection(mockUrl, (HttpsMethod) any);
            }
        };
    }

    // Tests_SRS_HTTPSREQUEST_11_002: [The function shall write the body to the connection.]
    @Test
    public void constructorWritesBodyToConnection(@Mocked final HttpsConnection mockConn) throws IOException
    {
        final HttpsMethod httpsMethod = HttpsMethod.GET;
        final byte[] body = { 1, 2, 3 };
        new NonStrictExpectations()
        {
            {
                mockUrl.getProtocol();
                result = "https";
            }
        };

        new HttpsRequest(mockUrl, httpsMethod, body);

        final byte[] expectedBody = body;
        new Verifications()
        {
            {
                new HttpsConnection(mockUrl, (HttpsMethod) any)
                        .writeOutput(expectedBody);
            }
        };
    }

    // Tests_SRS_HTTPSREQUEST_11_004: [The function shall use the given HTTPS method (i.e. GET) as the request method.]
    @Test
    public void constructorSetsHttpsMethodCorrectly(@Mocked final HttpsConnection mockConn) throws IOException
    {
        final HttpsMethod httpsMethod = HttpsMethod.GET;
        final byte[] body = new byte[0];
        new NonStrictExpectations()
        {
            {
                mockUrl.getProtocol();
                result = "https";
            }
        };

        new HttpsRequest(mockUrl, httpsMethod, body);

        new Verifications()
        {
            {
                new HttpsConnection((URL) any, httpsMethod);
            }
        };
    }

    // Tests_SRS_HTTPSREQUEST_11_005: [If an IOException occurs in setting up the HTTPS connection, the function shall throw an IOException.]
    @Test(expected = IOException.class)
    public void constructorThrowsIoExceptionIfCannotSetupConnection(@Mocked final HttpsConnection mockConn) throws IOException
    {
        final HttpsMethod httpsMethod = HttpsMethod.GET;
        final byte[] body = new byte[0];
        new NonStrictExpectations()
        {
            {
                mockUrl.getProtocol();
                result = "https";
                new HttpsConnection(mockUrl, httpsMethod);
                result = new IOException();
            }
        };

        new HttpsRequest(mockUrl, httpsMethod, body);
    }

    // Tests_SRS_HTTPSREQUEST_11_008: [The function shall send an HTTPS request as formatted in the constructor.]
    @Test
    public void sendHasCorrectHttpsMethod() throws IOException
    {
        final HttpsMethod expectedMethod = HttpsMethod.GET;
        final byte[] body = new byte[0];
        new MockUp<HttpsConnection>()
        {
            HttpsMethod testMethod;

            @Mock
            public void $init(URL url, HttpsMethod method)
            {
                this.testMethod = method;
            }

            @Mock
            public void connect() throws IOException
            {
                assertThat(testMethod, is(expectedMethod));
            }

            @Mock
            public void setRequestMethod(HttpsMethod method)
            {
                this.testMethod = method;
            }

            // every method that is used must be manually mocked.
            @Mock
            public void setRequestHeader(String field, String value)
            {

            }

            @Mock
            public void writeOutput(byte[] body)
            {

            }

            @Mock
            public byte[] readInput() throws IOException
            {
                return new byte[0];
            }

            @Mock
            public byte[] readError() throws IOException
            {
                return new byte[0];
            }

            @Mock
            public int getResponseStatus() throws IOException
            {
                return 0;
            }

            @Mock
            public Map<String, List<String>> getResponseHeaders()
                    throws IOException
            {
                return new HashMap<>();
            }
        };
        new NonStrictExpectations()
        {
            {
                mockUrl.getProtocol();
                result = "https";
            }
        };

        HttpsRequest request =
                new HttpsRequest(mockUrl, expectedMethod, body);
        request.send();
    }

    // Tests_SRS_HTTPSREQUEST_11_008: [The function shall send an HTTPS request as formatted in the constructor.]
    @Test
    public void sendSetsHeaderFieldsCorrectly() throws IOException
    {
        final HttpsMethod expectedMethod = HttpsMethod.GET;
        final byte[] body = new byte[0];
        final String field0 = "test-field0";
        final String value0 = "test-value0";
        final String field1 = "test-field1";
        final String value1 = "test-value1";
        final String userAgent = "User-Agent";
        final String userAgentValue = TransportUtils.javaDeviceClientIdentifier + TransportUtils.clientVersion;
        new MockUp<HttpsConnection>()
        {
            Map<String, String> testHeaderFields = new HashMap<>();

            @Mock
            public void $init(URL url, HttpsMethod method)
            {
            }

            @Mock
            public void connect() throws IOException
            {
                assertThat(testHeaderFields.size(), is(3));
                assertThat(testHeaderFields.get(field0), is(value0));
                assertThat(testHeaderFields.get(field1), is(value1));
                assertThat(testHeaderFields.get(userAgent), is(userAgentValue));
            }

            @Mock
            public void setRequestHeader(String field, String value)
            {
                testHeaderFields.put(field, value);
            }

            // every method that is used must be manually mocked.
            @Mock
            public void setRequestMethod(HttpsMethod method)
            {

            }

            @Mock
            public void writeOutput(byte[] body)
            {

            }

            @Mock
            public byte[] readInput() throws IOException
            {
                return new byte[0];
            }

            @Mock
            public byte[] readError() throws IOException
            {
                return new byte[0];
            }

            @Mock
            public int getResponseStatus() throws IOException
            {
                return 0;
            }

            @Mock
            public Map<String, List<String>> getResponseHeaders()
                    throws IOException
            {
                return new HashMap<>();
            }
        };
        new NonStrictExpectations()
        {
            {
                mockUrl.getProtocol();
                result = "https";
            }
        };

        HttpsRequest request = new HttpsRequest(mockUrl, expectedMethod, body);
        request.setHeaderField(field0, value0);
        request.setHeaderField(field1, value1);
        request.send();
    }

    // Tests_SRS_HTTPSREQUEST_11_008: [The function shall send an HTTPS request as formatted in the constructor.]
    @Test
    public void sendWritesBodyToOutputStream() throws IOException
    {
        final HttpsMethod httpsMethod = HttpsMethod.POST;
        final byte[] expectedBody = { 1, 2, 3 };
        new MockUp<HttpsConnection>()
        {
            byte[] testBody;

            @Mock
            public void $init(URL url, HttpsMethod method)
            {
            }

            @Mock
            public void connect() throws IOException
            {
                assertThat(testBody, is(expectedBody));
            }

            @Mock
            public void writeOutput(byte[] body)
            {
                this.testBody = body;
            }

            // every method that is used must be manually mocked.
            @Mock
            public void setRequestHeader(String field, String value)
            {

            }

            @Mock
            public void setRequestMethod(HttpsMethod method)
            {

            }

            @Mock
            public byte[] readInput() throws IOException
            {
                return new byte[0];
            }

            @Mock
            public byte[] readError() throws IOException
            {
                return new byte[0];
            }

            @Mock
            public int getResponseStatus() throws IOException
            {
                return 0;
            }

            @Mock
            public Map<String, List<String>> getResponseHeaders()
                    throws IOException
            {
                return new HashMap<>();
            }
        };
        new NonStrictExpectations()
        {
            {
                mockUrl.getProtocol();
                result = "https";
            }
        };

        HttpsRequest request =
                new HttpsRequest(mockUrl, httpsMethod, expectedBody);
        request.send();
    }

    // Tests_SRS_HTTPSREQUEST_11_009: [The function shall return the HTTPS response received, including the status code, body, header fields, and error reason (if any).]
    @Test
    public void sendReadsStatusCode(@Mocked final HttpsConnection mockConn) throws IOException
    {
        final HttpsMethod httpsMethod = HttpsMethod.GET;
        final byte[] body = new byte[0];
        final int status = 204;
        new NonStrictExpectations()
        {
            {
                mockUrl.getProtocol();
                result = "https";
                mockConn.getResponseStatus();
                result = status;
            }
        };

        HttpsRequest request =
                new HttpsRequest(mockUrl, httpsMethod, body);
        HttpsResponse response = request.send();
        int testStatus = response.getStatus();

        final int expectedStatus = status;
        assertThat(testStatus, is(expectedStatus));
    }

    // Tests_SRS_HTTPSREQUEST_11_009: [The function shall return the HTTPS response received, including the status code, body, header fields, and error reason (if any).]
    @Test
    public void sendReturnsBody(@Mocked final HttpsConnection mockConn) throws IOException
    {
        final HttpsMethod httpsMethod = HttpsMethod.GET;
        final byte[] requestBody = new byte[0];
        final byte[] responseBody = { 1, 2, 3, 0, 4 };
        new NonStrictExpectations()
        {
            {
                mockUrl.getProtocol();
                result = "https";
                mockConn.readInput();
                result = responseBody;
            }
        };

        HttpsRequest request =
                new HttpsRequest(mockUrl, httpsMethod, requestBody);
        HttpsResponse response = request.send();
        byte[] testBody = response.getBody();

        final byte[] expectedBody = responseBody;
        assertThat(testBody, is(expectedBody));
    }

    // Tests_SRS_HTTPSREQUEST_11_009: [The function shall return the HTTPS response received, including the status code, body, header fields, and error reason (if any).]
    // Tests_SRS_HTTPSREQUEST_11_012: [If an I/O exception occurs because of a bad response status code, the function shall attempt to flush or read the error stream so that the underlying HTTPS connection can be reused.]
    @Test
    public void sendReturnsError(@Mocked final HttpsConnection mockConn) throws IOException
    {
        final HttpsMethod httpsMethod = HttpsMethod.GET;
        final byte[] body = new byte[0];
        final byte[] error = { 5, 6, 7, 0, 1 };
        new NonStrictExpectations()
        {
            {
                mockUrl.getProtocol();
                result = "https";
                mockConn.connect();
                result = new IOException();
                mockConn.readError();
                result = error;
            }
        };

        HttpsRequest request =
                new HttpsRequest(mockUrl, httpsMethod, body);
        HttpsResponse response = request.send();
        byte[] testError = response.getErrorReason();

        final byte[] expectedError = error;
        assertThat(testError, is(expectedError));
    }

    // Tests_SRS_HTTPSREQUEST_11_009: [The function shall return the HTTPS response received, including the status code, body, header fields, and error reason (if any).]
    @Test
    public void sendReturnsHeaderFields(@Mocked final HttpsConnection mockConn) throws IOException
    {
        final Map<String, List<String>> headerFields = new HashMap<>();
        final String field = "test-field";
        final List<String> values = new LinkedList<>();
        final String value0 = "test-value0";
        final String value1 = "test-value1";
        values.add(value0);
        values.add(value1);
        headerFields.put(field, values);
        final HttpsMethod httpsMethod = HttpsMethod.POST;
        final byte[] body = new byte[0];
        new NonStrictExpectations()
        {
            {
                mockUrl.getProtocol();
                result = "https";
                mockConn.getResponseHeaders();
                result = headerFields;
            }
        };

        HttpsRequest request =
                new HttpsRequest(mockUrl, httpsMethod, body);
        HttpsResponse response = request.send();
        String testValues = response.getHeaderField(field);

        final String expectedValues = value0 + "," + value1;
        assertThat(testValues, is(expectedValues));
    }

    // Tests_SRS_HTTPSREQUEST_11_009: [The function shall return the HTTPS response received, including the status code, body, header fields, and error reason (if any).]
    @Test
    public void sendReturnsStatusCodeOnBadStatusException(@Mocked final HttpsConnection mockConn) throws IOException
    {
        final HttpsMethod httpsMethod = HttpsMethod.POST;
        final byte[] body = new byte[0];
        final int badStatus = 404;
        new NonStrictExpectations()
        {
            {
                mockUrl.getProtocol();
                result = "https";
                mockConn.connect();
                result = new IOException();
                mockConn.getResponseStatus();
                result = badStatus;
            }
        };

        HttpsRequest request =
                new HttpsRequest(mockUrl, httpsMethod, body);
        HttpsResponse response = request.send();
        int testStatus = response.getStatus();

        final int expectedStatus = badStatus;
        assertThat(testStatus, is(expectedStatus));
    }

    // Tests_SRS_HTTPSREQUEST_11_009: [The function shall return the HTTPS response received, including the status code, body, header fields, and error reason (if any).]
    @Test
    public void sendReturnsHeaderFieldsOnBadStatusException(@Mocked final HttpsConnection mockConn) throws IOException
    {
        final Map<String, List<String>> headerFields = new HashMap<>();
        final String field = "test-field";
        final List<String> values = new LinkedList<>();
        final String value = "test-value0";
        values.add(value);
        headerFields.put(field, values);
        final HttpsMethod httpsMethod = HttpsMethod.POST;
        final byte[] body = new byte[0];
        new NonStrictExpectations()
        {
            {
                mockUrl.getProtocol();
                result = "https";
                mockConn.connect();
                result = new IOException();
                mockConn.getResponseHeaders();
                result = headerFields;
            }
        };

        HttpsRequest request =
                new HttpsRequest(mockUrl, httpsMethod, body);
        HttpsResponse response = request.send();
        String testValues = response.getHeaderField(field);

        final String expectedValues = value;
        assertThat(testValues, is(expectedValues));
    }

    // Tests_SRS_HTTPSREQUEST_11_011: [If the client cannot connect to the server, the function shall throw an IOException.]
    @Test(expected = IOException.class)
    public void sendThrowsIoExceptionIfCannotConnect(@Mocked final HttpsConnection mockConn) throws IOException
    {
        final HttpsMethod httpsMethod = HttpsMethod.POST;
        final byte[] body = new byte[0];
        new NonStrictExpectations()
        {
            {
                mockUrl.getProtocol();
                result = "https";
                mockConn.connect();
                result = new IOException();
                mockConn.getResponseHeaders();
                result = new IOException();
                mockConn.getResponseStatus();
                result = new IOException();
                mockConn.readInput();
                result = new IOException();
                mockConn.readError();
                result = new IOException();
            }
        };

        HttpsRequest request =
                new HttpsRequest(mockUrl, httpsMethod, body);
        request.send();
    }

    // Tests_SRS_HTTPSREQUEST_11_013: [The function shall set the header field with the given name to the given value.]
    @Test
    public void setHeaderFieldSetsHeaderField(@Mocked final HttpsConnection mockConn) throws IOException
    {
        final HttpsMethod httpsMethod = HttpsMethod.POST;
        final byte[] body = new byte[0];
        final String field = "test-field";
        final String value = "test-value";
        new NonStrictExpectations()
        {
            {
                mockUrl.getProtocol();
                result = "https";
            }
        };

        HttpsRequest request =
                new HttpsRequest(mockUrl, httpsMethod, body);
        request.setHeaderField(field, value);

        new Verifications()
        {
            {
                mockConn.setRequestHeader(field, value);
            }
        };
    }

    // Tests_SRS_HTTPSREQUEST_11_014: [The function shall set the read timeout for the request to the given value.]
    @Test
    public void setReadTimeoutSetsReadTimeout(@Mocked final HttpsConnection mockConn) throws IOException
    {
        final HttpsMethod httpsMethod = HttpsMethod.POST;
        final byte[] body = new byte[0];
        final int readTimeout = 1;
        new NonStrictExpectations()
        {
            {
                mockUrl.getProtocol();
                result = "https";
            }
        };

        HttpsRequest request =
                new HttpsRequest(mockUrl, httpsMethod, body);
        request.setReadTimeoutMillis(readTimeout);

        final int expectedReadTimeout = readTimeout;
        new Verifications()
        {
            {
                mockConn.setReadTimeoutMillis(expectedReadTimeout);
            }
        };
    }
}
