/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package tests.unit.com.microsoft.azure.iothub.transport.amqps;

import com.microsoft.azure.iothub.DeviceClientConfig;
import com.microsoft.azure.iothub.IotHubMessageResult;
import com.microsoft.azure.iothub.auth.IotHubSasToken;
import com.microsoft.azure.iothub.net.IotHubUri;
import com.microsoft.azure.iothub.transport.amqps.AmqpsIotHubConnection;
import com.microsoft.azure.iothub.transport.amqps.AmqpsIotHubConnectionBaseHandler;
import com.microsoft.azure.iothub.transport.amqps.AmqpsMessage;
import com.microsoft.azure.iothub.transport.amqps.IotHubReactor;
import mockit.*;
import org.apache.qpid.proton.Proton;
import org.apache.qpid.proton.engine.BaseHandler;
import org.apache.qpid.proton.engine.Event;
import org.apache.qpid.proton.message.Message;
import org.apache.qpid.proton.reactor.Reactor;
import org.junit.Test;

import java.io.IOException;
import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.CompletableFuture;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.TimeoutException;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertNull;

public class AmqpsIotHubConnectionTest {

    final String hostName = "test.host.name";
    final String hubName = "test.iothub";
    final String deviceId = "test-deviceId";
    final String deviceKey = "test-devicekey?&test";
    final String resourceUri = "test-resource-uri";

    @Mocked
    protected Proton mockProton;
    @Mocked
    protected Reactor mockReactor;
    @Mocked
    protected AmqpsIotHubConnectionBaseHandler mockHandler;
    @Mocked
    protected DeviceClientConfig mockConfig;
    @Mocked
    protected IotHubUri mockIotHubUri;
    @Mocked
    protected IotHubSasToken mockToken;
    @Mocked
    protected Message mockMessage;
    @Mocked
    protected AmqpsMessage mockAmqpsMessage;
    @Mocked
    protected Event mockEvent;

    // Tests_SRS_AMQPSIOTHUBCONNECTION_14_001: [The constructor shall save the configuration.]
    // Tests_SRS_AMQPSIOTHUBCONNECTION_14_003: [The constructor shall create a new SAS token and copy all input parameters to private member variables.]
    // Tests_SRS_AMQPSIOTHUBCONNECTION_14_004: [The constructor shall set it’s state to CLOSED.]
    // Tests_SRS_AMQPSIOTHUBCONNECTION_14_005: [The constructor shall initialize a new private queue for received messages.]
    // Tests_SRS_AMQPSIOTHUBCONNECTION_14_006: [The constructor shall initialize a new private map for messages that are in progress.]
    // Tests_SRS_AMQPSIOTHUBCONNECTION_14_007: [The constructor shall initialize new private Futures for the status of the Connection and Reactor.]
    @Test(expected = IllegalStateException.class)
    public void constructorCopiesAllData() throws IOException {
        baseExpectations();

        AmqpsIotHubConnection connection = new AmqpsIotHubConnection(mockConfig);

        String actualHostName = Deencapsulation.getField(connection, "hostName");
        String actualDeviceId = Deencapsulation.getField(connection, "deviceID");
        String actualUserName = Deencapsulation.getField(connection, "userName");
        String actualSasToken = Deencapsulation.getField(connection, "sasToken");
        DeviceClientConfig actualConfig = Deencapsulation.getField(connection, "config");

        assertEquals(hostName, actualHostName);
        assertEquals(deviceId + "@sas." + hubName, actualUserName);
        assertEquals(deviceId, actualDeviceId);
        assertEquals(mockToken.toString(), actualSasToken);
        assertEquals(mockConfig, actualConfig);

        assertEquals(LinkedBlockingQueue.class, Deencapsulation.getField(connection, "receivedMessageQueue").getClass());
        assertEquals(HashMap.class, Deencapsulation.getField(connection, "inProgressMessageMap").getClass());
        assertEquals(CompletableFuture.class, Deencapsulation.getField(connection, "completionStatus").getClass());
        assertEquals(CompletableFuture.class, Deencapsulation.getField(connection, "reactorReady").getClass());

        assertNotNull(Deencapsulation.getField(connection, "receivedMessageQueue").getClass());
        assertNotNull(Deencapsulation.getField(connection, "inProgressMessageMap").getClass());
        assertNotNull(Deencapsulation.getField(connection, "completionStatus").getClass());
        assertNotNull(Deencapsulation.getField(connection, "reactorReady").getClass());

        //Throws exception if state is CLOSED
        connection.scheduleSend("");
    }

    //Tests_SRS_AMQPSIOTHUBCONNECTION_14_002: [The constructor shall throw a new IllegalArgumentException if any of the parameters of the configuration is null or empty.]
    @Test(expected = IllegalArgumentException.class)
    public void constructorThrowsIllegalArgumentExceptionIfHostNameIsEmpty(){
        new NonStrictExpectations() {
            {
                mockConfig.getIotHubHostname();
                result = "";
                mockConfig.getIotHubName();
                result = hubName;
                mockConfig.getDeviceId();
                result = deviceId;
                mockConfig.getDeviceKey();
                result = deviceKey;
            }
        };

        AmqpsIotHubConnection handler = new AmqpsIotHubConnection(mockConfig);
    }

    //Tests_SRS_AMQPSIOTHUBCONNECTION_14_002: [The constructor shall throw a new IllegalArgumentException if any of the parameters of the configuration is null or empty.]
    @Test(expected = IllegalArgumentException.class)
    public void constructorThrowsIllegalArgumentExceptionIfHostNameIsNull(){
        new NonStrictExpectations() {
            {
                mockConfig.getIotHubHostname();
                result = null;
                mockConfig.getIotHubName();
                result = hubName;
                mockConfig.getDeviceId();
                result = deviceId;
                mockConfig.getDeviceKey();
                result = deviceKey;
            }
        };

        AmqpsIotHubConnection handler = new AmqpsIotHubConnection(mockConfig);
    }

    //Tests_SRS_AMQPSIOTHUBCONNECTION_14_002: [The constructor shall throw a new IllegalArgumentException if any of the parameters of the configuration is null or empty.]
    @Test(expected = IllegalArgumentException.class)
    public void constructorThrowsIllegalArgumentExceptionIfDeviceIDIsEmpty(){
        new NonStrictExpectations() {
            {
                mockConfig.getIotHubHostname();
                result = hostName;
                mockConfig.getIotHubName();
                result = hubName;
                mockConfig.getDeviceId();
                result = "";
                mockConfig.getDeviceKey();
                result = deviceKey;
            }
        };

        AmqpsIotHubConnection handler = new AmqpsIotHubConnection(mockConfig);
    }

    //Tests_SRS_AMQPSIOTHUBCONNECTION_14_002: [The constructor shall throw a new IllegalArgumentException if any of the parameters of the configuration is null or empty.]
    @Test(expected = IllegalArgumentException.class)
    public void constructorThrowsIllegalArgumentExceptionIfDeviceIDIsNull(){
        new NonStrictExpectations() {
            {
                mockConfig.getIotHubHostname();
                result = hostName;
                mockConfig.getIotHubName();
                result = hubName;
                mockConfig.getDeviceId();
                result = null;
                mockConfig.getDeviceKey();
                result = deviceKey;
            }
        };

        AmqpsIotHubConnection handler = new AmqpsIotHubConnection(mockConfig);
    }

    //Tests_SRS_AMQPSIOTHUBCONNECTION_14_002: [The constructor shall throw a new IllegalArgumentException if any of the parameters of the configuration is null or empty.]
    @Test(expected = IllegalArgumentException.class)
    public void constructorThrowsIllegalArgumentExceptionIfUserNameIsEmpty(){
        new NonStrictExpectations() {
            {
                mockConfig.getIotHubHostname();
                result = hostName;
                mockConfig.getIotHubName();
                result = hubName;
                mockConfig.getDeviceId();
                result = deviceId;
                mockConfig.getDeviceKey();
                result = "";
            }
        };

        AmqpsIotHubConnection handler = new AmqpsIotHubConnection(mockConfig);
    }

    //Tests_SRS_AMQPSIOTHUBCONNECTION_14_002: [The constructor shall throw a new IllegalArgumentException if any of the parameters of the configuration is null or empty.]
    @Test(expected = IllegalArgumentException.class)
    public void constructorThrowsIllegalArgumentExceptionIfUserNameIsNull(){
        new NonStrictExpectations() {
            {
                mockConfig.getIotHubHostname();
                result = hostName;
                mockConfig.getIotHubName();
                result = hubName;
                mockConfig.getDeviceId();
                result = deviceId;
                mockConfig.getDeviceKey();
                result = null;
            }
        };

        AmqpsIotHubConnection handler = new AmqpsIotHubConnection(mockConfig);
    }

    //Tests_SRS_AMQPSIOTHUBCONNECTION_14_002: [The constructor shall throw a new IllegalArgumentException if any of the parameters of the configuration is null or empty.]
    @Test(expected = IllegalArgumentException.class)
    public void constructorThrowsIllegalArgumentExceptionIfSasTokenIsEmpty(){
        new NonStrictExpectations() {
            {
                mockConfig.getIotHubHostname();
                result = hostName;
                mockConfig.getIotHubName();
                result = "";
                mockConfig.getDeviceId();
                result = deviceId;
                mockConfig.getDeviceKey();
                result = deviceKey;
            }
        };

        AmqpsIotHubConnection handler = new AmqpsIotHubConnection(mockConfig);
    }

    //Tests_SRS_AMQPSIOTHUBCONNECTION_14_002: [The constructor shall throw a new IllegalArgumentException if any of the parameters of the configuration is null or empty.]
    @Test(expected = IllegalArgumentException.class)
    public void constructorThrowsIllegalArgumentExceptionIfSasTokenIsNull(){
        new NonStrictExpectations() {
            {
                mockConfig.getIotHubHostname();
                result = hostName;
                mockConfig.getIotHubName();
                result = null;
                mockConfig.getDeviceId();
                result = deviceId;
                mockConfig.getDeviceKey();
                result = deviceKey;
            }
        };

        AmqpsIotHubConnection handler = new AmqpsIotHubConnection(mockConfig);
    }

    // Tests_SRS_AMQPSIOTHUBCONNECTION_14_008: [The function shall initialize it’s AmqpsIotHubConnectionBaseHandler using the saved host name, user name, device ID and sas token.]
    @Test
    public void openInitializesBaseHandler(
            @Mocked CompletableFuture<Boolean> future) throws InterruptedException, ExecutionException, IOException, TimeoutException {
        baseExpectations();
        connectionOpenExpectations(future, false);

        AmqpsIotHubConnection connection = new AmqpsIotHubConnection(mockConfig);
        connection.open();

        new Verifications()
        {
            {
                new AmqpsIotHubConnectionBaseHandler(hostName,
                        deviceId + "@sas." + hubName, mockToken.toString(), deviceId, connection);
            }
        };
    }

    // Tests_SRS_AMQPSIOTHUBCONNECTION_14_009: [The function shall open the Amqps connection and trigger the Reactor (Proton) to begin running.]
    // Tests_SRS_AMQPSIOTHUBCONNECTION_14_011: [If the AMQPS connection is already open, the function shall do nothing.]
    // Tests_SRS_AMQPSIOTHUBCONNECTION_14_010: [Once the Reactor (Proton) is ready, the function shall set it's state to OPEN.]
    // Tests_SRS_AMQPSIOTHUBCONNECTION_14_031: [The function shall get the link credit from it's AmqpsIotHubConnectionBaseHandler and set the private maxQueueSize member variable.]
    // Tests_SRS_AMQPSIOTHUBCONNECTION_14_032: [The function shall successfully complete it’s CompletableFuture status member variable.]
    @Test
    public void openTriggersReactorAndOpensConnection(
            @Mocked CompletableFuture<Boolean> future) throws InterruptedException, ExecutionException, IOException, TimeoutException {
        new MockUp<AmqpsIotHubConnection>(){
            @Mock
            private void startReactorAsync(Invocation inv){inv.proceed();}
        };

        baseExpectations();
        new NonStrictExpectations() {
            {
                mockProton.reactor((BaseHandler) any);
                result = mockReactor;
                mockReactor.run();
                new CompletableFuture<>();
                result = future;
                future.get();
                result = true;
            }
        };

        //mockReactor.run() will only be invoked once if the state is set to OPEN after the first invocation
        AmqpsIotHubConnection connection = new AmqpsIotHubConnection(mockConfig);
        connection.open();
        connection.open();

        new Verifications() {
            {
                mockHandler.getLinkCredit(); times = 1;
                future.complete(new Boolean(true)); times = 1;
            }
        };
    }

    //TODO: // Tests_SRS_AMQPSIOTHUBCONNECTION_14_012: [If the AmqpsIotHubConnectionBaseHandler becomes invalidated before the Reactor (Proton) starts, the function shall throw an IOException.]

    // Tests_SRS_AMQPSIOTHUBCONNECTION_14_013: [The function shall invalidate the private Reactor (Proton) member variable.]
    // Tests_SRS_AMQPSIOTHUBCONNECTION_14_014: [The function shall free the AmqpsIotHubConnectionBaseHandler.]
    // Tests_SRS_AMQPSIOTHUBCONNECTION_14_015: [The function shall close the AMQPS connection.]
    // Tests_SRS_AMQPSIOTHUBCONNECTION_14_033: [The function shall close the AmqpsIotHubConnectionBaseHandler.]
    // Tests_SRS_AMQPSIOTHUBCONNECTION_14_034: [The function shall exceptionally complete all remaining messages that are currently in progress and clear the queue.]
    @Test
    public void closeFullTest(
            @Mocked CompletableFuture<Boolean> future) throws InterruptedException, ExecutionException, IOException, TimeoutException {
        baseExpectations();
        connectionOpenExpectations(future);

        AmqpsIotHubConnection connection = new AmqpsIotHubConnection(mockConfig);
        //Will close after opening
        connection.open();
        connection.close();

        IotHubReactor actualReactor = Deencapsulation.getField(connection, "iotHubReactor");
        AmqpsIotHubConnectionBaseHandler actualHandler = Deencapsulation.getField(connection, "amqpsHandler");
        assertNull(actualReactor);
        assertNull(actualHandler);

        new Verifications()
        {
            {
                mockHandler.shutdown();
                Deencapsulation.invoke(connection, "clearInProgressMap");
                Deencapsulation.invoke((Map)Deencapsulation.getField(connection, "inProgressMessageMap"), "clear");
                Deencapsulation.invoke(connection, "freeReactor");
                Deencapsulation.invoke(connection, "freeHandler");
            }
        };
    }

    // Tests_SRS_AMQPSIOTHUBCONNECTION_14_016: [If the AMQPS connection is already closed, the function shall do nothing.]
    @Test
    public void closeDoesNothingIfClosed() throws InterruptedException, ExecutionException, IOException
    {
        baseExpectations();

        AmqpsIotHubConnection connection = new AmqpsIotHubConnection(mockConfig);
        connection.close();

        new Verifications()
        {
            {
                Deencapsulation.invoke(connection, "freeReactor");
                //Todo: This should be here. But I cannot get this to cooperate at the moment.
                //times = 0;
            }
        };
    }

    // Tests_SRS_AMQPSIOTHUBCONNECTION_14_019: [The function shall attempt to remove a message from the queue.]
    // Tests_SRS_AMQPSIOTHUBCONNECTION_14_020: [The function shall return the message if one was pulled from the queue, otherwise it shall return null.]
    @Test
    public void consumeMessageAttemptsToRemoveAndPullsSuccessfully(
            @Mocked LinkedBlockingQueue<Message> queue) throws InterruptedException, ExecutionException, IOException
    {
        baseExpectations();
        new NonStrictExpectations()
        {
            {
                new LinkedBlockingQueue<>();
                result = queue;
                queue.size();
                result = 1;
                queue.remove();
                result = mockAmqpsMessage;
            }
        };

        AmqpsIotHubConnection connection = new AmqpsIotHubConnection(mockConfig);
        Message consumedMessage = connection.consumeMessage();

        assertNotNull(consumedMessage);

        new Verifications()
        {
            {
                queue.remove();
            }
        };
    }

    // Tests_SRS_AMQPSIOTHUBCONNECTION_14_019: [The function shall attempt to remove a message from the queue.]
    // Tests_SRS_AMQPSIOTHUBCONNECTION_14_020: [The function shall return the message if one was pulled from the queue, otherwise it shall return null.]
    @Test
    public void consumeMessageAttemptsToRemoveAndPullsUnsuccessfully(
            @Mocked LinkedBlockingQueue<Message> queue) throws InterruptedException, ExecutionException, IOException
    {
        baseExpectations();

        AmqpsIotHubConnection connection = new AmqpsIotHubConnection(mockConfig);
        Message consumedMessage = connection.consumeMessage();
        assertNull(consumedMessage);
    }

    // Tests_SRS_AMQPSIOTHUBCONNECTION_14_021: [If the message result is COMPLETE, ABANDON, or REJECT, the function shall acknowledge the last message with acknowledgement type COMPLETE, ABANDON, or REJECT respectively.]
    @Test
    public void sendMessageResultSendsProperAckForMessageResult(
            @Mocked CompletableFuture<Boolean> future) throws InterruptedException, ExecutionException, IOException, TimeoutException {
        baseExpectations();
        connectionOpenExpectations(future);

        AmqpsIotHubConnection connection = new AmqpsIotHubConnection(mockConfig);
        connection.open();
        Deencapsulation.setField(connection, "amqpsHandler", mockHandler);
        Deencapsulation.setField(connection, "lastMessage", mockAmqpsMessage);
        connection.sendMessageResult(IotHubMessageResult.COMPLETE);
        connection.sendMessageResult(IotHubMessageResult.ABANDON);
        connection.sendMessageResult(IotHubMessageResult.REJECT);

        new Verifications() {
            {

                mockAmqpsMessage.acknowledge(AmqpsMessage.ACK_TYPE.COMPLETE);
                mockAmqpsMessage.acknowledge(AmqpsMessage.ACK_TYPE.ABANDON);
                mockAmqpsMessage.acknowledge(AmqpsMessage.ACK_TYPE.REJECT);
            }
        };
    }

    // Tests_SRS_AMQPSIOTHUBCONNECTION_14_022: [If sendMessageResult(result) is called before a message is received, the function shall throw an IllegalStateException.]
    @Test(expected = IllegalStateException.class)
    public void sendMessageResultThrowsIllegalStateExceptionIfNoMessage(
            @Mocked CompletableFuture<Boolean> future) throws InterruptedException, ExecutionException, IOException, TimeoutException {
        baseExpectations();
        connectionOpenExpectations(future);

        AmqpsIotHubConnection connection = new AmqpsIotHubConnection(mockConfig);
        connection.open();
        Deencapsulation.setField(connection, "amqpsHandler", mockHandler);
        connection.sendMessageResult(IotHubMessageResult.COMPLETE);
    }

    // Tests_SRS_AMQPSIOTHUBCONNECTION_14_023: [If the acknowledgement fails, the function shall throw an IOException.]
    @Test(expected = IOException.class)
    public void sendMessageResultThrowsIOExceptionIfAckFails(
            @Mocked CompletableFuture<Boolean> future) throws InterruptedException, ExecutionException, IOException, TimeoutException {
        baseExpectations();
        connectionOpenExpectations(future);
        new NonStrictExpectations() {
            {
                mockAmqpsMessage.acknowledge((AmqpsMessage.ACK_TYPE) any);
                result = new Exception();

            }
        };

        AmqpsIotHubConnection connection = new AmqpsIotHubConnection(mockConfig);
        connection.open();
        Deencapsulation.setField(connection, "amqpsHandler", mockHandler);
        Deencapsulation.setField(connection, "lastMessage", mockAmqpsMessage);
        connection.sendMessageResult(IotHubMessageResult.COMPLETE);
    }

    // Tests_SRS_AMQPSIOTHUBCONNECTION_14_024: [If the AMQPS Connection is closed, the function shall throw an IllegalStateException.]
    @Test(expected = IllegalStateException.class)
    public void sendMessageResultThrowsIllegalStateExceptionIfConnectionClosed() throws IOException {

        baseExpectations();

        AmqpsIotHubConnection connection = new AmqpsIotHubConnection(mockConfig);
        Deencapsulation.setField(connection, "amqpsHandler", mockHandler);
        connection.sendMessageResult(IotHubMessageResult.COMPLETE);
    }

    // Tests_SRS_AMQPSIOTHUBCONNECTION_14_025: [If the AmqpsIotHubConnectionBaseHandler has not been initialized, the function shall throw a new IOException and exceptionally complete it’s CompletableFuture status member variable with the same exception.]
    @Test(expected = IOException.class)
    public void scheduleSendThrowsIOExceptionIfHandlerNotInitialized(
            @Mocked CompletableFuture<Boolean> future,
            @Mocked CompletableFuture<Boolean> mockCompletionStatus) throws InterruptedException, ExecutionException, IOException, TimeoutException {
        final byte[] msgBody = { 0x61, 0x62, 0x63 };

        baseExpectations();
        connectionOpenExpectations(future);

        AmqpsIotHubConnection connection = new AmqpsIotHubConnection(mockConfig);
        connection.open();
        Deencapsulation.setField(connection, "completionStatus", mockCompletionStatus);
        Deencapsulation.setField(connection, "amqpsHandler", null);
        connection.scheduleSend(msgBody);

        new Verifications()
        {
            {
                mockCompletionStatus.completeExceptionally((IOException)any); times = 1;
            }
        };
    }

    // Tests_SRS_AMQPSIOTHUBCONNECTION_14_025: [If the AmqpsIotHubConnectionBaseHandler has not been initialized, the function shall throw a new IOException and exceptionally complete it’s CompletableFuture status member variable with the same exception.]
    @Test
    public void scheduleSendCompletesFutureExceptionallyIfHandlerNotInitialized(
            @Mocked CompletableFuture<Boolean> future,
            @Mocked CompletableFuture<Boolean> mockCompletionStatus) throws InterruptedException, ExecutionException, IOException, TimeoutException {
        final byte[] msgBody = { 0x61, 0x62, 0x63 };

        baseExpectations();
        connectionOpenExpectations(future);

        AmqpsIotHubConnection connection = new AmqpsIotHubConnection(mockConfig);
        connection.open();
        Deencapsulation.setField(connection, "completionStatus", mockCompletionStatus);
        Deencapsulation.setField(connection, "amqpsHandler", null);
        try {
            connection.scheduleSend(msgBody);
        } catch(Exception e){}

        new Verifications()
        {
            {
                mockCompletionStatus.completeExceptionally((IOException)any);
            }
        };
    }

    // Tests_SRS_AMQPSIOTHUBCONNECTION_14_026: [The function shall create a new CompletableFuture for the message acknowledgement.]
    // Tests_SRS_AMQPSIOTHUBCONNECTION_14_027: [The function shall create a new Tuple containing the CompletableFuture, message content, and message ID.]
    // Tests_SRS_AMQPSIOTHUBCONNECTION_14_028: [The function shall acquire a lock and attempt to send the message.]
    @Test
    public void scheduleSendCreatesFutureAndSends(
            @Mocked CompletableFuture<Boolean> future) throws InterruptedException, ExecutionException, IOException, TimeoutException {
        final byte[] msgBody = { 0x61, 0x62, 0x63 };

        baseExpectations();
        connectionOpenExpectations(future);

        AmqpsIotHubConnection connection = new AmqpsIotHubConnection(mockConfig);
        connection.open();
        connection.scheduleSend(msgBody);

        new Verifications()
        {
            {
                //TODO: Improve by using actual Tuple type
                new CompletableFuture<>();
                mockHandler.createBinaryMessage(msgBody, null);
            }
        };
    }

    // Tests_SRS_AMQPSIOTHUBCONNECTION_14_029: [If the AMQPS Connection is closed, the function shall throw an IllegalStateException.]
    @Test(expected = IllegalStateException.class)
    public void scheduleSendThrowsIllegalStateExceptionIfClosed() throws InterruptedException, ExecutionException, IOException
    {
        final byte[] msgBody = { 0x61, 0x62, 0x63 };

        baseExpectations();

        AmqpsIotHubConnection connection = new AmqpsIotHubConnection(mockConfig);
        connection.scheduleSend(msgBody);
    }

    // Tests_SRS_AMQPSIOTHUBCONNECTION_14_030: [The event handler shall set the member AmqpsIotHubConnectionBaseHandler object to handle the connection events.]
    @Test
    public void onReactorInitSetsConnectionToHandler(
            @Mocked CompletableFuture<Boolean> future) throws InterruptedException, ExecutionException, IOException, TimeoutException {
        baseExpectations();
        connectionOpenExpectations(future);

        AmqpsIotHubConnection connection = new AmqpsIotHubConnection(mockConfig);
        connection.open();
        connection.onReactorInit(mockEvent);

        new Verifications()
        {
            {
                mockEvent.getReactor();
                mockReactor.connection(mockHandler);
            }
        };
    }

    public void baseExpectations()
    {
        new NonStrictExpectations() {
            {
                mockConfig.getIotHubHostname();
                result = hostName;
                mockConfig.getIotHubName();
                result = hubName;
                mockConfig.getDeviceId();
                result = deviceId;
                mockConfig.getDeviceKey();
                result = deviceKey;
                IotHubUri.getResourceUri(hostName, deviceId);
                result = resourceUri;
                new IotHubSasToken(resourceUri, deviceId, deviceKey, anyLong );
                result = mockToken;
            }
        };
    }

    public void connectionOpenExpectations(@Mocked CompletableFuture<Boolean> future, boolean needsBaseHandler) throws IOException, ExecutionException, InterruptedException, TimeoutException {
        new MockUp<AmqpsIotHubConnection>(){
            @Mock
            private void startReactorAsync(){}
        };

        if(needsBaseHandler) {
            new NonStrictExpectations() {
                {
                    new AmqpsIotHubConnectionBaseHandler(anyString, anyString, anyString, anyString, (AmqpsIotHubConnection) any);
                    result = mockHandler;
                    mockProton.reactor((BaseHandler) any);
                    result = mockReactor;
                    mockReactor.run();
                    new CompletableFuture<>();
                    result = future;
                    future.get();
                    result = true;
                    mockHandler.getLinkCredit();
                    result = 10;
                }
            };
        } else {
            new NonStrictExpectations() {
                {
                    mockProton.reactor((BaseHandler) any);
                    result = mockReactor;
                    mockReactor.run();
                    new CompletableFuture<>();
                    result = future;
                    future.get();
                    result = true;
                    mockHandler.getLinkCredit();
                    result = 10;
                }
            };
        }
    }

    public void connectionOpenExpectations(@Mocked CompletableFuture<Boolean> future) throws InterruptedException, ExecutionException, IOException, TimeoutException {
        connectionOpenExpectations(future, true);
    }
}
