/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.transport.http;

import mockit.integration.junit4.JMockit;
import org.junit.Test;
import org.junit.runner.RunWith;

import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

import static org.hamcrest.CoreMatchers.is;
import static org.hamcrest.CoreMatchers.not;
import static org.junit.Assert.assertThat;

/** Unit tests for HttpResponse. */
@RunWith(JMockit.class)
public class HttpsResponseTest
{
    // Tests_SRS_SERVICE_SDK_JAVA_HTTPSRESPONSE_12_001: [The constructor shall store the input arguments so that the getters can return them later.]
    // Tests_SRS_SERVICE_SDK_JAVA_HTTPSRESPONSE_12_002: [The function shall return the status code given in the constructor.]
    @Test
    public void getStatusReturnsStatus()
    {
        // Arrange
        final int status = 200;
        final byte[] body = { 1 };
        final Map<String, List<String>> headerFields = new HashMap<>();
        final byte[] errorReason = {};
        final int expectedStatus = status;
        // Act
        HttpResponse response = new HttpResponse(status, body, headerFields, errorReason);
        int testStatus = response.getStatus();
        // Assert
        assertThat(testStatus, is(expectedStatus));
    }

    // Tests_SRS_SERVICE_SDK_JAVA_HTTPSRESPONSE_12_001: [The constructor shall store the input arguments so that the getters can return them later.]
    // Tests_SRS_SERVICE_SDK_JAVA_HTTPSRESPONSE_12_003: [The function shall return a copy of the body given in the constructor.]
    @Test
    public void getBodyReturnsCopyOfBody()
    {
        // Arrange
        final int status = 200;
        final byte[] body = { 1, 2, 3, 4 };
        final Map<String, List<String>> headerFields = new HashMap<>();
        byte[] errorReason = {};
        final byte[] expectedBody = body;
        // Act
        HttpResponse response = new HttpResponse(status, body, headerFields, errorReason);
        byte[] testBody = response.getBody();
        // Assert
        assertThat(testBody, is(expectedBody));
        testBody[0] = 3;
        assertThat(testBody, is(not(expectedBody)));
    }

    // Tests_SRS_SERVICE_SDK_JAVA_HTTPSRESPONSE_12_001: [The constructor shall store the input arguments so that the getters can return them later.]
    // Tests_SRS_SERVICE_SDK_JAVA_HTTPSRESPONSE_12_004: [The function shall return a comma-separated list of the values associated with the header field name.]
    @Test
    public void getHeaderFieldReturnsHeaderField()
    {
        // Arrange
        final int status = 200;
        final byte[] body = { 1 };
        final byte[] errorReason = {};
        final Map<String, List<String>> headerFields = new HashMap<>();
        final String field = "test-field";
        final List<String> values = new LinkedList<>();
        final String value0 = "test-field-value0";
        final String value1 = "test-field-value1";
        final String expectedValues = value0 + "," + value1;
        values.add(value0);
        values.add(value1);
        headerFields.put(field, values);
        // Act
        HttpResponse response = new HttpResponse(status, body, headerFields, errorReason);
        String testValues = response.getHeaderField(field);
        // Assert
        assertThat(testValues, is(expectedValues));
    }

    // Tests_SRS_SERVICE_SDK_JAVA_HTTPSRESPONSE_12_005: [The function shall match the header field name in a case-insensitive manner.]
    @Test
    public void getHeaderFieldMatchesCaseInsensitive()
    {
        // Arrange
        final int status = 200;
        final byte[] body = { 1 };
        final byte[] errorReason = {};
        final Map<String, List<String>> headerFields = new HashMap<>();
        final String field = "test-field";
        final List<String> values = new LinkedList<>();
        final String value0 = "test-field-value0";
        final String value1 = "test-field-value1";
        final String expectedValues = value0 + "," + value1;
        values.add(value0);
        values.add(value1);
        headerFields.put(field, values);
        // Act
        HttpResponse response = new HttpResponse(status, body, headerFields, errorReason);
        String differentCaseField = "Test-Field";
        String testValues = response.getHeaderField(differentCaseField);
        // Assert
        assertThat(testValues, is(expectedValues));
    }

    // Tests_SRS_SERVICE_SDK_JAVA_HTTPSRESPONSE_12_006: [If a value could not be found for the given header field name, the function shall throw an IllegalArgumentException.]
    // Assert
    @Test(expected = IllegalArgumentException.class)
    public void getHeaderFieldRejectsInvalidFieldName() throws IllegalArgumentException
    {
        // Arrange
        final int status = 200;
        final byte[] body = { 1 };
        final byte[] errorReason = {};
        final Map<String, List<String>> headerFields = new HashMap<>();
        final String field = "test-field";
        // Act
        HttpResponse response = new HttpResponse(status, body, headerFields, errorReason);
        response.getHeaderField(field);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_HTTPSRESPONSE_12_001: [The constructor shall store the input arguments so that the getters can return them later.]
    // Tests_SRS_SERVICE_SDK_JAVA_HTTPSRESPONSE_12_007: [The function shall return the error reason given in the constructor.]
    @Test
    public void getErrorReasonReturnsErrorReason()
    {
        // Arrange
        final int status = 200;
        final byte[] body = { 1 };
        final Map<String, List<String>> headerFields = new HashMap<>();
        final byte[] errorReason = { 2, 3, 4, 5 };
        final byte[] expectedErrorReason = errorReason;
        // Act
        HttpResponse response = new HttpResponse(status, body, headerFields, errorReason);
        byte[] testErrorReason = response.getErrorReason();
        // Assert
        assertThat(testErrorReason, is(expectedErrorReason));
    }
}
