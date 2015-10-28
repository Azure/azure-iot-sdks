/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.sdk;

import com.microsoft.azure.iot.service.auth.IotHubServiceSasToken;
import com.microsoft.azure.iot.service.exceptions.IotHubException;
import com.microsoft.azure.iot.service.exceptions.IotHubExceptionManager;
import com.microsoft.azure.iot.service.transport.http.HttpResponse;
import com.microsoft.azure.iot.service.transport.http.HttpMethod;
import com.microsoft.azure.iot.service.transport.http.HttpRequest;

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

/**
 * Use the RegistryManager class to manage the identity registry in IoT Hubs.
 */
public class RegistryManager
{
    private final Integer DEFAULT_HTTP_TIMOUT_MS = 24000;
    private final ExecutorService executor = Executors.newFixedThreadPool(10);
    private IotHubConnectionString iotHubConnectionString;

    /**
     * Static constructor to create instance from connection string
     *
     * @param connectionString The iot hub connection string
     * @return The instance of RegistryManager
     * @throws Exception This exception is thrown if the object creation failed
     */
    public static RegistryManager createFromConnectionString(String connectionString) throws Exception
    {
        // Codes_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_001: [The constructor shall throw IllegalArgumentException if the input string is null or empty]
        if (Tools.isNullOrEmpty(connectionString))
        {
            throw new IllegalArgumentException();
        }
        // Codes_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_002: [The constructor shall create an IotHubConnectionString object from the given connection string]
        IotHubConnectionString iotHubConnectionString = IotHubConnectionStringBuilder.createConnectionString(connectionString);

        // Codes_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_003: [The constructor shall create a new RegistryManager, stores the created IotHubConnectionString object and return with it]
        RegistryManager iotHubRegistryManager = new RegistryManager();
        iotHubRegistryManager.iotHubConnectionString = iotHubConnectionString;
        return iotHubRegistryManager;
    }

    /**
     * Placeholder for open registry operations
     */
    public void open() {}

    /**
     * Placeholder for close registry operations
     */
    public void close() {}

    /**
     * Add device using the given Device object
     * Return with the response device object from IotHub
     *
     * @param device The device object to add
     * @return The future object for the requested operation
     * @throws IOException This exception is thrown if the IO operation failed
     * @throws IotHubException This exception is thrown if the response verification failed
     */
    public Device addDevice(Device device) throws IOException, IotHubException
    {
        // Codes_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_004: [The constructor shall throw IllegalArgumentException if the input device is null]
        if (device == null)
        {
            throw new IllegalArgumentException("device cannot be null");
        }

        // Codes_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_005: [The function shall deserialize the given device object to Json string]
        String deviceJson = device.serializeToJson();
        // Codes_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_006: [The function shall get the URL for the device]
        URL url = iotHubConnectionString.getUrlDevice(device.getDeviceId());
        // Codes_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_007: [The function shall create a new SAS token for the device]
        String sasTokenString = new IotHubServiceSasToken(this.iotHubConnectionString).toString();

        // Codes_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_008: [The function shall create a new HttpRequest for adding the device to IotHub]
        HttpRequest request = new HttpRequest(url, HttpMethod.PUT, deviceJson.getBytes());
        request.setReadTimeoutMillis(DEFAULT_HTTP_TIMOUT_MS);
        request.setHeaderField("authorization", sasTokenString);
        request.setHeaderField("Request-Id", "1001");
        request.setHeaderField("Accept", "application/json");
        request.setHeaderField("Content-Type", "application/json");
        request.setHeaderField("charset", "utf-8");

        // Codes_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_009: [The function shall send the created request and get the response]
        HttpResponse response = request.send();

        // Codes_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_010: [The function shall verify the response status and throw proper Exception]
        IotHubExceptionManager.httpResponseVerification(response);

        // Codes_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_011: [The function shall create a new Device object from the response and return with it]
        String bodyStr = new String(response.getBody(), StandardCharsets.UTF_8);
        Device iotHubDevice = Device.createFromJson(bodyStr);
        return iotHubDevice;
    }

    /**
     * Async wrapper for add() operation
     *
     * @param device The device object to add
     * @return The future object for the requested operation
     * @throws IOException This exception is thrown if the IO operation failed
     * @throws IotHubException This exception is thrown if the response verification failed
     */
    public CompletableFuture<Device> addDeviceAsync(Device device) throws IOException, IotHubException
    {
        // Codes_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_012: [The constructor shall throw IllegalArgumentException if the input device is null]
        if (device == null)
        {
            throw new IllegalArgumentException("device cannot be null");
        }

        // Codes_SRS_SERVICE_SDK_JAVA_ REGISTRYMANAGER_12_013: [The function shall create an async wrapper around the addDevice() function call, handle the return value or delegate exception]
        final CompletableFuture<Device> future = new CompletableFuture<>();
        executor.submit(() -> {
            try
            {
                Device responseDevice = addDevice(device);
                future.complete(responseDevice);
            } catch (IOException | IotHubException e)
            {
                future.completeExceptionally(e);
            }
        });
        return future;
    }

    /**
     * Get device data by device Id from IotHub
     *
     * @param deviceId The id of requested device
     * @return The device object of requested device
     * @throws IOException This exception is thrown if the IO operation failed
     * @throws IotHubException This exception is thrown if the response verification failed
     */
    public Device getDevice(String deviceId) throws IOException, IotHubException
    {
        // Codes_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_014: [The constructor shall throw IllegalArgumentException if the input string is null or empty]
        if (Tools.isNullOrEmpty(deviceId))
        {
            throw new IllegalArgumentException("deviceId cannot be null or empty");
        }

        // Codes_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_015: [The function shall get the URL for the device]
        URL url = iotHubConnectionString.getUrlDevice(deviceId);
        // Codes_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_016: [The function shall create a new SAS token for the device]
        String sasTokenString = new IotHubServiceSasToken(this.iotHubConnectionString).toString();

        // Codes_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_017: [The function shall create a new HttpRequest for getting a device from IotHub]
        HttpRequest request = new HttpRequest(url, HttpMethod.GET, new byte[0]);
        request.setReadTimeoutMillis(DEFAULT_HTTP_TIMOUT_MS);
        request.setHeaderField("authorization", sasTokenString);
        request.setHeaderField("Request-Id", "1001");
        request.setHeaderField("Accept", "application/json");
        request.setHeaderField("Content-Type", "application/json");
        request.setHeaderField("charset", "utf-8");

        // Codes_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_018: [The function shall send the created request and get the response]
        HttpResponse response = request.send();

        // Codes_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_019: [The function shall verify the response status and throw proper Exception]
        IotHubExceptionManager.httpResponseVerification(response);

        // Codes_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_020: [The function shall create a new Device object from the response and return with it]
        String bodyStr = new String(response.getBody(), StandardCharsets.UTF_8);
        Device iotHubDevice = Device.createFromJson(bodyStr);
        return iotHubDevice;
    }

    /**
     * Async wrapper for getDevice() operation
     *
     * @param deviceId The id of requested device
     * @return The future object for the requested operation
     * @throws IOException This exception is thrown if the IO operation failed
     * @throws IotHubException This exception is thrown if the response verification failed
     */
    public CompletableFuture<Device> getDeviceAsync(String deviceId) throws IOException, IotHubException
    {
        // Codes_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_021: [The constructor shall throw IllegalArgumentException if the input device is null]
        if (Tools.isNullOrEmpty(deviceId))
        {
            throw new IllegalArgumentException("deviceId cannot be null or empty");
        }

        // Codes_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_022: [The function shall create an async wrapper around the addDevice() function call, handle the return value or delegate exception]
        final CompletableFuture<Device> future = new CompletableFuture<>();
        executor.submit(() -> {
            try
            {
                Device responseDevice = getDevice(deviceId);
                future.complete(responseDevice);
            } catch (IotHubException | IOException e)
            {
                future.completeExceptionally(e);
            }
        });
        return future;
    }

    /**
     * Get list of devices
     *
     * @param maxCount The requested count of devices
     * @return The array of requested device objects
     * @throws IOException This exception is thrown if the IO operation failed
     * @throws IotHubException This exception is thrown if the response verification failed
     */
    public ArrayList<Device> getDevices(Integer maxCount) throws IOException, IotHubException
    {
        // Codes_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_023: [The constructor shall throw IllegalArgumentException if the input count number is less than 1]
        if (maxCount < 1)
        {
            throw new IllegalArgumentException("maxCount cannot be less then 1");
        }

        // Codes_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_024: [The function shall get the URL for the device]
        URL url = iotHubConnectionString.getUrlDeviceList(maxCount);
        // Codes_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_025: [The function shall create a new SAS token for the device]
        String sasTokenString = new IotHubServiceSasToken(this.iotHubConnectionString).toString();

        // Codes_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_026: [The function shall create a new HttpRequest for getting a device list from IotHub]
        HttpRequest request = new HttpRequest(url, HttpMethod.GET, new byte[0]);
        request.setReadTimeoutMillis(DEFAULT_HTTP_TIMOUT_MS);
        request.setHeaderField("authorization", sasTokenString);
        request.setHeaderField("Request-Id", "1001");
        request.setHeaderField("Accept", "application/json");
        request.setHeaderField("Content-Type", "application/json");
        request.setHeaderField("charset", "utf-8");

        // Codes_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_027: [The function shall send the created request and get the response]
        HttpResponse response = request.send();

        // Codes_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_028: [The function shall verify the response status and throw proper Exception]
        IotHubExceptionManager.httpResponseVerification(response);

        // Codes_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_029: [The function shall create a new ArrayList<Device> object from the response and return with it]
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

    /**
     * Async wrapper for getDevices() operation
     *
     * @param maxCount The requested count of devices
     * @return The future object for the requested operation
     * @throws IOException This exception is thrown if the IO operation failed
     * @throws IotHubException This exception is thrown if the response verification failed
     */
    public CompletableFuture<ArrayList<Device>> getDevicesAsync(Integer maxCount) throws IOException, IotHubException
    {
        // Codes_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_030: [The function shall throw IllegalArgumentException if the input count number is less than 1]
        if (maxCount < 1)
        {
            throw new IllegalArgumentException("maxCount cannot be less then 1");
        }

        // Codes_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_031: [The function shall create an async wrapper around the getDevices() function call, handle the return value or delegate exception]
        final CompletableFuture<ArrayList<Device>> future = new CompletableFuture<>();
        executor.submit(() -> {
            try
            {
                ArrayList<Device> response = getDevices(maxCount);
                future.complete(response);
            } catch (IotHubException | IOException e)
            {
                future.completeExceptionally(e);
            }
        });
        return future;
    }

    /**
     * Update device not forced
     *
     * @param device The device object containing updated data
     * @return The updated device object
     * @throws IOException This exception is thrown if the IO operation failed
     * @throws IotHubException This exception is thrown if the response verification failed
     */
    public Device updateDevice(Device device) throws IOException, IotHubException
    {
        // Codes_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_032: [The function shall throw IllegalArgumentException if the input device is null]
        if (device == null)
        {
            throw new IllegalArgumentException("device cannot be null");
        }
        // Codes_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_033: [The function shall call updateDevice with forceUpdate = false]
        return updateDevice(device, false);
    }

    /**
     * Update device with forceUpdate input parameter
     *
     * @param device The device object containing updated data
     * @param forceUpdate True is the update has to be forced regardless if the device state
     * @return The updated device object
     * @throws IOException This exception is thrown if the IO operation failed
     * @throws IotHubException This exception is thrown if the response verification failed
     */
    public Device updateDevice(Device device, Boolean forceUpdate) throws IOException, IotHubException
    {
        // Codes_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_034: [The function shall throw IllegalArgumentException if the input device is null]
        if (device == null)
        {
            throw new IllegalArgumentException("device cannot be null");
        }

        // Codes_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_035: [The function shall set forceUpdate on the device]
        device.setForceUpdate(forceUpdate);

        // Codes_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_036: [The function shall get the URL for the device]
        URL url = iotHubConnectionString.getUrlDevice(device.getDeviceId());
        // Codes_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_037: [The function shall create a new SAS token for the device]
        String sasTokenString = new IotHubServiceSasToken(this.iotHubConnectionString).toString();

        // Codes_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_038: [The function shall create a new HttpRequest for updating the device on IotHub]
        HttpRequest request = new HttpRequest(url, HttpMethod.PUT, device.serializeToJson().getBytes());
        request.setReadTimeoutMillis(DEFAULT_HTTP_TIMOUT_MS);
        request.setHeaderField("authorization", sasTokenString.toString());
        request.setHeaderField("Request-Id", "1001");
        request.setHeaderField("If-Match", "*");
        request.setHeaderField("Accept", "application/json");
        request.setHeaderField("Content-Type", "application/json");
        request.setHeaderField("charset", "utf-8");

        // Codes_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_039: [The function shall send the created request and get the response]
        HttpResponse response = request.send();

        // Codes_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_040: [The function shall verify the response status and throw proper Exception]
        IotHubExceptionManager.httpResponseVerification(response);

        // Codes_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_041: [The function shall create a new Device object from the response and return with it]
        String bodyStr = new String(response.getBody(), StandardCharsets.UTF_8);
        Device iotHubDevice = Device.createFromJson(bodyStr);
        return iotHubDevice;
    }

    /**
     * Async wrapper for updateDevice() operation
     *
     * @param device The device object containing updated data
     * @return The future object for the requested operation
     * @throws IOException This exception is thrown if the IO operation failed
     * @throws IotHubException This exception is thrown if the response verification failed
     */
    public CompletableFuture<Device> updateDeviceAsync(Device device) throws IOException, IotHubException
    {
        // Codes_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_042: [The function shall throw IllegalArgumentException if the input device is null]
        if (device == null)
        {
            throw new IllegalArgumentException("device cannot be null");
        }
        // Codes_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_043: [The function shall create an async wrapper around the updateDevice() function call, handle the return value or delegate exception]
        final CompletableFuture<Device> future = new CompletableFuture<>();
        executor.submit(() -> {
            try
            {
                Device responseDevice = updateDevice(device);
                future.complete(responseDevice);
            } catch (IotHubException | IOException e)
            {
                future.completeExceptionally(e);
            }
        });
        return future;
    }

    /**
     * Async wrapper for forced updateDevice() operation
     *
     * @param device The device object containing updated data
     * @param forceUpdate True is the update has to be forced regardless if the device state
     * @return The future object for the requested operation
     * @throws IOException This exception is thrown if the IO operation failed
     * @throws IotHubException This exception is thrown if the response verification failed
     */
    public CompletableFuture<Device> updateDeviceAsync(Device device, Boolean forceUpdate) throws IOException, IotHubException
    {
        // Codes_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_044: [The function shall throw IllegalArgumentException if the input device is null]
        if (device == null)
        {
            throw new IllegalArgumentException("device cannot be null");
        }
        // Codes_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_045: [The function shall create an async wrapper around the updateDevice(Device, device, Boolean forceUpdate) function call, handle the return value or delegate exception]
        final CompletableFuture<Device> future = new CompletableFuture<>();
        executor.submit(() -> {
            try
            {
                Device responseDevice = updateDevice(device, forceUpdate);
                future.complete(responseDevice);
            } catch (IotHubException | IOException e)
            {
                future.completeExceptionally(e);
            }
        });
        return future;
    }

    /**
     * Remove device
     *
     * @param deviceId The device name to remove
     * @throws IOException This exception is thrown if the IO operation failed
     * @throws IotHubException This exception is thrown if the response verification failed
     */
    public void removeDevice(String deviceId) throws IOException, IotHubException
    {
        // Codes_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_046: [The function shall throw IllegalArgumentException if the input string is null or empty]
        if (Tools.isNullOrEmpty(deviceId))
        {
            throw new IllegalArgumentException("deviceId cannot be null or empty");
        }

        // Codes_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_047: [The function shall get the URL for the device]
        URL url = iotHubConnectionString.getUrlDevice(deviceId);
        // Codes_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_048: [The function shall create a new SAS token for the device]
        String sasToken = new IotHubServiceSasToken(this.iotHubConnectionString).toString();

        // Codes_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_049: [The function shall create a new HttpRequest for removing the device from IotHub]
        HttpRequest request = new HttpRequest(url, HttpMethod.DELETE, new byte[0]);
        request.setReadTimeoutMillis(DEFAULT_HTTP_TIMOUT_MS);
        request.setHeaderField("authorization", sasToken);
        request.setHeaderField("If-Match", "*");

        // Codes_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_050: [The function shall send the created request and get the response]
        HttpResponse response = request.send();

        // Codes_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_051: [The function shall verify the response status and throw proper Exception]
        IotHubExceptionManager.httpResponseVerification(response);
    }

    /**
     * Async wrapper for removeDevice() operation
     *
     * @param deviceId The device object to remove
     * @return The future object for the requested operation
     * @throws IOException This exception is thrown if the IO operation failed
     * @throws IotHubException This exception is thrown if the response verification failed
     */
    public CompletableFuture<Boolean> removeDeviceAsync(String deviceId) throws IOException, IotHubException
    {
        // Codes_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_052: [The function shall throw IllegalArgumentException if the input string is null or empty]
        if (Tools.isNullOrEmpty(deviceId))
        {
            throw new IllegalArgumentException("deviceId cannot be null or empty");
        }

        // Codes_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_053: [The function shall create an async wrapper around the removeDevice() function call, handle the return value or delegate exception]
        final CompletableFuture<Boolean> future = new CompletableFuture<Boolean>();
        executor.submit(() -> {
            try
            {
                removeDevice(deviceId);
                future.complete(true);
            } catch (IotHubException | IOException e)
            {
                future.completeExceptionally(e);
            }
        });
        return future;
    }

    /**
     * Get device statistics
     *
     * @return RegistryStatistics object containing the requested data
     * @throws IOException This exception is thrown if the IO operation failed
     * @throws IotHubException This exception is thrown if the response verification failed
     */
    public RegistryStatistics getStatistics() throws IOException, IotHubException
    {
        // Codes_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_054: [The function shall get the URL for the device]
        URL url = iotHubConnectionString.getUrlDeviceStatistics();
        // Codes_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_055: [The function shall create a new SAS token for the device]
        String sasTokenString = new IotHubServiceSasToken(this.iotHubConnectionString).toString();

        // Codes_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_056: [The function shall create a new HttpRequest for getting statistics a device from IotHub]
        HttpRequest request = new HttpRequest(url, HttpMethod.GET, new byte[0]);
        request.setReadTimeoutMillis(DEFAULT_HTTP_TIMOUT_MS);
        request.setHeaderField("authorization", sasTokenString);
        request.setHeaderField("Request-Id", "1001");
        request.setHeaderField("Accept", "application/json");
        request.setHeaderField("Content-Type", "application/json");
        request.setHeaderField("charset", "utf-8");

        // Codes_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_057: [The function shall send the created request and get the response]
        HttpResponse response = request.send();

        // Codes_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_058: [The function shall verify the response status and throw proper Exception]
        IotHubExceptionManager.httpResponseVerification(response);

        // Codes_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_059: [The function shall create a new RegistryStatistics object from the response and return with it]
        String bodyStr = new String(response.getBody(), StandardCharsets.UTF_8);
        RegistryStatistics registryStatistics = RegistryStatistics.parse(bodyStr);
        return registryStatistics;
    }

    /**
     * Async wrapper for getStatistics() operation
     *
     * @return The future object for the requested operation
     * @throws IOException This exception is thrown if the IO operation failed
     * @throws IotHubException This exception is thrown if the response verification failed
     */
    public CompletableFuture<RegistryStatistics> getStatisticsAsync() throws IOException, IotHubException
    {
        // Codes_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_060: [The function shall create an async wrapper around the getStatistics() function call, handle the return value or delegate exception]
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
