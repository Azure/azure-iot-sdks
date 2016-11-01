/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */
package com.microsoft.azure.iothub.ws.impl;

import com.microsoft.azure.iothub.ws.WebSocket;
import com.microsoft.azure.iothub.ws.WebSocketHandler;
import com.microsoft.azure.iothub.ws.WebSocketHeader;
import org.apache.qpid.proton.engine.Transport;
import org.apache.qpid.proton.engine.TransportException;
import org.apache.qpid.proton.engine.impl.*;

import static com.microsoft.azure.iothub.ws.WebSocket.WebSocketFrameReadState.*;
import static com.microsoft.azure.iothub.ws.WebSocket.WebSocketState.*;
import static com.microsoft.azure.iothub.ws.WebSocketHandler.WebSocketMessageType.*;
import static org.apache.qpid.proton.engine.impl.ByteBufferUtils.*;

import java.nio.ByteBuffer;

import java.util.Map;

public class WebSocketImpl implements WebSocket, TransportLayer
{
    private int _maxFrameSize = (4 * 1024) + (16 * WebSocketHeader.MED_HEADER_LENGTH_MASKED);
    private boolean _tail_closed = false;
    private final ByteBuffer _inputBuffer;
    private boolean _head_closed = false;
    private final ByteBuffer _outputBuffer;
    private ByteBuffer _pingBuffer;
    private ByteBuffer _wsInputBuffer;
    private ByteBuffer _temp;

    private int _underlyingOutputSize = 0;
    private int _webSocketHeaderSize = 0;

    private WebSocketHandler _webSocketHandler;
    private WebSocketState _state = PN_WS_NOT_STARTED;

    private String _host = "";
    private String _path = "";
    private int _port = 0;
    private String _protocol = "";
    private Map<String, String> _additionalHeaders = null;

    protected Boolean _isWebSocketEnabled = false;

    private WebSocketHandler.WebSocketMessageType _lastType;
    private long _lastLength;
    private long _bytesRead = 0;
    private int _dataStart = 0;
    private WebSocketFrameReadState _frameReadState = INIT_READ;

    public WebSocketImpl()
    {
        _inputBuffer = newWriteableBuffer(_maxFrameSize);
        _outputBuffer = newWriteableBuffer(_maxFrameSize);
        _pingBuffer = newWriteableBuffer(_maxFrameSize);
        _wsInputBuffer = newWriteableBuffer(_maxFrameSize);
        _temp = newWriteableBuffer(_maxFrameSize);
        _lastType = WEB_SOCKET_MESSAGE_TYPE_UNKNOWN;
        _lastLength = 0;
        _isWebSocketEnabled = false;
    }

    public TransportWrapper wrap(final TransportInput input, final TransportOutput output)
    {
        return new WebSocketSniffer(new WebSocketTransportWrapper(input, output), new PlainTransportWrapper(output, input))
        {
            protected boolean isDeterminationMade()
            {
                _selectedTransportWrapper = _wrapper1;
                return true;
            }
        };
    }

    @Override
    public void configure(String host, String path, int port, String protocol, Map<String, String> additionalHeaders, WebSocketHandler webSocketHandler)
    {
        _host = host;
        _path = path;
        _port = port;
        _protocol = protocol;
        _additionalHeaders = additionalHeaders;

        if (webSocketHandler != null)
        {
            _webSocketHandler = webSocketHandler;
        }
        else
        {
            _webSocketHandler = new WebSocketHandlerImpl();
        }

        _isWebSocketEnabled = true;
    }

    @Override
    public void wrapBuffer(ByteBuffer srcBuffer, ByteBuffer dstBuffer)
    {
        if (_isWebSocketEnabled)
        {
            _webSocketHandler.wrapBuffer(srcBuffer, dstBuffer);
        }
        else
        {
            dstBuffer.clear();
            dstBuffer.put(srcBuffer);
        }
    }

    @Override
    public WebSocketHandler.WebsocketTuple unwrapBuffer(ByteBuffer buffer)
    {
        if (_isWebSocketEnabled)
        {
            return _webSocketHandler.unwrapBuffer(buffer);
        }
        else
        {
            return new WebSocketHandler.WebsocketTuple(0, WEB_SOCKET_MESSAGE_TYPE_UNKNOWN);
        }
    }

    @Override
    public WebSocketState getState()
    {
        return _state;
    }

    @Override
    public ByteBuffer getOutputBuffer()
    {
        return _outputBuffer;
    }

    @Override
    public ByteBuffer getInputBuffer()
    {
        return _inputBuffer;
    }

    @Override
    public ByteBuffer getPingBuffer()
    {
        return _pingBuffer;
    }

    @Override
    public ByteBuffer getWsInputBuffer()
    {
        return _wsInputBuffer;
    }

    @Override
    public Boolean getEnabled()
    {
        return _isWebSocketEnabled;
    }

    @Override
    public WebSocketHandler getWebSocketHandler()
    {
        return _webSocketHandler;
    }

    @Override
    public String toString()
    {
        StringBuilder builder = new StringBuilder();
        builder.append(
                "WebSocketImpl [isWebSocketEnabled=").append(_isWebSocketEnabled)
                .append(", state=").append(_state)
                .append(", protocol=").append(_protocol)
                .append(", host=").append(_host)
                .append(", path=").append(_path)
                .append(", port=").append(_port);

        if ((_additionalHeaders != null) && (!_additionalHeaders.isEmpty()))
        {
            builder.append(", additionalHeaders=");

            for (Map.Entry<String, String> entry : _additionalHeaders.entrySet())
            {
                builder.append(entry.getKey() + ":" + entry.getValue()).append(", ");
            }

            int lastIndex = builder.lastIndexOf(", ");
            builder.delete(lastIndex, lastIndex + 2);
        }

        builder.append("]");

        return builder.toString();
    }

    protected void writeUpgradeRequest()
    {
        _outputBuffer.clear();
        String request = _webSocketHandler.createUpgradeRequest(_host, _path, _port, _protocol, _additionalHeaders);
        _outputBuffer.put(request.getBytes());
    }

    protected void writePong()
    {
        _webSocketHandler.createPong(_pingBuffer, _outputBuffer);
    }

    protected void writeClose()
    {
        _outputBuffer.clear();
        _pingBuffer.flip();
        _outputBuffer.put(_pingBuffer);
    }

    private class WebSocketTransportWrapper implements TransportWrapper
    {
        private final TransportInput _underlyingInput;
        private final TransportOutput _underlyingOutput;
        private final ByteBuffer _head;

        private WebSocketTransportWrapper(TransportInput input, TransportOutput output)
        {
            _underlyingInput = input;
            _underlyingOutput = output;
            _head = _outputBuffer.asReadOnlyBuffer();
            _head.limit(0);
        }

        private void readInputBuffer()
        {
            ByteBufferUtils.pour(_inputBuffer, _temp);
        }

        private boolean sendToUnderlyingInput()
        {
            boolean _readComplete =  false;
            switch (_lastType)
            {
                case WEB_SOCKET_MESSAGE_TYPE_UNKNOWN:
                    _wsInputBuffer.position(_wsInputBuffer.limit());
                    _wsInputBuffer.limit(_wsInputBuffer.capacity());
                    break;
                case WEB_SOCKET_MESSAGE_TYPE_CHUNK:
                    _wsInputBuffer.position(_wsInputBuffer.limit());
                    _wsInputBuffer.limit(_wsInputBuffer.capacity());
                    break;
                case WEB_SOCKET_MESSAGE_TYPE_AMQP:
                    _wsInputBuffer.flip();

                    int bytes2 = pourAll(_wsInputBuffer, _underlyingInput);
                    if (bytes2 == Transport.END_OF_STREAM)
                    {
                        _tail_closed = true;
                    }
                    //_underlyingInput.process();

                    _wsInputBuffer.compact();
                    _wsInputBuffer.flip();
                    _readComplete = true;
                    break;
                case WEB_SOCKET_MESSAGE_TYPE_CLOSE:
                    _wsInputBuffer.flip();
                    _pingBuffer.put(_wsInputBuffer);
                    _state = PN_WS_CONNECTED_CLOSING;

                    _wsInputBuffer.compact();
                    _wsInputBuffer.flip();
                    _readComplete = true;
                    break;
                case WEB_SOCKET_MESSAGE_TYPE_PING:
                    _wsInputBuffer.flip();
                    _pingBuffer.put(_wsInputBuffer);
                    _state = PN_WS_CONNECTED_PONG;

                    _wsInputBuffer.compact();
                    _wsInputBuffer.flip();
                    _readComplete = true;
                    break;
            }
            _wsInputBuffer.position(_wsInputBuffer.limit());
            _wsInputBuffer.limit(_wsInputBuffer.capacity());
            return _readComplete;
        }

        private void processInput() throws TransportException
        {
            switch (_state)
            {
                case PN_WS_CONNECTING:
                    if (_webSocketHandler.validateUpgradeReply(_inputBuffer))
                    {
                        _state = WebSocketState.PN_WS_CONNECTED_FLOW;
                    }
                    _inputBuffer.compact();
                    break;
                case PN_WS_CONNECTED_FLOW:
                case PN_WS_CONNECTED_PONG:

                    if (_inputBuffer.remaining() > 0)
                    {
                        boolean _readComplete = false;
                        while(!_readComplete) {
                            switch (_frameReadState)
                            {
                                //State 1: Init_Read
                                case INIT_READ:
                                    //Reset the bytes read count
                                    _bytesRead = 0;
                                    //Determine how much to grab from the input buffer and only take that
                                    readInputBuffer();

                                    _frameReadState = _temp.position() < 2 ? CHUNK_READ : HEADER_READ;
                                    _readComplete = _frameReadState == CHUNK_READ;
                                    break;

                                //State 2: Chunk_Read
                                case CHUNK_READ:
                                    //Determine how much to grab from the input buffer and only take that
                                    readInputBuffer();

                                    _frameReadState = _temp.position() < 2 ? _frameReadState : HEADER_READ;
                                    _readComplete = _frameReadState == CHUNK_READ;
                                    break;

                                //State 3: Header_Read
                                case HEADER_READ:
                                    //Determine how much to grab from the input buffer and only take that
                                    readInputBuffer();

                                    _temp.flip();
                                    WebSocketHandler.WebsocketTuple unwrapResult = unwrapBuffer(_temp);
                                    _lastType = unwrapResult.getType();
                                    _lastLength = unwrapResult.getLength();

                                    _frameReadState = _lastType == WEB_SOCKET_MESSAGE_TYPE_HEADER_CHUNK ? CHUNK_READ : CONTINUED_FRAME_READ;
                                    _readComplete = _frameReadState == CHUNK_READ || _temp.position() == _temp.limit();

                                    if(_frameReadState == CONTINUED_FRAME_READ)
                                    {
                                        _temp.compact();
                                    }
                                    else
                                    {
                                        //Unflip the buffer to continue writing to it
                                        _temp.position(_temp.limit());
                                        _temp.limit(_temp.capacity());
                                    }

                                    break;

                                //State 4: Continued_Frame_Read (Similar to Chunk_Read but reading until we've read the number of bytes specified when unwrapping the buffer)
                                case CONTINUED_FRAME_READ:
                                    //Determine how much to grab from the input buffer and only take that
                                    readInputBuffer();
                                    _temp.flip();

                                    final byte[] data;
                                    if(_temp.remaining() >= _lastLength-_bytesRead)
                                    {
                                        data = new byte[(int)(_lastLength-_bytesRead)];
                                        _temp.get(data, 0, (int)(_lastLength-_bytesRead));
                                        _wsInputBuffer.put(data);
                                        _bytesRead += _lastLength-_bytesRead;
                                    }
                                    //Otherwise the remaining bytes is < the rest that we need
                                    else
                                    {
                                        data = new byte[_temp.remaining()];
                                        _temp.get(data);
                                        _wsInputBuffer.put(data);
                                        _bytesRead += data.length;
                                    }

                                    //Send whatever we have
                                    sendToUnderlyingInput();

                                    _frameReadState = _bytesRead == _lastLength ? INIT_READ : CONTINUED_FRAME_READ;
                                    _readComplete = _temp.remaining() == 0;
                                    _temp.compact();
                                    break;

                                //State 5: Read_Error
                                case READ_ERROR:
                                    break;
                            }
                        }
                    }
                    _inputBuffer.compact();
                    break;
                case PN_WS_NOT_STARTED:
                case PN_WS_CLOSED:
                case PN_WS_FAILED:
                default:
                    break;
            }
        }

        @Override
        public int capacity()
        {
            if (_isWebSocketEnabled)
            {
                if (_tail_closed)
                {
                    return Transport.END_OF_STREAM;
                }
                else
                {
                    return _inputBuffer.remaining();
                }
            }
            else
            {
                return _underlyingInput.capacity();
            }
        }

        @Override
        public int position()
        {
            if (_isWebSocketEnabled)
            {
                if (_tail_closed)
                {
                    return Transport.END_OF_STREAM;
                }
                else
                {
                    return _inputBuffer.position();
                }
            }
            else
            {
                return _underlyingInput.position();
            }
        }

        @Override
        public ByteBuffer tail()
        {
            if (_isWebSocketEnabled)
            {
                return _inputBuffer;
            }
            else
            {
                return _underlyingInput.tail();
            }
        }

        @Override
        public void process() throws TransportException
        {
            if (_isWebSocketEnabled)
            {
                _inputBuffer.flip();

                switch (_state)
                {
                    case PN_WS_CONNECTING:
                    case PN_WS_CONNECTED_FLOW:
                        processInput();
                        break;
                    case PN_WS_NOT_STARTED:
                    case PN_WS_FAILED:
                    default:
                        _underlyingInput.process();
                }
            }
            else
            {
                _underlyingInput.process();
            }
        }

        @Override
        public void close_tail()
        {
            _tail_closed = true;
            if (_isWebSocketEnabled)
            {
                _head_closed = true;
                _underlyingInput.close_tail();
            }
            else
            {
                _underlyingInput.close_tail();
            }
        }

        @Override
        public int pending()
        {
            if (_isWebSocketEnabled)
            {
                switch (_state)
                {
                    case PN_WS_NOT_STARTED:
                        if (_outputBuffer.position() == 0)
                        {
                            _state = WebSocketState.PN_WS_CONNECTING;

                            writeUpgradeRequest();

                            _head.limit(_outputBuffer.position());

                            if (_head_closed)
                            {
                                _state = PN_WS_FAILED;
                                return Transport.END_OF_STREAM;
                            }
                            else
                            {
                                return _outputBuffer.position();
                            }
                        }
                        else
                        {
                            return _outputBuffer.position();
                        }
                    case PN_WS_CONNECTING:

                        if (_head_closed && (_outputBuffer.position() == 0))
                        {
                            _state = PN_WS_FAILED;
                            return Transport.END_OF_STREAM;
                        }
                        else
                        {
                            return _outputBuffer.position();
                        }
                    case PN_WS_CONNECTED_FLOW:
                        _underlyingOutputSize = _underlyingOutput.pending();

                        if (_underlyingOutputSize > 0)
                        {
                            _webSocketHeaderSize = _webSocketHandler.calculateHeaderSize(_underlyingOutputSize);
                            return _underlyingOutputSize + _webSocketHeaderSize;
                        }
                        else
                        {
                            return _underlyingOutputSize;
                        }
                    case PN_WS_CONNECTED_PONG:
                        _state = WebSocketState.PN_WS_CONNECTED_FLOW;

                        writePong();

                        _head.limit(_outputBuffer.position());

                        if (_head_closed)
                        {
                            _state = PN_WS_FAILED;
                            return Transport.END_OF_STREAM;
                        }
                        else
                        {
                            return _outputBuffer.position();
                        }
                    case PN_WS_CONNECTED_CLOSING:
                        _state = PN_WS_CLOSED;

                        writeClose();

                        _head.limit(_outputBuffer.position());

                        if (_head_closed)
                        {
                            _state = PN_WS_FAILED;
                            return Transport.END_OF_STREAM;
                        }
                        else
                        {
                            return _outputBuffer.position();
                        }
                    case PN_WS_FAILED:
                    default:
                        return Transport.END_OF_STREAM;
                }
            }
            else
            {
                return _underlyingOutput.pending();
            }
        }

        @Override
        public ByteBuffer head()
        {
            if (_isWebSocketEnabled)
            {
                switch (_state)
                {
                    case PN_WS_CONNECTING:
                    case PN_WS_CONNECTED_PONG:
                    case PN_WS_CONNECTED_CLOSING:
                        return _head;
                    case PN_WS_CONNECTED_FLOW:
                        _underlyingOutputSize = _underlyingOutput.pending();

                        if (_underlyingOutputSize > 0)
                        {
                            wrapBuffer(_underlyingOutput.head(), _outputBuffer);

                            _webSocketHeaderSize = _outputBuffer.position() - _underlyingOutputSize;

                            _head.limit(_outputBuffer.position());
                        }

                        return _head;
                    case PN_WS_NOT_STARTED:
                    case PN_WS_CLOSED:
                    case PN_WS_FAILED:
                    default:
                        return _underlyingOutput.head();
                }

            }
            else
            {
                return _underlyingOutput.head();
            }
        }

        @Override
        public void pop(int bytes)
        {
            if (_isWebSocketEnabled)
            {
                switch (_state)
                {
                    case PN_WS_CONNECTING:
                        if (_outputBuffer.position() != 0)
                        {
                            _outputBuffer.flip();
                            _outputBuffer.position(bytes);
                            _outputBuffer.compact();
                            _head.position(0);
                            _head.limit(_outputBuffer.position());
                        }
                        else
                        {
                            _underlyingOutput.pop(bytes);
                        }
                        break;
                    case PN_WS_CONNECTED_FLOW:
                    case PN_WS_CONNECTED_PONG:
                    case PN_WS_CONNECTED_CLOSING:
                        if ((bytes >= _webSocketHeaderSize) && (_outputBuffer.position() != 0))
                        {
                            _outputBuffer.flip();
                            _outputBuffer.position(bytes);
                            _outputBuffer.compact();
                            _head.position(0);
                            _head.limit(_outputBuffer.position());
                            _underlyingOutput.pop(bytes - _webSocketHeaderSize);
                            _webSocketHeaderSize = 0;
                        }
                        else if ((bytes > 0) && (bytes < _webSocketHeaderSize))
                        {
                            _webSocketHeaderSize -= bytes;
                        }
                        else
                        {
                            _underlyingOutput.pop(bytes);
                        }
                        break;
                    case PN_WS_NOT_STARTED:
                    case PN_WS_CLOSED:
                    case PN_WS_FAILED:
                        _underlyingOutput.pop(bytes);
                        break;
                }
            }
            else
            {
                _underlyingOutput.pop(bytes);
            }
        }

        @Override
        public void close_head()
        {
            _underlyingOutput.close_head();
        }

        public final char[] HEX_DIGITS = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

        private String convertToHex(byte[] bb)
        {
            final int lgt = bb.length;

            final char[] out = new char[5*lgt];
            for( int i=0,j=0; i<lgt; i++ ){
                out[j++] = '0';
                out[j++] = 'x';
                out[j++] = HEX_DIGITS[(0xF0 & bb[i]) >>> 4];
                out[j++] = HEX_DIGITS[0x0F & bb[i]];
                out[j++] = '|';
            }
            return new String(out);
        }

        private String convertToHex(ByteBuffer bb)
        {
            final byte[] data = new byte[bb.remaining()];
            bb.duplicate().get(data);

            return convertToHex(data);
        }

        private String convertToBinary(byte[] bb)
        {
            StringBuilder sb = new StringBuilder();

            for(byte b : bb)
            {
                sb.append(String.format("%8s", Integer.toBinaryString(b & 0xFF)).replace(' ', '0'));
                sb.append('|');
            }

            return sb.toString();
        }

        private String convertToBinary(ByteBuffer bb)
        {
            final byte[] data = new byte[bb.remaining()];
            bb.duplicate().get(data);

            return convertToBinary(data);
        }
    }
}