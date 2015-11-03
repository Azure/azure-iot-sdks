/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package samples.com.microsoft.azure.iot.service.sdk;

import com.microsoft.azure.iot.service.sdk.FeedbackBatch;
import com.microsoft.azure.iot.service.sdk.FeedbackReceiver;
import com.microsoft.azure.iot.service.sdk.ServiceClient;

import java.io.IOException;
import java.net.URISyntaxException;
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

    private static ServiceClient serviceClient = null;
    private static FeedbackReceiver feedbackReceiver = null;

    /**
     * @param args
     * @throws IOException
     * @throws URISyntaxException
     */
    public static void main(String[] args) throws IOException, URISyntaxException, Exception
    {
        System.out.println("********* Starting ServiceClient sample...");

        openServiceClient();
        openFeedbackReceiver();

        String commandMessage = "Cloud to device message...";

        System.out.println("********* Sending message to device...");

        CompletableFuture<Void> completableFuture = serviceClient.sendAsync(deviceId, commandMessage);
        completableFuture.get();

        System.out.println("********* Waiting for feedback...");
        CompletableFuture<FeedbackBatch> future = feedbackReceiver.receiveAsync();
        FeedbackBatch feedbackBatch = future.get();
        System.out.println("********* Feedback received, feedback time: " + feedbackBatch.getEnqueuedTimeUtc().toString());

        closeFeedbackReceiver();
        closeServiceClient();

        System.out.println("********* Shutting down ServiceClient sample...");
        System.exit(0);
    }

    protected static void openServiceClient() throws Exception
    {
        System.out.println("Creating ServiceClient...");
        serviceClient = ServiceClient.createFromConnectionString(connectionString);

        CompletableFuture<Void> future = serviceClient.openAsync();
        future.get();
        System.out.println("********* Successfully created an ServiceClient.");
    }

    protected static void closeServiceClient() throws ExecutionException, InterruptedException, IOException
    {
        serviceClient.close();

        CompletableFuture<Void> future = serviceClient.closeAsync();
        future.get();
        serviceClient = null;
        System.out.println("********* Successfully closed ServiceClient.");
    }

    protected static void openFeedbackReceiver() throws ExecutionException, InterruptedException
    {
        if (serviceClient != null)
        {
            feedbackReceiver = serviceClient.getFeedbackReceiver(deviceId);
            if (feedbackReceiver != null)
            {
                CompletableFuture<Void> future = feedbackReceiver.openAsync();
                future.get();
                System.out.println("********* Successfully opened FeedbackReceiver...");
            }
        }
    }

    protected static void closeFeedbackReceiver() throws ExecutionException, InterruptedException
    {
        CompletableFuture<Void> future = feedbackReceiver.closeAsync();
        future.get();
        feedbackReceiver = null;
        System.out.println("********* Successfully closed FeedbackReceiver.");
    }
}
