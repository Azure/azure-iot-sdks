/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.transport.http;

import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * An HTTPS response. Contains the status code, body, header fields, and error
 * reason (if any).
 */
public class HttpResponse
{
    protected final int status;
    protected final byte[] body;
    protected final byte[] errorReason;
    protected final Map<String, String> headerFields;

    /**
     * Constructor.
     *
     * @param status The HTTPS status code.
     * @param body The response body.
     * @param headerFields The map of header field names and the values associated with the field name.
     * @param errorReason The error reason.
     */
    public HttpResponse(int status, byte[] body,
                        Map<String, List<String>> headerFields,
                        byte[] errorReason)
    {
        // Codes_SRS_SERVICE_SDK_JAVA_HTTPRESPONSE_12_001: [The constructor shall store the input arguments so that the getters can return them later.]
        this.status = status;
        this.body = Arrays.copyOf(body, body.length);
        this.errorReason = errorReason;

        this.headerFields = new HashMap<>();
        for (Map.Entry<String, List<String>> headerField : headerFields
                .entrySet())
        {
            String key = headerField.getKey();
            if (key != null)
            {
                String field = canonicalizeFieldName(key);
                String values = flattenValuesList(headerField.getValue());
                this.headerFields.put(field, values);
            }
        }
    }

    /**
     * Getter for the HTTPS status code.
     *
     * @return The HTTPS status code.
     */
    public int getStatus()
    {
        // Codes_SRS_SERVICE_SDK_JAVA_HTTPRESPONSE_12_002: [The function shall return the status code given in the constructor.]
        return this.status;
    }

    /**
     * Getter for the response body.
     *
     * @return The response body.
     */
    public byte[] getBody()
    {
        // Codes_SRS_SERVICE_SDK_JAVA_HTTPRESPONSE_12_003: [The function shall return a copy of the body given in the constructor.]
        return Arrays.copyOf(this.body, this.body.length);
    }

    /**
     * Getter for a header field.
     *
     * @param field the header field name.
     *
     * @return the header field value. If multiple values are present, they are
     * returned as a comma-separated list according to RFC2616.
     *
     * @throws IllegalArgumentException if no value exists for the given field
     * name.
     */
    public String getHeaderField(String field)
    {
        // Codes_SRS_SERVICE_SDK_JAVA_HTTPRESPONSE_12_008: [The function shall match the header field name in a case-insensitive manner.]
        String canonicalizedField = canonicalizeFieldName(field);
        String values = this.headerFields.get(canonicalizedField);
        // Codes_SRS_SERVICE_SDK_JAVA_HTTPRESPONSE_12_006: [If a value could not be found for the given header field name, the function shall throw an IllegalArgumentException.]
        if (values == null)
        {
            String errMsg = String.format("Could not find a value "
                    + "associated with the header field name '%s'.\n", field);

            throw new IllegalArgumentException(errMsg);
        }

        // Codes_SRS_SERVICE_SDK_JAVA_HTTPRESPONSE_12_004: [The function shall return a comma-separated list of the values associated with the header field name.]
        return values;
    }

    /**
     * Getter for the header fields.
     *
     * @return The copy of the header fields for this response.
     */
    public Map<String, String> getHeaderFields()
    {
        Map<String, String> headerFieldsCopy = new HashMap<>();
        for (Map.Entry<String, String> field : this.headerFields.entrySet())
        {
            headerFieldsCopy.put(field.getKey(), field.getValue());
        }

        return headerFieldsCopy;
    }

    /**
     * Getter for the error reason.
     *
     * @return The error reason.
     */
    public byte[] getErrorReason()
    {
        // Codes_SRS_SERVICE_SDK_JAVA_HTTPRESPONSE_12_007: [The function shall return the error reason given in the constructor.]
        return this.errorReason;
    }

    protected static String canonicalizeFieldName(String field)
    {
        String canonicalizedField = field;
        if (canonicalizedField != null)
        {
            canonicalizedField = field.toLowerCase();
        }

        return canonicalizedField;
    }

    protected static String flattenValuesList(List<String> values)
    {
        String valuesStr = "";
        for (String value : values)
        {
            valuesStr += value + ",";
        }
        // remove the trailing comma.
        valuesStr =
                valuesStr.substring(0, Math.max(0, valuesStr.length() - 1));

        return valuesStr;
    }

    protected HttpResponse()
    {
        this.status = 0;
        this.body = null;
        this.headerFields = null;
        this.errorReason = null;
    }
}
