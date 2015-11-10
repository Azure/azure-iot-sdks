/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.sdk;

import java.time.Instant;
import java.util.ArrayList;

/**
 * Data structure for feedback messages received in Json array
 * Provide getters and setters for batch properties and messages
 */
public class FeedbackBatch
{
    private Instant enqueuedTimeUtc;

    public Instant getEnqueuedTimeUtc()
    {
        return enqueuedTimeUtc;
    }

    public void setEnqueuedTimeUtc(Instant enqueuedTimeUtc)
    {
        this.enqueuedTimeUtc = enqueuedTimeUtc;
    }

    private String userId;

    public String getUserId()
    {
        return userId;
    }

    public void setUserId(String userId)
    {
        this.userId = userId;
    }

    private String lockToken;

    public String getLockToken()
    {
        return lockToken;
    }

    public void setLockToken(String lockToken)
    {
        this.lockToken = lockToken;
    }

    private ArrayList<FeedbackRecord> records;

    public ArrayList<FeedbackRecord> getRecords()
    {
        return records;
    }

    public void setRecords(ArrayList<FeedbackRecord> records)
    {
        this.records = records;
    }
}
