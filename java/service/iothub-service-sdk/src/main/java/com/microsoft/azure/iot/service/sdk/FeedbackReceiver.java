/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.sdk;

import com.microsoft.azure.iot.service.transport.amqps.AmqpReceive;

import java.io.IOException;
import java.util.concurrent.CompletableFuture;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class FeedbackReceiver extends Receiver
{
    private final long DEFAULT_TIMEOUT_MS = 60000;
    private final ExecutorService executor = Executors.newFixedThreadPool(10);

    private AmqpReceive amqpReceiver;
    private String deviceId;

    public FeedbackReceiver(String hostName, String userName, String sasToken, String deviceId)
    {
        this.deviceId = deviceId;
        this.amqpReceiver = new AmqpReceive(hostName, userName, sasToken);
    }

    private void open()
    {
        if (this.amqpReceiver != null)
        {
            this.amqpReceiver.open();
        }
    }

    private void close()
    {
        if (this.amqpReceiver != null)
        {
            this.amqpReceiver.close();
        }
    }

    private FeedbackBatch receive() throws IOException, InterruptedException
    {
        return receive(DEFAULT_TIMEOUT_MS);
    }

    private FeedbackBatch receive(long timeoutMs) throws IOException, InterruptedException
    {
        if (this.amqpReceiver != null)
        {
            return this.amqpReceiver.receive(this.deviceId, timeoutMs);
        }
        return null;
    }

    @Override
    public CompletableFuture<Void> openAsync()
    {
        final CompletableFuture<Void> future = new CompletableFuture<>();
        executor.submit(() -> {
            open();
            future.complete(null);
        });
        return future;
    }

    @Override
    public CompletableFuture<Void> closeAsync()
    {
        final CompletableFuture<Void> future = new CompletableFuture<>();
        executor.submit(() -> {
            close();
            future.complete(null);
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
        } catch (IOException e)
        {
            future.completeExceptionally(e);
        } catch (InterruptedException e)
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
