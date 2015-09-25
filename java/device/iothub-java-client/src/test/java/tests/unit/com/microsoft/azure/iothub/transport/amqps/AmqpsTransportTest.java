// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

package tests.unit.com.microsoft.azure.iothub.transport.amqps;

import static org.junit.Assert.assertThat;
import static org.hamcrest.CoreMatchers.is;

import com.microsoft.azure.iothub.*;
import com.microsoft.azure.iothub.MessageCallback;
import com.microsoft.azure.iothub.transport.amqps.AmqpsIotHubSession;
import com.microsoft.azure.iothub.transport.amqps.AmqpsTransport;
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
import java.util.HashMap;
import java.util.Map;
import java.util.Queue;

/** Unit tests for AmqpsTransport. */
public class AmqpsTransportTest
{
    @Mocked
    DeviceClientConfig mockConfig;

    @Mocked
    AmqpsIotHubSession mockSession;

    // Tests_SRS_AMQPSTRANSPORT_11_019: [The function shall open an AMQPS session with the IoT Hub given in the configuration.]
    @Test
    public void openOpensAmqpsSession() throws IOException
    {
        new NonStrictExpectations()
        {
            {
                new AmqpsIotHubSession(mockConfig);
                result = mockSession;
            }
        };

        AmqpsTransport transport = new AmqpsTransport(mockConfig);
        transport.open();

        final AmqpsIotHubSession expectedSession = mockSession;
        new Verifications()
        {
            {
                expectedSession.open();
            }
        };
    }

    // Tests_SRS_AMQPSTRANSPORT_11_020: [If an AMQPS session is already open, the function shall do nothing.]
    @Test
    public void openDoesNothingIfAlreadyOpened() throws IOException
    {
        AmqpsTransport transport = new AmqpsTransport(mockConfig);
        transport.open();
        transport.open();

        final AmqpsIotHubSession expectedSession = mockSession;
        new Verifications()
        {
            {
                expectedSession.open();
                times = 1;
            }
        };
    }

    // Tests_SRS_AMQPSTRANSPORT_11_021: [The function shall close an AMQPS session with the IoT Hub given in the configuration.]
    @Test
    public void closeClosesAmqpsSession() throws IOException
    {
        AmqpsTransport transport = new AmqpsTransport(mockConfig);
        transport.open();
        transport.close();

        final AmqpsIotHubSession expectedSession = mockSession;
        new Verifications()
        {
            {
                expectedSession.close();
            }
        };
    }

    // Tests_SRS_AMQPSTRANSPORT_11_023: [If the AMQPS session is closed, the function shall do nothing.]
    @Test
    public void closeDoesNothingIfSessionNeverOpened() throws IOException
    {
        AmqpsTransport transport = new AmqpsTransport(mockConfig);
        transport.close();

        final AmqpsIotHubSession expectedSession = mockSession;
        new Verifications()
        {
            {
                expectedSession.close();
                times = 0;
            }
        };
    }

    // Tests_SRS_AMQPSTRANSPORT_11_023: [If the AMQPS session is closed, the function shall do nothing.]
    @Test
    public void closeDoesNothingIfSessionAlreadyClosed() throws IOException
    {
        AmqpsTransport transport = new AmqpsTransport(mockConfig);
        transport.open();
        transport.close();
        transport.close();

        final AmqpsIotHubSession expectedSession = mockSession;
        new Verifications()
        {
            {
                expectedSession.close();
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
    @Test
    public void sendMessagesSendsAllMessages(
            @Mocked final Message mockMsg,
            @Mocked final IotHubEventCallback mockCallback,
            @Mocked final IotHubOutboundPacket mockPacket,
            @Mocked final AmqpsIotHubSession mockSession)
            throws IOException
    {
        final Map<String, Object> context = new HashMap<>();
        new NonStrictExpectations()
        {
            {
                new AmqpsIotHubSession(mockConfig);
                result = mockSession;
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

        final AmqpsIotHubSession expectedSession = mockSession;
        new Verifications()
        {
            {
                expectedSession.sendEvent(mockMsg);
                times = 2;
            }
        };
    }

    // Tests_SRS_AMQPSTRANSPORT_11_006: [For each message being sent, the function shall send the message and add the IoT Hub status code along with the callback and context to the callback list.]
    @Test
    public <T extends Queue> void sendMessagesAddsToCallbackQueue(
            @Mocked final Message mockMsg,
            @Mocked final IotHubEventCallback mockCallback,
            @Mocked final IotHubOutboundPacket mockPacket,
            @Mocked final IotHubCallbackPacket mockCallbackPacket)
            throws IOException
    {
        final Queue mockQueue = new MockUp<T>()
        {

        }.getMockInstance();
        final Map<String, Object> context = new HashMap<>();
        new NonStrictExpectations()
        {
            {
                new AmqpsIotHubSession(mockConfig);
                result = mockSession;
                new IotHubOutboundPacket(mockMsg, mockCallback, context);
                result = mockPacket;
                mockPacket.getCallback();
                result = mockCallback;
                mockPacket.getContext();
                result = context;
                mockSession.sendEvent((Message) any);
                returns(IotHubStatusCode.OK_EMPTY, IotHubStatusCode.ERROR);
                new IotHubCallbackPacket(IotHubStatusCode.OK_EMPTY,
                        mockCallback, context);
                result = mockCallbackPacket;
                new IotHubCallbackPacket(IotHubStatusCode.ERROR, mockCallback,
                        context);
                result = mockCallbackPacket;
            }
        };

        AmqpsTransport transport = new AmqpsTransport(mockConfig);
        transport.open();
        transport.addMessage(mockMsg, mockCallback, context);
        transport.addMessage(mockMsg, mockCallback, context);
        transport.sendMessages();

        new VerificationsInOrder()
        {
            {
                new IotHubCallbackPacket(IotHubStatusCode.OK_EMPTY,
                        mockCallback, context);
                mockQueue.add(mockCallbackPacket);
                new IotHubCallbackPacket(IotHubStatusCode.ERROR, mockCallback,
                        context);
                mockQueue.add(mockCallbackPacket);
            }
        };
    }

    // Tests_SRS_AMQPSTRANSPORT_11_007: [If the IoT Hub could not be reached, the message shall be buffered to be sent again next time.]
    @Test
    public void sendMessagesBuffersFailedMessages(
            @Mocked final Message mockMsg,
            @Mocked final IotHubEventCallback mockCallback,
            @Mocked final IotHubOutboundPacket mockPacket)
            throws IOException
    {
        final Map<String, Object> context = new HashMap<>();
        new NonStrictExpectations()
        {
            {
                mockSession.sendEvent((Message) any);
                result = new IOException(anyString);
                result = IotHubStatusCode.OK_EMPTY;
            }
        };

        AmqpsTransport transport = new AmqpsTransport(mockConfig);
        transport.open();
        transport.addMessage(mockMsg, mockCallback, context);
        try
        {
            transport.sendMessages();
            throw new AssertionFailedError();
        }
        catch (IOException e)
        {

        }
        transport.sendMessages();

        final AmqpsIotHubSession expectedSession = mockSession;
        new Verifications()
        {
            {
                expectedSession.sendEvent(mockMsg);
                times = 2;
            }
        };
    }

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
                new AmqpsIotHubSession(mockConfig);
                result = mockSession;
                new IotHubOutboundPacket(mockMsg, mockCallback, context);
                result = mockPacket;
                mockPacket.getMessage();
                result = mockMsg;
                mockSession.sendEvent(mockMsg);
                result = new IOException(anyString);
            }
        };

        AmqpsTransport transport = new AmqpsTransport(mockConfig);
        transport.open();
        transport.addMessage(mockMsg, mockCallback, context);
        transport.sendMessages();
    }

    // Tests_SRS_AMQPSTRANSPORT_11_034: [If the function throws any exception or error, it shall also close the AMQPS session.]
    @Test
    public void sendMessagesClosesAmqpsSessionIfSendFails(
            @Mocked final Message mockMsg,
            @Mocked final IotHubEventCallback mockCallback,
            @Mocked final IotHubOutboundPacket mockPacket)
            throws IOException
    {
        final Map<String, Object> context = new HashMap<>();
        new NonStrictExpectations()
        {
            {
                new AmqpsIotHubSession(mockConfig);
                result = mockSession;
                new IotHubOutboundPacket(mockMsg, mockCallback, context);
                result = mockPacket;
                mockPacket.getMessage();
                result = mockMsg;
                mockSession.sendEvent(mockMsg);
                result = new OutOfMemoryError();
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

        final AmqpsIotHubSession expectedSession = mockSession;
        new Verifications()
        {
            {
                expectedSession.close();
            }
        };
    }

    // Tests_SRS_AMQPSTRANSPORT_11_026: [If the transport had previously encountered any exception or error while open, it shall reopen a new AMQPS session with the IoT Hub given in the configuration.]
    @Test
    public void sendMessagesReopensAmqpsSessionIfSendFails(
            @Mocked final Message mockMsg,
            @Mocked final IotHubEventCallback mockCallback,
            @Mocked final IotHubOutboundPacket mockPacket)
            throws IOException
    {
        final Map<String, Object> context = new HashMap<>();
        new NonStrictExpectations()
        {
            {
                new AmqpsIotHubSession(mockConfig);
                result = mockSession;
                new IotHubOutboundPacket(mockMsg, mockCallback, context);
                result = mockPacket;
                mockPacket.getMessage();
                result = mockMsg;
                mockSession.sendEvent(mockMsg);
                result = new OutOfMemoryError();
                result = IotHubStatusCode.OK_EMPTY;
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

        final AmqpsIotHubSession expectedSession = mockSession;
        final DeviceClientConfig expectedConfig = mockConfig;
        new VerificationsInOrder()
        {
            {
                expectedSession.sendEvent((Message) any);
                new AmqpsIotHubSession(expectedConfig);
                expectedSession.open();
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

    // Tests_SRS_AMQPSTRANSPORT_11_008: [The function shall invoke all callbacks on its callback queue.]
    @Test
    public void invokeCallbacksInvokesAllCallbacks(
            @Mocked final Message mockMsg,
            @Mocked final IotHubEventCallback mockCallback,
            @Mocked final IotHubOutboundPacket mockPacket,
            @Mocked final IotHubCallbackPacket mockCallbackPacket)
            throws IOException
    {
        final Map<String, Object> context = new HashMap<>();
        new NonStrictExpectations()
        {
            {
                new AmqpsIotHubSession(mockConfig);
                result = mockSession;
                mockCallbackPacket.getStatus();
                returns(IotHubStatusCode.OK_EMPTY, IotHubStatusCode.ERROR);
                mockCallbackPacket.getCallback();
                result = mockCallback;
                mockCallbackPacket.getContext();
                result = context;
            }
        };

        AmqpsTransport transport = new AmqpsTransport(mockConfig);
        transport.open();
        transport.addMessage(mockMsg, mockCallback, context);
        transport.addMessage(mockMsg, mockCallback, context);
        transport.sendMessages();
        transport.invokeCallbacks();

        final IotHubEventCallback expectedCallback = mockCallback;
        final Map<String, Object> expectedContext = context;
        new VerificationsInOrder()
        {
            {
                expectedCallback.execute(
                        IotHubStatusCode.OK_EMPTY, expectedContext);
                expectedCallback.execute(
                        IotHubStatusCode.ERROR, expectedContext);
            }
        };
    }

    // Tests_SRS_AMQPSTRANSPORT_11_009: [If an exception is thrown during the callback, the function shall drop the callback from the queue.]
    @Test
    public void invokeCallbacksDropsFailedCallback(
            @Mocked final Message mockMsg,
            @Mocked final IotHubEventCallback mockCallback,
            @Mocked final IotHubOutboundPacket mockPacket,
            @Mocked final IotHubCallbackPacket mockCallbackPacket)
            throws IOException
    {
        final Map<String, Object> context = new HashMap<>();
        new NonStrictExpectations()
        {
            {
                new AmqpsIotHubSession(mockConfig);
                result = mockSession;
                mockCallbackPacket.getStatus();
                result = IotHubStatusCode.OK_EMPTY;
                mockCallbackPacket.getCallback();
                result = mockCallback;
                mockCallbackPacket.getContext();
                result = context;
                mockCallback.execute(IotHubStatusCode.OK_EMPTY, context);
                result = new IllegalStateException();
                result = null;
            }
        };

        AmqpsTransport transport = new AmqpsTransport(mockConfig);
        transport.open();
        transport.addMessage(mockMsg, mockCallback, context);
        transport.sendMessages();
        try
        {
            transport.invokeCallbacks();
            throw new AssertionFailedError();
        }
        catch (IllegalStateException e)
        {
            transport.invokeCallbacks();
        }

        final IotHubEventCallback expectedCallback = mockCallback;
        final Map<String, Object> expectedContext = context;
        new VerificationsInOrder()
        {
            {
                expectedCallback.execute(
                        IotHubStatusCode.OK_EMPTY, expectedContext);
            }
        };
    }

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
            @Mocked final MessageCallback mockCallback)
            throws IOException
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
        transport.open();
        transport.handleMessage();

        final AmqpsIotHubSession expectedSession = mockSession;
        new Verifications()
        {
            {
                expectedSession.receiveMessage();
            }
        };
    }

    // Tests_SRS_AMQPSTRANSPORT_11_011: [If a message is found and a message callback is registered, the function shall invoke the callback on the message.]
    @Test
    public void handleMessageInvokesCallbackIfMessageReceived(
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
                mockSession.receiveMessage();
                result = mockMsg;
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
                mockSession.receiveMessage();
                result = mockMsg;
                mockCallback.execute((Message) any, any);
                result = messageResult;
            }
        };

        AmqpsTransport transport = new AmqpsTransport(mockConfig);
        transport.open();
        transport.handleMessage();

        final AmqpsIotHubSession expectedSession = mockSession;
        final IotHubMessageResult expectedMessageResult =
                messageResult;
        new Verifications()
        {
            {
                expectedSession.sendMessageResult(
                        expectedMessageResult);
            }
        };
    }

    // Tests_SRS_AMQPSTRANSPORT_11_014: [If no AMQPS session exists with the IoT Hub, the function shall establish one.]
    @Test
    public void handleMessageCreatesAmqpsSessionIfNoneExists(
            @Mocked final MessageCallback mockCallback)
            throws IOException
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
        transport.open();
        transport.handleMessage();

        final DeviceClientConfig expectedConfig = mockConfig;
        new Verifications()
        {
            {
                new AmqpsIotHubSession(expectedConfig);
            }
        };
    }

    // Tests_SRS_AMQPSTRANSPORT_11_017: [If the IoT Hub could not be reached, the function shall throw an IOException.]
    @Test(expected = IOException.class)
    public void handleMessageThrowsIOExceptionIfReceiveFails(
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
                mockSession.receiveMessage();
                result = new IOException();
            }
        };

        AmqpsTransport transport = new AmqpsTransport(mockConfig);
        transport.open();
        transport.handleMessage();
    }

    // Tests_SRS_AMQPSTRANSPORT_11_035: [If the function throws any exception or error, it shall also close the AMQPS session.]
    @Test
    public void handleMessageClosesAmqpsSessionIfSessionFails(
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
                mockSession.receiveMessage();
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

        final AmqpsIotHubSession expectedSession = mockSession;
        new VerificationsInOrder()
        {
            {
                expectedSession.close();
            }
        };
    }

    // Tests_SRS_AMQPSTRANSPORT_11_031: [If the transport had previously encountered any exception or error while open, it shall reopen a new AMQPS session with the IoT Hub given in the configuration.]
    @Test
    public void handleMessageReopensAmqpsSessionIfSessionFails(
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
                mockSession.receiveMessage();
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

        final AmqpsIotHubSession expectedSession = mockSession;
        final DeviceClientConfig expectedConfig = mockConfig;
        new VerificationsInOrder()
        {
            {
                expectedSession.receiveMessage();
                new AmqpsIotHubSession(expectedConfig);
                expectedSession.open();
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
