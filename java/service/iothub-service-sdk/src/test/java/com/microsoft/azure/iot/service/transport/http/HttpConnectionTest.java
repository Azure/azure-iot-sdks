/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.transport.http;

import mockit.Mocked;
import mockit.NonStrictExpectations;
import mockit.Verifications;
import mockit.integration.junit4.JMockit;
import org.junit.Test;
import org.junit.runner.RunWith;

import javax.net.ssl.HttpsURLConnection;
import java.io.IOException;
import java.io.InputStream;
import java.net.URL;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

import static org.hamcrest.CoreMatchers.is;
import static org.junit.Assert.assertThat;

/** Unit tests for HttpConnection. */
@RunWith(JMockit.class)
public class HttpConnectionTest
{
    @Mocked
    URL mockUrl;
    @Mocked
    HttpsURLConnection mockUrlConn;

    // Tests_SRS_SERVICE_SDK_JAVA_HTTPSCONNECTION_12_001: [The constructor shall open a connection to the given URL.]
    @Test
    public void constructorOpensConnection() throws IOException
    {
        // Arrange
        final HttpMethod httpMethod = HttpMethod.PUT;
        new NonStrictExpectations()
        {
            {
                mockUrl.getProtocol();
                result = "https";
                mockUrl.openConnection();
                result = mockUrlConn;
            }
        };
        // Act
        new HttpConnection(mockUrl, httpMethod);
        // Assert
        new Verifications()
        {
            {
                mockUrl.openConnection();
            }
        };
    }

    // Tests_SRS_SERVICE_SDK_JAVA_HTTPSCONNECTION_12_002: [The constructor shall throw an IOException if the connection was unable to be opened.]
    // Assert
    @Test(expected = IOException.class)
    public void constructorThrowsIoExceptionIfCannotOpenConnection()
            throws IOException
    {
        // Arrange
        final HttpMethod httpsMethod = HttpMethod.PUT;
        new NonStrictExpectations()
        {
            {
                mockUrl.getProtocol();
                result = "https";
                mockUrl.openConnection();
                result = new IOException();
            }
        };
        // Act
        new HttpConnection(mockUrl, httpsMethod);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_HTTPSCONNECTION_12_003: [The constructor shall set the HTTPS method to the given method.]
    @Test
    public void constructorSetsRequestMethod() throws IOException
    {
        // Arrange
        final HttpMethod httpsMethod = HttpMethod.PUT;
        new NonStrictExpectations()
        {
            {
                mockUrl.getProtocol();
                result = "https";
                mockUrl.openConnection();
                result = mockUrlConn;
                mockUrlConn.getRequestMethod();
                result = httpsMethod.name();
            }
        };
        // Act
        new HttpConnection(mockUrl, httpsMethod);
        // Assert
        new Verifications()
        {
            {
                mockUrl.openConnection();
            }
        };
    }

    // Tests_SRS_SERVICE_SDK_JAVA_HTTPSCONNECTION_12_004: [If the URI given does not use the HTTPS protocol, the constructor shall throw an IllegalArgumentException.]
    // Assert
    @Test(expected = IllegalArgumentException.class)
    public void constructorRejectsNonHttpsUrl() throws IOException
    {
        // Arrange
        final HttpMethod httpsMethod = HttpMethod.PUT;
        new NonStrictExpectations()
        {
            {
                mockUrl.getProtocol();
                result = "http";
                mockUrl.openConnection();
                result = mockUrlConn;
                mockUrlConn.getRequestMethod();
                result = httpsMethod.name();
            }
        };
        // Act
        new HttpConnection(mockUrl, httpsMethod);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_HTTPSCONNECTION_12_005: [The function shall send a request to the URL given in the constructor.]
    @Test
    public void connectUsesCorrectUrl() throws IOException
    {
        // Arrange
        final HttpMethod httpsMethod = HttpMethod.PUT;
        new NonStrictExpectations()
        {
            {
                mockUrl.getProtocol();
                result = "https";
                mockUrl.openConnection();
                result = mockUrlConn;
                mockUrlConn.getRequestMethod();
                result = httpsMethod.name();
            }
        };
        // Act
        HttpConnection conn = new HttpConnection(mockUrl, httpsMethod);
        conn.connect();
        // Assert
        new Verifications()
        {
            {
                mockUrl.openConnection().connect();
            }
        };
    }

    // Tests_SRS_SERVICE_SDK_JAVA_HTTPSCONNECTION_12_006: [The function shall stream the request body, if present, through the connection.]
    @Test
    public void connectStreamsRequestBody() throws IOException
    {
        // Arrange
        final HttpMethod httpsMethod = HttpMethod.PUT;
        byte[] body = { 1, 2, 3 };
        final byte[] expectedBody = { 1, 2, 3 };
        new NonStrictExpectations()
        {
            {
                mockUrl.getProtocol();
                result = "https";
                mockUrl.openConnection();
                result = mockUrlConn;
                mockUrlConn.getRequestMethod();
                result = httpsMethod.name();
            }
        };
        // Act
        HttpConnection conn = new HttpConnection(mockUrl, httpsMethod);
        conn.writeOutput(body);
        body[0] = 5;
        conn.connect();
        // Assert
        new Verifications()
        {
            {
                mockUrl.openConnection().getOutputStream().write(expectedBody);
            }
        };
    }

    // Tests_SRS_SERVICE_SDK_JAVA_HTTPSCONNECTION_12_007: [The function shall throw an IOException if the connection could not be established, or the server responded with a bad status code.]
    // Assert
    @Test(expected = IOException.class)
    public void connectThrowsIoExceptionIfCannotConnect() throws IOException
    {
        // Arrange
        final HttpMethod httpsMethod = HttpMethod.PUT;
        new NonStrictExpectations()
        {
            {
                mockUrl.getProtocol();
                result = "https";
                mockUrl.openConnection();
                result = mockUrlConn;
                mockUrlConn.getRequestMethod();
                result = httpsMethod.name();
                mockUrlConn.connect();
                result = new IOException();
            }
        };
        // Act
        HttpConnection conn = new HttpConnection(mockUrl, httpsMethod);
        conn.connect();
    }

    // Tests_SRS_SERVICE_SDK_JAVA_HTTPSCONNECTION_12_008: [The function shall set the request method.]
    @Test
    public void setRequestMethodSetsRequestMethod() throws IOException
    {
        // Arrange
        final HttpMethod httpsMethod = HttpMethod.PUT;
        new NonStrictExpectations()
        {
            {
                mockUrl.getProtocol();
                result = "https";
                mockUrl.openConnection();
                result = mockUrlConn;
                mockUrlConn.getRequestMethod();
                result = httpsMethod.name();
            }
        };
        // Act
        HttpConnection conn = new HttpConnection(mockUrl, httpsMethod);
        conn.setRequestMethod(httpsMethod);
        // Assert
        new Verifications()
        {
            {
                ((HttpsURLConnection) mockUrl.openConnection())
                        .setRequestMethod(httpsMethod.name());
            }
        };
    }

    // Tests_SRS_SERVICE_SDK_JAVA_HTTPSCONNECTION_12_009: [The function shall throw an IllegalArgumentException if the request currently has a non-empty body and the new method is not a POST or a PUT.]
    // Assert
    @Test(expected = IllegalArgumentException.class)
    public void setRequestMethodRejectsNonPostOrPutIfHasBody() throws IOException
    {
        // Arrange
        final HttpMethod httpsMethod = HttpMethod.POST;
        final HttpMethod illegalHttpsMethod = HttpMethod.DELETE;
        final byte[] body = { 1, 2, 3 };
        new NonStrictExpectations()
        {
            {
                mockUrl.getProtocol();
                result = "https";
                mockUrl.openConnection();
                result = mockUrlConn;
                mockUrlConn.getRequestMethod();
                returns(httpsMethod.name(), illegalHttpsMethod.name());
            }
        };
        HttpConnection conn = new HttpConnection(mockUrl, httpsMethod);
        conn.writeOutput(body);
        // Act
        conn.setRequestMethod(illegalHttpsMethod);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_HTTPSCONNECTION_12_010: [The function shall set the given request header field.]
    @Test
    public void setRequestHeaderSetsRequestHeader() throws IOException
    {
        // Arrange
        final HttpMethod httpsMethod = HttpMethod.POST;
        final String field = "test-field";
        final String value = "test-value";
        new NonStrictExpectations()
        {
            {
                mockUrl.getProtocol();
                result = "https";
                mockUrl.openConnection();
                result = mockUrlConn;
                mockUrlConn.getRequestMethod();
                result = httpsMethod.name();
            }
        };
        HttpConnection conn = new HttpConnection(mockUrl, httpsMethod);
        // Act
        conn.setRequestHeader(field, value);
        // Assert
        new Verifications()
        {
            {
                mockUrl.openConnection().setRequestProperty(field, value);
            }
        };
    }

    // Tests_SRS_SERVICE_SDK_JAVA_HTTPSCONNECTION_12_011: [The function shall set the read timeout to the given value.]
    @Test
    public void setReadTimeoutSetsRequestTimeout() throws IOException
    {
        // Arrange
        final HttpMethod httpsMethod = HttpMethod.POST;
        final String field = "test-field";
        final String value = "test-value";
        final int timeout = 1;
        final int expectedTimeout = timeout;
        new NonStrictExpectations()
        {
            {
                mockUrl.getProtocol();
                result = "https";
                mockUrl.openConnection();
                result = mockUrlConn;
                mockUrlConn.getRequestMethod();
                result = httpsMethod.name();
            }
        };
        HttpConnection conn = new HttpConnection(mockUrl, httpsMethod);
        // Act
        conn.setReadTimeoutMillis(timeout);
        // Assert
        new Verifications()
        {
            {
                mockUrl.openConnection().setReadTimeout(expectedTimeout);
            }
        };
    }

    // Tests_SRS_SERVICE_SDK_JAVA_HTTPSCONNECTION_12_012: [The function shall save the body to be sent with the request.]
    // Assert
    @Test(expected = IllegalArgumentException.class)
    public void writeOutputFailsWhenMethodIsNotPostOrPut() throws IOException
    {
        // Arrange
        final HttpMethod httpsMethod = HttpMethod.GET;
        final byte[] body = { 1, 2 };
        new NonStrictExpectations()
        {
            {
                mockUrl.getProtocol();
                result = "https";
                mockUrl.openConnection();
                result = mockUrlConn;
                mockUrlConn.getRequestMethod();
                result = httpsMethod.name();
            }
        };
        HttpConnection conn = new HttpConnection(mockUrl, httpsMethod);
        // Act
        conn.writeOutput(body);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_HTTPSCONNECTION_12_013: [The function shall throw an IllegalArgumentException if the request does not currently use method POST or PUT and the body is non-empty.]
    @Test
    public void writeOutputDoesNotFailWhenBodyIsEmpty() throws IOException
    {
        // Arrange
        final HttpMethod httpsMethod = HttpMethod.GET;
        final byte[] body = new byte[0];
        // Assert
        new NonStrictExpectations()
        {
            {
                mockUrl.getProtocol();
                result = "https";
                mockUrl.openConnection();
                result = mockUrlConn;
                mockUrlConn.getRequestMethod();
                result = httpsMethod.name();
            }
        };
        HttpConnection conn = new HttpConnection(mockUrl, httpsMethod);
        // Act
        conn.writeOutput(body);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_HTTPSCONNECTION_12_014: [The function shall read from the input stream (response stream) and return the response.]
    @Test
    public void readInputCompletelyReadsInputStream(@Mocked final InputStream mockIs) throws IOException
    {
        // Arrange
        final HttpMethod httpsMethod = HttpMethod.GET;
        byte[] expectedResponse = { 1, 2, 3 };
        new NonStrictExpectations()
        {
            {
                mockUrl.getProtocol();
                result = "https";
                mockUrl.openConnection();
                result = mockUrlConn;
                mockUrlConn.getRequestMethod();
                result = httpsMethod.name();
                mockUrlConn.getInputStream();
                result = mockIs;
                mockIs.read();
                returns(1, 2, 3, -1);
            }
        };
        HttpConnection conn = new HttpConnection(mockUrl, httpsMethod);
        conn.connect();
        // Act
        byte[] testResponse = conn.readInput();
        // Assert
        assertThat(testResponse, is(expectedResponse));
    }

    // Tests_SRS_SERVICE_SDK_JAVA_HTTPSCONNECTION_12_015: [The function shall throw an IOException if the input stream could not be accessed.]
    // Assert
    @Test(expected = IOException.class)
    public void readInputFailsIfCannotAccessInputStream() throws IOException
    {
        // Arrange
        final HttpMethod httpsMethod = HttpMethod.GET;
        new NonStrictExpectations()
        {
            {
                mockUrl.getProtocol();
                result = "https";
                mockUrl.openConnection();
                result = mockUrlConn;
                mockUrlConn.getRequestMethod();
                result = httpsMethod.name();
                mockUrlConn.getInputStream();
                result = new IOException();
            }
        };
        HttpConnection conn = new HttpConnection(mockUrl, httpsMethod);
        conn.connect();
        // Act
        conn.readInput();
    }

    // Tests_SRS_SERVICE_SDK_JAVA_HTTPSCONNECTION_12_016: [The function shall close the input stream after it has been completely read.]
    @Test
    public void readInputClosesInputStream(@Mocked final InputStream mockIs) throws IOException
    {
        // Arrange
        final HttpMethod httpsMethod = HttpMethod.GET;
        new NonStrictExpectations()
        {
            {
                mockUrl.getProtocol();
                result = "https";
                mockUrl.openConnection();
                result = mockUrlConn;
                mockUrlConn.getRequestMethod();
                result = httpsMethod.name();
                mockUrlConn.getInputStream();
                result = mockIs;
                mockIs.read();
                result = -1;
            }
        };
        HttpConnection conn = new HttpConnection(mockUrl, httpsMethod);
        conn.connect();
        // Act
        conn.readInput();
        // Assert
        new Verifications()
        {
            {
                mockIs.close();
            }
        };
    }

    // Tests_SRS_SERVICE_SDK_JAVA_HTTPSCONNECTION_12_017: [The function shall read from the error stream and return the response.]
    @Test
    public void readErrorCompletelyReadsErrorStream(@Mocked final InputStream mockIs) throws IOException
    {
        // Arrange
        final HttpMethod httpsMethod = HttpMethod.GET;
        byte[] expectedError = { 1, 2, 3 };
        new NonStrictExpectations()
        {
            {
                mockUrl.getProtocol();
                result = "https";
                mockUrl.openConnection();
                result = mockUrlConn;
                mockUrlConn.getRequestMethod();
                result = httpsMethod.name();
                mockUrlConn.getErrorStream();
                result = mockIs;
                mockIs.read();
                returns(1, 2, 3, -1);
            }
        };
        HttpConnection conn = new HttpConnection(mockUrl, httpsMethod);
        conn.connect();
        // Act
        byte[] testError = conn.readError();
        // Assert
        assertThat(testError, is(expectedError));
    }

    // Tests_SRS_SERVICE_SDK_JAVA_HTTPSCONNECTION_12_017: [The function shall read from the error stream and return the response.]
    @Test
    public void readErrorReturnsEmptyErrorReasonIfNoErrorReason() throws IOException
    {
        // Arrange
        final HttpMethod httpsMethod = HttpMethod.GET;
        byte[] expectedError = {};
        new NonStrictExpectations()
        {
            {
                mockUrl.getProtocol();
                result = "https";
                mockUrl.openConnection();
                result = mockUrlConn;
                mockUrlConn.getRequestMethod();
                result = httpsMethod.name();
                mockUrlConn.getErrorStream();
                result = null;
            }
        };
        HttpConnection conn = new HttpConnection(mockUrl, httpsMethod);
        conn.connect();
        // Act
        byte[] testError = conn.readError();
        // Assert
        assertThat(testError, is(expectedError));
    }

    // Tests_SRS_SERVICE_SDK_JAVA_HTTPSCONNECTION_12_018: [The function shall throw an IOException if the error stream could not be accessed.]
    // Assert
    @Test(expected = IOException.class)
    public void readErrorFailsIfCannotAccessErrorStream() throws IOException
    {
        // Arrange
        final HttpMethod httpsMethod = HttpMethod.GET;
        new NonStrictExpectations()
        {
            {
                mockUrl.getProtocol();
                result = "https";
                mockUrl.openConnection();
                result = mockUrlConn;
                mockUrlConn.getRequestMethod();
                result = httpsMethod.name();
                mockUrlConn.getErrorStream();
                result = new IOException();
            }
        };
        HttpConnection conn = new HttpConnection(mockUrl, httpsMethod);
        conn.connect();
        // Act
        conn.readError();
    }

    // Tests_SRS_SERVICE_SDK_JAVA_HTTPSCONNECTION_12_019: [The function shall close the error stream after it has been completely read.]
    @Test
    public void readErrorClosesErrorStream(@Mocked final InputStream mockIs) throws IOException
    {
        // Arrange
        final HttpMethod httpsMethod = HttpMethod.GET;
        new NonStrictExpectations()
        {
            {
                mockUrl.getProtocol();
                result = "https";
                mockUrl.openConnection();
                result = mockUrlConn;
                mockUrlConn.getRequestMethod();
                result = httpsMethod.name();
                mockUrlConn.getErrorStream();
                result = mockIs;
                mockIs.read();
                result = -1;
            }
        };
        HttpConnection conn = new HttpConnection(mockUrl, httpsMethod);
        conn.connect();
        // Act
        conn.readError();
        // Assert
        new Verifications()
        {
            {
                mockIs.close();
            }
        };
    }

    // Tests_SRS_SERVICE_SDK_JAVA_HTTPSCONNECTION_12_020: [The function shall return the response status code.]
    @Test
    public void getResponseStatusReturnsResponseStatus(@Mocked final InputStream mockIs) throws IOException
    {
        // Arrange
        final HttpMethod httpsMethod = HttpMethod.GET;
        final int status = 204;
        int expectedStatus = status;
        new NonStrictExpectations()
        {
            {
                mockUrl.getProtocol();
                result = "https";
                mockUrl.openConnection();
                result = mockUrlConn;
                mockUrlConn.getRequestMethod();
                result = httpsMethod.name();
                mockUrlConn.getResponseCode();
                result = status;
            }
        };
        HttpConnection conn = new HttpConnection(mockUrl, httpsMethod);
        conn.connect();
        // Act
        int testStatus = conn.getResponseStatus();
        // Assert
        assertThat(testStatus, is(expectedStatus));
    }

    // Tests_SRS_SERVICE_SDK_JAVA_HTTPSCONNECTION_12_021: [The function shall throw an IOException if no response was received.]
    // Assert
    @Test(expected = IOException.class)
    public void getResponseStatusFailsIfDidNotReceiveResponse(@Mocked final InputStream mockIs) throws IOException
    {
        // Arrange
        final HttpMethod httpsMethod = HttpMethod.GET;
        new NonStrictExpectations()
        {
            {
                mockUrl.getProtocol();
                result = "https";
                mockUrl.openConnection();
                result = mockUrlConn;
                mockUrlConn.getRequestMethod();
                result = httpsMethod.name();
                mockUrlConn.getResponseCode();
                result = new IOException();
            }
        };
        HttpConnection conn = new HttpConnection(mockUrl, httpsMethod);
        conn.connect();
        // Act
        conn.getResponseStatus();
    }

    // Tests_SRS_SERVICE_SDK_JAVA_HTTPSCONNECTION_12_022: [The function shall return a mapping of header field names to the values associated with the header field name.]
    @Test
    public void getResponseHeadersReturnsResponseHeaders() throws IOException
    {
        // Arrange
        final HttpMethod httpsMethod = HttpMethod.GET;
        final String field0 = "test-field0";
        final String value0 = "test-value0";
        final String field1 = "test-field1";
        final String value1 = "test-value1";
        final List<String> values0 = new LinkedList<>();
        values0.add(value0);
        final List<String> values1 = new LinkedList<>();
        values1.add(value1);
        final Map<String, List<String>> responseHeaders = new HashMap<>();
        responseHeaders.put(field0, values0);
        responseHeaders.put(field1, values1);
        final Map<String, List<String>> expectedResponseHeaders = responseHeaders;
        new NonStrictExpectations()
        {
            {
                mockUrl.getProtocol();
                result = "https";
                mockUrl.openConnection();
                result = mockUrlConn;
                mockUrlConn.getRequestMethod();
                result = httpsMethod.name();
                mockUrlConn.getHeaderFields();
                result = responseHeaders;
            }
        };
        HttpConnection conn = new HttpConnection(mockUrl, httpsMethod);
        conn.connect();

        // Act
        Map<String, List<String>> testResponseHeaders = conn.getResponseHeaders();
        // Assert
        assertThat(testResponseHeaders.size(),
                is(expectedResponseHeaders.size()));
        // the list of values for each field is of size 1, so the lists
        // can be directly compared.
        assertThat(testResponseHeaders.get(field0),is(expectedResponseHeaders.get(field0)));
        assertThat(testResponseHeaders.get(field1),is(expectedResponseHeaders.get(field1)));
    }

    // Tests_SRS_SERVICE_SDK_JAVA_HTTPSCONNECTION_12_023: [The function shall throw an IOException if no response was received.]
    // Assert
    @Test(expected = IOException.class)
    public void getResponseHeadersFailsIfDidNotReceiveResponse() throws IOException
    {
        // Arrange
        final HttpMethod httpsMethod = HttpMethod.GET;
        new NonStrictExpectations()
        {
            {
                mockUrl.getProtocol();
                result = "https";
                mockUrl.openConnection();
                result = mockUrlConn;
                mockUrlConn.getRequestMethod();
                result = httpsMethod.name();
                mockUrlConn.getHeaderFields();
                result = new IOException();
            }
        };
        HttpConnection conn = new HttpConnection(mockUrl, httpsMethod);
        conn.connect();
        // Act
        conn.getResponseHeaders();
    }
}
