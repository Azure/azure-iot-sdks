/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.sdk;

import com.microsoft.azure.iot.service.auth.IotHubServiceSasToken;
import com.microsoft.azure.iot.service.transport.amqps.AmqpSend;
import mockit.*;
import org.junit.Test;

import java.io.IOException;
import java.util.concurrent.CompletableFuture;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotEquals;

public class ServiceClientTest
{
    @Mocked
    AmqpSend amqpSend;
    @Mocked
    IotHubServiceSasToken iotHubServiceSasToken;
    @Mocked
    FeedbackReceiver feedbackReceiver;

    // Tests_SRS_SERVICE_SDK_JAVA_SERVICECLIENT_12_001: [The constructor shall throw IllegalArgumentException if the input string is empty or null]
    // Assert
    @Test (expected = IllegalArgumentException.class)
    public void createFromConnectionString_input_null() throws Exception
    {
        // Arrange
        String connectionString = null;
        // Act
        ServiceClient.createFromConnectionString(connectionString);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_SERVICECLIENT_12_001: [The constructor shall throw IllegalArgumentException if the input string is empty or null]
    // Assert
    @Test (expected = IllegalArgumentException.class)
    public void createFromConnectionString_input_empty() throws Exception
    {
        // Arrange
        String connectionString = "";
        // Act
        ServiceClient.createFromConnectionString(connectionString);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_SERVICECLIENT_12_002: [The constructor shall create IotHubConnectionString object using the IotHubConnectionStringBuilder]
    // Tests_SRS_SERVICE_SDK_JAVA_SERVICECLIENT_12_003: [The constructor shall create a new instance of ServiceClient using the created IotHubConnectionString object and return with it]
    @Test
    public void createFromConnectionString_check_call_flow() throws Exception
    {
        // Arrange
        String iotHubName = "b.c.d";
        String hostName = "HOSTNAME." + iotHubName;
        String sharedAccessKeyName = "ACCESSKEYNAME";
        String policyName = "SharedAccessKey";
        String sharedAccessKey = "1234567890abcdefghijklmnopqrstvwxyz=";
        String connectionString = "HostName=" + hostName + ";SharedAccessKeyName=" + sharedAccessKeyName + ";" + policyName + "=" + sharedAccessKey;
        new Expectations()
        {
            {
                IotHubConnectionStringBuilder.createConnectionString(connectionString);
            }
        };
        // Act
        ServiceClient iotHubServiceClient = (ServiceClient) ServiceClient.createFromConnectionString(connectionString);
        // Assert
        assertNotEquals(null, iotHubServiceClient);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_SERVICECLIENT_12_004: [The constructor shall throw IllegalArgumentException if the input object is null]
    // Assert
    @Test (expected = IllegalArgumentException.class)
    public void constructor_input_null() throws Exception
    {
        // Arrange
        IotHubConnectionString iotHubConnectionString = null;
        // Act
        ServiceClient serviceClient = new ServiceClient(iotHubConnectionString);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_SERVICECLIENT_12_005: [The constructor shall create a SAS token object using the IotHubConnectionString]
    // Tests_SRS_SERVICE_SDK_JAVA_SERVICECLIENT_12_006: [The constructor shall store connection string, hostname, username and sasToken]
    // Tests_SRS_SERVICE_SDK_JAVA_SERVICECLIENT_12_007: [The constructor shall create a new instance of AmqpSend object]
    @Test
    public void constructor_create_sas_token() throws Exception
    {
        // Arrange
        String iotHubName = "IOTHUBNAME";
        String hostName = "HOSTNAME";
        String sharedAccessKeyName = "ACCESSKEYNAME";
        String policyName = "SharedAccessKey";
        String sharedAccessKey = "1234567890abcdefghijklmnopqrstvwxyz=";
        String connectionString = "HostName=" + hostName + "." + iotHubName + ";SharedAccessKeyName=" + sharedAccessKeyName + ";" + policyName + "=" + sharedAccessKey;
        IotHubConnectionString iotHubConnectionString = IotHubConnectionStringBuilder.createConnectionString(connectionString);
        new Expectations()
        {
            {
                iotHubServiceSasToken = new IotHubServiceSasToken(withNotNull());
                amqpSend = new AmqpSend(anyString, anyString, anyString);
            }
        };
        // Act
        ServiceClient serviceClient = ServiceClient.createFromConnectionString(connectionString);
        // Assert
        assertNotEquals(hostName, Deencapsulation.getField(serviceClient, "hostName"));
        assertEquals(iotHubConnectionString.getUserString(), Deencapsulation.getField(serviceClient, "userName"));
    }

    // Tests_SRS_SERVICE_SDK_JAVA_SERVICECLIENT_12_008: [The function shall throw IOException if the member AMQP sender object has not been initialized]
    // Assert
    @Test (expected = IOException.class)
    public void open_sender_null() throws Exception
    {
        // Arrange
        String iotHubName = "IOTHUBNAME";
        String hostName = "HOSTNAME";
        String sharedAccessKeyName = "ACCESSKEYNAME";
        String policyName = "SharedAccessKey";
        String sharedAccessKey = "1234567890abcdefghijklmnopqrstvwxyz=";
        String connectionString = "HostName=" + hostName + "." + iotHubName + ";SharedAccessKeyName=" + sharedAccessKeyName + ";" + policyName + "=" + sharedAccessKey;
        IotHubConnectionString iotHubConnectionString = IotHubConnectionStringBuilder.createConnectionString(connectionString);
        ServiceClient serviceClient = ServiceClient.createFromConnectionString(connectionString);
        Deencapsulation.setField(serviceClient, "amqpMessageSender", null);
        // Act
        serviceClient.open();
    }

    // Tests_SRS_SERVICE_SDK_JAVA_SERVICECLIENT_12_009: [The function shall call open() on the member AMQP sender object]
    @Test
    public void open_call_amqpsender_open() throws Exception
    {
        // Arrange
        String iotHubName = "IOTHUBNAME";
        String hostName = "HOSTNAME";
        String sharedAccessKeyName = "ACCESSKEYNAME";
        String policyName = "SharedAccessKey";
        String sharedAccessKey = "1234567890abcdefghijklmnopqrstvwxyz=";
        String connectionString = "HostName=" + hostName + "." + iotHubName + ";SharedAccessKeyName=" + sharedAccessKeyName + ";" + policyName + "=" + sharedAccessKey;
        IotHubConnectionString iotHubConnectionString = IotHubConnectionStringBuilder.createConnectionString(connectionString);
        ServiceClient serviceClient = ServiceClient.createFromConnectionString(connectionString);
        // Assert
        new Expectations()
        {
            {
                amqpSend.open();
            }
        };
        // Act
        serviceClient.open();
    }

    // Tests_SRS_SERVICE_SDK_JAVA_SERVICECLIENT_12_010: [The function shall throw IOException if the member AMQP sender object has not been initialized]
    // Assert
    @Test (expected = IOException.class)
    public void close_sender_null() throws Exception
    {
        // Arrange
        String iotHubName = "IOTHUBNAME";
        String hostName = "HOSTNAME";
        String sharedAccessKeyName = "ACCESSKEYNAME";
        String policyName = "SharedAccessKey";
        String sharedAccessKey = "1234567890abcdefghijklmnopqrstvwxyz=";
        String connectionString = "HostName=" + hostName + "." + iotHubName + ";SharedAccessKeyName=" + sharedAccessKeyName + ";" + policyName + "=" + sharedAccessKey;
        IotHubConnectionString iotHubConnectionString = IotHubConnectionStringBuilder.createConnectionString(connectionString);
        ServiceClient serviceClient = ServiceClient.createFromConnectionString(connectionString);
        Deencapsulation.setField(serviceClient, "amqpMessageSender", null);
        // Act
        serviceClient.close();
    }

    // Tests_SRS_SERVICE_SDK_JAVA_SERVICECLIENT_12_011: [The function shall call close() on the member AMQP sender object]
    @Test
    public void close_call_sender_close() throws Exception
    {
        // Arrange
        String iotHubName = "IOTHUBNAME";
        String hostName = "HOSTNAME";
        String sharedAccessKeyName = "ACCESSKEYNAME";
        String policyName = "SharedAccessKey";
        String sharedAccessKey = "1234567890abcdefghijklmnopqrstvwxyz=";
        String connectionString = "HostName=" + hostName + "." + iotHubName + ";SharedAccessKeyName=" + sharedAccessKeyName + ";" + policyName + "=" + sharedAccessKey;
        IotHubConnectionString iotHubConnectionString = IotHubConnectionStringBuilder.createConnectionString(connectionString);
        ServiceClient serviceClient = ServiceClient.createFromConnectionString(connectionString);
        // Assert
        new Expectations()
        {
            {
                amqpSend.close();
            }
        };
        // Act
        serviceClient.close();
    }

    // Tests_SRS_SERVICE_SDK_JAVA_SERVICECLIENT_12_0012: [The function shall throw IOException if the member AMQP sender object has not been initialized]
    // Assert
    @Test (expected = IOException.class)
    public void send_sender_null() throws Exception
    {
        // Arrange
        String iotHubName = "IOTHUBNAME";
        String hostName = "HOSTNAME";
        String sharedAccessKeyName = "ACCESSKEYNAME";
        String policyName = "SharedAccessKey";
        String sharedAccessKey = "1234567890abcdefghijklmnopqrstvwxyz=";
        String connectionString = "HostName=" + hostName + "." + iotHubName + ";SharedAccessKeyName=" + sharedAccessKeyName + ";" + policyName + "=" + sharedAccessKey;
        String deviceId = "XXX";
        String message = "HELLO";
        IotHubConnectionString iotHubConnectionString = IotHubConnectionStringBuilder.createConnectionString(connectionString);
        ServiceClient serviceClient = ServiceClient.createFromConnectionString(connectionString);
        Deencapsulation.setField(serviceClient, "amqpMessageSender", null);
        // Act
        serviceClient.send(deviceId, message);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_SERVICECLIENT_12_013: [The function shall call send() on the member AMQP sender object with the given parameters]
    @Test
    public void send_call_sender_close() throws Exception
    {
        // Arrange
        String iotHubName = "IOTHUBNAME";
        String hostName = "HOSTNAME";
        String sharedAccessKeyName = "ACCESSKEYNAME";
        String policyName = "SharedAccessKey";
        String sharedAccessKey = "1234567890abcdefghijklmnopqrstvwxyz=";
        String connectionString = "HostName=" + hostName + "." + iotHubName + ";SharedAccessKeyName=" + sharedAccessKeyName + ";" + policyName + "=" + sharedAccessKey;
        IotHubConnectionString iotHubConnectionString = IotHubConnectionStringBuilder.createConnectionString(connectionString);
        String deviceId = "XXX";
        String message = "HELLO";
        ServiceClient serviceClient = ServiceClient.createFromConnectionString(connectionString);
        // Assert
        new Expectations()
        {
            {
                amqpSend.send(deviceId, message);
            }
        };
        // Act
        serviceClient.send(deviceId, message);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_SERVICECLIENT_12_014: [The function shall create an async wrapper around the open() function call, handle the return value or delegate exception]
    @Test
    public void open_async_future_return_ok() throws Exception
    {
        // Arrange
        String iotHubName = "IOTHUBNAME";
        String hostName = "HOSTNAME";
        String sharedAccessKeyName = "ACCESSKEYNAME";
        String policyName = "SharedAccessKey";
        String sharedAccessKey = "1234567890abcdefghijklmnopqrstvwxyz=";
        String connectionString = "HostName=" + hostName + "." + iotHubName + ";SharedAccessKeyName=" + sharedAccessKeyName + ";" + policyName + "=" + sharedAccessKey;
        IotHubConnectionString iotHubConnectionString = IotHubConnectionStringBuilder.createConnectionString(connectionString);
        ServiceClient serviceClient = ServiceClient.createFromConnectionString(connectionString);
        // Assert
        new Expectations()
        {
            {
                amqpSend.open();
                serviceClient.open();
            }
        };
        // Act
        CompletableFuture<Void> completableFuture = serviceClient.openAsync();
        completableFuture.get();
    }

    // Tests_SRS_SERVICE_SDK_JAVA_SERVICECLIENT_12_014: [The function shall create an async wrapper around the open() function call, handle the return value or delegate exception]
    // Assert
    @Test (expected = Exception.class)
    public void open_async_future_throw() throws Exception
    {
        // Arrange
        new MockUp<ServiceClient>()
        {
            @Mock
            public void open() throws IOException
            {
                throw new IOException();
            }
        };
        String iotHubName = "IOTHUBNAME";
        String hostName = "HOSTNAME";
        String sharedAccessKeyName = "ACCESSKEYNAME";
        String policyName = "SharedAccessKey";
        String sharedAccessKey = "1234567890abcdefghijklmnopqrstvwxyz=";
        String connectionString = "HostName=" + hostName + "." + iotHubName + ";SharedAccessKeyName=" + sharedAccessKeyName + ";" + policyName + "=" + sharedAccessKey;
        IotHubConnectionString iotHubConnectionString = IotHubConnectionStringBuilder.createConnectionString(connectionString);
        ServiceClient serviceClient = ServiceClient.createFromConnectionString(connectionString);
        // Act
        CompletableFuture<Void> completableFuture = serviceClient.openAsync();
        completableFuture.get();
    }

    // Tests_SRS_SERVICE_SDK_JAVA_SERVICECLIENT_12_015: [The function shall create an async wrapper around the close() function call, handle the return value or delegate exception]
    @Test
    public void close_async_future_return_ok() throws Exception
    {
        // Arrange
        String iotHubName = "IOTHUBNAME";
        String hostName = "HOSTNAME";
        String sharedAccessKeyName = "ACCESSKEYNAME";
        String policyName = "SharedAccessKey";
        String sharedAccessKey = "1234567890abcdefghijklmnopqrstvwxyz=";
        String connectionString = "HostName=" + hostName + "." + iotHubName + ";SharedAccessKeyName=" + sharedAccessKeyName + ";" + policyName + "=" + sharedAccessKey;
        IotHubConnectionString iotHubConnectionString = IotHubConnectionStringBuilder.createConnectionString(connectionString);
        ServiceClient serviceClient = ServiceClient.createFromConnectionString(connectionString);
        // Assert
        new Expectations()
        {
            {
                amqpSend.close();
                serviceClient.close();
            }
        };
        // Act
        CompletableFuture<Void> completableFuture = serviceClient.closeAsync();
        completableFuture.get();
    }

    // Tests_SRS_SERVICE_SDK_JAVA_SERVICECLIENT_12_015: [The function shall create an async wrapper around the close() function call, handle the return value or delegate exception]
    // Assert
    @Test (expected = Exception.class)
    public void close_async_future_throw() throws Exception
    {
        // Arrange
        new MockUp<ServiceClient>()
        {
            @Mock
            public void close() throws IOException
            {
                throw new IOException();
            }
        };
        String iotHubName = "IOTHUBNAME";
        String hostName = "HOSTNAME";
        String sharedAccessKeyName = "ACCESSKEYNAME";
        String policyName = "SharedAccessKey";
        String sharedAccessKey = "1234567890abcdefghijklmnopqrstvwxyz=";
        String connectionString = "HostName=" + hostName + "." + iotHubName + ";SharedAccessKeyName=" + sharedAccessKeyName + ";" + policyName + "=" + sharedAccessKey;
        IotHubConnectionString iotHubConnectionString = IotHubConnectionStringBuilder.createConnectionString(connectionString);
        ServiceClient serviceClient = ServiceClient.createFromConnectionString(connectionString);
        // Act
        CompletableFuture<Void> completableFuture = serviceClient.closeAsync();
        completableFuture.get();
    }

    // Tests_SRS_SERVICE_SDK_JAVA_SERVICECLIENT_12_016: [The function shall create an async wrapper around the send() function call, handle the return value or delegate exception]
    @Test
    public void send_async_future_return_ok() throws Exception
    {
        // Arrange
        String iotHubName = "IOTHUBNAME";
        String hostName = "HOSTNAME";
        String sharedAccessKeyName = "ACCESSKEYNAME";
        String policyName = "SharedAccessKey";
        String sharedAccessKey = "1234567890abcdefghijklmnopqrstvwxyz=";
        String connectionString = "HostName=" + hostName + "." + iotHubName + ";SharedAccessKeyName=" + sharedAccessKeyName + ";" + policyName + "=" + sharedAccessKey;
        String deviceId = "XXX";
        String message = "HELLO";
        IotHubConnectionString iotHubConnectionString = IotHubConnectionStringBuilder.createConnectionString(connectionString);
        ServiceClient serviceClient = ServiceClient.createFromConnectionString(connectionString);
        // Assert
        new Expectations()
        {
            {
                amqpSend.send(deviceId, message);
                serviceClient.send(deviceId, message);
            }
        };
        // Act
        CompletableFuture<Void> completableFuture = serviceClient.sendAsync(deviceId, message);
        completableFuture.get();
    }

    // Tests_SRS_SERVICE_SDK_JAVA_SERVICECLIENT_12_016: [The function shall create an async wrapper around the send() function call, handle the return value or delegate exception]
    // Assert
    @Test (expected = Exception.class)
    public void send_async_future_throw() throws Exception
    {
        // Arrange
        new MockUp<ServiceClient>()
        {
            @Mock
            public void send(String deviceId, String message) throws IOException
            {
                throw new IOException();
            }
        };
        String iotHubName = "IOTHUBNAME";
        String hostName = "HOSTNAME";
        String sharedAccessKeyName = "ACCESSKEYNAME";
        String policyName = "SharedAccessKey";
        String sharedAccessKey = "1234567890abcdefghijklmnopqrstvwxyz=";
        String connectionString = "HostName=" + hostName + "." + iotHubName + ";SharedAccessKeyName=" + sharedAccessKeyName + ";" + policyName + "=" + sharedAccessKey;
        String deviceId = "XXX";
        String message = "HELLO";
        IotHubConnectionString iotHubConnectionString = IotHubConnectionStringBuilder.createConnectionString(connectionString);
        ServiceClient serviceClient = ServiceClient.createFromConnectionString(connectionString);
        // Act
        CompletableFuture<Void> completableFuture = serviceClient.sendAsync(deviceId, message);
        completableFuture.get();
    }

    // Tests_SRS_SERVICE_SDK_JAVA_SERVICECLIENT_12_017: [The function shall create a FeedbackReceiver object and returns with it]
    @Test
    public void getFeedbackReceiver_good_case() throws Exception
    {
        // Arrange
        String iotHubName = "IOTHUBNAME";
        String hostName = "HOSTNAME";
        String sharedAccessKeyName = "ACCESSKEYNAME";
        String policyName = "SharedAccessKey";
        String sharedAccessKey = "1234567890abcdefghijklmnopqrstvwxyz=";
        String connectionString = "HostName=" + hostName + "." + iotHubName + ";SharedAccessKeyName=" + sharedAccessKeyName + ";" + policyName + "=" + sharedAccessKey;
        String deviceId = "XXX";
        IotHubConnectionString iotHubConnectionString = IotHubConnectionStringBuilder.createConnectionString(connectionString);
        ServiceClient serviceClient = ServiceClient.createFromConnectionString(connectionString);
        // Assert
        new Expectations()
        {
            {
                feedbackReceiver = new FeedbackReceiver(anyString, anyString, anyString, deviceId);
            }
        };
        // Act
        FeedbackReceiver feedbackReceiver = serviceClient.getFeedbackReceiver(deviceId);
        // Assert
        assertNotEquals(null, feedbackReceiver);
    }
}
