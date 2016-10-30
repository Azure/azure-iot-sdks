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

import com.microsoft.azure.iothub.ws.WebSocket;
import com.microsoft.azure.iothub.ws.WebSocketHandler;
import com.microsoft.azure.iothub.ws.WebSocketHeader;
import org.apache.qpid.proton.engine.*;
import org.apache.qpid.proton.engine.impl.ByteBufferUtils;
import org.apache.qpid.proton.engine.impl.TransportInput;
import org.apache.qpid.proton.engine.impl.TransportOutput;
import org.apache.qpid.proton.engine.impl.TransportWrapper;
import org.junit.Test;
import org.mockito.invocation.InvocationOnMock;
import org.mockito.stubbing.Answer;

import java.nio.ByteBuffer;
import java.security.SecureRandom;
import java.util.Arrays;
import java.util.HashMap;
import java.util.Map;
import java.util.Random;

import static com.microsoft.azure.iothub.ws.WebSocketHeader.FINBIT_MASK;
import static com.microsoft.azure.iothub.ws.WebSocketHeader.OPCODE_MASK;
import static org.junit.Assert.*;
import static org.mockito.Mockito.*;

public class WebSocketImplTest
{
    private int _allocatedWebSocketBufferSize = (4 * 1024) + (16 * WebSocketHeader.MED_HEADER_LENGTH_MASKED);
    private String _hostName = "host_XXX";
    private String _webSocketPath = "path1/path2";
    private int _webSocketPort = 1234567890;
    private String _webSocketProtocol = "subprotocol_name";
    private Map<String, String> _additionalHeaders = new HashMap<String, String>();

    private int _lengthOfUpgradeRequest = 284;

    private void init()
    {
        _additionalHeaders.put("header1", "content1");
        _additionalHeaders.put("header2", "content2");
        _additionalHeaders.put("header3", "content3");
    }

    @Test
    public void testConstructor()
    {
        init();

        WebSocketImpl webSocketImpl = new WebSocketImpl();

        ByteBuffer inputBuffer = webSocketImpl.getInputBuffer();
        ByteBuffer outputBuffer = webSocketImpl.getOutputBuffer();
        ByteBuffer pingBuffer = webSocketImpl.getPingBuffer();

        assertNotNull(inputBuffer);
        assertNotNull(outputBuffer);
        assertNotNull(pingBuffer);

        assertEquals(inputBuffer.capacity(), _allocatedWebSocketBufferSize);
        assertEquals(outputBuffer.capacity(), _allocatedWebSocketBufferSize);
        assertEquals(pingBuffer.capacity(), _allocatedWebSocketBufferSize);

        assertFalse(webSocketImpl.getEnabled());
    }

    @Test
    public void testConfigure_handler_null()
    {
        init();

        WebSocketImpl webSocketImpl = new WebSocketImpl();

        webSocketImpl.configure(_hostName, _webSocketPath, _webSocketPort, _webSocketProtocol, _additionalHeaders, null);

        assertNotNull(webSocketImpl.getWebSocketHandler());
        assertTrue(webSocketImpl.getEnabled());
    }

    @Test
    public void testConfigure_handler_not_null()
    {
        init();

        WebSocketImpl webSocketImpl = new WebSocketImpl();
        WebSocketHandler webSocketHandler = new WebSocketHandlerImpl();

        webSocketImpl.configure(_hostName, _webSocketPath, _webSocketPort, _webSocketProtocol, _additionalHeaders, webSocketHandler);

        assertEquals(webSocketHandler, webSocketImpl.getWebSocketHandler());
        assertTrue(webSocketImpl.getEnabled());
    }

    @Test
    public void testWriteUpgradeRequest()
    {
        init();

        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();
        WebSocketHandlerImpl spyWebSocketHandler = spy(webSocketHandler);

        WebSocketImpl webSocketImpl = new WebSocketImpl();
        webSocketImpl.configure(_hostName, _webSocketPath, _webSocketPort, _webSocketProtocol, _additionalHeaders, spyWebSocketHandler);
        webSocketImpl.writeUpgradeRequest();

        verify(spyWebSocketHandler, times(1)).createUpgradeRequest(_hostName, _webSocketPath, _webSocketPort, _webSocketProtocol, _additionalHeaders);

        ByteBuffer outputBuffer = webSocketImpl.getOutputBuffer();
        outputBuffer.flip();

        assertEquals(_lengthOfUpgradeRequest, outputBuffer.remaining());
    }

    @Test
    public void testWritePong()
    {
        init();

        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();
        WebSocketHandlerImpl spyWebSocketHandler = spy(webSocketHandler);

        WebSocketImpl webSocketImpl = new WebSocketImpl();
        webSocketImpl.configure(_hostName, _webSocketPath, _webSocketPort, _webSocketProtocol, _additionalHeaders, spyWebSocketHandler);

        ByteBuffer outputBuffer = webSocketImpl.getOutputBuffer();
        ByteBuffer pingBuffer = webSocketImpl.getPingBuffer();

        webSocketImpl.writePong();

        verify(spyWebSocketHandler, times(1)).createPong(pingBuffer, outputBuffer);
    }

    @Test
    public void testWriteClose()
    {
        init();

        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();
        WebSocketHandlerImpl spyWebSocketHandler = spy(webSocketHandler);

        WebSocketImpl webSocketImpl = new WebSocketImpl();
        webSocketImpl.configure(_hostName, _webSocketPath, _webSocketPort, _webSocketProtocol, _additionalHeaders, spyWebSocketHandler);

        String message = "Message";

        ByteBuffer pingBuffer = webSocketImpl.getPingBuffer();
        pingBuffer.clear();
        pingBuffer.put(message.getBytes());

        ByteBuffer outputBuffer = webSocketImpl.getOutputBuffer();

        webSocketImpl.writeClose();

        assertTrue(Arrays.equals(pingBuffer.array(), outputBuffer.array()));
    }

    @Test
    public void testWrap_creates_sniffer()
    {
        init();

        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();

        WebSocketImpl webSocketImpl = new WebSocketImpl();
        webSocketImpl.configure(_hostName, _webSocketPath, _webSocketPort, _webSocketProtocol, _additionalHeaders, webSocketHandler);

        TransportInput mockTransportInput = mock(TransportInput.class);
        TransportOutput mockTransportOutput = mock(TransportOutput.class);
        TransportWrapper transportWrapper = webSocketImpl.wrap(mockTransportInput, mockTransportOutput);

        assertNotNull(transportWrapper);
    }

    @Test
    public void testWrapBuffer_enabled()
    {
        init();

        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();
        WebSocketHandlerImpl mockWebSocketHandler = mock(webSocketHandler.getClass());

        WebSocketImpl webSocketImpl = new WebSocketImpl();
        webSocketImpl.configure(_hostName, _webSocketPath, _webSocketPort, _webSocketProtocol, _additionalHeaders, mockWebSocketHandler);

        ByteBuffer srcBuffer = ByteBuffer.allocate(50);
        srcBuffer.clear();

        ByteBuffer dstBuffer = ByteBuffer.allocate(50);

        webSocketImpl._isWebSocketEnabled = true;
        webSocketImpl.wrapBuffer(srcBuffer, dstBuffer);

        verify(mockWebSocketHandler, times(1)).wrapBuffer(srcBuffer, dstBuffer);
    }

    @Test
    public void testWrapBuffer_not_enabled()
    {
        init();

        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();
        WebSocketHandlerImpl mockWebSocketHandler = mock(webSocketHandler.getClass());

        WebSocketImpl webSocketImpl = new WebSocketImpl();
        webSocketImpl.configure(_hostName, _webSocketPath, _webSocketPort, _webSocketProtocol, _additionalHeaders, mockWebSocketHandler);

        ByteBuffer srcBuffer = ByteBuffer.allocate(25);
        srcBuffer.clear();
        srcBuffer.put("abcdefghijklmnopqrstvwxyz".getBytes());
        srcBuffer.flip();

        ByteBuffer dstBuffer = ByteBuffer.allocate(25);
        dstBuffer.put("1234567890".getBytes());

        webSocketImpl._isWebSocketEnabled = false;
        webSocketImpl.wrapBuffer(srcBuffer, dstBuffer);

        dstBuffer.flip();
        assertTrue(Arrays.equals(srcBuffer.array(), dstBuffer.array()));
        verify(mockWebSocketHandler, times(0)).wrapBuffer((ByteBuffer) any(), (ByteBuffer) any());
    }

    @Test
    public void testUnwrapBuffer_enabled()
    {
        init();

        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();
        WebSocketHandlerImpl mockWebSocketHandler = mock(webSocketHandler.getClass());

        WebSocketImpl webSocketImpl = new WebSocketImpl();
        webSocketImpl.configure(_hostName, _webSocketPath, _webSocketPort, _webSocketProtocol, _additionalHeaders, mockWebSocketHandler);

        ByteBuffer srcBuffer = ByteBuffer.allocate(50);
        srcBuffer.clear();

        webSocketImpl._isWebSocketEnabled = true;
        webSocketImpl.unwrapBuffer(srcBuffer);

        verify(mockWebSocketHandler, times(1)).unwrapBuffer(srcBuffer);
    }

    @Test
    public void testUnwrapBuffer_not_enabled()
    {
        init();

        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();
        WebSocketHandlerImpl mockWebSocketHandler = mock(webSocketHandler.getClass());

        WebSocketImpl webSocketImpl = new WebSocketImpl();
        webSocketImpl.configure(_hostName, _webSocketPath, _webSocketPort, _webSocketProtocol, _additionalHeaders, mockWebSocketHandler);

        ByteBuffer srcBuffer = ByteBuffer.allocate(25);
        srcBuffer.clear();
        srcBuffer.put("abcdefghijklmnopqrstvwxyz".getBytes());
        srcBuffer.flip();

        webSocketImpl._isWebSocketEnabled = false;

        assertTrue(webSocketImpl.unwrapBuffer(srcBuffer).getType() == WebSocketHandler.WebSocketMessageType.WEB_SOCKET_MESSAGE_TYPE_UNKNOWN);
        verify(mockWebSocketHandler, times(0)).wrapBuffer((ByteBuffer) any(), (ByteBuffer) any());
    }

    @Test
    public void testPending_state_notStarted()
    {
        init();

        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();

        WebSocketImpl webSocketImpl = new WebSocketImpl();
        webSocketImpl.configure(_hostName, _webSocketPath, _webSocketPort, _webSocketProtocol, _additionalHeaders, webSocketHandler);

        TransportInput mockTransportInput = mock(TransportInput.class);
        TransportOutput mockTransportOutput = mock(TransportOutput.class);
        TransportWrapper transportWrapper = webSocketImpl.wrap(mockTransportInput, mockTransportOutput);
        transportWrapper.pending();

        ByteBuffer outputBuffer = webSocketImpl.getOutputBuffer();
        outputBuffer.flip();

        assertTrue(outputBuffer.remaining() == _lengthOfUpgradeRequest);
    }

    @Test
    public void testPending_state_notStarted__output_not_empty()
    {
        init();

        WebSocketImpl webSocketImpl = new WebSocketImpl();

        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();
        webSocketImpl.configure(_hostName, _webSocketPath, _webSocketPort, _webSocketProtocol, _additionalHeaders, webSocketHandler);

        String message = "Message";
        ByteBuffer outputBuffer = webSocketImpl.getOutputBuffer();
        outputBuffer.put(message.getBytes());

        TransportInput mockTransportInput = mock(TransportInput.class);
        TransportOutput mockTransportOutput = mock(TransportOutput.class);
        TransportWrapper transportWrapper = webSocketImpl.wrap(mockTransportInput, mockTransportOutput);

        assertTrue(message.length() == transportWrapper.pending());

        ByteBuffer actual = webSocketImpl.getOutputBuffer();
        assertTrue(Arrays.equals(outputBuffer.array(), actual.array()));
    }

    @Test
    public void testPending_state_notStarted__head_closed()
    {
        init();

        WebSocketImpl webSocketImpl = new WebSocketImpl();

        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();
        webSocketImpl.configure(_hostName, _webSocketPath, _webSocketPort, _webSocketProtocol, _additionalHeaders, webSocketHandler);

        TransportInput mockTransportInput = mock(TransportInput.class);
        TransportOutput mockTransportOutput = mock(TransportOutput.class);
        TransportWrapper transportWrapper = webSocketImpl.wrap(mockTransportInput, mockTransportOutput);

        transportWrapper.close_tail();
        assertTrue(transportWrapper.pending() == Transport.END_OF_STREAM);
        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_FAILED);

        ByteBuffer outputBuffer = webSocketImpl.getOutputBuffer();
        outputBuffer.flip();

        assertTrue(outputBuffer.remaining() == _lengthOfUpgradeRequest);
    }

    @Test
    public void testPending_state_connecting()
    {
        init();

        WebSocketImpl webSocketImpl = new WebSocketImpl();

        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();
        WebSocketHandlerImpl spyWebSocketHandler = spy(webSocketHandler);

        TransportInput mockTransportInput = mock(TransportInput.class);
        TransportOutput mockTransportOutput = mock(TransportOutput.class);

        webSocketImpl.configure(_hostName, _webSocketPath, _webSocketPort, _webSocketProtocol, _additionalHeaders, webSocketHandler);

        TransportWrapper transportWrapper = webSocketImpl.wrap(mockTransportInput, mockTransportOutput);

        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_NOT_STARTED);
        transportWrapper.pending();
        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_CONNECTING);

        ByteBuffer outputBuffer = webSocketImpl.getOutputBuffer();
        outputBuffer.flip();

        transportWrapper.pending();
        assertTrue(outputBuffer.remaining() == _lengthOfUpgradeRequest);
    }

    @Test
    public void testPending_state_connecting_head_closed_empty_buffer()
    {
        init();

        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();

        WebSocketImpl webSocketImpl = new WebSocketImpl();
        webSocketImpl.configure(_hostName, _webSocketPath, _webSocketPort, _webSocketProtocol, _additionalHeaders, webSocketHandler);

        TransportInput mockTransportInput = mock(TransportInput.class);
        TransportOutput mockTransportOutput = mock(TransportOutput.class);
        TransportWrapper transportWrapper = webSocketImpl.wrap(mockTransportInput, mockTransportOutput);

        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_NOT_STARTED);
        transportWrapper.pending();
        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_CONNECTING);

        ByteBuffer outputBuffer = webSocketImpl.getOutputBuffer();
        outputBuffer.clear();
        transportWrapper.close_tail();

        assertTrue(transportWrapper.pending() == Transport.END_OF_STREAM);
        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_FAILED);
    }

    @Test
    public void testPending_state_flow_empty_output()
    {
        init();

        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();
        WebSocketHandlerImpl mockWebSocketHandler = mock(webSocketHandler.getClass());

        WebSocketImpl webSocketImpl = new WebSocketImpl();
        webSocketImpl.configure(_hostName, _webSocketPath, _webSocketPort, _webSocketProtocol, _additionalHeaders, mockWebSocketHandler);

        TransportInput mockTransportInput = mock(TransportInput.class);
        TransportOutput mockTransportOutput = mock(TransportOutput.class);

        TransportWrapper transportWrapper = webSocketImpl.wrap(mockTransportInput, mockTransportOutput);

        when(mockWebSocketHandler.validateUpgradeReply((ByteBuffer) any())).thenReturn(true);
        when(mockWebSocketHandler.createUpgradeRequest(_hostName, _webSocketPath, _webSocketPort, _webSocketProtocol, _additionalHeaders)).thenReturn("Request");

        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_NOT_STARTED);
        transportWrapper.pending();
        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_CONNECTING);
        transportWrapper.process();
        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_CONNECTED_FLOW);

        String message = "Message";
        ByteBuffer outputBuffer = webSocketImpl.getOutputBuffer();
        outputBuffer.put(message.getBytes());
        when(mockTransportOutput.pending()).thenReturn(0);

        assertEquals(transportWrapper.pending(), 0);
        verify(mockWebSocketHandler, times(0)).wrapBuffer((ByteBuffer) any(), (ByteBuffer) any());
    }

    @Test
    public void testPending_state_flow_output_not_empty()
    {
        init();

        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();
        WebSocketHandlerImpl mockWebSocketHandler = mock(webSocketHandler.getClass());

        WebSocketImpl webSocketImpl = new WebSocketImpl();
        webSocketImpl.configure(_hostName, _webSocketPath, _webSocketPort, _webSocketProtocol, _additionalHeaders, mockWebSocketHandler);

        TransportInput mockTransportInput = mock(TransportInput.class);
        TransportOutput mockTransportOutput = mock(TransportOutput.class);

        TransportWrapper transportWrapper = webSocketImpl.wrap(mockTransportInput, mockTransportOutput);

        when(mockWebSocketHandler.validateUpgradeReply((ByteBuffer) any())).thenReturn(true);
        when(mockWebSocketHandler.createUpgradeRequest(_hostName, _webSocketPath, _webSocketPort, _webSocketProtocol, _additionalHeaders)).thenReturn("Request");

        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_NOT_STARTED);
        transportWrapper.pending();
        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_CONNECTING);
        transportWrapper.process();
        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_CONNECTED_FLOW);

        String message = "Message";
        ByteBuffer outputBuffer = webSocketImpl.getOutputBuffer();
        outputBuffer.put(message.getBytes());
        when(mockTransportOutput.pending()).thenReturn(message.length());
        when(mockWebSocketHandler.calculateHeaderSize(message.length())).thenReturn((int) WebSocketHeader.MIN_HEADER_LENGTH_MASKED);

        int expected = message.length() + WebSocketHeader.MIN_HEADER_LENGTH_MASKED;
        int actual = transportWrapper.pending();
        assertEquals(expected, actual);
        verify(mockWebSocketHandler, times(0)).wrapBuffer((ByteBuffer) any(), (ByteBuffer) any());
        verify(mockWebSocketHandler, times(1)).calculateHeaderSize(message.length());
    }

    @Test
    public void testPending_state_pong_changes_to_flow()
    {
        init();

        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();
        WebSocketHandlerImpl mockWebSocketHandler = mock(webSocketHandler.getClass());

        WebSocketImpl webSocketImpl = new WebSocketImpl();
        webSocketImpl.configure(_hostName, _webSocketPath, _webSocketPort, _webSocketProtocol, _additionalHeaders, mockWebSocketHandler);

        TransportInput mockTransportInput = mock(TransportInput.class);
        TransportOutput mockTransportOutput = mock(TransportOutput.class);

        TransportWrapper transportWrapper = webSocketImpl.wrap(mockTransportInput, mockTransportOutput);

        when(mockWebSocketHandler.validateUpgradeReply((ByteBuffer) any())).thenReturn(true);
        when(mockWebSocketHandler.createUpgradeRequest(_hostName, _webSocketPath, _webSocketPort, _webSocketProtocol, _additionalHeaders)).thenReturn("Request");
        when(mockWebSocketHandler.unwrapBuffer((ByteBuffer) any())).thenReturn(new WebSocketHandler.WebsocketTuple(7, WebSocketHandler.WebSocketMessageType.WEB_SOCKET_MESSAGE_TYPE_PING));

        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_NOT_STARTED);
        transportWrapper.pending();
        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_CONNECTING);
        transportWrapper.process();
        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_CONNECTED_FLOW);

        String message = "Message";
        ByteBuffer inputBuffer = webSocketImpl.getInputBuffer();
        inputBuffer.clear();
        inputBuffer.put(message.getBytes());

        transportWrapper.process();

        ByteBuffer pingBuffer = webSocketImpl.getPingBuffer();

        inputBuffer.flip();
        pingBuffer.flip();
        assertTrue(Arrays.equals(inputBuffer.array(), pingBuffer.array()));
        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_CONNECTED_PONG);

        transportWrapper.pending();
        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_CONNECTED_FLOW);
    }

    @Test
    public void testPending_state_closing_changes_to_closed()
    {
        init();

        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();
        WebSocketHandlerImpl mockWebSocketHandler = mock(webSocketHandler.getClass());

        WebSocketImpl webSocketImpl = new WebSocketImpl();
        webSocketImpl.configure(_hostName, _webSocketPath, _webSocketPort, _webSocketProtocol, _additionalHeaders, mockWebSocketHandler);

        TransportInput mockTransportInput = mock(TransportInput.class);
        TransportOutput mockTransportOutput = mock(TransportOutput.class);

        TransportWrapper transportWrapper = webSocketImpl.wrap(mockTransportInput, mockTransportOutput);

        when(mockWebSocketHandler.validateUpgradeReply((ByteBuffer) any())).thenReturn(true);
        when(mockWebSocketHandler.createUpgradeRequest(_hostName, _webSocketPath, _webSocketPort, _webSocketProtocol, _additionalHeaders)).thenReturn("Request");
        when(mockWebSocketHandler.unwrapBuffer((ByteBuffer) any())).thenReturn(new WebSocketHandler.WebsocketTuple(7, WebSocketHandler.WebSocketMessageType.WEB_SOCKET_MESSAGE_TYPE_CLOSE));

        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_NOT_STARTED);
        transportWrapper.pending();
        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_CONNECTING);
        transportWrapper.process();
        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_CONNECTED_FLOW);

        String message = "Message";
        ByteBuffer inputBuffer = webSocketImpl.getInputBuffer();
        inputBuffer.clear();
        inputBuffer.put(message.getBytes());

        transportWrapper.process();

        ByteBuffer pingBuffer = webSocketImpl.getPingBuffer();

        inputBuffer.flip();
        pingBuffer.flip();
        assertTrue(Arrays.equals(inputBuffer.array(), pingBuffer.array()));
        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_CONNECTED_CLOSING);

        transportWrapper.pending();
        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_CLOSED);
    }

    @Test
    public void testPending_state_closing_head_closed()
    {
        init();

        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();
        WebSocketHandlerImpl mockWebSocketHandler = mock(webSocketHandler.getClass());

        WebSocketImpl webSocketImpl = new WebSocketImpl();
        webSocketImpl.configure(_hostName, _webSocketPath, _webSocketPort, _webSocketProtocol, _additionalHeaders, mockWebSocketHandler);

        TransportInput mockTransportInput = mock(TransportInput.class);
        TransportOutput mockTransportOutput = mock(TransportOutput.class);

        TransportWrapper transportWrapper = webSocketImpl.wrap(mockTransportInput, mockTransportOutput);

        when(mockWebSocketHandler.validateUpgradeReply((ByteBuffer) any())).thenReturn(true);
        when(mockWebSocketHandler.createUpgradeRequest(_hostName, _webSocketPath, _webSocketPort, _webSocketProtocol, _additionalHeaders)).thenReturn("Request");
        when(mockWebSocketHandler.unwrapBuffer((ByteBuffer) any())).thenReturn(new WebSocketHandler.WebsocketTuple(7, WebSocketHandler.WebSocketMessageType.WEB_SOCKET_MESSAGE_TYPE_CLOSE));

        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_NOT_STARTED);
        transportWrapper.pending();
        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_CONNECTING);
        transportWrapper.process();
        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_CONNECTED_FLOW);

        String message = "Message";
        ByteBuffer inputBuffer = webSocketImpl.getInputBuffer();
        inputBuffer.clear();
        inputBuffer.put(message.getBytes());

        transportWrapper.process();

        ByteBuffer pingBuffer = webSocketImpl.getPingBuffer();

        inputBuffer.flip();
        pingBuffer.flip();
        assertTrue(Arrays.equals(inputBuffer.array(), pingBuffer.array()));
        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_CONNECTED_CLOSING);

        transportWrapper.close_tail();

        transportWrapper.pending();

        assertTrue(transportWrapper.pending() == Transport.END_OF_STREAM);
        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_FAILED);
    }

    @Test
    public void testProcess_state_not_started()
    {
        init();

        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();

        WebSocketImpl webSocketImpl = new WebSocketImpl();
        webSocketImpl.configure(_hostName, _webSocketPath, _webSocketPort, _webSocketProtocol, _additionalHeaders, webSocketHandler);

        TransportInput mockTransportInput = mock(TransportInput.class);
        TransportOutput mockTransportOutput = mock(TransportOutput.class);
        TransportWrapper transportWrapper = webSocketImpl.wrap(mockTransportInput, mockTransportOutput);

        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_NOT_STARTED);
        transportWrapper.process();
        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_NOT_STARTED);

        verify(mockTransportInput, times(1)).process();
    }

    @Test
    public void testProcess_state_changes_from_connecting_to_flow_on_valid_reply()
    {
        init();

        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();
        WebSocketHandlerImpl mockWebSocketHandler = mock(webSocketHandler.getClass());

        WebSocketImpl webSocketImpl = new WebSocketImpl();
        webSocketImpl.configure(_hostName, _webSocketPath, _webSocketPort, _webSocketProtocol, _additionalHeaders, mockWebSocketHandler);

        TransportInput mockTransportInput = mock(TransportInput.class);
        TransportOutput mockTransportOutput = mock(TransportOutput.class);
        TransportWrapper transportWrapper = webSocketImpl.wrap(mockTransportInput, mockTransportOutput);

        when(mockWebSocketHandler.validateUpgradeReply((ByteBuffer) any())).thenReturn(true);
        when(mockWebSocketHandler.createUpgradeRequest(_hostName, _webSocketPath, _webSocketPort, _webSocketProtocol, _additionalHeaders)).thenReturn("Request");

        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_NOT_STARTED);
        transportWrapper.pending();
        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_CONNECTING);
        transportWrapper.process();
        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_CONNECTED_FLOW);
    }

//    @Test
//    public void testProcess_state_flow_repeated_reply()
//    {
//        init();
//
//        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();
//        WebSocketHandlerImpl mockWebSocketHandler = mock(webSocketHandler.getClass());
//
//        WebSocketImpl webSocketImpl = new WebSocketImpl();
//        webSocketImpl.configure(_hostName, _webSocketPath, _webSocketPort, _webSocketProtocol, _additionalHeaders, mockWebSocketHandler);
//
//        TransportInput mockTransportInput = mock(TransportInput.class);
//        TransportOutput mockTransportOutput = mock(TransportOutput.class);
//
//        TransportWrapper transportWrapper = webSocketImpl.wrap(mockTransportInput, mockTransportOutput);
//
//        when(mockWebSocketHandler.validateUpgradeReply((ByteBuffer) any())).thenReturn(true);
//        when(mockWebSocketHandler.createUpgradeRequest(_hostName, _webSocketPath, _webSocketPort, _webSocketProtocol, _additionalHeaders)).thenReturn("Request");
//
//        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_NOT_STARTED);
//        transportWrapper.pending();
//        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_CONNECTING);
//        transportWrapper.process();
//        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_CONNECTED_FLOW);
//
//        String message = "HTTP ";
//        ByteBuffer inputBuffer = webSocketImpl.getInputBuffer();
//        inputBuffer.clear();
//        inputBuffer.put(message.getBytes());
//
//        transportWrapper.process();
//        verify(mockWebSocketHandler, times(0)).unwrapBuffer((ByteBuffer) any());
//    }

    @Test
    public void testProcess_state_flow_calls_underlying_amqp()
    {
        init();

        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();
        WebSocketHandlerImpl mockWebSocketHandler = mock(webSocketHandler.getClass());

        WebSocketImpl webSocketImpl = new WebSocketImpl();
        webSocketImpl.configure(_hostName, _webSocketPath, _webSocketPort, _webSocketProtocol, _additionalHeaders, mockWebSocketHandler);

        TransportInput mockTransportInput = spy(new TransportInput() {
            ByteBuffer bb = ByteBufferUtils.newWriteableBuffer(4224);

            @Override
            public int capacity() {
                return bb.remaining();
            }

            @Override
            public int position() {
                return bb.position();
            }

            @Override
            public ByteBuffer tail() throws TransportException {
                return bb;
            }

            @Override
            public void process() throws TransportException {
            }

            @Override
            public void close_tail() {
            }
        });
        TransportOutput mockTransportOutput = mock(TransportOutput.class);
        doNothing().when(mockTransportInput).process();
        doNothing().when(mockTransportInput).close_tail();

        TransportWrapper transportWrapper = webSocketImpl.wrap(mockTransportInput, mockTransportOutput);

        when(mockWebSocketHandler.validateUpgradeReply((ByteBuffer) any())).thenReturn(true);
        when(mockWebSocketHandler.createUpgradeRequest(_hostName, _webSocketPath, _webSocketPort, _webSocketProtocol, _additionalHeaders)).thenReturn("Request");
        when(mockWebSocketHandler.unwrapBuffer((ByteBuffer) any())).thenReturn(new WebSocketHandler.WebsocketTuple(7, WebSocketHandler.WebSocketMessageType.WEB_SOCKET_MESSAGE_TYPE_AMQP));

        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_NOT_STARTED);
        transportWrapper.pending();
        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_CONNECTING);
        transportWrapper.process();
        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_CONNECTED_FLOW);

        String message = "Message";
        ByteBuffer inputBuffer = webSocketImpl.getInputBuffer();
        inputBuffer.clear();
        inputBuffer.put(message.getBytes());

        transportWrapper.process();
        verify(mockTransportInput, times(1)).process();
    }

    /*Not needed*/
//    @Test
//    public void testProcess_state_flow_calls_underlying_empty()
//    {
//        init();
//
//        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();
//        WebSocketHandlerImpl mockWebSocketHandler = mock(webSocketHandler.getClass());
//
//        WebSocketImpl webSocketImpl = new WebSocketImpl();
//        webSocketImpl.configure(_hostName, _webSocketPath, _webSocketPort, _webSocketProtocol, _additionalHeaders, mockWebSocketHandler);
//
//        TransportInput mockTransportInput = mock(TransportInput.class);
//        TransportOutput mockTransportOutput = mock(TransportOutput.class);
//
//        TransportWrapper transportWrapper = webSocketImpl.wrap(mockTransportInput, mockTransportOutput);
//
//        when(mockWebSocketHandler.validateUpgradeReply((ByteBuffer) any())).thenReturn(true);
//        when(mockWebSocketHandler.createUpgradeRequest(_hostName, _webSocketPath, _webSocketPort, _webSocketProtocol, _additionalHeaders)).thenReturn("Request");
//        when(mockWebSocketHandler.unwrapBuffer((ByteBuffer) any())).thenReturn(new WebSocketHandler.WebsocketTuple(7, WebSocketHandler.WebSocketMessageType.WEB_SOCKET_MESSAGE_TYPE_UNKNOWN));
//
//        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_NOT_STARTED);
//        transportWrapper.pending();
//        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_CONNECTING);
//        transportWrapper.process();
//        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_CONNECTED_FLOW);
//
//        String message = "Message";
//        ByteBuffer inputBuffer = webSocketImpl.getInputBuffer();
//        inputBuffer.clear();
//        inputBuffer.put(message.getBytes());
//
//        transportWrapper.process();
//        verify(mockTransportInput, times(1)).process();
//    }

//    @Test
//    public void testProcess_state_flow_calls_underlying_invalid()
//    {
//        init();
//
//        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();
//        WebSocketHandlerImpl mockWebSocketHandler = mock(webSocketHandler.getClass());
//
//        WebSocketImpl webSocketImpl = new WebSocketImpl();
//        webSocketImpl.configure(_hostName, _webSocketPath, _webSocketPort, _webSocketProtocol, _additionalHeaders, mockWebSocketHandler);
//
//        TransportInput mockTransportInput = mock(TransportInput.class);
//        TransportOutput mockTransportOutput = mock(TransportOutput.class);
//
//        TransportWrapper transportWrapper = webSocketImpl.wrap(mockTransportInput, mockTransportOutput);
//
//        when(mockWebSocketHandler.validateUpgradeReply((ByteBuffer) any())).thenReturn(true);
//        when(mockWebSocketHandler.createUpgradeRequest(_hostName, _webSocketPath, _webSocketPort, _webSocketProtocol, _additionalHeaders)).thenReturn("Request");
//        when(mockWebSocketHandler.unwrapBuffer((ByteBuffer) any())).thenReturn(new WebSocketHandler.WebsocketTuple(7, WebSocketHandler.WebSocketMessageType.WEB_SOCKET_MESSAGE_TYPE_UNKNOWN));
//
//        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_NOT_STARTED);
//        transportWrapper.pending();
//        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_CONNECTING);
//        transportWrapper.process();
//        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_CONNECTED_FLOW);
//
//        String message = "Message";
//        ByteBuffer inputBuffer = webSocketImpl.getInputBuffer();
//        inputBuffer.clear();
//        inputBuffer.put(message.getBytes());
//
//        transportWrapper.process();
//        verify(mockTransportInput, times(1)).process();
//    }

    //    @Test
//    public void testProcess_state_flow_calls_underlying_invalid_length()
//    {
//        init();
//
//        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();
//        WebSocketHandlerImpl mockWebSocketHandler = mock(webSocketHandler.getClass());
//
//        WebSocketImpl webSocketImpl = new WebSocketImpl();
//        webSocketImpl.configure(_hostName, _webSocketPath, _webSocketPort, _webSocketProtocol, _additionalHeaders, mockWebSocketHandler);
//
//        TransportInput mockTransportInput = mock(TransportInput.class);
//        TransportOutput mockTransportOutput = mock(TransportOutput.class);
//
//        TransportWrapper transportWrapper = webSocketImpl.wrap(mockTransportInput, mockTransportOutput);
//
//        when(mockWebSocketHandler.validateUpgradeReply((ByteBuffer) any())).thenReturn(true);
//        when(mockWebSocketHandler.createUpgradeRequest(_hostName, _webSocketPath, _webSocketPort, _webSocketProtocol, _additionalHeaders)).thenReturn("Request");
//        when(mockWebSocketHandler.unwrapBuffer((ByteBuffer) any())).thenReturn(WebSocketHandler.WebSocketMessageType.WEB_SOCKET_MESSAGE_TYPE_UNKNOWN);
//
//        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_NOT_STARTED);
//        transportWrapper.pending();
//        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_CONNECTING);
//        transportWrapper.process();
//        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_CONNECTED_FLOW);
//
//        String message = "Message";
//        ByteBuffer inputBuffer = webSocketImpl.getInputBuffer();
//        inputBuffer.clear();
//        inputBuffer.put(message.getBytes());
//
//        transportWrapper.process();
//        verify(mockTransportInput, times(1)).process();
//    }
//
//    @Test
//    public void testProcess_state_flow_calls_underlying_invalid_masked()
//    {
//        init();
//
//        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();
//        WebSocketHandlerImpl mockWebSocketHandler = mock(webSocketHandler.getClass());
//
//        WebSocketImpl webSocketImpl = new WebSocketImpl();
//        webSocketImpl.configure(_hostName, _webSocketPath, _webSocketPort, _webSocketProtocol, _additionalHeaders, mockWebSocketHandler);
//
//        TransportInput mockTransportInput = mock(TransportInput.class);
//        TransportOutput mockTransportOutput = mock(TransportOutput.class);
//
//        TransportWrapper transportWrapper = webSocketImpl.wrap(mockTransportInput, mockTransportOutput);
//
//        when(mockWebSocketHandler.validateUpgradeReply((ByteBuffer) any())).thenReturn(true);
//        when(mockWebSocketHandler.createUpgradeRequest(_hostName, _webSocketPath, _webSocketPort, _webSocketProtocol, _additionalHeaders)).thenReturn("Request");
//        when(mockWebSocketHandler.unwrapBuffer((ByteBuffer) any())).thenReturn(WebSocketHandler.WebSocketMessageType.WEB_SOCKET_MESSAGE_TYPE_UNKNOWN);
//
//        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_NOT_STARTED);
//        transportWrapper.pending();
//        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_CONNECTING);
//        transportWrapper.process();
//        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_CONNECTED_FLOW);
//
//        String message = "Message";
//        ByteBuffer inputBuffer = webSocketImpl.getInputBuffer();
//        inputBuffer.clear();
//        inputBuffer.put(message.getBytes());
//
//        transportWrapper.process();
//        verify(mockTransportInput, times(1)).process();
//    }
//
    @Test
    public void testProcess_state_flow_changes_to_pong_after_ping_and_copies_the_buffer()
    {
        init();

        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();
        WebSocketHandlerImpl mockWebSocketHandler = mock(webSocketHandler.getClass());

        WebSocketImpl webSocketImpl = new WebSocketImpl();
        webSocketImpl.configure(_hostName, _webSocketPath, _webSocketPort, _webSocketProtocol, _additionalHeaders, mockWebSocketHandler);

        TransportInput mockTransportInput = mock(TransportInput.class);
        TransportOutput mockTransportOutput = mock(TransportOutput.class);

        TransportWrapper transportWrapper = webSocketImpl.wrap(mockTransportInput, mockTransportOutput);

        when(mockWebSocketHandler.validateUpgradeReply((ByteBuffer) any())).thenReturn(true);
        when(mockWebSocketHandler.createUpgradeRequest(_hostName, _webSocketPath, _webSocketPort, _webSocketProtocol, _additionalHeaders)).thenReturn("Request");
        when(mockWebSocketHandler.unwrapBuffer((ByteBuffer) any())).thenReturn(new WebSocketHandler.WebsocketTuple(7, WebSocketHandler.WebSocketMessageType.WEB_SOCKET_MESSAGE_TYPE_PING));

        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_NOT_STARTED);
        transportWrapper.pending();
        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_CONNECTING);
        transportWrapper.process();
        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_CONNECTED_FLOW);

        String message = "Message";
        ByteBuffer inputBuffer = webSocketImpl.getInputBuffer();
        inputBuffer.clear();
        inputBuffer.put(message.getBytes());

        transportWrapper.process();

        ByteBuffer pingBuffer = webSocketImpl.getPingBuffer();

        inputBuffer.flip();
        pingBuffer.flip();
        assertTrue(Arrays.equals(inputBuffer.array(), pingBuffer.array()));
        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_CONNECTED_PONG);
    }

    @Test
    public void testProcess_state_flow_changes_to_closing_after_close_and_copies_the_buffer()
    {
        init();

        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();
        WebSocketHandlerImpl mockWebSocketHandler = mock(webSocketHandler.getClass());

        WebSocketImpl webSocketImpl = new WebSocketImpl();
        webSocketImpl.configure(_hostName, _webSocketPath, _webSocketPort, _webSocketProtocol, _additionalHeaders, mockWebSocketHandler);

        TransportInput mockTransportInput = mock(TransportInput.class);
        TransportOutput mockTransportOutput = mock(TransportOutput.class);

        TransportWrapper transportWrapper = webSocketImpl.wrap(mockTransportInput, mockTransportOutput);

        when(mockWebSocketHandler.validateUpgradeReply((ByteBuffer) any())).thenReturn(true);
        when(mockWebSocketHandler.createUpgradeRequest(_hostName, _webSocketPath, _webSocketPort, _webSocketProtocol, _additionalHeaders)).thenReturn("Request");
        when(mockWebSocketHandler.unwrapBuffer((ByteBuffer) any())).thenReturn(new WebSocketHandler.WebsocketTuple(7, WebSocketHandler.WebSocketMessageType.WEB_SOCKET_MESSAGE_TYPE_CLOSE));

        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_NOT_STARTED);
        transportWrapper.pending();
        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_CONNECTING);
        transportWrapper.process();
        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_CONNECTED_FLOW);

        String message = "Message";
        ByteBuffer inputBuffer = webSocketImpl.getInputBuffer();
        inputBuffer.clear();
        inputBuffer.put(message.getBytes());

        transportWrapper.process();

        ByteBuffer pingBuffer = webSocketImpl.getPingBuffer();

        inputBuffer.flip();
        pingBuffer.flip();
        assertTrue(Arrays.equals(inputBuffer.array(), pingBuffer.array()));
        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_CONNECTED_CLOSING);
    }

    @Test
    public void testProcess_state_pong_changes_to_flow_head_closed()
    {
        init();

        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();
        WebSocketHandlerImpl mockWebSocketHandler = mock(webSocketHandler.getClass());

        WebSocketImpl webSocketImpl = new WebSocketImpl();
        webSocketImpl.configure(_hostName, _webSocketPath, _webSocketPort, _webSocketProtocol, _additionalHeaders, mockWebSocketHandler);

        TransportInput mockTransportInput = mock(TransportInput.class);
        TransportOutput mockTransportOutput = mock(TransportOutput.class);

        TransportWrapper transportWrapper = webSocketImpl.wrap(mockTransportInput, mockTransportOutput);

        when(mockWebSocketHandler.validateUpgradeReply((ByteBuffer) any())).thenReturn(true);
        when(mockWebSocketHandler.createUpgradeRequest(_hostName, _webSocketPath, _webSocketPort, _webSocketProtocol, _additionalHeaders)).thenReturn("Request");
        when(mockWebSocketHandler.unwrapBuffer((ByteBuffer) any())).thenReturn(new WebSocketHandler.WebsocketTuple(7, WebSocketHandler.WebSocketMessageType.WEB_SOCKET_MESSAGE_TYPE_PING));

        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_NOT_STARTED);
        transportWrapper.pending();
        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_CONNECTING);
        transportWrapper.process();
        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_CONNECTED_FLOW);

        String message = "Message";
        ByteBuffer inputBuffer = webSocketImpl.getInputBuffer();
        inputBuffer.clear();
        inputBuffer.put(message.getBytes());

        transportWrapper.process();

        ByteBuffer pingBuffer = webSocketImpl.getPingBuffer();

        inputBuffer.flip();
        pingBuffer.flip();
        assertTrue(Arrays.equals(inputBuffer.array(), pingBuffer.array()));
        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_CONNECTED_PONG);

        ByteBuffer outputBuffer = webSocketImpl.getOutputBuffer();
        outputBuffer.clear();
        transportWrapper.close_tail();

        transportWrapper.pending();
        assertTrue(transportWrapper.pending() == Transport.END_OF_STREAM);
        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_FAILED);
    }

    private byte[] createMessage(int size)
    {
        byte[] data = new byte[size];

        Random random = new SecureRandom();
        random.nextBytes(data);

        byte finbit = (byte) (FINBIT_MASK & 0xFF);
        byte opcode = (byte) OPCODE_MASK & 0x2;
        byte firstbyte = (byte) (finbit | opcode);
        byte secondbyte = (byte) (size-2);

        data[0] = firstbyte;
        data[1] = secondbyte;

        return data;
    }

    private byte[] getChunk(byte[] source, int chunkSize, int startPosition)
    {
        return Arrays.copyOfRange(source, startPosition, Math.min(source.length, startPosition+chunkSize));
    }

    @Test
    public void testProcess_multiple_WS_requests_chunks()
    {
        byte[] message1 = createMessage(100);
        byte[] message2 = createMessage(100);
        byte[] message3 = createMessage(100);

        byte[] concatenatedArray = new byte[message1.length + message2.length + message3.length];
        final byte[] expectedFinalArray = new byte[concatenatedArray.length - 6];
        final ByteBuffer actualFinalBuffer = ByteBufferUtils.newWriteableBuffer(4224);

        System.arraycopy(message1, 0, concatenatedArray, 0, message1.length);
        System.arraycopy(message2, 0, concatenatedArray, message1.length, message2.length);
        System.arraycopy(message3, 0, concatenatedArray, message1.length+message2.length, message3.length);

        System.arraycopy(message1, 2, expectedFinalArray, 0, message1.length-2);
        System.arraycopy(message2, 2, expectedFinalArray, message1.length-2, message2.length-2);
        System.arraycopy(message3, 2, expectedFinalArray, message1.length+message2.length-4, message3.length-2);

        int chunkCount = 10;
        int bytesPerChunk = concatenatedArray.length/chunkCount;

        init();

        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();
        WebSocketHandlerImpl mockWebSocketHandler = mock(webSocketHandler.getClass());

        WebSocketImpl webSocketImpl = new WebSocketImpl();
        webSocketImpl.configure(_hostName, _webSocketPath, _webSocketPort, _webSocketProtocol, _additionalHeaders, mockWebSocketHandler);

        TransportInput mockTransportInput = spy(new TransportInput() {
            ByteBuffer bb = ByteBufferUtils.newWriteableBuffer(4224);

            @Override
            public int capacity() {
                return bb.remaining();
            }

            @Override
            public int position() {
                return bb.position();
            }

            @Override
            public ByteBuffer tail() throws TransportException {
                return bb;
            }

            @Override
            public void process() throws TransportException {
                bb.flip();
                actualFinalBuffer.put(bb);
            }

            @Override
            public void close_tail() {
            }
        });
        TransportOutput mockTransportOutput = mock(TransportOutput.class);
        //doNothing().when(mockTransportInput).process();
        doNothing().when(mockTransportInput).close_tail();
        TransportWrapper transportWrapper = webSocketImpl.wrap(mockTransportInput, mockTransportOutput);

        when(mockWebSocketHandler.validateUpgradeReply((ByteBuffer) any())).thenReturn(true);
        when(mockWebSocketHandler.createUpgradeRequest(_hostName, _webSocketPath, _webSocketPort, _webSocketProtocol, _additionalHeaders)).thenReturn("Request");
        when(mockWebSocketHandler.unwrapBuffer((ByteBuffer) any())).thenAnswer(new Answer<WebSocketHandler.WebsocketTuple>(){
            @Override
            public WebSocketHandler.WebsocketTuple answer(InvocationOnMock invocation) throws Throwable {
                Object[] arguments = invocation.getArguments();
                ByteBuffer bb = (ByteBuffer)arguments[0];
                bb.position(2);
                return new WebSocketHandler.WebsocketTuple(98, WebSocketHandler.WebSocketMessageType.WEB_SOCKET_MESSAGE_TYPE_AMQP);
            }
        });

        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_NOT_STARTED);
        transportWrapper.pending();
        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_CONNECTING);
        transportWrapper.process();
        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_CONNECTED_FLOW);

        ByteBuffer inputBuffer = webSocketImpl.getInputBuffer();
        ByteBuffer wsInputBuffer = webSocketImpl.getWsInputBuffer();

        for (int i = 0; i < chunkCount; i++)
        {
            byte[] message = getChunk(concatenatedArray, bytesPerChunk, i*bytesPerChunk);
            inputBuffer.clear();
            inputBuffer.put(message);
            transportWrapper.process();
            mockTransportInput.tail().clear();
        }

        actualFinalBuffer.flip();
        byte[] actualFinalArray = new byte[actualFinalBuffer.limit()];
        actualFinalBuffer.get(actualFinalArray);
        assertTrue(Arrays.equals(expectedFinalArray, actualFinalArray));

        //Subtract 1 because the first 2 bytes are used as the header which come in 2 separate chunks
        //verify(mockTransportInput, times(chunkCount-1)).process();
    }

    @Test
    public void testProcess_small_chunks_one_byte_payload_no_mask()
    {
        final int payloadLength = 125;
        int chunkSize = 10;
        int chunkCount = 1 + payloadLength / chunkSize;

        init();

        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();
        WebSocketHandlerImpl mockWebSocketHandler = mock(webSocketHandler.getClass());

        WebSocketImpl webSocketImpl = new WebSocketImpl();
        webSocketImpl.configure(_hostName, _webSocketPath, _webSocketPort, _webSocketProtocol, _additionalHeaders, mockWebSocketHandler);

        TransportInput mockTransportInput = spy(new TransportInput() {
            ByteBuffer bb = ByteBufferUtils.newWriteableBuffer(4224);

            @Override
            public int capacity() {
                return bb.remaining();
            }

            @Override
            public int position() {
                return bb.position();
            }

            @Override
            public ByteBuffer tail() throws TransportException {
                return bb;
            }

            @Override
            public void process() throws TransportException {
            }

            @Override
            public void close_tail() {
            }
        });
        TransportOutput mockTransportOutput = mock(TransportOutput.class);
        doNothing().when(mockTransportInput).process();
        doNothing().when(mockTransportInput).close_tail();
        TransportWrapper transportWrapper = webSocketImpl.wrap(mockTransportInput, mockTransportOutput);

        when(mockWebSocketHandler.validateUpgradeReply((ByteBuffer) any())).thenReturn(true);
        when(mockWebSocketHandler.createUpgradeRequest(_hostName, _webSocketPath, _webSocketPort, _webSocketProtocol, _additionalHeaders)).thenReturn("Request");
        when(mockWebSocketHandler.unwrapBuffer((ByteBuffer) any())).thenAnswer(new Answer<WebSocketHandler.WebsocketTuple>(){
            @Override
            public WebSocketHandler.WebsocketTuple answer(InvocationOnMock invocation) throws Throwable {
                Object[] arguments = invocation.getArguments();
                ByteBuffer bb = (ByteBuffer)arguments[0];
                bb.position(2);
                return new WebSocketHandler.WebsocketTuple(payloadLength, WebSocketHandler.WebSocketMessageType.WEB_SOCKET_MESSAGE_TYPE_AMQP);
            }
        });

        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_NOT_STARTED);
        transportWrapper.pending();
        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_CONNECTING);
        transportWrapper.process();
        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_CONNECTED_FLOW);

        ByteBuffer inputBuffer = webSocketImpl.getInputBuffer();
        ByteBuffer wsInputBuffer = webSocketImpl.getWsInputBuffer();

        byte[] message = new byte[chunkSize];

        byte finbit = (byte) (FINBIT_MASK & 0xFF);
        byte opcode = (byte) OPCODE_MASK & 0x2;
        byte firstbyte = (byte) (finbit | opcode);
        byte secondbyte = (byte) payloadLength;

        message[0] = firstbyte;
        message[1] = secondbyte;

        int currentLength = 0;
        for (int i = 0; i < chunkCount + 1; i++)
        {
            if (i == 0)
            {
                inputBuffer.clear();
                inputBuffer.put(firstbyte);
                currentLength += 1;
                transportWrapper.process();
            }
            else if (i == 1)
            {
                inputBuffer.clear();
                inputBuffer.put(secondbyte);
                currentLength += 1;
                transportWrapper.process();
            }
            else
            {
                char c = (char) (65+i & 0xFF);
                for (int j = 0; j < chunkSize; j++)
                {
                    message[j] += c;
                }
                inputBuffer.clear();
                inputBuffer.put(message);
                currentLength += chunkSize;
                transportWrapper.process();

                //Get the buffer from the underlying input to check
                ByteBuffer bb = mockTransportInput.tail();
                bb.flip();
                byte[] transportInputArray = new byte[bb.remaining()];
                bb.duplicate().get(transportInputArray);

                //Check that the message chunk we sent is what the underlying input is going to process
                assertTrue(Arrays.equals(message, transportInputArray));
                mockTransportInput.tail().clear();
            }

            assertEquals(inputBuffer.position(), 0);
            assertEquals(inputBuffer.limit(), inputBuffer.capacity());
        }

        //Subtract 1 because the first 2 bytes are used as the header which come in 2 separate chunks
        verify(mockTransportInput, times(chunkCount-1)).process();
    }

    @Test
    public void testHead_websocket_not_enabled()
    {
        init();

        WebSocketImpl webSocketImpl = new WebSocketImpl();

        TransportInput mockTransportInput = mock(TransportInput.class);
        TransportOutput mockTransportOutput = mock(TransportOutput.class);

        TransportWrapper transportWrapper = webSocketImpl.wrap(mockTransportInput, mockTransportOutput);

        webSocketImpl._isWebSocketEnabled = false;

        transportWrapper.head();
        verify(mockTransportOutput, times(1)).head();
    }

    @Test
    public void testHead_state_not_started()
    {
        init();

        WebSocketImpl webSocketImpl = new WebSocketImpl();

        TransportInput mockTransportInput = mock(TransportInput.class);
        TransportOutput mockTransportOutput = mock(TransportOutput.class);

        TransportWrapper transportWrapper = webSocketImpl.wrap(mockTransportInput, mockTransportOutput);

        webSocketImpl._isWebSocketEnabled = true;

        transportWrapper.head();
        verify(mockTransportOutput, times(1)).head();
    }

    @Test
    public void testHead_state_connecting()
    {
        init();

        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();
        WebSocketHandlerImpl mockWebSocketHandler = mock(webSocketHandler.getClass());

        WebSocketImpl webSocketImpl = new WebSocketImpl();
        webSocketImpl.configure(_hostName, _webSocketPath, _webSocketPort, _webSocketProtocol, _additionalHeaders, mockWebSocketHandler);

        TransportInput mockTransportInput = mock(TransportInput.class);
        TransportOutput mockTransportOutput = mock(TransportOutput.class);

        TransportWrapper transportWrapper = webSocketImpl.wrap(mockTransportInput, mockTransportOutput);

        String request = "Request";
        when(mockWebSocketHandler.validateUpgradeReply((ByteBuffer) any())).thenReturn(true);
        when(mockWebSocketHandler.createUpgradeRequest(_hostName, _webSocketPath, _webSocketPort, _webSocketProtocol, _additionalHeaders)).thenReturn(request);
        //when(mockWebSocketHandler.unwrapBuffer((ByteBuffer) any())).thenReturn(WebSocketHandler.WebSocketMessageType.WEB_SOCKET_MESSAGE_TYPE_PING);

        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_NOT_STARTED);
        transportWrapper.pending();
        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_CONNECTING);

        ByteBuffer actual = transportWrapper.head();
        byte[] a = new byte[actual.remaining()];
        actual.get(a);

        assertTrue(Arrays.equals(request.getBytes(), a));
    }

    @Test
    public void testHead_state_flow_underlying_head_empty()
    {
        init();

        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();
        WebSocketHandlerImpl mockWebSocketHandler = mock(webSocketHandler.getClass());

        WebSocketImpl webSocketImpl = new WebSocketImpl();
        webSocketImpl.configure(_hostName, _webSocketPath, _webSocketPort, _webSocketProtocol, _additionalHeaders, mockWebSocketHandler);

        TransportInput mockTransportInput = mock(TransportInput.class);
        TransportOutput mockTransportOutput = mock(TransportOutput.class);

        TransportWrapper transportWrapper = webSocketImpl.wrap(mockTransportInput, mockTransportOutput);

        String request = "Request";
        when(mockWebSocketHandler.validateUpgradeReply((ByteBuffer) any())).thenReturn(true);
        when(mockWebSocketHandler.createUpgradeRequest(_hostName, _webSocketPath, _webSocketPort, _webSocketProtocol, _additionalHeaders)).thenReturn(request);
        //when(mockWebSocketHandler.unwrapBuffer((ByteBuffer) any())).thenReturn(WebSocketHandler.WebSocketMessageType.WEB_SOCKET_MESSAGE_TYPE_PING);

        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_NOT_STARTED);
        transportWrapper.pending();
        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_CONNECTING);
        transportWrapper.process();
        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_CONNECTED_FLOW);

        ByteBuffer actual = transportWrapper.head();
        byte[] a = new byte[actual.remaining()];
        actual.get(a);

        assertTrue(Arrays.equals(request.getBytes(), a));
        verify(mockTransportOutput, times(0)).head();
    }

    @Test
    public void testHead_state_flow_underlying_head_not_empty()
    {
        init();

        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();
        WebSocketHandlerImpl mockWebSocketHandler = mock(webSocketHandler.getClass());

        WebSocketImpl webSocketImpl = new WebSocketImpl();
        webSocketImpl.configure(_hostName, _webSocketPath, _webSocketPort, _webSocketProtocol, _additionalHeaders, mockWebSocketHandler);

        TransportInput mockTransportInput = mock(TransportInput.class);
        TransportOutput mockTransportOutput = mock(TransportOutput.class);

        TransportWrapper transportWrapper = webSocketImpl.wrap(mockTransportInput, mockTransportOutput);

        String request = "Request";
        when(mockWebSocketHandler.validateUpgradeReply((ByteBuffer) any())).thenReturn(true);
        when(mockWebSocketHandler.createUpgradeRequest(_hostName, _webSocketPath, _webSocketPort, _webSocketProtocol, _additionalHeaders)).thenReturn(request);
        //when(mockWebSocketHandler.unwrapBuffer((ByteBuffer) any())).thenReturn(WebSocketHandler.WebSocketMessageType.WEB_SOCKET_MESSAGE_TYPE_PING);

        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_NOT_STARTED);
        transportWrapper.pending();
        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_CONNECTING);
        transportWrapper.process();
        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_CONNECTED_FLOW);

        when(mockTransportOutput.pending()).thenReturn(1024);

        ByteBuffer actual = transportWrapper.head();
        byte[] a = new byte[actual.remaining()];
        actual.get(a);

        assertTrue(Arrays.equals(request.getBytes(), a));
        verify(mockWebSocketHandler, times(1)).wrapBuffer((ByteBuffer) any(), (ByteBuffer) any());
        verify(mockTransportOutput, times(1)).head();
    }

    @Test
    public void testHead_state_pong()
    {
        init();

        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();
        WebSocketHandlerImpl mockWebSocketHandler = mock(webSocketHandler.getClass());

        WebSocketImpl webSocketImpl = new WebSocketImpl();
        webSocketImpl.configure(_hostName, _webSocketPath, _webSocketPort, _webSocketProtocol, _additionalHeaders, mockWebSocketHandler);

        TransportInput mockTransportInput = mock(TransportInput.class);
        TransportOutput mockTransportOutput = mock(TransportOutput.class);

        TransportWrapper transportWrapper = webSocketImpl.wrap(mockTransportInput, mockTransportOutput);

        String request = "Request";
        when(mockWebSocketHandler.validateUpgradeReply((ByteBuffer) any())).thenReturn(true);
        when(mockWebSocketHandler.createUpgradeRequest(_hostName, _webSocketPath, _webSocketPort, _webSocketProtocol, _additionalHeaders)).thenReturn(request);
        when(mockWebSocketHandler.unwrapBuffer((ByteBuffer) any())).thenReturn(new WebSocketHandler.WebsocketTuple(7, WebSocketHandler.WebSocketMessageType.WEB_SOCKET_MESSAGE_TYPE_PING));

        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_NOT_STARTED);
        transportWrapper.pending();
        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_CONNECTING);
        transportWrapper.process();
        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_CONNECTED_FLOW);

        String message = "Message";
        ByteBuffer inputBuffer = webSocketImpl.getInputBuffer();
        inputBuffer.clear();
        inputBuffer.put(message.getBytes());

        transportWrapper.process();

        ByteBuffer pingBuffer = webSocketImpl.getPingBuffer();

        inputBuffer.flip();
        pingBuffer.flip();
        assertTrue(Arrays.equals(inputBuffer.array(), pingBuffer.array()));
        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_CONNECTED_PONG);

        ByteBuffer actual = transportWrapper.head();
        byte[] a = new byte[actual.remaining()];
        actual.get(a);

        assertTrue(Arrays.equals(request.getBytes(), a));
    }

    @Test
    public void testPop_websocket_not_enabled()
    {
        init();

        WebSocketImpl webSocketImpl = new WebSocketImpl();

        TransportInput mockTransportInput = mock(TransportInput.class);
        TransportOutput mockTransportOutput = mock(TransportOutput.class);

        TransportWrapper transportWrapper = webSocketImpl.wrap(mockTransportInput, mockTransportOutput);

        webSocketImpl._isWebSocketEnabled = false;

        String message = "Message";
        ByteBuffer outputBuffer = webSocketImpl.getOutputBuffer();
        outputBuffer.clear();
        outputBuffer.put(message.getBytes());

        transportWrapper.pop(message.getBytes().length);

        verify(mockTransportOutput, times(1)).pop(message.getBytes().length);
    }

    @Test
    public void testPop_websocket_not_started()
    {
        init();

        WebSocketImpl webSocketImpl = new WebSocketImpl();

        TransportInput mockTransportInput = mock(TransportInput.class);
        TransportOutput mockTransportOutput = mock(TransportOutput.class);

        TransportWrapper transportWrapper = webSocketImpl.wrap(mockTransportInput, mockTransportOutput);

        webSocketImpl._isWebSocketEnabled = true;

        String message = "Message";
        ByteBuffer outputBuffer = webSocketImpl.getOutputBuffer();
        outputBuffer.clear();
        outputBuffer.put(message.getBytes());

        transportWrapper.pop(message.getBytes().length);

        verify(mockTransportOutput, times(1)).pop(message.getBytes().length);
    }

    @Test
    public void testPop_websocket_connecting()
    {
        init();

        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();
        WebSocketHandlerImpl mockWebSocketHandler = mock(webSocketHandler.getClass());

        WebSocketImpl webSocketImpl = new WebSocketImpl();
        webSocketImpl.configure(_hostName, _webSocketPath, _webSocketPort, _webSocketProtocol, _additionalHeaders, mockWebSocketHandler);

        TransportInput mockTransportInput = mock(TransportInput.class);
        TransportOutput mockTransportOutput = mock(TransportOutput.class);

        TransportWrapper transportWrapper = webSocketImpl.wrap(mockTransportInput, mockTransportOutput);

        String request = "Request";
        when(mockWebSocketHandler.validateUpgradeReply((ByteBuffer) any())).thenReturn(true);
        when(mockWebSocketHandler.createUpgradeRequest(_hostName, _webSocketPath, _webSocketPort, _webSocketProtocol, _additionalHeaders)).thenReturn(request);
        //when(mockWebSocketHandler.unwrapBuffer((ByteBuffer) any())).thenReturn(WebSocketHandler.WebSocketMessageType.WEB_SOCKET_MESSAGE_TYPE_PING);

        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_NOT_STARTED);
        transportWrapper.pending();
        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_CONNECTING);

        String message = "Message";
        ByteBuffer outputBuffer = webSocketImpl.getOutputBuffer();
        outputBuffer.clear();
        outputBuffer.put(message.getBytes());

        transportWrapper.pop(message.getBytes().length);

        ByteBuffer actual = webSocketImpl.getOutputBuffer();
        assertTrue(actual.limit() == _allocatedWebSocketBufferSize);
        assertTrue(actual.position() == 0);
    }

    @Test
    public void testPop_state_connected_flow()
    {
        init();

        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();
        WebSocketHandlerImpl mockWebSocketHandler = mock(webSocketHandler.getClass());

        WebSocketImpl webSocketImpl = new WebSocketImpl();
        webSocketImpl.configure(_hostName, _webSocketPath, _webSocketPort, _webSocketProtocol, _additionalHeaders, mockWebSocketHandler);

        TransportInput mockTransportInput = mock(TransportInput.class);
        TransportOutput mockTransportOutput = mock(TransportOutput.class);

        TransportWrapper transportWrapper = webSocketImpl.wrap(mockTransportInput, mockTransportOutput);

        String request = "Request";
        when(mockWebSocketHandler.validateUpgradeReply((ByteBuffer) any())).thenReturn(true);
        when(mockWebSocketHandler.createUpgradeRequest(_hostName, _webSocketPath, _webSocketPort, _webSocketProtocol, _additionalHeaders)).thenReturn(request);
        //when(mockWebSocketHandler.unwrapBuffer((ByteBuffer) any())).thenReturn(WebSocketHandler.WebSocketMessageType.WEB_SOCKET_MESSAGE_TYPE_PING);

        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_NOT_STARTED);
        transportWrapper.pending();
        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_CONNECTING);
        transportWrapper.process();
        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_CONNECTED_FLOW);

        int webSocketHeaderSize = transportWrapper.pending();

        String message = "Message";
        ByteBuffer outputBuffer = webSocketImpl.getOutputBuffer();

        outputBuffer.clear();
        outputBuffer.put(message.getBytes());

        transportWrapper.pop(message.getBytes().length);

        ByteBuffer actual = webSocketImpl.getOutputBuffer();
        assertTrue(actual.limit() == _allocatedWebSocketBufferSize);
        assertTrue(actual.position() == 0);

        verify(mockTransportOutput, times(1)).pop(message.getBytes().length - webSocketHeaderSize);
    }

    @Test
    public void testPop_state_connected_pong()
    {
        init();

        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();
        WebSocketHandlerImpl mockWebSocketHandler = mock(webSocketHandler.getClass());

        WebSocketImpl webSocketImpl = new WebSocketImpl();
        webSocketImpl.configure(_hostName, _webSocketPath, _webSocketPort, _webSocketProtocol, _additionalHeaders, mockWebSocketHandler);

        TransportInput mockTransportInput = mock(TransportInput.class);
        TransportOutput mockTransportOutput = mock(TransportOutput.class);

        TransportWrapper transportWrapper = webSocketImpl.wrap(mockTransportInput, mockTransportOutput);

        String request = "Request";
        when(mockWebSocketHandler.validateUpgradeReply((ByteBuffer) any())).thenReturn(true);
        when(mockWebSocketHandler.createUpgradeRequest(_hostName, _webSocketPath, _webSocketPort, _webSocketProtocol, _additionalHeaders)).thenReturn(request);
        when(mockWebSocketHandler.unwrapBuffer((ByteBuffer) any())).thenReturn(new WebSocketHandler.WebsocketTuple(7, WebSocketHandler.WebSocketMessageType.WEB_SOCKET_MESSAGE_TYPE_PING));

        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_NOT_STARTED);
        transportWrapper.pending();
        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_CONNECTING);
        transportWrapper.process();
        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_CONNECTED_FLOW);

        String message = "Message";
        ByteBuffer inputBuffer = webSocketImpl.getInputBuffer();
        inputBuffer.clear();
        inputBuffer.put(message.getBytes());

        transportWrapper.process();

        ByteBuffer pingBuffer = webSocketImpl.getPingBuffer();

        inputBuffer.flip();
        pingBuffer.flip();
        assertTrue(Arrays.equals(inputBuffer.array(), pingBuffer.array()));
        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_CONNECTED_PONG);

        ByteBuffer outputBuffer = webSocketImpl.getOutputBuffer();

        transportWrapper.pop(message.getBytes().length);

        ByteBuffer actual = webSocketImpl.getOutputBuffer();
        assertTrue(actual.limit() == _allocatedWebSocketBufferSize);
        assertTrue(actual.position() == 0);

        verify(mockTransportOutput, times(1)).pop(message.getBytes().length);
    }

    @Test
    public void testPop_websocket_connecting_outbut_buffer_is_not_empty()
    {
        init();

        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();
        WebSocketHandlerImpl mockWebSocketHandler = mock(webSocketHandler.getClass());

        WebSocketImpl webSocketImpl = new WebSocketImpl();
        webSocketImpl.configure(_hostName, _webSocketPath, _webSocketPort, _webSocketProtocol, _additionalHeaders, mockWebSocketHandler);

        TransportInput mockTransportInput = mock(TransportInput.class);
        TransportOutput mockTransportOutput = mock(TransportOutput.class);

        TransportWrapper transportWrapper = webSocketImpl.wrap(mockTransportInput, mockTransportOutput);

        String request = "Request";
        when(mockWebSocketHandler.validateUpgradeReply((ByteBuffer) any())).thenReturn(true);
        when(mockWebSocketHandler.createUpgradeRequest(_hostName, _webSocketPath, _webSocketPort, _webSocketProtocol, _additionalHeaders)).thenReturn(request);
        //when(mockWebSocketHandler.unwrapBuffer((ByteBuffer) any())).thenReturn(WebSocketHandler.WebSocketMessageType.WEB_SOCKET_MESSAGE_TYPE_PING);

        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_NOT_STARTED);
        transportWrapper.pending();
        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_CONNECTING);

        String message = "Message";
        ByteBuffer outputBuffer = webSocketImpl.getOutputBuffer();
        outputBuffer.clear();
        outputBuffer.put(message.getBytes());
        outputBuffer.flip();

        transportWrapper.pop(message.getBytes().length);
        verify(mockTransportOutput, times(1)).pop(message.getBytes().length);
    }

    @Test
    public void testPop_state_connected_flow_outbut_buffer_is_not_empty()
    {
        init();

        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();
        WebSocketHandlerImpl mockWebSocketHandler = mock(webSocketHandler.getClass());

        WebSocketImpl webSocketImpl = new WebSocketImpl();
        webSocketImpl.configure(_hostName, _webSocketPath, _webSocketPort, _webSocketProtocol, _additionalHeaders, mockWebSocketHandler);

        TransportInput mockTransportInput = mock(TransportInput.class);
        TransportOutput mockTransportOutput = mock(TransportOutput.class);

        TransportWrapper transportWrapper = webSocketImpl.wrap(mockTransportInput, mockTransportOutput);

        String request = "Request";
        when(mockWebSocketHandler.validateUpgradeReply((ByteBuffer) any())).thenReturn(true);
        when(mockWebSocketHandler.createUpgradeRequest(_hostName, _webSocketPath, _webSocketPort, _webSocketProtocol, _additionalHeaders)).thenReturn(request);
        //when(mockWebSocketHandler.unwrapBuffer((ByteBuffer) any())).thenReturn(WebSocketHandler.WebSocketMessageType.WEB_SOCKET_MESSAGE_TYPE_PING);

        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_NOT_STARTED);
        transportWrapper.pending();
        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_CONNECTING);
        transportWrapper.process();
        assertTrue(webSocketImpl.getState() == WebSocket.WebSocketState.PN_WS_CONNECTED_FLOW);

        String message = "Message";
        ByteBuffer outputBuffer = webSocketImpl.getOutputBuffer();

        outputBuffer.clear();
        outputBuffer.put(message.getBytes());
        outputBuffer.flip();

        transportWrapper.pop(message.getBytes().length);

        verify(mockTransportOutput, times(1)).pop(message.getBytes().length);
    }

    @Test
    public void testCapacity_enabled()
    {
        init();

        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();
        WebSocketHandlerImpl mockWebSocketHandler = mock(webSocketHandler.getClass());

        WebSocketImpl webSocketImpl = new WebSocketImpl();
        webSocketImpl.configure(_hostName, _webSocketPath, _webSocketPort, _webSocketProtocol, _additionalHeaders, mockWebSocketHandler);

        TransportInput mockTransportInput = mock(TransportInput.class);
        TransportOutput mockTransportOutput = mock(TransportOutput.class);

        TransportWrapper transportWrapper = webSocketImpl.wrap(mockTransportInput, mockTransportOutput);

        String message = "Message";
        ByteBuffer inputBuffer = webSocketImpl.getInputBuffer();
        inputBuffer.clear();
        inputBuffer.put(message.getBytes());
        inputBuffer.flip();

        webSocketImpl._isWebSocketEnabled = true;

        int actual = transportWrapper.capacity();

        assertTrue(message.length() == actual);
        verify(mockTransportInput, times(0)).capacity();
    }

    @Test
    public void testCapacity_enabled_tail_closed()
    {
        init();

        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();
        WebSocketHandlerImpl mockWebSocketHandler = mock(webSocketHandler.getClass());

        WebSocketImpl webSocketImpl = new WebSocketImpl();
        webSocketImpl.configure(_hostName, _webSocketPath, _webSocketPort, _webSocketProtocol, _additionalHeaders, mockWebSocketHandler);

        TransportInput mockTransportInput = mock(TransportInput.class);
        TransportOutput mockTransportOutput = mock(TransportOutput.class);

        TransportWrapper transportWrapper = webSocketImpl.wrap(mockTransportInput, mockTransportOutput);

        String message = "Message";
        ByteBuffer inputBuffer = webSocketImpl.getInputBuffer();
        inputBuffer.clear();
        inputBuffer.put(message.getBytes());
        inputBuffer.flip();

        webSocketImpl._isWebSocketEnabled = true;
        transportWrapper.close_tail();

        int actual = transportWrapper.capacity();

        assertTrue(Transport.END_OF_STREAM == actual);
        verify(mockTransportInput, times(0)).capacity();
    }

    @Test
    public void testCapacity_not_enabled()
    {
        init();

        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();
        WebSocketHandlerImpl mockWebSocketHandler = mock(webSocketHandler.getClass());

        WebSocketImpl webSocketImpl = new WebSocketImpl();
        webSocketImpl.configure(_hostName, _webSocketPath, _webSocketPort, _webSocketProtocol, _additionalHeaders, mockWebSocketHandler);

        TransportInput mockTransportInput = mock(TransportInput.class);
        TransportOutput mockTransportOutput = mock(TransportOutput.class);

        TransportWrapper transportWrapper = webSocketImpl.wrap(mockTransportInput, mockTransportOutput);

        String message = "Message";
        ByteBuffer inputBuffer = webSocketImpl.getInputBuffer();
        inputBuffer.clear();
        inputBuffer.put(message.getBytes());
        inputBuffer.flip();

        webSocketImpl._isWebSocketEnabled = false;
        transportWrapper.capacity();

        verify(mockTransportInput, times(1)).capacity();
    }

    @Test
    public void testPosition_enabled()
    {
        init();

        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();
        WebSocketHandlerImpl mockWebSocketHandler = mock(webSocketHandler.getClass());

        WebSocketImpl webSocketImpl = new WebSocketImpl();
        webSocketImpl.configure(_hostName, _webSocketPath, _webSocketPort, _webSocketProtocol, _additionalHeaders, mockWebSocketHandler);

        TransportInput mockTransportInput = mock(TransportInput.class);
        TransportOutput mockTransportOutput = mock(TransportOutput.class);

        TransportWrapper transportWrapper = webSocketImpl.wrap(mockTransportInput, mockTransportOutput);

        String message = "Message";
        ByteBuffer inputBuffer = webSocketImpl.getInputBuffer();
        inputBuffer.clear();
        inputBuffer.put(message.getBytes());

        webSocketImpl._isWebSocketEnabled = true;

        int actual = transportWrapper.position();

        assertTrue(message.length() == actual);
        verify(mockTransportInput, times(0)).position();
    }

    @Test
    public void testPosition_enabled_tail_closed()
    {
        init();

        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();
        WebSocketHandlerImpl mockWebSocketHandler = mock(webSocketHandler.getClass());

        WebSocketImpl webSocketImpl = new WebSocketImpl();
        webSocketImpl.configure(_hostName, _webSocketPath, _webSocketPort, _webSocketProtocol, _additionalHeaders, mockWebSocketHandler);

        TransportInput mockTransportInput = mock(TransportInput.class);
        TransportOutput mockTransportOutput = mock(TransportOutput.class);

        TransportWrapper transportWrapper = webSocketImpl.wrap(mockTransportInput, mockTransportOutput);

        String message = "Message";
        ByteBuffer inputBuffer = webSocketImpl.getInputBuffer();
        inputBuffer.clear();
        inputBuffer.put(message.getBytes());

        webSocketImpl._isWebSocketEnabled = true;
        transportWrapper.close_tail();

        int actual = transportWrapper.position();

        assertTrue(Transport.END_OF_STREAM == actual);
        verify(mockTransportInput, times(0)).position();
    }

    @Test
    public void testPosition_not_enabled()
    {
        init();

        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();
        WebSocketHandlerImpl mockWebSocketHandler = mock(webSocketHandler.getClass());

        WebSocketImpl webSocketImpl = new WebSocketImpl();
        webSocketImpl.configure(_hostName, _webSocketPath, _webSocketPort, _webSocketProtocol, _additionalHeaders, mockWebSocketHandler);

        TransportInput mockTransportInput = mock(TransportInput.class);
        TransportOutput mockTransportOutput = mock(TransportOutput.class);

        TransportWrapper transportWrapper = webSocketImpl.wrap(mockTransportInput, mockTransportOutput);

        String message = "Message";
        ByteBuffer inputBuffer = webSocketImpl.getInputBuffer();
        inputBuffer.clear();
        inputBuffer.put(message.getBytes());

        webSocketImpl._isWebSocketEnabled = false;
        transportWrapper.position();

        verify(mockTransportInput, times(1)).position();
    }

    @Test
    public void testTail()
    {
        init();

        WebSocketImpl webSocketImpl = new WebSocketImpl();

        TransportInput mockTransportInput = mock(TransportInput.class);
        TransportOutput mockTransportOutput = mock(TransportOutput.class);

        TransportWrapper transportWrapper = webSocketImpl.wrap(mockTransportInput, mockTransportOutput);

        webSocketImpl._isWebSocketEnabled = true;

        String message = "Message";
        ByteBuffer inputBuffer = webSocketImpl.getInputBuffer();
        inputBuffer.clear();
        inputBuffer.put(message.getBytes());
        inputBuffer.flip();

        ByteBuffer actual = transportWrapper.tail();
        byte[] a = new byte[actual.remaining()];
        actual.get(a);

        assertTrue(Arrays.equals(message.getBytes(), a));
        verify(mockTransportInput, times(0)).tail();
    }

    @Test
    public void testTail_websocket_not_enabled()
    {
        init();

        WebSocketImpl webSocketImpl = new WebSocketImpl();

        TransportInput mockTransportInput = mock(TransportInput.class);
        TransportOutput mockTransportOutput = mock(TransportOutput.class);

        TransportWrapper transportWrapper = webSocketImpl.wrap(mockTransportInput, mockTransportOutput);

        webSocketImpl._isWebSocketEnabled = false;

        transportWrapper.tail();
        verify(mockTransportInput, times(1)).tail();
    }

    @Test
    public void testToString()
    {
        init();

        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();
        WebSocketHandlerImpl mockWebSocketHandler = mock(webSocketHandler.getClass());

        WebSocketImpl webSocketImpl = new WebSocketImpl();
        webSocketImpl.configure(_hostName, _webSocketPath, _webSocketPort, _webSocketProtocol, _additionalHeaders, mockWebSocketHandler);
        webSocketImpl._isWebSocketEnabled = true;

        String actual = webSocketImpl.toString();

        String expexted1 = "WebSocketImpl [isWebSocketEnabled=true" +
                ", state=PN_WS_NOT_STARTED" +
                ", protocol=" + _webSocketProtocol +
                ", host=" + _hostName +
                ", path=" + _webSocketPath +
                ", port=" + _webSocketPort;

        String expected2 = ", additionalHeaders=header3:content3, header2:content2, header1:content1]";

        assertTrue(actual.startsWith(expexted1));
        actual = actual.substring(expexted1.length());
        assertTrue(actual.equals(expected2));
    }

    @Test
    public void testToString_no_additional_headers()
    {
        init();

        WebSocketHandlerImpl webSocketHandler = new WebSocketHandlerImpl();
        WebSocketHandlerImpl mockWebSocketHandler = mock(webSocketHandler.getClass());

        WebSocketImpl webSocketImpl = new WebSocketImpl();
        webSocketImpl.configure(_hostName, _webSocketPath, _webSocketPort, _webSocketProtocol, null, mockWebSocketHandler);
        webSocketImpl._isWebSocketEnabled = true;

        String actual = webSocketImpl.toString();

        String expexted = "WebSocketImpl [isWebSocketEnabled=true" +
                ", state=PN_WS_NOT_STARTED" +
                ", protocol=" + _webSocketProtocol +
                ", host=" + _hostName +
                ", path=" + _webSocketPath +
                ", port=" + _webSocketPort + "]";

        assertEquals("Unexpected value for toString()", expexted, actual);
    }
}
