/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package samples.com.microsoft.azure.iot.service.sdk;

import com.microsoft.azure.iot.service.sdk.*;

import java.io.IOException;
import java.net.URISyntaxException;
import java.util.Date;
import java.util.HashMap;
import java.util.Map;
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
    /** Choose iotHubServiceClientProtocol */
    private static final IotHubServiceClientProtocol protocol = IotHubServiceClientProtocol.AMQPS;
//    private static final IotHubServiceClientProtocol iotHubServiceClientProtocol = IotHubServiceClientProtocol.AMQPS_WS;

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

        Message messageToSend = new Message(commandMessage);
        messageToSend.setDeliveryAcknowledgement(DeliveryAcknowledgement.Full);

        // Setting standard properties
        messageToSend.setMessageId(java.util.UUID.randomUUID().toString());
        Date now = new Date();
        messageToSend.setExpiryTimeUtc(new Date(now.getTime() + 60 * 1000));
        messageToSend.setCorrelationId(java.util.UUID.randomUUID().toString());
        messageToSend.setUserId(java.util.UUID.randomUUID().toString());
        messageToSend.clearCustomProperties();

        // Setting user properties
        Map<String, String> propertiesToSend = new HashMap<String, String>();
        propertiesToSend.put("mycustomKey1", "mycustomValue1");
        propertiesToSend.put("mycustomKey2", "mycustomValue2");
        propertiesToSend.put("mycustomKey3", "mycustomValue3");
        propertiesToSend.put("mycustomKey4", "mycustomValue4");
        propertiesToSend.put("mycustomKey5", "mycustomValue5");
        messageToSend.setProperties(propertiesToSend);

        CompletableFuture<Void> completableFuture = serviceClient.sendAsync(deviceId, messageToSend);
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
        serviceClient = ServiceClient.createFromConnectionString(connectionString, protocol);

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
