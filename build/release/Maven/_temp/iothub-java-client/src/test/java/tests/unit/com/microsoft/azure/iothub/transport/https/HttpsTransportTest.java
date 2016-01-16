// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

package tests.unit.com.microsoft.azure.iothub.transport.https;

import static org.junit.Assert.assertThat;
import static org.hamcrest.CoreMatchers.is;

import com.microsoft.azure.iothub.*;

import com.microsoft.azure.iothub.MessageCallback;
import com.microsoft.azure.iothub.transport.https.HttpsIotHubConnection;
import com.microsoft.azure.iothub.transport.https.HttpsBatchMessage;
import com.microsoft.azure.iothub.transport.https.HttpsMessage;
import com.microsoft.azure.iothub.transport.https.HttpsSingleMessage;
import com.microsoft.azure.iothub.transport.https.HttpsTransport;
import com.microsoft.azure.iothub.transport.IotHubCallbackPacket;
import com.microsoft.azure.iothub.transport.IotHubOutboundPacket;
import junit.framework.AssertionFailedError;
import mockit.MockUp;
import mockit.Mocked;
import mockit.NonStrictExpectations;
import mockit.Verifications;
import mockit.VerificationsInOrder;
import org.junit.Test;

import java.io.IOException;
import java.net.URISyntaxException;
import java.util.HashMap;
import java.util.Map;
import java.util.Queue;

import javax.naming.SizeLimitExceededException;

/** Unit tests for HttpsTransport. */
public class HttpsTransportTest
{
    @Mocked
    DeviceClientConfig mockConfig;
    @Mocked
    HttpsIotHubConnection mockConn;

    // Tests_SRS_HTTPSTRANSPORT_11_021: [The function shall establish an HTTPS connection with the IoT Hub given in the configuration.]
    @Test
    public void openCreatesCorrectHttpsConnection() throws IOException
    {
        HttpsTransport transport = new HttpsTransport(mockConfig);
        transport.open();

        final DeviceClientConfig expectedConfig = mockConfig;
        new Verifications()
        {
            {
                new HttpsIotHubConnection(expectedConfig);
            }
        };
    }

    // Tests_SRS_HTTPSTRANSPORT_11_022: [If the transport is already open, the function shall do nothing.]
    @Test
    public void openDoesNothingIfAlreadyOpen() throws IOException
    {
        HttpsTransport transport = new HttpsTransport(mockConfig);
        transport.open();
        transport.open();

        final DeviceClientConfig expectedConfig = mockConfig;
        new Verifications()
        {
            {
                new HttpsIotHubConnection(expectedConfig);
                times = 1;
            }
        };
    }

    // Tests_SRS_HTTPSTRANSPORT_11_035: [The function shall mark the transport as being closed.]
    @Test
    public void closeMarksTransportAsBeingClosed() throws IOException
    {
        HttpsTransport transport = new HttpsTransport(mockConfig);
        transport.open();
        transport.close();
        transport.open();

        new Verifications()
        {
            {
                new HttpsIotHubConnection((DeviceClientConfig) any);
                times = 2;
            }
        };
    }

    // Tests_SRS_HTTPSTRANSPORT_11_003: [The function shall add a packet containing the message, callback, and callback context to the transport queue.]
    @Test
    public <T extends Queue> void addMessageAddsToTransportQueue(
            @Mocked final Message mockMsg,
            @Mocked final IotHubEventCallback mockCallback,
            @Mocked final IotHubOutboundPacket mockPacket) throws IOException
    {
        final Queue mockQueue = new MockUp<T>()
        {

        }.getMockInstance();
        final Map<String, Object> context = new HashMap<>();

        HttpsTransport transport = new HttpsTransport(mockConfig);
        transport.open();
        transport.addMessage(mockMsg, mockCallback, context);

        new VerificationsInOrder()
        {
            {
                new IotHubOutboundPacket(mockMsg, mockCallback, context);
                mockQueue.add(mockPacket);
            }
        };
    }

    // Tests_SRS_HTTPSTRANSPORT_11_027: [If the transport is closed, the function shall throw an IllegalStateException.]
    @Test(expected = IllegalStateException.class)
    public void addMessageFailsIfTransportNeverOpened(
            @Mocked final Message mockMsg,
            @Mocked final IotHubEventCallback mockCallback,
            @Mocked final IotHubOutboundPacket mockPacket) throws IOException
    {
        final Map<String, Object> context = new HashMap<>();

        HttpsTransport transport = new HttpsTransport(mockConfig);
        transport.addMessage(mockMsg, mockCallback, context);
    }

    // Tests_SRS_HTTPSTRANSPORT_11_027: [If the transport is closed, the function shall throw an IllegalStateException.]
    @Test(expected = IllegalStateException.class)
    public void addMessageFailsIfTransportAlreadyClosed(
            @Mocked final Message mockMsg,
            @Mocked final IotHubEventCallback mockCallback,
            @Mocked final IotHubOutboundPacket mockPacket) throws IOException
    {
        final Map<String, Object> context = new HashMap<>();

        HttpsTransport transport = new HttpsTransport(mockConfig);
        transport.open();
        transport.close();
        transport.addMessage(mockMsg, mockCallback, context);
    }

    // Tests_SRS_HTTPSTRANSPORT_11_008: [The request shall be sent to the IoT Hub given in the configuration from the constructor.]
    @Test
    public void sendMessagesInitializesHttpsConnection(
            @Mocked final Message mockMsg,
            @Mocked final HttpsSingleMessage mockHttpsMsg,
            @Mocked final IotHubEventCallback mockCallback,
            @Mocked final HttpsBatchMessage mockBatch,
            @Mocked final IotHubStatusCode mockStatus)
            throws URISyntaxException, IOException, SizeLimitExceededException
    {
        final Map<String, Object> context = new HashMap<>();

        HttpsTransport transport = new HttpsTransport(mockConfig);
        transport.open();
        transport.addMessage(mockMsg, mockCallback, context);
        transport.sendMessages();

        final DeviceClientConfig expectedConfig = mockConfig;
        new Verifications()
        {
            {
                new HttpsIotHubConnection(expectedConfig);
            }
        };
    }

    // Tests_SRS_HTTPSTRANSPORT_11_004: [If no previous send request had failed while in progress, the function shall batch as many messages as possible such that the batched message body is of size at most 256 kb.]
    // Tests_SRS_HTTPSTRANSPORT_11_005: [The function shall configure a valid HTTPS request and send it to the IoT Hub.]
    @Test
    public void sendMessagesSendsAllMessages(
            @Mocked final Message mockMsg,
            @Mocked final HttpsSingleMessage mockHttpsMsg,
            @Mocked final IotHubEventCallback mockCallback,
            @Mocked final HttpsBatchMessage mockBatch,
            @Mocked final IotHubStatusCode mockStatus)
            throws URISyntaxException, IOException, SizeLimitExceededException
    {
        final Map<String, Object> context = new HashMap<>();
        new NonStrictExpectations()
        {
            {
                HttpsSingleMessage.parseHttpsMessage(mockMsg);
                result = mockHttpsMsg;
                new HttpsBatchMessage();
                result = mockBatch;
            }
        };

        HttpsTransport transport = new HttpsTransport(mockConfig);
        transport.open();
        transport.addMessage(mockMsg, mockCallback, context);
        transport.addMessage(mockMsg, mockCallback, context);
        transport.addMessage(mockMsg, mockCallback, context);
        transport.sendMessages();

        final HttpsSingleMessage expectedSingleMsg = mockHttpsMsg;
        final HttpsMessage expectedMsg = mockBatch;
        new VerificationsInOrder()
        {
            {
                mockBatch.addMessage(expectedSingleMsg);
                times = 3;
                mockConn.sendEvent(expectedMsg);
            }
        };
    }

    // Tests_SRS_HTTPSTRANSPORT_11_012: [If a previous send request had failed while in progress, the function shall resend the request.]
    @Test
    public void sendMessagesResendsFailedBatch(
            @Mocked final Message mockMsg,
            @Mocked final HttpsSingleMessage mockHttpsMsg,
            @Mocked final IotHubEventCallback mockCallback,
            @Mocked final HttpsBatchMessage mockBatch,
            @Mocked final IotHubStatusCode mockStatus)
            throws URISyntaxException, IOException, SizeLimitExceededException
    {
        final Map<String, Object> context = new HashMap<>();
        new NonStrictExpectations()
        {
            {
                HttpsSingleMessage.parseHttpsMessage(mockMsg);
                result = mockHttpsMsg;
                new HttpsBatchMessage();
                result = mockBatch;
                mockConn.sendEvent((HttpsMessage) any);
                result = new IOException();
                result = mockStatus;
            }
        };

        HttpsTransport transport = new HttpsTransport(mockConfig);
        transport.open();
        transport.addMessage(mockMsg, mockCallback, context);
        transport.addMessage(mockMsg, mockCallback, context);
        transport.addMessage(mockMsg, mockCallback, context);
        try
        {
            transport.sendMessages();
            throw new AssertionFailedError();
        }
        catch (IOException e)
        {

        }
        transport.addMessage(mockMsg, mockCallback, context);
        transport.sendMessages();

        final HttpsSingleMessage expectedSingleMsg = mockHttpsMsg;
        final HttpsMessage expectedMsg = mockBatch;
        new VerificationsInOrder()
        {
            {
                mockBatch.addMessage(expectedSingleMsg);
                times = 3;
                mockConn.sendEvent(expectedMsg);
                times = 2;
            }
        };
    }

    // Tests_SRS_HTTPSTRANSPORT_11_013: [If no messages fit using the batch format, the function shall send a single message without the batch format.]
    @Test
    public void sendMessagesSendsSingleMesssageIfBatchFormatExceedsMaxSize(
            @Mocked final Message mockMsg,
            @Mocked final HttpsSingleMessage mockHttpsMsg,
            @Mocked final IotHubEventCallback mockCallback,
            @Mocked final HttpsBatchMessage mockBatch,
            @Mocked final IotHubStatusCode mockStatus)
            throws URISyntaxException, IOException, SizeLimitExceededException
    {
        final Map<String, Object> context = new HashMap<>();
        new NonStrictExpectations()
        {
            {
                HttpsSingleMessage.parseHttpsMessage(mockMsg);
                result = mockHttpsMsg;
                new HttpsBatchMessage();
                result = mockBatch;
                mockBatch.addMessage(mockHttpsMsg);
                result = new SizeLimitExceededException();
            }
        };

        HttpsTransport transport = new HttpsTransport(mockConfig);
        transport.open();
        transport.addMessage(mockMsg, mockCallback, context);
        transport.addMessage(mockMsg, mockCallback, context);
        transport.addMessage(mockMsg, mockCallback, context);
        transport.sendMessages();

        final HttpsMessage expectedMsg = mockHttpsMsg;
        new Verifications()
        {
            {
                mockConn.sendEvent(expectedMsg);
            }
        };
    }

    // Tests_SRS_HTTPSTRANSPORT_11_014: [If the send request fails while in progress, the function shall throw an IOException.]
    @Test(expected = IOException.class)
    public void sendMessagesThrowsIOExceptionIfRequestFails(
            @Mocked final Message mockMsg,
            @Mocked final HttpsSingleMessage mockHttpsMsg,
            @Mocked final IotHubEventCallback mockCallback,
            @Mocked final HttpsBatchMessage mockBatch,
            @Mocked final IotHubStatusCode mockStatus)
            throws URISyntaxException, IOException
    {
        final Map<String, Object> context = new HashMap<>();
        new NonStrictExpectations()
        {
            {
                HttpsSingleMessage.parseHttpsMessage(mockMsg);
                result = mockHttpsMsg;
                new HttpsBatchMessage();
                result = mockBatch;
                mockConn.sendEvent((HttpsMessage) any);
                result = new IOException();
            }
        };

        HttpsTransport transport = new HttpsTransport(mockConfig);
        transport.open();
        transport.addMessage(mockMsg, mockCallback, context);
        transport.sendMessages();
    }

    // Tests_SRS_HTTPSTRANSPORT_11_006: [The function shall add a packet containing the callbacks, contexts, and response for all sent messages to the callback queue.]
    @Test
    public <T extends Queue> void sendMessagesAddsToCompletedQueue(
            @Mocked final Message mockMsg,
            @Mocked final HttpsSingleMessage mockHttpsMsg,
            @Mocked final IotHubEventCallback mockCallback,
            @Mocked final HttpsBatchMessage mockBatch,
            @Mocked final IotHubStatusCode mockStatus,
            @Mocked final IotHubCallbackPacket mockCallbackPacket)
            throws URISyntaxException, IOException
    {
        final Queue mockQueue = new MockUp<T>()
        {

        }.getMockInstance();
        final IotHubStatusCode iotHubStatus = IotHubStatusCode.OK;
        new NonStrictExpectations()
        {
            {
                HttpsSingleMessage.parseHttpsMessage(mockMsg);
                result = mockHttpsMsg;
                new HttpsBatchMessage();
                result = mockBatch;
                mockConn.sendEvent((HttpsMessage) any);
                result = iotHubStatus;
            }
        };
        final Map<String, Object> context = new HashMap<>();

        HttpsTransport transport = new HttpsTransport(mockConfig);
        transport.open();
        transport.addMessage(mockMsg, mockCallback, context);
        transport.addMessage(mockMsg, mockCallback, context);
        transport.sendMessages();

        final IotHubStatusCode expectedStatus = iotHubStatus;
        final Map<String, Object> expectedContext = context;
        new VerificationsInOrder()
        {
            {
                new IotHubCallbackPacket(expectedStatus, mockCallback,
                        expectedContext);
                mockQueue.add(mockCallbackPacket);
                times = 2;
            }
        };
    }

    // Tests_SRS_HTTPSTRANSPORT_11_029: [If the transport is closed, the function shall throw an IllegalStateException.]
    @Test(expected = IllegalStateException.class)
    public void sendMessagesFailsIfTransportNeverOpened() throws IOException
    {
        HttpsTransport transport = new HttpsTransport(mockConfig);
        transport.sendMessages();
    }

    // Tests_SRS_HTTPSTRANSPORT_11_029: [If the transport is closed, the function shall throw an IllegalStateException.]
    @Test(expected = IllegalStateException.class)
    public void sendMessagesFailsIfTransportAlreadyClosed() throws IOException
    {
        HttpsTransport transport = new HttpsTransport(mockConfig);
        transport.open();
        transport.close();
        transport.sendMessages();
    }

    // Tests_SRS_HTTPSTRANSPORT_11_007: [The function shall invoke all callbacks on the callback queue.]
    @Test
    public <T extends Queue> void invokeCallbacksInvokesAllEventCallbacks(
            @Mocked final Message mockMsg,
            @Mocked final HttpsSingleMessage mockHttpsMsg,
            @Mocked final IotHubEventCallback mockCallback,
            @Mocked final HttpsBatchMessage mockBatch,
            @Mocked final IotHubStatusCode mockStatus,
            @Mocked final IotHubCallbackPacket mockCallbackPacket)
            throws URISyntaxException, IOException
    {
        final Queue mockQueue = new MockUp<T>()
        {

        }.getMockInstance();
        final IotHubStatusCode iotHubStatus =
                IotHubStatusCode.INTERNAL_SERVER_ERROR;
        final Map<String, Object> context = new HashMap<>();
        new NonStrictExpectations()
        {
            {
                HttpsSingleMessage.parseHttpsMessage(mockMsg);
                result = mockHttpsMsg;
                new HttpsBatchMessage();
                result = mockBatch;
                mockCallbackPacket.getStatus();
                result = iotHubStatus;
                mockCallbackPacket.getContext();
                result = context;
            }
        };

        HttpsTransport transport = new HttpsTransport(mockConfig);
        transport.open();
        transport.addMessage(mockMsg, mockCallback, context);
        transport.addMessage(mockMsg, mockCallback, context);
        transport.addMessage(mockMsg, mockCallback, context);
        transport.sendMessages();
        transport.invokeCallbacks();

        final IotHubStatusCode expectedStatus = iotHubStatus;
        final Map<String, Object> expectedContext = context;
        new Verifications()
        {
            {
                mockCallback.execute(expectedStatus, expectedContext);
                times = 3;
            }
        };
    }

    // Tests_SRS_HTTPSTRANSPORT_11_031: [If the transport is closed, the function shall throw an IllegalStateException.]
    @Test(expected = IllegalStateException.class)
    public void invokeCallbacksFailsIfTransportNeverOpened() throws IOException
    {
        HttpsTransport transport = new HttpsTransport(mockConfig);
        transport.invokeCallbacks();
    }

    // Tests_SRS_HTTPSTRANSPORT_11_031: [If the transport is closed, the function shall throw an IllegalStateException.]
    @Test(expected = IllegalStateException.class)
    public void invokeCallbacksFailsIfTransportAlreadyClosed()
            throws IOException
    {
        HttpsTransport transport = new HttpsTransport(mockConfig);
        transport.open();
        transport.close();
        transport.invokeCallbacks();
    }

    // Tests_SRS_HTTPSTRANSPORT_11_009: [The function shall poll the IoT Hub for messages.]
    @Test
    public void handleMessagePollsForMessages(
            @Mocked final IotHubStatusCode mockStatus)
            throws URISyntaxException, IOException
    {
        HttpsTransport transport = new HttpsTransport(mockConfig);
        transport.open();
        transport.handleMessage();

        new Verifications()
        {
            {
                mockConn.receiveMessage();
            }
        };
    }

    // Tests_SRS_HTTPSTRANSPORT_11_010: [If a message is found and a message callback is registered, the function shall invoke the callback on the message.]
    @Test
    public void handleMessageInvokesCallback(
            @Mocked final IotHubStatusCode mockStatus,
            @Mocked final MessageCallback mockCallback,
            @Mocked final Message mockMessage)
            throws URISyntaxException, IOException
    {
        final HashMap<String, Object> context = new HashMap<>();
        new NonStrictExpectations()
        {
            {
                mockConfig.getMessageCallback();
                result = mockCallback;
                mockConfig.getMessageContext();
                result = context;
                mockConn.receiveMessage();
                result = mockMessage;
            }
        };

        HttpsTransport transport = new HttpsTransport(mockConfig);
        transport.open();
        transport.handleMessage();

        new Verifications()
        {
            {
                mockCallback.execute(mockMessage, context);
            }
        };
    }

    // Tests_SRS_HTTPSTRANSPORT_11_011: [The function shall return the message result (one of COMPLETE, ABANDON, or REJECT) to the IoT Hub.]
    @Test
    public void handleMessageSendsMessageResult(
            @Mocked final IotHubStatusCode mockStatus,
            @Mocked final MessageCallback mockCallback,
            @Mocked final Message mockMessage)
            throws URISyntaxException, IOException
    {
        final IotHubMessageResult messageResult =
                IotHubMessageResult.COMPLETE;
        new NonStrictExpectations()
        {
            {
                mockCallback.execute((Message) any, any);
                result = messageResult;
            }
        };

        HttpsTransport transport = new HttpsTransport(mockConfig);
        transport.open();
        transport.handleMessage();

        final IotHubMessageResult expectedResult = messageResult;
        new Verifications()
        {
            {
                mockConn.sendMessageResult(expectedResult);
            }
        };
    }

    // Tests_SRS_HTTPSTRANSPORT_11_020: [If the response from sending the IoT Hub message result does not have status code OK_EMPTY, the function shall throw an IOException.]
    @Test(expected = IOException.class)
    public void handleMessageThrowsIOExceptionIfSendResultFails(
            @Mocked final IotHubStatusCode mockStatus,
            @Mocked final MessageCallback mockCallback,
            @Mocked final Message mockMessage)
            throws URISyntaxException, IOException
    {
        new NonStrictExpectations()
        {
            {
                mockConn.sendMessageResult((IotHubMessageResult) any);
                result = new IOException();
            }
        };

        HttpsTransport transport = new HttpsTransport(mockConfig);
        transport.open();
        transport.handleMessage();
    }

    // Tests_SRS_HTTPSTRANSPORT_11_033: [If the transport is closed, the function shall throw an IllegalStateException.]
    @Test(expected = IllegalStateException.class)
    public void handleMessageFailsIfTransportNeverOpened() throws IOException
    {
        HttpsTransport transport = new HttpsTransport(mockConfig);
        transport.handleMessage();
    }

    // Tests_SRS_HTTPSTRANSPORT_11_033: [If the transport is closed, the function shall throw an IllegalStateException.]
    @Test(expected = IllegalStateException.class)
    public void handleMessageFailsIfTransportAlreadyClosed()
            throws IOException
    {
        HttpsTransport transport = new HttpsTransport(mockConfig);
        transport.open();
        transport.close();
        transport.handleMessage();
    }

    // Tests_SRS_HTTPSTRANSPORT_11_015: [The function shall return true if the waiting list, in progress list, and callback list are all empty, and false otherwise.]
    @Test
    public void isEmptyReturnsFalseIfWaitingListIsNotEmpty(
            @Mocked final Message mockMsg,
            @Mocked final IotHubEventCallback mockCallback)
            throws IOException
    {
        final Map<String, Object> context = new HashMap<>();

        HttpsTransport transport = new HttpsTransport(mockConfig);
        transport.open();
        transport.addMessage(mockMsg, mockCallback, context);
        boolean testIsEmpty = transport.isEmpty();

        boolean expectedIsEmpty = false;
        assertThat(testIsEmpty, is(expectedIsEmpty));
    }

    // Tests_SRS_HTTPSTRANSPORT_11_015: [The function shall return true if the waiting list, in progress list, and callback list are all empty, and false otherwise.]
    @Test
    public void isEmptyReturnsFalseIfCallbackListIsNotEmpty(
            @Mocked final Message mockMsg,
            @Mocked final HttpsSingleMessage mockHttpsMsg,
            @Mocked final IotHubEventCallback mockCallback,
            @Mocked final HttpsBatchMessage mockBatch,
            @Mocked final IotHubStatusCode mockStatus)
            throws URISyntaxException, IOException, SizeLimitExceededException
    {
        final Map<String, Object> context = new HashMap<>();
        new NonStrictExpectations()
        {
            {
                HttpsSingleMessage.parseHttpsMessage(mockMsg);
                result = mockHttpsMsg;
                new HttpsBatchMessage();
                result = mockBatch;
            }
        };

        HttpsTransport transport = new HttpsTransport(mockConfig);
        transport.open();
        transport.addMessage(mockMsg, mockCallback, context);
        transport.addMessage(mockMsg, mockCallback, context);
        transport.addMessage(mockMsg, mockCallback, context);
        transport.sendMessages();
        boolean testIsEmpty = transport.isEmpty();

        final boolean expectedIsEmpty = false;
        assertThat(testIsEmpty, is(expectedIsEmpty));
    }

    // Tests_SRS_HTTPSTRANSPORT_11_015: [The function shall return true if the waiting list, in progress list, and callback list are all empty, and false otherwise.]
    @Test
    public void isEmptyReturnsTrueIfEmpty(
            @Mocked final Message mockMsg,
            @Mocked final HttpsSingleMessage mockHttpsMsg,
            @Mocked final IotHubEventCallback mockCallback,
            @Mocked final HttpsBatchMessage mockBatch,
            @Mocked final IotHubStatusCode mockStatus)
            throws URISyntaxException, IOException, SizeLimitExceededException
    {
        final Map<String, Object> context = new HashMap<>();
        new NonStrictExpectations()
        {
            {
                HttpsSingleMessage.parseHttpsMessage(mockMsg);
                result = mockHttpsMsg;
                new HttpsBatchMessage();
                result = mockBatch;
            }
        };

        HttpsTransport transport = new HttpsTransport(mockConfig);
        transport.open();
        transport.addMessage(mockMsg, mockCallback, context);
        transport.addMessage(mockMsg, mockCallback, context);
        transport.addMessage(mockMsg, mockCallback, context);
        transport.sendMessages();
        transport.invokeCallbacks();
        boolean testIsEmpty = transport.isEmpty();

        final boolean expectedIsEmpty = true;
        assertThat(testIsEmpty, is(expectedIsEmpty));
    }
}
