/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.sdk;

import com.microsoft.azure.iot.service.exceptions.IotHubException;
import mockit.integration.junit4.JMockit;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.omg.CORBA.Environment;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Map;
import java.util.concurrent.CompletableFuture;

import static org.junit.Assert.assertEquals;

@RunWith(JMockit.class)
public class RegistryManagerE2ETest
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
        RegistryManager regmen = RegistryManager.createFromConnectionString(connectionString);
        try
        {
            Device device = regmen.getDevice(deviceId);
            deviceExist = true;
        } catch (IotHubException e)
        {
            System.out.println("Initialization OK, no device exist with name: " + deviceId);
        }
        if (deviceExist)
        {
            try
            {
                regmen.removeDevice(deviceId);
            } catch (IotHubException|IOException e)
            {
                System.out.println("Initialization failed, could not remove device: " + deviceId);
                e.printStackTrace();
            }
        }
        // Act

        // Create
        Device deviceAdded = Device.createFromId(deviceId);
        regmen.addDevice(deviceAdded);

        // Read
        Device deviceGet = regmen.getDevice(deviceId);

        // Delete
        regmen.removeDevice(deviceId);

        // Assert
        assertEquals(deviceId, deviceAdded.getDeviceId());
        assertEquals(deviceId, deviceGet.getDeviceId());
        try
        {
            Device device = regmen.getDevice(deviceId);
        } catch (IotHubException e)
        {
            System.out.println("Clean up OK, no device exist with name: " + deviceId);
        }
    }

    @Test
    public void getDeviceStatisticsTest() throws Exception
    {
        if (!Tools.isNullOrEmpty(connectionString))
        {
            // Arrange
            RegistryManager regmen = RegistryManager.createFromConnectionString(connectionString);
            // Act
            regmen.getStatistics();
        }
    }
}
