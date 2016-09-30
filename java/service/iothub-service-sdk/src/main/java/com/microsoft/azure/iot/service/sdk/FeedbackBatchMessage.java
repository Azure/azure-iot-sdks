/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.sdk;

import javax.json.Json;
import javax.json.JsonArray;
import javax.json.JsonObject;
import javax.json.JsonReader;
import java.io.StringReader;
import java.time.Instant;
import java.util.ArrayList;

/**
 * Provide static function to parse Json string to FeedbackBatch object.
 */
public class FeedbackBatchMessage
{
    /**
     * Parse received Json and create FeedbackBatch object
     *
     * @param jsonString Json string to parse
     * @return The created FeedbackBatch
     */
    public static FeedbackBatch parse(String jsonString)
    {
        FeedbackBatch returnFeedbackBatch = new FeedbackBatch();

        // Codes_SRS_SERVICE_SDK_JAVA_FEEDBACKBATCHMESSAGE_12_001: [The function shall return an empty FeedbackBatch object if the input is empty or null]
        if (!Tools.isNullOrEmpty(jsonString))
        {
            // Codes_SRS_SERVICE_SDK_JAVA_FEEDBACKBATCHMESSAGE_12_003: [The function shall remove data batch brackets if they exist]
            if (jsonString.startsWith("Data{"))
            {
                jsonString = jsonString.substring(5, jsonString.length() - 1);
            }

            // Codes_SRS_SERVICE_SDK_JAVA_FEEDBACKBATCHMESSAGE_12_002: [The function shall return an empty FeedbackBatch object if the content of the Data input is empty]
            if (!jsonString.equals(""))
            {
                try (JsonReader jsonReader = Json.createReader(new StringReader(jsonString)))
                {
                    JsonArray jsonArray = jsonReader.readArray();
                    ArrayList<FeedbackRecord> records = new ArrayList<>();

                    // Codes_SRS_SERVICE_SDK_JAVA_FEEDBACKBATCHMESSAGE_12_005: [The function shall parse all the Json record to the FeedbackBatch]
                    for (int i = 0; i < jsonArray.size(); i++)
                    {
                        // Codes_SRS_SERVICE_SDK_JAVA_FEEDBACKBATCHMESSAGE_12_004: [The function shall throw a JsonParsingException if the parsing failed]
                        JsonObject jsonObject = (JsonObject) jsonArray.get(i);

                        FeedbackRecord feedbackRecord = new FeedbackRecord();

                        feedbackRecord.setEnqueuedTimeUtc(Instant.parse(Tools.getValueFromJsonObject(jsonObject, "enqueuedTimeUtc")));

                        String originalMessageId = Tools.getValueFromJsonObject(jsonObject, "originalMessageId");
                        feedbackRecord.setOriginalMessageId(originalMessageId);
                        feedbackRecord.setCorrelationId("");

                        String description = Tools.getValueFromJsonObject(jsonObject, "description");
                        feedbackRecord.setDescription(description);
                        if (description.toLowerCase().equals("success"))
                        {
                            feedbackRecord.setStatusCode(FeedbackStatusCode.success);
                        } else if (description.toLowerCase().equals("expired"))
                        {
                            feedbackRecord.setStatusCode(FeedbackStatusCode.expired);
                        } else if (description.toLowerCase().equals("deliverycountexceeded"))
                        {
                            feedbackRecord.setStatusCode(FeedbackStatusCode.deliveryCountExceeded);
                        } else if (description.toLowerCase().equals("rejected"))
                        {
                            feedbackRecord.setStatusCode(FeedbackStatusCode.rejected);
                        }
                        else
                        {
                            feedbackRecord.setStatusCode(FeedbackStatusCode.unknown);
                        }
                        feedbackRecord.setDeviceId(Tools.getValueFromJsonObject(jsonObject, "deviceId"));
                        feedbackRecord.setDeviceGenerationId(Tools.getValueFromJsonObject(jsonObject, "deviceGenerationId"));

                        records.add(feedbackRecord);
                    }

                    if (records.size() > 0)
                    {
                        // Codes_SRS_SERVICE_SDK_JAVA_FEEDBACKBATCHMESSAGE_12_006: [The function shall copy the last record’s UTC time for batch UTC time]
                        returnFeedbackBatch.setEnqueuedTimeUtc(records.get(records.size() - 1).getEnqueuedTimeUtc());
                        returnFeedbackBatch.setUserId("");
                        returnFeedbackBatch.setLockToken("");
                        returnFeedbackBatch.setRecords(records);
                    }
                }
            }
        }
        return returnFeedbackBatch;
    }
}
