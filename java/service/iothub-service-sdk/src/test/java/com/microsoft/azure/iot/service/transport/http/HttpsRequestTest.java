/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.transport.http;

import mockit.*;
import mockit.integration.junit4.JMockit;
import org.junit.Test;
import org.junit.runner.RunWith;

import java.io.IOException;
import java.net.URL;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

import static org.hamcrest.CoreMatchers.is;
import static org.junit.Assert.assertThat;

/** Unit tests for HttpRequest. */
@RunWith(JMockit.class)
public class HttpsRequestTest
{
    @Mocked URL mockUrl;

    // Tests_SRS_SERVICE_SDK_JAVA_HTTPSREQUEST_12_001: [The function shall open a connection with the given URL as the endpoint.]
    @Test
    public void constructorOpensConnection(@Mocked final HttpConnection mockConn) throws IOException
    {
        // Arrange
        final HttpMethod httpsMethod = HttpMethod.GET;
        final byte[] body = new byte[0];
        new NonStrictExpectations()
        {
            {
                mockUrl.getProtocol();
                result = "http";
            }
        };
        // Act
        new HttpRequest(mockUrl, httpsMethod, body);
        // Assert
        new Verifications()
        {
            {
                new HttpConnection(mockUrl, (HttpMethod) any);
            }
        };
    }

    // Tests_SRS_SERVICE_SDK_JAVA_HTTPSREQUEST_12_002: [The function shall write the body to the connection.]
    @Test
    public void constructorWritesBodyToConnection(@Mocked final HttpConnection mockConn) throws IOException
    {
        // Arrange
        final HttpMethod httpsMethod = HttpMethod.GET;
        final byte[] body = { 1, 2, 3 };
        final byte[] expectedBody = body;
        new NonStrictExpectations()
        {
            {
                mockUrl.getProtocol();
                result = "http";
            }
        };
        // Act
        new HttpRequest(mockUrl, httpsMethod, body);
        // Assert
        new Verifications()
        {
            {
                new HttpConnection(mockUrl, (HttpMethod) any)
                        .writeOutput(expectedBody);
            }
        };
    }

    // Tests_SRS_SERVICE_SDK_JAVA_HTTPSREQUEST_12_003: [The function shall use the given HTTPS method (i.e. GET) as the request method.]
    @Test
    public void constructorSetsHttpsMethodCorrectly(@Mocked final HttpConnection mockConn) throws IOException
    {
        // Arrange
        final HttpMethod httpsMethod = HttpMethod.GET;
        final byte[] body = new byte[0];
        new NonStrictExpectations()
        {
            {
                mockUrl.getProtocol();
                result = "http";
            }
        };
        // Act
        new HttpRequest(mockUrl, httpsMethod, body);
        // Assert
        new Verifications()
        {
            {
                new HttpConnection((URL) any, httpsMethod);
            }
        };
    }

    // Tests_SRS_SERVICE_SDK_JAVA_HTTPSREQUEST_12_004: [If an IOException occurs in setting up the HTTPS connection, the function shall throw an IOException.]
    // Assert
    @Test(expected = IOException.class)
    public void constructorThrowsIoExceptionIfCannotSetupConnection(@Mocked final HttpConnection mockConn) throws IOException
    {
        // Arrange
        final HttpMethod httpsMethod = HttpMethod.GET;
        final byte[] body = new byte[0];
        new NonStrictExpectations()
        {
            {
                mockUrl.getProtocol();
                result = "http";
                new HttpConnection(mockUrl, httpsMethod);
                result = new IOException();
            }
        };
        // Act
        new HttpRequest(mockUrl, httpsMethod, body);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_HTTPSREQUEST_12_005: [The function shall send an HTTPS request as formatted in the constructor.]
    @Test
    public void sendHasCorrectHttpsMethod() throws IOException
    {
        // Arrange
        final HttpMethod expectedMethod = HttpMethod.GET;
        final byte[] body = new byte[0];
        new MockUp<HttpConnection>()
        {
            HttpMethod testMethod;

            @Mock
            public void $init(URL url, HttpMethod method)
            {
                this.testMethod = method;
            }

            @Mock
            public void connect() throws IOException
            {
                assertThat(testMethod, is(expectedMethod));
            }

            @Mock
            public void setRequestMethod(HttpMethod method)
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
        // Assert
        new NonStrictExpectations()
        {
            {
                mockUrl.getProtocol();
                result = "http";
            }
        };
        HttpRequest request = new HttpRequest(mockUrl, expectedMethod, body);
        // Act
        request.send();
    }

    // Tests_SRS_SERVICE_SDK_JAVA_HTTPSREQUEST_12_005: [The function shall send an HTTPS request as formatted in the constructor.]
    @Test
    public void sendSetsHeaderFieldsCorrectly() throws IOException
    {
        // Arrange
        final HttpMethod expectedMethod = HttpMethod.GET;
        final byte[] body = new byte[0];
        final String field0 = "test-field0";
        final String value0 = "test-value0";
        final String field1 = "test-field1";
        final String value1 = "test-value1";
        new MockUp<HttpConnection>()
        {
            Map<String, String> testHeaderFields = new HashMap<>();

            @Mock
            public void $init(URL url, HttpMethod method)
            {
            }

            @Mock
            public void connect() throws IOException
            {
                assertThat(testHeaderFields.size(), is(2));
                assertThat(testHeaderFields.get(field0), is(value0));
                assertThat(testHeaderFields.get(field1), is(value1));
            }

            @Mock
            public void setRequestHeader(String field, String value)
            {
                testHeaderFields.put(field, value);
            }

            // every method that is used must be manually mocked.
            @Mock
            public void setRequestMethod(HttpMethod method)
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
        // Assert
        new NonStrictExpectations()
        {
            {
                mockUrl.getProtocol();
                result = "http";
            }
        };
        // Act
        HttpRequest request = new HttpRequest(mockUrl, expectedMethod, body);
        request.setHeaderField(field0, value0);
        request.setHeaderField(field1, value1);
        request.send();
    }

    // Tests_SRS_SERVICE_SDK_JAVA_HTTPSREQUEST_12_005: [The function shall send an HTTPS request as formatted in the constructor.]
    @Test
    public void sendWritesBodyToOutputStream() throws IOException
    {
        // Arrange
        final HttpMethod httpsMethod = HttpMethod.POST;
        final byte[] expectedBody = { 1, 2, 3 };
        new MockUp<HttpConnection>()
        {
            byte[] testBody;

            @Mock
            public void $init(URL url, HttpMethod method)
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
            public void setRequestMethod(HttpMethod method)
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
        // Assert
        new NonStrictExpectations()
        {
            {
                mockUrl.getProtocol();
                result = "http";
            }
        };
        // Act
        HttpRequest request = new HttpRequest(mockUrl, httpsMethod, expectedBody);
        request.send();
    }

    // Tests_SRS_SERVICE_SDK_JAVA_HTTPSREQUEST_12_006: [The function shall return the HTTPS response received, including the status code, body, header fields, and error reason (if any).]
    @Test
    public void sendReadsStatusCode(@Mocked final HttpConnection mockConn) throws IOException
    {
        // Arrange
        final HttpMethod httpsMethod = HttpMethod.GET;
        final byte[] body = new byte[0];
        final int status = 204;
        final int expectedStatus = status;
        new NonStrictExpectations()
        {
            {
                mockUrl.getProtocol();
                result = "http";
                mockConn.getResponseStatus();
                result = status;
            }
        };
        // Act
        HttpRequest request = new HttpRequest(mockUrl, httpsMethod, body);
        HttpResponse response = request.send();
        int testStatus = response.getStatus();
        // Assert
        assertThat(testStatus, is(expectedStatus));
    }

    // Tests_SRS_SERVICE_SDK_JAVA_HTTPSREQUEST_12_006: [The function shall return the HTTPS response received, including the status code, body, header fields, and error reason (if any).]
    @Test
    public void sendReturnsBody(@Mocked final HttpConnection mockConn) throws IOException
    {
        // Arrange
        final HttpMethod httpsMethod = HttpMethod.GET;
        final byte[] requestBody = new byte[0];
        final byte[] responseBody = { 1, 2, 3, 0, 4 };
        final byte[] expectedBody = responseBody;
        new NonStrictExpectations()
        {
            {
                mockUrl.getProtocol();
                result = "http";
                mockConn.readInput();
                result = responseBody;
            }
        };
        HttpRequest request = new HttpRequest(mockUrl, httpsMethod, requestBody);
        // Act
        HttpResponse response = request.send();
        byte[] testBody = response.getBody();
        // Assert
        assertThat(testBody, is(expectedBody));
    }

    // Tests_SRS_SERVICE_SDK_JAVA_HTTPSREQUEST_12_006: [The function shall return the HTTPS response received, including the status code, body, header fields, and error reason (if any).]
    // Tests_SRS_SERVICE_SDK_JAVA_HTTPSREQUEST_12_008: [If an I/O exception occurs because of a bad response status code, the function shall attempt to flush or read the error stream so that the underlying HTTPS connection can be reused.]
    @Test
    public void sendReturnsError(@Mocked final HttpConnection mockConn) throws IOException
    {
        // Arrange
        final HttpMethod httpsMethod = HttpMethod.GET;
        final byte[] body = new byte[0];
        final byte[] error = { 5, 6, 7, 0, 1 };
        final byte[] expectedError = error;
        new NonStrictExpectations()
        {
            {
                mockUrl.getProtocol();
                result = "http";
                mockConn.connect();
                result = new IOException();
                mockConn.readError();
                result = error;
            }
        };
        // Act
        HttpRequest request = new HttpRequest(mockUrl, httpsMethod, body);
        HttpResponse response = request.send();
        byte[] testError = response.getErrorReason();
        // Assert
        assertThat(testError, is(expectedError));
    }

    // Tests_SRS_SERVICE_SDK_JAVA_HTTPSREQUEST_12_006: [The function shall return the HTTPS response received, including the status code, body, header fields, and error reason (if any).]
    @Test
    public void sendReturnsHeaderFields(@Mocked final HttpConnection mockConn) throws IOException
    {
        // Arrange
        final Map<String, List<String>> headerFields = new HashMap<>();
        final String field = "test-field";
        final List<String> values = new LinkedList<>();
        final String value0 = "test-value0";
        final String value1 = "test-value1";
        values.add(value0);
        values.add(value1);
        headerFields.put(field, values);
        final String expectedValues = value0 + "," + value1;
        final HttpMethod httpsMethod = HttpMethod.POST;
        final byte[] body = new byte[0];
        new NonStrictExpectations()
        {
            {
                mockUrl.getProtocol();
                result = "http";
                mockConn.getResponseHeaders();
                result = headerFields;
            }
        };
        HttpRequest request = new HttpRequest(mockUrl, httpsMethod, body);
        // Act
        HttpResponse response = request.send();
        String testValues = response.getHeaderField(field);
        // Assert
        assertThat(testValues, is(expectedValues));
    }

    // Tests_SRS_SERVICE_SDK_JAVA_HTTPSREQUEST_12_006: [The function shall return the HTTPS response received, including the status code, body, header fields, and error reason (if any).]
    @Test
    public void sendReturnsStatusCodeOnBadStatusException(@Mocked final HttpConnection mockConn) throws IOException
    {
        // Arrange
        final HttpMethod httpsMethod = HttpMethod.POST;
        final byte[] body = new byte[0];
        final int badStatus = 404;
        final int expectedStatus = badStatus;
        new NonStrictExpectations()
        {
            {
                mockUrl.getProtocol();
                result = "http";
                mockConn.connect();
                result = new IOException();
                mockConn.getResponseStatus();
                result = badStatus;
            }
        };
        HttpRequest request = new HttpRequest(mockUrl, httpsMethod, body);
        // Act
        HttpResponse response = request.send();
        int testStatus = response.getStatus();
        // Assert
        assertThat(testStatus, is(expectedStatus));
    }

    // Tests_SRS_SERVICE_SDK_JAVA_HTTPSREQUEST_12_007: [If the client cannot connect to the server, the function shall throw an IOException.]
    // Assert
    @Test(expected = IOException.class)
    public void sendThrowsIoExceptionIfCannotConnect(@Mocked final HttpConnection mockConn) throws IOException
    {
        // Arrange
        final HttpMethod httpsMethod = HttpMethod.POST;
        final byte[] body = new byte[0];
        new NonStrictExpectations()
        {
            {
                mockUrl.getProtocol();
                result = "http";
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
        HttpRequest request = new HttpRequest(mockUrl, httpsMethod, body);
        // Act
        request.send();
    }

    // Tests_SRS_SERVICE_SDK_JAVA_HTTPSREQUEST_12_008: [If an I/O exception occurs because of a bad response status code, the function shall attempt to flush or read the error stream so that the underlying HTTPS connection can be reused.]
    @Test
    public void sendReturnsHeaderFieldsOnBadStatusException(@Mocked final HttpConnection mockConn) throws IOException
    {
        // Arrange
        final Map<String, List<String>> headerFields = new HashMap<>();
        final String field = "test-field";
        final List<String> values = new LinkedList<>();
        final String value = "test-value0";
        values.add(value);
        headerFields.put(field, values);
        final HttpMethod httpsMethod = HttpMethod.POST;
        final byte[] body = new byte[0];
        final String expectedValues = value;
        new NonStrictExpectations()
        {
            {
                mockUrl.getProtocol();
                result = "http";
                mockConn.connect();
                result = new IOException();
                mockConn.getResponseHeaders();
                result = headerFields;
            }
        };
        HttpRequest request = new HttpRequest(mockUrl, httpsMethod, body);
        // Act
        HttpResponse response = request.send();
        String testValues = response.getHeaderField(field);
        // Assert
        assertThat(testValues, is(expectedValues));
    }

    // Tests_SRS_SERVICE_SDK_JAVA_HTTPSREQUEST_12_009: [The function shall set the header field with the given name to the given value.]
    @Test
    public void setHeaderFieldSetsHeaderField(@Mocked final HttpConnection mockConn) throws IOException
    {
        // Arrange
        final HttpMethod httpsMethod = HttpMethod.POST;
        final byte[] body = new byte[0];
        final String field = "test-field";
        final String value = "test-value";
        new NonStrictExpectations()
        {
            {
                mockUrl.getProtocol();
                result = "http";
            }
        };
        HttpRequest request = new HttpRequest(mockUrl, httpsMethod, body);
        // Act
        request.setHeaderField(field, value);
        // Assert
        new Verifications()
        {
            {
                mockConn.setRequestHeader(field, value);
            }
        };
    }

    // Tests_SRS_SERVICE_SDK_JAVA_HTTPSREQUEST_12_010: [The function shall set the read timeout for the request to the given value.]
    @Test
    public void setReadTimeoutSetsReadTimeout(@Mocked final HttpConnection mockConn) throws IOException
    {
        // Arrange
        final HttpMethod httpsMethod = HttpMethod.POST;
        final byte[] body = new byte[0];
        final int readTimeout = 1;
        final int expectedReadTimeout = readTimeout;
        new NonStrictExpectations()
        {
            {
                mockUrl.getProtocol();
                result = "http";
            }
        };
        HttpRequest request = new HttpRequest(mockUrl, httpsMethod, body);
        // Act
        request.setReadTimeoutMillis(readTimeout);
        // Assert
        new Verifications()
        {
            {
                mockConn.setReadTimeoutMillis(expectedReadTimeout);
            }
        };
    }
}
