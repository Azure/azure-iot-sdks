/*
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

import com.microsoft.azure.iothub.ws.WebSocketHeader;
import org.apache.qpid.proton.engine.impl.TransportWrapper;
import org.junit.Test;

import static org.junit.Assert.assertEquals;
import static org.mockito.Mockito.mock;

public class WebSocketSnifferTest
{
    @Test
    public void testMakeDetermination_wrapper1()
    {
        TransportWrapper mockTransportWrapper1 = mock(TransportWrapper.class);
        TransportWrapper mockTransportWrapper2 = mock(TransportWrapper.class);

        WebSocketSniffer webSocketSniffer = new WebSocketSniffer(mockTransportWrapper1, mockTransportWrapper2);

        assertEquals("Incorrect header size", WebSocketHeader.MIN_HEADER_LENGTH_MASKED, webSocketSniffer.bufferSize());

        byte[] bytes = new byte[WebSocketHeader.MIN_HEADER_LENGTH_MASKED];
        bytes[0] = WebSocketHeader.FINAL_OPCODE_BINARY;

        webSocketSniffer.makeDetermination(bytes);
        assertEquals("Incorrect wrapper selected", mockTransportWrapper1, webSocketSniffer.getSelectedTransportWrapper());
    }

    @Test
    public void testMakeDetermination_wrapper2()
    {
        TransportWrapper mockTransportWrapper1 = mock(TransportWrapper.class);
        TransportWrapper mockTransportWrapper2 = mock(TransportWrapper.class);

        WebSocketSniffer webSocketSniffer = new WebSocketSniffer(mockTransportWrapper1, mockTransportWrapper2);

        assertEquals("Incorrect header size", WebSocketHeader.MIN_HEADER_LENGTH_MASKED, webSocketSniffer.bufferSize());

        byte[] bytes = new byte[WebSocketHeader.MIN_HEADER_LENGTH_MASKED];
        bytes[0] = (byte) 0x81;

        webSocketSniffer.makeDetermination(bytes);
        assertEquals("Incorrect wrapper selected", mockTransportWrapper2, webSocketSniffer.getSelectedTransportWrapper());
    }

    @Test(expected = IllegalArgumentException.class)
    public void testMakeDetermination_insufficient_bytes()
    {
        TransportWrapper mockTransportWrapper1 = mock(TransportWrapper.class);
        TransportWrapper mockTransportWrapper2 = mock(TransportWrapper.class);

        WebSocketSniffer webSocketSniffer = new WebSocketSniffer(mockTransportWrapper1, mockTransportWrapper2);

        assertEquals("Incorrect header size", WebSocketHeader.MIN_HEADER_LENGTH_MASKED, webSocketSniffer.bufferSize());

        byte[] bytes = new byte[WebSocketHeader.MIN_HEADER_LENGTH_MASKED - 1];
        bytes[0] = WebSocketHeader.FINAL_OPCODE_BINARY;

        webSocketSniffer.makeDetermination(bytes);
    }
}
