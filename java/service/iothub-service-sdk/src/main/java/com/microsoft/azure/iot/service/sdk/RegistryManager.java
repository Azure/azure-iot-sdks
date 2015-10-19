/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.sdk;

import com.microsoft.azure.iot.service.exceptions.IotHubException;
import java.io.IOException;
import java.util.ArrayList;
import java.util.concurrent.CompletableFuture;

public abstract class RegistryManager
{
    public static RegistryManager createFromConnectionString(String connectionString) throws Exception
    {
        if (connectionString == null)
        {
            throw new IllegalArgumentException();
        }
        IotHubConnectionString iotHubConnectionString = IotHubConnectionStringBuilder.createConnectionString(connectionString);
        IotHubRegistryManager iotHubRegistryManager = new IotHubRegistryManager(iotHubConnectionString);
        return iotHubRegistryManager;
    }

    public abstract void open();

    public abstract void close();

    public abstract Device addDevice(Device device) throws IOException, IotHubException;
    public abstract CompletableFuture<Device> addDeviceAsync(Device device) throws IOException, IotHubException;

    public abstract Device getDevice(String deviceId) throws IOException, IotHubException;
    public abstract CompletableFuture<Device> getDeviceAsync(String deviceId) throws IOException, IotHubException;

    public abstract ArrayList<Device> getDevices(Integer maxCount) throws IOException, IotHubException;
    public abstract CompletableFuture<ArrayList<Device>> getDevicesAsync(Integer maxCount) throws IOException, IotHubException;

    public abstract Device updateDevice(Device device) throws IOException, IotHubException;
    public abstract Device updateDevice(Device device, Boolean forceUpdate) throws IOException, IotHubException;
    public abstract CompletableFuture<Device> updateDeviceAsync(Device device) throws IOException, IotHubException;
    public abstract CompletableFuture<Device> updateDeviceAsync(Device device, Boolean forceUpdate) throws IOException, IotHubException;

    public abstract void removeDevice(String deviceId) throws IOException, IotHubException;
    public abstract CompletableFuture<Void> removeDeviceAsync(String deviceId) throws IOException, IotHubException;

    public abstract RegistryStatistics getStatistics() throws IOException, IotHubException;
    public abstract CompletableFuture<RegistryStatistics> getStatisticsAsync() throws IOException, IotHubException;
}
