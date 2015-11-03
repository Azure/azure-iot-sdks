/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package tests.unit.com.microsoft.azure.iothub.transport.amqps;

import com.microsoft.azure.iothub.transport.amqps.AmqpsSender;
import com.microsoft.azure.iothub.transport.amqps.AmqpsSenderHandler;
import mockit.Mocked;
import mockit.NonStrictExpectations;
import mockit.Verifications;
import org.apache.qpid.proton.Proton;
import org.apache.qpid.proton.engine.BaseHandler;
import org.apache.qpid.proton.engine.Connection;
import org.apache.qpid.proton.engine.Event;
import org.apache.qpid.proton.engine.Session;
import org.apache.qpid.proton.reactor.Reactor;
import org.junit.Test;

import java.io.IOException;

import static org.junit.Assert.assertNull;

public class AmqpsSenderTest {

    @Mocked
    protected Proton mockProton;
    @Mocked
    protected Reactor mockReactor;
    @Mocked
    protected Event mockEvent;
    @Mocked
    protected Connection mockConnection;
    @Mocked
    protected Session mockSession;
    @Mocked
    protected AmqpsSenderHandler mockHandler;

    // Tests_SRS_AMQPSSENDER_14_002: [The Constructor shall copy all input parameters to private member variables.]
    // Tests_SRS_AMQPSSENDER_14_003: [The function shall initialize its AmqpsSenderHandler using the saved host name, user name, device ID and sas token.]
    @Test
    public void constructorCopiesAllData(){
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";

        AmqpsSender sender = new AmqpsSender(hostName, userName, deviceId, sasToken);
        sender.open();

        new Verifications()
        {
            {
                new AmqpsSenderHandler(hostName, userName, deviceId, sasToken);
            }
        };
    }

    // Tests_SRS_AMQPSSENDER_14_001: [The constructor shall throw a new IllegalArgumentException if any of the input parameters is null or empty.]
    @Test(expected = IllegalArgumentException.class)
    public void constructorThrowsIllegalArgumentExceptionIfHostNameIsEmpty(){
        final String hostName = "";
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";

        AmqpsSender sender = new AmqpsSender(hostName, userName, deviceId, sasToken);
    }

    // Tests_SRS_AMQPSSENDER_14_001: [The constructor shall throw a new IllegalArgumentException if any of the input parameters is null or empty.]
    @Test(expected = IllegalArgumentException.class)
    public void constructorThrowsIllegalArgumentExceptionIfHostNameIsNull(){
        final String hostName = null;
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";

        AmqpsSender sender = new AmqpsSender(hostName, userName, deviceId, sasToken);
    }

    // Tests_SRS_AMQPSSENDER_14_001: [The constructor shall throw a new IllegalArgumentException if any of the input parameters is null or empty.]
    @Test(expected = IllegalArgumentException.class)
    public void constructorThrowsIllegalArgumentExceptionIfDeviceIDIsEmpty(){
        final String hostName = "test.host.name";
        final String deviceId = "";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";

        AmqpsSender sender = new AmqpsSender(hostName, userName, deviceId, sasToken);
    }

    // Tests_SRS_AMQPSSENDER_14_001: [The constructor shall throw a new IllegalArgumentException if any of the input parameters is null or empty.]
    @Test(expected = IllegalArgumentException.class)
    public void constructorThrowsIllegalArgumentExceptionIfDeviceIDIsNull(){
        final String hostName = "test.host.name";
        final String deviceId = null;
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";

        AmqpsSender sender = new AmqpsSender(hostName, userName, deviceId, sasToken);
    }

    // Tests_SRS_AMQPSSENDER_14_001: [The constructor shall throw a new IllegalArgumentException if any of the input parameters is null or empty.]
    @Test(expected = IllegalArgumentException.class)
    public void constructorThrowsIllegalArgumentExceptionIfUserNameIsEmpty(){
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = "";
        final String sasToken = "test-token";

        AmqpsSender sender = new AmqpsSender(hostName, userName, deviceId, sasToken);
    }

    // Tests_SRS_AMQPSSENDER_14_001: [The constructor shall throw a new IllegalArgumentException if any of the input parameters is null or empty.]
    @Test(expected = IllegalArgumentException.class)
    public void constructorThrowsIllegalArgumentExceptionIfUserNameIsNull(){
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = null;
        final String sasToken = "test-token";

        AmqpsSender sender = new AmqpsSender(hostName, userName, deviceId, sasToken);
    }

    // Tests_SRS_AMQPSSENDER_14_001: [The constructor shall throw a new IllegalArgumentException if any of the input parameters is null or empty.]
    @Test(expected = IllegalArgumentException.class)
    public void constructorThrowsIllegalArgumentExceptionIfSasTokenIsEmpty(){
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "";

        AmqpsSender sender = new AmqpsSender(hostName, userName, deviceId, sasToken);
    }

    // Tests_SRS_AMQPSSENDER_14_001: [The constructor shall throw a new IllegalArgumentException if any of the input parameters is null or empty.]
    @Test(expected = IllegalArgumentException.class)
    public void constructorThrowsIllegalArgumentExceptionIfSasTokenIsNull(){
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = null;

        AmqpsSender sender = new AmqpsSender(hostName, userName, deviceId, sasToken);
    }

    // Tests_SRS_AMQPSSENDER_14_004: [The function shall invalidate its AmqpsSenderHandler.]
    // Tests_SRS_AMQPSSENDER_14_005: [If the AmqpsSenderHandler has not been initialized, the function shall throw a new IOException.]
    @Test(expected = IOException.class)
    public void closeInvalidatesSenderHandlerObject() throws IOException{
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";
        final byte[] msgBody = { 0x61, 0x62, 0x63 };

        AmqpsSender sender = new AmqpsSender(hostName, userName, deviceId, sasToken);
        sender.open();
        sender.onReactorInit(mockEvent);
        sender.close();
        sender.send(msgBody, null);
    }

    // Tests_SRS_AMQPSSENDER_14_006: [The function shall create a binary message with the given content and messageId.]
    @Test
    public void sendCreatesBinaryMessageWithContentAndID() throws IOException{
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";

        final byte[] msgBody = { 0x61, 0x62, 0x63 };
        final Object messageId = "123";

        AmqpsSender sender = new AmqpsSender(hostName, userName, deviceId, sasToken);
        sender.open();
        sender.send(msgBody, messageId);

        new Verifications()
        {
            {
                mockHandler.createBinaryMessage(msgBody, messageId);
            }
        };
    }

    // Tests_SRS_AMQPSSENDER_14_007: [The function shall initialize the Reactor (Proton) object.]
    // Tests_SRS_AMQPSSENDER_14_008: [The function shall start running the Reactor (Proton) object.]
    @Test
    public void sendInitializesandRunsReactorObject() throws IOException {
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";

        final AmqpsSender sender = new AmqpsSender(hostName, userName, deviceId, sasToken);
        sender.open();
        sender.send(null, null);

        new Verifications()
        {
            {
                mockProton.reactor(sender);
                mockReactor.run();
            }
        };
    }

    // Tests_SRS_AMQPSSENDER_14_011: [If a messageId is not provided, the function shall create a binary message using the given content and a null messageId.]
    @Test
    public void sendCreatesBinaryMessageWithNullMessageIDIfEmpty() throws IOException{
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";
        final byte[] msgBody = { 0x61, 0x62, 0x63 };

        final AmqpsSender sender = new AmqpsSender(hostName, userName, deviceId, sasToken);
        sender.open();
        sender.send(msgBody);

        new Verifications()
        {
            {
                mockHandler.createBinaryMessage(msgBody, null);
            }
        };
    }

    // Tests_SRS_AMQPSSENDER_14_009: [The event handler shall set the member AmqpsSenderHandler object to handle the connection events.]
    @Test
    public void onReactorInitSetsConnectionToHandler(){
        final String hostName = "test.host.name";
        final String deviceId = "test-deviceId";
        final String userName = "test-deviceId@sas.test.host.name";
        final String sasToken = "test-token";

        new NonStrictExpectations()
        {
            {
                new AmqpsSenderHandler(anyString, anyString, anyString, anyString);
                result = mockHandler;
            }
        };

        final AmqpsSender sender = new AmqpsSender(hostName, userName, deviceId, sasToken);
        sender.open();
        sender.onReactorInit(mockEvent);

        new Verifications()
        {
            {
                mockEvent.getReactor();
                mockReactor.connection(mockHandler);
            }
        };
    }
}
