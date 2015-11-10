/*
 *  Copyright (c) Microsoft. All rights reserved.
 *  Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.sdk;

import com.microsoft.azure.iot.service.exceptions.IotHubException;
import org.junit.Before;
import org.junit.Test;

import java.io.IOException;
import java.util.Map;
import java.util.UUID;
import java.util.concurrent.CompletableFuture;

import static org.junit.Assert.assertEquals;

public class ServiceClientE2ETest
{
    private static String connectionStringEnvVarName = "IOTHUB_CONNECTION_STRING";
    private static String connectionString = "";
    private static String deviceId = "java-service-client-e2e-test";
    private static String message = "abcdefghijklmnopqrstuvwxyz1234567890";

    @Before
    public void setUp()
    {
        Map<String, String> env = System.getenv();
        for (String envName : env.keySet())
        {
            if (envName.equals(connectionStringEnvVarName.toString()))
            {
                connectionString = env.get(envName);
            }
        }

        String uuid = UUID.randomUUID().toString();
        deviceId = deviceId.concat("-" + uuid);
    }

    @Test
    public void service_client_e2e() throws Exception
    {
        if (Tools.isNullOrEmpty(connectionString))
        {
            throw new IllegalArgumentException("Environment variable is not set: " + connectionStringEnvVarName);
        }

        // Arrange

        // We remove and recreate the device for a clean start
        RegistryManager registryManager = RegistryManager.createFromConnectionString(connectionString);

        try
        {
            registryManager.removeDevice(deviceId);
        } catch (IOException|IotHubException e)
        {
        }

        Device deviceAdded = Device.createFromId(deviceId);
        registryManager.addDevice(deviceAdded);

        Device deviceGetBefore = registryManager.getDevice(deviceId);

        // Act

        // Create service client
        ServiceClient serviceClient = ServiceClient.createFromConnectionString(connectionString);
        CompletableFuture<Void> futureOpen = serviceClient.openAsync();
        futureOpen.get();

        CompletableFuture<Void> completableFuture = serviceClient.sendAsync(deviceId, message);
        completableFuture.get();

        Device deviceGetAfter = registryManager.getDevice(deviceId);

        CompletableFuture<Void> futureClose = serviceClient.closeAsync();
        futureClose.get();

        registryManager.removeDevice(deviceId);

        // Assert
        assertEquals(deviceGetBefore.getDeviceId(), deviceGetAfter.getDeviceId());
        assertEquals(0, deviceGetBefore.getCloudToDeviceMessageCount());
        assertEquals(1, deviceGetAfter.getCloudToDeviceMessageCount());
    }
}
