// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

package tests.unit.com.microsoft.azure.iothub.transport.mqtt;

import com.microsoft.azure.iothub.*;
import com.microsoft.azure.iothub.transport.IotHubCallbackPacket;
import com.microsoft.azure.iothub.transport.IotHubOutboundPacket;
import com.microsoft.azure.iothub.transport.mqtt.MqttIotHubConnection;
import com.microsoft.azure.iothub.transport.mqtt.MqttTransport;
import junit.framework.AssertionFailedError;
import mockit.*;
import org.junit.Test;

import java.io.IOException;
import java.lang.reflect.Field;
import java.util.HashMap;
import java.util.Map;
import java.util.Queue;

import static org.hamcrest.core.Is.is;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertThat;

public class MqttTransportTest {

    @Mocked
    DeviceClientConfig mockConfig;

    @Mocked
    MqttIotHubConnection mockConnection;

    // Tests_SRS_MQTTTRANSPORT_15_003: [The function shall establish an MQTT connection
    // with IoT Hub given in the configuration.]
    @Test
    public void openOpensMqttConnection() throws IOException, NoSuchFieldException, IllegalAccessException
    {
        new NonStrictExpectations()
        {
            {
                new MqttIotHubConnection(mockConfig);
                result = mockConnection;
            }
        };

        MqttTransport transport = new MqttTransport(mockConfig);
        transport.open();

        final MqttIotHubConnection expectedConnection = mockConnection;
        new Verifications()
        {
            {
                expectedConnection.open();
            }
        };

        Field sendMessagesLock = transport.getClass().getDeclaredField("sendMessagesLock");
        sendMessagesLock.setAccessible(true);
        assertNotNull(sendMessagesLock.get(transport));

        Field handleMessageLock = transport.getClass().getDeclaredField("handleMessageLock");
        handleMessageLock.setAccessible(true);
        assertNotNull(handleMessageLock.get(transport));
    }

    // SRS_MQTTTRANSPORT_15_004: [If the MQTT connection is already open, the function shall do nothing.]
    @Test
    public void openDoesNothingIfAlreadyOpened() throws IOException
    {
        MqttTransport transport = new MqttTransport(mockConfig);
        transport.open();
        transport.open();

        final MqttIotHubConnection expectedConnection = mockConnection;
        new Verifications()
        {
            {
                expectedConnection.open();
                times = 1;
            }
        };
    }

    // Tests_SRS_MQTTTRANSPORT_15_005: [The function shall close the MQTT connection
    // with the IoT Hub given in the configuration.]
    @Test
    public void closeClosesMqttConnection() throws IOException
    {
        MqttTransport transport = new MqttTransport(mockConfig);
        transport.open();
        transport.close();

        final MqttIotHubConnection expectedConnection = mockConnection;
        new Verifications()
        {
            {
                expectedConnection.close();
            }
        };
    }

    // Tests_SRS_MQTTTRANSPORT_15_006: [If the MQTT connection is closed, the function shall do nothing.]
    @Test
    public void closeDoesNothingIfConnectionNeverOpened() throws IOException
    {
        MqttTransport transport = new MqttTransport(mockConfig);
        transport.close();

        final MqttIotHubConnection expectedConnection = mockConnection;
        new Verifications()
        {
            {
                expectedConnection.close();
                times = 0;
            }
        };
    }

    // Tests_SRS_MQTTTRANSPORT_15_006: [If the MQTT connection is closed, the function shall do nothing.]
    @Test
    public void closeDoesNothingIfConnectionAlreadyClosed() throws IOException
    {
        MqttTransport transport = new MqttTransport(mockConfig);
        transport.open();
        transport.close();
        transport.close();

        final MqttIotHubConnection expectedConnection = mockConnection;
        new Verifications()
        {
            {
                expectedConnection.close();
                times = 1;
            }
        };
    }

    // Tests_SRS_MQTTTRANSPORT_15_001: [The constructor shall initialize an empty transport queue
    // for adding messages to be sent as a batch.]
    // Tests_SRS_MQTTTRANSPORT_15_007: [The function shall add a packet containing the message,
    // callback, and callback context to the transport queue.]
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

        MqttTransport transport = new MqttTransport(mockConfig);
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

    // Tests_SRS_MQTTTRANSPORT_15_008: [If the transport is closed, the function shall throw an IllegalStateException.]
    @Test(expected = IllegalStateException.class)
    public void addMessageFailsIfTransportNotOpened(
            @Mocked final Message mockMsg,
            @Mocked final IotHubEventCallback mockCallback)
            throws IOException
    {
        final Map<String, Object> context = new HashMap<>();

        MqttTransport transport = new MqttTransport(mockConfig);
        transport.addMessage(mockMsg, mockCallback, context);
    }

    // Tests_SRS_MQTTTRANSPORT_15_008: [If the transport is closed, the function shall throw an IllegalStateException.]
    @Test(expected = IllegalStateException.class)
    public void addMessageFailsIfTransportAlreadyClosed(
            @Mocked final Message mockMsg,
            @Mocked final IotHubEventCallback mockCallback)
            throws IOException
    {
        final Map<String, Object> context = new HashMap<>();

        MqttTransport transport = new MqttTransport(mockConfig);
        transport.open();
        transport.close();
        transport.addMessage(mockMsg, mockCallback, context);
    }

    // Tests_SRS_MQTTTRANSPORT_15_009: [The function shall attempt to send every message
    // on its waiting list, one at a time.]
    @Test
    public void sendMessagesSendsAllMessages(
            @Mocked final Message mockMsg,
            @Mocked final IotHubEventCallback mockCallback,
            @Mocked final IotHubOutboundPacket mockPacket,
            @Mocked final MqttIotHubConnection mockConnection)
            throws IOException
    {
        final Map<String, Object> context = new HashMap<>();
        new NonStrictExpectations()
        {
            {
                new MqttIotHubConnection(mockConfig);
                result = mockConnection;
                new IotHubOutboundPacket(mockMsg, mockCallback, context);
                result = mockPacket;
                mockPacket.getMessage();
                result = mockMsg;
            }
        };

        MqttTransport transport = new MqttTransport(mockConfig);
        transport.open();
        transport.addMessage(mockMsg, mockCallback, context);
        transport.addMessage(mockMsg, mockCallback, context);
        transport.sendMessages();

        final MqttIotHubConnection expectedConnection = mockConnection;
        new Verifications()
        {
            {
                expectedConnection.sendEvent(mockMsg);
                times = 2;
            }
        };
    }

    // Tests_SRS_MQTTTRANSPORT_15_010: [For each message being sent, the function shall send the message
    // and add the IoT Hub status code along with the callback and context to the callback list.]
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
                new MqttIotHubConnection(mockConfig);
                result = mockConnection;
                new IotHubOutboundPacket(mockMsg, mockCallback, context);
                result = mockPacket;
                mockPacket.getCallback();
                result = mockCallback;
                mockPacket.getContext();
                result = context;
                mockConnection.sendEvent((Message) any);
                returns(IotHubStatusCode.OK_EMPTY, IotHubStatusCode.ERROR);
                new IotHubCallbackPacket(IotHubStatusCode.OK_EMPTY, mockCallback, context);
                result = mockCallbackPacket;
                new IotHubCallbackPacket(IotHubStatusCode.ERROR, mockCallback, context);
                result = mockCallbackPacket;
            }
        };

        MqttTransport transport = new MqttTransport(mockConfig);
        transport.open();
        transport.addMessage(mockMsg, mockCallback, context);
        transport.addMessage(mockMsg, mockCallback, context);
        transport.sendMessages();

        new VerificationsInOrder()
        {
            {
                new IotHubCallbackPacket(IotHubStatusCode.OK_EMPTY, mockCallback, context);
                mockQueue.add(mockCallbackPacket);
                new IotHubCallbackPacket(IotHubStatusCode.ERROR, mockCallback, context);
                mockQueue.add(mockCallbackPacket);
            }
        };
    }

    // Tests_SRS_MQTTTRANSPORT_15_011: [If the IoT Hub could not be reached, 
    // the message shall be buffered to be sent again next time.]
    @Test
    public void sendMessagesBuffersFailedMessages(
            @Mocked final Message mockMsg,
            @Mocked final IotHubEventCallback mockCallback)
            throws IOException
    {
        final Map<String, Object> context = new HashMap<>();
        new NonStrictExpectations()
        {
            {
                mockConnection.sendEvent((Message) any);
                result = new IllegalStateException(anyString);
                result = IotHubStatusCode.OK_EMPTY;
            }
        };

        MqttTransport transport = new MqttTransport(mockConfig);
        transport.open();
        transport.addMessage(mockMsg, mockCallback, context);
        transport.sendMessages();
        transport.sendMessages();

        final MqttIotHubConnection expectedConnection = mockConnection;
        new Verifications()
        {
            {
                expectedConnection.sendEvent(mockMsg);
                times = 2;
            }
        };
    }

    // Tests_SRS_MQTTTRANSPORT_15_011: [If the MQTT connection is closed,
    // the function shall throw an IllegalStateException.]
    @Test(expected = IllegalStateException.class)
    public void sendMessagesFailsIfTransportNeverOpened() throws IOException
    {
        MqttTransport transport = new MqttTransport(mockConfig);
        transport.sendMessages();
    }

    // Tests_SRS_MQTTTRANSPORT_15_011: [If the MQTT connection is closed,
    // the function shall throw an IllegalStateException.]
    @Test(expected = IllegalStateException.class)
    public void sendMessagesFailsIfTransportClosed() throws IOException
    {
        MqttTransport transport = new MqttTransport(mockConfig);
        transport.open();
        transport.close();
        transport.sendMessages();
    }

    // Tests_SRS_MQTTTRANSPORT_15_013: [The function shall invoke all callbacks on the callback queue.]
    @Test
    public void invokeCallbacksInvokesAllCallbacks(
            @Mocked final Message mockMsg,
            @Mocked final IotHubEventCallback mockCallback,
            @Mocked final IotHubCallbackPacket mockCallbackPacket)
            throws IOException
    {
        final Map<String, Object> context = new HashMap<>();
        new NonStrictExpectations()
        {
            {
                new MqttIotHubConnection(mockConfig);
                result = mockConnection;
                mockCallbackPacket.getStatus();
                returns(IotHubStatusCode.OK_EMPTY, IotHubStatusCode.ERROR);
                mockCallbackPacket.getCallback();
                result = mockCallback;
                mockCallbackPacket.getContext();
                result = context;
            }
        };

        MqttTransport transport = new MqttTransport(mockConfig);
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
                expectedCallback.execute(IotHubStatusCode.OK_EMPTY, expectedContext);
                expectedCallback.execute(IotHubStatusCode.ERROR, expectedContext);
            }
        };
    }

    // Tests_SRS_MQTTTRANSPORT_15_014: [If the transport is closed, the function shall throw an IllegalStateException.]
    @Test(expected = IllegalStateException.class)
    public void invokeCallbacksFailsIfTransportNeverOpened() throws IOException
    {
        MqttTransport transport = new MqttTransport(mockConfig);
        transport.invokeCallbacks();
    }

    // Tests_SRS_MQTTTRANSPORT_15_014: [If the transport is closed, the function shall throw an IllegalStateException.]
    @Test(expected = IllegalStateException.class)
    public void invokeCallbacksFailsIfTransportAlreadyClosed() throws IOException
    {
        MqttTransport transport = new MqttTransport(mockConfig);
        transport.open();
        transport.close();
        transport.invokeCallbacks();
    }

    // Tests_SRS_MQTTTRANSPORT_15_015: [If an exception is thrown during the callback,
    // the function shall drop the callback from the queue.]
    @Test
    public void invokeCallbacksDropsFailedCallback(
            @Mocked final Message mockMsg,
            @Mocked final IotHubEventCallback mockCallback,
            @Mocked final IotHubCallbackPacket mockCallbackPacket)
            throws IOException
    {
        final Map<String, Object> context = new HashMap<>();
        new NonStrictExpectations()
        {
            {
                new MqttIotHubConnection(mockConfig);
                result = mockConnection;
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

        MqttTransport transport = new MqttTransport(mockConfig);
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
                expectedCallback.execute(IotHubStatusCode.OK_EMPTY, expectedContext);
            }
        };
    }

    // Tests_SRS_MqttTransport_11_019: [The function shall return true if the waiting list
    // and callback list are all empty, and false otherwise.]
    @Test
    public void isEmptyReturnsFalseIfWaitingListIsNotEmpty(
            @Mocked final Message mockMsg,
            @Mocked final IotHubEventCallback mockCallback) throws IOException
    {
        final Map<String, Object> context = new HashMap<>();

        MqttTransport transport = new MqttTransport(mockConfig);
        transport.open();
        transport.addMessage(mockMsg, mockCallback, context);
        boolean testIsEmpty = transport.isEmpty();

        boolean expectedIsEmpty = false;
        assertThat(testIsEmpty, is(expectedIsEmpty));
    }

    // Tests_SRS_MqttTransport_11_019: [The function shall return true if the waiting list
    // and callback list are all empty, and false otherwise.]
    @Test
    public void isEmptyReturnsFalseIfCallbackListIsNotEmpty(
            @Mocked final Message mockMsg,
            @Mocked final IotHubEventCallback mockCallback)
            throws IOException
    {
        final Map<String, Object> context = new HashMap<>();

        MqttTransport transport = new MqttTransport(mockConfig);
        transport.open();
        transport.addMessage(mockMsg, mockCallback, context);
        transport.addMessage(mockMsg, mockCallback, context);
        transport.addMessage(mockMsg, mockCallback, context);
        transport.sendMessages();
        boolean testIsEmpty = transport.isEmpty();

        final boolean expectedIsEmpty = false;
        assertThat(testIsEmpty, is(expectedIsEmpty));
    }

    // Tests_SRS_MqttTransport_11_019: [The function shall return true if the waiting list
    // and callback list are all empty, and false otherwise.]
    @Test
    public void isEmptyReturnsTrueIfEmpty(
            @Mocked final Message mockMsg,
            @Mocked final IotHubEventCallback mockCallback)
            throws IOException
    {
        final Map<String, Object> context = new HashMap<>();

        MqttTransport transport = new MqttTransport(mockConfig);
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

    // Tests_SRS_MQTTTRANSPORT_15_016: [The function shall attempt to consume a message from the IoT Hub.]
    @Test
    public void handleMessageAttemptsToReceiveMessage(@Mocked final MessageCallback mockCallback) throws IOException
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

        MqttTransport transport = new MqttTransport(mockConfig);
        transport.open();
        transport.handleMessage();

        final MqttIotHubConnection expectedConnection = mockConnection;
        new Verifications()
        {
            {
                expectedConnection.receiveMessage();
            }
        };
    }

    // Tests_SRS_MQTTTRANSPORT_15_017: [If a message is found and a message callback is registered,
    // the function shall invoke the callback on the message.]
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
                mockConnection.receiveMessage();
                result = mockMsg;
            }
        };

        MqttTransport transport = new MqttTransport(mockConfig);
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

    // Tests_SRS_MQTTTRANSPORT_15_018: [If the MQTT connection is closed,
    // the function shall throw an IllegalStateException.]
    @Test(expected = IllegalStateException.class)
    public void handleMessageFailsIfTransportNeverOpened() throws IOException
    {
        MqttTransport transport = new MqttTransport(mockConfig);
        transport.handleMessage();
    }

    // Tests_SRS_MQTTTRANSPORT_15_018: [If the MQTT connection is closed,
    // the function shall throw an IllegalStateException.]
    @Test(expected = IllegalStateException.class)
    public void handleMessageFailsIfTransportAlreadyClosed() throws IOException
    {
        MqttTransport transport = new MqttTransport(mockConfig);
        transport.open();
        transport.close();
        transport.handleMessage();
    }
}