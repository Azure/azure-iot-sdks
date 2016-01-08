/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.sdk;

import com.microsoft.azure.iot.service.auth.IotHubServiceSasToken;
import com.microsoft.azure.iot.service.exceptions.IotHubException;
import com.microsoft.azure.iot.service.exceptions.IotHubExceptionManager;
import com.microsoft.azure.iot.service.transport.http.HttpMethod;
import com.microsoft.azure.iot.service.transport.http.HttpRequest;
import com.microsoft.azure.iot.service.transport.http.HttpResponse;
import mockit.*;
import mockit.integration.junit4.JMockit;
import org.junit.Test;
import org.junit.runner.RunWith;

import java.io.IOException;
import java.net.URL;
import java.util.ArrayList;
import java.util.concurrent.CompletableFuture;

import static org.junit.Assert.assertNotEquals;

@RunWith(JMockit.class)
public class RegistryManagerTest
{
    @Mocked
    Device device;
    @Mocked
    URL mockUrl;
    @Mocked
    HttpResponse mockHttpResponse;
    @Mocked
    IotHubServiceSasToken iotHubServiceSasToken;
    @Mocked
    IotHubExceptionManager mockIotHubExceptionManager;
    @Mocked
    RegistryStatistics registryStatistics;
    @Mocked
    IotHubConnectionString iotHubConnectionString;
    @Mocked
    HttpRequest mockHttpRequest;

    final String deviceJson = "{\"deviceId\":\"mockdevice\",\"generationId\":\"635864360921156105\",\"etag\":\"MA==\",\"" +
            "connectionState\":\"Disconnected\",\"connectionStateUpdatedTime\":\"0001-01-01T00:00:00\",\"status\":\"" +
            "disabled\",\"statusReason\":null,\"statusUpdatedTime\":\"0001-01-01T00:00:00\",\"" +
            "lastActivityTime\":\"0001-01-01T00:00:00\",\"cloudToDeviceMessageCount\":0,\n" +
            "\"authentication\":\n" +
            "{\"symmetricKey\":{\"primaryKey\":\"L1jehoyM+E2gQ5/x446tv9trVrQ2/AoS0BsXUnw3vgE=\",\"" +
            "secondaryKey\":\"JyJdmx4UnIj3KX1HAkTSguDEEDiz2VW4TmbjdY0+vaw=\"}}}";

    final String devicesJson = "[{\"encryptionMethod\":\"AES\",\"utcTimeDefault\":\"0001-01-01T00:00:00\",\"" +
            "deviceId\":\"java-crud-e2e-test-873e7831-5778-4b1e-a998-cf11fffa6415\",\"generationId\":\"\",\"" +
            "etag\":\"\",\"status\":\"disabled\",\"statusReason\":\"\",\"statusUpdatedTime\":\"0001-01-01T00:00:00\",\"" +
            "connectionState\":\"Disconnected\",\"connectionStateUpdatedTime\":\"0001-01-01T00:00:00\",\"" +
            "lastActivityTime\":\"0001-01-01T00:00:00\",\"cloudToDeviceMessageCount\":0,\"forceUpdate\":false,\"" +
            "authentication\":{\"symmetricKey\":{\"primaryKey\":\"CZJIeLzepSADZe3Z9mQsCg\\u003d\\u003d\",\"" +
            "secondaryKey\":\"wP7t1W95u6zF8ocRGSAoYQ\\u003d\\u003d\"}}},{\"encryptionMethod\":\"AES\",\"" +
            "utcTimeDefault\":\"0001-01-01T00:00:00\",\"deviceId\":\"" +
            "java-crud-e2e-test-873e7831-5778-4b1e-a998-cf11fffa64152\",\"generationId\":\"\",\"etag\":\"\",\"" +
            "status\":\"disabled\",\"statusReason\":\"\",\"statusUpdatedTime\":\"0001-01-01T00:00:00\",\"" +
            "connectionState\":\"Disconnected\",\"connectionStateUpdatedTime\":\"0001-01-01T00:00:00\",\"" +
            "lastActivityTime\":\"0001-01-01T00:00:00\",\"cloudToDeviceMessageCount\":0,\"forceUpdate\":false,\"" +
            "authentication\":{\"symmetricKey\":{\"primaryKey\":\"V0UYjIYYLTERQheXas+YUw\\u003d\\u003d\",\"" +
            "secondaryKey\":\"cDAeY0gZkA9UIxKmSqDGDg\\u003d\\u003d\"}}},{\"encryptionMethod\":\"AES\",\"" +
            "utcTimeDefault\":\"0001-01-01T00:00:00\",\"deviceId\":\"" +
            "java-crud-e2e-test-873e7831-5778-4b1e-a998-cf11fffa64153\",\"generationId\":\"\",\"etag\":\"\",\"" +
            "status\":\"disabled\",\"statusReason\":\"\",\"statusUpdatedTime\":\"0001-01-01T00:00:00\",\"" +
            "connectionState\":\"Disconnected\",\"connectionStateUpdatedTime\":\"0001-01-01T00:00:00\",\"" +
            "lastActivityTime\":\"0001-01-01T00:00:00\",\"cloudToDeviceMessageCount\":0,\"forceUpdate\":false,\"" +
            "authentication\":{\"symmetricKey\":{\"primaryKey\":\"kOZFCG80Ytfd89FFZWc4XQ\\u003d\\u003d\",\"" +
            "secondaryKey\":\"RnMYfSEbt8Ql/MhFoIpopA\\u003d\\u003d\"}}}]";

    final String registryStatisticsJson = "{\"totalDeviceCount\":45,\"enabledDeviceCount\":45,\"disabledDeviceCount\":0}";

    final String jobPropertiesJson = "{\"jobId\":\"some_guid\",\"type\":\"export\",\"progress\"" +
            ":0,\"outputBlobContainerUri\":\"https://myurl.com\",\"excludeKeysInExport\":true}";

    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_001: [The constructor shall throw IllegalArgumentException if the input string is null or empty]
    // Assert
    @Test (expected = IllegalArgumentException.class)
    public void constructor_input_null() throws Exception
    {
        String connectionString = null;

        RegistryManager.createFromConnectionString(connectionString);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_001: [The constructor shall throw IllegalArgumentException if the input string is null or empty]
    // Assert
    @Test (expected = IllegalArgumentException.class)
    public void constructor_input_empty() throws Exception
    {
        String connectionString = null;

        RegistryManager.createFromConnectionString(connectionString);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_002: [The constructor shall create an IotHubConnectionString object from the given connection string]
    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_003: [The constructor shall create a new RegistryManager, stores the created IotHubConnectionString object and return with it]
    @Test
    public void constructor_good_case() throws Exception
    {
        String connectionString = "HostName=aaa.bbb.ccc;SharedAccessKeyName=XXX;SharedAccessKey=YYY";

        new NonStrictExpectations()
        {
            {
                IotHubConnectionStringBuilder.createConnectionString(connectionString);
                result = iotHubConnectionString;
            }
        };

        RegistryManager registryManager = RegistryManager.createFromConnectionString(connectionString);

        assertNotEquals(null, registryManager);
        assertNotEquals(null, Deencapsulation.getField(registryManager, "iotHubConnectionString"));
    }

    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_004: [The constructor shall throw IllegalArgumentException if the input device is null]
    // Assert
    @Test (expected = IllegalArgumentException.class)
    public void addDevice_input_null() throws Exception
    {
        String connectionString = "HostName=aaa.bbb.ccc;SharedAccessKeyName=XXX;SharedAccessKey=YYY";
        RegistryManager registryManager = RegistryManager.createFromConnectionString(connectionString);

        registryManager.addDevice(null);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_005: [The function shall deserialize the given device object to Json string]
    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_006: [The function shall get the URL for the device]
    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_007: [The function shall create a new SAS token for the device]
    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_008: [The function shall create a new HttpRequest for adding the device to IotHub]
    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_009: [The function shall send the created request and get the response]
    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_010: [The function shall verify the response status and throw proper Exception]
    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_011: [The function shall create a new Device object from the response and return with it]
    @Test
    public void addDevice_good_case() throws Exception
    {
        String connectionString = "HostName=aaa.bbb.ccc;SharedAccessKeyName=XXX;SharedAccessKey=YYY";
        String deviceId = "somedevice";

        new NonStrictExpectations()
        {
            {
                device.getDeviceId();
                result = deviceId;
            }
        };

        commonExpectations(connectionString, deviceId);

        RegistryManager registryManager = RegistryManager.createFromConnectionString(connectionString);
        Device returnDevice = registryManager.addDevice(device);
        registryManager.close();

        commonVerifications(HttpMethod.PUT, deviceId, returnDevice);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_012: [The function shall throw IllegalArgumentException if the input device is null]
    // Assert
    @Test (expected = IllegalArgumentException.class)
    public void addDeviceAsync_input_null() throws Exception
    {
        String connectionString = "HostName=aaa.bbb.ccc;SharedAccessKeyName=XXX;SharedAccessKey=YYY";
        RegistryManager registryManager = RegistryManager.createFromConnectionString(connectionString);

        registryManager.addDeviceAsync(null);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_ REGISTRYMANAGER_12_013: [The function shall create an async wrapper around the addDevice() function call, handle the return value or delegate exception]
    @Test
    public void addDeviceAsync_future_return_ok() throws Exception
    {
        String connectionString = "HostName=aaa.bbb.ccc;SharedAccessKeyName=XXX;SharedAccessKey=YYY";
        String deviceId = "somedevice";
        new NonStrictExpectations()
        {
            {
                device.getDeviceId();
                result = deviceId;
            }
        };

        commonExpectations(connectionString, deviceId);

        RegistryManager registryManager = RegistryManager.createFromConnectionString(connectionString);
        CompletableFuture<Device> completableFuture =  registryManager.addDeviceAsync(device);
        Device returnDevice = completableFuture.get();

        commonVerifications(HttpMethod.PUT, deviceId, returnDevice);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_ REGISTRYMANAGER_12_013: [The function shall create an async wrapper around the addDevice() function call, handle the return value or delegate exception]
    // Assert
    @Test (expected = Exception.class)
    public void addDeviceAsync_future_throw() throws Exception
    {
        new MockUp<RegistryManager>()
        {
            @Mock
            public Device addDevice(Device device) throws IOException, IotHubException
            {
                throw new IOException();
            }
        };

        String connectionString = "HostName=aaa.bbb.ccc;SharedAccessKeyName=XXX;SharedAccessKey=YYY";
        RegistryManager registryManager = RegistryManager.createFromConnectionString(connectionString);

        CompletableFuture<Device> completableFuture =  registryManager.addDeviceAsync(device);
        completableFuture.get();
    }

    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_014: [The constructor shall throw IllegalArgumentException if the input string is null or empty]
    // Assert
    @Test (expected = IllegalArgumentException.class)
    public void getDevice_input_null() throws Exception
    {
        String connectionString = "HostName=aaa.bbb.ccc;SharedAccessKeyName=XXX;SharedAccessKey=YYY";
        RegistryManager registryManager = RegistryManager.createFromConnectionString(connectionString);

        registryManager.getDevice(null);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_015: [The function shall get the URL for the device]
    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_016: [The function shall create a new SAS token for the device]
    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_017: [The function shall create a new HttpRequest for getting a device from IotHub]
    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_018: [The function shall send the created request and get the response]
    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_019: [The function shall verify the response status and throw proper Exception]
    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_020: [The function shall create a new Device object from the response and return with it]
    @Test
    public void getDevice_good_case() throws Exception
    {
        String connectionString = "HostName=aaa.bbb.ccc;SharedAccessKeyName=XXX;SharedAccessKey=YYY";
        String deviceId = "somedevice";

        commonExpectations(connectionString, deviceId);

        RegistryManager registryManager = RegistryManager.createFromConnectionString(connectionString);
        Device returnDevice = registryManager.getDevice(deviceId);

        commonVerifications(HttpMethod.GET, deviceId, returnDevice);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_021: [The constructor shall throw IllegalArgumentException if the input device is null]
    // Assert
    @Test (expected = IllegalArgumentException.class)
    public void getDeviceAsync_input_null() throws Exception
    {
        String connectionString = "HostName=aaa.bbb.ccc;SharedAccessKeyName=XXX;SharedAccessKey=YYY";
        RegistryManager registryManager = RegistryManager.createFromConnectionString(connectionString);

        registryManager.getDeviceAsync(null);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_022: [The function shall create an async wrapper around the addDevice() function call, handle the return value or delegate exception]
    @Test
    public void getDeviceAsync_future_return_ok() throws Exception
    {
        String connectionString = "HostName=aaa.bbb.ccc;SharedAccessKeyName=XXX;SharedAccessKey=YYY";
        String deviceId = "somedevice";

        commonExpectations(connectionString, deviceId);

        RegistryManager registryManager = RegistryManager.createFromConnectionString(connectionString);
        CompletableFuture<Device> completableFuture =  registryManager.getDeviceAsync(deviceId);
        Device returnDevice = completableFuture.get();

        commonVerifications(HttpMethod.GET, deviceId, returnDevice);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_022: [The function shall create an async wrapper around the addDevice() function call, handle the return value or delegate exception]
    // Assert
    @Test (expected = Exception.class)
    public void getDeviceAsync_future_throw() throws Exception
    {
        String deviceId = "somedevice";
        new MockUp<RegistryManager>()
        {
            @Mock
            public Device getDevice(String deviceId) throws IOException, IotHubException
            {
                throw new IOException();
            }
        };
        String connectionString = "HostName=aaa.bbb.ccc;SharedAccessKeyName=XXX;SharedAccessKey=YYY";
        RegistryManager registryManager = RegistryManager.createFromConnectionString(connectionString);

        CompletableFuture<Device> completableFuture =  registryManager.getDeviceAsync(deviceId);
        completableFuture.get();
    }


    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_023: [The constructor shall throw IllegalArgumentException if the input count number is less than 1]
    // Assert
    @Test (expected = IllegalArgumentException.class)
    public void getDevices_input_zero() throws Exception
    {
        String connectionString = "HostName=aaa.bbb.ccc;SharedAccessKeyName=XXX;SharedAccessKey=YYY";
        RegistryManager registryManager = RegistryManager.createFromConnectionString(connectionString);

        registryManager.getDevices(0);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_024: [The function shall get the URL for the device]
    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_025: [The function shall create a new SAS token for the device]
    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_026: [The function shall create a new HttpRequest for getting a device list from IotHub]
    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_027: [The function shall send the created request and get the response]
    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_028: [The function shall verify the response status and throw proper Exception]
    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_029: [The function shall create a new ArrayList<Device> object from the response and return with it]
    @Test
    public void getDevices_good_case() throws Exception
    {
        String connectionString = "HostName=aaa.bbb.ccc;SharedAccessKeyName=XXX;SharedAccessKey=YYY";
        int numberOfDevices = 10;

        getDevicesExpectations(connectionString, numberOfDevices);

        RegistryManager registryManager = RegistryManager.createFromConnectionString(connectionString);
        ArrayList<Device> devices =  registryManager.getDevices(10);

        getDevicesVerifications(numberOfDevices, devices);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_030: [The function shall throw IllegalArgumentException if the input count number is less than 1]
    // Assert
    @Test (expected = IllegalArgumentException.class)
    public void getDevicesAsync_input_zero() throws Exception
    {

        String connectionString = "HostName=aaa.bbb.ccc;SharedAccessKeyName=XXX;SharedAccessKey=YYY";
        RegistryManager registryManager = RegistryManager.createFromConnectionString(connectionString);

        registryManager.getDevicesAsync(0);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_031: [The function shall create an async wrapper around the getDevices() function call, handle the return value or delegate exception]
    @Test
    public void getDevicesAsync_future_return_ok() throws Exception
    {
        String connectionString = "HostName=aaa.bbb.ccc;SharedAccessKeyName=XXX;SharedAccessKey=YYY";
        int numberOfDevices = 10;

        getDevicesExpectations(connectionString, numberOfDevices);

        RegistryManager registryManager = RegistryManager.createFromConnectionString(connectionString);
        CompletableFuture<ArrayList<Device>> completableFuture =  registryManager.getDevicesAsync(10);
        ArrayList<Device> devices = completableFuture.get();

        getDevicesVerifications(numberOfDevices, devices);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_031: [The function shall create an async wrapper around the getDevices() function call, handle the return value or delegate exception]
    // Assert
    @Test (expected = Exception.class)
    public void getDevicesAsync_future_throw() throws Exception
    {
        new MockUp<RegistryManager>()
        {
            @Mock
            public Device getDevices(Integer maxCount) throws IOException, IotHubException
            {
                throw new IOException();
            }
        };
        String connectionString = "HostName=aaa.bbb.ccc;SharedAccessKeyName=XXX;SharedAccessKey=YYY";
        RegistryManager registryManager = RegistryManager.createFromConnectionString(connectionString);

        CompletableFuture<ArrayList<Device>> completableFuture = registryManager.getDevicesAsync(10);
        completableFuture.get();
    }

    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_032: [The function shall throw IllegalArgumentException if the input device is null]
    // Assert
    @Test (expected = IllegalArgumentException.class)
    public void updateDevice_input_null() throws Exception
    {
        String connectionString = "HostName=aaa.bbb.ccc;SharedAccessKeyName=XXX;SharedAccessKey=YYY";
        RegistryManager registryManager = RegistryManager.createFromConnectionString(connectionString);

        registryManager.updateDevice(null);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_033: [The function shall call updateDevice with forceUpdate = false]
    @Test
    public void updateDevice_good_case() throws Exception
    {
        String connectionString = "HostName=aaa.bbb.ccc;SharedAccessKeyName=XXX;SharedAccessKey=YYY";
        String deviceId = "somedevice";

        new NonStrictExpectations()
        {
            {
                device.getDeviceId();
                result = deviceId;
            }
        };

        commonExpectations(connectionString, deviceId);

        RegistryManager registryManager = RegistryManager.createFromConnectionString(connectionString);
        Device returnDevice = registryManager.updateDevice(device);
        commonVerifications(HttpMethod.PUT, deviceId, returnDevice);

        new VerificationsInOrder()
        {
            {
                device.setForceUpdate(false);
                mockHttpRequest.setHeaderField("If-Match", "*");
            }
        };
    }

    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_034: [The function shall throw IllegalArgumentException if the input device is null]
    // Assert
    @Test (expected = IllegalArgumentException.class)
    public void updateDeviceForce_input_null() throws Exception
    {
        String connectionString = "HostName=aaa.bbb.ccc;SharedAccessKeyName=XXX;SharedAccessKey=YYY";
        RegistryManager registryManager = RegistryManager.createFromConnectionString(connectionString);

        registryManager.updateDevice(null, true);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_035: [The function shall set forceUpdate on the device]
    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_036: [The function shall get the URL for the device]
    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_037: [The function shall create a new SAS token for the device]
    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_038: [The function shall create a new HttpRequest for updating the device on IotHub]
    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_039: [The function shall send the created request and get the response]
    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_040: [The function shall verify the response status and throw proper Exception]
    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_041: [The function shall create a new Device object from the response and return with it]
    @Test
    public void updateDeviceForce_good_case() throws Exception
    {
        String connectionString = "HostName=aaa.bbb.ccc;SharedAccessKeyName=XXX;SharedAccessKey=YYY";
        String deviceId = "somedevice";

        new NonStrictExpectations()
        {
            {
                device.getDeviceId();
                result = deviceId;
            }
        };

        commonExpectations(connectionString, deviceId);

        RegistryManager registryManager = RegistryManager.createFromConnectionString(connectionString);
        Device returnDevice = registryManager.updateDevice(device, true);

        commonVerifications(HttpMethod.PUT, deviceId, returnDevice);

        new VerificationsInOrder()
        {
            {
                device.setForceUpdate(true);
                mockHttpRequest.setHeaderField("If-Match", "*");
            }
        };
    }

    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_042: [The function shall throw IllegalArgumentException if the input device is null]
    // Assert
    @Test (expected = IllegalArgumentException.class)
    public void updateDeviceAsync_input_null() throws Exception
    {
        String connectionString = "HostName=aaa.bbb.ccc;SharedAccessKeyName=XXX;SharedAccessKey=YYY";
        RegistryManager registryManager = RegistryManager.createFromConnectionString(connectionString);

        registryManager.updateDeviceAsync(null);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_043: [The function shall create an async wrapper around the updateDevice() function call, handle the return value or delegate exception]
    @Test
    public void updateDeviceAsync_future_return_ok() throws Exception
    {
        String connectionString = "HostName=aaa.bbb.ccc;SharedAccessKeyName=XXX;SharedAccessKey=YYY";
        String deviceId = "somedevice";

        new NonStrictExpectations()
        {
            {
                device.getDeviceId();
                result = deviceId;
            }
        };

        commonExpectations(connectionString, deviceId);

        RegistryManager registryManager = RegistryManager.createFromConnectionString(connectionString);
        CompletableFuture<Device> completableFuture = registryManager.updateDeviceAsync(device);
        Device returnDevice = completableFuture.get();

        commonVerifications(HttpMethod.PUT, deviceId, returnDevice);

        new VerificationsInOrder()
        {
            {
                device.setForceUpdate(false);
                mockHttpRequest.setHeaderField("If-Match", "*");
            }
        };
    }

    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_043: [The function shall create an async wrapper around the updateDevice() function call, handle the return value or delegate exception]
    // Assert
    @Test (expected = Exception.class)
    public void updateDeviceAsync_future_throw() throws Exception
    {
        new MockUp<RegistryManager>()
        {
            @Mock
            public Device updateDevice(Device device) throws IOException, IotHubException
            {
                throw new IOException();
            }
        };
        String connectionString = "HostName=aaa.bbb.ccc;SharedAccessKeyName=XXX;SharedAccessKey=YYY";
        RegistryManager registryManager = RegistryManager.createFromConnectionString(connectionString);

        CompletableFuture<Device> completableFuture = registryManager.updateDeviceAsync(device);
        completableFuture.get();
    }

    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_044: [The function shall throw IllegalArgumentException if the input device is null]
    // Assert
    @Test (expected = IllegalArgumentException.class)
    public void updateDeviceAsyncForce_input_null() throws Exception
    {
        String connectionString = "HostName=aaa.bbb.ccc;SharedAccessKeyName=XXX;SharedAccessKey=YYY";
        RegistryManager registryManager = RegistryManager.createFromConnectionString(connectionString);

        registryManager.updateDeviceAsync(null, true);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_045: [The function shall create an async wrapper around the updateDevice(Device, device, Boolean forceUpdate) function call, handle the return value or delegate exception]
    @Test
    public void updateDeviceAsyncForce_future_return_ok() throws Exception
    {
        String connectionString = "HostName=aaa.bbb.ccc;SharedAccessKeyName=XXX;SharedAccessKey=YYY";
        String deviceId = "somedevice";

        new NonStrictExpectations()
        {
            {
                device.getDeviceId();
                result = deviceId;
            }
        };

        commonExpectations(connectionString, deviceId);

        RegistryManager registryManager = RegistryManager.createFromConnectionString(connectionString);
        CompletableFuture<Device> completableFuture =  registryManager.updateDeviceAsync(device, true);
        Device returnDevice = completableFuture.get();

        commonVerifications(HttpMethod.PUT, deviceId, returnDevice);

        new VerificationsInOrder()
        {
            {
                device.setForceUpdate(true);
                mockHttpRequest.setHeaderField("If-Match", "*");
            }
        };
    }

    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_045: [The function shall create an async wrapper around the updateDevice(Device, device, Boolean forceUpdate) function call, handle the return value or delegate exception]
    // Assert
    @Test (expected = Exception.class)
    public void updateDeviceAsyncForce_future_throw() throws Exception
    {
        new MockUp<RegistryManager>()
        {
            @Mock
            public Device updateDevice(Device device, Boolean forceUpdate) throws IOException, IotHubException
            {
                throw new IOException();
            }
        };
        String connectionString = "HostName=aaa.bbb.ccc;SharedAccessKeyName=XXX;SharedAccessKey=YYY";
        RegistryManager registryManager = RegistryManager.createFromConnectionString(connectionString);

        CompletableFuture<Device> completableFuture =  registryManager.updateDeviceAsync(device, true);
        completableFuture.get();
    }

    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_046: [The function shall throw IllegalArgumentException if the input string is null or empty]
    // Assert
    @Test (expected = IllegalArgumentException.class)
    public void removeDevice_input_null() throws Exception
    {
        String connectionString = "HostName=aaa.bbb.ccc;SharedAccessKeyName=XXX;SharedAccessKey=YYY";
        RegistryManager registryManager = RegistryManager.createFromConnectionString(connectionString);

        registryManager.removeDevice(null);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_046: [The function shall throw IllegalArgumentException if the input string is null or empty]
    // Assert
    @Test (expected = IllegalArgumentException.class)
    public void removeDevice_input_empty() throws Exception
    {
        String connectionString = "HostName=aaa.bbb.ccc;SharedAccessKeyName=XXX;SharedAccessKey=YYY";
        RegistryManager registryManager = RegistryManager.createFromConnectionString(connectionString);

        registryManager.removeDevice("");
    }

    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_047: [The function shall get the URL for the device]
    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_048: [The function shall create a new SAS token for the device]
    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_049: [The function shall create a new HttpRequest for removing the device from IotHub]
    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_050: [The function shall send the created request and get the response]
    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_051: [The function shall verify the response status and throw proper Exception]
    @Test
    public void removeDevice_good_case() throws Exception
    {
        String connectionString = "HostName=aaa.bbb.ccc;SharedAccessKeyName=XXX;SharedAccessKey=YYY";
        String deviceId = "somedevice";

        commonExpectations(connectionString, deviceId);

        RegistryManager registryManager = RegistryManager.createFromConnectionString(connectionString);
        registryManager.removeDevice(deviceId);

        new VerificationsInOrder()
        {
            {
                iotHubConnectionString.getUrlDevice(deviceId);
                times = 1;
                new HttpRequest(mockUrl, HttpMethod.DELETE, new byte[0]);
                times = 1;
                mockHttpRequest.setReadTimeoutMillis(anyInt);
                mockHttpRequest.setHeaderField("authorization", anyString);
                mockHttpRequest.setHeaderField("If-Match", "*");
            }
        };
    }

    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_052: [The function shall throw IllegalArgumentException if the input string is null or empty]
    // Assert
    @Test (expected = IllegalArgumentException.class)
    public void removeDeviceAsync_input_null() throws Exception
    {
        String connectionString = "HostName=aaa.bbb.ccc;SharedAccessKeyName=XXX;SharedAccessKey=YYY";
        RegistryManager registryManager = RegistryManager.createFromConnectionString(connectionString);

        registryManager.removeDeviceAsync(null);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_052: [The function shall throw IllegalArgumentException if the input string is null or empty]
    // Assert
    @Test (expected = IllegalArgumentException.class)
    public void removeDeviceAsync_input_empty() throws Exception
    {
        String connectionString = "HostName=aaa.bbb.ccc;SharedAccessKeyName=XXX;SharedAccessKey=YYY";
        RegistryManager registryManager = RegistryManager.createFromConnectionString(connectionString);

        registryManager.removeDeviceAsync("");
    }

    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_053: [The function shall create an async wrapper around the removeDevice() function call, handle the return value or delegate exception]
    @Test
    public void removeDeviceAsync_future_return_ok() throws Exception
    {
        String connectionString = "HostName=aaa.bbb.ccc;SharedAccessKeyName=XXX;SharedAccessKey=YYY";
        String deviceId = "somedevice";

        commonExpectations(connectionString, deviceId);

        RegistryManager registryManager = RegistryManager.createFromConnectionString(connectionString);
        CompletableFuture completableFuture = registryManager.removeDeviceAsync(deviceId);
        completableFuture.get();

        new VerificationsInOrder()
        {
            {
                iotHubConnectionString.getUrlDevice(deviceId);
                times = 1;
                new HttpRequest(mockUrl, HttpMethod.DELETE, new byte[0]);
                times = 1;
                mockHttpRequest.setReadTimeoutMillis(anyInt);
                mockHttpRequest.setHeaderField("authorization", anyString);
                mockHttpRequest.setHeaderField("If-Match", "*");
            }
        };
    }

    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_053: [The function shall create an async wrapper around the removeDevice() function call, handle the return value or delegate exception]
    // Assert
    @Test (expected = Exception.class)
    public void removeDeviceAsync_future_throw() throws Exception
    {
        String deviceId = "somedevice";
        new MockUp<RegistryManager>()
        {
            @Mock
            public Device removeDevice(String deviceId) throws IOException, IotHubException
            {
                throw new IOException();
            }
        };
        String connectionString = "HostName=aaa.bbb.ccc;SharedAccessKeyName=XXX;SharedAccessKey=YYY";
        RegistryManager registryManager = RegistryManager.createFromConnectionString(connectionString);

        CompletableFuture completableFuture = registryManager.removeDeviceAsync(deviceId);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_054: [The function shall get the URL for the device]
    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_055: [The function shall create a new SAS token for the device]
    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_056: [The function shall create a new HttpRequest for getting statistics a device from IotHub]
    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_057: [The function shall send the created request and get the response]
    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_058: [The function shall verify the response status and throw proper Exception]
    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_059: [The function shall create a new RegistryStatistics object from the response and return with it]
    @Test
    public void getStatistics_good_case() throws Exception
    {
        String connectionString = "HostName=aaa.bbb.ccc;SharedAccessKeyName=XXX;SharedAccessKey=YYY";
        String deviceId = "somedevice";

        commonExpectations(connectionString, deviceId);

        RegistryManager registryManager = RegistryManager.createFromConnectionString(connectionString);

        RegistryStatistics statistics = registryManager.getStatistics();

        new VerificationsInOrder()
        {
            {
                iotHubConnectionString.getUrlDeviceStatistics();
                new HttpRequest(mockUrl, HttpMethod.GET, new byte[0]);
                mockHttpRequest.setReadTimeoutMillis(anyInt);
                mockHttpRequest.setHeaderField("authorization", anyString);
                mockHttpRequest.setHeaderField("Request-Id", "1001");
                mockHttpRequest.setHeaderField("Accept", "application/json");
                mockHttpRequest.setHeaderField("Content-Type", "application/json");
                mockHttpRequest.setHeaderField("charset", "utf-8");
                mockHttpRequest.send();
                mockIotHubExceptionManager.httpResponseVerification((HttpResponse) any);
            }
        };
        assertNotEquals(null, statistics);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_060: [The function shall create an async wrapper around the getStatistics() function call, handle the return value or delegate exception]
    @Test
    public void getStatisticsAsync_future_return_ok() throws Exception
    {
        String connectionString = "HostName=aaa.bbb.ccc;SharedAccessKeyName=XXX;SharedAccessKey=YYY";
        String deviceId = "somedevice";

        commonExpectations(connectionString, deviceId);

        RegistryManager registryManager = RegistryManager.createFromConnectionString(connectionString);

        CompletableFuture<RegistryStatistics> completableFuture = registryManager.getStatisticsAsync();
        RegistryStatistics statistics = completableFuture.get();

        new VerificationsInOrder()
        {
            {
                iotHubConnectionString.getUrlDeviceStatistics();
                new HttpRequest(mockUrl, HttpMethod.GET, new byte[0]);
                mockHttpRequest.setReadTimeoutMillis(anyInt);
                mockHttpRequest.setHeaderField("authorization", anyString);
                mockHttpRequest.setHeaderField("Request-Id", "1001");
                mockHttpRequest.setHeaderField("Accept", "application/json");
                mockHttpRequest.setHeaderField("Content-Type", "application/json");
                mockHttpRequest.setHeaderField("charset", "utf-8");
                mockHttpRequest.send();
                mockIotHubExceptionManager.httpResponseVerification((HttpResponse) any);
            }
        };
        assertNotEquals(null, statistics);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_060: [The function shall create an async wrapper around the getStatistics() function call, handle the return value or delegate exception]
    // Assert
    @Test (expected = Exception.class)
    public void getStatisticsAsync_future_throw() throws Exception
    {
        new MockUp<RegistryManager>()
        {
            @Mock
            public Device getStatistics() throws IOException, IotHubException
            {
                throw new IOException();
            }
        };
        String connectionString = "HostName=aaa.bbb.ccc;SharedAccessKeyName=XXX;SharedAccessKey=YYY";
        RegistryManager registryManager = RegistryManager.createFromConnectionString(connectionString);

        registryManager.getStatisticsAsync();
    }

    // TESTS_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_15_061: [The function shall throw IllegalArgumentException if any of the input parameters is null]
    public void exportDevices_blob_input_null() throws Exception
    {
        String connectionString = "HostName=aaa.bbb.ccc;SharedAccessKeyName=XXX;SharedAccessKey=YYY";
        RegistryManager registryManager = RegistryManager.createFromConnectionString(connectionString);

        registryManager.exportDevices(null, true);
    }

    // TESTS_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_15_061: [The function shall throw IllegalArgumentException if any of the input parameters is null]
    @Test (expected = IllegalArgumentException.class)
    public void exportDevices_exclude_keys_null() throws Exception
    {
        String connectionString = "HostName=aaa.bbb.ccc;SharedAccessKeyName=XXX;SharedAccessKey=YYY";
        RegistryManager registryManager = RegistryManager.createFromConnectionString(connectionString);

        registryManager.exportDevices("www.someurl.com", null);
    }

    // TESTS_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_15_061: [The function shall throw IllegalArgumentException if any of the input parameters is null]
    // TESTS_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_15_062: [The function shall get the URL for the bulk export job creation]
    // TESTS_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_15_063: [The function shall create a new SAS token for the bulk export job]
    // TESTS_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_15_064: [The function shall create a new HttpRequest for the export job creation]
    // TESTS_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_15_065: [The function shall send the created request and get the response]
    // TESTS_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_15_066: [The function shall verify the response status and throw proper Exception]
    // TESTS_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_15_067: [The function shall create a new JobProperties object from the response and return it]
    @Test
    public void exportDevices_good_case() throws Exception
    {
        String connectionString = "HostName=aaa.bbb.ccc;SharedAccessKeyName=XXX;SharedAccessKey=YYY";

        new NonStrictExpectations()
        {
            {
                IotHubConnectionStringBuilder.createConnectionString(connectionString);
                result = iotHubConnectionString;
                iotHubConnectionString.getUrlCreateExportImportJob();
                result = mockUrl;
                mockHttpRequest.send();
                result = mockHttpResponse;
                mockIotHubExceptionManager.httpResponseVerification((HttpResponse) any);
                mockHttpResponse.getBody();
                result = jobPropertiesJson.getBytes();
            }
        };

        RegistryManager registryManager = RegistryManager.createFromConnectionString(connectionString);
        JobProperties jobProperties = registryManager.exportDevices("blob1", true);

        new VerificationsInOrder()
        {
            {
                iotHubConnectionString.getUrlCreateExportImportJob();
                new HttpRequest(mockUrl, HttpMethod.POST, (byte[]) any);
                mockHttpRequest.setReadTimeoutMillis(anyInt);
                mockHttpRequest.setHeaderField("authorization", anyString);
                mockHttpRequest.setHeaderField("Request-Id", "1001");
                mockHttpRequest.setHeaderField("Accept", "application/json");
                mockHttpRequest.setHeaderField("Content-Type", "application/json");
                mockHttpRequest.setHeaderField("charset", "utf-8");
                mockHttpRequest.send();
                mockIotHubExceptionManager.httpResponseVerification((HttpResponse) any);
            }
        };
        assertNotEquals(null, jobProperties);
    }

    // TESTS_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_15_068: [The function shall create an async wrapper around
    // the exportDevices() function call, handle the return value or delegate exception ]
    @Test (expected = Exception.class)
    public void exportDevicesAsync_future_throw() throws Exception
    {
        new MockUp<RegistryManager>()
        {
            @Mock
            public JobProperties exportDevices(String url, Boolean excludeKeys)
                    throws IllegalArgumentException, IOException, IotHubException
            {
                throw new IllegalArgumentException();
            }
        };

        String connectionString = "HostName=aaa.bbb.ccc;SharedAccessKeyName=XXX;SharedAccessKey=YYY";
        RegistryManager registryManager = RegistryManager.createFromConnectionString(connectionString);

        CompletableFuture<JobProperties> completableFuture =  registryManager.exportDevicesAsync("blah", true);
        completableFuture.get();
    }

    // TESTS_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_15_069: [The function shall throw IllegalArgumentException if any of the input parameters is null]
    @Test (expected = IllegalArgumentException.class)
    public void importDevices_blob_import_null() throws Exception
    {
        String connectionString = "HostName=aaa.bbb.ccc;SharedAccessKeyName=XXX;SharedAccessKey=YYY";
        RegistryManager registryManager = RegistryManager.createFromConnectionString(connectionString);

        registryManager.importDevices(null, "outputblob");
    }

    // TESTS_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_15_069: [The function shall throw IllegalArgumentException if any of the input parameters is null]
    @Test (expected = IllegalArgumentException.class)
    public void importDevices_blob_output_null() throws Exception
    {
        String connectionString = "HostName=aaa.bbb.ccc;SharedAccessKeyName=XXX;SharedAccessKey=YYY";
        RegistryManager registryManager = RegistryManager.createFromConnectionString(connectionString);

        registryManager.importDevices("importblob", null);
    }

    // TESTS_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_15_070: [The function shall get the URL for the bulk import job creation]
    // TESTS_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_15_071: [The function shall create a new SAS token for the bulk import job]
    // TESTS_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_15_072: [The function shall create a new HttpRequest for the bulk import job creation]
    // TESTS_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_15_073: [The function shall send the created request and get the response]
    // TESTS_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_15_074: [The function shall verify the response status and throw proper Exception]
    // TESTS_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_15_075: [The function shall create a new JobProperties object from the response and return it]
    @Test
    public void importDevices_good_case() throws Exception
    {
        String connectionString = "HostName=aaa.bbb.ccc;SharedAccessKeyName=XXX;SharedAccessKey=YYY";

        new NonStrictExpectations()
        {
            {
                IotHubConnectionStringBuilder.createConnectionString(connectionString);
                result = iotHubConnectionString;
                iotHubConnectionString.getUrlCreateExportImportJob();
                result = mockUrl;
                mockHttpRequest.send();
                result = mockHttpResponse;
                mockIotHubExceptionManager.httpResponseVerification((HttpResponse) any);
                mockHttpResponse.getBody();
                result = jobPropertiesJson.getBytes();
            }
        };

        RegistryManager registryManager = RegistryManager.createFromConnectionString(connectionString);
        JobProperties jobProperties = registryManager.importDevices("blob1", "blob2");

        new VerificationsInOrder()
        {
            {
                iotHubConnectionString.getUrlCreateExportImportJob();
                new HttpRequest(mockUrl, HttpMethod.POST, (byte[]) any);
                mockHttpRequest.setReadTimeoutMillis(anyInt);
                mockHttpRequest.setHeaderField("authorization", anyString);
                mockHttpRequest.setHeaderField("Request-Id", "1001");
                mockHttpRequest.setHeaderField("Accept", "application/json");
                mockHttpRequest.setHeaderField("Content-Type", "application/json");
                mockHttpRequest.setHeaderField("charset", "utf-8");
                mockHttpRequest.send();
                mockIotHubExceptionManager.httpResponseVerification((HttpResponse) any);
            }
        };
        assertNotEquals(null, jobProperties);
    }

    // TESTS_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_15_076: [The function shall create an async wrapper around
    // the importDevices() function call, handle the return value or delegate exception]
    @Test (expected = Exception.class)
    public void importDevicesAsync_future_throw() throws Exception
    {
        new MockUp<RegistryManager>()
        {
            @Mock
            public JobProperties importDevices(String importBlobContainerUri, String outputBlobContainerUri)
                    throws IllegalArgumentException, IOException, IotHubException
            {
                throw new IllegalArgumentException();
            }
        };

        String connectionString = "HostName=aaa.bbb.ccc;SharedAccessKeyName=XXX;SharedAccessKey=YYY";
        RegistryManager registryManager = RegistryManager.createFromConnectionString(connectionString);

        CompletableFuture<JobProperties> completableFuture =  registryManager.importDevicesAsync("importblob", "outputblob");
        completableFuture.get();
    }

    // TESTS_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_15_077: [The function shall throw IllegalArgumentException if the input parameter is null]
    @Test (expected = IllegalArgumentException.class)
    public void getJob_job_id_null() throws Exception
    {
        String connectionString = "HostName=aaa.bbb.ccc;SharedAccessKeyName=XXX;SharedAccessKey=YYY";
        RegistryManager registryManager = RegistryManager.createFromConnectionString(connectionString);

        registryManager.getJob(null);
    }

    // TESTS_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_15_078: [The function shall get the URL for the get request]
    // TESTS_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_15_079: [The function shall create a new SAS token for the get request]
    // TESTS_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_15_080: [The function shall create a new HttpRequest for getting the properties of a job]
    // TESTS_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_15_081: [The function shall send the created request and get the response]
    // TESTS_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_15_082: [The function shall verify the response status and throw proper Exception]
    // TESTS_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_15_083: [The function shall create a new JobProperties object from the response and return it]
    @Test
    public void getJob_good_case() throws Exception
    {
        String connectionString = "HostName=aaa.bbb.ccc;SharedAccessKeyName=XXX;SharedAccessKey=YYY";
        String jobId = "somejobid";

        new NonStrictExpectations()
        {
            {
                IotHubConnectionStringBuilder.createConnectionString(connectionString);
                result = iotHubConnectionString;
                iotHubConnectionString.getUrlImportExportJob(jobId);
                result = mockUrl;
                mockHttpRequest.send();
                result = mockHttpResponse;
                mockIotHubExceptionManager.httpResponseVerification((HttpResponse) any);
                mockHttpResponse.getBody();
                result = jobPropertiesJson.getBytes();
            }
        };

        RegistryManager registryManager = RegistryManager.createFromConnectionString(connectionString);
        JobProperties jobProperties = registryManager.getJob(jobId);

        new VerificationsInOrder()
        {
            {
                iotHubConnectionString.getUrlImportExportJob(jobId);
                new HttpRequest(mockUrl, HttpMethod.GET, (byte[]) any);
                mockHttpRequest.setReadTimeoutMillis(anyInt);
                mockHttpRequest.setHeaderField("authorization", anyString);
                mockHttpRequest.setHeaderField("Request-Id", "1001");
                mockHttpRequest.setHeaderField("Accept", "application/json");
                mockHttpRequest.setHeaderField("Content-Type", "application/json");
                mockHttpRequest.setHeaderField("charset", "utf-8");
                mockHttpRequest.send();
                mockIotHubExceptionManager.httpResponseVerification((HttpResponse) any);
            }
        };
        assertNotEquals(null, jobProperties);
    }

    // TESTS_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_15_084: [The function shall create an async wrapper
    // around the getJob() function call, handle the return value or delegate exception]
    @Test (expected = Exception.class)
    public void getJobAsync_future_throw() throws Exception
    {
        new MockUp<RegistryManager>()
        {
            @Mock
            public JobProperties getJob(String jobId)
                    throws IllegalArgumentException, IOException, IotHubException
            {
                throw new IllegalArgumentException();
            }
        };

        String connectionString = "HostName=aaa.bbb.ccc;SharedAccessKeyName=XXX;SharedAccessKey=YYY";
        RegistryManager registryManager = RegistryManager.createFromConnectionString(connectionString);

        CompletableFuture<JobProperties> completableFuture =  registryManager.getJobAsync("someJobId");
        completableFuture.get();
    }

    private void commonExpectations(String connectionString, String deviceId) throws Exception
    {
        new NonStrictExpectations()
        {
            {
                IotHubConnectionStringBuilder.createConnectionString(connectionString);
                result = iotHubConnectionString;
                iotHubConnectionString.getUrlDevice(deviceId);
                result = mockUrl;
                mockHttpRequest.send();
                result = mockHttpResponse;
                mockIotHubExceptionManager.httpResponseVerification((HttpResponse) any);
                mockHttpResponse.getBody();
                result = deviceJson.getBytes();
            }
        };
    }

    private void commonVerifications(HttpMethod httpMethod, String requestDeviceId, Device responseDevice) throws Exception
    {
        new VerificationsInOrder()
        {
            {
                iotHubConnectionString.getUrlDevice(requestDeviceId);
                new HttpRequest(mockUrl, httpMethod, (byte[]) any);
                mockHttpRequest.setReadTimeoutMillis(anyInt);
                mockHttpRequest.setHeaderField("authorization", anyString);
                mockHttpRequest.setHeaderField("Request-Id", "1001");
                mockHttpRequest.setHeaderField("Accept", "application/json");
                mockHttpRequest.setHeaderField("Content-Type", "application/json");
                mockHttpRequest.setHeaderField("charset", "utf-8");
                mockHttpRequest.send();
                mockIotHubExceptionManager.httpResponseVerification((HttpResponse) any);
            }
        };
        assertNotEquals(null, responseDevice);
    }

    private void getDevicesExpectations(String connectionString, int numberOfDevices) throws Exception
    {
        new NonStrictExpectations()
        {
            {
                IotHubConnectionStringBuilder.createConnectionString(connectionString);
                result = iotHubConnectionString;
                iotHubConnectionString.getUrlDeviceList(numberOfDevices);
                result = mockUrl;
                mockHttpRequest.send();
                result = mockHttpResponse;
                mockIotHubExceptionManager.httpResponseVerification((HttpResponse) any);
                mockHttpResponse.getBody();
                result = devicesJson.getBytes();
            }
        };
    }

    private void getDevicesVerifications(int numberOfDevices, ArrayList<Device> devices) throws Exception
    {
        new VerificationsInOrder()
        {
            {
                iotHubConnectionString.getUrlDeviceList(numberOfDevices);
                times = 1;
                new HttpRequest(mockUrl, HttpMethod.GET, (byte[]) any);
                times = 1;
                mockHttpRequest.setReadTimeoutMillis(anyInt);
                mockHttpRequest.setHeaderField("authorization", anyString);
                mockHttpRequest.setHeaderField("Request-Id", "1001");
                mockHttpRequest.setHeaderField("Accept", "application/json");
                mockHttpRequest.setHeaderField("Content-Type", "application/json");
                mockHttpRequest.setHeaderField("charset", "utf-8");
                mockHttpRequest.send();
            }
        };
        assertNotEquals(null, devices);
    }
}