/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.sdk;

import mockit.integration.junit4.JMockit;
import org.junit.Test;
import org.junit.runner.RunWith;

import java.util.ArrayList;
import java.util.concurrent.CompletableFuture;

@RunWith(JMockit.class)
public class RegistryManagerE2ETest
{
    private static String connectionString = "";
    private static String deviceId = "xxx-test-device";

    @Test
    public void addDeviceTest() throws Exception
    {
        if (!Tools.isNullOrEmpty(connectionString))
        {
            // Arrange
            RegistryManager regmen = RegistryManager.createFromConnectionString(connectionString);
            Device device = Device.createFromId(deviceId);
            // Act
            regmen.addDevice(device);
        }
    }

    @Test
    public void getDeviceTest() throws Exception
    {
        if (!Tools.isNullOrEmpty(connectionString))
        {
            // Arrange
            RegistryManager regmen = RegistryManager.createFromConnectionString(connectionString);
            // Act
            Device device = regmen.getDevice(deviceId);
        }
    }

    @Test
    public void getDevicesTest() throws Exception
    {
        if (!Tools.isNullOrEmpty(connectionString))
        {
            // Arrange
            RegistryManager regmen = RegistryManager.createFromConnectionString(connectionString);
            // Act
            ArrayList<Device> devices = regmen.getDevices(1000);
        }
    }

    @Test
    public void updateDeviceTest() throws Exception
    {
        if (!Tools.isNullOrEmpty(connectionString))
        {
            // Arrange
            RegistryManager regmen = RegistryManager.createFromConnectionString(connectionString);
            Device device = Device.createFromId(deviceId);
            // Act
            regmen.updateDevice(device);
        }
    }

    @Test
    public void removeDeviceTest() throws Exception
    {
        if (!Tools.isNullOrEmpty(connectionString))
        {
            // Arrange
            RegistryManager regmen = RegistryManager.createFromConnectionString(connectionString);
            // Act
            regmen.removeDevice(deviceId);
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

    @Test
    public void addDeviceAsyncTest() throws Exception
    {
        if (!Tools.isNullOrEmpty(connectionString))
        {
            // Arrange
            RegistryManager regmen = RegistryManager.createFromConnectionString(connectionString);
            Device device = Device.createFromId(deviceId);
            // Act
            CompletableFuture<Device> future = regmen.addDeviceAsync(device);
            while (!future.isDone())
            {
                System.out.println("Task is not completed yet...");
                Thread.sleep(10);
            }
            System.out.println(future.get());
        }
    }
}
