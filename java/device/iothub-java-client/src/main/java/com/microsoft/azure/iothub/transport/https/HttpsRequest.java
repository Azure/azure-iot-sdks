// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

package com.microsoft.azure.iothub.transport.https;

import com.microsoft.azure.iothub.transport.TransportUtils;

import java.io.IOException;
import java.net.URL;
import java.util.List;
import java.util.Map;

/**
 * An HTTPS request.
 */
public class HttpsRequest
{
    /** The underlying HTTPS connection stream. */
    protected final HttpsConnection connection;

    /**
     * Constructor. Takes a URL as an argument and returns an HTTPS request that
     * is ready to be sent.
     *
     * @param url the URL for the request.
     * @param method the HTTPS request method (i.e. GET).
     * @param body the request body. Must be an array of size 0 if the request
     * method is GET or DELETE.
     *
     * @throws IOException if an IOException occurs in setting up the HTTPS
     * connection.
     * @throws IllegalArgumentException if the endpoint given does not use the
     * HTTPS protocol.
     */
    public HttpsRequest(URL url, HttpsMethod method, byte[] body)
            throws IOException
    {
        // Codes_SRS_HTTPSREQUEST_11_005: [If an IOException occurs in setting up the HTTPS connection, the function shall throw an IOException.]
        // Codes_SRS_HTTPSREQUEST_11_001: [The function shall open a connection with the given URL as the endpoint.]
        // Codes_SRS_HTTPSREQUEST_11_004: [The function shall use the given HTTPS method (i.e. GET) as the request method.]
        this.connection = new HttpsConnection(url, method);
        this.connection.setRequestHeader("User-Agent", TransportUtils.javaDeviceClientIdentifier + TransportUtils.clientVersion);
        // Codes_SRS_HTTPSREQUEST_11_002: [The function shall write the body to the connection.]
        this.connection.writeOutput(body);
    }

    /**
     * Executes the HTTPS request.
     *
     * @return an HTTPS response.
     *
     * @throws IOException if the connection could not be established, or the
     * input/output streams could not be accessed.
     */
    public HttpsResponse send() throws IOException
    {
        int responseStatus = -1;
        byte[] responseBody = new byte[0];
        byte[] errorReason = new byte[0];
        Map<String, List<String>> headerFields;
        try
        {
            // Codes_SRS_HTTPSREQUEST_11_008: [The function shall send an HTTPS request as formatted in the constructor.]
            this.connection.connect();

            responseStatus = this.connection.getResponseStatus();
            headerFields = this.connection.getResponseHeaders();
            responseBody = this.connection.readInput();
        }
        // Can be caused either by an unsuccessful
        // connection or by a bad status code.
        catch (IOException e)
        {
            // If the IOException was caused by a bad status code in the
            // response, then getResponseStatus() returns a valid status code.
            // Otherwise, a connection could not be established and
            // getResponseStatus() throws an IOException.
            // Codes_SRS_HTTPSREQUEST_11_011: [If the client cannot connect to the server, the function shall throw an IOException.]
            responseStatus = this.connection.getResponseStatus();
            headerFields = this.connection.getResponseHeaders();
            // Codes_SRS_HTTPSREQUEST_11_012: [If an I/O exception occurs because of a bad response status code, the function shall attempt to flush or read the error stream so that the underlying HTTPS connection can be reused.]
            // Connections are transparently managed by Java.
            // The error stream must be cleared so that the connection
            // can be reused later.
            errorReason = this.connection.readError();
        }

        // Codes_SRS_HTTPSREQUEST_11_009: [The function shall return the HTTPS response received, including the status code, body, header fields, and error reason (if any).]
        return new HttpsResponse(responseStatus, responseBody, headerFields,
                errorReason);
    }

    /**
     * Sets the header field to the given value.
     *
     * @param field the header field name.
     * @param value the header field value.
     *
     * @return itself, for fluent setting.
     */
    public HttpsRequest setHeaderField(String field, String value)
    {
        // Codes_SRS_HTTPSREQUEST_11_013: [The function shall set the header field with the given name to the given value.]
        this.connection.setRequestHeader(field, value);
        return this;
    }

    /**
     * Sets the read timeout, in milliseconds, for the request. The read timeout
     * is the number of milliseconds after the server receives a request and
     * before the server sends data back.
     *
     * @param timeout the read timeout.
     *
     * @return itself, for fluent setting.
     */
    public HttpsRequest setReadTimeoutMillis(int timeout)
    {
        // Codes_SRS_HTTPSREQUEST_11_014: [The function shall set the read timeout for the request to the given value.]
        this.connection.setReadTimeoutMillis(timeout);
        return this;
    }

    protected HttpsRequest()
    {
        this.connection = null;
    }
}
