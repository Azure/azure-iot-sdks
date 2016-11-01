/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */
package com.microsoft.azure.iothub.ws;

import java.nio.ByteBuffer;
import java.util.Map;

public interface WebSocketHandler
{
    public enum WebSocketMessageType
    {
        WEB_SOCKET_MESSAGE_TYPE_UNKNOWN,
        WEB_SOCKET_MESSAGE_TYPE_CHUNK,
        WEB_SOCKET_MESSAGE_TYPE_HEADER_CHUNK,
        WEB_SOCKET_MESSAGE_TYPE_AMQP,
        WEB_SOCKET_MESSAGE_TYPE_PING,
        WEB_SOCKET_MESSAGE_TYPE_CLOSE,
    }

    String createUpgradeRequest(String hostName, String webSocketPath, int webSocketPort, String webSocketProtocol, Map<String, String> additionalHeaders);

    Boolean validateUpgradeReply(ByteBuffer buffer);

    void wrapBuffer(ByteBuffer srcBuffer, ByteBuffer dstBuffer);

    WebsocketTuple unwrapBuffer(ByteBuffer srcBuffer);

    void createPong(ByteBuffer srcBuffer, ByteBuffer dstBuffer);

    int calculateHeaderSize(int payloadSize);

    public class WebsocketTuple{

        private long length;
        private WebSocketMessageType type;

        public WebsocketTuple(long length, WebSocketMessageType type){
            this.length = length;
            this.type = type;
        }

        public void setLength(long length){
            this.length = length;
        }

        public void setType(WebSocketMessageType type){
            this.type = type;
        }

        public long getLength(){
            return this.length;
        }
        public WebSocketMessageType getType(){
            return this.type;
        }
    }
}