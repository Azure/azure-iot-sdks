/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.sdk;

import com.microsoft.azure.iot.service.auth.IotHubServiceSasToken;
import com.microsoft.azure.iot.service.transport.amqps.AmqpMessageSender;

import javax.jms.JMSException;
import java.io.IOException;
import java.net.URISyntaxException;
import java.util.concurrent.CompletableFuture;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

/**
 * AMQP specific implementation of ServiceClient
 */
public class IotHubServiceClient extends ServiceClient
{
    private final ExecutorService executor = Executors.newFixedThreadPool(10);

    private AmqpMessageSender amqpMessageSender;
    String hostName;
    String userName;
    String sasToken;

    protected IotHubServiceClient(IotHubConnectionString iotHubConnectionString) throws URISyntaxException, IOException
    {
        if (iotHubConnectionString == null)
        {
            throw new IllegalArgumentException();
        }
        this.iotHubConnectionString = iotHubConnectionString;
        
        this.hostName = iotHubConnectionString.getHostName();
        this.userName = iotHubConnectionString.getUserString();

        IotHubServiceSasToken iotHubServiceSasToken = new IotHubServiceSasToken(iotHubConnectionString);
        this.sasToken = iotHubServiceSasToken.toString();

        this.amqpMessageSender = new AmqpMessageSender(hostName, userName, sasToken);
    }

    public void open() throws JMSException
    {
        if (this.amqpMessageSender != null)
        {
            this.amqpMessageSender.open();
        }
    }

    private void close() throws JMSException
    {
        if (this.amqpMessageSender != null)
        {
            this.amqpMessageSender.close();
        }
    }

    private void send(String deviceId, Message message) throws JMSException
    {
        if (this.amqpMessageSender != null)
        {
            this.amqpMessageSender.send(deviceId, message);
        }
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
    public CompletableFuture<Void> sendAsync(String deviceId, Message message)
    {
        final CompletableFuture<Void> future = new CompletableFuture<>();
        executor.submit(() -> {
            try
            {
                send(deviceId, message);
                future.complete(null);
            } catch (JMSException e)
            {
                future.completeExceptionally(e);
            }
        });
        return future;
    }

    @Override
    public FeedbackReceiver getFeedbackReceiver(String deviceId)
    {
        FeedbackReceiver feedbackReceiver = new FeedbackReceiver(hostName, userName, sasToken, deviceId);
        return feedbackReceiver;
    }
}
