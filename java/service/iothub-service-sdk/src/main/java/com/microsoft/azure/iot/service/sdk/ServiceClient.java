/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.sdk;

import java.io.IOException;
import java.util.concurrent.CompletableFuture;

public abstract class ServiceClient
{
    protected IotHubConnectionString iotHubConnectionString;

    /**
     * Create ServiceClient from the specified connection string
     * 
     * @param connectionString - Connection string for the IotHub
     * @return 
     */
    public static ServiceClient createFromConnectionString(String connectionString) throws Exception
    {
        if (Tools.isNullOrEmpty(connectionString))
        {
            throw new IllegalArgumentException(connectionString);
        }
        IotHubConnectionString iotHubConnectionString = IotHubConnectionStringBuilder.createConnectionString(connectionString);
        IotHubServiceClient iotServiceClient = new IotHubServiceClient(iotHubConnectionString);
        return iotServiceClient;
    }

    public abstract void open() throws IOException;

    public abstract void close();

    public abstract void send(String deviceId, String message) throws IOException;

    /**
     * Open the ServiceClient instance
     * @return 
     */
    public abstract CompletableFuture<Void> openAsync();
    
    /**
     * Close the ServiceClient instance
     * @return 
     */
    public abstract CompletableFuture<Void> closeAsync();
    
    /**
     * Send a one-way message to the specified device
     * @param deviceId - The device identifier for the target device
     * @param message - The message for the device
     * @return 
     */
    public abstract CompletableFuture<Void> sendAsync(String deviceId, String message);
    
    /**
     * Get the FeedbackReceiver
     * @return - An instance of the FeedbackReceiver
     */
    public abstract FeedbackReceiver getFeedbackReceiver(String deviceId);
}
