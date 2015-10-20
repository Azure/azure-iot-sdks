/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.transport.amqps;

import mockit.Expectations;
import mockit.Mocked;
import mockit.integration.junit4.JMockit;
import org.apache.qpid.proton.Proton;
import org.apache.qpid.proton.engine.Connection;
import org.apache.qpid.proton.engine.Event;
import org.apache.qpid.proton.engine.Session;
import org.apache.qpid.proton.reactor.Reactor;
import org.junit.Test;
import org.junit.runner.RunWith;

import java.io.IOException;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNull;

/** Unit tests for AmqpSend */
@RunWith(JMockit.class)
public class AmqpSendTest
{
    @Mocked Proton proton;
    @Mocked Reactor reactor;
    @Mocked Event event;
    @Mocked Connection connection;
    @Mocked Session session;

    // Tests_SRS_SERVICE_SDK_JAVA_AMQPSEND_12_002: [The constructor shall copy all input parameters to private member variables for event processing]
    @Test
    public void constructor_copies_params_to_members()
    {
        String hostName = "aaa";
        String userName = "bbb";
        String sasToken = "ccc";

        AmqpSend amqpSend = new AmqpSend(hostName, userName, sasToken);

        String _hostName = amqpSend.hostName;
        String _userName = amqpSend.userName;
        String _sasToken = amqpSend.sasToken;

        assertEquals(hostName, _hostName);
        assertEquals(userName, _userName);
        assertEquals(sasToken, _sasToken);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_AMQPSEND_12_001: [The constructor shall throw IllegalArgumentException if any of the input parameter is null or empty]
    @Test (expected = IllegalArgumentException.class)
    public void constructor_checks_if_hostName_null()
    {
        String hostName = null;
        String userName = "bbb";
        String sasToken = "ccc";

        AmqpSend amqpSend = new AmqpSend(hostName, userName, sasToken);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_AMQPSEND_12_001: [The constructor shall throw IllegalArgumentException if any of the input parameter is null or empty]
    @Test (expected = IllegalArgumentException.class)
    public void constructor_checks_if_hostName_empty()
    {
        String hostName = "";
        String userName = "bbb";
        String sasToken = "ccc";

        AmqpSend amqpSend = new AmqpSend(hostName, userName, sasToken);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_AMQPSEND_12_001: [The constructor shall throw IllegalArgumentException if any of the input parameter is null or empty]
    @Test (expected = IllegalArgumentException.class)
    public void constructor_checks_if_userName_null()
    {
        String hostName = "aaa";
        String userName = null;
        String sasToken = "ccc";

        AmqpSend amqpSend = new AmqpSend(hostName, userName, sasToken);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_AMQPSEND_12_001: [The constructor shall throw IllegalArgumentException if any of the input parameter is null or empty]
    @Test (expected = IllegalArgumentException.class)
    public void constructor_checks_if_userName_empty()
    {
        String hostName = "aaa";
        String userName = "";
        String sasToken = "ccc";

        AmqpSend amqpSend = new AmqpSend(hostName, userName, sasToken);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_AMQPSEND_12_001: [The constructor shall throw IllegalArgumentException if any of the input parameter is null or empty]
    @Test (expected = IllegalArgumentException.class)
    public void constructor_checks_if_sasToken_null()
    {
        String hostName = "aaa";
        String userName = "bbb";
        String sasToken = null;

        AmqpSend amqpSend = new AmqpSend(hostName, userName, sasToken);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_AMQPSEND_12_001: [The constructor shall throw IllegalArgumentException if any of the input parameter is null or empty]
    @Test (expected = IllegalArgumentException.class)
    public void constructor_checks_if_sasToken_empty()
    {
        String hostName = "aaa";
        String userName = "bbb";
        String sasToken = "";

        AmqpSend amqpSend = new AmqpSend(hostName, userName, sasToken);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_AMQPSEND_12_003: [The event handler shall set the member AmqpsSendHandler object to handle the given connection events]
    // Tests_SRS_SERVICE_SDK_JAVA_AMQPSEND_12_004: [The event handler shall create an AmqpsSendHandler object to handle reactor events]
    @Test
    public void onReactorInit_creates_SendHandler() throws IOException
    {
        String hostName = "aaa";
        String userName = "bbb";
        String sasToken = "ccc";

        AmqpSend amqpSend = new AmqpSend(hostName, userName, sasToken);
        amqpSend.open();

        new Expectations()
        {
            {
                reactor = event.getReactor();
                connection = reactor.connection(amqpSend.amqpSendHandler);
            }
        };
        amqpSend.onReactorInit(event);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_AMQPSEND_12_005: [The event handler shall invalidate the member AmqpsSendHandler object]
    @Test
    public void onReactorInit_invalidates_SendHandler() throws IOException
    {
        String hostName = "aaa";
        String userName = "bbb";
        String sasToken = "ccc";

        AmqpSend amqpSend = new AmqpSend(hostName, userName, sasToken);
        amqpSend.open();
        amqpSend.onReactorInit(event);

        amqpSend.close();
        assertNull(amqpSend.amqpSendHandler);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_AMQPSEND_12_006: [The event handler shall create a binary message with the given content]
    @Test
    public void send_creates_BinaryMessage() throws Exception
    {
        String hostName = "aaa";
        String userName = "bbb";
        String sasToken = "ccc";
        String deviceId = "deviceId";
        String content = "abcdefghijklmnopqrst";

        AmqpSend amqpSend = new AmqpSend(hostName, userName, sasToken);
        amqpSend.open();

        new Expectations()
        {
            {
                amqpSend.amqpSendHandler.createBinaryMessage(deviceId, content);
            }
        };
        amqpSend.send(deviceId, content);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_AMQPSEND_12_007: [The event handler shall initialize the Proton reactor object]
    // Tests_SRS_SERVICE_SDK_JAVA_AMQPSEND_12_008: [The event handler shall start the Proton reactor object]
    @Test
    public void send_initializes_Reactor() throws Exception
    {
        String hostName = "aaa";
        String userName = "bbb";
        String sasToken = "ccc";
        String deviceId = "deviceId";
        String content = "abcdefghijklmnopqrst";

        AmqpSend amqpSend = new AmqpSend(hostName, userName, sasToken);
        amqpSend.open();

        new Expectations()
        {
            {
                reactor = proton.reactor(amqpSend);
                reactor.run();
            }
        };
        amqpSend.send(deviceId, content);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_AMQPSEND_12_009: [The event handler shall throw IOException if the send handler object is not initialized]
    @Test (expected = IOException.class)
    public void send_throwsIOException_when_open_has_not_been_called() throws Exception
    {
        String hostName = "aaa";
        String userName = "bbb";
        String sasToken = "ccc";
        String deviceId = "deviceId";
        String content = "abcdefghijklmnopqrst";

        AmqpSend amqpSend = new AmqpSend(hostName, userName, sasToken);
        amqpSend.send(deviceId, content);
    }
}
