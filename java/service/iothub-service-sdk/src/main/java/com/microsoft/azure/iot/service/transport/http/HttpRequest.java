/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.transport.http;

import com.microsoft.azure.iot.service.transport.TransportUtils;

import java.io.IOException;
import java.net.URL;
import java.util.List;
import java.util.Map;

public class HttpRequest
{
    /** The underlying HTTPS connection stream. */
    protected final HttpConnection connection;

    /**
     * Constructor. Takes a URL as an argument and returns an HTTPS request that
     * is ready to be sent.
     *
     * @param url The URL for the request.
     * @param method The HTTPS request method (i.e. GET).
     * @param body The request body. Must be an array of size 0 if the request method is GET or DELETE.
     *
     * @throws IOException This exception thrown if an IOException occurs
     * in setting up the HTTPS connection.
     * @throws IllegalArgumentException This exception thrown if the endpoint
     * given does not use the HTTPS protocol.
     */
    public HttpRequest(URL url, HttpMethod method, byte[] body) throws IOException
    {
        // Codes_SRS_SERVICE_SDK_JAVA_HTTPREQUEST_12_001: [The function shall open a connection with the given URL as the endpoint.]
        // Codes_SRS_SERVICE_SDK_JAVA_HTTPREQUEST_12_003: [The function shall use the given HTTPS method (i.e. GET) as the request method.]
        // Codes_SRS_SERVICE_SDK_JAVA_HTTPREQUEST_12_004: [If an IOException occurs in setting up the HTTPS connection, the function shall throw an IOException.]
        this.connection = new HttpConnection(url, method);
        this.connection.setRequestHeader("User-Agent", TransportUtils.javaServiceClientIdentifier + TransportUtils.serviceVersion);
        // Codes_SRS_SERVICE_SDK_JAVA_HTTPREQUEST_12_002: [The function shall write the body to the connection.]
        this.connection.writeOutput(body);
    }

    /**
     * Executes the HTTPS request.
     *
     * @return The HTTPS response.
     *
     * @throws IOException This exception thrown if the connection could not be
     * established, or the input/output streams could not be accessed.
     */
    public HttpResponse send() throws IOException
    {
        int responseStatus = -1;
        byte[] responseBody = new byte[0];
        byte[] errorReason = new byte[0];
        Map<String, List<String>> headerFields;
        try
        {
            // Codes_SRS_SERVICE_SDK_JAVA_HTTPREQUEST_12_005: [The function shall send an HTTPS request as formatted in the constructor.]
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
            // Codes_SRS_SERVICE_SDK_JAVA_HTTPREQUEST_12_007: [If the client cannot connect to the server, the function shall throw an IOException.]
            responseStatus = this.connection.getResponseStatus();
            headerFields = this.connection.getResponseHeaders();
            // Codes_SRS_SERVICE_SDK_JAVA_HTTPREQUEST_12_008: [If an I/O exception occurs because of a bad response status code, the function shall attempt to flush or read the error stream so that the underlying HTTPS connection can be reused.]
            // Connections are transparently managed by Java.
            // The error stream must be cleared so that the connection
            // can be reused later.
            errorReason = this.connection.readError();
        }

        // Codes_SRS_SERVICE_SDK_JAVA_HTTPREQUEST_12_006: [The function shall return the HTTPS response received, including the status code, body, header fields, and error reason (if any).]
        return new HttpResponse(responseStatus, responseBody, headerFields,
                errorReason);
    }

    /**
     * Sets the header field to the given value.
     *
     * @param field The header field name.
     * @param value The header field value.
     *
     * @return The object itself, for fluent setting.
     */
    public HttpRequest setHeaderField(String field, String value)
    {
        // Codes_SRS_SERVICE_SDK_JAVA_HTTPREQUEST_12_009: [The function shall set the header field with the given name to the given value.]
        this.connection.setRequestHeader(field, value);
        return this;
    }

    /**
     * Sets the read timeout, in milliseconds, for the request. The read timeout
     * is the number of milliseconds after the server receives a request and
     * before the server sends data back.
     *
     * @param timeout The read timeout.
     *
     * @return The object itself, for fluent setting.
     */
    public HttpRequest setReadTimeoutMillis(int timeout)
    {
        // Codes_SRS_SERVICE_SDK_JAVA_HTTPREQUEST_12_010: [The function shall set the read timeout for the request to the given value.]
        this.connection.setReadTimeoutMillis(timeout);
        return this;
    }

    protected HttpRequest()
    {
        this.connection = null;
    }
}
