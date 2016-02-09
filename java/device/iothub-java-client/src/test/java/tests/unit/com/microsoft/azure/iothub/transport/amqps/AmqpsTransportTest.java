// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

package tests.unit.com.microsoft.azure.iothub.transport.amqps;

import static org.junit.Assert.assertThat;
import static org.hamcrest.CoreMatchers.is;
import static org.junit.Assert.assertTrue;

import com.microsoft.azure.iothub.*;
import com.microsoft.azure.iothub.Message;
import com.microsoft.azure.iothub.MessageCallback;
import com.microsoft.azure.iothub.transport.amqps.*;
import com.microsoft.azure.iothub.transport.IotHubCallbackPacket;
import com.microsoft.azure.iothub.transport.IotHubOutboundPacket;

import junit.framework.AssertionFailedError;
import mockit.*;
import org.apache.qpid.proton.message.*;
import org.apache.qpid.proton.message.impl.MessageImpl;
import org.junit.Test;

import java.io.IOException;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.Map;
import java.util.Queue;
import java.util.concurrent.*;

/** Unit tests for AmqpsTransport. */
public class AmqpsTransportTest
{
    @Mocked
    DeviceClientConfig mockConfig;

    @Mocked
    AmqpsIotHubConnection mockConnection;

    @Mocked
    AmqpsMessage mockAmqpsMessage;

    @Mocked
    AmqpsIotHubConnectionBaseHandler mockHandler;

    // Tests_SRS_AMQPSTRANSPORT_11_019: [The function shall open an AMQPS session with the IoT Hub given in the configuration.]
    @Test
    public void openOpensAmqpsConnection() throws IOException, ExecutionException, InterruptedException, TimeoutException {
        new NonStrictExpectations()
        {
            {
                new AmqpsIotHubConnection(mockConfig);
                result = mockConnection;
            }
        };

        AmqpsTransport transport = new AmqpsTransport(mockConfig);
        transport.open();

        final AmqpsIotHubConnection expectedConnection = mockConnection;
        new Verifications()
        {
            {
                expectedConnection.open();
            }
        };
    }

    // Tests_SRS_AMQPSTRANSPORT_11_020: [If an AMQPS session is already open, the function shall do nothing.]
    @Test
    public void openDoesNothingIfAlreadyOpened() throws IOException, ExecutionException, InterruptedException, TimeoutException {
        AmqpsTransport transport = new AmqpsTransport(mockConfig);
        transport.open();
        transport.open();

        final AmqpsIotHubConnection expectedConnection = mockConnection;
        new Verifications()
        {
            {
                expectedConnection.open();
                times = 1;
            }
        };
    }

    // Tests_SRS_AMQPSTRANSPORT_11_021: [The function shall close an AMQPS session with the IoT Hub given in the configuration.]
    @Test
    public void closeClosesAmqpsConnection() throws IOException
    {
        AmqpsTransport transport = new AmqpsTransport(mockConfig);
        transport.open();
        transport.close();

        final AmqpsIotHubConnection expectedConnection = mockConnection;
        new Verifications()
        {
            {
                expectedConnection.close();
            }
        };
    }

    // Tests_SRS_AMQPSTRANSPORT_11_023: [If the AMQPS session is closed, the function shall do nothing.]
    @Test
    public void closeDoesNothingIfConnectionAlreadyClosed() throws IOException
    {
        AmqpsTransport transport = new AmqpsTransport(mockConfig);
        transport.open();
        transport.close();
        transport.close();

        final AmqpsIotHubConnection expectedConnection = mockConnection;
        new Verifications()
        {
            {
                expectedConnection.close();
                times = 1;
            }
        };
    }

    // Tests_SRS_AMQPSTRANSPORT_11_001: [The function shall initialize an empty queue for messages waiting to be sent.]
    // Tests_SRS_AMQPSTRANSPORT_11_003: [The function shall add a packet containing the message, callback, and callback context to the transport queue.]
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

        AmqpsTransport transport = new AmqpsTransport(mockConfig);
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

    // Tests_SRS_AMQPSTRANSPORT_11_025: [If the AMQPS session is closed, the function shall throw an IllegalStateException.]
    @Test(expected = IllegalStateException.class)
    public void addMessageFailsIfTransportNotOpened(
            @Mocked final Message mockMsg,
            @Mocked final IotHubEventCallback mockCallback)
            throws IOException
    {
        final Map<String, Object> context = new HashMap<>();

        AmqpsTransport transport = new AmqpsTransport(mockConfig);
        transport.addMessage(mockMsg, mockCallback, context);
    }

    // Tests_SRS_AMQPSTRANSPORT_11_025: [If the AMQPS session is closed, the function shall throw an IllegalStateException.]
    @Test(expected = IllegalStateException.class)
    public void addMessageFailsIfTransportAlreadyClosed(
            @Mocked final Message mockMsg,
            @Mocked final IotHubEventCallback mockCallback)
            throws IOException
    {
        final Map<String, Object> context = new HashMap<>();

        AmqpsTransport transport = new AmqpsTransport(mockConfig);
        transport.open();
        transport.close();
        transport.addMessage(mockMsg, mockCallback, context);
    }

    // Tests_SRS_AMQPSTRANSPORT_11_004: [The function shall attempt to send every message on its waiting list, one at a time.]
    // Tests_SRS_AMQPSTRANSPORT_11_005: [If no AMQPS session exists with the IoT Hub, the function shall establish one.]
    //TODO: Improve with async-safe tests.
    @Test
    public void sendMessagesSendsAllMessages(
            @Mocked final Message mockMsg,
            @Mocked final IotHubEventCallback mockCallback,
            @Mocked final IotHubOutboundPacket mockPacket)
            throws IOException
    {
        final Map<String, Object> context = new HashMap<>();
        new NonStrictExpectations()
        {
            {
                new AmqpsIotHubConnection(mockConfig);
                result = mockConnection;
                new IotHubOutboundPacket(mockMsg, mockCallback, context);
                result = mockPacket;
                mockPacket.getMessage();
                result = mockMsg;
            }
        };

        AmqpsTransport transport = new AmqpsTransport(mockConfig);
        transport.open();
        transport.addMessage(mockMsg, mockCallback, context);
        transport.addMessage(mockMsg, mockCallback, context);
        transport.sendMessages();

        assertTrue(transport.isEmpty());
    }

    //TODO: // Tests_SRS_AMQPSTRANSPORT_11_006: [For each message being sent, the function shall send the message and add the IoT Hub status code along with the callback and context to the callback list.]
    //TODO: // Tests_SRS_AMQPSTRANSPORT_11_007: [If the IoT Hub could not be reached, the message shall be buffered to be sent again next time.]

    // Tests_SRS_AMQPSTRANSPORT_11_015: [If the IoT Hub could not be reached, the function shall throw an IOException.]
    @Test(expected = IOException.class)
    public void sendMessagesThrowsIOExceptionIfSendFails(
            @Mocked final Message mockMsg,
            @Mocked final IotHubEventCallback mockCallback,
            @Mocked final IotHubOutboundPacket mockPacket)
            throws IOException
    {
        final Map<String, Object> context = new HashMap<>();
        new NonStrictExpectations()
        {
            {
                new AmqpsIotHubConnection(mockConfig);
                result = mockConnection;
                new IotHubOutboundPacket(mockMsg, mockCallback, context);
                result = mockPacket;
                mockPacket.getMessage();
                result = mockMsg;
                Deencapsulation.invoke(mockConnection, "getCompletionStatus");
                result = new Exception();
            }
        };

        AmqpsTransport transport = new AmqpsTransport(mockConfig);
        transport.open();
        transport.addMessage(mockMsg, mockCallback, context);
        transport.sendMessages();
    }

    // Tests_SRS_AMQPSTRANSPORT_11_034: [If the function throws any exception or error, it shall also close the AMQPS session.]
    @Test
    public void sendMessagesClosesAmqpsConnectionIfSendFails(
            @Mocked final Message mockMsg,
            @Mocked final IotHubEventCallback mockCallback,
            @Mocked final IotHubOutboundPacket mockPacket,
            @Mocked final LinkedBlockingQueue<IotHubOutboundPacket> mockQueue)
            throws IOException
    {
        final Map<String, Object> context = new HashMap<>();
        new NonStrictExpectations()
        {
            {
                new AmqpsIotHubConnection(mockConfig);
                result = mockConnection;
                new IotHubOutboundPacket(mockMsg, mockCallback, context);
                result = mockPacket;
                mockPacket.getMessage();
                result = mockMsg;
                new LinkedBlockingQueue<>();
                result = mockQueue;
                mockQueue.size();
                result = 1;
                mockQueue.remove();
                result = new Exception();
            }
        };

        AmqpsTransport transport = new AmqpsTransport(mockConfig);
        try
        {
            transport.open();
            transport.addMessage(mockMsg, mockCallback, context);
            transport.sendMessages();
            throw new AssertionError();
        }
        catch (Throwable e)
        {

        }

        final AmqpsIotHubConnection expectedConnection = mockConnection;
        new Verifications()
        {
            {
                expectedConnection.close();
            }
        };
    }

    // Tests_SRS_AMQPSTRANSPORT_11_026: [If the transport had previously encountered any exception or error while open, it shall reopen a new AMQPS session with the IoT Hub given in the configuration.]
    @Test
    public void sendMessagesReopensAmqpsConnectionIfSendFails(
            @Mocked final Message mockMsg,
            @Mocked final IotHubEventCallback mockCallback,
            @Mocked final IotHubOutboundPacket mockPacket)
            throws IOException, ExecutionException, InterruptedException, TimeoutException {
        final Map<String, Object> context = new HashMap<>();
        new NonStrictExpectations()
        {
            {
                new AmqpsIotHubConnection(mockConfig);
                result = mockConnection;
                new IotHubOutboundPacket(mockMsg, mockCallback, context);
                result = mockPacket;
                mockPacket.getMessage();
                result = mockMsg;
                Deencapsulation.invoke(mockConnection, "getCompletionStatus");
                result = new Exception();
                result = new Boolean(true);
            }
        };

        AmqpsTransport transport = new AmqpsTransport(mockConfig);
        try
        {
            transport.open();
            transport.addMessage(mockMsg, mockCallback, context);
            transport.sendMessages();
            throw new AssertionError();
        }
        catch (Throwable e)
        {

        }
        transport.sendMessages();

        final AmqpsIotHubConnection expectedConnection = mockConnection;
        final DeviceClientConfig expectedConfig = mockConfig;
        new VerificationsInOrder()
        {
            {
                new AmqpsIotHubConnection(expectedConfig);
                expectedConnection.open();
            }
        };
    }

    // Tests_SRS_AMQPSTRANSPORT_11_028: [If the AMQPS session is closed, the function shall throw an IllegalStateException.]
    @Test(expected = IllegalStateException.class)
    public void sendMessagesFailsIfTransportNeverOpened() throws IOException
    {
        AmqpsTransport transport = new AmqpsTransport(mockConfig);
        transport.sendMessages();
    }

    // Tests_SRS_AMQPSTRANSPORT_11_028: [If the AMQPS session is closed, the function shall throw an IllegalStateException.]
    @Test(expected = IllegalStateException.class)
    public void sendMessagesFailsIfTransportAlreadyClosed() throws IOException
    {
        AmqpsTransport transport = new AmqpsTransport(mockConfig);
        transport.open();
        transport.close();
        transport.sendMessages();
    }

    //TODO: // Tests_SRS_AMQPSTRANSPORT_11_008: [The function shall invoke all callbacks on its callback queue.]

    // Tests_SRS_AMQPSTRANSPORT_11_030: [If the AMQPS session is closed, the function shall throw an IllegalStateException.]
    @Test(expected = IllegalStateException.class)
    public void invokeCallbacksFailsIfTransportNeverOpened() throws IOException
    {
        AmqpsTransport transport = new AmqpsTransport(mockConfig);
        transport.invokeCallbacks();
    }

    // Tests_SRS_AMQPSTRANSPORT_11_030: [If the AMQPS session is closed, the function shall throw an IllegalStateException.]
    @Test(expected = IllegalStateException.class)
    public void invokeCallbacksFailsIfTransportAlreadyClosed()
            throws IOException
    {
        AmqpsTransport transport = new AmqpsTransport(mockConfig);
        transport.open();
        transport.close();
        transport.invokeCallbacks();
    }

    // Tests_SRS_AMQPSTRANSPORT_11_013: [The function shall return true if the waiting list and callback list are all empty, and false otherwise.]
    @Test
    public void isEmptyReturnsFalseIfWaitingListIsNotEmpty(
            @Mocked final Message mockMsg,
            @Mocked final IotHubEventCallback mockCallback,
            @Mocked final IotHubOutboundPacket mockPacket) throws IOException
    {
        final Map<String, Object> context = new HashMap<>();

        AmqpsTransport transport = new AmqpsTransport(mockConfig);
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
            @Mocked final IotHubEventCallback mockCallback,
            @Mocked final IotHubOutboundPacket mockPacket,
            @Mocked final IotHubCallbackPacket mockCallbackPacket,
            @Mocked final IotHubStatusCode mockStatus)
            throws IOException
    {
        final Map<String, Object> context = new HashMap<>();

        AmqpsTransport transport = new AmqpsTransport(mockConfig);
        transport.open();
        transport.addMessage(mockMsg, mockCallback, context);
        transport.addMessage(mockMsg, mockCallback, context);
        transport.addMessage(mockMsg, mockCallback, context);
        transport.sendMessages();
        transport.addMessage(mockMsg, mockCallback, context);
        boolean testIsEmpty = transport.isEmpty();

        final boolean expectedIsEmpty = false;
        assertThat(testIsEmpty, is(expectedIsEmpty));
    }

    // Tests_SRS_HTTPSTRANSPORT_11_015: [The function shall return true if the waiting list, in progress list, and callback list are all empty, and false otherwise.]
    @Test
    public void isEmptyReturnsTrueIfEmpty(
            @Mocked final Message mockMsg,
            @Mocked final IotHubEventCallback mockCallback,
            @Mocked final IotHubOutboundPacket mockPacket,
            @Mocked final IotHubCallbackPacket mockCallbackPacket,
            @Mocked final IotHubStatusCode mockStatus)
            throws IOException
    {
        final Map<String, Object> context = new HashMap<>();

        AmqpsTransport transport = new AmqpsTransport(mockConfig);
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

    // Tests_SRS_AMQPSTRANSPORT_11_010: [The function shall attempt to consume a message from the IoT Hub.]
    @Test
    public void handleMessageAttemptsToReceiveMessage(
            @Mocked final MessageCallback mockCallback,
            @Mocked final Message mockMsg)
            throws IOException
    {
        new MockUp<AmqpsUtilities>(){
            @Mock
            Message protonMessageToMessage(MessageImpl msg){
                return mockMsg;
            }
        };

        final Object context = new Object();
        new NonStrictExpectations()
        {
            {
                mockConfig.getMessageCallback();
                result = mockCallback;
                mockConfig.getMessageContext();
                result = context;
            }
        };

        AmqpsTransport transport = new AmqpsTransport(mockConfig);
        transport.open();
        transport.handleMessage();

        final AmqpsIotHubConnection expectedConnection = mockConnection;
        new Verifications()
        {
            {
                expectedConnection.consumeMessage();
            }
        };
    }

    // Tests_SRS_AMQPSTRANSPORT_11_011: [If a message is found and a message callback is registered, the function shall invoke the callback on the message.]
    @Test
    public void handleMessageInvokesCallbackIfMessageReceived(
            @Mocked final MessageCallback mockCallback,
            @Mocked final Message mockMsg) throws IOException
    {
        new MockUp<AmqpsUtilities>(){
            @Mock
            Message protonMessageToMessage(MessageImpl msg){
                return mockMsg;
            }
        };

        final Object context = new Object();
        new NonStrictExpectations()
        {
            {
                mockConfig.getMessageCallback();
                result = mockCallback;
                mockConfig.getMessageContext();
                result = context;
                mockConnection.consumeMessage();
                result = mockAmqpsMessage;
            }
        };

        AmqpsTransport transport = new AmqpsTransport(mockConfig);
        transport.open();
        transport.handleMessage();

        final MessageCallback expectedCallback = mockCallback;
        final Message expectedMsg = mockMsg;
        final Object expectedContext = context;
        new Verifications()
        {
            {
                expectedCallback.execute(expectedMsg, expectedContext);
            }
        };
    }

    // Tests_SRS_AMQPSTRANSPORT_11_012: [The function shall return the message result (one of COMPLETE, ABANDON, or REJECT) to the IoT Hub.]
    @Test
    public void handleMessageSendsMessageResultIfCallbackReceived(
            @Mocked final MessageCallback mockCallback,
            @Mocked final Message mockMsg) throws IOException
    {
        new MockUp<AmqpsUtilities>(){
            @Mock
            Message protonMessageToMessage(MessageImpl msg){
                return mockMsg;
            }
        };

        final Object context = new Object();
        final IotHubMessageResult messageResult =
                IotHubMessageResult.COMPLETE;
        new NonStrictExpectations()
        {
            {
                mockConfig.getMessageCallback();
                result = mockCallback;
                mockConfig.getMessageContext();
                result = context;
                mockConnection.consumeMessage();
                result = mockAmqpsMessage;
                mockCallback.execute((Message) any, any);
                result = messageResult;
            }
        };

        AmqpsTransport transport = new AmqpsTransport(mockConfig);
        transport.open();
        transport.handleMessage();

        final AmqpsIotHubConnection expectedConnection = mockConnection;
        final IotHubMessageResult expectedMessageResult =
                messageResult;
        new Verifications()
        {
            {
                expectedConnection.sendMessageResult(
                        expectedMessageResult);
            }
        };
    }

    // Tests_SRS_AMQPSTRANSPORT_11_014: [If no AMQPS session exists with the IoT Hub, the function shall establish one.]
    @Test
    public void handleMessageCreatesAmqpsConnectionIfNoneExists(
            @Mocked final MessageCallback mockCallback,
            @Mocked final Message mockMsg)
            throws IOException
    {
        new MockUp<AmqpsUtilities>(){
            @Mock
            Message protonMessageToMessage(MessageImpl msg){
                return mockMsg;
            }
        };

        final Object context = new Object();
        new NonStrictExpectations()
        {
            {
                mockConfig.getMessageCallback();
                result = mockCallback;
                mockConfig.getMessageContext();
                result = context;
            }
        };

        AmqpsTransport transport = new AmqpsTransport(mockConfig);
        transport.open();
        transport.handleMessage();

        final DeviceClientConfig expectedConfig = mockConfig;
        new Verifications()
        {
            {
                new AmqpsIotHubConnection(expectedConfig);
            }
        };
    }

    // Tests_SRS_AMQPSTRANSPORT_11_017: [If the IoT Hub could not be reached, the function shall throw an IOException.]
    @Test(expected = IOException.class)
    public void handleMessageThrowsIOExceptionIfReceiveFails(
            @Mocked final MessageCallback mockCallback,
            @Mocked final Message mockMsg) throws IOException
    {
        new MockUp<AmqpsUtilities>(){
            @Mock
            Message protonMessageToMessage(MessageImpl msg){
                return mockMsg;
            }
        };

        final Object context = new Object();
        new NonStrictExpectations()
        {
            {
                mockConfig.getMessageCallback();
                result = mockCallback;
                mockConfig.getMessageContext();
                result = context;
                mockConnection.consumeMessage();
                result = new IOException();
            }
        };

        AmqpsTransport transport = new AmqpsTransport(mockConfig);
        transport.open();
        transport.handleMessage();
    }

    // Tests_SRS_AMQPSTRANSPORT_11_035: [If the function throws any exception or error, it shall also close the AMQPS session.]
    @Test
    public void handleMessageClosesAmqpsConnectionIfConnectionFails(
            @Mocked final MessageCallback mockCallback,
            @Mocked final Message mockMsg) throws IOException
    {
        final Object context = new Object();
        new NonStrictExpectations()
        {
            {
                mockConfig.getMessageCallback();
                result = mockCallback;
                mockConfig.getMessageContext();
                result = context;
            }
        };

        AmqpsTransport transport = new AmqpsTransport(mockConfig);
        try
        {
            transport.open();
            transport.handleMessage();
        }
        catch (Throwable e)
        {

        }

        final AmqpsIotHubConnection expectedConnection = mockConnection;
        new VerificationsInOrder()
        {
            {
                expectedConnection.close();
            }
        };
    }

    // Tests_SRS_AMQPSTRANSPORT_11_031: [If the transport had previously encountered any exception or error while open, it shall reopen a new AMQPS session with the IoT Hub given in the configuration.]
    @Test
    public void handleMessageReopensAmqpsConnectionIfConnectionFails(
            @Mocked final MessageCallback mockCallback,
            @Mocked final Message mockMsg) throws IOException, ExecutionException, InterruptedException, TimeoutException {
        new MockUp<AmqpsUtilities>(){
            @Mock
            Message protonMessageToMessage(MessageImpl msg){
                return mockMsg;
            }
        };

        final Object context = new Object();
        new NonStrictExpectations()
        {
            {
                mockConfig.getMessageCallback();
                result = mockCallback;
                mockConfig.getMessageContext();
                result = context;
                mockConnection.consumeMessage();
                result = new NullPointerException();
                result = null;
            }
        };

        AmqpsTransport transport = new AmqpsTransport(mockConfig);
        try
        {
            transport.open();
            transport.handleMessage();
        }
        catch (Throwable e)
        {

        }
        transport.handleMessage();

        final AmqpsIotHubConnection expectedConnection = mockConnection;
        final DeviceClientConfig expectedConfig = mockConfig;
        new VerificationsInOrder()
        {
            {
                expectedConnection.consumeMessage();
                new AmqpsIotHubConnection(expectedConfig);
                expectedConnection.open();
            }
        };
    }

    // Tests_SRS_AMQPSTRANSPORT_11_033: [If the AMQPS session is closed, the function shall throw an IllegalStateException.]
    @Test(expected = IllegalStateException.class)
    public void handleMessageFailsIfTransportNeverOpened()
            throws IOException
    {
        AmqpsTransport transport = new AmqpsTransport(mockConfig);
        transport.handleMessage();
    }

    // Tests_SRS_AMQPSTRANSPORT_11_033: [If the AMQPS session is closed, the function shall throw an IllegalStateException.]
    @Test(expected = IllegalStateException.class)
    public void handleMessageFailsIfTransportAlreadyClosed()
            throws IOException
    {
        AmqpsTransport transport = new AmqpsTransport(mockConfig);
        transport.open();
        transport.close();
        transport.handleMessage();
    }
}
