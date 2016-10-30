/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */
package com.microsoft.azure.iothub.ws;

import java.nio.ByteBuffer;
import java.util.Map;

/*
 * Provides interface for WebSocket
 */
public interface WebSocket
{
    public enum WebSocketState
    {
        /**
         * WebSocket
         */
        PN_WS_NOT_STARTED,
        /**
         * Pending connection
         */
        PN_WS_CONNECTING,
        /**
         * Connected and messages flow
         */
        PN_WS_CONNECTED_FLOW,
        /**
         * Connected and ping-pong
         */
        PN_WS_CONNECTED_PONG,
        /**
         * Connected and received a close
         */
        PN_WS_CONNECTED_CLOSING,
        /**
         * Connection closed
         */
        PN_WS_CLOSED,
        /**
         * Connection failed
         */
        PN_WS_FAILED
    }

    public enum WebSocketFrameReadState
    {
        /**
         * The initial read
         */
        INIT_READ,

        /**
         * Reading chunks of bytes until a full header is read
         */
        CHUNK_READ,

        /**
         * Continue reading bytes until correct number of bytes are read
         */
        CONTINUED_FRAME_READ,

        /**
         * Full header has been read
         */
        HEADER_READ,

        /**
         * An error reading
         */
        READ_ERROR
    }

    /**
     * Configure WebSocket connection
     */
    void configure(String host, String path, int port, String protocol, Map<String, String> additionalHeaders, WebSocketHandler webSocketHandler);

    /**
     * Add WebSocket frame to send the given buffer
     */
    void wrapBuffer(ByteBuffer srcBuffer, ByteBuffer dstBuffer);

    /**
     * Remove WebSocket frame from the given buffer
     *
     * @return The payload of the given WebSocket frame.
     */
    WebSocketHandler.WebsocketTuple unwrapBuffer(ByteBuffer buffer);

    /**
     * Access the handler for WebSocket functions.
     *
     * @return The WebSocket handler class.
     */
    WebSocketHandler getWebSocketHandler();

    /**
     * Access the current state of the layer.
     *
     * @return The state of the WebSocket layer.
     */
    WebSocketState getState();

    /**
     * Access if WebSocket enabled .
     *
     * @return True if WebSocket enabled otherwise false.
     */
    Boolean getEnabled();

    /**
     * Access the output buffer (read only).
     *
     * @return The current output buffer.
     */
    ByteBuffer getOutputBuffer();

    /**
     * Access the input buffer (read only).
     *
     * @return The current input buffer.
     */
    ByteBuffer getInputBuffer();

    /**
     * Access the ping buffer (read only).
     *
     * @return The ping input buffer.
     */
    ByteBuffer getPingBuffer();

    /**
     * Access the websocket input buffer (read only).
     *
     * @return The wsInputBuffer input buffer.
     */
    ByteBuffer getWsInputBuffer();
}