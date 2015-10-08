/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.sdk;

import com.microsoft.azure.iot.service.transport.amqps.AmqpFeedbackReceiver;

import javax.jms.JMSException;
import java.util.concurrent.CompletableFuture;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class FeedbackReceiver extends Receiver
{
    private final long DEFAULT_TIMEOUT_MS = 60000;
    private final ExecutorService executor = Executors.newFixedThreadPool(10);

    private AmqpFeedbackReceiver amqpFeedbacReceiver;
    private String deviceId;

    public FeedbackReceiver(String hostName, String userName, String sasToken, String deviceId)
    {
        this.deviceId = deviceId;
        this.amqpFeedbacReceiver = new AmqpFeedbackReceiver(hostName, userName, sasToken);
    }

    private void open() throws JMSException
    {
        if (this.amqpFeedbacReceiver != null)
        {
            this.amqpFeedbacReceiver.open();
        }
    }

    private void close() throws JMSException
    {
        if (this.amqpFeedbacReceiver != null)
        {
            this.amqpFeedbacReceiver.close();
        }
    }

    private FeedbackBatch receive() throws JMSException
    {
        return receive(DEFAULT_TIMEOUT_MS);
    }

    private FeedbackBatch receive(long timeoutMs) throws JMSException
    {
        if (this.amqpFeedbacReceiver != null)
        {
            return this.amqpFeedbacReceiver.receive(this.deviceId, timeoutMs);
        }
        return null;
    }

    @Override
    public CompletableFuture<Void> openAsync()
    {
        final CompletableFuture<Void> future = new CompletableFuture<>();
        executor.submit(() -> {
            try
            {
                open();
                future.complete(null);
            } catch (JMSException e)
            {
                future.completeExceptionally(e);
            }
        });
        return future;
    }

    @Override
    public CompletableFuture<Void> closeAsync()
    {
        final CompletableFuture<Void> future = new CompletableFuture<>();
        executor.submit(() -> {
            try
            {
                close();
                future.complete(null);
            } catch (JMSException e)
            {
                future.completeExceptionally(e);
            }
        });
        return future;
    }

    @Override
    public CompletableFuture<FeedbackBatch> receiveAsync()
    {
        return receiveAsync(DEFAULT_TIMEOUT_MS);
    }

    @Override
    public CompletableFuture<FeedbackBatch> receiveAsync(long timeoutSeconds)
    {
        final CompletableFuture<FeedbackBatch> future = new CompletableFuture<>();
        executor.submit(() -> {
            try
            {
                FeedbackBatch responseFeedbackBatch = receive(timeoutSeconds);
                future.complete(responseFeedbackBatch);
            } catch (JMSException e)
            {
                future.completeExceptionally(e);
            }
        });
        return future;
    }

    @Override
    public CompletableFuture<Boolean> abandonAsync(String lockToken)
    {
        return null;
    }

    @Override
    public CompletableFuture<Boolean> completeAsync(String lockToken)
    {
        return null;
    }

    @Override
    public CompletableFuture<Boolean> abandonAsync(Message message)
    {
        return null;
    }

    @Override
    public CompletableFuture<Boolean> completeAsync(Message message)
    {
        return null;
    }
}
