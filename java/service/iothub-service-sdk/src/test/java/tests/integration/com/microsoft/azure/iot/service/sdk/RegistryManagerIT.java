/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package tests.integration.com.microsoft.azure.iot.service.sdk;

import com.microsoft.azure.iot.service.exceptions.IotHubException;
import com.microsoft.azure.iot.service.sdk.Device;
import com.microsoft.azure.iot.service.sdk.RegistryManager;
import com.microsoft.azure.iot.service.sdk.Tools;
import org.junit.Before;
import org.junit.Test;

import java.io.IOException;
import java.util.Map;
import java.util.UUID;

import static org.junit.Assert.assertEquals;

public class RegistryManagerIT
{
    private static String connectionStringEnvVarName = "IOTHUB_CONNECTION_STRING";
    private static String connectionString = "";
    private static String deviceId = "java-crud-e2e-test";

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
    public void crud_e2e() throws Exception
    {
        Boolean deviceExist = false;
        if (Tools.isNullOrEmpty(connectionString))
        {
            throw new IllegalArgumentException("Environment variable is not set: " + connectionStringEnvVarName);
        }

        // Arrange
        // Check if device exists with the given name
        RegistryManager registryManager = RegistryManager.createFromConnectionString(connectionString);
        try
        {
            Device device = registryManager.getDevice(deviceId);
            deviceExist = true;
        } catch (IotHubException e)
        {
        }
        if (deviceExist)
        {
            try
            {
                registryManager.removeDevice(deviceId);
            } catch (IotHubException|IOException e)
            {
                System.out.println("Initialization failed, could not remove device: " + deviceId);
                e.printStackTrace();
            }
        }
        // Act

        // Create
        Device deviceAdded = Device.createFromId(deviceId, null, null);
        registryManager.addDevice(deviceAdded);

        // Read
        Device deviceGet = registryManager.getDevice(deviceId);

        // Delete
        registryManager.removeDevice(deviceId);

        // Assert
        assertEquals(deviceId, deviceAdded.getDeviceId());
        assertEquals(deviceId, deviceGet.getDeviceId());
        try
        {
            Device device = registryManager.getDevice(deviceId);
        } catch (IotHubException e)
        {
        }
    }

    @Test
    public void getDeviceStatisticsTest() throws Exception
    {
        if (!Tools.isNullOrEmpty(connectionString))
        {
            // Arrange
            RegistryManager registryManager = RegistryManager.createFromConnectionString(connectionString);
            // Act
            registryManager.getStatistics();
        }
    }
}