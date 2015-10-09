/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package samples.com.microsoft.azure.iot.service.sdk;

import com.microsoft.azure.iot.service.sdk.FeedbackBatch;
import com.microsoft.azure.iot.service.sdk.FeedbackReceiver;
import com.microsoft.azure.iot.service.sdk.Message;
import com.microsoft.azure.iot.service.sdk.ServiceClient;

import javax.jms.JMSException;
import java.io.IOException;
import java.net.URISyntaxException;
import java.nio.charset.StandardCharsets;
import java.util.concurrent.CompletableFuture;
import java.util.concurrent.ExecutionException;

/**
 * Service client example for:
 *  - sending message to the device
 *  - waiting and receive feedback from the device
 */
public class ServiceClientSample
{
    private static final String connectionString = "[Connection string goes here]";
    private static final String deviceId = "[Device name goes here]";
    private static final String primaryKey = "[New primary key goes here]";
    private static final String secondaryKey = "[New secondary key goes here]";

    private static ServiceClient serviceClient = null;
    private static FeedbackReceiver feedbackReceiver = null;

    /**
     * @param args
     * @throws IOException
     * @throws URISyntaxException
     */
    public static void main(String[] args) throws IOException, URISyntaxException, Exception
    {
        System.out.println("\n********* Starting ServiceClient sample...\n");

        openServiceClient();
        openFeedbackReceiver();

        Message commandMessage = new Message("Cloud to device message...".getBytes(StandardCharsets.UTF_8));

        System.out.printf("\n********* Sending message to device...\n");

        CompletableFuture<Void> completableFuture = serviceClient.sendAsync(deviceId, commandMessage);
        completableFuture.get();

        System.out.printf("\n********* Waiting for feedback...\n");
        CompletableFuture<FeedbackBatch> future = feedbackReceiver.receiveAsync();

        FeedbackBatch feedbackBatch = future.get();
        System.out.printf("\n********* FEEDBACK TIME: " + feedbackBatch.getEnqueuedTimeUtc().toString() + "\n");

        closeFeedbackReceiver();
        closeServiceClient();

        System.out.println("\n********* Shutting down ServiceClient sample...\n");
    }

    protected static void openServiceClient() throws Exception
    {
        System.out.println("Creating ServiceClient...");
        serviceClient = ServiceClient.createFromConnectionString(connectionString);
        CompletableFuture<Void> future = serviceClient.openAsync();
        future.get();
        System.out.println("********* Successfully created an ServiceClient.\n");
    }

    protected static void closeServiceClient() throws ExecutionException, InterruptedException
    {
        CompletableFuture<Void> future = serviceClient.closeAsync();
        future.get();
        serviceClient = null;
        System.out.println("********* Successfully closed ServiceClient.\n");
    }

    protected static void openFeedbackReceiver() throws JMSException, ExecutionException, InterruptedException
    {
        if (serviceClient != null)
        {
            feedbackReceiver = serviceClient.getFeedbackReceiver(deviceId);
            if (feedbackReceiver != null)
            {
                CompletableFuture<Void> future = feedbackReceiver.openAsync();
                future.get();
                System.out.println("********* Successfully opened FeedbackReceiver.\n");
            }
        }
    }

    protected static void closeFeedbackReceiver() throws JMSException, ExecutionException, InterruptedException
    {
        CompletableFuture<Void> future = feedbackReceiver.closeAsync();
        future.get();
        feedbackReceiver = null;
        System.out.println("********* Successfully closed FeedbackReceiver.\n");
    }
}
