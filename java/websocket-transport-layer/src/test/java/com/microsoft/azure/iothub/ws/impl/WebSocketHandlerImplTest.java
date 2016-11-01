/*
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 *
*/
package com.microsoft.azure.iothub.ws.impl;

import com.microsoft.azure.iothub.ws.WebSocketHandler;
import com.microsoft.azure.iothub.ws.WebSocketHeader;
import org.junit.Test;

import javax.xml.bind.DatatypeConverter;
import java.nio.ByteBuffer;
import java.security.SecureRandom;
import java.util.*;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;
import static org.mockito.Mockito.*;

public class WebSocketHandlerImplTest
{
    @Test
    public void testCreateUpgradeRequest()
    {
        String hostName = "host_XXX";
        String webSocketPath = "path1/path2";
        int webSocketPort = 1234567890;
        String webSocketProtocol = "subprotocol_name";
        Map<String, String> additionalHeaders = new HashMap<String, String>();
        additionalHeaders.put("header1", "content1");
        additionalHeaders.put("header2", "content2");
        additionalHeaders.put("header3", "content3");

        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();

        String actual = webSocketHandler.createUpgradeRequest(hostName, webSocketPath, webSocketPort, webSocketProtocol, additionalHeaders);

        Boolean isLineCountOk = false;
        Boolean isStatusLineOk = false;
        Boolean isUpgradeHeaderOk = false;
        Boolean isConnectionHeaderOk = false;
        Boolean isWebSocketVersionHeaderOk = false;
        Boolean isWebSocketKeyHeaderOk = false;
        Boolean isWebSocketProtocolHeaderOk = false;
        Boolean isHostHeaderOk = false;
        Boolean isAdditonalHeader1Ok = false;
        Boolean isAdditonalHeader2Ok = false;
        Boolean isAdditonalHeader3Ok = false;

        Scanner scanner = new Scanner(actual);

        int lineCount = 0;
        while (scanner.hasNextLine())
        {
            lineCount++;

            String line = scanner.nextLine();
            if (line.equals("GET https://" + hostName + "/" + webSocketPath + " HTTP/1.1"))
            {
                isStatusLineOk = true;
                continue;
            }
            if (line.equals("Connection: Upgrade,Keep-Alive"))
            {
                isConnectionHeaderOk = true;
                continue;
            }
            if (line.equals("Upgrade: websocket"))
            {
                isUpgradeHeaderOk = true;
                continue;
            }
            if (line.equals("Sec-WebSocket-Version: 13"))
            {
                isWebSocketVersionHeaderOk = true;
                continue;
            }
            if (line.startsWith("Sec-WebSocket-Key: "))
            {
                String keyBase64 = line.substring(19);
                if (keyBase64.length() == 24)
                {
                    byte[] decoded = DatatypeConverter.parseBase64Binary(keyBase64);

                    if (decoded.length == 16)
                    {
                        isWebSocketKeyHeaderOk = true;
                    }
                }
                continue;
            }
            if (line.equals("Sec-WebSocket-Protocol: " + webSocketProtocol))
            {
                isWebSocketProtocolHeaderOk = true;
                continue;
            }
            if (line.equals("Host: host_XXX"))
            {
                isHostHeaderOk = true;
                continue;
            }
            if (line.equals("header1: content1"))
            {
                isAdditonalHeader1Ok = true;
                continue;
            }
            if (line.equals("header2: content2"))
            {
                isAdditonalHeader2Ok = true;
                continue;
            }
            if (line.equals("header3: content3"))
            {
                isAdditonalHeader3Ok = true;
                continue;
            }
        }
        if (lineCount == 11)
        {
            isLineCountOk = true;
        }

        assertTrue(isLineCountOk);
        assertTrue(isStatusLineOk);
        assertTrue(isUpgradeHeaderOk);
        assertTrue(isConnectionHeaderOk);
        assertTrue(isWebSocketVersionHeaderOk);
        assertTrue(isWebSocketKeyHeaderOk);
        assertTrue(isWebSocketProtocolHeaderOk);
        assertTrue(isHostHeaderOk);
        assertTrue(isAdditonalHeader1Ok);
        assertTrue(isAdditonalHeader2Ok);
        assertTrue(isAdditonalHeader3Ok);
    }

    @Test
    public void testCreateUpgradeRequest_verify_subsequent_call()
    {
        String hostName = "host_XXX";
        String webSocketPath = "path1/path2";
        int webSocketPort = 1234567890;
        String webSocketProtocol = "subprotocol_name";
        Map<String, String> additionalHeaders = new HashMap<String, String>();
        additionalHeaders.put("header1", "content1");
        additionalHeaders.put("header2", "content2");
        additionalHeaders.put("header3", "content3");

        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();
        WebSocketHandlerImpl spyWebSocketHandler = spy(webSocketHandler);

        WebSocketUpgrade mockWebSocketUpgrade = mock(WebSocketUpgrade.class);

        doReturn(mockWebSocketUpgrade).when(spyWebSocketHandler).createWebSocketUpgrade(hostName, webSocketPath, webSocketPort, webSocketProtocol, additionalHeaders);
        spyWebSocketHandler.createUpgradeRequest(hostName, webSocketPath, webSocketPort, webSocketProtocol, additionalHeaders);
        verify(spyWebSocketHandler, times(1)).createWebSocketUpgrade(hostName, webSocketPath, webSocketPort, webSocketProtocol, additionalHeaders);
        verify(mockWebSocketUpgrade, times(1)).createUpgradeRequest();
    }

    @Test
    public void testCreatePong()
    {
        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();

        ByteBuffer ping = ByteBuffer.allocate(10);
        ByteBuffer pong = ByteBuffer.allocate(10);

        byte[] buffer = new byte[10];
        buffer[0] = WebSocketHeader.FINBIT_MASK | WebSocketHeader.OPCODE_PING;
        ping.put(buffer);
        ping.flip();
        webSocketHandler.createPong(ping, pong);

        int actual = pong.array()[0];
        int expected = WebSocketHeader.FINBIT_MASK | WebSocketHeader.OPCODE_PONG;

        assertEquals(actual, expected);
    }

    @Test(expected = IllegalArgumentException.class)
    public void testCreatePong_ping_null()
    {
        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();

        ByteBuffer pong = ByteBuffer.allocate(10);

        webSocketHandler.createPong(null, pong);
    }

    @Test(expected = IllegalArgumentException.class)
    public void testCreatePong_pong_null()
    {
        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();

        ByteBuffer ping = ByteBuffer.allocate(10);

        webSocketHandler.createPong(ping, null);
    }

    @Test(expected = IllegalArgumentException.class)
    public void testCreatePong_pong_capacity_insufficient()
    {
        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();

        ByteBuffer ping = ByteBuffer.allocate(10);
        ByteBuffer pong = ByteBuffer.allocate(9);

        webSocketHandler.createPong(ping, pong);
    }

    @Test
    public void testCreatePong_ping_no_remaining()
    {
        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();

        ByteBuffer ping = ByteBuffer.allocate(10);
        ByteBuffer pong = ByteBuffer.allocate(10);

        ping.flip();

        webSocketHandler.createPong(ping, pong);

        assertEquals(0, pong.limit());
        assertEquals(0, pong.position());
    }

    @Test
    public void testValidateUpgradeReply()
    {
        String hostName = "host_XXX";
        String webSocketPath = "path1/path2";
        int webSocketPort = 1234567890;
        String webSocketProtocol = "subprotocol_name";
        Map<String, String> additionalHeaders = new HashMap<String, String>();
        additionalHeaders.put("header1", "content1");
        additionalHeaders.put("header2", "content2");
        additionalHeaders.put("header3", "content3");

        ByteBuffer buffer = ByteBuffer.allocate(10);
        byte[] data = new byte[buffer.remaining()];

        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();
        WebSocketHandlerImpl spyWebSocketHandler = spy(webSocketHandler);

        WebSocketUpgrade mockWebSocketUpgrade = mock(WebSocketUpgrade.class);

        doReturn(mockWebSocketUpgrade).when(spyWebSocketHandler).createWebSocketUpgrade(hostName, webSocketPath, webSocketPort, webSocketProtocol, additionalHeaders);
        doReturn(true).when(mockWebSocketUpgrade).validateUpgradeReply(data);

        spyWebSocketHandler.createUpgradeRequest(hostName, webSocketPath, webSocketPort, webSocketProtocol, additionalHeaders);
        assertTrue(spyWebSocketHandler.validateUpgradeReply(buffer));
        assertFalse(mockWebSocketUpgrade == null);
        verify(mockWebSocketUpgrade, times(1)).validateUpgradeReply(data);
    }

    @Test
    public void testValidateUpgradeReply_no_remaining()
    {
        String hostName = "host_XXX";
        String webSocketPath = "path1/path2";
        int webSocketPort = 1234567890;
        String webSocketProtocol = "subprotocol_name";
        Map<String, String> additionalHeaders = new HashMap<String, String>();
        additionalHeaders.put("header1", "content1");
        additionalHeaders.put("header2", "content2");
        additionalHeaders.put("header3", "content3");

        ByteBuffer buffer = ByteBuffer.allocate(10);
        byte[] data = new byte[buffer.remaining()];
        buffer.limit(0);

        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();
        WebSocketHandlerImpl spyWebSocketHandler = spy(webSocketHandler);

        WebSocketUpgrade mockWebSocketUpgrade = mock(WebSocketUpgrade.class);

        doReturn(mockWebSocketUpgrade).when(spyWebSocketHandler).createWebSocketUpgrade(hostName, webSocketPath, webSocketPort, webSocketProtocol, additionalHeaders);
        doReturn(true).when(mockWebSocketUpgrade).validateUpgradeReply(data);

        spyWebSocketHandler.createUpgradeRequest(hostName, webSocketPath, webSocketPort, webSocketProtocol, additionalHeaders);
        assertFalse(spyWebSocketHandler.validateUpgradeReply(buffer));
        verify(mockWebSocketUpgrade, times(0)).validateUpgradeReply(data);
    }

    @Test
    public void testValidateUpgradeReply_websocketupgrade_null()
    {
        ByteBuffer buffer = ByteBuffer.allocate(10);
        byte[] data = new byte[buffer.remaining()];

        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();
        WebSocketHandlerImpl spyWebSocketHandler = spy(webSocketHandler);

        WebSocketUpgrade mockWebSocketUpgrade = mock(WebSocketUpgrade.class);

        assertFalse(spyWebSocketHandler.validateUpgradeReply(buffer));
        verify(mockWebSocketUpgrade, times(0)).validateUpgradeReply(data);
    }

    @Test
    public void testWrapBuffer_short_payload()
    {
        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();
        WebSocketHandlerImpl spyWebSocketHandler = spy(webSocketHandler);

        int payloadLength = 100;
        int messageLength = payloadLength + WebSocketHeader.MIN_HEADER_LENGTH_MASKED;

        byte[] maskingKey = new byte[]{0x01, 0x02, 0x03, 0x04};

        byte[] data = new byte[payloadLength];
        Random random = new SecureRandom();
        random.nextBytes(data);

        ByteBuffer srcBuffer = ByteBuffer.allocate(payloadLength);
        ByteBuffer dstBuffer = ByteBuffer.allocate(messageLength);
        srcBuffer.put(data);
        srcBuffer.flip();

        int expectedHeaderSize = WebSocketHeader.MIN_HEADER_LENGTH_MASKED;
        byte[] expected = new byte[messageLength];
        expected[0] = (byte) (WebSocketHeader.FINBIT_MASK | WebSocketHeader.OPCODE_BINARY);
        expected[1] = (byte) (WebSocketHeader.MASKBIT_MASK | payloadLength);
        expected[2] = maskingKey[0];
        expected[3] = maskingKey[1];
        expected[4] = maskingKey[2];
        expected[5] = maskingKey[3];

        for (int i = 0; i < srcBuffer.limit(); i++)
        {
            byte nextByte = srcBuffer.get();
            nextByte ^= maskingKey[i % 4];
            expected[i + WebSocketHeader.MIN_HEADER_LENGTH_MASKED] = nextByte;
        }
        srcBuffer.flip();

        doReturn(maskingKey).when(spyWebSocketHandler).createRandomMaskingKey();

        spyWebSocketHandler.wrapBuffer(srcBuffer, dstBuffer);
        dstBuffer.flip();

        byte[] actual = dstBuffer.array();

        assertEquals("invalid content length", srcBuffer.limit() + expectedHeaderSize, dstBuffer.limit());

        assertEquals("first byte mismatch", expected[0], actual[0]);
        assertEquals("second byte mismatch", expected[1], actual[1]);

        assertEquals("masking key mismatch 1", maskingKey[0], actual[2]);
        assertEquals("masking key mismatch 2", maskingKey[1], actual[3]);
        assertEquals("masking key mismatch 3", maskingKey[2], actual[4]);
        assertEquals("masking key mismatch 4", maskingKey[3], actual[5]);

        assertTrue(Arrays.equals(expected, actual));
    }

    @Test
    public void testWrapBuffer_short_payload_min()
    {
        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();
        WebSocketHandlerImpl spyWebSocketHandler = spy(webSocketHandler);

        int payloadLength = 1;
        int messageLength = payloadLength + WebSocketHeader.MIN_HEADER_LENGTH_MASKED;

        byte[] maskingKey = new byte[]{0x01, 0x02, 0x03, 0x04};

        byte[] data = new byte[payloadLength];
        Random random = new SecureRandom();
        random.nextBytes(data);

        ByteBuffer srcBuffer = ByteBuffer.allocate(payloadLength);
        ByteBuffer dstBuffer = ByteBuffer.allocate(messageLength);
        srcBuffer.put(data);
        srcBuffer.flip();

        int expectedHeaderSize = WebSocketHeader.MIN_HEADER_LENGTH_MASKED;
        byte[] expected = new byte[messageLength];
        expected[0] = (byte) (WebSocketHeader.FINBIT_MASK | WebSocketHeader.OPCODE_BINARY);
        expected[1] = (byte) (WebSocketHeader.MASKBIT_MASK | payloadLength);
        expected[2] = maskingKey[0];
        expected[3] = maskingKey[1];
        expected[4] = maskingKey[2];
        expected[5] = maskingKey[3];

        for (int i = 0; i < srcBuffer.limit(); i++)
        {
            byte nextByte = srcBuffer.get();
            nextByte ^= maskingKey[i % 4];
            expected[i + WebSocketHeader.MIN_HEADER_LENGTH_MASKED] = nextByte;
        }
        srcBuffer.flip();

        doReturn(maskingKey).when(spyWebSocketHandler).createRandomMaskingKey();

        spyWebSocketHandler.wrapBuffer(srcBuffer, dstBuffer);
        dstBuffer.flip();

        byte[] actual = dstBuffer.array();

        assertEquals("invalid content length", srcBuffer.limit() + expectedHeaderSize, dstBuffer.limit());

        assertEquals("first byte mismatch", expected[0], actual[0]);
        assertEquals("second byte mismatch", expected[1], actual[1]);

        assertEquals("masking key mismatch 1", maskingKey[0], actual[2]);
        assertEquals("masking key mismatch 2", maskingKey[1], actual[3]);
        assertEquals("masking key mismatch 3", maskingKey[2], actual[4]);
        assertEquals("masking key mismatch 4", maskingKey[3], actual[5]);

        assertTrue(Arrays.equals(expected, actual));
    }

    @Test
    public void testWrapBuffer_short_payload_max()
    {
        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();
        WebSocketHandlerImpl spyWebSocketHandler = spy(webSocketHandler);

        int payloadLength = WebSocketHeader.PAYLOAD_SHORT_MAX;
        int messageLength = payloadLength + WebSocketHeader.MIN_HEADER_LENGTH_MASKED;

        byte[] maskingKey = new byte[]{0x01, 0x02, 0x03, 0x04};

        byte[] data = new byte[payloadLength];
        Random random = new SecureRandom();
        random.nextBytes(data);

        ByteBuffer srcBuffer = ByteBuffer.allocate(payloadLength);
        ByteBuffer dstBuffer = ByteBuffer.allocate(messageLength);
        srcBuffer.put(data);
        srcBuffer.flip();

        int expectedHeaderSize = WebSocketHeader.MIN_HEADER_LENGTH_MASKED;
        byte[] expected = new byte[messageLength];
        expected[0] = (byte) (WebSocketHeader.FINBIT_MASK | WebSocketHeader.OPCODE_BINARY);
        expected[1] = (byte) (WebSocketHeader.MASKBIT_MASK | payloadLength);
        expected[2] = maskingKey[0];
        expected[3] = maskingKey[1];
        expected[4] = maskingKey[2];
        expected[5] = maskingKey[3];

        for (int i = 0; i < srcBuffer.limit(); i++)
        {
            byte nextByte = srcBuffer.get();
            nextByte ^= maskingKey[i % 4];
            expected[i + WebSocketHeader.MIN_HEADER_LENGTH_MASKED] = nextByte;
        }
        srcBuffer.flip();

        doReturn(maskingKey).when(spyWebSocketHandler).createRandomMaskingKey();

        spyWebSocketHandler.wrapBuffer(srcBuffer, dstBuffer);
        dstBuffer.flip();

        byte[] actual = dstBuffer.array();

        assertEquals("invalid content length", srcBuffer.limit() + expectedHeaderSize, dstBuffer.limit());

        assertEquals("first byte mismatch", expected[0], actual[0]);
        assertEquals("second byte mismatch", expected[1], actual[1]);

        assertEquals("masking key mismatch 1", maskingKey[0], actual[2]);
        assertEquals("masking key mismatch 2", maskingKey[1], actual[3]);
        assertEquals("masking key mismatch 3", maskingKey[2], actual[4]);
        assertEquals("masking key mismatch 4", maskingKey[3], actual[5]);

        assertTrue(Arrays.equals(expected, actual));
    }

    @Test
    public void testWrapBuffer_medium_payload()
    {
        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();
        WebSocketHandlerImpl spyWebSocketHandler = spy(webSocketHandler);

        int payloadLength = WebSocketHeader.PAYLOAD_SHORT_MAX + 100;
        int messageLength = payloadLength + WebSocketHeader.MED_HEADER_LENGTH_MASKED;

        byte[] maskingKey = new byte[]{0x01, 0x02, 0x03, 0x04};

        byte[] data = new byte[payloadLength];
        Random random = new SecureRandom();
        random.nextBytes(data);

        ByteBuffer srcBuffer = ByteBuffer.allocate(payloadLength);
        ByteBuffer dstBuffer = ByteBuffer.allocate(messageLength);
        srcBuffer.put(data);
        srcBuffer.flip();

        int expectedHeaderSize = WebSocketHeader.MED_HEADER_LENGTH_MASKED;
        byte[] expected = new byte[messageLength];
        expected[0] = (byte) (WebSocketHeader.FINBIT_MASK | WebSocketHeader.OPCODE_BINARY);
        expected[1] = (byte) (WebSocketHeader.MASKBIT_MASK | WebSocketHeader.PAYLOAD_EXTENDED_16);

        expected[2] = (byte) (payloadLength >> 8);
        expected[3] = (byte) (payloadLength);

        expected[4] = maskingKey[0];
        expected[5] = maskingKey[1];
        expected[6] = maskingKey[2];
        expected[7] = maskingKey[3];

        for (int i = 0; i < srcBuffer.limit(); i++)
        {
            byte nextByte = srcBuffer.get();
            nextByte ^= maskingKey[i % 4];
            expected[i + WebSocketHeader.MED_HEADER_LENGTH_MASKED] = nextByte;
        }
        srcBuffer.flip();

        doReturn(maskingKey).when(spyWebSocketHandler).createRandomMaskingKey();

        spyWebSocketHandler.wrapBuffer(srcBuffer, dstBuffer);
        dstBuffer.flip();

        byte[] actual = dstBuffer.array();

        assertEquals("invalid content length", srcBuffer.limit() + expectedHeaderSize, dstBuffer.limit());

        assertEquals("first byte mismatch", expected[0], actual[0]);
        assertEquals("second byte mismatch", expected[1], actual[1]);

        assertEquals("payload length byte mismatch 1", expected[2], actual[2]);
        assertEquals("payload length byte mismatch 2", expected[3], actual[3]);

        assertEquals("masking key mismatch 1", maskingKey[0], actual[4]);
        assertEquals("masking key mismatch 2", maskingKey[1], actual[5]);
        assertEquals("masking key mismatch 3", maskingKey[2], actual[6]);
        assertEquals("masking key mismatch 4", maskingKey[3], actual[7]);

        assertTrue(Arrays.equals(expected, actual));
    }

    @Test
    public void testWrapBuffer_medium_payload_min()
    {
        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();
        WebSocketHandlerImpl spyWebSocketHandler = spy(webSocketHandler);

        int payloadLength = WebSocketHeader.PAYLOAD_SHORT_MAX + 1;
        int messageLength = payloadLength + WebSocketHeader.MED_HEADER_LENGTH_MASKED;

        byte[] maskingKey = new byte[]{0x01, 0x02, 0x03, 0x04};

        byte[] data = new byte[payloadLength];
        Random random = new SecureRandom();
        random.nextBytes(data);

        ByteBuffer srcBuffer = ByteBuffer.allocate(messageLength);
        ByteBuffer dstBuffer = ByteBuffer.allocate(messageLength);
        srcBuffer.put(data);
        srcBuffer.flip();

        int expectedHeaderSize = WebSocketHeader.MED_HEADER_LENGTH_MASKED;
        byte[] expected = new byte[dstBuffer.capacity()];
        expected[0] = (byte) (WebSocketHeader.FINBIT_MASK | WebSocketHeader.OPCODE_BINARY);
        expected[1] = (byte) (WebSocketHeader.MASKBIT_MASK | WebSocketHeader.PAYLOAD_EXTENDED_16);

        expected[2] = (byte) (payloadLength >> 8);
        expected[3] = (byte) (payloadLength);

        expected[4] = maskingKey[0];
        expected[5] = maskingKey[1];
        expected[6] = maskingKey[2];
        expected[7] = maskingKey[3];

        for (int i = 0; i < srcBuffer.limit(); i++)
        {
            byte nextByte = srcBuffer.get();
            nextByte ^= maskingKey[i % 4];
            expected[i + WebSocketHeader.MED_HEADER_LENGTH_MASKED] = nextByte;
        }
        srcBuffer.flip();

        doReturn(maskingKey).when(spyWebSocketHandler).createRandomMaskingKey();

        spyWebSocketHandler.wrapBuffer(srcBuffer, dstBuffer);
        dstBuffer.flip();

        byte[] actual = dstBuffer.array();

        assertEquals("invalid content length", srcBuffer.limit() + expectedHeaderSize, dstBuffer.limit());

        assertEquals("first byte mismatch", expected[0], actual[0]);
        assertEquals("second byte mismatch", expected[1], actual[1]);

        assertEquals("payload length byte mismatch 1", expected[2], actual[2]);
        assertEquals("payload length byte mismatch 2", expected[3], actual[3]);

        assertEquals("masking key mismatch 1", maskingKey[0], actual[4]);
        assertEquals("masking key mismatch 2", maskingKey[1], actual[5]);
        assertEquals("masking key mismatch 3", maskingKey[2], actual[6]);
        assertEquals("masking key mismatch 4", maskingKey[3], actual[7]);

        assertTrue(Arrays.equals(expected, actual));
    }

    @Test
    public void testWrapBuffer_medium_payload_max()
    {
        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();
        WebSocketHandlerImpl spyWebSocketHandler = spy(webSocketHandler);

        int payloadLength = WebSocketHeader.PAYLOAD_MEDIUM_MAX;
        int messageLength = payloadLength + WebSocketHeader.MED_HEADER_LENGTH_MASKED;

        byte[] maskingKey = new byte[]{0x01, 0x02, 0x03, 0x04};

        byte[] data = new byte[payloadLength];
        Random random = new SecureRandom();
        random.nextBytes(data);

        ByteBuffer srcBuffer = ByteBuffer.allocate(messageLength);
        ByteBuffer dstBuffer = ByteBuffer.allocate(messageLength);
        srcBuffer.put(data);
        srcBuffer.flip();

        int expectedHeaderSize = WebSocketHeader.MED_HEADER_LENGTH_MASKED;
        byte[] expected = new byte[dstBuffer.capacity()];
        expected[0] = (byte) (WebSocketHeader.FINBIT_MASK | WebSocketHeader.OPCODE_BINARY);
        expected[1] = (byte) (WebSocketHeader.MASKBIT_MASK | WebSocketHeader.PAYLOAD_EXTENDED_16);

        expected[2] = (byte) (payloadLength >>> 8);
        expected[3] = (byte) (payloadLength);

        expected[4] = maskingKey[0];
        expected[5] = maskingKey[1];
        expected[6] = maskingKey[2];
        expected[7] = maskingKey[3];

        for (int i = 0; i < srcBuffer.limit(); i++)
        {
            byte nextByte = srcBuffer.get();
            nextByte ^= maskingKey[i % 4];
            expected[i + WebSocketHeader.MED_HEADER_LENGTH_MASKED] = nextByte;
        }
        srcBuffer.flip();

        doReturn(maskingKey).when(spyWebSocketHandler).createRandomMaskingKey();

        spyWebSocketHandler.wrapBuffer(srcBuffer, dstBuffer);
        dstBuffer.flip();

        byte[] actual = dstBuffer.array();

        assertEquals("invalid content length", srcBuffer.limit() + expectedHeaderSize, dstBuffer.limit());

        assertEquals("first byte mismatch", expected[0], actual[0]);
        assertEquals("second byte mismatch", expected[1], actual[1]);

        assertEquals("payload length byte mismatch 1", expected[2], actual[2]);
        assertEquals("payload length byte mismatch 2", expected[3], actual[3]);

        assertEquals("masking key mismatch 1", maskingKey[0], actual[4]);
        assertEquals("masking key mismatch 2", maskingKey[1], actual[5]);
        assertEquals("masking key mismatch 3", maskingKey[2], actual[6]);
        assertEquals("masking key mismatch 4", maskingKey[3], actual[7]);

        assertTrue(Arrays.equals(expected, actual));
    }

    @Test
    public void testWrapBuffer_large_payload()
    {
        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();
        WebSocketHandlerImpl spyWebSocketHandler = spy(webSocketHandler);

        int payloadLength = WebSocketHeader.PAYLOAD_MEDIUM_MAX + 100;
        int messageLength = payloadLength + WebSocketHeader.MAX_HEADER_LENGTH_MASKED;

        byte[] maskingKey = new byte[]{0x01, 0x02, 0x03, 0x04};

        byte[] data = new byte[payloadLength];
        Random random = new SecureRandom();
        random.nextBytes(data);

        ByteBuffer srcBuffer = ByteBuffer.allocate(messageLength);
        ByteBuffer dstBuffer = ByteBuffer.allocate(messageLength);
        srcBuffer.put(data);
        srcBuffer.flip();

        int expectedHeaderSize = WebSocketHeader.MAX_HEADER_LENGTH_MASKED;
        byte[] expected = new byte[dstBuffer.capacity()];
        expected[0] = (byte) (WebSocketHeader.FINBIT_MASK | WebSocketHeader.OPCODE_BINARY);
        expected[1] = (byte) (WebSocketHeader.MASKBIT_MASK | WebSocketHeader.PAYLOAD_EXTENDED_64);

        expected[2] = (byte) (payloadLength >>> 56);
        expected[3] = (byte) (payloadLength >>> 48);
        expected[4] = (byte) (payloadLength >>> 40);
        expected[5] = (byte) (payloadLength >>> 32);
        expected[6] = (byte) (payloadLength >>> 24);
        expected[7] = (byte) (payloadLength >>> 16);
        expected[8] = (byte) (payloadLength >>> 8);
        expected[9] = (byte) (payloadLength);

        expected[10] = maskingKey[0];
        expected[11] = maskingKey[1];
        expected[12] = maskingKey[2];
        expected[13] = maskingKey[3];

        for (int i = 0; i < srcBuffer.limit(); i++)
        {
            byte nextByte = srcBuffer.get();
            nextByte ^= maskingKey[i % 4];
            expected[i + WebSocketHeader.MAX_HEADER_LENGTH_MASKED] = nextByte;
        }
        srcBuffer.flip();

        doReturn(maskingKey).when(spyWebSocketHandler).createRandomMaskingKey();

        spyWebSocketHandler.wrapBuffer(srcBuffer, dstBuffer);
        dstBuffer.flip();

        byte[] actual = dstBuffer.array();

        assertEquals("invalid content length", srcBuffer.limit() + expectedHeaderSize, dstBuffer.limit());

        assertEquals("first byte mismatch", expected[0], actual[0]);
        assertEquals("second byte mismatch", expected[1], actual[1]);

        assertEquals("payload length byte mismatch 1", expected[2], actual[2]);
        assertEquals("payload length byte mismatch 2", expected[3], actual[3]);
        assertEquals("payload length byte mismatch 3", expected[4], actual[4]);
        assertEquals("payload length byte mismatch 4", expected[5], actual[5]);
        assertEquals("payload length byte mismatch 5", expected[6], actual[6]);
        assertEquals("payload length byte mismatch 6", expected[7], actual[7]);
        assertEquals("payload length byte mismatch 7", expected[8], actual[8]);
        assertEquals("payload length byte mismatch 8", expected[9], actual[9]);

        assertEquals("masking key mismatch 1", maskingKey[0], actual[10]);
        assertEquals("masking key mismatch 2", maskingKey[1], actual[11]);
        assertEquals("masking key mismatch 3", maskingKey[2], actual[12]);
        assertEquals("masking key mismatch 4", maskingKey[3], actual[13]);

        assertTrue(Arrays.equals(expected, actual));
    }

    @Test
    public void testWrapBuffer_large_payload_min()
    {
        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();
        WebSocketHandlerImpl spyWebSocketHandler = spy(webSocketHandler);

        int payloadLength = WebSocketHeader.PAYLOAD_MEDIUM_MAX + 1;
        int messageLength = payloadLength + WebSocketHeader.MAX_HEADER_LENGTH_MASKED;

        byte[] maskingKey = new byte[]{0x01, 0x02, 0x03, 0x04};

        byte[] data = new byte[payloadLength];
        Random random = new SecureRandom();
        random.nextBytes(data);

        ByteBuffer srcBuffer = ByteBuffer.allocate(messageLength);
        ByteBuffer dstBuffer = ByteBuffer.allocate(messageLength);
        srcBuffer.put(data);
        srcBuffer.flip();

        int expectedHeaderSize = WebSocketHeader.MAX_HEADER_LENGTH_MASKED;
        byte[] expected = new byte[dstBuffer.capacity()];
        expected[0] = (byte) (WebSocketHeader.FINBIT_MASK | WebSocketHeader.OPCODE_BINARY);
        expected[1] = (byte) (WebSocketHeader.MASKBIT_MASK | WebSocketHeader.PAYLOAD_EXTENDED_64);

        expected[2] = (byte) (payloadLength >>> 56);
        expected[3] = (byte) (payloadLength >>> 48);
        expected[4] = (byte) (payloadLength >>> 40);
        expected[5] = (byte) (payloadLength >>> 32);
        expected[6] = (byte) (payloadLength >>> 24);
        expected[7] = (byte) (payloadLength >>> 16);
        expected[8] = (byte) (payloadLength >>> 8);
        expected[9] = (byte) (payloadLength);

        expected[10] = maskingKey[0];
        expected[11] = maskingKey[1];
        expected[12] = maskingKey[2];
        expected[13] = maskingKey[3];

        for (int i = 0; i < srcBuffer.limit(); i++)
        {
            byte nextByte = srcBuffer.get();
            nextByte ^= maskingKey[i % 4];
            expected[i + WebSocketHeader.MAX_HEADER_LENGTH_MASKED] = nextByte;
        }
        srcBuffer.flip();

        doReturn(maskingKey).when(spyWebSocketHandler).createRandomMaskingKey();

        spyWebSocketHandler.wrapBuffer(srcBuffer, dstBuffer);
        dstBuffer.flip();

        byte[] actual = dstBuffer.array();

        assertEquals("invalid content length", srcBuffer.limit() + expectedHeaderSize, dstBuffer.limit());

        assertEquals("first byte mismatch", expected[0], actual[0]);
        assertEquals("second byte mismatch", expected[1], actual[1]);

        assertEquals("payload length byte mismatch 1", expected[2], actual[2]);
        assertEquals("payload length byte mismatch 2", expected[3], actual[3]);
        assertEquals("payload length byte mismatch 3", expected[4], actual[4]);
        assertEquals("payload length byte mismatch 4", expected[5], actual[5]);
        assertEquals("payload length byte mismatch 5", expected[6], actual[6]);
        assertEquals("payload length byte mismatch 6", expected[7], actual[7]);
        assertEquals("payload length byte mismatch 7", expected[8], actual[8]);
        assertEquals("payload length byte mismatch 8", expected[9], actual[9]);

        assertEquals("masking key mismatch 1", maskingKey[0], actual[10]);
        assertEquals("masking key mismatch 2", maskingKey[1], actual[11]);
        assertEquals("masking key mismatch 3", maskingKey[2], actual[12]);
        assertEquals("masking key mismatch 4", maskingKey[3], actual[13]);

        assertTrue(Arrays.equals(expected, actual));
    }

//    @Test
//    public void testWrapBuffer_large_payload_max()
//    {
//        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();
//        WebSocketHandlerImpl spyWebSocketHandler = spy(webSocketHandler);
//
//        int payloadLength = WebSocketHeader.PAYLOAD_LARGE_MAX / 16; // Limited by memory
//        int messageLength = payloadLength + WebSocketHeader.MAX_HEADER_LENGTH_MASKED;
//
//        byte[] maskingKey = new byte[]{0x01, 0x02, 0x03, 0x04};
//
//        byte[] data = new byte[payloadLength];
//        Random random = new SecureRandom();
//        //random.nextBytes(data);
//
//        ByteBuffer srcBuffer = ByteBuffer.allocate(messageLength);
//        ByteBuffer dstBuffer = ByteBuffer.allocate(messageLength);
//        srcBuffer.put(data);
//        srcBuffer.flip();
//
//        int expectedHeaderSize = WebSocketHeader.MAX_HEADER_LENGTH_MASKED;
//        byte[] expected = new byte[dstBuffer.capacity()];
//        expected[0] = (byte) (WebSocketHeader.FINBIT_MASK | WebSocketHeader.OPCODE_BINARY);
//        expected[1] = (byte) (WebSocketHeader.MASKBIT_MASK | WebSocketHeader.PAYLOAD_EXTENDED_64);
//
//        expected[2] = (byte) (payloadLength >>> 56);
//        expected[3] = (byte) (payloadLength >>> 48);
//        expected[4] = (byte) (payloadLength >>> 40);
//        expected[5] = (byte) (payloadLength >>> 32);
//        expected[6] = (byte) (payloadLength >>> 24);
//        expected[7] = (byte) (payloadLength >>> 16);
//        expected[8] = (byte) (payloadLength >>> 8);
//        expected[9] = (byte) (payloadLength);
//
//        expected[10] = maskingKey[0];
//        expected[11] = maskingKey[1];
//        expected[12] = maskingKey[2];
//        expected[13] = maskingKey[3];
//
//        for (int i = 0; i < srcBuffer.limit(); i++)
//        {
//            byte nextByte = srcBuffer.get();
//            nextByte ^= maskingKey[i % 4];
//            expected[i + WebSocketHeader.MAX_HEADER_LENGTH_MASKED] = nextByte;
//        }
//        srcBuffer.flip();
//
//        doReturn(maskingKey).when(spyWebSocketHandler).createRandomMaskingKey();
//
//        spyWebSocketHandler.wrapBuffer(srcBuffer, dstBuffer);
//        dstBuffer.flip();
//
//        byte[] actual = dstBuffer.array();
//
//        assertEquals("invalid content length", srcBuffer.limit() + expectedHeaderSize, dstBuffer.limit());
//
//        assertEquals("first byte mismatch", expected[0], actual[0]);
//        assertEquals("second byte mismatch", expected[1], actual[1]);
//
//        assertEquals("payload length byte mismatch 1", expected[2], actual[2]);
//        assertEquals("payload length byte mismatch 2", expected[3], actual[3]);
//        assertEquals("payload length byte mismatch 3", expected[4], actual[4]);
//        assertEquals("payload length byte mismatch 4", expected[5], actual[5]);
//        assertEquals("payload length byte mismatch 5", expected[6], actual[6]);
//        assertEquals("payload length byte mismatch 6", expected[7], actual[7]);
//        assertEquals("payload length byte mismatch 7", expected[8], actual[8]);
//        assertEquals("payload length byte mismatch 8", expected[9], actual[9]);
//
//        assertEquals("masking key mismatch 1", maskingKey[0], actual[10]);
//        assertEquals("masking key mismatch 2", maskingKey[1], actual[11]);
//        assertEquals("masking key mismatch 3", maskingKey[2], actual[12]);
//        assertEquals("masking key mismatch 4", maskingKey[3], actual[13]);
//
//        assertTrue(Arrays.equals(expected, actual));
//    }

    @Test(expected = IllegalArgumentException.class)
    public void testWrapBuffer_src_buffer_null()
    {
        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();
        WebSocketHandlerImpl spyWebSocketHandler = spy(webSocketHandler);

        int messageLength = 10;
        ByteBuffer srcBuffer = null;
        ByteBuffer dstBuffer = ByteBuffer.allocate(messageLength + WebSocketHeader.MAX_HEADER_LENGTH_MASKED);

        spyWebSocketHandler.wrapBuffer(srcBuffer, dstBuffer);
    }

    @Test(expected = IllegalArgumentException.class)
    public void testWrapBuffer_dst_buffer_null()
    {
        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();
        WebSocketHandlerImpl spyWebSocketHandler = spy(webSocketHandler);

        int messageLength = 10;
        ByteBuffer srcBuffer = ByteBuffer.allocate(messageLength + WebSocketHeader.MAX_HEADER_LENGTH_MASKED);
        ByteBuffer dstBuffer = null;

        spyWebSocketHandler.wrapBuffer(srcBuffer, dstBuffer);
    }

    @Test(expected = OutOfMemoryError.class)
    public void testWrapBuffer_dst_buffer_small()
    {
        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();
        WebSocketHandlerImpl spyWebSocketHandler = spy(webSocketHandler);

        int messageLength = 10;
        ByteBuffer srcBuffer = ByteBuffer.allocate(messageLength);
        ByteBuffer dstBuffer = ByteBuffer.allocate(messageLength);
        ;

        spyWebSocketHandler.wrapBuffer(srcBuffer, dstBuffer);
    }

    @Test
    public void testWrapBuffer_src_buffer_empty()
    {
        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();
        WebSocketHandlerImpl spyWebSocketHandler = spy(webSocketHandler);

        int messageLength = 10;
        ByteBuffer srcBuffer = ByteBuffer.allocate(messageLength);
        srcBuffer.flip();
        ByteBuffer dstBuffer = ByteBuffer.allocate(messageLength);
        ;

        spyWebSocketHandler.wrapBuffer(srcBuffer, dstBuffer);
    }

    @Test
    public void testUnwrapBuffer_opcode_ping()
    {
        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();
        WebSocketHandlerImpl spyWebSocketHandler = spy(webSocketHandler);

        int payloadLength = 100;
        int messageLength = payloadLength + WebSocketHeader.MIN_HEADER_LENGTH;

        byte[] data = new byte[messageLength];
        Random random = new SecureRandom();
        random.nextBytes(data);

        data[0] = (byte) (WebSocketHeader.FINBIT_MASK | WebSocketHeader.OPCODE_PING);
        data[1] = (byte) (payloadLength);

        ByteBuffer srcBuffer = ByteBuffer.allocate(messageLength);
        srcBuffer.put(data);
        srcBuffer.flip();

        assertEquals(WebSocketHandler.WebSocketMessageType.WEB_SOCKET_MESSAGE_TYPE_PING, spyWebSocketHandler.unwrapBuffer(srcBuffer).getType());

        byte[] expected = Arrays.copyOfRange(data, WebSocketHeader.MIN_HEADER_LENGTH, messageLength);
        byte[] actual = new byte[srcBuffer.remaining()];
        srcBuffer.get(actual);
        assertTrue(Arrays.equals(expected, actual));
    }

    @Test
    public void testUnwrapBuffer_opcode_close()
    {
        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();
        WebSocketHandlerImpl spyWebSocketHandler = spy(webSocketHandler);

        int payloadLength = 100;
        int messageLength = payloadLength + WebSocketHeader.MIN_HEADER_LENGTH;

        byte[] data = new byte[messageLength];
        Random random = new SecureRandom();
        random.nextBytes(data);

        data[0] = (byte) (WebSocketHeader.FINBIT_MASK | WebSocketHeader.OPCODE_CLOSE);
        data[1] = (byte) (payloadLength);

        ByteBuffer srcBuffer = ByteBuffer.allocate(messageLength);
        srcBuffer.put(data);
        srcBuffer.flip();

        assertEquals(WebSocketHandler.WebSocketMessageType.WEB_SOCKET_MESSAGE_TYPE_CLOSE, spyWebSocketHandler.unwrapBuffer(srcBuffer).getType());

        byte[] expected = Arrays.copyOfRange(data, WebSocketHeader.MIN_HEADER_LENGTH, messageLength);
        byte[] actual = new byte[srcBuffer.remaining()];
        srcBuffer.get(actual);
        assertTrue(Arrays.equals(expected, actual));
    }

    @Test
    public void testUnwrapBuffer_short_message()
    {
        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();
        WebSocketHandlerImpl spyWebSocketHandler = spy(webSocketHandler);

        int payloadLength = 100;
        int messageLength = payloadLength + WebSocketHeader.MIN_HEADER_LENGTH;

        byte[] data = new byte[messageLength];
        Random random = new SecureRandom();
        random.nextBytes(data);

        data[0] = (byte) (WebSocketHeader.FINBIT_MASK | WebSocketHeader.OPCODE_BINARY);
        data[1] = (byte) (payloadLength);

        ByteBuffer srcBuffer = ByteBuffer.allocate(messageLength);
        srcBuffer.put(data);
        srcBuffer.flip();

        assertEquals(WebSocketHandler.WebSocketMessageType.WEB_SOCKET_MESSAGE_TYPE_AMQP, spyWebSocketHandler.unwrapBuffer(srcBuffer).getType());

        byte[] expected = Arrays.copyOfRange(data, WebSocketHeader.MIN_HEADER_LENGTH, messageLength);
        byte[] actual = new byte[srcBuffer.remaining()];
        srcBuffer.get(actual);
        assertTrue(Arrays.equals(expected, actual));
    }

    @Test
    public void testUnwrapBuffer_short_message_min()
    {
        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();
        WebSocketHandlerImpl spyWebSocketHandler = spy(webSocketHandler);

        int payloadLength = 1;
        int messageLength = payloadLength + WebSocketHeader.MIN_HEADER_LENGTH;

        byte[] data = new byte[messageLength];
        Random random = new SecureRandom();
        random.nextBytes(data);

        data[0] = (byte) (WebSocketHeader.FINBIT_MASK | WebSocketHeader.OPCODE_BINARY);
        data[1] = (byte) (payloadLength);

        ByteBuffer srcBuffer = ByteBuffer.allocate(messageLength);
        srcBuffer.put(data);
        srcBuffer.flip();

        assertEquals(WebSocketHandler.WebSocketMessageType.WEB_SOCKET_MESSAGE_TYPE_AMQP, spyWebSocketHandler.unwrapBuffer(srcBuffer).getType());

        byte[] expected = Arrays.copyOfRange(data, 2, messageLength);
        byte[] actual = new byte[srcBuffer.remaining()];
        srcBuffer.get(actual);
        assertTrue(Arrays.equals(expected, actual));
    }

    @Test
    public void testUnwrapBuffer_short_message_max()
    {
        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();
        WebSocketHandlerImpl spyWebSocketHandler = spy(webSocketHandler);

        int payloadLength = WebSocketHeader.PAYLOAD_SHORT_MAX;
        int messageLength = payloadLength + WebSocketHeader.MIN_HEADER_LENGTH;

        byte[] data = new byte[messageLength];
        Random random = new SecureRandom();
        random.nextBytes(data);

        data[0] = (byte) (WebSocketHeader.FINBIT_MASK | WebSocketHeader.OPCODE_BINARY);
        data[1] = (byte) (payloadLength);

        ByteBuffer srcBuffer = ByteBuffer.allocate(messageLength);
        srcBuffer.put(data);
        srcBuffer.flip();

        assertEquals(WebSocketHandler.WebSocketMessageType.WEB_SOCKET_MESSAGE_TYPE_AMQP, spyWebSocketHandler.unwrapBuffer(srcBuffer).getType());

        byte[] expected = Arrays.copyOfRange(data, 2, messageLength);
        byte[] actual = new byte[srcBuffer.remaining()];
        srcBuffer.get(actual);
        assertTrue(Arrays.equals(expected, actual));
    }

    @Test
    public void testUnwrapBuffer_medium_message()
    {
        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();
        WebSocketHandlerImpl spyWebSocketHandler = spy(webSocketHandler);

        int payloadLength = WebSocketHeader.PAYLOAD_SHORT_MAX + 100;
        int messageLength = payloadLength + WebSocketHeader.MED_HEADER_LENGTH_NOMASK;

        byte[] data = new byte[messageLength];
        Random random = new SecureRandom();
        random.nextBytes(data);

        data[0] = (byte) (WebSocketHeader.FINBIT_MASK | WebSocketHeader.OPCODE_BINARY);
        data[1] = WebSocketHeader.PAYLOAD_EXTENDED_16;
        data[2] = (byte) (payloadLength >>> 8);
        data[3] = (byte) (payloadLength);

        ByteBuffer srcBuffer = ByteBuffer.allocate(messageLength);
        srcBuffer.put(data);
        srcBuffer.flip();

        assertEquals(WebSocketHandler.WebSocketMessageType.WEB_SOCKET_MESSAGE_TYPE_AMQP, spyWebSocketHandler.unwrapBuffer(srcBuffer).getType());

        byte[] expected = Arrays.copyOfRange(data, WebSocketHeader.MED_HEADER_LENGTH_NOMASK, messageLength);
        byte[] actual = new byte[srcBuffer.remaining()];
        srcBuffer.get(actual);
        assertTrue(Arrays.equals(expected, actual));
    }

    @Test
    public void testUnwrapBuffer_medium_message_min()
    {
        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();
        WebSocketHandlerImpl spyWebSocketHandler = spy(webSocketHandler);

        int payloadLength = WebSocketHeader.PAYLOAD_SHORT_MAX + 1;
        int messageLength = payloadLength + WebSocketHeader.MED_HEADER_LENGTH_NOMASK;

        byte[] data = new byte[messageLength];
        Random random = new SecureRandom();
        random.nextBytes(data);

        data[0] = (byte) (WebSocketHeader.FINBIT_MASK | WebSocketHeader.OPCODE_BINARY);
        data[1] = WebSocketHeader.PAYLOAD_EXTENDED_16;
        data[2] = (byte) (payloadLength >>> 8);
        data[3] = (byte) (payloadLength);

        ByteBuffer srcBuffer = ByteBuffer.allocate(messageLength);
        srcBuffer.put(data);
        srcBuffer.flip();

        assertEquals(WebSocketHandler.WebSocketMessageType.WEB_SOCKET_MESSAGE_TYPE_AMQP, spyWebSocketHandler.unwrapBuffer(srcBuffer).getType());

        byte[] expected = Arrays.copyOfRange(data, WebSocketHeader.MED_HEADER_LENGTH_NOMASK, messageLength);
        byte[] actual = new byte[srcBuffer.remaining()];
        srcBuffer.get(actual);
        assertTrue(Arrays.equals(expected, actual));
    }

    @Test
    public void testUnwrapBuffer_medium_message_max()
    {
        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();
        WebSocketHandlerImpl spyWebSocketHandler = spy(webSocketHandler);

        int payloadLength = WebSocketHeader.PAYLOAD_MEDIUM_MAX;
        int messageLength = payloadLength + WebSocketHeader.MED_HEADER_LENGTH_NOMASK;

        byte[] data = new byte[messageLength];
        Random random = new SecureRandom();
        random.nextBytes(data);

        data[0] = (byte) (WebSocketHeader.FINBIT_MASK | WebSocketHeader.OPCODE_BINARY);
        data[1] = WebSocketHeader.PAYLOAD_EXTENDED_16;
        data[2] = (byte) (payloadLength >>> 8);
        data[3] = (byte) (payloadLength);

        ByteBuffer srcBuffer = ByteBuffer.allocate(messageLength);
        srcBuffer.put(data);
        srcBuffer.flip();

        assertEquals(WebSocketHandler.WebSocketMessageType.WEB_SOCKET_MESSAGE_TYPE_AMQP, spyWebSocketHandler.unwrapBuffer(srcBuffer).getType());

        byte[] expected = Arrays.copyOfRange(data, WebSocketHeader.MED_HEADER_LENGTH_NOMASK, messageLength);
        byte[] actual = new byte[srcBuffer.remaining()];
        srcBuffer.get(actual);
        assertTrue(Arrays.equals(expected, actual));
    }

    @Test
    public void testUnwrapBuffer_large_message()
    {
        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();
        WebSocketHandlerImpl spyWebSocketHandler = spy(webSocketHandler);

        int payloadLength = WebSocketHeader.PAYLOAD_MEDIUM_MAX + 100;
        int messageLength = payloadLength + WebSocketHeader.MAX_HEADER_LENGTH_NOMASK;

        byte[] data = new byte[messageLength];
        Random random = new SecureRandom();
        random.nextBytes(data);

        data[0] = (byte) (WebSocketHeader.FINBIT_MASK | WebSocketHeader.OPCODE_BINARY);
        data[1] = WebSocketHeader.PAYLOAD_EXTENDED_64;
        data[2] = (byte) (payloadLength >>> 56);
        data[3] = (byte) (payloadLength >>> 48);
        data[4] = (byte) (payloadLength >>> 40);
        data[5] = (byte) (payloadLength >>> 32);
        data[6] = (byte) (payloadLength >>> 24);
        data[7] = (byte) (payloadLength >>> 16);
        data[8] = (byte) (payloadLength >>> 8);
        data[9] = (byte) (payloadLength);

        ByteBuffer srcBuffer = ByteBuffer.allocate(messageLength);
        srcBuffer.put(data);
        srcBuffer.flip();

        assertEquals(WebSocketHandler.WebSocketMessageType.WEB_SOCKET_MESSAGE_TYPE_AMQP, spyWebSocketHandler.unwrapBuffer(srcBuffer).getType());

        byte[] expected = Arrays.copyOfRange(data, WebSocketHeader.MAX_HEADER_LENGTH_NOMASK, messageLength);
        byte[] actual = new byte[srcBuffer.remaining()];
        srcBuffer.get(actual);
        assertTrue(Arrays.equals(expected, actual));
    }

    @Test
    public void testUnwrapBuffer_invalid_opcode()
    {
        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();
        WebSocketHandlerImpl spyWebSocketHandler = spy(webSocketHandler);

        byte[] data = {
                0b00001111,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0
        };

        ByteBuffer srcBuffer = ByteBuffer.wrap(data);

        assertEquals(WebSocketHandler.WebSocketMessageType.WEB_SOCKET_MESSAGE_TYPE_UNKNOWN, spyWebSocketHandler.unwrapBuffer(srcBuffer).getType());
    }

    @Test
    public void testUnwrapBuffer_src_buffer_empty()
    {
        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();
        WebSocketHandlerImpl spyWebSocketHandler = spy(webSocketHandler);

        int messageLength = 10;
        ByteBuffer srcBuffer = ByteBuffer.allocate(messageLength);
        srcBuffer.flip();

        assertEquals(WebSocketHandler.WebSocketMessageType.WEB_SOCKET_MESSAGE_TYPE_UNKNOWN, spyWebSocketHandler.unwrapBuffer(srcBuffer).getType());
    }

    @Test
    public void testUnwrapBuffer_src_buffer_medium_invalid_length()
    {
        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();
        WebSocketHandlerImpl spyWebSocketHandler = spy(webSocketHandler);

        int messageLength = WebSocketHeader.MIN_HEADER_LENGTH + 1;

        byte[] data = new byte[messageLength];
        Random random = new SecureRandom();
        random.nextBytes(data);

        data[0] = (byte) (WebSocketHeader.FINBIT_MASK | WebSocketHeader.OPCODE_BINARY);
        data[1] = WebSocketHeader.PAYLOAD_EXTENDED_16;
        data[2] = 0;

        ByteBuffer srcBuffer = ByteBuffer.allocate(messageLength);
        srcBuffer.put(data);
        srcBuffer.flip();

        assertEquals(WebSocketHandler.WebSocketMessageType.WEB_SOCKET_MESSAGE_TYPE_HEADER_CHUNK, spyWebSocketHandler.unwrapBuffer(srcBuffer).getType());
    }

    @Test
    public void testUnwrapBuffer_src_buffer_large_invalid_length()
    {
        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();
        WebSocketHandlerImpl spyWebSocketHandler = spy(webSocketHandler);

        int messageLength = WebSocketHeader.MED_HEADER_LENGTH_NOMASK + 1;

        byte[] data = new byte[messageLength];
        Random random = new SecureRandom();
        random.nextBytes(data);

        data[0] = (byte) (WebSocketHeader.FINBIT_MASK | WebSocketHeader.OPCODE_BINARY);
        data[1] = WebSocketHeader.PAYLOAD_EXTENDED_64;
        data[2] = 0;

        ByteBuffer srcBuffer = ByteBuffer.allocate(messageLength);
        srcBuffer.put(data);
        srcBuffer.flip();

        assertEquals(WebSocketHandler.WebSocketMessageType.WEB_SOCKET_MESSAGE_TYPE_HEADER_CHUNK, spyWebSocketHandler.unwrapBuffer(srcBuffer).getType());
    }

    @Test(expected = IllegalArgumentException.class)
    public void testUnwrapBuffer_src_buffer_null()
    {
        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();
        WebSocketHandlerImpl spyWebSocketHandler = spy(webSocketHandler);

        ByteBuffer srcBuffer = null;

        spyWebSocketHandler.unwrapBuffer(srcBuffer);
    }

    @Test
    public void testCalculateHeaderSize_zero_payload()
    {
        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();
        assertEquals(webSocketHandler.calculateHeaderSize(0), 0);
    }

    @Test
    public void testCalculateHeaderSize_small_payload()
    {
        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();
        assertEquals(webSocketHandler.calculateHeaderSize(
                WebSocketHeader.PAYLOAD_SHORT_MAX),
                WebSocketHeader.MIN_HEADER_LENGTH_MASKED);
    }

    @Test
    public void testCalculateHeaderSize_medium_payload()
    {
        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();
        assertEquals(webSocketHandler.calculateHeaderSize(
                WebSocketHeader.PAYLOAD_MEDIUM_MAX),
                WebSocketHeader.MED_HEADER_LENGTH_MASKED);
    }

    @Test
    public void testCalculateHeaderSize_large_payload()
    {
        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();
        assertEquals(webSocketHandler.calculateHeaderSize(
                WebSocketHeader.PAYLOAD_LARGE_MAX),
                WebSocketHeader.MAX_HEADER_LENGTH_MASKED);
    }
}
