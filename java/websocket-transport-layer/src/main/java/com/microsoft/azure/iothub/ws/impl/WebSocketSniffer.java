/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */
package com.microsoft.azure.iothub.ws.impl;

import com.microsoft.azure.iothub.ws.WebSocketHeader;
import org.apache.qpid.proton.engine.impl.HandshakeSniffingTransportWrapper;
import org.apache.qpid.proton.engine.impl.TransportWrapper;

public class WebSocketSniffer extends HandshakeSniffingTransportWrapper<TransportWrapper, TransportWrapper>
{
    public WebSocketSniffer(TransportWrapper webSocket, TransportWrapper other)
    {
        super(webSocket, other);
    }

    protected TransportWrapper getSelectedTransportWrapper()
    {
        return _selectedTransportWrapper;
    }

    @Override
    protected int bufferSize()
    {
        return WebSocketHeader.MIN_HEADER_LENGTH_MASKED;
    }

    @Override
    protected void makeDetermination(byte[] bytes)
    {
        if (bytes.length < bufferSize())
        {
            throw new IllegalArgumentException("insufficient bytes");
        }

        if (bytes[0] != WebSocketHeader.FINAL_OPCODE_BINARY)
        {
            _selectedTransportWrapper = _wrapper2;
            return;
        }

        _selectedTransportWrapper = _wrapper1;
    }
}
