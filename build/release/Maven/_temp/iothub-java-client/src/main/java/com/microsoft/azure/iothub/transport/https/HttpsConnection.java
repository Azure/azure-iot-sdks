// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

package com.microsoft.azure.iothub.transport.https;

import java.io.IOException;
import java.io.InputStream;
import java.net.ProtocolException;
import java.net.URL;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Map;

import javax.net.ssl.HttpsURLConnection;

/**
 * <p>
 * A wrapper for the Java SE class {@link HttpsURLConnection}. Used to avoid
 * compatibility issues when testing with the mocking framework JMockit, as well
 * as to avoid some undocumented side effects when using HttpsURLConnection.
 * </p>
 * <p>
 * The underlying {@link HttpsURLConnection} is transparently managed by Java. To reuse
 * connections, for each time {@link #connect()} is called, the input streams (input
 * stream or error stream, if input stream is not accessible) must be completely
 * read. Otherwise, the data remains in the stream and the connection will not
 * be reusable.
 * </p>
 */
public class HttpsConnection
{
    /** The underlying HTTPS connection. */
    protected final HttpsURLConnection connection;

    /**
     * The body. {@link HttpsURLConnection} silently calls connect() when the output
     * stream is written to. We buffer the body and defer writing to the output
     * stream until {@link #connect()} is called.
     */
    protected byte[] body;

    /**
     * Constructor. Opens a connection to the given URL.
     *
     * @param url the URL for the HTTPS connection.
     * @param method the HTTPS method (i.e. GET).
     *
     * @throws IOException if the connection could not be opened.
     */
    public HttpsConnection(URL url, HttpsMethod method) throws IOException
    {
        // Codes_SRS_HTTPSCONNECTION_11_022: [If the URI given does not use the HTTPS protocol, the constructor shall throw an IllegalArgumentException.]
        String protocol = url.getProtocol();
        if (!protocol.equalsIgnoreCase("HTTPS"))
        {
            String errMsg = String.format("Expected URL that uses protocol "
                            + "HTTPS but received one that uses "
                            + "protocol '%s'.\n",
                    protocol);
            throw new IllegalArgumentException(errMsg);
        }

        // Codes_SRS_HTTPSCONNECTION_11_001: [The constructor shall open a connection to the given URL.]
        // Codes_SRS_HTTPSCONNECTION_11_002: [The constructor shall throw an IOException if the connection was unable to be opened.]
        this.connection = (HttpsURLConnection) url.openConnection();
        // Codes_SRS_HTTPSCONNECTION_11_021: [The constructor shall set the HTTPS method to the given method.]
        this.connection.setRequestMethod(method.name());
        this.body = new byte[0];
    }

    /**
     * Sends the request to the URL given in the constructor.
     *
     * @throws IOException if the connection could not be established, or the
     * server responded with a bad status code.
     */
    public void connect() throws IOException
    {
        // Codes_SRS_HTTPSCONNECTION_11_004: [The function shall stream the request body, if present, through the connection.]
        if (this.body.length > 0)
        {
            this.connection.setDoOutput(true);
            this.connection.getOutputStream().write(this.body);
        }
        // Codes_SRS_HTTPSCONNECTION_11_003: [The function shall send a request to the URL given in the constructor.]
        // Codes_SRS_HTTPSCONNECTION_11_005: [The function shall throw an IOException if the connection could not be established, or the server responded with a bad status code.]
        this.connection.connect();
    }

    /**
     * Sets the request method (i.e. POST).
     *
     * @param method the request method.
     *
     * @throws IllegalArgumentException if the request currently has a non-empty
     * body and the new method is not a POST or a PUT. This is because Java's
     * {@link HttpsURLConnection} silently converts the HTTPS method to POST or PUT if a
     * body is written to the request.
     */
    public void setRequestMethod(HttpsMethod method)
    {
        // Codes_SRS_HTTPSCONNECTION_11_007: [The function shall throw an IllegalArgumentException if the request currently has a non-empty body and the new method is not a POST or a PUT.]
        if (method != HttpsMethod.POST && method != HttpsMethod.PUT)
        {
            if (this.body.length > 0)
            {
                throw new IllegalArgumentException(
                        "Cannot change the request method from POST "
                        + "or PUT when the request body is non-empty.");
            }
        }

        // Codes_SRS_HTTPSCONNECTION_11_006: [The function shall set the request method.]
        try
        {
            this.connection.setRequestMethod(method.name());
        }
        catch (ProtocolException e)
        {
            // should never happen, since the method names are hard-coded.
        }
    }

    /**
     * Sets the request header field to the given value.
     *
     * @param field the header field name.
     * @param value the header field value.
     */
    public void setRequestHeader(String field, String value)
    {
        // Codes_SRS_HTTPSCONNECTION_11_008: [The function shall set the given request header field.]
        this.connection.setRequestProperty(field, value);
    }

    /**
     * Sets the read timeout in milliseconds. The read timeout is the number of
     * milliseconds after the server receives a request and before the server
     * sends data back.
     *
     * @param timeout the read timeout.
     */
    public void setReadTimeoutMillis(int timeout)
    {
        // Codes_SRS_HTTPSCONNECTION_11_023: [The function shall set the read timeout to the given value.]
        this.connection.setReadTimeout(timeout);
    }

    /**
     * Saves the body to be sent with the request.
     *
     * @param body the request body.
     *
     * @throws IllegalArgumentException if the request does not currently use
     * method POST or PUT and the body is non-empty. This is because Java's
     * {@link HttpsURLConnection} silently converts the HTTPS method to POST or PUT if a
     * body is written to the request.
     */
    public void writeOutput(byte[] body)
    {
        // Codes_SRS_HTTPSCONNECTION_11_010: [The function shall throw an IllegalArgumentException if the request does not currently use method POST or PUT and the body is non-empty.]
        HttpsMethod method = HttpsMethod.valueOf(
                this.connection.getRequestMethod());
        if (method != HttpsMethod.POST && method != HttpsMethod.PUT)
        {
            if (body.length > 0)
            {
                throw new IllegalArgumentException(
                        "Cannot write a body to a request that "
                        + "is not a POST or a PUT request.");
            }
        }
        else
        {
            // Codes_SRS_HTTPSCONNECTION_11_009: [The function shall save the body to be sent with the request.]
            this.body = Arrays.copyOf(body, body.length);
        }
    }

    /**
     * Reads from the input stream (response stream) and returns the response.
     *
     * @return the response body.
     *
     * @throws IOException if the input stream could not be accessed, for
     * example if the server could not be reached.
     */
    public byte[] readInput() throws IOException
    {
        // Codes_SRS_HTTPSCONNECTION_11_011: [The function shall read from the input stream (response stream) and return the response.]
        // Codes_SRS_HTTPSCONNECTION_11_012: [The function shall throw an IOException if the input stream could not be accessed.]
        InputStream inputStream = this.connection.getInputStream();
        byte[] input = readInputStream(inputStream);
        // Codes_SRS_HTTPSCONNECTION_11_019: [The function shall close the input stream after it has been completely read.]
        inputStream.close();

        return input;
    }

    /**
     * Reads from the error stream and returns the error reason.
     *
     * @return the error reason.
     *
     * @throws IOException if the input stream could not be accessed, for
     * example if the server could not be reached.
     */
    public byte[] readError() throws IOException
    {
        // Codes_SRS_HTTPSCONNECTION_11_013: [The function shall read from the error stream and return the response.]
        // Codes_SRS_HTTPSCONNECTION_11_014: [The function shall throw an IOException if the error stream could not be accessed.]
        InputStream errorStream = this.connection.getErrorStream();

        byte[] error = new byte[0];
        // if there is no error reason, getErrorStream() returns null.
        if (errorStream != null)
        {
            error = readInputStream(errorStream);
            // Codes_SRS_HTTPSCONNECTION_11_020: [The function shall close the error stream after it has been completely read.]
            errorStream.close();
        }

        return error;
    }

    /**
     * Returns the response status code.
     *
     * @return the response status code.
     *
     * @throws IOException if no response was received.
     */
    public int getResponseStatus() throws IOException
    {
        // Codes_SRS_HTTPSCONNECTION_11_015: [The function shall return the response status code.]
        // Codes_SRS_HTTPSCONNECTION_11_016: [The function shall throw an IOException if no response was received.]
        return this.connection.getResponseCode();
    }

    /**
     * Returns the response headers as a {@link Map}, where the key is the
     * header field name and the values are the values associated with the
     * header field name.
     *
     * @return the response headers.
     *
     * @throws IOException if no response was received.
     */
    public Map<String, List<String>> getResponseHeaders() throws IOException
    {
        // Codes_SRS_HTTPSCONNECTION_11_017: [The function shall return a mapping of header field names to the values associated with the header field name.]
        // Codes_SRS_HTTPSCONNECTION_11_018: [The function shall throw an IOException if no response was received.]
        return this.connection.getHeaderFields();
    }

    /**
     * Reads the input stream until the stream is empty.
     *
     * @param stream the input stream.
     *
     * @return the content of the input stream.
     *
     * @throws IOException if the input stream could not be read from.
     */
    protected static byte[] readInputStream(InputStream stream)
            throws IOException
    {
        ArrayList<Byte> byteBuffer = new ArrayList<>();
        int nextByte = -1;
        // read(byte[]) reads the byte into the buffer and returns the number
        // of bytes read, or -1 if the end of the stream has been reached.
        while ((nextByte = stream.read()) > -1)
        {
            byteBuffer.add((byte) nextByte);
        }

        int bufferSize = byteBuffer.size();
        byte[] byteArray = new byte[bufferSize];
        for (int i = 0; i < bufferSize; ++i)
        {
            byteArray[i] = byteBuffer.get(i);
        }

        return byteArray;
    }

    protected HttpsConnection()
    {
        this.connection = null;
    }
}
