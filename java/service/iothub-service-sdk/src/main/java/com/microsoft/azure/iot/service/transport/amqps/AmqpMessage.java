/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.transport.amqps;

import com.microsoft.azure.iot.service.sdk.FeedbackBatch;
import com.microsoft.azure.iot.service.sdk.FeedbackRecord;
import com.microsoft.azure.iot.service.sdk.FeedbackStatusCodeEnum;
import com.microsoft.azure.iot.service.sdk.Tools;

import javax.jms.BytesMessage;
import javax.jms.JMSException;
import javax.json.Json;
import javax.json.JsonArray;
import javax.json.JsonObject;
import javax.json.JsonReader;
import java.io.StringReader;
import java.nio.charset.StandardCharsets;
import java.time.Instant;
import java.util.ArrayList;

public class AmqpMessage
{
    public static FeedbackBatch parseFeedbackBatch(BytesMessage bytesMessage) throws JMSException
    {
        FeedbackBatch returnFeedbackBatch = new FeedbackBatch();

        byte[] msgBody = new byte[(int) bytesMessage.getBodyLength()];
        bytesMessage.readBytes(msgBody);

        String jsonString = new String(msgBody, StandardCharsets.UTF_8);

        try (JsonReader jsonReader = Json.createReader(new StringReader(jsonString)))
        {
            JsonArray jsonArray = jsonReader.readArray();
            ArrayList<FeedbackRecord> records = new ArrayList<>();

            for (int i = 0; i < jsonArray.size(); i++)
            {
                JsonObject jsonObject = (JsonObject) jsonArray.get(i);

                FeedbackRecord feedbackRecord = new FeedbackRecord();

                feedbackRecord.setEnqueuedTimeUtc(Instant.parse(Tools.getStringValueFromJsonObject(jsonObject, "enqueuedTimeUtc")));
                feedbackRecord.setCorrelationId("");
                String description = Tools.getStringValueFromJsonObject(jsonObject, "description");
                feedbackRecord.setDescription(description);
                if (description.toLowerCase() == "success")
                {
                    feedbackRecord.setStatusCode(FeedbackStatusCodeEnum.success);
                }
                else if (description.toLowerCase() == "expired")
                {
                    feedbackRecord.setStatusCode(FeedbackStatusCodeEnum.expired);
                }
                else if (description.toLowerCase() == "deliverycountexceeded")
                {
                    feedbackRecord.setStatusCode(FeedbackStatusCodeEnum.deliveryCountExceeded);
                }
                else if (description.toLowerCase() == "rejected")
                {
                    feedbackRecord.setStatusCode(FeedbackStatusCodeEnum.rejected);
                }
                feedbackRecord.setDeviceId(Tools.getStringValueFromJsonObject(jsonObject, "deviceId"));
                feedbackRecord.setDeviceGenerationId(Tools.getStringValueFromJsonObject(jsonObject, "deviceGenerationId"));

                records.add(feedbackRecord);
            }

            if (records.size() > 0)
            {
                returnFeedbackBatch.setEnqueuedTimeUtc(records.get(records.size() - 1).getEnqueuedTimeUtc());
                returnFeedbackBatch.setUserId("");
                returnFeedbackBatch.setLockToken("");
                returnFeedbackBatch.setRecords(records);
            }
        }
        return returnFeedbackBatch;
    }
}
