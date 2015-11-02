/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package tests.unit.com.microsoft.azure.iothub.transport.amqps;

import com.microsoft.azure.iothub.transport.amqps.AmqpsReceiver;
import com.microsoft.azure.iothub.transport.amqps.AmqpsReceiverHandler;
import mockit.Deencapsulation;
import mockit.Mocked;
import mockit.NonStrictExpectations;
import mockit.Verifications;
import org.apache.qpid.proton.Proton;
import org.apache.qpid.proton.engine.*;
import org.apache.qpid.proton.message.Message;
import org.apache.qpid.proton.reactor.*;
import org.apache.qpid.proton.reactor.impl.IO;
import org.junit.Test;

import java.io.IOException;
import java.util.*;
import java.util.concurrent.CompletableFuture;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.Semaphore;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNull;

public class AmqpsReceiverTest {

    @Mocked
    protected Queue<Message> mockMessageQueue;
    @Mocked
    protected AmqpsReceiverHandler mockHandler;
    @Mocked
    protected Reactor mockReactor;
    @Mocked
    protected Proton mockProton;
    @Mocked
    protected Semaphore mockSemaphore;
    @Mocked
    protected Event mockEvent;
    @Mocked
    protected Message mockMessage;

    // Tests_SRS_AMQPSRECEIVER_14_002: [The constructor shall copy all input parameters to private member variables.]
    // Tests_SRS_AMQPSRECEIVER_14_003: [The constructor shall initialize a new private queue for messages.]
    @Test
    public void constructorCopiesAndInitializesAllData(){
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";

        AmqpsReceiver receiver = new AmqpsReceiver(hostName, userName, deviceId, sasToken);

        String actualHostName = Deencapsulation.getField(receiver, "hostName");
        String actualDeviceId = Deencapsulation.getField(receiver, "deviceID");
        String actualUserName = Deencapsulation.getField(receiver, "userName");
        String actualSasToken = Deencapsulation.getField(receiver, "sasToken");

        assertEquals(hostName, actualHostName);
        assertEquals(deviceId, actualDeviceId);
        assertEquals(userName, actualUserName);
        assertEquals(sasToken, actualSasToken);

        new Verifications()
        {
            {
                new LinkedBlockingQueue<>();
            }
        };
    }

    // Tests_SRS_AMQPSRECEIVER_14_001: [The constructor shall throw a new IllegalArgumentException if any of the input parameters is null or empty.]
    @Test(expected = IllegalArgumentException.class)
    public void constructorThrowsIllegalArgumentExceptionIfHostNameIsEmpty(){
        final String hostName = "";
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";

        AmqpsReceiver receiver = new AmqpsReceiver(hostName, userName, deviceId, sasToken);
    }

    // Tests_SRS_AMQPSRECEIVER_14_001: [The constructor shall throw a new IllegalArgumentException if any of the input parameters is null or empty.]
    @Test(expected = IllegalArgumentException.class)
    public void constructorThrowsIllegalArgumentExceptionIfHostNameIsNull(){
        final String hostName = null;
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";

        AmqpsReceiver receiver = new AmqpsReceiver(hostName, userName, deviceId, sasToken);
    }

    // Tests_SRS_AMQPSRECEIVER_14_001: [The constructor shall throw a new IllegalArgumentException if any of the input parameters is null or empty.]
    @Test(expected = IllegalArgumentException.class)
    public void constructorThrowsIllegalArgumentExceptionIfDeviceIDIsEmpty(){
        final String hostName = "test.host.name";
        final String deviceId = "";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";

        AmqpsReceiver receiver = new AmqpsReceiver(hostName, userName, deviceId, sasToken);
    }

    // Tests_SRS_AMQPSRECEIVER_14_001: [The constructor shall throw a new IllegalArgumentException if any of the input parameters is null or empty.]
    @Test(expected = IllegalArgumentException.class)
    public void constructorThrowsIllegalArgumentExceptionIfDeviceIDIsNull(){
        final String hostName = "test.host.name";
        final String deviceId = null;
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";

        AmqpsReceiver receiver = new AmqpsReceiver(hostName, userName, deviceId, sasToken);
    }

    // Tests_SRS_AMQPSRECEIVER_14_001: [The constructor shall throw a new IllegalArgumentException if any of the input parameters is null or empty.]
    @Test(expected = IllegalArgumentException.class)
    public void constructorThrowsIllegalArgumentExceptionIfUserNameIsEmpty(){
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = "";
        final String sasToken = "test-token";

        AmqpsReceiver receiver = new AmqpsReceiver(hostName, userName, deviceId, sasToken);
    }

    // Tests_SRS_AMQPSRECEIVER_14_001: [The constructor shall throw a new IllegalArgumentException if any of the input parameters is null or empty.]
    @Test(expected = IllegalArgumentException.class)
    public void constructorThrowsIllegalArgumentExceptionIfUserNameIsNull(){
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = null;
        final String sasToken = "test-token";

        AmqpsReceiver receiver = new AmqpsReceiver(hostName, userName, deviceId, sasToken);
    }

    // Tests_SRS_AMQPSRECEIVER_14_001: [The constructor shall throw a new IllegalArgumentException if any of the input parameters is null or empty.]
    @Test(expected = IllegalArgumentException.class)
    public void constructorThrowsIllegalArgumentExceptionIfSasTokenIsEmpty(){
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "";

        AmqpsReceiver receiver = new AmqpsReceiver(hostName, userName, deviceId, sasToken);
    }

    // Tests_SRS_AMQPSRECEIVER_14_001: [The constructor shall throw a new IllegalArgumentException if any of the input parameters is null or empty.]
    @Test(expected = IllegalArgumentException.class)
    public void constructorThrowsIllegalArgumentExceptionIfSasTokenIsNull(){
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = null;

        AmqpsReceiver receiver = new AmqpsReceiver(hostName, userName, deviceId, sasToken);
    }

    // Tests_SRS_AMQPSRECEIVER_14_004: [The function shall initialize it’s AmqpsReceiverHandler using the saved host name, user name, device ID and sas token.]
    @Test
    public void openInitializesHandlerUsingSavedData() throws IOException, InterruptedException {
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";

        final AmqpsReceiver receiver = new AmqpsReceiver(hostName, userName, deviceId, sasToken);

        receiver.open();

        new Verifications()
        {
            {
                new AmqpsReceiverHandler(hostName, userName, deviceId, sasToken, receiver);
            }
        };
    }

    // Tests_SRS_AMQPSRECEIVER_14_005: [The function shall open the Amqps connection and trigger the Reactor (Proton) to begin receiving.]
    @Test
    public void openOpensConnectionAndTriggersReactor() throws IOException, InterruptedException {
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";

        new NonStrictExpectations()
        {
            {
                mockProton.reactor((AmqpsReceiver)any);
                result = mockReactor;
            }
        };

        final AmqpsReceiver receiver = new AmqpsReceiver(hostName, userName, deviceId, sasToken);
        //Record the current number of currently running threads
        final int currentThreads = Thread.activeCount();

        CompletableFuture<Boolean> future = receiver.open();
        future.join();

        new Verifications()
        {
            {
                mockReactor.run();
            }
        };
    }

    // Tests_SRS_AMQPSRECEIVER_14_006: [The function shall invalidate its AmqpsReceiverHandler.]
    @Test(expected = IOException.class)
    public void closeInvalidatesReceiverHandlerObject() throws IOException, InterruptedException {
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";

        final AmqpsReceiver receiver = new AmqpsReceiver(hostName, userName, deviceId, sasToken);

        receiver.open();
        receiver.consumeMessage();
        receiver.close();
        receiver.consumeMessage();
    }

    // Tests_SRS_AMQPSRECEIVER_14_007: [If the AmqpsReceiverHandler has not been initialized, the function shall throw a new IOException.]
    @Test(expected = IOException.class)
    public void beginReceiveThrowsIOExceptionIfHandlerNotInit() throws IOException, InterruptedException{
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";

        final AmqpsReceiver receiver = new AmqpsReceiver(hostName, userName, deviceId, sasToken);

        receiver.beginReceive();
    }

    // Tests_SRS_AMQPSRECEIVER_14_007: [If the AmqpsReceiverHandler has not been initialized, the function shall throw a new IOException.]
    @Test(expected = IOException.class)
    public void beginReceiveThrowsIOExceptionIfHandlerNotInitWhenClosed() throws IOException, InterruptedException{
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";

        final AmqpsReceiver receiver = new AmqpsReceiver(hostName, userName, deviceId, sasToken);

        receiver.open();
        receiver.close();
        receiver.beginReceive();
    }

    // Tests_SRS_AMQPSRECEIVER_14_008: [The function shall attempt to acquire a permit from the semaphore.]
    @Test
    public void beginReceiveAttemptsToAcquireSemaphore() throws IOException, InterruptedException{
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";

        final AmqpsReceiver receiver = new AmqpsReceiver(hostName, userName, deviceId, sasToken);

        receiver.open();

        new Verifications()
        {
            {
                mockSemaphore.acquire();
            }
        };
    }

    // Tests_SRS_AMQPSRECEIVER_14_009: [The function shall initialize its Reactor object using itself as the handler.]
    @Test
    public void beginReceiveInitReactorObjectUsingSelf() throws IOException, InterruptedException{
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";

        new NonStrictExpectations()
        {
            {
                mockProton.reactor((BaseHandler)any);
                result = mockReactor;
            }
        };

        final AmqpsReceiver receiver = new AmqpsReceiver(hostName, userName, deviceId, sasToken);

        receiver.open();

        Reactor actualReactor = Deencapsulation.getField(receiver, "reactor");

        assertEquals(mockReactor, actualReactor);
    }

    // Tests_SRS_AMQPSRECEIVER_14_010: [The function shall asynchronously run the reactor.]
    // Tests_SRS_AMQPSRECEIVER_14_011: [The function shall asynchronously close the Receiver after running the Reactor.]
    // Tests_SRS_AMQPSRECEIVER_14_012: [The function shall asynchronously release the semaphore.]
    @Test
    public void beginReceiveAsyncRunCloseRelease() throws IOException, InterruptedException, ExecutionException {
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";

        new NonStrictExpectations()
        {
            {
                new Semaphore(anyInt);
                result = mockSemaphore;
                new AmqpsReceiverHandler(anyString, anyString, anyString, anyString, (AmqpsReceiver)any);
                result = mockHandler;
                mockProton.reactor((BaseHandler)any);
                result = mockReactor;
            }
        };

        final AmqpsReceiver receiver = new AmqpsReceiver(hostName, userName, deviceId, sasToken);
        Deencapsulation.setField(receiver, "reactorSemaphore", mockSemaphore);
        CompletableFuture<Boolean> future = receiver.open();
        future.join();

        new Verifications()
        {
            {
                mockReactor.run();
                receiver.close();
                mockSemaphore.release();
            }
        };
    }

    // Tests_SRS_AMQPSRECEIVER_14_013: [If the Reactor throws a HandlerException while running, the thread shall release the semaphore and close the Receiver.]
    @Test
    public void beginReceiveReactorHandlerException() throws IOException, InterruptedException{
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";

        new NonStrictExpectations()
        {
            {
                //Defined below, a HandlerExceptionReactor is implemented just for this test and always throws
                //a HandlerException when run() is called.
                mockProton.reactor((BaseHandler) any);
                result = new HandlerExceptionReactor();
            }
        };

        final AmqpsReceiver receiver = new AmqpsReceiver(hostName, userName, deviceId, sasToken);

        CompletableFuture<Boolean> future = receiver.open();
        future.join();

        new Verifications()
        {
            {
                receiver.close();
                mockSemaphore.release();
            }
        };
    }

    // Tests_SRS_AMQPSRECEIVER_14_015: [The function shall attempt to remove a message from the message queue.]
    @Test
    public void consumeMessageAttemptsToRemoveMessageFromQueue() throws IOException, InterruptedException{
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";

        final AmqpsReceiver receiver = new AmqpsReceiver(hostName, userName, deviceId, sasToken);

        Deencapsulation.setField(receiver, "messageQueue", mockMessageQueue);

        receiver.open();
        receiver.consumeMessage();

        new Verifications()
        {
            {
                mockMessageQueue.remove();
            }
        };
    }

    // Tests_SRS_AMQPSRECEIVER_14_016: [The function shall return the message if one was pulled from the queue, otherwise it shall return null.]
    @Test
    public void consumeMessageReturnsNullIfNoMessage() throws IOException, InterruptedException{
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";

        new NonStrictExpectations()
        {
            {
                mockMessageQueue.remove();
                result = new NoSuchElementException();
            }
        };

        final AmqpsReceiver receiver = new AmqpsReceiver(hostName, userName, deviceId, sasToken);

        Deencapsulation.setField(receiver, "messageQueue", mockMessageQueue);

        receiver.open();
        assertNull(receiver.consumeMessage());
    }

    // Tests_SRS_AMQPSRECEIVER_14_014: [If the AmqpsReceiverHandler has not been initialized, the function shall throw a new IOException.]
    @Test(expected = IOException.class)
    public void consumeMessageThrowsIOExceptionIfHandlerNotInit() throws IOException{
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";

        final AmqpsReceiver receiver = new AmqpsReceiver(hostName, userName, deviceId, sasToken);

        receiver.consumeMessage();
    }

    // Tests_SRS_AMQPSRECEIVER_14_017: [The event handler shall set the member AmqpsReceiverHandler object to handle the connection events.]
    @Test
    public void onReactorInitSetsConnectionToHandler() throws IOException, InterruptedException {
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";

        new NonStrictExpectations()
        {
            {
                new AmqpsReceiverHandler(anyString, anyString, anyString, anyString, (AmqpsReceiver)any);
                result = mockHandler;
                mockEvent.getReactor();
                result = mockReactor;
            }
        };

        final AmqpsReceiver receiver = new AmqpsReceiver(hostName, userName, deviceId, sasToken);

        receiver.open();
        receiver.onReactorInit(mockEvent);

        new Verifications()
        {
            {
                mockEvent.getReactor();
                mockReactor.connection(mockHandler);
            }
        };
    }

    //HandlerExceptionReactor implemented to simply throw a HandlerException whenever run() is called.
    class HandlerExceptionReactor implements Reactor{
        protected HandlerExceptionReactor(IO io) throws IOException {}
        protected HandlerExceptionReactor() throws IOException{}

        @Override
        public long mark() {return 0;}

        @Override
        public long now() {return 0;}

        @Override
        public Record attachments() {return null;}

        @Override
        public void setTimeout(long timeout) {}

        @Override
        public long getTimeout() {return 0;}

        @Override
        public Handler getGlobalHandler() {return null;}

        @Override
        public void setGlobalHandler(Handler handler) {}

        @Override
        public Handler getHandler() {return null;}

        @Override
        public void setHandler(Handler handler) {}

        @Override
        public Set<ReactorChild> children() {return null;}

        @Override
        public Collector collector() {return null;}

        @Override
        public Selectable selectable() {return null;}

        @Override
        public void update(Selectable selectable) {}

        @Override
        public void yield() {}

        @Override
        public boolean quiesced() {return false;}

        @Override
        public boolean process() throws HandlerException {return false;}

        @Override
        public void wakeup() {}

        @Override
        public void start() {}

        @Override
        public void stop() throws HandlerException {}

        @Override
        public void run(){throw new HandlerException(null, null);}

        @Override
        public Task schedule(int delay, Handler handler) {return null;}

        @Override
        public Connection connection(Handler handler) {return null;}

        @Override
        public Acceptor acceptor(String host, int port) throws IOException {return null;}

        @Override
        public Acceptor acceptor(String host, int port, Handler handler) throws IOException {return null;}

        @Override
        public void free() {    }
    }
}
