/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.sdk;

import mockit.integration.junit4.JMockit;
import org.junit.Test;
import org.junit.runner.RunWith;

import javax.json.stream.JsonParsingException;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;

/** Unit tests for FeedbackBatchMessage */
@RunWith(JMockit.class)
public class FeedbackBatchMessageTest
{
    // Tests_SRS_SERVICE_SDK_JAVA_FEEDBACKBATCHMESSAGE_12_001: [The function shall return an empty FeedbackBatch object if the input is empty or null]
    @Test
    public void json_null()
    {
        // Arrange
        FeedbackBatch fbb = new FeedbackBatch();
        String jsonString = null;
        // Act
        FeedbackBatch feedbackBatch = FeedbackBatchMessage.parse(jsonString);
        // Assert
        assertNotNull(feedbackBatch);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_FEEDBACKBATCHMESSAGE_12_001: [The function shall return an empty FeedbackBatch object if the input is empty or null]
    @Test
    public void json_empty()
    {
        // Arrange
        String jsonString = "";
        // Act
        FeedbackBatch feedbackBatch = FeedbackBatchMessage.parse(jsonString);
        // Assert
        assertNotNull(feedbackBatch);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_FEEDBACKBATCHMESSAGE_12_002: [The function shall return an empty FeedbackBatch object if the content of the Data input is empty]
    // Tests_SRS_SERVICE_SDK_JAVA_FEEDBACKBATCHMESSAGE_12_003: [The function shall remove data batch brackets if they exist]
    @Test
    public void json_data_empty1()
    {
        // Arrange
        String jsonString = "Data{}";
        // Act
        FeedbackBatch feedbackBatch = FeedbackBatchMessage.parse(jsonString);
        // Assert
        assertNotNull(feedbackBatch);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_FEEDBACKBATCHMESSAGE_12_002: [The function shall return an empty FeedbackBatch object if the content of the Data input is empty]
    // Tests_SRS_SERVICE_SDK_JAVA_FEEDBACKBATCHMESSAGE_12_003: [The function shall remove data batch brackets if they exist]
    @Test
    public void json_data_empty_array()
    {
        // Arrange
        String jsonString = "Data{[]}";
        // Act
        FeedbackBatch feedbackBatch = FeedbackBatchMessage.parse(jsonString);
        // Assert
        assertNotNull(feedbackBatch);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_FEEDBACKBATCHMESSAGE_12_004: [The function shall throw a JsonParsingException if the parsing failed]
    // Assert
    @Test (expected = JsonParsingException.class)
    public void json_data_invalid_format1()
    {
        // Arrange
        String jsonString = "Data}";
        // Act
        FeedbackBatch feedbackBatch = FeedbackBatchMessage.parse(jsonString);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_FEEDBACKBATCHMESSAGE_12_002: [The function shall return an empty FeedbackBatch object if the content of the Data input is empty]
    @Test
    public void json_data_invalid_format2()
    {
        // Arrange
        String jsonString = "Data{]";
        // Act
        FeedbackBatch feedbackBatch = FeedbackBatchMessage.parse(jsonString);
        // Assert
        assertNotNull(feedbackBatch);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_FEEDBACKBATCHMESSAGE_12_004: [The function shall throw a JsonParsingException if the parsing failed]
    // Assert
    @Test (expected = JsonParsingException.class)
    public void json_data_invalid_format3()
    {
        // Arrange
        String jsonString = "Data{[}";
        // Act
        FeedbackBatch feedbackBatch = FeedbackBatchMessage.parse(jsonString);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_FEEDBACKBATCHMESSAGE_12_004: [The function shall throw a JsonParsingException if the parsing failed]
    // Assert
    @Test (expected = JsonParsingException.class)
    public void json_data_invalid_format4()
    {
        // Arrange
        String jsonString = "Data{[]";
        // Act
        FeedbackBatch feedbackBatch = FeedbackBatchMessage.parse(jsonString);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_FEEDBACKBATCHMESSAGE_12_004: [The function shall throw a JsonParsingException if the parsing failed]
    // Assert
    @Test (expected = JsonParsingException.class)
    public void json_data_invalid_format5()
    {
        // Arrange
        String jsonString = "Data{[\"]]";
        // Act
        FeedbackBatch feedbackBatch = FeedbackBatchMessage.parse(jsonString);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_FEEDBACKBATCHMESSAGE_12_004: [The function shall throw a JsonParsingException if the parsing failed]
    // Assert
    @Test (expected = JsonParsingException.class)
    public void json_data_invalid_format6()
    {
        // Arrange
        String jsonString = "[,xxx]";
        // Act
        FeedbackBatch feedbackBatch = FeedbackBatchMessage.parse(jsonString);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_FEEDBACKBATCHMESSAGE_12_005: [The function shall parse all the Json records to the FeedbackBatch]
    // Tests_SRS_SERVICE_SDK_JAVA_FEEDBACKBATCHMESSAGE_12_006: [The function shall copy the last record’s UTC time for batch UTC time]
    @Test
    public void parse_FeedbackBatchMessage_json_good_case()
    {
        // Arrange
        String jsonString =
                "Data{[" +
                        "{\"originalMessageId\":\"a1aaaaaa-bbbb-cccc-dddd-eeeeeeeeeeee\",\"description\":\"Success\",\"deviceGenerationId\":\"111111111111111111\",\"deviceId\":\"xxx-01\",\"enqueuedTimeUtc\":\"2015-10-10T23:35:19.9774002Z\"}," +
                        "{\"originalMessageId\":\"a2aaaaaa-bbbb-cccc-dddd-eeeeeeeeeeee\",\"description\":\"Expired\",\"deviceGenerationId\":\"222222222222222222\",\"deviceId\":\"xxx-02\",\"enqueuedTimeUtc\":\"2015-10-11T23:35:19.9774002Z\"}," +
                        "{\"originalMessageId\":\"a3aaaaaa-bbbb-cccc-dddd-eeeeeeeeeeee\",\"description\":\"Deliverycountexceeded\",\"deviceGenerationId\":\"333333333333333333\",\"deviceId\":\"xxx-03\",\"enqueuedTimeUtc\":\"2015-10-12T23:35:19.9774002Z\"}," +
                        "{\"originalMessageId\":\"a3aaaaaa-bbbb-cccc-dddd-eeeeeeeeeeee\",\"description\":\"xyz\",\"deviceGenerationId\":\"444444444444444444\",\"deviceId\":\"xxx-04\",\"enqueuedTimeUtc\":\"2015-10-13T23:35:19.9774002Z\"}," +
                        "{\"originalMessageId\":\"a4aaaaaa-bbbb-cccc-dddd-eeeeeeeeeeee\",\"description\":\"Rejected\",\"deviceGenerationId\":\"555555555555555555\",\"deviceId\":\"xxx-05\",\"enqueuedTimeUtc\":\"2015-10-14T23:35:19.9774002Z\"}" +
                        "]}";
        // Act
        FeedbackBatch feedbackBatch = FeedbackBatchMessage.parse(jsonString);
        // Assert
        assertEquals(feedbackBatch.getRecords().size(), 5);

        assertEquals(feedbackBatch.getRecords().get(0).getDeviceId(), "xxx-01");
        assertEquals(feedbackBatch.getRecords().get(0).getDeviceGenerationId(), "111111111111111111");
        assertEquals(feedbackBatch.getRecords().get(0).getEnqueuedTimeUtc().toString(), "2015-10-10T23:35:19.977400200Z");
        assertEquals(feedbackBatch.getRecords().get(0).getCorrelationId(), "");
        assertEquals(feedbackBatch.getRecords().get(0).getDescription(), "Success");
        assertEquals(feedbackBatch.getRecords().get(0).getStatusCode(), FeedbackStatusCode.success);

        assertEquals(feedbackBatch.getRecords().get(1).getDeviceId(), "xxx-02");
        assertEquals(feedbackBatch.getRecords().get(1).getDeviceGenerationId(), "222222222222222222");
        assertEquals(feedbackBatch.getRecords().get(1).getEnqueuedTimeUtc().toString(), "2015-10-11T23:35:19.977400200Z");
        assertEquals(feedbackBatch.getRecords().get(1).getCorrelationId(), "");
        assertEquals(feedbackBatch.getRecords().get(1).getDescription(), "Expired");
        assertEquals(feedbackBatch.getRecords().get(1).getStatusCode(), FeedbackStatusCode.expired);

        assertEquals(feedbackBatch.getRecords().get(2).getDeviceId(), "xxx-03");
        assertEquals(feedbackBatch.getRecords().get(2).getDeviceGenerationId(), "333333333333333333");
        assertEquals(feedbackBatch.getRecords().get(2).getEnqueuedTimeUtc().toString(), "2015-10-12T23:35:19.977400200Z");
        assertEquals(feedbackBatch.getRecords().get(2).getCorrelationId(), "");
        assertEquals(feedbackBatch.getRecords().get(2).getDescription(), "Deliverycountexceeded");
        assertEquals(feedbackBatch.getRecords().get(2).getStatusCode(), FeedbackStatusCode.deliveryCountExceeded);

        assertEquals(feedbackBatch.getRecords().get(3).getDeviceId(), "xxx-04");
        assertEquals(feedbackBatch.getRecords().get(3).getDeviceGenerationId(), "444444444444444444");
        assertEquals(feedbackBatch.getRecords().get(3).getEnqueuedTimeUtc().toString(), "2015-10-13T23:35:19.977400200Z");
        assertEquals(feedbackBatch.getRecords().get(3).getCorrelationId(), "");
        assertEquals(feedbackBatch.getRecords().get(3).getDescription(), "xyz");
        assertEquals(feedbackBatch.getRecords().get(3).getStatusCode(), FeedbackStatusCode.unknown);

        assertEquals(feedbackBatch.getRecords().get(4).getDeviceId(), "xxx-05");
        assertEquals(feedbackBatch.getRecords().get(4).getDeviceGenerationId(), "555555555555555555");
        assertEquals(feedbackBatch.getRecords().get(4).getEnqueuedTimeUtc().toString(), "2015-10-14T23:35:19.977400200Z");
        assertEquals(feedbackBatch.getRecords().get(4).getCorrelationId(), "");
        assertEquals(feedbackBatch.getRecords().get(4).getDescription(), "Rejected");
        assertEquals(feedbackBatch.getRecords().get(4).getStatusCode(), FeedbackStatusCode.rejected);

        assertEquals(feedbackBatch.getEnqueuedTimeUtc().toString(), "2015-10-14T23:35:19.977400200Z");
        assertEquals(feedbackBatch.getUserId(), "");
        assertEquals(feedbackBatch.getLockToken(), "");
    }
}
