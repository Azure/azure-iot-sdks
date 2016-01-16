// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

package tests.unit.com.microsoft.azure.iothub.transport.https;

import static org.junit.Assert.assertThat;
import static org.hamcrest.CoreMatchers.is;
import static org.hamcrest.CoreMatchers.not;

import org.junit.Test;

import com.microsoft.azure.iothub.transport.https.HttpsResponse;

import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

/** Unit tests for HttpsResponse. */
public class HttpsResponseTest
{
    // Tests_SRS_HTTPSRESPONSE_11_001: [The constructor shall store the input arguments so that the getters can return them later.]
    // Tests_SRS_HTTPSRESPONSE_11_002: [The function shall return the status code given in the constructor.]
    @Test
    public void getStatusReturnsStatus()
    {
        final int status = 200;
        final byte[] body = { 1 };
        final Map<String, List<String>> headerFields = new HashMap<>();
        final byte[] errorReason = {};

        HttpsResponse response =
                new HttpsResponse(status, body, headerFields, errorReason);
        int testStatus = response.getStatus();

        final int expectedStatus = status;
        assertThat(testStatus, is(expectedStatus));
    }

    // Tests_SRS_HTTPSRESPONSE_11_001: [The constructor shall store the input arguments so that the getters can return them later.]
    // Tests_SRS_HTTPSRESPONSE_11_003: [The function shall return a copy of the body given in the constructor.]
    @Test
    public void getBodyReturnsCopyOfBody()
    {
        final int status = 200;
        final byte[] body = { 1, 2, 3, 4 };
        final Map<String, List<String>> headerFields = new HashMap<>();
        byte[] errorReason = {};

        HttpsResponse response =
                new HttpsResponse(status, body, headerFields, errorReason);
        byte[] testBody = response.getBody();

        final byte[] expectedBody = body;
        assertThat(testBody, is(expectedBody));

        testBody[0] = 3;
        assertThat(testBody, is(not(expectedBody)));
    }

    // Tests_SRS_HTTPSRESPONSE_11_001: [The constructor shall store the input arguments so that the getters can return them later.]
    // Tests_SRS_HTTPSRESPONSE_11_004: [The function shall return a comma-separated list of the values associated with the header field name.]
    @Test
    public void getHeaderFieldReturnsHeaderField()
    {
        final int status = 200;
        final byte[] body = { 1 };
        final byte[] errorReason = {};
        final Map<String, List<String>> headerFields = new HashMap<>();
        final String field = "test-field";
        final List<String> values = new LinkedList<>();
        final String value0 = "test-field-value0";
        final String value1 = "test-field-value1";
        values.add(value0);
        values.add(value1);
        headerFields.put(field, values);

        HttpsResponse response =
                new HttpsResponse(status, body, headerFields, errorReason);
        String testValues = response.getHeaderField(field);

        final String expectedValues = value0 + "," + value1;
        assertThat(testValues, is(expectedValues));
    }

    // Tests_SRS_HTTPSRESPONSE_11_008: [The function shall match the header field name in a case-insensitive manner.]
    @Test
    public void getHeaderFieldMatchesCaseInsensitive()
    {
        final int status = 200;
        final byte[] body = { 1 };
        final byte[] errorReason = {};
        final Map<String, List<String>> headerFields = new HashMap<>();
        final String field = "test-field";
        final List<String> values = new LinkedList<>();
        final String value0 = "test-field-value0";
        final String value1 = "test-field-value1";
        values.add(value0);
        values.add(value1);
        headerFields.put(field, values);

        HttpsResponse response =
                new HttpsResponse(status, body, headerFields, errorReason);
        String differentCaseField = "Test-Field";
        String testValues = response.getHeaderField(differentCaseField);

        final String expectedValues = value0 + "," + value1;
        assertThat(testValues, is(expectedValues));
    }

    // Tests_SRS_HTTPSRESPONSE_11_006: [If a value could not be found for the given header field name, the function shall throw an IllegalArgumentException.]
    @Test(expected = IllegalArgumentException.class)
    public void getHeaderFieldRejectsInvalidFieldName()
            throws IllegalArgumentException
    {
        final int status = 200;
        final byte[] body = { 1 };
        final byte[] errorReason = {};
        final Map<String, List<String>> headerFields = new HashMap<>();
        final String field = "test-field";

        HttpsResponse response =
                new HttpsResponse(status, body, headerFields, errorReason);
        response.getHeaderField(field);
    }

    // Tests_SRS_HTTPSRESPONSE_11_001: [The constructor shall store the input arguments so that the getters can return them later.]
    // Tests_SRS_HTTPSRESPONSE_11_007: [The function shall return the error reason given in the constructor.]
    @Test
    public void getErrorReasonReturnsErrorReason()
    {
        final int status = 200;
        final byte[] body = { 1 };
        final Map<String, List<String>> headerFields = new HashMap<>();
        final byte[] errorReason = { 2, 3, 4, 5 };

        HttpsResponse response =
                new HttpsResponse(status, body, headerFields, errorReason);
        byte[] testErrorReason = response.getErrorReason();

        final byte[] expectedErrorReason = errorReason;
        assertThat(testErrorReason, is(expectedErrorReason));
    }
}
