// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

package tests.unit.com.microsoft.azure.iothub.transport;

import static org.junit.Assert.assertThat;
import static org.hamcrest.CoreMatchers.is;
import static org.hamcrest.Matchers.everyItem;
import static org.hamcrest.Matchers.isIn;

import com.microsoft.azure.iothub.IotHubEventCallback;

import com.microsoft.azure.iothub.Message;
import com.microsoft.azure.iothub.transport.IotHubOutboundPacket;
import mockit.Mocked;
import org.junit.Test;

import java.util.HashMap;
import java.util.Map;
import java.util.Set;

/** Unit tests for IotHubOutboundPacket. */
public class IotHubOutboundPacketTest
{
    @Mocked
    Message mockMsg;
    @Mocked
    IotHubEventCallback mockCallback;

    // Tests_SRS_IOTHUBOUTBOUNDPACKET_11_001: [The constructor shall save the message, callback, and callback context.]
    // Tests_SRS_IOTHUBOUTBOUNDPACKET_11_002: [The function shall return the message given in the constructor.]
    @Test
    public void getMessageReturnsMessage()
    {
        final Map<String, Object> context = new HashMap<>();

        IotHubOutboundPacket packet =
                new IotHubOutboundPacket(mockMsg, mockCallback, context);
        Message testMsg = packet.getMessage();

        final Message expectedMsg = mockMsg;
        assertThat(testMsg, is(expectedMsg));
    }

    // Tests_SRS_IOTHUBOUTBOUNDPACKET_11_001: [The constructor shall save the message, callback, and callback context.]
    // Tests_SRS_IOTHUBOUTBOUNDPACKET_11_003: [The function shall return the callback given in the constructor.]
    @Test
    public void getCallbackReturnsCallback()
    {
        final Map<String, Object> context = new HashMap<>();

        IotHubOutboundPacket packet =
                new IotHubOutboundPacket(mockMsg, mockCallback, context);
        IotHubEventCallback testCallback = packet.getCallback();

        final IotHubEventCallback expectedCallback = mockCallback;
        assertThat(testCallback, is(expectedCallback));
    }

    // Tests_SRS_IOTHUBOUTBOUNDPACKET_11_001: [The constructor shall save the message, callback, and callback context.]
    // Tests_SRS_IOTHUBOUTBOUNDPACKET_11_004: [The function shall return the callback context given in the constructor.] 
    @Test
    public void getContextReturnsContext()
    {
        final Map<String, Object> context = new HashMap<>();
        final String key = "test-key";
        final String value = "test-value";
        context.put(key, value);

        IotHubOutboundPacket packet =
                new IotHubOutboundPacket(mockMsg, mockCallback, context);
        Map<String, Object> testContext =
                (Map<String, Object>) packet.getContext();
        Set<Map.Entry<String, Object>> testEntrySet = testContext.entrySet();

        final Set<Map.Entry<String, Object>> expectedEntrySet =
                context.entrySet();
        assertThat(testEntrySet, everyItem(isIn(expectedEntrySet)));
    }
}
