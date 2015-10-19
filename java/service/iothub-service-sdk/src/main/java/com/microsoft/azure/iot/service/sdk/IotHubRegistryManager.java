/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.sdk;

import com.microsoft.azure.iot.service.auth.IotHubServiceSasToken;
import com.microsoft.azure.iot.service.exceptions.IotHubException;
import com.microsoft.azure.iot.service.exceptions.IotHubExceptionManager;
import com.microsoft.azure.iot.service.transport.https.HttpsMethod;
import com.microsoft.azure.iot.service.transport.https.HttpsRequest;
import com.microsoft.azure.iot.service.transport.https.HttpsResponse;

import javax.json.Json;
import javax.json.JsonArray;
import javax.json.JsonObject;
import javax.json.JsonReader;
import java.io.IOException;
import java.io.StringReader;
import java.net.URL;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.concurrent.CompletableFuture;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class IotHubRegistryManager extends RegistryManager
{
    private final ExecutorService executor = Executors.newFixedThreadPool(10);

    private enum IotHubREgistryManagerActions
    {
        AddDevice,
        GetDevice,
        DeleteDevice
    }

    private IotHubConnectionString iotHubConnectionString;

    protected IotHubRegistryManager(IotHubConnectionString iotHubConnectionString)
    {
        if (iotHubConnectionString == null)
        {
            throw new IllegalArgumentException();
        }
        this.iotHubConnectionString = iotHubConnectionString;
    }

    private Boolean forceUpdate = false;

    @Override
    public void open() {}

    @Override
    public void close() {}

    @Override
    public Device addDevice(Device device) throws IOException, IotHubException
    {
        Integer timeoutSec = 24000;

        String deviceJson = device.serializeToJson();
        URL url = iotHubConnectionString.getUrlDevice(device.getId());
        String sasTokenString = new IotHubServiceSasToken(this.iotHubConnectionString).toString();

        HttpsRequest request =
                new HttpsRequest(url, HttpsMethod.PUT, deviceJson.getBytes()).
                        setReadTimeoutMillis(timeoutSec).
                        setHeaderField("authorization", sasTokenString).
                        setHeaderField("Request-Id", "1001").
                        setHeaderField("Accept", "application/json").
                        setHeaderField("Content-Type", "application/json").
                        setHeaderField("charset", "utf-8");

        HttpsResponse response = request.send();
        
        IotHubExceptionManager.HttpResponseVerification(IotHubExceptionManager.RegistryManagerAction.AddDevice, response);

        String bodyStr = new String(response.getBody(), StandardCharsets.UTF_8);
        Device iotHubDevice = Device.createFromJson(bodyStr);
        return iotHubDevice;
    }

    @Override
    public CompletableFuture<Device> addDeviceAsync(Device device) throws IOException, IotHubException
    {
        final CompletableFuture<Device> future = new CompletableFuture<>();
        executor.submit(() -> {
            try
            {
                Device responseDevice = addDevice(device);
                future.complete(responseDevice);
            } catch (IotHubException e)
            {
                future.completeExceptionally(e);
            } catch (IOException e)
            {
                future.completeExceptionally(e);
            }
        });
        return future;
    }

    @Override
    public Device getDevice(String deviceId) throws IOException, IotHubException
    {
        Integer timeoutSec = 24000;
        URL url = iotHubConnectionString.getUrlDevice(deviceId);
        String sasTokenString = new IotHubServiceSasToken(this.iotHubConnectionString).toString();

        HttpsRequest request =
                new HttpsRequest(url, HttpsMethod.GET, new byte[0]).
                        setReadTimeoutMillis(timeoutSec).
                        setHeaderField("authorization", sasTokenString).
                        setHeaderField("Request-Id", "1001").
                        setHeaderField("Accept", "application/json").
                        setHeaderField("Content-Type", "application/json").
                        setHeaderField("charset", "utf-8");

        HttpsResponse response = request.send();

        IotHubExceptionManager.HttpResponseVerification(IotHubExceptionManager.RegistryManagerAction.AddDevice, response);
        
        String bodyStr = new String(response.getBody(), StandardCharsets.UTF_8);
        Device iotHubDevice = Device.createFromJson(bodyStr);
        return iotHubDevice;
    }

    @Override
    public CompletableFuture<Device> getDeviceAsync(String deviceId) throws IOException, IotHubException
    {
        final CompletableFuture<Device> future = new CompletableFuture<>();
        executor.submit(() -> {
            try
            {
                Device responseDevice = getDevice(deviceId);
                future.complete(responseDevice);
            } catch (IotHubException e)
            {
                future.completeExceptionally(e);
            } catch (IOException e)
            {
                future.completeExceptionally(e);
            }
        });
        return future;
    }

    @Override
    public ArrayList<Device> getDevices(Integer maxCount) throws IOException, IotHubException
    {
        Integer timeoutSec = 24000;
        URL url = iotHubConnectionString.getUrlDeviceList(maxCount);
        String sasTokenString = new IotHubServiceSasToken(this.iotHubConnectionString).toString();

        HttpsRequest request =
                new HttpsRequest(url, HttpsMethod.GET, new byte[0]).
                        setReadTimeoutMillis(timeoutSec).
                        setHeaderField("authorization", sasTokenString).
                        setHeaderField("Request-Id", "1001").
                        setHeaderField("Accept", "application/json").
                        setHeaderField("Content-Type", "application/json").
                        setHeaderField("charset", "utf-8");

        HttpsResponse response = request.send();
        
        IotHubExceptionManager.HttpResponseVerification(IotHubExceptionManager.RegistryManagerAction.AddDevice, response);

        String bodyStr = new String(response.getBody(), StandardCharsets.UTF_8);
        try (JsonReader jsonReader = Json.createReader(new StringReader(bodyStr)))
        {
            ArrayList<Device> deviceList = new ArrayList<>();
            
            JsonArray deviceArray = jsonReader.readArray();
            
            for (int i = 0; i < deviceArray.size(); i++)
            {
                JsonObject jsonObject = deviceArray.getJsonObject(i);
                Device iotHubDevice = Device.createFromJson(jsonObject.toString());
                deviceList.add(iotHubDevice);
            }
            return deviceList;
        }
    }

    @Override
    public CompletableFuture<ArrayList<Device>> getDevicesAsync(Integer maxCount) throws IOException, IotHubException
    {
        final CompletableFuture<ArrayList<Device>> future = new CompletableFuture<>();
        executor.submit(() -> {
            try
            {
                ArrayList<Device> response = getDevices(maxCount);
                future.complete(response);
            } catch (IotHubException e)
            {
                future.completeExceptionally(e);
            } catch (IOException e)
            {
                future.completeExceptionally(e);
            }
        });
        return future;
    }

    @Override
    public Device updateDevice(Device device) throws IOException, IotHubException
    {
        if (forceUpdate)
        {
            device.setForceUpdate(true);
            this.forceUpdate = false;
        }

        Integer timeoutSec = 24000;
        URL url = iotHubConnectionString.getUrlDevice(device.getId());
        String sasTokenString = new IotHubServiceSasToken(this.iotHubConnectionString).toString();

        HttpsRequest request =
                new HttpsRequest(url, HttpsMethod.PUT, device.serializeToJson().getBytes()).
                        setReadTimeoutMillis(timeoutSec).
                        setHeaderField("authorization", sasTokenString.toString()).
                        setHeaderField("Request-Id", "1001").
                        setHeaderField("If-Match", "*").
                        setHeaderField("Accept", "application/json").
                        setHeaderField("Content-Type", "application/json").
                        setHeaderField("charset", "utf-8");

        HttpsResponse response = request.send();
        
        IotHubExceptionManager.HttpResponseVerification(IotHubExceptionManager.RegistryManagerAction.AddDevice, response);

        device.setForceUpdate(false);
        
        String bodyStr = new String(response.getBody(), StandardCharsets.UTF_8);
        Device iotHubDevice = Device.createFromJson(bodyStr);
        return iotHubDevice;
    }

    @Override
    public Device updateDevice(Device device, Boolean forceUpdate) throws IOException, IotHubException
    {
        this.forceUpdate = true;
        return updateDevice(device);
    }

    @Override
    public CompletableFuture<Device> updateDeviceAsync(Device device) throws IOException, IotHubException
    {
        final CompletableFuture<Device> future = new CompletableFuture<>();
        executor.submit(() -> {
            try
            {
                Device responseDevice = updateDevice(device);
                future.complete(responseDevice);
            } catch (IotHubException e)
            {
                future.completeExceptionally(e);
            } catch (IOException e)
            {
                future.completeExceptionally(e);
            }
        });
        return future;
    }

    @Override
    public CompletableFuture<Device> updateDeviceAsync(Device device, Boolean forceUpdate) throws IOException, IotHubException
    {
        this.forceUpdate = true;
        return updateDeviceAsync(device);
    }

    @Override
    public void removeDevice(String deviceId) throws IOException, IotHubException
    {
        Integer timeoutSec = 24000;
        URL url = iotHubConnectionString.getUrlDevice(deviceId);
        String sasToken = new IotHubServiceSasToken(this.iotHubConnectionString).toString();

        HttpsRequest request =
                new HttpsRequest(url, HttpsMethod.DELETE, new byte[0]).
                        setReadTimeoutMillis(timeoutSec).
                        setHeaderField("authorization", sasToken).
                        setHeaderField("If-Match", "*");

        HttpsResponse response = request.send();
        
        IotHubExceptionManager.HttpResponseVerification(IotHubExceptionManager.RegistryManagerAction.AddDevice, response);
    }

    @Override
    public CompletableFuture<Void> removeDeviceAsync(String deviceId) throws IOException, IotHubException
    {
        final CompletableFuture<Void> future = new CompletableFuture<>();
        executor.submit(() -> {
            try
            {
                removeDevice(deviceId);
            } catch (IotHubException e)
            {
                future.completeExceptionally(e);
            } catch (IOException e)
            {
                future.completeExceptionally(e);
            }
        });
        return future;
    }

    @Override
    public RegistryStatistics getStatistics() throws IOException, IotHubException
    {
        Integer timeoutSec = 24000;
        URL url = iotHubConnectionString.getUrlDeviceStatistics();
        String sasTokenString = new IotHubServiceSasToken(this.iotHubConnectionString).toString();

        HttpsRequest request =
                new HttpsRequest(url, HttpsMethod.GET, new byte[0]).
                        setReadTimeoutMillis(timeoutSec).
                        setHeaderField("authorization", sasTokenString).
                        setHeaderField("Request-Id", "1001").
                        setHeaderField("Accept", "application/json").
                        setHeaderField("Content-Type", "application/json").
                        setHeaderField("charset", "utf-8");

        HttpsResponse response = request.send();
        
        IotHubExceptionManager.HttpResponseVerification(IotHubExceptionManager.RegistryManagerAction.AddDevice, response);

        String bodyStr = new String(response.getBody(), StandardCharsets.UTF_8);
        RegistryStatistics registryStatistics = RegistryStatistics.createFromJson(bodyStr);
        return registryStatistics;
    }

    @Override
    public CompletableFuture<RegistryStatistics> getStatisticsAsync() throws IOException, IotHubException
    {
        final CompletableFuture<RegistryStatistics> future = new CompletableFuture<>();
        executor.submit(() -> {
            try
            {
                RegistryStatistics responseDevice = getStatistics();
                future.complete(responseDevice);
            } catch (IotHubException e)
            {
                future.completeExceptionally(e);
            } catch (IOException e)
            {
                future.completeExceptionally(e);
            }
        });
        return future;
    }
}
