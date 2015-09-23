// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

package tests.unit.com.microsoft.azure.iothub.transport;

import com.microsoft.azure.iothub.transport.amqps.AmqpsTransport;
import com.microsoft.azure.iothub.transport.IotHubSendTask;
import mockit.Mocked;
import mockit.NonStrictExpectations;
import mockit.Verifications;
import org.junit.Test;

import java.io.IOException;
import java.net.URISyntaxException;

/** Unit tests for IotHubSendTask. */
public class IotHubSendTaskTest
{
    @Mocked
    AmqpsTransport mockTransport;

    // Tests_SRS_IOTHUBSENDTASK_11_001: [The constructor shall save the transport.]
    // Tests_SRS_IOTHUBSENDTASK_11_002: [The function shall send all messages on the transport queue.]
    @Test
    public void runSendsAllMessages() throws IOException, URISyntaxException
    {
        IotHubSendTask sendTask = new IotHubSendTask(mockTransport);
        sendTask.run();

        new Verifications()
        {
            {
                mockTransport.sendMessages();
            }
        };
    }

    // Tests_SRS_IOTHUBSENDTASK_11_003: [The function shall invoke all callbacks on the transport's callback queue.]
    @Test
    public void runInvokesAllCallbacks() throws IOException, URISyntaxException
    {
        IotHubSendTask sendTask = new IotHubSendTask(mockTransport);
        sendTask.run();

        new Verifications()
        {
            {
                mockTransport.invokeCallbacks();
            }
        };
    }

    // Tests_SRS_IOTHUBSENDTASK_11_005: [The function shall not crash because of an IOException thrown by the transport.]
    @Test
    public void runDoesNotCrashFromIoException()
            throws IOException, URISyntaxException
    {
        new NonStrictExpectations()
        {
            {
                mockTransport.sendMessages();
                result = new IOException();
            }
        };

        IotHubSendTask sendTask = new IotHubSendTask(mockTransport);
        sendTask.run();
    }

    // Tests_SRS_IOTHUBSENDTASK_11_008: [The function shall not crash because of any error or exception thrown by the transport.]
    @Test
    public void runDoesNotCrashFromThrowable()
            throws IOException, URISyntaxException
    {
        new NonStrictExpectations()
        {
            {
                mockTransport.sendMessages();
                result = new Throwable("Test that send does not crash.");
            }
        };

        IotHubSendTask sendTask = new IotHubSendTask(mockTransport);
        sendTask.run();
    }
}
