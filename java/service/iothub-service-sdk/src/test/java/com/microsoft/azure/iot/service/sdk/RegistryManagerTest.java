/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.sdk;

import com.microsoft.azure.iot.service.auth.IotHubServiceSasToken;
import com.microsoft.azure.iot.service.exceptions.IotHubException;
import com.microsoft.azure.iot.service.exceptions.IotHubExceptionManager;
import com.microsoft.azure.iot.service.transport.http.HttpConnection;
import com.microsoft.azure.iot.service.transport.http.HttpMethod;
import com.microsoft.azure.iot.service.transport.http.HttpRequest;
import com.microsoft.azure.iot.service.transport.http.HttpResponse;
import mockit.*;
import mockit.integration.junit4.JMockit;
import org.junit.Test;
import org.junit.runner.RunWith;

import java.io.IOException;
import java.net.MalformedURLException;
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
    IotHubExceptionManager iotHubExceptionManager;
    @Mocked
    RegistryStatistics registryStatistics;

    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_001: [The constructor shall throw IllegalArgumentException if the input string is null or empty]
    // Assert
    @Test (expected = IllegalArgumentException.class)
    public void constructor_input_null() throws Exception
    {
        // Arrange
        String connectionString = null;
        // Act
        RegistryManager.createFromConnectionString(connectionString);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_001: [The constructor shall throw IllegalArgumentException if the input string is null or empty]
    // Assert
    @Test (expected = IllegalArgumentException.class)
    public void constructor_input_empty() throws Exception
    {
        // Arrange
        String connectionString = null;
        // Act
        RegistryManager.createFromConnectionString(connectionString);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_002: [The constructor shall create an IotHubConnectionString object from the given connection string]
    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_003: [The constructor shall create a new RegistryManager, stores the created IotHubConnectionString object and return with it]
    @Test
    public void constructor_good_case() throws Exception
    {
        // Arrange
        String connectionString = "HostName=aaa.bbb.ccc;SharedAccessKeyName=XXX;SharedAccessKey=YYY";
        // Act
        RegistryManager registryManager = RegistryManager.createFromConnectionString(connectionString);
        // Assert
        assertNotEquals(null, registryManager);
        assertNotEquals(null, Deencapsulation.getField(registryManager, "iotHubConnectionString"));
    }

    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_004: [The constructor shall throw IllegalArgumentException if the input device is null]
    // Assert
    @Test (expected = IllegalArgumentException.class)
    public void addDevice_input_null() throws Exception
    {
        // Arrange
        String connectionString = "HostName=aaa.bbb.ccc;SharedAccessKeyName=XXX;SharedAccessKey=YYY";
        RegistryManager registryManager = RegistryManager.createFromConnectionString(connectionString);
        // Act
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
        // Arrange
        final HttpMethod expectedMethod = HttpMethod.PUT;
        final byte[] body = { 1 };
        String jsonString = "{\"deviceId\":\"xxx-device\",\"generationId\":\"111111111111111111\",\"etag\":\"MA==\",\"connectionState\":\"Connected\",\"connectionStateUpdatedTime\":\"1111-01-01T00:00:00\",\"status\":\"enabled\",\"statusReason\":\"justbecause\",\"statusUpdatedTime\":\"2222-01-01T00:00:00\",\"lastActivityTime\":\"3333-01-01T00:00:00\",\"cloudToDeviceMessageCount\":10,\"authentication\":{\"symmetricKey\":{\"primaryKey\":\"AAABBBCCC111222333444000\",\"secondaryKey\":\"111222333444555AAABBBCCC\"}}}";

        new MockUp<IotHubConnectionString>()
        {
            @Mock
            public URL getUrlDevice(String deviceId) throws MalformedURLException
            {
                return new URL("http://aaa.com");
            }
        };
        new MockUp<HttpRequest>()
        {
            HttpConnection mockUpHttpConnection;

            @Mock
            public void $init(URL url, HttpMethod method, byte[] body) throws IOException
            {
            }

            @Mock
            public HttpResponse send() throws IOException
            {
                return mockHttpResponse;
            }

            @Mock
            public HttpRequest setHeaderField(String field, String value)
            {
                return null;
            }

            @Mock
            public HttpRequest setReadTimeoutMillis(int timeout)
            {
                return null;
            }
        };
        String connectionString = "HostName=aaa.bbb.ccc;SharedAccessKeyName=XXX;SharedAccessKey=YYY";
        RegistryManager registryManager = RegistryManager.createFromConnectionString(connectionString);
        registryManager.open();
        // Assert
        new NonStrictExpectations()
        {
            IotHubConnectionString iotHubConnectionString = new IotHubConnectionString();
            HttpRequest mockUpHttpRequest = new HttpRequest(null, null, null);
            IotHubExceptionManager iotHubExceptionManager;
            {
                device.serializeToJson();
                result = jsonString;
                iotHubConnectionString.getUrlDevice(anyString);

                new HttpRequest(mockUrl, expectedMethod, body);
                returns(mockUpHttpRequest);
                mockUpHttpRequest.setReadTimeoutMillis(anyInt);
                mockUpHttpRequest.setHeaderField("authorization", anyString);
                mockUpHttpRequest.setHeaderField("Request-Id", "1001");
                mockUpHttpRequest.setHeaderField("Accept", "application/json");
                mockUpHttpRequest.setHeaderField("Content-Type", "application/json");
                mockUpHttpRequest.setHeaderField("charset", "utf-8");

                mockUpHttpRequest.send();

                iotHubExceptionManager.httpResponseVerification(null);

                mockHttpResponse.getBody();
                result = body;
                device.createFromJson(jsonString);
                result = device;
            }
        };
        // Act
        Device returnDevice = registryManager.addDevice(device);
        registryManager.close();
        // Assert
        assertNotEquals(null, returnDevice);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_012: [The function shall throw IllegalArgumentException if the input device is null]
    // Assert
    @Test (expected = IllegalArgumentException.class)
    public void addDeviceAsync_input_null() throws Exception
    {
        // Arrange
        String connectionString = "HostName=aaa.bbb.ccc;SharedAccessKeyName=XXX;SharedAccessKey=YYY";
        RegistryManager registryManager = RegistryManager.createFromConnectionString(connectionString);
        // Act
        registryManager.addDeviceAsync(null);
    }
    
    // Tests_SRS_SERVICE_SDK_JAVA_ REGISTRYMANAGER_12_013: [The function shall create an async wrapper around the addDevice() function call, handle the return value or delegate exception]
    @Test
    public void addDeviceAsync_future_return_ok() throws Exception
    {
        // Arrange
        final HttpMethod expectedMethod = HttpMethod.PUT;
        final byte[] body = { 1 };
        String jsonString = "{\"deviceId\":\"xxx-device\",\"generationId\":\"111111111111111111\",\"etag\":\"MA==\",\"connectionState\":\"Connected\",\"connectionStateUpdatedTime\":\"1111-01-01T00:00:00\",\"status\":\"enabled\",\"statusReason\":\"justbecause\",\"statusUpdatedTime\":\"2222-01-01T00:00:00\",\"lastActivityTime\":\"3333-01-01T00:00:00\",\"cloudToDeviceMessageCount\":10,\"authentication\":{\"symmetricKey\":{\"primaryKey\":\"AAABBBCCC111222333444000\",\"secondaryKey\":\"111222333444555AAABBBCCC\"}}}";

        new MockUp<IotHubConnectionString>()
        {
            @Mock
            public URL getUrlDevice(String deviceId) throws MalformedURLException
            {
                return new URL("http://aaa.com");
            }
        };
        new MockUp<HttpRequest>()
        {
            HttpConnection mockUpHttpConnection;

            @Mock
            public void $init(URL url, HttpMethod method, byte[] body) throws IOException
            {
            }

            @Mock
            public HttpResponse send() throws IOException
            {
                return mockHttpResponse;
            }

            @Mock
            public HttpRequest setHeaderField(String field, String value)
            {
                return null;
            }

            @Mock
            public HttpRequest setReadTimeoutMillis(int timeout)
            {
                return null;
            }
        };
        String connectionString = "HostName=aaa.bbb.ccc;SharedAccessKeyName=XXX;SharedAccessKey=YYY";
        RegistryManager registryManager = RegistryManager.createFromConnectionString(connectionString);
        // Assert
        new NonStrictExpectations()
        {
            IotHubConnectionString iotHubConnectionString = new IotHubConnectionString();
            HttpRequest mockUpHttpRequest = new HttpRequest(null, null, null);
            IotHubExceptionManager iotHubExceptionManager;
            {
                device.serializeToJson();
                result = jsonString;
                iotHubConnectionString.getUrlDevice(anyString);

                new HttpRequest(mockUrl, expectedMethod, body);
                returns(mockUpHttpRequest);
                mockUpHttpRequest.setReadTimeoutMillis(anyInt);
                mockUpHttpRequest.setHeaderField("authorization", anyString);
                mockUpHttpRequest.setHeaderField("Request-Id", "1001");
                mockUpHttpRequest.setHeaderField("Accept", "application/json");
                mockUpHttpRequest.setHeaderField("Content-Type", "application/json");
                mockUpHttpRequest.setHeaderField("charset", "utf-8");

                mockUpHttpRequest.send();

                iotHubExceptionManager.httpResponseVerification(null);

                mockHttpResponse.getBody();
                result = body;
                device.createFromJson(jsonString);
                result = device;
            }
        };
        // Act
        CompletableFuture<Device> completableFuture =  registryManager.addDeviceAsync(device);
        completableFuture.get();
    }

    // Tests_SRS_SERVICE_SDK_JAVA_ REGISTRYMANAGER_12_013: [The function shall create an async wrapper around the addDevice() function call, handle the return value or delegate exception]
    // Assert
    @Test (expected = Exception.class)
    public void addDeviceAsync_future_throw() throws Exception
    {
        // Arrange
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
        // Act
        CompletableFuture<Device> completableFuture =  registryManager.addDeviceAsync(device);
        completableFuture.get();
    }
    
    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_014: [The constructor shall throw IllegalArgumentException if the input string is null or empty]
    // Assert
    @Test (expected = IllegalArgumentException.class)
    public void getDevice_input_null() throws Exception
    {
        // Arrange
        String connectionString = "HostName=aaa.bbb.ccc;SharedAccessKeyName=XXX;SharedAccessKey=YYY";
        RegistryManager registryManager = RegistryManager.createFromConnectionString(connectionString);
        // Act
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
        // Arrange
        final HttpMethod expectedMethod = HttpMethod.PUT;
        final byte[] body = { 1 };
        String jsonString = "{\"deviceId\":\"xxx-device\",\"generationId\":\"111111111111111111\",\"etag\":\"MA==\",\"connectionState\":\"Connected\",\"connectionStateUpdatedTime\":\"1111-01-01T00:00:00\",\"status\":\"enabled\",\"statusReason\":\"justbecause\",\"statusUpdatedTime\":\"2222-01-01T00:00:00\",\"lastActivityTime\":\"3333-01-01T00:00:00\",\"cloudToDeviceMessageCount\":10,\"authentication\":{\"symmetricKey\":{\"primaryKey\":\"AAABBBCCC111222333444000\",\"secondaryKey\":\"111222333444555AAABBBCCC\"}}}";

        new MockUp<IotHubConnectionString>()
        {
            @Mock
            public URL getUrlDevice(String deviceId) throws MalformedURLException
            {
                return new URL("http://aaa.com");
            }
        };
        new MockUp<HttpRequest>()
        {
            HttpConnection mockUpHttpConnection;

            @Mock
            public void $init(URL url, HttpMethod method, byte[] body) throws IOException
            {
            }

            @Mock
            public HttpResponse send() throws IOException
            {
                return mockHttpResponse;
            }

            @Mock
            public HttpRequest setHeaderField(String field, String value)
            {
                return null;
            }

            @Mock
            public HttpRequest setReadTimeoutMillis(int timeout)
            {
                return null;
            }
        };
        String connectionString = "HostName=aaa.bbb.ccc;SharedAccessKeyName=XXX;SharedAccessKey=YYY";
        RegistryManager registryManager = RegistryManager.createFromConnectionString(connectionString);
        // Assert
        new NonStrictExpectations()
        {
            IotHubConnectionString iotHubConnectionString = new IotHubConnectionString();
            HttpRequest mockUpHttpRequest = new HttpRequest(null, null, null);
            IotHubExceptionManager iotHubExceptionManager;
            {
                mockUrl = iotHubConnectionString.getUrlDevice(anyString);

                new HttpRequest(mockUrl, expectedMethod, body);
                mockUpHttpRequest.setReadTimeoutMillis(anyInt);
                mockUpHttpRequest.setHeaderField("authorization", anyString);
                mockUpHttpRequest.setHeaderField("Request-Id", "1001");
                mockUpHttpRequest.setHeaderField("Accept", "application/json");
                mockUpHttpRequest.setHeaderField("Content-Type", "application/json");
                mockUpHttpRequest.setHeaderField("charset", "utf-8");

                mockUpHttpRequest.send();

                iotHubExceptionManager.httpResponseVerification(null);

                mockHttpResponse.getBody();
                result = body;
                device.createFromJson(jsonString);
                result = device;
            }
        };
        // Act
        Device returnDevice = registryManager.getDevice("xxx-device");
        // Assert
        assertNotEquals(null, returnDevice);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_021: [The constructor shall throw IllegalArgumentException if the input device is null]
    // Assert
    @Test (expected = IllegalArgumentException.class)
    public void getDeviceAsync_input_null() throws Exception
    {
        // Arrange
        String connectionString = "HostName=aaa.bbb.ccc;SharedAccessKeyName=XXX;SharedAccessKey=YYY";
        RegistryManager registryManager = RegistryManager.createFromConnectionString(connectionString);
        // Act
        registryManager.getDeviceAsync(null);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_022: [The function shall create an async wrapper around the addDevice() function call, handle the return value or delegate exception]
    @Test
    public void getDeviceAsync_future_return_ok() throws Exception
    {
        // Arrange
        final HttpMethod expectedMethod = HttpMethod.PUT;
        final byte[] body = { 1 };
        String jsonString = "{\"deviceId\":\"xxx-device\",\"generationId\":\"111111111111111111\",\"etag\":\"MA==\",\"connectionState\":\"Connected\",\"connectionStateUpdatedTime\":\"1111-01-01T00:00:00\",\"status\":\"enabled\",\"statusReason\":\"justbecause\",\"statusUpdatedTime\":\"2222-01-01T00:00:00\",\"lastActivityTime\":\"3333-01-01T00:00:00\",\"cloudToDeviceMessageCount\":10,\"authentication\":{\"symmetricKey\":{\"primaryKey\":\"AAABBBCCC111222333444000\",\"secondaryKey\":\"111222333444555AAABBBCCC\"}}}";

        new MockUp<IotHubConnectionString>()
        {
            @Mock
            public URL getUrlDevice(String deviceId) throws MalformedURLException
            {
                return new URL("http://aaa.com");
            }
        };
        new MockUp<HttpRequest>()
        {
            HttpConnection mockUpHttpConnection;

            @Mock
            public void $init(URL url, HttpMethod method, byte[] body) throws IOException
            {
            }

            @Mock
            public HttpResponse send() throws IOException
            {
                return mockHttpResponse;
            }

            @Mock
            public HttpRequest setHeaderField(String field, String value)
            {
                return null;
            }

            @Mock
            public HttpRequest setReadTimeoutMillis(int timeout)
            {
                return null;
            }
        };
        String connectionString = "HostName=aaa.bbb.ccc;SharedAccessKeyName=XXX;SharedAccessKey=YYY";
        RegistryManager registryManager = RegistryManager.createFromConnectionString(connectionString);
        // Assert
        new NonStrictExpectations()
        {
            IotHubConnectionString iotHubConnectionString = new IotHubConnectionString();
            HttpRequest mockUpHttpRequest = new HttpRequest(null, null, null);
            IotHubExceptionManager iotHubExceptionManager;
            {
                mockUrl = iotHubConnectionString.getUrlDevice(anyString);

                new HttpRequest(mockUrl, expectedMethod, body);
                mockUpHttpRequest.setReadTimeoutMillis(anyInt);
                mockUpHttpRequest.setHeaderField("authorization", anyString);
                mockUpHttpRequest.setHeaderField("Request-Id", "1001");
                mockUpHttpRequest.setHeaderField("Accept", "application/json");
                mockUpHttpRequest.setHeaderField("Content-Type", "application/json");
                mockUpHttpRequest.setHeaderField("charset", "utf-8");

                mockUpHttpRequest.send();

                iotHubExceptionManager.httpResponseVerification(null);

                mockHttpResponse.getBody();
                result = body;
                device.createFromJson(jsonString);
                result = device;
            }
        };
        // Act
        CompletableFuture<Device> completableFuture =  registryManager.getDeviceAsync("xxx-device");
        completableFuture.get();
    }

    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_022: [The function shall create an async wrapper around the addDevice() function call, handle the return value or delegate exception]
    // Assert
    @Test (expected = Exception.class)
    public void getDeviceAsync_future_throw() throws Exception
    {
        // Arrange
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
        // Act
        CompletableFuture<Device> completableFuture =  registryManager.getDeviceAsync("xxx-device");
        completableFuture.get();
    }


    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_023: [The constructor shall throw IllegalArgumentException if the input count number is less than 1]
    // Assert
    @Test (expected = IllegalArgumentException.class)
    public void getDevices_input_zero() throws Exception
    {
        // Arrange
        String connectionString = "HostName=aaa.bbb.ccc;SharedAccessKeyName=XXX;SharedAccessKey=YYY";
        RegistryManager registryManager = RegistryManager.createFromConnectionString(connectionString);
        // Act
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
        // Arrange
        final HttpMethod expectedMethod = HttpMethod.PUT;
        final byte[] body = { 1 };
        String jsonString = "[{\"deviceId\":\"xxx-device\",\"generationId\":\"111111111111111111\",\"etag\":\"MA==\",\"connectionState\":\"Connected\",\"connectionStateUpdatedTime\":\"1111-01-01T00:00:00\",\"status\":\"enabled\",\"statusReason\":\"justbecause\",\"statusUpdatedTime\":\"2222-01-01T00:00:00\",\"lastActivityTime\":\"3333-01-01T00:00:00\",\"cloudToDeviceMessageCount\":10,\"authentication\":{\"symmetricKey\":{\"primaryKey\":\"AAABBBCCC111222333444000\",\"secondaryKey\":\"111222333444555AAABBBCCC\"}}}]";
        byte[] jsonBytes = jsonString.getBytes();

        new MockUp<IotHubConnectionString>()
        {
            @Mock
            public URL getUrlDevice(String deviceId) throws MalformedURLException
            {
                return new URL("http://aaa.com");
            }
        };

        new MockUp<HttpRequest>()
        {
            HttpConnection mockUpHttpConnection;

            @Mock
            public void $init(URL url, HttpMethod method, byte[] body) throws IOException
            {
            }

            @Mock
            public HttpResponse send() throws IOException
            {
                return mockHttpResponse;
            }

            @Mock
            public HttpRequest setHeaderField(String field, String value)
            {
                return null;
            }

            @Mock
            public HttpRequest setReadTimeoutMillis(int timeout)
            {
                return null;
            }
        };
        String connectionString = "HostName=aaa.bbb.ccc;SharedAccessKeyName=XXX;SharedAccessKey=YYY";
        RegistryManager registryManager = RegistryManager.createFromConnectionString(connectionString);
        // Assert
        new NonStrictExpectations()
        {
            IotHubConnectionString iotHubConnectionString = new IotHubConnectionString();
            HttpRequest mockUpHttpRequest = new HttpRequest(null, null, null);
            IotHubExceptionManager iotHubExceptionManager;
            {
                mockUrl = iotHubConnectionString.getUrlDevice(anyString);
                new HttpRequest(mockUrl, HttpMethod.GET, body);
                mockUpHttpRequest.setReadTimeoutMillis(anyInt);
                mockUpHttpRequest.setHeaderField("authorization", anyString);
                mockUpHttpRequest.setHeaderField("Request-Id", "1001");
                mockUpHttpRequest.setHeaderField("Accept", "application/json");
                mockUpHttpRequest.setHeaderField("Content-Type", "application/json");
                mockUpHttpRequest.setHeaderField("charset", "utf-8");

                mockUpHttpRequest.send();

                iotHubExceptionManager.httpResponseVerification(null);

                mockHttpResponse.getBody();
                result = jsonBytes;
            }
        };
        // Act
        ArrayList<Device> devices =  registryManager.getDevices(10);
        // Assert
        assertNotEquals(null, devices);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_030: [The function shall throw IllegalArgumentException if the input count number is less than 1]
    // Assert
    @Test (expected = IllegalArgumentException.class)
    public void getDevicesAsync_input_zero() throws Exception
    {
        // Arrange
        String connectionString = "HostName=aaa.bbb.ccc;SharedAccessKeyName=XXX;SharedAccessKey=YYY";
        RegistryManager registryManager = RegistryManager.createFromConnectionString(connectionString);
        // Act
        registryManager.getDevicesAsync(0);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_031: [The function shall create an async wrapper around the getDevices() function call, handle the return value or delegate exception]
    @Test
    public void getDevicesAsync_future_return_ok() throws Exception
    {
        // Arrange
        final HttpMethod expectedMethod = HttpMethod.PUT;
        final byte[] body = { 1 };
        String jsonString = "[{\"deviceId\":\"xxx-device\",\"generationId\":\"111111111111111111\",\"etag\":\"MA==\",\"connectionState\":\"Connected\",\"connectionStateUpdatedTime\":\"1111-01-01T00:00:00\",\"status\":\"enabled\",\"statusReason\":\"justbecause\",\"statusUpdatedTime\":\"2222-01-01T00:00:00\",\"lastActivityTime\":\"3333-01-01T00:00:00\",\"cloudToDeviceMessageCount\":10,\"authentication\":{\"symmetricKey\":{\"primaryKey\":\"AAABBBCCC111222333444000\",\"secondaryKey\":\"111222333444555AAABBBCCC\"}}}]";
        byte[] jsonBytes = jsonString.getBytes();

        new MockUp<IotHubConnectionString>()
        {
            @Mock
            public URL getUrlDevice(String deviceId) throws MalformedURLException
            {
                return new URL("http://aaa.com");
            }
        };

        new MockUp<HttpRequest>()
        {
            HttpConnection mockUpHttpConnection;

            @Mock
            public void $init(URL url, HttpMethod method, byte[] body) throws IOException
            {
            }

            @Mock
            public HttpResponse send() throws IOException
            {
                return mockHttpResponse;
            }

            @Mock
            public HttpRequest setHeaderField(String field, String value)
            {
                return null;
            }

            @Mock
            public HttpRequest setReadTimeoutMillis(int timeout)
            {
                return null;
            }
        };
        String connectionString = "HostName=aaa.bbb.ccc;SharedAccessKeyName=XXX;SharedAccessKey=YYY";
        RegistryManager registryManager = RegistryManager.createFromConnectionString(connectionString);
        // Assert
        new NonStrictExpectations()
        {
            IotHubConnectionString iotHubConnectionString = new IotHubConnectionString();
            HttpRequest mockUpHttpRequest = new HttpRequest(null, null, null);
            IotHubExceptionManager iotHubExceptionManager;
            {
                mockUrl = iotHubConnectionString.getUrlDevice(anyString);
                new HttpRequest(mockUrl, HttpMethod.GET, body);
                mockUpHttpRequest.setReadTimeoutMillis(anyInt);
                mockUpHttpRequest.setHeaderField("authorization", anyString);
                mockUpHttpRequest.setHeaderField("Request-Id", "1001");
                mockUpHttpRequest.setHeaderField("Accept", "application/json");
                mockUpHttpRequest.setHeaderField("Content-Type", "application/json");
                mockUpHttpRequest.setHeaderField("charset", "utf-8");

                mockUpHttpRequest.send();

                iotHubExceptionManager.httpResponseVerification(null);

                mockHttpResponse.getBody();
                result = jsonBytes;
            }
        };
        // Act
        CompletableFuture<ArrayList<Device>> completableFuture =  registryManager.getDevicesAsync(10);
        completableFuture.get();
    }

    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_031: [The function shall create an async wrapper around the getDevices() function call, handle the return value or delegate exception]
    // Assert
    @Test (expected = Exception.class)
    public void getDevicesAsync_future_throw() throws Exception
    {
        // Arrange
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
        // Act
        CompletableFuture<ArrayList<Device>> completableFuture = registryManager.getDevicesAsync(10);
        completableFuture.get();
    }

    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_032: [The function shall throw IllegalArgumentException if the input device is null]
    // Assert
    @Test (expected = IllegalArgumentException.class)
    public void updateDevice_input_null() throws Exception
    {
        // Arrange
        String connectionString = "HostName=aaa.bbb.ccc;SharedAccessKeyName=XXX;SharedAccessKey=YYY";
        RegistryManager registryManager = RegistryManager.createFromConnectionString(connectionString);
        // Act
        registryManager.updateDevice(null);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_033: [The function shall call updateDevice with forceUpdate = false]
    @Test
    public void updateDevice_good_case() throws Exception
    {
        // Arrange
        final HttpMethod expectedMethod = HttpMethod.PUT;
        final byte[] body = { 1 };
        String jsonString = "{\"deviceId\":\"xxx-device\",\"generationId\":\"111111111111111111\",\"etag\":\"MA==\",\"connectionState\":\"Connected\",\"connectionStateUpdatedTime\":\"1111-01-01T00:00:00\",\"status\":\"enabled\",\"statusReason\":\"justbecause\",\"statusUpdatedTime\":\"2222-01-01T00:00:00\",\"lastActivityTime\":\"3333-01-01T00:00:00\",\"cloudToDeviceMessageCount\":10,\"authentication\":{\"symmetricKey\":{\"primaryKey\":\"AAABBBCCC111222333444000\",\"secondaryKey\":\"111222333444555AAABBBCCC\"}}}";

        new MockUp<IotHubConnectionString>()
        {
            @Mock
            public URL getUrlDevice(String deviceId) throws MalformedURLException
            {
                return new URL("http://aaa.com");
            }
        };
        new MockUp<HttpRequest>()
        {
            HttpConnection mockUpHttpConnection;

            @Mock
            public void $init(URL url, HttpMethod method, byte[] body) throws IOException
            {
            }

            @Mock
            public HttpResponse send() throws IOException
            {
                return mockHttpResponse;
            }

            @Mock
            public HttpRequest setHeaderField(String field, String value)
            {
                return null;
            }

            @Mock
            public HttpRequest setReadTimeoutMillis(int timeout)
            {
                return null;
            }
        };
        String connectionString = "HostName=aaa.bbb.ccc;SharedAccessKeyName=XXX;SharedAccessKey=YYY";
        RegistryManager registryManager = RegistryManager.createFromConnectionString(connectionString);
        // Assert
        new NonStrictExpectations()
        {
            IotHubConnectionString iotHubConnectionString = new IotHubConnectionString();
            HttpRequest mockUpHttpRequest = new HttpRequest(null, null, null);
            IotHubExceptionManager iotHubExceptionManager;
            {
                device.serializeToJson();
                result = jsonString;
                iotHubConnectionString.getUrlDevice(anyString);

                new HttpRequest(mockUrl, expectedMethod, body);
                returns(mockUpHttpRequest);
                mockUpHttpRequest.setReadTimeoutMillis(anyInt);
                mockUpHttpRequest.setHeaderField("authorization", anyString);
                mockUpHttpRequest.setHeaderField("Request-Id", "1001");
                mockUpHttpRequest.setHeaderField("Accept", "application/json");
                mockUpHttpRequest.setHeaderField("Content-Type", "application/json");
                mockUpHttpRequest.setHeaderField("charset", "utf-8");

                mockUpHttpRequest.send();

                iotHubExceptionManager.httpResponseVerification(null);

                mockHttpResponse.getBody();
                result = body;
                device.createFromJson(jsonString);
                result = device;
            }
        };
        // Act
        Device returnDevice = registryManager.updateDevice(device);
        // Assert
        assertNotEquals(null, returnDevice);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_034: [The function shall throw IllegalArgumentException if the input device is null]
    // Assert
    @Test (expected = IllegalArgumentException.class)
    public void updateDeviceForce_input_null() throws Exception
    {
        // Arrange
        String connectionString = "HostName=aaa.bbb.ccc;SharedAccessKeyName=XXX;SharedAccessKey=YYY";
        RegistryManager registryManager = RegistryManager.createFromConnectionString(connectionString);
        // Act
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
        // Arrange
        final HttpMethod expectedMethod = HttpMethod.PUT;
        final byte[] body = { 1 };
        String jsonString = "{\"deviceId\":\"xxx-device\",\"generationId\":\"111111111111111111\",\"etag\":\"MA==\",\"connectionState\":\"Connected\",\"connectionStateUpdatedTime\":\"1111-01-01T00:00:00\",\"status\":\"enabled\",\"statusReason\":\"justbecause\",\"statusUpdatedTime\":\"2222-01-01T00:00:00\",\"lastActivityTime\":\"3333-01-01T00:00:00\",\"cloudToDeviceMessageCount\":10,\"authentication\":{\"symmetricKey\":{\"primaryKey\":\"AAABBBCCC111222333444000\",\"secondaryKey\":\"111222333444555AAABBBCCC\"}}}";

        new MockUp<IotHubConnectionString>()
        {
            @Mock
            public URL getUrlDevice(String deviceId) throws MalformedURLException
            {
                return new URL("http://aaa.com");
            }
        };
        new MockUp<HttpRequest>()
        {
            HttpConnection mockUpHttpConnection;

            @Mock
            public void $init(URL url, HttpMethod method, byte[] body) throws IOException
            {
            }

            @Mock
            public HttpResponse send() throws IOException
            {
                return mockHttpResponse;
            }

            @Mock
            public HttpRequest setHeaderField(String field, String value)
            {
                return null;
            }

            @Mock
            public HttpRequest setReadTimeoutMillis(int timeout)
            {
                return null;
            }
        };
        String connectionString = "HostName=aaa.bbb.ccc;SharedAccessKeyName=XXX;SharedAccessKey=YYY";
        RegistryManager registryManager = RegistryManager.createFromConnectionString(connectionString);
        // Assert
        new NonStrictExpectations()
        {
            IotHubConnectionString iotHubConnectionString = new IotHubConnectionString();
            HttpRequest mockUpHttpRequest = new HttpRequest(null, null, null);
            IotHubExceptionManager iotHubExceptionManager;
            {
                device.serializeToJson();
                result = jsonString;
                iotHubConnectionString.getUrlDevice(anyString);

                new HttpRequest(mockUrl, expectedMethod, body);
                returns(mockUpHttpRequest);
                mockUpHttpRequest.setReadTimeoutMillis(anyInt);
                mockUpHttpRequest.setHeaderField("authorization", anyString);
                mockUpHttpRequest.setHeaderField("Request-Id", "1001");
                mockUpHttpRequest.setHeaderField("Accept", "application/json");
                mockUpHttpRequest.setHeaderField("Content-Type", "application/json");
                mockUpHttpRequest.setHeaderField("charset", "utf-8");

                mockUpHttpRequest.send();

                iotHubExceptionManager.httpResponseVerification(null);

                mockHttpResponse.getBody();
                result = body;
                device.createFromJson(jsonString);
                result = device;
            }
        };
        // Act
        Device returnDevice = registryManager.updateDevice(device, true);
        // Assert
        assertNotEquals(null, returnDevice);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_042: [The function shall throw IllegalArgumentException if the input device is null]
    // Assert
    @Test (expected = IllegalArgumentException.class)
    public void updateDeviceAsync_input_null() throws Exception
    {
        // Arrange
        String connectionString = "HostName=aaa.bbb.ccc;SharedAccessKeyName=XXX;SharedAccessKey=YYY";
        RegistryManager registryManager = RegistryManager.createFromConnectionString(connectionString);
        // Act
        registryManager.updateDeviceAsync(null);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_043: [The function shall create an async wrapper around the updateDevice() function call, handle the return value or delegate exception]
    @Test
    public void updateDeviceAsync_future_return_ok() throws Exception
    {
        // Arrange
        final HttpMethod expectedMethod = HttpMethod.PUT;
        final byte[] body = { 1 };
        String jsonString = "{\"deviceId\":\"xxx-device\",\"generationId\":\"111111111111111111\",\"etag\":\"MA==\",\"connectionState\":\"Connected\",\"connectionStateUpdatedTime\":\"1111-01-01T00:00:00\",\"status\":\"enabled\",\"statusReason\":\"justbecause\",\"statusUpdatedTime\":\"2222-01-01T00:00:00\",\"lastActivityTime\":\"3333-01-01T00:00:00\",\"cloudToDeviceMessageCount\":10,\"authentication\":{\"symmetricKey\":{\"primaryKey\":\"AAABBBCCC111222333444000\",\"secondaryKey\":\"111222333444555AAABBBCCC\"}}}";

        new MockUp<IotHubConnectionString>()
        {
            @Mock
            public URL getUrlDevice(String deviceId) throws MalformedURLException
            {
                return new URL("http://aaa.com");
            }
        };
        new MockUp<HttpRequest>()
        {
            HttpConnection mockUpHttpConnection;

            @Mock
            public void $init(URL url, HttpMethod method, byte[] body) throws IOException
            {
            }

            @Mock
            public HttpResponse send() throws IOException
            {
                return mockHttpResponse;
            }

            @Mock
            public HttpRequest setHeaderField(String field, String value)
            {
                return null;
            }

            @Mock
            public HttpRequest setReadTimeoutMillis(int timeout)
            {
                return null;
            }
        };
        String connectionString = "HostName=aaa.bbb.ccc;SharedAccessKeyName=XXX;SharedAccessKey=YYY";
        RegistryManager registryManager = RegistryManager.createFromConnectionString(connectionString);
        // Assert
        new NonStrictExpectations()
        {
            IotHubConnectionString iotHubConnectionString = new IotHubConnectionString();
            HttpRequest mockUpHttpRequest = new HttpRequest(null, null, null);
            IotHubExceptionManager iotHubExceptionManager;
            {
                device.serializeToJson();
                result = jsonString;
                iotHubConnectionString.getUrlDevice(anyString);

                new HttpRequest(mockUrl, expectedMethod, body);
                returns(mockUpHttpRequest);
                mockUpHttpRequest.setReadTimeoutMillis(anyInt);
                mockUpHttpRequest.setHeaderField("authorization", anyString);
                mockUpHttpRequest.setHeaderField("Request-Id", "1001");
                mockUpHttpRequest.setHeaderField("Accept", "application/json");
                mockUpHttpRequest.setHeaderField("Content-Type", "application/json");
                mockUpHttpRequest.setHeaderField("charset", "utf-8");

                mockUpHttpRequest.send();

                iotHubExceptionManager.httpResponseVerification(null);

                mockHttpResponse.getBody();
                result = body;
                device.createFromJson(jsonString);
                result = device;
            }
        };
        // Act
        CompletableFuture<Device> completableFuture = registryManager.updateDeviceAsync(device);
        Device returnDevice = completableFuture.get();
        // Assert
        assertNotEquals(null, returnDevice);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_043: [The function shall create an async wrapper around the updateDevice() function call, handle the return value or delegate exception]
    // Assert
    @Test (expected = Exception.class)
    public void updateDeviceAsync_future_throw() throws Exception
    {
        // Arrange
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
        // Act
        CompletableFuture<Device> completableFuture = registryManager.updateDeviceAsync(device);
        completableFuture.get();
    }

    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_044: [The function shall throw IllegalArgumentException if the input device is null]
    // Assert
    @Test (expected = IllegalArgumentException.class)
    public void updateDeviceAsyncForce_input_null() throws Exception
    {
        // Arrange
        String connectionString = "HostName=aaa.bbb.ccc;SharedAccessKeyName=XXX;SharedAccessKey=YYY";
        RegistryManager registryManager = RegistryManager.createFromConnectionString(connectionString);
        // Act
        registryManager.updateDeviceAsync(null, true);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_045: [The function shall create an async wrapper around the updateDevice(Device, device, Boolean forceUpdate) function call, handle the return value or delegate exception]
    @Test
    public void updateDeviceAsyncForce_future_return_ok() throws Exception
    {
        // Arrange
        final HttpMethod expectedMethod = HttpMethod.PUT;
        final byte[] body = { 1 };
        String jsonString = "{\"deviceId\":\"xxx-device\",\"generationId\":\"111111111111111111\",\"etag\":\"MA==\",\"connectionState\":\"Connected\",\"connectionStateUpdatedTime\":\"1111-01-01T00:00:00\",\"status\":\"enabled\",\"statusReason\":\"justbecause\",\"statusUpdatedTime\":\"2222-01-01T00:00:00\",\"lastActivityTime\":\"3333-01-01T00:00:00\",\"cloudToDeviceMessageCount\":10,\"authentication\":{\"symmetricKey\":{\"primaryKey\":\"AAABBBCCC111222333444000\",\"secondaryKey\":\"111222333444555AAABBBCCC\"}}}";

        new MockUp<IotHubConnectionString>()
        {
            @Mock
            public URL getUrlDevice(String deviceId) throws MalformedURLException
            {
                return new URL("http://aaa.com");
            }
        };
        new MockUp<HttpRequest>()
        {
            HttpConnection mockUpHttpConnection;

            @Mock
            public void $init(URL url, HttpMethod method, byte[] body) throws IOException
            {
            }

            @Mock
            public HttpResponse send() throws IOException
            {
                return mockHttpResponse;
            }

            @Mock
            public HttpRequest setHeaderField(String field, String value)
            {
                return null;
            }

            @Mock
            public HttpRequest setReadTimeoutMillis(int timeout)
            {
                return null;
            }
        };
        String connectionString = "HostName=aaa.bbb.ccc;SharedAccessKeyName=XXX;SharedAccessKey=YYY";
        RegistryManager registryManager = RegistryManager.createFromConnectionString(connectionString);
        // Assert
        new NonStrictExpectations()
        {
            IotHubConnectionString iotHubConnectionString = new IotHubConnectionString();
            HttpRequest mockUpHttpRequest = new HttpRequest(null, null, null);
            IotHubExceptionManager iotHubExceptionManager;
            {
                device.serializeToJson();
                result = jsonString;
                iotHubConnectionString.getUrlDevice(anyString);

                new HttpRequest(mockUrl, expectedMethod, body);
                returns(mockUpHttpRequest);
                mockUpHttpRequest.setReadTimeoutMillis(anyInt);
                mockUpHttpRequest.setHeaderField("authorization", anyString);
                mockUpHttpRequest.setHeaderField("Request-Id", "1001");
                mockUpHttpRequest.setHeaderField("Accept", "application/json");
                mockUpHttpRequest.setHeaderField("Content-Type", "application/json");
                mockUpHttpRequest.setHeaderField("charset", "utf-8");

                mockUpHttpRequest.send();

                iotHubExceptionManager.httpResponseVerification(null);

                mockHttpResponse.getBody();
                result = body;
                device.createFromJson(jsonString);
                result = device;
            }
        };
        // Act
        CompletableFuture<Device> completableFuture =  registryManager.updateDeviceAsync(device, true);
        Device returnDevice = completableFuture.get();
        // Assert
        assertNotEquals(null, returnDevice);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_045: [The function shall create an async wrapper around the updateDevice(Device, device, Boolean forceUpdate) function call, handle the return value or delegate exception]
    // Assert
    @Test (expected = Exception.class)
    public void updateDeviceAsyncForce_future_throw() throws Exception
    {
        // Arrange
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
        // Act
        CompletableFuture<Device> completableFuture =  registryManager.updateDeviceAsync(device, true);
        completableFuture.get();
    }

    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_046: [The function shall throw IllegalArgumentException if the input string is null or empty]
    // Assert
    @Test (expected = IllegalArgumentException.class)
    public void removeDevice_input_null() throws Exception
    {
        // Arrange
        String connectionString = "HostName=aaa.bbb.ccc;SharedAccessKeyName=XXX;SharedAccessKey=YYY";
        RegistryManager registryManager = RegistryManager.createFromConnectionString(connectionString);
        // Act
        registryManager.removeDevice(null);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_046: [The function shall throw IllegalArgumentException if the input string is null or empty]
    // Assert
    @Test (expected = IllegalArgumentException.class)
    public void removeDevice_input_empty() throws Exception
    {
        // Arrange
        String connectionString = "HostName=aaa.bbb.ccc;SharedAccessKeyName=XXX;SharedAccessKey=YYY";
        RegistryManager registryManager = RegistryManager.createFromConnectionString(connectionString);
        // Act
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
        // Arrange
        final HttpMethod expectedMethod = HttpMethod.DELETE;
        final byte[] body = { 1 };
        String jsonString = "{\"deviceId\":\"xxx-device\",\"generationId\":\"111111111111111111\",\"etag\":\"MA==\",\"connectionState\":\"Connected\",\"connectionStateUpdatedTime\":\"1111-01-01T00:00:00\",\"status\":\"enabled\",\"statusReason\":\"justbecause\",\"statusUpdatedTime\":\"2222-01-01T00:00:00\",\"lastActivityTime\":\"3333-01-01T00:00:00\",\"cloudToDeviceMessageCount\":10,\"authentication\":{\"symmetricKey\":{\"primaryKey\":\"AAABBBCCC111222333444000\",\"secondaryKey\":\"111222333444555AAABBBCCC\"}}}";

        new MockUp<IotHubConnectionString>()
        {
            @Mock
            public URL getUrlDevice(String deviceId) throws MalformedURLException
            {
                return new URL("http://aaa.com");
            }
        };
        new MockUp<HttpRequest>()
        {
            HttpConnection mockUpHttpConnection;

            @Mock
            public void $init(URL url, HttpMethod method, byte[] body) throws IOException
            {
            }

            @Mock
            public HttpResponse send() throws IOException
            {
                return mockHttpResponse;
            }

            @Mock
            public HttpRequest setHeaderField(String field, String value)
            {
                return null;
            }

            @Mock
            public HttpRequest setReadTimeoutMillis(int timeout)
            {
                return null;
            }
        };
        String connectionString = "HostName=aaa.bbb.ccc;SharedAccessKeyName=XXX;SharedAccessKey=YYY";
        RegistryManager registryManager = RegistryManager.createFromConnectionString(connectionString);
        // Assert
        new NonStrictExpectations()
        {
            IotHubConnectionString iotHubConnectionString = new IotHubConnectionString();
            HttpRequest mockUpHttpRequest = new HttpRequest(null, null, null);
            IotHubExceptionManager iotHubExceptionManager;
            {
                device.serializeToJson();
                result = jsonString;
                iotHubConnectionString.getUrlDevice(anyString);

                new HttpRequest(mockUrl, expectedMethod, body);
                returns(mockUpHttpRequest);
                mockUpHttpRequest.setReadTimeoutMillis(anyInt);
                mockUpHttpRequest.setHeaderField("authorization", anyString);
                mockUpHttpRequest.setHeaderField("If-Match", "*");

                mockUpHttpRequest.send();

                iotHubExceptionManager.httpResponseVerification(null);
            }
        };
        // Act
        registryManager.removeDevice("xxx-device");
    }

    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_052: [The function shall throw IllegalArgumentException if the input string is null or empty]
    // Assert
    @Test (expected = IllegalArgumentException.class)
    public void removeDeviceAsync_input_null() throws Exception
    {
        // Arrange
        String connectionString = "HostName=aaa.bbb.ccc;SharedAccessKeyName=XXX;SharedAccessKey=YYY";
        RegistryManager registryManager = RegistryManager.createFromConnectionString(connectionString);
        // Act
        registryManager.removeDeviceAsync(null);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_052: [The function shall throw IllegalArgumentException if the input string is null or empty]
    // Assert
    @Test (expected = IllegalArgumentException.class)
    public void removeDeviceAsync_input_empty() throws Exception
    {
        // Arrange
        String connectionString = "HostName=aaa.bbb.ccc;SharedAccessKeyName=XXX;SharedAccessKey=YYY";
        RegistryManager registryManager = RegistryManager.createFromConnectionString(connectionString);
        // Act
        registryManager.removeDeviceAsync("");
    }

    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_053: [The function shall create an async wrapper around the removeDevice() function call, handle the return value or delegate exception]
    @Test
    public void removeDeviceAsync_future_return_ok() throws Exception
    {
        // Arrange
        final HttpMethod expectedMethod = HttpMethod.DELETE;
        final byte[] body = { 1 };
        String jsonString = "{\"deviceId\":\"xxx-device\",\"generationId\":\"111111111111111111\",\"etag\":\"MA==\",\"connectionState\":\"Connected\",\"connectionStateUpdatedTime\":\"1111-01-01T00:00:00\",\"status\":\"enabled\",\"statusReason\":\"justbecause\",\"statusUpdatedTime\":\"2222-01-01T00:00:00\",\"lastActivityTime\":\"3333-01-01T00:00:00\",\"cloudToDeviceMessageCount\":10,\"authentication\":{\"symmetricKey\":{\"primaryKey\":\"AAABBBCCC111222333444000\",\"secondaryKey\":\"111222333444555AAABBBCCC\"}}}";

        new MockUp<IotHubConnectionString>()
        {
            @Mock
            public URL getUrlDevice(String deviceId) throws MalformedURLException
            {
                return new URL("http://aaa.com");
            }
        };
        new MockUp<HttpRequest>()
        {
            HttpConnection mockUpHttpConnection;

            @Mock
            public void $init(URL url, HttpMethod method, byte[] body) throws IOException
            {
            }

            @Mock
            public HttpResponse send() throws IOException
            {
                return mockHttpResponse;
            }

            @Mock
            public HttpRequest setHeaderField(String field, String value)
            {
                return null;
            }

            @Mock
            public HttpRequest setReadTimeoutMillis(int timeout)
            {
                return null;
            }
        };
        String connectionString = "HostName=aaa.bbb.ccc;SharedAccessKeyName=XXX;SharedAccessKey=YYY";
        RegistryManager registryManager = RegistryManager.createFromConnectionString(connectionString);
        // Assert
        new NonStrictExpectations()
        {
            IotHubConnectionString iotHubConnectionString = new IotHubConnectionString();
            HttpRequest mockUpHttpRequest = new HttpRequest(null, null, null);
            IotHubExceptionManager iotHubExceptionManager;
            {
                device.serializeToJson();
                result = jsonString;
                iotHubConnectionString.getUrlDevice(anyString);

                new HttpRequest(mockUrl, expectedMethod, body);
                returns(mockUpHttpRequest);
                mockUpHttpRequest.setReadTimeoutMillis(anyInt);
                mockUpHttpRequest.setHeaderField("authorization", anyString);
                mockUpHttpRequest.setHeaderField("If-Match", "*");

                mockUpHttpRequest.send();

                iotHubExceptionManager.httpResponseVerification(null);
            }
        };
        // Act
        CompletableFuture completableFuture = registryManager.removeDeviceAsync("xxx-device");
    }

    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_053: [The function shall create an async wrapper around the removeDevice() function call, handle the return value or delegate exception]
    // Assert
    @Test (expected = Exception.class)
    public void removeDeviceAsync_future_throw() throws Exception
    {
        // Arrange
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
        // Act
        CompletableFuture completableFuture = registryManager.removeDeviceAsync("xxx-device");
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
        // Arrange
        final HttpMethod expectedMethod = HttpMethod.GET;
        final byte[] body = { 1 };
        String jsonString = "{\"deviceId\":\"xxx-device\",\"generationId\":\"111111111111111111\",\"etag\":\"MA==\",\"connectionState\":\"Connected\",\"connectionStateUpdatedTime\":\"1111-01-01T00:00:00\",\"status\":\"enabled\",\"statusReason\":\"justbecause\",\"statusUpdatedTime\":\"2222-01-01T00:00:00\",\"lastActivityTime\":\"3333-01-01T00:00:00\",\"cloudToDeviceMessageCount\":10,\"authentication\":{\"symmetricKey\":{\"primaryKey\":\"AAABBBCCC111222333444000\",\"secondaryKey\":\"111222333444555AAABBBCCC\"}}}";

        new MockUp<IotHubConnectionString>()
        {
            @Mock
            public URL getUrlDevice(String deviceId) throws MalformedURLException
            {
                return new URL("http://aaa.com");
            }
        };
        new MockUp<HttpRequest>()
        {
            HttpConnection mockUpHttpConnection;

            @Mock
            public void $init(URL url, HttpMethod method, byte[] body) throws IOException
            {
            }

            @Mock
            public HttpResponse send() throws IOException
            {
                return mockHttpResponse;
            }

            @Mock
            public HttpRequest setHeaderField(String field, String value)
            {
                return null;
            }

            @Mock
            public HttpRequest setReadTimeoutMillis(int timeout)
            {
                return null;
            }
        };
        String connectionString = "HostName=aaa.bbb.ccc;SharedAccessKeyName=XXX;SharedAccessKey=YYY";
        RegistryManager registryManager = RegistryManager.createFromConnectionString(connectionString);
        // Assert
        new NonStrictExpectations()
        {
            IotHubConnectionString iotHubConnectionString = new IotHubConnectionString();
            HttpRequest mockUpHttpRequest = new HttpRequest(null, null, null);
            IotHubExceptionManager iotHubExceptionManager;
            {
                mockUrl = iotHubConnectionString.getUrlDevice(anyString);

                new HttpRequest(mockUrl, expectedMethod, body);
                mockUpHttpRequest.setReadTimeoutMillis(anyInt);
                mockUpHttpRequest.setHeaderField("authorization", anyString);
                mockUpHttpRequest.setHeaderField("Request-Id", "1001");
                mockUpHttpRequest.setHeaderField("Accept", "application/json");
                mockUpHttpRequest.setHeaderField("Content-Type", "application/json");
                mockUpHttpRequest.setHeaderField("charset", "utf-8");

                mockUpHttpRequest.send();

                iotHubExceptionManager.httpResponseVerification(null);

                mockHttpResponse.getBody();
                result = body;
                RegistryStatistics.parse(jsonString);
            }
        };
        // Act
        RegistryStatistics statistics = registryManager.getStatistics();
        // Assert
        assertNotEquals(null, statistics);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_060: [The function shall create an async wrapper around the getStatistics() function call, handle the return value or delegate exception]
    @Test
    public void getStatisticsAsync_future_return_ok() throws Exception
    {
        // Arrange
        final HttpMethod expectedMethod = HttpMethod.GET;
        final byte[] body = { 1 };
        String jsonString = "{\"deviceId\":\"xxx-device\",\"generationId\":\"111111111111111111\",\"etag\":\"MA==\",\"connectionState\":\"Connected\",\"connectionStateUpdatedTime\":\"1111-01-01T00:00:00\",\"status\":\"enabled\",\"statusReason\":\"justbecause\",\"statusUpdatedTime\":\"2222-01-01T00:00:00\",\"lastActivityTime\":\"3333-01-01T00:00:00\",\"cloudToDeviceMessageCount\":10,\"authentication\":{\"symmetricKey\":{\"primaryKey\":\"AAABBBCCC111222333444000\",\"secondaryKey\":\"111222333444555AAABBBCCC\"}}}";

        new MockUp<IotHubConnectionString>()
        {
            @Mock
            public URL getUrlDevice(String deviceId) throws MalformedURLException
            {
                return new URL("http://aaa.com");
            }
        };
        new MockUp<HttpRequest>()
        {
            HttpConnection mockUpHttpConnection;

            @Mock
            public void $init(URL url, HttpMethod method, byte[] body) throws IOException
            {
            }

            @Mock
            public HttpResponse send() throws IOException
            {
                return mockHttpResponse;
            }

            @Mock
            public HttpRequest setHeaderField(String field, String value)
            {
                return null;
            }

            @Mock
            public HttpRequest setReadTimeoutMillis(int timeout)
            {
                return null;
            }
        };
        String connectionString = "HostName=aaa.bbb.ccc;SharedAccessKeyName=XXX;SharedAccessKey=YYY";
        RegistryManager registryManager = RegistryManager.createFromConnectionString(connectionString);
        // Assert
        new NonStrictExpectations()
        {
            IotHubConnectionString iotHubConnectionString = new IotHubConnectionString();
            HttpRequest mockUpHttpRequest = new HttpRequest(null, null, null);
            IotHubExceptionManager iotHubExceptionManager;
            {
                mockUrl = iotHubConnectionString.getUrlDevice(anyString);

                new HttpRequest(mockUrl, expectedMethod, body);
                mockUpHttpRequest.setReadTimeoutMillis(anyInt);
                mockUpHttpRequest.setHeaderField("authorization", anyString);
                mockUpHttpRequest.setHeaderField("Request-Id", "1001");
                mockUpHttpRequest.setHeaderField("Accept", "application/json");
                mockUpHttpRequest.setHeaderField("Content-Type", "application/json");
                mockUpHttpRequest.setHeaderField("charset", "utf-8");

                mockUpHttpRequest.send();

                iotHubExceptionManager.httpResponseVerification(null);

                mockHttpResponse.getBody();
                result = body;
                RegistryStatistics.parse(jsonString);
            }
        };
        // Act
        CompletableFuture<RegistryStatistics> completableFuture = registryManager.getStatisticsAsync();
        RegistryStatistics registryStatistics = completableFuture.get();
        // Assert
        assertNotEquals(null, registryStatistics);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_060: [The function shall create an async wrapper around the getStatistics() function call, handle the return value or delegate exception]
    // Assert
    @Test (expected = Exception.class)
    public void getStatisticsAsync_future_throw() throws Exception
    {
        // Arrange
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
        // Act
        CompletableFuture<RegistryStatistics> completableFuture = registryManager.getStatisticsAsync();
    }
}
