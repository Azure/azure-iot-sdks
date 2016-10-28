// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

package tests.unit.com.microsoft.azure.iothub.transport.amqps;

import com.microsoft.azure.iothub.*;
import com.microsoft.azure.iothub.transport.IotHubCallbackPacket;
import com.microsoft.azure.iothub.transport.IotHubOutboundPacket;
import com.microsoft.azure.iothub.transport.State;
import com.microsoft.azure.iothub.transport.amqps.AmqpsIotHubConnection;
import com.microsoft.azure.iothub.transport.amqps.AmqpsMessage;
import com.microsoft.azure.iothub.transport.amqps.AmqpsTransport;
import mockit.*;
import org.apache.qpid.proton.Proton;
import org.apache.qpid.proton.amqp.messaging.ApplicationProperties;
import org.apache.qpid.proton.message.impl.MessageImpl;
import org.junit.Assert;
import org.junit.Test;

import java.io.IOException;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.Map;
import java.util.Queue;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.LinkedBlockingDeque;
import java.util.concurrent.LinkedBlockingQueue;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertTrue;

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
    MessageImpl mockProtonMessage;

    @Mocked
    IotHubCallbackPacket mockIotHubCallbackPacket;

    @Mocked
    IotHubEventCallback mockIotHubEventCallback;

    @Mocked
    MessageCallback mockMessageCallback;

    // Tests_SRS_AMQPSTRANSPORT_15_001: [The constructor shall save the input parameters into instance variables.]
    @Test
    public void constructorSavesInputParameters()
    {
        DeviceClientConfig expectedClientConfig = mockConfig;
        Boolean expectedUseWebSockets = false;
        AmqpsTransport transport = new AmqpsTransport(expectedClientConfig, expectedUseWebSockets);


        DeviceClientConfig actualClientConfig = Deencapsulation.getField(transport, "config");
        Boolean actualUseWebSockets = Deencapsulation.getField(transport, "useWebSockets");

        assertEquals(expectedClientConfig, actualClientConfig);
        assertEquals(expectedUseWebSockets, actualUseWebSockets);
    }

    // Tests_SRS_AMQPSTRANSPORT_15_002: [The constructor shall set the transport state to CLOSED.]
    @Test
    public void constructorSetsStateToClosed()
    {
        AmqpsTransport transport = new AmqpsTransport(mockConfig, false);

        State state = Deencapsulation.getField(transport, "state");

        assertEquals(State.CLOSED, state);
    }

    // Tests_SRS_AMQPSTRANSPORT_15_003: [If an AMQPS connection is already open, the function shall do nothing.]
    @Test
    public void openDoesNothingIfAlreadyOpened() throws IOException, InterruptedException
    {
        AmqpsTransport transport = new AmqpsTransport(mockConfig, false);
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

    // Tests_SRS_AMQPSTRANSPORT_15_004: [The function shall open an AMQPS connection with the IoT Hub given in the configuration.]
    @Test
    public void openOpensAmqpsConnection() throws IOException, InterruptedException
    {
        new NonStrictExpectations()
        {
            {
                new AmqpsIotHubConnection(mockConfig, false);
                result = mockConnection;
            }
        };

        AmqpsTransport transport = new AmqpsTransport(mockConfig, false);
        transport.open();

        final AmqpsIotHubConnection expectedConnection = mockConnection;
        new Verifications()
        {
            {
                expectedConnection.open();
            }
        };
    }


    // Tests_SRS_AMQPSTRANSPORT_15_005: [The function shall add the transport to the list of listeners subscribed to the connection events.]
    @Test
    public void openAddsTransportToConnectionListenersList() throws IOException
    {
        new NonStrictExpectations()
        {
            {
                new AmqpsIotHubConnection(mockConfig, false);
                result = mockConnection;
            }
        };

        final AmqpsTransport transport = new AmqpsTransport(mockConfig, false);
        transport.open();

        new Verifications()
        {
            {
                mockConnection.addListener(transport);
                times = 1;
            }
        };
    }

    // Tests_SRS_AMQPSTRANSPORT_15_006: [If the connection was opened successfully, the transport state shall be set to OPEN.]
    @Test
    public void openSetsStateToOpenIfSuccessful() throws IOException
    {
        new NonStrictExpectations()
        {
            {
                new AmqpsIotHubConnection(mockConfig, false);
                result = mockConnection;
            }
        };

        final AmqpsTransport transport = new AmqpsTransport(mockConfig, false);
        transport.open();

        State state = Deencapsulation.getField(transport, "state");
        assertEquals(State.OPEN, state);
    }

    // Tests_SRS_AMQPSTRANSPORT_15_007: [If the AMQPS connection is closed, the function shall do nothing.]
    @Test
    public void closeDoesNothingIfConnectionAlreadyClosed() throws IOException, InterruptedException
    {
        AmqpsTransport transport = new AmqpsTransport(mockConfig, false);
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

    // Tests_SRS_AMQPSTRANSPORT_15_008: [The function shall close an AMQPS connection with the IoT Hub given in the configuration.]
    @Test
    public void closeClosesAmqpsConnection() throws IOException, InterruptedException
    {
        AmqpsTransport transport = new AmqpsTransport(mockConfig, false);
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

    // Tests_SRS_AMQPSTRANSPORT_15_009: [The function shall set the transport state to CLOSED.]
    @Test
    public void closeSetsStateToClosed() throws IOException
    {
        AmqpsTransport transport = new AmqpsTransport(mockConfig, false);
        transport.open();
        transport.close();

        State actualState = Deencapsulation.getField(transport, "state");
        Assert.assertEquals(State.CLOSED, actualState);
    }

    // Tests_SRS_AMQPSTRANSPORT_15_010: [If the AMQPS session is closed, the function shall throw an IllegalStateException.]
    @Test(expected = IllegalStateException.class)
    public void addMessageFailsIfTransportNotOpened(
            @Mocked final Message mockMsg,
            @Mocked final IotHubEventCallback mockCallback)
            throws IOException
    {
        final Map<String, Object> context = new HashMap<>();

        AmqpsTransport transport = new AmqpsTransport(mockConfig, false);
        transport.addMessage(mockMsg, mockCallback, context);
    }

    // Tests_SRS_AMQPSTRANSPORT_15_010: [If the AMQPS session is closed, the function shall throw an IllegalStateException.]
    @Test(expected = IllegalStateException.class)
    public void addMessageFailsIfTransportAlreadyClosed(
            @Mocked final Message mockMsg,
            @Mocked final IotHubEventCallback mockCallback)
            throws IOException
    {
        final Map<String, Object> context = new HashMap<>();

        AmqpsTransport transport = new AmqpsTransport(mockConfig, false);
        transport.open();
        transport.close();
        transport.addMessage(mockMsg, mockCallback, context);
    }

    // Tests_SRS_AMQPSTRANSPORT_15_011: [The function shall add a packet containing the message, callback,
    // and callback context to the queue of messages waiting to be sent.]
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

        AmqpsTransport transport = new AmqpsTransport(mockConfig, false);
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

    // Tests_SRS_AMQPSTRANSPORT_15_012: [If the AMQPS session is closed, the function shall throw an IllegalStateException.]
    @Test(expected = IllegalStateException.class)
    public void sendMessagesFailsIfTransportNeverOpened() throws IOException
    {
        AmqpsTransport transport = new AmqpsTransport(mockConfig, false);
        transport.sendMessages();
    }

    // Tests_SRS_AMQPSTRANSPORT_15_012: [If the AMQPS session is closed, the function shall throw an IllegalStateException.]
    @Test(expected = IllegalStateException.class)
    public void sendMessagesFailsIfTransportAlreadyClosed() throws IOException
    {
        AmqpsTransport transport = new AmqpsTransport(mockConfig, false);
        transport.open();
        transport.close();
        transport.sendMessages();
    }


    // Tests_SRS_AMQPSTRANSPORT_15_013: [If there are no messages in the waiting list, the function shall return.]
    @Test
    public void sendMessagesReturnsIfNoMessagesAreWaiting(@Mocked final IotHubOutboundPacket mockPacket)
            throws IOException
    {
        new NonStrictExpectations()
        {
            {
                new AmqpsIotHubConnection(mockConfig, false);
                result = mockConnection;
            }
        };

        AmqpsTransport transport = new AmqpsTransport(mockConfig, false);
        transport.open();
        transport.sendMessages();

        assertTrue(transport.isEmpty());
        new VerificationsInOrder()
        {
            {
                mockPacket.getMessage();
                times = 0;
            }
        };
    }


    // Tests_SRS_AMQPSTRANSPORT_15_014: [The function shall attempt to send every message on its waiting list, one at a time.]
    // Tests_SRS_AMQPSTRANSPORT_15_036: [The function shall create a new Proton message from the IoTHub message.]
    // Tests_SRS_AMQPSTRANSPORT_15_037: [The function shall attempt to send the Proton message to IoTHub using the underlying AMQPS connection.]
    @Test
    public void sendMessagesSendsAllMessages(
            @Mocked final Message mockMsg,
            @Mocked final IotHubEventCallback mockCallback,
            @Mocked final IotHubOutboundPacket mockPacket)
            throws IOException
    {
        final Map<String, Object> context = new HashMap<>();
        final byte[] messageBytes = new byte[] {1, 2};
        new NonStrictExpectations()
        {
            {
                new AmqpsIotHubConnection(mockConfig, false);
                result = mockConnection;
                new IotHubOutboundPacket(mockMsg, mockCallback, context);
                result = mockPacket;
                mockPacket.getMessage();
                result = mockMsg;
                mockMsg.getBytes();
                result = messageBytes;
            }
        };

        AmqpsTransport transport = new AmqpsTransport(mockConfig, false);
        transport.open();
        transport.addMessage(mockMsg, mockCallback, context);
        transport.addMessage(mockMsg, mockCallback, context);
        transport.sendMessages();

        new Verifications()
        {
            {
                new IotHubOutboundPacket(mockMsg, mockCallback, context);
                times = 2;
                mockPacket.getMessage();
                times = 2;
                mockConnection.sendMessage((org.apache.qpid.proton.message.Message) any);
                times = 2;
            }
        };
    }

    // Tests_SRS_AMQPSTRANSPORT_15_015: [The function shall skip messages with null or empty body.]
    @Test
    public void sendMessagesSkipsMessagesWithNullBody(
            @Mocked final Message mockMsg,
            @Mocked final IotHubEventCallback mockCallback,
            @Mocked final IotHubOutboundPacket mockPacket)
            throws IOException
    {
        final Map<String, Object> context = new HashMap<>();
        new NonStrictExpectations()
        {
            {
                new AmqpsIotHubConnection(mockConfig, false);
                result = mockConnection;
                new IotHubOutboundPacket(mockMsg, mockCallback, context);
                result = mockPacket;
                mockPacket.getMessage();
                result = null;
            }
        };

        AmqpsTransport transport = new AmqpsTransport(mockConfig, false);
        transport.open();
        transport.addMessage(mockMsg, mockCallback, context);
        transport.addMessage(mockMsg, mockCallback, context);
        transport.sendMessages();

        new Verifications()
        {
            {
                mockConnection.sendMessage((org.apache.qpid.proton.message.Message) any);
                times = 0;
            }
        };
    }

    // Tests_SRS_AMQPSTRANSPORT_15_038: [The function shall add all user properties to the application properties of the Proton message.]
    @Test
    public void sendMessagesAddsUserPropertiesToProtonApplicationProperties(
            @Mocked final Message mockMsg,
            @Mocked final IotHubEventCallback mockCallback,
            @Mocked final IotHubOutboundPacket mockPacket)
            throws IOException
    {
        final Map<String, Object> context = new HashMap<>();
        final byte[] messageBytes = new byte[] {1, 2};
        final MessageProperty[] iotHubMessageProperties = new MessageProperty[]
        {
            new MessageProperty("key1", "value1"),
            new MessageProperty("key2", "value2")
        };

        final Map<String, String> userProperties = new HashMap<>(2);
        userProperties.put(iotHubMessageProperties[0].getName(), iotHubMessageProperties[0].getValue());
        userProperties.put(iotHubMessageProperties[1].getName(), iotHubMessageProperties[1].getValue());

        new NonStrictExpectations()
        {
            {
                new AmqpsIotHubConnection(mockConfig, false);
                result = mockConnection;
                new IotHubOutboundPacket(mockMsg, mockCallback, context);
                result = mockPacket;
                mockPacket.getMessage();
                result = mockMsg;
                mockMsg.getBytes();
                result = messageBytes;
                new MessageImpl();
                result = mockProtonMessage;
                mockMsg.getProperties();
                result = iotHubMessageProperties;
                mockConnection.sendMessage(mockProtonMessage);
                result = 1;
                new ApplicationProperties(userProperties);
            }
        };

        AmqpsTransport transport = new AmqpsTransport(mockConfig, false);
        transport.open();
        transport.addMessage(mockMsg, mockCallback, context);
        transport.sendMessages();


        new Verifications()
        {
            {
                new IotHubOutboundPacket(mockMsg, mockCallback, context);
                times = 1;
                mockPacket.getMessage();
                times = 1;
                mockConnection.sendMessage(mockProtonMessage);
                times = 1;
                new ApplicationProperties(userProperties);
                times = 1;
                mockProtonMessage.setApplicationProperties((ApplicationProperties) any);
                times = 1;
            }
        };
    }

    // Tests_SRS_AMQPSTRANSPORT_15_015: [The function shall skip messages with null or empty body.]
    @Test
    public void sendMessagesSkipsMessagesWithEmptyBody(
            @Mocked final Message mockMsg,
            @Mocked final IotHubEventCallback mockCallback,
            @Mocked final IotHubOutboundPacket mockPacket)
            throws IOException
    {
        final Map<String, Object> context = new HashMap<>();
        new NonStrictExpectations()
        {
            {
                new AmqpsIotHubConnection(mockConfig, false);
                result = mockConnection;
                new IotHubOutboundPacket(mockMsg, mockCallback, context);
                result = mockPacket;
                mockPacket.getMessage();
                result = mockMsg;
                mockMsg.getBytes();
                result = new byte[0];
            }
        };

        AmqpsTransport transport = new AmqpsTransport(mockConfig, false);
        transport.open();
        transport.addMessage(mockMsg, mockCallback, context);
        transport.addMessage(mockMsg, mockCallback, context);
        transport.sendMessages();

        new Verifications()
        {
            {
                mockConnection.sendMessage((org.apache.qpid.proton.message.Message) any);
                times = 0;
            }
        };
    }

    // Tests_SRS_AMQPSTRANSPORT_15_016: [If the sent message hash is valid, it shall be added to the in progress map.]
    @Test
    public void sendMessagesAddsSentMessagesToInProgressMap(
            @Mocked final Message mockMsg,
            @Mocked final IotHubEventCallback mockCallback,
            @Mocked final IotHubOutboundPacket mockPacket)
            throws IOException
    {
        final Map<String, Object> context = new HashMap<>();
        final byte[] messageBytes = new byte[] {1, 2};
        new NonStrictExpectations()
        {
            {
                new AmqpsIotHubConnection(mockConfig, false);
                result = mockConnection;
                new IotHubOutboundPacket(mockMsg, mockCallback, context);
                result = mockPacket;
                mockPacket.getMessage();
                result = mockMsg;
                mockMsg.getBytes();
                result = messageBytes;
                mockConnection.sendMessage((org.apache.qpid.proton.message.Message) any);
                returns (1, 2);
            }
        };

        AmqpsTransport transport = new AmqpsTransport(mockConfig, false);
        transport.open();
        transport.addMessage(mockMsg, mockCallback, context);
        transport.addMessage(mockMsg, mockCallback, context);
        transport.sendMessages();

        Map<Integer, IotHubOutboundPacket> inProgressMessages = Deencapsulation.getField(transport, "inProgressMessages");
        Assert.assertEquals(2, inProgressMessages.size());

        new Verifications()
        {
            {
                new IotHubOutboundPacket(mockMsg, mockCallback, context);
                times = 2;
                mockPacket.getMessage();
                times = 2;
                mockConnection.sendMessage((org.apache.qpid.proton.message.Message) any);
                times = 2;
            }
        };
    }

    // Tests_SRS_AMQPSTRANSPORT_15_017: [If the sent message hash is not valid, it shall be buffered to be sent in a subsequent attempt.]
    @Test
    public void sendMessagesAddsNotSentMessagesToInProgressMap(
            @Mocked final Message mockMsg,
            @Mocked final IotHubEventCallback mockCallback,
            @Mocked final IotHubOutboundPacket mockPacket)
            throws IOException
    {
        final Map<String, Object> context = new HashMap<>();
        final byte[] messageBytes = new byte[] {1, 2};
        new NonStrictExpectations()
        {
            {
                new AmqpsIotHubConnection(mockConfig, false);
                result = mockConnection;
                new IotHubOutboundPacket(mockMsg, mockCallback, context);
                result = mockPacket;
                mockPacket.getMessage();
                result = mockMsg;
                mockMsg.getBytes();
                result = messageBytes;
                mockConnection.sendMessage((org.apache.qpid.proton.message.Message) any);
                returns (1, -1);
            }
        };

        AmqpsTransport transport = new AmqpsTransport(mockConfig, false);
        transport.open();
        transport.addMessage(mockMsg, mockCallback, context);
        transport.addMessage(mockMsg, mockCallback, context);
        transport.sendMessages();

        Map<Integer, IotHubOutboundPacket> inProgressMessages = Deencapsulation.getField(transport, "inProgressMessages");
        Assert.assertEquals(1, inProgressMessages.size());

        Queue<IotHubOutboundPacket> waitingMessages = Deencapsulation.getField(transport, "waitingMessages");
        Assert.assertEquals(1, waitingMessages.size());

        new Verifications()
        {
            {
                new IotHubOutboundPacket(mockMsg, mockCallback, context);
                times = 2;
                mockPacket.getMessage();
                times = 2;
                mockConnection.sendMessage((org.apache.qpid.proton.message.Message) any);
                times = 2;
            }
        };
    }

    // Tests_SRS_AMQPSTRANSPORT_15_039: [If the message is expired, the function shall create a callback
    // with the MESSAGE_EXPIRED status and add it to the callback list.]
    @Test
    public void sendMessagesAddsExpiredMessagesToCallbackListWithCorrectCode(
            @Mocked final Message mockMsg,
            @Mocked final IotHubEventCallback mockCallback,
            @Mocked final IotHubOutboundPacket mockPacket)
            throws IOException
    {
        final Map<String, Object> context = new HashMap<>();
        final byte[] messageBytes = new byte[] {1, 2};
        new NonStrictExpectations()
        {
            {
                new AmqpsIotHubConnection(mockConfig, false);
                result = mockConnection;
                new IotHubOutboundPacket(mockMsg, mockCallback, context);
                result = mockPacket;
                mockPacket.getMessage();
                result = mockMsg;
                mockMsg.getBytes();
                result = messageBytes;
                mockMsg.isExpired();
                returns (true, false);
                mockConnection.sendMessage((org.apache.qpid.proton.message.Message) any);
                result = 1;
            }
        };

        AmqpsTransport transport = new AmqpsTransport(mockConfig, false);
        transport.open();
        transport.addMessage(mockMsg, mockCallback, context);
        transport.addMessage(mockMsg, mockCallback, context);
        transport.sendMessages();

        Map<Integer, IotHubOutboundPacket> inProgressMessages = Deencapsulation.getField(transport, "inProgressMessages");
        Assert.assertEquals(1, inProgressMessages.size());

        Queue<IotHubOutboundPacket> waitingMessages = Deencapsulation.getField(transport, "waitingMessages");
        Assert.assertEquals(0, waitingMessages.size());

        Queue<IotHubCallbackPacket> callbackList = Deencapsulation.getField(transport, "callbackList");
        Assert.assertEquals(1, callbackList.size());

        new Verifications()
        {
            {
                new IotHubOutboundPacket(mockMsg, mockCallback, context);
                times = 2;
                mockPacket.getMessage();
                times = 2;
                mockConnection.sendMessage((org.apache.qpid.proton.message.Message) any);
                times = 1;
                new IotHubCallbackPacket(IotHubStatusCode.MESSAGE_EXPIRED, (IotHubEventCallback) any, any);
                times = 1;
            }
        };
    }

    // Tests_SRS_AMQPSTRANSPORT_15_019: [If the transport closed, the function shall throw an IllegalStateException.]
    @Test(expected = IllegalStateException.class)
    public void invokeCallbacksFailsIfTransportNotOpen()
            throws IOException
    {
        AmqpsTransport transport = new AmqpsTransport(mockConfig, false);
        transport.invokeCallbacks();
    }

    // Tests_SRS_AMQPSTRANSPORT_15_019: [If the transport closed, the function shall throw an IllegalStateException.]
    @Test(expected = IllegalStateException.class)
    public void invokeCallbacksFailsIfTransportOpenedAndClosed()
            throws IOException
    {
        AmqpsTransport transport = new AmqpsTransport(mockConfig, false);
        transport.open();
        transport.close();
        transport.invokeCallbacks();
    }

    // Tests_SRS_AMQPSTRANSPORT_15_020: [The function shall invoke all the callbacks from the callback queue.]
    @Test
    public void invokeCallbacksInvokesAllCallbacksFromQueue() throws IOException
    {
        final Integer context = 24;

        new NonStrictExpectations()
        {
            {
                new AmqpsIotHubConnection(mockConfig, false);
                result = mockConnection;
                mockIotHubCallbackPacket.getCallback();
                result = mockIotHubEventCallback;
                mockIotHubCallbackPacket.getStatus();
                result = IotHubStatusCode.OK_EMPTY;
                mockIotHubCallbackPacket.getContext();
                result = context;
            }
        };

        AmqpsTransport transport = new AmqpsTransport(mockConfig, false);
        transport.open();

        Queue<IotHubCallbackPacket> callbackList = new LinkedList<>();
        callbackList.add(mockIotHubCallbackPacket);
        callbackList.add(mockIotHubCallbackPacket);
        Deencapsulation.setField(transport, "callbackList", callbackList);

        transport.invokeCallbacks();

        new Verifications()
        {
            {
                mockIotHubCallbackPacket.getStatus();
                times = 2;
                mockIotHubCallbackPacket.getCallback();
                times = 2;
                mockIotHubCallbackPacket.getCallback();
                times = 2;
                mockIotHubEventCallback.execute(IotHubStatusCode.OK_EMPTY, context);
                times = 2;
            }
        };
    }

    // Tests_SRS_AMQPSTRANSPORT_15_021: [If the transport is closed, the function shall throw an IllegalStateException.]
    @Test(expected = IllegalStateException.class)
    public void handleMessageFailsIfTransportNeverOpened() throws IOException
    {
        AmqpsTransport transport = new AmqpsTransport(mockConfig, false);
        transport.handleMessage();
    }

    // Tests_SRS_AMQPSTRANSPORT_15_021: [If the transport is closed, the function shall throw an IllegalStateException.]
    @Test(expected = IllegalStateException.class)
    public void handleMessageFailsIfTransportAlreadyClosed() throws IOException
    {
        AmqpsTransport transport = new AmqpsTransport(mockConfig, false);
        transport.open();
        transport.close();
        transport.handleMessage();
    }

    // Tests_SRS_AMQPSTRANSPORT_15_025: [If no callback is defined, the list of received messages is cleared.]
    @Test
    public void handleMessageClearsReceivedMessagesListIfNoCallbackIsDefined() throws IOException
    {
        new NonStrictExpectations()
        {
            {
                new AmqpsIotHubConnection(mockConfig, false);
                result = mockConnection;
                mockConfig.getMessageCallback();
                result = null;
            }
        };

        AmqpsTransport transport = new AmqpsTransport(mockConfig, false);
        transport.open();

        Queue<AmqpsMessage> receivedMessages = new LinkedBlockingQueue<>();
        receivedMessages.add(mockAmqpsMessage);
        receivedMessages.add(mockAmqpsMessage);
        Deencapsulation.setField(transport, "receivedMessages", receivedMessages);

        transport.handleMessage();

        Queue<AmqpsMessage> receivedTransportMessages = Deencapsulation.getField(transport, "receivedMessages");

        Assert.assertTrue(receivedTransportMessages.size() == 0);
    }

    // Tests_SRS_AMQPSTRANSPORT_15_024: [If no message was received from IotHub, the function shall return.]
    @Test
    public void handleMessageReturnsIfConfigIsNull() throws IOException
    {
        new NonStrictExpectations()
        {
            {
                new AmqpsIotHubConnection(mockConfig, false);
                result = mockConnection;
                mockConfig.getMessageCallback();
                result = mockMessageCallback;
            }
        };

        AmqpsTransport transport = new AmqpsTransport(mockConfig, false);
        transport.open();
        transport.handleMessage();

        Queue<AmqpsMessage> receivedMessages = Deencapsulation.getField(transport, "receivedMessages");
        Assert.assertEquals(0, receivedMessages.size());

        new Verifications()
        {
            {
                mockMessageCallback.execute((Message) any, any);
                times = 0;
            }
        };
    }

    // Tests_SRS_AMQPSTRANSPORT_15_023: [The function shall attempt to consume a message from the IoT Hub.]
    // Tests_SRS_AMQPSTRANSPORT_15_026: [The function shall invoke the callback on the message.]
    // Tests_SRS_AMQPSTRANSPORT_15_027: [The function shall return the message result (one of COMPLETE, ABANDON, or REJECT) to the IoT Hub.]
    @Test
    public void handleMessageConsumesAMessage() throws IOException
    {
        new NonStrictExpectations()
        {
            {
                new AmqpsIotHubConnection(mockConfig, false);
                result = mockConnection;
                mockConfig.getMessageCallback();
                result = mockMessageCallback;
                mockMessageCallback.execute((Message) any, any);
                result = IotHubMessageResult.COMPLETE;
                mockConnection.sendMessageResult(mockAmqpsMessage, IotHubMessageResult.COMPLETE);
                result = true;
            }
        };

        new MockUp<AmqpsTransport>() {
            @Mock
            Message protonMessageToIoTHubMessage(MessageImpl protonMessage) {
                return new Message();
            }
        };

        AmqpsTransport transport = new AmqpsTransport(mockConfig, false);
        transport.open();

        Queue<AmqpsMessage> receivedMessages = new LinkedBlockingQueue<>();
        receivedMessages.add(mockAmqpsMessage);
        receivedMessages.add(mockAmqpsMessage);
        Deencapsulation.setField(transport, "receivedMessages", receivedMessages);

        transport.handleMessage();

        Queue<AmqpsMessage> receivedTransportMessages = Deencapsulation.getField(transport, "receivedMessages");

        new Verifications()
        {
            {
                mockMessageCallback.execute((Message) any, any);
                times = 1;
                mockConnection.sendMessageResult(mockAmqpsMessage, IotHubMessageResult.COMPLETE);
                times = 1;
            }
        };

        Assert.assertTrue(receivedTransportMessages.size() == 1);
    }

    // Tests_SRS_AMQPSTRANSPORT_15_028: [If the result could not be sent to IoTHub, the message shall be put back in the received messages queue to be processed again.]
    // Tests_SRS_AMQPSTRANSPORT_15_028: [If the result could not be sent to IoTHub, the message shall be put back in the received messages queue to be processed again.]
    @Test
    public void handleMessagePutsMessageBackIntoQueueIfCannotSendResultBackToServer() throws IOException
    {
        new NonStrictExpectations()
        {
            {
                new AmqpsIotHubConnection(mockConfig, false);
                result = mockConnection;
                mockConfig.getMessageCallback();
                result = mockMessageCallback;
                mockMessageCallback.execute((Message) any, any);
                result = IotHubMessageResult.COMPLETE;
                mockConnection.sendMessageResult(mockAmqpsMessage, IotHubMessageResult.COMPLETE);
                result = false;
            }
        };

        new MockUp<AmqpsTransport>() {
            @Mock
            Message protonMessageToIoTHubMessage(MessageImpl protonMessage) {
                return new Message();
            }
        };

        AmqpsTransport transport = new AmqpsTransport(mockConfig, false);
        transport.open();

        Queue<AmqpsMessage> receivedMessages = new LinkedBlockingQueue<>();
        receivedMessages.add(mockAmqpsMessage);
        receivedMessages.add(mockAmqpsMessage);
        Deencapsulation.setField(transport, "receivedMessages", receivedMessages);

        transport.handleMessage();

        Queue<AmqpsMessage> receivedTransportMessages = Deencapsulation.getField(transport, "receivedMessages");

        Assert.assertTrue(receivedTransportMessages.size() == 2);

        new Verifications()
        {
            {
                mockMessageCallback.execute((Message) any, any);
                times = 1;
                mockConnection.sendMessageResult(mockAmqpsMessage, IotHubMessageResult.COMPLETE);
                times = 1;
            }
        };

        Assert.assertTrue(receivedTransportMessages.size() == 2);
    }

    // Tests_SRS_AMQPSTRANSPORT_15_029: [If the hash cannot be found in the list of keys for the messages in progress, the method returns.]
    @Test
    public void messageSentReturnsIfThereAreNoMessagesInProgress() throws IOException
    {
        new NonStrictExpectations()
        {
            {
                new AmqpsIotHubConnection(mockConfig, false);
                result = mockConnection;
            }
        };

        AmqpsTransport transport = new AmqpsTransport(mockConfig, false);
        transport.open();

        Map<Integer, IotHubOutboundPacket> inProgressMessages = new ConcurrentHashMap<>();
        Deencapsulation.setField(transport, "inProgressMessages", inProgressMessages);

        transport.messageSent(1, true);

        new Verifications()
        {
            {
                new IotHubCallbackPacket(IotHubStatusCode.OK_EMPTY, (IotHubEventCallback) any, any);
                times = 0;
            }
        };
    }

    // Tests_SRS_AMQPSTRANSPORT_15_030: [If the message was successfully delivered,
    // its callback is added to the list of callbacks to be executed.]
    @Test
    public void messageSentRemovesSuccessfullyDeliveredMessageFromInProgressMap() throws IOException
    {
        new NonStrictExpectations()
        {
            {
                new AmqpsIotHubConnection(mockConfig, false);
                result = mockConnection;
            }
        };

        AmqpsTransport transport = new AmqpsTransport(mockConfig, false);
        transport.open();

        Map<Integer, IotHubOutboundPacket> inProgressMessages = new ConcurrentHashMap<>();
        inProgressMessages.put(1, new IotHubOutboundPacket(new Message(), mockIotHubEventCallback, new Object()));
        inProgressMessages.put(2, new IotHubOutboundPacket(new Message(), mockIotHubEventCallback, new Object()));
        Deencapsulation.setField(transport, "inProgressMessages", inProgressMessages);

        transport.messageSent(1, true);

        new Verifications()
        {
            {
                new IotHubCallbackPacket(IotHubStatusCode.OK_EMPTY, (IotHubEventCallback) any, any);
                times = 1;
            }
        };

        Queue<IotHubOutboundPacket> waitingMessages = Deencapsulation.getField(transport, "waitingMessages");
        Queue<IotHubCallbackPacket> callbackList  = Deencapsulation.getField(transport, "callbackList");

        Assert.assertTrue(inProgressMessages.size() == 1);
        Assert.assertTrue(waitingMessages.size() == 0);
        Assert.assertTrue(callbackList.size() == 1);
    }

    // Tests_SRS_AMQPSTRANSPORT_15_031: [If the message was not delivered successfully, it is buffered to be sent again.]
    @Test
    public void messageSentBuffersPreviouslySentMessageIfNotSuccessfullyDelivered() throws IOException
    {
        new NonStrictExpectations()
        {
            {
                new AmqpsIotHubConnection(mockConfig, false);
                result = mockConnection;
            }
        };

        AmqpsTransport transport = new AmqpsTransport(mockConfig, false);
        transport.open();

        Map<Integer, IotHubOutboundPacket> inProgressMessages = new ConcurrentHashMap<>();
        inProgressMessages.put(1, new IotHubOutboundPacket(new Message(), mockIotHubEventCallback, new Object()));
        inProgressMessages.put(2, new IotHubOutboundPacket(new Message(), mockIotHubEventCallback, new Object()));
        Deencapsulation.setField(transport, "inProgressMessages", inProgressMessages);

        transport.messageSent(1, false);

        new Verifications()
        {
            {
                new IotHubCallbackPacket(IotHubStatusCode.OK_EMPTY, (IotHubEventCallback) any, any);
                times = 0;
            }
        };

        Queue<IotHubOutboundPacket> waitingMessages = Deencapsulation.getField(transport, "waitingMessages");
        Queue<IotHubCallbackPacket> callbackList  = Deencapsulation.getField(transport, "callbackList");

        Assert.assertTrue(inProgressMessages.size() == 1);
        Assert.assertTrue(waitingMessages.size() == 1);
        Assert.assertTrue(callbackList.size() == 0);
    }

    // Tests_SRS_AMQPSTRANSPORT_15_032: [The messages in progress are buffered to be sent again.]
    // Tests_SRS_AMQPSTRANSPORT_15_033: [The map of messages in progress is cleared.]
    @Test
    public void connectionLostClearsAllInProgressMessagesAndAddsThemToTheWaitingList() throws IOException
    {
        new NonStrictExpectations()
        {
            {
                new AmqpsIotHubConnection(mockConfig, false);
                result = mockConnection;
            }
        };

        AmqpsTransport transport = new AmqpsTransport(mockConfig, false);
        transport.open();

        Map<Integer, IotHubOutboundPacket> inProgressMessages = new ConcurrentHashMap<>();
        inProgressMessages.put(1, new IotHubOutboundPacket(new Message(), mockIotHubEventCallback, new Object()));
        inProgressMessages.put(2, new IotHubOutboundPacket(new Message(), mockIotHubEventCallback, new Object()));
        Deencapsulation.setField(transport, "inProgressMessages", inProgressMessages);

        Queue<IotHubOutboundPacket> waitingMessages = new LinkedBlockingDeque<>();
        waitingMessages.add(new IotHubOutboundPacket(new Message(), mockIotHubEventCallback, new Object()));
        waitingMessages.add(new IotHubOutboundPacket(new Message(), mockIotHubEventCallback, new Object()));
        Deencapsulation.setField(transport, "waitingMessages", waitingMessages);

        transport.connectionLost();

        Assert.assertTrue(inProgressMessages.size() == 0);
        Assert.assertTrue(waitingMessages.size() == 4);
    }

    // Tests_SRS_AMQPSTRANSPORT_15_034: [The message received is added to the list of messages to be processed.]
    @Test
    public void messageReceivedAddsTheMessageToTheListOfMessagesToBeProcessed() throws IOException
    {
        new NonStrictExpectations()
        {
            {
                new AmqpsIotHubConnection(mockConfig, false);
                result = mockConnection;
            }
        };

        AmqpsTransport transport = new AmqpsTransport(mockConfig, false);
        transport.open();

        Queue<AmqpsMessage> receivedMessages = new LinkedBlockingQueue<>();
        receivedMessages.add(mockAmqpsMessage);
        receivedMessages.add(mockAmqpsMessage);
        Deencapsulation.setField(transport, "receivedMessages", receivedMessages);

        transport.messageReceived(mockAmqpsMessage);

        Assert.assertTrue(receivedMessages.size() == 3);
    }

    // Tests_SRS_AMQPSTRANSPORT_15_035: [The function shall return true if the waiting list,
    // in progress list and callback list are all empty, and false otherwise.]
    @Test
    public void isEmptyReturnsTrue() throws IOException
    {
        AmqpsTransport transport = new AmqpsTransport(mockConfig, false);
        Boolean isEmpty = transport.isEmpty();

        Assert.assertTrue(isEmpty);
    }

    // Tests_SRS_AMQPSTRANSPORT_15_035: [The function shall return true if the waiting list,
    // in progress list and callback list are all empty, and false otherwise.]
    @Test
    public void isEmptyReturnsFalseIfWaitingListIsNotEmpty() throws IOException
    {
        AmqpsTransport transport = new AmqpsTransport(mockConfig, false);

        Queue<IotHubOutboundPacket> waitingMessages = new LinkedBlockingDeque<>();
        waitingMessages.add(new IotHubOutboundPacket(new Message(), mockIotHubEventCallback, new Object()));
        Deencapsulation.setField(transport, "waitingMessages", waitingMessages);

        Boolean isEmpty = transport.isEmpty();

        Assert.assertFalse(isEmpty);
    }

    // Tests_SRS_AMQPSTRANSPORT_15_035: [The function shall return true if the waiting list,
    // in progress list and callback list are all empty, and false otherwise.]
    @Test
    public void isEmptyReturnsFalseIfInProgressMapIsNotEmpty() throws IOException
    {
        AmqpsTransport transport = new AmqpsTransport(mockConfig, false);

        Map<Integer, IotHubOutboundPacket> inProgressMessages = new ConcurrentHashMap<>();
        inProgressMessages.put(1, new IotHubOutboundPacket(new Message(), mockIotHubEventCallback, new Object()));
        Deencapsulation.setField(transport, "inProgressMessages", inProgressMessages);

        Boolean isEmpty = transport.isEmpty();

        Assert.assertFalse(isEmpty);
    }

    // Tests_SRS_AMQPSTRANSPORT_15_035: [The function shall return true if the waiting list,
    // in progress list and callback list are all empty, and false otherwise.]
    @Test
    public void isEmptyReturnsFalseIfCallbackListIsNotEmpty() throws IOException
    {
        AmqpsTransport transport = new AmqpsTransport(mockConfig, false);

        Queue<IotHubCallbackPacket> callbackList = new LinkedList<>();
        callbackList.add(mockIotHubCallbackPacket);
        Deencapsulation.setField(transport, "callbackList", callbackList);

        Boolean isEmpty = transport.isEmpty();

        Assert.assertFalse(isEmpty);
    }
}