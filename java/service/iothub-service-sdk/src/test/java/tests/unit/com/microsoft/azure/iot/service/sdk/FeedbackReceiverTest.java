/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.sdk;

import com.microsoft.azure.iot.service.transport.amqps.AmqpReceive;
import mockit.Deencapsulation;
import mockit.Expectations;
import mockit.Mocked;
import org.junit.Test;

import java.io.IOException;
import java.util.concurrent.CompletableFuture;

import static org.junit.Assert.assertEquals;

public class FeedbackReceiverTest
{
    @Mocked
    AmqpReceive amqpReceive;

    // Tests_SRS_SERVICE_SDK_JAVA_FEEDBACKRECEIVER_12_001: [The constructor shall throw IllegalArgumentException if any the input string is null or empty]
    // Assert
    @Test (expected = IllegalArgumentException.class)
    public void constructor_input_hostName_null() throws Exception
    {
        // Arrange
        String hostName = null;
        String userName = "xxx";
        String sasToken = "xxx";
        String deviceId = "xxx";
        // Act
        FeedbackReceiver feedbackReceiver = new FeedbackReceiver(hostName, userName, sasToken, deviceId);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_FEEDBACKRECEIVER_12_001: [The constructor shall throw IllegalArgumentException if any the input string is null or empty]
    // Assert
    @Test (expected = IllegalArgumentException.class)
    public void constructor_input_userName_null() throws Exception
    {
        // Arrange
        String hostName = "xxx";
        String userName = null;
        String sasToken = "xxx";
        String deviceId = "xxx";
        // Act
        FeedbackReceiver feedbackReceiver = new FeedbackReceiver(hostName, userName, sasToken, deviceId);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_FEEDBACKRECEIVER_12_001: [The constructor shall throw IllegalArgumentException if any the input string is null or empty]
    // Assert
    @Test (expected = IllegalArgumentException.class)
    public void constructor_input_sasToken_null() throws Exception
    {
        // Arrange
        String hostName = "xxx";
        String userName = "xxx";
        String sasToken = null;
        String deviceId = "xxx";
        // Act
        FeedbackReceiver feedbackReceiver = new FeedbackReceiver(hostName, userName, sasToken, deviceId);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_FEEDBACKRECEIVER_12_001: [The constructor shall throw IllegalArgumentException if any the input string is null or empty]
    // Assert
    @Test (expected = IllegalArgumentException.class)
    public void constructor_input_deviceId_null() throws Exception
    {
        // Arrange
        String hostName = "xxx";
        String userName = "xxx";
        String sasToken = "xxx";
        String deviceId = null;
        // Act
        FeedbackReceiver feedbackReceiver = new FeedbackReceiver(hostName, userName, sasToken, deviceId);
    }

    // Assert
    @Test (expected = IllegalArgumentException.class)
    public void constructor_input_hostName_empty() throws Exception
    {
        // Arrange
        String hostName = "";
        String userName = "xxx";
        String sasToken = "xxx";
        String deviceId = "xxx";
        // Act
        FeedbackReceiver feedbackReceiver = new FeedbackReceiver(hostName, userName, sasToken, deviceId);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_FEEDBACKRECEIVER_12_001: [The constructor shall throw IllegalArgumentException if any the input string is null or empty]
    // Assert
    @Test (expected = IllegalArgumentException.class)
    public void constructor_input_userName_empty() throws Exception
    {
        // Arrange
        String hostName = "xxx";
        String userName = "";
        String sasToken = "xxx";
        String deviceId = "xxx";
        // Act
        FeedbackReceiver feedbackReceiver = new FeedbackReceiver(hostName, userName, sasToken, deviceId);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_FEEDBACKRECEIVER_12_001: [The constructor shall throw IllegalArgumentException if any the input string is null or empty]
    // Assert
    @Test (expected = IllegalArgumentException.class)
    public void constructor_input_sasToken_empty() throws Exception
    {
        // Arrange
        String hostName = "xxx";
        String userName = "xxx";
        String sasToken = "";
        String deviceId = "xxx";
        // Act
        FeedbackReceiver feedbackReceiver = new FeedbackReceiver(hostName, userName, sasToken, deviceId);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_FEEDBACKRECEIVER_12_001: [The constructor shall throw IllegalArgumentException if any the input string is null or empty]
    // Assert
    @Test (expected = IllegalArgumentException.class)
    public void constructor_input_deviceId_empty() throws Exception
    {
        // Arrange
        String hostName = "xxx";
        String userName = "xxx";
        String sasToken = "xxx";
        String deviceId = "";
        // Act
        FeedbackReceiver feedbackReceiver = new FeedbackReceiver(hostName, userName, sasToken, deviceId);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_FEEDBACKRECEIVER_12_002: [The constructor shall store deviceId]
    // Tests_SRS_SERVICE_SDK_JAVA_FEEDBACKRECEIVER_12_003: [The constructor shall create a new instance of AmqpReceive object]
    @Test
    public void constructor_save_properties() throws Exception
    {
        // Arrange
        String hostName = "aaa";
        String userName = "bbb";
        String sasToken = "ccc";
        String deviceId = "xxx";
        new Expectations()
        {
            {
                amqpReceive = new AmqpReceive(anyString, anyString, anyString);

            }
        };
        // Act
        FeedbackReceiver feedbackReceiver = new FeedbackReceiver(hostName, userName, sasToken, deviceId);
        // Assert
        assertEquals(deviceId, Deencapsulation.getField(feedbackReceiver, "deviceId"));
    }

    // Tests_SRS_SERVICE_SDK_JAVA_FEEDBACKRECEIVER_12_004: [The function shall throw IOException if the member AMQPReceive object has not been initialized]
    // Assert
    @Test (expected = IOException.class)
    public void open_receiver_null() throws Exception
    {
        // Arrange
        String hostName = "xxx";
        String userName = "xxx";
        String sasToken = "xxx";
        String deviceId = "xxx";
        FeedbackReceiver feedbackReceiver = new FeedbackReceiver(hostName, userName, sasToken, deviceId);
        Deencapsulation.setField(feedbackReceiver, "amqpReceive", null);
        // Act
        feedbackReceiver.open();
    }

    // Tests_SRS_SERVICE_SDK_JAVA_FEEDBACKRECEIVER_12_005: [The function shall call open() on the member AMQPReceive object]
    @Test
    public void open_call_receiver_open() throws Exception
    {
        // Arrange
        String hostName = "xxx";
        String userName = "xxx";
        String sasToken = "xxx";
        String deviceId = "xxx";
        FeedbackReceiver feedbackReceiver = new FeedbackReceiver(hostName, userName, sasToken, deviceId);
        // Assert
        new Expectations()
        {
            {
                amqpReceive.open();
            }
        };
        // Act
        feedbackReceiver.open();
    }

    // Tests_SRS_SERVICE_SDK_JAVA_FEEDBACKRECEIVER_12_006: [The function shall throw IOException if the member AMQPReceive object has not been initialized]
    // Assert
    @Test (expected = IOException.class)
    public void close_receiver_null() throws Exception
    {
        // Arrange
        String hostName = "xxx";
        String userName = "xxx";
        String sasToken = "xxx";
        String deviceId = "xxx";
        FeedbackReceiver feedbackReceiver = new FeedbackReceiver(hostName, userName, sasToken, deviceId);
        Deencapsulation.setField(feedbackReceiver, "amqpReceive", null);
        // Act
        feedbackReceiver.close();
    }

    // Tests_SRS_SERVICE_SDK_JAVA_FEEDBACKRECEIVER_12_007: [The function shall call close() on the member AMQPReceive object]
    @Test
    public void close_call_receiver_close() throws Exception
    {
        // Arrange
        String hostName = "xxx";
        String userName = "xxx";
        String sasToken = "xxx";
        String deviceId = "xxx";
        FeedbackReceiver feedbackReceiver = new FeedbackReceiver(hostName, userName, sasToken, deviceId);
        // Assert
        new Expectations()
        {
            {
                amqpReceive.close();
            }
        };
        // Act
        feedbackReceiver.close();
    }

    // Tests_SRS_SERVICE_SDK_JAVA_FEEDBACKRECEIVER_12_008: [The function shall call receive(long timeoutMs) function with the default timeout]
    @Test
    public void receive_call_receive_timout() throws Exception
    {
        // Arrange
        String hostName = "xxx";
        String userName = "xxx";
        String sasToken = "xxx";
        String deviceId = "xxx";
        FeedbackReceiver feedbackReceiver = new FeedbackReceiver(hostName, userName, sasToken, deviceId);
        // Assert
        new Expectations()
        {
            {
                amqpReceive.receive(anyString, Deencapsulation.getField(feedbackReceiver, "DEFAULT_TIMEOUT_MS"));
            }
        };
        // Act
        feedbackReceiver.receive();
    }

    // Tests_SRS_SERVICE_SDK_JAVA_FEEDBACKRECEIVER_12_009: [The function shall throw IOException if the member AMQPReceive object has not been initialized]
    // Assert
    @Test (expected = IOException.class)
    public void receive_with_timeout_receiver_null() throws Exception
    {
        // Arrange
        long timeoutMs = 1000;
        String hostName = "xxx";
        String userName = "xxx";
        String sasToken = "xxx";
        String deviceId = "xxx";
        FeedbackReceiver feedbackReceiver = new FeedbackReceiver(hostName, userName, sasToken, deviceId);
        Deencapsulation.setField(feedbackReceiver, "amqpReceive", null);
        // Act
        feedbackReceiver.receive(timeoutMs);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_FEEDBACKRECEIVER_12_010: [The function shall call receive() on the member AMQPReceive object and return with the result]
    @Test
    public void receive_with_timout_call_receive_timout() throws Exception
    {
        // Arrange
        long timeoutMs = 1000;
        String hostName = "xxx";
        String userName = "xxx";
        String sasToken = "xxx";
        String deviceId = "xxx";
        FeedbackReceiver feedbackReceiver = new FeedbackReceiver(hostName, userName, sasToken, deviceId);
        // Assert
        new Expectations()
        {
            {
                amqpReceive.receive(anyString, timeoutMs);
            }
        };
        // Act
        feedbackReceiver.receive(timeoutMs);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_FEEDBACKRECEIVER_12_011: [The function shall create an async wrapper around the open() function call]
    @Test
    public void open_async() throws Exception
    {
        // Arrange
        String hostName = "xxx";
        String userName = "xxx";
        String sasToken = "xxx";
        String deviceId = "xxx";
        FeedbackReceiver feedbackReceiver = new FeedbackReceiver(hostName, userName, sasToken, deviceId);
        // Assert
        new Expectations()
        {
            {
                amqpReceive.open();
                feedbackReceiver.open();
            }
        };
        // Act
        CompletableFuture<Void> completableFuture = feedbackReceiver.openAsync();
        completableFuture.get();
    }

    // Tests_SRS_SERVICE_SDK_JAVA_FEEDBACKRECEIVER_12_012: [The function shall create an async wrapper around the close() function call]
    @Test
    public void close_async() throws Exception
    {
        // Arrange
        String hostName = "xxx";
        String userName = "xxx";
        String sasToken = "xxx";
        String deviceId = "xxx";
        FeedbackReceiver feedbackReceiver = new FeedbackReceiver(hostName, userName, sasToken, deviceId);
        // Assert
        new Expectations()
        {
            {
                amqpReceive.close();
                feedbackReceiver.close();
            }
        };
        // Act
        CompletableFuture<Void> completableFuture = feedbackReceiver.closeAsync();
        completableFuture.get();
    }

    // Tests_SRS_SERVICE_SDK_JAVA_FEEDBACKRECEIVER_12_013: [The function shall create an async wrapper around the receive() function call]
    @Test
    public void receive_async() throws Exception
    {
        // Arrange
        String hostName = "xxx";
        String userName = "xxx";
        String sasToken = "xxx";
        String deviceId = "xxx";
        FeedbackReceiver feedbackReceiver = new FeedbackReceiver(hostName, userName, sasToken, deviceId);
        // Assert
        new Expectations()
        {
            {
                amqpReceive.receive(anyString, Deencapsulation.getField(feedbackReceiver, "DEFAULT_TIMEOUT_MS"));
                feedbackReceiver.receive();
            }
        };
        // Act
        CompletableFuture<FeedbackBatch> completableFuture = feedbackReceiver.receiveAsync();
        completableFuture.get();
    }

    // Tests_SRS_SERVICE_SDK_JAVA_FEEDBACKRECEIVER_12_014: [The function shall create an async wrapper around the receive(long timeoutMs) function call]
    @Test
    public void receive_with_timout_async() throws Exception
    {
        // Arrange
        long timeoutMs = 1000;
        String hostName = "xxx";
        String userName = "xxx";
        String sasToken = "xxx";
        String deviceId = "xxx";
        FeedbackReceiver feedbackReceiver = new FeedbackReceiver(hostName, userName, sasToken, deviceId);
        // Assert
        new Expectations()
        {
            {
                amqpReceive.receive(anyString, timeoutMs);
                feedbackReceiver.receive(anyLong);
            }
        };
        // Act
        CompletableFuture<FeedbackBatch> completableFuture = feedbackReceiver.receiveAsync(timeoutMs);
        completableFuture.get();
    }
}
