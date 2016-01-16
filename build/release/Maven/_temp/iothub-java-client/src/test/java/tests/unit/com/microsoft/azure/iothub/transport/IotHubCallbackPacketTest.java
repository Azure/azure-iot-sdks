// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

package tests.unit.com.microsoft.azure.iothub.transport;

import static org.junit.Assert.assertThat;
import static org.hamcrest.CoreMatchers.is;
import static org.hamcrest.Matchers.everyItem;
import static org.hamcrest.Matchers.isIn;

import com.microsoft.azure.iothub.IotHubStatusCode;
import com.microsoft.azure.iothub.IotHubEventCallback;
import com.microsoft.azure.iothub.transport.IotHubCallbackPacket;

import mockit.Mocked;
import org.junit.Test;

import java.util.HashMap;
import java.util.Map;
import java.util.Set;

/** Unit tests for IotHubCallbackPacket. */
public class IotHubCallbackPacketTest
{
    @Mocked
    IotHubEventCallback mockCallback;

    // Tests_SRS_IOTHUBCALLBACKPACKET_11_001: [The constructor shall save the status, callback, and callback context.]
    // Tests_SRS_IOTHUBCALLBACKPACKET_11_002: [The function shall return the status given in the constructor.]
    @Test
    public void getStatusReturnsStatus()
    {
        final IotHubStatusCode status = IotHubStatusCode.BAD_FORMAT;
        final Map<String, Object> context = new HashMap<>();

        IotHubCallbackPacket packet =
                new IotHubCallbackPacket(status, mockCallback, context);
        IotHubStatusCode testStatus = packet.getStatus();

        final IotHubStatusCode expectedStatus = status;
        assertThat(testStatus, is(expectedStatus));
    }

    // Tests_SRS_IOTHUBCALLBACKPACKET_11_001: [The constructor shall save the status, callback, and callback context.]
    // Tests_SRS_IOTHUBCALLBACKPACKET_11_003: [The function shall return the callback given in the constructor.]
    @Test
    public void getCallbackReturnsCallback()
    {
        final IotHubStatusCode status =
                IotHubStatusCode.HUB_OR_DEVICE_ID_NOT_FOUND;
        final Map<String, Object> context = new HashMap<>();

        IotHubCallbackPacket packet =
                new IotHubCallbackPacket(status, mockCallback, context);
        IotHubEventCallback testCallback = packet.getCallback();

        final IotHubEventCallback expectedCallback = mockCallback;
        assertThat(testCallback, is(mockCallback));
    }

    // Tests_SRS_IOTHUBCALLBACKPACKET_11_001: [The constructor shall save the status, callback, and callback context.]
    // Tests_SRS_IOTHUBCALLBACKPACKET_11_004: [The function shall return the callback context given in the constructor.]
    @Test
    public void getContextReturnsContext()
    {
        final IotHubStatusCode status = IotHubStatusCode.OK;
        final Map<String, Object> context = new HashMap<>();
        final String key = "test-key";
        final String value = "test-value";
        context.put(key, value);

        IotHubCallbackPacket packet =
                new IotHubCallbackPacket(status, mockCallback, context);
        Map<String, Object> testContext =
                (Map<String, Object>) packet.getContext();
        Set<Map.Entry<String, Object>> testEntrySet = testContext.entrySet();

        final Set<Map.Entry<String, Object>> expectedEntrySet =
                context.entrySet();
        assertThat(testEntrySet, everyItem(isIn(expectedEntrySet)));
    }
}
