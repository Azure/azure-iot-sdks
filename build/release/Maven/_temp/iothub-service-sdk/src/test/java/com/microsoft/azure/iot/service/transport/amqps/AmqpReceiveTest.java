/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.transport.amqps;

import com.microsoft.azure.iot.service.sdk.FeedbackBatchMessage;
import mockit.Deencapsulation;
import mockit.Expectations;
import mockit.Mocked;
import mockit.integration.junit4.JMockit;
import org.apache.qpid.proton.Proton;
import org.apache.qpid.proton.engine.Connection;
import org.apache.qpid.proton.engine.Event;
import org.apache.qpid.proton.message.Message;
import org.apache.qpid.proton.reactor.Reactor;
import org.junit.Test;
import org.junit.runner.RunWith;

import java.io.IOException;
import java.util.concurrent.Semaphore;
import java.util.concurrent.TimeUnit;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNull;

/** Unit tests for AmqpReceive */
@RunWith(JMockit.class)
public class AmqpReceiveTest
{
    @Mocked Proton proton;
    @Mocked Reactor reactor;
    @Mocked Event event;
    @Mocked Connection connection;
    @Mocked Message message;
    @Mocked Semaphore semaphore;

    // Tests_SRS_SERVICE_SDK_JAVA_AMQPRECEIVE_12_001: [The constructor shall copy all input parameters to private member variables for event processing]
    @Test
    public void amqpReceive_init_ok()
    {
        // Arrange
        String hostName = "aaa";
        String userName = "bbb";
        String sasToken = "ccc";
        // Act
        AmqpReceive amqpReceive = new AmqpReceive(hostName, userName, sasToken);
        String _hostName = Deencapsulation.getField(amqpReceive, "hostName");
        String _userName = Deencapsulation.getField(amqpReceive, "userName");
        String _sasToken = Deencapsulation.getField(amqpReceive, "sasToken");
        // Assert
        assertEquals(hostName, _hostName);
        assertEquals(userName, _userName);
        assertEquals(sasToken, _sasToken);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_AMQPRECEIVE_12_002: [The event handler shall set the member AmqpsReceiveHandler object to handle the given connection events]
    // Tests_SRS_SERVICE_SDK_JAVA_AMQPRECEIVE_12_003: [The function shall create an AmqpsReceiveHandler object to handle reactor events]
    // Tests_SRS_SERVICE_SDK_JAVA_AMQPRECEIVE_12_004: [The function shall invalidate the member AmqpsReceiveHandler object]
    @Test
    public void onReactorInit_call_flow_and_init_ok() throws IOException
    {
        // Arrange
        String hostName = "aaa";
        String userName = "bbb";
        String sasToken = "ccc";
        AmqpReceive amqpReceive = new AmqpReceive(hostName, userName, sasToken);
        amqpReceive.open();
        // Assert
        new Expectations()
        {
            {
                reactor = event.getReactor();
                connection = reactor.connection(Deencapsulation.getField(amqpReceive, "amqpReceiveHandler"));
            }
        };
        // Act
        amqpReceive.onReactorInit(event);
        amqpReceive.close();
        // Assert
        assertNull(Deencapsulation.getField(amqpReceive, "amqpReceiveHandler"));
    }

    // Tests_SRS_SERVICE_SDK_JAVA_AMQPRECEIVE_12_005: [The function shall initialize the Proton reactor object]
    // Tests_SRS_SERVICE_SDK_JAVA_AMQPRECEIVE_12_006: [The function shall start the Proton reactor object]
    // Tests_SRS_SERVICE_SDK_JAVA_AMQPRECEIVE_12_007: [The function shall acquire a semaphore for event handling with no timeout if the input timoutMs is equal to 0]
    @Test
    public void receive_with_timout_zero_call_flow_ok() throws IOException, InterruptedException
    {
        // Arrange
        String hostName = "aaa";
        String userName = "bbb";
        String sasToken = "ccc";
        String deviceId = "deviceId";
        int timeoutMs = 0;
        AmqpReceive amqpReceive = new AmqpReceive(hostName, userName, sasToken);
        amqpReceive.open();
        // Assert
        new Expectations()
        {
            {
                reactor = proton.reactor(amqpReceive);
                reactor.run();
                semaphore.acquire();
            }
        };
        // Act
        amqpReceive.receive(deviceId, timeoutMs);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_AMQPRECEIVE_12_005: [The function shall initialize the Proton reactor object]
    // Tests_SRS_SERVICE_SDK_JAVA_AMQPRECEIVE_12_006: [The function shall start the Proton reactor object]
    // Tests_SRS_SERVICE_SDK_JAVA_AMQPRECEIVE_12_008: [The function shall acquire a semaphore for event handling with timeout if the input timoutMs is not equal to 0]
    @Test
    public void receiveWithTimout_non_zero_call_ok() throws IOException, InterruptedException
    {
        // Arrange
        String hostName = "aaa";
        String userName = "bbb";
        String sasToken = "ccc";
        String deviceId = "deviceId";
        int timeoutMs = 1;
        AmqpReceive amqpReceive = new AmqpReceive(hostName, userName, sasToken);
        amqpReceive.open();
        // Assert
        new Expectations()
        {
            {
                reactor = proton.reactor(amqpReceive);
                reactor.run();
                semaphore.tryAcquire(timeoutMs, TimeUnit.MILLISECONDS);
            }
        };
        // Act
        amqpReceive.receive(deviceId, timeoutMs);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_AMQPRECEIVE_12_008: [The function shall throw IOException if the send handler object is not initialized]
    // Assert
    @Test (expected = IOException.class)
    public void receiveException_throw() throws IOException, InterruptedException
    {
        // Arrange
        String hostName = "aaa";
        String userName = "bbb";
        String sasToken = "ccc";
        String deviceId = "deviceId";
        int timeoutMs = 1;
        AmqpReceive amqpReceive = new AmqpReceive(hostName, userName, sasToken);
        // Act
        amqpReceive.receive(deviceId, timeoutMs);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_AMQPRECEIVE_12_010: [The function shall parse the received Json string to FeedbackBath object]
    // Tests_SRS_SERVICE_SDK_JAVA_AMQPRECEIVE_12_011: [The function shall release the event handling semaphore]
    @Test
    public void onFeedbackReceived_call_flow_ok()
    {
        // Arrange
        String hostName = "aaa";
        String userName = "bbb";
        String sasToken = "ccc";
        String jsonData = "[]";
        AmqpReceive amqpReceive = new AmqpReceive(hostName, userName, sasToken);
        // Assert
        new Expectations()
        {
            {
                FeedbackBatchMessage.parse(jsonData);
                semaphore.release();
            }
        };
        // Act
        amqpReceive.onFeedbackReceived(jsonData);
    }
}
