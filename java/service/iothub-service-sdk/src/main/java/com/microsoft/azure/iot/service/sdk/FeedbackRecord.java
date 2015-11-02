/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.sdk;

import java.time.Instant;

public class FeedbackRecord
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

    private String correlationId;

    public String getCorrelationId()
    {
        return correlationId;
    }

    public void setCorrelationId(String correlationId)
    {
        this.correlationId = correlationId;
    }

    private FeedbackStatusCodeEnum statusCode;

    public FeedbackStatusCodeEnum getStatusCode()
    {
        return statusCode;
    }

    public void setStatusCode(FeedbackStatusCodeEnum statusCode)
    {
        this.statusCode = statusCode;
    }

    private String description;

    public String getDescription()
    {
        return description;
    }

    public void setDescription(String description)
    {
        this.description = description;
    }

    private String deviceGenerationId;

    public String getDeviceGenerationId()
    {
        return deviceGenerationId;
    }

    public void setDeviceGenerationId(String deviceGenerationId)
    {
        this.deviceGenerationId = deviceGenerationId;
    }

    private String deviceId;

    public String getDeviceId()
    {
        return deviceId;
    }

    public void setDeviceId(String deviceId)
    {
        this.deviceId = deviceId;
    }

}
